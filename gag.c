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
 * Program:     GALIL Motion Control Interface Header
 * File:        gag.c
 * Functions:   GAGetUseGalilFlag
 *              GASetUseGalilFlag
 *              GASetExcIntOutp
 *              GAGetGalilErrorFlag
 *              GASetGalilErrorFlag
 *              GASetDebugBit
 *              GAGetDebugBit
 *              GAWriteGalilInputOutputPort
 *              GAReadGalilInputOutputPort
 *              GASendReceiveGalil
 *              GAAmpInit
 *              GARouteStringToQueueBuffer
 *              GAFlushOutCmdStringQueueToGalil
 *              GAInitCmdString
 *              GAStartCommandsQueuing
 *              GACancelCommandsQueuing
 *              GASendSSCommand
 *              GASendAbortMotionCommand
 *              GASetTimeCommand
 *              GASendConfigureSystemCommand
 *              GASendAxesCommand
 *              GASetCommandString
 *              GASetValsLongDefined
 *              GASetValsDoubleDefined
 *              GASetValsLong
 *              GASetValsDouble
 *              GAGetValsLongDefined
 *              GAGetValsDoubleDefined
 *              GAGetValsLong
 *              GAGetValsDouble
 *
 * Description: Provide interface routines to the Motion Control
 *      module for functions used glbally throught the code.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/
#include <sys/io.h>
#include <string.h>
#include "sck.h"
#include "gafn.h"
#include "gag.h"
#include "scintr.h"
#include "scstat.h"
#include "sctim.h"
#include "gaintr.h"
#include "mapio.h"
#include "scio.h"
#include "dmclnx.h"

void GAGalilWriteIO(int, int);
int  GAGalilReadIO(int);
int  GAGalilReadOut(int);

extern HANDLEDMC ghDMC;
extern CONTROLLERINFO gControllerInfo;

char gGalilReadBuffer [MAXGASTR];
int giNumGalilRead = 0;
int giGalilReadPointer = 0;
char cpNull[8];
int giTOcount=0;
// 
// DMCCommand Interface
//	Check for Timeout
//	If time-out, reestablish connection.
//
int GASendDMCCommand(HANDLEDMC ghDMC, char* caCommand, char* caResp, int iMaxStr)
{
    int iTC=0;
    long rc=0;
    char caLocal[80];

    rc = DMCCommand(ghDMC, caCommand, caResp, iMaxStr);
    if (rc == -1)
    {
	++giTOcount;

	rc = DMCCommand(ghDMC, "TC", caLocal, 80);
	iTC = atoi(caLocal);

	//printf("Galil timeout cnt=%d at=%s TC=%d\n",giTOcount, caCommand, iTC);

	if (rc || iTC == 123) // TCP lost or Time-out
	{
	    rc = DMCClose(ghDMC);
	    rc = DMCOpen( &gControllerInfo, &ghDMC );
	    if (rc)
	    {
	    	//printf("DMCOpen Restablish Error: %d\n", rc);
            	return FAILURE;  
    	    }
	    // Just re-try
	    //printf("DMCOpen Restablished!: %d\n", rc);
	    rc = DMCCommand(ghDMC, caCommand, caResp, iMaxStr);
    	}
	else // Just re-try
	{
	    rc = DMCCommand(ghDMC, caCommand, caResp, iMaxStr);
	}
    }
    return rc; // rc=0 if Success, others if error
}

void GAGalilWriteIO(int nIONum, int iBit)
{
    int iResult;
    char cpBuf[16];
	char cpResp[16];

	if (iBit)
		sprintf(cpBuf, "SB%d\xD", nIONum);
	else
		sprintf(cpBuf, "CB%d\xD", nIONum);

	iResult = GASendReceiveGalil( GA_CARD_1, cpBuf, cpResp);
}

int  GAGalilReadIO(int nIONum)
{
    int iResult;
    char cpBuf[16];
	char cpResp[16];

	sprintf(cpBuf, "MG @IN[%d]\xD", nIONum);
    iResult = GASendReceiveGalil( GA_CARD_1, cpBuf, cpResp );

    iResult = atoi(cpResp);
	return iResult;
}

int  GAGalilReadOut(int nIONum)
{
    int iResult;
    char cpBuf[16];
	char cpResp[16];

	sprintf(cpBuf, "MG @OUT[%d]\xD", nIONum);
    iResult = GASendReceiveGalil( GA_CARD_1, cpBuf, cpResp );

    iResult = atoi(cpResp);
	return iResult;
}

/****************************************************************\
 *
 * Function:    MCUseGalilFlag
 *
 * Abstract:    Get the Use Galil Flag flag
 *
 * Parameters:  N/A
 *
 * Returns:     iUseGalilFlag       (out) UseGalilFlag
 *
\*****************************************************************/
int GAGetUseGalilFlag(void)
{
    int iUseGalilFlag;

    iUseGalilFlag = GAiUseGalil;

    return iUseGalilFlag;
}


