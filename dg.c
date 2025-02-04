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
 * File:        dg.c
 * Functions:   DiagStart
 *              DiagGetFlag
 *              DiagSetFlag
 *              DiagMenu
 *              DiagGetAnswer
 *              DiagStep1
 *              DiagStep2
 *              DiagStep3
 *              DiagStep4
 *              DiagStep5
 *              DiagStep6
 *              DiagStep7
 *              DiagStep8
 *              DiagStep9
 *              DiagStep10
 *              DiagStep11
 *              DiagStep12
 *              DiagStep13
 *              DiagStep14
 *
 * Description: Handles the interface to diagnostics mode
 *
 * Modification history:
 *
 ****************************************************************/

#include <stdlib.h>
#include <string.h>
#include "sck.h"
#include "ser.h"
#include "sctim.h"
#include "scttr.h"
#include "dgl.h"
#include "dg.h"
#include "rodg.h"
#include "ro.h"
#include "gag.h"
#include "scstat.h"
#include "fiog.h"
#include "scmac.h"
#include "mapio.h"
#include "scio.h"
#include "scproc.h"
#include "gatst.h"
#include "alisr.h"

/****************************************************************
 *
 * Function:    DiagStart
 *
 * Abstract:    Starts diagnostics mode. This function is the primary
 *      interface to the diagnostics module. It handles setting up the
 *      teach pendant to be used in diagnostics mode, the user keypresses
 *      to continue into either robot or prealigner diagnostics mode,
 *      and returns the teach pendant and robot to their normal
 *      working conditions when finished.
 *
 * Parameters:
 *      iPortNumArg     (in) The port number to run diagnostics to
 *      iRobArg         (in) Whether or not a robot is present (TRUE/FALSE)
 *      iPreArg         (in) Whether or not a prealigner is present (TRUE/FALSE)
 *      iVacArg         (in) Whether or not a vacuum indexer is present (TRUE/FALSE)
 *      iDefineFlagArg  (in) The system define flags
 *      iEmulatorArg    (in) The system emulation
 *
 * Returns: SUCCESS or FAILURE
 *
 * Replaces: diagnostics
 * Used in: main
 *
 ***************************************************************/
