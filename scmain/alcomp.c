/******************************************************************************\
 *
 * Program:     Aligner Module Math Functions
 *
 * Functions:
 *
 * Description:  Functions and Declarations used by the alignment computations
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
\******************************************************************************/
#include <sys/io.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "sctim.h"
#include "sck.h"
#include "ser.h"
#include "romain.h"
#include "rofio.h"
#include "ro.h"
#include "roga.h"
#include "fio.h"
#include "fiol.h"
#include "alk.h"
#include "alcomp.h"
#include "alisr.h"
#include "alstep.h"
#include "alpre.h"
#include "scintr.h"
#include "scio.h"
#include "gaintr.h"
#include "gag.h"
#include "sctag.h"
#include "alfio.h"

long glNotchAngle;                       
long glNotchStartAngle;      
long glNotchBegPos;      
long glNotchEndPos;   
long glOffsetPos;     
long glOffsetCounts;     
long glZPos1[8], glZPos2[8], glRPos1[8], glRPos2[8]; 
long glMeasuredMoveDistance;   
long glScaleSpeed[8], glScaleAccel[8], glPosition[8]; 

//int giFlatIdx;  
int giPtrCenter1;   
int giPtrCenter2;  
int giPtrCenter3;  
int giNumSqData;  
int giaBreakListS[15]; 
                      
int giaBreakListE[15];
int giBreakPoints;    
int giNumItems;     

double gdSumOffsetCoordX;  
double gdSumOffsetCoordY; 
double gdSumRadius=0.0; 
double gdTraceCenter;    
double gdaAvgA[8]={0,0,0,0,0,0,0,0};
double gdaAvgB[8]={0,0,0,0,0,0,0,0}; 
double gdaSquareSideCalcAA[100];
double gdaSquareSideCalcBB[100];  
double gdSquareSideAA;
double gdSquareSideBB;
double gdaX[500]; 
double gdaY[500];    
double gdComputedAngle;

int ALInitNotchIndex(int a, int b, int c, int d, int e, int f);
int ALGetAligningSpeed( );

void ALChuckVacuum(int iFlagArg); 
void ALPinVacuum(int iFlagArg);
int ALWaferOnChuck(void);

/*******************************************************************************
 * Function:    ALInitNotchMeasure
 * Abstract:    Initialize Flat Measurement
 * Parameters:  iFlatMeasureArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    flat_meas_int
 ******************************************************************************/
