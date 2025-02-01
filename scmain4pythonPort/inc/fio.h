/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        fio.h
 * Functions:   FIOValidateParaDesc
 *              FIOValidateStationIndex
 *              FIOGetParamFileType
 *              FIOErrHandler
 *              FIOComputeChkSum
 *              FIONoBDrive
 *              FIOTestBlockChkSum
 *              FIOGetParam
 *              FIOSetParam
 *              FIOSearchCalibTableNode
 *              FIOWriteBytes
 *
 * Description: Structures, defines, and functions used locally
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: The ??? in the comments indicates that the variable
 *      was not used at the time of commenting so it's
 *      function was not understood enough to write a comment.
 *      They must be fixed as they come into use and are debugged.
 *
 * NOTE: CSI stands for Coordinate Special Item. These items are used
 *      for special values that are intended for use by the user
 *      and are recorded in the coordinate file during a save so they
 *      can be recalled and used again at a later date. Some of them
 *      have come to have standard meanings. For example, CSI 8
 *      is now typically used for the cross-slot threshold.
 *
\***************************************************************/

#ifndef _H_FIO_H
#define _H_FIO_H

#include <stdio.h>
#include "fiog.h"
#include "fiol.h"

/* Special Flags absorbed in this module: A8 and GTO. */

/********** DEFINES **********/

/* The maximum number of stations. They are indexed to the user as A-Z and a-z. */
#define FIO_MAX_NUM_STATIONS 52


/* file name constants */
#define PARFILENAME     "/root/controller/scmain/pars"
#define CORFILENAME     "/root/controller/scmain/coords"
#define MACFILENAME     "/root/controller/scmain/macro.0"
#define MACWILDCARD     "/root/controller/scmain/macro.%ld"
#define WAFFILENAME     "/root/controller/scmain/wafer"
#define CALFILENAME     "/root/controller/scmain/caltbl"
#define CFGFILENAME     "/root/controller/scmain/cfgtbl"
#define GM4FILENAME     "/root/controller/scmain/gmac4axis"
#define GM8FILENAME     "/root/controller/scmain/gmac8axis"
#define LATCHCLNAME     "/root/controller/scmain/latcl"
#define LATCHDTNAME     "/root/controller/scmain/latdt"
#define LATCHMANAME     "/root/controller/scmain/latma"
#define LATCHMSNAME     "/root/controller/scmain/latms"
#define RCFILENAME      "/root/controller/scmain/rcdata"
#define SIOFILENAME	"/root/controller/scmain/siodata"
#define VECTORFNAME 	"/root/controller/scmain/vecparm"


/********** VARIABLES USED LOCALLY **********/

                                /* Variable names in the old code - Column 1
                                 *              Variable description - Column 2*/
typedef long aPosition[3];      /* position_t[3] */

/* Robot, prealigner and auxiliary parameter structure.
 * NOTE: The ordering of members is important!!! That's the order they are
 * recorded and read to and from the parameter file, respectively. */
typedef struct CtrlParamTag     /* ctrl_par_st  Structure to hold parameter values. */
{
    char    m_cSN[21];          /* SN[21]       File serial number, e.g. ATM-3400. */
    int     m_iBaudRate;        /* baud_rate    Global baud rate for all comm ports, NOT USED. */
    long    m_laHomeSpeed[3];   /* hm_speed[3]  Homing speed scaled to human units, i.e. inches/sec. */
    long    m_laHomeAccel[3];   /* hm_accel[3]  Homing acceleration scaled to human units, i.e. inches/sec^2. */
    long    m_laEncdRes[3];     /* enc_res[3]   Encoder resolution, for finding proper Home-Sw-to-Index distance,
                                 *              used to be reserved 1. */
    long    m_laLMAccel[3];     /* lm_accel[3]  ???, used to be reserved 2. */
    long    m_laOperaSpeed[3];  /* op_speed[3]  Operating speed scaled to human units, i.e. inches/sec. */
    long    m_laOperaAccel[3];  /* op_accel[3]  Operating acceleration scaled to human units, i.e. inches/sec^2. */
    long    m_laER[3];          /* ER[3]        Error limit. */
    long    m_laKP[3];          /* Was GN[3]    Proportional gain, part of PID. */
    long    m_laFA[3];          /* FA[3]        Feed-forward acceleration, additional PID tuning parameter. */
    long    m_laKI[3];          /* KI[3]        Integral gain, part of PID. */
    long    m_laTL[3];          /* TL[3]        Torque Limit. */
    long    m_laKD[3];          /* Was ZR[3]    Derivative gain, part of PID. */
    long    m_laIT[3];          /* IT[3]        Ramp, a.k.a. jerk, rate of change of acceleration. */
    long    m_laCHome[3];       /* C_home[3]    Customized home position. */
    long    m_laHomeOffset[3];  /* home_offset[3]   Distance from home switch to actual home position, in encoder units. */
    long    m_laMR[3];          /* MR[3]        Mechanical ratio, the gearing ratio. */
    long    m_laARM[3];         /* ARM[3]       Arm length. */
    long    m_laH[3];           /* H[3];        ???, used to be reserved 3. */
    long    m_laDC[3];          /* DC[3]        Operating deceleration, scaled to human units, i.e. inches/sec^2. */
    long    m_laSoftNegLimit[3];/* softnlim[3]  Software negative limit. */
    long    m_laSoftPosLimit[3];/* softplim[3]  Software positive limit. */
    long    m_laScanOffset[3];  /* scan_offset[3]   Mapper offset. */
    long    m_laSpeedLimit[3];  /* spd_limit[3] ??? */
    float   m_fRevision;        /* NEW in CM Build 17   File Revision */
} stCtrlParam, *pstCtrlParam;   /* ctrl_par_t   Structure to hold parameter values. */

