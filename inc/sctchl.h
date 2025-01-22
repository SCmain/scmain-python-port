/****************************************************************
 *
 * Program:     Controller firmware
 * File:        teachl.h
 * Functions:   TMTeachMenu
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
 *              TMTeachEdit
 *              TMRunCommand
 *              TMWaitTchMotionComplete
 *              TMWaitTchMotionNoError
 *
 * Description: Handles local variables and functions needed
 *      for teach mode
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
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

#ifndef _H_TEACH_LOCAL_H
#define _H_TEACH_LOCAL_H

/********** DEFINES **********/

#define MIC_PER_MIL 25L     /* Millimeter to Micron conversion factor. */
#define MIL_DEG_PER_HUND 25L    /* Hundreths of a degree to milli-degrees conversion factor. */
#define MAX_TCH_AXES 10     /* The maximum number of axes that can be taught. */

#define T 0                 /* Array position and existence definition for Theta axis. */
#define R 1                 /* Array position and existence definition for Radial axis. */
#define Z 2                 /* Array position and existence definition for Z axis. */
#define TRK 3               /* Array position and existence definition for Track axis. */
#define DBM 4               /* Array position and existence definition for Dual Arm axis. */
#define FLP 5               /* Array position and existence definition for Flipper axis. */
#define VACZ1 6             /* Array position and existence definition for Vacuum Indexer Z 1 axis. */
#define VACT1 7             /* Array position and existence definition for Vacuum Indexer Theta 1 axis. */
#define VACZ2 8             /* Array position and existence definition for Vacuum Indexer Z 2 axis. */
#define VACT2 9             /* Array position and existence definition for Vacuum Indexer Theta 2 axis. */

/* Mechanism defined array axis positions. */
int iaMechAxisIdx[MAX_TCH_AXES] = { 0, 1, 2, 3, 3, 3, 1, 0, 2, 3 };

/* Mechanism defined axis definitions. (To be filled in during initialization) */
unsigned long ulaMechAxisDef[MAX_TCH_AXES];

/* Positive limit switch triggered user messages. */
char *cpaSoftPosLimMessage[MAX_TCH_AXES] =
{
    "T-Pos Soft Lim ",
    "R-Pos Soft Lim ",
    "Z-Pos Soft Lim ",
    "Trk-Pos Soft Lm",
    "DA-Pos Soft Lim",
    "Fpr-Pos Soft Lm",
    "r-Pos Soft Lim ",
    "t-Pos Soft Lim ",
    "z-Pos Soft Lim ",
    "w-Pos Soft Lim "
};

/* Negative limit switch triggered user messages. */
char *cpaSoftNegLimMessage[MAX_TCH_AXES] =
{
    "T-Neg Soft Lim ",
    "R-Neg Soft Lim ",
    "Z-Neg Soft Lim ",
    "Trk-Neg Soft Lm",
    "DA-Neg Soft Lim",
    "Fpr-Neg Soft Lm",
    "r-Neg Soft Lim ",
    "t-Neg Soft Lim ",
    "z-Neg Soft Lim ",
    "w-Neg Soft Lim "
};

/* Limit switch triggered user message symbols. */
char caLMSymbol[4] = {' ', '+', '-', '?'};

/* Toggle Servo function axis user messages. */
char *cpAxisName[MAX_TCH_AXES] =
{
    "T",
    "R",
    "Z",
    "Trk",
    "DA",
    "Fpr",
    "r",
    "t",
    "z",
    "w"
};

/* Jog speed setting user messages. */
char *pcaTchSpdStr[MAX_TCH_AXES] = { " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10" };

/* 2-D array for jog speeds, Col-speed, Row-axis. */
int iaTeachSpeed[MAX_TCH_AXES][11] =
{
    {0, 2,  5, 10, 20, 50, 100, 200, 300, 400, 500},  /* T */
    {0, 5, 10, 20, 30, 50, 100, 200, 300, 400, 500},  /* R */
    {0, 2, 10, 15, 30, 50,  70,  80, 100, 120, 140},  /* Z */
    {0, 2,  5, 10, 20, 30,  50, 100, 200, 300, 400},  /* Track */
    {0, 2,  5, 10, 20, 50, 100, 200, 300, 400, 500},  /* Dual Arm */
    {0, 1,  2,  5, 10, 20,  30,  40,  50,  60,  80},  /* Flipper */
    {0, 1,  2,  5, 10, 11,  12,  15,  20,  25,  30},   /* Z1 */
    {0, 1,  2,  5, 10, 11,  12,  15,  20,  25,  30},   /* T1 */
    {0, 1,  2,  5, 10, 11,  12,  15,  20,  25,  30},   /* Z2 */
    {0, 1,  2,  5, 10, 11,  12,  15,  20,  25,  30}    /* T2 */
};

