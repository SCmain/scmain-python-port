/***************************************************************\
 *
 * Program:     Task module
 * File:        Task.c
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
 * Description: Emulating the multi-tasking process for macro.
 *              Each task has the id and has each own stack.
 *
 * NOTE:    Task #0 has the highest priority
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <malloc.h>
#include <limits.h>

#include "scta.h"
#include "sck.h"
#include "cmdfns.h"
#include "ser.h"
#include "scmac.h"
#include "sctim.h"
#include "scintr.h"
#include "scmem.h"

extern int giDispTime;

/***************************************************************
 * Function Name:   TKGetTaskListArray
 * Description:     get the first task in task_list array. The array holds 10 task and each
 *                  task is a structures.
 * returns:         return pointer that points to the beginning of an array.
 * **********************************************************************/
pstTaskList TKGetTaskListArray()
{
    return ((pstTaskList)staTaskList);
}

/**************************************************************
 * Function Name:   TKInitTaskList
 * Purpose:         allocate the memory and initialize the task list. There are 10 task in the list and
 *                  each task has each own stack. Also initialize the critical section to 0, set
 *                  no_task_flag to TRUE (no multi-tasking) and set the task id to be invalid (-1).
 * Returns:         success/failure
 * **************************************************************/
int TKInitTaskList(void)
{
    int icount, iCmdPort;               /* iCount is temporary holding the taskid
                                         * iCmdPort is command port id */

    iCmdPort = SERGetCmdPort();
    /* initialize all content in the task to be 0 */
    memset( staTaskList, 0, MAXTASKS*sizeof(stTaskList) );
    pstCurrentTaskList = NULL;

    iCurrentMacro = INVALID_OPCODE;
    /* Allocate the memory for the stack of each task.
     * Initialize the top of the stack to point to the begining of the stack.*/
    for(icount = 0; icount < MAXTASKS; icount++)
    {
        staTaskList[icount].m_pStatus = (struct prog_status_st *)malloc( MAXSTACK*sizeof(struct prog_status_st) );
        if(!staTaskList[icount].m_pStatus)
        {
#ifdef DEBUG
            SERPutsTxBuff(iCmdPort, "***TKInitTaskList: out of memory when doing malloc***\n\r?");
#endif
            return FAILURE;
        }
        staTaskList[icount].m_pStackTop = staTaskList[icount].m_pStatus;
    }
    iPCCriticalSectionFlag = 0;
    iNoTaskFlag = TRUE;                 /* set the flag to no multi-tasking */
    TKSetTaskId(-1);                    /* set the current task id to be invalid */
	iTimerForTKWait = TIGetCounter();
    return SUCCESS;
}

/**************************************************************
 * Function Name:   TKSetPCCriticalSection
 * Purpose:         set the critical section flag to either in the critical section
 *                  or exit the critical section. This crictical section allows only
 *                  one task access at a time into the task list.
 * Parameter:       iNewFlagArg - flag to determine in or out of critical section (-1/1)
 * Return:          flag
 * **************************************************************/
int TKSetPCCriticalSection( int iNewFlagArg )
{
    int iOldFlag;
    /* iPCCriticalSectionFlag = 0 that means it's not in the critical section.
     * iPCCriticalSecitonFlag > 0 that means it's in the critical section.*/
    if( !iPCCriticalSectionFlag && iNewFlagArg < 0 )
    {
        return(iPCCriticalSectionFlag);
    }
    iOldFlag = iPCCriticalSectionFlag;
    iPCCriticalSectionFlag += iNewFlagArg;
    return(iOldFlag);
}

/**************************************************************
 * Function Name:   TKGetNextTask
 * Purpose:         get the current task that active and valid. If
 *                  next task is equal to current task, then there is
 *                  no next task and return NULL. Otherwise return the
 *                  next active and valid task.
 * Returns:         current active/valid task in the tasklist or
 *                  return NULL if not active task in the list.
 * **************************************************************/
