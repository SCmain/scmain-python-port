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
* Program:      File contains those of functions needed for SECS-II com when our controller is a
*               device
* File:         secssxdev.c
* Functions:
*
* Description:
*
* Modification history:
* Rev      ECO#    Date    Author      Brief Description
*
************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "sck.h"
#include "secsl.h"
#include "secsg.h"
#include "scregg.h"

// For ToolS2F67
int iCMDA1 = 0;
int iCEID1 = 0;
int iPORT1 = 0;
int iSLOT1 = 0;

extern int iProcessingS2F65;

stDim stDimTable[] =
{
    SECS_MDLN,      SECS_ASCII, 1, 6,
    SECS_SOFTREV,   SECS_ASCII, 1, 6,
    SECS_ECID,      SECS_INT2,  1, 2,
    SECS_ECV,       SECS_INT4,  1, 4,
    SECS_EAC,       SECS_BINARY,1, 1,
    SECS_RCMD,      SECS_INT2,  1, 2,
    SECS_PORT,      SECS_INT2,  1, 2,
    SECS_FPORT,     SECS_INT2,  1, 2,
    SECS_TPORT,     SECS_INT2,  1, 2,
    SECS_ANGLE,     SECS_INT2,  1, 2,
    SECS_FSLOTS,    SECS_BINARY,1, 30,
    SECS_TSLOTS,    SECS_BINARY,1, 30,
    SECS_FSLOT,     SECS_INT2,  1, 2,
    SECS_WAFSTAT,   SECS_INT2,  1, 2,
    SECS_TSLOT,     SECS_INT2,  1, 2,
    SECS_ORG_PORT,  SECS_INT2,  1, 2,
    SECS_ORG_SLOT,  SECS_INT2,  1, 2,
    SECS_CMDA,      SECS_INT1,  1, 1,
    SECS_RCMD1,     SECS_INT2,  1, 2,
    SECS_LLK,       SECS_INT2,  1, 2,
    SECS_CMDA1,     SECS_INT1,  1, 1,
    SECS_ALCD,      SECS_BINARY,1, 1,
    SECS_ALID,      SECS_INT4,  1, 4,
    SECS_ALTX,      SECS_ASCII, 1, 0,
    SECS_STATUS,    SECS_INT1,  1, 1,
    SECS_CASSETTE,  SECS_INT1,  1, 1,
    SECS_SLOTS,     SECS_BINARY,1, 30,
    SECS_ORGPORT,   SECS_INT1,  1, 1,
    SECS_ORGSLOT,   SECS_INT1,  1, 1,
    SECS_WAFSTAT1,  SECS_INT1,  1, 1,
    SECS_CEID,      SECS_INT2,  1, 2,
    SECS_SLOT,      SECS_INT2,  1, 2,
    SECS_LDST,      SECS_INT2,  1, 2,
    SECS_WFST,      SECS_INT2,  1, 2,
    SECS_FRPT,      SECS_INT2,  1, 2,
    SECS_TOPT,      SECS_INT2,  1, 2,
    SECS_SPARE,     SECS_INT2,  1, 2,
};

int S2DeviceS2F67FromController(int iPortNumArg, int *ipParametersArg);

