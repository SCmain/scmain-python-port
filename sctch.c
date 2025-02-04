/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * Original Author      : Canh Le (Deceased)
 * Current Maintainer   : Georges Sancosme (georges@sancosme.net)
 * Maintained Since     : 13.01.2025
 * Created On           : 04.06.2007
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Program:     Controller firmware
 * File:        sctch.c
 * Functions:   TMTeach
 *              TMGetIP
 *              TMSetIP
 *              TMDoTeachKey
 *              TMDrawTeachScr
 *              TMDisplayAxes
 *              TMDisplayOthers
 *              TMDoAlign
 *              TMChgCoord
 *              TMSetTchSpd
 *              TMTeachAxis
 *              TMStoreCoord
 *              TMMoveToCoord
 *              TMPickPlaceWafer
 *              TMToggleServo
 *              TMTchMotionCheck
 *              TMDoGetPut
 *              TMTeachMenu
 *              TMGetTeachFlag
 *              TMSetTeachFlag
 *              TMTeachEdit
 *              TMRunCommand
 *              TMWaitTchMotionComplete
 *
 * Description: Handles the interface to teach mode
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 * NOTE: The array pcaTchSpdStr which holds the speed indices
 *      in strings is really unnecessary. The iaSpeedIndex value
 *      could just be converted to a string. I guess using strings
 *      referenced in an array gives some flexibility to what
 *      can be printed out to the teach pendant screen. It doesn't
 *      have to be just numbers from 1 to 10.
 *
 * NOTE: The way teach mode was accessed in the old code
 *      was through the go_to_teach flag. A value of 1
 *      meant "run teach mode through COM1". A value
 *      of 2 meant "run teach mode through COM2".
 *      a new flag, iTeachFlag, has been introduced to handle
 *      that access independently of diagnostics mode. It can
 *      be set and retrieved through the TMSetTeachFlag and
 *      TMGetTeachFlag functions, respectively, and
 *      can have values of NO_PORT_NUM, COM1, and COM2, the
 *      communications port that teach mode is to
 *      be run through.
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 ****************************************************************/

#include <sys/io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sck.h"
#include "scintr.h"
#include "ser.h"
#include "sctim.h"
#include "scttr.h"
#include "sctchl.h"
#include "sctch.h"
#include "scmac.h"
#include "scregg.h"
#include "gag.h"
#include "gaintr.h"
#include "ro.h"
#include "roga.h"
#include "rofio.h"
#include "fiog.h"
#include "scio.h"
#include "scproc.h"
#include "alstep.h"

//Holds the current Galil's modulo value
long lGalilModulo = 0;
extern HANDLEDMC ghDMC;
extern int giSysCfgNum;
extern int giIPWFlag[8];

long glaSoftNegLim[8];
long glaSoftPosLim[8];

long glVectorLength; // used for Smart-Motion vector distance

/****************************************************************
 *
 * Function:    TMTeach
 *
 * Abstract:    Main entry point for teach mode. Starts teach mode.
 *      It begins by setting up the teach pendant appropriately.
 *      It then asks the user whether to go to terminal mode,
 *      teach mode, or quit back to the previous screen (typically
 *      the Equipe logo screen). If teach mode is selected, TMTeachMenu
 *      is invoked which is the core of teach mode. It finishes by
 *      resetting all the interrupt macro flags, restoring normal
 *      teach pendant settings, and printing a prompt to the active terminal.
 *
 * Parameters:
 *      iPortNumArg         (in) The port number to run teach mode from
 *      iDefineFlagArg      (in) Define flags; needed to determine if alignment can be aborted
 *      iEmulatorArg        (in) Emulator; needed to determine microns or mils
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: teach
 * Used in: main
 *
 ***************************************************************/
