/***************************************************************\
 *
 * Program:     Controller Firmware
 * File:        ro.h
 * Functions:   These
 *              include general mechanism functions, motion functions
 *              (which actually should be in its own memotion.h header),
 *              and general purpose utility functions.
 *
 * Description: Provide an interface between other modules and the Mechanism
 *      module
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_RO_H
#define _H_RO_H

#include "romain.h"

#define ALFILENAME      "/root/controller/scmain/aldata"


/********** DEFINES **********/

/* Interface to retrieve the ulAxisALLRbt variable value. "A" in old code. */
unsigned long ROAxisAllRobot();
/* Interface to retrieve the ulAxisallPre variable value. "a" in old code. */
unsigned long ROAxisAllPre();
/* #define to call the ROAxisAllRobot function. For back-compatibility to the old code. */
#define RO_AXIS_ALL ROAxisAllRobot()
/* #define to call the ROAxisAllPre function. For back-compatibility to the old code. */
#define RO_AXIS_all ROAxisAllPre()


/********** GENERAL MECHANISM FUNCTIONS **********/

// Replaces: Part of ex_GALIL
int ROStartGalilMode(int iCardNumArg);
// Replaces: Part of ex_GLST
int ROReadGalilStatus(unsigned long *ulStatusArg);
// Replaces: Part of ex_BYPAS
int ROPortRedirect(int iPortNumArg);
// Replaces: Part of ex_RIPW
int ROReadInPositionWindow(unsigned long ulEquipeAxisArg, long *lPositionArg);
// Replaces: Part of ex_RCP
int ROReadCurrentPosition(unsigned long ulEquipeAxisArg, long *lPositionArg);
// Replaces: Part of ex_RTE
int ROReadCurrentError(unsigned long ulEquipeAxisArg, long *lErrorArg);
// Replaces: Part of ex_RTL
int ROReadCurrentTorque(unsigned long ulEquipeAxisArg, long *lTorqueArg);
// NEW Function
int ROReadHomeFlag(unsigned long ulEquipeAxisArg, long *lHomeFlagArg);
// NEW Function
int ROReadSCurveProfile(unsigned long ulEquipeAxisArg, long *lOnOffArg);
// Replaces: Part of ex_SIPW
int ROSetInPositionWindow(unsigned long ulEquipeAxisArg, long *lPositionArg);
// Replaces: Part of ex_QSCAN
int ROQuitScan(void);
// Replaces: Part of ex_HFLG
int ROSetHomeFlag(unsigned long ulEquipeAxisArg, long *lValueArg);
// Replaces: Part of ex_ABM
int ROAbortMotion(void);
// Replaces: Part of ex_TA
int ROEnableSCurveProfile(unsigned long ulEquipeAxisArg, long *lOnOffArg);
// Replaces: Part of ex_RESP
int ROGetResponseString(unsigned long ulEquipeAxisArg, int iRespErrFlagArg, int *iErrRespArg);
// Replaces: Part of ex_RLP
int ROReadLatchedPosition(unsigned long ulEquipeAxisArg, long *lPositionArg);
// Combines parts of ROStopMotion, ROServoMCOn, ROServoMCOff
int ROProcessMotion(unsigned long ulEquipeAxisArg, int iFlagArg);
// Replaces: Part of ex_STOP
int ROStopMotion(unsigned long ulEquipeAxisArg);
// Replaces: servo_mc_off
int ROServoMCOff(unsigned long ulEquipeAxisArg);
// Replaces: servo_mc_on
int ROServoMCOn(unsigned long ulEquipeAxisArg);
// Replaces: Part of ex_WMO
int ROIsAxisMotionInProgress(unsigned long ulEquipeAxisArg, int iTOTFlagArg, int *lValueArg);


/********** UTILITY FUNCTIONS **********/

// NEW Function
int ROGetCardIndex(void);
// Replaces: init_galil, init_galil1, init_auxgalil
int ROInitGalil(int iFileTypeArg);
// Replaces: check_switches
int ROCheckSwitches(int iCardArg);
// Replaces: check_limit_sw
int ROCheckLimitSwitches(void);
// Replaces: Part of ex_RLS
int ROReadLimitSwitches(unsigned long *ulSwitchesArg);
// Replaces: disconnect_power
void RODisconnectPower();
// Replaces: check_motion_error
void ROCheckMotionError(int iCardNumArg, unsigned uBitsArg);
// Replaces: write_galil_port
int ROWriteGalilPort(int iCardNo, int iBitNum, int iData);
// Replaces: read_galil_port
int ROReadGalilPort(int iCardNo);
// Replaces: motion_check
int ROAxisReadyToMove(unsigned long ulEquipeAxisArg, int *iCardNumArg, int *iFileTypeArg, unsigned *uGalilAxesArg);
// Replaces: scale_spd
int ROScaleSpd(unsigned long ulEquipeAxisArg, long *lSpeedArg, long *lScaledSpeedArg);
// Replaces: unscale_spd
int ROUnscaleSpd(unsigned long ulEquipeAxisArg, long *lSpeedArg, long *lUnscaledSpeedArg);
// Replaces: scale_pos
int ROScalePos(unsigned long ulEquipeAxisArg, long *lPositionArg, long *lScaledPositionArg);
// Replaces: unscale_pos
int ROUnscalePos(unsigned long ulEquipeAxisArg, long *lPositionArg, long *lUnscaledPositionArg);

