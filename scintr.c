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
 * File:        scintr.c
 * Functions:   
 *              ISGetJmpBufMarkPtr
 *              ISSetJmpBufMark
 *              ISGetJmpBufMainMarkPtr
 *              ISGetJmpBufMainMark
 *              ISEnableMathErrorHandling
 *
 * Description: General use defines mainly for direct hardware
 *      interfacing. Used mostly for scanner, I/O, motion control,
 *      and timer. Functions used to return to the top of the main
 *      loop in the case of a catastrophic failure.
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/
#include <sys/io.h>
#include <signal.h>
#include <float.h>
#include "sck.h"
#include "scintr.h"
#include "alk.h"
#include "alfio.h"

#define DIVIDE_BY_ZERO_INTNO 0

int iFPError = 0;

/* Function and function pointer prototypes */
//void __cdecl ISFPErrHandler(int iSignal, int iSubcode);
void ISFPErrHandler(int iSignal, int iSubcode);
void ISDivideByZeroPROC();
void (*ISOldDivideByZeroPROC)();

/* Keeps track of the global _disable and _enable of interrupts */
static int iDisableCount = 0;

/* Code marking variables. One for the main loop, and one for general use. */
int iaMainMark[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int iaMark[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};



/****************************************************************
 *
 * Function:    ISGetJmpBufMarkPtr
 *
 * Abstract:    Gets the pointer to the array
 *
 * Parameters:  None
 *
 * Returns:     The address of the array
 *
 ***************************************************************/
int *ISGetJmpBufMarkPtr()
{
    return &iaMark[0];
}


/****************************************************************
 *
 * Function:    ISSetJmpBufMark
 *
 * Abstract:    Sets the value of a particular index in the array
 *
 * Parameters:
 *      uMarkIndex      (in) The index of the array
 *      iNewVal         (in) The value to set it to
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ISSetJmpBufMark(unsigned uMarkIndex, int iNewVal)
{
    unsigned _JBLEN = 10;

    if (uMarkIndex >= _JBLEN)
        return FAILURE;

    iaMark[(int)(uMarkIndex)] = iNewVal;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ISGetJmpBufMainMarkPtr
 *
 * Abstract:    Gets the address of the array
 *
 * Parameters:  None
 *
 * Returns:     The address of the array
 *
 ***************************************************************/
int *ISGetJmpBufMainMarkPtr()
{
    return &iaMainMark[0];
}


/****************************************************************
 *
 * Function:    ISGetJmpBufMainMark
 *
 * Abstract:    Gets the value of a particular index in the array
 *
 * Parameters:
 *      uMarkIndex      (in) The index of the array
 *
 * Returns:     The value of the array at the index point
 *
 ***************************************************************/
int ISGetJmpBufMainMark(unsigned uMarkIndex)
{
    unsigned  _JBLEN = 10;

    if (uMarkIndex >= _JBLEN)
        return FAILURE;

    return iaMainMark[(int)(uMarkIndex)];
}


/****************************************************************
 *
 * Function:    ISFPErrHandler
 *
 * Abstract:    Redirects
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS of FAILURE
 *
 ***************************************************************/
//void __cdecl ISFPErrHandler(int iSignal, int iSubcode)
void ISFPErrHandler(int iSignal, int iSubcode)
{
    int iAligning;

    iFPError = iSubcode;
//    _fpreset();

//    ALGetIntVars(AL_ALIGNING_FLAG, AL_NOIDX, &iAligning);
    /* This will now handle any floating point error caused during aligning. */
//    if (iAligning)
//        longjmp(ISGetJmpBufMarkPtr(), -1);
    /* This will now handle any floating point error NOT caused during aligning. */
//    longjmp(ISGetJmpBufMainMarkPtr(), -3);
}


/****************************************************************
 *
 * Function:    ISEnableMathErrorHandling
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS of FAILURE
 *
 ***************************************************************/
int ISEnableMathErrorHandling()
{
    /* Set the floating point error handler. */
    signal(SIGFPE, (void*)ISFPErrHandler);

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ISDivideByZeroPROC
 *
 * Abstract:    Resets environment conditions if a divide-by-zero occurs
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 ***************************************************************/
void ISDivideByZeroPROC()
{
//    if (ISGetJmpBufMainMark(8))
//        longjmp(ISGetJmpBufMainMarkPtr(), -2);
}