int TMTeach(int iPortNumArg, int iDefineFlagArg, int iEmulatorArg)
{
    int iValidKey;      /* Indicates when a valid key press was received */
    int iDoTeach;       /* Indicates whether to continue or quit teach mode */
    char cKeyPressed;   /* The teach pendant key pressed */
    char caPrompt[5];   /* The prompt string to output when exiting. */
    int iStn;           /* station number converted from letter */
    int iLMM, iERR;     /* LMM & ERR enabled flags */

    for(iDoTeach=0; iDoTeach<8; ++iDoTeach)
    {
	giIPWFlag[iDoTeach] = 0;
    }

    /* Check validity of initialization parameters */
    if (ROGetSpecialAxis(RO_ROBOT))
    {
        iaAxisExists[T] = TRUE;
        ulaMechAxisDef[T] = RO_AXIS_T;
        iaAxisExists[R] = TRUE;
        ulaMechAxisDef[R] = RO_AXIS_R;
        iaAxisExists[Z] = TRUE;
        ulaMechAxisDef[Z] = RO_AXIS_Z;
    }
    if (ulaMechAxisDef[TRK]=ROGetSpecialAxis(RO_TRACK))
        iaAxisExists[TRK] = TRUE;
    if (ulaMechAxisDef[DBM]=ROGetSpecialAxis(RO_DUAL_ARM))
        iaAxisExists[DBM] = TRUE;
//    if (ulaMechAxisDef[FLP]=ROGetSpecialAxis(RO_FLIPPER))
//        iaAxisExists[FLP] = TRUE;
    if (ulaMechAxisDef[VACZ1]=ROGetSpecialAxis(RO_INDEXER_Z1))
        iaAxisExists[VACZ1] = TRUE;
    if (ulaMechAxisDef[VACT1]=ROGetSpecialAxis(RO_INDEXER_T1))
        iaAxisExists[VACT1] = TRUE;
    if (ulaMechAxisDef[VACZ2]=ROGetSpecialAxis(RO_INDEXER_Z2))
        iaAxisExists[VACZ2] = TRUE;
    if (ulaMechAxisDef[VACT2]=ROGetSpecialAxis(RO_INDEXER_T2))
        iaAxisExists[VACT2] = TRUE;

    /* Look through all possible axes to make sure at least 1 teachable axis exists. */
    for (iDoTeach=0; iDoTeach<MAX_TCH_AXES; iDoTeach++)
    {
        if (iaAxisExists[iDoTeach])
            break;
    }
    // No teachable axes exist (e.g. pre-aligner only).
    //if (iDoTeach == MAX_TCH_AXES)
    //    return FAILURE;

    /* Determine conversion units based on the emulation. */
    iTMDefineFlag = iDefineFlagArg;
    iTMEmulator = iEmulatorArg;
    if (iEmulatorArg & DFEMULATORB)
    {
        lUnitConvDist = MIC_PER_MIL;
        lUnitConvAngle = MIL_DEG_PER_HUND;
    }
    else
    {
        lUnitConvDist = 1L;
        lUnitConvAngle = 1L;
    }

    /* get the current station's EE# if DBM exists */
    if (iaAxisExists[DBM])
    {
        iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
        if (FIOGetStnEEindex(iStn, &iUseDBM) == FAILURE)
            return FAILURE;
    }
    else
        iUseDBM = 0;

    /* Check out a timer. */
    if ((iCounterIndex=TIGetCounter()) == FAILURE)
        return FAILURE;

    /* Set up comm port and teach pendant. */
    iTMPortNum = iPortNumArg;
    if (SERSetCommVals(iTMPortNum, TT_PORT, TRUE) == FAILURE)
        return FAILURE;
    if (SERSetCommVals(iTMPortNum, ECHO_MODE, FALSE) == FAILURE)
        return FAILURE;
    if (TTSetEchoMode(FALSE) == FAILURE)
        return FAILURE;

    if (TTSetKNPMode(FALSE) == FAILURE)
        return FAILURE;
    if (TTSetCursorType(NO_BLINK) == FAILURE)
        return FAILURE;
    if (TTSetCursorType(NO_CURSOR) == FAILURE)
        return FAILURE;

    /* Ask the user which mode to enter. */
    if (TTClearScreen() == FAILURE)
        return FAILURE;
    if (TTPrintsAt(1, 1, "T - Terminal") == FAILURE)
        return FAILURE;
    // No teachable axes exist (e.g. pre-aligner only), we don't need teach mode.
    if (iDoTeach < MAX_TCH_AXES)
    {
        if (TTPrintsAt(2, 1, "E - Teach Terminal") == FAILURE)
            return FAILURE;
    }
    if (TTPrintsAt(3, 1, "Q - Quit") == FAILURE)
        return FAILURE;
    if (TTPrintsAt(4, 1, "Enter Selection>") == FAILURE)
        return FAILURE;

    /* Get user selection of mode to enter. */
    iValidKey = FALSE;
    while (!iValidKey)
    {
        if (SERGetcRxBuff(iTMPortNum, &cKeyPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cKeyPressed)
        {
            case 'T':
                /* In the case of terminal mode, flush any pending commands.
                * If move commands or something were entered during this loop,
                * we don't want to jump out and execute them. They might unsafely
                * cause the robot to perform unexpected actions. */
                if (SERFlushComBuffer(iTMPortNum) == FAILURE)
                    return FAILURE;
                if (SERSetCommVals(iTMPortNum, CMD_PORT, TRUE) == FAILURE)
                    return FAILURE;
                iDoTeach = FALSE;
                iValidKey = TRUE;
                break;

            case 'E':
                // No teachable axes exist (e.g. pre-aligner only), we don't need the teach mode
                if (iDoTeach < MAX_TCH_AXES)
                {
                    iDoTeach = TRUE;
                    iValidKey = TRUE;
                }
                break;

            case 'Q':
                /* Check case 'T' to understand what happens here and why. */
                if (SERFlushComBuffer(COM1) == FAILURE)
                    return FAILURE;
                if (SERSetCommVals(COM1, CMD_PORT, TRUE) == FAILURE)
                    return FAILURE;
                iDoTeach = FALSE;
                iValidKey = TRUE;
                break;
        }
    }

    /* Run teach mode if selected. This will not return until teach
     * mode is exited, i.e. a ctrl-X or ctrl-E is pressed. */
    if (iDoTeach)
    {
        /* Key-not-pressed mode is needed for jog mode. When a key is pressed,
         * the robot starts moving but doesn't stop until the key release
         * returns a 0. Side effects of this mode are taken care of in other places. */
        if (TTSetKNPMode(TRUE) == FAILURE)
            return FAILURE;
        /* In teach mode, LMM & ERR should be disabled */
        iLMM = MRGetMacroIntegerVars(MR_LMM_ENABLED);
        iERR = MRGetMacroIntegerVars(MR_ERR_ENABLED);
        MASetMacroIntegerVars(MR_LMM_ENABLED, FALSE);
        MASetMacroIntegerVars(MR_ERR_ENABLED, FALSE);
        TMTeachMenu();
        MASetMacroIntegerVars(MR_LMM_ENABLED, iLMM);
        MASetMacroIntegerVars(MR_ERR_ENABLED, iERR);
        if (TTSetKNPMode(FALSE) == FAILURE)
            return FAILURE;
    }

    /* Reset all Interrupt flags */
    MASetFlagsToFalse();

    /* Restore original com port setting. */
    if (SERSetCommVals(COM2, TT_PORT, TRUE) == FAILURE)
        return FAILURE;

    /* Set up the teach pendant LCD screen appropriately. */
    if (SERGetCmdPort() == COM2)
    {
        if (TTClearScreen() == FAILURE)
            return FAILURE;
        if (SERSetCommVals(SERGetCmdPort(), ECHO_MODE, TRUE) == FAILURE)
            return FAILURE;
//        if (TTSetEchoMode(TRUE) == FAILURE)
//            return FAILURE;
        if (TTSetCursorPos(4, 1) == FAILURE)
            return FAILURE;
        if (TTSetCursorType(ENABLE_CURSOR) == FAILURE)
            return FAILURE;
        if (TTSetCursorType(BLINK) == FAILURE)
            return FAILURE;
    }
    else
    {
        if (TTShowLogo() == FAILURE)
            return FAILURE;
    }

    /* Send a prompt to the command port. That could be on the teach
     * pendant or EQT. This is a point of controversy. All host PC's
     * can't necessarily handle the \r and \n. Some want just a \r. */
    if (TIGetTimerVals(LOCKED) == 123)
        sprintf(caPrompt, "\r\n%c", PCGetWDPrompt());
    else
        sprintf(caPrompt, "\r\n>");
    if (SERPutsTxBuff(SERGetCmdPort(), caPrompt) == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    /* Check the timer back in. */
    if (TIReturnCounter(iCounterIndex) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMGetIP
 *
 * Abstract:    Gets the Galil ID command settings so they
 *      can be saved and restored later. (TOT in-position window).
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: read_IP
 * Used in: teach
 *
 ***************************************************************/
int TMGetIP()
{
    int iUseGalilTemp;
    int iAxis;
    long laTempIP[8];

    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            /* Record the UseGalil flag. On error, the previous state is restored. */
            iUseGalilTemp = GAGetUseGalilFlag();
            laTempIP[0] = laTempIP[1] = laTempIP[2] = laTempIP[3] = 0;
            /* Send the ID command. */
            ROGetInterruptPositionCommand(ulaMechAxisDef[iAxis], laTempIP);

            /* If there was an error sending the ID command, exit gracefully. */
            if (GAGetGalilErrorFlag())
            {
                GASetUseGalilFlag(iUseGalilTemp);
                GASetGalilErrorFlag(0);
                laSaveIP[iAxis] = 0;
                return FAILURE;
            }
            /* Otherwise put the results into the array that was passed in. */
            laSaveIP[iAxis] = laTempIP[iaMechAxisIdx[iAxis]];
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMSetIP
 *
 * Abstract:    Sets the Galil ID command settings so to
 *      previously saved values. (TOT in-position window).
 *
 * Parameters:
 *      lpIPArg     (in) Galil ID command parameters
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: set_IP
 * Used in: teach
 *
 ***************************************************************/
int TMSetIP(long *lpIPArg)
{
    int iUseGalilTemp;
    int iAxis;
    long laTempIP[8];

    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            /* Record the UseGalil flag. On error, the previous state is restored. */
            iUseGalilTemp = GAGetUseGalilFlag();
            /* Put the passed in values into the local copy. */
            laTempIP[iaMechAxisIdx[iAxis]] = lpIPArg[iAxis];
            /* Send the ID command. */
            ROSetInterruptPositionCommand(ulaMechAxisDef[iAxis], laTempIP);

            /* If there was an error sending the ID command, exit gracefully. */
            if (GAGetGalilErrorFlag())
            {
                GASetUseGalilFlag(iUseGalilTemp);
                GASetGalilErrorFlag(0);
                return FAILURE;
            }
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMTeachMenu
 *
 * Abstract:    Starts teach mode menu. It starts by saving all the
 *      speeds, accelerations, decelerations, and Galil ID command
 *      settings. Then it sets the new values which are much slower
 *      for teach mode. It then displays the teach menu and
 *      checks for keypresses calling the TMDoTeachKey function to
 *      process those keypresses. That main teach loop handles all
 *      I/O to operate the teach mode menu. When teach mode is exited,
 *      all previously saved values are restored.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: teach_menu
 * Used in: teach
 *
 * NOTE: The speed, acceleration, etc. setting and getting functions
 *      affect the entire array. We can't depend that values we don't
 *      intend to touch, will not be changed. Similarly, if we proceed
 *      not caring about those array points, the get and set functions
 *      might repetitively perform some operation that causes a divide-by-zero or
 *      floating point error. So array points not used have to be
 *      initialized to 0 every time before get and set functions are called.
 *
 ***************************************************************/
int TMTeachMenu()
{
    int iQuitLoop;                  /* Indicates that user terminated teach mode.
                                     * Causes an exit from the main teach loop. */
    char cButtonPressed;            /* The user keypress. */
    long laIPParms[MAX_TCH_AXES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  /* Temporary array to turn off all TOT functionality. */
    long laParms[8];                /* Temporary array for general use. */
    int iAxis;
    long lTAFlags[8]={0,0,0,0,0,0,0,0};
    unsigned long ulAxisAll = RO_AXIS_ALL;
    BOOL bCommandedMode;
    unsigned long laSoftNegLim[8]={0,0,0,0,0,0,0,0};
    unsigned long laSoftPosLim[8]={0,0,0,0,0,0,0,0};
    int iValue;
    int iStn;


    /* get the S-curve profile flag value either enable(1) or disable(0) */
//    if( ROReadSCurveProfile(ulAxisAll,lTAFlags) == FAILURE )
//        return FAILURE;

    //For VAC514 robots only
    if(iTMDefineFlag & DFVAC514)
    {
        // Store the current gearing type setting, gearing to commanded or actual position
        bCommandedMode = ROGetCM();
        ROSetCM(FALSE);
    }

    /* Save the old speeds, accelerations, and decelerations for all teachable axes. */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            /* Use the temporary array. We don't want to damage any data
             * already stored in the save arrays. */
            if (ROGetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_SPEED) == FAILURE)
                return FAILURE;
            /* Copy the new track value and only that value to the appropriate save array. */
            laSaveSpeed[iAxis] = laParms[iaMechAxisIdx[iAxis]];

            if (ROGetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_ACCEL) == FAILURE)
                return FAILURE;
            laSaveAccel[iAxis] = laParms[iaMechAxisIdx[iAxis]];

            if (ROGetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_DECEL) == FAILURE)
                return FAILURE;
            laSaveDecel[iAxis] = laParms[iaMechAxisIdx[iAxis]];

        }
    }
    if (ROGetParameter(TRUE, ulaMechAxisDef[0], glaSoftNegLim, SOFT_NEG_LIMIT) == FAILURE)
        return FAILURE;
    if (ROGetParameter(TRUE, ulaMechAxisDef[0], glaSoftPosLim, SOFT_POS_LIMIT) == FAILURE)
        return FAILURE;

    if (iaAxisExists[3])
    {
	if (ROGetParameter(TRUE, ulaMechAxisDef[3], laSoftNegLim, SOFT_NEG_LIMIT) == FAILURE)
            return FAILURE;
      	if (ROGetParameter(TRUE, ulaMechAxisDef[3], laSoftPosLim, SOFT_POS_LIMIT) == FAILURE)
            return FAILURE;
    	glaSoftNegLim[3] = laSoftNegLim[3];
    	glaSoftPosLim[3] = laSoftPosLim[3];
    }

    glVectorLength = 50; // Set Smart-motion vector length to minimum.

    iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
    if (FIOGetStnlscsi(iStn, 5, &iValue) == FAILURE)
	return FAILURE; 

    if (iValue == 313)
	iSmartMode = TRUE;
    else
	iSmartMode = FALSE;

    /* Save Galil ID information (TOT in-position window). It will be reset to 0 and then restored before exiting. */
    if (TMGetIP() == FAILURE)
        return FAILURE;

    if (TMSetJogSpeedAndAccel() == FAILURE)
        return FAILURE;

    /* Set the Galil ID information to 0, i.e. set the time-optimal trajectory
     * in-position window to 0 so as to prevent any axis from moving before another is complete. */
    if (TMSetIP(laIPParms) == FAILURE)
        return FAILURE;

    /* Setup the teach mode screen. */
    if (TMDrawTeachScr() == FAILURE)
        return FAILURE;
    if (TTPrintMessage(NO_BEEP, "Ready          ") == FAILURE)
        return FAILURE;
    /* The [C60] register is typically used to save the current station name. */
    if (RGSetCharacter(60, cTchStn) == FAILURE)
        return FAILURE;

    /*** The MAIN TEACH MODE LOOP. This gets key presses and processes them. ***/
    /* Update information should not be in while loop. 
	that changes regularly and without warning. */

    if (TMDisplayAxes() == FAILURE)
        return FAILURE;
    if (TMDisplayOthers() == FAILURE)
        return FAILURE;

    iQuitLoop = FALSE;
    while (!iQuitLoop)
    {
        /* Check for and process key presses. */
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        if (cButtonPressed != EOF)
        {
            if (TMDoTeachKey(cButtonPressed) == FAILURE)
                iQuitLoop = TRUE;
        }
    }

    /* Restore the scan mode setting to FALSE. */
    iScanMode = FALSE;
    iTrkMode = FALSE;
    iFprMode = FALSE;
    iVacMode = FALSE;
    iSelAxis = T;

    if (TMRestoreSpeedAndAccel() == FAILURE)
        return FAILURE;

    /* Restore the old Galil ID settings (TOT in-position window). */
    if (TMSetIP(laSaveIP) == FAILURE)
        return FAILURE;

    // For VAC514 robots only
    if(iTMDefineFlag & DFVAC514)
    {
        // Restore the old gearing type setting
        ROSetCM(bCommandedMode);
    }
//    ROEnableSCurveProfile( ulAxisAll, lTAFlags );  /* set the S-curve motion profile flag (enable/disable) */

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMDoTeachKey
 *
 * Abstract:    Services keypresses from the teach pendant while
 *      in teach mode. Processes the key pressed. Calls the appropriate
 *      functions or changes the relevant variables.
 *
 * Parameters:
 *      cCharToProcessArg   (in) The character to process
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: do_teach_key, do_teach_key1
 * Used in: teach
 *
 ***************************************************************/
int TMDoTeachKey(char cCharToProcessArg)
{
    int iGAOutByte, iGAOutByt2; // Output port current stat to toggle output bit
    char cButtonPressed;    /* The vacuum indexer to teach. */

    if (cCharToProcessArg != 0)
    {
        /* Clear any error message */
        if (TTPrintMessage(NO_BEEP, "               ") == FAILURE)
            return FAILURE;
    }

    switch (cCharToProcessArg)
    {
        case 0:         /* ignore nulls from KNP mode */
            break;

        case 25:        /* ^Y -- align */
            if (ROGetSpecialAxis(RO_3_AXIS_PRE) || ROGetSpecialAxis(RO_SINGLE_PRE))
                TMDoAlign();
            break;

        case 5:         /* ^E -- return to background, COM1 */
            /* See the while loop in TMTeach to understand this section. */
            if (SERFlushComBuffer(COM1) == FAILURE)
                return FAILURE;
            if (SERSetCommVals(COM1, CMD_PORT, TRUE) == FAILURE)
                return FAILURE;
            if (TTBeepTP(CLICK) == FAILURE)
                return FAILURE;
            return FAILURE;

        case 6:         /* ^F -- activate flipper teach screen*/
            if (iaAxisExists[FLP] && !iTrkMode && !iVacMode)
            {
                if (iFprMode)
                {
                    iFprMode = FALSE;
                    iSelAxis = T;
                }
                else
                {
                    iFprMode = TRUE;
                    iSelAxis = FLP;
                }
                if (TMDrawTeachScr() == FAILURE)
                    return FAILURE;
                if (TMDisplayAxes() == FAILURE)
                    return FAILURE;
                if (TMDisplayOthers() == FAILURE)
                    return FAILURE;
                break;
            }
            else
                goto error_exit;

        case 24:        /* ^X -- return to background, COM3 */
            /* See the while loop in TMTeach to understand this section. */
            /* This delay is required to capture the 0 KNP character that
             * follows the ^X. Then the entire thing is flushed away. */
            if (TIDelay(250) == FAILURE)
                return FAILURE;
            if (SERFlushComBuffer(iTMPortNum) == FAILURE)
                return FAILURE;
            if (SERSetCommVals(iTMPortNum, CMD_PORT, TRUE) == FAILURE)
                return FAILURE;
            if (TTBeepTP(CLICK) == FAILURE)
                return FAILURE;
            return FAILURE;

        case 14:       /* ^N -- Toggle between R and DBM axis if DBM exists */
            if (iaAxisExists[DBM])
            {
                if (iUseDBM)
                    iUseDBM = 0;
                else
                    iUseDBM = 1;

                if (TMDrawTeachScr() == FAILURE)
                    return FAILURE;
                if (TMDisplayAxes() == FAILURE)
                    return FAILURE;
                if (TMDisplayOthers() == FAILURE)
                    return FAILURE;
            }
            else
                goto error_exit;
            break;

        case 19:        /* ^S -- toggles scan coordinate mode */
            if (!iTrkMode && !iFprMode)
            {
                if (iScanMode)
                    iScanMode = FALSE;
                else
                    iScanMode = TRUE;
                if (iScanMode)
                {
                    if (TTPrintcAt(1, 19, '*') == FAILURE)
                        return FAILURE;
                }
                else
                {
                    if (TTPrintcAt(1, 19, ' ') == FAILURE)
                        return FAILURE;
                }
            }
            else
                goto error_exit;
            break;

        case CR:        /* CR = HERE key */
            TMStoreCoord();
            break;

        case 'A':       /* Test a taught station by moving to it. Moves the track too. */
            if (TMMoveToCoord() == FAILURE)
                TTPrintsAt(4, 1, "Msg: Failed         ");
            break;

        case 'B':
            if (iaAxisExists[T] && !iTrkMode && !iFprMode && !iVacMode)
            {
                /* Pick. Can't be done from the track teach screen. Why?
                 * It moves the track too. Non/simulated mode is handled within. */
                if(iScanMode)
                    TTBeepTP(ALERT);
                else
                    TMPickPlaceWafer(TRUE);
                break;
            }
            else
                goto error_exit;

        case 'C':
            if (iaAxisExists[T] && !iTrkMode && !iFprMode && !iVacMode)
            {
                /* Place. Can't be done from the track teach screen. Why?
                 * It moves the track too. Non/simulated mode is handled within. */
                if(iScanMode)
                    TTBeepTP(ALERT);
                else
                    TMPickPlaceWafer(FALSE);
                break;
            }
            else
                goto error_exit;

        case 'D':       /* Change the station being taught, e.g. 'A' to 'c' */
            TMChgCoord();
            break;

        case 'F':       /* Rotate clockwise or move flipper. This changes the selected axis. */
            if (iFprMode)
            {
                TMTeachAxis(FLP, -1);
                break;
            }
            else if (iVacMode == VACZ1 && iaAxisExists[VACT1])
            {
                TMTeachAxis(VACT1, -1);
                break;
            }
            else if (iVacMode == VACZ2 && iaAxisExists[VACT2])
            {
                TMTeachAxis(VACT2, -1);
                break;
            }
            else if (iaAxisExists[T] && !iTrkMode)
            {
		//For VAC514 robots only
		if(iTMDefineFlag & DFVAC514)
		{
		    // Store the current modulo and reset it to 0, to avoid the shortest path mode in Teach mode
		    if(ROReadGalilModulo(&lGalilModulo) == FAILURE)
			return FAILURE;
		    if(ROSetGalilModulo(0) == FAILURE)
			return FAILURE;
		}

		TMTeachAxis(T, -1);

		//For VAC514 robots only
		if(iTMDefineFlag & DFVAC514)
		{
		    // Restore the old modulo setting
		    if(ROSetGalilModulo(lGalilModulo) == FAILURE)
			return FAILURE;
		}

                break;
            }
            else
                goto error_exit;

        case 'G':       /* Rotate counter clockwise or move flipper. This changes the selected axis. */
            if (iFprMode)
            {
                TMTeachAxis(FLP, +1);
                break;
            }
            else if (iVacMode == VACZ1 && iaAxisExists[VACT1])
            {
                TMTeachAxis(VACT1, +1);
                break;
            }
            else if (iVacMode == VACZ2 && iaAxisExists[VACT2])
            {
                TMTeachAxis(VACT2, +1);
                break;
            }
            else if (iaAxisExists[T] && !iTrkMode)
            {
		//For VAC514 robots only
		if(iTMDefineFlag & DFVAC514)
		{
		    // Store the current modulo and reset it to 0, to avoid the shortest path mode in Teach mode
		    if(ROReadGalilModulo(&lGalilModulo) == FAILURE)
			return FAILURE;
		    if(ROSetGalilModulo(0) == FAILURE)
			return FAILURE;
		}

                TMTeachAxis(T, +1);

		//For VAC514 robots only
		if(iTMDefineFlag & DFVAC514)
		{
		    // Restore the old modulo setting
		    if(ROSetGalilModulo(lGalilModulo) == FAILURE)
			return FAILURE;
		}

                break;
            }
            else
                goto error_exit;

        case 'K':       /* Retract or jog track to left. This changes the selected axis. */
            if (iTrkMode)
            {
                TMTeachAxis(TRK, -1);
                break;
            }
            else if (iaAxisExists[T] && !iFprMode && !iVacMode)
            {
                if (iUseDBM)
                    TMTeachAxis(DBM, -1);
                else
                    TMTeachAxis(R, -1);
                break;
            }
            else
                goto error_exit;

        case 'L':       /* Extend or jog track to right. This changes the selected axis. */
            if (iTrkMode)
            {
                TMTeachAxis(TRK, +1);
                break;
            }
            else if (iaAxisExists[T] && !iFprMode && !iVacMode)
            {
                if (iUseDBM)
                    TMTeachAxis(DBM, +1);
                else
                    TMTeachAxis(R, +1);
                break;
            }
            else
                goto error_exit;

        case 'P':       /* Jog Z down. This changes the selected axis. */
            if (iVacMode == VACZ1 && iaAxisExists[VACZ1])
            {
                TMTeachAxis(VACZ1, -1);
                break;
            }
            else if (iVacMode == VACZ2 && iaAxisExists[VACZ2])
            {
                TMTeachAxis(VACZ2, -1);
                break;
            }
            else if (iaAxisExists[T] && !iTrkMode && !iFprMode)
            {
                TMTeachAxis(Z, -1);
                break;
            }
            else
                goto error_exit;

        case 'Q':       /* Jog Z up. This changes the selected axis. */
            if (iVacMode == VACZ1 && iaAxisExists[VACZ1])
            {
                TMTeachAxis(VACZ1, +1);
                break;
            }
            else if (iVacMode == VACZ2 && iaAxisExists[VACZ2])
            {
                TMTeachAxis(VACZ2, +1);
                break;
            }
            else if (iaAxisExists[T] && !iTrkMode && !iFprMode)
            {
                TMTeachAxis(Z, +1);
                break;
            }
            else
                goto error_exit;

        case 'H':       /* Toggle the theta servo or flipper ON or OFF */
            if (iVacMode == VACZ1 && iaAxisExists[VACT1])
            {
                TMToggleServo(VACT1);
                break;
            }
            else if (iVacMode == VACZ2 && iaAxisExists[VACT2])
            {
                TMToggleServo(VACT2);
                break;
            }
            else if (iFprMode)
            {
                TMToggleServo(FLP);
                break;
            }
            else if (iaAxisExists[T] && !iTrkMode)
            {
                TMToggleServo(T);
                break;
            }
            else
                goto error_exit;

        case 'I':       /* Toggle the R axis servo or track ON or OFF */
            if (iTrkMode)
            {
                TMToggleServo(TRK);
                break;
            }
            else if (iaAxisExists[T] && !iFprMode && !iVacMode)
            {
                if (iUseDBM)
                    TMToggleServo(DBM);
                else
                    TMToggleServo(R);
                break;
            }
            else
                goto error_exit;

        case 'J':       /* Toggle the Z axis servo ON or OFF */
            if (iVacMode == VACZ1 && iaAxisExists[VACZ1])
            {
                TMToggleServo(VACZ1);
                break;
            }
            else if (iVacMode == VACZ2 && iaAxisExists[VACZ2])
            {
                TMToggleServo(VACZ2);
                break;
            }
            else if (iaAxisExists[T] && !iTrkMode && !iFprMode)
            {
                TMToggleServo(Z);
                break;
            }
            else
                goto error_exit;

        case 'O':       /* Refresh the teach pendant screen */
            if (TMDrawTeachScr() == FAILURE)
                return FAILURE;
            break;

        case 'R':       /* Decrease the jog speed for the selected axis */
            if (iTrkMode || iFprMode || iVacMode || iaAxisExists[T])
            {
                TMSetTchSpd(iSelAxis, -1);
                break;
            }
            else
                goto error_exit;

        case 'S':       /* Increase the jog speed for the selected axis */
            if (iTrkMode || iFprMode || iVacMode || iaAxisExists[T])
            {
                TMSetTchSpd(iSelAxis, +1);
                break;
            }
            else
                goto error_exit;

	// new keys for Smart-Motion Teaching
	case '1':
	case '2':
	case '3':
	case '4':
	case '6':
	case '7':
	case '8':
	case '9':
	    if(iSmartMode)
	    {
		TMMoveCartesian(cCharToProcessArg);
	    }
	    break;

        case 'V':       /* Select the output bit to toggle */
            if (++iOutputBit > 3)
                iOutputBit = 0;
            if (TTBeepTP(CLICK) == FAILURE)
                return FAILURE;
            break;

        case 'W':       /* Toggle the selected output bit ON or OFF */
            iGAOutByte = inb(IO_ROBOT_OUTPUT_A);
	    iGAOutByt2 = iGAOutByte>>iOutputBit;
	    if(iGAOutByt2 & 1)
		iGAOutByte &= ~(1<<iOutputBit);
	    else
		iGAOutByte |= (1<<iOutputBit);
	    IOWriteIO(-1, iGAOutByte, IO_ROBOT_OUTPUT_A);
	    
//            ROWriteGalilPort(0, iOutputBit, !(iGAOutByte & (1<<iOutputBit)));
            if (TTBeepTP(CLICK) == FAILURE)
                return FAILURE;
            break;

        case 'X':       /* Select the input bit to be read during main teach loop cycling */
            if (++iInputBit > 3)
                iInputBit = 0;
            if (TTBeepTP(CLICK) == FAILURE)
                return FAILURE;
            break;

        case 20:        /* ^T -- switch to track mode or back to robot mode */
            if (iaAxisExists[TRK] && !iFprMode)
            {
                if (iTrkMode)
                {
                    iTrkMode = FALSE;
                    iSelAxis = T;
                }
                else
                {
                    iTrkMode = TRUE;
                    iSelAxis = TRK;
                }
            }
            else if ((iaAxisExists[VACZ1] || iaAxisExists[VACT1] || iaAxisExists[VACZ2] || iaAxisExists[VACT2])
                        && !iFprMode)
            {
                if (iVacMode)
                {
                    iVacMode = FALSE;
                    iSelAxis = T;
                }
                else
                {
                    if ((iaAxisExists[VACZ1] && iaAxisExists[VACZ2]) || (iaAxisExists[VACT1] && iaAxisExists[VACT2]))
                    {
                        if (TTPrintsAt(4, 1, "Vac Inx?<0/1>   ") == FAILURE)
                            return FAILURE;
                        while (!iVacMode)
                        {
                            if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
                                return FAILURE;
                            switch (cButtonPressed)
                            {
                                case '0':
                                    iVacMode = VACZ1;
                                    iSelAxis = VACZ1;
                                    break;

                                case '1':
                                    iVacMode = VACZ2;
                                    iSelAxis = VACZ2;
                                    break;

                                case 'Z':
                                case ESC:
                                    TTPrintsAt(4, 1, "Msg:                ");
                                    TTPrintMessage(CLICK, "                    ");
                                    iVacMode = Z;
                                    iSelAxis = T;
                                    return SUCCESS;
                            }
                        }
                        if (iVacMode == Z)
                            iVacMode = FALSE;
                    }
                    else if (iaAxisExists[VACZ1] || iaAxisExists[VACT1])
                    {
                        iVacMode = VACZ1;
                        iSelAxis = VACZ1;
                    }
                    else if (iaAxisExists[VACZ2] || iaAxisExists[VACT2])
                    {
                        iVacMode = VACZ2;
                        iSelAxis = VACZ2;
                    }
                }
            }
            else
                goto error_exit;
            if (TMDrawTeachScr() == FAILURE)
                return FAILURE;
            if (TMDisplayAxes() == FAILURE)
                return FAILURE;
            if (TMDisplayOthers() == FAILURE)
                return FAILURE;
            break;

        default:
            /* Keys not defined above will end up here. */
            goto error_exit;
    }

    return SUCCESS;

error_exit:
    if (TTPrintMessage(ALERT, "?              ") == FAILURE)
        return FAILURE;
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMDrawTeachScr
 *
 * Abstract:    Redraw the part of the teach screen that is not
 *      constantly refreshed. Essentially, draw the robot/track
 *      axes labelling portion of the teach pendant screen.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: draw_teach_scr, draw_teach_scr1
 * Used in: teach
 *
 ***************************************************************/
int TMDrawTeachScr()
{
    /* Since this function acts as a screen refresh, clear everything first. */
    if (TTClearScreen() == FAILURE)
        return FAILURE;
    if (iVacMode == VACZ1)
    {
        if (iaAxisExists[VACT1])
        {
            if (TTPrintsAt(1, 1, "t: ") == FAILURE)
                return FAILURE;
        }
        if (iaAxisExists[VACZ1])
        {
            if (TTPrintsAt(2, 1, "r: ") == FAILURE)
                return FAILURE;
        }
    }
    else if (iVacMode == VACZ2)
    {
        if (iaAxisExists[VACT2])
        {
            if (TTPrintsAt(1, 1, "w: ") == FAILURE)
                return FAILURE;
        }
        if (iaAxisExists[VACZ2])
        {
            if (TTPrintsAt(2, 1, "z: ") == FAILURE)
                return FAILURE;
        }
    }
    else if (iTrkMode)
    {
        if (TTPrintsAt(1, 1, "Trk:") == FAILURE)
            return FAILURE;
    }
    else if (iFprMode)
    {
        if (TTPrintsAt(1, 1, "Fpr:") == FAILURE)
            return FAILURE;
    }
    else if (iaAxisExists[T])
    {
        if (TTPrintsAt(1, 1, "T:") == FAILURE)
            return FAILURE;
        if (iUseDBM)
        {
            if (TTPrintsAt(2, 1, "W:") == FAILURE)
                return FAILURE;
        }
        else
        {
            if (TTPrintsAt(2, 1, "R:") == FAILURE)
                return FAILURE;
        }
        if (TTPrintsAt(3, 1, "Z:") == FAILURE)
            return FAILURE;
    }
    if (TTPrintsAt(4, 1, "Msg:") == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMDisplayAxes
 *
 * Abstract:    Display part of teach screen associated with
 *      the axes that is constantly changing and needs to be
 *      refreshed. Essentially, print out the axis specific
 *      information to the teach pendant screen: current position,
 *      limit switch status, jog speed, and selected axis.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: display_axes, display_axes1
 * Used in: teach
 *
 ***************************************************************/
int TMDisplayAxes()
{
    int iAxis;
    int iAxisTemp;
    int iLimit;
    int iStartIndex = 0;
    int iEndIndex = 0;
    long laCurrPos[8] = {0,0,0,0,0,0,0,0};
    long laCurrDBM[8] = {0,0,0,0,0,0,0,0};
    char *cpTchSpd;
    char caOutMsg[21];
    int iRow, iCol;
    unsigned long ulAxesToRead = 0;

    /* Get the current axes positions and check limit switches. */
    if (iVacMode == VACZ1)
    {
        if (iaAxisExists[VACZ1])
            ulAxesToRead |= ulaMechAxisDef[VACZ1];
        if (iaAxisExists[VACT1])
            ulAxesToRead |= ulaMechAxisDef[VACT1];
    }
    else if (iVacMode == VACZ2)
    {
        if (iaAxisExists[VACZ2])
            ulAxesToRead |= ulaMechAxisDef[VACZ2];
        if (iaAxisExists[VACT2])
            ulAxesToRead |= ulaMechAxisDef[VACT2];
    }
    else if (iTrkMode)
    {
        ulAxesToRead = ulaMechAxisDef[TRK];
    }
    else if (iFprMode)
    {
        ulAxesToRead = ulaMechAxisDef[FLP];
    }
    else if (iaAxisExists[T])
    {
        ulAxesToRead = ROAxisAllRobot();
    }
    if (ROReadCurrentPosition(ulAxesToRead, laCurrPos) == FAILURE)
        return FAILURE;
    if (iUseDBM && ulAxesToRead==ROAxisAllRobot())
    {
        if (ROReadCurrentPosition(ulaMechAxisDef[DBM], laCurrDBM) == FAILURE)
            return FAILURE;
        laCurrPos[iaMechAxisIdx[DBM]] = laCurrDBM[iaMechAxisIdx[DBM]];
    }
//    if (ROCheckSwitches(GA_CARD_0) == FAILURE)
//        return FAILURE;
//    if (ROCheckSwitches(GA_CARD_1) == FAILURE)
//        return FAILURE;

    /* Depending on whether robot or track mode is selected... */
    if (iVacMode == VACZ1)
    {
        if (iaAxisExists[VACZ1] && !iaAxisExists[VACT1])
        {
            iStartIndex = VACZ1;
            iEndIndex = VACZ1+1;
        }
        else if (!iaAxisExists[VACZ1] && iaAxisExists[VACT1])
        {
            iStartIndex = VACT1;
            iEndIndex = VACT1+1;
        }
        else
        {
            iStartIndex = VACT1;
            iEndIndex = VACZ1+1;
        }
    }
    else if (iVacMode == VACZ2)
    {
        if (iaAxisExists[VACZ2] && !iaAxisExists[VACT2])
        {
            iStartIndex = VACZ2;
            iEndIndex = VACZ2+1;
        }
        else if (!iaAxisExists[VACZ2] && iaAxisExists[VACT2])
        {
            iStartIndex = VACT2;
            iEndIndex = VACT2+1;
        }
        else
        {
            iStartIndex = VACT2;
            iEndIndex = VACZ2+1;
        }
    }
    else if (iTrkMode)
    {
        iStartIndex = TRK;
        iEndIndex = TRK+1;
    }
    else if (iFprMode)
    {
        iStartIndex = FLP;
        iEndIndex = FLP+1;
    }
    else if (iaAxisExists[T])
    {
        iStartIndex = T;
        iEndIndex = Z+1;
    }
    /* ...go through the relevant axes, collecting and printing info to the TP. */
    for (iAxis=iStartIndex; iAxis<iEndIndex; iAxis++)
    {
        /* Dependent on read_galil_pos returning track info in index 3.
         * Print the current position information. */
        if(iAxis == R && iUseDBM)
            iAxisTemp = DBM;
        else
            iAxisTemp = iAxis;

		//For VAC514 robots only and if Galil modulo was set to other then 0
        //we need to recalculate T position on the display if modulo was set in order to
        //have same position on the Teach screen and in the Terminal mode
        if((iTMDefineFlag & DFVAC514) && (lGalilModulo != 0) && (iAxisTemp == T))
		{
			long lTemp;

            lTemp = laCurrPos[iaMechAxisIdx[iAxisTemp]];
            if(lTemp < 0)
			{
                lTemp = lTemp%lGalilModulo + lGalilModulo;
			}
            else
            {
                lTemp = lTemp%lGalilModulo;
            }
			laCurrPos[iaMechAxisIdx[iAxisTemp]] = lTemp;
		}

//        ltoa(laCurrPos[iaMechAxisIdx[iAxisTemp]], caOutMsg, 10);
	sprintf(caOutMsg, "%ld", laCurrPos[iaMechAxisIdx[iAxisTemp]]);
        strcat(caOutMsg, "            ");

        /* Indicate that the axis is beyond its acceptable range
         * if the limit switch has been triggered. */
        iLimit = 0;
        if (ROGetLMPosFlag() & ulaMechAxisDef[iAxisTemp])
            iLimit +=1;
        if (ROGetLMNegFlag() & ulaMechAxisDef[iAxisTemp])
            iLimit +=2;
        caOutMsg[7] = caLMSymbol[iLimit];

        /* Print out the jog speeeeed. */
        cpTchSpd = pcaTchSpdStr[iaSpeedIndex[iAxisTemp]-1];
        caOutMsg[9]  = *cpTchSpd++;
        caOutMsg[10] = *cpTchSpd;

        /* Put an asterisk next to the currently selected axis. */
        if (iAxisTemp == iSelAxis)
            caOutMsg[11] = '*';
        caOutMsg[13] = 0;

        /* Print out the composed string to the teach pendant LCD screen. */
        if (iVacMode == VACZ1)
        {
            if (iAxis == VACT1)
                iRow = 1;
            else
                iRow = 2;
            iCol = 4;
        }
        else if (iVacMode == VACZ2)
        {
            if (iAxis == VACT2)
                iRow = 1;
            else
                iRow = 2;
            iCol = 4;
        }
        else if (iTrkMode || iFprMode)
        {
            iRow = 1;
            iCol = 5;
        }
        else if (iaAxisExists[T])
        {
            iRow = iAxis+1;
            iCol = 3;
        }
        if (TTPrintsAt(iRow, iCol, caOutMsg) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMDisplayOthers
 *
 * Abstract:    Print out the non-robot/track-specific
 *      information to the teach pendant screen. Update right
 *      hand side of display: station name, input bit status,
 *      output bit condition, and scan mode status. The only reason
 *      this is constantly refreshed is because the input bit
 *      status may change. Seems a little overkill, doesn't it?
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: display_others
 * Used in: teach
 *
 ***************************************************************/
int TMDisplayOthers()
{
    int iGAByte;        /* The state of the output or input port. */
    char caOutMsg[21];  /* The string to be printed to the teach pendant. */

    /* Print out the station number that is currently being taught. */
    if (TTPrintcAt(1, 18, cTchStn) == FAILURE)
        return FAILURE;

    if (!iTrkMode && !iFprMode)
    {
        /* Print an asterisk to the upper right corner of the teach
         * pendant LCD screen if the user has selected scanner mode.
         * Teach mode does not check for the existence of a scanner. */
        if (iScanMode)
        {
            if (TTPrintcAt(1, 19, '*') == FAILURE)
                return FAILURE;
        }

        /* Get and print the current state of the selected OUTput bit. */
//        itoa(iOutputBit, caOutMsg, 10);
	sprintf(caOutMsg, "%d", iOutputBit);
        iGAByte = inb(IO_ROBOT_OUTPUT_A);
        if (iGAByte & (1<<iOutputBit))
            strcpy(&caOutMsg[1], " ON ");
        else
            strcpy(&caOutMsg[1], " OFF");
        if (TTPrintsAt(2, 16, caOutMsg) == FAILURE)
            return FAILURE;

        /* Get and print the current state of the selected INput bit. */
//        itoa(iInputBit, caOutMsg, 10);
	sprintf(caOutMsg, "%d", iInputBit);
        iGAByte = inb(IO_ROBOT_INPUT_F);
        if (iGAByte & (1<<iInputBit))
            strcpy(&caOutMsg[1], " ON ");
        else
            strcpy(&caOutMsg[1], " OFF");
        if (TTPrintsAt(3, 16, caOutMsg) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMDoAlign
 *
 * Abstract:    Non-simulated wafer align. Actually calls the
 *      ALIGN macro.
 *
 * Parameters:  None
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: do_align
 * Used in: teach
 *
 ***************************************************************/
int TMDoAlign()
{
    int iQuitLoop;      /* For user selection of "Simulate (Y/N)". */
    int iAlign;         /* Temp Flag used to select Align Code if user selects 'Y'. */
    char cButtonPressed;/* For user selection of "Align (Y/N)". */

    if (TMTchMotionCheck((unsigned)(ROAxisAllPre())) == FAILURE)
        return FAILURE;

    if (TTPrintsAt(4, 1, "ALIGN?<Y/N>     ") == FAILURE)
        return FAILURE;

    /* Asks the user to Align or not. */
    iQuitLoop = FALSE;
    while (!iQuitLoop)
    {
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            case CR:
            case 'Y':
                iAlign = TRUE;
                iQuitLoop = TRUE;
                break;

            case 'N':
                iAlign = FALSE;
                iQuitLoop = TRUE;
                break;

            case 'Z':
            case ESC:
                TTPrintsAt(4, 1, "Msg:                ");
                TTPrintMessage(CLICK, "                    ");
                return FAILURE;
        }
    }

    if (!iAlign)
    {
        TTPrintMessage(CLICK, "               ");
        return SUCCESS;
    }

    if (iAlign)
    {
        if (TTPrintsAt(4, 1, "Msg:              ") == FAILURE)
            return FAILURE;
        if (TTPrintMessage(SHORTBELL, "ALIGN") == FAILURE)
            return FAILURE;
        if (TMRunCommand("ALIGN") == FAILURE)
            return FAILURE;
        if (TTPrintMessage(SHORTBELL, "Done") == FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMChgCoord
 *
 * Abstract:    Routine to handle changing of the teach coordinate
 *      station. It waits for the user to input the new station
 *      number, checks the new number for validity, and updates
 *      all relevant variables locally and globally. The TP
 *      LCD screen update is done through TMDisplayOthers.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: chg_coord
 * Used in: teach
 *
 ***************************************************************/
int TMChgCoord()
{
    char cButtonPressed;
    char cOldStn;
    int iQuitLoop = FALSE;
    int iStn, iValue;

    /* Save the current station. If the user presses ESC it will be restored. */
    cOldStn = cTchStn;
    if (TTPrintMessage(CLICK, "Alpha,CR or ESC") == FAILURE)
        return FAILURE;

    while (!iQuitLoop)
    {
        /* Get the user keypress. */
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            case ESC:
                /* Reset the old station name...then exit. */
                cTchStn = cOldStn;
	        iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
	        if (FIOGetStnlscsi(iStn, 5, &iValue) == FAILURE)
		    return FAILURE; 
	        if (iValue == 313)
		    iSmartMode = TRUE;
	        else
		    iSmartMode = FALSE;
                if (RGSetCharacter(60, cTchStn) == FAILURE)
                    return FAILURE;

            case CR:
                /* Exit keeping the new (or old, restored from ESC) station name. */
                if (TTPrintMessage(NO_BEEP, "               ") == FAILURE)
                    return FAILURE;
                /* Before quit, set the global iUseDBM, so the right axis info can
                 * be used/displayed. */
                if (iaAxisExists[DBM])
                {
                    iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
                    if (FIOGetStnEEindex(iStn, &iUseDBM) == FAILURE)
                        return FAILURE;
                }
                else
                    iUseDBM = 0;
                if (TMDrawTeachScr() == FAILURE)
                    return FAILURE;
                if (TMDisplayAxes() == FAILURE)
                    return FAILURE;
                if (TMDisplayOthers() == FAILURE)
                    return FAILURE;
                iQuitLoop = TRUE;
                break;

            default:
                /* If an appropriate station name was entered, make it current. */
                if (isalpha(cButtonPressed))
                {
                    /* This is a ridiculous conversion. If the new station selected
                     * is simply the old station, but lowercase, instead of just setting
                     * the cTchStn variable equal to the new station letter, it actually
                     * converts the capital letter to lowercase. Go figure. */
//                    if (cButtonPressed == (cTchStn & ~0x20))
//                        cTchStn ^= 0x20;
//                    else
//                        cTchStn = cButtonPressed;
                    cTchStn = cButtonPressed;
		    iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
		    if (FIOGetStnlscsi(iStn, 5, &iValue) == FAILURE)
			return FAILURE; 
		    if (iValue == 313)
			iSmartMode = TRUE;
		    else
			iSmartMode = FALSE;

                    if (TTPrintcAt(1, 18, cTchStn) == FAILURE)
                        return FAILURE;
                    if (RGSetCharacter(60, cButtonPressed) == FAILURE)
                        return FAILURE;
                }
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMSetTchSpd
 *
 * Abstract:    Changes the jog speed for the selected axis.
 *      It changes the speed index, changes the speeds in the
 *      Galil card, and prints the change to the teach pendant
 *      LCD screen.
 *
 * Parameters:
 *      iAxis       (in) The axis to change the jog speed of
 *      iDirection  (in) Increase or decrease the speed
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: set_tch_spd, set_tch_spd1
 * Used in: teach
 *
 * NOTE: In the iaSpeedIndex array the column in the 2-D iaTeachSpeed
 *      array is saved by axis. This number is also printed
 *      to the screen as the speed setting for the user to reference.
 *      It ranges from 1 to 10. Then the actual jog speed can be
 *      retrieved from the 2-D array by the speed index and axis.
 *
 ***************************************************************/
int TMSetTchSpd(int iAxis, int iDirection)
{
    int *ipSpdIndex;
    long laParms[8];
    int iRow, iCol;

    ROUpdateTS(FALSE);

    /* Validate parameters passed in. */
    if (!iaAxisExists[iAxis])
        return FAILURE;
    if ((iDirection != -1) && (iDirection != +1))
        return FAILURE;

    /* Gets the current index to the jog speed. */
    ipSpdIndex = &iaSpeedIndex[iAxis];

    /* Increment it or decrement it depending on direction. */
    *ipSpdIndex += iDirection;
    if (*ipSpdIndex > 10)
        *ipSpdIndex = 1;  /* 10 wraps back to 1 */
    if (*ipSpdIndex < 1)
        *ipSpdIndex = 1;   /* 1 stays at 1 */

    /* Get and set the JOG speeds and accelerations for the selected axis. */
    laParms[iaMechAxisIdx[iAxis]] = (long)(iaTeachSpeed[iAxis][*ipSpdIndex]);
    if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
        laParms[iaMechAxisIdx[iAxis]] *= lUnitConvAngle;
    else
        laParms[iaMechAxisIdx[iAxis]] *= lUnitConvDist;
    if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_SPEED) == FAILURE)
        return FAILURE;

    laParms[iaMechAxisIdx[iAxis]] = (long)(iaTeachAccel[iAxis][*ipSpdIndex]);
    if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
        laParms[iaMechAxisIdx[iAxis]] *= lUnitConvAngle;
    else
        laParms[iaMechAxisIdx[iAxis]] *= lUnitConvDist;
    if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;

    laParms[iaMechAxisIdx[iAxis]] = (long)(iaTeachAccel[iAxis][*ipSpdIndex]);
    if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
        laParms[iaMechAxisIdx[iAxis]] *= lUnitConvAngle;
    else
        laParms[iaMechAxisIdx[iAxis]] *= lUnitConvDist;
    if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_ACCEL) == FAILURE)
            return FAILURE;

    // use R speed index to set Smart Motion Vector Distance for teaching
    if (iAxis == R)
    	glVectorLength = iaSmartInterval[*ipSpdIndex];

    /* Print the new speed index to the TP LCD screen. */
    iCol = 12;
    if (iVacMode == VACZ1)
    {
        if (iAxis == VACT1)
            iRow = 1;
        else
            iRow = 2;
    }
    else if (iVacMode == VACZ2)
    {
        if (iAxis == VACT2)
            iRow = 1;
        else
            iRow = 2;
    }
    else if (iTrkMode || iFprMode)
    {
        iRow = 1;
	iCol = 14;
    }
    else if (iAxis == DBM)
        iRow = R+1;
    else if (iaAxisExists[T])
        iRow = iAxis+1;

    if (TTPrintsAt(iRow, iCol, pcaTchSpdStr[*ipSpdIndex-1]) == FAILURE)
        return FAILURE;
    if (TTBeepTP(CLICK) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMTeachAxis
 *
 * Abstract:    Sets the selected axis and jogs it or steps it.
 *      It also checks for limit switch conditions, like the limit
 *      switch can't be triggered already or the move can't
 *      start or go beyond the software limit switch positions.
 *
 * Parameters:
 *      iAxis       (in) The axis to jog
 *      iDirection  (in) The direction in which to jog it
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: teach_axis
 * Used in: teach
 *
 ***************************************************************/
int TMTeachAxis(int iAxis, int iDirection)
{
    int iMode;          /* 0 = step; 1 = jog */
    long lStep;         /* In step mode, the distance to step. */
    char cButtonPressed;/* Gets the KNP mode null response to end jog mode. */
    long laPosition[8] = {0,0,0,0,0,0,0,0};
//    long laSoftPosLim[8] = {0,0,0,0,0,0,0,0};
//    long laSoftNegLim[8] = {0,0,0,0,0,0,0,0};
    int iRow;

    /* Validate parameters passed in. */
    if (!iaAxisExists[iAxis])
        return FAILURE;
    if ((iDirection != -1) && (iDirection != +1))
        return FAILURE;

    /* Change axis selection. */
    if (!iTrkMode && !iFprMode)
    {
        if (iVacMode == VACZ1)
        {
            if (iSelAxis == VACT1)
                iRow = 1;
            else
                iRow = 2;
        }
        else if (iVacMode == VACZ2)
        {
            if (iSelAxis == VACT2)
                iRow = 1;
            else
                iRow = 2;
        }
        else if (iSelAxis == DBM)
            iRow = R+1;
        else
            iRow = iSelAxis+1;

        if (TTPrintcAt(iRow, 14, ' ') == FAILURE)
            return FAILURE;

        iSelAxis = iAxis;

        if (iVacMode == VACZ1)
        {
            if (iSelAxis == VACT1)
                iRow = 1;
            else
                iRow = 2;
        }
        else if (iVacMode == VACZ2)
        {
            if (iSelAxis == VACT2)
                iRow = 1;
            else
                iRow = 2;
        }
        else if (iSelAxis == DBM)
            iRow = R+1;
        else
            iRow = iSelAxis+1;

        if (TTPrintcAt(iRow, 14, '*') == FAILURE)
            return FAILURE;
    }

    /* Go to jog mode if the KNP null doesn't come back in less than 250 ms.
     * Step mode is a key press and release. Jog mode is held.
     * The key return check is done 120 lines further along. We just spend
     * the waiting time doing something useful instead of just sitting and waiting.*/
    if (TISetCounter(iCounterIndex, 50) == FAILURE)
        return FAILURE;

    /* Make sure the axis is not in motion. */
    if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[iAxis])) == FAILURE)
        return FAILURE;

    /* Verify that the axis is not already passed the hardware limit switch. */
    if (iDirection == 1)
    {
        if (ROGetLMPosFlag()&ulaMechAxisDef[iAxis])
        {
            TTPrintMessage(ALERT, " Limit switch  ");
            return FAILURE;
        }
    }
    else
    {
        if (ROGetLMNegFlag()&ulaMechAxisDef[iAxis])
        {
            TTPrintMessage(ALERT, " Limit switch  ");
            return FAILURE;
        }
    }

    /* Get some information necessary to set up the move. */
    if (ROReadCurrentPosition(ulaMechAxisDef[iAxis], laPosition) == FAILURE)
        return FAILURE;

    /* Verify that the axis is not already passed the software limit switch. */
    if (iDirection == 1)
    {
        if (laPosition[iaMechAxisIdx[iAxis]] >= glaSoftPosLim[iaMechAxisIdx[iAxis]])
        {
//printf("axis=%d idx=%d pos=%d pl=%d def=%d\n",iAxis,iaMechAxisIdx[iAxis],laPosition[iaMechAxisIdx[iAxis]],glaSoftPosLim[iaMechAxisIdx[iAxis]],ulaMechAxisDef[iAxis]);
            TTPrintMessage(ALERT, cpaSoftPosLimMessage[iAxis]);
            return FAILURE;
        }
    }
    else
    {
        if (laPosition[iaMechAxisIdx[iAxis]] <= glaSoftNegLim[iaMechAxisIdx[iAxis]])
        {
            TTPrintMessage(ALERT, cpaSoftNegLimMessage[iAxis]);
            return FAILURE;
        }
    }

	/* Use timeout to decide step or jog mode */
    iMode = 1;
    while (!TICountExpired(iCounterIndex))
    {
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        if (cButtonPressed != EOF)
        {
            iMode = 0;
            break;
        }
    }

    /*** STEP mode ***/
    if (iMode == 0)
    {
        /* Calculate the position to move to. */
        if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
            lStep = (iDirection*iaSpeedIndex[iAxis]*lUnitConvAngle*5)+laPosition[iaMechAxisIdx[iAxis]];
        else
            lStep = (iDirection*iaSpeedIndex[iAxis]*lUnitConvDist*5)+laPosition[iaMechAxisIdx[iAxis]];
        /* Make sure that the next step isn't going to put the
         * axis beyond the limit switch. */
        if (iDirection == 1)
        {
            if (lStep > glaSoftPosLim[iaMechAxisIdx[iAxis]])
            {
                TTPrintMessage(ALERT, cpaSoftPosLimMessage[iAxis]);
                return FAILURE;
            }
        }
        else
        {
            if (lStep < glaSoftNegLim[iaMechAxisIdx[iAxis]])
            {
                TTPrintMessage(ALERT, cpaSoftNegLimMessage[iAxis]);
                return FAILURE;
            }
        }

        /* Move the axis the step distance. */
        if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
            laPosition[iaMechAxisIdx[iAxis]] = iDirection*iaSpeedIndex[iAxis]*lUnitConvAngle*5;
        else
            laPosition[iaMechAxisIdx[iAxis]] = iDirection*iaSpeedIndex[iAxis]*lUnitConvDist*5;
        if ((ROMoveToRel(ulaMechAxisDef[iAxis], laPosition) == FAILURE)) // || (GAGetGalilErrorFlag()))
        {
            ROStopMotion(ulaMechAxisDef[iAxis]);
            TTPrintMessage(ALERT, "?              ");
            return FAILURE;
        }
        /* Otherwise wait for the move to complete. */
        else
        {
            if (TMWaitTchMove() == FAILURE)
                return FAILURE;
            if (TTBeepTP(CLICK) == FAILURE)
                return FAILURE;
        }

        return SUCCESS;
    }

    /*** JOG mode ***/
    /* Verify that the axis is not already passed the software limit switch. */
    if (iDirection == 1)
    {
        laPosition[iaMechAxisIdx[iAxis]] = glaSoftPosLim[iaMechAxisIdx[iAxis]];
    }
    else
    {
        laPosition[iaMechAxisIdx[iAxis]] = glaSoftNegLim[iaMechAxisIdx[iAxis]];
    }

    /* Begin a jog move. */
    if ((ROMoveToAbs(ulaMechAxisDef[iAxis], laPosition) == FAILURE) || (GAGetGalilErrorFlag()))
    {
        ROStopMotion(ulaMechAxisDef[iAxis]);
        TTPrintMessage(ALERT, "?              ");
        return FAILURE;
    }

    /* Wait for the user to stop the move by releasing the depressed key.
     * This returns a 0 because of KNP mode. But any keypress will stop it. */
    cButtonPressed = EOF;
    while (cButtonPressed == EOF)
    {
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            break;
        if (TMDisplayAxes() == FAILURE)
            break;
        if (TMDisplayOthers() == FAILURE)
            break;
    }

    /* As soon as the user presses a key, stop the move.
     * If the user reaches the hard stops, motion will automatically abort. */
    if (ROStopMotion(ulaMechAxisDef[iAxis]) == FAILURE)
        return FAILURE;

    /* Wait for the deceleration to complete. */
    if (TMWaitTchMove() == FAILURE)
        return FAILURE;
    if (TTBeepTP(CLICK) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMStoreCoord
 *
 * Abstract:    Store coordinate to memory -- HERE command.
 *      This checks that all axes are homed. (All axes, including
 *      the track, are saved at once.) The software limits
 *      are checked to make sure the robot hasn't exceeded them.
 *      Then the coordinates are saved to the data file. In the
 *      case of scan mode, the user is allowed to calibrate the
 *      scanner at the newly taught station.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: store_coord
 * Used in: teach
 *
 ***************************************************************/
int TMStoreCoord()
{
    int iAxis, iStn, iQuitIndex;
    char cButtonPressed;
    long laPosition[8] = {0,0,0,0,0,0,0,0};
    long laPosDBM[8] = {0,0,0,0,0,0,0,0};
    long laSoftPosLim[8] = {0,0,0,0,0,0,0,0};
    long laSoftNegLim[8] = {0,0,0,0,0,0,0,0};
    long laPosLimDBM[8] = {0,0,0,0,0,0,0,0};
    long laNegLimDBM[8] = {0,0,0,0,0,0,0,0};
    long laScanOffset[8] = {0,0,0,0,0,0,0,0};
    char caCommandLine[20];

    caCommandLine[0] = 0;
    /* Check that the axis is not in motion. */
    if (iaAxisExists[VACZ1] || iaAxisExists[VACT1] || iaAxisExists[VACZ2] || iaAxisExists[VACT2])
    {
        if (~ROGetHomed() & (ulaMechAxisDef[VACZ1] | ulaMechAxisDef[VACT1] | ulaMechAxisDef[VACZ2] | ulaMechAxisDef[VACT2]))
        {
            TTPrintMessage(ALERT, " Vac Not homed ");
            return FAILURE;
        }
    }
    if (iaAxisExists[TRK])
    {
        if (~ROGetHomed() & ulaMechAxisDef[TRK])
        {
            TTPrintMessage(ALERT, " Trk Not homed ");
            return FAILURE;
        }
    }
    if (iaAxisExists[FLP])
    {
       if (~ROGetHomed() & ulaMechAxisDef[FLP])
       {
            TTPrintMessage(ALERT, " Fpr Not homed ");
            return FAILURE;
        }
    }
    if (iUseDBM)
    {
        if (~ROGetHomed() & ulaMechAxisDef[DBM])
        {
            TTPrintMessage(ALERT, " Dbm Not homed ");
            return FAILURE;
        }
    }
    if (iaAxisExists[T])
    {
        if (~ROGetHomed() & ROAxisAllRobot())
        {
            TTPrintMessage(ALERT, " Rbt Not homed ");
            return FAILURE;
        }
    }

    /* Convert the character station name to a number.
     * The conversion is:
     *      'A'-'Z'     0-25
     *      'a'-'z'     26-51   */
    iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;

    if (iVacMode != VACZ1 && iVacMode != VACZ2)
    {
        /* Save EE# for this station */
        if (FIOSetStnEEindex(iStn, iUseDBM) == FAILURE)
            return FAILURE;

        if (iaAxisExists[TRK])
        {
            /* Get the current track position and save it. */
            laPosition[0] = laPosition[1] = laPosition[2] = laPosition[3] = 0;
            if (ROReadCurrentPosition(ulaMechAxisDef[TRK], laPosition) == FAILURE)
                return FAILURE;
            /* Coordinate special item 9 is the standard track position storage. */
            if (FIOSetStnVals(iStn, TRACK_POS, laPosition[iaMechAxisIdx[TRK]]) == FAILURE)
                return FAILURE;
            /* Removed because of Double Beep/Double Message. */
            /*if (TTPrintMessage(SHORTBELL, "Trk Pos Stored ") == FAILURE) return FAILURE;*/
        }
        if (iaAxisExists[FLP])
        {
            /* Get some information necessary to set up the move. */
            if (ROReadCurrentPosition(ulaMechAxisDef[FLP], laPosition) == FAILURE)
                return FAILURE;
            /* Save the coordinates to the datafile. Use regular station coordinates. */
            if (FIOSetStnVals(iStn, FLP_COORD, laPosition[iaMechAxisIdx[TRK]]) == FAILURE)
                return FAILURE;
            /* Removed because of Double Beep/Double Message */
            /*if (TTPrintMessage(SHORTBELL, "Position Stored") == FAILURE) return FAILURE;*/
        }
        if (iaAxisExists[T])
        {
            /* Get the current robot position. */
            if (ROReadCurrentPosition(ROAxisAllRobot(), laPosition) == FAILURE)
                return FAILURE;
            if (ROGetParameter(TRUE, ROAxisAllRobot(), laSoftNegLim, SOFT_NEG_LIMIT) == FAILURE)
                return FAILURE;
            if (ROGetParameter(TRUE, ROAxisAllRobot(), laSoftPosLim, SOFT_POS_LIMIT) == FAILURE)
                return FAILURE;
            if (iUseDBM)
            {
                /* Get the current robot position. */
                if (ROReadCurrentPosition(ulaMechAxisDef[DBM], laPosDBM) == FAILURE)
                    return FAILURE;
                laPosition[iaMechAxisIdx[DBM]] = laPosDBM[iaMechAxisIdx[DBM]];
                if (ROGetParameter(TRUE, ulaMechAxisDef[DBM], laNegLimDBM, SOFT_NEG_LIMIT) == FAILURE)
                    return FAILURE;
                if (ROGetParameter(TRUE, ulaMechAxisDef[DBM], laPosLimDBM, SOFT_POS_LIMIT) == FAILURE)
                    return FAILURE;
                laSoftNegLim[iaMechAxisIdx[DBM]] = laNegLimDBM[iaMechAxisIdx[DBM]];
                laSoftPosLim[iaMechAxisIdx[DBM]] = laPosLimDBM[iaMechAxisIdx[DBM]];
            }

            /* Check the position against the limit switches. Include all appropriate offsets. */
            for (iAxis=0; iAxis<3; iAxis++)
            {
                if (FIOGetParamVals(ROBOTFILE, SCAN_OFFSET, laScanOffset) == FAILURE)
                    return FAILURE;
                if (iAxis == R && iUseDBM)
                {
                    if ((laPosition[iaMechAxisIdx[DBM]] + (iScanMode ? laScanOffset[iAxis] : 0)) < laSoftNegLim[iAxis])
                    {
                        TTPrintMessage(ALERT, cpaSoftNegLimMessage[DBM]);
                        return FAILURE;
                    }
                    if ((laPosition[iaMechAxisIdx[DBM]] + (iScanMode ? laScanOffset[iAxis] : 0)) > laSoftPosLim[iAxis])
                    {
                        TTPrintMessage(ALERT, cpaSoftPosLimMessage[DBM]);
                        return FAILURE;
                    }
                }
                else
                {
                    if ((laPosition[iAxis] + (iScanMode ? laScanOffset[iAxis] : 0)) < laSoftNegLim[iAxis])
                    {
                        TTPrintMessage(ALERT, cpaSoftNegLimMessage[iAxis]);
                        return FAILURE;
                    }
                    if ((laPosition[iAxis] + (iScanMode ? laScanOffset[iAxis] : 0)) > laSoftPosLim[iAxis])
                    {
                        TTPrintMessage(ALERT, cpaSoftPosLimMessage[iAxis]);
                        return FAILURE;
                    }
                }
            }
        }
    }

    if (!iScanMode)
    {
        if (iVacMode == VACZ1 || iVacMode == VACZ2)
        {
            if (iVacMode == VACZ1)
            {
                /* Coordinate special item 9 is the standard track position storage.
                 * But in a vacuum indexer system there can be no track so it is used
                 * to store which indexer's station position is being saved. */
                if (FIOSetStnVals(iStn, TRACK_POS, 0) == FAILURE)
                    return FAILURE;
            }
            else
            {
                if (FIOSetStnVals(iStn, TRACK_POS, 1) == FAILURE)
                    return FAILURE;
            }

            for (iAxis=0; iAxis<=1; iAxis++)
            {
                if (iaAxisExists[iVacMode+iAxis])
                {
                    /* Get the current vacuum indexer axis position. */
                    if (ROReadCurrentPosition(ulaMechAxisDef[iVacMode+iAxis], laPosition) == FAILURE)
                        return FAILURE;
                    /* Coordinate special items 12 and 13 are the standard vacuum indexer position storage. */
                    if (FIOSetStnlscsi(iStn, iAxis+2, laPosition[iaMechAxisIdx[iVacMode+iAxis]]) == FAILURE)
                        return FAILURE;
                }
            }
        }
        else if (iaAxisExists[T])
        {
            /* Save the coordinates to the datafile. Use regular station coordinates. */
            for (iAxis=0; iAxis<3; iAxis++)
            {
                if (iAxis == R && iUseDBM)
                {
                    if (FIOSetStnCoord(iStn, iAxis, laPosition[iaMechAxisIdx[DBM]]) == FAILURE)
                        return FAILURE;
                }
                else
                {
                    if (FIOSetStnCoord(iStn, iAxis, laPosition[iAxis]) == FAILURE)
                        return FAILURE;
                }
            }
        }
        if (TTPrintMessage(SHORTBELL, "Position Stored") == FAILURE)
            return FAILURE;
    }
    else
    {
        if (iaAxisExists[T] || iVacMode == VACZ1 || iVacMode == VACZ2)
        {
            if (iVacMode == VACZ1 || iVacMode == VACZ2)
            {
                if (iVacMode == VACZ1)
                {
                    /* Coordinate special item 9 is the standard track position storage.
                     * But in a vacuum indexer system there can be no track so it is used
                     * to store which indexer's station position is being saved. */
                    if (FIOSetStnVals(iStn, TRACK_POS, 0) == FAILURE)
                        return FAILURE;
                }
                else
                {
                    if (FIOSetStnVals(iStn, TRACK_POS, 1) == FAILURE)
                        return FAILURE;
                }

                for (iAxis=0; iAxis<=1; iAxis++)
                {
                    if (iaAxisExists[iVacMode+iAxis])
                    {
                        /* Get the current vacuum indexer axis position. */
                        if (ROReadCurrentPosition(ulaMechAxisDef[iVacMode+iAxis], laPosition) == FAILURE)
                            return FAILURE;
                        /* Coordinate special items 14 and 15 are the standard vacuum indexer scanner position storage. */
                        if (FIOSetStnlscsi(iStn, iAxis+4, laPosition[iaMechAxisIdx[iVacMode+iAxis]]) == FAILURE)
                            return FAILURE;
                    }
                }
            }
            else if (iaAxisExists[T])
            {
                /* Save the coordinates to the datafile. Use scan station coordinates. */
                for (iAxis=0; iAxis<3; iAxis++)
                {
                    if (iAxis == R && iUseDBM)
                    {
                        if (FIOSetStnScanCoord(iStn, iAxis, laPosition[iaMechAxisIdx[DBM]]+laScanOffset[iAxis]) == FAILURE)
                            return FAILURE;
                    }
                    else
                    {
                        if (FIOSetStnScanCoord(iStn, iAxis, laPosition[iAxis]+laScanOffset[iAxis]) == FAILURE)
                            return FAILURE;
                    }
                }
            }
            if (TTPrintMessage(SHORTBELL, "Position Stored") == FAILURE)
                return FAILURE;
            if (TIDelay(1000) == FAILURE)
                return FAILURE;

            /* Scan mode allows the user to calibrate the scanner at the
             * newly saved station. */
            if (TTPrintMessage(NO_BEEP, "Calibrate?<Y/N>") == FAILURE)
                return FAILURE;

            /* See if the user wants to calibrate the scanner at this time or not. */
            iQuitIndex = 0;
            while (!iQuitIndex)
            {
                if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
                    return FAILURE;
                switch (cButtonPressed)
                {
                    case CR:
                    case 'Y':
                        iQuitIndex = 1;
                        break;

                    case 'N':
                    case 'Z':
                    case ESC:
                        if (TTPrintsAt(4, 1, "Msg:                ") == FAILURE)
                            return FAILURE;
                        if (TTPrintMessage(CLICK, "               ") == FAILURE)
                            return FAILURE;
                        iQuitIndex = 2;
                        break;
                }
            }

            if (iQuitIndex == 1)
            {
                /* Make sure the robot is servoed on. This check isn't
                 * necessary to just store coordinates, only if the TSCN
                 * macro is invoked which will move the robot. */
                if (iVacMode == VACZ1 || iVacMode == VACZ2)
                {
                    if (iaAxisExists[iVacMode])
                    {
                        if (ROServoMCOn(ulaMechAxisDef[iVacMode]) == FAILURE)
                            return FAILURE;
                    }
                    if (iaAxisExists[iVacMode+1])
                    {
                        if (ROServoMCOn(ulaMechAxisDef[iVacMode+1]) == FAILURE)
                            return FAILURE;
                    }
                }
                else
                {
                    if (ROServoMCOn(ROAxisAllRobot()) == FAILURE)
                        return FAILURE;
                }
                if (iUseDBM)
                {
                    if (ROServoMCOn(ulaMechAxisDef[DBM]) == FAILURE)
                        return FAILURE;
                }
                sprintf( caCommandLine, "TSCN %c, 13", cTchStn );
                /*** EXECUTE TSCN MACRO ***/
                if( TMRunCommand( caCommandLine ) == FAILURE )
                    return FAILURE;
            }
            if (TTPrintMessage(SHORTBELL, "Scan Pos Stored") == FAILURE)
                return FAILURE;
        }
    }
    if (TIDelay(2000) == FAILURE)
        return FAILURE;
    if (TTPrintMessage(NO_BEEP, "               ") == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMMoveToCoord
 *
 * Abstract:    Moves all axes of the robot to a station.
 *      It checks that all axes are ready for motion.
 *      It asks the user if they want to retract first,
 *      for safety reasons, so the robot won't hit things
 *      when the track moves. Then it retracts (if requested),
 *      moves the track, moves the theta and Z axes, and finishes
 *      with an extend.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: move_to_coord
 * Used in: teach
 *
 ***************************************************************/
int TMMoveToCoord()
{
    long lPosition[8]={0,0,0,0,0,0,0,0};
    long laFprCoord[8]={0,0,0,0,0,0,0,0};
    long laSafeR[8]={0,0,0,0,0,0,0,0};
    long lPosHome;
    int iRetract;       /* If the user choses to retract before moving. */
    int iStn;           /* The station number that datafile needs, 0-51, not A-Z and a-z. */
    int iQuitLoop;      /* For user selection of "Retract (Y/N)". */
    char cButtonPressed;/* For user selection of "Retract (Y/N)". */

    /* All axes must be ready for motion. */
    /* Note they return Success if motion check fails, because motion check already
       displayed the error message. We don't want to return failure so TMTeach will not overwrite
       the error message displayed by motion-check */
    if (iaAxisExists[VACZ1] || iaAxisExists[VACT1] || iaAxisExists[VACZ2] || iaAxisExists[VACT2])
    {
        if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[VACZ1] | ulaMechAxisDef[VACT1] | ulaMechAxisDef[VACZ2] | ulaMechAxisDef[VACT2])) == FAILURE)
            return SUCCESS;
    }
    if (iaAxisExists[TRK])
    {
        if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[TRK])) == FAILURE)
            return SUCCESS;
    }
    if (iaAxisExists[FLP])
    {
        if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[FLP])) == FAILURE)
            return SUCCESS;
    }
    if (iUseDBM)
    {
        if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[DBM])) == FAILURE)
            return SUCCESS;
    }
    if (iaAxisExists[T])
    {
        if (TMTchMotionCheck((unsigned)(ROAxisAllRobot())) == FAILURE)
            return SUCCESS;
    }

    /* Convert the character station name to a number.
     * The conversion is:
     *      'A'-'Z'     0-25
     *      'a'-'z'     26-51   */
    iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
    if (iVacMode == VACZ1 || iVacMode == VACZ2)
    {
        if (FIOGetStnVals(iStn, TRACK_POS, &lPosHome) == FAILURE)
            return FAILURE;
        if (iVacMode == VACZ1 && lPosHome != 0)
            goto inx_mismatch_error_exit;
        if (iVacMode == VACZ2 && lPosHome != 1)
            goto inx_mismatch_error_exit;

        if (TTPrintMessage(NO_BEEP, "Move to...     ") == FAILURE)
            return FAILURE;

        /* Get the current vacuum indexer position and move to it. */
        if (iaAxisExists[iVacMode])
        {
            if (FIOGetStnlscsi(iStn, 2, &lPosition[iaMechAxisIdx[iVacMode]]) == FAILURE)
                return FAILURE;
            if (ROMoveToAbs(ulaMechAxisDef[iVacMode], lPosition) == FAILURE)
                return FAILURE;
        }
        if (iaAxisExists[iVacMode+1])
        {
            if (FIOGetStnlscsi(iStn, 3, &lPosition[iaMechAxisIdx[iVacMode+1]]) == FAILURE)
                return FAILURE;
            if (ROMoveToAbs(ulaMechAxisDef[iVacMode+1], lPosition) == FAILURE)
                return FAILURE;
        }
        if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[iVacMode])) == FAILURE)
            return FAILURE;
        if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[iVacMode+1])) == FAILURE)
            return FAILURE;
        goto exit_point;
    }

    if (TTPrintMessage(NO_BEEP, "Retract?<Y/N>  ") == FAILURE)
        return FAILURE;

    /* See if the user wants to retract before moving to the station. */
    iQuitLoop = FALSE;
    while (!iQuitLoop)
    {
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            case CR:
            case 'Y':
                iRetract = TRUE;
                iQuitLoop = TRUE;
                break;

            case 'N':
                iRetract = FALSE;
                iQuitLoop = TRUE;
                break;

            case 'Z':
            case ESC:
                if (TTPrintsAt(4, 1, "Msg:                ") == FAILURE)
                    return FAILURE;
                if (TTPrintMessage(CLICK, "               ") == FAILURE)
                    return FAILURE;
                return SUCCESS;
        }
    }

    if (TTPrintMessage(NO_BEEP, "Move to...     ") == FAILURE)
        return FAILURE;

    /* Use a Z index value of 1. */
    if (FIOSetStnVals(iStn, Z_INDEX, 1) == FAILURE)
        return FAILURE;

    /* Retract the Robot arm first (Safety is first) */
    if (iRetract)
    {
        /* Get station retract position.
         * Retract to the station R/W retract position. */
        if (FIOGetStnVals(iStn, RET_HOME, &lPosHome) == FAILURE)
            return FAILURE;
        /* RORetractR moves the correct axis noted by iUseDBM. */
        if (RORetractR(lPosHome, iUseDBM) == FAILURE)
            return FAILURE;
        if (iUseDBM)
        {
            if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
                return FAILURE;
        }
        else
        {
            if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
                return FAILURE;
        }
    }

    /* Now it is safe to move the track. */
    if (iaAxisExists[TRK])
    {
        /* Get the current track position and move to it. */
        if (FIOGetStnVals(iStn, TRACK_POS, &lPosition[iaMechAxisIdx[TRK]]) == FAILURE)
            return FAILURE;
        if (ROMoveToAbs(ulaMechAxisDef[TRK], lPosition) == FAILURE)
            return FAILURE;
    }

    /* Move to safe-R then move Flipper to Coordinate. */
    if (iaAxisExists[FLP])
    {
        /* Get Safe-R Position and move to position. */
        if (FIOGetStnVals(iStn, FLP_SAFE_R, &laSafeR[iaMechAxisIdx[R]]) == FAILURE)
            return FAILURE;
        if (ROMoveToAbs(RO_AXIS_R, laSafeR) == FAILURE)
            return FAILURE;
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            return FAILURE;

        /* Get the Flipper Coordinate and move to position. */
        if (FIOGetStnVals(iStn, FLP_COORD, &laFprCoord[iaMechAxisIdx[FLP]]) == FAILURE)
            return FAILURE;
        if (ROMoveToAbs(ROGetSpecialAxis(RO_FLIPPER), laFprCoord) == FAILURE)
            return FAILURE;
        if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[FLP])) == FAILURE)
            return FAILURE;
    }

    /* Move other Robot's axes in 2 sequences, theta and Z first, then extend. */
    if (iScanMode)
    {   /* Scan mode, move to scan coordinate,   V<-- This flag True */
        if (ROMoveRetract(iStn, 0, FALSE, TRUE, TRUE) == FAILURE)
            return FAILURE;
    }
    else
    {   /* Not in Scan mode, move to station coordinate */
	if (iTrkMode)
	{
	    if (ROMoveRetract(iStn, 0, FALSE, FALSE, FALSE) == FAILURE)
            	return FAILURE;
	}
	else
	{
	    if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[TRK])) == FAILURE)
        	return FAILURE;
	    if (ROMoveRetract(iStn, 0, FALSE, TRUE, FALSE) == FAILURE)
            	return FAILURE;
	}
	
    }

    if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
        return FAILURE;
    if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[TRK])) == FAILURE)
        return FAILURE;

    if (iUseDBM)
    {
        if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
            return FAILURE;
    }
