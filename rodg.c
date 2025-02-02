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
 * Program:     Controller Firmware
 * File:        rodg.c
 * Functions:   ROTestHome
 *              ROAdjustRAtHome
 *              ROAdjustRAtIndex
 *              ROSetHomeR
 *              ROSetTestScaling
 *              RODoROffset
 *              ROPresetHomeR
 *              ROTestVacuum
 *              ROCorrectDistance
 *              ROTestRobotIO
 *
 * Description: Provide an interface to mechanism based diagnostics
 *      functions
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <sys/io.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "sck.h"
#include "rodg.h"
#include "rofio.h"
#include "ro.h"
#include "roloc.h"
#include "gag.h"
#include "gaintr.h"
#include "fiog.h"
#include "ser.h"
#include "scttr.h"
#include "scio.h"
#include "scintr.h"
#include "sctim.h"

extern int giNumOfAxes;

void GAGalilWriteIO(int, int);
int  GAGalilReadIO(int);
int  GAGalilReadOut(int);

char cpNull[8];
/****************************************************************
 *
 * Function:    ROTestHome
 *
 * Abstract:    Home Test & Set. This tests the home positions
 *      for each/all axes. It starts by saving the parameters
 *      which will be changed. An an axis keypress, it sets the
 *      new parameters, adjusts flags, and initiates a home sequence.
 *      Once the home sequence is complete, it finds the edge
 *      (the distance from the home position to the home switch),
 *      then prints that value to the screen, and shuts off all axes.
 *      The home position is defined to be the first index pulse
 *      from the home/limit switch. It restores the saved parameters
 *      on exit.
 *
 * Parameters:
 *      iCardNoArg      (in) The Galil card which axes are to be homed
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROTestHome(int iCardNoArg, int iVacExistsArg)
{
    int  iPortBase;         /* The robot I/O (F) port to read/write to. */
    int  iWhichParm;        /* Relevant parameter file: ROBOTFILE or PREALIGNFILE. */
    int  iTempCardNum;      /* Relevant Galil card number: GA_CARD_0 or GA_CARD_1.  */
    int  iTint;             /* First temporary variable. */
    int  iTS = 0;           /* This variable is used to record internally
                             * which axi(e)s is(are) being homed. */
    int  iTrackCardNum = 0; /* The Galil card number that the track is on.
                             * If there is no track, this variable will remain 0. */
    int  iDualArmCardNum = 0;       /* The Galil card number that the dual arm is on.
                                     * If there is no dual arm, this variable will remain 0. */
    int  iCardToUse = 0;    /* If there are multiple possibilities, the card number that should be used. */
    int  iReturnValue = 0;  /* Second temporary variable. */
    int  iMaxAxis = 3;      /* Used to print data to the screen. If the auxilliary axis
                             * is on the robot Galil card, command returns will
                             * be parsed to 4 numbers. Just a robot parses to 3 only. */
    int  iVacIndex;
    char cBuf[MAXGASTR], *pChar, cButtonPressed; //, *pRDistanceTemp;

    char caTP[20];

    long lTP, lLowDist, lHighDist;  /* Used to determine if a belt correction is needed. */
    unsigned long ulAuxAxis = 0;    /* Axis designation if an auxilliary axis exists. */
    /* The remaining variables are used to get/set or save/restore
     * Galil parameters values. */
    long lTempVal[8];
    long lHomeSpeed[8]={0,0,0,0,0,0,0,0};
    long lHomeAccel[8]={0,0,0,0,0,0,0,0};
    long lEncRes[8]={0,0,0,0,0,0,0,0};
    long lAuxHomeSpeed[8]={0,0,0,0,0,0,0,0};
    long lAuxHomeAccel[8]={0,0,0,0,0,0,0,0};
    long lAuxEncRes[8]={0,0,0,0,0,0,0,0};
    long lSaveSpeed[8]={0,0,0,0,0,0,0,0};
    long lSaveAccel[8]={0,0,0,0,0,0,0,0};
    long lSaveDecel[8]={0,0,0,0,0,0,0,0};
    long lSaveTorqueLimit[8]={0,0,0,0,0,0,0,0};
    long lZAxisBrake[3] = {0,0,0};
//    long lTAFlags[8]={0,0,0,0,0,0,0,0};
//    long lTAFlagOff[8]={0,0,0,0,0,0,0,0};


    /* Set the right card number and IO port address depending on the
     * card number, setup an auxilliary axis if one exists, and initialize the amps.
     * Card number (iCardNoArg) : 0=robot, 1=prealigner. */
    TTPrintsAt( 4, 1, "Init. AMP. board" );
    iTempCardNum = iCardNoArg ? GA_CARD_1 : GA_CARD_0;

	if (iTempCardNum == GA_CARD_1) goto preTest;

    /* Check to see if a dual arm exists in the system. */
    if (ROGetSpecialAxis(RO_DUAL_ARM))
    {
        iDualArmCardNum = GA_CARD_0;
    }
    /* Check to see if a track exists in the system. */
    else if (ROGetSpecialAxis(RO_TRACK))
    {
        /* Find out which card the track is on. */
        if ((ROGetSpecialAxis(RO_TRACK) <= RO_AXIS_W))
            iTrackCardNum = GA_CARD_0;
    }
    /* Decide if the additional axis should be displayed. */
    if (((iDualArmCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0)))
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iTrackCardNum = 0;
    }
    iPortBase = IO_ROBOT_INPUT_F;
    iWhichParm = ROBOTFILE;
    GAAmpInit();
    TIDelay( 1500 );

    /* Read INPUT G port, and check the status. */
    iTint = inb(iPortBase+3);
	iTint = 0;

    if( iTint )
    {
        TTPrintsAt( 1, 1, "Connection Error" );
        TTPrintsAt( 2, 1, "Check Connection" );
        GASendReceiveGalil(iTempCardNum, (char *)"MO\r", cpNull);
        IOWriteIO( -1, 0, iPortBase+4 );
        TIDelay( 500 );
        return SUCCESS;
    }

    /* Allow user to select an axis to Home. */
    TTClearScreen();
    TTPrintsAt( 1, 1, "T:" );
    TTPrintsAt( 2, 1, "R:" );
    TTPrintsAt( 3, 1, "Z:" );
    if ((iTrackCardNum) || (iDualArmCardNum))
        TTPrintsAt( 3, 10, "W:" );
    TTPrintsAt( 4, 1, "Select Axis to Home" );

    /* If an auxilliary axis is to be shown, get its axis designation. */
    if (iDualArmCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_DUAL_ARM);
    if (iTrackCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_TRACK);

    /* Get the Z brake values from the datafile. */
    FIOGetParamVals(ROBOTFILE, H_VALUE, lZAxisBrake);
    /* Save Galil values to be restored before exiting. */
    /* Save old robot axes values. */
    ROGetParameter(FALSE, ulAxisALLRbt, lSaveSpeed, OPERATIONAL_SPEED);
    ROGetParameter(FALSE, ulAxisALLRbt, lSaveAccel, OPERATIONAL_ACCEL);
    ROGetParameter(FALSE, ulAxisALLRbt, lSaveDecel, OPERATIONAL_DECEL);
    ROGetParameter(FALSE, ulAxisALLRbt, lSaveTorqueLimit, TORQUE_LIMIT);

    /* Set new robot torque limits. */
    lTempVal[0] = lTempVal[1] = lTempVal[2] = lTempVal[3] = 9999;
    ROSetParameter(FALSE, ulAxisALLRbt, lTempVal, TORQUE_LIMIT);

    if (ulAuxAxis)
    {
        /* Save old auxilliary axes values if appropriate. */
        lTempVal[0] = lTempVal[1] = lTempVal[2] = lTempVal[3] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_SPEED);
        lSaveSpeed[3] = lTempVal[3];

        lTempVal[0] = lTempVal[1] = lTempVal[2] = lTempVal[3] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_ACCEL);
        lSaveAccel[3] = lTempVal[3];

        lTempVal[0] = lTempVal[1] = lTempVal[2] = lTempVal[3] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_DECEL);
        lSaveDecel[3] = lTempVal[3];

        lTempVal[0] = lTempVal[1] = lTempVal[2] = lTempVal[3] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, TORQUE_LIMIT);
        lSaveTorqueLimit[3] = lTempVal[3];

        /* Set new auxilliary axis torque limits. */
        lTempVal[0] = lTempVal[1] = lTempVal[2] = lTempVal[3] = 9999;
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, TORQUE_LIMIT);
    }

    /* get the S-curve profile flag value either enable(1) or disable(0) */
//    if( ROReadSCurveProfile(ulAxisALLRbt,lTAFlags) == FAILURE )
//        return FAILURE;
//    ROEnableSCurveProfile( ulAxisALLRbt, lTAFlagOff );  /* set the S-curve motion profile flag (enable/disable) */

    do
    {
	ROUpdateTS(FALSE);

        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);

        /* These following data are changed in the unscale/scale functions.
         * We need to get the data that is stored in Parameters file
         * instead using the data that is sent to Galil. */
        FIOGetParamVals(iWhichParm, HOME_SPEED, lHomeSpeed);
        FIOGetParamVals(iWhichParm, HOME_ACCEL, lHomeAccel);
        FIOGetParamVals(iWhichParm, ENCODER_RESOLUTION, lEncRes);

        if (ulAuxAxis)
        {
	    if(giNumOfAxes <= 4)
	    {
        	/* Get the same info for the auxilliary axis if appropriate. */
        	FIOGetParamVals(PREALIGNFILE, HOME_SPEED, lAuxHomeSpeed);
        	ROArrangeGetArrayForW(ulAuxAxis, lAuxHomeSpeed);
        	FIOGetParamVals(PREALIGNFILE, HOME_ACCEL, lAuxHomeAccel);
        	ROArrangeGetArrayForW(ulAuxAxis, lAuxHomeAccel);
        	FIOGetParamVals(PREALIGNFILE, ENCODER_RESOLUTION, lAuxEncRes);
        	ROArrangeGetArrayForW(ulAuxAxis, lAuxEncRes);
//printf("AuxEncRes: %d %d %d\n",lAuxEncRes[0], lAuxEncRes[1], lAuxEncRes[2]);
//lAuxEncRes[0]=500;
	    }
	    else
	    {
		/* Get the same info for the auxilliary axis if appropriate. */
        	FIOGetParamVals(AUXFILE, HOME_SPEED, lAuxHomeSpeed);
        	ROArrangeGetArrayForW(ulAuxAxis, lAuxHomeSpeed);
        	FIOGetParamVals(AUXFILE, HOME_ACCEL, lAuxHomeAccel);
        	ROArrangeGetArrayForW(ulAuxAxis, lAuxHomeAccel);
        	FIOGetParamVals(AUXFILE, ENCODER_RESOLUTION, lAuxEncRes);
        	ROArrangeGetArrayForW(ulAuxAxis, lAuxEncRes);
	    }
        }

        switch( cButtonPressed )
        {
            case 'A':   /* User wants to home all axes. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing all axes...  " );
                /* Set home speeds for robot axes. */
                ROSetParameter(FALSE, ulAxisALLRbt, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, ulAxisALLRbt, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, ulAxisALLRbt, lHomeAccel, OPERATIONAL_DECEL);
                if (ulAuxAxis)
                {
                    /* Set home speeds for auxilliary axes if appropriate. */
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeSpeed, OPERATIONAL_SPEED);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_ACCEL);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_DECEL);
                }

                /* Turn servo ON for robot or pre-aligner. */
                GASendReceiveGalil(iTempCardNum, (char *)"SHXYZ\r" , cpNull);
//                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);
                /* If a Z brake releasing is required... */
		ROServiceBrake(ROBOTFILE, TRUE);

//                if ((lZAxisBrake[0] > 0))
//                {
                    /* ...release the Z brake. */
                    /* WARNING: In ME domain card number 0 is GA_CARD_0. */
//                    ROWriteGalilPort(0, (int)lZAxisBrake[0], 1);
//                }
                TIDelay( 500 );
                /* If an auxilliary axis exists, servo it on. */
                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (iCardToUse)
                {
                    GASendReceiveGalil(iCardToUse, (char *)"SHW\r" , cpNull);
//                    GASendReceiveGalil(iCardToUse, (char *)"EI 255,255\r", cpNull);
                }
                TIDelay( 500 );

                /* Initiate home operation for all axes and set the after-motion,
                 * error, and TS flags appropriately. */
                /* All robot axes. */
                ulERFlag &= ~ulAxisALLRbt;
                TIDelay(100);
                GASendReceiveGalil(iTempCardNum, (char *)"HMXYZ\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGXYZ\r" , cpNull);
                ulAMFlag &= ~ulAxisALLRbt;
                iTS = (int)(ulAxisALLRbt);

                /* If a dual arm exists, home it. */
                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                /* If a track exists, home it. */
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (iCardToUse)
                {
                    GASendReceiveGalil(iCardToUse, (char *)"HMW\r" , cpNull);
                    GASendReceiveGalil(iCardToUse, (char *)"BGW\r" , cpNull);
                }
                if (ulAuxAxis)
                {
                    ulERFlag &= ~ulAuxAxis;
                    ulAMFlag &= ~ulAuxAxis;
                    iTS = (int)(ulAuxAxis);
                }
                break;  /* case A */

            case 'H':
            case 'M':   /* T or t axis homing. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing Theta...     " );
                /* Set home speeds for robot T axis. */
                ROSetParameter(FALSE, RO_AXIS_T, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, RO_AXIS_T, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, RO_AXIS_T, lHomeAccel, OPERATIONAL_DECEL);

                /* Turn servo ON for T or t axis. */
                GASendReceiveGalil(iTempCardNum, (char *)"SHX\r" , cpNull);
                ulERFlag &= ~RO_AXIS_T;
//                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);
                TIDelay( 500 );

                /* Initiate home operation for T or t axis... */
                GASendReceiveGalil(iTempCardNum, (char *)"HMX\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGX\r" , cpNull);

                /* ...and set the after-motion, error, and TS flags appropriately. */
                iTS = RO_AXIS_T;
                ulAMFlag &= ~RO_AXIS_T;

                /* Remember the encoder tolerance from the edge to the first index pulse
                 * in case a belt correction has to be made. See ROHomeAxis for more details. */
                if( cButtonPressed == 'H' )
                {
                    lLowDist = lEncRes[0];
                    lHighDist = 3*lEncRes[0];
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                }
                else
                {
                    lLowDist = lEncRes[0]*2-100;
                    lHighDist = lLowDist+200;
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                }
                break;

            case 'I':
            case 'N':   /* R or r axis homing. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing Radial...    " );
                /* Set home speeds for robot R axis. */
                ROSetParameter(FALSE, RO_AXIS_R, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, RO_AXIS_R, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, RO_AXIS_R, lHomeAccel, OPERATIONAL_DECEL);

                GASendReceiveGalil(iTempCardNum, (char *)"SHY\r" , cpNull);
//                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);

                ulERFlag &= ~RO_AXIS_R;
                TIDelay( 500 );

                /* Initiate home operation for R or r axis... */
                GASendReceiveGalil(iTempCardNum, (char *)"HMY\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGY\r" , cpNull);

                /* ...and set the after-motion, error, and TS flags appropriately. */
                iTS = RO_AXIS_R;
                ulAMFlag &= ~RO_AXIS_R;

                /* Remember the encoder tolerance from the edge to the first index pulse
                 * in case a belt correction has to be made. See ROHomeAxis for more details. */
                if( cButtonPressed == 'I' )
                {
                    lLowDist = lEncRes[1];
                    lHighDist = 3*lEncRes[1];
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                }
                else
                {
                    lLowDist = lEncRes[1]*2-100;
                    lHighDist = lLowDist+200;
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                }
                break;

            case 'J':
            case 'O':   /* Z or z axis homing. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing Z...         " );
                /* Set home speeds for robot Z axis. */
                ROSetParameter(FALSE, RO_AXIS_Z, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, RO_AXIS_Z, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, RO_AXIS_Z, lHomeAccel, OPERATIONAL_DECEL);

                /* Turn servo ON for Z or z axis. */
                GASendReceiveGalil(iTempCardNum, (char *)"SHZ\r" , cpNull);