/*****************************************************************************
 * Function Name:   S2CreateMessageItem
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2CreateMessageItem(unsigned char *pucReturnBufferArg, unsigned uTypeArg, int iNumOfLengthBytesArg,
                        unsigned uDataLengthArg, unsigned char *pucPureDataArg)
{
    int iByteUsed = 0;
    unsigned int uiCounter;
    unsigned char *pucTemp;

    // Assume always length byte is 1 byte long
    pucReturnBufferArg[0] = (unsigned char)(uTypeArg | (0x03 & iNumOfLengthBytesArg));
    pucReturnBufferArg[1] = uDataLengthArg & 0x00FF;
    iByteUsed = 2 + uDataLengthArg;
    pucTemp = pucReturnBufferArg+2;

    switch(uTypeArg)
    {
        case SECS_LIST:
            iByteUsed = 2;
            break;
        case SECS_ASCII:
        case SECS_BINARY:
            memcpy(pucTemp, pucPureDataArg, (size_t)uDataLengthArg);
            iByteUsed = uDataLengthArg + 2;
            break;
        case SECS_INT1:
        case SECS_UINT1:
            for(uiCounter = 0; uiCounter < uDataLengthArg; uiCounter++)
                pucTemp[uiCounter] = pucPureDataArg[uiCounter];             // MSB
            break;
        case SECS_INT2:
        case SECS_UINT2:
            for(uiCounter = 0; uiCounter < uDataLengthArg; uiCounter += 2)
            {
                pucTemp[uiCounter+1] = pucPureDataArg[uiCounter];           // LSB
                pucTemp[uiCounter] = pucPureDataArg[uiCounter+1];           // MSB
            }
            break;
        case SECS_INT4:
        case SECS_UINT4:
            for(uiCounter = 0; uiCounter < uDataLengthArg; uiCounter += 4)
            {
                pucTemp[uiCounter] = pucPureDataArg[uiCounter+3];           // MSB
                pucTemp[uiCounter+1] = pucPureDataArg[uiCounter+2];
                pucTemp[uiCounter+2] = pucPureDataArg[uiCounter+1];
                pucTemp[uiCounter+3] = pucPureDataArg[uiCounter];           // LSB
            }
            break;
        case SECS_FLOAT8:
        case SECS_FLOAT4:
        case SECS_UINT8:
        case SECS_BOOLEAN:
        case SECS_JIS8:
        case SECS_INT8:
        default:
            break;
    }
    return iByteUsed;
}
/*****************************************************************************
 * Function Name:   S2GetMessageItem
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2GetMessageItem(unsigned char *pucDataArg, unsigned uTypeArg, int iNumOfLengthBytesArg,
                                        unsigned uDataLengthArg, unsigned char *pucReturnBufferArg)
{
    int iByteUsed = 0;
    unsigned int uiCounter,uiNumOfLenBytes;
    unsigned long int ulDataLength = 0;
    unsigned char *pucTemp;

    if((unsigned char)(pucDataArg[0] & 0xFC) != (unsigned char)uTypeArg)
	{
        if(uTypeArg != (unsigned)SECS_PASS_THROUGH)
            return SUCCESS;
	}

    uiNumOfLenBytes = (unsigned int)(pucDataArg[0] & 0x03);
    pucTemp = (unsigned char *)&ulDataLength;

    for(uiCounter = 0; uiCounter < uiNumOfLenBytes; uiCounter++)
        pucTemp[uiCounter] = pucDataArg[uiNumOfLenBytes-uiCounter];

    if(uDataLengthArg != ulDataLength)
    {
        if((uTypeArg != (unsigned)SECS_LIST ) && (uTypeArg != (unsigned)SECS_PASS_THROUGH))
            return SUCCESS;
    }
    if(uTypeArg == (unsigned)SECS_PASS_THROUGH)
        uTypeArg = (unsigned)SECS_LIST;

    iByteUsed = uiNumOfLenBytes + 1 + uDataLengthArg;
    pucTemp = pucDataArg + uiNumOfLenBytes + 1;

    switch(uTypeArg)
    {
        case SECS_LIST:
            iByteUsed = uiNumOfLenBytes + 1;
            pucReturnBufferArg[0] = pucDataArg[1];
            pucReturnBufferArg[1] = 0;
            break;
        case SECS_ASCII:
        case SECS_BINARY:
            memcpy(pucReturnBufferArg, pucTemp, (size_t)uDataLengthArg);
            iByteUsed = uDataLengthArg + uiNumOfLenBytes + 1;
            break;
        case SECS_INT1:
        case SECS_UINT1:
            for(uiCounter = 0; uiCounter < uDataLengthArg; uiCounter++)
                pucReturnBufferArg[uiCounter] = pucTemp[uiCounter];      // MSB
            break;
        case SECS_INT2:
        case SECS_UINT2:
            for(uiCounter = 0; uiCounter < uDataLengthArg; uiCounter += 2)
            {
                pucReturnBufferArg[uiCounter] = pucTemp[uiCounter+1];     // LSB
                pucReturnBufferArg[uiCounter+1] = pucTemp[uiCounter];     // MSB
            }
            break;
        case SECS_INT4:
        case SECS_UINT4:
            for(uiCounter = 0; uiCounter < uDataLengthArg; uiCounter += 4)
            {
                pucReturnBufferArg[uiCounter] = pucTemp[uiCounter+3];     // LSB
                pucReturnBufferArg[uiCounter+1] = pucTemp[uiCounter+2];
                pucReturnBufferArg[uiCounter+2] = pucTemp[uiCounter+1];
                pucReturnBufferArg[uiCounter+3] = pucTemp[uiCounter];     // MSB
            }
            break;
        case SECS_FLOAT8:
        case SECS_FLOAT4:
        case SECS_UINT8:
        case SECS_BOOLEAN:
        case SECS_JIS8:
        case SECS_INT8:
        default:
            break;
    }
    return iByteUsed;
}
/*****************************************************************************
 * Function Name:   S2ReadDimensionTable
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2ReadDimensionTable(unsigned char *pucReturnBufferArg, enumTypeDim eDim, unsigned char *pucBufferArg )
{
    //FIXME what is this here for?
    //unsigned char ucDimType;
    int iRetValue;

    if(eDim < SECS_MDLN || eDim > SECS_SPARE)
        return FAILURE;

    //ucDimType = (unsigned char)(DIM_table[DIM].SECSTYPE|DIM_table[DIM].num_len_byte);

    iRetValue = S2GetMessageItem(pucReturnBufferArg,stDimTable[eDim].ucSecsType, stDimTable[eDim].iNumOfLengthBytes,
           stDimTable[eDim].iDataLength, pucBufferArg) ;

    return iRetValue;
}
/*****************************************************************************
 * Function Name:   S2CreateCommand
 * Parameters:
 * Returns:
 ****************************************************************************/
