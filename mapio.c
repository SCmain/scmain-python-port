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
 * Program:     Scan Mapper IO Module
 * File:        mapio.c
 * Functions:
 *
 *      // Function prototypes
 *      int     MPInitScanIO();
 *      int     MPEnableWES();
 *      void    MPDisableWES();
 *      void    MPDisableTestWES();
 *      int     MPEnableTestWES();
 *      int     MPReadLatchedPosition(); 
 *
 *      // new Interface functions for globals
 *      int     MPIsNotScanning();
 *      void    MPSetNotScanning();
 *      int     MPGetHitcount()
 *      void    MPSetScanPointers()
 *      void    MPInitScanDataBuffer();
 *      int     MPTestScanning(int iCardNoArg, int iVacExistsArg);
 *
 * Description: Mapper I/O functions & interrupt service
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Description
 *
\******************************************************************************/
#include <sys/io.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "sck.h"
#include "ser.h"
#include "scintr.h"
#include "scio.h"
#include "gag.h"
#include "gaintr.h"
#include "sctim.h"
#include "mapfn.h"
#include "mapio.h"
#include "mapk.h"
#include "ro.h"
#include "rofio.h"
#include "fiog.h"
#include "scttr.h"
#include "scstat.h"

// Globals
// These are passed by interface functions.
int         iTestWESFlag = 0;
long        lTestWESPos  = 0L;
long        lTestWESPos1 = 0L;
unsigned    uScanBitValue = NOTSCANNING;
int         iTestAxisPos = 2;
unsigned long   ulTestAxis = RO_AXIS_Z;
unsigned long   ulMapAxis = RO_AXIS_Z;
int         iGACardNum = GA_CARD_0;

int			giMapIORunning = 0;
int			giMapTest = 0;
int			giMapSensorStatus = 1;

int 	giSOPToggle;

//static int  iSOP;
int	giSOPprev;

long        lZMechRatio;
int         iMapperAxis = MP_ON_ROBOT_VAL_0;

int GAGalilReadOut(int);
int GAGalilReadIO(int);
void GAGalilWriteIO(int, int);

// pointers only used in scan IO
long *lpZPos;
int  *ipTrans;
int  *ipHitcount;

char cBuf[80];
char cpNull[8];

pthread_t threadMP;

void *procMP(void *ptr)
{
    unsigned char iC;
    struct timespec ntv, mtv;

    ntv.tv_sec = 0;
    ntv.tv_nsec = 500000;
    mtv.tv_sec = 0;
    mtv.tv_nsec = 10000;

    while (1)
    {
        if (giMapIORunning)
	{
	    MPUpdateIO(TRUE);
	    nanosleep(&mtv, NULL);
	}
        nanosleep(&ntv, NULL);
    }
    // will not reach here!
    pthread_exit(ptr);
}


void MPUpdateIO(int iFlagArg)
{
    int iStatus, iSOP;
//    char caTPcommand[5] = "TPY\xD";
//    char caResp[MAXGASTR];
//    int rc;
//    long lTP;

    if (iFlagArg & giMapIORunning)
    {
//	iStatus = inb(SCANNERPORT) & 0x04;
	iSOP = inb(SCANNERPORT) & 0x04;
//	iSOP = iStatus ? 2 : 0;

        if ( iSOP == giSOPprev ) // wafer signal not changed
	    return;

	// Yes, wafer detected signal changed
	giSOPprev = iSOP;

        // Get Z axis position
//	rc = DMCCommand(ghDMC, caTBcommand, caResp, MAXGASTR);
//	lTP = atol(caResp);

        if (MPReadLatchedPosition( &lTestWESPos ) == FAILURE)
            lTestWESPos = -1L;

        // now store the data in buffer
        // Make sure pointer does not go beyond the buffer size
        if (*ipHitcount < SCAN_ARRAY_SIZE)
        {
            lpZPos[*ipHitcount] = lTestWESPos;
            ipTrans[*ipHitcount] = iSOP;
            ++(*ipHitcount);
        }
    }
}