//                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);
                    ulERFlag &= ~RO_AXIS_Z;
                /* If a Z brake releasing is required... */
		ROServiceBrake(ROBOTFILE, TRUE);
//                if (lZAxisBrake[0] > 0)
//                {
                    /* ...activate the Z brake. */
                    /* WARNING: In ME domain card number 0 is GA_CARD_0. */
//                    ROWriteGalilPort(0, (int)lZAxisBrake[0], 1);
//                }
                TIDelay( 500 );

                /* Initiate home operation for Z or z axis... */
                GASendReceiveGalil(iTempCardNum, (char *)"HMZ\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGZ\r" , cpNull);

                /* ...and set the after-motion, error, and TS flags appropriately. */
                iTS = RO_AXIS_Z;
                ulAMFlag &= ~RO_AXIS_Z;

                /* Remember the encoder tolerance from the edge to the first index pulse
                 * in case a belt correction has to be made. See ROHomeAxis for more details. */
                if( cButtonPressed == 'J' )
                {
                    lLowDist = lEncRes[2];
                    lHighDist = 3*lEncRes[2];
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                }
                else
                {
                    lLowDist = lEncRes[2]*2-100;
                    lHighDist = lLowDist+200;
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                }
                break;

            case 'E':   /* W or w axis homing. */
                if (ulAuxAxis)
                {
                    /* Start by making sure NO axis in the system is in motion. */
                    if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                    {
                        TTBeepTP( ALERT );
                        break;
                    }

                    if (iDualArmCardNum)
                        TTPrintsAt( 4, 1, "Homing Dual Arm...  " );
                    if (iTrackCardNum)
                        TTPrintsAt( 4, 1, "Homing Track...     " );
                    /* Set home speeds for W or w axis. */
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeSpeed, OPERATIONAL_SPEED);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_ACCEL);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_DECEL);

                    /* Turn servo ON for W or w axis. */
                    /* If a dual arm exists, servo it on. */
                    if (iDualArmCardNum)
                        iCardToUse = iDualArmCardNum;
                    /* If a track exists, servo it on. */
                    if (iTrackCardNum)
                        iCardToUse = iTrackCardNum;
                    if (iCardToUse)
                    {
                        GASendReceiveGalil(iCardToUse, (char *)"SHW\r"  , cpNull);
//                        GASendReceiveGalil(iCardToUse, (char *)"EI 255,255\r" , cpNull);
                    }
                    ulERFlag &= ~ulAuxAxis;
                    TIDelay( 500 );

                    /* Initiate home operation for dual arm axis
                     * or track axis or vacuum indexer second theta axis. */
                    if (iCardToUse)
                    {
                        GASendReceiveGalil(iCardToUse, (char *)"HMW\r" , cpNull);
                        GASendReceiveGalil(iCardToUse, (char *)"BGW\r" , cpNull);
                    }

                    /* ...and set the after-motion, error, and TS flags appropriately. */
                    iTS = (int)(ulAuxAxis);
                    ulAMFlag &= ~ulAuxAxis;

                    /* Remember the encoder tolerance from the edge to the first index pulse
                     * in case a belt correction has to be made. See ROHomeAxis for more details. */
                    if( cButtonPressed == 'E' )
                    {
                        lLowDist = lAuxEncRes[3];
                        lHighDist = 3*lAuxEncRes[3];
//printf("W: low=%d high=%d\n",lLowDist,lHighDist);
                    }
                }
                else
                    TTBeepTP( ALERT );
                break;

            case CR:    /* If an enter is pressed, ignore it. */
                /* If any axis is in motion, start an internal loop until motion completes. */
                while( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
		    ROUpdateTS(FALSE);
                    SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                    /* Only an ESCape key press will exit the loop manually and also the test. */
                    if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                    {
                        /* Should not exit the loop so fake a key entered. */
                        cButtonPressed = ' ';
                        break;
                    }
                }
                break;

            case ESC:
            case 'Z':   /* User wants to abort the current operation. */
                /* Issue STop. */
                GASendReceiveGalil(iTempCardNum, (char *)"STXYZ\r" , cpNull);

                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (iCardToUse)
                    GASendReceiveGalil(iCardToUse, (char *)"STW\r" , cpNull);

                /* Indicate motion stop on the flag. */
                ulAMFlag |= ( ulAxisALLRbt | RO_AXIS_W );
                /* Wait for all motion to actually stop before issuing another Galil command. */
                TIDelay( 1000 );
                break;

            case EOF:
            case 0:     /* Finished homing; Key-Not-Pressed return code is 0. */
                /* No axis currently selected for homing. */
                if( !iTS )
                    break;

                /* Check if there was any error during homing. */
                if( ulERFlag & (unsigned long)(iTS) )
                {
                    TTPrintsAt( 4, 1, "Error               " );
                    TTBeepTP( ALERT );
                    ulAMFlag |= (unsigned long)(iTS);
                    iTS = iReturnValue = 0;
                    break;
                }

                /* Homing completed without any error. */
                if( !iReturnValue )
                {
                    /* Robot. */
                    /* Motion complete for all axes, proceed to find edge. */
                    if( (ulAMFlag & ulAllAMFlag) == ulAllAMFlag )
                    {
      			ROUpdateTS(FALSE);
                        /* Check if there was any error during homing. */
                        if( ulERFlag & (unsigned long)(iTS) )
                        {
                            TTPrintsAt( 4, 1, "Error               " );
                            TTBeepTP( ALERT );
                            ulAMFlag |= (unsigned long)(iTS);
                            iTS = iReturnValue = 0;
                            break;
                        }
                        TTPrintsAt( 4, 1, "Moving to Home Sw   " );

                        /* Issue galil command FE (Find Edge) for robot. */
                        GASendReceiveGalil(iTempCardNum, (char *)"SP500,500,500\r" , cpNull);
                        GASendReceiveGalil(iTempCardNum, (char *)"AC50000,50000,50000\r" , cpNull);
                        GASendReceiveGalil(iTempCardNum, (char *)"FEXYZ\r" , cpNull);
                        /* Issue galil command FE (Find Edge) for the auxilliary axis if appropriate. */
                        if ((iDualArmCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0))
                        {
                            GASendReceiveGalil(iTempCardNum, (char *)"SP,,,500\r" , cpNull);
                            GASendReceiveGalil(iTempCardNum, (char *)"AC,,,50000\r"  , cpNull);
                            GASendReceiveGalil(iTempCardNum, (char *)"FEW\r" , cpNull);
                        }

                        if( iTS == RO_AXIS_T )
                        {
                            ulAMFlag &= ~RO_AXIS_T;
                            GASendReceiveGalil(iTempCardNum, (char *)"BGX\r" , cpNull);
                        }
                        else if( iTS == RO_AXIS_R )
                        {
                            ulAMFlag &= ~RO_AXIS_R;
                            GASendReceiveGalil(iTempCardNum, (char *)"BGY\r" , cpNull);
                        }
                        else if( iTS == RO_AXIS_Z )
                        {
                            ulAMFlag &= ~RO_AXIS_Z;
                            GASendReceiveGalil(iTempCardNum, (char *)"BGZ\r" , cpNull);
                        }
                        else if( iTS == RO_AXIS_W )
                            GASendReceiveGalil(iTempCardNum, (char *)"BGW\r" , cpNull);
                        else if( iTS == (int)(ulAxisALLRbt) )
                        {
                            ulAMFlag &= (unsigned long)(~iTS);
                            GASendReceiveGalil(iTempCardNum, (char *)"BGXYZ\r" , cpNull);
                        }
                        else if( iTS == (int)(ulAxisALLRbt|RO_AXIS_W) )
                        {
                            ulAMFlag &= (unsigned long)(~iTS);
                            GASendReceiveGalil(iTempCardNum, (char *)"BGXYZW\r" , cpNull);
                        }
                        iReturnValue = 1;
                    }   /* Motion complete; find edge. */
                }   /* If !iReturnValue. */
                else
                {
                    /* Motion complete for all axes, proceed to find distance to index. */
                    if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                    {
			ROUpdateTS(FALSE);
                        break;
                    }
                    if( !((iDefineFlag & DFVAC514) && (iTS == RO_AXIS_T)) )
                    {
                        /* Display the distance-to-index for the requested axis. */
                        TTPrintsAt( 4, 1, "Distance to Index   " );
                        GASendReceiveGalil(iTempCardNum, (char *)"TP\r"  , cBuf);

                        pChar = strtok( cBuf, "," );

                        for( iReturnValue=0; iReturnValue<iMaxAxis; iReturnValue++ )
                        {
//                            if( iTS & (1<<(iReturnValue+((iTempCardNum==GA_CARD_1) ? 4 : 0))) )
                            if( iTS & (1<<iReturnValue) )
                            {
                            	lTP = labs( atol( pChar ) );
			    	sprintf(caTP, "%d", lTP);
                                /* W or w axis. */
                                if (iReturnValue == 3)
                                {
                                    TTPrintsAt( iReturnValue, 12, "       " );
//                                    TTPrintsAt( iReturnValue, 12, pChar+5 );
//                                    TTPrintsAt( iReturnValue, 12, pChar );
                                    TTPrintsAt( iReturnValue, 12, caTP );
                                }
                                /* Robot or pre-aligner axes. */
                                else
                                {
                                    TTPrintsAt( iReturnValue+1, 3, "       " );
//                                    TTPrintsAt( iReturnValue+1, 3, pChar );
//                                    TTPrintsAt( iReturnValue+1, 3, pChar+5 );
                                    TTPrintsAt( iReturnValue+1, 3, caTP );
                                }
                            }
                            pChar = strtok( NULL, ",\r" );
                        }
                    }
                    /* Turn off motor for requested axis and prompt a belt correction if needed. */
                    /* For robot. */
                    /* If a Z brake activation is required... */
                    if (lZAxisBrake[0] > 0)
                    {
                        /* ...activate the Z brake. */
                        /* WARNING: In ME domain card number 0 is GA_CARD_0. */
                        ROWriteGalilPort(0, (int)lZAxisBrake[0], 0);
                    }
                    TIDelay(100);
                    if( iTS == RO_AXIS_T )
                       GASendReceiveGalil(iTempCardNum, (char *)"MOX\r" , cpNull);
                    else if( iTS == RO_AXIS_R )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOY\r" , cpNull);
                    else if( iTS == RO_AXIS_Z )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOZ\r" , cpNull);
                    else if( iTS == RO_AXIS_W )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOW\r" , cpNull);
                    else if( iTS == (int)(ulAxisALLRbt) )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOXYZ\r" , cpNull);
                    else if( iTS == (int)(ulAxisALLRbt|RO_AXIS_W) )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOXYZW\r" , cpNull);
                    TTBeepTP( ALERT );

                    /* In the case of a single axis home only... */
                    if( (iTS != (int)(ulAxisALLRbt|RO_AXIS_W)) && (iTS != (int)(ulAxisALLRbt)) )
                    {
                        if( !((iDefineFlag & DFVAC514) && (iTS == RO_AXIS_T)) )
                        {
                            /* ...check that the distance-to-index is within tolerances. */
                            if( lTP < lLowDist || lTP > lHighDist )
                            {
                                TTBeepTP( LONGBELL );
                                TTPrintsAt( 4, 1, "Waiting for corr.   " );
                                TIDelay( 2500 );
                                /* Otherwise prompt the user to make a belt correction. */
                                ROCorrectDistance( iTempCardNum, lTP, (unsigned)(iTS) );

                                /* Redraw the main home test screen. */
                                TTClearScreen( );
                                TTPrintsAt( 1, 1, "T:" );
                                TTPrintsAt( 2, 1, "R:" );
                                TTPrintsAt( 3, 1, "Z:" );
                                if ((iDualArmCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0))
                                {
                                    TTPrintsAt( 3, 10, "W:" );
                                }
                                TTPrintsAt( 4, 1, "Select Axis to Home" );
                            }
                        }
                    }   /* Single axis home only. */
                    iTS = iReturnValue = 0;
                }   /* else   (If !iReturnValue). */
                break;

            default:
                TTPrintsAt( 4, 1, "Select Axis to Home " );
                TTBeepTP( ALERT );
        }
    } while( (cButtonPressed != CR) && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* HOME TEST COMPLETED!!! */
    /* If a Z brake activation is required... */
    ROServiceBrake(ROBOTFILE, FALSE);
//    if (lZAxisBrake[0] > 0)
//    {
        /* ...activate the Z brake. */
        /* WARNING: In ME domain card number 0 is GA_CARD_0. */
//        ROWriteGalilPort(0, (int)lZAxisBrake[0], 0);
//    }
    /* Wait for brake to engage. */
    TIDelay(100);
    /* Turn off motor... */
    GASendReceiveGalil(iTempCardNum, (char *)"MOXYZW\r" , cpNull);
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (iCardToUse)
        GASendReceiveGalil(iCardToUse, (char *)"MOW\r" , cpNull);

    /* ...and restore the operating conditions as saved in the beginning. */
//    IOWriteIO( -1, 0, iPortBase+4 );
    /* Restore speed, acceleration, deceleration, and torque limit for robot. */
    ROSetParameter(FALSE, ulAxisALLRbt, lSaveSpeed, OPERATIONAL_SPEED);
    ROSetParameter(FALSE, ulAxisALLRbt, lSaveAccel, OPERATIONAL_ACCEL);
    ROSetParameter(FALSE, ulAxisALLRbt, lSaveDecel, OPERATIONAL_DECEL);
    ROSetParameter(FALSE, ulAxisALLRbt, lSaveTorqueLimit, TORQUE_LIMIT);
    if (ulAuxAxis)
    {
        lTempVal[0] = lTempVal[1] = lTempVal[2] = 0;
        lTempVal[3] = lSaveSpeed[3];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_SPEED);

        lTempVal[0] = lTempVal[1] = lTempVal[2] = 0;
        lTempVal[3] = lSaveAccel[3];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_ACCEL);

        lTempVal[0] = lTempVal[1] = lTempVal[2] = 0;
        lTempVal[3] = lSaveDecel[3];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_DECEL);

        lTempVal[0] = lTempVal[1] = lTempVal[2] = 0;
        lTempVal[3] = lSaveTorqueLimit[3];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, TORQUE_LIMIT);
    }
    TIDelay( 500 );

    /* Restore the S-curve motion profile flag (enable/disable) */
//    ROEnableSCurveProfile( ulAxisALLRbt, lTAFlags );  

    return SUCCESS;

