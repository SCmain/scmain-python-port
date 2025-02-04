/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * File Name            : cmdal.c
 * Description          : Routines for low level commands from Aligner module
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
 *
 * Program:     Aligner low level commands
 *
 * Functions:   ex_SCSN
 *              ex_ALST
 *              ex_BAL
 *              ex_BASQ
 *              ex_BCOR
 *              ex_CALC
 *              ex_CCT
 *              ex_MCOR
 *              ex_MEAS
 *              ex_RAD1
 *              ex_RAD2
 *              ex_RCAFP
 *              ex_RCASP
 *              ex_RCFP
 *              ex_RCSP
 *              ex_RCT
 *              ex_RDA
 *              ex_RFAA
 *              ex_RFT
 *              ex_RMAC
 *              ex_RMAF
 *              ex_RMI
 *              ex_RMSC
 *              ex_RMSC2
 *              ex_RMSC3
 *              ex_RMSF
 *              ex_RMSF2
 *              ex_RMSF3
 *              ex_RMT
 *              ex_RPMO
 *              ex_RPTY
 *              ex_RSP1
 *              ex_RSP2
 *              ex_RWL
 *              ex_RWS
 *              ex_RWT
 *              ex_RWU
 *              ex_SAD1
 *              ex_SAD2
 *              ex_SCAFP
 *              ex_SCASP
 *              ex_SCFP
 *              ex_SCSP
 *              ex_SCT
 *              ex_SDA
 *              ex_SFAA
 *              ex_SFT
 *              ex_SMAC
 *              ex_SMAF
 *              ex_SMSC
 *              ex_SMSC2
 *              ex_SMSC3
 *              ex_SMSF
 *              ex_SMSF2
 *              ex_SMSF3
 *              ex_SMT
 *              ex_SPMO
 *              ex_SSP1
 *              ex_SSP2
 *              ex_SWL
 *              ex_SWS
 *              ex_SWT
 *              ex_SWU
 *              ex_TRACE
 *              ex_ZZ1
 *              ex_ZZ2
 *              ex_DUMPC
 *              ex_DUMPM
 *              ex_DUMPW
 *              ex_WRIW
 * 				ex_SCVAC
 * 				ex_RCVAC
 *              CMDAlign
 *              CMDReadParam
 *              CMDSetParam3
 *
 * Description: Routines for low level commands from Aligner module
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
\***************************************************************/

#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include "cmdfns.h"
#include "cmdsp.h"
#include "cmdal.h"
#include "scregg.h"
#include "ser.h"
#include "ro.h"
#include "roga.h"
#include "alk.h"
#include "alfio.h"
#include "alstep.h"
#include "alcomp.h"
#include "alisr.h"
#include "fiol.h"
#include "fiog.h"
#include "mapstn.h"
#include "scta.h"
#include "scmac.h"
#include "scintr.h"
#include "scstat.h"
#include "scmem.h"

char caString[6];
int iFpError = 0;
int iHexToggle = TRUE;

