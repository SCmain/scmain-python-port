/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * File Name            : cmdfn2.c
 * Description          : Functions which act as an interface for the LowLevel
 *              commands. These functions are local to the low-level command module.
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
#include <sys/io.h>

#include "cmdfns.h"
#include "cmdga.h"
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
#include "roga.h"
#include "roloc.h"
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
#include "alstep.h"
#include "fio.h"

extern stVectorParameter sVectorParameter;

extern int giEncoderDriftDetector;
extern int giEncoderAlarm;
extern int giTOTFlag;
extern int giDisableLatch;
extern long glPickCounter;
extern long glIdleTime;
extern long glMaxWaferPickCounter;
extern int giFrictionTestAxis;
extern int giFrictionTest;
extern int glFrictionDataReady;
extern int giFrictionTestAxis;

extern long glMotorResolution[8];

int  giRCNeedUpload = 0;
int  giRCActive = 0;


long PDConvAxis(ULONG ulAxisArg);

int FIODownGalilMac();

int ex_GMAC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;

    Optr=instr->opr_ptr;
    if( CMDgetValue(Optr,&lVal) == SUCCESS )  /* 2nd operand, get the direction  */
    {
	if (lVal == 313) return FIODownGalilMac();
    }

    return FAILURE;
}

/***************************************************************************
 * Function Name:   ex_SVOF
 * Description:     Turns the servo off (send motor the off command). If no parameter specified, then
 *                  turn off the servo of all the axes in the whole system. Otherwise, turn off the servo
 *                  of the specified axis.
 * NOTE:            This function will get executed when the user typed SVOF command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SVOF [AXIS]
 *                      - AXIS:     specify the axis to turn off the servo. If the no axis specify,
 *                                  turn off the servo of all the axes in the whole system.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SVOF(instr_ptr instr)
{
    ULONG ulAxis;

    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )  /* if can get axis from operand */
        return ROServoMCOff(ulAxis);    /* turn motor off */
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SVON
 * Description:     Turn the servo on for specified axis. If no parameter specified, then turn on
 *                  the servo of all axes in the whole system. Otherwise, turn on the servo of the
 *                  specified axis.
 * NOTE:            This function will get executed when the user typed SVON command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SVON [AXIS]
 *                      - AXIS:     specify the axis to turn on the servo. If the no axis specify,
 *                                  turn on the servo of all the axes in the whole system.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SVON(instr_ptr instr)
{
    ULONG ulAxis;

    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )  /* if can get axis from operand */
    {
        return ROServoMCOn(ulAxis);     /* turn servo on */
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_WMO
 * Description:     Wait for motion to complete on specified axis. If the is not axis specified,
 *                  WMO will wait for completion for all 3 axis (T,R,Z).  For system with TOT supported and enabled,
 *                  WMO will wait for motion to reach the in-position-window (IPW) instead of to full completion.
 * Implementation:  First get the axis and if no operand, then the axis is for all robot axes (T,R,Z). Then check for
 *                  motion to complete. If not complete then execute the same instruction to wait for motion to complete.
 *                  For all axis, won't work for TOT and only for specific axis.  In the case, there is specific axis specified,
 *                  then check if there is a second operand. If there is second operand, then get the flag value and validate
 *                  the flag to make sure it is a 1. A 1 is meant to wait for motion to reach the in-position-window instead
 *                  of waiting for full completion before moving another axis. if a 1 is specified, it is for TOT.
 * NOTE:            This function will get executed when the user typed WMO command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: WMO [AXIS [,1]]
 *                      - AXIS:     axis to wait for motion to complete.
 *                      - 1:        a flag to enable TOT. This flag is valid only for system with TOT supported.
 *                                  Also, can only do with specific axis.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WMO(instr_ptr instr)
{
    ULONG ulAxis;       /* first operand */
    int iTOTFlag = 0;   /* second operand */
    int iMotion = 0;    /* flag used in ROIsAxisMotionInProgress */
    int iOnOffFlag; //i, iIndex, 
    CMDoperand_ptr Optr;
    //unsigned long ulRobotAllAxis, ulPreAllAxis;
    //long lTAFlags[4] = {0,0,0,0};

    /* get axis */
    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )
    {
        /* check the second operand */
        /* If no second operand, just proceed */
        if ((Optr = instr->opr_ptr->next))
        {
            /* Get TOT feature OnOff flag */
//            if (FIOGetCfgFeatureFlags(TOT, &iOnOffFlag) == FAILURE)
//                return FAILURE;
            /* Get the second operand, and it must be 0 or 1.*/
	    if(CMDgetIntegerValue(Optr, &iTOTFlag)==FAILURE)
                return FAILURE;
            /* if TOT not turned on, cannot have 1 as 2nd parameter */
//            if(iOnOffFlag != 1 && iTOTFlag == 1)
//                return FAILURE;
	    if(iTOTFlag != 1 && iTOTFlag != 0)
		return FAILURE;
//	    giTOTFlag = iTOTFlag; // set it to kick off TP update
	
        }

        /* wait for motion to complete. if iMotion is return with a 1, then the
         * motion is not completed. So re-execute the same instruction again, to wait
         * until the motion is completed. */
        if( ROIsAxisMotionInProgress(ulAxis, iTOTFlag, &iMotion) == SUCCESS )
        {
            if( iMotion )
            {
//if(ulAxis == 0x01)
//printf("WMO keep PC. axis=%d\n",ulAxis);
                MASetPC(instr);         /* re-execute the instruction to wait until the motion is completed. */
            }
//if(ulAxis == 0x01)
//printf("WMO return SUCCESS. axis=%d\n",ulAxis);
            return SUCCESS;
        }
    }
//printf("ex_WMO return FAILURE..... axis=%d tot=%d iKeep=%d\n",ulAxis, iTOTFlag, iMotion);
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_ABM
 * Description:     Abort Motion.  Stops motion on all axes if axes are moving and also stops macro execution.
 *                  ABM command will kill servo on all axes.
 * NOTE:            This function will get executed when the user typed ABM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ABM
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ABM(instr_ptr instr)
{
    if(ROAbortMotion()==FAILURE)        /* stop the motion */
        return FAILURE;
    if(MRAbortMacro()==FAILURE)         /* stop the macro execution */
        return FAILURE;
//#ifdef SX
    S2AbortSecs();          /* abort secs communications in progress */
//#endif
    if( CMDinitFlags&DFPRE )
    {
        if(ALAbort()==FAILURE)              /* stop the alignment */
            return FAILURE;
    }
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_AMPI
 * Description:     Amplifier Initialization. This command enables the amplifiers by turning
 *                  on the enable signal. However, it does not turn on the servo. On the power up
 *                  controller enables amplifiers amplifiers through the I/O board.  The enable signal remains
 *                  enabled until the next power cycle or if emergency-off is activated. Thus, this command
 *                  should be issued only to recover from EMO.
 * NOTE:            This function will get executed when the user typed AMPI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: AMPI
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_AMPI( instr_ptr instr )
{
    return GAAmpInit();
}
/***************************************************************************
 * Function Name:   ex_RETH
 * Description:     Retract Arm.
 * Implementation:  If no operand specified, then retract the robot radial axis to customized home position
 *                  at current speed and acceleration.  If there is operand, then get it and check if it's a
 *                  station name (A-Z or a-z) or a number (0/1). If it's a station name, then retract the axis
 *                  denoted by the station's EE#. If it's a number, 0=R, 1=W, retract the axis to the customized
 *                  home position.
 *
 *                  Command Syntax: RETH [STATION] or [iAxisNumber]
 *                      - STATION:  it is optional parameter.  If this parameter is specified, then
 *                                  it specifies the station name for retracting to the station retract
 *                                  position that is set by SRET command.
 *                      - iAxisNumber 0=R, 1=W axis.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RETH(instr_ptr instr)
{
    int iStationIndex=FAILURE;
    long lRWAxis=0;
    long lHome;
    long lCHome[8];
    int iEEindex;
    int iRet;

    /* let's start by getting R-axis customized home position */
    /* This is the case where there's no operand, or 0 is specified. */
    if( FIOGetParamVals(ROBOTFILE, CUSTOMIZED_HOME, lCHome) == FAILURE )
        return FAILURE;
    lHome = lCHome[1];

    if( instr->opr_ptr )
    {
        /* what kind operand is it? a station or a number */
        iStationIndex = CMDgetStnIndx(instr->opr_ptr);
        /* it's not a station, it should be a number */
        if( iStationIndex == FAILURE)
        {
            /* get what axis is requested */
            if( CMDgetValue(instr->opr_ptr, &lRWAxis) == FAILURE)
                return FAILURE;
            /* W-axis requested */
            if( lRWAxis )
            {
                /* validate if we have the dual arm system */
	            if( ROGetSpecialAxis(RO_DUAL_ARM))
                {   /* then get its customized home position */
                    if( FIOGetParamVals(AUXFILE, CUSTOMIZED_HOME, lCHome) == FAILURE )
                        return FAILURE;
                    lHome = lCHome[0];
                }
                else
                {
                    /* no dual arm exists, the request is denied. */
                    return FAILURE;
                }
            }
        }
        else
        {
            /* it's a station */
            /* let's get its retract position */
            if( FIOGetStnVals(iStationIndex, RET_HOME, &lHome) == FAILURE )
                return FAILURE;
            if (FIOGetStnEEindex(iStationIndex, &iEEindex) == FAILURE)
                return FAILURE;
            if (iEEindex)
                lRWAxis = 1;
            else
                lRWAxis = 0;
        }
    }
    iRet = RORetractR(lHome, (int)lRWAxis); // retract the radial 

    return iRet;

}
/***************************************************************************
 * Function Name:   ex_STROK
 * Description:     Stroke Z up or down. This command will move Z-axis up(0) or down(1) by number specified by SST
 *                  command.
 * Implementation:  Get the station name from first operand and use the station name to get
 *                  station index.  Get the direction value from the second operand.  The
 *                  move Z-axis in direction that specified from second operand.
 * NOTE:            This function will get executed when the user typed STROK command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STROK <STATION>, <DIRECTION>
 *                      - STATION:      station name
 *                      - DIRECTION:    either up(0) or down(1)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STROK(instr_ptr instr)
{
    int iStationIndex, iStrokeDirection;
    long lVal;
    CMDoperand_ptr Optr;
    int iRet;

    /* get the first operand */
    Optr=instr->opr_ptr;
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )  /* 1st operand, get station index */
        return FAILURE;
    if( CMDgetValue(Optr->next,&lVal) == SUCCESS )  /* 2nd operand, get the direction  */
        iStrokeDirection = (lVal ? -1 : +1);

    iRet = ROMoveStroke( iStationIndex, iStrokeDirection );  /* move Z-axis up or down */

    return iRet;

}
/***************************************************************************
 * Function Name:   ex_HOME
 * Description:     Home to the specified Axis. Ihis command if no parameter is specifed , it will
 *                  perform home on all robot axes starting with Radial axis first.  if axis is
 *                  specified, it will perform home on that particular axis.
 *                  THIS COMMAND IS NOT AVALIABLE IN NOVELLUS SYSTEM.
 * NOTE:            This function will get executed when the user typed HOME command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: HOME [AXIS]
 *                      - AXIS:     Axis name to home.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_HOME(instr_ptr instr)
{
    ULONG ulAxis;
    int iRet;

    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )
    {
        iRet = ROHomeAxis(ulAxis, FALSE);      /* home the specified axis */

        return iRet;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_FE
 * Description:     This commands was created for the Stage controllers. Home to the specified Axis.
 *                  Ihis command if no parameter is specifed , it will
 *                  perform home on all robot axes starting with Radial axis first.  if axis is
 *                  specified, it will perform home on that particular axis.
 *                  THIS COMMAND IS NOT AVALIABLE IN NOVELLUS SYSTEM.
 * NOTE:            This command was created to accomodate homing for Stage controllers. Homing of Stage is process
 *                  similar to normal homing, except the part when motor searches the first index pulse is skipped.
 *                  So instead of Galil HM command we send FE to the Motion Card.
 *
 *                  Command Syntax: FE [AXIS]
 *                      - AXIS:     Axis name to home.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FE(instr_ptr instr)
{
    ULONG ulAxis;
    int iRet;

    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )
    {
        iRet = ROHomeAxis(ulAxis, TRUE);      /* home the specified axis with parameter indicating the Stage Controller*/
        return iRet;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_MOVA
 * Description:     Move absolute axis to position(s) specified. Either move a specific axis or move all 3 axes
 *                  if specified axis is A.
 * Implementation:  In this command, there must be at least 2 operands, otherwise return failure.  Get axis
 *                  from the first operand, and get the position from operand(s). Then move absolute to the
 *                  specified position(s) by calling ROMoveToAbs().
 * NOTE:            This function will get executed when the user typed MOVA command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MOVA <AXIS>, <POS1>,[POS2, POS3]
 *                      - AXIS:     the moving axis name
 *                      - POS(n):   position to move to.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MOVA(instr_ptr instr)
{
    ULONG ulAxis;
    long lPosition[8]={0,0,0,0,0,0,0,0};
    CMDoperand_ptr Optr=instr->opr_ptr;
    int iRet;

      /* get axis from first operand */
    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )
    {
        /* get value for specific axis  */
	if(CMDgetAxisValue(ulAxis, Optr->next, lPosition)==FAILURE)
	    return FAILURE;
        /* move absolute to the position specified. */
        iRet = ROMoveToAbs( ulAxis, lPosition );

        return iRet;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_MOVR
 * Description:     Move relative axis to the position(s) specified.
 * Implementation:  In this command, there must be at least 2 operands, otherwise return failure.  Get axis
 *                  from the first operand, and get the distance value from operand(s). Then move relative with
 *                  specified distance(s) by calling ROMoveToAbs().
 * NOTE:            This function will get executed when the user typed MOVR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MOVR <AXIS>, <DISTANCE1>, [DISTANCE2, DISTANCE3]
 *                      - AXIS:         the moving axis name
 *                      - DISTANCE(n):  distance to move.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MOVR(instr_ptr instr)
{
    ULONG ulAxis;
    long lPosition[8]={0,0,0,0,0,0,0,0};
    CMDoperand_ptr Optr=instr->opr_ptr;
    int iRet;

    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )  /* get axis */
    {
        /* if for all axes, get the distance values from operands */
        /* get the distance value for specified axis. */
	if(CMDgetAxisValue(ulAxis, Optr->next, lPosition)==FAILURE)
            return FAILURE;
        iRet = ROMoveToRel( ulAxis, lPosition );  /* move relative distance specified */

        return iRet;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_STOP
 * Description:     Stop motion the specified axis.  STOP will stop all the axis in the
 *                  whole system and stop the macro if there is no parameter specified on
 *                  axis and option flag.  If the optional flag is 1, then stop motion of specified
 *                  axis and stop the macro execution.  If the optional flag is 0, then only
 *                  stop the motion of the specified axis but the macro execution is still running if
 *                  there is a running macro.
 * NOTE:            This function will get executed when the user typed STOP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STOP <AXIS>, <FLAG>
 *                      - AXIS:     axis to stop the motion
 *                      - FLAG:     flag to determine either stop the macro execution or not.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STOP(instr_ptr instr)
{
    ULONG ulAxis;
    long lValue = TRUE;
    CMDoperand_ptr Optr = instr->opr_ptr;
	char cpCommand[8] = "ST";
	char cpBuf[20];

    if(instr->nofopr>=1)                /* can be only one operand */
    {
        if(!Optr)
            return FAILURE;
    }

    ulAxis = CMDgetAxis(instr);          /* get the axis */
    if( ulAxis != (unsigned long)FAILURE )
    {
        if( ROStopMotion(ulAxis) == SUCCESS )  /* stop the motion of specified axis */
        {
	    if (ulAxis == 0) // just to make sure threads are killed in Galil
		GASendReceiveGalil( 1, cpCommand, cpBuf );

            /* if there is second operand, then get the flag value */
            if(instr->nofopr>=2)
            {
                if(CMDgetValue(Optr->next,&lValue)==SUCCESS)
                {
                    if (lValue!=0 && lValue!=1)
                        return FAILURE;
                }
            }
            if (lValue)                 /* if flag value is 1, then abort macro execution */
            {
                MRAbortMacro();         /* abort any macro in process */
//#ifdef SX
                S2AbortSecs();          /* abort secs communications in progress */
//#endif
            }
            if( CMDinitFlags&DFPRE )
                ALAbort();              /* abort alignment process */

            return SUCCESS;
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_MATS
 * Description:     Move one axis only to a station. It works for T,R,Z,W,w axis.
 * Implementation:  First, get the station name from first operand, and use the station name to
 *                  get the station index.  Then set the current station to be the specified station
 *                  that got from first operand.  Get the axis from the second operand. If the axis is Z-axis
 *                  get the direction to move the Z-axis such as moving with Z-offset or Z-stroke.  Then
 *                  call ROMoveAxisDirect() to move the specified axis to the specified station.
 * NOTE:            This function will get executed when the user typed MATS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MATS <STATION>, <AXIS> [Z-OFFSET]
 *                      - STATION:  Station name
 *                      - AXIS:     axis (T, R, Z, W, w)
 *                      - Z-OFFSET: Z-axis offset flag. This optional parameter is in effect only
 *                                  when Z is specified. The flag value as following:
 *                                      - 0:    default (not moving with any offset)
 *                                      - 1:    uses Z offset.
 *                                      - 2:    uses Z-offset - Z-stroke.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MATS(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex;
    long lDir=0;
    char cAxis;
    ULONG ulAxis;
    int iRet;

    /* get the first operand */
    Optr=instr->opr_ptr;
    /* get the station nane from first operand and use station name to get station index. */
    iStationIndex = CMDgetStnIndx(Optr);
    if(iStationIndex == FAILURE)
        return FAILURE;
    /* set the current station */
    if(FIOSetCurrentStationInt(iStationIndex)==FAILURE)
        return FAILURE;

    /* get the second operand */
    Optr = Optr->next;
	if(CMDgetCharacter(Optr, &cAxis)==FAILURE)
		return FAILURE;
    switch( cAxis )
    {
        case 'T' :
            ulAxis = RO_AXIS_T;
            break;
        case 'R' :
            ulAxis = RO_AXIS_R;
            break;
        case 'W' :
            ulAxis = RO_AXIS_W;
            break;
        case 'w' :
            ulAxis = RO_AXIS_w;
            break;
        case 't' :
            ulAxis = RO_AXIS_t;
            break;
        case 'r' :
            ulAxis = RO_AXIS_r;
            break;
        case 'z' :
            ulAxis = RO_AXIS_z;
            break;
        case 'Z' :                      /* if Z axis, then get the direction  */
            ulAxis = RO_AXIS_Z;
            if( (Optr=Optr->next) )
            {
                if( CMDgetValue(Optr,&lDir) == FAILURE )
                    return FAILURE;
            }
            break;
        default:
            return FAILURE;
    }
    /* move the axis to specified station coordinate. */
    iRet = ROMoveAxisDirect( ulAxis, iStationIndex, (int)lDir );

    return iRet; 
}
/***************************************************************************
 * Function Name:   ex_MTCS
 * Description:     Move to station coordinates,  all axes simultaneously with an optional Z-offset.
 *                  If flag from the second parameter specified a 0, move without Z-offset. If flag from second
 *                  parameter specified a 1, move with Z-offset.
 * Implementation:  First, get the station name from first operand, and use the station name to
 *                  get the station index.  Then set the current station to be the specified station
 *                  that got from first operand.  Get the direction flag from the second operand if
 *                  there is the second operand. The direction value is the same as MTCR command. Then
 *                  call ROMoveDirect() to move all axes to the specified station simutaneously.
 * NOTE:            This function will get executed when the user typed MTCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MTCS <STATION>, <FLAG>
 *                      - STATION:  station name
 *                      - FLAG:     Optional flag for Z-offset (0,1,2)- same as MATS command
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MTCS(instr_ptr instr)
{
    int iStationIndex;
    CMDoperand_ptr Optr;
    long lDir=0;
    int iRet;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )  /* get the station index */
		return FAILURE;
    /* set the current station */
    if(FIOSetCurrentStationInt(iStationIndex)==FAILURE)
        return FAILURE;
    Optr=Optr->next;     /* if there is second operand, get the Z-direction */
    if (Optr)
    {
        if( CMDgetValue(Optr,&lDir) == FAILURE )
            return FAILURE;
    }
    iRet = ROMoveDirect(iStationIndex,(int)lDir);  /* move all the axes to station. */

    return iRet;
}
/***************************************************************************
 * Function Name:   ex_MTCR
 * Description:     Move to a station with optional retract and Z-offset and followed by extend.
 * Implementation:  Get the station name and use the station name to get the station index. Then
 *                  set current station as the specified station from the first operand. Get the
 *                  the Z-offset direction flag (0..2) from the second operand. Get the rectract
 *                  flag (TRUE/FALSE) from third operand for doing retract or not. Then get scanner
 *                  flag from fourth operand for telling the station is the scanner station and to deal
 *                  with the Z-position and Z-index of scanner station if the scanner flag is TRUE.
 *                  Then call ROMoveRetract() with providing the information from the operands to do the
 *                  move to the specified station with all the optional flags such as retract, z-offset and
 *                  scanner mode. This command will finished the movement with extending the radial axis.
 * NOTE:            This function will get executed when the user typed MTCR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MTCR <STATION>, <Z-OFFSET>, <RETRACT FLAG>, <SCAN MODE FLAG>
 *                      - STATION:          station name
 *                      - Z-OFFSET:         Z-offset direction - same as MATS command
 *                      - RETRACT FLAG:     to do retract or not
 *                      - SCAN MODE FLAG:   for scanner station if mode is true, otherwise for regular station;
 *                                          to deal with scanner Z-position.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MTCR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex, iDoRetract=TRUE;
    long lVal, lZOffset=0, lUseScan=FALSE;
    int iActionStatus;
    int iRet;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )  /* get the station index */
		return FAILURE;
    /* set the current station */
    if(FIOSetCurrentStationInt(iStationIndex)==FAILURE)
        return FAILURE;
    /* set scanner station info */
    MPGetExtStn( iStationIndex<26?iStationIndex+'A':iStationIndex+'a'-26, 1 );
    Optr=Optr->next;     /* if 2nd operand */
    if (Optr)
    {
        if( CMDgetValue(Optr,&lZOffset) != SUCCESS )  /* get Z-offset direction flag*/
            return FAILURE;
        Optr=Optr->next;  /* if 3rd operand  */
        if (Optr)
        {
            if( CMDgetValue(Optr,&lVal) == FAILURE )
                return FAILURE;
            iDoRetract = (int)(lVal==0);  /* get retraction option flag*/
            Optr=Optr->next;  /* if 4th operand */
            if (Optr)
            {
                if( CMDgetValue(Optr,&lUseScan) == FAILURE )  /* Get scan mode flag from 4th operand */
                    return FAILURE;
            }
        }
    }
    /* moves theta and Z axis to station and extend the radial at the final movement. */
    iActionStatus = ROActionStatus();
    if (iActionStatus)
	MASetPC(instr);         // re-execute the instruction to wait until 
    else
    {
    	iRet = ROMoveRetract( iStationIndex, (int)lZOffset, iDoRetract, TRUE, (int)lUseScan );

        return iRet;
    }
}
/***************************************************************************
 * Function Name:   ex_MTPS
 * Description:     Move to station, with retract, with/without(optional) z-offset, and no final extension.
 *                  The command is always retracts the radial axis first.
 * Implementation:  Get station name from the first operand and use the station name to get the
 *                  station index.  Start at the second operand on, the operands are optional.
 *                  If there is operand then get the value of Z-offset direction flag from second
 *                  operand, get the retraction flag from third operand and get scan mode flag from the
 *                  four operand. Then execute the function ROMoveDirect() which will retract first and
 *                  move theta and Z to station with optional flag of Z-offset and scan mode flag if they
 *                  exist. No final extension.
 * NOTE:            This function will get executed when the user typed MTPS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MTPS <STATION>, [Z-OFFSET], [RETRACT], [SCAN MODE]
 *                      - STATION:      station name
 *                      - Z-OFFSET:     Optional parameter for Z-offset direction flag
 *                      - RETRACT:      Optional parameter for retraction flag
 *                      - SCAN MODE:    Optional parameter for scan mode flag (believe for scanner station or regular station
 *                                      since it deals with Z-positon and Z-index of scanner station if scan mode is TRUE.)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MTPS(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationIndex, iDoRetract=TRUE;
    long lVal, lZOffset=0, lUseScan=FALSE;
    int iRet;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if( (iStationIndex=CMDgetStnIndx(Optr)) == FAILURE )  /* get the station index */
        return FAILURE;
    /* set the current station */
    if(FIOSetCurrentStationInt(iStationIndex)==FAILURE)
        return FAILURE;
    /* set scanner station info */
    MPGetExtStn( iStationIndex<26?iStationIndex+'A':iStationIndex+'a'-26, 1 );
    /* the rest of operand can be optional */
    Optr=Optr->next;     /* if 2nd operand */
    if (Optr)
    {
        if( CMDgetValue(Optr,&lZOffset) == FAILURE )  /* get the Z-offset optional flag */
            return FAILURE;
        Optr=Optr->next;  /* if 3rd operand */
        if (Optr)
        {
            if( CMDgetValue(Optr,&lVal) == FAILURE )
                return FAILURE;
            iDoRetract = (int)(lVal==0);  /* get retraction option */
            Optr=Optr->next;  /* if 4th operand */
            if (Optr)
            {
                if( CMDgetValue(Optr,&lUseScan) == FAILURE )  /* get scanner option */
                    return FAILURE;
            }
        }
    }
    /* move the axis to station with retract first and without Z-offset and no final extend. */
    iRet = ROMoveRetract( iStationIndex, (int)lZOffset, iDoRetract, FALSE, (int)lUseScan );

    return iRet;
}
/***************************************************************************
 * Function Name:   ex_MHS
 * Description:     Moves specified axis to the Home Switch
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MHS(instr_ptr instr)
{
    ULONG ulAxis;
    int iRet;

    ulAxis = CMDgetAxis(instr);          /* get the axis */
    if( ulAxis != (unsigned long)FAILURE )
    {
        iRet = ROSendMoveToHSorIP(ulAxis, 0, 1, TRUE);
        return iRet;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_MIP
 * Description:     Moves specified axis to the Index pulse
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MIP(instr_ptr instr)
{
    ULONG ulAxis;
    int iRet;

    ulAxis = CMDgetAxis(instr);          /* get the axis */
    if( ulAxis != (unsigned long)FAILURE )
    {
        iRet = ROSendMoveToHSorIP(ulAxis, 1, 1, TRUE);
        return iRet;
    }
    return FAILURE;
}

/***************************************************************************
 * Function Name:   ex_XRC
 * Description:     Starts Galil data recording by RC command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XRC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    ULONG ulAxis;
    long lFlag;

    Optr = instr->opr_ptr;         /* if there is an operand */
    if( Optr )
    {
        ulAxis = CMDgetAxis(instr);          /* get the axis */
        if( ulAxis != (unsigned long)FAILURE )
        {
	    Optr=Optr->next;     /* if 2nd operand */
    	    if (Optr)
    	    {
                if( CMDgetValue(Optr, &lFlag) == FAILURE )  /* get start index */
                    return FAILURE;
	    }
	    else
		lFlag = 0;
	    return ROSendRQ(ulAxis, lFlag);   // RC 1 starts
        }
    }
//printf("RC axis=%ld\n",ulAxis);
    return FAILURE;
}

/***************************************************************************
 * Function Name:   ex_XQU
 * Description:     Dumps Galil data recorded by RC command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XQU(instr_ptr instr)
{
    ULONG ulAxis;

    ulAxis = CMDgetAxis(instr);          /* get the axis */
    if( ulAxis != (unsigned long)FAILURE )
    {
	return ROSendQU(ulAxis);   // RC 1 starts
    }
    return FAILURE;	   // RC 0 ends
}
/***************************************************************************
 * Function Name:   ex_XDM
 * Description:     Dumps Galil data recorded by RC command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XDM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    ULONG ulAxis;
    long lFirst, lLast;

    Optr=instr->opr_ptr;         /* if there is an operand */
    ulAxis = CMDgetAxis(instr);          /* get the axis */
    if( ulAxis == (unsigned long)FAILURE )
	return FAILURE;

    Optr=Optr->next;     /* if 2nd operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lFirst) == FAILURE )  /* get start index */
            return FAILURE;
        Optr=Optr->next;  /* if 3rd operand */
        if (Optr)
        {
            if( CMDgetValue(Optr, &lLast) == FAILURE ) /* get last index */
                return FAILURE;
	    return RODumpRC(ulAxis, lFirst, lLast);  
	}
    }
//printf("DM axis=%ld first=%ld last=%ld\n",ulAxis, lFirst, lLast);
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SEDF
 * Description:     Sets Encoder Drift-detector Flag
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SEDF(instr_ptr instr)
{
    giEncoderDriftDetector = 0;
    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_REDF
 * Description:     Reads Encoder Drift-detected Status
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_REDF(instr_ptr instr)
{

    return CMDReadParam(instr->opr_ptr, giEncoderDriftDetector);
}
/***************************************************************************
 * Function Name:   ex_XCLR
 * Description:     Sets Encoder Drift-detector Flag
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XCLR(instr_ptr instr)
{
    int iDat;
    FILE *iFP;
    int iCount;

    CMDoperand_ptr Optr;
    long lAlarm, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum != 313) return FAILURE;

	ROClearAlarm();
    	return SUCCESS;
    }
    return FAILURE;
}

/***************************************************************************
 * Function Name:   ex_RARM
 * Description:     Reads Encoder Drift-detected Status
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RARM(instr_ptr instr)
{

    return CMDReadParam(instr->opr_ptr, giEncoderAlarm);
}
/***************************************************************************
 * Function Name:   ex_WXRC
 * Description:     Write RC data (Torques & Pos data) to a file
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WXRC(instr_ptr instr)
{
    return ROWriteRC();
}
/***************************************************************************
 * Function Name:   ex_RXRC
 * Description:     Read RC data (Torques & Pos data) from a file
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RXRC(instr_ptr instr)
{
    return ROReadRC();
}
// Wafer Pick Counter Increment
int ex_XWCI(instr_ptr instr)
{
    ++glPickCounter;
    return SUCCESS;
}

// Return Pick Counter 
int ex_XRWC(instr_ptr instr)
{
    return CMDReadParam(instr->opr_ptr, glPickCounter);
}

// Wafer Pick Counter Set (only for testing purpose)
int ex_XSWC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lCount, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum != 313) return FAILURE;

    	Optr=Optr->next;         /* if there is a second operand */
	if(!Optr) return FAILURE;

        if( CMDgetValue(Optr, &lCount) == FAILURE )  /* counter set value */
            return FAILURE;
	
	if(lCount >= 0)
	    glPickCounter = lCount;
	else
	    return FAILURE;

    	return SUCCESS;
    }
    return FAILURE;
}