preTest:
    /* Check to see if a track exists in the system. */
    if (ROGetSpecialAxis(RO_TRACK))
    {
        /* Find out which card the track is on. */
        if ((ROGetSpecialAxis(RO_TRACK) > RO_AXIS_W))
            iTrackCardNum = GA_CARD_1;
    }
    /* Decide if the additional axis should be displayed. */
    if (((iDualArmCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1)))
    {
        iMaxAxis = 4;
    }
    else if ((ROGetSpecialAxis(RO_INDEXER_T2)) && (iCardNoArg == 1))
    {
        iMaxAxis = 4;
    }
    /* Or not displayed. */
    else
    {
        iDualArmCardNum = 0;
        iTrackCardNum = 0;
    }
    iPortBase = IO_PRE_INPUT_K;
    iWhichParm = PREALIGNFILE;
    GAAmpInit();
    TIDelay( 1500 );

    /* Read INPUT G port, and check the status. */
    iTint = inb(iPortBase+3);
	iTint = 0;
    iTint &= 0xFC;


    if( iTint )
    {
        TTPrintsAt( 1, 1, "Connection Error" );
        TTPrintsAt( 2, 1, "Check Connection" );
        GASendReceiveGalil(iTempCardNum, (char *)"MO\r", cpNull);
        IOWriteIO( -1, 0, iPortBase+4 );
        TIDelay( 500 );
        return SUCCESS;
    }

    /* Allow user to select an axis to Home. */
    TTClearScreen();
    TTPrintsAt( 1, 1, "t:" );
    TTPrintsAt( 2, 1, "r:" );
    TTPrintsAt( 3, 1, "z:" );

    if ((iTrackCardNum) || (iDualArmCardNum))
    {
        TTPrintsAt( 3, 10, "w:" );
    }
    TTPrintsAt( 4, 1, "Select Axis to Home" );

    /* If an auxilliary axis is to be shown, get its axis designation. */
    if (iTrackCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_TRACK);
    if (iVacExistsArg)
        ulAuxAxis = ROGetSpecialAxis(RO_INDEXER_T2);

    if (iVacExistsArg)
    {
        /* Get the vacuum indexer brake values from the datafile. */
        FIOGetParamVals(PREALIGNFILE, H_VALUE, lZAxisBrake);
    }
    /* Save Galil values to be restored before exiting. */
    /* Save old pre-aligner axes values. */
    ROGetParameter(FALSE, ulAxisallPre, lSaveSpeed, OPERATIONAL_SPEED);
    ROGetParameter(FALSE, ulAxisallPre, lSaveAccel, OPERATIONAL_ACCEL);
    ROGetParameter(FALSE, ulAxisallPre, lSaveDecel, OPERATIONAL_DECEL);
    ROGetParameter(FALSE, ulAxisallPre, lSaveTorqueLimit, TORQUE_LIMIT);

    /* Set new pre-aligner torque limits. */
    lTempVal[4] = lTempVal[5] = lTempVal[6] = lTempVal[7] = 9999;
    ROSetParameter(FALSE, ulAxisallPre, lTempVal, TORQUE_LIMIT);

    if (ulAuxAxis)
    {
        /* Save old auxilliary axes values if appropriate. */
        lTempVal[4] = lTempVal[5] = lTempVal[6] = lTempVal[7] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_SPEED);
        lSaveSpeed[3] = lTempVal[3];

        lTempVal[4] = lTempVal[5] = lTempVal[6] = lTempVal[7] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_ACCEL);
        lSaveAccel[3] = lTempVal[3];

        lTempVal[4] = lTempVal[5] = lTempVal[6] = lTempVal[7] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_DECEL);
        lSaveDecel[3] = lTempVal[3];

        lTempVal[4] = lTempVal[5] = lTempVal[6] = lTempVal[7] = 0;
        ROGetParameter(FALSE, ulAuxAxis, lTempVal, TORQUE_LIMIT);
        lSaveTorqueLimit[3] = lTempVal[3];

        /* Set new auxilliary axis torque limits. */
        lTempVal[4] = lTempVal[5] = lTempVal[6] = lTempVal[7] = 9999;
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, TORQUE_LIMIT);
    }

    /* get the S-curve profile flag value either enable(1) or disable(0) */
//    if( ROReadSCurveProfile(ulAxisallPre,lTAFlags) == FAILURE )
//        return FAILURE;
//    ROEnableSCurveProfile( ulAxisallPre, lTAFlagOff );  /* set the S-curve motion profile flag (enable/disable) */


    do
    {
	ROUpdateTS(FALSE);

        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);

        /* These following data are changed in the unscale/scale functions.
         * We need to get the data that is stored in Parameters file
         * instead using the data that is sent to Galil. */
        FIOGetParamVals(iWhichParm, HOME_SPEED, lHomeSpeed);
        FIOGetParamVals(iWhichParm, HOME_ACCEL, lHomeAccel);
        FIOGetParamVals(iWhichParm, ENCODER_RESOLUTION, lEncRes);

        if (ulAuxAxis)
        {
            /* Get the same info for the auxilliary axis if appropriate. */
            FIOGetParamVals(AUXFILE, HOME_SPEED, lAuxHomeSpeed);
            ROArrangeGetArrayForW(ulAuxAxis, lAuxHomeSpeed);
            FIOGetParamVals(AUXFILE, HOME_ACCEL, lAuxHomeAccel);
            ROArrangeGetArrayForW(ulAuxAxis, lAuxHomeAccel);
            FIOGetParamVals(AUXFILE, ENCODER_RESOLUTION, lAuxEncRes);
            ROArrangeGetArrayForW(ulAuxAxis, lAuxEncRes);
        }

        switch( cButtonPressed )
        {
            case 'A':   /* User wants to home all axes. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing all axes...  " );
                /* Set home speeds for pre-aligner axes. */
                ROSetParameter(FALSE, ulAxisallPre, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, ulAxisallPre, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, ulAxisallPre, lHomeAccel, OPERATIONAL_DECEL);
                if (ulAuxAxis)
                {
                    /* Set home speeds for auxilliary axes if appropriate. */
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeSpeed, OPERATIONAL_SPEED);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_ACCEL);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_DECEL);
                }

                /* Turn servo ON for robot or pre-aligner. */
                GASendReceiveGalil(iTempCardNum, (char *)"SHEFG\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);
                /* If a Z brake releasing is required... */
                TIDelay( 500 );
                /* If an auxilliary axis exists, servo it on. */
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (iCardToUse)
                {
                    GASendReceiveGalil(iCardToUse, (char *)"SHH\r" , cpNull);
                    GASendReceiveGalil(iCardToUse, (char *)"EI 255,255\r", cpNull);
                }
                TIDelay( 500 );

                /* Initiate home operation for all axes and set the after-motion,
                 * error, and TS flags appropriately. */
                /* All pre-aligner axes. */
                ulERFlag &= ~ulAxisallPre;
                GASendReceiveGalil(iTempCardNum, (char *)"HMFG\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGFG\r" , cpNull);
                ulAMFlag &= ~(RO_AXIS_r|RO_AXIS_z);
                iTS = (int)(ulAxisallPre);

                /* If a track exists, home it. */
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (iCardToUse)
                {
                    GASendReceiveGalil(iCardToUse, (char *)"HMH\r" , cpNull);
                    GASendReceiveGalil(iCardToUse, (char *)"BGH\r" , cpNull);
                }
                if (ulAuxAxis)
                {
                    ulERFlag &= ~ulAuxAxis;
                    ulAMFlag &= ~ulAuxAxis;
                    iTS = (int)(ulAuxAxis);
                }
                break;  /* case A */

            case 'H':
            case 'M':   /* T or t axis homing. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                /* The pre-aligner t axis doesn't have a home switch. */
                TTPrintsAt( 4, 1, "Theta homed...      " );
                iTS = 0;
				break;

                /* Set home speeds for pre-aligner t axis. */
                ROSetParameter(FALSE, RO_AXIS_t, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, RO_AXIS_t, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, RO_AXIS_t, lHomeAccel, OPERATIONAL_DECEL);

                /* Turn servo ON for T or t axis. */
                // VAC514 robots both servos needs to be on for motion of either
                GASendReceiveGalil(iTempCardNum, (char *)"SHE\r" , cpNull);
                ulERFlag &= ~RO_AXIS_t;
                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);
                TIDelay( 500 );

                /* Initiate home operation for R or r axis... */
                GASendReceiveGalil(iTempCardNum, (char *)"HME\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGE\r" , cpNull);

                /* ...and set the after-motion, error, and TS flags appropriately. */
                iTS = RO_AXIS_t;
                ulAMFlag &= ~RO_AXIS_t;

                /* Remember the encoder tolerance from the edge to the first index pulse
                 * in case a belt correction has to be made. See ROHomeAxis for more details. */
                if( cButtonPressed == 'H' )
                {
                    lLowDist = lEncRes[4];
                    lHighDist = 3*lEncRes[4];
                }
                else
                {
                    lLowDist = lEncRes[4]*2-100;
                    lHighDist = lLowDist+200;
                }
                break;

            case 'I':
            case 'N':   /* R or r axis homing. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                /* This is a vacuum indexer system with no Z1 axis. */
                if (iVacExistsArg && !ROGetSpecialAxis(RO_INDEXER_Z1))
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing Radial...    " );
                /* Set home speeds for pre-aligner r axis. */
                ROSetParameter(FALSE, RO_AXIS_r, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, RO_AXIS_r, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, RO_AXIS_r, lHomeAccel, OPERATIONAL_DECEL);

                /* Turn servo ON for R or r axis. */
                // VAC514 robots both servos needs to be on for motion of either
                GASendReceiveGalil(iTempCardNum, (char *)"SHF\r" , cpNull);

                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);

                ulERFlag &= ~RO_AXIS_r;

                /* Initiate home operation for R or r axis... */
                GASendReceiveGalil(iTempCardNum, (char *)"HMF\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGF\r" , cpNull);

                iTS = RO_AXIS_r;
                ulAMFlag &= ~RO_AXIS_r;

                /* Remember the encoder tolerance from the edge to the first index pulse
                 * in case a belt correction has to be made. See ROHomeAxis for more details. */
                if( cButtonPressed == 'I' )
                {
                    lLowDist = lEncRes[5];
                    lHighDist = 3*lEncRes[5];
                }
                else
                {
                    lLowDist = lEncRes[5]*2-100;
                    lHighDist = lLowDist+200;
                }
                break;

            case 'J':
            case 'O':   /* Z or z axis homing. */
                /* Start by making sure NO axis in the system is in motion. */
                if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
                    TTBeepTP( ALERT );
                    break;
                }

                /* This is a vacuum indexer system with no Z2 axis. */
                if (iVacExistsArg && !ROGetSpecialAxis(RO_INDEXER_Z2))
                {
                    TTBeepTP( ALERT );
                    break;
                }

                TTPrintsAt( 4, 1, "Homing z...         " );
                /* Set home speeds for pre-aligner z axis. */
                ROSetParameter(FALSE, RO_AXIS_z, lHomeSpeed, OPERATIONAL_SPEED);
                ROSetParameter(FALSE, RO_AXIS_z, lHomeAccel, OPERATIONAL_ACCEL);
                ROSetParameter(FALSE, RO_AXIS_z, lHomeAccel, OPERATIONAL_DECEL);

                /* Turn servo ON for Z or z axis. */
                GASendReceiveGalil(iTempCardNum, (char *)"SHG\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"EI 255,255\r" , cpNull);
                ulERFlag &= ~RO_AXIS_z;

                /* Initiate home operation for Z or z axis... */
                GASendReceiveGalil(iTempCardNum, (char *)"HMG\r" , cpNull);
                GASendReceiveGalil(iTempCardNum, (char *)"BGG\r" , cpNull);

                /* ...and set the after-motion, error, and TS flags appropriately. */
                iTS = RO_AXIS_z;
                ulAMFlag &= ~RO_AXIS_z;

                /* Remember the encoder tolerance from the edge to the first index pulse
                 * in case a belt correction has to be made. See ROHomeAxis for more details. */
                if( cButtonPressed == 'J' )
                {
                    lLowDist = lEncRes[6];
                    lHighDist = 3*lEncRes[6];
                }
                else
                {
                    lLowDist = lEncRes[6]*2-100;
                    lHighDist = lLowDist+200;
                }
                break;

            case 'E':   /* W or w axis homing. */
                if (ulAuxAxis)
                {
                    /* Start by making sure NO axis in the system is in motion. */
                    if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                    {
                        TTBeepTP( ALERT );
                        break;
                    }

                    if (iTrackCardNum)
                        TTPrintsAt( 4, 1, "Homing Track...     " );
                    if (iVacExistsArg && iTempCardNum == GA_CARD_1)
                        TTPrintsAt( 4, 1, "Homing Vac Theta2..." );
                    /* Set home speeds for W or w axis. */
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeSpeed, OPERATIONAL_SPEED);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_ACCEL);
                    ROSetParameter(FALSE, ulAuxAxis, lAuxHomeAccel, OPERATIONAL_DECEL);

                    /* Turn servo ON for W or w axis. */
                    /* If a track exists, servo it on. */
                    if (iTrackCardNum)
                        iCardToUse = iTrackCardNum;
                    /* If a vacuum indexer second theta axis exists, servo it on. */
                    if (iCardToUse)
                    {
                        GASendReceiveGalil(iCardToUse, (char *)"SHH\r"  , cpNull);
                        GASendReceiveGalil(iCardToUse, (char *)"EI 255,255\r" , cpNull);
                    }
                    ulERFlag &= ~ulAuxAxis;
                    TIDelay( 500 );

                    /* Initiate home operation for dual arm axis
                     * or track axis or vacuum indexer second theta axis. */
                    if (iCardToUse)
                    {
                        GASendReceiveGalil(iCardToUse, (char *)"HMH\r" , cpNull);
                        GASendReceiveGalil(iCardToUse, (char *)"BGH\r" , cpNull);
                    }

                    /* ...and set the after-motion, error, and TS flags appropriately. */
                    iTS = (int)(ulAuxAxis);
                    ulAMFlag &= ~ulAuxAxis;

                    /* Remember the encoder tolerance from the edge to the first index pulse
                     * in case a belt correction has to be made. See ROHomeAxis for more details. */
                    if( cButtonPressed == 'E' )
                    {
                        lLowDist = lEncRes[7];
                        lHighDist = 3*lEncRes[7];
                    }
                }
                else
                    TTBeepTP( ALERT );
                break;

            case CR:    /* If an enter is pressed, ignore it. */
                /* If any axis is in motion, start an internal loop until motion completes. */
                while( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                {
		    ROUpdateTS(FALSE);
                    SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                    /* Only an ESCape key press will exit the loop manually and also the test. */
                    if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                    {
                        /* Should not exit the loop so fake a key entered. */
                        cButtonPressed = ' ';
                        break;
                    }
                }
                break;

            case ESC:
            case 'Z':   /* User wants to abort the current operation. */
                /* Issue STop. */
                GASendReceiveGalil(iTempCardNum, (char *)"STEFG\r" , cpNull);

                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (iCardToUse)
                    GASendReceiveGalil(iCardToUse, (char *)"STH\r" , cpNull);

                /* Indicate motion stop on the flag. */
                ulAMFlag |= ( ulAxisallPre | RO_AXIS_w );
                /* Wait for all motion to actually stop before issuing another Galil command. */
                TIDelay( 1000 );
                break;

            case EOF:
            case 0:     /* Finished homing; Key-Not-Pressed return code is 0. */
                /* No axis currently selected for homing. */
                if( !iTS )
                    break;

                /* Check if there was any error during homing. */
                if( ulERFlag & (unsigned long)(iTS) )
                {
                    TTPrintsAt( 4, 1, "Error               " );
                    TTBeepTP( ALERT );
                    ulAMFlag |= (unsigned long)(iTS);
                    iTS = iReturnValue = 0;
                    break;
                }

                /* Homing completed without any error. */
                if( !iReturnValue )
                {
                    /* Motion complete for all axes, proceed to find edge. */
                    if( (ulAMFlag & ulAllAMFlag) == ulAllAMFlag )
                    {
                        /* Check if there was any error during homing. */
                        if( ulERFlag & (unsigned long)(iTS) )
                        {
                            TTPrintsAt( 4, 1, "Error               " );
                            TTBeepTP( ALERT );
                            ulAMFlag |= (unsigned long)(iTS);
                            iTS = iReturnValue = 0;
                            break;
                        }
                        TTPrintsAt( 4, 1, "Moving to Home Sw   " );

                        /* Issue galil command FE (Find Edge) for pre-aligner. */
                        GASendReceiveGalil(iTempCardNum, (char *)"SP,,,,500,500,500\r" , cpNull);
                        GASendReceiveGalil(iTempCardNum, (char *)"AC,,,,50000,50000,50000\r" , cpNull);
                        GASendReceiveGalil(iTempCardNum, (char *)"FEEFG\r" , cpNull);

                        /* Issue galil command FE (Find Edge) for the auxilliary axis if appropriate. */
                        if ((iDualArmCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1) || (ROGetSpecialAxis(RO_INDEXER_T2)))
                        {
                            GASendReceiveGalil(iTempCardNum, (char *)"SP,,,,,,,500\r" , cpNull);
                            GASendReceiveGalil(iTempCardNum, (char *)"AC,,,,,,,50000\r" , cpNull);
                            GASendReceiveGalil(iTempCardNum, (char *)"FEH\r"  , cpNull);
                        }

                        if( iTS == RO_AXIS_t )
                            GASendReceiveGalil(iTempCardNum, (char *)"BGE\r"  , cpNull);
                        else if( iTS == RO_AXIS_r )
                            GASendReceiveGalil(iTempCardNum, (char *)"BGF\r"  , cpNull);
                        else if( iTS == RO_AXIS_z )
                            GASendReceiveGalil(iTempCardNum, (char *)"BGG\r"  , cpNull);
                        else if( iTS == RO_AXIS_w )
                            GASendReceiveGalil(iTempCardNum,  (char *)"BGH\r"  , cpNull);
                        else if( iTS == (int)(ulAxisallPre) )
                        {
                            GASendReceiveGalil(iTempCardNum, (char *)"BGFG\r"  , cpNull);
                            ulAMFlag &= ~(RO_AXIS_r|RO_AXIS_z);
                            iReturnValue = 1;
                            break;
                        }
                        else if( iTS == (int)(ulAxisallPre|RO_AXIS_w) )
                        {
                            GASendReceiveGalil(iTempCardNum, (char *)"BGFGH\r"  , cpNull);
                            ulAMFlag &= ~(RO_AXIS_r|RO_AXIS_z|RO_AXIS_w);
                            iReturnValue = 1;
                            break;
                        }
                        ulAMFlag &= (unsigned long)(~iTS);
                        iReturnValue = 1;
                    }   /* Motion complete; find edge. */
                }   /* If !iReturnValue. */
                else
                {
                    /* Motion complete for all axes, proceed to find distance to index. */
                    if( (ulAMFlag & ulAllAMFlag) != ulAllAMFlag )
                    {
			ROUpdateTS(FALSE);
                        break;
                    }
                    /* Turn off motor for requested axis and prompt a belt correction if needed. */
                    /* For pre-aligner. */
                    if( iTS == RO_AXIS_t )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOE\r" , cpNull);
                    else if( iTS == RO_AXIS_r )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOF\r" , cpNull);
                    else if( iTS == RO_AXIS_z )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOG\r" , cpNull);
                    else if( iTS == RO_AXIS_w )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOH\r" , cpNull);
                    else if( iTS == (int)(ulAxisallPre) )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOEFG\r" , cpNull);
                    else if( iTS == (int)(ulAxisallPre|RO_AXIS_w) )
                        GASendReceiveGalil(iTempCardNum, (char *)"MOEFGH\r" , cpNull);
                    /* In the case of a single axis home only... */
                    if( (iTS != (int)(ulAxisallPre|RO_AXIS_w)) && (iTS != (int)(ulAxisallPre)) )
                    {
                        /* ...check that the distance-to-index is within tolerances. */
                        if( lTP < lLowDist || lTP > lHighDist )
                        {
                            TTBeepTP( LONGBELL );
                            TTPrintsAt( 4, 1, "Waiting for corr.   " );
                            TIDelay( 2500 );

                            /* Otherwise prompt the user to make a belt correction. */
                            ROCorrectDistance( iTempCardNum, lTP, (unsigned)(iTS) );

                            /* Redraw the main home test screen. */
                            TTClearScreen();
                            TTPrintsAt( 1, 1, "t:" );
                            TTPrintsAt( 2, 1, "r:" );
                            TTPrintsAt( 3, 1, "z:" );
                            if ((iDualArmCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1))
                            {
                                TTPrintsAt( 3, 10, "w:" );
                            }
                            TTPrintsAt( 4, 1, "Select Axis to Home" );
                        }   /* Single axis home only. */
                    }   /* For pre-aligner. */
                    iTS = iReturnValue = 0;
                }   /* else   (If !iReturnValue). */
                break;

            default:
                TTPrintsAt( 4, 1, "Select Axis to Home " );
                TTBeepTP( ALERT );
        }
    } while( (cButtonPressed != CR) && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* HOME TEST COMPLETED!!! */
    /* Turn off motor... */
    GASendReceiveGalil(iTempCardNum, (char *)"MOEFG\r" , cpNull);
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (iVacExistsArg)
        iCardToUse = iTempCardNum;
    if (iCardToUse)
        GASendReceiveGalil(iCardToUse, (char *)"MOH\r" , cpNull);

    /* ...and restore the operating conditions as saved in the beginning. */
    IOWriteIO( -1, 0, iPortBase+4 );
    /* Restore speed, acceleration, deceleration, and torque limit for pre-aligner. */
    ROSetParameter(FALSE, ulAxisallPre, lSaveSpeed, OPERATIONAL_SPEED);
    ROSetParameter(FALSE, ulAxisallPre, lSaveAccel, OPERATIONAL_ACCEL);
    ROSetParameter(FALSE, ulAxisallPre, lSaveDecel, OPERATIONAL_DECEL);
    ROSetParameter(FALSE, ulAxisallPre, lSaveTorqueLimit, TORQUE_LIMIT);

    if (ulAuxAxis)
    {
        lTempVal[4] = lTempVal[5] = lTempVal[6] = 0;
        lTempVal[7] = lSaveSpeed[7];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_SPEED);

        lTempVal[4] = lTempVal[5] = lTempVal[6] = 0;
        lTempVal[7] = lSaveAccel[7];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_ACCEL);

        lTempVal[4] = lTempVal[5] = lTempVal[6] = 0;
        lTempVal[7] = lSaveDecel[7];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, OPERATIONAL_DECEL);

        lTempVal[4] = lTempVal[5] = lTempVal[6] = 0;
        lTempVal[7] = lSaveTorqueLimit[7];
        ROSetParameter(FALSE, ulAuxAxis, lTempVal, TORQUE_LIMIT);
    }
    TIDelay( 500 );

    /* Restore the S-curve motion profile flag (enable/disable) */
