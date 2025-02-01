/***************************************************************\
 *
 * Program:     Motion Control Main Routines
 * File:        gamn.h
 * Functions:   GAInit
 *              GAGetMaxNumberOfAxisSupported
 *
 * Description: Provide routines for initializing the Motion
 *      Control module and getting initization realted information.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_GAMN_H
#define _H_GAMN_H

/********** FUNCTION PROTOTYPES **********/
int GAInit(int iNumberOfGalilCards, int iPreOnly);
int GAGetMaxNumberOfAxisSupported(int iCardNoArg, int *iNumOfAxis);
int MEPROC(int iCardNoArg);

#endif