// System Idle Time Set
int ex_XITM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lTime, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum != 313) return FAILURE;

    	Optr=Optr->next;         /* if there is a second operand */
	if(!Optr) return FAILURE;

        if( CMDgetValue(Optr, &lTime) == FAILURE )  /* idle time in seconds */
            return FAILURE;
	
	if(lTime >= 0)
	    glIdleTime = lTime;
	else
	    return FAILURE;

    	return SUCCESS;
    }
    return FAILURE;
}

// return system idle time
int ex_XRTM(instr_ptr instr)
{
    return CMDReadParam(instr->opr_ptr, glIdleTime);
}

// Set Maximum wafer pick counter
int ex_XSMX(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lCount, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum != 313) return FAILURE;

    	Optr=Optr->next;         /* if there is a second operand */
	if(!Optr) return FAILURE;

        if( CMDgetValue(Optr, &lCount) == FAILURE )  /* idle time in seconds */
            return FAILURE;
	
	if(lCount >= 0)
	    glMaxWaferPickCounter = lCount;
	else
	    return FAILURE;

    	return SUCCESS;
    }
    return FAILURE;
}

// return Maximum wafer pick counter
int ex_XRMX(instr_ptr instr)
{
    return CMDReadParam(instr->opr_ptr, glMaxWaferPickCounter);
}

