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
* Program:      Basic file for SECS-II
* File:         secxhost.c
* Functions:    S2InitSecs
*               S2InitStructMembers
*               S2InitFluoroTracTagData
*               S2GetLastError
*               S2GetWTCurrentActivity
*               S2GetNofRetries
*               S2GetT1
*               S2GetT2
*               S2GetMaxRetries
*               S2GetMsgBlock
*               S2GetTMsgBlock
*               S2GetMessageSent
*               S2SetLastError
*               S2SetWTCurrentActivity
*               S2SetNofRetries
*               S2SetT1
*               S2SetT2
*               S2SetMaxretries
*               S2SetMsgBlock
*               S2SetTMsgBlock
*               S2SetMessageSent
*               S2GetSecsInfoCom
*               S2GetSecsInfoDev
*               S2AbortSecs
*               S2CheckSumOK
*               S2PrepareSecsMsg
*               S2InterpretSecsMsg
*               S2SecsCommand
*
* Description:  Performes activities for the Secs-II layer of SECS Communication,
*               the SECS-I layer is integrated into the Comm module
*
* Modification history:
* Rev      ECO#    Date    Author      Brief Description
*
****************************************************************/

//PMP #define COMPORT6

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "sck.h"
#include "secsg.h"
#include "secsl.h"
#include "serg.h"
#include "scmac.h"
#include "fiog.h"
#include "scmem.h"

#include "ser.h"
//#include "serl.h"
#include "serm.h"