pstTaskList TKGetNextTask()
{
    pstTaskList pTaskListTemp, pCurrentTaskListTemp;
    int icount;

   /* currenttask is null, then go through the list and
    * get the first task in the list that is active */
    if( !pstCurrentTaskList )
    {
        pCurrentTaskListTemp = (pstTaskList)staTaskList;   /*get the first task in the list */
        for( icount=0;icount<MAXTASKS;icount++ )
        {
            if( pCurrentTaskListTemp->m_iActive )
                return( pCurrentTaskListTemp );         /*return the first active task in the list */
            else
                pCurrentTaskListTemp++;
        }
        pCurrentTaskListTemp = NULL;
        return pCurrentTaskListTemp;
    }
    /* current is not null, then first task in the list that valid */
    pTaskListTemp = pstCurrentTaskList + 1;
    for( icount=0;icount<MAXTASKS;icount++ )
    {
        if( (pTaskListTemp - (pstTaskList)staTaskList) >= MAXTASKS )
            pTaskListTemp = (pstTaskList)staTaskList;          /*pointed to the first task in list */
        if( pTaskListTemp->m_iValid )
            return( pTaskListTemp==pstCurrentTaskList?NULL:pTaskListTemp );
        pTaskListTemp++;
    }
    return( NULL );
}

/*********************************************************
 * Function Name:   TKGetNextWaitingTask
 * Purpose:         get the next waiting or active task from the tasklist
 *                  return the task. If there is no task in the tasklist is waiting or
 *                  active state then return NULL.
 * return:          structure pointer to next waiting task
 * *********************************************************/
pstTaskList TKGetNextWaitingTask()
{
    pstTaskList pCurrentTaskListTemp;
    int icount;

    pCurrentTaskListTemp = (pstTaskList)staTaskList;
    for( icount=0; icount<MAXTASKS; icount++ )
    {
        if( pCurrentTaskListTemp->m_iPCWaiting || pCurrentTaskListTemp->m_iActive )
        {
            return( pCurrentTaskListTemp );
        }
        else
            pCurrentTaskListTemp++;
    }
    pCurrentTaskListTemp = NULL;
    return pCurrentTaskListTemp;
}

/**************************************************************
 * Function Name:   TKGetCurrentTask
 * Purpose:         get the current running task
 * Return:          structure pointer to current task.
 * **************************************************************/
pstTaskList TKGetCurrentTask( )
{
    return( pstCurrentTaskList );
}

/**************************************************************
 * Function Name:   TKSetCurrentTask
 * Purpose:         set the task to be the current task
 * Parameter:       pTaskListArg    - task to be set to become current task.
 * return:          previous task.
 * **************************************************************/
pstTaskList TKSetCurrentTask( pstTaskList pTaskListArg )
{
    pstTaskList pTaskListTemp;

    pTaskListTemp = pstCurrentTaskList;
    pstCurrentTaskList = pTaskListArg;
    return( pTaskListTemp );
}

/***********************************************************************
 * Function Name:   TKSetCurrentMacro
 * Purpose:         Before set the entire macro entry to oc table.
 *                  Now, we only need to set the OC number.
 *                  Only keep oc number for current macro.
 * Return:          previous current macro.
 * ***********************************************************************/
int TKSetCurrentMacro( int iOpcodeArg )
{
    int iTempOpcode;

    iTempOpcode = iCurrentMacro;
    iCurrentMacro = iOpcodeArg;
    return( iTempOpcode );
}


/*****************************************************
 * Function Name:   TKKillCurrentTask
 * Purpose:         kill current task by initializing structure of the task.
 *                  such as waiting flag, active flag, valid flag, taskpendingkilled flag to FAlSE,
 *                  set the timer counter to 0, set the top of stack of current task points to the
 *                  begining of stack of current task and then set the current task pointer to NULL.
 * Arguments:       iFailureFlag - indicates if the task is getting killed because of the macro failure
 * Returns:         none.
 * ******************************************************/