//    ROEnableSCurveProfile( ulAxisallPre, lTAFlags );  

    return SUCCESS;

}



/****************************************************************
 *
 * Function:    ROAdjustRAtHome
 *
 * Abstract:    This starts by asking the if the home position
 *      is to be adjusted. If "Yes", then initiate a find edge for
 *      the radial axis. Then wait for the axis to finish moving.
 *      Finally ask the user to move the axis to the proper
 *      home switch position and wait for an Enter keypress before
 *      exiting. This is used to adjust the belt position with respect
 *      to the encoder index pulse and the home switch.
 *
 * Parameters:
 *      ulAxisArg   (in) The axis to operate on, R or W
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROAdjustRAtHome(unsigned long ulAxisArg)
{
    int iReturn = SUCCESS;
    int iPerformed = FALSE;
    char cKeyPressed, cButtonPressed;

    TTClearScreen( );
    if( ulAxisArg == RO_AXIS_R )
        TTPrintsAt( 2, 1, "  Adjust R at HOME?" );
    if( ulAxisArg == RO_AXIS_W )
        TTPrintsAt( 2, 1, "  Adjust W at HOME?" );
    TTPrintsAt( 4, 1, " Y-Yes         N-No" );

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
        switch( cKeyPressed )
        {
            case 'Y':   /* User wants to adjust R or W home. */
                /* Y/N response from "Adjust R (or W) at HOME?" already processed. Awaiting Enter. */
                if( iPerformed ) break;

                /* Proceed to homing xis. */
                TTPrintsAt( 4, 1, "\r\nMoving to Home Sw..." );

                /* Servo ON axis.
                 * Homing is done by Galil's FE command (Find Edge). */
                if( ulAxisArg == RO_AXIS_R )
                {
                    if(iDefineFlag & DFVAC514)
                    {
                        ROElectronicGearingOFF();
                        ROElectronicGearingON('Y', 'X', VAC514_R_GEARING_RATIO, FALSE);
                        GASendReceiveGalil(GA_CARD_0,  "SHXY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
                        TIDelay( 500 );
                        ulAMFlag &= ~RO_AXIS_R;
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGY\r" , cpNull);
                        while((ulAMFlag & RO_AXIS_R) != RO_AXIS_R)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        GASendReceiveGalil(GA_CARD_0, (char *)"SP,500\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"AC,1000\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"DC,1000\r" , cpNull);

                        ulAMFlag &= ~RO_AXIS_R;
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGY\r" , cpNull);
                        while((ulAMFlag & RO_AXIS_R) != RO_AXIS_R)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        ulAMFlag &= ~RO_AXIS_T;
                        ROElectronicGearingOFF();
                        ROElectronicGearingON('X', 'Y', VAC514_T_GEARING_RATIO, FALSE);
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEX\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGX\r" , cpNull);

                        while((ulAMFlag & RO_AXIS_T) != RO_AXIS_T)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        GASendReceiveGalil(GA_CARD_0, (char *)"SP500\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"AC,1000\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"DC,1000\r" , cpNull);
                        ulAMFlag &= ~RO_AXIS_T;
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEX\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGX\r" , cpNull);

                        while((ulAMFlag & RO_AXIS_T) != RO_AXIS_T)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;
                    }
                    else
                    {
                        GASendReceiveGalil(GA_CARD_0,  "SHY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
                        TIDelay( 500 );
                        GASendReceiveGalil(GA_CARD_0, "FEY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, "BGY\r" , cpNull);
                    }
                }
                if( ulAxisArg == RO_AXIS_W )
                {
                    GASendReceiveGalil(GA_CARD_0, "SHW\r" , cpNull);
                    GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
                    TIDelay( 500 );
                    GASendReceiveGalil(GA_CARD_0, "FEW\r" , cpNull);
                    GASendReceiveGalil(GA_CARD_0, "BGW\r" , cpNull);
                }
                if(!(iDefineFlag & DFVAC514))
                {
                    ulERFlag &= ~ulAxisArg;
                    ulAMFlag &= ~ulAxisArg;
                }

                /* Wait until motion complete or aborted by user. */
                while( (ulAMFlag & ulAxisArg) != ulAxisArg )
                {
		    ROUpdateTS(0);
                    SERGetcRxBuff(SERGetTTPort(), &cKeyPressed, FALSE);

                    /* Key ESC aborts the homing operation. */
                    if ( (cKeyPressed == ESC) || (cKeyPressed == 'Z') )
                    {
                        iReturn = FAILURE;
                        goto exit;
                    }

                    /* Galil error also aborts the homing operation. */
                    if( ulERFlag & ulAxisArg )
                    {
                        TTPrintsAt( 4, 1, "\n\rError\n\r" );
                        TTBeepTP( ALERT );
                        iReturn = FAILURE;
                        goto exit;
                    }
                }

                /* Now, axis is Homed, proceed to let user adjust axis manually. */
                iPerformed = TRUE;
                TTClearScreen( );
                if( ulAxisArg == RO_AXIS_R )
                    TTPrintsAt( 1, 1, "Move R to proper" );
                if( ulAxisArg == RO_AXIS_W )
                    TTPrintsAt( 1, 1, "Move W to proper" );
                TTPrintsAt( 2, 1, "HOME SW position." );
                TTPrintsAt( 3, 1, "Press \"Enter\" when" );
                TTPrintsAt( 4, 1, "done or ESC" );
                break;

            case 'N':   /* User does NOT want to adjust home. */
                /* Y/N response from "Adjust R (or W) at HOME?" already processed. Awaiting Enter.*/
                if( iPerformed ) break;

                /* No error conditions, so exit happily. */
                iReturn = SUCCESS;
                goto exit;

            case 'Z':
            case ESC:   /* Abort homing operation. */
                iReturn = FAILURE;
                goto exit;

            case CR:    /* User is done with manual adjustments. */
                /* Homing NOT done. Manual adjustments can NOT be complete. */
                if( !iPerformed ) break;

                /* Otherwise exit happily. */
                iReturn = SUCCESS;
                goto exit;

            case 0:     /* 0 is normal return for Key-Not-Pressed mode. */
            case EOF:   /* EOF is normal return from SERGetcRxBuff if no data is available. */
                break;

            default:
                TTPrintsAt( 4, 1, "Bad Key           " );
                TTBeepTP( ALERT );
        }
    } while ( 1 );

exit:
    /* STop motion, turn off MOtors, and exit. */
    GASendReceiveGalil(GA_CARD_0, "ST\r" , cpNull);
    TIDelay( 200 );
    GASendReceiveGalil(GA_CARD_0, "MO\r" , cpNull);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROAdjustRAtIndex
 *
 * Abstract:    This starts by asking the if the index position
 *      is to be adjusted. If "Yes", then initiate a home sequence for
 *      the radial axis. Then wait for the axis to finish moving.
 *      Finally ask the user to move the axis to the proper
 *      index position and wait for an Enter keypress before
 *      exiting. This is used to adjust the belt position with respect
 *      to the encoder index pulse and the home switch.
 *
 * Parameters:
 *      ulAxisArg   (in) The axis to operate on, R or W
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROAdjustRAtIndex(unsigned long ulAxisArg)
{
    int iPerformed = FALSE;
    int iReturn = SUCCESS;
    char cKeyPressed, cButtonPressed;
    long lParms[8];

    TTClearScreen( );
    if( ulAxisArg == RO_AXIS_R )
        TTPrintsAt( 2, 1, "  Adjust R at INDEX?" );
    if( ulAxisArg == RO_AXIS_W )
        TTPrintsAt( 2, 1, "  Adjust W at INDEX?" );
    TTPrintsAt( 4, 1, " Y-Yes         N-No" );

    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cKeyPressed, FALSE);
        switch( cKeyPressed )
        {
            case 'Y':   /* User wants to perform R or W index adjustment. */
                /* Y/N response from "Adjust R (or W) at HOME?" already processed. Awaiting Enter. */
                if( iPerformed ) break;

                /* Proceed to homing axis. */
                TTPrintsAt( 4, 1, "\r\nMoving to Index..." );

                /* Servo ON axis.
                 * Homing is done by Galil's HM command (HoMe). */
                if( ulAxisArg == RO_AXIS_R )
                {
                    //VAC514 robots homing requires special procedure
                    if(iDefineFlag & DFVAC514)
                    {
                        ROElectronicGearingOFF();
                        ROElectronicGearingON('Y', 'X', VAC514_R_GEARING_RATIO, FALSE);
                        GASendReceiveGalil(GA_CARD_0, "SHXY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
                        TIDelay( 500 );

                        ulAMFlag &= ~RO_AXIS_R;
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGY\r" , cpNull);
                        while((ulAMFlag & RO_AXIS_R) != RO_AXIS_R)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        GASendReceiveGalil(GA_CARD_0, (char *)"SP,500\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"AC,1000\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"DC,1000\r" , cpNull);

                        ulAMFlag &= ~RO_AXIS_R;
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGY\r" , cpNull);
                        while((ulAMFlag & RO_AXIS_R) != RO_AXIS_R)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        ulAMFlag &= ~RO_AXIS_T;
                        ROElectronicGearingOFF();
                        ROElectronicGearingON('X', 'Y', VAC514_T_GEARING_RATIO, FALSE);
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEX\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGX\r" , cpNull);

                        while((ulAMFlag & RO_AXIS_T) != RO_AXIS_T)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        GASendReceiveGalil(GA_CARD_0, (char *)"SP500\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"AC1000\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"DC1000\r" , cpNull);
                        ulAMFlag &= ~RO_AXIS_T;
                        GASendReceiveGalil(GA_CARD_0, (char *)"FEX\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGX\r" , cpNull);

                        while((ulAMFlag & RO_AXIS_T) != RO_AXIS_T)
                        {
                            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
                            /* Only an ESCape key press will exit the loop manually and also the test. */
                            if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                                break;
                        }
                        if ((cButtonPressed == ESC) || (cButtonPressed == 'Z'))
                            break;

                        ROElectronicGearingOFF();
                        ROElectronicGearingON('Y', 'X', VAC514_R_GEARING_RATIO, FALSE);
                        GASendReceiveGalil(GA_CARD_0, (char *)"JG, 500\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"FIY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, (char *)"BGY\r" , cpNull);
                    }
                    else
                    {
                        GASendReceiveGalil(GA_CARD_0, "SHY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
                        TIDelay( 500 );
                        GASendReceiveGalil(GA_CARD_0, "HMY\r" , cpNull);
                        GASendReceiveGalil(GA_CARD_0, "BGY\r" , cpNull);
                    }
                }
                if( ulAxisArg == RO_AXIS_W )
                {
                    GASendReceiveGalil(GA_CARD_0, "SHW\r" , cpNull);
                    GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
                    TIDelay( 500 );
                    GASendReceiveGalil(GA_CARD_0, "HMW\r" , cpNull);
                    GASendReceiveGalil(GA_CARD_0, "BGW\r" , cpNull);
                }
                ulERFlag &= ~ulAxisArg;
                ulAMFlag &= ~ulAxisArg;

                /* Wait until motion complete or aborted by user. */
                while( (ulAMFlag & ulAxisArg) != ulAxisArg )
                {
                    SERGetcRxBuff(SERGetTTPort(), &cKeyPressed, FALSE);

                    /* Key ESC aborts the homing operation. */
                    if ( (cKeyPressed == ESC) || (cKeyPressed == 'Z') )
                    {
                        iReturn = FAILURE;
                        goto exit;
                    }

                    /* Galil error also aborts the homing operation. */
                    if( ulERFlag & ulAxisArg )
                    {
                        TTPrintsAt( 4, 1, "\n\rError\n\r" );
                        TTBeepTP( ALERT );
                        iReturn = FAILURE;
                        goto exit;
                    }
                }


                /* Now, axis is Homed, proceed to let user adjust axis manually. */
                iPerformed = TRUE;
                TTClearScreen( );
                if( ulAxisArg == RO_AXIS_R )
                    TTPrintsAt( 1, 1, "Move R to proper" );
                if( ulAxisArg == RO_AXIS_W )
                    TTPrintsAt( 1, 1, "Move W to proper" );
                TTPrintsAt( 2, 1, "INDEX position." );
                TTPrintsAt( 3, 1, "Press \"Enter\" when" );
                TTPrintsAt( 4, 1, "done or ESC" );

                /* Allow some flexibility in the positioning of the motors.
                 * i.e. soften the tuning. */
                ROGetParameter(TRUE, ulAxisArg, lParms, PROPORTIONAL_GAIN);
                if( ulAxisArg == RO_AXIS_R )
                    lParms[1] /= 2;
                if( ulAxisArg == RO_AXIS_W )
                    lParms[3] /= 2;
                ROSetParameter(FALSE, ulAxisArg, lParms, PROPORTIONAL_GAIN);

                ROGetParameter(TRUE, ulAxisArg, lParms, DERIVATIVE_GAIN);
                if( ulAxisArg == RO_AXIS_R )
                    lParms[1] /= 2;
                if( ulAxisArg == RO_AXIS_W )
                    lParms[3] /= 2;
                ROSetParameter(FALSE, ulAxisArg, lParms, DERIVATIVE_GAIN);
                break;

            case 'N':   /* User does NOT want to adjust home. */
                /* Y/N response from "Adjust R (or W) at HOME?" already processed. Awaiting Enter.*/
                if( iPerformed ) break;

                /* No error conditions, so exit happily. */
                iReturn = SUCCESS;
                goto exit;

            case 'Z':
            case ESC:   /* Abort homing operation. */
                iReturn = FAILURE;
                goto exit;

            case CR:    /* User is done with manual adjustments. */
                /* Homing NOT done. Manual adjustments can NOT be complete. */
                if( !iPerformed ) break;

                /* Otherwise exit happily. */
                iReturn = SUCCESS;
                goto exit;

            case 0:     /* 0 is normal return for Key-Not-Pressed mode. */
            case EOF:   /* EOF is normal return from SERGetcRxBuff if no data is available. */
                break;

            default:
                TTPrintsAt( 4, 1, "Bad Key           " );
                TTBeepTP( ALERT );
        }
    } while ( 1 );

exit:
    /* STop motion, turn off MOtors, restore original tuning, and exit. */
    GASendReceiveGalil(GA_CARD_0, "ST\r" , cpNull);
    TIDelay( 200 );
    GASendReceiveGalil(GA_CARD_0, "MO\r" , cpNull);

    ROGetParameter(TRUE, ulAxisArg, lParms, PROPORTIONAL_GAIN);
    ROSetParameter(FALSE, ulAxisArg, lParms, PROPORTIONAL_GAIN);

    ROGetParameter(TRUE, ulAxisArg, lParms, DERIVATIVE_GAIN);
    ROSetParameter(FALSE, ulAxisArg, lParms, DERIVATIVE_GAIN);
    return iReturn;
}



/****************************************************************
 *
 * Function:    ROSetHomeR
 *
 * Abstract:    Start by initializing the amplifiers, saving the
 *      current speed, acceleration, deceleration, and torque
 *      limit for the radial axis. Then check for an R adjustment
 *      at home or at index. Then home the R axis, while waiting
 *      for completion, check the limit switch, and move to the
 *      home offset. Check with the user if the offset is correct.
 *      If it isn't, let the user move the arm to the correct offset
 *      position, and save the new offset value to the datafile.
 *      In the case of the offset being correct already or a new offset
 *      being set, define the new position as the 0 position, restore
 *      old values, and exit. For a failure exit, just restore the values
 *      and exit; DO NOT define a new 0 position or save a new offset.
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: The home offset position that is stored in the datafile
 *      is in encoder counts. It is NOT scaled to normal units.
 *      This value is used directly to move the radial axis from
 *      the home switch to the actual home position. It compensates
 *      for the error between the belt/home switch location and
 *      the actual/correct home position.
 *
 ***************************************************************/
int ROSetHomeR(void)
{
    int  iReturn = SUCCESS; /* First, temporary variable. */
    int  iTint;             /* Second, temporary variable. */
    char cBuf[MAXGASTR], cButtonPressed;
    unsigned long ulAxis = RO_AXIS_R, ulTempAxis;
    /* The remaining variables are to set/get or save/restore Galil parameters. */
    long lHomeSpeed[8]={0,0,0,0,0,0,0,0};
    long lHomeAccel[8]={0,0,0,0,0,0,0,0};
    long lHomeOffset[8]={0,0,0,0,0,0,0,0};
    long lTorqueLimit[8] = {9999,9999,9999,9999,9999,9999,9999,9999};
    long lSaveSpeed[8]={0,0,0,0,0,0,0,0};
    long lSaveAccel[8]={0,0,0,0,0,0,0,0};
    long lSaveDecel[8]={0,0,0,0,0,0,0,0};
    long lSaveTorqueLimit[8] = {9999,9999,9999,9999,9999,9999,9999,9999};

    TTPrintsAt( 4, 1, "Init. AMP. board" );
    GAAmpInit();
    TIDelay( 1500 );

    /* Get internal power supply status from INPUT G port. */
    iTint = inb( IO_ROBOT_INPUT_G );

	iTint = 0;

    /* If any of the bits are set, it's a problem. */
    if( iTint )
    {
        TTPrintsAt( 1, 1, "Connection Error" );
        TTPrintsAt( 2, 1, "Check Connection" );
        return FAILURE;
    }

    /* If a dual arm exists, find out which arm the user wants to adjust. */
    if( ROGetSpecialAxis(RO_DUAL_ARM) )
    {
        TTClearScreen( );
        TTPrintsAt( 2, 1, "   Arm to Adjust?" );
        if (ROGetSpecialAxis(RO_DUAL_ARM))
            TTPrintsAt( 4, 1, "R-Radial  W-Dual Arm" );
        do
        {
            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
        } while( cButtonPressed != 'R' && cButtonPressed != 'W' && cButtonPressed != 'Z' && cButtonPressed != ESC );

        if( cButtonPressed == 'W' )
            ulAxis = RO_AXIS_W;
    }

    /* Key ESC aborts the homing operation. */
    if( (cButtonPressed == 'Z') || (cButtonPressed == ESC) )
    {
        return SUCCESS;
    }

    /* Get operating conditions: speed, acceleration, deceleration, and torque limit.
     * Save these parameters, and set conditions for HOMEing operation. */

    // For VAC514 set the pars  for Theta as well
    ulTempAxis = ulAxis;

    ROGetParameter(FALSE, ulTempAxis, lSaveTorqueLimit, TORQUE_LIMIT);
    ROSetParameter(FALSE, ulTempAxis, lTorqueLimit, TORQUE_LIMIT);

    ROGetParameter(FALSE, ulTempAxis, lSaveSpeed, OPERATIONAL_SPEED);
    lHomeSpeed[0] = lHomeSpeed[1] = lHomeSpeed[2] = lHomeSpeed[3] = 0;
    ROGetParameter(TRUE, ulTempAxis, lHomeSpeed, HOME_SPEED);
    ROSetParameter(FALSE, ulTempAxis, lHomeSpeed, OPERATIONAL_SPEED);

    ROGetParameter(FALSE, ulTempAxis, lSaveAccel, OPERATIONAL_ACCEL);
    lHomeAccel[0] = lHomeAccel[1] = lHomeAccel[2] = lHomeAccel[3] = 0;
    ROGetParameter(TRUE, ulTempAxis, lHomeAccel, HOME_ACCEL);
    ROSetParameter(FALSE, ulTempAxis, lHomeAccel, OPERATIONAL_ACCEL);

    ROGetParameter(FALSE, ulTempAxis, lSaveDecel, OPERATIONAL_DECEL);
    lHomeAccel[0] = lHomeAccel[1] = lHomeAccel[2] = lHomeAccel[3] = 0;
    ROGetParameter(TRUE, ulTempAxis, lHomeAccel, HOME_ACCEL);
    ROSetParameter(FALSE, ulTempAxis, lHomeAccel, OPERATIONAL_DECEL);

    /* Must have servo ON. */
    if( ulAxis == RO_AXIS_R )
    {
        GASendReceiveGalil(GA_CARD_0, "SHY\r" , cpNull);
//        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
    }
    if( ulAxis == RO_AXIS_W )
    {
        GASendReceiveGalil(GA_CARD_0, "SHW\r" , cpNull);
//        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
    }
    ulERFlag &= ~ulAxis;
    TIDelay( 500 );

    /* 1st sub step: Adjust R or W at HOME. Function above. */
    if( ROAdjustRAtHome( ulAxis ) == FAILURE )
    {
        goto end_R_home;
    }

    /* 2nd sub step: Adjust R or W at INDEX. Function above. */
    if( ROAdjustRAtIndex( ulAxis ) == FAILURE )
    {
        goto end_R_home;
    }

    /* Home and Index are all adjusted. Home axis to proceed.
     * Must have servo ON. It might have turned off in one of the sub-steps.
     * Home axis to proceed to next sub step: Set Home Offset.
     * It's done by Galil's HM command (HoMe). */
    if( ulAxis == RO_AXIS_R )
    {
        TTPrintsAt( 4, 1, "\n\rHoming R-axis..." );
        GASendReceiveGalil(GA_CARD_0, "SHY\r" , cpNull);
//        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
        TIDelay( 500 );
        GASendReceiveGalil(GA_CARD_0, "HMY\r" , cpNull);
        GASendReceiveGalil(GA_CARD_0, "BGY\r" , cpNull);
    }
    if( ulAxis == RO_AXIS_W )
    {
        TTPrintsAt( 4, 1, "\n\rHoming W-axis..." );
        GASendReceiveGalil(GA_CARD_0, "SHW\r" , cpNull);
//        GASendReceiveGalil(GA_CARD_0, "EI 255,255\r" , cpNull);
        TIDelay( 500 );
        GASendReceiveGalil(GA_CARD_0, "HMW\r" , cpNull);
        GASendReceiveGalil(GA_CARD_0, "BGW\r" , cpNull);
    }
    ulERFlag &= ~ulAxis;
    ulAMFlag &= ~ulAxis;

    /* Wait until motion complete or aborted by user. */
    while( (ulAMFlag & ulAxis) != ulAxis )
    {
	ROUpdateTS(FALSE);

        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);

        /* Key ESC aborts the homing operation. */
        if( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
        {
            TTPrintsAt( 4, 1, "\n\rHome Aborted   " );
            goto end_R_home;
        }

        /* Galil error also aborts the homing operation. */
        if( ulERFlag & ulAxis )
        {
            TTPrintsAt( 4, 1, "\n\rError\n\r" );
            TTBeepTP( ALERT );
            iReturn = FAILURE;
            goto end_R_home;
        }
    }

    /* Get the status of the limit switch of the axis. */
    if( ulAxis == RO_AXIS_R )
        GASendReceiveGalil(GA_CARD_0, "TSY\r"  , cBuf);
    if( ulAxis == RO_AXIS_W )
        GASendReceiveGalil(GA_CARD_0, "TSW\r"  , cBuf);

    /* Check if Negative limit switch is triggered.
     * (No need to check positive limit in homing test.)
     * If it's triggered, display the message, and skip the rest of testing. */
    if( !(atoi( cBuf ) & NEGLIM) )
    {
        TTPrintsAt( 4, 1, "Stuck at LM       \n\r" );
        iReturn = FAILURE;
        goto end_R_home;
    }
    TIDelay( 1000 );

    /* Home Offset testing. Get R or W-axis home offset parameter
     * and move axis to the negative offset position. */
    TTPrintsAt( 4, 1, "\n\rOffsetting R-axis.." );
    ROGetParameter(TRUE, ulAxis, lHomeOffset, HOME_OFFSET);
    if( ulAxis == RO_AXIS_R )
    {
        sprintf( cBuf, "PA,%ld\r", -lHomeOffset[1] );
        GASendReceiveGalil(GA_CARD_0, cBuf , cpNull);
        sprintf( cBuf, "BGY\r");
        GASendReceiveGalil(GA_CARD_0, cBuf , cpNull);
    }
    if( ulAxis == RO_AXIS_W )
    {
        sprintf( cBuf, "PA,,,%ld\r", -lHomeOffset[3] );
        GASendReceiveGalil(GA_CARD_0, cBuf , cpNull);
        sprintf( cBuf, "BGW\r" );
        GASendReceiveGalil(GA_CARD_0, cBuf , cpNull);
    }
    ulAMFlag &= ~ulAxis;

    /* Wait until motion complete or aborted by user. */
    while( (ulAMFlag & ulAxis) != ulAxis )
    {
	ROUpdateTS(FALSE);

        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);

        /* Key ESC aborts the homing operation. */
        if( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
        {
            TTPrintsAt( 4, 1, "\n\rOffsetting Aborted" );
            goto end_R_home;
        }

        /* Galil error also aborts the homing operation. */
        if( ulERFlag & ulAxis )
        {
            TTPrintsAt( 4, 1, "\n\rError\n\r" );
            TTBeepTP( ALERT );
            iReturn = FAILURE;
            goto end_R_home;
        }
    }

    /* Ask user if axis offset movement is correct. */
    TTClearScreen( );
    TTPrintsAt( 2, 1, " Is Offset correct?" );
    TTPrintsAt( 4, 1, " Y-Yes         N-No" );

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
        if( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
        {
            TTPrintsAt( 4, 1, "\n\rOffsetting Aborted" );
            goto end_R_home;
        }
    } while ( (cButtonPressed != 'Y') && (cButtonPressed != 'N') );

    /* If user pressed Yes, this test is done. Display the appropriate
     * message and return. */
    if ( cButtonPressed == 'Y' )
    {
        goto end_R_homeA;
    }

    /* If offset was not correct, allow user to move axis manually,
     * and to set the offset manually. */
    TTClearScreen( );
    TTPrintsAt( 4, 1, "Switching Off Servo" );
    if( ulAxis == RO_AXIS_R )
    {
        // VAC514 robots both servos needs to be on/off for either motion, of T or R
        if(iDefineFlag & DFVAC514)
            GASendReceiveGalil(GA_CARD_0, "MOXY\r" , cpNull);
        else
            GASendReceiveGalil(GA_CARD_0, "MOY\r" , cpNull);

        TIDelay( 2000 );
        TTPrintsAt( 4, 1, "\n\rMove R-axis to 0" );
    }
    if( ulAxis == RO_AXIS_W )
    {
        GASendReceiveGalil(GA_CARD_0, "MOW\r" , cpNull);
        TIDelay( 2000 );
        TTPrintsAt( 4, 1, "\n\rMove W-axis to 0" );
    }
    TTPrintsAt( 4, 1, "\n\rand HERE or ESC" );

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
    } while( (cButtonPressed != CR) && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* If user pressed ESC or Z, don't save the offset. */
    if( cButtonPressed == ESC || cButtonPressed == 'Z' )
    {
        goto end_R_home;
    }

    /* User wants to save this offset as axis Home offset.
     * Get the current R or W position, and save it in the parameter structure. */
    if( ulAxis == RO_AXIS_R )
    {
        GASendReceiveGalil(GA_CARD_0, "TPY\r", cBuf);
        FIOGetParamVals(ROBOTFILE, HOME_OFFSET, lHomeOffset);
        lHomeOffset[1] = (-1)*(atol( cBuf ));
        FIOSetParamVals(ROBOTFILE, HOME_OFFSET, lHomeOffset);
    }
    if( ulAxis == RO_AXIS_W )
    {
        GASendReceiveGalil(GA_CARD_0, "TPW\r", cBuf);
        FIOGetParamVals(AUXFILE, HOME_OFFSET, lHomeOffset);
        lHomeOffset[0] = (-1)*(atol( cBuf ));
        FIOSetParamVals(AUXFILE, HOME_OFFSET, lHomeOffset);
    }

    /* Ask user if they want to save the parameter file. */
    TTClearScreen( );
    TTPrintsAt( 2, 1, "Save parameter file?" );
    TTPrintsAt( 4, 1, " Y-Yes         N-No" );
    iTint = FALSE;

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
        if( (cButtonPressed == ESC) || (cButtonPressed == 'Z') || (cButtonPressed == 'N') )
            goto end_R_homeA;
    } while ( cButtonPressed != 'Y' );

    /* If user pressed Yes, this test is done. */
    iTint = TRUE;

/* This label is for ending the test after Home was set correctly. */
end_R_homeA:
    if( ulAxis == RO_AXIS_R )
        GASendReceiveGalil(GA_CARD_0, "DP,0\r", cpNull);
    if( ulAxis == RO_AXIS_W )
        GASendReceiveGalil(GA_CARD_0, "DP,,,0\r", cpNull);
    TTPrintsAt( 4, 1, "\n\rHome set\n\r" );
    TIDelay( 200 );
    iReturn = SUCCESS;

/* This label is for ending the test before Home was set correctly.
 * Restore all the operating condition parameters back in the beginning of this test. */
end_R_home:
    GASendReceiveGalil(GA_CARD_0, "ST\r", cpNull);
    TIDelay( 200 );
    GASendReceiveGalil(GA_CARD_0, "MO\r", cpNull);
    ROSetParameter(FALSE, ulAxis, lSaveTorqueLimit, TORQUE_LIMIT);
    ROSetParameter(FALSE, ulAxis, lSaveSpeed, OPERATIONAL_SPEED);
    ROSetParameter(FALSE, ulAxis, lSaveAccel, OPERATIONAL_ACCEL);
    ROSetParameter(FALSE, ulAxis, lSaveDecel, OPERATIONAL_DECEL);
    IOWriteIO( -1, 0x01, IO_ROBOT_OUTPUT_B);
    if (iTint && ulAxis == RO_AXIS_R)
        FIOWriteParam(ROBOTFILE);
    if (iTint && ulAxis == RO_AXIS_W)
        FIOWriteParam(AUXFILE);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROSetTestScaling
 *
 * Abstract:    This prints the axes to the teach pendant screen
 *      and then constantly polls the Galil for current position.
 *      The ROReadCurrentPosition function calls ROScalePos inside
 *      it. That function uses the arm length and the mechanical
 *      ratio to convert enc7oder counts to thousandths of an inch,
 *      the normal units that are printed out for the user. This
 *      diagnostics step checks that those numbers are correct.
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetTestScaling(int iCardNoArg, int iVacExistsArg)
{
    char cBuf[21], cButtonPressed;
    int  iAxisIndex;
    int  iTrackCardNum = 0;
    int  iDualArmCardNum = 0;
    int  iSinglePreCardNum = 0;
    int  iMaxAxis = 3;
    int  iVacAxisMask;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    long laAuxPosition[8]={0,0,0,0,0,0,0,0};
    unsigned long ulAuxAxis = 0;

	if (iCardNoArg == 1) goto preTest;

    /* Check to see if a dual arm exists in the system. */
    if (ROGetSpecialAxis(RO_DUAL_ARM))
    {
        iDualArmCardNum = GA_CARD_0;
    }
    /* Check to see if a track exists in the system. */
    else if (ROGetSpecialAxis(RO_TRACK))
    {
        /* Find out which card the track is on. */
        if ((ROGetSpecialAxis(RO_TRACK) <= RO_AXIS_W))
            iTrackCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_SINGLE_PRE))
    {
        iSinglePreCardNum = GA_CARD_0;
    }

    /* Decide if the additional axis should be displayed. */
    if (((iDualArmCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0) || (iSinglePreCardNum == GA_CARD_0)))
    {
        iMaxAxis = 4;
    }
    /* Or not displayed. */
    else
    {
        iDualArmCardNum = 0;
        iSinglePreCardNum = 0;
        iTrackCardNum = 0;
    }

    TTClearScreen();
    TTPrintsAt( 1, 1, "T:" );
    TTPrintsAt( 2, 1, "R:" );
    TTPrintsAt( 3, 1, "Z:" );
    if ((iTrackCardNum) || (iDualArmCardNum) || (iSinglePreCardNum))
        TTPrintsAt( 4, 1, "W:" );
    else
        TTPrintsAt(4, 1, "Move Axes");

    /* If an auxilliary axis is to be shown, get its axis designation. */
    if (iDualArmCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_DUAL_ARM);
    if (iSinglePreCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_SINGLE_PRE);
    if (iTrackCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_TRACK);

	// turn on brake 
	//    ROServiceBrake(ROBOTFILE, TRUE);
    ROServiceBrake(ROBOTFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    /* Display the current position for all axes: T, R, Z, W. */
    do
    {
        /* Get the current position for the robot... */
        ROReadCurrentPosition(ROAxisAllRobot(), laPosition);

        /* Get the current position for the auxilliary axis. */
        if (ulAuxAxis)
        {
            ROReadCurrentPosition(ulAuxAxis, laAuxPosition);
            laPosition[3] = laAuxPosition[3];
        }

        /* Display the positions to the screen. */
        for( iAxisIndex = 0; iAxisIndex < iMaxAxis; iAxisIndex++ )
        {
            sprintf( cBuf, "%ld", laPosition[iAxisIndex] );
            TTPrintsAt( iAxisIndex+1, 5, "            " );
            TTPrintsAt( iAxisIndex+1, 5, cBuf );
        }
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
    } while ( cButtonPressed != CR && cButtonPressed != ESC && cButtonPressed != 'Z' );

    ROServiceBrake(ROBOTFILE, FALSE);
    return SUCCESS;

preTest:

    /* Check to see if a track exists in the system. */
    if (ROGetSpecialAxis(RO_TRACK))
    {
        /* Find out which card the track is on. */
        if ((ROGetSpecialAxis(RO_TRACK) > RO_AXIS_W))
            iTrackCardNum = GA_CARD_1;
    }

    if (((iDualArmCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1) || (iSinglePreCardNum == GA_CARD_1)))
    {
        iMaxAxis = 4;
    }
    else if ((ROGetSpecialAxis(RO_INDEXER_T2)))
    {
        iMaxAxis = 4;
    }
    /* Or not displayed. */
    else
    {
        iDualArmCardNum = 0;
        iSinglePreCardNum = 0;
        iTrackCardNum = 0;
    }

    TTClearScreen();
    TTPrintsAt( 1, 1, "t:" );
    TTPrintsAt( 2, 1, "r:" );
    TTPrintsAt( 3, 1, "z:" );
    if ((iTrackCardNum) || (iDualArmCardNum) || (iSinglePreCardNum))
        TTPrintsAt( 4, 1, "w:" );
    else
        TTPrintsAt(4, 1, "Move Axes");

    /* If an auxilliary axis is to be shown, get its axis designation. */
    if (iDualArmCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_DUAL_ARM);
    if (iSinglePreCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_SINGLE_PRE);
    if (iTrackCardNum)
        ulAuxAxis = ROGetSpecialAxis(RO_TRACK);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    /* Display the current position for all axes: T, R, Z, W. */
    do
    {
        /* ...or prealigner, depending on which is being tested. */
        ROReadCurrentPosition(ROAxisAllPre(), laPosition);

        /* Get the current position for the auxilliary axis. */
        if (ulAuxAxis)
        {
            ROReadCurrentPosition(ulAuxAxis, laAuxPosition);
            laPosition[7] = laAuxPosition[7];
        }

        /* Display the positions to the screen. */
        for( iAxisIndex = 0; iAxisIndex < iMaxAxis; iAxisIndex++ )
        {
            sprintf( cBuf, "%ld", laPosition[iAxisIndex+4] );
            TTPrintsAt( iAxisIndex+1, 5, "            " );
            TTPrintsAt( iAxisIndex+1, 5, cBuf );
        }
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
    } while ( cButtonPressed != CR && cButtonPressed != ESC && cButtonPressed != 'Z' );

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    RODoROffset
 *
 * Abstract:    Find mid-point from neg. lim. sw. to pos. lim. sw.
 *      This is determined by moving to the pos. lim. sw. and recording
 *      the position; and then doing the same for the neg. lim. sw.
 *      The average value is the home offset. This is only used for the
 *      horizontal (r) axis of the popup 3-axis prealigner.
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int RODoROffset(void)
{
    int iCurrAct = 1;
    long lLMPosition[2] = {0,0};
    long lHomeOffset[8] = {0,0,0,0,0,0,0,0};
    char cBuf[MAXGASTR], cButtonPressed;

    ulERFlag &= ~RO_AXIS_r;

    do
    {
        /* Get the state of the limit switches. */
        GASendReceiveGalil(GA_CARD_1, "TSF\r", cBuf );

        /* Pos. Lim. Sw. check on second pass. */
        if( iCurrAct )
        {
            if( !(atoi( cBuf ) & POSLIM ) )
            {
                TTPrintsAt( 4, 1, "\n\rPos. Sw. Already Activated" );
                return FAILURE;
            }
        }
        /* Neg. Lim. Sw. check on first pass. */
        else
        {
            if( !(atoi( cBuf ) & NEGLIM ) )
            {
                TTPrintsAt( 4, 1, "\n\rNeg. Sw. Already Activated" );
                return FAILURE;
            }
        }

        /* Move to Pos. Lim. Sw. on second pass. */
        if( iCurrAct )
        {
            TTPrintsAt( 4, 1, "\n\rMoving to Pos. Sw." );
            GASendReceiveGalil( GA_CARD_1, "PA,,,,,500000\r", cpNull );
            GASendReceiveGalil( GA_CARD_1, "BGF\r", cpNull );
        }
        /* Move to Neg. Lim. Sw. on first pass. */
        else
        {
            TTPrintsAt( 4, 1, "\n\rMoving to Neg. Sw." );
            GASendReceiveGalil( GA_CARD_1, "PA,,,,,-500000\r", cpNull );
            GASendReceiveGalil( GA_CARD_1, "BGF\r", cpNull );
        }
        ulAMFlag &= ~RO_AXIS_r;

        /* Wait until R motion complete or aborted by user. */
        while( (ulAMFlag & RO_AXIS_r) != RO_AXIS_r )
        {
	    ROUpdateTS(FALSE);
            SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);

            /* Key ESC aborts the homing operation. */
            if ( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
            {
                TTPrintsAt( 4, 1, "\n\rHome Aborted   " );
                return SUCCESS;
            }

            /* Galil error also aborts the homing operation. */
            if( ulERFlag & RO_AXIS_r )
            {
                TTPrintsAt( 4, 1, "\n\rError" );
                TTBeepTP( ALERT );
                return FAILURE;
            }
        }

        /* Get the end position and store it. */
        GASendReceiveGalil( GA_CARD_1, "TPF\r", cBuf );
        lLMPosition[iCurrAct--] = atol( cBuf );
    } while ( iCurrAct >= 0 );

    /* Save the home offset as the point between the 2 limit switches. */
    /* It's 1/16th close to the negative limit switch */
    FIOGetParamVals(PREALIGNFILE, HOME_OFFSET, lHomeOffset);
    lHomeOffset[5] = -(lLMPosition[1] - lLMPosition[0]) / 16;
    FIOSetParamVals(PREALIGNFILE, HOME_OFFSET, lHomeOffset);

    /* Move from the midpoint to the pos. lim. sw. to test the new home offset value. */
    TTPrintsAt( 4, 1, "\n\rOffsetting r..." );
    sprintf( cBuf, "PA,,,,,%ld\r", (-1)*lHomeOffset[5] );
    ulAMFlag &= ~RO_AXIS_r;
    GASendReceiveGalil( GA_CARD_1, cBuf, cpNull );
    GASendReceiveGalil( GA_CARD_1, "BGF\r", cpNull);

    /* Wait until R motion complete or aborted by user. */
    while( (ulAMFlag & RO_AXIS_r) != RO_AXIS_r )
    {
	ROUpdateTS(FALSE);
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);

        /* Key ESC aborts the homing operation. */
        if ( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
        {
            return SUCCESS;
        }

        /* Galil error also aborts the homing operation. */
        if( ulERFlag & RO_AXIS_r )
        {
            TTPrintsAt( 4, 1, "\n\rError" );
            TTBeepTP( ALERT );
            return FAILURE;
        }
    }
    
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    MEPre(aligner)setHomeR
 *
 * Abstract:    Same as ROSetHomeR. The only difference is that
 *      this routine is intended for the 2 popup axes of a 3-axis
 *      prealigner. And the r axis homing, although implemented
 *      here, is actually diverted to the RODoROffset function above.
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROPresetHomeR(void)
{
    int iReturn = SUCCESS;  /* First, temporary variable. */
    int iTint;              /* Second, temporary variable. */
    unsigned long ulMaskIndex;
    char cBuf[MAXGASTR], cEquipeAxis, cGalilAxis, cButtonPressed;
    char cRespBuf[MAXGASTR];
    /* The remaining variables are to set/get or save/restore Galil parameters. */
    long lHomeSpeed[8]={0,0,0,0,0,0,0,0};
    long lHomeAccel[8]={0,0,0,0,0,0,0,0};
    long lHomeOffset[8]={0,0,0,0,0,0,0,0};
    long lTorqueLimit[8] = {9999,9999,9999,9999,9999,9999,9999,9999};
    long lSaveSpeed[8]={0,0,0,0,0,0,0,0};
    long lSaveAccel[8]={0,0,0,0,0,0,0,0};
    long lSaveDecel[8]={0,0,0,0,0,0,0,0};
    long lSaveTorqueLimit[8] = {9999,9999,9999,9999,9999,9999,9999,9999};

    TTPrintsAt( 4, 1, "Init. AMP. board" );
    IOWriteIO( -1, 0x1D, IO_PRE_INPUT_G );
    TIDelay( 1500 );

    /* Get internal power supply status from INPUT G port. */
    iTint = inb( IO_PRE_INPUT_G ) & 0xFC;

	iTint = 0;

    /* If any of the bits are set, it's a problem. */
    if( iTint )
    {
        TTPrintsAt( 1, 1, "Connection Error" );
        TTPrintsAt( 2, 1, "Check Connection" );
        return FAILURE;
    }

    /* Allow the user to select which axis to home: R or Z, acutally, r or z. */
    TTClearScreen( );
    TTPrintsAt( 1, 1, "Press R Toggle" );
    TTPrintsAt( 2, 1, "      OR      " );
    TTPrintsAt( 3, 1, "Press Z Toggle" );

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
    } while( (cButtonPressed != 'I') && (cButtonPressed != 'J') && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* Key ESC aborts the homing operation. */
    if( (cButtonPressed == 'Z') || (cButtonPressed == ESC) )
    {
        return SUCCESS;
    }

    /* Based on user's response, initialize varibales. */
    cGalilAxis = (cButtonPressed == 'I') ? 'F' : 'G';
    cEquipeAxis = (cButtonPressed == 'I') ? 'r' : 'z';
    ulMaskIndex = (cButtonPressed == 'I') ? RO_AXIS_r : RO_AXIS_z;

    /* Get operating conditions: speed, acceleration, deceleration, and torque limit.
     * Save these parameters, and set conditions for HOMEing operation. */
    ROGetParameter(FALSE, ulAxisallPre, lSaveTorqueLimit, TORQUE_LIMIT);
    ROSetParameter(FALSE, ulAxisallPre, lTorqueLimit, TORQUE_LIMIT);

    ROGetParameter(FALSE, ulAxisallPre, lSaveSpeed, OPERATIONAL_SPEED);
    lHomeSpeed[4] = lHomeSpeed[5] = lHomeSpeed[6] = lHomeSpeed[7] = 0;
    ROGetParameter(TRUE, ulAxisallPre, lHomeSpeed, HOME_SPEED);
    ROSetParameter(FALSE, ulAxisallPre, lHomeSpeed, OPERATIONAL_SPEED);

    ROGetParameter(FALSE, ulAxisallPre, lSaveAccel, OPERATIONAL_ACCEL);
    lHomeAccel[4] = lHomeAccel[5] = lHomeAccel[6] = lHomeAccel[7] = 0;
    ROGetParameter(TRUE, ulAxisallPre, lHomeAccel, HOME_ACCEL);
    ROSetParameter(FALSE, ulAxisallPre, lHomeAccel, OPERATIONAL_ACCEL);

    ROGetParameter(FALSE, ulAxisallPre, lSaveDecel, OPERATIONAL_DECEL);
    lHomeAccel[4] = lHomeAccel[5] = lHomeAccel[6] = lHomeAccel[7] = 0;
    ROGetParameter(TRUE, ulAxisallPre, lHomeAccel, HOME_ACCEL);
    ROSetParameter(FALSE, RO_AXIS_R, lHomeAccel, OPERATIONAL_DECEL);

    /* Must have servo ON. */
    sprintf( cBuf, "SH%c\r", cGalilAxis );
    GASendReceiveGalil( GA_CARD_1, cBuf, cpNull );
    ulERFlag &= ~ulMaskIndex;
    TIDelay( 500 );

    /* The r axis homing, although implemented here,
     * is actually diverted to the RODoROffset function above. */
    if( cButtonPressed == 'I' )
    {
        iReturn = RODoROffset( );
        goto end_R_homeA;
    }

    /* Home z-axis to proceed to next step: Set Home Offset. */
    sprintf( cBuf, "\n\rHoming %c-axis...", cEquipeAxis );
    TTPrintsAt( 4, 1, cBuf );

    /* It's done by Galil's HM command (HoMe). */
    sprintf( cBuf, "HM%c\r", cGalilAxis );
    GASendReceiveGalil( GA_CARD_1, cBuf, cpNull );
    sprintf( cBuf, "BG%c\r", cGalilAxis );
    // Set the flag before we send it to Galil in case we are already in
    // that position = interrupt comes right away
    // even though with Home we have some time, keep it here for good practice
    ulAMFlag &= ~ulMaskIndex;
    GASendReceiveGalil( GA_CARD_1, cBuf, cpNull );

    /* Wait until R or Z motion complete or aborted by user. */
    while( (ulAMFlag & ulMaskIndex) != ulMaskIndex )
    {
	ROUpdateTS(FALSE);
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );

        /* Key ESC aborts the homing operation. */
        if ( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
        {
            TTPrintsAt( 4, 1, "\n\rHome Aborted   " );
//            iReturn = FAILURE;
            goto end_R_home;
        }

        /* Galil error also aborts the homing operation. */
        if( ulERFlag & ulMaskIndex )
        {
            TTPrintsAt( 4, 1, "\n\rError" );
            TTBeepTP( ALERT );
            iReturn = FAILURE;
            goto end_R_home;
        }
    }

    /* Get the status of the limit switches. */
    sprintf( cBuf, "TS%c\r", cGalilAxis );
    GASendReceiveGalil(GA_CARD_1, cBuf, cRespBuf);

    /* Check if Negative limit switch is triggered.
     * (No need to check positive limit in homing test.)
     * If it's triggered, display the message, and skip the rest of testing. */
    if( !(atoi( cRespBuf ) & NEGLIM) )
    {
        TTPrintsAt( 4, 1, "z stuck at LM   \n\r" );
        iReturn = FAILURE;
        goto end_R_home;
    }
    TIDelay( 1000 );

    /* z axis Home Offset testing. Get home offset parameter
     * and move z axis to the negative offset position. */
    sprintf( cBuf, "\n\rOffsetting %c-axis..", cEquipeAxis );
    TTPrintsAt( 4, 1, cBuf );
    ROGetParameter(TRUE, ulAxisallPre, lHomeOffset, HOME_OFFSET);
    sprintf( cBuf, "PA,%ld,%ld\r", -lHomeOffset[5], -lHomeOffset[6]);
    GASendReceiveGalil(GA_CARD_1, cBuf, cpNull );
    sprintf( cBuf, "BG%c\r", cGalilAxis );
    // Set the flag before we send it to Galil in case we are already in
    // that position = interrupt comes right away
    ulAMFlag &= ~ulMaskIndex;
    GASendReceiveGalil(GA_CARD_1, cBuf, cpNull );

    /* Wait until R or Z motion complete or aborted by user. */
    while( (ulAMFlag & ulMaskIndex) != ulMaskIndex )
    {
	ROUpdateTS(FALSE);
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );

        /* Key ESC aborts the homing operation. */
        if ( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
        {
            TTPrintsAt( 4, 1, "\n\rOffsetting Aborted" );
//            iReturn = FAILURE;
            goto end_R_home;
        }

        /* Galil error also aborts the homing operation. */
        if( ulERFlag & ulMaskIndex )
        {
            TTPrintsAt( 4, 1, "\n\rError" );
            TTBeepTP( ALERT );
            iReturn = FAILURE;
            goto end_R_home;
        }
    }

    /* Ask user if R or Z axis offset movement is correct. */
    TTClearScreen( );
    TTPrintsAt( 2, 1, " Is Offset correct?" );
    TTPrintsAt( 4, 1, " Y-Yes         N-No" );

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
    } while ( (cButtonPressed != 'Y') && (cButtonPressed != 'N') && (cButtonPressed != 'Z') && (cButtonPressed != ESC) );

    /* Key ESC aborts the homing operation. */
    if( (cButtonPressed == 'Z') || (cButtonPressed == ESC) )
    {
//        iReturn = FAILURE;
        goto end_R_home;
    }

    /* If user pressed Yes, this test is done. Display the appropriate
     * message and return. */
    if( cButtonPressed == 'Y' )
    {
        goto end_R_homeA;
    }

    /* If offset was not correct, allow user to move R or Z axis manually,
     * and to set the offset manually. */
    TTClearScreen( );
    TTPrintsAt( 4, 1, "Switching Off Servo" );
    sprintf( cBuf, "MO%c\r", cGalilAxis );
    GASendReceiveGalil( GA_CARD_1, cBuf, cpNull );
    TIDelay( 250 );

    sprintf( cBuf, "\n\rMove %c-axis to 0", cEquipeAxis );
    TTPrintsAt( 4, 1, cBuf );
    TTPrintsAt( 4, 1, "\n\rand HERE or ESC" );

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
    } while ( (cButtonPressed != CR) && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* If user pressed ESC or Z, don't save the offset. */
    if( cButtonPressed == ESC || cButtonPressed == 'Z' )
    {
//        iReturn = FAILURE;
        goto end_R_home;
    }

    /* User wants to save this offset as R or Z axis Home offset.
     * Get the current R or Z position, and save it in the parameter structure. */
    sprintf( cBuf, "TP%c\r", cGalilAxis );
    GASendReceiveGalil(GA_CARD_1, cBuf, cRespBuf);
    FIOGetParamVals(PREALIGNFILE, HOME_OFFSET, lHomeOffset);
    lHomeOffset[(int)(cGalilAxis-'E')+4] = (-1)*(atol( cRespBuf ));
    FIOSetParamVals(PREALIGNFILE, HOME_OFFSET, lHomeOffset);

    /* Ask user if they want to save the parameter file. */
    TTClearScreen( );
    TTPrintsAt( 2, 1, "Save parameter file?" );
    TTPrintsAt( 4, 1, " Y-Yes         N-No" );
    iTint = FALSE;

    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
        if( (cButtonPressed == ESC) || (cButtonPressed == 'Z') || (cButtonPressed == 'N') )
            goto end_R_homeA;
    } while ( cButtonPressed != 'Y' );

    /* If user pressed Yes, this test is done. */
    iTint = TRUE;

/* This label is for ending the test after Home was set correctly. */
end_R_homeA:
    if( cGalilAxis == 'F' )
        GASendReceiveGalil( GA_CARD_1, (char *)"DP,,,,,0\r", cpNull );
    else
        GASendReceiveGalil( GA_CARD_1, (char *)"DP,,,,,,0\r", cpNull );
    TTPrintsAt( 4, 1, "\n\rHome set\n\r" );
    TIDelay( 200 );
    iReturn = SUCCESS;

/* This label is for ending the test before Home was set correctly.
 * Restore all the operating condition parameters back in the beginning of this test. */
end_R_home:
    GASendReceiveGalil( GA_CARD_1, "STEFGH\r", cpNull );
    TIDelay( 200 );
    GASendReceiveGalil( GA_CARD_1, "MOEFGH\r", cpNull );
    ROSetParameter(FALSE, ulAxisallPre, lSaveTorqueLimit, TORQUE_LIMIT);
    ROSetParameter(FALSE, RO_AXIS_r, lSaveSpeed, OPERATIONAL_SPEED);
    ROSetParameter(FALSE, RO_AXIS_r, lSaveAccel, OPERATIONAL_ACCEL);
    ROSetParameter(FALSE, RO_AXIS_r, lSaveDecel, OPERATIONAL_DECEL);
    IOWriteIO( -1, 0, IO_PRE_OUTPUT_B );
    if (iTint)
        FIOWriteParam(PREALIGNFILE);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROTestVacuum
 *
 * Abstract:    This starts by printing out the current conditions
 *      of the vaccuum valve(s) and sensor(s). It then waits for
 *      user key presses to change the vaccuum valve(s) state.
 *      It constantly updates the state(s) of the vaccuum sensor(s).
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROTestVacuum(int iPreArg)
{
    unsigned char iValue;
    int iOutPortID;
//	int iInputNum, iOutputNum;
    char cKey;

    TTPrintsAt( 1, 1, "           #1   #2" );
    TTPrintsAt( 2, 1, "Vac Sensor" );
    TTPrintsAt( 3, 1, "Vac Valve" );
    TTPrintsAt( 4, 1, "Press '1' or '2'" );

    /* Get the right IO port addresses depending on the card number. */
    iOutPortID = iPreArg ? IO_PRE_OUTPUT_L : IO_ROBOT_OUTPUT_A;

    iValue = inb(iOutPortID);
    if(iPreArg)
	iValue = ~iValue;
    if(iValue & 2)
	TTPrintsAt( 3, 17, "OFF");
    else
	TTPrintsAt( 3, 17, "ON ");

    if(iValue & 1)
	TTPrintsAt( 3, 12, "OFF");
    else
	TTPrintsAt( 3, 12, "ON ");

    /* Allow user to turn ON/OFF Vac Valve, and
     * display the Vac Sensor status. If Vac Valve is ON, Vac Sensor should
     * be set ON if an object is placed, or OFF if no object detected. */
    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cKey, FALSE);
        switch (cKey)
        {
            case '1':
		iValue = inb(iOutPortID);
		// just toggle bit 0
		if(iValue & 1)
		    iValue = iValue & 0xFE;   	// off bit 0
		else
		    iValue = iValue | 1;  	// on bit 0
		IOWriteIO(-1, iValue, iOutPortID);
	    	if(iPreArg)
		    iValue = ~iValue;
	    	if(iValue & 2)
		    TTPrintsAt( 3, 17, "OFF");
	    	else
		    TTPrintsAt( 3, 17, "ON ");

		if(iValue & 1)
		    TTPrintsAt( 3, 12, "OFF");
		else
		    TTPrintsAt( 3, 12, "ON ");
		break;
	    case '2':
		iValue = inb(iOutPortID);
		if(iValue & 2)
		    iValue = iValue & 0xFD;	// off bit 1
		else
		    iValue = iValue | 2;	// on bit 1
		IOWriteIO(-1, iValue, iOutPortID);
	    	if(iPreArg)
		    iValue = ~iValue;
	    	if(iValue & 2)
		    TTPrintsAt( 3, 17, "OFF");
	    	else
		    TTPrintsAt( 3, 17, "ON ");
	
		if(iValue & 1)
		    TTPrintsAt( 3, 12, "OFF");
		else
		    TTPrintsAt( 3, 12, "ON ");
		break;
	}

        /* check the vacuum sensor, and display the status change. */
    	if (iPreArg)
		iValue = inb(IO_PRE_INPUT_K);
	else
		iValue = inb(IO_ROBOT_INPUT_F);

	if(iValue & 2)
	    TTPrintsAt(2, 17, "OFF");
	else
	    TTPrintsAt(2, 17, "ON ");
	if(iValue & 1)
	    TTPrintsAt(2, 12, "OFF");
	else
	    TTPrintsAt(2, 12, "ON ");


    } while ( (cKey != CR) && (cKey != ESC) && (cKey != 'Z') );

    return SUCCESS;

}

