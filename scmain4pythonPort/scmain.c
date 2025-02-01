/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : scmain.c
 * Description          : This is main module for the whole system/software.
 *                      In this module, it does initialization of all the other
 *                      modules, providing all the configuration information
 *                      for the entire system. It also handles the main loop
 *                      through which all system-wide activity is handled, from
 *                      comm port I/O to macro scheduling and execution.
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
 * Program              : scmain module
 
 * Main Functions       :   main
 *                          Init
 *                          FailureExit
 *
 * Environment: SUSE LINUX C
 *
 * Modification history:
 *
 * Rev      Date        Brief Description
 * 01A      04.06.2007  Initial version
 * 01B      13.01.2025  Take over
 *
\***************************************************************/
#include <sys/io.h>
#include <signal.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>


//#include <conio.h>
#include <malloc.h>
#include <errno.h>

#include "sck.h"
#include "scstat.h"
#include "scver.h"
#include "scmac.h"
#include "serm.h"
#include "ser.h"
#include "dg.h"
#include "scio.h"
#include "ro.h"
#include "rofn.h"
#include "roga.h"
#include "gag.h"
#include "gamn.h"
#include "fiog.h"
#include "mapio.h"
#include "mapstn.h"
#include "sctag.h"
#include "sctim3.h"
#include "sctim.h"
//#include "sctim2.h"
#include "scregg.h"
#include "scintr.h"
#include "sctch.h"
#include "scproc.h"
#include "scttr.h"
#include "cmdfns.h"
#include "alk.h"
#include "alfns.h"
#include "scmem.h"

#include "pdio.h"
#include "scmain.h"


//PMP undefine COMPORT6
#undef COMPORT6
// #define COMPORT6

void TITimerPROC(void);

int iDefineFlag, iEmulatorMode, iaDiagParms[3];

int giNumOfAxes;

long glSN;
u_char gucAddr[10];

//extern pthread_t threadIO;
//void *procIO(void *ptr);
//void IO_PROC();

//extern pthread_t threadRO;
//void *procRO(void *ptr);
//void ROUpdateTS();
//void ROUpdateTP();

//extern pthread_t threadMP;
//void *procMP(void *ptr);

extern pthread_t threadTimer;
void *procTimer(void *ptr);

extern int giROCount;

extern instr_ptr   PC;    // holds current instruction to execute
extern instr_ptr   oldPC; // holds the last execution instruction. Needed only for RLN command

extern pthread_t threadAL;
void *procAL(void *ptr);

int ETHInitPort(void);
int ETHProcessing(void);

char caSysCfgString[15];

void sigIOHandler() 
{
    printf("SIGIO present!\n");
}


/****************************************************************
 *
 * Function:    main
 *
 * Abstract:    Mainline for entire program coordination. It
 *      handles the main loop through which all system-wide
 *      activity is handled, from comm port I/O to macro
 *      scheduling and execution to teach or diagnostics mode
 *      initiation.
 *
 * Parameters:
 *      argv    (in) The number of command line arguments
 *                  including the call to the executable
 *      argc    (in) An array of strings of those arguments
 *
 * Returns:     N/A
 *
 ***************************************************************/
