/****************************************************************
*
* Program:      File with all SxFx functions for SECS-II
* File:         S2Secs.c
* Functions:    S2HostS1F1
*               S2HostS1F2
*               S2HostS1F5
*               S2HostS1F6
*               S2HostS2F15
*               S2HostS2F16
*               S2HostS2F41
*               S2HostS2F42
*               S2HostS4F111
*               S2HostS4F112
*               S2HostS4F113
*               S2HostS4F114
*               S2HostS5F1
*               S2HostS6F13
*               S2HostSxFx
*
* Description:  Functions which process each particular message,
*               each SxFx message has its own function, which can be further broken down
*               depending on the other parameters. SxFx Functions replace former Equipe
*               Indexer functions
*
* Modification history:
* Rev      ECO#    Date    Author      Brief Description
*
****************************************************************/

#include <string.h>
#include "sck.h"
#include "secsg.h"
#include "secsl.h"
#include "sctim.h"
#include "screg.h"

unsigned long ulS1F1TimeOut;
extern fpstFluoroTracTagData fpstFluoroTracTagDataArray[];
//optimization
//unsigned char caSecsMsgData[245]; // Worst scenario

int S2HostS1F1(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    //was Equipe function #19 and #100
    //Assume Port 7 is dedicated for FluoroTrac : PORT G --> bits 8 - 11 must be 7
    if(iParameterArg == 1)
    {
        if((uDevIDArg & MASK_PORT_G) == MASK_PORT_G)
            return SUCCESS;
    }
    if(!(*ipPCWaitingArg))
    {
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        ulS1F1TimeOut = 0;
        if(iParameterArg != 0 && iParameterArg != 1)
            return FAILURE;
        if(iParameterArg == 0) //#19
            fpstTempDevArg->m_wDevID = 0;
    }
    else if(fpstTempComArg->m_wLastError == RETRY_ERROR)
    {
        *ipPCWaitingArg = FALSE;
        return FAILURE;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
            ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
            *ipSetPCInstrArg = TRUE;
            return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else if(!(fpstTempDevArg->m_wDevID))
            *ipSetPCInstrArg = TRUE;
        else
            *ipPCWaitingArg = FALSE;
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 1, 1, 1, 1, caSecsMsgData, 0);
    return(iRetVal);
}

int S2HostS1F2(fpstSecsInfo fpstTempDevArg, int iRegisterNumber)
{
    //was Equipe function #20 and #101
//printf("fpstTempDev=%x iReg=%d\n",fpstTempDevArg, iRegisterNumber);
    RGSetRegister(iRegisterNumber, fpstTempDevArg->m_wDevID & 0x0000FFFFL);
    return SUCCESS;
}

int S2HostS1F5(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                unsigned uDevIDArg,int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

//    if(!(fpstTempDevArg->m_wDevID))
//        return FAILURE;

//printf("S1F5 PCWait=%d iParm=%d\n",*ipPCWaitingArg, iParameterArg);

    if(!(*ipPCWaitingArg) )
    {
        fpstTempComArg->m_wNewMap = FALSE;
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        switch(iParameterArg)
        {
            case 2 : //SFCD = 2, gets map from Device, was Equipe function #2
                memcpy(caSecsMsgData, "\x21\x01\x02", 3);
                break;
            case 0 : //SFCD = 0, gets map from Device, was Equipe function #8
                memcpy(caSecsMsgData, "\x21\x01\x00", 3);
                break;
            default:
                return FAILURE;
        }
        ulS1F1TimeOut = 0;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
        ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else
        {
            if(!(fpstTempComArg->m_wNewMap))
                *ipSetPCInstrArg = TRUE;
            else
                *ipPCWaitingArg = FALSE;
        }
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 1, 5, 1, 1, caSecsMsgData, 3);
    return(iRetVal);
}

