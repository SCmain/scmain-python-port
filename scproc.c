/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * Original Author      : (Deceased)
 * Current Maintainer   : gsancosme (georges@sancosme.net)
 * Maintained Since     : 13.01.2025
 * Created On           : 04.06.2007
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Program:     Controller firmware
 * File:        scproc.c
 * Functions:   PCProcessCmdLine
 *              PCProcessMacroInstr
 *              PCProcessExecuteInstr
 *              PCSetPromptMode
 *              PCGetWDPrompt
 *
 * Description: handles processing command line instructions and macro
 *              instructions, executing a specific instruction .
 *              Also, handling interrupt macro checking and executing.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
\***************************************************************/

#include <malloc.h>
#include <memory.h>
#include <string.h>

#include "scproc.h"
#include "cmdfns.h"
#include "sck.h"
#include "sctag.h"
#include "scmac.h"
#include "ser.h"
#include "scstat.h"
#include "sctim.h"
#include "ro.h"
#include "scintr.h"
#include "gag.h"
#include "secsg.h"

char cWDPrompt = '$';
int iPromptMode = TRUE;
//#ifdef SX
instr_t secs_special_instr;
//#endif

long glRobotIdleStart;

/************************************************
 * Function Name:   PCGetWDPrompt
 *
 * Description:     interface for getting the prompt to be used after
 *                  a watchdog timer timeout occurs. This prompt
 *                  only replaces the '>', not the '?' or '_ERR'.
 *
 * Parameter:       none
 * Returns:         post-watchdog timer prompt prompt.
 *
 * ************************************************/
char PCGetWDPrompt()
{
    return cWDPrompt;
}

/************************************************
 * Function Name:   PCSetPromptMode
 *
 * Description:     interface for setting the prompt mode value.
 *                  This value tells which mode the prompt should print when error occur.
 *                  Novellus or regular mode.
 *
 * Parameter:
 *                  iModeArg    - new mode value
 *
 * Returns:         none
 *
 * ************************************************/
void PCSetPromptMode(int iModeArg)
{
    iPromptMode = iModeArg;
}

/*******************************************************************
 * Function Name:   PCProcessCmdLine
 * Purpose:         Handling the process of the command line such as parsing,
 *                  validating and executing.
 *                  First, check for is there a multi-tasking running, if there is, then check
 *                  is the task pending killed; if so, just exit the function.  Otherwise,
 *                  setting the executing macro flag to false (no macro is running). Then get the
 *                  command line form COM Port by calling SERGetsRxBuff from comm module.  When you call
 *                  SERGetsRxBuff, make sure to specify number of byte that you want to get from the
 *                  COM buffer.  If you want to get the whole string with ending of "\r", then set the number
 *                  of bytes to -1.  Make sure to initialize the instruction pointer before pass into the function
 *                  (CMDcmndLineAction())for parsing the command. Then passing the whole command line to low-level
 *                  module to parse and validate the command. If the parsing and validation are success,
 *                  then the function will return the instruction which is the instruction to be executed.
 *                  After finished executing the instruction, if there is instruction
 *                  in the current PC and there is current task, then set the current PC to be next
 *                  execution instruction in the current task. Remember to free the operand from the
 *                  instruction after executed the instruction. if success executed, it will print
 *                  "\r\n>", else if it is failure from validating or parse then print out "\r\n?". If the
 *                  command line contains only "\r" then it will consider as blank line and print out
 *                  "\r\n>". The prompt is printed also depending on the mdoe you are on such as
 *                  regular or Brook emulator (NOVELLUS). For NOVELLUS, if it is failure from validating or
 *                  parsing then print out error code.
 *                  This function is called from the mainloop.
 * Parameter:       none
 *
 * Returns:         none
 * ********************************************************************/
