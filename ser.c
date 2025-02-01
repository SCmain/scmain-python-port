<<<<<<< HEAD
/****************************************************************
 * Copyright
 * This software is the copyrighted property of XyZ Automation, Inc., A California Corporation.
 * Any use, distribution or reproduction of any such software are exclusively reserved to 
 * licensed customers and governed by the terms of the licesnse agreeement applicable to that 
 * software between XyZ Automation, Inc and its licensees. If you are a licensed customer or a 
 * representative of a licensed customer, XyZ Automation, Inc. authorizes you to copy, distribute
 * and use the software for yourself or, if applicable, only within the customer organization, and
 * only for the intended purposes authorized by XyZ Automation, Inc..
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
 * Program:     Controller firmware
 * File:        ser.c
 * Functions:   SERInitPorts
 *              SERCheckINXMode
 *              SERClosePorts
 *              SERGetRawChar
 *              SERRxBuffAvail
 *              SERGetsRxBuff
 *              SERGetcRxBuff
 *              SERPutsTxBuff
 *              SERFlushTxBuff
 *              SERWaitTxBuff
 *              SERSetEOTChar
 *              SERSetIGNOREChar
 *              SERGetCommVals
 *              SERSetCommVals
 *              SERGetTTPort
 *              SERGetCmdPort
 *              SERPortRedirect
 *              SERGetHistoryList
 *              SERFlushComBuffer
 *              SERCheckForEOT
 *              SERCheckValidPort
 *              SERFetchRxCh
 *              SERStoreRxCh
 *              SERBackspRxCh
 *              SERFetchTxCh
 *              SERStoreTxCh
 *              SERSetInternalCommStatus
 *              SERSetCommErr
 *              SERGetSECSMsg
 *              SERSendSECSMsg
 *              SERSetSECSStatus
 *
<<<<<<< HEAD
 * Description: Handles low-level RS-232 communications
=======
 * Description: Handles low-level RS-232 communications
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 *		Implements Ethernet communication commands & responses.
 *
 ****************************************************************/
#include <sys/io.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <malloc.h>
#include <errno.h>
<<<<<<< HEAD
#include <string.h>
//#include <pthread.h>        /* for POSIX threads */
//#include <sys/socket.h> /* for socket(), bind(), and connect() */
//#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

//#include <netinet/in.h>
//#include <linux/if.h>
=======
#include <string.h>
//#include <pthread.h>        /* for POSIX threads */
//#include <sys/socket.h> /* for socket(), bind(), and connect() */
//#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

//#include <netinet/in.h>
//#include <linux/if.h>
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

#include "sck.h"
#include "ser.h"
#include "serm.h"
#include "serl.h"
#include "sctim.h"
#include "scstat.h"
#include "scmac.h"
#include "secsg.h"
#include "secsl.h"
#include "scttr.h"
#include "scproc.h"
<<<<<<< HEAD
#include "scver.h"
#include "fiog.h"
#include "fio.h"
#include "scmem.h"
#include "roga.h"
#include "scio.h"
#include "otf.h"

//#undef COMPORT6
//PMP #define COMPORT6

int giMaxFD;
fd_set	fdsInput;
fd_set	fdsOutput;

struct timeval	stTimeout;

unsigned int uiEOTretries = 0;
unsigned int uiPrevAct = 99999;
int giSECSComplete = 1;
int giChkSum = 0;

int giFlowDropCount = 1;
int SERFlushRxBuff(int iPortNumArg);

=======
#include "scver.h"
#include "fiog.h"
#include "fio.h"
#include "scmem.h"
#include "roga.h"
#include "scio.h"
#include "otf.h"

//#undef COMPORT6
//PMP #define COMPORT6

int giMaxFD;
fd_set	fdsInput;
fd_set	fdsOutput;

struct timeval	stTimeout;

unsigned int uiEOTretries = 0;
unsigned int uiPrevAct = 99999;
int giSECSComplete = 1;
int giChkSum = 0;

int giFlowDropCount = 1;
int SERFlushRxBuff(int iPortNumArg);

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

/****************************************************************
 *
 * Function:    SERInitPorts
 *
 * Abstract:    Initializes the selected comm port by setting the
 *      member variables of the port structures to initial values.
 *
 * Parameters: (all inputs)
 *      iPortNumArg     Com port number
 *      lBaudRateArg    Baud Rate
 *      iParityArg      Parity
 *      iDataBitsArg    Data bits
 *      iStopBitsArg    StopBits
 *      iFlowCtrlArg    TRUE to use transmit flow control
 *      iEchoPortArg    TRUE to echo the port
 *      iSECSArg        TRUE to use SECS communications
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: init_ports
 * Used in: main
 *
 ***************************************************************/
int SERInitPorts(   int iPortNumArg, int iBaudRateArg, int iParityArg,
                    int iDataBitsArg, int iStopBitsArg, int iFlowCtrlArg,
                    int iEchoPortArg, int iSECSArg)
{
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */
    long BAUD, DATABITS, STOPBITS, PARITYON, PARITY, RTSCTS;
    /* Verifies that operations are requested on ports COM1, COM2,
     * or COM3. Any other port names are invalid. */
    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    switch (iBaudRateArg)
    {
 	case 38400:
		BAUD = B38400;
		break;
	case 19200:
		BAUD = B19200;
		break;
	case 9600:
	default:
		BAUD = B9600;
		break;
	case 4800:
		BAUD = B4800;
		break;
	case 2400:
		BAUD = B2400;
		break;
	case 1200:
		BAUD = B1200;
		break;
	case 600:
		BAUD = B600;
		break;
	case 300:
		BAUD = B300;
		break;
	case 150:
		BAUD = B150;
		break;
 	case 110:
		BAUD = B110;
		break;
    }

    switch (iDataBitsArg)
    {
	case 8:
	default:
		DATABITS = CS8;
		break;
	case 7:
		DATABITS = CS7;
		break;
	case 6:
		DATABITS = CS6;
		break;
    }

    switch (iStopBitsArg)
    {
	case 1:
	default:
		STOPBITS = 0;
		break;
	case 2:
		STOPBITS = CSTOPB;
		break;
    }

    switch (iParityArg)
    {
	case 0:
	default:
		PARITYON = 0;
		PARITY = 0;
		break;
	case 1:
		PARITYON = PARENB;
		PARITY = PARODD;
		break;
	case 2:
		PARITYON = PARENB;
		PARITY = 0;
		break;
    }

    switch (iFlowCtrlArg)
    {
	case 0:
	default:
		RTSCTS = 0;
		break;
	case 1:
<<<<<<< HEAD
		RTSCTS = CRTSCTS;
=======
		RTSCTS = CRTSCTS;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("comport=%d RTSCTS=%d\n",iPortNumArg, RTSCTS);
		break;
    }

    if (iPortNumArg == COM1)
    {
	FD_ZERO(&fdsInput);
	FD_ZERO(&fdsOutput);
	stTimeout.tv_sec = 0;
	stTimeout.tv_usec = 0;
    }

    /* Gets a pointer to the array element associated with the selected port. */
    pCommData = stCommData+iPortNumArg-1;
    /* Sets the port status as open. This can be changed to close on an error exit. */
    pCommData->m_iPortOpen = TRUE;

    /* Initialize structure variables */
    pCommData->m_iStatus = 0;
    pCommData->m_cpRxGet = pCommData->m_cpRxPut = pCommData->m_caRxBuf;
    pCommData->m_cpRxEnd = pCommData->m_caRxBuf+RX_BUF_SIZE;
    memset(pCommData->m_caRxBuf, 0, sizeof(char)*RX_BUF_SIZE);
    pCommData->m_cpTxGet = pCommData->m_cpTxPut = pCommData->m_caTxBuf;
    pCommData->m_cpTxEnd = pCommData->m_caTxBuf+TX_BUF_SIZE;
    memset(pCommData->m_caTxBuf, 0, sizeof(char)*TX_BUF_SIZE);
    pCommData->m_iBaud = iBaudRateArg;
    pCommData->m_iParity = iParityArg;
    pCommData->m_iDataBits = iDataBitsArg;
    pCommData->m_iStopBits = iStopBitsArg;
    pCommData->m_iFlowCtrl = iFlowCtrlArg;
    pCommData->m_iEcho = iEchoPortArg;
    pCommData->m_iSECS = iSECSArg;


    pCommData->m_iFD = open(caCommDevName[iPortNumArg-1], O_RDWR | O_NOCTTY | O_NONBLOCK);
<<<<<<< HEAD
    if (pCommData->m_iFD == -1) {
        // PMP- device file does not exist
        pCommData->m_iPortOpen = FALSE;
    }
=======
    if (pCommData->m_iFD == -1) {
        // PMP- device file does not exist
        pCommData->m_iPortOpen = FALSE;
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)



    // get the current comport setting
    tcgetattr (pCommData->m_iFD, &pCommData->m_OldTIO);

    fcntl (pCommData->m_iFD, F_SETFL, FNDELAY);
    fcntl (pCommData->m_iFD, F_SETFL, FASYNC);

    // set the new setting to the comport
    pCommData->m_NewTIO.c_cflag = BAUD | RTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
    pCommData->m_NewTIO.c_iflag = IGNPAR;
    pCommData->m_NewTIO.c_oflag = 0;
    pCommData->m_NewTIO.c_lflag = 0;
    pCommData->m_NewTIO.c_cc[VMIN] = 1;
    pCommData->m_NewTIO.c_cc[VTIME] = 0;
    tcflush (pCommData->m_iFD, TCIFLUSH);
    tcsetattr (pCommData->m_iFD, TCSANOW, &pCommData->m_NewTIO);


    // Get the max fd
    if(pCommData->m_iFD > giMaxFD) 
	giMaxFD = pCommData->m_iFD;


    /* Flow control options:
     * CTS high required to transmit (unless flow_ctrl turned off)
     * DTR high when port is open
     * RTS goes low when input buffer is almost full, else high */

    /* Check for valid flow control and set it with the ASYNCH Manager.
     * The corresponding Get function is used by the main loop to check if
     * there is a receiving device connected. This should be checked with
     * the DTR signal, but that isn't available for most systems. */
    if (SERSetCommVals(iPortNumArg, FLOW_CTRL, iFlowCtrlArg) == FAILURE) goto error_exit;

    iRet = A_OK;
    if (S2InitSecs(iPortNumArg) == FAILURE) goto error_exit;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    if (iSECSArg)
    {
        /* Receive flow control */
//        iRet = setop_a1(iPortNumArg, LOC_HARD_FLOW_CTRL_OPT, 0);          /* both RTS & DTR set by flow ctrl */
        if (iRet != A_OK) goto error_exit;
//        iRet = setop_a1(iPortNumArg, RDY_LOC_HARD_FLOW_CTRL_OPT, 0);      /* open/ready:    RTS high  DTR high  */
        if (iRet != A_OK) goto error_exit;
//        iRet = setop_a1(iPortNumArg, UNRDY_LOC_HARD_FLOW_CTRL_OPT, 1);    /* open/unready:  RTS low   DTR low   */
        if (iRet != A_OK) goto error_exit;
    }
    else
    {
        /* Receive flow control */
//        iRet = setop_a1(iPortNumArg, LOC_HARD_FLOW_CTRL_OPT, 3);          /* both RTS & DTR set by flow ctrl */
        if (iRet != A_OK) goto error_exit;



//        iRet = setop_a1(iPortNumArg, RDY_LOC_HARD_FLOW_CTRL_OPT, 3);      /* open/ready:    RTS high  DTR high  */
        if (iRet != A_OK) goto error_exit;
//        iRet = setop_a1(iPortNumArg, UNRDY_LOC_HARD_FLOW_CTRL_OPT, 1);    /* open/unready:  RTS low   DTR low   */
        if (iRet != A_OK) goto error_exit;
    }
//    iRet = setop_a1(iPortNumArg, CLOSE_LOC_HARD_FLOW_CTRL_OPT, 0);        /* port closed:   RTS low   DTR low   */
    if (iRet != A_OK) goto error_exit;

    /* Check and set internal variables */
    if (SERSetCommVals(iPortNumArg, ECHO_MODE, iEchoPortArg) == FAILURE) goto error_exit;

    /* Check out a counter from the timer module. */
    if ((iaTimerIndex[iPortNumArg-1]=TIGetCounter()) == FAILURE)
<<<<<<< HEAD
    { 
=======
    { 
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("SERInitPorts failed checking out a counter from timer module\n");
    	return FAILURE;
    }


    return SUCCESS;

error_exit:
    pCommData->m_iPortOpen = FALSE;
    return FAILURE;
}


/****************************************************************
 *
 * Function:    SERClosePorts
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERClosePorts(int iPortNumArg)
{
    struct CommStr *pCommData;

    if (SERCheckValidPort(iPortNumArg) == FAILURE) return FAILURE;

    pCommData = stCommData + iPortNumArg - 1;

    if (!pCommData->m_iPortOpen)
        return FAILURE;

      // restore old port settings
//    if (iPortNumArg == COM1)
//    {
//      tcsetattr(pCommData->m_iFD, TCSANOW, &pCommData->m_OldTIO);
//      tcsetattr(pCommData->m_iTTY,TCSANOW, &pCommData->m_OldKey);
//      close(pCommData->m_iTTY);
    close(pCommData->m_iFD);        //close the com port
//    }
 
    /* The the ASYNCH Manager to return control of the port to DOS. */
//    close_a1(iPortNumArg);
    /* Check the timeout counter back in to the timer module. */
    TIReturnCounter(iaTimerIndex[iPortNumArg-1]);
    /* Inidicate that the port is closed. */
    pCommData->m_iPortOpen = FALSE;

    return SUCCESS;
}
/****************************************************************
 *
 * Function:    SERBufTXLeft
 *
 * Abstract:    Check TX Buff number of chars left
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *	iNumCharLeftArg(out) # characters written
 *
 * Returns: The status of the get-data operation
 ***************************************************************/
int SERBufTXLeft(int iPortNumArg, int *iNumCharLeftArg)
{
    //int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    *iNumCharLeftArg = giTXNumCharLeft[iPortNumArg-1];
    return A_OK;
}
/****************************************************************
 *
 * Function:    SERBufRXLeft
 *
 * Abstract:    Check RX Buff number of chars left
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *	iNumCharLeftArg(out) # characters written
 *	iStatusArg	(out) status
 *
 * Returns: The status of the get-data operation
 ***************************************************************/
int SERBufRXLeft(int iPortNumArg, int *iNumCharLeftArg, int *iStatusArg)
{
    //int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    *iNumCharLeftArg = giRXNumCharLeft[iPortNumArg-1];
    *iStatusArg = A_OK;
    return A_OK;
}


/****************************************************************
 *
 * Function:    SERWriteString
 *
 * Abstract:    Write String to port
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      iLengthArg      (in) length of string to write
 *	cpStrOutArg	(in) string
 *	iNumBytesWrittenArg(out) # characters written
 *
 * Returns: The status of the get-data operation
 *
 *
 ***************************************************************/
