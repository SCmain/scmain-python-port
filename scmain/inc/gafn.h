/***************************************************************\
 *
 * Program:     Motion Control local header
 * File:        gafn.h
 * Functions:   GAInitTimerCounters
 *              GAInitStatusWord
 *              GAIsEndOfString
 *              GABuildAxesArgumentsCommand
 *              GABuildParameterArgumentsCommand
 *              GABuildDoubleParameterArgumentsCommand
 *              GABuildInterrogationCommand
 *              GAConvertAsciiToDouble
 *              GAConvertAsciiToLong
 *              GAReadGalilRevisionInfo
 *              GAValidateCardNo
 *              GAValidateAxis
 *              GAValidateInterrupt
 *              GAClearReadGalilFIFOBuffer
 *              GAClearWriteGalilFIFOBuffer
 *              GATurnOnGalilInt
 *              GASetUpInterruptVector
 *              GA_PROC
 *              GA_PROC1
 *              GASetCommandString
 *
 * Description: Provide interface for variables and routines
 *      used locally in the Motion Control module.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_GALOCAL_H
#define _H_GALOCAL_H

/********** DEFINES **********/
/* Timeout values. */
#define GA_TIMEOUT               50      /* 50 ms time-out for GALIL card */
#define GA_CLEAR_WRITE_FIFO_WAIT 20      /* Wait for 20 msec after Clear Write FIFO buffer */
#define GA_WAIT_20_MSECS         20      /* Wait for 20 msec */
#define GA_WAIT_50_MSECS         50      /* Wait for 50 msec */
#define GA_WAIT_250_MSECS        25     /* Wait for 250 msec */

/* Galil responses. */
#define EOI_CHAR           ((unsigned char)':')
#define EOI_STRING         ((char *)":")

/* Galil identification strings. */
#define DMC82XX   "DMC82"
#define DMC8230   "DMC8230"
#define DMC8240   "DMC8240"
//VAC514
#define LATEST_GALIL_VER_STRING     "2.2d"

#define VER_TOKEN "ver"
#define REV_TOKEN "rev"

/* Galil card interrupt masks. */
#define   GA_INTNO	0xF
#define   GA_MASK   0x80

#define   GA1_INTNO	0xD
#define   GA1_MASK  0x20

/* Lengths of status words returned by Galil. */
#define   SS_RESPONSE_LEN  5
#define   EI_COMMAND_LEN  5

/* Maximum Galil cards allowed. */
#define MAX_GALIL_CARDS 2

/* Minimum Galil cards allowed. */
#define MIN_GALIL_CARDS 1

