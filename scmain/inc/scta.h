/***************************************************************\
 *
 * Program:     task module interface header
 * File:        task.h
 * Functions:   None
 *
 * Description: Provide interface between task module other
 *              modules
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_TASK_H
#define _H_TASK_H

#include "sctag.h"

/********** VARIABLES USED LOCALLY **********/
pstTaskList pstCurrentTaskList;         /* current task list pointer */
int iCurrentMacro;                      /* current macro is running in task */
int iPCCriticalSectionFlag;             /* flag for enter and exit critical section */
int iNoTaskFlag;                        /* flag signal with or without multi-tasking */
stTaskList staTaskList[MAXTASKS];       /* list of 0-9 tasks */
int iTaskID;                            /* current task id */
int iTimerForTKWait;

#endif
