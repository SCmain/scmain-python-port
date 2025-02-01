<<<<<<< HEAD
/******************************************************************************\
=======
/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * Original Author      : (Deceased)
 * Current Maintainer   : gsancosme (georges@sancosme.net)
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
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 *
 * Program:     Scan mapper Module
 * File:        map.c
 * Functions:   Cassette mapping scanner functions
 *      int  MPDoScan();
 *      void MPFilterPass0(); NOT used currently
 *      void MPFilterPass1(); NOT used currently
 *      void MPFilterPass2(); NOT used currently
 *      void MPFilterPass3(); Current algorithm
 *
 * Description: Cassette mapping scanner functions
 *              MPDoScan() is called by ex_PSCAN() and prepares the data buffer
 *              and starts the interrupt.
 *              Scan Macro will move Z-axis, and during Z-axis motion data are
 *              collected in the buffer. When Z motion completes, Scan Macro
 *              issues QSCAN. ex_QSCAN() disables the interrupt.
 *              After completed scanning for two passes, Macro issues FILTR.
 *              FilterPassn() is called by ex_FILTR(). MPFilterPass3() calls the
 *              current (most updated) filtering algorithm MPNewScanFilter().
 *              ex_RROP() can be called any time to retrieve the data acquired.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#include <math.h>
#include <memory.h>
#include <malloc.h>
<<<<<<< HEAD
#include <string.h>
=======
#include <string.h>
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include <stdlib.h>

#include "sck.h"
#include "ser.h"
#include "fiog.h"
#include "mapio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
<<<<<<< HEAD
#include "scstat.h"
=======
#include "scstat.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include "otf.h"

// Globals
//      to retain values that are to be returned to
//      other modules for various purposes:
//      for example, raw data and computed results are
//      to be retained and to be passed to the calling
//      modules.
//
//  used in scanning preparation
extern pstMapStn pstExtScan;
extern pstMapStn pstCurrScan;
extern long lpPass1Pos[SCAN_ARRAY_SIZE * 4];
extern long lpPass2Pos[SCAN_ARRAY_SIZE * 4];
extern int  ipPass1Trans[SCAN_ARRAY_SIZE * 4];
extern int  ipPass2Trans[SCAN_ARRAY_SIZE * 4];
extern int iHitCount[2];
extern int iMapperAxis;

/****************************************************************
 *
 * Function:    MPDoScan
 *
 *
 * Parameters:  iPassNumArg - pass number
 *              iScOnWhichDev - which device the scanner is mounted on:
 *                                  robot, vacuum indexer r, or vacuum indexer z
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPDoScan(int iPassNumArg, int iScOnWhichDev)
{
    // pass number can be 0..1
    if ( iPassNumArg < 0 || iPassNumArg >= NOFPASSES )
        return FAILURE;

    // extended scan station should be created prior (in PSCAN)
    if (!pstExtScan)
        return FAILURE;

    pstCurrScan = pstExtScan;

    // if first time, allocate memory for the data buffers
    if (MPAllocRawData() == FAILURE)
        return FAILURE;

    // set the buffer pointer for each different pass
    if ( iPassNumArg )
    {
        iHitCount[1] = 1;
        MPSetScanPointers(&iHitCount[1], lpPass2Pos, ipPass2Trans);
    }
    else
    {
        iHitCount[0] = 1;
        MPSetScanPointers(&iHitCount[0], lpPass1Pos, ipPass1Trans);
    }

    // initialize the data buffers
    MPInitScanDataBuffer();

    // following 2 lines of code are mysterious!!!
    SSGetGENSystemStatus( FALSE );
    SSGetSystemStatus( );
    //
    //
    // record the axis the scanner is on
    iMapperAxis = iScOnWhichDev;
    return( MPEnableWES() );
}

/****************************************************************
 *
 * Function:    MPFilterPass0
 *
 * Abstract:    Filter 0 algorithm: NOT Used currently, but
 *              it's there for backward compatibility.
 *
 * Parameters:  iStnArg     - scan station
 *              iMaxThArg   - ???
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPFilterPass0( int iStnArg, int iMaxThArg )
{
    pstMapStn pstScan;
    int index, iTemp;
    int iCarryOver;
    int *ipDNslot;
    int *ipUPslot;

    if (MPSearchMapStation(iStnArg, &pstScan))
        return FAILURE;

    iCarryOver = 0;
    ipDNslot = pstScan->m_iLoaded[1];
    ipUPslot = pstScan->m_iLoaded[0];

    for (index=0; index<NOFSLOTS; index++)
    {
        if (iCarryOver != 0)
        {
            if (ipDNslot[index] == 0)
            {
                ipDNslot[index] = iCarryOver;
                iCarryOver = 0;
            }
            else
            {
                iTemp = ipDNslot[index];
                ipDNslot[index] = iCarryOver;
                iCarryOver = iTemp;
                continue;
             } // if ipDNslot==0 else
        } // if iCarryOver!=0

        if ((ipUPslot[index] == 0) && (ipDNslot[index] == 0))      /* <0..0> */
            continue;

        if ((ipUPslot[index] != 0) && (ipDNslot[index] != 0))    /* <1  1> */
        {
            if(ipUPslot[index] < (ipDNslot[index] - iMaxThArg)) /* <1..0> */
            {
                iCarryOver = ipDNslot[index];
                ipDNslot[index] = 0;
                continue;
            }
            else
                continue;             /* <1..1> */
        }

        if  ( index+1 >= NOFSLOTS )
            continue;

        if (ipUPslot[index] != 0)                 /* <1  0> */
        {
            if (ipDNslot[index+1] < (ipUPslot[index] + iMaxThArg)) /* <1..2> */
            {
                ipDNslot[index] = ipDNslot[index+1];
                ipDNslot[index+1] = 0;
                continue;
            }
            else
                continue;           /* <1..0> */
        }
        else                                            /* <0  1> */
        {
            if ((ipUPslot[index+1] > (ipDNslot[index] - iMaxThArg))&&   /* <2..1> */
                (ipUPslot[index+1] < (ipDNslot[index] + 5)) )
            {
                ipUPslot[index] = ipUPslot[index+1];
                ipUPslot[index+1] = 0;
                continue;
            }
            else                                    /* <0..1> */
                continue;
        }
    } // for loop

    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPFilterPass1
 *
 * Abstract:    Filter 1 algorithm: NOT Used currently, but
 *              it's there for backward compatibility.
 *
 * Parameters:  iStnArg     - scan station
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPFilterPass1( int iStnArg )
{
    pstMapStn pstScan;
    long    lNofSlot;
    int     nPass;
    int     iStn;
    long    lTempSlot;
    long    lTemp;
    long    lSlot;
    long    lAccLow, lAccHigh;

    // get the station name index
    if (MPSearchMapStation(iStnArg, &pstScan))
        return FAILURE;

    iStn = (pstScan->m_cStName < 'a') ?
        (pstScan->m_cStName - 'A') : (pstScan->m_cStName - 'a' + 26);

    if( FIOGetStnVals(iStn,ACC_LOW,&lAccLow) != SUCCESS )
        return FAILURE;

    if( FIOGetStnVals(iStn,ACC_HIGH,&lAccHigh) != SUCCESS )
        return FAILURE;

    for ( nPass=0; nPass<2; nPass++ )
    {
        for (lNofSlot=pstScan->m_plROffset[nPass][49]-1; lNofSlot>=0; lNofSlot-- )
        {
            lTempSlot = pstScan->m_plROffset[nPass][lNofSlot] % 100L;
            if ( lTempSlot <= lAccLow || lTempSlot >= lAccHigh)
            {
                lTemp = pstScan->m_plROffset[nPass][lNofSlot];
                if( lTempSlot < lAccLow )
                    pstScan->m_plROffset[nPass][lNofSlot] -= lTempSlot;
                else
                    pstScan->m_plROffset[nPass][lNofSlot] += (100 - lTempSlot);
                lSlot = pstScan->m_plROffset[nPass][lNofSlot] / 100 - 1;
                if ( !pstScan->m_plZPos[nPass][lSlot] )
                    pstScan->m_plZPos[nPass][lSlot] = lTemp;
            } // if lTempSlot
        } // for lnofSlot loop
    }
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPFilterPass2
 *
 * Abstract:    Filter 2 algorithm: NOT Used currently, but
 *              it's there for backward compatibility.
 *
 * Parameters:  iStnArg     - scan station
 *              iPassNumArg - pass number
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPFilterPass2(int iStnArg, int iPassNumArg)
{
    long lSlot0, lSlotP0, lSlotP1;
    long nSlot, lAveThick;
    int  *fpiLoadedRes;
    int  iStn;
    long lPitch;
    long lCSLow, lCSHigh;
    pstMapStn pstScan;

    // get the station name index
    if (MPSearchMapStation(iStnArg, &pstScan))
        return FAILURE;

    iStn = (pstScan->m_cStName < 'a') ?
        (pstScan->m_cStName - 'A') : (pstScan->m_cStName - 'a' + 26);

    // lAveThick = p_stn->ave_thick * p_stn->pitch;

    if( FIOGetStnVals(iStn,AVE_THICK,&lAveThick) != SUCCESS )
        return FAILURE;

    if( FIOGetStnVals(iStn,PITCH,&lPitch) != SUCCESS )
        return FAILURE;

    // The following change was made to adapt a micron metric to a
    // 10,000ths of an inch standard. It's special for Brooks/Novellus emulation.
    if (iEmulMode & DFEMULATORB)
    {
        lPitch *= 10;
    }
    if( FIOGetStnVals(iStn,CROSS_SLOT_LOW,&lCSLow) != SUCCESS )
        return FAILURE;
    if( FIOGetStnVals(iStn,CROSS_SLOT_HIGH,&lCSLow) != SUCCESS )
        return FAILURE;

    if ( iPassNumArg )
        fpiLoadedRes = pstScan->m_iLoaded[3];
    else
        fpiLoadedRes = pstScan->m_iLoaded[2];
    memset( fpiLoadedRes, 0, (size_t)(sizeof(int) * NOFSLOTS));

    for ( nSlot=0; nSlot<NOFSLOTS; nSlot++ )
    {
        if ( pstScan->m_iLoaded[0][nSlot] &&     // Both sides have info
            pstScan->m_iLoaded[1][nSlot] )
        {
            lSlot0 = labs( pstScan->m_plZPos[0][nSlot] -
                        (pstScan->m_plZPos[1][nSlot] - lAveThick));
            lSlot0 /= lPitch;
            lSlotP1 = (pstScan->m_iLoaded[0][nSlot] +
	                    pstScan->m_iLoaded[1][nSlot]) / 2;
            lSlotP0 = lSlotP1%100L;
            if ( (lSlotP0 > lCSLow && lSlotP0 < lCSHigh) )
                fpiLoadedRes[nSlot] = 2; // Both say Xslotted
            else
                fpiLoadedRes[nSlot] = 1;
        }
        else
        {                    // Only one is good
            if ( pstScan->m_iLoaded[0][nSlot] )
                lSlot0 = pstScan->m_iLoaded[0][nSlot] % 100L;
            else if( pstScan->m_iLoaded[1][nSlot] )
                lSlot0 = pstScan->m_iLoaded[1][nSlot] % 100L;
            else                   // Both 0's
                continue;
            if ( lSlot0 <= lCSLow || lSlot0 >= lCSHigh )
                fpiLoadedRes[nSlot] = 1; // Sure not Xslotted
            else
                fpiLoadedRes[nSlot] = 3; // Not sure 50/50
        }
    }

    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPFilterPass3
 *
 * Abstract:    Filter 3 algorithm: This routine calls NewScanFilter()
 *              which is the current filtering algorithm.
 *
 * Parameters:  iStnArg         - scan station
 *              bCalibArg       - Teach Scan Station flag
 *              iIterArg        - pass number 0..4
 *              iNewSchemeArg   - new vac inx scanning scheme
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPFilterPass3( int iStnArg, int bCalibArg, int iIterArg, int iNewSchemeArg )
{

    int  iStn;
    long lPitch;
    long lCSLow, lCSHigh;
    long lAccLow, lAccHigh;
    long lAveThick;
    long lScsi0, lScsi1;
    pstMapStn pstScan;

    // get the station name index
    if (MPSearchMapStation(iStnArg, &pstScan))
        return FAILURE;

    // save the raw data currently acquired
    // the current data buffer will be overwritten by filtering routines.
    if (MPSaveRawData() == FAILURE)
        return FAILURE;

    iStn = (pstScan->m_cStName < 'a') ?
        (pstScan->m_cStName - 'A') : (pstScan->m_cStName - 'a' + 26);

    // Get filtering parameters (thresholds & other info)
    if( FIOGetStnVals(iStn,PITCH,&lPitch) != SUCCESS )
        return FAILURE;
    // The following change was made to adapt a micron metric to a
    // 10,000ths of an inch standard. It's special for Brooks/Novellus emulation.
    if (iEmulMode & DFEMULATORB)
    {
        lPitch *= 10;
    }
    if( FIOGetStnVals(iStn,CROSS_SLOT_LOW,&lCSLow) != SUCCESS )
        return FAILURE;
    if( FIOGetStnVals(iStn,CROSS_SLOT_HIGH,&lCSHigh) != SUCCESS )
        return FAILURE;
    if( FIOGetStnVals(iStn,ACC_LOW,&lAccLow) != SUCCESS )
        return FAILURE;
    if( FIOGetStnVals(iStn,ACC_HIGH,&lAccHigh) != SUCCESS )
        return FAILURE;
    if( FIOGetStnVals(iStn,AVE_THICK,&lAveThick) != SUCCESS )
        return FAILURE;

    FIOGetStnVals(iStn, TAUGHT_POS_TO_WAFER, &lScsi0);
    FIOGetStnVals(iStn, CROSS_SLOT_THRESH, &lScsi1);

    // CSLow is lDiffPassThreshold and must be > 0, usuall ~800
    // CSHigh is lGapThreshold and must be > 0, usually ~5000
    if(lCSLow == 0 || lCSHigh == 0 || lPitch == 0)
        return FAILURE;

    // Set the parameters: the routine converts them to values compatible with
    // Z-axis position counts acquired by MPReadLatchedPosition() function.
    MPSetNSFPars( lPitch, lCSLow, lCSHigh, lAccHigh, lAveThick, lScsi1);

    // Now call the filter routine.
    // If pass number is not 0..3, default it to 0.
    if ( iIterArg>0 && iIterArg<4 )
    {
	    lAccLow = MPNewScanFilter(pstScan->m_plZPos[0],
                    pstScan->m_iLoaded[iIterArg], bCalibArg, &lScsi0, iNewSchemeArg );
        if( FIOSetStnVals(iStn,ACC_LOW,lAccLow) != SUCCESS )
            return FAILURE;

        FIOSetStnVals(iStn, TAUGHT_POS_TO_WAFER, lScsi0);
    }
    else
    {
	    lAccLow = MPNewScanFilter(pstScan->m_plZPos[0],
                    pstScan->m_iLoaded[0], bCalibArg, &lScsi0, iNewSchemeArg );
        if( FIOSetStnVals(iStn,ACC_LOW,lAccLow) != SUCCESS )
            return FAILURE;
        FIOSetStnVals(iStn, TAUGHT_POS_TO_WAFER, lScsi0);
    }


    if ( bCalibArg )
    {
        if( FIOSetStnVals(iStn,ACC_HIGH,MPGetRefPoint()) != SUCCESS )
            return FAILURE;
    }
    return SUCCESS;

}

/****************************************************************
 *
 * Function:    MPFilterPass4
 *
 * Abstract:    Filter 4 algorithm: This routine calls aligning
 *              algorithm for OTF
 *
 * Parameters:
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPFilterPass4( int iStationIndexArg, int iCalibArg, long lTorgArg, char cCalStationArg, int iIOShift )
<<<<<<< HEAD
{ 
=======
{ 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    int iRet;
    long    lPosition[8]={0,0,0,0,0, 0, 0, 0};
    int  iStn;

    iStn = (iStationIndexArg < 'a') ? (iStationIndexArg - 'A') : (iStationIndexArg - 'a' + 26);

    // Get coordinates T and R
    if (FIOGetStnCoord(iStn, 0, &lPosition[0]) == FAILURE)
        return FAILURE;
    if (FIOGetStnCoord(iStn, 1, &lPosition[1]) == FAILURE)
        return FAILURE;

    iStn = (cCalStationArg < 'a') ? (cCalStationArg - 'A') : (cCalStationArg - 'a' + 26);
    iRet = MPAligning((double)lPosition[0], (double)lPosition[1], (double)lTorgArg,
                iCalibArg, cCalStationArg, iIOShift);

    return iRet;
}