exit_point:
    if (TTPrintMessage(CLICK, "               ") == FAILURE)
        return FAILURE;

    return SUCCESS;
inx_mismatch_error_exit:
    if (TTPrintMessage(ALERT, "Inx # Mismatch ") == FAILURE)
        return FAILURE;
    return FAILURE;
}


/****************************************************************
 *
 * Function:    TMDoGetPut
 *
 * Abstract:    Non-simulated wafer get or put. Actually calls the
 *      GET or PUT macro.
 *
 * Parameters:
 *      iPickArg    (in) Indicates whether to pick
 *                      or place the wafer (TRUE/FALSE)
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: do_get_put
 * Used in: teach
 *
 ***************************************************************/
int TMDoGetPut(int iPickArg, long lSlotnArg)
{
    char caCommand[20];

    if (iSmartMode)
    {
	if (cTchStn >= 'A' && cTchStn <= 'Z')
	{
	    sprintf(caCommand, "%s %c,%ld", iPickArg?"XGET":"XPUT", cTchStn, lSlotnArg);
	}
	else
	    return FAILURE;
    }
    else
    {
        sprintf(caCommand, "%s %c,%ld", iPickArg?"GET":"PUT", cTchStn, lSlotnArg);
    }
    if (TTPrintsAt(4, 1, "Msg:              ") == FAILURE)
        return FAILURE;
    if (TTPrintMessage(SHORTBELL, caCommand) == FAILURE)
        return FAILURE;
    if (TMRunCommand(caCommand) == FAILURE)
        return FAILURE;
//    if (TTPrintMessage(SHORTBELL, "Done") == FAILURE)
//        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMPickPlaceWafer
 *
 * Abstract:    Wafer pick or place. Simulated performs a Get or Put
 *      internally here using jog speeds. Non-simulated calls TMDoGetPut
 *      which calls the GET or PUT macros. It checks that all axes are
 *      ready for motion. Then it gets information from the user
 *      about stroke distance, offset distance, end-effector number,
 *      simulated or non-simulated mode, and slot number. Then it:
 *      1) Retracts, 2) Moves the track, 3) Moves the Flipper,
 *      4) Moves theta and Z, 5) Extends, 6) Handles the vacuum,
 *      7) Strokes the Z, and 7) Retracts.
 *
 * Parameters:
 *      iPickArg    (in) Indicates whether to pick
 *                      or place the wafer (TRUE/FALSE)
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: pick_place_wafer
 * Used in: teach
 *
 ***************************************************************/