/****************************************************************
 *
 * Function:    MPInitScanIO
 *
 * Abstract:    Initialize ScanIO globals and interrupt vector
 *
 * Parameters:  none
 * Returns:     none
 *
 ***************************************************************/
int MPInitScanIO( void )
{
    int iReadData;
    long lMechRatio[8];
    unsigned long ulAxis;
//	int rc;
//	pthread_t threadMap;
    giSOPToggle = 0;
    giSOPprev = 0;
    iTestWESFlag = 0;
    lTestWESPos = 0L;
    lTestWESPos1 = 0L;
    uScanBitValue = NOTSCANNING;
    giMapIORunning = 0;
    giMapTest = 0;
    giMapSensorStatus = inb(SCANNERPORT);

    iGACardNum = GA_CARD_0;

    if(FIOGetParamVals(ROBOTFILE, MECHANICAL_RATIO, lMechRatio) != SUCCESS)
	return FAILURE;

    lZMechRatio = lMechRatio[2];
    ulMapAxis = RO_AXIS_Z;

    uScanBitValue = 0;

    return SUCCESS;
/*
    // Store the current scanner axis Mech Ratio,
    // to be used in converting encoder reading into position value
    if( (iMapperAxis == MP_ON_ROBOT_VAL_0) || (iMapperAxis == MP_ON_ROBOT_VAL_1) )
    {
        // Mapper mounted on robot
        if( FIOGetParamVals(ROBOTFILE, MECHANICAL_RATIO, lMechRatio) != SUCCESS )
            return FAILURE;
        lZMechRatio = lMechRatio[2];
        ulMapAxis = RO_AXIS_Z;
    }
    else if( iMapperAxis == MP_ON_VAC_IDX_Z1 || iMapperAxis == MP_ON_VAC_IDX_Z2)
    {
        // Mapper mounted on vacuum indexer
        if( FIOGetParamVals(PREALIGNFILE, MECHANICAL_RATIO, lMechRatio) != SUCCESS )
            return FAILURE;
        // Mapper mounted on first vacuum indexer
        if( iMapperAxis == MP_ON_VAC_IDX_Z1 )
        {
            lZMechRatio = lMechRatio[1];
            ulMapAxis = ROGetSpecialAxis(RO_INDEXER_Z1);
        }
        // Mapper mounted on second vacuum indexer
        else if( iMapperAxis == MP_ON_VAC_IDX_Z2 )
        {
            lZMechRatio = lMechRatio[2];
            ulMapAxis = ROGetSpecialAxis(RO_INDEXER_Z2);
        }
        iGACardNum = GA_CARD_1;
    }
    else if( iMapperAxis == MP_ON_TRACK_VAL_0 || iMapperAxis == MP_ON_TRACK_VAL_1)
    {
        // Mapper mounted on vacuum indexer
        if( FIOGetParamVals(AUXFILE, MECHANICAL_RATIO, lMechRatio) != SUCCESS )
            return FAILURE;
        // Mapper mounted on track
        lZMechRatio = lMechRatio[1];
        ulMapAxis = ROGetSpecialAxis(RO_TRACK);
        if( iMapperAxis == MP_ON_TRACK_VAL_1)
            iGACardNum = GA_CARD_1;
    }
    else // invalid value passed in 
        return FAILURE;

    if (ulMapAxis == 0) // no such axis exists
        return FAILURE;

    uScanBitValue = 0;

    return SUCCESS;
*/
}


/****************************************************************
 *
 * Function:    MPSetScanPointers
 *
 * Abstract:    Set Scan data buffer pointers.
 *
 * Parameters:
 *          ipCountArg  - pointer to counter array
 *          lpZPosArg   - pointer to Z Position data array
 *          ipTransArg  - pointer to Z Trans (UP/DOWN) data array
 * Returns:     none
 *
 ***************************************************************/
