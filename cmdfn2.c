/***************************************************************\
 *
<<<<<<< HEAD
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        LLexmisc.c
=======
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : cmdfn2.c
 * Description          : Functions which act as an interface for the LowLevel
 *              commands. These functions are local to the low-level command module.
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
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 *
 * Functions:
 *
 * Description: Functions which act as an interface for the LowLevel
 *              commands. These functions are local to the low-level command module.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <malloc.h>
#include <string.h>

#include "cmdfns.h"
#include "cmdfn2.h"
#include "cmdsp.h"
#include "scmac.h"
#include "scio.h"
#include "sctim.h"
#include "gag.h"
<<<<<<< HEAD
#include "scstat.h"
=======
#include "scstat.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include "ser.h"
//#include "serl.h"
#include "serm.h"
#include "fiog.h"
#include "sctag.h"
#include "scver.h"
#include "scregg.h"
#include "scmem.h"
#include "ro.h"
#include "rofio.h"
#include "gaintr.h"
#include "sctch.h"
#include "dg.h"
#include "scttr.h"
#include "mapio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
#include "scproc.h"
<<<<<<< HEAD
#include "secsg.h"
#include "secsl.h"

#define RX_BUF_SIZE 1024    /* Receive buffer size in bytes */
#define TX_BUF_SIZE 1024    /* Transmit buffer size in bytes */

struct CommStr
{
    int m_iFD;		    /* device fd from open device "/dev/ttyS0" */
//    int m_iTTY;		    /* console port id "/dev/tty" */
    struct termios m_OldTIO;/* terminal io structure for current setting */
    struct termios m_NewTIO;/* terminal io structure for new console setting */
//    struct termios m_OldKey;/* terminal io for current keyboard setting */
//    struct termios m_NewKey;/* terminal io for new keyboard setting */
//    struct sigaction m_saIO;/* signal action for each port communication */
=======
#include "secsg.h"
#include "secsl.h"

#define RX_BUF_SIZE 1024    /* Receive buffer size in bytes */
#define TX_BUF_SIZE 1024    /* Transmit buffer size in bytes */

struct CommStr
{
    int m_iFD;		    /* device fd from open device "/dev/ttyS0" */
//    int m_iTTY;		    /* console port id "/dev/tty" */
    struct termios m_OldTIO;/* terminal io structure for current setting */
    struct termios m_NewTIO;/* terminal io structure for new console setting */
//    struct termios m_OldKey;/* terminal io for current keyboard setting */
//    struct termios m_NewKey;/* terminal io for new keyboard setting */
//    struct sigaction m_saIO;/* signal action for each port communication */
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    int m_iStatus;          /* port status word */
    char *m_cpRxGet;        /* Input ring-buffer TAIL pointer */
    char *m_cpRxPut;        /* Input ring-buffer HEAD pointer */
    char *m_cpRxEnd;        /* Pointer to end of internal input buffer.
                             * Used to wrap-around the head and tail pointers. */
    char m_caRxBuf[RX_BUF_SIZE];    /* Internal input RING-buffer */
    char *m_cpTxGet;        /* Output ring-buffer TAIL pointer */
    char *m_cpTxPut;        /* Output ring-buffer HEAD pointer */
    char *m_cpTxEnd;        /* Pointer to end of internal input buffer.
                             * Used to wrap-around the head and tail pointers. */
    char m_caTxBuf[TX_BUF_SIZE];    /* Internal output RING-buffer */
    int m_iBaud;            /* Current baud rate */
    int m_iParity;          /* Current parity setting */
    int m_iDataBits;        /* Current data bits setting */
    int m_iStopBits;        /* Current stop bits setting */
    int m_iFlowCtrl;        /* TRUE to require CTS high before sending */
    int m_iEcho;            /* TRUE to echo chars coming in */
    int m_iSECS;            /* TRUE if port is to be used for SECS communication */
    int m_iPortOpen;        /* Flag indicating the port has been successfully opened.
                             *
                             * WARNING!!! CAUTION!!!
                             *
                             * This flag can indicate that the port is open by using
                             * SERSetPortOpen even though SERInitPort may not have been called. */
};
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int SERCheckForEOT(struct CommStr *pCommDataArg);
int SERCheckValidPort(int iPortNumArg);
int SERFetchRxCh(struct CommStr *pCommDataArg, char *cCharToFetch);
int SERStoreRxCh(struct CommStr *pCommDataArg, char cCharToStore);
int SERBackspRxCh(struct CommStr *pCommDataArg);
int SERFetchTxCh(struct CommStr *pCommDataArg, char *cCharToFetch);
int SERStoreTxCh(struct CommStr *pCommDataArg, char cCharToStore);
int SERSetInternalCommStatus(int iPortNumArg, int iStatus);
int SERSetCommErr(int iPortNumArg);
int SERGetSECSMsg(int iPortNumArg);
int SERSendSECSMsg(int iPortNumArg);
int SERSetSECSStatus(int iPortNumArg, int iCurrActivityStatArg, int iLastErrorStatArg, char cAckNakStatArg, int iGetStatArg);
<<<<<<< HEAD

#ifdef COMPORT6
#define MAXSECSPORTS 6
#else
#define MAXSECSPORTS 3
#endif

=======

#ifdef COMPORT6
#define MAXSECSPORTS 6
#else
#define MAXSECSPORTS 3
#endif

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
extern int giSECSComplete;
int giFirstSECS = TRUE;
int giExecutingSECS = FALSE;

/**************************************************************************************
 * Function Name:   ex_NOP
 * Description:     A no-operation execution routine.  This routine is only used for development
 *                  and debugging.
 * Note:            not implement for this version (I2BRT)
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 **************************************************************************************/
int ex_NOP(instr_ptr instr)
{
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_DM
 * Description:     Dump Memory.  This funciton is used for debugging only
 * NOTE:            This command is not used now.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DM(instr_ptr instr)
{
    unsigned char *ucfPtr;
    int iCount;
    long lCount, lCount2, lSegment, lOffset, lRange, lR0;
    char sBuf[20], sBuf2[20];
    CMDoperand_ptr Optr;
	int iCmdPort;

    if( RGGetRegister(0,&lR0) == FAILURE )  /* if unable to get the information */
        return FAILURE;
    ucfPtr = (unsigned char *)lR0;    /* get the pointer segment info */
//    iCount = _FP_SEG( ucfPtr );
	iCmdPort = SERGetCmdPort();
    sprintf( sBuf, "%X\r\n", iCount );      /* output the amount of memory to be dumped */
	if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
		return FAILURE;

    /* get first operand, the segment */
	Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lSegment)==FAILURE)
		return FAILURE;

