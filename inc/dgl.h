/****************************************************************
 *
 * Program:     Controller firmware
 * File:        dgl.h
 * Functions:   DiagMenu
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
 * Description: Handles local variables and functions needed
 *      for diagnostics mode
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 * NOTE: The robot and preligner tests call the same functions on the whole.
 *          Within those individual functions, the iDevMode variable
 *          is used to differentiate between prealigner and robot
 *          specific operations, iDevMode=0 being robot and =1 being prealigner.
 *          So within the DiagMenu function, there is a long list
 *          of which function to call depending on which test is to be
 *          run. But the function numbers cycle with the only difference
 *          being the iDevMode parameter that is sent to the main
 *          operational function.
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
 ****************************************************************/

#ifndef _H_DIAG_LOCAL_H
#define _H_DIAG_LOCAL_H

/********** DEFINES **********/
#define NUM_OF_STEPS    14  /* Total number of tests (steps) in diagnostic mode.
                             * Also the index of the last robot test. */
#define MAX_STEPS       26  /* The index of the last prealigner/vacuum indexer test.
                             * Also the index of the highest step number reachable.*/

/* A table of the test display names indexed directly by step number. */
char DIAG_STEP_STR[27][20] =
{
    "                   ",
    "    NVSRAM TEST    ",
    " GALIL & I/O TEST  ",
    " ENCODER READ TEST ",
    " LIMIT & HOME TEST ",
    " AMP. BOARD SETUP  ",
    " AMP. MODULE SETUP ",
    "     SERVO TEST    ",
    "  HOME TEST & SET  ",
    "R/W-HOME TEST & SET",
    "SCALING FACTOR TEST",
    " VAC. VALVE/SENSOR ",
    "    ROBOT I/O'S    ",
    "   SCANNING TEST   ",
    " CCD & CHUCK TEST  ",
    "    NVSRAM TEST    ",
    " GALIL & I/O TEST  ",
    " ENCODER READ TEST ",
    " LIMIT & HOME TEST ",
    "     SERVO TEST    ",
    "  HOME TEST & SET  ",
    "R/Z HOME TEST & SET",
    "SCALING FACTOR TEST",
    " VAC. VALVE/SENSOR ",
    "     PRE I/O'S     ",
    " CCD & CHUCK TEST  ",
    "   SCANNING TEST   "
};


/********** VARIABLES USED LOCALLY **********/
int iRobExists = TRUE;  /* Indicates whether or not a robot is present */
int iPreExists = TRUE;  /* Indicates whether or not a pre-aligner is present */
int iVacExists = TRUE;  /* Indicates whether or not a vacuum indexer is present */
int iDiagPortNum;         /* The port number to run diagnostics from */
int iDevMode = 0;       /* Which card to run tests on */
int iDiagStep = 1;      /* Which step of the diagnostics menu is currently selected */
char cButtonPressed;    /* The character corresponding to the TP button pressed */
int iDiagFlag = 0;      /* The flag to indicate whether to run diagnostics mode
                         * and the port to run it on. Used through the main loop primarily. */
int iDiagDefineFlag = 0;  /* The system define flags */
int iDiagEmulator = 0;    /* The system emulation */


/********** FUNCTIONS USED LOCALLY **********/

/****************************************************************
 *
 * Used in: diagnostics
 * Description: Handles stepping through the diagnostics menu.
 *      It also calls the appropriate diagnostics test function
 *      as selected. It does NOT handle wrapping around the test
 *      numbers, i.e. robot step 13 wraps back to step 1. That
 *      is handled by the DiagGetAnswer function.
 *
 ***************************************************************/
int DiagMenu();

/****************************************************************
 *
 * Used in: diag_menu
 * Abstract:    Gets the user's response whether to go to the
 *      next step, the previous step, exit, run the current step,
 *      or toggle to the aligner diagnostics menu
 *
 ***************************************************************/
int DiagGetAnswer(int *iMinStepArg,
                int *iMaxStepArg);

/****************************************************************
 *
 * Used in: diag_menu
 * Abstract: Runs the individual diagnostics tests
 *
 ***************************************************************/

/* NVSRAM Test */
int DiagStep1();

/* Galil and I/O Test */
int DiagStep2();

/* Encoder Test */
int DiagStep3();

/* Limit and Home Test */
int DiagStep4();

/* Setup Amp Board */
int DiagStep5();

/* Setup Amp Mode */
int DiagStep6();

/* Servo Test */
int DiagStep7();

/* Home Test and Set */
int DiagStep8();

/* R Home Test */
int DiagStep9();

/* Scaling Test */
int DiagStep10();

/* Vacuum Test */
int DiagStep11();

/* Robot I/O Test */
int DiagStep12();

/* Mapper Test */
int DiagStep13();

/* CCD & Chuck Test */
int DiagStep14();

#endif
