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
 * Program:     IO Module
 * File:        scio.c
 * Functions:   IO Subsystem functions
 *          Interrupt call function prototypes
 *          void IO_PROC(int bFlag );
 *
 *          Function prototypes
 *          int     IOInitIO(void);         // to be called by main()
 *          int     IOInitIOS293(void);     // to be called by main()
 *
 *          Interface function prototypes
 *          int     IOGetUseInts(void);             // returns bUseInts
 *          void    IOSetIOIntPending(int bFlag);   // sets bIOIntPending
 *          void    IOWriteIO(int iBit, int iData, int iPort);  // write to IO port
 *          int     IOTestIO( int iDevModeArg );    // Diagnostic IO test

 * Description: IO subsystem functions and Interrupts
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/
#include <sys/io.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "sck.h"
#include "sctim.h"
#include "scstat.h"
#include "fiog.h"
#include "gag.h"
#include "scmac.h"
#include "scintr.h"
#include "scio.h"
#include "mapio.h"
#include "mapstn.h"
#include "ser.h"
#include "scttr.h"
#include "ro.h"
#include "otf.h"
#include "scmem.h"
#include "dmclnx.h"

extern HANDLEDMC ghDMC;

extern 	char caALBcommand[10];
extern 	char caRLBcommand[10];
extern 	char caARBcommand[5];
long   	glOTFData[MAX_OTF_INTERRUPTS];
int    	giOTFCounter = 0;
int	giSensorCount = 0;
int	giSensorMap[MAX_OTF_INTERRUPTS];
int	giPrevSensor = 0;
extern  int iOTFFirstTime;

int 	aiPortsMap[MAX_OTF_INTERRUPTS] = {0,0,0,0,0,0,0,0,0,0,0,0};
//int	giPrevTint = 7; // dark-ON
int	giPrevTint = 0; // light-ON
int 	giXYZRobot = 0;

// internal to iosub
unsigned    uCustInpByte;       // formerly, cust_inp_byte
unsigned    uCustInpByte2;      // Extended Input byte 2
unsigned    uCustInpByte3;      // Extended Input byte 3
int         bSpeedCheckingFlag; // formerly, SpeedCheckingFlag
int         LASER_TIMER=-1;
int         bVersion3IOCard;    // formerly, Version3IOCard
int         bInSpeed;           // formerly, InSpeed
int         bUseInts;           // formerly, UseInts
int         bIOIntPending;      // formerly, IOIntPending
unsigned char iInputG = 0;          // formerly, input_G
unsigned char iInputH;          // formerly, input_H
unsigned char iInputI;          // formerly, input_I
// previous status of EMO bit
unsigned uPreviousEMOStatus;
unsigned uPreviousSSTOPStatus;
int 	   giTRKLoopFail = 0;
int giIOCount = 0;
int giIOCountMax = 17;

extern int iPowerDisconnected;
extern int iOTFAligning;

extern int giNumOfAxes;

extern unsigned long ulAMFlag;

unsigned long glTimeStart00 = 0;
unsigned long glTimeStart01 = 0;
unsigned long glTimeStart10 = 0;
unsigned long glTimeStart11 = 0;
unsigned long glTimeStart20 = 0;
unsigned long glTimeStart21 = 0;

int giCheck00Flag = 0;
int giCheck01Flag = 0;
int giCheck10Flag = 0;
int giCheck11Flag = 0;
int giCheck20Flag = 0;
int giCheck21Flag = 0;

int giOutpStnNumber = 0;

int giTimeIOCounter[10][4];
unsigned long glTimeIO[10][4][100];
//		       ^   ^  ^_____ 0 to 99 time lapse in ms
//		       |   |________ 0=(outp 0,0) 1=(outp 0,1) 2=(outp 1,0) 3=(outp 1,1)
//		       |____________ up to 8 stations
	
//pthread_t threadIO;

	  /* * *   I / O   B O A R D   R O U T I N E S	 * * */

/************************************************
 * Function Name:   IOInitIO
 *
 * Description:     Initialize globals and enable IO.
 *
 * Parameter:       none
 * Returns:         SUCCESS/FAILURE
 *
 * ************************************************/