void TKKillCurrentTask(int iFailureFlagArg)
{
    int iFoundOtherActiveTask = FALSE, iCounter;
    pstProgStatus  pstkTempStatus, pstkTempTop;
    char caLineNumbers[50];

    TKSetPCCriticalSection( 1 );
    /* kill the current task by reset the all the flag to default value */
    if( pstCurrentTaskList )
    {
        // macro failure, record the line path of the failure
        if(iFailureFlagArg)
        {
            //find the number of task failed
            for(iCounter = 0; iCounter < MAXTASKS; iCounter++)
            {
                if( pstCurrentTaskList == &staTaskList[iCounter] )
                    break;
            }
            pstkTempStatus = pstCurrentTaskList->m_pStatus;
            pstkTempTop = pstCurrentTaskList->m_pStackTop;
            caLineNumbers[0] = '\0';

            //search through the list of macro's nested statuses for the current line_numbers
            MAFindPath(pstkTempStatus, pstkTempTop, caLineNumbers);
            // store the particular fail message into the global variable so it can be later retrieved when needed
//            itoa(staTaskList[iCounter].m_NextPC->line_number, caLineNumbers + strlen(caLineNumbers), 10);
	    sprintf( caLineNumbers+strlen(caLineNumbers), "%d", staTaskList[iCounter].m_NextPC->line_number);
            sprintf( caTaskStatus[iCounter], "Task #%d failed at %s\r\n", iCounter, caLineNumbers );
        }

        pstCurrentTaskList->m_iValid = FALSE;
        pstCurrentTaskList->m_iPCWaiting = 0;
        pstCurrentTaskList->m_ulTimerCounter = 0;
        pstCurrentTaskList->m_iActive = FALSE;
        pstCurrentTaskList->m_iTaskPendingKilled = FALSE;
        pstCurrentTaskList->m_pStackTop = pstCurrentTaskList->m_pStatus;
        pstCurrentTaskList = NULL;
    }
    TKSetPCCriticalSection( -1 );
}

/**************************************************************
 * Funciton Name:   TKSetNoTaskFlag
 * Purpose:         set no task flag. This flag is signal either there is multi-tasking
 *                  or no multi-tasking.
 * Parameter:       iNoTaskFlagArg - flag value to set.(true/false)
 * Returns:         none.
 ***************************************************************/
void TKSetNoTaskFlag(int iNoTaskFlagArg)
{
    iNoTaskFlag = iNoTaskFlagArg;
}

/**************************************************************
 * Function Name:   TKReadNoTaskFlag
 * Purpose:         get no task flag
 * Return:          value of the flag (true/false)
 * ***************************************************************/
int TKReadNoTaskFlag()
{
    return( iNoTaskFlag );
}

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
int TKKillTask(int iOperandFlagArg, int iTaskIdArg, instr_ptr NextInstrArg)
{
    int iCount;

    TKSetPCCriticalSection( 1 );

    /* if no task id is specified, then kill the all the task */
    if (!iOperandFlagArg)
    {
        for( iCount=0;iCount<MAXTASKS;iCount++ )
        {
            staTaskList[iCount].m_iValid = FALSE;
            staTaskList[iCount].m_iPCWaiting = 0;
            staTaskList[iCount].m_ulTimerCounter = 0;
            staTaskList[iCount].m_iActive = FALSE;
            staTaskList[iCount].m_iTaskPendingKilled = FALSE;
            staTaskList[iCount].m_pStackTop = staTaskList[iCount].m_pStatus;
            caTaskStatus[iCount][0] = '\0';
        }
        TKSetNoTaskFlag(TRUE);
        pstCurrentTaskList = NULL;
        goto exit_point;
    }

    if( iTaskIdArg>=0 && iTaskIdArg<MAXTASKS )
    {
        if( staTaskList[iTaskIdArg].m_iActive )
        {
            staTaskList[iTaskIdArg].m_iTaskPendingKilled = TRUE;
            staTaskList[iTaskIdArg].m_iValid = FALSE;
            staTaskList[iTaskIdArg].m_iPCWaiting = 0;
            staTaskList[iTaskIdArg].m_ulTimerCounter = 0;
            staTaskList[iTaskIdArg].m_iActive = FALSE;
            caTaskStatus[iTaskIdArg][0] = '\0';

            if ( TKGetNextTask() == NULL )
            {
                pstCurrentTaskList = NULL;
                /* if the task was killed and next task or no more task is currently running, then
                 * the no-task-flag should get set back to normal mode (no multi-tasking). Which means giving
                 * signal that there is no macro is currently running. So the status will update correctly, or
                 * else the status will still so the macro is still running but there was nothing running.*/
                TKSetNoTaskFlag(TRUE);
            }

            if( NextInstrArg )
            {
                staTaskList[iTaskIdArg].m_NextPC = NextInstrArg;
            }
            else
            {
                staTaskList[iTaskIdArg].m_NextPC = MRGetPC();
            }
        }
        goto exit_point;
    }

    TKSetPCCriticalSection( -1 );
    return FAILURE;
exit_point:
    TKSetPCCriticalSection( -1 );
    return SUCCESS;
}