/**************************************************************************
 * Function Name:   ex_SCSN
 * Description:     Sets calibration file serial number
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCSN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char sBuf[21];

    if( (Optr = instr->opr_ptr) )
    {
        if( CMDgetString(Optr,sBuf,0) == SUCCESS )
            return FIOSetCalibSerNum(sBuf);
    }
    return FAILURE;
}
/**************************************************************************
 * Function Name:   ex_ALST
 * Description:     Gets Alignment status error message
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ALST(instr_ptr instr)
{
    int iAlignmentStatus; //iCmdPortNumber, 
    //char caTempString[100];

    if( ALGetIntVars(AL_ALIGNMENT_STATUS,AL_NOIDX,&iAlignmentStatus) == FAILURE )
        return FAILURE;
	if(CMDSetOrShowStatus(instr->opr_ptr, iAlignmentStatus)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_BAL
 * Description:     Begin Alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BAL(instr_ptr instr)
{
    return CMDAlign(instr, BAL);
}
/***************************************************************************
 * Function Name:   ex_BASQ
 * Description:     Begin Square Wafer Alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BASQ(instr_ptr instr)
{
    return CMDAlign(instr, BASQ);
}
/***************************************************************************
 * Function Name:   ex_BCOR
 * Description:     Begin Correction of offset and flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BCOR(instr_ptr instr)
{
    return CMDAlign(instr, BCOR);
}
/***************************************************************************
 * Function Name:   ex_CALC
 * Description:     Get item from the calibration table.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CALC(instr_ptr instr)
{
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_CCT
 * Description:     Remove CT node.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CCT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lWaferSize, lType;

    if(instr->nofopr != 2)              /* if not 2 operands */
        return FAILURE;

    Optr = instr->opr_ptr;              /* get wafer size from the first operand */
	if(CMDgetValue(Optr, &lWaferSize)==FAILURE)
		return FAILURE;

    Optr = Optr->next;                  /* get type from second operand */
	if(CMDgetValue(Optr, &lType)==FAILURE)
		return FAILURE;
    return FIORemoveCTNode(lWaferSize, lType);
}
/***************************************************************************
 * Function Name:   ex_MCOR
 * Description:     Makes Correction
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MCOR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    double dDestination;
    long lCoord, lValue;
	long laPos[8]={0,0,0,0,0,0,0,0};
	ULONG ulAxis = RO_AXIS_R;
	//char cAxis;
	ULONG ulDualAxis;
	int iEndEffector;

    Optr = instr->opr_ptr;        /* get 1st operand */
    if((iStationIndex = (int)CMDgetStnIndx(Optr)) == FAILURE)  /* get station index */
		return FAILURE;
    if(FIOGetStnCoord(iStationIndex,1,&lCoord) == FAILURE)  /* get station coordinate */
		return FAILURE;
	if(FIOGetStnEEindex(iStationIndex, &iEndEffector)==FAILURE)
		return FAILURE;
    dDestination = (double)lCoord*25.4;
	Optr = Optr->next;
	if(CMDgetValue(Optr, &lValue)==FAILURE)
		return FAILURE;
	dDestination += (double)lValue;
	dDestination = dDestination / 25.4;
	if(dDestination < 0)
		dDestination += -0.5;
	else
		dDestination +=  0.5;
	ulDualAxis = ROGetSpecialAxis(RO_DUAL_ARM);
	if((iEndEffector == 1) && (ulDualAxis == RO_AXIS_W))
	{
		ulAxis = RO_AXIS_W;
		laPos[3] = (long)dDestination;
	}
	else
		laPos[1] = (long)dDestination;
	if(ROMoveToAbs(ulAxis, laPos)==FAILURE)
		return FAILURE;
	return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_MEAS
 * Description:     Measure Alignment, don't correct
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MEAS(instr_ptr instr)
{
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RAD1
 * Description:     Read the Wafer Parameter, Measurement acceleration, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RAD1(instr_ptr instr)
{
	int i;
    long laParameters[8];
    ULONG ulAxis;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    ulAxis = CMDgetAxis(instr);
    if(ulAxis < RO_AXIS_ALL)
        return FAILURE;
    if(FIOGetParamWaferAccel(laParameters) == FAILURE)
        return FAILURE;
    laParameters[3] = laParameters[0];
	for (i=0; i<4; ++i)
		laParameters[i+4] = laParameters[i];

    if (Optr)
    	Optr = Optr->next;
    return CMDreadBackParms(Optr, laParameters, ulAxis);
}
/***************************************************************************
 * Function Name:   ex_RAD2
 * Description:     Read the Wafer Parameter, Measurement acceleration, no wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RAD2(instr_ptr instr)
{
	int i;
    long laParameters[8];
    ULONG ulAxis;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    ulAxis = CMDgetAxis(instr);
    if(ulAxis < RO_AXIS_ALL)
        return FAILURE;
    if(FIOGetParamNoWaferAccel(laParameters) == FAILURE)
        return FAILURE;
    laParameters[3] = laParameters[0];
	for (i=0; i<4; ++i)
		laParameters[i+4] = laParameters[i];
    if (Optr)
    	Optr = Optr->next;
    return CMDreadBackParms(Optr, laParameters, ulAxis);
}
/***************************************************************************
 * Function Name:  ex_RCAFP
 * Description:     Read the Wafer Parameter, CCD1 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCAFP(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, CCD1_ANGLE);
}
/***************************************************************************
 * Function Name:   ex_RCASP
 * Description:     Read the Wafer Parameter, CCD2 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCASP(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, CCD2_ANGLE);
}
/***************************************************************************
 * Function Name:   ex_RCFP
 * Description:     Read the Wafer Parameter, CCD1 First Pixel Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCFP(instr_ptr instr)
{
    return CMDReadWaferValue(instr->opr_ptr, CCD1_FIRST_PIXEL);
}
/***************************************************************************
 * Function Name:   ex_RCSP
 * Description:     Read the Wafer Parameter, CCD2 first Pixel position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCSP(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, CCD2_FIRST_PIXEL);
}
/***************************************************************************
 * Function Name:   ex_RCT
 * Description:     Read Calibration Table.
 * Implementation:  First, make sure there is at least 3 operand in the instruction.  Then get the
 *                  first operand which wafer size.  Get the second operand which is the type of wafer and
 *                  get the third operand which the item or index in the calibration table structure.  Then use
 *                  informtion that was provided from first 3 operands to get the data from calibration table. If
 *                  there is the fourth operand then set the data into the register; otherwise send the data to
 *                  comm port.
 * NOTE:            This function will get executed when the user typed RCT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCT <WAFER SIZE>, <WAFER TYPE>, <ITEM>, [RETURN VALUE]
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lWaferSize, lType, lItemNum, lData;

    Optr = instr->opr_ptr;            /* get wafer size */
    if(CMDgetValue(Optr, &lWaferSize)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                /* get type */
    if(CMDgetValue(Optr, &lType)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                /* get Item Number */
    if(CMDgetValue(Optr, &lItemNum)==FAILURE)
        return FAILURE;
    if(lItemNum<0 || lItemNum>=MAXCTITEMS)
        return FAILURE;
    /* by providing wafer size and type of wafer and item to get the data */
    if((lData = FIOGetCalibTblItem(lWaferSize,lType,lItemNum)) == FAILURE)
        return FAILURE;

    return CMDReadParam(Optr->next, lData);
}
/***************************************************************************
 * Function Name:   ex_RDA
 * Description:     For calibration table
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RDA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lParm1, lResult;

    if(instr->nofopr > 2)
        return FAILURE;

    Optr = instr->opr_ptr;              /* 1st operand is the first parameter */
	if(CMDgetValue(Optr, &lParm1)==FAILURE)
		return FAILURE;
    if(lParm1<0 || lParm1>=MAXCTITEMS)
        return FAILURE;

    lResult = FIOGetItemFromCalTbl((int)lParm1);
    return CMDreadBackParms(Optr->next, &lResult, 0);
}
/***************************************************************************
 * Function Name:   ex_RFAA
 * Description:     Read the Wafer Parameter, Flat angle after alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFAA(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, FLAT_ANGLE);
}
/***************************************************************************
 * Function Name:   ex_RFT
 * Description:     Read the Wafer Parameter, Flat type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFT(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, FLAT_TYPE);
}
/***************************************************************************
 * Function Name:   ex_RMAC
 * Description:     Read the Wafer Parameter, Measurement acceleration for centering
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMAC(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_ACCEL_CENTER);
}
/***************************************************************************
 * Function Name:   ex_RMAF
 * Description:     Read the Wafer Parameter, Measurement Acceleration for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMAF(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_ACCEL_FLAT);
}
/***************************************************************************
 * Function Name:   ex_RMI
 * Description:     Read last good data in each section of data. There are 3
 *                  portion of data from 0-1999, 2000-2499, and 2500-2999.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char caString[80];
    unsigned long ulDuplicateCount, ulBadCount;
    int iCmdPortNumber, iOffsetIndexEnd, iFlatIndexEndLastRead, iFlatEndPrev;

    iCmdPortNumber = SERGetCmdPort();
    ALGetDUPCount(&ulDuplicateCount);
    ALGetBadCounter(&ulBadCount);
    if( ALGetIntVars(AL_OFFSET_INDEX_END,AL_NOIDX,&iOffsetIndexEnd) == FAILURE )
        return FAILURE;
    if( ALGetIntVars(AL_OFFSET_INDEX_END,AL_NOIDX,&iFlatIndexEndLastRead) == FAILURE )
        return FAILURE;
    if( ALGetIntVars(AL_FLAT_INDEX_END_PREV,AL_NOIDX,&iFlatEndPrev) == FAILURE )
        return FAILURE;

    if((instr->nofopr != 0) && (instr->nofopr != 5))
        return FAILURE;

    if( instr->nofopr == 5 )
    {
        Optr = instr->opr_ptr;
		if(CMDSetIndirectReg(Optr, (long)iOffsetIndexEnd)==FAILURE)
			return FAILURE;
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, (long)iFlatEndPrev)==FAILURE)
			return FAILURE;
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, (long)iFlatIndexEndLastRead)==FAILURE)
			return FAILURE;
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, (long)ulDuplicateCount)==FAILURE)
			return FAILURE;
        Optr = Optr->next;
        if(CMDSetIndirectReg(Optr, (long)ulBadCount)==FAILURE)
			return FAILURE;
    }
    else
    {
        /* No variable specified, output to RS-232 */
        sprintf(caString, "%d,%d,%d,%ld,%ld", iOffsetIndexEnd, iFlatEndPrev, iFlatIndexEndLastRead, ulDuplicateCount, ulBadCount);
		if(CMDShowDescription(iCmdPortNumber, caString)==FAILURE)
			return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RMSC
 * Description:     Read the Wafer Parameter, Measurement speed for centering
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSC(instr_ptr instr)
{
    return CMDReadWaferValue(instr->opr_ptr, MEASURE_SPEED_CENTER);
}
/***************************************************************************
 * Function Name:   ex_RMSC2
 * Description:     Read the Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSC2(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_SPEED_CNTRF200);
}
/***************************************************************************
 * Function Name:   ex_RMSC3
 * Description:     Read the Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSC3(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_SPEED_CNTRF300);
}
/***************************************************************************
 * Function Name:
 * Description:     Read the Wafer Parameter, Measurement speed for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSF(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_SPEED_FLAT);
}
/***************************************************************************
 * Function Name:   ex_RMSF2
 * Description:     Read the Wafer Parameter, Measurement speed for flat F200
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSF2(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_SPEED_FLAT_F200);
}
/***************************************************************************
 * Function Name:   ex_RMSF3
 * Description:     Read the Wafer Parameter, Measurement speed for flat F300
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSF3(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_SPEED_FLAT_F300);
}
/***************************************************************************
 * Function Name:   ex_RMT
 * Description:     Read the Wafer Parameter, Measurement Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMT(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, MEASURE_TYPE);
}
/***************************************************************************
 * Function Name:   ex_RPMO
 * Description:     Read Prealigner Mode
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPMO(instr_ptr instr)
{
    int iMode; // iCmdPortNumber;
    //char caTempString[100];
    //CMDoperand_ptr  Optr;

    if( ALGetIntVars(AL_PREALIGNER_MODE,AL_NOIDX,&iMode) == FAILURE )
        return FAILURE;
	if(CMDReadParam(instr->opr_ptr, (long)iMode)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RPTY
 * Description:     Read Prealigner Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPTY(instr_ptr instr)
{
    int   iType; // iCmdPortNumber;
    //char caTempString[100];
    //CMDoperand_ptr  Optr;

    if( ALGetIntVars(AL_PREALIGNER_TYPE,AL_NOIDX,&iType) == FAILURE )
        return FAILURE;
	if(CMDReadParam(instr->opr_ptr, (long)iType)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
 /***************************************************************************
 * Function Name:   ex_RSP1
 * Description:     Read the Wafer Parameter, Measurement speed, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSP1(instr_ptr instr)
{
	int i;
    long laParameters[8];
    ULONG ulAxis;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    ulAxis = CMDgetAxis(instr);
    if(ulAxis < RO_AXIS_ALL)
        return FAILURE;
    if(FIOGetParamWaferSpeed(laParameters) == FAILURE)
        return FAILURE;
    laParameters[3] = laParameters[0];
	for (i=0; i<4; i++)
	{
		laParameters[i+4] = laParameters[i];
	}
    if (Optr)
    	Optr = Optr->next;
    return CMDreadBackParms(Optr, laParameters, ulAxis);
}
/***************************************************************************
 * Function Name:   ex_RSP2
 * Description:     Read the Wafer Parameter, Measurement speed, no wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSP2(instr_ptr instr)
{
	int i;
    long laParameters[8];
    ULONG ulAxis;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    ulAxis = CMDgetAxis(instr);
    if(ulAxis < RO_AXIS_ALL)
        return FAILURE;
    if(FIOGetParamNoWaferSpeed(laParameters) == FAILURE)
        return FAILURE;
    laParameters[3] = laParameters[0];
	for (i=0; i<4; i++)
	{
		laParameters[i+4] = laParameters[i];
	}
    if (Optr)
    	Optr = Optr->next;
    return CMDreadBackParms(Optr, laParameters, ulAxis);
}
/***************************************************************************
 * Function Name:   ex_RWL
 * Description:     Read the Wafer Parameter, Wafer Loading Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWL(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, WAFER_LOAD_POSITION);
}
/***************************************************************************
 * Function Name:   ex_RWS
 * Description:     Read the Wafer Parameter, Wafer Size
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWS(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, WAFER_SIZE);
}
/***************************************************************************
 * Function Name:   ex_RWT
 * Description:     Read the Wafer Parameter, Wafer Type
 *                  There are two wafer type values but they contain the same value;
 *                  therefore, when the user reads the value it will return WAFER_CAL_TYPE
 *                  value. Look at SWT for more information.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWT(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, WAFER_CAL_TYPE);
}
/***************************************************************************
 * Function Name:   ex_RWU
 * Description:     Read the Wafer Parameter, Wafer Unloading Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWU(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, WAFER_LIFT_UP_POSITION );
}
/***************************************************************************
 * Function Name:   ex_SAD1
 * Description:     Set the wafer parameter movement acceleration, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAD1(instr_ptr instr)
{
    long laParameters[8];

    if(FIOGetParamWaferAccel(laParameters) == FAILURE)
        return FAILURE;
    if(CMDSetParam3(instr, laParameters))
        return FAILURE;
    if(FIOSetParamWaferAccel(laParameters) == FAILURE)
        return FAILURE;

    return  SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SAD2
 * Description:     Set Wafer parameters movement acceleration, wafer not on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAD2(instr_ptr instr)
{
    long laParameters[8];

    if(FIOGetParamNoWaferAccel(laParameters) == FAILURE)
        return FAILURE;
    if(CMDSetParam3(instr, laParameters))
        return FAILURE;
    if(FIOSetParamNoWaferAccel(laParameters) == FAILURE)
        return FAILURE;

    return  SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SCAFP
 * Description:     Set the wafer parameter, CCD1 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCAFP(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, CCD1_ANGLE);
}
/***************************************************************************
 * Function Name:   ex_SCASP
 * Description:     Set the wafer parameter, CCD2 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCASP(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, CCD2_ANGLE);
}
/***************************************************************************
 * Function Name:   ex_SCFP
 * Description:     Set the wafer parameter, CCD1 First Pixel Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCFP(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, CCD1_FIRST_PIXEL);
}
/***************************************************************************
 * Function Name:   ex_SCSP
 * Description:     Set the wafer parameter, CCD2 First Pixel Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCSP(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, CCD2_FIRST_PIXEL);
}
/***************************************************************************
 * Function Name:   ex_SCT
 * Description:     Set Calibration Table.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lWaferSize, lType, lItemNo, lValue;

    if(instr->nofopr != 4)
        return FAILURE;

    Optr = instr->opr_ptr;        /* get wafer size from 1st operand */
	if(CMDgetValue(Optr, &lWaferSize)==FAILURE)
		return FAILURE;

    Optr = Optr->next;            /* get type from 2nd operand */
	if(CMDgetValue(Optr, &lType)==FAILURE)
		return FAILURE;

    Optr = Optr->next;            /* get item number from 3rd operand */
	if(CMDgetValue(Optr, &lItemNo)==FAILURE)
		return FAILURE;
    if(lItemNo<0 || lItemNo>=MAXCTITEMS)
        return FAILURE;

    Optr = Optr->next;            /* get the new value from 4th operand */
	if(CMDgetValue(Optr,&lValue)==FAILURE)
		return FAILURE;

    return FIOEnterCalibTableItem(lWaferSize,lType,lItemNo,lValue);
}
/***************************************************************************
 * Function Name:   ex_SDA
 * Description:     For calibration table
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SDA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lParm1, lParm2;

    if(instr->nofopr != 2)
        return FAILURE;

    Optr = instr->opr_ptr;              /* get parameter 1 from the 1st operand */
	if(CMDgetValue(Optr, &lParm1)==FAILURE)
		return FAILURE;
    if(lParm1<0 || lParm1>=MAXCTITEMS)
        return FAILURE;

    Optr = Optr->next;                  /* get parameter 2 from the 2nd operand */
	if(CMDgetValue(Optr, &lParm2)==FAILURE)
		return FAILURE;

    return FIOSetItemToCalTbl((int)lParm1, lParm2);  /* set calibration table with info */
}
/***************************************************************************
 * Function Name:   ex_SFAA
 * Description:     Set the wafer parameter flat angle after alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SFAA(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, FLAT_ANGLE);
}
/***************************************************************************
 * Function Name:   ex_SFT
 * Description:     Set the wafer parameter, Flat Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SFT(instr_ptr instr)
{
    return CMDSetWaferValue(instr->opr_ptr, FLAT_TYPE );
}
/***************************************************************************
 * Function Name:   ex_SMAC
 * Description:     Set the wafer parameter measurement acceleration for centering
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMAC(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_ACCEL_CENTER);
}
/***************************************************************************
 * Function Name:   ex_SMAF
 * Description:     Set the wafer parameter Measurement speed for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMAF(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_ACCEL_FLAT);
}
/***************************************************************************
 * Function Name:   ex_SMSC
 * Description:     Set the wafer parameter measurement speed for centering.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSC(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_SPEED_CENTER);
}
/***************************************************************************
 * Function Name:   ex_SMSC2
 * Description:     Set Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSC2(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_SPEED_CNTRF200);
}
/***************************************************************************
 * Function Name:   ex_SMSC3
 * Description:     Set Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSC3(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_SPEED_CNTRF300);
}
/***************************************************************************
 * Function Name:   ex_SMSF
 * Description:     Set the wafer parameter Measurement acceleration for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSF(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_SPEED_FLAT);
}
/***************************************************************************
 * Function Name:   ex_SMSF2
 * Description:     prealigner
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSF2(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr,MEASURE_SPEED_FLAT_F200 );
}
/***************************************************************************
 * Function Name:   ex_SMSF3
 * Description:     prealigner
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSF3(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_SPEED_FLAT_F300);
}
/***************************************************************************
 * Function Name:   ex_SMT
 * Description:     Set the wafer parameter measurement type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMT(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, MEASURE_TYPE);
}
/***************************************************************************
 * Function Name:   ex_SPMO
 * Description:     Set Prealigner Mode
 *                  0 = standard, 1 = fast speed prealigner.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE, returns FAILURE if trying to set 1
 *                  for standard prealigner.
 ***************************************************************************/