int SERWriteString(int iPortNumArg, int iLengthArg, char *cpStrOutArg, int *iNumBytesWrittenArg)
{
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* Refresh the SOFTWARE watchdog timer. This is frequently called in the comm
     * module since comm functions are almost always cycled through regularly.
     * It is a consistent method of resetting the watchdog counter to 0. */
//    if (TIRefreshWD() == FAILURE)
//        return FAILURE;

    *iNumBytesWrittenArg = 0;
    // Initialize multiple ports input & output lists for select() 
    struct CommStr *pCommData1, *pCommData2;
    struct CommStr *pCommData3, *pCommData4;
    struct CommStr *pCommData5, *pCommData6;
    pCommData1 = stCommData;
    pCommData2 = stCommData + 1;
    pCommData3 = stCommData + 2;
<<<<<<< HEAD

#ifdef COMPORT6
    pCommData4 = stCommData + 3;
    pCommData5 = stCommData + 4;
    pCommData6 = stCommData + 5;
#endif

    // PMP- check if port is open before setting device mask
    if (pCommData1->m_iFD != -1)
        FD_SET(pCommData1->m_iFD, &fdsOutput);
    if (pCommData2->m_iFD != -1)
        FD_SET(pCommData2->m_iFD, &fdsOutput);
    if (pCommData3->m_iFD != -1)
        FD_SET(pCommData3->m_iFD, &fdsOutput);

#ifdef COMPORT6
    if (pCommData4->m_iFD != -1)
        FD_SET(pCommData4->m_iFD, &fdsOutput);
    if (pCommData5->m_iFD != -1)
        FD_SET(pCommData5->m_iFD, &fdsOutput);
    if (pCommData6->m_iFD != -1)
        FD_SET(pCommData6->m_iFD, &fdsOutput);
#endif
=======

#ifdef COMPORT6
    pCommData4 = stCommData + 3;
    pCommData5 = stCommData + 4;
    pCommData6 = stCommData + 5;
#endif

    // PMP- check if port is open before setting device mask
    if (pCommData1->m_iFD != -1)
        FD_SET(pCommData1->m_iFD, &fdsOutput);
    if (pCommData2->m_iFD != -1)
        FD_SET(pCommData2->m_iFD, &fdsOutput);
    if (pCommData3->m_iFD != -1)
        FD_SET(pCommData3->m_iFD, &fdsOutput);

#ifdef COMPORT6
    if (pCommData4->m_iFD != -1)
        FD_SET(pCommData4->m_iFD, &fdsOutput);
    if (pCommData5->m_iFD != -1)
        FD_SET(pCommData5->m_iFD, &fdsOutput);
    if (pCommData6->m_iFD != -1)
        FD_SET(pCommData6->m_iFD, &fdsOutput);
#endif
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    // Check if any char in port to read
    iRet = select(giMaxFD+1, NULL, &fdsOutput, NULL, &stTimeout);

    if (iRet == 0) return A_OK;
    if (iRet < 0)
    {
	switch (iRet)
	{
		case EBADF: printf("Invalid fd in select().\n\r"); break;
		case ENOMEM: printf("Unable to allocate memory in select().\n\r");break;
		case EINVAL: printf("Negative ndfs or timeout in select().\n\r"); break;
		case EINTR:  printf("Signal was caught in select().\n\r"); break;
        }
        return iRet;
    }
    if ((pCommData1->m_iFD != -1) && FD_ISSET(pCommData1->m_iFD, &fdsOutput))
    {
        if (pCommData1 == pCommData)
<<<<<<< HEAD
	{
=======
	{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("writing... %s   ", cpStrOutArg);
		TIDelay(20);
    		if (iLengthArg > 0)
			write(pCommData->m_iFD, cpStrOutArg, iLengthArg);
<<<<<<< HEAD
    		*iNumBytesWrittenArg = iLengthArg;
=======
    		*iNumBytesWrittenArg = iLengthArg;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("     ending...\n");
    		return A_OK;
	}
    }
    if ((pCommData2->m_iFD != -1) && FD_ISSET(pCommData2->m_iFD, &fdsOutput))
    {
        if (pCommData2 == pCommData)
	{
		TIDelay(20);
    		if (iLengthArg > 0)
			write(pCommData->m_iFD, cpStrOutArg, iLengthArg);
    		*iNumBytesWrittenArg = iLengthArg;
    		return A_OK;
	}
    }
    if ((pCommData3->m_iFD != -1) && FD_ISSET(pCommData3->m_iFD, &fdsOutput))
    {
        if (pCommData3 == pCommData)
	{
		TIDelay(20);
	    	if (iLengthArg > 0)
			write(pCommData->m_iFD, cpStrOutArg, iLengthArg);
	    	*iNumBytesWrittenArg = iLengthArg;
	    	return A_OK;
	}
    }
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#ifdef COMPORT6
    if ((pCommData4->m_iFD != -1) && FD_ISSET(pCommData4->m_iFD, &fdsOutput))
    {
        if (pCommData4 == pCommData)
	{
		TIDelay(20);
	    	if (iLengthArg > 0)
			write(pCommData->m_iFD, cpStrOutArg, iLengthArg);
	    	*iNumBytesWrittenArg = iLengthArg;
	    	return A_OK;
	}
<<<<<<< HEAD
    }
=======
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if ((pCommData5->m_iFD != -1) && FD_ISSET(pCommData5->m_iFD, &fdsOutput))
    {
        if (pCommData5 == pCommData)
	{
		TIDelay(20);
	    	if (iLengthArg > 0)
			write(pCommData->m_iFD, cpStrOutArg, iLengthArg);
	    	*iNumBytesWrittenArg = iLengthArg;
	    	return A_OK;
	}
    }
    if ((pCommData6->m_iFD != -1) && FD_ISSET(pCommData6->m_iFD, &fdsOutput))
    {
        if (pCommData6 == pCommData)
	{
		TIDelay(20);
	    	if (iLengthArg > 0)
			write(pCommData->m_iFD, cpStrOutArg, iLengthArg);
	    	*iNumBytesWrittenArg = iLengthArg;
	    	return A_OK;
	}
<<<<<<< HEAD
    }
#endif

=======
    }
#endif

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return A_OK;
}

/****************************************************************
 *
 * Function:    COReadChar
 *
 * Abstract:    Read port and return a character
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cCharInArg      (out) The character retrieved
 *	iNumCharsLeftArg(out) # characters in buffer
 *
 * Returns: The status of the get-data operation
 *
 *
 ***************************************************************/
int COReadChar(int iPortNumArg, char *cCharInArg, int *iNumCharsLeftArg, unsigned int *iStatusArg)
{
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* Refresh the SOFTWARE watchdog timer. This is frequently called in the comm
     * module since comm functions are almost always cycled through regularly.
     * It is a consistent method of resetting the watchdog counter to 0. */
//    if (TIRefreshWD() == FAILURE)
//        return FAILURE;

    *iStatusArg = 0;
    // Check if Number of characters left is > 0
    if (giRXNumCharLeft[iPortNumArg-1] > 0)
    {
        *cCharInArg = caRXCommBuf[iPortNumArg-1][giRXCommBufIndex[iPortNumArg-1]];
        --giRXNumCharLeft[iPortNumArg-1];
	++giRXCommBufIndex[iPortNumArg-1];
	return A_OK;
    }
    // Initialize multiple ports input & output lists for select() 
    struct CommStr *pCommData1, *pCommData2;
    struct CommStr *pCommData3, *pCommData4;
    struct CommStr *pCommData5, *pCommData6;
    pCommData1 = stCommData;
    pCommData2 = stCommData + 1;
    pCommData3 = stCommData + 2;
<<<<<<< HEAD

#ifdef COMPORT6
    pCommData4 = stCommData + 3;
    pCommData5 = stCommData + 4;
    pCommData6 = stCommData + 5;
#endif

    // PMP - avoid setting input mask for not open device
    if (pCommData1->m_iFD != -1)
        FD_SET(pCommData1->m_iFD, &fdsInput);
    if (pCommData2->m_iFD != -1)
        FD_SET(pCommData2->m_iFD, &fdsInput);
    if (pCommData3->m_iFD != -1)
        FD_SET(pCommData3->m_iFD, &fdsInput);


#ifdef COMPORT6
    if (pCommData4->m_iFD != -1)
        FD_SET(pCommData4->m_iFD, &fdsInput);
    if (pCommData5->m_iFD != -1)
        FD_SET(pCommData5->m_iFD, &fdsInput);
    if (pCommData6->m_iFD != -1)
        FD_SET(pCommData6->m_iFD, &fdsInput);
#endif

=======

#ifdef COMPORT6
    pCommData4 = stCommData + 3;
    pCommData5 = stCommData + 4;
    pCommData6 = stCommData + 5;
#endif

    // PMP - avoid setting input mask for not open device
    if (pCommData1->m_iFD != -1)
        FD_SET(pCommData1->m_iFD, &fdsInput);
    if (pCommData2->m_iFD != -1)
        FD_SET(pCommData2->m_iFD, &fdsInput);
    if (pCommData3->m_iFD != -1)
        FD_SET(pCommData3->m_iFD, &fdsInput);


#ifdef COMPORT6
    if (pCommData4->m_iFD != -1)
        FD_SET(pCommData4->m_iFD, &fdsInput);
    if (pCommData5->m_iFD != -1)
        FD_SET(pCommData5->m_iFD, &fdsInput);
    if (pCommData6->m_iFD != -1)
        FD_SET(pCommData6->m_iFD, &fdsInput);
#endif

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    // Check if any char in port to read
    iRet = select(giMaxFD+1, &fdsInput, NULL, NULL, &stTimeout);

    if (iRet == 0) return IN_Q_EMPTY;
    if (iRet < 0)
    {
	switch (iRet)
	{
		case EBADF: printf("Invalid fd in select().\n\r"); break;
		case ENOMEM: printf("Unable to allocate memory in select().\n\r");break;
		case EINVAL: printf("Negative ndfs or timeout in select().\n\r"); break;
		case EINTR:  printf("Signal was caught in select().\n\r"); break;
        }
        if(iRet == EINTR) return IN_Q_EMPTY;
	*iStatusArg = STATUS_ERR;
        return STATUS_ERR;
    }

    // check all input ports
    if ((pCommData1->m_iFD != -1) && FD_ISSET(pCommData1->m_iFD, &fdsInput))
    {
	if(pCommData1 == pCommData)
	{
    	// Read the port
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);
    	if (giRXNumCharLeft[iPortNumArg-1] <= 0)
    	{
		*iStatusArg = STATUS_ERR;
		return STATUS_ERR;
    	}
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];
    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }
    if ((pCommData2->m_iFD != -1) && FD_ISSET(pCommData2->m_iFD, &fdsInput))
    {
	if(pCommData2 == pCommData)
	{
    	// Read the port
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);
    	if (giRXNumCharLeft[iPortNumArg-1] <= 0)
    	{
		*iStatusArg = STATUS_ERR;
		return STATUS_ERR;
    	}
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];
    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }
    if ((pCommData3->m_iFD != -1) && FD_ISSET(pCommData3->m_iFD, &fdsInput))
    {
	if(pCommData3 == pCommData)
	{
    	// Read the port
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);
    	if (giRXNumCharLeft[iPortNumArg-1] <= 0)
    	{
		*iStatusArg = STATUS_ERR;
		return STATUS_ERR;
    	}
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];
    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#ifdef COMPORT6
    if ((pCommData4->m_iFD != -1) && FD_ISSET(pCommData4->m_iFD, &fdsInput))
    {
	if(pCommData4 == pCommData)
	{
    	// Read the port
<<<<<<< HEAD
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);

//int i;
=======
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);

//int i;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//for(i=0; i<giRXNumCharLeft[3];++i)
//printf("com4 read char=%2x charleft=%d\n",caRXCommBuf[3][i], i);
    	if (giRXNumCharLeft[iPortNumArg-1] <= 0)
    	{
		*iStatusArg = STATUS_ERR;
		return STATUS_ERR;
    	}
<<<<<<< HEAD
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];

	// clear RX buf if ENQ received 
//	if(*cCharInArg == 5) giRXNumCharLeft[iPortNumArg-1] = 1;

    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }

=======
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];

	// clear RX buf if ENQ received 
//	if(*cCharInArg == 5) giRXNumCharLeft[iPortNumArg-1] = 1;

    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if ((pCommData5->m_iFD != -1) && FD_ISSET(pCommData5->m_iFD, &fdsInput))
    {
	if(pCommData5 == pCommData)
	{
    	// Read the port
<<<<<<< HEAD
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);

//int i;
=======
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);

//int i;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//for(i=0; i<giRXNumCharLeft[3];++i)
//printf("com4 read char=%2x charleft=%d\n",caRXCommBuf[3][i], i);
    	if (giRXNumCharLeft[iPortNumArg-1] <= 0)
    	{
		*iStatusArg = STATUS_ERR;
		return STATUS_ERR;
    	}
<<<<<<< HEAD
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];

	// clear RX buf if ENQ received 
//	if(*cCharInArg == 5) giRXNumCharLeft[iPortNumArg-1] = 1;

    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
=======
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];

	// clear RX buf if ENQ received 
//	if(*cCharInArg == 5) giRXNumCharLeft[iPortNumArg-1] = 1;

    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    	return A_OK;
	}
    }

    if ((pCommData6->m_iFD != -1) && FD_ISSET(pCommData6->m_iFD, &fdsInput))
    {
	if(pCommData6 == pCommData)
	{
    	// Read the port
<<<<<<< HEAD
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);

//int i;
=======
    	giRXNumCharLeft[iPortNumArg-1] = read(pCommData->m_iFD, caRXCommBuf[iPortNumArg-1], RX_BUF_SIZE);

//int i;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//for(i=0; i<giRXNumCharLeft[3];++i)
//printf("com4 read char=%2x charleft=%d\n",caRXCommBuf[3][i], i);
    	if (giRXNumCharLeft[iPortNumArg-1] <= 0)
    	{
		*iStatusArg = STATUS_ERR;
		return STATUS_ERR;
    	}
<<<<<<< HEAD
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];

	// clear RX buf if ENQ received 
//	if(*cCharInArg == 5) giRXNumCharLeft[iPortNumArg-1] = 1;

    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }
#endif
=======
    	*cCharInArg = caRXCommBuf[iPortNumArg-1][0];

	// clear RX buf if ENQ received 
//	if(*cCharInArg == 5) giRXNumCharLeft[iPortNumArg-1] = 1;

    	*iNumCharsLeftArg = --giRXNumCharLeft[iPortNumArg-1];
    	giRXCommBufIndex[iPortNumArg-1] = 1;
    	*iStatusArg = 0;
    	return A_OK;
	}
    }
#endif
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    // no char in port yet.
    *iStatusArg = 0;
    return IN_Q_EMPTY;
}

int COReadString (int iPortNumArg, int iNumCharsLeftArg, char *fpcaMsgBlockArg, /*int *iRetArg,*/ int *iNumCharsLeftAfterReadArg, int *iStatusArg)
{
    //int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */
    char cCharIn;
    int i, iRdone;

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    iRdone = FALSE;
<<<<<<< HEAD
    i = 0;
    for(i=0; i<=giRXNumCharLeft[iPortNumArg-1]; i++)
    {
  	fpcaMsgBlockArg[i] = caRXCommBuf[iPortNumArg-1][i];
//printf("COReadString char=%2x i=%d\n",fpcaMsgBlockArg[i], i);
    }
    *iNumCharsLeftAfterReadArg = giRXNumCharLeft[iPortNumArg-1];
    giRXNumCharLeft[iPortNumArg-1] = 0;
    *iStatusArg = A_OK;
=======
    i = 0;
    for(i=0; i<=giRXNumCharLeft[iPortNumArg-1]; i++)
    {
  	fpcaMsgBlockArg[i] = caRXCommBuf[iPortNumArg-1][i];
//printf("COReadString char=%2x i=%d\n",fpcaMsgBlockArg[i], i);
    }
    *iNumCharsLeftAfterReadArg = giRXNumCharLeft[iPortNumArg-1];
    giRXNumCharLeft[iPortNumArg-1] = 0;
    *iStatusArg = A_OK;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

//    while (!iRdone)
//    {
//	COReadChar(iPortNumArg, &cCharIn, iNumCharsLeftAfterReadArg, iStatusArg);
//	if (*iStatusArg == A_OK)
//	{
//		*fpcaMsgBlockArg[i++] = cCharIn;
//		if (cCharIn == '\0' || cCharIn == '\r' || cCharIn == '\n')
//			iRdone = TRUE;
//	}
//	else
//	{
//		*iRetArg = FAILURE;
//		return FAILURE;
//   	}
//    }
//    *iRetArg = A_OK;
    return A_OK;
<<<<<<< HEAD
}


int COReadStringCR (int iPortNumArg, char *fpcaMsgBlockArg)
=======
}


int COReadStringCR (int iPortNumArg, char *fpcaMsgBlockArg)
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
{
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */
    char cCharIn;
    int i, iNumCharsLeft, iRdone, nChar;

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;
<<<<<<< HEAD

//    SERFlushComBuffer(iPortNumArg);

    iRdone = FALSE;
    i=0;
    nChar = read(pCommData->m_iFD, fpcaMsgBlockArg, RX_BUF_SIZE);
//printf("COReadStringCR chars=%s slen=%d i=%d\n",fpcaMsgBlockArg, strlen(fpcaMsgBlockArg), i);
    iRet = A_OK;
=======

//    SERFlushComBuffer(iPortNumArg);

    iRdone = FALSE;
    i=0;
    nChar = read(pCommData->m_iFD, fpcaMsgBlockArg, RX_BUF_SIZE);
//printf("COReadStringCR chars=%s slen=%d i=%d\n",fpcaMsgBlockArg, strlen(fpcaMsgBlockArg), i);
    iRet = A_OK;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    return iRet;
}
/****************************************************************
 *
 * Function:    SERWriteChar
 *
 * Abstract:    Write a char to port
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cCharInArg      (in) The character to write
 *
 * Returns: The status of the get-data operation
 *
 ***************************************************************/