int IOInitIO( void )
{
    int iLEDStatus;
    int i; //,j,k;

    bInSpeed = FALSE;
    bUseInts = TRUE;

    ioperm(IO_DIO_BASE_ADR, 120, 1);

//	ioperm(, 120, 1);
//	ioperm(IO_DIO_BASE_ADR, 24, 1);

//    if((LASER_TIMER = TIGetCounter()) == FAILURE)
//        return FAILURE;
//    if (!bUseInts)
//        return SUCCESS;

    IOWriteIO(-1, IO_DIO_GROUP_1_MODE, IO_DIO_GROUP_1);
    IOWriteIO(-1, IO_DIO_GROUP_2_MODE, IO_DIO_GROUP_2);
    IOWriteIO(-1, IO_DIO_GROUP_3_MODE, IO_DIO_GROUP_3);
    IOWriteIO(-1, IO_DIO_GROUP_4_MODE, IO_DIO_GROUP_4);
    IOWriteIO(-1, IO_DIO_GROUP_5_MODE, IO_DIO_GROUP_5);

//    bIOIntPending = FALSE;


    // Make sure Track Brake engage
    //    Next version will use 0xDF instead of 0xDD.
    //
    i = inb( IO_ROBOT_OUTPUT_B );
    i &= 0xDD;
    outb (i, IO_ROBOT_OUTPUT_B);

    // Initialize EMO & SSTOP status
    uPreviousEMOStatus = inb( IO_ROBOT_INPUT_G ) & EMO;
    uPreviousSSTOPStatus = inb( IO_ROBOT_INPUT_G ) & STOP_SIGNAL;

    i = inb(IO_XYZ_INPUT_G) & 0x01;
    if(i == 0) 
	giXYZRobot = 1;

//    for(i=0; i<10; ++i)
//  	for(j=0; j<4; ++j)
//	    for(k=0; k<100; ++k)
//		glTimeIO[i][j][k] = 0;

    IOReadSTNIO();

//printf("XYZ robot=%d\n",giXYZRobot);

    // turn on YELLOW 
    iLEDStatus = inb(IO_PRE_OUTPUT_L);
//    iLEDStatus &= 0xFB;	// 3rd bit off to turn on YELLOW
//
    iLEDStatus &= 0x7B;	// 3rd bit off to turn on YELLOW, 8th bit should be off
    outb(iLEDStatus, IO_PRE_OUTPUT_L);

    ROServiceBrake(ROBOTFILE, FALSE);

    return SUCCESS;
}

/************************************************
 * Function Name:   IOInitIOS293
 *
 * Description:     Initialize globals and enable IO for S293 support.
 *
 * Parameter:       none
 * Returns:         SUCCESS/FAILURE
 *
 * ************************************************/
int IOInitIOS293( void )
{
//    unsigned long ulSpeedCtrlJumps;
//    long    lpMR[3];

    if (!bUseInts)
        return SUCCESS;

    MPSetScanReady(TRUE);
    // Check if this is a Version 3.X IO card

    return SUCCESS;
}

/********************************************
 * Name: IO_OTF
 *
 ********************************************/
void IO_OTF(void)
{
    unsigned char uSensor;
    int iDupCnt;  //, iDone;
    int iTimerIndex;
    int iTriggerFlag;
    int iDoneFlag;

    if( iOTFFirstTime )
    {
	iOTFFirstTime = FALSE;
	iTriggerFlag = FALSE;
	iDoneFlag = FALSE;
	iDupCnt = 0;
    	while (!iDoneFlag)
	{
	    uSensor = inb(IO_ROBOT_INPUT_H);
	    if(uSensor != giPrevSensor)
	    {
		giPrevSensor = uSensor;
		giSensorMap[giSensorCount++] = (int) uSensor;
//printf("SenCnt=%d Sen=%d iDup=%d\n",giSensorCount, uSensor, iDupCnt);
		iTriggerFlag = TRUE;
	    }
	    if(iTriggerFlag && (uSensor == (int)0xFF))
	    {
		iDoneFlag = TRUE;
printf("SenCnt=%d dup=%d\n",giSensorCount,iDupCnt);
	    }
	    if (++iDupCnt > 299999)	// we need this in case robot didn't move or sensor fails
		iDoneFlag = TRUE;
	}
    }
}

/************************************************
 * Function Name:   IO_PROC
 *
 * ************************************************/
