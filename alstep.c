/******************************************************************************\
/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : alstep.c
 * Description          : Functions and Declarations used during the alignment process
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
 * Program:     Aligner Module Process Functions
 *
 * Functions:
 *
 * Description:  Functions and Declarations used during the alignment process
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
\******************************************************************************/
#include <stdlib.h>
#include <string.h>
<<<<<<< HEAD
#include <pthread.h>
#include <unistd.h>
=======
#include <pthread.h>
#include <unistd.h>
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include <time.h>

#include "sck.h"
#include "alk.h"
#include "alpre.h"
#include "alstep.h"
#include "alcomp.h"
#include "alisr.h"
#include "fio.h"
#include "romain.h"
#include "ro.h"
#include "sctim.h"
#include "ser.h"
#include "cmdfns.h"
#include "roga.h"
<<<<<<< HEAD

int MCCheckWaferOnChuck();
void ALChuckVacuum(int iFlagArg);
void ALPinVacuum(int iFlagArg);
int ALWaferOnChuck(void);

=======

int MCCheckWaferOnChuck();
void ALChuckVacuum(int iFlagArg);
void ALPinVacuum(int iFlagArg);
int ALWaferOnChuck(void);

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
extern long glZPos1[8], glZPos2[8], glRPos1[8], glRPos2[8]; 

long glMaxOffsetVector;         // max_offset_vec - max offset for only 1
int giMaxNumCorr;               // max_no_of_cor
int giDataEdgeIndex;            // data_edge_i - index in data edge meas.
int giWABalFlatSubStep;         // bfp_substep
int giWASubStep;                // bi_substep
double gdCountIn100th;          // count_in_100th
<<<<<<< HEAD

int giBCORSubStep;
int giBCORcount;
int giBCORStepOff;
int giBCORDone;
int giFlatSubStepOff;

int giRndDataCount;

pthread_t thCorrectOffset;
void *procALCorrectOffset(void *ptr);

int giALCorrectOffsetDone;
int giThreadCreated = 0;

void *procALCorrectOffset(void *ptr)
{
    int iRetVal;

    giALCorrectOffsetDone = 0;
=======

int giBCORSubStep;
int giBCORcount;
int giBCORStepOff;
int giBCORDone;
int giFlatSubStepOff;

int giRndDataCount;

pthread_t thCorrectOffset;
void *procALCorrectOffset(void *ptr);

int giALCorrectOffsetDone;
int giThreadCreated = 0;

void *procALCorrectOffset(void *ptr)
{
    int iRetVal;

    giALCorrectOffsetDone = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    iRetVal = ALOffsetAnglePosition();
    if( iRetVal < 0 )
    {
        if( iRetVal == -1 )
            giTrace = 6110;
        else if( iRetVal == -2 )
            giTrace = 6120;
        else if( iRetVal == -3 )
            giTrace =  6130;
        else if( iRetVal == -4 )
            giTrace =  6140;
        else if( iRetVal == -5 )
            giTrace =  6150;
        giPrevNumCorr = giNumCorr;
        giNumCorr++;
<<<<<<< HEAD
        giWAStep = 1;                   // reset alignment step to begining
		giALCorrectOffsetDone = -1;
    }
    else
    {
        ALCorrectOffset();
        giALCorrectOffsetDone = 1;
    }
}
=======
        giWAStep = 1;                   // reset alignment step to begining
		giALCorrectOffsetDone = -1;
    }
    else
    {
        ALCorrectOffset();
        giALCorrectOffsetDone = 1;
    }
}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

/*******************************************************************************
 * Function:    ALGetWaferRoundSeg
 * Abstract:    Get and return the size of the round segment of the requested wafer
 *              size
 * Parameters:  iWaferSizeArg
 *              fpSegmentArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    ROUND_SEG_XXXX
 ******************************************************************************/