// return axis to be tested for friction
int ex_XRFA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lValue[4];
    ULONG   ulAxis = RO_AXIS_T | RO_AXIS_R | RO_AXIS_Z; 

    lValue[0] = giFrictionTestAxis;
    lValue[1] = giFrictionTest;
    lValue[2] = glFrictionDataReady;
    return CMDreadBackParms( instr->opr_ptr, lValue, ulAxis );
}

// Set Friction Test flag
int ex_XSFT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lCount, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum != 313) return FAILURE;

    	Optr=Optr->next;         /* if there is a second operand */
	if(!Optr) return FAILURE;

        if( CMDgetValue(Optr, &lCount) == FAILURE )  /* idle time in seconds */
            return FAILURE;

    	Optr=Optr->next;         /* if there is a second operand */
	if(!Optr) return FAILURE;

        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* idle time in seconds */
            return FAILURE;

	
	if(lCount >= 0)
	{
	    giFrictionTest = 1;
	    glFrictionDataReady = lNum; // The data is overwritten
	    giFrictionTestAxis = lCount;  // start with the particular
	}
	else
	    return FAILURE;

    	return SUCCESS;
    }
    return FAILURE;
}

// Enable Latch for drift detection
int ex_XENL(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lCount, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum < 0 || lNum > 1) return FAILURE;

	if(lNum == 1)
	    RODisableLatch(0);
	else
	    RODisableLatch(1);

    	return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XREL
 * Description:     Reads Drift-detect Enable/Disable Status
 *			0 = drift check OK
 *			1 = drift check Disabled
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XREL(instr_ptr instr)
{

    return CMDReadParam(instr->opr_ptr, giDisableLatch);
}

