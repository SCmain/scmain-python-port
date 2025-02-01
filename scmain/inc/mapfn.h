/******************************************************************************\
 *
 * Program:     Scan Mapper function Module
 * File:        mapfn.h
 * Functions:   Cassette mapping scan mapper functions
 *
 * Description: Header file for map.c
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#ifndef _MAPFN_H
#define _MAPFN_H

// function prototypes for execute.c
long MPGetLiftUpXSlotDiffThreshold(void);
void MPSetLiftUpXSlotDiffThreshold(long lValueArg);
int  MPGetHitcount(int iIndexArg);
long MPReadTempData(int iType, int iPassNum, int iIndex );
int  MPLoadTempData(int iIndex, long lPass1, int iTrans1, long lPass2, int iTrans2);

// function prototypes internal to scanner module
int MPAllocRawData( void );
int MPSaveRawData( void );

void MPSetNSFPars
(
    long lPitchArg,
    long lDiffPassArg,
    long lGapArg,
    long lRefPointArg,
    long lThickArg,
    long lCrossRatioArg
);
long MPGetRefPoint(void);
int MPRemoveMultiple
(
    long  *lpPass,            // Z pos data buffer pointer
    int   *ipTrans,           // Z trans data buffer pointer
    long  *lpPos,             // filtered Z pos data buffer
    long  *lpWaf,             // computed Wafer thickness buffer
    long  *lpPitch,           // computed Pitch data buffer
    long  *lpThick            // accumulated thickness
);
long MPCheckSlotStatus
(
    long  *lpWafPos1Arg,     // wafer pos in pass 1
    long  *lpWafPos2Arg,     // wafer pos in pass 2
    long  *lpWafThick1Arg,   // wafer thickness in pass 1
    long  *lpWafThick2Arg,   // wafer thickness in pass 2
    long  *lpPosResultArg,   // wafer pos finally computed
    int   *ipSlotStatArg,    // slot status buffer
    char  *cpBadArg,         // temporary flag buffer
    long lAvThicknessArg,         // average thickness computed before
    long *lpCassAngleArg,         // error-check output
    int  iNewSchemeArg            // new Indexer mapping scheme
);
long MPNewScanFilter
(
    long       *lpPosResultArg,      // output Z pos result data buffer
    int        *ipSlotStatArg,       // output slot status data buffer
    int           iTeachArg,            // input
    long       *lpSlotOffArg,        // output slot offset computation
    int           iNewSchemeArg         // new Indexer mapping scheme
);

#endif
