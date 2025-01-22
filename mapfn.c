/******************************************************************************\
 *
 * Program:     Scan mapper Module
 * File:        mapfn.c
 * Functions:   Mapper filter functions
 *      int  MPAllocRawData();
 *      int  MPGetHitCount();
 *      int  MPSaveRawData();
 *      long MPReadTempData();
 *      int  MPLoadTempData();
 *      void MPSetNSFPars();
 *      long MPGetRefPoint();
 *      long MPNewScanFilter();
 *      int  MPRemoveMultiple();
 *      long MPCheckSlotStatus();
 *
 * Description: This is FilterPass3 Algorithm.
 *      The algorithm filters the raw data in 4 major steps:
 *      (1) Remove 0-thickness hits
 *      (2) Remove multiple hits
 *      (3) Corrects two-pass data
 *      (4) Check and determine the slot status
 *
 *      Definitions:
 *      "hit" is a paired data of UP and DOWN, where UP is the beginning edge
 *          of the detected wafer, and DOWN is the ending edge.
 *      "0-thickness" is a hit where UP & DOWN position is equal
 *      "multiple hits" are two or more hits per a wafer
 *
 *      Possible situations in the raw data:
 *      (1) In a scan pass, 0-thickness hits can occur.
 *      (2) In a scan pass, more than a hit can be generated for a wafer.
 *      (3) When comparing two passes, one pass data may be different more than
 *          threshold from another pass data. (In the future algorithms, this
 *          situation will be handled differently.)
 *      (4) The wafer's computed real middle position does not fall into the
 *          expected range of position.
 *      (5) The wafer's computed thickness is greater than the expected range.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "sck.h"
#include "ser.h"
#include "mapk.h"
#include "mapfn.h"
#include "scmem.h"

// Globals
//      to retain values that are to be returned to
//      other modules for various purposes:
//      for example, raw data and computed results are
//      to be retained and to be passed to the calling
//      modules.
//
//  used in filtering algorithm: MPNewScanFilter
long  lPITCH = 250000;      // station Pitch, default to .25 inch
long  lDiffPassThreshold;   // difference between 2 passes
long  lGapThreshold;        // for multiple hits detection: gap between multiple hits
long  lRefPoint;            // Save this after cal.
float fThickThreshold;      // for double wafer detection
float fLocalCrossSlotRatio; // for cross slot detection
float fCROSS_SLOT_TRSHLD_L;
// CK: 11142000 XSlot ZAngle Diff Threshold, if the user wants to, the recommended value is 15000. 
long  lLiftUpXSlotDiffThreshold = 99999;  // default is high, so it doesn't check for lifted-up wafers

//  scan input data buffer pointers
long  lpPass1Pos[SCAN_ARRAY_SIZE * 4];
long  lpPass2Pos[SCAN_ARRAY_SIZE * 4];
int   ipPass1Trans[SCAN_ARRAY_SIZE * 4];
int   ipPass2Trans[SCAN_ARRAY_SIZE * 4];
//long *lpPass1Pos;   // pass 1 Z position data buffer
//long *lpPass2Pos;   // pass 2 Z position data buffer
//int  *ipPass1Trans; // pass 1 Trans data buffer
//int  *ipPass2Trans; // pass 2 Trans data buffer
int         iHitCount[2];   // pass 1 & 2  counter
int         bInitRawData = FALSE;

//  save data buffer pointers
long  lpSave1Pos[SCAN_ARRAY_SIZE * 4];
long  lpSave2Pos[SCAN_ARRAY_SIZE * 4];
int   ipSave1Trans[SCAN_ARRAY_SIZE * 4];
int   ipSave2Trans[SCAN_ARRAY_SIZE * 4];
//long *lpSave1Pos;   // pass 1 Z position raw data buffer
//long *lpSave2Pos;   // pass 2 Z position raw data buffer
//int  *ipSave1Trans; // pass 1 Trans raw data buffer
//int  *ipSave2Trans; // pass 2 Trans raw data buffer
int          iSaveCount[2]; // pass 1 & 2 counter
int          bInitSaveBuffer = FALSE;

//  scan filter calculated intermediate buffer pointers
long  lpWafThick1[(HALF_SCAN_ARRAY+2) * 4];
long  lpWafThick2[(HALF_SCAN_ARRAY+2) * 4];
long  lpWafPos1[(HALF_SCAN_ARRAY+2) * 4];
long  lpWafPos2[(HALF_SCAN_ARRAY+2) * 4];
long  lpSlotPitch1[(HALF_SCAN_ARRAY+2) * 4];
long  lpSlotPitch2[(HALF_SCAN_ARRAY+2) * 4];
//long *lpWafThick1;  // pass 1 wafer thickness data buffer
//long *lpWafThick2;  // pass 2 wafer thickness data buffer
//long *lpWafPos1;    // pass 1 wafer position data buffer
//long *lpWafPos2;    // pass 2 wafer position data buffer
//long *lpSlotPitch1; // pass 1 slot pitch data buffer
//long *lpSlotPitch2; // pass 2 slot pitch data buffer
char  cpBad[HALF_SCAN_ARRAY+2];
//char *cpBad;        // Bad slot data buffer
long *lpPosFinal;   // Final Position result data buffer pointer
int  *ipSlotFinal;  // Final Slot status data buffer pointer

/****************************************************************
 *
 * Function:    MPGetLiftUpXSlotDiffThreshold
 *
 * Returns:     lLiftUpXSlotDiffThreshold
 *
 ***************************************************************/
long MPGetLiftUpXSlotDiffThreshold(void)
{
    return lLiftUpXSlotDiffThreshold;
}
/****************************************************************
 *
 * Function:    MPSetLiftUpXSlotDiffThreshold
 *
 * Abstract:    Sets lLiftUpXSlotDiffThreshold
 * Returns:     none
 *
 ***************************************************************/
