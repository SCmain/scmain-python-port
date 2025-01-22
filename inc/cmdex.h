/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdex.c
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

#ifndef _H_CMDEXEC_H
#define _H_CMDEXEC_H

/***************************************************************************
 * Function Name:   ex_FRMT
 * Description:     Format NVSRAM which the read and write disk.
 * Implementaton:   First, we call the function call FORM_MAIN(0,1) from purchased library, it does the format the
 *                  NVSram and return the status word, then send the status word to command port. Check status
 *                  is not 6 then the format is done and return sucess. Otherwise, close all initialized devices and
 *                  restores all the vector as necessary and call FORM_MAIN(1,1) to cause the reboot and format.
 * NOTE:            This function will get executed when the user typed FRMT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: FRMT 313
 *                      - 313 always needs to format the NVSram.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FRMT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DUMPP
 * Description:     Dump Parameters data from robot, prealigner and auxaulary file.
 *                  Parameters are sent in ASCII presentation. The parameter is showed
 *                  in order as show in the "Software Manual". Please refer to "software
 *                  manual" for more information. They have the fix length and number of lines
 *                  There is 25 lines in every parameter files (execept the old version of controller
 *                  there is 19 lines). The end of each line doesn't include the CR and LF. This
 *                  command will wait for a CR from host after sending each line.
 * Implementation:  First, allocate the memory for temparary buffer to store information to dump to
 *                  the comm port. Then get type of parameter file from operand. Then dump the parameter
 *                  data depend on the type of parameter file. Each line parameter data send to the comm
 *                  port will wait for CR from host before send the next line.  The function that send
 *                  data to comm port and wait unitl the CR is CMDdumpBuf. However, you see in the code
 *                  there is also CMDdumpLparms function and this function combine the data with commas
 *                  and call CMDdumpBuf to send the data and wait for CR. At the end send the CTRL_Z to
 *                  end of the file.
 * NOTE:            This function will get executed when the user typed DUMPP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DUMPP [parameter file type]
 *                      - parameter file type: the value should be 0,1, or 2
 *                        0 or nothing(default):    for robot parameter file
 *                        1:                        for prealigner parameter file
 *                        2:                        for auxaulary parameter file such as track, flipper, dual arm.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DUMPS
 * Description:     Dump Station coordinates. This command will send all coordinates
 *                  information to the comm port. This command is the same as DUMPP that waits for a
 *                  CR from the host.
 * Implementation:  Get customized home from the galil by call MEGetCustomizedHome() to use
 *                  the value to check against with coordinate position for sending or not sending
 *                  to the comm port. So if coordinate position is not equal to customized home, then
 *                  send the coordinate position to the comm, ohterwise skip it.  The coordinate position of
 *                  T,R,Z will be send to the comm and wait for CR from the host before sending another
 *                  coordinate postions.  This will go through the loop from 0 to 51 stations.
 *                  Where the indices(stations) are as following:
 *                      A-Z (ASCII 65- 90) have indices  0-25 respectively
 *                      a-z (ASCII 97-122) have indices 26-51 respectively
 * NOTE:            This function will get executed when the user typed DUMPS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DUMPS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_INPUT
 * Description:     Reads value of port number specified by the parameter.
 *                  This command reads the value of input and output port.  The value read
 *                  is sent to the host or stored in numeric register if it is specified.
 * Implementation:  Get the port name or bit number from an operand.  Then call CMDreadPort() to
 *                  read the port value and there is second operand, then store the port value into
 *                  indirect register type; otherwise, just send the port value to the comm port.
 * NOTE:            This function will get executed when the user typed INPUT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: INPUT <PORT or BIT OF SPECIFIC PORT>, [STORE VALUE]
 *                      - PORT:     when the user wanted get information of whole port by specified the name of port.
 *                      - BIT OF SEPCIFIC PORT: providing the specific bit number of the port.
 *                  NOTE: parameter can be character type or numeric type. If specified the whole port, then parameter
 *                        is character type or INDIRECT_CHAR which is the name of the port. If specified the specific
 *                        bit number, then parameter is numberic type or INDIRECT_REG.
 *                      - STORE VALUE:  it is optional parameter. If it existed, then value of the port will be stored
 *                                      in the variable which must be INDIRECT_REG type.
 *                      PLEASE LOOK AT "SOFTWARE MANUAL" IN I/O MAP SECTION FOR MORE DETAIL.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_INPUT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_OUTP
 * Description:     Write value to an output port. If the port number is in input mode
 *                  then commnad is ignored.
 * Implementation:  Get the port name or bit number from the first operand and get the value
 *                  to write to port from second operand. If port is A or L, then call
 *                  MEWrtieGalilPort() to write value to galil output. If port is B,C,D,E, then
 *                  call IOWriteIO() to write the value to output port.
 * NOTE:            This function will get executed when the user typed OUTP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: OUTP <PORT or BIT OF SPECIFIC PORT>, <VALUE>
 *                      - PORT:     when the user wanted set information of whole port by specified the name of port.
 *                                  The output port can be either (A,B,C,D,E,L) or numeric (0..47).
 *                      - BIT OF SEPCIFIC PORT: providing the specific bit number of the port to write to.
 *                  NOTE: parameter can be character type or numeric type. If specified the whole port, then parameter
 *                        is character type or INDIRECT_CHAR which is the name of the port. If specified the specific
 *                        bit number, then parameter is numberic type or INDIRECT_REG.
 *                      - VALUE:  the new value to write to output port.
 *                      PLEASE LOOK AT "SOFTWARE MANUAL" IN I/O MAP SECTION FOR MORE DETAIL.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_OUTP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMCR
 * Description:     Read macros from NVSRAM.  RMCR deletes curently active macro set and loads macro set from
 *                  NVSram.  The optional numerical parameter indicates which macro set to load. Valid
 *                  parameter is 0-99 then saved by SMCR command.
 * Implementation:  If there is an operand then get the file id value. If there is no operand, then set the
 *                  file id to default which is 0.  Then call MRLoadMacroFromNVsram() to load the macro set
 *                  from the specified file id or default file id in the NVSram to the buffer. This macro set will
 *                  be the current set of macro for executing.
 * NOTE:            This function will get executed when the user typed RMCR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RMCR [FILE ID]
 *                      - FILE ID:  specific file id to load from NVSram since there can be 100 files from NVSram.
 *                                  The file id is in the rangle of 0-99.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMCR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RNCS
 * Description:     Read NVSRAM checksum status. There is 16-bit status word which are:
 *                      - bit#0:    Check-sum error on robot parameter file.
 *                      - bit#1:    Check-sum error on coordinate file.
 *                      - bit#2:    Check-sum error on macro file.
 *                      - bit#3:    Check-sum error on pre-aligner parameter file.
 *                      - bit#4:    Check-sum error on pre-aligner wafer file.
 *                      - bit#5:    Check-sum error on pre-aligner calibration file.
 *                      - bit#6:    check-sum error on auxaulary file.
 *                      - bit#7-14: not used (always 0)
 *                      - bit#15:   NVSRam is not formated or does not exist.
 * Implementation:  If there is an operand, then make sure operand type is INDIRECT_REG. Then
 *                  get the check-sum status from Status module by call SSGetCheckSumStatus() and set the
 *                  status into the return register.  If there is no operand, then get the check-sum status
 *                  and send the status to comm port.
 * NOTE:            This function will get executed when the user typed RNCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RNCS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RNCS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SAV
 * Description:     Save the parameter and coordinate file to NVSRAM.  all info gotten from galil
 *                  and written to the parameter files in NVSRam.
 *                  The command will get the following values from galil and save into the NVSRam:
 *                  SP, AC, DC, ER, KP, KD, KI, FA, TL, and RM.
 * NOTE:            This function will get executed when the user typed SAV command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SAV
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAV(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMCR
 * Description:     Save macros to NVSRAM.  All macros that have been defined successfully are saved
 *                  to NVSram.  The command will save the macor in the binary form.  If optional
 *                  numerical parameter is specified, macro will use it as file ID. Up to 100 (0-99) file IDs
 *                  are allowed.  These IDs identify macro sets.  If ID is not specified, SMCR will use ID 0.
 *                  In power cycle, macro set wiht ID 0 is automatically loaded from NVSram to DRAM as the
 *                  default macro set.
 * Implementation:  if there is an operand, then save macros to the specified file id that get from the
 *                  operand. Otherwise, save macros to default file id which is 0. To save macros by calling
 *                  the function MASaveMacrotoNvsram().
 * NOTE:            This function will get executed when the user typed SMCR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SMCR [FILE ID]
 *                      - FILE ID:  represent the id of the file to save to and there are 100 file ids (0-99).
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMCR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_WRIP
 * Description:     Write parameters to controller. This command writes directly
 *                  to NVSRam and the effect is apparent immediately. The data is
 *                  sending line by line in the order same the DUMPP. The controller
 *                  will return back CR each line had sent down. Please look at
 *                  "Software Manual" for the order of the data.
 * Implementation:  Get file type from first operand. Then get each line of data from comm port
 *                  in the order that expected(look in "software manual" at DUMPP for the order
 *                  of parameter file. After getting each line, call function from Datafile module
 *                  to do the setting and send back to comm port "CR" before getting another line.
 *                  After getting all the data from the file then call ROInitGalil() to set some of
 *                  data to galil and write the data to NVSRam.
 *
 * NOTE:            This function will get executed when the user typed WRIP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: WRIP <FILE TYPE>
 *                      - FILE TYPE:    flag to tell to write with type of file to the controller.
 *                                      - 0 or default: robot parameter file
 *                                      - 1:            prealigner file.
 *                                      - 2:            auxaulary file (track, dual aram or fliper)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_WRIS
 * Description:     Write Stations to controller and overwritting existing coordinate setup.
 *                  WRIS will write directly to NVSRam. For each station sent to controller will
 *                  send back a CR.
 * Implementation:  while not end of file (ctrl_z), get the station information from the comm port,
 *                  parse out the space and blank line and check for CTRL_Z to end the writing and
 *                  check for invalid station name (a-z or A-Z). Then convert the station name to
 *                  number by call CMDstationNumber().  Then parse the coordinate value and set
 *                  the data into the buffer by call FIOSetStnCoord() and send back to comm port a
 *                  CR. After getting out from the loop, write the coordinate to memory by
 *                  call FIOWriteCoord to write to NVSram.
 *
 * NOTE:            This function will get executed when the user typed WRIS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: WRIS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_EQUAL
 * Description:     Math assignment statements, e.g. R10 = R11 + R12.
 *                  This is called when the instruction has "=".
 * Implementation:  Get the index of register for storing result from first operand and it
 *                  must be indirect register type. It can be INDIRECT_REG, INDIRECT_STR or
 *                  INDIRECT_CHAR. To know which type of register, we set the flag to whatever
 *                  type of the operand. However, for the INDIRECT_STR, call CMDstringEqual() to
 *                  do the string manipulation and exit the function. if not INDIRECT_STR,
 *                  then get the value of next operand and set the default operator
 *                  to '+', then check for which kind of operator and do the math calculation.  The
 *                  get math operator and forward to next operand. This will continue doing while the
 *                  operand is not NULL. After finish calculation and exit from the loop, then
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EQUAL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_QUERY
 * Description:     QUERY is the "?" command for sending the value to active port. This command
 *                  can be used to send user-generated status or information to host computer.
 *                  The first parameter (D/H) indicates radix used for converting numerical parameter
 *                  (either D(ec) or H(ex)).  It has not effect on character, but must be specified. Also
 *                  It is allowed to have mixed type variables.
 * NOTE:            This function will get executed when the user typed ? command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ?, D/H, <VARIABLE or CONST>
 *                      - D/H:      it is radix for either decimal or hexidecimal.
 *                      - VARIABLE or CONST:    the value to send to the comm port can be stored in the variables or
 *                                              a constant.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_QUERY(instr_ptr instr);
/********************************************************************************************
 * Function Name:   ex_GALCO
 * Description:     routine for OC number GALCO, which is a command line beginning with '\',
 *                  takes 1. operand from instruction structure, which is a Galil command string,
 *                  put it together and sends to Galil Card, the response received is either
 *                  printed out to the screen or stored into the registers
 * Parameters:      instr - instruction pointer
 *
 * Returns:         SUCCESS/FAILURE
 *********************************************************************************************/