int TMPickPlaceWafer(int iPickArg)
{
    int iSimulated;     /* If the user choses to simulate the pick/place or not. */
    int iInverted = FALSE;  /* If the user choses to Invert the pick/place or not. */
    int iStn;           /* The station number that datafile needs, 0-51, not A-Z and a-z. */
    int iVacSense;      /* During a pick, if there is no wafer, sense that. */
    int iQuitLoop;      /* For user selection of "Simulate (Y/N)". */
    char cButtonPressed;/* For user selection of "Simulate (Y/N)". */
    long lTempValue;    /* For end-effector number selection. */
    long lSlotn = 1;    /* For slot number selection if non-simulated mode is chosen. */
    long lRetPos;       /* The station R retract position. */
    long laTrackPos[8]={0,0,0,0,0,0,0,0};
    long laFprCoord[8]={0,0,0,0,0,0,0,0};
    long laSafeR[8]={0,0,0,0,0,0,0,0};
    int  iByte;

    /* All axes must be ready for motion. */
    if (iaAxisExists[TRK])
    {
        if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[TRK])) == FAILURE)
            return FAILURE;
    }
    if (iaAxisExists[FLP])
    {
        if (TMTchMotionCheck((unsigned)(ulaMechAxisDef[FLP])) == FAILURE)
            return FAILURE;
    }
    if (iUseDBM)
    {
        if (TMTchMotionCheck((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
            return FAILURE;
    }
    if (iaAxisExists[T])
    {
        if (TMTchMotionCheck((unsigned)(ROAxisAllRobot())) == FAILURE)
            return FAILURE;
    }

    /* Convert the character station name to a number.
     * The conversion is:
     *      'A'-'Z'     0-25
     *      'a'-'z'     26-51   */
    iStn = isupper(cTchStn) ? cTchStn-'A' : cTchStn-'a'+26;
    if (FIOSetStnVals(iStn, Z_INDEX, 1) == FAILURE)
        return FAILURE;

    /* Ask the user to enter the Stroke distance. */
    if (TTPrintsAt(4, 1, "Enter Stroke:      ") == FAILURE)
        return FAILURE;
    /* Get the current Stroke distance from the datafile. */
    if (FIOGetStnVals(iStn, STROKE, &lTempValue) == FAILURE)
        goto error_exit;
    /* Let the user adjust it. */
    if (TMTeachEdit( &lTempValue) == FAILURE)
        goto error_exit;
    /* Set the new value in the datafile. */
    if (FIOSetStnVals(iStn, STROKE, lTempValue) == FAILURE)
        goto error_exit;

    /* Ask the user to enter the Offset distance. */
    if (TTPrintsAt(4, 1, "Enter Offset:      ") == FAILURE)
        return FAILURE;
    /* Get the current Offset distance from the datafile. */
    if (FIOGetStnVals(iStn, OFFSET, &lTempValue) == FAILURE)
        goto error_exit;
    /* Let the user adjust it. */
    if (TMTeachEdit( &lTempValue) == FAILURE)
        goto error_exit;
    /* Set the new value in the datafile. */
    if (FIOSetStnVals(iStn, OFFSET, lTempValue) == FAILURE)
        goto error_exit;

    /* Asks the user if the pick or place is to be simulated or not. */
    if (TTPrintsAt(4, 1, "Simulated?<Y/N>     ") == FAILURE)
        return FAILURE;
    iQuitLoop = FALSE;
    while (!iQuitLoop)
    {
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            case CR:
            case 'Y':
                iSimulated = TRUE;
                iQuitLoop = TRUE;
                break;

            case 'N':
                iSimulated = FALSE;
		if(giSysCfgNum == 30) // 30 = I2AXO special 
		    iSimulated = TRUE; // due to SECS processing, force to Simulation mode
                iQuitLoop = TRUE;
                break;

            case 'Z':
            case ESC:
                TTPrintsAt(4, 1, "Msg:                ");
                TTPrintMessage(CLICK, "               ");
                return FAILURE;
        }
    }

    // Force non-Simulated if smart-motion
    if(iSmartMode)
	iSimulated = FALSE;

    /* In non-simulated mode, ask for a slot number, then execute the GET or PUT macro. */
    if (!iSimulated)
    {
        /* Ask the user to enter the Slot number. */
        if (TTPrintsAt(4, 1, "Enter Slot:      ") == FAILURE)
            goto error_exit;
        /* Let the user adjust it. */
        if ( TMTeachEdit(&lSlotn) == FAILURE)
            goto error_exit;

        // Execute the GET or PUT macro. Or XGET or XPUT if smartMotion mode.
        if (TMDoGetPut(iPickArg, lSlotn) == FAILURE)
            return FAILURE;

        return SUCCESS;
    }

    /* Simulated - If flipper: (1) Set default Inverted/not-Inverted based Flipper Coordinate
     *                         (2) ask if Inverted GET/PUT? */
    if (iaAxisExists[FLP])
    {
        /* (1) Get the Flipper coordinate, (2) set Inverted motion flag to default False, (3) check if the Flipper
         * coordinate is set to other than 0, (4) If the Flipper coordinate is other than 0, set the Inverted motion
         * flag to default True. */
        if (FIOGetStnVals(iStn, FLP_COORD, &laFprCoord[iaMechAxisIdx[FLP]]) == FAILURE)
            goto error_exit;
        iInverted = FALSE;
        if (laFprCoord[iaMechAxisIdx[FLP]] != 0)
            iInverted = TRUE;

        /* Prompt the user to change the Inverted motion flag. */
        if (TTPrintsAt(4, 1, "Inverted?<Y/N>     ") == FAILURE)
            return FAILURE;

        iQuitLoop = FALSE;
        while (!iQuitLoop)
        {
            if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
                return FAILURE;
            switch (cButtonPressed)
            {
                case CR:
                    iQuitLoop = TRUE;
                    break;

                case 'Y':
                    iInverted = TRUE;
                    iQuitLoop = TRUE;
                    break;

                case 'N':
                    iInverted = FALSE;
                    iQuitLoop = TRUE;
                    break;

                case 'Z':
                case ESC:
                    if (TTPrintsAt(4, 1, "Msg:                ") == FAILURE)
                        return FAILURE;
                    if (TTPrintMessage(CLICK, "                    ") == FAILURE)
                        return FAILURE;
                    return FAILURE;
            }
        }
    }

    if (TTPrintsAt(4, 1, "Msg:") == FAILURE)
        goto error_exit;
    if (TTPrintMessage(NO_BEEP, "Retracting...") == FAILURE)
        goto error_exit;

    /* Get station home position */
    if (FIOGetStnVals(iStn, RET_HOME, &lRetPos) == FAILURE)
        goto error_exit;
    /* Retract to station retract position */
    /* RORetractR uses iUseDBM flag to move the right axis */
    if (RORetractR(lRetPos, iUseDBM) == FAILURE)
        goto error_exit;

    if (iUseDBM)
    {
        if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
            goto error_exit;
    }
    else
    {
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            goto error_exit;
    }

    if (TTPrintMessage(NO_BEEP, (iPickArg ? "Picking        " : "Placing        ") ) == FAILURE)
        goto error_exit;

    /* Now it is safe to move the track */
    if (iaAxisExists[TRK])
    {
        /* Get the current track position and move to it. */
        if (FIOGetStnVals(iStn, TRACK_POS, &laTrackPos[iaMechAxisIdx[TRK]]) == FAILURE)
            goto error_exit;
        if (ROMoveToAbs(ulaMechAxisDef[TRK], laTrackPos) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[TRK])) == FAILURE)
            goto error_exit;
    }

    /* Move to safe-R then move Flipper to Coordinate. */
    if (iaAxisExists[FLP])
    {
        /* Get Safe-R Position and move to position. */
        if (FIOGetStnVals(iStn, FLP_SAFE_R, &laSafeR[iaMechAxisIdx[R]]) == FAILURE)
            goto error_exit;
        if (ROMoveToAbs(RO_AXIS_R, laSafeR) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            goto error_exit;

        /* Get the Flipper Coordinate and move to position. */
        if (FIOGetStnVals(iStn, FLP_COORD, &laFprCoord[iaMechAxisIdx[FLP]]) == FAILURE)
            goto error_exit;
        if (ROMoveToAbs(ulaMechAxisDef[FLP], laFprCoord) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ulaMechAxisDef[FLP])) == FAILURE)
            goto error_exit;
    }

    /* Extend. */
    /* If iOffDirArg = 1, then Robot comes in above the wafer. */
    /* If iOffDirArg = 2, then Robot comes in under the wafer. */
    if (iInverted)
    {
        if (ROMoveRetract(iStn, 1, FALSE, TRUE, FALSE) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            goto error_exit;
        if (iUseDBM)
        {
            if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
                goto error_exit;
        }
    }
    else
    {
        if (ROMoveRetract(iStn, (iPickArg ? 2 : 1), FALSE, TRUE, FALSE) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            goto error_exit;
        if (iUseDBM)
        {
            if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
                goto error_exit;
        }
    }

    /* If not Inverted, Turn on/off vacuum. Vacuum is always on Galil card number 0.
     * One more condition to be met: not Vac Prealigner or VAC514 */
    iInputBit = iOutputBit = iUseDBM;
    if (!iInverted && !(iTMDefineFlag&DFVACPRE) && !(iTMDefineFlag&DFVAC514))
    {
	iByte = inb (IO_ROBOT_OUTPUT_A);
	if(iPickArg) // turn on vac, OUTP 0,0
	{
	    if(iInputBit)
		iByte &= 0xFC;
	    else
		iByte &= 0xFE;
	}
	else	// placing, turn off vac, OUTP 0,1
	{
	    if(iInputBit)
		iByte |= 0x02;
	    else
		iByte |= 0x01;
	}

	outb (iByte, IO_ROBOT_OUTPUT_A);

//        if (ROWriteGalilPort(0, iOutputBit, !iPickArg) == FAILURE)
//            goto error_exit;
        if (TIDelay(150) == FAILURE)
            goto error_exit;
    }

    /* Move up/down. */
    if (iInverted)
    {
        if (ROMoveStroke(iStn, -1) == FAILURE)
            goto error_exit;
    }
    else
    {
        if (ROMoveStroke(iStn, (iPickArg ? +1 : -1)) == FAILURE)
            goto error_exit;
    }
    if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
        goto error_exit;

    /* If Inverted, Turn on/off vacuum. Vacuum is always on Galil card number 0.
     * One more condition to be met: not Vac Prealigner or VAC514 */
    if (iInverted && !(iTMDefineFlag&DFVACPRE) && !(iTMDefineFlag&DFVAC514))
    {
        if (ROWriteGalilPort(0, iOutputBit, !iPickArg) == FAILURE)
            goto error_exit;
        if (TIDelay(150) == FAILURE)
            goto error_exit;
    }

    /* Only non-vac prealigner, no VAC514 */
    if (!(iTMDefineFlag&DFVACPRE) && !(iTMDefineFlag&DFVAC514))
    {
        /* Sense for vacuum on/off. */
	iVacSense = inb (IO_ROBOT_INPUT_F) & 0x03;

        /* If we're not Inverted and picking and there is no wafer, sense that and move back down. */
        if (!iInverted && iPickArg && (iVacSense & (1<<iInputBit)))
        {
            /* Turn off vacuum. */
	    iByte = inb (IO_ROBOT_OUTPUT_A) | 0x03;
	    outb (iByte, IO_ROBOT_OUTPUT_A);
            if (TIDelay(150) == FAILURE)
                goto error_exit;
            /* Move back down. */
            if (ROMoveStroke(iStn, -1) == FAILURE)
            	goto error_exit;
            if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            	goto error_exit;
	}

        /* Retract in lowered position. */
    	if (RORetractR(lRetPos, iUseDBM) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            goto error_exit;
        if (iUseDBM)
        {
            if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
                goto error_exit;
        }

        if (TTPrintMessage(CLICK, "               ") == FAILURE)
            goto error_exit;

        return SUCCESS;
    }

    /* Retract. */
    if (iInverted)
    {
        /* Move Z-axis up */
        if (ROMoveStroke(iStn, +1) == FAILURE)
            goto error_exit;
        if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
            goto error_exit;
        /* Retract. */
        /* If iOffDirArg = 1, then Robot comes in above the wafer. */
        /* If iOffDirArg = 2, then Robot comes in under the wafer. */
        if (ROMoveRetract(iStn, 1, TRUE, FALSE, FALSE) == FAILURE)
            goto error_exit;
    }
    else if (!iInverted)
    {
        if (ROMoveRetract(iStn, (iPickArg ? 1 : 2), TRUE, FALSE, FALSE) == FAILURE)
            goto error_exit;
    }
    if (TMWaitTchMotionNoError((unsigned)(ROAxisAllRobot())) == FAILURE)
        goto error_exit;
    if (iUseDBM)
    {
        if (TMWaitTchMotionNoError((unsigned)ulaMechAxisDef[DBM]) == FAILURE)
            goto error_exit;
    }

    if (TTPrintMessage(CLICK, "               ") == FAILURE)
        goto error_exit;

    return SUCCESS;

error_exit:

    TTPrintsAt(4, 1, "Msg: Failed         ");
    return FAILURE;
}


/****************************************************************
 *
 * Function:    TMToggleServo
 *
 * Abstract:    Toggle servo ON/OFF for specified axis.
 *
 * Parameters:
 *      iAxis   (in) The axis whose servo to toggle ON or OFF
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: toggle_servo
 * Used in: teach
 *
 ***************************************************************/
int TMToggleServo(int iAxis)
{
    int iRet;
    char caOutMsg[21];

    /* Validate parameters passed in. */
    if (!iaAxisExists[iAxis])
        return FAILURE;
    strcpy(caOutMsg, cpAxisName[iAxis]);

    /* Toggle the axis servo. */
    if (ROGetServoFlag() & ulaMechAxisDef[iAxis])
    {
        iRet = ROServoMCOn(ulaMechAxisDef[iAxis]);

        //For VAC514 robots Servo buttons for T and R work as a one button
        if((iTMDefineFlag & DFVAC514) && (iAxis == T) && (iRet != FAILURE))
        {
            iRet = ROServoMCOn(ulaMechAxisDef[R]);
            strcat(caOutMsg, ",R");
        }

        if((iTMDefineFlag & DFVAC514) && (iAxis == R) && (iRet != FAILURE))
        {
            iRet = ROServoMCOn(ulaMechAxisDef[T]);
            strcat(caOutMsg, ",T");
        }

        strcat(caOutMsg, " ON ");
    }
    else
    {
        iRet = ROServoMCOff(ulaMechAxisDef[iAxis]);

        //For VAC514 robots Servo buttons for T and R work as a one button
        if((iTMDefineFlag & DFVAC514) && (iAxis == R) && (iRet != FAILURE))
        {
            iRet = ROServoMCOff(ulaMechAxisDef[T]);
            strcat(caOutMsg, ",T");
        }

        if((iTMDefineFlag & DFVAC514) && (iAxis == T) && (iRet != FAILURE))
        {
            iRet = ROServoMCOff(ulaMechAxisDef[R]);
            strcat(caOutMsg, ",R");
        }

        strcat(caOutMsg, " OFF ");
    }

    /* Check if it completed successfully or not and tell the user. */
    if (iRet == FAILURE)
    {
        strcat(caOutMsg, "FAILS");
        if (TTPrintMessage(ALERT, caOutMsg) == FAILURE)
            return FAILURE;
    }
    else
    {
        strcat(caOutMsg, "SUCCEEDS");
        if (TTPrintMessage(CLICK, caOutMsg) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMWaitTchMotionComplete
 *
 * Abstract:    Wait for a motion to complete. Just loop
 *      and wait for the after-motion flag to be changed by
 *      the ME_PROC. Update the screen while you wait. Also check if
 *      the user wants to abort the move.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: wait_tch_RBT_motion_complete, wait_tch_TRK_motion_complete
 * Used in: teach
 *
 ***************************************************************/
int TMWaitTchMotionComplete(unsigned uAxis)
{
    char cButtonPressed;
    int i, iReturn;
    char caMTCRcommand[10] = "MG VMTC";
    char caInMotionResp[50];


    //Chek for motion complete on both, T and R axes for the VAC514 robots
    if((iTMDefineFlag & DFVAC514) && ((uAxis & RO_AXIS_R) || (uAxis & RO_AXIS_T)))
    {
        uAxis |= (RO_AXIS_R | RO_AXIS_T);
    }

    i = 1;
    while (i) // loop if MTCR running
    {
        /* ...and update the teach pendant screen while you wait... */
        if (TMDisplayAxes() == FAILURE)
            return FAILURE;
        if (TMDisplayOthers() == FAILURE)
            return FAILURE;
        /* ...while checking for keypresses... */
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            /* ... and stop the motion if the user pressed ESC. */
            case 'Z':
            case ESC:
                ROStopMotion(uAxis);
                TTPrintMessage(CLICK, "               ");
                return FAILURE;
        }
	iReturn = GASendDMCCommand(ghDMC, caMTCRcommand, caInMotionResp, 50);
	if(iReturn)
	{
	    TIDelay(100);
	}
	else
	{
	    i = atoi(caInMotionResp);
	}
    }

    /* Check if the axis is still in motion... */
    while (~ROGetAMFlag() & (unsigned long)(uAxis))
    {

	ROUpdateTS(FALSE);
        /* ...and update the teach pendant screen while you wait... */
        if (TMDisplayAxes() == FAILURE)
            return FAILURE;
        if (TMDisplayOthers() == FAILURE)
            return FAILURE;
        /* ...while checking for keypresses... */
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            /* ... and stop the motion if the user pressed ESC. */
            case 'Z':
            case ESC:
                ROStopMotion(uAxis);
                TTPrintMessage(CLICK, "               ");
                return FAILURE;
        }
    }
    if (ROCheckLimitSwitches() == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMWaitTchMove
 *
 * Abstract:    Wait for a teach mode motion to complete.
 *      This function is used when we don't know which axis
 *      we're waiting for motion to complete on, specifically in
 *      jog and step mode. We do know that in step and jog mode
 *      we can't move the track and the robot at the same time.
 *      So we wait for the axis based on mode.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: wait_tch_move
 * Used in: teach
 *
 ***************************************************************/
int TMWaitTchMove()
{
    if (iVacMode == VACZ1)
    {
        if (TMWaitTchMotionComplete((unsigned)(ulaMechAxisDef[VACZ1] | ulaMechAxisDef[VACT1])) == FAILURE) return FAILURE;
    }
    else if (iVacMode == VACZ2)
    {
        if (TMWaitTchMotionComplete((unsigned)(ulaMechAxisDef[VACZ2] | ulaMechAxisDef[VACT2])) == FAILURE) return FAILURE;
    }
    else if (iTrkMode)
    {
        if (TMWaitTchMotionComplete((unsigned)(ulaMechAxisDef[TRK])) == FAILURE) return FAILURE;
    }
    else if (iFprMode)
    {
        if (TMWaitTchMotionComplete((unsigned)(ulaMechAxisDef[FLP])) == FAILURE) return FAILURE;
    }
    else
    {
        if (TMWaitTchMotionComplete((unsigned)(ROAxisAllRobot())) == FAILURE) return FAILURE;
        if (iUseDBM)
        {
            if (TMWaitTchMotionComplete((unsigned)ulaMechAxisDef[DBM]) == FAILURE) return FAILURE;
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMTchMotionCheck
 *
 * Abstract:    Check if ready for motion and output message to teach pendant
 *      if not, i.e. makes sure that the robot is servoed on, homed, and not moving.
 *      Otherwise we can't command a teach motion. Note that the iAxis
 *      passed in must be in terms of mechanism module defines.
 *
 * Parameters:
 *      iAxis   (in) The axis to check
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: tch_motion_check
 * Used in: teach
 *
 ***************************************************************/
int TMTchMotionCheck(unsigned uAxis)
{
    int i, iReturn;
    char caHXcommand[10] = "HX";
    char caMTCRcommand[10] = "MG VMTC";
    char caInMotionResp[50];

    /* Checks for FAILURE are not necessary. Error states
     * return FAILURE by default. */
    //On VAC514 robots check for both, R and T axes
    ROUpdateTS(FALSE);
    if((iTMDefineFlag & DFVAC514) && ((uAxis & RO_AXIS_R) || (uAxis & RO_AXIS_T)))
    {
        uAxis |= (RO_AXIS_R | RO_AXIS_T);
    }

    if (ROGetServoFlag() & (unsigned long)(uAxis))
    {
        TTPrintMessage(ALERT, " Servo OFF     ");
        return FAILURE;
    }

    iReturn = GASendDMCCommand(ghDMC, caHXcommand, caInMotionResp, 50);
    i = atoi(caInMotionResp);
    if (i)
    {
        TTPrintMessage(ALERT, " Axis moving   ");
        return FAILURE;
    }

    if (~ROGetAMFlag() & (unsigned long)(uAxis))
    {
        TTPrintMessage(ALERT, " Axis moving   ");
        return FAILURE;
    }

    if (~ROGetHomed() & (unsigned long)(uAxis))
    {
        TTPrintMessage(ALERT, " Not homed     ");
        return FAILURE;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMGetTeachFlag
 *
 * Abstract:    Gets the value of the teach flag (was go_to_teach
 *      variable). See header notes for more details.
 *
 * Parameters:  None
 *
 * Returns:     The value of the teach flag
 *
 * Replaces: go_to_teach variable
 * Used in: main, execute
 *
 ***************************************************************/
int TMGetTeachFlag()
{
    return iTeachFlag;
}


/****************************************************************
 *
 * Function:    TMSetTeachFlag
 *
 * Abstract:    Sets the value of the teach flag (was go_to_teach
 *      variable). See header notes for more details.
 *
 * Parameters:
 *      iPortNumArg     (in) The new value for the flag
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: go_to_teach variable
 * Used in: main, execute
 *
 ***************************************************************/
int TMSetTeachFlag(int iPortNumArg)
{
    if ((iPortNumArg != COM1) && (iPortNumArg != COM2) &&
        (iPortNumArg != COM3) && (iPortNumArg != COM4) &&
	(iPortNumArg != NO_PORT_NUM))
    {
        return FAILURE;
    }

    iTeachFlag = iPortNumArg;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMRunCommand
 *
 * Abstract:    Run Command/Macro from Teach Mode. This is like
 *      a mini main loop. It runs the TSCN and GET and PUT
 *      macros.
 *
 * Parameters:
 *          char    *cpCommand
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: run_command
 * Used in: store_coord, do_ALIGN, do_get_put
 *
 ******************************************************************/
int TMRunCommand(char *cpCommand)
{
    instr_t cmd_line_instr;
    instr_ptr instr, TempPC;
    pstTaskList pCurrentTaskListTemp;
    char cKeyPressed;
    int iReturn;
    long laParms[8] = {0,0,0,0,0,0,0,0};
    long laIPParms[MAX_TCH_AXES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    /* Initialize. */
    instr = (instr_ptr)&cmd_line_instr;
    TKSetPCCriticalSection( 1 );
    if ((iReturn=MASetFlagsToFalse()) == FAILURE)
        goto error_exit;

    /* Decode the command. */
    iReturn = CMDcmndLineAction(cpCommand, instr, FALSE);

    if (iReturn == SUCCESS || iReturn == MACRO)
    {
        if (TMRestoreSpeedAndAccel() == FAILURE)
            return FAILURE;

        /* Restore the old Galil ID settings */
        if (TMSetIP(laSaveIP) == FAILURE)
            return FAILURE;

        /* Start the macro running. */
        iReturn = MRExecuteInstructions(instr, FALSE);
        if (iReturn == FAILURE)
        {
            TTPrintMessage(NO_BEEP, "Can't execute  ");
            goto error_exit;
        }
        else
        {
            /* Tell the user exactly what macro statement is being run. */
            if ((iReturn=TTPrintMessage(NO_BEEP, cpCommand)) == FAILURE)
                goto error_exit;
            /* Free the operands. */
            CMDfreeOperands(instr->opr_ptr);
            pCurrentTaskListTemp = NULL;

            /* Keep executing the macro instr until finished. */
            TempPC = MRGetPC();
            while ( TempPC ||  pCurrentTaskListTemp )
            {
                /* Process the next macro instruction if ready. */
                PCProcessMacroInstr();
                if ( TempPC == NULL)
                {
                    if ( pCurrentTaskListTemp = TKGetNextWaitingTask() )
                        TKSetPCCriticalSection( -1 );
                }
                /* Check for user key presses. */
                if ((iReturn=SERGetcRxBuff(iTMPortNum, &cKeyPressed, FALSE)) == FAILURE)
                    goto error_exit;
                /* If the user pressed ESC... */
                if ( cKeyPressed == 'Z' ||  cKeyPressed == ESC )
                {
                    /* ...stop all axes... */
                    ROStopMotion(0);
                    /* ...and abort any macro in progress. */
                    MRAbortMacro();
                    if( iTMDefineFlag&DFPRE )
                        ALAbort();              /* abort alignment process */
                    break;
                }
                TempPC = MRGetPC();
            }
        }

        if (TMSetJogSpeedAndAccel() == FAILURE)
            return FAILURE;

        /* Set the Galil ID information to 0. */
        laParms[T] = laParms[R] = laParms[Z] = laParms[TRK] = 0;
        if (TMSetIP(laIPParms) == FAILURE) return FAILURE;
    }
    else
    {
        /* Invalid command. */
        if ((iReturn=TTPrintMessage(NO_BEEP, "Invalid Command")) == FAILURE) goto error_exit;
        if ((iReturn=SERFlushComBuffer(iTMPortNum)) == FAILURE) goto error_exit;
     }

    /* Setup the teach mode screen. */
    if ((iReturn=TMDrawTeachScr()) == FAILURE) goto error_exit;
    if ((iReturn=TTPrintMessage(NO_BEEP, "Ready          ")) == FAILURE) goto error_exit;

error_exit:
     TKSetPCCriticalSection( -1 );
     return iReturn;
}


/****************************************************************
 *
 * Function:    TMTeachEdit
 *
 * Abstract:    Edit the value from teach pendant. Used mainly
 *      for pick and place value editing. The number passed in is
 *      printed to the screen. User keypresses are handled.
 *      Backspaces simply move the cursor index/end-of-string
 *      back one space and reprint the number to the screen.
 *      +/- and numbers are added to the string, the index
 *      is incremented, and the string is reprinted. An escape
 *      exits without changing the passed in number. An enter
 *      converts the new string to a number and passes it back to
 *      the calling function. The end result of all this is that
 *      the teach pendant acts sort of like a Windows dialog box.
 *
 * Parameters:
 *          long    *lValue (in/out)   Value to edit
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: tch_edit
 * Used in: do_get_put
 *
 ******************************************************************/
int TMTeachEdit(long *lValue)
{
    char cKeyPressed;
    int iNumCharsLeft;
    int iUpdate = FALSE;    /* Update the teach pendant if necessary. */
    char caBuf[5];          /* Temporary buffer for the number to be entered,
                             * modified, and printed to the screen. */
    int i = 0;              /* The character in the temporary buffer
                             * where the cursor is currently. */

    /* Initialize. */
    memset(caBuf, 0, sizeof(char)*5);
    sprintf(caBuf, "%ld", *lValue);
    i = strlen(caBuf);

    /* Print out to user */
    if (TTPrintsAt(4, 15, caBuf) == FAILURE)
        return FAILURE;
    if (TTSetCursorPos(4, 15+i) == FAILURE)
        return FAILURE;
    if (TTSetCursorType(ENABLE_CURSOR) == FAILURE)
        return FAILURE;
    if (TTSetCursorType(BLINK) == FAILURE)
        return FAILURE;

    for (;;)
    {
        /* Get a character directly from the comm port. Bypass the internal
         * buffer; go directly to the ASYNCH manager. Otherwise, backspaces
         * are filtered out. */
        if (SERGetRawChar(iTMPortNum, &cKeyPressed, &iNumCharsLeft) == FAILURE)
            return FAILURE;
        switch(cKeyPressed)
        {
            case 'Z':
            case ESC:                       /* Don't change the value. */
                if (TTSetCursorType(NO_CURSOR) == FAILURE)
                    return FAILURE;
                if (TTSetCursorType(NO_BLINK) == FAILURE)
                    return FAILURE;
                return FAILURE;

            case CR :                       /* New value accepted. */
                *lValue = atol(caBuf);      /* Change the pass-back variable. */
                if (TTSetCursorType(NO_CURSOR) == FAILURE)
                    return FAILURE;
                if (TTSetCursorType(NO_BLINK) == FAILURE)
                    return FAILURE;
                return SUCCESS;

            case '\b':                      /* Handle backspace. */
                iUpdate = TRUE;             /* Update teach pendant. */
                if ( i > 0 )
                {
                    /* Change the string index if possible (not at
                     * the beginning of the line). */
                    i--;
                }
                caBuf[i] = 0;
                break;

            default:                        /* Save the digits, +'s, and -'s. */
                if ( (isdigit (cKeyPressed)) || (i==0 && ((cKeyPressed=='+') || (cKeyPressed=='-'))) )
                {
                    iUpdate = TRUE;         /* Update teach pendant. */
                    if ( i < 4 )
                    {
                        /* Record the key pressed and change the string index
                         * if possible (not at the end of the line). */
                        caBuf[i] = cKeyPressed;
                        i++;
                    }
                }
                break;
        }

        /* Print out to user if anything significant has changed. */
        if ( iUpdate)
        {
            if (TTPrintsAt(4, 15, "     ") == FAILURE)
                return FAILURE;
            if (TTPrintsAt(4, 15, caBuf) == FAILURE)
                return FAILURE;
            if (TTSetCursorPos(4, 15 + i) == FAILURE)
                return FAILURE;
            iUpdate = FALSE;
        }
    }
}


/****************************************************************
 *
 * Function:    TMRestoreSpeedAndAccel
 *
 * Abstract:    Restores the original speeds, accelerations,
 *      and deceleration that were saved upon teach mode entry.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ******************************************************************/
int TMRestoreSpeedAndAccel()
{
    long laParms[8];        /* Temporary array for general use. */
    int iAxis;

    ROUpdateTS(FALSE);

    /* Restore the original operating speeds... */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            laParms[iaMechAxisIdx[iAxis]] = laSaveSpeed[iAxis];
            if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_SPEED) == FAILURE)
                return FAILURE;
        }
    }

    /* ...and accelerations... */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            laParms[iaMechAxisIdx[iAxis]] = laSaveAccel[iAxis];
            if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_ACCEL) == FAILURE)
                return FAILURE;
        }
    }

    /* ...and decelerations for all axes. */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            laParms[iaMechAxisIdx[iAxis]] = laSaveDecel[iAxis];
            if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_DECEL) == FAILURE)
                return FAILURE;
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    TMSetJogSpeedAndAccel
 *
 * Abstract:    Sets the speeds, accelerations, and decelerations
 *      that are selected by the user for jogging the axes.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ******************************************************************/
int TMSetJogSpeedAndAccel()
{
    long laParms[8];        /* Temporary array for general use. */
    int iAxis;

    ROUpdateTS(FALSE);

    /* Get and set the JOG speeds... */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            laParms[iaMechAxisIdx[iAxis]] = (long)(iaTeachSpeed[iAxis][iaSpeedIndex[iAxis]]);
            if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
                laParms[iaMechAxisIdx[iAxis]] *= lUnitConvAngle;
            else
                laParms[iaMechAxisIdx[iAxis]] *= lUnitConvDist;
            if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_SPEED) == FAILURE)
                return FAILURE;
        }
    }

    /* ...and accelerations... */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            laParms[iaMechAxisIdx[iAxis]] = (long)(iaTeachAccel[iAxis][iaSpeedIndex[iAxis]]);
            if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
                laParms[iaMechAxisIdx[iAxis]] *= lUnitConvAngle;
            else
                laParms[iaMechAxisIdx[iAxis]] *= lUnitConvDist;
            if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_ACCEL) == FAILURE)
                return FAILURE;
        }
    }

    /* ...and deceleration for all axes. */
    for (iAxis=0; iAxis<MAX_TCH_AXES; iAxis++)
    {
        if (iaAxisExists[iAxis])
        {
            laParms[iaMechAxisIdx[iAxis]] = (long)(iaTeachAccel[iAxis][iaSpeedIndex[iAxis]]);
            if ((iAxis == T) || (iAxis == VACT1) || (iAxis == VACT2))
                laParms[iaMechAxisIdx[iAxis]] *= lUnitConvAngle;
            else
                laParms[iaMechAxisIdx[iAxis]] *= lUnitConvDist;
            if (ROSetParameter(FALSE, ulaMechAxisDef[iAxis], laParms, OPERATIONAL_DECEL) == FAILURE)
                return FAILURE;
        }
    }

    return SUCCESS;
}
/****************************************************************
 *
 * Function:    TMWaitTchMotionNoError
 *
 * Abstract:    Wait for a motion to complete without error on other axis. Just loop
 *      and wait for the after-motion flag to be changed by
 *      the ME_PROC. Update the screen while you wait. Also check if
 *      the user wants to abort the move.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces: wait_tch_RBT_motion_complete, wait_tch_TRK_motion_complete
 * Used in: teach
 *
 ***************************************************************/
