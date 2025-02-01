/**********************************************************************
/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : alfio.c
 * Description          : Contains access Functions for Wafer Calibration Structure
 *                  as well as Get and Set Functions for Aligner module variables
 *                  needed from outside the module
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
 *  Program:        Aligner Module GetSet functions
 *  File:           WAGetSet.c
 *  Function:
 *  Description:    Contains access Functions for Wafer Calibration Structure
 *                  as well as Get and Set Functions for Aligner module variables
 *                  needed from outside the module
 *
 *  Modification history:
 *
 *      Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#include <string.h>
#include <stdlib.h>
#include "rofio.h"
#include "sck.h"
#include "alk.h"
#include "alpre.h"
#include "alfio.h"
#include "fiol.h"
#include "fiog.h"

extern int giVersionPA;

extern int giVersionPA;

/*******************************************************************************
 * Function:    ALSetIntVars
 * Abstract:    Set the Variables used during Alignment process
 * Parameters:  iVariableTypeArg - Type of variable
 *              iIndex - this is only applicable for some variable types
 *              iVariableValueArg - the value the variable is to be set to
 * Returns:     SUCCESS / FAILURE
 ******************************************************************************/
int ALSetIntVars(int iVariableTypeArg, int iIndexArg, int iVariableValueArg)
{
    switch(iVariableTypeArg)
    {
        case AL_TRACE :
            giTrace = iVariableValueArg;
            break;
        case AL_OFFSET_INDEX_END :
            giOffsetIndexEnd = iVariableValueArg;
            break;
        case AL_FLAT_INDEX_LAST_READ_END :
            giFlatIndexEndLastRead = iVariableValueArg;
            break;
        case AL_FLAT_INDEX_END_PREV :
            giFlatIndexEndPrev = iVariableValueArg;
            break;
        case AL_CCD_FIRST_PIXEL_POS :
            giCCDFirstPixelPos = iVariableValueArg;
            break;
        case AL_DISPLAY_MODE :
            giDisplayMode = iVariableValueArg;
            break;
        case AL_ROUND_DATA_INDEX :
            giRndDataIdx = iVariableValueArg;
            break;
        case AL_ALIGNMENT_STATUS :
            giAlignmentStatus = iVariableValueArg;
            break;
        case AL_NUM_CORRECTIONS :
            giNumCorr = iVariableValueArg;
            break;
        case AL_DATA_COLLECTION :
            giMeasurementInProgress = iVariableValueArg;
            break;
        case AL_ALIGNING_FLAG :
            giAligning = iVariableValueArg;
            break;
        case AL_ALIGNMENT_STEP :
            giWAStep = iVariableValueArg;
            break;
        case AL_TRACE_TABLE :
            if( (iIndexArg<0) || (iIndexArg>9) )
                return FAILURE;
            giaTraceTable[iIndexArg] = iVariableValueArg;
            break;
        case AL_DATA_DIFF :
/*            if( (iIndexArg<0) || (iIndexArg>(AL_MAX_INDEX+1)) )
                return FAILURE;
            giaDataDiff[iIndexArg] = iVariableValueArg;*/
            break;
        case AL_STAT :
            if( (iIndexArg<0) || (iIndexArg>(AL_MAX_INDEX+1)) )
                return FAILURE;
            giaDataStat[iIndexArg] = iVariableValueArg;
            break;
        case AL_PREALIGNER_MODE :
            if( giAlignerType )
            {   // Fast prealigner can use both modes
                if( iVariableValueArg==AL_STANDARD ||
                    iVariableValueArg==AL_FASTPRE )
                    giAlignerMode = iVariableValueArg;
                else
                    return FAILURE;
            }
            else
            {   // Non-Fast prealigner can only use standard mode
                if( iVariableValueArg==AL_STANDARD )
                    giAlignerMode = iVariableValueArg;
                else
                    return FAILURE;
            }
            break;
        default:
            return FAILURE;
    };
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALGetIntVars
 * Abstract:    Set the Variables used during Alignment process
 * Parameters:  iVariableTypeArg
 *              ipVariableValueArg
 * Returns:     SUCCESS / FAILURE
 ******************************************************************************/
int ALGetIntVars(int iVariableTypeArg, int iIndexArg, int *ipVariableValueArg)
{
    switch(iVariableTypeArg)
    {
        case AL_TRACE :
            *ipVariableValueArg = giTrace;
            break;
        case AL_OFFSET_INDEX_END :
            *ipVariableValueArg = giOffsetIndexEnd;
            break;
        case AL_FLAT_INDEX_LAST_READ_END :
            *ipVariableValueArg = giFlatIndexEndLastRead;
            break;
        case AL_FLAT_INDEX_END_PREV :
            *ipVariableValueArg = giFlatIndexEndPrev;
            break;
        case AL_CCD_FIRST_PIXEL_POS :
            *ipVariableValueArg = giCCDFirstPixelPos;
            break;
        case AL_DISPLAY_MODE :
            *ipVariableValueArg = giDisplayMode;
            break;
        case AL_ROUND_DATA_INDEX :
            *ipVariableValueArg = giRndDataIdx;
            break;
        case AL_ALIGNMENT_STATUS :
            *ipVariableValueArg = giAlignmentStatus;
            break;
        case AL_NUM_CORRECTIONS :
            *ipVariableValueArg = giNumCorr;
            break;
        case AL_DATA_COLLECTION :
            *ipVariableValueArg = giMeasurementInProgress;
            break;
        case AL_ALIGNING_FLAG :
            *ipVariableValueArg = giAligning;
            break;
        case AL_ALIGNMENT_STEP :
            *ipVariableValueArg = giWAStep;
            break;
        case AL_TRACE_TABLE :
            if( (iIndexArg<0) || (iIndexArg>9) )
                return FAILURE;
            *ipVariableValueArg = giaTraceTable[iIndexArg];
            break;
        case AL_DATA_DIFF :
            if( (iIndexArg<0) || (iIndexArg>(AL_MAX_INDEX+1)) )
                return FAILURE;
            if (iIndexArg == 0)
                *ipVariableValueArg = 0;
            else
                *ipVariableValueArg = (int)(guiaData[iIndexArg] - guiaData[iIndexArg-1]);
            break;
        case AL_STAT :
            if( (iIndexArg<0) || (iIndexArg>(AL_MAX_INDEX+1)) )
                return FAILURE;
            *ipVariableValueArg = giaDataStat[iIndexArg];
            break;
        case AL_PREALIGNER_MODE :
            *ipVariableValueArg = giAlignerMode;
            break;
        case AL_PREALIGNER_TYPE :
            *ipVariableValueArg = giAlignerType;
            break;
        case AL_CHUCK_VAC_SENSOR :
            *ipVariableValueArg = giChuckVacSensorInputPos;
            break;

        default:
            return FAILURE;
    };
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSetLongVars
 * Abstract:    Set the Long Variables used during Alignment process
 * Parameters:  iVariableTypeArg - Type of variable
 *              iIndex - this is only applicable for some variable types
 *              lVariableValueArg - the value the variable is to be set to
 * Returns:     SUCCESS / FAILURE
 ******************************************************************************/
int ALSetLongVars(int iVariableTypeArg, int iIndexArg, long lVariableValueArg)
{
    switch(iVariableTypeArg)
    {
        case AL_DATA_ANGLE :
            if( (iIndexArg < 0) || (iIndexArg > (AL_MAX_INDEX +1)) )
                return FAILURE;
            glaAngle[iIndexArg] = lVariableValueArg;
            break;
        case AL_OFFSET_VECTOR :
            gstWaferCal.m_lOffsetVector = lVariableValueArg;
            break;
        case AL_OFFSET_ANGLE :
            gstWaferCal.m_lOffsetAngle = lVariableValueArg;
            break;
        case AL_FLAT_MIDPOINT :
            gstWaferCal.m_lFlatMiddle = lVariableValueArg;
            break;
        default:
            return FAILURE;
    };
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALGetLongVars
 * Abstract:    Get the Long Variables used during Alignment process
 * Parameters:  iVariableTypeArg - Type of variable
 *              iIndex - this is only applicable for some variable types
 *              lpVariableValueArg - the value the variable gotten
 * Returns:     SUCCESS / FAILURE
 ******************************************************************************/
int ALGetLongVars(int iVariableTypeArg, int iIndexArg, long *lpVariableValueArg)
{
    switch(iVariableTypeArg)
    {
        case AL_DATA_ANGLE :
            if( (iIndexArg < 0) || (iIndexArg > (AL_MAX_INDEX +1)) )
                return FAILURE;
            *lpVariableValueArg = glaAngle[iIndexArg];
            break;
        case AL_OFFSET_VECTOR :
            *lpVariableValueArg = gstWaferCal.m_lOffsetVector;
            break;
        case AL_OFFSET_ANGLE :
            *lpVariableValueArg = gstWaferCal.m_lOffsetAngle;
            break;
        case AL_FLAT_MIDPOINT :
            *lpVariableValueArg = gstWaferCal.m_lFlatMiddle;
            break;
        case AL_WAFER_SIZE :
            *lpVariableValueArg = gstWaferCal.m_lWaferSize;
            break;
		case AL_ALL_PRE_AXES : 
			*lpVariableValueArg = (long)gulAllAxes;
			break;
		case AL_CHUCK_AXIS :
			*lpVariableValueArg = (long)gulAxis;
			break;
		case AL_ROBOT_T_PICKUP_POS :
			*lpVariableValueArg = glRobotTAxisOffset;
			break;
		case AL_ROBOT_R_PICKUP_POS :
			*lpVariableValueArg = glRobotRAxisOffset;
			break;
		case AL_CHUCK_FINAL_ROTATE_POS :
			*lpVariableValueArg = glPreTAxisOffset;
			break;
        default:
            return FAILURE;
    };
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALGetPreData
 * Abstract:    Gets item from Data collection buffer array
 * Parameters:  lIndexArg - index of the array
 *              puiDataArg - returning value
 * Returns:     SUCCESS
 ******************************************************************************/
int ALGetPreData(long lIndexArg, unsigned int *puiDataArg)
{
    if( (lIndexArg < 0) || (lIndexArg>(AL_MAX_INDEX+1) ) )
        return FAILURE;
    *puiDataArg = guiaData[(int)lIndexArg];
    return SUCCESS;
}
/*******************************************************************************
 * Function:    ALSetPreData
 * Abstract:    Sets item from Data collection buffer array
 * Parameters:  lIndexArg - index of the array
 *              uiDataArg - returning value
 * Returns:     SUCCESS
 ******************************************************************************/
int ALSetPreData(long lIndexArg, unsigned int uiDataArg)
{
    if((lIndexArg < 0) || (lIndexArg>(AL_MAX_INDEX+1)))
        return FAILURE;
    guiaData[(int)lIndexArg] = uiDataArg;
    return SUCCESS;
}
/*******************************************************************************
 * Function:
 * Abstract:
 * Parameters:
 *
 * Returns:     SUCCESS
 ******************************************************************************/
int ALGetDUPCount(unsigned long *ulpDuplicateCountArg)
{
    *ulpDuplicateCountArg = gulDuplicateCount;
    return SUCCESS;
}
/*******************************************************************************
 * Function:
 * Abstract:
 * Parameters:
 *
 * Returns:     SUCCESS
 ******************************************************************************/
int ALSetDUPCount(unsigned long ulDuplicateCountArg)
{
    gulDuplicateCount = ulDuplicateCountArg;
    return SUCCESS;
}
/*******************************************************************************
 * Function:
 * Abstract:
 * Parameters:
 *
 * Returns:     SUCCESS
 ******************************************************************************/
int ALGetBadCounter(unsigned long *ulpBadCountArg)
{
    *ulpBadCountArg = gulBadCount;
    return SUCCESS;
}
/*******************************************************************************
 * Function:
 * Abstract:
 * Parameters:
 *
 * Returns:     SUCCESS
 ******************************************************************************/
int ALSetBadCount(unsigned long ulBadCountArg)
{
    gulBadCount = ulBadCountArg;
    return SUCCESS;
}
/*******************************************************************************
 * Function: 	ALGetAligningSpeed
 * Abstract:    Get speed and accel or measurement speed and accel from
 * 				wafer parameter file.
 * Parameters:  iWhichArg - type of speed or accel that want to get
 * 					        (AL_OPERATIONAL or AL_MEASURE_CENTER)
 * 				lpWaferSpeedArg - store the speed that get from wafer param.
 * 				lpWaferAccelArg - store the accel that get from wafer param.
 *
 * Returns:     SUCCESS/FAILURE
 ******************************************************************************/
int ALGetAligningSpeed(int iWhichArg, long *lpWaferSpeedArg, long *lpWaferAccelArg)
{
	long lWaferSize;
	long lWaferSpeed, lWaferAccel;
	int  i, iSpeedDesc, iAccelDesc;

    if( FIOGetParamWaferVals(WAFER_SIZE,&lWaferSize) == FAILURE )
        return FAILURE;
    if(iWhichArg == AL_SP_OPERATIONAL)
    {
    	if( lWaferSize==AL_WAFER_SIZE_12 || lWaferSize==AL_WAFER_SIZE_300 )
    	{
        	// get speed and acceleration for no wafer present
        	if( FIOGetParamNoWaferSpeed(lpWaferSpeedArg) == FAILURE )
            	return FAILURE;
        	if( FIOGetParamNoWaferAccel(lpWaferAccelArg) == FAILURE )
            	return FAILURE;
    	}
    	else
    	{
            // get speed and acceleration for wafer present
            if( FIOGetParamWaferSpeed(lpWaferSpeedArg) == FAILURE )
            	return FAILURE;
            if( FIOGetParamWaferAccel(lpWaferAccelArg) == FAILURE )
            	return FAILURE;
    	}
	if(giAxisIndx != 0)
	{
	    lpWaferSpeedArg[giAxisIndx] = lpWaferSpeedArg[0];
	    lpWaferAccelArg[giAxisIndx] = lpWaferAccelArg[0];
	}
	if(!giVersionPA)
	{
	    for (i=0; i<4; ++i)
	    {
		lpWaferSpeedArg[i+4] = lpWaferSpeedArg[i];
		lpWaferAccelArg[i+4] = lpWaferAccelArg[i];
	    }
	}
	if(!giVersionPA)
	{
	    for (i=0; i<4; ++i)
	    {
		lpWaferSpeedArg[i+4] = lpWaferSpeedArg[i];
		lpWaferAccelArg[i+4] = lpWaferAccelArg[i];
	    }
	}
    }
    else
    {
    	if(iWhichArg == AL_SP_MEASURE_CENTER)
	{
	    iAccelDesc = MEASURE_ACCEL_CENTER;
    	    if( giAlignerMode == AL_FASTPRE && giAlignerType == AL_FASTPRE )
    		{
        		if( lWaferSize == AL_WAFER_SIZE_12 || lWaferSize == AL_WAFER_SIZE_300 )
					iSpeedDesc = MEASURE_SPEED_CNTRF300;
        		else
					iSpeedDesc = MEASURE_SPEED_CNTRF200;
    		}
    		else
				iSpeedDesc = MEASURE_SPEED_CENTER;
		}
		else
		{
			iAccelDesc = MEASURE_ACCEL_FLAT;
    		if( giAlignerMode == AL_FASTPRE && giAlignerType == AL_FASTPRE )
    		{
        		if( lWaferSize == AL_WAFER_SIZE_12 || lWaferSize == AL_WAFER_SIZE_300 )
					iSpeedDesc = MEASURE_SPEED_FLAT_F300;
        		else
					iSpeedDesc = MEASURE_SPEED_FLAT_F200;
    		}
    		else
				iSpeedDesc = MEASURE_SPEED_FLAT;
		}

        if( FIOGetParamWaferVals(iSpeedDesc,&lWaferSpeed)==FAILURE )
            return FAILURE;
        if( FIOGetParamWaferVals(iAccelDesc,&lWaferAccel)==FAILURE )
            return FAILURE;
    	// Conversion intended for single axis aligner but is good for both
    	lpWaferSpeedArg[giAxisIndx] = lWaferSpeed;
    	lpWaferAccelArg[giAxisIndx] = lWaferAccel;
//printf("giAxisIndx=%d waferSpeed=%d Accel=%d\n",giAxisIndx, lWaferSpeed, lWaferAccel);
//		if(!giVersionPA)
//		{
//			for (i=0; i<4; ++i)
//			{
//				lpWaferSpeedArg[i+4] = lpWaferSpeedArg[i];
//				lpWaferAccelArg[i+4] = lpWaferAccelArg[i];
//			}
    	lpWaferAccelArg[giAxisIndx] = lWaferAccel;
//printf("giAxisIndx=%d waferSpeed=%d Accel=%d\n",giAxisIndx, lWaferSpeed, lWaferAccel);
//		if(!giVersionPA)
//		{
//			for (i=0; i<4; ++i)
//			{
//				lpWaferSpeedArg[i+4] = lpWaferSpeedArg[i];
//				lpWaferAccelArg[i+4] = lpWaferAccelArg[i];
//			}
//		}
	}
	return SUCCESS;
}
/*******************************************************************************
 * Function: 	WASetSpAcToGail
 * Abstract:    to set the speed, accel and decel to galil as operational
 * 				speed and accel.
 * Parameters:  ulAxisArg 		- the axis that need to set the speed and accel.
 * 				lpWaferSpeedArg - speed to be set
 * 				lpWaferAccelArg - accel to be set
 *
 * Returns:     SUCCESS/FAILURE
 ******************************************************************************/
int ALSetAligningGASpeed(unsigned long ulAxisArg, long *lpWaferSpeedArg, long *lpWaferAccelArg)
{
    long laWaferDecel[8] = {0,0,0,0,0,0,0,0};

    memcpy(laWaferDecel, lpWaferAccelArg, sizeof(long)*8);
    /* set operating speed, acceleration and deceleration */
    if(ROSetParameter(FALSE, ulAxisArg, lpWaferSpeedArg, OPERATIONAL_SPEED)==FAILURE)
    {
printf("axis=%d speed=%d\n",ulAxisArg,lpWaferSpeedArg[4]);
        goto error_exit;
    }
    if(ROSetParameter(FALSE, ulAxisArg, lpWaferAccelArg, OPERATIONAL_ACCEL)==FAILURE)
    {
printf("axis=%d accel=%d\n",ulAxisArg,lpWaferAccelArg[4]);
        goto error_exit;
    }
    if(ROSetParameter(FALSE, ulAxisArg, laWaferDecel, OPERATIONAL_DECEL)==FAILURE)
    {
printf("axis=%d decel=%d\n",ulAxisArg,laWaferDecel[4]);
        goto error_exit;
    if(ROSetParameter(FALSE, ulAxisArg, lpWaferSpeedArg, OPERATIONAL_SPEED)==FAILURE)
    {
printf("axis=%d speed=%d\n",ulAxisArg,lpWaferSpeedArg[4]);
        goto error_exit;
    }
    if(ROSetParameter(FALSE, ulAxisArg, lpWaferAccelArg, OPERATIONAL_ACCEL)==FAILURE)
    {
printf("axis=%d accel=%d\n",ulAxisArg,lpWaferAccelArg[4]);
        goto error_exit;
    }
    if(ROSetParameter(FALSE, ulAxisArg, laWaferDecel, OPERATIONAL_DECEL)==FAILURE)
    {
printf("axis=%d decel=%d\n",ulAxisArg,laWaferDecel[4]);
        goto error_exit;
    }
    return SUCCESS;
error_exit:
    giAlignmentStatus |= AL_BAD_SPEED_ACCEL;
    return FAILURE;
}