// Read Motor Resolution
int ex_XRMR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr; 
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);  // get the axis 
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	lData = glMotorResolution[lAxis];
	return CMDReadParam(Optr->next, lData);
    }
    return FAILURE;
}

//
// MOTION PROFILE DATA ACQUISITION
//
int ex_XMPA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iStationSource, iStationDestin;
    long lSourcePos[4], lDestinPos[4];
    long lEncSource, lEncDestin;
    long lCHome[8];
    ULONG ulAxis;

    ulAxis = CMDgetAxis(instr);	// get axis first

    Optr=instr->opr_ptr;
    if(Optr)
    {

	Optr = Optr->next;
	iStationSource = CMDgetStnIndx(Optr);
    	if( iStationSource < 0 )
            return FAILURE;

     	Optr = Optr->next;
	iStationDestin = CMDgetStnIndx(Optr);
    	if( iStationDestin < 0 )
            return FAILURE;

        if( FIOGetStnCoord(iStationSource, 0, &lSourcePos[0])==FAILURE ||
            FIOGetStnCoord(iStationSource, 1, &lSourcePos[1])==FAILURE ||
            FIOGetStnCoord(iStationSource, 2, &lSourcePos[2])==FAILURE ) 
	    return FAILURE;

        if( FIOGetStnCoord(iStationDestin, 0, &lDestinPos[0])==FAILURE ||
            FIOGetStnCoord(iStationDestin, 1, &lDestinPos[1])==FAILURE ||
            FIOGetStnCoord(iStationDestin, 2, &lDestinPos[2])==FAILURE ) 
	    return FAILURE;

	switch(ulAxis)
	{
	    case RO_AXIS_R: // R-axis source position is retract pos
    		if( FIOGetParamVals(ROBOTFILE, CUSTOMIZED_HOME, lCHome) == FAILURE )
        	    return FAILURE;
		if (ROEUToEncoder(ulAxis, lCHome[1], &lEncSource)==FAILURE)
		    return FAILURE;
		if (ROEUToEncoder(ulAxis, lSourcePos[1], &lEncDestin)==FAILURE)
		    return FAILURE;
		break;
	    case RO_AXIS_W:
 	    	if( FIOGetStnVals(iStationSource, TRACK_POS, &lSourcePos[3]) == FAILURE ) 
		    return FAILURE;
 	    	if( FIOGetStnVals(iStationDestin, TRACK_POS, &lDestinPos[3]) == FAILURE ) 
		    return FAILURE;
		if (ROEUToEncoder(ulAxis, lSourcePos[3], &lEncSource)==FAILURE)
		    return FAILURE;
		if (ROEUToEncoder(ulAxis, lDestinPos[3], &lEncDestin)==FAILURE)
		    return FAILURE;
		break;
	    case RO_AXIS_T:
		if (ROEUToEncoder(ulAxis, lSourcePos[0], &lEncSource)==FAILURE)
		    return FAILURE;
		if (ROEUToEncoder(ulAxis, lDestinPos[0], &lEncDestin)==FAILURE)
		    return FAILURE;
		break;
	    case RO_AXIS_Z:
		if (ROEUToEncoder(ulAxis, lSourcePos[2], &lEncSource)==FAILURE)
		    return FAILURE;
		if (ROEUToEncoder(ulAxis, lDestinPos[2], &lEncDestin)==FAILURE)
		    return FAILURE;
		break;
	    default:
		return FAILURE;
	}
	if(ROMotionProfTest(ulAxis, lEncSource, lEncDestin) == FAILURE)
	    return FAILURE;

	return SUCCESS;

    }

    return FAILURE;
}

