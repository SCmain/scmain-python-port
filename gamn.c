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
 * Program:     Motion Control Main Routines
 * File:        gamn.c
 * Functions:   GAInit
 *              GAGetMaxNumberOfAxisSupported
 *              GAInitTimerCounters
 *              GAInitStatusWord
 *              GATurnOnGalilInt
 *              GATurnOffGalilInt
 *              GASetUpInterruptVector
 *
 * Description: Provide routines for initializing the Motion
 *      Control module and getting initization realted information.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/
#include <sys/io.h>
#include <string.h>
#include <pthread.h>
#include "sck.h"
#include "gafn.h"
#include "gamn.h"
#include "gag.h"
#include "scintr.h"
#include "scstat.h"
#include "sctim.h"

#include "dmclnx.h"

/*********************************************\
 * GLOBAL VARIABLES DECLARATION
\*********************************************/
HANDLEDMC      	ghDMC = -1;         // Handle to controller
CONTROLLERINFO 	gControllerInfo;    // Controller information structure	

int GAiUseGalil;
int GAiUseInts;
int GAiExcIntOutp;
unsigned MCuGalilError = 0;
int GAiDefaultIRQ7 = 0;
int GAiNumberOfGalilCards = 1;
int GAiPreOnly;

extern int giXYZRobot;

// The Index 0 is not being used
// The fisrt Galil card is 0x01 (GA_CARD_0)
// The second Galil card is 0x02 (GA_CARD_1)
// For both cards is 0x03 (GA_ALL_CARDS)
int GL_TIMER[GA_BOTH_CARDS] = { FAILURE, FAILURE, FAILURE };
int GA_TIMER = FAILURE;
unsigned GALIL_RW[3] = { 0, 740, 736 };
unsigned GALIL_ST[3] = { 0, 741, 737 };
unsigned GALIL_CLR_W[3] = { 0, 741, 737 };
unsigned GALIL_CLR_R[3] = { 0, 743, 739 };
unsigned GL_TIME_ERR[3] = { 0, 0x0001, 0x0010 };
unsigned GL_RPLY_ERR[3] = { 0, 0x0002, 0x0020 };
unsigned GL_REENT_ERR[3]= { 0, 0x0004, 0x0040 };
unsigned GL_WRITE_DATA[3] = { 0, 0x0004, 0x0040 };
unsigned GL_DEBUG_MODE[3] = { 0, 0x0008, 0x0080 };
int GL_NUM_AXIS[3] = { 0x0F, 0xF0, 0xFF};

// Local Copy of Galil Commands Queues
// Note the index 0 has not been used
stGACommandStr stGAQueueCmdString[3];

pthread_t thread1;
void *procGalilPCIInterrupt(void *ptr);
pthread_t thread2;

extern glSN;

long glSN3;
char gaSN3[20];

