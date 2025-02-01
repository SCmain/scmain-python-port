/****************************************************************
 *
 * Program:     Controller firmware
 * File:        dg.h
 * Functions:   DiagStart
 *              DiagGetFlag
 *              DiagSetFlag
 *
 * Description: Handles the interface to diagnostics mode
 *
 * Modification history:
 *
 *
 ****************************************************************/

#ifndef _H_DIAG_H
#define _H_DIAG_H

/******** Function Prototypes ***********/

/****************************************************************
 *
 * Used in: main
 * Description: Starts diagnostics mode. This function is the primary
 *      interface to the diagnostics module. It handles setting up the
 *      teach pendant to be used in diagnostics mode, the user keypresses
 *      to continue into either robot or prealigner diagnostics mode,
 *      and returns the teach pendant and robot to their normal
 *      working conditions when finished.
 *
 *
 ***************************************************************/
int DiagStart(int iPortNumArg, int iRobArg, int iPreArg, int iVacArg, int iDefineFlagArg, int iEmulatorArg);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: This GETS the value of the iDiagFlag variable.
 *      It replaces the diagnostics mode operation of the old
 *      go_to_teach variable used in the main (main loop)
 *      and execute (ex_DIAG) functions.
 *
\***************************************************************/
int DiagGetFlag();

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: This SETS the value of the iDiagFlag variable.
 *      It replaces the diagnostics mode operation of the old
 *      go_to_teach variable used in the main (main loop)
 *      and execute (ex_DIAG) functions.
 *
\***************************************************************/
int DiagSetFlag(int iPortNumArg);

#endif