int ALInitNotchMeasure(int iFlatMeasureArg)
{
    long laScaledPos[8]={0,0,0,0,0,0,0,0},    // buffers used for scaling and unscaling
         laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    long lMax, lRound;
    int iCStatus;

    if( TISetCounter(giWATimer2,300) == FAILURE )
        return FAILURE;
    iCStatus = TICountExpired(giWATimer2);
//    while( giMeasurementInProgress && !(iCStatus == TRUE) )
    while( giMeasurementInProgress )
    {
        iCStatus = TICountExpired(giWATimer2);
    }
    iCStatus = TICountExpired(giWATimer2);
    if( iCStatus == TRUE )
        return FAILURE;
    laScaledPos[giAxisIndx] = AL_FLAT_FIND_ROT / AL_CAL_SCALER;
    laUnscaledPos[giAxisIndx] = 0;
    if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
        return FAILURE;
    lMax = laUnscaledPos[giAxisIndx];

    laScaledPos[giAxisIndx] = AL_ROUND_BEFORE_FLAT / AL_CAL_SCALER;
    laUnscaledPos[giAxisIndx] = 0;
    if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
        return FAILURE;
    lRound = laUnscaledPos[giAxisIndx];

    if( iFlatMeasureArg )
    {
		ALInitNotchIndex(TRUE, AL_OFFCENTER_END_INDEX, AL_OFFCENTER_END_INDEX,
						 AL_OFFCENTER_END_INDEX, AL_FLAT_BEGINING_INDEX, AL_FLAT_BEGINING_INDEX);

        laScaledPos[giAxisIndx] = (gstWaferCal.m_lFlatBegAngleFromChuck-AL_BEG_FLAT_ADJ_PRO)
                                    /AL_CAL_SCALER;
        if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
            return FAILURE;
        glNotchStartAngle = laUnscaledPos[giAxisIndx];
        glMaxAngleMeasured = glNotchStartAngle + lMax - AL_SHORT_MEAS_BY_COUNT;
        glRoundBeforeFlat = glNotchStartAngle + lRound;
    }
    else
    {
		ALInitNotchIndex(FALSE, AL_FLAT_BEGINING_INDEX, AL_FLAT_BEGINING_INDEX,
						 AL_FLAT_BEGINING_INDEX, AL_MAX_INDEX, AL_MAX_INDEX);

        laScaledPos[giAxisIndx] = (gstWaferCal.m_lFlatEndAngleFromChuck+AL_BEG_FLAT_ADJ_PRO)
                                    /AL_CAL_SCALER;
        if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
            return FAILURE;
        glNotchStartAngle = laUnscaledPos[giAxisIndx];
        glMaxAngleMeasured = glNotchStartAngle - lMax + AL_SHORT_MEAS_BY_COUNT;
        glRoundBeforeFlat = glNotchStartAngle - lRound;
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALMoveToNotchMeasure
 * Abstract:    Move flat to 0 location
 * Parameters:  iFlatMeasArg
 *              iBegArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    flat_meas_move_to
 ******************************************************************************/
int ALMoveToNotchMeasure(int iFlatMeasArg, int iBegArg)
{
    long lMult;
    //long lWaferSize;
    //long lBuf[4];
    int iReturn;
    unsigned uTMWait20=0;

    // determine the speed modification for the necessary movement
    if( iFlatMeasArg )
    {
        glPosition[giAxisIndx]
            = (gstWaferCal.m_lFlatBegAngleFromChuck - (long)AL_BEG_FLAT_ADJ_PRO)/AL_CAL_SCALER;
        glNotchBegPos = glPosition[giAxisIndx];
        if( iBegArg == 1 )
        {
            if( ALGetSpeedChange(glOffsetPos,glNotchBegPos,&lMult)==FAILURE )
                return FAILURE;
        }
        else
        {
            if( ALGetSpeedChange(glNotchBegPos,glNotchEndPos,&lMult)==FAILURE )
                return FAILURE;
        }
    }
    else
    {
        glPosition[giAxisIndx]
            = (gstWaferCal.m_lFlatEndAngleFromChuck + (long)AL_BEG_FLAT_ADJ_PRO)/AL_CAL_SCALER;
        glNotchEndPos = glPosition[giAxisIndx];
        if( iBegArg != 1 )
        {
            if( ALGetSpeedChange(glNotchBegPos,glNotchEndPos,&lMult)==FAILURE )
                return FAILURE;
        }
        else
        {
            if( ALGetSpeedChange(glOffsetPos,glNotchEndPos,&lMult)==FAILURE )
                return FAILURE;
        }
    }

    glPosition[giAxisIndx] += glOffsetCounts;

    // get the operational speed, accel and decel from wafer parameter file
    if(ALGetAligningSpeed(AL_SP_OPERATIONAL, glScaleSpeed, glScaleAccel)==FAILURE)
        return FAILURE;

    // if second pass wait 20ms
    if( iBegArg == 2 )
        uTMWait20 = 2;

    if( (ROGetServoFlag() | ROGetERFlag()) & gulAllAxes )
    {
        giTrace += 5;
        return FAILURE;
    }

    // set speed, accel and decel to galil
    if((iReturn=ALSetAligningGASpeed(gulAxis, glScaleSpeed, glScaleAccel))==FAILURE)
        goto exit_point;
        // move the chuck to the absolute position
    if( (iReturn=ROMoveToAbs(gulAxis,glPosition)) == FAILURE )
        goto exit_point;

exit_point:
    if(iReturn)
    giTrace += 7;
    return iReturn;
}
/*******************************************************************************
 * Function:    WAFlatMeasureStart
 * Abstract:    Find the beginning of the flat
 * Parameters:  iFlatBeginningArg
 * Returns:     SUCCESS or FAILURE, or 2
 * Replaces:    flat_meas_start()
 ******************************************************************************/
int ALStartNotchMeasure(int iFlatBeginingArg)
{
    long laScaledPos[8]={0,0,0,0,0,0,0,0};      // buffers used for scaling and unscaling

    // this needs to return FAILURE, SUCCESS, or 2
    if( !giWaitForFlatMeasuring )
    {
        if( gulAllAxes & ~ROGetAMFlag() )
            return 2;
        else
            giWaitForFlatMeasuring = 1;
    }

    if( giWaitForFlatMeasuring < 2 )
    {
        if( ALSetFineScanSpeed() != SUCCESS )
            return FAILURE;
        else if( TISetCounter(giWATimer2,2) == FAILURE )
            return FAILURE;
    }
//    if( !TICountExpired(giWATimer2) )
//    {
//        giWaitForFlatMeasuring = 2;
//        return 2;
//    }

    // Set the angular difference threshold for fine-scan for FAST Prealigner
//    if( giAlignerMode == AL_FASTPRE )
        glDistanceBetween2Meas = AL_FINESCAN_DIS;

    laScaledPos[giAxisIndx] = AL_FLAT_FIND_ROT / AL_CAL_SCALER;
    if( !iFlatBeginingArg )
        laScaledPos[giAxisIndx] *= -1L;  // inverse

    if( ROMoveToRel(gulAxis, laScaledPos) != SUCCESS )
    {
        giTrace += 5;
        return FAILURE;
    }

    giWaitForFlatMeasuring = 0;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALNotchMeasureDir
 * Abstract:    Determine if the flat measurement starts at the beginning of the flat
 *              or the end of the flat.
 * Parameters:  iFlatBeginingArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    measure_order
 ******************************************************************************/
int ALNotchMeasureDir(int *ipFlatBeginingArg)
{
    long lRequestedFinalAnglePos;

    if( FIOGetParamWaferVals(FLAT_ANGLE,&lRequestedFinalAnglePos) != SUCCESS )
        return FAILURE;
    if( ALCorrectNormAngle(180000L,&lRequestedFinalAnglePos) == SUCCESS )
    {
        *ipFlatBeginingArg = FALSE;
        if( lRequestedFinalAnglePos<180000L && giCCDNumber==2 )
            *ipFlatBeginingArg = TRUE;
        if( (lRequestedFinalAnglePos<135000L || lRequestedFinalAnglePos>315000L)
                && (giCCDNumber==1 || giCCDNumber==2) )
            *ipFlatBeginingArg = TRUE;
        return SUCCESS;
    }
    return FAILURE;
}
/*******************************************************************************
 * Function:    ALComputeOffset
 * Abstract:    Load Action buffer for offset correction
 * Parameters:  lOffsetArg
 *              iRMovementArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    move_offset
 ******************************************************************************/
int ALComputeOffset(long lOffsetArg, int iRMovementArg)
{
    int iRet;
    long lWaferLiftUpPos;
    unsigned long ulAxis, ulAxisTemp;
    struct timespec tv;

    tv.tv_sec = 0;
    //unsigned uAxisMask;

    iRet = 1;
    ulAxis = RO_AXIS_t;
    while( iRet )
    {
    	ulAxisTemp = ROGetAMFlag() & ulAxis;
	if (ulAxisTemp == ulAxis) iRet = 0;
    }
    if( FIOGetParamWaferVals(WAFER_LIFT_UP_POSITION,&lWaferLiftUpPos) != SUCCESS )
        return FAILURE;
    if( ~giDefineType & DFSAP4 )         // single axis prealigner in an 8 axis system
    {
        giTrace += 1;
	
	ALChuckVacuum(0);	// turn off chuck vacuum
	ALPinVacuum(1);		// turn on pin vacuum
        tv.tv_nsec = 50000000;
	nanosleep(&tv, NULL);
        // turn off chuck vacuum and move z up
        glZPos1[6] = lWaferLiftUpPos;
	ulAxis = RO_AXIS_z;
	ROMoveToAbs(ulAxis, glZPos1);
	iRet = 1;
    	while( iRet )
    	{
	    ulAxisTemp = ROGetAMFlag() & ulAxis;
	    if (ulAxisTemp == ulAxis) iRet = 0;
    	}

        giTrace += 1;
        // move r to offset
        glRPos1[5] = lOffsetArg;
        ulAxis = RO_AXIS_r;
	ROMoveToAbs(ulAxis, glRPos1);
	iRet = 1;
    	while( iRet )
    	{
	    ulAxisTemp = ROGetAMFlag() & ulAxis;
	    if (ulAxisTemp == ulAxis) iRet = 0;
    	}


        giTrace += 1;
	ALPinVacuum(0);		// turn off pin vacuum
        tv.tv_nsec = 50000000;
	nanosleep(&tv, NULL);
        // move z down
        glZPos2[6] = 0;
	ulAxis = RO_AXIS_z;
	ROMoveToAbs(ulAxis, glZPos2);
	iRet = 1;
    	while( iRet )
    	{
	    ulAxisTemp = ROGetAMFlag() & ulAxis;
	    if (ulAxisTemp == ulAxis) iRet = 0;
    	}

        giTrace += 1;
        // move r to 0
        glRPos2[5] = 0;
	ulAxis = RO_AXIS_r;
	ROMoveToAbs(RO_AXIS_r, glZPos2);
	iRet = 1;
    	while( iRet )
    	{
	    ulAxisTemp = ROGetAMFlag() & ulAxis;
	    if (ulAxisTemp == ulAxis) iRet = 0;
    	}

        if( iRMovementArg )
            return SUCCESS; // CodeMerge, no need to append &end_pad action buffer.
    }

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCorrectOffset
 * Abstract:    correct offset by R-axis, load to action buffer
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    move_offset_vector
 ******************************************************************************/
int ALCorrectOffset(void)
{
    long lOffset, lMaxOffset;
    int iCnt = 0;

    if( giDefineType & DFSAP4 )         // single axis aligner - which this is not applicable for
        return FAILURE;

    lMaxOffset = AL_MAX_OFFSET;
    lOffset = gstWaferCal.m_lOffsetVector;

    while( (lOffset-lMaxOffset) > 0L )
    {
        lOffset -= lMaxOffset;
        if( ALComputeOffset(lMaxOffset,FALSE) == FAILURE )
            return FAILURE;
        ++iCnt;
        if (iCnt >= 5)  // Allow only 5 times at Max due to action buffer size
            break;
        giTrace += 6;
    }
    return ALComputeOffset(lOffset,TRUE);
}
/*******************************************************************************
 * Function:    ALHomeChuck
 * Abstract:    Reset the chuck counter and galil position
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    home_t
 ******************************************************************************/
int ALHomeChuck(void)
{
//    int iAllCCDCount = 0;
//    int iCCDCount = 0;
//    long lChuckX2;
//    long lCCDPos2;
//    long lChuckG2;
    long lBuf[8]={0,0,0,0,0,0,0,0};
    long lUnscaleBuf[8]={0,0,0,0,0,0,0,0};

    if( ALResetChuck() == FAILURE )
        return FAILURE;
    if( ROHomeAxis(gulAxis, FALSE) == FAILURE )
        return FAILURE;
    if( ROReadCurrentPosition(gulAxis,lBuf) == FAILURE )
        return FAILURE;
    if( ROUnscalePos(gulAxis,lBuf,lUnscaleBuf) == FAILURE )
        return FAILURE;
//    lChuckG2 = lUnscaleBuf[giAxisIndx];

//    glOffsetCounts = lChuckG2 - lChuckX2;
//    if( glOffsetCounts < -60000L )
//        glOffsetCounts += 65536L;
    glOffsetCounts = 0;
    return SUCCESS;
/*    while( iAllCCDCount < 3 )           // do the home_t 3 times, incase of failure
    {
        iCCDCount = 0;
        while( iCCDCount < 3 )
        {
            if( ALResetChuck() == FAILURE )
                return FAILURE;
            TIDelay(10);
            if( ALTakeMeasurement(&lChuckX2,&lCCDPos2) == FAILURE )
                return FAILURE;
            if( (lChuckX2>=65515L && lChuckX2<=65535L) ||
                    (lChuckX2<=20L && lChuckX2>=0L) ||
                    (lChuckX2>=-20L && lChuckX2<=-1L) )
                break;
            else
                iCCDCount++;
        }

        if( giDefineType & DFSAP4 )         // single axis prealigner in a 4 axis system
        {
            if( iCCDCount >= 3 )
                return FAILURE;
        }
        else
        {
            if( iCCDCount >= 3 )
            {
                iAllCCDCount++;
                continue;
            }
        }
        if( ROHomeAxis(gulAxis, FALSE) == FAILURE )
            return FAILURE;
        TIDelay(50);
        iCCDCount = 0;
        while( iCCDCount < 3 )
        {
            if( ALTakeMeasurement(&lChuckX2,&lCCDPos2) == FAILURE )
                return FAILURE;
            if( (lChuckX2>=65515L && lChuckX2<=65535L) ||
                    (lChuckX2<=20L && lChuckX2>=0L) ||
                    (lChuckX2>=-20L && lChuckX2<=-1L) )
                break;
            else
                iCCDCount++;
        }

        if( giDefineType & DFSAP4 )         // single axis prealigner in a 4 axis system
        {
            if( iCCDCount >= 3 )
                return FAILURE;
        }
        else
        {
            if( iCCDCount >= 3 )
            {
                iAllCCDCount++;
                continue;
            }
        }

        if( ROReadCurrentPosition(gulAxis,lBuf) == FAILURE )
            return FAILURE;
        if( ROUnscalePos(gulAxis,lBuf,lUnscaleBuf) == FAILURE )
            return FAILURE;
        lChuckG2 = lUnscaleBuf[giAxisIndx];

        glOffsetCounts = lChuckG2 - lChuckX2;
        if( glOffsetCounts < -60000L )
            glOffsetCounts += 65536L;
        return SUCCESS;
    }
    return FAILURE;
*/
}
/*******************************************************************************
 * Function:    ALFilterRound
 * Abstract:    Find the Round part of the wafer for a non-chipped wafer
 * Parameters:  iIndexArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    round
 ******************************************************************************/
int ALFilterRound(int iIndexArg)
{
    long lCurrAnglePos;                 // current angle position
    long lAngleSize;
    int  iSumDataDiff;                  // added differences from reading
    int  iLoop;
    int iStat = AL_DATA_STATUS_NOT_USED;
    int iRoundLevelInt = giRndLevel;

     if( iIndexArg<0 || iIndexArg>AL_MAX_INDEX )  // validate passed index
        return FAILURE;

    lAngleSize = (long)(gdRndSeg / 2.0);
    if( giaDataStat[iIndexArg] == AL_DATA_STATUS_READ_OK )
    {
        lCurrAnglePos = glaAngle[iIndexArg];

		iSumDataDiff = (int)(guiaData[iIndexArg] - guiaData[iIndexArg-1]);
        for( iLoop=iIndexArg-1;
            ((glaAngle[iLoop]-lCurrAnglePos) >= -lAngleSize) && iLoop>=AL_BEGIN_DATA;
            iLoop-- )
        {
			iSumDataDiff += ((int)(guiaData[iLoop] - guiaData[iLoop-1]));
        }
        for( iLoop=iIndexArg+1;
            ((glaAngle[iLoop]-lCurrAnglePos) <= lAngleSize) && iLoop<=giRndDataIdx;
                iLoop ++ )
        {
			iSumDataDiff += ((int)(guiaData[iLoop] - guiaData[iLoop-1]));
        }

        if( iSumDataDiff < -iRoundLevelInt )
            iStat = AL_DATA_STATUS_FLAT_BEG;
        else
        {
            if( iSumDataDiff > iRoundLevelInt )
                iStat = AL_DATA_STATUS_FLAT_END;
            else
                iStat = AL_DATA_STATUS_ROUND_EDGE;
        }
        giaDataStat[iIndexArg] = iStat;
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALFilterEdgeNotch
 * Abstract:    Find the flat/notch on the wafer
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    flat_filter
 ******************************************************************************/
int ALFilterEdgeNotch(void)
{
    long lFlatEdge;                     // angle of flat start or stop condition
    long lMainMaxSize;                  // maximum angle size of main flat
    long laScaledPos[8]={0,0,0,0,0,0,0,0},      // buffers used for scaling and unscaling
         laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    long lMaxAngMeas;                   // max angle measured
    long lFlatBegAngle;                 // angle of flat beginning position
    long lFlatEndAngle;                 // angle of flat ending position

    int iDataIndex;
    int iIndx, iIndx2;
    int iFlatBegFind=TRUE;              // flat beginning found
    int iFlatEndFind=TRUE;              // flat ending found
    int iFlatBegPos=FALSE;              // counter pos for 1st flat beg status
    int iFlatEndPos=FALSE;              // counter pos for 1st flat end stat
    int iFlatBegErr=FALSE;
    int iFlatEndErr=FALSE;
    int iMaxErrLevel=2;                 // max # measurements to cancel flat
    int iFlatStatus;                    // 1st or 2nd flat flag
    double dFlatAngle;                  // size of flat angle
    char sBuf[80];

    giWAStep++;         // assume succes and advancement to the next alignment step
    giTrace = 4010;

    lFlatEdge = (long)( 0.23 * gdCountInDeg );
    lMainMaxSize = (long)( gdMainFlatMaxSize * gdCountInDeg );

    laScaledPos[giAxisIndx] = AL_CENTER_FIND_ROT / AL_CAL_SCALER;
    if( ROUnscalePos( gulAxis, laScaledPos, laUnscaledPos ) == FAILURE )
        return FAILURE;
    lMaxAngMeas = laUnscaledPos[giAxisIndx];

    if( giDisplayMode & (AL_DISPAR_A|AL_DISPAR_B|AL_DISPAR_S) )
        SERPutsTxBuff( SERGetCmdPort(), "\r\n" );

    iDataIndex = giRndDataIdx;
    for(iIndx=AL_BEGIN_DATA; glaAngle[iIndx]<lMaxAngMeas && iIndx<iDataIndex; iIndx++)
    {
//        TIRefreshWD();
        if( giaDataStat[iIndx] == AL_DATA_STATUS_FLAT_BEG )
        {
            // the begining of the flat is found, mark it
            if( iFlatBegFind )
            {
                iFlatBegFind = FALSE;
                iFlatBegErr = FALSE;
                iFlatBegPos = iIndx;
                lFlatBegAngle = glaAngle[iIndx];
            }
            // now look for the end of the flat
            if( (glaAngle[iIndx]-lFlatBegAngle) > lFlatEdge )
            {
                for( iIndx2=iIndx;
                        (glaAngle[iIndx2]-lFlatBegAngle)<lMainMaxSize && iIndx2<iDataIndex;
                        iIndx2++ );
                // double check that the end of the flat was found, if not stub it
                if( iIndx2 > giRndDataIdx )
                    iIndx = giRndDataIdx;

                for( ; iIndx2>iFlatBegPos; iIndx2-- )
                {
                    if( giaDataStat[iIndx2] == AL_DATA_STATUS_FLAT_END )
                    {
                        // the end of the flat is found, mark it
                        if( iFlatEndFind )
                        {
                            iFlatEndFind = FALSE;
                            iFlatEndErr = FALSE;
                            iFlatEndPos = iIndx2;
                            lFlatEndAngle = glaAngle[iIndx2];
                        }
                        // refine the definition of the flat
                        if( (lFlatEndAngle-glaAngle[iIndx2]) > lFlatEdge )
                        {
                            iIndx = iFlatEndPos;
                            //dFlatAngle = (double)(lFlatEndAngle-lFlatBegAngle)
                            //                    / gdCountInDeg;
                            glNotchAngle = lFlatEndAngle - lFlatBegAngle;
                            dFlatAngle = (double) glNotchAngle / gdCountInDeg;
                            // marking the main flat
                            if( dFlatAngle > gdMainFlatMinAngle )
                            {
                                iFlatStatus = AL_DATA_STATUS_IN_MAIN_FLAT;
                                if( giDisplayMode & AL_DISPAR_A )
                                {
                                    sprintf( sBuf, "Main flat: %5.1lfø. \r\n",
                                            dFlatAngle );
                                    SERPutsTxBuff( SERGetCmdPort(), sBuf );
                                }
                            }
                            // marking the notch
                            else if( dFlatAngle < 7.0 )
                            {
                                iFlatStatus = AL_DATA_STATUS_IN_NOTCH;
                                if( giDisplayMode & AL_DISPAR_A )
                                {
                                    sprintf( sBuf, "Notch: %5.1lfø. \r\n",
                                            dFlatAngle );
                                    SERPutsTxBuff( SERGetCmdPort(), sBuf );
                                }
                            }
                            // marking the secondary flat
                            else
                            {
                                iFlatStatus = AL_DATA_STATUS_IN_2ND_FLAT;
                                if( giDisplayMode & AL_DISPAR_A )
                                {
                                    sprintf( sBuf, "Secondary flat: %5.1lfø.\r\n",
                                            dFlatAngle );
                                    SERPutsTxBuff( SERGetCmdPort(), sBuf );
                                }
                            }
                            for( iIndx2=iFlatBegPos, iIndx2++; iIndx2<iFlatEndPos;
                                    iIndx2++ )
                                giaDataStat[iIndx2] = iFlatStatus;  // mark flat

                            // start flat measurement
                            if( lFlatEndAngle >= (long)(360.0*gdCountInDeg) &&
                                    lFlatBegAngle <= (long)(360.0*gdCountInDeg) )
                            {
                                lFlatEndAngle %= (long)(360.0*gdCountInDeg);
                                lFlatEndAngle += (long) gdCountInDeg;
                                // find end of flat
                                for( iIndx2=AL_BEGIN_DATA;
                                    (glaAngle[iIndx2]-lFlatEndAngle)<=0L; iIndx2++ );
                                iFlatEndFind = TRUE;
                                for( ; iIndx2>=AL_BEGIN_DATA; iIndx2-- )
                                {
                                    if( giaDataStat[iIndx2] == AL_DATA_STATUS_FLAT_END )
                                    {
                                        // mark the end of the flat for refinement
                                        if( iFlatEndFind )
                                        {
                                            iFlatEndFind = FALSE;
                                            iFlatEndErr = FALSE;
                                            iFlatEndPos = iIndx2;
                                            lFlatEndAngle = glaAngle[iIndx2];
                                        }
                                        // refine the markings of the flat in the status buffer
                                        if( (glaAngle[iIndx]-lFlatBegAngle) > lFlatEdge )
                                            for( ; iIndx2>=AL_BEGIN_DATA; iIndx2-- )
                                                giaDataStat[iIndx2] = iFlatStatus;
                                    }
                                    else if( iFlatEndErr++ >iMaxErrLevel )
                                        iFlatEndFind = TRUE;
                                }
                            }
                            break;
                        }
                    }
                    else if( iFlatEndErr++ > iMaxErrLevel )
                        iFlatEndFind = TRUE;
                }
            }
        }
        else if( iFlatBegErr++ > iMaxErrLevel )
            iFlatBegFind = TRUE;
    }
    giTrace = 4020;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCalcCenterOffset
 * Abstract:    Calculate the center of the offset
 * Parameters:  ipIndex1Arg - Index to 1st point
 *              ipIndex2Arg - Index to 2nd point
 *              ipIndex3Arg - Index to 3rd point
 * Returns:     SUCCESS or FAILURE
 * Replaces:    circuit
 ******************************************************************************/
int ALCalcCenterOffset(int *ipIndex1Arg, int *ipIndex2Arg, int *ipIndex3Arg)
{
    double dX1, dX2, dX3, dY1, dY2, dY3;  // coord of points coresp to index 1,2, & 3
    double dVec1, dVec2, dVec3;         // Vectors coresp to the radius of the wafer
    double dDiffX1_3, dDiffX2_3;        // differences bet X points
    double dDiffY1_3, dDiffY2_3;        // differences bet Y points
    double dVecDiff3_1, dVecDiff3_2;    // diff bet the Vectors
    double dPtDist;
    double dAngle;
    double dResultX, dResultY, dRadius=0.0;  // results of calculations

    if( ALCalcCircleOffset(ipIndex1Arg,&dAngle,&dVec1) != SUCCESS )
        return FAILURE;
    dX1 = dVec1 * cos(dAngle);          // compute x and y coordinates
    dY1 = dVec1 * sin(dAngle);
    dVec1 *= dVec1;                     // square the vector

    if( ALCalcCircleOffset(ipIndex2Arg,&dAngle,&dVec2) != SUCCESS )
        return FAILURE;
    dX2 = dVec2 * cos(dAngle);          // compute x and y coordinates
    dY2 = dVec2 * sin(dAngle);
    dVec2 *= dVec2;                     // square the vector

    if( ALCalcCircleOffset(ipIndex3Arg,&dAngle,&dVec3) != SUCCESS )
        return FAILURE;
    dX3 = dVec3 * cos(dAngle);          // compute x and y coordinates
    dY3 = dVec3 * sin(dAngle);
    dVec3 *= dVec3;                     // square the vector

    dDiffX1_3 = dX1 - dX3;              // compute differences
    dDiffX2_3 = dX2 - dX3;
    dDiffY1_3 = dY1 - dY3;
    dDiffY2_3 = dY2 - dY3;

    dVecDiff3_1 = dVec3 - dVec1;
    dVecDiff3_2 = dVec3 - dVec2;

    dPtDist = dDiffX1_3*dDiffY2_3 - dDiffX2_3*dDiffY1_3;
    if( dPtDist != 0.0 )                // will not cause a divide by 0 error
    {
        dResultX = (dVecDiff3_1*dDiffY2_3 - dVecDiff3_2*dDiffY1_3)/dPtDist;
        if( dDiffY1_3 != 0.0 )
            dResultY = (dVecDiff3_1 - dResultX*dDiffX1_3)/dDiffY1_3;
        else if( dDiffY2_3 != 0.0 )
            dResultY = (dVecDiff3_2 - dResultX*dDiffX2_3)/dDiffY2_3;
        else
            return SUCCESS;

//        dRadius =  -dVec3 - dResultX*dX3 - dResultY*dY3;
        dRadius = -dVec3;
        dRadius -= (dResultX*dX3);
        dRadius -= (dResultY*dY3);
        dRadius = (dResultX*dResultX) + (dResultY*dResultY) - dRadius;
        if( dRadius > 0.0 )
            dRadius = sqrt(dRadius);
        else
            return SUCCESS;

        // increment the global accumulators for offset, radius, and number of items
        gdSumOffsetCoordX += dResultX;
        gdSumOffsetCoordY += dResultY;
        gdSumRadius += dRadius;
        giNumItems++;
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCalcCircleOffset
 * Abstract:    Calculate the offset of the circle/wafer for a non-chipped wafer
 * Parameters:  ipIndexArg
 *              dpAngleArg - angle read (in radians)
 *              dpRadiusArg - vector (in pixels) from chuck center to CCD break
 * Returns:     SUCCESS or FAILURE
 * Replaces:    medium_input
 ******************************************************************************/
int ALCalcCircleOffset(int *ipIndexArg, double *dpAngleArg, double *dpRadiusArg )
{
    int iIdx;
    double dAngleCorrConst;             // correction constant for the angle
    long lAngleFilterSize;              // angle size for filter
    long lAngleDiff;                    // change in the angle
    long lAngleSum=0;                  // sum of angle size
    double dRadiusSum=0.0;              // sum of radius

    if( *ipIndexArg<0 || *ipIndexArg>AL_MAX_INDEX )
        return FAILURE;

//    gdTraceCenter = (float)( *ipIndexArg );  // for tracing program - never used
    dAngleCorrConst = PIDEF / (double)gdCountInDeg / 180.0;
    lAngleFilterSize = (long)(2.0*gdCountInDeg);

    // gather information
    for( iIdx=*ipIndexArg; lAngleSum<lAngleFilterSize &&
               giaDataStat[iIdx]==AL_DATA_STATUS_ROUND_EDGE && iIdx<giOffsetIndexEnd;
         iIdx++ )
    {
//        TIRefreshWD();
        // compute difference between current and previous angle
        lAngleDiff = glaAngle[iIdx] - glaAngle[iIdx-1];
        // accumulate the sum of the radius
        dRadiusSum += (double)( guiaData[iIdx]*lAngleDiff );
        // accumulate the sum of the angle difference
        lAngleSum += lAngleDiff;
    }

    // check the sum of the angle and compute the radius
    if( lAngleSum )
        *dpRadiusArg = dRadiusSum / (double)(lAngleSum);
    else
        *dpRadiusArg = 0.0;

    // compute the angle
    *dpAngleArg = (double)( lAngleSum /2L + glaAngle[*ipIndexArg] )*dAngleCorrConst;

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALFindNotchStartIndex
 * Abstract:    find the begining of the flat index in the array of data
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    flat_index
 ******************************************************************************/
int ALFindNotchStartIndex(void)
{
    int iBegPt, iEndPt, iFlatFoundFlag=0;
    char sBuf[100];
    int iStartIndex, iEndIndex;

    // look through data to find the begining of the flag
    if( giDefineType & DFSAP4 )         // single axis prealigner in a 4 axis system
    {
        iStartIndex = AL_BEGIN_DATA;
        iEndIndex = giOffsetIndexEnd + 1;
    }
    else
    {
        iStartIndex = AL_BEGIN_DATA + 5;
        iEndIndex = giOffsetIndexEnd;
    }
    for( iBegPt=iStartIndex; iBegPt<iEndIndex; iBegPt++ )
    {
        if( giaDataStat[iBegPt+1]==AL_DATA_STATUS_IN_MAIN_FLAT &&
                giaDataStat[iBegPt]==AL_DATA_STATUS_FLAT_BEG )
        {
            iFlatFoundFlag = 1;
            break;
        }
    }

    if( iFlatFoundFlag==0 )
    {
        for( iBegPt=iStartIndex; iBegPt<iEndIndex; iBegPt++ )
        {
            if( giaDataStat[iBegPt+1]==AL_DATA_STATUS_IN_NOTCH &&
                    giaDataStat[iBegPt]==AL_DATA_STATUS_FLAT_BEG )
            {
                iFlatFoundFlag = 2;
                break;
            }
        }
    }

    // the begining of the flat was not found
    if( iFlatFoundFlag == 0 )
        return FAILURE;

    gstWaferCal.m_iFlatBegIndexFromChuck = iBegPt;

    if( giDisplayMode & AL_DISPAR_B )
    {
        sprintf( sBuf, " \r\n Flat big index is %d ... angle_0 is %ld", iBegPt,
                            (long)((float)glaAngle[iBegPt]*100.0/gdCountInDeg) );
        SERPutsTxBuff( SERGetCmdPort(), sBuf );
    }
    if( iFlatFoundFlag==1 )
    {
        for( iEndPt=iBegPt; iEndPt<iEndIndex; iEndPt++ )
        {
            if( giaDataStat[iEndPt-1]==AL_DATA_STATUS_IN_MAIN_FLAT &&
                    giaDataStat[iEndPt]==AL_DATA_STATUS_FLAT_END )
                break;
        }
        if( iEndPt > giOffsetIndexEnd )  // point not found
        {
            for( iEndPt=AL_BEGIN_DATA; iEndPt<iBegPt; iEndPt++ )
            {
                if( giaDataStat[iEndPt-1]==AL_DATA_STATUS_IN_MAIN_FLAT &&
                        giaDataStat[iEndPt]==AL_DATA_STATUS_FLAT_END )
                    break;
            }
        }
        if( iEndPt == iBegPt )
            return FAILURE;
    }
    else
    {
        for( iEndPt=iBegPt; iEndPt<giOffsetIndexEnd; iEndPt++ )
        {
            if( giaDataStat[iEndPt-1]==AL_DATA_STATUS_IN_NOTCH &&
                    giaDataStat[iEndPt]==AL_DATA_STATUS_FLAT_END )
                break;
        }
        if( iEndPt == giOffsetIndexEnd )
        {
            for( iEndPt=AL_BEGIN_DATA; iEndPt<iBegPt; iEndPt++ )
            {
                if( giaDataStat[iEndPt-1]==AL_DATA_STATUS_IN_NOTCH &&
                        giaDataStat[iEndPt]==AL_DATA_STATUS_FLAT_END )
                    break;
            }
        }
        if( iEndPt == iBegPt )
            return FAILURE;
    }
    gstWaferCal.m_iFlatEndIndexFromChuck = iEndPt;

    if( giDisplayMode & AL_DISPAR_B )
    {
        sprintf( sBuf, " \r\n Flat end index is %d ... angle_0 is %ld", iEndPt,
                            (long)((float)glaAngle[iEndPt]*100.0/gdCountInDeg) );
        SERPutsTxBuff( SERGetCmdPort(), sBuf );
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCorrectNotchAngle
 * Abstract:    Correct the flat angle for the beginning and/or end of flat for a
 *              non-chipped wafer
 * Parameters:  iFlatBeginArg
 *              iPassNumArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    flat_angle
 ******************************************************************************/
int ALCorrectNotchAngle(int iFlatBeginArg, int iPassNumArg)
{
    double dOffsetCoordX, dOffsetCoordY;  // x & y offset coordinates
    double dFlatCoordX, dFlatCoordY;    // x & y flat coordinates
    double dX, dY;                      // points x and y
    int iFlatIdx;                       // flat index
    long lFlatAngle;                    // angle of flat edge
    long lChuckAngle;                   // angle of chuck
    char sBuf[80];

    iFlatIdx = iFlatBeginArg ? gstWaferCal.m_iFlatBegIndexFromChuck
                                 : gstWaferCal.m_iFlatEndIndexFromChuck;

    dOffsetCoordX = dOffsetCoordY = (double)(guiaData[iFlatIdx]);
    dOffsetCoordX *= cos((double)glaAngle[iFlatIdx]/gdCountInDeg
                                *DEG_TO_RAD);
    dOffsetCoordY *= sin((double)glaAngle[iFlatIdx]/gdCountInDeg
                                *DEG_TO_RAD);

    dFlatCoordX = (double)(gstWaferCal.m_lOffsetVector) / 1000.0;
    dFlatCoordX /= AL_PIXEL_SPACING;
    dFlatCoordY = dFlatCoordX;
    dFlatCoordX *= cos( (double)gstWaferCal.m_lOffsetAngle * DEG_TO_RAD / 1000.0);
    dFlatCoordY *= sin( (double)gstWaferCal.m_lOffsetAngle * DEG_TO_RAD / 1000.0);

    dX = dOffsetCoordX - dFlatCoordX;
    dY = dOffsetCoordY - dFlatCoordY;

    if( dY != 0.0 )                     // check for divide by 0 error
    {
        lFlatAngle = (long)(atan2(dY,dX) / DEG_TO_RAD * 1000.0);
        if( lFlatAngle < 0L )
            lFlatAngle += 360000L;      // make movement positive only
    }
    else
        lFlatAngle = ( dX>0.0 ? 180000L : 0L );
    lFlatAngle += 360000L;

    if( giDisplayMode & AL_DISPAR_B )
    {
        sprintf( sBuf, "\r\nFlat Position Correction 1 - angle %ld \r\n",
                    lFlatAngle/AL_CAL_SCALER );
        SERPutsTxBuff( SERGetCmdPort(), sBuf );
    }

	if(giDefineType & DFSAP4)
    	lChuckAngle = gstWaferCal.m_lOffsetAngle;
	else
    	lChuckAngle = gstWaferCal.m_lOffsetAngle + glCCDAnglePosition*AL_CAL_SCALER;

    if( iPassNumArg != 1 )
    {
        if( iFlatBeginArg )
            lChuckAngle = gstWaferCal.m_lFlatEndAngleFromChuck;
        else
            lChuckAngle = gstWaferCal.m_lFlatBegAngleFromChuck;
    }

    if( ALFindShortestMove(lChuckAngle,&lFlatAngle) != SUCCESS )
        return FAILURE;

    if( iFlatBeginArg )
        gstWaferCal.m_lFlatBegAngleFromChuck = lFlatAngle;
    else
        gstWaferCal.m_lFlatEndAngleFromChuck = lFlatAngle;

    if( giDisplayMode & AL_DISPAR_B )
    {
        sprintf( sBuf, "   -----> %ld \r\n", lFlatAngle / AL_CAL_SCALER );
        SERPutsTxBuff( SERGetCmdPort(), sBuf );
    }

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALFindNotchPosition
 * Abstract:    Find the flat position from the flat measurement
 * Parameters:  iFlatBeginArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    fine_f_flat
 ******************************************************************************/
int ALFindNotchPosition(int iFlatBeginArg)
{
    static long glFlatIndex;            // ind_f
    static long glFlatIdxReadBeg;       // ind_f_rb
    static long glNumSum;               // no_in_sum
    static long glRoundSum;             // round_sum
    int iFlatIndexEnd;                  // fl_i_end
    int iNumRndPnts=0;                  // ssno_rd
    int iNumFlatPnts=0;                 // no_fb
    long lRoundSize=0;                  // round_size
    long lMeasSize;                     // meas_size
    long lFlatIndex;                    // ind_f_re
    long laScaledPos[8]={0,0,0,0,0,0,0,0},    // buffers used for scaling and unscaling
         laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    char sBuf[100];
    //int iTemp;

    if( giFindFlatSubStep == 0 )             // begining of finding the flat process
    {
        glFlatIndex = glFlatIdxReadBeg = giBegDataPos+1;
        glNumSum = 0;
        glRoundSum = 0;
        giFindFlatSubStep = 1;
    }

    if( giFindFlatSubStep == 1 )            // will only get here if failed once
    {
        if( giFlatIndexEndLastRead >= giRndDataIdx )
            return 2;
        else
            giFindFlatSubStep = 2;
    }

    do
    {
        if( glFlatIndex < giRndDataIdx )
        {
            glRoundSum += guiaData[glFlatIndex];
            glNumSum++;
            if( (glaAngle[glFlatIndex]>=glRoundBeforeFlat && iFlatBeginArg) ||
                (glaAngle[glFlatIndex]<=glRoundBeforeFlat && !iFlatBeginArg) )
                break;
            glFlatIndex++;
        }
        iFlatIndexEnd = giFlatIndexEndLastRead;
//        TIRefreshWD();
    } while( glFlatIndex < iFlatIndexEnd );

    giFindFlatSubStep = 0;              // reset subset number

    if( glFlatIndex >= giFlatIndexEndLastRead )
        return FAILURE;

    glRoundSum *= 1000L;
    if (glNumSum == 0L)
        return FAILURE;
    lRoundSize = glRoundSum / glNumSum;

    // create distance between round sample and checked point
    lFlatIndex = glFlatIndex;
    do
    {
        if( (glFlatIndex-lFlatIndex) <= (long)AL_FLAT_ROUND_DISTANCE )
            break;
        glFlatIndex++;
        iFlatIndexEnd = giFlatIndexEndLastRead;
//        TIRefreshWD();
    } while( glFlatIndex < iFlatIndexEnd );

    do{
//        TIRefreshWD();
        if( glFlatIndex < giRndDataIdx )
        {
            lMeasSize = (long)( guiaData[glFlatIndex]+guiaData[glFlatIndex-1]
                                +guiaData[glFlatIndex-2] )*1000 / 3;
            if( (lRoundSize-lMeasSize) > giMinFlatSize )
            {
                iNumRndPnts = 0;
                if( ++iNumFlatPnts >= AL_PRIMARY_FLAT_POINTS )
                {
                    if( iFlatBeginArg )
                    {
                        // calculate begining index from flat and begining angle from flat
                        gstWaferCal.m_iFlatBegIndexFromFlat
                                         = (int)glFlatIndex - iNumFlatPnts;
                        laUnscaledPos[giAxisIndx] = glaAngle[(int)glFlatIndex-iNumFlatPnts];
                        laScaledPos[giAxisIndx] = 0;
                        if( ROScalePos( gulAxis, laUnscaledPos, laScaledPos ) == FAILURE )
                            return FAILURE;
                        gstWaferCal.m_lFlatBegAngleFromFlat = laScaledPos[giAxisIndx] * AL_CAL_SCALER;
                        // display begining index and begining angle from flat
                        if( giDisplayMode & AL_DISPAR_B )
                        {
                            sprintf( sBuf,
                                "\r\n flat begining position corect_2 - index %5d",
                                gstWaferCal.m_iFlatBegIndexFromFlat );
                            SERPutsTxBuff( SERGetCmdPort(), sBuf );
                            sprintf( sBuf, ".... angle %7ld \r\n ",
                               gstWaferCal.m_lFlatBegAngleFromFlat / AL_CAL_SCALER );
                            SERPutsTxBuff( SERGetCmdPort(), sBuf );
                        }
                    }
                    else
                    {
                        // calculate ending index from flat
                        gstWaferCal.m_iFlatEndIndexFromFlat
                                        = (int) glFlatIndex - iNumFlatPnts;
                        // calculate ending angle from flat
                        laUnscaledPos[giAxisIndx] = glaAngle[(int)glFlatIndex-iNumFlatPnts];
                        laScaledPos[giAxisIndx] = 0;
                        if( ROScalePos( gulAxis, laUnscaledPos, laScaledPos ) == FAILURE )
                            return FAILURE;
                        gstWaferCal.m_lFlatEndAngleFromFlat = laScaledPos[giAxisIndx] * AL_CAL_SCALER;
                        // display ending index and ending angle from flat
                        if( giDisplayMode & AL_DISPAR_B )
                        {
                            sprintf( sBuf,
                                "\r\n flat end position corect_2 - index %5d",
                                gstWaferCal.m_iFlatEndIndexFromFlat );
                            SERPutsTxBuff( SERGetCmdPort(), sBuf );
                            sprintf( sBuf, ".... angle %7ld \r\n ",
                               gstWaferCal.m_lFlatEndAngleFromFlat / AL_CAL_SCALER );
                            SERPutsTxBuff( SERGetCmdPort(), sBuf );
                        }
                    }
// ****************************************************************************************
// CK: 071200 - This portion of checking is deleted from the routine.
//      It used to check for distance(room) between the end of round and the beginning of flat.
//      This checking ensures that the room of 2*AL_FLAT_ROUND_DISTANCE exists.
//      This checking will miss the flat if the flat data starts a little ahead of the expected angle.
//      As soon as we have enough Flat points, we assume we succeeded in finding the flat.
// ****************************************************************************************
//                  iTemp = giBegDataPos + (int)glNumSum;
//                  iTemp += (2*AL_FLAT_ROUND_DISTANCE);
//                  if(iTemp >= ((int)glFlatIndex - iNumFlatPnts))
//                  {
//                        if( giDisplayMode & AL_DISPAR_B )
//                        {
//                            sprintf( sBuf, "In Flat: Beg=%d, Sum=%d, Tot=%d, Findx=%d, Pnts=%d\r\n",
//                                giBegDataPos, (int)glNumSum, iTemp, (int)glFlatIndex, iNumFlatPnts );
//                            SERPutsTxBuff( SERGetCmdPort(), sBuf );
//                        }
//                        return FAILURE;
//                  }
// ****************************************************************************************
                    return SUCCESS;
                }
            }
            else if( iNumRndPnts++ > AL_PRIMARY_ROUND_POINTS )
                iNumFlatPnts = 0;
            glFlatIndex++;
            glRoundSum += ( (long)guiaData[lFlatIndex++]-(long)guiaData[glFlatIdxReadBeg++] )
                                * 1000L;
            lRoundSize = glRoundSum / glNumSum;
        }
        iFlatIndexEnd = giFlatIndexEndLastRead;
    } while( glFlatIndex < iFlatIndexEnd );

    if( glFlatIndex >= giFlatIndexEndLastRead )
        return FAILURE;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALRotateChuck420
 * Abstract:    rotate the chuck with speed for the centering measurement
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    rotate_chuck_cent
 ******************************************************************************/
int ALRotateChuck420(void)
{
    //long lWaferSize, lWaferSpeed, lWaferAccel;
    long laPosition[8]={0,0,0,0,0,0,0,0};
    long laPositionUnscale[8]={0,0,0,0,0,0,0,0};
    long laWaferSpeed[8]={0,0,0,0,0,0,0,0};
    long laWaferAccel[8]={0,0,0,0,0,0,0,0};
    //int iSpeedSetError=FALSE;

    if (GAGetGalilErrorFlag())
        return FAILURE;

    while( gulAllAxes & ~ROGetAMFlag() )
    {
        if( gulAllAxes & (ROGetServoFlag() | ROGetERFlag()) )
        {
            giTrace = 2020;
            return FAILURE;
        }
    }

    giTrace = 2030;
    // Get operational speed, accel and decel from wafer parameter file
	if(ALGetAligningSpeed(AL_SP_OPERATIONAL, laWaferSpeed, laWaferAccel)==FAILURE)
        return FAILURE;
//printf("gulAllAxes: %d smsc: %d smac: %d\n", gulAllAxes, laWaferSpeed[4], laWaferAccel[4]);

    // get and set the centering speed, acceleration, and deceleration
	if(ALGetAligningSpeed(AL_SP_MEASURE_CENTER, laWaferSpeed, laWaferAccel)==FAILURE)
		return FAILURE;
//printf("gulAllAxes: %d smsc: %d smac: %d\n", gulAllAxes, laWaferSpeed[4], laWaferAccel[4]);

    giTrace = 2040;
    // Set operational speed, accel and decel to galil
	if(ALSetAligningGASpeed(gulAllAxes, laWaferSpeed, laWaferAccel)==FAILURE)
		return FAILURE;
//printf("gulAllAxes: %d smsc: %d smac: %d\n", gulAllAxes, laWaferSpeed[4], laWaferAccel[4]);
    giTrace = 2050;
    // rotate chuck to the absolute position
    laPosition[giAxisIndx] = AL_CENTER_FIND_ROT / AL_CAL_SCALER;
	glMeasuredMoveDistance = laPosition[giAxisIndx];
    if( ROUnscalePos(gulAxis,laPosition,laPositionUnscale) == FAILURE )
        return FAILURE;
    laPositionUnscale[giAxisIndx] += glOffsetCounts;
    if( ROScalePos(gulAxis, laPositionUnscale, laPosition) == FAILURE )
		return FAILURE;
    if( ROMoveToAbs(gulAllAxes,laPosition) != SUCCESS )
        return FAILURE;
    if( GAGetGalilErrorFlag() )
        return FAILURE;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALFindShortestMove
 * Abstract:    Find the minimum move between two angles
 * Note:        Angles are in 1/1000 of a degree converted to longs (aligner units).
 * Parameters:  lFixAngleArg
 *              lpMoveAngleArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    angle_min_ch
 ******************************************************************************/
int ALFindShortestMove(long lFixAngleArg, long *lpMoveAngleArg)
{
    long lNumRotations;                 // number of 360 degree rotations
    long lDiffAng;                      // the angular difference between lFixAngleArg
                                        //       and lpMoveAngleArg

    if( lFixAngleArg  < 0L )            // the fixed angle must be positive
        return FAILURE;

    while( *lpMoveAngleArg < 0L )        // put the move angle between 0-360 degrees
        *lpMoveAngleArg += 360000L;

    lNumRotations = lFixAngleArg % 360000L;  // determine # rotations
    lDiffAng = lFixAngleArg / 360000L;
    *lpMoveAngleArg %= 360000L;

    // determine the minimum move
    if( labs(lNumRotations - *lpMoveAngleArg) > 180000L )
    {
        if( labs(lNumRotations - *lpMoveAngleArg - 360000L ) < 180000L )
            *lpMoveAngleArg += 360000L;
        else
            *lpMoveAngleArg -= 360000L;
    }
    *lpMoveAngleArg += 360000L * lDiffAng;  // compute the actual necessary move

    if( *lpMoveAngleArg < 2*AL_BEG_FLAT_ADJ_PRO )
        *lpMoveAngleArg += 360000L;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCorrectNormAngle
 * Abstract:    Normalize the passed angle betweeen -180 and 180 degrees
 * Note:        Angles were in 1/100 of a degree converted to longs (robot units).
 *              now in 1/1000 of a degree
 * Parameters:  lOldPosArg
 *              lpNewPosArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    cor_pos_move
 ******************************************************************************/
int ALCorrectNormAngle(long lOldPosArg, long *lpNewPosArg)
{
    while( (lOldPosArg - *lpNewPosArg) > 180000L )  // if angle bet -360 & -180 deg
        *lpNewPosArg += 360000L;
    while( (lOldPosArg - *lpNewPosArg) < -180000L )  // if angle between 180 & 360
        *lpNewPosArg -= 360000L;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALComputeWaferCenter
 * Abstract:    Calculate the location of the wafer center for Non-Chipped Wafers
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    center_cal()
 ******************************************************************************/
int ALComputeWaferCenter(void)
{
    // initialize necessary global variables
    gdSumOffsetCoordX = (double) 0.0;
    gdSumOffsetCoordY = (double) 0.0;
    gdSumRadius = (double) 0.0;
    giNumItems = 0;

    while(  (giPtrCenter3<giOffsetIndexEnd) && (giNumItems<AL_MAX_CIRCLE_CALCS) )
    {
//        TIRefreshWD();
        if( ALCalcCenterOffset(&giPtrCenter1,&giPtrCenter2,&giPtrCenter3) == FAILURE )
            return FAILURE;

        while( giaDataStat[giPtrCenter1+=AL_POINT_INC]!=AL_DATA_STATUS_ROUND_EDGE &&
                giPtrCenter1<=giOffsetIndexEnd );

        while( giaDataStat[giPtrCenter2+=AL_POINT_INC]!=AL_DATA_STATUS_ROUND_EDGE &&
                giPtrCenter2<=giOffsetIndexEnd );

        while( giaDataStat[giPtrCenter3+=AL_POINT_INC]!=AL_DATA_STATUS_ROUND_EDGE &&
                giPtrCenter3<=giOffsetIndexEnd );
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALInitWaferCenterVar
 * Abstract:    Initialize global variables used to determine the center of the
 *              wafer, during the wafer alignment process.  For Non-chipped wafers.
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    center_int
 ******************************************************************************/
int ALInitWaferCenterVar(void)
{
    giPtrCenter1 = AL_BEGIN_DATA;
    while( giaDataStat[++giPtrCenter1]!=AL_DATA_STATUS_ROUND_EDGE &&
            giPtrCenter1<giOffsetIndexEnd );

    giPtrCenter2 = giPtrCenter1 + AL_POINT_STEP;
    while( giaDataStat[++giPtrCenter2]!=AL_DATA_STATUS_ROUND_EDGE &&
            giPtrCenter2<giOffsetIndexEnd );

    giPtrCenter3 = giPtrCenter2 + AL_POINT_STEP;
    while( giaDataStat[++giPtrCenter3]!=AL_DATA_STATUS_ROUND_EDGE &&
            giPtrCenter3<giOffsetIndexEnd );

//printf("giPtrCenter1=%d giOffsetIndexEnd=%d giPtrCenter2=%d giPtrCenter3=%d\n",giPtrCenter1, giOffsetIndexEnd, giPtrCenter2, giPtrCenter3);

    if( (giPtrCenter1>=giOffsetIndexEnd) || (giPtrCenter2>=giOffsetIndexEnd)
            || (giPtrCenter3>=giOffsetIndexEnd) )
        return FAILURE;
    else
        return SUCCESS;
}
/*******************************************************************************
 * Function:    ALCenterCircuit
 * Abstract:    Read results from circle calculations for a non-chipped wafer
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    center_res() and circuit_res()
 ******************************************************************************/
int ALCenterCircuit(void)
{
    double dOffsetX, dOffsetY;          // wafer offsets for x and y coordinates
    double dOffsetSize;                 // wafer offset size
    long lPos;                          // current position of the chuck
    int iIndex;
    long laScaledPos[8]={0,0,0,0,0,0,0,0},    // buffers used for scaling and unscaling
         laUnscaledPos[8]={0,0,0,0,0,0,0,0};
    char sBuf[100];

    if( giNumItems == 0 )
        return FAILURE;

    dOffsetX = gdSumOffsetCoordX / (double)( giNumItems*2 );
    dOffsetY = gdSumOffsetCoordY / (double)( giNumItems*2 );
    dOffsetSize = sqrt( (dOffsetX*dOffsetX) + (dOffsetY*dOffsetY) )
                    * (double)AL_PIXEL_SPACING;
    iIndex = (int)( dOffsetSize+0.5f );
    iIndex = ( iIndex>5 ? 5 : iIndex);

    gstWaferCal.m_lOffsetVector = (long)(dOffsetSize * gdOffsetSizCor[iIndex]
                                             * (double)AL_XY_PIXEL_SCALER);

    if( dOffsetY == 0.0 )
    {
        if( dOffsetX < 0.0 )
            gstWaferCal.m_lOffsetAngle = 180000L;
        else
            gstWaferCal.m_lOffsetAngle = 0L;
    }
    else
        gstWaferCal.m_lOffsetAngle
            = (long)( atan2(dOffsetY,dOffsetX) * RAD_TO_THOUSANDDEG );
    gstWaferCal.m_lOffsetAngle += 180000L;  // increment angle by 180 degrees

    // get unscaled value of the center rotation
    laScaledPos[giAxisIndx] = AL_CENTER_FIND_ROT / AL_CAL_SCALER;
    if( ROUnscalePos(gulAxis,laScaledPos,laUnscaledPos) == FAILURE )
        return FAILURE;
    lPos = laUnscaledPos[giAxisIndx];

    if( (lPos-gstWaferCal.m_lOffsetAngle)
            > (gstWaferCal.m_lOffsetAngle+360000L-lPos) )
        gstWaferCal.m_lOffsetAngle += 360000L;

    gstWaferCal.m_lWaferSize = (long)( gdSumRadius / (double)giNumItems * AL_PIXEL_SPACING
                                                                            * 2000.0 + 0.05 );

    if( giDisplayMode & AL_DISPAR_A )
    {
        sprintf( sBuf, "offset = %ldæm  angle = %ld   wafer size = %ldæm  \r\n",
                gstWaferCal.m_lOffsetVector, gstWaferCal.m_lOffsetAngle/AL_CAL_SCALER,
                gstWaferCal.m_lWaferSize );
        SERPutsTxBuff( SERGetCmdPort(), sBuf );
    }

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALGetSpeedChange
 * Abstract:    return decremented multiplier for speed modification based on size
 *              of move
 * Parameters:  lOldPos
 *              lNewPos
 *              lpMult
 * Returns:     SUCCESS or FAILURE
 * Replaces:    sp_ac_dec()
 ******************************************************************************/
int ALGetSpeedChange(long lOldPosArg, long lNewPosArg, long *lpMultArg)
{
    long lAbsDiff;

    // determine the absolute difference between the old and new positions
    lAbsDiff = labs(lNewPosArg - lOldPosArg);

    if( lAbsDiff == 0L )
        lAbsDiff = 1L;                  // there must be some difference

    lAbsDiff = 18000L / lAbsDiff;
    // check limits of the differences
    if( lAbsDiff < 1L )                 // the difference was less than 180 degrees
        lAbsDiff = 1L;
    else if( lAbsDiff > 10L )
        lAbsDiff = 10L;
    *lpMultArg = lAbsDiff;
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALOffsetAnglePosition
 * Abstract:    position the chuck to correct for the center offset, and begin the
 *              corrections
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    pos_offset_angle()
 ******************************************************************************/
int ALOffsetAnglePosition(void)
{
    long lSpeedModBase;
    long laUnscaledPos[8] = {0,0,0,0,0,0,0,0};
    long laWaferSpeed[8] = {0,0,0,0,0,0,0,0};
    long laWaferAccel[8] = {0,0,0,0,0,0,0,0};
    long laAbsPos[8] = {0,0,0,0,0,0,0,0};
    int iRetVal;
    unsigned long ulAxes;
    struct timespec tv;

    tv.tv_sec = 0;

    laAbsPos[giAxisIndx] = gstWaferCal.m_lOffsetAngle/AL_CAL_SCALER + glCCDAnglePosition;

    iRetVal = 1;
    while (iRetVal == 1)
    {
	ulAxes = ROGetAMFlag() & gulAllAxes;
	if (ulAxes == gulAllAxes)
		iRetVal = 0;
    }

    glOffsetPos = laAbsPos[giAxisIndx];

    iRetVal = ALGetSpeedChange(glMeasuredMoveDistance, glOffsetPos, &lSpeedModBase);
    if (iRetVal != SUCCESS )
        return -3;

    // get operational speed, accel, and decel from wafer parameter
    iRetVal = ALGetAligningSpeed(AL_SP_OPERATIONAL, laWaferSpeed, laWaferAccel);
    if (iRetVal == FAILURE)
        return -3;

    // modify the speed and acceleration
    laWaferSpeed[giAxisIndx] /= lSpeedModBase;
    laWaferAccel[giAxisIndx] /= lSpeedModBase;

    // Set operational speed, accel, and decel to galil
    iRetVal = ALSetAligningGASpeed(gulAxis, laWaferSpeed, laWaferAccel);
    if (iRetVal == FAILURE)
        return -4;

    // position the change
    laUnscaledPos[giAxisIndx] = 0;
    if( ROUnscalePos(gulAxis,laAbsPos,laUnscaledPos) == FAILURE )
        return -3;

    // with the added offset
    laUnscaledPos[giAxisIndx] += glOffsetCounts;
    laAbsPos[giAxisIndx] = 0;
    if( ROScalePos( gulAxis,laUnscaledPos,laAbsPos ) == FAILURE )
        return -3;

    ALChuckVacuum(1);
    tv.tv_nsec = 50000000;
    nanosleep(&tv, NULL);

    // and move the axis
    iRetVal = ROMoveToAbs(gulAxis,laAbsPos);
    if( (iRetVal == FAILURE) || GAGetGalilErrorFlag() )
    {
        TKListActiveTasks();
        return -5;
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALMoveToNotchPosition
 * Abstract:    Move the wafer to it's final resting position
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    position_flat()
 ******************************************************************************/
int ALMoveToNotchPosition(void)
{
    static long lPosition;
    long lFlatAngle; // lWaferSize;
    static long laWaferSpeed[8] = {0,0,0,0,0,0,0,0};
	static long laWaferAccel[8] = {0,0,0,0,0,0,0,0};
    long lZPosition;
    long laMovement[8] = {0,0,0,0,0,0,0,0};
    char sBuf[100];

	sBuf[0] = 0;
    if( !giWaitForFlatPositioning )
    {
//	ALChuckVacuum(1);
//	usleep(300000);
//
        if(ALFindNotchMidAngle(&lPosition)==FAILURE)
            return FAILURE;
        if( FIOGetParamWaferVals(FLAT_ANGLE,&lFlatAngle) != SUCCESS )
            return FAILURE;
        lPosition = ((lPosition/AL_CAL_SCALER)+lFlatAngle)*AL_CAL_SCALER;   // 0 pos + cust final resting
        if( ALCorrectNormAngle(glNotchEndPos*10L,&lPosition) != SUCCESS )
            return FAILURE;
        lPosition /= AL_CAL_SCALER;
        giWaitForFlatPositioning = 1;
//printf("1. giWaitForFlatPositioning=%d\n",giWaitForFlatPositioning);
    }

    if( giWaitForFlatPositioning == 1 )  // check if axis still running
    {
        if( ~ROGetAMFlag() & gulAllAxes )
            return 2;
    }

//printf("2. giWaitForFlatPositioning=%d\n",giWaitForFlatPositioning);
    giTrace = 8020;
    if( gulAllAxes & (ROGetServoFlag() | ROGetERFlag()) )
        return FAILURE;

//printf("3. giWaitForFlatPositioning=%d\n",giWaitForFlatPositioning);
    giTrace = 8030;
    if( giWaitForFlatPositioning < 2 )
    {
        // Get operational speed, accel and decel from wafer parameter
        if(ALGetAligningSpeed(AL_SP_OPERATIONAL, laWaferSpeed, laWaferAccel)==FAILURE)
            return FAILURE;

        // set operational speed, accel and decel to galil
        if(ALSetAligningGASpeed(gulAxis, laWaferSpeed, laWaferAccel)==FAILURE)
	{
printf("trace=%d axis=%d spd=%d acl=%d\n",giTrace,gulAxis,laWaferSpeed[4],laWaferAccel[4]);
            return 2;
	}

        // get pin loading position and move there
        if( FIOGetParamWaferVals(WAFER_LOAD_POSITION,&lZPosition) != SUCCESS )
            return FAILURE;
        if( lZPosition > 0L )           // must be negative
            lZPosition = 0L;

        giTrace = 8040;
        if( giDefineType & DFSAP4 )         // single axis prealigner in a 4 axis system
        {
            // move chuck to correct position
            giWaitForFlatPositioning = 0;
            laMovement[giAxisIndx] = lPosition;
            if( ROMoveToAbs(gulAxis,laMovement) != SUCCESS )
                return FAILURE;
        }
        else                            // move pins to loading/chuck position
        {
//	    ALChuckVacuum(0);
//	    usleep(800000);
            laMovement[giAxisIndx] = lPosition;
            laMovement[2] = lZPosition;
            if( ROMoveToAbs(gulAxis|RO_AXIS_z,laMovement) != SUCCESS )
                return FAILURE;
        }
    }
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSQCalcFittest
 * Abstract:
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    BestFit()
 ******************************************************************************/
void ALSQCalcFittest(double *dpSideXArg, double *dpSideYArg, int iLenArg)
{
    int iIdx;
    double dSumX=0.0;                   // sum of elements on the x side
    double dSumY=0.0;                   // sum of elements on the y side
    double dSumXY=0.0;                  // sum of elements x-side * y-side
    double dSumXX=0.0;                  // sum of elements x-side²
    double dSumYY=0.0;                  // sum of elements y-side²

    if( fabs(dpSideXArg[iLenArg-1]-dpSideXArg[0])
            <= fabs(dpSideYArg[iLenArg-1]-dpSideYArg[0]) )
    {
        giTrace = 5051;
        for( iIdx=0; iIdx<iLenArg; iIdx++ )
        {
            dSumX += dpSideYArg[iIdx];
            dSumY -= dpSideXArg[iIdx];

            dSumXY += (-dpSideXArg[iIdx]) * dpSideYArg[iIdx];
            dSumYY += pow( dpSideYArg[iIdx], (double)2.0 );
        }
        if (dSumX == (double)0.0)
            dSumX = (double)1.0; // divide by 0 check
        gdSquareSideAA = (dSumXY - dSumYY*dSumY/dSumX )
                            / (dSumX - iLenArg*dSumYY/dSumX );
        gdSquareSideBB = ( dSumY - gdSquareSideAA*iLenArg )/ dSumX;
        if( gdSquareSideBB == (double)0.0 )
            gdSquareSideBB = 0.00000001;  // to elliminate the divide by 0
        gdSquareSideAA = -gdSquareSideAA / gdSquareSideBB;
        gdSquareSideBB = -1.0/gdSquareSideBB;
    }
    else
    {
        giTrace = 5052;
        for( iIdx=0; iIdx<iLenArg; iIdx++ )
        {
            dSumX += dpSideXArg[iIdx];
            dSumY += dpSideYArg[iIdx];
            dSumXY += dpSideXArg[iIdx]*dpSideYArg[iIdx];
            dSumXX += pow( dpSideXArg[iIdx], (double)2.0 );
        }
        if (dSumX == (double)0.0)
            dSumX = (double)1.0; // divide by 0 check
        gdSquareSideAA = (dSumXY - dSumXX*dSumY/dSumX)
                             / (dSumX - iLenArg*dSumXX/dSumX);
        gdSquareSideBB = (dSumY - gdSquareSideAA*iLenArg) / dSumX;
    }
    return;
}
/*******************************************************************************
 * Function:    ALSQCheckPeak
 * Abstract:
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    CheckPeak()
 ******************************************************************************/
int  ALSQCheckPeak(int iIndexArg)
{
    int iIdx;  // iDataIdx;
    double dSum1=0.0, dSum2=0.0;

    giTrace = 5053;

    // gather and sum the data from the ten points preceeding iIndexArg
    for( iIdx=iIndexArg-10; iIdx<=iIndexArg; iIdx++ )
    {
        dSum1 += (double)((double)guipDataX[(iIdx+1)%(giNumSqData-1)]
                            - (double)guipDataX[iIdx%(giNumSqData-1)]);
    }
    // gather and sum the data from the point iIndexArg and the 10 points following it
    for( iIdx=iIndexArg+1; iIdx<=iIndexArg+11; iIdx++ )
    {
//        dSum2 += (double)((double)guipDataX[(iIdx+1)%(giNumSqData-1)]
//                            - (double)guipDataX[iIdx%(giNumSqData-1)]);
        dSum2 += (double)((double)(guipDataX[(iIdx+1)%(giNumSqData-1)])
                        -(double)(guipDataX[(iIdx)%(giNumSqData-1)]))
                        /((double)(glpAngleX[(iIdx+1)%(giNumSqData-1)])
                        -(double)(glpAngleX[(iIdx)%(giNumSqData-1)]))/gdCountIn100th;
    }

    if( dSum1>=0.0 && dSum2<=0.0 )
        return SUCCESS;
    else
        return FAILURE;
}
/*******************************************************************************
 * Function:    ALSQListBreaks
 * Abstract:
 * Parameters:
 * Returns:
 * Replaces:    CreateBreakList()
 ******************************************************************************/
void ALSQListBreaks(void)
{
    int iIdx, iIdx2;
    int iFoundFlat=FALSE;
    unsigned uBreak, uMin;
    double dDataPt1, dDataPt2;
    int iFirst, iSecond;

    uBreak = (unsigned)(AL_CCD_MAX_READING+giCCDFirstPixelPos-5);
    uMin = (unsigned)(giCCDFirstPixelPos+5);
//    uBreak = (unsigned)(AL_CCD_MAX_READING+giCCDFirstPixelPos-20);
//    uMin = (unsigned)(giCCDFirstPixelPos+20);
    giNumSqData = 10000;                // max number of points that could have been collected
    giBreakPoints = 0;
    giTrace = 5054;

//    memset( giaBreakListS, 0, 15*sizeof(int) );
//    memset( giaBreakListE, 0, 15*sizeof(int) );

    for( iIdx=0; iIdx<=giNumSqData+2; iIdx++ )
    {
        // Max break points
        if(giBreakPoints >= 15)
            break;
        // 20000 is 4 possible cycles from the 2 pulses * 5000 encoder counts
        // this is used to determine if data started mid flat / mid corner data
        if( !iFoundFlat && (glpAngleX[iIdx]>=(glpAngleX[0]+20000)) )
        {
            giNumSqData = iIdx;
            iFoundFlat = TRUE;
        }
        iFirst = (iIdx+1)%(giNumSqData-1);
        iSecond = (iIdx)%(giNumSqData-1);
        // if inside corner
        if( guipDataX[iFirst]<uBreak && guipDataX[iSecond]<uBreak &&
            guipDataX[iFirst]>uMin && guipDataX[iSecond]>uMin )
        {
            // first good data
            giaBreakListS[giBreakPoints] = iIdx;
            for( iIdx2=iIdx; iIdx2<giNumSqData+2; iIdx2++ )
            {
                if( !iFoundFlat && (glpAngleX[iIdx2]>=(glpAngleX[0]+20000)) )
                {
                    giNumSqData=iIdx2;
                    iFoundFlat = TRUE;
                }

                // first bad data
                if( guipDataX[(iIdx2+1)%(giNumSqData-1)]>=uBreak ||
                    guipDataX[(iIdx2)  %(giNumSqData-1)]>=uBreak ||
                    guipDataX[(iIdx+1) %(giNumSqData-1)]<=uMin ||
                    guipDataX[(iIdx)   %(giNumSqData-1)]<=uMin ||
                    iIdx2>=giNumSqData )
                {
                    giaBreakListE[giBreakPoints] = iIdx2;
                    iIdx = iIdx2 + 1;
                    giBreakPoints++;
                    break;
                }
                dDataPt1 = (double)( (double)guipDataX[(iIdx2+1)%(giNumSqData-1)]
                                   - (double)guipDataX[(iIdx2) %(giNumSqData-1)]);
                dDataPt2 = (double)( (double)guipDataX[(iIdx2+2)%(giNumSqData-1)]
                                   - (double)guipDataX[(iIdx2+1)%(giNumSqData-1)]);
                if( dDataPt1>=0.0 && dDataPt2<0.0 )
                {
                    if( ALSQCheckPeak(iIdx2) == SUCCESS )
                    {
                        giaBreakListE[giBreakPoints] = iIdx2 - 5;
                        giBreakPoints++;
                        iIdx = iIdx2 + 5;
                        break;
                    }
                }
            }
        }
    }
    // remove garbage from the break list
    for( iIdx=0; iIdx<giBreakPoints; iIdx++ )
    {
        if( giaBreakListE[iIdx] <= giaBreakListS[iIdx] )
        {
//            memmove( &giaBreakListS[iIdx], &giaBreakListS[iIdx+1],
//                        (sizeof(int)*(giBreakPoints-iIdx-1)) );
//            memmove( &giaBreakListE[iIdx], &giaBreakListE[iIdx+1],
//                        (sizeof(int)*(giBreakPoints-iIdx-1)) );
            for( iIdx2=iIdx+1; iIdx2<giBreakPoints; iIdx2++)
            {
                giaBreakListS[iIdx2-1] = giaBreakListS[iIdx2];
                giaBreakListE[iIdx2-1] = giaBreakListE[iIdx2];
            }
            giBreakPoints--;
        }
    }
    // check for continuation - if readings began in the middle of a corner => shift data
    if( giBreakPoints > 0 )
    {
        if( giaBreakListS[0]==0 && giaBreakListE[giBreakPoints-1]==giNumSqData )
        {
            giaBreakListS[0] = giaBreakListS[giBreakPoints-1];
            giBreakPoints--;
        }
    }
    // modify data if readings began in the middle of a corner
    if( giaBreakListS[0] > giaBreakListE[0] )
        giaBreakListE[0] += giNumSqData-1;
}
/*******************************************************************************
 * Function:    ALSQAlign
 * Abstract:    Determine / compute the information necessary to align a square wafer
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    Align()
 ******************************************************************************/
int ALSQAlign(void)
{
    double dAngle; //dAngleS, dAngleE, dAngle;
    double dAbsSideBB, dAvgA, dAvgB, dCalcBAdj;
    double dCenterX, dCenterY;
    double dOffset;
    double dFlatAngle=0;
    double dX1, dY1;
    int iIdx, iIdx2, iIdx3, iIdx4, iStat, iCnt;
    int iSwitch1, iSwitch2, iSwitch1Old=0;
    long lCenterScaleOffset;
    //long laScaledPos[4]={0,0,0,0},  // buffers used for scaling and unscaling
    //   laUnscaledPos[4]={0,0,0,0};

    giTrace = 5055;
    // if wafer is too much off from the center, do not align.
    if(giBreakPoints != 4)
        return FAILURE;

//    memset( gdaAvgA, 0, 4*sizeof(double) );
//    memset( gdaAvgB, 0, 4*sizeof(double) );
    glCCDOffset = (long)giCCDFirstPixelPos;

    // compute x and y averages using polar coordinates
    iIdx = giaBreakListS[0]%(giNumSqData-1);
    iIdx2 = giaBreakListE[0]%(giNumSqData-1);
    gdaX[0] = (double)(guipDataX[iIdx]);
    gdaX[0] *= cos( (double)glpAngleX[iIdx] / gdCountInRadians );

    gdaX[1] = (double)(guipDataX[iIdx2]);
    gdaX[1] *= cos( (double)glpAngleX[iIdx2] / gdCountInRadians );

    gdaY[0] = (double)(guipDataX[iIdx]);
    gdaY[0] *= sin( (double)glpAngleX[iIdx] / gdCountInRadians );

    gdaY[1] = (double)(guipDataX[iIdx2]);
    gdaY[1] *= sin( (double)glpAngleX[iIdx2] / gdCountInRadians );

    gdComputedAngle = 0.0;
    if( (gdaX[1]-gdaX[0]) == 0.0 )
        gdComputedAngle = 3.0;
    else
        gdComputedAngle
            = fabs(atan((gdaY[1]-gdaY[0])/(gdaX[1]-gdaX[0]))*RAD_TO_DEG);
    gdComputedAngle -= (double)((int)(gdComputedAngle/90.0));
    gdComputedAngle -= 90.0 + 3.0;
    gdComputedAngle *= DEG_TO_RAD;

    for( iIdx=0; iIdx<giBreakPoints; iIdx++ )
    {
        iIdx3 = 0;
        for( iIdx2=giaBreakListS[iIdx]; iIdx2<=giaBreakListE[iIdx];
                iIdx2++ )
        {
            iIdx4 = iIdx2%(giNumSqData-1);
            gdaX[iIdx3] = (double)(guipDataX[iIdx4]);
            gdaX[iIdx3] *= cos( (double)glpAngleX[iIdx4]/gdCountInRadians + gdComputedAngle );
            gdaY[iIdx3] = (double)(guipDataX[iIdx4]);
            gdaY[iIdx3] *= sin( (double)glpAngleX[iIdx4]/gdCountInRadians + gdComputedAngle );
            iIdx3++;
        }
//        TIRefreshWD();
        ALSQCalcFittest(gdaX, gdaY, iIdx3);
        gdaSquareSideCalcAA[iIdx] = gdSquareSideAA;
        gdaSquareSideCalcBB[iIdx] = gdSquareSideBB;
    }

    giTrace = 5056;
    iIdx3 = 0;
    iStat = 0;
    for( iIdx=0; iIdx<giBreakPoints; iIdx++)
    {
        iSwitch1 = ( gdaSquareSideCalcBB[iIdx]>0) ? 1 : -1;
//        iSwitch1Old = iSwitch1;
        dAbsSideBB = fabs( gdaSquareSideCalcBB[iIdx] );
        dAvgA = gdaSquareSideCalcAA[iIdx];
        dAvgB = gdaSquareSideCalcBB[iIdx];
        iCnt = 1;

        for( iIdx2=0; iIdx2<4; iIdx2++ )
        {
            iIdx4 = iIdx + iIdx2 + 1;
            dCalcBAdj = dAbsSideBB
                         / fabs( gdaSquareSideCalcBB[iIdx4%giBreakPoints]);
            iSwitch2 = (gdaSquareSideCalcBB[iIdx4%giBreakPoints]>0) ? 1 : -1;
            if(iSwitch1==iSwitch2 && iStat!=2 && dCalcBAdj<1000.0 && dCalcBAdj>0.001)
            {
                dAvgA += gdaSquareSideCalcAA[iIdx4%giBreakPoints];
                dAvgB += gdaSquareSideCalcBB[iIdx4%giBreakPoints];
                iCnt++;
                iStat = 1;
            }
            else if( dAbsSideBB>500 &&
                     fabs(gdaSquareSideCalcBB[iIdx4%giBreakPoints])>500 &&
                     iStat!= 1 )
            {
                dAvgA = fabs(dAvgA)
                         + fabs(gdaSquareSideCalcAA[iIdx4%giBreakPoints]);
                dAvgB = -iSwitch1Old * fabs(dAvgB)
                         + fabs(gdaSquareSideCalcBB[iIdx4%giBreakPoints]);
                iCnt++;
                iStat = 2;
            }
            else
            {
                iIdx += iIdx2;
                iStat = 0;
                break;
            }
        }
        gdaAvgA[iIdx3%4] = dAvgA / iCnt;
        gdaAvgB[iIdx3%4] = dAvgB / iCnt;
        iSwitch1Old = iSwitch1;
        iIdx3++;
    }

    // check if alignment failed
    for( iIdx=0; iIdx<4; iIdx++ )
    {
        if( gdaAvgA[iIdx]==0.0 && gdaAvgB[iIdx]==0.0 )
            return FAILURE;
    }

    // calculate the corners and closest sfaa (?closest side for alignment?)
    giTrace = 5057;
    dCenterX = 0;
    dCenterY = 0;

    // computing center of x and y
    // using mod so that repeated data is not viewed/used twice
    for( iIdx=0; iIdx<4; iIdx++ )
    {
        dX1 = (gdaAvgA[(iIdx+1)%4]-gdaAvgA[iIdx%4])
                     / (gdaAvgB[iIdx%4]-gdaAvgB[(iIdx+1)%4]);
        dY1 = gdaAvgA[iIdx%4] + gdaAvgB[iIdx%4]*dX1;
        dCenterX += dX1;
        dCenterY += dY1;
    }
    dCenterX /= 4.0;
    dCenterY /= 4.0;

    giTrace = 5058;
    gdComputedAngle *= RAD_TO_DEG;
    if( dCenterY == 0.0 )
    {
        if( dCenterX < 0.0 )
            dAngle = 180.0;
        else
            dAngle = 0.0;
    }
    else
        dAngle = atan2(dCenterY,dCenterX)*RAD_TO_DEG;

    dAngle -= gdComputedAngle;

    for( iIdx=0; iIdx<4; iIdx++ )       // make modifications to prevent divide by 0
    {
        if( gdaAvgB[iIdx]==0 )
            gdaAvgB[iIdx] = 0.00000001;
    }
    giTrace=5059;
    // determine flat angle and populate the global variable
    if( (gdaAvgB[0]>=0.0 && gdaAvgB[1]<=0.0 && gdaAvgB[2]>=0.0 && gdaAvgB[3]<=0.0) ||
        (gdaAvgB[0]<=0.0 && gdaAvgB[1]>=0.0 && gdaAvgB[2]<=0.0 && gdaAvgB[3]>=0.0) )
        dFlatAngle = (atan(gdaAvgB[0]) + atan(gdaAvgB[2]))/4.0
                        + (atan(gdaAvgB[1]) + atan(gdaAvgB[3]))/4.0;
    else
        dFlatAngle = (atan(fabs(gdaAvgB[0])) + atan(fabs(gdaAvgB[2])))/4.0
                        + (atan(-fabs(gdaAvgB[1])) + atan(-fabs(gdaAvgB[3])))/4.0;
    dFlatAngle *= RAD_TO_THOUSANDDEG;
    dFlatAngle -= 45000.0 + gdComputedAngle*1000.0;

    // make quadrant adjustment
    if( dFlatAngle < -45000.0 )         // if less than -45 degrees
        dFlatAngle += 90000.0;          //      move the next quadrant
    else if( dFlatAngle > 45000.0 )     // if greater than 45 degrees
        dFlatAngle -= 90000.0;

    // determine the offset angle of the wafer and populate the global variable
    gstWaferCal.m_lOffsetAngle = (long)(dAngle * 1000.0);
    lCenterScaleOffset = AL_CENTER_FIND_ROT;  // may need to be divided by AL_CAL_SCALER
    if( (lCenterScaleOffset-gstWaferCal.m_lOffsetAngle)
            > (gstWaferCal.m_lOffsetAngle+360000L-lCenterScaleOffset) )
        gstWaferCal.m_lOffsetAngle += 360000L;

    // determine the begining and ending of the flat angle and populate the globals
    dFlatAngle -= (double)(glCCDAnglePosition*AL_CAL_SCALER);  // is AL_CAL_SCALER necessary?
    gstWaferCal.m_lFlatBegAngleFromFlat = (long)(dFlatAngle - 1.0);
    gstWaferCal.m_lFlatEndAngleFromFlat = (long)(dFlatAngle + 1.0);

    // determine the Offset Vector of the wafer and populate the global variable
    dOffset = sqrt( dCenterX*dCenterX + dCenterY*dCenterY );
    dOffset *= (double)AL_PIXEL_SPACING;
    iIdx = (int)( dOffset+0.5 );
    iIdx = ( iIdx>5 ? 5 : iIdx);

    gstWaferCal.m_lOffsetVector = (long)(dOffset * gdOffsetSizCor[iIdx]
                                            * (double)AL_XY_PIXEL_SCALER);

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSQCalibrateCCD
 * Abstract:
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    CalibrateCCD()
 ******************************************************************************/
void ALSQCalibrateCCD(void)
{
    int iMax=0;
    int iPos=0;
    int iIdx, iIdx2, iIdx3;
    double dMin=1000000.0, dError, dAngle, dSideX, dSideY;

    glCCDOffset = -1;

    for( iIdx=0; iIdx<giBreakPoints; iIdx++ )
    {
        if( (giaBreakListE[iIdx]-giaBreakListS[iIdx]) > iMax )
        {
            iMax = giaBreakListE[iIdx] - giaBreakListS[iIdx];
            iPos = iIdx;
        }
    }

    for( iIdx=-4500; iIdx<4500; iIdx++ )  // loop 9000 times
    {
        for( iIdx2=giaBreakListS[iPos], iIdx3=0; iIdx2<giaBreakListE[iPos];
                iIdx2++, iIdx3++ )
        {
//            TIRefreshWD();
            dAngle = (double)(glpAngleX[iIdx2%(giNumSqData-1)]) / gdCountInRadians;
            gdaX[iIdx3] = (double)(guipDataX[iIdx2%(giNumSqData-1)]+iIdx)*cos(dAngle);
            gdaY[iIdx3] = (double)(guipDataX[iIdx2%(giNumSqData-1)]+iIdx)*sin(dAngle);
        }

        ALSQCalcFittest(gdaX,gdaY,iIdx3);
        dError = 0.0;

        for( iIdx2=giaBreakListS[iPos]; iIdx2<=giaBreakListE[iPos]; iIdx2++ )
        {
//            TIRefreshWD();
            dAngle = (double)( guipDataX[iIdx2%(giNumSqData-1)] ) / gdCountInRadians;
            dSideX = (double)( guipDataX[iIdx2%(giNumSqData-1)] + iIdx )*cos(dAngle);
            dSideY = (double)( guipDataX[iIdx2%(giNumSqData-1)] + iIdx )*sin(dAngle);
            dError += pow((dSideY-gdSquareSideAA-gdSquareSideBB*dSideX),(double)2.0);
        }
        if( dError < dMin )
        {
            dMin = dError;
            glCCDOffset = (long)iIdx;
        }
    }
    giCCDFirstPixelPos += (int)glCCDOffset;
    FIOSetItemToCalTbl( CT_CCD_POS_COR, (long)giCCDFirstPixelPos );
}
/*******************************************************************************
 * Function:    ALSQFindNotch
 * Abstract:
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    FindCut()
 ******************************************************************************/
int ALSQFindNotch(void)
{
    int iIdx, iIdx2, iIdx3=0;
    unsigned int uiMax, uiMin, uiaPeak[8]={0,0,0,0,0,0,0,0};
    long lPos, laPeakPos[4], lWaferParFlatAngle; // lFlatNormalizedAngle;
    double dFlatAngle;
    //long laScaledPos[4]={0,0,0,0},  // buffers used for scaling and unscaling
    //     laUnscaledPos[4]={0,0,0,0};

    //glNotchEndPos = 42000L;
    //glNotchBegPos = 42000L;

    uiMax = 0;

    for( iIdx=0; iIdx<=32000; iIdx++ )
    {
        if( glpAngleX[iIdx]==0 && guipDataX[iIdx]==0 )
            break;
//        TIRefreshWD();
        if( guipDataX[iIdx] > uiMax )
        {
            uiMax = guipDataX[iIdx];
            lPos = glpAngleX[iIdx];
        }
        iIdx3++;
    }
    // determine the position for each quadrant
    laPeakPos[0] = lPos % AL_COUNT_360DEG;
    laPeakPos[1] = (long)(lPos + 90.0f*gdCountInDeg) % AL_COUNT_360DEG;
    laPeakPos[2] = (long)(lPos + 180.0f*gdCountInDeg) % AL_COUNT_360DEG;
    laPeakPos[3] = (long)(lPos + 270.0f*gdCountInDeg) % AL_COUNT_360DEG;

    for( iIdx=0; iIdx<4; iIdx++)
    {
        if(laPeakPos[iIdx] <= glpAngleX[0])
            laPeakPos[iIdx] += AL_COUNT_360DEG;
    }

    for( iIdx=1; iIdx<iIdx3-1; iIdx++ )
    {
//        TIRefreshWD();
        if( glpAngleX[iIdx]==laPeakPos[0] ||
            (glpAngleX[iIdx-1]<laPeakPos[0] && glpAngleX[iIdx+1]>laPeakPos[0]) )
        {
            if( guipDataX[iIdx] > uiaPeak[0] )
                uiaPeak[0] =  guipDataX[iIdx];
        }

        if( glpAngleX[iIdx]==laPeakPos[1] ||
            (glpAngleX[iIdx-1]<laPeakPos[1] && glpAngleX[iIdx+1]>laPeakPos[1]) )
        {
            if( guipDataX[iIdx] > uiaPeak[1] )
                uiaPeak[1] =  guipDataX[iIdx];
        }

        if( glpAngleX[iIdx]==laPeakPos[2] ||
            (glpAngleX[iIdx-1]<laPeakPos[2] && glpAngleX[iIdx+1]>laPeakPos[2]) )
        {
            if( guipDataX[iIdx] > uiaPeak[2] )
                uiaPeak[2] = guipDataX[iIdx];
        }

        if( glpAngleX[iIdx]==laPeakPos[3] ||
            (glpAngleX[iIdx-1]<laPeakPos[3] && glpAngleX[iIdx+1]>laPeakPos[3]) )
        {
            if( guipDataX[iIdx] > uiaPeak[3] )
                uiaPeak[3] = guipDataX[iIdx];
        }
    }

    uiMin = 30000;
    for( iIdx=0; iIdx<4; iIdx++ )
    {
        if( uiaPeak[iIdx] < uiMin )
        {
            uiMin = uiaPeak[iIdx];
            iIdx2 = iIdx;
        }
    }

    if( FIOGetParamWaferVals(FLAT_ANGLE,&lWaferParFlatAngle) != SUCCESS )
        return FAILURE;
    //**************************************************
    // CKK -- Use scale function instead of computing
    //    dFlatAngle = ((double)laPeakPos[iIdx2]*36000*AL_CAL_SCALER/AL_COUNT_360DEG + glCCDAnglePosition*AL_CAL_SCALER);
    //**************************************************
    //laUnscaledPos[giAxisIndx] = laPeakPos[iIdx2];
    //if( ROScalePos(gulAxis,laUnscaledPos,laScaledPos) == FAILURE )
    //    return FAILURE;
    //dFlatAngle = (double)laScaledPos[giAxisIndx];
    //dFlatAngle *= AL_CAL_SCALER;
    dFlatAngle = ((double)laPeakPos[iIdx2]*36000*AL_CAL_SCALER/AL_COUNT_360DEG + glCCDAnglePosition*AL_CAL_SCALER);
    dFlatAngle = (double)90000.0 - dFlatAngle;
    dFlatAngle /= 10.0;
    dFlatAngle -= (int)(dFlatAngle / 36000) * 36000;
    dFlatAngle = 36000 - dFlatAngle + 4500;
    dFlatAngle -= (int)(dFlatAngle / 36000) * 36000;
    dFlatAngle += lWaferParFlatAngle;
    if (dFlatAngle > 0.0)
        dFlatAngle = (int)((dFlatAngle + 500)/9000)*90000;
    else
        dFlatAngle = (int)((dFlatAngle - 500)/9000)*90000;

    glNotchEndPos = 42000L;
    glNotchBegPos = 42000L;

    // determine quadrant
    //if( dFlatAngle > 0 )
    //    dFlatAngle = dFlatAngle+50;
    //else
    //    dFlatAngle = dFlatAngle-50;

    // take out SFAA & CCDAngle (these will be re-entered in step 8)
    dFlatAngle -= (double)(glCCDAnglePosition*AL_CAL_SCALER);
    dFlatAngle -= (double)(lWaferParFlatAngle*AL_CAL_SCALER);


    // set global variables to reflect computations
    gstWaferCal.m_lFlatBegAngleFromFlat = (long)( dFlatAngle - 1.0 );
    gstWaferCal.m_lFlatEndAngleFromFlat = (long)( dFlatAngle + 1.0 );

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALDetectPrealigner
 * Abstract:    Read the hardware port for the prealigner and determine if the
 *              aligner is a fast prealigner or a standard prealigner
 * Parameters:  None
 * Returns:     SUCCESS or FAILURE
 * Replaces:    detect_pre
 ******************************************************************************/
int ALDetectPrealigner(void)
{
    int iCount, iCCDData, iSaveData, iAngleData, iAlignerData;

    for( iCount=0; iCount<=3; iCount++ )
    {
        iSaveData = inb( AL_CCD_COMMAND_ADR );
        iCCDData = iSaveData & 0x17;    // turn off bits 4, 6, 7, 8
        iCCDData |= 0x10;               // turn on bit 5
        IOWriteIO( -1, iCCDData, AL_CCD_COMMAND_ADR );
        iAngleData = inb( AL_CCD_DATA_ADR );
        iAlignerData = inb( AL_CCD_DATA_ADR + 1);

        // just restore the content in the command port
        IOWriteIO( -1, iSaveData, AL_CCD_COMMAND_ADR );

        // fast prealigner found and can be run - set hardware and software flags
        if( iAngleData==0xAA && iAlignerData==0xAA )
        {
            giAlignerType = AL_FASTPRE;
	    ALChangeFastMode(AL_FASTPRE, AL_FASTMEASURE_DIS);
            return SUCCESS;
        }
    }

    // can not run in fast mode - reset hardware and set software flags
    giAlignerType = AL_STANDARD;
    ALChangeFastMode(AL_STANDARD, AL_REGULARMEASURE_DIS);
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSetPrealignerMode
 * Abstract:    Set the prealigner type to the requested type, if able.  Can not set
 *              to fast mode if hardware does not support the option.
 * Parameters:  lModeArg
 * Returns:     SUCCESS or FAILURE
 * Replaces:    set_pre_mode()
 ******************************************************************************/
int ALSetPrealignerMode(int iModeArg)
{
    if( giAlignerType == AL_FASTPRE )  // if fast prealigner
    {
        if( iModeArg == AL_FASTPRE )  // if requesting fast mode
            // set the hardware bits to fast mode
			ALChangeFastMode(AL_FASTPRE, AL_FASTMEASURE_DIS);
        else                            // requesting standard mode
			ALChangeFastMode(AL_STANDARD, AL_REGULARMEASURE_DIS);
    }
    else                                // standard prealigner
    {
        if( iModeArg == AL_FASTPRE )  // if requesting fast mode
        {
            // check if aligner type is fast and just not being used
			// note: this function always return success.
            ALDetectPrealigner();
			/* aligner type is not fast, set back the mode to standard and return failure.
			 * note: mode and type already set in the WADectectAlignerHardware() */
            if( giAlignerType != AL_FASTPRE )
                return FAILURE;
        }
        else
        {
            giAlignerMode = giAlignerType = AL_STANDARD;
            glDistanceBetween2Meas = AL_REGULARMEASURE_DIS;
        }
    }
    return SUCCESS;
}
/* for fast aligner. */
int ALChangeFastMode(int iFlagArg, long lDistArg)
{
	unsigned char iData;

	iData = inb(AL_CCD_COMMAND_ADR);
	if(iFlagArg)
	{
		iData &= 0xEF;
		iData |= 0x08;
	}
	else
		iData |= 0x18; // set bit 3 & 4 ON for standard mode
//		iData &= 0xE7;
	IOWriteIO(-1, iData, AL_CCD_COMMAND_ADR);
	giAlignerMode = iFlagArg;
	glDistanceBetween2Meas = lDistArg;
	return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSetFineScanSpeed
 * Abstract:    Send galil the right speed for finding the flat.  The speed selected
 *              depends on the aligning speed mode and wafer size.  The same
 *              acceleration and deceleration is used in either case.
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replaces:    send_flt_speed
 ******************************************************************************/
int ALSetFineScanSpeed(void)
{
    //long lWaferSize;
    long laFlatSpeed[8] = {0,0,0,0,0,0,0,0};
	long laFlatAccel[8] = {0,0,0,0,0,0,0,0};

    // get the acceleration and deceleration speed for finding the flat
	if(ALGetAligningSpeed(AL_SP_MEASURE_FLAT, laFlatSpeed, laFlatAccel)==FAILURE)
		return FAILURE;

    if(ALSetAligningGASpeed(gulAxis, laFlatSpeed, laFlatAccel)==FAILURE)
        return FAILURE;

   return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSinglePickAlignment
 * Abstract:    find the flat and compute the correction offset position by
 * 				intellipick algorithm.
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 ******************************************************************************/
int ALSinglePickAlignment(void)
{
	long lPosition;

	if(ALNotchFromCenter( )==FAILURE)
		return FAILURE;
    giTrace = 7042;
	if(ALFindNotchMidAngle(&lPosition)==FAILURE)
		return FAILURE;
    giTrace = 7052;
	/* calculate the position offset for robot and prealign to pick-up and rotate. */
    if(ALSinglePickPosition()==FAILURE)
        return FAILURE;
    giTrace = 7062;

    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSinglePickPosition
 * Abstract:    Find proper pickup offset correction position of:
 * 					- Robot T-axis (relative position)
 * 					- Robot R-axis (relative position)
 * 					- Prealigner t-axis (absolute positon)
 * Parameters:
 * Returns:     SUCCESS or FAILURE
 * Replace:		WaferPickupPosition
 ******************************************************************************/
int ALSinglePickPosition ()
{
    double  dAlfa0,  	/* [radian] offset vector angle ref. to CCD (alfa_offset) */
            dVecOffset, /* [mm] 	offset vector amplitude */
            dAlfa1,     /* [radian] notch/flat vector angle ref. to CCD (alfa_notch) */
            dDelta,     /* [radian] robot-chuck-CCD angle from calibration */
            dL,         /* [mm]     robot-chuck distance from calibration */
            dTheta,  	/* [radian] robot corection angle of T-axis */
            dZeta,      /* [radian] pre-aligner corection angle of chuck axis */
            dPhi,
            dGamma,
            dOmega,
            dEpsilon,
            dBeta,      /* [radian] new offset angle for ref. point from (ofset vector is ref.) (PIDEF-alfa0 + alpha1) */
            dPsi,       /* {radian] angle for robot triagle, chuck point */
            dl;         /* [mm]     side of robot right angle triagle */

    double  dTemp1, dTemp2, dTemp3;     /* temporary variables */
    double  dAdjustOffset;              /* to adjust the value before convert to long value from double value */
	char    caBuf[80];                  /* temporary buffer */
	int     iCmdPort;                   /* command port id */
	long    lFlatAngle;                 /* temporary variable to hold value of final flat angle that set by user(SFAA) */
	double  dTAxisOffset,               /* hold the calculated correction position of robot T-axis */
		    dRAxisOffset,               /* hold the calculated correction position of robot R-axis */
		    dtAxisOffset;               /* hold the calculated correction position of prealign t-axis */

	caBuf[0] = 0;
    dTemp1 = dTemp2 = dTemp3 = 0;
	iCmdPort = SERGetCmdPort();

	/* convert offset angle into radian  */
    dAlfa0   = (double)gstWaferCal.m_lOffsetAngle * THOUSANDDEG_TO_RAD;

   /* convert vector offset into mm */
    dVecOffset      = (double)gstWaferCal.m_lOffsetVector /1000.0;
    if( giDisplayMode & AL_DISPAR_B )
    {
        sprintf( caBuf, "\r\n off_ang=%ld, alfa0=%lf, off_amp=%ld, r0=%lf\r\n",
   	        gstWaferCal.m_lOffsetAngle, dAlfa0, gstWaferCal.m_lOffsetVector, dVecOffset);
        SERPutsTxBuff(iCmdPort, caBuf );
    }

	/* convert mid_flat angle to radian */
    dAlfa1   = (double)gstWaferCal.m_lFlatMiddle * THOUSANDDEG_TO_RAD;

	/* convert distance from micron to mm */
    dL = (double)FIOGetItemFromCalTbl(CT_ROB_CHUCK);
    dL = dL / 1000.0;

    if( giDisplayMode & AL_DISPAR_B )
    {
		caBuf[0] = 0;
        sprintf( caBuf, " flat_mid=%ld, alfa1=%lf, L(mm)=%lf\r\n",
   	        gstWaferCal.m_lFlatMiddle, dAlfa1, dL);
        SERPutsTxBuff(iCmdPort, caBuf );
    }

    if (dL <= 10.0)
    {
		/* 10mm? no way! this length must have been set thru SCT 5,# command. */
        return FAILURE;
    }

	/* delta = PIDEF - angle between robot and sensor */
	/* convert the angle between the robot and sensor to radian */
	dDelta = (double)FIOGetItemFromCalTbl(CT_CCD_ROB_ANG);
    if( giDisplayMode & AL_DISPAR_B )
    {
		caBuf[0] = 0;
        sprintf( caBuf, " rob_sensor_ang=%ld\r\n", (long)dDelta);
        SERPutsTxBuff(iCmdPort, caBuf );
    }

	dDelta = dDelta * THOUSANDDEG_TO_RAD;
	dDelta *= AL_CAL_SCALER;
    /* CORRECT THE FLAT POSITION WHEN CALCULATION!!!! */
    dAlfa1 = dAlfa1 + dDelta;
	dDelta = PIDEF - dDelta;

    if( FIOGetParamWaferVals(FLAT_ANGLE,&lFlatAngle) != SUCCESS )
        return FAILURE;
	/* convert the mid_flat angle to radian */
	dOmega = (double)lFlatAngle * THOUSANDDEG_TO_RAD;
	dOmega *= AL_CAL_SCALER;

	/* find beta */
    dBeta = PIDEF - dAlfa0 + dAlfa1;

	/* find epsilon */
    dEpsilon = PIDEF - dOmega;

	/* find gamma (cwr) */
    dGamma = dEpsilon - dBeta;

	/* find offset correction position for robot T-axis */
    dTemp1 = dVecOffset / dL;
    dTemp2 = sin(dGamma);
    dTemp1 = dTemp1 * dTemp2;
    dTheta = asin(dTemp1);

	/* find psi and gamma needs to normalize only here. */
	/*  psi = PIDEF - |theta| - |gamma| */
	dTemp1 = dGamma;
    ALSinglePickNormAngle(&dTemp1);
    dTemp2 = fabs(dTemp1);
    dTemp3 = fabs(dTheta);
    dPsi = PIDEF - dTemp2 - dTemp3;
    if( giDisplayMode & AL_DISPAR_B )
    {
		caBuf[0] = 0;
        sprintf( caBuf, " gamma=%lf, theta=%lf, psi=%lf\r\n", dGamma, dTheta, dPsi);
        SERPutsTxBuff(iCmdPort, caBuf );
    }

	/* find phi */
	/* zeta = alfa1 - (PIDEF - delta) + omega; */
    if(dTheta > 0)
        dPhi = dDelta + dPsi;
    else
        dPhi = dDelta - dPsi;

    dZeta =  dAlfa0 + dPhi;
    ALSinglePickNormAngle(&dZeta);
    if( giDisplayMode & AL_DISPAR_B )
    {
		caBuf[0] = 0;
        sprintf( caBuf, " phi=%lf, zeta=%lf\r\n", dPhi, dZeta);
        SERPutsTxBuff(iCmdPort, caBuf );
    }

    // FIND l for robot extension
   // l = L * sin(psi) / sin(gamma)
    dTemp1 = sin(dPsi);
    dTemp1 = dTemp1 * dL;
	dTemp3 = dGamma;
    ALSinglePickNormAngle(&dTemp3);
    dTemp3 = fabs(dTemp3);
    dTemp2 = sin(dTemp3);
    if (dTemp2 == 0.0)
        dl = dL;
    else
        dl = dTemp1 / dTemp2;
    if( giDisplayMode & AL_DISPAR_B )
    {
		caBuf[0] = 0;
        sprintf( caBuf, " l (wafer pick up)=%lf\r\n", dl);
        SERPutsTxBuff(iCmdPort, caBuf );
    }
    dAdjustOffset = .5;

	/*  convert T-robot need to robot for pickup to user unit [degree * 100]*/
	dTAxisOffset = dTheta * RAD_TO_DEG;
	dTAxisOffset = dTAxisOffset * 100.0 + dAdjustOffset;

	/* find l is the distance to pickup which the difference between L and l,
	 * and convert offset position to user unit [micron] */
    dTemp1 = dl - dL;
    dTemp1 = dTemp1 / 25.4;
    dRAxisOffset = dTemp1 * 1000.0 + dAdjustOffset;

	/* convert final rotation position to user unit [degree * 100]*/
	dtAxisOffset = dZeta * RAD_TO_DEG;
	dtAxisOffset = dtAxisOffset * 100 + dAdjustOffset;

    /* set to the global variables for ZZ3 command to return the value to macro for pickup. */
	glRobotTAxisOffset 	= (long)dTAxisOffset;
	glRobotRAxisOffset 	= (long)dRAxisOffset;
	glPreTAxisOffset 	= (long)dtAxisOffset;

    if( giDisplayMode & AL_DISPAR_B )
    {
        caBuf[0] = 0;
        sprintf( caBuf, " Result: rob_T=%f, rob_R=%f, pre_T=%f\r\n",
   	        dTAxisOffset, dRAxisOffset, dtAxisOffset);
        SERPutsTxBuff(iCmdPort, caBuf );
    }

    return SUCCESS;
}
int ALFindNotchMidAngle(long *lpPositionArg)
{
	char caBuf[40];
    double dDelta;

    if(gstWaferCal.m_lFlatBegAngleFromFlat > gstWaferCal.m_lFlatEndAngleFromFlat)
        *lpPositionArg = (gstWaferCal.m_lFlatBegAngleFromFlat +
                        gstWaferCal.m_lFlatEndAngleFromFlat + 360000L) / 2L;
    else
        *lpPositionArg = (gstWaferCal.m_lFlatBegAngleFromFlat +
                        gstWaferCal.m_lFlatEndAngleFromFlat) / 2L;

     /* for single-axis prealigner, the zero reference position is facing the robot direction. */
    if(giDefineType & DFSAP4)
    {
        dDelta = (double)FIOGetItemFromCalTbl(CT_CCD_ROB_ANG);
        *lpPositionArg -= dDelta * AL_CAL_SCALER;
    }
    else
        *lpPositionArg += glCCDAnglePosition * AL_CAL_SCALER;

    gstWaferCal.m_lFlatMiddle = *lpPositionArg % 360000L;
    if( giDisplayMode & AL_DISPAR_A )
    {
		caBuf[0] = 0;
        sprintf( caBuf, "\r\n___Flat position was___%ldø\r\n",
                    gstWaferCal.m_lFlatMiddle );
        SERPutsTxBuff( SERGetCmdPort(), caBuf );
    }
	return SUCCESS;
}
int ALSinglePickNormAngle (double *dAngleArg)
{
    double dTwoPI = 2.0 * PIDEF;

    while (*dAngleArg > dTwoPI)
        *dAngleArg -= dTwoPI;

    while (*dAngleArg < -dTwoPI)
        *dAngleArg += dTwoPI;

    if ( *dAngleArg > PIDEF )
        *dAngleArg = -2.0 * PIDEF + *dAngleArg;

    if ( *dAngleArg < -PIDEF )
        *dAngleArg =  2.0 * PIDEF + *dAngleArg;

    return SUCCESS;
}
int ALInitNotchIndex(int iDirArg, int iBeginDataIndxArg, int iRndDataIndxArg,
						 int iStartIndxArg, int iFlatEndIndx, int iMaxMeasureArg)
{
    giDataReadPosDir = iDirArg;
    giBegDataPos = iBeginDataIndxArg;
    giRndDataIdx = iRndDataIndxArg;
    giStartDataIdx = iStartIndxArg;
    giFlatIndexEndLastRead = iFlatEndIndx;
    giMaxNumMeasStored = iMaxMeasureArg;
	return SUCCESS;
}
// load the calibration table to the current buffer for
// new function to break out the step for intellipick alignment.
int ALCalcSinglePick(void)
{
    giTrace = 7012;
	if(ALLoadWaferCal()==FAILURE)
		return FAILURE;
    giTrace = 7022;
	if(ALCenterCircuit()==FAILURE)
		return FAILURE;
    giTrace = 7032;
	if(ALSinglePickAlignment()==FAILURE)
		return FAILURE;
	return SUCCESS;
}

// Return the computed flat angle
long ALReadNotchAngle( void )
{
    return glNotchAngle;
}
