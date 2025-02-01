/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdmap.c
 *
 * Functions:
 *
 * Description: Functions which act as an interface for the LowLevel
 *              commands. These functions are local to the low-level command module.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_CMDMAP_H
#define _H_CMDMAP_H

/***************************************************************************
 * Function Name:   ex_SLUX
 * Description:     Set the Lift Up XSlot threshold.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SLUX(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RLUX
 * Description:     Read the Lift Up XSlot Threshold.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RLUX(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_FILTR
 * Description:     Invoke Filter Ring Algorithm for scan data.
 * Implementation:  Get the station name and use the station name to get the scanner station structure. Get the
 *                  pass number from the second operand.  This pass number determine which scanner filter algorithm to
 *                  run. Get the calibration flag from the third operand and get the iteration number from the
 *                  fourth operand. Then start scanning with/without the calibration option and number of iteration.
 * NOTE:            This function will get executed when the user typed FILTR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: FILTR <STATION>, <PASS>, <CALIB FLAG>, <ITERATION>
 *                      - STATION:      scanner station name
 *                      - PASS:         pass number (0/1)
 *                      - CALIB FLAG:   calibration flag for calibrating or not calibrating.  In TSCN macro will specify
 *                                      with 1 to do calibration. However, for MAP or DMAP macro will specify with 0 for
 *                                      not doing the calibration.
 *                      - ITERATION:    number of iteration/pass to move up and down while scanning.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FILTR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_PSCAN
 * Description:     Prepare scan station. PSCAN prepares controller for capturing data from scanning sensor.
 *                  It sets up interrupt vector for high-speed Z-axis position capturing. Z-axis position is
 *                  captured on every transition of the scanning sensor. If the command is successful, it
 *                  returns 0. Otherwise 1. PSCAN fails if the scanning sensor is already activated (detects wafer).
 * Implementation:  First, get the station name from the first operand. Then search for station if it exists in the
 *                  scanner station list. If it is not, then create the scanner station structure and add to
 *                  scanner station list. Then, get the pass number from the second operand and get the used value
 *                  from the third operand which is always be 1. Then perform scanning and store the result(0/1) into
 *                  the register which is the fourth operand.
 * NOTE:            This function will get executed when the user typed PSCAN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: PSCAN <STATION>, <PASS>, <1>, <RETURN VALUE>
 *                      - STATION:      station name
 *                      - PASS:         Pass number. When scanning the robot scans the cassette in two passes,
 *                                      up and down.  This parameter must be 0 for the up pass and 1 for the
 *                                      down pass.
 *                      - 1:            This parameter must be always 1. It's not really used in the implementation
 *                                      but have to put as a parameter.
 *                      - RETURN VALUE: stores the return value after PSCAN executed.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_PSCAN(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_QSCAN
 * Description:     Quits Scanning mode.  QSCAN returns controller to normal operating
 *                  mode.  This includes disabling high-speed position capturing that
 *                  can slow down controller response in normal mode.
 * NOTE:            This function will get executed when the user typed QSCAN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: QSCAN
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_QSCAN(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RROP
 * Description:     Read slot Radial Offset Position for scanner Station.
 * Implementation:  Get the option/station name from the first operand, then get the station information.
 *                  Get the pass (first or second pass) from the second operand and get slot number from
 *                  the third operand. Then Get the data from scanner data structure. If there is the
 *                  fourth operand then store the return data into the register; otherwise, send to the
 *                  comm port.
 * NOTE:            This function will get executed when the user typed RROP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RROP <OPTION>, <PASS>, <SLOT> ,[RETURN VALUE]
 *                      - OPTION:   option to get which type of data from scanner data.
 *                                  The options are:
 *                                  - A:    Wafer thickness
 *                                  - B:    Wafer position
 *                                  - C:    Pitch
 *                                  - D:    Z-pass data
 *                                  - E:    Transition data (up or down)
 *                                  - F:    Number of hits.
 *                      - PASS:     first or second pass (0 or 1)
 *                      - SLOT:     slot number at based 0.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RROP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_LLOP
 * Description:     Load scan data into raw data buffer for testing.
 * Implementation:  First, get the index from first operand. Get pass1 from second operand. Get
 *                  transition1 from third operand. Get pass2 from fourth operand and get transition2 from
 *                  fifth operand. Then call the function MPLoadTempData() to download the data to the
 *                  scanner raw data structure in the controller.
 * NOTE:            This function will get executed when the user typed LLOP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: LLOP <INDEX>, <PASS1>, <TRANS1>, <PASS2>, <TRANS2>
 *                      - INDEX:    index to access specific location for raw data buffer.
 *                      - PASS1:    pass 1
 *                      - TRANS1:   transition 1
 *                      - PASS2:    pass 2
 *                      - TRANS2:   transition 2
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_LLOP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RBS
 * Description:     Read Base scan position for the station
 * NOTE:            This function will get executed when the user typed RBS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RBS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SBS
 * Description:     Set Base Scan Position for the station
 * NOTE:            This function will get executed when the user typed RCT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCT
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SBS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RZP
 * Description:     Reads Z-axis position of wafer edge.
 * Implementation:  First, get station name from first operand and get scanner information by using the station name.
 *                  Then get edge position from the second operand and get the slot from the third number.  Get the
 *                  Z-position of scanner station at specified slot. If there is fourth operand, then set the Z-position
 *                  to register; otherwise, send to the comm port.
 * NOTE:            This function will get executed when the user typed RZP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RZP <STATION>, <POSITION>, <SLOT>, [RETURN VALUE]
 *                      - STATION:      Station name
 *                      - POSITION:     Edge position. 0 for bottom edge and 1 for top edge.
 *                      - SLOT:         Slot number (zero-based)
 *                      - RETURN VALUE: Optional parameter to store return value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RZP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_POTF
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_POTF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_QOTF
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_QOTF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SOTFI
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SOTFI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ROTF
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ROTFI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SOTFD
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SOTFD(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ROTFD
 * Description:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ROTFD(instr_ptr instr);

#endif
