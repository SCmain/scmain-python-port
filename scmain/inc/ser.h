/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        comm.h
 * Functions:   SERClosePorts
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
 *
 * Description: Handles low-level RS232 communications
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 * 01A      N/A     040707  Creator	    Generic communication
 *                                          interface
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 * Localized variables are:
 *      EOT_char
 *      IGNORE_char
 *      DC_mode
 *      Local
 *      Flow_ctrl
 *      UseCOM2
 *      UseCom3
 *      echo_COM2
 *
\***************************************************************/

#ifndef COMM
#define COMM

#include "serg.h"

/********** DEFINES **********/

/* Max length input line. Max buffer size. Same as RX_BUF_SIZE. */
#define MAXLINE  275

/* Descriptors to reference which values to get or set. */
#define FLOW_CTRL   0
#define ECHO_MODE   1
#define COMM_STATUS 2
#define DC_MODE     3
#define SECS_MODE   4
#define PORT_OPEN   5
#define TT_PORT     6
#define CMD_PORT    7


/********** FUNCTION PROTOTYES *********/

int SERClosePorts(int iPortNumArg);

int SERGetRawChar(int iPortNumArg,
                    char *cCharInArg,
                    int *iNumCharsLeftArg);

int SERRxBuffAvail(int iPortNumArg);

int SERGetsRxBuff(  int iPortNumArg,
                    char *cpRxBuffArg,
                    int iZFlagArg,      /* TRUE to terminate on Ctrl-Z or EOT_char */
                    int *iNumOfBytes,   /* Total number of bytes to read; returns number read */
                                        /* -1 returns as many bytes as are available */
                    int iTimeout);      /* TRUE to wait for a timeout period before exiting */

int SERGetcRxBuff(  int iPortNumArg,
                    char *cRxCharArg,
                    int iTimeout);

int SERPutsTxBuff(  int iPortNumArg,
                    char *cpTxStrArg);

int SERFlushTxBuff(int iPortNumArg);

int SERWaitTxBuff(int iPortNumArg);

int SERSetEOTChar(char cEOTCharArg);

int SERSetIGNOREChar(char cIGNORECharArg);

int SERGetCommVals(int iPortNumArg, int iValueDescArg, int *iValueArg);

int SERSetCommVals(int iPortNumArg, int iValueDescArg, int iValueArg);

int SERGetTTPort();

int SERGetCmdPort();

int SERGetNumCharsInBuffer(int iPortNumArg);

int SERPortRedirect(int iPortToRedirectToArg);

int SERGetHistoryList(int iPortNumArg);

int SERFlushComBuffer(int iPortNumArg);

int SERPutsRxBuff(int iPortNumArg, char *cpRxStrArg);

int SERGetSecsActivity(int iPortNumArg);

int COReadChar(int iPortNumArg, char *cCharInArg, int *iNumCharsLeftArg, unsigned int *iStatusArg);

int COReadString (int iPortNumArg, int iNumCharsLeftArg, char *fpcaMsgBlockArg, /*int *iRetArg,*/ int *iNumCharsLeftAfterReadArg, int *iStatusArg);


#endif