/****************************************************************
 *
 * Function:    ROCorrectDistance
 *
 * Abstract:    Used by test_HOME if a home error is identified
 *      by the user. It allows the user to remove the belt,
 *      moves the motor to a good position, and then prompts
 *      the user to replace the belt. Then the distance from
 *      the home switch to the first encoder index pulse will
 *      be in an acceptable range.
 *
 * Parameters:
 *      iCardNoArg      (in) The card number being operated on
 *      lHomeIndexArg   (in) Current axis position
 *      iAxisIndexArg   (in) RO_AXIS_T, R, Z, W, t, r, z, or w
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROCorrectDistance(int iCardNoArg, long lHomeIndexArg, unsigned uAxisIndexArg)
{
    int iAxis;
    char sTestStr[25];
    char cButtonPressed;
    char cAxisName[2];
    long lParms[8];

    /* Display the user interface message. */
    TTClearScreen( );

	if (iCardNoArg > 1) goto preTest;

    cAxisName[1] = 0;
    if (uAxisIndexArg == RO_AXIS_T)
        cAxisName[0] = 'T';
    else if (uAxisIndexArg == RO_AXIS_R)
        cAxisName[0] = 'R';
    else if (uAxisIndexArg == RO_AXIS_Z)
        cAxisName[0] = 'Z';
    else if (uAxisIndexArg == RO_AXIS_W)
        cAxisName[0] = 'W';

    sprintf(sTestStr, "Move %s Axis         ", cAxisName);
    TTPrintsAt( 1, 1, sTestStr);
    TTPrintsAt( 2, 1, "to GOOD position    " );
    TTPrintsAt( 3, 1, "Press \"Enter\" when  " );
    TTPrintsAt( 4, 1, "done or ESC         " );

    /* Allow user to move axes to Good position until CR or ESC or Z is pressed. */
    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
    } while ( (cButtonPressed != CR) && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* If not CR, abort this procedure of making a belt correction. */
    if( cButtonPressed != CR )
        goto cd_exit;

    /* Proceed to Lock the axis so that user can release the belt.
     * Just turn on the servo. */
    if (uAxisIndexArg == RO_AXIS_T)
            strcpy(sTestStr, "SHX\r");
    else if (uAxisIndexArg == RO_AXIS_R)
        strcpy(sTestStr, "SHY\r");
    else if (uAxisIndexArg == RO_AXIS_Z)
        strcpy(sTestStr, "SHZ\r");
    else if (uAxisIndexArg == RO_AXIS_W)
        strcpy(sTestStr, "SHW\r");
    GASendReceiveGalil( iCardNoArg, sTestStr, cpNull);

    ROServiceBrake(ROBOTFILE, TRUE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    ulERFlag &= (unsigned long)(~uAxisIndexArg);
    TIDelay( 500 );

    /* Let user know OK to release the belt. */
    TTClearScreen( );
    sprintf(sTestStr, "Lock %s Axis         ", cAxisName);
    TTPrintsAt( 1, 1, sTestStr );
    TTPrintsAt( 2, 1, "Release The Belt    " );
    TTPrintsAt( 3, 1, "Press \"Enter\" when  " );
    TTPrintsAt( 4, 1, "done or ESC         " );

    /* Temporarily set KP and KD. i.e. soften the tuning. */
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);
    for (iAxis=0; iAxis<3; iAxis++) lParms[iAxis] /= 2;
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);

    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);
    for (iAxis=0; iAxis<3; iAxis++) lParms[iAxis] /= 2;
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);

    /* Wait for user replace the belt. */
    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
    } while( (cButtonPressed != ESC) && (cButtonPressed != CR) && (cButtonPressed != 'Z') );

    /* If aborted by user, abort this procedure. */
    if( cButtonPressed != CR )
        goto cd_exit;

    /* User replaced the belt; make correction by relative move of home index. */
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, ENCODER_RESOLUTION);
    TTPrintsAt( 4, 1, "\n\rMaking correction..." );

    if( uAxisIndexArg == RO_AXIS_T)
        sprintf( sTestStr, "PR%ld\r", lHomeIndexArg - 2 * lParms[0] );
    else if( uAxisIndexArg == RO_AXIS_R)
        sprintf( sTestStr, "PR,%ld\r", lHomeIndexArg - 2 * lParms[1] );
    else if( uAxisIndexArg == RO_AXIS_Z)
        sprintf( sTestStr, "PR,,%ld\r", lHomeIndexArg - 2 * lParms[2] );
    else if( uAxisIndexArg == RO_AXIS_W)
        sprintf( sTestStr, "PR,,,%ld\r", lHomeIndexArg - 2 * lParms[3] );
    GASendReceiveGalil(iCardNoArg, sTestStr, cpNull );

    if( uAxisIndexArg == RO_AXIS_T)
            sprintf( sTestStr, "BGX\r" );
    else if( uAxisIndexArg == RO_AXIS_R)
            sprintf( sTestStr, "BGY\r" );
    else if( uAxisIndexArg == RO_AXIS_Z)
        sprintf( sTestStr, "BGZ\r" );
    else if( uAxisIndexArg == RO_AXIS_W)
        sprintf( sTestStr, "BGW\r");
    GASendReceiveGalil(iCardNoArg, sTestStr, cpNull );
    ulAMFlag &= (unsigned long)(~uAxisIndexArg);

    /* Wait until motion complete, or user aborts. */
    while( (ulAMFlag & (unsigned long)(uAxisIndexArg)) != (unsigned long)(uAxisIndexArg))
    {
	ROUpdateTS(FALSE);
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
        if ( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
            goto cd_exit;
    }

    /* Correction finished...
     * Allow user to replace the belt. */
    TTBeepTP( LONGBELL );
    TTClearScreen( );
    TTPrintsAt( 1, 1, "Correction Done     " );
    TTPrintsAt( 2, 1, "Replace The Belt    " );
    TTPrintsAt( 3, 1, "Press \"Enter\" when  " );
    TTPrintsAt( 4, 1, "done                " );

    /* Restore the original tuning parameters. */
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);
    TIDelay( 1000 );

    /* Wait until user replaces the belt. */
    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
    } while( cButtonPressed != CR );

