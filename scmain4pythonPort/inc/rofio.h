/***************************************************************\
 *
 * Program:     Controller Firmware
 * File:        rofio.h
 * Functions:   ROGetParameter() - To allow the Mechanism's customer to get
 *                                  the parameters from Mechanism module.
 *                                  Either from Datafile or Motion.
 *              ROSetParameter() - To allow the Mechanism's customer to put
 *                                  the parameters to Mechanism module
 *                                  Either to Datafile or Motion.
 *
 * Description: Provide an interface to mechanism Get/Set functions.
 *      Used to get and set galil/datafile variables.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_ROFIO_H
#define _H_ROFIO_H

/********** GET FUNCTIONS; Datafile and Motion interaction **********/
int ROGetParameter(int iGetFromParamFileArg, unsigned long ulEquipeAxisArg, long *lParameterArg, int iParameterType);

/********** SET FUNCTIONS; Datafile and Motion interaction **********/
int ROSetParameter(int iSaveToParamFileArg, unsigned long ulEquipeAxisArg, long *lParameterArg, int iParameterTypeArg);

#endif
