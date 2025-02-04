/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * Original Author      : Canh Le (Deceased)
 * Current Maintainer   : Georges Sancosme (georges@sancosme.net)
 * Maintained Since     : 13.01.2025
 * Created On           : 04.06.2007
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Program:     Scan mapper Module
 * File:        mapstn.c
 * Functions:   Mapper station functions
 *      int MPInitMapper();
 *      int MPSearchScanIndex();
 *      int MPSearchMapStation();
 *      int MPMapStationExist();
 *      int MPGetScanParamLastPass();
 *      int MPSetScanParamLastPass();
 *      int MPGetScanParamZPos();
 *      int MPSetScanParamZPos();
 *      int MPGetScanParamROffset();
 *      int MPSetScanParamROffset();
 *      int MPGetScanParamLoaded();
 *      int MPSetScanParamLoaded();
 *      int MPGetCurrScanName();
 *      int MPSetCurrMapStn();
 *      int MPGetExtStn();
 *      int MPCompareExtStn();
 *      int MPCreateAuxScan();
 *      int MPGetStnSearched();
 *      int MPIsScanReady();
 *      void MPSetScanReady();
 *      pstMapStn MPGetCurrPstScan();
 *      void MPSetCurrPstScan();
 *      pstMapStn MPGetPstHeadScan();
 *      void MPSetFpHeadScan();
 *
 * Description: Mapper station handling functions.
 *              Scan station is created as PSCAN prepares for scanning.
 *              After filtering the acquired data, the result information
 *              is stored in the scan station structure. Scan station pointers
 *              are linked list. (No need to allocate the memory for all
 *              stations: A - Z, a - z. Only used/requested station is created,
 *              and linked to the list.)
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "sck.h"
#include "mapstn.h"
#include "mapio.h"
#include "ser.h"
#include "scmem.h"

// Globals
// used in scanning preparation
pstMapStn  pstHeadScan;    // head pointer to the scan station linked list
pstMapStn  pstCurrScan;    // pointer to the current station
pstMapStn  pstExtScan;     // pointer to the extended station

int         iStnSearched;   // the current station prepared for scanning
int         bScanReady;     // flag

int         iDefFlags = 0;  // the define flags for the current system configuration
int         iEmulMode = 0;  // the emulation type of the current system configuration

/****************************************************************
 *
 * Function:    MPInitMapper
 *
 * Abstract:    Initialize globals and scanner IO.
 *
 * Parameters:  iDefineFlagArg  - define flags for the current system config
 *              iEmulatorArg    - emulation type of the current system config
 *
 * Returns:     none
 *
 ***************************************************************/
