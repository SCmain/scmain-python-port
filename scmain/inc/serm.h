/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        commM.h
 * Functions:   SERInitPorts
 *              SERCheckINXMode
 *
 * Description: Handles low-level RS232 communications
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef COMM_MAIN
#define COMM_MAIN

#include "serg.h"

/********** FUNCTION PROTOTYES *********/

/***************************************************************\
 *
 * Used in: main
 * Description: Initializes the selected comm port by setting the
 *      member variables of the port structures to initial values.
\***************************************************************/
int SERInitPorts(    int iPortNumArg,        /* Com port number */
                    int iBaudRateArg,       /* Baud Rate */
                    int iParityArg,         /* Parity */
                    int iDataBitsArg,       /* Data bits */
                    int iStopBitsArg,       /* StopBits */
                    /* TRUE to require CTS high before sending */
                    int iFlowCtrlArg,
                    /* TRUE to open port for the teach pendant */
                    int iEchoPortArg,
                    /* TRUE to setup port with SECS communications */
                    int iSECSArg);

/***************************************************************\
 *
 * Used in: main
 * Description: Toggles the INX mode for a port. It checks if
 *      CTS/RTS has dropped. If it has on COM2 or COM3, it toggles
 *      the port into SECS mode. If CTS/RTS recovered, it switches
 *      back to ASCII mode and displays the Equipe logo.
 *
\***************************************************************/
int SERCheckINXMode();

#endif