/*************************************************************************
 * Function Name:   TKListActiveTasks
 * Purpose:         list all active tasks from the list is currently running.
 *                  (for ex_TSKL)
 * return:          SUCCESS or FALSE
 * *************************************************************************/
int TKListActiveTasks()
{
    int icount, iNoTask = TRUE;
    char caBuf[50];
    pstProgStatus pstkTempStatus, pstkTempTop;
    char caLineNumbers[50];

    TKSetPCCriticalSection( 1 );
    // we are checking for the statuses of the all tasks whether some of them failed or no
    // if there was a failure we print out to the screen the Failure message including the
    // path to the line number where the task failed
    for(icount = 0; icount < MAXTASKS; icount++)
    {
        if( caTaskStatus[icount][0] != '\0')
        {
            sprintf( caBuf, "%s", caTaskStatus[icount] );
            SERPutsTxBuff( SERGetCmdPort(), caBuf);
        }
    }
    caBuf[0] = '\0';
    for( icount=0;icount<MAXTASKS;icount++ )
    {
        pstkTempStatus = staTaskList[icount].m_pStatus;
        pstkTempTop = staTaskList[icount].m_pStackTop;
        caLineNumbers[0] = '\0';

        if( staTaskList[icount].m_iActive )
        {
            iNoTask = FALSE;        /*there are active tasks */

            // add the string with the path of all nested macros to the current line number string
            MAFindPath(pstkTempStatus, pstkTempTop, caLineNumbers);
//            itoa(staTaskList[icount].m_NextPC->line_number, caLineNumbers + strlen(caLineNumbers), 10);
	    sprintf( caLineNumbers+strlen(caLineNumbers), "%d", staTaskList[icount].m_NextPC->line_number);
            sprintf( caBuf, "Task #%d at %s:", icount, caLineNumbers );
            SERPutsTxBuff( SERGetCmdPort(), caBuf);
            MRListInstructions(staTaskList[icount].m_NextPC,FALSE);
        }
    }
    /* no active task */
    if( iNoTask )
        SERPutsTxBuff(SERGetCmdPort(), "No Active Task\r\n");
    SERFlushTxBuff(SERGetCmdPort());

    TKSetPCCriticalSection( -1 );
    return SUCCESS;
}

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
int TKMakeTask(int iOperandFlagArg, int iTaskIdArg, instr_ptr NextInstrArg)
{
    int icount;
    pstTaskList pTaskListTemp;
    long lCfTemp;
    instr_ptr PCTemp;
    int iReturn = FAILURE;

    lCfTemp = MRGetCf();
    PCTemp = MRGetPC();
    TKSetPCCriticalSection( 1 );
    pTaskListTemp = pstCurrentTaskList;

    if (!iOperandFlagArg)              /* no operand defaults to use next avail */
    {
        for( icount=0;icount<MAXTASKS;icount++ )
        {
            if( !staTaskList[icount].m_iValid )
            {
                pTaskListTemp = (pstTaskList)&(staTaskList[icount]);
                pTaskListTemp->m_iValid = TRUE;
                pTaskListTemp->m_iActive = TRUE;
                pTaskListTemp->m_iTaskPendingKilled = FALSE;
                pTaskListTemp->m_iPCWaiting = FALSE;
                pTaskListTemp->m_NextPC = PCTemp;
                pTaskListTemp->m_iCount = 0;
                pTaskListTemp->p_MOwnerTbl = iCurrentMacro;
                pTaskListTemp->m_lcf = lCfTemp;
                pTaskListTemp->m_pStackTop = pTaskListTemp->m_pStatus;
                if( pstCurrentTaskList )            /* If current task, assume TSKM called */
                {                                   /* by another macro */
                    MRPopPC(pstCurrentTaskList);    /* returns to the calling macro */
                    if( PCTemp)
                        pstCurrentTaskList->m_NextPC = PCTemp;
                    pstCurrentTaskList->m_lcf = lCfTemp;
                }
                else
                    pstCurrentTaskList = pTaskListTemp;
                if( TKReadNoTaskFlag() )
                    TKSetNoTaskFlag(FALSE);
                iReturn = SUCCESS;
                goto exit_point;
            }
        }
        goto exit_point;
    }
    /* To make task specified by taskid */
    if( iTaskIdArg>=0 && iTaskIdArg<MAXTASKS )
    {
        if( !staTaskList[iTaskIdArg].m_iValid )
        {
            pTaskListTemp = (pstTaskList)&(staTaskList[iTaskIdArg]);
            pTaskListTemp->m_iValid = TRUE;
            pTaskListTemp->m_iActive = TRUE;            /* make the task active. */
            pTaskListTemp->m_iTaskPendingKilled = FALSE;
            pTaskListTemp->m_iPCWaiting = FALSE;
            pTaskListTemp->m_NextPC = NextInstrArg;
            pTaskListTemp->m_iCount = 0;
            pTaskListTemp->m_lcf = lCfTemp;
            pTaskListTemp->p_MOwnerTbl = iCurrentMacro;
            pTaskListTemp->m_pStackTop = pTaskListTemp->m_pStatus;
            if( pstCurrentTaskList )            /* If current task, assume TSKM called */
            {                                   /* by another macro*/
                MRPopPC(pstCurrentTaskList);    /* returns to the calling macro */
                if( PCTemp)
                    pstCurrentTaskList->m_NextPC = PCTemp;
                pstCurrentTaskList->m_lcf = lCfTemp;
            }
            else
                pstCurrentTaskList = pTaskListTemp;  /* set the task just made become the current task if
                                                      * there is no current task. */
            /* reset the flag to become multi-tasking if the flag has not got set back.
             * The flag with TRUE value:    no multi-tasking
             * The flag with FALSE value:   with multi-tasking */
            if( TKReadNoTaskFlag() )
                TKSetNoTaskFlag(FALSE);
            iReturn = SUCCESS;
            goto exit_point;
        }
    }
    else
    {
       MREndMacro( 0 );
    }
exit_point:
    TKSetPCCriticalSection( -1 );
    return iReturn;
}

