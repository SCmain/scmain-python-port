/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        LLexec.c
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : cmdex.c
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
 * Program:     low-level command, command interpreter and Opcode table.
 *
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
#include <sys/io.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <malloc.h>

#include "cmdfns.h"
#include "cmdex.h"
<<<<<<< HEAD
#include "cmdsp.h"
=======
#include "cmdsp.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include "cmdal.h"
#include "scmac.h"
#include "scio.h"
#include "sctim.h"
#include "gag.h"
#include "scstat.h"
#include "ser.h"
#include "fiog.h"
#include "fiol.h"
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
#include "scio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
#include "scproc.h"
<<<<<<< HEAD
#include "secsg.h"
#include "scio.h"
#include "scintr.h"
#include "alk.h"

int input_L = 0;
int input_F = 0;

void ALChuckVacuum(int);
void ALPinVacuum(int);
void GAGalilWriteIO(int, int);
int  GAGalilReadIO(int);

extern unsigned long glTimeStart00;
extern unsigned long glTimeStart01;
extern unsigned long glTimeStart10;
extern unsigned long glTimeStart11;
extern unsigned long glTimeStart20;
extern unsigned long glTimeStart21;
extern int giCheck00Flag;
extern int giCheck01Flag;
extern int giCheck10Flag;
extern int giCheck11Flag;
extern int giCheck20Flag;
extern int giCheck21Flag;

extern int giOutpStnNumber;
extern unsigned long glTimeIO[10][4][100];
extern int giTimeIOCounter[10][4];

=======
#include "secsg.h"
#include "scio.h"
#include "scintr.h"
#include "alk.h"

int input_L = 0;
int input_F = 0;

void ALChuckVacuum(int);
void ALPinVacuum(int);
void GAGalilWriteIO(int, int);
int  GAGalilReadIO(int);

extern unsigned long glTimeStart00;
extern unsigned long glTimeStart01;
extern unsigned long glTimeStart10;
extern unsigned long glTimeStart11;
extern unsigned long glTimeStart20;
extern unsigned long glTimeStart21;
extern int giCheck00Flag;
extern int giCheck01Flag;
extern int giCheck10Flag;
extern int giCheck11Flag;
extern int giCheck20Flag;
extern int giCheck21Flag;

extern int giOutpStnNumber;
extern unsigned long glTimeIO[10][4][100];
extern int giTimeIOCounter[10][4];

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//int FORM_MAIN(int,int);

/***************************************************************************
 * Function Name:   ex_FRMT
 * Description:     Format NVSRAM which the read and write disk.
 * Implementaton:   The operand type has to be constant numeric and the operand value has to be 313.
 *                  313 is a magic number - I don't why it is 313.First, we call the function call
 *                  FORM_MAIN(0,1) from purchased library, it does the format the
 *                  NVSram and return the status word, then send the status word to command port. Check status
 *                  is not 6 then the format is done and return sucess. Otherwise, close all initialized devices and
 *                  restores all the vector as necessary and call FORM_MAIN(1,1) to cause the reboot and format.
 * NOTE:            This function will get executed when the user typed FRMT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: FRMT 313
 *                      - 313 always needs to format the NVSram.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FRMT(instr_ptr instr)
{
    //char sBuf[10];
    //int iStatus, iReturn;
    CMDoperand_ptr Optr;

    if( instr->nofopr < 1 )             /* if no operands than it's a problem */
        return FAILURE;
    Optr = instr->opr_ptr;              /* get first operand */
    if( Optr->opr.l != 313 )            /* the value must be 313 */
        return FAILURE;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return SUCCESS;

//    iReturn = FIOWriteConfig();       /* write the configuration table back to NVSRAM */
//    sprintf( sBuf, "%04X", iStatus );
//    CMDShowDescription(SERGetCmdPort(), sBuf);

//    if( iStatus != 6 )                  /* thats all thats necessary with a status!=6 */
//    {
//        if( iReturn == FAILURE )
//            return FAILURE;
        // after format create new WriteToNVSRAMLogFile in NVSRAM with counters equal 0
//        FIOWriteToNVSRAMLogFile(INIT_FILECOUNTERS);
//        return SUCCESS;
//    }

//    if (TIDelay(50) == FAILURE)
//        return FAILURE;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//    FIOWriteToNVSRAMLogFile(INIT_FILECOUNTERS);