void IO_PROC(int iFlagArg)
{
    unsigned char uTint;
    unsigned char uTint2;
    unsigned char inpK;

    //char ReturnBuffer[MAXGASTR];

    //int iOutputL;
    //int rc;
    //long lTemp;
    unsigned long ulTime;

    // in future we may have 8 Customer Ports, possible use for OTF

//    uTint = inb(IO_PRE_OUTPUT_L);
//    if (uTint & 2)
//printf("pin vacuum on, uTint = %d\n",uTint);


    if(iFlagArg & (++giIOCount < giIOCountMax)) return;
    giIOCount = 0;


    // Check cable connections
    uTint = inb( IO_ROBOT_INPUT_G );
    uTint2 = inb( IO_PRE_INPUT_K );

    if ( (uTint & EMO) != uPreviousEMOStatus )
    {
	iInputG |= EMO;

        uPreviousEMOStatus = (uTint & EMO);
        ROSetEMOWasSet(TRUE);
        if (MRGetMacroIntegerVars(MR_RUNNING_INTLK))
        {
            MASetMacroIntegerVars(MR_PENDING_INTLK, TRUE);
        }
        else if (!MRGetMacroIntegerVars(MR_RUNNING_CMER))
        {
            MASetMacroIntegerVars(MR_INTLK_INTERRUPT, TRUE);
            MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);
        }
    }

    // Check STOP signal cable
    if ( (uTint & STOP_SIGNAL) != uPreviousSSTOPStatus )
    {
	iInputG |= STOP_SIGNAL;

        uPreviousSSTOPStatus = (uTint & STOP_SIGNAL);

        // Make sure this SSTOP is being serviced only if neither CMER nor INTLK is running
        if (MRGetMacroIntegerVars(MR_RUNNING_SSTOP))
        {
            MASetMacroIntegerVars(MR_PENDING_SSTOP, TRUE);
        }
        else if (!MRGetMacroIntegerVars(MR_RUNNING_CMER) &&
                 !MRGetMacroIntegerVars(MR_RUNNING_INTLK) )
        {
            MASetMacroIntegerVars(MR_SSTOP_INTERRUPT, TRUE);
            MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);
        }
    }

    if (uTint2 & 0xC0)
    {
	// just read uTint2 again to make sure it's not a noise
 	giTRKLoopFail++;
	inpK = inb( IO_PRE_INPUT_K );
	if (inpK & 0xC0)
	{
	    giTRKLoopFail++;
	    if (inpK & 0xC0)
	    {
	    	giTRKLoopFail++;
		if (inpK & 0xC0)
		{
		    giTRKLoopFail++;
//printf("First noise? uT2=%x, count=%d\n", inpK, giTRKLoopFail);
		}
	    }
	}
    }
    else
    {
	giTRKLoopFail = 0;
    }


    // Automatically disconnected
