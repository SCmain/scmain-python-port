/****************************************************************
 *
 * Program:     Controller firmware
 * File:        isr.h
 * Functions:   
 *              ISSetJmpBufMark
 *              ISGetJmpBufMainMarkPtr
 *              ISGetJmpBufMainMark
 *              ISEnableMathErrorHandling
 *
 * Description: General use defines mainly for direct hardware
 *      interfacing. Used mostly for scanner, I/O, motion control,
 *      and timer. Functions used to return to the top of the main
 *      loop in the case of a catastrophic failure. The functions
 *      also handle the divide-by-zero interrupt service routine
 *      and global interrupt enabling and disabling (_enable() and
 *      _disable()).
 *
 * NOTE: The jmp system functions are functions intended for error handling.
 *      The setjmp function sets an element in the mainmark array to the
 *      memory address from where it was called. The longjmp function,
 *      when passed that array element, jumps back to that stored address.
 *      In this code, when an unrecoverable error is trapped, the program
 *      jumps back to the top of the main loop (which is where the setjmp
 *      function is). It is like a program-wide goto statement.
 *
 * NOTE: The _enable()/_disable() counter was necessary because of
 *      embedded calling. If a function called _disable() followed
 *      by another function that called _disable(), when the embedded
 *      function called _enable(), it would leave the original
 *      function enabled also. So in the new overlay functions,
 *      the actual call to _enable() cannot be made until until
 *      it has been called an equal number of times as the disable.
 *      WARNING!!!!! MATCH UP DISABLE AND ENABLE CALLS OR THE SYSTEM
 *      MAY NEVER BE RE-ENABLED!!!!!
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#ifndef _H_PROC_H
#define _H_PROC_H

#include <setjmp.h>

/********** DEFINES **********/


/********** FUNCTION PROTOTYPES **********/

// See the file header Note: for a general explanation of these functions
int *ISGetJmpBufMarkPtr();
int ISSetJmpBufMark(unsigned uMarkIndex, int iNewVal);
int *ISGetJmpBufMainMarkPtr();
int ISGetJmpBufMainMark(unsigned uMarkIndex);

int ISEnableMathErrorHandling();

#endif