int MPInitMapper(int iDefineFlagArg, int iEmulatorArg)
{
    // Set all pointers to NULL
    pstHeadScan = NULL;
    pstCurrScan = NULL;
    pstExtScan = NULL;

    // no station prepared yet, set scanner ready to take action
    iStnSearched = -1;
    bScanReady = TRUE;

    // copy over the define and emulator flags to the local copies
    iDefFlags = iDefineFlagArg;
    iEmulMode = iEmulatorArg;

    // initialize scanner IO
    if(MPInitScanIO() ==FAILURE)
        return FAILURE;

    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPSearchScanIndex
 *
 * Abstract:    Search the current scan station's slot status at the given
 *              index.
 *
 * Parameters:  lIndexArg    - slot index
 *
 * Return:      FAILURE if slot is empty at the index, or
 *              the slot status at the index
 *
 ***************************************************************/
int MPSearchScanIndex(long lIndexArg)
{
    int  nCount;

    // Index should be <= number of slots
    if (lIndexArg > NOFSLOTS)
        return FAILURE;

    // The current scan station must exist (be prepared).
    if (!pstCurrScan)
        return FAILURE;

    // Search for the slot status in all passes, if found, return it.
    for ( nCount=0; nCount <= pstCurrScan->m_iLastPass; nCount++ )
    {
        if ( pstCurrScan->m_iLoaded[nCount][lIndexArg-1] )
	        return( (int)(lIndexArg-1) );
    }
    return FAILURE;
}

/****************************************************************
 *
 * Function:    MPSearchMapStation
 *
 * Abstract:    Search the scan station link list for given station name.
 *              If the station is found, set the pointer to it.
 *
 * Parameters:  Input:  iStnArg     station name in char
 *              Output: pstScanArg  pointer to the station if found
 *
 * Return:      SUCCESS if found, FAILURE if not.
 *
 ***************************************************************/
int MPSearchMapStation(int iStnArg, pstMapStn *pstScanArg)
{
    pstMapStn pstScan;

    // station name must be alphabetic, A-Z, a-z.
    if (!isalpha(iStnArg))
        return FAILURE;

    // Assign the first pointer and start search
    pstScan = pstHeadScan;
    while ( pstScan )
    {
        // if station found, set the pointer to it, and return
        if (pstScan->m_cStName == iStnArg)
        {
            *pstScanArg = pstScan;
            return SUCCESS;
        }
        pstScan = pstScan->m_pstNextPtr;
    } // while

    return FAILURE;
}

/****************************************************************
 *
 * Function:    MPMapStationExist
 *
 * Abstract:    Search the scan station link list for given station name.
 *
 * Parameters:  iStnArg     station name in char
 *
 * Return:      TRUE if found, FALSE if not.
 *
 ***************************************************************/
int MPMapStationExist(int iStnArg)
{
    pstMapStn pstScan;

    // If failed to find the station, return 0 ***
    if (MPSearchMapStation(iStnArg, &pstScan) == FAILURE)
        return FALSE;
    // If found, return 1 ***
    return TRUE;
}

/****************************************************************
 *
 * Function:    MPGetScanParamLastPass
 *
 * Abstract:    Search for given station, get the station's last pass number.
 *
 * Parameters:  Input:  iStnArg         station name in char
 *              Output: ipLastPassArg   pointer to long to store value
 *
 * Return:      SUCCESS if found, FAILURE if not.
 *
 ***************************************************************/
int MPGetScanParamLastPass(int iStnArg, int *ipLastPassArg)
{
    pstMapStn pstScan;

    // Search for the station, if not found, return failure
    if (MPSearchMapStation(iStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // Station found. Store the last pass number.
    *ipLastPassArg = pstScan->m_iLastPass;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPSetScanParamLastPass
 *
 * Abstract:    Search for given station, set the station's last pass number.
 *
 * Parameters:  iStnArg         station name in char
 *              ipLastPassArg   last pass number
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPSetScanParamLastPass(int iMapStnArg, int iLastPassArg)
{
    pstMapStn pstScan;

    // Search the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // Station found, store the last pass number
    pstScan->m_iLastPass = iLastPassArg;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPGetScanParamZPos
 *
 * Abstract:    Search for given station, get the slot position at the
 *              given slot number and pass number
 *
 * Parameters:
 *      Input:  iMapStnArg     character station name
 *              iIndex1Arg      pass number
 *              iIndex2Arg      slot number
 *      Output: lpZPosArg       pointer to long to store Z Position
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPGetScanParamZPos
(
    int iMapStnArg,
    int iIndex1Arg,
    int iIndex2Arg,
    long *lpZPosArg
)
{
    pstMapStn pstScan;

    // range check for pass number argument
    if (iIndex1Arg >= NOFPASSES || iIndex1Arg < 0)
        return FAILURE;
    // range check for slot number argument
    if (iIndex2Arg >= NOFSLOTS || iIndex2Arg < 0)
        return FAILURE;
    // search for the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // now, store the z position of the slot
    *lpZPosArg = pstScan->m_plZPos[iIndex1Arg][iIndex2Arg];
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPSetScanParamZPos
 *
 * Abstract:    Search for given station, set the slot position at the
 *              given slot number and pass number
 *
 * Parameters:
 *      Input:  iMapStnArg     character station name
 *              iIndex1Arg      pass number
 *              iIndex2Arg      slot number
 *              lpZPosArg       Z Position
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPSetScanParamZPos
(
    int iMapStnArg,
    int iIndex1Arg,
    int iIndex2Arg,
    long lZPosArg
)
{
    pstMapStn pstScan;

    // range check for pass number argument
    if (iIndex1Arg >= NOFPASSES || iIndex1Arg < 0)
        return FAILURE;
    // range check for slot number argument
    if (iIndex2Arg >= NOFSLOTS || iIndex2Arg < 0)
        return FAILURE;
    // search for the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // now, store the z position of the slot
    pstScan->m_plZPos[iIndex1Arg][iIndex2Arg] = lZPosArg;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPSetScanParamROffset
 *
 * Abstract:    Search for given station, set the slot ROffset at the
 *              given slot number and pass number
 *
 * Parameters:
 *      Input:  iMapStnArg     character station name
 *              iIndex1Arg      pass number
 *              iIndex2Arg      slot number
 *              lROffsetArg     ROffset
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPSetScanParamROffset
(
    int iMapStnArg,
    int iIndex1Arg,
    int iIndex2Arg,
    long lROffsetArg
)
{
    pstMapStn pstScan;

    // range check for pass number argument
    if (iIndex1Arg >= NOFPASSES || iIndex1Arg < 0)
        return FAILURE;
    // range check for slot number argument
    if (iIndex2Arg >= (NOFSLOTS*2) || iIndex2Arg < 0)
        return FAILURE;
    // search for the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // now, store the ROffset of the slot
    pstScan->m_plROffset[iIndex1Arg][iIndex2Arg] = lROffsetArg;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPGetScanParamROffset
 *
 * Abstract:    Search for given station, get the slot ROffset at the
 *              given slot number and pass number
 *
 * Parameters:
 *      Input:  iMapStnArg     character station name
 *              iIndex1Arg      pass number
 *              iIndex2Arg      slot number
 *      Output: lpROffsetArg    pointer to long to store value
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPGetScanParamROffset
(
    int  iMapStnArg,
    int  iIndex1Arg,
    int  iIndex2Arg,
    long *lpROffsetArg
)
{
    pstMapStn pstScan;

    // range check for pass number argument
    if (iIndex1Arg >= NOFPASSES || iIndex1Arg < 0)
        return FAILURE;
    // range check for slot number argument
    if (iIndex2Arg >= NOFSLOTS*2 || iIndex2Arg < 0)
        return FAILURE;
    // search for the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // now, store the ROffset of the slot
    *lpROffsetArg = pstScan->m_plROffset[iIndex1Arg][iIndex2Arg];
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPSetScanParamLoaded
 *
 * Abstract:    Search for given station, set the slot status at the
 *              given slot number and pass number
 *
 * Parameters:
 *      Input:  iMapStnArg     character station name
 *              iIndex1Arg      pass number
 *              iIndex2Arg      slot number
 *              iLoaded         slot status
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPSetScanParamLoaded
(
    int iMapStnArg,
    int iIndex1Arg,
    int iIndex2Arg,
    int iLoadedArg
)
{
    pstMapStn pstScan;

    // range check for pass number argument
    if (iIndex1Arg >= NOFPASSES+1 || iIndex1Arg < 0)
        return FAILURE;
    // range check for slot number argument
    if (iIndex2Arg >= NOFSLOTS || iIndex2Arg < 0)
        return FAILURE;
    // search for the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // now, store the slot staus of the slot
    pstScan->m_iLoaded[iIndex1Arg][iIndex2Arg] = iLoadedArg;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPGetScanParamLoaded
 *
 * Abstract:    Search for given station, set the slot status at the
 *              given slot number and pass number
 *
 * Parameters:
 *      Input:  iMapStnArg     character station name
 *              iIndex1Arg      pass number
 *              iIndex2Arg      slot number
 *      Output: *iLoadedArg     slot status
 *
 * Return:      SUCCESS if station found, FAILURE if not.
 *
 ***************************************************************/
int MPGetScanParamLoaded
(
    int iMapStnArg,
    int iIndex1Arg,
    int iIndex2Arg,
    int *ipLoadedArg
)
{
    pstMapStn pstScan;

    // range check for pass number argument
    if (iIndex1Arg >= NOFPASSES+1 || iIndex1Arg < 0)
        return FAILURE;
    // range check for slot number argument
    if (iIndex2Arg >= NOFSLOTS || iIndex2Arg < 0)
        return FAILURE;
    // search for the station, if not found, return failure
    if (MPSearchMapStation(iMapStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // now, store the slot staus of the slot
    *ipLoadedArg = pstScan->m_iLoaded[iIndex1Arg][iIndex2Arg];
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPGetCurrScanName
 *
 * Abstract:    Return station name pointed by pstCurrScan.
 *
 * Parameters:  none
 *
 * Return:      FAILURE if station not found, otherwise,
 *              the station name.
 *
 ***************************************************************/
int MPGetCurrScanName()
{
    if (pstCurrScan)
        return pstCurrScan->m_cStName;
    return FAILURE;
}

/****************************************************************
 *
 * Function:    MPSetCurrMapStn
 *
 * Abstract:    Search for the given station name, and set pstCurrScan to the
 *              station pointer if found.
 *
 * Parameters:  iStnArg     station name to be searched
 *
 * Return:      FAILURE if station not found, otherwise,
 *              SUCCESS.
 *
 ***************************************************************/
int MPSetCurrMapStn(int iStnArg)
{
    pstMapStn pstScan;

    // Search for the given station name, if not found, return failure.
    if (MPSearchMapStation(iStnArg, &pstScan) == FAILURE)
        return FAILURE;

    // station found. set the pointer.
    pstCurrScan = pstScan;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPGetExtStn
 *
 * Abstract:    Checks for iStnArg validity, sets iStnSearched, and
 *              search for the station name. If not found, get the last
 *              station pointer and set the return station name.
 *
 *              Station indices are as follows:
 *              A -> Z   (ASCII 65 -> 90)  have indices  0 -> 25
 *              a -> z   (ASCII 97 -> 122) have indices 26 -> 51
 *
 * Parameters:
 *      Input:  iStnArg    station name in char
 *              iWhereToPoint   0: pointer pstExtScan
 *                              1: pointer pstCurrScan
 *
 * Return:      1   if invalid argument
 *              2   if no station has been scanned (Null pstHeadScan)
 *              3   if station not found, but pointer set to the last station
 *              0   if station found, pointer set the the station
 *
 ***************************************************************/
int MPGetExtStn(int iStnArg, int iWhereToPoint)
{
    pstMapStn pstScan, pstPrevScan;

    // station name must be alphabetic
    if (!isalpha(iStnArg))
        return 1;

    // Set the current station
    iStnSearched = iStnArg;

    // Start search
    pstScan = pstHeadScan;
    if ( pstScan )
    {
        // loop ends if station is found, or
        // end of list is reached.
	    while( pstScan->m_cStName != iStnArg )
        {
	        pstPrevScan = pstScan;
	        pstScan = pstScan->m_pstNextPtr;
            // if the end of list reached, set to the last station
	        if( !pstScan )
            {
		        pstScan = pstPrevScan;
		        break;
	        }
	    }
    }

    // Set the pointer to depending input.
    if (iWhereToPoint == 0)
        pstExtScan = pstScan;
    else
        pstCurrScan = pstScan;

    // if pointer has been set to a station, check if it's the same station
    // as the requested station name
    if (pstScan)
    {
        if (pstScan->m_cStName == iStnArg)
            return 0;   // pointer is set to the requested station
        else
            return 3;   // pointer is set to the last station, it's not the
                        // requested station
    }

    return 2;           // station head pointer is null -- no station has been prepared.
}

/****************************************************************
 *
 * Function:    MPCompareExtStn
 *
 * Abstract:    Compare pstExtStn's station name with iStnArg.
 *
 * Parameters:  iStnArg     station name to be searched
 *
 * Return:      TRUE if pstExtStn name equals iStnArg, otherwise,
 *              FALSE.
 *
 ***************************************************************/
int MPCompareExtStn(int iStnArg)
{

    // station name must be alphabetic
    if (!isalpha(iStnArg))
        return FALSE;

    // Set the global, last searched station
    iStnSearched = iStnArg;

    // if pstExtStn not exist, return FALSE
    if (!pstExtScan)
        return FALSE;
    // if pstExtStn name does not match, return FALSE
    if (pstExtScan->m_cStName != iStnArg)
        return FALSE;

    return TRUE;
}

/****************************************************************
 *
 * Function:    MPCreateAuxScan
 *
 * Abstract:    Create Scan station if it's not already created.
 *
 * Parameters:  iStnArg     station name to be created
 *
 * Return:      SUCCESS
 *              FAILURE if invalid argument or malloc error.
 *
 ***************************************************************/
int MPCreateAuxScan(int iStnArg)
{
    pstMapStn pstScanAux;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    // station name must be alphabetic
    if (!isalpha(iStnArg))
        return FAILURE;

    // search for the requested station in the defined station list
    // if extended station not exist, or the requested station does not exist
    // then create one.
    if(!pstExtScan || pstExtScan->m_cStName != iStnArg)
    {
        /* if memory low, do not proceed to allocate data buffer */
        if (HPGetMemLowFlag())
            return FAILURE;

        // Station never been scanned, so create one
        pstScanAux = (pstMapStn) malloc(sizeof(stMapStn));
        if (!pstScanAux)
        {
#ifdef DEBUG
            COPutsTxBuff(iCmdPort, "****MPCreateAuxScan: out of memory.***\n\r?");
#endif
            return FAILURE;
        }

        pstScanAux->m_cStName = iStnArg;
        pstScanAux->m_pstNextPtr = NULL;

        if (pstHeadScan)
            pstExtScan->m_pstNextPtr = pstScanAux;
        else
            pstHeadScan = pstScanAux;

        pstExtScan = pstScanAux;
    }

    return SUCCESS;

}

/****************************************************************
 *
 * Function:    MPGetStnSearched
 *
 * Abstract:    Returns a station searched name saved as global
 *
 * Parameters:  none
 *
 * Return:      iStnSearched
 *
 ***************************************************************/
int MPGetStnSearched(void)
{
    return iStnSearched;
}

/****************************************************************
 *
 * Function:    MPIsScanReady
 *
 * Abstract:    Returns a global flag bScanReady.
 *
 * Parameters:  none
 *
 * Return:      bScanReady
 *
 ***************************************************************/
int MPIsScanReady(void)
{
    return bScanReady;
}

/****************************************************************
 *
 * Function:    MPSetScanReady
 *
 * Abstract:    Sets a global flag bScanReady.
 *
 * Parameters:  bScanReady
 *
 * Return:      none
 *
 ***************************************************************/
void MPSetScanReady(int bFlagArg)
{
    bScanReady = bFlagArg;
}

/****************************************************************
 *
 * Function:    MPGetCurrPstScan
 *
 * Abstract:    Returns a global pointer pstCurrScan.
 *
 * Parameters:  none
 *
 * Return:      pstCurrScan
 *
 ***************************************************************/
pstMapStn MPGetCurrPstScan(void)
{
    return pstCurrScan;
}

/****************************************************************
 *
 * Function:    MPSetCurrPstScan
 *
 * Abstract:    Sets a global pointer pstCurrScan.
 *
 * Parameters:  pstScanArg      Scan station pointer
 *
 * Return:      none
 *
 ***************************************************************/
void MPSetCurrPstScan(pstMapStn pstScanArg)
{
    pstCurrScan = pstScanArg;
}

/****************************************************************
 *
 * Function:    MPGetPstHeadScan
 *
 * Abstract:    Returns a global pointer pstHeadScan.
 *
 * Parameters:  none
 *
 * Return:      pstHeadScan
 *
 ***************************************************************/
pstMapStn MPGetPstHeadScan(void)
{
    return pstHeadScan;
}

/****************************************************************
 *
 * Function:    MPSetFpHeadScan
 *
 * Abstract:    Sets a global pointer pstHeadScan.
 *
 * Parameters:  pstScanArg
 *
 * Return:      none
 *
 ***************************************************************/
void MPSetFpHeadScan(pstMapStn pstScanArg)
{
    pstHeadScan = pstScanArg;
}
