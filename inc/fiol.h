/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        datafaln.h
 * Functions:   FIOInitWaferParam
 *              FIOReadWafer
 *              FIOWriteWafer
 *              FIOReadCalibTable
 *              FIOWriteCalibTable
 *              FIOEnterCalibTableItem
 *              FIOGetCalibTblItem
 *              FIORemoveCTNode
 *              FIOGetCTNext
 *              FIOGetItemFromCalTbl
 *              FIOSetItemToCalTbl
 *              FIOGetCalibSerNum
 *              FIOSetCalibSerNum
 *
 *              FIOGetParamWaferXXXXXXX() - Provide an interface to get parameters
 *                                  from the wafer parameter file parameters
 *
 *              FIOPutParamWaferXXXXXXX() - Provide an interface to set parameters
 *                                  to the wafer parameter file parameters
 *
 * Description: Provide an interface between the aligner datafile structures and
 *              all other modules.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_FIOALN_H
#define _H_FIOALN_H


/********** DEFINES **********/

/* Maximum number of items in the wafer calibration table. */
#define MAXCTITEMS          20

/* Array elements in wafer calibration table. */
#define CT_WSIZE            0       /* name - wafer size */
#define CT_WTYPE            1       /* name - wafer type */
#define CT_CCD_NO           2       /* use of ccd no1. for 76-125 */
#define CT_REP_NO           3       /* number of reps from beg. */
#define CT_MAX_OFF          4       /* max. offset for one center meas. */
#define CT_ROB_CHUCK   		5       /* distance between center of robot and chuck */
#define CT_CCD_ROB_ANG 		6       /* angle between robot and CCD sensor */
//#define CT_RESERVED         7       /* reserved for future use */
#define CT_CCD_POS_COR      8       /* correct ccd position for waf. dia. */
#define CT_CCD_AG_COR       9       /* correct angular ccd pos. */
#define CT_OFFSET_COR_0     10      /* offsest meas. correction */
#define CT_OFFSET_COR_1     11      /* offsest meas. correction */
#define CT_OFFSET_COR_2     12      /* offsest meas. correction */
#define CT_OFFSET_COR_3     13      /* offsest meas. correction */
#define CT_OFFSET_COR_4     14      /* offsest meas. correction */
#define CT_OFFSET_COR_5     15      /* offsest meas. correction */
//#define CT_RESERVED         16      /* reserved for future use */
//#define CT_RESERVED         17      /* reserved for future use */
//#define CT_RESERVED         18      /* reserved for future use */
//#define CT_RESERVED         19      /* reserved for future use */

/* Maximum number of items in the wafer parameter table. */
#define MAX_WAFER_PARAM_ITEMS  43

/* Elements of the wafer parameter structure. They are used to generically get and set
 * values in the wafer parameter structure. */
#define MEASURE_SPEED_CENTER    0   /* meas_spd_ctr         3 ??? */
#define MEASURE_ACCEL_CENTER    1   /* meas_accel_ctr       4 ??? */
#define MEASURE_SPEED_FLAT      2   /* meas_spd_flt         5 ??? */
#define MEASURE_ACCEL_FLAT      3   /* meas_accel_flt       6 ??? */
#define CHUCK_DEAD_BAND         4   /* chuck_dead_band      7 ??? */
#define MEASURE_SPEED_CNTRF200  5   /* was dummy[index 0]   8 ??? */
#define MEASURE_SPEED_CNTRF300  6   /* was dummy[index 1]   9 ??? */
#define REVERSE_MODE            7   /* was dummy1           10 ??? */
#define FLAT_ANGLE              8   /* flat_angle           22 ??? */
#define MEASURE_TYPE            9   /* meas_type            23 ??? */
#define WAFER_TYPE              10  /* wafer_type           24 ??? */
#define WAFER_LOAD_POSITION     11  /* wafer_load_pos       25 ??? */
#define WAFER_LIFT_UP_POSITION  12  /* wafer_liftup_pos     26 ??? */
#define MEASURE_SPEED_FLAT_F200 13  /* was dummy2[index 0]  27 ??? */
#define MEASURE_SPEED_FLAT_F300 14  /* was dummy2[index 1]  28 ??? */
#define DUMMY2A                 15  /* dummy2[index 0]      29 ??? */
#define DUMMY2B                 16  /* dummy2[index 1]      30 ??? */
#define DUMMY2C                 17  /* dummy2[index 2]      31 ??? */
#define DUMMY2D                 18  /* dummy2[index 3]      32 ??? */
#define DUMMY2E                 19  /* dummy2[index 4]      33 ??? */
#define DUMMY2F                 20  /* dummy2[index 5]      34 ??? */
#define DUMMY2G                 21  /* dummy2[index 6]      35 ??? */
#define DUMMY2H                 22  /* dummy2[index 7]      36 ??? */
#define DUMMY2I                 23  /* dummy2[index 8]      37 ??? */
#define CCD1_FIRST_PIXEL        24  /* CCD1_first_pixel     38 ??? */
#define CCD1_ANGLE              25  /* CCD1_angle           39 ??? */
#define CHUCK_VAC_USE           26  /* was dummy3           40 ??? */
#define CCD2_FIRST_PIXEL        27  /* CCD2_first_pixel     41 ??? */
#define CCD2_ANGLE              28  /* CCD2_angle           42 ??? */
#define DUMMY4A                 29  /* dummy4[index 0]      43 ??? */
#define DUMMY4B                 30  /* dummy4[index 1]      44 ??? */
#define DUMMY4C                 31  /* dummy4[index 2]      45 ??? */
#define DUMMY4D                 32  /* dummy4[index 3]      46 ??? */
#define WAFER_SIZE              33  /* wafer_size           47 ??? */
#define FLAT_TYPE               34  /* flat_type            48 ??? */
#define WAFER_CAL_TYPE          35  /* wafer_caltype        49 ??? */
#define DUMMY5A                 36  /* dummy5[index 0]      50 ??? */
#define DUMMY5B                 37  /* dummy5[index 1]      51 ??? */
#define DUMMY5C                 38  /* dummy5[index 2]      52 ??? */
#define DUMMY5D                 39  /* dummy5[index 3]      53 ??? */
#define DUMMY5E                 40  /* dummy5[index 4]      54 ??? */
#define DUMMY5F                 41  /* dummy5[index 5]      55 ??? */
#define DUMMY5G                 42  /* dummy6[index 6]      56 ??? */