    /* get second operand, the offset */
	Optr = Optr->next;
	if(CMDgetValue(Optr,&lOffset)==FAILURE)
		return FAILURE;

     /* get third operand, the range */
	Optr = Optr->next;
	if(CMDgetValue(Optr, &lRange)==FAILURE)
		return FAILURE;

    ucfPtr = (unsigned char *)( (lSegment<<16) | lOffset );  /* manipulate the info */
    for( lCount=0; lCount<lRange; lCount+=16 )
    {
        memset( sBuf, '.', 16 );        /* initialize the first 16 locations to '.' */
        strcpy( &sBuf[16], "\r\n\0" );  /* add appropriate end of line characters */
        sprintf( sBuf2, "%sfp ", (unsigned char *)(ucfPtr+lCount) );
		if(CMDShowDescription(iCmdPort, sBuf2)==FAILURE)
			return FAILURE;
        for( iCount=0,lCount2=lCount; lCount2<lCount+16; lCount2++ )
        {                               /* sBuf=1st bye sBuf2=2nd byte ??? */
            if( isprint( ucfPtr[lCount2] ) )  /* if printable, populate sBuf w/the */
                sBuf[lCount2-lCount] = ucfPtr[lCount2];  /* contents of memory   */
            if( iCount == 8 )           /* if on byte boundry output spacer */
            {
				if(CMDShowDescription(iCmdPort, " - ")==FAILURE)
					return FAILURE;
            }
            sprintf( sBuf2, "%02hX ", ucfPtr[lCount2] );
			if(CMDShowDescription(iCmdPort, sBuf2)==FAILURE)
				return FAILURE;
            iCount++;
        }
        if (SERPutsTxBuff( iCmdPort, "  " ) == FAILURE) return FAILURE;
		if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
			return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_WM
 * Description:     Write memory.  this function is used for debugging only.
 * NOTE:            This command is not used now.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lSegment, lOffset, lTmp;
    unsigned char ucPtr;
    unsigned char *ucfPtr;

    Optr=instr->opr_ptr;       /* get first operand, if no operand FAILURE */
	if(CMDgetValue(Optr, &lSegment)==FAILURE)
		return FAILURE;

    Optr=Optr->next;           /* get second operand, if no operand FAILURE      */
	if(CMDgetValue(Optr, &lOffset)==FAILURE)
		return FAILURE;

    Optr=Optr->next;           /* get third operand, if no operand FAILURE */
	if(CMDgetValue(Optr, &lTmp)==FAILURE)
		return FAILURE;
	ucPtr = (unsigned char)lTmp;

    ucfPtr = (unsigned char *)((lSegment<<16)|lOffset);
    *ucfPtr = ucPtr;                    /* contents of memory */
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_DIAG
 * Description:     Begin Teach Mode in specified comm port.
 * Implementation:  First set the default Comm port for diagnostic is at COM3.
 *                  Check if there is an operand in an instruction, then get the comm
 *                  port id and validate to make sure it is either COM1 or COM3.  Then
 *                  Start execute the diagnostic by calling DiagSetFlag() from Diagnostic module.
 * NOTE:            This function will get executed when the user typed DIAG command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DIAG [comm port]
 *                      - comm port:    COM1 or COM3. Also, the value should be LONG_CONST ONLY.
 *                      - default with operand parameter is COM3.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DIAG(instr_ptr instr)
{
    int iCom=COM2;                      /* default value */

    if( instr->opr_ptr )                /* if there is an operand */
        iCom = (int)(instr->opr_ptr->opr.l);  /* get comm port id */
    if (iCom != COM1 && iCom != COM2)   /* validate the com port, only take COM1 or COM3. */
        return FAILURE;
    return DiagSetFlag(iCom);         /* start the diagnostic. */
}
/***************************************************************************
 * Function Name:   ex_ENDTR
 * Description:     End Teach Mode.  this command is only valid when you are on the terminal mode with teach pendant.
 *                  It is used to exit from the terminal mode on the teach pendant.
 * Implementation:  First, check if the user execute this command is at teach pendant port or other port.
 *                  If the not teach pendant port (COM3), then return failure. Otherwise, set comm port to be
 *                  COM1(or EQT port) and clear the COM1 buffer by calling SERFlushComBuffer() from comm module,
 *                  and show the Eqipe logo by calling TTShowLogo() from teach pendant module.
 * NOTE:            This function will get executed when the user typed ENDTR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ENDTR
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ENDTR(instr_ptr instr)
{
    if( SERGetCmdPort() != COM1 )        /* it must be teach pendant port which is COM3 */
    {
        /* before ending the terminal mode from teach pendant, set command port to be COM1 which is EQT.*/
        if( SERSetCommVals(COM1, CMD_PORT, TRUE) == SUCCESS )
        {
            SERFlushComBuffer(COM1);     /* flush the COM1 buffer. */
            return TTShowLogo();        /* show the Equipe logo. */
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_GALIL
 * Description:     Go to GALIL mode, and bypassing command interpreter.
 *                  This command allows the user to work directly on the level of the GALIL
 *                  card. When returning to interpreter mode, make sure that all parameters are proper.
 *                  The interpreter doesn't recognize any change made in this mode.
 * Implementation:  First, check if there is any operand.  If there is no operand, then default is 0 which is
 *                  first galil card (robot card). If there is operand, then get the operand and validate the value
 *                  which must be 0 or 1. Then call MAStartGalilMode() to start the galil mode.
 * NOTE:            This function will get executed when the user typed GALIL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: GALIL [galil card number]
 *                      - galil card number:    the card number is either 0 or 1.
 *                                              - 0 and default:    robot galil card and it is first card in the controller.
 *                                              - 1:    prealigner galil card and it is the second card in the controller.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_GALIL(instr_ptr instr)
{
    int iCardNum=0;                     /* default value, first galil card */
//    if( instr->opr_ptr )
//    {
//		if(CMDgetIntegerValue(instr->opr_ptr, &iCardNum)==FAILURE)
//			return FAILURE;
//        if(iCardNum != 0 && iCardNum != 1)  /* validate the card 0 or 1 */
//            return FAILURE;
//    }
    return ROStartGalilMode( iCardNum );  /* start galil mode. */
}
/***************************************************************************
 * Function Name:   ex_EXIT
 * Description:     Debug instruction to exit the program and return to DOS.
 *                  This command is for internal use only. Not for customer.
 * Implementation:  First, get the value from an operand and check for value which must
 *                  be 628.  628 is the password to exit to DOS.  Then close all initialized
 *                  devices and restores all the vector as neccesary and exit the program.
 * NOTE:            This function will get executed when the user typed EXIT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EXIT 628
 *                      - 628:   is the password to exit to DOS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EXIT(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    if( instr->nofopr < 1 )             /* if no operands than it's a problem */
        return FAILURE;
    Optr = instr->opr_ptr;              /* get first operand */
    if( Optr->opr.l != 628 )            /* the value must be 628 */
        return FAILURE;
    /* Close all initialized devices as appropriate. */

//    TIDisableTimer();

//    exit(0);                            /* exit program */
    /* Beyond the point of no return..... */
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_NPAR
 * Description:     Get Next Parameter or get the number of parameters that passed in from the macro agruments.
 *                  The macro parameters are saved in the Special Registers (special register for internally use only).
 *                  This command is only for NOVELLUS macro.
 * Implementation:  There must have at least 2 operands, but it also depend on the value of the first operands to
 *                  require for third operand or not.  So we must check for first 2 operands to be not NULL.  Get
 *                  value of the first operand.
 *                  If the value of first operand is less than 0:
 *                      - The second operand type must be the LONG_CONST.
 *                      - Set the prompt mode by calling PCSetPromptMode() and PCSetPromptMode will
 *                        determine the value of the mode to print the prompt or not. If mode is 1, print
 *                        the "\r\n>". If mode is 0, don't print "\r\n>" when hit CR or RESP command execute.
 *                  If the value of first operand is equal or greater than 0:
 *                      - The second operand type must be INDIRECT_REG type.
 *                      - There must be a third operand and it must be INDIRECT_REG type.
 *                  If the value of the first operand is greater than value that store internally in special register 0,
 *                  then set -1 into the second and third operand and return them. The special register 0 is holding
 *                  number of operands(parameters) that macro has and it got storing at the macro execution time
 *                  in MRExecuteMacro(). That's meant the user try to get parameter that does not exist.
 *                  If the first operand is less than the value internally in special register 0,
 *                  then get the value of specified parameter and store into the second operand register and set the third
 *                  operand register to 0.
 * NOTE:            This function will get executed when the user typed NPAR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: NPAR <ACTION>, <FLAG or RETURN VALUE>,[RESULT OF EXECUTION]
 *                      - ACTION:   the purpose of calling the command at that time.
 *                                  if -1, relate to prompt mode.
 *                                  if 0, get number of parameters that macro has when it is executing.
 *                                  if >=1, get the specific parameter.
 *                                  This parameter can be LONG_CONST or INDIRECT_REG type.
 *                      - FLAG or REUTRN VALUE:     If the first parameter is -1, then this flag will be
 *                                                  telling process mode:
 *                                                  - If 1, in Equipe mode and allowing to download and
 *                                                    when you press enter, you will get "CRLF>".
 *                                                  - if 0, in Brooks mode. You will not "CRLF>" when
 *                                                    press enter or sending RESP command.
 *                                                  - NOTE: the first parameter is -1, then this parameter
 *                                                          must be LONG_CONST type.
 *                                                  If the first parameter is 0, then it stores the number
 *                                                  of parameters.
 *                                                  If the first parameter >=1, then it stores the value of
 *                                                  specific parameter.
 *                                                  This parameter can be LONG_CONST or INDIRECT_REG type depend
 *                                                  on the first parameter value.
 *                     - RESULT OF EXECUTION:       This will store the success or failure of the command when
 *                                                  trying to do some thing.  Example, if trying to get the value
 *                                                  of the third parameter but there is not more parameter then
 *                                                  this variable will store -1. It has to be INDIRECT_REG type.
 *                      Example:    NPAR -1, 1:     Puts in Equipe mode, gives you a "CRLF>" when you press enter.
 *                                                  Allows downloading.
 *                                  NPAR -1,0:      Puts i Brooks mode. No "CRLF>" when you press enter or when
 *                                                  it gets a RESP command.
 *                                  NPAR 0,[R1],[R2]:   Returns the number of parameters on the command line in R1.
 *                                                      If none then [R1] returns 0 and [R2] returns -1 else >1.
 *                                  NPAR 1,[R1],[R2]:   Returns the first parameter in [R1] if none then [R2] = -1
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_NPAR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal, lTmp;
    int iRegister;

    Optr = instr->opr_ptr;              /* get first operand */
	if(CMDgetValue(Optr, &lVal)==FAILURE)
		return FAILURE;

    Optr = Optr->next;                  /* get second operand */
    if(!Optr)
        return FAILURE;
    iRegister = Optr->opr.i;

    /* Third operand is required for non negative 1st operand */
    if ( lVal >= 0 )
    {
        /* Handle NPAR 0, [R1], [R2]
         * Handle NPAR 1, [R1], [R2]
         * Handle NPAR 2, [R1], [R2]
         * Handle NPAR n, [R1], [R2] */
        Optr = Optr->next;                      /* Optional Third Operand */
        if(!Optr)
            return FAILURE;
    }
    else
    {
        /* Handle NPAR -1, 0 or NPAR -1, 1
         * It must be long constant */
        if (Optr->type != LONG_CONST)
            return FAILURE;
    }
    /* Get # of Macro Parameters from internal special register 0.
     * special register 0 stored the number of parameters when
     * macro is execute at the command line. number of parameters are
     * stored in the MRExecuteMacro(). */
    if(RGGetSpecialRegister(0,&lTmp) == FAILURE)
    {
        return FAILURE;
    }

    if(lVal < 0)            /* NPAR 1st Operand */
    {
        if (iRegister!=TRUE && iRegister!=FALSE)
            return FAILURE;
        PCSetPromptMode(iRegister);   /* To set the Prompt Type */
    }
    else if(lVal > lTmp)
    {
        if(RGSetRegister(iRegister,-1) == FAILURE)
            return FAILURE;
        if(RGSetRegister(Optr->opr.i, -1)==FAILURE)
            return FAILURE;
    }
    else
    {
        if(RGGetSpecialRegister((int)lVal, &lTmp)==FAILURE) /* Get the Nth parameters passed by Macro */
            return FAILURE;
        if(RGSetRegister(iRegister, lTmp)==FAILURE)
            return FAILURE;
        if(RGSetRegister(Optr->opr.i, 0)==FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_EIS
 * Description:     enter interrupt section. This will block other interrupt from get
 *                  into the interrupt section. It's mean there may be some interrupt is in the
 *                  processing or executing.
 * Implementation:  Set the enter interruption section flag to true to block other from come into
 *                  the interrupt section to process the interrupt while another interrupt process the
 *                  interrupt routine.
 * NOTE:            This function will get executed when the user typed EIS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EIS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 *************************************************************************/
int ex_EIS(instr_ptr instr)
{
    MASetMacroIntegerVars(MR_INTR_SECT_FLAG, TRUE);  /* block from getting into the interrupt section */
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_LIS
 * Description:     Leave interrupt section. It will allow other interrrupts to come in
 *                  the interrupt section to process their interrupt if there is an interrupt.
 * Implementation:  To allow other interrupt to come in the interrupt section by setting the
 *                  interrupt section flag to false.
 * NOTE:            This function will get executed when the user typed EIS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EIS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 * **************************************************************************/
int ex_LIS(instr_ptr instr)
{
    MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);  /* reset the flag for allowing other interrupts to come int the interrupt section. */
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_TEACH
 * Description:     Begin Teach Mode to teach the robot or track at certain coordinate.
 * Implementation:  The default COMM port from teach mode to start is at COM3 if there is no operand.
 *                  If there is an operand, get the comm port id and validate the comm port id.  Then
 *                  start running the teach mode.
 * NOTE:            This function will get executed when the user typed TEACH command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TEACH [PORT]
 *                      - PORT:     which comm port is teach mode will be running on.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TEACH(instr_ptr instr)
{
    int iCom=COM2;                      /* default value */

    if( instr->opr_ptr )                /* if there is an operand */
        iCom = (int)(instr->opr_ptr->opr.l);
    if (iCom != COM1 && iCom != COM2)   /* validate the comm port id */
        return FAILURE;
    return TMSetTeachFlag(iCom);        /* start to run the teach mode. */
}
/***************************************************************************
 * Function Name:   ex_RES
 * Description:     Reset Controller. This command does software reset which re-initialize controller
 *                  to power-up condition. when the controller is ready, it will send the ready prompt.
 * NOTE:            This function will get executed when the user typed RES command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RES
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RES(instr_ptr instr)
{
    long lMode;
<<<<<<< HEAD
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr,&lMode)==FAILURE)    /* get tge tracing mode */
        return FAILURE;
=======
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr,&lMode)==FAILURE)    /* get tge tracing mode */
        return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if(lMode == 313)
        TIResetComputer();

    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_MTRC
 * Description:     Macro tracing for debugging only the macro while it is executing.
 *                  You can turn trace on or off and how you want to print out the trace;
 *                  depends on the mode:
 *                      - 0:    turn off the trace mode.
 *                      - 1:    turn on the trace mode and display all the executing lines in the
 *                              macro include the macro call.
 *                      - 2:    turn on the trace mode display only those macro call inside the executing
 *                              macro.
 *                  All the tracing message will send to comm port.  Default port is the current using port.
 *                  Otherwise, using the specified port.
 *                  Only in codemerge
 * Implementation:  First, if there is no operand then set to default mode and port which is turn on trace mode and
 *                  use current port. If there is an operand, get the mode and get the port from the second operand if there
 *                  is one. Otherwise, set the default port. Then set the mode and port to get ready for debugging.
 *                  When the macro is executing, the tracing is happened or not depending on the mode. The mode will get
 *                  checked from the excecution instruction in the macro module.
 * NOTE:            This function will get executed when the user typed MTRC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MTRC [MODE], [PORT]
 *                      - MODE:     Tracing mode such as turn off/on tracing
 *                      - PORT:     Comm port to send the trace to.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MTRC(instr_ptr instr)
{
    long lMode = 1;
    CMDoperand_ptr Optr;
    long lCmdPort;

    lCmdPort = (long)SERGetCmdPort();
    /* if no operand, then the default is trace mode is on with the same option
     * as 1 which is printing out all the executing macro instruction and the
     * port for sending out the tracing macro instruction is on current cmd_port. */
    if(instr->nofopr == 0)
        goto exit_point;
    /* get first operand which is the tracing mode. */
    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr,&lMode)==FAILURE)    /* get tge tracing mode */
        return FAILURE;

    /* second operand can be ignore if you want the default port. Otherwise, you have to
     * specify the comm port that you want to send the tracing instruction to. */
    Optr = Optr->next;
    if (Optr)
    {
        if(CMDgetValue(Optr,&lCmdPort)==FAILURE)  /* get the port id */
            return FAILURE;
    }

exit_point:
    MASetMacroIntegerVars(MR_TRACE_MODE, (int)lMode);
    MASetMacroIntegerVars(MR_TRACE_PORT, (int)lCmdPort);
    return SUCCESS;
}
//#ifdef SX
/***************************************************************************
 * Function Name:   ex_SECS
 * Description:     send SECS communications message out to a selected comm port
 * Implementation:
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SECS(instr_ptr instr)
{
    int iRetVal, iPortNum, iStream, iFunction, iParameter, iSECSFlag;
    unsigned uDevID;
    int iaGenValues[5] = {0, 0, 0, 0, 0};   /* it can be either direct value or index of register for output */
    char pcGenString[256];
    int iSetPCInstr = 0;                    /* returned  1 means set PC = instr, if 0  don't set it */
    int iChangePCWaiting = 0;
    int iCounter;
    long lOprValue;
<<<<<<< HEAD
    CMDoperand_ptr Optr;

=======
    CMDoperand_ptr Optr;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    iChangePCWaiting = MRGetMacroIntegerVars(MR_PC_WAITING);

    Optr = instr->opr_ptr;
    if(CMDgetIntegerValue(Optr, &iPortNum)==FAILURE)
	return FAILURE;

    if(SERGetCommVals(iPortNum, SECS_MODE, &iSECSFlag) == FAILURE)
        return FAILURE;
    if(iSECSFlag != TRUE)
    {
        // For SECS as device echo back to cmd_port if the CTS is On (ASCII mode)
        if(iPortNum == COM1)
        {
            sprintf(pcGenString, "SECS %d, ", iPortNum);
            if(SERPutsTxBuff(iPortNum, pcGenString) == FAILURE)
                return FAILURE;
            Optr = Optr->next;
            while(Optr)
            {
//                if(TIRefreshWD() == FAILURE)
//                    return FAILURE;
                if(CMDgetValue(Optr, &lOprValue) == FAILURE)
                    return FAILURE;
//                ltoa(lOprValue, pcGenString, 10);
		sprintf(pcGenString, "%d", (int)lOprValue);
                if(SERPutsTxBuff(iPortNum, pcGenString) == FAILURE)
                    return FAILURE;
                Optr = Optr->next;
                if(Optr)
                {
                    if(SERPutsTxBuff(iPortNum, ", ") == FAILURE)
                        return FAILURE;
                }
                else
                {
                    if(SERPutsTxBuff(iPortNum, "\r\n") == FAILURE)
                        return FAILURE;
                }
            }
            return SUCCESS;
        }
        else
            return FAILURE;
    }

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &uDevID)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &iStream)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &iFunction)==FAILURE)
	return FAILURE;