/***************************************************************************
 * Function Name:   ex_XFDC
 * Description:     Friction data Clear
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XFDC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lAlarm, lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum != 313) return FAILURE;

	ROClearFrictionData();
    	return SUCCESS;
    }
    return FAILURE;
}

/***************************************************************************
 * Function Name:   ex_XMVC
 * Description:     Move absolute axis to position(s) specified in cartesian coordinate system, s straight line. Must specify T and R.
 ***************************************************************************/
int ex_XMVC(instr_ptr instr)
{
    ULONG ulAxis;
    long lPosition[8]={0,0,0,0,0,0,0,0};
    CMDoperand_ptr Optr=instr->opr_ptr;
    int iRet;

      /* get axis from first operand */
    if( (ulAxis=CMDgetAxis(instr)) != (unsigned long)FAILURE )
    {
        /* get value for specific axis  */
	if(CMDgetAxisValue(ulAxis, Optr->next, lPosition)==FAILURE)
	    return FAILURE;
        /* move absolute to the position specified. */
        iRet = ROMoveToCartesian( ulAxis, lPosition );

        return iRet;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XSEE
 * 	Set End-Effector length for vector operation
 ***************************************************************************/
int ex_XSEE(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum < 0) return FAILURE;

	sVectorParameter.m_lEELength = lNum;
    	return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XREE
 * 	Read End-Effector length for vector operation
 ***************************************************************************/
int ex_XREE(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    return CMDReadParam(instr->opr_ptr, sVectorParameter.m_lEELength);
}
/***************************************************************************
 * Function Name:   ex_XSVS
 * 	Set Vector Speed for vector operation
 ***************************************************************************/
int ex_XSVS(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum < 0) return FAILURE;

	sVectorParameter.m_lVS = lNum;
    	return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XRVS
 * 	Read vector speed for vector operation
 ***************************************************************************/
int ex_XRVS(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    return CMDReadParam(instr->opr_ptr, sVectorParameter.m_lVS);
}
/***************************************************************************
 * Function Name:   ex_XSVA
 * 	Set Vector Acceleration for vector operation
 ***************************************************************************/
int ex_XSVA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum < 0) return FAILURE;

	sVectorParameter.m_lVA = lNum;
    	return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XRVA
 * 	Read vector accel for vector operation
 ***************************************************************************/
int ex_XRVA(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    return CMDReadParam(instr->opr_ptr, sVectorParameter.m_lVA);
}

/***************************************************************************
 * Function Name:   ex_XSVD
 * 	Set Vector Deceleration for vector operation
 ***************************************************************************/
int ex_XSVD(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* complex number */
            return FAILURE;

	if(lNum < 0) return FAILURE;

	sVectorParameter.m_lVD = lNum;
    	return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XRVD
 * 	Read vector decel for vector operation
 ***************************************************************************/
int ex_XRVD(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    return CMDReadParam(instr->opr_ptr, sVectorParameter.m_lVD);
}

/***************************************************************************
 * Function Name:   ex_XSAV
 * 	save vector parameter for vector operation
 ***************************************************************************/
int ex_XSAV(instr_ptr instr)
{
    return FIOWriteVectorFile();
}

int ex_XSRF(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum) == FAILURE )  /* Efem Theta Zero Angle */
            return FAILURE;

	if(lNum < 0) return FAILURE;

	sVectorParameter.m_lEfemTheta0 = lNum;
	sVectorParameter.m_fSinTheta = ROGetSinTheta(lNum);
	sVectorParameter.m_fCosTheta = ROGetCosTheta(lNum);
    	return SUCCESS;
    }
    return FAILURE;
}

