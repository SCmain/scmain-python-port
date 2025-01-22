/***************************************************************\
 *
 * Program:     Controller Firmware
 * File:        roloc.h
 * Functions:   ROGetMappedAxis
 *              ROGetMilScale
 *              ROSetMilScale
 *              ROArrangeGetArrayForW
 *              ROArrangeSetArrayForW
 *              ROCorrectDistance
 *              ROAxisReadyToMove
 *              ROMoveToAbsOrRel
 *              ROStopMotionLocal
 *              ROServoMCOffLocal
 *              ROServoMCOnLocal
 *              ROInitGalilLocal
 *              ROSendMoveToHSorIP
 *              ROMoveAllOrAxisDirect
 *
 * Description: Functions and variables used locally within the
 *      mechanism module
 *
 * Modification history:
 * Rev      Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_ROLOC_H
#define _H_ROLOC_H

#include "romain.h"

/********** DEFINES **********/

/* Character to exit Galil mode (^I) */
#define END_TALK    9

/* uaAxisMap type definitions. Used to set and get system configuration information */
#define MECH_TYPE       0
#define EQUIPE_AXIS     1
#define GALIL_AXIS      2
#define SPECIAL_AXIS    3

/* The gearing ration for a VAC514 robot with a 1-to-1 arm. */
#define VAC514_T_GEARING_RATIO      -1.0
#define VAC514_R_GEARING_RATIO      1.0


/********** VARIABLES USED LOCALLY **********/

extern unsigned long ulAxisALLRbt;  /* The axis #Define combination for a 3-axis robot only */
extern unsigned long ulAxisallPre;  /* The axis #Define combination for a pre-aligner only */
extern unsigned long ulAllAMFlag;   /* The axis combination for all available axes */
extern unsigned long ulHoming;      /* 1 = in the process of homing; bitwise by axis */
extern unsigned long ulHomed;       /* 1 = axis has previously homed; bitwise by axis */
extern unsigned long ulServoFlag;   /* 1 = servo off; 0 = servo on; bitwise by axis */
extern unsigned long ulERFlag;      /* 1 = position error on axis; bitwise by axis */
extern unsigned long ulAMFlag;      /* 1 = motion completed on axis; bitwise by axis */
extern unsigned long ulLMPosFlag;   /* 1 = pos limit sw now on; bitwise by axis */
extern unsigned long ulLMNegFlag;   /* 1 = neg limit sw now on; bitwise by axis */
extern unsigned long ulLMPosMom;    /* 1 = pos limit sw momentarily on; bitwise by axis */
extern unsigned long ulLMNegMom;    /* 1 = neg limit sw momemtarily on; bitwise by axis */
extern unsigned long ulLMPosChanged;/* 1 = positive limit switch value has changed; bitwise by axis */
extern unsigned long ulLMNegChanged;/* 1 = negative limit switch value has changed; bitwise by axis */
extern unsigned uSCurveFlag;        /* 1 = S-curve profile on; bitwise by axis */
extern int iDefineFlag;             /* The define flags for the current system configuration */
extern int iEmulator;               /* The emulation type of the current system configuration */
extern int iPreOnly;                /* Special considerations for a pre-aligner only system */

/********** FUNCTIONS USED LOCALLY **********/

/* Checks that the axes requested are present in this configuration */
unsigned ROGetMappedAxis(unsigned uAxisArg, unsigned uWhichMapArg);
/* Scales a set of 4 values by 1000. Used to scale Kp, Kd, Ki, and torque limit. */
void ROGetMilScale(double *dInArg, long *lOutArg);
/* Scales a set of 4 values by 1000. Used to scale Kp, Kd, Ki, and torque limit. */
void ROSetMilScale(long *lInArg, double *dOutArg);
/* Puts the W axes returned from a datafile operation into the 4th element of the array */
void ROArrangeGetArrayForW(unsigned long ulEquipeAxisArg, long *lValue);
/* Puts the W axes from the 4th element of the array into the appropriate location
 * in preparation for a datafile operation */
void ROArrangeSetArrayForW(unsigned long ulEquipeAxisArg, long *lValue);
int ROCorrectDistance(int iCardNoArg, long lHomeIndexArg, unsigned uAxisIndexArg);
int ROMoveToAbsOrRel(unsigned long ulEquipeAxisArg, int iRelativeFlagArg, long *lPositionArg);
// Combines ROMoveDirect and ROMoveAxisDirect
int ROMoveAllOrAxisDirect(unsigned long ulAxisArg, int iStnArg, int iOffDirArg, int iFlagArg);
// Combines ROSendMoveToHomeSwitch & ROSendMoveToIndexPulse
int ROSendMoveToHSorIP(ULONG ulAxisArg, int iFlagArg, int iResetParsFlag, int iGearingFlagArg);
// Internalized function. Actually replaces part of ex_STOP.
int ROStopMotionLocal(unsigned long ulEquipeAxisArg);
// Internalized function. Actually replaces part of ex_SVOF.
int ROServoMCOffLocal(unsigned long ulEquipeAxisArg);
// Internalized function. Actually replaces part of ex_SVON.
int ROServoMCOnLocal(unsigned long ulEquipeAxisArg);
// Replaces: init_galil, init_galil1, init_auxgalil
int ROInitGalilLocal(unsigned long ulEquipeAxesArg);

#endif
