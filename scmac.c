/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * Original Author      : Canh Le (Deceased)
 * Current Maintainer   : Georges Sancosme (georges@sancosme.net)
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
 * Program:     smart controller macro
 * File:        scmac.c
 * Functions:   MAInit
 *              MAInitMacros
 *              MRMacroDefine
 *              MRTakeBody
 *              MAInsertMacro
 *              MRListMacroNames
 *              MRListMacro
 *              MADeleteMacro
 *              MADeletedMacroScan
 *              MAFreeInstructions
 *              MAInitStack
 *              MRPushPC
 *              MRPopPC
 *              MRLaunchMacro
 *              MRWriteMacros
 *              MRWriteBody
 *              MRWriteBytes
 *              MRWriteCheckSum
 *              MRLoadMacros
 *              MAReadMacros
 *              MAReadBody
 *              MAReadInstr
 *              MAReadBytes
 *              MAReadCheckSum
 *              MASaveMacrotoNvsram
 *              MRLoadMacroFromNvsram
 *              MRPreLaunchMacro
 *              MRExecuteMacro
 *              MRExecuteInstructions
 *              MRListInstructions
 *              MRAbortMacro
 *              MAFindOpcode
 *              MREndMacro
 *              MAReadMacroLineNumber
 *              MRGetPStatus
 *              MRGetStkTop
 *              MRGetStkTopBeforeInit
 *              MRGetSavedStkTop
 *              MASetSavedStkTop
 *              MRGetNumberOfMacro
 *              MASetNumberOfMacro
 *              MRGetFreezeMacro
 *              MASetFreezeMacro
 *              MRGetAbortLineNumber
 *              MASetAbortLineNumber
 *              MRGetCf
 *              MASetCf
 *              MRGetMacroStatus
 *              MASetMacroStatus
 *              MRGetPC
 *              MASetPC
 *              MASetFlagsToFalse
 *              MRGetMacroAvoidRecursive
 *              MADisplayMacroTrace
 *
 * Description: Handling the macro programming. Create or define, list, save, and load macros.
 *              Handling the all the interrupt macro. Execution of macro and command line instruction.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "sck.h"
#include "ser.h"
#include "scmac.h"
#include "maclab.h"
#include "sctag.h"
#include "scstat.h"
#include "scregg.h"
#include "scver.h"
#include "cmdfns.h"
#include "cmdval.h"
#include "cmdex.h"
#include "cmdmac.h"
#include "cmdsp.h"
#include "sctim.h"
#include "mapio.h"
#include "fiog.h"
#include "scintr.h"
#include "scmem.h"
#include "secsg.h"

int iDefineFlag, iEmulatorFlag;         /* flag for system configuration  */
int iMod2;                              /* for checksum algorithm use. It's initialized in MASaveMacroToNvsram and
                                         * MRLoadMacroFromNvsram.*/
FILE *iFilePointer;                       /* file pointer to open,read, and write to file */
unsigned uMacroCheckSum;                /* Macro checksum status */
unsigned uNumberOfBytes;
char caMacroAvoidRecursive[10];         /* holds name of macro just define and durring macro defining,
                                         * it can not call itself. */

stProgStatus staPStat[MAXSTACK];
pstProgStatus psStkTop;               /* points to current top of stack. Always points to next open slot */
pstProgStatus psStkTopBeforeInit = NULL;  /* save the top of stack before initalize the stack in the case of error.
                                           * Mostly, using for RLN. */
pstProgStatus psSavedStkTop = NULL;

instr_ptr   PC;                         /* holds current instruction to execute */
instr_ptr   oldPC;                      /* holds the last execution instruction. Needed only for RLN command */
instr_ptr   tempOldPC;                  /* holds empty location to initialize the OldPC pointer
                                         * when the macro is running successfully. For RLN only. */
instr_ptr   errPC;                      /* ERR interrupt macro */
instr_ptr   iimPC;                      /* IIM interrupt macro */
instr_ptr   lmmPC;                      /* LMM interrupt macro */
instr_ptr   derPC;                      /* DER interrupt macro */
instr_ptr   com2PC;                     /* COM2 interrupt macro */
instr_ptr   com3PC;                     /* COM3 interrrupt macro */
instr_ptr   secsPC;                     /* SECS interrupt macro */
instr_ptr   secsPC2;                     /* SECS interrupt macro */
instr_ptr   secsPC3;                     /* SECS interrupt macro */
instr_ptr   secsPC4;                     /* SECS interrupt macro */
instr_ptr   iim7PC;                     /* For SV only */
instr_ptr   cmerPC;                     /* CMER interrupt macro */
instr_ptr   INTLKPC;                    /* (INTLK)Emergency Motor Off interrupt macro*/
instr_ptr   sstopPC;                    /* (SSTOP)Signal Stop interrupt macro */

long    lCf;

int iPCWaiting;                         /* TRUE if we're in the middle of a wait period */
int iNumberOfMacro;                     /* number of macros currently defined */
int iFreezeMacro;                       /* TRUE if the macro is in the freeze state */
int iDefiningMacro;                     /* TRUE if the macro is in the middle of defining */
int iExecutingMacro;                    /* TRUE if the macro is in the middle of executing */
int iAbortLineNumber;                   /* hold the current error line number of the instruction */
int iIAMMode;                           /* IAM mode (0/1) only do something if the mode is on and endm mdoe is 1 */

int iEnterInterruptSectionFlag;         /* flag for either or not enter to interrupt section. */
int iRunningCMERMacro;                  /* flag to signal the CMER macro interrupt is running */
int iRunningSSTOPMacro;                 /* flag to signal SSTOP is running */
int iRunningINTLKMacro;                 /* flag to signal INTLK is running */
int iRunningERRMacro;                   /* flag to signal ERR is running */
int iPendingSSTOPMacro;                 /* flag to signal SSTOP is pending which means there maybe other interrupt comming */
int iPendingINTLKMacro;                 /* flag to signal INTLK is pending which means there maybe other interrupt comming*/
int iPendingERRMacro;                   /* flag to signal ERR is pending */
int iLevelOfNestingForIntMacros;        /* counter to keep track of interrupt macro nesting */

unsigned uMacroStatus;                  /* macro status */

int iERRInterrupt;              /* (ERR)Error interrupt flag*/
int iIIMInterrupt;              /* (IIM)I/O interrupt flag */
int iLMMInterrupt;              /* (LMM) Limit switch interrupt flag */
int iDERInterrupt;		// (DER) Drift Error interrupt
int iSECSInterrupt;             /* SECS interrupt flag */
int iSECSInterrupt2;            /* SECS interrupt2 flag */
int iSECSInterrupt3;            /* SECS interrupt3 flag */
int iSECSInterrupt4;            /* SECS interrupt4 flag */
int iIIM7Interrupt;
int iCMERInterrupt;             /* (CMER) Command error interrupt flag */
int iINTLKInterrupt;            /* (INTLK) Emergency motion off interrupt flag */
int iSSTOPInterrupt;            /* (SSTOP) Signal stop interrupt flag */

int iIIM7Enabled;
int iIIMEnabled;    /* (IIM) flag to disable/enable the I/O interrupt*/
int iERREnabled;    /* (ERR) flag to disable/enable error interrupt*/
int iLMMEnabled;    /* (LMM) flag to disable/enable limit sw interrupt*/
int iDEREnabled;    /* (LMM) flag to disable/enable limit sw interrupt*/
int iSECSEnabled;   /* flag to disable/enable SECS interrupt */
int iCOM2Enabled;   /* flag to disable/enable COM2 interrupt */
int iCOM3Enabled;   /* flag to disable/enable COM3 interrupt */
int iCMEREnabled;   /* flag to disable/enable command error interrupt*/
int iINTLKEnabled;  /* (INTLK)flag to disable/enable Emergency Motor Off interrupt */
int iSSTOPEnabled;  /* (SSTOP) flag to disable/enable Signal Stop off interrupt*/

int iTraceMode;     /* Mode for tracing macro at real time */
int iTracePort;     /* Port id to print out line in the macro when tracing mode is on */
int iMacroTracing;  /* Macro is on the tracing mode while macro is executing. By the time
                                         * the macro finished executing the flag is off. */
int iCMEFlag = 0;   /* CMER returns "?" if set true */

extern int giMeasurementInProgress;	// To kill aligner thread.
extern int giMapTest;
extern int giMapIORunning;

/*******************************************************
 * Function Name:   MAClearMacroNestLevel
 * Description:     Set 0 to macro nest level variable
 * ********************************************************/
void MAClearMacroNestLevel( void )
{
    iLevelOfNestingForIntMacros = 0;
    return;
}

/*******************************************************
 * Function Name:   MASetMacroIntegerVars
 * Description:     Set various macro global variables
 * Parameters:      iFlagArg - Flag indicating which variable to be set
 *                  iValueArg - value that the variable is to be set to
 * Returns:
 * ********************************************************/
int MASetMacroIntegerVars(int iFlagArg, int iValueArg)
{
    switch( iFlagArg )
    {
        case MR_RUNNING_CMER :
            iRunningCMERMacro = iValueArg;
            break;
        case MR_RUNNING_INTLK :
            iRunningINTLKMacro = iValueArg;
            break;
        case MR_RUNNING_SSTOP :
            iRunningSSTOPMacro = iValueArg;
            break;
        case MR_RUNNING_ERR :
            iRunningERRMacro = iValueArg;
            break;
        case MR_PENDING_ERR :
            iPendingERRMacro = iValueArg;
            break;
        case MR_PENDING_INTLK :
            iPendingINTLKMacro = iValueArg;
            break;
        case MR_PENDING_SSTOP :
            iPendingSSTOPMacro = iValueArg;
            break;
        case MR_PC_WAITING :
            iPCWaiting = iValueArg;
            break;
        case MR_DEFINING_MACRO :
            iDefiningMacro = iValueArg;
            break;
        case MR_EXECUTING_MACRO :
            iExecutingMacro = iValueArg;
            break;
        case MR_IAM_MODE :
            iIAMMode = iValueArg;
            break;
        case MR_ERR_INTERRUPT :
            iERRInterrupt = iValueArg;
            break;
        case MR_IIM_INTERRUPT :
            iIIMInterrupt = iValueArg;
            break;
        case MR_LMM_INTERRUPT :
            iLMMInterrupt = iValueArg;
            break;
        case MR_DER_INTERRUPT :
            iDERInterrupt = iValueArg;
printf("iDERInterrrupt=%d\n",iDERInterrupt);
            break;
        case MR_SECS_INTERRUPT :
            iSECSInterrupt = iValueArg;
            break;
        case MR_SECS2_INTERRUPT :
            iSECSInterrupt2 = iValueArg;
            break;
        case MR_SECS3_INTERRUPT :
            iSECSInterrupt3 = iValueArg;
            break;
        case MR_SECS4_INTERRUPT :
            iSECSInterrupt4 = iValueArg;
            break;
        case MR_CMER_INTERRUPT :
            iCMERInterrupt = iValueArg;
            break;
        case MR_INTLK_INTERRUPT :
            iINTLKInterrupt = iValueArg;
            break;
        case MR_SSTOP_INTERRUPT :
            iSSTOPInterrupt = iValueArg;
            break;
        case MR_INTR_SECT_FLAG :
            iEnterInterruptSectionFlag = iValueArg;
            break;
        case MR_IIM_ENABLED :
            iIIMEnabled = iValueArg;
            break;
        case MR_ERR_ENABLED :
            iERREnabled = iValueArg;
            break;
        case MR_LMM_ENABLED :
            iLMMEnabled = iValueArg;
            break;
        case MR_DER_ENABLED :
            iDEREnabled = iValueArg;
            break;
        case MR_SECS_ENABLED :
            iSECSEnabled = iValueArg;
            break;
        case MR_COM2_ENABLED :
            iCOM2Enabled = iValueArg;
            break;
        case MR_COM3_ENABLED :
            iCOM3Enabled = iValueArg;
            break;
        case MR_CMER_ENABLED :
            iCMEREnabled = iValueArg;
            break;
        case MR_INTLK_ENABLED :
            iINTLKEnabled = iValueArg;
            break;
        case MR_SSTOP_ENABLED :
            iSSTOPEnabled = iValueArg;
            break;
        case MR_TRACE_MODE :
            iTraceMode = iValueArg;
            break;
        case MR_TRACE_PORT :
            iTracePort = iValueArg;
            break;
        case MR_EMULATOR_FLAG :
            iEmulatorFlag = iValueArg;
            break;
        default:
            return FAILURE;
    }
    return SUCCESS;
}
/*******************************************************
 * Function Name:   MRGetMacroIntegerVars
 * Description:     Get various macro global variables
 * Parameters:      iFlagArg - Flag indicating which variable to be set
 * Returns:         The requested value
 * ********************************************************/
int MRGetMacroIntegerVars(int iFlagArg)
{
    switch( iFlagArg )
    {
        case MR_RUNNING_CMER :
            return iRunningCMERMacro;
        case MR_RUNNING_INTLK :
            return iRunningINTLKMacro;
        case MR_RUNNING_SSTOP :
            return iRunningSSTOPMacro;
        case MR_RUNNING_ERR :
            return iRunningERRMacro;
        case MR_PENDING_ERR :
            return iPendingERRMacro;
        case MR_PENDING_INTLK :
            return iPendingINTLKMacro;
        case MR_PENDING_SSTOP :
            return iPendingSSTOPMacro;
        case MR_PC_WAITING :
            return iPCWaiting;
        case MR_DEFINING_MACRO :
            return iDefiningMacro;
        case MR_EXECUTING_MACRO :
            return iExecutingMacro;
        case MR_IAM_MODE :
            return iIAMMode;
        case MR_ERR_INTERRUPT :
            return iERRInterrupt;
        case MR_IIM_INTERRUPT :
            return iIIMInterrupt;
        case MR_LMM_INTERRUPT :
            return iLMMInterrupt;
        case MR_DER_INTERRUPT :
            return iDERInterrupt;
        case MR_SECS_INTERRUPT :
            return iSECSInterrupt;
        case MR_SECS2_INTERRUPT :
            return iSECSInterrupt2;
        case MR_SECS3_INTERRUPT :
            return iSECSInterrupt3;
        case MR_SECS4_INTERRUPT :
            return iSECSInterrupt4;
        case MR_CMER_INTERRUPT :
            return iCMERInterrupt;
        case MR_INTLK_INTERRUPT :
            return iINTLKInterrupt;
        case MR_SSTOP_INTERRUPT :
            return iSSTOPInterrupt;
        case MR_INTR_SECT_FLAG :
            return iEnterInterruptSectionFlag;
        case MR_IIM_ENABLED :
            return iIIMEnabled;
        case MR_ERR_ENABLED :
            return iERREnabled;
        case MR_LMM_ENABLED :
            return iLMMEnabled;
        case MR_DER_ENABLED :
            return iDEREnabled;
        case MR_SECS_ENABLED :
            return iSECSEnabled;
        case MR_COM2_ENABLED :
            return iCOM2Enabled;
        case MR_COM3_ENABLED :
            return iCOM3Enabled;
        case MR_CMER_ENABLED :
            return iCMEREnabled;
        case MR_INTLK_ENABLED :
            return iINTLKEnabled;
        case MR_SSTOP_ENABLED :
            return iSSTOPEnabled;
        case MR_TRACE_MODE :
            return iTraceMode;
        case MR_TRACE_PORT :
            return iTracePort;
        case MR_EMULATOR_FLAG :
            return iEmulatorFlag;
        default:
            return 0;
    }
}