<<<<<<< HEAD
//printf("SECS COM%d Dev=%d S%dF%d act=%d executing=%d\n",iPortNum, uDevID, iStream, iFunction, SERGetSecsActivity(iPortNum), giExecutingSECS);

    if(SERGetSecsActivity(iPortNum) != 0 && !giExecutingSECS) 
    {
        MASetPC(instr);
	return SUCCESS;
    }

    if(giFirstSECS)
    {
	giSECSComplete = 0;
	giFirstSECS = FALSE;
=======
//printf("SECS COM%d Dev=%d S%dF%d act=%d executing=%d\n",iPortNum, uDevID, iStream, iFunction, SERGetSecsActivity(iPortNum), giExecutingSECS);

    if(SERGetSecsActivity(iPortNum) != 0 && !giExecutingSECS) 
    {
        MASetPC(instr);
	return SUCCESS;
    }

    if(giFirstSECS)
    {
	giSECSComplete = 0;
	giFirstSECS = FALSE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    }

    /* For SECS as device read next parameter if it's S0, F0, 200 function only */
    if((iPortNum != COM1) || ((iPortNum == COM1) && (iFunction == 0) && (iStream == 0)))
    {
        Optr = Optr->next;
        if(CMDgetValue(Optr, &lOprValue) != SUCCESS)
            return FAILURE;
        if(iPortNum == COM1)
            iaGenValues[0] = (int)lOprValue;
        else
            iParameter = (int)lOprValue;
        Optr = Optr->next;
<<<<<<< HEAD
    }

//printf("SECS COM%d Dev=%d S%dF%d parm=%d\n",iPortNum, uDevID, iStream, iFunction, iParameter);
=======
    }