int ex_SPMO(instr_ptr instr)
{
    long   lMode;
    CMDoperand_ptr  Optr;

    Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lMode)==FAILURE)
		return FAILURE;

    if ((lMode < 0L) || (lMode > 1L))
        return FAILURE;

	if(ALSetPrealignerMode((int)lMode)==FAILURE)
        return FAILURE;
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SSP1
 * Description:     Set the wafer parameter movement speed, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSP1(instr_ptr instr)
{
    long laParameters[8];

    if(FIOGetParamWaferSpeed(laParameters) == FAILURE)
        return FAILURE;
    if(CMDSetParam3(instr, laParameters))
        return FAILURE;
    if(FIOSetParamWaferSpeed(laParameters) == FAILURE)
        return FAILURE;

    return  SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SSP2
 * Description:     Set the wafer parameter movement speed, wafer not on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSP2(instr_ptr instr)
{
    long laParameters[8];

    if(FIOGetParamNoWaferSpeed(laParameters) == FAILURE)
        return FAILURE;
    if(CMDSetParam3(instr, laParameters))
        return FAILURE;
    if(FIOSetParamNoWaferSpeed(laParameters) == FAILURE)
        return FAILURE;

    return  SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SWL
 * Description:     Set the wafer parameter wafer loading type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWL(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, WAFER_LOAD_POSITION);
}
 /***************************************************************************
 * Function Name:   ex_SWS
 * Description:     Set the wafer parameter, Wafer Size
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWS(instr_ptr instr)
{
    long lWaferCalType=0, lParameter, lOldValue;
    CMDoperand_ptr Optr;
    int iOnOffFlag;

    Optr = instr->opr_ptr;

    if(CMDgetValue(Optr, &lParameter) == FAILURE)
        return FAILURE;
    if(ALValidWaferSize(lParameter) == FALSE)
        return FAILURE;
    if(FIOGetParamWaferVals(WAFER_SIZE, &lOldValue) == FAILURE)
        return FAILURE;
    if(FIOSetParamWaferVals(WAFER_SIZE, lParameter) == FAILURE)
        return FAILURE;

    if((Optr = Optr->next))
    {
        if(CMDgetValue(Optr, &lWaferCalType) == FAILURE)
            return FAILURE;
    }
    if(FIOSetParamWaferVals(WAFER_CAL_TYPE,lWaferCalType) == FAILURE)
        return FAILURE;
    if(FIOSetParamWaferVals(WAFER_TYPE,lWaferCalType) == FAILURE)
        return FAILURE;

    //If we have OTF we don't want to setup enviroment for wafer aligning,
    //we just need to keep the wafer size stored
    if(FIOGetCfgFeatureFlags(OTF, &iOnOffFlag) == FAILURE)
        return FAILURE;
    if(!iOnOffFlag)
    {
        if(ALSetWaferSize((int)lParameter) == FAILURE)
        {
            /* just restore the old value */
            FIOSetParamWaferVals(WAFER_SIZE, lOldValue);
            return FAILURE;
        }
    }

    return  SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SWT
 * Description:     Set the wafer parameter wafer type
 *                  There are two wafer type value, and they have the same value;
 *                  therefore, when user sets the wafer type value, the value will
 *                  be stored into two places (WAFER_TYPE, WAFER_CAL_TYPE). However, we
 *                  will use the value in WAFER_CAL_TYPE when the user reads it.
 *                  Look at RWT for further information.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWT(instr_ptr instr)
{
	if(CMDSetWaferValue(instr->opr_ptr, WAFER_TYPE)==FAILURE)
		return FAILURE;
	if(CMDSetWaferValue(instr->opr_ptr, WAFER_CAL_TYPE)==FAILURE)
		return FAILURE;
    return  SUCCESS;
}
 /***************************************************************************
 * Function Name:   ex_SWU
 * Description:     Set the wafer parameter, Wafer unloading position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWU(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, WAFER_LIFT_UP_POSITION);
}
 /***************************************************************************
 * Function Name:   ex_TRACE
 * Description:     Read Trace variable
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TRACE(instr_ptr instr)
{
    int iTrace, iTableIndx;
    char caTempString[100];
    CMDoperand_ptr Optr;

    if( ALGetIntVars(AL_TRACE,AL_NOIDX,&iTrace) == FAILURE )
        return FAILURE;
    Optr = instr->opr_ptr;

    if(instr->nofopr)
    {
        if(CMDgetIntegerValue(Optr, &iTableIndx)==FAILURE)
            return FAILURE;
        if( ALGetIntVars(AL_TRACE_TABLE,iTableIndx,&iTrace) == FAILURE )
            return FAILURE;
        if(instr->nofopr == 2)
        {
            Optr = Optr->next;
            if (Optr->type == INDIRECT_REG)
            {
                if(RGSetRegister(Optr->opr.i, (long)iTrace) == FAILURE)
                    return FAILURE;
                return SUCCESS;
            }
            else
                return FAILURE;
        }
    }
    sprintf(caTempString, "%d", iTrace);
    if( SERPutsTxBuff(SERGetCmdPort(), caTempString) == FAILURE )
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_ZZ1
 * Description:     Debugging command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ZZ1(instr_ptr instr)
{
    long lTempParameter;
    char caString[15];
    int iCmdPortNumber;
    CMDoperand_ptr Optr;

    iCmdPortNumber = SERGetCmdPort();

    if(instr->nofopr == 0)
    {
        if( ALGetLongVars(AL_OFFSET_VECTOR,AL_NOIDX,&lTempParameter) == FAILURE )
            return FAILURE;
        sprintf(caString, "%ld\r\n", lTempParameter);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;

        if( ALGetLongVars(AL_OFFSET_ANGLE,AL_NOIDX,&lTempParameter) == FAILURE )
            return FAILURE;
        sprintf(caString, "%ld\r\n", lTempParameter);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;

        if( ALGetLongVars(AL_WAFER_SIZE,AL_NOIDX,&lTempParameter) == FAILURE )
            return FAILURE;
        sprintf(caString, "%ld\r\n", lTempParameter);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;

        if( ALGetLongVars(AL_FLAT_MIDPOINT,AL_NOIDX,&lTempParameter) == FAILURE )
            return FAILURE;
        sprintf(caString, "%ld\r\n", lTempParameter);
		if(CMDShowDescription(iCmdPortNumber, caString)==FAILURE)
            return FAILURE;

        return SUCCESS;
    }
    Optr = instr->opr_ptr;

    if( ALGetLongVars(AL_OFFSET_VECTOR,AL_NOIDX,&lTempParameter) == FAILURE )
        return FAILURE;
	if(CMDSetIndirectReg(Optr, lTempParameter)==FAILURE)
		return FAILURE;
    Optr = Optr->next;

    if( ALGetLongVars(AL_OFFSET_ANGLE,AL_NOIDX,&lTempParameter) == FAILURE )
        return FAILURE;
	if(CMDSetIndirectReg(Optr, lTempParameter)==FAILURE)
		return FAILURE;
    Optr = Optr->next;

    if( ALGetLongVars(AL_WAFER_SIZE,AL_NOIDX,&lTempParameter) == FAILURE )
        return FAILURE;
	if(CMDSetIndirectReg(Optr, lTempParameter)==FAILURE)
		return FAILURE;
    Optr = Optr->next;

    if( ALGetLongVars(AL_FLAT_MIDPOINT,AL_NOIDX,&lTempParameter) == FAILURE )
        return FAILURE;
	if(CMDSetIndirectReg(Optr, lTempParameter)==FAILURE)
		return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_ZZ2
 * Description:     Debugging command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
void takeAL(int iMode);
int ex_ZZ2(instr_ptr instr)
{
//    return CMDAlign(instr, ZZ2);
    long   lMode;
    CMDoperand_ptr  Optr;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lMode)==FAILURE)
	return FAILURE;

    takeAL((int)lMode);

    return SUCCESS;

}

/***************************************************************************
 * Function Name:   ex_DUMPC
 * Description:     Dump Calibration Table
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPC(instr_ptr instr )
{
    char sBuf[MAXLINE];
    int iCmdPort;
    long lWaferSize, lType;
	int iReturn;

    iCmdPort = SERGetCmdPort();

    //sBuf = (char *)malloc(MAXLINE);
//    if(!sBuf)
 //   {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "EX_DUMPC: out of memory for malloc string buffer.***\n\r?");
//#endif
//        return FAILURE;
//    }
    if( (iReturn=FIOGetCalibSerNum(sBuf)) == FAILURE )   /* get serial # */
        goto error_exit;
    strcat( sBuf, "\r\n" );
    if( (iReturn=CMDdumpBuf(sBuf)) == FAILURE )  /* dump serial number */
        goto error_exit;
    while(FIOGetCTNext(&lWaferSize, &lType) == SUCCESS)
    {
        sprintf(sBuf, "%10ld %10ld\r\n", lWaferSize, lType);
        if( (iReturn=CMDdumpBuf(sBuf)) == FAILURE )
            goto error_exit;
    }
    sBuf[0] = CTRL_Z;
    sBuf[1] = 0;
	if((iReturn=CMDShowDescription(iCmdPort, sBuf))==FAILURE)
        goto error_exit;