int TMWaitTchMotionNoError(unsigned uAxis)
{
    char cButtonPressed;
    int i, iReturn;
    char caMTCRcommand[10] = "MG VMTC";
    char caInMotionResp[50];

    //Chek for motion complete on both, T and R axes for the VAC514 robots
    if((iTMDefineFlag & DFVAC514) && ((uAxis & RO_AXIS_R) || (uAxis & RO_AXIS_T)))
    {
        uAxis |= (RO_AXIS_R | RO_AXIS_T);
    }

    i = 1;
    while (i) // loop if MTCR running
    {
        /* ...and update the teach pendant screen while you wait... */
        if (TMDisplayAxes() == FAILURE)
            return FAILURE;
        if (TMDisplayOthers() == FAILURE)
            return FAILURE;
        /* ...while checking for keypresses... */
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            /* ... and stop the motion if the user pressed ESC. */
            case 'Z':
            case ESC:
                ROStopMotion(uAxis);
                TTPrintMessage(CLICK, "               ");
                return FAILURE;
        }

	iReturn = GASendDMCCommand(ghDMC, caMTCRcommand, caInMotionResp, 50);
	if(iReturn)
	{
	    TIDelay(100);
	}
	else
	{
	    i = atoi(caInMotionResp);
	}
    }

    /* Check if the axis is still in motion... */
    while (~ROGetAMFlag() & (unsigned long)(uAxis))
    {
        /* check for other axes error */
	ROUpdateTS(FALSE);
        if (ROGetServoFlag() & (unsigned)(ROAxisAllRobot()))
        {
            ROStopMotion(uAxis);
            TTPrintMessage(CLICK, "               ");
            return FAILURE;
        }
        if (iaAxisExists[TRK])
        {
            if (ROGetServoFlag() & (unsigned)(ulaMechAxisDef[TRK]))
            {
                ROStopMotion(uAxis);
                TTPrintMessage(CLICK, "               ");
                return FAILURE;
            }
        }
        /* ...and update the teach pendant screen while you wait... */
        if (TMDisplayAxes() == FAILURE)
            return FAILURE;
        if (TMDisplayOthers() == FAILURE)
            return FAILURE;
        /* ...while checking for keypresses... */
        if (SERGetcRxBuff(iTMPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            /* ... and stop the motion if the user pressed ESC. */
            case 'Z':
            case ESC:
                ROStopMotion(uAxis);
                TTPrintMessage(CLICK, "               ");
                return FAILURE;
        }
    }

    if (ROGetERFlag() & (unsigned long)uAxis)
        return FAILURE;

    if (ROCheckLimitSwitches() == FAILURE)
        return FAILURE;

    return SUCCESS;
}

int TMMoveCartesian(char cCharToProcessArg)
{
    long lDir;
    long lDistance;
    unsigned uAxis = 0x03; // T & R

    if (!iSmartMode)
	return;

    lDir = cCharToProcessArg - '0';

    lDistance = glVectorLength;

    if (ROMoveCartesianDirection(lDir, lDistance)==FAILURE)
	return FAILURE;

    if(TMWaitTchMotionNoError(uAxis)==FAILURE)
	return FAILURE;  

    return SUCCESS;
}

