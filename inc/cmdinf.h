/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:       cmdinf.h
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

#ifndef _H_CMDINF_H
#define _H_CMDINF_H

/***************************************************************************
 * Function Name:   ex_GLST
 * Description:     Read galil motion control card status.
 *                  This command will tell you the status of the motion control card in a 32-bit
 *                  status double word. In the case of error on which axis and it also indicates
 *                  on which axis the servo is off. Please look at the "Software Manual" for detail
 *                  description information of status bit.
 * Implementation:  Get the galil status from Mechanism module by calling ROReadGalilStatus() and it
 *                  returns the 32-bit status word. Then is there an operand and operand type is INDIRECT_REG?
 *                  If it is, then store the status value into INDIRECT_REG at specified index that is provided
 *                  in the operand structure. Otherwise, send the status word to comm port.
 * NOTE:            This function will get executed when the user typed GLST command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: GLST [return value]
 *                      - return value: it store the status in INDIRECT_REG only if the user does not want
 *                                      to print out the screen.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_GLST(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_STAT
 * Description:     Send System Status Word. For specific status bit information, please look at
 *                  "software manual". This status word has information on command executing error, invalid
 *                  command executed, vaccum sensor ans switch status, motor, limit switch, homing flag, alignment
 *                  error, macro status, axis status, servo, COM2 status, controller error and COM1 status. This
 *                  command is used when there is some error occured or some problem while trying to execute somehting.
 * NOTE:            This function will get executed when the user typed STAT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STAT [FLAG]
 *                      - FLAG:     Optional parameter for showing the description of each
 *                                  error status bit if the flag is exist.
 *                                  - 1: for description of each error bit
 *                                  - 2: for description and resolving method for each error bit.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STAT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_STAT2
 * Description:     Send system status word in 32 bits.  It is a combination of the first 16 bits of STAT and
 *                  the last 16 bits are about COM3 and others about scanner.
 *                  For codemerge (6.00 I2BRT), There is only COM3 status in 6th bit of
 *                  the last 16 status bits for now since there is not S-293 scanner.
 * NOTE:            This function will get executed when the user typed STAT2 command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: STAT2 [FLAG]
 *                      - FLAG:     Optional parameter for showing the description of each
 *                                  error status bit if the flag is exist.
 *                                  - 1: for description of each error bit
 *                                  - 2: for description and resolving method for each error bit.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STAT2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_STAT3
 * Description:     Send initialziation failures of basic modules status word in 16 bits.
 * NOTE:            This function will get executed when the user typed STAT3 command
 *                  at the command line. *
 *                  Command Syntax: STAT3 [FLAG]
 *                      - FLAG:     Optional parameter for showing the description of each
 *                                  error status bit if the flag is exist.
 *                                  - 1: for description of each error bit
 *                                  - 2: for description and resolving method for each error bit.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_STAT3(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_GSTAT
 * Description:     Send emulation genmark status. NOT USED
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_GSTAT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_VER
 * Description:     Display the firmare version number. It will displace the version and more
 *                  depend on the option flag that you passed in the parameter.
 * Implementation:  Get option flag for printing from first operand if there is one.
 *                  If there is no operand, then print Equipe name, copyright and firmware version.
 *                  If there is an operand and the flag value is 1, then print version,
 *                  and date of image was created. If the flag value is 2, then
 *                  print version, date and time of image was created. Otherwise, print
 *                  the library version.
 * NOTE:            This function will get executed when the user typed VER command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: VER [OPTIONAL FLAG]
 *                      - OPTIONAL FLAG:    it is the flag to display difference information of
 *                                          the firmware.
 *                                          - default: display the Equipe name, copyright and
 *                                                     firmware version string.
 *                                          - 1:       display version, and date of image is created.
 *                                          - 2:       display version, time and date of image is created.
 *                                          - 3:       display library version.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_VER(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RSRN
 * Description:     Read Serial Number of each component in the system such as robot, prealigner, track,
 *                  flipper. If parameter is omitted or 0, RSRN returns robot serial number.  If parameter is 1, then
 *                  RSRN returns prealigner serial number. 2 is for auxaulary component such as track or flipper.
 * Implementation:
 * NOTE:            This function will get executed when the user typed RSRN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSRN <OPTION>
 *                      - OPTION:   type of component serial number such as Robot, prealigner and track
 *                                  or flipper or dual arm. The value of the option are:
 *                                  - 0 or nothing: robot serial number
 *                                  - 1:            prealigner serial number
 *                                  - 2:            track or flipper serial number.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSRN(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_HEAP
 * Description:     Debug instruction which causes information on the heap to be dumped.
 * Implementation:  It called HPHeapDump() to dump the memory information in NVSram such as
 *                  amount of heap used, amount of heap freed, number of bytes available and number of
 *                  bytes used in NVSram.
 * NOTE:            This function will get executed when the user typed HEAP command
 *                  at the command line. The function is actually called in MRExecuteInstructions().
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *                  Command Syntax: HEAP .
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_HEAP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RKEY
 * Description:     Display the current total number of timer ticks.
 * Implementation:  Get the current total number of timer ticks as a long.
 *                  Print it out to the screen.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RKEY(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCFG
 * Description:     Sets system configuration parameters by decoding passwords
 *                  and then interfacing with the datafile module to set
 *                  configuration file parameters.
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCFG(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCFG
 * Description:     Displays system configuration settings.
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCFG(instr_ptr instr);
/****************************************************************
 * Function:        ex_RSTIM
 * Description:     Gets the current timer counts
 * Implementation:
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************/
int ex_RSTIM(instr_ptr instr);

int ex_SDBG(instr_ptr instr);
int ex_RDBG(instr_ptr instr);

int ex_XETH(instr_ptr instr);
int ex_RSTMR(instr_ptr instr);

#endif
