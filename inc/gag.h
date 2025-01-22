/***************************************************************\
 *
 * Program:     Motion Control Interface Header
 * File:        gag.h
 * Functions:   GAGetUseGalilFlag
 *              GASetUseGalilFlag
 *              GASetExcIntOutp
 *              GAGetGalilErrorFlag
 *              GASetGalilErrorFlag
 *              GASetDebugBit
 *              GAGetDebugBit
 *              GAWriteGalilInputOutputPort
 *              GAReadGalilInputOutputPort
 *              GASendReceiveGalil
 *              GAAmpInit
 *              GARouteStringToQueueBuffer
 *              GAFlushOutCmdStringQueueToGalil
 *              GAInitCmdString
 *              GAStartCommandsQueuing
 *              GACancelCommandsQueuing
 *              GASendSSCommand
 *              GASendAbortMotionCommand
 *              GASetTimeCommand
 *              GASendConfigureSystemCommand
 *              GASendAxesCommand
 *              GASetValsLongDefined
 *              GASetValsDoubleDefined
 *              GASetValsLong
 *              GASetValsDouble
 *              GAGetValsLongDefined
 *              GAGetValsDoubleDefined
 *              GAGetValsLong
 *              GAGetValsDouble
 *
 * Description: Provide interface routines to the Motion Control
 *      module for functions used glbally throught the code.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_GAG_H
#define _H_GAG_H

#include "dmclnx.h"

/********** DEFINES **********/
/* Motion control (Galil) card number specifiers. */
#define GA_CARD_0       0x01
#define GA_CARD_1       0x02
#define GA_BOTH_CARDS   0x03

/* The maximum number of characters in a Galil I/O string. */
#define MAXGASTR        80

/* Bit mask pattern for axes specifications passed motion functions. */
#define GAXAXIS0        0x01
#define GAYAXIS0        0x02
#define GAZAXIS0        0x04
#define GAWAXIS0        0x08
#define GAALLAXIS0      0x0F
#define GAEAXIS0	0x10
#define GAFAXIS0	0x20
#define GAGAXIS0	0x40
#define GAHAXIS0	0x80
#define GAALLAXES	0xFF

/* Galil "TS" (Tell Switches) bits. */
#define     INMOTION	0x80
#define     INERROR     0x40
#define     MOTOFF      0x20
#define     UNDEF       0x10
#define     POSLIM      0x08
#define     NEGLIM      0x04
#define     HOMSW       0x02
#define     LATCHED     0x01

