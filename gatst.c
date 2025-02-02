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
 * Program:     Motion Control Test Routines (Diagnostics)
 * File:        gatst.c
 * Functions:   GATestGalil
 *              GATestLSHS
 *              GATestEncoder
 *              GASetupAMPBoard
 *              GASetupAMPMode
 *
 * Description: Provide test routines for Motion Control module
 *      testing. They are used by the diagnostics test steps.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <sys/io.h>
#include <stdlib.h>
#include <string.h>
#include "sck.h"
#include "gafn.h"
#include "gag.h"
#include "gatst.h"
#include "romain.h"
#include "roloc.h"
#include "scintr.h"
#include "fiog.h"
#include "sctim.h"
#include "ser.h"
#include "serm.h"
#include "scio.h"
#include "scttr.h"
#include "gaintr.h"
#include "ro.h"
#include "dmclnx.h"

char cpNull[8];
char caTSTcommand[64];
char caTSTresponse[MAXGASTR];
extern HANDLEDMC ghDMC;



/****************************************************************\
 *
 * Function:    GATestGalil
 *
 * Abstract:    Test the Motion Control Card
 *
 * Parameters:  iCardNoArg    (in)
 *
 * Returns:     0
 *
\*****************************************************************/
int GATestGalil( int iCardNoArg )
{
    unsigned long ulAxisIndex;
    char cSSStr[20];
    long lERPar[8];
    int iTempCardNum;
    int iLimitSwitchVal;
    int rc;

    TTPrintsAt( 1,1,"Testing GALIL..." );

    iTempCardNum = iCardNoArg ? GA_CARD_1 : GA_CARD_0;
    GAiUseGalil = iTempCardNum + 2;
    GASendReceiveGalil( iTempCardNum, (char *)"\x0D", cpNull );

    if( GAiUseGalil >= (iTempCardNum + 1))
    {
        GASendReceiveGalil( iTempCardNum, (char *)"\x0D", cpNull );
    }

    if( GAiUseGalil < (iTempCardNum + 1))
    {
        TTPrintsAt( 1,1, "GALIL Failed    " );
    }
    else
    {
        ulAxisIndex = ROAxisAllRobot();

        GASendReceiveGalil( iTempCardNum, (char *)"ST\r", cpNull );
        TIDelay( 100 );
        GASendReceiveGalil( iTempCardNum, (char *)"SH\r", cpNull );
        if( GAiUseGalil <= iTempCardNum )
        {
            TTPrintsAt( 1,1, "GALIL can't servo" );
            goto CANT_SERVO;
        }
        TIDelay( 100 );
//        GASendReceiveGalil( iTempCardNum, (char *)"EI 255,255\xD", cpNull );
//        TIDelay( 100 );
        ROGetERFlag(ROGetERFlag() & ~ulAxisIndex);
        TIDelay( 20 );
        GASendReceiveGalil( iTempCardNum, (char *)"SHX\xD", cpNull );

        if( GAiUseGalil <= iTempCardNum )
        {
            TTPrintsAt( 1,1, "GALIL can't servo" );
            goto CANT_SERVO;
        }

        FIOGetParamVals( (iTempCardNum - 1), ERROR_LIMIT, lERPar );
        /* check if the forward limit switch for Theta is hit.
         * If it is, move to the opposite direction. */
        GASendReceiveGalil( iTempCardNum, (char *)"MG _TSX\r", cSSStr );
        iLimitSwitchVal = atoi(cSSStr);

        //Set appropriate gearing for VAC514 robots
        if(iDefineFlag & DFVAC514)
        {
            if(ROElectronicGearingOFF() == FAILURE)
                return FAILURE;

            if(ROElectronicGearingON('X', 'Y', VAC514_T_GEARING_RATIO, FALSE) == FAILURE)
                return FAILURE;
        }
	// instead of disabling amplifier,
	// we will use low torque to generate ER
	sprintf(caTSTcommand, "TL 0.001");
    	rc = GASendDMCCommand(ghDMC, caTSTcommand, caTSTresponse, MAXGASTR);

        /* Mask off 4th bit of Galil's _TS (limit switch active). */
        if(iLimitSwitchVal & 0x08)
        {
            sprintf( cSSStr, "PR %ld\r", lERPar[0]*2 );
        }
        else
        {
            sprintf( cSSStr, "PR %ld\r", -(lERPar[0]*2) );
        }
        GASendReceiveGalil( iTempCardNum, (char *)cSSStr, cpNull );
        sprintf( cSSStr, "BGX\r");
        GASendReceiveGalil( iTempCardNum, (char *)cSSStr, cpNull );
	// restore torque
	sprintf(caTSTcommand, "TL 9.999");
    	rc = GASendDMCCommand(ghDMC, caTSTcommand, caTSTresponse, MAXGASTR);
        TIDelay( 1000 );
        ROUpdateTS(FALSE);
	
        if( ROGetERFlag() & ulAxisIndex )
        {
            TTPrintsAt( 1,1, "GALIL passed     " );
        }
        else
        {
  	    	// instead of disabling amplifier,
	    	// we will use low torque to generate ER
	    	sprintf(caTSTcommand, "TL ,0.001");
    	    rc = GASendDMCCommand(ghDMC, caTSTcommand, caTSTresponse, MAXGASTR);
            /* Before making decision, test on the 2nd axis */
            /* Mask off 4th bit of Galil's _TS (limit switch active). */
            if(iLimitSwitchVal & 0x08)
            {
                sprintf( cSSStr, "PR ,%ld\r", lERPar[1]*2 );
            }
            else
            {
                sprintf( cSSStr, "PR ,%ld\r", -(lERPar[1]*2) );
            }
            GASendReceiveGalil( iTempCardNum, (char *)cSSStr, cpNull );
            sprintf( cSSStr, "BGY\r");
            GASendReceiveGalil( iTempCardNum, (char *)cSSStr, cpNull );
	    TIDelay(500);
	    ROUpdateTS(FALSE);
  	    // restore torque
	    sprintf(caTSTcommand, "TL ,9.999");
    	    rc = GASendDMCCommand(ghDMC, caTSTcommand, caTSTresponse, MAXGASTR);
            TIDelay( 1000 );
	    ROUpdateTS(FALSE);
            if( ROGetERFlag() & ulAxisIndex )
            {
                TTPrintsAt( 1,1, "GALIL passed     " );
            }
            else
                TTPrintsAt( 1,1, "GALIL Not pass   " );
        }

    }

CANT_SERVO:

    GASendReceiveGalil( iTempCardNum, (char *)"MO\xD", cpNull );

    TIDelay( 1000 );
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GATestLSHS
 *
 * Abstract:    Test the Motion Control Card Limit Switch and Home Switch
 *
 * Parameters:  iCardNoArg    (in)
 *
 * Returns:     0 when user type CR and 1 when ESC
 *
\*****************************************************************/
int GATestLSHS( int iCardNoArg, int iVacExistsArg )
{
    char cKeyPressed;
    int  iAxisIndex;
    int  iTS;
    char cBuf[MAXGASTR];
    char *pCh;
    int  iTempCardNum;
    int  iTrackCardNum = 0;
    int  iDualArmCardNum = 0;
    int  iFlipperCardNum = 0;
    int  iCardToUse = 0;
    int  iMaxAxis = 3;
    int  iVacAxisMask;

    iTempCardNum = iCardNoArg ? GA_CARD_1 : GA_CARD_0;
	if (iTempCardNum == GA_CARD_1) goto preTest;

    if (ROGetSpecialAxis(RO_DUAL_ARM))
    {
        iDualArmCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_FLIPPER))
    {
        iFlipperCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_TRACK))
    {
        if ((ROGetSpecialAxis(RO_TRACK) <= RO_AXIS_W) && (iTempCardNum == GA_CARD_0))
            iTrackCardNum = GA_CARD_0;
    }

    if (((iDualArmCardNum == GA_CARD_0) || (iFlipperCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0)))
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iFlipperCardNum = 0;
        iTrackCardNum = 0;
    }

    // Turn on brake before Motor Off.
    ROServiceBrake(ROBOTFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"MOXYZ\r", cpNull );
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iFlipperCardNum)
        iCardToUse = iFlipperCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
        GASendReceiveGalil( iCardToUse, (char *)"MOW\r", cpNull );

    TTClearScreen( );
    TTPrintsAt( 1, 1, "T:" );
    TTPrintsAt( 2, 1, "R:" );
    TTPrintsAt( 3, 1, "Z:" );
    if ((iTrackCardNum) || (iDualArmCardNum) || (iFlipperCardNum))
    {
        TTPrintsAt( 4, 1, "W:" );
    }
    else
        TTPrintsAt( 4, 1, "Act. Lm. switches" );

    //ROServiceBrake(ROBOTFILE, TRUE);

	/* if there is track : m.z.*/
	//if(ROGetSpecialAxis(RO_TRACK))
	//	ROServiceBrake(AUXFILE, TRUE);

    do
    {
        GASendReceiveGalil( iTempCardNum, (char *)"TS\r", cBuf);
        pCh = strtok( cBuf, "," );
        for( iAxisIndex = 0; iAxisIndex < iMaxAxis; iAxisIndex++ )
        {
            iTS = atoi( pCh );
            if( iTS & POSLIM )
                TTPrintsAt( iAxisIndex+1, 15, " " );
            else
                TTPrintsAt( iAxisIndex+1, 15, "+" );
            if( iTS & NEGLIM )
                TTPrintsAt( iAxisIndex+1, 5, " " );
            else
                TTPrintsAt( iAxisIndex+1, 5, "-" );
            if( iTS & HOMSW )
                TTPrintsAt( iAxisIndex+1, 10, " " );
            else
                TTPrintsAt( iAxisIndex+1, 10, "H" );
            pCh = strtok( NULL, "," );
        }
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

    //ROServiceBrake(ROBOTFILE, FALSE);

	/* if there is track : m.z.*/
	//if(ROGetSpecialAxis(RO_TRACK))
	//	ROServiceBrake(AUXFILE, FALSE);

    return SUCCESS;

preTest:

    if (ROGetSpecialAxis(RO_TRACK))
    {
        if ((ROGetSpecialAxis(RO_TRACK) > RO_AXIS_W) && (iTempCardNum == GA_CARD_1))
            iTrackCardNum = GA_CARD_1;
    }

    if (((iDualArmCardNum == GA_CARD_1) || (iFlipperCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1)))
    {
        iMaxAxis = 4;
    }
    else if ((ROGetSpecialAxis(RO_INDEXER_T2)))
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iFlipperCardNum = 0;
        iTrackCardNum = 0;
    }

    GASendReceiveGalil( iTempCardNum, (char *)"MOEFG\r", cpNull );
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iFlipperCardNum)
        iCardToUse = iFlipperCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
        GASendReceiveGalil( iCardToUse, (char *)"MOH\r", cpNull );

    TTClearScreen( );
    if (iVacExistsArg)
    {
        if (ROGetSpecialAxis(RO_INDEXER_T1))
            TTPrintsAt( 1, 1, "t:" );
        if (ROGetSpecialAxis(RO_INDEXER_Z1))
            TTPrintsAt( 2, 1, "r:" );
        if (ROGetSpecialAxis(RO_INDEXER_Z2))
            TTPrintsAt( 3, 1, "z:" );
        if (ROGetSpecialAxis(RO_INDEXER_T2))
            TTPrintsAt( 4, 1, "w:" );
        else
            TTPrintsAt( 4, 1, "Act. Lm. switches" );
    }
    else
    {
        TTPrintsAt( 1, 1, "t:" );
        TTPrintsAt( 2, 1, "r:" );
        TTPrintsAt( 3, 1, "z:" );

        if ((iTrackCardNum) || (iDualArmCardNum) || (iFlipperCardNum))
        {
            TTPrintsAt( 4, 1, "w:" );
        }
        else
            TTPrintsAt( 4, 1, "Act. Lm. switches" );
    }

    if (iVacExistsArg)
        ROServiceBrake(PREALIGNFILE, TRUE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    do
    {
        GASendReceiveGalil( iTempCardNum, (char *)"TSEFGH\r", cBuf);
        pCh = strtok( cBuf, "," );
        for( iAxisIndex = 0; iAxisIndex < iMaxAxis; iAxisIndex++ )
        {
            iTS = atoi( pCh );
            if (iVacExistsArg && iTempCardNum == GA_CARD_1)
            {
                iVacAxisMask = 1 << (iAxisIndex+4);
                if ((int)(ROGetAllAMFlag()) & iVacAxisMask)
                {
                    if( iTS & POSLIM )
                        TTPrintsAt( iAxisIndex+1, 15, " " );
                    else
                        TTPrintsAt( iAxisIndex+1, 15, "+" );
                    if( iTS & NEGLIM )
                        TTPrintsAt( iAxisIndex+1, 5, " " );
                    else
                        TTPrintsAt( iAxisIndex+1, 5, "-" );
                    if( iTS & HOMSW )
                        TTPrintsAt( iAxisIndex+1, 10, " " );
                    else
                        TTPrintsAt( iAxisIndex+1, 10, "H" );
                }
            }
            else
            {
                if( iTS & POSLIM )
                    TTPrintsAt( iAxisIndex+1, 15, " " );
                else
                    TTPrintsAt( iAxisIndex+1, 15, "+" );
                if( iTS & NEGLIM )
                    TTPrintsAt( iAxisIndex+1, 5, " " );
                else
                    TTPrintsAt( iAxisIndex+1, 5, "-" );
                if( iTS & HOMSW )
                    TTPrintsAt( iAxisIndex+1, 10, " " );
                else
                    TTPrintsAt( iAxisIndex+1, 10, "H" );
            }
            pCh = strtok( NULL, "," );
        }
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

    if (iTempCardNum == GA_CARD_1 && iVacExistsArg)
        ROServiceBrake(PREALIGNFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    return SUCCESS;

}


/****************************************************************\
 *
 * Function:    GATestEncoder
 *
 * Abstract:    Test the Motion Control Card Encoder
 *
 * Parameters:  iCardNoArg    (in)
 *
 * Returns:     0 when user type CR and 1 when ESC
 *
\*****************************************************************/
int GATestEncoder( int iCardNoArg, int iVacExistsArg )
{
    char  cKeyPressed;
    int   iAxisIndex;
    char  cBuf[MAXGASTR];
    char  cBuf1[MAXGASTR];
    char  cBuf2[MAXGASTR];
    char  *pCh;
    int   iTempCardNum;
    int   iTrackCardNum = 0;
    int   iDualArmCardNum = 0;
    int   iSinglePreCardNum = 0;
    int   iCardToUse = 0;
    int   iMaxAxis = 3;
    int   iVacAxisMask;
    int   iBrakeToggle = 0;

    iTempCardNum = iCardNoArg ? GA_CARD_1 : GA_CARD_0;

	if (iTempCardNum == GA_CARD_1) goto preTest;

    if (ROGetSpecialAxis(RO_DUAL_ARM))
    {
        iDualArmCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_TRACK))
    {
        if ((ROGetSpecialAxis(RO_TRACK) <= RO_AXIS_W))
            iTrackCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_SINGLE_PRE))
    {
        iSinglePreCardNum = GA_CARD_0;
    }

    if (((iDualArmCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0) || (iSinglePreCardNum == GA_CARD_0)))
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iTrackCardNum = 0;
        iSinglePreCardNum = 0;
    }

    // Turn on brake before Motor Off.
    ROServiceBrake(ROBOTFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"MOXYZ\r", cpNull );
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iSinglePreCardNum)
        iCardToUse = iSinglePreCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
        GASendReceiveGalil( iCardToUse, (char *)"MOW\r", cpNull );

    TTClearScreen( );
    TTPrintsAt( 1, 1, "T:" );
    TTPrintsAt( 2, 1, "R:" );
    TTPrintsAt( 3, 1, "Z:" );
    if ((iTrackCardNum) || (iDualArmCardNum) || (iSinglePreCardNum))
    {
        TTPrintsAt( 4, 1, "W:" );
    }
    else
        TTPrintsAt( 4, 1, "Move Axes" );

    cBuf1[0] = 0;
    cBuf2[0] = 0;
    do
    {
        GASendReceiveGalil(iTempCardNum, (char *)"TPABCD\r", cBuf);
        if( strcmp( cBuf, cBuf1 ) )
        {
            strcpy( cBuf1, cBuf );
            pCh = strtok( cBuf, "," );
            for( iAxisIndex=0; iAxisIndex < iMaxAxis; iAxisIndex++ )
            {
                TTPrintsAt( iAxisIndex+1, 5, "              " );
                TTPrintsAt( iAxisIndex+1, 5, pCh );
                pCh = strtok( NULL, ",\r" );
            }
        }
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
        if (cKeyPressed == 'J')
        {
            if (iBrakeToggle)
            {
                ROServiceBrake(ROBOTFILE, TRUE);
                iBrakeToggle = 0;
            }
            else
            {
                ROServiceBrake(ROBOTFILE, FALSE);
                iBrakeToggle = 1;
            }
        }
        else if (cKeyPressed == 'E')
        {
            if (iBrakeToggle)
            {
	            /* if there is track : m.z.*/
	            if(ROGetSpecialAxis(RO_TRACK))
                {
		            ROServiceBrake(AUXFILE, TRUE);
                    iBrakeToggle = 0;
                }
            }
            else
            {
	            /* if there is track : m.z.*/
	            if(ROGetSpecialAxis(RO_TRACK))
                {
		            ROServiceBrake(AUXFILE, FALSE);
                    iBrakeToggle = 1;
                }
            }
        }
    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

    //ROServiceBrake(ROBOTFILE, TRUE);

	/* if there is track : m.z.*/
	//if(ROGetSpecialAxis(RO_TRACK))
	//	ROServiceBrake(AUXFILE, TRUE);

    return SUCCESS;

preTest:

    if (ROGetSpecialAxis(RO_TRACK))
    {
        if ((ROGetSpecialAxis(RO_TRACK) > RO_AXIS_W))
            iTrackCardNum = GA_CARD_1;
    }

    if (((iDualArmCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1) || (iSinglePreCardNum == GA_CARD_1)) )
    {
        iMaxAxis = 4;
    }
    else if ((ROGetSpecialAxis(RO_INDEXER_T2)))
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iTrackCardNum = 0;
        iSinglePreCardNum = 0;
    }

    // Turn on brake before Motor Off.
    if (iVacExistsArg)
        ROServiceBrake(PREALIGNFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"MOEFG\r", cpNull );
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iSinglePreCardNum)
        iCardToUse = iSinglePreCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
        GASendReceiveGalil( iCardToUse, (char *)"MOH\r", cpNull );

    TTClearScreen( );
    if (iVacExistsArg)
    {
        if (ROGetSpecialAxis(RO_INDEXER_T1))
            TTPrintsAt( 1, 1, "t:" );
        if (ROGetSpecialAxis(RO_INDEXER_Z1))
            TTPrintsAt( 2, 1, "r:" );
        if (ROGetSpecialAxis(RO_INDEXER_Z2))
            TTPrintsAt( 3, 1, "z:" );
        if (ROGetSpecialAxis(RO_INDEXER_T2))
            TTPrintsAt( 4, 1, "w:" );
        else
            TTPrintsAt( 4, 1, "Move Axes" );
    }
    else
    {
        TTPrintsAt( 1, 1, "t:" );
        TTPrintsAt( 2, 1, "r:" );
        TTPrintsAt( 3, 1, "z:" );
        if ((iTrackCardNum) || (iDualArmCardNum) || (iSinglePreCardNum))
        {
            TTPrintsAt( 4, 1, "w:" );
        }
        else
            TTPrintsAt( 4, 1, "Move Axes" );
    }

    cBuf1[0] = 0;
    cBuf2[0] = 0;
    do
    {
        GASendReceiveGalil(iTempCardNum, (char *)"TPEFGH\r", cBuf);
        if( strcmp( cBuf, cBuf1 ) )
        {
            strcpy( cBuf1, cBuf );
            pCh = strtok( cBuf, "," );
            for( iAxisIndex=0; iAxisIndex < iMaxAxis; iAxisIndex++ )
            {
                if (iVacExistsArg)
                {
                    iVacAxisMask = 1 << (iAxisIndex+4);
                    if ((int)(ROGetAllAMFlag()) & iVacAxisMask)
                    {
                        TTPrintsAt( iAxisIndex+1, 5, "              " );
                        TTPrintsAt( iAxisIndex+1, 5, pCh );
                    }
                }
                else
                {
                    TTPrintsAt( iAxisIndex+1, 5, "              " );
                    TTPrintsAt( iAxisIndex+1, 5, pCh );
                }
                pCh = strtok( NULL, ",\r" );
            }
        }
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
        if (cKeyPressed == 'J')
        {
            if (iBrakeToggle)
            {
                if (iVacExistsArg)
                {
                    ROServiceBrake(PREALIGNFILE, TRUE);
                    iBrakeToggle = 0;
                }
            }
            else
            {
                if (iVacExistsArg)
                {
                    ROServiceBrake(PREALIGNFILE, FALSE);
                    iBrakeToggle = 1;
                }
            }
        }
        else if (cKeyPressed == 'E')
        {
            if (iBrakeToggle)
            {
	            /* if there is track : m.z.*/
	            if(ROGetSpecialAxis(RO_TRACK))
                {
		            ROServiceBrake(AUXFILE, TRUE);
                    iBrakeToggle = 0;
                }
            }
            else
            {
	            /* if there is track : m.z.*/
	            if(ROGetSpecialAxis(RO_TRACK))
                {
		            ROServiceBrake(AUXFILE, FALSE);
                    iBrakeToggle = 1;
                }
            }
        }
    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

    if (iVacExistsArg)
        ROServiceBrake(PREALIGNFILE, TRUE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    return SUCCESS;

}


/****************************************************************\
 *
 * Function:    GASetupAMPBoard
 *
 * Abstract:    Set up the Amplifier Board
 *
 * Parameters:  iCardNoArg    (in)
 *
 * Returns:     0 when user type CR and 1 when ESC
 *
\*****************************************************************/
int GASetupAMPBoard( int iCardNoArg )
{
    int  iTint = 0;
    int  iCount = 0;
    int  iAxisIndex;
    char cBuf[MAXGASTR];
    char cBuf1[MAXGASTR];
    char *pCh;
    int  iTtPort;
    long lHomeSpeed[3];
    int  iTempCardNum;

    iTempCardNum = iCardNoArg ? GA_CARD_1 : GA_CARD_0;

	if (iTempCardNum == GA_CARD_1) goto preTest;

    TTPrintsAt( 1,1,"Make sure all" );
    TTPrintsAt( 2,1,"amplifier modules" );
    TTPrintsAt( 3,1,"OFF the board" );
    TTPrintsAt( 4,1," ESC-Quit    CR-GO" );

    do
    {
        iTtPort = SERGetTTPort();
        SERGetcRxBuff( iTtPort, (char *)&iTint, FALSE );
    }   while( iTint != ESC && iTint != CR && iTint != 'Z' );

    if( iTint == ESC || iTint == 'Z' )
    {
        return FAILURE;
    }

    TTPrintsAt( 4,1,  "Init. AMP. board  " );
    GAAmpInit();
    TIDelay( 500 );
    iTint = inb( IO_ROBOT_INPUT_G );

	iTint = 0;

    if( iTempCardNum == GA_CARD_1 )
    {
        iTint &= 0xFC;
    }
    if( iTint&0xF8 )
    {
        TTPrintsAt( 1,1, "Connection Error" );
        TTPrintsAt( 2,1, "Check Connection" );
        return FAILURE;
    }

    TTClearScreen( );
    TTPrintsAt( 1,1, "T:" );
    TTPrintsAt( 2,1, "R:" );
    TTPrintsAt( 3,1, "Z:" );

    // Only for Robot i.e. (Cardno -1)
    FIOGetParamVals(ROBOTFILE, HOME_SPEED, lHomeSpeed);

    GASendReceiveGalil( iTempCardNum, (char *)"TL9.999,9.999,9.999\r", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"OE0,0,0\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"SH\xD", cpNull );
    TIDelay( 100 );
    ROServiceBrake(ROBOTFILE, TRUE);

    /* if there is track : m.z.*/
//    if(ROGetSpecialAxis(RO_TRACK))
//	ROServiceBrake(AUXFILE, TRUE);

    GASendReceiveGalil( iTempCardNum, (char *)"TT\r", cBuf1);
    sprintf( cBuf, "JG%ld,%ld,%ld\r", lHomeSpeed[0],
                                      lHomeSpeed[1],
                                      lHomeSpeed[2]);
    GASendReceiveGalil( iTempCardNum, (char *)cBuf, cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"BGXYZ\xD", cpNull );
    TTPrintsAt( 4,1, "Move Axes" );

    do
    {
        GASendReceiveGalil( iTempCardNum, (char *)"TT\r", cBuf);
        if( strcmp( cBuf, cBuf1 ) )
        {
            strcpy( cBuf1, cBuf );
            pCh = strtok( cBuf, "," );
            for ( iAxisIndex = 0; iAxisIndex < 3; iAxisIndex++ )
            {
                TTPrintsAt( iAxisIndex+1, 5, "            " );
                TTPrintsAt( iAxisIndex+1, 5, pCh );
                pCh = strtok( NULL, ",\r" );
            }
        }
        iTtPort = SERGetTTPort();
        SERGetcRxBuff( iTtPort, (char *)&iCount, FALSE );
    } while( iCount != CR && iCount != ESC && iCount != 'Z' );
    GASendReceiveGalil( iTempCardNum, (char *)"ST\r", cpNull );
    TIDelay( 100 );
    ROServiceBrake(ROBOTFILE, FALSE);

	/* if there is track : m.z.*/
//	if(ROGetSpecialAxis(RO_TRACK))
//		ROServiceBrake(AUXFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"OE1,1,1\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"MO\xD", cpNull );
    IOWriteIO( -1, 0, IO_ROBOT_OUTPUT_B );

    return SUCCESS;

preTest:

    TTPrintsAt( 1,1,"Make sure all" );
    TTPrintsAt( 2,1,"amplifier modules" );
    TTPrintsAt( 3,1,"OFF the board" );
    TTPrintsAt( 4,1," ESC-Quit    CR-GO" );

    do
    {
        iTtPort = SERGetTTPort();
        SERGetcRxBuff( iTtPort, (char *)&iTint, FALSE );
    }   while( iTint != ESC && iTint != CR && iTint != 'Z' );

    if( iTint == ESC || iTint == 'Z' )
    {
        return FAILURE;
    }

    TTPrintsAt( 4,1,  "Init. AMP. board  " );
    GAAmpInit();
    TIDelay( 500 );
    iTint = inb( IO_PRE_INPUT_G );

	iTint = 0;

    iTint &= 0xFC;
    if( iTint&0xF8 )
    {
        TTPrintsAt( 1,1, "Connection Error" );
        TTPrintsAt( 2,1, "Check Connection" );
        return FAILURE;
	}

    TTClearScreen( );
    TTPrintsAt( 1,1, "t:" );
    TTPrintsAt( 2,1, "r:" );
    TTPrintsAt( 3,1, "z:" );

    // Only for Robot i.e. (Cardno -1)
//    FIOGetParamVals(ROBOTFILE, HOME_SPEED, lHomeSpeed);

    GASendReceiveGalil( iTempCardNum, (char *)"TL,,,,9.999,9.999,9.999\r", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"OE0,0,0\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"SH\xD", cpNull );
    TIDelay( 100 );

    /* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

//    GASendReceiveGalil( iTempCardNum, (char *)"TTEFHG\r", cBuf1);
//    sprintf( cBuf, "JG%ld,%ld,%ld\r", lHomeSpeed[0],
//                                      lHomeSpeed[1],
//                                      lHomeSpeed[2]);
//    GASendReceiveGalil( iTempCardNum, (char *)cBuf, cpNull );
//    GASendReceiveGalil( iTempCardNum, (char *)"BGXYZ\xD", cpNull );

    TTPrintsAt( 4,1, "Move Axes" );

    do
    {
        GASendReceiveGalil( iTempCardNum, (char *)"TTEFGH\r", cBuf);
        if( strcmp( cBuf, cBuf1 ) )
        {
            strcpy( cBuf1, cBuf );
            pCh = strtok( cBuf, "," );
            for ( iAxisIndex = 0; iAxisIndex < 3; iAxisIndex++ )
            {
                TTPrintsAt( iAxisIndex+1, 5, "            " );
                TTPrintsAt( iAxisIndex+1, 5, pCh );
                pCh = strtok( NULL, ",\r" );
            }
        }
        iTtPort = SERGetTTPort();
        SERGetcRxBuff( iTtPort, (char *)&iCount, FALSE );
    } while( iCount != CR && iCount != ESC && iCount != 'Z' );
    GASendReceiveGalil( iTempCardNum, (char *)"STEFGH\r", cpNull );
    TIDelay( 100 );

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"OE,,,,1,1,1\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"MOEFGH\xD", cpNull );
    IOWriteIO( -1, 0, IO_PRE_OUTPUT_B );

    return SUCCESS;

}


/****************************************************************\
 *
 * Function:    GASetupAMPMode
 *
 * Abstract:    Set up the Amplifier Board Mode
 *
 * Parameters:  iCardNoArg    (in)
 *
 * Returns:     0 when user type CR and 1 when ESC
 *
\*****************************************************************/
int GASetupAMPMode( int iCardNoArg, int iVacExistsArg )
{
    int   iAMPOn = TRUE;
    int   iTL = 3333;
    int   iTint;
    char  cKeyPressed;
    int   iAxisIndex;
    char  cBuf[MAXGASTR];
    char  cBuf1[MAXGASTR];
    char  cBuf2[MAXGASTR];
    char  *pCh;
    int   iTempCardNum;
    int   iTrackCardNum = 0;
    int   iDualArmCardNum = 0;
    int   iSinglePreCardNum = 0;
    int   iCardToUse = 0;
    int   iMaxAxis = 3;
    int   iVacAxisMask;

    iTempCardNum = iCardNoArg ? GA_CARD_1 : GA_CARD_0;

	if (iTempCardNum == GA_CARD_1) goto preTest;

    if (ROGetSpecialAxis(RO_DUAL_ARM))
    {
        iDualArmCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_TRACK))
    {
        if ((ROGetSpecialAxis(RO_TRACK) <= RO_AXIS_W))
            iTrackCardNum = GA_CARD_0;
    }
    else if (ROGetSpecialAxis(RO_SINGLE_PRE))
    {
        iSinglePreCardNum = GA_CARD_0;
    }

    if (((iDualArmCardNum == GA_CARD_0) || (iTrackCardNum == GA_CARD_0) || (iSinglePreCardNum == GA_CARD_0)))
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iTrackCardNum = 0;
        iSinglePreCardNum = 0;
    }

    TTPrintsAt( 4,1, "Init. AMP. board" );
    GAAmpInit();
    TIDelay( 500 );
    iTint = inb( IO_ROBOT_INPUT_G );

	iTint = 0;

    if( iTint )
    {
        TTPrintsAt( 1, 1, "Connection Error" );
        TTPrintsAt( 2, 1, "Check Connection" );
        return FAILURE;
    }

    TTClearScreen( );
    TTPrintsAt( 1, 1, "T:" );
    TTPrintsAt( 2, 1, "R:" );
    TTPrintsAt( 3, 1, "Z:" );
    if ((iTrackCardNum) || (iDualArmCardNum) || (iSinglePreCardNum))
    {
        TTPrintsAt( 4, 1, "W:" );
    }
    else
        TTPrintsAt( 4, 1, "Move Axes" );

    GASendReceiveGalil(iTempCardNum, (char *)"TL3.333,3.333,3.333\xD", cpNull );
    GASendReceiveGalil(iTempCardNum, (char *)"OE0,0,0\xD", cpNull );
    GASendReceiveGalil(iTempCardNum, (char *)"SHXYZ\xD" , cpNull);
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iSinglePreCardNum)
        iCardToUse = iSinglePreCardNum;
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
    {
        GASendReceiveGalil(iCardToUse, (char *)"TL,,,3.333\xD", cpNull );
        GASendReceiveGalil(iCardToUse, (char *)"OE,,,0\xD", cpNull );
        GASendReceiveGalil(iCardToUse, (char *)"SHW\xD", cpNull );
    }

    ROServiceBrake(ROBOTFILE, TRUE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    TTPrintsAt( 2, 14, "  TL " );
    sprintf( cBuf, "% 4d", iTL );
    TTPrintsAt( 3, 14, cBuf );

    cBuf1[0] = 0;
    cBuf2[0] = 0;
    do
    {
        GASendReceiveGalil(iTempCardNum, (char *)"TT\r", cBuf);
        if( strcmp( cBuf, cBuf1 ) )
        {
            strcpy( cBuf1, cBuf );
            pCh = strtok( cBuf, "," );
            for( iAxisIndex = 0; iAxisIndex < iMaxAxis; iAxisIndex++ )
            {
                TTPrintsAt( iAxisIndex+1, 5, "        " );
                TTPrintsAt( iAxisIndex+1, 5, pCh );
                pCh = strtok( NULL, ",\r" );
            }
        }
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
        switch( cKeyPressed )
        {
            case 'S':
                iTL = iTL==3333?6666:iTL==6666?9999:9999;
                sprintf( cBuf, "TL%1.5lf,%1.5lf,%1.5lf\r", (double)(iTL)/1000.0, (double)(iTL)/1000.0, (double)(iTL)/1000.0 );
                GASendReceiveGalil(iTempCardNum, (char *)cBuf, cpNull );
                sprintf( cBuf, "TL,,,%1.5lf\r", (double)(iTL)/1000.0 );
                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                if (iSinglePreCardNum)
                    iCardToUse = iSinglePreCardNum;
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (ROGetSpecialAxis(RO_INDEXER_T2))
                    iCardToUse = iTempCardNum;
                if (iCardToUse)
                    GASendReceiveGalil(iCardToUse, (char *)cBuf, cpNull );
                sprintf( cBuf, "% 4d", iTL );
                TTPrintsAt( 3, 14, "     " );
                TTPrintsAt( 3, 14, cBuf );
                break;
            case 'R':
                iTL = iTL==9999?6666:iTL==6666?3333:3333;
                sprintf( cBuf, "TL%1.5lf,%1.5lf,%1.5lf\r", (double)(iTL)/1000.0, (double)(iTL)/1000.0, (double)(iTL)/1000.0 );
                GASendReceiveGalil(iTempCardNum, (char *)cBuf, cpNull );
                sprintf( cBuf, "TL,,,%1.5lf\r", (double)(iTL)/1000.0 );
                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                if (iSinglePreCardNum)
                    iCardToUse = iSinglePreCardNum;
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (ROGetSpecialAxis(RO_INDEXER_T2))
                    iCardToUse = iTempCardNum;
                if (iCardToUse)
                    GASendReceiveGalil(iCardToUse, (char *)cBuf, cpNull );
                sprintf( cBuf, "% 4d", iTL );
                TTPrintsAt( 3, 14, "     " );
                TTPrintsAt( 3, 14, cBuf );
                break;
            case 'Z':
            case CR:
            case ESC:
                break;
        }
    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

    ROServiceBrake(ROBOTFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"TL9.999,9.999,9.999\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"OE1,1,1\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"MOXYZ\xD", cpNull );
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iSinglePreCardNum)
        iCardToUse = iSinglePreCardNum;
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
    {
        GASendReceiveGalil( iCardToUse, (char *)"TL,,,9.999\xD", cpNull );
        GASendReceiveGalil( iCardToUse, (char *)"OE,,,1\xD", cpNull );
        GASendReceiveGalil( iCardToUse, (char *)"MOW\xD", cpNull );
    }
    IOWriteIO( -1, 0, IO_ROBOT_OUTPUT_B );

    return SUCCESS;

preTest:

    if (ROGetSpecialAxis(RO_TRACK))
    {
        if ((ROGetSpecialAxis(RO_TRACK) > RO_AXIS_W))
            iTrackCardNum = GA_CARD_1;
    }

    if (((iDualArmCardNum == GA_CARD_1) || (iTrackCardNum == GA_CARD_1) || (iSinglePreCardNum == GA_CARD_1)))
    {
        iMaxAxis = 4;
    }
    else if ((ROGetSpecialAxis(RO_INDEXER_T2)) )
    {
        iMaxAxis = 4;
    }
    else
    {
        iDualArmCardNum = 0;
        iTrackCardNum = 0;
        iSinglePreCardNum = 0;
    }

    TTPrintsAt( 4,1, "Init. AMP. board" );
    GAAmpInit();
    TIDelay( 500 );
    iTint = inb( IO_PRE_INPUT_G );

	iTint = 0;
    iTint &= 0xFC;
    if( iTint )
    {
        TTPrintsAt( 1, 1, "Connection Error" );
        TTPrintsAt( 2, 1, "Check Connection" );
        return FAILURE;
    }

    TTClearScreen( );
    if (iVacExistsArg)
    {
        if (ROGetSpecialAxis(RO_INDEXER_T1))
            TTPrintsAt( 1, 1, "t:" );
        if (ROGetSpecialAxis(RO_INDEXER_Z1))
            TTPrintsAt( 2, 1, "r:" );
        if (ROGetSpecialAxis(RO_INDEXER_Z2))
            TTPrintsAt( 3, 1, "z:" );
        if (ROGetSpecialAxis(RO_INDEXER_T2))
            TTPrintsAt( 4, 1, "w:" );
        else
            TTPrintsAt( 4, 1, "Move Axes" );
    }
    else
    {
        TTPrintsAt( 1, 1, "t:" );
        TTPrintsAt( 2, 1, "r:" );
        TTPrintsAt( 3, 1, "z:" );
        if ((iTrackCardNum) || (iDualArmCardNum) || (iSinglePreCardNum))
        {
            TTPrintsAt( 4, 1, "w:" );
        }
        else
            TTPrintsAt( 4, 1, "Move Axes" );
    }

    GASendReceiveGalil(iTempCardNum, (char *)"TL,,,,3.333,3.333,3.333\xD", cpNull );
    GASendReceiveGalil(iTempCardNum, (char *)"OE,,,,0,0,0\xD", cpNull );
    GASendReceiveGalil(iTempCardNum, (char *)"SHEFG\xD" , cpNull);
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iSinglePreCardNum)
        iCardToUse = iSinglePreCardNum;
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
    {
        GASendReceiveGalil(iCardToUse, (char *)"TL,,,,,,,3.333\xD", cpNull );
        GASendReceiveGalil(iCardToUse, (char *)"OE,,,,,,,0\xD", cpNull );
        GASendReceiveGalil(iCardToUse, (char *)"SHH\xD", cpNull );
    }

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, TRUE);

    TTPrintsAt( 2, 14, "  TL " );
    sprintf( cBuf, "% 4d", iTL );
    TTPrintsAt( 3, 14, cBuf );

    cBuf1[0] = 0;
    cBuf2[0] = 0;
    do
    {
        GASendReceiveGalil(iTempCardNum, (char *)"TTEFGH\r", cBuf);
        if( strcmp( cBuf, cBuf1 ) )
        {
            strcpy( cBuf1, cBuf );
            pCh = strtok( cBuf, "," );
            for( iAxisIndex = 0; iAxisIndex < iMaxAxis; iAxisIndex++ )
            {
                if (iVacExistsArg)
                {
                    iVacAxisMask = 1 << (iAxisIndex+4);
                    if ((int)(ROGetAllAMFlag()) & iVacAxisMask)
                    {
                        TTPrintsAt( iAxisIndex+1, 5, "        " );
                        TTPrintsAt( iAxisIndex+1, 5, pCh );
                    }
                }
                else
                {
                    TTPrintsAt( iAxisIndex+1, 5, "        " );
                    TTPrintsAt( iAxisIndex+1, 5, pCh );
                }
                pCh = strtok( NULL, ",\r" );
            }
        }
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
        switch( cKeyPressed )
        {
            case 'S':
                iTL = iTL==3333?6666:iTL==6666?9999:9999;
                sprintf( cBuf, "TL,,,,%1.5lf,%1.5lf,%1.5lf\r", (double)(iTL)/1000.0, (double)(iTL)/1000.0, (double)(iTL)/1000.0 );
                GASendReceiveGalil(iTempCardNum, (char *)cBuf, cpNull );
                sprintf( cBuf, "TL,,,,,,,%1.5lf\r", (double)(iTL)/1000.0 );
                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                if (iSinglePreCardNum)
                    iCardToUse = iSinglePreCardNum;
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (ROGetSpecialAxis(RO_INDEXER_T2))
                    iCardToUse = iTempCardNum;
                if (iCardToUse)
                    GASendReceiveGalil(iCardToUse, (char *)cBuf, cpNull );
                sprintf( cBuf, "% 4d", iTL );
                TTPrintsAt( 3, 14, "     " );
                TTPrintsAt( 3, 14, cBuf );
                break;
            case 'R':
                iTL = iTL==9999?6666:iTL==6666?3333:3333;
                sprintf( cBuf, "TL,,,,%1.5lf,%1.5lf,%1.5lf\r", (double)(iTL)/1000.0, (double)(iTL)/1000.0, (double)(iTL)/1000.0 );
                GASendReceiveGalil(iTempCardNum, (char *)cBuf, cpNull );
                sprintf( cBuf, "TL,,,,,,,%1.5lf\r", (double)(iTL)/1000.0 );
                if (iDualArmCardNum)
                    iCardToUse = iDualArmCardNum;
                if (iSinglePreCardNum)
                    iCardToUse = iSinglePreCardNum;
                if (iTrackCardNum)
                    iCardToUse = iTrackCardNum;
                if (ROGetSpecialAxis(RO_INDEXER_T2))
                    iCardToUse = iTempCardNum;
                if (iCardToUse)
                    GASendReceiveGalil(iCardToUse, (char *)cBuf, cpNull );
                sprintf( cBuf, "% 4d", iTL );
                TTPrintsAt( 3, 14, "     " );
                TTPrintsAt( 3, 14, cBuf );
                break;
            case 'Z':
            case CR:
            case ESC:
                break;
        }
    } while( (cKeyPressed != CR) && (cKeyPressed != ESC) && (cKeyPressed != 'Z') );

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    GASendReceiveGalil( iTempCardNum, (char *)"TL,,,,9.999,9.999,9.999\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"OE,,,,1,1,1\xD", cpNull );
    GASendReceiveGalil( iTempCardNum, (char *)"MOEFG\xD", cpNull );
    if (iDualArmCardNum)
        iCardToUse = iDualArmCardNum;
    if (iSinglePreCardNum)
        iCardToUse = iSinglePreCardNum;
    if (iTrackCardNum)
        iCardToUse = iTrackCardNum;
    if (ROGetSpecialAxis(RO_INDEXER_T2))
        iCardToUse = iTempCardNum;
    if (iCardToUse)
    {
        GASendReceiveGalil( iCardToUse, (char *)"TL,,,,,,,9.999\xD", cpNull );
        GASendReceiveGalil( iCardToUse, (char *)"OE,,,,,,,1\xD", cpNull );
        GASendReceiveGalil( iCardToUse, (char *)"MOH\xD", cpNull );
    }
    IOWriteIO( -1, 0, IO_PRE_OUTPUT_B );

    return SUCCESS;

}