cd_exit:
    GASendReceiveGalil( iCardNoArg, (char *)"STXYZW\r", cpNull );
    TIDelay( 200 );
    ROServiceBrake(ROBOTFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    TIDelay(100);
    GASendReceiveGalil( iCardNoArg, (char *)"MOXYZW\r", cpNull );

    return SUCCESS;

preTest:

    cAxisName[1] = 0;
    if (uAxisIndexArg == RO_AXIS_t)
        cAxisName[0] = 't';
    else if (uAxisIndexArg == RO_AXIS_r)
        cAxisName[0] = 'r';
    else if (uAxisIndexArg == RO_AXIS_z)
        cAxisName[0] = 'z';
    else if (uAxisIndexArg == RO_AXIS_w)
        cAxisName[0] = 'w';

    sprintf(sTestStr, "Move %s Axis         ", cAxisName);
    TTPrintsAt( 1, 1, sTestStr);
    TTPrintsAt( 2, 1, "to GOOD position    " );
    TTPrintsAt( 3, 1, "Press \"Enter\" when  " );
    TTPrintsAt( 4, 1, "done or ESC         " );

    /* Allow user to move axes to Good position until CR or ESC or Z is pressed. */
    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
    } while ( (cButtonPressed != CR) && (cButtonPressed != ESC) && (cButtonPressed != 'Z') );

    /* If not CR, abort this procedure of making a belt correction. */
    if( cButtonPressed != CR )
        goto pcd_exit;

    /* Proceed to Lock the axis so that user can release the belt.
     * Just turn on the servo. */

    if (uAxisIndexArg == RO_AXIS_t)
        strcpy(sTestStr, "SHE\r");
    else if (uAxisIndexArg == RO_AXIS_r)
        strcpy(sTestStr, "SHF\r");
    else if (uAxisIndexArg == RO_AXIS_z)
        strcpy(sTestStr, "SHG\r");
    else if (uAxisIndexArg == RO_AXIS_w)
        strcpy(sTestStr, "SHH\r");
    GASendReceiveGalil( iCardNoArg, sTestStr, cpNull);

