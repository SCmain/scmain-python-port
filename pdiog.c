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
 * File:        pdiog.c
 *
 * Program:     Controller firmware
 *
\***************************************************************/
#include <sys/io.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "cmdfns.h"
#include "cmdsp.h"
#include "cmdal.h"
#include "fio.h"
#include "gag.h"
#include "sck.h"
#include "scstat.h"
#include "otf.h"
#include "scio.h"
#include "scmem.h"
#include "dmclnx.h"
#include "ro.h"

extern HANDLEDMC ghDMC;

#define NUMBER_OF_LATCH_DATA 500
#define MOVINGDATANUM 20

/********** Variables Used Locally **********
* calibration table for realtime monitoring
*    8 axes
*    0 (+ motion), 1 (- motion)
*    0(min), 1(max), 2(ave), 3(user-range), 4(not used)
*********************************************/
long glPDCalTable[8][2][5]; // 0(min), 1(max), 2(ave), 3(user-range), 4(not used)
long glPDCalTableEU[8][2][5]; // 0(min), 1(max), 2(ave), 3(user-range), 4(not used)

/*********************************************
* Latch Encoder data buffer
*    8 axes
*    0 (+ motion), 1 (- motion)
*    500 data
*********************************************/
long glLatchData[8][2][NUMBER_OF_LATCH_DATA];
long glLatchDataEU[8][2][NUMBER_OF_LATCH_DATA];

/*********************************************
* Latch Encoder data moving average of 20 data
*    8 axes
*    0 (+ motion), 1 (- motion)
*    500 data
*********************************************/
long glLatchMovingAverage[8][2][NUMBER_OF_LATCH_DATA];

/*********************************************
* Latch Encoder data moving slope computed by moving average of 20 data 
*    8 axes
*    0 (+ motion), 1 (- motion)
*    500 data
*********************************************/
long glLatchMovingSlope[8][2][NUMBER_OF_LATCH_DATA];


/*********************************************
* Latch buffer pointer
*    8 axes
*    0 (+ motion), 1 (- motion)
*    0(current run pointer), 1(total counter)
*********************************************/
long glLatchPtr [8][2][2];

long PDGetMovingAverage(int iAxisArg, int iDirArg, int iRunArg)
{
    int iRunTotal;
    long lSum;
    int iFirst, i, j;

    lSum = 0;

    iRunTotal = glLatchPtr[iAxisArg][iDirArg][1];
    if (iRunTotal < MOVINGDATANUM)
	iFirst = 0;
    else
	iFirst = iRunArg - MOVINGDATANUM + 1;

    for (i=iFirst; i<=iRunArg; ++i)
    {
	j=i;
	if(j<0) j=j+NUMBER_OF_LATCH_DATA;
	lSum = lSum + glLatchData[iAxisArg][iDirArg][j];
    }

    lSum = lSum / MOVINGDATANUM;
    glLatchMovingAverage[iAxisArg][iDirArg][iRunArg] = lSum;
    return lSum;
}
long PDGetMovingSlope(int iAxisArg, int iDirArg, int iRunArg)
{
//    int iRunTotal;
    long lSlope;
    int iFirst;    //, i, j;
    double dSlope;

    lSlope = 0;

    if (glLatchPtr[iAxisArg][iDirArg][1] < MOVINGDATANUM)
    {
	glLatchMovingSlope[iAxisArg][iDirArg][iRunArg] = 0;
	return 0;
    }

    iFirst = iRunArg - MOVINGDATANUM + 1;
    if(iFirst < 0)
	iFirst = iFirst + NUMBER_OF_LATCH_DATA;
    dSlope = (glLatchMovingAverage[iAxisArg][iDirArg][iRunArg] - glLatchMovingAverage[iAxisArg][iDirArg][iFirst]) / MOVINGDATANUM;
    lSlope = dSlope * 1000;
    glLatchMovingSlope[iAxisArg][iDirArg][iRunArg] = lSlope;
    return lSlope;
}
//*****************************************************************
// Do a range check on the current latch data
// Range: (Average - UserRange) <--> (Average + UserRange) 
// Returns 1 if out-of-range
// Returns 0 if within range
//*****************************************************************
int PDOutOfRange(int iAxisArg, int iDirArg, int iRunArg)
{
    if(glLatchData[iAxisArg][iDirArg][iRunArg] < glPDCalTable[iAxisArg][iDirArg][2] - glPDCalTable[iAxisArg][iDirArg][3])
	return 1;
    if(glLatchData[iAxisArg][iDirArg][iRunArg] > glPDCalTable[iAxisArg][iDirArg][2] + glPDCalTable[iAxisArg][iDirArg][3])
	return 1;

    // Must compute Moving Average before Moving Slope.
    PDGetMovingAverage(iAxisArg, iDirArg, iRunArg);
    PDGetMovingSlope(iAxisArg, iDirArg, iRunArg);

    return 0;
}


