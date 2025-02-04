/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * Original Author      : Canh Le (Deceased)
 * Current Maintainer   : Georges Sancosme (georges@sancosme.net)
 * Maintained Since     : 13.01.2025
 * Created On           : 04.06.2007
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Program:     Controller firmware
 * File:        sctim.c
 * Functions:   TIEnableTimer
 *              TIActivateWD
 *              TIDisableTimer
 *              TIResetComputer
 *              TISetCounter
 *              TIReadCounter
 *              TICountExpired
 *              TIDelay
 *              TIRefreshWD
 *              TIGetCounter
 *              TIReturnCounter
 *              TITimerPROC
 *              TIOldTimerPROC
 *
 * Description: Handles all timer related operations
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 * NOTE: The enum was removed because the new timer module acts like a
 *      library for counter numbers. They must checked out to be used
 *      and checked back in after use is complete. So there are no
 *      more hard coded timer numbers. The Motion module permanently
 *      checks out 2 counters. The Comm module permanently checks out 3
 *      counters. That leaves 4 more for general use.
 *
 * NOTE: The software watchdog timer was created to substitute the hardware
 *      watchdog timer. It's essence is the iChangeLEDStat variable that is a
 *      counter that is automatically decremented by the timer PROC and
 *      reset to CHANGE_LED_ON or CHANGE_LED_OFF by the TIRefreshWD function.
 *      This Refresh function must be called frequently by the
 *      main loop or a subsidiary function, otherwise after a 10 sec interval
 *      the iContLockedUp variable is set to 123 which, through the Macro module,
 *      restricts access to a number of low-level commands. This means that
 *      the firmware was stuck in a loop somewhere and was unable to
 *      refresh the watchdog timer through a 10 sec interval. The timer
 *      PROC also does a jump back to the top of the main loop when the
 *      watchdog timer is tripped thus breaking out of the trapped loop
 *      and effectively resetting the controller.
 *
 * NOTE: Does NOT take any care about a ulSysTimer overflow.
 *      Many variables would be affected in that case.
 *
 * NOTE: The ulSysTimer variable is a 32-bit variable that changes without
 *      warning in the timer PROC. If it is being accessed by another routine
 *      at exactly the same time that an interrupt occurs, a half-old/half-new
 *      variable may register. So it must be protected by PIC masking. Thus
 *      there are a number of functions that simply mask the PIC, copy a variable
 *      (that may change in the PROC), and unmask the PIC.
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 ****************************************************************/

#include <malloc.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/io.h>
#include "sck.h"
#include "sctim.h"
#include "sctim2.h"
#include "sctim3.h"
#include "scintr.h"
#include "ser.h"
#include "gag.h"
#include "scio.h"
#include "ro.h"
#include "fiog.h"
#include "scmem.h"

int     ulTimerInterval = 0;

int	giDispTime = 0;

struct sigaction gSigAction;
struct itimerval gSysTimer;

//void timer_handler(int signum)
//{
//  ++(*ulSysTimer);
//  if (*ulSysTimer % 100 == 0)  printf("%llu\n\r", *ulSysTimer);
//}

#define MAXLONGVAL 2147383600

unsigned long glTimerMsec = 0;
unsigned long glTimerMday = 0;
unsigned long glTarMsec[10] = {0,0,0,0,0,0,0,0,0,0};
unsigned long glTarMday[10] = {0,0,0,0,0,0,0,0,0,0};
int  giTimerUsed[10] = {0,0,0,0,0,0,0,0,0,0};
long glExpireCount = 0;

pthread_t threadTimer;

void *procTimer(void *ptr)
{
    // int i;
    struct timespec ntv;

    ntv.tv_sec = 0;
    ntv.tv_nsec = 1000;

    while(1)
    {
	if(++glTimerMsec >= MAXLONGVAL)
	{
	    ++glTimerMday;
	    glTimerMsec = 0;
    	}
	nanosleep(&ntv, NULL);
    }
}

void MainTimerHandler(int signum)
{
    if(++glTimerMsec >= MAXLONGVAL)
    {
	++glTimerMday;
	glTimerMsec = 0;
    }
}

int MainTimerExpired(int iTimerArg)
{
    if(iTimerArg < 0 || iTimerArg >= 10) return 1;
//    if(++glExpireCount <= 35) return 0;
    glExpireCount = 0;

    if(glTimerMday < glTarMday[iTimerArg]) return 0;
    if(glTimerMsec < glTarMsec[iTimerArg]) return 0;
    return 1;
}    

