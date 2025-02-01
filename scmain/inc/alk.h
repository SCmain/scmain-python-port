/**********************************************************************
 *
 *  Program:        Aligner Module defines
 *  Function:
 *  Description:    Contains the #define statements for the aligner
 *                  module.
 *
 *  Modification history:
 *
 *  Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#ifndef _H_ALK_H
#define _H_ALK_H

#define PIDEF 3.14159265359
#define THOUSANDDEG_TO_RAD ( PIDEF / 180000.0 )   // .001 degrees to 1 radian conversion
#define DEG_TO_RAD   ( PIDEF / 180.0 )      // 1 degree to 1 radian
#define RAD_TO_THOUSANDDEG ( 180000.0 / PIDEF )   // 1 radian to .001 degrees conversion
#define RAD_TO_DEG   ( 180.0 / PIDEF )      // 1 radian to 1 degree

#define AL_COUNT_360DEG       20000L // NO_OF_COUNTS

#define AL_DISPAR_A          1    // DISP_APAR, disp flat size, offset, and flat pos
#define AL_DISPAR_B          2    // DISP_BPAR, disp rest of flat info
#define AL_DISPAR_S       4    // DISP_STEP, disp current alignment step
#define AL_STATUS_FAIL        8    // New option bit to display align status when failed.
#define AL_DISPAR_Z       16    // New option bit to do only 420 deg rotation (as in ALZ).

#define AL_STANDARD 0           // standard aligner speed
#define AL_FASTPRE     1           // fast aligner speed

//#define AL_FASTMEASURE_DIS    3L // for fast mode, unit is in counts, the max distance between 2 measurements
#define AL_FASTMEASURE_DIS    10L // for fast mode, unit is in counts, the max distance between 2 measurements
#define AL_REGULARMEASURE_DIS     12L // for standard mode, unit is in counts, the max distance between 2 
//#define AL_REGULARMEASURE_DIS     3L // for standard mode, unit is in counts, the max distance between 2 measurements
#define AL_FINESCAN_DIS      2L // for fast mode, during fine-scan data acquisition

#define AL_CCD_MASK        0x0004       // CCD_MASK

// Aligner Status/Error Messages
#define AL_WAFER_NOT_ON_CHUCK       0x0001  // AS_CH_W
#define AL_MAX_RETRIES              0x0002  // AS_O_REP
#define AL_FLAT_NOT_FOUND           0x0004  // AS_N_FL
#define AL_OFFSET_NOT_FOUND         0x0008  // AS_N_OF
#define AL_SYSTEM_NOT_HOMED         0x0010  // AS_N_HOM
#define AL_SERVO_NOT_ON             0x0020  // AS_N_SV
#define AL_BAD_SPEED_ACCEL          0x0080
#define AL_MATH_ERROR               0x0100  // AS_MER_INT
#define AL_ALIGNMENT_REPEAT         0x0800  // AS_REP, alignment tried more than 1x
#define AL_BAD_SIZE_OR_TYPE         0x2000  // AS_SEL_ER

// status from reading data during alignment process
#define AL_DATA_STATUS_NOT_USED          0  // DATA_ST_NU - data not used in any calc
#define AL_DATA_STATUS_READ_OK           1  // DATA_ST_OK - reading ok
#define AL_DATA_STATUS_CHIP_EDGE         2  // DATA_ST_CP - data chip edge
#define AL_DATA_STATUS_ROUND_EDGE        4  // DATA_ST_RD - Round Edge wafer
#define AL_DATA_STATUS_FLAT_BEG_50       8  // DATA_ST_FB_H - flat begining ~ 50%
#define AL_DATA_STATUS_FLAT_BEG         16  // DATA_ST_FB - ~ flat beginning
#define AL_DATA_STATUS_FLAT_END_50      32  // DATA_ST_FE_H - flat end ~ 50%
#define AL_DATA_STATUS_FLAT_END         64  // DATA_ST_FE - ~ flat end
#define AL_DATA_STATUS_IN_MAIN_FLAT    128  // DATA_ST_FM - inside main flat
#define AL_DATA_STATUS_IN_2ND_FLAT     256  // DATA_ST_FS - inside secondary flat
#define AL_DATA_STATUS_IN_NOTCH        512  // DATA_ST_FN - inside notch
#define AL_DATA_STATUS_READ_ERROR     1024  // DATA_ST_ER - data reading error
#define AL_DATA_STATUS_IN_FLAT        2048  // DATA_ST_FL - data in flat part

// data used during computations
#define AL_POINT_STEP           200L    // PT_STEP
#define AL_POINT_INC              4L    // PT_INC
#define AL_MAX_CIRCLE_CALCS     100L    // CIR_MAX_NO - max no. of circle calcs
#define AL_CAL_SCALER            10L    // CAL_SK - scale factor bet robot/aligner
#define AL_XY_PIXEL_SCALER     1000L    // XY_PIX_SK_STO - scale factor
#define AL_PRIMARY_FLAT_POINTS    5L    // PR_FLAT - # pts to signed pr. flat
#define AL_PRIMARY_ROUND_POINTS   2L    // PR_RD - # pts to signed round
#define AL_FLAT_ROUND_DISTANCE   10L    // FLAT_RD_DIS - # pts bet round/ckd flat
#define AL_REQ_PTS_BEF_FLAT      10L    // RD_BEF_FL - # pts before flat required
#define AL_OFFSET_SZ_CORR         6L    // OFF_SC - # of offset size corr
#define AL_FILTER_END           100L    // FILTER_END -struct size for edge stat data
                                        //          (multi-chipped)
#define AL_EDGE_FILTER_END      100L    // EDGE_FILTER_END - struct size for edge
                                        //          statistical data
#define AL_PIXEL_SPACING         .0107  // PIXELS_SP - Pixel spacing, non-chipped waf

// supported wafer sizes
#define AL_WAFER_SIZE_3          3      // 3" or 76.2mm
#define AL_WAFER_SIZE_76        76      // 3" or 76.2mm
#define AL_WAFER_SIZE_80        80      // 80 mm
#define AL_WAFER_SIZE_90        90      // 90mm
#define AL_WAFER_SIZE_4          4      // 4" or 100mm
#define AL_WAFER_SIZE_100      100      // 4" or 100mm
#define AL_WAFER_SIZE_5          5      // 5" or 125mm
#define AL_WAFER_SIZE_125      125      // 5" or 125mm
#define AL_WAFER_SIZE_6          6      // 6" or 150mm
#define AL_WAFER_SIZE_150      150      // 6" OR 150mm
#define AL_WAFER_SIZE_8          8      // 8" or 200mm
#define AL_WAFER_SIZE_200      200      // 8" or 200mm
#define AL_WAFER_SIZE_12        12      // 12" or 300mm
#define AL_WAFER_SIZE_300      300      // 12" or 300mm

// primary flat angle minimum size for the specified wafer size
#define AL_PRIM_FLAT_3          25.4    // 3" (76.2mm) - non-chipped wafer, 80mm too
#define AL_PRIM_FLAT_100        29.3    // 100mm - non-chipped wafer
#define AL_PRIM_FLAT_125        32.6    // 125mm - non-chipped wafer
#define AL_PRIM_FLAT_150        33.0    // 150mm - non-chipped wafer
#define AL_PRIM_FLAT_200        24.0    // 200mm - non-chipped wafer

#define AL_PRIM_FLAT_3_MC       25.4    // 3" (76.2mm) - multi-chipped wafer, 80mm too
#define AL_PRIM_FLAT_100_MC     29.3    // 100mm - multi-chipped wafer
#define AL_PRIM_FLAT_125_MC     32.6    // 125mm - multi-chipped wafer
#define AL_PRIM_FLAT_150_MC     33.0    // 150mm - multi-chipped wafer
#define AL_PRIM_FLAT_200_MC     24.0    // 200mm - multi-chipped wafer

#define AL_MAIN_FLAT_3          40.0    // 3" (76.2mm) non-chipped wafer
#define AL_MAIN_FLAT_100        45.0    // 100mm non-chipped wafer
#define AL_MAIN_FLAT_125        45.0    // 125mm non-chipped wafer
#define AL_MAIN_FLAT_150        50.0    // 150mm non-chipped wafer
#define AL_MAIN_FLAT_200        40.0    // 200mm non-chipped wafer

#define AL_ROUND_LEVEL_3         6      // 10 * # pixels for 3" non-chipped wafer
#define AL_ROUND_LEVEL_80        8      // 10 * # pixels for 80mm non-chipped wafer
#define AL_ROUND_LEVEL_90        8      // 10 * # pixels for 90mm non-chipped wafer
#define AL_ROUND_LEVEL_100       8      // 10 * # pixels for 100mm non-chipped wafer
#define AL_ROUND_LEVEL_125       8      // 10 * # pixels for 125mm non-chipped wafer
#define AL_ROUND_LEVEL_150       6      // 10 * # pixels for 150mm non-chipped wafer
#define AL_ROUND_LEVEL_200       6      // 10 * # pixels for 200mm non-chipped wafer

#define AL_MAX_OFF_VEC_3        3000L   // max offset vector in mm, 1 correction
#define AL_MAX_OFF_VEC_80       3000L
#define AL_MAX_OFF_VEC_90       4000L
#define AL_MAX_OFF_VEC_100      4000L
#define AL_MAX_OFF_VEC_125      4000L
#define AL_MAX_OFF_VEC_150      4000L
#define AL_MAX_OFF_VEC_200      5000L

#define AL_OFFSET_SIZE_COR_3    1.0000   // correction of center offset
#define AL_OFFSET_SIZE_COR_80   1.0000
#define AL_OFFSET_SIZE_COR_90   1.0000
#define AL_OFFSET_SIZE_COR_100  1.0300
#define AL_OFFSET_SIZE_COR_125  0.9000
#define AL_OFFSET_SIZE_COR_150  0.9852
#define AL_OFFSET_SIZE_COR_200  0.9346

#define AL_MIN_FL_LEVEL_DEC_3   3000    // minimum level of data decrease for the
#define AL_MIN_FL_LEVEL_DEC_80  3000    //      flat, 1000 is 1 CCD pixel
#define AL_MIN_FL_LEVEL_DEC_90  3000    // FLAT_LEV_FL_XXXX
#define AL_MIN_FL_LEVEL_DEC_100 4000
#define AL_MIN_FL_LEVEL_DEC_125 5000
#define AL_MIN_FL_LEVEL_DEC_150 6000
#define AL_MIN_FL_LEVEL_DEC_200 6000

#define AL_EDGE_SEG_3           1.5     // minimum flat segment in mm
#define AL_EDGE_SEG_100         2.0
#define AL_EDGE_SEG_125         2.5
#define AL_EDGE_SEG_150         3.0
#define AL_EDGE_SEG_200         4.0
#define AL_EDGE_SEG_300         5.0

// data used by shared arrays during alignment process or to define the arrays
#define AL_MAX_INDEX                 3000   // INDEX_END - max index
#define AL_OFFCENTER_END_INDEX       2000   // IND_C_END - offcenter meas ending indx
#define AL_FLAT_BEGINING_INDEX       2500   // IND_FB_END - flat beg meas ending indx

#define AL_SKIP_END                     50  // END_SKIP - end of data, skip the rest
#define AL_BEGIN_DATA                  100  // D_BEG - beginning index of good data
#define AL_IGNORE_BEGIN_DATA            10  // D_IGNORE_BEG - ignore data past this

#define AL_SHORT_MEAS_BY_COUNT          10  // SHORT_MEAS - short meas by # of counts
#define AL_CENTER_FIND_ROT         420000L  // OFFSET_MES_LN - amount of chuck
                                            //   rotation needed by alignment process
#define AL_FLAT_FIND_ROT             6500L  // FLAT_MES_LN - amount of chuck rotation
                                            //   used to compute flat location
#define AL_ROUND_BEFORE_FLAT          500L  // RD_PART - Round part before big flat
#define AL_MAX_CORRECTIONS              2L  // MAX_NO_OF_COR
#define AL_MAX_OFFSET                5200L  // MAX_OFFSET - max corr in mm counts

#define AL_TRACE_TABLE_SIZE             10  // size of the trace table

// IO Ports Re-defines <-> scio.h MUST MATCH WITH GROUP 5
#define AL_CCD_BASE_ADR     0x0310                // FF_ADR
#define AL_CCD_COMMAND_ADR  (AL_CCD_BASE_ADR + 2) // FF_COMAND
#define AL_CCD_DATA_ADR     (AL_CCD_BASE_ADR )    // FF_DATA
#define AL_CCD_PORTC        (AL_CCD_BASE_ADR + 1) // PORT C to check the interrupt from Prealginer FPGA
#define AL_DIO_CONTROL		(AL_CCD_BASE_ADR + 3)   // port D control ports setting
#define AL_DIO_CONTROL_MODE    0x92    // A(IN) B(IN) C(OUT)

#define AL_CCD_MASK_NUM        0x0005  // CCD_NO_MASK
#define AL_CCD_0               0x0000  // CCD_0
#define AL_CCD_1               0x0001  // CCD_1
#define AL_CCD_2               0x0004  // CCD_2
#define AL_CCD_3               0x0005  // CCD_3
#define AL_CCD_MIN_READING        280  // CCD_MIN - Min reading value from CCD
//#define AL_CCD_MAX_READING       2800  // CCD_MAX - Max reading value from CCD
#define AL_CCD_MAX_READING       2660  // CCD_MAX - Max reading value from CCD
#define AL_CCD_DATA_SEL_MASK   0x0080  // DATA_SEL_MASK
#define AL_CCD_DATA            0x0080  // CCD_DATA
#define AL_CCD_DATA_MASK       0x0fff  // CCD_DATA_MASK
#define AL_CCD_CHUCK_DATA      0x0000  // CHUCK_DATA
#define AL_CCD_CHUCK_MASK      0x7fff  // CHUCK_MASK
#define AL_CCD_CHUCK_B15_MASK  0x2000  // CHUCK_B15_MASK
#define AL_CCD_CHUCK_B15_SHIFT      2  // CHUCK_B15_SHIFT
#define AL_CCD_RESET_MASK      0x0060  // RESET_MASK
#define AL_CCD_PREV_RESET_MASK 0x4000  // X_WAS_RES_MASK
#define AL_CCD_RESET_ON        0x0040  // RESET_ON
#define AL_CCD_RESET_OFF       0x0020  // RESET_OFF
#define AL_CCD_ERROR_MASK      0x1000  // ERROR_MASK
#define AL_CCD_REVERSE_MODE      2500  // Used to invert vacuum prealigner data when needed
/*
 *  WA initialization defines - used in conjunction with the parameters passed to
 *                              ALInitialize
 */
