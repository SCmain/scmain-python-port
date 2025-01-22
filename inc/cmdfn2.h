/***************************************************************\
 *
 * Program:     miscellaneous low-level command, command interpreter and Opcode table.
 *
 * File:        cmdfn2.c
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

#ifndef _H_CMDFN2_H
#define _H_CMDFN2_H

/**************************************************************************************
 * Function Name:   ex_NOP
 * Description:     A no-operation execution routine.  This routine is only used for development
 *                  and debugging.
 * Note:            not implement for this version (I2BRT)
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 **************************************************************************************/
int ex_NOP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DIAG
 * Description:     Begin Teach Mode in specified comm port.
 * Implementation:  First set the default Comm port for diagnostic is at COM2.
 *                  Check if there is an operand in an instruction, then get the comm
 *                  port id and validate to make sure it is either COM1 or COM2.  Then
 *                  Start execute the diagnostic by calling DGSetDiagFlag() from Diagnostic module.
 * NOTE:            This function will get executed when the user typed DIAG command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: DIAG [comm port]
 *                      - comm port:    COM1 or COM2. Also, the value should be LONG_CONST ONLY.
 *                      - default with operand parameter is COM2.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DIAG(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DM
 * Description:     Dump Memory.  This funciton is used for debugging only
 * NOTE:            This command is not used now.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_WM
 * Description:     Write memory.  this function is used for debugging only.
 * NOTE:            This command is not used now.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ENDTR
 * Description:     End Teach Mode.  this command is only valid when you are on the terminal mode with teach pendant.
 *                  It is used to exit from the terminal mode on the teach pendant.
 * Implementation:  First, check if the user execute this command is at teach pendant port or other port.
 *                  If the not teach pendant port (COM2), then return failure. Otherwise, set comm port to be
 *                  COM1(or EQT port) and clear the COM1 buffer by calling COFlushComBuffer() from comm module,
 *                  and show the Eqipe logo by calling TTShowLogo() from teach pendant module.
 * NOTE:            This function will get executed when the user typed ENDTR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ENDTR
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ENDTR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_GALIL
 * Description:     Go to GALIL mode, and bypassing command interpreter.
 *                  This command allows the user to work directly on the level of the GALIL
 *                  card. When returning to interpreter mode, make sure that all parameters are proper.
 *                  The interpreter doesn't recognize any change made in this mode.
 * Implementation:  First, check if there is any operand.  If there is no operand, then default is 0 which is
 *                  first galil card (robot card). If there is operand, then get the operand and validate the value
 *                  which must be 0 or 1. Then call MAStartGalilMode() to start the galil mode.
 * NOTE:            This function will get executed when the user typed GALIL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: GALIL [galil card number]
 *                      - galil card number:    the card number is either 0 or 1.
 *                                              - 0 and default:    robot galil card and it is first card in the controller.
 *                                              - 1:    prealigner galil card and it is the second card in the controller.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_GALIL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_EXIT
 * Description:     Debug instruction to exit the program and return to DOS.
 *                  This command is for internal use only. Not for customer.
 * Implementation:  First, get the value from an operand and check for value which must
 *                  be 628.  628 is the password to exit to DOS.  Then close all initialized
 *                  devices and restores all the vector as neccesary and exit the program.
 * NOTE:            This function will get executed when the user typed EXIT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EXIT 628
 *                      - 628:   is the password to exit to DOS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_EXIT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_NPAR
 * Description:     Get Next Parameter or get the number of parameters that passed in from the macro agruments.
 *                  The macro parameters are saved in the Special Registers (special register for internally use only).
 *                  This command is only for NOVELLUS macro.
 * Implementation:  There must have at least 2 operands, but it also depend on the value of the first operands to
 *                  require for third operand or not.  So we must check for first 2 operands to be not NULL.  Get
 *                  value of the first operand.
 *                  If the value of first operand is less than 0:
 *                      - The second operand type must be the LONG_CONST.
 *                      - Set the prompt mode by calling PCSetPromptMode() and PCSetPromptMode will
 *                        determine the value of the mode to print the prompt or not. If mode is 1, print
 *                        the "\r\n>". If mode is 0, don't print "\r\n>" when hit CR or RESP command execute.
 *                  If the value of first operand is equal or greater than 0:
 *                      - The second operand type must be INDIRECT_REG type.
 *                      - There must be a third operand and it must be INDIRECT_REG type.
 *                  If the value of the first operand is greater than value that store internally in special register 0,
 *                  then set -1 into the second and third operand and return them. The special register 0 is holding
 *                  number of operands(parameters) that macro has and it got storing at the macro execution time
 *                  in MRExecuteMacro(). That's meant the user try to get parameter that does not exist.
 *                  If the first operand is less than the value internally in special register 0,
 *                  then get the value of specified parameter and store into the second operand register and set the third
 *                  operand register to 0.
 * NOTE:            This function will get executed when the user typed NPAR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: NPAR <ACTION>, <FLAG or RETURN VALUE>,[RESULT OF EXECUTION]
 *                      - ACTION:   the purpose of calling the command at that time.
 *                                  if -1, relate to prompt mode.
 *                                  if 0, get number of parameters that macro has when it is executing.
 *                                  if >=1, get the specific parameter.
 *                                  This parameter can be LONG_CONST or INDIRECT_REG type.
 *                      - FLAG or REUTRN VALUE:     If the first parameter is -1, then this flag will be
 *                                                  telling process mode:
 *                                                  - If 1, in Equipe mode and allowing to download and
 *                                                    when you press enter, you will get "CRLF>".
 *                                                  - if 0, in Brooks mode. You will not "CRLF>" when
 *                                                    press enter or sending RESP command.
 *                                                  - NOTE: the first parameter is -1, then this parameter
 *                                                          must be LONG_CONST type.
 *                                                  If the first parameter is 0, then it stores the number
 *                                                  of parameters.
 *                                                  If the first parameter >=1, then it stores the value of
 *                                                  specific parameter.
 *                                                  This parameter can be LONG_CONST or INDIRECT_REG type depend
 *                                                  on the first parameter value.
 *                     - RESULT OF EXECUTION:       This will store the success or failure of the command when
 *                                                  trying to do some thing.  Example, if trying to get the value
 *                                                  of the third parameter but there is not more parameter then
 *                                                  this variable will store -1. It has to be INDIRECT_REG type.
 *                      Example:    NPAR -1, 1:     Puts in Equipe mode, gives you a "CRLF>" when you press enter.
 *                                                  Allows downloading.
 *                                  NPAR -1,0:      Puts i Brooks mode. No "CRLF>" when you press enter or when
 *                                                  it gets a RESP command.
 *                                  NPAR 0,[R1],[R2]:   Returns the number of parameters on the command line in R1.
 *                                                      If none then [R1] returns 0 and [R2] returns -1 else >1.
 *                                  NPAR 1,[R1],[R2]:   Returns the first parameter in [R1] if none then [R2] = -1
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_NPAR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RES
 * Description:     Reset Controller. This command does software reset which re-initialize controller
 *                  to power-up condition. when the controller is ready, it will send the ready prompt.
 * NOTE:            This function will get executed when the user typed RES command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RES
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RES(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TEACH
 * Description:     Begin Teach Mode to teach the robot or track at certain coordinate.
 * Implementation:  The default COMM port from teach mode to start is at COM2 if there is no operand.
 *                  If there is an operand, get the comm port id and validate the comm port id.  Then
 *                  start running the teach mode.
 * NOTE:            This function will get executed when the user typed TEACH command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TEACH [PORT]
 *                      - PORT:     which comm port is teach mode will be running on.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TEACH(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_EIS
 * Description:     enter interrupt section. This will block other interrupt from get
 *                  into the interrupt section. It's mean there may be some interrupt is in the
 *                  processing or executing.
 * Implementation:  Set the enter interruption section flag to true to block other from come into
 *                  the interrupt section to process the interrupt while another interrupt process the
 *                  interrupt routine.
 * NOTE:            This function will get executed when the user typed EIS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EIS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 *************************************************************************/
int ex_EIS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_LIS
 * Description:     Leave interrupt section. It will allow other interrrupts to come in
 *                  the interrupt section to process their interrupt if there is an interrupt.
 * Implementation:  To allow other interrupt to come in the interrupt section by setting the
 *                  interrupt section flag to false.
 * NOTE:            This function will get executed when the user typed EIS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: EIS
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 * **************************************************************************/
int ex_LIS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_MTRC
 * Description:     Macro tracing for debugging only the macro while it is executing.
 *                  You can turn trace on or off and how you want to print out the trace;
 *                  depends on the mode:
 *                      - 0:    turn off the trace mode.
 *                      - 1:    turn on the trace mode and display all the executing lines in the
 *                              macro include the macro call.
 *                      - 2:    turn on the trace mode display only those macro call inside the executing
 *                              macro.
 *                  All the tracing message will send to comm port.  Default port is the current using port.
 *                  Otherwise, using the specified port.
 *                  Only in codemerge
 * Implementation:  First, if there is no operand then set to default mode and port which is turn on trace mode and
 *                  use current port. If there is an operand, get the mode and get the port from the second operand if there
 *                  is one. Otherwise, set the default port. Then set the mode and port to get ready for debugging.
 *                  When the macro is executing, the tracing is happened or not depending on the mode. The mode will get
 *                  checked from the excecution instruction in the macro module.
 * NOTE:            This function will get executed when the user typed MTRC command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: MTRC [MODE], [PORT]
 *                      - MODE:     Tracing mode such as turn off/on tracing
 *                      - PORT:     Comm port to send the trace to.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MTRC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SECS
 * Description:     send SECS communications message out to a selected comm port
 * Implementation:
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SECS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DOS
 * Description:     passes DOS level command to the operating system
 * Syntax:          DOS "DOS command"
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
//Dos&Counters
int ex_DOS(instr_ptr instr);
int ex_SCSR(instr_ptr instr); // special command to reset SECS processing
int ex_SRES(instr_ptr instr); // special command to reset SECS processing
int ex_DSEC(instr_ptr instr); // special command to dump SECS processing
#endif
