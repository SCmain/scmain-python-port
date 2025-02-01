/***************************************************************\
 *
 * Program:     SECS-II Communication Local Header File
 * File:        s2local.H
 * Functions:   S2PrepareSecsMsg
 *              S2CheckSumOK
 *              S2InitFluoroTracTagData
 *              S2GetSecsInfoCom
 *              S2GetSecsInfoDev
 *              S2InitStructMembers
 *              S2HostS1F1
 *              S2HostS1F2
 *              S2HostS1F5
 *              S2HostS1F6
 *              S2HostS2F15
 *              S2HostS2F16
 *              S2HostS2F41
 *              S2HostS2F42
 *              S2HostS4F111
 *              S2HostS4F112
 *              S2HostS4F113
 *              S2HostS4F114
 *              S2HostS5F1
 *              S2HostS6F13
 *              S2HostSxFx
 *
 * Description: Declarations of local variables and funtions
 *              for SECS module
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef  _H_S2LOCAL_H
#define  _H_S2LOCAL_H

/********** DEFINES **********/

#define SECS_HEADER_SIZE  10
#define MIN_MESSAGE_BYTE  SECS_HEADER_SIZE
#define MAX_MESSAGE_BYTE  254
#define MAX_DEVICES_S  	25
#define TAG_DATA_SIZE     16                // Smart-Comm SECS Msg Set Rev 1.3
#define MASK_PORT_G       0x0700            // Port G is 7 in bits 8 - 11
#define NUMBER_OF_DEV_ID 8

#define WAIT_SEND	345
#define WAITING_S2F68	456
#define WAITING_CEID1	457

// used when it is device
#define SECS_LIST       0x00
#define SECS_BINARY		0x20
#define SECS_BOOLEAN    0x24
#define SECS_ASCII		0x40
#define SECS_JIS8		0x44
#define SECS_INT8		0x60
#define SECS_INT1		0x64
#define SECS_INT2		0x68
#define SECS_INT4		0x70
#define SECS_FLOAT8		0x80
#define SECS_FLOAT4		0x90
#define SECS_UINT8		0xA0
#define SECS_UINT1		0xA4
#define SECS_UINT2		0xA8
#define SECS_UINT4		0xB0
#define SECS_PASS_THROUGH    0xFF

#define NO_SLOT_INFO    345

/********** VARIABLES USED LOCALLY **********/

typedef union uSysBytesName
{
    unsigned long   u_ulSystemBytes;
	struct
	{
        BYTE um_bSystem1;
        BYTE um_bSystem2;
        BYTE um_bSystem3;
        BYTE um_bSystem4;
    } stSysBytes;

} uSysBytes;

typedef struct tgSecsHeader
{
    BYTE m_bUpperDevID: 7;        // 0???????
    BYTE m_bRbit  : 1;            // ?0000000
    BYTE m_bLowerDevID;
    BYTE m_bStream : 7;           // 0???????
    BYTE m_bWbit   : 1;           // ?0000000
    BYTE m_bFunction;
    BYTE m_bUpperBlockNumber : 7; // 0???????
    BYTE m_bEbit         : 1;     // ?0000000
    BYTE m_bLowerBlockNumber;
    uSysBytes m_uSystemBytesUnion;
}  stSecsHeader, *npstSecsHeader, *fpstSecsHeader;

// AmatX specific      FluroTrac Tagging
enum
{
    eS4F111Sent = 0,
    eS4F112Received,
    eS4F113Sent,
    eS4F114Received
};

#define NUMBER_OF_FLUOROTRAC_DATA (eS4F114Received + 1)

// FluoroTrac Tag Data
typedef struct tgFluoroTracTagData
{
    WORD     m_wDevID;                     // Format: A9 Device ID
    BYTE     m_bTagLog;                    // Format: A5 bin number (1-255)
    BYTE     m_bPageNumber;                // Format: A5 'A' to 'P'
    BYTE     m_bAck6;                      // Format: A5 (0-3) & 0xFF
    char     m_cFTTagData[TAG_DATA_SIZE];  // Format: 41 ASCII
}  stFluoroTracTagData, *fpstFluoroTracTagData;

typedef struct tgSecsInfo
{
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
} stSecsInfo, *pstSecsInfo, *fpstSecsInfo;

typedef enum eDimTag
{
    SECS_MDLN,
	SECS_SOFTREV,
	SECS_ECID,
	SECS_ECV,
	SECS_EAC,
	SECS_RCMD,
	SECS_PORT,
	SECS_FPORT,
	SECS_TPORT,
	SECS_ANGLE,
	SECS_FSLOTS,
	SECS_TSLOTS,
	SECS_FSLOT,
	SECS_WAFSTAT,
	SECS_TSLOT,
	SECS_ORG_PORT,
	SECS_ORG_SLOT,
	SECS_CMDA,
	SECS_RCMD1,
	SECS_LLK,
	SECS_CMDA1,
	SECS_ALCD,
	SECS_ALID,
	SECS_ALTX,
	SECS_STATUS,
	SECS_CASSETTE,
	SECS_SLOTS,
	SECS_ORGPORT,
	SECS_ORGSLOT,
	SECS_WAFSTAT1,
	SECS_CEID,
	SECS_SLOT,
	SECS_LDST,
	SECS_WFST,
	SECS_FRPT,
	SECS_TOPT,
	SECS_SPARE,
} enumTypeDim;

typedef struct tagDim
{
    enumTypeDim     eDim;
    unsigned char   ucSecsType;
    int             iNumOfLengthBytes;
    int             iDataLength;
} stDim, *pstDim;


/********** FUNCTION PROTOTYPES **********/