void MPSetLiftUpXSlotDiffThreshold(long lValueArg)
{
    lLiftUpXSlotDiffThreshold = lValueArg;
}
/****************************************************************
 *
 * Function:    MPAllocRawData
 *
 * Abstract:    Memory allocation for all data buffers.
 *
 * Parameters:  none
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPAllocRawData( )
{
    bInitRawData = TRUE;
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPGetHitCount
 *
 * Abstract:    Return iHitCount
 *
 * Parameters:  iIndexArg   - 0=first pass count, 1=second pass count
 *
 * Returns:     iHitCount[0 or 1]
 *
 ***************************************************************/
int MPGetHitCount(int iIndexArg)
{
    if ( iIndexArg == 0 || iIndexArg == 1)
    {
	    return (iHitCount[iIndexArg]);
    }
    return FAILURE;
}

/****************************************************************
 *
 * Function:    MPSaveRawData
 *
 * Abstract:    Save the raw data into raw data buffer. When called first time,
 *              it allocates the memory for the data to be saved.
 *
 * Parameters:  none
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPSaveRawData( )
{
    int iCount,iCmdPort;

    iCmdPort = SERGetCmdPort();

    // Save pass 1 data only if memory is already allocated
    iSaveCount[0] = iHitCount[0];
    for (iCount = 0; iCount <= iSaveCount[0]; iCount++)
    {
        lpSave1Pos[iCount] = lpPass1Pos[iCount];
        ipSave1Trans[iCount] = ipPass1Trans[iCount];
    }
    // Save pass 2 data only if memory is already allocated
    iSaveCount[1] = iHitCount[1];
    for (iCount = 0; iCount <= iSaveCount[1]; iCount++)
    {
        lpSave2Pos[iCount] = lpPass2Pos[iCount];
        ipSave2Trans[iCount] = ipPass2Trans[iCount];
    }
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPReadTempData
 *
 * Abstract:    RROP calls this function to retrieve the data.
 *          RROP X, Y, Z
 *          X = A   - Wafer Thickness
 *              B   - Wafer Position
 *              C   - Pitch at this wafer
 *              D   - Z position
 *              E   - Trans data (UP or DOWN: 2 or 0)
 *              F   -  counter
 *              G   - Z position raw data
 *              H   - Trans raw data
 *              I   -  counter raw data
 *              J   - Final Z position of wafer
 *              K   - Final slot status of wafer
 *          Y = 0   - first pass
 *              1   - second pass
 *          Z = Index to the data buffer (array)
 *
 * Parameters:  iTypeArg    - X: type of return value: A to I
 *              iPassNumArg - Y: pass number: 0 to 1
 *              iIndexArg   - Z: index to the data array
 *
 * Returns:     value of the data requested
 *
 ***************************************************************/
long MPReadTempData( int iTypeArg, int iPassNumArg, int iIndexArg )
{
    switch( iTypeArg )
    {
        // Wafer thickness
        case 'A':
            if( iPassNumArg )
                return (lpWafThick2[iIndexArg]);
            else
                return (lpWafThick1[iIndexArg]);
            break;
        // Wafer position
        case 'B':
            if( iPassNumArg )
                return (lpWafPos2[iIndexArg]);
            else
                return (lpWafPos1[iIndexArg]);
            break;
        // Slot pitch at the wafer
        case 'C':
            if( iPassNumArg )
                return (lpSlotPitch2[iIndexArg]);
            else
                return (lpSlotPitch1[iIndexArg]);
            break;
        // Z-position, Intermediate data
        case 'D':
            if( iPassNumArg )
                return (lpPass2Pos[iIndexArg]);
            else
                return (lpPass1Pos[iIndexArg]);
            break;
        // Trans buffer, Intermediate data
        case 'E':
            if( iPassNumArg )
                return( (long)ipPass2Trans[iIndexArg] );
            else
                return( (long)ipPass1Trans[iIndexArg] );
            break;
        // counter
        case 'F':
            if( iPassNumArg )
                return( (long) MPGetHitCount(1) );
            else
                return( (long) MPGetHitCount(0) );
        // Z-position, Raw data
        case 'G':
            if( iPassNumArg )
                return (lpSave2Pos[iIndexArg]);
            else
                return (lpSave1Pos[iIndexArg]);
            break;
        // Trans buffer, Raw data
        case 'H':
            if( iPassNumArg )
                return( (long)ipSave2Trans[iIndexArg] );
            else
                return( (long)ipSave1Trans[iIndexArg] );
            break;
        // counter, Raw data
        case 'I':
            if( iPassNumArg )
                return( (long) iSaveCount[1] );
            else
                return( (long) iSaveCount[0] );
            break;
        // Computed wafer's final position result
        case 'J':
                return( lpPosFinal[iIndexArg] );
            break;
        // Computed wafer's final slot status
        case 'K':
                return( (long) ipSlotFinal[iIndexArg] );
            break;
    } // switch

    return FAILURE;
}