//printf("SECS COM%d Dev=%d S%dF%d parm=%d\n",iPortNum, uDevID, iStream, iFunction, iParameter);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    if((iParameter == 200) || (iParameter == 201) || (iParameter == 202))
    {
        switch(iParameter)
        {
            case 200 : /* non-SECS command */
                break;
            case 201 : /* generic send, with arbitrary Stream and Function */
                if(iPortNum == COM1)
                    return FAILURE;
		if(CMDgetIntegerValue(Optr, &iaGenValues[0])==FAILURE)
		    return FAILURE;
                Optr = Optr->next;
                if(CMDgetString(Optr, pcGenString,0) == FAILURE)
                    return FAILURE;
                break;
            case 202 :  /* generic read, reads last message replay */
                if(iPortNum == COM1)
                    return FAILURE;
		if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
		    return FAILURE;
                Optr = Optr->next;
		if(CMDgetIndirectValue(Optr, &iaGenValues[1])==FAILURE)
		    return FAILURE;
                Optr = Optr->next;
		if(CMDgetIndirectValue(Optr, &iaGenValues[2])==FAILURE)
		    return FAILURE;
                Optr = Optr->next;
		if(CMDgetIndirectStrValue(Optr, &iaGenValues[3])==FAILURE)
		    return FAILURE;
                break;
            default:
                return FAILURE;
        }
    }
    else
    {
        switch(iStream)
        {
            case 1 :
                switch(iFunction)
                {
                    case 1 :
                        break;
                    case 2 :	// S1F2 requires 2 Parameters....
                        if(iPortNum != COM1)
<<<<<<< HEAD
                        {
=======
                        {
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("ex_SECS S1F2\n");
			    if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
				return FAILURE;
                        }
                        break;
                    case 6 :
                        if(iPortNum == COM1)
                            return FAILURE;
                        if(iParameter == 2)
                        {
			    if(CMDgetIntegerValue(Optr, &iaGenValues[0])==FAILURE)
				return FAILURE;
                            Optr = Optr->next;
			    if(CMDgetIndirectValue(Optr, &iaGenValues[1])==FAILURE)
				return FAILURE;
                            break;
                        }
                        else if(iParameter == 0)
                        {
                            if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
<<<<<<< HEAD
				return FAILURE;
			    Optr = Optr->next;
			    if (!Optr)
				return FAILURE;
			    CMDgetIndirectValue(Optr, &iaGenValues[1]);
			    Optr = Optr->next;
			    if (!Optr)
				return FAILURE;
=======
				return FAILURE;
			    Optr = Optr->next;
			    if (!Optr)
				return FAILURE;
			    CMDgetIndirectValue(Optr, &iaGenValues[1]);
			    Optr = Optr->next;
			    if (!Optr)
				return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    CMDgetIndirectValue(Optr, &iaGenValues[2]);
                            break;
                        }
                        else
                            return FAILURE;
<<<<<<< HEAD
                    case 5 :
=======
                    case 5 :
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("ex_SECS S1F5 check\n");
                        if(iPortNum == COM1)
                            return FAILURE;
                        break;
                    default:
                        return FAILURE;
                }
                break;
            case 2 :
                switch(iFunction)
<<<<<<< HEAD
                {
		    case 13 :
			if(iPortNum == COM1)
			    return FAILURE;
=======
                {
		    case 13 :
			if(iPortNum == COM1)
			    return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			break;
                    case 14 :
                        if(iPortNum != COM1)
                            return FAILURE;
                        break;
                    case 15 :
                        if(iPortNum == COM1)
                            return FAILURE;
                        break;
                    case 16 :
<<<<<<< HEAD
                        if(iPortNum == COM1)
			    return FAILURE;
                        break;
		    case 37:
			if(iPortNum == COM1)
			    return FAILURE;
			break;
		    case 38:
			if(iPortNum == COM1)
			    return FAILURE;
			break;
=======
                        if(iPortNum == COM1)
			    return FAILURE;
                        break;
		    case 37:
			if(iPortNum == COM1)
			    return FAILURE;
			break;
		    case 38:
			if(iPortNum == COM1)
			    return FAILURE;
			break;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

                    case 42 :
                        if(iPortNum == COM1)
                            return FAILURE;
			if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
			    return FAILURE;
                        break;
                    case 41 :
                        if(iPortNum == COM1)
                            return FAILURE;
                        if((iParameter == 6) || (iParameter == 12) || iParameter == 149)
                        {
			    if(CMDgetIntegerValue(Optr, &iaGenValues[0])==FAILURE)
				return FAILURE;
                        }
                        break;
                    case 66 :
                        if(iPortNum != COM1)
                            return FAILURE;
                        Optr = Optr->next;
			if(CMDgetIntegerValue(Optr, &iaGenValues[0])==FAILURE)
			    return FAILURE;
                        break;
                    case 67 :
                        if(iPortNum != COM1)
                            return FAILURE;
                        for(iCounter = 0; iCounter < 3; iCounter++)
                        {
                            Optr = Optr->next;
			    if(CMDgetIntegerValue(Optr, &iaGenValues[iCounter])==FAILURE)
				return FAILURE;
                        }
                        break;
                    default:
                        return FAILURE;
                }
                break;
            case 4 :
                if(iPortNum == COM1)
                    return FAILURE;
                switch(iFunction)
                {
                    case 111 :
                    case 113 :
			if(CMDgetIntegerValue(Optr, &iaGenValues[0])==FAILURE)
			    return FAILURE;
                        Optr = Optr->next;
			if(CMDgetIntegerValue(Optr, &iaGenValues[1])==FAILURE)
			    return FAILURE;
                        if(iFunction == 111)
                            break;
                        Optr = Optr->next;
                        if(CMDgetString(Optr, pcGenString,0) == FAILURE)
                            return FAILURE;
                        break;
                    case 112 :
			if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
			    return FAILURE;
                        Optr = Optr->next;
			if(CMDgetIndirectValue(Optr, &iaGenValues[1])==FAILURE)
			    return FAILURE;
                        break;
                    case 114 :
			if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
			    return FAILURE;
                        break;
                    default:
                        return FAILURE;
                }
                break;
            case 5 :
                switch(iFunction)
                {
                    case 1 :        // FIXME, to be consistent this function should be named S5F2, it is response
                        if(iPortNum == COM1)
                        {
                            for(iCounter = 0; iCounter < 2; iCounter++)
                            {
                                Optr = Optr->next;
				if(CMDgetIntegerValue(Optr, &iaGenValues[iCounter])==FAILURE)
				    return FAILURE;
                            }
                        }
                        else
<<<<<<< HEAD
                        {
 			    if(Optr)
=======
                        {
 			    if(Optr)
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    {
                            	if(Optr->type != INDIRECT_REG)
                                	return FAILURE;
                            	else
                                	iaGenValues[0] = Optr->opr.i;
<<<<<<< HEAD
                            }
			    else
				return FAILURE;
=======
                            }
			    else
				return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			}
                        break;
                    default:
                        return FAILURE;
                }
                break;
            case 6 :
                switch(iFunction)
<<<<<<< HEAD
                {
		    case 4 :	// reply to Indexer (S6F3 ack)
                        if(iPortNum == COM1)
                            return FAILURE;
		   	break;	
		    case 12 :	// reply to Indexer (S6F11 ack)
=======
                {
		    case 4 :	// reply to Indexer (S6F3 ack)
                        if(iPortNum == COM1)
                            return FAILURE;
		   	break;	
		    case 12 :	// reply to Indexer (S6F11 ack)
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                        if(iPortNum == COM1)
                            return FAILURE;
		   	break;	
                    case 13 :
                        if(iPortNum == COM1)
                            return FAILURE;
			if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
			    return FAILURE;
                        break;
                    case 65 :
                        if(iPortNum != COM1)
                            return FAILURE;
                        break;
                    case 69 :
                        if(iPortNum != COM1)
                            return FAILURE;
                        for(iCounter = 0; iCounter < 5; iCounter++)
                        {
                            Optr = Optr->next;
			    if(CMDgetIntegerValue(Optr, &iaGenValues[iCounter])==FAILURE)
				return FAILURE;
                        }
                        break;
                    case 71 :
                        if(iPortNum != COM1)
                            return FAILURE;
                        for(iCounter = 0; iCounter < 3; iCounter++)
                        {
                            Optr = Optr->next;
			    if(CMDgetIntegerValue(Optr, &iaGenValues[iCounter])==FAILURE)
				return FAILURE;
                        }
                        break;
                    default:
                        return FAILURE;
                }
                break;
            case 102 :
                if(iPortNum != COM1)
                    return FAILURE;
                Optr = Optr->next;
		if(CMDgetIndirectValue(Optr, &iaGenValues[0])==FAILURE)
		    return FAILURE;
                break;
            case 106 :
                if(iPortNum != COM1)
                    return FAILURE;
                for(iCounter = 0; iCounter < 3; iCounter++)
                {
                    Optr = Optr->next;
                    if((iCounter != 0) && (!Optr))
                        break;
		    if(CMDgetIndirectValue(Optr, &iaGenValues[iCounter])==FAILURE)
			return FAILURE;
                }
                break;
            case 200 :
                break;
            default:
                return FAILURE;
        }
<<<<<<< HEAD
    }

iSetPCInstr = 0;
//printf("ex_SECS S%dF%d iParm=%d\n",iStream,iFunction,iParameter);
=======
    }

iSetPCInstr = 0;
//printf("ex_SECS S%dF%d iParm=%d\n",iStream,iFunction,iParameter);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    if(iPortNum == COM1)
        iRetVal = S2SecsDeviceCommand(iPortNum, uDevID, iStream, iFunction, iaGenValues, &iSetPCInstr);
    else
        iRetVal = S2SecsCommand(iPortNum, uDevID, iStream, iFunction, iParameter,
                            iaGenValues, pcGenString, &iSetPCInstr, &iChangePCWaiting);

    if(iSetPCInstr != TRUE && iSetPCInstr != FALSE)
<<<<<<< HEAD
        return FAILURE;

    if(iSetPCInstr == TRUE && giSECSComplete == 0)
    {
	giExecutingSECS = TRUE;
        MASetPC(instr);
=======
        return FAILURE;

    if(iSetPCInstr == TRUE && giSECSComplete == 0)
    {
	giExecutingSECS = TRUE;
        MASetPC(instr);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    }
    else if(S2GetSecsSpecial())
    {
        CMDfreeOperands(instr->opr_ptr);
        MASetPC(NULL);
<<<<<<< HEAD
        S2SetSecsSpecial(FALSE);
    }

if(giSECSComplete)
{
	//printf("ex_SECS complete! setPC=%d PCWait=%d\n",iSetPCInstr, iChangePCWaiting);
	giFirstSECS = TRUE;
	iChangePCWaiting = FALSE;
	giExecutingSECS = FALSE;
}
    if(iChangePCWaiting != TRUE && iChangePCWaiting != FALSE)
        return FAILURE;
=======
        S2SetSecsSpecial(FALSE);
    }

if(giSECSComplete)
{
	//printf("ex_SECS complete! setPC=%d PCWait=%d\n",iSetPCInstr, iChangePCWaiting);
	giFirstSECS = TRUE;
	iChangePCWaiting = FALSE;
	giExecutingSECS = FALSE;
}
    if(iChangePCWaiting != TRUE && iChangePCWaiting != FALSE)
        return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    MASetMacroIntegerVars(MR_PC_WAITING, iChangePCWaiting);

    return iRetVal;
<<<<<<< HEAD
}

// special command to reset SECS processing
// the current activity is forced to set to IDLE
=======
}

