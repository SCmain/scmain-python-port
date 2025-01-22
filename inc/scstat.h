/***************************************************************\
 *
 * Program:     Status utility interface header
 * File:        status.h
 * Functions:   SSSetStatusWord
 *              SSReadStatusBits
 *              SSGetVacuumBits
 *              SSGetSystemStatus
 *              SSGetSystemStatus2
 *              SSGetGENSystemStatus
 *              SSSetCheckSumStatus
 *              SSGetCheckSumStatus
 *              SSSetStatusWord2
 *              SSReadStatusBit2
 *              SSInitStatus
 *              SSSetModulesInitializationsStatus
 *              SSGetModulesInitializationsStatus
 *
 * Description: Maintain the system status and provide the
 *      interface for other modules to access.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_STATUS_H
#define _H_STATUS_H

/*********** DEFINES ************/
#define     CMD_NOT_EXED    0x0001      /* Previous command not executed */
#define     CMD_INVAL       0x0002      /* Previous command invalid */
#define     VACUUM_SEN      0x0004      /* Vacuum sensor is activated */
#define     VACUUM_SW       0x0008      /* Vacuum switch is on */
#define     MOT_ERROR       0x0010      /* Motor error on one or more axes */
#define     LIMIT_SW        0x0020      /* One or more limit switches are triggered */
#define     HOME_NOT_EXED   0x0040      /* One or more axes are not homed yet */
#define     ALIGNING        0x0080      /* Error on last alignment */
#define     RUNNING_MACRO   0x0100      /* Running macro */
#define     RBT_MOVING      0x0200      /* One or more axes are moving */
#define     SERVO_OFF       0x0400      /* Servo off on one or more axes */
#define     COM2_ERR        0x0800      /* Error on COM2 */
#define     AVAIL_1         0x1000      /* Not used; always 1 */
#define     NVSRAM_ST       0x2000      /* NVSRAM error */
#define     CTRL_ERR        0x4000      /* Controller error */
#define     COM1_ERR        0x8000      /* Error on COM1 */

#define     LED_ERROR       0xEC10
#define     BUSY_INDI       0x07D0

/* The following defines are used for Genmark system status */
#define     GEN_LIMIT_SW    0x0002      /* One or more limit switches are triggered */
#define     GEN_MOT_ERROR_T 0x0010      /* Motor error on one T axis */
#define     GEN_MOT_ERROR_R 0x0020      /* Motor error on one R axis */
#define     GEN_MOT_ERROR_Z 0x0040      /* Motor error on one Z axis */
#define     GEN_MOT_IDLE_T  0x0080      /* T axis is moving */
#define     GEN_MOT_IDLE_R  0x0100      /* R axis is moving */
#define     GEN_MOT_IDLE_Z  0x0200      /* Z axis is moving */
#define     GEN_PRE_VAC_SEN 0x0020      /* Prealigner vacuum sensor is activated */
#define     GEN_PRE_VAC_SW  0x0040      /* Prealigner vacuum switch is on */
#define     GEN_RBT_MOVING  0x0080      /* One or more axes are moving in an integrated system */
#define     GEN_TEACH_ACT   0x0800      /* Teach mode active, [R60]!=0 */
#define     GEN_SEARCH_ACT  0x1000      /* Search mode active, [R33]!=0 */

#define     GEN_INTEGRATED_UNUSED   0x0300  /* Bits not used in an integrated system */

/* The following 7 defines are used internally, mostly, by the Macro module.
 * MACRO_RUNNING is used by the Status module though. */
#define     MACRO_IDLE      0
#define     MACRO_RUNNING   1
#define     FROZEN          2
#define     ABORTED_BY_USER 3
#define     STACK_OVERFLOW  4
#define     OTHERS          5