/****************************************************************
 *
 * Function:    MPLoadTempData
 *
 * Abstract:    LLOP calls this function to load the test data into buffers.
 *              This is exactly reverse of MPReadTempData.
 *          LLOP N, X1, Y1, X2, Y2
 *          N = 0       -  counter
 *            = 1..256  - Index to the array
 *          X1, Y1      - Z position Data for pass 1 if N>0
 *                         counter for pass 1 & 2 if N=0
 *          X2, Y2      - Z position Data for pass 2 if N>0
 *                        Not used if N=0
 *
 * Parameters:  iIndexArg = N as above
 *              lPass1Arg = X1 as above
 *              iTrans1Arg = Y1 as above
 *              lPass2Arg = X2 as above
 *              iTrans2Arg = Y2 as above
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int MPLoadTempData
(
    int iIndexArg,
    long lPass1Arg,
    int iTrans1Arg,
    long lPass2Arg,
    int iTrans2Arg
)
{
    // if index is out of range, return failure
    if (iIndexArg < 0 || iIndexArg >= SCAN_ARRAY_SIZE)
        return FAILURE;
    // try to allocate memory, if returned failure, return failure
    if (MPAllocRawData() == FAILURE)
        return FAILURE;

    // if iIndex is 0, load the next 2 values into counter array
    if (iIndexArg == 0)
    {
        iHitCount[0] = iTrans1Arg;
        iHitCount[1] = iTrans2Arg;
    }
    else
    {   // otherwise, load the next 4 values into data buffers
        lpPass1Pos[iIndexArg] = lPass1Arg;
        ipPass1Trans[iIndexArg] = iTrans1Arg;
        lpPass2Pos[iIndexArg] = lPass2Arg;
        ipPass2Trans[iIndexArg] = iTrans2Arg;
    }
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    MPSetNSFPars
 *
 * Abstract:    Set Filtering parameters/thresholds, and initialize data buffers
 *              when called first time.
 *
 * Parameters:  lPitchArg       - scan station pitch                (Pitch:    RPI)
 *              lDiffPassArg    - Difference in passes threshold    (CSLow:    RCSI 2)
 *              lGapArg         - Gap between hits threshold        (CSHigh:   RCSI 3)
 *              lRefPointArg    - Reference point                   (AccHigh:  RCSI 5)
 *              lThickArg       - Wafer thickness threshold         (AveThick: RCSI 6)
 *              lCrossRatioArg  - Cross slot ratio threshold        (lscsi1:   RCSI 8)
 *
 * Returns:     none
 *
 ***************************************************************/
void MPSetNSFPars
(
    long lPitchArg,
    long lDiffPassArg,
    long lGapArg,
    long lRefPointArg,
    long lThickArg,
    long lCrossRatioArg
)
{
    float fTemp;

    // convert pitch to the same unit as Z position data unit
    // NOTE: Z position data unit is defined in MPReadLatchedPosition()
    lPITCH = lPitchArg * 100L;

    // lDiffPassThreshold is relative to lPITCH:
    // larger the lDiffPassArg (CSLow), lower the lDiffPassThreshold.
    // In the future: this threshold will be redefined in the filter.
    // Currently the range of CSLow from 1 to 100000 doesn't make much
    // difference in the filtering process.
    fTemp = (float)lDiffPassArg / (float)1000;
    lDiffPassThreshold = (long)( (float)lPITCH / fTemp );

    // lGapThreshold is relative to lPITCH:
    // larger the lGapArg (CSHigh), lower the lGapThreshold.
    // Recommended values (CSHigh) are 5000 or 10000, which will yield
    // lGapThrehold 1/5 of pitch or 1/10 of pitch.
    fTemp = (float)lGapArg / (float)1000;
    lGapThreshold = (long)( (float)lPITCH / fTemp );

    // fThickThreshold is used when testing a double slotted wafer thickness.
    // If a wafer thickness is greater than this number times the average wafer
    // thickness, it's considered to be double slotted.
    fThickThreshold = (float)lThickArg / (float)1000;

    // fLocalCrossSlotRatio is used for testing cross slotted wafer.
    // If the difference between the detected wafer position and where it's supposed
    // to be, divided by lPITCH, is greater than this ratio, it's considered to be
    // cross slotted.
    // Thus the larger ratio, the less sensitive in detecting cross slot.
    if ( lCrossRatioArg<=10L || lCrossRatioArg>=900L)
    {
        if (lPITCH<250000L)
            fLocalCrossSlotRatio = 0.23f;
        else
            fLocalCrossSlotRatio = 0.21f;
    }
    else
        fLocalCrossSlotRatio = (float)lCrossRatioArg/1000;

    lRefPoint = lRefPointArg;

}

/****************************************************************
 *
 * Function:    MPGetRefPoint
 *
 * Abstract:    Return the global, lRefPoint.
 *
 * Parameters:  none
 *
 * Returns:     lRefPoint
 *
 ***************************************************************/
long MPGetRefPoint( )
{
    return lRefPoint;
}

/****************************************************************
 *
 * Function:    MPNewScanFilter
 *
 * Abstract:    This is the main routine for filtering the raw data.
 *              The algorithm filters the raw data in 4 major steps:
 *              (1) Remove 0-thickness hits
 *              (2) Remove multiple hits
 *              (3) Corrects two-pass data
 *              (4) Check and determine the slot status
 *
 * Parameters:
 *      Input (in Global):
 *          long    *lpPass1Arg     // first pass Z pos raw-data buffer
 *          int     *ipTrans1Arg    // first pass Z trans raw-data buffer
 *          long    *lpPass2Arg     // second pass Z pos raw-data buffer
 *          int     *ipTrans2Arg    // second pass Z trans raw-data buffer
 *          (as argument)
 *          int     iTeachArg       // if 2 or 3, use only pass 1 data
 *                                  // and if 1 or 3, just store lRefPoint and return.
 *          int     iNewSchemeArg   // if 1, new Indexer mapping scheme
 *
 *      Output:
 *          long    *lpPosResultArg // output Z pos result data buffer
 *          int     *ipSlotStatArg  // output slot status data buffer
 *          long    *lpSlotOffArg   // Average offset
 *                                  // (offset of wafer position from the real middle)
 *
 * Returns: lCassAngle      - Average cassette tilt angle
 *
 ***************************************************************/