/* 2-D array for jog accelerations and decelerations, Col-accel, Row-axis. */
int iaTeachAccel[MAX_TCH_AXES][11] =
{
    {0, 2500, 2500, 2500, 2500, 2500, 5000, 5000, 5000, 5000, 5000},    /* T */
    {0, 2500, 2500, 2500, 2500, 2500, 3000, 3000, 3000, 3000, 3000},    /* R */
    {0, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500},    /* Z */
    {0, 2500, 2500, 2500, 2500, 2500, 5000, 5000, 5000, 5000, 5000},    /* Track */
    {0, 2500, 2500, 2500, 2500, 2500, 5000, 5000, 5000, 5000, 5000},    /* Dual Arm */
    {0, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500},    /* Flipper */
    {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},              /* Z1 */
    {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},              /* T1 */
    {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},              /* Z2 */
    {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100}               /* T2 */
};

/* Speed/accel index for each axis. */
int iaSpeedIndex[MAX_TCH_AXES] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// Smart Motion Vector Distance for teaching: R speed Index will be used.
int iaSmartInterval[11] = { 100, 200, 350, 450, 600, 750, 850, 1000, 1200, 1400, 1600 };

/********** VARIABLES USED LOCALLY **********/

int iaAxisExists[MAX_TCH_AXES]; /* Initialization values based on whether
                                 * certain axes are present. */
int iTMPortNum;         /* Port that user chooses to run teach mode on. */
int iScanMode = FALSE;  /* Toggles scanner mode on/off. */
int iTrkMode = FALSE;   /* Toggles from robot mode to track mode. */
int iFprMode = FALSE;   /* Toggles from robot mode to flipper mode. */
int iVacMode = FALSE;   /* Toggles from robot mode to vacuum indexer mode. */
int iSmartMode = FALSE; /* Smart-Motion Mode set if station CSI 15 = 313 */

long laSaveSpeed[MAX_TCH_AXES];    /* Speed settings entering teach mode;
                                    * restored before exit. */
long laSaveAccel[MAX_TCH_AXES];    /* Acceleration settings entering teach mode;
                                    * restored before exit. */
long laSaveDecel[MAX_TCH_AXES];    /* Deceleration settings entering teach mode;
                                    * restored before exit. */
long laSaveIP[MAX_TCH_AXES];       /* Interrupt position settings entering teach mode;
                                    * restored before exit. */
char cTchStn = 'A';     /* Station currently being taught (A-Z, a-z). */
int iInputBit = 0;      /* Input port value. */
int iOutputBit = 0;     /* Output port value. */
int iSelAxis = T;       /* Selected axis. */
int iTeachFlag = 0;     /* The new go_to_teach flag. */
int iCounterIndex;      /* Counter checked out from the timer module. */
long lUnitConvDist;     /* Conversion factor for distance calculations,
                         * e.g. mils to microns. */
long lUnitConvAngle;    /* Conversion factor for angle calculations,
                         * e.g. hundreths of a degree to milli-degrees. */
int iTMDefineFlag = 0;  /* The define flags for the current system configuration */
int iTMEmulator = 0;    /* The emulation type of the current system configuration */
int iUseDBM = 0;        /* in case using Dual Arm, it's set to 1 */


/********** FUNCTIONS USED LOCALLY **********/

/****************************************************************
 *
 * Replaces: teach_menu
 * Used in: teach
 * Abstract:    Starts teach mode menu. It starts by saving all the
 *      speeds, accelerations, decelerations, and Galil ID command
 *      settings. Then it sets the new values which are much slower
 *      for teach mode. It then displays the teach menu and
 *      checks for keypresses calling the TMDoTeachKey function to
 *      process those keypresses. That main teach loop handles all
 *      I/O to operate the teach mode menu. When teach mode is exited,
 *      all previously saved values are restored.
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
int TMTeachMenu();

/****************************************************************
 *
 * Replaces: read_IP
 * Used in: teach
 * Abstract:    Gets the Galil ID command settings so they
 *      can be saved and restored later. (TOT in-position window).
 *
 ***************************************************************/
