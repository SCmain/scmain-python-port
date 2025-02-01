/****************************************************************
 *
 * Program:     Controller firmware
 * File:        ttscr.c
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
 * Rev      ECO#    Date    Author      Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 ****************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "sck.h"
#include "sctim.h"
#include "ser.h"
#include "scttr2.h"
#include "scttr.h"

/****************************************************************
 *
 * Function:    TTSetCursorPos
 *
 * Abstract:    Sets the current cursor position on the teach
 *      pendant LCD screen.
 *
 * Parameters:
 *      iRowArg     (in) The row to set the cursor on
 *      iColArg     (in) The column to set the cursor on
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: setcpos
 * Used in: diag, teach
 *
 ***************************************************************/
int TTSetCursorPos(int iRowArg, int iColArg)
{
    char caRowColString[21];    /* for setting the row and col */

    /* Verify that a valid argument was passed */
    if ((iRowArg < MINROW) || (iRowArg > MAXROW) ||
        (iColArg < MINCOL) || (iColArg > MAXCOL))
    {
        return FAILURE;
    }

    /* Initiate "position cursor" command */
    if (SERPutsTxBuff(SERGetTTPort(), SET_CURSOR_STR_BEG) == FAILURE)
        return FAILURE;

    /* Send the TP the row and column to move to with a semicolon separating them */
    sprintf(caRowColString, "%d", iRowArg);
    if (SERPutsTxBuff(SERGetTTPort(), caRowColString) == FAILURE)
        return FAILURE;
    if (SERPutsTxBuff(SERGetTTPort(), SEMICOLON) == FAILURE)
        return FAILURE;
    sprintf(caRowColString, "%d", iColArg);
    if (SERPutsTxBuff(SERGetTTPort(), caRowColString) == FAILURE)
        return FAILURE;

    /* Terminate "position cursor" command */
    if (SERPutsTxBuff(SERGetTTPort(), SET_CURSOR_STR_END) == FAILURE)
        return FAILURE;

    /* Wait for the command to be completely sent */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTSetCursorType
 *
 * Abstract:    Sets the current cursor type on the teach pendant
 *
 * Parameters:
 *      iCursorTypeArg  (in) The cursor type selected from the enum
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: setctype
 * Used in: diag, teach
 *
 ***************************************************************/
int TTSetCursorType(int iCursorTypeArg)
{
    /* Verify that a valid argument was passed */
    if ((iCursorTypeArg != ENABLE_CURSOR) && (iCursorTypeArg != NO_CURSOR) &&
        (iCursorTypeArg != BLINK) && (iCursorTypeArg != NO_BLINK))
    {
        return FAILURE;
    }

    /* Send the ESCape sequence (or string) to set the appropriate cursor type */
    if (SERPutsTxBuff(SERGetTTPort(), SET_CURSOR_TYPE_STR[iCursorTypeArg]) == FAILURE)
        return FAILURE;

    /* Wait for the command to be completely sent */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    /* Remember that cursor type for future use */
    iCursorType = iCursorTypeArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTClearScreen
 *
 * Abstract:    Clears the teach pendant LCD screen.
 *
 * Parameters:  None
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: clrscr
 * Used in: EVERYWHERE
 *
 ***************************************************************/
int TTClearScreen()
{
    /* Send the ESCape sequence (or string) to clear the screen */
    if (SERPutsTxBuff(SERGetTTPort(), CLRSTR) == FAILURE)
        return FAILURE;

    /* Wait for the command to be completely sent */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TBeepTP
 *
 * Abstract:    Causes the speaker on the teach pendant to beep
 *
 * Parameters:
 *      iBeepTypeArg    (in) The beep type selected from the enum
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: beep_tp
 * Used in: EVERYWHERE
 *
 ***************************************************************/
int TTBeepTP(int iBeepTypeArg)
{
    /* Verify that a valid argument was passed */
    if ((iBeepTypeArg != SHORTBELL) && (iBeepTypeArg != LONGBELL) &&
        (iBeepTypeArg != ALERT) && (iBeepTypeArg != CLICK) && (iBeepTypeArg != NO_BEEP))
    {
        return FAILURE;
    }

    /* If a beep was requested... */
    if (iBeepTypeArg != NO_BEEP)
    {
        /* Send the ESCape sequence (or string) to make da noise! */
        if (SERPutsTxBuff(SERGetTTPort(), BEEP_STR[iBeepTypeArg]) == FAILURE)
            return FAILURE;

        /* Wait for the command to be completely sent */
        if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
            return FAILURE;
    }

    /* Remember that beep type for future use */
    iBeepType = iBeepTypeArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTPrintsAt
 *
 * Abstract:    Prints the specified string on the teach
 *      pendant LCD screen at the specified coordinates
 *
 * Parameters:
 *      iRowArg             (in) The row to set the cursor on
 *      iColArg             (in) The column to set the cursor on
 *      cpOutputStringArg   (in) The string to print
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: prints_at
 * Used in: EVERYWHERE
 *
 ***************************************************************/
int TTPrintsAt(int iRowArg, int iColArg, char *cpOutputStringArg)
{
    /* Set the cursor position to print the string at */
    if (TTSetCursorPos(iRowArg, iColArg) == FAILURE)
        return FAILURE;

    /* Print out the string */
    if (SERPutsTxBuff(SERGetTTPort(), cpOutputStringArg) == FAILURE)
        return FAILURE;

    /* Wait for the string to print completely */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTPrintcAt
 *
 * Abstract:    Prints the specified character on the teach
 *      pendant LCD screen at the specified position.
 *
 * Parameters:
 *      iRowArg         (in) The row to set the cursor on
 *      iColArg         (in) The column to set the cursor on
 *      cOutputCharArg  (in) The character to print
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: printc_at
 * Used in: execute2, diag, teach
 *
 ***************************************************************/
int TTPrintcAt(int iRowArg, int iColArg, char cOutputCharArg)
{
    char caOutMsg[3];                   /* String to copy the character to */

    /* Set the cursor position to print the string at */
    if (TTSetCursorPos(iRowArg, iColArg) == FAILURE)
        return FAILURE;

    /* Print out the character from a string */
    caOutMsg[0] = cOutputCharArg;
    caOutMsg[1] = 0;                    /* NULL terminate the string */
    if (SERPutsTxBuff(SERGetTTPort(), caOutMsg) == FAILURE)
        return FAILURE;

    /* Wait for the character to print completely */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTPrintMessage
 *
 * Abstract:    Prints the specified string on the teach
 *      pendant LCD screen starting at the 4th row and
 *      5th column followed by a beep of the specified type
 *
 * Parameters:
 *      iBeepTypeArg        (in) The type of beep
 *      cpOutputStringArg   (in) The string to print
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: print_message
 * Used in: execute2, teach
 *
 ***************************************************************/
int TTPrintMessage(int iBeepTypeArg, char *cpOutputStringArg)
{
    char caRowColString[21];    /* for setting the row and col */

    /* Clear area that will be printed to */
    if (TTPrintsAt(4, 5, "                ") == FAILURE)
        return FAILURE;

    /* Print string on TP. Truncate if necessary */
    if (strlen(cpOutputStringArg) > 16)
    {
        strncpy(caRowColString, cpOutputStringArg, 16);
        caRowColString[15] = 0;
        if (TTPrintsAt(4, 5, caRowColString) == FAILURE)
            return FAILURE;
    }
    else
    {
        if (TTPrintsAt(4, 5, cpOutputStringArg) == FAILURE)
            return FAILURE;
    }

    /* Make da noise! */
    if (TTBeepTP(iBeepTypeArg) == FAILURE)
        return FAILURE;

    /* Wait for the string to print completely and the beep to finish */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTSetEchoMode
 *
 * Abstract:    Sets the echo mode. If selected,
 *      the teach pendant echos all characters locally to the
 *      teach pendant LCD screen. This way the Comm module doesn't have
 *      to echo the keypress back to the teach pendant.
 *
 * Parameters:
 *      iEchoOnArg   (in) The value to set echo mode to
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int TTSetEchoMode(int iEchoOnArg)
{
    char cByte = EOF;
    int iWhichX = 0;

    /* Verify that a valid argument was passed */
    if ((iEchoOnArg != TRUE) && (iEchoOnArg != FALSE))
        return FAILURE;

    if (!iOldTP)
    {
        /* Send the ESCape sequence (or string) to set the echo mode appropriately */
        if (SERPutsTxBuff(SERGetTTPort(), ECHO_STR_NEW[iEchoOnArg]) == FAILURE)
            return FAILURE;
        /* Wait for the command to be completely sent */
        if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
            return FAILURE;

        /* Send the ESCape sequence (or string) to set the CRLF mode appropriately.
         * The local echo needs to provide its own carriage returns and line feeds. */
        if (SERPutsTxBuff(SERGetTTPort(), CR_LF_MODE_STR[iEchoOnArg]) == FAILURE)
            return FAILURE;
        /* Wait for the command to be completely sent */
        if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
            return FAILURE;
    }
    else
    {
        if (SERSetCommVals(SERGetTTPort(), ECHO_MODE, iEchoOnArg) == FAILURE)
            return FAILURE;
        /* Send the ESCape sequence (or string) to set the echo mode appropriately */
//        if (SERPutsTxBuff(SERGetTTPort(), ECHO_STR[iEchoOnArg]) == FAILURE)
//            return FAILURE;
        /* Wait for the command to be completely sent */
//        if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
//            return FAILURE;

/*        while (iWhichX < 2)
        {
            if (SERGetcRxBuff(SERGetTTPort(), &cByte, FALSE) == FAILURE)
                return FAILURE;
            if (cByte == 'x')
                iWhichX++;
        }*/
    }

    /* Remember that echo mode for future use */
    iEchoMode = iEchoOnArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTSetKNPMode
 *
 * Abstract:    Sets the KNP mode (key-not-pressed). If selected,
 *      the teach pendant responds when a key is released.
 *      Otherwise it reponds right when the key is pressed. (TRUE/FALSE)
 *
 * Parameters:
 *      iKNPOnArg   (in) The value to set KNP mode to
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: set_knp_func
 * Used in: execute2, diag, teach
 *
 ***************************************************************/
int TTSetKNPMode(int iKNPOnArg)
{
    /* Verify that a valid argument was passed */
    if ((iKNPOnArg != TRUE) && (iKNPOnArg != FALSE))
        return FAILURE;

    /* Send the ESCape sequence (or string) to set the KNP mode appropriately */
    if (SERPutsTxBuff(SERGetTTPort(), KNP_STR[iKNPOnArg]) == FAILURE)
    {   printf("TTSetKNPMode failed SERPutsTxBuff\n");
        return FAILURE;
    }
    /* Wait for the command to be completely sent */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
    {   printf("TTSetKNPMode failed SERFlushTxBuff\n");
        return FAILURE;
    }

    /* Remember that KNP mode for future use */
    iKNPMode = iKNPOnArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTSetKeyRepeat
 *
 * Abstract:    Sets the value of the key repeat mode.
 *      This allows a single key press to repeat.
 *
 * Parameters:
 *      iKeyRepeatArg   (in) The value of key repeat mode
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: key_repeat
 * Used in: Nowhere, including its own file
 *
 ***************************************************************/
int TTSetKeyRepeat(int iKeyRepeatArg)
{
    /* Verify that a valid argument was passed */
    if ((iKeyRepeatArg != TRUE) && (iKeyRepeatArg != FALSE))
        return FAILURE;

    /* Send the ESCape sequence (or string) to set the key repeat mode appropriately */
    if (SERPutsTxBuff(SERGetTTPort(), KEY_REPEAT_STR[iKeyRepeatArg]) == FAILURE)
        return FAILURE;

    /* Wait for the command to be completely sent */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;

    /* Remember that key repeat mode for future use */
    iKeyRepeatMode = iKeyRepeatArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TTShowLogo
 *
 * Abstract:    Displays the company logo on the teach pendant
 *      LCD screen. Also sets up the teach pendant for idling.
 *
 * Parameters:  none
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: show_teach_logo
 * Used in: diag, execute, main, teach
 *
 ***************************************************************/
int TTShowLogo()
{
    /* Turn off CTS/RTS temporarily */
    if (SERSetCommVals(SERGetTTPort(), FLOW_CTRL, FALSE) == FAILURE)
    {   printf("TTShowLogo failed set flow ctrl\n");
        return FAILURE;
    }

    /* Reset comm parameters in case they have been changed */
//    if (SERSetCommVals(SERGetTTPort(), ECHO_MODE, FALSE) == FAILURE)
//        return FAILURE;
    if (TTInitTP() == FAILURE)
    {   printf("TTShowLogo failed TTInitTP\n");
        return FAILURE;
    }

    if (TTSetEchoMode(FALSE) == FAILURE)
    {   printf("TTShowLogo failed TTSetEchoMode\n");
        return FAILURE;
    }

    /* Reset TP modes in case they have been changed */
    if (TTSetKNPMode(FALSE) == FAILURE)
    {   printf("TTShowLogo failed TTSetKNPMode\n");
        return FAILURE;
    }
    if (TTSetCursorType(NO_BLINK) == FAILURE)
    {   printf("TTShowLogo failed TTSetCursorType\n");
        return FAILURE;
    }
    if (TTSetCursorType(NO_CURSOR) == FAILURE)
    {   printf("TTShowLogo failed TTSetCursorType no cursor\n");
        return FAILURE;
    }

    /* Clear the TP LCD screen and print out the Equipe Logo.
     * Waits for transmit to finish are done within each individual function. */
    if (TTClearScreen() == FAILURE)
    {   printf("TTShowLogo failed TTClearScreen\n");
        return FAILURE;
    }
    if (TTPrintsAt(1, 14, "tm") == FAILURE)
    {   printf("TTShowLogo failed TTPrintsAt\n");
        return FAILURE;
    }
    if (TTPrintsAt(2, 8, "XyZ") == FAILURE)
    {   printf("TTShowLogo failed TTPrintsAt\n");
        return FAILURE;
    }
    if (TTPrintsAt(3, 5, "AUTOMATION") == FAILURE)
    {   printf("TTShowLogo failed TTPrintsAt\n");
        return FAILURE;
    }

    /* Turn CTS/RTS back on */
    if (SERSetCommVals(SERGetTTPort(), FLOW_CTRL, TRUE) == FAILURE)
    {   printf("TTShowLogo failed SERSetCommVals\n");
        return FAILURE;
    }

    return SUCCESS;
}
/****************************************************************
 *
 * Function:    TTInitTP
 *
 * Abstract:    Gets the teach pendant ID (old or new) and sets a
 *      variable accordingly. A new teach pendant uses a local echo
 *      and also has to perform carriage returns and line feeds.
 *      Normally the Comm module echo would take care of that like on
 *      an old teach pendant.
 *
 * Parameters:  none
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int TTInitTP()
{
    char sBuf[ID_LENGTH+1];

    TTReadTPID(sBuf);
    /* Compare the Teach Pendant ID */
    if (!strncmp(sBuf, NEW_TP_ID, ID_LENGTH))
        iOldTP = FALSE;                 /* New teach pendant */
    else
        iOldTP = TRUE;

    return SUCCESS;
}
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
int TTReadTPID(char *sVersionStringPar)
{
    char sBuf[ID_LENGTH+1], cByte;
//    int iNumBytes = ID_LENGTH;
    int iNumBytes = 0;
    int iTimeout;

    memset(sBuf, 0, (ID_LENGTH+1));
    /* Send the ESCape sequence (or string) to get the teach pendant ID */
    if (SERPutsTxBuff(SERGetTTPort(), REPORT_ID_STR[0]) == FAILURE)
        return FAILURE;
    /* Wait for the command to be completely sent */
    if (SERFlushTxBuff(SERGetTTPort()) == FAILURE)
        return FAILURE;
    /* Wait for the incoming characters to be received */
    if ((iTimeout=TIGetCounter()) == FAILURE)
        return FAILURE;
    if (TISetCounter(iTimeout, 500) == FAILURE)
        return FAILURE;
    while (iNumBytes != ID_LENGTH)
    {
        if (TICountExpired(iTimeout))
            return FAILURE;
        if (SERGetcRxBuff(SERGetTTPort(), &cByte, FALSE) == FAILURE)
            return FAILURE;
        if (cByte != EOF && cByte != 0)
            sBuf[iNumBytes++] = cByte;
    }
//    if (SERGetsRxBuff(SERGetTTPort(), sBuf, FALSE, &iNumBytes, TRUE) == FAILURE)
//        return FAILURE;
    if (TISetCounter(iTimeout, 100) == FAILURE)
        return FAILURE;
    while (cByte != 'x')
    {
        if (TICountExpired(iTimeout))
            return FAILURE;
        if (SERGetcRxBuff(SERGetTTPort(), &cByte, FALSE) == FAILURE)
            return FAILURE;
    }
    strcpy(sVersionStringPar, sBuf);

    if (TIReturnCounter(iTimeout) == FAILURE)
        return FAILURE;

    return SUCCESS;
}