/* Galil command strings. */
#define  ABORT_MOTION_COMMAND           "AB"
#define  ACCELERATION_COMMAND           "AC"
#define  AFTER_DISTANCE_COMMAND         "AD"
#define  ANALOG_FEEDBACK_COMMAND        "AF"
#define  AFTER_INPUT_COMMAND            "AI"
#define  ARM_LATCH_COMMAND              "AL"
#define  AFTER_MOVE_COMMAND             "AM"
#define  AFTER_ABS_POS_COMMAND          "AP"
#define  AFTER_REL_DIST_COMMAND         "AR"
#define  AFTER_SPEED_COMMAND            "AS"
#define  AT_TIME_COMMAND                "AT"
#define  AFTER_VECTOR_DIST_COMMAND      "AV"
#define  BEGIN_MOTION_COMMAND           "BG"
#define  REVERSE_SW_LIMIT_COMMAND       "BL"
#define  BURN_COMMAND                   "BN"
#define  BURN_PROGRAM_COMMAND           "BP"
#define  BURN_VARIABLE_COMMAND          "BV"
#define  CLEAR_BIT_COMMAND              "CB"
#define  CONFIG_COMPORT2_COMMAND        "CC"
#define  CONTOUR_DATA_COMMAND           "CD"
#define  CONFIG_ENCODER_COMMAND         "CE"
#define  COMM_INTERRUPT_COMMAND         "CI"
#define  CONTOURING_MODE_COMMAND        "CM"
#define  CONFIG_SYSTEM_COMMAND          "CN"
#define  CONFIG_OUTPUT_COMMAND          "CO"
#define  CIRCLE_COMMAND                 "CR"
#define  CLEAR_SEQUENCE_COMMAND         "CS"
#define  COPYRIGHT_INFO_COMMAND         "CW"
#define  DEALLOC_VARS_ARRAYS_COMMAND    "DA"
#define  DECELERATION_COMMAND           "DC"
#define  DUAL_ENCODER_POS_COMMAND       "DE"
#define  DOWNLOAD_COMMAND               "DL"
#define  DIMENSION_COMMAND              "DM"
#define  DEFINE_POSITION_COMMAND        "DP"
#define  CONFIG_SECOND_COMM_CH_COMMAND  "DR"
#define  DELTA_TIME_COMMAND             "DT"
#define  DUAL_VELOCITY_COMMAND          "DV"
#define  CHOOSE_ECAM_MASTER_COMMAND     "EA"
#define  ENABLE_ECAM_COMMAND            "EB"
#define  EDIT_COMMAND                   "ED"
#define  ECAM_GO_COMMAND                "EG"
#define  ENABLE_INTERRUPTS_COMMAND      "EI"
#define  CAM_CYCLE_COMMAND              "EM"
#define  ECHO_COMMAND                   "EO"
#define  CAM_TABLE_INTERVAL_COMMAND     "EP"
#define  ECAM_QUIT_COMMAND              "EQ"
#define  ERROR_LIMIT_COMMAND            "ER"
#define  ELLIPSE_SCALE_COMMAND          "ES"
#define  ELECTRONIC_CAM_TABLE_COMMAND   "ET"
#define  FEED_FORW_ACCEL_COMMAND        "FA"
#define  FIND_EDGE_COMMAND              "FE"
#define  FIND_INDEX_COMMAND             "FI"
#define  FORW_SOFTWARE_LIMIT_COMMAND    "FL"
#define  VELOCITY_FEED_FORW_COMMAND     "FV"
#define  MASTER_AXIS_GEARING_COMMAND    "GA"
#define  MASTER_AXIS_GEARING_COMMAND_P  "GAC"
#define  GANTRY_MODE_COMMAND            "GM"
#define  GAIN_COMMAND                   "GN"
#define  GEAR_RATIO_COMMAND             "GR"
#define  HOME_COMMAND                   "HM"
#define  HALT_EXECUTION_COMMAND         "HX"
#define  OPT_INTR_POS_COMMAND           "ID"
#define  INPUT_INTERRUPT_COMMAND        "II"
#define  INTEGRATOR_LIMIT_COMMAND       "IL"
#define  INC_POSITION_COMMAND           "IP"
#define  INDEP_TIME_COMMAND             "IT"
#define  JOG_MODE_COMMAND               "JG"
#define  KDERIVATIVE_COMMAND            "KD"
#define  KINTEGRATOR_COMMAND            "KI"
#define  KPROPORTIONAL_COMMAND          "KP"
#define  STEP_MOTOR_SMOOTHING_COMMAND   "KS"
#define  LINEAR_INTERP_END_COMMAND      "LE"
#define  LINEAR_INTERP_DIST_COMMAND     "LI"
#define  LINEAR_INTERP_MODE_COMMAND     "LM"
#define  LEADING_ZEROS_COMMAND          "LZ"
#define  MOTION_COMPLETE_COMMAND        "MC"
#define  MODULO_COMMAND                 "MD"
#define  FORW_MOTION_TO_POS_COMMAND     "MF"
#define  MESSAGE_COMMAND                "MG"
#define  MOTOR_OFF_COMMAND              "MO"
#define  REV_MOTION_TO_POS_COMMAND      "MR"
#define  MOTOR_TYPE_COMMAND             "MT"
#define  NO_OPERATION_COMMAND           "NO"
#define  OUTPUT_BIT_COMMAND             "OB"
#define  CIRCULAR_COMPARE_COMMAND       "OC"
#define  OFF_ON_ERROR_COMMAND           "OE"
#define  GAIN_OFFSET_COMMAND            "OF"
#define  OUPUT_PORT_COMMAND             "OP"
#define  OUTPUT_BLOCK_COMMAND           "OQ"
#define  POSITION_ABS_COMMAND           "PA"
#define  POSITION_FORMAT_COMMAND        "PF"
#define  POLE_COMMAND                   "PL"
#define  POSITION_REL_COMMAND           "PR"
#define  RT_DMA_INFO_COMMAND            "QZ"
#define  RECORD_ARRAY_COMMAND           "RA"
#define  RT_FR_ERROR_ROUTINE_COMMAND    "RE"
#define  RT_FR_INTERRUPT_ROUTINE        "RI"
#define  REPORT_LATCH_POS_COMMAND       "RL"
//#define  ACC_DECEL_RAMP_COMMAND         "RM"
#define  ACC_DECEL_RAMP_COMMAND         "NO"
#define  REF_POSITION_COMMAND           "RP"
#define  RESET_COMMAND                  "RS"
#define  MASTER_RESET_COMMAND           "\x012\x013\r"  // <ctrl>R <ctrl>S
#define  READ_VERSION_COMMAND           "\x012\x016\r"  // <ctrl>R <ctrl>V
#define  SET_BIT_COMMAND                "SB"
#define  STOP_CODE_COMMAND              "SC"
#define  SPECIAL_GEARING_COMMAND        "SG"
#define  SERVO_ON_COMMAND               "SH"
#define  SPEED_COMMAND                  "SP"
#define  SENSE_SWITCHES_COMMAND         "SS"
#define  STOP_MOTION_COMMAND            "ST"
#define  SCURVE_COMMAND                 "TA"
#define  TELL_STATUS_BYTE_COMMAND       "TB"
#define  TELL_ERROR_CODE_COMMAND        "TC"
#define  TELL_DUAL_ENCODER              "TD"
#define  TELL_ERROR_COMMAND             "TE"
#define  TELL_INPUPS_COMMAND            "TI"
#define  TORQUE_LIMIT_COMMAND           "TL"
#define  TIME_COMMAND                   "TM"
#define  TANGENT_COMMAND                "TN"
#define  TELL_POSITION_COMMAND          "TP"
#define  TRACE_COMMAND                  "TR"
#define  TELL_SWITCHES_COMMAND          "TS"
#define  TELL_TORQUE_COMMAND            "TT"
#define  TELL_VELOCITY_COMMAND          "TV"
#define  TIMEOUT_FOR_IN_POS_COMMAND     "TW"
#define  USER_INTERRUPT_COMMAND         "UI"
#define  VECTOR_ACC_COMMAND             "VC"
#define  VECTOR_DEC_COMMAND             "VD"
#define  VECTOR_SEQ_END_COMMAND         "VE"
#define  VARIABLE_FORMAT_COMMAND        "VF"
#define  COORDINATED_MOTION_COMMAND     "VM"
#define  VECTOR_POSITION_COMMAND        "VP"
#define  VECTOR_SPEED_RATIO_COMMAND     "VR"
#define  VECTOR_SPEED_COMMAND           "VS"
#define  VECTOR_TIME_CONSTANT_COMMAND   "VT"
#define  WAIT_FOR_CONTOUR_DATA_COMMAND  "WC"
#define  WAIT_COMMAND                   "WT"
#define  EXECUTE_PROGRAM_COMMAND        "XQ"
#define  COMP_ZERO_COMMAND              "ZR"
#define  ZERO_SUB_STACK_COMMAND         "ZS"


