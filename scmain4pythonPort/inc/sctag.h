/***************************************************************\
 *
 * Program:     global task module interface header
 * File:        taskglob.h
 * Functions:   TKGetTaskListArray
 *              TKInitTaskList
 *              TKSetPCCriticalSection
 *              TKGetNextTask
 *              TKGetNextWaitingTask
 *              TKGetCurrentTask
 *              TKSetCurrentTask
 *              TKSetCurrentMacro
 *              TKKillCurrentTask
 *              TKSetNoTaskFlag
 *              TKReadNoTaskFlag
 *              TKKillTask
 *              TKListActiveTasks
 *              TKMakeTask
 *              TKGetTaskId
 *              TKSetTaskId
 *              TKWait
 *              TKCheckTask
 *
 * Description: Provide interface between task module and other
 *              modules for multi-tasking mostly.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_TASKGLOB_H
#define _H_TASKGLOB_H

#include "cmdfns.h"

#define MAXTASKS    10
#define MAXSTACK    30

/********** VARIABLES **********/

// stores the the path of failure (x.x.x.) if task has failed
char caTaskStatus[MAXTASKS][50];

/* structure that holds information of each task*/
typedef struct task_list_tag
{
    instr_ptr m_NextPC;                             /* To execute next */
    instr_ptr m_OtherPC;                            /* Interrupted one (to go back) - never been used */
    int m_iValid;                                   /* Used or not used */
    int m_iActive;                                  /* Currently executing */
    int m_iCount;                                   /* Pending execution counter */
    int m_iFreezeStatus;
    long unsigned m_ulTimerCounter;                 /* For WAIT command */
    unsigned  m_uTcOverflow;
    int m_iPCWaiting;                               /* task in the waiting process flag */
    int m_iTaskPendingKilled;
    struct prog_status_st  *m_pStatus;         /* pointer always points to begin of array or stack */
    struct prog_status_st  *m_pStackTop;       /* pointer to hold the current top of stact of specific task */
    long m_lcf;
    int p_MOwnerTbl;                                /* macro that is owned by the specific task */
}stTaskList,  *pstTaskList;


/********** FUNCTION PROTOTYPES **********/

/***************************************************************
 * Function Name:   TKGetTaskListArray
 * Description:     get the first task in task_list array. The array holds 10 task and each
 *                  task is a structure.
 * returns:         return pointer that points to the beginning of an array.
 * **********************************************************************/
pstTaskList TKGetTaskListArray();
/**************************************************************
 * Function Name:   TKInitTaskList
 * Purpose:         allocate the memory and initialize the task list. There are 10 task in the list and
 *                  each task has each own stack. Also initialize the critical section to 0, set
 *                  no_task_flag to TRUE (no multi-tasking) and set the task id to be invalid (-1).
 * Returns:         success/failure
 * **************************************************************/
int TKInitTaskList(void);
/**************************************************************
 * Function Name:   TKSetPCCriticalSection
 * Purpose:         set the critical section flag to either in the critical section
 *                  or exit the critical section. This crictical section allows only
 *                  one task access at a time into the task list.
 * Parameter:       iNewFlagArg - flag to determine in or out of critical section (-1/1)
 * Return:          flag
 * **************************************************************/
int  TKSetPCCriticalSection( int newflagArg );
/**************************************************************
 * Function Name:   TKGetNextTask
 * Purpose:         get the current task that active and valid. If
 *                  next task is equal to current task, then there is
 *                  no next task and return NULL. Otherwise return the
 *                  next active and valid task.
 * Returns:         current active/valid task in the tasklist or
 *                  return NULL if not active task in the list.
 * **************************************************************/
pstTaskList  TKGetNextTask( void );
/*********************************************************
 * Function Name:   TKGetNextWaitingTask
 * Purpose:         get the next waiting or active task from the tasklist
 *                  return the task. If there is no task in the tasklist is waiting or
 *                  active state then return NULL.
 * return:          structure pointer to next waiting task
 * *********************************************************/
pstTaskList  TKGetNextWaitingTask( void );
/**************************************************************
 * Function Name:   TKGetCurrentTask
 * Purpose:         get the current running task
 * Return:          structure pointer to current task.
 * **************************************************************/
pstTaskList  TKGetCurrentTask( void );
/**************************************************************
 * Function Name:   TKSetCurrentTask
 * Purpose:         set the task to be the current task
 * Parameter:       pTaskListArg    - task to be set to become current task.
 * return:          previous task.
 * **************************************************************/
pstTaskList  TKSetCurrentTask( pstTaskList pTaskListArg );
/***********************************************************************
 * Function Name:   TKSetCurrentMacro
 * Purpose:         Before set the entire macro entry to oc table.
 *                  Now, we only need to set the OC number.
 *                  Only keep oc number for current macro.
 * Return:          previous current macro.
 * ***********************************************************************/
