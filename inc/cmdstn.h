/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdstn.h
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

#ifndef _H_CMDSTN_H
#define _H_CMDSTN_H

/***************************************************************************
 * Function Name:   ex_CLRC
 * Description:     Clear Coordinate.
 * Implemetation:   First get the customized home position from the galil by calling
 *                  MEGetCustomizedHome() from Mechanism module. If the there is station name specified,
 *                  then clear only that specific station otherwise clear all 52 stations. Clear coordinate by
 *                  setting the coordinates position to deafult values of customized home which are:
 *                      T = T from customized home.
 *                      R = R from customized home.
 *                      Z = Z from customized home.
 *                      Station retract position = R from customized home.
 * NOTE:            This function will get executed when the user typed CLRC command
 *                  at the command line. The function is actually called in MRExecuteInstructions().
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *                  Command Syntax: CLRC [station name]
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CLRC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_INDEX
 * Description:     Set the Index value for a station.  This command is used when a move to
 *                  coordinate command is issued.  Index number specifies the position of Z-axis relative
 *                  to the taught coordinate Z-axis position.
 * Implementation:  First, call CMDgetStationParameters() which will get the station name and index.  The index
 *                  return in the first element in the array that passed as reference and the function will return
 *                  the station name.  Then call FIOSetStnZIndex() from datafile module to set the position of
 *                  Z-index.  The purpose is to use this position to move to the specific slot of cassette to
 *                  do a get or put in the macro.
 * RLG NOTE:        always Zindex
 * NOTE:            This function will get executed when the user typed INDEX command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: INDEX <STATION>, <SLOT>
 *                      - STATION:  name of station which is CHARACTER_CONST or INDIRECT_CHAR type.
 *                      - SLOT:     the slot number of the cassette to specify the Z-position relative to
 *                                  the taught station of Z-axis position. It can be LONG_CONSTR or
 *                                  INDIRECT_REG type.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_INDEX(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_PITCH
 * Description:     Set the Pitch value for a station.  Pitch is the distance of on Z-axis between two adjacent indices.
 *                  This command together with INDEX and MTCR or MTCS will determine position of Z-axis.
 *                  The Unit is in 0.0001".
 * Implementation:  Call CMDgetStationParameters() to get the station name and pitch value. CMDgetStationParameters will
 *                  get values from operands and return the station name and pitch value into first element in an array.
 *                  Then call FIOSetStnPitch() to set pitch value to specified station name.
 * NOTE:            This function will get executed when the user typed PITCH command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: PITCH <STATION>,<VALUE>
 *                      - STATION:  the name of station and it can be in INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - VALUE:    It is new value to set. It can be INDIRECT_REG or LONG_CONST type.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_PITCH(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RIND
 * Description:     Read an index value for a station that the last set by INDEX command.
 * Implementation:  First, check that there must be at least one operand for this command. So get the first
 *                  operand which is the station name, then use the station name to get the station index
 *                  by calling CMDgetStnIndx(). And use station index to get Z-index which is the value we
 *                  want to get for this command by calling FIOGetStnZindex(). Then call
 *                  CMDreadBackParams() to determine either storing the value into the return register if the
 *                  second operand existed, or sending the value to comm port.
 * NOTE:            This function will get executed when the user typed RIND command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RIND <STATION>, [STORED VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - STORED VALUE:     This is optional parameter. Only existence if you want
 *                                          to store the result.  It has to be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RIND(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCSI
 * Descripton:      Read Coordinate Special Item of specified station.
 * Implementation:  First, get station name from the first operand and using station name to
 *                  get the station location by call CMDgetStnIndx(). Get the second operand which is
 *                  the item number to read the value from.  Get value from specified item (please look
 *                  at the code belove for more detail implementation information). Then determine the return
 *                  value either send to comm port or store into the return register.
 * NOTE:            This function will get executed when the user typed RCSI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCSI <STATION>, <INDEX>, [RETURN VALUE]
 *                      - STATION:      Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - INDEX:        specific item. It can be INDIRECT_REG or LONG_CONST.
 *                      - RETURN VALUE: Stores the return value and it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCSI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCS
 * Description:     Read Coordinate setting
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  station coordinates (T,R,Z) by calling FIOGetStnCoord(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCS <STATION>, [VALUE1, VALUE2, VALUE3]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the T-axis position of specified station
 *                      - VALUE2:   Stores the R-axis position of specified station
 *                      - VALUE3:   Stores the Z-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RFC
 * Description:     Read Flipper Coordinates
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  station coordinates by calling FIOGetFprCoord(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RFC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RFC <STATION>, [VALUE1]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the Fpr-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RSRP
 * Description:     Read Safe R Position
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  Safe R Position by calling FIOGetSafeR(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RSRP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSRP <STATION>, [VALUE1]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the W-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSRP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ROF
 * Description:     Read offset value for the station
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnOffset() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed ROF command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ROF <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the offset value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ROF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RPI
 * Description:     Read Pitch Value for the station that set by PITCH command.
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnPitch() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RPI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RPI <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the pitch value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RRET
 * Description:     Read retract value for the station that set by SRET command
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnRHome() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RRET command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RRET <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the retract position value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RRET(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RSE
 * Description:     Read the Second end effector flag for the station. If there is the second arguement specified,
 *                  the result of the reading is stored in the numerical variable.  Together with OUT commnad, this
 *                  command chooses the right vacuum line if double paddle end-effect is used.
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnEEindex() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RSEE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSEE <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the flag value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSE(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RST
 * Description:     Read the Stroke value for the station.
 * Implementation:  First get the station name and use station name to get the station index by calling
 *                  CMDgetStnIndx().  Then use the station index to get the station offset by calling
 *                  FIOGetStnStroke() and if there is a second operand, then set the offset value into the register,
 *                  otherwise, send the offset value to comm port.
 * NOTE:            This function will get executed when the user typed RST command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RST <STATION>, [RETURN VALUE]
 *                      - STATION:          Station name. It can be INDIRECT_CHAR or CHARACTER_CONST type.
 *                      - RETURN VALUE:     It is optional argument to store the stroke value if don't want to send
 *                                          to the comm port. If it exists, it must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RST(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCSI
 * Description:     Set coordinate special item. For scanning, items 0-7 are used. Each station
 *                  has 16 items as properties to store value.
 * NOTE:            This function will get executed when the user typed SCSI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SCSI <STATION>, <ITEM>, <VALUE>
 *                      - STATION:  Station name
 *                      - ITEM:     item number or index (0..15)
 *                      - VALUE:    the value to be set.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 *  Set Position coordinates for a station
 ***************************************************************************/
