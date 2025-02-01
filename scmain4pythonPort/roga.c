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
 * File:        roga.c
 * Functions:   ROHomeAxis
 *              ROMoveToAbsOrRel
 *              ROMoveToAbs
 *              ROMoveToRel
 *              ROMoveRetract
 *              ROMoveDirect
 *              ROMoveAxisDirect
 *              ROMoveStroke
 *              RORetractR
 *              ROSendMoveToHSorIP
 *              ROMoveAllOrAxisDirect
 *
 * Description: Handles all normal robot motion
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/
#include <math.h>
#include <sys/io.h>

#include "sck.h"
#include "ro.h"
#include "roga.h"
#include "roloc.h"
#include "rofio.h"
#include "romain.h"
#include "gaintr.h"
#include "gag.h"
#include "fiog.h"
#include "fio.h"
#include "mapstn.h"
#include "sctch.h"
#include "ser.h"
#include "dmclnx.h"
#include "sctim.h"
#include "scio.h"

#define PIDEF 3.14159265359
#define HUNDREDDEG_TO_RAD ( PIDEF / 18000.0 )   // .01 degrees to 1 radian conversion
#define DEG_TO_RAD   ( PIDEF / 180.0 )      // 1 degree to 1 radian
#define RAD_TO_HUNDREDDEG ( 18000.0 / PIDEF )   // 1 radian to .01 degrees conversion
#define RAD_TO_DEG   ( 180.0 / PIDEF )      // 1 radian to 1 degree

//long glEELength = 9250;
long glEELength = 7500;
extern stVectorParameter sVectorParameter;

/********** VARIABLES USED LOCALLY (within this source file) **********/

/* Variables used exclusively in ROHomeAxis */
long laHomeOffset[8];
long laCustomizedHome[8];
long laHomeOffsetW[8];
long laCustomizedHomeW[8];
long laCustomizedHomeUnscale[8];
long laCustomizedHomeWUnscale[8];

unsigned long ulCurrentAxis, ulTempAxis;
char sBuf[80];
char cpNull[8];

extern HANDLEDMC ghDMC;
extern int giVersionPA;

extern unsigned long ulHoming;

extern int giEncoderAlarm;

/* NOTE: These variable are file globals, and not internal to functions,
 *      because they are used by the action buffer. If they were used locally
 *      they would disappear after the function was exited and the action
 *      buffer would be left high and dry. */

extern int giIPW[8];
extern int giIPWenc[8];
extern int giIPWFlag[8];
extern int giIPWDirection[8];
extern int giIPWDestination[8];

int ALResetChuck();

char ReturnTT[TTARRAYSIZE*12];
char ReturnTP[TTARRAYSIZE*12];
int  TTTData[TTARRAYSIZE];
int  TTPData[TTARRAYSIZE];
int  TTPDataEU[TTARRAYSIZE];
double  TDFT[TTARRAYSIZE];
int  RTTData[TTARRAYSIZE];
int  RTPData[TTARRAYSIZE];
int  RTPDataEU[TTARRAYSIZE];
double  RDFT[TTARRAYSIZE];
int  ZTTData[TTARRAYSIZE];
int  ZTPData[TTARRAYSIZE];
int  ZTPDataEU[TTARRAYSIZE];
double  ZDFT[TTARRAYSIZE];
int  WTTData[TTARRAYSIZE];
int  WTPData[TTARRAYSIZE];
int  WTPDataEU[TTARRAYSIZE];
double  WDFT[TTARRAYSIZE];
int  tTTData[TTARRAYSIZE];
int  tTPData[TTARRAYSIZE];
int  tTPDataEU[TTARRAYSIZE];
double  tDFT[TTARRAYSIZE];
int  rTTData[TTARRAYSIZE];
int  rTPData[TTARRAYSIZE];
int  rTPDataEU[TTARRAYSIZE];
double  rDFT[TTARRAYSIZE];
int  zTTData[TTARRAYSIZE];
int  zTPData[TTARRAYSIZE];
int  zTPDataEU[TTARRAYSIZE];
double  zDFT[TTARRAYSIZE];
int  wTTData[TTARRAYSIZE];
int  wTPData[TTARRAYSIZE];
int  wTPDataEU[TTARRAYSIZE];
double  wDFT[TTARRAYSIZE];

long glMotorResolution[8];
long glEncoderResolution[8];

int giXRCFlag = 0;
int giRCStatusToWrite = 0;
int giCheckIdle = 0;

extern long glRobotIdleStart;
extern int giFrictionTest;	// Friction Test to be performed
extern long glFrictionDataReady;	// Friction data ready?
extern long glPickCounter;	// wafer pick counter, when it's greater than 2000
extern long glIdleTime;	// System Idle time 30 seconds.
extern long glMaxWaferPickCounter; // wafer pick counter checked against this value
extern int giFrictionTestAxis;	// Axis tested for friction test
extern int giDisableLatch;
extern int giNumOfAxes;
extern int giXYZRobot;

int giFrictionTestFail;

/****************************************************************
 *
 * Function Name:   ROHomeAxis
 *
 * Description:     Home the specified axis. If no axis is
 *      specified (axis = A) then home all robot axes. Otherwise,
 *      home the specific axis. It starts by initialzing variables,
 *      collecting information about the axis to home (speeds,
 *      accelerations, etc.), and setting up action buffer variables.
 *      Then it schedules a DP, PA, and BG in the action buffer.
 *      Finally it kick starts the homing by sending an HM and
 *      BG.
 *
 *      For A (all robot) axes, it schedules a DP, PA, and BG
 *      for the R axis first, then schedules the HM and BG for
 *      T and Z axes, and then schedules the DP, PA, and BG for
 *      the T and Z axes. It finally kick starts the homing with
 *      an HM and BG for the R axis.
 *
 *      For a (all pre-aligner) axes, it schedules a DP, PA, and
 *      BG for the r and z axes and then just a DP for the t axis.
 *      It finally kick starts the homing with an HM and BG for
 *      the r and z axes. The spindle (t) has no home switch.
 *
 *      The track or flipper schedules a DP, PA, and BG and
 *      then gets kicked with an HM and BG. W and w axes
 *      have to be homed separately so they don't need much scheduling.
 *
 *      VAC514 robot requires special homing sequence:
 *
 *      Homing on VAC514 robot for R or T axis is same:
 *      1. FEY with the user defined home speed
 *      2. FEY with the slow hardcoded speed
 *      3. FEX at the user defined speed along with the HMZ at user defined speed
 *      4. FEX at the slow hardcoded speed
 *      5. Disengage any gearing
 *      5. JG -hardcoded speed, hardcoded speed to set directions for the Find Index Pulse motion
 *      6. FIXY
 *      7. Wait for all axes including Z, and do DP, home_offset, home_offset, home_offset
 *      8. Move to Customized home for all axes
 *
 * Parameter:
 *      ulEquipeAxisArg     (in) The axes to home.
 *      iStageFlagArg       indicates if we are homing the Stage
 *                          (In Stage homing process we replace HM Galil command with FE)
 *
 * Return:          SUCCESS or FAILURE.
 *
 ***************************************************************/