/****************************************************************\
 *
 * Function:    GAGetGLNumAxis
 *
 * Abstract:    Get GL_NUM_AXIS
 *
 * Parameters:  N/A
 *
 * Returns:     GL_NUM_AXIS       (out) UseGalilFlag
 *
\*****************************************************************/
unsigned GAGetGLNumAxis(int iCardArg)
{
    if (iCardArg == GA_CARD_0 || iCardArg == GA_CARD_1)
        return GL_NUM_AXIS[iCardArg];
    else
        return 0;
}


/****************************************************************\
 *
 * Function:    GASetUseGalilFlag
 *
 * Abstract:    Set the Use Galil Flag flag
 *
 * Parameters:  iUseGalilFlag       (in) Use Galil Flag
 *
 * Returns:    N/A
 *
\*****************************************************************/
void GASetUseGalilFlag(int iUseGalilFlag)
{
    GAiUseGalil = iUseGalilFlag;
}


/****************************************************************\
 *
 * Function:    GASetExcIntOutp
 *
 * Abstract:    Set the excessive interrupt output flag
 *
 * Parameters:  iExcIntOutp     (in) iExcIntOutp
 *
 * Returns:    N/A
 *
\*****************************************************************/
void GASetExcIntOutp(int iExcIntOutp)
{
    GAiExcIntOutp = iExcIntOutp;
}


/****************************************************************\
 *
 * Function:    GAGetGalilErrorFlag
 *
 * Abstract:    Get the Galil Error flag
 *
 * Parameters:  N/A
 *
 * Returns:     Galil Error flag
 *
\*****************************************************************/
unsigned GAGetGalilErrorFlag(void)
{
    unsigned uGalilError;

    uGalilError = MCuGalilError;

    return uGalilError;
}


/****************************************************************\
 *
 * Function:    GASetGalilErrorFlag
 *
 * Abstract:    Set the Galil Error flag
 *
 * Parameters:  uGalilErrorFlag     (in) Galil Error
 *
 * Returns:    Galil Error flag
 *
\*****************************************************************/
void GASetGalilErrorFlag(unsigned uGalilErrorFlag)
{
    MCuGalilError = uGalilErrorFlag;
}

