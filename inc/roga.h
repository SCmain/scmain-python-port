/***************************************************************\
 *
 * Program:     Controller Firmware
 * File:        roga.h
 * Functions:
 *
 * Description: Provide an interface between other modules and the Mechanism
 *      module motion functions
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_ROGA_H
#define _H_ROGA_H

#include "romain.h"

/*
 * Speeds and accel used in the second phase of the homing sequence,
 * when we need to sense switches and index pulse precisely
*/
#define HOMING_SPEED_T      14      //in ESC firmware units
#define HOMING_SPEED_R      4       //in ESC firmware units
#define HOMING_ACCEL_T      24      //in ESC firmware units
#define HOMING_ACCEL_R      8       //in ESC firmware units
#define HOMING_JOG_SPEED    1000    //in encoder units

#define TTARRAYSIZE 3900


/********** MOTION FUNCTIONS; written by Clay Kim **********/

// Replaces: Part of ex_HOME
int ROHomeAxis(unsigned long ulEquipeAxisArg, int iStageFlagArg);
int ROMoveToAbs(unsigned long ulEquipeAxisArg, long *lPositionArg);
int ROMoveToRel(unsigned long ulEquipeAxisArg, long *lPositionArg);
// Replaces: move_retract
int ROMoveRetract(int iStnArg, int iOffDirArg, int iRetractArg, int iExtendArg, int iUseScanArg);
// Replaces: move_direct
int ROMoveDirect(int iStnArg, int iOffDirArg);
// Replaces: move_axis_direct
int ROMoveAxisDirect(unsigned long ulAxisArg, int iStnArg, int iOffDirArg);
// Replaces: move_stroke
int ROMoveStroke(int iStnArg, int iStrokeDirArg);
// The following functions have unknown origin
int RORetractR(long lPosArg, int iAxisFlag);

//Following functions are used in the homing sequence of VAC514 robots
int ROSetSlowHomingSpeed(unsigned long ulAxisArg);
int ROSetJGSpeed(unsigned long ulAxisArg);

int ROSendRC(unsigned long ulAxisArg, long lFlagArg);
int ROSendQU(unsigned long ulAxisArg);
int RODumpRC(unsigned long ulAxisArg, long lFirstArg, long lLastArg);
int ROWriteRC(void);
int ROReadRC(void);
void ROCheckIdle(void);
int ROFrictionTest(unsigned long ulAxisArg);
int ROGotoPrevPos(unsigned long ulAxisArg, long *lPositionArg, unsigned long ulAxisTrack, long *lCurrentSpeedArg);
int ROGotoSafePos( long *lSafePos, unsigned long ulAxisTrack );
int ROMotionProfTest(unsigned int ulAxisArg, long lEncSourceArg, long lEncDestinArg);
void ROClearFrictionData(void);
int RORestoreAxis(unsigned int ulAxisArg, long lPosArg, long *lCurrentSpeedArg);

int ROMoveToCartesian(unsigned long ulEquipeAxisArg, long *lPositionArg);
void ROGetMidPositions(long lStartTArg, long lStartRArg, long lFinalTArg, long lFinalRArg,
	long *lMidTPosArg, long *lMidRPosArg, long *lMidWXArg, long *lMidWYArg, int *nPointArg);
void ROConvertWorldToRobot(long lWXArg, long lWYArg, long *lPosTArg, long *lPosRArg);
void ROConvertRobotToWorld(long lPosTArg, long lPosRArg, long *lWXArg, long *lWYArg);
void ROConvertWorldToEfem(long lPosTArg, long lPosRArg, long *lWXArg, long *lWYArg);
void ROConvertEfemToWorld(long lPosTArg, long lPosRArg, long *lWXArg, long *lWYArg);

float ROGetSinTheta(long lThetaArg);
float ROGetCosTheta(long lThetaArg);

int ROMoveCartesianDirection(long lDirectionArg, long lDistanceArg);

#endif