//    if ( uTint & ALL_LOOPS_FAIL || uTint2 & 0x80)       // No closed loop

    if (giNumOfAxes > 4)
    {
	if ( ((uTint & ALL_LOOPS_FAIL) || (uTint2 & 0xC0)) && (giTRKLoopFail >= 4))       // No closed loop
//    if ( uTint & 0xF9 || uTint2 & 0x80)       // No closed loop
    	{
	    if(!iPowerDisconnected)
	    {
        	RODisconnectPower();   
//printf("Disconnect Power: uT1=%x uT2=%x count=%d\n",uTint, uTint2, giTRKLoopFail);
	    }
    	}
    	else
    	{
	    iPowerDisconnected = FALSE;
	    iInputG = 0;
	    giTRKLoopFail = 0;
    	}
    }
    else // no need for the next set of amps check (ONLY 4 axes)
    {
	if (uTint & 0x7F)
    	{
	    if(!iPowerDisconnected)
	    {
        	RODisconnectPower();   
//printf("Disconnect Power: uT1=%x uT2=%x count=%d\n",uTint, uTint2, giTRKLoopFail);
	    }
    	}
    	else
    	{
	    iPowerDisconnected = FALSE;
	    iInputG = 0;
	    giTRKLoopFail = 0;
    	}
    }

    uTint = inb( IO_ROBOT_OUTPUT_B) & 0x10; // check Z amp disabled?
    uTint2 = inb (IO_ROBOT_OUTPUT_A) & 0x10; // check Z brake
    if (!uTint && uTint2)
    {
	ROServiceBrake(ROBOTFILE, FALSE);
//printf("Power down by outp B\n");
    }


    // check for IO changes for vacuum
    // timeout at 2 seconds
    //
    uTint = inb( IO_ROBOT_INPUT_F);
    ulTime = TIRSTime();

    if (giCheck00Flag)	// outp 0,0
    {
	if(((uTint & 0x01) == 0) || (labs(ulTime - glTimeStart00)>1999))
	{
	    glTimeIO[giOutpStnNumber][0][giTimeIOCounter[giOutpStnNumber][0]++] = labs(ulTime - glTimeStart00);
	    if (giTimeIOCounter[giOutpStnNumber][0] >= 100)
		giTimeIOCounter[giOutpStnNumber][0] = 0;
	    giCheck00Flag = 0;
	}
    }
    else if (giCheck01Flag) // outp 0,1
    {
	if(((uTint & 0x01) == 1) || (labs(ulTime - glTimeStart01)>1999))
	{
	    glTimeIO[giOutpStnNumber][1][giTimeIOCounter[giOutpStnNumber][1]++] = labs(ulTime - glTimeStart01);
	    if (giTimeIOCounter[giOutpStnNumber][1] >= 100)
		giTimeIOCounter[giOutpStnNumber][1] = 0;
	    giCheck01Flag = 0;
	}
    }
    else if (giCheck10Flag) // outp 1,0
    {
	if(((uTint & 0x02) == 0) || (labs(ulTime - glTimeStart10)>1999))
	{
	    glTimeIO[giOutpStnNumber][2][giTimeIOCounter[giOutpStnNumber][2]++] = labs(ulTime - glTimeStart10);
	    if (giTimeIOCounter[giOutpStnNumber][2] >= 100)
		giTimeIOCounter[giOutpStnNumber][2] = 0;
	    giCheck10Flag = 0;
	}
    }
    else if (giCheck11Flag) // outp 1,1
    {
	if(((uTint & 0x02) == 1) || (labs(ulTime - glTimeStart11)>1999))
	{
	    glTimeIO[giOutpStnNumber][3][giTimeIOCounter[giOutpStnNumber][3]++] = labs(ulTime - glTimeStart11);
	    if (giTimeIOCounter[giOutpStnNumber][3] >= 100)
		giTimeIOCounter[giOutpStnNumber][3] = 0;
	    giCheck11Flag = 0;
	}
    }


    inpK = inb( IO_PRE_INPUT_K );
    if (giCheck20Flag) // output port L, 0
    {
	if(((inpK & 0x01) == 1) || (labs(ulTime - glTimeStart20)>1999))
	{
	    glTimeIO[9][0][giTimeIOCounter[9][0]++] = labs(ulTime - glTimeStart20);
	    if (giTimeIOCounter[9][0] >= 100)
		giTimeIOCounter[9][0] = 0;
	    giCheck20Flag = 0;
	}
    }
    else if (giCheck21Flag) // output port L, 1
    {
	if(((inpK & 0x01) == 0) || (labs(ulTime - glTimeStart21)>1999))
	{
	    glTimeIO[9][1][giTimeIOCounter[9][1]++] = labs(ulTime - glTimeStart21);
	    if (giTimeIOCounter[9][1] >= 100)
		giTimeIOCounter[9][1] = 0;
	    giCheck21Flag = 0;
	}
    }

    // Check for OTF data acquisition
//    if(iOTFAligning)

    // Check for transition to trigger macro IIM on basic customer input port
    uTint = inb( IO_ROBOT_INPUT_H );

    // collect sensor data if OTF is running
//    uTint &= 0x07;	// only the first 3 bits
//   if((uTint != giPrevTint) && iOTFAligning && (giSensorCount <= MAX_OTF_INTERRUPTS))
//    {
//	giPrevTint = uTint;
//	giSensorMap[giSensorCount++] = (int) uTint;
//    }
//
// otf will not use IO's at all.
//
//    uTint &= 0x07;
//    if((uTint != giPrevTint) && iOTFAligning && (giOTFCounter <= MAX_OTF_INTERRUPTS))
//    {
//        MPCollectOTFData(aiPortsMap);
	// latch R by output L 8th bit 0
////printf("giOTFCounter=%d, uTint=%d, prevTint=%d\n",giOTFCounter, uTint, giPrevTint);
//	giPrevTint = uTint;

	// just use TP to get R pos
//	rc = GASendDMCCommand(ghDMC, "TPY", ReturnBuffer, MAXGASTR);