int S2HostS1F6(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iParameterArg,
                int iRegisterNum1, int iRegisterNum2, int iRegisterNum3)
{
    long lElvPos;


    switch(iParameterArg)
    {
        case 2 :  //SFCD = 2, reads map results from S1F5, SFCD = 2, was Equipe function #4
            if(!(fpstTempDevArg->m_wDevID))
                return FAILURE;
            RGSetRegister(iRegisterNum2, (unsigned long)fpstTempDevArg->m_cMap[iRegisterNum1]);
//            RGSetRegister(iRegisterNum2, (unsigned long)fpstTempComArg->m_cMap[iRegisterNum1]);
            break;
        case 0 :  //SFCD = 0, gets status from Device, was Equipe function #9
//            RGSetRegister(iRegisterNum1, (unsigned long)fpstTempComArg->m_ulInxFsd);
            RGSetRegister(iRegisterNum1, (unsigned long)fpstTempDevArg->m_ulInxFsd);
	    if(iRegisterNum2>0)
	    {	// get elevator position in mm.
		lElvPos = fpstTempDevArg->m_ucMsgBlock[56];
		lElvPos <<= 8;
		lElvPos |= fpstTempDevArg->m_ucMsgBlock[57];
		RGSetRegister(iRegisterNum2, lElvPos);
//printf("S2HostS1F6 0 msg0=%x msg1=%x pos=%d\n ", fpstTempComArg->m_ucMsgBlock[56],fpstTempComArg->m_ucMsgBlock[57], lElvPos);
	    }
	    RGSetRegister(iRegisterNum3, (unsigned long)fpstTempDevArg->m_ucMsgBlock[23]);

            break;
        default:
            return FAILURE;
    }

//printf("S1F6 fpCom=%x iReg1=%d iReg2=%d iReg3=%d\n",fpstTempComArg, fpstTempComArg->m_ulInxFsd, lElvPos, fpstTempComArg->m_ucMsgBlock[23]);
//printf("S1F6 fpDev=%x iReg1=%d iReg2=%d iReg3=%d\n", fpstTempDevArg, fpstTempDevArg->m_ulInxFsd, lElvPos, fpstTempDevArg->m_ucMsgBlock[23]);

    return SUCCESS;
}

int S2HostS2F13(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                   unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg )
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    //was Equipe function #15
//    if(!(fpstTempDevArg->m_wDevID))
//        return FAILURE;

    if(!(*ipPCWaitingArg))
    {
        // Request EC
        memcpy(caSecsMsgData, "\x01\x01\xA5\x01\x01", 5);
//        caSecsMsgData[9] = (unsigned char)((uDevIDArg & 0xFF00)>>8);  // UpperDevID
//        caSecsMsgData[10] = (unsigned char)(uDevIDArg & 0x00FF);      // LowerDevID
        caSecsMsgData[4] = iParameterArg; // ECID
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        fpstTempComArg->m_uiInxEac = 0xFFFF;
        ulS1F1TimeOut = 0;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
        ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else
        {
            if((fpstTempComArg->m_uiInxEac & 0xFF00) == 0xFF00)
                *ipSetPCInstrArg = TRUE;
            else
                *ipPCWaitingArg = FALSE;
        }
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 2, 13, 1, 1, caSecsMsgData, 5);
    // R bit is 0, E bit is 1, W bit is 1,
    return(iRetVal);
}

int S2HostS2F15(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                   unsigned uDevIDArg, int *ipSetPCInstrArg, int *ipPCWaitingArg )
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    //was Equipe function #15
//    if(!(fpstTempDevArg->m_wDevID))
//        return FAILURE;

    if(!(*ipPCWaitingArg))
    {
        // Modify the Indexer Equipment Constant
        // ECID = 40, DevId Default is 251
        memcpy(caSecsMsgData, "\x01\x01\x01\x02\xA5\x01\x28\xA9\x02", 9);
        caSecsMsgData[9] = (unsigned char)((uDevIDArg & 0xFF00)>>8);  // UpperDevID
        caSecsMsgData[10] = (unsigned char)(uDevIDArg & 0x00FF);      // LowerDevID
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        fpstTempComArg->m_uiInxEac = 0xFFFF;
        ulS1F1TimeOut = 0;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
        ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else
        {
            if((fpstTempComArg->m_uiInxEac & 0xFF00) == 0xFF00)
                *ipSetPCInstrArg = TRUE;
            else
                *ipPCWaitingArg = FALSE;
        }
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 2, 15, 1, 1, caSecsMsgData, 11);
    // R bit is 0, E bit is 1, W bit is 1,
    return(iRetVal);
}