//    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_DUMPP
 * Description:     Dump Parameters data from robot, prealigner and auxaulary file.
 *                  Parameters are sent in ASCII presentation. The parameter is showed
 *                  in order as show in the "Software Manual". Please refer to "software
 *                  manual" for more information. They have the fix length and number of lines
 *                  There is 25 lines in every parameter files (execept the old version of controller
 *                  there is 19 lines). The end of each line doesn't include the CR and LF. This
 *                  command will wait for a CR from host after sending each line.
 * Implementation:  First, allocate the memory for temparary buffer to store information to dump to
 *                  the comm port. Then get type of parameter file from operand. Then dump the parameter
 *                  data depend on the type of parameter file. Each line parameter data send to the comm
 *                  port will wait for CR from host before send the next line.  The function that send
 *                  data to comm port and wait unitl the CR is CMDdumpBuf. However, you see in the code
 *                  there is also CMDdumpLparms function and this function combine the data with commas
 *                  and call CMDdumpBuf to send the data and wait for CR. At the end send the CTRL_Z to
 *                  end of the file.
 * NOTE:            This function will get executed when the user typed DUMPP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DUMPP [parameter file type]
 *                      - parameter file type: the value should be 0,1, or 2
 *                        0 or nothing(default):    for robot parameter file
 *                        1:                        for prealigner parameter file
 *                        2:                        for auxaulary parameter file such as track, flipper, dual arm.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPP(instr_ptr instr)
{
    long lBuf[8];
    CMDoperand_ptr Optr;
    char sBuf[MAXLINE];
    int iBaudeRate;
    int iType=ROBOTFILE;                /* default is robot file */
    int iCmdPort;
    int iParmPtr;

    iCmdPort = SERGetCmdPort();

<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* get the parameter file type, default or 0 is robot file*/
    if( (Optr=instr->opr_ptr) )
    {
        if( Optr->opr.l == 1 )          /* prealigner file */
            iType = PREALIGNFILE;
        else if( Optr->opr.l==2 )       /* auxaulary file (track, flipper or dual arm */
            iType = AUXFILE;
    }

    /* get and dump the serial number */
    if( FIOGetParamSerialNum(iType,sBuf) == FAILURE )
        goto error_exit;
    strcat( sBuf, "\r\n" );
    if( CMDdumpBuf(sBuf) == FAILURE )
        goto error_exit;

    /* get and write the baud rate */
    if( FIOGetParamBaudRate(iType,&iBaudeRate) == FAILURE )
        goto error_exit;
<<<<<<< HEAD
//    itoa( iBaudeRate, sBuf, 10 );
=======
//    itoa( iBaudeRate, sBuf, 10 );
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    sprintf(sBuf, "%d", iBaudeRate);
    strcat( sBuf, "\r\n" );
    if( CMDdumpBuf(sBuf) == FAILURE )  /* dump baude rate  and wait for the CR from the host */
        goto error_exit;

    /* get and write the rest of the parameters contained in the file */
    for( iParmPtr=HOME_SPEED; iParmPtr<MAX_PARAM_ITEMS; iParmPtr++ )
    {
        if( FIOGetParamVals(iType,iParmPtr,lBuf) == FAILURE )
            goto error_exit;
        else if( CMDdumpLparms(lBuf) == FAILURE )
            goto error_exit;
    }

    sBuf[0] = CTRL_Z;
    sBuf[1] = 0;
	if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
		goto error_exit;
<<<<<<< HEAD

    return SUCCESS;

error_exit:

=======

    return SUCCESS;

error_exit:

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_DUMPS
 * Description:     Dump Station coordinates. This command will send all coordinates
 *                  information to the comm port. This command is the same as DUMPP that waits for a
 *                  CR from the host.
 * Implementation:  Get customized home from the galil by call MEGetCustomizedHome() to use
 *                  the value to check against with coordinate position for sending or not sending
 *                  to the comm port. So if coordinate position is not equal to customized home, then
 *                  send the coordinate position to the comm, ohterwise skip it.  The coordinate position of
 *                  T,R,Z will be send to the comm and wait for CR from the host before sending another
 *                  coordinate postions.  This will go through the loop from 0 to 51 stations.
 *                  Where the indices(stations) are as following:
 *                      A-Z (ASCII 65- 90) have indices  0-25 respectively
 *                      a-z (ASCII 97-122) have indices 26-51 respectively
 * NOTE:            This function will get executed when the user typed DUMPS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DUMPS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPS(instr_ptr instr)
{
    int iStation, indx, iCmdPort, iContinueFlag, iEndEffector;
    int iStnIdx;
    long lCoord[8], lCHome[8], lScanCoord[8], lCSI[16];
    long lOffset, lStroke, lRetractValue, lPitch, lZindex;
    char sBuf[MAXLINE];

    iCmdPort = SERGetCmdPort();

<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* get Customized Home */
    if( ROGetParameter(TRUE,RO_AXIS_ALL,lCHome,CUSTOMIZED_HOME) == FAILURE )
        goto error_exit;
    /* there are 52 station from A-Z and a-Z */
    for( iStation=0; iStation<52; iStation++ )
    {
        /* get station coordinates */
        for( indx=0; indx<3; indx++ )
        {
            if( FIOGetStnCoord(iStation,indx,&lCoord[indx]) == FAILURE )
                goto error_exit;
        }
        /* get Offset by specifying the station index */
        if( FIOGetStnVals(iStation,OFFSET,&lOffset) == FAILURE )
            goto error_exit;

        /* get stroke */
        if( FIOGetStnVals(iStation,STROKE,&lStroke) == FAILURE )
            goto error_exit;

        /* get Retract position of station */
        if( FIOGetStnVals(iStation,RET_HOME,&lRetractValue) == FAILURE )
            goto error_exit;

        /* get station pitch */
        if( FIOGetStnVals(iStation,PITCH,&lPitch) == FAILURE )
            goto error_exit;

        /* get the scanning coordinate (T,R,Z) */
        for( indx=0; indx<3; indx++ )
        {
            if( FIOGetStnScanCoord(iStation,indx,&lScanCoord[indx])==FAILURE )
                goto error_exit;
        }
        /* get end effector flag */
        if( FIOGetStnEEindex(iStation, &iEndEffector) == FAILURE )
            goto error_exit;
        /* get the Z-index for specified station index/name */
        if( FIOGetStnVals(iStation,Z_INDEX,&lZindex) == FAILURE )
            goto error_exit;

        for( indx=0, iStnIdx=START_ANGLE; iStnIdx<=TRACK_POS; indx++, iStnIdx++)
        {
            if( FIOGetStnVals(iStation,iStnIdx,&lCSI[indx]) == FAILURE )
                goto error_exit;
        }

        for( indx=10; indx<FIO_MAX_NUM_CSI; indx++ )
        {
            if( FIOGetStnlscsi(iStation, indx-10, &lCSI[indx]) == FAILURE )
                goto error_exit;
        }

        iContinueFlag = TRUE;
        /* check coords if customized home is equal then don't print the coordinate information */
        if( (lCoord[0]!=lCHome[0]) || (lCoord[1]!=lCHome[1]) || (lCoord[2]!=lCHome[2]) )
            iContinueFlag = FALSE;
        if( lOffset!=35 )
            iContinueFlag = FALSE;
        if( lStroke!=70 )
            iContinueFlag = FALSE;
        if( lRetractValue!=lCHome[1] )
            iContinueFlag = FALSE;
        if( lPitch!=2500 )
            iContinueFlag = FALSE;
        if( (lScanCoord[0]!=lCHome[0]) || (lScanCoord[1]!=lCHome[1]) || (lScanCoord[2]!=lCHome[2]) )
            iContinueFlag = FALSE;
        if( iEndEffector!=0 )
            iContinueFlag = FALSE;
        if( lZindex!=1 )
            iContinueFlag = FALSE;
        if( lCSI[0]!=50 )
            iContinueFlag = FALSE;
        if( lCSI[1]!=250 )
            iContinueFlag = FALSE;
        if( lCSI[2]!=30 )
            iContinueFlag = FALSE;
        if( lCSI[3]!=75 )
            iContinueFlag = FALSE;
        if( lCSI[4]!=30 )
            iContinueFlag = FALSE;
        if( lCSI[5]!=70 )
            iContinueFlag = FALSE;
        if( lCSI[6]!=0 )
            iContinueFlag = FALSE;
        if( lCSI[7]!=50 )
            iContinueFlag = FALSE;
        if( lCSI[8]!=233 )
            iContinueFlag = FALSE;
        for( indx=9; indx<=15; indx++ )
        {
            if( lCSI[indx]!=0 )
                iContinueFlag = FALSE;
        }

        if ( iContinueFlag )
            continue;

        sprintf( sBuf, "%c%ld,%ld,%ld\r\n", (char)CMDstationCharacter(iStation),
                lCoord[0], lCoord[1], lCoord[2] );
        if( CMDdumpBuf(sBuf) == FAILURE )  /* send the buffer to comm port and wait for CR. */
            goto error_exit;
    }
    sBuf[0] = CTRL_Z;
    sBuf[1] = 0;
	if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
		goto error_exit;
<<<<<<< HEAD

    return SUCCESS;

error_exit:

=======

    return SUCCESS;

error_exit:

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_INPUT
 * Description:     Reads value of port number specified by the parameter.
 *                  This command reads the value of input and output port.  The value read
 *                  is sent to the host or stored in numeric register if it is specified.
 * Implementation:  Get the port name or bit number from an operand.  Then call CMDreadPort() to
 *                  read the port value and there is second operand, then store the port value into
 *                  indirect register type; otherwise, just send the port value to the comm port.
 * NOTE:            This function will get executed when the user typed INPUT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: INPUT <PORT or BIT OF SPECIFIC PORT>, [STORE VALUE]
 *                      - PORT:     when the user wanted get information of whole port by specified the name of port.
 *                      - BIT OF SEPCIFIC PORT: providing the specific bit number of the port.
 *                  NOTE: parameter can be character type or numeric type. If specified the whole port, then parameter
 *                        is character type or INDIRECT_CHAR which is the name of the port. If specified the specific
 *                        bit number, then parameter is numberic type or INDIRECT_REG.
 *                      - STORE VALUE:  it is optional parameter. If it existed, then value of the port will be stored
 *                                      in the variable which must be INDIRECT_REG type.
 *                      PLEASE LOOK AT "SOFTWARE MANUAL" IN I/O MAP SECTION FOR MORE DETAIL.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_INPUT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lBitNum, lValue, lPortID;
<<<<<<< HEAD
    //int iOnOffFlag;
=======
    //int iOnOffFlag;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    //unsigned char iValue;
    char cBuf[5];

    // get first operand, which is the port name or bit number
<<<<<<< HEAD
    Optr = instr->opr_ptr; 
    if(!Optr)
        return FAILURE;

    // set values of Bit number and Port Id
    switch( Optr->type )  
    {
        case LONG_CONST :   // bit number
=======
    Optr = instr->opr_ptr; 
    if(!Optr)
        return FAILURE;

    // set values of Bit number and Port Id
    switch( Optr->type )  
    {
        case LONG_CONST :   // bit number
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        case INDIRECT_REG :
	    if(CMDgetValue(Optr, &lBitNum)==FAILURE)
		return FAILURE;
            lPortID = 0;
            break;
        case CHAR_CONST :   // port name
        case INDIRECT_CHAR :
            lBitNum = -1;
	    if(CMDgetCharacter(Optr, &cBuf[0])==FAILURE)
		return FAILURE;
            lPortID = (long)cBuf[0];
            break;
        default:
            return FAILURE;
    }
<<<<<<< HEAD

    // read the port value
    lValue = (long)(CMDreadPort(lBitNum,(char)lPortID));

    if( lValue==FAILURE )
        return FAILURE;

    // if Second operand, store the value into the indirect register
    if( (Optr=Optr->next) )
=======

    // read the port value
    lValue = (long)(CMDreadPort(lBitNum,(char)lPortID));

    if( lValue==FAILURE )
        return FAILURE;

    // if Second operand, store the value into the indirect register
    if( (Optr=Optr->next) )
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    {
        return CMDSetIndirectReg(Optr, lValue);
    }
    else // no second operand, send the value to comm port.
    {
        sprintf( cBuf, "%02lX", lValue );
        return CMDShowDescription(SERGetCmdPort(), cBuf+(lBitNum==-1?0:1));
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_WRIP
 * Description:     Write parameters to controller. This command writes directly
 *                  to NVSRam and the effect is apparent immediately. The data is
 *                  sending line by line in the order same the DUMPP. The controller
 *                  will return back CR each line had sent down. Please look at
 *                  "Software Manual" for the order of the data.
 * Implementation:  Get file type from first operand. Then get each line of data from comm port
 *                  in the order that expected(look in "software manual" at DUMPP for the order
 *                  of parameter file. After getting each line, call function from Datafile module
 *                  to do the setting and send back to comm port "CR" before getting another line.
 *                  After getting all the data from the file then call ROInitGalil() to set some of
 *                  data to galil and write the data to NVSRam.
 *
 * NOTE:            This function will get executed when the user typed WRIP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: WRIP <FILE TYPE>
 *                      - FILE TYPE:    flag to tell to write with type of file to the controller.
 *                                      - 0 or default: robot parameter file
 *                                      - 1:            prealigner file.
 *                                      - 2:            auxaulary file (track, dual aram or fliper)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iType, iNumBytes, i;
    char sBuf[MAXLINE];
    long lBuf[8];
    int iCmdPort, iParmIdx, iReturn;

    // Increase the WriteToNVSRAM counter for WRIP command and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_WRIPCOMMANDCOUNTER);

    iCmdPort = SERGetCmdPort();

<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* get file type from first operand if there is one. If there is
     * no operand then the default is robot parameter file.
     * If there is operand and value is 0, then it is robot
     * parameter file. */
<<<<<<< HEAD
    iType = ROBOTFILE;
=======
    iType = ROBOTFILE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if( (Optr=instr->opr_ptr) )
    {
        if( Optr->opr.l == 1 )          /* prealigner parameter file */
            iType = PREALIGNFILE;
        else if( Optr->opr.l == 2 )     /* auxaulary parameter file */
            iType = AUXFILE;
    }

    /* get and write the serial number */
    iNumBytes = -1;
    if( (iReturn=SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE)) == FAILURE )
        goto error_exit;
    sBuf[strlen(sBuf)-1] = 0;
    if (strchr(sBuf, ';'))
        *strchr(sBuf, ';') = 0;
    if( (iReturn=FIOSetParamSerialNum(iType,sBuf)) == FAILURE )
        goto error_exit;
	if((iReturn=CMDShowDescription(iCmdPort, "\r"))==FAILURE)
		goto error_exit;

    /* get and write the baud rate */
    iNumBytes = -1;
    if( (iReturn=SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE)) == FAILURE )
        goto error_exit;
    if( (iReturn=FIOSetParamBaudRate(iType,atoi(sBuf))) == FAILURE )
        goto error_exit;
    if((iReturn=CMDShowDescription(iCmdPort, "\r"))==FAILURE)
	goto error_exit;

    for( iParmIdx=HOME_SPEED; iParmIdx<MAX_PARAM_ITEMS; iParmIdx++ )
    {
        if( (iReturn=CMDreadLparms(lBuf)) == FAILURE )
            goto error_exit;
<<<<<<< HEAD
		if (iType == 1)
			for (i=0; i<4; i++)
				lBuf[i+4] = lBuf[i];
=======
		if (iType == 1)
			for (i=0; i<4; i++)
				lBuf[i+4] = lBuf[i];
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if( (iReturn=FIOSetParamVals(iType,iParmIdx,lBuf)) == FAILURE )
            goto error_exit;
    }

    if( (iReturn=ROInitGalil(iType)) == FAILURE )     /* read parameters from data files and write to galil */
    {
        /* if writing to galil failed, read data from
         * the appropriate/original parameter file */
        FIOReadParam(iType);
        ROInitGalil(iType);
        goto error_exit;
    }
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return FIOWriteParam(iType);         /* write information to NVSRAM */

error_exit:
	if(iReturn != FAILURE)
	{
        if( ROInitGalil(iType) == FAILURE )     /* read parameters from data files and write to galil */
        {
            /* if writing to galil failed, read data from
             * the appropriate/original parameter file */
            FIOReadParam(iType);
            ROInitGalil(iType);
        }
        else
            iReturn = FIOWriteParam(iType);         /* write information to NVSRAM */
	}
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return iReturn;
}
/***************************************************************************
 * Function Name:   ex_WRIS
 * Description:     Write Stations to controller and overwritting existing coordinate setup.
 *                  WRIS will write directly to NVSRam. For each station sent to controller will
 *                  send back a CR.
 * Implementation:  while not end of file (ctrl_z), get the station information from the comm port,
 *                  parse out the space and blank line and check for CTRL_Z to end the writing and
 *                  check for invalid station name (a-z or A-Z). Then convert the station name to
 *                  number by call CMDstationNumber().  Then parse the coordinate value and set
 *                  the data into the buffer by call FIOSetStnCoord() and send back to comm port a
 *                  CR. After getting out from the loop, write the coordinate to memory by
 *                  call FIOWriteCoord to write to NVSram.
 *
 * NOTE:            This function will get executed when the user typed WRIS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: WRIS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIS(instr_ptr instr)
{
    int iDone=FALSE, iStationNumber, iNumBytes;
    char sBuf[MAXLINE];
    char *sPtr;
    long laStnCoords[8];
    int iCmdPort;

    // Increase the WriteToNVSRAM counter for WRIS command and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_WRISCOMMANDCOUNTER);

    iCmdPort = SERGetCmdPort();

<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    while( !iDone )                     /* loop while not ctrl-z */
    {
        /* get the station coordinate from the comm port. */
        iNumBytes = -1;
        if( SERGetsRxBuff(SERGetCmdPort(),sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            goto error_exit;
        if( sBuf[strlen(sBuf)-1] == CTRL_Z )  /* last character is a ^Z, it is done.*/
            iDone = TRUE;

        sPtr = sBuf;                    /* set sPtr to first location of sBuf */

        /* look for station number */
        while( *sPtr==' ' )sPtr++;      /* skip white spaces */
        if( *sPtr == CTRL_Z )     /* if a character other than the last is ^Z, exit */
            break;
        else if( *sPtr == '\0' )        /* skip blank lines  */
            continue;
        else if( !isalpha(*sPtr) )      /* non alphabetical character, error  */
            goto error_exit;
        iStationNumber = CMDstationNumber(*sPtr);  /* convert station name to station number */

        while( *++sPtr == ' ');         /* skip white spaces */

        /* parse the station coordinates */
        laStnCoords[0] = atol(strtok(sPtr,","));
        laStnCoords[1] = atol(strtok(NULL,","));
        laStnCoords[2] = atol(strtok(NULL,","));
        /* set the station coordinate to datafile buffer. */
        if( FIOSetStnCoord(iStationNumber,0,laStnCoords[0])==FAILURE ||
            FIOSetStnCoord(iStationNumber,1,laStnCoords[1])==FAILURE ||
            FIOSetStnCoord(iStationNumber,2,laStnCoords[2])==FAILURE )
            goto error_exit;
        /* send back a CR */
        if(CMDShowDescription(iCmdPort, "\r") == FAILURE)
            goto error_exit;
    }
    return FIOWriteCoord();              /* write the NVSram */

error_exit:
    FIOReadCoord();
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_EQUAL
 * Description:     Math assignment statements, e.g. R10 = R11 + R12.
 *                  This is called when the instruction has "=".
 * Implementation:  Get the index of register for storing result from first operand and it
 *                  must be indirect register type. It can be INDIRECT_REG, INDIRECT_STR or
 *                  INDIRECT_CHAR. To know which type of register, we set the flag to whatever
 *                  type of the operand. However, for the INDIRECT_STR, call CMDstringEqual() to
 *                  do the string manipulation and exit the function. if not INDIRECT_STR,
 *                  then get the value of next operand and set the default operator
 *                  to '+', then check for which kind of operator and do the math calculation.  The
 *                  get math operator and forward to next operand. This will continue doing while the
 *                  operand is not NULL. After finish calculation and exit from the loop, then
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EQUAL(instr_ptr instr)
{
    int iRegister, iCharMode;
    long lAccumulator, lArgument;
    char cMathOperation, cArgument;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;              /* get register index from the first operand */
    iRegister = Optr->opr.i;            /* remember value of first operand */
    if( Optr->type == INDIRECT_REG )
        iCharMode = FALSE;
    else if( Optr->type == INDIRECT_CHAR )
        iCharMode = TRUE;
    else                                /* manipulate string */
        return CMDstringEqual( iRegister, Optr->next );

    Optr = Optr->next;                  /* second operand */
    cMathOperation = '+';               /* initial operation */
    lAccumulator = 0;                   /* initialize accumulator */

    while( Optr )                       /* cycle through operands */
    {
        /* get operand */
        switch( Optr->type )
        {
            case LONG_CONST :
            case INDIRECT_REG :
				if(CMDgetValue(Optr, &lArgument)==FAILURE)
					return FAILURE;
                break;
            case CHAR_CONST :
            case INDIRECT_CHAR :
				if(CMDgetCharacter(Optr, &cArgument)==FAILURE)
					return FAILURE;
                lArgument = (long)cArgument;
                break;
            default:
                return FAILURE;
        }
        /* perform math operation */
        switch( cMathOperation )
        {
            case '+' :
                lAccumulator += lArgument;
                break;
            case '-' :
                lAccumulator -= lArgument;
                break;
            case '*' :
                lAccumulator *= lArgument;
                break;
            case '/' :
                if( lArgument == 0 )    /* do not allow divide by 0   */
                    return FAILURE;
                lAccumulator /= lArgument;
                break;
            case '&' :
                lAccumulator &= lArgument;
                break;
            case '|' :
                lAccumulator |= lArgument;
                break;
            case '<' :
                lAccumulator <<= lArgument;
                break;
            case '>' :
                lAccumulator >>= lArgument;
                break;
        }
        /* get next Operator */
        if( !(Optr=Optr->next) )        /* if no operator, exit normally from loop */
            break;
        cMathOperation = Optr->opr.i;   /* get next math operation */
        if( !(Optr=Optr->next) )        /* expect another operator */
            return FAILURE;
    }                                   /* end of while loop */
    if( iCharMode )                     /* if results stored in a character register */
    {
        cArgument = (char)lAccumulator;
        return RGSetCharacter( iRegister, cArgument );
    }
    else
        return RGSetRegister( iRegister, lAccumulator );

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_OUTP
 * Description:     Write value to an output port. If the port number is in input mode
 *                  then commnad is ignored.
 * Implementation:  Get the port name or bit number from the first operand and get the value
 *                  to write to port from second operand. If port is A or L, then call
 *                  MEWrtieGalilPort() to write value to galil output. If port is B,C,D,E, then
 *                  call IOWriteIO() to write the value to output port.
 * NOTE:            This function will get executed when the user typed OUTP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: OUTP <PORT or BIT OF SPECIFIC PORT>, <VALUE>
 *                      - PORT:     when the user wanted set information of whole port by specified the name of port.
 *                                  The output port can be either (A,B,C,D,E,L) or numeric (0..47).
 *                      - BIT OF SEPCIFIC PORT: providing the specific bit number of the port to write to.
 *                  NOTE: parameter can be character type or numeric type. If specified the whole port, then parameter
 *                        is character type or INDIRECT_CHAR which is the name of the port. If specified the specific
 *                        bit number, then parameter is numberic type or INDIRECT_REG.
 *                      - VALUE:  the new value to write to output port.
 *                      PLEASE LOOK AT "SOFTWARE MANUAL" IN I/O MAP SECTION FOR MORE DETAIL.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_OUTP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
<<<<<<< HEAD
    int iBitWrite; //, iOnOffFlag;
=======
    int iBitWrite; //, iOnOffFlag;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    //unsigned char iValue;
    long lBitNum, lValue;
    char cPortID;

    Optr = instr->opr_ptr;
    switch( Optr->type )                /* set values according to the first operand */
    {
        case LONG_CONST :
        case INDIRECT_REG :
            iBitWrite = TRUE;
	    if(CMDgetValue(Optr, &lBitNum)==FAILURE)
		return FAILURE;
            break;
        case CHAR_CONST :
        case INDIRECT_CHAR :
            iBitWrite = FALSE;
	    if(CMDgetCharacter(Optr, &cPortID)==FAILURE)
		return FAILURE;
            break;
        default:
            return FAILURE;
    }

    if( iBitWrite )                     /* portID and BitNum not set, so set them */
    {
        if( lBitNum<0 || lBitNum>47 )
            return FAILURE;
        else if( lBitNum <= 39 )
            cPortID = 'A' + (char)(lBitNum/8);
        else
            cPortID = 'L';
        lBitNum &= 0x07;
    }
    else
        lBitNum = -1;                   /* clear the Bit Number */

    if( CMDgetValue(Optr->next,&lValue) == FAILURE )  /* get value for second operand */
<<<<<<< HEAD
        return FAILURE;

    switch( cPortID )
    {
        case 'A' :
	    if(lBitNum == 0)
    	    {
	    	if((lValue & 0x01) == 0)
	    	{
	    	    glTimeStart00 = TIRSTime();
	    	    giCheck00Flag = 1;
	    	}
	    	else if ((lValue & 0x01) == 1)
	        {
	    		glTimeStart01 = TIRSTime();
	    		giCheck01Flag = 1;
	        }
    	    }
    	    else if (lBitNum == 1)
    	    {
	    	if((lValue & 0x01) == 0)
	    	{
	     	    glTimeStart10 = TIRSTime();
	    	    giCheck10Flag = 1;
	    	}
	    	else if ((lValue & 0x01) == 1)
	     	{
	    	    glTimeStart11 = TIRSTime();
	      	    giCheck11Flag = 1;
	    	}
            }
	    IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_A);
            break;
        case 'B' :
            IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_B );
            break;
        case 'C' :
=======
        return FAILURE;

    switch( cPortID )
    {
        case 'A' :
	    if(lBitNum == 0)
    	    {
	    	if((lValue & 0x01) == 0)
	    	{
	    	    glTimeStart00 = TIRSTime();
	    	    giCheck00Flag = 1;
	    	}
	    	else if ((lValue & 0x01) == 1)
	        {
	    		glTimeStart01 = TIRSTime();
	    		giCheck01Flag = 1;
	        }
    	    }
    	    else if (lBitNum == 1)
    	    {
	    	if((lValue & 0x01) == 0)
	    	{
	     	    glTimeStart10 = TIRSTime();
	    	    giCheck10Flag = 1;
	    	}
	    	else if ((lValue & 0x01) == 1)
	     	{
	    	    glTimeStart11 = TIRSTime();
	      	    giCheck11Flag = 1;
	    	}
            }
	    IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_A);
            break;
        case 'B' :
            IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_B );
            break;
        case 'C' :
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//            IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_C );
            break;
        case 'D' :
            IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_D );
            break;
<<<<<<< HEAD
        case 'E' :
//            IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_E );
            break;
        case 'L' :
	    if(lBitNum == 0)
    	    {
	    	if((lValue & 0x01) == 0)
	    	{
	    	    glTimeStart20 = TIRSTime();
	    	    giCheck20Flag = 1;
	    	}
	    	else if ((lValue & 0x01) == 1)
	        {
	    		glTimeStart21 = TIRSTime();
	    		giCheck21Flag = 1;
	        }
    	    }
	    IOWriteIO( (int)lBitNum, (int)lValue, IO_PRE_OUTPUT_L );
=======
        case 'E' :
//            IOWriteIO( (int)lBitNum, (int)lValue, IO_ROBOT_OUTPUT_E );
            break;
        case 'L' :
	    if(lBitNum == 0)
    	    {
	    	if((lValue & 0x01) == 0)
	    	{
	    	    glTimeStart20 = TIRSTime();
	    	    giCheck20Flag = 1;
	    	}
	    	else if ((lValue & 0x01) == 1)
	        {
	    		glTimeStart21 = TIRSTime();
	    		giCheck21Flag = 1;
	        }
    	    }
	    IOWriteIO( (int)lBitNum, (int)lValue, IO_PRE_OUTPUT_L );
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            break;
        case 'F' :                      /* valid but write nothing */
        case 'G' :
        case 'H' :
        case 'I' :
        case 'J' :
        case 'K' :
            break;
        default:
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RNCS
 * Description:     Read NVSRAM checksum status. There is 16-bit status word which are:
 *                      - bit#0:    Check-sum error on robot parameter file.
 *                      - bit#1:    Check-sum error on coordinate file.
 *                      - bit#2:    Check-sum error on macro file.
 *                      - bit#3:    Check-sum error on pre-aligner parameter file.
 *                      - bit#4:    Check-sum error on pre-aligner wafer file.
 *                      - bit#5:    Check-sum error on pre-aligner calibration file.
 *                      - bit#6:    check-sum error on auxaulary file.
 *                      - bit#7-14: not used (always 0)
 *                      - bit#15:   NVSRam is not formated or does not exist.
 * Implementation:  If there is an operand, then make sure operand type is INDIRECT_REG. Then
 *                  get the check-sum status from Status module by call SSGetCheckSumStatus() and set the
 *                  status into the return register.  If there is no operand, then get the check-sum status
 *                  and send the status to comm port.
 * NOTE:            This function will get executed when the user typed RNCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RNCS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RNCS(instr_ptr instr)
{
	int iStatus;

	iStatus = SSGetCheckSumStatus(0xffff);
	if(CMDSetOrShowStatus(instr->opr_ptr, iStatus)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SMCR
 * Description:     Save macros to NVSRAM.  All macros that have been defined successfully are saved
 *                  to NVSram.  The command will save the macor in the binary form.  If optional
 *                  numerical parameter is specified, macro will use it as file ID. Up to 100 (0-99) file IDs
 *                  are allowed.  These IDs identify macro sets.  If ID is not specified, SMCR will use ID 0.
 *                  In power cycle, macro set wiht ID 0 is automatically loaded from NVSram to DRAM as the
 *                  default macro set.
 * Implementation:  if there is an operand, then save macros to the specified file id that get from the
 *                  operand. Otherwise, save macros to default file id which is 0. To save macros by calling
 *                  the function MASaveMacrotoNvsram().
 * NOTE:            This function will get executed when the user typed SMCR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SMCR [FILE ID]
 *                      - FILE ID:  represent the id of the file to save to and there are 100 file ids (0-99).
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMCR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iReturn = FAILURE;

    //Increase the WriteToNVSRAM counter for SMCR command and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_SMCRCOMMANDCOUNTER);
    /* if no operand, save the macro default file which the file id is 0 */
    if( !(Optr=instr->opr_ptr) )
        iReturn = MASaveMacrotoNvsram(0);  /* save macros to file id 0 */
    else if( Optr->type == LONG_CONST )  /* if there is operand, save macros to the specified file id. */
        iReturn = MASaveMacrotoNvsram((int)Optr->opr.l);

    return iReturn;
}
/***************************************************************************
 * Function Name:   ex_RESC
 * Description:     Restore coordinates from those contained in NVSRAM and use them as current
 *                  coordinate setup.
 * Implementation:  The command has no operand.  Call FIOReadCoord() to restore the coordinates from
 *                  NVSRam
 * NOTE:            This function will get executed when the user typed RESC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RESC
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RESC(instr_ptr instr)
{
    return FIOReadCoord();               /* read the coordinate from NVSram and set the coordinates as
                                         * current coordinates */
}
/***************************************************************************
 * Function Name:   ex_RPAR
 * Description:     Restore parameter file from those contained in NVSRAM and use them as current
 *                  coordinate setup.
 * Implementation:  The command has no operand.  Call FIOReadParam() to restore the parameter
 *                  file from NVSRam. Then call ROInitGalil to restore those values to the Galil card.
 * NOTE:            This function will get executed when the user typed RPAR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RPAR [parameter file type]
 *                      - parameter file type: the value should be 0,1, or 2
 *                        0 or nothing(default):    for robot parameter file
 *                        1:                        for prealigner parameter file
 *                        2:                        for auxaulary parameter file such as track, flipper, dual arm.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPAR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iType=ROBOTFILE;                /* default is robot file */

    /* get the parameter file type, default or 0 is robot file */
    if( (Optr=instr->opr_ptr) )
    {
        if( Optr->opr.l == 1 )          /* prealigner file */
            iType = PREALIGNFILE;
        else if( Optr->opr.l == 2 )     /* auxaulary file (track, flipper or dual arm */
            iType = AUXFILE;
    }

    FIOReadParam(iType);
    return ROInitGalil(iType);          /* read the coordinate from NVSram and set the coordinates as
                                         * current coordinates */
}
/***************************************************************************
 * Function Name:   ex_RMCR
 * Description:     Read macros from NVSRAM.  RMCR deletes curently active macro set and loads macro set from
 *                  NVSram.  The optional numerical parameter indicates which macro set to load. Valid
 *                  parameter is 0-99 then saved by SMCR command.
 * Implementation:  If there is an operand then get the file id value. If there is no operand, then set the
 *                  file id to default which is 0.  Then call MRLoadMacroFromNVsram() to load the macro set
 *                  from the specified file id or default file id in the NVSram to the buffer. This macro set will
 *                  be the current set of macro for executing.
 * NOTE:            This function will get executed when the user typed RMCR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RMCR [FILE ID]
 *                      - FILE ID:  specific file id to load from NVSram since there can be 100 files from NVSram.
 *                                  The file id is in the rangle of 0-99.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMCR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
	int iReturn = FAILURE;

    if (MRGetMacroStatus() == MACRO_RUNNING)
        return FAILURE;
    if( !(Optr=instr->opr_ptr) )        /* if no operands, the default file id is 0. So load the macro file with file id 0*/
        iReturn = MRLoadMacroFromNvsram(0);
    else                                /* if the correct type of operand */
        iReturn = MRLoadMacroFromNvsram((int)Optr->opr.l);  /* load the specified macro file from NVSram */
    return iReturn;
}
/***************************************************************************
 * Function Name:   ex_SAV
 * Description:     Save the parameter and coordinate file to NVSRAM.  all info gotten from galil
 *                  and written to the parameter files in NVSRam.
 *                  The command will get the following values from galil and save into the NVSRam:
 *                  SP, AC, DC, ER, KP, KD, KI, FA, TL, and RM.
 * NOTE:            This function will get executed when the user typed SAV command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SAV
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAV(instr_ptr instr)
{
    long lValue[8]={0,0,0,0,0,0,0,0};
    long lSpeed[8]={0,0,0,0,0,0,0,0};
    long lAccel[8]={0,0,0,0,0,0,0,0};
    long lDecel[8]={0,0,0,0,0,0,0,0};
<<<<<<< HEAD
    int iFileType, iOTFFlag;
=======
    int iFileType, iOTFFlag;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    unsigned long ulAxes;
	int iCount, iIndex;
	unsigned long ulaAxisArray[2] = {0,0};
    int iCardNum, iFileTypeA;
    unsigned uGalilAxes;

     //Dos&Counters
    // Increase the WriteToNVSRAM counter for SAV command and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_SAVCOMMANDCOUNTER);

   for (iFileType=0; iFileType<3; iFileType++)
    {
        /* Get the Equipe axes in this iFileType. */
        ulAxes = ROGetCardAxes(iFileType);

        /* validate the axis, and this function will set the according axis with
         * equipe axis name, galil axis, card number and type of parameter file. */
	if(ulAxes == RO_CONFLICT)
	{
	    iCount = 2;
	    ulaAxisArray[0] = RO_AXIS_W;
	    ulaAxisArray[1] = RO_AXIS_w;
            if (ROValidAxis(RO_AXIS_W, &iCardNum, &iFileTypeA, &uGalilAxes) == FAILURE) continue;
            if (ROValidAxis(RO_AXIS_w, &iCardNum, &iFileTypeA, &uGalilAxes) == FAILURE) continue;
	}
	else
	{
	    iCount = 1;
	    ulaAxisArray[0] = ulAxes;
            if (ROValidAxis(ulAxes, &iCardNum, &iFileTypeA, &uGalilAxes) == FAILURE) continue;
	}

        /* initialize local temp arrays */
        lSpeed[0] = lSpeed[1] = lSpeed[2] = lSpeed[3] = 0;
        lAccel[0] = lAccel[1] = lAccel[2] = lAccel[3] = 0;
        lDecel[0] = lDecel[1] = lDecel[2] = lDecel[3] = 0;
        lSpeed[4] = lSpeed[5] = lSpeed[6] = lSpeed[7] = 0;
        lAccel[4] = lAccel[5] = lAccel[6] = lAccel[7] = 0;
        lDecel[4] = lDecel[5] = lDecel[6] = lDecel[7] = 0;

	for(iIndex = 0; iIndex < iCount; iIndex++)
	{
            /* get the operating speed from galil*/
            if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lSpeed,OPERATIONAL_SPEED) == FAILURE )
            	return FAILURE;
        	/* set the operating speed to NVSRAM */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lSpeed,OPERATIONAL_SPEED) == FAILURE )
            	return FAILURE;
            /* get the operating acceleration from galil*/
            else if (ROGetParameter(FALSE, ulaAxisArray[iIndex], lAccel, OPERATIONAL_ACCEL) == FAILURE)
            	return FAILURE;
            /* set the operating acceleration to NVSRAM */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lAccel,OPERATIONAL_ACCEL) == FAILURE )
            	return FAILURE;
            /* get the error limit from galil*/
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,ERROR_LIMIT) == FAILURE )
            	return FAILURE;
            /* set the error limit to NVSRam */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,ERROR_LIMIT) == FAILURE )
            	return FAILURE;
            /* get the Gain/Proportional constant from galil */
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,PROPORTIONAL_GAIN) == FAILURE )
            	return FAILURE;
            /* set the proportional constant to NVSRam */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,PROPORTIONAL_GAIN) == FAILURE )
            	return FAILURE;
            /* get the feedforward acceleration from galil*/
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,FEED_FORWARD_ACCEL) == FAILURE )
            	return FAILURE;
            /* set the feedforward acceleration to NVSram*/
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,FEED_FORWARD_ACCEL) == FAILURE )
            	return FAILURE;
            /* get the integral gain to galil*/
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,INTEGRAL_GAIN) == FAILURE )
            	return FAILURE;
            /* set the integral gain to NVsram */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,INTEGRAL_GAIN) == FAILURE )
            	return FAILURE;
        	/* get the torque limit from galil*/
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,TORQUE_LIMIT) == FAILURE )
            	return FAILURE;
            /* set the torque limit to NVSram */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,TORQUE_LIMIT) == FAILURE )
            	return FAILURE;
            /* get the compensating zero from galil */
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,DERIVATIVE_GAIN) == FAILURE )
            	return FAILURE;
            /* set the compensating zero to NVSram */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,DERIVATIVE_GAIN) == FAILURE )
            	return FAILURE;
            /* get the ramp value from galil*/
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,JERK) == FAILURE )
            	return FAILURE;
            /* set the ramp value to NVSram */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,JERK) == FAILURE )
            	return FAILURE;
            /* get the IT value from galil*/
            else if( ROGetParameter(FALSE,ulaAxisArray[iIndex],lValue,IT_SCURVE) == FAILURE )
            	return FAILURE;
            /* set the IT value to NVSram */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lValue,IT_SCURVE) == FAILURE )
            	return FAILURE;
            /* get the operating deceleration from galil */
            else if (ROGetParameter(FALSE, ulaAxisArray[iIndex], lDecel, OPERATIONAL_DECEL) == FAILURE)
            	return FAILURE;
            /* set the operating deceleration to NVSram */
            else if( ROSetParameter(TRUE,ulaAxisArray[iIndex],lDecel,OPERATIONAL_DECEL) == FAILURE )
            	return FAILURE;
    	}
    }

    for (iFileType=0; iFileType<3; iFileType++)
    {
        if (FIOWriteParam(iFileType) == FAILURE)  /* save parameter file */
            return FAILURE;
    }

    if (FIOGetCfgFeatureFlags(OTF, &iOTFFlag) == FAILURE)