void MPSetScanPointers(int *ipCountArg, long *lpZPosArg, int *ipTransArg)
{
    ipHitcount = ipCountArg;
    lpZPos = lpZPosArg;
    ipTrans = ipTransArg;
    return;
}

/****************************************************************
 *
 * Function:    MPInitScanDataBuffer
 *
 * Abstract:    Initialize Scan data buffer to 0's.
 *              Data buffers are pointed by the global pointers.
 *
 * Parameters:  none
 *
 * Returns:     none
 *
 ***************************************************************/
void MPInitScanDataBuffer( void )
{
    memset(lpZPos, 0, sizeof(long)*SCAN_ARRAY_SIZE*4);
    memset(ipTrans, 0, sizeof(int)*SCAN_ARRAY_SIZE*4);
}

/****************************************************************
 *
 * Function:    MPEnableWES
 *
 * Parameters:  none
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPEnableWES( void )
{
	giMapTest = 0;
	giMapIORunning = 1;
	giMapSensorStatus = 1;

    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPDisableWES
 *
 * Parameters:  none
 *
 * Returns:     none
 *
 ***************************************************************/
void MPDisableWES( void )
{
	giMapTest = 0;
	giMapIORunning = 0;
    return;
}


/****************************************************************
 *
 * Function:    MPEnableTestWES
 *
 * Parameters:  none
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int  MPEnableTestWES( void )
{
	giMapTest = 1;
	giMapIORunning = 1;
	giMapSensorStatus = 1;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPDisableTestWES
 *
 * Parameters:  none
 *
 * Returns:     none
 *
 ***************************************************************/
void MPDisableTestWES( void )
{
	giMapTest = 0;
	giMapIORunning = 0;
    return;
}


/****************************************************************
 *
 * Function:    MPIsNotScanning
 *
 * Abstract:    Boolean function to return TRUE if uScanBitValue is
 *              set to NOTSCANNING.
 *
 * Parameters:  none
 *
 * Returns:     TRUE or FALSE
 *
 ***************************************************************/
int MPIsNotScanning( )
{
    return ((uScanBitValue == NOTSCANNING) ? TRUE : FALSE);
}

/****************************************************************
 *
 * Function:    MPSetNotScanning
 *
 * Abstract:    Sets uScanBitValue to NOTSCANNING (formerly, scan_bit_value)
 *
 * Parameters:  none
 *
 * Returns:     none
 *
 ***************************************************************/
void MPSetNotScanning( )
{
    uScanBitValue = NOTSCANNING;
}

/****************************************************************
 *
 * Function:    MPReadLatchedPosition
 *
 ***************************************************************/
int MPReadLatchedPosition(long *lValue)
{
    char cpPosBuf[25];
    long lPos;
    double dTemp;

    // Send galil commands to Tell Position and get the response string
    // from galil
    // Mapper mounted on robot
    if (GASendReceiveGalil(iGACardNum, "TPZ\r", cpPosBuf) == FAILURE)
        goto error_exit;

/*    if( (iMapperAxis == MP_ON_ROBOT_VAL_0) || (iMapperAxis == MP_ON_ROBOT_VAL_1) )
    {
        // Mapper mounted on robot
        if (GASendReceiveGalil(iGACardNum, "TPZ\r", cpPosBuf) == FAILURE)
            goto error_exit;
    }
    else if (iMapperAxis == MP_ON_VAC_IDX_Z1 || iMapperAxis == MP_ON_VAC_IDX_Z2)
    {
        // Mapper mounted on vacuum indexer
        // Mapper mounted on first vacuum indexer
        if( iMapperAxis == MP_ON_VAC_IDX_Z1 )
        {
            if (GASendReceiveGalil(iGACardNum, "TPY\r", cpPosBuf) == FAILURE)
                goto error_exit;
        }
        // Mapper mounted on second vacuum indexer
        else if( iMapperAxis == MP_ON_VAC_IDX_Z2 )
        {
            if (GASendReceiveGalil(iGACardNum, "TPZ\r", cpPosBuf) == FAILURE)
                goto error_exit;
        }
    }
    else if (iMapperAxis == MP_ON_TRACK_VAL_0 || iMapperAxis == MP_ON_TRACK_VAL_1)
    {
        if (GASendReceiveGalil(iGACardNum, "TPW\r", cpPosBuf) == FAILURE)
            goto error_exit;
    }
    else
        goto error_exit;
*/

    lPos = atol(cpPosBuf);
    // Convert the encoder reading to ESC counts using double arithmatic.
    // Changed from long int multiplication due to the integer overflow occurred
    // during vertical track mapping.
    // Note: Z & track scaling methods are identical.
    dTemp = (double)lZMechRatio / 100.0;
    dTemp = dTemp * (double)lPos;
    *lValue = (unsigned long) dTemp;

    return SUCCESS;

error_exit:
    return FAILURE;
}