int S2HostS2F16(fpstSecsInfo fpstTempDevArg, int iRegisterNum)
{
    //was Equipe function #16
    RGSetRegister(iRegisterNum,(fpstTempDevArg->m_uiInxEac & 0x00FF));
    return SUCCESS;
}


int S2HostS2F37(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg, unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    unsigned uMsgLen;
    unsigned char caSecsMsgData[245];
    int iRetVal;

//    if( !(fpstTempDevArg->m_wDevID) )
//        return FAILURE;

    if(!(*ipPCWaitingArg) )
    {
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
			//     L,2         CEED        item2
        memcpy(caSecsMsgData, "\x01\x02\xA5\x01\x01\x01\x00", 7);
  	if(iParameterArg == 0)
	    caSecsMsgData[4] = (unsigned char)0x00;
        ulS1F1TimeOut = 0;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
        ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 2, 37, 1, 1, caSecsMsgData, 7);
    return(iRetVal);

}

int S2HostS2F38(fpstSecsInfo fpstTempDevArg, int iRegisterNumArg)
{
    if(fpstTempDevArg->m_uiInxHcack == 0x00FF)
        // Illegal HACK, make it as can't perform now
        RGSetRegister(iRegisterNumArg, 0x0200);
    else
        RGSetRegister(iRegisterNumArg, (unsigned long)fpstTempDevArg->m_uiInxHcack);
    return SUCCESS;
}