int ALGetWaferRoundSeg(int iWaferSizeArg, double *dpSegmentArg)
{
    switch(iWaferSizeArg)
    {
        case AL_WAFER_SIZE_3 :
        case AL_WAFER_SIZE_76 :
            *dpSegmentArg = gdCountInDeg * 1.376;
            break;
        case AL_WAFER_SIZE_80 :
            *dpSegmentArg = gdCountInDeg * 1.250;
            break;
        case AL_WAFER_SIZE_90 :
            *dpSegmentArg = gdCountInDeg * 1.111;
            break;
        case AL_WAFER_SIZE_4 :
        case AL_WAFER_SIZE_100 :
            *dpSegmentArg = gdCountInDeg * 1.000;
            break;
        case AL_WAFER_SIZE_5 :
        case AL_WAFER_SIZE_125 :
            *dpSegmentArg = gdCountInDeg * 0.800;
            break;
        case AL_WAFER_SIZE_6 :
        case AL_WAFER_SIZE_150 :
            *dpSegmentArg = gdCountInDeg * 0.700;
            break;
        case AL_WAFER_SIZE_8 :
        case AL_WAFER_SIZE_200 :
            *dpSegmentArg = gdCountInDeg * 0.500;
            break;
        default:                        // 200mm wafer
            *dpSegmentArg = gdCountInDeg * 0.500;
            break;
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSetWaferSize
 * Abstract:    Setup globals used for wafer alignment
 * Parameters:  iWaferSizeArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    set_wafer_size()
 ******************************************************************************/
int ALSetWaferSize(int iWaferSizeArg)
{
    int iSizeErr=FALSE;
    int iIndex;
    long lFirstPixel, lCCDAngle; // lWaferType;
    //psCalTable psCalTbl;
	int iCCDPixelDesc, iCCDAngleDesc;

    switch(iWaferSizeArg)
    {
        case AL_WAFER_SIZE_3 :          // 3" or 76.2mm wafer
        case AL_WAFER_SIZE_76 :
			if(ALSetDefaultWaferValue(iWaferSizeArg, 1,AL_CCD_1, AL_PRIM_FLAT_3, AL_MAIN_FLAT_3,
							AL_ROUND_LEVEL_3, AL_MAX_OFF_VEC_3, AL_OFFSET_SIZE_COR_3, AL_MIN_FL_LEVEL_DEC_3)==FAILURE)
				return FAILURE;
            break;
        case AL_WAFER_SIZE_80 :         // 80mm
			if(ALSetDefaultWaferValue(iWaferSizeArg, 1,AL_CCD_1, AL_PRIM_FLAT_3, AL_MAIN_FLAT_3,
							AL_ROUND_LEVEL_80, AL_MAX_OFF_VEC_80, AL_OFFSET_SIZE_COR_80, AL_MIN_FL_LEVEL_DEC_80)==FAILURE)
				return FAILURE;
            break;
        case AL_WAFER_SIZE_90 :         // 90mm
			if(ALSetDefaultWaferValue(iWaferSizeArg, 1,AL_CCD_1, AL_PRIM_FLAT_100, AL_MAIN_FLAT_100,
							AL_ROUND_LEVEL_90, AL_MAX_OFF_VEC_90, AL_OFFSET_SIZE_COR_90, AL_MIN_FL_LEVEL_DEC_90)==FAILURE)
				return FAILURE;
            break;
        case AL_WAFER_SIZE_4 :          // 4"
        case AL_WAFER_SIZE_100 :        // 100mm
			if(ALSetDefaultWaferValue(iWaferSizeArg, 1,AL_CCD_1, AL_PRIM_FLAT_100, AL_MAIN_FLAT_100,
							AL_ROUND_LEVEL_100, AL_MAX_OFF_VEC_100, AL_OFFSET_SIZE_COR_100, AL_MIN_FL_LEVEL_DEC_100)==FAILURE)
				return FAILURE;
            break;
        case AL_WAFER_SIZE_5 :          // 5"
        case AL_WAFER_SIZE_125 :        // 125 mm
			if(ALSetDefaultWaferValue(iWaferSizeArg, 2,AL_CCD_2, AL_PRIM_FLAT_125, AL_MAIN_FLAT_125,
							AL_ROUND_LEVEL_125, AL_MAX_OFF_VEC_125, AL_OFFSET_SIZE_COR_125, AL_MIN_FL_LEVEL_DEC_125)==FAILURE)
				return FAILURE;
            break;
        case AL_WAFER_SIZE_6 :          // 6"
        case AL_WAFER_SIZE_150 :        // 150mm
			if(ALSetDefaultWaferValue(iWaferSizeArg, 2,AL_CCD_2, AL_PRIM_FLAT_150, AL_MAIN_FLAT_150,
							AL_ROUND_LEVEL_150, AL_MAX_OFF_VEC_150, AL_OFFSET_SIZE_COR_150, AL_MIN_FL_LEVEL_DEC_150)==FAILURE)
				return FAILURE;
            break;
        case AL_WAFER_SIZE_8 :          // 8"
        case AL_WAFER_SIZE_200 :        // 200mm
			if(ALSetDefaultWaferValue(iWaferSizeArg, 3,AL_CCD_3, AL_PRIM_FLAT_200, AL_MAIN_FLAT_200,
							AL_ROUND_LEVEL_200, AL_MAX_OFF_VEC_200, AL_OFFSET_SIZE_COR_200, AL_MIN_FL_LEVEL_DEC_200)==FAILURE)
				return FAILURE;
            break;
        default:
            iSizeErr = TRUE;
			if(ALSetDefaultWaferValue(iWaferSizeArg, 3,AL_CCD_3, AL_PRIM_FLAT_200, AL_MAIN_FLAT_200,
							AL_ROUND_LEVEL_200, AL_MAX_OFF_VEC_200, AL_OFFSET_SIZE_COR_200, AL_MIN_FL_LEVEL_DEC_200)==FAILURE)
				return FAILURE;
            break;
    }

    // set up common to all wafer sizes

    giMaxNumCorr = AL_MAX_CORRECTIONS;
    if( giCCDNumber == 1)
    {
		iCCDPixelDesc = CCD1_FIRST_PIXEL;
		iCCDAngleDesc = CCD1_ANGLE;
    }
    else
    {
		iCCDPixelDesc = CCD2_FIRST_PIXEL;
		iCCDAngleDesc = CCD2_ANGLE;
    }
    if( FIOGetParamWaferVals(iCCDPixelDesc,&lFirstPixel) != SUCCESS )
        return FAILURE;
    if( FIOGetParamWaferVals(iCCDAngleDesc,&lCCDAngle) != SUCCESS )
        return FAILURE;
    giCCDFirstPixelPos = (int)lFirstPixel;
    glCCDAnglePosition = lCCDAngle;

    for( iIndex=1; iIndex<AL_OFFSET_SZ_CORR; iIndex++ )
        gdOffsetSizCor[iIndex] = gdOffsetSizCor[0]; // loc 0 set accord to wafer size

    // if the calibration table exists, than it should be reflected in the set-up
	if(ALLoadWaferCal()==FAILURE)
		return FAILURE;

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALAbort
 * Abstract:    Abort the wafer alignment process
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    abort_alignment()
 ******************************************************************************/
int ALAbort(void)
{
    giNumCorr = 0;
    giAligning = FALSE;
    giWAStep = 0;
    return ALStopMeasurement();
}
/*******************************************************************************
 * Function:    ALBegin
 * Abstract:    Begin the alignment process
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_begin()
 ******************************************************************************/
int ALBegin(void)
{
<<<<<<< HEAD
    long laScaledPos[8]={100000,100000,100000,100000,100000,100000,100000,100000}, 
=======
    long laScaledPos[8]={100000,100000,100000,100000,100000,100000,100000,100000}, 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		 laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    // initialize trace table and trace variable
    for( giTrace=0; giTrace<10; giTrace++ )
        giaTraceTable[giTrace] = 0;

    giTrace = 0;

<<<<<<< HEAD
    // initialize the alignment status start value
=======
    // initialize the alignment status start value
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    giAlignmentStatus = AL_FLAT_NOT_FOUND | AL_OFFSET_NOT_FOUND | 0x1000;

    if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
        return FAILURE;
    gdCountInDeg = (double)(laUnscaledPos[giAxisIndx]) / 1000.0;
    gdCountInRadians = 10000.0/(double)PIDEF;  // rlg - I don't believe this is ever used with this value or that this value is correct
    giPrevNumCorr = giNumCorr = 0;                   // number of corrections during this alignment

    if( TISetCounter(giWATimer1,20000) == FAILURE )  // set LL timer for alignment
        return FAILURE;

    giTrace = 10;                // is the system homed, so that alignment can begin
    if( ROAxisReadyToMove(gulAllAxes, &iCardNum, &iFileType, &uGalilAxes) == FAILURE )
    {
        giAlignmentStatus = AL_SYSTEM_NOT_HOMED;
        return FAILURE;
    }

    giWASubStep = 0;
<<<<<<< HEAD
    giBCORSubStep = 0;
    giBCORStepOff = 0;
    giBCORcount = 0;
    giBCORDone = 0;
=======
    giBCORSubStep = 0;
    giBCORStepOff = 0;
    giBCORcount = 0;
    giBCORDone = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    giFlatSubStepOff = 0;
    giTrace = 20;
    giWAStep++;
    gulDuplicateCount = 0;              // used to count the number of times through the interrupt
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALInit
 * Abstract:    Initialize variables and move the wafer to the chuck.  This is the
 *              first step in the wafer alignment process
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_init
 ******************************************************************************/
int ALInit(int iTypeAlignmentArg)
{
    long lWaferSize, lChuckVacUse;
    long laScaledPos[8]={0,0,0,0,0,0,0,0};
    long laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    long lPosBuf[8]={0,0,0,0,0,0,0,0};
<<<<<<< HEAD
    int  iWaferOnChuck = TRUE;
    struct timespec tv;

=======
    int  iWaferOnChuck = TRUE;
    struct timespec tv;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    tv.tv_sec = 0;

    // alignment will not proceed if motion is not complete.
    if ( (ROGetAMFlag() & gulAllAxes) != gulAllAxes )
    {
        return SUCCESS; 
    }

    // substep 1
    if( giWASubStep == 0 )
    {
        if( giTrace != 1020 && giTrace != 1021 )
        {
            if( giNumCorr > 0 )         // alignment being repeated
                giaTraceTable[giNumCorr-1] = giTrace;
            giTrace = 1010;
        }
        giWABalFlatSubStep = 0;
        giWaitForFlatPositioning = FALSE;
        giWaitForFlatMeasuring = FALSE;
        if( giNumCorr > 0 )         // alignment being repeated
        {
            giAlignmentStatus |= AL_ALIGNMENT_REPEAT;
            if( giNumCorr > giMaxNumCorr )
            {
                giAlignmentStatus |= AL_MAX_RETRIES;
                return FAILURE;
            }
            if( giTrace < 1020 )
                giTrace = 1020;
            // square wafer will move pin and chuck
            if( iTypeAlignmentArg == BASQ )         // if square wafer
            {
                if( giTrace == 1020 )
                {
                    // alignment in progress, wait for positioning
                    if( (ROGetAMFlag()&gulAllAxes) != gulAllAxes )
                    {
                        // motion error
                        if( (ROGetServoFlag() | ROGetERFlag()) & gulAllAxes )
                            return FAILURE;
                        else
                            return SUCCESS;
                    }
                    giTrace = 1021;
                    // read current axis position
                    if( ROReadCurrentPosition(gulAxis,lPosBuf) != SUCCESS )
                        return FAILURE;
                    if( lPosBuf[giAxisIndx] > 18000L )
                    {
                        lPosBuf[giAxisIndx] = 36000L;
                    }
                    else
                    {
                        lPosBuf[giAxisIndx] = 0L;
                    }
                    // fixme RLG NOTE - to fix me square wafer problem position absolute here
                    // and movetoabs call below should be to location 0
                }
                if( (ROGetAMFlag()&gulAllAxes) != gulAllAxes ) // alignment in progress
                {
                    if( (ROGetServoFlag() | ROGetERFlag()) & gulAllAxes )  // motion error
                        return FAILURE;
                    else
                        return SUCCESS;
                }
                giTrace = 1030;

                // move the axis appropriately
                if( ROMoveToAbs(gulAllAxes,lPosBuf) == FAILURE )
                    return FAILURE;
            }
        }
        giTrace = 1040;

        if( ALHomeChuck() == FAILURE )
            return FAILURE;
        giTrace = 1050;

        if( FIOGetParamWaferVals(WAFER_SIZE,&lWaferSize) == FAILURE )
            return FAILURE;
        if( ALSetWaferSize((int)lWaferSize) == FAILURE )
            return FAILURE;
        giTrace = 1060;
        giWASubStep = 1;
        return SUCCESS;
    }

    // substep 2
    if( giWASubStep == 1 )
    {
//        if( TISetCounter(giWATimer2,10) == FAILURE )
//            return FAILURE;
        giTrace = 1110;
        giWASubStep = 2;
        return SUCCESS;
    }
    else if( giWASubStep == 2)
    {
//        if( TICountExpired(giWATimer2) )
//        {
            giTrace = 1210;
            giWASubStep = 0;            // timer expired, reset global sub-step
//        }
//        else
//            return SUCCESS;             // try (checking for the timeout) again
    }
    if( FIOGetParamWaferVals(CHUCK_VAC_USE,&lChuckVacUse) == FAILURE )
        return FAILURE;
    if( ~giDefineType & DFVACPRE && lChuckVacUse)      // if not vacuum prealigner
    {
//        if (giChuckVacSensorInputPos == AL_CHUCK_VAC_SEN_SAP4)  // prealigner on first Galil card
//            iWaferOnChuck = ~ROReadGalilPort(0) & giChuckVacSensorInputPos;
//        else
<<<<<<< HEAD
//            iWaferOnChuck = ~ROReadGalilPort(1) & giChuckVacSensorInputPos;
	ALChuckVacuum(1);
	ALPinVacuum(0);
        tv.tv_nsec = 50000000;
	nanosleep(&tv, NULL);
	iWaferOnChuck = ALWaferOnChuck();
=======
//            iWaferOnChuck = ~ROReadGalilPort(1) & giChuckVacSensorInputPos;
	ALChuckVacuum(1);
	ALPinVacuum(0);
        tv.tv_nsec = 50000000;
	nanosleep(&tv, NULL);
	iWaferOnChuck = ALWaferOnChuck();
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    	// if wafer is not on chuck
    	if( !iWaferOnChuck )
    	{
        	giAlignmentStatus |= AL_WAFER_NOT_ON_CHUCK;
        	return FAILURE;
    	}
    }

    giTrace = 1220;
    // Set the angular difference threshold for FAST Prealigner
    // Note this threshold is changed for fine-scan flat finding.
    if( giAlignerMode == AL_FASTPRE )
        glDistanceBetween2Meas = AL_FASTMEASURE_DIS;
    else
        glDistanceBetween2Meas = AL_REGULARMEASURE_DIS;

    giDataReadPosDir = TRUE;
    giRndDataIdx = 0;
    giStartDataIdx = 0;
    giDataEdgeIndex = AL_BEGIN_DATA;
    giMaxNumMeasStored = AL_OFFCENTER_END_INDEX;
    giOffsetIndexEnd = AL_OFFCENTER_END_INDEX;
<<<<<<< HEAD

    giWASubStep = 0;
    giBCORSubStep = 0;
    giBCORStepOff = 0;
    giBCORcount = 0;
    giBCORDone = 0;
=======

    giWASubStep = 0;
    giBCORSubStep = 0;
    giBCORStepOff = 0;
    giBCORcount = 0;
    giBCORDone = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    giFlatSubStepOff = 0;
    gulDuplicateCount = 0;              // used to count the number of times through the interrupt

	if(iTypeAlignmentArg == BALI)
	{
		glRobotTAxisOffset = 0;
		glRobotRAxisOffset = 0;
		glPreTAxisOffset = 0;
	}
    /*
     * reduce max angle by 5*AL_SHORT_MEAS_BY_COUNT, this allows enough room for
     * detecting the last CCD reading after the t-axis motion has stopped at 420
     * degrees
     */
    laScaledPos[giAxisIndx] = AL_CENTER_FIND_ROT/AL_CAL_SCALER;
    if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
        return FAILURE;
    glMaxAngleMeasured = laUnscaledPos[giAxisIndx] - AL_SHORT_MEAS_BY_COUNT;
    giCCDOutOfRange = FALSE;
    memset( giaDataStat, 0, (int)(AL_MAX_INDEX+1)*sizeof(int) );
    memset( guiaData, 0, (int)(AL_MAX_INDEX+1)*sizeof(int) );
    memset( glaAngle, 0, (int)(AL_MAX_INDEX+1)*sizeof(long) );

    giTrace = 1230;
    giWAStep++;                         // this step has been completed
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALStartMeasureProc
 * Abstract:    Start measurements needed for the wafer alignment proccess and rotate
 *              the chuck appropriately
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_start_mes()
 ******************************************************************************/
int ALStartMeasureProc(void)
{
    giTrace = 2010;

    if( ALRotateChuck420() == FAILURE )
        return FAILURE;
    giTrace = 2060;

    if( ALStartMeasurement() == FAILURE )
    {
        ALStopMeasurement();
        return FAILURE;
    }
    giTrace = 2070;
    giWAStep++;         // successful completion of this step, advance to the next
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCheckEdge
 * Abstract:    check data for the round, flat big, flat end, and stop measurement,
 *              for non-chipped wafers
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_edge_check()
 ******************************************************************************/
int ALCheckEdge(void)
{
    int iDataPos=giRndDataIdx;
    int iRetVal=SUCCESS;

<<<<<<< HEAD
    giTrace = 3010;
    if(giDataEdgeIndex+AL_SKIP_END >= iDataPos)
    {
//printf("EdgeIdx=%d iDataPos=%d giRndDataIdx=%d\n",giDataEdgeIndex, iDataPos, giRndDataIdx);
	while(giDataEdgeIndex+AL_SKIP_END >= iDataPos) iDataPos = giRndDataIdx;
    }
=======
    giTrace = 3010;
    if(giDataEdgeIndex+AL_SKIP_END >= iDataPos)
    {
//printf("EdgeIdx=%d iDataPos=%d giRndDataIdx=%d\n",giDataEdgeIndex, iDataPos, giRndDataIdx);
	while(giDataEdgeIndex+AL_SKIP_END >= iDataPos) iDataPos = giRndDataIdx;
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    while( (giDataEdgeIndex+AL_SKIP_END) < iDataPos )
    {
//        TIRefreshWD();
        if( ALFilterRound(giDataEdgeIndex++) == FAILURE )
            iRetVal = FAILURE;
        // mark collected data when there is a big change in data (possible chips)
        iDataPos = giRndDataIdx;
<<<<<<< HEAD
//    if(giDataEdgeIndex > giOffsetIndexEnd-5 || iRetVal == FAILURE)
//    {
//	long ulAmf;
//	ulAmf = ROGetAMFlag();
//	printf("amf=%d axis=%d EdgeIdx=%d OffIdx=%d ret=%d\n",ulAmf, gulAxis, giDataEdgeIndex, giOffsetIndexEnd,iRetVal);
//    }
    }

//    if(giRndDataIdx > 1200 && giRndDataIdx < 1210)
//    {
//	long ulAmf;
//	ulAmf = ROGetAMFlag();
//	printf("amf=%d axis=%d EdgeIdx=%d OffIdx=%d ret=%d\n",ulAmf, gulAxis, giDataEdgeIndex, giOffsetIndexEnd,iRetVal);
//    }


    // begining index of the data read >= last read index of the data read
=======
//    if(giDataEdgeIndex > giOffsetIndexEnd-5 || iRetVal == FAILURE)
//    {
//	long ulAmf;
//	ulAmf = ROGetAMFlag();
//	printf("amf=%d axis=%d EdgeIdx=%d OffIdx=%d ret=%d\n",ulAmf, gulAxis, giDataEdgeIndex, giOffsetIndexEnd,iRetVal);
//    }
    }

//    if(giRndDataIdx > 1200 && giRndDataIdx < 1210)
//    {
//	long ulAmf;
//	ulAmf = ROGetAMFlag();
//	printf("amf=%d axis=%d EdgeIdx=%d OffIdx=%d ret=%d\n",ulAmf, gulAxis, giDataEdgeIndex, giOffsetIndexEnd,iRetVal);
//    }


    // begining index of the data read >= last read index of the data read
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if(( giDataEdgeIndex >= giOffsetIndexEnd ) || (ROGetAMFlag()&gulAxis))
    {
        giTrace = 3020;
        ALStopMeasurement();            // stop measurement
        giWAStep++;                     // advance to the next alignment step
<<<<<<< HEAD
    }
=======
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    else
        giTrace = 3030;
    return iRetVal;
}
/*******************************************************************************
 * Function:    ALCenterCalc
 * Abstract:    perform calculations necessary to determine where the center of the
 *              wafer is
 *
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_center_caucul()
 ******************************************************************************/
int ALCenterCalc(int iAlignTypeArg)
{
<<<<<<< HEAD
    long laScaledPos[8]={0,0,0,0,0,50,0,0}, 
		 laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    long lOffsetVec;

//	giRndDataCount = giRndDataIdx;
//	RGplRegister[19] = giRndDataIdx;
=======
    long laScaledPos[8]={0,0,0,0,0,50,0,0}, 
		 laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    long lOffsetVec;

//	giRndDataCount = giRndDataIdx;
//	RGplRegister[19] = giRndDataIdx;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    giTrace = 5010;
    if( ALInitWaferCenterVar() == FAILURE )     // initialize centering variables
        return FAILURE;

    giTrace = 5020;
    if( ALComputeWaferCenter() == FAILURE )  // calculate the wafer center location
        return FAILURE;

    giTrace = 5030;
	if(iAlignTypeArg != BALI)
	{
    	if( ALCenterCircuit() == FAILURE )  // read the results from the calculations
        	return FAILURE;
	}
    giTrace = 5040;

    if(~giDefineType & DFSAP4)
	{
        if( ROUnscalePos((unsigned long)RO_AXIS_r,laScaledPos,laUnscaledPos) == FAILURE )
            return FAILURE;
        lOffsetVec = laUnscaledPos[1];
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        laScaledPos[1] = gstWaferCal.m_lOffsetVector;
        laUnscaledPos[1] = 0;
        if( ROUnscalePos((unsigned long)RO_AXIS_r,laScaledPos,laUnscaledPos) == FAILURE )
            return FAILURE;
        if( laUnscaledPos[1] < lOffsetVec )     // if offset vector within 50 microns
            giWAStep++;                         //      skip the next step (center
    }

<<<<<<< HEAD
	// if computed offset is greater than 100mm, something wrong.
	if(gstWaferCal.m_lOffsetVector > 100000)
	{
	    if( giDisplayMode & AL_DISPAR_A )
			printf("Offset Vector Out of Range: %d\n",gstWaferCal.m_lOffsetVector);		
		return FAILURE;
	}
=======
	// if computed offset is greater than 100mm, something wrong.
	if(gstWaferCal.m_lOffsetVector > 100000)
	{
	    if( giDisplayMode & AL_DISPAR_A )
			printf("Offset Vector Out of Range: %d\n",gstWaferCal.m_lOffsetVector);		
		return FAILURE;
	}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

	/* for intellipick only */
    giWAStep++;                     // advance to the next alignment step
    giAlignmentStatus &= ~AL_OFFSET_NOT_FOUND;  // turn off this bit
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCenterCorrectBCOR
 * Abstract:    Make correction for the center offset
 *
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_center_corect()
 ******************************************************************************/
int ALCenterCorrectBCOR(void)
<<<<<<< HEAD
{
    long rc;
    int iRetVal;
    long lWaferLiftUpPos;
    long lOffset, lMaxOffset, lMoveOffset;
    struct timespec tv;

=======
{
    long rc;
    int iRetVal;
    long lWaferLiftUpPos;
    long lOffset, lMaxOffset, lMoveOffset;
    struct timespec tv;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    tv.tv_sec = 0;

    // if ZZ1 Mode (only 420 deg rotation) is ON, skip this step.
    if (giDisplayMode & AL_DISPAR_Z)
    {
        giWAStep++;
        return SUCCESS;
    }
<<<<<<< HEAD
    giTrace = 6010;
=======
    giTrace = 6010;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    // alignment will not proceed if motion is not complete.
    if ( (ROGetAMFlag() & gulAllAxes) != gulAllAxes )
        return SUCCESS;
<<<<<<< HEAD

    switch (giBCORSubStep)
    { 
        giTrace += 10;
	case 0:
=======

    switch (giBCORSubStep)
    { 
        giTrace += 10;
	case 0:
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	    // move t to offset angle
	    iRetVal = ALOffsetAnglePosition();
	    if( iRetVal < 0 )
	    {
	        if( iRetVal == -1 )
	            giTrace = 6110;
	        else if( iRetVal == -2 )
	            giTrace = 6120;
	        else if( iRetVal == -3 )
	            giTrace =  6130;
	        else if( iRetVal == -4 )
	            giTrace =  6140;
	        else if( iRetVal == -5 )
	            giTrace =  6150;
	        giPrevNumCorr = giNumCorr;
	        giNumCorr++;
<<<<<<< HEAD
	        giWAStep = 1;                   // reset alignment step to begining
			return FAILURE;
	    }
	    giBCORSubStep = 1;
	    return SUCCESS;

	case 1: // USE PINS to move the wafer to the center
	    giTrace += 10;
	    switch (giBCORStepOff)
	    {
		case 0: // move r to 0 pos to begin the next offset correction
		    giTrace += 1;
		    glRPos2[5] = 0;
		    ROMoveToAbs(RO_AXIS_r, glRPos2);
		    giBCORStepOff++;
		    return SUCCESS;
		case 1: // turn vacuum and move z up
		    giTrace += 1;
		    if( FIOGetParamWaferVals(WAFER_LIFT_UP_POSITION,&lWaferLiftUpPos) != SUCCESS )
        		return FAILURE;

		    ALChuckVacuum(0);	// turn off chuck vacuum
		    ALPinVacuum(1);	// turn on pin vacuum
    		    tv.tv_nsec = 50000000;
		    nanosleep(&tv, NULL);

        	glZPos1[6] = lWaferLiftUpPos;
		    ROMoveToAbs(RO_AXIS_z, glZPos1);
		    giBCORStepOff++;
		    return SUCCESS;
	
		case 2:  // move r to offset or maxoffset
		    giTrace += 1;
		    giBCORcount++;
		    lMaxOffset = AL_MAX_OFFSET;
		    lOffset = gstWaferCal.m_lOffsetVector;
		    lMoveOffset = lOffset - lMaxOffset * giBCORcount;
		    if (lMoveOffset > 0L)
			lMoveOffset = lMaxOffset;
		    else
		    {
				lMoveOffset = lOffset;
				giBCORDone = 1;
		    }
//printf("lOffset:%d lMoveOffset: %d giBCORDone: %d\n",lOffset, lMoveOffset, giBCORDone);
		    if(giBCORcount > 5)
				giBCORDone = 1;
		    glRPos1[5] = lMoveOffset;
		    ROMoveToAbs(RO_AXIS_r, glRPos1);
		    giBCORStepOff++;
		    return SUCCESS;

		case 3:  // turn off pin vacuum and pin to chuck position (0 pos)
		    giTrace += 1;
		    ALPinVacuum(0); // turn off pin vacuum
		    ALChuckVacuum(1); // turn on chuck vacuum
    		tv.tv_nsec = 50000000;
		    nanosleep(&tv, NULL);
		    glZPos2[6] = 0;
		    ROMoveToAbs(RO_AXIS_z, glZPos2);
		    giBCORStepOff++;
		    return SUCCESS;

		case 4: // move r to 0 pos to begin the next offset correction
		    giTrace += 1;
		    glRPos2[5] = 0;
		    ROMoveToAbs(RO_AXIS_r, glRPos2);
		    giBCORStepOff = 0;
		    giBCORSubStep = 1; // begin the offset correction
		    if (giBCORDone)  
				giWAStep++;	// all correction done, go to next (finished BCOR)
		    return SUCCESS;
	    } // switch BCORStepOff
    } // switch BCORSubStep
=======
	        giWAStep = 1;                   // reset alignment step to begining
			return FAILURE;
	    }
	    giBCORSubStep = 1;
	    return SUCCESS;

	case 1: // USE PINS to move the wafer to the center
	    giTrace += 10;
	    switch (giBCORStepOff)
	    {
		case 0: // move r to 0 pos to begin the next offset correction
		    giTrace += 1;
		    glRPos2[5] = 0;
		    ROMoveToAbs(RO_AXIS_r, glRPos2);
		    giBCORStepOff++;
		    return SUCCESS;
		case 1: // turn vacuum and move z up
		    giTrace += 1;
		    if( FIOGetParamWaferVals(WAFER_LIFT_UP_POSITION,&lWaferLiftUpPos) != SUCCESS )
        		return FAILURE;

		    ALChuckVacuum(0);	// turn off chuck vacuum
		    ALPinVacuum(1);	// turn on pin vacuum
    		    tv.tv_nsec = 50000000;
		    nanosleep(&tv, NULL);

        	glZPos1[6] = lWaferLiftUpPos;
		    ROMoveToAbs(RO_AXIS_z, glZPos1);
		    giBCORStepOff++;
		    return SUCCESS;
	
		case 2:  // move r to offset or maxoffset
		    giTrace += 1;
		    giBCORcount++;
		    lMaxOffset = AL_MAX_OFFSET;
		    lOffset = gstWaferCal.m_lOffsetVector;
		    lMoveOffset = lOffset - lMaxOffset * giBCORcount;
		    if (lMoveOffset > 0L)
			lMoveOffset = lMaxOffset;
		    else
		    {
				lMoveOffset = lOffset;
				giBCORDone = 1;
		    }
//printf("lOffset:%d lMoveOffset: %d giBCORDone: %d\n",lOffset, lMoveOffset, giBCORDone);
		    if(giBCORcount > 5)
				giBCORDone = 1;
		    glRPos1[5] = lMoveOffset;
		    ROMoveToAbs(RO_AXIS_r, glRPos1);
		    giBCORStepOff++;
		    return SUCCESS;

		case 3:  // turn off pin vacuum and pin to chuck position (0 pos)
		    giTrace += 1;
		    ALPinVacuum(0); // turn off pin vacuum
		    ALChuckVacuum(1); // turn on chuck vacuum
    		tv.tv_nsec = 50000000;
		    nanosleep(&tv, NULL);
		    glZPos2[6] = 0;
		    ROMoveToAbs(RO_AXIS_z, glZPos2);
		    giBCORStepOff++;
		    return SUCCESS;

		case 4: // move r to 0 pos to begin the next offset correction
		    giTrace += 1;
		    glRPos2[5] = 0;
		    ROMoveToAbs(RO_AXIS_r, glRPos2);
		    giBCORStepOff = 0;
		    giBCORSubStep = 1; // begin the offset correction
		    if (giBCORDone)  
				giWAStep++;	// all correction done, go to next (finished BCOR)
		    return SUCCESS;
	    } // switch BCORStepOff
    } // switch BCORSubStep
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return FAILURE;
}
/*******************************************************************************
 * Function:    ALNotchFromCenter
 * Abstract:    find the center of the wafer
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_flat_from_cent()
 ******************************************************************************/
int ALNotchFromCenter(void)
{
    long lpos;

    giTrace = 7011;

    if( ALFindNotchStartIndex() == FAILURE ) // if can not find the beg index of the flat
    {
        // For single_axis prealigner, if fail to find flat/notch, then
        // up to macro to check for the status and retry.
        if(~giDefineType & DFSAP4)
        {
            giNumCorr++;            // increment number of corrections counter
            giTrace = 7021;         // set trace
            giWAStep = 1;           // reset alignment step to begining
            return SUCCESS;
        }
        else
            return FAILURE;
    }
    giTrace = 7031;

    if( ALCorrectNotchAngle(TRUE,1) == FAILURE )  // measure the begining of the flat
        return FAILURE;
    giTrace = 7041;

    if( ALCorrectNotchAngle(FALSE,2) == FAILURE )  // measure the end of the flat
        return FAILURE;
    giTrace = 7051;

    gstWaferCal.m_lFlatBegAngleFromFlat = gstWaferCal.m_lFlatBegAngleFromChuck;
    gstWaferCal.m_lFlatEndAngleFromFlat = gstWaferCal.m_lFlatEndAngleFromChuck;
    if(ALFindNotchMidAngle(&lpos)==FAILURE)
        return FAILURE;
    giWAStep++;                         // advance to the next alignment step
    giAlignmentStatus &= ~AL_FLAT_NOT_FOUND;  // clear bit
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALPositionNotch
 * Abstract:    Find the beginning of the main flat for non-chipped wafers.  This
 *              function will be called repeatedly until all cases have been executed
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_flat_position()
 ******************************************************************************/
int ALPositionNotch(void)
{
    static int giFlatBegining;          // if TRUE => flat begining, else flat end
    static int giPass=1;                // 1=first flat edge measurement
                                        // 2=second flat edge measurement
    static int giInternalTrace;
    //static long laWaferSpeed[4] = {0,0,0,0};
    //static long laWaferAccel[4] = {0,0,0,0};
    int iRetVal;

    // if ZZ1 Mode (only 420 deg rotation) is ON, skip this step.
    if (giDisplayMode & AL_DISPAR_Z)
    {
        giWAStep++;
        return SUCCESS;
    }

    switch( giWABalFlatSubStep )
    {
        case 0 :
            giTrace = 7010;
            if( ALFindNotchStartIndex() == FAILURE )  // find the start index of the flat
            {
                // For single_axis prealigner, if fail to find flat/notch, then
                // up to macro to check for the status and retry.
                if(~giDefineType & DFSAP4)
                {
                    giNumCorr++;            // increment number of corrections counter
                    giTrace = 7020;  // set trace
                    giWAStep = 1;           // reset alignment step to begining
                    return SUCCESS;
                }
                else
                    return FAILURE;
            }
            giTrace = 7030;

            giFlatBegining = FALSE;      // flat end indicator
            if( ALNotchMeasureDir(&giFlatBegining) == FAILURE )
                return FAILURE;
            giWABalFlatSubStep = 1;
            giPass = 1;                 // first flat edge measurement
            return SUCCESS;
            break;
<<<<<<< HEAD
        case 1 :
	    // before the move to notch angle, we need to have a motion complete
    	    if( (ROGetAMFlag()&gulAllAxes) != gulAllAxes )
	    {
//printf("finescan motion wait in case 1: giFlatBegining=%d giPass=%d\n", giFlatBegining, giPass);

		return SUCCESS;
	    }

=======
        case 1 :
	    // before the move to notch angle, we need to have a motion complete
    	    if( (ROGetAMFlag()&gulAllAxes) != gulAllAxes )
	    {
//printf("finescan motion wait in case 1: giFlatBegining=%d giPass=%d\n", giFlatBegining, giPass);

		return SUCCESS;
	    }

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            giInternalTrace = (giFlatBegining) ? 1 : 2;  // set internal trace
            giInternalTrace += 7100;
            giTrace = giInternalTrace + 10;  // set global trace

            if( ALCorrectNotchAngle(giFlatBegining,giPass) == FAILURE )
                return FAILURE;
            giTrace = giInternalTrace + 20;

            if( ALMoveToNotchMeasure(giFlatBegining,giPass) == FAILURE )
                return FAILURE;
            giTrace = giInternalTrace + 30;

            if( ALInitNotchMeasure(giFlatBegining) == FAILURE )
                return FAILURE;
            giWABalFlatSubStep = 2;
            return SUCCESS;
            break;
        case 2 :
            giInternalTrace += 100;
            if( !giWaitForFlatMeasuring )
                giTrace = giInternalTrace + 40;

            iRetVal = ALStartNotchMeasure(giFlatBegining);
            if( iRetVal )
            {
                if( iRetVal == 2 ) // begining of flat not found, repeat this substep
                    return SUCCESS;
                else
                    return FAILURE;     // start over
            }
            giTrace = giInternalTrace + 50;

            if( ALStartMeasurement() == FAILURE )  // unable to start measurement
            {
                ALStopMeasurement();
                giTrace = giInternalTrace + 60;
                return FAILURE;
            }
            giWABalFlatSubStep = 3;     // advance to next substep of this step
            giFindFlatSubStep = 0;      // clear find flat substep
            return SUCCESS;
            break;
        case 3 :
            giInternalTrace += 100;
            giTrace = giInternalTrace + 70;

            iRetVal = ALFindNotchPosition(giFlatBegining);
            if( iRetVal == FAILURE )    // unable to find the flat
            {
                // For single_axis prealigner, if fail to find flat/notch, then
                // up to macro to check for the status and retry.
                if(~giDefineType & DFSAP4)
                {
                    giNumCorr++;            // increment number of corrections counter
                    giTrace = giInternalTrace + 80;  // set trace
                    giWAStep = 1;           // reset alignment step to begining
                    return SUCCESS;
                }
                else
                    return FAILURE;
            }
<<<<<<< HEAD
            else if( iRetVal == 2)      // repeat this substep
{
//printf("case3: giInternalTrace=%d lastRead=%d giRndDataIdx=%d\n", giInternalTrace,giFlatIndexEndLastRead,giRndDataIdx);

                return SUCCESS;
}
//printf("switching starting point: giFlatBegining=%d iRetVal\n", giFlatBegining, iRetVal);
//printf("case3: giInternalTrace=%d lastRead=%d giRndDataIdx=%d\n", giInternalTrace,giFlatIndexEndLastRead,giRndDataIdx);
=======
            else if( iRetVal == 2)      // repeat this substep
{
//printf("case3: giInternalTrace=%d lastRead=%d giRndDataIdx=%d\n", giInternalTrace,giFlatIndexEndLastRead,giRndDataIdx);

                return SUCCESS;
}
//printf("switching starting point: giFlatBegining=%d iRetVal\n", giFlatBegining, iRetVal);
//printf("case3: giInternalTrace=%d lastRead=%d giRndDataIdx=%d\n", giInternalTrace,giFlatIndexEndLastRead,giRndDataIdx);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

            // switch from measurements starting point (beg or end)
            giFlatBegining = !giFlatBegining;
            giPass++;
            giTrace = giInternalTrace + 90;
            giFlatIndexEndPrev = giFlatIndexEndLastRead;;
            giWABalFlatSubStep = 1;     // assume 3 passes not completed
            if( giPass <= 2 )           // if have not completed 3 passes
                return SUCCESS;
            break;
    }

    giTrace = 7300;
    giAlignmentStatus &= ~AL_FLAT_NOT_FOUND;
    giWAStep++;                         // this step completed, advance to the next
    giWABalFlatSubStep = 0;               // set up for next time
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCorrectNotchPos
 * Abstract:    Make correction for flat, including the cutomers offset
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_flat_corect
 ******************************************************************************/
int ALCorrectNotchPos(void)
{
    int iRetVal;

    // if ZZ1 Mode (only 420 deg rotation) is ON, skip this step.
    if (giDisplayMode & AL_DISPAR_Z)
    {
        giWAStep++;
        return SUCCESS;
    }

    giTrace = 8010;

    iRetVal = ALMoveToNotchPosition();         // position the flat to it's final position
    if( iRetVal )
    {
        if( iRetVal == 2 )
            iRetVal = SUCCESS;             // repeat this step
    }
    else
        giWAStep++;

    return iRetVal;
}
/*******************************************************************************
 * Function:    ALEnd
 * Abstract:    Ending of the Alignment Process
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    bal_end()
 ******************************************************************************/
int ALEnd(void)
{
    unsigned int uiTimerTicks;
    char sBuf[100];

    giTrace = 9010;

    giNumCorr = 0;
    if( TIReadCounter(giWATimer1,&uiTimerTicks) == FAILURE )
        return FAILURE;

    if( giDisplayMode & AL_DISPAR_A )
    {
        sprintf( sBuf, "\r\nCounter: %u\r\n", 20000-uiTimerTicks );
        SERPutsTxBuff( SERGetCmdPort(), sBuf );
    }
    giTrace = 9040;

    giWAStep = 0;                       // alignment completed
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSQWaferAlignment
 * Abstract:    Perform alignment on a square wafer
 * Parameters:  ipStepNumArg
 *              iCCDArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    DoAll()
 ******************************************************************************/
int ALSQWaferAlignment(int iCCDArg)
{
    int iRetVal;

    guipDataX = &guiaData[AL_BEGIN_DATA];
    glpAngleX = &glaAngle[AL_BEGIN_DATA];
    gdCountIn100th = gdCountInDeg / 100.0;
    gdCountInRadians = gdCountInDeg * RAD_TO_DEG;

    if( iCCDArg == 384 )
    {
        giAlignmentStatus &= ~AL_OFFSET_NOT_FOUND;
        iRetVal = ALSQFindNotch();
        if( iRetVal == SUCCESS )
            giAlignmentStatus &= ~AL_FLAT_NOT_FOUND;
        giWAStep = 8;
        return iRetVal*360/20000;
    }

    ALSQListBreaks();
    if( iCCDArg == 0x800 )              // check calibration of CCD's
    {
        ALSQCalibrateCCD();
        giWAStep = 9;
        iRetVal = SUCCESS;
    }
    else
    {
        giAlignmentStatus &= ~AL_FLAT_NOT_FOUND;
        iRetVal = ALSQAlign();
        if( iRetVal == SUCCESS )
            giAlignmentStatus &= ~AL_OFFSET_NOT_FOUND;
        giWAStep++;
    }
    return iRetVal;
}
/*******************************************************************************
 *
 * function Name:   ALSetDefaultWaferValue
 *
 * purpose:			setup the default value of each wafer size before alignment
 *
 ******************************************************************************/
int ALSetDefaultWaferValue(int iWaferSizeArg, int iCCDArg,int iMSKCCDArg, double dPrimFlatArg, double dMainFlatArg,
							int iRndLevelArg, long lMaxOffsetCorArg, double dOffsetSizeCor, int iMinFlatLevelArg  )
{
	giCCDNumber = iCCDArg;
    gdMainFlatMinAngle = dPrimFlatArg;
    gdMainFlatMaxSize = dMainFlatArg;
    giRndLevel = iRndLevelArg;
    if( ALGetWaferRoundSeg(iWaferSizeArg,&gdRndSeg) != SUCCESS )
        return FAILURE;
    glMaxOffsetVector = lMaxOffsetCorArg;
    gdOffsetSizCor[0] = dOffsetSizeCor;
    giMinFlatSize = iMinFlatLevelArg;
    if( ALSelectCCDNumber(iMSKCCDArg) != SUCCESS )
        return FAILURE;
	return SUCCESS;
}
// specified wafer type and size.
int ALLoadWaferCal()
{
	long lWaferType, lWaferSize;
	int iCCDNumber;
    psCalTable psCalTbl;

    if( FIOGetParamWaferVals(WAFER_TYPE,&lWaferType) == FAILURE )
        return FAILURE;
    if( FIOGetParamWaferVals(WAFER_SIZE,&lWaferSize) == FAILURE )
        return FAILURE;
    if( (psCalTbl=FIOSearchCalibTableNode(lWaferSize,lWaferType)) )
    {
        giMaxNumCorr = (int)FIOGetItemFromCalTbl(CT_REP_NO);
		iCCDNumber = ALReturnCCDMask((int)FIOGetItemFromCalTbl(CT_CCD_NO));
    	if( ALSelectCCDNumber(iCCDNumber) != SUCCESS )
        	return FAILURE;
        glMaxOffsetVector = FIOGetItemFromCalTbl(CT_MAX_OFF);
        giCCDFirstPixelPos = (int)FIOGetItemFromCalTbl(CT_CCD_POS_COR);
        glCCDAnglePosition = FIOGetItemFromCalTbl(CT_CCD_AG_COR);
        gdOffsetSizCor[0] = (double)FIOGetItemFromCalTbl(CT_OFFSET_COR_0) / 1000.0f;
        gdOffsetSizCor[1] = (double)FIOGetItemFromCalTbl(CT_OFFSET_COR_1) / 1000.0f;
        gdOffsetSizCor[2] = (double)FIOGetItemFromCalTbl(CT_OFFSET_COR_2) / 1000.0f;
        gdOffsetSizCor[3] = (double)FIOGetItemFromCalTbl(CT_OFFSET_COR_3) / 1000.0f;
        gdOffsetSizCor[4] = (double)FIOGetItemFromCalTbl(CT_OFFSET_COR_4) / 1000.0f;
        gdOffsetSizCor[5] = (double)FIOGetItemFromCalTbl(CT_OFFSET_COR_5) / 1000.0f;
    }
    else
    {
        giAlignmentStatus |= AL_BAD_SIZE_OR_TYPE;
        return FAILURE;
    }
	return SUCCESS;
}
/*******************************************************************************
 * Function:    ALValidWaferSize
 * Abstract:    Decide if the wafer size is valid
 * Parameters:  lWaferSizeArg
 * Returns:     TRUE or FALSE
 ******************************************************************************/
int ALValidWaferSize(long lWaferSizeArg)
{
    switch((int)lWaferSizeArg)
    {
        case AL_WAFER_SIZE_3 :
        case AL_WAFER_SIZE_76 :
        case AL_WAFER_SIZE_80 :
        case AL_WAFER_SIZE_90 :
        case AL_WAFER_SIZE_4 :
        case AL_WAFER_SIZE_100 :
        case AL_WAFER_SIZE_5 :
        case AL_WAFER_SIZE_125 :
        case AL_WAFER_SIZE_6 :
        case AL_WAFER_SIZE_150 :
        case AL_WAFER_SIZE_8 :
        case AL_WAFER_SIZE_200 :
        case AL_WAFER_SIZE_12 :
        case AL_WAFER_SIZE_300 :
            return TRUE;
    }
    return FALSE;
}