void S2CreateCommand(char *pcReturnString, char cFStn, int iFSlot, int iWafStat, char cTStn, int iTSlot,
                            int iAngle, int iOrgPort, int iOrgSlot)
{
    char caTempString[10];

    if(cFStn >= 'A' && cFStn <= 'D')
    {
        if(cTStn >= 'A' && cTStn <= 'D')                        // Port to Port
            strcpy(caTempString, "MPTP");
        else if(cTStn == 'E' || cTStn == 'F')                   // Port to LL
            strcpy(caTempString, "MPTL");
        else if(cTStn == 'R')                                   // Port to Rbt
            strcpy( caTempString, "MPTR");
        else if(cTStn == 'Z')                                   // Port to Orienter
            strcpy(caTempString, "MPTO");
        else
            strcpy(caTempString, "BAD");
    }
    else if(cFStn == 'E' || cFStn == 'F')
    {
        if(cTStn >= 'A' && cTStn <= 'D')                        // LL to Port
            strcpy(caTempString, "MLTP");
        else if(cTStn == 'E' || cTStn == 'F')                   // LL to LL
            strcpy(caTempString, "MLTL");
        else if(cTStn == 'R')                                   // LL to Rbt
            strcpy(caTempString, "MLTR");
        else if(cTStn == 'Z')                                   // LL to Orienter
            strcpy(caTempString, "MLTO");
        else
            strcpy(caTempString, "BAD");
    }
    else if(cFStn == 'R')
    {
        if(cTStn >= 'A' && cTStn <= 'D')                      // Rbt to Port
            strcpy(caTempString, "MRTP");
        else if(cTStn == 'E' || cTStn == 'F')                 // Rbt to LL
            strcpy(caTempString, "MRTL");
        else if(cTStn == 'R')                                // Rbt to Rbt
            strcpy(caTempString, "BAD");
        else if( cTStn == 'Z' )                              // Rbt to Orienter
            strcpy(caTempString, "MRTO");
        else
            strcpy(caTempString, "BAD");
    }
    else if(cFStn == 'Z')
    {
        if(cTStn >= 'A' && cTStn <= 'D')                      // Orienter to Port
            strcpy(caTempString, "MOTP");
        else if(cTStn == 'E' || cTStn == 'F')                 // Orienter to LL
            strcpy(caTempString, "MOTL");
        else if(cTStn == 'R' )                               // Orienter to Rbt
            strcpy(caTempString, "MOTR");
        else if(cTStn == 'Z' )                               // Orienter to Orienter
            strcpy(caTempString, "BAD");
        else
            strcpy(caTempString, "BAD");
    }
    else
        strcpy(caTempString, "BAD");

    sprintf(pcReturnString, "%s %c,%d,%d,%c,%d,%d,%d,%d\r", caTempString, cFStn, iFSlot, iWafStat,
                                                cTStn, iTSlot, iAngle, iOrgPort, iOrgSlot);
}
/*****************************************************************************
 * Function Name:   S2ReadStatusAfterS2F67
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2ReadStatusAfterS2F67(int iIndexArg)
{
    switch(iIndexArg)
    {
        case SECS_CMDA1:
            return iCMDA1;
        case SECS_CEID:
            return iCEID1;
        case SECS_PORT:
            return iPORT1;
        case SECS_SLOT:
            return iSLOT1;
        default:
            break;
    }
    return FAILURE;
}
/****************************************************************************
 * Function Name:   S2SecsDeviceCommand
 * Description:
 * Parameter:       iPortNumArg - the number of com-port the message will be sent through
 *                  uiDevIDArg - ID of device to be adressed
 *                  ucStreamArg - Secs message Stream number
 *                  ucFunctionArg - Secs message Function number
 *                  ipParametersArg - parameters to send through SxFx or registers to receive response to
 *                  ipSetPCInstrArg - this argument indicates to calling function if PC has to be set
 *                          to current instruction ( to repeat instruction)
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2SecsDeviceCommand(int iPortNumArg, unsigned int uDevIDArg, int iStreamArg, int iFunctionArg, int *ipParametersArg,
                    int *iSetPCInstrArg)
{
    int iRetValue = 0, iS2F67Return;
    fpstSecsInfo fpstTempSecsInfoCom;

    if(!(fpstTempSecsInfoCom = S2GetSecsInfoCom(iPortNumArg)))
        return FAILURE;

    switch(iStreamArg)
    {
        case 1 :
            switch(iFunctionArg)
            {
                case 1:
                    iRetValue = S2DeviceS1F1FromController(iPortNumArg);
                    break;
                default:
                    return FAILURE;
            }
            break;
        case 2 :
            switch(iFunctionArg)
            {
                case 66 :
                    iRetValue = S2DeviceS2F66FromController(iPortNumArg, ipParametersArg);
                    break;
                case 67 :
                    iRetValue = S2DeviceS2F67FromController(iPortNumArg, ipParametersArg);
                    break;
                default:
                    return FAILURE;
            }
            break;
        case 5 :
            switch(iFunctionArg)
            {
                case 1:
                    iRetValue = S2DeviceS5F1FromController(iPortNumArg, ipParametersArg);
                    break;
                default:
                    return FAILURE;
            }
            break;
        case 6:
            switch(iFunctionArg)
            {
                case 65 :
                    iRetValue = S2DeviceS6F65FromController(iPortNumArg);
                    break;
                case 69 :
                    iRetValue = S2DeviceS6F69FromController(iPortNumArg, ipParametersArg);
                    break;
                case 71 :
                    iRetValue = S2DeviceS6F71FromController(iPortNumArg, ipParametersArg);
                    break;
                default:
                    return FAILURE;
            }
            break;
        // Streams > 100 just wait for respond message
        case 102:
            switch(iFunctionArg)
            {
                case 68 :
                    if((iS2F67Return = S2ReadStatusAfterS2F67(SECS_CMDA1)) == WAITING_S2F68)
                        *iSetPCInstrArg = TRUE;
                    else
                        RGSetRegister(ipParametersArg[0], (long)iS2F67Return);
                    break;
                default:
                    return FAILURE;
            }
            break;
        case 106:
            switch(iFunctionArg)
            {
                case 67 :
                    if((iS2F67Return = S2ReadStatusAfterS2F67(SECS_CEID)) == WAITING_CEID1)
                        *iSetPCInstrArg = TRUE;
                    else
                    {
                        RGSetRegister(ipParametersArg[0], (long)iS2F67Return);
                        if(ipParametersArg[1] != 0)
                            RGSetRegister(ipParametersArg[1], (long)S2ReadStatusAfterS2F67(SECS_PORT));
                        if(ipParametersArg[2] != 0)
                            RGSetRegister(ipParametersArg[2], (long)S2ReadStatusAfterS2F67(SECS_SLOT));
                    }
                    iRetValue = SUCCESS;
                    break;
                default:
                    return FAILURE;
            }
        case 200:
            if(fpstTempSecsInfoCom->m_wWTCurrActivity != IDLE)
            {
                *iSetPCInstrArg = TRUE;
                return SUCCESS;
            }
            break;
        default:
            return FAILURE;
    }

    return iRetValue;
}
/****************************************************************************
 * Function Name:   S2InterpretSecsMesgDevice
 * Description:
 * Parameter:
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2InterpretSecsMsgDevice(int iPortNumArg)
{
    fpstSecsInfo fpstTempSecsInfo;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock;
    unsigned int uHeaderDevID;

    if(!(fpstTempSecsInfo = S2GetSecsInfoCom(iPortNumArg)))
        return FAILURE;
    fpucMsgBlock = fpstTempSecsInfo->m_ucMsgBlock;
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);

    // Interpret only if prev header non-equal current header
    if(memcmp(&(fpstTempSecsInfo->m_prevSH), fpstTempSecsHeader, sizeof(stSecsHeader)))
    {
//        TIRefreshWD();
        uHeaderDevID = (((WORD)fpstTempSecsHeader->m_bUpperDevID<<8) | ((WORD)fpstTempSecsHeader->m_bLowerDevID));
        // don't send the response if DevId in the Header of received message doesn't match our DevID,
        // unless the message is S1F1, then we set our DevID to the one received in the message
        if((fpstTempSecsInfo->m_wDevID != uHeaderDevID) &&
                !(fpstTempSecsHeader->m_bStream == 1 && fpstTempSecsHeader->m_bFunction == 1))
            return FAILURE;

        fpucMsgBlock += sizeof(stSecsHeader) + 1;

        switch(fpstTempSecsHeader->m_bStream)
        {
            case 1:
                switch(fpstTempSecsHeader->m_bFunction)
                {
                    case 1 :
                        S2DeviceS1F1FromHost(iPortNumArg);
                        break;
                    case 2 :
                        S2DeviceS1F2FromHost();
                        break;
                    default:
                        return FAILURE;
                }
            case 2:
                switch(fpstTempSecsHeader->m_bFunction)
                {
                    case 13 :
                        S2DeviceS2F13FromHost(iPortNumArg);
                        break;
                    case 15 :
                        S2DeviceS2F15FromHost(iPortNumArg);
                        break;
                    case 41 :
                        S2DeviceS2F41FromHost(iPortNumArg);
                        break;
                    case 65 :
                        S2DeviceS2F65FromHost(iPortNumArg);
                        break;
                    case 68 :
                        S2DeviceS2F68FromHost(iPortNumArg);
                        break;
                    default:
                        return FAILURE;
                }
            case 6:
                switch(fpstTempSecsHeader->m_bFunction)
                {
                    case 67 :
                        S2DeviceS6F67FromHost(iPortNumArg);
                        break;
                    default:
                        return FAILURE;
                }
            default:
                return FAILURE;
        }
        memcpy(&(fpstTempSecsInfo->m_prevSH), fpstTempSecsHeader, sizeof(stSecsHeader));
    }

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2PrepareSecsMsgDevice
 * Description:
 * Parameter:
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2PrepareSecsMsgDevice(int iPortNumArg, unsigned char ucStreamArg, unsigned char ucFunctionArg,
            unsigned char ucEbitArg, unsigned char ucWbitArg, unsigned char *fpucMsgBlockArg,
            unsigned int uiMsgLenghtArg, unsigned long lSystemBytesArg)
{
    fpstSecsInfo fpstTempSecsInfo;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock;
    unsigned int uiChkSum = 0, iCounter;
    WORD uDevID;

    if(!(fpstTempSecsInfo = S2GetSecsInfoCom(iPortNumArg)))
        return FAILURE;
    if(fpstTempSecsInfo->m_wWTCurrActivity != IDLE)
        return WAIT_SEND;

    S2GetTMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpucMsgBlock[0] = (unsigned char)(uiMsgLenghtArg + sizeof(stSecsHeader));
    uDevID = fpstTempSecsInfo->m_wDevID;

    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);

    fpstTempSecsHeader->m_bUpperDevID = (uDevID&0xFF00) >> 8;
    fpstTempSecsHeader->m_bLowerDevID = (BYTE)(uDevID & 0x00FF);
    fpstTempSecsHeader->m_bRbit = 1;
    fpstTempSecsHeader->m_bWbit = ucWbitArg;
    fpstTempSecsHeader->m_bStream = ucStreamArg;
    fpstTempSecsHeader->m_bFunction = ucFunctionArg;
    fpstTempSecsHeader->m_bEbit = ucEbitArg;
    fpstTempSecsHeader->m_bUpperBlockNumber = 0;        // Single block only
    fpstTempSecsHeader->m_bLowerBlockNumber = 1;
    if(ucWbitArg )
        (fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes)++;
    else
        fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes = lSystemBytesArg;

    if(uiMsgLenghtArg)
        memcpy(fpucMsgBlock + sizeof(stSecsHeader) + 1, fpucMsgBlockArg, uiMsgLenghtArg);

    for(iCounter = 0; iCounter < (unsigned int)fpucMsgBlock[0]; iCounter++)
        uiChkSum += (unsigned int)fpucMsgBlock[iCounter + 1];

    fpucMsgBlock[iCounter + 1] = (unsigned char)((uiChkSum & 0xFF00) >> 8);
    fpucMsgBlock[iCounter + 2] = (unsigned char)(uiChkSum & 0x00FF);

    fpstTempSecsInfo->m_wWTCurrActivity = MSG_TO_SEND;
    fpstTempSecsInfo->m_wLastError = OK;
    fpstTempSecsInfo->m_wNofRty = 0;
    fpstTempSecsInfo->m_iMsgSent = FALSE;

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   S2PutTogetherS2F66Response
 * Description:
 * Parameter:
 * Returns:         SUCCESS/FALURE
 ****************************************************************************/
