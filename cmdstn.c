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
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdstn.c
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
#include "cmdstn.h"
#include "cmdsp.h"
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
#include "sctch.h"
#include "dg.h"
#include "scttr.h"
#include "mapio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
#include "scproc.h"
#include "secsg.h"


/***************************************************************************
 * Function Name:   ex_CLRC
 * Description:     Clear Coordinate.
 * Implemetation:   First get the customized home position from the galil by calling
 *                  MEGetCustomizedHome() from Mechanism module. If the there is station name specified,
 *                  then clear only that specific station otherwise clear all 52 stations. Clear coordinate by
 *                  setting the coordinates position to deafult values of customized home which are:
 *                      T = T from customized home.
 *                      R = R from customized home.
 *                      Z = Z from customized home.
 *                      Station retract position = R from customized home.
 * NOTE:            This function will get executed when the user typed CLRC command
 *                  at the command line. The function is actually called in MRExecuteInstructions().
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *                  Command Syntax: CLRC [station name]
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CLRC(instr_ptr instr)
{
    int iStationIndx, indx;
    long lCHome[4]={0,0,0,0};

    /* get customized home from  data file */
    if( ROGetParameter(TRUE, RO_AXIS_ALL, lCHome, CUSTOMIZED_HOME) != SUCCESS )
        return FAILURE;
    if( !instr->nofopr )               /* no parameters -> clear all */
    {
        for( indx=0; indx<52; indx++ )
        {
            /* set the station coordinates with customized home */
            if( FIOSetStnCoord(indx,0,lCHome[0]) != SUCCESS )        /* set T = T-customized home */
                return FAILURE;
            if( FIOSetStnCoord(indx,1,lCHome[1]) != SUCCESS )        /* set R = R-customized home */
                return FAILURE;
            if( FIOSetStnCoord(indx,2,lCHome[2]) != SUCCESS )        /* set Z = Z-customized home */
                return FAILURE;
            if( FIOSetStnVals(indx, RET_HOME, lCHome[1]) != SUCCESS )          /* set station retraction position to R-customized home */
                return FAILURE;
            if( FIOSetStnScanCoord(indx,0,lCHome[0]) != SUCCESS )    /* set scan T = T-customized home */
                return FAILURE;
            if( FIOSetStnScanCoord(indx,1,lCHome[1]) != SUCCESS )    /* set scan R = R-customized home */
                return FAILURE;
            if( FIOSetStnScanCoord(indx,2,lCHome[2]) != SUCCESS )    /* set scan Z = Z-customized home */
                return FAILURE;
        }
    }
    else                                /* clear specific station */
    {
        if( (iStationIndx=(int)CMDgetStnIndx(instr->opr_ptr)) == FAILURE )
            return FAILURE;
        if( FIOSetStnCoord(iStationIndx,0,lCHome[0]) != SUCCESS )        /* set T = T-customized home */
            return FAILURE;
        if( FIOSetStnCoord(iStationIndx,1,lCHome[1]) != SUCCESS )        /* set R = R-customized home  */
            return FAILURE;
        if( FIOSetStnCoord(iStationIndx,2,lCHome[2]) != SUCCESS )        /* set Z = Z-customized home */
            return FAILURE;
        if( FIOSetStnVals(iStationIndx, RET_HOME, lCHome[1]) != SUCCESS )          /* set station retraction position to R-customized home */
            return FAILURE;
        if( FIOSetStnScanCoord(iStationIndx,0,lCHome[0]) != SUCCESS )    /* set scan T = T-customized home */
            return FAILURE;
        if( FIOSetStnScanCoord(iStationIndx,1,lCHome[1]) != SUCCESS )    /* set scan R = R-customized home */
            return FAILURE;
        if( FIOSetStnScanCoord(iStationIndx,2,lCHome[2]) != SUCCESS )    /* set scan Z = Z-customized home */
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_INDEX
 * Description:     Set the Index value for a station.  This command is used when a move to
 *                  coordinate command is issued.  Index number specifies the position of Z-axis relative
 *                  to the taught coordinate Z-axis position.
 * Implementation:  First, call CMDgetStationParameters() which will get the station name and index.  The index
 *                  return in the first element in the array that passed as reference and the function will return
 *                  the station name.  Then call FIOSetStnZIndex() from datafile module to set the position of
 *                  Z-index.  The purpose is to use this position to move to the specific slot of cassette to
 *                  do a get or put in the macro.
 * RLG NOTE:        always Zindex
 * NOTE:            This function will get executed when the user typed INDEX command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: INDEX <STATION>, <SLOT>
 *                      - STATION:  name of station which is CHARACTER_CONST or INDIRECT_CHAR type.
 *                      - SLOT:     the slot number of the cassette to specify the Z-position relative to
 *                                  the taught station of Z-axis position. It can be LONG_CONSTR or
 *                                  INDIRECT_REG type.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_INDEX(instr_ptr instr)
{
    long lZindex[4];
    int iStation;

    /* get the parameters from instruction. We need to get the station name and index/slot */
    if( (iStation=CMDgetStationParameters(instr,lZindex)) == FAILURE )
        return FAILURE;
    /* set the index position into the ZIndex position in the datafile module. why the value that is needed to be
     * set is the first element in the array? because there is only value of the station name in the parameter
     * of any instruction. ex: INDEX A, 2 ==> there is only '2' after the station name and so it got storing into
     * the first element in the array in CMDgetStationParameters function. */
    return FIOSetStnVals( iStation, Z_INDEX, lZindex[0] );
}
/***************************************************************************
 * Function Name:   ex_PITCH
 * Description:     Set the Pitch value for a station.  Pitch is the distance of on Z-axis between two adjacent indices.
 *                  This command together with INDEX and MTCR or MTCS will determine position of Z-axis.
 *                  The Unit is in 0.0001".
 * Implementation:  Call CMDgetStationParameters() to get the station name and pitch value. CMDgetStationParameters will
 *                  get values from operands and return the station name and pitch value into first element in an array.
 *                  Then call FIOSetStnPitch() to set pitch value to specified station name.
 * NOTE:            This function will get executed when the user typed PITCH command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: PITCH <STATION>,<VALUE>
 *                      - STATION:  the name of station and it can be in INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - VALUE:    It is new value to set. It can be INDIRECT_REG or LONG_CONST type.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_PITCH(instr_ptr instr)
{
    long lvalue[4];
    int iStationIndex;

    /* get the station name and pitch value from instruction. */
    if( (iStationIndex=CMDgetStationParameters(instr,lvalue)) == FAILURE )
        return FAILURE;
    return FIOSetStnVals( iStationIndex, PITCH, lvalue[0] );  /* set station pitch */
}
/***************************************************************************
 * Function Name:   ex_RIND
 * Description:     Read an index value for a station that the last set by INDEX command.
 * Implementation:  First, check that there must be at least one operand for this command. So get the first
 *                  operand which is the station name, then use the station name to get the station index
 *                  by calling CMDgetStnIndx(). And use station index to get Z-index which is the value we
 *                  want to get for this command by calling FIOGetStnZindex(). Then call
 *                  CMDreadBackParams() to determine either storing the value into the return register if the
 *                  second operand existed, or sending the value to comm port.
 * NOTE:            This function will get executed when the user typed RIND command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RIND <STATION>, [STORED VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - STORED VALUE:     This is optional parameter. Only existence if you want
 *                                          to store the result.  It has to be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RIND(instr_ptr instr)
{
    return CMDReadStnProperty(instr->opr_ptr, Z_INDEX);
}
/***************************************************************************
 * Function Name:   ex_RCS
 * Description:     Read Coordinate setting
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  station coordinates (T,R,Z) by calling FIOGetStnCoord(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCS <STATION>, [VALUE1, VALUE2, VALUE3]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the T-axis position of specified station
 *                      - VALUE2:   Stores the R-axis position of specified station
 *                      - VALUE3:   Stores the Z-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCS(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lValue[4];
    ULONG   ulAxis = RO_AXIS_T | RO_AXIS_R | RO_AXIS_Z; /* three parm values to
                                                         * output by
                                                         * CMDreadBackParms ** by CKK */

    if( !(Optr=instr->opr_ptr) )
        return FAILURE;
    else if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )  /* get station index */
        return FAILURE;
    else if( FIOGetStnCoord(iStationIndex,0,&lValue[0])==FAILURE ||  /* get station coords. or T,R,Z*/
            FIOGetStnCoord(iStationIndex,1,&lValue[1])==FAILURE ||
            FIOGetStnCoord(iStationIndex,2,&lValue[2])==FAILURE )
        return FAILURE;

    /* in the case if there is 4 operands, so stores the values into operands inorder of :
     *  - second operand:   stores T-axis position
     *  - third operand:    stores R-axis position
     *  - fourth operand:   stores Z-axis position.
     *  If there is only one operand then the coordinate value will send to the comm port.*/
    Optr = Optr->next;
    return CMDreadBackParms( Optr, lValue, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RFC
 * Description:     Read Flipper Coordinates
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  Flipper station coordinates by calling FIOGetFprCoord(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RFC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RFC <STATION>, [VALUE1]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the W-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lValue[4];
    ULONG ulAxis;

    ulAxis = ROGetSpecialAxis(RO_FLIPPER);
    Optr=instr->opr_ptr;
    /* get station index */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )
        return FAILURE;
    /* get flipper coordinates */
    if( FIOGetStnVals(iStationIndex, FLP_COORD, &lValue[3]) == FAILURE)
        return FAILURE;

    Optr = Optr->next;
    return CMDreadBackParms( Optr, lValue, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RSRP
 * Description:     Read Safe R Position
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  Safe R Position by calling FIOGetSafeR(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RSRP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSRP <STATION>, [VALUE1]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the W-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSRP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lValue[4];
    ULONG ulAxis;

    ulAxis = ROGetSpecialAxis(RO_FLIPPER);
    Optr=instr->opr_ptr;
    /* get station index */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )
        return FAILURE;
    /* get flipper coordinates */
    if( FIOGetStnVals(iStationIndex, FLP_SAFE_R, &lValue[3]) == FAILURE)
        return FAILURE;

    Optr = Optr->next;
    return CMDreadBackParms( Optr, lValue, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RSCS
 * Description:     Read scanning coordinate settings of a station
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  station coordinates (T,R,Z) by calling FIOGetStnScanCoord(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RSCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSCS <STATION>, [VALUE1, VALUE2, VALUE3]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the T-axis position of specified station
 *                      - VALUE2:   Stores the R-axis position of specified station
 *                      - VALUE3:   Stores the Z-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSCS(instr_ptr instr )
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lValue[4];
    ULONG   ulAxis = RO_AXIS_T | RO_AXIS_R | RO_AXIS_Z; /* three parm values to be
                                                         * returned from
                                                         * CMDreadBackParms ** by CKK */

    Optr=instr->opr_ptr;
    /* get the station index/location  */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )
        return FAILURE;
    /* get the scanning coordinate (T,R,Z) */
    if( FIOGetStnScanCoord(iStationIndex,0,&lValue[0])==FAILURE ||
        FIOGetStnScanCoord(iStationIndex,1,&lValue[1])==FAILURE ||
        FIOGetStnScanCoord(iStationIndex,2,&lValue[2])==FAILURE )
            return FAILURE;

    /* in the case if there is 4 operands, so stores the values into operands inorder of :
     *  - second operand:   stores T-axis position
     *  - third operand:    stores R-axis position
     *  - fourth operand:   stores Z-axis position.
     *  If there is only one operand then the coordinate value will send to the comm port.*/
    Optr = Optr->next;
    return CMDreadBackParms( Optr, lValue, ulAxis );

}
/***************************************************************************
 * Function Name:   ex_RCSI
 * Descripton:      Read Coordinate Special Item of specified station.
 * Implementation:  First, get station name from the first operand and using station name to
 *                  get the station location by call CMDgetStnIndx(). Get the second operand which is
 *                  the item number to read the value from.  Get value from specified item (please look
 *                  at the code belove for more detail implementation information). Then determine the return
 *                  value either send to comm port or store into the return register.
 * NOTE:            This function will get executed when the user typed RCSI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCSI <STATION>, <INDEX>, [RETURN VALUE]
 *                      - STATION:      Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - INDEX:        specific item. It can be INDIRECT_REG or LONG_CONST.
 *                      - RETURN VALUE: Stores the return value and it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCSI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lValue[4];
    ULONG ulAxis = RO_AXIS_T; /* only one parm value to output by
                               * CMDreadBackParms ** by CKK */

    Optr=instr->opr_ptr;        /* get first operand which is the station name */
    /* use the station name to get the index or location of station in an array */
    iStationIndex=CMDgetStnIndx(Optr);
    if( iStationIndex == FAILURE )
        return FAILURE;

	Optr = Optr->next;
	if(CMDgetValue(Optr,&lValue[1])==FAILURE)
		return FAILURE;
    /* the item 0-6 is reverved for the scanner information */
    switch( lValue[1] )
    {
        case 0 :                        /* T-scanner offset */
            if( FIOGetStnVals(iStationIndex, START_ANGLE, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 1 :                        /* R-scanner offset */
            if( FIOGetStnVals(iStationIndex, END_ANGLE, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 2 :                        /* used by filtering algorithm */
            if( FIOGetStnVals(iStationIndex, CROSS_SLOT_LOW, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 3 :                        /* used by filtering algorithm */
            if( FIOGetStnVals(iStationIndex, CROSS_SLOT_HIGH, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 4 :                        /* average z-delta beween left and right pass.  Calculated durring scanning */
            if( FIOGetStnVals(iStationIndex, ACC_LOW, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 5 :                        /* position of the first wafer in the cassette. Calculated using TSCN command. */
            if( FIOGetStnVals(iStationIndex, ACC_HIGH, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 6 :                        /* double-slot threshold. */
            if( FIOGetStnVals(iStationIndex, AVE_THICK, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 7 :                        /* distance from taught position to actual position of first wafer. */
            if( FIOGetStnVals(iStationIndex, TAUGHT_POS_TO_WAFER, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 8 :                        /* cross slot threshold. */
            if( FIOGetStnVals(iStationIndex, CROSS_SLOT_THRESH, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        case 9 :                        /* track position. */
            if( FIOGetStnVals(iStationIndex, TRACK_POS, &lValue[0]) == FAILURE )
                return FAILURE;
            break;
        default:                        /* from item 10-15 is available for any usuage. */
            if( lValue[1]>=10 && lValue[1]<FIO_MAX_NUM_CSI )
            {
                if( FIOGetStnlscsi(iStationIndex, (int)(lValue[1]-10), &lValue[0]) == FAILURE )
                    return FAILURE;
                break;
            }
            else
                return FAILURE;
    }
    Optr = Optr->next;              /* get the third operand */
    /* the return value either stores to return register or sends to comm port.  */
    return CMDreadBackParms( Optr, lValue, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_ROF
 * Description:     Read offset value for the station
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnOffset() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed ROF command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ROF <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the offset value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ROF(instr_ptr instr)
{
	return CMDReadStnProperty(instr->opr_ptr, OFFSET);
}
/***************************************************************************
 * Function Name:   ex_RPI
 * Description:     Read Pitch Value for the station that set by PITCH command.
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnPitch() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RPI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RPI <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the pitch value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPI(instr_ptr instr)
{
	return CMDReadStnProperty(instr->opr_ptr, PITCH);
}
/***************************************************************************
 * Function Name:   ex_RRET
 * Description:     Read retract value for the station that set by SRET command
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnRHome() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RRET command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RRET <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the retract position value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RRET(instr_ptr instr)
{
	return CMDReadStnProperty(instr->opr_ptr, RET_HOME);
}
/***************************************************************************
 * Function Name:   ex_RSE
 * Description:     Read the Second end effector flag for the station. If there is the second arguement specified,
 *                  the result of the reading is stored in the numerical variable.  Together with OUT commnad, this
 *                  command chooses the right vacuum line if double paddle end-effect is used.
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnEEindex() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RSEE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSEE <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the flag value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSE(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex, iEndEffector;
    long lEETemp;
    ULONG   ulAxis = RO_AXIS_T; /* Only one parm value to output by
                                 * CMDreadBackParms ** by CKK */

    /* get first operand which is the station name */
    Optr = instr->opr_ptr;
    /* get station index */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )
        return FAILURE;

    /* get end effector flag */
    if( FIOGetStnEEindex(iStationIndex, &iEndEffector) == FAILURE )
        return FAILURE;

    Optr = Optr->next;
    lEETemp = (long)(iEndEffector);

    /* Next operand; it can be NULL. If it is null, then send the value to comm port; otherwise, set the value
     * into the register */
    return CMDreadBackParms( Optr, &lEETemp, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RST
 * Description:     Read the Stroke value for the station.
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnStroke() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RST command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RST <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the stroke value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RST(instr_ptr instr)
{
	return CMDReadStnProperty(instr->opr_ptr, STROKE);
}
/***************************************************************************
 * Function Name:   ex_SOF
 * Description:     Set the offset values for the station
 * Implementation:  Get offset of the cassette and station name from operand, then set the offset into
 *                  specified station name.
 * NOTE:            This function will get executed when the user typed SOF command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SOF <STATION>, <VALUE>
 *                      - STATION:  station name
 *                      - VALUE:    the offset value to be set.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SOF(instr_ptr instr)
{
    int iStationIndex;
    long lOffset[4];

    if( (iStationIndex=CMDgetStationParameters(instr,lOffset)) != FAILURE )  /* get offset */
        return FIOSetStnVals( iStationIndex, OFFSET, lOffset[0] );  /* set offset */
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SCSI
 * Description:     Set coordinate special item. For scanning, items 0-7 are used. Each station
 *                  has 16 items as properties to store value.
 * NOTE:            This function will get executed when the user typed SCSI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SCSI <STATION>, <ITEM>, <VALUE>
 *                      - STATION:  Station name
 *                      - ITEM:     item number or index (0..15)
 *                      - VALUE:    the value to be set.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 *  Set Position coordinates for a station
 ***************************************************************************/
int ex_SCSI(instr_ptr instr)
{
    long lCoordinates[4];
    int iStationIdx;

    if(instr->nofopr != 3)              /* there must be 3 parameters */
        return FAILURE;
    /* get station index and get item number and value to be set */
    iStationIdx = CMDgetStationParameters(instr,lCoordinates);
    if (iStationIdx == FAILURE)
        return FAILURE;

    /* if the item number is 0-7, then set the value to specific catagory for specific purpose.
     * Otherwise, items 8-15 can be used for any purpose. */
    switch (lCoordinates[0])
    {
        case 0:                         /* R-scanner offset */
            return FIOSetStnVals(iStationIdx, START_ANGLE, lCoordinates[1]);
        case 1:                         /* T-scanner offset */
            return FIOSetStnVals(iStationIdx, END_ANGLE, lCoordinates[1]);
        case 2:                         /* Used by filtering algorithm. Should not be changed. */
            return FIOSetStnVals(iStationIdx, CROSS_SLOT_LOW, lCoordinates[1]);
        case 3:                         /* Used by filtering algorithm. Should not be changed. */
            return FIOSetStnVals(iStationIdx, CROSS_SLOT_HIGH, lCoordinates[1]);
        case 4:                         /* Average Z-delta between left and right pass */
            return FIOSetStnVals(iStationIdx, ACC_LOW, lCoordinates[1]);
        case 5:                         /* position of first wafer in the cassette. Calculated by TSCN */
            return FIOSetStnVals(iStationIdx, ACC_HIGH, lCoordinates[1]);
        case 6:                         /* Double-slot threshold. */
            return FIOSetStnVals(iStationIdx, AVE_THICK, lCoordinates[1]);
        case 7:                         /* Distance from taught position to actual position of first wafer. */
            return FIOSetStnVals(iStationIdx, TAUGHT_POS_TO_WAFER, lCoordinates[1]);
        case 8:                         /* Cross slot threshold. */
            return FIOSetStnVals(iStationIdx, CROSS_SLOT_THRESH, lCoordinates[1]);
        case 9:                         /* Track position. */
            return FIOSetStnVals(iStationIdx, TRACK_POS, lCoordinates[1]);
        default:                        /* using for any purpose */
            if (lCoordinates[0] >= 10 && lCoordinates[0] < FIO_MAX_NUM_CSI)
                return FIOSetStnlscsi(iStationIdx, (int) (lCoordinates[0]-10), lCoordinates[1]);
            else
                return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SPO
 * Description:     Set position coordinates for a station. All 3 positions of T,R and Z must be
 *                  provided. Can't set one axis at a time.
 * NOTE:            This function will get executed when the user typed SPC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SPC <STATION>, <POS1>, <POS2>, <POS3>
 *                      - STATION:  Station name
 *                      - POS:      Position of each axis T,R, and Z to be set.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SPO(instr_ptr instr)
{
    long lPosCoord[4], lSoftwareNL[4], lSoftwarePL[4];
    int iCount, iStationIdx;

    /* get station index and position coordinates  */
    if( (iStationIdx=CMDgetStationParameters(instr,lPosCoord)) != FAILURE )
    {
        /* get software negative limit and positive limit to check for the
         * positions that you want to set for specified coordinate are not
         * out of negative or positive limit switch. Or else, this position can
         * not to reach by the robot since these position will be the taught postion
         * for specified station.*/
        if(( ROGetParameter(TRUE, RO_AXIS_ALL,lSoftwareNL, SOFT_NEG_LIMIT) == SUCCESS ) &&
            ( ROGetParameter(TRUE, RO_AXIS_ALL,lSoftwarePL, SOFT_POS_LIMIT) == SUCCESS ))
        {
            /* the coordinates must be equal or greater than negative limit and
             * coordinates must be equal or less than position limit.*/
            for( iCount=0; iCount<3; iCount++ )
            {
                if(( lPosCoord[iCount] < lSoftwareNL[iCount] ) ||
                    ( lPosCoord[iCount] > lSoftwarePL[iCount] ))
                    return FAILURE;
            }
            /* set station coord to the position coord  */
            for( iCount=0; iCount<3; iCount++ )
            {
                if( FIOSetStnCoord(iStationIdx,iCount,lPosCoord[iCount]) == FAILURE )
                    return FAILURE;
            }
            return SUCCESS;
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SFC
 * Description:     Set Flipper Coordinate for a station.
 * *NOTE:           This function will get executed when the user types SFC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SFC <STATION>, <POS>
 *                      - STATION:  Station name
 *                      - POS:      Position of Flipper axis W to be set.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SFC(instr_ptr instr)
{
    int  iStationIdx;
    long lFprCoord;
    long lSoftwareNL[4] = {0,0,0,0};
    long lSoftwarePL[4] = {0,0,0,0};

    /* get station index and coordinate */
    if( (iStationIdx=CMDgetStationParameters(instr,&lFprCoord)) != FAILURE )
    {
        /* get the negative and positive software limits */
        if ((ROGetParameter(TRUE, ROGetSpecialAxis(RO_FLIPPER), lSoftwareNL, SOFT_NEG_LIMIT) == SUCCESS) &&
            (ROGetParameter(TRUE, ROGetSpecialAxis(RO_FLIPPER), lSoftwarePL, SOFT_POS_LIMIT) == SUCCESS))
        {
            /* check that the current position is between the limits */
            if(( lFprCoord < lSoftwareNL[3] ) || ( lFprCoord > lSoftwarePL[3] ))
                return FAILURE;
            /* set the coordinate */
            if( FIOSetStnVals(iStationIdx, FLP_COORD, lFprCoord) == FAILURE )
                return FAILURE;
            return SUCCESS;
        }
        return FAILURE;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SSRP
 * Description:     Set Safe R Position
 * *NOTE:           This function will get executed when the user types SSRP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SSRP <STATION>, <POS>
 *                      - STATION:  Station name
 *                      - POS:      Position of R-Axis when Safe to Flip.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSRP(instr_ptr instr)
{
    int  iStationIdx;
    long lSafeR;
    long lSoftwareNL[4] = {0,0,0,0};
    long lSoftwarePL[4] = {0,0,0,0};

    /* get station index and coordinate */
    if( (iStationIdx=CMDgetStationParameters(instr,&lSafeR)) != FAILURE )
    {
        /* get the negative and positive software limits */
        if ((ROGetParameter(TRUE, RO_AXIS_R, lSoftwareNL, SOFT_NEG_LIMIT) == SUCCESS) &&
            (ROGetParameter(TRUE, RO_AXIS_R, lSoftwarePL, SOFT_POS_LIMIT) == SUCCESS))
        {
            /* check that the current position is between the limits */
            if(( lSafeR < lSoftwareNL[1] ) || ( lSafeR > lSoftwarePL[1] ))
                return FAILURE;
            /* set the coordinate */
            if( FIOSetStnVals(iStationIdx, FLP_SAFE_R, lSafeR) == FAILURE )
                return FAILURE;
            return SUCCESS;
        }
        return FAILURE;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SRET
 * Description:     Set Retract Value for Station by specified that station name and position
 *                  to be retract position. The position must be in the range of software negative and
 *                  position limit switch. If there is just 1 parameter specified, it must be a '*'
 *                  wild card which sets all the stations' retract positions to be same as the customized
 *                  home position.
 * NOTE:            This function will get executed when the user types the SRET command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SRET <STATION>, <VALUE>
 *                      - STATION:  Station name
 *                      - VALUE:    postion to set be set as station retract position.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SRET(instr_ptr instr)
{
    int iStationIdx;
    long lCustomizedHome[4], lSoftwareNL[4], lSoftwarePL[4];

    if( instr->nofopr == 1 )            /* 1 operand, must be '*' */
    {
        if( (instr->opr_ptr->type == CHAR_CONST) && (instr->opr_ptr->opr.i == 42) )
        {
            if( ROGetParameter(FALSE, RO_AXIS_ALL,lCustomizedHome, CUSTOMIZED_HOME) == SUCCESS )       /* get customized home */
            {
                /* set all stations' retract position to be same as customized home retract position */
                for( iStationIdx=0; iStationIdx<52; iStationIdx++ )
                {
                    if( FIOSetStnVals(iStationIdx, RET_HOME, lCustomizedHome[1]) == FAILURE )  /* set station Rhome  */
                        return FAILURE;
                }
                return SUCCESS;
            }
        }
    }
    else                                /* there are operands, get a specific value  */
    {
        /* get station index and customized home values */
        if( (instr->opr_ptr->type == CHAR_CONST) && (instr->opr_ptr->opr.i == 42) )
        {
            if( CMDgetValue(instr->opr_ptr->next,&lCustomizedHome[0]) == FAILURE )
                return FAILURE;
        }
        else
        {
            if( (iStationIdx=CMDgetStationParameters(instr,lCustomizedHome)) == FAILURE )
                return FAILURE;
        }
        /* get software negative limit   */
        if( ROGetParameter(TRUE, RO_AXIS_ALL,lSoftwareNL, SOFT_NEG_LIMIT) == SUCCESS )
        {
            /* get software negative limit */
            if( ROGetParameter(TRUE, RO_AXIS_ALL,lSoftwarePL, SOFT_POS_LIMIT) == SUCCESS )
            {
                /* if in the range of negative and positive limit */
                if(( lCustomizedHome[0] >= lSoftwareNL[1] ) && ( lCustomizedHome[0] <= lSoftwarePL[1] ))
                {
                    /* set station Rhome */
                    if( (instr->opr_ptr->type == CHAR_CONST) && (instr->opr_ptr->opr.i == 42) )
                    {
                        /* set all stations' retract position to be same as customized home retract position */
                        for( iStationIdx=0; iStationIdx<52; iStationIdx++ )
                        {
                            if( FIOSetStnVals(iStationIdx, RET_HOME, lCustomizedHome[0]) == FAILURE )  /* set station Rhome  */
                                return FAILURE;
                        }
                        return SUCCESS;
                    }
                    else if( FIOSetStnVals(iStationIdx, RET_HOME, lCustomizedHome[0]) == SUCCESS )     /* set station Rhome  */
                        return SUCCESS;
                }
            }
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SSE
 * Description:     Set Second End Effector Flag for the station
 * NOTE:            This function will get executed when the user typed SSEE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SSEE <STATION>, <FLAG>
 *                      - STATION:  Station name
 *                      - FLAG:     0/1 to determine which end-effect. O is for default end-effect.
 *                                  1 is for second end-effect or dual arm end-effector.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSE(instr_ptr instr)
{
    int iStationIdx;
    long lEndEffectorFlag[4];

   /* get the station name and end-effect flag */
    if( (iStationIdx=CMDgetStationParameters(instr,lEndEffectorFlag)) != FAILURE )
    {
        /* validate the end-effect flag and set the flag value into the specified station's structure */
        if( lEndEffectorFlag[0]==0 || lEndEffectorFlag[0]==1 )
            return FIOSetStnEEindex(iStationIdx,(int)lEndEffectorFlag[0]);
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SST
 * Description:     Set Stroke Value for the Station
 * NOTE:            This function will get executed when the user typed SST command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SST <STATION>, <VALUE>
 *                      - STATION:  Station name
 *                      - VALUE:    Stroke value to be set
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SST(instr_ptr instr)
{
    int iStationIdx;
    long lStrokeValue[4];

   /* get the station name and stroke value for setting, then set the value into
    * specified station's structure. */
    if( (iStationIdx=CMDgetStationParameters(instr,lStrokeValue)) != FAILURE )
        return FIOSetStnVals( iStationIdx, STROKE, lStrokeValue[0] );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SPSC
 * Description:     Set the scanning position coordinates for a station. All 3 position values of
 *                  T,R and Z need to provide at once.
 * NOTE:            This function will get executed when the user typed SPSC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SPSC <STATION>, <POS1>, <POS2>, <POS3>
 *                      - STATION:  Station name
 *                      - POS:      positions to be set as taught positions for scanning station.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SPSC(instr_ptr instr )
{
    long lPosScanCoord[4], lSoftwareNL[4], lSoftwarePL[4];
    int iCount, iStationIdx;

    /* get station name and values to be set as scanning coordinates for specified station */
    if( (iStationIdx=CMDgetStationParameters(instr,lPosScanCoord)) != FAILURE )
    {
        /* get software negative limit and positive limit to check for the
         * positions that you want to set for specified coordinate are not
         * out of negative or positive limit switch. Or else, this position can
         * not to reach by the robot since these position will be the taught postion
         * for specified station.*/
        if(( ROGetParameter(TRUE, RO_AXIS_ALL,lSoftwareNL, SOFT_NEG_LIMIT) == SUCCESS ) &&
            ( ROGetParameter(TRUE, RO_AXIS_ALL,lSoftwarePL, SOFT_POS_LIMIT) == SUCCESS ))
        {
            /* the coordinates must be equal or greater than negative limit and
             * coordinates must be equal or less than position limit.*/
            for( iCount=0; iCount<3; iCount++ )
            {
                if(( lPosScanCoord[iCount] < lSoftwareNL[iCount] ) ||
                    ( lPosScanCoord[iCount] > lSoftwarePL[iCount] ))
                    return FAILURE;
            }
            /* set station coord to the position coord  */
            for( iCount=0; iCount<3; iCount++ )
            {
                if( FIOSetStnScanCoord(iStationIdx,iCount,lPosScanCoord[iCount]) == FAILURE )
                    return FAILURE;
            }
            return SUCCESS;
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SSS
 * Description:     Set Slot Status for the station.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSS(instr_ptr instr )
{
    CMDoperand_ptr Optr;
    int iStation, iAction, iPassNumber, iSlotNumber;
    char cStation;
    long lValue;

	Optr = instr->opr_ptr;
	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
		return FAILURE;
    iStation = (int)cStation;

    if( MPGetExtStn(iStation,0) == 1 )  /* get scanner station info */
        return FAILURE;

	Optr = Optr->next;
    if(CMDgetIntegerValue(Optr, &iAction)==FAILURE)
		return FAILURE;

	Optr = Optr->next;
	if(CMDgetIntegerValue(Optr, &iPassNumber)==FAILURE)
		return FAILURE;

	Optr = Optr->next;
	if(CMDgetIntegerValue(Optr, &iSlotNumber)==FAILURE)
		return FAILURE;

	Optr = Optr->next;
	if(CMDgetValue(Optr, &lValue)==FAILURE)
        return FAILURE;

    /* parameters validated in the scanning module functions */
    switch( iAction )
    {
        case 1:                         /* set the R-Offset Value */
            if( MPSetScanParamROffset(iStation,iPassNumber,iSlotNumber,lValue) == FAILURE )
                return FAILURE;
            break;
        case 2:                         /* set the Z-Pos Value */
            if( MPSetScanParamZPos(iStation,iPassNumber,iSlotNumber,lValue) == FAILURE )
                return FAILURE;
            break;
        case 3:                         /* set the Loaded value */
            if( MPSetScanParamLoaded(iStation,iPassNumber,iSlotNumber,(int)lValue) == FAILURE )
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RSS
 * Description:     Read Slot Status for the station. This command reads status of the result
 *                  generated by FILTR command. The status of each slot as following:
 *                      - 0:    no wafer
 *                      - 1:    wafer present
 *                      - 2:    cross-slotted.
 *                      - 3:    scanner failed.
 *                      - 4:    double-slotted.
 * Implementation:  Get the station name and check to make sure the station exists and get the
 *                  scanner station information. Get the pass number and slot number. Then using all these
 *                  information to get the slot status. If there is an operand, then store the slot status
 *                  into register otherwise send to the comm port.
 * NOTE:            This function will get executed when the user typed RSS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSS <STATION>, <PASS>, <SLOT>,[REUTNR VALUE]
 *                      - STATION:      station name
 *                      - PASS:         pass number (0/1)
 *                      - SLOT:         slot number of cassette
 *                      - RETURN VALUE: optional parameter to store return value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSS(instr_ptr instr )
{
    CMDoperand_ptr Optr;
    int iStation, iLoaded;
    char cStation;
    long lPass, lIndex, lLoaded;
    int iNoData = 0;

	Optr = instr->opr_ptr;
	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
		return FAILURE;
	iStation = (int)cStation;

    if( MPGetExtStn(iStation,0) != 0 )  /* get scanner station info */
        iNoData = 1;

    Optr = Optr->next;
	if(CMDgetValue(Optr, &lPass)==FAILURE)
		return FAILURE;

    Optr = Optr->next;
	if(CMDgetValue(Optr, &lIndex)==FAILURE)
		return FAILURE;

    if (iNoData)
        iLoaded = -1;
    else
        MPGetScanParamLoaded(iStation, (int)lPass, (int)lIndex, &iLoaded);

    lLoaded = (long)iLoaded;
    return CMDreadBackParms(Optr->next, &lLoaded, 1);
}