// special command to reset SECS processing
// the current activity is forced to set to IDLE
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int ex_SCSR(instr_ptr instr)
{
    int iRetVal, iPortNum, iStream, iFunction, iParameter, iSECSFlag;
    unsigned uDevID;
    int iaGenValues[5] = {0, 0, 0, 0, 0};   /* it can be either direct value or index of register for output */
    char pcGenString[256];
    int iSetPCInstr = 0;                    /* returned  1 means set PC = instr, if 0  don't set it */
    int iChangePCWaiting = 0;
    int iCounter;
    long lOprValue;
<<<<<<< HEAD
    CMDoperand_ptr Optr;

=======
    CMDoperand_ptr Optr;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    Optr = instr->opr_ptr;
    if(CMDgetIntegerValue(Optr, &iPortNum)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
	if(CMDgetIntegerValue(Optr, &uDevID)==FAILURE)
		return FAILURE;

    Optr = Optr->next;
	if(CMDgetIntegerValue(Optr, &iStream)==FAILURE)
		return FAILURE;

    Optr = Optr->next;
	if(CMDgetIntegerValue(Optr, &iFunction)==FAILURE)
		return FAILURE;

<<<<<<< HEAD
    return S2AbortSecs();

//    return SUCCESS;
}
// special command to reset SECS port RESet
// the current activity is forced to set to IDLE
=======
    return S2AbortSecs();

//    return SUCCESS;
}
// special command to reset SECS port RESet
// the current activity is forced to set to IDLE
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int ex_SRES(instr_ptr instr)
{
    int iRetVal, iPortNum, iStream, iFunction, iParameter, iSECSFlag;
    unsigned uDevID;
    int iaGenValues[5] = {0, 0, 0, 0, 0};   /* it can be either direct value or index of register for output */
    char pcGenString[256];
    int iSetPCInstr = 0;                    /* returned  1 means set PC = instr, if 0  don't set it */
    int iChangePCWaiting = 0;
    int iCounter;
    long lOprValue;
<<<<<<< HEAD
    CMDoperand_ptr Optr;

=======
    CMDoperand_ptr Optr;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    Optr = instr->opr_ptr;
    if(CMDgetIntegerValue(Optr, &iPortNum)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &uDevID)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &iStream)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &iFunction)==FAILURE)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &iParameter)==FAILURE)
	return FAILURE;