typedef union tgSSBitStructTag
{
    unsigned char     m_ucSSBytes[SS_RESPONSE_LEN];
    struct
    {
        unsigned       m_uAfterSpeedNotify     : 4;     // Byte 0 - Bit 0-3
        unsigned                               : 4;     // Byte 0 - Bit 4-7
        unsigned       m_uNegativeLimitSwitches: 4;     // Byte 1 - Bit 0-3
        unsigned       m_uPositiveLimitSwitches: 4;     // Byte 1 - Bit 4-7
        unsigned       m_uPositionErrors       : 4;     // Byte 2 - Bit 0-3
        unsigned       m_uMotionComplete       : 4;     // Byte 2 - Bit 4-7
        unsigned       m_uAfterPositionNotify  : 4;     // Byte 3 - Bit 0-3
        unsigned       m_uAfterDistanceNotify  : 4;     // Byte 3 - Bit 4-7
        unsigned       m_uKI_Overflow          : 4;     // Byte 4 - Bit 0-3
        unsigned       m_uReserved_nible4_1    : 4;     // Byte 4 - Bit 4-7
    };

    struct
    {
        unsigned       m_X_AfterSpeedNotify    : 1;     // Byte 0 - Bit 0
        unsigned       m_Y_AfterSpeedNotify    : 1;     // Byte 0 - Bit 1
        unsigned       m_Z_AfterSpeedNotify    : 1;     // Byte 0 - Bit 2
        unsigned       m_W_AfterSpeedNotify    : 1;     // Byte 0 - Bit 3
        unsigned       m_ucReserved_bit_0_4    : 1;     // Byte 0 - Bit 4
        unsigned       m_ucReserved_bit_0_5    : 1;     // Byte 0 - Bit 5
        unsigned       m_ucReserved_bit_0_6    : 1;     // Byte 0 - Bit 6
        unsigned       m_ucReserved_bit_0_7    : 1;     // Byte 0 - Bit 7

        unsigned       m_X_NegativeLimitSwitch : 1;     // Byte 1 - Bit 0
        unsigned       m_Y_NegativeLimitSwitch : 1;     // Byte 1 - Bit 1
        unsigned       m_Z_NegativeLimitSwitch : 1;     // Byte 1 - Bit 2
        unsigned       m_W_NegativeLimitSwitch : 1;     // Byte 1 - Bit 3
        unsigned       m_X_PositiveLimitSwitch : 1;     // Byte 1 - Bit 4
        unsigned       m_Y_PositiveLimitSwitch : 1;     // Byte 1 - Bit 5
        unsigned       m_Z_PositiveLimitSwitch : 1;     // Byte 1 - Bit 6
        unsigned       m_W_PositiveLimitSwitch : 1;     // Byte 1 - Bit 7

        unsigned       m_X_PositionError       : 1;     // Byte 2 - Bit 0
        unsigned       m_Y_PositionError       : 1;     // Byte 2 - Bit 1
        unsigned       m_Z_PositionError       : 1;     // Byte 2 - Bit 2
        unsigned       m_W_PositionError       : 1;     // Byte 2 - Bit 3
        unsigned       m_X_MotionComplete      : 1;     // Byte 2 - Bit 4
        unsigned       m_Y_MotionComplete      : 1;     // Byte 2 - Bit 5
        unsigned       m_Z_MotionComplete      : 1;     // Byte 2 - Bit 6
        unsigned       m_W_MotionComplete      : 1;     // Byte 2 - Bit 7

        unsigned       m_X_AfterPositionNotify : 1;     // Byte 3 - Bit 0
        unsigned       m_Y_AfterPositionNotify : 1;     // Byte 3 - Bit 1
        unsigned       m_Z_AfterPositionNotify : 1;     // Byte 3 - Bit 2
        unsigned       m_W_AfterPositionNotify : 1;     // Byte 3 - Bit 3
        unsigned       m_X_AfterDistanceNotify : 1;     // Byte 3 - Bit 4
        unsigned       m_Y_AfterDistanceNotify : 1;     // Byte 3 - Bit 5
        unsigned       m_Z_AfterDistanceNotify : 1;     // Byte 3 - Bit 6
        unsigned       m_W_AfterDistanceNotify : 1;     // Byte 3 - Bit 7

        unsigned       m_X_KI_Overflow         : 1;     // Byte 4 - Bit 0
        unsigned       m_Y_KI_Overflow         : 1;     // Byte 4 - Bit 1
        unsigned       m_Z_KI_Overflow         : 1;     // Byte 4 - Bit 2
        unsigned       m_W_KI_Overflow         : 1;     // Byte 4 - Bit 3
        unsigned       m_ucReserved_bit_4_4    : 1;     // Byte 4 - Bit 4
        unsigned       m_ucReserved_bit_4_5    : 1;     // Byte 4 - Bit 5
        unsigned       m_ucReserved_bit_4_6    : 1;     // Byte 4 - Bit 6
        unsigned       m_ucReserved_bit_4_7    : 1;     // Byte 4 - Bit 7
    };
} stSSBitStruct,  *fpstSSBitStruct;