int TMGetIP();

/****************************************************************
 *
 * Replaces: set_IP
 * Used in: teach
 * Abstract:    Sets the Galil ID command settings so to
 *      previously saved values. (TOT in-position window).
 *
 ***************************************************************/
int TMSetIP(long *lpIPArg);

/****************************************************************
 *
 * Replaces: do_teach_key, do_teach_key1
 * Used in: teach
 * Abstract:    Services keypresses from the teach pendant while
 *      in teach mode. Processes the key pressed. Calls the appropriate
 *      functions or changes the relevant variables.
 *
 ***************************************************************/
int TMDoTeachKey(char cCharToProcessArg);

/****************************************************************
 *
 * Replaces: draw_teach_scr, draw_teach_scr1
 * Used in: teach
 * Abstract:    Redraw the part of the teach screen that is not
 *      constantly refreshed. Essentially, draw the robot/track
 *      axes labelling portion of the teach pendant screen.
 *
 ***************************************************************/
int TMDrawTeachScr();

/****************************************************************
 *
 * Replaces: display_axes, display_axes1
 * Used in: teach
 * Abstract:    Display part of teach screen associated with
 *      the axes that is constantly changing and needs to be
 *      refreshed. Essentially, print out the axis specific
 *      information to the teach pendant screen: current position,
 *      limit switch status, jog speed, and selected axis.
 *
 ***************************************************************/
int TMDisplayAxes();

/****************************************************************
 *
 * Replaces: display_others
 * Used in: teach
 * Abstract:    Print out the non-robot/track-specific
 *      information to the teach pendant screen. Update right
 *      hand side of display: station name, input bit status,
 *      output bit condition, and scan mode status. The only reason
 *      this is constantly refreshed is because the input bit
 *      status may change. Seems a little overkill, doesn't it?
 *
 ***************************************************************/
int TMDisplayOthers();

/****************************************************************
 *
 * Replaces: do_align
 * Used in: teach
 * Abstract:    Non-simulated wafer align. Actually calls the
 *      ALIGN macro.
 *
 ***************************************************************/
int TMDoAlign();

/****************************************************************
 *
 * Replaces: chg_coord
 * Used in: teach
 * Abstract:    Routine to handle changing of the teach coordinate
 *      station. It waits for the user to input the new station
 *      number, checks the new number for validity, and updates
 *      all relevant variables locally and globally. The TP
 *      LCD screen update is done through TMDisplayOthers.
 *
 ***************************************************************/
int TMChgCoord();

/****************************************************************
 *
 * Replaces: set_tch_spd, set_tch_spd1
 * Used in: teach
 * Abstract:    Changes the jog speed for the selected axis.
 *      It changes the speed index, changes the speeds in the
 *      Galil card, and prints the change to the teach pendant
 *      LCD screen.
 *
 * NOTE: In the iaSpeedIndex array the column in the 2-D iaTeachSpeed
 *      array is saved by axis. This number is also printed
 *      to the screen as the speed setting for the user to reference.
 *      It ranges from 1 to 10. Then the actual jog speed can be
 *      retrieved from the 2-D array by the speed index and axis.
 *
 ***************************************************************/
int TMSetTchSpd(int iAxis, int iDirection);

/****************************************************************
 *
 * Replaces: teach_axis
 * Used in: teach
 * Abstract:    Sets the selected axis and jogs it or steps it.
 *      It also checks for limit switch conditions, like the limit
 *      switch can't be triggered already or the move can't
 *      start or go beyond the software limit switch positions.
 *
 ***************************************************************/
int TMTeachAxis(int iAxis, int iDirection);

/****************************************************************
 *
 * Replaces: store_coord
 * Used in: teach
 * Abstract:    Store coordinate to memory -- HERE command.
 *      This checks that all axes are homed. (All axes, including
 *      the track, are saved at once.) The software limits
 *      are checked to make sure the robot hasn't exceeded them.
 *      Then the coordinates are saved to the data file. In the
 *      case of scan mode, the user is allowed to calibrate the
 *      scanner at the newly taught station.
 *
 ***************************************************************/
