/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : alisr.c
 * Description          : These are functions to control the interrupt
 *              operation of the CCD array
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
 *
 * Program:     Aligner Module CCD interrupt functions
 * File:        alisr.c
 * Functions:
 * Description: These are functions to control the interrupt
 *              operation of the CCD array
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <time.h>
#include <pthread.h>
#include <sys/io.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "sck.h"
#include "alk.h"
#include "alpre.h"
#include "alisr.h"
#include "alisr.h"
#include "alstep.h"
#include "sctim.h"
#include "scintr.h"
#include "ser.h"
#include "scttr.h"
#include "fiol.h"
#include "gag.h"
#include "scio.h"
#include "dmclnx.h"

void ALChuckVacuum(int iFlagArg);
void ALPinVacuum(int iFlagArg);
int ALWaferOnChuck(void);
#include "fiol.h"
#include "gag.h"
#include "scio.h"
#include "dmclnx.h"

void ALChuckVacuum(int iFlagArg);
void ALPinVacuum(int iFlagArg);
int ALWaferOnChuck(void);

/********** Variables used globally within this file **********/
unsigned short gusPortRead;          // portread - reading from ccd data port
unsigned short gusCurrentData;       // data_curent - last read data buffer
int giDataStatus;                    // data_st - status of data read from CCD
unsigned short gusChuckBit15;        // chuck15 - chuck position bit 15
int giDataError;                     // data_error - error in data read from CCD
int giXilinxAfterHardwareReset;      // x_was_res - xiling after hardware reset
long glCurrentAngle;                 // angle_curent - last angle for data reading
int giMeasureUpdate = FALSE;         // measure_update
int giXilinxMonitor = FALSE;         // xilinx_monitor - if TRUE interrupt subroutine is not stopping itself
static int giChuckStatus = AL_DATA_STATUS_READ_OK;   // chuck_st - INITIALIZE during module
                                                    //      initialization
static unsigned long gulAngularDiff; // angular_dif
unsigned long gulDuplicateCurrentData = 0;   // dd_cur
unsigned long gulDuplicatePreviousData = 0;  // dd_prev
int giCCDNumber;                     // ccd_no - active ccd number
long glReverseMode;
// Following flag we use for debugging of spurious interrupts problem,
// there is a part of the code we change the mode of PIC by writing to it.
// There is a suspicion, interrupts can occur during that write and it can cause the
// PIC to really get messed up, variable helps to track us the timeframe when we writing to PIC
// and thus to identify whether this write causes the problem of surious interrupts
int iWritingToPICFlag = FALSE;
int giThreadAlignerCreated = 0;

pthread_t threadAL;
void *procAL(void *ptr);
void procTakeAMeasurement(void);
void GetAlignerData(void);

int giMeasurementTaken = 0;

int tmBuf[3000];
int tmIdx = 0;
int tmIdx2 = 0;
int giIdleCnt = 0;
int tmIdle[3000];
int lxBuf[1000];
int lxIdx = 0;
char cmCommandLATX[10] = "XQ#LATX";
char cmRespLATX[MAXGASTR];
extern HANDLEDMC ghDMC;
void getLATXdata()
{
  int rc;
    rc = GASendDMCCommand(ghDMC, cmCommandLATX, cmRespLATX, MAXGASTR);
    if(lxIdx >=1000) lxIdx=0;
    lxBuf[lxIdx++] = atoi(cmRespLATX);
    printf("%d %d\n", lxIdx-1, lxBuf[lxIdx-1]);
}
void printLATXdata()
{
  int i;
  for (i=0; i<1000; ++i)
    printf("%d %d %d %d\n",i, lxBuf[i], lxBuf[i+1], lxBuf[i+1] - lxBuf[i]);
}
void usleepTimer(int iMsecArg)
{
//  struct timeval tv;
  struct timespec tv;

  int iSec, iMsec, iSecNow, iMsecNow;
  int iWait;

  iWait = 1;

//  gettimeofday(&tv, NULL);
  clock_gettime(CLOCK_MONOTONIC, &tv);

  iSec = tv.tv_sec;
  iMsec = tv.tv_nsec + 1000*iMsecArg;
  if(iMsec > 1000000000)
  {
     iSec++;
     iMsec -= 1000000000;
  }
  while (iWait)
  {
//    getLATXdata();    
//    gettimeofday(&tv, NULL);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    iSecNow = tv.tv_sec;
    iMsecNow = tv.tv_nsec;
    if (iSecNow == iSec)
    {
  	if (iMsecNow >= iMsec)
		iWait = 0;
    }
    else if (iSecNow > iSec)
	iWait = 0;
  }
}