/****************************************************************
 *
 * Function:    MPTestScanning
 *
 * Abstract:    Mapper test routine for diagnostic step 13
 *              This routine now takes care of both Z axis mapping and vertical
 *              track mapping. The track can be either on the first card as W-axis or
 *              on the second card as w-axis. Thus, the usage of iCardNum must be carefully
 *              noted.
 *
 * Parameters:  iCardNoArg -    The card number to test the scanner on
 *              iVacExistsArg - Whether or not a vacuum indexer exists
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPTestScanning( int iCardNoArg, int iVacExistsArg )
{
    int  iCount, iStatus;
    char cAxis=0;
    char cCount;
    char cTscr[10];
    int  iGAOutByte;
    int  iPortBase, iData;
    long laSaveSpeed[8];
    long laParms[8];
    unsigned uGalilAxis=GAZAXIS0;
    int  iCardNum;
    unsigned long ulTrackAxis = 0;

    TTSetKNPMode(TRUE);
    TTClearScreen();
    TTPrintsAt( 1, 5, "SCANNING TEST" );

    // iCardNum is to be used only in axis related command.
    // This is implemented as a result of track mapping. Track can be on card 0 or 1.
    iCardNum = iCardNoArg;
    iPortBase = 1;

    // Decide which axis to read position from
    ulTrackAxis = ROGetSpecialAxis(RO_TRACK);
    if (ulTrackAxis != 0)
    {   // track exists, let user decide which axis to use for scanning
        TTPrintsAt( 3, 1, " Z or Track?<0/1>" );
        while (!cAxis)
        {
            // Get user's input (KEY pressed from Teach pendant)
            SERGetcRxBuff(SERGetTTPort(), &cCount, FALSE);
            switch (cCount)
            {
                case '0' :
                    iTestAxisPos = 2;
                    ulTestAxis = RO_AXIS_Z;
                    cAxis = 'Z';
                    break;
                case '1' :  // track is w or W
                    iTestAxisPos = 3;
                    ulTestAxis = ulTrackAxis;
                    uGalilAxis = GAWAXIS0;
                    if(ulTestAxis == RO_AXIS_w)
                    {
                        cAxis = 'w';
                        iCardNum = iCardNoArg + 1;
                    }
                    else
                        cAxis = 'W';
                    break;
            }
        }
    }
    else
    {   // No track exists, just use Z-axis
        iTestAxisPos = 2;
        ulTestAxis = RO_AXIS_Z;
        cAxis = 'Z';
    }

    ROGetParameter(FALSE, ulTestAxis, laSaveSpeed, OPERATIONAL_SPEED);
    ROGetParameter(TRUE, ulTestAxis, laParms, HOME_SPEED);
    // Hardcode the value for scanning speed, minimum required for S293 system
    laParms[2] = Z_S293_SPEED;
    ROSetParameter(FALSE, ulTestAxis, laParms, OPERATIONAL_SPEED);

    TTPrintsAt( 2, 1, "S-Servo" );
    TTPrintsAt( 3, 1, "OUT:              " );
    TTPrintsAt( 4, 1, "AT:" );
    if (ROGetServoFlag() & ulTestAxis)
        sprintf(cTscr, "OFF ALL");
    else
        sprintf(cTscr, "ON ALL ");
    TTPrintsAt( 2, 10, cTscr );

    /* Get the IO port address depending on card number. */
    /* Read a byte from the galil port. */
    /* Read a byte from the IO port. */
    //iGAOutByte = ROReadGalilPort( iCardNoArg );
    iGAOutByte = inb(IO_ROBOT_OUTPUT_A);
    for (iCount=0; iCount<=7; iCount++)
    {
        if (iGAOutByte&(1<<iCount))
            cCount = '1';
        else
            cCount = '0';
        TTPrintcAt( 3, 6+iCount, cCount );
    }

    MPEnableTestWES();
    giMapSensorStatus = inb(IO_ROBOT_INPUT_F) & 0x04;

    // Mapper test involves
    //  (1) toggling ON/OFF on ports 0 to 7 or 40 to 47
    //  (2) at port 2 ON or 40/41 ON (laser ON).
    //  (3) toggling the servo ON/OFF and jogging the correct axis
    // When laser detects an object (ie, wafer), display LO->HI message (UP)
    // when laser does not detect an object, display HI->LO message (DOWN)
    // On either LO->HI or HI->LO, display the correct axis position
    do
    {
        // Get user's input (KEY pressed from Teach pendant)
        SERGetcRxBuff(SERGetTTPort(), &cCount, FALSE);
        switch (cCount)
        {
            case 0 :
                if (~ROGetAMFlag() & ulTestAxis)
                    ROStopMotion(ulTestAxis);
                break;
            case EOF:
                /* Check if there was any error during motion. */
		ROUpdateTS(FALSE);
                if( ROGetERFlag() & ulTestAxis )
                {
                    TTPrintsAt( 2, 10, "Error  " );
                    TTBeepTP( ALERT );
                    ROGetERFlag(ROGetERFlag() & ~ulTestAxis);
                    TIDelay(2000);
                    sprintf(cTscr, "OFF ALL");
                    TTPrintsAt( 2, 10, cTscr );
                }
                break;
            case 'P' :
                if (ulTestAxis & (ROGetServoFlag() | ROGetERFlag() | ~ROGetAMFlag()) )
                {
                    if (ulTestAxis & (ROGetServoFlag() | ROGetERFlag()) )
                        TTBeepTP( ALERT );
                }
                else
                {
                    laParms[iTestAxisPos] = -30000L;
                    GASetValsLong(iCardNum+1, POSITION_ABS_COMMAND, uGalilAxis, laParms);
                    GASendAxesCommand(iCardNum+1, BEGIN_MOTION_COMMAND, uGalilAxis);
                    ROSetAMFlag(ROGetAMFlag() & ~ulTestAxis);
               }
                break;
            case 'Q' :
                if (ulTestAxis & (ROGetServoFlag() | ROGetERFlag() | ~ROGetAMFlag()) )
                {
                    if (ulTestAxis & (ROGetServoFlag() | ROGetERFlag()) )
                        TTBeepTP( ALERT );
                }
                else
                {
                    laParms[iTestAxisPos] = 1000000L;
                    GASetValsLong(iCardNum+1, POSITION_ABS_COMMAND, uGalilAxis, laParms);
                    GASendAxesCommand(iCardNum+1, BEGIN_MOTION_COMMAND, uGalilAxis);
                    ROSetAMFlag(ROGetAMFlag() & ~ulTestAxis);
                }
                break;
            case 'S' :
                if (ROGetServoFlag() & ulTestAxis)
                {
                    //We turn on servo for all available axes to avoid Position Error on the other axes
                    ROServoMCOn(0);
                    //Because we have problem with the brake when entering this step we de/activate brake
                    //again for robot scanning test
                    if(cAxis == 'Z')
                        ROServiceBrake(ROBOTFILE, TRUE);
                    if(cAxis == 'w' || cAxis == 'W')
                        ROServiceBrake(AUXFILE, TRUE);
                    sprintf(cTscr, "ON ALL ");
                }
                else
                {
                    //We turn on servo for all available axes to avoid Position Error on the other axes
                    ROServoMCOff(0);
                    //Because we have problem with the brake when entering this step we de/activate brake
                    //again for robot scanning test
                    if(cAxis == 'Z')
                        ROServiceBrake(ROBOTFILE, FALSE);
                    if(cAxis == 'w' || cAxis == 'W')
                        ROServiceBrake(AUXFILE, FALSE);
                    sprintf(cTscr, "OFF ALL");
                }
                TTPrintsAt( 2, 10, cTscr );
                if (cAxis == 'Z')
                {   // only if Z-axis, update the port IO
		    iGAOutByte = inb(IO_ROBOT_INPUT_F);
		    iGAOutByte &= 0x04;
                    TTPrintcAt( 3, 10, (char) (iGAOutByte ? '1':'0'));
                }
                break;

            default:
                // For user input '0' to '7'
                // Toggle the port ON/OFF, and display the message
		iGAOutByte = inb(IO_ROBOT_OUTPUT_A);
                if ( cCount >= '0' && cCount <= '7' )
                {
                    iCount = cCount - '0';
		    if (iGAOutByte&(1<<iCount))
			iGAOutByte &= ~(1<<iCount);
		    else
			iGAOutByte |= 1<<iCount;
		    outb(iGAOutByte, IO_ROBOT_OUTPUT_A);

                    if (iGAOutByte&(1<<iCount))
                        cCount = '1';
                    else
                        cCount = '0';
                    TTPrintcAt( 3, 6+iCount, cCount );
                }
		break;
        }

        // Detect an object
        // Check if interrupt is generated
        // If so, display the appropriate message depending on the
        // laser detection status
		iStatus = inb(IO_ROBOT_INPUT_F) & 0x04;
		if (giMapSensorStatus != iStatus) 
		{
			MPReadLatchedPosition(&lTestWESPos);
			if (iStatus) iTestWESFlag = 4;
			else iTestWESFlag = 2;
			giMapSensorStatus = iStatus;
		}
		else
			iTestWESFlag = 0;

        if ( iTestWESFlag )
        {
            TTPrintsAt( 4, 5, "       " );
            TTBeepTP( CLICK );
            if ( iTestWESFlag & 2 )
            {
//                ltoa(lTestWESPos, cTscr, 10 );
				sprintf(cTscr, "%ld", lTestWESPos);
                TTPrintsAt( 4, 12, "LO->HI" );
                iTestWESFlag = 0;
            }
            else if ( iTestWESFlag & 4 )
            {
//                ltoa(lTestWESPos1, cTscr, 10 );
				sprintf(cTscr, "%ld", lTestWESPos);
                iTestWESFlag = 0;
                TTPrintsAt( 4, 12, "HI->LO" );
    	    }
            TTPrintsAt( 4, 5, cTscr );
        }
    } while ( cCount != CR && cCount != ESC && cCount != 'Z' );

    MPDisableTestWES();
    TTSetKNPMode(FALSE);
    ROSetParameter(FALSE, ulTestAxis, laSaveSpeed, OPERATIONAL_SPEED);
    ROServoMCOff(0);
    if(cAxis == 'Z')
        ROServiceBrake(ROBOTFILE, FALSE);
    if(cAxis == 'w' || cAxis == 'W')
        ROServiceBrake(AUXFILE, FALSE);

    ROReadGalilStatus(&ulTrackAxis); // will not use the status, just clear GLST

    return SUCCESS;
}
