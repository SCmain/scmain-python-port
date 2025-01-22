/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        timerM.h
 * Functions:   TIEnableTimer
 *              TIActivateWD
 *
 * Description: Handles low-level counter/timer initialization
 *      including software watchdog initialization
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
\***************************************************************/

#ifndef _H_TIMER_MAIN_H
#define _H_TIMER_MAIN_H

/********** FUNCTION PROTOTYES *********/

/***************************************************************\
 *
 * Replaces: enable_timer
 * Used in: main
 *
\***************************************************************/
int TIEnableTimer();

/***************************************************************\
 *
 * Replaces: activate_wd
 * Used in: main
 * Description: Enables the software watchdog timer. See the note
 *      in the header comments for more details.
 *
\***************************************************************/
int TIActivateWD();

#endif
