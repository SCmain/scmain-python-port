/****************************************************************
 *
 * Program:     Controller firmware
 * File:        commL.h
 * Functions:   SERCheckForEOT
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
 * Description: file global variables and defines
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#ifndef _H_COMM_LOCAL_H
#define _H_COMM_LOCAL_H

// PMP - undefine COMPORT6
#undef COMPORT6
//#define COMPORT6

#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/signal.h>

/********** DEFINES **********/
#define A_OK	0
#define STATUS_ERR	1
#define IN_Q_EMPTY	2
#define OUT_Q_FULL	3
#define CTS		4


/* AST COMM Status bits. These set the internal COM status word only. */
#define COM_DEV_ERR     0x0001    /* Device Error */
#define COM_RX_TO       0x0002    /* Receive Time Out error */
#define COM_TX_TO       0x0004    /* Transmit Time Out error */
#define COM_OVR_ERR     0x0008    /* Overrun error */
#define COM_PAR_ERR     0x0010    /* Parity error */
#define COM_FRM_ERR     0x0020    /* Framing error */
#define COM_BREAK       0x0040    /* Break */
#define COM_LOST_ERR    0x0080    /* Blaise "INPUT_LOST" error */
#define COM_ASYN_ERR    0x0100    /* Blaise "ASYNCH_ERROR" error */
#define COM_RX_RDY      0x0200    /* Receiver Ready */
#define COM_TX_RDY      0x0400    /* Transmitter Ready */

#define TX_TIMEOUT  6000    /* transmit timeout is 60 sec */
#define RX_TIMEOUT  6000    /* receive timeout is 60 sec */

//#define RX_BUF_SIZE 275     /* Receive buffer size in bytes */
//#define TX_BUF_SIZE 275     /* Transmit buffer size in bytes */
#define RX_BUF_SIZE 1024    /* Receive buffer size in bytes */
#define TX_BUF_SIZE 1024    /* Transmit buffer size in bytes */

/* ASYNCH Manager constants used to set comm parameters. */
#define BAUD_RATE_OPT   1
#define PARITY_OPT      2
#define DATA_BITS_OPT   3
#define STOP_BITS_OPT   4
#define REM_FLOW_CTRL_OPT   5
#define LOC_FLOW_CTRL_OPT   6
#define BIT_7_TRIM_OPT  7
#define BIT_7_FORCE_OPT 8
#define REQ_CTS_OPT     9
#define SET_IRQ_VECT_OPT        11
#define LOC_HARD_FLOW_CTRL_OPT  12
#define RDY_LOC_HARD_FLOW_CTRL_OPT      13
#define UNRDY_LOC_HARD_FLOW_CTRL_OPT    14
#define CLOSE_LOC_HARD_FLOW_CTRL_OPT    15
#define ACT_MODEM_STAT_REG              16
#define SIM_MODEM_STAT_REG_RESET_DELTA  17
#define SIM_MODEM_STAT_REG              18


/********** VARIABLES USED LOCALLY **********/

/* structure to hold COM port status info */
struct CommStr
{
    int m_iFD;		    /* device fd from open device "/dev/ttyS0" */
//    int m_iTTY;		    /* console port id "/dev/tty" */
    struct termios m_OldTIO;/* terminal io structure for current setting */
    struct termios m_NewTIO;/* terminal io structure for new console setting */
//    struct termios m_OldKey;/* terminal io for current keyboard setting */
//    struct termios m_NewKey;/* terminal io for new keyboard setting */
//    struct sigaction m_saIO;/* signal action for each port communication */
    int m_iStatus;          /* port status word */
    char *m_cpRxGet;        /* Input ring-buffer TAIL pointer */
    char *m_cpRxPut;        /* Input ring-buffer HEAD pointer */
    char *m_cpRxEnd;        /* Pointer to end of internal input buffer.
                             * Used to wrap-around the head and tail pointers. */
    char m_caRxBuf[RX_BUF_SIZE];    /* Internal input RING-buffer */
    char *m_cpTxGet;        /* Output ring-buffer TAIL pointer */
    char *m_cpTxPut;        /* Output ring-buffer HEAD pointer */
    char *m_cpTxEnd;        /* Pointer to end of internal input buffer.
                             * Used to wrap-around the head and tail pointers. */
    char m_caTxBuf[TX_BUF_SIZE];    /* Internal output RING-buffer */
    int m_iBaud;            /* Current baud rate */
    int m_iParity;          /* Current parity setting */
    int m_iDataBits;        /* Current data bits setting */
    int m_iStopBits;        /* Current stop bits setting */
    int m_iFlowCtrl;        /* TRUE to require CTS high before sending */
    int m_iEcho;            /* TRUE to echo chars coming in */
    int m_iSECS;            /* TRUE if port is to be used for SECS communication */
    int m_iPortOpen;        /* Flag indicating the port has been successfully opened.
                             *
                             * WARNING!!! CAUTION!!!
                             *
                             * This flag can indicate that the port is open by using
                             * SERSetPortOpen even though SERInitPort may not have been called. */
};