void takeAMeasurement();

void takeAL(int iMSecArg)
{
//  struct timeval tv;
  struct timespec tv;

  int iSec, iMsec, iSecNow, iMsecNow;
  int iWait, iC, iPrev=0;

  iWait = 1;
//  gettimeofday(&tv, NULL);
  clock_gettime(CLOCK_MONOTONIC, &tv);

  iSec = tv.tv_sec;
  iMsec = tv.tv_nsec + iMSecArg*1000000;
  tmIdx = 0;
  giIdleCnt = 0;
  giRndDataIdx = 0;
  tmIdx2 = 0;
  giMeasurementInProgress = 0;
  if (iMsec > 1000000000)
  {
      	iSec++;
      	iMsec -= 1000000000;
  }
  while (iWait)
  {
      	iC = inb (AL_CCD_PORTC);
      	if (iC & 0x80)
	{
	    if(!iPrev)
	    {
		iPrev = 1;
	    	takeAMeasurement();
	    	tmIdx++;
	    }
	    tmIdx2++;
	}
	else
	{
   	    giIdleCnt++;
	    iPrev=0;
	}

//        gettimeofday(&tv, NULL);
  	clock_gettime(CLOCK_REALTIME, &tv);
        iSecNow = tv.tv_sec;
        iMsecNow = tv.tv_nsec;
        if (iSecNow == iSec)
        {
  	    if (iMsecNow >= iMsec)
		iWait = 0;
        }
        else if (iSecNow > iSec)
	    iWait = 0;
  }

  printf("tmIdx:%d iRnd:%d idx2:%d idle:%d\n",tmIdx, giRndDataIdx, tmIdx2, giIdleCnt);

}





void *procAL(void *ptr)
{
    unsigned char iC;
    struct timespec ntv, mtv, mtv2;
    int iPrev;
    iPrev = 0;
    ntv.tv_sec = 0;
    ntv.tv_nsec = 500000;
    mtv.tv_sec = 0;
    mtv.tv_nsec = 10;
    mtv2.tv_sec = 0;
    mtv2.tv_nsec = 15;

    giMeasurementInProgress = 1;

    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);

    while (1)
    {
	    while (giMeasurementInProgress)
	    {
        	iC = inb (AL_CCD_PORTC);
        	if (iC & 0x80)
		{
		    if (!iPrev)
		    {
		   	iPrev = 1;
//			takeAMeasurement();
			procTakeAMeasurement();
//			nanosleep(&mtv, NULL);
//			usleepTimer(8);
		    }
 		    tmIdx++;
		}
//		nanosleep(&mtv2, NULL);
//		usleepTimer(100);
		else
		{
//		    usleepTimer(8);
		    iPrev = 0;
	   	    giIdleCnt++;
		}
	    }

	    if(giMeasurementTaken == 1)
	    {
			giMeasurementTaken = 0;
//int i;
//		for (i=0; i<300; ++i)
//printf("%d: data: %d idle:%d\n",i, glaAngle[i], guiaData[i]);
//printf("tIdx:%d dIdx:%d dup:%ld bad:%ld idle:%d tIdx2:%d\n",tmIdx, giRndDataIdx,gulDuplicateCount,gulBadCount,giIdleCnt,tmIdx2);
		tmIdx=0; giIdleCnt=0;

	    }
	    nanosleep(&ntv, NULL);
    }
    // will not reach here!
    pthread_exit(ptr);
}