#define AL_CHUCK_VAC_SEN_SAP4   0x0008
#define AL_CHUCK_VAC_SEN_PREO   0x0001

#define AL_BEG_FLAT_ADJ_PRO     1800L

#define AL_DISABLE 0       // for enable and disable CCD_interrupt
#define AL_ENABLE 1
#define AL_CLEAR_INTERRUPT  2

#define AL_SP_MEASURE_CENTER				1
#define AL_SP_OPERATIONAL					2
#define AL_SP_MEASURE_FLAT					3

// Equates for GET/SET functions
#define AL_NOIDX                   -1
#define AL_TRACE                    0   // giTrace
#define AL_OFFSET_INDEX_END         1   // giOffsetIndexEnd
#define AL_FLAT_INDEX_LAST_READ_END 2   // giFlatIndexEndLatRead
#define AL_FLAT_INDEX_END_PREV      3   // giFlatIndexEndPrev
#define AL_CCD_FIRST_PIXEL_POS      4   // giCCDFirstPixelPos
#define AL_DISPLAY_MODE             5   // giDisplayMode
#define AL_ROUND_DATA_INDEX         6   // giRndDataIdx
#define AL_ALIGNMENT_STATUS         7   // giAlignmentStatus
#define AL_NUM_CORRECTIONS          8   // giNumCorr
#define AL_DATA_COLLECTION          9   // giMeasurementInProgress
#define AL_ALIGNING_FLAG           10   // giAligning
#define AL_ALIGNMENT_STEP          11   // giWAStep
#define AL_TRACE_TABLE             12   // giaTraceTable[]
#define AL_DATA_DIFF               13   // giaDataDiff[]
#define AL_STAT                    14   // giaDataStat[]
#define AL_DATA_ANGLE              15   // glaAngle[]
#define AL_OFFSET_VECTOR           16   // gstWaferCal.m_lOffsetVector
#define AL_OFFSET_ANGLE            17   // gstWaferCal.m_lOffsetAngle
#define AL_FLAT_MIDPOINT           18   // gstWaferCal.m_lFlatMiddle
#define AL_PREALIGNER_MODE         19   // giAlignerMode
#define AL_WAFER_SIZE              20   // gstWaferCal.m_lWaferSize
#define AL_ALL_PRE_AXES            21   // gulAllAxis
#define AL_CHUCK_AXIS			   22   // gulAxis
#define AL_PREALIGNER_TYPE         23   // giAlignerType
#define AL_ROBOT_T_PICKUP_POS      24   // glRobotTAxisOffset
#define AL_ROBOT_R_PICKUP_POS      25   // glRobotRAxisOffset
#define AL_CHUCK_FINAL_ROTATE_POS  26   // glPreTAxisOffset
#define AL_CHUCK_VAC_SENSOR        27   // giChuckVacSensorInputPos

#endif