/***********************************************
 * Function Name:   MRGetCf
 * Description:     Get Carrier flag.
 *                  This flag is used to control the comparison in the macro programming.
 *                  When doing the compare the value of less than, equal or greater than is stored
 *                  in this flag, so later the conditional JUMP command can use this flag to determine
 *                  the jump.
 * Returns:         value of: > 0, <0, or 0
 * **********************************************/
long MRGetCf()
{
    return lCf;
}

/***********************************************
 * Function Name:   MASetCf
 * Description:     Set the carrier flag.
 *                  This flag is used to control the comparison in the macro programming.
 *                  When doing the compare the value of less than, equal or greater than is stored
 *                  in this flag, so later the conditional JUMP command can use this flag to determine
 *                  the jump.
 * Returns:         none.
 * ***********************************************/
void MASetCf(long lValueArg)
{
    lCf = lValueArg;
}

/************************************************
 * Function Name:   MRGetMacroStatus
 * Description:     Get the current macro status.
 * Returns:         status (unsigned)
 * ************************************************/
int MRGetMacroStatus()
{
    return uMacroStatus;
}

/************************************************
 * Function Name:   MASetMacroStatus
 * Description:     Set the current macro status.
 * Returns:         status (unsigned)
 * ***********************************************/
void MASetMacroStatus(unsigned uStatusArg)
{
    uMacroStatus = uStatusArg;
}

/************************************************
 * Function Name:   MRGetPC
 * Description:     Get the current instruction (PC).
 *                  It is holding the current execution instruction.
 * returns:         instruction pointer (instr_ptr)
 ************************************************/
instr_ptr MRGetPC(void)
{
    return PC;
}

/************************************************
 * Function Name:   MASetPC
 * Description:     Set the current instruction (PC).
 *                  It is holding the current execution instruction.
 * Returns:         none.
 ************************************************/
void MASetPC(instr_ptr PCArg)
{
    PC = PCArg;
}

/************************************************
 * Function Name:   MRGetMacroPC
 * Description:     Get the first instruction pointer of interrupt macro.
 *                  This pointer is got when there is an interrupt and need to execute
 *                  interrupt macro. It can get the instruction pointer for
 *                  all the interrupt macros.
 * Parameters:      iFlagArg - Flag indicating which variable to be set
 * Returns:         instruction pointer (instr_ptr)
 * ***********************************************/
instr_ptr MRGetMacroPC(int iFlagArg)
{
    switch (iFlagArg)
    {
        case MR_ERR_INTERRUPT :
            return errPC;
        case MR_IIM_INTERRUPT :
            return iimPC;
        case MR_LMM_INTERRUPT :
            return lmmPC;
        case MR_COM2_INTERRUPT :
            return com2PC;
        case MR_COM3_INTERRUPT :
            return com3PC;
        case MR_SECS_INTERRUPT :
            return secsPC;
        case MR_SECS2_INTERRUPT :
            return secsPC2;
        case MR_SECS3_INTERRUPT :
            return secsPC3;
        case MR_SECS4_INTERRUPT :
            return secsPC4;
        case MR_CMER_INTERRUPT :
            return cmerPC;
        case MR_INTLK_INTERRUPT :
            return INTLKPC;
        case MR_SSTOP_INTERRUPT :
            return sstopPC;
        case MR_DER_INTERRUPT :
            return derPC;
        default:
            return NULL;
    }
    return NULL;
}

/*************************************************
 * Function Name:   MAInit
 * Description:     Get configure flag from the main module. Initialize all
 *                  the global flags and variables of macro module.
 *                  Also, check if there is the auto macro then run the auto macro.
 *                  This function is called by the mainloop module.
 * Return:          value to determine whether there is auto macro or not
 * *************************************************/
int MAInit(int iDefineFlagArg, int iEmulatorFlagArg)
{
    int iReturnCode;
    int iOpcode;
    char *strTemp;
    OC_TABLE MacroEntry;

    /* set the configure flag and store into global variables for the whole module to use */
    iDefineFlag = iDefineFlagArg;
    iEmulatorFlag = iEmulatorFlagArg;

    /* initialize all variables that is globally to macro module only */
    MAInitMacros();

    iReturnCode = MRLoadMacroFromNvsram(0);      /* read the macro to nvsram */
    if(!iReturnCode)
    {
        strTemp = MAFindOpcode("AUTO", &iOpcode);  /* find auto macro*/
        /* if there is AUTO macro then iReturnCode = 100
         * if there is no AUTO macro then iReturnCode = 0 */
        if (iOpcode != INVALID_OPCODE)
        {
            if(CMDcommands(CMDGET, iOpcode, &MacroEntry)==SUCCESS)
            {
                if (&MacroEntry)
                {
                    /* launch the auto macro and ready to execute to exectue the
                     * AUTO macro. */
                    if(MRLaunchMacro(MacroEntry.p.macro)==FAILURE)
                        iReturnCode = 0;
                    else
                        iReturnCode = 100;
                }
                else
                    iReturnCode = 0;
            }
            else
                iReturnCode = 0;
        }
        else
            iReturnCode = 0;
    }
    else
    {
        iReturnCode = 0;
    }
    return iReturnCode;
}

/*************************************************
 * Fucntion Name:   MAInitMacros
 * Description:     Initialize all the global variables and flags that only global to
 *                  macro module. Also, initalize the stack.
 * Returns:         SUCCESS/FAILURE
 * *************************************************/
int MAInitMacros()
{
    int iReturn;

    iTraceMode = 0;
    iTracePort = COM1;
    iMacroTracing = FALSE;

    iAbortLineNumber = 0;
    iNumberOfMacro = 0;
    iIAMMode = 0;
    iPCWaiting = FALSE;
    iFreezeMacro = FALSE;
    iDefiningMacro = FALSE;
    iExecutingMacro = FALSE;
    uMacroStatus = MACRO_IDLE;
    iEnterInterruptSectionFlag = FALSE;
    caMacroAvoidRecursive[0] = 0;
    TKSetTaskId(-1);                 // RH

    MASetFlagsToFalse();

    iSECSInterrupt = FALSE;
    iSECSInterrupt2 = FALSE;
    iSECSInterrupt3 = FALSE;
    iSECSInterrupt4 = FALSE;
    iIIM7Interrupt = FALSE;

    iIIM7Enabled = FALSE;
//    iIIMEnabled = TRUE;
//    iERREnabled = TRUE;
//    iLMMEnabled = TRUE;
    iIIMEnabled = FALSE;
    iERREnabled = FALSE;
    iLMMEnabled = FALSE;
    iSECSEnabled = FALSE;
    iCOM2Enabled = FALSE;
    iCOM3Enabled = FALSE;
    iCMEREnabled = TRUE;
    iINTLKEnabled = TRUE;
    iSSTOPEnabled = TRUE;
    iDEREnabled = TRUE;

    iimPC = NULL;
    iim7PC = NULL;
    errPC = NULL;
    lmmPC = NULL;
    cmerPC = NULL;
    secsPC = NULL;
    secsPC2 = NULL;
    secsPC3 = NULL;
    secsPC4 = NULL;
    INTLKPC = NULL;
    sstopPC = NULL;
    derPC = NULL;

    iLevelOfNestingForIntMacros = 0;

    psStkTop = NULL;
    psSavedStkTop = NULL;
    psStkTopBeforeInit = staPStat;
    /* tempOldPC is an instruction pointer that each of the member in the
     * struction is intialized to 0. For implement RLN when OldPC is need to be
     * initialized, we use this tempOldPC to initialize the OldPC pointer insteal of
     * set OldPC to NULL. When OldPC = NULL, the instruction is not initialized properly and
     * you may get garbage value. */
    tempOldPC = (instr_ptr)malloc(sizeof(instr_t));
	oldPC = tempOldPC;
    memset(tempOldPC,0,sizeof(instr_t));
    /* initialize the stack */
    iReturn = MAInitStack();
    return iReturn;
}

/**************************************************************************************************
 * Function name:  MADefineMacro
 * Description:    There is a character pointer that's passed from a function arguement.  The pointer
 *                 is pointer at the position after parse out the key word "MACRO". We need to parse all
 *                 the begining space if any. Then start at the pointer points to the first character of the
 *                 name of the macro, Find the opcode and get the opcode.  The opcode should be
 *                 INVALID_OPCODE. If it is valid then the macro is already defined and it should return
 *                 failure. Then parse out the name of macro and save it for avoiding macro calls itself.
 *                 Construct the new instruction by allocate the memory and initalize it so we can store the
 *                 the operand information from calling CMDget_operand. After has the instruction with operands,
 *                 validate the macro instruction and insert macro into OC table and macro is assigned with new
 *                 OC number.  Also, initialize the label structure. When get_operand, validate macro or insert
 *                 macro return failure, you need to free the memory of the instruction that you already
 *                 allocated.  Then set the defining macro flag to TRUE which means the macro is at defining state.
 *                 While at the defining state, the MRTakebody is get the each instruction from the command line
 *                 and parse and validate before linking with the macro instruction list. The macro finishes defining
 *                 when the ENDM is typed and set back the defining macro flag to FALSE. Finally, clean up and fix all
 *                 undefine label.
 *
 * Return:         SUCCESS or FAILURE
 * **************************************************************************************************/
int     MRMacroDefine(char *pcStrArg)
{
    char *pcStart, caMacroName[10];
    int iOpcode, iLen, iReturnCode;
    instr_ptr instr;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    while (*pcStrArg == ' ') pcStrArg++;      /* skip over blanks */

    /* Decode name for new macro and make sure the name is not exist in the table*/
    pcStart = pcStrArg;
    pcStrArg = MAFindOpcode(pcStrArg, &iOpcode);

   /* if the name of macro is exist in the table, then return failure */
    if (iOpcode != INVALID_OPCODE)
        return FAILURE;

    /* Get a copy of macro name (excess chars are truncated) */
    iLen = pcStrArg-pcStart;
    if (iLen > 5)
        return FAILURE;
//        iLen = 5;
    strncpy(caMacroName, pcStart, iLen);
    caMacroName[iLen] = 0;

    /* Allocate memory for macro definition instruction */
    instr = (instr_ptr)malloc( sizeof(instr_t) );
    if (!instr)
    {
#ifdef DEBUG
        SERPutsTxBuff( iCmdPort, "****MRMacroDefine:  Out of memory.****\n\r?" );
#endif
        return FAILURE;
    }

    /* Initialize blank command */
    instr->OC = INVALID_OPCODE;     /* MAInsertMacro() will replace the OC number with valid OC */
    instr->nofopr = 0;
    instr->opr_ptr = NULL;
    instr->line_number = 0;
    instr->next_instr = NULL;

    /* Parse operands/arguements of macro */
    iReturnCode = CMDgetOperands(pcStrArg, instr);
    if (iReturnCode < 0)
    {
        MAFreeInstructions(instr);
        return FAILURE;
    }

   /* validate the macro definition statement*/
    if (vld_macro_def(instr))
    {
        MAFreeInstructions(instr);
        return FAILURE;
    }

   /* insert the macro instruction into the table */
    if (MAInsertMacro(caMacroName, instr, NOIDX, FALSE))
    {
        MAFreeInstructions(instr);
        return FAILURE;
    }

    /* save the defined macro name while macro still at defining state,
     * to make sure the macro can not call itself.*/
    strcpy(caMacroAvoidRecursive,caMacroName);
    caMacroAvoidRecursive[strlen(caMacroName)] = 0;
    /* Set up for label definitions */
    MAInitLabels();

    iDefiningMacro = TRUE;
    /* Read in macro body statements */
    iReturnCode = MRTakeBody(instr);
    /* finished defining macro and reset back the flag */
    iDefiningMacro = FALSE;

    /* Cleanup */
    if (iReturnCode)
    {
        MADeleteMacro(instr->OC, TRUE);
        MAInitLabels();
        return FAILURE;
    }
    else
    {
        MACleanupLabels();
        return SUCCESS;
    }
}

/*************************************************
 * Function Name:   MRTakeBody
 * Description:     Here we get line by line from the command line. Then
 *                  parse and validate each line to make sure it's valid.
 *                  Allocate the memory for each new instruction and link it into
 *                  the instruction list. The macro finishes defining when the
 *                  command line is sent "ENDM". In the case of the instrution is
 *                  a condition jump or label command then call MAResolvedLabel to
 *                  parse the label and resolve the label reference.
 *
 * Return:          SUCCESS or FAILURE
 * *************************************************/
int     MRTakeBody(instr_ptr instrArg)
{
    instr_ptr new_instr;
    int iReturnCode, iOldLine, iDone;
    char caCmd[MAXLINE];
    //char *pcTempMemCheck;
    int iCmdPort;
    int iNumOfBytes;

    iCmdPort = SERGetCmdPort();
//    caCmd = (char *)malloc(MAXLINE);
//    if(!caCmd)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***MRTakeBody 1:  out of memory. ****\n\r?");
//#endif
//        return FAILURE;
//    }
    /* Create and link in a new instruction */
    new_instr = (instr_ptr)malloc( sizeof(instr_t) );
    if (!new_instr)
    {
#ifdef DEBUG
        SERPutsTxBuff( iCmdPort,"***MRTakeBody 2:  OUT OF MEMORY. ****\r\n?");
#endif
        goto error_exit;
    }

    iOldLine = instrArg->line_number;
    instrArg->next_instr = new_instr;
    instrArg = new_instr;
    instrArg->next_instr = NULL;
    if (SERPutsTxBuff(iCmdPort, "\r\nM>") == FAILURE) goto error_exit;
    if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;

    /* loop to create new instruction, parsing command line and insert into the instruction list. */
    iDone = FALSE;
    do
    {
        /* when defining the commands inside the macro we flush buffer
         * so we can send out M> to the host when in SECS device configuration. */
        while(SERRxBuffAvail(iCmdPort) <= 0)
            SERFlushTxBuff(iCmdPort);
        iNumOfBytes = -1;
        /* get the command line from the input */
        if(SERGetsRxBuff(iCmdPort, caCmd, FALSE, &iNumOfBytes, FALSE)==FAILURE)
            goto error_exit;
        /* parsing and validate the instruction */
        iReturnCode = CMDcmndLineAction(caCmd, instrArg, FALSE);
        switch (iReturnCode)
        {
            case MACRO:
            case SUCCESS:
                instrArg->line_number = ++iOldLine;
                switch(instrArg->OC)
                {
                    case ENDM:
                        iDone = TRUE;
                        continue;   /* get out of here, finished defining macro. */
                    case LABEL:
                        if( MAResolveLabel(instrArg)==FAILURE )
                        {
                            instrArg->line_number = --iOldLine;
                            goto error_continue;
                        }
                        break;

                    case JUMP:
                    case JPG:
                    case JPGE:
                    case JPL:
                    case JPLE:
                    case JPNZ:
                    case JPZ:
                        if( MAResolveLabel(instrArg)==FAILURE )
                            goto error_exit;
                }

                /*Create and link in a new instruction */
                new_instr = (instr_ptr)malloc( sizeof(instr_t) );
                if (!new_instr)
                {
#ifdef DEBUG
                    SERPutsTxBuff( iCmdPort,"***MRTakeBody 3:  Out Of Memory**\n\r?");
#endif
                    goto error_exit;
                }
                instrArg->next_instr = new_instr;
                instrArg = new_instr;
                instrArg->next_instr = NULL;
                if (SERPutsTxBuff(iCmdPort, "\r\nM>") == FAILURE) goto error_exit;
                break;

            case FAILURE:
            default:
error_continue:
                if (SERPutsTxBuff(iCmdPort, "\r\nM?") == FAILURE) goto error_exit;
//                pcTempMemCheck = (char *)HPmalloc(MAXLINE);
//                if ( !pcTempMemCheck )
//                {
//                    iDone = TRUE;
//                    continue;
//                }
//                else
//                    HPfree(pcTempMemCheck);
                break;

            case BLANKLINE:
                if (SERPutsTxBuff(iCmdPort, "\r\nM>") == FAILURE) goto error_exit;
                break;
        }
        if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;
    } while (!iDone);
//    if(caCmd)
//        free(caCmd);
    return SUCCESS;

error_exit:
//    if(caCmd)
//        free(caCmd);
    return FAILURE;
}