<<<<<<< HEAD
        return FAILURE;

    if (iOTFFlag)
    {
        if (FIOWriteOTFFile() == FAILURE)
	{
//printf("oft file write error\n");
            return FAILURE;
=======
        return FAILURE;

    if (iOTFFlag)
    {
        if (FIOWriteOTFFile() == FAILURE)
	{
//printf("oft file write error\n");
            return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	}
    }

    /* if 8-axis integrate system, save coordinate, calibration and wafer file. */
    if (CMDinitFlags&DFPRE)
    {
        if ((FIOWriteCoord() | FIOWriteCalibTable() |  FIOWriteWafer()) == FAILURE)
            return FAILURE;
    }

    return FIOWriteCoord();              /* save coordinate file */
}
/***************************************************************************
 * Function Name:   ex_EXINP
 * Description:     get the value of specific port by using the direct address of I/O card.
 * NOTE:            This function will get executed when the user typed EXINP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EXINP <ADDRESS>, [RETURN VALUE]
 *                      - ADDRESS:      direct address at I/O card.
 *                      - RETURN VALUE: stores the return value of port.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EXINP(instr_ptr instr )
{
    CMDoperand_ptr Optr;
    long lPortID, lValue;
    char sBuf[10];

    Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lPortID)==FAILURE)
<<<<<<< HEAD
		return FAILURE;
    if (lPortID < 0x300 || lPortID > 0x300+20)
	return FAILURE;
=======
		return FAILURE;
    if (lPortID < 0x300 || lPortID > 0x300+20)
	return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    lValue = (long)inb((int)lPortID);

    if( (Optr=Optr->next) )          /* if there is second operand, then store the value into the indirect register */
    {
        return CMDSetIndirectReg(Optr, lValue);
    }
    else                                /* no second operand, send the value to comm port. */
    {
        sprintf( sBuf, "%02lX", lValue );
        return CMDShowDescription(SERGetCmdPort(), sBuf);
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_EXOUT
 * Description:
 * NOTE:            This function will get executed when the user typed EXOUT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EXOUT <ADDRESS>, <VALUE>
 *                      - ADDRESS:  direct address at I/O card
 *                      - VALUE:    value to be set to I/O port in the I/O card.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EXOUT(instr_ptr instr )
{
    CMDoperand_ptr Optr;
    long lPortID, lValue;
    char cValue;
    int iBitNo;

    Optr=instr->opr_ptr;        /* if no operands      */
	if(CMDgetValue(Optr, &lPortID)==FAILURE)
		return FAILURE;
<<<<<<< HEAD

    if (lPortID < 0x300 || lPortID > 0x300+20)
	return FAILURE;
=======

    if (lPortID < 0x300 || lPortID > 0x300+20)
	return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    if( !(Optr=Optr->next) )            /* get second operand  */
        return FAILURE;
    switch( Optr->type )                /* get the value to be set */
    {
        case LONG_CONST :
        case INDIRECT_REG :
			if(CMDgetValue(Optr, &lValue)==FAILURE)
				return FAILURE;
            break;
        case CHAR_CONST :
        case INDIRECT_CHAR :
			if(CMDgetCharacter(Optr, &cValue)==FAILURE)
				return FAILURE;
			lValue = (long)cValue;
            break;
        default:
            return FAILURE;
    }

    if( !(Optr=Optr->next) )    /* not a third operand     */
		iBitNo = -1;
    else
    {
        iBitNo = (int)lValue;
        if( iBitNo>7 || iBitNo<0 )  /* check range of 2nd operand */
            return FAILURE;
        switch( Optr->type )
        {
            case LONG_CONST :
            case INDIRECT_REG :
				if(CMDgetValue(Optr, &lValue)==FAILURE)
					return FAILURE;
                break;
            case CHAR_CONST :
            case INDIRECT_CHAR :
				if(CMDgetCharacter(Optr, &cValue)==FAILURE)
					return FAILURE;
				lValue = (long)cValue;
            	break;
            default:
                return FAILURE;
        }
    }
    IOWriteIO(iBitNo,(int)lValue,(int)lPortID);
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_QUERY
 * Description:     QUERY is the "?" command for sending the value to active port. This command
 *                  can be used to send user-generated status or information to host computer.
 *                  The first parameter (D/H) indicates radix used for converting numerical parameter
 *                  (either D(ec) or H(ex)).  It has not effect on character, but must be specified. Also
 *                  It is allowed to have mixed type variables.
 * NOTE:            This function will get executed when the user typed ? command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ?, D/H, <VARIABLE or CONST>
 *                      - D/H:      it is radix for either decimal or hexidecimal.
 *                      - VARIABLE or CONST:    the value to send to the comm port can be stored in the variables or
 *                                              a constant.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_QUERY(instr_ptr instr)
{
    CMDoperand_ptr localOpr;
    instr_ptr localInstr;
    int iRadix, iCmdPort;

    localInstr = instr;
    localOpr = localInstr->opr_ptr;
    iRadix = (localOpr->opr.i=='D')?10:16;  /* hex or dec base */
    iCmdPort = SERGetCmdPort();
    localOpr = localOpr->next;
    return CMDdumpVariables(localOpr, iRadix, iCmdPort);  /* send the value through the comm port. */
}
/********************************************************************************************
 * Function Name:   ex_GALCO
 * Description:     routine for OC number GALCO, which is a command line beginning with '\',
 *                  takes 1. operand from instruction structure, which is a Galil command string,
 *                  put it together and sends to Galil Card, the response received is either
 *                  printed out to the screen or stored into the registers
 * Parameters:      instr - instruction pointer
 *
 * Returns:         SUCCESS/FAILURE
 *********************************************************************************************/
int ex_GALCO(instr_ptr instr)
{
    char pcGalilResponse[100]; //, *pcTemp;
    char caString[100];
    //int iNumOfChars = 1;    /* Number of characters to copy from Galil response to each register */
    CMDoperand_ptr Optr;
    int iLength = 0;
    int iGalilCardNumber;

    Optr = instr->opr_ptr;
    strcpy(caString, Optr->opr.s);
<<<<<<< HEAD
    iLength = strlen(caString);
    if (iLength > 0)
=======
    iLength = strlen(caString);
    if (iLength > 0)
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    {
    	if(caString[iLength - 1] == '>')
        	iLength--;
    	caString[iLength] = '\r';
    	caString[iLength + 1] = '\0';

    	Optr = Optr->next;
    	iGalilCardNumber = Optr->opr.i;

    	if(GASendReceiveGalil(iGalilCardNumber, caString, pcGalilResponse) == FAILURE)
        	return FAILURE;
    	SERPutsTxBuff(SERGetCmdPort(), pcGalilResponse);
<<<<<<< HEAD
    	return SUCCESS;
    }
    else 
=======
    	return SUCCESS;
    }
    else 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SMOD
 * Description:     Command sets the modulo, the position of Theta axis when the enoceder gets reset
 *                  to zero, usually 360 deg, 360000 is equal to 1 revolution on the Theta axis
 *
 *                  Command Syntax: SMOD  <NUMBER OR REGISTER>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMOD(instr_ptr instr)
{
    long lModulo = 0;
    //char cBuf[80];
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;

    if(instr->nofopr == 1)
    {
        if(instr->opr_ptr->type == INDIRECT_REG)
        {
            if( RGGetRegister(Optr->opr.i, &lModulo) == FAILURE )
                return FAILURE;
        }
        else
        {
            if(CMDgetValue(instr->opr_ptr, &lModulo))
                return FAILURE;
        }

        if(ROSetGalilModulo(lModulo) == FAILURE)
            return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RMOD
 * Description:     Command reads the modulo and prints it to the sreen or stores into the register,
 *                  Command Syntax: RMOD  <REGISTER>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMOD(instr_ptr instr)
{
    char cBuf[80];
    long lModulo;

    if(ROReadGalilModulo(&lModulo) == FAILURE)
        return FAILURE;

    sprintf(cBuf, "%li", lModulo);
    if(instr->nofopr == 0)
    {
        if(SERPutsTxBuff(SERGetCmdPort(), cBuf))
            return FAILURE;
    }
    else if((instr->nofopr == 1) && (instr->opr_ptr->type == INDIRECT_REG))
    {
        if(RGSetRegister(instr->opr_ptr->opr.i, lModulo) == FAILURE)
            return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SCM
 * Description:     Command sets the type of the gearing mode. For VAC514 robots with geared Theta
 *                  and Radial axes we can chose the normal mode, when the slave axis is geared to
 *                  the master's actual position. The commanded mode is when the slave's axis is
 *                  geared to the commanded position
 *                  Command Syntax: SCM  <0 or 1>
 *                      0 - actual position gearing
 *                      1 - commanded position gearing
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lMode;

    Optr = instr->opr_ptr;

    if((instr->nofopr == 1))
    {
        if(instr->opr_ptr->type == INDIRECT_REG)
        {
            if( RGGetRegister(Optr->opr.i, &lMode) == FAILURE )
                return FAILURE;
        }
        else
            lMode = Optr->opr.i;

        if((lMode == 0) || (lMode == 1))
            ROSetCM((BOOL)lMode);
        else
            return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RCM
 * Description:     Command reads the type of the gearing mode and prints it either to screen or
 *                  stores into the register
 *                  Command Syntax: RCM  <Register>
 *                      0 - actual position gearing
 *                      1 - commanded position gearing
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCM(instr_ptr instr)
{
    int lMode;
    char cBuf[30];

    lMode = (long) ROGetCM();

    if((instr->nofopr == 1) && (instr->opr_ptr->type == INDIRECT_REG))
    {
        if(RGSetRegister(instr->opr_ptr->opr.i, lMode) == FAILURE)
            return FAILURE;
    }
    else if(instr->nofopr == 0)
    {
        sprintf(cBuf,"%d\n", lMode);
        if(SERPutsTxBuff(SERGetCmdPort(), cBuf))
            return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}

/****************************************************************
 * Function Name:   ex_SHLN
 * Description:     Sets revision number of a given system file.
 * Command Syntax:  SHLN n, [Sn] | "1.0"
 *                  n = 0 robot par, *.par
 *                      1 prealigner par, *.par
 *                      2 auxiliary par, *.par
 *                      3 coordinate file, *.cor
 *                      4 calibration file, *.cal
 *                      5 wafer parameter, *.waf
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_SHLN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char sPtr[MAXSTRINGLENGTH];
    long lVal;
    //int iRegister;

    /* get the numberic value in the first operand.*/
    Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lVal)==FAILURE)
		return FAILURE;
    if (lVal < 0L || lVal > 5)
        return FAILURE;

    /* get the string index in the second operand. It has to be INDIRECT_STR  */
    Optr = Optr->next;
    if(CMDgetString(Optr, sPtr, 0)==FAILURE)
		return FAILURE;
    if (FIOSetFileRevision(lVal, sPtr) == FAILURE)
        return FAILURE;
    return SUCCESS;
}
/****************************************************************
 * Function Name:   ex_RHLN
 * Description:     Reads revision number of a given system file.
 * Command Syntax:  RHLN n<, [Sn]>
 *                  n = 0 robot par, *.par
 *                      1 prealigner par, *.par
 *                      2 auxiliary par, *.par
 *                      3 coordinate file, *.cor
 *                      4 calibration file, *.cal
 *                      5 wafer parameter, *.waf
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_RHLN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char sPtr[MAXSTRINGLENGTH];
    long lVal;
    int iRegister;

    /* get the numberic value in the first operand.*/
    Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lVal)==FAILURE)
		return FAILURE;
    if (lVal < 0L || lVal > 5)
        return FAILURE;

    /* Get the revision string from the file */
    if (FIOGetFileRevision(lVal, sPtr) == FAILURE)
        return FAILURE;

    /* get the string index in the second operand. If no operand, output to COMport */
    Optr = Optr->next;
    if (Optr)
    {
	    if(CMDgetIndirectStrValue(Optr, &iRegister) == FAILURE)
		    return FAILURE;
        if (RGEqualSRegStr(iRegister, sPtr) == FAILURE)
            return FAILURE;
    }
    else
    {
        if(SERPutsTxBuff(SERGetCmdPort(), sPtr) == FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}
<<<<<<< HEAD

//
// Set Output Station Number
//
int ex_XOSN(instr_ptr instr)
{
    //CMDoperand_ptr Optr;
    long lStnNum;

    if(CMDgetValue(instr->opr_ptr, &lStnNum)==FAILURE)
	return FAILURE;

    if(lStnNum < 0 || lStnNum > 9)
	return FAILURE;

    giOutpStnNumber = (int) lStnNum;

    return SUCCESS;
}
//
// Read Output Station Number
//
int ex_XROS(instr_ptr instr)
{
//    CMDoperand_ptr Optr;
//    int lStnNum;

    return CMDReadParam(instr->opr_ptr, (long) giOutpStnNumber);
}
//
// Read Output Station IO Counter
//
int ex_XROC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lStnNum, lCaseNum;
    int iCounter;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lStnNum)==FAILURE)
	return FAILURE;

    if(lStnNum < 0 || lStnNum > 9)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetValue(Optr, &lCaseNum)==FAILURE)
	return FAILURE;

    if(lCaseNum < 0 || lCaseNum > 3)
	return FAILURE;

    iCounter = giTimeIOCounter[lStnNum][lCaseNum];

    Optr=Optr->next;
    return CMDReadParam(Optr, (long) iCounter);
}

//
// XRSI - Read Station IO time for vacuum
//
int ex_XRSI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lStnNum, lCaseNum, lCurPtr, lTime;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lStnNum)==FAILURE)
	return FAILURE;

    if(lStnNum < 0 || lStnNum > 9)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetValue(Optr, &lCaseNum)==FAILURE)
	return FAILURE;

    if(lCaseNum < 0 || lCaseNum > 3)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetValue(Optr, &lCurPtr)==FAILURE)
	return FAILURE;

    if(lCurPtr < 0 || lCurPtr > 99)
	return FAILURE;

    lTime = glTimeIO[lStnNum][lCaseNum][lCurPtr];

    return CMDReadParam(Optr->next, lTime);

}

