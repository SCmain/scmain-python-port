/****************************************************************
 *
 * Program:     Controller firmware
 * File:        ttscrL.h
 * Functions:   none
 * Description: Locally defined variables and functions that support
 *      the teach terminal screen module
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#ifndef _H_TTSCR_LOCAL_H
#define _H_TTSCR_LOCAL_H

/********** DEFINES **********/

/* The teach pendant screen is 4 by 20 characters. */
#define  MINROW     1
#define  MINCOL     1
#define  MAXROW     4
#define  MAXCOL     20

#define  SET_CURSOR_STR_BEG "\x1B["     /* Set Cursor String Beginning
                                         * Used to initiate "position cursor" command */
#define  SET_CURSOR_STR_END "H"         /* Set Cursor String End
                                         * Used to terminate "position cursor" command */
#define  CLRSTR             "\x1B[2J"   /* TP command string to clear LCD screen */
#define  SEMICOLON          ";"
#define  NEW_TP_ID          "\x1B[0xEQUIPE45X 8912\x1B[1x" /* Used to detect a new teach pendant */
//#define  OLD_TP_ID          "\x1B[0x EQP45X 7F8C\x1B[1x" /* Used to detect a old teach pendant */
#define  ID_LENGTH          16


/********** VARIABLES USED LOCALLY **********/

/* String used to set the Type mode (of cursor) */
static char SET_CURSOR_TYPE_STR[4][5] =
{
    "\x1B[4t",      /* Enable cursor */
    "\x1B[5t",      /* Disable cursor */
    "\x1B[6t",      /* Enable cursor blink */
    "\x1B[7t"       /* Disable cursor blink */
};

/* String used to set the Beep mode (of teach pendant) */
static char BEEP_STR[4][5] =
{
    "\x1B[0q",      /* Shortbell */
    "\x1B[1q",      /* Longbell */
    "\x1B[2q",      /* Alert */
    "\x1B[3q"       /* Key click */
};

/* String used to set the Key Repeat mode(?) (of teach pendant) */
static char KEY_REPEAT_STR[2][6] =
{
    "\x1B[?8l",     /* Disable key repeat */
    "\x1B[?8h"      /* Enable key repeat */
};

/* String used to set echo mode (of OLD teach pendant). This mode is used
 * mainly in teach, diagnostics, and terminal mode. When a key is pressed,
 * that key is transmitted to the controller but is also locally echoed
 * to the teach pendant LCD screen. This way the Comm module doesn't have
 * to echo the keypress back to the teach pendant. */
static char ECHO_STR[2][21] =
{
    "\x1B[1z;;;;;;;;;;;D\x1B[0z",   /* Disable echo mode */
    "\x1B[1z;;;;;;;;;;;E\x1B[0z"    /* Enable echo mode */
};

/* Same as ECHO_STR except it is customized for the NEW teach pendant. */
static char ECHO_STR_NEW[2][5] =
{
    "\x1B[2v",
    "\x1B[1v"
};

/* String used to set the teach pendant automatic carriage return/line feed.
 * When local echo is used, the teach pendant has to perform carriage returns
 * and line feeds. Normally the Comm module echo would take care of that. */
static char CR_LF_MODE_STR[2][6] =
{
    "\x1B[20l",
    "\x1B[20h"
};

/* String used to the get the teach pendant ID number. This is used to
 * differentiate between the old and new teach pendants so that the correct
 * echo mode activation string can be used. */
static char REPORT_ID_STR[1][4] =
{
    "\x1B[p"
};

/* String used to set Key-Not-Pressed mode (of teach pendant)
 * This mode is used mainly in teach mode. When a key is pressed,
 * that key is transmitted to the controller. But when it is released
 * a 0 is sent to controller also. That way jogging can be done smoothly. */
static char KNP_STR[2][6] =
{
    "\x1B[3t",      /* Disable KNP */
    "\x1B[2t"       /* Enable KNP */
};

/* String used to set the Key Click mode (of teach pendant)
 * This causes the teach pandant to make a clicking noise
 * every time a key is pressed. */
static char KEY_CLICK_STR[2][6] =
{
    "\x1B[1t",      /* Disable key click */
    "\x1B[0t"       /* Enable key click */
};

int iCursorType = 0;    /* Initialized to enable cursor */
int iBeepType = 0;      /* Initialized to shortbell */
int iEchoMode = 0;      /* Initialized to echo mode Off */
int iKNPMode = 0;       /* Initialized to KNP mode Off */
int iKeyRepeatMode = 0; /* Initialized to NOT repeat key presses */
int iKeyClickMode = 0;  /* Initialized to NOT click on each key press */
int iOldTP = 0;         /* Initialized to NEW teach pendant */

#endif
