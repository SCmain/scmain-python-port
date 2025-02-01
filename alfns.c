/****************************************************************
 *
 * Program:     Wafer Aligner Interface funciton
/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : alfns.c
 * Description          : Initialize the Aligner module
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
 * Program:     Wafer Aligner Interface function
 * Functions:   Initialization
 * Description: Initialize the Aligner module
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#include "sck.h"
#include "alk.h"
#include "alpre.h"
#include "alcomp.h"
#include "alfns.h"
#include "ro.h"
#include "fiog.h"
#include "alisr.h"
#include "sctim.h"

// allocate memory for variables global to this module
stAlignResult gstWaferCal;      // wafer calibration structure

unsigned int guiaData[AL_MAX_INDEX+1];    // data - data read from ccd during chuck rotation

int giRndDataIdx;               // data_measure_i - begining index of data read
                                //      - used by Math, Proc, intr
int giStartDataIdx;             // start_data_i - position in read data for the round
                                //      - used by math, proc, intr
int giRndLevel;                 // round_level - used by proc and math
int giMinFlatSize;              // flat_lev_fl - shared by proc and math
int giTrace;                    // trace - wafer alignment trace var
                                //      - shared by proc and math
int giFlatIndexEndLastRead;     // flat_i_end - last read data for flat pos
                                //      - shared by proc, math, intr
int giFindFlatSubStep;          // fff_substep - find flat function substep num
                                //      - shared by proc and math
int giaDataStat[AL_MAX_INDEX+1];  // datastat - status of data read
                                        //      - shared by all
int giDataReadPosDir;           // pos_mdir - positive direction of reading data
int giPrevNumCorr;              // prev_nofcorr - used by process and get/set
int giNumCorr;                  // nofcorr - used by process and get/set
int giAligning;                 // aligning - used by process and get/set
int giOffsetIndexEnd;           // offset_i_end - last read data from CCD
                                //      - used by proc and math
int giWAStep;                   // step_bal - shared by proc and get/set
int giFlatIndexEndPrev;         // flat_i_end_old - shared by process and get/set
int giDisplayMode;              // disp - display mode while aligning
                                //      - used everywhere
int giWaitForFlatPositioning;   // pfwaswaiting - shared by proc and math
int giWaitForFlatMeasuring=0;   // fms_waswaiting
int giAlignmentStatus=0x1000;   // al_st - alignment status
                                //      - used by process and math
int giAlignerType;              // iPreType
int giAlignerMode;              // iAlignMode
int giDefineType;               // Define flags
int giEmulatorMode;             //  Emulator Mode
// FIXME - global replace giAxisIndx with giChuckAxisIndx
int giAxisIndx;          // the index corresponding to the prealigner chuck
                                // axis, used when pulling information from the
                                // arrays returned by the data module primarily
int giCCDNumber;                // ccd_no - active ccd number
int giaTraceTable[AL_TRACE_TABLE_SIZE]={0,0,0,0,0,0,0,0,0,0};
                                // trace_tbl -  stores trace during alignment
int giMeasurementInProgress = FALSE; // measure - measurement in progress
int giMaxNumMeasStored;         // ind_end - max num. measurements to store
int giBegDataPos;               // data_measure_i_beg - beg pos of data read
int giChuckVacSensorInputPos;   // CHUCK_V_SEN - should now be defined @ init
int giWATimer1, giWATimer2;     // new - to coordinate the checking out and returning
                                //      of the timer between the first and last step
                                //      of wafer alignment
int giCCDOutOfRange = FALSE;    // CCD_out_range

// FIXME - global replace gulAxis with gulChuckAxis
unsigned long gulAxis;          // the prealigner chuck axis
unsigned long gulAllAxes;       // all available prealigner axes
unsigned long gulBadCount = 0;  // bad_count
unsigned long gulDuplicateCount;// dup_count
long glaAngle[AL_MAX_INDEX+1];    // angle - comp. angles of data read
                                        //      - used by all
long glRoundBeforeFlat;         // angel_rd - round part before flat
                                //      - used by proc and math
long glCCDAnglePosition;        // CCD_ang - CCD angle position - used by all
long glMaxAngleMeasured;        // angel_max - max angle to be measured
                                //      - used by process and math
long glCCDOffset;               // CCDOffset - used by math only, but needed init
long glDistanceBetween2Meas;    // holding the value of max distance between 2 measurements depending on the mode

double gdCountInDeg;            // count_in_deg - number of encoder counts per 1
                                //      degree - shared by proc and math
double gdRndSeg;                // round_seg - size of angle for round part
                                //      - used by proc and math
double gdMainFlatMaxSize;       // main_fl_max - main flat max size
                                //      - used by proc and math
double gdMainFlatMinAngle;      // min_ang_prim_fl - min angle for primary
                                //      - shared by proc and math
double gdOffsetSizCor[AL_OFFSET_SZ_CORR];  // offset_size_cor - correction of
                                //      offset 1 radian - used by proc and math
                                //      for multi-chipped wafers
double gdCountInRadians;        // count_in_rad number - of encoder counts in
                                //      - used by proc and math
int giCCDFirstPixelPos;         // CCD_pos - CCD first pixel position, can
                                //     differ from wafer_par.CCDx_first_pixel
unsigned int *guipDataX;        // datax - shared by math and process
long *glpAngleX;                // anglex - shared by math and process
long glRobotTAxisOffset;        // pickup offset of robot T-axis (intellipick)
long glRobotRAxisOffset;        // pickup offset of robot R-axis (intellipick)
long glPreTAxisOffset;          // final robotation position of chuck (intellipick)

int ALInitialize(int iDefineTypeArg,         // Define Flags
                        int iEmulatorModeArg)       // Emulator Mode
{
    // initialize global variables
    giWaitForFlatMeasuring = 0;
    giFindFlatSubStep = 0;
    giAligning = FALSE;
    giWaitForFlatPositioning = FALSE;
    glCCDOffset = 689;

    ALEnableCCD();                      // enable ccd interrupt
    ALDetectPrealigner();
    ALSetPrealignerMode(giAlignerType); // initial mode defaults to aligner type

    giDefineType = iDefineTypeArg;
    giEmulatorMode = iEmulatorModeArg;

    // determine the chuck axis and index
    if( giDefineType & DFSAP4 )         // single axis prealigner
    {
        gulAxis = ROGetSpecialAxis(RO_SINGLE_PRE);
        gulAllAxes = gulAxis;
        switch( gulAxis )
        {
            case RO_AXIS_W :
                giAxisIndx = 3;
                giChuckVacSensorInputPos = AL_CHUCK_VAC_SEN_PREO;
                break;
            case RO_AXIS_t :
                giAxisIndx = 4;
                giChuckVacSensorInputPos = AL_CHUCK_VAC_SEN_PREO;
                break;
            default:
                return FAILURE;
        }
    }
    else
    {
        gulAllAxes = RO_AXIS_all;
        gulAxis = RO_AXIS_t; 
        giAxisIndx = 4;
        giChuckVacSensorInputPos = AL_CHUCK_VAC_SEN_PREO;
    }

    if( (giWATimer1=TIGetCounter()) == FAILURE )  // get global timer
        return FAILURE;
    if( (giWATimer2=TIGetCounter()) == FAILURE )  // get global timer
        return FAILURE;
    return SUCCESS;
}
