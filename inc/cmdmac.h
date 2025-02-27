/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdmac.h
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

#ifndef _H_CMDMAC_H
#define _H_CMDMAC_H

/***************************************************************************
 * Function Name:   ex_CMP
 * Description:     Compare instructions.
 *                  First, parse and store the values from the instruction pointer depending
 *                  on the operand type.  Make sure to check the operand is not NULL before
 *                  try to getting the value in an operand. Then call RGCompareStrStr() for comparing
 *                  string or RGCompareNumNum() for comparing character or long number.
 *                  RGCompareNumNum and RGCompareStrStr are from Register(RCS) module.
 * NOTE:            This function will get executed when the user typed CMP command
 *                  at the command line. The function is actually called in MRExecuteInstructions().
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: CMP <value1>,<value2>
 *                      <value1> and <value2>: can be STRING_CONST, CHAR_CONST, LONG_CONST,
 *                                             BR_CONST, INDIRECT_STR, INDIRECT_CHAR, INDIRECT_REG.
 *                      Remember these value1 and value2 have to have same type for comparing string.
 *                      However, for character and long number they can be combination since they are
 *                      treated as long number for comparison.  Aslo, this command is only work inside
 *                      macro.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CMP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ENDM
 * Description:     End of Macro.  This indicates the ending of macro definition. When defining
 *                  macro, controller will be set to macro-defining mode which is terminated by ENDM N
 *                  command.
 * Implementation:  First, get the mode of end macro and validate the mode to make it is in the range
 *                  of 0..3 if there is a mode, otherwise the default mode is 0. Then call MREndMacro() from
 *                  macro module and it will do all clear up, set up for next execution if any and also determine
 *                  how to terminate the macro. Please look at the MREndMacro() source code for more detail of
 *                  how ENDM does.
 * NOTE:            The PC of the instruction just after the call to the macro
 *                  is now on top of the stack.
 * NOTE:            This function will get executed when the user typed ENDM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ENDM [mode]
 *                      - mode:     This value to determine the termination of the defined macro at the end
 *                                  of the macro execution.
 *                                  The mode value as following:
 *                                  - 0 or nothing:     standard (non-interrupt) macro - no status sent
 *                                  - 1:                Standard (non-interrupt) macro -status sent
 *                                  - 2:                Exits from interrupted macro, and returns to the interrupted
 *                                                      macro if there is any macro executed.
 *                                  - 3:                Clears macro stack and sets controller to idle - controller
 *                                                      will not execute interrupted macro.
 *                         Note:    if mode is 1 and IAM is set with some mode, then the status is sent depend on
 *                                  IAM mode. Look at IAM for more detail about how the status is sent.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ENDM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TSKK
 * Description:     Kill the task. This command is for multi-tasking only. TSKK kills a task with a given
 *                  task id. If task id is not specified, TSKK will kill all tasks in the task list.
 * NOTE:            This function will get executed when the user typed TSKK command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TSKK [ID]
 *                      - ID:    Task id to specify for killing. The id is in the range 0..9.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKK(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TSKL
 * Description:     List Active Tasks. This command is for multi-tasking only. It lists the status of all the active
 *                  task. The status of the task includes a tas id and the line number that task is currently running at.
 * NOTE:            This function will get executed when the user typed TSKL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TSKL
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TSKM
 * Description:     Makes task.  If no task number specified makes a task on the first avaialable task
 *                  list, otherwise on the specified task list. A task creation should typically be paired
 *                  wiht a task termination which is TSKK command.
 * Implementation:  If there is an operand, then get the task id which will be the new task. Ohterwise, it will
 *                  search in the task list for the first available (unused) task id starting from 0..9 and create
 *                  a new task using that usused task id.  To create a new task by calling TKMakeTask() which will
 *                  create the task and check if there is a current task, then continue running the current task. If there
 *                  is no current task, then set the new task to be the current task.
 * NOTE:            This function will get executed when the user typed TSKM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TSKM [ID]
 *                      - ID:   Task id to specify for making the new task. Task id is in the range of 0..9.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TSKS
 * Description:     Signals awaiting task. THIS COMMAND IS NOT USED SINCE THE
 *                  IMPLEMENTATION IS INCORRECT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TSKW
 * Description:     Waits task. THIS COMMAND IS NOT USED SINCE THE
 *                  IMPLEMENTATION IS INCORRECT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKW(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TSKY
 * Description:     Yields currently executing task to task specified immediately.
 *                  THIS COMMAND IS NOT USED SINCE THE IMPLEMENTATION IS INCORRECT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKY(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JUMP
 * Description:     Unconditional jump which jump to next instruction that specified.
 * Implementation:  Get the label from an operand and it must be value or else return failure.
 *                  Then get the macro instruction pointer from the operand and setup the current
 *                  PC (program counter) to be the new macro instructioin pointer for executing by the
 *                  time it goes back to the mainloop.
 * NOTE:            This function will get executed when the user typed JUMP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JUMP <label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JUMP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JPG
 * Description:     Conditional jump. Jump if Comparison register is greater than 0.
 *                  The conditional jump commands checks the condition register (internal)
 *                  and assign PC(program counter) to the address where teh label is defined.
 * Implementation:  Get the carrier flag that is controlled by the macro module. If the value of the flag
 *                  is greater than 0, then call JUMP command to jump to instruction with having the specified
 *                  label; otherwise continue the next instruction in the macro.
 * NOTE:            This function will get executed when the user typed JPG command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JPG <label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JPG(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JPGE
 * Description:     Conditional jump. Jump if Comparison register greater than or equal to 0.
 *                  The conditional jump commands checks the condition register (internal)
 *                  and assign PC(program counter) to the address where teh label is defined.
 * Implementation:  Get the carrier flag that is controlled by the macro module. If the value of the flag
 *                  is greater than or equal to 0, then call JUMP command to jump to instruction with having the specified
 *                  label; otherwise continue the next instruction in the macro.
 * NOTE:            This function will get executed when the user typed JPGE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JPGE<label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JPGE(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JPL
 * Description:     Conditional jump. Jump if comparison register less than 0.
 *                  The conditional jump commands checks the condition register (internal)
 *                  and assign PC(program counter) to the address where teh label is defined.
 * Implementation:  Get the carrier flag that is controlled by the macro module. If the value of the flag
 *                  is less than 0, then call JUMP command to jump to instruction with having the specified
 *                  label; otherwise continue the next instruction in the macro.
 * NOTE:            This function will get executed when the user typed JPL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JPL<label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JPL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JPLE
 * Description:     Conditional jump. Jump if Comparison register less than or equal to 0.
 *                  The conditional jump commands checks the condition register (internal)
 *                  and assign PC(program counter) to the address where teh label is defined.
 * Implementation:  Get the carrier flag that is controlled by the macro module. If the value of the flag
 *                  is less than or equal to 0, then call JUMP command to jump to instruction with having the specified
 *                  label; otherwise continue the next instruction in the macro.
 * NOTE:            This function will get executed when the user typed JPLE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JPLE<label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JPLE(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JPZ
 * Description:     Conditional jump. Jump if the comparison register equals 0.
 *                  The conditional jump commands checks the condition register (internal)
 *                  and assign PC(program counter) to the address where teh label is defined.
 * Implementation:  Get the carrier flag that is controlled by the macro module. If the value of the flag
 *                  is equal to 0, then call JUMP command to jump to instruction with having the specified
 *                  label; otherwise continue the next instruction in the macro.
 * NOTE:            This function will get executed when the user typed JPZ command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JPZ<label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JPZ(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_JPNZ
 * Description:     Conditional jump. Jump if the Comparison Register is not equal to 0.
 *                  The conditional jump commands checks the condition register (internal)
 *                  and assign PC(program counter) to the address where teh label is defined.
 * Implementation:  Get the carrier flag that is controlled by the macro module. If the value of the flag
 *                  is not equal to 0, then call JUMP command to jump to instruction with having the specified
 *                  label; otherwise continue the next instruction in the macro.
 * NOTE:            This function will get executed when the user typed JPNZ command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: JPNZ<label>
 *                      - label:    it the numeric number only to go to instruction where LABEL ####
 *                                  in the macro code.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_JPNZ(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RLN
 * Description:     Read macro line numbers while executing macro is stop. RLN will return 0 if
 *                  last macro is successfully executed.  It reaches the ENDM instruction.  If macro
 *                  is aborted before it executes the ENDM, RLN will return line number of instruction
 *                  which is aborted.  First instruction of the macro is assigned to line number one.  If RLN
 *                  is issued from command line while macro is executing, it will send line number of currently
 *                  executed instruction. RLN will return more than one number if a macro calls another macro. For
 *                  instance, macro A at line 10 is an instruction to call macro B. Upon issuing RLN, macro B is
 *                  executing line 15. The return value of RLN is 10.15. RLN returns as many numbers, separated by '.'
 *                   as the allowed depth of macro calling.
 * Implementation:  Call MAReadMacroLineNumber() from macro module. It will send out the line
 *                  number that is stop while executing the macro.
 * NOTE:            This function will get executed when the user typed RLN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RLN
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RLN(instr_ptr instr);
/**************************************************************************
 * Function Name:   ex_TSKI
 * Description:     Get current task id. This command is for multi-tasking.
 * Implementation:  If there is an operand, then get the current task id and store the value value into
 *                  the register that operand had specified the register index.  Otherwise, send the
 *                  current task id to comm port.
 * NOTE:            This function will get executed when the user typed TSKI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TSKI [ID]
 *                      - ID:   current task id and it is optional parameter. It must be indirect_reg type.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TSKI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_CSTR
 * Description:     Replaces a character to string at an index.
 * Implementation:  There is an instruction structure pointer's passed in from the
 *                  function arguement.  It contains the pointer to list of operand that we
 *                  need to parse and get information about operand for execution. The first
 *                  operand is either INDIRECT_CHAR or CHAR_CONST which is a character that want
 *                  to insert into the string.  Then get the second operand from operand list.
 *                  It is the position that a character want to insert at.  Then get the third
 *                  operand which must be INDIRECT_STR. Finally, call RGInsertStringSRegChar() to
 *                  insert character into specified position in the string.
 * NOTE:            This function will get executed when the user typed CSTR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: CSTR <character to be inserted>, <index for insertion>, <string>
 *                     - character to be inserted : must be INDIRECT_CHAR
 *                     - index for insertion:       must be INDIRECT_REG or LONG_CONST
 *                     - string:                    must be INDIRECT_STR
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CSTR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_EIM
 * Description:     Enable or disable interrupt macros.
 * Implementation:  There is instruction structure pointer passed in from function
 *                  arguement and it has pointer to operand list. This command is expect to
 *                  have to 2 operands which is bit and flag for enable/disable. So get the
 *                  value of first operand which is bit number from operand list. Then validate
 *                  the bit number to make sure it is in the range of 0..8. Then get the value of
 *                  the second operand which is the enabled/disabled flag from operand list.  Then
 *                  validate the flag to make sure it is 0 or 1 only. Finally, depend on the bit number
 *                  to call function from macro module to set the bit to be enable or disable.
 * NOTE:            This function will get executed when the user typed EIM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EIM <Bit#>, <flag>
 *                      - Bit#: the bit to be disabled or enable depending on the flag, and in the range of (0-8).
 *                              Each bit represents as:
 *                              0:  Enable/disable Com2 interrupt macro.
 *                              1:  Enable/disable IIM (I/0) interrupt macro.
 *                              2:  Enable/disable ERR (motor error) interrupt macro.
 *                              3:  Enable/disable LMM (limit switch) interrupt macro.
 *                              4:  set enable/disable for all bits.
 *                              5:  Enable/disable SECS interrupt macro.
 *                              6:  Enable/disable COM3 interrupt macro.
 *                              7:  Enable/disable INTLK (Emergency Motor Off) interrupt macro.
 *                              8:  Enable/disable SSTOP (Stop signal) interrupt macro.
 *                              9:  Enable/disable command error interrupt macro.
 *                              10: Enable/disable Equipe bus interrupt macro.
 *                      - flag: either enable(1) or disable(0)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EIM(instr_ptr instr);
int ex_REIM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_IAM
 * Description:     Inform after a Macro.
 *                  IAM sets type of tatus to be sent after finishing execution of a macro
 *                  when end of macro is encountered.  Status is sent only if the macro is ended by
 *                  ENDM with 1 as its parameter. It will not inform any thing if ENDM without 1.
 * Implemetation:   Get the value of the flag from operand pointer. Then validate it to make it is
 *                  valid flag in the range of 0..3. Finally, call MASetIAMMode() from macro module
 *                  to set the flag. So this flag will be used when the macro is executed at the end of
 *                  of the macro.
 * NOTE:            This function will get executed when the user typed IAM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: IAM <flag>
 *                  flag is an INDIRECT_REG or LONG_CONST.
 *                  The value of INDIRECT_REG or LONG_CONST must be 0, 1, 2, or 3.
 *                      0:  Do not send status
 *                      1:  Sends standard Equipe Status Word
 *                      2:  Sends non-standard customized status word
 *                      3:  Sends >
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_IAM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_LMCR
 * Description:     List the names of all macros contained in the OpCode table, or list the body of
 *                  individual macro specified by the operand. When listing body of macro LMCR can list it in two
 *                  formats. The format is specified by the line number flag. By default line number flag is 1 which
 *                  is to list body with line number at the beginning of each instruction.  If 0 is specified, it will list
 *                  in the format the body of macro is loaded.  Thus, allowing terminal to directly read and save to
 *                  a file that can be loaded for in the future.
 * Implementation:  Get the value of the first operand. If there is no operand, then call MRListMacroNames() to list
 *                  all the name of macros in the oc table.  Otherwise, before list body of specified macro, check if
 *                  there is a second operand. If there is second operand which is the option of printing the line number
 *                  or not and default is printing the line number. Then Call MRListMacro() to list body contents of
 *                  specified macro.  In MRListMacro will check if the specified macro is in the oc table, then list.
 *                  Otherwise return failure.
 * NOTE:            This function will get executed when the user typed LMCR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: LMCR [NAME OF DEFINED MACRO, LINE NUMBER FLAG]
 *                      - NAME OF DEFINED MACRO:    Name of defined macro that you want to list content to the macro.
 *                      - LINE NUMBER FLAG:         Either 0/1. 0 to list the instructions without line number.
 *                                                  1 or default to list the instructions with line number.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_LMCR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_MCLR
 * Description:     Macro Clear. This command removes the macro specified by "Macro Name". If '*' is
 *                  specified MCLR removes all currently loaded macros in the oc table. MCLR is used
 *                  before re-downloading macros to avoid the macro with the same name that currently in
 *                  the oc_table. There is not allow to define macro that already existed in the oc_table.
 * Implementation:  First, if there is only one operand, then if it is -255, then call MADeleteMacro() with passing
 *                  option flag to delete all the macros in the oc_table. If the operand value is not -255, then
 *                  it must be the specific macro name. Then call MADeleteMacro() with giving opcode number of
 *                  the specified macro and option flag to delete single macro. In the DeleteMacro will check
 *                  for opcode number of the macro is valid or not inorder to delete from the oc table. It will
 *                  return failure if the opocde number is invalid.
 * NOTE:            This function will get executed when the user typed MCLR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MCLR <*> or <NAME OF DEFINED MACRO>
 *                      - '*':  to delete all the macros that currently loaded
 *                      - NAME OF DEFINED MACRO:    delete only specified macro.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MCLR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_NTOS
 * Description:     Convert a number to a string and store the results in the specified string register.
 *                  If the radix is 10, then convert the number to string in decimal.
 *                  If the radix is 16, then convert the number to string in Hexidecimal.
 * Implementation:  There must have 3 operands, otherwise return failure.  Get the value to be converted in
 *                  the first operand. Check the second operan must be an INDIRECT_STR to store the result. Then
 *                  get the radix value in the third operand.  Then call RGConvertNumToStrNumSReg() to convert the
 *                  numberic value into string and store it into specified register (second operand).
 * NOTE:            This function will get executed when the user typed NTOS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: NTOS <NUMBER TO CONVERT>,<STRING VARIABLE>,<RADIX>
 *                      - NUMBER TO CONVERT:    number to be converted. It can be LONG_CONST or INDIRECT_REG.
 *                      - STRING VARIABLE:      variable to store the result. It must be a INDIRECT_STR.
 *                      - RADIX:                base of value in Hex or Dec. It can be LONG_CONST or INDIRECT_REG.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_NTOS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMCS
 * Description:     Read Macro Status. Macro status provides a mean of checking end of macro execution
 *                  and reason of th eend of macro execution. The status value has the difference meaning:
 *                  - 0:    No macro is running, previous macro was executed successfully through the end of
 *                          macro (ENDM).
 *                  - 1:    Macro is being executed.
 *                  - 2:    Since FRZ command is not used, so this value has no meaning.
 *                  - 3:    Last macro was aborted by STOP or ABM commands, no macro is currently running.
 *                  - 4:    Stack overflow, level too deep
 *                  - 5:    Others, controller is in condition such that current command cannot be executed
 *                          when the controller is in the following conditon:
 *                          - Position error generated by one of move commands.
 *                          - Axis is in servo off when a move command to the axis is issued.
 *                          - Axis is moving when another move command is issued.
 *                          - Home has not been executed before any move command.
 *                          - Limit switch is triggered.
 * Implementation:  There is no operand for this command. So, get the macro status from the macro module by calling
 *                  MRGetMacroStatus() and convert the status into the ascii characters.  Then send the status
 *                  to comm port.
 * NOTE:            This function will get executed when the user typed RMCS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RMCS [RETURN VALUE]
 *                      - RETURN VALUE: Store the reading status. It must be INDIRECT_REG.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMCS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_STON
 * Description:     Convert a String to a number.
 * Implementation:  Get the string value from the first operand. Then get the index of register to store
 *                  the result from the second operand which must be an INDIRECT_REG. Then get the radix from
 *                  the third operand.  Then call RGConvertStrToNumStrReg() to convert the string number basing
 *                  on the radix value (either HEX or DEC).
 * NOTE:            This function will get executed when the user typed STON command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STON <STRING VALUE>, <NUMERIC VALUE>,<RADIX>
 *                      - STRING VALUE:     It is the string to be convert to numeric. It can can INDIRECT_STR or STRING_CONST.
 *                      - NUMERIC VALUE:    Stores result after converting.  It must be INDIRECT_REG type.
 *                      - RADIX:            base of value in Hex or Dec. It can be LONG_CONST or INDIRECT_REG.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STON(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_STRC
 * Description:     Retrieve a Character from a String at a specified index and
 *                  store into a character variable.
 * Implementation:  Get the string value from first operand and the operand type must be INDIRECT_STR type.
 *                  Get index value from the second operand and validate the index to make sure it
 *                  is in the range 0..255. Then get index of INDIRECT_CHAR to store the character
 *                  value in the register at index of INDIRECT_CHAR.  Then call RGExtractCharacterStrCReg()
 *                  to retrieve a character at a specified index in the string.
 * NOTE:            This function will get executed when the user typed STRC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STRC <STRING VALUE>, <INDEX>,<CHARACTER>
 *                      - STRING VALUE:     a string value and it must be INDIRECT_STR.
 *                      - INDEX:            at position in the string to get the character.
 *                                          It can be LONG_CONST or INDIRECT_REG.
 *                      - CHARACTER:        store the character that retrieve from the string at INDEX.
 *                                          It must be INDIRECT_CHAR.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STRC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_WAIT
 * Description:     Wait for a specified time period before executing the next instruction.
 * NOTE:            This function will get executed when the user typed WAIT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: WAIT
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WAIT(instr_ptr instr);

/***************************************************************************
 * Function Name:   ex_CONT
 * Description:     Command that causes frozen macro to continue. After macro was frozen this command
 *                  finishes the motion stopped by FRZ command and executes next instructions in the macro.
 * Command Syntax:  CONT
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CONT(instr_ptr instr);

/***************************************************************************
 * Function Name:   ex_FRZ
 * Description:     Command that freezes execution of macro. It immediatelly decelerates to stop all current motions,
 *                  and keeps macro in the frozen state until CONT command is called.
 * Command Syntax:  FRZ
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FRZ(instr_ptr instr);

/***************************************************************************
 * Function Name:   ex_SCMEF
 * Description:     Set iCMEFlag used by CMER interrupt.
 * Command Syntax:  SCMEF 0/1
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCMEF(instr_ptr instr);

/***************************************************************************
 * Function Name:   ex_RCMEF
 * Description:     Read iCMEFlag used by CMER interrupt.
 * Command Syntax:  RCMEF <[R]>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCMEF(instr_ptr instr);
int ex_SDUMP(instr_ptr instr);
#endif
