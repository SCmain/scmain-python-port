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
 * Program:     Robot mechanism module
 * File:        ro.c
 * Functions:   A lot of functions assimilated from
 *              execute, execute2, execute3, execmd, and galil. These
 *              include general mechanism functions, motion functions
 *              (which actually should be in its own memotion.h header),
 *              and general purpose utility functions.
 *
 *              The following are local functions which are in this source file:
 *              ROGetMappedAxis
 *              ROAxisReadyToMove
 *              ROScaleSpd
 *              ROUnscaleSpd
 *              ROScalePos
 *              ROUnscalePos
 *              ROStopMotionLocal
 *              ROServoMCOffLocal
 *              ROServoMCOnLocal
 *
 * Description: Provide an interface between other modules and the Mechanism
 *      module
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: A possible problem in the system is that the Galil axis
 *      variable and file type variable are global. They can change
 *      suddenly because of the action buffer and mess up an
 *      operation in process.
 *
\***************************************************************/
#include <sys/io.h>

<<<<<<< HEAD
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
=======
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include <math.h>
#include <string.h>
#include "sck.h"
#include "ro.h"
#include "roga.h"
#include "rofn.h"
#include "rofio.h"
#include "roloc.h"
#include "fiog.h"
#include "gag.h"
#include "gaintr.h"
#include "gamn.h"
#include "ser.h"
#include "scstat.h"
#include "mapio.h"
#include "sctim.h"
#include "scintr.h"
#include "scmac.h"
#include "scio.h"
<<<<<<< HEAD
#include "otf.h"
#include "fiol.h"
#include "dmclnx.h"
#include "pdio.h"
=======
#include "otf.h"
#include "fiol.h"
#include "dmclnx.h"
#include "pdio.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//#include "serl.h"

/********** VARIABLES USED LOCALLY (within Mechanism module) and EXTERNED **********/

unsigned long ulAxisALLRbt = 0; /* The axis #Define combination for a 3-axis robot only */
unsigned long ulAxisallPre = 0; /* The axis #Define combination for a pre-aligner only */
unsigned long ulAllAMFlag = 0;  /* The axis combination for all available axes */
unsigned long ulHoming = 0;     /* 1 = in the process of homing; bitwise by axis */
unsigned long ulHomed = 0;      /* 1 = axis has previously homed; bitwise by axis */
unsigned long ulServoFlag = 0xFF;   /* 1 = servo off; 0 = servo on; bitwise by axis */
unsigned long ulERFlag = 0;     /* 1 = position error on axis; bitwise by axis */
<<<<<<< HEAD
unsigned long ulAMFlag = 0xFF;  /* 1 = motion completed on axis; bitwise by axis */
=======
unsigned long ulAMFlag = 0xFF;  /* 1 = motion completed on axis; bitwise by axis */
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
unsigned long ulAMIPFlag = 0xFF;/* 1 = motion complete by InPosition */
unsigned long ulLMPosFlag = 0;  /* 1 = pos limit sw now on; bitwise by axis */
unsigned long ulLMNegFlag = 0;  /* 1 = neg limit sw now on; bitwise by axis */
unsigned long ulLMPosMom = 0;   /* 1 = pos limit sw momentarily on; bitwise by axis */
unsigned long ulLMNegMom = 0;   /* 1 = neg limit sw momemtarily on; bitwise by axis */
unsigned long ulLMPosChanged = 0;   /* 1 = positive limit switch value has changed; bitwise by axis */
unsigned long ulLMNegChanged = 0;   /* 1 = negative limit switch value has changed; bitwise by axis */
unsigned uSCurveFlag = 0;       /* 1 = S-curve profile on; bitwise by axis */
int iDefineFlag = 0;            /* The define flags for the current system configuration */
int iEmulator = 0;              /* The emulation type of the current system configuration */
int iScanCardNum = 1;           /* The scanner card number is saved to be used in QuitScan */
int iPreOnly = TRUE;            /* Special considerations for a pre-aligner only system */

BOOL bCommandedGearingMode = TRUE;  // variable holds the current setting for the type of gearing for VAC514 robots
                                    // used to be FALSE, now defaults to TRUE. CK:12122000 

/********** VARIABLES USED LOCALLY (within this source file ONLY) **********/

int iNumAxes = 0;               /* The total number of axes in the system. */
unsigned uaAxisMap[8][4];       /* The local axis mapping matrix copied during initialization. */
int iNumGalilCards = 1;         /* The total number of Galil cards in the system, either 1 or 2. */
<<<<<<< HEAD

extern int giNumOfAxes;

=======

extern int giNumOfAxes;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
unsigned uAbortAxis = 0;
//int iPowerDisconnected = TRUE;
int iPowerDisconnected = FALSE;
int iEMOWasSet = FALSE;
//double dMechRatioVAC514 = 13563.36805556; CK:06022000 Steve's request for new constant
double dMechRatioVAC514 = 21541.81985294;

<<<<<<< HEAD
char cpNull[8];

extern HANDLEDMC ghDMC;
extern CONTROLLERINFO gControllerInfo;

void GAGalilWriteIO(int, int);

extern int giVersionPA;
extern int giSysCfgNum;
extern int giMapTest;
extern int giMapIORunning;

int giTimerGA;

int giIPW[8];
int giIPWenc[8];
int giIPWFlag[8];
int giIPWDirection[8];
int giIPWDestination[8];
char TScommand[4] = "TS";
char TPcommand[4] = "TP";
char HXcommand[4] = "HX";
char HX2command[10] = "VMTC=0";
char HOMDcommand[10] = "MG VHMD";
char HOMGcommand[10] = "MG VHMG";

// LATCH related Galil commands
char caALAcommand[10] = "MG _ALA";
char caALBcommand[10] = "MG _ALB";
char caALCcommand[10] = "MG _ALC";
char caALDcommand[10] = "MG _ALD";
char caALEcommand[10] = "MG _ALE";
char caALFcommand[10] = "MG _ALF";
char caALGcommand[10] = "MG _ALG";
char caALHcommand[10] = "MG _ALH";
char caRLAcommand[10] = "MG _RLA";
char caRLBcommand[10] = "MG _RLB";
char caRLCcommand[10] = "MG _RLC";
char caRLDcommand[10] = "MG _RLD";
char caRLEcommand[10] = "MG _RLE";
char caRLFcommand[10] = "MG _RLF";
char caRLGcommand[10] = "MG _RLG";
char caRLHcommand[10] = "MG _RLH";
char caTPAcommand[5] = "TPA";
char caTPBcommand[5] = "TPB";
char caTPCcommand[5] = "TPC";
char caTPDcommand[5] = "TPD";
char caTPEcommand[5] = "TPE";
char caTPFcommand[5] = "TPF";
char caTPGcommand[5] = "TPG";
char caTPHcommand[5] = "TPH";
char caARAcommand[5] = "ALA";
char caARBcommand[5] = "ALB";
char caARCcommand[5] = "ALC";
char caARDcommand[5] = "ALD";
char caAREcommand[5] = "ALE";
char caARFcommand[5] = "ALF";
char caARGcommand[5] = "ALG";
char caARHcommand[5] = "ALH";

char caSTcommand[5] = "ST"; // to stop all axes motion

extern long glPDCalTable[8][2][5]; // 0(min), 1(max), 2(ave), 3(user), 4(notused)
extern long glLatchData[8][2][500];
extern long glLatchDataEU[8][2][500];
extern long glLatchPtr [8][2][2];

int giTPCount = 0;
int giTSCount = 0;
int giLACount = 0;
int giROCount = 0;

int giTOTFlag;

extern int iOTFFirstTime;
extern int giOTFNullCount;
extern int iOTFAligning;

int giEncoderDriftFlag = 0;
int giEncoderDriftDetector = 0;
int giDisableLatch = 1;
int giEncoderAlarm = 0;

int giPrevERFlag[8] = {0,0,0,0,0,0,0,0};
int giPrevPLFlag[8] = {0,0,0,0,0,0,0,0};
int giPrevNLFlag[8] = {0,0,0,0,0,0,0,0};

long glTSArray[8];
char glTSReturnBuffer[MAXGASTR];

extern int iERRInterrupt;
extern int iLMMInterrupt;
extern int iDERInterrupt;

extern unsigned char iInputG;

void RODisableLatch(int iFlagArg)
{
    int rc;
    char caResp[MAXGASTR];

    if(iFlagArg) // Disable the latch for drift detection
    {
	// This will latch all armed axes.
	// Thus, all axes are now un-armed.
	rc = GASendDMCCommand(ghDMC, "CN,,1", caResp, MAXGASTR);
	rc = GASendDMCCommand(ghDMC, "CN,,-1", caResp, MAXGASTR);
	giDisableLatch = 1;
    }
    else	// Enable the latch for drifit detection
    {
	rc = GASendDMCCommand(ghDMC, "CN,,-1", caResp, MAXGASTR);
	rc = GASendDMCCommand(ghDMC, "CN,,1", caResp, MAXGASTR);
	if(giNumOfAxes <= 4)
	    rc = GASendDMCCommand(ghDMC, "ALABCD", caResp, MAXGASTR);
	else
	    rc = GASendDMCCommand(ghDMC, "ALABCDEFGH", caResp, MAXGASTR);
	giDisableLatch = 0;
    }

}

void ConvertStrToL(char* strBuf, int iNumAxesArg, long* lTSArray)
{
    int i, j, k;
    int iDone;
    char aNum[40];

    for(i=0;i<8; i++) lTSArray[i]=0;

    i=j=k=iDone=0;
    while (!iDone)
    {
	aNum[i] = strBuf[k];
	if ((aNum[i] == ',') || (strBuf[k] == NULL))
	{
	    aNum[i] = '\0';
	    if(i>0)
	    {
		lTSArray[j] = atol(aNum);
	    }
	    else
	    {
		lTSArray[j] = 0;
	    }
	    j++;
	    i=0;
	    if (j >= iNumAxesArg || strBuf[k] == NULL) iDone = 1;
	    k++;
	}
	else
	{
	    i++; 
	    k++;
	}
    }
}
  
int ConvertStrToi(char* strBufArg, int iMaxArg, int* iArrayArg)
{
    int i, j, iTemp;
    int iDone;
    char *aNum;

    i = 0;
    iArrayArg[i++] = atoi( strtok(strBufArg, ",") );

    iDone = 0;
    while (!iDone)
    {
	aNum = strtok(NULL, ",");
	if (aNum == NULL)
	    iArrayArg[i++] = 0;
	else
	    iArrayArg[i++] = atoi( aNum );

	if (i >= iMaxArg)
	    iDone = 1;
    }

    return i;

}
    
int ROActionStatus(void)
{
    int rc, iTemp;
    char caResp[MAXGASTR];
    char caTBcommand[5] = "TB\xD";

    rc = GASendDMCCommand(ghDMC, caTBcommand, caResp, MAXGASTR);

    iTemp = atoi(caResp);
    iTemp &= 0x80;
    if (iTemp) return 1;
    return 0;
}


void ROUpdateTP(int iFlagArg)
{
    int i, rc;
    long lTemp, lTPArray[8];
    char ReturnBuffer[MAXGASTR];

    // Make sure that ROUpdateTP(FALSE) gets Executed no matter!!!
    //
    if(iFlagArg && iOTFFirstTime) return; // don't bother Galil during OTF

    if(iFlagArg && (++giTPCount < 3)) return;// iFlagArg 0 will update no matter!!!
    giTPCount = 0;

//    ulAMIPFlag = ulAMFlag;

    rc = GASendDMCCommand(ghDMC, TPcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROUpdateTP DMC failure=%d\n",rc);
	return;
    }

    lTemp = strlen(ReturnBuffer);
    ReturnBuffer[lTemp++] = ',';
    ReturnBuffer[lTemp] = '\0';

    ConvertStrToL(ReturnBuffer, 8, lTPArray);

    for (i = 0; i < 4; i++)
    {
	if (giIPWFlag[i])
	{
	    // check if In Position is set if in motion
//	    if (ulAMFlag & ~uaAxisMap[i][EQUIPE_AXIS] == 0)
//	    ulAMIPFlag = ulAMFlag;
	    // if axis is in motion, check its position 
	    // if position is within IPW, set IPW motion complete
	    // otherwise, set it to not complete.
	    if ((ulAMFlag & (1<<i)) != (1<<i))
	    {
		if (giIPWDirection[i] == 1)
		{   // positive direction
		    if (giIPWDestination[i] <= lTPArray[i])
		    {
			if((ulAMIPFlag & (1<<i)) != (1<<i))
			{
			    ulAMIPFlag |= uaAxisMap[i][EQUIPE_AXIS];
//printf("pos dir dest=%d curTP=%d axis=%d flag=%d\n",giIPWDestination[i], lTPArray[i], i, ulAMIPFlag);
			}
		    }
		    else
		    {
			ulAMIPFlag &= ~uaAxisMap[i][EQUIPE_AXIS];
		    }

		}
		else
		{   // negative direction
		    if (giIPWDestination [i] >= lTPArray[i])
		    {
			if((ulAMIPFlag & (1<<i)) != (1<<i))
			{
			    ulAMIPFlag |= uaAxisMap[i][EQUIPE_AXIS];
//printf("neg dir dest=%d curTP=%d axis=%d flag=%d\n",giIPWDestination[i], lTPArray[i], i, ulAMIPFlag);
			}
		    }
		    else
		    {
			ulAMIPFlag &= ~uaAxisMap[i][EQUIPE_AXIS];
		    }
		}
	    }
	}
    }
}


void ROTurnLight(int iLightNoArg, int iFlagArg)
{
    int iDat;

    switch(iLightNoArg)
    {
	case 0:
	    iDat = inb(IO_PRE_OUTPUT_L);
	    if(iFlagArg == 1)
		iDat |= 0x80;
	    else
		iDat &= 0x7F;
	    outb(iDat, IO_PRE_OUTPUT_L);
	    break;

	case 1:
 	    iDat = inb(IO_PRE_OUTPUT_B);
	    if(iFlagArg == 1)
		iDat |= 0x40;
	    else
		iDat &= 0xBF;
	    outb(iDat, IO_PRE_OUTPUT_B);
	    break;
    }
  
    return;
}