long MPNewScanFilter
(
    long *lpPosResultArg,
    int  *ipSlotStatArg,
    int         iTeachArg,
    long *lpSlotOffArg,
    int         iNewSchemeArg
)
{
    int     nHit;               // index to data array
    int     nHitRes;            // number of Hits resulted after multiples removed
    int     n;                  // index
    int     nBadHit;            // number of bad hits: missing hits, pass 1 has the hit
                                // but pass 2 doesn't have it, or vice versa.
    long    lAvOffset;          // average offset (distance how far wafer is off from the
                                // expected position
    long    lDiffPass;          // position difference between two passes
    long    lCassAngle = 0L;    // cassette tilted angle computed during slot status checking
    int     iPass1Empty = 0;    // flag if pass 1 data is empty
    long    lAvThickness;       // average thickness of wafers

    fCROSS_SLOT_TRSHLD_L = fLocalCrossSlotRatio;

    // set the intermediate result pointers
    lpPosFinal = lpPosResultArg;
    ipSlotFinal = ipSlotStatArg;

    // iTeachArg forces to use only pass 1 data
    if (iTeachArg == 2 || iTeachArg == 3)
    {
	    for (nHit = 1; nHit < SCAN_ARRAY_SIZE; nHit++)
	    {
			lpPass2Pos[nHit] = lpPass1Pos[nHit];
			ipPass2Trans[nHit] = ipPass1Trans[nHit];
	    }
	}

	// initialize slot status buffer to 0's
	for (nHit = 0; nHit <= NO_OF_SLOTS; nHit++)
		ipSlotStatArg[nHit] = 0;

    for (nHit = 0; nHit <= HALF_SCAN_ARRAY; nHit++)
        cpBad[nHit] = -50;

	/////////////// step 1 /////////////////////////////////////
    //    Remove 0 thickness hits for pass 1 data
    //    * Use Hit count if possible, and adjust number of hits
    //    * to be implemented in the future...
    //    Currently, it breaks the loop when the position buffer
    //    contains 0's in two rows consecutively. (like End-of-Data flag)
	for (nHit = 1; nHit < SCAN_ARRAY_SIZE; nHit++)
	{
		if (lpPass1Pos[nHit] == 0 && lpPass1Pos[nHit+1] == 0)
            break;
        // if two consecutive data are same value, discard this pair of data
        // by shifting the array elements in two position (nHit+2).
		if (lpPass1Pos[nHit] == lpPass1Pos[nHit+1])
        {
		    if (nHit+2 <= SCAN_ARRAY_SIZE)
            {
                memmove((void *)&lpPass1Pos[nHit],
                       (void *)&lpPass1Pos[nHit+2],
                       (size_t)((SCAN_ARRAY_SIZE-nHit-2) * sizeof(long)));
		    }
        } // if thickness = 0
    } // for all hits for pass 1

    // The pass 1 data: first 2 hits are 0 => no hits at all?
    // Assume no hits at all. Use Pass 2 data. Move pass 2 data to
    // pass 1 array.
	if (nHit == 1)
	{
	    iPass1Empty = 1;
	    for (nHit = 1; nHit < SCAN_ARRAY_SIZE; nHit++)
        {
            lpPass1Pos[nHit] = lpPass2Pos[nHit];
            ipPass1Trans[nHit] = ipPass2Trans[nHit];
        }
    } // if no hits at all for pass 1

    // Remove 0 thickness hits for pass 2 data: SAME AS PASS 1
	for (nHit = 1; nHit < SCAN_ARRAY_SIZE; nHit++)
	{
        if (lpPass2Pos[nHit] == 0L && lpPass2Pos[nHit+1] == 0L)
            break;
		if (lpPass2Pos[nHit] == lpPass2Pos[nHit+1])
        {
		    if (nHit+2 <= SCAN_ARRAY_SIZE)
            {
                memmove((void *)&lpPass2Pos[nHit],
                       (void *)&lpPass2Pos[nHit+2],
                       (size_t)((SCAN_ARRAY_SIZE-nHit-2) * sizeof(long)));
		    }
        } // if 0 thickness
    } // for all hits for pass 2

    // The pass 2 data: first 2 hits are 0 => no hits at all?
    // If no hits at all, use pass 1 data if pass 1 data is not empty.
    // Otherwise, set slot status to 0's (empty cassette) and return;
    if (nHit == 1)
    {
	    if (iPass1Empty)
        {
		    for (nHit = 0; nHit < NO_OF_SLOTS; nHit++)
                ipSlotStatArg[nHit] = 0;
            return 0;
	    }
	    else
        {
		    for (nHit = 1; nHit < SCAN_ARRAY_SIZE; nHit++)
            {
                lpPass2Pos[nHit] = lpPass1Pos[nHit];
                ipPass2Trans[nHit] = ipPass1Trans[nHit];
            }
        }
    } // if no hits at all for pass 2

	/////////////// step 2 /////////////////////////////////////
	// Remove all multiple hits in pass 1 data buffer
	// NOTE: Here it assumes that both pass 1 and 2 have the same
    //       number of hits! In the future implementation, this
    //       assumption must be verified.
    lAvThickness = 0L;
    nHitRes = MPRemoveMultiple(lpPass1Pos, ipPass1Trans, lpWafPos1,
                        lpWafThick1, lpSlotPitch1, &lAvThickness);
	// Fill the rest with 0 in pass 1 pos, waf, pitch buffers
	for (n = nHitRes+1; n <= HALF_SCAN_ARRAY; n++)
    {
		lpWafPos1[n] = 0;
		lpWafThick1[n] = 0;
		lpSlotPitch1[n] = 0;
	}

	// Remove all multiple hits in pass 2 data buffer
    nHitRes = MPRemoveMultiple(lpPass2Pos, ipPass2Trans, lpWafPos2,
						lpWafThick2, lpSlotPitch2, &lAvThickness);
	// lAvThickness is accumulation of pass 1 and pass 2
    // Assume 2nd pass nHitRes is same as 1st pass.
    if ((nHitRes-1) > 0)
        lAvThickness /= (nHitRes-1) * 2L;
  	else
        lAvThickness = 30000L;

	// Fill the rest with 0 in pass 2 pos, waf, pitch buffers
	for (nHit = nHitRes+1; nHit <= HALF_SCAN_ARRAY; nHit++)
    {
		lpWafPos2[nHit] = 0;
		lpWafThick2[nHit] = 0;
		lpSlotPitch2[nHit] = 0;
	}

    // for iTeachArg = 1 or 3, just store lRefPoint and return.
    if (iTeachArg==1 || iTeachArg==3)
    {
		// set reference point
        lRefPoint = (lpWafPos1[1] + lpWafPos2[1]) / 2L - lPITCH;
		return 1;
	}

	lpWafPos1[0] = lRefPoint;
	lpWafPos2[0] = lRefPoint;
	lpWafThick1[0] = lAvThickness;
	lpWafThick2[0] = lAvThickness;

	/////////////// step 3 /////////////////////////////////////
	// Compare pass 1 and pass 2 positions.
	// If the difference is greater than threshold, then the position
	// is set as bad, and the lower position data is inserted into
	// the data buffer of the higher position.
    // In the future implementation, here also can use nHitRes returned
    // by MPRemoveMultiple() instead of checking both pass 1 and 2 data being 0's.
    nBadHit = 0;
	for (nHit = 0; nHit < NO_OF_SLOTS; nHit++)
	{
        if (lpWafPos1[nHit] == 0L || lpWafPos2[nHit] == 0L)
            break;
        // calculate the difference between pass 1 and 2 position at nHit index
		lDiffPass = lpWafPos1[nHit] - lpWafPos2[nHit];
        // check the difference against the threshold
        // if it's greater than threshold, this nHit's position data of pass 1 and 2
        // must be broken into two different (distinct) wafer positions, and
        // make a note that at this index nHit this happened (store this info in cpBad)
		if (labs(lDiffPass) > lDiffPassThreshold)
		{
            // if positive diff, pass 1 is greater than pass 2, so
            // insert pass 2 data into pass 1 buffer
            if (lDiffPass > 0L)
            {
                memmove((void *)&lpWafPos1[nHit+1],
                    (void *)&lpWafPos1[nHit],
                    (size_t)((NO_OF_SLOTS+2-nHit) * sizeof(long)));
                memmove((void *)&lpWafThick1[nHit+1],
                    (void *)&lpWafThick1[nHit],
                    (size_t)((NO_OF_SLOTS+2-nHit) * sizeof(long)));
                // set pass 2 data to pass 1 buffer at nHit index.
                // make note of this situation by setting a flag in cpBad.
                // Note cpBad's index is number of this situation and its
                // content is nHit.
				lpWafPos1[nHit] = lpWafPos2[nHit];
				lpWafThick1[nHit] = lpWafThick2[nHit];
                cpBad[nBadHit++] = (char)nHit;
            }
            else
            {
                // set pass 1 data to pass 2 buffer at nHit index.
                // make note of this situation by setting a flag in cpBad.
                // Note cpBad's index is number of this situation and its
                // content is nHit.
                memmove((void *)&lpWafPos2[nHit+1],
                    (void *)&lpWafPos2[nHit],
                    (size_t)((NO_OF_SLOTS+2-nHit) * sizeof(long)));
                memmove((void *)&lpWafThick2[nHit+1],
                    (void *)&lpWafThick2[nHit],
                    (size_t)((NO_OF_SLOTS+2-nHit) * sizeof(long)));

				lpWafPos2[nHit] = lpWafPos1[nHit];
				lpWafThick2[nHit] = lpWafThick1[nHit];
                cpBad[nBadHit++] = (char)nHit;
            } // if positive lDiffPass... else...
        } // if lDiffPass greater than threshold...
    } // for nHit

	/////////////// step 4 /////////////////////////////////////
    // Final filter step:
    // Check slot status - find slot number for a given wafer position
    // Determine the status of slot, OK, CrossSlot, DoubleSlot, Empty.
    lAvOffset = MPCheckSlotStatus(lpWafPos1, lpWafPos2, lpWafThick1, lpWafThick2,
		    lpPosResultArg, ipSlotStatArg, cpBad, lAvThickness, &lCassAngle, iNewSchemeArg);

    // Filter is finished...
    // Shift up the result arrays from index 1 to 0
    // Only reference data is stored at index 0.
    *lpSlotOffArg = (long)(-lAvOffset / 1000L);
    memmove(&ipSlotStatArg[0], &ipSlotStatArg[1],
        (size_t)((NO_OF_SLOTS) * sizeof(int)));
    memmove(&lpPosResultArg[0], &lpPosResultArg[1],
        (size_t)((NO_OF_SLOTS) * sizeof(long)));

	return lCassAngle;
}

