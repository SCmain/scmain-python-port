/***************************************************************\
 *
 *              Copyright (c) 2007 XyZ Automation, Inc.
 *
 * Program:     main header
 *
 * File:        scmain.h
 *
 * Functions:   Init
 *
 *
 * Description: This is main module for the whole project.  In this module, it does all the
 *              initialization of other modules. It provides all the configuration of the
 *              system.
 *
 *
 *
 * Environment: SUSE LINUX C
 *
 * Modification history:
 *
 * Rev      Date    Brief Description
 * 01A      040607  Initial version
 *
\***************************************************************/

#ifndef _H_SCMAIN_H
#define _H_SCMAIN_H

/********** DEFINES **********/

#define EX_AUTOMACRO    100


/********** FUNCTIONS USED LOCALLY **********/

/****************************************************************\
 *
 * Description: Do initialization of all the other
 *      modules, providing all the configuration information
 *      for the entire system.
 *
\*****************************************************************/
int Init(int iWatchDog);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Prints a message of which module failed initialization
 *      and then error exits to the prompt, in DEBUG MODE ONLY!!!
 *      In normal operation, the firmware continues even after
 *      an initialization failure. So the user can determine what
 *      failed later through host commands.
 *
\***************************************************************/
int FailureExit(char *cpFailMsg);

#endif