/****************************************************************\
 *
 * Function:    GAWriteGalilInputOutputPort
 *
 * Abstract:    Write bit to the Motion Control card I/O
 *              This function is to read a Galil card I/O address.
 *              Address of Robot input for Galil card 0 is 0x300 (IPORT F)
 *              Address of Robot input for Galil card 1 is 0x310 (IPORT K)
 *
 * Parameters:  iCardNoArg         (in) Card No
 *              iBitNumber         (in) -1  = write entire byte
 *                                 (in) 0-7 = write only specified bit
 *              iData              (in)  Data to write
 *                                 (TRUE to set bit, FALSE to clear)
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAWriteGalilInputOutputPort(int iCardNoArg, int iBitNumber, int iData)
{
    int iBitMask, iSetMask, iCardNum, iDataVal;

    // Validate the card no for this configuration
    // CAN'T DO THIS! For pre only and 1-axis pre system we need access
    // to second card bits even though the card doesn't exist in the system
//    if ( GAValidateCardNo(iCardNoArg) == FAILURE)
//        return FAILURE;
    if (iCardNoArg == GA_CARD_0)
        iCardNum = IO_ROBOT_OUTPUT_A;
    else if (iCardNoArg == GA_CARD_1)
        iCardNum = IO_PRE_OUTPUT_L;
    else
        return FAILURE;
    // Work out which bits to set &/or reset
    if (iBitNumber != -1)
    {
        iBitMask = 1 << iBitNumber;
        iSetMask = (iData ? iBitMask : 0);
        iDataVal = (inb(iCardNum) & ~iBitMask) | iSetMask;
    }
    else
        iDataVal = iData;
    //  Write the I/O data at address iPortAddress (0x300 or 0x310)
    IOWriteIO( -1, iDataVal, iCardNum );
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GAReadGalilInputOutputPort
 *
 * Abstract:   Read the Motion Control card I/O
 *             This function is to read a Galil card I/O address.
 *             Address of Robot input for Galil card 0 is 0x300 (IPORT F)
 *             Address of Robot input for Galil card 1 is 0x310 (IPORT K)
 *
 * Parameters:  int    iCardNoArg               (in) Card No
 *              int    *piReadData           (out) Data Read
 *
 * Returns:     SUCCESS or FAILURE
 *
\****************************************************************/
int GAReadGalilInputOutputPort(int iCardNoArg, int *piReadData)
{
    int iCardNum;

    // Validate the card no for this configuration
    // CAN'T DO THIS! For pre only and 1-axis pre system we need access
    // to second card bits even though the card doesn't exist in the system
    if ( iCardNoArg == GA_CARD_0 )
        iCardNum = IO_ROBOT_INPUT_F;
    else if ( iCardNoArg == GA_CARD_1 )
        iCardNum = IO_PRE_INPUT_K;
    else
        return FAILURE;

    *piReadData = inb( iCardNum );
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GASendReceiveGalil
 *
 * Abstract:    Put string to Galil card and receive response from Galil.
 *
 *  Output a command string to the Galil and wait for reply(s).
 *  This routine waits for the Galil's write buffer to be empty and
 *  then blasts the data to it at full speed.  This means that the
 *  command string cannot exceed 256 chars.
 *
 *  After string is sent to the Galil, the routine will wait for the
 *  response string from the Galil. This routine will collect
 *  characters from the galil until a ":" or "?" is received.  The
 *  entire string, including the terminator is returned to the caller.
 *  If a timeout occurs or a "?" is received, then galil_error is set
 *  appropriately.  A good reply clears MCuGalilError.
 *  At the end, the read string is copied to pcRespStr if it's not NULL.
 *
 * Parameters:
 *
 *       iCardNoArg         (in) Card No
 *       pcCmdStr           (in) command string (256 chars max)
 *       pcRespStr          (out) response string (80 chars max)
 *
 * Returns:   If No error SUCCESS  else FAILURE
 *
\*****************************************************************/
int GASendReceiveGalil(int iCardNoArg, char *pcCmdStr, char *pcRespStr)
{
    int iStatus, iTch, iTemp, GAiRetries;
    int iIndex = 0;
    int bSpecialCmd = FALSE;
    char pcReadStr[MAXGASTR];
    static int iReEntrantFlag[3] = {0, 0, 0};

    pcReadStr[0] = 0;
    iStatus = SUCCESS;

    iStatus = GASendDMCCommand( ghDMC, pcCmdStr, pcRespStr, MAXGASTR);
    if (iStatus)
  		//printf("Error DMC: %d Command:%s Resp:%s\n",iStatus,pcCmdStr,pcRespStr);

    return iStatus;

}

int MCCheckWaferOnChuck(void)
{
    int iStatus;
    char pcReadStr[MAXGASTR];
    char pcCmdStr[MAXGASTR];

    iStatus = 0;

    sprintf(pcCmdStr, "MG @IN[13]\n\r");
    iStatus = GASendDMCCommand( ghDMC, pcCmdStr, pcReadStr, MAXGASTR);
    if (iStatus)
    {
	//printf("Error DMC checking wafer on chuck: %d\n", iStatus);
	iStatus = 0;
    }
    else
    {
	iStatus = atoi(pcReadStr);
	if (iStatus == 0) 
		iStatus = 1;
	else
		iStatus = 0;
    }

    return iStatus;
}


/****************************************************************\
 *
 * Function:    GAReadCharFromGalil
 *
 * Abstract:    Read Char From Motion Control card
 *
 *  Get a single character from the galil.  This routine receives a
 *  single character from the galil.  No checking on the character
 *  value is made.  A timeout sets MCuGalilError appropriately and
 *   returns 0.  Anything other than a timeout clears MCuGalilError.
 *
 * Parameters:
 *
 *       int    iCardNoArg            (in) Card No
 *       int    *piReadChar        (out) Read Char
 *
 * Returns:   SUCCESS or FAILURE
 *
\*****************************************************************/
int GAReadCharFromGalil(int iCardNoArg, int *piReadChar)
{
    int iTemp, iResult;
    int iBytesRead;

    if ( (iResult = GAValidateCardNo(iCardNoArg)) == FAILURE)
        return iResult;
    if ((iResult = TISetCounter(GL_TIMER[iCardNoArg], GA_TIMEOUT)) == FAILURE)
        goto EXIT;
    if (giGalilReadPointer >= giNumGalilRead)
    {
		iResult = DMCReadData(ghDMC, gGalilReadBuffer, MAXGASTR, &iBytesRead);
		giNumGalilRead = iBytesRead;
		giGalilReadPointer = 1;
        *piReadChar = gGalilReadBuffer[0];
    }
    else
    {
		*piReadChar = gGalilReadBuffer[giGalilReadPointer++];
    }

EXIT:
    return iResult;
}


/****************************************************************\
 *
 * Function:    GAWriteCharToGalil
 *
 * Abstract:    Write a single Char to Motion Control card
 *
 *  Write a single character To the galil.
 *  A timeout sets MCuGalilError appropriately and
 *  returns FAILURE.  Anything other than a timeout clears MCuGalilError.
 *
 * Parameters:
 *
 *       iCardNoArg            (in) Card No
 *       iChar              (in) char to write
 *
 * Returns:   SUCCESS or FAILURE
 *
\*****************************************************************/
int GAWriteCharToGalil(int iCardNoArg, int iChar)
{
    int iTemp, iStatus;
    char cBuf[5];

    if ( (iStatus = GAValidateCardNo(iCardNoArg)) == FAILURE)
        return iStatus;

    // Set up the Galil Timer register
    if ((iStatus = TISetCounter(GL_TIMER[iCardNoArg], GA_TIMEOUT)) == FAILURE)
        goto EXIT;

    cBuf[0] = iChar;
    cBuf[1] = '\0';
    iStatus = DMCWriteData(ghDMC, cBuf, 1, &iTemp); 

EXIT:
    return iStatus;
}

/****************************************************************\
 *
 * Function:    GAAmpInit
 *
 * Abstract:    Initialize the Amplifier
 *              NOTE:
 *                  EMOWasSet flag is reset in MEAmpInit()
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAAmpInit(void)
{
    int iResult;

    if (!GAiPreOnly)
    {
        IOWriteIO( -1, 0x01, IO_ROBOT_OUTPUT_B );
    }
    // GAiNumberOfGalilCards is init in GAInit()
    // MAX_GALIL_CARDS is 2
    if (GAiNumberOfGalilCards == MAX_GALIL_CARDS)
    {
        IOWriteIO( -1, 0x01, IO_PRE_OUTPUT_B );
    }

    // Wait for 20 Msec (short_wait)
    if ((iResult = TISetCounter( GA_TIMER, GA_WAIT_20_MSECS )) == FAILURE)
    {
        return FAILURE;
    }
    // Wait until timer expired
    while( !TICountExpired( GA_TIMER ) );

    if (!GAiPreOnly)
    {
        IOWriteIO( -1, 0xFF, IO_ROBOT_OUTPUT_B );
    }
    if (GAiNumberOfGalilCards == MAX_GALIL_CARDS)
    {
        IOWriteIO( -1, 0xFF, IO_PRE_OUTPUT_B );     // Was 0x1D
    }

    // Wait for 50 Msec
    if ((iResult = TISetCounter( GA_TIMER, GA_WAIT_50_MSECS )) == FAILURE)
    {
        return FAILURE;
    }
    // Wait until timer expired
    while( !TICountExpired( GA_TIMER ) );

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GARouteStringToQueueBuffer
 *
 * Abstract:    Route the String to Queue Buffer instead of to Galil Card
 *
 * Parameters:  int     (in)            iCardNoArg
 *              int     (in)            Number of Galil Command strings
 *              char *  (in)            Galil Command String
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GARouteStringToQueueBuffer(int iCardNoArg, int iNumOfCommands, char  *cpCommandString)
{
    // Make sure within the array boundary, enough space in queue buffer (action buffer) for this command
    if ( (strlen(cpCommandString) + strlen(stGAQueueCmdString[iCardNoArg].m_cCmdStr)) > MAXGASTR)
        return FAILURE;

    // Route the Command string to Local Queue
    stGAQueueCmdString[iCardNoArg].m_iNCmds += iNumOfCommands;
    strcat(stGAQueueCmdString[iCardNoArg].m_cCmdStr, cpCommandString);
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:   GAFlushOutCmdStringQueueToGalil
 *
 * Abstract:    Send out the MC local Commands string Queue to Galil and reset the local cmd
 *              string
 *
 * Parameters:  int     iCardNoArg     (in)        GALIL card no
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAFlushOutCmdStringQueueToGalil(int iCardNoArg)
{
    char *fpcpString;
    char cBuf[MAXGASTR];
    char cResp[MAXGASTR];
    int iIndex = 0;
    int iCount = 0;
    int iReturn = SUCCESS;

    // Validate the card no for this configuration
    if (GAValidateCardNo(iCardNoArg) == SUCCESS)
    {
        // Make sure there is Galil command to send
        if (stGAQueueCmdString[iCardNoArg].m_iNCmds > 0)
        {
            // Search for all commands separated by ';' and send to galil.
            while (stGAQueueCmdString[iCardNoArg].m_cCmdStr[iIndex])
            {
                if (stGAQueueCmdString[iCardNoArg].m_cCmdStr[iIndex] == ';')
                {
                    cBuf[iCount++] = '\r';
                    cBuf[iCount] = '\0';
                    if (GASendReceiveGalil(iCardNoArg, cBuf, cResp) == FAILURE)
                    {
                        return FAILURE;
                    }
                    iCount = 0;
                }
                else
                {
                    cBuf[iCount++] = stGAQueueCmdString[iCardNoArg].m_cCmdStr[iIndex];
                }
                ++iIndex;
            }

            // Initialize the local copy of Galil Command structure and ready for next queuing process
            iReturn = GAInitCmdString(iCardNoArg);
        }
        else
            // You shouldn't try to flush the Queue when the it was empty
            iReturn = FAILURE;
    }
    else
        iReturn = FAILURE;

    return iReturn;
}


/****************************************************************\
 *
 * Function:    GAInitCmdString
 *
 * Abstract:    Initialize the MC local Cmd string.
 *              This function is used to initialize the local copy of Galil command string
 *              structure for the action buffer.
 *
 * Parameters:  int     iCardNoArg     (in)        GALIL card no
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAInitCmdString(int iCardNoArg)
{
   // Validate the card no for this configuration
    if (GAValidateCardNo(iCardNoArg) == SUCCESS)
    {
        stGAQueueCmdString[iCardNoArg].m_StartQueuing = FALSE;
        memset(stGAQueueCmdString[iCardNoArg].m_cCmdStr, 0, MAXGASTR);
        stGAQueueCmdString[iCardNoArg].m_iNCmds = 0;
        return SUCCESS;
    }
    return FAILURE;
}


/****************************************************************\
 *
 * Function:   GAStartCommandsQueuing
 *
 * Abstract:    To start up the command Queuing and store the commands string
 *              in stGAQueueCmdString[iCardNoArg]
 *
 * Parameters:  int     iCardNoArg     (in)        GALIL card no
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAStartCommandsQueuing( int iCardNoArg )
{
    // Validate the card no for this configuration
    if ( GAValidateCardNo(iCardNoArg) == SUCCESS )
    {
        if (stGAQueueCmdString[iCardNoArg].m_StartQueuing == TRUE)
            return FAILURE;
        // Initialize the Local Copy Galil command string structure for action buffer
        if (GAInitCmdString(iCardNoArg) == SUCCESS)
            stGAQueueCmdString[iCardNoArg].m_StartQueuing = TRUE;
        return SUCCESS;
    }
    return FAILURE;
 }


/****************************************************************\
 *
 * Function:   GACancelCommandsQueuing
 *
 * Abstract:    To cancel the command Queuing and store the commands string
 *              in stGAQueueCmdString[iCardNoArg]
 *
 * Parameters:  int     iCardNoArg     (in)        GALIL card no
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GACancelCommandsQueuing(int iCardNoArg)
{
    // Validate the card no for this configuration
    if ( GAValidateCardNo(iCardNoArg) == SUCCESS )
    {
        // Reset the queuing start flag
        stGAQueueCmdString[iCardNoArg].m_StartQueuing = FALSE;
        return SUCCESS;
    }
    return FAILURE;
}


/****************************************************************\
 *
 * Function:    GASendSSCommand
 *
 * Abstract:    Send SS command to Galil and Read back the SS bit struct
 *              from Galil
 *
 * Parameters:  iCardNoArg       (in)       Galil card no
 *              pcSSBitStruct    (out)      pcSSBitStructt
 *
 * Returns:    SUCCESS or FAILURE
 *
\*****************************************************************/
int GASendSSCommand(int iCardNoArg, char *pcSSBitStruct)
{
    int iResult;
    int iIndex;
    char cpCommandString[MAXGASTR];
    char cpRespString[MAXGASTR];
    unsigned uGalilAxisName = GAALLAXIS0;
    fpstSSBitStruct fpstSSBitSt = (fpstSSBitStruct)pcSSBitStruct;

   // Initialze the command String
    memset(cpCommandString, 0, sizeof(char)*MAXGASTR);
    memset( pcSSBitStruct, 0, sizeof(pcSSBitStruct));

//    if (( iResult = GABuildAxesArgumentsCommand(iCardNoArg, uGalilAxisName,
//            SENSE_SWITCHES_COMMAND, TRUE, cpCommandString)) == FAILURE)
//    {
//        return FAILURE;
//    }
    if (( iResult = GABuildAxesArgumentsCommand(iCardNoArg, uGalilAxisName,
            TELL_SWITCHES_COMMAND, TRUE, cpCommandString)) == FAILURE)
    {
        return FAILURE;
    }

    // The num of reply is 0 so that GAReadCharFromGalil() can get the data from Galil FIFO
    if((iResult = GASendReceiveGalil(iCardNoArg, cpCommandString, cpRespString)) == FAILURE)
    {
        return FAILURE;
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GASendAbortMotionCommand
 *
 * Abstract:    Send AB command to Galil
 *
 * Parameters:  iCardNoArg        (in)      Galil card no
 *              iAbortMotionOnly  (in)      Abort Motion flag
 *                        TRUE = abort motion without aborting program
 *                        FALSE = abort both motion and program
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASendAbortMotionCommand(int iCardNoArg, int iAbortMotionOnly)
{
    int     iResult;
    int     iEndOfString;
    char    cbuf[10];

    // Once Queue started, the cpCommand[] will be terminated by ";" instead of "\r"
    if ((iResult = GAIsEndOfString(iCardNoArg, &iEndOfString)) == FAILURE)
    {
        return FAILURE;
    }

    if ( iEndOfString == TRUE )
    {
        if(iAbortMotionOnly)
        {
            iResult = GASendReceiveGalil(iCardNoArg, (char *)"AB1\r", cbuf);
        }
        else
        {
            iResult = GASendReceiveGalil(iCardNoArg, (char *)"AB\r", cbuf);
        }
    }
    else
    {
        if(iAbortMotionOnly)
        {
             // Route the Command string to Local Queue
            iResult = GARouteStringToQueueBuffer(iCardNoArg, 1, (char *)"AB1;");
        }
        else
        {
             // Route the Command string to Local Queue
            iResult = GARouteStringToQueueBuffer(iCardNoArg, 1, (char *)"AB;");
        }
    }
    return iResult;
}


/****************************************************************\
 *
 * Function:    GASetTimeCommand
 *
 * Abstract:    Send Set Time Command (TM) command to Galil
 *
 * Parameters:  iCardNoArg          (in)     Galil card no
 *              lTimeCommandArg     (Out)    New control loop time step value
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASetTimeCommand(int iCardNoArg, long lTimeCommandArg)
{
    int     iResult;
    char    cpCommandString[MAXGASTR];
    int     iEndOfString;
    char    cpBuf[10];

    // Once Queue started, the cpCommand[] will be terminated by ";" instead of "\r"
    if ((iResult = GAIsEndOfString(iCardNoArg, &iEndOfString)) == FAILURE)
    {
        return FAILURE;
    }

    // Build up Galil command: TM
    if (iEndOfString) sprintf(cpCommandString, "%s %ld\r", TIME_COMMAND, lTimeCommandArg);
    else sprintf(cpCommandString, "%s %ld;", TIME_COMMAND, lTimeCommandArg);
    if ( iResult == SUCCESS )
    {
        if ( iEndOfString == TRUE )
        {
            iResult = GASendReceiveGalil( iCardNoArg, cpCommandString, cpBuf);
        }
        else
        {
            // Route the Command string to Local Queue
            iResult = GARouteStringToQueueBuffer(iCardNoArg, 1, cpCommandString);
        }
    }
    return iResult;
}


/****************************************************************\
 *
 * Function:    GASendConfigureSystemCommand
 *
 * Abstract:    Send Configure System (CN) command to Galil
 *
 * Parameters:  iCardNoArg          (in)    Galil card no
 *              iLimitSwitchArg     (in)    Limit switch active polarity (1, -1)
 *              iHomeSwitchArg      (in)    Home switch active polarity (1, -1)
 *              iLatchInputArg      (in)    Latch input active polarity (1, -1)
 *              iReservedArg        (in)    ?
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASendConfigureSystemCommand(int iCardNoArg, int iLimitSwitchArg, int iHomeSwitchArg, int iLatchInputArg, int iReservedArg)
{
    int     iResult;
    char    cpCommandString[MAXGASTR], cpTemp[MAXGASTR];
    int     iEndOfString;

    // Once Queue started, the cpCommand[] will be terminated by ";" instead of "\r"
    if ((iResult = GAIsEndOfString(iCardNoArg, &iEndOfString)) == FAILURE)
    {
        return FAILURE;
    }

    sprintf(cpCommandString, "%s ", CONFIG_SYSTEM_COMMAND);
    if ((iLimitSwitchArg == -1) || (iLimitSwitchArg == 1))
    {
        sprintf(cpTemp, "%d", iLimitSwitchArg);
        strcat(cpCommandString, cpTemp);
    }
    strcat(cpCommandString, ",");
    if ((iHomeSwitchArg == -1) || (iHomeSwitchArg == 1))
    {
        sprintf(cpTemp, "%d", iHomeSwitchArg);
        strcat(cpCommandString, cpTemp);
    }
    strcat(cpCommandString, ",");
    if ((iLatchInputArg == -1) || (iLatchInputArg == 1))
    {
        sprintf(cpTemp, "%d", iLatchInputArg);
        strcat(cpCommandString, cpTemp);
    }
    strcat(cpCommandString, ",");
    if (iReservedArg >= 0)
    {
        sprintf(cpTemp, "%d", iReservedArg);
        strcat(cpCommandString, cpTemp);
    }

    if (iEndOfString) strcat(cpCommandString, "\r");
    else strcat(cpCommandString, ";");

    if ( iEndOfString == TRUE )
    {
        iResult = GASendReceiveGalil( iCardNoArg, cpCommandString, cpTemp);
    }
    else
    {
        // Route the Command string to Local Queue
        iResult = GARouteStringToQueueBuffer(iCardNoArg, 1, cpCommandString);
    }
    return iResult;
}


/****************************************************************\
 *
 * Function:    GASendAxesCommand
 *
 * Abstract:    Send command to Galil that only needs axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              pcCommandArg        (in) Galil command to send
 *              uGalilAxisArg       (in) Galil axes to affect
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASendAxesCommand(int iCardNoArg, char *pcCommandArg, unsigned uGalilAxisArg)
{
    char cpCommandString[MAXGASTR];
    int iResult, iEndOfString;
    char cpBuf[10];

    // Once Queue started, the cpCommand[] will be terminated by ";" instead of "\r"
    if (GAIsEndOfString(iCardNoArg, &iEndOfString) == FAILURE)
        return FAILURE;

    // Build up Galil command
    if (GABuildAxesArgumentsCommand(iCardNoArg, uGalilAxisArg, pcCommandArg, iEndOfString, cpCommandString) == FAILURE)
        return FAILURE;
    if ( iEndOfString == TRUE )
    {
        iResult = GASendReceiveGalil( iCardNoArg, cpCommandString, cpBuf );
    }
    else
    {
        // Route the Command string to Local Queue
        iResult = GARouteStringToQueueBuffer( iCardNoArg, 1, cpCommandString );
    }

    return iResult;
}


/****************************************************************\
 *
 * Function:    GASetValsLongDefined
 *
 * Abstract:    Send command to Galil that needs long parameters as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              iParamDescArg       (in) Define from Datafile module
 *              uGalilAxisArg       (in) Galil axes to affect
 *              plValuesArg         (in) Long values to send to Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASetValsLongDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, long *plValuesArg)
{
    char cpCommandString[5];

    // copy the command string
    if( GASetCommandString(iParamDescArg, cpCommandString) == FAILURE)
        return FAILURE;

    if (GASetValsLong(iCardNoArg, cpCommandString, uGalilAxisArg, plValuesArg) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GASetValsDoubleDefined
 *
 * Abstract:    Send command to Galil that needs double parameters as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              iParamDescArg       (in) Define from Datafile module
 *              uGalilAxisArg       (in) Galil axes to affect
 *              pdValuesArg         (in) Double values to send to Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASetValsDoubleDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, double *pdValuesArg)
{
    char cpCommandString[5];

    // copy the command string
    if( GASetCommandString(iParamDescArg, cpCommandString) == FAILURE)
        return FAILURE;

    if (GASetValsDouble(iCardNoArg, cpCommandString, uGalilAxisArg, pdValuesArg) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GASetValsLong
 *
 * Abstract:    Send command to Galil that needs long parameters as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              pcCommandArg        (in) Galil command to send
 *              uGalilAxisArg       (in) Galil axes to affect
 *              plValuesArg         (in) Long values to send to Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASetValsLong(int iCardNoArg, char *pcCommandArg, unsigned uGalilAxisArg, long *plValuesArg)
{
    char cpCommandString[MAXGASTR];
    int iResult, iEndOfString;
    char cpBuf[8];

    // Once Queue started, the cpCommand[] will be terminated by ";" instead of "\r"
    if (GAIsEndOfString(iCardNoArg, &iEndOfString) == FAILURE)
        return FAILURE;

    // Build up Galil command
    iResult = GABuildParameterArgumentsCommand(iCardNoArg, uGalilAxisArg,
                                                plValuesArg, pcCommandArg,
                                                iEndOfString, cpCommandString);
    if ( iResult == SUCCESS )
    {
        if ( iEndOfString == TRUE )
        {
            iResult = GASendReceiveGalil( iCardNoArg, cpCommandString, cpBuf );
        }
        else
        {
            // Route the Command string to Local Queue
            iResult = GARouteStringToQueueBuffer( iCardNoArg, 1, cpCommandString );
        }
    }
    return iResult;
}


/****************************************************************\
 *
 * Function:    GASetValsDouble
 *
 * Abstract:    Send command to Galil that needs double parameters as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              pcCommandArg        (in) Galil command to send
 *              uGalilAxisArg       (in) Galil axes to affect
 *              pdValuesArg         (in) Double values to send to Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASetValsDouble(int iCardNoArg, char *pcCommandArg, unsigned uGalilAxisArg, double *pdValuesArg)
{
    char cpCommandString[MAXGASTR];
    int iResult, iEndOfString;
    char cpBuf[8];

    // Once Queue started, the cpCommand[] will be terminated by ";" instead of "\r"
    if (GAIsEndOfString(iCardNoArg, &iEndOfString) == FAILURE)
        return FAILURE;

    // Build up Galil command
    iResult = GABuildDoubleParameterArgumentsCommand(iCardNoArg, uGalilAxisArg,
                                                        pdValuesArg, pcCommandArg,
                                                        iEndOfString, cpCommandString);
    if ( iResult == SUCCESS )
    {
        if ( iEndOfString == TRUE )
        {
            iResult = GASendReceiveGalil(iCardNoArg, cpCommandString, cpBuf );
        }
        else
        {
            // Route the Command string to Local Queue
            iResult = GARouteStringToQueueBuffer( iCardNoArg, 1, cpCommandString );
        }
    }
    return iResult;
}


/****************************************************************\
 *
 * Function:    GAGetValsLongDefined
 *
 * Abstract:    Send command to Galil that returns long values as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              iParamDescArg       (in) Define from Datafile module
 *              uGalilAxisArg       (in) Galil axes to read
 *              plValuesArg         (in) Long values returned from Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAGetValsLongDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, long *plValuesArg)
{
    char cpCommandString[5];

    // copy the command string
    if( GASetCommandString(iParamDescArg, cpCommandString) == FAILURE)
        return FAILURE;

    if (GAGetValsLong(iCardNoArg, cpCommandString, uGalilAxisArg, plValuesArg) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GAGetValsDoubleDefined
 *
 * Abstract:    Send command to Galil that returns double values as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              iParamDescArg       (in) Define from Datafile module
 *              uGalilAxisArg       (in) Galil axes to read
 *              pdValuesArg         (in) double values returned from Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAGetValsDoubleDefined(int iCardNoArg, int iParamDescArg, unsigned uGalilAxisArg, double *pdValuesArg)
{
    char cpCommandString[5];

    // copy the command string
    if( GASetCommandString(iParamDescArg, cpCommandString) == FAILURE)
        return FAILURE;

    if (GAGetValsDouble(iCardNoArg, cpCommandString, uGalilAxisArg, pdValuesArg) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GAGetValsLong
 *
 * Abstract:    Send command to Galil that returns long values as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              pcCommandArg        (in) Galil command to send
 *              uGalilAxisArg       (in) Galil axes to read
 *              plValuesArg         (in) Long values returned from Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAGetValsLong(int iCardNoArg, char *pcCommandArg, unsigned uGalilAxisArg, long *plValuesArg)
{
    int iResult;
    char cpCommandString[MAXGASTR], cpReadString[MAXGASTR];

    if (strcmp(pcCommandArg, REF_POSITION_COMMAND) && strcmp(pcCommandArg, TELL_SWITCHES_COMMAND) &&
            strcmp(pcCommandArg, TELL_POSITION_COMMAND) && strcmp(pcCommandArg, TELL_ERROR_COMMAND) &&
            strcmp(pcCommandArg, REPORT_LATCH_POS_COMMAND))
    {
        if (GABuildInterrogationCommand(iCardNoArg, uGalilAxisArg, pcCommandArg, TRUE, cpCommandString) == FAILURE)
            return FAILURE;
    }
    else
    {
        if (GABuildAxesArgumentsCommand(iCardNoArg, uGalilAxisArg, pcCommandArg, TRUE, cpCommandString) == FAILURE)
            return FAILURE;
    }

    iResult = GASendReceiveGalil(iCardNoArg, cpCommandString, cpReadString);

    if (iResult == SUCCESS)
        iResult = GAConvertAsciiToLong(uGalilAxisArg, cpReadString, plValuesArg);

    return iResult;
}


/****************************************************************\
 *
 * Function:    GAGetValsDouble
 *
 * Abstract:    Send command to Galil that returns double values as
 *      well as axes designations.
 *
 * Parameters:  iCardNoArg          (in) Galil card no
 *              pcCommandArg        (in) Galil command to send
 *              uGalilAxisArg       (in) Galil axes to read
 *              pdValuesArg         (in) double values returned from Galil
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAGetValsDouble(int iCardNoArg, char *pcCommandArg, unsigned uGalilAxisArg, double *pdValuesArg)
{
    int iResult;
    char cpCommandString[MAXGASTR], cpReadString[MAXGASTR];
    if (strcmp(pcCommandArg, TELL_TORQUE_COMMAND))
    {
        if (GABuildInterrogationCommand(iCardNoArg, uGalilAxisArg, pcCommandArg, TRUE, cpCommandString) == FAILURE)
            return FAILURE;
    }
    else
    {
        if (GABuildAxesArgumentsCommand(iCardNoArg, uGalilAxisArg, pcCommandArg, TRUE, cpCommandString) == FAILURE)
            return FAILURE;
    }

    iResult = GASendReceiveGalil(iCardNoArg, cpCommandString, cpReadString);
    if (iResult == SUCCESS)
        iResult = GAConvertAsciiToDouble(uGalilAxisArg, cpReadString, pdValuesArg);

    return iResult;
}