int SERWriteChar(int iPortNumArg, char cCharIn)
{
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* Refresh the SOFTWARE watchdog timer. This is frequently called in the comm
     * module since comm functions are almost always cycled through regularly.
     * It is a consistent method of resetting the watchdog counter to 0. */
//    if (TIRefreshWD() == FAILURE)
//        return FAILURE;

    // Initialize multiple ports input & output lists for select() 
    struct CommStr *pCommData1, *pCommData2;
    struct CommStr *pCommData3, *pCommData4;
    struct CommStr *pCommData5, *pCommData6;
    pCommData1 = stCommData;
    pCommData2 = stCommData + 1;
    pCommData3 = stCommData + 2;
<<<<<<< HEAD

#ifdef COMPORT6
    pCommData4 = stCommData + 3;
    pCommData5 = stCommData + 4;
    pCommData6 = stCommData + 5;
#endif
=======

#ifdef COMPORT6
    pCommData4 = stCommData + 3;
    pCommData5 = stCommData + 4;
    pCommData6 = stCommData + 5;
#endif
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    FD_SET(pCommData1->m_iFD, &fdsOutput);
    FD_SET(pCommData2->m_iFD, &fdsOutput);
    FD_SET(pCommData3->m_iFD, &fdsOutput);

#ifdef COMPORT6
<<<<<<< HEAD
    FD_SET(pCommData4->m_iFD, &fdsOutput);
    FD_SET(pCommData5->m_iFD, &fdsOutput);
    FD_SET(pCommData6->m_iFD, &fdsOutput);
#endif
=======
    FD_SET(pCommData4->m_iFD, &fdsOutput);
    FD_SET(pCommData5->m_iFD, &fdsOutput);
    FD_SET(pCommData6->m_iFD, &fdsOutput);
#endif
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    // Check if any char in port to read
    iRet = select(giMaxFD+1, NULL, &fdsOutput, NULL, &stTimeout);

    if (iRet == 0) return A_OK;
    if (iRet < 0)
    {
	switch (iRet)
	{
		case EBADF: printf("Invalid fd in select().\n\r"); break;
		case ENOMEM: printf("Unable to allocate memory in select().\n\r");break;
		case EINVAL: printf("Negative ndfs or timeout in select().\n\r"); break;
		case EINTR:  printf("Signal was caught in select().\n\r"); break;
        }
        return iRet;
    }
    if (FD_ISSET(pCommData1->m_iFD, &fdsOutput))
    {
        if (pCommData1 == pCommData)
	{
	    // Check if Number of characters left is > 0
	    if (giTXNumCharLeft[iPortNumArg-1] > 0)
	    {
		caTXCommBuf[iPortNumArg-1][giTXCommBufIndex[iPortNumArg-1]+giTXNumCharLeft[iPortNumArg-1]] = cCharIn;
	        write(pCommData->m_iFD, &caTXCommBuf[giTXCommBufIndex[iPortNumArg-1]], 1);
		++giTXCommBufIndex[iPortNumArg-1];
		return A_OK;
	    }
	    write(pCommData->m_iFD, &cCharIn, 1);
	    giTXNumCharLeft[iPortNumArg-1] = 0;
	    giTXCommBufIndex[iPortNumArg-1] = 0;
	    return A_OK;
	}
    }
    if (FD_ISSET(pCommData2->m_iFD, &fdsOutput))
    {
        if (pCommData2 == pCommData)
	{
	    // Check if Number of characters left is > 0
	    if (giTXNumCharLeft[iPortNumArg-1] > 0)
	    {
		caTXCommBuf[iPortNumArg-1][giTXCommBufIndex[iPortNumArg-1]+giTXNumCharLeft[iPortNumArg-1]] = cCharIn;
	        write(pCommData->m_iFD, &caTXCommBuf[giTXCommBufIndex[iPortNumArg-1]], 1);
		++giTXCommBufIndex[iPortNumArg-1];
		return A_OK;
	    }
	    write(pCommData->m_iFD, &cCharIn, 1);
	    giTXNumCharLeft[iPortNumArg-1] = 0;
	    giTXCommBufIndex[iPortNumArg-1] = 0;
	    return A_OK;
	}
    }
    if (FD_ISSET(pCommData3->m_iFD, &fdsOutput))
    {
        if (pCommData3 == pCommData)
	{
	    // Check if Number of characters left is > 0
	    if (giTXNumCharLeft[iPortNumArg-1] > 0)
	    {
		caTXCommBuf[iPortNumArg-1][giTXCommBufIndex[iPortNumArg-1]+giTXNumCharLeft[iPortNumArg-1]] = cCharIn;
	        write(pCommData->m_iFD, &caTXCommBuf[giTXCommBufIndex[iPortNumArg-1]], 1);
		++giTXCommBufIndex[iPortNumArg-1];
		return A_OK;
	    }
	    write(pCommData->m_iFD, &cCharIn, 1);
	    giTXNumCharLeft[iPortNumArg-1] = 0;
	    giTXCommBufIndex[iPortNumArg-1] = 0;
	    return A_OK;
	}
<<<<<<< HEAD
    }
=======
    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

#ifdef COMPORT6
    if (FD_ISSET(pCommData4->m_iFD, &fdsOutput))
    {
        if (pCommData4 == pCommData)
	{
	    // Check if Number of characters left is > 0
	    if (giTXNumCharLeft[iPortNumArg-1] > 0)
	    {
		caTXCommBuf[iPortNumArg-1][giTXCommBufIndex[iPortNumArg-1]+giTXNumCharLeft[iPortNumArg-1]] = cCharIn;
	        write(pCommData->m_iFD, &caTXCommBuf[giTXCommBufIndex[iPortNumArg-1]], 1);
		++giTXCommBufIndex[iPortNumArg-1];
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
		return A_OK;
	    }
	    write(pCommData->m_iFD, &cCharIn, 1);
	    giTXNumCharLeft[iPortNumArg-1] = 0;
<<<<<<< HEAD
	    giTXCommBufIndex[iPortNumArg-1] = 0;
=======
	    giTXCommBufIndex[iPortNumArg-1] = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
	    return A_OK;
	}
    }
<<<<<<< HEAD


=======


>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if (FD_ISSET(pCommData5->m_iFD, &fdsOutput))
    {
        if (pCommData5 == pCommData)
	{
	    // Check if Number of characters left is > 0
	    if (giTXNumCharLeft[iPortNumArg-1] > 0)
	    {
		caTXCommBuf[iPortNumArg-1][giTXCommBufIndex[iPortNumArg-1]+giTXNumCharLeft[iPortNumArg-1]] = cCharIn;
	        write(pCommData->m_iFD, &caTXCommBuf[giTXCommBufIndex[iPortNumArg-1]], 1);
		++giTXCommBufIndex[iPortNumArg-1];
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
		return A_OK;
	    }
	    write(pCommData->m_iFD, &cCharIn, 1);
	    giTXNumCharLeft[iPortNumArg-1] = 0;
<<<<<<< HEAD
	    giTXCommBufIndex[iPortNumArg-1] = 0;
=======
	    giTXCommBufIndex[iPortNumArg-1] = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
	    return A_OK;
	}
    }
<<<<<<< HEAD


=======


>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if (FD_ISSET(pCommData6->m_iFD, &fdsOutput))
    {
        if (pCommData6 == pCommData)
	{
	    // Check if Number of characters left is > 0
	    if (giTXNumCharLeft[iPortNumArg-1] > 0)
	    {
		caTXCommBuf[iPortNumArg-1][giTXCommBufIndex[iPortNumArg-1]+giTXNumCharLeft[iPortNumArg-1]] = cCharIn;
	        write(pCommData->m_iFD, &caTXCommBuf[giTXCommBufIndex[iPortNumArg-1]], 1);
		++giTXCommBufIndex[iPortNumArg-1];
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
		return A_OK;
	    }
	    write(pCommData->m_iFD, &cCharIn, 1);
	    giTXNumCharLeft[iPortNumArg-1] = 0;
<<<<<<< HEAD
	    giTXCommBufIndex[iPortNumArg-1] = 0;
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
	    return A_OK;
	}
    }
#endif
=======
	    giTXCommBufIndex[iPortNumArg-1] = 0;
//printf("com4 write char=%d, charleft=%d\n",cCharIn, giTXNumCharLeft[iPortNumArg-1]);
	    return A_OK;
	}
    }
#endif
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return A_OK;

}

/****************************************************************
 *
 * Function:    SERGetRawChar
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cCharInArg      (out) The character retrieved
 *
 * Returns: The status of the get-data operation
 *
 ***************************************************************/
int SERGetRawChar(int iPortNumArg, char *cCharInArg, int *iNumCharsLeftArg)
{
    unsigned iStatus;
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* Refresh the SOFTWARE watchdog timer. This is frequently called in the comm
     * module since comm functions are almost always cycled through regularly.
     * It is a consistent method of resetting the watchdog counter to 0. */
//    if (TIRefreshWD() == FAILURE)
//        return FAILURE;

    /* Nothing in buffer, actively try to fetch one */
//    if ((iLocal) && (iPortNumArg == COM1))
//    {
//        if (kbhit())
//        {
//            *cCharInArg = (char)getche();
//            iRet = A_OK;
//        }
//        else iRet = IN_Q_EMPTY;
//    }
//    else
//    {
        /* Read a character from the ASYNCH Manager. */
//        iRet = rdch_a1(iPortNumArg, cCharInArg, iNumCharsLeftArg, &iStatus);
	iRet =COReadChar(iPortNumArg, cCharInArg, iNumCharsLeftArg, &iStatus);
        /* If there was a read error, log it for AST to read. */
        if (iStatus & STATUS_ERR)
        {
//#ifdef SX
            if (pCommData->m_iSECS)
            {
                S2SetSecsStructureMember(COM, iPortNumArg, INTERNAL_READ, LAST_ERROR);
//                iflsh_a1(iPortNumArg);
		SERFlushRxBuff(iPortNumArg);
            }
            else
            {
                SERSetInternalCommStatus(iPortNumArg, iStatus);
            }
//#else
//            SERSetInternalCommStatus(iPortNumArg, iStatus);
//#endif
            iRet = FAILURE;
        }
//    }

    return iRet;
}


/****************************************************************
 *
 * Function:    SERRxBuffAvail
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: The number of characters available in the receive buffer
 *
 ***************************************************************/
int SERRxBuffAvail(int iPortNumArg)
{
    int iRxDone, iNumCharsLeft, iRet;
    char cCharIn, caStrBuf[3];
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    /* After getting the pointer to the array element of the selected port,
     * a verification that the port is open is necessary. */
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* Good to have watchdog refresh here for case when we waiting for receive buffer to be filled. */
//    if (TIRefreshWD() == FAILURE)
//        return FAILURE;
//#ifdef SX
    if (pCommData->m_iSECS)
    {
        /* Removed check for failure on SERGetSECSMsg.
         * In case we are in SECS mode but no SECS message came we still want
         * to retrieve from buffer what was put there before. */
        SERGetSECSMsg(iPortNumArg);
        goto exit_point;
    }
//#endif

    iRxDone = FALSE;
    while (!iRxDone)
    {
        /* Get any available character from the ASYNCH Manager. */
        iRet = SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft);
        switch (iRet)
        {
            /* A character was successfully read */
            case A_OK:
                /* Echo it if desired */
//                if ((iLocal) && (iPortNumArg == COM1))
//                {
//                    if (cCharIn == '\r') putch('\n');
//                }
//                else if (pCommData->m_iEcho)
                if (pCommData->m_iEcho)
                {
                    /* Doesn't check for FAILURE returned. Not essential. */
                    caStrBuf[0] = cCharIn;
                    caStrBuf[1] = 0;
                    SERPutsTxBuff(iPortNumArg, caStrBuf);
                    if (cCharIn == '\r')
                    {
                        caStrBuf[0] = '\n';
                        caStrBuf[1] = 0;
                        SERPutsTxBuff(iPortNumArg, caStrBuf);
                    }
<<<<<<< HEAD
    		    if (iPortNumArg == SERGetTTPort())
    		    	if (SERFlushTxBuff(iPortNumArg) == FAILURE) break;
                }

=======
    		    if (iPortNumArg == SERGetTTPort())
    		    	if (SERFlushTxBuff(iPortNumArg) == FAILURE) break;
                }

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

                /* Do nothing on the internal buffer for an ignorable character */
                if (cCharIn == cIGNOREChar) break;
                /* Backspace */
                if (cCharIn == '\b')
                {
                    /* Changes the head pointer. Disregard return code. Has no repercussions */
                    SERBackspRxCh(pCommData);
                    break;
                }
                /* An end-of-transmission character should not break
                 * the read from the asynch buffer. Only an empty buffer should.*/
                /*test for EOT */
                if (cCharIn == cEOTChar) iRxDone = TRUE;
                /* store char */
                if (SERStoreRxCh(pCommData, cCharIn) == FAILURE) iRxDone = TRUE;
<<<<<<< HEAD
//printf("cCharIn=%d numLeft=%d RxDone=%d\n",cCharIn, iNumCharsLeft,iRxDone);
=======
//printf("cCharIn=%d numLeft=%d RxDone=%d\n",cCharIn, iNumCharsLeft,iRxDone);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                break;

            /* Nothing available from ASYNCH */
            case IN_Q_EMPTY:
                iRxDone = TRUE;
                break;

            default:
                return FAILURE;
        }
    }

exit_point:
    /* Only send back a non-zero result (the number of bytes available)
     * if an end-of-transmission character (typically a carriage return)
     * is encountered in the unread portion of the internal buffer. */
    if (SERCheckForEOT(pCommData) == SUCCESS)
<<<<<<< HEAD
    {

//printf("EOT: put#=%d get#=%d\n",pCommData->m_cpRxPut, pCommData->m_cpRxGet);
=======
    {

//printf("EOT: put#=%d get#=%d\n",pCommData->m_cpRxPut, pCommData->m_cpRxGet);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

        /* Calculate the number of bytes available incorporating
         * input buffer wraparound. */
        if (pCommData->m_cpRxPut < pCommData->m_cpRxGet)
        {
            return (pCommData->m_cpRxPut+RX_BUF_SIZE-pCommData->m_cpRxGet);
        }
        return (pCommData->m_cpRxPut-pCommData->m_cpRxGet);
    }

    /* No bytes available */
    return 0;
}


/****************************************************************
 *
 * Function:    SERCheckForEOT
 *
 *
 * Parameters:
 *      pCommData   (in) Comm port data structure
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int SERCheckForEOT(struct CommStr *pCommDataArg)
{
    char *cpNextChar;

    /* Pick up the first unread character in the internal input buffer. */
    cpNextChar = pCommDataArg->m_cpRxGet;
    /* Go until there are no more bytes available to read... */
    while (cpNextChar != pCommDataArg->m_cpRxPut)
    {
        /* ...checking for a carriage return. */
        if (*cpNextChar == cEOTChar)
            return SUCCESS;
        /* Beware of input ring-buffer wraparound! */
        if (++cpNextChar == pCommDataArg->m_cpRxEnd)
        {
            cpNextChar = pCommDataArg->m_caRxBuf;
        }
    }

    return FAILURE;
}


/****************************************************************
 *
 * Function:    SERGetsRxBuff
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cpRxBuffArg     (out) Variable through which string is returned
 *      iZFlagArg       (in) Locks function and waits for EOF or
 *                          ctrl-Z before exiting (TRUE/FALSE)
 *      iNumOfBytes     (in/out) The number of bytes to read before exiting.
 *                          -1 for a normal read.
 *      iTimeout        (in) Indicates that a read until timeout is
 *                          requested (TRUE/FALSE)
 *
 * Returns: The length of the string returned
 *
 ***************************************************************/