/**************************************************************************************************
 * Function Name:       MAInsertMacro
 * Description:         To insert the macro into the OC table:
 *                      1. Fill in the property information in the structure of OC table. OC table
 *                         structure is one entry in the OC table. The structure contains Opcode,
 *                         macro name or low-level string name, valid flag, pointer points to first
 *                         macro instruction or pointer points to validate rounter, and fucntion pointer to
 *                         execution routine. So set the opcode and macro name into structure, set pointer points
 *                         to the first instruction of the macro and set the function pointer points to MRExecuteMacro.
 *                      2. There is iNVSram flag for either insert the macro to oc table from the command line or insert macro
 *                         into oc table when loading the macro from NVSRam. If iNVSram is TRUE, then call the
 *                         CMDcommands() from low-level module to add the entry without sorting the table. In this way, we insert
 *                         all the macro entries into table and sort at once at the end. if iNVSram is FALSE,
 *                         then call the CMDcommands() from low-level module to add the entry with sorting the table each
 *                         added to the table.
 *                      3. After added each entry at a time, copy the new Opcode that is assigned by insertion routine
 *                         from low-level module to the instruction.
 *                      4. If successfuly inserted, increment the number of macro defined.
 *                      5. Check for interrupt macro, if the defined macro is one of the interrupt macro,
 *                         then save the instruction pointer to specific type of interrupt instruction.
 *
 * Parameters:          pcNameArg   - macro name
 *                      instrArg    - instruction to insert into the table
 *                      iOpcodeArg  - Opcode of instruction and macro name
 *                      iNVSram     - flag to check if insert macro from command line or
 *                                    insert macro when loading macro from NVSRam.
 * Return:              SUCCESS or FAILURE
 * **************************************************************************************************/
