/***************************************************************\
 *
 * Program:     Motion Control Test Routines (Diagnostics)
 * File:        gatst.h
 * Functions:   GATestGalil
 *              GATestLSHS
 *              GATestEncoder
 *              GASetupAMPBoard
 *              GASetupAMPMode
 *
 * Description: Provide test routines for Motion Control module
 *      testing. They are used by the diagnostics test steps.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_GATST_H
#define _H_GATST_H

/********** FUNCTION PROTOTYPES **********/
int GATestGalil( int iCardNoArg );
int GATestLSHS( int iCardNoArg, int iVacExistsArg );
int GATestEncoder( int iCardNoArg, int iVacExistsArg );
int GASetupAMPBoard( int iCardNoArg );
int GASetupAMPMode( int iCardNoArg, int iVacExistsArg );

#endif