typedef union  tgEIBitStructTag
{
    unsigned char     m_ucEIBytes[EI_COMMAND_LEN];
    struct
    {
        unsigned       m_uAfterSpeedNotify     : 4;     // Byte 0 - Bit 0-3
        unsigned                               : 4;     // Byte 0 - Bit 4-7
        unsigned       m_uNegativeLimitSwitches: 4;     // Byte 1 - Bit 0-3
        unsigned       m_uPositiveLimitSwitches: 4;     // Byte 1 - Bit 4-7
        unsigned       m_uPositionErrors       : 4;     // Byte 2 - Bit 0-3
        unsigned       m_uMotionComplete       : 4;     // Byte 2 - Bit 4-7
        unsigned       m_uAfterPositionNotify  : 4;     // Byte 3 - Bit 0-3
        unsigned       m_uAfterDistanceNotify  : 4;     // Byte 3 - Bit 4-7
        unsigned       m_uKI_Overflow          : 4;     // Byte 4 - Bit 0-3
        unsigned       m_uReserved_nible4_1    : 4;     // Byte 4 - Bit 4-7
    };

    struct
    {
        unsigned       m_X_AfterSpeedNotify    : 1;     // Byte 0 - Bit 0
        unsigned       m_Y_AfterSpeedNotify    : 1;     // Byte 0 - Bit 1
        unsigned       m_Z_AfterSpeedNotify    : 1;     // Byte 0 - Bit 2
        unsigned       m_W_AfterSpeedNotify    : 1;     // Byte 0 - Bit 3
        unsigned       m_ucReserved_bit_0_4    : 1;     // Byte 0 - Bit 4
        unsigned       m_ucReserved_bit_0_5    : 1;     // Byte 0 - Bit 5
        unsigned       m_ucReserved_bit_0_6    : 1;     // Byte 0 - Bit 6
        unsigned       m_ucReserved_bit_0_7    : 1;     // Byte 0 - Bit 7

        unsigned       m_X_NegativeLimitSwitch : 1;     // Byte 1 - Bit 0
        unsigned       m_Y_NegativeLimitSwitch : 1;     // Byte 1 - Bit 1
        unsigned       m_Z_NegativeLimitSwitch : 1;     // Byte 1 - Bit 2
        unsigned       m_W_NegativeLimitSwitch : 1;     // Byte 1 - Bit 3
        unsigned       m_X_PositiveLimitSwitch : 1;     // Byte 1 - Bit 4
        unsigned       m_Y_PositiveLimitSwitch : 1;     // Byte 1 - Bit 5
        unsigned       m_Z_PositiveLimitSwitch : 1;     // Byte 1 - Bit 6
        unsigned       m_W_PositiveLimitSwitch : 1;     // Byte 1 - Bit 7

        unsigned       m_X_PositionError       : 1;     // Byte 2 - Bit 0
        unsigned       m_Y_PositionError       : 1;     // Byte 2 - Bit 1
        unsigned       m_Z_PositionError       : 1;     // Byte 2 - Bit 2
        unsigned       m_W_PositionError       : 1;     // Byte 2 - Bit 3
        unsigned       m_X_MotionComplete      : 1;     // Byte 2 - Bit 4
        unsigned       m_Y_MotionComplete      : 1;     // Byte 2 - Bit 5
        unsigned       m_Z_MotionComplete      : 1;     // Byte 2 - Bit 6
        unsigned       m_W_MotionComplete      : 1;     // Byte 2 - Bit 7

        unsigned       m_X_AfterPositionNotify : 1;     // Byte 3 - Bit 0
        unsigned       m_Y_AfterPositionNotify : 1;     // Byte 3 - Bit 1
        unsigned       m_Z_AfterPositionNotify : 1;     // Byte 3 - Bit 2
        unsigned       m_W_AfterPositionNotify : 1;     // Byte 3 - Bit 3
        unsigned       m_X_AfterDistanceNotify : 1;     // Byte 3 - Bit 4
        unsigned       m_Y_AfterDistanceNotify : 1;     // Byte 3 - Bit 5
        unsigned       m_Z_AfterDistanceNotify : 1;     // Byte 3 - Bit 6
        unsigned       m_W_AfterDistanceNotify : 1;     // Byte 3 - Bit 7

        unsigned       m_X_KI_Overflow         : 1;     // Byte 4 - Bit 0
        unsigned       m_Y_KI_Overflow         : 1;     // Byte 4 - Bit 1
        unsigned       m_Z_KI_Overflow         : 1;     // Byte 4 - Bit 2
        unsigned       m_W_KI_Overflow         : 1;     // Byte 4 - Bit 3
        unsigned       m_ucReserved_bit_4_4    : 1;     // Byte 4 - Bit 4
        unsigned       m_ucReserved_bit_4_5    : 1;     // Byte 4 - Bit 5
        unsigned       m_ucReserved_bit_4_6    : 1;     // Byte 4 - Bit 6
        unsigned       m_ucReserved_bit_4_7    : 1;     // Byte 4 - Bit 7
    };
} stEIBitStruct,  *fpstEIBitStruct;