int MAInsertMacro(char *pcNameArg, instr_ptr instrArg, int iOpcodeArg, int iNVSram)
{
    OC_TABLE MacroEntry;
    int iReturnCode;

    /* fill in the property structure before insert */
    MacroEntry.OC = iOpcodeArg;
    strcpy(MacroEntry.OC_str, pcNameArg);
    MacroEntry.flag = CMD_VALID_IN_MACRO | CMD_VALID_CMDLINE | CMD_DEFINE_MACRO;
    MacroEntry.p.macro = instrArg;
    MacroEntry.exec = MRExecuteMacro;

    /* Insert the macro structure into the OC TABLE .
     * Check if the instruction to insert is from command line or loading from NVSRam.
     * if from the command line, insert the macro with sort.
     * if from NVSRam, insert the macro without sort. We want to sort after insert all the macros into the table
     * and sort at once.  So we can speed up the time of loading macros from NVSRam.*/
    if(iNVSram)
    {
        /* insert macro from NVSRam */
        iReturnCode = CMDcommands(CMDPUT_NOSORT, iOpcodeArg, &MacroEntry);
    }
    else
    {
        /* insert macro from the command line */
        iReturnCode = CMDcommands(CMDPUT, iOpcodeArg, &MacroEntry);
    }
    if(&MacroEntry && (iReturnCode == SUCCESS))
    {
        /* get the new OC number after insert the macro into the table */
        iOpcodeArg = MacroEntry.OC;
        /* set new OC number into instruction */
        instrArg->OC = MacroEntry.OC;
        MacroEntry.p.macro->OC = MacroEntry.OC;
        instrArg = MacroEntry.p.macro;
    }
    else
    {
        return FAILURE;
    }
    /* defined macro successfully, then increment the number of macro defined */
    iNumberOfMacro++;

    /* check for interrupt macro, if defined macro is one of the interrupt macro,
     * then save the instruction pointer to specific type of interrupt instruction.*/
    if(iEmulatorFlag & DFEMULATORM)
    {
        if (!strcmp(pcNameArg, "MER"))
            errPC = instrArg;
    }
    else
    {
        if (!strcmp(pcNameArg, "ERR"))  /* ERR interrupt macro */
            errPC = instrArg;
    }
    if (!strcmp(pcNameArg, "IIM"))      /* I/O interrupt macro */
        iimPC = instrArg;
    if( !strcmp(pcNameArg, "IIM7") )    /* I/O interrupt macro (for SV only) */
        iim7PC = instrArg;
    if (!strcmp(pcNameArg, "LMM"))      /* Limit switch interrupt macro */
        lmmPC = instrArg;
    if (!strcmp(pcNameArg, "DER"))      /* Drift Error interrupt macro */
{
        derPC = instrArg;
printf("derPC=%x\n",derPC);
}
    if (!strcmp(pcNameArg, "COM2"))     /* COM2 interrupt macro */
        com2PC = instrArg;
    if (!strcmp(pcNameArg, "COM3"))     /* COM3 interrupt macro */
        com3PC = instrArg;
//    if (!strcmp(pcNameArg, "SECS2"))    /* SECS interrupt macro */
//        secsPC = instrArg;
    if (!strcmp(pcNameArg, "SECA"))    /* SECS interrupt macro */
        secsPC = instrArg;
    if (!strcmp(pcNameArg, "SECB"))    /* SECS interrupt macro */
        secsPC2 = instrArg;
    if (!strcmp(pcNameArg, "SECC"))    /* SECS interrupt macro */
        secsPC3 = instrArg;
    if (!strcmp(pcNameArg, "SECD"))    /* SECS interrupt macro */
        secsPC4 = instrArg;
    if(!strcmp(pcNameArg, "CMER"))      /* Command error interrupt macro */
        cmerPC = instrArg;
    if(!strcmp(pcNameArg, "INTLK"))     /* Emergency off (INTLK) interrupt macro */
        INTLKPC = instrArg;
    if(!strcmp(pcNameArg, "SSTOP"))     /* Signal stop (SSTOP) interrupt macro*/
        sstopPC = instrArg;

    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MRListMacroNames
 * Description:     List all macro names that are defined without the content of the each macro.
 *                  First, call Low-level function LLCmnds to get the first macro in the OC table.
 *                  If the return value is FAIULRE, then that's meant there's no macro in the table.
 *                  Otherwise, loop to get the next macro from table and print out the name of the macro
 *                  until there's no more macro. While printing out the name of macro to the terminal port,
 *                  if the port is EQT (COM1) then list the macro name without waiting for the CR from the
 *                  user; otherwise, if the port is Teach pendant(COM2), then list each macro name and require
 *                  the CR from the user before listing another macro name.
 *                  This function will be executed when user typed "LMCR" at the command line.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int     MRListMacroNames()
{
    int iOpcode;
    OC_TABLE MacroEntry;
    char caBuf[CMD_OPCODE_LENGTH+10];
    int iCmdPort, iTTPort;

    iCmdPort = SERGetCmdPort();
    iTTPort = SERGetTTPort();            /* for listing macro using Teach pedant */

    /* get the first macro from the table  */
    if(CMDcommands(CMDFIRSTMACRO, iOpcode, &MacroEntry)==SUCCESS)
    {
        do                              /* do while there is still macro entry in the table */
        {
//            if(TIRefreshWD()==FAILURE) return FAILURE;

            if (&MacroEntry)            /* check for valid macro entry */
            {
                strcpy(caBuf, MacroEntry.OC_str);
                strcat(caBuf, "\r\n");
                if(iCmdPort == iTTPort)
                {
                    /* For teach pendant, list each macro name and wait for CR from user
                     * before showing the next macro name.*/
                    if (CMDdumpBuf(caBuf) == FAILURE) return FAILURE;
                }
                else
                {
                    /* For EQT, just list the whole list of macro without wait for the
                     * CR from the user */
                    if (SERPutsTxBuff(iCmdPort, caBuf) == FAILURE) return FAILURE;
                    if (SERFlushTxBuff(iCmdPort) == FAILURE) return FAILURE;
                }
                /* using teach pendant and while listing macros, make the CTS is not drop and termimal
                 * does exit. */
            }
            else
                return FAILURE;
        } while(CMDcommands(CMDNEXTMACRO, iOpcode, &MacroEntry)==SUCCESS); /* get the next macro from the table */
    }
    return SUCCESS;

}

/**************************************************************************************************
 * Function Name:   MRListMacro
 * Description:     List specific macro with content of the macro and either with line
 *                  number or not.
 *                  First, validate the Opcode that pass in as arguement. Then get the macro entry from
 *                  the OC table. Macro entry contains macro name or low-level command name,
 *                  Opcode, union pointer to macro or validate rountine, validation flag when the command
 *                  is valid and function pointer to execution rountine.
 *                  Then check for entry to make sure it's not NULL and it's not
 *                  low-level command. Then get the first pointer of instruction list from the macro
 *                  entry. Go through each instruction from the list and print out the instruction
 *                  with or without the line number; it depends on the iNumflagArg value (true/false) that passed in
 *                  from the arguement. TRUE print with line number and FALSE without line number.
 * Parameters:      iOpcodeArg  - OC/Opcode of the macro instruction
 *                  iNumflagArg - flag for listing with or without line number.
 * Return:          SUCCESS and FAILURE
 * **************************************************************************************************/
int MRListMacro(int iOpcodeArg, int iNumFlagArg)
{
    OC_TABLE MacroEntry;
    instr_ptr instr;
    int first;
    char caBuf[55];
    int iCmdPort;
    struct timespec tv;
    tv.tv_sec = 0;

    iCmdPort = SERGetCmdPort();

    /* check for valid OC */
    if(iOpcodeArg == INVALID_OPCODE)
    {
#if DEBUG
        SERPutsTxBuff(iCmdPort, "**MRListMacro:  BAD OPCODE**\n\r");
#endif
        return FAILURE;
    }
    /* Get pointer to first instruction of macro */
    if(CMDcommands(CMDGET, iOpcodeArg, &MacroEntry)==FAILURE)
    {
#if DEBUG
        SERPutsTxBuff(iCmdPort, "**MRListMacro:   get the entry from the OC_TABLE failed\n\r");
#endif
        return FAILURE;
    }
    if(&MacroEntry)
    {
        instr = MacroEntry.p.macro;
    }
    else
        return FAILURE;

    /* list instructin in the macro */
    first = TRUE;
    while (instr)
    {
//        if(TIRefreshWD()==FAILURE) return FAILURE;
        caBuf[0] = 0;
        if (iNumFlagArg)
            sprintf(caBuf, "%3d:  ", instr->line_number);
        if (first)
        {
            strcat(caBuf, "MACRO ");
            first = FALSE;
        }
        if (caBuf[0] != 0)
        {
            if (SERPutsTxBuff(SERGetCmdPort(), caBuf) == FAILURE) return FAILURE;
            if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE) return FAILURE;
        }
        MRListInstructions(instr,FALSE);
        tv.tv_nsec = 10000000;
	nanosleep(&tv, NULL);
	//usleep(10000);
        instr = instr->next_instr;
    }
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MADeleteMacro
 * Description:     Delete the macro from the table, if
 *                  iScanFlagArg is false, delete all; else
 *                  delete specific macro only.
 *                  To delete specific macro, need to validate the Opcode to make sure
 *                  the macro is existing. Then check for interrupt macro; if the macro to be
 *                  deleted is the interrupt macro then set the instruction pointer of interrupt macro
 *                  to NULL.  Then delete the macro and decrement the number of defined macro.  Then
 *                  scan through (reference)/other macro that call this macro and set OC of instruction/line to be
 *                  INVALID_OPCODE. So it will be failed when executing the instruction with INVALID_OPCODE.
 *                  To delete all macro, set all intruction pointers of interrupt macro to NULL.  Then call Low-level
 *                  function to delete all the macro from the OC table. Then set the number of defined macro
 *                  to 0.
 * Parameters:      iOpcodeArg      - OC/Opcode of macro
 *                  iScanFlagArg    - Flag to tell delete all macro or
 *                                    delete specific macro.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MADeleteMacro(int iOpcodeArg, int iScanFlagArg)
{
    OC_TABLE MacroEntry;
    int iReturn;

    if( iScanFlagArg )                /* deleting a single macro */
    {
        /* validate the macro opcode */
        if (iOpcodeArg == INVALID_OPCODE)
        {
#if DEBUG
            SERPutsTxBuff(SERGetCmdPort(),"**MADeleteMacro:  BAD OPCODE**\n\r");
#endif
            return FAILURE;
        }
        /* get the macro entry from the table, check for existing or not */
        if(CMDcommands(CMDGET, iOpcodeArg, &MacroEntry)==FAILURE)
        {
#if DEBUG
            SERPutsTxBuff(SERGetCmdPort(),"**MADeleteMacro:  get entry from OC_TABLE failed **\n\r");
#endif
            return FAILURE;
        }
        if(!(&MacroEntry)) return FAILURE;

        /* check the specific macro delete is one of these interrupt macro then
         * set the instruction pointer of that interrupt to NULL.*/
        if(iEmulatorFlag & DFEMULATORM)
        {
            if (!strcmp(MacroEntry.OC_str, "MER"))
                errPC = NULL;
        }
        else
        {
            if (!strcmp(MacroEntry.OC_str, "ERR"))  /* Error interrupt macro */
                errPC = NULL;
        }
        if (!strcmp(MacroEntry.OC_str, "IIM"))   /* I/O interrupt macro */
            iimPC = NULL;
        if( !strcmp(MacroEntry.OC_str, "IIM7") ) /* I/O interrupt macro (for SV only) */
            iim7PC = NULL;
        if (!strcmp(MacroEntry.OC_str, "LMM"))   /* Limit switch interrupt macro */
            lmmPC = NULL;
        if (!strcmp(MacroEntry.OC_str, "DER"))   /* Drift error interrupt macro */
            derPC = NULL;
        if (!strcmp(MacroEntry.OC_str, "COM2"))  /* COM2 interrupt macro */
            com2PC = NULL;
        if (!strcmp(MacroEntry.OC_str, "COM3"))  /* COM3 interrupt macro */
            com3PC = NULL;
//        if (!strcmp(MacroEntry.OC_str, "SECS2")) /* SECS interrupt macro */
//            secsPC = NULL;
        if (!strcmp(MacroEntry.OC_str, "SECA")) /* SECS interrupt macro */
            secsPC = NULL;
        if (!strcmp(MacroEntry.OC_str, "SECB")) /* SECS interrupt macro */
            secsPC2 = NULL;
        if (!strcmp(MacroEntry.OC_str, "SECC")) /* SECS interrupt macro */
            secsPC3 = NULL;
        if (!strcmp(MacroEntry.OC_str, "SECD")) /* SECS interrupt macro */
            secsPC4 = NULL;
        if (!strcmp(MacroEntry.OC_str, "CMER"))  /* Command errror interrupt macro */
            cmerPC = NULL;
        if (!strcmp(MacroEntry.OC_str, "INTLK")) /* INTLK-Emergency Motor Off interrupt macro */
            INTLKPC = NULL;
        if (!strcmp(MacroEntry.OC_str, "SSTOP")) /* SSTOP-Stop Signal interrupt macro */
            sstopPC = NULL;

        /* delete the specified macro */
        if( CMDcommands(CMDDELETE,iOpcodeArg,&MacroEntry)==FAILURE )
        {
#if DEBGU
            SERPutsTxBuff(SERGetCmdPort(), "MADeleteMacro: delete entry from oc_table failed\n\r");
#endif
            return FAILURE;
        }
        iNumberOfMacro--;               /* decrement the number of macro defined */
        /* check if other macros using this macro to set the instruction to be invalid */
        iReturn = MADeletedMacroScan(iOpcodeArg);
        if(iReturn==FAILURE)
            return FAILURE;
    }
    else                                /* deleting all macros */
    {
        /* set all the interrupt macro instruction to be NULL */
        errPC = NULL;
        iimPC = NULL;
        iim7PC = NULL;
        lmmPC = NULL;
        derPC = NULL;
        com2PC = NULL;
        com3PC = NULL;
        secsPC = NULL;
        cmerPC = NULL;
        INTLKPC = NULL;
        sstopPC = NULL;

        /* delete all the macro in the table */
        if( CMDcommands(CMDDELALLMACRO,-1,NULL) == FAILURE )
        {
#if DEBUG
            SERPutsTxBuff(SERGetCmdPort(), "MADeleteMacro: delete entry from oc_table failed\n\r");
#endif
            return FAILURE;
        }
        iNumberOfMacro = 0;             /* set the number of macro defined to 0 */
    }
    return SUCCESS;
}

/*************************************************
 * Function Name:   MADeleteMacroScan
 * Description:     Delete the any reference to this macro in OC table.
 *                  First get the first macro and go through the whole table to check
 *                  if the Opcode in the table = iOpcodeArg, then set the Opcode in
 *                  the table to be INVALID_OPCODE.  This function is called if delete specific
 *                  macro.
 * Parameters:      iOpocdeArg  - OC/opcode of macro to be deleted.
 * Return:          SUCCESS or FAILURE
 * *************************************************/
int    MADeletedMacroScan(int iOpcodeArg)
{
    OC_TABLE MacroEntry;
    instr_ptr instr;
    int iReturn;

   /* validate the Opcode */
    if(iOpcodeArg == INVALID_OPCODE)
        return FAILURE;
    /* scan in the whole table and set any macro that have reference to the INVALID_OPCODE macro. */
    if(CMDcommands(CMDFIRSTMACRO, iOpcodeArg, &MacroEntry)==SUCCESS)
    {
        do
        {
            if(&MacroEntry)
            {
                instr = MacroEntry.p.macro;
                instr = instr->next_instr;
                while(instr)
                {
                    if (instr->OC == iOpcodeArg)
                        instr->OC = INVALID_OPCODE;  /* set opcode to be INVALID_OPCODE */
                    instr = instr->next_instr;
                }
            }
            else
               return FAILURE;
            iReturn = CMDcommands(CMDNEXTMACRO, iOpcodeArg, &MacroEntry);  /* get next macro in the table */
        }while(iReturn==SUCCESS);
    }
    return SUCCESS;
}

/*************************************************
 * Function Name:   MAFreeInstructions
 * Description:     Free memory from the instruction list.
 *                  Go through the each instruction in the instruction list and delete
 *                  all operands belong to the instruction first, then delete the instruciton.
 *                  instrArg - pointer points to the first instruction in the instruction list.
 * return:          SUCCESS or FAILURE
 * *************************************************/
int    MAFreeInstructions(instr_ptr instrArg)
{
    instr_ptr p_next;

    while (instrArg)
    {
        CMDfreeOperands(instrArg->opr_ptr);  /* free the operands */
        p_next = instrArg->next_instr;
        free(instrArg);               /* free the instruction */
        instrArg = p_next;
    }
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MRLaunchMacro
 * Description:      Launching the running the macro, and push into the stack. Other word, set up
 *                  the macro instruction for mainloop to execute.
 *                  1. Check instruction pointer is NULL or not.  It's ok if
 *                     the instruction is NULL, then exit the function. it's possible that executing the
 *                     interrupt macro and it is NULL.
 *                  2. if there is not macro running since PC is NULL, then initalize the stack and put
 *                     the instruction of macro information to the top of stack.  The stack(LIFO) is
 *                     an array of prog_status_st structure pointer. Each struction contains PC,
 *                     oldPC and other macro status.  However, the important is PC and OldPC.  PC is current
 *                     insturction pointer for executing instruction.  OldPC is used for RLN command to
 *                     get the information of line number. So put instruction into the OldPC of the top of stack,
 *                     and the next instruction into the PC of the top of the stack. Also, make sure to set
 *                     the flag to be no multi-tasking and set the pointer of psStkTopBeforeInit to NULL.
 *                     psStkTopBeforeInit is pointer to save the top of the stack if there is command line error
 *                     durring executing macro.  So the RLN command can read the line that has failed at.
 *                  3. In other case, if there is something is currently running (PC is not NULL), then
 *                     get current task and push it into the stack.
 *                  4. Set up current instruction for mainloop to executing the macro instruction.
 *
 * Return:          SUCCESS or FAILURE
 * *************************************************************************************************/
int MRLaunchMacro(instr_ptr instrArg)
{
    int iReturnCode = SUCCESS, iCounter;

    TKSetPCCriticalSection( 1 );
    // we need to clear the statuses of macro tasks
    for(iCounter = 0; iCounter < MAXTASKS; iCounter++)
        caTaskStatus[iCounter][49] = '\0';

    if (!instrArg)        /* interrupt macro could be NULL */
    {
        goto exit_point;
    }

    iFreezeMacro = FALSE;   /* somebody wants to start things up */
    uMacroStatus = MACRO_RUNNING;
    ++iLevelOfNestingForIntMacros; /* increment nest level, whenever a macro is launched. */

    /* Push anything currently executing & check for stack overflow */
    if (PC != NULL)
    {
        if( TKReadNoTaskFlag( ) )  /* for no multi-tasking */
            iReturnCode = MRPushPC(NULL);
        else                            /* with multi-taksing */
            iReturnCode = MRPushPC(TKGetCurrentTask());

        /* if error initialize stack */
        if (iReturnCode)
        {
            iAbortLineNumber = 1;
            MAInitStack();
            iReturnCode = FAILURE;
            goto exit_point;
        }
        iFreezeMacro = FALSE;   /* somebody wants to start things up */
        uMacroStatus = MACRO_RUNNING;
    }
    /* no multi-tasking is running at this pointer and
     * there is no macro is running. Now start to run the macro.*/
    else
    {
        TKSetNoTaskFlag(TRUE);
        MAInitStack();
        psStkTopBeforeInit = NULL;
        psStkTop->m_OldPC = instrArg;
        psStkTop->m_PC = instrArg->next_instr;
        uMacroStatus = MACRO_RUNNING;
        iFreezeMacro = FALSE;   /* somebody wants to start things up */
    }

    /* Set up for executing the new macro */
    oldPC = instrArg;
    PC = instrArg->next_instr;
    lCf = 0;

exit_point:
    TKSetPCCriticalSection( -1 );
    return iReturnCode;
}

/**************************************************************************************************
 * Function Name:   MRWriteMacros
 * Description:     Write the macro into file (NVSRam). This function is called when
 *                  save the macro to NVSRam.
 *                  First, write the version string and number of defined macro. Then
 *                  get the first macro in the OC table from low-level module. Then write the
 *                  contents of each macro while there's next macro in the OC table.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRWriteMacros()
{
    int iOpcode;
    OC_TABLE MacroEntry;
    char *strVersion;
    unsigned uCfgFeature;

    /* write the version string */
    strVersion = GetVersionString();
    if(MRWriteBytes( VERSTRLEN, strVersion )==FAILURE) return FAILURE;
    /* get feature setting and write it */
    uCfgFeature = FIOGetCfgFeatureInt();
    if(MRWriteBytes(4, &uCfgFeature) == FAILURE) return FAILURE;
    /* write # of defined macros */
    if(MRWriteBytes(4, &iNumberOfMacro)==FAILURE) return FAILURE;

    /* write each macro with its contents */
    if(CMDcommands(CMDFIRSTMACRO, iOpcode, &MacroEntry)==SUCCESS)
    {
        do
        {
            if(&MacroEntry)
            {
                /* write the body of the macro */
                if(MRWriteBody(&MacroEntry)==FAILURE) return FAILURE;
            }
            else
                return FAILURE;
        }while(CMDcommands(CMDNEXTMACRO, iOpcode, &MacroEntry)==SUCCESS);  /* get next macro in the OC table */
    }
    return SUCCESS;
}

/*************************************************
 * Function Name:   MRWriteBody
 * Description:     Write the body or content of specific macro to the file.
 *                  First, write the name of the macro. Then go through the instruction list
 *                  to count the number of instructions that the macro has. For number of instructions,
 *                  write instruction information such as Opcode, number of operands and line number.  Then
 *                  for number of operands, write contents of each operand. However, if the operand has STRING_CONST
 *                  type, then need find the string length and write the string length and string content.
 * Return:          SUCCESS or FAILURE
 * ************************************************/
int    MRWriteBody(OC_TABLE *pEntryArg)
{
    int i, j, iNumOfInstr, nBytes, k;
    instr_ptr instr;
    CMDoperand_ptr p_opr;

    /* write macro name */
    if(MRWriteBytes(6, pEntryArg->OC_str)==FAILURE) return FAILURE;

    iNumOfInstr = 0;
    /* get number of instructions that specified macro has*/
    instr = pEntryArg->p.macro;
    while (instr)
    {
//        if(TIRefreshWD()==FAILURE) return FAILURE;
        iNumOfInstr++;
        instr = instr->next_instr;
    }
    /* write # of instructions */
    if(MRWriteBytes(4, &iNumOfInstr)==FAILURE) return FAILURE;
    /* Loop through macro instructions to write all the operands in the instruction */
    instr = pEntryArg->p.macro;
    for (i=0; i<iNumOfInstr; i++)
    {
//        if(TIRefreshWD()==FAILURE) return FAILURE;

        if(MRWriteBytes(4, &instr->OC)==SUCCESS)     /* write OPCODE  */
        {
            if(MRWriteBytes(4, &instr->nofopr)==SUCCESS)     /* write # of operands */
            {
                if(MRWriteBytes(4, &instr->line_number)==FAILURE)     /* write line number   */
                    return FAILURE;
            }
            else
                return FAILURE;
        }
        else
            return FAILURE;
        /* loop thru operands */
        p_opr = instr->opr_ptr;
        nBytes = sizeof(CMDoperand) - sizeof(void *);
        for (j=0; j<instr->nofopr; j++)
        {
//            if(TIRefreshWD()==FAILURE)
//                return FAILURE;
            if(MRWriteBytes(nBytes, p_opr)==FAILURE)    /* write operands, less ptr */
                return FAILURE;
            if( p_opr->type == STRING_CONST )
            {
                k = strlen( p_opr->opr.s );            /* find the len of the string */
                if(MRWriteBytes( sizeof( int ),  (void *)&k )==FAILURE)  /* write # of bytes of the string */
                    return FAILURE;
                if(MRWriteBytes( k, p_opr->opr.s )==FAILURE)  /* write the string */
                    return FAILURE;
            }
            p_opr = p_opr->next;
        }
        instr = instr->next_instr;
    }
    return SUCCESS;

}

/**************************************************************************************************
 * Function Name:   MRWriteBytes
 * Description:     Write specific number of byte to the file. Before write
 *                  to the file calculate the checksum. For checksum calculation, there is
 *                  global variable(iMod2) that got initialized to 0 before read or write to NVSram.
 *                  The checksum is calculated with alternative exclusive or each character at upper 8-bit or
 *                  lower 8-bit. if iMod2 is 1, checksum exclusive or with a character at upper 8-bit. if iMod2
 *                  is 0, checksum exclusive or with a chacter at lower 8-bit. The checksum is calculate for
 *                  the whole macro file.  Writing the checksum to file after wrote the whole macro content to file.
 *
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRWriteBytes(unsigned uNumBytesArg, void *ptrArg)
{
    int nCount, nBytes;
    char *pcChar;
    int iReturn;

    pcChar = (char *)ptrArg;
    nBytes = uNumBytesArg;
    /* iMod2 is global variable which is intialized before save(write) the macro to NVSram or
     * load(read) the macro from NVSram. It's initialized to 0*/
    for ( nCount=0; uNumBytesArg>0; uNumBytesArg--)
    {
        if( iMod2 )                     /* the mode is alternative switch (0/1) */
        {
            iMod2=0;                    /* now the mode is 1, then switch to 0 */
            uMacroCheckSum ^= (pcChar[nCount] & 0x00FF);  /* exclusive or with upper 8-bit */
        }
        else
        {
            iMod2=1;                    /* now the mdoe is 0, then switch to 1 */
            uMacroCheckSum ^= (unsigned)pcChar[nCount] << 8;  /* exclusive or with lower 8-bit */
        }

        uNumberOfBytes++;
        nCount++;
    }
    nCount = 0;
    iReturn = SUCCESS;
    nCount = fwrite(ptrArg, sizeof(char), nBytes, iFilePointer);
    /* make sure if there is enough memory for writing */
    if(iReturn != SUCCESS || nCount < nBytes)
    {
#if DEBUG
        printf("MRWriteBytes: error on writing to NVSram\n");
#endif
        return FAILURE;
    }
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MRWriteCheckSum
 * Description:     Write the check sum to the file 
 *                  It needs to make sure the function call is success by check for number of bytes that
 *                  want to write with number of bytes have read and returned. They both should be equal.
 *                  This function is called after wrote all the macro to file.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRWriteCheckSum()
{
    int nCount, nBytes;
    int iReturn;

    nBytes = sizeof(unsigned);
    nCount = 0;
    iReturn = SUCCESS;
    nCount = fwrite((void *)&uMacroCheckSum, sizeof(unsigned), 1, iFilePointer);
    /* make sure there is enough memory for writing */
    if(iReturn != SUCCESS || nCount < 1)
    {
#if DEBUG
        printf("MRWriteCheckSum: error on writing to NVSram.\n");
#endif
        return FAILURE;
    }
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MAReadMacros
 * Description:     Read macros from the file(NVSRam).
 *                  - read the version string and compare the version string
 *                  - read number of defined macros
 *                  - read each macro with contents and insert into the table with sorting
 *                  - sorting after inserting all the macros into the table.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int     MAReadMacros()
{
    int i, iNumOfMacroInFile;
    int iRc = SUCCESS;
    char caVersion[VERSTRLEN+1];
    unsigned uCfgFeature;

    if((iRc = MAReadBytes( VERSTRLEN, caVersion )))  /* read version string */
        goto err_exit;
    /* compare the version string */
    if( (iRc = strncmp( GetVersionString(), caVersion, strlen(GetVersionString()) )) != 0 )
       goto err_exit;

    if( (iRc = MAReadBytes(4, &uCfgFeature)) )         /* read Cfg Feature */
        goto err_exit;
    /* compare the Cfg feature */
    if( FIOGetCfgFeatureInt() != uCfgFeature)
    {
        iRc = FAILURE;
        goto err_exit;
    }

    if((iRc = MAReadBytes(4, &iNumOfMacroInFile)))   /* read # of macros in file */
        goto err_exit;
    /* read contents of each macro */
    for (i=0; i<iNumOfMacroInFile; i++)
	{
//        if((iRc = TIRefreshWD())==FAILURE)
//            goto err_exit;
	if ((iRc = MAReadBody()) )
            goto err_exit;
    }
    CMDcommands(CMDINIT,-1,NULL);            /* sort the table and update the table */
err_exit:
    return iRc;
}

/**************************************************************************************************
 * Function Name:   MAReadBody
 * Description:     Read the body or content of the macro from the file.
 *                  and insert the macro into the table.
 *                  - insert the macro name into the table.
 *                  - read all instructions until there is no more instruction
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int     MAReadBody()
{
    int iOpcode;
    unsigned uNumInstr;
    char caMacroName[6], caBuf[10];
    char *strTemp;
    instr_ptr instr, head;

    if(MAReadBytes(6, caMacroName))         /* read macro name */
        return FAILURE;
    caMacroName[strlen(caMacroName)] = 0;         /* (add null for safety) */
    strcpy(caBuf, caMacroName);
    /* get opocde by passing macro string name */
    strTemp = MAFindOpcode(caBuf, &iOpcode);
    if (iOpcode != INVALID_OPCODE)
        return FAILURE;
    if(MAReadBytes(4, &uNumInstr))      /* read # of instructions */
        return FAILURE;
    head = instr = MAReadInstr(NULL);	    /* read first instruction */
    if (!instr)
        return FAILURE;
    /* Insert it into macro table */
    if(MAInsertMacro(caMacroName, instr, NOIDX, TRUE))
        return FAILURE;
    /* Loop through macro instructions */
    MAInitLabels( );                    /* initalize the labels */

    for (uNumInstr-- ; uNumInstr>0; uNumInstr--)
	{
//        if(TIRefreshWD()) return FAILURE;
        instr = MAReadInstr(instr);     /* read the instruction from the file */
		if (!instr)
		{
			MAInitLabels( );
            return FAILURE;
		}
		switch (instr->OC)
		{
		  case LABEL:
		  case JUMP:
		  case JPG:
		  case JPGE:
		  case JPL:
		  case JPLE:
		  case JPNZ:
		  case JPZ:
            if( MAResolveLabel(instr) )  /* resolve the label reference */
			{
			  MAInitLabels( );
              return FAILURE;
			}
			break;
		  default:
			break;
		}
    }
    MACleanupLabels( );                 /* clean up the undefined label */
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MAReadInstr
 * Description:     This function is called by MAReadBody and return instruction pointer.
 *                  Read the instruction and operands from the file:
 *                  - Allocate the memory for instruction.
 *                  - read OC, number of operands and line number and store into the instruction structure.
 *                  - Allocate the memory for operands and read the operands
 *                  - link the instruction to list.
 * return:          pointer to instr
 * **************************************************************************************************/
instr_ptr  MAReadInstr(instr_ptr prev_instrArg)
{
    int i, nBytes, j;
    instr_ptr instr;
    CMDoperand_ptr p_opr;
    CMDoperand_ptr prev_opr;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    /* Allocate memory for instruction */
    instr = (instr_ptr)malloc( sizeof(instr_t) );
    if (!instr)
	{
#ifdef DEBUG
        SERPutsTxBuff( iCmdPort,"****MAReadMacros:  Out of memory when doing malloc for instr *****\n\r?");
#endif
		return NULL;
    }

    /* Read instruction structure */
    MAReadBytes(4, &instr->OC);		    /* read opcode */
    MAReadBytes(4, &instr->nofopr);	    /* read # of operands */
    MAReadBytes(4, &instr->line_number);     /* read line number   */
    instr->opr_ptr = NULL;
    instr->next_instr = NULL;

    /* Loop thru operands */
    nBytes = sizeof(CMDoperand) - sizeof(void *);
    prev_opr = NULL;
    for (i=0; i<instr->nofopr; i++)
	{
        p_opr = (CMDoperand_ptr)malloc( sizeof(CMDoperand) );
		if (!p_opr)
		{
#ifdef DEBUG
            SERPutsTxBuff( iCmdPort,"****MAReadMacros:  Out of memory when doing malloc for operand****\n\r?");
#endif
			goto err_exit;
		}
        if(MAReadBytes(nBytes, p_opr)) goto err_exit;     /* read operand */
        if( p_opr->type == STRING_CONST )
		{
            if(MAReadBytes( sizeof( int ), (void *)&j )) goto err_exit;

            if( !(p_opr->opr.s = (char *)malloc( j+1 )) )
			{
#ifdef DEBUG
               SERPutsTxBuff( iCmdPort,"***MAReadMacros:  Out of memory when doing malloc for string.***\n\r?");
#endif
			   goto err_exit;
			}
            if(MAReadBytes( j, p_opr->opr.s )) goto err_exit;
			p_opr->opr.s[j] = 0;
		}
        p_opr->next = NULL;
		if (!prev_opr)
			instr->opr_ptr = p_opr;
		else
            prev_opr->next = p_opr;
		prev_opr = p_opr;
    }
    /* Link to previous instruction */
    if (prev_instrArg)
        prev_instrArg->next_instr = instr;
    return instr;

err_exit:
    MAFreeInstructions(instr);
    return NULL;
}

/**************************************************************************************************
 * Function Name:   MAReadBytes
 * Description:     Read specific number of bytes from the file
 *		    Then make sure the function call read properly by checking number of
 *                  byte want to read is equal to number of bytes have read and number of bytes have read is
 *                  not zero and the return value is SUCCESS.
 * return:          SUCCESS or FAILURE
 * *************************************************/
int    MAReadBytes(unsigned uNumBytesArg, void *ptrArg)
{
    int nCount;
    int iReturn;

    nCount = 0;
    iReturn = SUCCESS;
    nCount = fread( ptrArg, sizeof(char), uNumBytesArg, iFilePointer);
    /* check to make sure the reading has no error */
    if(iReturn != SUCCESS || ((unsigned)nCount < uNumBytesArg && nCount != 0) )
    {
#if DEBUG
        printf("MAReadBytes: error on reading from NVSram.\n");
#endif
        return FAILURE;
    }
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MAReadCheckSum
 * Description:     Read the checksum from the file and compare
 *                  if the checksum is not the same, then delete all
 *                  macro just read.
 * Note:            This function is not using at all in the code.  If you want to use make sure
 *                  test or watch out the for the part the delete all macros.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int     MAReadCheckSum(void)
{
    unsigned uFileChkSum;
    OC_TABLE MacroEntry;
    int nCount, nBytes, iReturn;
    int iOpcode;

    nBytes = sizeof(unsigned);
    nCount = 0;
    iReturn = SUCCESS;
    nCount = fread((void *)&uFileChkSum, sizeof (unsigned), 1, iFilePointer);
    /* check to make sure the reading has no error */
    if(iReturn != SUCCESS || (nCount < 1))
    {
#if DEBUG
        printf("MAReadCheckSum: error on reading from NVSram.\n");
#endif
        return FAILURE;
    }
    if (uMacroCheckSum != uFileChkSum)
	{
	/* Delete all the macros just read */
// fixme - need to call other delete macro function. This way is really slow down the process.
//         check with Anna.
        if(CMDcommands(CMDFIRSTMACRO, iOpcode, &MacroEntry)==SUCCESS)
        {
            do
            {
                if(&MacroEntry)
                {
                    if(MADeleteMacro(MacroEntry.OC, FALSE)) return FAILURE;
                }
                else
                    return FAILURE;
            }while(CMDcommands(CMDNEXTMACRO, iOpcode, &MacroEntry)==SUCCESS);
        }
        else
            return FAILURE;
    }
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MASaveMacroToNvsram
 * Description:     Write the macros into nvsram(for ex_SMCR). It will write each macro contents to
 *                  macro file in NVSram.
 *                  First, to make sure NVSram checksum status is good in order to read from NVSram.
 *                  If it's bad NVSram checksum then set the checksum status bit on for macro file and
 *                  return failure. Then build the macro name.
 *                  If create successfully, then write the content of the macro by calling MRWriteMacros.
 *                  In MRWriteMacros will get content of each macro from oc table and before write to the
 *                  file, it calculates the checksum.  After writing all the macros that are in the oc
 *                  table, then write the checksum to file by calling MRWriteChecksum().
 * return:          SUCCESS or FAILURE
 * *************************************************************************************************/
int     MASaveMacrotoNvsram(int iFileIdArg)
{
    char  caFilename[125];
    int   iReturn;

   /* check the checksum status of NVSram for macro file */
    if( SSGetCheckSumStatus(BAD_NVSRAM) & BAD_NVSRAM )
	{
      SSSetCheckSumStatus( MACR_FILE, TRUE );  /* turn on the status bit of macro file is bad */
      return FAILURE;
    }

    if( iFileIdArg > 999 )
    {
	return FAILURE;
    }

    SSSetCheckSumStatus(MACR_FILE, FALSE);  /* the macro checksum status is good */
    /* make filename */
    sprintf( caFilename, MACWILDCARD, (long)iFileIdArg );

    //Dos&Counters
    //Increase the WriteToNVSRAM counter for macro file and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_MACROFILECOUNTER);

    /* create file */
    iFilePointer = fopen(caFilename, "w");

    if( iFilePointer == NULL )
    {
        SSSetCheckSumStatus(MACR_FILE, TRUE);  /* set he macro checksum status to bad when create
                                                * macro is failed. */
        return FAILURE;
    }
	uMacroCheckSum = iMod2 = uNumberOfBytes = 0;
        iReturn = MRWriteMacros( );     /* write macros */
        if(iReturn == FAILURE)
            goto exit_point;
        iReturn = MRWriteCheckSum( );   /* write checksum */

exit_point:
        fclose(iFilePointer);           /* close file */
    return iReturn;
}

/**************************************************************************************************
 * Function Name:   MRLoadMacroFromNvsram
 * Description:     Load the from nvsram (for ex_RMCR). It reads the macro file from NVSram and insert
 *                  each macro content into the oc table.
 *                  First, to make sure NVSram checksum status is good in order to read from NVSram.
 *                  If it's bad NVSram checksum then set the checksum status bit on for macro file and
 *                  return failure. Otherwise, delete all the existing macro in the oc table and set
 *                  checksum status bit off for macro file. Then build the macro name and test the
 *                  file checksum by calling FIOTestFileChkSum from Datafile module. If the test is passed
 *                  then open the macro file and read the content of the macro from the file by calling
 *                  MAReadMacros(). MReadMacro will build and put the macro into the oc table.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int     MRLoadMacroFromNvsram(int iFileIdArg)
{
    char caFilename[125];
    int iReturnValue;

    if( SSGetCheckSumStatus(BAD_NVSRAM) & BAD_NVSRAM )
    {
      SSSetCheckSumStatus( MACR_FILE, TRUE );
      return FAILURE;
    }

    if( iFileIdArg > 999 )
    {
        return FAILURE;
    }

    /* Delete all existing macros */
    if((iReturnValue = MADeleteMacro(INVALID_OPCODE, FALSE))==FAILURE)
        return FAILURE;
    SSSetCheckSumStatus(MACR_FILE, FALSE);

    /* build filename */
    sprintf( caFilename, MACWILDCARD, (long)iFileIdArg );
    /*test the file checksum */
    if( (iReturnValue=FIOTestFileChkSum( caFilename )) )
	{
        if( iReturnValue == 1 || iReturnValue == -1 )
            SSSetCheckSumStatus( MACR_FILE, TRUE );
        return FAILURE;
    }
    /* open the file */
    iFilePointer = fopen(caFilename, "r");
    if (iFilePointer == NULL)
    {
        SSSetCheckSumStatus( MACR_FILE, TRUE );
        return FAILURE;
    }

    iReturnValue = uMacroCheckSum = iMod2= uNumberOfBytes = 0;

    iReturnValue = MAReadMacros();  /* read macros from the file */

//exit_point:
    fclose( iFilePointer );
    if(iReturnValue)
        SSSetCheckSumStatus(MACR_FILE, TRUE);
    return iReturnValue;
}

//#ifdef SX
/**************************************************************************************************
 * Function Name:   MRPreLaunchMacro
 * Description:     Set the passed registers to DevID, Stream, Function, Message,
 *                  for SECS communication only.
 *                  Implemented but not support for 6.00 I2BRT.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int     MRPreLaunchMacro(instr_ptr instrArg, long p0, long p1, long p2, char *p3)
{
    int i;
    CMDoperand_ptr mOpr;

    mOpr = instrArg->opr_ptr;

    for (i=0;i<4;i++)
	{
		if (!mOpr )
			break;
		switch( i )
		{
			case 0:
				RGSetRegister(mOpr->opr.i, p0);
			  break;
			case 1:
				RGSetRegister(mOpr->opr.i, p1);
			  break;
			case 2:
				RGSetRegister(mOpr->opr.i, p2);
			  break;
			case 3:
                		RGSetString(mOpr->opr.i, p3, 1);
			  break;
		}
		mOpr = mOpr->next;

    }  /* end of for loop */
    return SUCCESS;
}
//#endif

/**************************************************************************************************
 * Function Name:   MRExecuteMacro
 * Description:     Exectuing the macro will launch the macro and push macro information to the
 *                  stack and set up the next instruction for MRExecuteInstructions to execute by the time
 *                  in the mainloop.
 *                  To Execute the macro: Frist, get the macro entry from oc table by passing in
 *                  opcode. If it is valid then get the first macro instruction pointer from the macro
 *                  entry, so we can get the arguement of the defined macro. For Brook simulation (NOVELLUS),
 *                  we need to set all arguement information to special registers that reserved for NPAR comamnd.
 *                  (please look at the code for clean information about getting the macro arguement). Then
 *                  launch the macro to be ready to execute the next instruction in the macro and set opcode of
 *                  this specified macro to current macro.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int MRExecuteMacro(instr_ptr instrArg)
{
    int i, iOpcode;
    instr_ptr instr;
    CMDoperand_ptr pOpr;
    CMDoperand_ptr mOpr;
    OC_TABLE MacroEntry;
    char cTemp;

    /* First make sure we have a valid macro */
    /* Get pointer to first instruction of macro */
	iOpcode = instrArg->OC;
    if(CMDcommands(CMDGET, iOpcode, &MacroEntry) )
    {
#if DEBUG
        SERPutsTxBuff(SERGetCmdPort(), "**ListMacro:   get the entry from the OC_TABLE failed\n\r");
#endif
        return FAILURE;
    }
    if(! (&MacroEntry))
	{
#if DEBUG
		SERPutsTxBuff(SERGetCmdPort(), "**ListMacro:  no entry for given opcode\n\r");
#endif
        return FAILURE;
	}
    if(!(MacroEntry.flag & CMD_VALID_IN_MACRO))
        return FAILURE;
    /* Get ptr to first instruction in macro */
    instr = MacroEntry.p.macro;
    /* No par parsing (no par checking) for SECS macro */
    if ( (instr != secsPC) && (instr != secsPC2) && (instr != secsPC3) && (instr != secsPC4) )
    {
    /* Execute the macro header instruction (argument passing) */
        pOpr = instrArg->opr_ptr;
        mOpr = instr->opr_ptr;
        if(mOpr)
        {
            for (i=0; i<instrArg->nofopr; i++)
            {
                if (!mOpr || !pOpr)
                    return FAILURE;
                if (mOpr->type == MACRO_ALPHA)  /* C-register or character value arguement */
                {
                    switch (pOpr->type)
                    {
                        case CHAR_CONST:
                                if(RGEqualCRegChar(mOpr->opr.i, (char)pOpr->opr.i)==FAILURE)
                                    return FAILURE;
                                break;
                        case INDIRECT_CHAR:
                                if(RGEqualCRegCReg(mOpr->opr.i, pOpr->opr.i)==FAILURE)
                                    return FAILURE;
                                break;
                        default:
                                return FAILURE;
                    }
                }
                else if (mOpr->type == MACRO_NUM )      /* R-register or numeric value arguement */
                {
                    switch (pOpr->type)
                    {
                        case BR_CONST :
                                if(RGEqualRegNum(mOpr->opr.i, pOpr->opr.l|BR_FLAG)==FAILURE)
                                    return FAILURE;
                                break;
                        case LONG_CONST:
                                if(RGEqualRegNum(mOpr->opr.i,pOpr->opr.l)==FAILURE)
                                    return FAILURE;
                                break;
                        case INDIRECT_REG:
                                if(RGEqualRegReg(mOpr->opr.i, pOpr->opr.i)==FAILURE)
                                    return FAILURE;
                                break;
                        case CHAR_CONST :
                                if(RGEqualRegNum(mOpr->opr.i, (long)pOpr->opr.i&0x000000FF)==FAILURE)
                                    return FAILURE;
                                break;
                        default:
                                return FAILURE;
                    }
                }
                else       /* S-register or string value arguement */
                {
                    switch (pOpr->type)
                    {
                        case STRING_CONST:
                                if(RGEqualSRegStr(mOpr->opr.i, pOpr->opr.s)==FAILURE) return FAILURE;
                                break;
                        case INDIRECT_STR:
                                if(RGEqualSRegSReg(mOpr->opr.i, pOpr->opr.i)==FAILURE) return FAILURE;
                                break;
                        default:
                                return FAILURE;
                    }
                }
                pOpr = pOpr->next;
                mOpr = mOpr->next;

            }  /* end of for loop */
        }
        else
        {
            if(!iExecutingMacro)
            {
                // Handle Special Customer's registers
                if (RGInitSpecialRegister() == FAILURE)
                    return FAILURE;
                for ( i = 1; (i<= instrArg->nofopr) && (i<MAXSPECIALREG); i++)
                {
                    switch(pOpr->type)
                    {
                        case CHAR_CONST :
                            /* Save it into Special register for NPAR */
                            if(RGEqualSpecialRegNum(i,(long)pOpr->opr.i)==FAILURE)
                                return FAILURE;
                            break;
                        case INDIRECT_CHAR :
                            if(RGGetCharacter(pOpr->opr.i,&cTemp)==FAILURE)
                                return FAILURE;
                            /* Save it into Special register for NPAR */
                            if(RGEqualSpecialRegNum(i,(long)cTemp)==FAILURE)
                                return FAILURE;
                            break;
                        case BR_CONST :
                            /* Save it into Special register for NPAR */
                            if(RGEqualSpecialRegNum(i,pOpr->opr.l|BR_FLAG)==FAILURE)
                                return FAILURE;
                            break;
                        case LONG_CONST :
                            /* Save it into Special register for NPAR */
                            if(RGEqualSpecialRegNum(i,pOpr->opr.l)==FAILURE)
                                return FAILURE;
                            break;
                        case INDIRECT_REG :
                            /* Save it into Special register for NPAR */
                            if(RGEqualSpecialRegReg(i,pOpr->opr.i)==FAILURE)
                                return FAILURE;
                            break;
                        default:
                            return FAILURE;
                    }
                    pOpr = pOpr->next;
                }   /* end for loop */
                /* i-1 is number of operands - string name */
                /* Save it into Special register for NPAR */
                if(RGSetSpecialRegister(0, i-1)==FAILURE)
                    return FAILURE;
            }
        }
    }       /* ( instr != secsPC) */
    /* Set up PC */
    if (MRLaunchMacro(instr))
        return FAILURE;
    TKSetCurrentMacro( MacroEntry.OC );
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MRExecuteInstructions
 * Description:     This the function that does the real execution of each line of macro
 *                  code or low-level command. It also prints the tracing macro if trace mode is on.
 *                  If the executing the instruction in the macro, then
 *                  setup the next instruction to PC for next execution by the time go back
 *                  to the mainloop. Before executing either macro or low-level command, it needs to
 *                  validate the opcode by get the entry from oc table. If there is tracing mode, then
 *                  list the instruction and execute either macro or low-level command. To determine either
 *                  macro or low-level command by checking the flag in the macro entry that get from oc
 *                  table. If there is error, set the status bit to be "CMD_NOT_EXED". If the macro is running
 *                  and error then set the CMER interrupt flag on. So the CMER interrupt macro can execute.
 *                  However, if there is no CMER interrupt macro, then we need to do the clean up like
 *                  MREndMacro such kill current task and set PC to NULL.  In the case of there is CMER interrupt
 *                  macro, by the time the macro finished executing at MREndMacro will kill current task and
 *                  set PC to NULL.
 * return:          SUCCESS or FAILRUE
 * **************************************************************************************************/
int MRExecuteInstructions(instr_ptr instrArg, int iCheckPCArg)
{
    int iRc, iFlag;
    pstTaskList  TaskList;
    pstTaskList pCurrentTaskPtr;
    OC_TABLE OCEntry;
    int iTaskIdTemp;
    int iPortNum;                       /* CmdPort number */

    TKSetPCCriticalSection( 1 );
    if( (instrArg == NULL) || ((iCheckPCArg == TRUE) && (PC == NULL)) )
    {
        iRc = SUCCESS;
        goto exit_point;
    }

    if (iExecutingMacro)
    {
	oldPC = PC;		/* save for RLN command */
	PC = PC->next_instr;
    }
    iRc = SUCCESS;
    /* Check for invalid opcode -- e.g., deleted macro */
    if (instrArg->OC == INVALID_OPCODE)
    {
        iRc = FAILURE;
	goto exit_exec;
    }
    if(CMDcommands(CMDGET, instrArg->OC, &OCEntry))  /* get macro entry from the table */
    {
        iRc = FAILURE;
        goto exit_exec;
    }
    if(!(&OCEntry))                     /* validate the entry to make sure it's not NULL */
    {
        iRc = FAILURE;
        goto exit_exec;
    }

    iFlag = OCEntry.flag;
    if ( !(iFlag & CMD_VALID_IN_MOTION) )
    {
        if ( TIGetTimerVals(LOCKED) == 123 )
        {
//printf("Not valid in motion: OC=%d line=%s\n",OCEntry.OC, OCEntry.OC_str);
            iRc = FAILURE;
            goto exit_exec;
        }
    }

    /* For tracing macro */
    if ((iFlag & CMD_DEFINE_MACRO) && (iTraceMode != 0))
        iMacroTracing = TRUE;
    /* for the case, the macro stop in the middle of execution and
    * next time to execute the low level command the flag need to
    * set back to false. So trace won't show up while you execute the
    * low level command from the command line. The trace is only for debug
    * macro.  It traces the macro while macro is executing. */
    else if(!iExecutingMacro)
        iMacroTracing = FALSE;

    if(iMacroTracing)
    {
        if( iTraceMode == 1 )   /* print all the instructions */
        {
            iRc = MADisplayMacroTrace(instrArg);
        }
    }
    /* Call the appropriate execution routine */
    if (iFlag & CMD_DEFINE_MACRO)
    {
        if(iTraceMode ==2)              /* print only the macro call */
        {
            iRc = MADisplayMacroTrace(instrArg);
        }
        iRc = MRExecuteMacro(instrArg);  /* execute the macro */
    }
    else
    {
        iRc = (*OCEntry.exec)(instrArg);  /* execute the instruction from low-level commad such ex_XXX */
    }

exit_exec:
    if (iRc)
    {
	SSSetStatusWord(CMD_NOT_EXED, TRUE);
//printf("CMD_NOT_EXED: OC=%d line=%s\n",OCEntry.OC, OCEntry.OC_str);
    }
    else
    {
       if (instrArg->OC != STAT)
			SSSetStatusWord(CMD_NOT_EXED, FALSE);
    }

    if (iExecutingMacro && iRc)
    {
	iAbortLineNumber = instrArg->line_number;
	/* If nobody else said why we're aborting, it must be OTHERS */
        /* (Note that uMacroStatus could be STACK_OVERFLOW here) */
	if (uMacroStatus == MACRO_RUNNING)
{
	    uMacroStatus = OTHERS;
//printf("macroStat=%d\n",uMacroStatus);
}
        if(iMacroTracing) /* reset the flag if the executing of macro is fail */
            iMacroTracing = FALSE;
    }
    if( iRc == FAILURE )
	{
        /* Implement CMER interrupt */
        if ( iExecutingMacro ) /*cmer interrupt is only allowed in macro */
        {
            // instruction aborted due to abort_macro?
            if ((iCheckPCArg == TRUE) && (PC == NULL))
            {   // Then, do not cause CMER to execute
                iRc = SUCCESS;
                goto exit_point;
            }
            // CMER should not generate another CMER
            if (iRunningCMERMacro)
            {
                ex_ENDM(NULL);
                goto exit_point;
            }

           if ( MRGetCMEFlag() &&
               (cmerPC == NULL || !MRGetMacroIntegerVars(MR_CMER_ENABLED)))
           {
               /* Should we return "?" */
               iPortNum = SERGetCmdPort();
               SERPutsTxBuff(iPortNum, "\r\n?");
           }

           /* if running the interrupt macro and error occurred, make sure the iEnterInterruptSectionFlag is
            * reset back, so the next interrupt can get in.*/
           //if(iEnterInterruptSectionFlag)
           //     iEnterInterruptSectionFlag = FALSE;
           MASetMacroIntegerVars(MR_CMER_INTERRUPT, TRUE);
           MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);

           /* If debug bit is on, list the instruction to comport */
           if (SSIsDebugBitOn(CMER_DEBUG_BIT))
               MRListInstructions(instrArg, TRUE);

           /* in the case, if there is command error but there is not CMER interrupt macro to execute,
            * then you want to make sure the macro stop executing after the error
            * Also, if CMER Interrupt is not enabled, it should stop executing.   */
           if (cmerPC == NULL || !MRGetMacroIntegerVars(MR_CMER_ENABLED))
           {
                TKKillCurrentTask(TRUE);
                PC = NULL;
                TaskList = TKGetNextTask();
                if( TaskList )
                {
                    TISetTimerVals(TIMER_INTERRUPT, TRUE);
                }
                MASetFlagsToFalse();
           }

           if ((pCurrentTaskPtr = TKGetCurrentTask()))
           {
               TaskList = TKGetTaskListArray();   /* First task in the array */
               iTaskIdTemp = (int)(pCurrentTaskPtr - TaskList);
               /* Set the Task ID for  ex_TSKI */
               TKSetTaskId(iTaskIdTemp);
           }
        }
    }
exit_point:
    TKSetPCCriticalSection( -1 );
    return iRc;
}

/**************************************************************************************************
 * Function Name:   MRListInstructions
 * Description:     List the instructions is using to print out the macro code for tracing and also for
 *                  LMCR macro_name. if the trace flag is on then listing the instruction for the
 *                  trace, else it lists for LMCR command. When listing the instruction (a line of
 *                  macro code inside macro body), go through each operand and print out the operand content
 *                  according to their type. If the opcode of the instruction is INVALID_OPCODE, then print
 *                  out "????".
 *
 * Parameters:      instrArg    - instruction for listing
 *                  iTraceFlagArg - flag for checking trace mode.
 * returns:         SUCCESS or FAILURE
 * **************************************************************************************************/
int MRListInstructions(instr_ptr instrArg, int iTraceFlagArg)
{
    CMDoperand_ptr Operand;
    int iOpcode, nOpr, iComma;
    char caBuf[MAXLINE * 2];
    OC_TABLE MacroEntry;
    char caBRString[10];
    int iCmdPort, iTTPort;

    iCmdPort = SERGetCmdPort();
    iTTPort = SERGetTTPort();            /* for teach pedant when listing macro */

    caBRString[0] = 0;
    if(iTraceFlagArg)                   /* if the trace is on, set port to specified port for tracing */
        iCmdPort = iTracePort;
    else
        iCmdPort = SERGetCmdPort();
//    caBuf = (char *)malloc(MAXLINE * 2);
//    if(!caBuf)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***MRListInstructions:  out of memory when doing malloc for string buffer.***\n\r?");
//#endif
//        return FAILURE;
//    }
    nOpr = 0;
    iOpcode = instrArg->OC;
    if (iOpcode != INVALID_OPCODE)
	{
        if(CMDcommands(CMDGET, iOpcode, &MacroEntry))
            goto error_exit;
        if (&MacroEntry)                 /* to a deleted macro */
            strcpy(caBuf, MacroEntry.OC_str);  /* copy the macro name or command string */
		else
			strcpy(caBuf, "?????");
    }
    else
		strcpy(caBuf, "?????");

    if (iOpcode != EQUAL)
    {
        strcat(caBuf, " ");             /* add a space */
        if (SERPutsTxBuff(iCmdPort, caBuf) == FAILURE) goto error_exit;    /* print opcode */
        if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;
    }
    /* list operands */
    Operand = instrArg->opr_ptr;
    while (Operand)
	{
//        if(TIRefreshWD()==FAILURE) goto error_exit;
		nOpr++;
		iComma = TRUE;
        switch(Operand->type)
		{
            case CHAR_CONST:
                    if (iOpcode == EQUAL)
                        sprintf(caBuf, "'%c'", Operand->opr.i);
                    else
                        sprintf(caBuf, "%c", Operand->opr.i);
                    break;
            case LONG_CONST:
                    sprintf(caBuf, "%ld", Operand->opr.l);
                    break;
            case BR_CONST :
                    CMDgetBRspecialString((int)Operand->opr.l, caBRString);
                    sprintf(caBuf, "%s", caBRString);
                    break;
            case INDIRECT_REG:
                    if (iOpcode == EQUAL)
                        sprintf(caBuf, "R%d", Operand->opr.i);
                    else
                        sprintf(caBuf, "[R%d]", Operand->opr.i);
                    break;
            case INDIRECT_CHAR:
                    if (iOpcode == EQUAL)
                        sprintf(caBuf, "C%d", Operand->opr.i);
                    else
                        sprintf(caBuf, "[C%d]", Operand->opr.i);
                    break;
            case STRING_CONST:
//                    sprintf( caBuf, "\"%Fs\"",Operand->opr.s );
                    sprintf( caBuf, "\"%s\"",Operand->opr.s );
                    break;
            case INDIRECT_STR:
                    if (iOpcode == EQUAL)
                    {
                        if(iEmulatorFlag & DFEMULATORM)
                        {
                            sprintf(caBuf, "Q%d", Operand->opr.i);
                        }
                        else
                        {
                            sprintf(caBuf, "S%d", Operand->opr.i);
                        }
                    }
                    else
                    {
                        if (iEmulatorFlag & DFEMULATORM)
                        {
                            sprintf(caBuf, "[Q%d]", Operand->opr.i);
                        }
                        else
                        {
                            sprintf(caBuf, "[S%d]", Operand->opr.i);
                        }
                    }
                    break;
            case MACRO_NUM:
                    sprintf(caBuf, "#R%d", Operand->opr.i);
                    break;
            case MACRO_ALPHA:
                    sprintf(caBuf, "#C%d", Operand->opr.i);
                    break;
            case MACRO_STR:
                    sprintf(caBuf, "#S%d", Operand->opr.i);
                    break;
            case LLMATH_OPR:
                    switch (Operand->opr.i)
                    {
                        case '+':
                                strcpy(caBuf, "+");
                                break;
                        case '-':
                                strcpy(caBuf, "-");
                                break;
                        case '*':
                                strcpy(caBuf, "*");
                                break;
                        case '/':
                                strcpy(caBuf, "/");
                                break;
                        case '&':
                                strcpy(caBuf, "&");
                                break;
                        case '|':
                                strcpy(caBuf, "|");
                                break;
                        case '>':
                                strcpy(caBuf, ">>");
                                break;
                        case '<':
                                strcpy(caBuf, "<<");
                                break;
                    }
                    break;
            case JUMP_DEST:   /* Don't print these type operands */
                    iComma = FALSE;
                    caBuf[0] = 0;
                    break;
            default:
                    strcpy(caBuf, "???");
                    break;
		}
		Operand = Operand->next;
		if (Operand != NULL)
		{
			if (iOpcode == EQUAL)
                strcat(caBuf, " ");
			else if (iComma)
				strcat(caBuf, ", ");
		}

		if (iOpcode == EQUAL && nOpr == 1)
			strcat(caBuf, "= ");

        if (SERPutsTxBuff(iCmdPort, caBuf) == FAILURE) goto error_exit;
        if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;
    }  /* end of  while(p_opr)	*/

    if(iCmdPort == iTTPort)
        CMDdumpBuf("\r");
    else if (SERPutsTxBuff(iCmdPort, "\r\n") == FAILURE)
        goto error_exit;
    else if (SERFlushTxBuff(iCmdPort) == FAILURE)
        goto error_exit;

//    if(caBuf)
//        free(caBuf);
    return SUCCESS;
error_exit:
//    if(caBuf)
//        free(caBuf);
    return FAILURE;
}

/**************************************************************************************************
 * Function Name:   MRAbortMacro
 * Description:     Aborts macro and saves line number which aborted--used by ABM & STOP.
 *                  Abort the macro by initializing the stack. In initialization of the stack, it sets
 *                  PC to NULL with mean stop executing the current instruction. Then kill all the
 *                  tasks that are currently running and set the macro status bit to "ABORTED_BY_USER".
 *                  Also, reset the flag of CMER, SSTOP, and INTLK to not running and set the
 *                  current task id to be -1.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int MRAbortMacro()
{
    int iReturn;

    TKSetPCCriticalSection( 1 );

	giMapTest = 0;		// mapping IO stopped
	giMapIORunning = 0;

    if (PC != NULL)
    	iAbortLineNumber = PC->line_number;
    iReturn = MAInitStack();                    /* initialize the stack */
    if(iReturn == FAILURE)
        goto Exit_point;
    uMacroStatus = ABORTED_BY_USER;             /* set the macro status */
    iReturn = TKKillTask(FALSE, -1, NULL);      /* kill all the current running tasks */
    if(iReturn == FAILURE)
        goto Exit_point;
    MPSetNotScanning();
Exit_point:
    TKSetPCCriticalSection( -1 );
    giMeasurementInProgress = 0;	// stop alignment if running, that kills the thread.
    iRunningCMERMacro = FALSE;                   /* CMER is not running */
    iRunningSSTOPMacro = FALSE;                  /* SSTOP is not running */
    iRunningINTLKMacro  = FALSE;                 /* INTLK is not running */
    iPendingSSTOPMacro = FALSE;
    iPendingINTLKMacro = FALSE;
    TKSetTaskId(-1);                             /* set task id to invalid (-1) */

    return iReturn;
}

