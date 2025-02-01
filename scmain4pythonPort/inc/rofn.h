/***************************************************************\
 *
 * Program:     Controller Firmware
 * File:        rofn.h
 * Functions:   
 * Description: Provides the interface between the Main module and Mechanism
 *      module. Initializes the system configuration.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_ROFN_H
#define _H_ROFN_H

#include "romain.h"

/********** FUNCTION PROTOTYPES **********/

/* Main module initializes the Mechanism module configuration */
int ROInit(int iNumAxesArg,         /* The total number of axes in the system */
            int *iaMechTypeArg,     /* Array of the datafile type in the system; by axis */
            int *iaEquipeAxisArg,   /* Array of the Equipe axes definitions in the system; by axis */
            int *iaGalilAxisArg,    /* Array of the corresponding Galil axes; by axis */
            int *iaSpecialAxisArg,  /* Array defining if a particular axes has special characteristics,
                                     * i.e. RO_TRACK, RO_DUAL_ARM, etc.; by axis */
            int iDefineFlagArg,     /* The define flags for the current system configuration */
            int iEmulatorArg);      /* The emulation type for the current system configuration */

#endif