void takeAMeasurement(void)
{
int giThreadAlignerCreated = 0;

pthread_t threadAL;
void *procAL(void *ptr);
void procTakeAMeasurement(void);
void GetAlignerData(void);

int giMeasurementTaken = 0;

int tmBuf[3000];
int tmIdx = 0;
int tmIdx2 = 0;
int giIdleCnt = 0;
int tmIdle[3000];
int lxBuf[1000];
int lxIdx = 0;
char cmCommandLATX[10] = "XQ#LATX";
char cmRespLATX[MAXGASTR];
extern HANDLEDMC ghDMC;
void getLATXdata()
{
  int rc;
    rc = GASendDMCCommand(ghDMC, cmCommandLATX, cmRespLATX, MAXGASTR);
    if(lxIdx >=1000) lxIdx=0;
    lxBuf[lxIdx++] = atoi(cmRespLATX);
    printf("%d %d\n", lxIdx-1, lxBuf[lxIdx-1]);
}
void printLATXdata()
{
  int i;
  for (i=0; i<1000; ++i)
    printf("%d %d %d %d\n",i, lxBuf[i], lxBuf[i+1], lxBuf[i+1] - lxBuf[i]);
}
void usleepTimer(int iMsecArg)
{
//  struct timeval tv;
  struct timespec tv;

  int iSec, iMsec, iSecNow, iMsecNow;
  int iWait;

  iWait = 1;

//  gettimeofday(&tv, NULL);
  clock_gettime(CLOCK_MONOTONIC, &tv);

  iSec = tv.tv_sec;
  iMsec = tv.tv_nsec + 1000*iMsecArg;
  if(iMsec > 1000000000)
  {
     iSec++;
     iMsec -= 1000000000;
  }
  while (iWait)
  {
//    getLATXdata();    
//    gettimeofday(&tv, NULL);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    iSecNow = tv.tv_sec;
    iMsecNow = tv.tv_nsec;
    if (iSecNow == iSec)
    {
  	if (iMsecNow >= iMsec)
		iWait = 0;
    }
    else if (iSecNow > iSec)
	iWait = 0;
  }
}


void takeAMeasurement();

void takeAL(int iMSecArg)
{
//  struct timeval tv;
  struct timespec tv;

  int iSec, iMsec, iSecNow, iMsecNow;
  int iWait, iC, iPrev=0;

  iWait = 1;
//  gettimeofday(&tv, NULL);
  clock_gettime(CLOCK_MONOTONIC, &tv);

  iSec = tv.tv_sec;
  iMsec = tv.tv_nsec + iMSecArg*1000000;
  tmIdx = 0;
  giIdleCnt = 0;
  giRndDataIdx = 0;
  tmIdx2 = 0;
  giMeasurementInProgress = 0;
  if (iMsec > 1000000000)
  {
      	iSec++;
      	iMsec -= 1000000000;
  }
  while (iWait)
  {
      	iC = inb (AL_CCD_PORTC);
      	if (iC & 0x80)
	{
	    if(!iPrev)
	    {
		iPrev = 1;
	    	takeAMeasurement();
	    	tmIdx++;
	    }
	    tmIdx2++;
	}
	else
	{
   	    giIdleCnt++;
	    iPrev=0;
	}

//        gettimeofday(&tv, NULL);
  	clock_gettime(CLOCK_REALTIME, &tv);
        iSecNow = tv.tv_sec;
        iMsecNow = tv.tv_nsec;
        if (iSecNow == iSec)
        {
  	    if (iMsecNow >= iMsec)
		iWait = 0;
        }
        else if (iSecNow > iSec)
	    iWait = 0;
  }

  printf("tmIdx:%d iRnd:%d idx2:%d idle:%d\n",tmIdx, giRndDataIdx, tmIdx2, giIdleCnt);

}





void *procAL(void *ptr)
{
    unsigned char iC;
    struct timespec ntv, mtv, mtv2;
    int iPrev;
    iPrev = 0;
    ntv.tv_sec = 0;
    ntv.tv_nsec = 500000;
    mtv.tv_sec = 0;
    mtv.tv_nsec = 10;
    mtv2.tv_sec = 0;
    mtv2.tv_nsec = 15;

    giMeasurementInProgress = 1;

    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);

    while (1)
    {
	    while (giMeasurementInProgress)
	    {
        	iC = inb (AL_CCD_PORTC);
        	if (iC & 0x80)
		{
		    if (!iPrev)
		    {
		   	iPrev = 1;
//			takeAMeasurement();
			procTakeAMeasurement();
//			nanosleep(&mtv, NULL);
//			usleepTimer(8);
		    }
 		    tmIdx++;
		}
//		nanosleep(&mtv2, NULL);
//		usleepTimer(100);
		else
		{
//		    usleepTimer(8);
		    iPrev = 0;
	   	    giIdleCnt++;
		}
	    }

	    if(giMeasurementTaken == 1)
	    {
			giMeasurementTaken = 0;
//int i;
//		for (i=0; i<300; ++i)
//printf("%d: data: %d idle:%d\n",i, glaAngle[i], guiaData[i]);
//printf("tIdx:%d dIdx:%d dup:%ld bad:%ld idle:%d tIdx2:%d\n",tmIdx, giRndDataIdx,gulDuplicateCount,gulBadCount,giIdleCnt,tmIdx2);
		tmIdx=0; giIdleCnt=0;

	    }
	    nanosleep(&ntv, NULL);
    }
    // will not reach here!
    pthread_exit(ptr);
}