/* Motion command queue structure. */
typedef struct tgMcCommandTag
{
    int       m_StartQueuing;               /* TRUE, start Queing the Command String */
    int       m_iNCmds;                     /* n_cmds: # of cmd replies to expect */
    char      m_cCmdStr[256];               /* cmdstr[]: cmd string for Motion Card */
} stGACommandStr,  *fpstGACommandStr;

typedef void (*VOIDFUNCPTR) (int);          /* ptr to function returning void */


/********** VARIABLES USED LOCALLY **********/
extern int GAiUseGalil;
extern int GAiUseInts;
extern unsigned MCuGalilError;
extern int GAiDefaultIRQ7;
extern int GAiExcIntOutp;
extern int GAiNumberOfGalilCards;
extern int GAiPreOnly;
extern unsigned GAiDebugBit;

extern int GL_TIMER[];
extern int GA_TIMER;

extern unsigned GALIL_RW[];
extern unsigned GALIL_ST[];
extern unsigned GALIL_CLR_W[];
extern unsigned GALIL_CLR_R[];
extern unsigned GL_TIME_ERR[];
extern unsigned GL_RPLY_ERR[];
extern unsigned GL_REENT_ERR[];
extern unsigned GL_WRITE_DATA[];
extern unsigned GL_DEBUG_MODE[];
extern int GL_NUM_AXIS[];

extern VOIDFUNCPTR MEPROCCallBackFuncPtr[];

extern stGACommandStr stGAQueueCmdString[];


/********** FUNCTIONS USED LOCALLY **********/
/* Local functions used only in initialization. */
int GAInitTimerCounters(void);
void GAInitStatusWord(void);

/* Are we at the end of the Galil return string? */
int GAIsEndOfString(int iCardNoArg, int *iEndOfString);

/* General purpose routines to build Galil command strings. */
int GABuildAxesArgumentsCommand(int iCardNoArg, unsigned uGalilAxisNameArg, char *cpOpCode, int iEndOfString, char *cpCommandString);
int GABuildParameterArgumentsCommand(int iCardNoArg, unsigned uGalilAxisNameArg, long *lValue, char *cpOpCode, int iEndOfString, char *cpCommandString);
int GABuildDoubleParameterArgumentsCommand(int iCardNoArg, unsigned uGalilAxisNameArg, double *dValue, char *cpOpCode, int iEndOfString, char *cpCommandString);
int GABuildInterrogationCommand(int iCardNoArg, unsigned uGalilAxisNameArg, char *cpOpCode, int iEndOfString, char *cpCommandString);

/* General purpose routines to decode Galil command strings. */
int GAConvertAsciiToDouble(unsigned uGalilAxesArg, char *cpReadStringArg, double *dpGalilParmArg);
int GAConvertAsciiToLong(unsigned uGalilAxesArg, char *cpReadStringArg, long *lpGalilParmArg);
int GASetCommandString(int iParamDescArg, char *cpCommandString);

/* Routine to get Galil card info. */
int GAReadGalilRevisionInfo(int iCardNoArg, char *szRevision);

/* Validation routines. */
int GAValidateCardNo(int iCardNoArg);
int GAValidateAxis(int iCardNoArg, unsigned uGalilAxisNameArg, unsigned *puAxisName);
int GAValidateInterrupt(int *iCardNoArg);

/* Routines to clear the Galil I/O buffers. */
int GAClearReadGalilFIFOBuffer(int iCardNoArg);
int GAClearWriteGalilFIFOBuffer(int iCardNoArg);

/*** Interrupt related functions  ***/
/* Interrupt routines used only in initialization. */
int GATurnOnGalilInt(int iCardNoArg);
int GASetUpInterruptVector(int iCardNoArg);
void GA_PROC(void);
void GA_PROC1(void);

#endif // _H_GALOCAL_H
