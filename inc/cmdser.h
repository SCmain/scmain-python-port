/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdser.h
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

#ifndef _H_CMDSER_H
#define _H_CMDSER_H

/***************************************************************************
 * Function Name:   ex_AST
 * Description:     Asynchronous Communications Status Request.
 * Implemetation:   In This function how it work:
 *                  - Get the com port id from the instruction that passed in
 *                    from the function arguement.
 *                  - Get comm status of specified com port id by calling COGetCommStatus from the
 *                    comm module. It returns 16-bit status word of the comm.
 *                  - get the number bytes that has in the comm buffer now. If it returns -1, that's mean
 *                    there is no data in the comm buffer and set bits 11-15 to 0; otherwise set number of
 *                    bytes in the comm buffer and combine number of bytes in the buffer and 16-bit status word
 *                    into 32-bit status word.
 *                  - Then check if there is next operand, it must be an indirect R-register.  Convert the
 *                    32-bit status word into numeric and store into the R-register at specified index. Otherwise
 *                    just put out to the comm port.
 *                  - Then reset the comm status to 0 by calling COSetCommStatus(PortId,0) and clear error bit of
 *                    specified comm in system status word by calling SSSetStatusWord() for COM1 and COM2 or
 *                    SSetStatusWord2() for COM3.
 * NOTE:            This function will get executed when the user typed AST command
 *                  at the command line. The function is actually called in MRExecuteInstructions().
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *                  Command Syntax: AST <com port> [, [register]].
 *                      - com port: LONG_CONST or INDIRECT_REG that contains 1, 2, or 3.
 *                      - register: register to save status word, and it must be INDIRECT_REG.
 *                  The old firmware version, the com port id is as the following:
 *                      0 - COM1
 *                      1 - COM2
 *                      2 - COM3
 *                  Now, in CodeMerge98 (I2BRT)
 *                      1 - COM1
 *                      2 - COM2
 *                      3 - COM3
 * Note:            In the old firmware version, there is only 16-bit status word.
 *                  Now, in CodeMerge98 (I2BRT), there is 32-bit status word.
 *                  bit #0:     Device error
 *                  bit #1:     Receive time out error
 *                  bit #2:     Transmit time out error
 *                  bit #3:     Over-run error
 *                  bit #4:     Parity error
 *                  bit #5:     Framing error
 *                  bit #6:     Break
 *                  bit #7:     Blaise "INPUT_LOST" error
 *                  bit #8:     Blaise "ASYNCH_ERROR" error
 *                  bit #9:     Receiver Ready
 *                  bit #10:    Transmitter Ready
 *                  bit #11-15: not used for now (reserved for future SECSII implementation)
 *                  bit #16-31: used for number of character in the buffer.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_AST(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_BYPAS
 * Description:     By pass the interpreter and send directly from one comm port
 *                  to other comm port. You can only bypass COM2 or COM3.
 * Implementation:  Get the comm port id if there is an operand, then validate to make
 *                  sure it is only either COM2 or COM3. If there is no operand, then default
 *                  comm port is COM2.  Then call ROPortRedirect() to do direct communication
 *                  between two comm.
 * NOTE:            This function will get executed when the user typed BYPAS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: BYPAS <COMM PORT>
 *                      - COMM PORT:    only COM2 or COM3
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BYPAS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DCHN
 * Description:     This command can be used to send command or status to secondary port (COM2 or COM3)
 *                  daisy-chained with the controller.
 * Implementation:  Get the port id value from the first operand. If there is no port id, then get the
 *                  radix value (D/H) at the first operand. Then move to the next operand and validate
 *                  the operand. If it is NULL then return failure since you must have some
 *                  values to send to the comm port. If the operand is not NULL , then call
 *                  CMDdumpVariables() to send out the values to the comm port while there is still operand
 *                  in the operand list.
 * NOTE:            This function will get executed when the user typed DCHN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DCHN [port id],<radix>, <values to send>
 *                      - port id:          it is optional. If not specified, the default is COM3.
 *                      - radix:            either D(ec) or H(ex) which used for converting the numerical parameter to
 *                                          either HEX or DEC. And it has no effect on character, but the radix
 *                                          must be specified. Usually, the user will use with D.
 *                      - values to send:   values can be mix of character, string and numeric and it can be
 *                                          a constant value or indirect register.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DCHN(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_READ
 * Description:     Read string from COM buffer.  READ reads the whole buffer from comm port and
 *                  either store the whole string message in the string register if there is no delimiter(','),
 *                  or parse the message with delimiter (',') and store each part
 *                  of the message to appropriate register that specified in the arguements.
 * Implementation:  First get the Comm port id from the operand list and validate the comm port id to make
 *                  sure it is in the range of 1..3. Get the second operand which has to be the indirect_reg
 *                  type for storing the number of bytes have been read from the comm. Then check if
 *                  there is something in the comm buffer by calling CORxBuffAvail() from comm module.  If
 *                  there is something in the buffer then it will return number of bytes which is greater than
 *                  zero.  Then get everything from the comm buffer by specifying the number of bytes to
 *                  read as -1 in the function COGetsRxBuff().  Then get the length of message to store into
 *                  the register for return value. Now start at 3rd operand, while there is more operand in the
 *                  list parse the message by delimiter ',' and store each part of message according to
 *                  specified operand type. If get the message from the comm buffer failure, return -1 as number
 *                  of bytes have read.
 * NOTE:            This function will get executed when the user typed READ command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: READ <COMM PORT>, <NUMBER OF BYTE READ>, [STRING or NUMBER or CHAR],...,[STRING or NUMBER or CHAR]
 *                      COMM PORT:  comm port id (1,2 or 3) and it can be INDIRECT_REG or LONG_CONST.
 *                      NUMBER OF BYTE READ:    this will store the number of bytes have read successfully
 *                                              from the comm buffer. It can only be INDIRECT_REG.
 *                      The third operand up can be a string or numeric or character but it has to
 *                      be a INDIRECT register type.  The third operand up is usually used to parse the message
 *                      that you just read from the comm buffer in the way that you already knew the format of
 *                      the message a head of time.
 *                      Example:    if the message send to comm port is like this:
 *                                      sss, 555,aCR
 *                                  Then you can use read to get the message from the comm port and
 *                                  parse it at the same time by calling:
 *                                      READ 3, [R1], [S1], [R2], [C1]
 *                                  So READ will read 11 bytes including the CR from COM3, and R1 will
 *                                  get stored 11. S1 will have "sss", R2 will have 555, and C1 will
 *                                  have 'a'.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_READ(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_READC
 * Description:     Read the specified number of bytes from COM n.
 * Implementation:  There are two temporary buffer for holding the string that read from the comm buffer.
 *                  They need to allocate memory at beginning and deallocate memory before exiting the function.
 *                  Then get the first operand which is comm port id and validate the port id to make sure it is in the range
 *                  of 1 to 3. Get the second operand which is the number of bytes that wanted to read from the comm buffer.
 *                  Check if there is something in the comm buffer, then get string from the comm buffer with number of bytes
 *                  have specified in the second operand.  In the case of nothing in the comm buffer, then we set the number of
 *                  bytes have read to 0 and store to return register; otherwise set whatever number of bytes have read into
 *                  the return register.  Then looping while there is operand and store either character or string value into
 *                  return register(C-register or S-register type).
 * NOTE:            This function will get executed when the user typed READC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: READC <COMM PORT>, <# OF BYTES TO READ>, <# OF BYTES RETURN>, <RETURN VALUE>
 *                      - COMM PORT:            Comm port id (1, 2 or 3).  It can be INDIRECT_REG or LONG_CONST.
 *                      - #OF BYTES TO READ:    Stores number of characters that you want to read.
 *                                              It can be INDIRECT_REG or LONG_CONST.
 *                      - #OF BYTES RETURN:     Stores the return actual number of characters after
 *                                              read from the Comm buffer. It has to INDIRECT-REG.
 *                      - RETURN VALUE:         Stores return value of either string or a character.
 *                                              It can only be either INDIRECT_STR or INDIRECT_CHAR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_READC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SDCM
 * Description:     Set Daisy Chain Mode.  Enables or disables Daisy Chain Mode.  When on Daisy Chain
 *                  Mode any data from COM2 will not be interpreted by the controller.  Data will be stored
 *                  in a buffer until read by READC or REAC commands.  This also allows COM2 macro to handle
 *                  the data. The flag value is either 0(disable) or 1(enable).
 * Implementation:  Get the flag value from the operand and call COSetDCMode() to set the daisy chain
 *                  mode to be enabled or disabled.
 * NOTE:            This function will get executed when the user typed SDCM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SDCM <FLAG>
 *                      - FLAG:     enable or disable the daisy chain mode. It can be INDIRECT_REG or LONG_CONST
 *                                  operand type.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SDCM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SEOT
 * Description:     Set End of Transmission character. (only use for cybeq emulator)
 * Implementation:  First, get the mode. if 0 then call COSetEOTChar() to set the end of transmission character
 *                  is CR and ignore LF. If 1 then call COSetEOTChar() to set the end of transmission character
 *                  is LF and ignore CR; 1 is the cybeq emulator mode.
 * NOTE:            This function will get executed when the user typed SEOT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SEOT <MODE>
 *                      - MODE: It is either 0 or 1.
 *                              - 0:    for regular mode that has the CR
 *                              - 1:    for cybeq emulation mode that has only LF
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SEOT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DMPAS
 * Description:     This command provides a history list of all the commands/keys entered
 *                  on the selected comm port.
 * NOTE:            This function will get executed when the user typed DMPAS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DMPAS <COMM PORT>
 *                      - COMM PORT:    comm port id (1/2/3)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DMPAS(instr_ptr instr );

#endif