// Convert Axis to a number from 0 to 7 to access the table
long PDConvAxis(ULONG ulAxisArg)
{
    if(ulAxisArg == 1) return 0;
    if(ulAxisArg == 2) return 1;
    if(ulAxisArg == 4) return 2;
    if(ulAxisArg == 8) return 3;
    if(ulAxisArg == 16) return 4;
    if(ulAxisArg == 32) return 5;
    if(ulAxisArg == 64) return 6;
    if(ulAxisArg == 128) return 7;

    return -1;
}

// Read Latch calibration file into table
int PDReadLatchCalFile()
{
    FILE *iFP;
    int iTotalInt;

    iFP = fopen(LATCHCLNAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "Latch Cal Read Open Error " );
    }
    else
    {
        iTotalInt = fread( glPDCalTable, sizeof( long ), 80, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Latch Cal Read Error " );
	}
        iTotalInt = fread( glPDCalTableEU, sizeof( long ), 80, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Latch Cal EU Read Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// Latch Calibration file write
int PDWriteLatchCalFile()
{
    FILE *iFP;
    int iCount;
    
    iFP = fopen( LATCHCLNAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Latch Cal Write Open Error " );
    }
    else
    {
        iCount = fwrite(glPDCalTable, sizeof(long), 80, iFP);
        if (iCount <= 0) 
        {
            perror( "Latch Cal Write Error " );
        }
        iCount = fwrite(glPDCalTableEU, sizeof(long), 80, iFP);
        if (iCount <= 0) 
        {
            perror( "Latch Cal Write Error " );
        }
        
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// Latch Data buffer read from file.
int PDReadLatchDataFile()
{
    FILE *iFP;
    int iTotalInt;

    iFP = fopen(LATCHDTNAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "Latch Data Read Open Error " );
    }
    else
    {
	iTotalInt = fread( glLatchPtr,  sizeof( long ), 32, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Latch Ptr Read Error " );
	}
        iTotalInt = fread( glLatchData, sizeof( long ), 8000, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Latch Data Read Error " );
	}
        iTotalInt = fread( glLatchDataEU, sizeof( long ), 8000, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Latch Data Read Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// Latch Data buffer write to file.
int PDWriteLatchDataFile()
{
    FILE *iFP;
    int iCount;
    
    iFP = fopen( LATCHDTNAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Latch Data Write Open Error " );
    }
    else
    {
	iCount = fwrite(glLatchPtr, sizeof(long), 32, iFP);
        if (iCount <= 0) 
        {
            perror( "Latch Ptr Write Error " );
        }
        iCount = fwrite(glLatchData, sizeof(long), 8000, iFP);
        if (iCount <= 0) 
        {
            perror( "Latch Data Write Error " );
        }
        iCount = fwrite(glLatchDataEU, sizeof(long), 8000, iFP);
        if (iCount <= 0) 
        {
            perror( "Latch Data Write Error " );
        }
        
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}
// Moving Average Data buffer read from file.
int PDReadMovingAverageFile()
{
    FILE *iFP;
    int iTotalInt;

    iFP = fopen(LATCHMANAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "Moving Average Read Open Error " );
    }
    else
    {
        iTotalInt = fread( glLatchMovingAverage, sizeof( long ), 8000, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Moving Average Read Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// Moving Average write to file.
int PDWriteMovingAverageFile()
{
    FILE *iFP;
    int iCount;
    
    iFP = fopen( LATCHMANAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Moving Average Write Open Error " );
    }
    else
    {
        iCount = fwrite(glLatchMovingAverage, sizeof(long), 8000, iFP);
        if (iCount <= 0) 
        {
            perror( "Moving Average Write Error " );
        }
        
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}
// Latch Moving Slope Data buffer read from file.
int PDReadMovingSlopeFile()
{
    FILE *iFP;
    int iTotalInt;

    iFP = fopen(LATCHMSNAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "Moving Slope Read Open Error " );
    }
    else
    {
        iTotalInt = fread( glLatchMovingSlope, sizeof( long ), 8000, iFP);
	if( iTotalInt<=0 )
	{
            perror( "Moving Slope Read Error " );
	}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}

// Moving Slope write to file.
int PDWriteMovingSlopeFile()
{
    FILE *iFP;
    int iCount;
    
    iFP = fopen( LATCHMSNAME, "w");
    if( iFP == (FILE *)0 )
    {
        perror( "Moving Slope Write Open Error " );
    }
    else
    {
        iCount = fwrite(glLatchMovingSlope, sizeof(long), 8000, iFP);
        if (iCount <= 0) 
        {
            perror( "Moving Slope Write Error " );
        }
        
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
        fclose( iFP );
    }

    return SUCCESS;
}
//
// Command to write Predictive Calibration Table to a file
// WRCT
int ex_WRCT(instr_ptr instr)
{
    return PDWriteLatchCalFile();

}
//
// Command to read Predictive Calibration Table from a file
// LDCT
int ex_LDCT(instr_ptr instr)
{
    return PDReadLatchCalFile();

}
//
// Command to write Predictive Latch Data Buffer to a file
// WRPD -- writes latch data, moving average, moving slope into separate files
int ex_WRPD(instr_ptr instr)
{
    if (PDWriteLatchDataFile() == FAILURE)
	return FAILURE;
    if (PDWriteMovingAverageFile() == FAILURE)
	return FAILURE;
    return PDWriteMovingSlopeFile();

}
//
// Command to read a file into Predictive Latch Data Buffers
// LDPD -- reads latch data, moving average, moving slope from separate files
int ex_LDPD(instr_ptr instr)
{
    if (PDReadLatchDataFile() == FAILURE)
	return FAILURE;
    if (PDReadMovingAverageFile() == FAILURE)
	return FAILURE;
    return PDReadMovingAverageFile();

}
//
// Command to access Predictive Calibration Table, (Read calib table Item)
// RPCT axis, item, [R]
int ex_RPCT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lType, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr; 
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);  // get the axis 
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	Optr = Optr->next;   // get direction # 
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
	    return FAILURE;
	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;  // get type # (min, max, ave, user-range, unused)
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lType)==FAILURE)
	    return FAILURE;
    	if(lType < 0 || lType > 4) return FAILURE;

	lData = glPDCalTable[lAxis][lDir][lType];

	return CMDReadParam(Optr->next, lData);
    }
    return FAILURE;
}
int ex_RPCTE(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lType, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr; 
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);  // get the axis 
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	Optr = Optr->next;   // get direction # 
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
	    return FAILURE;
	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;  // get type # (min, max, ave, user-range, unused)
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lType)==FAILURE)
	    return FAILURE;
    	if(lType < 0 || lType > 4) return FAILURE;

	lData = glPDCalTableEU[lAxis][lDir][lType];

	return CMDReadParam(Optr->next, lData);
    }
    return FAILURE;
}
// Command to Set Predictive Calibration Table Item
// SPCT axis, direction, item, [R]/V
int ex_SPCT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lAxis, lDir, lType, lData;
    ULONG ulAxis;

    if(instr->nofopr != 4)
        return FAILURE;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;
    
	Optr = Optr->next;                /* get direction # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
	    return FAILURE;
	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;  // get type # (min, max, ave, user-range, unused)
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lType)==FAILURE)
	    return FAILURE;
	if(lType < 0 || lType > 4) return FAILURE;

	Optr = Optr->next;            /* get the new value */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr,&lData)==FAILURE)
	    return FAILURE;

	glPDCalTable[lAxis][lDir][lType] = lData;
	ROEncoderToEU(ulAxis, lData, &glPDCalTableEU[lAxis][lDir][lType]);

	return SUCCESS;
    }
    return FAILURE;
}
// Command to Read Predictive Data (Latched Encoder Pos)
// RPDA axis, direction, item, [R]
// If wrong operands entered, force them to a default.
//
int ex_RPDA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lItem, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) lAxis = 0; // force T-axis return FAILURE;

	Optr = Optr->next;                /* get direction # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
	    lDir = 0; // force + direction return FAILURE;

	if(lDir < 0 || lDir > 1) lDir = 0; // force + direction return FAILURE;

	Optr = Optr->next;                /* get item # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lItem)==FAILURE) lItem = 0; // for index 0 return FAILURE;
    	if(lItem < 0 || lItem > (NUMBER_OF_LATCH_DATA-1)) lItem = 0; // force index 0 return FAILURE;

	lData = glLatchData[lAxis][lDir][lItem];

	return CMDReadParam(Optr->next, lData);
    }
    return FAILURE;
}
int ex_RDEU(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lItem, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) lAxis = 0; // force T-axis return FAILURE;

	Optr = Optr->next;                /* get direction # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
	    lDir = 0; // force + direction return FAILURE;

	if(lDir < 0 || lDir > 1) lDir = 0; // force + direction return FAILURE;

	Optr = Optr->next;                /* get item # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lItem)==FAILURE) lItem = 0; // for index 0 return FAILURE;
    	if(lItem < 0 || lItem > (NUMBER_OF_LATCH_DATA-1)) lItem = 0; // force index 0 return FAILURE;

	lData = glLatchDataEU[lAxis][lDir][lItem];

	return CMDReadParam(Optr->next, lData);
    }
    return FAILURE;
}

// Command to Set Predictive Data (Latched Encoder Pos)
// SPDA axis, direction, item, [R]
int ex_SPDA(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lItem, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	Optr = Optr->next;                /* get direction # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
            return FAILURE;
	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;                /* get item # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lItem)==FAILURE)
            return FAILURE;
	if(lItem < 0 || lItem > (NUMBER_OF_LATCH_DATA-1)) return FAILURE;

	Optr = Optr->next;                /* get value */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lData)==FAILURE)
            return FAILURE;

	glLatchData[lAxis][lDir][lItem] = lData;
	return SUCCESS;
    }
    return FAILURE;
}