int S2HostS2F41(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                    unsigned uDevIDArg, int iParameterArg, int iPositionArg, int *ipSetPCInstrArg,
                    int *ipPCWaitingArg)
{
    unsigned uMsgLenght;
    //optimization
    unsigned char caSecsMsgData[245];

    int iRetVal;

//    if( !(fpstTempDevArg->m_wDevID) )
//        return FAILURE;

    if(!(*ipPCWaitingArg))
    {
        memcpy(caSecsMsgData, "\x01\x02\xA5\x01\x00\x01\x00", 7);
        caSecsMsgData[4] = (unsigned char)iParameterArg;

        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        ulS1F1TimeOut = 0;
        uMsgLenght = 7;

        switch(iParameterArg)
        {
            case 6 :  //RMCD = 6, moves to slot was Equipe function #6
            case 12 : //RMCD = 12, moves to position, was Equipe function #7
                caSecsMsgData[6] = (unsigned char)0x01;
                caSecsMsgData[7] = (unsigned char)0xA9;
                caSecsMsgData[8] = (unsigned char)0x02;
                caSecsMsgData[9] = (unsigned char)((iPositionArg & 0xFF00)>>8);
                caSecsMsgData[10] = (unsigned char)(iPositionArg & 0x00FF);
                uMsgLenght = 11;
                fpstTempDevArg->m_uiInxHcack = 0x00FF;
                fpstTempDevArg->m_uiInxCeid &= 0xFFF0;
//printf("S1F41 RMCD=6, 12: ipos=%d\n",iPositionArg);
                break;
            case 16 : //RMCD = 5, resets alarm, was Equipe function #5
                fpstTempDevArg->m_uiInxHcack = 0;
                fpstTempDevArg->m_uiInxCeid = 0;
                break;
	    case 149 :
                caSecsMsgData[6] = (unsigned char)0x03;
                caSecsMsgData[7] = (unsigned char)0xA9;
                caSecsMsgData[8] = (unsigned char)0x02;
                caSecsMsgData[9] = (unsigned char)((iPositionArg & 0xFF00)>>8);
                caSecsMsgData[10] = (unsigned char)(iPositionArg & 0x00FF);
                caSecsMsgData[11] = (unsigned char)0xA9;
                caSecsMsgData[12] = (unsigned char)0x02;
                caSecsMsgData[13] = (unsigned char)0x07; //((1000 & 0xFF00)>>8);
                caSecsMsgData[14] = (unsigned char)0x0D; //(1000 & 0x00FF);
                caSecsMsgData[15] = (unsigned char)0xA9;
                caSecsMsgData[16] = (unsigned char)0x02;
                caSecsMsgData[17] = (unsigned char)0x04; //((1000 & 0xFF00)>>8);
                caSecsMsgData[18] = (unsigned char)0x01; //(1000 & 0x00FF);
                uMsgLenght = 19;
                fpstTempDevArg->m_uiInxHcack = 0x00FF;
                fpstTempDevArg->m_uiInxCeid &= 0xFFF0;
//printf("S2F41 149, pos=%d c1=%x c1=%x c2=%x c2=%x c3=%x c3=%x\n",iPositionArg,caSecsMsgData[9],caSecsMsgData[10],caSecsMsgData[9],caSecsMsgData[13],caSecsMsgData[14],caSecsMsgData[17],caSecsMsgData[18]);
		break;

            case 9 :   //RMCD = 9, move to stage, was Equipe command #0
            case 13 :  //RMCD = 13, move to home, was Equipe function #1
            default :  //passes RMCD, was Equipe function #10
                fpstTempComArg->m_uiInxHcack = 0x00FF;
                break;
        }
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
        ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else
        {
            switch(iParameterArg)
            {
                case 9 :
                case 13 :
                default :
                    if((fpstTempComArg->m_uiInxHcack & 0x00FF) == 0x00FF)
                        *ipSetPCInstrArg = TRUE;
                    else
                        *ipPCWaitingArg = FALSE;
                    break;
                case 16 :
                    if((fpstTempDevArg->m_uiInxHcack & 0x00FF) == 0x00FF)
                        *ipSetPCInstrArg = TRUE;
                    else
                    {
                        *ipPCWaitingArg = FALSE;
                        if(!(fpstTempDevArg->m_uiInxHcack & 0x00FF))
                            fpstTempDevArg->m_uiInxAlid = 0;
                    }
                    break;
                case 6 :
                case 12 :
                    if((fpstTempComArg->m_uiInxHcack & 0x00FF) == 0x00FF)
                        *ipSetPCInstrArg = TRUE;
                    else
                    {
                        *ipPCWaitingArg = FALSE;
                        if(!(fpstTempDevArg->m_uiInxHcack & 0x00FF))
                            fpstTempDevArg->m_uiInxAlid = 0;
                    }
                    break;
            }
        }
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 2, 41, 1, 1, caSecsMsgData, uMsgLenght);
    return(iRetVal);
}

int S2HostS2F42(fpstSecsInfo fpstTempDevArg, int iRegisterNumArg)
{
    //was part of Equipe function #3, gets HCACK
//printf("S2F42 fpst=%x iReg=%d m_ack=%d\n",fpstTempDevArg, iRegisterNumArg, fpstTempDevArg->m_uiInxHcack);

    if(fpstTempDevArg->m_uiInxHcack == 0x00FF)
    {
        // Illegal HACK, make it as can't perform now
        RGSetRegister(iRegisterNumArg, 0x0200);
//        RGSetRegister(iRegisterNumArg, 0x0000);
printf("S2F42 ack=0200 fp=%x\n",fpstTempDevArg);
    }
    else
        RGSetRegister(iRegisterNumArg, (unsigned long)fpstTempDevArg->m_uiInxHcack);
    return SUCCESS;
}