/**************************************************************************************************
 * Function Name:   MAFindOpcode
 * Description:     This function gets opcode by passing in the string name. In low-level command will
 *                  search in the oc table by string name (macro or low-level command name). If the oc entry is
 *                  returned with NULL then we know that the macro with given name does not exist. Then return the
 *                  INVALID_OPCODE; otherwise return opcode (OC).
 * Parameters:      pcOpcodeNameArg - command line
 *                  piOpcodeArg     - return opcode
 * Return:          pointer to next string in the pcOpcodeNameArg string
 * **************************************************************************************************/
char *MAFindOpcode(char *pcOpcodeNameArg, int *piOpcodeArg)
{
    OC_TABLE MacroEntry;
    int iReg;

   /* get the opcode by parsing the command line */
    pcOpcodeNameArg = CMDget_opcode(pcOpcodeNameArg, &MacroEntry, &iReg, FALSE);
    if(&MacroEntry)
        *piOpcodeArg = MacroEntry.OC;   /* return opcode */
    else
        *piOpcodeArg = INVALID_OPCODE;  /* return INVALID_OPCODE, macro does not exist */
    return pcOpcodeNameArg;
}

/**************************************************************************************************
 * Function Name:   MAInitStack
 * Description:     Initialize macro stack also used to abort all macros.
 *                  To initialize the stack, set top of the stack to the begining of the stack,
 *                  set the PC to NULL and set the macro is not waiting.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int MAInitStack()
{
    TKSetPCCriticalSection( 1 );
    psStkTop = staPStat;                /* initialize the top of stack */
    PC = NULL;
    lCf = 0;
    iFreezeMacro = FALSE;
    iPCWaiting = FALSE;
    TKSetPCCriticalSection( -1 );
    return SUCCESS;
}

