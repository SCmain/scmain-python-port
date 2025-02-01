/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        fiog.h
 * Functions:   FIOTestFileChkSum
 *              FIOTestNVSRAM
 *              FIOInitNVSErrorHandler
 *              FIOSetDefaultParam
 *              FIOInitParamModule
 *              FIOReadParam
 *              FIOWriteParam
 *              FIOInitCoordinate
 *              FIOReadCoord
 *              FIOWriteCoord
 *              FIOInitConfig
 *              FIOReadConfig
 *              FIOWriteConfig
 *              FIOGetCfgFeatureFlags
 *              FIOSetCfgFeatureFlags
 *              FIOWriteOTFFile
 *              FIOReadOTFFile
 *
 *              GetParamXXXXXXX() - To allow all modules
 *                  to get the parameter values from
 *                  the parameter structure.
 *
 *              PutParaXXXXXXX() - To allow all modules
 *                  to write the parameter values to
 *                  parameter structure.
 *
 *              GetStnXXXXXXX() - To allow all modules
 *                  to get the station coordinate values from
 *                  the station structure.
 *
 *              PutStnXXXXXXX() - To allow all modules
 *                  to write the station coordinate values to
 *                  the station structure.
 *

 * Description: Provide interface between datafile structures
 *      and all the other modules.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: The ??? in the comments indicates that the variable
 *      was not used at the time of commenting so it's
 *      function was not understood enough to write a comment.
 *      They must be fixed as they come into use and are debugged.
 *
 * NOTE: When values are changed using host commands, they are
 *      only changed locally in the file structure. If the
 *      system power cycles, those changes are lost, UNLESS
 *      they are saved with the SAV host command. Then the
 *      values are gathered and written to data files in the
 *      NVSRAM.
 *
\***************************************************************/

#ifndef _H_FIOG_H
#define _H_FIOG_H

/********** DEFINES **********/

/* Numerical designations for the parameter files. */
#define ROBOTFILE           0
#define PREALIGNFILE        1
#define AUXFILE             2

/* Bits in  "chk_sum_stat" */
#define PARA_FILE     0x0001        /* robot parameter file */
#define COOR_FILE     0x0002	    /* coordinate file */
#define MACR_FILE     0x0004        /* macro file */
#define PARA_FILE1    0x0008        /* pre-aligner parameter file */
#define WAFER_FILE    0x0010	    /* wafer parameter file */
#define CALTBL_FILE   0x0020        /* wafer calibration file */
#define PARA_FILE2    0x0040        /* auxilliary parameter file */
#define CFG_FILE      0x0080        /* configuration table file */
#define OTF_FILE      0x0100        /* otf parameter file */
#define BAD_NVSRAM    0x8000	    /* NVSRAM not recognized */

/* Maximum number of items in the configuration table mechanism configuration arrays. */
#define MAXMECHCONFIG       8

/* Maximum number of items in the configuration table
 * feature configuration array minus the version string. */
#define MAXFEATURECONFIG    11

/* Array elements in configuration table. */
#define SECS_HOST           0
#define SECS_DEVICE         1
#define TOT                 2
#define OTF                 3
#define MULTI_CHIP_ALIGN    4
#define COM3_SUPPORT        5
#define INTELLIPICK         6
#define SQUARE_WAFER_ALIGN  7
#define SCANNER             8
#define EQUIPE_BUS          9
#define FAST_ALIGN          10
#define SYSCFG              11          /* This entry MUST be last. */

/* Maximum number of items in the parameter table. */
#define MAX_PARAM_ITEMS     23

/* Elements of the parameter structure. They are used to generically get and set
 * values in the parameter structure. */
#define HOME_SPEED          0   /* Homing speed scaled to human units, i.e. inches/sec. */
#define HOME_ACCEL          1   /* Homing acceleration scaled to human units, i.e. inches/sec^2. */
#define ENCODER_RESOLUTION  2   /* Encoder resolution, for finding proper Home-Sw-to-Index distance,
                                 *                  used to be reserved 1. */