/**********************************************************
 * Function Name:   TkGetTaskId
 * Purpose:         get the task id (for ex_TSKI)
 * return:          SUCCESS or FAILURE
 * **********************************************************/
int TKGetTaskId( )
{
    return iTaskID;
}

/***********************************************************
 * Function Name:   TkSetTaskId
 * Purpose:         Set the task id (for ex_TSKI)
 * return:          None
 * **********************************************************/
void TKSetTaskId( int iTaskIdArg )
{
    iTaskID = iTaskIdArg;
}

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
int TKWait(long lTimeArg, instr_ptr InstrArg)
{
    unsigned long time;
    pstTaskList ptl;
	int iReturn = SUCCESS;
	ULONG ulTime;

    /* get the current task and if there is a current task and the task is already in the waiting state
     * then turn off the timer interrupt and exit the function. If the task is not at waiting state then
     * set the task state to be waiting and set the timer counter for waiting.  Also set it's state to be
     * not active, turn on the task timer interrupt for switching tast, turn on the timer interrupt and exit
     * the function.*/
    ptl = TKGetCurrentTask( );
    if( ptl )
    {
//printf("current task: ptl\n");
        if( ptl->m_iPCWaiting )
        {
		ulTime = TIGetTimerVals(SYS_TIMER);
		if(ulTime > ptl->m_ulTimerCounter)
		 	ptl->m_iPCWaiting = FALSE;
		else
		{
			MASetPC(InstrArg);
			if(TKGetNextTask())
        		TISetTimerVals(TIMER_INTERRUPT, TRUE);
		}
		goto exit_point;
        }
        time = (unsigned long) (lTimeArg);
        if (time > UINT_MAX)
        {
		iReturn = FAILURE;
		goto exit_point;
        }
	ulTime = TIGetTimerVals(SYS_TIMER);
        ptl->m_ulTimerCounter = (unsigned long)time + ulTime;
        ptl->m_iPCWaiting = TRUE;
        ptl->m_NextPC = InstrArg;
        ptl->m_lcf = MRGetCf();
	MASetPC(InstrArg);
        TISetTimerVals(TIMER_INTERRUPT, TRUE);
	goto exit_point;
    }
    /* if there is no current task and no multi-tasking, then check current PC is at waiting state or not.
     * if it's not at waiting state then set timer counter, set the state to be waiting state and re-run the
     * instruction again until the timer is up. Otherwise, it's at waiting state, then check for timer if the
     * the time is up then set the waiting state to FALSE else re-run the instruction again.*/

giDispTime = 0;

    if (!MRGetMacroIntegerVars(MR_PC_WAITING))
    {
        time = (unsigned long) (lTimeArg);
if(giDispTime)
printf("wait time=%ld\n",time);
        if (time > UINT_MAX)
        {
		iReturn = FAILURE;
		goto exit_point;
        }
        // TISetCounter(iTimerForTKWait, (unsigned) time);
	MainTimerSet(iTimerForTKWait, (long)time);
        MASetMacroIntegerVars(MR_PC_WAITING, TRUE);
        MASetPC(InstrArg);     /* repeat this instruction */
    }
    else           /* at waiting state already */
    {
        //if (!TICountExpired(iTimerForTKWait))
	if(!MainTimerExpired(iTimerForTKWait))
            MASetPC(InstrArg);     /* repeat this instruction */
        else
	{
            MASetMacroIntegerVars(MR_PC_WAITING, FALSE);      /* time up */
if(giDispTime)
printf("wait time=%ld Expired\n", lTimeArg);
	}
    }

exit_point:
giDispTime = 0;

    return iReturn;
}

/**************************************************************
 * Function Name:   TKCheckTask
 * Purpose:         check for the waiting and valid task. Then
 *                  if counter time < luSysTimer then set the
 *                  task to be active and no waiting.
 * Parameter:       luSystimer - the time for check to switch to anther
 *                               task.
 * returns:         none
 * **************************************************************/
void TKCheckTask(long unsigned luSysTimer)
{
    pstTaskList pTaskListTemp;
    int iCount;

    pTaskListTemp = (pstTaskList)staTaskList;
    for( iCount=0; iCount<MAXTASKS; iCount++ )
    {
        if( pTaskListTemp->m_iPCWaiting && pTaskListTemp->m_iValid )
        {
            if( pTaskListTemp->m_ulTimerCounter < luSysTimer )
            {
                pTaskListTemp->m_iActive = TRUE;
                pTaskListTemp->m_iPCWaiting = FALSE;
            }
        }
        pTaskListTemp++;
    }
}