int DiagStart(int iPortNumArg, int iRobArg, int iPreArg, int iVacArg, int iDefineFlagArg, int iEmulatorArg)
{
    int iValidKey;          /* Indicates when a valid key press was received */
    int iQuitKey;           /* Indicates whether to continue or quit diagnostics */
    int iSVOffSuccess;      /* Indicates whether to quit diagnostics because of a servo off failure */
    char caRobPreMsg[19];   /* The message to press R-Robot, P-Prealigner, V-Vacuum Indexer */

    iDiagDefineFlag = iDefineFlagArg;
    iDiagEmulator = iEmulatorArg;
    /* This variable is used internally to indicate that a ROBOT is present in
     * the system. */
    if ((iRobArg != TRUE) && (iRobArg != FALSE))
        return FAILURE;
    iRobExists = iRobArg;
    /* This variable is used internally to indicate that a PREALIGNER is present in
     * the system. */
    if ((iPreArg != TRUE) && (iPreArg != FALSE))
        return FAILURE;
    iPreExists = iPreArg;
    /* This variable is used internally to indicate that a VACUUM INDEXER is present in
     * the system. */
    if ((iVacArg != TRUE) && (iVacArg != FALSE))
        return FAILURE;
    iVacExists = iVacArg;
    /* A system cannot have both a prealigner and vacuum indexer on the second Galil card. */
    if ((iPreArg == TRUE) && (iVacArg == TRUE))
        return FAILURE;
    if ((iRobArg == FALSE) && (iPreArg == FALSE) && (iVacArg == FALSE))
        return FAILURE;

    /* The comm port number that user wishes to run diagnostics through. */
    iDiagPortNum = iPortNumArg;

    /* Sets the teach pendant port to the user selected port number. */
    if (SERSetCommVals(iDiagPortNum, TT_PORT, TRUE) == FAILURE)
        return FAILURE;
    if (SERSetCommVals(iDiagPortNum, ECHO_MODE, FALSE) == FAILURE)
        return FAILURE;
    if (TTSetEchoMode(FALSE) == FAILURE)
        return FAILURE;

    /* Sets up the teach pendant with the appropriate settings. */
    if (TTSetKNPMode(FALSE) == FAILURE)
        return FAILURE;
    if (TTSetCursorType(NO_BLINK) == FAILURE)
        return FAILURE;
    if (TTSetCursorType(NO_CURSOR) == FAILURE)
        return FAILURE;

    /* Prints the opening diagnostics mode screen. */
    if (TTClearScreen() == FAILURE)
        return FAILURE;
    if (TTPrintsAt(2, 6, "DIAGNOSTICS") == FAILURE)
        return FAILURE;
    if (TTPrintsAt(4, 1, " ESC-QUIT   HERE-GO") == FAILURE)
        return FAILURE;

    /* Waits for ESC (quit) or ENTER (go). */
    iValidKey = FALSE;
    while (!iValidKey)
    {
        if (SERGetcRxBuff(iDiagPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;
        switch (cButtonPressed)
        {
            case 'Z':
            case ESC:
                iQuitKey = TRUE;
                iValidKey = TRUE;
                break;

            case CR:
                iQuitKey = FALSE;
                iValidKey = TRUE;
                break;
        }
    }

    /* If enter was pressed... */
    if (!iQuitKey)
    {
        /* ...continue by asking which component to perform diagnostics on. */
        if (TTClearScreen() == FAILURE)
            return FAILURE;
        if (TTPrintsAt(2, 6, "SELECT MODE") == FAILURE)
            return FAILURE;
        strcpy(caRobPreMsg, "");
        if (iPreExists)
            strcat(caRobPreMsg, " P-PRE    ");
        if (iVacExists)
            strcat(caRobPreMsg, " I-INX    ");
        if (iRobExists)
            strcat(caRobPreMsg, " R-ROBOT");
        if (TTPrintsAt(4, 1, caRobPreMsg) == FAILURE)
            return FAILURE;

        /* Waits for a P for prealigner, R for robot, or I for vacuum indexer,
         * depending on what is present. */
        iValidKey = FALSE;
        while (!iValidKey)
        {
            if (SERGetcRxBuff(iDiagPortNum, &cButtonPressed, FALSE) == FAILURE)
                return FAILURE;
            switch (cButtonPressed)
            {
                case 'R':
                case 'r':
                    /* If a robot doesn't exists, R or r are not valid options
                     * and do not register with a keypress. */
                    if (iRobExists)
                    {
                        iDevMode = 0;
                        iValidKey = TRUE;
                    }
                    break;

                case 'P':
                case 'p':
                    /* If a prealigner doesn't exist, P or p are not valid options
                     * and do not register with a keypress. */
                    if (iPreExists)
                    {
                        iDevMode = 1;
                        iValidKey = TRUE;
                    }
                    break;

                case 'I':
                case 'i':
                    /* If a vacuum indexer doesn't exist, I or i are not valid options
                     * and do not register with a keypress. */
                    if (iVacExists)
                    {
                        iDevMode = 1;
                        iValidKey = TRUE;
                    }
                    break;
            }
        }

        /* Servo off all relevant axes so user can move them. */
        iSVOffSuccess = TRUE;
        if (iRobExists && iSVOffSuccess)
        {
            if (ROServoMCOff(RO_AXIS_ALL) == FAILURE)
                iSVOffSuccess = FALSE;
        }
        if ((iPreExists || iVacExists) && iSVOffSuccess)
        {
            if (ROGetSpecialAxis(RO_SINGLE_PRE))
            {
                if (ROServoMCOff(ROGetSpecialAxis(RO_SINGLE_PRE)) == FAILURE)
                    iSVOffSuccess = FALSE;
            }
            else
            {
                if (ROServoMCOff(RO_AXIS_all) == FAILURE)
                    iSVOffSuccess = FALSE;
            }
        }
        if (ROGetSpecialAxis(RO_TRACK) && iSVOffSuccess)
        {
            if (ROServoMCOff(ROGetSpecialAxis(RO_TRACK)) == FAILURE)
                iSVOffSuccess = FALSE;
        }
        if (ROGetSpecialAxis(RO_FLIPPER) && iSVOffSuccess)
        {
            if (ROServoMCOff(ROGetSpecialAxis(RO_FLIPPER)) == FAILURE)
                iSVOffSuccess = FALSE;
        }
        if (ROGetSpecialAxis(RO_DUAL_ARM) && iSVOffSuccess)
        {
            if (ROServoMCOff(ROGetSpecialAxis(RO_DUAL_ARM)) == FAILURE)
                iSVOffSuccess = FALSE;
        }
        if (ROGetSpecialAxis(RO_INDEXER_T2) && iSVOffSuccess)
        {
            if (ROServoMCOff(ROGetSpecialAxis(RO_INDEXER_T2)) == FAILURE)
                iSVOffSuccess = FALSE;
        }
        if (iSVOffSuccess)
        {
            /* Use key-not-pressed mode. */
            if (TTSetKNPMode(TRUE) == FAILURE)
                return FAILURE;
            /* Run the diagnostics menu which calls its subsidiary functions.
             * This function call is NOT checked for a FAILURE return because
             * if any function returns a failure, we want to exit
             * diagnostics mode gracefully.*/
            DiagMenu();
            /* Turn key-not-pressed mode back off for normal operation. */
            if (TTSetKNPMode(FALSE) == FAILURE)
                return FAILURE;
            /* Make sure all axes are ready to servo on. We don't want to take
             * any chances with SVON efter exiting diagnostics mode. */
            if (GAAmpInit() == FAILURE)
                return FAILURE;
        }
    }

    /* Reset all Interrupt flags. */
    MASetFlagsToFalse();

    /* Restore original com port setting. */
    if (SERSetCommVals(COM2, TT_PORT, TRUE) == FAILURE)
        return FAILURE;

    /* Set up the teach pendant LCD screen appropriately... */
    if (SERGetCmdPort() == COM2)
    {
        /* ...for terminal mode... */
        if (TTClearScreen() == FAILURE)
            return FAILURE;
//        if (SERSetCommVals(SERGetCmdPort(), ECHO_MODE, TRUE) == FAILURE)
//            return FAILURE;
        if (TTSetEchoMode(TRUE) == FAILURE)
            return FAILURE;
        if (TTSetCursorPos(4, 1) == FAILURE)
            return FAILURE;
        if (TTSetCursorType(ENABLE_CURSOR) == FAILURE)
            return FAILURE;
        if (TTSetCursorType(BLINK) == FAILURE)
            return FAILURE;
    }
    else
    {
        /* ...or host mode. */
        if (TTShowLogo() == FAILURE)
            return FAILURE;
    }

    /* Send a prompt to the command port. That could be on the teach
     * pendant or EQT. This is a point of controversy. All host PC's
     * can't necessarily handle the \r and \n. */
    if (TIGetTimerVals(LOCKED) == 123)
        sprintf(caRobPreMsg, "\r\n%c", PCGetWDPrompt());
    else
        sprintf(caRobPreMsg, "\r\n>");
    if (SERPutsTxBuff(SERGetCmdPort(), caRobPreMsg) == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagMenu
 *
 * Abstract:    Handles stepping through the diagnostics menu.
 *      It also calls the appropriate diagnostics test function
 *      as selected.
 *
 * Parameters:  None
 *
 * Returns: SUCCESS or FAILURE
 *
 * NOTE: The individual tests are not checked for FAILURE returned
 *      because we don't want to exit diagnostics mode on a complete
 *      test failure. The indication of a catastrophic failure is
 *      that the test number jumps back to the NVSRAM test.
 *
 * NOTE: It does NOT handle wrapping around the test
 *      numbers, i.e. robot step 13 wraps back to step 1. That
 *      is handled by the DiagGetAnswer function. Also, after each
 *      test is run, the next step is already selected to be
 *      displayed. That increment is handled by each individual
 *      test function.
 *
 ***************************************************************/
int DiagMenu()
{
    int iMinStep, iMaxStep;

    /* Setup the test numbers for a robot or prealigner/vacuum indexer. */
    if (iDevMode == 1)
    {
        /* The prealigner and vacuum indexer tests start at step 15. */
        iDiagStep = NUM_OF_STEPS+1;
        iMinStep = NUM_OF_STEPS+1;
        iMaxStep = MAX_STEPS;
    }
    else
    {
        /* The robot tests start at step 1. */
        iDiagStep = 1;
        iMinStep = 1;
        iMaxStep = NUM_OF_STEPS;
    }

    while (1)
    {
        /* Get the next step from the user. The primary user I/O loop occurs in this function. */
        if (DiagGetAnswer(&iMinStep, &iMaxStep) == FAILURE)
            return FAILURE;
        if (TTClearScreen() == FAILURE)
            return FAILURE;

        /* Aside from the NVSRAM test, no other tests will run if
         * there has been a Galil error. So reset the test counter
         * to the NVSRAM test if there has been a Galil failure. */
        if (!GAGetUseGalilFlag() && (iDiagStep > 2))
        {
            if (TTPrintsAt(1, 1, " Galil Comm. Error") == FAILURE)
                return FAILURE;
            if (TTPrintsAt(2, 1, "    Try Step #2") == FAILURE)
                return FAILURE;
            iDiagStep = 2+NUM_OF_STEPS*iDevMode;
        }
        else
        {
            /* Run the selected test */
            switch(iDiagStep)
            {
                case -1:
                    /* Actual exit point for diagnostics mode. */
                    return SUCCESS;

                case 1:
                    /* NVSRAM Test */
                    DiagStep1();
                    break;

                case 2:
                    /* Galil and I/O Test */
                    DiagStep2();
                    break;

                case 3:
                    /* Encoder Test */
                    DiagStep3();
                    break;

                case 4:
                    /* Limit and Home Test */
                    DiagStep4();
                    break;

                case 5:
                    /* Setup Amp Board */
                    DiagStep5();
                    break;

                case 6:
                    /* Setup Amp Mode */
                    DiagStep6();
                    break;

                case 7:
                    /* Servo Test */
                    DiagStep7();
                    break;

                case 8:
                    /* Home Test and Set */
                    DiagStep8();
                    break;

                case 9:
                    /* R/W Home Test & Set */
                    DiagStep9();
                    break;

                case 10:
                    /* Scaling Test */
                    DiagStep10();
                    break;

                case 11:
                    /* Vacuum Test */
                    DiagStep11();
                    break;

                case 12:
                    /* Robot I/O Test */
                    DiagStep12();
                    break;

                case 13:
                    /* Mapper Test */
                    DiagStep13();
                    break;

                case 14:
                    /* CCD & Chuck Test */
                    DiagStep14();
                    break;

                case 15:
                    /* NVSRAM Test */
                    DiagStep1();
                    break;

                case 16:
                    /* Galil and I/O Test */
                    DiagStep2();
                    break;

                case 17:
                    /* Encoder Test */
                    DiagStep3();
                    break;

                case 18:
                    /* Limit and Home Test */
                    DiagStep4();
                    break;

                case 19:
                    /* Servo Test */
                    DiagStep7();
                    break;

                case 20:
                    /* Home Test and Set */
                    DiagStep8();
                    break;

                case 21:
                    /* R/Z Home Test & Set */
                    DiagStep9();
                    break;

                case 22:
                    /* Scaling Test */
                    DiagStep10();
                    break;

                case 23:
                    /* Vacuum Test */
                    DiagStep11();
                    break;

                case 24:
                    /* Pre I/O Test */
                    DiagStep12();
                    break;

                case 25:
                    /* CCD & Chuck Test */
                    DiagStep14();
                    break;

                case 26:
                    /* Mapper Test */
                    DiagStep13();
                    break;
            }
        }

        /* After the test is complete, the user must press Enter to continue. */
        if (TTPrintsAt(4, 1, "Press \"Enter\"       ") == FAILURE)
            return FAILURE;

        /* Wait for user to press enter before continuing. */
        cButtonPressed = EOF;
        while (cButtonPressed != CR)
        {
            if (SERGetcRxBuff(iDiagPortNum, &cButtonPressed, FALSE) == FAILURE)
                return FAILURE;
        }
    }

    /* Unreachable code. The actual return point is at case -1 */
    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagGetAnswer
 *
 * Abstract:    Gets the user's response whether to go to the
 *      next step, the previous step, exit, run the current step,
 *      or toggle to the aligner diagnostics menu
 *
 * Parameters:
 *      iMinStepArg     (in/out) The lowest step number. See the
 *          #define section of the header file for a more detailed
 *          description of the range of the steps
 *      iMaxStepArg     (in/out) The highest step number. This changes
 *          dependent on whether robot, prealigner, or vacuum indexer
 *          tests are being run.
 *
 * Returns: SUCCESS or FAILURE
 *
 ***************************************************************/
int DiagGetAnswer(int *iMinStepArg, int *iMaxStepArg)
{
    int iUpdateScn = TRUE;  /* Whether or not to update the teach pendant screen. */
    int iExitLoop = FALSE;  /* Exits the user keypress registry loop. */
    char caTempString[25];  /* To display the currently selected test number. */

    /* Flush the buffer */
    if (SERFlushComBuffer(iDiagPortNum) == FAILURE)
        return FAILURE;

    while (!iExitLoop)
    {
        /* If a key was pressed, the teach pendant screen may have to be updated. */
        if (iUpdateScn)
        {
            if (TTClearScreen() == FAILURE)
                return FAILURE;

            /* Print the name and number of the next test. Retrieve the test name
             * string from the table, indexed by step number. */
            if (TTPrintsAt(2, 1, DIAG_STEP_STR[iDiagStep]) == FAILURE)
                return FAILURE;
            if (TTPrintsAt(4, 1, "Next Step:") == FAILURE)
                return FAILURE;
//            itoa(iDiagStep, caTempString, 10);
	    sprintf(caTempString, "%d", iDiagStep);
            if (TTPrintsAt(4, 12, caTempString) == FAILURE)
                return FAILURE;
            /* Indicate that the screen has been updated. */
            iUpdateScn = FALSE;
        }

        /* Get the next key press if there is one. */
        if (SERGetcRxBuff(iDiagPortNum, &cButtonPressed, FALSE) == FAILURE)
            return FAILURE;

        switch (cButtonPressed)
        {
            case 'Z':   /* End diagnostic mode */
            case ESC:
                /* Remember, this value is the actual exit condition. */
                iDiagStep = -1;
                iExitLoop = TRUE;
                break;

            case CR:    /* Run the currently displayed test */
                iExitLoop = TRUE;
                break;

            case 18:    /* Ctrl-R - Toggle between robot and prealigner/vacuum indexer menus */
                if (*iMinStepArg >= (NUM_OF_STEPS+1))
                {
                    if (iRobExists)
                    {
                        iDevMode = 0;
                        *iMinStepArg = 1;
                        *iMaxStepArg = NUM_OF_STEPS;
                    }
                }
                else if (*iMinStepArg <= NUM_OF_STEPS)
                {
                    if (iPreExists || iVacExists)
                    {
                        iDevMode = 1;
                        *iMinStepArg = NUM_OF_STEPS+1;
                        *iMaxStepArg = MAX_STEPS;
                    }
                }

                /* Prealigner/vacuum indexer step numbers range from 15 to 26 while robot step
                 * numbers range from 1 to 14. When the modes are toggled, the step
                 * number will be out of the range of the new mode. It must be adjusted. */
                iDiagStep = *iMinStepArg;

                /* The step number changed, so update the teach pendant screen. */
                iUpdateScn = TRUE;
                break;

            case 'S':   /* Step forward to next test */
                iDiagStep++;
                /* Handle wraparaound for Galil card 1 diagnostics. */
                if (iDevMode)
                {
                    /* Normally wraparound at CCD test or greater, which includes the scanner test. */
                    if (iDiagStep > (*iMaxStepArg-1))
                        iDiagStep = *iMinStepArg;
                    /* For vacuum indexer systems... */
                    if (iVacExists)
                    {
                        /*  ...skip the CCD test and run the scanner test instead... */
                        if (iDiagStep == (*iMaxStepArg-1))
                            iDiagStep = *iMaxStepArg;
                        /* ...or skip the vacuum test and run the pre I/O test instead... */
                        if (iDiagStep == (*iMaxStepArg-3))
                            iDiagStep = *iMaxStepArg-2;
                        /* ...or skip the set home offset and run the scaling test instead. */
                        if (iDiagStep == (*iMaxStepArg-5))
                            iDiagStep = *iMaxStepArg-4;
                    }
                }
                /* Handle wraparaound for Galil card 0 diagnostics. */
                else
                {
                    /* Normally wraparound at CCD test which is past the scanner test. */
                    if (iDiagStep > *iMaxStepArg)
                        iDiagStep = *iMinStepArg;
                    /* For vacuum indexer systems, skip the scanner test and run the CCD test instead. */
                    if (iDiagDefineFlag & DFVACPRE)
                    {
                        if (iDiagStep == (*iMaxStepArg-1))
                            iDiagStep = *iMaxStepArg;
                    }
                    /* For NON vacuum indexer systems, if there is no aligner on the first Galil card,
                     * wraparound after the scanner test. Don't wait for the CCD test. */
                    else if (ROGetSpecialAxis(RO_SINGLE_PRE) != RO_AXIS_W)
                    {
                        if (iDiagStep > (*iMaxStepArg-1))
                            iDiagStep = *iMinStepArg;
                    }
                }
                /* The step number changed, so update the teach pendant screen. */
                iUpdateScn = TRUE;
                break;

            case 'R':   /* Step back to previous test */
                iDiagStep--;
                /* Handle wraparaound for Galil card 1 diagnostics. */
                if (iDevMode)
                {
                    /* For vacuum indexer systems... */
                    if (iVacExists)
                    {
                        /* ...wraparound to the scanner test... */
                        if (iDiagStep < *iMinStepArg)
                            iDiagStep = *iMaxStepArg;
                        /* ...and skip the CCD test. */
                        if (iDiagStep == (*iMaxStepArg-1))
                            iDiagStep = *iMaxStepArg-2;
                        /* ...or skip the vacuum test... */
                        if (iDiagStep == (*iMaxStepArg-3))
                            iDiagStep = *iMaxStepArg-4;
                        /* ...or skip the set home offset. */
                        if (iDiagStep == (*iMaxStepArg-5))
                            iDiagStep = *iMaxStepArg-6;
                    }
                    /* Normally wraparound to CCD test. */
                    if (iDiagStep < *iMinStepArg)
                        iDiagStep = *iMaxStepArg-1;
                }
                else
                {
                    /* For vacuum indexer systems... */
                    if (iDiagDefineFlag & DFVACPRE)
                    {
                        /* ...wraparound to the CCD test... */
                        if (iDiagStep < *iMinStepArg)
                            iDiagStep = *iMaxStepArg;
                        /* ...and skip the scanner test. */
                        if (iDiagStep == (*iMaxStepArg-1))
                            iDiagStep = *iMaxStepArg-2;
                    }
                    /* For NON vacuum indexer systems, if there is no aligner on the first Galil card,
                     * wraparound to the scanner test, NOT the CCD test. */
                    else if (ROGetSpecialAxis(RO_SINGLE_PRE) != RO_AXIS_W)
                    {
                        if (iDiagStep < *iMinStepArg)
                            iDiagStep = *iMaxStepArg-1;
                    }
                    /* Normally wraparound to CCD test which is past the scanner test. */
                    if (iDiagStep < *iMinStepArg)
                        iDiagStep = *iMaxStepArg;
                }
                /* The step number changed, so update the teach pendant screen. */
                iUpdateScn = TRUE;
                break;
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep1
 *
 * Abstract:    Runs the NVSRAM test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep1()
{
    if (FIOTestNVSRAM(iDevMode) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    iDiagStep = 2+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep2
 *
 * Abstract:    Runs the Galil test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep2()
{
    if (GATestGalil(iDevMode) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }

    if (IOTestIO(iDevMode) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    iDiagStep = 3+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep3
 *
 * Abstract:    Runs the encoder test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep3()
{
    if (GATestEncoder(iDevMode, iVacExists) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    iDiagStep = 4+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep4
 *
 * Abstract:    Runs the limit switch and home test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep4()
{
    if (GATestLSHS(iDevMode, iVacExists) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    iDiagStep = 5+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep5
 *
 * Abstract:    Sets up the amplifer board (obsolete)
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep5()
{
//    if (GASetupAMPBoard(iDevMode) == FAILURE)
    if (GASetupAMPMode(iDevMode, iVacExists) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    /* The Setup AMP Board test doesn't exist in prealigner mode. */
    iDiagStep = 6;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep6
 *
 * Abstract:    Sets up the amplifier mode (obsolete)
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep6()
{
    if (GASetupAMPMode(iDevMode, iVacExists) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    /* The Setup AMP Mode test doesn't exist in prealigner mode either. */
    iDiagStep = 7;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep7
 *
 * Abstract:    Runs the servo test. Actually just calls step 6,
 *      Setup AMP Mode. Why is step 6 just not called directly?
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep7()
{
    if (DiagStep6() == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    /* Prealigner mode has to compensate for the 2 missing tests
     * (the Setup AMP Board/Mode tests). Robot mode just keeps
     * chugging along. This difference carries through all the
     * rest of the tests from this point on. */
    if (iDevMode)
        iDiagStep = 6+NUM_OF_STEPS*iDevMode;
    else
        iDiagStep = 8+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep8
 *
 * Abstract:    Runs the home test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep8()
{
    if (ROTestHome(iDevMode, iVacExists) == FAILURE)
    {
        ROSetHomed(0);
        SSSetStatusWord(HOME_NOT_EXED, TRUE);
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    if (iDevMode)
    {
        if (iVacExists)
            iDiagStep = 8+NUM_OF_STEPS*iDevMode;
        else
            iDiagStep = 7+NUM_OF_STEPS*iDevMode;
    }
    else
        iDiagStep = 9+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep9
 *
 * Abstract:    Sets and checks the radial home position
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep9()
{
    if (!iDevMode)
    {
        if (ROSetHomeR() == FAILURE)
        {
            ROSetHomed(0);
            SSSetStatusWord(HOME_NOT_EXED, TRUE);
            iDiagStep = 1+NUM_OF_STEPS*iDevMode;
            return FAILURE;
        }
    }
    else
    {
        if (ROPresetHomeR() == FAILURE)
        {
            ROSetHomed(0);
            SSSetStatusWord(HOME_NOT_EXED, TRUE);
            iDiagStep = 1+NUM_OF_STEPS*iDevMode;
            return FAILURE;
        }
    }
    if (iDevMode)
        iDiagStep = 8+NUM_OF_STEPS*iDevMode;
    else
        iDiagStep = 10+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep10
 *
 * Abstract:    Sets and checks scaling values
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep10()
{
    if (ROSetTestScaling(iDevMode, iVacExists) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    if (iDevMode)
    {
        if (iVacExists)
            iDiagStep = 10+NUM_OF_STEPS*iDevMode;
        else
            iDiagStep = 9+NUM_OF_STEPS*iDevMode;
    }
    else
        iDiagStep = 11+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep11
 *
 * Abstract:    Runs the vacuum test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep11()
{
    if (ROTestVacuum(iDevMode) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    if (iDevMode)
        iDiagStep = 10+NUM_OF_STEPS*iDevMode;
    else
        iDiagStep = 12+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep12
 *
 * Abstract:    Runs the robot I/O test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep12()
{
    if (ROTestRobotIO(iDevMode) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    /* Decide which step to perform next for Galil card 1. */
    if (iDevMode)
    {
        /* For vacuum indexer systems, proceed to the scanner test. */
        if (iVacExists)
            iDiagStep = 12+NUM_OF_STEPS*iDevMode;
        /* For all other systems, proceed to the CCD test. */
        else
            iDiagStep = 11+NUM_OF_STEPS*iDevMode;
    }
    /* Decide which step to perform next for Galil card 0. */
    else
    {
        /* For vacuum prealigner systems, proceed to the CCD test. */
        if (iDiagDefineFlag & DFVACPRE)
            iDiagStep = 14+NUM_OF_STEPS*iDevMode;
        /* For all other systems, proceed to the scanner test. */
        else
            iDiagStep = 13+NUM_OF_STEPS*iDevMode;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep13
 *
 * Abstract:    Runs the scanner test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep13()
{
    if (MPTestScanning(iDevMode, iVacExists) == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    /* All tests complete. Wraparound back to first test. */
    if (iDevMode)
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
    else
    {
        /* Unless it is an atmospheric system and a single axis prealinger
         * exists on the first card. Then we have to perform the CCD test also. */
        if ((~iDiagDefineFlag & DFVACPRE) && (ROGetSpecialAxis(RO_SINGLE_PRE) == RO_AXIS_W))
            iDiagStep = 14+NUM_OF_STEPS*iDevMode;
        else
            iDiagStep = 1+NUM_OF_STEPS*iDevMode;
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    DiagStep14
 *
 * Abstract:    Runs the CCD and chuck test
 *
 * Parameters:  None
 *
 * Returns:     The next test number
 *
 ***************************************************************/
int DiagStep14()
{
    if (ALTestChuckAndCCD() == FAILURE)
    {
        iDiagStep = 1+NUM_OF_STEPS*iDevMode;
        return FAILURE;
    }
    /* All tests complete. Wraparound back to first test. */
    iDiagStep = 1+NUM_OF_STEPS*iDevMode;

    return SUCCESS;
}


#ifdef NOFP
int ALTestChuckAndCCD()
{
    return SUCCESS;
}
int WAIrsCCDSetCommandImage(unsigned uCommandImageArg)
{
    return FAILURE;
}
int ALAbort()
{
    return SUCCESS;
}
int ALGetIntVars(int iVariableTypeArg, int iIndexArg, int *ipVariableValueArg)
{
    *ipVariableValueArg = FALSE;
    return SUCCESS;
}
#endif


/****************************************************************
 *
 * Function:    DiagGetFlag
 *
 * Abstract:    Gets the value of the diag flag (was go_to_teach
 *      variable). See header notes for more details.
 *
 * Parameters:  None
 *
 * Returns:     The value of the diag flag
 *
 * Replaces: go_to_teach variable
 * Used in: main, execute
 *
 ***************************************************************/
int DiagGetFlag()
{
    return iDiagFlag;
}


/****************************************************************
 *
 * Function:    DiagSetFlag
 *
 * Abstract:    Sets the value of the diag flag (was go_to_teach
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
int DiagSetFlag(int iPortNumArg)
{
    if ((iPortNumArg != COM1) && (iPortNumArg != COM2) &&
        (iPortNumArg != COM3) && (iPortNumArg != COM4) &&
	(iPortNumArg != NO_PORT_NUM))
    {
        return FAILURE;
    }

    iDiagFlag = iPortNumArg;

    return SUCCESS;
}