#define LIMIT_SWITCH_ACCEL  3   /* used to be reserved 2. */
#define OPERATIONAL_SPEED   4   /* Operating speed scaled to human units, i.e. inches/sec. */
#define OPERATIONAL_ACCEL   5   /* Operating acceleration scaled to human units, i.e. inches/sec^2. */
#define ERROR_LIMIT         6   /* Error limit. */
#define PROPORTIONAL_GAIN   7   /* Proportional gain, part of PID. */
#define FEED_FORWARD_ACCEL  8   /* Feed-forward acceleration, additional PID tuning parameter. */
#define INTEGRAL_GAIN       9   /* Integral gain, part of PID. */
#define TORQUE_LIMIT        10  /* Torque Limit. */
#define DERIVATIVE_GAIN     11  /* Derivative gain, part of PID. */
#define JERK                12  /* Ramp, a.k.a. jerk, rate of change of acceleration. */
#define CUSTOMIZED_HOME     13  /* Customized home position. */
#define HOME_OFFSET         14  /* Distance from home switch to actual home position, in encoder units. */
#define MECHANICAL_RATIO    15  /* Mechanical ratio, the gearing ratio. */
#define ARM_LENGTH          16  /* Arm length. */
#define H_VALUE             17  /* used to be reserved 3. */
#define OPERATIONAL_DECEL   18  /* Operating deceleration, scaled to human units, i.e. inches/sec^2. */
#define SOFT_NEG_LIMIT      19  /* Software negative limit. */
#define SOFT_POS_LIMIT      20  /* Software positive limit. */
#define SCAN_OFFSET         21  /* Mapper offset. */
#define SPEED_LIMIT         22  /* ?? */
#define JOG_SPEED           23  /* needed for VAC514 when in Homing sequence we use Galil JG command*/
#define IT_SCURVE			24  /* NEW PARAMETER IN GALIL SET BY IT COMMAND */

/* Elements of the station structure. They are used to generically get and set
 * values in the station structure. */
#define OFFSET              0   /* offset           Distance above the wafer position to move during a stroke. */
#define Z_INDEX             1   /* z_index          Slot number in the cassette, used with MTCR and MATS. */
#define STROKE              2   /* stroke           Z distance to move during a STROK command. */
#define START_ANGLE         3   /* start_angle      Starting angle relative to coordinate, CSI 0, NOT USED. */
#define END_ANGLE           4   /* end_angle        Ending angle relative to coordinate, CSI 1. */
#define CROSS_SLOT_LOW      5   /* CSLow            Cross-slot/double-slot low threshold, CSI 2. */
#define CROSS_SLOT_HIGH     6   /* CSHigh           Cross-slot/double-slot high threshold, CSI 3. */
#define ACC_LOW             7   /* AccLow           ???, maybe set by TSCN, CSI 4. */
#define ACC_HIGH            8   /* AccHigh          ???, maybe set by TSCN, CSI 5. */
#define AVE_THICK           9   /* ave_thick        Average thickness of wafer, also affected by slanted cassette, CSI 6. */
#define TAUGHT_POS_TO_WAFER 10  /* NEW              Distance from taught position to actual position of first wafer. */
#define CROSS_SLOT_THRESH   11  /* NEW              Cross slot threshold. */
#define TRACK_POS           12  /* NEW              Track position. */
#define RET_HOME            13  /* R_home           Retracted home position for radial axis.
                                 *                  This is typically the customized home but can be
                                 *                  different for different stations. */
#define PITCH               14  /* pitch            The distance between slots in a cassette. It's units are
                                 *                  ten-thousandths of an inch or microns. This is not standardized
                                 *                  and still a point of controversy. */
#define FLP_COORD           15  /* NEW              The flipper coordinate. */
#define FLP_SAFE_R          16  /* NEW              The safe R position to flip the flipper. */

/* The maximum number of coordinate special items. */
#define FIO_MAX_NUM_CSI      16

//Dos&Counters
#define NUMBER_OF_NVSRAM_COUNTERS  14

enum WriteLogFile
{
    INCREMENT_ROBPARFILECOUNTER, INCREMENT_PREPARFILECOUNTER, INCREMENT_AUXPARFILECOUNTER,
    INCREMENT_COORDFILECOUNTER, INCREMENT_CALIBFILECOUNTER, INCREMENT_WAFERFILECOUNTER,
    INCREMENT_MACROFILECOUNTER, INCREMENT_SAVCOMMANDCOUNTER, INCREMENT_SMCRCOMMANDCOUNTER,
    INCREMENT_WRIPCOMMANDCOUNTER, INCREMENT_WRISCOMMANDCOUNTER, INCREMENT_WRIWCOMMANDCOUNTER,
    READ_FILECOUNTERS, CLEAR_FILECOUNTERS, INIT_FILECOUNTERS, INCREMENT_OTFFILECOUNTER,
    INCREMENT_CONFIGFILECOUNTER
};