int S2HostS4F111(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                    unsigned uDevIDArg, int iTagLocArg, int iPageNumberArg, unsigned uFluoroTracDevIDArg,
                    int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    //was Equipe function #11
//    if(!(fpstTempDevArg->m_wDevID))
//        return FAILURE;
    // S4,F111 FT_TAG Read Request S,H -> E, Reply
    // Bits 0-7 Smart-Comm Device ID
    // Bits 8-11 set to terminal id
    // Bits 12-14 Area - id (used in only when > 254 Smart-Comm)

    if(!(*ipPCWaitingArg))
    {
        //Prepare S4F111 command to send (S,H -> E
        caSecsMsgData[0] = (unsigned char)0x01;  // L,3
        caSecsMsgData[1] = (unsigned char)0x03;  // L,3
        caSecsMsgData[2] = (unsigned char)0xA9;                       // 1. <DEVID> Format: A9
        caSecsMsgData[3] = (unsigned char)0x02;
        //FluoroTrac Dev ID
        caSecsMsgData[4] = (unsigned char)((uFluoroTracDevIDArg & 0xFF00)>>8);  // UpperDevID
        caSecsMsgData[5] = (unsigned char)(uFluoroTracDevIDArg & 0x00FF);       // LowerDevID
        caSecsMsgData[6] = (unsigned char)0xA5;             // 2. <TAGLOG> Format: A5
        caSecsMsgData[7] = (unsigned char)0x01;
        caSecsMsgData[8] = (unsigned char)iTagLocArg;
        caSecsMsgData[9] = (unsigned char)0xA5;             // 3. <PAGE_NUMBER> Format: A5  Asyst Manual says: 41
        caSecsMsgData[10] = (unsigned char)0x01;
        caSecsMsgData[11] = (unsigned char)iPageNumberArg;      // Always "A" page number for now

        // Init the ACK6 & Tag data in S4F112, Terminal ID is in bits 8 -11 of DEVID
        if(S2InitFluoroTracTagData(eS4F112Received) == FAILURE)
            return FAILURE;
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        fpstTempDevArg->m_uiInxHcack = 0x00FF;
        ulS1F1TimeOut = 0;

        // R bit is 0, E bit is 1, W bit is 1, Smart-Comm Dev ID (bits 0 -7) only in SECS Header
        iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 4, 111, 1, 1, caSecsMsgData, 12);
        return(iRetVal);
    }
    else if(fpstTempComArg->m_iMsgSent)
        *ipPCWaitingArg = FALSE;
    else
        *ipSetPCInstrArg = TRUE;
    return SUCCESS;
}

int S2HostS4F112(int iRegisterNum1Arg, int iRegisterNum2Arg)
{
    //was Equipe function #12
    int iRetVal;

    // S4,F112 FT_TAG Read Data S,H <- E
    // Get the FluoroTrac Tag Read Data
    // Bits 0-7 Smart-Comm Device ID
    // Bits 8-11 set to terminal id, reset bu Smart-Comm
    // Bits 12-14 Area - id (used in only when > 254 Smart-Comm)
    // Get operands for ACK6 and Tag Data

    char caTemp[MAX_MESSAGE_BYTE];

    RGSetRegister(iRegisterNum1Arg, fpstFluoroTracTagDataArray[eS4F112Received]->m_bAck6);
    memcpy(caTemp, fpstFluoroTracTagDataArray[eS4F112Received]->m_cFTTagData, TAG_DATA_SIZE);
    RGSetString(iRegisterNum2Arg, caTemp, 1);

    // Init the ACK6 & Tag data in S4F112, Terminal ID is in bits 8 -11 of DEVID
    iRetVal = S2InitFluoroTracTagData(eS4F112Received);
    return iRetVal;
}

