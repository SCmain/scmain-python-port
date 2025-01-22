/***************************************************************\
 *
 * Program:     Motion Control Interrupt Service Routines
 * File:        gaintr.h
 *              GATurnOffGalilInt
 *
 * Description: Provide interface between other modules and the Motion
 *              Control Interrupt routines.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef  _H_GAINTR_H
#define  _H_GAINTR_H

/********** FUNCTION PROTOTYPES **********/

/* Grouped in with local interrupt functions in MCMain.c */
int GATurnOffGalilInt(int iCardNoArg);


#endif