/* Wafer parameter structure.
 * NOTE: The ordering of members is important!!! That's the order they are
 * recorded and read to and from the wafer parameter file, respectively. */
typedef struct WaferParamTag        /* wafer_par_st         ??? */
{
    char   m_cSN[21];               /* SN[21]               1 ??? */
    long   m_lMeasureSpeedCount;    /* meas_spd_ctr         2 ??? */
    long   m_lMeasureAccelCount;    /* meas_accel_ctr       3 ??? */
    long   m_lMeasureSpeedFlt;      /* meas_spd_flt         4 ??? */
    long   m_lMeasureAccelFlt;      /* meas_accel_flt       5 ??? */
    long   m_lChuckDeadBand;        /* chuck_dead_band      6 ??? */
    long   m_lMeasureSpeedCntrF200; /* dummy[index 0]       7 ??? */
    long   m_lMeasureSpeedCntrF300; /* dummy[index 1]       8 ??? */
    long   m_lReverseMode;          /* dummy[index 2]       9 ??? */
    long   m_lWaferSpeed[3];        /* wafer_spd_T          10 ???
                                     * wafer_spd_R          11 ???
                                     * wafer_spd_Z          12 ??? */
    long   m_lWaferAccel[3];        /* wafer_accel_T        13 ???
                                     * wafer_accel_R        14 ???
                                     * wafer_accel_Z        15 ??? */
    long   m_lNoWaferSpeed[3];      /* no_wafer_spd_T       16 ???
                                     * no_wafer_spd_R       17 ???
                                     * no_wafer_spd_Z       18 ??? */
    long   m_lNoWaferAccel[3];      /* no_wafer_accel_T     19 ???
                                     * no_wafer_accel_R     20 ???
                                     * no_wafer_accel_Z     21 ??? */
    long   m_lFlatAngle;            /* flat_angle           22 ??? */
    long   m_lMeasureType;          /* meas_type            23 ??? */
    long   m_lWaferType;            /* wafer_type           24 ??? */
    long   m_lWaferLoadPosition;    /* wafer_load_pos       25 ??? */
    long   m_lWaferLiftUpPosition;  /* wafer_liftup_pos     26 ??? */
    long   m_lMeasureSpeedFlatF200; /* dummy2[index 0]      27 ??? */
    long   m_lMeasureSpeedFlatF300; /* dummy2[index 1]      28 ??? */
    long   m_lDummy2[9];            /* was dummy2[11]       was 27,28,29,30,31,32,33,34,35,36,37 ??? */
    long   m_lCCD1FirstPixel;       /* CCD1_first_pixel     38 ??? */
    long   m_lCCD1Angle;            /* CCD1_angle           39 ??? */
    long   m_lChuckVacUse;       	/* was dummy3           40 ??? */
    long   m_lCCD2FirstPixel;       /* CCD2_first_pixel     41 ??? */
    long   m_lCCD2Angle;            /* CCD2_angle           42 ??? */
    long   m_lDummy4[4];            /* dummy4[4]            43,44,45,46 ??? */
    long   m_lWaferSize;            /* wafer_size           47 ??? */
    long   m_lFlatType;             /* flat_type            48 ??? */
    long   m_lWaferCalType;         /* wafer_caltype        49 ??? */
    long   m_lDummy5[7];            /* dummy5[7]            50,51,52,53,54,55,56 ??? */
    float  m_fRevision;             /* NEW in CM Build 17   File Revision */
} stWaferParam, *pstWaferParam;     /* wafer_par_t          ??? */

/* Station coordinate structure.
 * NOTE: The ordering of members is important!!! That's the order they are
 * recorded and read to and from the coordinate file, respectively. */