void takeAMeasurement(void)
{
    gusPortRead = inw( AL_CCD_DATA_ADR );
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_CHUCK_DATA);

    gusCurrentData = gusPortRead & AL_CCD_DATA_MASK;
    if ( (gusCurrentData < AL_CCD_MAX_READING ) && (gusCurrentData > AL_CCD_MIN_READING) )
        giDataStatus = AL_DATA_STATUS_READ_OK;
    else
    {
        giDataStatus = AL_DATA_STATUS_READ_ERROR;
        giCCDOutOfRange = TRUE;
    }
    gusCurrentData += (unsigned)giCCDFirstPixelPos;

    gusChuckBit15 = ( gusPortRead & AL_CCD_CHUCK_B15_MASK ) << AL_CCD_CHUCK_B15_SHIFT;
    giDataError = (int)(( gusPortRead & AL_CCD_ERROR_MASK ) / AL_CCD_ERROR_MASK);
    giXilinxAfterHardwareReset = (int)(( gusPortRead & AL_CCD_PREV_RESET_MASK ) / AL_CCD_PREV_RESET_MASK);

    gusPortRead = inw( AL_CCD_DATA_ADR );
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);

    glCurrentAngle = (long)((gusPortRead & AL_CCD_CHUCK_MASK) | gusChuckBit15);
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    giChuckStatus = AL_DATA_STATUS_READ_OK;
    giaDataStat[giRndDataIdx] = giDataStatus;
    glaAngle[giRndDataIdx] = glCurrentAngle;
    guiaData[giRndDataIdx] = gusCurrentData;
<<<<<<< HEAD
    giRndDataIdx++;

    if( giRndDataIdx >= giMaxNumMeasStored  )
{
    giMeasurementTaken = 1;
=======
    giRndDataIdx++;

    if( giRndDataIdx >= giMaxNumMeasStored  )
{
    giMeasurementTaken = 1;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if( giRndDataIdx <= AL_OFFCENTER_END_INDEX )
        giOffsetIndexEnd = giRndDataIdx - (int)AL_SKIP_END - 1;
    else
        giFlatIndexEndLastRead = giRndDataIdx - 1;
    giMeasurementInProgress = FALSE;
}
    return;
<<<<<<< HEAD
}

void procTakeAMeasurement(void)
{
=======
}