void PCProcessCmdLine()
{
    int iRc;                            /* holds return code */
    static char caCmdTemp[MAXLINE*2];   /* holds the command line that get from the COM */
    instr_ptr instr;                    /* instruction pointer for holding an instruction
                                         * or line of executing code of level command or macro */
    pstTaskList  pCurrentTaskListTemp;  /* temporary pointer for task list */
    int iPortNum;                       /* COM port id */
    int iNumOfBytes;                    /* number of bytes want to read from the com buffer */
    int iEmulFlags;                     /* emulator flag */
    instr_ptr instrTemp;
    instr_t cmd_line_instr;             /* holds temporary location of instruction, so can use
                                         * this address to initialize the instruction pointer. Since
                                         * this instruction is already allocated. */
    OC_TABLE OCEntry;                   /* opcode table entry corresponding to the current instruction */

    caCmdTemp[0] = 0;                   /* initialize the buffer */

    TKSetPCCriticalSection( 1 );
    /* get current task  and check to make it's pendingkilled*/
    if( (pCurrentTaskListTemp = TKGetCurrentTask()) )
    {
        if( pCurrentTaskListTemp->m_iTaskPendingKilled)
        {
            TKSetPCCriticalSection( -1 );
            return;
        }
    }
    /* set the executing macro flag to false since you are executing
     * command line */
    MASetMacroIntegerVars(MR_EXECUTING_MACRO, FALSE);
    instr = (instr_ptr) &cmd_line_instr;  /* initialize the instruction pointer before pass
                                           * in to the decode function (CMDcmndLineAction) to get back the instruction.
                                           * If you don't initalize then you have to allocate memory for it,
                                           * or else you get into memory problem. */
    iPortNum = SERGetCmdPort();
    iNumOfBytes = -1;                   /* initialize this to -1 to indicate that you will get everything from COM buffer. */

    /* get the command line from the COM */
    SERGetsRxBuff( iPortNum, caCmdTemp, FALSE, &iNumOfBytes, FALSE );
    if (iNumOfBytes <= 0)
    {
	iRc = 0;
	goto exit;
    }

    /* as you already intialize instr, then you can pass into this function, so
     * the function will return an instruction pointer after decode the command.
     * In CMDcmndLineAction, beside parsing it also validate the command.*/
    iRc = CMDcmndLineAction(caCmdTemp, instr, FALSE);
//#ifdef SX
    if (instr->OC == SECS)
    {
        memcpy(&secs_special_instr, instr, sizeof(instr_t));
        instr = (instr_ptr) &secs_special_instr;
        S2SetSecsSpecial(TRUE);
    }
//#endif
    switch (iRc)
    {
        /* if parsing and validating are success then start to execute the command */
        case MACRO:
	case SUCCESS:
            iRc = MRExecuteInstructions(instr, FALSE);  /* executing instruction */

	    // Check for System Idle Status if command processed is not STAT and GLST
	    // Host may keep interrogating STAT or GLST, all others keep system alive
	    //
	    if (instr->OC != STAT && instr->OC != GLST)
	    {
            	glRobotIdleStart = TIRSTime();
//printf("scproc macro idlestart=%ld\n",glRobotIdleStart);
	    }
            /* if there is instruction in the current PC and there is current task
             * set the current PC to be next execution instruction in the current task */
            instrTemp = MRGetPC();
            if( instrTemp && ( pCurrentTaskListTemp = TKGetCurrentTask()))
                pCurrentTaskListTemp->m_NextPC = instrTemp;
//#ifdef SX
            if(instr->OC != SECS)
                CMDfreeOperands(instr->opr_ptr);  /* free the operand after executing the command */
//#endif
            break;

	case BLANKLINE:          /* blank line */
            iRc = 0;
            instr->OC = 0;      /* dummy value so prompt will print */
            break;

	case FAILURE:
	default:
		    SSSetStatusWord(CMD_NOT_EXED, TRUE);
            break;
    }

    iPortNum = SERGetCmdPort();
    /* Output a prompt after all commands except start of a macro */
    /* (Note: iRc can be set by either CMDcmndLineAction or MRExecuteInstruction) */
    iEmulFlags = MRGetMacroIntegerVars(MR_EMULATOR_FLAG);

    /* print prompt depend on with emulator mode.  Emulator mode is just another name to
     * distinguish between difference system and customer special requirement.
     * For Novellus which the emulator mode is DFEMULATORB:
     *      if there is an error, then the prompt will be shown the error code.
     *      if success, the prompt will be shown "\r\n>".
     *      In the case of exit from teach pedant by typing ENDTR command, the prompt will be
     *      shown "\r>". There is no line feed.
     * For others:
     *      if there is an error, the prompt will be shown the "?"
     *      if success, the prompt will be shown "\r\n>". */
    if(iPromptMode)
    {
        /* in the case of error */
        if (iRc)
        {
            /* Novellus brook simulation mode */
            if (iEmulFlags&DFEMULATORB)
            {
                 SERPutsTxBuff(iPortNum, "_ERR 0303\r" );
            }
            else
            {
                /* regular mode */
                SERPutsTxBuff(iPortNum, "\r\n?");
            }
        }
        else
        {
            CMDcommands(CMDGET, instr->OC, &OCEntry);     /* get macro entry from the table */
            /* in the case of executing successfully */
            if ((instr->OC != INVALID_OPCODE) && !(OCEntry.flag & CMD_DEFINE_MACRO))
            {
                /* if Novellus mode and execute ENDTR command then show "\r>" */
                if((instr->OC == ENDTR) && (iEmulFlags&DFEMULATORB))
                {
                    if (TIGetTimerVals(LOCKED) == 123)
                        sprintf(caCmdTemp, "\r%c", PCGetWDPrompt());
                    else
                        sprintf(caCmdTemp, "\r>");
                }
                /* for all cases include Novellus mode exception of ENDTR command */
                else
                {
                    if (TIGetTimerVals(LOCKED) == 123)
                        sprintf(caCmdTemp, "\r\n%c", PCGetWDPrompt());
                    else
                        sprintf(caCmdTemp, "\r\n>");
                }
                SERPutsTxBuff(iPortNum, caCmdTemp);
            }
        }
    }
    else
    {
        if (iRc)
            SERPutsTxBuff(iPortNum, "_ERR 0303\r" );
    }
    SERFlushTxBuff(iPortNum);
exit:
    TKSetPCCriticalSection( -1 );

    return;
}