int main(int argv, char *argc[])
{
    int iFlag;              // The value of the teach and diagnostics mode flags.
                            // These are changed from 0 to a port number to
                            // indicate that the mode is to be run on that port.
    int iDCMode;            // The Daisy-Chain Mode value-holder. It is retrieved
                            // by pointer from the comm module. So a variable is necessary.
    char cCharFromCom2;     // A character from the teach pendant comm buffer.
    // char cCharFromCom3;     // A character from the teach pendant comm buffer.
                            // It has a non-EOF value used to initiate teach or diagnostics mode.
    int iPort;              // To cycle through all the comm ports to flush output buffers.
    int iCmdPortTemp;       // To temporarily hold current CMD port value.
    int iSecsModeOnCOM1;    // Keeps temporarily status on COM1.
    struct timeval tv;
    struct timezone tz;

//    unsigned long ulTimeStart=0, ulTimeStartOld=0, ulTimeEnd=0;
//    unsigned long ulProcTime1=0, ulProcTime2=0, ulProcTime3;
//    unsigned long ulProcTime1Tot=0, ulProcTime2Tot=0, ulProcTime3Tot=0;
//    unsigned long ulCount=0;
//    unsigned long ulTimeBuf1[500], ulTimeBuf2[500];
//    char sBuf[100];
    int rc;  	//, iLen;


    tv.tv_sec = 0;
    tv.tv_usec = 0;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
//    settimeofday(&tv, &tz);


    rc = pthread_create( &threadTimer, NULL, procTimer, (void*) NULL);
    if (rc)
    {
	//printf("Thread Timer create error: %ld\n", rc);
        return rc;  
    }

    /* System-wide initialization. */
    if( Init(TRUE) != EX_AUTOMACRO )        /* If there was no AUTOexecute macro... */
    {
printf("No AUTO macro executed\n");
        /* ...give the user a prompt. */
        SERPutsTxBuff(SERGetCmdPort(), "\r\n>");
    }

    // PMP- COM port data receive handler generates SIGIO. Ignore it.
    signal(SIGIO, SIG_IGN);

    /* infinite loop, FOREVER....... */
    for (;;)
    {
        /* Updates limit switch flags in the Mechanism module. These can
         * be read by the STAT and RLS host commands. */
        //ROCheckLimitSwitches();
//	nanosleep(&tv, NULL);

//	++giROCount;
	ROUpdateTS(TRUE);
	ROUpdateTP(TRUE);
	IO_PROC(TRUE);
	IO_OTF();
	MPUpdateIO(TRUE);
	ROCheckLatch(TRUE);

//	TITimerPROC();
        ROCheckIdle();

        /* Check INX MODE when IDLE only. */
        SERCheckINXMode();

        /* Check if command available for processing. */
        if (SERRxBuffAvail(SERGetCmdPort()) > 0)
        {
            /* Process the command line. Execute command from command line if ready */
            PCProcessCmdLine();
        }

//	SERRxBuffAvail(COM3);
//
//#ifdef COMPORT6
//	// Process SECS message on COM4
//        SERRxBuffAvail(COM3);
//        SERRxBuffAvail(COM4);
//        SERRxBuffAvail(COM5);
//        SERRxBuffAvail(COM6);
//#endif
//
        /* This is special for SECS as device, because if we device we can receive commands
         * through COM1 eventhough it is not current Command Port, so we check receive buffer.
         * If there is a command from SECS module available for processing. */
        SERGetCommVals(COM1, SECS_MODE, &iSecsModeOnCOM1);
        if (iSecsModeOnCOM1)
        {
//printf("SecsModeOnCOM1\n");
            iCmdPortTemp = SERGetCmdPort();
            SERSetCommVals(COM1, CMD_PORT, TRUE);
            if (SERRxBuffAvail(SERGetCmdPort()) > 0)
            {
                PCProcessCmdLine();
            }
            SERSetCommVals(iCmdPortTemp, CMD_PORT, TRUE);
        }

        /* Process the macro instructions. Execute next line of any pending macros. */
        if( MRGetMacroStatus() != FROZEN)
	{
/*
	    ulCount++;
	    ulTimeStart = TIRSTime();
	    if(ulTimeStartOld > 0)
	    {
		ulProcTime3 = labs(ulTimeStart - ulTimeStartOld);
		ulProcTime3Tot += ulProcTime3;
	    }
   	    ulTimeStartOld = ulTimeStart;
	    if(ulTimeEnd > 0)
	    {
		ulProcTime1 = labs(ulTimeStart - ulTimeEnd);
		ulProcTime1Tot += ulProcTime1;
	    }

	    if(labs(ulTimeEnd - ulTimeStart) > 4000)
	    {
//		printf("**BEFORE start = %ld end = %ld\n\r",ulTimeStart, ulTimeEnd);
		sprintf(sBuf,"*BEFORE* start=%ld end=%ld dif=%ld\n\r",ulTimeStart, ulTimeEnd, labs(ulTimeStart-ulTimeEnd));
//		SERWriteString(COM1, strlen(sBuf), sBuf, &iLen);

		if(PC) 
		{
		    sprintf(sBuf,"PC->OC=%d line=%d nofopr=%d\n\r",PC->OC, PC->line_number, PC->nofopr);
//		    SERWriteString(COM1, strlen(sBuf), sBuf, &iLen);
		}
		if(oldPC)
		{
		    sprintf(sBuf,"oldPC->OC=%d line=%d nofopr=%d\n\r",oldPC->OC, oldPC->line_number, oldPC->nofopr);
//		    SERWriteString(COM1, strlen(sBuf), sBuf, &iLen);
		}
	    }
*/

            PCProcessMacroInstr();

/*
	    ulTimeEnd = TIRSTime();
	    ulProcTime2 = labs(ulTimeStart - ulTimeEnd);
	    ulProcTime2Tot += ulProcTime2;
	    if(ulCount >= 10000)
	    {
		ulProcTime1 = ulProcTime1Tot;
		ulProcTime3 = ulProcTime3Tot;
		ulProcTime2 = ulProcTime2Tot;
//		printf("TIME: ets=%ld sts=%ld ste=%ld\n",ulProcTime1,ulProcTime3,ulProcTime2);
		sprintf(sBuf,"TIME: ets=%ld sts=%ld ste=%ld\n\r",ulProcTime1,ulProcTime3,ulProcTime2);
//		SERWriteString(COM1, strlen(sBuf), sBuf, &iLen);
		ulProcTime1Tot = 0;
		ulProcTime3Tot = 0;
		ulProcTime2Tot = 0;
		ulCount = 0;
	    }
	    if(labs(ulTimeEnd - ulTimeStart) > 2000)
	    {
//		printf("**AFTER start = %ld end = %ld\n\r",ulTimeStart, ulTimeEnd);
		sprintf(sBuf,"*AFTER* start=%ld end=%ld dif=%ld\n\r",ulTimeStart, ulTimeEnd, labs(ulTimeStart-ulTimeEnd));
//                SERPutsTxBuff(COM1,sBuf);
//		SERFlushTxBuff(COM1);
		if(PC) 
		{
		    sprintf(sBuf,"PC->OC=%d line=%d nofopr=%d\n\r",PC->OC, PC->line_number, PC->nofopr);
//		    SERWriteString(COM1, strlen(sBuf), sBuf, &iLen);
		}
		if(oldPC)
		{
		    sprintf(sBuf,"oldPC->OC=%d line=%d nofopr=%d\n\r",oldPC->OC, oldPC->line_number, oldPC->nofopr);
//		    SERWriteString(COM1, strlen(sBuf), sBuf, &iLen);
		}
	    }
*/

	}

        /* Flush out all data from the internal comm output buffer to the port. */
        for (iPort=COM1; iPort<=COM3; iPort++)
        {
            SERGetCommVals(iPort, SECS_MODE, &iFlag);
            if ((iPort != SERGetCmdPort()) && (iFlag == TRUE))
                SERRxBuffAvail(iPort);
            SERFlushTxBuff(iPort);
        }
#ifdef COMPORT6
        for (iPort=COM4; iPort<=COM6; iPort++)
        {
            SERGetCommVals(iPort, SECS_MODE, &iFlag);
            if ((iPort != SERGetCmdPort()) && (iFlag == TRUE))
                SERRxBuffAvail(iPort);
            SERFlushTxBuff(iPort);
        }
#endif
	ETHProcessing();

        /* Retrieves Daisy-Chain Mode value, ON/OFF, actually TRUE/FALSE */
        SERGetCommVals(NO_PORT_NUM, DC_MODE, &iDCMode);

        /* If a macro is running, you can't start teach or diagnostics
         * mode from the teach pendant. */
        if( MRGetMacroStatus() != MACRO_RUNNING && !iDCMode)
        {
            /* If everything is good-to-go, meaning EQT, NOT the teach pendant,
             * is the active terminal and there has been a teach pendant
             * attached at some point, go ahead and check for a possible start
             * of teach or diagnostics mode. If the teach pendant is in terminal
             * mode, you can't start teach or diagnostics mode with a ctrl-D or ctrl-T.
             * And then characters aren't diverted from a host command through the
             * following SERGetcRxBuff. */
            SERGetCommVals(COM2, PORT_OPEN, &iPort);
//printf("com3 open=%d cmdport=%d\n",iPort, SERGetCmdPort());

            if ( (SERGetCmdPort()==COM1) && iPort )
            {
                /* Get a possible control char from the teach pendant. */
                SERGetcRxBuff(COM2, &cCharFromCom2,FALSE );
                /* cCharFromCom3 = ^T goto teach mode */
                if( cCharFromCom2 == 0x14)
                    TMSetTeachFlag(SERGetTTPort());
                /* cCharFromCom3 = ^D goto diagnostics mode */
                else if(cCharFromCom2 == 0x04)
                    DiagSetFlag(SERGetTTPort());
            }
            /* Get the teach flag. The way teach and diagnostics mode is entered is
             * by setting the appropriate flag to the Comm port number that the
             * mode is to be run through. See the respective module for more details. */
            iFlag = TMGetTeachFlag();
            /* The flag for teaching is either COM1 or COM3. */
            if (iFlag > 0)
            {
                /* Start Teach Mode */
                TMTeach(iFlag, iDefineFlag, iEmulatorMode);
                /* Reset the teach flag so the mode isn't re-entered the next
                 * time through this main loop. */
                TMSetTeachFlag(NO_PORT_NUM);
            }
            /* Get the diagnostic flag. */
            iFlag = DiagGetFlag();
            if (iFlag > 0)
            {
                /* Start Diagnostics Mode */
                DiagStart(iFlag, iaDiagParms[0], iaDiagParms[1], iaDiagParms[2], iDefineFlag, iEmulatorMode);
                /* Reset the diagnostics flag so the mode isn't re-entered the next
                 * time through this main loop. */
                DiagSetFlag(NO_PORT_NUM);
            }
        }
    }

    return 0;
}