/* For debugging mainly */
//#ifdef DEBUG
//int iLocal = TRUE;      /* TRUE to use kybd for COM1 commands. */
//#else
//int iLocal = FALSE;     /* FALSE to use COM1. */
//#endif

int iaTimerIndex[6];    /* The timeout counter number. This is checked out from
                         * the timer module in the com initialization. */
int giCmdPort = COM1;    /* Current port that commands come from. */
int giTTPort = COM2;     /* Current teach terminal port. Can be COM1 for EQT. */

struct CommStr stCommData[6];    /* Port data structures. This is the core of the
                                  * comm module. Each of these array elements holds
                                  * all the essential information that defines a
                                  * port's behavior, including the internal ring-buffer. */

/* Data buffers for COMM port interrupt routines. These are given to
 * the ASYNCH Manager to use. */
char caRXCommBuf[6][RX_BUF_SIZE];
char caTXCommBuf[6][TX_BUF_SIZE];
int  giRXNumCharLeft[6] = { 0, 0, 0, 0, 0, 0 };
int  giRXCommBufIndex[6] = { 0, 0, 0, 0, 0, 0 };
int  giTXNumCharLeft[6] = { 0, 0, 0, 0, 0, 0 };
int  giTXCommBufIndex[6] = { 0, 0, 0, 0, 0, 0 };
//char caCommBuf2[RX_BUF_SIZE + TX_BUF_SIZE];
//char caCommBuf3[RX_BUF_SIZE + TX_BUF_SIZE];

/* End-of-transmission character */
char cEOTChar = '\r';
/* Ignore character */
char cIGNOREChar = '\n';
/* Daisy-chain-mode values */
int  iDCMode = FALSE;
/* Used to track a CTS/RTS drop. This is used when the teach pendant is un/re-plugged.
 * It causes the XyZ Logo screen to reappear and adjusts the command port appropriately. */
int iPrevCTSCOM1 = -1;
int iPrevCTSCOM2 = -1;
int iPrevCTSCOM3 = -1;
int iPrevCTSCOM4 = -1;
int iPrevCTSCOM5 = -1;
int iPrevCTSCOM6 = -1;

#ifdef COMPORT6
char caCommDevName[6][18] = 
	{ "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyPS0", "/dev/ttyPS1", 
	  			      "/dev/ttyPS2", "/dev/ttyPS3" };
#else
char caCommDevName[6][18] = 
	{ "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "dummy", "dummy" }; 
#endif



int SERCheckForEOT(struct CommStr *pCommDataArg);

int SERCheckValidPort(int iPortNumArg);

int SERFetchRxCh(struct CommStr *pCommDataArg, char *cCharToFetch);

int SERStoreRxCh(struct CommStr *pCommDataArg, char cCharToStore);

int SERBackspRxCh(struct CommStr *pCommDataArg);

int SERFetchTxCh(struct CommStr *pCommDataArg, char *cCharToFetch);

int SERStoreTxCh(struct CommStr *pCommDataArg, char cCharToStore);

int SERSetInternalCommStatus(int iPortNumArg, int iStatus);

int SERSetCommErr(int iPortNumArg);

int SERGetSECSMsg(int iPortNumArg);

int SERSendSECSMsg(int iPortNumArg);

int SERSetSECSStatus(int iPortNumArg, int iCurrActivityStatArg, int iLastErrorStatArg, char cAckNakStatArg, int iGetStatArg);

#endif
