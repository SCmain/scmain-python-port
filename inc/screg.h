/***************************************************************\
 *
 * Program:     Register R, C, and S
 * File:        rcs.h
 * Functions:   None
 *
 * Description: Declare global variables for R-Registers, C-Registers, S-Registers and
 *              Special R-Registers for Novellus.
 *              These variables are only global to local module (RCS) only.
 *              -   R-Registers have 100 registers (0-99)
 *              -   C-Registers have 100 registers (0-99)
 *              -   S-Registers have 20 registers (1-20), each string register's length is 256.
 *              -   Special R-Registers have 25 registers (for novellus only)
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_RCS_H
#define _H_RCS_H

#include "scregg.h"

/********** VARIABLES USED LOCALLY **********/

long *RGplRegister;                  /* R-variable */
long *RGplSpecialRegister;           /* R-Special variable for novellus*/
char *RGpcCharacter;                 /* C-variable */
char *RGpacString[MAXSTRINGVARS+1];  /* S-variable */

#endif