int S2HostS4F113(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg, unsigned uDevIDArg,
                    int iTagLocArg, int iPageNumberArg, char *pcStringArg, unsigned uFluoroTracDevIDArg, int *ipSetPCInstrArg,
                        int *ipPCWaitingArg)
{
    //was Equipe function #13
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    // S4,F113 FT_TAG Write Request S,H -> E, Reply
    // Bits 0-7 Smart-Comm Device ID
    // Bits 8-11 set to 0
    // Bits 12-14 Area - id (used in only when > 254 Smart-Comm)

//    if(!(fpstTempDevArg->m_wDevID))
//        return FAILURE;
    if(!(*ipPCWaitingArg))
    {
        if(S2InitFluoroTracTagData(eS4F114Received) == FAILURE)
            return FAILURE;

        caSecsMsgData[0] = (unsigned char)0x01;  // L,4
        caSecsMsgData[1] = (unsigned char)0x04;  // L,4
        caSecsMsgData[2] = (unsigned char)0xA9;              // 1. <DEVID> Format: A9
        caSecsMsgData[3] = (unsigned char)0x02;
        caSecsMsgData[4] = (unsigned char)((uFluoroTracDevIDArg & 0xFF00)>>8);  // UpperDevID
        caSecsMsgData[5] = (unsigned char)(uFluoroTracDevIDArg & 0x00FF);     // LowerDevID
        caSecsMsgData[6] = (unsigned char)0xA5;             // 2. <TAGLOG> Format: A5
        caSecsMsgData[7] = (unsigned char)0x01;
        caSecsMsgData[8] = (unsigned char)iTagLocArg;
        caSecsMsgData[9] = (unsigned char)0xA5;             // 3. <PAGE_NUMBER> Format: A5   Manual says Format: 41
        caSecsMsgData[10] = (unsigned char)0x01;
        caSecsMsgData[11] = (unsigned char)iPageNumberArg;     // Always page "A"
        caSecsMsgData[12] = (unsigned char)0x41;             // 4. <FT_TAGDATA> Format: 41
        caSecsMsgData[13] = (unsigned char)TAG_DATA_SIZE;

        memcpy(&caSecsMsgData[14], pcStringArg, TAG_DATA_SIZE);
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        fpstTempDevArg->m_uiInxHcack = 0x00FF;
        ulS1F1TimeOut = 0;
        // R bit is 0, E bit is 1, W bit is 1, Smart Comm Device ID (bits 0 -7) only in SECS Header
        iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 4, 113, 1, 1, caSecsMsgData, 30);
        return(iRetVal);
    }
    else
        if(fpstTempComArg->m_iMsgSent)
            *ipPCWaitingArg = FALSE;
        else
            *ipSetPCInstrArg = TRUE;
    return SUCCESS;
}

int S2HostS4F114(int iRegisterNumArg)
{
    //was Equipe function #14

    int iRetVal;
    // S4,F114 FT_TAG Write Acknowledge S,H <- E
    // Get the FluoroTrac Tag Read Data

    RGSetRegister(iRegisterNumArg, (unsigned long)fpstFluoroTracTagDataArray[eS4F114Received]->m_bAck6);

    // Init the ACK6 & Tag data in S4F114, Terminal ID is in bits 8 -11 of DEVID
    iRetVal = S2InitFluoroTracTagData(eS4F114Received);
    return iRetVal;
}

int S2HostS5F1(fpstSecsInfo fpstTempDevArg,int iRegisterNumArg)
{
    //was part of Equipe function #3, returns ALID and ALCD
    unsigned long ulTempRegister;

//printf("S5F1 fpst=%x iReg=%d m_aid=%d\n",fpstTempDevArg, iRegisterNumArg, fpstTempDevArg->m_uiInxAlid);

    if((fpstTempDevArg->m_uiInxAlid & 0x00FF) == 0x00FF)
    {
        // Illegal ALID, make it as an others alarm
        ulTempRegister = 37; // ALID Others
        if((fpstTempDevArg->m_uiInxAlid & 0xFF00) == 0xFF00)
            // Illegal ALCD, make it as warning
            ulTempRegister |= 0x0600;   // ALCD warning
        else
            ulTempRegister |= (unsigned long)(fpstTempDevArg->m_uiInxAlid & 0xFF00); // Copy the ALCD
        RGSetRegister(iRegisterNumArg, ulTempRegister);
    }
    else if((fpstTempDevArg->m_uiInxAlid & 0xFF00) == 0xFF00)
    {
        // Illegal ALCD, make it as warning
        ulTempRegister = 0x0600;   // ALCD warning
        ulTempRegister |= (unsigned long)(fpstTempDevArg->m_uiInxAlid & 0x00FF); // Copy the ALID
        RGSetRegister(iRegisterNumArg, ulTempRegister);
    }
    else
        // Get both ALID & ALCD variables
        RGSetRegister(iRegisterNumArg, (unsigned long)fpstTempDevArg->m_uiInxAlid);
    return SUCCESS;
}