//function which returns pointer to SecsInfo structure for the first layer (COM1,2,3)
fpstSecsInfo S2GetSecsInfoCom(int iPortNumArg);
//function which returns pointer to SecsInfo structure for the second leayer (Device1,2,3,4,5,6,7,8)
fpstSecsInfo S2GetSecsInfoDev(unsigned uDevIDArg);
int S2PrepareSecsMsg(int iPortNumArg, unsigned int uiStreamArg, unsigned char ucFunctionArg,
                     unsigned char ucEbitArg,unsigned char ucWbitArg,unsigned char ucPsmgBlkArg,
                     unsigned char *fpucMsgLenArg, unsigned int uiSystemBytesArg);
int S2InitFluoroTracTagData(int iIndexArg);

int S2InitStructMembers(int iTypeArg, int iIndexArg);
int S2HostS1F1(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumberArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);
int S2HostS1F2(fpstSecsInfo fpstTempDevArg, int iRegisterNumber);
int S2HostS1F5(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumberArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);
int S2HostS1F6(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iParameterArg,
                int iRegisterNum1, int iRegisterNum2, int iRegisterNum3);
int S2HostS2F13(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                   unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg );
int S2HostS2F15(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                   unsigned uDevIDArg, int *ipSetPCInstrArg, int *ipPCWaitingArg );
int S2HostS2F16(fpstSecsInfo fpstTempDevArg, int iRegisterNum);
int S2HostS2F37(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
		   unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);
int S2HostS2F38(fpstSecsInfo fpstTempDevArg, int iRegisterNumArg);

int S2HostS2F41(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                    unsigned uDevIDArg, int iParameterArg, int iPosition, int *ipSetPCInstrArg,
                    int *ipPCWaitingArg);
int S2HostS2F42(fpstSecsInfo fpstTempDevArg, int iRegisterNumArg);
int S2HostS4F111(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg,
                    unsigned uDevIDArg, int iTagLocArg, int iPageNumberArg, unsigned uFluoroTracDevID,
                    int *ipSetPCInstrArg, int *ipPCWaitingArg);
int S2HostS4F112(int iRegisterNum1Arg, int iRegisterNum2Arg);
int S2HostS4F113(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg, unsigned uDevIDArg,
                    int iTagLocArg, int iPageNumberArg, char *pcStringArg, unsigned uFluoroTracDevID, int *ipSetPCInstrArg,
                        int *ipPCWaitingArg);
int S2HostS4F114(int iRegisterNumArg);
int S2HostS5F1(fpstSecsInfo fpstTempDevArg,int iRegisterNumArg);
int S2HostS6F4(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumberArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);
int S2HostS6F12(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumberArg,
                unsigned uDevIDArg, int iParameterArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);
int S2HostS6F13(fpstSecsInfo fpstTempDevArg, int iRegisterNumArg, int iParameterArg);
int S2HostSxFx(fpstSecsInfo fpstTempComArg, fpstSecsInfo fpstTempDevArg, int iPortNumArg, unsigned uDevID,
                int iParameterArg, int iRegisterNum1Arg, int iRegisterNum2Arg, int iRegisterNum3Arg,
                int iRegisterNum4Arg, char *pcStringArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);

//declarations for SECS as device
int S2CreateMessageItem(unsigned char *pucReturnBufferArg, unsigned uTypeArg, int iNumOfLengthBytesArg,
                        unsigned uDataLengthArg, unsigned char *pucPureDataArg);
int S2GetMessageItem(unsigned char *pucDataArg, unsigned uTypeArg, int iNumOfLengthBytesArg,
                                        unsigned uDataLengthArg, unsigned char *pucReturnBufferArg);
int S2ReadDimensionTable(unsigned char *pucReturnBufferArg, enumTypeDim eDim, unsigned char *pucBufferArg);
void S2CreateCommand(char *pcReturnString, char cFStn, int iFSlot, int iWafStat, char cTStn, int iTSlot,
                            int iAngle, int iOrgPort, int iOrgSlot);
int S2ReadStatusAfterS2F67(int iIndexArg);
int S2SecsDeviceCommand(int iPortNumArg, unsigned int uDevIDArg, int iStreamArg, int iFunctionArg, int *ipParametersArg,
                    int *iSetPCInstrArg);
int S2InterpretSecsMsgDevice(int iPortNumArg);
int S2PrepareSecsMsgDevice(int iPortNumArg, unsigned char ucStreamArg, unsigned char ucFunctionArg,
            unsigned char ucEbitArg, unsigned char ucWbitArg, unsigned char *fpucMsgBlockArg,
            unsigned int uiMsgLenghtArg, unsigned long lSystemBytesArg);
int S2PutTogetherS2F66Response(int iPortNumArg, char *caStrArg);
int S2DeviceS1F1FromController(int iPortNumArg);
int S2DeviceS1F2FromController();
int S2DeviceS2F14FromController();
int S2DeviceS2F14FromController();
int S2DeviceS2F16FromController();
int S2DeviceS2F66FromController(int iPortNumArg, int *ipParametersArg);
int S2DeviceS5F1FromController(int iPortNumArg, int *ipParametersArg);
int S2DeviceS6F65FromController(int iPortNumArg);
int S2DeviceS6F69FromController(int iPortNumArg, int *ipParametersArg);
int S2DeviceS6F71FromController(int iPortNumArg, int *ipParametersArg);
int S2DeviceS1F1FromHost(int iPortNumArg);
int S2DeviceS1F2FromHost();
int S2DeviceS2F13FromHost(int iPortNumArg);
int S2DeviceS2F15FromHost(int iPortNumArg);
int S2DeviceS2F41FromHost(int iPortNumArg);
int S2DeviceS2F65FromHost(int iPortNumArg);
int S2DeviceS2F68FromHost(int iPortNumArg);
int S2DeviceS6F67FromHost(int iPortNumArg);

#endif