int SERGetsRxBuff(int iPortNumArg, char *cpRxBuffArg, int iZFlagArg,
                 int *iNumOfBytes, int iTimeout)
{
    char cCharOut;
    int iNumBytesReceived = 0;
    int iQuitLoop = FALSE;
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* This argument can only have a TRUE/FALSE value. */
    if ((iZFlagArg != TRUE) && (iZFlagArg != FALSE))
        return FAILURE;

    while (!iQuitLoop)
    {
        /* Get the next available character */
        if (SERGetcRxBuff(iPortNumArg, &cCharOut, iTimeout) == FAILURE)
            return FAILURE;

        /* char received */
        iNumBytesReceived++;
        /* Put it into the string argument which is to be passed back
         * to the calling function. */
        *cpRxBuffArg++ = cCharOut;
        if (cCharOut == cEOTChar || cCharOut == 0x09)
        {
            /* If it is an end-of-transmission character (typically a
             * carriage return) */
            iQuitLoop = TRUE;
        }
        else if (iZFlagArg)
        {
            /* Stop on a control-Z if that option was selected. */
            if (cCharOut == CTRL_Z) iQuitLoop = TRUE;
        }
        else if (*iNumOfBytes > 0)
        {
            /* If a non-negative number was sent, it means a set number
             * of bytes is requested. Quit after that many bytes were read.
             * The argument is changed from the number of bytes requested to
             * be read, to the number of bytes read, before exiting. */
            if (iNumBytesReceived >= *iNumOfBytes) iQuitLoop = TRUE;
        }
        else if (pCommData->m_cpRxPut == pCommData->m_cpRxGet)
        {
            /* And finally, stop if the input buffer is empty. If the timeout feature
             * is selected, the buffer can be empty, waiting for more data, for another
             * 60 seconds before reaching this point and exiting. */
            iQuitLoop = TRUE;
        }
    }

    /* NULL terminate the string to be returned. */
    *cpRxBuffArg = 0;
    /* Return to the calling function the number of bytes read. */
    *iNumOfBytes = iNumBytesReceived;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERGetsRxBuffTM
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cpRxBuffArg     (out) Variable through which string is returned
 *      iZFlagArg       (in) Locks function and waits for EOF or
 *                          ctrl-Z before exiting (TRUE/FALSE)
 *      iNumOfBytes     (in/out) The number of bytes to read before exiting.
 *                          -1 for a normal read.
 *      iTimeout        (in) Indicates that a read until timeout is
 *                          requested (TRUE/FALSE)
 *
 * Returns: The length of the string returned
 *
 ***************************************************************/
int SERGetsRxBuffTM(int iPortNumArg, char *cpRxBuffArg, int iZFlagArg,
                 int *iNumOfBytes, int iTimeout)
{
    char cCharOut;
    int iNumBytesReceived = 0;
    int iQuitLoop = FALSE;
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* This argument can only have a TRUE/FALSE value. */
    if ((iZFlagArg != TRUE) && (iZFlagArg != FALSE))
        return FAILURE;

    while (!iQuitLoop)
    {
        /* Get the next available character */
        if (SERGetcRxBuff(iPortNumArg, &cCharOut, iTimeout) == FAILURE)
            return FAILURE;

        /* char received */
        iNumBytesReceived++;
        /* Put it into the string argument which is to be passed back
         * to the calling function. */
        *cpRxBuffArg++ = cCharOut;
        if (cCharOut == cEOTChar)
        {
            /* If it is an end-of-transmission character (typically a
             * carriage return) */
            iQuitLoop = TRUE;
        }
        else if (iZFlagArg)
        {
            /* Stop on a control-Z if that option was selected. */
            if (cCharOut == CTRL_Z) iQuitLoop = TRUE;
        }
        else if (*iNumOfBytes > 0)
        {
            /* If a non-negative number was sent, it means a set number
             * of bytes is requested. Quit after that many bytes were read.
             * The argument is changed from the number of bytes requested to
             * be read, to the number of bytes read, before exiting. */
            if (iNumBytesReceived >= *iNumOfBytes) iQuitLoop = TRUE;
        }
        else if (pCommData->m_cpRxPut == pCommData->m_cpRxGet)
        {
            /* And finally, stop if the input buffer is empty. If the timeout feature
             * is selected, the buffer can be empty, waiting for more data, for another
             * 60 seconds before reaching this point and exiting. */
            iQuitLoop = TRUE;
        }
    }

    /* NULL terminate the string to be returned. */
    *cpRxBuffArg = 0;
    /* Return to the calling function the number of bytes read. */
    *iNumOfBytes = iNumBytesReceived;

    return SUCCESS;
}

/****************************************************************
 *
 * Function:    SERGetcRxBuff
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cTxCharArg      (out) Variable through which character is returned
 *      iTimeout        (in) Indicates that a read until timeout is
 *                          requested (TRUE/FALSE)
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERGetcRxBuff(int iPortNumArg, char *cRxCharArg, int iTimeout)
{
    char cCharOut;
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    *cRxCharArg = 0;
    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* This argument can only have a TRUE/FALSE value. */
    if ((iTimeout != TRUE) && (iTimeout != FALSE))
        return FAILURE;

//    if (TIRefreshWD() == FAILURE)
//        return FAILURE;

//#ifdef SX
    if (!pCommData->m_iSECS)
    {
//#endif
        if (iTimeout)
        {
            /* Set the counter to wait the timeout duration, 60 secs.
            * The timer number was checked out in the initialization. */
            if (TISetCounter(iaTimerIndex[iPortNumArg-1], RX_TIMEOUT) == FAILURE)
                return FAILURE;
            /* Keep checking for any bytes available (up to the EOT char of course) */
            while ((iRet = SERRxBuffAvail(iPortNumArg)) <= 0)
            {
                if ((iRet == FAILURE) || (iRet = TICountExpired(iaTimerIndex[iPortNumArg-1])))
                {
                    /* If there was a failure doing a read... */
                    if (iRet == FAILURE)
                        return FAILURE;
                    /* ...or a timeout, record it appropriately. */
                    pCommData->m_iStatus |= COM_RX_TO;
                    SERSetCommErr(iPortNumArg);
                    return FAILURE;
                }
            }
        }
        else
        {
            /* If there was no timeout selected, just flush the ASYNCH buffer
             * out to the internal buffer. */
            iRet = SERRxBuffAvail(iPortNumArg);
        }
        if (iRet == FAILURE)
            return FAILURE;
//#ifdef SX
    }
//#endif

    /* Check for data already in buffer. Return an EOF (-1) by default.
     * Beware that the ASYNCH Manager (through the SERGetRawChar function)
     * returns a 0 by default. */
    *cRxCharArg = EOF;
    /* If there is data available get it out of the internal input buffer
     * (and adjust the tail pointer.) We take no other action on a FAILURE
     * return (i.e. internal input buffer empty) because we return an
     * EOF by default when the buffer is empty. */
    if (SERFetchRxCh(pCommData, &cCharOut) != FAILURE)
    {
        *cRxCharArg = cCharOut;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERPutsTxBuff
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      cpTxCharArg     (in) variable with string to send
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERPutsTxBuff(int iPortNumArg, char *cpTxStrArg)
{
    char cCharOut, *cpTxStrTemp;
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
    {   //printf("SERPutsTxBuf failed SERCheckValidPort\n");
        return FAILURE;
    }
    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
    {   //printf("SERPutsTxBuf port not opened\n");
        return FAILURE;
    }

//    if ((iLocal) && (iPortNumArg == COM1))
//    {
        /* If COM1 is operating remote, just print the string to the
         * screen. No hardware action is necessary. */
//        cprintf("%s", cpTxStrArg);
//        return SUCCESS;
//    }

    cpTxStrTemp = cpTxStrArg;
    while (*cpTxStrTemp != 0)
    {
//        if (TIRefreshWD() == FAILURE)
//	    {   //printf("SERPutsTxBuf failed TIRefreshWD\n");
//	        return FAILURE;
//	    }

        /* Get the next character in the string to send. */
        cCharOut = *cpTxStrTemp++;

        /* If there is room available, put the character into the internal
         * output buffer (and adjust the head pointer.) If a FAILURE is returned,
         * i.e. buffer full, then...*/
        if (SERStoreTxCh(pCommData, cCharOut) == FAILURE)
        {
//#ifdef SX
            /* Don't flush the buffer if in Secs mode, it will be sent out by Secs module. */
            if (!pCommData->m_iSECS)
            {
//#endif
                /* ...flush the buffer... */
                if (SERFlushTxBuff(iPortNumArg) != FAILURE)
                {
                    /* ...and try to store the character again. */
                    if (SERStoreTxCh(pCommData, cCharOut) == FAILURE)
		    {   //printf("SERPutsTxBuf failed SERStoreTxCh\n");
		        return FAILURE;
		    }
                }
                else
                    return FAILURE;
//#ifdef SX
            }
            else
                return FAILURE;
//#endif
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERFlushTxBuff
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERFlushTxBuff(int iPortNumArg)
{
    int iNumBytesWritten, iLength, iRet;
    char cCharOut;
    static char caStrOut[MAXLINE];
    char *cpStrOutTemp;
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    /* If COM1 is operating remote, "cprintf" will be handled by
     * DOS. i.e. fire-and-forget */
//    if ((iLocal) && (iPortNumArg == COM1)) return SUCCESS;

    cpStrOutTemp = caStrOut;
    /* If there is data available get it out of the internal output buffer and
     * send it out the comm port (and adjust the tail pointer.) On a FAILURE
     * return (i.e. internal output buffer empty), we simply continue and wait
     * for the ASYNCH Manager to finish sending all the characters. */
    while (SERFetchTxCh(pCommData, &cCharOut) != FAILURE)
    {
        *cpStrOutTemp++ = cCharOut;
    }
    *cpStrOutTemp = 0;
    cpStrOutTemp = caStrOut;

//#ifdef SX
    if (pCommData->m_iSECS)
    {
        /* If there is something in the transmit buffer and it doesn't come from the macro
         * build it into S2F66 message and send it away. */
        if (cpStrOutTemp && (MRGetMacroStatus() != MACRO_RUNNING))
        {
            if(S2PutTogetherS2F66Response(iPortNumArg, cpStrOutTemp) == FAILURE)
                return FAILURE;
        }
        if (SERSendSECSMsg(iPortNumArg) == FAILURE)
            return FAILURE;
        return SUCCESS;
    }
//#endif

    if (TISetCounter(iaTimerIndex[iPortNumArg-1], TX_TIMEOUT) == FAILURE)
        return FAILURE;

    /* Write the whole string to the comm port. */
    iLength = strlen(cpStrOutTemp);
    while (iLength > 0)
    {
        /* Try to write the string to the ASYNCH Manager buffer at once. */
//        iRet = wrtst_a1(iPortNumArg, iLength, cpStrOutTemp, &iNumBytesWritten);
 	iRet = SERWriteString(iPortNumArg, iLength, cpStrOutTemp, &iNumBytesWritten);
        switch (iRet)
        {
            case A_OK:
                iLength = 0;
                break;

            case OUT_Q_FULL:
                /* Retry if the output buffer is full. */
//                if (TIRefreshWD() == FAILURE)
//                    return FAILURE;
                iRet = TICountExpired(iaTimerIndex[iPortNumArg-1]);
		if (iRet)
                {
                    if (iRet == FAILURE)
                        return FAILURE;
                    stCommData[iPortNumArg].m_iStatus |= COM_TX_TO;
                    SERSetCommErr(iPortNumArg);
                    return FAILURE;
                }
                /* Since some ASYNCH buffer space might free up, but
                 * not necessarily enough to write the rest of the
                 * string, you have to keep track of how many characters
                 * still have to be written out. */
                cpStrOutTemp += iNumBytesWritten;
                iLength -= iNumBytesWritten;
                break;

            default:
                return FAILURE;
        }
    }

    return SUCCESS;
}

int SERFlushRxBuff(int iPortNumArg)
{
    //int iNumBytesRead, iLength; //, iRet;
    //char cCharOut;
    //static char caStrOut[MAXLINE];
    //char *cpStrOutTemp;
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;


    return SUCCESS;
}

/****************************************************************
 *
 * Function:    SERWaitTxBuff
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERWaitTxBuff(int iPortNumArg)
{
    int iNumCharsLeft, iRet;

    if (TISetCounter(iaTimerIndex[iPortNumArg-1], TX_TIMEOUT) == FAILURE)
        return FAILURE;
    while ((iRet = TICountExpired(iaTimerIndex[iPortNumArg-1])) != TRUE)
    {
        if (iRet == FAILURE)
            return FAILURE;
//        if (TIRefreshWD() == FAILURE)
//            return FAILURE;
        /* Ask the ASYNCH Manager how many bytes are still left to transmit. */
//        iRet = oqsiz_a1(iPortNumArg, &iNumCharsLeft);
	iRet = SERBufTXLeft(iPortNumArg, &iNumCharsLeft);
        if (iRet != A_OK)
            return FAILURE;
        if (!iNumCharsLeft)
            return SUCCESS;
    }

    stCommData[iPortNumArg-1].m_iStatus |= COM_TX_TO;
    SERSetCommErr(iPortNumArg);
    return FAILURE;
}


/****************************************************************
 *
 * Function:    SERSetEOTChar
 *
 * Parameters:
 *      cEOTCharArg     (in) New value for the EOT character
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSetEOTChar(char cEOTCharArg)
{
    cEOTChar = cEOTCharArg;
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERSetIGNOREChar
 *
 * Parameters:
 *      cIGNORECharArg  (in) New value for the IGNORE character
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSetIGNOREChar(char cIGNORECharArg)
{
    cIGNOREChar = cIGNORECharArg;
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERGetCommVals
 *
 * Parameters:
 *      iPortNumArg     (in) The port number to operate on
 *      iValueDescArg   (in) The descriptor of the variable to get
 *      iValueArg       (out) The current value of the variable
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERGetCommVals(int iPortNumArg, int iValueDescArg, int *iValueArg)
{
    int iModemStatRegVal = -1;
    int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */
    int iSerCmd, iSerStat;

    if (iValueDescArg != DC_MODE)
    {
        if (SERCheckValidPort(iPortNumArg) == FAILURE)
            return FAILURE;
        pCommData = stCommData+iPortNumArg-1;
        if (iValueDescArg != PORT_OPEN)
        {
            if (!pCommData->m_iPortOpen)
                return FAILURE;
        }
    }

    switch (iValueDescArg)
    {
        case FLOW_CTRL :
            /* Ask ASYNCH Manager for the CTS status. */
//            iRet = retop_a1(iPortNumArg, ACT_MODEM_STAT_REG, &iModemStatRegVal);
//            if (iRet != A_OK)
//                return FAILURE;
            // get the current comport setting
  	    iSerCmd = TIOCM_CTS;
  	    iRet = ioctl(pCommData->m_iFD, TIOCMBIS, &iSerCmd);
  	    if (iRet == -1) 
	    {
    		//printf("TIOCMBIS returned error: %d.\n", errno);
		return FAILURE;
	    }
  	    // read the CTS pin status
  	    iRet = ioctl(pCommData->m_iFD, TIOCMGET, &iSerStat);
  	    if (iRet == -1)
	    {
    	        //printf("TIOCMGET returned error: %d.\n", errno);
		return FAILURE;
	    }

  	    if (iSerStat & TIOCM_CTS)
    		iModemStatRegVal = 1;
  	    else
    		iModemStatRegVal = 0;
            // set the new setting to the comport
            pCommData->m_iFlowCtrl = iModemStatRegVal;
            *iValueArg = iModemStatRegVal;
            break;

        case ECHO_MODE :
            *iValueArg = pCommData->m_iEcho;
            break;

        case COMM_STATUS :
            *iValueArg = pCommData->m_iStatus;
            break;

        case DC_MODE :
            *iValueArg = iDCMode;
            break;

        case SECS_MODE :
            *iValueArg = pCommData->m_iSECS;
            break;

        case PORT_OPEN :
            *iValueArg = pCommData->m_iPortOpen;
            break;

        default:
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERSetCommVals
 *
 * Parameters:
 *      iPortNumArg     (in) The port number to operate on
 *      iValueDescArg   (in) The descriptor of the variable to set
 *      iValueArg       (out) The new value of the variable
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSetCommVals(int iPortNumArg, int iValueDescArg, int iValueArg)
{
    //int iRet;                   /* Return code. Used wherever needed. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */
    long lRtscts;

    if (iValueDescArg != DC_MODE)
    {
        if (SERCheckValidPort(iPortNumArg) == FAILURE)
        { //printf("SERSetCommVals check valid port failed.\n");
	    return FAILURE;
	}
        pCommData = stCommData+iPortNumArg-1;
        if (iValueDescArg != PORT_OPEN)
        {
            if (!pCommData->m_iPortOpen)
	        { //printf("SERSetCommVals port not opened.\n");
		    return FAILURE;
		}
        }
        if ((iValueDescArg != COMM_STATUS) && (iValueDescArg != TT_PORT) && (iValueDescArg != CMD_PORT))
        {
            if ((iValueArg != TRUE) && (iValueArg != FALSE))
	        { //printf("SERSetCommVals wrong iValueArg = %d\n", iValueArg);
		    return FAILURE;
		}
        }
    }

    switch (iValueDescArg)
    {
        case FLOW_CTRL :
            pCommData->m_iFlowCtrl = iValueArg;
            // get the current comport setting
            tcgetattr (pCommData->m_iFD, &pCommData->m_NewTIO);
            // set the new setting to the comport
            lRtscts = 0;
	    if (iValueArg) lRtscts = CRTSCTS;
	    pCommData->m_NewTIO.c_cflag &= ~CRTSCTS;
	    pCommData->m_NewTIO.c_cflag |= lRtscts;
	    tcsetattr(pCommData->m_iFD, TCSANOW, &pCommData->m_NewTIO);
//            if (iValueArg)
//            {
                /* CTS required to send */
//                iRet = setop_a1(iPortNumArg, REQ_CTS_OPT, 1);
//            }
//            else
//            {
                /* CTS not required to send */
//                iRet = setop_a1(iPortNumArg, REQ_CTS_OPT, 0);
//           }
//            if (iRet != A_OK)
//                return FAILURE;
            break;

        case ECHO_MODE :
            pCommData->m_iEcho = iValueArg;
            break;

        case COMM_STATUS :
            pCommData->m_iStatus = iValueArg;
            break;

        case DC_MODE :
            iDCMode = iValueArg;
            break;

        case SECS_MODE :
            pCommData->m_iSECS = iValueArg;
            break;

        case PORT_OPEN :
            pCommData->m_iPortOpen = iValueArg;
            break;

        case TT_PORT :
            giTTPort = iPortNumArg;
            break;

        case CMD_PORT :
            giCmdPort = iPortNumArg;
            break;

        default:
	    //printf("SERSetCommVals wrong description value = %d\n", iValueDescArg);
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERPortRedirect
 *
 * Parameters:
 *      iPortToRedirectToArg    (in) New value of the redirection setting
 *                                  -1 for none; otherwise port that it
 *                                  is redirected to
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERPortRedirect(int iPortToRedirectToArg)
{
    unsigned  iStatus;
    int iNumCharsLeft, iRet;
    char cCharIn, caPrompt[5];
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if ((iPortToRedirectToArg != COM2) && (iPortToRedirectToArg != COM3))
        return FAILURE;

    if (iPortToRedirectToArg == NO_PORT_NUM)
        return FAILURE;

    pCommData = stCommData+COM1-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    pCommData = stCommData+iPortToRedirectToArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    if (TIGetTimerVals(LOCKED) == 123)
    {
	char aPrompt = '$';
        sprintf(caPrompt, "\r\n%c", aPrompt);
//        sprintf(caPrompt, "\r\n%c", PCGetWDPrompt());
    }
    else
        sprintf(caPrompt, "\r\n>");
    if (SERPutsTxBuff(SERGetCmdPort(), caPrompt) == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    while (1)
    {
//        if (TIRefreshWD() == FAILURE)
//            return FAILURE;

        /* Get a character from COM1 */
//        iRet = rdch_a1(COM1, &cCharIn, &iNumCharsLeft, &iStatus);
	iRet = COReadChar(COM1, &cCharIn, &iNumCharsLeft, &iStatus);
        /* Make sure you got it OK */
        switch (iRet)
        {
            case A_OK:
            case IN_Q_EMPTY:
                break;

            default:
                return FAILURE;
        }

        if (iRet == A_OK)
        {
            /* Stop at CTRL-K */
            if ((cCharIn == 0x0B) && (SERGetCmdPort() == COM1)) break;
            /* Otherwise echo it out to the redirection port */
//            iRet = wrtch_a1(iPortToRedirectToArg, cCharIn);
	    iRet = SERWriteChar(iPortToRedirectToArg, cCharIn);
            if (iRet != A_OK)
                return FAILURE;
        }

        /* Get a character from the redirection port */
//        iRet = rdch_a1(iPortToRedirectToArg, &cCharIn, &iNumCharsLeft, &iStatus);
	iRet = COReadChar(iPortToRedirectToArg, &cCharIn, &iNumCharsLeft, &iStatus);

        /* Make sure you got it OK */
        switch (iRet)
        {
            case A_OK:
            case IN_Q_EMPTY:
                break;

            default:
                return FAILURE;
        }

        if (iRet == A_OK)
        {
            /* Stops at CTRL-K */
            if ((cCharIn == 0x0B) && (SERGetCmdPort() == COM2 )) break;
            /* Otherwise echo it out to the redirection port */
//            iRet = wrtch_a1(COM1, cCharIn);
	    iRet = SERWriteChar(COM1, cCharIn);
            if (iRet != A_OK)
                return FAILURE;
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERGetTTPort
 *
 * Parameters:
 *      None
 *
 * Returns: The current teach terminal port
 *
 ***************************************************************/
int SERGetTTPort()
{
    return giTTPort;
}


/****************************************************************
 *
 * Function:    SERGetCmdPort
 *
 * Abstract:    Returns the current command port number
 *
 * Parameters:
 *      None
 *
 * Returns: The current command port number
 *
 ***************************************************************/
int SERGetCmdPort()
{
    return giCmdPort;
}


/****************************************************************
 *
 * Function:    SERGetNumCharsInBuffer
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERGetNumCharsInBuffer(int iPortNumArg)
{
    int iNumChars;
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData+iPortNumArg-1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    iNumChars = (int)(pCommData->m_cpRxPut-pCommData->m_cpRxGet);
    if (iNumChars<0)
        iNumChars += MAXLINE;

    return iNumChars;
}


/****************************************************************
 *
 * Function:    SERCheckValidPort
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERCheckValidPort(int iPortNumArg)
{
    if ((iPortNumArg < COM1) || (iPortNumArg > COM6))
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERFetchRxCh
 *
 * Parameters:
 *      pCommData   (in) Comm port data structure
 *
 * Returns: The fetched character
 *
 ***************************************************************/
int SERFetchRxCh(struct CommStr *pCommDataArg, char *cCharToFetch)
{
    char *cpNextChar;

    /* Set temporary pointer to current position in input buffer array */
    cpNextChar = pCommDataArg->m_cpRxGet;

    /* No more data; input buffer empty */
    if (cpNextChar == pCommDataArg->m_cpRxPut)
        return FAILURE;

    /* Get the character */
    *cCharToFetch = *pCommDataArg->m_cpRxGet;

    /* Increment pointer; if at end of input buffer, wrap back to beginning */
    if ((cpNextChar = pCommDataArg->m_cpRxGet+1) == pCommDataArg->m_cpRxEnd)
    {
        cpNextChar = pCommDataArg->m_caRxBuf;
    }

    /* Set current location in input buffer to position of last character retrieved */
    pCommDataArg->m_cpRxGet = cpNextChar;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERStoreRxCh
 *
 * Parameters:
 *      pCommData   (in) Comm port data structure
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERStoreRxCh(struct CommStr *pCommDataArg, char cCharToStore)
{
    char *cpNextChar;

    /* If at end of input buffer, wrap back to beginning */
    if ((cpNextChar = pCommDataArg->m_cpRxPut+1) == pCommDataArg->m_cpRxEnd)
    {
        cpNextChar = pCommDataArg->m_caRxBuf;
    }

    /* No more room; input buffer full */
    if (cpNextChar == pCommDataArg->m_cpRxGet)
        return FAILURE;

    /* Store the character */
    *pCommDataArg->m_cpRxPut = cCharToStore;
    /* Set current location in input buffer to position of new charcter entered */
    pCommDataArg->m_cpRxPut = cpNextChar;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERBackspRxCh
 *
 * Parameters:
 *      pCommData   (in) Comm port data structure
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERBackspRxCh(struct CommStr *pCommDataArg)
{
    /* Input buffer is empty */
    if (pCommDataArg->m_cpRxPut == pCommDataArg->m_cpRxGet)
        return FAILURE;

    /* If at beginning of input buffer, wrap back to end */
    if (pCommDataArg->m_cpRxPut == pCommDataArg->m_caRxBuf)
    {
        pCommDataArg->m_cpRxPut = pCommDataArg->m_cpRxEnd;
    }

    /* Decrement input ring-buffer head pointer */
    pCommDataArg->m_cpRxPut--;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERFetchTxCh
 *
 * Parameters:
 *      pCommData   (in) Comm port data structure
 *
 * Returns: The fetched character
 *
 ***************************************************************/
int SERFetchTxCh(struct CommStr *pCommDataArg, char *cCharToFetch)
{
    char *cpNextChar;

    /* Set temporary pointer to current position in output buffer array */
    cpNextChar = pCommDataArg->m_cpTxGet;

    /* No more data; output buffer empty */
    if (cpNextChar == pCommDataArg->m_cpTxPut)
        return FAILURE;

    /* Get the character */
    *cCharToFetch = *pCommDataArg->m_cpTxGet;

    /* Increment pointer; if at end of output buffer, wrap back to beginning */
    if ((cpNextChar = pCommDataArg->m_cpTxGet+1) == pCommDataArg->m_cpTxEnd)
    {
        cpNextChar = pCommDataArg->m_caTxBuf;
    }

    /* Set current location in output buffer to position of last character retrieved */
    pCommDataArg->m_cpTxGet = cpNextChar;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERStoreTxCh
 *
 * Parameters:
 *      pCommData   (in) Comm port data structure
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERStoreTxCh(struct CommStr *pCommDataArg, char cCharToStore)
{
    char *cpNextChar;

    /* If at end of output buffer, wrap back to beginning */
    if ((cpNextChar = pCommDataArg->m_cpTxPut+1) == pCommDataArg->m_cpTxEnd)
    {
        cpNextChar = pCommDataArg->m_caTxBuf;
    }

    /* No more room; output buffer full */
    if (cpNextChar == pCommDataArg->m_cpTxGet)
        return FAILURE;

    /* Store the character */
    *pCommDataArg->m_cpTxPut = cCharToStore;
    /* Set current location in output buffer to position of new charcter entered */
    pCommDataArg->m_cpTxPut = cpNextChar;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERSetInternalCommStatus
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *      iStatus         (in) Port status returned by Asynch.obj
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSetInternalCommStatus(int iPortNumArg, int iStatus)
{
    int *pCommDataStat;

    /* Get a pointer to the status word of the port internal structure. */
    pCommDataStat = &stCommData[iPortNumArg-1].m_iStatus;

    /* If it was a serious error... */
    if (iStatus & STATUS_ERR)
    {
        /* ...set the internal status word for AST... */
        *pCommDataStat |= COM_DEV_ERR;
        /* ...as well as the status bit for STAT. */
        SERSetCommErr(iPortNumArg);
    }

    /* Otherwise, just set the internal status word bit(s) appropriately. */
    if (iStatus & 0x01)  *pCommDataStat |= COM_LOST_ERR;
    if (iStatus & 0x02)  *pCommDataStat |= COM_ASYN_ERR;
    if (iStatus & 0x10)  *pCommDataStat |= COM_PAR_ERR;
    if (iStatus & 0x20)  *pCommDataStat |= COM_OVR_ERR;
    if (iStatus & 0x40)  *pCommDataStat |= COM_FRM_ERR;
    if (iStatus & 0x80)  *pCommDataStat |= COM_BREAK;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERSetCommErr
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSetCommErr(int iPortNumArg)
{
    /* Set the status bit of the error variable accessed by the STAT
     * host command. */
    if (iPortNumArg == COM1)
    {
        SSSetStatusWord(COM1_ERR, TRUE);
    }
    else if (iPortNumArg == COM2)
    {
        SSSetStatusWord(COM2_ERR, TRUE);
    }
    else if (iPortNumArg == COM3)
    {
        SSSetStatusWord2(COM3_ERR, TRUE);
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERGetHistoryList
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERGetHistoryList(int iPortNumArg)
{
    char *cpHistGet;
    /* Pointer to data structure. Changes for each port */
    struct CommStr *pCommDataTemp;
    /* The command string to print. The Get pointer just keeps track
     * of where new characters are to placed in the string.*/
    char  *caTempStr;
    char  *cpTempGet;
    int iRet;                   /* Return code. Used wherever needed. */

    pCommDataTemp = stCommData+iPortNumArg-1;
    if (!pCommDataTemp->m_iPortOpen)
        return FAILURE;

    /* Initialization */
//    cpTempGet = caTempStr = (char *)HPmalloc(sizeof(char)*MAXLINE);
    cpTempGet = caTempStr = (char *)malloc(sizeof(char)*MAXLINE);
    cpHistGet = pCommDataTemp->m_cpRxPut;

    /* Dump processed data from the internal comm buffer */
    do
    {
        if (*cpHistGet != 0)
        {
            /* Get characters from the buffer 1 by 1 */
            *cpTempGet = *cpHistGet;
            cpTempGet++;
        }
        if (*cpHistGet == '\r')
        {
            *cpTempGet = '\n';
            /* After a carriage return, NULL terminate the string */
            *(++cpTempGet) = 0;
            /* If the string is NOT just a carriage return, print it.
             * (Empty lines are NOT displayed) */
            if (strlen(caTempStr) > 2)
            {
                if ((iRet=SERPutsTxBuff(giCmdPort, caTempStr)) == FAILURE) goto error_exit;
                if ((iRet=SERFlushTxBuff(giCmdPort)) == FAILURE) goto error_exit;
                if ((iRet=SERWaitTxBuff(giCmdPort)) == FAILURE) goto error_exit;
            }
            /* Set the current character pointer back to the beginning
             * of the display string. */
            cpTempGet = caTempStr;
        }
        /* Pick up the next character from the internal input buffer. */
        if (++cpHistGet == pCommDataTemp->m_cpRxEnd)
        {
            cpHistGet = pCommDataTemp->m_caRxBuf;
        }
    } while (cpHistGet != pCommDataTemp->m_cpRxPut);

    /* Apply a carriage return, line feed, and NULL string termination
     * to the last entry from the internal buffer. */
    *cpTempGet = '\r';
    *(++cpTempGet) = '\n';
    *(++cpTempGet) = 0;
    /* If it is NOT a blank line, print it out. */
    if (strlen(caTempStr) > 2)
    {
        if ((iRet=SERPutsTxBuff(giCmdPort, caTempStr)) == FAILURE) goto error_exit;
        if ((iRet=SERFlushTxBuff(giCmdPort)) == FAILURE) goto error_exit;
    }
    /* Make sure it was completely output before exiting. */
    iRet = SERWaitTxBuff(giCmdPort);

error_exit:
//    HPfree(caTempStr);
//    free(caTempStr);
    return iRet;
}


/****************************************************************
 *
 * Function:    SERFlushComBuffer
 *
 * Parameters:
 *      iPortNumArg     (in) Comm port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERFlushComBuffer(int iPortNumArg)
{
    int iNotEmpty = TRUE;
    char cByte;

    while (iNotEmpty)
    {
        if (SERGetcRxBuff(iPortNumArg, &cByte, FALSE) == FAILURE)
            return FAILURE;
        if (cByte == EOF)
            iNotEmpty = FALSE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERCheckINXMode
 *
 * Parameters:  None
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERCheckINXMode()
{
    int iOnOffFlag;         /* The value of the configuration file COM1 SECS setting. */
    int iCTS;               /* The new CTS value. Used in conjuction with iPrevCTS. */
    unsigned int uiSECSStat;/* The current SECS activity status. */
    //char cCharFromCom2;     /* A character from the teach pendant comm buffer.
     //                        * It has a non-EOF value used to initiate teach or diagnostics mode. */
    struct CommStr *pCommData;  /* Pointer to data structure. Changes for each port.
                                 * This is set in the beginning of all functions.
                                 * This common pointer can then be used generically on
                                 * any port. */

    /* get the feature active/deactive value */
    iOnOffFlag = FALSE;
    if (FIOGetCfgFeatureFlags(SECS_DEVICE, &iOnOffFlag) == FAILURE)
        iOnOffFlag = FALSE;
    if (iOnOffFlag)
    {
//#ifdef SX
        /* Get the current SECS activity status. */
        uiSECSStat = IDLE;
        S2GetSecsStructureMember(COM, COM1, &uiSECSStat, WTCURR_ACTIVITY);
//#else
//        uiSECSStat = IDLE;
//#endif

        /* Don't check for CTS/RTS drop if SECS is not idle. */
        if (uiSECSStat != IDLE)
            return SUCCESS;

        pCommData = stCommData+COM1-1;
        if ((SERGetCommVals(COM1, FLOW_CTRL, &iCTS) == SUCCESS) && (pCommData->m_iPortOpen))
        {
            /* Checks if CTS/RTS value is equal to its previous value. If there was
            * just a blip and CTS/RTS has not actually been dropped, ignore the blip. */
            if( iCTS != iPrevCTSCOM1 )
            {
                /* If there was an actual change, service it and don't register it
                * the next time through this main loop by setting the current and
                * previous CTS/RTS values equal. */
                iPrevCTSCOM1 = iCTS;
                /* Is it on? */
                if( iCTS & CTS )
                {
                    /* Transmit flow control */
                    SERSetCommVals(COM1, FLOW_CTRL, TRUE);

                    /* Receive flow control */
//                    setop_a1(COM1, LOC_HARD_FLOW_CTRL_OPT, 3);          /* both RTS & DTR set by flow ctrl */
//                    setop_a1(COM1, RDY_LOC_HARD_FLOW_CTRL_OPT, 3);      /* open/ready:    RTS high  DTR high  */
//                    setop_a1(COM1, UNRDY_LOC_HARD_FLOW_CTRL_OPT, 1);    /* open/unready:  RTS low   DTR low   */

                    SERSetCommVals(COM1, SECS_MODE, FALSE);
                }
                else
                {
                    /* Transmit flow control */
                    SERSetCommVals(COM1, FLOW_CTRL, FALSE);

                    /* Receive flow control */
//                    setop_a1(COM1, LOC_HARD_FLOW_CTRL_OPT, 0);          /* both RTS & DTR set by flow ctrl */
//                    setop_a1(COM1, RDY_LOC_HARD_FLOW_CTRL_OPT, 0);      /* open/ready:    RTS high  DTR high  */
//                    setop_a1(COM1, UNRDY_LOC_HARD_FLOW_CTRL_OPT, 1);    /* open/unready:  RTS low   DTR low   */

//                    SERSetCommVals(COM1, SECS_MODE, TRUE);
                }
            }
        }
    }

//#ifdef SX
    /* Get the current SECS activity status. */
    uiSECSStat = IDLE;
    S2GetSecsStructureMember(COM, COM2, &uiSECSStat, WTCURR_ACTIVITY);
//#else
//    uiSECSStat = IDLE;
//#endif

    /* Don't check for CTS/RTS drop if SECS is not idle. */
    if (uiSECSStat != IDLE)
        return SUCCESS;

    /* Check for CTS/RTS drop on the teach pendant port. */
    pCommData = stCommData+COM2-1;
    if ((SERGetCommVals(COM2, FLOW_CTRL, &iCTS) == SUCCESS) && (pCommData->m_iPortOpen))
    {
        /* Checks if CTS/RTS value is equal to its previous value. If there was
            * just a blip and CTS/RTS has not actually been dropped, ignore the blip. */
        if( iCTS != iPrevCTSCOM2 )
        {
//printf("\n\r iPrevCTSCOM2 = %d  iCTS = %d\n\r", iPrevCTSCOM2, iCTS);
            /* If there was an actual change, service it and don't register it
                * the next time through this main loop by setting the current and
                * previous CTS/RTS values equal. */
            iPrevCTSCOM2 = iCTS;
            /* Is it on? */
            if( iCTS ) // & CTS )
            {
                /* Transmit flow control */
//                SERSetCommVals(COM2, FLOW_CTRL, TRUE);

                /* Receive flow control */
//                setop_a1(COM2, LOC_HARD_FLOW_CTRL_OPT, 3);          /* both RTS & DTR set by flow ctrl */
//                setop_a1(COM2, RDY_LOC_HARD_FLOW_CTRL_OPT, 3);      /* open/ready:    RTS high  DTR high  */
//                setop_a1(COM2, UNRDY_LOC_HARD_FLOW_CTRL_OPT, 1);    /* open/unready:  RTS low   DTR low   */

                SERSetCommVals(COM2, SECS_MODE, FALSE);
                /* Yes, display the Equipe Logo. */
               	TTShowLogo();
            }
            else
            {
                /* Transmit flow control */
//                SERSetCommVals(COM2, FLOW_CTRL, FALSE);

                /* Receive flow control */
//                setop_a1(COM2, LOC_HARD_FLOW_CTRL_OPT, 0);          /* both RTS & DTR set by flow ctrl */
//                setop_a1(COM2, RDY_LOC_HARD_FLOW_CTRL_OPT, 0);      /* open/ready:    RTS high  DTR high  */
//                setop_a1(COM2, UNRDY_LOC_HARD_FLOW_CTRL_OPT, 1);    /* open/unready:  RTS low   DTR low   */

                /* Is the teach pendant active as Terminal? */
                if( COM2 == giCmdPort )
                {
                    /* CTS dropped. So prepare COM1/EQT port to be
                        * used as active terminal. WARNING!!! The active terminal
                        * moves to the EQT port automatically if the teach
                        * pendant was unplugged. If the EQT cable is then also
                        * disconnected, CTS/RTS is dropped on that line too which
                        * may cause a COM1 failure and stop macro execution. */
                    SERFlushComBuffer(COM1);
                    SERSetCommVals(COM1, CMD_PORT, TRUE);
                }
//                SERSetCommVals(COM2, SECS_MODE, TRUE);
            }
            /* Why????? */
//            SERGetcRxBuff(COM2, &cCharFromCom2, FALSE);
            SERSetCommVals(COM2, COMM_STATUS, 0);
//            SSSetStatusWord(COM2_ERR, FALSE);
        }
    }



    /* We want to check COM3 status only if COM3 is supported, otherwise it causes
     * failed return and doesn't check for COM1. */
    return SUCCESS;
}

int iNeed = 1;
//#ifdef SX
/****************************************************************
 *
 * Function:    SERGetSECSMsg
 *
 * Parameters:
 *      iPortNumArg     Com port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERGetSECSMsg(int iPortNumArg)
{
    int iNumCharsLeft, iStatus, iNumCharsLeftAfterRead, iRet;
    unsigned int uiCurrActivity, uiTimeOutInterval, uiNumRetries, uiMaxNumRetries;
    unsigned long ulTimeOut;
    char cCharIn;
    char *fpcaMsgBlock;
<<<<<<< HEAD
    int i, iLen;
    int iTotRead, iNumRead;
    char caMsgRest[256];

    if (S2GetSecsStructureMember(COM, iPortNumArg, &uiCurrActivity, WTCURR_ACTIVITY) == FAILURE) return FAILURE;


    switch (uiCurrActivity)
    {

        case IDLE:          /* Idle, check for ENQ. */
	    uiEOTretries = 0;
            if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE)
=======
    int i, iLen;
    int iTotRead, iNumRead;
    char caMsgRest[256];

    if (S2GetSecsStructureMember(COM, iPortNumArg, &uiCurrActivity, WTCURR_ACTIVITY) == FAILURE) return FAILURE;


    switch (uiCurrActivity)
    {

        case IDLE:          /* Idle, check for ENQ. */
	    uiEOTretries = 0;
            if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE)
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		return FAILURE;
            if (iRet == A_OK)
            {
                if (iNumCharsLeft)
                {           // Got something, NOT ENQ. 
                    S2SetSecsStructureMember(COM, iPortNumArg, INVALID_ENQ, LAST_ERROR);
<<<<<<< HEAD
		    SERFlushRxBuff(iPortNumArg);
=======
		    SERFlushRxBuff(iPortNumArg);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
printf("SERGetSECSMsg: IDLE, got not ENQ.\n");
                    return FAILURE;
                }
                else if (cCharIn == ENQ)
                {           /* Got ENQ. */
		    iRet = SERWriteChar(iPortNumArg, EOT);
                    if (iRet != A_OK)
                    {
                        S2SetSecsStructureMember(COM, iPortNumArg, INTERNAL_WRITE, LAST_ERROR);
			SERWriteChar(iPortNumArg, NAK);
printf("SERGetSECSMsg: IDLE, received ENQ, write EOT error.\n");
                        return FAILURE;
<<<<<<< HEAD
                    }
                    /* Sending EOT, ready to receive data. */
                    S2SetSecsStructureMember(COM, iPortNumArg, SEND_EOT, WTCURR_ACTIVITY);
                    S2SetTimeOut(COM, iPortNumArg, 0L);

printf("SERGetSECSMsg: IDLE, received ENQ, write EOT, set SEND_EOT.\n");

		    TIDelay(50);


                }
            }
            break;

  	case AWAIT_EOT:	// ENQ sent look for EOT
=======
                    }
                    /* Sending EOT, ready to receive data. */
                    S2SetSecsStructureMember(COM, iPortNumArg, SEND_EOT, WTCURR_ACTIVITY);
                    S2SetTimeOut(COM, iPortNumArg, 0L);

printf("SERGetSECSMsg: IDLE, received ENQ, write EOT, set SEND_EOT.\n");

		    TIDelay(50);


                }
            }
            break;

  	case AWAIT_EOT:	// ENQ sent look for EOT
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
            if (!ulTimeOut)
            {
            	if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
            	if (cCharIn == EOT)
            	{
                    S2SetSecsStructureMember(COM, iPortNumArg, SEND_MESSAGE, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERGetSECSMsg3 cur activity=%d --> 14\n", uiCurrActivity);
iNeed = 1;
=======
//printf("SERGetSECSMsg3 cur activity=%d --> 14\n", uiCurrActivity);
iNeed = 1;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    S2GetTMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
                    iStatus = (int)(fpcaMsgBlock[0]) + 3;
//                  iRet =  wrtst_a1(iPortNumArg, iStatus, fpcaMsgBlock, &iRet);
		    iRet = SERWriteString(iPortNumArg, iStatus, fpcaMsgBlock, &iRet);
                    if (iRet != A_OK)
                    {
                        SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
<<<<<<< HEAD
//printf("SERGetSECSMsg23 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
                        return FAILURE;
                    }
            	}
		else
=======
//printf("SERGetSECSMsg23 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
                        return FAILURE;
                    }
            	}
		else
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                {
                    SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);

//                    SERSetSECSStatus(iPortNumArg, AWAIT_EOT, OK, ENQ, TRUE);
<<<<<<< HEAD
//printf("SERGetSECSMsg24 cur activity=%d --> 11 invalidEOT=%x\n", uiCurrActivity, cCharIn);
		    SERFlushRxBuff(iPortNumArg);
		    SERFlushTxBuff(iPortNumArg);
                    return FAILURE;
                }

            }
	    else
	    {
                if (ulTimeOut < TIRSTime())
		{
=======
//printf("SERGetSECSMsg24 cur activity=%d --> 11 invalidEOT=%x\n", uiCurrActivity, cCharIn);
		    SERFlushRxBuff(iPortNumArg);
		    SERFlushTxBuff(iPortNumArg);
                    return FAILURE;
                }

            }
	    else
	    {
                if (ulTimeOut < TIRSTime())
		{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                    S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
                    S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                    if (uiNumRetries > uiMaxNumRetries)
                    {
                    	SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_eot timeout over max NumRtys=%d\n", uiCurrActivity, uiNumRetries);
=======
printf("SERGetSECSMsg: await_eot timeout over max NumRtys=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		    	SERFlushRxBuff(iPortNumArg);
		    	SERFlushTxBuff(iPortNumArg);
                    	return FAILURE;
                    }
                    else
<<<<<<< HEAD
                    {           /* Retry. */
printf("SERGetSECSMsg: await_eot timeout retry=%d\n",uiNumRetries); 
                    	SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
//printf("SERGetSECSMsg33 cur activity=%d --> 19 retry=%d\n", uiCurrActivity, uiNumRetries);
		    	SERFlushRxBuff(iPortNumArg);
		    	SERFlushTxBuff(iPortNumArg);
                    }
	     	}
		else
		{
=======
                    {           /* Retry. */
printf("SERGetSECSMsg: await_eot timeout retry=%d\n",uiNumRetries); 
                    	SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
//printf("SERGetSECSMsg33 cur activity=%d --> 19 retry=%d\n", uiCurrActivity, uiNumRetries);
		    	SERFlushRxBuff(iPortNumArg);
		    	SERFlushTxBuff(iPortNumArg);
                    }
	     	}
		else
		{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            	    if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
            	    if (cCharIn == EOT)
            	    {
                    	S2SetSecsStructureMember(COM, iPortNumArg, SEND_MESSAGE, WTCURR_ACTIVITY);
<<<<<<< HEAD
printf("SERGetSECSMsg await_eot EOT received, set SEND_MESSAGE\n");
=======
printf("SERGetSECSMsg await_eot EOT received, set SEND_MESSAGE\n");
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    	S2GetTMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
                    	iStatus = (int)(fpcaMsgBlock[0]) + 3;
		    	iRet = SERWriteString(iPortNumArg, iStatus, fpcaMsgBlock, &iRet);
                    	if (iRet != A_OK)
                    	{
                            SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg await_eot write msg error nRtry=%d\n", uiNumRetries);
=======
printf("SERGetSECSMsg await_eot write msg error nRtry=%d\n", uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		    	    SERFlushRxBuff(iPortNumArg);
		    	    SERFlushTxBuff(iPortNumArg);
                            return FAILURE;
                    	}
<<<<<<< HEAD
            	    }
                    else
                    {
			uiEOTretries++;
			if (uiEOTretries > 50)
			{
                    	    S2SetSecsStructureMember(COM, iPortNumArg, SEND_MESSAGE, WTCURR_ACTIVITY);
                            SERSetSECSStatus(iPortNumArg, AWAIT_EOT, OK, ENQ, TRUE);
printf("SERGetSECSMsg: await_eot invalid eot read, send ENQ again\n");
		    	    SERFlushRxBuff(iPortNumArg);
		    	    SERFlushTxBuff(iPortNumArg);
			    uiEOTretries = 0;
//if(iNeed)
//printf("SERGetSECSMsg25 cur activity=%d retry=%d\n", uiCurrActivity, uiEOTretries);
//iNeed=0;
			}
                    }
		}
=======
            	    }
                    else
                    {
			uiEOTretries++;
			if (uiEOTretries > 50)
			{
                    	    S2SetSecsStructureMember(COM, iPortNumArg, SEND_MESSAGE, WTCURR_ACTIVITY);
                            SERSetSECSStatus(iPortNumArg, AWAIT_EOT, OK, ENQ, TRUE);
printf("SERGetSECSMsg: await_eot invalid eot read, send ENQ again\n");
		    	    SERFlushRxBuff(iPortNumArg);
		    	    SERFlushTxBuff(iPortNumArg);
			    uiEOTretries = 0;
//if(iNeed)
//printf("SERGetSECSMsg25 cur activity=%d retry=%d\n", uiCurrActivity, uiEOTretries);
//iNeed=0;
			}
                    }
		}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	    }
	    break;


        case SEND_EOT:       /* Sending EOT, checks if EOT is sent and start receiving data. */
            S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
            if (!ulTimeOut)
            {
//                iRet = oqsiz_a1(iPortNumArg, &iNumCharsLeft);
		iRet = SERBufTXLeft(iPortNumArg, &iNumCharsLeft);
                if (iRet != A_OK )
                {
                    S2SetSecsStructureMember(COM, iPortNumArg, IDLE, WTCURR_ACTIVITY);
                    S2SetSecsStructureMember(COM, iPortNumArg, INTERNAL_WRITE, LAST_ERROR);
<<<<<<< HEAD
//printf("SERGetSECSMsg4 cur activity=%d --> 0\n", uiCurrActivity);
=======
//printf("SERGetSECSMsg4 cur activity=%d --> 0\n", uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    return FAILURE;
                }
                else if (!iNumCharsLeft)
                {           /* EOT sent. */
                    if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
                    if (iRet == IN_Q_EMPTY)
                    {
                        S2GetSecsStructureMember(COM, iPortNumArg, &uiTimeOutInterval, T2);
                        ulTimeOut = (unsigned long)(uiTimeOutInterval) + TIRSTime();
                        S2SetTimeOut(COM, iPortNumArg, ulTimeOut);
                    }
                    else if (iRet == A_OK)
                    {       /* Length byte received. */
                        if ((cCharIn >= 10) && (cCharIn <= 254))
                        {
                            S2GetMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
                            fpcaMsgBlock[0] = (unsigned char)(cCharIn);
                            S2GetSecsStructureMember(COM, iPortNumArg, &uiTimeOutInterval, T1);
                            ulTimeOut = (unsigned long)(cCharIn)*(unsigned long)(uiTimeOutInterval) + TIRSTime();
                            S2SetTimeOut(COM, iPortNumArg, ulTimeOut);
                            S2SetSecsStructureMember(COM, iPortNumArg, REC_MESSAGE, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERGetSECSMsg5 cur activity=%d --> 3\n", uiCurrActivity);
			    TIDelay(100);
                        }
                        else
                        {
                            SERSetSECSStatus(iPortNumArg, IDLE, INV_LEN_BYTE, NAK, TRUE);
//printf("SERGetSECSMsg22 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg5 cur activity=%d --> 3\n", uiCurrActivity);
			    TIDelay(100);
                        }
                        else
                        {
                            SERSetSECSStatus(iPortNumArg, IDLE, INV_LEN_BYTE, NAK, TRUE);
//printf("SERGetSECSMsg22 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                            return FAILURE;
                        }
                    }
                    else
                    {       /* ERROR reading. */
                        SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_READ, NAK, TRUE);
<<<<<<< HEAD
//printf("SERGetSECSMsg21 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg21 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                        return FAILURE;
                    }
                }
            }
            else
            {               /* Time-out has been set for len byte. */
                S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
                if (ulTimeOut < TIRSTime())
                {           /* Time-out. */
                    SERSetSECSStatus(iPortNumArg, IDLE, S_TIMEOUT, NAK, TRUE);
<<<<<<< HEAD
//printf("SERGetSECSMsg20 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg20 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    return FAILURE;
                }
                if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
                if (iRet == A_OK)
                {
                    S2GetMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
                    fpcaMsgBlock[0] = (unsigned char)(cCharIn);
                    S2GetSecsStructureMember(COM, iPortNumArg, &uiTimeOutInterval, T1);
                    ulTimeOut = (unsigned long)(cCharIn)*(unsigned long)(uiTimeOutInterval) + TIRSTime();
                    S2SetTimeOut(COM, iPortNumArg, ulTimeOut);
                    S2SetSecsStructureMember(COM, iPortNumArg, REC_MESSAGE, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERGetSECSMsg7 cur activity=%d --> 3\n", uiCurrActivity);
		    TIDelay(50);
=======
//printf("SERGetSECSMsg7 cur activity=%d --> 3\n", uiCurrActivity);
		    TIDelay(50);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                }
            }
            break;

        case REC_MESSAGE:
//            iRet = qsize_a1(iPortNumArg, &iNumCharsLeft, &iStatus);
            S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
<<<<<<< HEAD
            if (ulTimeOut < TIRSTime())
	    {
                    SERSetSECSStatus(iPortNumArg, IDLE, S_TIMEOUT, NAK, TRUE);
printf("SERGetSECSMsg rec_message timeOut\n");
                    return FAILURE;
	    }

 	    iRet = SERBufRXLeft(iPortNumArg, &iNumCharsLeft, &iStatus);
=======
            if (ulTimeOut < TIRSTime())
	    {
                    SERSetSECSStatus(iPortNumArg, IDLE, S_TIMEOUT, NAK, TRUE);
printf("SERGetSECSMsg rec_message timeOut\n");
                    return FAILURE;
	    }

 	    iRet = SERBufRXLeft(iPortNumArg, &iNumCharsLeft, &iStatus);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)


            if (iRet == A_OK)
            {               /* All data received. */
                S2GetMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
//                if (iNumCharsLeft == ((int)(fpcaMsgBlock[0])+2))
//                {
//                    S2GetMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
//                    iRet = rdst_a1(iPortNumArg, iNumCharsLeft, fpcaMsgBlock+1, &iRet, &iNumCharsLeftAfterRead, &iStatus );
<<<<<<< HEAD
//		    iRet = COReadString(iPortNumArg, iNumCharsLeft, fpcaMsgBlock+1, &iNumCharsLeftAfterRead, &iStatus);

		    TIDelay(50);

		    iRet = COReadString(iPortNumArg, iNumCharsLeft, fpcaMsgBlock, &iNumCharsLeftAfterRead, &iStatus);


		    iLen = (unsigned int)fpcaMsgBlock[0] + 1;

//printf("SERGetSECSMsg53  iNumRead=%d msglen=%d\n", iNumCharsLeftAfterRead, iLen);

		    if (iNumCharsLeftAfterRead == (iLen+1))
		    {
                    	if (S2CheckSumOK(fpcaMsgBlock))
                    	{
			    giChkSum = 0;
//printf("SERGetSECSMsg8 REC_MESSAGE chksumOK iNumCharsLeft=%d iStatus=%d msglen=%d\n", iNumCharsLeftAfterRead, iStatus, iLen);
                            SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg19 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
//for(i=0; i<=iNumCharsLeftAfterRead; ++i)
//printf(" %d: %x\n",i, fpcaMsgBlock[i]);
                            S2InterpretSecsMsg(iPortNumArg);
                        }
                        else
                        {
			    if(++giChkSum > 2)
			    {
//printf("SERGetSECSMsg88 REC_MESSAGE chksumNK iNumCharsLeft=%d iStatus=%d msglen=%d\n", iNumCharsLeftAfterRead, iStatus, iLen);
                                SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg99 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
//for(i=0; i<=iNumCharsLeftAfterRead; ++i)
//printf(" %d: %x\n",i, fpcaMsgBlock[i]);
                                S2InterpretSecsMsg(iPortNumArg);
				giChkSum = 0;
			    }
			    else
			    {
                            	SERSetSECSStatus(iPortNumArg, IDLE, CHECK_SUM_ERR, NAK, TRUE);
//printf("SERGetSECSMsg18 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
				TIDelay(100);
                            	return FAILURE;
			    }
                    	}
		    }
		    else	// must read the rest message in
		    {	
			TIDelay(50);
	            	if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
         	   	if (iRet == A_OK)
         	   	{
			    iRet = COReadString(iPortNumArg, iNumCharsLeft, caMsgRest, &iNumRead, &iStatus);
			    iTotRead = iNumCharsLeftAfterRead + iNumRead + 1;

//for(i=0;i<=iNumCharsLeftAfterRead;++i)
//printf("SERGetSECSMsg55 frst %d = %x\n",i,fpcaMsgBlock[i]);
//printf("SERGetSECSMsg55 frst %d = %x\n",i,cCharIn);

			    if (iTotRead == (iLen+1))
			    {
//				fpcaMsgBlock[iNumCharsLeftAfterRead+1] = cCharIn;
				for(i=0; i<=iNumRead; ++i)
				{
				    fpcaMsgBlock[iNumCharsLeftAfterRead+i+1] = caMsgRest[i]; 
//printf("SERGetSECSMsg56 rest %d = %x\n",i+iNumCharsLeftAfterRead+1,caMsgRest[i]);
				}
				if (S2CheckSumOK(fpcaMsgBlock))
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg56 chksum OK\n");
				}
				else
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg56 chksum Not OK\n");
				}
                                S2InterpretSecsMsg(iPortNumArg);
				giChkSum = 0;


			    }
			    else
			    {
//				fpcaMsgBlock[iNumCharsLeftAfterRead+1] = cCharIn;
				for(i=0; i<=iNumRead; ++i)
				{
				    fpcaMsgBlock[iNumCharsLeftAfterRead+i+1] = caMsgRest[i]; 
//printf("SERGetSECSMsg57 rest %d = %x\n",i+iNumCharsLeftAfterRead+1,caMsgRest[i]);
				}
				if (S2CheckSumOK(fpcaMsgBlock))
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg57 chksum OK\n");
				}
				else
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg57 chksum Not OK\n");
				}
                                S2InterpretSecsMsg(iPortNumArg);
				giChkSum = 0;
			    }

			}
		    }
=======
//		    iRet = COReadString(iPortNumArg, iNumCharsLeft, fpcaMsgBlock+1, &iNumCharsLeftAfterRead, &iStatus);

		    TIDelay(50);

		    iRet = COReadString(iPortNumArg, iNumCharsLeft, fpcaMsgBlock, &iNumCharsLeftAfterRead, &iStatus);


		    iLen = (unsigned int)fpcaMsgBlock[0] + 1;

//printf("SERGetSECSMsg53  iNumRead=%d msglen=%d\n", iNumCharsLeftAfterRead, iLen);

		    if (iNumCharsLeftAfterRead == (iLen+1))
		    {
                    	if (S2CheckSumOK(fpcaMsgBlock))
                    	{
			    giChkSum = 0;
//printf("SERGetSECSMsg8 REC_MESSAGE chksumOK iNumCharsLeft=%d iStatus=%d msglen=%d\n", iNumCharsLeftAfterRead, iStatus, iLen);
                            SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg19 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
//for(i=0; i<=iNumCharsLeftAfterRead; ++i)
//printf(" %d: %x\n",i, fpcaMsgBlock[i]);
                            S2InterpretSecsMsg(iPortNumArg);
                        }
                        else
                        {
			    if(++giChkSum > 2)
			    {
//printf("SERGetSECSMsg88 REC_MESSAGE chksumNK iNumCharsLeft=%d iStatus=%d msglen=%d\n", iNumCharsLeftAfterRead, iStatus, iLen);
                                SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg99 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
//for(i=0; i<=iNumCharsLeftAfterRead; ++i)
//printf(" %d: %x\n",i, fpcaMsgBlock[i]);
                                S2InterpretSecsMsg(iPortNumArg);
				giChkSum = 0;
			    }
			    else
			    {
                            	SERSetSECSStatus(iPortNumArg, IDLE, CHECK_SUM_ERR, NAK, TRUE);
//printf("SERGetSECSMsg18 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
				TIDelay(100);
                            	return FAILURE;
			    }
                    	}
		    }
		    else	// must read the rest message in
		    {	
			TIDelay(50);
	            	if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
         	   	if (iRet == A_OK)
         	   	{
			    iRet = COReadString(iPortNumArg, iNumCharsLeft, caMsgRest, &iNumRead, &iStatus);
			    iTotRead = iNumCharsLeftAfterRead + iNumRead + 1;

//for(i=0;i<=iNumCharsLeftAfterRead;++i)
//printf("SERGetSECSMsg55 frst %d = %x\n",i,fpcaMsgBlock[i]);
//printf("SERGetSECSMsg55 frst %d = %x\n",i,cCharIn);

			    if (iTotRead == (iLen+1))
			    {
//				fpcaMsgBlock[iNumCharsLeftAfterRead+1] = cCharIn;
				for(i=0; i<=iNumRead; ++i)
				{
				    fpcaMsgBlock[iNumCharsLeftAfterRead+i+1] = caMsgRest[i]; 
//printf("SERGetSECSMsg56 rest %d = %x\n",i+iNumCharsLeftAfterRead+1,caMsgRest[i]);
				}
				if (S2CheckSumOK(fpcaMsgBlock))
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg56 chksum OK\n");
				}
				else
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg56 chksum Not OK\n");
				}
                                S2InterpretSecsMsg(iPortNumArg);
				giChkSum = 0;


			    }
			    else
			    {
//				fpcaMsgBlock[iNumCharsLeftAfterRead+1] = cCharIn;
				for(i=0; i<=iNumRead; ++i)
				{
				    fpcaMsgBlock[iNumCharsLeftAfterRead+i+1] = caMsgRest[i]; 
//printf("SERGetSECSMsg57 rest %d = %x\n",i+iNumCharsLeftAfterRead+1,caMsgRest[i]);
				}
				if (S2CheckSumOK(fpcaMsgBlock))
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg57 chksum OK\n");
				}
				else
                    		{
                                    SERSetSECSStatus(iPortNumArg, IDLE, OK, ACK, TRUE);
//printf("SERGetSECSMsg57 chksum Not OK\n");
				}
                                S2InterpretSecsMsg(iPortNumArg);
				giChkSum = 0;
			    }

			}
		    }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            }
            else
            {
                SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_READ, NAK, TRUE);
<<<<<<< HEAD
//printf("SERGetSECSMsg15 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg15 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                return FAILURE;
            }
            break;

        case AWAIT_ACK_NAK:
            S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
            if (!ulTimeOut)
<<<<<<< HEAD
            {
=======
            {
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
printf("SERGetSECSMsg: await_ack_nak, no TimeOut set\n");
            	if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
            	if (iRet == A_OK)
            	{
                    if (cCharIn == ACK)
                    {
                    	S2SetSecsStructureMember(COM, iPortNumArg, IDLE, WTCURR_ACTIVITY);
                    	S2SetSecsStructureMember(COM, iPortNumArg, OK, LAST_ERROR);
                    	S2SetSecsStructureMember(COM, iPortNumArg, TRUE, MESSAGE_SENT);
                    	S2SetSecsStructureMember(COM, iPortNumArg, 0, NUM_OF_RETRIES);
<<<<<<< HEAD
//printf("SERGetSECSMsg59 cur activity=%d --> 0\n", uiCurrActivity);
=======
//printf("SERGetSECSMsg59 cur activity=%d --> 0\n", uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    }
                    else
                    {
                    	S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                    	S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
<<<<<<< HEAD
//printf("SERGetSECSMsg69 cur activity=%d --> 0 retry=%d charIn=%x\n", uiCurrActivity, uiNumRetries, cCharIn);
=======
//printf("SERGetSECSMsg69 cur activity=%d --> 0 retry=%d charIn=%x\n", uiCurrActivity, uiNumRetries, cCharIn);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    	S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                    	if (uiNumRetries > uiMaxNumRetries)
                      	{
                            SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
<<<<<<< HEAD
//printf("SERGetSECSMsg10 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg10 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    SERFlushRxBuff(iPortNumArg);
			    SERFlushTxBuff(iPortNumArg);
                            return FAILURE;
                    	}
                    	else
                    	{           /* Retry. */
//                            SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
                            SERSetSECSStatus(iPortNumArg, REC_MESSAGE, OK, NAK, FALSE);
<<<<<<< HEAD
//printf("SERGetSECSMsg11 cur activity=%d --> 19 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg11 cur activity=%d --> 19 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    SERFlushRxBuff(iPortNumArg);
			    SERFlushTxBuff(iPortNumArg);
                    	}
                    }
                    S2SetTimeOut(COM, iPortNumArg, 0L);
            	}
            	else if (iRet == IN_Q_EMPTY)
            	{                   /* Nothing received yet. */
                    S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
                    if (ulTimeOut < TIRSTime())
                    {
                    	S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                    	S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
                    	S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                    	if (uiNumRetries > uiMaxNumRetries)
                    	{
                            SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
<<<<<<< HEAD
//printf("SERGetSECSMsg12 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg12 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                            return FAILURE;
                    	}
                    	else
                    	{           /* Retry. */
                            SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
<<<<<<< HEAD
//printf("SERGetSECSMsg13 cur activity=%d --> 19 retry=%d\n", uiCurrActivity, uiNumRetries);
=======
//printf("SERGetSECSMsg13 cur activity=%d --> 19 retry=%d\n", uiCurrActivity, uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    SERFlushRxBuff(iPortNumArg);
			    SERFlushTxBuff(iPortNumArg);
                    	}
                    }
             	}
            	else
            	{                   /* Problem. */
                    SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
<<<<<<< HEAD
//printf("SERGetSECSMsg14 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
                    return FAILURE;
            	}
            }
    	    else
	    {
                if (ulTimeOut < TIRSTime())
		{
=======
//printf("SERGetSECSMsg14 cur activity=%d --> 0 retry=%d\n", uiCurrActivity, uiNumRetries);
                    return FAILURE;
            	}
            }
    	    else
	    {
                if (ulTimeOut < TIRSTime())
		{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                    S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
                    S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                    if (uiNumRetries > uiMaxNumRetries)
                    {
                    	SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak, timeout retry over max, nRty=%d\n", uiNumRetries);
=======
printf("SERGetSECSMsg: await_ack_nak, timeout retry over max, nRty=%d\n", uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
		    	SERFlushRxBuff(iPortNumArg);
		    	SERFlushTxBuff(iPortNumArg);
                    	return FAILURE;
                    }
                    else
                    {           /* Retry. */
                    	SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak, timeout retrying MSG_TO_SEND, nRtry=%d\n", uiNumRetries);
		    	SERFlushRxBuff(iPortNumArg);
		    	SERFlushTxBuff(iPortNumArg);
                    }
	    	}
		else
		{
=======
printf("SERGetSECSMsg: await_ack_nak, timeout retrying MSG_TO_SEND, nRtry=%d\n", uiNumRetries);
		    	SERFlushRxBuff(iPortNumArg);
		    	SERFlushTxBuff(iPortNumArg);
                    }
	    	}
		else
		{
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            	    if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
            	    if (iRet == A_OK)
            	    {
                    	if (cCharIn == ACK)
                    	{
                    	    S2SetSecsStructureMember(COM, iPortNumArg, IDLE, WTCURR_ACTIVITY);
                    	    S2SetSecsStructureMember(COM, iPortNumArg, OK, LAST_ERROR);
                    	    S2SetSecsStructureMember(COM, iPortNumArg, TRUE, MESSAGE_SENT);
                    	    S2SetSecsStructureMember(COM, iPortNumArg, 0, NUM_OF_RETRIES);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak, ACK received\n");
=======
printf("SERGetSECSMsg: await_ack_nak, ACK received\n");
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    	}
                    	else
                    	{
                    	    S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                    	    S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
<<<<<<< HEAD
//printf("SERGetSECSMsg99 cur activity=%d --> 0 retry=%d charIn=%x\n", uiCurrActivity, uiNumRetries, cCharIn);
=======
//printf("SERGetSECSMsg99 cur activity=%d --> 0 retry=%d charIn=%x\n", uiCurrActivity, uiNumRetries, cCharIn);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    	    S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                    	    if (uiNumRetries > uiMaxNumRetries)
                      	    {
                            	SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak no ACK received, retry over max, nRtry=%d\n", uiNumRetries);
=======
printf("SERGetSECSMsg: await_ack_nak no ACK received, retry over max, nRtry=%d\n", uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    	SERFlushRxBuff(iPortNumArg);
			    	SERFlushTxBuff(iPortNumArg);
                            	return FAILURE;
                    	    }
                    	    else
                    	    {           /* Retry. */
                            	SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
//                            	SERSetSECSStatus(iPortNumArg, AWAIT_EOT, OK, ENQ, TRUE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak no ACK received, retry MSG_TO_SEND, nRty=%d\n", uiNumRetries);
=======
printf("SERGetSECSMsg: await_ack_nak no ACK received, retry MSG_TO_SEND, nRty=%d\n", uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    	SERFlushRxBuff(iPortNumArg);
			    	SERFlushTxBuff(iPortNumArg);
                    	    }
                    	}
                     	S2SetTimeOut(COM, iPortNumArg, 0L);
            	    }
            	    else if (iRet == IN_Q_EMPTY)
            	    {                   /* Nothing received yet. */
                    	S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
                    	if (ulTimeOut < TIRSTime())
                    	{
                    	    S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                    	    S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
                    	    S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                    	    if (uiNumRetries > uiMaxNumRetries)
                    	    {
                            	SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak Q_Empty, timeOut over max, nRty=%d\n", uiNumRetries);
=======
printf("SERGetSECSMsg: await_ack_nak Q_Empty, timeOut over max, nRty=%d\n", uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                            	return FAILURE;
                    	    }
                    	    else
                    	    {           /* Retry. */
                            	SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak Q_Empty, timeOut, retrying MSG_TO_SEND, nRtry=%d\n", uiNumRetries);
=======
printf("SERGetSECSMsg: await_ack_nak Q_Empty, timeOut, retrying MSG_TO_SEND, nRtry=%d\n", uiNumRetries);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
			    	SERFlushRxBuff(iPortNumArg);
			    	SERFlushTxBuff(iPortNumArg);
                    	    }
                    	}
             	    }
            	    else
            	    {                   /* Problem. */
                    	SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
<<<<<<< HEAD
printf("SERGetSECSMsg: await_ack_nak, RX_buffer error\n");
                    	return FAILURE;
            	    }
		}
	    }

            break;

=======
printf("SERGetSECSMsg: await_ack_nak, RX_buffer error\n");
                    	return FAILURE;
            	    }
		}
	    }

            break;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        default:
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERSendSECSMsg
 *
 * Parameters:
 *      iPortNumArg     Com port number
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSendSECSMsg(int iPortNumArg)
{
    int iNumCharsLeft, iStatus, iRet;
    unsigned int uiCurrActivity, uiTimeOutInterval, uiLastErr, uiNumRetries, uiMaxNumRetries;
    unsigned long ulTimeOut;
    char cCharIn;
    unsigned char *fpcaMsgBlock;

<<<<<<< HEAD
    if (S2GetSecsStructureMember(COM, iPortNumArg, &uiCurrActivity, WTCURR_ACTIVITY) == FAILURE) return FAILURE;

if(uiCurrActivity != uiPrevAct)
{
//printf("SERSendSECSMsg port=%d activity=%d prev=%d\n",iPortNumArg, uiCurrActivity, uiPrevAct);
uiPrevAct = uiCurrActivity;
}
else
goto exit_out;

    switch (uiCurrActivity)
    {
        case MSG_TO_SEND:     /* There is message to send. */

giSECSComplete = 0;
=======
    if (S2GetSecsStructureMember(COM, iPortNumArg, &uiCurrActivity, WTCURR_ACTIVITY) == FAILURE) return FAILURE;

if(uiCurrActivity != uiPrevAct)
{
//printf("SERSendSECSMsg port=%d activity=%d prev=%d\n",iPortNumArg, uiCurrActivity, uiPrevAct);
uiPrevAct = uiCurrActivity;
}
else
goto exit_out;

    switch (uiCurrActivity)
    {
        case MSG_TO_SEND:     /* There is message to send. */

giSECSComplete = 0;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

            S2SetSecsStructureMember(COM, iPortNumArg, OK, LAST_ERROR);
	    iRet = SERWriteChar(iPortNumArg, ENQ);
            if (iRet != A_OK)
            {
                S2SetSecsStructureMember(COM, iPortNumArg, IDLE, WTCURR_ACTIVITY);
                S2SetSecsStructureMember(COM, iPortNumArg, INTERNAL_WRITE, LAST_ERROR);
                return FAILURE;
            }
            S2SetSecsStructureMember(COM, iPortNumArg, AWAIT_EOT, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERSendSECSMsg cur activity=%d --> 11\n", uiCurrActivity);
=======
//printf("SERSendSECSMsg cur activity=%d --> 11\n", uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
            S2GetSecsStructureMember(COM, iPortNumArg, &uiTimeOutInterval, T2);
            ulTimeOut = (unsigned long)(uiTimeOutInterval) + TIRSTime();
            S2SetTimeOut(COM, iPortNumArg, ulTimeOut);
            break;

        case SEND_ENQ:       /* Sending ENQ. */
<<<<<<< HEAD
//printf("SERSendSECSMsg SEND_ENQ port=%d activity=%d\n",iPortNumArg, uiCurrActivity);
=======
//printf("SERSendSECSMsg SEND_ENQ port=%d activity=%d\n",iPortNumArg, uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	    iRet = SERBufRXLeft(iPortNumArg, &iNumCharsLeft, &iStatus);
            if (iRet == A_OK)
            {
                if (iNumCharsLeft == 1)
                {           /* This may be ENQ. */
                    if ((iRet=SERGetRawChar(iPortNumArg, &cCharIn, &iNumCharsLeft)) == FAILURE) return FAILURE;
                    if (cCharIn == EOT)
                    {
                        S2SetSecsStructureMember(COM, iPortNumArg, SEND_MESSAGE, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERSendSECSMsg cur activity=%d --> 14\n", uiCurrActivity);
=======
//printf("SERSendSECSMsg cur activity=%d --> 14\n", uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                        S2GetTMsgBlock(COM, iPortNumArg, &fpcaMsgBlock);
                        iStatus = (int)(fpcaMsgBlock[0]) + 3;
			iRet = SERWriteString(iPortNumArg, iStatus, fpcaMsgBlock, &iRet);
                        if (iRet != A_OK)
                        {
                            SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
                            return FAILURE;
                        }
                    }
                    if (cCharIn == ENQ)
                    {       /* Device contention. */
			iRet = SERWriteChar(iPortNumArg, EOT);
                        /* Sending EOT, ready to receive data. */
                        S2SetSecsStructureMember(COM, iPortNumArg, SEND_EOT, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERSendSECSMsg cur activity=%d --> 2\n", uiCurrActivity);
=======
//printf("SERSendSECSMsg cur activity=%d --> 2\n", uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                        S2SetSecsStructureMember(COM, iPortNumArg, OK, LAST_ERROR);
                        do
                        {
                            if (SERGetSECSMsg(iPortNumArg) == FAILURE) return FAILURE;
                            S2GetSecsStructureMember(COM, iPortNumArg, &uiLastErr, LAST_ERROR);
                            S2GetSecsStructureMember(COM, iPortNumArg, &uiCurrActivity, WTCURR_ACTIVITY);
                        } while((uiCurrActivity != IDLE) && (uiLastErr == OK));
                        S2SetSecsStructureMember(COM, iPortNumArg, MSG_TO_SEND, WTCURR_ACTIVITY);
<<<<<<< HEAD
//printf("SERSendSECSMsg cur activity=%d --> 19\n", uiCurrActivity);
=======
//printf("SERSendSECSMsg cur activity=%d --> 19\n", uiCurrActivity);
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                        break;
                    }
                }
                else if (!iNumCharsLeft)
                {           /* No data yet. */
                    S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
                    if (ulTimeOut < TIRSTime())
                    {       /* Time-out. */
                        S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
                        S2SetSecsStructureMember(COM, iPortNumArg, ++uiNumRetries, NUM_OF_RETRIES);
                        S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
                        if (uiNumRetries > uiMaxNumRetries)
                        {
                            SERSetSECSStatus(iPortNumArg, IDLE, RETRY_ERROR, NAK, FALSE);
                            return FAILURE;
                        }
                        else
                        {   /* Retry. */
                            SERSetSECSStatus(iPortNumArg, MSG_TO_SEND, OK, NAK, FALSE);
			    SERFlushTxBuff(iPortNumArg);
			    SERFlushRxBuff(iPortNumArg);
                        }
                    }
                }
                else
                {           /* Too many bytes. */
                    SERSetSECSStatus(iPortNumArg, IDLE, TOO_MANY_BYTES, NAK, FALSE);
		    SERFlushRxBuff(iPortNumArg);
                    return FAILURE;
                }
            }
            else
            {
                SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
                return FAILURE;
            }
            break;

        case SEND_MESSAGE:   /* Sending msg. */
	    iRet = SERBufTXLeft(iPortNumArg, &iNumCharsLeft);
            if (iRet == A_OK)
            {
                if (!iNumCharsLeft)
                {           /* All sent. */
                    S2SetSecsStructureMember(COM, iPortNumArg, AWAIT_ACK_NAK, WTCURR_ACTIVITY);
<<<<<<< HEAD
printf("SERSendSECSMsg: send_message, no char left, set to await_ack_nak\n");
=======
printf("SERSendSECSMsg: send_message, no char left, set to await_ack_nak\n");
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    S2GetSecsStructureMember(COM, iPortNumArg, &uiTimeOutInterval, T2);
                    ulTimeOut = (unsigned long)(uiTimeOutInterval) + TIRSTime();
                    S2SetTimeOut(COM, iPortNumArg, ulTimeOut);
                }
            }
            else
            {               /* Problem. */
                SERSetSECSStatus(iPortNumArg, IDLE, INTERNAL_WRITE, NAK, FALSE);
                return FAILURE;
            }
            break;
        default:
            return FAILURE;
    }
<<<<<<< HEAD

exit_out:
=======

exit_out:
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    SERSetSECSStatus
 *
 * Parameters:
 *      iPortNumArg             (in) Com port number
 *      iCurrActivityStatArg    (in) The new value for the current activity
 *      iLastErrorStatArg       (in) The new value for the last error
 *      cAckNakStatArg          (in) Whether an ACK or NAK is to be sent out
 *      iGetStatArg             (in) TRUE if a GetSECSMsg being executed
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int SERSetSECSStatus(int iPortNumArg, int iCurrActivityStatArg, int iLastErrorStatArg, char cAckNakStatArg, int iGetStatArg)
{
    S2SetSecsStructureMember(COM, iPortNumArg, iCurrActivityStatArg, WTCURR_ACTIVITY);
    S2SetSecsStructureMember(COM, iPortNumArg, iLastErrorStatArg, LAST_ERROR);
    S2SetTimeOut(COM, iPortNumArg, 0L);
//printf("SECSStatus activity=%d prev=%d lastErr=%d\n", iCurrActivityStatArg, uiPrevAct, iLastErrorStatArg);
    if (iGetStatArg)
    {
//        iflsh_a1(iPortNumArg);
//        wrtch_a1(iPortNumArg, cAckNakStatArg);
	SERFlushRxBuff(iPortNumArg);
<<<<<<< HEAD
	SERWriteChar(iPortNumArg, cAckNakStatArg);
    }

    if(iCurrActivityStatArg == IDLE)
    {
	giSECSComplete = 1;
=======
	SERWriteChar(iPortNumArg, cAckNakStatArg);
    }

    if(iCurrActivityStatArg == IDLE)
    {
	giSECSComplete = 1;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    }

    return SUCCESS;
}
<<<<<<< HEAD
int SERDumpSECSStatus(int iPortNumArg)
{
    unsigned int ulTimeOut, uiTimeOutInertval;
    int iLastError, iCurrActivity, uiMaxNumRetries, uiNumRetries;

=======
int SERDumpSECSStatus(int iPortNumArg)
{
    unsigned int ulTimeOut, uiTimeOutInertval;
    int iLastError, iCurrActivity, uiMaxNumRetries, uiNumRetries;

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    S2GetTimeOut(COM, iPortNumArg, &ulTimeOut);
    S2GetSecsStructureMember(COM, iPortNumArg, &uiTimeOutInertval, T2);
    S2GetSecsStructureMember(COM, iPortNumArg, &iLastError, LAST_ERROR);
    S2GetSecsStructureMember(COM, iPortNumArg, &iCurrActivity, WTCURR_ACTIVITY);
    S2GetSecsStructureMember(COM, iPortNumArg, &uiMaxNumRetries, MAX_RETRIES);
    S2GetSecsStructureMember(COM, iPortNumArg, &uiNumRetries, NUM_OF_RETRIES);
<<<<<<< HEAD
    printf("curAct=%d prevAct=%d \r\n timeout=%d timeInterv=%d \r\n maxRetry=%d retried=%d err=%d\n",iCurrActivity,uiPrevAct,ulTimeOut, uiTimeOutInertval, uiMaxNumRetries, uiNumRetries, iLastError);
    
    return SUCCESS;
}
int SERGetSecsActivity(int iPortNumArg)
{
    unsigned int uAct;
    S2GetSecsStructureMember(COM, iPortNumArg, &uAct, WTCURR_ACTIVITY);
    return uAct;
}
=======
    printf("curAct=%d prevAct=%d \r\n timeout=%d timeInterv=%d \r\n maxRetry=%d retried=%d err=%d\n",iCurrActivity,uiPrevAct,ulTimeOut, uiTimeOutInertval, uiMaxNumRetries, uiNumRetries, iLastError);
    
    return SUCCESS;
}
int SERGetSecsActivity(int iPortNumArg)
{
    unsigned int uAct;
    S2GetSecsStructureMember(COM, iPortNumArg, &uAct, WTCURR_ACTIVITY);
    return uAct;
}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

/****************************************************************
 *
 * Function:    SERPutsRxBuff
 *
 * Parameters:
 *      iPortNumArg             (in) Com port number
 *      cpRxStrArg              (in) String to put in the receive buffer
 *
 * Returns: SUCCESS or FAILURE
 *
 ****************************************************************/
int SERPutsRxBuff(int iPortNumArg, char *cpRxStrArg)
{
    char cCharOut, *cpRxStrTemp;
    struct CommStr *pCommData;

    if (SERCheckValidPort(iPortNumArg) == FAILURE)
        return FAILURE;

    pCommData = stCommData + iPortNumArg - 1;
    if (!pCommData->m_iPortOpen)
        return FAILURE;

    cpRxStrTemp = cpRxStrArg;
    while (*cpRxStrTemp != 0)
    {
//        if (TIRefreshWD() == FAILURE)
//            return FAILURE;
        cCharOut = *cpRxStrTemp++;
        if (SERStoreRxCh(pCommData, cCharOut) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}
//#endif
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
