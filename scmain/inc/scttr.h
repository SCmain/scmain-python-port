/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        ttscr.h
 * Functions:   TTSetCursorPos
 *              TTSetCursorType
 *              TTClearScreen
 *              TTBeepTP
 *              TTPrintsAt
 *              TTPrintcAt
 *              TTPrintMessage
 *              TTSetEchoMode
 *              TTSetKNPMode
 *              TTSetKeyRepeat
 *              TTShowLogo
 *              TTInitTP
 *
 * Description: Handles interface to the teach pendant
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
\***************************************************************/

#ifndef _H_TTSCR_H
#define _H_TTSCR_H

/*********** ENUMS ************/
enum eCursorType
{
    ENABLE_CURSOR = 0,
    NO_CURSOR = 1,
    BLINK = 2,
    NO_BLINK = 3
};

enum eBeepType
{
    NO_BEEP = -1,
    SHORTBELL = 0,
    LONGBELL = 1,
    ALERT = 2,
    CLICK = 3,
};


/******** FUNCTION PROTOTYPES ***********/

/***************************************************************\
 *
 * Replaces: setcpos
 * Used in: diag, teach
 * Description: Sets the current cursor position on the teach
 *      pendant LCD screen.
 *
\***************************************************************/
int TTSetCursorPos(   int iRowArg,
                    int iColArg);

/***************************************************************\
 *
 * Replaces: setctype
 * Used in: diag, teach
 * Description: Sets the current cursor type on the teach pendant
 *
\***************************************************************/
int TTSetCursorType(int iCursorTypeArg);

/***************************************************************\
 *
 * Replaces: clrscr
 * Used in: EVERYWHERE
 * Description:
 *
\***************************************************************/
int TTClearScreen();

/***************************************************************\
 *
 * Replaces: beep_tp
 * Used in: EVERYWHERE
 * Description: Causes the speaker on the teach pendant to beep
 *
\***************************************************************/
int TTBeepTP(int iBeepTypeArg);

/***************************************************************\
 *
 * Replaces: prints_at
 * Used in: EVERYWHERE
 * Description: Prints the specified string on the teach
 *      pendant LCD screen at the specified coordinates
 *
\***************************************************************/
int TTPrintsAt(   int iRowArg,
                int iColArg,
                char *cpOutputStringArg);

/***************************************************************\
 *
 * Replaces: printc_at
 * Used in: execute2, diag, teach
 * Description: Prints the specified character on the teach
 *      pendant LCD screen at the specified position.
 *
\***************************************************************/
int TTPrintcAt(   int iRowArg,
                int iColArg,
                char cOutputCharArg);

/***************************************************************\
 *
 * Replaces: print_message
 * Used in: execute2, teach
 * Description: Prints the specified string on the teach
 *      pendant LCD screen starting at the 4th row and
 *      5th column followed by a beep of the specified type
 *
\***************************************************************/
int TTPrintMessage(   int iBeepTypeArg,
                    char *cpOutputStringArg);

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Sets the echo mode. If selected,
 *      the teach pendant echos all characters locally to the
 *      teach pendant LCD screen. This way the Comm module doesn't have
 *      to echo the keypress back to the teach pendant.
 *
\***************************************************************/
int TTSetEchoMode(int iEchoOnArg);

/***************************************************************\
 *
 * Replaces: set_knp_func
 * Used in: execute2, diag, teach
 * Description: Sets the KNP mode (key-not-pressed). If selected,
 *      the teach pendant responds when a key is released.
 *      Otherwise it reponds right when the key is pressed.
 *
\***************************************************************/
int TTSetKNPMode(int iKNPOnArg);

/***************************************************************\
 *
 * Replaces: key_repeat
 * Used in: Nowhere, including its own file
 * Description: Sets the value of the key repeat mode.
 *      This allows a single key press to repeat.
 *
\***************************************************************/
int TTSetKeyRepeat(int iKeyRepeatArg);

/***************************************************************\
 *
 * Replaces: show_teach_logo
 * Used in: diag, execute, main, teach
 * Description: Displays the company logo on the teach pendant
 *      LCD screen. Also sets up the teach pendant for idling.
 *
\***************************************************************/
int TTShowLogo();

/***************************************************************\
 *
 *                  NEW FUNCTION
 *
 * Description: Gets the teach pendant ID (old or new) and sets a
 *      variable accordingly. A new teach pendant uses a local echo
 *      and also has to perform carriage returns and line feeds.
 *      Normally the Comm module echo would take care of that like on
 *      an old teach pendant.
 *
\***************************************************************/
int TTInitTP();
/****************************************************************
 *
 * Function:    TTReadTPID
 *
 * Abstract:    Function interogates Teach Pendant  firmware to return its
 *              Firmware version string and returns it back to the calling routine
 * Parameters:  none
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int TTReadTPID(char *sVersionStringPar);

#endif
