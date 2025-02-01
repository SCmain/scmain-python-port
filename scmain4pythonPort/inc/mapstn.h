/******************************************************************************\
 *
 * Program:     Mapper Module
 * File:        mapstn.h
 * Functions:   Cassette mapping scanner station handling.
 *
 * Description: Header file for mapstn.c
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#ifndef _H_MAPSTN_H
#define _H_MAPSTN_H

// constants
#include "mapk.h"

// scan station structure
typedef struct tgMapStnTag
{
    int     m_cStName;
    int     m_iLoaded[NOFPASSES+1][NOFSLOTS];
    int     m_iLastPass;
    long    m_plROffset[NOFPASSES][NOFSLOTS*2];
    long    m_plZPos[NOFPASSES][NOFSLOTS];
    struct  tgMapStnTag  *m_pstNextPtr;
}   stMapStn,  *pstMapStn;

// scan station related functions
int MPInitMapper(int iDefineFlagArg, int iEmulatorArg);
//      MPSearchScanIndex is formerly search_index() in execmd.c
//      Note the change in the argument list.
int MPSearchScanIndex(long lIndex);
//      MPSearchMapStation only used internally in Mapper module
int MPSearchMapStation(int iStn, pstMapStn *pstScan);
//      MPMapStationExist returns TRUE if iStn exists
int MPMapStationExist(int iStn);

// Scan Station Parameter Interface functions
//      Get and Set m_iLastPass
int MPGetScanParamLastPass(int iMapStn, int *ipLastPass);
int MPSetScanParamLastPass(int iMapStn, int iLastPass);
//      Get and Set m_plZPos[][]
int MPGetScanParamZPos(int iMapStn, int iIndex1, int iIndex2, long *lpZPos);
int MPSetScanParamZPos(int iMapStn, int iIndex1, int iIndex2, long lZPos);
//      Get and Set m_plROffset[][]
int MPGetScanParamROffset(int iMapStn, int iIndex1, int iIndex2, long *lpROffset);
int MPSetScanParamROffset(int iMapStn, int iIndex1, int iIndex2, long lROffset);
//      Get and Set m_iLoaded[][]
int MPGetScanParamLoaded(int iMapStn, int iIndex1, int iIndex2, int *ipLoaded);
int MPSetScanParamLoaded(int iMapStn, int iIndex1, int iIndex2, int iLoaded);

//      Return station name of the current scan station pointed by pstCurrScan.
int MPGetCurrScanName();
//      pstCurrScan points to the station with iStn name
int MPSetCurrMapStn(int iStnArg);
//      MPGetExtStn() is formerly get_ext_stn() in execute.c
int MPGetExtStn(int iStn, int iWhere);
int MPCompareExtStn(int iStnArg);
int MPCreateAuxScan(int iStnArg);
int MPGetStnSearched(void);
int MPIsScanReady(void);
void MPSetScanReady(int bFlag);
pstMapStn MPGetCurrPstScan(void);
void MPSetCurrPstScan(pstMapStn pstStn);
pstMapStn MPGetPstHeadScan(void);
void MPSetFpHeadScan(pstMapStn pstStn);

#endif