int TMStoreCoord();

/****************************************************************
 *
 * Replaces: move_to_coord
 * Used in: teach
 * Abstract:    Moves all axes of the robot to a station.
 *      It checks that all axes are ready for motion.
 *      It asks the user if they want to retract first,
 *      for safety reasons, so the robot won't hit things
 *      when the track moves. Then it retracts (if requested),
 *      moves the track, moves the theta and Z axes, and finishes
 *      with an extend.
 *
 ***************************************************************/
int TMMoveToCoord();

/****************************************************************
 *
 * Replaces: pick_place_wafer
 * Used in: teach
 * Abstract:    Wafer pick or place. Simulated performs a Get or Put
 *      internally here using jog speeds. Non-simulated calls TMDoGetPut
 *      which calls the GET or PUT macros. It checks that all axes are
 *      ready for motion. Then it gets information from the user
 *      about stroke distance, offset distance, end-effector number,
 *      simulated or non-simulated mode, and slot number. Then it:
 *      1) Retracts, 2) Moves the track, 3) Moves theta and Z,
 *      4) Extends, 5) Handles the vacuum, 6) Strokes the Z, and
 *      7) Retracts.
 *
 ***************************************************************/
int TMPickPlaceWafer(int iPickArg);

/****************************************************************
 *
 * Replaces: toggle_servo
 * Used in: teach
 * Abstract:    Toggle servo ON/OFF for specified axis.
 *
 ***************************************************************/
int TMToggleServo(int iAxis);

/****************************************************************
 *
 * Replaces: wait_tch_move
 * Used in: teach
 * Abstract:    Wait for a teach mode motion to complete.
 *      This function is used when we don't know which axis
 *      we're waiting for motion to complete on, specifically in
 *      jog and step mode. We do know that in step and jog mode
 *      we can't move the track and the robot at the same time.
 *      So we wait for the axis based on mode.
 *
 ***************************************************************/
int TMWaitTchMove();

/****************************************************************
 *
 * Replaces: tch_motion_check
 * Used in: teach
 * Abstract:    Check if ready for motion and output message to teach pendant
 *      if not, i.e. makes sure that the robot is servoed on, homed, and not moving.
 *      Otherwise we can't command a teach motion. Note that the iAxis
 *      passed in must be in terms of mechanism module defines.
 *
 ***************************************************************/
int TMTchMotionCheck(unsigned uAxis);

/****************************************************************
 *
 * Replaces: do_get_put
 * Used in: teach
 * Abstract:    Non-simulated wafer get or put. Actually calls the
 *      GET or PUT macro.
 *
 ***************************************************************/
int TMDoGetPut(int iPickArg, long lSlotnArg);

/****************************************************************
 *
 * Replaces: tch_edit
 * Used in: do_get_put
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
 ******************************************************************/
int TMTeachEdit(long *lValue);

/****************************************************************
 *
 * Replaces: run_command
 * Used in: store_coord, do_ALIGN, do_FLIP, do_get_put
 * Abstract:    Run Command/Macro from Teach Mode. This is like
 *      a mini main loop. It runs the TSCN and GET and PUT
 *      macros.
 *
 ******************************************************************/
int TMRunCommand(char *cpCommand);

/****************************************************************
 *
 * Replaces: wait_tch_RBT_motion_complete
 * Used in: teach
 * Abstract:    Wait for a motion to complete. Just loop
 *      and wait for the after-motion flag to be changed by
 *      the ME_PROC. Update the screen while you wait. Also check if
 *      the user wants to abort the move.
 *
 ***************************************************************/
int TMWaitTchMotionComplete(unsigned uAxis);

/****************************************************************
 *
 *              NEW FUNCTION
 *
 * Abstract:    Restores the original speeds, accelerations,
 *      and deceleration that were saved upon teach mode entry.
 *
 ***************************************************************/
int TMRestoreSpeedAndAccel();

/****************************************************************
 *
 *              NEW FUNCTION
 *
 * Abstract:    Sets the speeds, accelerations, and decelerations
 *      that are selected by the user for jogging the axes.
 *
 ***************************************************************/
int TMSetJogSpeedAndAccel();

int TMWaitTchMotionNoError(unsigned uAxis);

int TMMoveCartesian(char cCharToProcessArg);

#endif