int iQueryEquipmentDevId;
//Structure keeps the secs info for com port - 1. layer
fpstSecsInfo fpstSecsInfoCom[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
//Multiple Device ID - structure for 2. layer
fpstSecsInfo fpstSecsInfoDev[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
//Amatx, to keep track the Taglog, Page number sent and received
fpstFluoroTracTagData fpstFluoroTracTagDataArray[NUMBER_OF_FLUOROTRAC_DATA] = {NULL, NULL, NULL, NULL};

int iSecsSpecialFlag;                   /* this flag is for distinguishing ex_secs executing from command line
                                         * or from macro. */

/*****************************************************************************
 * Function Name:   S2InitSecs
 * Description:     Allocates memory for Secs-II objects, and initializes the
 *                  data for special device FluoroTrac. For each com-port we
 *                  allocate one structure, and we also allocate 8 structures
 *                  for devices connected, those structures will be shared by all com-ports
 * Parameter:       iPortNumArg - number of com-port to be initialized (COM1, COM2, COM3)
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2InitSecs(int iPortNumArg)
{
    int iCounter;

    if( !(fpstSecsInfoCom[iPortNumArg-1] = (fpstSecsInfo)HPmalloc( sizeof(stSecsInfo) )) )
        return FAILURE;
printf("fpCom%d=%x\n",iPortNumArg,fpstSecsInfoCom[iPortNumArg-1]);
    if(S2InitStructMembers(COM, iPortNumArg-1))
        return FAILURE;

    iQueryEquipmentDevId = FALSE;
    iSecsSpecialFlag = FALSE;

    if( !(fpstSecsInfoDev[iPortNumArg-1] = (fpstSecsInfo)HPmalloc( sizeof(stSecsInfo) )) )
        return FAILURE;
printf("fpDev%d=%x\n",iPortNumArg,fpstSecsInfoDev[iPortNumArg-1]);
    if(S2InitStructMembers(DEV, iPortNumArg-1))
        return FAILURE;

    // We only need one Set of fpstSecsInfoMultDev[] to be shared by all the ports
    // if COM1 we are in the SECS as Device mode, so we allocate memory for multiplay devices when we
    // are initializing COM2
//    if(iPortNumArg == COM2)
    if(iPortNumArg == COM3)
    {
        // Amatx
        // Initialize all FluoroTrac Data (Port A to H, Lan Port & PIO device)
        // Smart-Comm removes the Port Id in bits 8 -11 when S4, F112 returns
        for (iCounter = 0; iCounter < NUMBER_OF_FLUOROTRAC_DATA; iCounter++)
        {
            if( !(fpstFluoroTracTagDataArray[iCounter] = (fpstFluoroTracTagData) HPmalloc( sizeof(stFluoroTracTagData) )) )
                return FAILURE;
            if (S2InitFluoroTracTagData(iCounter) == FAILURE)
                return FAILURE;
        }
    }

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2InitStructMembers
 * Description:     Initializes the members of the Secs structure, used from S2InitSecs
 * Parameter:       iTypeArg - type of structure we want to initialize, COM - port structure
 *                                                                      DEV - device structure
 *                  iIndexArg - index of structure, for COM is 0, 1, 2, 3
 *                                                  for DEV is 0, 1, 2, 3, 4, 5, 6, 7
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2InitStructMembers( int iTypeArg, int iIndexArg)
{
    fpstSecsInfo fpstTemp;
    int iOnOffFlagSecs;

    // check what type of SECS it is
    if (FIOGetCfgFeatureFlags(SECS_DEVICE, &iOnOffFlagSecs) == FAILURE)
        return FAILURE;

    if(iTypeArg == COM)
        fpstTemp = fpstSecsInfoCom[iIndexArg];
    else if(iTypeArg == DEV)
        fpstTemp = fpstSecsInfoDev[iIndexArg];
    else
        return FAILURE;

    fpstTemp->m_wLastError = OK;
    fpstTemp->m_wWTCurrActivity = IDLE;
    fpstTemp->m_wIBCurrActivity = IDLE;
    fpstTemp->m_ulTimeOut = 0;
//printf("fpTemp=%x index=%d WTAct=%d\n",fpstTemp,iIndexArg,fpstTemp->m_wWTCurrActivity);
    if(iTypeArg == DEV)
        fpstTemp->m_wTimerID = 0;
    else
        fpstTemp->m_wTimerID = iIndexArg;
    fpstTemp->m_wNofRty = 0;
    // for Secs as a device we make our controller's default DeviceID equal 1
    // in Secs as device  mode we later check the DevID received in S1F1 message
    // and set this DevID to the internal structure, so all later messages have to have the coorect DevID in the header
    if(iOnOffFlagSecs)
        fpstTemp->m_wDevID = 1;
    else
        fpstTemp->m_wDevID = 0;
    fpstTemp->m_iMsgSent = 0;
    fpstTemp->m_uiInxAlid = 0;
    fpstTemp->m_uiInxCeid = 0;
    fpstTemp->m_uiInxHcack = 0;
    fpstTemp->m_uiInxEac = 0;
    fpstTemp->m_wT1 = 1000;
    if(iOnOffFlagSecs)
    {
//        fpstTemp->m_wT2 = 200;
//        fpstTemp->m_wT3 = 1000;
//        fpstTemp->m_wT4 = 1000;
        fpstTemp->m_wT2 = 1000;
        fpstTemp->m_wT3 = 30000;
        fpstTemp->m_wT4 = 30000;
    }
    else
    {
//        fpstTemp->m_wT2 = 20;
//        fpstTemp->m_wT3 = 500;
//        fpstTemp->m_wT4 = 100;
        fpstTemp->m_wT2 = 1000;
        fpstTemp->m_wT3 = 30000;
        fpstTemp->m_wT4 = 30000;
    }
//    fpstTemp->m_wT5 = 100;
//    fpstTemp->m_wT6 = 100;
    fpstTemp->m_wT5 = 30000;
    fpstTemp->m_wT6 = 30000;
    fpstTemp->m_wMaxRetries = 3;

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2InitFlouroTracTagData
 * Description:     Initializes members of the data structure for Fluorotrac device
 * Parameter:       iIndex - index of the structure, (0,1,2,3)
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
// Amatx, initialize the Smart Comm FluoroTrac Tag Data
int S2InitFluoroTracTagData(int iIndexArg)
{
    if(iIndexArg < eS4F111Sent || iIndexArg > eS4F114Received)
        return FAILURE;
    // Format: A9 Dev Id
    fpstFluoroTracTagDataArray[iIndexArg]->m_wDevID = 0;
    // Format: A5 bin number (1-255)
    fpstFluoroTracTagDataArray[iIndexArg]->m_bTagLog = 0;
    // Format: A5 'A' to 'P' Manual says 41
    fpstFluoroTracTagDataArray[iIndexArg]->m_bPageNumber = 0;
    // Format: A5 (0-3) and 0xFF
    fpstFluoroTracTagDataArray[iIndexArg]->m_bAck6 = 0xFF;
    // Format: 41 ASCII
    memset(fpstFluoroTracTagDataArray[iIndexArg]->m_cFTTagData, 0x20, TAG_DATA_SIZE);

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2GetSecsStructureMember
 * Description:     Function retrieves member of the data Secs structure
 * Parameter:       iPointerTypeArg - COM for the com-port structure
 *                                    DEV for the device structure
 *                  iIndexArg - index of the structure, for COM type COM1, COM2, COM3
 *                                                      for DEV type 0, 1, 2, 3, 4, 5, 6, 7
 *                  *wReturnArg - pointer which points to the retrieved value
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2GetSecsStructureMember(int iPointerTypeArg, int iIndexArg, WORD *wReturnArg, int iMemberTypeArg)
{
    int iCounter, iFoundDevID = FALSE;

    if(iPointerTypeArg != COM && iPointerTypeArg != DEV)
        return FAILURE;

#ifdef COMPORT6
    if(iIndexArg < 1 || iIndexArg > 6)
        return FAILURE;
#else
    if(iIndexArg < 1 || iIndexArg > 3)
        return FAILURE;
#endif

    if(iPointerTypeArg == COM)
    {
        switch(iMemberTypeArg)
        {
            case LAST_ERROR :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_wLastError;
                break;
            case WTCURR_ACTIVITY :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_wWTCurrActivity;
                break;
            case NUM_OF_RETRIES :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_wNofRty;
                break;
            case T1 :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_wT1;
                break;
            case T2 :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_wT2;
                break;
            case MAX_RETRIES :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_wMaxRetries;
                break;
            case MESSAGE_SENT :
                *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_iMsgSent;
                break;
            default :
                return FAILURE;
        }
    }
    else
    {
        switch(iMemberTypeArg)
        {
            case LAST_ERROR :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_wLastError;
                break;
            case WTCURR_ACTIVITY :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_wWTCurrActivity;
                break;
            case NUM_OF_RETRIES :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_wNofRty;
                break;
            case T1 :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_wT1;
                break;
            case T2 :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_wT2;
                break;
            case MAX_RETRIES :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_wMaxRetries;
                break;
            case MESSAGE_SENT :
                *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_iMsgSent;
                break;
            default:
                return FAILURE;
        }
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2SetSecsStructureMember
 * Description:     Function sets member of the data secs structure
 * Parameter:       iPointerTypeArg - COM for the com-port structure
 *                                    DEV for the device structure
 *                  iIndexArg - for COM type index of the structure, COM1, COM2, COM3
 *                              for DEV type Device ID, which has the picked structure to contain
 *                  wSetValueArg - value for structure member to be set
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2SetSecsStructureMember(int iPointerTypeArg, int iIndexArg, WORD wSetValueArg, int iMemberTypeArg)
{
    int iCounter, iFoundDevID = FALSE;

    if(iPointerTypeArg != COM && iPointerTypeArg != DEV)
        return FAILURE;

#ifdef COMPORT6
    if(iIndexArg < 1 || iIndexArg > 6)
        return FAILURE;
#else
    if(iIndexArg < 1 || iIndexArg > 3)
        return FAILURE;
#endif

    if(iPointerTypeArg == COM)
    {
        switch(iMemberTypeArg)
        {
            case LAST_ERROR :
                fpstSecsInfoCom[iIndexArg-1]->m_wLastError = wSetValueArg;
                break;
            case WTCURR_ACTIVITY :
                fpstSecsInfoCom[iIndexArg-1]->m_wWTCurrActivity = wSetValueArg;
                break;
            case NUM_OF_RETRIES :
                fpstSecsInfoCom[iIndexArg-1]->m_wNofRty = wSetValueArg;
                break;
            case T1 :
                fpstSecsInfoCom[iIndexArg-1]->m_wT1 = wSetValueArg;
                break;
            case T2 :
                fpstSecsInfoCom[iIndexArg-1]->m_wT2 = wSetValueArg;
                break;
            case MAX_RETRIES :
                fpstSecsInfoCom[iIndexArg-1]->m_wMaxRetries = wSetValueArg;
                break;
            case MESSAGE_SENT :
                fpstSecsInfoCom[iIndexArg-1]->m_iMsgSent = wSetValueArg;
                break;
            default:
                return FAILURE;
        }
    }
    else
    {
        switch(iMemberTypeArg)
        {
            case LAST_ERROR :
                fpstSecsInfoDev[iIndexArg-1]->m_wLastError = wSetValueArg;
                break;
            case WTCURR_ACTIVITY :
                fpstSecsInfoDev[iIndexArg-1]->m_wWTCurrActivity = wSetValueArg;
                break;
            case NUM_OF_RETRIES :
                fpstSecsInfoDev[iIndexArg-1]->m_wNofRty = wSetValueArg;
                break;
            case T1 :
                fpstSecsInfoDev[iIndexArg-1]->m_wT1 = wSetValueArg;
                break;
            case T2 :
                fpstSecsInfoDev[iIndexArg-1]->m_wT2 = wSetValueArg;
                break;
            case MAX_RETRIES :
                fpstSecsInfoDev[iIndexArg-1]->m_wMaxRetries = wSetValueArg;
                break;
            case MESSAGE_SENT :
                fpstSecsInfoDev[iIndexArg-1]->m_iMsgSent = wSetValueArg;
                break;
            default:
                return FAILURE;
	}
    }

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2GetMsgBlock
 * Description:     Function retrieves member of the data Secs structure (Message Block)
 * Parameter:       iPointerTypeArg - COM for the com-port structure
 *                                    DEV for the device structure
 *                  iIndexArg - index of the structure, for COM type COM1, COM2, COM3
 *                                                      for DEV type 0, 1, 2, 3, 4, 5, 6, 7
 *                  wReturnArg - pointer which points to the string retrieved from the structure
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2GetMsgBlock(int iPointerTypeArg, int iIndexArg, unsigned char **wReturnArg)
{
    int iCounter, iFoundDevID = FALSE;

    if(iPointerTypeArg != COM && iPointerTypeArg != DEV)
        return FAILURE;

#ifdef COMPORT6
    if(iIndexArg < 1 || iIndexArg > 6)
        return FAILURE;
#else
    if(iIndexArg < 1 || iIndexArg > 3)
        return FAILURE;
#endif

    if(iPointerTypeArg == COM)
    {
        *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_ucMsgBlock;
    }
    else
    {
        *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_ucMsgBlock;
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2GetTMessageBlock
 * Description:     Function retrieves member of the data Secs structure (Transmitted Message Block)
 * Parameter:       iPointerTypeArg - COM for the com-port structure
 *                                    DEV for the device structure
 *                  iIndexArg - index of the structure, for COM type COM1, COM2, COM3
 *                                                      for DEV type 0, 1, 2, 3, 4, 5, 6, 7
 *                  *wReturnArg - pointer which points to string retrieved from the structure
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2GetTMsgBlock(int iPointerTypeArg, int iIndexArg, unsigned char **wReturnArg)
{
    int iCounter, iFoundDevID = FALSE;

    if(iPointerTypeArg != COM && iPointerTypeArg != DEV)
        return FAILURE;

#ifdef COMPORT6
    if(iIndexArg < 1 || iIndexArg > 6)
        return FAILURE;
#else
    if(iIndexArg < 1 || iIndexArg > 3)
        return FAILURE;
#endif

    if(iPointerTypeArg == COM)
    {
        *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_ucTMsgBlock;
    }
    else
    {
        *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_ucTMsgBlock;
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2GetTimeOut
 * Description:     Function retrieves member of the data Secs structure (TimeOut)
 * Parameter:       iPointerTypeArg - COM for the com-port structure
 *                                    DEV for the device structure
 *                  iIndexArg - index of the structure, for COM type COM1, COM2, COM3
 *                                                      for DEV type 0, 1, 2, 3, 4, 5, 6, 7
 *                  *wReturnArg - pointer which points to the retrieved value
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2GetTimeOut(int iPointerTypeArg, int iIndexArg, DWORD *wReturnArg)
{
    int iCounter, iFoundDevID = FALSE;

    if(iPointerTypeArg != COM && iPointerTypeArg != DEV)
        return FAILURE;

#ifdef COMPORT6
    if(iIndexArg < 1 || iIndexArg > 6)
        return FAILURE;
#else
    if(iIndexArg < 1 || iIndexArg > 3)
        return FAILURE;
#endif

    if(iPointerTypeArg == COM)
    {
        *wReturnArg = fpstSecsInfoCom[iIndexArg-1]->m_ulTimeOut;
    }
    else
    {
        *wReturnArg = fpstSecsInfoDev[iIndexArg-1]->m_ulTimeOut;
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2SetTimeOut
 * Description:     Function sets member of the data secs structure (TimeOut)
 * Parameter:       iPointerTypeArg - COM for the com-port structure
 *                                    DEV for the device structure
 *                  iIndexArg - index of the structure, for COM type COM1, COM2, COM3
 *                                                      for DEV type 0, 1, 2, 3, 4, 5, 6, 7
 *                  wSetValueArg - value for structure member to be set
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int S2SetTimeOut(int iPointerTypeArg, int iIndexArg, DWORD wSetValueArg)
{
    int iCounter, iFoundDevID = FALSE;

    if(iPointerTypeArg != COM && iPointerTypeArg != DEV)
        return FAILURE;

#ifdef COMPORT6
    if(iIndexArg < 1 || iIndexArg > 6)
        return FAILURE;
#else
    if(iIndexArg < 1 || iIndexArg > 3)
        return FAILURE;
#endif

    if(iPointerTypeArg == COM)
    {
        fpstSecsInfoCom[iIndexArg-1]->m_ulTimeOut = wSetValueArg;
    }
    else
    {
        fpstSecsInfoDev[iIndexArg-1]->m_ulTimeOut = wSetValueArg;
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2GetSecsInfoCom
 * Description:     Function gets the pointer to the Secs com-port structure for particular port
 * Parameter:       iPortNumArg - number of com-port
 * Returns:         pointer to the according com-port structure
 ****************************************************************************/
fpstSecsInfo S2GetSecsInfoCom(int iPortNumArg)
{
    if((iPortNumArg < 1) || (iPortNumArg > 6))
        return NULL;
    return(fpstSecsInfoCom[iPortNumArg-1]);
}
/****************************************************************************
 * Function Name:   S2GetSecsInfoDev
 * Description:     Function gets the pointer to the Secs device structure for particular device
 * Parameter:       uDevIDArg - device ID for which we want to pull out the structure
 * Returns:         pointer to the according com-port structure
 ****************************************************************************/
fpstSecsInfo S2GetSecsInfoDev(unsigned uDevIDArg)
{
    int iCounter;

    for(iCounter = 0; iCounter < NUMBER_OF_DEV_ID; iCounter++)
    {
        if(fpstSecsInfoDev[iCounter]->m_wDevID == uDevIDArg)
            return( fpstSecsInfoDev[iCounter] );
    }
    return(NULL);
}
/****************************************************************************
 * Function Name:   S2AbortSecs
 * Description:     Clears all the data members of the Secs structure
 * Parameter:       None
 * Returns:         None
 ****************************************************************************/
int S2AbortSecs(void)
{
    int iCounter;
#ifdef COMPORT6
    int nPortNum = 6;
#else
    int nPortNum = 4;
#endif

    iQueryEquipmentDevId = FALSE;

    for(iCounter = 0; iCounter < nPortNum; iCounter++)
    {
        if(fpstSecsInfoCom[iCounter])
        {
            fpstSecsInfoCom[iCounter]->m_wLastError = OK;
            fpstSecsInfoCom[iCounter]->m_wWTCurrActivity = IDLE;
            fpstSecsInfoCom[iCounter]->m_wIBCurrActivity = IDLE;
            fpstSecsInfoCom[iCounter]->m_ulTimeOut = 0;
            fpstSecsInfoCom[iCounter]->m_wNofRty = 0;
            fpstSecsInfoCom[iCounter]->m_iMsgSent = 0;
        }
    }
/*
    for (iCounter = 0; iCounter < NUMBER_OF_DEV_ID; iCounter++)
    {
        if(fpstSecsInfoDev[iCounter])
        {
            fpstSecsInfoDev[iCounter]->m_wLastError = OK;
            fpstSecsInfoDev[iCounter]->m_wWTCurrActivity = IDLE;
            fpstSecsInfoDev[iCounter]->m_wIBCurrActivity = IDLE;
            fpstSecsInfoDev[iCounter]->m_ulTimeOut = 0;
            fpstSecsInfoDev[iCounter]->m_wNofRty = 0;
            fpstSecsInfoDev[iCounter]->m_iMsgSent = 0;
        }
    }

*/
//    iPortNum = 3;
//    SERClosePorts(iPortNum);
//    if (SERInitPorts(iPortNum, BAUD_9600, NO_PARITY, EIGHT_DATA_BITS, ONE_STOP_BIT, FALSE, FALSE, TRUE) == FAILURE)
//    {
//
//printf("S2AbortSecs unsuccessful!\n");
//	    return FAILURE;
//    }

//printf("S2AbortSecs successful!\n");

    return SUCCESS;

}
/****************************************************************************
 * Function Name:   S2CheckSumOK
 * Description:     Check the CheckSum of the given secs message
 * Parameter:       pointer to the message string to be checked on CheckSum
 * Returns:         0 - if  checksum is ok
 *                  1 - if sent checksum differs from the computed one
 ****************************************************************************/
int S2CheckSumOK(unsigned char *fpucMsg)
{
    unsigned int uiCalcChkSum = 0, uiSentChkSum, uiLenghtByte, uiCounter;

    //first byte of message is the lenght, it is total bytes of header and text excluding the
    //checksum and byte itself
    uiLenghtByte = (unsigned int)fpucMsg[0] + 1;

    //checksum is unsigned sum of all bytes in the header and data
    for(uiCounter = 1; uiCounter < uiLenghtByte; uiCounter++)
        uiCalcChkSum += (unsigned int)fpucMsg[uiCounter];

    //last two bytes of the message is the checksum
    uiSentChkSum = ((unsigned int)fpucMsg[uiCounter] << 8) | (unsigned int)fpucMsg[uiCounter+1];

//printf("sent=%x calc=%x len=%d\n",uiSentChkSum, uiCalcChkSum, uiLenghtByte);

    return(uiCalcChkSum == uiSentChkSum);
}
/****************************************************************************
 * Function Name:   S2PrepareSecsMsg
 * Description:     Function putds together the Secs message, adds the header, checksum, system
 *                  bytes and stores it in Secs structure for Com module to be able to retrieve
 *                  On the end it sets the corresponding status in Secs info structure
 * Parameter:       iPortNumArg - the number of com-port the message will be sent through
 *                  uiDevIDArg - ID of device to be adressed
 *                  ucStreamArg - Secs message Stream number
 *                  ucFunctionArg - Secs message Function number
 *                  ucEbitArg - Secs message E-bit 0 - all blocks, parts of multiblock message
 *                                                     have set it to 0
 *                                                 1 - last block in the series or only block
 *                  ucWbitArg - Secs message W-bit 0 - no replay from the client is expected
 *                                                 1 - replay from the client is expected
 *                  Note: we don't need the R-bit of Secs message, is always
 *
 *                  *fpucMsgBlockArg - pointer to the message string
 *                  uiMsgLenght -  Length of the message
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2PrepareSecsMsg(int iPortNumArg, unsigned int uiDevIDArg,
                     unsigned char ucStreamArg, unsigned char ucFunctionArg,
                     unsigned char ucEbitArg,unsigned char ucWbitArg,
                     unsigned char *fpucMsgBlockArg, unsigned int uiMsgLenght)
{
  int iPtrIndex;
  fpstSecsHeader fpstSecsMsgHeader;
  unsigned char *fpucMsgBlock;
  unsigned int uiChkSum = 0, uiCounter;
  
  if(uiMsgLenght > 245)
    return FAILURE;
  iPtrIndex = iPortNumArg - 1;
  if(!fpstSecsInfoCom[iPtrIndex])
    return FAILURE;
  
  fpucMsgBlock = fpstSecsInfoCom[iPtrIndex]->m_ucTMsgBlock;
  //    fpucMsgBlock[0] = (unsigned char)(uiMsgLenght + sizeof(stSecsHeader));
  fpucMsgBlock[0] = (unsigned char)(uiMsgLenght + 10);
  fpstSecsMsgHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
  fpstSecsMsgHeader->m_bUpperDevID = (uiDevIDArg & 0xFF00) >> 8;
  fpstSecsMsgHeader->m_bLowerDevID = (BYTE)(uiDevIDArg & 0x00FF);
  fpstSecsMsgHeader->m_bRbit = 0;
  fpstSecsMsgHeader->m_bWbit = ucWbitArg;
  fpstSecsMsgHeader->m_bStream = ucStreamArg;
  fpstSecsMsgHeader->m_bFunction = ucFunctionArg;
  fpstSecsMsgHeader->m_bEbit = ucEbitArg;
  fpstSecsMsgHeader->m_bUpperBlockNumber = 0; //Single block only
  fpstSecsMsgHeader->m_bLowerBlockNumber = 1;
  fpstSecsMsgHeader->m_uSystemBytesUnion.stSysBytes.um_bSystem1 = rand()%256;
  fpstSecsMsgHeader->m_uSystemBytesUnion.stSysBytes.um_bSystem2 = rand()%256;
  fpstSecsMsgHeader->m_uSystemBytesUnion.stSysBytes.um_bSystem3 = rand()%256;
  fpstSecsMsgHeader->m_uSystemBytesUnion.stSysBytes.um_bSystem4 = rand()%256;
  
  if(uiMsgLenght)
    {
      //        memcpy(fpucMsgBlock + sizeof(stSecsHeader) + 1, fpucMsgBlockArg, uiMsgLenght);
      memcpy(fpucMsgBlock + 11, fpucMsgBlockArg, uiMsgLenght);
    }
  for(uiCounter = 0; uiCounter < (unsigned int)fpucMsgBlock[0]; uiCounter++ )
    {
      uiChkSum += (unsigned int)fpucMsgBlock[uiCounter+1];
      printf("%x ",fpucMsgBlock[uiCounter+1]);
    }
  
  fpucMsgBlock[uiCounter+1] = (unsigned char)((uiChkSum & 0xFF00) >> 8);
  fpucMsgBlock[uiCounter+2] = (unsigned char)(uiChkSum & 0x00FF);
  
  fpstSecsInfoCom[iPtrIndex]->m_wWTCurrActivity = MSG_TO_SEND;
  fpstSecsInfoCom[iPtrIndex]->m_wLastError = OK;
  fpstSecsInfoCom[iPtrIndex]->m_iMsgSent = FALSE;
  printf("\nS2Prepare: COM%d S%dF%d len=%d\n", iPortNumArg, ucStreamArg, ucFunctionArg, uiMsgLenght);
  
  if(ucStreamArg==1 && ucFunctionArg==5)
    {
      fpstSecsInfoCom[iPtrIndex]->m_ulInxFsd = 0;
    }
  printf("Hung: %d hi=%x lo=%x\n", uiChkSum,fpucMsgBlock[uiCounter+1],fpucMsgBlock[uiCounter+2]);
  return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2InterpretSecsMsg
 * Description:     Function interprets the message from the received in Com module,
 *                  first it compares to the previous message, if it's same, diregards it
 *                  according to the standard, if different it analyse the message and
 *                  depending on the type (S?F?) it sets appropriate info structures
 * Parameter:       iPortNumArg - the number of com-port the message is interpreted from
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2InterpretSecsMsg(int iPortNumArg)
{
    //if it is COM1 we behave as device
    if(iPortNumArg == COM1)
    {
        return S2InterpretSecsMsgDevice(iPortNumArg);
    }
    else
    {
        int iPtrIndex;
        fpstSecsInfo fpstTempSecsInfo, fpstTempSecsCom, fpstTempSecsDev;
        fpstSecsHeader fpstSecsMsgHeader;
        unsigned char *fpucMsgBlock;
        unsigned int uiChkSum = 0, uiCounter;
        unsigned int uHeaderDevID;

        iPtrIndex = iPortNumArg - 1;
        fpstTempSecsInfo = fpstSecsInfoCom[iPtrIndex];
        if(!fpstTempSecsInfo)
            return FAILURE;

        fpucMsgBlock = fpstTempSecsInfo->m_ucMsgBlock;
        fpstSecsMsgHeader = (fpstSecsHeader)(fpucMsgBlock + 1);

        uHeaderDevID = ( ((WORD)fpstSecsMsgHeader->m_bUpperDevID<<8) | ((WORD)fpstSecsMsgHeader->m_bLowerDevID ));


    	fpstTempSecsCom = fpstSecsInfoCom[iPtrIndex];
    	fpstTempSecsDev = fpstSecsInfoDev[iPtrIndex];


        // We'll always use fpSI[0] for Query on Equipment Dev Id
/*        if (iQueryEquipmentDevId == FALSE)
        {
	    fpstTempSecsInfo = fpstTempSecsDev;

//printf("S2InterpretSecsMsg 2 fpstInfo=%x \n",fpstTempSecsInfo);
            fpstTempSecsInfo->m_wLastError = fpstTempSecsCom->m_wLastError;
            fpstTempSecsInfo->m_wWTCurrActivity = fpstTempSecsCom->m_wWTCurrActivity;
            fpstTempSecsInfo->m_wIBCurrActivity = fpstTempSecsCom->m_wIBCurrActivity;
            fpstTempSecsInfo->m_ulTimeOut = fpstTempSecsCom->m_ulTimeOut;
            fpstTempSecsInfo->m_wNofRty = fpstTempSecsCom->m_wNofRty;
            fpstTempSecsInfo->m_wMaxRetries = fpstTempSecsCom->m_wMaxRetries;
            fpstTempSecsInfo->m_iMsgSent = fpstTempSecsCom->m_iMsgSent;
//            memcpy(fpucMsgBlock, fpstTempSecsInfo->m_ucMsgBlock, 256);
            memcpy(fpstTempSecsInfo->m_ucMsgBlock, fpucMsgBlock, 256);
            fpucMsgBlock = fpstTempSecsInfo->m_ucMsgBlock;
            fpstSecsMsgHeader = (fpstSecsHeader)(fpucMsgBlock+1);
        }
*/
        // If prev !equals curr interprets
        if(memcmp( &(fpstTempSecsInfo->m_prevSH), fpstSecsMsgHeader, sizeof(stSecsHeader) ))
        {
//            fpucMsgBlock += sizeof(stSecsHeader)+1;
            fpucMsgBlock += 11;
//            if(MRGetMacroPC(MR_SECS_INTERRUPT) && uHeaderDevID == 868)
            if(MRGetMacroPC(MR_SECS_INTERRUPT) && iPortNumArg == COM3)
            {
		// trigger SECS2 macro call only if S6F13 with event 1 & 2
		if (fpstSecsMsgHeader->m_bStream == 6 &&
		    fpstSecsMsgHeader->m_bFunction == 13 &&
		    (	(unsigned int)fpucMsgBlock[7] == 1 || 
			(unsigned int) fpucMsgBlock[7] == 2))
		{
                    MASetMacroIntegerVars(MR_SECS_INTERRUPT, TRUE);
                    MRPreLaunchMacro(MRGetMacroPC(MR_SECS_INTERRUPT), (unsigned long)uHeaderDevID,
                                    (long)fpstSecsMsgHeader->m_bStream,
                                    (long)fpstSecsMsgHeader->m_bFunction, fpucMsgBlock);
		}
            }

//            if(MRGetMacroPC(MR_SECS2_INTERRUPT) && uHeaderDevID == 1124)
            if(MRGetMacroPC(MR_SECS2_INTERRUPT) && iPortNumArg == COM4)
            {
		// trigger SECS2 macro call only if S6F13 with event 1 & 2
		if (fpstSecsMsgHeader->m_bStream == 6 &&
		    fpstSecsMsgHeader->m_bFunction == 13 &&
		    (	(unsigned int)fpucMsgBlock[7] == 1 || 
			(unsigned int) fpucMsgBlock[7] == 2))
		{
                    MASetMacroIntegerVars(MR_SECS2_INTERRUPT, TRUE);
                    MRPreLaunchMacro(MRGetMacroPC(MR_SECS2_INTERRUPT), (unsigned long)uHeaderDevID,
                                    (long)fpstSecsMsgHeader->m_bStream,
                                    (long)fpstSecsMsgHeader->m_bFunction, fpucMsgBlock);
		}
            }

//            if(MRGetMacroPC(MR_SECS3_INTERRUPT) && uHeaderDevID == 1380)
            if(MRGetMacroPC(MR_SECS3_INTERRUPT) && iPortNumArg == COM5)
            {
		// trigger SECS2 macro call only if S6F13 with event 1 & 2
		if (fpstSecsMsgHeader->m_bStream == 6 &&
		    fpstSecsMsgHeader->m_bFunction == 13 &&
		    (	(unsigned int)fpucMsgBlock[7] == 1 || 
			(unsigned int) fpucMsgBlock[7] == 2))
		{
                    MASetMacroIntegerVars(MR_SECS3_INTERRUPT, TRUE);
                    MRPreLaunchMacro(MRGetMacroPC(MR_SECS3_INTERRUPT), (unsigned long)uHeaderDevID,
                                    (long)fpstSecsMsgHeader->m_bStream,
                                    (long)fpstSecsMsgHeader->m_bFunction, fpucMsgBlock);
		}
            }

//            if(MRGetMacroPC(MR_SECS4_INTERRUPT) && uHeaderDevID == 1636)
            if(MRGetMacroPC(MR_SECS4_INTERRUPT) && iPortNumArg == COM6)
            {
		// trigger SECS2 macro call only if S6F13 with event 1 & 2
		if (fpstSecsMsgHeader->m_bStream == 6 &&
		    fpstSecsMsgHeader->m_bFunction == 13 &&
		    (	(unsigned int)fpucMsgBlock[7] == 1 || 
			(unsigned int) fpucMsgBlock[7] == 2))
		{
                    MASetMacroIntegerVars(MR_SECS4_INTERRUPT, TRUE);
                    MRPreLaunchMacro(MRGetMacroPC(MR_SECS4_INTERRUPT), (unsigned long)uHeaderDevID,
                                    (long)fpstSecsMsgHeader->m_bStream,
                                    (long)fpstSecsMsgHeader->m_bFunction, fpucMsgBlock);
		}
            }

            switch(fpstSecsMsgHeader->m_bStream)
            {
                case 1:
                    if(fpstSecsMsgHeader->m_bFunction == 2)
                    {
                        fpstTempSecsInfo->m_wDevID = uHeaderDevID;
                        iQueryEquipmentDevId = FALSE;
                    }
                    else if(fpstSecsMsgHeader->m_bFunction == 6)
		    {
printf("S2Interpret: fpCom=%x S1F6 SFCD=%d\n",fpstTempSecsCom, fpucMsgBlock[4]);
                        if(fpucMsgBlock[4] == 2)
                        {       // Intercept SFCD = 2
                            uiChkSum = (unsigned int)fpucMsgBlock[6];
                            fpucMsgBlock += 7;
                            for(uiCounter = 0; uiCounter < uiChkSum; uiCounter++)
                            {
                                switch(fpucMsgBlock[uiCounter*3+2])
                                {
                                    case 1:
                                        fpstTempSecsInfo->m_cMap[uiCounter] = 1;
                                        break;
                                    case 2:
                                        fpstTempSecsInfo->m_cMap[uiCounter] = 0;
                                        break;
                                    case 3:
                                        fpstTempSecsInfo->m_cMap[uiCounter] = 3;
                                        break;
                                    case 4:
                                        fpstTempSecsInfo->m_cMap[uiCounter] = 2;
                                        break;
                                    default:
                                        fpstTempSecsInfo->m_cMap[uiCounter] = 4;
                                }
//printf("%d",fpstTempSecsInfo->m_cMap[uiCounter]);
                            }
                            fpstTempSecsInfo->m_wNewMap = TRUE;
//printf("\n",fpstTempSecsInfo->m_cMap[uiCounter]);
                        }
                        else if(fpucMsgBlock[4] == 0)
                        {       // Intercept SFCD = 0
                            fpstTempSecsInfo->m_wNewMap = TRUE;
                            fpstTempSecsInfo->m_ulInxFsd = (unsigned long)fpucMsgBlock[12];
                            fpstTempSecsInfo->m_ulInxFsd |= (unsigned long)fpucMsgBlock[15] << 1L;
                            fpstTempSecsInfo->m_ulInxFsd |= (unsigned long)fpucMsgBlock[18] << 2L;
                        }
//printf("S1F6: dev=%d hack=%d fp=%x\n",fpstTempSecsInfo->m_wDevID, fpstTempSecsInfo->m_ulInxFsd,fpstTempSecsInfo);
//printf("S1F6: fpInfo=%x hack=%d\n",fpstTempSecsInfo, fpstTempSecsInfo->m_ulInxFsd);
		    }
                    break;
                case 2:
printf("S2Interpret: fpCom=%x S2F42 ack=%d\n",fpstTempSecsCom, fpucMsgBlock[2]);
                    if(fpstSecsMsgHeader->m_bFunction == 42)
                        fpstTempSecsInfo->m_uiInxHcack = (unsigned int)fpucMsgBlock[2] & 0x00FF;
                    else if (fpstSecsMsgHeader->m_bFunction == 16)
                        fpstTempSecsInfo->m_uiInxEac = (unsigned int)(fpucMsgBlock[2]);
		    else if (fpstSecsMsgHeader->m_bFunction == 38)
			fpstTempSecsInfo->m_uiInxHcack = (unsigned int)fpucMsgBlock[2] & 0x00FF;

//printf("S2Fxx: fpInfo=%x hack=%d\n",fpstTempSecsInfo, fpstTempSecsInfo->m_uiInxHcack);

                    break;
                                            // Amatx
                case 4:                           // S4
                    if(fpstSecsMsgHeader->m_bFunction == 112)  // S4,F112
                    {
                        fpstFluoroTracTagDataArray[eS4F112Received]->m_wDevID = fpucMsgBlock[4];                      // Format: A9 Smart-Comm Dev Id
                        fpstFluoroTracTagDataArray[eS4F112Received]->m_wDevID |= (unsigned int)((fpucMsgBlock[5] << 8)&0xFF00);
                        fpstFluoroTracTagDataArray[eS4F112Received]->m_bTagLog = fpucMsgBlock[8];                               // Format: A5 bin number (1-255)
                        fpstFluoroTracTagDataArray[eS4F112Received]->m_bPageNumber = fpucMsgBlock[11];                          // Format: A5 'A' to 'P'
                        fpstFluoroTracTagDataArray[eS4F112Received]->m_bAck6 = fpucMsgBlock[14];                                // Format: A5 (0-3)
                        memcpy(fpstFluoroTracTagDataArray[eS4F112Received]->m_cFTTagData,  &fpucMsgBlock[17], TAG_DATA_SIZE); // Format: 41 ASCII
                    }
                    else
                        if (fpstSecsMsgHeader->m_bFunction == 114)  // S4,F114
                        {
                            fpstFluoroTracTagDataArray[eS4F114Received]->m_wDevID = fpucMsgBlock[4];          // Format: A9 Smart-Comm Dev Id
                            fpstFluoroTracTagDataArray[eS4F114Received]->m_wDevID |= (unsigned int)((fpucMsgBlock[5] << 8)&0xFF00);
                            fpstFluoroTracTagDataArray[eS4F114Received]->m_bTagLog = fpucMsgBlock[8];                   // Format: A5 bin number (1-255)
                            fpstFluoroTracTagDataArray[eS4F114Received]->m_bPageNumber = fpucMsgBlock[11];              // Format: A5 'A' to 'P'
                            fpstFluoroTracTagDataArray[eS4F114Received]->m_bAck6 = fpucMsgBlock[14];                    // Format: A5 (0-3)
                        }
                    break;
                case 5:
                    if(fpstSecsMsgHeader->m_bFunction == 1)
                    {
                        fpstTempSecsInfo->m_uiInxAlid = (unsigned int)fpucMsgBlock[7];  // ALID
                        fpstTempSecsInfo->m_uiInxAlid &= 0x00FF;   // Strips off upper byte, space for ALCD (special request from AMAT)
                        fpstTempSecsInfo->m_uiInxAlid |= (unsigned int)(fpucMsgBlock[4] << 8);  // ALCD
printf("S2Interpret: fpCom=%x S5F1 alid=%d\n",fpstTempSecsInfo, fpstTempSecsInfo->m_uiInxAlid);
                    }
                    break;
                case 6:
                    if(fpstSecsMsgHeader->m_bFunction == 13)
                        fpstTempSecsInfo->m_uiInxCeid = (unsigned int)fpucMsgBlock[7] & 0x00FF;
printf("S2Interpret: fpCom=%x S6F13 ceid=%d\n",fpstTempSecsInfo, fpstTempSecsInfo->m_uiInxCeid);
                    break;
            }
        memcpy( &(fpstTempSecsInfo->m_prevSH), fpstSecsMsgHeader, sizeof(stSecsHeader) );
        }
        return SUCCESS;
    }
}
/****************************************************************************
 * Function Name:   S2SecsCommand
 * Description:
 * Parameter:       iPortNumArg - the number of com-port the message will be sent through
 *                  uiDevIDArg - ID of device to be adressed
 *                  ucStreamArg - Secs message Stream number
 *                  ucFunctionArg - Secs message Function number
 *                  iParameterArg - specific parameter to distinguis diferent flavors of the same S?F?
 *                  ipRegisterNumArg - [R?] to read from input or write to output for lower S?F? functions
 *                  pcStringArg - String for lower S?F? functions passed from command line
 *                  ipSetPCInstrArg - this argument indicates to calling function if PC has to be set
 *                          to current instruction ( to repeat instruction)
 *                  ipcPCWaitingArg - argument indicates to calling function whether PCWaiting
 *                          variable needs to be set
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2SecsCommand(int iPortNumArg, unsigned int uDevIDArg, int iStreamArg, int iFunctionArg, int iParameterArg,
                    int *ipRegisterNumArg, char *pcStringArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    int iCounter, iReturnValue;
    // Amatx
    fpstSecsInfo  fpstTempSecsInfoCom, fpstTempSecsInfoDev;
    unsigned int uFluoroTracDevID;

    if( !(fpstTempSecsInfoCom = S2GetSecsInfoCom(iPortNumArg)) )
        return FAILURE;

    fpstTempSecsInfoDev = fpstSecsInfoDev[iPortNumArg-1];


    // Amatx, Make both COM2 & COM 3 handle Multiple Dev IDs
    // Command 11 & 13 use only SmartComm DEV ID
    if(iStreamArg == 4 && (iFunctionArg == 111 || iFunctionArg == 113))
    {
        uFluoroTracDevID = uDevIDArg;
        uDevIDArg &= 0x00FF;                  // mask off the bits 8 - 11
    }
    else
        uFluoroTracDevID = 0;

    if(fpstTempSecsInfoDev->m_wDevID != uDevIDArg)
    {
	fpstTempSecsInfoDev->m_wDevID = uDevIDArg;
    }


    if((iParameterArg == 200) || (iParameterArg == 201) || (iParameterArg == 202))
    {
        //Wait function, Generic send and generic read
        if(iParameterArg == 201)
        {
            ipRegisterNumArg[2] = iStreamArg;
            ipRegisterNumArg[3] = iFunctionArg;
        }
//printf("S2SecsCommand 202 R1=%d R2=%d R3=%d R4=%d\n",ipRegisterNumArg[0],ipRegisterNumArg[1],ipRegisterNumArg[2],ipRegisterNumArg[3]);
        iReturnValue = S2HostSxFx(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg, uDevIDArg,
                iParameterArg, ipRegisterNumArg[0], ipRegisterNumArg[1], ipRegisterNumArg[2],
                ipRegisterNumArg[3], pcStringArg, ipSetPCInstrArg, ipPCWaitingArg);
    }
    else
    {
        switch(iStreamArg)
        {
            case 1 :
                switch(iFunctionArg)
                {
                    case 1 : //Command
                        iReturnValue = S2HostS1F1(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                                uDevIDArg, iParameterArg, ipSetPCInstrArg, ipPCWaitingArg);
//printf("S2SecsCommand S2S1F1 fpCom=%x fpDev=%x\n",fpstTempSecsInfoCom, fpstTempSecsInfoDev);
                        break;
                    case 2 : //Replay
//                        iReturnValue = S2HostS1F2(fpstTempSecsInfoDev, ipRegisterNumArg[0]);
                        iReturnValue = S2HostS1F2(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
//printf("S2SecsCommand S1F2 sent iRet=%d SetPC=%d PCWait=%d\n",iReturnValue, *ipSetPCInstrArg, *ipPCWaitingArg);
//printf("S2SecsCommand S2S1F2 fpCom=%x\n",fpstTempSecsInfoCom);
                        break;
                    case 5 : //Command
                        iReturnValue = S2HostS1F5(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                uDevIDArg, iParameterArg, ipSetPCInstrArg, ipPCWaitingArg);
                        break;
                    case 6 : //Replay
                        iReturnValue = S2HostS1F6(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iParameterArg,
                                                    ipRegisterNumArg[0], ipRegisterNumArg[1], ipRegisterNumArg[2]);
                        break;
                }
                break;
            case 2 :
                switch(iFunctionArg)
                {
		    case 13 : // Request EC
			iReturnValue = S2HostS2F13(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
						uDevIDArg, iParameterArg, ipSetPCInstrArg, ipPCWaitingArg);
			break;
                    case 15 : //Send command
                        iReturnValue = S2HostS2F15(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                            uDevIDArg, ipSetPCInstrArg, ipPCWaitingArg);
//printf("S2SecsCommand S2F15 sent iRet=%d SetPC=%d PCWait=%d\n",iReturnValue, *ipSetPCInstrArg, *ipPCWaitingArg);
                        break;
                    case 16 :  //Read replay
                        iReturnValue = S2HostS2F16(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
                        break;

		    case 37:  // enable all events
			iReturnValue = S2HostS2F37(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg, uDevIDArg, iParameterArg, ipSetPCInstrArg, ipPCWaitingArg);
			break;
		    case 38:  // Read replay
			iReturnValue = S2HostS2F38(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
			break;

                    case 41 : //Remote command
                        iReturnValue = S2HostS2F41(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                            uDevIDArg, iParameterArg, ipRegisterNumArg[0],
                                            ipSetPCInstrArg, ipPCWaitingArg);
                        break;
                    case 42 :
//                        iReturnValue = S2HostS2F42(fpstTempSecsInfoDev, ipRegisterNumArg[0]);
                        iReturnValue = S2HostS2F42(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
//printf("fpCom=%x hcack=%d\n",fpstTempSecsInfoCom, fpstTempSecsInfoCom->m_uiInxHcack);

                        break;
                }
                break;
            case 4 :
                switch(iFunctionArg)
                {
                    case 111 : //Send command
                        iReturnValue = S2HostS4F111(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                        uDevIDArg, ipRegisterNumArg[0], ipRegisterNumArg[1], uFluoroTracDevID,
                                        ipSetPCInstrArg, ipPCWaitingArg);
                        break;
                    case 112 :  //Read replay
                        iReturnValue = S2HostS4F112(ipRegisterNumArg[0], ipRegisterNumArg[1]);
                        break;
                    case 113 : //Send command
                        iReturnValue = S2HostS4F113(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                            uDevIDArg, ipRegisterNumArg[0], ipRegisterNumArg[1], pcStringArg,
                                            uFluoroTracDevID, ipSetPCInstrArg, ipPCWaitingArg);
                        break;
                    case 114 : //Read replay
                        iReturnValue = S2HostS4F114(ipRegisterNumArg[0]);
                        break;
                }
                break;
            case 5 :
                switch(iFunctionArg)
                {
                    case 1 : //Read replay
                        iReturnValue = S2HostS5F1(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
//                        iReturnValue = S2HostS5F1(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
                        break;
                }
                break;
            case 6 :
                switch(iFunctionArg)
                {
                    case 4 : //ack to S6F3
                        iReturnValue = S2HostS6F4(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                                uDevIDArg, iParameterArg, ipSetPCInstrArg, ipPCWaitingArg);
                        break;
                    case 12 : //ack to S6F11
                        iReturnValue = S2HostS6F12(fpstTempSecsInfoCom, fpstTempSecsInfoCom, iPortNumArg,
                                                uDevIDArg, iParameterArg, ipSetPCInstrArg, ipPCWaitingArg);
                        break;
                    case 13 : //Read replay
                        iReturnValue = S2HostS6F13(fpstTempSecsInfoCom, ipRegisterNumArg[0], iParameterArg);
//                        iReturnValue = S2HostS6F13(fpstTempSecsInfoCom, ipRegisterNumArg[0]);
                        break;
                }
                break;
            default :
                return FAILURE;
        }
    }

//printf("S2SecsCommand S%dF%d iSetPC=%d PCWait=%d\n", iStreamArg, iFunctionArg, *ipSetPCInstrArg, *ipPCWaitingArg);
    return iReturnValue;
}

int S2GetSecsSpecial()
{
    return iSecsSpecialFlag;
}

void S2SetSecsSpecial(int iFlagArg)
{
    iSecsSpecialFlag = iFlagArg;
}


int SERSetSECSStatus(int iPortNumArg, int iCurrActivityStatArg, int iLastErrorStatArg, char cAckNakStatArg, int iGetStatArg);

int S2SecsReset(int iPortNumArg, unsigned int uDevIDArg, int iStreamArg, int iFunctionArg)
{
    int iCounter, iReturnValue;

    fpstSecsInfo  fpstTempSecsInfoCom, fpstTempSecsInfoDev;

    if( !(fpstTempSecsInfoCom = S2GetSecsInfoCom(iPortNumArg)) )
        return FAILURE;


    if(iStreamArg != 313 && iFunctionArg != 313)
        return FAILURE;

    SERSetSECSStatus(iPortNumArg, IDLE, OK, NAK, TRUE);

    return SUCCESS;
}


int S2SecsDump(int iPortNumArg)
{
    int iCounter, iReturnValue;

    fpstSecsInfo  pCom;

    if( !(pCom = S2GetSecsInfoCom(iPortNumArg)) )
        return FAILURE;
/*
    WORD     m_wWTCurrActivity, m_wIBCurrActivity;
    WORD     m_wTimerID, m_wNofRty;
    WORD     m_wLastError;
    WORD     m_wT1;
    WORD     m_wT2;
    WORD     m_wT3;
    WORD     m_wT4;
    WORD     m_wT5;
    WORD     m_wT6;
    WORD     m_wMaxRetries;
    unsigned long m_ulTimeOut;
    unsigned char m_ucMsgBlock[256];
    unsigned char m_ucTMsgBlock[256];
    stSecsHeader m_prevSH;
    unsigned int m_uiInxAlid;
    unsigned int m_uiInxHcack;
    unsigned int m_uiInxCeid;
    unsigned long m_ulInxFsd;
    unsigned int  m_uiInxEac;
    WORD     m_wDevID, m_wNewMap;
    char     m_cMap[26];
    int      m_iMsgSent;
*/
    printf("pCom=%x, WTCurr=%d IBCurr=%d lastErr=%d timeOut=%d\n",pCom,pCom->m_wWTCurrActivity,pCom->m_wIBCurrActivity,pCom->m_wLastError,pCom->m_ulTimeOut);
    printf("S%dF%d, Alid=%d Hcack=%d Ceid=%d Fsd=%d\n",pCom->m_prevSH.m_bStream,pCom->m_prevSH.m_bFunction,pCom->m_uiInxAlid,pCom->m_uiInxHcack,pCom->m_uiInxCeid,pCom->m_ulInxFsd);
    printf("Eac=%d, msgSent=%d maxRty=%d NofRty=%d\n",pCom->m_uiInxEac,pCom->m_iMsgSent,pCom->m_wMaxRetries,pCom->m_wNofRty);


    return SUCCESS;
}