typedef struct StationDataTag   /* stn_data     Structure to hold station coordinate values. */
{
    aPosition  m_alCoord;       /* coord        Station coordinate, standard units are 0.01 deg and 0.001 inches. */
    long    m_lFprCoord;        /* coord        Flipper station coordinate, standard units are 0.01 deg and 0.001 inches. */
    long    m_lSafeR;           /* coord        Safe R-Axis Coordinate when using Flipper. */
    long    m_lOffset;          /* offset       Distance above the wafer position to move during a stroke. */
    long    m_lZIndex;          /* z_index      Slot number in the cassette, used with MTCR and MATS. */
    long    m_lStroke;          /* stroke       Z distance to move during a STROK command. */
    long    m_lStartAngle;      /* start_angle  Starting angle relative to coordinate, CSI 0, NOT USED. */
    long    m_lEndAngle;        /* end_angle    Ending angle relative to coordinate, CSI 1. */
    long    m_lCSLow;           /* CSLow        Cross-slot/double-slot low threshold, CSI 2. */
    long    m_lCSHigh;          /* CSHigh       Cross-slot/double-slot high threshold, CSI 3. */
    long    m_lAccLow;          /* AccLow       ???, maybe set by TSCN, CSI 4. */
    long    m_lAccHigh;         /* AccHigh      ???, maybe set by TSCN, CSI 5. */
    long    m_lAveThick;        /* ave_thick    Average thickness of wafer, also affected by slanted cassette, CSI 6. */
    long    m_lPosToWaf;        /* NEW          Distance from taught position to actual position of first wafer. */
    long    m_lCSThresh;        /* NEW          Cross slot threshold. */
    long    m_lTrackPos;        /* NEW          Track position. */
    long    m_alLscsi[FIO_MAX_NUM_CSI-10];   /* lscsi[25]    Usually used as lscsi[10], open for user, CSI 10-15. */
    aPosition   m_alScanCoord;  /* scan_coord   Station coordinate specific to scanning. */
    long    m_lRHome;           /* R_home       Retracted home position for radial axis.
                                 *              This is typically the customized home but can be
                                 *              different for different stations. */
    long    m_lPitch;           /* pitch        The distance between slots in a cassette. It's units are
                                 *              ten-thousandths of an inch or microns. This is not standardized
                                 *              and still a point of controversy. */
    int     m_iEEindex;         /* ee_indx      End effector index, 0 for blade A, 1 for blade B. */
} stStationData, *pstStationData;   /* stn_data_t, *stn_data_ptr
                                     *          Structure and pointer-to-structure to hold station coordinate values. */

/* Calibration table structure.
 * NOTE: The ordering of members is important!!! That's the order they are
 * recorded and read to and from the calibration file, respectively. */
typedef struct CalibrationTableTag  /* cal_table_st Structure to get and set wafer calibration parameters. */
{
    long    m_lWaferSize;           /* waf_size     Wafer size in mm, i.e. 200 mm diameter wafer. */
    long    m_lType;                /* type         ??? */
    long    m_alItemArr[MAXCTITEMS];/* item_arr     ??? */
    struct  CalibrationTableTag  *mNextPtr;    /* *next_ptr
                                                     *  For linked list of wafer calibration parameters. */
} stCalTable,  *psCalTable;    /* cal_table_t, far *cal_table_p
                                     *              Structure and pointer-to-structure to hold
                                     *              wafer calibration parameters. */

/* structure to hold the offset between the robot end-effector and chuck to avoid of hitting. */
typedef struct TROffsetTag
{
	long   m_lTNegMaxOffset;        /* T-axis negative maximum offset to avoid robot hitting chuck (intellipick)  43 */
	long   m_lTPosMaxOffset;        /* T-axis positive maximum offset to avoid robot hitting chuck (intellipick)  44 */
	long   m_lRNegMaxOffset;        /* R-axis negative maximum offset to avoid robot hitting chuck (intellipick)  45 */
	long   m_lRPosMaxOffset;        /* R-axis positive maximum offset to avoid robot hitting chuck (intellipick)  46 */
} stRobotTROffset;

/* Configuration table structure.
 * NOTE: The ordering of members is important!!! That's the order they are
 * recorded and read to and from the configuration table file, respectively. */
typedef struct ConfigurationTableTag
{
    char    m_caSysCfgString[15];               /*  String representing what configuration the
                                                 *  firmware is to be set up as. */
    int     m_iaFeatureFlags[MAXFEATURECONFIG]; /*  Feature flags that indicate which features
                                                 *  are to be active and inactive. */
} stCfgTable,  *psCfgTable;    /*              Structure and pointer-to-structure to hold
                                     *              configuration table parameters. */

extern stCtrlParam sPreParam;       /* ctrl_par_t, prectrl_par
                                     *              Global parameter structure for the pre-aligner parameter file. */

