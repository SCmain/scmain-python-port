/***************************************************************\
 *
 * Program:     SECS-II Communication Global Header file
 * File:        s2global.H
 * Functions:   S2GetLastError
 *              S2GetWTCurrentActivity
 *              S2GetNofRetries
 *              S2GetT1
 *              S2GetT2
 *              S2GetMaxRetries
 *              S2GetMsgBlock
 *              S2GetTMsgBlock
 *              S2GetMessageSent
 *              S2SetLastError
 *              S2SetWTCurrentActivity
 *              S2SetNofRetries
 *              S2SetT1
 *              S2SetT2
 *              S2SetMaxretries
 *              S2SetMsgBlock
 *              S2SetTMsgBlock
 *              S2SetMessageSent
 *              S2InitSecs
 *              S2InterpretSecsMsg
 *              S2AbortSecs
 *              S2SecsInx
 *
 * Description: Declares the SECS-II functions and variables available globally
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef  _H_S2GLOBAL_H
#define  _H_S2GLOBAL_H

/********** DEFINES **********/

// ascii definitions
#define ENQ	0x05
//#define ENQ	0x95
#define EOT	0x04
#define ACK	0x06
#define NAK     0x15

// Within Transaction activities
#define IDLE         	0
#define GOT_ENQ      	1
#define SEND_EOT     	2
#define REC_MESSAGE  	3
#define REC_LEN_BYTE  	4
#define REC_BLOCK    	5
#define REC_CHECKSUM 	6
#define REC_BAD_CHARS 	7
#define SEND_ACK     	8
#define SEND_NAK     	9
#define SEND_ENQ     	10
#define AWAIT_EOT    	11
#define DEV_CONTENTION  12
#define SEND_MESSAGE 	14
#define SEND_LEN_BYTE 	15
#define SEND_BLOCK   	16
#define SEND_CHECKSUM   17
#define AWAIT_ACK_NAK 	18
#define MSG_TO_SEND   	19
#define MSG_TO_RECEIVE	20

//Last Error constants
#define OK		0
#define S_TIMEOUT    	1
#define RETRY_ERROR  	2
#define RECEIVE_NAK  	3
#define FAILED_SEND  	4
#define INTERNAL_READ   5
#define INTERNAL_WRITE  6
#define ENQ_TIMEOUT  	7
#define EOT_TIMEOUT  	8
#define ACK_NAK_TIMEOUT 9
#define INVALID_ENQ  	10
#define TOO_MANY_BYTES  11
#define INV_LEN_BYTE  	12
#define CHECK_SUM_ERR 	14

#define  COM    1 // for Get, set member of the structure, either for layer one(COM) or two(DEV)
#define  DEV    2

#define LAST_ERROR 0
#define WTCURR_ACTIVITY 1
#define NUM_OF_RETRIES 2
#define T1 3
#define T2 4
#define MAX_RETRIES 5
#define MESSAGE_SENT 6


/********** FUNCTION PROTOTYPES **********/

// functions that allow the access to the members of the SecsInfo structure
int S2GetSecsStructureMember(int iPointerTypeArg, int iIndexArg, WORD *wReturnArg, int iMemberType);
int S2GetMsgBlock(int iPointerTypeArg, int iIndexArg, unsigned char  **wReturnArg);
int S2GetTMsgBlock(int iPointerTypeArg, int iIndexArg, unsigned char  **wReturnArg);
int S2GetTimeOut(int iPointerTypeArg, int iIndexArg, DWORD *wReturnArg);

int S2SetSecsStructureMember(int iPointerTypeArg, int iIndexArg, WORD wSetValueArg, int iMemberType);
int S2SetMsgBlock(int iPointerTypeArg, int iIndexArg, unsigned char  *wSetValueArg);
int S2SetTMsgBlock(int iPointerTypeArg, int iIndexArg, unsigned char  *wSetValueArg);
int S2SetTimeOut(int iPointerTypeArg, int iIndexArg, DWORD wSetValueArg);

//Secs initialization and allocation
int S2InitSecs(int iPortNumArg);
//function which processes the Secs message
int S2InterpretSecsMsg(int iPortNumArg);
//clears the secs status words
int S2AbortSecs(void);
int S2CheckSumOK(unsigned char  *fpucMsg);
//function which process INX command,
int S2SecsCommand(int iPortNumArg, unsigned int uDevIDArg, int iStreamArg, int iFunctionArg, int iParameterArg,
                    int *ipRegisterNumArg, char *pcStringArg, int *ipSetPCInstrArg, int *ipPCWaitingArg);

int S2GetSecsSpecial();
void S2SetSecsSpecial(int iFlagArg);

#endif