/**************************************************************************************************
 * Function Name:   MRPushPc
 * Description:     Push instruction (PC and cf) onto macro stack when macro calls another macro.
 *                  Put the PC and OldPC information into the top of the stack and increment the
 *                  top of stack pointer. For multi-tasking, put the information of macro instruction
 *                  into stack of specified task. Otherwise, put the information into local stack.
 *                  When put the structure into the top of stack, need to save the top of stack into
 *                  psStkBeforeInit for RLN command to read line number if the macro is failed.
 * Note:            About the stack, the stack is an array of 30 structure of program status. It
 *                  is (LIFO) last in first out.  Program status is structure that contains
 *                  PC and OldPc (instruction pointers) and other macro status flag which is needed
 *                  to be verified with Steven The.
 *
 * return:          SUCCESS or FAILURE if stack overflow
 * **************************************************************************************************/
int MRPushPC(pstTaskList pCurrentTaskArg)
{
    int iReturn = SUCCESS;

    TKSetPCCriticalSection(1);
    if( pCurrentTaskArg )     /*for multitasking */
    {
        if( pCurrentTaskArg->m_pStackTop < (pCurrentTaskArg->m_pStatus)+MAXSTACK )
        {
            pCurrentTaskArg->m_pStackTop->m_PC  = PC;
            pCurrentTaskArg->m_pStackTop->m_OldPC  = oldPC;   /* needed for RLN command */
            pCurrentTaskArg->m_pStackTop->m_lCf  = lCf;
            pCurrentTaskArg->m_pStackTop->m_iFrzStatus = iFreezeMacro;
            pCurrentTaskArg->m_pStackTop->m_iMacStatus = uMacroStatus;
            (pCurrentTaskArg->m_pStackTop)++;
            goto exit_point;
        }
    }
    else        /*for no task application  */
    {
        if( psStkTop < staPStat+MAXSTACK )
        {
            psStkTop->m_PC = PC;
            psStkTop->m_OldPC = oldPC;   /* needed for RLN command */
            psStkTop->m_lCf = lCf;
            psStkTop->m_iFrzStatus = iFreezeMacro;
            psStkTop->m_iMacStatus = uMacroStatus;
            psStkTop++;
            psStkTopBeforeInit = psStkTop;
            goto exit_point;
        }
    }
    uMacroStatus = STACK_OVERFLOW;
    iReturn = FAILURE;
exit_point:
    TKSetPCCriticalSection( -1 );
    return iReturn;
}