/***********************************************************************
 * Function Name:   PCProcessMacroInstr
 *
 * Purpose:         Handling the execution of macro in the mainloop.
 *                  Frist, check if there is no interrupt macro running, then come in to the interrupt section to
 *                  check for any interrupt that need to execute the interrupt macro. Durring the interrupt macro is
 *                  running, no interrupt can execute.  In the case, If There is an interrupt but the enable flag is off,
 *                  then interrupt needs to set back the flag to off.  So we can avoid of other interrupt come in.
 *                  This situation happens to all the interrupt except for COM2 and COM3 interrupt. Then if no
 *                  multi-tasking is make, executing the next command that got push when interrupt macro is
 *                  executing.  If there is multi-tasking , the current task is not waiting then executing the
 *                  current task.  However, before executing, make sure the timer interrupt of the task is not up yet.
 *                  If the timer interrupt is up and not interrupt signal, then switch to next task that is active.
 *                  Then execute the task.  Other thing, if the interrupt macro is executing and critical interrupt section is
 *                  TRUE then you don't allow to switch the task.
 *
 * Parameter:       none
 *
 * Returns:         none
 *
 * ************************************************************************/
void PCProcessMacroInstr()
{
    pstTaskList pCurrentTaskListTemp;   /* holds temporary current pointer of task list */
    pstTaskList pNextTaskListTemp;      /* holds temporary next pointer of task list */
    int iCurrentPendingKilled;          /* flag for current pending killed task */
    int iInterruptType = FAILURE;       /* holds interrupt type for interrupt macro */
    int iDCMode;                        /* Daisy chain mdoe flag */
    instr_ptr instrTemp;
	int iTime;

    /* if there is no interrupt macro is running then come in to check for any interrupt.
     * Durring the interrupt macro is running, no interrupt can execute.
     * There is an interrupt but the enable flag is off, then
     * interrupt flag needs to get set back to off. So we can avoid of
     * other interrupt comes in. This situation happens to all the interrupt execept for
     * COM2 and COM3 interrupt.*/
    if ( !MRGetMacroIntegerVars(MR_INTR_SECT_FLAG) )
    {
        SERGetCommVals(NO_PORT_NUM, DC_MODE, &iDCMode);
        /* CMER interrupt. The interupt is generated when executing macro and one of the
         * intruction in the macro is error. Then the CMER macro will execute if CMER exists.
         * After finished executing the macro will stop. */
        if (MRGetMacroIntegerVars(MR_CMER_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_CMER_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = CMER_TYPE;
            }
            else
                MASetMacroIntegerVars(MR_CMER_INTERRUPT, FALSE);
        }
        /* INTLK interrupt. The interrupt is generated when the user presses the INTLK
         * (old name EMO) button. INTLK interrupt has priority over the SSTOP. One INTLK
         * is buffered, next are ignored. The INTLK and SSTOP macros can be overriden by themselves. */
        else if (MRGetMacroIntegerVars(MR_INTLK_INTERRUPT))
        {
            ROEmergencyOff(FALSE);

            if(MRGetMacroIntegerVars(MR_INTLK_ENABLED))
            {
                MASetMacroIntegerVars(MR_PENDING_INTLK, FALSE);
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = INTLK_TYPE;
            }
            else
                MASetMacroIntegerVars(MR_INTLK_INTERRUPT, FALSE);
        }
        /* SSTOP interrupt. The interrupt is generated when the user presses the STOP button.
         * INTLK interrupt has priority over the SSTOP. One SSTOP is buffered, next are ignored. */
        else if (MRGetMacroIntegerVars(MR_SSTOP_INTERRUPT))
        {
            /* Send Stop Motion to Galil */
            ROStopAllFromIntr();

            if(MRGetMacroIntegerVars(MR_SSTOP_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                MASetMacroIntegerVars(MR_PENDING_SSTOP, FALSE);
                iInterruptType = SSTOP_TYPE;
            }
            else
                MASetMacroIntegerVars(MR_SSTOP_INTERRUPT, FALSE);
        }
        /* ERR interrupt. The interrupt is generated when any axis is in the error state.
         * Example: if the robot is servo on, then you hit the robot and servo is off. At that
         * time the interrupt is generated and if you have the ERR macro, then macro will execute */
        else if (MRGetMacroIntegerVars(MR_ERR_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_ERR_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = ERR_TYPE;
            }
            else
                 MASetMacroIntegerVars(MR_ERR_INTERRUPT, FALSE);
        }
        /* LMM interrupt. The interrupt is generate only when robot is moving and hit the limit switch.
         * Then execute the LMM macro if the macro exists.  The interrupt will not generate if you move the robot
         * manually to limit switch.*/
        else if (MRGetMacroIntegerVars(MR_LMM_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_LMM_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = LMM_TYPE;
            }
            else
                MASetMacroIntegerVars(MR_LMM_INTERRUPT, FALSE);
        }
        else if (MRGetMacroIntegerVars(MR_DER_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_DER_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = DER_TYPE;
printf("DER interrupt = %d\n", MRGetMacroIntegerVars(MR_DER_INTERRUPT));
            }
            else
                MASetMacroIntegerVars(MR_DER_INTERRUPT, FALSE);
        }
        /* SECS interrupt. The interrupt is generated when SECS message is received
         * from COM3 or COM2 */
        else if(MRGetMacroIntegerVars(MR_SECS_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_SECS_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = SECS2_TYPE;
            }
            else
                MASetMacroIntegerVars(MR_SECS_INTERRUPT, FALSE);
        }
        else if(MRGetMacroIntegerVars(MR_SECS2_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_SECS_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = SECS2_TYPE2;
            }
            else
                MASetMacroIntegerVars(MR_SECS2_INTERRUPT, FALSE);
        }
        else if(MRGetMacroIntegerVars(MR_SECS3_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_SECS_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = SECS2_TYPE3;
            }
            else
                MASetMacroIntegerVars(MR_SECS3_INTERRUPT, FALSE);
        }
        else if(MRGetMacroIntegerVars(MR_SECS4_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_SECS_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = SECS2_TYPE4;
            }
            else
                MASetMacroIntegerVars(MR_SECS4_INTERRUPT, FALSE);
        }
        /* IIM interrupt. The interrupt is generated when one of the customer changed one of
         * the cutomized I/O (port H).*/
        else if (MRGetMacroIntegerVars(MR_IIM_INTERRUPT))
        {
            if(MRGetMacroIntegerVars(MR_IIM_ENABLED))
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = IIM_TYPE;
            }
            else
                MASetMacroIntegerVars(MR_IIM_INTERRUPT, FALSE);
        }
        /* COM2 can get interrupt only if there is daisy chain set and enabled flag is on
         * and there is something in the COM buffer. */
        else if( MRGetMacroIntegerVars(MR_COM2_ENABLED) && iDCMode )  /* COM2 interrupt */
        {
            /* check if there is any data in the COM buffer. */
            if(SERRxBuffAvail(COM2) > 0)
            {
                MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
                iInterruptType = DC_MODE_TYPE;
            }
        }
        /* COM3 can get interrupt only when there is something in the buffer and enabled flag is on*/
        else if( MRGetMacroIntegerVars(MR_COM3_ENABLED) && SERRxBuffAvail(COM3)>0 )  /* COM3 interrupt */
        {
            MAClearMacroNestLevel(); /* Macro nest level is cleared when interrupt macro is incurred */
            iInterruptType = COM3_TYPE;
        }

        /* the iInterruptType is not -1 that means there is an interrupt. Then execute
         * the interrupt macro.*/
        if( iInterruptType != FAILURE )
        {
            /* excute the interrupt macro */
            if( PCExecuteInterrupt(iInterruptType, &iCurrentPendingKilled))
                return;
        }
    }
    else
    {
        TISetTimerVals( TIMER_INTERRUPT, FALSE );   /* if it's in critical section, don't allow to switch tasks */
    }

    /*No multi-task has been made, simply execute the next command in macro */
    instrTemp = MRGetPC();
    if (instrTemp != NULL)
    {
//#ifdef SX
	// any macro instruction execution puts system alive
    	// System Idle check starts over
    	glRobotIdleStart = TIRSTime();
//printf("procInstr idlestart=%ld\n",glRobotIdleStart);

        if ((instrTemp->OC == SECS) && S2GetSecsSpecial())
        {
            MRExecuteInstructions(instrTemp, TRUE);
            return;
        }
        else if (TKReadNoTaskFlag() && instrTemp)
        {
            MASetMacroStatus( MACRO_RUNNING);     /* keep jamming this on */
            MASetMacroIntegerVars(MR_EXECUTING_MACRO, TRUE);
            TISetTimerVals(TIMER_INTERRUPT, FALSE);
            MRExecuteInstructions(instrTemp, TRUE);
	    return;
        }
    }
//#else
//    if (TKReadNoTaskFlag() && instrTemp)
//    {
//        MASetMacroStatus( MACRO_RUNNING);     /* keep jamming this on */
//        MASetMacroIntegerVars(MR_EXECUTING_MACRO, TRUE);
//        TISetTimerVals(TIMER_INTERRUPT, FALSE);
//        MRExecuteInstructions(instrTemp, TRUE);
//		return;
//    }
//#endif

    /* multi-Task feature is enabled */
    if( (pCurrentTaskListTemp = TKGetCurrentTask() ))
    {
        if( !pCurrentTaskListTemp->m_iPCWaiting )    /*This is not waiting task, execute it; else don't execute it. */
        {
            if( pCurrentTaskListTemp )
                iCurrentPendingKilled = pCurrentTaskListTemp->m_iTaskPendingKilled;
            else
                iCurrentPendingKilled = FALSE;

            /* Execute next line of any pending macros */
			iTime = TIGetTimerVals(TIMER_INTERRUPT);
            if ( instrTemp && (!iTime || iCurrentPendingKilled) )
            {
                MASetMacroStatus( MACRO_RUNNING );       /* keep jamming this on */
                MASetMacroIntegerVars( MR_EXECUTING_MACRO, TRUE );
                TISetTimerVals( TIMER_INTERRUPT, FALSE );
                MRExecuteInstructions(instrTemp, TRUE);
                instrTemp = MRGetPC();
                if( instrTemp && ( pCurrentTaskListTemp=TKGetCurrentTask() ) )
                    pCurrentTaskListTemp->m_NextPC = instrTemp;
                return;
            }
        }
    }

    /* if time is up for a task and no interrupt swap the task */
	iTime = TIGetTimerVals(TIMER_INTERRUPT);
    if ( iTime && !MRGetMacroIntegerVars(MR_INTR_SECT_FLAG) )
    {
        TKSetPCCriticalSection( 1 );
        TISetTimerVals( TIMER_INTERRUPT, FALSE );
        if( (pNextTaskListTemp = TKGetNextTask() ))  /* Swap tasks if there is next task in the list that is active */
        {
            if( pCurrentTaskListTemp )
            {
                if( pCurrentTaskListTemp != pNextTaskListTemp )
                    pCurrentTaskListTemp->m_lcf = MRGetCf();
            }
            /* executing next task */
            if( pNextTaskListTemp->m_iActive )
            {
                MASetCf( pNextTaskListTemp->m_lcf );
                MASetMacroStatus( MACRO_RUNNING);       /* keep jamming this on */
                MASetMacroIntegerVars( MR_EXECUTING_MACRO, TRUE );
				instrTemp = pNextTaskListTemp->m_NextPC;
                MASetPC(instrTemp);
                TKSetCurrentTask( pNextTaskListTemp);
                MRExecuteInstructions(instrTemp, TRUE);
                instrTemp = MRGetPC();
                if( instrTemp && ( pCurrentTaskListTemp = TKGetCurrentTask() ) )
                    pCurrentTaskListTemp->m_NextPC = instrTemp;
            }
		}
		else
		{
			if(pCurrentTaskListTemp->m_iPCWaiting==TRUE)
			{
				instrTemp = pCurrentTaskListTemp->m_NextPC;
				MASetPC(instrTemp);
				MRExecuteInstructions(instrTemp, TRUE);
				instrTemp = MRGetPC();
				if(instrTemp && (pCurrentTaskListTemp = TKGetCurrentTask()))
					pCurrentTaskListTemp->m_NextPC = instrTemp;
			}
		}
        TKSetPCCriticalSection( -1 );
    }
    return;
}