int S2HostS6F13(fpstSecsInfo fpstTempDevArg, int iRegisterNumArg, int iParameterArg)
{
    //was part of Equipe function #3, returns CEID
//printf("S6F13 fpst=%x iReg=%d m_cid=%d\n",fpstTempDevArg, iRegisterNumArg, fpstTempDevArg->m_uiInxCeid);

    if(iParameterArg == 99)
	fpstTempDevArg->m_uiInxCeid = 0;
    else if(fpstTempDevArg->m_uiInxCeid == 0x00FF)
        // Illegal CEID, make it as POWER_UP
        RGSetRegister(iRegisterNumArg, 5);
    else
        RGSetRegister(iRegisterNumArg, (unsigned long)fpstTempDevArg->m_uiInxCeid);
    return SUCCESS;
}

int S2HostSxFx(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                unsigned uDevIDArg, int iParameterArg, int iRegisterNum1Arg, int iRegisterNum2Arg,
                int iRegisterNum3Arg, int iRegisterNum4Arg, char *pcStringArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    unsigned char *fpucMsgBlock;
    //optimization
    unsigned char caSecsMsgData[245];
    unsigned uMsgLenght;
    int iMsgLen;
    fpstSecsHeader fpstSecsMsgHeader;
    char pcTempString[256];
    int iRetVal;

    uMsgLenght = iRegisterNum1Arg;
    switch(iParameterArg)
    {
        case 200 :  //Wait until current activity completed, was Equipe function #200
            if(fpstTempComArg->m_wWTCurrActivity != IDLE)
                *ipSetPCInstrArg = TRUE;
            return SUCCESS;
        case 201 :  //Generic send, was Equipe function #102
            if(!(fpstTempDevArg->m_wDevID))
                return FAILURE;
            if(!(*ipPCWaitingArg))
            {
                memcpy( caSecsMsgData, pcStringArg, uMsgLenght );
                *ipSetPCInstrArg = TRUE;
                *ipPCWaitingArg = TRUE;
                fpstTempComArg->m_uiInxHcack = 0x00FF;
                ulS1F1TimeOut = 0;
//printf("S2HostSxFx 201 S=%d F=%d Len=%d\n",iRegisterNum3Arg,iRegisterNum4Arg,uMsgLenght);
                break;
            }
            else if(fpstTempComArg->m_iMsgSent)
            {
                *ipPCWaitingArg = FALSE;
                return SUCCESS;
            }
            *ipSetPCInstrArg = TRUE;
            return SUCCESS;
        case 202 :  //Generic read, was Equipe function #103
//printf("S2HostSxFx 202 devID=%d\n",fpstTempDevArg->m_wDevID);
printf("SxFx 202 fpCom=%x DevID=%x fpDev=%x devID=%d\n", fpstTempComArg,fpstTempComArg->m_wDevID, fpstTempDevArg,fpstTempDevArg->m_wDevID);
            *ipSetPCInstrArg = FALSE;
            *ipPCWaitingArg = FALSE;
            if(!(fpstTempDevArg->m_wDevID))
                return FAILURE;
//            fpucMsgBlock = fpstTempComArg->m_ucMsgBlock;
            fpucMsgBlock = fpstTempDevArg->m_ucMsgBlock;
//            uMsgLenght = (unsigned int)fpucMsgBlock[0] - sizeof(stSecsHeader);
            uMsgLenght = (unsigned int)fpucMsgBlock[0] - 10;
            iMsgLen = (unsigned int)fpucMsgBlock[0] - 10;
	    if (iMsgLen < 0)
		iMsgLen = 128;
//		return FAILURE;
            fpstSecsMsgHeader = (fpstSecsHeader)(fpucMsgBlock + 1);
//            fpucMsgBlock += sizeof(stSecsHeader) + 1;
            fpucMsgBlock += 11;
            //Gets Sn
            RGSetRegister(iRegisterNum1Arg, fpstSecsMsgHeader->m_bStream);
            //Gets Fn
            RGSetRegister(iRegisterNum2Arg, fpstSecsMsgHeader->m_bFunction);
            //Gets message length
            RGSetRegister(iRegisterNum3Arg, uMsgLenght);
//printf("S2HostSxFx 202 S=%d F=%d Len=%d\n",fpstSecsMsgHeader->m_bStream,fpstSecsMsgHeader->m_bFunction,uMsgLenght);
            //Gets message string
//printf("SxFx 202 fpCom=%x DevID=%x uMsglen=%d\n", fpstTempComArg,fpstTempComArg->m_wDevID, uMsgLenght);
            memcpy(pcTempString, fpucMsgBlock, iMsgLen);
            pcTempString[uMsgLenght] = '\0';
//printf("S2HostSxFx 202 tempStr=%x, msgBlk=%x\n", pcTempString, fpucMsgBlock);

            RGSetString(iRegisterNum4Arg, pcTempString, 1);
            return SUCCESS;
        default:
            return FAILURE;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, (unsigned char)iRegisterNum3Arg,
                (unsigned char)iRegisterNum4Arg, 1, 1, caSecsMsgData, iRegisterNum1Arg);
    return(iRetVal);
}