/****************************************************************\
 *
 * Function:    Init()
 *
 * Abstract:    Do initialization of all the other
 *      modules, providing all the configuration information
 *      for the entire system.
 *
 * Parameters:  iUseWatchDogArg
 *
 * Returns:     return value indicate either there is AUTO macro or not.
 *
\*****************************************************************/
int Init(int iUseWatchDogArg)
{
    int iOnOffFlag;
    int iNumAxes;
    int iaEquipeAxis[8], iaGalilAxis[8], iaMechType[8], iaSpecialAxis[8];
    long rc;
    //int ifd, 
    int iTot;
    FILE *ifd;

    /* I/O Subsystem initialization. */
    if(IOInitIO() == FAILURE)
    {
        FailureExit("I/O module init has failed");
        SSSetModulesInitializationsStatus(IO_MODULE, TRUE);
    }
printf("IOInitIO done...\n");
    /* Preallocate 2K Heap Memory HERE */
    if (HPPreAllocate() == FAILURE)
    {
        FailureExit("Heap memory preallocation failed");
    }
    if(ISEnableMathErrorHandling() == FAILURE)
    {
        FailureExit("ISEnableMathErrorHandler has failed");
        SSSetModulesInitializationsStatus(MATH_EROR_HANDLER, TRUE);
    }

    if (TIEnableTimer() == FAILURE)
    {
        FailureExit("TIEnableTimer has failed");
        SSSetModulesInitializationsStatus(SYSTEM_TIMER, TRUE);
    }
    if (iUseWatchDogArg)
    {
        /* Activate the software watchdog timer if desired.
         * See the timer module for more details. */
        if (TIActivateWD() == FAILURE) FailureExit("TIActivateWD has failed");
    }

    /* Initializes the configuration file. These functions CANNOT check for a
     * FAILURE return. Otherwise it will always fail on first time system powerup.
     * Also include initialization of WriteToNVSRAM counters*/
    FIOInitNVSErrorHandler();
    FIOReadConfig();
//    FIOWriteConfig();
//    FIOWriteOTFFile();

    /* Initialize the firmware version. */
    FIOGetCfgSysCfgString(caSysCfgString);
    InitVersionString(caSysCfgString, __FILE__);
printf("Config file done...\n");

    InitMechArrays(&iNumAxes, &iDefineFlag, &iEmulatorMode, iaEquipeAxis, iaGalilAxis, iaMechType, iaSpecialAxis, iaDiagParms);
    giNumOfAxes = iNumAxes;

printf("InitMechArrays done...Axes#=%d\n",giNumOfAxes);

    /* Tell the Status module which emulation to use. */
    if (SSInitStatus(iDefineFlag, iEmulatorMode) == FAILURE)
    {
        FailureExit("SSInitStatus has failed");
        SSSetModulesInitializationsStatus(STATUS_MODULE, TRUE);
    }

    /* NOTE:  Nothing before this can output to COM port, since it isn't
     * initialized. However, theoretically we must read NVSRAM to get baud
     * rate first. Although the baud rate parameter is not actually used. */

    /* Initialize Comm ports. */
    if (SERInitPorts(COM1, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, TRUE, FALSE, FALSE) == FAILURE)
    {
        FailureExit("SERInitPorts COM1 has failed");
        SSSetModulesInitializationsStatus(COM1_PORT, TRUE);
    }

    if (SERInitPorts(COM2, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, TRUE, FALSE, FALSE) == FAILURE)
    {
        FailureExit("SERInitPorts COM2 has failed");
        SSSetModulesInitializationsStatus(COM2_PORT, TRUE);
    }

    if (SERInitPorts(COM3, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
    {
        FailureExit("SERInitPorts COM3 has failed");
        SSSetModulesInitializationsStatus(COM3_PORT, TRUE);
    }

#ifdef COMPORT6

    if (SERInitPorts(COM4, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
    {
        FailureExit("SERInitPorts COM4 has failed");
        SSSetModulesInitializationsStatus(COM4_PORT, TRUE);
    }

    if (SERInitPorts(COM5, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
    {
        FailureExit("SERInitPorts COM5 has failed");
        SSSetModulesInitializationsStatus(COM5_PORT, TRUE);
    }
    if (SERInitPorts(COM6, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
    {
        FailureExit("SERInitPorts COM6 has failed");
        SSSetModulesInitializationsStatus(COM6_PORT, TRUE);
    }
#endif


printf("SERInitPorts done...\n");

    if (TTInitTP() == FAILURE)
    {
        FailureExit("TTInitTP has failed");
        SSSetModulesInitializationsStatus(COM3_PORT, TRUE);
    }
printf("TTInitTP done...\n");

    /* Set command line (active terminal) port to EQT port. */
    if (SERSetCommVals(COM1, CMD_PORT, TRUE) == FAILURE) 
	FailureExit("SERSetCmdPort has failed");

    /* Initialize the multi-tasking macro feature. */
    if (TKInitTaskList() == FAILURE)
    {
        FailureExit("task list init has failed");
        SSSetModulesInitializationsStatus(TASK_LIST, TRUE);
    }
printf("TKInitTaskList done...\n");

    /* Initialize R-register, C-register, S-register table. */
    if (RGInit() == FAILURE)
    {
        FailureExit("Init R, C, S variables FAILED");
        SSSetModulesInitializationsStatus(REGISTERS_TABLE, TRUE);
    }
printf("RGInit done...\n");


    // read serial numbers from file
    ifd = fopen("/root/controller/scmain/snpar.par","r");
    if(ifd <= 0)
	glSN = 0;
    else
    {
	iTot = fread(&glSN, sizeof(long), 1, ifd);
	iTot = fread(gucAddr, sizeof(u_char), 10, ifd);
	fclose(ifd);
    }

    /* Initialize the Mechanism module which in turn initializes the Parameter
     * and Motion modules. */
    if (ROInit(iNumAxes, iaMechType, iaEquipeAxis, iaGalilAxis, iaSpecialAxis, iDefineFlag, iEmulatorMode) == FAILURE)
    {
        FailureExit("ROInit has failed");
	exit(-1); // MUST EXIT if ROInit fails.
        SSSetModulesInitializationsStatus(MECHANISM_MODULE, TRUE);
    }
printf("ROInit done...\n");

    /* I/O Subsystem initialization for S293 support. */
    if(IOInitIOS293() == FAILURE)
    {
        FailureExit("I/O module S293 init has failed");
        SSSetModulesInitializationsStatus(IO_MODULE, TRUE);
    }

    /* Initalize the scanner. */
    if (MPInitMapper(iDefineFlag, iEmulatorMode) == FAILURE)
    {
        FailureExit("Mapper module init has failed");
        SSSetModulesInitializationsStatus(SCANNER_MODULE, TRUE);
    }
printf("MPInitMapper done...\n");


    /* Initalize the aligner. */
    if (ALInitialize(iDefineFlag, iEmulatorMode) == FAILURE)
    {
        FailureExit("Aligner init has FAILED");
        SSSetModulesInitializationsStatus(ALIGNER_MODULE, TRUE);
    }
printf("ALInitialize done...\n");

    /* Set Teach and Diagnostic mode flags to NOT run immediately. */
    if (TMSetTeachFlag(NO_PORT_NUM) == FAILURE) FailureExit("TMSetTeachFlag has failed");
    if (DiagSetFlag(NO_PORT_NUM) == FAILURE) FailureExit("DiagSetFlag has failed");

    /* Initialize the OC table. */
    if (CMDinitialize(CMDINIT, &iDefineFlag, &iEmulatorMode) == FAILURE)
    {
        FailureExit("Low level commands init has failed");
        SSSetModulesInitializationsStatus(LOW_LEVEL_MODULE, TRUE);
    }
printf("CMDinitialize done...\n");

    /* Initialize macros. */
    if((iOnOffFlag=MAInit(iDefineFlag, iEmulatorMode)) == FAILURE)
    {
        FailureExit("Macro module init has failed");
        SSSetModulesInitializationsStatus(MACRO_MODULE, TRUE);
    }

//extern OC_TABLE *OC_table;
//extern int num_table_entries;
//FILE *ifp;
//int i;
//ifp = fopen("octable.txt","w");
//for (i=0; i<num_table_entries; ++i)
//    fprintf(ifp, "%3d  %3d  %s   %x   %d   %x\n", i, OC_table[i].OC, OC_table[i].OC_str,
//    OC_table[i].p, OC_table[i].flag, OC_table[i].exec);
//fclose(ifp);


    if(PDInitLatch()==FAILURE)
    {
	FailureExit("Latch initialization has failed");
    }

    if(ETHInitPort()==FAILURE)
    {
	FailureExit("Ethernet Port initialization has failed");
    }

    if(ROReadRC()==FAILURE)
    {
	FailureExit("Friction data read has failed");
    }

    if(FIOReadVectorFile()==FAILURE)
    {
	FailureExit("Vector file read has failed");
    }

    rc = pthread_create( &threadAL, NULL, procAL, (void*) NULL);
    if (rc)
    {
	//printf("Thread AL create error: %ld\n", rc);
        return rc;  
    }

    int iByte;
    iByte = inb(IO_LED_OUTPORT);
//    iByte |= 0x01;
    iByte &= 0xFE;
    outb(iByte, IO_LED_OUTPORT);

    return iOnOffFlag;
}


/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Prints a message of which module failed initialization
 *      and then error exits to the prompt, in DEBUG MODE ONLY!!!
 *      In normal operation, the firmware continues even after
 *      an initialization failure. So the user can determine what
 *      failed later through host commands.
 *
\***************************************************************/
int FailureExit(char *cpFailMsg)
{
//#ifdef DEBUG
	printf("%s\n", cpFailMsg);
	return 0;
//    exit(-1);
//#endif
}
