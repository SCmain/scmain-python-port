/****************************************************************
*
* Program:      File with all SxFx functions for SECS-II when our controller is device
* File:         secsfndev.c
* Functions:
*
* Description:
*
* Modification history:
* Rev      ECO#    Date    Author      Brief Description
*
****************************************************************/

#include "sck.h"
#include "mapk.h"
#include "secsg.h"
#include "secsl.h"
#include "mapstn.h"
#include "fiol.h"
#include "cmdfns.h"
#include "scstat.h"
#include "scmac.h"
#include "ser.h"

extern int iCMDA1;
extern int iCEID1;
extern int iPORT1;
extern int iSLOT1;

unsigned char caSecsMsgData[244];
int iProcessingS2F65 = FALSE;
extern stDim stDimTable[];

// TOOL (AMAT host) <--------- WMS (Wafer Management System, Equipe controller)
// Routines called from INX command, they are used when SECS command is called from macro or command line
/*****************************************************************************
 * Function Name:   S2DeviceS1F1FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS1F1FromController(int iPortNumArg)
{
    return(S2PrepareSecsMsgDevice(iPortNumArg, 1, 1, 1, 1, NULL, 0, 0L));
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F66FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS2F66FromController(int iPortNumArg, int *ipParametersArg)
{
    int iDataLength;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader) (fpucMsgBlock + 1);

    if((iDataLength = S2CreateMessageItem(caSecsMsgData, SECS_INT1, 1, 1, (unsigned char*)&ipParametersArg[0])))
        return(S2PrepareSecsMsgDevice(iPortNumArg, 2, 66, 1, 0, caSecsMsgData, iDataLength, fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));

    return FAILURE;
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F67FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS2F67FromController(int iPortNumArg, int *ipParametersArg)
{
    int iNumberOfBytes, iPrepareRetValue;
    unsigned char ucL = 2;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader) (fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = caSecsMsgData;

    // Create main L
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 2, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;
    // Create RCMD1
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_RCMD1].ucSecsType, stDimTable[SECS_RCMD1].iNumOfLengthBytes,
                                stDimTable[SECS_RCMD1].iDataLength, (unsigned char*)&ipParametersArg[0])) <= 0)
        return FAILURE;

    pucTemp += iNumberOfBytes;
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 2, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create LLK
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_LLK].ucSecsType, stDimTable[SECS_LLK].iNumOfLengthBytes,
                                stDimTable[SECS_LLK].iDataLength, (unsigned char*)&ipParametersArg[1])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create SLOT
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_SLOT].ucSecsType, stDimTable[SECS_SLOT].iNumOfLengthBytes,
                                stDimTable[SECS_SLOT].iDataLength, (unsigned char*)&ipParametersArg[2])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;
    iNumberOfBytes = pucTemp - caSecsMsgData;
    iPrepareRetValue = S2PrepareSecsMsgDevice(iPortNumArg, 2, 67, 1, 1, caSecsMsgData, iNumberOfBytes, fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes);
    if(!iPrepareRetValue)
        iCMDA1 = WAITING_S2F68;

    return(iPrepareRetValue);
}
/*****************************************************************************
 * Function Name:   S2DeviceS5F1FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS5F1FromController(int iPortNumArg, int *ipParametersArg)
{
    int iNumberOfBytes;
    unsigned char ucL = 2;
    fpstSecsHeader fpstTempSecsHeader;
    long lALID;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock+1);
    fpucMsgBlock += sizeof(stSecsHeader)+1;
    pucTemp = caSecsMsgData;

    // Create main L
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 3, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create ALCD
    ucL = ipParametersArg[0];
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_ALCD].ucSecsType,
                     stDimTable[SECS_ALCD].iNumOfLengthBytes, stDimTable[SECS_ALCD].iDataLength, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    lALID = ipParametersArg[1];
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_ALID].ucSecsType,
                     stDimTable[SECS_ALID].iNumOfLengthBytes, stDimTable[SECS_ALID].iDataLength,
                     (unsigned char*)&lALID )) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_ALTX].ucSecsType,
                     stDimTable[SECS_ALTX].iNumOfLengthBytes,
                     stDimTable[SECS_ALTX].iDataLength, &ucL)) <= 0)
        return FAILURE;

    pucTemp += iNumberOfBytes;
    iNumberOfBytes = pucTemp - caSecsMsgData;

    return(S2PrepareSecsMsgDevice(iPortNumArg, 5, 1, 1, 0, caSecsMsgData, iNumberOfBytes, ++fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
}
/*****************************************************************************
 * Function Name:   S2DeviceS6F65FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS6F65FromController(int iPortNumArg)
{
    int iCounter, iCounter2, iNumberOfBytes, iStationOffset;
    unsigned char ucL = 2, ucaScanRet[NOFSLOTS];
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *pucTemp;
    pstMapStn fpstTempMapStation;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = caSecsMsgData;

    // Create main L,6
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 6, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    fpstTempMapStation = MPGetPstHeadScan();

    for(iCounter = 0; iCounter < 4 ; iCounter++)
    {
        if(!fpstTempMapStation)
            return FAILURE;
        // Create main L,3
        if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 3, &ucL)) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        // Create STATUS
        iStationOffset = (int)fpstTempMapStation->m_plROffset[2][0];
        if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_STATUS].ucSecsType,
                     stDimTable[SECS_STATUS].iNumOfLengthBytes,
                     stDimTable[SECS_STATUS].iDataLength, (unsigned char*)&iStationOffset)) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        // Create CASSETTE
        iStationOffset = (int)fpstTempMapStation->m_plROffset[2][1];
        if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_CASSETTE].ucSecsType,
                         stDimTable[SECS_CASSETTE].iNumOfLengthBytes,
                     stDimTable[SECS_CASSETTE].iDataLength, (unsigned char*)&iStationOffset )) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        if(fpstTempMapStation->m_plROffset[2][2] != NO_SLOT_INFO)
        {
            // Create main L,0
            if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 0, &ucL)) <= 0)
                return FAILURE;
            pucTemp += iNumberOfBytes;
        }
        else
        {
            // Create main L,1
            if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 1, &ucL)) <= 0)
                return FAILURE;
            pucTemp += iNumberOfBytes;

            for(iCounter2 = 0; iCounter2 < NOFSLOTS; iCounter2++)
                ucaScanRet[iCounter2] = (unsigned char)fpstTempMapStation->m_plZPos[2][iCounter2];
            // Create SLOTS
            if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_SLOTS].ucSecsType,
                     stDimTable[SECS_SLOTS].iNumOfLengthBytes,
                     stDimTable[SECS_SLOTS].iDataLength, ucaScanRet)) <= 0)
                return FAILURE;
            pucTemp += iNumberOfBytes;
        }
        fpstTempMapStation = fpstTempMapStation->m_pstNextPtr;
    }

    fpstTempMapStation = fpstTempMapStation->m_pstNextPtr;      // Skip E and F
    fpstTempMapStation = fpstTempMapStation->m_pstNextPtr;
    for(iCounter = 0; iCounter < 2; iCounter++)
    {
        if(!fpstTempMapStation)
            return FAILURE;

        // Create main L,4
        if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 4, &ucL)) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        // Create STATUS
        iStationOffset = (int)fpstTempMapStation->m_plROffset[2][0];
        if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_STATUS].ucSecsType,
                     stDimTable[SECS_STATUS].iNumOfLengthBytes,
                     stDimTable[SECS_STATUS].iDataLength, (unsigned char*)&iStationOffset )) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        // Create ORGPORT
        iStationOffset = (int)fpstTempMapStation->m_plROffset[2][1];
        if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_ORGPORT].ucSecsType,
                     stDimTable[SECS_ORGPORT].iNumOfLengthBytes,
                     stDimTable[SECS_ORGPORT].iDataLength, (unsigned char*)&iStationOffset )) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        // Create ORGSLOT
        iStationOffset = (int)fpstTempMapStation->m_plROffset[2][2];
        if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_ORGSLOT].ucSecsType,
                     stDimTable[SECS_ORGSLOT].iNumOfLengthBytes,
                     stDimTable[SECS_ORGSLOT].iDataLength, (unsigned char*)&iStationOffset )) <= 0)
            return FAILURE;
        pucTemp += iNumberOfBytes;

        // Create WAFSTAT1
        iStationOffset = (int)fpstTempMapStation->m_plROffset[2][3];
        if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_WAFSTAT1].ucSecsType,
                     stDimTable[SECS_WAFSTAT1].iNumOfLengthBytes,
                     stDimTable[SECS_WAFSTAT1].iDataLength, (unsigned char*)&iStationOffset )) <= 0)
            return FAILURE;

        pucTemp += iNumberOfBytes;
        fpstTempMapStation = fpstTempMapStation->m_pstNextPtr;
    }
    iNumberOfBytes = pucTemp - caSecsMsgData;

    return(S2PrepareSecsMsgDevice(iPortNumArg, 6, 65, 1, 0, caSecsMsgData, iNumberOfBytes, ++fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
}
/*****************************************************************************
 * Function Name:   S2DeviceS6F69FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS6F69FromController(int iPortNumArg, int *ipParametersArg)
{
    int iNumberOfBytes;
    unsigned char ucL = 2;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = caSecsMsgData;

    // Create main L
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 5, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create LDST
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_LDST].ucSecsType,
                     stDimTable[SECS_LDST].iNumOfLengthBytes,
                     stDimTable[SECS_LDST].iDataLength, (unsigned char*)&ipParametersArg[0])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create LLK
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_LLK].ucSecsType,
                     stDimTable[SECS_LLK].iNumOfLengthBytes,
                     stDimTable[SECS_LLK].iDataLength, (unsigned char*)&ipParametersArg[1])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create PORT
    if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_PORT].ucSecsType,
                     stDimTable[SECS_PORT].iNumOfLengthBytes,
                     stDimTable[SECS_PORT].iDataLength, (unsigned char*)&ipParametersArg[2])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create SLOT
    if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_SLOT].ucSecsType,
                     stDimTable[SECS_SLOT].iNumOfLengthBytes,
                     stDimTable[SECS_SLOT].iDataLength, (unsigned char*)&ipParametersArg[3])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create WFST
    if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_WFST].ucSecsType,
                     stDimTable[SECS_WFST].iNumOfLengthBytes,
                     stDimTable[SECS_WFST].iDataLength, (unsigned char*)&ipParametersArg[4])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;
    iNumberOfBytes = pucTemp - caSecsMsgData;

    return(S2PrepareSecsMsgDevice(iPortNumArg, 6, 69, 1, 0, caSecsMsgData, iNumberOfBytes, ++fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));

}
/*****************************************************************************
 * Function Name:   S2DeviceS6F71FromController
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS6F71FromController(int iPortNumArg, int *ipParametersArg)
{
    int iNumberOfBytes;
    unsigned char ucL = 2;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = caSecsMsgData;

    // create main L
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, SECS_LIST, 1, 3, &ucL)) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create CEID
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_CEID].ucSecsType,
                     stDimTable[SECS_CEID].iNumOfLengthBytes,
                     stDimTable[SECS_CEID].iDataLength, (unsigned char*)&ipParametersArg[0])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create FRPT
    if((iNumberOfBytes = S2CreateMessageItem(pucTemp, stDimTable[SECS_FRPT].ucSecsType,
                     stDimTable[SECS_FRPT].iNumOfLengthBytes,
                     stDimTable[SECS_FRPT].iDataLength, (unsigned char*)&ipParametersArg[1])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;

    // Create TOPT
    if((iNumberOfBytes = S2CreateMessageItem( pucTemp, stDimTable[SECS_TOPT].ucSecsType,
                     stDimTable[SECS_TOPT].iNumOfLengthBytes,
                     stDimTable[SECS_TOPT].iDataLength, (unsigned char*)&ipParametersArg[2])) <= 0)
        return FAILURE;
    pucTemp += iNumberOfBytes;
    iNumberOfBytes = pucTemp - caSecsMsgData;

    return(S2PrepareSecsMsgDevice(iPortNumArg, 6, 71, 1, 0, caSecsMsgData, iNumberOfBytes, ++fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
}

// TOOL (AMAT Host) -----> WMS (Wafer Management System, EQUIPE controller)
// Following are the routines to serve SECS streams and functions received from the host to our controller
// We have to respond with sending the corresponding stream and function

/*****************************************************************************
 * Function Name:   S2DeviceS1F1FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS1F1FromHost(int iPortNumArg)
{
    //Following is not SECS-II standard for S1F1 message, we keep it to be backward compatible
    //supposed to be L,2
    //                  <ASCII1>
    //                  <ASCII2>
    fpstSecsInfo fpstTempSecsInfo;
    int iNumOfItemBytes, iTotalBytes;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock;
    unsigned uReceivedDevID;

    if(!(fpstTempSecsInfo = S2GetSecsInfoCom(iPortNumArg)))
        return FAILURE;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);

    uReceivedDevID = (((WORD)fpstTempSecsHeader->m_bUpperDevID<<8) | ((WORD)fpstTempSecsHeader->m_bLowerDevID));
    fpstTempSecsInfo->m_wDevID = uReceivedDevID;

    iNumOfItemBytes = S2CreateMessageItem(caSecsMsgData, SECS_ASCII, 1, 6, (unsigned char *)"WMS   ");
    iTotalBytes = iNumOfItemBytes +
            S2CreateMessageItem(caSecsMsgData + iNumOfItemBytes, SECS_ASCII, 1, 6, (unsigned char *)"REV1.1");

    if(fpstTempSecsHeader->m_bWbit)
    {
        return(S2PrepareSecsMsgDevice(iPortNumArg, 1, 2, 1, 0, caSecsMsgData, iTotalBytes, fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
    }

    return SUCCESS;
}
/*****************************************************************************
 * Function Name:   S2DeviceS1F2FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS1F2FromHost()
{
    return SUCCESS;
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F13FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS2F13FromHost(int iPortNumArg)
{
    int iCounter, iNumberOfBytes1, iNumberOfBytes2;
    int iLmain,iECID;
    long lDl, lWaferSize, lFlatAngle;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *fpucTemp1, *fpucTemp2;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    fpucTemp1 = fpucMsgBlock;
    fpucTemp2 = caSecsMsgData;

    // Get main L
    if((iNumberOfBytes1 = S2GetMessageItem(fpucTemp1, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
        return SUCCESS;
    iNumberOfBytes2 = S2CreateMessageItem(fpucTemp2, SECS_LIST, 1, iLmain, fpucTemp1);

    for(iCounter = 0; iCounter < iLmain; iCounter++)
    {
        fpucTemp1 += iNumberOfBytes1;
        fpucTemp2 += iNumberOfBytes2;
        if((iNumberOfBytes1 = S2ReadDimensionTable(fpucTemp1, SECS_ECID, (unsigned char*)&iECID)) <= 0)
            return SUCCESS;
        if(iECID == 1)
        {
            if(FIOGetParamWaferVals(WAFER_SIZE,&lWaferSize) == FAILURE)
                return FAILURE;
            lDl = lWaferSize;
        }
        else if(iECID == 2)
        {
            lDl = 1;
        }
        else if(iECID == 3)
        {
            if(FIOGetParamWaferVals(FLAT_ANGLE,&lFlatAngle) == FAILURE)
                return FAILURE;
            lDl = lFlatAngle / 100L;
        }
        else
            return SUCCESS;

        iNumberOfBytes2 = S2CreateMessageItem(fpucTemp2, stDimTable[SECS_ECV].ucSecsType,
                                stDimTable[SECS_ECV].iNumOfLengthBytes,
                                stDimTable[SECS_ECV].iDataLength, (unsigned char*)&lDl);
    }
    iNumberOfBytes1 = fpucTemp2 - caSecsMsgData;

    if(fpstTempSecsHeader->m_bWbit)
    {
        return(S2PrepareSecsMsgDevice(iPortNumArg, 2, 14, 1, 0, caSecsMsgData, iNumberOfBytes1, fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
    }

    return SUCCESS;
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F15FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS2F15FromHost(int iPortNumArg)
{
    int iCounter, iNumberOfBytes;
    int iLmain,iECID, iDI = 0, iL;
    long  lECV;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;

    pucTemp = fpucMsgBlock;
    // Get main L
    if((iNumberOfBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
        iDI = 0x40;
    else
    {
        for(iCounter = 0; iCounter < iLmain; iCounter++)
        {
            pucTemp += iNumberOfBytes;
            if((iNumberOfBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iL)) <= 0)
            {
                iDI = 0x40;
                break;
            }
            pucTemp += iNumberOfBytes;
            if((iNumberOfBytes = S2ReadDimensionTable(pucTemp, SECS_ECID, (unsigned char*)&iECID)) <= 0)
            {
                iDI = 0x40;
                break;
            }
            if((iNumberOfBytes = S2ReadDimensionTable(pucTemp,SECS_ECV,(unsigned char*)&lECV)) <= 0)
            {
                iDI = 0x40;
                break;
            }
            if(iECID == 1)
            {
                if(FIOSetParamWaferVals(WAFER_SIZE,lECV) == FAILURE)
                    return FAILURE;
            }
            else if(iECID == 2)
            {
                iDI = 0;
            }
            else if(iECID == 3)
            {
                if(FIOSetParamWaferVals(FLAT_ANGLE,lECV*100L) == FAILURE)
                    return FAILURE;
            }
            else
            {
                iDI = 1;
                break;
            }
        }
    }
    iNumberOfBytes = S2CreateMessageItem(caSecsMsgData, stDimTable[SECS_EAC].ucSecsType, stDimTable[SECS_EAC].iNumOfLengthBytes,
                            stDimTable[SECS_EAC].iDataLength, (unsigned char*)&iDI);

    if(fpstTempSecsHeader->m_bWbit)
    {
        return(S2PrepareSecsMsgDevice(iPortNumArg, 2, 16, 1, 0, caSecsMsgData, iNumberOfBytes, fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
    }

    return SUCCESS;
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F41FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS2F41FromHost(int iPortNumArg)
{
    // This command is propably some leftover, doesn't check what we received and just sends out
    // 0 length message
    int iNumberOfBytes = 0;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);

    if(fpstTempSecsHeader->m_bWbit)
    {
        return(S2PrepareSecsMsgDevice(iPortNumArg, 2, 42, 1, 0, caSecsMsgData, iNumberOfBytes, fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes));
    }
    return SUCCESS;
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F65FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
struct instr_st  SecsInstr;

int S2DeviceS2F65FromHost(int iPortNumArg)
{
    int iBytes, iNumberOfBytes;
    unsigned char ucCMDA = 0;
    int iLmain, iRCMD;
    fpstSecsHeader fpstTempSecsHeader;
    unsigned char *fpucMsgBlock, *pucTemp;
    instr_ptr DummyInstr;
    int iResult = FALSE;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpstTempSecsHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = fpucMsgBlock;

    iProcessingS2F65 = TRUE;
    // Get main L
    if((iNumberOfBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
    {
        ucCMDA = 1;
        goto EndLabel;
    }
    if(iLmain != 2)
    {
        ucCMDA = 1;
        goto EndLabel;
    }
    pucTemp += iNumberOfBytes;

    if((iBytes = S2ReadDimensionTable(pucTemp, SECS_RCMD, (unsigned char*)&iRCMD)) <= 0)
    {
        ucCMDA = 1;
        goto EndLabel;
    }
    iNumberOfBytes += iBytes;
    pucTemp += iBytes;

    DummyInstr = (instr_ptr) &SecsInstr;

    if((MRGetMacroStatus() == MACRO_RUNNING) && !(iRCMD >= 5 && iRCMD <= 7) && (iRCMD != 24))
    {
        ucCMDA = 2;
        goto EndLabel;
    }

    switch(iRCMD)
    {
        case 1:
            iBytes = CMDcmndLineAction("INIT", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "INIT\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 2:
            iBytes = CMDcmndLineAction("INMAP", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "INMAP\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 3:
            iBytes = CMDcmndLineAction("INPRE", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "INPRE\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 4:
            iBytes = CMDcmndLineAction("INRBT", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "INRBT\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 5:
            iBytes = CMDcmndLineAction("RETRY", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "RETRY\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 6:
            iBytes = CMDcmndLineAction("STPAL", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "STPAL\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 7:
            iBytes = CMDcmndLineAction("ABORT", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "ABORT\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 24:
            iBytes = CMDcmndLineAction("PAUSE", DummyInstr, 0);
            if(iBytes == MACRO)
            {
                SERPutsRxBuff(iPortNumArg, "PAUSE\r");
                ucCMDA = 0;
            }
            else
                ucCMDA = 1;
            break;
        case 8:
            break;
        case 20:
            break;
        case 21:
            break;
        case 9:
        case 23:
            {
                int iPort;

                if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                pucTemp += iBytes;

                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_PORT, (unsigned char*)&iPort)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iPort < 1 || iPort > 4)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iRCMD == 9)
                    sprintf(caSecsMsgData, "MAP %c\r", (char)(iPort-1) + 'A');
                else
                    sprintf(caSecsMsgData, "BSIM %c\r", (char)(iPort-1) + 'A');
                iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 0);
                if(iBytes == MACRO)
                {
                    SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                    ucCMDA = 0;
                }
                else
                    ucCMDA = 1;
            }
            break;
        case 10:
            {
                int iFPort;
                int iTPort;
                int iAngle;

                if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iLmain != 3)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp,SECS_FPORT,(unsigned char*)&iFPort)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iFPort < 1 || iFPort > 4)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp,SECS_TPORT,(unsigned char*)&iTPort)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iTPort < 14 || iTPort > 15)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp,SECS_ANGLE,(unsigned char*)&iAngle)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iAngle < -1 || iAngle > 359)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                sprintf(caSecsMsgData, "LOAD %c,%c,%d\r", (char)(iFPort-1) + 'A', (char)(iTPort-14) + 'E', iAngle);
                iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 0);
                if(iBytes == MACRO)
                {
                    ucCMDA = 0;
                    SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                }
                else
                    ucCMDA = 1;
            }
            break;
        case 11:
        case 12:
            {
                int iFPort, iFStn, iTPort, iTStn, iAngle, iCounter;
                char unsigned cuaFSlots[NOFSLOTS], cuaTSlots[NOFSLOTS];
                pstMapStn fpstMapStationF, fpstMapStationT;

                if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain )) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iLmain != 5)
                {
                    ucCMDA = 2;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FPORT, (unsigned char*)&iFPort)) <= 0)
                {
                    ucCMDA = 3;
                    goto EndLabel;
                }
                if(iFPort >= 1 && iFPort <= 4)          // P1 to P4
                    iFStn = 'A' + iFPort - 1;
                else if(iFPort == 14)                   // LLA
                    iFStn = 'E';
                else if(iFPort == 15)                   // LLB
                    iFStn = 'F';
                else
                {
                    ucCMDA = 4;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FSLOTS, cuaFSlots)) <= 0)
                {
                    ucCMDA = 5;
                    goto EndLabel;
                }

                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TPORT, (unsigned char*)&iTPort)) <= 0)
                {
                    ucCMDA = 6;
                    goto EndLabel;
                }
                if(iTPort >= 1 && iTPort <= 4)          // P1 to P4
                    iTStn = 'A' + iTPort - 1;
                else if( iTPort == 14 )                 // LLA
                    iTStn = 'E';
                else if( iTPort == 15 )                 // LLB
                    iTStn = 'F';
                else
                {
                    ucCMDA = 7;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TSLOTS, cuaTSlots)) <= 0)
                {
                    ucCMDA = 8;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_ANGLE, (unsigned char*)&iAngle)) <= 0)
                {
                    ucCMDA = 9;
                    goto EndLabel;
                }
                if(iAngle < -1 || iAngle > 359)
                {
                    ucCMDA = 10;
                    goto EndLabel;
                }
                fpstMapStationF = MPGetPstHeadScan();
                while(fpstMapStationF)
                {
                    if(fpstMapStationF->m_cStName == iFStn)
                        break;
                    fpstMapStationF = fpstMapStationF->m_pstNextPtr;
                }
                if(!fpstMapStationF)
                {
                    ucCMDA = 11;
                    goto EndLabel;
                }
                for(iCounter = 0; iCounter < NOFSLOTS; iCounter++)
                    fpstMapStationF->m_plZPos[2][iCounter] = (long)cuaFSlots[iCounter];

                fpstMapStationT = MPGetPstHeadScan();
                while(fpstMapStationT)
                {
                    if(fpstMapStationT->m_cStName == iTStn )
                        break;
                    fpstMapStationT = fpstMapStationT->m_pstNextPtr;
                }
                if(!fpstMapStationT)
                {
                    ucCMDA = 12;
                    goto EndLabel;
                }
                if(iRCMD == 12)
                {
                    for(iCounter = 0; iCounter < NOFSLOTS; iCounter++)
                        fpstMapStationT->m_plZPos[2][iCounter] = (long)cuaTSlots[iCounter];
                    sprintf(caSecsMsgData, "MOVE %c,%c,%d\r", (char)iFStn, (char)iTStn, iAngle);
                }
                else
                    sprintf(caSecsMsgData, "ULOAD %c,%c,%d\r", (char)iFStn, (char)iTStn, iAngle);
                iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 0);
                if(iBytes == MACRO)
                {
                    ucCMDA = 0;
                    SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                }
                else
                    ucCMDA = 13;
            }
            break;
        case 13:
        case 14:
            {
                int iFPort, iFStn, iAngle;

                if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iLmain == 1)
                {
                    iFPort = 6;
                    pucTemp += iBytes;
                    if((iBytes = S2ReadDimensionTable(pucTemp, SECS_ANGLE, (unsigned char*)&iAngle)) <= 0)
                    {
                        ucCMDA = 9;
                        goto EndLabel;
                    }
                }
                else if(iLmain == 2)
                {
                    pucTemp += iBytes;
                    if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FPORT, (unsigned char*)&iFPort)) <= 0)
                    {
                        ucCMDA = 3;
                        goto EndLabel;
                    }
                    pucTemp += iBytes;
                    if((iBytes = S2ReadDimensionTable(pucTemp, SECS_ANGLE, (unsigned char*)&iAngle)) <= 0)
                    {
                        ucCMDA = 9;
                        goto EndLabel;
                    }
                }
                else
                {
                    ucCMDA = 2;
                    goto EndLabel;
                }
                if(iFPort >= 1 && iFPort <= 4)          // P1 to P4
                    iFStn = 'A' + iFPort - 1;
                else if(iFPort == 6)
                    iFStn = 'Z';
                else
                {
                    ucCMDA = 4;
                    goto EndLabel;
                }
                sprintf(caSecsMsgData, "ORWAF %c,%d\r", (char)iFStn, iAngle);
                iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 0);
                if(iBytes == MACRO)
                {
                    ucCMDA = 0;
                    SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                }
                else
                    ucCMDA = 13;
            }
            break;
        case 15:
            {
                int iFPort, iFStn, iFSlot;
                int iTPort, iTStn, iTSlot;
                int iWafStat, iAngle, iOrgPort, iOrgSlot;

                if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iLmain != 8)
                {
                    ucCMDA = 2;
                    goto EndLabel;
                }
                pucTemp += iBytes;

                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FPORT, (unsigned char*)&iFPort)) <= 0)
                {
                    ucCMDA = 3;
                    goto EndLabel;
                }
                if(iFPort >= 1 && iFPort <= 4)          // P1 to P4
                    iFStn = 'A' + iFPort - 1;
                else if(iFPort == 14)                   // LLA
                    iFStn = 'E';
                else if(iFPort == 15)                   // LLB
                    iFStn = 'F';
                else if(iFPort == 6)                    // orienter
                    iFStn = 'Z';
                else if(iFPort == 7)                    // robot
                    iFStn = 'R';
                else
                {
                    ucCMDA = 4;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FSLOT, (unsigned char*)&iFSlot)) <= 0)
                {
                    ucCMDA = 5;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_WAFSTAT, (unsigned char*)&iWafStat)) <= 0)
                {
                    ucCMDA = 6;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TPORT, (unsigned char*)&iTPort)) <= 0)
                {
                    ucCMDA = 7;
                    goto EndLabel;
                }
                if(iTPort >= 1 && iTPort <= 4)        // P1 to P4
                    iTStn = 'A' + iTPort - 1;
                else if(iTPort == 14)                 // LLA
                    iTStn = 'E';
                else if(iTPort == 15)                 // LLB
                    iTStn = 'F';
                else if(iTPort == 6)                  // orienter
                    iTStn = 'Z';
                else if(iTPort == 7)                  // robot
                    iTStn = 'R';
                else
                {
                    ucCMDA = 8;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TSLOT, (unsigned char*)&iTSlot)) <= 0)
                {
                    ucCMDA = 9;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_ANGLE, (unsigned char*)&iAngle)) <= 0)
                {
                    ucCMDA = 10;
                    goto EndLabel;
                }
                if(iAngle < -1 || iAngle > 359)
                {
                    ucCMDA = 11;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TPORT, (unsigned char*)&iOrgPort)) <= 0)
                {
                    ucCMDA = 12;
                    goto EndLabel;
                }
                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TPORT, (unsigned char*)&iOrgSlot)) <= 0)
                {
                    ucCMDA = 13;
                    goto EndLabel;
                }
                S2CreateCommand(caSecsMsgData, (char)iFStn, iFSlot, iWafStat, (char)iTStn, iTSlot,
                                    iAngle, iOrgPort, iOrgSlot );

                iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 0);
                if(iBytes == MACRO)
                {
                    SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                    ucCMDA = 0;
                }
                else
                    ucCMDA = 14;
                // Note: this returned CMDA = 15 if execute_instr failed,
                // for now we don't check for execute_instr failure
            }
            break;
        case 16:
        case 17:
            {
                int iFPort, iFSlot, iTSlot, iWafStat, iStn;
                pstMapStn fpstMapStation;
                int iPrevFSlot = 0, iPrevTSlot = 0, iPrevWafStat = 0;

                if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 4, (unsigned char*)&iLmain)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iLmain != 4)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                pucTemp += iBytes;

                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FPORT, (unsigned char*)&iFPort)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }

                if(iFPort >= 1 && iFPort <= 4)          // P1 to P4
                    iStn = 'A' + iFPort - 1;
                else if(iFPort == 6)                    // Orienter
                    iStn = 'Z';
                else if(iFPort == 7)                    // Robot
                    iStn = 'R';
                else
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }

                fpstMapStation = MPGetPstHeadScan();
                while(fpstMapStation)
                {
                    if(fpstMapStation->m_cStName == iStn )
                        break;
                    fpstMapStation = fpstMapStation->m_pstNextPtr;
                }
                if(!fpstMapStation)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }

                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_FSLOT, (unsigned char*)&iFSlot)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iFSlot < 1 || iFSlot > NOFSLOTS)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }

                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_TSLOT, (unsigned char*)&iTSlot)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                if(iTSlot < 0 || iTSlot > NOFSLOTS)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }

                pucTemp += iBytes;
                if((iBytes = S2ReadDimensionTable(pucTemp, SECS_WAFSTAT, (unsigned char*)&iWafStat)) <= 0)
                {
                    ucCMDA = 1;
                    goto EndLabel;
                }
                switch(iFPort)
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        if(!iTSlot)
                        {
                            if(iRCMD == 16)
                                fpstMapStation->m_plZPos[2][iFSlot - 1] = (long)iWafStat;
                            else
                                fpstMapStation->m_plZPos[2][iFSlot - 1] = 0;
                        }
                        else
                        {
                            if(iFSlot > iTSlot)
                            {
                                iBytes = iTSlot;
                                iTSlot = iFSlot;
                                iFSlot = iBytes;
                            }
                            iFSlot--;
                            for(; iFSlot < iTSlot; iFSlot++)
                                if(iRCMD == 16)
                                    fpstMapStation->m_plZPos[2][iFSlot] = (long)iWafStat;
                                else
                                    fpstMapStation->m_plZPos[2][iFSlot] = 0;
                        }
                        break;
                    case 6:
                    case 7:
                        iPrevFSlot = (int)fpstMapStation->m_plROffset[2][1];
                        iPrevTSlot = (int)fpstMapStation->m_plROffset[2][2];
                        iPrevWafStat = (int)fpstMapStation->m_plROffset[2][3];

                        if(iRCMD == 16)
                        {
                            fpstMapStation->m_plROffset[2][1] = (long)iFSlot;
                            fpstMapStation->m_plROffset[2][2] = (long)iTSlot;
                            fpstMapStation->m_plROffset[2][3] = (long)iWafStat;
                        }
                        else
                            fpstMapStation->m_plROffset[2][3] = (long)0;
                        break;
                }
                sprintf(caSecsMsgData, "DCWAF %c, %d, %d, %d, %d\r", (char)iStn,
                                17 - iRCMD, iPrevFSlot, iPrevTSlot, iPrevWafStat);
                iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 0);
                if(iBytes == MACRO)
                {
                    ucCMDA = 0;
                    SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                }
                else
                    ucCMDA = 1;
            }
            break;
        case 25:
            if((iBytes = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain)) <= 0)
            {
                ucCMDA = 1;
                goto EndLabel;
            }
            if(iLmain != 1)
            {
                ucCMDA = 2;
                goto EndLabel;
            }
            pucTemp += iBytes;
            if((iBytes = S2GetMessageItem(pucTemp, SECS_PASS_THROUGH, 1, 1, (unsigned char*)&iLmain)) <= 0)
            {
                ucCMDA = 3;
                goto EndLabel;
            }
            if((iBytes = S2GetMessageItem(pucTemp, SECS_ASCII, 1, iLmain, caSecsMsgData)) <= 0)
            {
                ucCMDA = 4;
                goto EndLabel;
            }
            caSecsMsgData[iLmain] = '\r';
            iBytes = CMDcmndLineAction(caSecsMsgData, DummyInstr, 1);
            if(iBytes == MACRO)
            {
                ucCMDA = 0;
                SERPutsRxBuff(iPortNumArg, caSecsMsgData);
            }
            else if(iBytes == -1)
            {
                ucCMDA = 5;
            }
            else if(iBytes == -2)
            {
                ucCMDA = 6;
            }
            else if(iBytes == 0)
            {
                SERPutsRxBuff(iPortNumArg, caSecsMsgData);
                // Note: in old code this returned CMDA = 7 if execute_instr failed, for this RMCD we will return
                // if macro/command failed when we send S2F66 from com module
                return SUCCESS;
            }
            break;
        default:
            return FAILURE;
    }
EndLabel:
    if(fpstTempSecsHeader->m_bWbit == 0)
        return SUCCESS;
    else
    {
        iBytes = S2CreateMessageItem(caSecsMsgData,SECS_INT1,1,1,&ucCMDA);
        if(iBytes != 0)
        {
            iBytes = S2PrepareSecsMsgDevice(iPortNumArg, 2, 66, 1, 0, caSecsMsgData, iBytes,
                                    fpstTempSecsHeader->m_uSystemBytesUnion.u_ulSystemBytes);
        }
    }

    return iBytes;
}
/*****************************************************************************
 * Function Name:   S2DeviceS2F68FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS2F68FromHost(int iPortNumArg)
{
    int iTableRet;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = fpucMsgBlock;

    iCMDA1 = 0;
    if((iTableRet = S2ReadDimensionTable(pucTemp, SECS_CMDA1, (unsigned char*)&iCMDA1)) <= 0)
        iCMDA1 = 1;
    if(!iCMDA1)
        iCEID1 = WAITING_CEID1;

    return SUCCESS;
}
/*****************************************************************************
 * Function Name:   S2DeviceS6F67FromHost
 * Parameters:
 * Returns:
 ****************************************************************************/
int S2DeviceS6F67FromHost(int iPortNumArg)
{
    int iTableRet;
    int iLmain;
    unsigned char *fpucMsgBlock, *pucTemp;

    S2GetMsgBlock(COM, iPortNumArg, &fpucMsgBlock);
    fpucMsgBlock += sizeof(stSecsHeader) + 1;
    pucTemp = fpucMsgBlock;

    // Get main L
    if((iTableRet = S2GetMessageItem(pucTemp, SECS_LIST, 1, 1, (unsigned char*)&iLmain )) <= 0)
        return FAILURE;
    if( iLmain != 3 )
        return FAILURE;
    pucTemp += iTableRet;
    if((iTableRet = S2ReadDimensionTable(pucTemp, SECS_CEID, (unsigned char*)&iCEID1)) <= 0)
        return FAILURE;
    pucTemp += iTableRet;
    if((iTableRet = S2ReadDimensionTable(pucTemp, SECS_PORT, (unsigned char*)&iPORT1)) <= 0)
        return FAILURE;
    pucTemp += iTableRet;
    if((iTableRet = S2ReadDimensionTable(pucTemp, SECS_SLOT, (unsigned char*)&iSLOT1)) <= 0)
        return FAILURE;

    return SUCCESS;
}