/***********************************************************************************************
 * Function Name:       MRPopPC
 * Description:         Pop instruction (PC and cf) from macro stack when end of macro is reached.
 *                      Get macro instruction from the current top of stack and update the PC and
 *                      OldPC to continue running. If there is no instruction to pop then there is
 *                      no more instruction to run then set PC and OldPC to NULL. This function is
 *                      called in MREndMacro.  For multi-tasking, get macro instruction from stack of
 *                      specified task. Otherwise, just get from local stack.
 * Note:                About the stack, the stack is an array of 30 structure of program status. It
 *                      is (LIFO) last in first out.  Program status is structure that contains
 *                      PC and OldPc (instruction pointers) and other macro status flag which is needed
 *                      to be verified with Steven The.
 * return:              SUCCESS or FAILURE
 * ***********************************************************************************************/
int MRPopPC(pstTaskList pCurrentTaskArg)
{
    int iReturn = SUCCESS;

    TKSetPCCriticalSection( 1 );
    if( pCurrentTaskArg )        /* multi-tasking */
    {
        if( pCurrentTaskArg->m_pStackTop > pCurrentTaskArg->m_pStatus )  /* there are more to pop */
        {
            (pCurrentTaskArg->m_pStackTop)--;
            PC = pCurrentTaskArg->m_pStackTop->m_PC;                    /* set the PC for executing */
            iFreezeMacro = pCurrentTaskArg->m_pStackTop->m_iFrzStatus;
            uMacroStatus = pCurrentTaskArg->m_pStackTop->m_iMacStatus;
            oldPC = pCurrentTaskArg->m_pStackTop->m_OldPC;              /* set the OldPC for RLN */
            lCf = pCurrentTaskArg->m_pStackTop->m_lCf;
        }
        else                            /* no more macro instruction in the stack. */
        {                               /* Then initialize the PC to NULL and OldPc points to
                                         * tempOldPC.  In the tempOldPC structure all the member is already initialized
                                         * to zero but doing the memset at the initailization of macro module. */
            oldPC=tempOldPC;
            PC=NULL;
            lCf = 0;
        }
    }
    else   /* no multi-tasking */
    {
        if( psStkTop > staPStat )
        {
            psStkTopBeforeInit = psStkTop;
            psStkTop--;
            PC = psStkTop->m_PC;        /* update current PC for executing */
            if(!PC)
            {
                iReturn = FAILURE;
                goto exit_point;
            }
            iFreezeMacro = psStkTop->m_iFrzStatus;
            uMacroStatus = psStkTop->m_iMacStatus;
            oldPC = psStkTop->m_OldPC;  /* update the current oldPC for RLN*/
            lCf = psStkTop->m_lCf;
        }
       /* top is equal to begining of the stack */
        else                            /* no more macro instruction in the stack. */
        {                               /* Then initialize the PC to NULL and OldPc points to
                                         * tempOldPC.  In the tempOldPC structure all the member is already initialized
                                         * to zero but doing the memset at the initailization of macro module. */
            oldPC = tempOldPC;          /* initialize the oldPC to all zero, for RLN only */
            PC = NULL;
            lCf = 0;
        }
    }
exit_point:
    TKSetPCCriticalSection( -1 );
    return iReturn;
}

/*************************************************
 * Function Name:   MRGetPStatus
 * Description:     Get the first pointer address of program status
 * return:          first pointer to the program status
 * *************************************************/
pstProgStatus MRGetPStatus()
{
    return staPStat;
}

/*************************************************
 * Function Name:   MRGetStkTop
 * Description:     Get address of the top stack
 * return:          pointer to the stack
 * *************************************************/
pstProgStatus MRGetStkTop()
{
    return psStkTop;
}

/*************************************************
 * Function Name:   MASetSavedStkTop
 * Description:     Set the address of saved stack top
 * Returns:         none.
 * *************************************************/
void MASetSavedStkTop(pstProgStatus psProgStatusArg)
{
    psSavedStkTop = psProgStatusArg;
}

