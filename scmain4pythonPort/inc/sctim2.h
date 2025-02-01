/****************************************************************
 *
 * Program:     Controller firmware
 * File:        timerL.h
 * Functions:   TITimerPROC
 *              TIOldTimerPROC
 *
 * Description: Handles all timer related operations.
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
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
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 ****************************************************************/

#ifndef _H_TIMER_LOCAL_H
#define _H_TIMER_LOCAL_H

/********** DEFINES **********/

#define MAX_TIMER 15        /* Total number of timers */
#define TIMER_INTNO 0x08    /* Hardware system timer interrupt number. */
#define TM_MASK 0x01        /* Programmable interrupt controller mask for
                             * tunring on/off the system timer interrupt. */


/********** VARIABLES USED LOCALLY **********/


/* Indicates that the initialization functions have been successfully called */
int iTimerEnabled = FALSE;
int iWDTimerActive = FALSE;
/* Keeps track of which timers are being used */
int iTimerActive[MAX_TIMER] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int iTimerUsed[MAX_TIMER] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/* The timeout value that must be exceeded, listed by timer number */
long long unsigned ulTimerCounter[MAX_TIMER][2] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/* The timer value that is updated by the PROC and its unimplemented overflow indicator */
long long unsigned  *ulSysTimer;
long long unsigned  *ulSysTimerOverflow;
/* Indicates that the PROC has been hit often but the watchdog timer refresh has not.
 * This is checked by the Macro module to restrict access to low-level functions
 * when the watchdog timer is "tripped". */
int iContLockedUp = 0;
/* Controller front panel LED operators */
static int iChangeLEDStat = 0;
static int CHANGE_LED_ON = 35;
static int CHANGE_LED_OFF = 65;
/* Indicates that it is time to switch between multi-tasked macros; 20ms interval */
static int iMacroSwitchTime = 0;
/* The last 2 variables; ??? */
static int iTimerInterrupt = FALSE;
static int iActionTimerChk = FALSE;


/********** FUNCTIONS USED LOCALLY **********/
void TITimerPROC();
void *TIOldTimerPROC();

#endif
