/**********************************************************************
 *
 *  Program:        Aligner module Local Functions and Declarations
 *  Function:
 *  Description:    Contains the declarations for functions which Global to the
 *                  wafer aligner module.
 *
 *  Modification history:
 *
 *  Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#ifndef _H_ALPRE_H
#define _H_ALPRE_H

// typedefs
typedef struct tgAlignResultTag         // wafer resolution structure, used while
{                                           //    computing wafer alignment information
    long m_lOffsetVector;                   // wafer_cal.off_amp
    long m_lOffsetAngle;                    // wafer_cal.off_ang
    long m_lWaferSize;                      // wafer_cal.waf_size
    int  m_iFlatBegIndexFromChuck;          // wafer_cal.flat_bind_c
    long m_lFlatBegAngleFromChuck;          // wafer_cal.flat_bang_c
    int  m_iFlatEndIndexFromChuck;          // wafer_cal.flat_eind_c
    long m_lFlatEndAngleFromChuck;          // wafer_cal.flat_eang_c
    int  m_iFlatBegIndexFromFlat;           // wafer_cal.flat_bind_f
    long m_lFlatBegAngleFromFlat;           // wafer_cal.flat_bang_f
    int  m_iFlatEndIndexFromFlat;           // wafer_cal.flat_eind_f
    long m_lFlatEndAngleFromFlat;           // wafer_cal.flat_eang_f
    long m_lFlatMiddle;                     // wafer_cal.flat_mid
} stAlignResult;


// global variables common to more than one part of the alignment module
extern stAlignResult gstWaferCal;   // wafer_cal - used by all

extern unsigned int guiaData[];     // data - data read from ccd during chuck rotation

extern int giDefineType;            // Define flags
extern int giEmulatorMode;          // Emulator Mode
extern int giAxisIndx;              // the index corresponding to the prealigner chuck
                                    // axis, used when pulling information from the
                                    // arrays returned by the data module primarily
extern int giChuckVacSensorInputPos; // CHUCK_V_SEN - should now be defined @ init
extern int giRndDataIdx;            // data_measure_i - begining index of data read
                                    //      - used by Math, Proc, and intr
extern int giStartDataIdx;          // start_data_i - position in read data for the round
                                    //      - used by math, proc, intr
extern int giRndLevel;              // round_level - shared by proc and math
extern int giMinFlatSize;           // flat_lev_fl - shared by proc and math
extern int giTrace;                 // trace - wafer alignment trace var
                                    //      - shared by proc and math
extern int giFlatIndexEndLastRead;  // flat_i_end - last read data for flat pos
                                    //      - shared by proc, math, intr
extern int giFindFlatSubStep;       // fff_substep - find flat function substep #
                                    //      - shared by proc and math
extern int giaDataStat[];           // datastat - status of data read - shared by all
extern int giAlignmentStatus;       // al_st - alignment status
                                    //      - used by process and math
extern int giCCDFirstPixelPos;      // CCD_pos - CCD first pixel position, can
                                    //      differ from wafer_par.CCDx_first_pixel
                                    //      shared by proc and math
extern int giPrevNumCorr;           // prev_nofcorr - used by process and get/set
extern int giNumCorr;               // nofcorr - used by process and get/set
extern int giAligning;              // aligning - used by process and get/set
extern int giOffsetIndexEnd;        // offset_i_end - last read data from CCD
                                    //      - used by proc and math
extern int giWAStep;                // step_bal - shared by proc and get/set
extern int giFlatIndexEndPrev;      // flat_i_end_old - used by process and get/set
extern int giDisplayMode;           // disp - display mode while aligning
                                    //      - used everywhere
extern int giWaitForFlatPositioning; // pfwaswaiting - shared by proc and math
extern int giWaitForFlatMeasuring;  // fms_waswaiting
extern int giDataReadPosDir;        // pos_mdir - positive direction of reading data
extern int giCCDNumber;             // ccd_no - active ccd number
extern int giaTraceTable[];         // trace_tbl -  stores trace when alignment
extern int giMeasurementInProgress; // measure - measurement in progress
extern int giMaxNumMeasStored;      // ind_end - max num. measurements to store
                                    //      - used by math and process
extern int giBegDataPos;            // data_measure_i_beg - beg pos of data read
extern int giAlignerType;           // iPreType
extern int giAlignerMode;           // iAlignMode
extern int giWATimer1, giWATimer2;  // new - to coordinate the checking out and returning
                                    //      of the timer between the first and last step
                                    //      of wafer alignment
extern int giCCDOutOfRange;         // CCD_out_range

extern unsigned long gulBadCount;   // bad_count
extern unsigned long gulDuplicateCount; // dup_count
extern unsigned long gulAxis;       // the prealigner chuck axis
extern unsigned long gulAllAxes;    // all available prealigner axes
extern long glaAngle[];             // angle - comp. angles of data read
                                    //      - used by all
extern long glRoundBeforeFlat;      // angel_rd - round part before flat
                                    //      - used by proc and math
extern long glCCDAnglePosition;     // CCD_ang - CCD angle position - used by all
extern long glMaxAngleMeasured;     // angel_max - max angle to be measured
                                    //      - used by process and math
extern long glCCDOffset;            // CCDOffset - used by math only but needed init
extern long glDistanceBetween2Meas; // holding the value of max distance between 2 measurements depending on the mode

extern double gdCountInDeg;         // count_in_deg - number of encoder counts per 1
                                    //      degree - used by proc and math
extern double gdRndSeg;             // round_seg - size of angle for round part
                                    //      - used by proc and math
extern double gdMainFlatMaxSize;    // main_fl_max - main flat max size
                                    //      - shared by proc and math
extern double gdMainFlatMinAngle;   // min_ang_prim_fl - min angle for primary
                                    //      - used by proc and math
extern double gdOffsetSizCor[];     // offset_size_cor - correction of offset by 1
                                    //      radian - used by proc and math for
                                    //      multi-chipped wafers
extern double gdCountInRadians;     // count_in_rad - number of encoder counts in
                                    //      - used by proc and math
extern double gdCountIn100th;       // count_in_100th
extern unsigned int *guipDataX;     // datax - shared by math and process
extern long *glpAngleX;             // anglex - shared by math and process
extern long glRobotTAxisOffset;     // pickup offset of robot T-axis (intellipick)
extern long glRobotRAxisOffset;     // pickup offset of robot R-axis (intellipick)
extern long glPreTAxisOffset;       // final robotation position of chuck (intellipick)

#endif