/**************************************************************************************************
 * Function Name:   MREndMacro
 * Description:     For ex_ENDM to call.
 *                  This function is where to indicate the ending of macro and need to do all clear up
 *                  and set up for next execution.
 *                  By the time the macro is ended, we need to do:
 *                  1. Turn off the iMacroTracing false if the tracing is on.  So by the time macro completely
 *                     finished executing, you can execute low-level command without trace line printing out.
 *                     Tracing feautre is only trace macro and not regular command line execution such as only
 *                     low-level comamnd.
 *                  2. Determine the end of macro mode (lEndMModeArg) to end the macro.  Mode is 3 usually for
 *                     interrupt macro and it will clear the macro stack and kill all the tasks after the macro is
 *                     done. On the other mode, all you need to do is to pop the instruction from the stack. The pop
 *                     function will set the current instruction either to next instruction or NULL. However, if ENDM with
 *                     mode 1, we need to check for Imform After Macro (IAM) flag to print out proper prompt with or without
 *                     status.
 *                  3. Check for the finished macro is an INTLK, SSTOP or CMER interrupt macro.
 *                     - If INTLK interrupt macro, check for any pending INTLK or SSTOP interrupt macro, then we can continue
 *                       to execute the pending interrupt macro by reset the interrupt flag to TRUE and when it gets back to
 *                       mainloop in the PCProcessMacroInstr() will continue executing the pending macro.
 *                     - If SSTOP interrupt macro, check for any pending SSTOP interrupt. If there is, then set the
 *                       interrupt flag to continue executing the pending SSTOP interrupt macro.
 *                     - If CMER interrupt macro and without multi-tasking set the PC to NULL, so current running macro is
 *                       stop and reset all the interrupt flag to default(FALSE).  Then set back the  enter interrupt section
 *                       flag to FALSE, so next interrupt can come in. For multi-tasking, if the current task is pending killed
 *                       and CMER is running, then kill current task, and the rest the same as no multi-tasking.
 *                  4. By now the PC is NULL if no pending interrupt excecuting, then initialize the psStkBeforeInit to NULL,
 *                     get the next task. If there is next task then set the interrupt timer. Otherwise, no more
 *                     multi-tasking, then reset the enter interrupt flag and set no-task-flag to TRUE.
 *
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int MREndMacro(long lEndMModeArg )
{
    int iStatus;
    char caBuf[55];
    pstTaskList pCurrentTaskTemp;
    //pstProgStatus  pstkCurrentStackTop;
    int iReturn = SUCCESS;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    TKSetPCCriticalSection(1);
    pCurrentTaskTemp = TKGetCurrentTask();

//    if(iTraceMode && iMacroTracing)
//        iMacroTracing = FALSE;
    /* lEndMModeArg is value which determine how to end the macro.
     * if the lEndMModeArg = 3 usually uses for interrupt macro.  With this mode
     * it will initialize the macro stack and set the controller to idle by killing
     * all the tasks.  The controller will not execute interrupted macro. */
    if (lEndMModeArg == 3)
    {
        TKKillTask(FALSE, -1, NULL);    /* kill all the task */
        iAbortLineNumber = psStkTop == staPStat;
        if((iReturn=MAInitStack()))     /* initialize the stack */
            goto exit_error;
    }
    /* in this case, the lEndMModeArg is 0,1, or 2.  These value is a standard
     * ending macro. We need to pop the macro instruction from the stack.
     *  0 - Standard (non-interrupt) macro - no status sent
     *  1 - Standard (non-interrupt) macro - status sent
     *  2 - Exits from interrupt macro, and returns to the interrupted macro if
     *      there is any macro executed. */
    else
    {
        iAbortLineNumber = 0;
        if((iReturn=MRPopPC(pCurrentTaskTemp)))  /* pop current task */
            goto exit_error;
    }
    /* Send status if ENDM 1 and we're not returning to another macro.
     * The iIAMMode is set before execute macro that has ENDM 1. After executed
     * the macro, it depends on the value of the mode to print out the prompt and
     * with or without status.
     * iIAMMode = 1:    shows the status with "\r\n>"
     * iIAMMode = 2:    shows the status of GenMark with "\r\n>"
     * iIAMMode = 3:    shows the "\r\n>". */
    if (lEndMModeArg == 1  &&  PC == NULL)
    {
        switch (iIAMMode)
        {
			case 1:
                iStatus = SSGetSystemStatus();
                sprintf(caBuf, "%04X\r\n>", iStatus);
                SERPutsTxBuff(iCmdPort, caBuf);
				break;
			case 2:
                iStatus = SSGetGENSystemStatus(TRUE);
                sprintf(caBuf, "%04X\r\n>", iStatus);
                SERPutsTxBuff(iCmdPort, caBuf);
				break;
			case 3:
                SERPutsTxBuff(iCmdPort, "\r\n>");
				break;
            case 0:                     /* default don't print anything. */
			default:
				break;
		}
        SERFlushTxBuff(iCmdPort);
    }

    --iLevelOfNestingForIntMacros;  /* Decrement nest level whenever ending a macro */

    /* For multi-tasking and without multi-tasking, the INTLK interrupt finished executing, but we need to check if
    * there is a pending INTLK interrupt macro, then run the pending INTLK interrupt macro.
    * If there is no pending INTLK interrupt macro, there is a pending SSTOP interrupt macro,
    * then start executing the pending SSTOP interrupt macro.  The INTLK interrupt always has the higher
    * priority than the SSTOP. if there is pending INTLK interrupt macro, set the interrupt flag to TRUE,
    * so we can continue to execute the pend INTLK interrupt macro.*/
    if(TKReadNoTaskFlag())              /* without multi-tasking */
    {
        //pstkCurrentStackTop = psStkTop;
        //if(psSavedStkTop == pstkCurrentStackTop)
        if (iLevelOfNestingForIntMacros <= 0)
        {
            /* CMER interrupt is running and at finished state now, so we need stop the current executed macro by
             * setting the PC to NULL and reset all interrupt flag to FALSE. Then any interrupt can get interrupt again */
            if(iRunningCMERMacro)
            {
                PC = NULL;
                iERRInterrupt = FALSE;
                iINTLKInterrupt = FALSE;
                iSSTOPInterrupt = FALSE;
                MASetFlagsToFalse();
            }
            else if(iRunningINTLKMacro)
            {
                if (iPendingINTLKMacro)
                {
                    iINTLKInterrupt = TRUE;
                }
                iSSTOPInterrupt = FALSE;
                iERRInterrupt = FALSE;
                iRunningINTLKMacro = FALSE;
                iPendingINTLKMacro = FALSE;
                iRunningSSTOPMacro = FALSE;
                iPendingSSTOPMacro = FALSE;
                iRunningERRMacro = FALSE;
                iPendingERRMacro = FALSE;
            }
            else if(iRunningSSTOPMacro)
            {
                if (iPendingSSTOPMacro)
                {
                    iSSTOPInterrupt = TRUE;
                }
                iERRInterrupt = FALSE;
                iRunningSSTOPMacro = FALSE;
                iPendingSSTOPMacro = FALSE;
                iRunningERRMacro = FALSE;
                iPendingERRMacro = FALSE;
            }
            else if (iRunningERRMacro)
            {
                if (iPendingERRMacro)
                {
                    iERRInterrupt = TRUE;
                }
                iRunningERRMacro = FALSE;
                iPendingERRMacro = FALSE;
            }
            /* this flag needs to reset to false so any interrupt can come into the interrupt section when there is an
             * interrupt generated. */
            iEnterInterruptSectionFlag = FALSE;
        }
	}
    else if( pCurrentTaskTemp )         /* with multi-tasking */
    {
        //pstkCurrentStackTop =  pCurrentTaskTemp->m_pStackTop;
        //if(psSavedStkTop == pstkCurrentStackTop)
        if (iLevelOfNestingForIntMacros <= 0)
        {
            /* CMER interrupt is running and at finished state now, so we need stop the current executed macro by
             * setting the PC to NULL, kill the current task and reset all interrupt flag to FALSE.
             * Then any interrupt can get interrupt again */
            if(iRunningCMERMacro || pCurrentTaskTemp->m_iTaskPendingKilled )
            {
                TKKillCurrentTask(FALSE);   /* kill current task */
                PC = NULL;
                iERRInterrupt = FALSE;
                iINTLKInterrupt = FALSE;
                iSSTOPInterrupt = FALSE;
                MASetFlagsToFalse();        /* reset all interrupt flag to FALSE */
            }
            else if(iRunningINTLKMacro)
            {
                if (iPendingINTLKMacro)
                {
                    iINTLKInterrupt = TRUE;
                }
                iSSTOPInterrupt = FALSE;
                iERRInterrupt = FALSE;
                iRunningINTLKMacro = FALSE;
                iPendingINTLKMacro = FALSE;
                iRunningSSTOPMacro = FALSE;
                iPendingSSTOPMacro = FALSE;
                iRunningERRMacro = FALSE;
                iPendingERRMacro = FALSE;
            }
            else if(iRunningSSTOPMacro)
            {
                if (iPendingSSTOPMacro)
                {
                    iSSTOPInterrupt = TRUE;
                }
                iERRInterrupt = FALSE;
                iRunningSSTOPMacro = FALSE;
                iPendingSSTOPMacro = FALSE;
                iRunningERRMacro = FALSE;
                iPendingERRMacro = FALSE;
            }
            else if (iRunningERRMacro)
            {
                if (iPendingERRMacro)
                {
                    iERRInterrupt = TRUE;
                }
                iRunningERRMacro = FALSE;
                iPendingERRMacro = FALSE;
            }
            /* this flag needs to reset to false so any interrupt can come into the interrupt section when there is an
             * interrupt generated. */
            iEnterInterruptSectionFlag = FALSE;
        }
        /* if not an interrupt but current task is pending killed then
         * kill current task and set PC to NULL. */
        if( pCurrentTaskTemp->m_iTaskPendingKilled )
        {
            TKKillCurrentTask(FALSE);
            PC = NULL;
        }
    }
    if ((PC == NULL) || ((PC->OC == SECS) && (S2GetSecsSpecial() == TRUE) && (iLevelOfNestingForIntMacros <= 0)))
    {
        uMacroStatus = MACRO_IDLE;
         /* No mode 3 or no macro interrupted */
        if( lEndMModeArg != 3 )
            psStkTopBeforeInit = NULL;  /* in the old code, this pointer got set the begining of stack, but it's not
                                         * working properly for RLN command.  We need to set to NULL, then when
                                         * call MAReadMacroLineNumber and macro is running ok. The RLN command should
                                         * give 0.  Inorder to do that in the code when trying command this pointer with
                                         * top of stack, it will not equal and it will go to local oldPc to get the line
                                         * number and line number is already intialize to zero. */
        /* get the next task and if not NULL, then
         * set interrupt timer on. */
        if( (pCurrentTaskTemp=TKGetNextTask()) )
            TISetTimerVals(TIMER_INTERRUPT, TRUE);
        /* if next task is NULL, that's meant there is no more multi-task,
        * reset the flag to no multi-task and set the enter interrupt section flag to FALSE */
        else
        {
            iEnterInterruptSectionFlag = FALSE;
            TKSetNoTaskFlag(TRUE);
        }
        MASetFlagsToFalse();
    }
exit_error:
    TKSetPCCriticalSection( -1 );
    return iReturn;
}

/**************************************************************************************************
 * Function Name:   MAReadMacroLineNumber
 * Description:     For ex_RLN to call. It's just a debug feature for macro.
 *                  It will give the line number that macro got error while executing.
 *                  As you know every time running a macro and inside the running macro has another macro,
 *                  the instruction of the macro will be pushed to the stack.  At that time there is a variable
 *                  (psStkBeforeInit) which will save the top of stack. By the time you get the error the stack
 *                  will be initialized and psStkBeforeInit already saved the top of stack. We will use that top with
 *                  the beginning of the stack to get all the line numbers.  If the macro executed successfully, in
 *                  MREndMacro function will pop the stack, initialize the PC and OldPc to NULL and psStkBeforeInit
 *                  also set to NULL. After execute the macro, execute the RLN command. The psStkBeforeInit should be 0 now
 *                  and it will be less than the beginning of the stack when doing comparison; therefore, we just print
 *                  out the line number of local global OldPC, which should be 0.  In the case of error while executing,
 *                  we print out the line number while the beginning of the stack is less than the top of
 *                  stack (psStkBeforeInit). The last line number that is printed is also the current executed line
 *                  and error line.
 *
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int MAReadMacroLineNumber()
{
    pstProgStatus  pstkStatus, pTopStack;
    char caBuf[120];

    TKSetPCCriticalSection( 1 );
     /* set the top of stack to where it stopped executing or when macro
      * ran successfuly the psStkTopBeforeInit should be NULL. */
    if (PC == NULL)
        pTopStack = psStkTopBeforeInit;  /* this is same as top that you already
                                          * save at PushPc and initialize at MREndMacro.*/
    else
        pTopStack = psStkTop;           /* current top of stack since PC is not NULL which mean there is some thing running. */
    caBuf[0] = 0;
    pstkStatus = staPStat;              /* set the beginning pointer of the stack. */
    /* compare while beginning of stack is less then top of stack.
     * if less than, then you know that inside the macro it called other macros.*/
    MAFindPath(pstkStatus, pTopStack, caBuf);

    /* after the beginning of the stack either equal or greater than the top of stack, it got out the loop.
     * Either there is no macro push or there is only the macro is currently running. If the PC is not NULL,
     * the macro is still running, so you get the local global OldPC instruction to get the currently line
     * number that is running. */
	if( PC )
	    sprintf(caBuf+strlen(caBuf), "%d", oldPC->line_number);
//          itoa(oldPC->line_number, caBuf+strlen(caBuf), 10);
   /* if the PC is NULL, that mean the macro finished running or
    * it got killed after getting the command error.*/
    else
    {
        /* there is the last instruction in the stack need to get the line number */
        if( pTopStack == staPStat )
	    sprintf(caBuf+strlen(caBuf), "%d", oldPC->line_number);
//            itoa(pTopStack->m_OldPC->line_number, caBuf, 10);
        /* This is the case that macro is running successfully, the line number of OldPC should
         * be 0 since we already initialized the whole the structure to be zero at the PopPC(). */
        else
	    sprintf(caBuf+strlen(caBuf), "%d", oldPC->line_number);
//            itoa(oldPC->line_number, caBuf+strlen(caBuf), 10);
	}
    TKSetPCCriticalSection( -1 );
    /* print out the line numbers */
    if (SERPutsTxBuff(SERGetCmdPort(), caBuf) == FAILURE) return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE) return FAILURE;
    return SUCCESS;
}

/*************************************************
 * Function Name:   MASetFlagsToFalse
 * Description:     Resets all the interrupt flags to false when:
 *                  - Initialize the macro module.
 *                  - Start teach mode
 *                  - Start diagnostic
 *                  - Inside teach when you want to run a command such as pick, place or Calibration for scanner.
 *                  - When execting the instruction and it's fail, so the other interrupt can execute next time.
 *                    Instead of the specific interrupt keep triggering.
 *                  - In the MREndMacro, if CMER finished executing then all flags is reset for the execute interrupt to come.
 *
 * return:          SUCCESS/FAILURE
 * *************************************************/
int MASetFlagsToFalse()
{
    iRunningCMERMacro = FALSE;
    iRunningSSTOPMacro = FALSE;
    iRunningINTLKMacro = FALSE;
    iRunningERRMacro = FALSE;
    iPendingSSTOPMacro = FALSE;
    iPendingINTLKMacro = FALSE;
    iPendingERRMacro = FALSE;

    iIIMInterrupt = FALSE;
    iLMMInterrupt = FALSE;
    iDERInterrupt = FALSE;
    iCMERInterrupt = FALSE;
    iSECSInterrupt = FALSE;
    iSECSInterrupt2 = FALSE;
    iSECSInterrupt3 = FALSE;
    iSECSInterrupt4 = FALSE;
    return SUCCESS;
}
/*********************************************************
 * Function Name:   MRGetMacroAvoidRecursive
 * Description:     Get the macro name that during macro
 *                  defining. So we can check for macro call itself
 *                  and lmcr itself.
 * return:          macro name
 * **********************************************************/
char* MRGetMacroAvoidRecursive()
{
    return caMacroAvoidRecursive;
}

/*********************************************************
 * Function Name:   MADisplayMacroTrace
 * Description:     Display the macro code when the trace mode is on
 *                  and while the macro is executing.
 *                  First, display the line number then check if the line number
 *                  is 0, display the word "MACRO" and the rest of instruction.
 *                  For list the rest of instruction, call function MRListInstruction with
 *                  pass in the flag is TRUE which's meant the trace is on.
 * Return:          SUCCESS/FAILURE
 * **********************************************************/
int MADisplayMacroTrace(instr_ptr instrArg)
{
    char caBuf[50];
    static instr_ptr instrOLD = NULL;

    if (MADifferentInstr(instrOLD, instrArg))
    {
        instrOLD = instrArg;
        caBuf[0] = 0;
        /* print line number */
        sprintf(caBuf, "\r\n%3d:  ", instrArg->line_number);
        if(instrArg->line_number == 0)      /* print "MACRO" */
        {
            strcat(caBuf, "MACRO ");
        }
        if (caBuf[0] != 0)
        {
            if(SERPutsTxBuff(iTracePort, caBuf)==FAILURE)
                return FAILURE;
            if(SERFlushTxBuff(iTracePort)==FAILURE)
                return FAILURE;
        }
        /* list the instruction with tracing on */
        if(MRListInstructions(instrArg,TRUE)==FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}
/*********************************************************
 * Function Name:   MASetFreezeMacro
 * Description:     Sets iFreezeVariable, which indicates if the macro is in frozen state
 * Return:          SUCCESS/FAILURE
 * **********************************************************/
void MASetFreezeMacro(int iSetValueArg)
{
    iFreezeMacro = iSetValueArg;
    return;
}
/*********************************************************
 * Function Name:   MRGetFreezeMacro
 * Description:     Returns iFreezeMacro, which indicates if the macro is in frozen state
 * Return:          iFreezeMacro global variable
 * **********************************************************/
int MRGetFreezeMacro()
{
    return iFreezeMacro;
}
/**************************************************************
 * Function Name:   MAFindPath
 * Purpose:         Function is being used when we need to search the Status structure of the current macro,
 *                  and store the path to the last nested macro. It is put together and returned in the string buffer
 *
 * Parameter:       pstkStatusStackArg - pointer to beginning of the list of statuses
 *                  pstkStatusStackTopArg - pointer to the top of the statuses list
 *                  caBufferArg - pointer to store the final string
 *
 * Returns:         SUCCESS/FAILURE
 * **************************************************************/
void MAFindPath(pstProgStatus pstkStatusStackArg, pstProgStatus pstkStatusStackTopArg, char caBufferArg[])
{
    while (pstkStatusStackArg < pstkStatusStackTopArg)
    {
//        itoa(pstkStatusStackArg->m_OldPC->line_number, caBufferArg + strlen(caBufferArg), 10);
        sprintf(caBufferArg+strlen(caBufferArg), "%d", pstkStatusStackArg->m_OldPC->line_number);

        strcat(caBufferArg, ".");
        ++pstkStatusStackArg;
    }
}
/*********************************************************
 * Function Name:   MASetCMEFlag
 * Description:     Sets iCMEFlag
 * Return:          none
 * **********************************************************/
void MASetCMEFlag(int iSetValueArg)
{
    iCMEFlag = iSetValueArg;
}
/*********************************************************
 * Function Name:   MRGetCMEFlag
 * Description:     Sets iCMEFlag
 * Return:          none
 * **********************************************************/
int MRGetCMEFlag(void)
{
    return iCMEFlag;
}
/*********************************************************
 * Function Name:   MADifferentInstr
 * Description:     Returns TRUE if two instr's are different.
 * Return:          TRUE/FALSE
 * **********************************************************/
int MADifferentInstr(instr_ptr instrArg1, instr_ptr instrArg2)
{
    if (instrArg1 == NULL || instrArg2 == NULL)
        return TRUE;
    if (instrArg1->nofopr != instrArg2->nofopr ||
        instrArg1->opr_ptr != instrArg2->opr_ptr ||
        instrArg1->line_number != instrArg2->line_number ||
        instrArg1->OC != instrArg2->OC )
        return TRUE;
    return FALSE;
}