//	iOutputL = inb(IO_PRE_OUTPUT_L);
//	iOutputL &= 0x7F;		// turn 8th bit to 0
//	outb(iOutputL, IO_PRE_OUTPUT_L); // This sends latch signal to Galil
	// now get the latch pos from galil
//	rc = GASendDMCCommand(ghDMC, caRLBcommand, ReturnBuffer, MAXGASTR);
//	lTemp = atol(ReturnBuffer);
//	glOTFData[giOTFCounter] = lTemp;
	// re-arm the latch
//	iOutputL |= 0x80;		// turn 8th bit to 1
//	outb(iOutputL, IO_PRE_OUTPUT_L); // This sends latch ready signal to Galil
//	rc = GASendDMCCommand(ghDMC, caARBcommand, ReturnBuffer, MAXGASTR);
	// store R pos in otf data buffer
//	aiPortsMap[giOTFCounter++] = (int) uTint;
//    }
//    else if ( uTint != uCustInpByte   &&  !MRGetMacroIntegerVars(MR_IIM_INTERRUPT))
    if ( uTint != uCustInpByte   &&  !MRGetMacroIntegerVars(MR_IIM_INTERRUPT))
    {
        uCustInpByte = uTint;
        bIOIntPending = MRGetMacroIntegerVars(MR_IIM_ENABLED);
        MASetMacroIntegerVars(MR_IIM_INTERRUPT, bIOIntPending);
    }

    return;
}

/************************************************
 * Function Name:   IOEnableIoTest
 *
 * Description:     Enable IOTEST interrupt.
 *
 * Parameter:       none
 * Returns:         none.
 *
 * ************************************************/
void IOEnableIoTest( int iCardArg )
{
    if (!bUseInts)
	    return;

    return;
}

/************************************************
 * Function Name:   IODisableIoTest
 *
 * Description:     Disable IOTEST interrupt.
 *
 * Parameter:       none
 * Returns:         none.
 *
 * ************************************************/
void IODisableIoTest( int iCardArg )
{
    return;
}

/************************************************
 * Function Name:   IOWriteIO
 *
 * Description:     Write to IO port.
 *
 * Parameter:       iBit    - bit number, -1 = all the bits
 *                  iData   - data to be written to IO port
 *                  iPort   - IO port number
 * Returns:         none.
 *
 * ************************************************/
void IOWriteIO(int iBitArg, unsigned char iDataArg, unsigned short int iPortArg)
{
    int iClrMask;
    int iSetMask;

    // write whole byte?
    if (iBitArg != -1)
    {
	    iClrMask = 1 << iBitArg;
	    iSetMask = (iDataArg ? iClrMask : 0);
    }

    if (!bUseInts)	// don't write to I/O unless we're on machine
	    return;

    // if turning only a bit
    if (iBitArg != -1)
	    iDataArg = ( (inb(iPortArg) & ~iClrMask) | iSetMask);

    outb(iDataArg, iPortArg);

    return;
}

/************************************************
 * Function Name:   IOGetUseInts
 *
 * Description:     Return the global flag, bUseInts.
 *
 * Parameter:       none
 * Returns:         bUseInts.
 *
 * ************************************************/
int IOGetUseInts(void)
{
    return bUseInts;
}

/************************************************
 * Function Name:   IOSetIOIntPending
 *
 * Description:     Set the global flag, bIOIntPending.
 *
 * Parameter:       bFlag   - value to set
 * Returns:         none.
 *
 * ************************************************/
void IOSetIOIntPending(int bFlagArg)
{
    bIOIntPending = bFlagArg;
}


/************************************************
 * Function Name:   IOTestIO
 *
 * Description:     IO port Test routine in diagnostic step 2.
 *                  Display Messages are routed to COM port.
 *
 * Parameter:       iDevMode    - Robot or Prealigner
 * Returns:         SUCCESS or FAILURE.
 *
 * ************************************************/