int MainTimerSet(int iTimerArg, long lMsecArg)
{
    if(iTimerArg < 0 || iTimerArg >= 10) return FAILURE;

    glTarMsec[iTimerArg] = glTimerMsec + lMsecArg;
    glTarMday[iTimerArg] = glTimerMday;

    if (glTarMsec[iTimerArg] >= MAXLONGVAL)
    {
	++glTarMday[iTimerArg];
    	glTarMsec[iTimerArg] -= MAXLONGVAL;
    }

    return SUCCESS;
}

int MainReadTimer(int iTimerArg, long *lTimerValArg)
{
    // need to check for Mday overrun
    if(iTimerArg < 0 || iTimerArg >= 10) return FAILURE;

    *lTimerValArg = (glTarMday[iTimerArg] - glTimerMday) + glTarMsec[iTimerArg] - glTimerMsec;
    return SUCCESS;
}

int MainTimerDelay(long lMsecArg)
{
    int iTimerIndex;

    /* Check out a timer. */
    if ((iTimerIndex = TIGetCounter()) == FAILURE)
        return FAILURE;

    /* Set it. */
    if (MainTimerSet(iTimerIndex, lMsecArg) == FAILURE)
        return FAILURE;

    /* Wait until it times out refreshing the watchdog timer while you wait. */
    while (!MainTimerExpired(iTimerIndex))
    {

    }

    /* Check the timer back in. */
    if (TIReturnCounter(iTimerIndex) == FAILURE)
        return FAILURE;

    return SUCCESS;
    
}



/****************************************************************
 *
 * Function:    TIEnableTimer
 *
 * Abstract:    Enables timer PROC.  If ChainTimerInt is true,
 *      then the PROC will chain in front of normal timer int and interval
 *      will be 55 ms. -- this is useful for testing, since changing the
 *      timer interval will disrupt the time of day clock and the floppy
 *      disk drive motor controller.  If ChainTimerInt is set to false,
 *      then the system will change the timer interval to 10 ms which
 *      is the interval used in the production system. It also
 *      allocates memory for the ulSysTimer variable as well as
 *      saving the old interrupt vector.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: enable_timer
 * Used in: main
 *
 ***************************************************************/