/********** FUNCTION PROTOTYES *********/

/* File and NVSRAM related functions. */
int FIOTestFileChkSum(char *pcFileName);     /* Replaces: test_macro_chksum */
int FIOTestNVSRAM(int iDevModeArg);          /* Replaces: test_NVSRAM */
void FIOInitNVSErrorHandler();               /* Replaces: init_nvs_err_handler */

/* Functions for manipulation of entire parameter structure at once. */
void FIOSetDefaultParam(int iFileDes);       /* Replaces: init_ctrl_par */
int FIOInitParamModule();                    /* NEW Function */
int FIOReadParam(int iFileDes);              /* Replaces: read_par */
int FIOWriteParam(int iFileDes);             /* Replaces: write_par */

/* Function to support File Revision */
int FIOSetFileRevision(long lFileNum, char *sRevisionArg); /* NEW to support File Revision */
int FIOGetFileRevision(long lFileNum, char *sRevisionArg); /* NEW to support File Revision */

/* Functions for manipulation of entire station coordinate structure at once. */
void FIOInitCoordinate();                    /* NEW Function */
int FIOReadCoord();                          /* Replaces: read_coord */
int FIOWriteCoord();                         /* Replaces: write_coord */

/* Functions for manipulation of entire configuration table structure at once. */
void FIOInitConfig();                        /* NEW Function */
int FIOReadConfig();                         /* NEW Function */
int FIOWriteConfig();                        /* NEW Function */


/********** NEW FUNCTIONS *************
 * The rest of the functions get and set variable values
 ******************************************/

/* GET variable values from the PARAMETER structure. */
int FIOGetParamSerialNum(int iParaDesc, char *pcSN);
int FIOGetParamBaudRate(int iParaDesc, int *piBaudRate);
int FIOGetParamVals(int iParamFileArg, int iParamDescArg, long *plaValuesArg);

/* SET variable values to the PARAMETER structure. */
int FIOSetParamSerialNum(int iParaDesc, char *pcSN);
int FIOSetParamBaudRate(int iParaDesc, int iBaudRate);
int FIOSetParamVals(int iParamFileArg, int iParamDescArg, long *plaValuesArg);


/* GET variable values from the STATION structure. */
int FIOGetCurrentStationInt();
int FIOGetStnCoord(int iStn, int Index, long *plCoord);
int FIOGetStnlscsi(int iStn, int Index, long *plLscsi);
int FIOGetStnScanCoord(int iStn, int Index, long *plScanCoord);
int FIOGetStnEEindex(int iStn, int *piEEindex);
int FIOGetStnVals(int iStnArg, int iValueDescArg, long *plValueArg);

/* SET variable values to the STATION structure. */
int FIOSetCurrentStationInt(int iStn);
int FIOSetStnCoord(int iStn, int Index, long plCoord);
int FIOSetStnlscsi(int iStn, int Index, long lLscsi);
int FIOSetStnScanCoord(int iStn, int Index, long plScanCoord);
int FIOSetStnEEindex(int iStn, int iEEindex);
int FIOSetStnVals(int iStnArg, int iValueDescArg, long lValueArg);


/* GET variable values from the CONFIGURATION structure. */
int FIOGetCfgFeatureFlags(int iFeatureIndexArg, int *ipFeatureFlagsArg);
int FIOGetCfgSysCfgString(char  *cpSysCfgStringArg);
unsigned FIOGetCfgFeatureInt( void );

/* SET variable values to the CONFIGURATION structure. */
int FIOSetCfgFeatureFlags(int iFeatureIndexArg, int iFeatureFlagsArg);
int FIOSetCfgSysCfgString(char  *cpSysCfgStringArg);

/* OTF */
int FIOWriteOTFFile();
int FIOReadOTFFile();

/* Vector */
int FIOWriteVectorFile();
int FIOReadVectorFile();

//Dos&Counters
int FIOWriteToNVSRAMLogFile(int iTypeOfOperationArg);

#endif
