/***************************************************************\
 *
<<<<<<< HEAD
=======
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
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdmap.c
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
#include "cmdmap.h"
#include "cmdsp.h"
#include "scmac.h"
#include "scio.h"
#include "sctim.h"
#include "gag.h"
#include "scstat.h"
#include "ser.h"
<<<<<<< HEAD
#include "fiog.h"
=======
#include "fiog.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
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
#include "mapk.h"
#include "mapio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
#include "scproc.h"
#include "secsg.h"
#include "otf.h"


/***************************************************************************
 * Function Name:   ex_SLUX
 * Description:     Set the Lift Up XSlot threshold.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SLUX(instr_ptr instr)
{
    long lParameter;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    if(CMDgetValue(Optr, &lParameter) == FAILURE)
        return FAILURE;
    MPSetLiftUpXSlotDiffThreshold(lParameter);

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RLUX
 * Description:     Read the Lift Up XSlot Threshold.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RLUX(instr_ptr instr)
{
    long lParameter;
    int iCmdPortNumber;
    char caString[10];
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;

    iCmdPortNumber = SERGetCmdPort();
    lParameter = MPGetLiftUpXSlotDiffThreshold();

    if(Optr)
	{
        if(CMDSetIndirectReg(Optr, lParameter)==FAILURE)
			return FAILURE;
    }
    else
    {
<<<<<<< HEAD
//        ltoa(lParameter, caString, 10);
=======
//        ltoa(lParameter, caString, 10);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	sprintf(caString, "%ld", lParameter);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;
    }

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_QSCAN
 * Description:     Quits Scanning mode.  QSCAN returns controller to normal operating
 *                  mode.  This includes disabling high-speed position capturing that
 *                  can slow down controller response in normal mode.
 * NOTE:            This function will get executed when the user typed QSCAN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: QSCAN
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_QSCAN(instr_ptr instr )
{
    return ROQuitScan();                /* quit scanning mode */
}
/***************************************************************************
 * Function Name:   ex_SBS
 * Description:     Set Base Scan Position for the station
 * NOTE:            This function will get executed when the user typed RCT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCT
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SBS(instr_ptr instr )
{
    long lScanPosition[8];
    int iStationIndex;

    if( (iStationIndex=CMDgetStationParameters(instr,lScanPosition)) != FAILURE )
        return FIOSetStnScanCoord(iStationIndex,2,lScanPosition[0]);
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_PSCAN
 * Description:     Prepare scan station. PSCAN prepares controller for capturing data from scanning sensor.
 *                  It sets up interrupt vector for high-speed Z-axis position capturing. Z-axis position is
 *                  captured on every transition of the scanning sensor. If the command is successful, it
 *                  returns 0. Otherwise 1. PSCAN fails if the scanning sensor is already activated (detects wafer).
 * Implementation:  First, get the station name from the first operand. Then search for station if it exists in the
 *                  scanner station list. If it is not, then create the scanner station structure and add to
 *                  scanner station list. Then, get the pass number from the second operand and get the used value
 *                  from the third operand which is always be 1. Then perform scanning and store the result(0/1) into
 *                  the register which is the fourth operand.
 * NOTE:            This function will get executed when the user typed PSCAN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: PSCAN <STATION>, <PASS>, <Axis>, <RETURN VALUE>
 *                      - STATION:      station name
 *                      - PASS:         Pass number. When scanning the robot scans the cassette in two passes,
 *                                      up and down.  This parameter must be 0 for the up pass and 1 for the
 *                                      down pass.
 *                      - Axis:         0 & 1 -> robot Z axis
 *                                      5 -> indexer r axis
 *                                      6 -> indexer z axis
 *                                      7 -> track W (first galil card)
 *                                      8 -> track w (second galil card)
 *                                      Z -> robot Z axis
 *                                      r -> indexer r axis
 *                                      z -> indexer z axis
 *                                      W -> track W
 *                                      w -> track w
 *                      - RETURN VALUE: stores the return value after PSCAN executed.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_PSCAN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lPassNumber, lScOnWhichDev;
    int iStation, iRCode, iScanResults;
    char cStation, cRet;

	Optr = instr->opr_ptr;
	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
		return FAILURE;
	iStation = (int)cStation;
	Optr = Optr->next;
    if( (iRCode=MPGetExtStn(iStation,0)) != 1 )  /* get scanner station info */
    {
        if( iRCode )        /* Check return code on scanner info */
        {
            if( MPCreateAuxScan(iStation) )  /* malloc the station structure, set the pointer */
                return FAILURE;
        }
		if(CMDgetValue(Optr, &lPassNumber)==FAILURE)
			return FAILURE;

		Optr = Optr->next;
        switch (Optr->type)
        {
            case INDIRECT_REG:
            case LONG_CONST :
		        if(CMDgetValue(Optr, &lScOnWhichDev)==FAILURE)
			        return FAILURE;
                break;
            case INDIRECT_CHAR:
            case CHAR_CONST :
                if(CMDgetCharacter(Optr, &cRet) ==  FAILURE)
                    return FAILURE;
                // map the axis into device number
                switch (cRet)
                {
                    case 'Z':
                        lScOnWhichDev = MP_ON_ROBOT_VAL_0;
                        break;
                    case 'r':
                        lScOnWhichDev = MP_ON_VAC_IDX_Z1;
                        break;
                    case 'z':
                        lScOnWhichDev = MP_ON_VAC_IDX_Z2;
                        break;
                    case 'W':
                        lScOnWhichDev = MP_ON_TRACK_VAL_0;
                        break;
                    case 'w':
                        lScOnWhichDev = MP_ON_TRACK_VAL_1;
                        break;
                    default:
                        return FAILURE;
                }
        }

        iScanResults = MPDoScan( (int)lPassNumber, (int)lScOnWhichDev );  /* perform scan */

        if((Optr=Optr->next))
        {
            if(CMDSetIndirectReg(Optr, (long)iScanResults)==FAILURE)
                return FAILURE;
        }
        return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_FILTR
 * Description:     Invoke Filter Ring Algorithm for scan data.
 * Implementation:  Get the station name and use the station name to get the scanner station structure. Get the
 *                  pass number from the second operand.  This pass number determine which scanner filter algorithm to
 *                  run. Get the calibration flag from the third operand and get the iteration number from the
 *                  fourth operand. Then start scanning with/without the calibration option and number of iteration.
 * NOTE:            This function will get executed when the user typed FILTR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: FILTR <STATION>, <PASS>, <CALIB FLAG>, <ITERATION>
 *                      - STATION:      scanner station name
 *                      - PASS:         pass number (0/1)
 *                      - CALIB FLAG:   calibration flag for calibrating or not calibrating.  In TSCN macro will specify
 *                                      with 1 to do calibration. However, for MAP or DMAP macro will specify with 0 for
 *                                      not doing the calibration.
 *                      - ITERATION:    number of iteration/pass to move up and down while scanning.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FILTR(instr_ptr instr )
{
    int iStation, iRet;
    long lPassNumber, lMaxTH, lIter, lNewScheme=0L;
    CMDoperand_ptr Optr;
    char cStation, cCalStation;

    if( (Optr=instr->opr_ptr ) )            /* if operands */
    {
	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
            return FAILURE;
        iStation = (int)cStation;

	Optr = Optr->next;
	if(CMDgetValue(Optr, &lPassNumber)==FAILURE)
            return FAILURE;
        if( lPassNumber<0 || lPassNumber>5 )    /* Will be 0-5 when AMAT added */
            return FAILURE;

        /* In case we call for OTF filter we don't need to have scanning station structure
         * so we don't return failure */
        if ((MPGetExtStn(iStation,0) != SUCCESS)  && (lPassNumber < 4))    /* if not 0, failed to find the scanned station */
            return FAILURE;

        /* validate pass and check for more operands */
        if( lPassNumber!=1 && lPassNumber!=2 && (!(Optr=Optr->next)))
            return FAILURE;
        if( lPassNumber!=1 && lPassNumber!=2 && Optr )  /* if there is more operand */
        {
	    if(CMDgetValue(Optr, &lMaxTH)==FAILURE)
                return FAILURE;
            if( lPassNumber!=0 && (!(Optr=Optr->next)) )   /* if should be a 3rd operands and not  */
                return FAILURE;
            if( lPassNumber!=0 && Optr )
            {
		if(CMDgetValue(Optr, &lIter)==FAILURE)
                    return FAILURE;
                Optr = Optr->next;
                if(lPassNumber == 3 && Optr) /* Filter 3, New Indexer Mapping Scheme */
                {
                    if(CMDgetValue(Optr, &lNewScheme)==FAILURE)
                        return FAILURE;
                }
                if((lPassNumber == 4) || (lPassNumber == 5))
                {
                    if(CMDgetCharacter(Optr, &cCalStation)==FAILURE)
                        return FAILURE;
                }
            }
        }

        switch( lPassNumber )
        {
            case 0L :                   /* filter 0 */
                return MPFilterPass0( iStation, (int)lMaxTH );
            case 1L :                   /* filter 1 */
                return MPFilterPass1( iStation );
            case 2L :                   /* filter 2 */
                return MPFilterPass2( iStation, (int)lMaxTH );
            case 3L :                   /* filter 3 */
                return MPFilterPass3( iStation, (int)lMaxTH, (int)lIter, (int)lNewScheme );
            case 4L :                   /* filters 4 and 5 are for OTF aligning */
            case 5L :
<<<<<<< HEAD
                iRet = MPFilterPass4( iStation, (int)lMaxTH, lIter, cCalStation, (int)(lPassNumber - 4)*3 );
                Optr = Optr->next;
		if (Optr)
		{
		    RGSetRegister(Optr->opr.i, iRet);
		    return SUCCESS;
		}
=======
                iRet = MPFilterPass4( iStation, (int)lMaxTH, lIter, cCalStation, (int)(lPassNumber - 4)*3 );
                Optr = Optr->next;
		if (Optr)
		{
		    RGSetRegister(Optr->opr.i, iRet);
		    return SUCCESS;
		}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		return iRet;
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RBS
 * Description:     Read Base scan position for the station
 * NOTE:            This function will get executed when the user typed RBS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RBS(instr_ptr instr )
{
    int iStationIndex;
    long lStnCoord2[8];

    /* get the station index */
    if( (iStationIndex=CMDgetStnIndx(instr->opr_ptr)) != FAILURE )
    {
        /* get scanner coordinates */
        if( FIOGetStnScanCoord(iStationIndex, 2, &lStnCoord2[2]) != FAILURE )
            return CMDreadBackParms( instr->opr_ptr->next, lStnCoord2, RO_AXIS_Z );
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RROP
 * Description:     Read slot Radial Offset Position for scanner Station.
 * Implementation:  Get the option/station name from the first operand, then get the station information.
 *                  Get the pass (first or second pass) from the second operand and get slot number from
 *                  the third operand. Then Get the data from scanner data structure. If there is the
 *                  fourth operand then store the return data into the register; otherwise, send to the
 *                  comm port.
 * NOTE:            This function will get executed when the user typed RROP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RROP <OPTION>, <PASS>, <SLOT> ,[RETURN VALUE]
 *                      - OPTION:   option to get which type of data from scanner data.
 *                                  The options are:
 *                                  - A:    Wafer thickness
 *                                  - B:    Wafer position
 *                                  - C:    Pitch
 *                                  - D:    Z-pass data
 *                                  - E:    Transition data (up or down)
 *                                  - F:    Number of hits.
 *                      - PASS:     first or second pass (0 or 1)
 *                      - SLOT:     slot number at based 0.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RROP(instr_ptr instr )
{
    int iStation;
    long lPassNumber, lIndex, lValue;
    CMDoperand_ptr Optr;
    char cStation;

    Optr = instr->opr_ptr;              /* get 1st operand */
	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
		return FAILURE;
    iStation = cStation;

    if( MPGetExtStn(iStation,0) == 1 )  /* get scanner station info */
        return FAILURE;

    Optr = Optr->next;                  /* get 2nd operand which is the pass number*/
	if(CMDgetValue(Optr, &lPassNumber)==FAILURE)
        return FAILURE;
    if (lPassNumber >= NOFPASSES || lPassNumber < 0)
        return FAILURE;

    Optr = Optr->next;                  /* get 3rd operand which is the slot number*/
	if(CMDgetValue(Optr, &lIndex)==FAILURE)
        return FAILURE;

    /* read the raw data and send data to comm port */
    lValue = MPReadTempData( iStation,(int)lPassNumber,(int)lIndex);
    return CMDreadBackParms( Optr->next, &lValue, 1 );
}
/***************************************************************************
 * Function Name:   ex_LLOP
 * Description:     Load scan data into raw data buffer for testing.
 * Implementation:  First, get the index from first operand. Get pass1 from second operand. Get
 *                  transition1 from third operand. Get pass2 from fourth operand and get transition2 from
 *                  fifth operand. Then call the function MPLoadTempData() to download the data to the
 *                  scanner raw data structure in the controller.
 * NOTE:            This function will get executed when the user typed LLOP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: LLOP <INDEX>, <PASS1>, <TRANS1>, <PASS2>, <TRANS2>
 *                      - INDEX:    index to access specific location for raw data buffer.
 *                      - PASS1:    pass 1
 *                      - TRANS1:   transition 1
 *                      - PASS2:    pass 2
 *                      - TRANS2:   transition 2
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_LLOP(instr_ptr instr )
{
    long lPass1, lPass2, lTrans1, lTrans2, lIndex;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;              /* get 1st operand */
	if(CMDgetValue(Optr, &lIndex)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                  /* get 2nd operand */
	if(CMDgetValue(Optr, &lPass1)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                  /* get 3rd operand  */
	if(CMDgetValue(Optr, &lTrans1)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                  /* get 4th operand */
	if(CMDgetValue(Optr, &lPass2)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                  /* get 5th operand */
	if(CMDgetValue(Optr, &lTrans2)==FAILURE)
        return FAILURE;

    /* download the raw data to scanner data structure */
    return MPLoadTempData((int)lIndex, lPass1, (int)lTrans1, lPass2, (int)lTrans2);
}
/***************************************************************************
 * Function Name:   ex_RZP
 * Description:     Reads Z-axis position of wafer edge.
 * Implementation:  First, get station name from first operand and get scanner information by using the station name.
 *                  Then get edge position from the second operand and get the slot from the third number.  Get the
 *                  Z-position of scanner station at specified slot. If there is fourth operand, then set the Z-position
 *                  to register; otherwise, send to the comm port.
 * NOTE:            This function will get executed when the user typed RZP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RZP <STATION>, <POSITION>, <SLOT>, [RETURN VALUE]
 *                      - STATION:      Station name
 *                      - POSITION:     Edge position. 0 for bottom edge and 1 for top edge.
 *                      - SLOT:         Slot number (zero-based)
 *                      - RETURN VALUE: Optional parameter to store return value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RZP(instr_ptr instr)
{
    int iStation;
    char cStation;
    long lPassNumber, lIndex, lpZPos;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;              /* get station from 1st operand */
	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
		return FAILURE;
    iStation = cStation;

    if(MPGetExtStn(iStation,0) != 0)  /* get scanner station info */
        return FAILURE;

    Optr = Optr->next;                  /* get pass number from 2nd operand */
    if(CMDgetValue(Optr, &lPassNumber)==FAILURE)
        return FAILURE;

    Optr = Optr->next;                  /* get index/slot from 3rd operand */
    if(CMDgetValue(Optr, &lIndex)==FAILURE)
        return FAILURE;

    /* get the Z-axis position */
    if(MPGetScanParamZPos(iStation,(int)lPassNumber,(int)lIndex,&lpZPos) == SUCCESS)
        return CMDreadBackParms(Optr->next, &lpZPos, 1);
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_POTF
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_POTF(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iRet;

    Optr = instr->opr_ptr;
    iRet = MPPrepareOTFAligning();
    if (Optr)
    {
        if (RGSetRegister(Optr->opr.i, iRet) == FAILURE)
            return FAILURE;
    }
    return iRet;
}
/***************************************************************************
 * Function Name:   ex_QOTF
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_QOTF(instr_ptr instr)
{
    int iOTFBufferOverflow;
    CMDoperand_ptr Optr;
    int iRetVal = SUCCESS; //iCmdPort, 

    Optr = instr->opr_ptr;
    if (MPQuitOTFAligning(&iOTFBufferOverflow) == FAILURE)
        iRetVal = FAILURE;
    if (Optr)
    {
        if (RGSetRegister(Optr->opr.i, iOTFBufferOverflow) == FAILURE)
            iRetVal = FAILURE;
    }

    return iRetVal;
}
/***************************************************************************
 * Function Name:   ex_SOTFI
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SOTFI(instr_ptr instr)
{
    int iStationIndex;
    long laParameters[8]={0,0,0,0,0,0,0,0};
    int iNumOfOperands;

    iNumOfOperands = instr->nofopr;

    iStationIndex = CMDgetStationParameters(instr, laParameters);
    if (iStationIndex == FAILURE)
        return FAILURE;

    if(((laParameters[0] == 0) || (laParameters[0] == 6) || (laParameters[0] == 7)) && (iNumOfOperands != 4))
        return FAILURE;
    if((laParameters[0] == 9) && (iNumOfOperands != 5))
        return FAILURE;
    if(((laParameters[0] == 2) || (laParameters[0] == 3) || (laParameters[0] == 4) || (laParameters[0] == 5))
             && (iNumOfOperands != 3))
        return FAILURE;

    if (MPSetOTFItem((char)iStationIndex, laParameters) == FAILURE)
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_ROTFI
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ROTFI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lParameters[8]={0,0,0,0,0,0,0,0};
    //ULONG ulAxis = RO_AXIS_T;

    if (!(Optr = instr->opr_ptr))
        return FAILURE;
    iStationIndex = CMDgetStnIndx(Optr);
    if (iStationIndex == FAILURE)
        return FAILURE;

    if (!(Optr = Optr->next))
        return FAILURE;
    if(CMDgetValue(Optr, &lParameters[1])==FAILURE)
        return FAILURE;

    if ((lParameters[1] == 0) || (lParameters[1] == 6) || (lParameters[1] == 7))
    {
        if (!(Optr = Optr->next))            /* get the next operand, 2. dimension for CSI 0, 6, 7 */
            return FAILURE;
        if(CMDgetValue(Optr, &lParameters[2])==FAILURE)
            return FAILURE;
    }
    else if ((lParameters[1] == 8) || (lParameters[1] == 9))
    {
        if (!(Optr = Optr->next))            /* get the next operand, 2. dimension for CSI 8, 9 */
            return FAILURE;
        if(CMDgetValue(Optr, &lParameters[2])==FAILURE)
            return FAILURE;

        if (!(Optr = Optr->next))            /* get the next operand, 3. dimension for CSI 8, 9 */
            return FAILURE;
        if(CMDgetValue(Optr, &lParameters[3])==FAILURE)
            return FAILURE;
    }

    if (MPReadOTFItem((char) iStationIndex, lParameters) == FAILURE)
        return FAILURE;

    Optr = Optr->next;

    return CMDreadBackParms(Optr, lParameters, RO_AXIS_T);
}
/***************************************************************************
 * Function Name:   ex_ROTFD
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
<<<<<<< HEAD
extern int 	giOTFCounter;
=======
extern int 	giOTFCounter;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int ex_ROTFD(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lIndex, laXCoordinate[MAX_OTF_INTERRUPTS], laYCoordinate[MAX_OTF_INTERRUPTS];
    int iaSensorsBitMap[MAX_OTF_INTERRUPTS], iNumOfOTFInterrupts, iArrayIndex; //iCounter, 
    float faXCentersCoordinates[6], faYCentersCoordinates[6];
    char sBuf[100];

    if (!(Optr = instr->opr_ptr))
        return FAILURE;
    if(CMDgetValue(Optr, &lIndex) == FAILURE)
        return FAILURE;
    Optr = Optr->next;

    switch((int)lIndex)
    {
        case 1 :
//            MPGetNumberOfOTFInterrupts(&iNumOfOTFInterrupts);
            if(Optr)
            {
//                if(CMDSetIndirectReg(Optr, (long)iNumOfOTFInterrupts)==FAILURE)
                if(CMDSetIndirectReg(Optr, (long)giOTFCounter)==FAILURE)
                    return FAILURE;
            }
            else
            {
                sprintf(sBuf, "%i\r\n", giOTFCounter);
                if(SERPutsTxBuff(SERGetCmdPort(), sBuf) == FAILURE)
                    return FAILURE;
            }
            break;
        case 2 :
            if(!Optr)
                return FAILURE;
            if(CMDgetIntegerValue(Optr, &iArrayIndex)==FAILURE)
                return FAILURE;
            if((iArrayIndex < 0) || (iArrayIndex >= MAX_OTF_INTERRUPTS))
                return FAILURE;
            MPGetOTFSensorMap(iaSensorsBitMap);
            if((Optr = Optr->next))
            {
                if(CMDSetIndirectReg(Optr, (long)iaSensorsBitMap[iArrayIndex])==FAILURE)
                    return FAILURE;
            }
            else
            {
                sprintf(sBuf, "%i\r\n", iaSensorsBitMap[iArrayIndex]);
                if(SERPutsTxBuff(SERGetCmdPort(), sBuf) == FAILURE)
                    return FAILURE;
            }
            break;
        case 3 :
            if(!Optr)
                return FAILURE;
            if(CMDgetIntegerValue(Optr, &iArrayIndex)==FAILURE)
                return FAILURE;
            if(((iArrayIndex < 0) || (iArrayIndex >= MAX_OTF_INTERRUPTS)))
                return FAILURE;
            MPGetOTFPositions(laXCoordinate, laYCoordinate);
            if((Optr = Optr->next))
            {
                if(CMDSetIndirectReg(Optr, laXCoordinate[iArrayIndex])==FAILURE)
                    return FAILURE;
                if(!(Optr = Optr->next))
                    return FAILURE;
                if(CMDSetIndirectReg(Optr, laYCoordinate[iArrayIndex])==FAILURE)
                    return FAILURE;
            }
            else
            {
                sprintf(sBuf, "%li, %li\r\n", laXCoordinate[iArrayIndex], laYCoordinate[iArrayIndex]);
                if(SERPutsTxBuff(SERGetCmdPort(), sBuf) == FAILURE)
                    return FAILURE;
            }
            break;
        case 4 :
            if(!Optr)
                return FAILURE;
            if(CMDgetIntegerValue(Optr, &iArrayIndex)==FAILURE)
                return FAILURE;
            if(((iArrayIndex < 0) || (iArrayIndex > 5)))  // OTF algorithm always computes 6 average centers only
                return FAILURE;
            MPGetOTFCenters(faXCentersCoordinates, faYCentersCoordinates);
            if((Optr = Optr->next))
            {
                if(CMDSetIndirectReg(Optr, (long)faXCentersCoordinates[iArrayIndex])==FAILURE)
                    return FAILURE;
                if(!(Optr = Optr->next))
                    return FAILURE;
                if(CMDSetIndirectReg(Optr, (long)faYCentersCoordinates[iArrayIndex])==FAILURE)
                    return FAILURE;
            }
            else
            {
                sprintf(sBuf, "%f, %f\r\n", faXCentersCoordinates[iArrayIndex], faYCentersCoordinates[iArrayIndex]);
                if(SERPutsTxBuff(SERGetCmdPort(), sBuf) == FAILURE)
                    return FAILURE;
            }
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SOTFD
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SOTFD(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lValue;
    int iIndex, iArrayIndex;

    if (!(Optr = instr->opr_ptr))
        return FAILURE;
    if(CMDgetIntegerValue(Optr, &iIndex)==FAILURE)
        return FAILURE;

    if(!(Optr = Optr->next))
        return FAILURE;
    if(CMDgetIntegerValue(Optr, &iArrayIndex)==FAILURE)
        return FAILURE;
    if((iArrayIndex < 0) || (iArrayIndex >= MAX_OTF_INTERRUPTS))
        return FAILURE;

    if(!(Optr = Optr->next))
        return FAILURE;
    if(CMDgetValue(Optr, &lValue) == FAILURE)
        return FAILURE;

    switch(iIndex)
    {
        case 2 :
            if(MPSetOTFSensorMap(iArrayIndex, (int)lValue) == FAILURE)
                return FAILURE;
            break;
        case 3 :
            if(MPSetOTFPositions(iArrayIndex, (int)lValue) == FAILURE)
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}