int TIEnableTimer()
{
    int iCmdPort;

    /* Initialize variable */
    iCmdPort = SERGetCmdPort();
    /* If the timer was already initialized, don't re-initialize it. */
    if (iTimerEnabled)
        return FAILURE;

    /* Disable all interrupts globally before setting up variables
     * that will be used in the timer PROC. */

    /* See the function header for more details on this operation. */
//    if (!iChainTimerInt)
//    {
//        outp(0x43, 0x36);
//        outp(0x40, 0x96);
//        outp(0x40, 0x2e);
//    }

    /* Allocate memory for the important variables. */
    ulSysTimer = (long long unsigned *)malloc(sizeof(long long unsigned));
    if(!ulSysTimer)
    {
        return FAILURE;
    }
    ulSysTimerOverflow = (long long unsigned *)malloc(sizeof(long long unsigned));
    if(!ulSysTimerOverflow)
    {
        return FAILURE;
    }

    /* Initialize their values. */
    *ulSysTimer = 0;
    *ulSysTimerOverflow = 0;



    /* Install timer handler as the signal handler */
    memset(&gSigAction, 0, sizeof(gSigAction));
    gSigAction.sa_handler = &MainTimerHandler;
    sigaction (SIGALRM, &gSigAction, NULL);

    // configure the timer to expire after 1 msec
    gSysTimer.it_value.tv_sec = 0;
    gSysTimer.it_value.tv_usec = 10000;
    // and every 1 msec after that
    gSysTimer.it_interval.tv_sec = 0;
    gSysTimer.it_interval.tv_usec = 10000;
    // start a REAL timer. it counts down
//    setitimer(ITIMER_REAL, &gSysTimer, NULL);


    /* Indicate that the timer module was initialized. This variable will be
     * checked by a number of functions that are dependent on the timer PROC
     * being installed correctly, including this one. */
    iTimerEnabled = TRUE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TIDisableTimer
 *
 * Abstract:    Disables timer PROC by restoring the old interrupt
 *      vector and resetting the timer chaining feature if it was
 *      used. It also disables system interrupts globally before
 *      restoring old settings.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: disable_timer
 * Used in: execute2
 *
 ***************************************************************/
int TIDisableTimer()
{
    /* If the timer was not initialized, this function cannot be performed.
     * This check is performed in a number of functions that are dependent on the
     * timer PROC being installed correctly. */
    if (!iTimerEnabled) return FAILURE;



    /* Indicate that the timer module was closed. This variable will be
     * checked by a number of functions that are dependent on the timer PROC
     * being installed correctly, including this one. Those functions
     * will all fail after this point unless TIEnableTimer is called again. */
    iTimerEnabled = FALSE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TIActivateWD
 *
 * Abstract:    Enables the software watchdog timer. See the note
 *      in the header comments for more details.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: activate_wd
 * Used in: main
 *
 ***************************************************************/
int TIActivateWD()
{
    /* If the watchdog timer was already initialized, don't re-initialize it. */
    if (iWDTimerActive)
        return FAILURE;

    /* Indicate that the watchdog timer was initialized. This variable will be
     * checked by a number of watchdog related functions that need it to be
     * installed correctly and only once, including this one. */
    iWDTimerActive = TRUE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TIResetComputer
 *
 * Abstract:    Causes a reboot (may be called under interrupt from watchdog timer)
 *      The reboot occurs by jumping to the reboot address at F000:E05B.
 *      Since it is not possible to jump to an absolute address in C,
 *      we form a function pointer and then call the reboot address --
 *      obviously there is no return to this call. That's why there are no
 *      checks for a FAILURE return code from any functions called.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: reset_computer
 * Used in: execute2
 *
 ***************************************************************/
void TIResetComputer()
{
    char sBuf[30] = "shutdown +0 -r"; // linux shutdown zero wait and restart

    /* FAR_FUNC_PTR is a typedef for a pointer to a generic far function. */
//    typedef int (*FAR_FUNC_PTR)(void);

//    FAR_FUNC_PTR RebootFuncPtr;
//    int  *iBootFlag;

    /* Activate the brake. */
//   ROServiceBrake(ROBOTFILE, FALSE);
    /* Send it to both cards if there are 2 in the system. */
//    if (ROGetSpecialAxis(ME_INDEXER_Z1) || ROGetSpecialAxis(ME_INDEXER_Z2))
//        ROServiceBrake(PREALIGNFILE, FALSE);

	/* if there is track : m.z.*/
//	if(ROGetSpecialAxis(RO_TRACK))
//		ROServiceBrake(AUXFILE, FALSE);

    /* Wait for brakes to engage. */
    TIDelay(60);

    /* Close all initialized devices, including this one, as appropriate. */
//    SERClosePorts(COM2);
//    SERClosePorts(COM3);
//    SERClosePorts(COM4);
//    SERClosePorts(COM1);
//    TIDisableTimer();

    system(sBuf);

    /* Set flag address and value for hard reset. */
//    iBootFlag = (int  *) 0x00000072L;
//    *iBootFlag = 0;

    /* Jump to reboot address. */
//    RebootFuncPtr = (FAR_FUNC_PTR) 0xF000E05BL;
//    (*RebootFuncPtr)();
                    /* Beyond the point of no return! */
}


/****************************************************************
 *
 * Function:    TIGetCounter
 *
 * Abstract:    Gets the number of the next available counter.
 *      The new timer module acts like a
 *      library for counter numbers. They must checked out to be used
 *      and checked back in after use is complete. So there are no
 *      more hard coded timer numbers. The Motion module permanently
 *      checks out 2 counters. The Comm module permanently checks out 3
 *      counters. That leaves 4 more for general use.
 *
 * Parameters:  None
 *
 * Returns:     The next available timer index
 *
 ***************************************************************/
int TIGetCounter()
{
    int iTimerIndex;

    if (!iTimerEnabled)
        return FAILURE;

    /* Find the next available timer... */
    for (iTimerIndex=0; iTimerIndex<MAX_TIMER; iTimerIndex++)
    {
        if (!iTimerUsed[iTimerIndex])
        {
            /* ...and assign it as used. */
            iTimerUsed[iTimerIndex] = TRUE;
            /* Don't forget to give it back to the user so he can use it. */
            return iTimerIndex;
        }
    }

    return FAILURE;
}


/****************************************************************
 *
 * Function:    TIReturnCounter
 *
 * Abstract:    Returns a timer index so it can be used by another
 *      module. See TIGetCounter for more details.
 *
 * Parameters:
 *      uWhichTimerArg  (in) The timer number to return
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int TIReturnCounter(unsigned uWhichTimerArg)
{
    if (!iTimerEnabled)
        return FAILURE;

    /* Check for an invalid timer number. Since the timer number is
     * passed in as an unsigned, it cannot be negative. So we don't
     * need to check that it is not less than 0. This assumption
     * is made for a number of other functions that check the range
     * of a timer number. */
    if (uWhichTimerArg >= MAX_TIMER)
        return FAILURE;

    /* Verify that the timer number was checked out. This is also checked
     * in a number of other functions where a timer number is passed in. */
    if (!iTimerUsed[uWhichTimerArg])
        return FAILURE;

    /* Return it and make it available for use by another module. */
    iTimerUsed[uWhichTimerArg] = FALSE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TISetCounter
 *
 * Abstract:    Sets the number of timer ticks before a timeout
 *      is registered for a checked out counter number
 *      by adding the requested number of timer-ticks-
 *      to-a-timeout to the current value of the system timer.
 *      REMEMBER: Timer ticks occur in 10 ms increments.
 *
 * Parameters:
 *      uWhichTimerArg  (in) The timer number to setup
 *      uTimeoutArg     (in) The number of ticks before a timeout
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: set_counter, cset_counter
 * Used in: EVERYWHERE, mydelya1
 *
 ***************************************************************/
int TISetCounter(unsigned uWhichTimerArg, unsigned uTimeoutArg)
{
//    struct timeval tv;
//    struct timezone tz;
    struct timespec tv;
    unsigned long long ur, uSec, ulMsec;

//    return MainTimerSet((int)uWhichTimerArg, (long)uTimeoutArg);

    // Original func 

    if (!iTimerEnabled)
        return FAILURE;

    if (uWhichTimerArg >= MAX_TIMER)
        return FAILURE;

    if (!iTimerUsed[uWhichTimerArg])
        return FAILURE;

//    gettimeofday(&tv, &tz);
//    clock_gettime(CLOCK_REALTIME, &tv);
    clock_gettime(CLOCK_MONOTONIC, &tv);
if(giDispTime)
printf("TISetCounter cur time: %d:%d\n",(int)tv.tv_sec, (int)tv.tv_nsec);
//  convert nsec to msec
    ulMsec = tv.tv_nsec/1000 + 1000*uTimeoutArg;
    uSec = ulMsec / 1000000;
    ur = ulMsec - (1000000 * uSec);
//printf("sec=%d usec=%d tiemout=%d ulMsec=%d uSec=%d ur=%d\n",tv.tv_sec, tv.tv_usec, uTimeoutArg, ulMsec, uSec, ur);
    tv.tv_sec += uSec;
    tv.tv_nsec = 1000 * ur;
//printf("sec=%d usec=%d tiemout=%d ulMsec=%d uSec=%d ur=%d\n",tv.tv_sec, tv.tv_usec, uTimeoutArg, ulMsec, uSec, ur);
    ulTimerCounter[uWhichTimerArg][0] = tv.tv_sec;
    ulTimerCounter[uWhichTimerArg][1] = tv.tv_nsec;
if(giDispTime)
printf("TISetCounter exp time: %d:%d\n",(int)tv.tv_sec, (int)tv.tv_nsec);

    /* Set the timeout value. See the function header comments for more details. */
//    ulTimerCounter[uWhichTimerArg] = TIGetTimerVals(SYS_TIMER)+(long long unsigned)uTimeoutArg;
    iTimerActive[uWhichTimerArg] = TRUE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TIReadCounter
 *
 * Abstract:    Gets the current number of elapsed timer ticks
 *      to a timeout. It subtracts the current system timer
 *      value from the requested timeout value thus returning the number
 *      of timer ticks elapsed since a timeout value was set
 *      by TISetCounter.
 *
 * Parameters:
 *      uWhichTimerArg  (in) The timer number to read
 *      uTimerValArg    (out) The current value of the timer
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: read_counter
 * Used in: bal
 *
 ***************************************************************/
int TIReadCounter(unsigned uWhichTimerArg, unsigned *uTimerValArg)
{
//    struct timeval tv;
//    struct timezone tz;
    struct timespec tv;

    unsigned long long uSec, ulMsec;

//    return MainReadTimer((int) uWhichTimerArg, (long *) uTimerValArg);


    // Original code

    if (!iTimerEnabled)
        return FAILURE;

    if (uWhichTimerArg >= MAX_TIMER)
        return FAILURE;

    if (!iTimerUsed[uWhichTimerArg])
        return FAILURE;

    /* Check that the counter was set.
     * Otherwise there can be no differences returned. */
    if (!iTimerActive[uWhichTimerArg])
        return FAILURE;

//    gettimeofday(&tv, &tz);

    clock_gettime(CLOCK_MONOTONIC, &tv);

    uSec =  tv.tv_sec - ulTimerCounter[uWhichTimerArg][0];
    ulMsec = tv.tv_nsec - ulTimerCounter[uWhichTimerArg][1];

    /* Return the difference between the current time and
     * the timeout time. See the function header comments for more details. */
//    *uTimerValArg = (unsigned)(ulTimerCounter[uWhichTimerArg]-TIGetTimerVals(SYS_TIMER));

    *uTimerValArg = (1000000000*uSec + ulMsec)/1000000000;


    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TICountExpired
 *
 * Abstract:    Gets a TRUE or FALSE value as to whether or not
 *      the requested timeout period has elapsed. This is indicated
 *      when the system timer exceeds the timeout value set by
 *      TISetCounter.
 *
 * Parameters:
 *      uWhichTimerArg  (in) The timer number to check
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: count_expired, ccount_expired
 * Used in: EVERYWHERE, mydelya1
 *
 ***************************************************************/
int TICountExpired(unsigned uWhichTimerArg)
{
//    struct timeval tv;
//    struct timezone tz;
//    int uSec, ulMsec;
    struct timespec tv;


//    uSec = ulTimerCounter[uWhichTimerArg][0] / 1000;
//    ulMSec = ulTimerCounter[uWhichTimerArg][1] % 1000;
//    gettimeofday(&tv, &tz);

//    return MainTimerExpired((int) uWhichTimerArg);

    // Original code

    if (!iTimerEnabled)
        return FAILURE;

    if (uWhichTimerArg >= MAX_TIMER)
        return FAILURE;

    if (!iTimerUsed[uWhichTimerArg])
        return FAILURE;

    /* Check that the counter was set.
     * Otherwise there can be no timer expiration. */
    if (!iTimerActive[uWhichTimerArg])
        return FAILURE;

    /* Refresh the watchdog timer. */
    //if (TIRefreshWD() == FAILURE)
    //    return FAILURE;
//    //printf("tCounter=%llu timer=%u sysTimer=%llu\n\r", ulTimerCounter[uWhichTimerArg], uWhichTimerArg, TIGetTimerVals(SYS_TIMER));

    /* Disable the timer. This timer has timed out and needs to be reset. */
//if (uWhichTimerArg == 137)
////printf("timercount=%d systimer=%d, timer=%d\n",ulTimerCounter[uWhichTimerArg], TIGetTimerVals(SYS_TIMER), uWhichTimerArg);

//    uSec = ulTimerCounter[uWhichTimerArg][0] / 1000;
//    ulMSec = ulTimerCounter[uWhichTimerArg][1] % 1000;
//    gettimeofday(&tv, &tz);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    if (tv.tv_sec >= ulTimerCounter[uWhichTimerArg][0] && tv.tv_nsec >= ulTimerCounter[uWhichTimerArg][1])
    {
if(giDispTime)
printf("TICountExpire time: %d:%d\n",(int)tv.tv_sec, (int)tv.tv_nsec);
        iTimerActive[uWhichTimerArg] = FALSE;
        /* Indicate to the calling function that a timeout occurred. */
        return TRUE;
    }

    return FALSE;
}


/****************************************************************
 *
 * Function:    TIDelay
 *
 * Abstract:    Gets a TRUE or FALSE value as to whether or not
 *      the requested timeout period has elapsed during which time
 *      it pauses for the specified number of milliseconds.
 *      This doesn't make much sense because it waits for timer ticks
 *      to elapse. But the timer ticks off 10ms increments, not 1ms.
 *
 * NOTE: This function locks the processor while running.
 *      Other functions can't execute while this function is running.
 *
 * NOTE: This used to be either the function delay_a5 or delay_a1
 *      (which were part of the ASYNCH Manager) or mydelya1 in the
 *      old code. Those functions varied by whether or not an actual
 *      time in ms was passed in or the number of timer ticks, in 10ms
 *      intervals, was passed in. They also didn't refresh the watchdog
 *      timer. All those issues were fixed in this one function.
 *
 * Parameters:
 *      uTimeoutArg     (in) The number of milliseconds to wait
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: delay_a5, short_wait
 * Used in: EVERYWHERE, isr
 *
 ***************************************************************/
int TIDelay(unsigned uTimeoutArg)
{
    int iTimerIndex;

//    return MainTimerDelay((int) uTimeoutArg);


    // Original code

    /* Check out a timer. */
    if ((iTimerIndex = TIGetCounter()) == FAILURE)
        return FAILURE;

    /* Set it. */
    if (TISetCounter(iTimerIndex, uTimeoutArg) == FAILURE)
        return FAILURE;
    /* Wait until it times out refreshing the watchdog timer while you wait. */
    while (!TICountExpired(iTimerIndex))
    {
//        if (TIRefreshWD() == FAILURE)
//            return FAILURE;
    }

    /* Check the timer back in. */
    if (TIReturnCounter(iTimerIndex) == FAILURE)
        return FAILURE;
//    usleep (1000*uTimeoutArg);	// mSec --> uSec
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TIRefreshWD
 *
 * Abstract:    Resets the watchdog timer. See header
 *      comments for more details.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: refresh_wd
 * Used in: EVERYWHERE
 *
 ***************************************************************/
int TIRefreshWD()
{
    int iRet;

    if (!iWDTimerActive)
        return FAILURE;

    /* If the watchdog timer hasn't been refreshed in a while,
     * it will get locked by the PROC. If it isn't locked,
     * toggle the yellow "RUN" LED on the controller. */
    if (TIGetTimerVals(LOCKED) != 123)
    {
        /* Mask the timer interrupt to protect against a sudden call
         * and variable change. */
        /* Copy the variable value to a local copy. */
        iRet = iChangeLEDStat;
        /* Unmask the interrupt; allow timer interrupts again. This protection
         * process happens frequently throughout this module. */

        /* So it's still greater than -1000 (10 sec timeout interval) */
        if (iRet <= 0)
        {
            /* Get the LED status now... */
            iRet = inb(IO_LED_OUTPORT);
            /* ...and toggle it. */
            if (iRet & 0x01)
            {
                iRet &= 0xFE;
                iChangeLEDStat = CHANGE_LED_OFF;
            }
            else
            {
                iRet |= 0x01;
                iChangeLEDStat = CHANGE_LED_ON;
            }
            /* Don't forget to update the LED's. */
            IOWriteIO(-1, iRet, IO_LED_OUTPORT);
        }
    }
    /* After a watchdog timeout occurs, you must keep the error light on
     * and the run light from blinking. */
    else if (iChangeLEDStat <= 0)
    {
        IOWriteIO(-1, ~inb(IO_LED_OUTPORT), IO_LED_OUTPORT);
        iChangeLEDStat = CHANGE_LED_OFF;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TIGetTimerVals
 *
 * Abstract:    Safely gets the current value of the requested variable.
 *
 * Parameters:
 *      iValueDescArg       (in) pre-defined descriptor of which variable to get
 *
 * Returns:     The current value of the requested variable
 *
 ***************************************************************/
long long unsigned TIGetTimerVals(int iValueDescArg)
{
    long long unsigned ulValueTemp;

    if (!iTimerEnabled)
    {
        if ((iValueDescArg == SYS_TIMER_OVERFLOW) || (iValueDescArg == SYS_TIMER))
            return 0;
        else
            return ((unsigned long)(FAILURE));
    }
    switch (iValueDescArg)
    {
        case SYS_TIMER :
            ulValueTemp = *ulSysTimer;
//printf("TIGetTimerVals = %d\n",*ulSysTimer);
            break;

        case SYS_TIMER_OVERFLOW :
            ulValueTemp = *ulSysTimerOverflow;
            break;

        case LOCKED :
            ulValueTemp = (unsigned long)(iContLockedUp);
            break;

        case TIMER_INTERRUPT :
            ulValueTemp = (unsigned long)(iTimerInterrupt);
            break;

        case ACTION_TIMER_CHECK :
            ulValueTemp = (unsigned long)(iActionTimerChk);
            break;

        default:
            ulValueTemp = (unsigned long)(FAILURE);
            break;
    }

    return ulValueTemp;
}


/****************************************************************
 *
 * Function:    TISetTimerVals
 *
 * Abstract:    Sets the current value of the requested variable.
 *
 * Parameters:
 *      iValueDescArg       (in) pre-defined descriptor of which variable to get
 *      ulNewTimerValueArg  (in) The new value for the requested variable
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int TISetTimerVals(int iValueDescArg, long long unsigned ulNewTimerValueArg)
{
    if (!iTimerEnabled)
        return FAILURE;
    if ((iValueDescArg != SYS_TIMER_OVERFLOW) && (iValueDescArg != SYS_TIMER) &&
        (ulNewTimerValueArg != TRUE) && (ulNewTimerValueArg != FALSE))
        return FAILURE;
    switch (iValueDescArg)
    {
        case SYS_TIMER :
            *ulSysTimer = ulNewTimerValueArg;
            break;

        case SYS_TIMER_OVERFLOW :
            *ulSysTimerOverflow = ulNewTimerValueArg;
            break;

        case LOCKED :
            iContLockedUp = (int)(ulNewTimerValueArg);
            break;

        case TIMER_INTERRUPT :
            iTimerInterrupt = (int)(ulNewTimerValueArg);
            break;

        case ACTION_TIMER_CHECK :
            iActionTimerChk = (int)(ulNewTimerValueArg);
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}



/****************************************************************
 *
 * Function:    TITimerPROC
 *
 * Abstract:    Timer interrupt service routine
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 ***************************************************************/
void TITimerPROC()
{
    /* run this proc only certain interval */

    if(++ulTimerInterval <= 725) return;
    ulTimerInterval = 0;
//printf("TITimerPROC: iChangeLE=%d iMacroSwitchTime=%d\n",iChangeLEDStat, iMacroSwitchTime);

    /* The ulSysTimerOverflow is never actually used. */
    if (++(*ulSysTimer) == 0) (*ulSysTimerOverflow)++;
    /* ??? */
    iActionTimerChk = TRUE;

    /* Check for a watchdog timer timeout. */
//    if (--iChangeLEDStat < -1000)
//    {
        /* If a timeout occurred, indicate it with the iContLockedUp variable. */
//        iContLockedUp = 123;
        /* Make sure the iChangeLEDStat variable never gets set to a valid number again. */
//        CHANGE_LED_ON = CHANGE_LED_OFF = 100;
        /* Change the LEDs to indicate a controller error. */
//        IOWriteIO(-1, 0xFE, IO_LED_OUTPORT);
        /* Jump back to the top of the main loop. See the PROC module for details. */
//        longjmp(ISGetJmpBufMainMarkPtr(), -1);
//    }

    /* For 20ms task switching in multi-tasking mode. */
//    iMacroSwitchTime++;
//    if ((!iTimerInterrupt) && (iMacroSwitchTime >= 2))
//    {
//        iMacroSwitchTime = 0;
//        iTimerInterrupt = TRUE;
//    }

}

unsigned long TIRSTime(void)
{
    struct timespec tv;
//    struct timezone tz;
    unsigned long ulTime;

//    int uSec, ulMsec;


//    uSec = ulTimerCounter[uWhichTimerArg][0] / 1000;
//    ulMSec = ulTimerCounter[uWhichTimerArg][1] % 1000;
//    gettimeofday(&tv, &tz);

    clock_gettime(CLOCK_MONOTONIC, &tv);
    ulTime = tv.tv_sec * 1000 + tv.tv_nsec/1000000;
//printf("sec=%d usec=%d ulTime=%ld\n",tv.tv_sec, tv.tv_nsec, ulTime);

    return ulTime;
}

unsigned long TIRSTimeTMR(void)
{
//    struct timespec tv;
//    struct timezone tz;
    unsigned long ulTime;

//    int uSec, ulMsec;


//    uSec = ulTimerCounter[uWhichTimerArg][0] / 1000;
//    ulMSec = ulTimerCounter[uWhichTimerArg][1] % 1000;
//    gettimeofday(&tv, &tz);

//    clock_gettime(CLOCK_MONOTONIC, &tv);
//    ulTime = tv.tv_sec * 1000 + tv.tv_nsec/1000000;
//printf("sec=%d usec=%d ulTime=%ld\n",tv.tv_sec, tv.tv_nsec, ulTime);

    ulTime = MAXLONGVAL * glTimerMday + glTimerMsec;
    return ulTime;
}