/****************************************************************
 *
 * Function:    MPRemoveMultiple
 *
 * Abstract:    This is the second filter step in scanning process,
 *              that removes multiple hits on a wafer.
 *
 * Parameters:
 *      Input:
 *          long *lpPassArg         Z pos data buffer pointer
 *          int  *ipTransArg        Z trans data buffer pointer
 *          long *lpPosArg          filtered Z pos data buffer
 *          long *lpWafArg          computed Wafer thickness buffer
 *          long *lpPitchArg        computed Pitch data buffer
 *
 *      Output:
 *          long *lpThickArg        accumulated thickness
 *
 *  Return:
 *      nHitRes:    No of hits after removing multiple hits
 *
 ***************************************************************/
int MPRemoveMultiple
(
    long *lpPassArg,
    int  *ipTransArg,
    long *lpPosArg,
    long *lpWafArg,
    long *lpPitchArg,
    long *lpThickArg
)
{
    int  nHit;              // index to lpPassArg[]
    int  nHitRes;           // number of hit result after removing multiple hits
	int  nNextHit;          // index to lpPassArg[] for multiple hit search
    int  bMultFound;        // flag set TRUE if multiple hit found
	long lHitThick;         // temporary wafer thickness
    long lHitThickTotal;    // total wafer thickness for multiple hits
	long lGap;              // gap between two hits
    long lGap2Total;        // total gap among multiple hits
    long lGap2;
    long lWafThick;
    long lPitchTotal;       // pitch: from a wafer position to the next wafer

    nHitRes = 1;
    bMultFound = 0;

    // Loops until both Position data and Trans data are 0's.
    // In the future implementation, Hit counter should be used...
    for (nHit = 1; nHit < SCAN_ARRAY_SIZE; nHit++)
    {
        if (lpPassArg[nHit] == 0L && ipTransArg[nHit] == 0L)
            break;

        // check the hit-pair (it has to be TRANS_UP & TRANS_DN)
        // At TRANS_UP: begin-edge of the hit-pair
        // at TRANS_DN: end-edge of the hit-pair
        if (ipTransArg[nHit] == TRANS_UP && ipTransArg[nHit+1] == TRANS_DN)
        {
            // store begin-edge as the wafer position
            lpPosArg[nHitRes] = lpPassArg[nHit];
            // lHitThick=thickness of the hit-pair
            lHitThick = lpPassArg[nHit+1] - lpPassArg[nHit];
            // lGap=gap between this hit-pair and the next hit
            lGap = lpPassArg[nHit+2] - lpPassArg[nHit+1];

            lHitThickTotal = 0L;
            lGap2Total = 0L;
            lGap2 = 0L;
            // check the gap against the threshold.
            // if it is too small, consider it to be part of the current wafer.
            // that is, we found the multiple hit that needs to be removed.
            if (lGap < lGapThreshold)
            {
                // proceed to the next hit-pair until
                // (1) the end of array is reached, or
                // (2) we found the gap > threshold.
                nNextHit = nHit + 2;
				for(;;)
                {
                    if (nNextHit >= SCAN_ARRAY_SIZE || lpPassArg[nNextHit] == 0)
                    {
                        // lWafThick=total thickness
						// smaller gaps (lGap2Total) are all added in thickness
                        lWafThick = lHitThick + lHitThickTotal + lGap2Total + lGap;
                        // lPitchTotal=gap between this and the next hit-pair
						lPitchTotal = lGap2;
                        // if total pitch is 0 or less, use the previous slot pitch
                        if (lPitchTotal <= 0L)
                            lPitchTotal = lpPitchArg[nHitRes-1];
                        lpWafArg[nHitRes] = lWafThick;
                        lpPitchArg[nHitRes] = lPitchTotal;
                        // end of hit buffer reached, just return
                        return nHitRes;
                    } // if end of hit buffer

                    if (ipTransArg[nNextHit]==TRANS_UP && ipTransArg[nNextHit+1]==TRANS_DN)
                    {
                        // lHitThickTotal=accumulation of thickness of hit-pairs
                        // lGap2=gap between this hit-pair and the next
                        // lGap2Total=accumulation of lGap2, total gap
                        lHitThickTotal += (lpPassArg[nNextHit+1] - lpPassArg[nNextHit]);
                        lGap2 = lpPassArg[nNextHit+2] - lpPassArg[nNextHit+1];
                        lGap2Total += lGap2;
                        // check the gap against threshold.
                        // if it is large enough, break-point found for the current wafer
                        // and the next wafer.
						if (lGap2 > lGapThreshold)
                        {
                            // do not include this gap, it's part of the next wafer
                            lGap2Total -= lGap2;
                            // nNextHit is to be the next hit to be tested.
                            nHit = nNextHit;
							break;
                        } // if gap is greater than threshold
                    } // if hit-pair UP&DN
					nNextHit++;
                } // for infinite-loop

                // compute the wafer thickness, adding hit-thickness and hit-gap that's
                // less than threshold
                lWafThick = lHitThick + lHitThickTotal + lGap2Total + lGap;
                lPitchTotal = lGap2;
                bMultFound = 1;
            } // if gap is less than threshold ==> multiple hit found

            if (bMultFound == 0)
            {
                lWafThick = lHitThick + lHitThickTotal + lGap2Total;
                lPitchTotal = lGap + lGap2;
            } // if bMultFound

            // reset bMultFound, store the computed values: wafer thickness, pitch
            // lpThickArg is accumulation of wafer thickness, later to be converted
            // to average by dividing by number of hit results.
            bMultFound = 0;
            *lpThickArg += lWafThick;
            lpWafArg[nHitRes] = lWafThick;
            lpPitchArg[nHitRes] = lPitchTotal;
            nHitRes++;
        } // for a hit-pair UP & DN
    } // for nHit

    return nHitRes;
}