/* Calibration file additions for dual arm intellipick. */
#define T_NEG_MAX_OFFSET        50  /* NEW */
#define T_POS_MAX_OFFSET        51  /* NEW */
#define R_NEG_MAX_OFFSET        52  /* NEW */
#define R_POS_MAX_OFFSET        53  /* NEW */


/********** FUNCTION PROTOTYES *********/

/* Functions for manipulation of entire wafer parameter structure at once. */
void FIOInitWaferParam();                    /* NEW Function */
int FIOReadWafer();                          /* Replaces: read_wafer */
int FIOWriteWafer();                         /* Replaces: write_wafer */

/* Functions for manipulation of entire wafer calibration parameter table,
 * a linked list of wafer calibration parameter structures, at once. */
int FIOReadCalibTable();                     /* Replaces: read_caltbl */
int FIOWriteCalibTable();                    /* Replaces: write_caltbl */
int FIOEnterCalibTableItem(long lWaferSizeArg, long lTypeArg, long lItemNoArg, long lDataArg);   /* Replaces: enter_ctitem */
long FIOGetCalibTblItem(long lWaferSizeArg, long lTypeArg, long lItemNoArg); /* Replaces: get_ctitem */
int FIORemoveCTNode(long lWaferSizeArg, long lTypeArg);                      /* Replaces: remove_ctnode */
int FIOGetCTNext(long *lWaferSizeArg, long *lTypeArg);                       /* Replaces: get_ctnext */
long FIOGetItemFromCalTbl(int iItemNoArg);                                   /* Replaces: get_ct_it */
int FIOSetItemToCalTbl(int iItemNoArg, long lItemValue);                     /* Replaces: set_ct_it */
int FIOGetCalibSerNum(char *pcaCalibSerNumArg);
int FIOSetCalibSerNum(char *pcaCalibSerNumArg);


/********** NEW FUNCTIONS *************
 * These functions get and set variable values
 ******************************************/

/* GET variable values from the WAFER structure. */
int FIOGetParamWaferSerialNum(char *pcaWaferSerialNum);          /* wafer_par_st.SN[21] */
int FIOGetParamWaferVals(int iValueDescArg, long *plValueArg);
int FIOGetParamWaferSpeed(long *plSpeed);                        /* wafer_spd_T, wafer_spd_R, wafer_spd_Z */
int FIOGetParamWaferAccel(long *plAccel);                        /* wafer_accel_T, wafer_accel_R, wafer_accel_Z */
int FIOGetParamNoWaferSpeed(long *plSpeed);                      /* no_wafer_spd_T, no_wafer_spd_R, no_wafer_spd_Z */
int FIOGetParamNoWaferAccel(long *plAccel);                      /* no_wafer_accel_T, no_wafer_accel_R, no_wafer_accel_Z */
int FIOGetTROffset(int iValueDescArg, long *lValueArg, long lWhichArmArg);

/* SET variable values to the WAFER structure. */
int FIOSetParamWaferSerialNum(char *pcaWaferSerialNum);          /* wafer_par_st.SN[21] */
int FIOSetParamWaferVals(int iValueDescArg, long lValueArg);
int FIOSetParamWaferSpeed(long *plSpeed);                        /* wafer_spd_T, wafer_spd_R, wafer_spd_Z */
int FIOSetParamWaferAccel(long *plAccel);                        /* wafer_accel_T, wafer_accel_R, wafer_accel_Z */
int FIOSetParamNoWaferSpeed(long *plSpeed);                      /* no_wafer_spd_T, no_wafer_spd_R, no_wafer_spd_Z */
int FIOSetParamNoWaferAccel(long *plAccel);                      /* no_wafer_accel_T, no_wafer_accel_R, no_wafer_accel_Z */
int FIOSetTROffset(int iValueDescArg, long lValueArg, long lWhichArmArg);

/* Revision control functions */
int FIOGetCalibFileRevision(float *fRevision);
int FIOSetCalibFileRevision(float fRevision);
int FIOGetWaferFileRevision(float *fRevision);
int FIOSetWaferFileRevision(float fRevision);

#endif