/********** FUNCTION PROTOTYPES **********/
/* Routines to get and set flag values. */
int GAGetUseGalilFlag(void);
void GASetUseGalilFlag(int iUseGalilFlag);
void GASetExcIntOutp(int iExcIntOup);
unsigned GAGetGalilErrorFlag(void);
void GASetGalilErrorFlag(unsigned uGalilErrorFlag);
unsigned GAGetGLNumAxis(int iCardArg);
unsigned GAGetDebugBit();
void GASetDebugBit(unsigned iBitArg);

/* Routines for direct communications with the Galil card. */
int GAWriteGalilInputOutputPort(int iGalilCardNo, int iBitNumber, int iData);
int GAReadGalilInputOutputPort(int iCardNoArg, int *piReadData);
int GASendReceiveGalil(int iCardNoArg, char  *pcCmdStr, char  *pcReadStr);
int GAReadCharFromGalil(int iCardNoArg, int *piReadChar);
int GAWriteCharToGalil(int iCardNoArg, int iChar);
int GAAmpInit(void);

// DMCCommand interface
int GASendDMCCommand(HANDLEDMC ghDMC, char* caCommand, char* caResp, int iMaxStr);

/* Routines to manipulate a command queue. */
int GARouteStringToQueueBuffer(int iCardNoArg, int iNumOfCommands, char *cpCommandString);
int GAFlushOutCmdStringQueueToGalil(int iCardNoArg);
int GAInitCmdString(int iCardNoArg);
int GAStartCommandsQueuing(int iCardNoArg);
int GACancelCommandsQueuing(int iCardNoArg);

/* Galil commands with special parameter considerations. */
int GASendSSCommand(int iCardNoArg, char  *pcSSBitStruct);
int GASendAbortMotionCommand(int iCardNoArg, int iAbortMotionOnly);
int GASetTimeCommand(int iCardNoArg, long lTimeCommandArg);
int GASendConfigureSystemCommand(int iCardNoArg, int iLimitSwitchArg, int iHomeSwitchArg, int iLatchInputArg, int iReservedArg);

/* Galil commands with only axis names, no parameter values. */
int GASendAxesCommand(int iCardNoArg, char  *pcCommandArg, unsigned uGalilAxisArg);

/*** General routines to get Galil values. ***/
int GASetValsLongDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, long *plValuesArg);
int GASetValsDoubleDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, double *pdValuesArg);
int GASetValsLong(int iCardNoArg, char  *pcCommandArg, unsigned uGalilAxisArg, long *plValuesArg);
int GASetValsDouble(int iCardNoArg, char  *pcCommandArg, unsigned uGalilAxisArg, double *pdValuesArg);

/*** General routines to set Galil values. ***/
/* Galil commands that return long values. */
int GAGetValsLongDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, long *plValuesArg);
int GAGetValsDoubleDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, double *pdValuesArg);
int GAGetValsLong(int iCardNoArg, char  *pcCommandArg, unsigned uGalilAxisArg, long *plValuesArg);
int GAGetValsDouble(int iCardNoArg, char  *pcCommandArg, unsigned uGalilAxisArg, double *pdValuesArg);

#endif
