/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        LLexinfo.c
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

#include "cmdfns.h"
#include "cmdinf.h"
#include "cmdsp.h"
#include "cmdal.h"
#include "scmac.h"
#include "scio.h"
#include "sctim.h"
#include "gag.h"
#include "scstat.h"
#include "ser.h"
#include "fiog.h"
#include "sctag.h"
#include "scver.h"
#include "scregg.h"
#include "scmem.h"
#include "ro.h"
#include "rofio.h"
#include "gaintr.h"
#include "gafn.h"
#include "sctch.h"
#include "dg.h"
#include "scttr.h"
#include "scio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
#include "scproc.h"
#include "secsg.h"

long lCurrTimerTicks;   /* This is the time stamp used by the password decoding functions */

extern int giNumOfAxes;
extern int iDefineFlag, iEmulatorMode, iaDiagParms[3];

extern char gaIPAddress[14];
extern int giPortno;

char gsVersionString[20] = LIBVERSTRING;

/***************************************************************************
 * Function Name:   ex_GLST
 * Description:     Read galil motion control card status.
 *                  This command will tell you the status of the motion control card in a 32-bit
 *                  status double word. In the case of error on which axis and it also indicates
 *                  on which axis the servo is off. Please look at the "Software Manual" for detail
 *                  description information of status bit.
 * Implementation:  Get the galil status from Mechanism module by calling ROReadGalilStatus() and it
 *                  returns the 32-bit status word. Then is there an operand and operand type is INDIRECT_REG?
 *                  If it is, then store the status value into INDIRECT_REG at specified index that is provided
 *                  in the operand structure. Otherwise, send the status word to comm port.
 * NOTE:            This function will get executed when the user typed GLST command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: GLST [return value]
 *                      - return value: it store the status in INDIRECT_REG only if the user does not want
 *                                      to print out the screen.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_GLST(instr_ptr instr)
{
    unsigned long ulGalilStatus;
    char cBuf[15];

    /* read galil status */
    if( ROReadGalilStatus(&ulGalilStatus) == FAILURE )
		return FAILURE;
    if( instr->opr_ptr )                /* if there is an operand */
    {
		return CMDSetIndirectReg(instr->opr_ptr, (long)ulGalilStatus);
    }
    else                            /* there is no operand */
    {
        /* send the status to the comm port */
        sprintf( cBuf, "%08lX", ulGalilStatus );
		return CMDShowDescription(SERGetCmdPort(), cBuf);
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_VER
 * Description:     Display the firmare version number. It will display the version and more
 *                  depending on the option flag that you passed in the parameter.
 * Implementation:  Get option flag for printing from first operand if there is one.
 *                  If there is no operand, then print Equipe name, copyright and firmware version.
 *                  If there is an operand and the flag value is 1, then print version,
 *                  and date of image was created. If the flag value is 2, then
 *                  print version, date and time of image was created. Otherwise, print
 *                  the firmware version and the executable name and library version build number.
 * NOTE:            This function will get executed when the user typed VER command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: VER [OPTIONAL FLAG]
 *                      - OPTIONAL FLAG:    it is the flag to display difference information of
 *                                          the firmware.
 *                                          - default: display the Equipe name, copyright and
 *                                                     firmware version string.
 *                                          - 1:       display version, and date of image is created.
 *                                          - 2:       display version, time and date of image is created.
 *                                          - 3:       display executable name and library version build number.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_VER(instr_ptr instr)
{
    struct stat f_stat;
    struct tm *f_date;
    char sBuf[VERSTRLEN+1], sBuf1[50], sBuf2[50];
    int iCmdPort;
    long lOperand=0;

    /* get the option flag from first operand. */
    if( instr->opr_ptr )
        lOperand = instr->opr_ptr->opr.l;
    iCmdPort = SERGetCmdPort();
    if( ! lOperand )              /* no operand, print the equipe name, copyright and version */
    {
	if(CMDShowDescription(iCmdPort, "XyZ Automation, Inc.\r\n")==FAILURE)
            return FAILURE;
	if(CMDShowDescription(iCmdPort, "Copyright (c) 2007-2012\r\n")==FAILURE)
            return FAILURE;
	if(CMDShowDescription(iCmdPort, "Version ")==FAILURE)
            return FAILURE;
	if(CMDShowDescription(iCmdPort, GetVersionString())==FAILURE)
            return FAILURE;
	if(CMDShowDescription(iCmdPort, "\r\n")==FAILURE)
            return FAILURE;
    }
    else if(lOperand == 2)
    {
        GAReadGalilRevisionInfo(GA_CARD_0, sBuf2);
        sprintf(sBuf1,"Galil Card firmware version: %s", sBuf2);
        if(CMDShowDescription(iCmdPort, sBuf1)==FAILURE)
            return FAILURE;
    }
    else if(lOperand == 3)
    {
        if (TTReadTPID(sBuf2) == SUCCESS)
        {
            sprintf(sBuf1,"Teach Pendant firmware version: %s", sBuf2);
            if(CMDShowDescription(iCmdPort, sBuf1)==FAILURE)
                return FAILURE;
        }
        else
            return FAILURE;
    }
    else if (lOperand == 4)
    {
	sprintf(sBuf1,"Software Revision: %s", gsVersionString);
	if(CMDShowDescription(iCmdPort, sBuf1) == FAILURE)
	    return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_STAT
 * Description:     Send System Status Word. For specific status bit information, please look at
 *                  "software manual". This status word has information on command executing error, invalid
 *                  command executed, vaccum sensor ans switch status, motor, limit switch, homing flag, alignment
 *                  error, macro status, axis status, servo, COM3 status, controller error and COM1 status. This
 *                  command is used when there is some error occured or some problem while trying to execute somehting.
 * NOTE:            This function will get executed when the user typed STAT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STAT [FLAG]
 *                      - FLAG:     Optional parameter for showing the description of each
 *                                  error status bit if the flag is exist.
 *                                  - 1: for description of each error bit
 *                                  - 2: for description and resolving method for each error bit.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STAT(instr_ptr instr)
{
    return CMDsendSTAT( instr, SSGetSystemStatus(), TRUE );
}
/***************************************************************************
 * Function Name:   ex_GSTAT
 * Description:     Send emulation genmark status. NOT USED
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_GSTAT(instr_ptr instr)
{
    return CMDsendSTAT( instr, SSGetGENSystemStatus(FALSE), FALSE );
}
/***************************************************************************
 * Function Name:   ex_STAT2
 * Description:     Send system status word in 32 bits.  It is a combination of the first 16 bits of STAT and
 *                  the last 16 bits are about COM3 and others about scanner.
 *                  For codemerge (6.00 I2BRT), There is only COM3 status in 6th bit of
 *                  the last 16 status bits for now since there is not S-293 scanner.
 * NOTE:            This function will get executed when the user typed STAT2 command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STAT2 [FLAG]
 *                      - FLAG:     Optional parameter for showing the description of each
 *                                  error status bit if the flag is exist.
 *                                  - 1: for description of each error bit
 *                                  - 2: for description and resolving method for each error bit.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STAT2(instr_ptr instr)
{
    return CMDsendSTAT2( instr, SSGetSystemStatus(), SSGetSystemStatus2() );
}
/***************************************************************************
 * Function Name:   ex_STAT3
 * Description:     Send initialziation failures of basic modules status word in 16 bits.
 * NOTE:            This function will get executed when the user typed STAT3 command
 *                  at the command line. *
 *                  Command Syntax: STAT3 [Rx]
 *                              [Rx] - optional register to store result in
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STAT3(instr_ptr instr)
{
    return CMDsendSTAT( instr, SSGetModulesInitializationsStatus(), FALSE );
}
/***************************************************************************
 * Function Name:   ex_RSRN
 * Description:     Read Serial Number of each component in the system such as robot, prealigner, track,
 *                  flipper. If parameter is omitted or 0, RSRN returns robot serial number.  If parameter is 1, then
 *                  RSRN returns prealigner serial number. 2 is for auxaulary component such as track or flipper.
 * Implementation:
 * NOTE:            This function will get executed when the user typed RSRN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSRN <OPTION>
 *                      - OPTION:   type of component serial number such as Robot, prealigner and track
 *                                  or flipper or dual arm. The value of the option are:
 *                                  - 0 or nothing: robot serial number
 *                                  - 1:            prealigner serial number
 *                                  - 2:            track or flipper serial number.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSRN(instr_ptr instr)
{
    char saSerialNumber[80];
    int iParamDesc;
    int iReturn=FAILURE;
    long lOp=0;
    int iCmdPort;

    iReturn = FAILURE;
    iCmdPort = SERGetCmdPort();

    /* if there is an operand, get the option value for type of serial number. */
    if( instr->opr_ptr )
    {
        if( CMDgetValue(instr->opr_ptr,&lOp) == FAILURE )
            goto error_exit;
    }
    /* get type of serical number */
    if( lOp==0 )
        iParamDesc = ROBOTFILE;
    else if( lOp==1 )
        iParamDesc = PREALIGNFILE;
    else if( lOp==2 )
        iParamDesc = AUXFILE;
    else
        goto error_exit;
    /* get the serial number depending on the option flag. */
    if( FIOGetParamSerialNum(iParamDesc,saSerialNumber) == FAILURE )
        goto error_exit;
    if(CMDShowDescription(iCmdPort, saSerialNumber)==FAILURE)
        goto error_exit;
    iReturn = SUCCESS;

error_exit:
    return iReturn;
}
/***************************************************************************
 * Function Name:   ex_HEAP
 * Description:     Debug instruction which causes information on the heap to be dumped.
 * Implementation:  It called HPHeapDump() to dump the memory information in NVSram such as
 *                  amount of heap used, amount of heap freed, number of bytes available and number of
 *                  bytes used in NVSram.
 * NOTE:            This function will get executed when the user typed HEAP command
 *                  at the command line. The function is actually called in MRExecuteInstructions().
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *                  Command Syntax: HEAP .
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_HEAP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long laHeapResults[4];
    int iOperNum;

    if (HPHeapDump(laHeapResults) == FAILURE)
        return FAILURE;

    Optr = instr->opr_ptr;              /* get first operand */
    for (iOperNum=0; iOperNum<4; iOperNum++)
    {
        if (Optr)
        {
            if (Optr->type != INDIRECT_REG)
                return FAILURE;
            else if (RGSetRegister(Optr->opr.i, laHeapResults[iOperNum]) == FAILURE)
                return FAILURE;
            Optr = Optr->next;          /* get next operand */
        }
    }

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RKEY
 * Description:     Display the current total number of timer ticks.
 * Implementation:  Get the current total number of timer ticks as a long.
 *                  Print it out to the screen.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RKEY(instr_ptr instr)
{
    char sBuf[MAXLINE];
    int iCmdPort;

    /* get the current total number of timer ticks as long */
    lCurrTimerTicks = (signed long)(TIGetTimerVals(SYS_TIMER));
    while (lCurrTimerTicks >= 2000000L)
        lCurrTimerTicks -= 2000000L;
    sprintf( sBuf, "%ld", lCurrTimerTicks );
    iCmdPort = SERGetCmdPort();
    /* send the time and date string to the comm port */
	if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
        return FAILURE;

    return SUCCESS;
}




/****************************************************************
 * Function:        ex_RSTIM
 * Description:     Gets the current timer counts
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_RSTIM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    unsigned long ulTimerCounts;
    char sBuf[MAXLINE];

//    ulTimerCounts = TIGetTimerVals(SYS_TIMER);
    ulTimerCounts = TIRSTime();
    Optr = instr->opr_ptr;
    if(Optr)
    {
        if (RGSetRegister(Optr->opr.i, ulTimerCounts) == FAILURE)
            return FAILURE;
    }
    else
    {
        sprintf( sBuf, "%ld", ulTimerCounts );
        if (CMDShowDescription(SERGetCmdPort(), sBuf) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}

/****************************************************************
 * Function:        ex_RSTMR
 * Description:     Gets the current timer counts
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_RSTMR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    unsigned long ulTimerCounts;
    char sBuf[MAXLINE];

//    ulTimerCounts = TIGetTimerVals(SYS_TIMER);
    ulTimerCounts = TIRSTimeTMR();
    Optr = instr->opr_ptr;
    if(Optr)
    {
        if (RGSetRegister(Optr->opr.i, ulTimerCounts) == FAILURE)
            return FAILURE;
    }
    else
    {
        sprintf( sBuf, "%ld", ulTimerCounts );
        if (CMDShowDescription(SERGetCmdPort(), sBuf) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SCFG
 * Description:     Sets system configuration parameters by decoding passwords
 *                  and then interfacing with the datafile module to set
 *                  configuration file parameters.
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCFG(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iFeatureNum, iOnOffFlag=-1, i;
    double dDecryptedPassword;
    long lPassword, lDecryptedPassword;
    char caSysCfgString[15];

    int iNumAxes;
    int iaEquipeAxis[8], iaGalilAxis[8], iaMechType[8], iaSpecialAxis[8];


    Optr = instr->opr_ptr;              /* get the first parameter */
    if( !Optr )
        return FAILURE;
    if( Optr->type != LONG_CONST )      /* it can only be a constant; registers aren't allowed */
        return FAILURE;
    iFeatureNum = Optr->opr.i;
    if (iFeatureNum != 313) return FAILURE;

//    if (iFeatureNum >= SYSCFG && iFeatureNum != 99)
//        return FAILURE;
//    if( iFeatureNum == 99 )
//        iFeatureNum = SYSCFG;

    Optr = Optr->next;                  /* get the second parameter */
    if( !Optr )
        return FAILURE;
//    if( Optr->type == LONG_CONST )      /* it can only be a constant; registers aren't allowed */
//        iOnOffFlag = Optr->opr.i;
//    else if( Optr->type == STRING_CONST )

    if( Optr->type == STRING_CONST )
    {
        strcpy(caSysCfgString,Optr->opr.s);
//        strupr(caSysCfgString);
	for (i = 0; i < strlen(caSysCfgString); ++i) caSysCfgString[i] = toupper(caSysCfgString[i]);
    }
    else
        return FAILURE;

    if (ValidateSysCfgString(caSysCfgString) == MAXNUMSYSCFGS)
        return FAILURE;
    if( FIOSetCfgSysCfgString(caSysCfgString) == FAILURE )
        return FAILURE;

//    Optr = Optr->next;             /* get the third parameter */
//    if( !Optr )
//        return FAILURE;
//    if( Optr->type != LONG_CONST ) /* it can only be a constant; registers aren't allowed */
//        return FAILURE;
//    lPassword = Optr->opr.l;

    /* Deal with the password checking */
//    dDecryptedPassword = CMDgetDecryptedPassword( iFeatureNum, lCurrTimerTicks );
//    lDecryptedPassword = (long)(dDecryptedPassword);
//    if( lDecryptedPassword == FAILURE )
//        return FAILURE;
//    if( lPassword != lDecryptedPassword )
//        return FAILURE;

    /* set the feature active/deactive value appropriately */
//    if (iOnOffFlag >= 0)
//    {
//        if( FIOSetCfgFeatureFlags(iFeatureNum, iOnOffFlag) == FAILURE )
//            return FAILURE;
//    }
//    else
//    {
//        if (ValidateSysCfgString(caSysCfgString) == MAXNUMSYSCFGS)
//            return FAILURE;
//        if( FIOSetCfgSysCfgString(caSysCfgString) == FAILURE )
//            return FAILURE;
//    }
    if( FIOWriteConfig() == FAILURE )
        return FAILURE;

// continue on initializing the system with new configuration

    FIOReadConfig();

    /* Initialize the firmware version. */
    FIOGetCfgSysCfgString(caSysCfgString);
    InitVersionString(caSysCfgString, __FILE__);
//printf("Config file done...\n");

    InitMechArrays(&iNumAxes, &iDefineFlag, &iEmulatorMode, iaEquipeAxis, iaGalilAxis, iaMechType, iaSpecialAxis, iaDiagParms);
    giNumOfAxes = iNumAxes;

//printf("InitMechArrays done...Axes#=%d\n",giNumOfAxes);

    /* Tell the Status module which emulation to use. */
//    if (SSInitStatus(iDefineFlag, iEmulatorMode) == FAILURE)
 //   {
 //       FailureExit("SSInitStatus has failed");
 //       SSSetModulesInitializationsStatus(STATUS_MODULE, TRUE);
 //   }

    /* Initialize the Mechanism module which in turn initializes the Parameter
     * and Motion modules. */
//    if (ROInit(iNumAxes, iaMechType, iaEquipeAxis, iaGalilAxis, iaSpecialAxis, iDefineFlag, iEmulatorMode) == FAILURE)
//    {
//        FailureExit("ROInit has failed");
//        SSSetModulesInitializationsStatus(MECHANISM_MODULE, TRUE);
//    }
//printf("ROInit done...\n");







    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RCFG
 * Description:     Displays system configuration settings.
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCFG(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iOnOffFlag, iCmdPort;
    char sBuf[MAXLINE];
    long lFeatureNum;

    Optr = instr->opr_ptr;              /* get the first parameter */
    if(CMDgetValue(Optr, &lFeatureNum)==FAILURE)
        return FAILURE;
    if(lFeatureNum != 313) return FAILURE;

    if( FIOGetCfgSysCfgString(sBuf) == FAILURE )
        return FAILURE;

//    if (lFeatureNum >= SYSCFG && lFeatureNum != 99)
//        return FAILURE;
//    if( lFeatureNum == 99 )
//        lFeatureNum = SYSCFG;

    /* get the feature active/deactive value */
//    if (lFeatureNum != SYSCFG)
//    {
//        if( FIOGetCfgFeatureFlags((int)(lFeatureNum), &iOnOffFlag) == FAILURE )
//            return FAILURE;
//    }
//    else
//    {
//        if( FIOGetCfgSysCfgString(sBuf) == FAILURE )
//            return FAILURE;
//    }

    /* save the output to a register if desired */
    Optr = Optr->next;
    if( Optr )
    {
        if( Optr->type == INDIRECT_STR )
        {
            if( RGSetString(Optr->opr.i,sBuf,0) != SUCCESS )
                return FAILURE;
        }
        else
            return FAILURE;
    }
    else
    {
        iCmdPort = SERGetCmdPort();
        /* send the active/deactive value to the comm port */
	if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SDBG
 * Description:     Set Debug bit for general purpose
 *                  0 = off, 1 = debug on.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS
 ***************************************************************************/
int ex_SDBG(instr_ptr instr)
{
    long   lValue;
    CMDoperand_ptr  Optr;

    Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lValue)==FAILURE)
		return FAILURE;
    SSSetDebugBit((unsigned int)lValue);
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RDBG
 * Description:     Reads Debug bit for general purpose
 *                  0 = off, 1 = debug on.
 *                  Also prints out to the screen two usefull information, spurious
 *                  interrupts counter and how many of these happened in critical
 *                  section of code (ALStartMeasurement, WAStopMeasurement) counter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS
 ***************************************************************************/
int ex_RDBG(instr_ptr instr)
{
    unsigned int   uValue;
    char cBuf[140];

    //CMDoperand_ptr  Optr;

    uValue = SSGetDebugBit();

    sprintf(cBuf,"%d\r\n", uValue);
    SERPutsTxBuff(SERGetCmdPort(), cBuf);
    return SUCCESS;
}

//
// XETH is a new XSC command to print IP address and Port number for Ethernet comm
//
int ex_XETH(instr_ptr instr)
{
    char saIPAddress[80];
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    sprintf(saIPAddress, "%s %d", gaIPAddress, giPortno);
    if(CMDShowDescription(iCmdPort, saIPAddress)==FAILURE)
        return FAILURE;

    return SUCCESS;
}