/****************************************************************\
 *
 * Function:    GAInit
 *
 * Abstract:    Initialize the Motion Control Module Timer and Galil Interrupt
 *
 * Parameters:  iNumberOfGalilCards     (in)
 *              iPreOnly                (in)
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAInit(int iNumberOfGalilCards, int iPreOnly)
{
    long rc = 0, lSN;
    char buffer[MAXGASTR];
    USHORT         	usStatus;          //the interrupt status byte retreived from the controller after a UI 

    // Initialize Motion local variables
    GAiUseGalil = iNumberOfGalilCards + 1;
    MCuGalilError = 0;
    GAiUseInts = 1;
    GAiPreOnly = iPreOnly;

//    ioperm(GALIL_RW[2], 16, 1);

    memset(&gControllerInfo, '\0', sizeof(gControllerInfo));

/////////////////////////////////////////////////////////////////////////////////////
//  MODEL 2100 ETHERNET INTERFACE
    gControllerInfo.cbSize = sizeof(gControllerInfo);
    gControllerInfo.usModelID = MODEL_2100; 
    gControllerInfo.fControllerType = ControllerTypeEthernet;
    gControllerInfo.ulTimeout = 1000;
    gControllerInfo.ulDelay = 5;
//    gControllerInfo.ulDelay = 0;
//    strcpy(gControllerInfo.hardwareinfo.socketinfo.szIPAddress, "169.254.82.100");
    strcpy(gControllerInfo.hardwareinfo.socketinfo.szIPAddress, "10.10.1.11");
    gControllerInfo.hardwareinfo.socketinfo.fProtocol = EthernetProtocolTCP;
/////////////////////////////////////////////////////////////////////////////////////
//  MODEL 1800 PCI INTERFACE
//    gControllerInfo.cbSize = sizeof(gControllerInfo);
//    gControllerInfo.usModelID = MODEL_1800; 
//    gControllerInfo.fControllerType = ControllerTypePCIBus;
//    gControllerInfo.ulTimeout = 1000;
//    gControllerInfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessFIFO;
//    gControllerInfo.ulSerialNumber = 0;  //use relative number
//    gControllerInfo.ulRelativeNumber = 0; //(argc == 2 ? atoi(argv[1]) : 0); //0 is for /
//////////////////////////////////////////////////////////////////////////////////////

    DMCInitLibrary();
	
    // Open the connection
    rc = DMCOpen( &gControllerInfo, &ghDMC );
    if (rc)
    {
	printf("DMCOpen Error: %d\n", rc);
        return rc;  
    }

    // Init The Galil Timer Counters
    if (GAInitTimerCounters() == SUCCESS)
    {
	rc = DMCCommand(ghDMC, "\x12\x16", buffer, sizeof(buffer));
	printf("Connected to %s", buffer);
	rc = DMCCommand(ghDMC, "MG _BN", buffer, sizeof(buffer));
	printf("The serial number is %s", buffer);
	lSN = atol(buffer);
	if (lSN != glSN)
	{
	    printf("galil SN not match!\n");
	    return FAILURE;
        }
	glSN3 = 3 * glSN;
	sprintf(gaSN3, "%d", glSN3);


	rc = DMCCommand(ghDMC, "XQ#AUTO", buffer, sizeof(buffer));
        if(rc)
            printf("XQ#AUTO not success. %ld\n", rc);


        // Initialize Galil Interrupt
        if ( (iNumberOfGalilCards > MAX_GALIL_CARDS) ||
             (iNumberOfGalilCards < MIN_GALIL_CARDS))
            return FAILURE;
        GAiNumberOfGalilCards  = iNumberOfGalilCards;

        // Initialize the first galil card
        if  (!iPreOnly)
        {
            // Galil interrupt for card 0
            if (GATurnOnGalilInt(GA_CARD_0) == FAILURE)
                return FAILURE;
            // Delay for 250 Msec
            //TIDelay(250);
            if (GAGetMaxNumberOfAxisSupported(GA_CARD_0, &GL_NUM_AXIS[GA_CARD_0]) == FAILURE)
                return FAILURE;
            if (GAInitCmdString(GA_CARD_0) == FAILURE)
                return FAILURE;

        }
        // Initialize the second galil card
        if (iNumberOfGalilCards == MAX_GALIL_CARDS)
        {
            // Galil Interrupt for card 1
            // Assume we have only maximum 2 galil cards
            if (GATurnOnGalilInt(GA_CARD_1) == FAILURE)
                return FAILURE;
            // Delay for 250 Msec
            //TIDelay(250);
            if (GAGetMaxNumberOfAxisSupported(GA_CARD_1, &GL_NUM_AXIS[GA_CARD_1]) == FAILURE)
                return FAILURE;
            if (GAInitCmdString(GA_CARD_1) == FAILURE)
                return FAILURE;
        }
    }

//    usStatus = 1;
//    rc = DMCGetInterruptStatus(ghDMC, &usStatus); //clear the interrupt queue

//    rc = pthread_create( &thread1, NULL, procGalilPCIInterrupt, (void*) NULL);
//    if (rc)
//    {
//	printf("Thread procGalilPCIInterrupt create error: %d\n", rc);
//        return rc;  
//    }

    // Set some galil variables according to the robot type XYZ
    TIDelay(100);
//    if(giXYZRobot)
//    {
//	rc = DMCCommand(ghDMC, "VSFE=600", buffer, sizeof(buffer));
//	rc = DMCCommand(ghDMC, "VSFI=150", buffer, sizeof(buffer));
//	rc = DMCCommand(ghDMC, "VSPA=80000", buffer, sizeof(buffer));
//    }

    GAInitStatusWord();
    return SUCCESS;
 }


/****************************************************************\
 *
 * Function:    GAGetMaxNumberOfAxisSupported
 *
 * Abstract:    Get Galil Card Maximum number of Axis supported
 *
 * Parameters:  iCardNoArg      (in)        Galil Card No
 *              iNumOfAxis      (out)       Number of axis supported by Galil (out)
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAGetMaxNumberOfAxisSupported(int iCardNoArg, int *iNumOfAxis)
{
    char szRevision[MAXGASTR], sVersion[5];
    int iCount;

    *iNumOfAxis = 0x0F;
/*    if (GAReadGalilRevisionInfo(iCardNoArg, szRevision) == FAILURE)
    {
        *iNumOfAxis = 0;
        return FAILURE;
    }
    if (szRevision[0] == 0)
    {
        *iNumOfAxis = 0;
        return FAILURE;
    }
    else
    {
        for (iCount=12; iCount<=15; iCount++)
            sVersion[iCount-12] = szRevision[iCount];
        sVersion[4] = 0;
        if (strcmp(sVersion, LATEST_GALIL_VER_STRING))
            SSSetStatusWord2(OLD_GALIL_VER, TRUE);
        if (strncmp(szRevision, DMC8230, 7) == 0)
        {
            *iNumOfAxis = GAXAXIS0 | GAYAXIS0 | GAZAXIS0;
        }
        else if (strncmp(szRevision, DMC8240, 7) == 0)
        {
            *iNumOfAxis = GAXAXIS0 | GAYAXIS0 | GAZAXIS0 | GAWAXIS0;
        }
        else
        {
            *iNumOfAxis = 0;
            return FAILURE;
        }
    }
*/
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GAInitStatusWord
 *
 * Abstract:    Initialize the Galil related status word
 *
 * Parameters:  None
 *
 * Returns:     None
 *