// Command to Read Latch PoinTer
// RLPT axis, direction, item, [R]
int ex_RLPT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lItem, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	Optr = Optr->next;                /* get direction # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
            return FAILURE;
	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;                /* get item # */
	if(!Optr)
	    return FAILURE;

    	if(CMDgetValue(Optr, &lItem)==FAILURE)
            return FAILURE;

	if(lItem < 0 || lItem > 1) return FAILURE;

	lData = glLatchPtr[lAxis][lDir][lItem];

	return CMDReadParam(Optr->next, lData);
    } 
    return FAILURE;
}
// Command to Set Latch PoinTer
// SLPT axis, direction, item, [R]
int ex_SLPT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lItem, lData;
    ULONG ulAxis;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	Optr = Optr->next;                /* get direction # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lDir)==FAILURE)
            return FAILURE;
    	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;                /* get item # */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lItem)==FAILURE)
            return FAILURE;
	if(lItem < 0 || lItem > 1) return FAILURE;

	Optr = Optr->next;                /* get value */
	if(!Optr)
	    return FAILURE;

	if(CMDgetValue(Optr, &lData)==FAILURE)
            return FAILURE;

	glLatchPtr[lAxis][lDir][lItem] = lData;
    	return SUCCESS;
    }
    return FAILURE;
}
// Compute the alarm level
// 1. current encoder position out of range (min, max) <-- calibrated
// 2. current encoder position out of range (average + allowance) <-- calibrated
// 3. accumulated difference (encoder deviation from average) sets the alarm level
// 4. accumulated difference running total
int PDComputeAlarm(int iAxisArg, int iDirArg, int iRunArg, int iTotalArg)
{
    int i;
    int iRun, iRunPrev;
    long lTotalDev;
    long lTotalDif;
    int iAlarm;
    long glAveAllowance;

    lTotalDev = 0;
    lTotalDif = 0;
    iRun = iRunArg;
    if(glLatchData[iAxisArg][iDirArg][iRun] < glPDCalTable[iAxisArg][iDirArg][0]) // less than min?
    {
	iAlarm++;
    }
    else if(glLatchData[iAxisArg][iDirArg][iRun] > glPDCalTable[iAxisArg][iDirArg][1]) // greater than max?
    {
	iAlarm++;
    }
    else if(glLatchData[iAxisArg][iDirArg][iRun] < (glPDCalTable[iAxisArg][iDirArg][2] - glAveAllowance)) // less than ave allowance?
    {
	iAlarm++;
    }
    else if(glLatchData[iAxisArg][iDirArg][iRun] > (glPDCalTable[iAxisArg][iDirArg][2] + glAveAllowance)) // greater than ave allowance?
    {
	iAlarm++;
    }

    for (i=0; i<20; i++)
    {
	iRun = iRunArg + i;
        if (iRun > (NUMBER_OF_LATCH_DATA-1)) 
  	    iRun = iRun - (NUMBER_OF_LATCH_DATA-1); // ring buffer cnt 1 to 199 (0 excluded)
        lTotalDev += glPDCalTable[iAxisArg][iDirArg][2] - glLatchData[iAxisArg][iDirArg][iRun]; // accumulate difference
        iRunPrev = iRun - 1;
  	if (iRunPrev <= 0) 
	    iRunPrev = (NUMBER_OF_LATCH_DATA-1);
        lTotalDif += glLatchData[iAxisArg][iDirArg][iRun] - glLatchData[iAxisArg][iDirArg][iRunPrev];
    }

    if(lTotalDev < -50 || lTotalDev > 50) // encoder change accumulation > 100
    {
	iAlarm += 40;
    }
    else if(lTotalDev < -40 || lTotalDev > 40) // encoder change accumulation > 50
    {
	iAlarm += 30;
    }
    else if(lTotalDev < -30 || lTotalDev > 30) // encoder change accumulation > 30
    {
	iAlarm += 20;
    }
    else if(lTotalDev < -20 || lTotalDev > 20) // encoder change accumulation > 20
    {
	iAlarm += 10;
    }
 
    if(lTotalDif < -50 || lTotalDif > 50) // encoder change accumulation > 100
    {
	iAlarm += 400;
    }
    else if(lTotalDif < -40 || lTotalDif > 40) // encoder change accumulation > 50
    {
	iAlarm += 300;
    }
    else if(lTotalDif < -30 || lTotalDif > 30) // encoder change accumulation > 30
    {
	iAlarm += 200;
    }
    else if(lTotalDif < -20 || lTotalDif > 20) // encoder change accumulation > 20
    {
	iAlarm += 100;
    }
 
    return iAlarm;

}

