/****************************************************************
 *
 * Program:     Controller firmware
 * File:        timer.h
 * Functions:   TIDisableTimer
 *              TIResetComputer
 *              TISetCounter
 *              TIReadCounter
 *              TICountExpired
 *              TIDelay
 *              TIRefreshWD
 *              TIGetCounter
 *              TIReturnCounter
 *
 * Description: Interface functions to the timer routines
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 ****************************************************************/

#ifndef _H_TIMER_H
#define _H_TIMER_H

/********** DEFINES **********/

/* Descriptors to reference which values to get or set. */
#define SYS_TIMER           0
#define SYS_TIMER_OVERFLOW  1
#define LOCKED              2
#define TIMER_INTERRUPT     3
#define ACTION_TIMER_CHECK  4

/* extern used statically */
extern int iLEDOutPort;     /* LEDOUTPORT1 or LEDOUTPORT2 if PA config */

/********** FUNCTION PROTOTYES *********/

/****************************************************************
 *
 * Replaces: disable_timer
 * Used in: execute2
 * Description: Disables timer PROC by restoring the old interrupt
 *      vector and resetting the timer chaining feature if it was
 *      used. It also disables system interrupts globally before
 *      restoring old settings.
 *
 ***************************************************************/
int TIDisableTimer();

/***************************************************************\
 *
 * Replaces: reset_computer
 * Used in: execute2
 * Description: Causes a reboot (may be called under interrupt from watchdog timer)
 *      The reboot occurs by jumping to the reboot address at F000:E05B.
 *      Since it is not possible to jump to an absolute address in C,
 *      we form a function pointer and then call the reboot address --
 *      obviously there is no return to this call. That's why there are no
 *      checks for a FAILURE return code from any functions called.
 *
\***************************************************************/
void TIResetComputer();

/***************************************************************\
 *
 * Replaces: set_counter, cset_counter
 * Used in: EVERYWHERE, mydelya1
 * Description: Sets the number of timer ticks before a timeout
 *      is registered for a checked out counter number
 *      by adding the requested number of timer-ticks-
 *      to-a-timeout to the current value of the system timer.
 *      REMEMBER: Timer ticks occur in 10 ms increments.
 *
\***************************************************************/
int TISetCounter(   unsigned uWhichTimerArg,    /* which timer to track;
                                                 * max is 8, min is 0 */
                    unsigned uTimeoutArg);      /* the timeout interval, i.e.
                                                 * number of timer ticks to wait */

/***************************************************************\
 *
 * Replaces: read_counter
 * Used in: bal
 * Description: Gets the current number of elapsed timer ticks
 *      to a timeout. It subtracts the current system timer
 *      value from the requested timeout value thus returning the number
 *      of timer ticks elapsed since a timeout value was set
 *      by TISetCounter.
 *
\***************************************************************/
int TIReadCounter(  unsigned uWhichTimerArg,    /* which timer to track */
                    unsigned *uTimerValArg);    /* number of timer ticks elapsed */

/***************************************************************\
 *
 * Replaces: count_expired, ccount_expired
 * Used in: EVERYWHERE, mydelya1
 * Description: Gets a TRUE or FALSE value as to whether or not
 *      the requested timeout period has elapsed. This is indicated
 *      when the system timer exceeds the timeout value set by
 *      TISetCounter.
 *
\***************************************************************/
int TICountExpired(unsigned uWhichTimerArg);

/***************************************************************\
 *
 * Replaces: delay_a5, short_wait
 * Used in: EVERYWHERE, isr
 * Description: Gets a TRUE or FALSE value as to whether or not
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
\***************************************************************/
int TIDelay(unsigned uTimeoutArg);

/***************************************************************\
 *
 * Replaces: refresh_wd
 * Used in: EVERYWHERE
 * Description: Resets the watchdog timer. See header
 *      comments for more details.
 *
\***************************************************************/
int TIRefreshWD();


/*************** NEW FUNCTIONS ******************
 * Next few functions get and set variable values
 ************************************************/
int TIGetCounter();

int TIReturnCounter(unsigned uWhichTimerArg);

long long unsigned TIGetTimerVals(int iValueDescArg);

int TISetTimerVals(int iValueDescArg, long long unsigned ulNewTimerValueArg);

/****************************************************************
 * Function:    TIInitLedInternalPort
 * Abstract:    Assign port number for LED and INPUT G.
 * Parameters:  caSysCfgStringArg
 * Used in:     main
 ***************************************************************/
void TIInitLedInternalPort(char *caSysCfgStringArg);

unsigned long TIRSTime(void);
unsigned long TIRSTimeTMR(void);
void *procTimer(void *ptr);
void MainTimerHandler(int signum);
int MainTimerExpired(int iTimerArg);
int MainTimerSet(int iTimerArg, long lMsecArg);
int MainReadTimer(int iTimerArg, long *lTimerValArg);
int MainTimerDelay(long lMsecArg);






#endif