int ex_SCSI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SOF
 * Description:     Set the offset values for the station
 * Implementation:  Get offset of the cassette and station name from operand, then set the offset into
 *                  specified station name.
 * NOTE:            This function will get executed when the user typed SOF command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SOF <STATION>, <VALUE>
 *                      - STATION:  station name
 *                      - VALUE:    the offset value to be set.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SOF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SPO
 * Description:     Set position coordinates for a station. All 3 positions of T,R and Z must be
 *                  provided. Can't set one axis at a time.
 * NOTE:            This function will get executed when the user typed SPC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SPC <STATION>, <POS1>, <POS2>, <POS3>
 *                      - STATION:  Station name
 *                      - POS:      Position of each axis T,R, and Z to be set.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SPO(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SFC
 * Description:     Set Flipper Coordinate for a station.
 * *NOTE:           This function will get executed when the user types SFC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SFC <STATION>, <POS>
 *                      - STATION:  Station name
 *                      - POS:      Position of Flipper axis W to be set.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SFC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SSRP
 * Description:     Set Safe R Position
 * *NOTE:           This function will get executed when the user types SSRP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SSRP <STATION>, <POS>
 *                      - STATION:  Station name
 *                      - POS:      Position of R-Axis when Safe to Flip.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSRP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SRET
 * Description:     Set Retract Value for Station by specified that station name and position
 *                  to be retract position. The position must be in the rangle of software negative and
 *                  position limit switch. If there is no parameter specified at all, the default is set all
 *                  stations' retract position to be same as customized home retract position.
 * NOTE:            This function will get executed when the user typed SRET command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SRET <STATION>, <VALUE>
 *                      - STATION:  Station name
 *                      - VALUE:    postion to set be set as station retract position.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SRET(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SSE
 * Description:     Set Second End Effector Flag for the station
 * NOTE:            This function will get executed when the user typed SSEE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SSEE <STATION>, <FLAG>
 *                      - STATION:  Station name
 *                      - FLAG:     0/1 to determine which end-effect. O is for default end-effect.
 *                                  1 is for second end-effect or dual arm end-effector.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSE(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SST
 * Description:     Set Stroke Value for the Station
 * NOTE:            This function will get executed when the user typed SST command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SST <STATION>, <VALUE>
 *                      - STATION:  Station name
 *                      - VALUE:    Stroke value to be set
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SST(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RSCS
 * Description:     Read scanning coordinate settings of a station
 * Implementation:  First get the first operand which is the station name, and use the station name
 *                  to get station index by calling CMDgetStnIndx(). Then using the station index to get
 *                  station coordinates (T,R,Z) by calling FIOGetStnScanCoord(). Then call CMDgetBackparms() to
 *                  determine the return values is either storing into the return registers or sending to
 *                  the comm port.
 * NOTE:            This function will get executed when the user typed RSCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSCS <STATION>, [VALUE1, VALUE2, VALUE3]
 *                      - STATION:  Station name. It can be INDIRECT_CHAR or CHARACTER_CONST.
 *                      - VALUE1:   Stores the T-axis position of specified station
 *                      - VALUE2:   Stores the R-axis position of specified station
 *                      - VALUE3:   Stores the Z-axis position of specified station
 *                      These values must be INDIRECT_REG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSCS(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_SPSC
 * Description:     Set the scanning position coordinates for a station. All 3 position values of
 *                  T,R and Z need to provide at once.
 * NOTE:            This function will get executed when the user typed SPSC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SPSC <STATION>, <POS1>, <POS2>, <POS3>
 *                      - STATION:  Station name
 *                      - POS:      positions to be set as taught positions for scanning station.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SPSC(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_SSS
 * Description:     Set Slot Status for the station.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSS(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_RSS
 * Description:     Read Slot Status for the station. This command reads status of the result
 *                  generated by FILTR command. The status of each slot as following:
 *                      - 0:    no wafer
 *                      - 1:    wafer present
 *                      - 2:    cross-slotted.
 *                      - 3:    scanner failed.
 *                      - 4:    double-slotted.
 * Implementation:  Get the station name and check to make sure the station exists and get the
 *                  scanner station information. Get the pass number and slot number. Then using all these
 *                  information to get the slot status. If there is an operand, then store the slot status
 *                  into register otherwise send to the comm port.
 * NOTE:            This function will get executed when the user typed RSS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSS <STATION>, <PASS>, <SLOT>,[REUTNR VALUE]
 *                      - STATION:      station name
 *                      - PASS:         pass number (0/1)
 *                      - SLOT:         slot number of cassette
 *                      - RETURN VALUE: optional parameter to store return value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSS(instr_ptr instr );

#endif