int ex_XRRF(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum;

    Optr=instr->opr_ptr;         /* if there is an operand */
    CMDReadParam(Optr, sVectorParameter.m_lEfemTheta0);

    if (Optr)
	Optr = Optr->next;
    lNum = (long) (1000. * sVectorParameter.m_fSinTheta);
    CMDReadParam(Optr, lNum);

    if (Optr)
	Optr = Optr->next;
    lNum = (long) (1000. * sVectorParameter.m_fCosTheta);
    CMDReadParam(Optr, (long) (1000 * sVectorParameter.m_fCosTheta));

    return SUCCESS;
}

int ex_XR2W(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum1, lNum2, lNum3, lNum4;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum1) == FAILURE )  /* Theta */
            return FAILURE;
    }
    else
	return FAILURE;

    Optr = Optr->next;
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum2) == FAILURE )  /* R */
            return FAILURE;
    }
    else
	return FAILURE;

    ROConvertRobotToWorld(lNum1, lNum2, &lNum3, &lNum4);

    Optr = Optr->next;
    CMDReadParam(Optr, lNum3);

    if (Optr)
	Optr = Optr->next;
    CMDReadParam(Optr, lNum4);

    return SUCCESS;
}

int ex_XW2R(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum1, lNum2, lNum3, lNum4;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum1) == FAILURE )  /* x */
            return FAILURE;
    }
    else
	return FAILURE;

    Optr = Optr->next;
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum2) == FAILURE )  /* y */
            return FAILURE;
    }
    else
	return FAILURE;

    ROConvertWorldToRobot(lNum1, lNum2, &lNum3, &lNum4);

    Optr = Optr->next;
    CMDReadParam(Optr, lNum3);

    if (Optr)
	Optr = Optr->next;
    CMDReadParam(Optr, lNum4);

    return SUCCESS;
}