<<<<<<< HEAD
    return S2AbortSecs();

//    if((iPortNum >= 3 && iPortNum <= MAXSECSPORTS) && iParameter==313)
//    {
//	SERClosePorts(iPortNum);
//    	if (SERInitPorts(iPortNum, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
//    	{
//            SSSetModulesInitializationsStatus(COM3_PORT, TRUE);
//	    return FAILURE;
//    	}
=======
    return S2AbortSecs();

//    if((iPortNum >= 3 && iPortNum <= MAXSECSPORTS) && iParameter==313)
//    {
//	SERClosePorts(iPortNum);
//    	if (SERInitPorts(iPortNum, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
//    	{
//            SSSetModulesInitializationsStatus(COM3_PORT, TRUE);
//	    return FAILURE;
//    	}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//    }
//    return SUCCESS;
}
//#endif
/***************************************************************************
 * Function Name:   ex_DOS
 *
 * Description:     Passes the DOS level command to the operating system. It also can clear or read the
 *                  WriteToNVSRAM counters stored in the file in NVSRAM, for tracking of NVSRAM writings
 *                  purposes
 *
 * Syntax:          - DOS "any DOS command" - executes any DOS level command
 *                  - DOS "READ" prints out the WriteToNVSRAM counters from the file stored in NVSRAM
 *                  - DOS "CLEAR" clears these counters to 0
 *
 * Parameter:       instr - instruction pointer.
 *
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
//Dos&Counters
int ex_DOS(instr_ptr instr)
{
    char sBuf[50];

    if(CMDgetString(instr->opr_ptr, sBuf, 0) == FAILURE)
        return FAILURE;

    // if user typed READ as an argument, show the content of our log file
    if(!strcmp(sBuf, "READ"))
    {
        FIOWriteToNVSRAMLogFile(READ_FILECOUNTERS);
    }
    // if user typed CLEAR as an argument, clear all the counters
    else if(!strcmp(sBuf, "CLEAR"))
    {
        FIOWriteToNVSRAMLogFile(CLEAR_FILECOUNTERS);
    }
    // other argument = execute DOS system command
    else
    {
        system(sBuf);
    }

    return SUCCESS;
}
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int ex_DSEC(instr_ptr instr)
{
    int iRetVal, iPortNum, iStream, iFunction, iParameter, iSECSFlag;
    unsigned uDevID;
    int iaGenValues[5] = {0, 0, 0, 0, 0};   /* it can be either direct value or index of register for output */
    char pcGenString[256];
    int iSetPCInstr = 0;                    /* returned  1 means set PC = instr, if 0  don't set it */
    int iChangePCWaiting = 0;
    int iCounter;
    long lOprValue;
<<<<<<< HEAD
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    if(CMDgetIntegerValue(Optr, &iPortNum)==FAILURE)
	return FAILURE;

=======
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    if(CMDgetIntegerValue(Optr, &iPortNum)==FAILURE)
	return FAILURE;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    S2SecsDump(iPortNum);

//    return SUCCESS;
}