/***************************************************************************
 * Function Name:   ex_XWIO
 * Description:     Write RC data (Torques & Pos data) to a file
=======

//
// Set Output Station Number
//
int ex_XOSN(instr_ptr instr)
{
    //CMDoperand_ptr Optr;
    long lStnNum;

    if(CMDgetValue(instr->opr_ptr, &lStnNum)==FAILURE)
	return FAILURE;

    if(lStnNum < 0 || lStnNum > 9)
	return FAILURE;

    giOutpStnNumber = (int) lStnNum;

    return SUCCESS;
}
//
// Read Output Station Number
//
int ex_XROS(instr_ptr instr)
{
//    CMDoperand_ptr Optr;
//    int lStnNum;

    return CMDReadParam(instr->opr_ptr, (long) giOutpStnNumber);
}
//
// Read Output Station IO Counter
//
int ex_XROC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lStnNum, lCaseNum;
    int iCounter;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lStnNum)==FAILURE)
	return FAILURE;

    if(lStnNum < 0 || lStnNum > 9)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetValue(Optr, &lCaseNum)==FAILURE)
	return FAILURE;

    if(lCaseNum < 0 || lCaseNum > 3)
	return FAILURE;

    iCounter = giTimeIOCounter[lStnNum][lCaseNum];

    Optr=Optr->next;
    return CMDReadParam(Optr, (long) iCounter);
}

//
// XRSI - Read Station IO time for vacuum
//
int ex_XRSI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lStnNum, lCaseNum, lCurPtr, lTime;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lStnNum)==FAILURE)
	return FAILURE;

    if(lStnNum < 0 || lStnNum > 9)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetValue(Optr, &lCaseNum)==FAILURE)
	return FAILURE;

    if(lCaseNum < 0 || lCaseNum > 3)
	return FAILURE;

    Optr = Optr->next;
    if(CMDgetValue(Optr, &lCurPtr)==FAILURE)
	return FAILURE;

    if(lCurPtr < 0 || lCurPtr > 99)
	return FAILURE;

    lTime = glTimeIO[lStnNum][lCaseNum][lCurPtr];

    return CMDReadParam(Optr->next, lTime);

}

/***************************************************************************
 * Function Name:   ex_XWIO
 * Description:     Write RC data (Torques & Pos data) to a file
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XWIO(instr_ptr instr)
{
<<<<<<< HEAD
    return IOWriteSTNIO();
}
/***************************************************************************
 * Function Name:   ex_XRIO
 * Description:     Read RC data (Torques & Pos data) from a file
=======
    return IOWriteSTNIO();
}
/***************************************************************************
 * Function Name:   ex_XRIO
 * Description:     Read RC data (Torques & Pos data) from a file
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XRIO(instr_ptr instr)
{
<<<<<<< HEAD
    return IOReadSTNIO();
=======
    return IOReadSTNIO();
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
}
