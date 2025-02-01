/**********************************************************************
 *
 *  Program:        Aligner Module File I/O and Read/Set Variable functions
 *  Function:
 *  Description:    Contains access Functions for Wafer Calibration Structure
 *                  as well as Get and Set Functions for Aligner module variables
 *                  needed from outside the module
 *
 *  Modification history:
 *
 *  Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#ifndef _H_ALFIO_H
#define _H_ALFIO_H

extern int giFlatIndexEndPrev;                              // flat_i_end_old

int ALGetPreData(long lIndexArg, unsigned int *puiDataArg);    // data - data collection buf
int ALSetPreData(long lIndexArg, unsigned int uiDataArg);
int ALGetDUPCount(unsigned long *ulpDuplicateCountArg);
int ALSetDUPCount(unsigned long ulDuplicateCountArg);
int ALGetBadCounter(unsigned long *ulpBadCountArg);
int ALSetBadCounter(unsigned long ulBadCountArg);
int ALGetIntVars(int iVariableTypeArg, int iIndexArg, int *ipVariableValueArg);
int ALSetIntVars(int iVariableTypeArg, int iIndexArg, int iVariableValueArg);
int ALGetLongVars(int iVariableTypeArg, int iIndexArg, long *lpVariableValueArg);
int ALSetLongVars(int iVariableTypeArg, int iIndexArg, long lVariableValueArg);

int ALSetAligningGASpeed(unsigned long ulAxisArg, long *lpWaferSpeedArg, long *lpWaferAccelArg);
int ALGetAligningSpeed(int iWhichArg, long *lpWaferSpeedArg, long *lpWaferAccelArg);

#endif