unsigned long ROGetERFlag();
unsigned long ROGetLMPosFlag();
unsigned long ROGetLMNegFlag();
unsigned long ROGetLMPosMom();
unsigned long ROGetLMNegMom();
unsigned long ROGetHomed();
unsigned long ROGetServoFlag();
unsigned long ROGetAMFlag();
unsigned long ROGetAllAMFlag();

int ROSetERFlag(unsigned long ulERFlagArg);
int ROSetHomed(unsigned long ulHomedArg);
int ROSetAMFlag(unsigned long ulAMFlagArg);
int ROSetEMOWasSet(int iEMOWasSetArg);


int ROSendDefinePositionCommand(unsigned long ulEquipeAxisArg, long *lValueArg);
int ROSendFindEdgeCommand(unsigned long ulEquipeAxisArg);
int ROSendHomeCommand(unsigned long ulEquipeAxisArg);
int ROSendBeginMotionCommand(unsigned long ulEquipeAxisArg);
int ROSendPositionAbsoluteCommand(unsigned long ulEquipeAxisArg, long *lValueArg);
int ROGetInterruptPositionCommand(unsigned long ulEquipeAxesArg, long *plValueArg);
int ROSetInterruptPositionCommand(unsigned long ulEquipeAxesArg, long *plValueArg);
int ROSendMoveToHomeSwitch(unsigned long ulAxisArg);
int ROSendMoveToIndexPulse(unsigned long ulAxisArg);
int ROElectronicGearingON(char cMasterArg, char cSlaveArg, double dGearRatioArg, BOOL CommandedPosGearingArg);
int ROElectronicGearingOFF();
void ROPrepareScan(unsigned long ulEquipeAxisArg);
int ROValidAxis(unsigned long ulEquipeAxisArg, int *iCarNumArg, int *iFileTypeArg, unsigned *uGalilAxesArg);
unsigned long ROGetSpecialAxis(unsigned uSpecialAxisArg);

void ROStopAllFromIntr( void );
void ROEmergencyOff( int iSw );
int ROAddActionBufferMotion(unsigned long ulEquipeAxisArg, unsigned uAMWaitArg, unsigned uAMNextArg,
                                unsigned uINWaitArg, unsigned uINNextArg, unsigned uTMWaitArg, long *plDataBufArg);
int ROAddActionBufferSet(unsigned long ulEquipeAxisArg, unsigned uAMWaitArg, unsigned uAMNextArg, unsigned uTMWaitArg, long *plDataBufArg, int iParamTypeArg);
int ROGetCardAxes(int iFileTypeArg);
int ROServiceBrake(int iParamFileArg, int iOnOffArg);

//Following functions set/read internal variable which holds commanded motion flag for VAC514 robots
void ROSetCM(BOOL iModeArg);
BOOL ROGetCM();
long ROReadGalilModulo(long *plModuloArg);
long ROSetGalilModulo(long lModuloArg);

void ROUpdateTS(int iFlagArg);
void ROUpdateTP(int iFlagArg);
int  ROActionStatus(void);
void ROCheckLatch(int iFlagArg);
int ROReadITValue(unsigned long ulEquipeAxisArg, long *lValueArg);
int ROEncoderToEU(unsigned long ulEquipeAxisArg, long lEncoderArg, long *lScaledPositionArg);
int ROEUToEncoder(unsigned long ulEquipeAxisArg, long lPositionArg, long *lUnscaledPositionArg);
void RORaiseAlarm(unsigned int uAlarmBitArg);
void ROClearAlarm(void);
void ROTurnLight(int iLightNoArg, int iFlagArg);
void ConvertStrToL(char* strBuf, int iNumAxesArg, long* lTSArray);
int ConvertStrToi(char* strBufArg, int iMaxArg, int* iArrayArg);
void RODisableLatch(int iFlagArg);
#endif