int S2PutTogetherS2F66Response(int iPortNumArg, char *caStrArg)
{
    unsigned char *fpucMsgBlock, *pucTemp = 0;
    unsigned char caSecsMsgData[50];
    int iRetValue, iBufferStringLength = 0;
    unsigned int uiBytes;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char ucL = 2, ucCMDA;

    // Don't send any garbage from the COM transmit buffer if we are not explicitly waiting for S2F66
    if(iProcessingS2F65 != TRUE)
        return SUCCESS;
    else
        iProcessingS2F65 = FALSE;

    pucTemp = caSecsMsgData;
    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);

    iBufferStringLength = strlen(caStrArg);

    // if processing of the command failed (there is r/n/? in the buffer), return the CMDA = 7
    if(iBufferStringLength == 0)
    {
        iProcessingS2F65 = TRUE;
        return SUCCESS;
    }
    else if(caStrArg[iBufferStringLength - 1] == '?')
    {
        ucCMDA = 7;
        uiBytes = S2CreateMessageItem(pucTemp, SECS_INT1, 1, 1,&ucCMDA);
    }
    // if command was accepted and there is no response from the controller (r/n/> in the buffer), return CMDA = 0
    else if(iBufferStringLength == 3 && caStrArg[iBufferStringLength - 1] == '>')
    {
        ucCMDA = 0;
        uiBytes = S2CreateMessageItem(pucTemp, SECS_INT1, 1, 1,&ucCMDA);
    }
    else
    {
        // if command was accepted and there is a response from the controller ( ...ASCII... r/n/> ), return CMDA = 25
        // and entire ASCII string
        ucCMDA = 25;
        // < L,2
        //      1. CMDA
        //      2. ASCII
        // >
        if((uiBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 2, &ucL)) <= 0)
            return FAILURE;
        pucTemp += uiBytes;
        if((uiBytes = S2CreateMessageItem(pucTemp, SECS_INT1, 1, 1,&ucCMDA)) <= 0)
            return FAILURE;
        if(caStrArg[iBufferStringLength - 2] == 'M')
        {
            caStrArg[0] = 'M';
            caStrArg[1] = '\0';
            iBufferStringLength = strlen(caStrArg);
        }
        pucTemp += uiBytes;
        if((uiBytes = S2CreateMessageItem(pucTemp, SECS_ASCII, 1, iBufferStringLength, (unsigned char*)caStrArg)) <= 0)
            return FAILURE;
        pucTemp += uiBytes;
        uiBytes =  pucTemp - caSecsMsgData;
    }
    //return S2F66
    iRetValue = S2PrepareSecsMsgDevice(iPortNumArg, 2, 66, 1, 0, caSecsMsgData, uiBytes,
                                fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes++);

    return iRetValue;
}
