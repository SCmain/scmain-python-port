/****************************************************************
 *
 * Program:     Controller firmware
 * File:        stdver.h
 * Functions:   InitVersionString
 *              GetVersionString
 *              GetFilename
 *              InitMechArrays
 *              ValidateSysCfgString
 *
 * Description: Manages the firmware and library versions
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#ifndef _H_STDVER_H
#define _H_STDVER_H

/********** DEFINES **********/

#define VERSTRLEN       40              /* string length for version */
#define LIBVERSTRING    "2.30"            /* library version build number */
//
// 2.00 - Merge 1.24 and 2.01, by adding smart-motion to 1.24.
//	  Add smart-motion capabilities to robot: coordinate conversion between
//	  world coordinate and robot (r-theta) coordinate systems.
//	  use Galil special command to move the robot in straight line.
//	  New commands added:
//	1. XMVC	A,T,R,Z - Move cartesian. From the current position
//	   		  move straight line to (T, R) ignore Z.
//	2. XSEE, XREE	- Set/Read End-Effector length in R unit
//	3. XSVS, XRVS	- Set/Read Vector Speed
//	4. XSVA, XRVA	- Set/Read Vector Accel
//	5. XSVD, XRVD	- Set/Read Vector Decel
//	6. XSAV		- Save vector op parameters
// 	7. XSRF, XRRF	- Set/Read Efem Reference Theta Angle
//	8. XR2W, XW2R	- Convert Robot to World, World to Robot coordintates
//	9. XW2E, XE2W	- Convert World to Efem, Efem to World coordintates
//	10. XMCD dir, distance	- Move cartesian direction from the current position
//		direction - 8(N) 2(S) 4(W) 6(E) 7(NW) 9(NE) 1(SW) 3 (SE)
//	11. scio.c 	- check for prealigner input G only if there's a track.
//	12. sctch.c	- Implement Teaching Smart-Motion 
//	13. sctch.c 	- fix a bug in Step/Jogging - amflag not updated. Increas step.
//
//		
//
//	Fuctions added:
//	ROConvertRobotToWorld() - convert (T, R) coordinate to (x, y) world coordinate
//	ROConvertWorldToRobot() - convert (x, y) to (T, R).
//	ROConvertWorldToEfem() - rotate (x,y) by Efem-Theta-Zero angle
//	ROConvertEfemToWorld() - reverse rotate (x,y) by Efem-Theta-Zero angle
//	ROGetMidPositions() - get middle positions on the straight line from the start pos
//		to final pos, with interval of 0.5". Any T position < start pos is 360-deg
//
// 1.24 - Process SECS interrupt for each indexer.
//	  SECA, SECB, SECC, SECD
//
// 1.23 - Modify SECS2 interrupt processing. It's called only with S6F13 ceid 1 & 2.
//	  POD remove & arrive only events.
//
// 1.22 - implement DERR drift detection interrupt macro.
// 1.21 - fix READ command to re-route the port number to the current command port
//
// 1.20 - Add a SECS command to clear the events. S6F13 99 will clear the event.
//
// 1.19 - Fix a bug in SAV command that didn't work for W (t) track axis
//	  The prealigner file is used for I2AXO, but in FIOWriteParam, prealigner params
//	  are forced to move to 5th place in array. This should not be done for I2AXO.
//
// 1.18 - In Teach Mode, remove software delay when pressing R & S. The delay was caused
//	   by Teach Terminal Displaying the axes information even if nothing was changed.
//	   Teach Terminal Display is updated only when axes are changed.
//
// 1.17 - In Teach Mode, increase Step Size, decrease wait time for key, and update 
//	  AMFlag when setting Spd, Acc, Dec.
//
// 1.16 - OTF in GETA takes 300ms delay because of sensor bitmap I/O acquisition
//	  OTF_IO is modified to reduce this delay just until it completes 6 IO data.
//
// 1.15 - fix otf computation, discards one or two computed center points if they are far
//	  apart from the average center
//
// 1.14 - fix otf computation, different points selected for center computation.
//
// 1.13 - fix checksum error during S1F6 processing
//
// 1.12 - fix 110x error during HOM, in SER.C checking for INX com port.
// 	- add Brake ON/Off for Track
//
// 1.11 - fixes In Position windows, WMO
//	- secs com, wait for EOT, send EOT, receive Message (checksum error), add time-delay
// 

#define MAXARRAYSIZE    8
#define MAXNUMSYSCFGS   31              /* max number of system configurations */


/********** VARIABLES **********/

/* structure contains the information for each configuration. */
typedef struct sys_cfg_st
{
    char m_caSysCfgType[10];
    int m_iNumOfAxes;
    int m_iDefineFlag;
    int m_iEmulator;
    int m_iaEquipeAxes[MAXARRAYSIZE];
    int m_iaGalilAxes[MAXARRAYSIZE];
    int m_iaMechType[MAXARRAYSIZE];
    int m_iaSpecialAxes[MAXARRAYSIZE];
    int m_iaDiagParms[3];
} stSysCfgs, *pstSysCfgs;

/********** FUNCTION PROTOTYPES **********/

/****************************************************************
 * Function:    InitVersionString
 * Abstract:    Initializes the version strings for both executable
 *      and library versions. The version string, e.g. I2BRT, is passed
 *      from main's initialization as is the executable filename,
 *      e.g. MAINNOFP.EXE.
 * Parameters:
 *      cpVerStrArg     (in) The version string, e.g. I2BRT
 *      cpFilenameArg   (in) The executable filename, e.g. MAINNOFP.EXE
 * Return:      SUCCESS or FAILURE
 ***************************************************************/
void InitVersionString(char *cpSysCfgArg, char *cpFilenameArg);
/****************************************************************
 * Function:    GetVersionString
 * Abstract:    Returns the version string
 * Returns:     The version string
 ***************************************************************/
char *GetVersionString();
/****************************************************************
 * Function:    GetFilename
 * Abstract:    Returns the executable filename
 * Returns:     The executable filename
 ***************************************************************/
char *GetFilename();
/****************************************************************
 * Function:    InitMechArrays
 * Abstract:    Sets up the mechanism initialization arrays to
 *      the correct values for the system version being run. The version
 *      string is retrieved from the configuration file and is set with
 *      password protection.
 * Parameters:
 *      ipNumOfAxes         (out) The number of axes in the system
 *      ipDefineFlagArg     (out) The define flags for the system
 *      ipEmulatorArg       (out) The emulator flags for the system
 *      ipEquipeAxesArg     (out) The Equipe axes designations in the system
 *      ipGalilAxesArg      (out) The corresponding Galil axes designations in the system
 *      ipMechTypeArg       (out) The The corresponding parameter files in the system
 *      ipSpecialAxesArg    (out) Any axes which are designated as special in the system
 *      ipDiagParmsArg      (out) The TRUE/FALSE values that are passed to the diagnostics module
 * Returns:     SUCCESS or FAILURE
 ***************************************************************/
int InitMechArrays(int *ipNumOfAxesArg, int *ipDefineFlagArg, int *ipEmulatorArg,
        int *ipEquipeAxesArg, int *ipGalilAxesArg, int *ipMechTypeArg,
        int *ipSpecialAxesArg, int *ipDiagParmsArg);
/****************************************************************
 * Function:    ValidateSysCfgString
 * Abstract:    Returns the array number corresponding to the system
 *      configuration string passed in.
 * Parameters:
 *      cpSysCfgStringArg   (in) The system configuration string
 * Returns:     Array number
 ***************************************************************/
int ValidateSysCfgString(char *cpSysCfgStringArg);

#endif
