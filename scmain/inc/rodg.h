/***************************************************************\
 *
 * Program:     Controller Firmware
 * File:        rodg.h
 * Functions:   ROTestHome
 *              ROAdjustRAtHome
 *              ROAdjustRAtIndex
 *              ROSetHomeR
 *              ROSetTestScaling
 *              RODoROffset
 *              ROPresetHomeR
 *              ROTestVacuum
 *              ROTestRobotIO
 *
 * Description: Provide an interface to mechanism based diagnostics
 *      functions
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_RODG_H
#define _H_RODG_H

#include "romain.h"

/********** DIAGNOSTICS FUNCTIONS **********/

int ROTestHome(int iCardNoArg, int iVacExistsArg);
int ROAdjustRAtHome(unsigned long ulAxisArg);
int ROAdjustRAtIndex(unsigned long ulAxisArg);
int ROSetHomeR(void);
int ROSetTestScaling(int iDevModeArg, int iVacExistsArg);
int RODoROffset(void);
int ROPresetHomeR(void);
int ROTestVacuum(int iCardNoArg);
int ROTestRobotIO(int iDevModeArg);

#endif