int PDInitLatch()
{
    int rc, iTemp;
    char caResp[MAXGASTR];
    char caALcommand[20] = "ALABCDEFGH\xD";

    iTemp = inb (IO_PRE_OUTPUT_L);
    iTemp |= 0x40; // 01000000 --> otf sensor signals default state.
    outb(iTemp, IO_PRE_OUTPUT_L);

    // arm all axes as initial states
    // IO_PROC() checks for latch status and re-arm it.
    rc = DMCCommand(ghDMC, caALcommand, caResp, MAXGASTR);

    // Initially latch, thus it's not armed.
    rc = DMCCommand(ghDMC, "CN,,-1", caResp, MAXGASTR);


    iTemp = inb(IO_PRE_OUTPUT_L);
    if((iTemp & 0x40) != 0x40)
	return FAILURE;

    if(PDReadLatchCalFile() == FAILURE) return FAILURE;
    if(PDReadLatchDataFile() == FAILURE) return FAILURE;
    if(PDReadMovingAverageFile() == FAILURE) return FAILURE;
    return PDReadMovingSlopeFile();

}

int PDMinMax(int iAxisArg, int iDirArg, int iFirstArg, int iLastArg, long *lMinArg, long *lMaxArg, long *lAveArg)
{
    int i;
    int iLast;
    int iRun;
    long lMin, lMax, lAve, lTotal;

    if(iAxisArg < 0 || iAxisArg > 7 || iDirArg < 0 || iDirArg > 1) return FAILURE;
    if(iLastArg < 0 || iLastArg >= NUMBER_OF_LATCH_DATA || iFirstArg < 0 || iFirstArg >=NUMBER_OF_LATCH_DATA) 
	return FAILURE;

    iLast = iLastArg;
    if(iLastArg < iFirstArg) 
	iLast = NUMBER_OF_LATCH_DATA + iLastArg;

    lMin = glLatchData[iAxisArg][iDirArg][iFirstArg];
    lMax = glLatchData[iAxisArg][iDirArg][iFirstArg];
    lTotal = glLatchData[iAxisArg][iDirArg][iFirstArg];

    for (i=iFirstArg+1; i<=iLast; ++i)
    {
	iRun = i;
 	if (iRun >= NUMBER_OF_LATCH_DATA) iRun = i - NUMBER_OF_LATCH_DATA; // roll back ptr 
	if (glLatchData[iAxisArg][iDirArg][iRun] > lMax) lMax = glLatchData[iAxisArg][iDirArg][iRun];
	if (glLatchData[iAxisArg][iDirArg][iRun] < lMin) lMin = glLatchData[iAxisArg][iDirArg][iRun];
	lTotal += glLatchData[iAxisArg][iDirArg][iRun];
    }
    lAve = lTotal / (iLast - iFirstArg + 1);
    *lMinArg = lMin;
    *lMaxArg = lMax;
    *lAveArg = lAve; 
    return SUCCESS;
}

