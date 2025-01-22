/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        proces98.h
 * Functions:   PCProcessCmdLine
 *              PCProcessMacroInstr
 *              PCProcessExecuteInstr
 *              PCSetPromptMode
 *              PCGetWDPrompt
 *
 * Description: Handles processing command line instructions and macro
 *              execution, executing a specific interrupt macro.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
\***************************************************************/

#ifndef _H_PROCES98_H
#define _H_PROCES98_H

/********** DEFINES **********/

#define IIM_TYPE        0               /* I/O interrupt */
#define ERR_TYPE        1               /* Error motion interrupt */
#define LMM_TYPE        2               /* Limit switch interrupt */
#define DC_MODE_TYPE    3               /* Daisy chain mode type for COM2 interrupt */
#define COM2_TYPE       DC_MODE_TYPE    /* COM2 and DC chain set */
#define COM3_TYPE       4               /* COM3 interrupt */
#define CMER_TYPE       5               /* Command error interrupt */
#define INTLK_TYPE      6               /* Emergency Off Interrupt */
#define SSTOP_TYPE      7               /* Signal Stop Interrupt */
#define SECS2_TYPE      8               /* SECS interrupt */
#define DER_TYPE        9               /* Drift Error interrupt */
#define SECS2_TYPE2    10               /* SECS2 interrupt */
#define SECS2_TYPE3    11               /* SECS3 interrupt */
#define SECS2_TYPE4    12               /* SECS4 interrupt */


/********** FUNCTION PROTOTYPES **********/

/*******************************************************************
 * Function Name:   PCProcessCmdLine
 * Purpose:         Handling the process of the command line such as parsing,
 *                  validating and executing.
 *                  First, check for is there a multi-tasking running, if there is, then check
 *                  is the task pending killed; if so, just exit the function.  Otherwise,
 *                  setting the executing macro flag to false (no macro is running). Then get the
 *                  command line form COM Port by calling COGetsRxBuff from comm module.  When you call
 *                  COGetsRxBuff, make sure to specify number of byte that you want to get from the
 *                  COM buffer.  If you want to get the whole string with ending of "\r", then set the number
 *                  of bytes to -1.  Make sure to initialize the instruction pointer before pass into the function
 *                  (CMDcmndLineAction())for parsing the command. Then passing the whole command line to low-level
 *                  module to parse and validate the command. If the parsing and validation are success,
 *                  then the function will return the instruction which is the instruction to be executed.
 *                  After finished executing the instruction, if there is instruction
 *                  in the current PC and there is current task, then set the current PC to be next
 *                  execution instruction in the current task. Remember to free the operand from the
 *                  instruction after executed the instruction. if success executed, it will print
 *                  "\r\n>", else if it is failure from validating or parse then print out "\r\n?". If the
 *                  command line contains only "\r" then it will consider as blank line and print out
 *                  "\r\n>". The prompt is printed also depending on the mdoe you are on such as
 *                  regular or Brook emulator (NOVELLUS). For NOVELLUS, if it is failure from validating or
 *                  parsing then print out error code.
 *                  This function is called from the mainloop.
 * Parameter:       none
 *
 * Returns:         none
 * ********************************************************************/
void PCProcessCmdLine(void);
/***********************************************************************
 * Function Name:   PCProcessMacroInstr
 *
 * Purpose:         Handling the execution of macro in the mainloop.
 *                  Frist, check if there is no interrupt macro running, then come in to the interrupt section to
 *                  check for any interrupt that need to execute the interrupt macro. Durring the interrupt macro is
 *                  running, no interrupt can execute.  In the case, If There is an interrupt but the enable flag is off,
 *                  then interrupt needs to set back the flag to off.  So we can avoid of other interrupt come in.
 *                  This situation happens to all the interrupt except for COM2 and COM3 interrupt. Then if no
 *                  multi-tasking is make, executing the next command that got push when interrupt macro is
 *                  executing.  If there is multi-tasking , the current task is not waiting then executing the
 *                  current task.  However, before executing, make sure the timer interrupt of the task is not up yet.
 *                  If the timer interrupt is up and not interrupt signal, then switch to next task that is active.
 *                  Then execute the task.  Other thing, if the interrupt macro is executing and critical interrupt section is
 *                  TRUE then you don't allow to switch the task.
 *
 * Parameter:       none
 *
 * Returns:         none
 *
 * ************************************************************************/
void PCProcessMacroInstr(void);
/************************************************************************
 * Function Name:   PCExecuteInterrupt
 * Purpose:         handling the executing of interrupt macro by checking which
 *                  type of interrupt macro need to be executed. Then check to make sure
 *                  insturction pointer of interrupt macro of that type is not null,
 *                  then execute.  However, if instruction pointer of that interrupt macro is
 *                  NULL, then reset the enter interrupt section flag to false. So the next
 *                  interrupt can come in to the interrupt section. Before execute the instruction
 *                  make sure to save the top of stack, so by the time you finished executing the
 *                  interrupt you can continue to execute the other macro that you have saved, but
 *                  this only happen to COM2 and COM3 interrupt macro. In the case, if the
 *                  interrupt macro is executing and there is an error, then we need to
 *                  set the enter interrupt section flag to false since the macro will never
 *                  go to end of macro since end of macro function is where we set back the interrupt
 *                  section flag to FALSE after finished executing the function. The rest of
 *                  the macro execution will continue at the mainloop in PCProcessMacroInstr().
 *
 * Parameter:       iInterruptTypeArg   - type of interrupt
 *                  iCurrentPendingKilledArg - (true/false return value)
 * return:          SUCCESS or FAILURE
 * *************************************************************************/
int PCExecuteInterrupt(int, int*);
/************************************************
 * Function Name:   PCSetPromptMode
 *
 * Description:     interface for setting the prompt mode value.
 *                  This value tells which mode the prompt should print when error occur.
 *                  Novellus or regular mode.
 *
 * Parameter:
 *                  iModeArg    - new mode value
 *
 * Returns:         none
 *
 * ************************************************/
void PCSetPromptMode(int iModeArg);
/************************************************
 * Function Name:   PCGetWDPrompt
 *
 * Description:     interface for getting the prompt to be used after
 *                  a watchdog timer timeout occurs. This prompt
 *                  only replaces the '>', not the '?' or '_ERR'.
 *
 * Parameter:       none
 * Returns:         post-watchdog timer prompt prompt.
 *
 * ************************************************/
char PCGetWDPrompt();

#endif