void procTakeAMeasurement(void)
{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    gusPortRead = inw( AL_CCD_DATA_ADR );
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_CHUCK_DATA);

    gusCurrentData = gusPortRead & AL_CCD_DATA_MASK;
    if ( (gusCurrentData < AL_CCD_MAX_READING ) && (gusCurrentData > AL_CCD_MIN_READING) )
        giDataStatus = AL_DATA_STATUS_READ_OK;
    else
    {
        giDataStatus = AL_DATA_STATUS_READ_ERROR;
        giCCDOutOfRange = TRUE;
    }
    gusCurrentData += (unsigned)giCCDFirstPixelPos;

    gusChuckBit15 = ( gusPortRead & AL_CCD_CHUCK_B15_MASK ) << AL_CCD_CHUCK_B15_SHIFT;
    giDataError = (int)(( gusPortRead & AL_CCD_ERROR_MASK ) / AL_CCD_ERROR_MASK);
    giXilinxAfterHardwareReset = (int)(( gusPortRead & AL_CCD_PREV_RESET_MASK ) / AL_CCD_PREV_RESET_MASK);

    gusPortRead = inw( AL_CCD_DATA_ADR );
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);

    glCurrentAngle = (long)((gusPortRead & AL_CCD_CHUCK_MASK) | gusChuckBit15);
    giMeasureUpdate = TRUE;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if( !giXilinxMonitor )
    {
        giChuckStatus = AL_DATA_STATUS_READ_OK;
        if( giRndDataIdx < giMaxNumMeasStored  )
        {
            if( (giRndDataIdx > AL_BEGIN_DATA) && (!giStartDataIdx) )
            {
                gulAngularDiff = (unsigned long)labs( glCurrentAngle - glaAngle[giRndDataIdx-1] );
<<<<<<< HEAD
                if( gulAngularDiff < glDistanceBetween2Meas)
		{
		    ++tmIdx2;
                    goto check_angle;
=======
                if( gulAngularDiff < glDistanceBetween2Meas)
		{
		    ++tmIdx2;
                    goto check_angle;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		}
            }

            // first time through should go into else
            if( giRndDataIdx > giStartDataIdx )
            {
                gulAngularDiff = (unsigned long)labs( glCurrentAngle - glaAngle[giRndDataIdx-1] );
                if( gulAngularDiff > 150L )
                {
                    gulBadCount++;
                    giDataStatus = AL_DATA_STATUS_READ_ERROR;
                }
                else if( (gulAngularDiff == 0L) && (gusCurrentData == guiaData[giRndDataIdx-1]) )
                {
                    gulDuplicateCount++;
                    if( gulDuplicateCount > 800 ) // used to be 80
                        goto terminate_meas;
                    else
                        goto check_angle;
                }
            }
            else
            {
                giDataStatus = AL_DATA_STATUS_READ_ERROR;
                giChuckStatus = AL_DATA_STATUS_READ_ERROR;
            }

            giaDataStat[giRndDataIdx] = giDataStatus;
            glaAngle[giRndDataIdx] = glCurrentAngle;
            guiaData[giRndDataIdx] = gusCurrentData;
<<<<<<< HEAD
            giRndDataIdx++;

	    giMeasurementTaken = 1;
=======
            giRndDataIdx++;

	    giMeasurementTaken = 1;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        }
        else
            goto terminate_meas;

check_angle:
<<<<<<< HEAD
//        tmIdx++;
=======
//        tmIdx++;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if( giChuckStatus == AL_DATA_STATUS_READ_OK )
        {
            if( (glCurrentAngle > 65400L) ||
                ((glCurrentAngle > glMaxAngleMeasured) && !giDataReadPosDir) ||
                ((glCurrentAngle < glMaxAngleMeasured) && giDataReadPosDir) )
<<<<<<< HEAD
                goto continue_meas;
=======
                goto continue_meas;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("check angle: an=%ld gidx=%d max=%ld\n", glCurrentAngle, giRndDataIdx, glMaxAngleMeasured);
        }
        else
            goto continue_meas;
    }
    else
        goto continue_meas;

terminate_meas:
    if( giRndDataIdx <= AL_OFFCENTER_END_INDEX )
        giOffsetIndexEnd = giRndDataIdx - (int)AL_SKIP_END - 1;
    else
        giFlatIndexEndLastRead = giRndDataIdx - 1;
    giMeasurementInProgress = FALSE;

continue_meas:
    return;
<<<<<<< HEAD
}
=======
}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)



/****************************************************************
 *
 * Function:    ALSelectCCDNumber
 *
 * Abstract:    Allows selection of correct CCD corresponding to wafer size
 *
 * Parameters:  iCCDNumArg      (in) The CCD number to select as active
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    set_CCD_no
 * Used in:
 *
 ***************************************************************/