error_exit:
//    if(sBuf)
//        free(sBuf);
    return iReturn;
}
/***************************************************************************
 * Function Name:   ex_DUMPM
 * Description:     Dump Prealigner data
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iCCDFirstPixelPos, iDataDiff, iDataStat, iCmdPortNumber;
    char cString[MAXLINE];
    long lIndex, lDataAngle;
    unsigned int uiData;

    if(instr->nofopr != 1 && instr->nofopr != 5)
        return FAILURE;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lIndex) == FAILURE)
        return FAILURE;
    if (lIndex<0)
    {
        if (lIndex == -1)
            iHexToggle = FALSE;
        else if (lIndex == -2)
            iHexToggle = TRUE;
        else
            return FAILURE;
        return SUCCESS;
    }
    iCmdPortNumber = SERGetCmdPort();
    ALGetPreData(lIndex, &uiData);
    if( ALGetIntVars(AL_CCD_FIRST_PIXEL_POS,AL_NOIDX,&iCCDFirstPixelPos) == FAILURE )
        return FAILURE;
    if( ALGetIntVars(AL_DATA_DIFF,(int)lIndex,&iDataDiff) == FAILURE )
        return FAILURE;
    if( ALGetIntVars(AL_STAT,(int)lIndex,&iDataStat) == FAILURE )
        return FAILURE;
    if( ALGetLongVars(AL_DATA_ANGLE,(int)lIndex,&lDataAngle) == FAILURE )
        return FAILURE;

    if(instr->nofopr == 5)
    {
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, (long)(uiData - iCCDFirstPixelPos))==FAILURE)
			return FAILURE;
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, (long)iDataDiff)==FAILURE)
			return FAILURE;
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, (long)iDataStat)==FAILURE)
			return FAILURE;
        Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, lDataAngle)==FAILURE)
			return FAILURE;
    }
    else
    {
        /* No variable specified, output to RS-232 */
        if (iHexToggle)
            sprintf(cString, "%d,%d,%02X,%ld", (int)(uiData) - iCCDFirstPixelPos, iDataDiff, iDataStat, lDataAngle);
        else
            sprintf(cString, "%d,%d,%d,%ld", (int)(uiData) - iCCDFirstPixelPos, iDataDiff, iDataStat, lDataAngle);
		if(CMDShowDescription(iCmdPortNumber, cString)==FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_WRIM
 * Description:     Write Prealigner data
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iCCDFirstPixelPos, iDataDiff, iDataStat;
    long lIndex, lVal;
    unsigned int uiData;

    if(instr->nofopr != 5)
        return FAILURE;

    /* get the first parameter, the index */
    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lIndex) == FAILURE)
        return FAILURE;

    /* get the current CCDFirstPixelPos value */
    if( ALGetIntVars(AL_CCD_FIRST_PIXEL_POS,AL_NOIDX,&iCCDFirstPixelPos) == FAILURE )
        return FAILURE;
    /* get the second parameter */
    Optr = Optr->next;
    if(CMDgetValue(Optr, &lVal) == FAILURE)
        return FAILURE;
    /* increment the CCDFirstPixelPos value by the second parameter, and store the results */
    uiData = (unsigned int)(lVal + (long)(iCCDFirstPixelPos));
    ALSetPreData(lIndex, uiData);

    /* get the third parameter, and set giDataDiff[lIndex] = <3rd parameter> */
    Optr = Optr->next;
    if(CMDgetValue(Optr, &lVal) == FAILURE)
        return FAILURE;
    iDataDiff = (int)(lVal);
    if( ALSetIntVars(AL_DATA_DIFF,(int)lIndex,iDataDiff) == FAILURE )
        return FAILURE;

    /* get the 4th parameter, and set giDataStat[lIndex] = <4th parameter> */
    Optr = Optr->next;
    if(CMDgetValue(Optr, &lVal) == FAILURE)
        return FAILURE;
    iDataStat = (int)(lVal);
    if( ALSetIntVars(AL_STAT,(int)lIndex,iDataStat) == FAILURE )
        return FAILURE;

    /* get the 5th parameter, and set giAngle[lIndex] = <5th parameter> */
    Optr = Optr->next;
    if(CMDgetValue(Optr, &lVal) == FAILURE)
        return FAILURE;
    if( ALSetLongVars(AL_DATA_ANGLE,(int)lIndex,lVal) == FAILURE )
        return FAILURE;
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_DUMPW
 * Description:     Dump Wafer Parameters - prealigner
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPW(instr_ptr instr)
{
    //long *lpTemp;
    int iCmdPortNumber;
    char sBuf[MAXLINE];
    int iParmPtr;
    long lParamVal, laParamVal[8];
	int iReturn;

//    sBuf = (char *)malloc(MAXLINE*sizeof(char));
//    if(!sBuf)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPortNumber, "EX_DUMPW: out of memory for malloc string buffer.***\n\r?");
//#endif
//        return FAILURE;
//    }
    iCmdPortNumber = SERGetCmdPort();

    /* Serial Number */
    if( (iReturn=FIOGetParamWaferSerialNum(sBuf)) == FAILURE )
        goto error_exit;
    strcat( sBuf, "\r\n" );
    if((iReturn=CMDdumpBuf(sBuf))==FAILURE)
        goto error_exit;

    /* Dump parameters measurement speed for centering through dummy1 */
    for( iParmPtr=MEASURE_SPEED_CENTER; iParmPtr<=REVERSE_MODE; iParmPtr++ )
    {
        if( (iReturn=FIOGetParamWaferVals(iParmPtr,&lParamVal)) == FAILURE )
			goto error_exit;
//        ltoa( lParamVal, sBuf, 10 );
	sprintf(sBuf, "%d", (int)lParamVal);
        strcat( sBuf, "\r\n" );
        if( (iReturn=CMDdumpBuf(sBuf))==FAILURE )
            goto error_exit;
    }

    /*  Dump parameter speed with wafer */
    if( (iReturn=FIOGetParamWaferSpeed(laParamVal)) == FAILURE )
		goto error_exit;
    for( iParmPtr = 0; iParmPtr < 3; iParmPtr++)
    {   /* dump out one by one */
        sprintf( sBuf, "%ld\r\n", laParamVal[iParmPtr] );
        if( (iReturn=CMDdumpBuf(sBuf))==FAILURE )
            goto error_exit;
    }

    /*  Dump parameter acceleration with wafer */
    if( (iReturn=FIOGetParamWaferAccel(laParamVal)) == FAILURE )
		goto error_exit;
    for( iParmPtr = 0; iParmPtr < 3; iParmPtr++)
    {   /* dump out one by one */
        sprintf( sBuf, "%ld\r\n", laParamVal[iParmPtr] );
        if( (iReturn=CMDdumpBuf(sBuf))==FAILURE )
            goto error_exit;
    }

    /*  Dump parameter speed without wafer */
    if( (iReturn=FIOGetParamNoWaferSpeed(laParamVal)) == FAILURE )
		goto error_exit;
    for( iParmPtr = 0; iParmPtr < 3; iParmPtr++)
    {   /* dump out one by one */
        sprintf( sBuf, "%ld\r\n", laParamVal[iParmPtr] );
        if( (iReturn=CMDdumpBuf(sBuf))==FAILURE )
            goto error_exit;
    }

    /*  Dump parameter acceleration without wafer */
    if( (iReturn=FIOGetParamNoWaferAccel(laParamVal)) == FAILURE )
		goto error_exit;
    for( iParmPtr = 0; iParmPtr < 3; iParmPtr++)
    {   /* dump out one by one */
        sprintf( sBuf, "%ld\r\n", laParamVal[iParmPtr] );
        if( (iReturn=CMDdumpBuf(sBuf))==FAILURE )
            goto error_exit;
    }

    /* Dump parameters from flat angle till the end of the parameters */
    for( iParmPtr=FLAT_ANGLE; iParmPtr<MAX_WAFER_PARAM_ITEMS; iParmPtr++ )
    {
        if( (iReturn=FIOGetParamWaferVals(iParmPtr,&lParamVal)) == FAILURE )
			goto error_exit;
//        ltoa( lParamVal, sBuf, 10 );
	sprintf(sBuf, "%d", (int)lParamVal);
        strcat( sBuf, "\r\n" );
        if( (iReturn=CMDdumpBuf(sBuf))==FAILURE )
            goto error_exit;
    }


    sBuf[0] = CTRL_Z;
    sBuf[1] = 0;
    if( (iReturn=SERPutsTxBuff(iCmdPortNumber, sBuf)) == FAILURE )
		goto error_exit;

error_exit:
//    if (sBuf)
//        free(sBuf);
    return iReturn;
}
/***************************************************************************
 * Function Name:   ex_WRIW
 * Description:     Write wafer file to controller. This command writes directly
 *                  to NVSRam and the effect is apparent immediately. The data is
 *                  sending line by line. The controller
 *                  will return back CR each line had sent down. Please look at
 *                  "Software Manual" for the order of the data.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIW(instr_ptr instr )
{
    char sBuf[MAXLINE];
    int iParmIdx, iNumBytes, iCmdPort;
    long laValue[8]; //lValue, 

    //Dos&Counter
    // Increase the WriteToNVSRAM counter for WRIW command and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_WRIWCOMMANDCOUNTER);

    iCmdPort = SERGetCmdPort();          /* get the port */
    /* serial number - file line 1 */
    iNumBytes = -1;
    if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
        return FAILURE;
    sBuf[strlen(sBuf)-1] = 0;
    if( FIOSetParamWaferSerialNum(sBuf) == FAILURE )
        return FAILURE;
    if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
        return FAILURE;

    /* the measurement speed for centering through dummy1  - file lines 2-9 */
    for( iParmIdx=MEASURE_SPEED_CENTER; iParmIdx<=REVERSE_MODE; iParmIdx++ )
    {
        iNumBytes = -1;
        if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            return FAILURE;
        if( FIOSetParamWaferVals(iParmIdx,atol(sBuf)) == FAILURE )
            return FAILURE;
		if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
            return FAILURE;
    }

    /* speed with wafer, 3 speeds to retrieve - file lines 10-12 */
    for( iParmIdx=0; iParmIdx<3; iParmIdx++ )
    {
        iNumBytes = -1;
        if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            return FAILURE;
		if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
            return FAILURE;
        laValue[iParmIdx] = atol(sBuf);
    }
    if( FIOSetParamWaferSpeed(laValue) == FAILURE )
        return FAILURE;

    /* acceleration with wafer, 3 values to retrieve - file lines13-15 */
    for( iParmIdx=0; iParmIdx<3; iParmIdx++ )
    {
        iNumBytes = -1;
        if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            return FAILURE;
		if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
            return FAILURE;
        laValue[iParmIdx] = atol(sBuf);
    }
    if( FIOSetParamWaferAccel(laValue) == FAILURE )
        return FAILURE;

    /* speed without wafer, 3 values to retrieve - file lines 16-18 */
    for( iParmIdx=0; iParmIdx<3; iParmIdx++ )
    {
        iNumBytes = -1;
        if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            return FAILURE;
		if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
            return FAILURE;
        laValue[iParmIdx] = atol(sBuf);
    }
    if( FIOSetParamNoWaferSpeed(laValue) == FAILURE )
        return FAILURE;

    /* acceleration without wafer, 3 values to retrive - file lines 19-21 */
    for( iParmIdx=0; iParmIdx<3; iParmIdx++ )
    {
        iNumBytes = -1;
        if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            return FAILURE;
		if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
            return FAILURE;
        laValue[iParmIdx] = atol(sBuf);
    }
    if( FIOSetParamNoWaferAccel(laValue) == FAILURE )
        return FAILURE;

    /* from flat angle till the end of the parameters */
    for( iParmIdx=FLAT_ANGLE; iParmIdx<MAX_WAFER_PARAM_ITEMS; iParmIdx++ )
    {
        iNumBytes = -1;
        if( SERGetsRxBuff(iCmdPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            return FAILURE;
        if( FIOSetParamWaferVals(iParmIdx,atol(sBuf)) == FAILURE )
            return FAILURE;
		if(CMDShowDescription(iCmdPort, "\r")==FAILURE)
            return FAILURE;
    }

    return FIOWriteWafer();              /* write the parameters to memory */
}
/***************************************************************************
 * Function Name:   CMDAlign
 * Description:     Common function for BAL, BCOR, BASQ and ZZ2 commands
 * Parameter:       intstr - instruction pointer
 *                  iFlag - distinguishing parameter, BAL, BASQ, BCOR, ZZ2
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDAlign(instr_ptr instr, int iFlag)
{
    int iRetValue, iDisplayMode, iCmdPortNumber, iNumberOfCorrections, iBalStep;
    int iTrace, iFpError = 0, iOffsetIndexEndArg, iDataCollInProg, iAlignmentStatus;
    long lDataAngle, lSingleBalStep, lTemp, lAxis; //, lWaferType;
    unsigned int uiData;
    char caTempString[100];
    CMDoperand_ptr Optr, Optr_step;
	//ULONG ulAxis;
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    //long laZDownPosition[4] = { 0, 0, 0, 0 };
    //long lZPosition;

    Optr = instr->opr_ptr;

    if(instr->nofopr == 0)
    {
        if( ALSetIntVars(AL_DISPLAY_MODE,AL_NOIDX,0) == FAILURE )
            return FAILURE;
    }
    else if(Optr->type == INDIRECT_REG)
    {
        if( RGGetRegister(Optr->opr.i, &lTemp) == FAILURE )
            return FAILURE;
        if( ALSetIntVars(AL_DISPLAY_MODE,AL_NOIDX,(int)lTemp) == FAILURE )
            return FAILURE;
    }
    else
    {
        if( ALSetIntVars(AL_DISPLAY_MODE,AL_NOIDX,Optr->opr.i) == FAILURE )
            return FAILURE;
    }
    if( instr->nofopr == 3 )
    {
        Optr_step = instr->opr_ptr->next->next;
        if( Optr_step->type == INDIRECT_REG )
        {
            if(RGGetRegister(Optr_step->opr.i,&lSingleBalStep) == FAILURE)
                return FAILURE;
        }
        else
            lSingleBalStep = Optr_step->opr.l;
        if( ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,(int)lSingleBalStep) == FAILURE )
            return FAILURE;
    }

    iCmdPortNumber = SERGetCmdPort();
    if((iFlag == BAL) || (iFlag == BASQ) || (iFlag == ZZ2) || (iFlag == BALI))
        TKSetPCCriticalSection(1);

    if( ALGetIntVars(AL_DISPLAY_MODE,AL_NOIDX,&iDisplayMode) == FAILURE )
        return FAILURE;

    if(iDisplayMode == FAILURE)
   	{
        ALAbort();
        SSSetStatusWord(ALIGNING, TRUE);
        return FAILURE;
   	}
    if((iFlag == ZZ2)||(((iFlag == BAL)||(iFlag == BALI)||(iFlag == BCOR)) && (CMDinitFlags & DFSAP4)))
    {
        ALGetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,&iBalStep);
        if(iDisplayMode > 99)
        {
            if(iBalStep == 0)
            {
                ALAbort();
                SSSetStatusWord(ALIGNING, TRUE);
                return FAILURE;
            }
            iDisplayMode -= 100;
        }
        else
        {
            if(iBalStep >= 7)
                ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,0);
        }
    }

//    iRetValue = setjmp(ISGetJmpBufMarkPtr());
    iRetValue = 0;
    if(iRetValue == 0)
    {
        if( ALGetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,&iBalStep) == FAILURE )
            iBalStep = 100;
        switch(iBalStep)
     	{
            case 0:
		if(ALGetLongVars(AL_ALL_PRE_AXES, AL_NOIDX, &lAxis)==FAILURE)
			return FAILURE;
                if( ROAxisReadyToMove((unsigned long)lAxis, &iCardNum, &iFileType, &uGalilAxes) != SUCCESS )
                {
                    iRetValue = FAILURE;
                    break;
                }
                SSSetStatusWord(ALIGNING, FALSE);
                iRetValue = ALBegin();
                break;
     	    case 1:
                iRetValue = ALSetIntVars(AL_ALIGNING_FLAG,AL_NOIDX,TRUE);
                if( iRetValue == SUCCESS )
		    iRetValue = ALInit(iFlag);
         	    break;
     	    case 2:
                iRetValue = ALStartMeasureProc();
        	    break;
     	    case 3:
                if(iFlag == BASQ)
                    iRetValue = ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,++iBalStep);
                else
                    iRetValue = ALCheckEdge();
        	    break;
     	    case 4:
                if(iFlag == BASQ)
                    iRetValue = ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,++iBalStep);
                else
                    iRetValue = ALFilterEdgeNotch();
                break;
     	    case 5:
                if(iFlag == BASQ)
                {
                    if( ALGetIntVars(AL_OFFSET_INDEX_END,AL_NOIDX,&iOffsetIndexEndArg) == FAILURE )
                        iRetValue = FAILURE;
                    else if( ALGetIntVars(AL_DATA_COLLECTION,AL_NOIDX,&iDataCollInProg) == FAILURE )
                        iRetValue = FAILURE;
                    if(iDataCollInProg)
                        break;
                    if( iRetValue == SUCCESS )
                    {
                        if(iDisplayMode == 7)
                        {
                            for(iRetValue = AL_BEGIN_DATA; iRetValue < iOffsetIndexEndArg; iRetValue++)
                            {
                                if( ALGetLongVars(AL_DATA_ANGLE,iRetValue,&lDataAngle) == FAILURE )
                                {
                                    iRetValue = FAILURE;
                                    break;
                                }
                                ALGetPreData((long) iRetValue, &uiData);
                                sprintf(caTempString, "%d,%ld,%u\r\n", iRetValue, lDataAngle, uiData);
                                if( SERPutsTxBuff(iCmdPortNumber, caTempString) == FAILURE )
                                {
                                    iRetValue = FAILURE;
                                    break;
                                }
                            }
                        }
                        iRetValue = ALSQWaferAlignment(iDisplayMode);
                    }
                    break;
                }
                else
                    iRetValue = ALCenterCalc(iFlag);
        	    break;
     	    case 6:
                if( (iFlag == BASQ && iDisplayMode == 128) || (iFlag == ZZ2) || (CMDinitFlags & DFSAP4) )
				{
                    if(CMDinitFlags & DFSAP4)
					{
                        if(CMDReadAlignStatus(Optr->next) == FAILURE)
                        {
                            iRetValue = FAILURE;
							break;
                        }
					}
                    if (ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,++iBalStep) == FAILURE)
                    {
                        iRetValue = FAILURE;
                        break;
                    }
                    return SUCCESS;
				}
                else
                    iRetValue = ALCenterCorrectBCOR();
        	    break;
       	    case 7:
                if(iFlag == BASQ)
                    iRetValue = ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,++iBalStep);
				else if(iFlag == BALI)
					iRetValue = ALCalcSinglePick();
                else if(iFlag == BCOR || iDisplayMode & AL_DISPAR_Z)
                    iRetValue = ALNotchFromCenter();
                else
                    iRetValue = ALPositionNotch();
        	    break;
     	    case 8:
                if (iFlag == BALI)
                    iRetValue = ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,++iBalStep);
                else
                    iRetValue = ALCorrectNotchPos();
        	    break;
     	    case 9:
                iRetValue = ALEnd();
                if (iRetValue == FAILURE)
                    break;
                iRetValue = ALSetIntVars(AL_ALIGNING_FLAG,AL_NOIDX,FALSE);
                if (iRetValue == FAILURE)
                    break;
				if((iRetValue=CMDReadAlignStatus(Optr->next))== FAILURE)
					break;
                TKSetPCCriticalSection(-1);
                return SUCCESS;
     	    default:
                ALAbort();
                SSSetStatusWord(ALIGNING, TRUE);
                ISSetJmpBufMark(8, 0);
                if((iFlag == BAL) || (iFlag == BASQ) || (iFlag == ZZ2) || iFlag == BALI)
                    TKSetPCCriticalSection(-1);
                return FAILURE;
        }   /* switch (iBalStep) */
    }   /* if (iRetValue == 0) after iRetValue = setjmp... */
    else
    {
        iRetValue = 0;
        if( ALGetIntVars(AL_NUM_CORRECTIONS,AL_NOIDX,&iNumberOfCorrections) == FAILURE )
            iRetValue = FAILURE;
        else if( ALSetIntVars(AL_NUM_CORRECTIONS,AL_NOIDX,iNumberOfCorrections++) == FAILURE )
            iRetValue = FAILURE;
        else if( ALSetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,1) == FAILURE )
            iRetValue = FAILURE;
        else
        {
            ALStopMeasurement();
            if( ALGetIntVars(AL_TRACE,AL_NOIDX,&iTrace) == FAILURE )
                iRetValue = FAILURE;
            if(iFpError && iDisplayMode)
            {
                sprintf(caTempString, "\r\nFPE_CODE = %04X\r\nTRACE = %d", iFpError, iTrace );
                if( SERPutsTxBuff(iCmdPortNumber, caTempString) == FAILURE )
                    iRetValue = FAILURE;
                iFpError = 0;
            }
        }
    }   /* if (iRetValue == 0)'s else */

    if(iDisplayMode & AL_DISPAR_S)
    {
        if( ALGetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,&iBalStep) == FAILURE )
            iBalStep = 100;
        sprintf(caTempString," \r\n   last step %3d  return %2d\r\n", iBalStep - 1, iRetValue);
        if( SERPutsTxBuff(iCmdPortNumber, caTempString) == FAILURE )
            iRetValue = FAILURE;
    }

    if(iRetValue == FAILURE)
    {
        ALAbort();
        SSSetStatusWord(ALIGNING, TRUE);
        if(iDisplayMode & AL_DISPAR_S)
        {
            sprintf(caTempString, " \r\n   bal_step reset\r\n");
            SERPutsTxBuff(iCmdPortNumber, caTempString);
        }
        if(iDisplayMode & AL_STATUS_FAIL)
        {
            if( ALGetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,&iBalStep) == FAILURE )
                iBalStep = 99;
            sprintf(caTempString," \r\n Aligner Fail Status: iBalStep=%d\r\n", iBalStep);
            SERPutsTxBuff(iCmdPortNumber, caTempString);
            if( ALGetIntVars(AL_TRACE,AL_NOIDX,&iTrace) == FAILURE )
                iTrace = 99;
            sprintf(caTempString, "\r\n     iFpError = %d     iTrace = %d\r\n", iFpError, iTrace );
            SERPutsTxBuff(iCmdPortNumber, caTempString);
            if( ALGetIntVars(AL_ALIGNMENT_STATUS,AL_NOIDX,&iAlignmentStatus) == FAILURE )
		        iAlignmentStatus = 99;
            if( ALGetIntVars(AL_NUM_CORRECTIONS,AL_NOIDX,&iNumberOfCorrections) == FAILURE )
		        iNumberOfCorrections = 99;
            sprintf(caTempString, "\r\n     iAlnStat = %d     iNumCorr = %d\r\n", iAlignmentStatus, iNumberOfCorrections );
            SERPutsTxBuff(iCmdPortNumber, caTempString);
        }
		/* either save the aligner status to register or send to com-port. */
		CMDReadAlignStatus(Optr->next);