//    ROServiceBrake(PREALIGNFILE, TRUE);

	/* if there is track : m.z.*/
//	if(ROGetSpecialAxis(RO_TRACK))
//		ROServiceBrake(AUXFILE, TRUE);

    ulERFlag &= (unsigned long)(~uAxisIndexArg);
    TIDelay( 500 );

    /* Let user know OK to release the belt. */
    TTClearScreen( );
    sprintf(sTestStr, "Lock %s Axis         ", cAxisName);
    TTPrintsAt( 1, 1, sTestStr );
    TTPrintsAt( 2, 1, "Release The Belt    " );
    TTPrintsAt( 3, 1, "Press \"Enter\" when  " );
    TTPrintsAt( 4, 1, "done or ESC         " );

    /* Temporarily set KP and KD. i.e. soften the tuning. */
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);
    for (iAxis=4; iAxis<7; iAxis++) lParms[iAxis] /= 2;
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);

    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);
    for (iAxis=4; iAxis<7; iAxis++) lParms[iAxis] /= 2;
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);

    /* Wait for user replace the belt. */
    do
    {
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
    } while( (cButtonPressed != ESC) && (cButtonPressed != CR) && (cButtonPressed != 'Z') );

    /* If aborted by user, abort this procedure. */
    if( cButtonPressed != CR )
        goto pcd_exit;

    /* User replaced the belt; make correction by relative move of home index. */
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, ENCODER_RESOLUTION);
    TTPrintsAt( 4, 1, "\n\rMaking correction..." );

    if( uAxisIndexArg == RO_AXIS_t )
        sprintf( sTestStr, "PR,,,,%ld\r", lHomeIndexArg - 2 * lParms[4] );
    else if( uAxisIndexArg == RO_AXIS_r )
        sprintf( sTestStr, "PR,,,,,%ld\r", lHomeIndexArg - 2 * lParms[5] );
    else if( uAxisIndexArg == RO_AXIS_z )
        sprintf( sTestStr, "PR,,,,,,%ld\r", lHomeIndexArg - 2 * lParms[6] );
    else if( uAxisIndexArg == RO_AXIS_w )
        sprintf( sTestStr, "PR,,,,,,,%ld\r", lHomeIndexArg - 2 * lParms[7] );
    GASendReceiveGalil(iCardNoArg, sTestStr, cpNull );

    if( uAxisIndexArg == RO_AXIS_t )
        sprintf( sTestStr, "BGE\r" );
    else if( uAxisIndexArg == RO_AXIS_r )
        sprintf( sTestStr, "BGF\r" );
    else if( uAxisIndexArg == RO_AXIS_z )
        sprintf( sTestStr, "BGG\r" );
    else if( uAxisIndexArg == RO_AXIS_w )
        sprintf( sTestStr, "BGH\r");
    GASendReceiveGalil(iCardNoArg, sTestStr, cpNull );
    ulAMFlag &= (unsigned long)(~uAxisIndexArg);

    /* Wait until motion complete, or user aborts. */
    while( (ulAMFlag & (unsigned long)(uAxisIndexArg)) != (unsigned long)(uAxisIndexArg))
    {
	ROUpdateTS(FALSE);
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
        if ( (cButtonPressed == ESC) || (cButtonPressed == 'Z') )
            goto pcd_exit;
    }

    /* Correction finished...
     * Allow user to replace the belt. */
    TTBeepTP( LONGBELL );
    TTClearScreen( );
    TTPrintsAt( 1, 1, "Correction Done     " );
    TTPrintsAt( 2, 1, "Replace The Belt    " );
    TTPrintsAt( 3, 1, "Press \"Enter\" when  " );
    TTPrintsAt( 4, 1, "done                " );

    /* Restore the original tuning parameters. */
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, PROPORTIONAL_GAIN);
    ROGetParameter(TRUE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);
    ROSetParameter(FALSE, (unsigned long)(uAxisIndexArg), lParms, DERIVATIVE_GAIN);
    TIDelay( 1000 );

    /* Wait until user replaces the belt. */
    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cButtonPressed, FALSE);
    } while( cButtonPressed != CR );