/****************************************************************
 *
 * Function:    MPCheckSlotStatus
 *
 * Abstract:    This is the fourth filter step, the main filtering process
 *              in scanner module. It determines slot status.
 *
 * Parameters:
 *      Input:
 *          long *lpWafPos1Arg        // wafer pos in pass 1
 *          long *lpWafPos2Arg        // wafer pos in pass 2
 *          long *lpWafThick1Arg      // wafer thickness in pass 1
 *          long *lpWafThick2Arg      // wafer thickness in pass 2
 *          long     lAvThicknessArg        // average thickness computed before
 *          char *cpBadArg            // temporary flag buffer
 *          int         iNewSchemeArg       // if 1, new Indexer mapping scheme
 *
 *      Output:
 *          long *lpPosResultArg      // wafer pos finally computed
 *          int  *ipSlotStatArg       // slot status buffer
 *          long *lpCassAngleArg            // average cassette tilt angle
 *
 *  Return:
 *          lAvOffset:    Average offset (offset of wafer position from the real middle)
 *
 ***************************************************************/
long MPCheckSlotStatus
(
    long *lpWafPos1Arg,
    long *lpWafPos2Arg,
    long *lpWafThick1Arg,
    long *lpWafThick2Arg,
    long *lpPosResultArg,
    int  *ipSlotStatArg,
    char *cpBadArg,
    long lAvThicknessArg,
    long *lpCassAngleArg,
    int iNewSchemeArg
)
{
    int     nHit;           // index: Hit number
    int     nSlot;          // index: Slot number
    int     nOffPoint;      // number of OFF points (wafer position far apart from
                            //                       the expected position)
    int     nOfPoints;      // number of good points (wafer position) found during checking
    int     nIterations;    // will do the slot status checking all over

    long    lWthick1;       // wafer thickness for pass 1
    long    lWthick2;       // wafer thickness for pass 2
    long    lMidPos1;       // wafer's middle position for pass 1
    long    lMidPos2;       // wafer's middle position for pass 2
    long    lRealMiddle;    // Real middle position, where the wafer supposed to be
    long    lWafPos;        // wafer position, (low edge + half thickness)


    long    lRunOffset;     // offset to use when computing fRatio
    long    lAvOffset;      // average offset
    long    lAvThickness;   // average wafer thickness
    long    lMidDiff;       // ZAngle of each wafer detected

    long    lZOffset[NO_OF_SLOTS + 1];  // wafer offset (z position) data array
    long    lZAngle[NO_OF_SLOTS + 1];   // wafer tilted angle data array

    // new stuff
    float   fAveRatio;      // average fRatio
    float   fRatio;         // used for checking cross-slot
    long    lTempMid;
    double  dTempRatio;

    // initialize work fields
    lAvOffset = 0L;
    nOffPoint = 0;
	nIterations = 0;
	lRunOffset = 0L;
    lAvThickness = lAvThicknessArg;

// We'll do this process twice
// if the average offset (lAvOffset) > 10000L and < lPITCH / 2.75.
// This condition for iteration is to be further analyzed...
NEXT_LOOP:
    // nOfPoints is the number of good points found
    // during slot checking
	nOfPoints = 0;

    // initialize temporary buffer used for
    // testing slot offset
	for (nHit = 0; nHit <= NO_OF_SLOTS; nHit++)
	{
	    lZOffset[nHit] = 5000000L;
	    lZAngle[nHit] = 5000000L;
	}

	// 	Align position data buffer.
	//	if any pass position is 0, use the other pass data if
	//  it is a valid position data.
	for (nHit = 1; nHit <= NO_OF_SLOTS; nHit++)
	{
        if (lpWafPos1Arg[nHit] == 0L && lpWafPos2Arg[nHit] > 0L)
            lpWafPos1Arg[nHit] = lpWafPos2Arg[nHit];
        if (lpWafPos2Arg[nHit] == 0L && lpWafPos1Arg[nHit] > 0L)
            lpWafPos2Arg[nHit] = lpWafPos1Arg[nHit];
	}

	//	Initialize the final result data buffer with 0's
	for (nHit = 0; nHit <= NO_OF_SLOTS; nHit++)
	{
        lpPosResultArg[nHit] = 0L;
	}

	fAveRatio = 0.0f;

    // Now, go through all the position data stored in lpWafPos
    // (which was filtered by MPRemoveMultiple()), and determine
    // slot number and status for each position data.
    // Given a position data, we'll find the slot number for it.
    // Using the real middle position, we'll calculate how far the
    // wafer position is different from where it's expected to be.
    for(nHit = 0; nHit < NO_OF_SLOTS+1; nHit++)
    {
        if (lpWafPos1Arg[nHit] == 0L || lpWafPos2Arg[nHit] == 0L)
            break;

		lWthick1 = lpWafThick1Arg[nHit];
        lWthick2 = lpWafThick2Arg[nHit];

		// use the previous wafer thickness if the current is < 0.
        lWthick1 = (lWthick1 < 0L) ? lpWafThick1Arg[nHit-1] : lpWafThick1Arg[nHit];
        lWthick2 = (lWthick2 < 0L) ? lpWafThick2Arg[nHit-1] : lpWafThick2Arg[nHit];

		// get the middle position
		//	pos 1 & 2 are begin-edge positions
		//	middle pos is begin-edge + thickness/2
        lMidPos1 = lpWafPos1Arg[nHit] + lWthick1 / 2L;
        lMidPos2 = lpWafPos2Arg[nHit] + lWthick2 / 2L;
        lMidDiff = labs(lMidPos1 - lMidPos2);

		// average(m1-m2) is proportional to angle of the cassette
		// average the two positions
		// this is the wafer mid-position found
        lWafPos = (lMidPos1 + lMidPos2) / 2L;
		// now, find the slot number for this position: lWafPos
        // and check this wafer's slot status
        for (nSlot = 0; nSlot < NO_OF_SLOTS+1; nSlot++)
        {
            // RealMiddle is the calculated position of the midpoint
            // of the slot (nSlot).
            // Ratio is difference between RealMiddle and wafer position
            // adjusted by lRunOffset to a fixed PITCH.
            // If this ratio is too high (greater than threshold), it's a crossed slot.
            lTempMid = (lWthick1 + lWthick2) / 4L;
		    lRealMiddle = lRefPoint + lPITCH * nSlot + lTempMid;
            dTempRatio = (double)(lRealMiddle - lWafPos + lRunOffset);
		    fRatio = (float)(dTempRatio / (double)lPITCH);

            // following use of cpBad[nHit] is completely removed.
            if (cpBadArg[nHit] == nSlot)
            {
                cpBadArg[nHit] = -1;
            }

			// check if the wafer pos is within the range of a half pitch at the
			// RealMiddle adjusted by avOffset. If it is, the slot number is
			// found for this position. "nSlot" is the slot number for lWafPos.
            if (lWafPos > lRealMiddle - lPITCH / 2L + lRunOffset &&
                lWafPos < lRealMiddle + lPITCH / 2L + lRunOffset)
		    {
				fAveRatio += fRatio;
				// now, check the double-wafer thickness against the avThickness
				// times the user-defined scale. Note this scale is arbitrarily
				// set by the user. Either pass 1 or 2 wafer thickness is greater
				// than this threshold, it's considered to be double-wafer.
				if (lWthick1 > (long)((float)lAvThickness * fThickThreshold) ||
                    lWthick2 > (long)((float)lAvThickness * fThickThreshold))
					ipSlotStatArg[nSlot] = M_DOUBLE;

                // check for cross-slot
				// if "ratio" is less than the threshold and it's not double
				// wafer thickness, then it's M_OKAY slot.
                if (fabs(fRatio) < fCROSS_SLOT_TRSHLD_L && ipSlotStatArg[nSlot] == 0)
				{
                    // CK: 06142000 X-slot improvement (Amat Implant report)
                    // If a cross-slotted wafer is lifted up so that its mid pos is within
                    // the good range of GOOD Slot, the only way to detect its X-slot is to use
                    // ZAngle of the wafer, that is lMidDiff.
                    //
                    if (lMidDiff >= lLiftUpXSlotDiffThreshold)
                    {
                        if (fRatio > 0.0 && nSlot > 0)
                            ipSlotStatArg[nSlot-1] = M_CROSSSLOT;
                        if (fRatio < 0.0 && nSlot < NO_OF_SLOTS)
                            ipSlotStatArg[nSlot+1] = M_CROSSSLOT;
                        ipSlotStatArg[nSlot] = M_CROSSSLOT;
                    }
                    else
                    {
					    ipSlotStatArg[nSlot] = M_OKAY;
                    }
                    lpPosResultArg[nSlot] = lWafPos / 1000L; // store the position in unit=.01"
                    if (cpBadArg[nHit] != nSlot)
                    {
                        lZOffset[nOfPoints] = lRealMiddle - lWafPos;
                        lZAngle[nOfPoints] = lMidDiff;
                        nOfPoints++;
                    }
                }
				else if (fabs(fRatio) >= fCROSS_SLOT_TRSHLD_L && nSlot!= 0)
				{
					// if "fRatio" is greater than the threshold, it's cross-slot.
					// positive ratio means this slot is crossed with the previous
					// slot, and negative with the next slot.
                    if (fRatio > 0.0 && nSlot > 0)
                        ipSlotStatArg[nSlot-1] = M_CROSSSLOT;
                    if (fRatio < 0.0 && nSlot < NO_OF_SLOTS)
                        ipSlotStatArg[nSlot+1] = M_CROSSSLOT;
                    ipSlotStatArg[nSlot] = M_CROSSSLOT;
                    lpPosResultArg[nSlot] = lWafPos / 1000L;
                    if (cpBadArg[nHit] != nSlot)
                    {
                        lZOffset[nOfPoints] = lRealMiddle - lWafPos;
                        lZAngle[nOfPoints] = lMidDiff;
                        nOfPoints++;
                    }
                } // if iCROSS_SLOT_THRSHLD_L
            } // if slot number for this pos found
        } // for nSlot

		*lpCassAngleArg += lMidPos1 - lMidPos2;

    } // for nHit

    // if any valid hit, compute the average offset angle
    // *lpCassAngleArg is average of all (midpos1-midpos2)
    if ( nHit )
        *lpCassAngleArg /= (long)nHit;
	else
        *lpCassAngleArg = 0x7FFFFF;

	lAvOffset = 0;
	nOffPoint = 0;

	for (nHit = 1; nHit <= NO_OF_SLOTS; nHit++)
	{
        if (lZOffset[nHit] == 5000000L && lZAngle[nHit] == 5000000L)
            break;

        // only when the difference of midpoint between pass 1
        // and pass 2 is smaller than 8000L, accumulates the
        // difference between the real middle and the wafer position.
	    if (labs(lZAngle[nHit]) < 8000L)
	    {
			lAvOffset += lZOffset[nHit];
			nOffPoint++;
	    }

	}

    // If there are any OFF points, compute the average offset of wafer positions
    // from the real middle
    if (nOffPoint != 0)
        lAvOffset /= (long)nOffPoint;
    else
        lAvOffset = 0L;

    // The following piece of code will be skipped if it's a new Indexer mapping scheme.
    // Implemented in the old firmware I3AV
    if (!iNewSchemeArg)
    {
        // If the average offset falls into the following condition,
        // do the whole thing over. This time, use the average offset when calculating
        // the offset ratio (how much the wafer position is apart from the expected position).
        // NOTE: This condition is UNCLEAR. The following IF-Statement means as following:
        // Is the average offset greater than 1/25 of the Pitch, and is it less than 1/2.75 of
        // the Pitch? *** It's not clear why this condition is checked. ***
	    if (labs(lAvOffset) > 10000L && labs(lAvOffset) < (long)((double)lPITCH / 2.75))
	    {
	        nIterations++;
	        if (nIterations < 2)
	        {
			    for(nHit = 0; nHit <= NO_OF_SLOTS; nHit++)
                    ipSlotStatArg[nHit] = 0;
			    lRunOffset = -lAvOffset;
			    goto NEXT_LOOP;
	        }
	    }
    }
    return lAvOffset;
}