\*****************************************************************/
void GAInitStatusWord(void)
{
    // Set the status word
    SSSetStatusWord( MOT_ERROR, FALSE );
    SSSetStatusWord( SERVO_OFF, TRUE );
    SSSetStatusWord( HOME_NOT_EXED, TRUE );
}


/****************************************************************\
 *
 * Function:    GAInitTimerCounters
 *
 * Abstract:    Get Galil Card related timer counter
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAInitTimerCounters(void)
{
    // We'll always initialize 2 counters for galil card 1 & galil card 2
    // as well as the Galil GA_TIMER
    // Check if the MC timer has been initialized
    if ( GL_TIMER[GA_CARD_0] == FAILURE )
    {
        if ((GL_TIMER[GA_CARD_0] = TIGetCounter()) == FAILURE)
            return FAILURE;
    }
    // Check if MC timer has been initialized
    if ( GL_TIMER[GA_CARD_1] == FAILURE )
    {
        if ((GL_TIMER[GA_CARD_1] = TIGetCounter()) == FAILURE)
            return FAILURE;
    }
    if ( GA_TIMER == FAILURE )
    {
        if ((GA_TIMER = TIGetCounter()) == FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}




/****************************************************************\
 *
 * Function:    GATurnOnGalilInt
 *
 * Abstract:    Turn on galil MC interrupts (enable_mc)
 *              It is called once in initialization
 *
 * Parameters:  int iCardNoArg  (in)  Galil Card number
 *                          GA_CARD_0
 *                          GA_CARD_1
 *
 * Returns:    SUCCESS or FAILURE
 *
\*****************************************************************/
int GATurnOnGalilInt(int iCardNoArg)
{
    int iCardNum;
    char cpBuf[8];

    iCardNum = iCardNoArg;
    if ( GAValidateInterrupt(&iCardNum) == FAILURE)
        return FAILURE;

    // Clear up the Galil read and write FIFO buffer
    if ( iCardNum & GA_CARD_0 )
    {
        GAClearWriteGalilFIFOBuffer(GA_CARD_0);
        GAClearReadGalilFIFOBuffer(GA_CARD_0);
    }
    if ( iCardNum & GA_CARD_1 )
    {
        GAClearWriteGalilFIFOBuffer(GA_CARD_1);
        GAClearReadGalilFIFOBuffer(GA_CARD_1);
    }


    // Send Command to Galil Card 0
    //if (GASendReceiveGalil( GA_CARD_0, (char *)"EI 255,255\xD", cpBuf ) == FAILURE)
    //    return FAILURE;

    return SUCCESS;
}