/* structure to hold the offset between the robot end-effector and chuck to avoid of hitting. */
typedef struct VectorParameterTag
{
	long   m_lEELength;	// EE length, required for world coordinate conversion
	long   m_lVS;		// Vector Speed
	long   m_lVA;		// Vector Accel
	long   m_lVD;		// Vector Decel
	long   m_lEfemTheta0;	// Efem Reference Theta Zero angle
	float  m_fSinTheta;	// Sin (m_lEfemTheta0)
	float  m_fCosTheta;	// Cos (m_lEfemTheta0)
	long   m_lV04;		// variable 04
	long   m_lV05;		// variable 05
	long   m_lV06;		// variable 06
	long   m_lV07;		// variable 07
	long   m_lV08;		// variable 08
} stVectorParameter;

/********** FUNCTIONS USED LOCALLY **********/

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Checks that the numerical designation for the
 *      parameter file being accessed is either ROBOTFILE,
 *      PREALIGNFILE, or AUXFILE, the acceptable range
 *      of station coordinate indexes.
 *
\***************************************************************/
int FIOValidateParaDesc(int iParaDesc);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Checks that the index of coordinate/station
 *      being accessed is between 0 and 51, the acceptable range
 *      of station coordinate indexes.
 *
\***************************************************************/
int FIOValidateStationIndex(int iStn);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Returns a pointer to the structure of the
 *      requested parameter file type, i.e. the argument
 *      ROBOTFILE would return a pointer to the robot parameter
 *      structure.
 *
\***************************************************************/
stCtrlParam *FIOGetParamFileType(int iParamDesc);

/***************************************************************\
 *
 * Replaces: devhdr
 * Used in: not used anywhere
 * Description: Critical error handler for int 0x24.  Checks if
 *      error occurred on drive B (the NVSRAM) and sets NVSRAM_ST
 *      if so. Always returns to application program with an error code.
 *
\***************************************************************/
void FIOErrHandler(unsigned uDevError, unsigned uErrCode, unsigned  *ufDevHdr );

/***************************************************************\
 *
 * Replaces: get_chk_sum
 * Used in: Save_parameters, Load_parameters, test_chk_sum,
 *      write_param, write_wafer, write_coord
 * Description: Computes a check sum for a specified block.
 *      The checksum is computed as a 16 bit exclusive-or of
 *      all words in the block.
 *
\***************************************************************/
unsigned FIOComputeChkSum(void  *vfStartAddr, int iBlockSize);

/***************************************************************\
 *
 * Replaces: no_bdrive
 * Used in: main/init, test_NVSRAM
 * Description: Calls a DOS function to set the B drive (NVSRAM)
 *      parameters using the function return code to verify
 *      that there is NVSRAM installed.
 *
\***************************************************************/
int FIONoBDrive();

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Computes a checksum for the specified block
 *      and compares it against a test value.
 *
\***************************************************************/
int FIOTestBlockChkSum(void  *fvStartAddr, int iBlockSize, unsigned uChkSum);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Copies values from the data buffer to the output
 *      buffer from index 0 to 2.
 *
\***************************************************************/
int FIOGetParam(long *lOutBuffer, long *laDataBuffer);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Copies values from the output buffer to the data
 *      buffer from index 0 to 2.
 *
\***************************************************************/
int FIOSetParam(long *laOutBuffer, long *laDataBuffer);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Searches the calibration table linked list for
 *      the entry with the matching wafer size and wafer type.
 *      If the user enters a wafer size number in an Equipe
 *      coded format, it switches to the internal storage format,
 *      i.e. 3=76mm, 4=100mm, 5=125mm, 6=150mm, 8=200mm.
 *
\***************************************************************/
psCalTable FIOSearchCalibTableNode(long lWaferSizeArg, long lTypeArg);

/****************************************************************\
 *
 * Replaces:    write_bytes
 * Used in:     internal function, locally used
 * Description: Writes bytes to file; accumulates checksum. Note that the
 *      pointer argument is a far pointer so that we can write directly
 *      from the malloc'd data structures.
 *
\*****************************************************************/
//int FIOWriteBytes(int iFPArg, int iNumBytesArg, void  *vfDataPtrArg);
int FIOWriteBytes(FILE *iFPArg, int iNumBytesArg, void *vfDataPtrArg);

/****************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Returns the corresponding wafer size in opposite units.
 *      That mean if the wafer size is in inches, it's corresponding size
 *      in millimeters is returned.
 *      i.e. 3=76mm, 4=100mm, 5=125mm, 6=150mm, 8=200mm.
 *
\*****************************************************************/
int FIOGetMatchSize(long lWaferSizeArg, long *lMatchedSizeArg);

#endif