int S2HostS6F4(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    //Assume Port 7 is dedicated for FluoroTrac : PORT G --> bits 8 - 11 must be 7
    if(iParameterArg == 1)
    {
        if((uDevIDArg & MASK_PORT_G) == MASK_PORT_G)
            return SUCCESS;
    }
    if(!(*ipPCWaitingArg))
    {
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        ulS1F1TimeOut = 0;
    }
    else if(fpstTempComArg->m_wLastError == RETRY_ERROR)
    {
        *ipPCWaitingArg = FALSE;
        return FAILURE;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
            ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
            *ipSetPCInstrArg = TRUE;
            return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else if(!(fpstTempDevArg->m_wDevID))
            *ipSetPCInstrArg = TRUE;
        else
            *ipPCWaitingArg = FALSE;
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 6, 4, 1, 1, caSecsMsgData, 0);
    return(iRetVal);
}

int S2HostS6F12(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg)
{
    int iRetVal;
    //optimization
    unsigned char caSecsMsgData[245];

    //Assume Port 7 is dedicated for FluoroTrac : PORT G --> bits 8 - 11 must be 7
    if(iParameterArg == 1)
    {
        if((uDevIDArg & MASK_PORT_G) == MASK_PORT_G)
            return SUCCESS;
    }
    if(!(*ipPCWaitingArg))
    {
        *ipSetPCInstrArg = TRUE;
        *ipPCWaitingArg = TRUE;
        ulS1F1TimeOut = 0;
    }
    else if(fpstTempComArg->m_wLastError == RETRY_ERROR)
    {
        *ipPCWaitingArg = FALSE;
        return FAILURE;
    }
    else if(fpstTempComArg->m_iMsgSent && !ulS1F1TimeOut)
    {
            ulS1F1TimeOut = TIGetTimerVals(SYS_TIMER) + fpstTempComArg->m_wT3;
            *ipSetPCInstrArg = TRUE;
            return SUCCESS;
    }
    else if(ulS1F1TimeOut)
    {
        if(ulS1F1TimeOut < TIGetTimerVals(SYS_TIMER))
            *ipPCWaitingArg = FALSE;
        else if(!(fpstTempDevArg->m_wDevID))
            *ipSetPCInstrArg = TRUE;
        else
            *ipPCWaitingArg = FALSE;
        return SUCCESS;
    }
    else
    {
        *ipSetPCInstrArg = TRUE;
        return SUCCESS;
    }
    iRetVal = S2PrepareSecsMsg(iPortNumArg, uDevIDArg, 6, 12, 1, 1, caSecsMsgData, 0);
    return(iRetVal);
}