int IOTestIO( int iDevModeArg )
{
    int iCount;
    int iOutFail = 0, iInpFail = 0;
    int iPrevG, iPrevH, iPortVal;
    unsigned short int iPortBase, iPortRW;
    unsigned char iValue, iMask;
    char cPort='B', cPortMax, cKeyPressed;
    char cPortsModified[22];

    iPortBase = iDevModeArg ? IO_PRE_INPUT_K : IO_ROBOT_INPUT_F;
    if( iDevModeArg )
    {
        cPortMax = 'C';
        strcpy( cPortsModified, "Ports A B C OK      " );
    }
    else
    {
        cPortMax = 'E';
        strcpy( cPortsModified, "Ports A B C D E OK  " );
    }

    iPrevH = iInputH = inb( IO_ROBOT_INPUT_H );

    // Send display message strings to COM port
    TTPrintsAt( 2, 1, "Testing Port        " );
    TTPrintsAt( 3, 1, "                    " );
//    srand( (unsigned)time(NULL) );
    TIDelay(500);

    // Test Port A to E
    for ( cPort = 'A'; cPort <= cPortMax; cPort++ )
    {
        TTPrintcAt( 3, 3+(2*(cPort-'A')), cPort );
        switch ( cPort )
        {
            case 'A':
                iMask = 0x0F;
                iPortRW = IO_ROBOT_OUTPUT_A;
                break;
            case 'B':
                iMask = 0x3F;
                iPortRW = IO_ROBOT_OUTPUT_B;
                break;
            case 'C':
                iMask = 0x0F;
                iPortRW = IO_ROBOT_OUTPUT_A;
                break;
            case 'D':
                iMask = 0xFF;
                iPortRW = IO_ROBOT_OUTPUT_D;
                break;
            case 'E':
                iMask = 0xFF;
                iPortRW = IO_ROBOT_OUTPUT_D;
                break;
        } // switch on cPort (A to E)

        // Test each port: iPortRW
        // Write a value to port and read it back
        // If Write/Read value are not same, set the Fail bit
        iPrevG = inb( iPortRW );
        for( iCount = 0; iCount < 128; iCount++ )
        {
            iValue = rand( ) % 256;
            iValue &= iMask;
            IOWriteIO( -1, iValue, iPortRW );
            iPortVal = inb( iPortRW ) & iMask;
            if( iValue != iPortVal )
            {
            	iOutFail |= 1 << (cPort - 'A');
                break;
            }
        } // for iCount on each port
        // Just write back the saved value to the port
        IOWriteIO(-1, iPrevG, iPortRW );
        TIDelay( 500 );
    } // for cPort (A to Max port)

    // Now, iOutFail contatins the bit string of test result
    // Display the results
    iCount = 0;
    TTPrintsAt( 2, 1, "Port                " );
    TTPrintsAt( 3, 1, "                    " );

	iOutFail = 0;
    while ( iOutFail )
    {
        if ( iOutFail & 1 )
            TTPrintcAt( 3, 1+2*iCount, (char)('A'+iCount) );
        iCount++;
        if ( iOutFail & 1 )
            TTPrintcAt( 3, 2*iCount, ',' );
        iOutFail >>= 1;
    } // while iOutFail
    // Display the overall port test result message
    // iCount is the number of failed ports
    if ( iCount )
        TTPrintsAt( 3, 11, " Failed" );
    else
        TTPrintsAt( 2, 1, cPortsModified );
    TIDelay(500);


    // Port G test
    // Set teach pendant mode to respond when Key is released
    // During this test, the controller internal cables (i.e., robot signal cable)
    // may be disconnected one at a time to verify the status of port G
    TTSetKNPMode(FALSE);
    IOWriteIO( -1, 0xFF, IO_ROBOT_OUTPUT_B );
    TIDelay( 1000 );

    TTClearScreen( );
    TTPrintsAt( 1, 1, "  Testing input G" );
    TTPrintsAt( 2, 1, "       VALUE:" );
    TTPrintsAt( 4, 1, "Generate Error" );
    iInputG = inb( IO_ROBOT_INPUT_G );
    iPrevG = !iInputG;
    //IOEnableIoTest(iDevModeArg);
    do
    {
	iInputG = inb( IO_ROBOT_INPUT_G );
        if ( iPrevG != iInputG )
        {
            iPrevG = iInputG;

            for ( iCount = 0; iCount < 8; iCount++ )
                TTPrintcAt( 3, 7+iCount, (char)((iPrevG & (1<<iCount)) ? '1' : '0'));

//	    iInputG = inb( IO_ROBOT_INPUT_G );
//            IOWriteIO(-1, 0xFF, IO_ROBOT_OUTPUT_B );
        } // if iPrevG != iInputG
        SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
    }
    while( cKeyPressed != CR );

    // External Input and Output port test
    if( iDevModeArg )
    {   // Prealigner test (No external customer port exists)
//        IODisableIoTest(iDevModeArg);
        IOWriteIO( -1, 0x01, IO_ROBOT_OUTPUT_B );
        TIDelay( 500 );
    }
    else
    {   // Robot External Input port test
        // During this test, external switches/sensors (ie, EMO & STOP) may be
        // turned ON/OFF to verify its status
        TTClearScreen( );
        TTPrintsAt( 1, 1, "Testing Ext. Inp." );
        TTPrintsAt( 2, 1, "Press Switches" );
        TTPrintsAt( 3, 1, "IO Read:" );

//        IOWriteIO( -1, 0, iPortBase+7 );
        iInputH = inb( IO_ROBOT_INPUT_H );
        iPrevH = !iInputH;
        iInputG = inb( IO_ROBOT_INPUT_G );
        iPrevG = !iInputG;
        do
        {
	    iInputH = inb( IO_ROBOT_INPUT_H );
            if ( iPrevH != iInputH )
            {
                iPrevH = iInputH;
                for ( iCount = 0; iCount < 8; iCount++ )
                    TTPrintcAt( 3, 9+iCount, (char)((iPrevH&(1<<iCount)) ? '1' : '0'));
            }
	    iInputG = inb( IO_ROBOT_INPUT_G );
            if ( iPrevG != iInputG )
            {
                iPrevG = iInputG;
                if ( iPrevG&2 )
                    TTPrintsAt( 4,1, "INTLK ON " );
                else
                    TTPrintsAt( 4,1, "INTLK OFF" );
                if ( iPrevG&4 )
                    TTPrintsAt( 4,12, "STOP ON " );
                else
                    TTPrintsAt( 4,12, "STOP OFF" );
            }
            SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
        }
        while( cKeyPressed != CR );

        //IODisableIoTest(iDevModeArg);
//        IOWriteIO( -1, 0xFF, iPortBase+7 );
        IOWriteIO( -1, 0x01, IO_ROBOT_OUTPUT_B );
        TIDelay( 500 );

        // Test External Output ports
        // This test allows to toggle the output to the port using
        // numeric key pad on Teach pendant.
        TTClearScreen( );
        TTPrintsAt( 1,1, "Testing Ext. Out." );
        TTPrintsAt( 2,1, "Enter Bit# to togl" );

        iInpFail = inb( IO_ROBOT_OUTPUT_D);
        TTPrintsAt( 3,1, "IO Read:" );
        for ( iCount = 0; iCount < 8; iCount++ )
            TTPrintcAt( 3, 9+iCount, (char)((iInpFail&(1<<iCount)) ? '1' : '0'));
        do
        {
            SERGetcRxBuff( SERGetTTPort(), &cKeyPressed, FALSE );
            if( cKeyPressed != EOF )
            {
                if( cKeyPressed >= '0' && cKeyPressed <= '7' )
                {
                    iCount = (int)(cKeyPressed-'0');
                    iInpFail = (iInpFail&(1<<iCount))?
                    (iInpFail&~(1<<iCount)):(iInpFail|(1<<iCount));
                    IOWriteIO( -1, iInpFail, IO_ROBOT_OUTPUT_D );
                    TTPrintcAt( 3, 9+iCount, (char)((iInpFail&(1<<iCount))?'1':'0'));
                }
                else if( cKeyPressed != CR )
                    TTPrintMessage(ALERT, "Invalid Key");
            }
        }
        while( cKeyPressed != CR );
    }

    TTSetKNPMode(TRUE);


    return SUCCESS;
}


//
// Read STN IO data from a file
//
int IOReadSTNIO()
{
    FILE *iFP;
    int iTotalInt;

    iFP = fopen(SIOFILENAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "Stn IO file Read Open Error " );
    }
    else
    {
        iTotalInt = fread( glTimeIO, sizeof( long ), 4000, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Stn IO file Read Error " );
	}
        iTotalInt = fread( giTimeIOCounter, sizeof( int ), 40, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Stn IO file Read Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// RC Data Write to a file
int IOWriteSTNIO()
{
    FILE *iFP;
    int iTotalInt;
    
    iFP = fopen( SIOFILENAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Stn IO file Write Open Error " );
    }
    else
    {
        iTotalInt = fwrite( glTimeIO, sizeof( long ), 4000, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Stn IO file 1 write Error " );
	}
        iTotalInt = fwrite( giTimeIOCounter, sizeof( int ), 40, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Stn IO file 1 write Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}