int TKSetCurrentMacro( int iOpcodeArg );
/*****************************************************
 * Function Name:   TKKillCurrentTask
 * Purpose:         kill current task by initializing structure of the task.
 *                  such as waiting flag, active flag, valid flag, taskpendingkilled flag to FAlSE,
 *                  set the timer counter to 0, set the top of stack of current task points to the
 *                  begining of stack of current task and then set the current task pointer to NULL.
 * Parameter:       iFailureFlag - indicates if the task is getting killed because of the macro failure
 * Returns:         none.
 * ******************************************************/
void TKKillCurrentTask( int iFailureFlagArg );
/**************************************************************
 * Funciton Name:   TKSetNoTaskFlag
 * Purpose:         set no task flag. This flag is signal either there is multi-tasking
 *                  or no multi-tasking.
 * Parameter:       iNoTaskFlagArg - flag value to set.(true/false)
 * Returns:         none.
 ***************************************************************/
void TKSetNoTaskFlag( int iNoTaskFlagArg );
/**************************************************************
 * Function Name:   TKReadNoTaskFlag
 * Purpose:         get no task flag
 * Return:          value of the flag (true/false)
 * ***************************************************************/
int  TKReadNoTaskFlag( void );


/********** ex_COMMAND RELATED FUNCTION PROTOTYPES **********/

/**************************************************************
 * Function Name:   TKKillTask
 * Purpose:         Kill the given task.(for ex_TSKK use)
 *                  When killing the specified task, make sure to check for
 *                  the next task is not NULL. If it is NULL, then there is no
 *                  more task is running and that's mean there is no multi-tasking, so
 *                  we need to set no-task-flag to TRUE.
 * Note:            Kills task.  If no task id specified, kills all tasks.
 * Parameter:       iOperandFlagArg     - (true/false) there is operand or no operand
 *                  iTaskIdArg          - task id
 *                  NextInstrArg        - next instruction to be set into the next executing instruction.
 * Return:          SUCCESS or FAILURE
 * **************************************************************/
int TKKillTask(int iOperandFlagArg, int iTaskIdArg, instr_ptr NextInstrArg);
/*************************************************************************
 * Function Name:   TKListActiveTasks
 * Purpose:         list all active tasks from the list is currently running.
 *                  (for ex_TSKL)
 * return:          SUCCESS or FALSE
 * *************************************************************************/
int TKListActiveTasks(void);
/**************************************************************
 * Function Name:   TKMakeTask
 * Purpose:         Create the task and set it to be the current task if there is
 *                  not current task. Ohterwise, after created the task and continue
 *                  to run the currrent task.  If no task number specified,
 *                  then makes task on first available task in the list.
 *                  Then check if the no-task-flag is TRUE(no multi-tasking), we need
 *                  to set the no-task-flag to FALSE (with multi-tasking) since there is
 *                  a multi-tasking running after the task is created.
 *                  To create the task, you need to make sure the specified task id is valid
 *                  and task structure is available. Then set the active and valid flag in the
 *                  structure to be TRUE. NOTE: for the structure information please look the
 *                  code or header file for more detail.
 *                  This function is called by ex_TSKM.
 * return:          SUCCESS or FAILURE
 * **************************************************************/
int TKMakeTask(int iOperandFlagArg, int iTaskIdArg, instr_ptr NextInstrArg);
/**********************************************************
 * Function Name:   TkGetTaskId
 * Purpose:         get the task id (for ex_TSKI)
 * return:          SUCCESS or FAILURE
 * **********************************************************/
int  TKGetTaskId();
/***********************************************************
 * Function Name:   TkSetTaskId
 * Purpose:         Set the task id (for ex_TSKI)
 * return:          None
 * **********************************************************/
void TKSetTaskId( int iTaskIdArg );
/***********************************************************************************************************
 * Function Name:   TKWait
 * Purpose:         waiting for certain time. (for ex_Wait)
 *                  First, getting the current task and if there is a current task (with multi-tasking) and the task is
 *                  already in the waiting state then turn off the timer interrupt and exit the function.
 *                  If the task is not at waiting state then set the task state to be waiting (TRUE) and
 *                  set the time to wait to timer counter. Also set it's state to be not active, turn on the task
 *                  timer interrupt and exit the function.
 *                  If there is no current task (no multi-tasking), then check current PC is at waiting state or not.
 *                  If it's not at waiting state then set the timer counter, set the state to be waiting state (TRUE) and
 *                  re-run the instruction again until the timer is up.  Otherwise, it's at waiting state, then check for
 *                  timer if the timer is up then set the waiting state to FALSE else re-run the instruction again.
 * return:          SUCCESS or FAILURE
 * ***********************************************************************************************************/
int TKWait( long lTime, instr_ptr instrArg);
/**************************************************************
 * Function Name:   TKCheckTask
 * Purpose:         check for the waiting and valid task. Then
 *                  if counter time < luSysTimer then set the
 *                  task to be active and no waiting.
 * Parameter:       luSystimer - the time for check to switch to anther
 *                               task.
 * returns:         none
 * **************************************************************/
void TKCheckTask(long unsigned luSysTimer);

#endif