void RORaiseAlarm(unsigned int uAlarmBitArg)
{
    int iDat;
    int iFP;
    int iCount;
    int rc;
    char ReturnBuffer[MAXGASTR];

    // Save this portion for later use
    giEncoderAlarm |= uAlarmBitArg;
    ROEmergencyOff(FALSE);
    iDat = inb(IO_PRE_OUTPUT_L);
    iDat |= 0x80;
    outb(iDat, IO_PRE_OUTPUT_L);
    rc = GASendDMCCommand(ghDMC, caSTcommand, ReturnBuffer, MAXGASTR);
    
    MASetMacroIntegerVars(MR_DER_INTERRUPT, TRUE);
=======
char cpNull[8];

extern HANDLEDMC ghDMC;
extern CONTROLLERINFO gControllerInfo;

void GAGalilWriteIO(int, int);

extern int giVersionPA;
extern int giSysCfgNum;
extern int giMapTest;
extern int giMapIORunning;

int giTimerGA;

int giIPW[8];
int giIPWenc[8];
int giIPWFlag[8];
int giIPWDirection[8];
int giIPWDestination[8];
char TScommand[4] = "TS";
char TPcommand[4] = "TP";
char HXcommand[4] = "HX";
char HX2command[10] = "VMTC=0";
char HOMDcommand[10] = "MG VHMD";
char HOMGcommand[10] = "MG VHMG";

// LATCH related Galil commands
char caALAcommand[10] = "MG _ALA";
char caALBcommand[10] = "MG _ALB";
char caALCcommand[10] = "MG _ALC";
char caALDcommand[10] = "MG _ALD";
char caALEcommand[10] = "MG _ALE";
char caALFcommand[10] = "MG _ALF";
char caALGcommand[10] = "MG _ALG";
char caALHcommand[10] = "MG _ALH";
char caRLAcommand[10] = "MG _RLA";
char caRLBcommand[10] = "MG _RLB";
char caRLCcommand[10] = "MG _RLC";
char caRLDcommand[10] = "MG _RLD";
char caRLEcommand[10] = "MG _RLE";
char caRLFcommand[10] = "MG _RLF";
char caRLGcommand[10] = "MG _RLG";
char caRLHcommand[10] = "MG _RLH";
char caTPAcommand[5] = "TPA";
char caTPBcommand[5] = "TPB";
char caTPCcommand[5] = "TPC";
char caTPDcommand[5] = "TPD";
char caTPEcommand[5] = "TPE";
char caTPFcommand[5] = "TPF";
char caTPGcommand[5] = "TPG";
char caTPHcommand[5] = "TPH";
char caARAcommand[5] = "ALA";
char caARBcommand[5] = "ALB";
char caARCcommand[5] = "ALC";
char caARDcommand[5] = "ALD";
char caAREcommand[5] = "ALE";
char caARFcommand[5] = "ALF";
char caARGcommand[5] = "ALG";
char caARHcommand[5] = "ALH";

char caSTcommand[5] = "ST"; // to stop all axes motion

extern long glPDCalTable[8][2][5]; // 0(min), 1(max), 2(ave), 3(user), 4(notused)
extern long glLatchData[8][2][500];
extern long glLatchDataEU[8][2][500];
extern long glLatchPtr [8][2][2];

int giTPCount = 0;
int giTSCount = 0;
int giLACount = 0;
int giROCount = 0;

int giTOTFlag;

extern int iOTFFirstTime;
extern int giOTFNullCount;
extern int iOTFAligning;

int giEncoderDriftFlag = 0;
int giEncoderDriftDetector = 0;
int giDisableLatch = 1;
int giEncoderAlarm = 0;

int giPrevERFlag[8] = {0,0,0,0,0,0,0,0};
int giPrevPLFlag[8] = {0,0,0,0,0,0,0,0};
int giPrevNLFlag[8] = {0,0,0,0,0,0,0,0};

long glTSArray[8];
char glTSReturnBuffer[MAXGASTR];

extern int iERRInterrupt;
extern int iLMMInterrupt;
extern int iDERInterrupt;

extern unsigned char iInputG;

void RODisableLatch(int iFlagArg)
{
    int rc;
    char caResp[MAXGASTR];

    if(iFlagArg) // Disable the latch for drift detection
    {
	// This will latch all armed axes.
	// Thus, all axes are now un-armed.
	rc = GASendDMCCommand(ghDMC, "CN,,1", caResp, MAXGASTR);
	rc = GASendDMCCommand(ghDMC, "CN,,-1", caResp, MAXGASTR);
	giDisableLatch = 1;
    }
    else	// Enable the latch for drifit detection
    {
	rc = GASendDMCCommand(ghDMC, "CN,,-1", caResp, MAXGASTR);
	rc = GASendDMCCommand(ghDMC, "CN,,1", caResp, MAXGASTR);
	if(giNumOfAxes <= 4)
	    rc = GASendDMCCommand(ghDMC, "ALABCD", caResp, MAXGASTR);
	else
	    rc = GASendDMCCommand(ghDMC, "ALABCDEFGH", caResp, MAXGASTR);
	giDisableLatch = 0;
    }

}

void ConvertStrToL(char* strBuf, int iNumAxesArg, long* lTSArray)
{
    int i, j, k;
    int iDone;
    char aNum[40];

    for(i=0;i<8; i++) lTSArray[i]=0;

    i=j=k=iDone=0;
    while (!iDone)
    {
	aNum[i] = strBuf[k];
	if ((aNum[i] == ',') || (strBuf[k] == NULL))
	{
	    aNum[i] = '\0';
	    if(i>0)
	    {
		lTSArray[j] = atol(aNum);
	    }
	    else
	    {
		lTSArray[j] = 0;
	    }
	    j++;
	    i=0;
	    if (j >= iNumAxesArg || strBuf[k] == NULL) iDone = 1;
	    k++;
	}
	else
	{
	    i++; 
	    k++;
	}
    }
}
  
int ConvertStrToi(char* strBufArg, int iMaxArg, int* iArrayArg)
{
    int i, j, iTemp;
    int iDone;
    char *aNum;

    i = 0;
    iArrayArg[i++] = atoi( strtok(strBufArg, ",") );

    iDone = 0;
    while (!iDone)
    {
	aNum = strtok(NULL, ",");
	if (aNum == NULL)
	    iArrayArg[i++] = 0;
	else
	    iArrayArg[i++] = atoi( aNum );

	if (i >= iMaxArg)
	    iDone = 1;
    }

    return i;

}
    
int ROActionStatus(void)
{
    int rc, iTemp;
    char caResp[MAXGASTR];
    char caTBcommand[5] = "TB\xD";

    rc = GASendDMCCommand(ghDMC, caTBcommand, caResp, MAXGASTR);

    iTemp = atoi(caResp);
    iTemp &= 0x80;
    if (iTemp) return 1;
    return 0;
}


void ROUpdateTP(int iFlagArg)
{
    int i, rc;
    long lTemp, lTPArray[8];
    char ReturnBuffer[MAXGASTR];

    // Make sure that ROUpdateTP(FALSE) gets Executed no matter!!!
    //
    if(iFlagArg && iOTFFirstTime) return; // don't bother Galil during OTF

    if(iFlagArg && (++giTPCount < 3)) return;// iFlagArg 0 will update no matter!!!
    giTPCount = 0;

//    ulAMIPFlag = ulAMFlag;

    rc = GASendDMCCommand(ghDMC, TPcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROUpdateTP DMC failure=%d\n",rc);
	return;
    }

    lTemp = strlen(ReturnBuffer);
    ReturnBuffer[lTemp++] = ',';
    ReturnBuffer[lTemp] = '\0';

    ConvertStrToL(ReturnBuffer, 8, lTPArray);

    for (i = 0; i < 4; i++)
    {
	if (giIPWFlag[i])
	{
	    // check if In Position is set if in motion
//	    if (ulAMFlag & ~uaAxisMap[i][EQUIPE_AXIS] == 0)
//	    ulAMIPFlag = ulAMFlag;
	    // if axis is in motion, check its position 
	    // if position is within IPW, set IPW motion complete
	    // otherwise, set it to not complete.
	    if ((ulAMFlag & (1<<i)) != (1<<i))
	    {
		if (giIPWDirection[i] == 1)
		{   // positive direction
		    if (giIPWDestination[i] <= lTPArray[i])
		    {
			if((ulAMIPFlag & (1<<i)) != (1<<i))
			{
			    ulAMIPFlag |= uaAxisMap[i][EQUIPE_AXIS];
//printf("pos dir dest=%d curTP=%d axis=%d flag=%d\n",giIPWDestination[i], lTPArray[i], i, ulAMIPFlag);
			}
		    }
		    else
		    {
			ulAMIPFlag &= ~uaAxisMap[i][EQUIPE_AXIS];
		    }

		}
		else
		{   // negative direction
		    if (giIPWDestination [i] >= lTPArray[i])
		    {
			if((ulAMIPFlag & (1<<i)) != (1<<i))
			{
			    ulAMIPFlag |= uaAxisMap[i][EQUIPE_AXIS];
//printf("neg dir dest=%d curTP=%d axis=%d flag=%d\n",giIPWDestination[i], lTPArray[i], i, ulAMIPFlag);
			}
		    }
		    else
		    {
			ulAMIPFlag &= ~uaAxisMap[i][EQUIPE_AXIS];
		    }
		}
	    }
	}
    }
}


void ROTurnLight(int iLightNoArg, int iFlagArg)
{
    int iDat;

    switch(iLightNoArg)
    {
	case 0:
	    iDat = inb(IO_PRE_OUTPUT_L);
	    if(iFlagArg == 1)
		iDat |= 0x80;
	    else
		iDat &= 0x7F;
	    outb(iDat, IO_PRE_OUTPUT_L);
	    break;

	case 1:
 	    iDat = inb(IO_PRE_OUTPUT_B);
	    if(iFlagArg == 1)
		iDat |= 0x40;
	    else
		iDat &= 0xBF;
	    outb(iDat, IO_PRE_OUTPUT_B);
	    break;
    }
  
    return;
}

void RORaiseAlarm(unsigned int uAlarmBitArg)
{
    int iDat;
    int iFP;
    int iCount;
    int rc;
    char ReturnBuffer[MAXGASTR];

    // Save this portion for later use
    giEncoderAlarm |= uAlarmBitArg;
    ROEmergencyOff(FALSE);
    iDat = inb(IO_PRE_OUTPUT_L);
    iDat |= 0x80;
    outb(iDat, IO_PRE_OUTPUT_L);
    rc = GASendDMCCommand(ghDMC, caSTcommand, ReturnBuffer, MAXGASTR);
    
    MASetMacroIntegerVars(MR_DER_INTERRUPT, TRUE);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    iFP = fopen( ALFILENAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Alarm File Open Error " );
    }
    else
<<<<<<< HEAD
    {
        iCount = fwrite(&giEncoderAlarm, sizeof(int), 1, iFP);
        if (iCount <= 0) 
=======
    {
        iCount = fwrite(&giEncoderAlarm, sizeof(int), 1, iFP);
        if (iCount <= 0) 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        {
            perror( "Alarm File Write Error " );
        }
        
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
<<<<<<< HEAD
    }
}

void ROClearAlarm( void )
{
    int iDat;
    FILE *iFP;
    int iCount;

    giEncoderAlarm = 0;
    iDat = inb(IO_PRE_OUTPUT_L);
    iDat &= 0x7F;
    outb(iDat, IO_PRE_OUTPUT_L);
=======
    }
}

void ROClearAlarm( void )
{
    int iDat;
    FILE *iFP;
    int iCount;

    giEncoderAlarm = 0;
    iDat = inb(IO_PRE_OUTPUT_L);
    iDat &= 0x7F;
    outb(iDat, IO_PRE_OUTPUT_L);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    
    iFP = fopen( ALFILENAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Alarm File Open Error " );
    }
    else
<<<<<<< HEAD
    {
        iCount = fwrite(&giEncoderAlarm, sizeof(int), 1, iFP);
        if (iCount <= 0) 
=======
    {
        iCount = fwrite(&giEncoderAlarm, sizeof(int), 1, iFP);
        if (iCount <= 0) 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        {
            perror( "Alarm File Write Error " );
        }
        
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
<<<<<<< HEAD
    }
}

void ROCheckLatch(int iFlagArg)
{
    int iLatch, rc, iDat;
    long lTemp, lTP;
    char ReturnBuffer[MAXGASTR];
    FILE *iFP;
    int iCount;

    if(giDisableLatch) return;

    if(giSysCfgNum == 30) // 30 = I2AXO special 
    {
	if ((~ulHomed) & 0x0F) return;
    }
    else
    {
    	if ((~ulHomed) & 0xFF) return;
    }

    if(iFlagArg && iOTFAligning) return; // don't bother Galil during OTF

    if(iFlagArg && (++giLACount < 23)) return;
    giLACount = 0;

    // axis A
    rc = GASendDMCCommand(ghDMC, caALAcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch A DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch) // Active Low
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLAcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPAcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[0][0][glLatchPtr[0][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_T, lTemp, &glLatchDataEU[0][0][glLatchPtr[0][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(0, 0, glLatchPtr[0][0][0]))
	    {
		RORaiseAlarm(1);
	    }
	    glLatchPtr[0][0][0]++; // increment current ptr
	    glLatchPtr[0][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[0][0][0] > 499)
		glLatchPtr[0][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[0][1][glLatchPtr[0][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_T, lTemp, &glLatchDataEU[0][1][glLatchPtr[0][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(0, 1, glLatchPtr[0][1][0]))
	    {
		RORaiseAlarm(1);
	    }
	    glLatchPtr[0][1][0]++; // increment current ptr
	    glLatchPtr[0][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[0][1][0] > 499)
		glLatchPtr[0][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARAcommand, ReturnBuffer, MAXGASTR);
    }

    // axis B
// COMMENTED OUT FOR OTF TEST 110104   
    rc = GASendDMCCommand(ghDMC, caALBcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch B DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLBcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPBcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[1][0][glLatchPtr[1][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_R, lTemp, &glLatchDataEU[1][0][glLatchPtr[1][0][0]]);
//printf("Rlatch: enc=%d eu=%d ptr=%d\n",lTemp,glLatchDataEU[1][0][glLatchPtr[1][0][0]],glLatchPtr[1][0][0]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(1, 0, glLatchPtr[1][0][0]))
	    {
		RORaiseAlarm(2);
	    }
	    glLatchPtr[1][0][0]++; // increment current ptr
	    glLatchPtr[1][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[1][0][0] > 499)
		glLatchPtr[1][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[1][1][glLatchPtr[1][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_R, lTemp, &glLatchDataEU[1][1][glLatchPtr[1][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(1, 1, glLatchPtr[1][1][0]))
	    {
		RORaiseAlarm(2);
	    }
	    glLatchPtr[1][1][0]++; // increment current ptr
	    glLatchPtr[1][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[1][1][0] > 499)
		glLatchPtr[1][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARBcommand, ReturnBuffer, MAXGASTR);
    }

    // axis C
    rc = GASendDMCCommand(ghDMC, caALCcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch C DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLCcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPCcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[2][0][glLatchPtr[2][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_Z, lTemp, &glLatchDataEU[2][0][glLatchPtr[2][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(2, 0, glLatchPtr[2][0][0]))
	    {
		RORaiseAlarm(4);
	    }
	    glLatchPtr[2][0][0]++; // increment current ptr
	    glLatchPtr[2][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[2][0][0] > 499)
		glLatchPtr[2][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[2][1][glLatchPtr[2][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_Z, lTemp, &glLatchDataEU[2][1][glLatchPtr[2][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(2, 1, glLatchPtr[2][1][0]))
	    {
		RORaiseAlarm(4);
	    }
	    glLatchPtr[2][1][0]++; // increment current ptr
	    glLatchPtr[2][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[2][1][0] > 499)
		glLatchPtr[2][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARCcommand, ReturnBuffer, MAXGASTR);
    }

    // axis D
    rc = GASendDMCCommand(ghDMC, caALDcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch D DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLDcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPDcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[3][0][glLatchPtr[3][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_W, lTemp, &glLatchDataEU[3][0][glLatchPtr[3][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(3, 0, glLatchPtr[3][0][0]))
	    {
		RORaiseAlarm(8);
	    }
	    glLatchPtr[3][0][0]++; // increment current ptr
	    glLatchPtr[3][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[3][0][0] > 499)
		glLatchPtr[3][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[3][1][glLatchPtr[3][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_W, lTemp, &glLatchDataEU[3][1][glLatchPtr[3][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(3, 1, glLatchPtr[3][1][0]))
	    {
		RORaiseAlarm(8);
	    }
	    glLatchPtr[3][1][0]++; // increment current ptr
	    glLatchPtr[3][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[3][1][0] > 499)
		glLatchPtr[3][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARDcommand, ReturnBuffer, MAXGASTR);
    }

    if(giNumOfAxes <= 4) // If only 4 axes, no need to go further.
	return;

    // axis E
    rc = GASendDMCCommand(ghDMC, caALEcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch E DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLEcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPEcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[4][0][glLatchPtr[4][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_t, lTemp, &glLatchDataEU[4][0][glLatchPtr[4][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(4, 0, glLatchPtr[4][0][0]))
	    {
		RORaiseAlarm(16);
	    }
	    glLatchPtr[4][0][0]++; // increment current ptr
	    glLatchPtr[4][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[4][0][0] > 499)
		glLatchPtr[4][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[4][1][glLatchPtr[4][1][0]++] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_t, lTemp, &glLatchDataEU[4][1][glLatchPtr[4][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(4, 1, glLatchPtr[4][1][0]))
	    {
		RORaiseAlarm(16);
	    }
	    glLatchPtr[4][1][0]++; // increment current ptr
	    glLatchPtr[4][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[4][1][0] > 499)
		glLatchPtr[4][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caAREcommand, ReturnBuffer, MAXGASTR);
    }

    // axis F
    rc = GASendDMCCommand(ghDMC, caALFcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch F DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLFcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPFcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[5][0][glLatchPtr[5][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_r, lTemp, &glLatchDataEU[5][0][glLatchPtr[5][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(5, 0, glLatchPtr[5][0][0]))
	    {
		RORaiseAlarm(32);
	    }
	    glLatchPtr[5][0][0]++; // increment current ptr
	    glLatchPtr[5][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[5][0][0] > 499)
		glLatchPtr[5][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[5][1][glLatchPtr[5][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_t, lTemp, &glLatchDataEU[5][1][glLatchPtr[5][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(5, 1, glLatchPtr[5][1][0]))
	    {
		RORaiseAlarm(32);
	    }
	    glLatchPtr[5][1][0]++; // increment current ptr
	    glLatchPtr[5][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[5][1][0] > 499)
		glLatchPtr[5][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARFcommand, ReturnBuffer, MAXGASTR);
    }

    // axis G
    rc = GASendDMCCommand(ghDMC, caALGcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch G DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLGcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPGcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[6][0][glLatchPtr[6][0][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_z, lTemp, &glLatchDataEU[6][0][glLatchPtr[6][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(6, 0, glLatchPtr[6][0][0]))
	    {
		RORaiseAlarm(64);
	    }
	    glLatchPtr[6][0][0]++; // increment current ptr
	    glLatchPtr[6][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[6][0][0] > 499)
		glLatchPtr[6][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[6][1][glLatchPtr[6][1][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_z, lTemp, &glLatchDataEU[6][1][glLatchPtr[6][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(6, 1, glLatchPtr[6][1][0]))
	    {
		RORaiseAlarm(64);
	    }
	    glLatchPtr[6][1][0]++; // increment current ptr
	    glLatchPtr[6][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[6][1][0] > 499)
		glLatchPtr[6][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARGcommand, ReturnBuffer, MAXGASTR);
    }

    // axis H
    rc = GASendDMCCommand(ghDMC, caALHcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch H DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLHcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPHcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[7][0][glLatchPtr[7][0][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_w, lTemp, &glLatchDataEU[7][0][glLatchPtr[7][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(7, 0, glLatchPtr[7][0][0]))
	    {
		RORaiseAlarm(128);
	    }
	    glLatchPtr[7][0][0]++; // increment current ptr
	    glLatchPtr[7][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[7][0][0] > 499)
		glLatchPtr[7][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[7][1][glLatchPtr[7][1][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_w, lTemp, &glLatchDataEU[7][1][glLatchPtr[7][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(7, 1, glLatchPtr[7][1][0]))
	    {
		RORaiseAlarm(128);
	    }
	    glLatchPtr[7][1][0]++; // increment current ptr
	    glLatchPtr[7][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[7][1][0] > 499)
		glLatchPtr[7][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARHcommand, ReturnBuffer, MAXGASTR);
    }

}

void ROUpdateTS(int iFlagArg)
{
    int i, rc, iLEDStatus;
    long lTemp;
    char ReturnBuffer[MAXGASTR];

    if(iFlagArg && iOTFFirstTime)
    {
	if(++giOTFNullCount < 19999) 
	    return; // don't bother Galil during OTF
	else
	{
//printf("giOTFNullCount reached! = %d\n",giOTFNullCount);
 	}
    }
    if(iFlagArg && (++giTSCount < 7)) return;
    giTSCount = 0;


    rc = GASendDMCCommand(ghDMC, TScommand, glTSReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROUpdateTS DMC failure=%d\n",rc);
	return;
    }

    lTemp = strlen(glTSReturnBuffer);
    glTSReturnBuffer[lTemp++] = ',';
    glTSReturnBuffer[lTemp] = '\0';

    ConvertStrToL(glTSReturnBuffer, 8, glTSArray);

    for (i = 0; i < iNumAxes; i++)
    {
//    	if (i == 0)
// 	    // Initialize the lTemp pointer to the first entry
//       	    lTemp = atol( strtok(ReturnBuffer, ",") );
//	else
//	    lTemp = atol( strtok(NULL, ",") );

	// motion checking
	if (glTSArray[i] & 0x80)
	{
	    ulAMFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // axis in motion
	}
	else
	{
	    // motion complete if not homing
	    rc = GASendDMCCommand(ghDMC, HOMGcommand, ReturnBuffer, MAXGASTR);
	    ulHoming = atol(ReturnBuffer);
	    if (ulHoming & uaAxisMap[i][EQUIPE_AXIS])
	    {
		ulAMFlag = ulAMFlag;
////printf("motion not complete due to homing...\n");
	    }
	    else
		ulAMFlag |= uaAxisMap[i][EQUIPE_AXIS]; // axis motion complete
	}

	// ER checking
	if (glTSArray[i] & 0x40)
	{
		ulERFlag |= uaAxisMap[i][EQUIPE_AXIS]; // axis ER
		if (giPrevERFlag[i] == 0)
		{	// raise ERR Interrupt
			iERRInterrupt = TRUE;
			giPrevERFlag[i] = 1;
			rc = GASendDMCCommand(ghDMC, HXcommand, ReturnBuffer, MAXGASTR);
			rc = GASendDMCCommand(ghDMC, HX2command, ReturnBuffer, MAXGASTR);
			if (i==2)
			{
				ROServiceBrake(ROBOTFILE, FALSE);
			}
		}
	}
	else
	{
		ulERFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // axis ER clear
		giPrevERFlag[i] = 0;
	}

	// SH checking
	if (glTSArray[i] & 0x20)
	{
////printf("servo off axis=%d TS=%d\n",i,glTSArray[i]);
		ulServoFlag |= uaAxisMap[i][EQUIPE_AXIS]; // servo off
		// turn on red LED light
		iLEDStatus = inb(IO_PRE_OUTPUT_L);
		iLEDStatus &= 0xF7;	// 4th bit OFF to turn on RED
		outb(iLEDStatus, IO_PRE_OUTPUT_L);
	}
	else
	{
		ulServoFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // servo on
	}

	if(ulServoFlag == 0)
	{
		// turn off red LED light
		iLEDStatus = inb(IO_PRE_OUTPUT_L);
		iLEDStatus |= 0x08;	// 4th bit ON to turn off RED
		outb(iLEDStatus, IO_PRE_OUTPUT_L);
	}

	// Limit checking		
	if (i != 1)		// skip for R-axis limit checking
	{
	    if (glTSArray[i] & 0x08)
	    {
		ulLMPosFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // pos LM clear
		giPrevPLFlag[i] = 0;
	    }
	    else
	    {
		ulLMPosFlag |= uaAxisMap[i][EQUIPE_AXIS]; // pos LM
		if (giPrevPLFlag[i] == 0 && ulHomed & (1<<i))
		{
			iLMMInterrupt = TRUE;
			giPrevPLFlag[i] = 1;
		}
	    }

	    if (glTSArray[i] & 0x04)
	    {
		ulLMNegFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // neg LM clear
		giPrevNLFlag[i] = 0;
	    }
	    else
	    {
		ulLMNegFlag |= uaAxisMap[i][EQUIPE_AXIS]; // neg LM
		if(giPrevNLFlag[i] == 0 && ulHomed & (1<<i))
		{
			iLMMInterrupt = TRUE;
			giPrevNLFlag[i] = 1;
		}
	    }
/*********************************************************
/* HOME Flag check should be something else
	    if (glTSArray[i] & 0x02)
	    {
		ulLMNegFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // neg LM clear (home swith = Neg LM)
		giPrevNLFlag[i] = 0;
	    }
	    else
	    {
		ulLMNegFlag |= uaAxisMap[i][EQUIPE_AXIS]; // neg LM (home switch = Neg LM
		if(giPrevNLFlag[i] == 0 && ulHomed & (1<<i))
		{
			iLMMInterrupt = TRUE;
			giPrevNLFlag[i] = 1;
		}
	    }
*/
	}
    }

    if(giSysCfgNum == 30) // 30 = I2AXO special 
    {
	if ((~ulHomed) & 0x0F)
    	{
	    rc = GASendDMCCommand(ghDMC, HOMDcommand, ReturnBuffer, MAXGASTR);
	    ulHomed = atol(ReturnBuffer);
     	}
    }
    else
    {
    	if ((~ulHomed) & 0xFF)
    	{
	    rc = GASendDMCCommand(ghDMC, HOMDcommand, ReturnBuffer, MAXGASTR);
	    ulHomed = atol(ReturnBuffer);
    	}
    }
////printf("Homed:%x  AM:%x  Servo:%x  ER:%x  pLM:%x  nLM:%x\n",ulHomed, ulAMFlag, ulServoFlag, ulERFlag, ulLMPosFlag, ulLMNegFlag);
}


=======
    }
}

void ROCheckLatch(int iFlagArg)
{
    int iLatch, rc, iDat;
    long lTemp, lTP;
    char ReturnBuffer[MAXGASTR];
    FILE *iFP;
    int iCount;

    if(giDisableLatch) return;

    if(giSysCfgNum == 30) // 30 = I2AXO special 
    {
	if ((~ulHomed) & 0x0F) return;
    }
    else
    {
    	if ((~ulHomed) & 0xFF) return;
    }

    if(iFlagArg && iOTFAligning) return; // don't bother Galil during OTF

    if(iFlagArg && (++giLACount < 23)) return;
    giLACount = 0;

    // axis A
    rc = GASendDMCCommand(ghDMC, caALAcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch A DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch) // Active Low
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLAcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPAcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[0][0][glLatchPtr[0][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_T, lTemp, &glLatchDataEU[0][0][glLatchPtr[0][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(0, 0, glLatchPtr[0][0][0]))
	    {
		RORaiseAlarm(1);
	    }
	    glLatchPtr[0][0][0]++; // increment current ptr
	    glLatchPtr[0][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[0][0][0] > 499)
		glLatchPtr[0][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[0][1][glLatchPtr[0][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_T, lTemp, &glLatchDataEU[0][1][glLatchPtr[0][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(0, 1, glLatchPtr[0][1][0]))
	    {
		RORaiseAlarm(1);
	    }
	    glLatchPtr[0][1][0]++; // increment current ptr
	    glLatchPtr[0][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[0][1][0] > 499)
		glLatchPtr[0][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARAcommand, ReturnBuffer, MAXGASTR);
    }

    // axis B
// COMMENTED OUT FOR OTF TEST 110104   
    rc = GASendDMCCommand(ghDMC, caALBcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch B DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLBcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPBcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[1][0][glLatchPtr[1][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_R, lTemp, &glLatchDataEU[1][0][glLatchPtr[1][0][0]]);
//printf("Rlatch: enc=%d eu=%d ptr=%d\n",lTemp,glLatchDataEU[1][0][glLatchPtr[1][0][0]],glLatchPtr[1][0][0]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(1, 0, glLatchPtr[1][0][0]))
	    {
		RORaiseAlarm(2);
	    }
	    glLatchPtr[1][0][0]++; // increment current ptr
	    glLatchPtr[1][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[1][0][0] > 499)
		glLatchPtr[1][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[1][1][glLatchPtr[1][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_R, lTemp, &glLatchDataEU[1][1][glLatchPtr[1][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(1, 1, glLatchPtr[1][1][0]))
	    {
		RORaiseAlarm(2);
	    }
	    glLatchPtr[1][1][0]++; // increment current ptr
	    glLatchPtr[1][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[1][1][0] > 499)
		glLatchPtr[1][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARBcommand, ReturnBuffer, MAXGASTR);
    }

    // axis C
    rc = GASendDMCCommand(ghDMC, caALCcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch C DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLCcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPCcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[2][0][glLatchPtr[2][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_Z, lTemp, &glLatchDataEU[2][0][glLatchPtr[2][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(2, 0, glLatchPtr[2][0][0]))
	    {
		RORaiseAlarm(4);
	    }
	    glLatchPtr[2][0][0]++; // increment current ptr
	    glLatchPtr[2][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[2][0][0] > 499)
		glLatchPtr[2][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[2][1][glLatchPtr[2][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_Z, lTemp, &glLatchDataEU[2][1][glLatchPtr[2][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(2, 1, glLatchPtr[2][1][0]))
	    {
		RORaiseAlarm(4);
	    }
	    glLatchPtr[2][1][0]++; // increment current ptr
	    glLatchPtr[2][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[2][1][0] > 499)
		glLatchPtr[2][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARCcommand, ReturnBuffer, MAXGASTR);
    }

    // axis D
    rc = GASendDMCCommand(ghDMC, caALDcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch D DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLDcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPDcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[3][0][glLatchPtr[3][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_W, lTemp, &glLatchDataEU[3][0][glLatchPtr[3][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(3, 0, glLatchPtr[3][0][0]))
	    {
		RORaiseAlarm(8);
	    }
	    glLatchPtr[3][0][0]++; // increment current ptr
	    glLatchPtr[3][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[3][0][0] > 499)
		glLatchPtr[3][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[3][1][glLatchPtr[3][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_W, lTemp, &glLatchDataEU[3][1][glLatchPtr[3][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(3, 1, glLatchPtr[3][1][0]))
	    {
		RORaiseAlarm(8);
	    }
	    glLatchPtr[3][1][0]++; // increment current ptr
	    glLatchPtr[3][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[3][1][0] > 499)
		glLatchPtr[3][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARDcommand, ReturnBuffer, MAXGASTR);
    }

    if(giNumOfAxes <= 4) // If only 4 axes, no need to go further.
	return;

    // axis E
    rc = GASendDMCCommand(ghDMC, caALEcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch E DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLEcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPEcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[4][0][glLatchPtr[4][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_t, lTemp, &glLatchDataEU[4][0][glLatchPtr[4][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(4, 0, glLatchPtr[4][0][0]))
	    {
		RORaiseAlarm(16);
	    }
	    glLatchPtr[4][0][0]++; // increment current ptr
	    glLatchPtr[4][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[4][0][0] > 499)
		glLatchPtr[4][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[4][1][glLatchPtr[4][1][0]++] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_t, lTemp, &glLatchDataEU[4][1][glLatchPtr[4][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(4, 1, glLatchPtr[4][1][0]))
	    {
		RORaiseAlarm(16);
	    }
	    glLatchPtr[4][1][0]++; // increment current ptr
	    glLatchPtr[4][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[4][1][0] > 499)
		glLatchPtr[4][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caAREcommand, ReturnBuffer, MAXGASTR);
    }

    // axis F
    rc = GASendDMCCommand(ghDMC, caALFcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch F DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLFcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPFcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[5][0][glLatchPtr[5][0][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_r, lTemp, &glLatchDataEU[5][0][glLatchPtr[5][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(5, 0, glLatchPtr[5][0][0]))
	    {
		RORaiseAlarm(32);
	    }
	    glLatchPtr[5][0][0]++; // increment current ptr
	    glLatchPtr[5][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[5][0][0] > 499)
		glLatchPtr[5][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[5][1][glLatchPtr[5][1][0]] = lTemp; 	// current ptr
	    ROEncoderToEU(RO_AXIS_t, lTemp, &glLatchDataEU[5][1][glLatchPtr[5][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(5, 1, glLatchPtr[5][1][0]))
	    {
		RORaiseAlarm(32);
	    }
	    glLatchPtr[5][1][0]++; // increment current ptr
	    glLatchPtr[5][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[5][1][0] > 499)
		glLatchPtr[5][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARFcommand, ReturnBuffer, MAXGASTR);
    }

    // axis G
    rc = GASendDMCCommand(ghDMC, caALGcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch G DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLGcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPGcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[6][0][glLatchPtr[6][0][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_z, lTemp, &glLatchDataEU[6][0][glLatchPtr[6][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(6, 0, glLatchPtr[6][0][0]))
	    {
		RORaiseAlarm(64);
	    }
	    glLatchPtr[6][0][0]++; // increment current ptr
	    glLatchPtr[6][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[6][0][0] > 499)
		glLatchPtr[6][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[6][1][glLatchPtr[6][1][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_z, lTemp, &glLatchDataEU[6][1][glLatchPtr[6][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(6, 1, glLatchPtr[6][1][0]))
	    {
		RORaiseAlarm(64);
	    }
	    glLatchPtr[6][1][0]++; // increment current ptr
	    glLatchPtr[6][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[6][1][0] > 499)
		glLatchPtr[6][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARGcommand, ReturnBuffer, MAXGASTR);
    }

    // axis H
    rc = GASendDMCCommand(ghDMC, caALHcommand, ReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROCheckLatch H DMC failure=%d\n",rc);
	return;
    }

    iLatch = atoi(ReturnBuffer);
    if(!iLatch)
    {
    	giEncoderDriftDetector = 1;
	rc = GASendDMCCommand(ghDMC, caRLHcommand, ReturnBuffer, MAXGASTR);
	lTemp = atol(ReturnBuffer);
	rc = GASendDMCCommand(ghDMC, caTPHcommand, ReturnBuffer, MAXGASTR);
	lTP = atol(ReturnBuffer);
	if(lTP > lTemp) // + direction motion
	{
	    glLatchData[7][0][glLatchPtr[7][0][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_w, lTemp, &glLatchDataEU[7][0][glLatchPtr[7][0][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(7, 0, glLatchPtr[7][0][0]))
	    {
		RORaiseAlarm(128);
	    }
	    glLatchPtr[7][0][0]++; // increment current ptr
	    glLatchPtr[7][0][1]++; // total counts accumulated forever
	    if(glLatchPtr[7][0][0] > 499)
		glLatchPtr[7][0][0] = 0;
	}
	else		// - direction motion
	{
	    glLatchData[7][1][glLatchPtr[7][1][0]] = lTemp; 	// current ptr data
	    ROEncoderToEU(RO_AXIS_w, lTemp, &glLatchDataEU[7][1][glLatchPtr[7][1][0]]);
	    // Do a range-check on this latch pos
	    if(PDOutOfRange(7, 1, glLatchPtr[7][1][0]))
	    {
		RORaiseAlarm(128);
	    }
	    glLatchPtr[7][1][0]++; // increment current ptr
	    glLatchPtr[7][1][1]++; // total counts accumulated forever
	    if(glLatchPtr[7][1][0] > 499)
		glLatchPtr[7][1][0] = 0;
	}
	// re-arm it
	rc = GASendDMCCommand(ghDMC, caARHcommand, ReturnBuffer, MAXGASTR);
    }

}

void ROUpdateTS(int iFlagArg)
{
    int i, rc, iLEDStatus;
    long lTemp;
    char ReturnBuffer[MAXGASTR];

    if(iFlagArg && iOTFFirstTime)
    {
	if(++giOTFNullCount < 19999) 
	    return; // don't bother Galil during OTF
	else
	{
//printf("giOTFNullCount reached! = %d\n",giOTFNullCount);
 	}
    }
    if(iFlagArg && (++giTSCount < 7)) return;
    giTSCount = 0;


    rc = GASendDMCCommand(ghDMC, TScommand, glTSReturnBuffer, MAXGASTR);

    // If galil timeout error, just return.
    if(rc != SUCCESS) 
    {
//printf("ROUpdateTS DMC failure=%d\n",rc);
	return;
    }

    lTemp = strlen(glTSReturnBuffer);
    glTSReturnBuffer[lTemp++] = ',';
    glTSReturnBuffer[lTemp] = '\0';

    ConvertStrToL(glTSReturnBuffer, 8, glTSArray);

    for (i = 0; i < iNumAxes; i++)
    {
//    	if (i == 0)
// 	    // Initialize the lTemp pointer to the first entry
//       	    lTemp = atol( strtok(ReturnBuffer, ",") );
//	else
//	    lTemp = atol( strtok(NULL, ",") );

	// motion checking
	if (glTSArray[i] & 0x80)
	{
	    ulAMFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // axis in motion
	}
	else
	{
	    // motion complete if not homing
	    rc = GASendDMCCommand(ghDMC, HOMGcommand, ReturnBuffer, MAXGASTR);
	    ulHoming = atol(ReturnBuffer);
	    if (ulHoming & uaAxisMap[i][EQUIPE_AXIS])
	    {
		ulAMFlag = ulAMFlag;
////printf("motion not complete due to homing...\n");
	    }
	    else
		ulAMFlag |= uaAxisMap[i][EQUIPE_AXIS]; // axis motion complete
	}

	// ER checking
	if (glTSArray[i] & 0x40)
	{
		ulERFlag |= uaAxisMap[i][EQUIPE_AXIS]; // axis ER
		if (giPrevERFlag[i] == 0)
		{	// raise ERR Interrupt
			iERRInterrupt = TRUE;
			giPrevERFlag[i] = 1;
			rc = GASendDMCCommand(ghDMC, HXcommand, ReturnBuffer, MAXGASTR);
			rc = GASendDMCCommand(ghDMC, HX2command, ReturnBuffer, MAXGASTR);
			if (i==2)
			{
				ROServiceBrake(ROBOTFILE, FALSE);
			}
		}
	}
	else
	{
		ulERFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // axis ER clear
		giPrevERFlag[i] = 0;
	}

	// SH checking
	if (glTSArray[i] & 0x20)
	{
////printf("servo off axis=%d TS=%d\n",i,glTSArray[i]);
		ulServoFlag |= uaAxisMap[i][EQUIPE_AXIS]; // servo off
		// turn on red LED light
		iLEDStatus = inb(IO_PRE_OUTPUT_L);
		iLEDStatus &= 0xF7;	// 4th bit OFF to turn on RED
		outb(iLEDStatus, IO_PRE_OUTPUT_L);
	}
	else
	{
		ulServoFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // servo on
	}

	if(ulServoFlag == 0)
	{
		// turn off red LED light
		iLEDStatus = inb(IO_PRE_OUTPUT_L);
		iLEDStatus |= 0x08;	// 4th bit ON to turn off RED
		outb(iLEDStatus, IO_PRE_OUTPUT_L);
	}

	// Limit checking		
	if (i != 1)		// skip for R-axis limit checking
	{
	    if (glTSArray[i] & 0x08)
	    {
		ulLMPosFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // pos LM clear
		giPrevPLFlag[i] = 0;
	    }
	    else
	    {
		ulLMPosFlag |= uaAxisMap[i][EQUIPE_AXIS]; // pos LM
		if (giPrevPLFlag[i] == 0 && ulHomed & (1<<i))
		{
			iLMMInterrupt = TRUE;
			giPrevPLFlag[i] = 1;
		}
	    }

	    if (glTSArray[i] & 0x04)
	    {
		ulLMNegFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // neg LM clear
		giPrevNLFlag[i] = 0;
	    }
	    else
	    {
		ulLMNegFlag |= uaAxisMap[i][EQUIPE_AXIS]; // neg LM
		if(giPrevNLFlag[i] == 0 && ulHomed & (1<<i))
		{
			iLMMInterrupt = TRUE;
			giPrevNLFlag[i] = 1;
		}
	    }
/*********************************************************
/* HOME Flag check should be something else
	    if (glTSArray[i] & 0x02)
	    {
		ulLMNegFlag &= ~uaAxisMap[i][EQUIPE_AXIS]; // neg LM clear (home swith = Neg LM)
		giPrevNLFlag[i] = 0;
	    }
	    else
	    {
		ulLMNegFlag |= uaAxisMap[i][EQUIPE_AXIS]; // neg LM (home switch = Neg LM
		if(giPrevNLFlag[i] == 0 && ulHomed & (1<<i))
		{
			iLMMInterrupt = TRUE;
			giPrevNLFlag[i] = 1;
		}
	    }
*/
	}
    }

    if(giSysCfgNum == 30) // 30 = I2AXO special 
    {
	if ((~ulHomed) & 0x0F)
    	{
	    rc = GASendDMCCommand(ghDMC, HOMDcommand, ReturnBuffer, MAXGASTR);
	    ulHomed = atol(ReturnBuffer);
     	}
    }
    else
    {
    	if ((~ulHomed) & 0xFF)
    	{
	    rc = GASendDMCCommand(ghDMC, HOMDcommand, ReturnBuffer, MAXGASTR);
	    ulHomed = atol(ReturnBuffer);
    	}
    }
////printf("Homed:%x  AM:%x  Servo:%x  ER:%x  pLM:%x  nLM:%x\n",ulHomed, ulAMFlag, ulServoFlag, ulERFlag, ulLMPosFlag, ulLMNegFlag);
}


>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

/****************************************************************
 *
 * Function:    ROInit
 *
 * Abstract:    Main module initialize the Mechanism module configuration.
 *      This copies in the initialization arrays that are passed in
 *      into the local matrix concurrently setting up the axis maps
 *      for A (all robot axes), a (all pre-aligner axes), and all
 *      system axes. Then it initializes the motion module, the
 *      datafile module, the action buffer, and reads the parameter
 *      files out of NVSRAM into the local datafile structures
 *      and uses that information to initialize the Galil card(s).
 *
 * Parameters:
 *      iNumAxesArg     (in) The total number of axes in the system
 *      iaMechTypeArg   (in) The mechanism type (robot, pre, etc.)
 *      iaEquipeAxisArg (in) The Equipe axis designation (R, w, etc.)
 *      iaGalilAxisArg  (in) The Galil axis it maps to (X, Y, etc.)
 *      iDefineFlagArg  (in) The system define flags
 *      iEmulatorArg    (in) The emulation used in the system
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROInit(int iNumAxesArg, int *iaMechTypeArg, int *iaEquipeAxisArg, int *iaGalilAxisArg, int *iaSpecialAxisArg, int iDefineFlagArg, int iEmulatorArg)
{
    int iAxis;                  /* The axis/index to data being copied into the matrix. */
    int iGANumAxes;             /* The total number of axes available from the motion module. */
    int iGANumAxesCard0 = 0;    /* Used to make sure there aren't more than 4 axes
                                 * assigned to the first Galil card. */
    int iGANumAxesCard1 = 0;    /* Used to make sure there aren't more than 4 axes
                                 * assigned to the second Galil card. */
    int iNumAxesCard0 = 0;      /* Used to make sure there aren't more than 4 axes
                                 * assigned to the first Galil card. */
    int iNumAxesCard1 = 0;      /* Used to make sure there aren't more than 4 axes
                                 * assigned to the second Galil card. */
    int iRobotExists = FALSE;   /* If a robot exists, this flag reads in the datafile and inits the Galil card. */
    int iPreExists = FALSE;     /* If a pre-aligner exists, this flag reads in the datafile and inits the Galil card. */
    int iAuxExists = FALSE;     /* If auxilliary components exist, this flag reads in the datafile and inits the Galil card. */
    int iOTFFlag;               /* Feature configuration file for OTF active. */
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    FILE *iFP;
    int iCount;

    /* Copy over the define and emulator flags to the local copies. */
    iDefineFlag = iDefineFlagArg;
    iEmulator = iEmulatorArg;

    /* Initialize the datafile module. */
    if (FIOInitParamModule() == FAILURE)
    {
        SSSetModulesInitializationsStatus(DATAFILE_MODULE, TRUE);
        return FAILURE;
    }

    for (iAxis=0; iAxis<iNumAxesArg; iAxis++)
    {
        /* Copy over the axes maps from the arrays passed in to the local matrix. */
        uaAxisMap[iAxis][MECH_TYPE] = iaMechTypeArg[iAxis];
        uaAxisMap[iAxis][EQUIPE_AXIS] = iaEquipeAxisArg[iAxis];
        uaAxisMap[iAxis][GALIL_AXIS] = iaGalilAxisArg[iAxis];
        uaAxisMap[iAxis][SPECIAL_AXIS] = iaSpecialAxisArg[iAxis];

<<<<<<< HEAD
//printf("uaAxisMap[%d][galil]=%d\n",iAxis,iaGalilAxisArg[iAxis]);

=======
//printf("uaAxisMap[%d][galil]=%d\n",iAxis,iaGalilAxisArg[iAxis]);

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        /* Possibly add in the current axis to the A (all robot axes) mask. */
        if ((iaEquipeAxisArg[iAxis] >= RO_AXIS_T) &&
            (iaEquipeAxisArg[iAxis] <= RO_AXIS_Z))
        {
            ulAxisALLRbt |= (unsigned long)(iaEquipeAxisArg[iAxis]);
            iPreOnly = FALSE;
        }

        /* Increment the number of axes requested on the first Galil card.
         * This will used to check against the number of axes available,
         * which is reported by the motion module. */
        if ((iaEquipeAxisArg[iAxis] >= RO_AXIS_T) &&
            (iaEquipeAxisArg[iAxis] <= RO_AXIS_W))
        {
            iNumAxesCard0++;
        }

        /* Possibly add in the current axis to the a (all pre-aligner axes) mask. */
        if ((iaEquipeAxisArg[iAxis] >= RO_AXIS_t) &&
            (iaEquipeAxisArg[iAxis] <= RO_AXIS_z))
        {
            ulAxisallPre |= (unsigned long)(iaEquipeAxisArg[iAxis]);
            iNumGalilCards = 2;
        }

        /* Increment the number of axes requested on the first Galil card.
         * This will used to check against the number of axes available,
         * which is reported by the motion module. */
        if ((iaEquipeAxisArg[iAxis] >= RO_AXIS_t) &&
            (iaEquipeAxisArg[iAxis] <= RO_AXIS_w))
        {
            iNumAxesCard1++;
        }

        /* Update the "all system axes" mask. */
        ulAllAMFlag |= (unsigned long)(iaEquipeAxisArg[iAxis]);

        /* Set the flags to read the parameter file and initialize the Galil card. */
        if (iaMechTypeArg[iAxis] == ROBOTFILE) iRobotExists = TRUE;
        if (iaMechTypeArg[iAxis] == PREALIGNFILE) iPreExists = TRUE;
        if (iaMechTypeArg[iAxis] == AUXFILE) iAuxExists = TRUE;
    }

    /* Remember the total number for axes in the system. */
    iNumAxes = iNumAxesArg;

    /* Adjust the 'a' in the case of a single axis prealigner on the second Galil card. */
    if (ROGetSpecialAxis(RO_SINGLE_PRE) == RO_AXIS_t)
        ulAxisallPre = RO_NO_COMBINED_AXES;

    /* Initialize the motion module. */
    if (GAInit(iNumGalilCards, iPreOnly) == FAILURE)
    {
        SSSetModulesInitializationsStatus(MOTION_MODULE, TRUE);
        return FAILURE;
    }

    if (!iPreOnly)
    {
        /* Make sure there aren't more than 4 axes assigned to the first Galil card. */
        if (GAGetMaxNumberOfAxisSupported(GA_CARD_0, &iGANumAxesCard0) == FAILURE)
            return FAILURE;
        /* Check to see if there are enough axes on the Galil cards
         * to handle the number of axes initialized. iNumAxesCard(0 or 1) should
         * be 7 or 15, for a 3-axis or 4-axis Galil card, respectively. */
        iGANumAxes = 0;
        for (iAxis=0; iAxis<4; iAxis++)
        {
            if (iGANumAxesCard0 & (1 << iAxis))
            {
                iGANumAxes++;
            }
        }
        if (iGANumAxes < iNumAxesCard0)
            return FAILURE;
    }

    /* If there are 2 Galil cards in the system. */
    if (iNumGalilCards >= 2)
    {
        /* Make sure there aren't more than 4 axes assigned to the first Galil card. */
        if (GAGetMaxNumberOfAxisSupported(GA_CARD_1, &iGANumAxesCard1) == FAILURE)
            return FAILURE;
        /* Check to see if there are enough axes on the Galil cards
         * to handle the number of axes initialized. iNumAxesCard(0 or 1) should
         * be 7 or 15, for a 3-axis or 4-axis Galil card, respectively. */
        iGANumAxes = 0;
        for (iAxis=0; iAxis<4; iAxis++)
        {
            if (iGANumAxesCard1 & (1 << iAxis))
            {
                iGANumAxes++;
            }
        }
        if (iGANumAxes < iNumAxesCard1)
            return FAILURE;
    }

    /* Read the parameter file in NVSRAM into the datafile internal structure
     * and initialize the Galil card for... */
    /* Robot */
    if (iRobotExists)
    {
        FIOReadParam(ROBOTFILE);
        if (ROInitGalil(ROBOTFILE) == FAILURE)
            return FAILURE;
    }
    /* Pre-aligner */
    if (iPreExists)
    {
<<<<<<< HEAD
        FIOReadParam(PREALIGNFILE);
	if(giSysCfgNum == 30) // 30 = I2AXO special 
	{
            if (ROInitGalil(PREALIGNFILE) == FAILURE)
            	return FAILURE;
	}
	else
	{
            if (ROInitGalil(PREALIGNFILE) == FAILURE)
    	        return FAILURE;
    	    FIOReadWafer();
    	    FIOReadCalibTable();
=======
        FIOReadParam(PREALIGNFILE);
	if(giSysCfgNum == 30) // 30 = I2AXO special 
	{
            if (ROInitGalil(PREALIGNFILE) == FAILURE)
            	return FAILURE;
	}
	else
	{
            if (ROInitGalil(PREALIGNFILE) == FAILURE)
    	        return FAILURE;
    	    FIOReadWafer();
    	    FIOReadCalibTable();
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	}
    }
    /* Auxilliary components */
    if (iAuxExists)
    {
        FIOReadParam(AUXFILE);
        if (ROInitGalil(AUXFILE) == FAILURE)
            return FAILURE;
    }
    /* Initialize/enable the amplifiers. */
//    if (GAAmpInit() == FAILURE)
//        return FAILURE;

    /* Read the coordinate file from NVSRAM into the datafile internal structure. */
    FIOReadCoord();

    /* If system configured for OTF, read OTF station calibration file. */
    if (FIOGetCfgFeatureFlags(OTF, &iOTFFlag) == FAILURE)
        return FAILURE;
    if (iOTFFlag)
    {
       FIOReadOTFFile();
    }
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    iFP = fopen( ALFILENAME, "r");
    if( iFP == (FILE *)0 )
    {
	perror( "Alarm File Read Open Error " );
    }
    else
<<<<<<< HEAD
    {
        iCount = fread(&giEncoderAlarm, sizeof(int), 1, iFP);
	if (iCount <= 0) 
=======
    {
        iCount = fread(&giEncoderAlarm, sizeof(int), 1, iFP);
	if (iCount <= 0) 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	{
	    perror( "Alarm File Read Error " );
	}
        
      	/* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
       	fclose( iFP );
<<<<<<< HEAD
    }

////printf("iNumAxes: %d   ulAxisALLRbt: %x    ulAxisallPre: %x\n", iNumAxes, ulAxisALLRbt, ulAxisallPre);
=======
    }

////printf("iNumAxes: %d   ulAxisALLRbt: %x    ulAxisallPre: %x\n", iNumAxes, ulAxisALLRbt, ulAxisallPre);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROAxisAllRobot
 *
 * Abstract:    A function interface to the ulAxisALLRbt variable
 *
 * Parameters:  None
 *
 * Returns:     The ulAxisALLRbt variable value
 *
 ***************************************************************/
unsigned long ROAxisAllRobot()
{
    return ulAxisALLRbt;
}


/****************************************************************
 *
 * Function:    ROAxisAllPre
 *
 * Abstract:    A function interface to the ulAxisallPre variable
 *
 * Parameters:  None
 *
 * Returns:     The ulAxisallPre variable value
 *
 ***************************************************************/
unsigned long ROAxisAllPre()
{
    return ulAxisallPre;
}


/****************************************************************
 *
 * Function:    ROStartGalilMode
 *
 * Abstract:    Initializes Galil mode. Galil mode is a direct
 *      communication interface to the Galil motion control card.
 *      It reads characters directly from the comm port and
 *      writes them to the Galil card which uses an ASCII
 *      communications interface. And vice versa.
 *
 * Parameters:
 *      iCardNumArg     (in) The Galil card to communicate with.
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROStartGalilMode(int iCardNumArg)
{
    char cIOChar;                       /* The character read from the comm port. */
//    int iGAChar;                        /* The character read from the Galil card. */
    int iQuitLoop = FALSE;              /* Quits Galil mode on a ctrl-I key press. */
    int iStatus, iUseGalil, iGalilError, iEcho, iCmdPort, iNumCharsLeft;  /* Temporary variables. */
<<<<<<< HEAD
    char sBuf[MAXGASTR];
    char cpRespStr[MAXGASTR];
    char cpCommand[MAXGASTR];
    int iCardNum;
=======
    char sBuf[MAXGASTR];
    char cpRespStr[MAXGASTR];
    char cpCommand[MAXGASTR];
    int iCardNum;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    int iRet, nChar;

    /* Translate the card number passed in (0 or 1) into the motion module card designation. */
    iCardNum = GA_CARD_0;

    /* Save current settings to be restored later. */
    iUseGalil = GAGetUseGalilFlag();
    iGalilError = GAGetGalilErrorFlag();
    iStatus = SSReadStatusBits(CTRL_ERR);
    /* Initialize variables used in this module. */
    iCmdPort = SERGetCmdPort();
<<<<<<< HEAD
    if (SERGetCommVals(iCmdPort, ECHO_MODE, &iEcho) == FAILURE) goto error_exit;
    if (iCmdPort == SERGetTTPort())
    {
        if (SERSetCommVals(iCmdPort, ECHO_MODE, TRUE) == FAILURE) goto error_exit;
=======
    if (SERGetCommVals(iCmdPort, ECHO_MODE, &iEcho) == FAILURE) goto error_exit;
    if (iCmdPort == SERGetTTPort())
    {
        if (SERSetCommVals(iCmdPort, ECHO_MODE, TRUE) == FAILURE) goto error_exit;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (TTSetEchoMode(TRUE) == FAILURE) goto error_exit;
    }

    /* Put out a prompt to get started */
    if (SERPutsTxBuff(iCmdPort, ":") == FAILURE) goto error_exit;
    if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;
<<<<<<< HEAD

    nChar = 0;
    while (!iQuitLoop)
    {
	iRet = DMCGetUnsolicitedResponse(ghDMC, cpRespStr, MAXGASTR);
 
    	iRet =COReadChar(iCmdPort, &cIOChar, &iNumCharsLeft, &iStatus);

        /* Check for character from user */
        if (iRet == FAILURE) goto error_exit;
     	if (iRet == 0)
	{
//	    if(iEcho)
//	    {
//                sBuf[0] = cIOChar;
//                sBuf[1] = 0;
//                if (SERPutsTxBuff(iCmdPort, sBuf) == FAILURE) goto error_exit;
//	    }
	
	    if (cIOChar == END_TALK)
	    {
		iQuitLoop = TRUE;
	    }
	    else
	    {

		cpCommand[nChar++] = cIOChar;
		if (cIOChar == '\r' )
		{ 
		    cpCommand[nChar] = '\0';
		    nChar = 0;
		    iRet = GASendDMCCommand(ghDMC, cpCommand, cpRespStr, MAXGASTR);
		    if (iRet != 0)
		    {
			  //printf("GASendDMCCommand cpCommand error: %s %s %d\n", cpCommand, cpRespStr, iRet);
			  DMCClear(ghDMC);
		    }
		    if (SERPutsTxBuff(iCmdPort, cpRespStr) == FAILURE) goto error_exit;
		    if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;
		}
	    }
	}
    }
=======

    nChar = 0;
    while (!iQuitLoop)
    {
	iRet = DMCGetUnsolicitedResponse(ghDMC, cpRespStr, MAXGASTR);
 
    	iRet =COReadChar(iCmdPort, &cIOChar, &iNumCharsLeft, &iStatus);

        /* Check for character from user */
        if (iRet == FAILURE) goto error_exit;
     	if (iRet == 0)
	{
//	    if(iEcho)
//	    {
//                sBuf[0] = cIOChar;
//                sBuf[1] = 0;
//                if (SERPutsTxBuff(iCmdPort, sBuf) == FAILURE) goto error_exit;
//	    }
	
	    if (cIOChar == END_TALK)
	    {
		iQuitLoop = TRUE;
	    }
	    else
	    {

		cpCommand[nChar++] = cIOChar;
		if (cIOChar == '\r' )
		{ 
		    cpCommand[nChar] = '\0';
		    nChar = 0;
		    iRet = GASendDMCCommand(ghDMC, cpCommand, cpRespStr, MAXGASTR);
		    if (iRet != 0)
		    {
			  //printf("GASendDMCCommand cpCommand error: %s %s %d\n", cpCommand, cpRespStr, iRet);
			  DMCClear(ghDMC);
		    }
		    if (SERPutsTxBuff(iCmdPort, cpRespStr) == FAILURE) goto error_exit;
		    if (SERFlushTxBuff(iCmdPort) == FAILURE) goto error_exit;
		}
	    }
	}
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)


    /* Restore saved settings. */
    GASetGalilErrorFlag(iGalilError);
    GASetUseGalilFlag(iUseGalil);
    SSSetStatusWord(CTRL_ERR, iStatus);
<<<<<<< HEAD

    if (iCmdPort == SERGetTTPort())
    {
	iEcho = FALSE;
        if (SERSetCommVals(iCmdPort, ECHO_MODE, iEcho) == FAILURE) goto error_exit;
    }
    if (SERSetCommVals(iCmdPort, ECHO_MODE, iEcho) == FAILURE) goto error_exit;
=======

    if (iCmdPort == SERGetTTPort())
    {
	iEcho = FALSE;
        if (SERSetCommVals(iCmdPort, ECHO_MODE, iEcho) == FAILURE) goto error_exit;
    }
    if (SERSetCommVals(iCmdPort, ECHO_MODE, iEcho) == FAILURE) goto error_exit;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return SUCCESS;

error_exit:
    return FAILURE;
}


/****************************************************************
 *
 * Function:    ROReadGalilStatus
 *
 * Abstract:    Gets the Galil status. It pools all relevant
 *      information about the motion control card and returns
 *      it to the calling function. It also clears errors
 *      if there are any.
 *
 * Parameters:
 *      uStatusArg      (in) The Galil card status.
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadGalilStatus(unsigned long *ulStatusArg)
{
    /* Get the error condition. */
    *ulStatusArg = GAGetGalilErrorFlag();

    /* Clear and reset all error variables. */
    GASetGalilErrorFlag(0);
    GASetExcIntOutp(0);
    GASetUseGalilFlag(iNumGalilCards+1);
    SSSetStatusWord(CTRL_ERR, FALSE);
    SSSetStatusWord2(GA_COM_TIMEOUT, FALSE);
    SSSetStatusWord2(GA_REENT_ERROR, FALSE);

    /* Pool the status information bit shifting them into the proper bit locations. */
    *ulStatusArg |= ((ulERFlag & ulAllAMFlag) << 8) |
                    ((ulAMFlag & ulAllAMFlag) << 16) |
                    ((ulServoFlag & ulAllAMFlag) << 24);

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROPortRedirect
 *
 * Abstract:    Bypasses the control and connects COM1 to
 *      the requested bypass port.
 *
 * Parameters:
 *      iPortNumArg     (in) The port number to redirect to
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROPortRedirect(int iPortNumArg)
{
    int iReturn;

    /* Not allowed while moving. */
    if ((ulAMFlag&ulAllAMFlag) != ulAllAMFlag)
        return FAILURE;

    /* ...and redirect the comm port... */
    iReturn = SERPortRedirect(iPortNumArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROReadInPositionWindow
 *
 * Abstract:    Read in position window. This is used for
 *      time optimal trajectories (TOT). It creates a window
 *      whereby when the robot is within that window from its
 *      final position, the motion is registered as complete.
 *      Then another axis can move. The end result is that
 *      motion on an axis can commence while another is still
 *      moving.
 *
 *      This is used in conjunction with WMO with the parameter
 *      1, e.g. WMO R,1
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lPositionArg        (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadInPositionWindow(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int iReturn=0;
//    long laPosition[8]={0,0,0,0,0,0,0,0};
    int i, iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if(ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the in position window with the Galil "ID" command. */
//    iReturn = GAGetValsLong(iCardNum, OPT_INTR_POS_COMMAND, uGalilAxes, laPosition);
<<<<<<< HEAD

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	lPositionArg[i] = giIPW[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		lPositionArg[0] = giIPW[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		lPositionArg[1] = giIPW[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		lPositionArg[2] = giIPW[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		lPositionArg[3] = giIPW[3];

=======

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	lPositionArg[i] = giIPW[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		lPositionArg[0] = giIPW[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		lPositionArg[1] = giIPW[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		lPositionArg[2] = giIPW[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		lPositionArg[3] = giIPW[3];

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Scale the position from encoder counts to normal units. */
//    ROScalePos(ulEquipeAxisArg, laPosition, lPositionArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROReadCurrentPosition
 *
 * Abstract:    Gets the current axes positions from the motion
 *      control card.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lPositionArg        (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadCurrentPosition(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int iReturn;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the current axes positions with the Galil "TP" command. */
    if (ulEquipeAxisArg == ROGetSpecialAxis(RO_FLIPPER))
        iReturn = GAGetValsLong(iCardNum, REF_POSITION_COMMAND, uGalilAxes, laPosition);
    else
        iReturn = GAGetValsLong(iCardNum, TELL_POSITION_COMMAND, uGalilAxes, laPosition);

    /* Scale the position from encoder counts to normal units. */
    ROScalePos(ulEquipeAxisArg, laPosition, lPositionArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROReadCurrentError
 *
 * Abstract:    Gets the current position error from the motion
 *      control card.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lErrorArg           (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadCurrentError(unsigned long ulEquipeAxisArg, long *lErrorArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the current axes errors with the Galil "TE" command. */
    iReturn = GAGetValsLong(iCardNum, TELL_ERROR_COMMAND, uGalilAxes, lErrorArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROReadCurrentTorque
 *
 * Abstract:    Gets the current torque from the motion
 *      control card.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lTorqueArg          (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadCurrentTorque(unsigned long ulEquipeAxisArg, long *lTorqueArg)
{
    double dTorque[8];      /* The torque values return from the motion module
                             * are doubles. They are multiplied by 1000 and put
                             * into a long array in the ROGetMilScale function. */
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the current axes torques with the Galil "TT" command. */
    iReturn = GAGetValsDouble(iCardNum, TELL_TORQUE_COMMAND, uGalilAxes, dTorque);

    /* Dont' forget, the torque is multiplied by 1000 for the user. */
    ROGetMilScale(dTorque, lTorqueArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROReadHomeFlag
 *
 * Abstract:    Gets the home flag values by axis. This RHFLG
 *      command complements the HFLG or HOME commands.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lHomeFlagArg        (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadHomeFlag(unsigned long ulEquipeAxisArg, long *lHomeFlagArg)
{
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the home flag values by axis and put them in the array to be returned. */
    if ((ulEquipeAxisArg&RO_AXIS_T) && (ulHomed&RO_AXIS_T)) lHomeFlagArg[0] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_R) && (ulHomed&RO_AXIS_R)) lHomeFlagArg[1] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_Z) && (ulHomed&RO_AXIS_Z)) lHomeFlagArg[2] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_W) && (ulHomed&RO_AXIS_W)) lHomeFlagArg[3] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_t) && (ulHomed&RO_AXIS_t)) lHomeFlagArg[4] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_r) && (ulHomed&RO_AXIS_r)) lHomeFlagArg[5] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_z) && (ulHomed&RO_AXIS_z)) lHomeFlagArg[6] = 1;
    if ((ulEquipeAxisArg&RO_AXIS_w) && (ulHomed&RO_AXIS_w)) lHomeFlagArg[7] = 1;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROReadSCurveProfile
 *
 * Abstract:    Gets the S curve profile on/off settings by axis.
 *      This RTA command complements the TA commands.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lOnOffArg           (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadSCurveProfile(unsigned long ulEquipeAxisArg, long *lOnOffArg)
{
    char caSCurveMsg[25], caSCurveResp[25];
    int iReturn, iAxis;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* For each axis... */
    for (iAxis=0; iAxis<4; iAxis++)
    {
        /* ...if the axis "TA" status was requested... */
        if (uGalilAxes & (1 << iAxis))
        {
            /* ...set up the interrogation command. */
            caSCurveMsg[0] = caSCurveResp[0] = 0;
            strcpy(caSCurveMsg, "MG _TA");
            if (iAxis == 0)
                strcat(caSCurveMsg, "X");
            else if (iAxis == 1)
                strcat(caSCurveMsg, "Y");
            else if (iAxis == 2)
                strcat(caSCurveMsg, "Z");
            else if (iAxis == 3)
                strcat(caSCurveMsg, "W");
            strcat(caSCurveMsg, "\r");

            /* Send it to the appropriate Galil card and get a response. */
            iReturn = GASendReceiveGalil(iCardNum, caSCurveMsg, caSCurveResp);

            /* Make sure the read/write was kosher. */
            if (iReturn == FAILURE)
                return FAILURE;
            if (strlen(caSCurveResp) != 19)
                return FAILURE;

            /* Decode the response and put it in the array to be returned. */
            if (caSCurveResp[10] == '1')
            {
                lOnOffArg[iAxis] = 1;
            }
        }
    }

    return SUCCESS;
}

/****************************************************************
 *
<<<<<<< HEAD
 * Function:    ROReadITValue
 *
 * Abstract:    Gets the independent time constants
=======
 * Function:    ROReadITValue
 *
 * Abstract:    Gets the independent time constants
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 *      This RIT command complements the IT commands.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lValueArg           (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadITValue(unsigned long ulEquipeAxisArg, long *lValueArg)
{
    int iReturn;
    int iCardNum, iFileType;
<<<<<<< HEAD
    unsigned uGalilAxes;
=======
    unsigned uGalilAxes;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	double dTempParameter[8];

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    iReturn = GAGetValsDoubleDefined(iCardNum, IT_SCURVE, uGalilAxes, dTempParameter);
<<<<<<< HEAD
    ROGetMilScale(dTempParameter, lValueArg);
=======
    ROGetMilScale(dTempParameter, lValueArg);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return iReturn;
}

/****************************************************************
 *
 * Function:    ROSetInPositionWindow
 *
 * Abstract:    Set in position window. This is used for
 *      time optimal trajectories (TOT). It creates a window
 *      whereby when the robot is within that window from its
 *      final position, the motion is registered as complete.
 *      Then another axis can move. The end result is that
 *      motion on an axis can commence while another is still
 *      moving.
 *
 *      This is used in conjunction with WMO with the parameter
 *      1, e.g. WMO R,1
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to set
 *      lPositionArg        (out) The values set
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetInPositionWindow(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int iReturn=0;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    int i, iCardNum, iFileType;
    unsigned uGalilAxes;
 
    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Not allowed while moving. */
//    if ((ulAMFlag&ulEquipeAxisArg) != ulEquipeAxisArg)
//        return FAILURE;
<<<<<<< HEAD

    if (ulEquipeAxisArg == RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPW[i] = lPositionArg[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPW[0] = lPositionArg[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPW[1] = lPositionArg[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPW[2] = lPositionArg[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPW[3] = lPositionArg[3];

    /* Unscale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, lPositionArg, laPosition) == FAILURE)
        return FAILURE;

    if (ulEquipeAxisArg == RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPWenc[i] = laPosition[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPWenc[0] = laPosition[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPWenc[1] = laPosition[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPWenc[2] = laPosition[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPWenc[3] = laPosition[3];

	for (i=0; i<8; ++i) giIPWFlag[i] = (giIPW[i] > 0) ? 1 : 0;


=======

    if (ulEquipeAxisArg == RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPW[i] = lPositionArg[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPW[0] = lPositionArg[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPW[1] = lPositionArg[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPW[2] = lPositionArg[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPW[3] = lPositionArg[3];

    /* Unscale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, lPositionArg, laPosition) == FAILURE)
        return FAILURE;

    if (ulEquipeAxisArg == RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPWenc[i] = laPosition[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPWenc[0] = laPosition[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPWenc[1] = laPosition[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPWenc[2] = laPosition[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPWenc[3] = laPosition[3];

	for (i=0; i<8; ++i) giIPWFlag[i] = (giIPW[i] > 0) ? 1 : 0;


>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Set the in position window with the Galil "ID" command. */
//    iReturn = GASetValsLong(iCardNum, OPT_INTR_POS_COMMAND, uGalilAxes, laPosition);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROQuitScan
 *
 * Abstract:    Quit scanning. Just release the position latching
 *      feature.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROQuitScan()
{
    long laPosition[10];        /* The latched position. */
    int iReturn;

    /* This disables the scanner interrupt and sets the scanner flags
     * to "not scanning". By disabling the interrupt, the scanner IO
     * won't be constantly polling the motion module for latched positions. */
    MPDisableWES();
    MPSetNotScanning();

    /* Read the last latched position with the Galil "RL" command thus
     * emptying the Galil internal buffer. */
    iReturn = GAGetValsLong(iScanCardNum, REPORT_LATCH_POS_COMMAND, GAZAXIS0, laPosition);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROSetHomeFlag
 *
 * Abstract:    Manually sets the ulHomed flag. This HFLG
 *      command complements the RHFLG command.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to set
 *      lValueArg               (in) The values set
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetHomeFlag(unsigned long ulEquipeAxisArg, long *lValueArg)
{
    int iCardNum, iFileType;
<<<<<<< HEAD
    unsigned uGalilAxes;
    int rc;
    char caCommand[20];
    char caResp[80];

=======
    unsigned uGalilAxes;
    int rc;
    char caCommand[20];
    char caResp[80];

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Set the home flag values by axis by getting them from the
     * array and setting the ulHomed bit pattern appropriately. */
    if (ulEquipeAxisArg&RO_AXIS_T)
    {
        if (lValueArg[0]) ulHomed |= RO_AXIS_T;
        else ulHomed &= ~RO_AXIS_T;
    }
    if (ulEquipeAxisArg&RO_AXIS_R)
    {
        if (lValueArg[1]) ulHomed |= RO_AXIS_R;
        else ulHomed &= ~RO_AXIS_R;
    }
    if (ulEquipeAxisArg&RO_AXIS_Z)
    {
        if (lValueArg[2]) ulHomed |= RO_AXIS_Z;
        else ulHomed &= ~RO_AXIS_Z;
    }
    if (ulEquipeAxisArg&RO_AXIS_W)
    {
        if (lValueArg[3]) ulHomed |= RO_AXIS_W;
        else ulHomed &= ~RO_AXIS_W;
    }

    if (ulEquipeAxisArg&RO_AXIS_t)
    {
        if (lValueArg[4]) ulHomed |= RO_AXIS_t;
        else ulHomed &= ~RO_AXIS_t;
    }
    if (ulEquipeAxisArg&RO_AXIS_r)
    {
        if (lValueArg[5]) ulHomed |= RO_AXIS_r;
        else ulHomed &= ~RO_AXIS_r;
    }
    if (ulEquipeAxisArg&RO_AXIS_z)
    {
        if (lValueArg[6]) ulHomed |= RO_AXIS_z;
        else ulHomed &= ~RO_AXIS_z;
    }
    if (ulEquipeAxisArg&RO_AXIS_w)
    {
        if (lValueArg[7]) ulHomed |= RO_AXIS_w;
        else ulHomed &= ~RO_AXIS_w;
<<<<<<< HEAD
    }

    sprintf(caCommand,"VHMD = %d",(int)ulHomed);
    rc = GASendDMCCommand(ghDMC, caCommand, caResp, MAXGASTR);
=======
    }

    sprintf(caCommand,"VHMD = %d",(int)ulHomed);
    rc = GASendDMCCommand(ghDMC, caCommand, caResp, MAXGASTR);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

//    SSSetStatusWord(HOME_NOT_EXED, (int)(~ulHomed & ulAllAMFlag));

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROAbortMotion
 *
 * Abstract:    Aborts motion on all axes with sudden deceleration.
 *      It also clears any pending movements from the action buffer
 *      and sets the homing, servo, and after-motion flags to
 *      "motion terminated" conditions.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROAbortMotion(void)
{
    int iReturn;
<<<<<<< HEAD
    int rc;
    char caTempCmd[20];
    char caResp[80];

	giMapTest = 0;		// mapping IO stopped
	giMapIORunning = 0;
=======
    int rc;
    char caTempCmd[20];
    char caResp[80];

	giMapTest = 0;		// mapping IO stopped
	giMapIORunning = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* The Abort Motion command de-servos the system. So we have to activate the brake. */
    if (!iPreOnly)
        ROServiceBrake(ROBOTFILE, FALSE);
    /* Send it to both cards if there are 2 in the system. */
    if (iNumGalilCards >= 2)
        ROServiceBrake(PREALIGNFILE, FALSE);

	/* if there is track : m.z.*/
	if(ROGetSpecialAxis(RO_TRACK))
		ROServiceBrake(AUXFILE, FALSE);

    /* Wait for brakes to engage. */
    TIDelay(20);

    /* Send the "AB" command to the Galil card. */
    if (!iPreOnly)
        iReturn = GASendAbortMotionCommand(GA_CARD_0, TRUE);
    /* Send it to both cards if there are 2 in the system. */
    if (iNumGalilCards >= 2)
        iReturn = GASendAbortMotionCommand(GA_CARD_1, TRUE);
    /* Wait for all motion to complete. */
    TIDelay(150);

    /* Abort any homing. */
    ulHoming = 0;
<<<<<<< HEAD
    sprintf(caTempCmd, "VHMG=%d",ulHoming);
    rc = GASendDMCCommand(ghDMC, caTempCmd, caResp, MAXGASTR);

=======
    sprintf(caTempCmd, "VHMG=%d",ulHoming);
    rc = GASendDMCCommand(ghDMC, caTempCmd, caResp, MAXGASTR);

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Indicate that servo is off and motion is complete on all system axes. */
    ulAMFlag = ulAllAMFlag;
    ulServoFlag = ulAllAMFlag;
    SSSetStatusWord(SERVO_OFF, (int)(ulServoFlag & ulAllAMFlag));
    /* Update the limit switch variables. */
    if (!iPreOnly)
        ROCheckSwitches(GA_CARD_0);
    if (iNumGalilCards >= 2)
        ROCheckSwitches(GA_CARD_1);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROEnableSCurveProfile
 *
 * Abstract:    Tells the motion control card to enable or disable
 *      s-curve profile moves. This TA command complements the RTA command.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lOnOffArg           (in) Whether to turn it on or off (TRUE/FALSE)
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROEnableSCurveProfile(unsigned long ulEquipeAxisArg, long *lOnOffArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Not allowed while moving. */
    if ((ulAMFlag&ulEquipeAxisArg) != ulEquipeAxisArg)
        return FAILURE;

    /* Set the s-curve profile with the Galil "TA" command. */
    iReturn = GASetValsLong(iCardNum, SCURVE_COMMAND, uGalilAxes, lOnOffArg);

/* This is used to re-enable the s-curve profile after the LMM interrupt
 * has shut it off. It's brother is inside the ME_PROC.
    if (iReturn == SUCCESS) uSCurveFlag |= uGalilAxes;*/

    return iReturn;
}

/****************************************************************
 *
 * Function:    ROSetITValue
 *
<<<<<<< HEAD
 * Abstract:    Tells the motion control card to set IT. 
=======
 * Abstract:    Tells the motion control card to set IT. 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 *				This IT command complements the RIT command.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lOnOffArg           (in) Whether to turn it on or off (TRUE/FALSE)
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetITValue(unsigned long ulEquipeAxisArg, long *lValueArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Not allowed while moving. */
    if ((ulAMFlag&ulEquipeAxisArg) != ulEquipeAxisArg)
        return FAILURE;

    /* Set the s-curve profile with the Galil "IT" command. */
    iReturn = GASetValsLong(iCardNum, INDEP_TIME_COMMAND, uGalilAxes, lValueArg);

/* This is used to re-enable the s-curve profile after the LMM interrupt
 * has shut it off.
    if (iReturn == SUCCESS) uSCurveFlag |= uGalilAxes;*/

    return iReturn;
}

/****************************************************************
 *
 * Function:    ROGetResponseString
 *
 * Abstract:    Returns a response string indicating the states
 *      of motion, error, homing, and servo for all specified axes.
 *      There should be 2 sets of return codes: one for Brooks
 *      emulation and another for the normal Equipe command set.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      iRespErrFlagArg     (in) A word containing what information
 *                              is requested
 *      iErrRespArg         (in) The response word
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: This command only works over T, R, Z, and W axes, NOT
 *      for t, r, z, or w axes.
 *
 ***************************************************************/
int ROGetResponseString(unsigned long ulEquipeAxisArg, int iRespErrFlagArg, int *iErrRespArg)
{
    unsigned long ulMEAxisT = RO_AXIS_T;
    unsigned long ulMEAxisR = RO_AXIS_R;
    unsigned long ulMEAxisZ = RO_AXIS_Z;
    unsigned long ulMEAxisW = RO_AXIS_W;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Initialize the return code to "no failures". */
    *iErrRespArg = 0;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Assign proper axis constants to work for both TRZW and trzw. */
    if ((ulEquipeAxisArg & (unsigned long)RO_AXIS_t) || (ulEquipeAxisArg & (unsigned long)RO_AXIS_r) ||
        (ulEquipeAxisArg & (unsigned long)RO_AXIS_z) || (ulEquipeAxisArg & (unsigned long)RO_AXIS_w))
    {
        ulMEAxisT = RO_AXIS_t;
        ulMEAxisR = RO_AXIS_r;
        ulMEAxisZ = RO_AXIS_z;
        ulMEAxisW = RO_AXIS_w;
    }

    /* First bit checks if the axis is currently in motion. */
    if (iRespErrFlagArg & 0x01)
    {
        /* Checks if any one of the requested axes are in motion.
         * If none are in motion it just skips to the next check. */
        if (~ulAMFlag & ulEquipeAxisArg)
        {
            /* Initialize the response code for an axis still in motion.
             * Then tailor the response for the axis that is still in motion. */
            if (iEmulator & DFEMULATORB) *iErrRespArg = 220;

            /* If the T axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisT)
            {
                /* ...check if it is the one that is still in motion. */
                if (~ulAMFlag & ulMEAxisT)
                {
                    /* Brooks error code 220. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 0;
                        return FAILURE;
                    }
                    /* Equipe error code 1. */
                    else
                        *iErrRespArg |= 0x01;
                }
            }

            /* If the R axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisR)
            {
                /* ...check if it is the one that is still in motion. */
                if (~ulAMFlag & ulMEAxisR)
                {
                    /* Brooks error code 221. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 1;
                        return FAILURE;
                    }
                    /* Equipe error code 1. */
                    else
                        *iErrRespArg |= 0x01;
                }
            }

            /* If the Z axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisZ)
            {
                /* ...check if it is the one that is still in motion. */
                if (~ulAMFlag & ulMEAxisZ)
                {
                    /* Brooks error code 222. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 2;
                        return FAILURE;
                    }
                    /* Equipe error code 1. */
                    else
                        *iErrRespArg |= 0x01;
                }
            }

            /* If the W axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisW)
            {
                /* ...check if it is the one that is still in motion. */
                if (~ulAMFlag & ulMEAxisW)
                {
                    /* Brooks error code 223. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 3;
                        return FAILURE;
                    }
                    /* Equipe error code 1. */
                    else
                        *iErrRespArg |= 0x01;
                }
            }
        }
    }

    /* Second bit checks if the axis is currently in an error state. */
    if (iRespErrFlagArg & 0x02)
    {
        /* Checks if any one of the requested axes are in an error state.
         * If none are in an error state it just skips to the next check. */
        if (ulERFlag & ulEquipeAxisArg)
        {
            /* Initialize the response code for an axis in an error state.
             * Then tailor the response for the axis that is in an error state. */
            if (iEmulator & DFEMULATORB) *iErrRespArg = 204;

            /* If the T axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisT)
            {
                /* ...check if it is the one that is in an error state. */
                if (ulERFlag & ulMEAxisT)
                {
                    /* Brooks error code 204. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 0;
                        return FAILURE;
                    }
                    /* Equipe error code 2. */
                    else
                        *iErrRespArg |= 0x02;
                }
            }

            /* If the R axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisR)
            {
                /* ...check if it is the one that is in an error state. */
                if (ulERFlag & ulMEAxisR)
                {
                    /* Brooks error code 205. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 1;
                        return FAILURE;
                    }
                    /* Equipe error code 2. */
                    else
                        *iErrRespArg |= 0x02;
                }
            }

            /* If the Z axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisZ)
            {
                /* ...check if it is the one that is in an error state. */
                if (ulERFlag & ulMEAxisZ)
                {
                    /* Brooks error code 206. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 2;
                        return FAILURE;
                    }
                    /* Equipe error code 2. */
                    else
                        *iErrRespArg |= 0x02;
                }
            }

            /* If the W axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisW)
            {
                /* ...check if it is the one that is in an error state. */
                if (ulERFlag & ulMEAxisW)
                {
                    /* Brooks error code 207. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 3;
                        return FAILURE;
                    }
                    /* Equipe error code 2. */
                    else
                        *iErrRespArg |= 0x02;
                }
            }
        }
	}

    /* Third bit checks if the axis is currently not homed. */
    if (iRespErrFlagArg & 0x04)
    {
        /* Checks if any one of the requested axes are not homed.
         * If all are homed it just skips to the next check. */
        if (~ulHomed & ulEquipeAxisArg)
        {
            /* Initialize the response code for an axis that is not homed.
             * Then tailor the response for the axis that is not homed. */
            if (iEmulator & DFEMULATORB) *iErrRespArg = 201;

            /* If the T axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisT)
            {
                /* ...check if it is the one that is not homed. */
                if (~ulHomed & ulMEAxisT)
                {
                    /* Brooks error code 201. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 0;
                        return FAILURE;
                    }
                    /* Equipe error code 4. */
                    else
                        *iErrRespArg |= 0x04;
                }
            }

            /* If the R axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisR)
            {
                /* ...check if it is the one that is not homed. */
                if (~ulHomed & ulMEAxisR)
                {
                    /* Brooks error code 202. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 1;
                        return FAILURE;
                    }
                    /* Equipe error code 4. */
                    else
                        *iErrRespArg |= 0x04;
                }
            }

            /* If the Z axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisZ)
            {
                /* ...check if it is the one that is not homed. */
                if (~ulHomed & ulMEAxisZ)
                {
                    /* Brooks error code 203. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 2;
                        return FAILURE;
                    }
                    /* Equipe error code 4. */
                    else
                        *iErrRespArg |= 0x04;
                }
            }

            /* If the W axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisW)
            {
                /* ...check if it is the one that is not homed. */
                if (~ulHomed & ulMEAxisW)
                {
                    /* Brooks error code 204. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 3;
                        return FAILURE;
                    }
                    /* Equipe error code 4. */
                    else
                        *iErrRespArg |= 0x04;
                }
            }
        }
	}

    /* Fourth bit checks if the axis is currently servoed off. */
    if (iRespErrFlagArg & 0x08)
    {
        /* Checks if any one of the requested axes are servoed off.
         * If all are servoed on it just skips to the next check. */
        if (ulServoFlag & ulEquipeAxisArg)
        {
            /* Initialize the response code for an axis that is servoed off.
             * Then tailor the response for the axis that is servoed off. */
            if (iEmulator & DFEMULATORB) *iErrRespArg = 204;

            /* If the T axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisT)
            {
                /* ...check if it is the one that is servoed off. */
                if (ulServoFlag & ulMEAxisT)
                {
                    /* Brooks error code 204. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 0;
                        return FAILURE;
                    }
                    /* Equipe error code 8. */
                    else
                        *iErrRespArg |= 0x08;
                }
            }

            /* If the R axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisR)
            {
                /* ...check if it is the one that is servoed off. */
                if (ulServoFlag & ulMEAxisR)
                {
                    /* Brooks error code 205. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 1;
                        return FAILURE;
                    }
                    /* Equipe error code 8. */
                    else
                        *iErrRespArg |= 0x08;
                }
            }

            /* If the Z axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisZ)
            {
                /* ...check if it is the one that is servoed off. */
                if (ulServoFlag & ulMEAxisZ)
                {
                    /* Brooks error code 206. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 2;
                        return FAILURE;
                    }
                    /* Equipe error code 8. */
                    else
                        *iErrRespArg |= 0x08;
                }
            }

            /* If the W axis is one of the requested axes... */
            if (ulEquipeAxisArg & ulMEAxisW)
            {
                /* ...check if it is the one that is servoed off. */
                if (ulServoFlag & ulMEAxisW)
                {
                    /* Brooks error code 207. */
                    if (iEmulator & DFEMULATORB)
                    {
                        *iErrRespArg += 3;
                        return FAILURE;
                    }
                    /* Equipe error code 8. */
                    else
                        *iErrRespArg |= 0x08;
                }
            }
        }
    }

    /* One or more status on requested axes are BAD. */
    if (*iErrRespArg) return FAILURE;

    /* All status on all requested axes are OK. */
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROReadLatchedPosition
 *
 * Abstract:    Read robot latched position. This is mainly used by the
 *      scanner module. When a scanner interrupt registers, meaning
 *      there was a wafer detected, the Z position of that wafer
 *      must be immediately grabbed.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lPositionArg        (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadLatchedPosition(unsigned long ulEquipeAxisArg, long *lPositionArg)
{
    int iReturn;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if(ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Read the position with the Galil "TP" command. */
    iReturn = GAGetValsLong(iCardNum, TELL_POSITION_COMMAND, uGalilAxes, laPosition);

    /* Scale the position from encoder counts to normal units. */
    ROScalePos(ulEquipeAxisArg, laPosition, lPositionArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROProcessMotion
 *
 * Abstract:    Stops motion, Servo on/off on the selected axis.
 *      Combines ROStopMotion, MeServoMCOn, & ROServoMCOff (CODE-OPTIMIZATION)
 *      If no operand
 *      on the command line, all available axes are stopped. This
 *      is done by calling the ROStopMotionLocal function,
 *      the meat of the STOP command, 4 times with axes A,
 *      W, a, and w.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *      iFlagArg                (in) 0=Stop motion
 *                                   1=Servo On
 *                                   2=Servo Off
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: This assumes that A is T, R, and Z axes and a is t, r, and z axes.
 *
 ***************************************************************/
int ROProcessMotion(unsigned long ulEquipeAxisArg, int iFlagArg)
{
    int iCount;                 /* Temporary variable to count through a select number of axes. */
    unsigned long ulEquipeMask; /* The bit pattern for the assembled Equipe axes. */
    //int iCounterIndex;

    if (ulEquipeAxisArg != 0)
    {
        if (iFlagArg == 0)
        {
            /* Stop motion on the specified axis. */
            if (ROStopMotionLocal(ulEquipeAxisArg) == FAILURE)
                return FAILURE;
        }
        else if (iFlagArg == 1)
        {
            /* Servo On on the specified axis */
            if (ROServoMCOnLocal(ulEquipeAxisArg) == FAILURE)
                return FAILURE;
        }
        else
        {
            /* Servo Off on the specified axis */
            if (ROServoMCOffLocal(ulEquipeAxisArg) == FAILURE)
                return FAILURE;
        }
    }
    else
    {
        /* This part checks for a robot and it's valid axes names, i.e. A. */
        ulEquipeMask = 0;
        for (iCount=0; iCount<3; iCount++)
        {
            /* If the T, R, and Z axes exist, compose the bit pattern for A. */
            if (ROGetMappedAxis(1<<iCount, GALIL_AXIS) != 0)
                ulEquipeMask |= 1<<iCount;
        }
        /* Stop motion for the A (all robot) axes. */
        if (ulEquipeMask != 0)
        {
            if (iFlagArg == 0)
            {
                /* Stop motion on the specified axis. */
                if (ROStopMotionLocal(ulEquipeMask) == FAILURE)
                    return FAILURE;
            }
            else if (iFlagArg == 1)
            {
                /* Servo On on the specified axis */
                if (ROServoMCOnLocal(ulEquipeMask) == FAILURE)
                    return FAILURE;
            }
            else
            {
                /* Servo Off on the specified axis */
                if (ROServoMCOffLocal(ulEquipeMask) == FAILURE)
                    return FAILURE;
            }
        }

        /* This part checks for a track or dual arm or other auxiliary
         * device on the first Galil card, i.e. W axis. */
        if (ROGetMappedAxis(RO_AXIS_W, GALIL_AXIS) != 0)
        {
            if (iFlagArg == 0)
            {
                /* Stop motion on the specified axis. */
                if (ROStopMotionLocal(RO_AXIS_W) == FAILURE)
                    return FAILURE;
            }
            else if (iFlagArg == 1)
            {
                /* Servo On on the specified axis */
                if (ROServoMCOnLocal(RO_AXIS_W) == FAILURE)
                    return FAILURE;
                if (ROServoMCOnLocal(RO_AXIS_W) == FAILURE)
                    return FAILURE;
            }
            else
            {
                /* Servo Off on the specified axis */
                if (ROServoMCOffLocal(RO_AXIS_W) == FAILURE)
                    return FAILURE;
            }
        }

        /* This part checks for a prealigner and it's valid axes names, i.e. a. */
        ulEquipeMask = 0;
        for (iCount=4; iCount<7; iCount++)
        {
            /* If the t, r, and z axes exist, compose the bit pattern for a. */
            if (ROGetMappedAxis(1<<iCount, GALIL_AXIS) != 0)
                ulEquipeMask |= 1<<iCount;
        }
        /* Stop motion for the a (all pre-aligner) axes. */
        if (ulEquipeMask != 0)
        {
            if (iFlagArg == 0)
            {
                /* Stop motion on the specified axis. */
                if (ROStopMotionLocal(ulEquipeMask) == FAILURE)
                    return FAILURE;
            }
            else if (iFlagArg == 1)
            {
                /* Servo On on the specified axis */
                if (ROServoMCOnLocal(ulEquipeMask) == FAILURE)
                    return FAILURE;
            }
            else
            {
                /* Servo Off on the specified axis */
                if (ROServoMCOffLocal(ulEquipeMask) == FAILURE)
                    return FAILURE;
            }
        }

        /* This part checks for a track or flipper or other auxiliary
         * device on the second Galil card, i.e. w axis. */
        if (ROGetMappedAxis(RO_AXIS_w, GALIL_AXIS) != 0)
        {
            if (iFlagArg == 0)
            {
                /* Stop motion on the specified axis. */
                if (ROStopMotionLocal(RO_AXIS_w) == FAILURE)
                    return FAILURE;
            }
            else if (iFlagArg == 1)
            {
                /* Servo On on the specified axis */
                if (ROServoMCOnLocal(RO_AXIS_w) == FAILURE)
                    return FAILURE;
            }
            else
            {
                /* Servo Off on the specified axis */
                if (ROServoMCOffLocal(RO_AXIS_w) == FAILURE)
                    return FAILURE;
            }
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROStopMotion
 *
 * Abstract:    Stops motion on the selected axis. If no operand
 *      on the command line, all available axes are stopped. This
 *      is done by calling the ROStopMotionLocal function,
 *      the meat of the STOP command, 4 times with axes A,
 *      W, a, and w.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: This assumes that A is T, R, and Z axes and a is t, r, and z axes.
 *
 ***************************************************************/
int ROStopMotion(unsigned long ulEquipeAxisArg)
{
//    unsigned long ulEquipeMask; /* The bit pattern for the assembled Equipe axes. */
//    int iCounterIndex;
<<<<<<< HEAD
//    struct timespec tv;
=======
//    struct timespec tv;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* Do stop motion, 2nd argument=0 */
    if (ROProcessMotion(ulEquipeAxisArg, 0) == FAILURE)
        return FAILURE;
/*
    iCounterIndex = TIGetCounter();
    TISetCounter(iCounterIndex, 100);
    if (ulEquipeAxisArg != 0)
        ulEquipeMask = ulEquipeAxisArg;
    else
<<<<<<< HEAD
        ulEquipeMask = ulAllAMFlag;
    tv.tv_sec = 0;
    tv.tv_nsec = 10000;
    while (!TICountExpired(iCounterIndex) && ((ulAMFlag&ulEquipeMask) != ulEquipeMask))
    {
	ROUpdateTS(FALSE);
	TIRefreshWD();
	nanosleep(&tv, NULL);
=======
        ulEquipeMask = ulAllAMFlag;
    tv.tv_sec = 0;
    tv.tv_nsec = 10000;
    while (!TICountExpired(iCounterIndex) && ((ulAMFlag&ulEquipeMask) != ulEquipeMask))
    {
	ROUpdateTS(FALSE);
	TIRefreshWD();
	nanosleep(&tv, NULL);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    }
    TIReturnCounter(iCounterIndex);
*/
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROStopMotionLocal
 *
 * Abstract:    Stops motion on the selected axis.
 *      We must give special attention to setting AM_flag since
 *      some axes may be specified as moving not because they are
 *      actually moving, but rather because they have actions
 *      pending in the action buffer and will be moving soon.
 *      However, if the action buffer is cleared, these axes will
 *      never get an end of motion interrupt and so their AM_flag
 *      bits will never be reset.  We interrogate the galil directly
 *      to determine which axes fall into this category.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: This function does NOT check for FAILUREs returned.
 *      The STOP function is too critical to error out of.
 *
 ***************************************************************/
int ROStopMotionLocal(unsigned long ulEquipeAxisArg)
{
    unsigned long ulAMFlagTemp;     /* Unsigned long version of the ulAMFlag. */
    int iCardNum, iFileType;
    unsigned uGalilAxes;
    int iNoAxes, iShifter;  //, iAxis;  /* Temporary variables. */
    int iStopMotionComplete = 0;
<<<<<<< HEAD
    long lCurrSwitches[8];          /* The limit switch values returned from the Galil card. */
    char caTempCmd[20];
    int rc;
    char caResp[80];


//    struct timespec tv;

//int i=0;
//    tv.tv_sec = 0;
//    tv.tv_nsec = 200000000;

	giMapTest = 0;		// mapping IO stopped
	giMapIORunning = 0;

=======
    long lCurrSwitches[8];          /* The limit switch values returned from the Galil card. */
    char caTempCmd[20];
    int rc;
    char caResp[80];


//    struct timespec tv;

//int i=0;
//    tv.tv_sec = 0;
//    tv.tv_nsec = 200000000;

	giMapTest = 0;		// mapping IO stopped
	giMapIORunning = 0;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Initialize locally used values to first Galil card and no limit switches trigered. */
    iShifter = 0;
    iNoAxes = 0;
    lCurrSwitches[0] = lCurrSwitches[1] = lCurrSwitches[2] = lCurrSwitches[3] = 0;
    lCurrSwitches[4] = lCurrSwitches[5] = lCurrSwitches[6] = lCurrSwitches[7] = 0;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Stop motion with the Galil "ST" command. */
    GASendAxesCommand(iCardNum, STOP_MOTION_COMMAND, uGalilAxes);
    /* Convert the unsigned to an unsigned long. This explicit conversion
     * was necessary because without it the compiler causes a bug. */
    ulAMFlagTemp = ulAMFlag;
    /* If the axis is in motion acknowledge that motion was aborted. */
    if (~ulAMFlagTemp & ulEquipeAxisArg)
        uAbortAxis = (unsigned)(ulEquipeAxisArg);
    else
        uAbortAxis = 0;

    /* Abort any homing. */
    ulHoming &= ~ulEquipeAxisArg;
<<<<<<< HEAD

    sprintf(caTempCmd, "VHMG=%d",ulHoming);
    rc = GASendDMCCommand(ghDMC, caTempCmd, caResp, MAXGASTR);


    iNoAxes = GAGetGLNumAxis(iCardNum);
/*
    while (!iStopMotionComplete)
    {
//        if (GAGetValsLong(iCardNum, TELL_SWITCHES_COMMAND, (unsigned)(iNoAxes), lCurrSwitches) == FAILURE)
        if (GAGetValsLong(iCardNum, TELL_SWITCHES_COMMAND, uGalilAxes, lCurrSwitches) == FAILURE)
             return FAILURE;
++i;
=======

    sprintf(caTempCmd, "VHMG=%d",ulHoming);
    rc = GASendDMCCommand(ghDMC, caTempCmd, caResp, MAXGASTR);


    iNoAxes = GAGetGLNumAxis(iCardNum);
/*
    while (!iStopMotionComplete)
    {
//        if (GAGetValsLong(iCardNum, TELL_SWITCHES_COMMAND, (unsigned)(iNoAxes), lCurrSwitches) == FAILURE)
        if (GAGetValsLong(iCardNum, TELL_SWITCHES_COMMAND, uGalilAxes, lCurrSwitches) == FAILURE)
             return FAILURE;
++i;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("i = %d TS: %d %d %d %d %d %d %d %d\n", i, lCurrSwitches[0], lCurrSwitches[1],lCurrSwitches[2],lCurrSwitches[3],lCurrSwitches[4],lCurrSwitches[5],lCurrSwitches[6],lCurrSwitches[7]);
        iStopMotionComplete = 1;
        if (uGalilAxes & RO_AXIS_T)
            if (lCurrSwitches[0] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_R)
            if (lCurrSwitches[1] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_Z)
            if (lCurrSwitches[2] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_W)
            if (lCurrSwitches[3] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_t)
            if (lCurrSwitches[4] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_r)
            if (lCurrSwitches[5] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_z)
            if (lCurrSwitches[6] & INMOTION)
                iStopMotionComplete = 0;
        if (uGalilAxes & RO_AXIS_w)
            if (lCurrSwitches[7] & INMOTION)
                iStopMotionComplete = 0;
<<<<<<< HEAD
	nanosleep(&tv, NULL);
    }
*/
=======
	nanosleep(&tv, NULL);
    }
*/
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROServoMCOff
 *
 * Abstract:    Servo's off the selected axis. If no operand
 *      on the command line, all available axes are servoed off. This
 *      is done by calling the ROServoMCOffLocal function,
 *      the meat of the SVOF command, 4 times with axes A,
 *      W, a, and w.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROServoMCOff(unsigned long ulEquipeAxisArg)
{
    /* Do Servo Off, 2nd argument=2 */
    if (ROProcessMotion(ulEquipeAxisArg, 2) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROServoMCOffLocal
 *
 * Abstract:    Turns off the servo for the selected axis.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROServoMCOffLocal(unsigned long ulEquipeAxisArg)
{
    long lZAxisBrake[8] = {0,0,0,0,0,0,0,0};  /* The datafile Z brake values. */
    unsigned uErr;
    int iRet, iVacIndex;
    int iCardNum, iFileType;
    unsigned uGalilAxes;
// This routine may need to be updated for servo flipper/z-brake interaction

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;
    if ((ulAMFlag&ulEquipeAxisArg) != ulEquipeAxisArg)
        return FAILURE;

    /* Turn on the brake on the Z axis. */
    if (ulEquipeAxisArg & RO_AXIS_Z)
    {
        if ((iRet=ROServiceBrake(ROBOTFILE, FALSE)) == FAILURE) goto exit_point;
        /* Wait for brakes to engage. */
        TIDelay(150);
    }

    /* if there is track : m.z.*/
    if(ulEquipeAxisArg & ROGetSpecialAxis(RO_TRACK))
    {
	ROServiceBrake(AUXFILE, FALSE);
        TIDelay(150);
    }

    /* Send it to both cards if there are 2 in the system. */
//    else if (ulEquipeAxisArg & (RO_AXIS_r|RO_AXIS_z))
//    {
        /* Get the brake values from the datafile. */
//        if ((iRet=FIOGetParamVals(PREALIGNFILE, H_VALUE, lZAxisBrake)) == FAILURE) goto exit_point;
//        for (iVacIndex=1; iVacIndex<=2; iVacIndex++)
//        {
            /* If a brake activation is required... */
//            if ((lZAxisBrake[iVacIndex] > 0) && (ulEquipeAxisArg & (1<<(iVacIndex+4))))
//            {
                /* ...activate the brake. */
                /* WARNING: In ME domain card number 1 is GA_CARD_1. */
//                ROWriteGalilPort(1, (int)lZAxisBrake[iVacIndex]-40, 0);
//            }
//        }
        /* Wait for brakes to engage. */
//        TIDelay(150);
//    }

    /* Servo off with the Galil "MO" command. */
    iRet = GASendAxesCommand(iCardNum, MOTOR_OFF_COMMAND, uGalilAxes);

    /* See if there was an error sending the motor off command. */
    uErr = GAGetGalilErrorFlag();
    /* If there was no error... */
    if (!uErr)
    {
        /* ...mark axis as servoed off and motion complete. */
        ulServoFlag |= ulEquipeAxisArg;
        ulAMFlag |= ulEquipeAxisArg;
    }

exit_point:

    /* If there was error sending the motor off command. */
    if (uErr)
        return FAILURE;

    /* If the servos were turned off successfully, set the status word to reflect that. */
    SSSetStatusWord(SERVO_OFF, (int)(ulServoFlag & ulAllAMFlag));

    return iRet;
}


/****************************************************************
 *
 * Function:    ROServoMCOn
 *
 * Abstract:    Servo's on the selected axis. If no operand
 *      on the command line, all available axes are servoed on. This
 *      is done by calling the ROServoMCOnLocal function,
 *      the meat of the SVON command, 4 times with axes A,
 *      W, a, and w.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROServoMCOn(unsigned long ulEquipeAxisArg)
{
    /* Do Servo On, 2nd argument=1 */
    if (ROProcessMotion(ulEquipeAxisArg, 1) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROServoMCOnLocal
 *
 * Abstract:    Turns on the servo for the selected axis.
 *
 * Parameters:
 *      ulEquipeAxisArg         (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROServoMCOnLocal(unsigned long ulEquipeAxisArg)
{
<<<<<<< HEAD
//    long lMotorType[8] = {-1,-1,-1,-1,-1,-1,-1,-1}; 
    long lMotorType[8] = {1,1,1,-1,1,1,1,-1}; 
=======
//    long lMotorType[8] = {-1,-1,-1,-1,-1,-1,-1,-1}; 
    long lMotorType[8] = {1,1,1,-1,1,1,1,-1}; 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
									/* Set the motor type, i.e. positive amp reference voltage
                                     * turns the motor clockwise or counter-clockwise. */
    long lZAxisBrake[8] = {0,0,0,0,0,0,0,0};  /* The datafile Z brake values. */
    unsigned uErr;
    int iRet, iVacIndex;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    if ((ulAMFlag&ulEquipeAxisArg) != ulEquipeAxisArg)
        return FAILURE;

    /* EMO was set or robot power cable was disconnected. */
    if (iEMOWasSet || iPowerDisconnected)
    {
        if ((iRet=ROServiceBrake(ROBOTFILE, FALSE)) == FAILURE) goto exit_point;
		TIDelay(30);
        /* If the robot power cable was disconnected... */
        if (iPowerDisconnected)
        {
            /* Tie up loose ends by de-servoing before re-servoing. */
            if (GASendAxesCommand(iCardNum, MOTOR_OFF_COMMAND, uGalilAxes) == FAILURE)
                return FAILURE;
            iPowerDisconnected = FALSE;
        }
        /* If the EMO was pressed, the amps have to be reinitialized. But this process will not
         * repeat because the iEMOWasSet flag is reset a few lines later. */
        if (GAAmpInit() == FAILURE)
            return FAILURE;
        if (TIDelay(30) == FAILURE)
            return FAILURE;
    }
<<<<<<< HEAD

	iInputG = inb(IO_ROBOT_INPUT_G);
//	if (iInputG & (EMO | LOOP_CHECK | POWER_CHECK) ) goto exit_point;
	if (iInputG & (LOOP_CHECK | POWER_CHECK) ) goto exit_point;
=======

	iInputG = inb(IO_ROBOT_INPUT_G);
//	if (iInputG & (EMO | LOOP_CHECK | POWER_CHECK) ) goto exit_point;
	if (iInputG & (LOOP_CHECK | POWER_CHECK) ) goto exit_point;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* Initialize the amplifiers. This takes 200 to 300ms with older amps. It only takes
     * 170 ms on newer solid state amps. A 350ms wait is safe for the amps to juice up.
     * But only do it if the amplifiers are not already powered up. Otherwise the power
     * cycle causes servo fluctuations. */
    if (inb(IO_ROBOT_INPUT_G) || (inb(IO_ROBOT_OUTPUT_B) != 0xFF))
    {
        if (GAAmpInit() == FAILURE)
            return FAILURE;
        if (TIDelay(30) == FAILURE)
            return FAILURE;
    }

    /* Check internal power supply statuses. */
    if (iCardNum == GA_CARD_0) iRet = inb(IO_ROBOT_INPUT_G);
    else if (iCardNum == GA_CARD_1) iRet = inb(IO_PRE_INPUT_G);
    else iRet = inb(IO_ROBOT_INPUT_G) | inb(IO_PRE_INPUT_G);

<<<<<<< HEAD
    /* Error out on a power supply failure. */
//  WE NEED THIS CODE BACK WHEN WE IMPLEMENT INPUT G I/O
=======
    /* Error out on a power supply failure. */
//  WE NEED THIS CODE BACK WHEN WE IMPLEMENT INPUT G I/O
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    iRet = 0;
    if (iRet & 0xFF)
        return FAILURE;

    /* If the Emergency Motor Off had been pressed, set it back to normal operation. */
    iEMOWasSet = FALSE;

    /* Turn on servo for specified axes */

    /* Set the motor type (whether positive amp reference voltage turns the motor
     * clockwise or counter-clockwise) and then turn on the PID servo mechanism. */
    /* WARNING: W axis may have reversed polarity for some systems. */
//    lMotorType[0]=lMotorType[1]=lMotorType[2]=lMotorType[3]=-1;
//    if (ulEquipeAxisArg == (ROGetSpecialAxis(RO_FLIPPER)))
//        lMotorType[3] = 2;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    iRet = SUCCESS;
    /* Set the motor type with the Galil "MT" command. */
//    if (iRet == SUCCESS)
//        iRet = GASetValsLong(iCardNum, MOTOR_TYPE_COMMAND, uGalilAxes, lMotorType);
    /* Servo on with the Galil "SH" command. */
    if (iRet == SUCCESS)
        iRet = GASendAxesCommand(iCardNum, SERVO_ON_COMMAND, uGalilAxes);

    /* Turn off the brake on the Z axis */
    if (ulEquipeAxisArg & RO_AXIS_Z)
<<<<<<< HEAD
    {
=======
    {
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if ((iRet=ROServiceBrake(ROBOTFILE, TRUE)) == FAILURE) goto exit_point;
    }

    /* if there is track : m.z.*/
    if(ulEquipeAxisArg & ROGetSpecialAxis(RO_TRACK))
	ROServiceBrake(AUXFILE, TRUE);

    /* Send it to both cards if there are 2 in the system. */
//    else if (ulEquipeAxisArg & (RO_AXIS_r|RO_AXIS_z))
//    {
        /* Get the brake values from the datafile. */
//        if ((iRet=FIOGetParamVals(PREALIGNFILE, H_VALUE, lZAxisBrake)) == FAILURE) goto exit_point;
//        for (iVacIndex=1; iVacIndex<=2; iVacIndex++)
//        {
            /* If a brake activation is required... */
//            if ((lZAxisBrake[iVacIndex] > 0) && (ulEquipeAxisArg & (1<<(iVacIndex+4))))
//            {
                /* ...activate the brake. */
                /* WARNING: In ME domain card number 1 is GA_CARD_1. */
//                ROWriteGalilPort(1, (int)lZAxisBrake[iVacIndex]-40, 1);
//            }
//        }
//    }

    /* See if there was an error sending the servo here command. */
    uErr = GAGetGalilErrorFlag();
    /* If there was no error... */
    if (!uErr)
    {
        /* ...mark axis as servoed on, motion complete, and no errors. */
        ulServoFlag &= ~ulEquipeAxisArg;
        ulERFlag &= ~ulEquipeAxisArg;
        ulAMFlag |= ulEquipeAxisArg;
	}

    /* Enable interrupts on the Galil card. Without this, the Galil card won't
     * generate any interrupts. */
//    if (iCardNum == GA_CARD_0 || iCardNum == GA_CARD_1)
//    {
//        /* iRet isn't set to the return value of this function because we want
//         * to preserve iRet from the GASetMotorType and GASendServoHereCommand functions. */
//		iRet = GASendReceiveGalil(iCardNum, (char *)"EI 255,255\xD", cpNull);
//    }

exit_point:

    MASetMacroIntegerVars(MR_ERR_INTERRUPT, FALSE);

    /* If there was error sending the motor off command. */
    if (uErr)
        return FAILURE;

    /* If the servos were turned on successfully, set the status word to reflect that. */
    SSSetStatusWord(MOT_ERROR, (int)(ulERFlag & ulAllAMFlag));
    SSSetStatusWord(SERVO_OFF, (int)(ulServoFlag & ulAllAMFlag));

    return iRet;
}


/****************************************************************
 *
 * Function:    ROIsAxisMotionInProgress
 *
 * Abstract:    Finds out whether or not an axis is currently
 *      in motion. This checks the in-position-window. So this
 *      may return true without an interrupt occurring. It's
 *      used for time-optimal-trajectories (TOT) where the motion complete
 *      flag returns true but the robot is still some distance away
 *      from the actual target position. So the next move can be
 *      smoothly blended into the last move. This is used in conjuction
 *      with SIPW and RIPW host commands.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROIsAxisMotionInProgress(unsigned long ulEquipeAxisArg, int iTOTFlagArg, int *iKeepCheckingArg)
{
    char caInMotionMsg[50], caInMotionResp[50];
<<<<<<< HEAD
    char *caXYZW[8] = {"A", "B", "C", "D", "E", "F", "G", "H" };
=======
    char *caXYZW[8] = {"A", "B", "C", "D", "E", "F", "G", "H" };
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    char caMTCRcommand[10] = "MG VMTC";
    int i, iReturn;
    unsigned long ulAMFlagTemp;
    int iCardNum, iFileType;
    unsigned uGalilAxes;
<<<<<<< HEAD
    int iAxisCnt;
    int k, m;
=======
    int iAxisCnt;
    int k, m;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* Validate that the axis exists in the system. */
//    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
//        return FAILURE;

    *iKeepCheckingArg = TRUE;

    /* Logic is changed -- CKK */
    /* It will wait until AMFlag, then continue to poll galil only if TOTFlag is on */

    /* TOT for wait for all axes or single axis WITHOUT the ,1 option.
     * Note: ulAMFlag is updated by galil interrupt saying the axis is within
     *       the in-position window. */
    /* Convert the unsigned to an unsigned long. This explicit conversion
     * was necessary because without it the compiler causes a bug.
     * If the motion is complete on the selcted axis, return TRUE if TOTFlag is not ON,
     * otherwise, continue to polling galil for motion complete. */
<<<<<<< HEAD

    // check if motion complete by IP and not forced "motion complete"
    // ulAMIPFlag is updated by IP
    // 3 conditions: (1) wmo not forced, (2) ipw set, (3) amIPflag set
    ROUpdateTP(0);

    iAxisCnt = 0;

    if (!iTOTFlagArg)
    {
	for (i=0; i<4; ++i)
	{
	    if((ulEquipeAxisArg & (1<<i)) == (1<<i) )
	    {
//printf("wmo chk amipflag=%d axis=%d i=%d\n",ulAMIPFlag, ulEquipeAxisArg, i);
		if(giIPWFlag[i] == 0)
		    goto keep_check;
//k=ulAMIPFlag & (1<<i);
//m=1<<i;
//printf("wmo ch2 k=%d m=%d\n",k, m);
		if((ulAMIPFlag & (1<<i)) != (1<<i))
		{
//printf("wmo not amipflag=%d axis=%d i=%d\n",ulAMIPFlag, ulEquipeAxisArg, i);
		    goto keep_check;
		}
	    }
	    else
	    {
		++iAxisCnt;
	    }
	}

	if(iAxisCnt >= 4) 
	    goto keep_check;

//printf("wmo amipflag=%d axis=%d i=%d\n",ulAMIPFlag, ulEquipeAxisArg, i);
    	*iKeepCheckingArg = FALSE;
	return SUCCESS;
    }

keep_check:

    // check motion complete flag, just return until it's complete
    // AM flag is updated by TS
    //
    //ulAMFlagTemp = ulAMFlag;
    if (~ulAMFlag & ulEquipeAxisArg)
    {
////printf("k1 - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
        return SUCCESS;
    }

    // check if MTCR is running. if it is, just return until it's complete
    iReturn = GASendDMCCommand(ghDMC, caMTCRcommand, caInMotionResp, 50);
    i = atoi(caInMotionResp);
    if (i) return SUCCESS;


    // We'll check if the motion is really completed (not just in the in-position window).
    // The axis specified can be any of T,R,Z,W,t,r,z,w,A,or a.

    if(iTOTFlagArg) // force WMO from Galil TS
    {
	ROUpdateTS(0);
	if (~ulAMFlag & ulEquipeAxisArg)
    	{
//printf("force WMO not complete - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
             return SUCCESS;
        }
//printf("force WMO complete - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
    }
=======

    // check if motion complete by IP and not forced "motion complete"
    // ulAMIPFlag is updated by IP
    // 3 conditions: (1) wmo not forced, (2) ipw set, (3) amIPflag set
    ROUpdateTP(0);

    iAxisCnt = 0;

    if (!iTOTFlagArg)
    {
	for (i=0; i<4; ++i)
	{
	    if((ulEquipeAxisArg & (1<<i)) == (1<<i) )
	    {
//printf("wmo chk amipflag=%d axis=%d i=%d\n",ulAMIPFlag, ulEquipeAxisArg, i);
		if(giIPWFlag[i] == 0)
		    goto keep_check;
//k=ulAMIPFlag & (1<<i);
//m=1<<i;
//printf("wmo ch2 k=%d m=%d\n",k, m);
		if((ulAMIPFlag & (1<<i)) != (1<<i))
		{
//printf("wmo not amipflag=%d axis=%d i=%d\n",ulAMIPFlag, ulEquipeAxisArg, i);
		    goto keep_check;
		}
	    }
	    else
	    {
		++iAxisCnt;
	    }
	}

	if(iAxisCnt >= 4) 
	    goto keep_check;

//printf("wmo amipflag=%d axis=%d i=%d\n",ulAMIPFlag, ulEquipeAxisArg, i);
    	*iKeepCheckingArg = FALSE;
	return SUCCESS;
    }

keep_check:

    // check motion complete flag, just return until it's complete
    // AM flag is updated by TS
    //
    //ulAMFlagTemp = ulAMFlag;
    if (~ulAMFlag & ulEquipeAxisArg)
    {
////printf("k1 - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
        return SUCCESS;
    }

    // check if MTCR is running. if it is, just return until it's complete
    iReturn = GASendDMCCommand(ghDMC, caMTCRcommand, caInMotionResp, 50);
    i = atoi(caInMotionResp);
    if (i) return SUCCESS;


    // We'll check if the motion is really completed (not just in the in-position window).
    // The axis specified can be any of T,R,Z,W,t,r,z,w,A,or a.

    if(iTOTFlagArg) // force WMO from Galil TS
    {
	ROUpdateTS(0);
	if (~ulAMFlag & ulEquipeAxisArg)
    	{
//printf("force WMO not complete - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
             return SUCCESS;
        }
//printf("force WMO complete - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

//    for (i = 0; i < 8; i++) // for all the possible axes
//    {
//        /* Set up the interrogation command for each axis. */
//        caInMotionMsg[0] = caInMotionResp[0] = 0;
//        strcpy(caInMotionMsg, "MG _BG");
//        if (uGalilAxes & (1 << i))
//        {
//            strcat(caInMotionMsg, caXYZW[i]);
//            strcat(caInMotionMsg, "\r");
//            /* Send it to the appropriate Galil card and get a response. */
//            iReturn = GASendReceiveGalil(iCardNum, caInMotionMsg, caInMotionResp);
//
//            /* Make sure the read/write was kosher. */
//            if (iReturn == FAILURE)
//                return FAILURE; // just return SUCCESS instead FAILURE to continue WMO
//            if (strlen(caInMotionResp) != 19)
//                return FAILURE; // just return SUCCESS instead FAILURE to continue WMO
//
//            /* Decode the response and return the TOT motion complete status. */
//            if (caInMotionResp[10] == '1')
//            if (caInMotionResp[1] == '1')
//                return SUCCESS;
//        }
//    }

    /* Only when all the axes Motion complete are checked, WMO completes */
//printf("k0 - iTOT=%d AM=%d axis=%d\n",iTOTFlagArg, ulAMFlag, ulEquipeAxisArg);
    *iKeepCheckingArg = FALSE;
//    giTOTFlag = 0; // turn off TP update
    return SUCCESS;
}


/********** End of General Mechanism Functions **********/


/****************************************************************
 *
 * Function:    ROValidAxis
 *
 * Abstract:    Checks that the axes requested are present in
 *      this configuration. This requires all the knowledge of the
 *      possible system types. The ROInit routine just blindly
 *      stuffs an axis mapping. But this routine checks that
 *      axes requested are present and sets the appropriate
 *      MC and DF variables to do read/writes.
 *
 * Parameters:
 *      ulEquipeAxisArg (in) The Equipe axis designation (R, w, etc.)
 *      iCardNumArg     (out) The Galil card number to operate on
 *      iFileTypeArg    (out) The relevant parameter file
 *      uGalilAxesArg   (out) The Galil axes corresponding to the Equipe axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROValidAxis(unsigned long ulEquipeAxisArg, int *iCardNumArg, int *iFileTypeArg, unsigned *uGalilAxesArg)
{
    unsigned uMappedAxis;   /* The Galil axis corresponding a specific Equipe axis. */

    /* Check for a valid axis bit pattern. */
    if ((ulEquipeAxisArg>=256) || (ulEquipeAxisArg<=0))
        return FAILURE;

    /* Initialize variables. Get ready to put it all together. */
    *iCardNumArg = GA_BOTH_CARDS;
    *iFileTypeArg = -1;
    *uGalilAxesArg = 0;

    if (ulEquipeAxisArg & RO_AXIS_T)
    {
        /* If the card number already selected is different than that
         * associated with the axis, an invalid axes combination has
         * been requested. */
        if (*iCardNumArg == GA_CARD_1)
            return FAILURE;
        *iCardNumArg = GA_CARD_0;
        /* Find out what Galil axis the Equipe axis name maps to.
         * If the Equipe axis is not present in the system, 0 is returned. */
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_T, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        /* If the file type has already been determined, i.e. ROBOTFILE,
         * but an inconsistent axis has been requested, i.e. track,
         * return a FAILURE. If it is of the same type, i.e. still
         * ROBOTFILE, everything is fine. */
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_T, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_T, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_R)
    {
        if (*iCardNumArg == GA_CARD_1)
            return FAILURE;
        *iCardNumArg = GA_CARD_0;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_R, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_R, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_R, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_Z)
    {
        if (*iCardNumArg == GA_CARD_1)
            return FAILURE;
        *iCardNumArg = GA_CARD_0;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_Z, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_Z, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_Z, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_W)
    {
        if (*iCardNumArg == GA_CARD_1)
            return FAILURE;
        *iCardNumArg = GA_CARD_0;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_W, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_W, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_W, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_t)
    {
        if (iNumGalilCards < 2)
            return FAILURE;
        if (*iCardNumArg == GA_CARD_0)
            return FAILURE;
        *iCardNumArg = GA_CARD_1;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_t, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_t, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_t, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_r)
    {
        if (iNumGalilCards < 2)
            return FAILURE;
        if (*iCardNumArg == GA_CARD_0)
            return FAILURE;
        *iCardNumArg = GA_CARD_1;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_r, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_r, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_r, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_z)
    {
        if (iNumGalilCards < 2)
            return FAILURE;
        if (*iCardNumArg == GA_CARD_0)
            return FAILURE;
        *iCardNumArg = GA_CARD_1;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_z, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_z, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_z, MECH_TYPE));
    }
    if (ulEquipeAxisArg & RO_AXIS_w)
    {
        if (iNumGalilCards < 2)
            return FAILURE;
        if (*iCardNumArg == GA_CARD_0)
            return FAILURE;
        *iCardNumArg = GA_CARD_1;
        if ((uMappedAxis=ROGetMappedAxis(RO_AXIS_w, GALIL_AXIS)) == 0)
            return FAILURE;
        *uGalilAxesArg |= uMappedAxis;
        if ((*iFileTypeArg > -1) && (*iFileTypeArg != (int)(ROGetMappedAxis(RO_AXIS_w, MECH_TYPE))))
            return FAILURE;
        *iFileTypeArg = (int)(ROGetMappedAxis(RO_AXIS_w, MECH_TYPE));
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROGetMappedAxis
 *
 * Abstract:    Returns the mapped information for the appropriate
 *      Equipe axis.
 *
 * Parameters:
 *      uAxisArg        (in) The Equipe axis designation (R, w, etc.)
 *      uWhichMapArg    (in) The define for the mapped information
 *
 * Returns:     The mapped information
 *
 ***************************************************************/
unsigned ROGetMappedAxis(unsigned uAxisArg, unsigned uWhichMapArg)
{
    int iAxis;

    /* For all available system axes... */
    for (iAxis=0; iAxis<iNumAxes; iAxis++)
    {
        /* ...if the Equipe axis designation matches... */
        if (uaAxisMap[iAxis][EQUIPE_AXIS] == uAxisArg)
        {
            /* ...return the requested axis map of that Equipe axis. */
            return uaAxisMap[iAxis][uWhichMapArg];
        }
    }

    /* The Equipe axis designation doesn't exist in the system. */
    return 0;
}


/****************************************************************
 *
 * Function:    ROGetSpecialAxis
 *
 * Abstract:    Returns the axis map information for the appropriate
 *      special axis.
 *
 * Parameters:
 *      uSpecialAxisArg     (in) The special axis designation (RO_TRACK, etc.)
 *
 * Returns:     The mapped information
 *
 ***************************************************************/
unsigned long ROGetSpecialAxis(unsigned uSpecialAxisArg)
{
    int iAxis;

    /* For all available system axes... */
    for (iAxis=0; iAxis<iNumAxes; iAxis++)
    {
        /* ...if the special axis designation matches the request... */
        if (uaAxisMap[iAxis][SPECIAL_AXIS] == uSpecialAxisArg)
        {
            /* ...return the Equipe axis corresponding to the requested special component. */
            return ((unsigned long)(uaAxisMap[iAxis][EQUIPE_AXIS]));
        }
    }

    /* The special component doesn't exist in the system. */
    return 0;
}


/****************************************************************
 *
 * Function:    ROAxisReadyToMove
 *
 * Abstract:    Returns the combined state of the relevant flags
 *      which indicate if the axes are ready to move, i.e.
 *      servos on, homed, etc.
 *
 * Parameters:
 *      ulEquipeAxisArg (in) The axes to read
 *      iCardNumArg     (out) The Galil card number to operate on
 *      iFileTypeArg    (out) The relevant parameter file
 *      uGalilAxesArg   (out) The Galil axes corresponding to the Equipe axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROAxisReadyToMove(unsigned long ulEquipeAxisArg, int *iCardNumArg, int *iFileTypeArg, unsigned *uGalilAxesArg)
{
    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, iCardNumArg, iFileTypeArg, uGalilAxesArg) == FAILURE)
        return FAILURE;

    // On VAC514 R and T motion requires geared T and R axes, thus always check on both axes simultanouselly
    if(iDefineFlag & DFVAC514)
    {
        if((ulEquipeAxisArg & RO_AXIS_R) || (ulEquipeAxisArg & RO_AXIS_T))
            ulEquipeAxisArg |= (RO_AXIS_R | RO_AXIS_T);
    }

    /* Error if servo off, motion not complete, or not homed on the requested Equipe axes. */
    if (ulEquipeAxisArg & (ulServoFlag | ~ulAMFlag | ~ulHomed))
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROScaleSpd
 *
 * Abstract:    Scales speed information from encoder counts
 *      to real world units. It works on all axes together.
 *
 * Parameters:
 *      lSpeedArg       (in) The speed data for all axes
 *      lScaledSpeedArg (out) The scaled speed data for all axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: 1) Unneeded axes should be initialized to zero to
 *      ensure no floating point errors.
 *       2) The W axis is typically the first column of the
 *      parameter file. The w axis is typically the second
 *      column of the parameter file. The overriding exception
 *      is that the track data is always in the second column.
 *      So for a 4 axis system where the track is the W axis,
 *      it's information MUST be pulled from the second index
 *      of the arrays returned from the datafile.
 *
 ***************************************************************/
int ROScaleSpd(unsigned long ulEquipeAxisArg, long *lSpeedArg, long *lScaledSpeedArg)
{
    long lMechRatio[3], lArmLength[3];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // temporary double vars are moved from global to local to protect from interrupt.
    double dScaleTemp1, dScaleTemp2, dScaleTemp3, dScaleTemp4, dScaleTemp5;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the arm length of the relevant axes from the datafile.
     * This pertains to R axis only. */
    if (FIOGetParamVals(iFileType, ARM_LENGTH, lArmLength) == FAILURE)
        return FAILURE;
    /* Get the mechanical ratio of the relevant axes from the datafile. */
    if (FIOGetParamVals(iFileType, MECHANICAL_RATIO, lMechRatio) == FAILURE)
        return FAILURE;

<<<<<<< HEAD
    /* T axis. */
	if ((ulEquipeAxisArg & RO_AXIS_T) && (lSpeedArg[0] != 0))
=======
    /* T axis. */
	if ((ulEquipeAxisArg & RO_AXIS_T) && (lSpeedArg[0] != 0))
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    {
        if(iDefineFlag & DFVAC514)
            dScaleTemp1 = dMechRatioVAC514 / 1.0e5;
        else
            dScaleTemp1 = (double)(lMechRatio[0]) / 1.0e5;
        dScaleTemp2 = 0.1 * (double)(lSpeedArg[0]);
        dScaleTemp3 = dScaleTemp1 * dScaleTemp2;

        dScaleTemp1 = dScaleTemp3 + 0.5;
        lScaledSpeedArg[0] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[0] = 0;
    }

<<<<<<< HEAD
    /* t axis. */
=======
    /* t axis. */
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if ((ulEquipeAxisArg & RO_AXIS_t) && (lSpeedArg[4] != 0))
    {
        dScaleTemp1 = (double)(lMechRatio[0]) / 1.0e5;
        dScaleTemp2 = 0.1 * (double)(lSpeedArg[4]);
        dScaleTemp3 = dScaleTemp1 * dScaleTemp2;

        dScaleTemp1 = dScaleTemp3 + 0.5;
        lScaledSpeedArg[4] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[4] = 0;
    }
    /* R axis. */
    if ((ulEquipeAxisArg & RO_AXIS_R) && (lSpeedArg[1] != 0))
    {
        dScaleTemp1 = (double)(lMechRatio[1]) / 1.0e9;
        dScaleTemp2 = (double)(lArmLength[1]) / 1.570796326795;
        dScaleTemp3 = dScaleTemp1 * dScaleTemp2;
        dScaleTemp4 = 0.1 * (double)(lSpeedArg[1]);
        dScaleTemp5 = dScaleTemp3 * dScaleTemp4;

        dScaleTemp1 = dScaleTemp5 + 0.5;
        lScaledSpeedArg[1] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[1] = 0;
    }
    /* r axis. */
    if ((ulEquipeAxisArg & RO_AXIS_r) && (lSpeedArg[5] != 0))
    {
//        if ( iDefineFlag & DFAK )
//        {
            dScaleTemp3 = (double)(lMechRatio[1]) / 1.0e5;
//        }
//        else
//        {
//            dScaleTemp1 = (double)(lMechRatio[1]) / 1.0e9;
//            dScaleTemp2 = (double)(lArmLength[1]) / 1.570796326795;
//            dScaleTemp3 = dScaleTemp1 * dScaleTemp2;
//        }
        dScaleTemp4 = 0.1 * (double)(lSpeedArg[5]);
        dScaleTemp5 = dScaleTemp3 * dScaleTemp4;

        dScaleTemp1 = dScaleTemp5 + 0.5;
        lScaledSpeedArg[5] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[5] = 0;
    }
    /* Z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_Z) && (lSpeedArg[2] != 0))
    {
        dScaleTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dScaleTemp2 = 0.1 * (double)(lSpeedArg[2]);
        dScaleTemp3 = dScaleTemp1 * dScaleTemp2;

        dScaleTemp1 = dScaleTemp3 + 0.5;
        lScaledSpeedArg[2] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[2] = 0;
    }
    /* z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_z) && (lSpeedArg[6] != 0))
    {
        dScaleTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dScaleTemp2 = 0.1 * (double)(lSpeedArg[6]);
        dScaleTemp3 = dScaleTemp1 * dScaleTemp2;

        dScaleTemp1 = dScaleTemp3 + 0.5;
        lScaledSpeedArg[6] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[6] = 0;
    }
    /* W axis. */
    if ((ulEquipeAxisArg & RO_AXIS_W ) && (lSpeedArg[3] != 0))
    {
        if (ulEquipeAxisArg == ROGetSpecialAxis(RO_DUAL_ARM))
        {
            if (lArmLength[0] == 0) lArmLength[0] = 10500;
            dScaleTemp1 = (double)(lMechRatio[0]) / 1.0e9;
            dScaleTemp2 = (double)(lArmLength[0]) / 1.570796326795;
            dScaleTemp3 = dScaleTemp1 * dScaleTemp2;
            dScaleTemp4 = 0.1 * (double)(lSpeedArg[3]);
            dScaleTemp5 = dScaleTemp3 * dScaleTemp4;

            dScaleTemp1 = dScaleTemp5 + 0.5;
            lScaledSpeedArg[3] = (long)(dScaleTemp1);
        }
        else
        {
            dScaleTemp1 = (double)(lMechRatio[0]) / 1.0e5;
            dScaleTemp2 = 0.1 * (double)(lSpeedArg[3]);
            dScaleTemp3 = dScaleTemp1 * dScaleTemp2;

            dScaleTemp1 = dScaleTemp3 + 0.5;
            lScaledSpeedArg[3] = (long)(dScaleTemp1);
        }
    }
    else
    {
        lScaledSpeedArg[3] = 0;
    }
    /* w axis. */
    if ((ulEquipeAxisArg & RO_AXIS_w) && (lSpeedArg[7] != 0))
    {
        dScaleTemp1 = (double)(lMechRatio[1]) / 1.0e5;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        dScaleTemp2 = 0.1 * (double)(lSpeedArg[7]);
        dScaleTemp3 = dScaleTemp1 * dScaleTemp2;

        dScaleTemp1 = dScaleTemp3 + 0.5;
        lScaledSpeedArg[7] = (long)(dScaleTemp1);
    }
    else
    {
        lScaledSpeedArg[7] = 0;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROUnscaleSpd
 *
 * Abstract:    Unscales speed information from real world units
 *      to encoder counts. It works on all axes together.
 *
 * Parameters:
 *      lSpeedArg           (in) The speed data for all axes
 *      lUnscaledSpeedArg   (out) The unscaled speed data for all axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: 1) Unneeded axes should be initialized to zero to
 *      ensure no floating point errors.
 *       2) The W axis is typically the first column of the
 *      parameter file. The w axis is typically the second
 *      column of the parameter file. The overriding exception
 *      is that the track data is always in the second column.
 *      So for a 4 axis system where the track is the W axis,
 *      it's information MUST be pulled from the second index
 *      of the arrays returned from the datafile.
 *
 ***************************************************************/
int ROUnscaleSpd(unsigned long ulEquipeAxisArg, long *lSpeedArg, long *lUnscaledSpeedArg)
{
    long lMechRatio[3], lArmLength[3];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // temporary double vars are moved from global to local to protect from interrupt.
    double dUnscaleTemp1, dUnscaleTemp2, dUnscaleTemp3, dUnscaleTemp4, dUnscaleTemp5;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the arm length of the relevant axes from the datafile.
     * This pertains to R axis only. */
    if (FIOGetParamVals(iFileType, ARM_LENGTH, lArmLength) == FAILURE)
        return FAILURE;
    /* Get the mechanical ratio of the relevant axes from the datafile. */
    if (FIOGetParamVals(iFileType, MECHANICAL_RATIO, lMechRatio) == FAILURE)
        return FAILURE;

    /* T axis. */
    if ((ulEquipeAxisArg & RO_AXIS_T ) && (lSpeedArg[0] != 0))
    {
        if(iDefineFlag & DFVAC514)
            dUnscaleTemp1 = dMechRatioVAC514 / 1.0e5;
        else
            dUnscaleTemp1 = (double)(lMechRatio[0]) / 1.0e5;

        dUnscaleTemp2 = 0.1 * dUnscaleTemp1;
        dUnscaleTemp3 = 1.0 / dUnscaleTemp2;

        dUnscaleTemp1 = dUnscaleTemp3 * (double)(lSpeedArg[0]);
        lUnscaledSpeedArg[0] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[0] = 0;
    }
    /* t axis. */
    if ((ulEquipeAxisArg & RO_AXIS_t) && (lSpeedArg[4] != 0))
    {
        dUnscaleTemp1 = (double)(lMechRatio[0]) / 1.0e5;

        dUnscaleTemp2 = 0.1 * dUnscaleTemp1;
        dUnscaleTemp3 = 1.0 / dUnscaleTemp2;

        dUnscaleTemp1 = dUnscaleTemp3 * (double)(lSpeedArg[4]);
        lUnscaledSpeedArg[4] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[4] = 0;
    }
    /* R axis. */
    if ((ulEquipeAxisArg & RO_AXIS_R ) && (lSpeedArg[1] != 0))
    {
        if (iDefineFlag & DFAK)
        {
            dUnscaleTemp3 = (double)(lMechRatio[1]) / 1.0e5;
        }
        else
        {
            dUnscaleTemp1 = (double)(lMechRatio[1]) / 1.0e9;
            dUnscaleTemp2 = (double)(lArmLength[1]) / 1.570796326795;
            dUnscaleTemp3 = dUnscaleTemp1 * dUnscaleTemp2;
        }
        dUnscaleTemp4 = 0.1 * dUnscaleTemp3;
        dUnscaleTemp5 = 1.0 / dUnscaleTemp4;

        dUnscaleTemp1 = dUnscaleTemp5 * (double)(lSpeedArg[1]);
        lUnscaledSpeedArg[1] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[1] = 0;
    }
    /* r axis. */
    if ((ulEquipeAxisArg & RO_AXIS_r) && (lSpeedArg[5] != 0))
    {
        dUnscaleTemp3 = (double)(lMechRatio[1]) / 1.0e5;
        dUnscaleTemp4 = 0.1 * dUnscaleTemp3;
        dUnscaleTemp5 = 1.0 / dUnscaleTemp4;

        dUnscaleTemp1 = dUnscaleTemp5 * (double)(lSpeedArg[5]);
        lUnscaledSpeedArg[5] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[5] = 0;
    }
    /* Z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_Z ) && (lSpeedArg[2] != 0))
    {
        dUnscaleTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dUnscaleTemp2 = 0.1 * dUnscaleTemp1;
        dUnscaleTemp3 = 1.0 / dUnscaleTemp2;

        dUnscaleTemp1 = dUnscaleTemp3 * (double)(lSpeedArg[2]);
        lUnscaledSpeedArg[2] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[2] = 0;
    }
    /* z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_z) && (lSpeedArg[6] != 0))
    {
        dUnscaleTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dUnscaleTemp2 = 0.1 * dUnscaleTemp1;
        dUnscaleTemp3 = 1.0 / dUnscaleTemp2;

        dUnscaleTemp1 = dUnscaleTemp3 * (double)(lSpeedArg[6]);
        lUnscaledSpeedArg[6] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[6] = 0;
    }
    /* W axis. */
    if ((ulEquipeAxisArg & RO_AXIS_W ) && (lSpeedArg[3] != 0))
    {
        if (ulEquipeAxisArg == ROGetSpecialAxis(RO_DUAL_ARM))
        {
            if (lArmLength[0] == 0) lArmLength[0] = 10500;
            dUnscaleTemp1 = (double)(lMechRatio[0]) / 1.0e9;
            dUnscaleTemp2 = (double)(lArmLength[0]) / 1.570796326795;
            dUnscaleTemp3 = dUnscaleTemp1 * dUnscaleTemp2;
            dUnscaleTemp4 = 0.1 * dUnscaleTemp3;
            dUnscaleTemp5 = 1.0 / dUnscaleTemp4;

            dUnscaleTemp1 = dUnscaleTemp5 * (double)(lSpeedArg[3]);
            lUnscaledSpeedArg[3] = (long)(dUnscaleTemp1);
        }
        else
        {
            if ((ulEquipeAxisArg == RO_AXIS_W) && (~iDefineFlag & DFSS))
                dUnscaleTemp1 = (double)(lMechRatio[0]) / 1.0e5;
            else /* if (ulEquipeAxis == RO_AXIS_w) */
                dUnscaleTemp1 = (double)(lMechRatio[1]) / 1.0e5;

            dUnscaleTemp2 = 0.1 * dUnscaleTemp1;
            dUnscaleTemp3 = 1.0 / dUnscaleTemp2;

            dUnscaleTemp1 = dUnscaleTemp3 * (double)(lSpeedArg[3]);
            lUnscaledSpeedArg[3] = (long)(dUnscaleTemp1);
        }
    }
    else
    {
        lUnscaledSpeedArg[3] = 0;
    }
    /* w axis. */
    if ((ulEquipeAxisArg & RO_AXIS_w) && (lSpeedArg[7] != 0))
    {
        dUnscaleTemp1 = (double)(lMechRatio[1]) / 1.0e5;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        dUnscaleTemp2 = 0.1 * dUnscaleTemp1;
        dUnscaleTemp3 = 1.0 / dUnscaleTemp2;

        dUnscaleTemp1 = dUnscaleTemp3 * (double)(lSpeedArg[7]);
        lUnscaledSpeedArg[7] = (long)(dUnscaleTemp1);
    }
    else
    {
        lUnscaledSpeedArg[7] = 0;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROScalePos
 *
 * Abstract:    Scales position information from encoder counts
 *      to real world units. It works on all axes together.
 *
 * Parameters:
 *      lPositionArg        (in) The position data for all axes
 *      lScaledPositionArg  (out) The scaled position data for all axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: 1) Unneeded axes should be initialized to zero to
 *      ensure no floating point errors.
 *       2) The W axis is typically the first column of the
 *      parameter file. The w axis is typically the second
 *      column of the parameter file. The overriding exception
 *      is that the track data is always in the second column.
 *      So for a 4 axis system where the track is the W axis,
 *      it's information MUST be pulled from the second index
 *      of the arrays returned from the datafile.
 *
 ***************************************************************/
int ROScalePos(unsigned long ulEquipeAxisArg, long *lPositionArg, long *lScaledPositionArg)
{
    long lMechRatio[3], lArmLength[3];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // temporary double vars are moved from global to local to protect from interrupt.
    double dTemp1, dTemp2, dTemp3, dTemp4;  //, dTemp5;
    double dPosOffset;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the arm length of the relevant axes from the datafile.
     * This pertains to R axis only. */
    if (FIOGetParamVals(iFileType, ARM_LENGTH, lArmLength) == FAILURE)
        return FAILURE;
    /* Get the mechanical ratio of the relevant axes from the datafile. */
    if (FIOGetParamVals(iFileType, MECHANICAL_RATIO, lMechRatio) == FAILURE)
        return FAILURE;

    /* T axis. */
    if ((ulEquipeAxisArg & RO_AXIS_T ) && (lPositionArg[0] != 0))
    {
        if(iDefineFlag & DFVAC514)
            dTemp1 = dMechRatioVAC514 / 1.0e5;
        else
            dTemp1 = (double)(lMechRatio[0]) / 1.0e5;

        dTemp2 = dTemp1 * (double)(lPositionArg[0]);
        if (lPositionArg[0] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lScaledPositionArg[0] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[0] = 0;
    }
    /* t axis. */
    if ((ulEquipeAxisArg & RO_AXIS_t) && (lPositionArg[4] != 0))
    {
        dTemp1 = (double)(lMechRatio[0]) / 1.0e5;

        dTemp2 = dTemp1 * (double)(lPositionArg[4]);
        if (lPositionArg[4] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lScaledPositionArg[4] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[4] = 0;
    }
    /* R axis. */
    if ((ulEquipeAxisArg & RO_AXIS_R ) && (lPositionArg[1] != 0))
    {
        dTemp1 = (double)(lMechRatio[1]) * (double)(lPositionArg[1]);
        dTemp2 = dTemp1 / 1.0e9;
        dTemp3 = sin(dTemp2);
        dTemp4 = (double)(lArmLength[1]) * dTemp3;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (lPositionArg[1] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp4 + dPosOffset;
        lScaledPositionArg[1] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[1] = 0;
    }
    /* r axis. */
    if ((ulEquipeAxisArg & RO_AXIS_r) && (lPositionArg[5] != 0))
    {
        dTemp1 = (double)(lMechRatio[1]) * (double)(lPositionArg[5]);
        dTemp4 = dTemp1 / 1.0e5;
        if (lPositionArg[5] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp4 + dPosOffset;
        lScaledPositionArg[5] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[5] = 0;
    }
    /* Z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_Z ) && (lPositionArg[2] != 0))
    {
        dTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dTemp2 = dTemp1 * (double)(lPositionArg[2]);
        if (lPositionArg[2] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lScaledPositionArg[2] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[2] = 0;
    }
    /* z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_z) && (lPositionArg[6] != 0))
    {
        dTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dTemp2 = dTemp1 * (double)(lPositionArg[6]);
        if (lPositionArg[6] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lScaledPositionArg[6] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[6] = 0;
    }
    /* W axis. */
    if ((ulEquipeAxisArg & RO_AXIS_W ) && (lPositionArg[3] != 0))
    {
        if (ulEquipeAxisArg == ROGetSpecialAxis(RO_DUAL_ARM))
        {
            if (lArmLength[0] == 0) lArmLength[0] = 10500;
            dTemp1 = (double)(lMechRatio[0]) * (double)(lPositionArg[3]);
            dTemp2 = dTemp1 / 1.0e9;
            dTemp3 = sin(dTemp2);
            dTemp4 = (double)(lArmLength[0]) * dTemp3;
            if (lPositionArg[3] < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp4 + dPosOffset;
            lScaledPositionArg[3] = (long)(dTemp1);
        }
        else
        {
            if ((ulEquipeAxisArg == RO_AXIS_W) && (~iDefineFlag & DFSS))
                dTemp1 = (double)(lMechRatio[0]) / 1.0e5;
            else /* if (ulEquipeAxis == RO_AXIS_w) */
                dTemp1 = (double)(lMechRatio[1]) / 1.0e5;

            dTemp2 = dTemp1 * (double)(lPositionArg[3]);
            if (lPositionArg[3] < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp2 + dPosOffset;
            lScaledPositionArg[3] = (long)(dTemp1);
        }
    }
    else
    {
        lScaledPositionArg[3] = 0;
    }
    /* w axis. */
    if ((ulEquipeAxisArg & RO_AXIS_w) && (lPositionArg[7] != 0))
    {
        dTemp1 = (double)(lMechRatio[1]) / 1.0e5;

        dTemp2 = dTemp1 * (double)(lPositionArg[7]);
        if (lPositionArg[7] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lScaledPositionArg[7] = (long)(dTemp1);
    }
    else
    {
        lScaledPositionArg[7] = 0;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROUnscalePos
 *
 * Abstract:    Unscales position information from real world
 *      units to encoder counts. It works on all axes together.
 *
 * Parameters:
 *      lPositionArg            (in) The position data for all axes
 *      lUnscaledPositionArg    (out) The unscaled position data for all axes
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: 1) Unneeded axes should be initialized to zero to
 *      ensure no floating point errors.
 *       2) The W axis is typically the first column of the
 *      parameter file. The w axis is typically the second
 *      column of the parameter file. The overriding exception
 *      is that the track data is always in the second column.
 *      So for a 4 axis system where the track is the W axis,
 *      it's information MUST be pulled from the second index
 *      of the arrays returned from the datafile.
 *
 ***************************************************************/
int ROUnscalePos(unsigned long ulEquipeAxisArg, long *lPositionArg, long *lUnscaledPositionArg)
{
    long lMechRatio[3], lArmLength[3];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // temporary double vars are moved from global to local to protect from interrupt.
    double dTemp1, dTemp2, dTemp3;  //, dTemp4; //, dTemp5;
    double dPosOffset, dInvAngle, dInvAngleTemp;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the arm length of the relevant axes from the datafile.
     * This pertains to R axis only. */
    if (FIOGetParamVals(iFileType, ARM_LENGTH, lArmLength) == FAILURE)
        return FAILURE;
    /* Get the mechanical ratio of the relevant axes from the datafile. */
    if (FIOGetParamVals(iFileType, MECHANICAL_RATIO, lMechRatio) == FAILURE)
        return FAILURE;

    /* T axis. */
    if ((ulEquipeAxisArg & RO_AXIS_T ) && (lPositionArg[0] != 0))
    {
        if(iDefineFlag & DFVAC514)
            dTemp1 = 1.0e5 / dMechRatioVAC514;
        else
            dTemp1 = 1.0e5 / (double)(lMechRatio[0]);

        dTemp2 = dTemp1 * (double)(lPositionArg[0]);
        if (lPositionArg[0] < 0)
        {
            dPosOffset = -0.5;
        } 
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lUnscaledPositionArg[0] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[0] = 0;
    }
    /* t axis. */
    if ((ulEquipeAxisArg & RO_AXIS_t) && (lPositionArg[4] != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[0]);

        dTemp2 = dTemp1 * (double)(lPositionArg[4]);
        if (lPositionArg[4] < 0)
        {
            dPosOffset = -0.5;
        } 
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lUnscaledPositionArg[4] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[4] = 0;
    }
    /* R axis. */
    if ((ulEquipeAxisArg & RO_AXIS_R ) && (lPositionArg[1] != 0))
    {
        dInvAngleTemp = 1.0 / (double)(lArmLength[1]);
        dInvAngle = dInvAngleTemp * (double)(lPositionArg[1]);
        if (dInvAngle > 1.0)
        {
            if (dInvAngle > 2.0)
                dInvAngle = 1.0;
            else
               dInvAngle = 2.0 - dInvAngle;
        }
        else if (dInvAngle < -1.0)
        {
            if (dInvAngle < -2.0)
               dInvAngle = -1.0;
            else
                dInvAngle = (-2.0) - dInvAngle;
        }

        dTemp1 = 1.0e9 / (double)(lMechRatio[1]);
        dTemp2 = asin(dInvAngle);
        dTemp3 = dTemp1 * dTemp2;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (lPositionArg[1] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp3 + dPosOffset;
        lUnscaledPositionArg[1] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[1] = 0;
    }
    /* r axis. */
    if ((ulEquipeAxisArg & RO_AXIS_r) && (lPositionArg[5] != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[1]);
        dTemp3 = dTemp1 * (double)(lPositionArg[5]);
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (lPositionArg[5] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp3 + dPosOffset;
        lUnscaledPositionArg[5] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[5] = 0;
    }
    /* Z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_Z ) && (lPositionArg[2] != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[2]);
        dTemp2 = dTemp1 * (double)(lPositionArg[2]);
        if (lPositionArg[2] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lUnscaledPositionArg[2] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[2] = 0;
    }
    /* z axis. */
    if ((ulEquipeAxisArg & RO_AXIS_z) && (lPositionArg[6] != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[2]);
        dTemp2 = dTemp1 * (double)(lPositionArg[6]);
        if (lPositionArg[6] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lUnscaledPositionArg[6] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[6] = 0;
    }
    /* W axis. */
    if ((ulEquipeAxisArg & RO_AXIS_W ) && (lPositionArg[3] != 0))
    {
        if (ulEquipeAxisArg == ROGetSpecialAxis(RO_DUAL_ARM))
        {
            /* ??? */
            if (lArmLength[0] == 0) lArmLength[0] = 10500;
            dInvAngleTemp = 1.0 / (double)(lArmLength[0]);
            dInvAngle = dInvAngleTemp * (double)(lPositionArg[3]);
            if (dInvAngle > 1.0)
            {
                if (dInvAngle > 2.0)
                    dInvAngle = 1.0;
                else
                    dInvAngle = 2.0 - dInvAngle;
            }
            else if (dInvAngle < -1.0)
            {
                if (dInvAngle < -2.0)
                    dInvAngle = -1.0;
                else
                    dInvAngle = (-2.0) - dInvAngle;
            }

            dTemp1 = 1.0e9 / (double)(lMechRatio[0]);
            dTemp2 = asin(dInvAngle);
            dTemp3 = dTemp1 * dTemp2;
            if (lPositionArg[3] < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp3 + dPosOffset;
            lUnscaledPositionArg[3] = (long)(dTemp1);
        }
        else
        {
            if ((ulEquipeAxisArg == RO_AXIS_W) && (~iDefineFlag & DFSS))
                dTemp1 = 1.0e5 / (double)(lMechRatio[0]);
            else /* if (ulEquipeAxis == RO_AXIS_w) */
                dTemp1 = 1.0e5 / (double)(lMechRatio[1]);

            dTemp2 = dTemp1 * (double)(lPositionArg[3]);
            if (lPositionArg[3] < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp2 + dPosOffset;
            lUnscaledPositionArg[3] = (long)(dTemp1);
        }
    }
    else
    {
        lUnscaledPositionArg[3] = 0;
    }
    /* w axis. */
    if ((ulEquipeAxisArg & RO_AXIS_w) && (lPositionArg[7] != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[1]);

        dTemp2 = dTemp1 * (double)(lPositionArg[7]);
        if (lPositionArg[7] < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        lUnscaledPositionArg[7] = (long)(dTemp1);
    }
    else
    {
        lUnscaledPositionArg[7] = 0;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROGetCardIndex
 *
 * Abstract:    Returns the MC card index appropriate for
 *      the current system.
 *
 * Parameters:  None
 *
 * Returns:     MC card index
 *
 ***************************************************************/
int ROGetCardIndex()
{
    if (iPreOnly)
        return GA_CARD_1;
    if (iNumGalilCards == 1)
        return GA_CARD_0;

    return GA_BOTH_CARDS;
}


/****************************************************************
 *
 * Function:    ROInitGalilLocal
 *
 * Abstract:    Initializes the Galil card with the appropriate
 *      datafile parameters. It gets the parameter values from the
 *      datafile and then sets the value to the Galil card.
 *      All interaction is taken care of through the mechanism
 *      Get/Set functions.
 *
 * Parameters:
 *      ulEquipeAxesArg     (in) Equipe axes designation for axes
 *                              from a particular parameter file
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: Because the parameter files don't match the axis definitions
 *      explicitly or necessarily, we must create the axis bit pattern
 *      by parameter file. However, if during initialization, the parameter
 *      file to Equipe axis pairing doesn't match the capabilities of
 *      the system, we must exit with an error. This check is performed
 *      inherently by the ROValidAxis function and explicitly in
 *      ROGetCardAxes.
 *
 ***************************************************************/
int ROInitGalilLocal(unsigned long ulEquipeAxesArg)
{
    long lValues[8];                    /* Values returned from the datafile. */
    double dGainOffset[8]={0,0,0,0,0,0,0,0};  /* Z gain offset values. */
    int iCardNum, iFileType, i;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system and can be used as configured. */
    if (ROValidAxis(ulEquipeAxesArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

<<<<<<< HEAD
    /* Galil "OE" command. Axes de-servo if the error limit is surpassed. */
=======
    /* Galil "OE" command. Axes de-servo if the error limit is surpassed. */
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	for (i=0; i<8; ++i) lValues[i] = 1;
//    lValues[0] = lValues[1] = lValues[2] = lValues[3] = lValues[4] = 1;
    if (GASetValsLong(iCardNum, OFF_ON_ERROR_COMMAND, uGalilAxes, lValues) == FAILURE)
        return FAILURE;

    /* Galil "ER" command. Limit that servo error can reach before a fault is registered.
     * This is primarily for safety. If the robot hits someone, it won't drive through them. */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, ERROR_LIMIT) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, ERROR_LIMIT) == FAILURE)
        return FAILURE;

    /* Galil "GN" command. The PID proportional gain value. */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, PROPORTIONAL_GAIN) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, PROPORTIONAL_GAIN) == FAILURE)
        return FAILURE;

    /* Galil "FA" command. The feed-forward acceleration value. */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, FEED_FORWARD_ACCEL) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, FEED_FORWARD_ACCEL) == FAILURE)
        return FAILURE;

    /* Galil "KI" command. The PID integral gain value. */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, INTEGRAL_GAIN) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, INTEGRAL_GAIN) == FAILURE)
        return FAILURE;

    /* Galil "TL" command. Torque limit to restrict over-driving the motors. */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, TORQUE_LIMIT) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, TORQUE_LIMIT) == FAILURE)
        return FAILURE;

    /* Galil "ZR" command. The PID derivative gain value. */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, DERIVATIVE_GAIN) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, DERIVATIVE_GAIN) == FAILURE)
        return FAILURE;

    /* Galil "RM" command. The acceleration ramp or rate of change of
     * acceleration, a.k.a. "jerk". */
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, JERK) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, JERK) == FAILURE)
        return FAILURE;

    /* Galil "SP" command. Sets the motor maximum turning speed. */
	for (i=0; i<8; ++i) lValues[i] = 0;
//    lValues[0] = lValues[1] = lValues[2] = lValues[3] = 0;
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, OPERATIONAL_SPEED) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, OPERATIONAL_SPEED) == FAILURE)
        return FAILURE;

    /* Galil "AC" command. Sets the motor maximum acceleration. */
	for (i=0; i<8; ++i) lValues[i] = 0;
//    lValues[0] = lValues[1] = lValues[2] = lValues[3] = 0;
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;

    /* Galil "DC" command. Sets the motor maximum deceleration. */
	for (i=0; i<8; ++i) lValues[i] = 0;
//    lValues[0] = lValues[1] = lValues[2] = lValues[3] = 0;
    if (ROGetParameter(TRUE, ulEquipeAxesArg, lValues, OPERATIONAL_DECEL) == FAILURE)
        return FAILURE;
    if (ROSetParameter(FALSE, ulEquipeAxesArg, lValues, OPERATIONAL_DECEL) == FAILURE)
        return FAILURE;

    /* Galil "IT" command. Sets the system sampling time. */
    if (GASetTimeCommand(iCardNum, 1000) == FAILURE)
        return FAILURE;

    /* Galil "MT" command. Set the motor type (whether positive amp reference
     * voltage turns the motor clockwise or counter-clockwise) and then
     * turn on the PID servo mechanism. The only other place this command is used
     * is in the ROServoMCOnLocal function. */
    /* WARNING: W axis may have reversed polarity for some systems. */
//	for (i=0; i<8; ++i) lValues[i] = -1;
	for (i=0; i<8; ++i) lValues[i] = 1;	// dmc2186 reversed polarity
//    lValues[0] = lValues[1] = lValues[2] = -1;
//    if (ulEquipeAxesArg & (ROGetSpecialAxis(RO_FLIPPER)))
//        lValues[3] = 2;
<<<<<<< HEAD
//    else
//	{
        	lValues[3] = -1;
        	lValues[7] = -1;
//	}
=======
//    else
//	{
        	lValues[3] = -1;
        	lValues[7] = -1;
//	}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    
    /* Delay is needed for 2 Galil card systems when this command doesn't go through. */
    if (TIDelay(50) == FAILURE)
        return FAILURE;
    if (GASetValsLong(iCardNum, MOTOR_TYPE_COMMAND, uGalilAxes, lValues) == FAILURE)
        return FAILURE;

    /* Galil "CN" command. Configure limit switch polarity. */
    if (ulEquipeAxesArg & (ROGetSpecialAxis(RO_TRACK) | ROGetSpecialAxis(RO_FLIPPER)))
    {
        // different Home switches polarity for Stage controllers
        if(iDefineFlag & DFAK)
        {
            if (GASendConfigureSystemCommand(iCardNum, 1, -1, 1, 3) == FAILURE)
                return FAILURE;
        }
        else
        {
//            if (GASendConfigureSystemCommand(iCardNum, 0, 0, 0, 3) == FAILURE)
            if (GASendConfigureSystemCommand(iCardNum, 0, 0, 1, 0) == FAILURE)
                return FAILURE;
        }
    }
    else
    {
        // different Home switches polarity for Stage controllers
        if(iDefineFlag & DFAK)
        {
            if (GASendConfigureSystemCommand(iCardNum, 1, -1, 1, 0) == FAILURE)
                return FAILURE;
        }
        else
        {
            if (GASendConfigureSystemCommand(iCardNum, 0, 0, 1, 0) == FAILURE)
                return FAILURE;
        }
    }

<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if (iFileType == ROBOTFILE)
    {
        /* Gets Z brake and gain offset values from the datafile. */
        if (FIOGetParamVals(iFileType, H_VALUE, lValues) == FAILURE)
            return FAILURE;
        /* Don't forget, the normal units must be divided by 1000. */
        dGainOffset[2] = (double)(lValues[1])/1000.0;
        /* Set the gain offset in the Galil card. */
        if (GASetValsDouble(iCardNum, GAIN_OFFSET_COMMAND, GAZAXIS0, dGainOffset) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROCheckSwitches
 *
 * Abstract:    Reads limit switches from galil and sets status
 *      bits appropriately. It does this by sending a "TS"
 *      Tell Switches command and waiting for the reply. The reply
 *      is always of the form "nnn,nnn,nnn(,nnn)\r\n:". It is worth
 *      noting that if the limit switch values changed, the new values
 *      are not registered. Only the fact that they changed
 *      or were momentarily triggered is recorded. The new values
 *      themselves must be updated through this function.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROCheckSwitches(int iCardArg)
{
    int iNoAxes, iShifter, iAxis;  /* Temporary variables. */
    long lCurrSwitches[8];          /* The limit switch values returned from the Galil card. */
    unsigned uIMFlag;               /* 1 = in motion according to galil; bitwise by axis */
    unsigned uAxisT = RO_AXIS_T;
    unsigned uAxisR = RO_AXIS_R;
    unsigned uAxisZ = RO_AXIS_Z;
    unsigned uAxisW = RO_AXIS_W;

    /* Initialize locally used values to first Galil card and no limit switches trigered. */
    iShifter = 0;
    iNoAxes = 0;
    lCurrSwitches[0] = lCurrSwitches[1] = lCurrSwitches[2] = lCurrSwitches[3] = 0;
    lCurrSwitches[4] = lCurrSwitches[5] = lCurrSwitches[6] = lCurrSwitches[7] = 0;

    if (iCardArg == GA_CARD_1)
    {
        if (iNumGalilCards < 2)
            return SUCCESS;
        uAxisT = RO_AXIS_t;
        uAxisR = RO_AXIS_r;
        uAxisZ = RO_AXIS_z;
        uAxisW = RO_AXIS_w;
        iShifter = 4;
        /* Initialize the default switch values to NOT triggered. */
        ulLMPosFlag &= 0x00FF;
        ulLMNegFlag &= 0x00FF;
    }
    else
    {
        /* Initialize the default switch values to NOT triggered. */
        ulLMPosFlag &= 0xFF00;
        ulLMNegFlag &= 0xFF00;
    }
    iNoAxes = GAGetGLNumAxis(iCardArg);
    if (GAGetValsLong(iCardArg, TELL_SWITCHES_COMMAND, (unsigned)(iNoAxes), lCurrSwitches) == FAILURE)
        return FAILURE;
    if (GAGetGalilErrorFlag())
        return FAILURE;

    /* This In Motion flag is NEVER USED. */
    uIMFlag &= 0x00;

    /* Galil gives limit switches negative true. */
    for (iAxis=0; iAxis<8; iAxis++) lCurrSwitches[iAxis] = ~lCurrSwitches[iAxis];

    /* For each axis, register the in motion status. Then if the limit switch
     * is triggered, register the triggered switch in the correct bit patterened word. */
    /* T axis. */
    if (ROGetMappedAxis(uAxisT, GALIL_AXIS))
    {
        /* Register the value in the in motion flag. */
        uIMFlag |= (unsigned)((~lCurrSwitches[0] & INMOTION) >> (7-iShifter));
        /* Check if either the negative or positive limit switch is triggered. */
        if (lCurrSwitches[0] & (POSLIM | NEGLIM))
        {
            /* If the negative limit switch is triggered, register it. */
            ulLMNegFlag |= (lCurrSwitches[0] & NEGLIM) ? 1 << iShifter : 0;
            /* If the positive limit switch is triggered, register it. */
            ulLMPosFlag |= (lCurrSwitches[0] & POSLIM) ? 1 << iShifter : 0;
        }
    }

    /* R axis. */
    if (ROGetMappedAxis(uAxisR, GALIL_AXIS))
    {
        /* Register the value in the in motion flag. */
        uIMFlag |= (unsigned)((~lCurrSwitches[1] & INMOTION) >> (6-iShifter));
        if (lCurrSwitches[1] & (POSLIM | NEGLIM))
        {
            /* If the negative limit switch is triggered, register it. */
            ulLMNegFlag |= (lCurrSwitches[1] & NEGLIM) ? 2 << iShifter : 0;
            /* If the positive limit switch is triggered, register it. */
            ulLMPosFlag |= (lCurrSwitches[1] & POSLIM) ? 2 << iShifter : 0;
        }
    }

    /* Z axis. */
    if (ROGetMappedAxis(uAxisZ, GALIL_AXIS))
    {
        /* Register the value in the in motion flag. */
        uIMFlag |= (unsigned)((~lCurrSwitches[2] & INMOTION) >> (5-iShifter));
        if (lCurrSwitches[2] & (POSLIM | NEGLIM))
        {
            /* If the negative limit switch is triggered, register it. */
            ulLMNegFlag |= (lCurrSwitches[2] & NEGLIM) ? 4 << iShifter : 0;
            /* If the positive limit switch is triggered, register it. */
            ulLMPosFlag |= (lCurrSwitches[2] & POSLIM) ? 4 << iShifter : 0;
        }
    }

    /* W axis. */
    if (ROGetMappedAxis(uAxisW, GALIL_AXIS))
    {
        /* Register the value in the in motion flag. */
        uIMFlag |= (unsigned)((~lCurrSwitches[3] & INMOTION) >> (4-iShifter));
        if (lCurrSwitches[3] & (POSLIM | NEGLIM))
        {
            /* If the negative limit switch is triggered, register it. */
            ulLMNegFlag |= (lCurrSwitches[3] & NEGLIM) ? 8 << iShifter : 0;
            /* If the positive limit switch is triggered, register it. */
            ulLMPosFlag |= (lCurrSwitches[3] & POSLIM) ? 8 << iShifter : 0;
        }
    }

    /* If the abort axis flag is triggered but no longer needed, reset it. */
    if (uAbortAxis)
    {
        ulAMFlag |= (unsigned long)(uAbortAxis);
        uAbortAxis = 0;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROCheckLimitSwitches
 *
 * Abstract:    Tallies the conditions of all the limit switches.
 *      The states are tracked in the ulLMNegChanged and ulLMPosChanged
 *      variables. It is worth noting that if the limit switch
 *      values changed, the new values are not registered.
 *      Only the fact that they changed or were momentarily triggered
 *      is recorded. The new values themselves must be updated
 *      through this function.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROCheckLimitSwitches()
{
    unsigned uAllLM;
<<<<<<< HEAD

    //ROUpdateTS(FALSE);

    ROCheckSwitches(GA_CARD_0);
=======

    //ROUpdateTS(FALSE);

    ROCheckSwitches(GA_CARD_0);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* Find out if  recorded a change in the limit switch values. */
    uAllLM = (unsigned)(ulLMNegChanged | ulLMPosChanged) & 0xFF;

    /* If they did change, record the change internally, IF the axis exists in the system. */
    if (uAllLM)
    {

        /* If the T axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_T) && (ulAMFlag & RO_AXIS_T) && ROGetMappedAxis(RO_AXIS_T, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_T;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_T;
        }

        /* If the R axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_R) && (ulAMFlag & RO_AXIS_R) && ROGetMappedAxis(RO_AXIS_R, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_R;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_R;
        }

        /* If the Z axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_Z) && (ulAMFlag & RO_AXIS_Z) && ROGetMappedAxis(RO_AXIS_Z, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_Z;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_Z;
        }

        /* If the W axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_W) && (ulAMFlag & RO_AXIS_W) && ROGetMappedAxis(RO_AXIS_W, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_W;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_W;
        }


        /* If the t axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_t) && (ulAMFlag & RO_AXIS_t) && ROGetMappedAxis(RO_AXIS_t, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_t;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_t;
        }

        /* If the r axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_r) && (ulAMFlag & RO_AXIS_r) && ROGetMappedAxis(RO_AXIS_r, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_r;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_r;
        }

        /* If the z axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_z) && (ulAMFlag & RO_AXIS_z) && ROGetMappedAxis(RO_AXIS_z, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_z;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_z;
        }

        /* If the w axis limit switch was triggered, the axis is not in motion,
         * and the axis exists in the system (not line noise), record the change. */
        if ((uAllLM & RO_AXIS_w) && (ulAMFlag & RO_AXIS_w) && ROGetMappedAxis(RO_AXIS_w, GALIL_AXIS))
        {
            /* The negative limit switch value changed. */
            ulLMNegChanged &= ~RO_AXIS_w;
            /* The positive limit switch value changed. */
            ulLMPosChanged &= ~RO_AXIS_w;
        }

    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROReadLimitSwitches
 *
 * Abstract:    Returns the limit switch flag values.
 *
 * Parameters:
 *      lSwitchesArg    (out) The limit switch flag values bit
 *                          shifted so they all fit in a long variable
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROReadLimitSwitches(unsigned long *ulSwitchesArg)
{
    /* Update the limit switch values. */
//    if (!iPreOnly)
//    {
//        if(ROCheckSwitches(GA_CARD_0) == FAILURE)
//            return FAILURE;
//    }
//    if (iNumGalilCards >= 2)
//    {
//        if(ROCheckSwitches(GA_CARD_1) == FAILURE)
//            return FAILURE;
//    }
<<<<<<< HEAD

	ROUpdateTS(FALSE);
//printf("TS: %d,%d,%d,%d,%d,%d,%d,%d\n",glTSArray[0],glTSArray[1],glTSArray[2],glTSArray[3],glTSArray[4],glTSArray[5],glTSArray[6],glTSArray[7]);
//printf("TSbuf: %s\n",glTSReturnBuffer);
=======

	ROUpdateTS(FALSE);
//printf("TS: %d,%d,%d,%d,%d,%d,%d,%d\n",glTSArray[0],glTSArray[1],glTSArray[2],glTSArray[3],glTSArray[4],glTSArray[5],glTSArray[6],glTSArray[7]);
//printf("TSbuf: %s\n",glTSReturnBuffer);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* Add in the positive limit switch values as the lowest 8 bits. */
    *ulSwitchesArg = ulLMPosFlag&0x00FF;
    /* Add in the negative limit switch values as the next 8 bits. */
    *ulSwitchesArg |= (ulLMNegFlag&0x00FF) << 8L;
    /* Add in the positive limit switch momentary triggered values as the next 8 bits. */
    *ulSwitchesArg |= (ulLMPosMom&0x00FF) << 16L;
    /* Add in the negative limit switch momentary triggered values as the highest 8 bits. */
    *ulSwitchesArg |= (ulLMNegMom&0x00FF) << 24L;

    /* Clear the momentary limit switch values, i.e. read once and erase. */
    ulLMPosMom = ulLMNegMom = 0;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROStopAllFromIntr
 *
 * Abstract:    Stop all from interrupts. This stop motion,
 *      clears the action buffer, and aborts macro execution.
 *      It is called during an SSTOP macro.
 *
 * Parameters:  None
 *
 * Returns:     None. Safety function. FAILURE returns would be ignored anyway.
 *
 ***************************************************************/
void ROStopAllFromIntr( void )
{
    /* Stop motion, kill homing, clear the action buffer, and
     * set all appropriate flags for all axes. */
    ROStopMotion(0);

    /* Abort any macro in progress. */
    if (!MRGetMacroIntegerVars(MR_RUNNING_CMER) && !MRGetMacroIntegerVars(MR_RUNNING_INTLK) && !MRGetMacroIntegerVars(MR_RUNNING_SSTOP))
        MRAbortMacro();
}


/****************************************************************
 *
 * Function:    ROEmergencyOff
 *
 * Abstract:    Set the emergency off condition. This aborts
 *      motion, clears the action buffer, and possibly aborts
 *      macro execution. It is called during an INTLK macro.
 *
 * Parameters:
 *      iSw             (in) TRUE means stop all motion and pending motion
 *                          on all axes. FALSE means disable the amplifiers only.
 *      iCardNumArg     (in) The Galil card number to act on.
 *
 * Returns:     None. Safety function. FAILURE returns would be ignored anyway.
 *
 ***************************************************************/
void ROEmergencyOff( int iSw )
{
    int iPortn;

    if( !iSw )
    {
<<<<<<< HEAD
    	ROStopMotion(0);
	TIDelay(500);
=======
    	ROStopMotion(0);
	TIDelay(500);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

        /* Send abort motion which stops motion, kills homing, clears
         * the action buffer, and set appropriate flags for all axes.*/
        ROAbortMotion();

        /* Delay for a while so that the Galil command "AB" can complete. */
        TIDelay(10);

        /* Servo off on all axes. */
        ROServoMCOff(0);

        /* Abort any macro in progress. */
    	if (!MRGetMacroIntegerVars(MR_RUNNING_CMER) && !MRGetMacroIntegerVars(MR_RUNNING_INTLK))
            MRAbortMacro();
    }
    else
    {
        /* Disable amplifiers and amplifier power. */
//        iPortn = (iCardNumArg == GA_CARD_0) ? IO_ROBOT_OUTPUT_B:IO_PRE_OUTPUT_B;
        IOWriteIO( -1, inb(IO_ROBOT_OUTPUT_B) | 0x01, IO_ROBOT_OUTPUT_B );
    }
}


/****************************************************************
 *
 * Function:    RODisconnectPower
 *
 * Abstract:    Turn off alls amps and clear action buffer. This
 *      function is called from the IO_PROC when the signal cable
 *      has problems. It recognizes that the signal loop is now
 *      open and clears the action buffer for the IO card that
 *      the faliure was on and stops motion an all available axes
 *      (on both cards if there are 2 in the system).
 *
 * Returns:     None. Safety function. FAILURE returns would be ignored anyway.
 *
 ***************************************************************/
void RODisconnectPower()
<<<<<<< HEAD
{
    char caSTCommand[10] = "ST";
    char caABCommand[10] = "AB";
    char caMOCommand[10] = "MO";
    char caResp[MAXGASTR];
    long rc;
=======
{
    char caSTCommand[10] = "ST";
    char caABCommand[10] = "AB";
    char caMOCommand[10] = "MO";
    char caResp[MAXGASTR];
    long rc;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* Sets flags to indicate re-homing is required. */
    if( !iEMOWasSet || (inb( IO_ROBOT_INPUT_G) & (POWER_CHECK | LOOP_CHECK) ))
    {
//        ulHomed &= 0x0F << (4*(iCardNumArg - 1));
<<<<<<< HEAD
        ulHomed = 0;
=======
        ulHomed = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        SSSetStatusWord(HOME_NOT_EXED, TRUE);
    }

    /* Stop motion, kill homing, clear the action buffer, and
     * set all appropriate flags for all axes. */
<<<<<<< HEAD
    rc = GASendDMCCommand(ghDMC, caSTCommand, caResp, MAXGASTR);

    giTimerGA = TIGetCounter();
//printf("timer start:%d\n", giTimerGA);
    TISetCounter(giTimerGA, 1000);
    while (!TICountExpired(giTimerGA));
//printf("timer end:\n");
    TIReturnCounter(giTimerGA);

    rc = GASendDMCCommand(ghDMC, caABCommand, caResp, MAXGASTR);
=======
    rc = GASendDMCCommand(ghDMC, caSTCommand, caResp, MAXGASTR);

    giTimerGA = TIGetCounter();
//printf("timer start:%d\n", giTimerGA);
    TISetCounter(giTimerGA, 1000);
    while (!TICountExpired(giTimerGA));
//printf("timer end:\n");
    TIReturnCounter(giTimerGA);

    rc = GASendDMCCommand(ghDMC, caABCommand, caResp, MAXGASTR);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//    ROStopMotion(0);

    /* For the IO card the cable was disconnected on. */
    /* Servo off on all axes on the Galil card. */
//    ROServoMCOff(RO_AXIS_T|RO_AXIS_R|RO_AXIS_Z);
//    ROServoMCOff(RO_AXIS_W);
//    ROServoMCOff(RO_AXIS_t|RO_AXIS_r|RO_AXIS_z);
//    ROServoMCOff(RO_AXIS_w);
<<<<<<< HEAD
    rc = GASendDMCCommand(ghDMC, caMOCommand, caResp, MAXGASTR);
    ulServoFlag = 0;
    ulAMFlag = 0xFF;

=======
    rc = GASendDMCCommand(ghDMC, caMOCommand, caResp, MAXGASTR);
    ulServoFlag = 0;
    ulAMFlag = 0xFF;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    iPowerDisconnected = TRUE;
}


/****************************************************************
 *
 * Function:    ROCheckMotionError
 *
 * Abstract:    Checks the status words for motion errors
 *      and performs the appropriate shutdowns if there is one.
 *      Checks position error flags returned from galil and sets ER_flag.
 *      If a position error did occur, the error macro is triggered.
 *
 * Parameters:
 *      iCardNumArg (in) The Galil card number the error was on
 *      uBits       (in) The axes to check
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
void ROCheckMotionError(int iCardNumArg, unsigned uBitsArg)
{
    if (uBitsArg & 0xFF)
    {
        if (uBitsArg & 0x08) GASetExcIntOutp(0x0008);
        else GASetExcIntOutp(0);

        /* Set the error flag to indicate there was an error on the passed in axes. */
        ulERFlag |= (unsigned long)(uBitsArg);
        SSSetStatusWord(MOT_ERROR, (int)(ulERFlag & ulAllAMFlag));

        /* Set up to run the ERR macro. */
        if (MRGetMacroIntegerVars(MR_RUNNING_ERR))
        {
            MASetMacroIntegerVars(MR_PENDING_ERR, TRUE);
        }
        else if (!MRGetMacroIntegerVars(MR_RUNNING_CMER) &&
                 !MRGetMacroIntegerVars(MR_RUNNING_INTLK) &&
                 !MRGetMacroIntegerVars(MR_RUNNING_SSTOP))
        {
            MASetMacroIntegerVars(MR_INTR_SECT_FLAG, FALSE);
            MASetMacroIntegerVars(MR_ERR_INTERRUPT, MRGetMacroIntegerVars(MR_ERR_ENABLED));
        }

        if (ulERFlag & RO_AXIS_T)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_T);
            ulAMFlag |= RO_AXIS_T;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_T);
        }
        if (ulERFlag & RO_AXIS_R)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_R);
            ulAMFlag |= RO_AXIS_R;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_R);
        }
        if (ulERFlag & RO_AXIS_Z)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_Z);
            ulAMFlag |= RO_AXIS_Z;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_Z);
        }
        if (ulERFlag & RO_AXIS_W)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_W);
            ulAMFlag |= RO_AXIS_W;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_W);
        }
        if (ulERFlag & RO_AXIS_t)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_t);
            ulAMFlag |= RO_AXIS_t;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_t);
        }
        if (ulERFlag & RO_AXIS_r)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_r);
            ulAMFlag |= RO_AXIS_r;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_r);
        }
        if (ulERFlag & RO_AXIS_z)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_z);
            ulAMFlag |= RO_AXIS_z;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_z);
        }
        if (ulERFlag & RO_AXIS_w)
        {
            /* Stop motion, kill homing, clear the action buffer, and
             * set all appropriate flags for the axis. */
            ROStopMotion(RO_AXIS_w);
            ulAMFlag |= RO_AXIS_w;
            /* Servo off on the axis on the Galil card. */
            ROServoMCOff(RO_AXIS_w);
        }

        /* Abort any macro in progress. */
    if (!MRGetMacroIntegerVars(MR_RUNNING_CMER) && !MRGetMacroIntegerVars(MR_RUNNING_INTLK)
            && !MRGetMacroIntegerVars(MR_RUNNING_SSTOP) && !MRGetMacroIntegerVars(MR_RUNNING_ERR))
            MRAbortMacro();
    }
}


/****************************************************************
 *
 * Function:    ROWriteGalilPort
 *
 * Abstract:    Write to the Galil Output Port.
 *
 * Parameters:
 *      iCardNo     (in) The Galil card number whose port is to be
 *                          output to
 *      iBitNum     (in) If -1 then write the entire byte, otherwise should be
 *                          between 0 and 7 and write the specified bit
 *      iData       (in) The data to write, TRUE=set bit FALSE=Clear bit
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROWriteGalilPort(int iCardNo, int iBitNum, int iData)
{
    int iCardNum;

//    if (iCardNo == 0)
//        iCardNum = GA_CARD_0;
//    else if (iCardNo == 1)
//        iCardNum = GA_CARD_1;
//    else
//        return FAILURE;

<<<<<<< HEAD
//    return GAWriteGalilInputOutputPort(iCardNum, iBitNum, iData);
=======
//    return GAWriteGalilInputOutputPort(iCardNum, iBitNum, iData);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return 0;
}


/****************************************************************
 *
 * Function:    ROReadGalilPort
 *
 * Abstract:    Read from the Galil Input Port.
 *
 * Parameters:
 *      iCardNo     (in) The Galil card number whose port is to be
 *                          read from
 *
 * Returns:     The port value
 *
 ***************************************************************/
int ROReadGalilPort(int iCardNo)
{
    int iData;
    int iCardNum;

//    if (iCardNo == 0)
//        iCardNum = GA_CARD_0;
//    else if (iCardNo == 1)
//        iCardNum = GA_CARD_1;
//    else
//        return FAILURE;

//    if (GAReadGalilInputOutputPort(iCardNum, &iData) == FAILURE)
//        return FAILURE;

    return 0; //iData;
}


/****************************************************************
 *
 * Function:    ROGetERFlag
 *
 * Abstract:    Returns the ulERFlag variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulERFlag variable value
 *
 ***************************************************************/
unsigned long ROGetERFlag()
{
    return ulERFlag;
}


/****************************************************************
 *
 * Function:    ROGetLMPosFlag
 *
 * Abstract:    Returns the ulLMPosFlag variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulLMPosFlag variable value
 *
 ***************************************************************/
unsigned long ROGetLMPosFlag()
{
    return ulLMPosFlag;
}


/****************************************************************
 *
 * Function:    ROGetLMNegFlag
 *
 * Abstract:    Returns the ulLMNegFlag variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulLMNegFlag variable value
 *
 ***************************************************************/
unsigned long ROGetLMNegFlag()
{
    return ulLMNegFlag;
}


/****************************************************************
 *
 * Function:    ROGetLMPosMom
 *
 * Abstract:    Returns the ulLMPosMom variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulLMPosMom variable value
 *
 ***************************************************************/
unsigned long ROGetLMPosMom()
{
    return ulLMPosMom;
}


/****************************************************************
 *
 * Function:    ROGetLMNegMom
 *
 * Abstract:    Returns the ulLMNegMom variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulLMNegMom variable value
 *
 ***************************************************************/
unsigned long ROGetLMNegMom()
{
    return ulLMNegMom;
}

/****************************************************************
 *
 * Function:    ROGetHomed
 *
 * Abstract:    Returns the ulHomed variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulHomed variable value
 *
 ***************************************************************/
unsigned long ROGetHomed()
{
    return ulHomed;
}


/****************************************************************
 *
 * Function:    ROGetServoFlag
 *
 * Abstract:    Returns the ulServoFlag variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulServoFlag variable value
 *
 ***************************************************************/
unsigned long ROGetServoFlag()
{
    return ulServoFlag;
}


/****************************************************************
 *
 * Function:    ROGetAMFlag
 *
 * Abstract:    Returns the ulAMFlag variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulAMFlag variable value
 *
 ***************************************************************/
unsigned long ROGetAMFlag()
<<<<<<< HEAD
{
=======
{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//    ROUpdateTS(FALSE);
    return ulAMFlag;
}


/****************************************************************
 *
 * Function:    ROGetAllAMFlag
 *
 * Abstract:    Returns the ulAllAMFlag variable value
 *
 * Parameters:  None
 *
 * Returns:     The ulAllAMFlag variable value
 *
 ***************************************************************/
unsigned long ROGetAllAMFlag()
{
//    ROUpdateTS(FALSE);
    return ulAllAMFlag;
}


/****************************************************************
 *
 * Function:    ROSetERFlag
 *
 * Abstract:    Sets the ulERFlag variable
 *
 * Parameters:
 *      ulERFlagArg      (in) The new variable value
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetERFlag(unsigned long ulERFlagArg)
{
    ulERFlag = ulERFlagArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROSetHomed
 *
 * Abstract:    Sets the ulHomed variable
 *
 * Parameters:
 *      ulHomedArg   (in) The new variable value
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetHomed(unsigned long ulHomedArg)
{
    ulHomed = ulHomedArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROSetAMFlag
 *
 * Abstract:    Sets the ulAMFlag variable
 *
 * Parameters:
 *      ulAMFlagArg  (in) The new variable value
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetAMFlag(unsigned long ulAMFlagArg)
{
    ulAMFlag = ulAMFlagArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROSetEMOWasSet
 *
 * Abstract:    Sets the iEMOWasSet variable
 *
 * Parameters:
 *      iEMOWasSetArg  (in) The new variable value
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetEMOWasSet(int iEMOWasSetArg)
{
    if ((iEMOWasSetArg != TRUE) && (iEMOWasSetArg != FALSE))
    {
        return FAILURE;
    }

    iEMOWasSet = iEMOWasSetArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROSendDefinePositionCommand
 *
 * Abstract:    send the define position "DP" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lValueArg           (in) The values to send
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSendDefinePositionCommand(unsigned long ulEquipeAxisArg, long *lValueArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Galil "DP" command. This defines the current position to be the value passed. */
    iReturn = GASetValsLong(iCardNum, DEFINE_POSITION_COMMAND, uGalilAxes, lValueArg);

    if ((ulEquipeAxisArg & RO_AXIS_t) && (iReturn == SUCCESS))
        ulHomed |= RO_AXIS_t;

    return iReturn;
}


/****************************************************************
 *
 * Function:    MESendFindEdgeCommandCommand
 *
 * Abstract:    send the find edge command "FE" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSendFindEdgeCommand(unsigned long ulEquipeAxisArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Galil "FE" command. Finds the exact point at which the home switch trips. */
    iReturn = GASendAxesCommand(iCardNum, FIND_EDGE_COMMAND, uGalilAxes);
    /* Flag indicating that homing is in progress. */
    ulHoming |= ulEquipeAxisArg;

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROSendHomeCommand
 *
 * Abstract:    send the home command "HM" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSendHomeCommand(unsigned long ulEquipeAxisArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Galil "HM" command. Homes the axes. Homing is defined to be
     * the first encoder index pulse from the home switch. */
    iReturn = GASendAxesCommand(iCardNum, HOME_COMMAND, uGalilAxes);
    /* Flag indicating that homing is in progress. */
    ulHoming |= ulEquipeAxisArg;

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROSendBeginMotionCommand
 *
 * Abstract:    send the begin motion "BG" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSendBeginMotionCommand(unsigned long ulEquipeAxisArg)
{
    int iReturn;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    // VAC514 needs to switch the master and slave for T and R motion each time motion applies on these
    // !!!this function is used from Homing routine only, thus we hardcoded the Commanded position option to FALSE
    // We want to use GA (not GAC) when homing
    if(iDefineFlag & DFVAC514)
    {
        if(ulEquipeAxisArg & RO_AXIS_T)
        {
            if(ROElectronicGearingOFF() == FAILURE)
                return FAILURE;

            if(ROElectronicGearingON('X', 'Y', VAC514_T_GEARING_RATIO, FALSE) == FAILURE)
                return FAILURE;
        }
        else if(ulEquipeAxisArg & RO_AXIS_R)
        {
            if(ROElectronicGearingOFF() == FAILURE)
                return FAILURE;

            if(ROElectronicGearingON('Y', 'X', VAC514_R_GEARING_RATIO, FALSE) == FAILURE)
                return FAILURE;
        }
    }

    /* Galil "BG" command. Begins a programmed motion, e.g. "HM;BG\r". */
    iReturn = GASendAxesCommand(iCardNum, BEGIN_MOTION_COMMAND, uGalilAxes);
    /* Flag indicating motion is in progress. */
    ulAMFlag &= (unsigned)(~ulEquipeAxisArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROSendPositionAbsoluteCommand
 *
 * Abstract:    send the position absolute "PA" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      lValueArg           (in) The values to send
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSendPositionAbsoluteCommand(unsigned long ulEquipeAxisArg, long *lValueArg)
{
    int iReturn;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Unscale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, lValueArg, laPosition) == FAILURE)
        return FAILURE;

    /* Galil "PA" command. Sets an absolute position to move to. */
    iReturn = GASetValsLong(iCardNum, POSITION_ABS_COMMAND, uGalilAxes, laPosition);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROGetInterruptPositionCommand
 *
 * Abstract:    send the interrupt position "ID" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      plValueArg          (out) The values read
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROGetInterruptPositionCommand(unsigned long ulEquipeAxisArg, long *plValueArg)
{
    int iReturn=0;
//    long laPosition[8]={0,0,0,0,0,0,0,0};
    int i, iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Galil "ID" command. ??? */
//    iReturn = GAGetValsLong(iCardNum, OPT_INTR_POS_COMMAND, uGalilAxes, laPosition);
<<<<<<< HEAD

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	plValueArg[i] = giIPW[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		plValueArg[0] = giIPW[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		plValueArg[1] = giIPW[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		plValueArg[2] = giIPW[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		plValueArg[3] = giIPW[3];
=======

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	plValueArg[i] = giIPW[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		plValueArg[0] = giIPW[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		plValueArg[1] = giIPW[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		plValueArg[2] = giIPW[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		plValueArg[3] = giIPW[3];
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Scale the position from encoder counts to normal units. */
//    ROScalePos(ulEquipeAxisArg, laPosition, plValueArg);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROSetInterruptPositionCommand
 *
 * Abstract:    send the interrupt position "ID" to galil
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to read
 *      plValueArg          (in) The values to send
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetInterruptPositionCommand(unsigned long ulEquipeAxisArg, long *plValueArg)
{
    int iReturn = 0;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    int i, iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
<<<<<<< HEAD
        return FAILURE;

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPW[i] = plValueArg[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPW[0] = plValueArg[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPW[1] = plValueArg[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPW[2] = plValueArg[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPW[3] = plValueArg[3];

    /* Unscale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, plValueArg, laPosition) == FAILURE)
        return FAILURE;

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPWenc[i] = laPosition[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPWenc[0] = laPosition[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPWenc[1] = laPosition[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPWenc[2] = laPosition[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPWenc[3] = laPosition[3];

	for (i=0; i<8; ++i) giIPWFlag[i] = (giIPW[i] > 0) ? 1 : 0;

=======
        return FAILURE;

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPW[i] = plValueArg[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPW[0] = plValueArg[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPW[1] = plValueArg[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPW[2] = plValueArg[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPW[3] = plValueArg[3];

    /* Unscale the position from normal units to encoder counts. */
    if (ROUnscalePos(ulEquipeAxisArg, plValueArg, laPosition) == FAILURE)
        return FAILURE;

    if (ulEquipeAxisArg & RO_AXIS_ALL)
		for (i=0; i<3; ++i)	giIPWenc[i] = laPosition[i];
    else if (ulEquipeAxisArg & RO_AXIS_T)
		giIPWenc[0] = laPosition[0];
    else if (ulEquipeAxisArg & RO_AXIS_R)
		giIPWenc[1] = laPosition[1];
    else if (ulEquipeAxisArg & RO_AXIS_Z)
		giIPWenc[2] = laPosition[2];
    else if (ulEquipeAxisArg & RO_AXIS_W)
		giIPWenc[3] = laPosition[3];

	for (i=0; i<8; ++i) giIPWFlag[i] = (giIPW[i] > 0) ? 1 : 0;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    /* Galil "ID" command. ??? */
//    iReturn = GASetValsLong(iCardNum, OPT_INTR_POS_COMMAND, uGalilAxes, laPosition);

    return iReturn;
}


/****************************************************************
 *
 * Function:    ROPrepareScan
 *
 * Abstract:    Prepare axis for scanning.
 *
 * Parameters:  ulEquipeAxisArg     (in) The Equipe axis to move
 *
 * Returns:     none
 *
 ***************************************************************/
void ROPrepareScan(unsigned long ulEquipeAxisArg)
{
    long lValue[8];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes);

    iScanCardNum = iCardNum;
    /* Galil "RL" command. Flushes the position latching buffer by getting any
     * position that may still be available. */
    GAGetValsLong(iCardNum, REPORT_LATCH_POS_COMMAND, uGalilAxes, lValue);
    /* Galil "AL" command. Allows scanner interrupt to latch the axis position when they happened. */
    GASendAxesCommand(iCardNum, ARM_LATCH_COMMAND, uGalilAxes);
}



/****************************************************************
 *
 * Function:    ROElectronicGearingON
 *
 * Abstract:    Turns on Electronic gearing mode on GALIL card,
 *              uses Galil commands GA, GR and SG
 *              for the robots on which correct motion one axis requires
 *              to be involved two motors in master-slave relationship
 *
 * Parameters:  cMasterArg - axis which is set as master
 *              cSlaveArg - axis which is set as a slave
 *              fGearRatio - ratio between master and slave
 *              CommandedPosGearingArg - whether we use GAC or GA command for gearing,
 *                                          TRUE - commanded, FALSE - actual
 *
 * Returns:     SUCCESS/FAILURE
 *
 **************************************************************/
int ROElectronicGearingON(char cMasterArg, char cSlaveArg, double dGearRatioArg, BOOL bCommandedPosGearingArg)
{
    //char caString1[30], caString2[30], caString3[30];
    int iCardNum;  //, iReturn, iCounter;
    unsigned uGalilAxis;
    double daGearingRatio[8];
    long laSpecialGearing[8]={0,0,0,0,0,0,0,0};

    if(cMasterArg == cSlaveArg)
        return FAILURE;
    if((cMasterArg != 'X') && (cMasterArg != 'x') && (cMasterArg != 'Y') &&
            (cMasterArg != 'y') && (cMasterArg != 'Z') && (cMasterArg != 'z'))
        return FAILURE;

    iCardNum = GA_CARD_0;

    switch(cMasterArg)
    {
        case 'x' :
        case 'X' :
            uGalilAxis = GAXAXIS0;
            break;
        case 'y' :
        case 'Y' :
            uGalilAxis = GAYAXIS0;
            break;
        case 'z' :
        case 'Z' :
            uGalilAxis = GAZAXIS0;
            break;
        case 'w' :
        case 'W' :
            uGalilAxis = GAWAXIS0;
            break;
    }

    if(bCommandedPosGearingArg)
    {
        if(GASendAxesCommand(iCardNum, MASTER_AXIS_GEARING_COMMAND_P, uGalilAxis) == FAILURE)
            goto FAILURE_EXIT;
    }
    else
    {
        if(GASendAxesCommand(iCardNum, MASTER_AXIS_GEARING_COMMAND, uGalilAxis) == FAILURE)
            goto FAILURE_EXIT;
    }

    switch(cSlaveArg)
    {
        case 'x' :
        case 'X' :
            uGalilAxis = GAXAXIS0;
            daGearingRatio[0] = dGearRatioArg;
            laSpecialGearing[0] = 1;
            break;
        case 'y' :
        case 'Y' :
            uGalilAxis = GAYAXIS0;
            daGearingRatio[1] = dGearRatioArg;
            laSpecialGearing[1] = 1;
            break;
        case 'z' :
        case 'Z' :
            uGalilAxis = GAZAXIS0;
            daGearingRatio[2] = dGearRatioArg;
            laSpecialGearing[2] = 1;
            break;
        case 'w' :
        case 'W' :
            uGalilAxis = GAWAXIS0;
            daGearingRatio[3] = dGearRatioArg;
            laSpecialGearing[3] = 1;
            break;
    }

    if(GASetValsDouble(iCardNum, GEAR_RATIO_COMMAND, uGalilAxis, daGearingRatio) == FAILURE)
        goto FAILURE_EXIT;

    if(GASetValsLong(iCardNum, SPECIAL_GEARING_COMMAND, GAALLAXIS0, laSpecialGearing) == FAILURE)
        goto FAILURE_EXIT;


    return SUCCESS;

FAILURE_EXIT:

    return FAILURE;
}

/****************************************************************
 *
 * Function:    ROElectronicGearingOFF
 *
 * Abstract:    Turns off Electronic gearing mode on GALIL card, uses Galil commands GA, GR and SG
 *              for the robots on which correct motion one axis requires
 *              to be involved two motors in master-slave relationship.
 *              Because of the GALIL syntax, we need to interogate to find out who is the master currently,
 *              and then set the slave's gearing ratio to 0
 *
  * Returns:     SUCCESS/FAILURE
 *
 **************************************************************/
int ROElectronicGearingOFF()
{
    double dGearingRatio[8];
    int iCardNum, iFileType, iCounter;
    unsigned uGalilAxes;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(RO_AXIS_ALL, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    if(GAGetValsDouble(iCardNum, GEAR_RATIO_COMMAND, uGalilAxes, dGearingRatio) == FAILURE)
        goto FAILURE_EXIT;

    uGalilAxes = 0;
    for(iCounter = 0; iCounter < 4; iCounter++)
    {
        if(dGearingRatio[iCounter] != 0)
        {
            dGearingRatio[iCounter] = 0;
            uGalilAxes =  1 << iCounter;
            if(GASetValsDouble(iCardNum, GEAR_RATIO_COMMAND, uGalilAxes, dGearingRatio) == FAILURE)
                goto FAILURE_EXIT;
        }
    }

    return SUCCESS;

FAILURE_EXIT:

    return FAILURE;
}

/****************************************************************
 *
 * Function:    ROGetCardAxes
 *
 * Abstract:    Get the axes from the given the filetype.
 *      return axes or conflict indicator if w and W is in
 *      the same data file.
 *
 * Parameters:
 *      iFileTypeArg    (in) The mechanism file type (robot, pre, etc.)
 *
 * Returns:     Equipe axes corresponding to the filetype.
 *      RO_CONFLICT if W and w axes are used at the same time.
 *
 ***************************************************************/
int ROGetCardAxes(int iFileTypeArg)
{
	int iAxis;
	unsigned long ulEquipeAxes = 0;

    for (iAxis=0; iAxis<iNumAxes; iAxis++)
    {
        /* ...if the axis matches the passed in parameter file request... */
        if (uaAxisMap[iAxis][MECH_TYPE] == (unsigned)(iFileTypeArg))
        {
            /* ...pull its matched Equipe axis and add it to the cumulative bit pattern. */
            ulEquipeAxes |= uaAxisMap[iAxis][EQUIPE_AXIS];
        }
    }

	if((ulEquipeAxes & RO_AXIS_W) && (ulEquipeAxes & RO_AXIS_w))
		return RO_CONFLICT;
    return ulEquipeAxes;
}


/****************************************************************
 *
 * Function:    ROInitGalil
 *
 * Abstract:    Gets the Equipe axes combination from a particular
 *      parameter file. Then it checks that the combination will
 *      be read and written to only 1 Galil card at a time. In the
 *      case of the auxiliary file, we might try to access both
 *      Galil cards with 1 axes combination. This function precludes
 *      that possibility.
 *
 * Parameters:
 *      iFileTypeArg    (in) The mechanism file type (robot, pre, etc.)
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROInitGalil(int iFileTypeArg)
{
	unsigned long ulAxes = 0;

	ulAxes = ROGetCardAxes(iFileTypeArg);
	if(ulAxes == RO_CONFLICT)
	{
		if(ROInitGalilLocal(RO_AXIS_W)==FAILURE)
			return FAILURE;
		if(ROInitGalilLocal(RO_AXIS_w)==FAILURE)
			return FAILURE;
	}
	else
	{
		if(ROInitGalilLocal(ulAxes)==FAILURE)
			return FAILURE;
	}
	return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROServiceBrake
 *
 * Abstract:    Service the appropriate brake, robot or vacuum indexer.
 *
 * Parameters:
 *      iParamFileArg   (in) Which device to de/activate brake
 *      iOnOffArg       (in) Turn the brake on or off
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROServiceBrake(int iParamFileArg, int iOnOffArg)
{
//    long lZAxisBrake[8] = {0,0,0,0,0,0,0,0};
//    int  iVacIndex;
<<<<<<< HEAD
//	char cpBuf[16];
    int iValue;

=======
//	char cpBuf[16];
    int iValue;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    if (iParamFileArg == ROBOTFILE)
    {
        /* Get the Z brake values from the datafile. */
//        FIOGetParamVals(ROBOTFILE, H_VALUE, lZAxisBrake);
        /* If a Z brake activation is required... */
//        if (lZAxisBrake[0] > 0)
//        {
            /* ...activate the Z brake. */
<<<<<<< HEAD
            /* WARNING: In ME domain card number 0 is GA_CARD_0. */
//			if (iOnOffArg)
//				GASendReceiveGalil( GA_CARD_1, (char *)"SB3\xD", cpBuf );
//			else
//				GASendReceiveGalil( GA_CARD_1, (char *)"CB3\xD", cpBuf );
	iValue = inb(IO_ROBOT_OUTPUT_A);
	iValue &= 0xCF;
	if (iOnOffArg)
	    iValue |= 0x10;       // 5th bit on
//	else
//	    iValue &= 0xEF;	      // off
	outb(iValue, IO_ROBOT_OUTPUT_A);

=======
            /* WARNING: In ME domain card number 0 is GA_CARD_0. */
//			if (iOnOffArg)
//				GASendReceiveGalil( GA_CARD_1, (char *)"SB3\xD", cpBuf );
//			else
//				GASendReceiveGalil( GA_CARD_1, (char *)"CB3\xD", cpBuf );
	iValue = inb(IO_ROBOT_OUTPUT_A);
	iValue &= 0xCF;
	if (iOnOffArg)
	    iValue |= 0x10;       // 5th bit on
//	else
//	    iValue &= 0xEF;	      // off
	outb(iValue, IO_ROBOT_OUTPUT_A);

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//            ROWriteGalilPort(0, (int)lZAxisBrake[0], iOnOffArg);
 //       }
    }
//    else if (iParamFileArg == PREALIGNFILE)
//    {
        /* Get the brake values from the datafile. */
//        FIOGetParamVals(PREALIGNFILE, H_VALUE, lZAxisBrake);
//		if (iOnOffArg)
<<<<<<< HEAD
//			GASendReceiveGalil( GA_CARD_1, (char *)"SB4\xD", cpBuf );
//		else
//			GASendReceiveGalil( GA_CARD_1, (char *)"CB4\xD", cpBuf );
=======
//			GASendReceiveGalil( GA_CARD_1, (char *)"SB4\xD", cpBuf );
//		else
//			GASendReceiveGalil( GA_CARD_1, (char *)"CB4\xD", cpBuf );
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//        for (iVacIndex=1; iVacIndex<=2; iVacIndex++)
//        {
//            /* If a brake activation is required... */
//            if (lZAxisBrake[iVacIndex] > 0)
//            {
//                /* ...activate the brake. */
//                /* WARNING: In ME domain card number 1 is GA_CARD_1. */
//                ROWriteGalilPort(1, (int)lZAxisBrake[iVacIndex]-40, iOnOffArg);
//            }
//        }
//    }
    else if (iParamFileArg == AUXFILE)
    {
<<<<<<< HEAD
	iValue = inb(IO_ROBOT_OUTPUT_B);
	if (iOnOffArg)
	    iValue |= 0x22;       // 2nd and 6th bits on (disengages brake)
	else
	    iValue &= 0xDD;	  // 2nd and 6th bits off (engage brake)

	outb(iValue, IO_ROBOT_OUTPUT_B);

=======
	iValue = inb(IO_ROBOT_OUTPUT_B);
	if (iOnOffArg)
	    iValue |= 0x22;       // 2nd and 6th bits on (disengages brake)
	else
	    iValue &= 0xDD;	  // 2nd and 6th bits off (engage brake)

	outb(iValue, IO_ROBOT_OUTPUT_B);

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        /* Get the track brake values from the datafile. */
 //       FIOGetParamVals(AUXFILE, H_VALUE, lZAxisBrake);
        /* If a track brake activation is required... */
   //     if (lZAxisBrake[0] > 0)
     //   {
		//	int iCardNum, iFileType;
//			unsigned uGalilAxes;

            /* ...activate the track brake. */
	//		unsigned long ulAxis=ROGetSpecialAxis(RO_TRACK);

		//	if(FAILURE==ROValidAxis(ulAxis, &iCardNum, &iFileType, &uGalilAxes))
			//	return FAILURE;

//			if (iOnOffArg)
<<<<<<< HEAD
//				GASendReceiveGalil( GA_CARD_1, (char *)"SB5\xD", cpBuf );
//			else
//				GASendReceiveGalil( GA_CARD_1, (char *)"CB5\xD", cpBuf );
=======
//				GASendReceiveGalil( GA_CARD_1, (char *)"SB5\xD", cpBuf );
//			else
//				GASendReceiveGalil( GA_CARD_1, (char *)"CB5\xD", cpBuf );
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//			if(iCardNum==2)
//				ROWriteGalilPort(1, (int)lZAxisBrake[0]-40, iOnOffArg);
//			else
//				ROWriteGalilPort(0, (int)lZAxisBrake[0], iOnOffArg);
//        }
    }
//    else
//        return FAILURE;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    ROSetCM
 *
 * Abstract:    Sets the the internal variable which keeps the current
 *              setting of type of gearing for VAC514 robots
 *
 * Parameters:
 *
 *      bModeArg - TRUE - gearing to the commandedposition
 *               - FALSE - gearing to the actual position
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
void ROSetCM(BOOL bModeArg)
{
    bCommandedGearingMode = bModeArg;
    return ;
}
/****************************************************************
 *
 * Function:    ROGetCM
 *
 * Abstract:    Returns the the internal variable which keeps the current
 *              setting of type of gearing for VAC514 robots
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
BOOL ROGetCM()
{
    return bCommandedGearingMode;
}
/****************************************************************
 *
 * Function:    ROReadGalilModulo
 *
 * Abstract:    Interogates Galil for the modulo value and returns it back
 *
 * Parameters:  plModuloArg - variable to store the read modulo
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
long ROReadGalilModulo(long *plModuloArg)
{
    long laModulo[8]={0,0,0,0,0,0,0,0};
    long laEncModulo[8]={0,0,0,0,0,0,0,0};

    if(GAGetValsLong(GA_CARD_0, MODULO_COMMAND, GAXAXIS0, laEncModulo) == FAILURE)
        return FAILURE;

    if(ROScalePos(RO_AXIS_T, laEncModulo, laModulo) == FAILURE)
        return FAILURE;

    *plModuloArg = laModulo[0];

    return SUCCESS;
}
/****************************************************************
 *
 * Function:    ROSetGalilModulo
 *
 * Abstract:    Sets the modulo in Galil to the desired value
 *
 * Parameters:  lModuloArg - value for modulo to be set to
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
long ROSetGalilModulo(long lModuloArg)
{
    long laModulo[8]={0,0,0,0,0,0,0,0};
    long laEncModulo[8]={0,0,0,0,0,0,0,0};

    laModulo[0] = lModuloArg;

    if(ROUnscalePos(RO_AXIS_T, laModulo, laEncModulo) == FAILURE)
        return FAILURE;

    if(GASetValsLong(GA_CARD_0, MODULO_COMMAND, GAXAXIS0, laEncModulo) == FAILURE)
        return FAILURE;

    return SUCCESS;
}
<<<<<<< HEAD



=======



>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int ROEncoderToEU(unsigned long ulEquipeAxisArg, long lEncoderArg, long *lScaledPositionArg)
{
    long lMechRatio[3], lArmLength[3];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // temporary double vars are moved from global to local to protect from interrupt.
    double dTemp1, dTemp2, dTemp3, dTemp4;  //, dTemp5;
    double dPosOffset;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the arm length of the relevant axes from the datafile.
     * This pertains to R axis only. */
    if (FIOGetParamVals(iFileType, ARM_LENGTH, lArmLength) == FAILURE)
        return FAILURE;
    /* Get the mechanical ratio of the relevant axes from the datafile. */
    if (FIOGetParamVals(iFileType, MECHANICAL_RATIO, lMechRatio) == FAILURE)
        return FAILURE;

    /* T axis. */
    if ((ulEquipeAxisArg & RO_AXIS_T ) && (lEncoderArg != 0))
    {
        if(iDefineFlag & DFVAC514)
            dTemp1 = dMechRatioVAC514 / 1.0e5;
        else
            dTemp1 = (double)(lMechRatio[0]) / 1.0e5;

        dTemp2 = dTemp1 * (double)(lEncoderArg);
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_t) && (lEncoderArg != 0))
    {
        dTemp1 = (double)(lMechRatio[0]) / 1.0e5;

        dTemp2 = dTemp1 * (double)(lEncoderArg);
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_R ) && (lEncoderArg != 0))
    {
        dTemp1 = (double)(lMechRatio[1]) * (double)(lEncoderArg);
        dTemp2 = dTemp1 / 1.0e9;
        dTemp3 = sin(dTemp2);
        dTemp4 = (double)(lArmLength[1]) * dTemp3;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp4 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }
    /* r axis. */
    else if ((ulEquipeAxisArg & RO_AXIS_r) && (lEncoderArg != 0))
    {
        dTemp1 = (double)(lMechRatio[1]) * (double)(lEncoderArg);
        dTemp4 = dTemp1 / 1.0e5;
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp4 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }
    /* Z axis. */
    else if ((ulEquipeAxisArg & RO_AXIS_Z ) && (lEncoderArg != 0))
    {
        dTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dTemp2 = dTemp1 * (double)(lEncoderArg);
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }
    /* z axis. */
    else if ((ulEquipeAxisArg & RO_AXIS_z) && (lEncoderArg != 0))
    {
        dTemp1 = (double)(lMechRatio[2]) / 1.0e5;
        dTemp2 = dTemp1 * (double)(lEncoderArg);
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }
    /* W axis. */
    else if ((ulEquipeAxisArg & RO_AXIS_W ) && (lEncoderArg != 0))
    {
        if (ulEquipeAxisArg == ROGetSpecialAxis(RO_DUAL_ARM))
        {
            if (lArmLength[0] == 0) lArmLength[0] = 10500;
            dTemp1 = (double)(lMechRatio[0]) * (double)(lEncoderArg);
            dTemp2 = dTemp1 / 1.0e9;
            dTemp3 = sin(dTemp2);
            dTemp4 = (double)(lArmLength[0]) * dTemp3;
            if (lEncoderArg < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp4 + dPosOffset;
            *lScaledPositionArg = (long)(dTemp1);
        }
        else
        {
            if ((ulEquipeAxisArg == RO_AXIS_W) && (~iDefineFlag & DFSS))
                dTemp1 = (double)(lMechRatio[0]) / 1.0e5;
            else /* if (ulEquipeAxis == RO_AXIS_w) */
                dTemp1 = (double)(lMechRatio[1]) / 1.0e5;

            dTemp2 = dTemp1 * (double)(lEncoderArg);
            if (lEncoderArg < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp2 + dPosOffset;
            *lScaledPositionArg = (long)(dTemp1);
        }
    }
    /* w axis. */
    else if ((ulEquipeAxisArg & RO_AXIS_w) && (lEncoderArg != 0))
    {
        dTemp1 = (double)(lMechRatio[1]) / 1.0e5;

        dTemp2 = dTemp1 * (double)(lEncoderArg);
        if (lEncoderArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lScaledPositionArg = (long)(dTemp1);
    }

    return SUCCESS;
}
<<<<<<< HEAD

//
// ROEUToEncoder
//
=======

//
// ROEUToEncoder
//
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
int ROEUToEncoder(unsigned long ulEquipeAxisArg, long lPositionArg, long *lUnscaledPositionArg)
{
    long lMechRatio[3], lArmLength[3];
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // temporary double vars are moved from global to local to protect from interrupt.
    double dTemp1, dTemp2, dTemp3;  //, dTemp4; //, dTemp5;
    double dPosOffset, dInvAngle, dInvAngleTemp;

    /* Validate that the axis exists in the system. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Get the arm length of the relevant axes from the datafile.
     * This pertains to R axis only. */
    if (FIOGetParamVals(iFileType, ARM_LENGTH, lArmLength) == FAILURE)
        return FAILURE;
    /* Get the mechanical ratio of the relevant axes from the datafile. */
    if (FIOGetParamVals(iFileType, MECHANICAL_RATIO, lMechRatio) == FAILURE)
        return FAILURE;

    /* T axis. */
    if ((ulEquipeAxisArg & RO_AXIS_T ) && (lPositionArg != 0))
    {
        if(iDefineFlag & DFVAC514)
            dTemp1 = 1.0e5 / dMechRatioVAC514;
        else
            dTemp1 = 1.0e5 / (double)(lMechRatio[0]);

        dTemp2 = dTemp1 * (double)(lPositionArg);
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        } 
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_t) && (lPositionArg != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[0]);

        dTemp2 = dTemp1 * (double)(lPositionArg);
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        } 
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
<<<<<<< HEAD
    }
=======
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    else if ((ulEquipeAxisArg & RO_AXIS_R ) && (lPositionArg != 0))
    {
        dInvAngleTemp = 1.0 / (double)(lArmLength[1]);
        dInvAngle = dInvAngleTemp * (double)(lPositionArg);
        if (dInvAngle > 1.0)
        {
            if (dInvAngle > 2.0)
                dInvAngle = 1.0;
            else
               dInvAngle = 2.0 - dInvAngle;
        }
        else if (dInvAngle < -1.0)
        {
            if (dInvAngle < -2.0)
               dInvAngle = -1.0;
            else
                dInvAngle = (-2.0) - dInvAngle;
        }

        dTemp1 = 1.0e9 / (double)(lMechRatio[1]);
        dTemp2 = asin(dInvAngle);
        dTemp3 = dTemp1 * dTemp2;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp3 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_r) && (lPositionArg != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[1]);
        dTemp3 = dTemp1 * (double)(lPositionArg);
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp3 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_Z ) && (lPositionArg != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[2]);
        dTemp2 = dTemp1 * (double)(lPositionArg);
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_z) && (lPositionArg != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[2]);
        dTemp2 = dTemp1 * (double)(lPositionArg);
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
    }
    else if ((ulEquipeAxisArg & RO_AXIS_W ) && (lPositionArg != 0))
    {
        if (ulEquipeAxisArg == ROGetSpecialAxis(RO_DUAL_ARM))
        {
            /* ??? */
            if (lArmLength[0] == 0) lArmLength[0] = 10500;
            dInvAngleTemp = 1.0 / (double)(lArmLength[0]);
            dInvAngle = dInvAngleTemp * (double)(lPositionArg);
            if (dInvAngle > 1.0)
            {
                if (dInvAngle > 2.0)
                    dInvAngle = 1.0;
                else
                    dInvAngle = 2.0 - dInvAngle;
            }
            else if (dInvAngle < -1.0)
            {
                if (dInvAngle < -2.0)
                    dInvAngle = -1.0;
                else
                    dInvAngle = (-2.0) - dInvAngle;
            }

            dTemp1 = 1.0e9 / (double)(lMechRatio[0]);
            dTemp2 = asin(dInvAngle);
            dTemp3 = dTemp1 * dTemp2;
            if (lPositionArg < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp3 + dPosOffset;
            *lUnscaledPositionArg = (long)(dTemp1);
        }
        else
        {
            if ((ulEquipeAxisArg == RO_AXIS_W) && (~iDefineFlag & DFSS))
                dTemp1 = 1.0e5 / (double)(lMechRatio[0]);
            else /* if (ulEquipeAxis == RO_AXIS_w) */
                dTemp1 = 1.0e5 / (double)(lMechRatio[1]);

            dTemp2 = dTemp1 * (double)(lPositionArg);
            if (lPositionArg < 0)
            {
                dPosOffset = -0.5;
            }
            else
            {
                dPosOffset = 0.5;
            }

            dTemp1 = dTemp2 + dPosOffset;
            *lUnscaledPositionArg = (long)(dTemp1);
        }
<<<<<<< HEAD
    }
=======
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    else if ((ulEquipeAxisArg & RO_AXIS_w) && (lPositionArg != 0))
    {
        dTemp1 = 1.0e5 / (double)(lMechRatio[1]);

        dTemp2 = dTemp1 * (double)(lPositionArg);
        if (lPositionArg < 0)
        {
            dPosOffset = -0.5;
        }
        else
        {
            dPosOffset = 0.5;
        }

        dTemp1 = dTemp2 + dPosOffset;
        *lUnscaledPositionArg = (long)(dTemp1);
    }
    else
    {
        *lUnscaledPositionArg = 0;
    }

    return SUCCESS;
}