//        ISSetJmpBufMark(8, 0);
        if((iFlag == BAL) || (iFlag == BASQ) || (iFlag == ZZ2) || (iFlag == BALI))
            TKSetPCCriticalSection(-1);
        return SUCCESS;
    }
    else
    {
        ALGetIntVars(AL_ALIGNMENT_STEP,AL_NOIDX,&iBalStep);
// FIX ME: for anna, I think I need to think about this code......
        if( (MRGetPC() || (iFlag == BCOR)) && !( (instr->nofopr == 3) && ((long)iBalStep == (lSingleBalStep+1)) ) )
            MASetPC(instr);     /* PC == NULL when STOP'ed */
        else
        {
            /* Should abort any alignment as the check_motion_error() has already abort_macro() & kill all tasks */
            ALAbort();
            SSSetStatusWord(ALIGNING, TRUE);
        }
    }
    ISSetJmpBufMark(8, 0);
    if(iFlag == BAL)
        TKSetPCCriticalSection(-1);
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   CMDReadParam
 * Descrition:      Common code routine for commands reading wafer parameter values.
 * Parameter:       intstr - instruction pointer
 *                  lParameter - parameter value to be read
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDReadParam(CMDoperand_ptr OperandArg, long lParameterArg)
{
    int iCmdPortNumber;
    char caString[10];

    iCmdPortNumber = SERGetCmdPort();

	if(OperandArg)
	{
		if(CMDSetIndirectReg(OperandArg, lParameterArg)==FAILURE)
			return FAILURE;
    }
    else
    {
//        ltoa(lParameterArg, caString, 10);
	sprintf(caString, "%d", (int)lParameterArg);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   CMDSetParam3
 * Descrition:      Reads from the command line or registers either one or all
 *                  3 values and sets the passed array to them
 *                  if only one value want to be set, the rest two values are untouched
 * Parameter:       instr - instruction pointer
 *                  laParametersArg - aray of 3 values to be changed
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDSetParam3(instr_ptr instr, long laParametersArg[])
{
    ULONG ulAxis;
    int iCounter;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;

    ulAxis = CMDgetAxis(instr);
    if(ulAxis < RO_AXIS_ALL)
        return FAILURE;

    if((ulAxis == RO_AXIS_ALL) || (ulAxis == RO_AXIS_all))
    {
        for(iCounter = 0; iCounter < 3; iCounter++)
        {
            Optr = Optr->next;
            if(CMDgetValue(Optr, &laParametersArg[iCounter]) == FAILURE)
                return FAILURE;
        }
    }
    else
    {
        Optr = Optr->next;
        if(ulAxis == RO_AXIS_t)
        {
            if(CMDgetValue(Optr, &laParametersArg[0]) == FAILURE)
                return FAILURE;
        }
        else if(ulAxis == RO_AXIS_r)
        {
            if(CMDgetValue(Optr, &laParametersArg[1]) == FAILURE)
                return FAILURE;
        }
        else if(ulAxis == RO_AXIS_z)
        {
            if(CMDgetValue(Optr, &laParametersArg[2]) == FAILURE)
                return FAILURE;
        }
        else if(ulAxis == RO_AXIS_W)
        {
            if(CMDgetValue(Optr, &laParametersArg[0]) == FAILURE)
                return FAILURE;
        }
    }

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   CMDReadAlignStatus
 * Descrition:      Reads the alignment status from the aligner module
 *                  and sets the passed register to it or displays it
 *                  to the screen
 * Parameter:       intstr - instruction pointer
 *                  OptrArg - last operand on the command line
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDReadAlignStatus(CMDoperand_ptr OptrArg)
{
	int iAlignmentStatus;
	char caTempString[80];
	//int iCmdPortNumber;

	caTempString[0] = 0;
    if( ALGetIntVars(AL_ALIGNMENT_STATUS,AL_NOIDX,&iAlignmentStatus) == FAILURE )
		return FAILURE;

	if(CMDSetOrShowStatus(OptrArg, iAlignmentStatus)==FAILURE)
		return FAILURE;
	return SUCCESS;
}
int ex_CDIS(instr_ptr instr)
{
	CMDoperand_ptr Optr;
    char caBuf[40];
    long lThetaAngle, lVectorOffset;
    double dDistance, dAngle;
	//int iCmdPort;

    caBuf[0] = 0;

    Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lThetaAngle)==FAILURE)
		return FAILURE;
	Optr = Optr->next;
	if(CMDgetValue(Optr, &lVectorOffset)==FAILURE)
		return FAILURE;
	/* value from the user is in 100 of degree, so need to
	 * convert to degree. */
    dAngle = (double)lThetaAngle/100.0;
	/* convert to radian */
    dAngle = dAngle * DEG_TO_RAD;
	/* find the distance between the center of robot and center of chuck */
    dDistance = (double)lVectorOffset / sin(dAngle);
	/* round off the number */
    dDistance += (dDistance > 0) ? 0.5 : -0.5;
    Optr = Optr->next;
	if(CMDReadParam(Optr, (long)dDistance)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
/* read robot radial negative maximum offset */
int ex_RRNMO(instr_ptr instr)
{
	return CMDReadTRCalibrationVals(instr, R_NEG_MAX_OFFSET);
}
/* read robot radial positive maximum offset */
int ex_RRPMO(instr_ptr instr)
{
	return CMDReadTRCalibrationVals(instr, R_POS_MAX_OFFSET);
}
/* read robot theta negatvie maximum offset */
int ex_RTNMO(instr_ptr instr)
{
	return CMDReadTRCalibrationVals(instr, T_NEG_MAX_OFFSET);
}
/* read robot theta positive maximum offset */
int ex_RTPMO(instr_ptr instr)
{
	return CMDReadTRCalibrationVals(instr, T_POS_MAX_OFFSET);
}
/* set robot radial negative maximum offset */
int ex_SRNMO(instr_ptr instr)
{
	return CMDSetTROffsetValue(instr, R_NEG_MAX_OFFSET);
}
/* set robot radial positive maximum offset */
int ex_SRPMO(instr_ptr instr)
{
	return CMDSetTROffsetValue(instr, R_POS_MAX_OFFSET);
}
/* set robot theta negative maximum offset */
int ex_STNMO(instr_ptr instr)
{
	return CMDSetTROffsetValue(instr, T_NEG_MAX_OFFSET);
}
/* set robot theta positive maximum offset */
int ex_STPMO(instr_ptr instr)
{
	return CMDSetTROffsetValue(instr, T_POS_MAX_OFFSET);
}
/***************************************************************************
 * Function Name:   ex_BAL
 * Description:     Begin Alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BALI(instr_ptr instr)
{
    return CMDAlign(instr, BALI);
}
/***************************************************************************
 * Function Name:   ex_ZZ3
 * Description:     Displays result of calculation to output port
 *                  for intellipick with offset with new algorithm 11/27/98
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ZZ3(instr_ptr instr)
{
	CMDoperand_ptr Optr;
    char caBuf[40];
    long lRobotT, lRobotR, lPreT;
	int iCmdPort;

    caBuf[0] = 0;

	if(ALGetLongVars(AL_ROBOT_T_PICKUP_POS, AL_NOIDX, &lRobotT)==FAILURE)
		return FAILURE;
	if(ALGetLongVars(AL_ROBOT_R_PICKUP_POS, AL_NOIDX, &lRobotR)==FAILURE)
		return FAILURE;
	if(ALGetLongVars(AL_CHUCK_FINAL_ROTATE_POS, AL_NOIDX, &lPreT)==FAILURE)
		return FAILURE;

    Optr = instr->opr_ptr;
    if(!Optr)
    {
		iCmdPort = SERGetCmdPort();
        sprintf(caBuf, "%ld, %ld, %ld\n", lRobotT, lRobotR, lPreT);
        SERPutsTxBuff(iCmdPort, caBuf);
    }
    else
    {
        if(instr->nofopr != 3)
            return FAILURE;
		if(CMDSetIndirectReg(Optr, lRobotT)==FAILURE)
			return FAILURE;

		Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, lRobotR)==FAILURE)
			return FAILURE;

		Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, lPreT)==FAILURE)
			return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function name: ex_RCVAC
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCVAC(instr_ptr instr)
{
	return CMDReadWaferValue(instr->opr_ptr, CHUCK_VAC_USE);
}
/***************************************************************************
 * Function name: ex_SCVAC
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCVAC(instr_ptr instr)
{
	return CMDSetWaferValue(instr->opr_ptr, CHUCK_VAC_USE);
}
int CMDReadWaferValue(CMDoperand_ptr OperandArg, int iTypeArg)
{
    long lValue;
	//int iCmdPort;

    if( FIOGetParamWaferVals(iTypeArg,&lValue) != SUCCESS )
        return FAILURE;
	if(CMDReadParam(OperandArg, lValue)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
int CMDSetWaferValue(CMDoperand_ptr OperandArg, int iTypeArg)
{
	long lValue;

	if(CMDgetValue(OperandArg, &lValue)==FAILURE)
		return FAILURE;
    if( FIOSetParamWaferVals(iTypeArg,lValue) == FAILURE )
        return FAILURE;
    return SUCCESS;
}
int CMDReadTROffsetValue(CMDoperand_ptr OperandArg, int iTypeArg, long lWhichArmArg)
{
    long lValue;
	//int iCmdPort;

	if(lWhichArmArg != 0 && lWhichArmArg != 1)
		return FAILURE;
    if( FIOGetTROffset(iTypeArg, &lValue, lWhichArmArg) != SUCCESS )
        return FAILURE;
	if(CMDReadParam(OperandArg, lValue)==FAILURE)
		return FAILURE;
    return SUCCESS;
}
int CMDSetTROffsetValue(instr_ptr instr, int iTypeArg)
{
	long lWhichArm = 0;
	long lValue;
	CMDoperand_ptr Optr;

	Optr = instr->opr_ptr;
	if(CMDgetValue(Optr, &lValue)==FAILURE)
		return FAILURE;
	if(instr->nofopr == 2)
	{
		if(CMDgetValue(Optr->next, &lWhichArm)==FAILURE)
			return FAILURE;
	}
    if( FIOSetTROffset(iTypeArg, lValue, lWhichArm) == FAILURE )
        return FAILURE;
	return SUCCESS;
}
int CMDReadTRCalibrationVals(instr_ptr instr, int iTypeArg)
{
    long lValue = 0;
	CMDoperand_ptr Optr;

	Optr = instr->opr_ptr;
	if(instr->nofopr == 0)
		goto exit_point;

    // get flag value
    if(CMDgetValue(Optr, &lValue)==FAILURE)
        return FAILURE;
	Optr = Optr->next;

exit_point:
	return CMDReadTROffsetValue(Optr, iTypeArg, lValue);
}
/***************************************************************************
 * Function name: ex_RNFA
 * Description:     Read Noth/Flat Angle
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RNFA(instr_ptr instr)
{
    long lValue;
    //int iCmdPortNumber;

    lValue = ALReadNotchAngle();

	return CMDReadParam(instr->opr_ptr, lValue);
}

int ex_RCCD(instr_ptr instr)
{
	long lValue[2];
    char caString[15];
    int iCmdPortNumber;
    CMDoperand_ptr Optr;

	ALTakeMeasurement(&lValue[1], &lValue[0]);

    if(instr->nofopr == 0)
    {
	    iCmdPortNumber = SERGetCmdPort();
        sprintf(caString, "%ld\r\n", lValue[0]);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;
        sprintf(caString, "%ld\r\n", lValue[1]);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;
        return SUCCESS;
    }
	else if (instr->nofopr == 2)
	{
	    Optr = instr->opr_ptr;
		if(CMDSetIndirectReg(Optr, lValue[0])==FAILURE)
			return FAILURE;
	    Optr = Optr->next;
		if(CMDSetIndirectReg(Optr, lValue[1])==FAILURE)
			return FAILURE;
		return SUCCESS;
	}
	
	return FAILURE;
}