int PDSTDeviation(int iAxisArg, int iDirArg, int iFirstArg, int iLastArg, long lAveArg, double *dStdev)
{
    int i;
    int iLast;
    int iRun;
    double dTotal;

    if(iFirstArg == iLastArg)
	return FAILURE;
    if(iAxisArg < 0 || iAxisArg > 7 || iDirArg < 0 || iDirArg > 1) return FAILURE;
    if(iLastArg < 0 || iLastArg >= NUMBER_OF_LATCH_DATA || iFirstArg < 0 || iFirstArg >=NUMBER_OF_LATCH_DATA) return FAILURE;

    iLast = iLastArg;
    if(iLastArg < iFirstArg) iLast = NUMBER_OF_LATCH_DATA + iLastArg;

    dTotal = 0.0;

    for (i=iFirstArg; i<=iLast; ++i)
    {
	iRun = i;
 	if (iRun >= NUMBER_OF_LATCH_DATA) iRun = i - NUMBER_OF_LATCH_DATA; // roll back ptr 
	dTotal += abs(lAveArg - glLatchData[iAxisArg][iDirArg][iRun]) * abs(lAveArg - glLatchData[iAxisArg][iDirArg][iRun]);
    }
    dTotal /= (iLast - iFirstArg);
    *dStdev = sqrt(dTotal);
    return SUCCESS;
}
// Command to Compute Min,Max,Ave
// XCL axis, direction, runpointer, #item
int ex_XCL(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    //char sBuf[10];
    long lAxis, lDir, lItem, lRun, lLast, lMin, lMax, lAve;
    ULONG ulAxis;

    Optr = instr->opr_ptr;            /* get axis # */
    if(Optr)
    {
	ulAxis = CMDgetAxis(instr);          /* get the axis */
	lAxis = PDConvAxis(ulAxis);
        if(lAxis < 0 || lAxis > 7) return FAILURE;

	Optr = Optr->next;                /* get direction # */
	if(CMDgetValue(Optr, &lDir)==FAILURE)
            return FAILURE;
    	if(lDir < 0 || lDir > 1) return FAILURE;

	Optr = Optr->next;                /* get run pointer */
	if(CMDgetValue(Optr, &lRun)==FAILURE)
            return FAILURE;

	Optr = Optr->next;                /* get item # */
	if(CMDgetValue(Optr, &lItem)==FAILURE)
            return FAILURE;

	lLast = lRun + lItem - 1;
	if(lLast >= NUMBER_OF_LATCH_DATA)
	    lLast -= NUMBER_OF_LATCH_DATA;

	PDMinMax(lAxis, lDir, lRun, lLast, &lMin, &lMax, &lAve);

	glPDCalTable[lAxis][lDir][0] = lMin;
	glPDCalTable[lAxis][lDir][1] = lMax;
	glPDCalTable[lAxis][lDir][2] = lAve;
	ROEncoderToEU(ulAxis, lMin, &glPDCalTableEU[lAxis][lDir][0]);
	ROEncoderToEU(ulAxis, lMax, &glPDCalTableEU[lAxis][lDir][1]);
	ROEncoderToEU(ulAxis, lAve, &glPDCalTableEU[lAxis][lDir][2]);
    	return SUCCESS;
    }
    return FAILURE;
}