int ROHomeAxis(unsigned long ulEquipeAxisArg, int iStageFlagArg)
{
    int iReturn, iAxis, iCardNum, iFileType;
    unsigned uGalilAxes;
    long laHomeSpeed[8];    /* The home speeds to set for the axes. */
    long laHomeAccel[8];    /* The home accelerations to set for the axes. */
    long laHomeDecel[8];    /* The home decelerations to set for the axes. */
    long laSCurves[8];      /* The array to turn of S-curve profiling. Since we're
                             * moving without knowing when we are going to hit a
                             * home switch, S-curve profiles don't make sense. */
    long laScanOffset[8];   /* For a dual arm, the scanner offset assists in determining
                             * the customized home position. */
    unsigned uAMWait, uAMNext, uINWait, uINNext, uTMWait;
    unsigned long ulAxis, ulHomeParsSetTempAxis, ulVAC514TempAxis;
    long rc;
    USHORT usStatus;
    char HMcommand[MAXGASTR];
    char ReturnBuffer[MAXGASTR];


    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    /* We have to set a permanent variable to the axis definition because
     * it is referenced in the action buffer. The passed in parameter will
     * disappear as soon as we exit this function. */
    ulCurrentAxis = ulEquipeAxisArg;

    /* Variable initialization. */
    for (iAxis=0; iAxis<8; iAxis++)
    {
        laHomeSpeed[iAxis] = 0;
        laHomeAccel[iAxis] = 0;
        laHomeDecel[iAxis] = 0;
        laSCurves[iAxis] = 0;
        laScanOffset[iAxis] = 0;
    }

    /* Validation to check if homing is possible with the given axis parameters. */
    if (ROValidAxis(ulCurrentAxis, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Must have servo on and motion complete. */
    if (ulCurrentAxis & (ulServoFlag | ~ulAMFlag) )
        return FAILURE;

    // On VAC514, T and R are always geared, thus check for motion flag on both
    if(iDefineFlag & DFVAC514)
    {
        if(ulCurrentAxis & RO_AXIS_R)
        {
            if (RO_AXIS_T & (ulServoFlag | ~ulAMFlag))
                return FAILURE;
        }

        if(ulCurrentAxis & RO_AXIS_T)
        {
            if (RO_AXIS_R & (ulServoFlag | ~ulAMFlag))
                return FAILURE;
        }
    }

    // On VAC514 homing T or R axis requires to home both axes thus set the parameters for both axes
    if((iDefineFlag & DFVAC514) && ((ulCurrentAxis == RO_AXIS_T) || (ulCurrentAxis == RO_AXIS_R)))
        ulHomeParsSetTempAxis = RO_AXIS_T | RO_AXIS_R;
    else
        ulHomeParsSetTempAxis = ulCurrentAxis;

    /* Get and set speeds and accelerations. These are very slow during homing.
     * Also turn off S-curve profiling.
     * we check if we are homing the Stage, it's homing doesn't require special homing speeds,
     * we just need to turn off the S-Curve. */
    if(!iStageFlagArg)
    {
        if (ROGetParameter(TRUE, ulHomeParsSetTempAxis, laHomeSpeed, HOME_SPEED) == FAILURE)
            return FAILURE;
        if (ROGetParameter(TRUE, ulHomeParsSetTempAxis, laHomeAccel, HOME_ACCEL) == FAILURE)
            return FAILURE;

        for (iAxis=0; iAxis<8; iAxis++) laHomeDecel[iAxis] = laHomeAccel[iAxis];

        if (ROSetParameter(FALSE, ulHomeParsSetTempAxis, laHomeSpeed, OPERATIONAL_SPEED) == FAILURE)
            return FAILURE;
        if (ROSetParameter(FALSE, ulHomeParsSetTempAxis, laHomeAccel, OPERATIONAL_ACCEL)==FAILURE)
            return FAILURE;
        if (ROSetParameter(FALSE, ulHomeParsSetTempAxis, laHomeDecel, OPERATIONAL_DECEL)==FAILURE)
            return FAILURE;
    }
//    if (ROEnableSCurveProfile(ulHomeParsSetTempAxis, laSCurves)==FAILURE)
//        return FAILURE;

    /* Get the home offset and customized home position. The home offset is the distance
     * from the home switch that the actual home position is. It is recorded in the
     * datafile in encoder counts. */
    if ((ulCurrentAxis == ROGetSpecialAxis(RO_TRACK)) || (ulCurrentAxis == ROGetSpecialAxis(RO_DUAL_ARM)))
   {
        if (ROGetParameter(TRUE, ulHomeParsSetTempAxis, laHomeOffsetW, HOME_OFFSET) == FAILURE)
            return FAILURE;
        if (ROGetParameter(TRUE, ulHomeParsSetTempAxis, laCustomizedHomeW, CUSTOMIZED_HOME) == FAILURE)
            return FAILURE;
    }
    else
    {
        if (ROGetParameter(TRUE, ulHomeParsSetTempAxis, laHomeOffset, HOME_OFFSET) == FAILURE)
            return FAILURE;
        if (ROGetParameter(TRUE, ulHomeParsSetTempAxis, laCustomizedHome, CUSTOMIZED_HOME) == FAILURE)
            return FAILURE;
        if (ulCurrentAxis & RO_AXIS_t)	// prealigner t
            laHomeOffset[4] = laCustomizedHome[4] = 0;
		if (ulCurrentAxis & RO_AXIS_T && giVersionPA) // prealigner t
            laHomeOffset[0] = laCustomizedHome[0] = 0;

    }

    uTMWait = uINWait = uINNext = 0;

//    usStatus = 1;  // clear interrupt status
//    rc = DMCGetInterruptStatus(ghDMC, &usStatus);// clear interrupt status

//    sprintf(HMcommand, "EI256");
//    rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);

    ulHoming |= ulCurrentAxis;

    /* If the user asks for multiple axes, do only 1 first, specifically R. */
    if (ulCurrentAxis == ulAxisALLRbt || ulCurrentAxis == ulAxisallPre)
    {
	if(ulCurrentAxis == ulAxisALLRbt)
	{
		ulHomed &= ~RO_AXIS_T;
		ulHomed &= ~RO_AXIS_R;
		ulHomed &= ~RO_AXIS_Z;
		ulAMFlag &= ~RO_AXIS_T;
		ulAMFlag &= ~RO_AXIS_R;
		ulAMFlag &= ~RO_AXIS_Z;
		sprintf(HMcommand, "VRTO = %d", laHomeOffset[0]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_T, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRTC = %d", laCustomizedHomeUnscale[0]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "VRRO = %d", laHomeOffset[1]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_R, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRRC = %d", laCustomizedHomeUnscale[1]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "VRZO = %d", laHomeOffset[2]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_Z, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRZC = %d", laCustomizedHomeUnscale[2]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "XQ#HOMA,0");
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
	}	
	else
	{	// all pre axis: PA=XYZ, others=EFG galil axis
		if (giVersionPA)
		{	// DP0 and HOME R and Z
			sprintf(HMcommand, "DP0");
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                	ulHomed |= RO_AXIS_t;
			ulHomed &= ~RO_AXIS_r;
			ulHomed &= ~RO_AXIS_z;
	    		ulAMFlag &= ~RO_AXIS_r;
	    		ulAMFlag &= ~RO_AXIS_z;
			sprintf(HMcommand, "VRRO = %d", laHomeOffset[1]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_r, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VRRC = %d", laCustomizedHomeUnscale[1]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "VRZO = %d", laHomeOffset[2]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_z, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VRZC = %d", laCustomizedHomeUnscale[2]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMR,1");
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMZ,2");
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		}
		else	// Home ALL axes
		{
			// DP 0 fo E-axis (t-axis)
			sprintf(HMcommand, "DP,,,,0");
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
        		ulHomed |= RO_AXIS_t;
			sprintf(HMcommand,"VHMD = %d",(int) ulHomed);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ulHomed &= ~RO_AXIS_r;
			ulHomed &= ~RO_AXIS_z;
	    		ulAMFlag &= ~RO_AXIS_r;
	    		ulAMFlag &= ~RO_AXIS_z;
			sprintf(HMcommand, "VPRO = %d", laHomeOffset[5]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_r, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VPRC = %d", laCustomizedHomeUnscale[5]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "VPZO = %d", laHomeOffset[6]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_z, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VPZC = %d", laCustomizedHomeUnscale[6]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMa,3");
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		}
	}
    }
    /* If the user asks for a single axis, set it up. */
    else
    {
        switch (ulCurrentAxis)
        {
            case RO_AXIS_t :    /* Prealigner spindle is relative and doesn't need homing. */
		    //ALResetChuck();
		if(giVersionPA)
			sprintf(HMcommand, "DP0");
		else
			sprintf(HMcommand, "DP,,,,0");
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
        	ulHomed |= RO_AXIS_t;
		sprintf(HMcommand,"VHMD = %d",(int) ulHomed);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return SUCCESS;

            case RO_AXIS_r :
		if (giVersionPA)
		{
	                ulHomed &= ~RO_AXIS_r;
	    		ulAMFlag &= ~RO_AXIS_r;
			sprintf(HMcommand, "VRRO = %d", laHomeOffset[1]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_r, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VRRC = %d", laCustomizedHomeUnscale[1]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMR,1");
		}
		else
		{
	                ulHomed &= ~RO_AXIS_r;
	    		ulAMFlag &= ~RO_AXIS_r;
			sprintf(HMcommand, "VPRO = %d", laHomeOffset[5]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_r, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VPRC = %d", laCustomizedHomeUnscale[5]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMr,1"); 
		}
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
		break;
		
		
            case RO_AXIS_z :
		if (giVersionPA)
		{
	                ulHomed &= ~RO_AXIS_z;
	    		ulAMFlag &= ~RO_AXIS_z;
			sprintf(HMcommand, "VRZO = %d", laHomeOffset[2]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_z, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VRZC = %d", laCustomizedHomeUnscale[2]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMZ,2");
		}
		else
		{
	                ulHomed &= ~RO_AXIS_z;
	    		ulAMFlag &= ~RO_AXIS_z;
			sprintf(HMcommand, "VPZO = %d", laHomeOffset[6]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			ROUnscalePos(RO_AXIS_z, laCustomizedHome, laCustomizedHomeUnscale);
			sprintf(HMcommand, "VPZC = %d", laCustomizedHomeUnscale[6]);
			rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
			sprintf(HMcommand, "XQ#HOMz,2"); 
		}
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
                break;

            case RO_AXIS_w :    /* Almost always track. */
                ulHomed &= ~RO_AXIS_w;
    		ulAMFlag &= ~RO_AXIS_w;
		sprintf(HMcommand, "VPWO = %d", laHomeOffset[3]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_w, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VPWC = %d", laCustomizedHomeUnscale[3]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "XQ#HOMw,7");	// track in 8-axis system
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
		break;

            case RO_AXIS_T :
                ulHomed &= ~RO_AXIS_T;
    		ulAMFlag &= ~RO_AXIS_T;
		sprintf(HMcommand, "VRTO = %d", laHomeOffset[0]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_T, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRTC = %d", laCustomizedHomeUnscale[0]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "XQ#HOMT,4");	// robot T
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
		break;

            case RO_AXIS_R :
                ulHomed &= ~RO_AXIS_R;
    		ulAMFlag &= ~RO_AXIS_R;
		sprintf(HMcommand, "VRRO = %d", laHomeOffset[1]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_R, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRRC = %d", laCustomizedHomeUnscale[1]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "XQ#HOMR,5");	// robot R or prealinger r if PA
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
				break;

            case RO_AXIS_Z :
                ulHomed &= ~RO_AXIS_Z;
    		ulAMFlag &= ~RO_AXIS_Z;
		sprintf(HMcommand, "VRZO = %d", laHomeOffset[2]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_Z, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRZC = %d", laCustomizedHomeUnscale[2]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "XQ#HOMZ,6");	// robot Z or prealinger z if PA
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
		break;

            case RO_AXIS_W :
                ulHomed &= ~RO_AXIS_W;
    		ulAMFlag &= ~RO_AXIS_W;
		sprintf(HMcommand, "VRWO = %d", laHomeOffset[3]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		ROUnscalePos(RO_AXIS_W, laCustomizedHome, laCustomizedHomeUnscale);
		sprintf(HMcommand, "VRWC = %d", laCustomizedHomeUnscale[3]);
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
		sprintf(HMcommand, "XQ#HOMW,7");	// robot W
		rc = GASendDMCCommand(ghDMC, HMcommand, ReturnBuffer, MAXGASTR);
                return rc;
				break;
            default:            /* Unreachable code. Valid axis names are checked first thing. */
                return FAILURE;
        }
    }


    /* Make sure that Galil accepted the commands successfully. */
    iReturn = GAGetGalilErrorFlag();
    if (iReturn)
        goto error_exit;


    return SUCCESS;

error_exit:
    return FAILURE;
}


/****************************************************************
 *
 * Function:    ROMoveToAbsOrRel
 *
 * Abstract:    Performs all types of moves, absolute or relative.
 *      It start by checking that the axes to be moved are valid
 *      in the system. Then it checks if those axes are ready to move,
 *      i.e. servoed on, homed, not in motion, and not in an error state.
 *      Then it grabs the starting position. This is used to calculate
 *      a relative move as well being used by the WPOA and WPOR host
 *      commands. The move is calculated and checked against the software
 *      limits. Then the position is converted to encoder counts and
 *      sent to the motion control card via a PA (position absolute)
 *      and BG (begin motion) command. Finally the AMFlag is changed
 *      to indicate that the axis is in motion.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      iRelativeFlagArg    (in) Whether it is an absolute or relative
 *                              move (TRUE/FALSE)
 *      lPositionArg        (in) The position to move to
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveToAbsOrRel(unsigned long ulEquipeAxisArg, int iRelativeFlagArg, long *lPositionArg)
{
    int i, iAxis;                  /* Temporary variable to loop through all axes. */
    int iMechAxis;              /* The bit pattern of the axis being checked. */
    long laPosSoftwareLimit[8]; /* Local copy of the positive software limit to check
                                 * against requested movement position. */
    long laNegSoftwareLimit[8]; /* Local copy of the negative software limit to check
                                 * against requested movement position. */
    long lStartPos[8];          /* Local variable of current position */
    long laFinalPosition[8]={0,0,0,0,0,0,0,0};  /* The final position to move to. */
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    /* Check to see if the axis is ready to move, i.e. servoed on, homed,
     * not currently in motion, and not currently in an error state. */
    if (ROAxisReadyToMove(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the start position to calculate a relative move and also for use
     * in the WPOA and WPOR host commands. */

    /* Check for VAC514, cannot move all axes (T&R are checked in ROAxisReadyToMove()) */
    if(iDefineFlag & DFVAC514)
    {
        if(ulEquipeAxisArg == RO_AXIS_ALL)
            goto error_exit;
    }

    lStartPos[0] = lStartPos[1] = lStartPos[2] = lStartPos[3] = 0;
    lStartPos[4] = lStartPos[5] = lStartPos[6] = lStartPos[7] = 0;
    if (ROReadCurrentPosition(ulEquipeAxisArg, lStartPos) == FAILURE)
        goto error_exit;

    /* Get the software limits to check that the move doesn't exceed boundary limits. */
    if(ROGetParameter(TRUE, ulEquipeAxisArg, laPosSoftwareLimit, SOFT_POS_LIMIT) == FAILURE)
        goto error_exit;
    if(ROGetParameter(TRUE, ulEquipeAxisArg, laNegSoftwareLimit, SOFT_NEG_LIMIT) == FAILURE)
        goto error_exit;

    /* Calculate a relative move and check the position against the software limits. */
    for (iAxis=0; iAxis<8; iAxis++)
    {
        /* Get the bit pattern for each axis. */
        iMechAxis = 1 << iAxis;
        /* If the axis is requested to be moved... */
        if (uGalilAxes & iMechAxis)
        {
            /* ...calculate the relative move position (absolute moves don't need to be calculated)... */
            if (iRelativeFlagArg)
            {
                lPositionArg[iAxis] += lStartPos[iAxis];
            }

            /* ...and check the position against the software limits. */
            if ((lPositionArg[iAxis] < laNegSoftwareLimit[iAxis]) ||
                (lPositionArg[iAxis] > laPosSoftwareLimit[iAxis]))
            {
                goto error_exit;
            }
 
	    if (giIPWFlag[iAxis])
	    {
		if (lPositionArg[iAxis] > lStartPos[iAxis]) // positive
		    giIPWDirection[iAxis] = 1;
		else					    // negative
		    giIPWDirection[iAxis] = -1;
	    }

        }
        /* If the axis is NOT requested to be moved, make sure it stays where it is. */
        else
        {
            lPositionArg[iAxis] = lStartPos[iAxis];
        }
    }

    /* Scale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, lPositionArg, laFinalPosition) == FAILURE)
        goto error_exit;

	for (i=0; i<4; i++)
		if (giIPWFlag[i] && (ulEquipeAxisArg & (1<<i)))
			giIPWDestination[i] = laFinalPosition[i] - giIPWenc[i] * giIPWDirection[i];

// print destin
//int giIPWDestinationEU[8];
//ROScalePos(ulEquipeAxisArg, giIPWDestination, giIPWDestinationEU);
//printf("IPWDestinEU=%d,%d,%d\n",giIPWDestinationEU[0],giIPWDestinationEU[1],giIPWDestinationEU[2]);
//printf("IPWDestin  =%d,%d,%d\n",giIPWDestination[0],giIPWDestination[1],giIPWDestination[2]);




    /* Send the PA (position absolute) and BG (begin motion) commands to the Galil card. */
    if (GASetValsLong(iCardNum, POSITION_ABS_COMMAND, uGalilAxes, laFinalPosition) == FAILURE)
        goto error_exit;

    // VAC514 needs to switch the master and slave for T and R motion each time motion applies on these
    if(iDefineFlag & DFVAC514)
    {
        if(ulEquipeAxisArg & RO_AXIS_T)
        {
            if(ROElectronicGearingOFF() == FAILURE)
                return FAILURE;
            if(ROElectronicGearingON('X', 'Y', VAC514_T_GEARING_RATIO, ROGetCM()) == FAILURE)
                return FAILURE;
        }
        else if(ulEquipeAxisArg & RO_AXIS_R)
        {
            if(ROElectronicGearingOFF() == FAILURE)
                return FAILURE;
            if(ROElectronicGearingON('Y', 'X', VAC514_R_GEARING_RATIO, ROGetCM()) == FAILURE)
                return FAILURE;
        }
    }

    if (GASendAxesCommand(iCardNum, BEGIN_MOTION_COMMAND, uGalilAxes) == FAILURE)
        goto error_exit;

    /* If everything was successful, indicate that the axis is in motion. */
    if (!GAGetGalilErrorFlag())
    {
        ulAMFlag &= ~ulEquipeAxisArg;
    }
    else
    {
        goto error_exit;
    }

    return SUCCESS;

error_exit:
    return FAILURE;
}


/****************************************************************
 *
 * Function:    ROMoveToAbs
 *
 * Abstract:    Performs absolute moves.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lPositionArg        (in) The position to move to
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveToAbs(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int iReturn;

    iReturn = ROMoveToAbsOrRel(ulEquipeAxisArg, FALSE, lPositionArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROMoveToRel
 *
 * Abstract:    Performs relative moves.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lPositionArg        (in) The position to move to
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveToRel(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int iReturn;

    iReturn = ROMoveToAbsOrRel(ulEquipeAxisArg, TRUE, lPositionArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROMoveRetract
 *
 * Abstract:    This is a subroutine to combine some of the code
 *      for MTPS and MTCR host commands. This routine will set up
 *      a sequence of actions. It starts by gathering information
 *      based on the parameters passed in: station coordinate info,
 *      Z offset info, retract before moving or not, extend at end
 *      of move or not, and whether to use scan position coordinates
 *      or not. Then it sets up the move: retract if desired, move
 *      T and Z axes using the correct Z position based on offset demands,
 *      and finally extend if desired.
 *
 * Parameters:
 *      iStnArg         (in) Station to move to (A-Z or a-z)
 *      iOffDirArg      (in) 0   don't use Z offset
 *                           1   add Z offset (go above wafer)
 *                           2   add Z offset - lStroke (go below wafer)
 *      iRetractArg     (in) TRUE to perform initial iRetractArg
 *      iExtendArg      (in) TRUE to perform final extension
 *      iUseScanArg     (in) TRUE will use Z-pos from scanning if one available
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveRetract(int iStnArg, int iOffDirArg, int iRetractArg, int iExtendArg, int iUseScanArg )
{
    int     iAxis, iSindex, iCount, iLastPass, iPass, iEEindex; /* Temporary variable.  */
    int     iCurrName = 0;
    long    lZdest, lZtemp, lZindex, lPitch, lStroke, lOffset;  /* Station parameters. */
    long    lScanCoord[8]={0,0,0,0,0,0,0,0};  /* The station scan position. */
    long    lCoord[8]={0,0,0,0,0,0,0,0};      /* The normal station position. */
    long    lSensorToEE[8]={0,0,0,0,0,0,0,0};
    long    lRHome[8];
    long    lExtPos[8];
    long    lPosition[8];
    long    lRHomeUncale[8];
    long    lExtPosUncale[8];
    long    lPositionUncale[8];
    unsigned long ulRAxis, ulTZAxis;
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    long rc;
//    USHORT usStatus;
    char GAcommand[MAXGASTR];
    char ReturnBuffer[MAXGASTR];

    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    /* Check that the parameters passed in are valid. */
    /* Stroke direction should be between 0 and 2. */
    if (iOffDirArg < 0 || iOffDirArg > 2)
        return FAILURE;
    if ((iRetractArg != TRUE) && (iRetractArg != FALSE))
        return FAILURE;
    if ((iExtendArg != TRUE) && (iExtendArg != FALSE))
        return FAILURE;
    if ((iUseScanArg != TRUE) && (iUseScanArg != FALSE))
        return FAILURE;

    /* Initialize variables. */
    for (iAxis=0; iAxis<8; iAxis++)
    {
        lRHome[iAxis] = 0;
        lPosition[iAxis] = 0;
        lExtPos[iAxis] = 0;
    }

    /* Check to see if the axis is ready to move, i.e. servoed on, homed,
     * not currently in motion, and not currently in an error state. */
    if (ROAxisReadyToMove(ulAxisALLRbt, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Need to get info about the second arm ASAP for axis checking */
    if (FIOGetStnEEindex(iStnArg, &iEEindex) == FAILURE)
        return FAILURE;
    if ((iEEindex == 1) && (ROGetSpecialAxis(RO_DUAL_ARM)))
    {
        ulRAxis = RO_AXIS_W;
        if (ROAxisReadyToMove(ulRAxis, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
            return FAILURE;
    }
    else
    {
        ulRAxis = RO_AXIS_R;
    }

    /*** Get station parameters. ***/
    for (iAxis = 0; iAxis < 3; iAxis++)
    {
        if (FIOGetStnCoord(iStnArg, iAxis, &lCoord[iAxis]) == FAILURE)
            return FAILURE;
        if (FIOGetStnScanCoord(iStnArg, iAxis, &lScanCoord[iAxis]) == FAILURE)
            return FAILURE;
    }
    if (FIOGetStnVals(iStnArg, PITCH, &lPitch) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, Z_INDEX, &lZindex) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, STROKE, &lStroke) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, OFFSET, &lOffset) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, RET_HOME, &lRHome[1]) == FAILURE)
        return FAILURE;

    /* The following change was made to convert a 10,000ths of an inch standard
     * to its mil real-world value. The Brooks/Novellus micron standard doesn't need it. */
    if (~iEmulator & DFEMULATORB)
    {
        lPitch /= 10;
    }

    /* Axes checking is done in the actual move function. */
    ulTZAxis = RO_AXIS_T|RO_AXIS_Z;
    lExtPos[1] = iUseScanArg ? lScanCoord[1] : lCoord[1];
    lExtPos[3] = lExtPos[1]; /* just prepare for W-axis */

    /*** Calculate destination height. ***/
    iCurrName = MPGetCurrScanName();
    if ( iUseScanArg && (iCurrName != -1) )
    {
        iSindex = MPSearchScanIndex(lZindex);
        if (MPGetScanParamLastPass(iCurrName, &iLastPass) == FAILURE)
            return FAILURE;
        if (iLastPass >= 0 && iSindex >= 0)
        {
            if (MPGetScanParamZPos(iCurrName, 0, iSindex, &lZdest) == FAILURE)
                return FAILURE;
            if (lZdest)
	            iCount = 1;
	        else
	            iCount = 0;

	        for ( iPass = 1; iPass <= iLastPass; iPass++ )
            {
                if (MPGetScanParamZPos(iCurrName, 0, iPass, &lZtemp) == FAILURE)
                    return FAILURE;
                if (lZtemp)
                {
	                lZdest += lZtemp;
	                iCount++;
                }
            }
	        lZdest /= iCount * 1000;
	        lZdest += lSensorToEE[iEEindex];
        }
        else
        {
            lZdest = lCoord[2] + ((lZindex - 1) * lPitch);
        }
    }
    else
    {
        lZdest = (iUseScanArg ? lScanCoord[2] : lCoord[2]) + ((lZindex - 1) * lPitch);
    }

    /* Add on offset or stroke distance depending on what the user requested. */
    if (iOffDirArg == 1)
        lZdest += lOffset;
    if (iOffDirArg == 2)
        lZdest += (lOffset-lStroke);

    /* Z position finally computed. */
    lPosition[2] = lZdest;

    /* T position. */
    lPosition[0] = iUseScanArg ? lScanCoord[0] : lCoord[0];

    /*** Move the axes. ***/
    /* If the user wants to retract R first, send the command to move R.
     * The station retract position was already gathered above. */

	if (iRetractArg)
		sprintf(GAcommand, "VRET = 1");
	else
		sprintf(GAcommand, "VRET = 0");
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);

	if (iExtendArg)
		sprintf(GAcommand, "VEXT = 1");
	else
		sprintf(GAcommand, "VEXT = 0");
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);

	ROUnscalePos(RO_AXIS_R, lRHome, lRHomeUncale);
	sprintf(GAcommand, "VREP = %d", lRHomeUncale[1]);	// R home position
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);

	ROUnscalePos(RO_AXIS_R, lExtPos, lExtPosUncale);
	sprintf(GAcommand, "VRXP = %d", lExtPosUncale[1]);	// R extend position
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);

	ROUnscalePos(RO_AXIS_T, lPosition, lPositionUncale);
	sprintf(GAcommand, "VTPO = %d", lPositionUncale[0]);	// T position
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);

	ROUnscalePos(RO_AXIS_Z, lPosition, lPositionUncale);
	sprintf(GAcommand, "VZPO = %d", lPositionUncale[2]);	// Z position
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);

	sprintf(GAcommand, "XQ#MTCR");	// MTCR galil macro call
	rc = GASendDMCCommand(ghDMC, GAcommand, ReturnBuffer, MAXGASTR);
    return rc;


}


/****************************************************************
 *
 * Function:    ROMoveAllOrAxisDirect
 *
 * Abstract:    Move to station, all axes or one axis only.
 *      Combines ROMoveDirect and ROMoveAxisDirect. CODE-OPTIMIZATION: CK.
 *
 * Parameters:
 *      ulAxesArg       (in) The axes to move.
 *      iStnArg         (in) Station to move to (A-Z or a-z)
 *      iOffDirArg      (in) 0   don't use Z offset
 *                           1   add Z offset (go above wafer)
 *                           2   add Z offset - lStroke (go below wafer)
 *      iFlagArg        (in) 0   one axis only
 *                           1   all axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveAllOrAxisDirect(unsigned long ulAxisArg, int iStnArg, int iOffDirArg, int iFlagArg)
{
    long    lStroke, lZindex, lPitch, lOffset;  /* Station parameters. */
    int     iEEindex;
    long    lPosTemp;
    long    lPosition[8];

    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    /* Stroke direction should be between 0 and 2. */
    if (iOffDirArg < 0 || iOffDirArg > 2)
        return FAILURE;

    /* Get coordinates T, R, Z. */
    if (FIOGetStnCoord(iStnArg, 0, &lPosition[0]) == FAILURE)
        return FAILURE;
    if (FIOGetStnCoord(iStnArg, 1, &lPosition[1]) == FAILURE)
        return FAILURE;
    if (FIOGetStnCoord(iStnArg, 2, &lPosition[2]) == FAILURE)
        return FAILURE;

    /* Prepare for W, just in case for dual arm */
    lPosition[3] = lPosition[1];

    if (FIOGetStnEEindex(iStnArg, &iEEindex) == FAILURE)
        return FAILURE;

    /* Also prepare for the cases of Track, Flipper while checking axis validity */
    if (ulAxisArg == RO_AXIS_W) /* it can be DualArm, Flipper, Track */
    {
	    if (ROGetSpecialAxis(RO_DUAL_ARM)) /* dual arm */
        {
            /* station's EE# must match, otherwise deny the request */
            if(iEEindex == 0)
                return FAILURE;
        }
        else if (ROGetSpecialAxis(RO_FLIPPER)) /* flipper */
        {
            /* get station's flipper position */
            if (FIOGetStnVals(iStnArg, FLP_COORD, &lPosTemp) == FAILURE)
                return FAILURE;
            lPosition[3] = lPosTemp;
        }
        else if (ulAxisArg == ROGetSpecialAxis(RO_TRACK)) /* track in robot&track system */
        {
            /* get station's track position */
            if (FIOGetStnVals(iStnArg, TRACK_POS, &lPosTemp) == FAILURE)
                return FAILURE;
            lPosition[3] = lPosTemp;
        }
        else /* W-axis without any DBM, Flipper, Track, must be denied. */
            return FAILURE;
    }
    else if (ulAxisArg == RO_AXIS_w) /* track in integrated system */
    {
        /* get station's track position */
        if (ulAxisArg == ROGetSpecialAxis(RO_TRACK)) /* track in robot&track system */
        {
            if (FIOGetStnVals(iStnArg, TRACK_POS, &lPosTemp) == FAILURE)
                return FAILURE;
            lPosition[3] = lPosTemp;
        }
        else /* w-axis without track? it must be denied. */
            return FAILURE;
    }
    else if (ulAxisArg == RO_AXIS_R && iEEindex != 0 && ROGetSpecialAxis(RO_DUAL_ARM))
        return FAILURE; /* station taught with W, cannot move R */

    /* Get station parameters. */
    if (FIOGetStnVals(iStnArg, PITCH, &lPitch) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, Z_INDEX, &lZindex) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, STROKE, &lStroke) == FAILURE)
        return FAILURE;
    if (FIOGetStnVals(iStnArg, OFFSET, &lOffset) == FAILURE)
        return FAILURE;

    /* The following change was made to convert a 10,000ths of an inch standard
     * to its mil real-world value. The Brooks/Novellus micron standard doesn't need it. */
    if (~iEmulator & DFEMULATORB)
    {
        lPitch /= 10;
    }

    /* Set the destination Z height. */
    lPosition[2] += ((lZindex - 1) * lPitch);
    if (iOffDirArg == 1)
        lPosition[2] += lOffset;
    if (iOffDirArg == 2)
        lPosition[2] += (lOffset-lStroke);

    /* All axes. Axes checking is done in the actual move function. */
    if (iFlagArg)
    {
        /* Only one condition is satisfied here:
         * that is, all axes with W-axis to move. */
        if ((iEEindex == 1) && (ROGetSpecialAxis(RO_DUAL_ARM)))
        {
            /* we cannot issue TZ&W axes command simultaneously
             * TZ and W must be separated, otherwise, ValidAxis returns FAILURE */
            ulAxisArg = RO_AXIS_T|RO_AXIS_Z;
            /* Move the axes T&Z. */
            if (ROMoveToAbs(ulAxisArg, lPosition) == FAILURE)
                return FAILURE;
 //           ulAxisArg = RO_AXIS_W;
            /* Move the axis W. */
 //           if (ROMoveToAbs(ulAxisArg, lPosition) == FAILURE)
 //               return FAILURE;
            return SUCCESS;
        }
    }

    /* The rest conditions are satisfied here:
     * that is, (1) one axis only, (2) all axes (no W-axis) */
    if (ROMoveToAbs(ulAxisArg, lPosition) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROMoveDirect
 *
 * Abstract:    Move to station, all axes simultaneously with
 *      optional Z offset. Start by collecting all relevant
 *      information on the station. Then calculate the Z position.
 *      And finish by initiating the move.
 *
 * Parameters:
 *      iStnArg         (in) Station to move to (A-Z or a-z)
 *      iOffDirArg      (in) 0   don't use Z offset
 *                           1   add Z offset (go above wafer)
 *                           2   add Z offset - lStroke (go below wafer)
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveDirect(int iStnArg, int iOffDirArg)
{
    /* all axes: 4th argument = 1 */
    return (ROMoveAllOrAxisDirect(ulAxisALLRbt, iStnArg, iOffDirArg, 1));
}


/****************************************************************
 *
 * Function:    ROMoveAxisDirect
 *
 * Abstract:    Move to station, one specified axis only.
 *      Start by collecting all relevant information on the station.
 *      Then calculate the Z position. And finish by initiating the move.
 *
 * Parameters:
 *      ulAxesArg       (in) The axes to move.
 *      iStnArg         (in) Station to move to (A-Z or a-z)
 *      iOffDirArg      (in) 0   don't use Z offset
 *                           1   add Z offset (go above wafer)
 *                           2   add Z offset - lStroke (go below wafer)
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveAxisDirect(unsigned long ulAxisArg, int iStnArg, int iOffDirArg)
{
    long lWhichInx;
    long lPosition[8];

    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    if (FIOGetStnVals(iStnArg, TRACK_POS, &lWhichInx) == FAILURE)
        return FAILURE;
    if ((ulAxisArg == ROGetSpecialAxis(RO_INDEXER_Z1)) ||
        (ulAxisArg == ROGetSpecialAxis(RO_INDEXER_Z2)))
    {
        if (((ulAxisArg == ROGetSpecialAxis(RO_INDEXER_Z1)) && (lWhichInx != 0)) ||
            ((ulAxisArg == ROGetSpecialAxis(RO_INDEXER_Z2)) && (lWhichInx != 1)))
            return FAILURE;
        if (FIOGetStnlscsi(iStnArg, 2, &lPosition[1]) == FAILURE)
            return FAILURE;
        lPosition[2] = lPosition[1];
        return (ROMoveToAbs(ulAxisArg, lPosition));
    }
    if ((ulAxisArg == ROGetSpecialAxis(RO_INDEXER_T1)) ||
        (ulAxisArg == ROGetSpecialAxis(RO_INDEXER_T2)))
    {
        if (((ulAxisArg == ROGetSpecialAxis(RO_INDEXER_T1)) && (lWhichInx != 0)) ||
            ((ulAxisArg == ROGetSpecialAxis(RO_INDEXER_T2)) && (lWhichInx != 1)))
            return FAILURE;
        if (FIOGetStnlscsi(iStnArg, 3, &lPosition[0]) == FAILURE)
            return FAILURE;
        lPosition[3] = lPosition[0];
        return (ROMoveToAbs(ulAxisArg, lPosition));
    }
    /* Validation to check if motion is possible with the given axis parameters.
     * Axes checking is usually done in the actual move function but we need
     * to use the axes here to collect information. */
    if ((ulAxisArg != RO_AXIS_T) && (ulAxisArg != RO_AXIS_R) &&
        (ulAxisArg != RO_AXIS_Z) && (ulAxisArg != RO_AXIS_W) &&
        (ulAxisArg != RO_AXIS_w))
        return FAILURE;

    /* one axis only: 4th argument = 0 */
    return (ROMoveAllOrAxisDirect(ulAxisArg, iStnArg, iOffDirArg, 0));
}


/****************************************************************
 *
 * Function:    ROMoveStroke
 *
 * Abstract:    Move Z up or down by station's stroke distance.
 *      Start by collecting all relevant information on the station.
 *      Then calculate the Z position. And finish by initiating the move.
 *
 * Parameters:
 *      iStnArg         (in) Station to move to (A-Z or a-z)
 *      iStrokeDirArg   (in) +1  move up
 *                           -1  move down
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveStroke(int iStnArg, int iStrokeDirArg)
{
    unsigned long ulEquipeAxis = RO_AXIS_Z;     /* Z axis only */
    long    lStroke;                            /* Station parameters. */
    long    lPosition[8];

    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    /* Stroke direction should be between -1 and 1. */
    if (iStrokeDirArg < -1 || iStrokeDirArg > 1)
        return FAILURE;

    /* If Stroke direction is 0, do nothing. */
    if (iStrokeDirArg == 0)
        return SUCCESS;

    /* Get station parameters. */
    if (FIOGetStnVals(iStnArg, STROKE, &lStroke) == FAILURE)
        return FAILURE;

    /* Set Z axis relative move/stroke distance. */
    lPosition[2] = iStrokeDirArg * lStroke;

    /* Move the Z axis. */
    if (ROMoveToRel(ulEquipeAxis, lPosition) == FAILURE)
        return FAILURE;

    return SUCCESS;

}


/****************************************************************
 *
 * Function:    RORetractR
 *
 * Abstract:    This routine retracts R/W to the position passed in.
 *      It sets the R/W position and initiates the move.
 *
 * Parameters:
 *      lPosArg         (in) Position to retract R
 *      iRWAxisFlag     (in) 0=R-axis, 1=W-axis
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int RORetractR(long lPosArg, int iRWAxisFlag)
{
    unsigned long ulEquipeAxis;
    long lPosition[8];

    if(iRWAxisFlag && ROGetSpecialAxis(RO_DUAL_ARM))
    {
        ulEquipeAxis = RO_AXIS_W;   /* W-axis only */
        lPosition[3] = lPosArg;
    }
    else
    {
        ulEquipeAxis = RO_AXIS_R;   /* R-axis only */
        lPosition[1] = lPosArg;
    }

    /* Move the R or W axis. */
    if (ROMoveToAbs(ulEquipeAxis, lPosition) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/***************************************************************************
 *
 * Function Name:   ROSendMoveToHSorIP
 *
 * Description:     Moves specified axis to the Home Switch or Index pulse
 *
 * Parameter:       ulAxisArg       (in) The axis to move
 *                  iFlagArg        (in) 0=Home Switch, 1=Index Pulse
 *                  iResetParsFlagArg
 *                  iGearingFlagArg  indicates whether we want t use gearing or not,
 *                                   i.e. Homing sequence on VAC514 doesn't use gearing
 *
 * Returns:         SUCCESS or FAILURE.
 *
 ***************************************************************************/
int ROSendMoveToHSorIP(ULONG ulAxisArg, int iFlagArg, int iResetParsFlagArg, int iGearingFlagArg)
{
    int iReturn, iCounter, iCardNum, iFileType;
    unsigned uGalilAxes;
    long laHomeSpeed[8]={0,0,0,0,0,0,0,0};
    long laHomeAccel[8]={0,0,0,0,0,0,0,0};
    long laHomeDecel[8]={0,0,0,0,0,0,0,0};
    long laSCurves[8]={0,0,0,0,0,0,0,0};

    /* Validation to check if homing is possible with the given axis parameters. */
    if (ROValidAxis(ulAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;
    /* Return error if servo off or motion not complete on the requested axes. */
    if(ulAxisArg & (ulServoFlag | ~ulAMFlag))
        return FAILURE;

    // On VAC514, T and R are always geared, thus check for motion flag on both
    if((iDefineFlag & DFVAC514) && iGearingFlagArg)
    {
        if(ulAxisArg & RO_AXIS_R)
        {
            if (RO_AXIS_T & (ulServoFlag | ~ulAMFlag))
                return FAILURE;
        }

        if(ulAxisArg & RO_AXIS_T)
        {
            if (RO_AXIS_R & (ulServoFlag | ~ulAMFlag))
                return FAILURE;
        }
    }

    // Get speeds and accelerations. These rather be very slow like during homing.
    if(iResetParsFlagArg)
    {
        if(ROGetParameter(TRUE, ulAxisArg, laHomeSpeed, HOME_SPEED) == FAILURE)
            return FAILURE;
        if(ROGetParameter(TRUE, ulAxisArg, laHomeAccel, HOME_ACCEL) == FAILURE)
            return FAILURE;
        for(iCounter = 0; iCounter < 4; iCounter++) laHomeDecel[iCounter] = laHomeAccel[iCounter];

        // Set speeds and accelerations.
        if(ROSetParameter(FALSE, ulAxisArg, laHomeSpeed, OPERATIONAL_SPEED) == FAILURE)
            return FAILURE;
        if(ROSetParameter(FALSE, ulAxisArg, laHomeAccel, OPERATIONAL_ACCEL) == FAILURE)
            return FAILURE;
        if(ROSetParameter(FALSE, ulAxisArg, laHomeDecel, OPERATIONAL_DECEL) == FAILURE)
            return FAILURE;
    }

    /* Also turn off S-curve profiling. */
//    if(ROEnableSCurveProfile(ulAxisArg, laSCurves)==FAILURE)
//        return FAILURE;

    switch(ulAxisArg)
    {
        case RO_AXIS_T :
            //Set appropriate gearing for the VAC514 robots if required
            if((iDefineFlag & DFVAC514) && iGearingFlagArg)
            {
                if(ROElectronicGearingOFF() == FAILURE)
                    return FAILURE;

                if(ROElectronicGearingON('X', 'Y', VAC514_T_GEARING_RATIO, FALSE) == FAILURE)
                    return FAILURE;
            }
        case RO_AXIS_t :
            if(iFlagArg)
				iReturn=GASendReceiveGalil(iCardNum, (char *)"FIX\r",  cpNull);
            else
				iReturn=GASendReceiveGalil(iCardNum, (char *)"FEX\r", cpNull);
            if (iReturn == SUCCESS)
                iReturn=GASendReceiveGalil(iCardNum, (char *)"BGX\r", cpNull);
            break;
        case RO_AXIS_R :
            //Set appropriate gearing for the VAC514 robots if required
            if((iDefineFlag & DFVAC514) && iGearingFlagArg)
            {
                if(ROElectronicGearingOFF() == FAILURE)
                    return FAILURE;

                if(ROElectronicGearingON('Y', 'X', VAC514_R_GEARING_RATIO, FALSE) == FAILURE)
                    return FAILURE;
            }
        case RO_AXIS_r :
            if(iFlagArg)
				iReturn=GASendReceiveGalil(iCardNum, (char *)"FIY\r", cpNull);
            else
				iReturn=GASendReceiveGalil(iCardNum, (char  *)"FEY\r", cpNull);
            if (iReturn == SUCCESS)
				iReturn=GASendReceiveGalil(iCardNum, (char  *)"BGY\r", cpNull);
            break;
        case RO_AXIS_Z :
        case RO_AXIS_z :
            if(iFlagArg)
				iReturn=GASendReceiveGalil(iCardNum, (char  *)"FIZ\r", cpNull);
            else
				iReturn=GASendReceiveGalil(iCardNum, (char  *)"FEZ\r", cpNull);
            if (iReturn == SUCCESS)
				iReturn=GASendReceiveGalil(iCardNum, (char  *)"BGZ\r", cpNull);
            break;
        case RO_AXIS_W :
        case RO_AXIS_w :
            if(iFlagArg)
				iReturn=GASendReceiveGalil(iCardNum, (char *)"FIW\r", cpNull);
            else
				iReturn=GASendReceiveGalil(iCardNum, (char *)"FEW\r", cpNull);
            if (iReturn == SUCCESS)
				iReturn=GASendReceiveGalil(iCardNum, (char *)"BGW\r", cpNull);
            break;
        default:
            iReturn = FAILURE;
    }
    /* If everything was successful, indicate that the axis is in motion. */
    if (!GAGetGalilErrorFlag() && iReturn != FAILURE)
    {
        ulAMFlag &= ~ulAxisArg;
    }

    return iReturn;
}


/***************************************************************************
 *
 * Function Name:   ROSendMoveToHomeSwitch
 *
 * Description:     Moves specified axis to the Home Switch
 *
 * Parameter:       ulAxisArg   (in) The axis to move
 *
 * Returns:         SUCCESS or FAILURE.
 *
 ***************************************************************************/
int ROSendMoveToHomeSwitch(ULONG ulAxisArg)
{
    return (ROSendMoveToHSorIP(ulAxisArg, 0, 0, TRUE));
}

/***************************************************************************
 *
 * Function Name:   ROSendMoveToIndexPulse
 *
 * Description:     Moves specified axis to the Index pulse
 *
 * Parameter:       ulAxisArg       (in) The axis to move
 *
 * Returns:         SUCCESS or FAILURE.
 *
 ***************************************************************************/
int ROSendMoveToIndexPulse(ULONG ulAxisArg)
{
    return (ROSendMoveToHSorIP(ulAxisArg, 1, 0, FALSE));
}
/***************************************************************************
 *
 * Function Name:   ROSetSlowHomingSpeed
 *
 * Description:     Function sets the low hardcoded speed for the second phase of VAC514 homing,
 *                  it is called from action buffer
 *
 * Parameter:       ulAxisArg       (in) The axis to operate on
 *
 * Returns:         SUCCESS or FAILURE.
 *
 ***************************************************************************/
int ROSetSlowHomingSpeed(unsigned long ulAxisArg)
{
    long laHomeSpeed[8];
    long laHomeAccel[8];
    long laHomeDecel[8];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    if(ROValidAxis(ulAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    if (ROGetParameter(FALSE, ulAxisArg, laHomeSpeed, OPERATIONAL_SPEED) == FAILURE)
        return FAILURE;
    if (ROGetParameter(FALSE, ulAxisArg, laHomeAccel, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;
    if (ROGetParameter(FALSE, ulAxisArg, laHomeDecel, OPERATIONAL_DECEL) == FAILURE)
        return FAILURE;

    if(ulAxisArg == RO_AXIS_T)
    {
        laHomeSpeed[0] = (long)HOMING_SPEED_T;
        laHomeAccel[0] = laHomeDecel[0] = (long)HOMING_ACCEL_T;
    }
    else if(ulAxisArg == RO_AXIS_R)
    {
        laHomeSpeed[1] = (long)HOMING_SPEED_R;
        laHomeAccel[1] = laHomeDecel[1] = (long)HOMING_ACCEL_R;
    }
    else
        return FAILURE;

    if (ROSetParameter(FALSE, ulAxisArg, laHomeSpeed, OPERATIONAL_SPEED) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulAxisArg, laHomeAccel, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulAxisArg, laHomeDecel, OPERATIONAL_DECEL) == FAILURE)
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 *
 * Function Name:   ROSetJGSpeed
 *
 * Description:     Function executes the JG command for the second phase of VAC514 homing,
 *                  it is called from action buffer. It needs to be included in the sequence because the FI
 *                  GALIL command direction depends on the sign setting of JG command
 *
 * Parameter:       ulAxisArg       (in) The axis to move
 *
 * Returns:         SUCCESS or FAILURE.
 *
 ***************************************************************************/
int ROSetJGSpeed(unsigned long ulAxisArg)
{
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    long laValue[8]={0,0,0,0,0,0,0,0};

    if(ROValidAxis(ulAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    //So far function can be used only for the T and R axes on VAC514 robots
    if((ulAxisArg != RO_AXIS_T) && (ulAxisArg != RO_AXIS_R) && (ulAxisArg != (RO_AXIS_T | RO_AXIS_R)))
        return FAILURE;

    if(ulAxisArg & RO_AXIS_T)
        laValue[0] = (long)HOMING_JOG_SPEED;
    if(ulAxisArg & RO_AXIS_R)
        laValue[1] = (long)(HOMING_JOG_SPEED * (-1));

    if(GASetValsLongDefined(iCardNum, JOG_SPEED, uGalilAxes, laValue) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


// ROSendRQ is faster data recording for motion profile
int ROSendRQ(unsigned long ulAxisArg, long lFlagArg)
{
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    int rc;
    char RC0command[10] = "RC 0";
    char RCTcommand[10] = "XQ#XRQT";
    char RCRcommand[10] = "XQ#XRQR";
    char RCZcommand[10] = "XQ#XRQZ";
    char RCWcommand[10] = "XQ#XRQW";
    char RCtcommand[10] = "XQ#XRQt";
    char RCrcommand[10] = "XQ#XRQr";
    char RCzcommand[10] = "XQ#XRQz";
    char RCwcommand[10] = "XQ#XRQw";
    char ReturnBuffer[MAXGASTR];


    if(lFlagArg == 0)
    {
	rc = GASendDMCCommand(ghDMC, RC0command, ReturnBuffer, MAXGASTR);
//printf("RC0 return=%d\n",rc);
	giXRCFlag = 0; // notifies RC is no more in progress
	return SUCCESS;
    }

    // if any RC is in progress, must return failure
    if(giXRCFlag)
	return FAILURE;
	
//    if(ROValidAxis(ulAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
//	return FAILURE;

    switch(ulAxisArg)
    {
	case 1: //RO_AXIS_T:
		rc = GASendDMCCommand(ghDMC, RCTcommand, ReturnBuffer, MAXGASTR);
		break;
	case 2: //RO_AXIS_R:
		rc = GASendDMCCommand(ghDMC, RCRcommand, ReturnBuffer, MAXGASTR);
		break;
	case 4: //RO_AXIS_Z:
		rc = GASendDMCCommand(ghDMC, RCZcommand, ReturnBuffer, MAXGASTR);
//printf("axis=%ld flag=%ld rc=%d\n",ulAxisArg, lFlagArg, rc);
		break;
	case 8: //RO_AXIS_W:
		rc = GASendDMCCommand(ghDMC, RCWcommand, ReturnBuffer, MAXGASTR);
		break;
	case 16: //RO_AXIS_t:
		rc = GASendDMCCommand(ghDMC, RCtcommand, ReturnBuffer, MAXGASTR);
		break;
	case 32: //RO_AXIS_r:
		rc = GASendDMCCommand(ghDMC, RCrcommand, ReturnBuffer, MAXGASTR);
		break;
	case 64: //RO_AXIS_z:
		rc = GASendDMCCommand(ghDMC, RCzcommand, ReturnBuffer, MAXGASTR);
		break;
	case 128: //RO_AXIS_w:
		rc = GASendDMCCommand(ghDMC, RCwcommand, ReturnBuffer, MAXGASTR);
		break;
	default:
		return FAILURE;
    }

    giXRCFlag = 1;
    giRCStatusToWrite =  1;

    return SUCCESS;
}

int ROSendRC(unsigned long ulAxisArg, long lFlagArg)
{
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    int rc;
    char RC0command[10] = "RC 0";
    char RCTcommand[10] = "XQ#XRCT";
    char RCRcommand[10] = "XQ#XRCR";
    char RCZcommand[10] = "XQ#XRCZ";
    char RCWcommand[10] = "XQ#XRCW";
    char RCtcommand[10] = "XQ#XRCt";
    char RCrcommand[10] = "XQ#XRCr";
    char RCzcommand[10] = "XQ#XRCz";
    char RCwcommand[10] = "XQ#XRCw";
    char ReturnBuffer[MAXGASTR];


    if(lFlagArg == 0)
    {
	rc = GASendDMCCommand(ghDMC, RC0command, ReturnBuffer, MAXGASTR);
//printf("RC0 return=%d\n",rc);
	giXRCFlag = 0; // notifies RC is no more in progress
	return SUCCESS;
    }

    // if any RC is in progress, must return failure
    if(giXRCFlag)
	return FAILURE;
	
//    if(ROValidAxis(ulAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
//	return FAILURE;

    switch(ulAxisArg)
    {
	case 1: //RO_AXIS_T:
		rc = GASendDMCCommand(ghDMC, RCTcommand, ReturnBuffer, MAXGASTR);
		break;
	case 2: //RO_AXIS_R:
		rc = GASendDMCCommand(ghDMC, RCRcommand, ReturnBuffer, MAXGASTR);
		break;
	case 4: //RO_AXIS_Z:
		rc = GASendDMCCommand(ghDMC, RCZcommand, ReturnBuffer, MAXGASTR);
//printf("axis=%ld flag=%ld rc=%d\n",ulAxisArg, lFlagArg, rc);
		break;
	case 8: //RO_AXIS_W:
		rc = GASendDMCCommand(ghDMC, RCWcommand, ReturnBuffer, MAXGASTR);
		break;
	case 16: //RO_AXIS_t:
		rc = GASendDMCCommand(ghDMC, RCtcommand, ReturnBuffer, MAXGASTR);
		break;
	case 32: //RO_AXIS_r:
		rc = GASendDMCCommand(ghDMC, RCrcommand, ReturnBuffer, MAXGASTR);
		break;
	case 64: //RO_AXIS_z:
		rc = GASendDMCCommand(ghDMC, RCzcommand, ReturnBuffer, MAXGASTR);
		break;
	case 128: //RO_AXIS_w:
		rc = GASendDMCCommand(ghDMC, RCwcommand, ReturnBuffer, MAXGASTR);
		break;
	default:
		return FAILURE;
    }

    giXRCFlag = 1;
    giRCStatusToWrite =  1;

    return SUCCESS;
}

int ROSendQU(unsigned long ulAxisArg)
{
    int i;
    int rc;
    char QUTTcommand[20] = "QU TOR[],,,1";
    char QUTPcommand[20] = "QU POS[],,,1";
    char *ReturnBuffer;
    char caCommand[40];
    char caResp[40];
/*
    char QUTcommandT[20] = "QU TTDA[],,,1";
    char QUPcommandT[20] = "QU TPDA[],,,1";
    char QUTcommandR[20] = "QU TORR[],,,1";
    char QUPcommandR[20] = "QU POSR[],,,1";
    char QUTcommandZ[20] = "QU TORZ[],,,1";
    char QUPcommandZ[20] = "QU POSZ[],,,1";
    char QUTcommandW[20] = "QU TORW[],,,1";
    char QUPcommandW[20] = "QU POSW[],,,1";
    char QUTcommandt[20] = "QU TORt[],,,1";
    char QUPcommandt[20] = "QU POSt[],,,1";
    char QUTcommandr[20] = "QU TORr[],,,1";
    char QUPcommandr[20] = "QU POSr[],,,1";
    char QUTcommandz[20] = "QU TORz[],,,1";
    char QUPcommandz[20] = "QU POSz[],,,1";
    char QUTcommandw[20] = "QU TORw[],,,1";
    char QUPcommandw[20] = "QU POSw[],,,1";
*/

    switch(ulAxisArg)
    {
	case RO_AXIS_T:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		TTTData[i] = 0;
		TTPData[i] = 0;
		TTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, TTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, TTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_T, TTPData[i], &TTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_R:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		RTTData[i] = 0;
		RTPData[i] = 0;
		RTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, RTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, RTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_R, RTPData[i], &RTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_Z:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		ZTTData[i] = 0;
		ZTPData[i] = 0;
		ZTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, ZTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, ZTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_Z, ZTPData[i], &ZTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_W:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		WTTData[i] = 0;
		WTPData[i] = 0;
		WTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, WTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, WTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_W, WTPData[i], &WTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_t:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		tTTData[i] = 0;
		tTPData[i] = 0;
		tTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, tTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, tTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_t, tTPData[i], &tTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_r:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		rTTData[i] = 0;
		rTPData[i] = 0;
		rTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, rTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, rTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_r, rTPData[i], &rTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_z:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		zTTData[i] = 0;
		zTPData[i] = 0;
		zTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, zTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, zTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_z, zTPData[i], &zTPDataEU[i]);
	    }
	    break;
	case RO_AXIS_w:
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
		wTTData[i] = 0;
		wTPData[i] = 0;
		wTPDataEU[i] = 0;
	    }
	    rc = GASendDMCCommand(ghDMC, QUTTcommand, ReturnTT, TTARRAYSIZE*12);
	    rc = GASendDMCCommand(ghDMC, QUTPcommand, ReturnTP, TTARRAYSIZE*12);
	    ConvertStrToi(ReturnTT, TTARRAYSIZE, wTTData);
	    ConvertStrToi(ReturnTP, TTARRAYSIZE, wTPData);
	    for(i=0; i<TTARRAYSIZE; ++i)
	    {
	    	ROEncoderToEU(RO_AXIS_w, wTPData[i], &wTPDataEU[i]);
	    }
	    break;
	default:
		return FAILURE;

    }


    return SUCCESS;

}
		
void ROClearFrictionData(void)
{
    int i;

    for(i=0; i<TTARRAYSIZE; ++i)
    {
	TTTData[i] = 0; TTPData[i] = 0; TTPDataEU[i] = 0;
	RTTData[i] = 0; RTPData[i] = 0; RTPDataEU[i] = 0;
	ZTTData[i] = 0; ZTPData[i] = 0; ZTPDataEU[i] = 0;
	WTTData[i] = 0; WTPData[i] = 0; WTPDataEU[i] = 0;
	tTTData[i] = 0; tTPData[i] = 0; tTPDataEU[i] = 0;
	rTTData[i] = 0; rTPData[i] = 0; rTPDataEU[i] = 0;
	zTTData[i] = 0; zTPData[i] = 0; zTPDataEU[i] = 0;
	wTTData[i] = 0; wTPData[i] = 0; wTPDataEU[i] = 0;
    }
}


int RODumpRC(unsigned long ulAxisArg, long lFirstArg, long lLastArg)
{
    int i, iCardNum, iFileType;
    int rc;
    unsigned uGalilAxes;
    int *TTBuffer;
    int *TPBuffer;
    char caString[40];
    int iCmdPortNumber;

//    if(ROValidAxis(ulAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
//	return FAILURE;

    switch(ulAxisArg)
    {
	case 1: //RO_AXIS_T:
		TTBuffer = TTTData;
		TPBuffer = TTPData;
		break;
	case 2: //RO_AXIS_R:
		TTBuffer = RTTData;
		TPBuffer = RTPData;
		break;
	case 4: //RO_AXIS_Z:
//printf("DM axis=%ld first=%ld last=%ld\n",ulAxisArg, lFirstArg, lLastArg);
		TTBuffer = ZTTData;
		TPBuffer = ZTPData;
		break;
	case 8: //RO_AXIS_W:
		TTBuffer = WTTData;
		TPBuffer = WTPData;
		break;
	case 16: //RO_AXIS_t:
		TTBuffer = tTTData;
		TPBuffer = tTPData;
		break;
	case 32: //RO_AXIS_r:
		TTBuffer = rTTData;
		TPBuffer = rTPData;
		break;
	case 64: //RO_AXIS_z:
		TTBuffer = zTTData;
		TPBuffer = zTPData;
		break;
	case 128: //RO_AXIS_w:
		TTBuffer = wTTData;
		TPBuffer = wTPData;
		break;
	default:
		return FAILURE;
    }

    if(lFirstArg < 0 || lFirstArg > TTARRAYSIZE-1 || lLastArg < 0 || lLastArg > TTARRAYSIZE-1)
	return FAILURE;

    iCmdPortNumber = SERGetCmdPort();

    for (i = (int)lFirstArg; i <= (int)lLastArg; ++i)
    {
	sprintf(caString, "%d, %d, %d\n\r", i, TTBuffer[i], TPBuffer[i]);
        if( SERPutsTxBuff(iCmdPortNumber, caString) == FAILURE )
            return FAILURE;
    }
    return SUCCESS;
}

//
// Read RC data from a file
//
int ROReadRC()
{
    FILE *iFP;
    int iTotalInt;

    iFP = fopen(RCFILENAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "RC file Read Open Error " );
    }
    else
    {
        iTotalInt = fread( TTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 1 Read Error " );
	}
        iTotalInt = fread( TTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 2 Read Error " );
	}
        iTotalInt = fread( RTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 3 Read Error " );
	}
        iTotalInt = fread( RTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 4 Read Error " );
	}
        iTotalInt = fread( ZTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 5 Read Error " );
	}
        iTotalInt = fread( ZTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 6 Read Error " );
	}
        iTotalInt = fread( WTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 7 Read Error " );
	}
        iTotalInt = fread( WTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 8 Read Error " );
	}
        iTotalInt = fread( tTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 9 Read Error " );
	}
        iTotalInt = fread( tTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 10 Read Error " );
	}
        iTotalInt = fread( rTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 11 Read Error " );
	}
        iTotalInt = fread( rTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 12 Read Error " );
	}
        iTotalInt = fread( zTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 13 Read Error " );
	}
        iTotalInt = fread( zTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 14 Read Error " );
	}
        iTotalInt = fread( wTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 15 Read Error " );
	}
        iTotalInt = fread( wTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 16 Read Error " );
	}
        iTotalInt = fread( TDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 17 Read Error " );
	}
        iTotalInt = fread( RDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 18 Read Error " );
	}
        iTotalInt = fread( ZDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 19 Read Error " );
	}
        iTotalInt = fread( WDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 20 Read Error " );
	}
        iTotalInt = fread( tDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 21 Read Error " );
	}
        iTotalInt = fread( rDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 22 Read Error " );
	}
        iTotalInt = fread( zDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 23 Read Error " );
	}
        iTotalInt = fread( wDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 24 Read Error " );
	}
        iTotalInt = fread( TTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 25 Read Error " );
	}
        iTotalInt = fread( RTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 26 Read Error " );
	}
        iTotalInt = fread( ZTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 27 Read Error " );
	}
        iTotalInt = fread( WTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 28 Read Error " );
	}
        iTotalInt = fread( tTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 29 Read Error " );
	}
        iTotalInt = fread( rTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 30 Read Error " );
	}
        iTotalInt = fread( zTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 31 Read Error " );
	}
        iTotalInt = fread( wTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 32 Read Error " );
	}
        iTotalInt = fread( glMotorResolution, sizeof( long ), 8, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 33 Read Error " );
	}
        iTotalInt = fread( glEncoderResolution, sizeof( long ), 8, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 34 Read Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// RC Data Write to a file
int ROWriteRC()
{
    FILE *iFP;
    int iTotalInt;
    
    iFP = fopen( RCFILENAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "RC File Write Open Error " );
    }
    else
    {
        iTotalInt = fwrite( TTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 1 write Error " );
	}
        iTotalInt = fwrite( TTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 2 write Error " );
	}
        iTotalInt = fwrite( RTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 3 write Error " );
	}
        iTotalInt = fwrite( RTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 4 write Error " );
	}
        iTotalInt = fwrite( ZTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 5 write Error " );
	}
        iTotalInt = fwrite( ZTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 6 write Error " );
	}
        iTotalInt = fwrite( WTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 7 write Error " );
	}
        iTotalInt = fwrite( WTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 8 write Error " );
	}
        iTotalInt = fwrite( tTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 9 write Error " );
	}
        iTotalInt = fwrite( tTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 10 write Error " );
	}
        iTotalInt = fwrite( rTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 11 write Error " );
	}
        iTotalInt = fwrite( rTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 12 write Error " );
	}
        iTotalInt = fwrite( zTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 13 write Error " );
	}
        iTotalInt = fwrite( zTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 14 write Error " );
	}
        iTotalInt = fwrite( wTTData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 15 write Error " );
	}
        iTotalInt = fwrite( wTPData, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 16 write Error " );
	}
        iTotalInt = fwrite( TDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 17 write Error " );
	}
        iTotalInt = fwrite( RDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 18 write Error " );
	}
        iTotalInt = fwrite( ZDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 19 write Error " );
	}
        iTotalInt = fwrite( WDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 20 write Error " );
	}
        iTotalInt = fwrite( tDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 21 write Error " );
	}
        iTotalInt = fwrite( rDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 22 write Error " );
	}
        iTotalInt = fwrite( zDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 23 write Error " );
	}
        iTotalInt = fwrite( wDFT, sizeof( double ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 24 write Error " );
	}
        iTotalInt = fwrite( TTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 25 write Error " );
	}
        iTotalInt = fwrite( RTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 26 write Error " );
	}
        iTotalInt = fwrite( ZTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 27 write Error " );
	}
        iTotalInt = fwrite( WTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 28 write Error " );
	}
        iTotalInt = fwrite( tTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 29 write Error " );
	}
        iTotalInt = fwrite( rTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 30 write Error " );
	}
        iTotalInt = fwrite( zTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 31 write Error " );
	}
        iTotalInt = fwrite( wTPDataEU, sizeof( long ), TTARRAYSIZE, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 32 write Error " );
	}
        iTotalInt = fwrite( glMotorResolution, sizeof( long ), 8, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 33 write Error " );
	}
        iTotalInt = fwrite( glEncoderResolution, sizeof( long ), 8, iFP);
	if( iTotalInt<=0 )
	{
            perror( "RC file 34 write Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

//
// DFT fourier transformation function
//
void DFT_FID(int *iFromArrayArg, int iBeginArg, int iEndArg, int iResArg, int iCoeffArg, double *dToArrayArg)
{
    double PI = 3.14159;

    int iResolution;
    double dScale;

    double dSpectrum[TTARRAYSIZE];
    double dH1[TTARRAYSIZE];
    double dH2[TTARRAYSIZE];
    double dG[TTARRAYSIZE];

    int i, n, s, t, iMaxCoef;

    iResolution = iResArg;
    iMaxCoef = iCoeffArg;

    n = 0;

    dScale = (2 * PI) / iResolution;

    for (i=0; i < (iEndArg - iBeginArg); ++i)
    {
	dG[i] = iFromArrayArg[i + iBeginArg];
    }

    for (s=0; s <= iMaxCoef; ++s)
    {
	dH1[s] = 0;
	dH2[s] = 0;

	for (t=0; t < iResolution; ++t)
	{
	    dH1[s] = dH1[s] + dG[t] * cos(dScale * s * t);
	    dH2[s] = dH2[s] + dG[t] * sin(dScale * s * t);
	}
    }

    for (i=1; i <= iMaxCoef; ++i)
    {
	dSpectrum[i] = sqrt((dH1[i] * dH1[i]) + (dH2[i] * dH2[i]));
	dToArrayArg[i] = (dSpectrum[i] / 256) * (1024 / (iEndArg - iBeginArg));
    }
}

//
// Check system idle
// If system is in idle, process XRC data
//
//int giFrictionTest = 0;	// Friction Test to be performed
//long glFrictionDataReady = 0;	// Friction data ready?
//long glPickCounter = 0;	// wafer pick counter, when it's greater than 2000
//long glIdleTime = 30;		// System Idle time 30 seconds.
//long glMaxWaferPickCounter = 2000; // max wafer pick counter
//int giFrictionAxis = 0;	// Axis tested for friction test
//
void ROCheckIdle(void)
{
    long lCurrentTime;
    long lLapseTime;
    int  iRet, i;
    int iCmdPort;
    int iData;

    // safe position stored in 'x' station
    long lSafePos[8];
    unsigned long ulAxisTrack;

    // current position & speed saved
    long lCurrentPosition[8];
    long lCurrentSpeed[8];
    long lGetSpeed[8];
    long lOperatingSpeed[8]={0,0,0,0,0,0,0,0};
    long lSoftwareNegLimit[8]={0,0,0,0,0,0,0,0};
    long lSoftwarePosLimit[8]={0,0,0,0,0,0,0,0};
    long lNegPos, lPosPos;
    long lPosT, lPosR, lPosZ, lPosTrack, lPost, lPosr, lPosz;
    long lSpeedT, lSpeedR, lSpeedZ, lSpeedTrack, lSpeedt, lSpeedr, lSpeedz;
    long lPosition[8];

    // retract arm position
    long lHome;
    long lCHome[8];

    // just counter to skip this check 
    if(++giCheckIdle < 175) return;

    giCheckIdle = 0;

    // did host send command to perform the test?
    if( !giFrictionTest ) return; // Host didn't request to perform fr test.

    // has # of picks reached the user-defined counter?
    if( glPickCounter < glMaxWaferPickCounter) return; // wafer picks not reached.

    // did host upload the previous data?
    if( glFrictionDataReady ) return; // Host hasn't uploaded the data

    // is the command port COM1 Serial communication?
    iCmdPort = SERGetCmdPort();
    if(iCmdPort == COM2) return;  // Teach Pendant active on COM2

    // is wafer on EE?
    iData = inb( IO_ROBOT_INPUT_F ) & 0x01; // check bit 0 for wafer detection
    if(iData == 0) return;	// wafer detected on EE

    // is safe position station taught? station 'x' (index 49) is safe position
    // station x coordinates must not contain 0
    if( FIOGetStnCoord(49, 0, &lSafePos[0])==FAILURE ||
        FIOGetStnCoord(49, 1, &lSafePos[1])==FAILURE ||
        FIOGetStnCoord(49, 2, &lSafePos[2])==FAILURE ) return;

    if( lSafePos[0]==0 || lSafePos[1]==0 || lSafePos[2]==0) return; // NOT TAUGHT

    // track position
    ulAxisTrack = ROGetSpecialAxis(RO_TRACK);
    if (ulAxisTrack)
    {
	if(giNumOfAxes == 4)
        {
 	    if( FIOGetStnVals(49, TRACK_POS, &lSafePos[3]) == FAILURE ) return;
 	    if(lSafePos[3]==0) return;	// NOT TAUGHT
        }
	else
        {
 	    if( FIOGetStnVals(49, TRACK_POS, &lSafePos[7]) == FAILURE ) return;
 	    if(lSafePos[7]==0) return; 	// NOT TAUGHT
        }
    }

    // does time lapse reach the idle time?
    lCurrentTime = TIRSTime();
    lLapseTime = lCurrentTime - glRobotIdleStart;
    if (lLapseTime < 1000*glIdleTime) return; // this is 30 seconds lapse default.

    // All the conditions are met!!!
    // Proceed to perform friction test
    //
    glPickCounter = 0;

    // Start with saving all the robot parameters

    // Save the current position T, R, Z
    if( ROReadCurrentPosition(0x07, lCurrentPosition) == FAILURE )
        return;
    lPosT = lCurrentPosition[0];
    lPosR = lCurrentPosition[1];
    lPosZ = lCurrentPosition[2];

    // Save the current position Track
    if (giNumOfAxes >= 4 && ulAxisTrack)
    {
	if( ROReadCurrentPosition(ulAxisTrack,lCurrentPosition) == FAILURE )
	    return FAILURE;
	// Save the current speed T, R, Z, W, t, r, z, w
	if(ulAxisTrack == RO_AXIS_W)
	    lPosTrack = lCurrentPosition[3];	// track W axis
	else
	    lPosTrack = lCurrentPosition[7];	// track w axis

    	lCurrentPosition[0] = lPosT;
	lCurrentPosition[1] = lPosR;
	lCurrentPosition[2] = lPosZ;
    }

    // Save the current position t, r, z 
    if (giNumOfAxes > 4)
    {
	if( ROReadCurrentPosition(0x70, lCurrentPosition) == FAILURE )
	    return;
    	lPost = lCurrentPosition[4];
    	lPosr = lCurrentPosition[5];
    	lPosz = lCurrentPosition[6];
    	lCurrentPosition[0] = lPosT;
	lCurrentPosition[1] = lPosR;
	lCurrentPosition[2] = lPosZ;
	lCurrentPosition[7] = lPosTrack;
    }

    // Save the current speed T, R, Z
    if( ROGetParameter(FALSE, 0x07, lCurrentSpeed, OPERATIONAL_SPEED) == FAILURE )
        return;

    lSpeedT = lCurrentSpeed[0];
    lSpeedR = lCurrentSpeed[1];
    lSpeedZ = lCurrentSpeed[2];

    // Save the current speed Track
    if (giNumOfAxes >= 4 && ulAxisTrack)
    {
    	if( ROGetParameter(FALSE, ulAxisTrack, lGetSpeed, OPERATIONAL_SPEED) == FAILURE )
	    return FAILURE;
	// Save the current speed T, R, Z, W, t, r, z, w
	if(ulAxisTrack == RO_AXIS_W)
	    lCurrentSpeed[3] = lGetSpeed[3];	// track W axis
	else
	    lCurrentSpeed[7] = lGetSpeed[7];	// track w axis
    }

    // save the prealigner speed t, r, z
    if (giNumOfAxes > 4)
    {
    	if( ROGetParameter(FALSE, 0x70, lGetSpeed, OPERATIONAL_SPEED) == FAILURE )
	    return;
    	lCurrentSpeed[4] = lGetSpeed[4];
    	lCurrentSpeed[5] = lGetSpeed[5];
    	lCurrentSpeed[6] = lGetSpeed[6];
    }


    RODisableLatch(1);

    if (~giFrictionTestAxis & RO_AXIS_T)
    {
	// MUST GO TO SAFE POSITION
	if (ROGotoSafePos( lSafePos, ulAxisTrack) == FAILURE)
	    return;

	// NOW THE ROBOT IS IN SAFE POSITION
	if (ROFrictionTest(RO_AXIS_T) == FAILURE)
	{
	    giFrictionTestFail = 11;
	    return;
	}

	// NOW Move the robot to its original position
	if(ROGotoPrevPos(RO_AXIS_T, lCurrentPosition, ulAxisTrack, lCurrentSpeed) == FAILURE)
	    return;

        giFrictionTestAxis |= RO_AXIS_T;

    }
    else if (~giFrictionTestAxis & RO_AXIS_R)
    {
	// MUST GO TO SAFE POSITION
	if (ROGotoSafePos( lSafePos, ulAxisTrack) == FAILURE)
	    return;

	// NOW THE ROBOT IS IN SAFE POSITION
	if (ROFrictionTest(RO_AXIS_R) == FAILURE)
	{
	    giFrictionTestFail = 12;
	    return;
	}

	// NOW Move the robot to its original position
	if(ROGotoPrevPos(RO_AXIS_R, lCurrentPosition, ulAxisTrack, lCurrentSpeed) == FAILURE)
	    return;

        giFrictionTestAxis |= RO_AXIS_R;

    }
    else if (~giFrictionTestAxis & RO_AXIS_Z)
    {
	// MUST GO TO SAFE POSITION
	if (ROGotoSafePos( lSafePos, ulAxisTrack) == FAILURE)
	    return;

	// NOW THE ROBOT IS IN SAFE POSITION
	if (ROFrictionTest(RO_AXIS_Z) == FAILURE)
	{
	    giFrictionTestFail = 13;
	    return;
	}

	// NOW Move the robot to its original position
	if(ROGotoPrevPos(RO_AXIS_Z, lCurrentPosition, ulAxisTrack, lCurrentSpeed) == FAILURE)
	    return;

        giFrictionTestAxis |= RO_AXIS_Z;
    }
    else if (ulAxisTrack)
    {
	if(ulAxisTrack == RO_AXIS_W)
	{
	    if(~giFrictionTestAxis & RO_AXIS_W)
	    {
		// MUST GO TO SAFE POSITION
		if (ROGotoSafePos( lSafePos, ulAxisTrack) == FAILURE)
		    return;

		// NOW THE ROBOT IS IN SAFE POSITION
		if (ROFrictionTest(RO_AXIS_W) == FAILURE)
		{
		    giFrictionTestFail = 14;
		    return;
		}

		// NOW Move the robot to its original position
		if(ROGotoPrevPos(RO_AXIS_W, lCurrentPosition, ulAxisTrack, lCurrentSpeed) == FAILURE)
		    return;

        	giFrictionTestAxis |= RO_AXIS_W;
	    }
	}
	else
     	{
	    if(~giFrictionTestAxis & RO_AXIS_w)
	    {
		// MUST GO TO SAFE POSITION
		if (ROGotoSafePos( lSafePos, ulAxisTrack) == FAILURE)
		    return;

		// NOW THE ROBOT IS IN SAFE POSITION
		if (ROFrictionTest(RO_AXIS_w) == FAILURE)
		{
		    giFrictionTestFail = 14;
		    return;
		}

		// NOW Move the robot to its original position
		if(ROGotoPrevPos(RO_AXIS_w, lCurrentPosition, ulAxisTrack, lCurrentSpeed) == FAILURE)
		    return;

        	giFrictionTestAxis |= RO_AXIS_w;
		// ASSUMPTION: 8-AXIS SYSTEM WITH 4TH AXIS EMPTY!!! (DBM NEEDS TO BE TESTED)
		giFrictionTestAxis |= RO_AXIS_W;	// ASSUME THIS AXIS IS EMPTY!!!!
	    }
	}
    }
    else if (giNumOfAxes > 4)
    {
	if(~giFrictionTestAxis & RO_AXIS_t)
	{
	    // NO NEED TO MOVE THE ROBOT TO SAFE POSITION
	    if (ROFrictionTest(RO_AXIS_t) == FAILURE)
	    {
	    	giFrictionTestFail = 14;
	    	return;
	    }

	    if (RORestoreAxis(RO_AXIS_t, lPost, lCurrentSpeed) == FAILURE)
		return;

	    giFrictionTestAxis |= RO_AXIS_t;
	}
	else if (~giFrictionTestAxis & RO_AXIS_r)
	{
	    // NO NEED TO MOVE THE ROBOT TO SAFE POSITION
	    if (ROFrictionTest(RO_AXIS_r) == FAILURE)
	    {
	    	giFrictionTestFail = 15;
	    	return;
	    }

	    if (RORestoreAxis(RO_AXIS_r, lPosr, lCurrentSpeed) == FAILURE)
		return;

	    giFrictionTestAxis |= RO_AXIS_r;
	}
	else if (~giFrictionTestAxis & RO_AXIS_z)
	{
	    // NO NEED TO MOVE THE ROBOT TO SAFE POSITION
	    if (ROFrictionTest(RO_AXIS_z) == FAILURE)
	    {
	    	giFrictionTestFail = 16;
	    	return;
	    }

	    if (RORestoreAxis(RO_AXIS_z, lPosz, lCurrentSpeed) == FAILURE)
		return;

	    giFrictionTestAxis |= RO_AXIS_z;
	}
    }

    // FRICTION TEST DONE.
    // CHECK IF ALL AXES ARE TESTED

    glPickCounter = 0;

    if ( (giNumOfAxes==3) && (giFrictionTestAxis & 0x07) == 0x07)
	glFrictionDataReady = 1;

    if ( (giNumOfAxes==4) && (giFrictionTestAxis & 0x0F) == 0x0F)
	glFrictionDataReady = 1;

    if ( (giNumOfAxes==6) && (giFrictionTestAxis & 0x77) == 0x77)
	glFrictionDataReady = 1;

    if ( (giNumOfAxes==8) && (giFrictionTestAxis & 0xFF) == 0xFF)
	glFrictionDataReady = 1;

    RODisableLatch(0);
}




int ROGotoSafePos( long *lSafePosArg, unsigned long ulAxisTrack )
{
    int i;
    long lCHome[8];
    long lHome;
    long lSafePos[8];

    // 1. RETRACT ARM FIRST

    if( FIOGetParamVals(ROBOTFILE, CUSTOMIZED_HOME, lCHome) == FAILURE )
        return FAILURE;

    lHome = lCHome[1];
    if (RORetractR(lHome, 0) == FAILURE) // retract the radial 
        return FAILURE;
    

    TIDelay(100);
    ROUpdateTS(0);

    // check for R motion complete
    while(~ulAMFlag & RO_AXIS_R)
    {
	TIDelay(100);
	ROUpdateTS(0);

	// if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    // 2. MOVE TO SAFE POSITION, track first, then T-axis
    if (ulAxisTrack)
    {
	for(i=0; i<8; ++i) lSafePos[i] = lSafePosArg[i];
	if (ROMoveToAbs( ulAxisTrack, lSafePos )==FAILURE)
	    return FAILURE;
    }

    for(i=0; i<8; ++i) lSafePos[i] = lSafePosArg[i];
    if (ROMoveToAbs( RO_AXIS_T, lSafePos) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for R motion complete
    while((~ulAMFlag & RO_AXIS_T) || (~ulAMFlag & ulAxisTrack))
    {
	TIDelay(500);
	ROUpdateTS(0);

	// if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }


    // NOW THE ROBOT IS IN SAFE POSITION
    return SUCCESS;

}

int ROGotoPrevPos(unsigned long ulAxisArg, long *lPositionArg, unsigned long ulAxisTrack, long *lCurrentSpeedArg)
{
    int i;
    long lCHome[8];
    long lHome;

    if( ROSetParameter( FALSE, ulAxisArg, lCurrentSpeedArg, OPERATIONAL_SPEED ) == FAILURE)
        return FAILURE;


    // 1. RETRACT ARM FIRST
    if( FIOGetParamVals(ROBOTFILE, CUSTOMIZED_HOME, lCHome) == FAILURE )
        return FAILURE;

    lHome = lCHome[1];
    if (RORetractR(lHome, 0) == FAILURE) // retract the radial 
        return FAILURE;

    TIDelay(100);
    ROUpdateTS(0);

    // check for R motion complete
    while(~ulAMFlag & RO_AXIS_R)
    {
	TIDelay(100);
	ROUpdateTS(0);

	// if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    // MOVE TO original POSITION, track first, then T-axis
    if (ulAxisTrack)
    {
   	for(i=0;i<8;++i) lCHome[i] = lPositionArg[i];
   	if (ROMoveToAbs( ulAxisTrack, lCHome )==FAILURE)
	    return FAILURE;
    }

    for(i=0;i<8;++i) lCHome[i] = lPositionArg[i];
    if (ROMoveToAbs( RO_AXIS_T, lCHome) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for T and track motion complete
    while((~ulAMFlag & RO_AXIS_T) || (~ulAMFlag & ulAxisTrack))
    {
	TIDelay(500);
	ROUpdateTS(0);

    	// if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    // Finally the R axis original position
    for(i=0;i<8;++i) lCHome[i] = lPositionArg[i];
    if (ROMoveToAbs( RO_AXIS_R, lCHome) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for R motion complete
    while( ~ulAMFlag & RO_AXIS_R )
    {
   	TIDelay(500);
	ROUpdateTS(0);

    	// if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }
	
}


int ROFrictionTest(unsigned long ulAxisArg)
{
    long lSoftwareNegLimit[8];
    long lSoftwarePosLimit[8];
    long lEncRes[8];
    long lNegPos;
    long lPosPos;
    long lFrictionSpeed;
    long lOperatingSpeed[8];
    long lPosition[8], lPositionPos[8];
    long lTimeDelay;

    // use these for index signal finding
    long lIndexSpeed[8] = {20,20,20,20,20,20,20,20};
    long lPositionMid[8];
    long lPosMid;
    int iLatch, rc;
    long lTemp;
    char ReturnBuffer[MAXGASTR];
    int  iOutputLSave;
    int  iOutputL;
    long lFirstSignal;
    long lSecondSignal;
    int  iAxisIndex;
    long lMotorRes;

    char caALcommand[40];
    char caMGALcommand[40];
    char caMGRLcommand[40];

    if( ROGetParameter(TRUE,ulAxisArg,lSoftwareNegLimit,SOFT_NEG_LIMIT) == FAILURE )
        return FAILURE;
    if( ROGetParameter(TRUE,ulAxisArg,lSoftwarePosLimit,SOFT_POS_LIMIT) == FAILURE )
	return FAILURE;
    if( ROGetParameter(TRUE,ulAxisArg,lEncRes,ENCODER_RESOLUTION) == FAILURE )
	return FAILURE;

//printf("encRes=%d %d %d %d %d\n",lEncRes[0],lEncRes[1],lEncRes[2],lEncRes[3]);

    // RCSI x,8 has time delay value
    if( FIOGetStnVals(49, CROSS_SLOT_THRESH, &lTimeDelay) == FAILURE )
	return FAILURE;

    switch(ulAxisArg)
    {
	case RO_AXIS_T:
	    iAxisIndex = 0;
	    sprintf(caALcommand,"ALA");
	    sprintf(caMGALcommand,"MG _ALA");
	    sprintf(caMGRLcommand,"MG _RLA");

	    if( FIOGetStnVals(49, START_ANGLE, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, START_ANGLE, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, START_ANGLE, &lPosPos) == FAILURE )
		return FAILURE;
	    // set the pos -/+ manually
	    //lNegPos = 10000;
	    //lPosPos = 27000;
	    lOperatingSpeed[0] = lFrictionSpeed;
	    lPosition[0] =  lNegPos;
	    lPositionPos[0] =  lPosPos;
	    //lPosMid = lNegPos + 1500;
	    //lPositionMid[0] = lPosMid;
  	    glEncoderResolution[0] = lEncRes[0];
	    lTemp = 8*lEncRes[0];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_T, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[0] = lPosMid;
	    break;
	case RO_AXIS_R:
	    iAxisIndex = 1;
	    sprintf(caALcommand,"ALB");
	    sprintf(caMGALcommand,"MG _ALB");
	    sprintf(caMGRLcommand,"MG _RLB");
//	    lNegPos = lSoftwareNegLimit[1] + 1000;	// this is starting pos
//	    lPosPos = lSoftwarePosLimit[1] - 1000; 	// this is ending pos
	    if( FIOGetStnVals(49, END_ANGLE, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, END_ANGLE, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, END_ANGLE, &lPosPos) == FAILURE )
		return FAILURE;
	    // set the pos -/+ manually
	    //lNegPos = -7000;
	    //lPosPos = 7000;
	    lOperatingSpeed[1] = lFrictionSpeed;
	    lPosition[1] =  lNegPos;
	    lPositionPos[1] =  lPosPos;
	    //lPosMid = lNegPos + 1500;
	    //lPositionMid[1] = lPosMid;
  	    glEncoderResolution[1] = lEncRes[1];
	    lTemp = 8*lEncRes[1];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_R, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[1] = lPosMid;
	    break;
	case RO_AXIS_Z:
	    iAxisIndex = 2;
	    sprintf(caALcommand,"ALC");
	    sprintf(caMGALcommand,"MG _ALC");
	    sprintf(caMGRLcommand,"MG _RLC");
	    if( FIOGetStnVals(49, CROSS_SLOT_LOW, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, CROSS_SLOT_LOW, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, CROSS_SLOT_LOW, &lPosPos) == FAILURE )
		return FAILURE;
	    // set the pos -/+ manually
	    //lNegPos = 1000;
	    //lPosPos = 8000;
	    lOperatingSpeed[2] = lFrictionSpeed;
	    lPosition[2] =  lNegPos;
	    lPositionPos[2] =  lPosPos;
	    //lPosMid = lNegPos + 1500;
	    //lPositionMid[2] = lPosMid;
  	    glEncoderResolution[2] = lEncRes[2];
	    lTemp = 8*lEncRes[2];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_Z, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[2] = lPosMid;
	    break;
	case RO_AXIS_W:
	    iAxisIndex = 3;
	    sprintf(caALcommand,"ALD");
	    sprintf(caMGALcommand,"MG _ALD");
	    sprintf(caMGRLcommand,"MG _RLD");
	    if( FIOGetStnVals(49, CROSS_SLOT_HIGH, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, CROSS_SLOT_HIGH, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, CROSS_SLOT_HIGH, &lPosPos) == FAILURE )
		return FAILURE;
	    // set the pos -/+ manually
	    //lNegPos = 1000;
	    //lPosPos = 8000;
	    lOperatingSpeed[3] = lFrictionSpeed;
	    lPosition[3] =  lNegPos;
	    lPositionPos[3] =  lPosPos;
	    //lPosMid = lNegPos + 3000;
	    //lPositionMid[3] = lPosMid;
  	    glEncoderResolution[3] = lEncRes[3];
	    lTemp = 8*lEncRes[3];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_W, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[3] = lPosMid;
	    break;
	case RO_AXIS_t:
	    iAxisIndex = 4;
	    sprintf(caALcommand,"ALE");
	    sprintf(caMGALcommand,"MG _ALE");
	    sprintf(caMGRLcommand,"MG _RLE");
	    if( FIOGetStnVals(49, ACC_LOW, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, ACC_LOW, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, ACC_LOW, &lPosPos) == FAILURE )
		return FAILURE;
	    lOperatingSpeed[4] = lFrictionSpeed;
	    lPosition[4] =  lNegPos;
	    lPositionPos[4] =  lPosPos;
	    //lPosMid = lNegPos + 1500;
	    //lPositionMid[4] = lPosMid;
  	    glEncoderResolution[4] = lEncRes[4];
	    lTemp = 8*lEncRes[4];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_t, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[4] = lPosMid;
	    break;
	case RO_AXIS_r:
	    iAxisIndex = 5;
	    sprintf(caALcommand,"ALF");
	    sprintf(caMGALcommand,"MG _ALF");
	    sprintf(caMGRLcommand,"MG _RLF");
	    if( FIOGetStnVals(49, ACC_HIGH, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, ACC_HIGH, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, ACC_HIGH, &lPosPos) == FAILURE )
		return FAILURE;
	    lOperatingSpeed[5] = lFrictionSpeed;
	    lPosition[5] =  lNegPos;
	    lPositionPos[5] =  lPosPos;
	    //lPosMid = lNegPos + 1500;
	    //lPositionMid[5] = lPosMid;
  	    glEncoderResolution[5] = lEncRes[5];
	    lTemp = 8*lEncRes[5];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_r, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[5] = lPosMid;
	    break;
	case RO_AXIS_z:
	    iAxisIndex = 6;
	    sprintf(caALcommand,"ALG");
	    sprintf(caMGALcommand,"MG _ALG");
	    sprintf(caMGRLcommand,"MG _RLG");
	    if( FIOGetStnVals(49, AVE_THICK, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, AVE_THICK, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, AVE_THICK, &lPosPos) == FAILURE )
		return FAILURE;
	    lOperatingSpeed[6] = lFrictionSpeed;
	    lPosition[6] =  lNegPos;
	    lPositionPos[6] =  lPosPos;
	    //lPosMid = lNegPos + 1500;
	    //lPositionMid[6] = lPosMid;
  	    glEncoderResolution[6] = lEncRes[6];
	    lTemp = 8*lEncRes[6];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_z, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[6] = lPosMid;
	    break;
	case RO_AXIS_w:
	    iAxisIndex = 7;
	    sprintf(caALcommand,"ALH");
	    sprintf(caMGALcommand,"MG _ALH");
	    sprintf(caMGRLcommand,"MG _RLH");
	    if( FIOGetStnVals(49, TAUGHT_POS_TO_WAFER, &lFrictionSpeed) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(50, TAUGHT_POS_TO_WAFER, &lNegPos) == FAILURE )
		return FAILURE;
	    if( FIOGetStnVals(51, TAUGHT_POS_TO_WAFER, &lPosPos) == FAILURE )
		return FAILURE;
	    lOperatingSpeed[7] = lFrictionSpeed;
	    lPosition[7] =  lNegPos;
	    lPositionPos[7] =  lPosPos;
	    //lPosMid = lNegPos + 3000;
	    //lPositionMid[7] = lPosMid;
  	    glEncoderResolution[7] = lEncRes[7];
	    lTemp = 8*lEncRes[7];	// at least 2 motor-turns 
	    ROEncoderToEU(RO_AXIS_w, lTemp, &lPosMid);
	    lPosMid += lNegPos;
	    lPositionMid[7] = lPosMid;
	    break;
    }

    if( lFrictionSpeed <= 50 ) // if the speed is too low, error out.
	return FAILURE;

    // with current speed, move the axis to the starting pos
    if (ROMoveToAbs( ulAxisArg, lPosition) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for axis motion complete
    while(~ulAMFlag & ulAxisArg)
    {
	TIDelay(500);
	ROUpdateTS(0);

        // if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }


//    if(!giXYZRobot) 
//	goto skip_index;	// no feedback from robot, must skip index finding

    // Axis is in starting position
    // Now set the index speed, and do index checking.
    if( ROSetParameter( FALSE, ulAxisArg, lIndexSpeed, OPERATIONAL_SPEED ) == FAILURE)
        return FAILURE;
    
    iOutputLSave = inb(IO_PRE_OUTPUT_L);
    iOutputL = iOutputLSave & 0x8F;	// encoder index select: X000XXXX
    outb(iOutputL, IO_PRE_OUTPUT_L);

    lFirstSignal = 0;
    lSecondSignal = 0;

    rc = GASendDMCCommand(ghDMC, caALcommand, ReturnBuffer, MAXGASTR);

    if (ROMoveToAbs( ulAxisArg, lPositionMid) == FAILURE)
	return FAILURE;
    
    ROUpdateTS(0);

    // check for the axis motion complete
    while(~ulAMFlag & ulAxisArg)
    {
	rc = GASendDMCCommand(ghDMC, caMGALcommand, ReturnBuffer, MAXGASTR);
	iLatch = atoi(ReturnBuffer);
	if(!iLatch) // Active Low
    	{
	    rc = GASendDMCCommand(ghDMC, caMGRLcommand, ReturnBuffer, MAXGASTR);
	    lTemp = atol(ReturnBuffer);
	    if(lFirstSignal == 0)
 	    {
		lFirstSignal = lTemp;
		rc = GASendDMCCommand(ghDMC, caALcommand, ReturnBuffer, MAXGASTR);
	    }
	    else
		lMotorRes = labs(lTemp - lFirstSignal);

	}
	ROUpdateTS(0);
        // if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    lFirstSignal = 0;
    lSecondSignal = 0;

    rc = GASendDMCCommand(ghDMC, caALcommand, ReturnBuffer, MAXGASTR);

    if (ROMoveToAbs( ulAxisArg, lPosition) == FAILURE)
	return FAILURE;
    
    ROUpdateTS(0);

    // check for the axis motion complete
    while(~ulAMFlag & ulAxisArg)
    {
	rc = GASendDMCCommand(ghDMC, caMGALcommand, ReturnBuffer, MAXGASTR);
	iLatch = atoi(ReturnBuffer);
	if(!iLatch) // Active Low
    	{
	    rc = GASendDMCCommand(ghDMC, caMGRLcommand, ReturnBuffer, MAXGASTR);
	    lTemp = atol(ReturnBuffer);
	    if(lFirstSignal == 0)
 	    {
		lFirstSignal = lTemp;
		rc = GASendDMCCommand(ghDMC, caALcommand, ReturnBuffer, MAXGASTR);
	    }
	    else
	    {
		glMotorResolution[iAxisIndex] = (lMotorRes + labs(lFirstSignal - lTemp) )/2;
//printf("motor resolution=%ld\n",glMotorResolution[iAxisIndex]);
	    }
	}
	ROUpdateTS(0);
        // if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    // index signal test should be done
    outb(iOutputLSave, IO_PRE_OUTPUT_L);




//skip_index:


    // Axis is in starting position
    // Now set the friction speed, and start recording, and move it to ending position.

    if( ROSetParameter( FALSE, ulAxisArg, lOperatingSpeed, OPERATIONAL_SPEED ) == FAILURE)
        return FAILURE;

    // Start Data Recording
    if( ROSendRC(ulAxisArg, 1) == FAILURE)
	return FAILURE;

//printf("time delay RC=%ld pos=%ld\n",lTimeDelay,lPosition[0]);

    // Dealy time before axis motion: to get ready for data recording
    TIDelay((unsigned int)lTimeDelay);	// delay 1.8 seconds.
//printf("time delay done\n");

    // with the friction speed, move the axis to the ending pos
    if (ROMoveToAbs( ulAxisArg, lPositionPos) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for the axis motion complete
    while(~ulAMFlag & ulAxisArg)
    {
	TIDelay(500);
	ROUpdateTS(0);

        // if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }


    // ending position reached, continue to back to starting position
    if (ROMoveToAbs( ulAxisArg, lPosition) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for the axis motion complete
    while(~ulAMFlag & ulAxisArg)
    {
	TIDelay(500);
	ROUpdateTS(0);

        // if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    // all the motions are complete, stop data recording
    if( ROSendRC(ulAxisArg, 0) == FAILURE)
	return FAILURE;

    // upload data
    if(	ROSendQU(ulAxisArg) == FAILURE)
	return FAILURE;

    if( ROWriteRC() == FAILURE)
	return FAILURE;

    return SUCCESS;

}

// Restore Axis speed and its saved position
//    This function is to be called after Friction Test.
//
int RORestoreAxis(unsigned int ulAxisArg, long lPosArg, long *lCurrentSpeedArg)
{

    if( ROSetParameter( FALSE, ulAxisArg, lCurrentSpeedArg, OPERATIONAL_SPEED ) == FAILURE)
        return FAILURE;

    // ending position reached, continue to back to starting position
    if (ROMoveToAbs( ulAxisArg, lPosArg) == FAILURE)
	return FAILURE;

    TIDelay(500);
    ROUpdateTS(0);

    // check for the axis motion complete
    while(~ulAMFlag & ulAxisArg)
    {
	TIDelay(500);
	ROUpdateTS(0);

        // if there's any error during motion, fail out
	if (ROGetERFlag() || ROGetServoFlag())
	    return FAILURE;
    }

    return SUCCESS;
}

int ROMotionProfDone(void)
{
    int rc;
    char ReturnBuffer[MAXGASTR];
    int iFlag;

    rc = GASendDMCCommand(ghDMC, "MG VRQF", ReturnBuffer, MAXGASTR);
    iFlag = atoi(ReturnBuffer);

    return iFlag;
}

int ROMotionProfTest(unsigned int ulAxisArg, long lEncSourceArg, long lEncDestinArg)
{
    int rc;
    char ReturnBuffer[MAXGASTR];
    char sMsgS[40];
    char sMsgD[40];
    char sMsgX[40];
    int iFlag;

    rc = GASendDMCCommand(ghDMC, "MG VRQF", ReturnBuffer, MAXGASTR);
    iFlag = atoi(ReturnBuffer);
    if (iFlag != 0) return FAILURE;

    
    sprintf(sMsgS, "VSRC=%ld\n",lEncSourceArg);
    rc = GASendDMCCommand(ghDMC, sMsgS, ReturnBuffer, MAXGASTR);

    sprintf(sMsgD, "VDES=%ld\n",lEncDestinArg);
    rc = GASendDMCCommand(ghDMC, sMsgD, ReturnBuffer, MAXGASTR);

    switch(ulAxisArg)
    {
	case RO_AXIS_T:
	    sprintf(sMsgX, "XQ#XRQT\n");
	    rc = GASendDMCCommand(ghDMC, sMsgX, ReturnBuffer, MAXGASTR);
	    break;
	case RO_AXIS_R:
	    sprintf(sMsgX, "XQ#XRQR\n");
	    rc = GASendDMCCommand(ghDMC, sMsgX, ReturnBuffer, MAXGASTR);
	    break;
	case RO_AXIS_Z:
	    sprintf(sMsgX, "XQ#XRQZ\n");
	    rc = GASendDMCCommand(ghDMC, sMsgX, ReturnBuffer, MAXGASTR);
	    break;
	case RO_AXIS_W:
	    sprintf(sMsgX, "XQ#XRQW\n");
	    rc = GASendDMCCommand(ghDMC, sMsgX, ReturnBuffer, MAXGASTR);
	    break;
	default:
	    return FAILURE;
    }

    iFlag = 1;
    while (iFlag)
    {
	TIDelay(1000);
	rc = GASendDMCCommand(ghDMC, "MG VRQF", ReturnBuffer, MAXGASTR);
	iFlag = atoi(ReturnBuffer);
    }

    return SUCCESS;
}

void ROConvertRobotToWorld(long lPosTArg, long lPosRArg, long *lWXArg, long *lWYArg)
{
    int lRLen;

//    lRLen = glEELength + lPosRArg;
    lRLen = sVectorParameter.m_lEELength + lPosRArg;

    if (lRLen <= 0) // cannot be zero, nor less
    {
	*lWXArg = 0;
	*lWYArg = 0;
	return;
    }

    *lWXArg = (long) (lRLen * sin((float)lPosTArg * HUNDREDDEG_TO_RAD) * (-1));
    *lWYArg = (long) (lRLen * cos((float)lPosTArg * HUNDREDDEG_TO_RAD));

    return;
}

void ROConvertWorldToRobot(long lWXArg, long lWYArg, long *lPosTArg, long *lPosRArg)
{
    long posT1, posT2;
    long lRLen;

    lRLen = (long) sqrt( (float)lWXArg*(float)lWXArg + (float)lWYArg*(float)lWYArg);
//    *lPosRArg = lRLen - glEELength;
    *lPosRArg = lRLen - sVectorParameter.m_lEELength;


    if (lRLen < 0) // cannot be zero, nor less
    {
	*lPosTArg = 0;
	*lPosRArg = 0;
	return;
    }

    if (lWXArg == 0 && lWYArg == 0)
    {
	*lPosTArg = 0;
	return;
    }

    posT1 = (long)(asin(-1.0*(float)lWXArg/(float)lRLen) * RAD_TO_HUNDREDDEG);
    posT2 = (long)(acos((float)lWYArg/(float)lRLen) * RAD_TO_HUNDREDDEG);

//printf("W2R: T1=%d T2=%d x=%d y=%d\n",posT1,posT2,lWXArg, lWYArg);

    if (posT1 >= 0)
    {
	if (posT2 >= 9000)
	{
            *lPosTArg = 18000 - posT1; // this is the case T = 90 to 180
//printf("T correction1: T1=%d T2=%d posT=%d x=%d y=%d\n",posT1,posT2,*lPosTArg,lWXArg, lWYArg);
	}
	else
	{
	    *lPosTArg = posT1;	// this is the case T = 0 to 90
	}
    }
    else
    {
	if (posT2 <= 9000)
	{
	    *lPosTArg = 36000 + posT1; // this is the case T = 270 to 360
//printf("T correction2: T1=%d T2=%d posT=%d x=%d y=%d\n",posT1,posT2,*lPosTArg,lWXArg, lWYArg);
	}
	else
	{
	    *lPosTArg = 18000 - posT1; // this is the case T = 180 to 270
//printf("T correction3: T1=%d T2=%d posT=%d x=%d y=%d\n",posT1,posT2,*lPosTArg,lWXArg, lWYArg);
	}
    }
    return;
}

//
// Compute Mid points positions:
// 	Assume: starting T and final T are not far: the difference is < 180 deg
//
void ROGetMidPositions(long lStartTArg, long lStartRArg, long lFinalTArg, long lFinalRArg,
	long *lMidTPosArg, long *lMidRPosArg, long *lMidWXArg, long *lMidWYArg, int *nPointArg)
{
    long lVecLen, nSeg, segLen, xDelta, yDelta;
    long lStartWX, lStartWY, lFinalWX, lFinalWY;
    long wX=0;
    long wY=0;
    long posT, posR;
    long lDirection;
    int i;

    ROConvertRobotToWorld(lStartTArg, lStartRArg, &lStartWX, &lStartWY);
    ROConvertRobotToWorld(lFinalTArg, lFinalRArg, &lFinalWX, &lFinalWY);

    lDirection = 0;	// T going from small to large angle
    if(lStartTArg > lFinalTArg)	// T going from large angle to small angle
	lDirection = 1;

//printf("getM sT=%d sR=%d swX=%d swY=%d fT=%d fR=%d fwX=%d fwY=%d\n",lStartTArg,lStartRArg, lStartWX, lStartWY, lFinalTArg, lFinalRArg, lFinalWX, lFinalWY);

    lVecLen = (long)sqrt((lFinalWX-lStartWX)*(lFinalWX-lStartWX) + (lFinalWY-lStartWY)*(lFinalWY-lStartWY));

    nSeg = lVecLen / 500 + 1; 	// at least 2 segments with 1/2 inch segment
    segLen = lVecLen / nSeg;	// each segment length
    xDelta = (lFinalWX-lStartWX)/nSeg;	// delta x
    yDelta = (lFinalWY-lStartWY)/nSeg;	// delta y

//printf("getM vec=%d nseg=%d xD=%d yD=%d\n",lVecLen, nSeg, xDelta, yDelta);

    lMidTPosArg[0] = lStartTArg; // start pos T & R
    lMidRPosArg[0] = lStartRArg;
    lMidWXArg[0] = lStartWX;	 // start wx
    lMidWYArg[0] = lStartWY;	 //       wy

    wX = lStartWX;
    wY = lStartWY;

    for (i=1; i<nSeg; ++i)	// get mid positions at least 1 <== nSeg-1
    {
	wX += xDelta;
	wY += yDelta;
    	ROConvertWorldToRobot(wX, wY, &posT, &posR);

	// Must correct T if T pos is > 360
	if (lDirection == 0)
	{
	    if (posT < lStartTArg) 
	    {
		posT += 36000;	// T must be > 360
	    }
	}
	else
	{	// direction = T from large to small angle
	    if (posT < lFinalTArg)
	    {
		posT += 36000;	// T must be > 360
	    }
	}
	
	lMidTPosArg[i] = posT;
	lMidRPosArg[i] = posR;
	lMidWXArg[i] = wX;
	lMidWYArg[i] = wY;
//printf("getM i=%d wX=%d wY=%d midT=%d midR=%d\n",i,wX,wY,posT,posR);
    }

    lMidTPosArg[i] = lFinalTArg; // final pos T & R
    lMidRPosArg[i] = lFinalRArg;
    lMidWXArg[i] = lFinalWX;	 // final wx
    lMidWYArg[i] = lFinalWY;	 //       wy

    *nPointArg = i-1;	// number of mid points excluding the start & final


    return;
}

/****************************************************************
 *
 * Function:    ROMoveToCartesian
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROMoveToCartesian(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int i, iAxis;                  /* Temporary variable to loop through all axes. */
    int iMechAxis;              /* The bit pattern of the axis being checked. */
    long laPosSoftwareLimit[8]; /* Local copy of the positive software limit to check
                                 * against requested movement position. */
    long laNegSoftwareLimit[8]; /* Local copy of the negative software limit to check
                                 * against requested movement position. */
    long lStartPos[8];          /* Local variable of current position */
    long laFinalPosition[8]={0,0,0,0,0,0,0,0};  /* The final position to move to. */
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    long lPosition[8]={0,0,0,0,0,0,0,0};	// position in T & R
    long lPositionEC[8]={0,0,0,0,0,0,0,0};	// Encoder converted from lPosition
    char caLICmdStr[80], caResp[80];

    long lMidTPos[80], lMidRPos[80], lMidWX[80], lMidWY[80];
    long lMidT1, lMidT2, lMidR1, lMidR2;
    int nPoint;
    long vX, vY;
    int rc;


    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    /* Check to see if the axis is ready to move, i.e. servoed on, homed,
     * not currently in motion, and not currently in an error state. */
    if (ROAxisReadyToMove(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the start position to calculate a relative move and also for use
     * in the WPOA and WPOR host commands. */

    lStartPos[0] = lStartPos[1] = lStartPos[2] = lStartPos[3] = 0;
    lStartPos[4] = lStartPos[5] = lStartPos[6] = lStartPos[7] = 0;
    if (ROReadCurrentPosition(ulEquipeAxisArg, lStartPos) == FAILURE)
        goto error_exit;

    /* Get the software limits to check that the move doesn't exceed boundary limits. */
    if(ROGetParameter(TRUE, ulEquipeAxisArg, laPosSoftwareLimit, SOFT_POS_LIMIT) == FAILURE)
        goto error_exit;
    if(ROGetParameter(TRUE, ulEquipeAxisArg, laNegSoftwareLimit, SOFT_NEG_LIMIT) == FAILURE)
        goto error_exit;

    /* Calculate a relative move and check the position against the software limits. */
    for (iAxis=0; iAxis<8; iAxis++)
    {
        /* Get the bit pattern for each axis. */
        iMechAxis = 1 << iAxis;
        /* If the axis is requested to be moved... */
        if (uGalilAxes & iMechAxis)
        {

            /* ...and check the position against the software limits. */
            if ((lPositionArg[iAxis] < laNegSoftwareLimit[iAxis]) ||
                (lPositionArg[iAxis] > laPosSoftwareLimit[iAxis]))
            {
                goto error_exit;
            }
 
        }
        /* If the axis is NOT requested to be moved, make sure it stays where it is. */
        else
        {
            lPositionArg[iAxis] = lStartPos[iAxis];
        }
    }

    /* Scale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, lPositionArg, laFinalPosition) == FAILURE)
        goto error_exit;

    ROGetMidPositions(lStartPos[0], lStartPos[1], lPositionArg[0], lPositionArg[1], 
	lMidTPos, lMidRPos, lMidWX, lMidWY, &nPoint);

    rc = GASendDMCCommand(ghDMC, "LMAB", caResp, 80);

/*
    lPosition[0] = lStartPos[0];
    ROUnscalePos( RO_AXIS_T, lPosition, lPositionEC);
    lMidT1 = lPositionEC[0];
    lPosition[1] = lStartPos[1];
    ROUnscalePos( RO_AXIS_R, lPosition, lPositionEC);
    lMidR1 = lPositionEC[1];
    lPosition[0] = lPositionArg[0];
    ROUnscalePos( RO_AXIS_T, lPosition, lPositionEC);
    lMidT2 = lPositionEC[0];
    lPosition[1] = lPositionArg[1];
    ROUnscalePos( RO_AXIS_R, lPosition, lPositionEC);
    lMidR2 = lPositionEC[1];
    vX = lMidT2 - lMidT1;
    vY = lMidR2 - lMidR1;
    sprintf(caLICmdStr,"LI %d, %d", vX, vY);
    rc = GASendDMCCommand(ghDMC, caLICmdStr, caResp, 80);
*/
    for (i=0; i<=nPoint; ++i) // nPoint is # of mid pos excluding start & final
    {
	lPosition[0] = lMidTPos[i];
	ROUnscalePos( RO_AXIS_T, lPosition, lPositionEC);
	lMidT1 = lPositionEC[0];
	lPosition[1] = lMidRPos[i];
	ROUnscalePos( RO_AXIS_R, lPosition, lPositionEC);
	lMidR1 = lPositionEC[1];
	lPosition[0] = lMidTPos[i+1];
	ROUnscalePos( RO_AXIS_T, lPosition, lPositionEC);
	lMidT2 = lPositionEC[0];
	lPosition[1] = lMidRPos[i+1];
	ROUnscalePos( RO_AXIS_R, lPosition, lPositionEC);
	lMidR2 = lPositionEC[1];
	
	vX = lMidT2 - lMidT1;
	vY = lMidR2 - lMidR1;

//printf("xmvc i=%d T0=%d Te=%d R0=%d Re=%d T1=%d Te=%d R1=%d Re=%d vx=%d vy=%d\n",i, lMidTPos[i], lMidT1, lMidRPos[i], lMidR1, lMidTPos[i+1],lMidT2,lMidRPos[i+1],lMidR2,vX,vY);

	sprintf(caLICmdStr,"LI %d, %d", vX, vY);
	rc = GASendDMCCommand(ghDMC, caLICmdStr, caResp, 80);
    }
    rc = GASendDMCCommand(ghDMC, "LE", caResp, 80);
    sprintf(caLICmdStr,"VS %d", sVectorParameter.m_lVS);
    rc = GASendDMCCommand(ghDMC, caLICmdStr, caResp, 80);
    sprintf(caLICmdStr,"VA %d", sVectorParameter.m_lVA);
    rc = GASendDMCCommand(ghDMC, caLICmdStr, caResp, 80);
    sprintf(caLICmdStr,"VD %d", sVectorParameter.m_lVD);
    rc = GASendDMCCommand(ghDMC, caLICmdStr, caResp, 80);
    rc = GASendDMCCommand(ghDMC, "BGS", caResp, 80);

    /* If everything was successful, indicate that the axis is in motion. */
    if (!GAGetGalilErrorFlag())
    {
        ulAMFlag &= ~ulEquipeAxisArg;
    }
    else
    {
        goto error_exit;
    }

    return SUCCESS;

error_exit:
    return FAILURE;
}

float ROGetSinTheta(long lThetaArg)
{
    return sin((float)lThetaArg * HUNDREDDEG_TO_RAD);
}

float ROGetCosTheta(long lThetaArg)
{
    return cos((float)lThetaArg * HUNDREDDEG_TO_RAD);
}

void ROConvertWorldToEfem(long lPosTArg, long lPosRArg, long *lWXArg, long *lWYArg)
{
    *lWXArg = (long) (sVectorParameter.m_fCosTheta * lPosTArg - sVectorParameter.m_fSinTheta * lPosRArg + 0.5);
    *lWYArg = (long) (sVectorParameter.m_fSinTheta * lPosTArg + sVectorParameter.m_fCosTheta * lPosRArg + 0.5);

    return;
}

void ROConvertEfemToWorld(long lPosTArg, long lPosRArg, long *lWXArg, long *lWYArg)
{
    *lWXArg = (long) (sVectorParameter.m_fCosTheta * lPosTArg + sVectorParameter.m_fSinTheta * lPosRArg + 0.5);
    *lWYArg = (long) (sVectorParameter.m_fCosTheta * lPosRArg - sVectorParameter.m_fSinTheta * lPosTArg + 0.5);

    return;
}


int ROMoveCartesianDirection(long lDirectionArg, long lDistanceArg)
{
    unsigned long ulEquipeAxis = RO_AXIS_T | RO_AXIS_R | RO_AXIS_Z;
    long lStartPos[8]={0,0,0,0,0,0,0,0};	// position in T & R
    long wX1, wY1, eX1, eY1;
    long wX2, wY2, eX2, eY2;
    long startT, startR, finalT, finalR;
    long wX3, wY3, eX3, eY3, finalT2, finalR2; // mid point intermediary

    // if Encoder Drift Alarm is ON, don't do anything
    if(giEncoderAlarm)
    {
	return FAILURE;
    }

    if (ROReadCurrentPosition(ulEquipeAxis, lStartPos) == FAILURE)
    {
	return FAILURE;
    }

    // convert robot to world
    startT = lStartPos[0];
    startR = lStartPos[1];
    ROConvertRobotToWorld(startT, startR, &wX1, &wY1);

    // convert efem to world
    ROConvertEfemToWorld(wX1, wY1, &eX1, &eY1);

    eX2 = eX1; eY2 = eY1;
    eX3 = eX1; eY3 = eY1;
    switch (lDirectionArg)
    {
	case 8:	// north move
	    eY2 = eY1 + lDistanceArg;
	    eY3 = eY1 + lDistanceArg/2;
	    break;
	case 2:	// south move
	    eY2 = eY1 - lDistanceArg;
	    eY3 = eY1 - lDistanceArg/2;
	    break;
	case 6:	// east move
	    eX2 = eX1 + lDistanceArg;
	    eX3 = eX1 + lDistanceArg/2;
	    break;
	case 4:	// west move
	    eX2 = eX1 - lDistanceArg;
	    eX3 = eX1 - lDistanceArg/2;
	    break;
	case 9:	// north-east move
	    eY2 = eY1 + (long)(lDistanceArg * 0.70711);
	    eX2 = eX1 + (long)(lDistanceArg * 0.70711);
	    eY3 = eY1 + (long)(lDistanceArg * 0.70711)/2;
	    eX3 = eX1 + (long)(lDistanceArg * 0.70711)/2;
	    break;
	case 3:	// south-east move
	    eY2 = eY1 - (long)(lDistanceArg * 0.70711);
	    eX2 = eX1 + (long)(lDistanceArg * 0.70711);
	    eY3 = eY1 - (long)(lDistanceArg * 0.70711)/2;
	    eX3 = eX1 + (long)(lDistanceArg * 0.70711)/2;
	    break;
	case 7:	// north-west move
	    eY2 = eY1 + (long)(lDistanceArg * 0.70711);
	    eX2 = eX1 - (long)(lDistanceArg * 0.70711);
	    eY3 = eY1 + (long)(lDistanceArg * 0.70711)/2;
	    eX3 = eX1 - (long)(lDistanceArg * 0.70711)/2;
	    break;
	case 1:	// south-west move
	    eY2 = eY1 - (long)(lDistanceArg * 0.70711);
	    eX2 = eX1 - (long)(lDistanceArg * 0.70711);
	    eY3 = eY1 - (long)(lDistanceArg * 0.70711)/2;
	    eX3 = eX1 - (long)(lDistanceArg * 0.70711)/2;
	    break;
    }


    // convert world to efem
    ROConvertWorldToEfem(eX2, eY2, &wX2, &wY2);
    ROConvertWorldToEfem(eX3, eY3, &wX3, &wY3);

    // convert world to robot
    ROConvertWorldToRobot(wX2, wY2, &finalT, &finalR);
    ROConvertWorldToRobot(wX3, wY3, &finalT2, &finalR2);

//printf("sT=%d sR=%d eX1=%d eY1=%d wX1=%d wY1=%d\n",startT,startR,eX1,eY1,wX1,wY1);
//printf("fT2=%d fR2=%d eX3=%d eY3=%d wX3=%d wY3=%d\n",finalT2,finalR2,eX3,eY3,wX3,wY3);

    if(finalT2 > startT && finalT2 > finalT)
    {
	if(abs(finalT-startT) > 27000)
	    finalT = finalT + 36000;
    }
    else if(finalT2 < startT && finalT2 < finalT)
    {
        if(abs(finalT - startT) > 27000)
	    finalT = finalT + 36000;
    }
    else if(finalT < (startT - 36000))
    {
	finalT = finalT + 36000;
    }

//printf("fT=%d fR=%d eX2=%d eY2=%d wX2=%d wY2=%d\n",finalT,finalR,eX2,eY2,wX2,wY2);

    lStartPos[0] = finalT;
    lStartPos[1] = finalR;

    ROMoveToCartesian(ulEquipeAxis, lStartPos);
    return SUCCESS;
}