/* Additional status bits; for stat2 */
#define     GA_COM_TIMEOUT      0x0001  /* Galil communication error */
#define     IO_CARD_VER_2       0x0002  /* IO card version is older than V3.xx */
#define     BAD_JUMPER_SET      0x0004  /* Jumper setting is wrong or parameter file mismatch */
#define     SCAN_START_ER       0x0008  /* Scan start position error */
#define     SPEEDCHK_DISABLE    0x0010  /* Speed check disabled */
#define     SPEED_DROP          0x0020  /* Speed drop */
#define     COM3_ERR            0x0040  /* Com3 error */
#define     OLD_GALIL_VER       0x0080  /* Galil firmware version 2.2d or older */
#define     LOW_MEMORY          0x0100  /* Low memory warning, less than 2kB */
#define     GA_REENT_ERROR      0x0200  /* Galil send/receive Re-entrant error */
#define     NON_ERROR_STAT2_BITS    (IO_CARD_VER_2|SPEEDCHK_DISABLE|OLD_GALIL_VER|LOW_MEMORY)
#define     WANT_SPEED_DROP_CHECK   1   /* if KLA wants this checking */
#define     WANT_MISMATCH_PAR_SCAN  1   /* if the mismatch doesn't bother KLA */

/* Definitions of status bits for STAT3 command */
#define     MATH_EROR_HANDLER   0x0001
#define     SYSTEM_TIMER        0x0002
#define     STATUS_MODULE       0x0004
#define     COM1_PORT           0x0008
#define     COM2_PORT           0x0010
#define     COM3_PORT           0x0020
#define     TASK_LIST           0x0040
#define     REGISTERS_TABLE     0x0080
#define     SCANNER_MODULE      0x0100
#define     DATAFILE_MODULE     0x0200
#define     MOTION_MODULE       0x0400
#define     MECHANISM_MODULE    0x0800
#define     IO_MODULE           0x1000
#define     ALIGNER_MODULE      0x2000
#define     LOW_LEVEL_MODULE    0x4000
#define     MACRO_MODULE        0x8000
#define	    COM4_PORT		0x0020
#define	    COM5_PORT		0x0020
#define	    COM6_PORT		0x0020

/* Debug option bit definition */
#define     GA_REENT_BIT        0x0001  /* Debug option bit for MC Re-entrant error */
#define     CMER_DEBUG_BIT      0x0002  /* Debug option bit for CMER during MR Execution */
#define     AL_DEBUG_BIT        0x0004  /* Debug option bit for WA during alignment */
#define     SC_FILTER_BIT       0x0008  /* Dummy(place holder for now)*/

/* The following 11 defines are for general purpose use. They are not used though. */
#define     BIT_05          0x0020
#define     BIT_06          0x0040
#define     BIT_07          0x0080
#define     BIT_08          0x0100
#define     BIT_09          0x0200
#define     BIT_10          0x0400
#define     BIT_11          0x0800
#define     BIT_12          0x1000
#define     BIT_13          0x2000
#define     BIT_14          0x4000
#define     BIT_15          0x8000


/******** FUNCTION PROTOTYPES ***********/

// Replaces: void set_status(unsigned mask, int sw)
void SSSetStatusWord(unsigned uMaskArg, int iFlagArg);

// Replaces: unsigned read_status_bits(unsigned mask)
unsigned SSReadStatusBits(unsigned uMaskArg);

// Replaces: int get_vac_bits(void)
int SSGetVacuumBits(void);

// Replaces: int get_sys_status(void)
int SSGetSystemStatus(void);

// Replaces: int get_genmark_status(int)
int SSGetGENSystemStatus(int iFromENDMFlagArg);

// Replaces: void set_chk_sum_stat(unsigned bitn, unsigned flag)   from n2.c file
void SSSetCheckSumStatus(unsigned uBitArg, unsigned uFlagArg);

// NEW function
unsigned SSGetCheckSumStatus(unsigned uMaskArg);

// Replaces: void set_stat2(unsigned mask, int sw)
void SSSetStatusWord2(unsigned uMaskArg, int iFlagArg);

// Replaces: unsigned read_stat2_bits(unsigned mask)
unsigned SSReadStatusBit2(unsigned uMaskArg);

// NEW function
int SSGetSystemStatus2(void);

// NEW function
int SSInitStatus(int iDefineFlagArg, int iEmulatorTypeArg);

// NEW functions - Get/Set for system initialization status
int SSSetModulesInitializationsStatus(int iMaskArg, int iFlagArg);
int SSGetModulesInitializationsStatus(void);

// New functions to get the general purpose debug bit on/off information.
int SSIsDebugBitOn(unsigned int uBitArg);
void SSSetDebugBit(unsigned int uBitArg);
unsigned int SSGetDebugBit( void );

#endif