int ex_XW2E(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum1, lNum2, lNum3, lNum4;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum1) == FAILURE )  /* x */
            return FAILURE;
    }
    else
	return FAILURE;

    Optr = Optr->next;
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum2) == FAILURE )  /* y */
            return FAILURE;
    }
    else
	return FAILURE;

    ROConvertWorldToEfem(lNum1, lNum2, &lNum3, &lNum4);

    Optr = Optr->next;
    CMDReadParam(Optr, lNum3);

    if (Optr)
	Optr = Optr->next;
    CMDReadParam(Optr, lNum4);

    return SUCCESS;
}

int ex_XE2W(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lNum1, lNum2, lNum3, lNum4;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum1) == FAILURE )  /* x */
            return FAILURE;
    }
    else
	return FAILURE;

    Optr = Optr->next;
    if (Optr)
    {
        if( CMDgetValue(Optr, &lNum2) == FAILURE )  /* y */
            return FAILURE;
    }
    else
	return FAILURE;

    ROConvertEfemToWorld(lNum1, lNum2, &lNum3, &lNum4);

    Optr = Optr->next;
    CMDReadParam(Optr, lNum3);

    if (Optr)
	Optr = Optr->next;
    CMDReadParam(Optr, lNum4);

    return SUCCESS;
}

/***************************************************************************
 * Function Name:   ex_XMCD
 * Description:     Cartesian move in direction from the current pos.
 ***************************************************************************/
int ex_XMCD(instr_ptr instr)
{
    long lDirection, lDistance;
    CMDoperand_ptr Optr;

    Optr=instr->opr_ptr;         /* if there is an operand */
    if (Optr)
    {
        if( CMDgetValue(Optr, &lDirection) == FAILURE )  /* x */
            return FAILURE;
    }
    else
	return FAILURE;

    Optr = Optr->next;
    if (Optr)
    {
        if( CMDgetValue(Optr, &lDistance) == FAILURE )  /* y */
            return FAILURE;
    }
    else
	return FAILURE;

    return ROMoveCartesianDirection(lDirection, lDistance);

}