int ALSelectCCDNumber(int iCCDNumArg)
{
	if((iCCDNumArg != AL_CCD_1) && (iCCDNumArg != AL_CCD_2) && (iCCDNumArg != AL_CCD_3))
		return FAILURE;
    ALSelectCCDType(AL_CCD_MASK_NUM, iCCDNumArg);

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALEnableCCD
 *
 * Abstract:    Install CCD interrupt routine
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    enable_CCD
 * Used in:
 *
 ***************************************************************/
int ALEnableCCD(void)
{
    IOWriteIO(-1, AL_DIO_CONTROL_MODE, AL_DIO_CONTROL);

    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALResetChuck
 *
 * Abstract:    Reset chuck counter
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    chuck_counter_reset
 * Used in:
 *
 ***************************************************************/
int ALResetChuck(void)
{
    ALSelectCCDType(AL_CCD_RESET_MASK, AL_CCD_RESET_ON);
    TIDelay(10);
    ALSelectCCDType(AL_CCD_RESET_MASK, AL_CCD_RESET_OFF);

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALStartMeasurement
 *
 * Abstract:    enable CCD (FF) interrupt in order to start taking measurments
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    mesument_start
 * Used in:
 *
 ***************************************************************/
int ALStartMeasurement(void)
<<<<<<< HEAD
{
    long rc;
=======
{
    long rc;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    giMeasurementInProgress = TRUE;
    giMeasureUpdate = FALSE;
    giXilinxMonitor = FALSE;
<<<<<<< HEAD

//    GetAlignerData();

//    printLATXdata();

=======

//    GetAlignerData();

//    printLATXdata();

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALStopMeasurement
 *
 * Abstract:    disable CCD (FF) interrupt in order to stop taking measurments.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    mesument_stop
 * Used in:
 *
 ***************************************************************/
int ALStopMeasurement(void)
{
    giMeasurementInProgress = FALSE;

    giXilinxMonitor = FALSE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALTakeMeasurement
 *
 * Abstract:    disable CCD (FF) interrupt in order to stop taking measurments.
 *
 * Parameters:
 *      lpAngleArg  (out) The current chuck angle
 *      lpDataArg   (out) The current measurement data
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    mesument_one
 * Used in:
 *
 ***************************************************************/
int ALTakeMeasurement(long *lpAngleArg, long *lpDataArg)
{
    int iUpdate;
<<<<<<< HEAD
    long lWaferSize;
=======
    long lWaferSize;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    giMeasurementInProgress = TRUE;
    giXilinxMonitor = TRUE;
    giMeasureUpdate = FALSE;
    iUpdate = giMeasureUpdate;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if( FIOGetParamWaferVals(WAFER_SIZE,&lWaferSize) == FAILURE )
        return FAILURE;
    if( ALSetWaferSize((int)lWaferSize) == FAILURE )
        return FAILURE;
<<<<<<< HEAD
//	ALResetChuck();
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_CHUCK_DATA);

    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);
    *lpDataArg = inw( AL_CCD_DATA_ADR ) & AL_CCD_DATA_MASK;
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_CHUCK_DATA);
	*lpAngleArg = inw( AL_CCD_DATA_ADR )& AL_CCD_CHUCK_MASK;

//    ALStartMeasurement( );
//    procTakeAMeasurement();

=======
//	ALResetChuck();
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_CHUCK_DATA);

    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_DATA);
    *lpDataArg = inw( AL_CCD_DATA_ADR ) & AL_CCD_DATA_MASK;
    ALSelectCCDType(AL_CCD_DATA_SEL_MASK, AL_CCD_CHUCK_DATA);
	*lpAngleArg = inw( AL_CCD_DATA_ADR )& AL_CCD_CHUCK_MASK;

//    ALStartMeasurement( );
//    procTakeAMeasurement();

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//    *lpDataArg = (long)(gusCurrentData - giCCDFirstPixelPos);
//    *lpDataArg = (long)(gusCurrentData);
//    *lpAngleArg = glCurrentAngle;

//    ALStopMeasurement( );
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALSelectCCDType
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    _SEL_CCD0_, _SEL_CCD1, _SEL_CCD2, _SEL_CCD3,
 *              _SEL_CCD_DATA, _SEL_CHUCK_DATA
 *  - WASelectCCDInterrupt0
 *  - WASelecCCDInterrupt1
 *  - WASelecCCDInterrupt2
 *  - WASelecCCDInterrupt3
 *  - WASelectCCDInterruptData
 *  - WASelectChuckDataInterrupt
 * Used in:
 *
 ***************************************************************/
int ALSelectCCDType(unsigned uMaskArg, int iIsrCCDTypeArg)
{
    unsigned char uImage;

    //ioperm(AL_CCD_COMMAND_ADR, 1, 1);
    uImage = inb(AL_CCD_COMMAND_ADR);
    uImage = (uImage & ~uMaskArg) | iIsrCCDTypeArg;
    IOWriteIO(-1, uImage, AL_CCD_COMMAND_ADR );
    //ioperm(AL_CCD_COMMAND_ADR, 1, 0);

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ALReturnCCDMask
 *
 * Abstract:
 *
 * Parameters:  iCCDNumArg  (in) CCD number 1, 2, 3
 *
 * Returns:     CCD Mask for given CCD number or FAILURE
 *
 * Used in:     WASetWaferSize
 *
 ***************************************************************/
int ALReturnCCDMask(int iCCDNumArg)
{
	switch(iCCDNumArg)
	{
		case 1 :
			return AL_CCD_1;
		case 2 :
			return AL_CCD_2;
		case 3 :
			return AL_CCD_3;
	}
	return FAILURE;
}


/****************************************************************
 *
 * Function:    ALTestChuckAndCCD
 *
 * Abstract:    tests CCD and chuck for diagnostics
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    test_CCD_CHUCK
 * Used in:
 *
 ***************************************************************/
int ALTestChuckAndCCD(void)
{
    int iTerminated=FALSE, iPrevRes=0, iPrevError=0;
    int iTemp;
    unsigned uPrevData=0;
    long lPrevAngle=0;
    char caBuf[25], cButtonPressed;

    TTPrintsAt( 1, 1, " CCD  ANGLE  E R NO " );
    TTPrintsAt( 2, 1, "00000 00000  0 0    " );
    if( giCCDNumber == 1 )
        TTPrintsAt( 2, 18, "1" );
    else if( giCCDNumber == 2 )
        TTPrintsAt( 2, 18, "2" );
	else
        TTPrintsAt( 2, 18, "3" );
    TTPrintsAt( 3, 1, "R-Reset             " );
    TTPrintsAt( 4, 1, "1, 2, or 3 CCD      " );
    ALStartMeasurement( );
<<<<<<< HEAD
    procTakeAMeasurement();
=======
    procTakeAMeasurement();
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    do
    {
	    procTakeAMeasurement();
        if( uPrevData != gusCurrentData )
        {
            uPrevData = gusCurrentData;
            sprintf( caBuf, "%05u", uPrevData-giCCDFirstPixelPos );
            TTPrintsAt( 2, 1, caBuf );
        }
        if( lPrevAngle != glCurrentAngle )
        {
            lPrevAngle = glCurrentAngle;
            sprintf( caBuf, "%05ld", lPrevAngle );
            TTPrintsAt( 2, 7, caBuf );
        }
        if( iPrevError != giDataError )
        {
            iPrevError = giDataError;
            TTPrintsAt( 2, 14, (iPrevError)?"1":"0" );
        }
        if( iPrevRes != giXilinxAfterHardwareReset )
        {
            iPrevRes = giXilinxAfterHardwareReset;
            TTPrintsAt( 2, 16, (iPrevRes)?"1":"0" );
        }
        SERGetcRxBuff( SERGetTTPort(), &cButtonPressed, FALSE );
        switch( cButtonPressed )
        {
            case '1':
            case '2':
            case '3':   /* Num must be converted to CCDMask in order to select CCD */
                iTemp = ALReturnCCDMask( (int)(cButtonPressed - '0') );
                ALSelectCCDNumber(iTemp);
                caBuf[0] = cButtonPressed;
                caBuf[1] = 0;
                TTPrintsAt( 2, 18, caBuf );
                break;

            case 'R':
                ALResetChuck( );
                ALStopMeasurement( );
                TTPrintsAt( 1, 1, " CCD  ANGLE  E R NO " );
                TTPrintsAt( 2, 1, "00000 00000  0 0    " );
                if( giCCDNumber == 1 )
                    TTPrintsAt( 2, 18, "1" );
                else if( giCCDNumber == 2 )
                    TTPrintsAt( 2, 18, "2" );
                else
                    TTPrintsAt( 2, 18, "3" );
                TTPrintsAt( 3, 1, "R-Reset             " );
                TTPrintsAt( 4, 1, "1, 2, or 3 CCD      " );
                ALStartMeasurement( );
                break;

            case ESC:
            case CR:
            case 'Z':
                iTerminated = TRUE;
                break;

            case 0:
            case EOF:
                break;

            default:
                TTBeepTP( ALERT );
        }
    } while( !iTerminated );
    ALStopMeasurement( );

    return SUCCESS;
}
// Following flag we use for debugging of spurious interrupts problem,
// there is a part of the code we change the mode of PIC by writing to it.
// There is a suspicion, interrupts can occur during that write and it can cause the
// PIC to really get messed up, variable helps to track us the timeframe when we writing to PIC
// and thus to identify whether this write causes the problem of surious interrupts
int ALReturnDebugVar()
{
    return iWritingToPICFlag;
}
<<<<<<< HEAD

void ALChuckVacuum(int iFlagArg)
{
    int iResult;

    iResult = inb(IO_PRE_OUTPUT_L);

    if (iFlagArg == 1)
   	iResult |= 1;
    else
   	iResult &= 0xFE;

    outb(iResult, IO_PRE_OUTPUT_L);
//    IOWriteIO(iResult, -1, IO_PRE_OUTPUT_L);
}

void ALPinVacuum(int iFlagArg) 
{
    int iResult;

    iResult = inb(IO_PRE_OUTPUT_L);

    if (iFlagArg == 1)
   	iResult |= 2;
    else
   	iResult &= 0xFD;

    outb(iResult, IO_PRE_OUTPUT_L);

//printf("pin vacuum = %d\n",iResult);
}

int ALWaferOnChuck(void)
{
    int iRet, iResult;
    char cpBuf[16];

    iRet = 0;
    iResult = inb( IO_PRE_INPUT_K ) & 1;
//printf("wafer on chuck: pre_input_k = %d\n",iResult);
//    iResult = GASendReceiveGalil( GA_CARD_1, (char *)"MG @IN[13]\xD", cpBuf );
//    iResult = atoi(cpBuf);  // if input = 0, wafer is on.
    if (iResult == 0)
	iRet = 1;

    return iRet;
}
=======

void ALChuckVacuum(int iFlagArg)
{
    int iResult;

    iResult = inb(IO_PRE_OUTPUT_L);

    if (iFlagArg == 1)
   	iResult |= 1;
    else
   	iResult &= 0xFE;

    outb(iResult, IO_PRE_OUTPUT_L);
//    IOWriteIO(iResult, -1, IO_PRE_OUTPUT_L);
}

void ALPinVacuum(int iFlagArg) 
{
    int iResult;

    iResult = inb(IO_PRE_OUTPUT_L);

    if (iFlagArg == 1)
   	iResult |= 2;
    else
   	iResult &= 0xFD;

    outb(iResult, IO_PRE_OUTPUT_L);

//printf("pin vacuum = %d\n",iResult);
}

int ALWaferOnChuck(void)
{
    int iRet, iResult;
    char cpBuf[16];

    iRet = 0;
    iResult = inb( IO_PRE_INPUT_K ) & 1;
//printf("wafer on chuck: pre_input_k = %d\n",iResult);
//    iResult = GASendReceiveGalil( GA_CARD_1, (char *)"MG @IN[13]\xD", cpBuf );
//    iResult = atoi(cpBuf);  // if input = 0, wafer is on.
    if (iResult == 0)
	iRet = 1;

    return iRet;
}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