pcd_exit:
    GASendReceiveGalil( iCardNoArg, (char *)"STEFGH\r", cpNull );
    TIDelay( 200 );
//    ROServiceBrake(PREALIGNFILE, FALSE);

	/* if there is track : m.z.*/
//	if(ROGetSpecialAxis(RO_TRACK))
//		ROServiceBrake(AUXFILE, FALSE);

    TIDelay(100);
    GASendReceiveGalil( iCardNoArg, (char *)"MOEFGH\r", cpNull );

    return SUCCESS;

}


/****************************************************************
 *
 * Function:    ROTestRobotIO
 *
 * Abstract:    Diagnostic Robot IO Test. It starts by reading
 *      the current state of the appropriate robot port and
 *      printing it to the screen, bit by bit. On user key
 *      presses it changes the appropriate bit, writes it
 *      out to the port, and verifies the change by reading the
 *      port back in printing its new state to screen ONLY if
 *      it has changed.
 *
 * Parameters:
 *      iDevModeArg     (in) The card number being operated on
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROTestRobotIO(int iDevModeArg)
{
    int iCount, iByteImage, iPortBase;
    char cKeyPressed;

    TTPrintsAt( 1, 1, "BYTE #: 01234567" );
    TTPrintsAt( 3, 1, "OUT:" );
    TTPrintsAt( 4, 1, "IN: " );

    /* Get the IO port address depending on card number. */
    iPortBase = iDevModeArg ? 9: 1;

    /* Read a byte from the galil port. */
    /* Read a byte from the IO port. */
    /* Display the bit image of the bytes read. */
    for( iCount = 0; iCount < 8; iCount++ )
    {
		iByteImage = GAGalilReadOut(iPortBase+iCount);
		if (!iDevModeArg) iByteImage = iByteImage ? 0 : 1;
        if( iByteImage )
            TTPrintcAt( 3, 9+iCount, '1' );
        else
            TTPrintcAt( 3, 9+iCount, '0' );

        if( iByteImage )
            TTPrintcAt( 4, 9+iCount, '1' );
        else
            TTPrintcAt( 4, 9+iCount, '0' );
    }

    /* Get a user input, if in '0' to '7' range, toggle the bit, and display it. */
    do
    {
        SERGetcRxBuff(SERGetTTPort(), &cKeyPressed, FALSE);
        if ( (cKeyPressed >= '0') && (cKeyPressed <= '7') )
        {
            iCount = (int)(cKeyPressed - '0');
			iByteImage = GAGalilReadOut(iPortBase+iCount);
			iByteImage = iByteImage ? 0 : 1;
			GAGalilWriteIO(iPortBase+iCount, iByteImage);
			iByteImage = GAGalilReadOut(iPortBase+iCount);
			if (!iDevModeArg) iByteImage = iByteImage ? 0 : 1;
            TTPrintcAt( 3, 9+iCount, (iByteImage ? '1':'0'));
            TTPrintcAt( 4, 9+iCount, (iByteImage ? '1':'0'));
        }

    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

    return SUCCESS;
}