/************************************************************************
 * Function Name:   PCExecuteInterrupt
 * Purpose:         handling the executing of interrupt macro by checking which
 *                  type of interrupt macro need to be executed. Then check to make sure
 *                  insturction pointer of interrupt macro of that type is not null,
 *                  then execute.  However, if instruction pointer of that interrupt macro is
 *                  NULL, then reset the enter interrupt section flag to false. So the next
 *                  interrupt can come in to the interrupt section. Before execute the instruction
 *                  make sure to save the top of stack, so by the time you finished executing the
 *                  interrupt you can continue to execute the other macro that you have saved, but
 *                  this only happen to COM2 and COM3 interrupt macro. In the case, if the
 *                  interrupt macro is executing and there is an error, then we need to
 *                  set the enter interrupt section flag to false since the macro will never
 *                  go to end of macro since end of macro function is where we set back the interrupt
 *                  section flag to FALSE after finished executing the function. The rest of
 *                  the macro execution will continue at the mainloop in PCProcessMacroInstr().
 *
 * Parameter:       iInterruptTypeArg   - type of interrupt
 *                  iCurrentPendingKilledArg - (true/false return value)
 * return:          SUCCESS or FAILURE
 * *************************************************************************/
int PCExecuteInterrupt(int iInterruptTypeArg, int *iCurrentPendingKilledArg)
{
    pstTaskList pCurrentTaskListTemp;   /* temporary pointer for current task list */
    int iReturn;                        /* return code (success/failure)*/
    int iPC = FALSE;                    /* flag for telling the interrupt macro is exist or not. when there is an interrupt */
    instr_ptr currPC;                   /* temporary for holding current PC instruction. */

    /* check for which interrupt is generated and then set the interrutpt flag of
     * that interrupt type to false before executing the interrupt macro.*/
    switch(iInterruptTypeArg)
    {
        case IIM_TYPE:
            MASetMacroIntegerVars( MR_IIM_INTERRUPT, FALSE );
            break;

        case ERR_TYPE:
            MASetMacroIntegerVars( MR_ERR_INTERRUPT, FALSE );
            break;

        case LMM_TYPE:
            MASetMacroIntegerVars( MR_LMM_INTERRUPT, FALSE );
            break;

        case DC_MODE_TYPE:              /* COM2 interrupt  */
            MASetMacroIntegerVars( MR_COM2_ENABLED, FALSE );
            break;

       case COM3_TYPE:
            MASetMacroIntegerVars( MR_COM3_ENABLED, FALSE );
            break;

        case CMER_TYPE :
            MASetMacroIntegerVars( MR_CMER_INTERRUPT, FALSE );
            break;

       case INTLK_TYPE :
            MASetMacroIntegerVars( MR_INTLK_INTERRUPT, FALSE );
            break;

       case SSTOP_TYPE :
            MASetMacroIntegerVars( MR_SSTOP_INTERRUPT, FALSE );
            break;

       case SECS2_TYPE :
            MASetMacroIntegerVars( MR_SECS_INTERRUPT, FALSE );
            break;

       case SECS2_TYPE2 :
            MASetMacroIntegerVars( MR_SECS2_INTERRUPT, FALSE );
            break;

       case SECS2_TYPE3 :
            MASetMacroIntegerVars( MR_SECS3_INTERRUPT, FALSE );
            break;

       case SECS2_TYPE4 :
            MASetMacroIntegerVars( MR_SECS4_INTERRUPT, FALSE );
            break;

        case DER_TYPE:
            MASetMacroIntegerVars( MR_DER_INTERRUPT, FALSE );
            break;

       default:
           return FAILURE;

    }

    *iCurrentPendingKilledArg = MRGetMacroIntegerVars(MR_EXECUTING_MACRO);
    MASetMacroIntegerVars(MR_EXECUTING_MACRO, FALSE);
    /* save the current top of stack before executing the interrupt macro.
     * So by the time finished we can current top */
    //if ( TKReadNoTaskFlag())
    //    MASetSavedStkTop( MRGetStkTop() );
    //else
    //{
    //    if( pCurrentTaskListTemp = TKGetCurrentTask() )
    //        MASetSavedStkTop( pCurrentTaskListTemp->m_pStackTop);
    //}
    // Instead of saving the stack top, use a macro call nest counter

    /* executing the interrupt macro. iPC is set to True if there is interrupt macro to execute. */
    switch(iInterruptTypeArg)
    {
        case IIM_TYPE:
            currPC = MRGetMacroPC(MR_IIM_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case ERR_TYPE:
            currPC = MRGetMacroPC(MR_ERR_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                MASetMacroIntegerVars(MR_RUNNING_ERR, TRUE); /* this is set only when there is a macro defined */
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case LMM_TYPE:
            currPC = MRGetMacroPC(MR_LMM_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case DC_MODE_TYPE:
            currPC = MRGetMacroPC(MR_COM2_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

       case COM3_TYPE:
            currPC = MRGetMacroPC(MR_COM3_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case CMER_TYPE :
            MASetMacroIntegerVars(MR_RUNNING_CMER, TRUE);
            currPC = MRGetMacroPC(MR_CMER_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case INTLK_TYPE :
            currPC = MRGetMacroPC(MR_INTLK_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                MASetMacroIntegerVars(MR_RUNNING_INTLK, TRUE); /* this is set only when there is a macro defined */
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case SSTOP_TYPE :
            currPC = MRGetMacroPC(MR_SSTOP_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                MASetMacroIntegerVars(MR_RUNNING_SSTOP, TRUE); /* this is set only when there is a macro defined */
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case SECS2_TYPE :
            currPC = MRGetMacroPC(MR_SECS_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case SECS2_TYPE2 :
            currPC = MRGetMacroPC(MR_SECS2_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case SECS2_TYPE3 :
            currPC = MRGetMacroPC(MR_SECS3_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case SECS2_TYPE4 :
            currPC = MRGetMacroPC(MR_SECS4_INTERRUPT);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;

        case DER_TYPE:
            currPC = MRGetMacroPC(MR_DER_INTERRUPT);
printf("derPC = %x\n",currPC);
            if(currPC)
            {
                iPC = TRUE;
                iReturn = MRExecuteInstructions( currPC, FALSE );
            }
            break;
        default:
            iReturn = FAILURE;
            break;
    }
    if(iReturn == FAILURE )
	{
        MASetMacroIntegerVars(MR_EXECUTING_MACRO, *iCurrentPendingKilledArg);
        /* in this case the execute the instruction is failure, we
         * need to reset iEnterinterruptsectionflag. So the next interrupt
         * can get into the interrupt section.*/
        MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);
		return FAILURE;
	}
    /* if iPC is true then set the Interrupt section flag to true, so
     * no interrupt can be executed. However, if iPC is false then we neeed to reset the
     * interrupt seciton to same case as executing interrupt macro and fail.*/
    if(iPC)
        MASetMacroIntegerVars(MR_INTR_SECT_FLAG, TRUE);
    else
        MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);

    currPC = MRGetPC();
    if( currPC && (pCurrentTaskListTemp = TKGetCurrentTask() ) )
        pCurrentTaskListTemp->m_NextPC = currPC;

	return SUCCESS;
}