int ex_GALCO(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_EXINP
 * Description:     get the value of specific port by using the direct address of I/O card.
 * NOTE:            This function will get executed when the user typed EXINP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EXINP <ADDRESS>, [RETURN VALUE]
 *                      - ADDRESS:      direct address at I/O card.
 *                      - RETURN VALUE: stores the return value of port.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EXINP(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_EXOUT
 * Description:
 * NOTE:            This function will get executed when the user typed EXOUT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EXOUT <ADDRESS>, <VALUE>
 *                      - ADDRESS:  direct address at I/O card
 *                      - VALUE:    value to be set to I/O port in the I/O card.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EXOUT(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_RESC
 * Description:     Restore coordinates from those contained in NVSRAM and use them as current
 *                  coordinate setup.
 * Implementation:  The command has no operand.  Call FIOReadCoord() to restore the coordinates from
 *                  NVSRam
 * NOTE:            This function will get executed when the user typed RESC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RESC
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RESC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RPAR
 * Description:     Restore parameter file from those contained in NVSRAM and use them as current
 *                  coordinate setup.
 * Implementation:  The command has no operand.  Call FIOReadParam() to restore the parameter
 *                  file from NVSRam. Then call ROInitGalil to restore those values to the Galil card.
 * NOTE:            This function will get executed when the user typed RPAR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RPAR [parameter file type]
 *                      - parameter file type: the value should be 0,1, or 2
 *                        0 or nothing(default):    for robot parameter file
 *                        1:                        for prealigner parameter file
 *                        2:                        for auxaulary parameter file such as track, flipper, dual arm.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPAR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMOD
 * Description:     Command sets the modulo, the position of Theta axis when the enoceder gets reset
 *                  to zero, usually 360 deg, 1 296 000 encoder counts
 *                  Command Syntax: SMOD <NUMBER OR REGISTER>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMOD(instr_ptr instr);
int ex_RMOD(instr_ptr instr);
/****************************************************************
 * Function Name:   ex_SCM
 * Description:     Set commanded motion variable
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_SCM(instr_ptr instr);
/****************************************************************
 * Function Name:   ex_RCM
 * Description:     Read commanded mnotion flag
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_RCM(instr_ptr instr);
/****************************************************************
 * Function Name:   ex_RHLN
 * Description:     Reads revision number of a given system file.
 * Command Syntax:  RHLN n<, [Sn]>
 *                  n = 0 robot par, *.par
 *                      1 prealigner par, *.par
 *                      2 auxiliary par, *.par
 *                      3 coordinate file, *.cor
 *                      4 calibration file, *.cal
 *                      5 wafer parameter, *.waf
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_RHLN(instr_ptr instr);
/****************************************************************
 * Function Name:   ex_SHLN
 * Description:     Sets revision number of a given system file.
 * Command Syntax:  SHLN n, [Sn] | "1.0"
 *                  n = 0 robot par, *.par
 *                      1 prealigner par, *.par
 *                      2 auxiliary par, *.par
 *                      3 coordinate file, *.cor
 *                      4 calibration file, *.cal
 *                      5 wafer parameter, *.waf
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_SHLN(instr_ptr instr);

//
// Station Output Timing for vacuum
//
int ex_XOSN(instr_ptr instr);
int ex_XROS(instr_ptr instr);
int ex_XROC(instr_ptr instr);
int ex_XRSI(instr_ptr instr);
int ex_XWIO(instr_ptr instr);
int ex_XRIO(instr_ptr instr);

#endif
