/***************************************************************\
 *
 * Program:     Controller firmware
 * File:        macro.h
 * Functions:   MAInit
 *              MAInitMacros
 *              MRMacroDefine
 *              MRTakeBody
 *              MAInsertMacro
 *              MRListMacroNames
 *              MRListMacro
 *              MADeleteMacro
 *              MADeletedMacroScan
 *              MAFreeInstructions
 *              MAInitStack
 *              MRPushPC
 *              MRPopPC
 *              MRLaunchMacro
 *              MRWriteMacros
 *              MRWriteBody
 *              MRWriteBytes
 *              MRWriteCheckSum
 *              MRLoadMacros
 *              MAReadMacros
 *              MAReadBody
 *              MAReadInstr
 *              MAReadBytes
 *              MAReadCheckSum
 *              MASaveMacrotoNvsram
 *              MRLoadMacroFromNvsram
 *              MRPreLaunchMacro
 *              MRExecuteMacro
 *              MRExecuteInstructions
 *              MRListInstructions
 *              MRAbortMacro
 *              MAFindOpcode
 *              MREndMacro
 *              MAReadMacroLineNumber
 *              MRGetPStatus
 *              MRGetStkTop
 *              MRGetStkTopBeforeInit
 *              MRGetSavedStkTop
 *              MASetSavedStkTop
 *              MRGetNumberOfMacro
 *              MASetNumberOfMacro
 *              MRGetFreezeMacro
 *              MASetFreezeMacro
 *              MRGetAbortLineNumber
 *              MASetAbortLineNumber
 *              MRGetCf
 *              MASetCf
 *              MRGetMacroStatus
 *              MASetMacroStatus
 *              MRGetPC
 *              MASetPC
 *              MRGetMacroIntegerVars
 *              MASetMacroIntegerVars
 *              MRGetMacroPC
 *              MASetFlagsToFalse
 *              MRGetMacroAvoidRecursive
 *              MADisplayMacroTrace
 *
 * Description: Handling the macro programming. Create, list, save, and load macros.
 *              Handling the all the interrupt macro.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: Function returns SUCCESS or FAILURE unless otherwise
 *          specified
 *
\***************************************************************/

#ifndef _H_MACRO_H
#define _H_MACRO_H

#include "cmdfns.h"
#include "sctag.h"
#include "fio.h"

/********** DEFINES **********/

#define MAXSTACK    30
//#define MACWILDCARD "macro.%ld"
#define UNKNOWN_OPCODE -2

/* Descriptors to get and set file global variables. */
#define MR_RUNNING_CMER      0
#define MR_RUNNING_INTLK     1
#define MR_RUNNING_SSTOP     2
#define MR_PENDING_INTLK     3
#define MR_PENDING_SSTOP     4
#define MR_PC_WAITING        5
#define MR_EXECUTING_MACRO   6
#define MR_IAM_MODE          7
#define MR_ERR_INTERRUPT     8
#define MR_IIM_INTERRUPT     9
#define MR_LMM_INTERRUPT    10
#define MR_SECS_INTERRUPT   11
#define MR_CMER_INTERRUPT   12
#define MR_INTLK_INTERRUPT  13
#define MR_SSTOP_INTERRUPT  14
#define MR_INTR_SECT_FLAG   15
#define MR_IIM_ENABLED      16
#define MR_ERR_ENABLED      17
#define MR_LMM_ENABLED      18
#define MR_SECS_ENABLED     19
#define MR_COM2_ENABLED     20
#define MR_COM3_ENABLED     21
#define MR_CMER_ENABLED     22
#define MR_INTLK_ENABLED    23
#define MR_SSTOP_ENABLED    24
#define MR_TRACE_MODE       25
#define MR_TRACE_PORT       26
#define MR_DEFINING_MACRO   27
#define MR_EMULATOR_FLAG    28
#define MR_COM2_INTERRUPT   29
#define MR_COM3_INTERRUPT   30
#define MR_RUNNING_ERR      33
#define MR_PENDING_ERR      34
#define MR_DER_INTERRUPT    35
#define MR_DER_ENABLED      36
#define MR_SECS2_INTERRUPT  37
#define MR_SECS3_INTERRUPT  38
#define MR_SECS4_INTERRUPT  39



/********** VARIABLES **********/

/* structure contains the information of macro execution. */
typedef struct prog_status_st
{
    instr_ptr m_PC;                     /* PC is current instruction */
    instr_ptr m_OldPC;                  /* previous instruction, for RLN use only*/
    long m_lCf;
    int m_iFrzStatus;
    int m_iMacStatus;
}stProgStatus, *pstProgStatus;


/********** FUNCTION PROTOTYPES **********/
/*******************************************************
 * Function Name:   MAClearMacroNestLevel
 * Description:     Set 0 to macro nest level variable
 * ********************************************************/
void MAClearMacroNestLevel( void );

/*******************************************************
 * Function Name:   MASetMacroIntegerVars
 * Description:     Set various macro global variables
 * Parameters:      iFlagArg - Flag indicating which variable to be set
 *                  iValueArg - value that the variable is to be set to
 * Returns:         SUCCESS / FAILURE
 * ********************************************************/
int MASetMacroIntegerVars(int iFlagArg, int iValueArg);
/*******************************************************
 * Function Name:   MRGetMacroIntegerVars
 * Description:     Get various macro global variables
 * Parameters:      iFlagArg - Flag indicating which variable to be set
 * Returns:         the requested value
 * ********************************************************/
int MRGetMacroIntegerVars(int iFlagArg);
/************************************************
 * Function Name:   MRGetMacroPC
 * Description:     Get the first instruction pointer of interrupt macro.
 *                  This pointer is got when there is an interrupt and need to execute
 *                  interrupt macro. It can get the instruction pointer for
 *                  all the interrupt macros.
 * Parameters:      iFlagArg - Flag indicating which variable to be set
 * Returns:         instruction pointer (instr_ptr)
 * ***********************************************/
instr_ptr MRGetMacroPC(int iFlagArg);
/*************************************************
 * Function Name:   MAInit
 * Description:     Get configure flag from the main module. Initialize all
 *                  the global flags and variables of macro module.
 *                  Also, check if there is the auto macro then run the auto macro.
 *                  This function is called by the mainloop module.
 * Return:          value to determine whether there is auto macro or not
 * *************************************************/
int    MAInit(int iDefineFlagArg, int iEmulatorFlagArg);
/*************************************************
 * Fucntion Name:   MAInitMacros
 * Description:     Initialize all the global variables and flags that only global to
 *                  macro module. Also, initalize the stack.
 * Returns:         SUCCESS/FAILURE
 * *************************************************/
int    MAInitMacros(void);
/**************************************************************************************************
 * Function name:  MADefineMacro
 * Description:    There is a character pointer that's passed from a function arguement.  The pointer
 *                 is pointer at the position after parse out the key word "MACRO". We need to parse all
 *                 the begining space if any. Then start at the pointer points to the first character of the
 *                 name of the macro, Find the opcode and get the opcode.  The opcode should be
 *                 INVALID_OPCODE. If it is valid then the macro is already defined and it should return
 *                 failure. Then parse out the name of macro and save it for avoiding macro calls itself.
 *                 Construct the new instruction by allocate the memory and initalize it so we can store the
 *                 the operand information from calling CMDget_operand. After has the instruction with operands,
 *                 validate the macro instruction and insert macro into OC table and macro is assigned with new
 *                 OC number.  Also, initialize the label structure. When get_operand, validate macro or insert
 *                 macro return failure, you need to free the memory of the instruction that you already
 *                 allocated.  Then set the defining macro flag to TRUE which means the macro is at defining state.
 *                 While at the defining state, the MRTakebody is get the each instruction from the command line
 *                 and parse and validate before linking with the macro instruction list. The macro finishes defining
 *                 when the ENDM is typed and set back the defining macro flag to FALSE. Finally, clean up and fix all
 *                 undefine label.
 *
 * Return:         SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRMacroDefine(char *pcStrArg);
/*************************************************
 * Function Name:   MRTakeBody
 * Description:     Here we get line by line from the command line. Then
 *                  parse and validate each line to make sure it's valid.
 *                  Allocate the memory for each new instruction and link it into
 *                  the instruction list. The macro finishes defining when the
 *                  command line is sent "ENDM". In the case of the instrution is
 *                  a condition jump or label command then call MAResolvedLabel to
 *                  parse the label and resolve the label reference.
 *
 * Return:          SUCCESS or FAILURE
 * *************************************************/
int    MRTakeBody(instr_ptr instrArg);
/**************************************************************************************************
 * Function Name:       MAInsertMacro
 * Description:         To insert the macro into the OC table:
 *                      1. Fill in the property information in the structure of OC table. OC table
 *                         structure is one entry in the OC table. The structure contains Opcode,
 *                         macro name or low-level string name, valid flag, pointer points to first
 *                         macro instruction or pointer points to validate rounter, and fucntion pointer to
 *                         execution routine. So set the opcode and macro name into structure, set pointer points
 *                         to the first instruction of the macro and set the function pointer points to MRExecuteMacro.
 *                      2. There is iNVSram flag for either insert the macro to oc table from the command line or insert macro
 *                         into oc table when loading the macro from NVSRam. If iNVSram is TRUE, then call the
 *                         CMDcommands() from low-level module to add the entry without sorting the table. In this way, we insert
 *                         all the macro entries into table and sort at once at the end. if iNVSram is FALSE,
 *                         then call the CMDcommands() from low-level module to add the entry with sorting the table each
 *                         added to the table.
 *                      3. After added each entry at a time, copy the new Opcode that is assigned by insertion routine
 *                         from low-level module to the instruction.
 *                      4. If successfuly inserted, increment the number of macro defined.
 *                      5. Check for interrupt macro, if the defined macro is one of the interrupt macro,
 *                         then save the instruction pointer to specific type of interrupt instruction.
 *
 * Parameters:          pcNameArg   - macro name
 *                      instrArg    - instruction to insert into the table
 *                      iOpcodeArg  - Opcode of instruction and macro name
 *                      iNVSram     - flag to check if insert macro from command line or
 *                                    insert macro when loading macro from NVSRam.
 * Return:              SUCCESS or FAILURE
 * **************************************************************************************************/
int    MAInsertMacro(char *pcNameArg, instr_ptr instrArg, int iOpcodeArg, int iNVSram);
/**************************************************************************************************
 * Function Name:   MRListMacroNames
 * Description:     List all macro names that are defined without the content of the each macro.
 *                  First, call Low-level function LLCmnds to get the first macro in the OC table.
 *                  If the return value is FAIULRE, then that's meant there's no macro in the table.
 *                  Otherwise, loop to get the next macro from table and print out the name of the macro
 *                  until there's no more macro. While printing out the name of macro to the terminal port,
 *                  if the port is EQT (COM1) then list the macro name without waiting for the CR from the
 *                  user; otherwise, if the port is Teach pendant(COM2), then list each macro name and require
 *                  the CR from the user before listing another macro name.
 *                  This function will be executed when user typed "LMCR" at the command line.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRListMacroNames(void);
/**************************************************************************************************
 * Function Name:   MRListMacro
 * Description:     List specific macro with content of the macro and either with line
 *                  number or not.
 *                  First, validate the Opcode that pass in as arguement. Then get the macro entry from
 *                  the OC table. Macro entry contains macro name or low-level command name,
 *                  Opcode, union pointer to macro or validate rountine, validation flag when the command
 *                  is valid and function pointer to execution rountine.
 *                  Then check for entry to make sure it's not NULL and it's not
 *                  low-level command. Then get the first pointer of instruction list from the macro
 *                  entry. Go through each instruction from the list and print out the instruction
 *                  with or without the line number; it depends on the iNumflagArg value (true/false) that passed in
 *                  from the arguement. TRUE print with line number and FALSE without line number.
 * Parameters:      iOpcodeArg  - OC/Opcode of the macro instruction
 *                  iNumflagArg - flag for listing with or without line number.
 * Return:          SUCCESS and FAILURE
 * **************************************************************************************************/
int    MRListMacro(int iOpcodeArg, int iNumFlagArg);
/**************************************************************************************************
 * Function Name:   MADeleteMacro
 * Description:     Delete the macro from the table, if
 *                  iScanFlagArg is false, delete all; else
 *                  delete specific macro only.
 *                  To delete specific macro, need to validate the Opcode to make sure
 *                  the macro is existing. Then check for interrupt macro; if the macro to be
 *                  deleted is the interrupt macro then set the instruction pointer of interrupt macro
 *                  to NULL.  Then delete the macro and decrement the number of defined macro.  Then
 *                  scan through (reference)/other macro that call this macro and set OC of instruction/line to be
 *                  INVALID_OPCODE. So it will be failed when executing the instruction with INVALID_OPCODE.
 *                  To delete all macro, set all intruction pointers of interrupt macro to NULL.  Then call Low-level
 *                  function to delete all the macro from the OC table. Then set the number of defined macro
 *                  to 0.
 * Parameters:      iOpcodeArg      - OC/Opcode of macro
 *                  iScanFlagArg    - Flag to tell delete all macro or
 *                                    delete specific macro.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MADeleteMacro(int iOpcodeArg, int iScanFlagArg);
/*************************************************
 * Function Name:   MADeleteMacroScan
 * Description:     Delete the any reference to this macro in OC table.
 *                  First get the first macro and go through the whole table to check
 *                  if the Opcode in the table = iOpcodeArg, then set the Opcode in
 *                  the table to be INVALID_OPCODE.  This function is called if delete specific
 *                  macro.
 * Parameters:      iOpocdeArg  - OC/opcode of macro to be deleted.
 * Return:          SUCCESS or FAILURE
 * *************************************************/
int    MADeletedMacroScan(int iOpcodeArg);
/*************************************************
 * Function Name:   MAFreeInstructions
 * Description:     Free memory from the instruction list.
 *                  Go through the each instruction in the instruction list and delete
 *                  all operands belong to the instruction first, then delete the instruciton.
 *                  instrArg - pointer points to the first instruction in the instruction list.
 * return:          SUCCESS or FAILURE
 * *************************************************/
int    MAFreeInstructions(instr_ptr instrArg);
/**************************************************************************************************
 * Function Name:   MAInitStack
 * Description:     Initialize macro stack also used to abort all macros.
 *                  To initialize the stack, set top of the stack to the begining of the stack,
 *                  set the PC to NULL and set the macro is not waiting.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MAInitStack(void);
/**************************************************************************************************
 * Function Name:   MRPushPc
 * Description:     Push instruction (PC and cf) onto macro stack when macro calls another macro.
 *                  Put the PC and OldPC information into the top of the stack and increment the
 *                  top of stack pointer. For multi-tasking, put the information of macro instruction
 *                  into stack of specified task. Otherwise, put the information into local stack.
 *                  When put the structure into the top of stack, need to save the top of stack into
 *                  psStkBeforeInit for RLN command to read line number if the macro is failed.
 * Note:            About the stack, the stack is an array of 30 structure of program status. It
 *                  is (LIFO) last in first out.  Program status is structure that contains
 *                  PC and OldPc (instruction pointers) and other macro status flag which is needed
 *                  to be verified with Steven The.
 *
 * return:          SUCCESS or FAILURE if stack overflow
 * **************************************************************************************************/
int    MRPushPC(pstTaskList pCurrentTaskArg);
/***********************************************************************************************
 * Function Name:       MRPopPC
 * Description:         Pop instruction (PC and cf) from macro stack when end of macro is reached.
 *                      Get macro instruction from the current top of stack and update the PC and
 *                      OldPC to continue running. If there is no instruction to pop then there is
 *                      no more instruction to run then set PC and OldPC to NULL. This function is
 *                      called in MREndMacro.  For multi-tasking, get macro instruction from stack of
 *                      specified task. Otherwise, just get from local stack.
 * Note:                About the stack, the stack is an array of 30 structure of program status. It
 *                      is (LIFO) last in first out.  Program status is structure that contains
 *                      PC and OldPc (instruction pointers) and other macro status flag which is needed
 *                      to be verified with Steven The.
 * return:              SUCCESS or FAILURE
 * ***********************************************************************************************/
int    MRPopPC(pstTaskList pCurrentTaskArg);
/**************************************************************************************************
 * Function Name:   MRLaunchMacro
 * Description:      Launching the running the macro, and push into the stack. Other word, set up
 *                  the macro instruction for mainloop to execute.
 *                  1. Check instruction pointer is NULL or not.  It's ok if
 *                     the instruction is NULL, then exit the function. it's possible that executing the
 *                     interrupt macro and it is NULL.
 *                  2. if there is not macro running since PC is NULL, then initalize the stack and put
 *                     the instruction of macro information to the top of stack.  The stack(LIFO) is
 *                     an array of prog_status_st structure pointer. Each struction contains PC,
 *                     oldPC and other macro status.  However, the important is PC and OldPC.  PC is current
 *                     insturction pointer for executing instruction.  OldPC is used for RLN command to
 *                     get the information of line number. So put instruction into the OldPC of the top of stack,
 *                     and the next instruction into the PC of the top of the stack. Also, make sure to set
 *                     the flag to be no multi-tasking and set the pointer of psStkTopBeforeInit to NULL.
 *                     psStkTopBeforeInit is pointer to save the top of the stack if there is command line error
 *                     durring executing macro.  So the RLN command can read the line that has failed at.
 *                  3. In other case, if there is something is currently running (PC is not NULL), then
 *                     get current task and push it into the stack.
 *                  4. Set up current instruction for mainloop to executing the macro instruction.
 *
 * Return:          SUCCESS or FAILURE
 * *************************************************************************************************/
int    MRLaunchMacro(instr_ptr instrArg);
/**************************************************************************************************
 * Function Name:   MRWriteMacros
 * Description:     Write the macro into file (NVSRam). This function is called when
 *                  save the macro to NVSRam.
 *                  First, write the version string and number of defined macro. Then
 *                  get the first macro in the OC table from low-level module. Then write the
 *                  contents of each macro while there's next macro in the OC table.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRWriteMacros(void);
/*************************************************
 * Function Name:   MRWriteBody
 * Description:     Write the body or content of specific macro to the file.
 *                  First, write the name of the macro. Then go through the instruction list
 *                  to count the number of instructions that the macro has. For number of instructions,
 *                  write instruction information such as Opcode, number of operands and line number.  Then
 *                  for number of operands, write contents of each operand. However, if the operand has STRING_CONST
 *                  type, then need find the string length and write the string length and string content.
 * Return:          SUCCESS or FAILURE
 * ************************************************/
int    MRWriteBody(OC_TABLE *pEntryArg);
/**************************************************************************************************
 * Function Name:   MRWriteBytes
 * Description:     Write specific number of byte to the file. Before write
 *                  to the file calculate the checksum. For checksum calculation, there is
 *                  global variable(iMod2) that got initialized to 0 before read or write to NVSram.
 *                  The checksum is calculated with alternative exclusive or each character at upper 8-bit or
 *                  lower 8-bit. if iMod2 is 1, checksum exclusive or with a character at upper 8-bit. if iMod2
 *                  is 0, checksum exclusive or with a chacter at lower 8-bit. The checksum is calculate for
 *                  the whole macro file.  Writing the checksum to file after wrote the whole macro content to file.
 *
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRWriteBytes(unsigned uNumBytesArg, void *ptrArg);
/**************************************************************************************************
 * Function Name:   MRWriteCheckSum
 * Description:     Write the check sum to the file 
 *                  It needs to make sure the function call is success by check for number of bytes that
 *                  want to write with number of bytes have read and returned. They both should be equal.
 *                  This function is called after wrote all the macro to file.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRWriteCheckSum(void);
/**************************************************************************************************
 * Function Name:   MAReadMacros
 * Description:     Read macros from the file(NVSRam).
 *                  - read the version string and compare the version string
 *                  - read number of defined macros
 *                  - read each macro with contents and insert into the table with sorting
 *                  - sorting after inserting all the macros into the table.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MAReadMacros(void);
/**************************************************************************************************
 * Function Name:   MAReadBody
 * Description:     Read the body or content of the macro from the file.
 *                  and insert the macro into the table.
 *                  - insert the macro name into the table.
 *                  - read all instructions until there is no more instruction
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MAReadBody(void);
/**************************************************************************************************
 * Function Name:   MAReadInstr
 * Description:     This function is called by MAReadBody and return instruction pointer.
 *                  Read the instruction and operands from the file:
 *                  - Allocate the memory for instruction.
 *                  - read OC, number of operands and line number and store into the instruction structure.
 *                  - Allocate the memory for operands and read the operands
 *                  - link the instruction to list.
 * return:          pointer to instr
 * **************************************************************************************************/
instr_ptr  MAReadInstr(instr_ptr prev_instrArg);
/**************************************************************************************************
 * Function Name:   MAReadBytes
 * Description:     Read specific number of bytes from the file 
 *                   Then make sure the function call read properly by checking number of
 *                  byte want to read is equal to number of bytes have read and number of bytes have read is
 *                  not zero and the return value is SUCCESS.
 * return:          SUCCESS or FAILURE
 * *************************************************/
int    MAReadBytes(unsigned uNumBytesArg, void *ptrArg);
/**************************************************************************************************
 * Function Name:   MAReadCheckSum
 * Description:     Read the checksum from the file and compare
 *                  if the checksum is not the same, then delete all
 *                  macro just read.
 * Note:            This function is not using at all in the code.  If you want to use make sure
 *                  test or watch out the for the part the delete all macros.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MAReadCheckSum(void);
/**************************************************************************************************
 * Function Name:   MASaveMacroToNvsram
 * Description:     Write the macros into nvsram(for ex_SMCR). It will write each macro contents to
 *                  macro file in NVSram.
 *                  First, to make sure NVSram checksum status is good in order to read from NVSram.
 *                  If it's bad NVSram checksum then set the checksum status bit on for macro file and
 *                  return failure. Then build the macro name
 *                  If create successfully, then write the content of the macro by calling MRWriteMacros.
 *                  In MRWriteMacros will get content of each macro from oc table and before write to the
 *                  file, it calculates the checksum.  After writing all the macros that are in the oc
 *                  table, then write the checksum to file by calling MRWriteChecksum().
 * return:          SUCCESS or FAILURE
 * *************************************************************************************************/
int    MASaveMacrotoNvsram(int iFileId);
/**************************************************************************************************
 * Function Name:   MRLoadMacroFromNvsram
 * Description:     Load the from nvsram (for ex_RMCR). It reads the macro file from NVSram and insert
 *                  each macro content into the oc table.
 *                  First, to make sure NVSram checksum status is good in order to read from NVSram.
 *                  If it's bad NVSram checksum then set the checksum status bit on for macro file and
 *                  return failure. Otherwise, delete all the existing macro in the oc table and set
 *                  checksum status bit off for macro file. Then build the macro name and test the
 *                  file checksum by calling FIOTestFileChkSum from Datafile module. If the test is passed
 *                  then open the macro file and read the content of the macro from the file by calling
 *                  MAReadMacros(). MReadMacro will build and put the macro into the oc table.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRLoadMacroFromNvsram(int iFileId);
/**************************************************************************************************
 * Function Name:   MRPreLaunchMacro
 * Description:     Set the value for SECS communicatioan only.
 *                  Implemented but not support for 6.00 I2BRT.
 * return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRPreLaunchMacro(instr_ptr instrArg, long p0, long p1, long p2, char *p3);
/**************************************************************************************************
 * Function Name:   MRExecuteMacro
 * Description:     Exectuing the macro will launch the macro and push macro information to the
 *                  stack and set up the next instruction for MRExecuteInstructions to execute by the time
 *                  in the mainloop.
 *                  To Execute the macro: Frist, get the macro entry from oc table by passing in
 *                  opcode. If it is valid then get the first macro instruction pointer from the macro
 *                  entry, so we can get the arguement of the defined macro. For Brook simulation (NOVELLUS),
 *                  we need to set all arguement information to special registers that reserved for NPAR comamnd.
 *                  (please look at the code for clean information about getting the macro arguement). Then
 *                  launch the macro to be ready to execute the next instruction in the macro and set opcode of
 *                  this specified macro to current macro.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRExecuteMacro(instr_ptr instrArg);
/**************************************************************************************************
 * Function Name:   MRExecuteInstructions
 * Description:     This the function that does the real execution of each line of macro
 *                  code or low-level command. It also prints the tracing macro if trace mode is on.
 *                  If the executing the instruction in the macro, then
 *                  setup the next instruction to PC for next execution by the time go back
 *                  to the mainloop. Before executing either macro or low-level command, it needs to
 *                  validate the opcode by get the entry from oc table. If there is tracing mode, then
 *                  list the instruction and execute either macro or low-level command. To determine either
 *                  macro or low-level command by checking the flag in the macro entry that get from oc
 *                  table. If there is error, set the status bit to be "CMD_NOT_EXED". If the macro is running
 *                  and error then set the CMER interrupt flag on. So the CMER interrupt macro can execute.
 *                  However, if there is no CMER interrupt macro, then we need to do the clean up like
 *                  MREndMacro such kill current task and set PC to NULL.  In the case of there is CMER interrupt
 *                  macro, by the time the macro finished executing at MREndMacro will kill current task and
 *                  set PC to NULL.
 * return:          SUCCESS or FAILRUE
 * **************************************************************************************************/
int    MRExecuteInstructions(instr_ptr instrArg, int iCheckPCArg);
/**************************************************************************************************
 * Function Name:   MRListInstructions
 * Description:     List the instructions is using to print out the macro code for tracing and also for
 *                  LMCR macro_name. if the trace flag is on then listing the instruction for the
 *                  trace, else it lists for LMCR command. When listing the instruction (a line of
 *                  macro code inside macro body), go through each operand and print out the operand content
 *                  according to their type. If the opcode of the instruction is INVALID_OPCODE, then print
 *                  out "????".
 *
 * Parameters:      instrArg    - instruction for listing
 *                  iTraceFlagArg - flag for checking trace mode.
 * returns:         SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRListInstructions(instr_ptr instrArg,int iTraceFlagArg);
/**************************************************************************************************
 * Function Name:   MRAbortMacro
 * Description:     Aborts macro and saves line number which aborted--used by ABM & STOP.
 *                  Abort the macro by initializing the stack. In initialization of the stack, it sets
 *                  PC to NULL with mean stop executing the current instruction. Then kill all the
 *                  tasks that are currently running and set the macro status bit to "ABORTED_BY_USER".
 *                  Also, reset the flag of CMER, SSTOP, and INTLK to not running and set the
 *                  current task id to be -1.
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MRAbortMacro(void);
/**************************************************************************************************
 *        NEW FUNCTION
 * Function Name:   MAFindOpcode
 * Description:     This function gets opcode by passing in the string name. In low-level command will
 *                  search in the oc table by string name (macro or low-level command name). If the oc entry is
 *                  returned with NULL then we know that the macro with given name does not exist. Then return the
 *                  INVALID_OPCODE; otherwise return opcode (OC).
 * Parameters:      pcOpcodeNameArg - command line
 *                  piOpcodeArg     - return opcode
 * Return:          pointer to next string in the pcOpcodeNameArg string
 * **************************************************************************************************/
char   *MAFindOpcode(char *pcOpcodeNameArg, int *ipOpcodeArg);
/**************************************************************************************************
 * Function Name:   MREndMacro
 * Description:     For ex_ENDM to call.
 *                  This function is where to indicate the ending of macro and need to do all clear up
 *                  and set up for next execution.
 *                  By the time the macro is ended, we need to do:
 *                  1. Turn off the iMacroTracing false if the tracing is on.  So by the time macro completely
 *                     finished executing, you can execute low-level command without trace line printing out.
 *                     Tracing feautre is only trace macro and not regular command line execution such as only
 *                     low-level comamnd.
 *                  2. Determine the end of macro mode (lEndMModeArg) to end the macro.  Mode is 3 usually for
 *                     interrupt macro and it will clear the macro stack and kill all the tasks after the macro is
 *                     done. On the other mode, all you need to do is to pop the instruction from the stack. The pop
 *                     function will set the current instruction either to next instruction or NULL. However, if ENDM with
 *                     mode 1, we need to check for Imform After Macro (IAM) flag to print out proper prompt with or without
 *                     status.
 *                  3. Check for the finished macro is an INTLK, SSTOP or CMER interrupt macro.
 *                     - If INTLK interrupt macro, check for any pending INTLK or SSTOP interrupt macro, then we can continue
 *                       to execute the pending interrupt macro by reset the interrupt flag to TRUE and when it gets back to
 *                       mainloop in the PCProcessMacroInstr() will continue executing the pending macro.
 *                     - If SSTOP interrupt macro, check for any pending SSTOP interrupt. If there is, then set the
 *                       interrupt flag to continue executing the pending SSTOP interrupt macro.
 *                     - If CMER interrupt macro and without multi-tasking set the PC to NULL, so current running macro is
 *                       stop and reset all the interrupt flag to default(FALSE).  Then set back the  enter interrupt section
 *                       flag to FALSE, so next interrupt can come in. For multi-tasking, if the current task is pending killed
 *                       and CMER is running, then kill current task, and the rest the same as no multi-tasking.
 *                  4. By now the PC is NULL if no pending interrupt excecuting, then initialize the psStkBeforeInit to NULL,
 *                     get the next task. If there is next task then set the interrupt timer. Otherwise, no more
 *                     multi-tasking, then reset the enter interrupt flag and set no-task-flag to TRUE.
 *
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MREndMacro(long lEndMModeArg);
/**************************************************************************************************
 * Function Name:   MAReadMacroLineNumber
 * Description:     For ex_RLN to call. It's just a debug feature for macro.
 *                  It will give the line number that macro got error while executing.
 *                  As you know every time running a macro and inside the running macro has another macro,
 *                  the instruction of the macro will be pushed to the stack.  At that time there is a variable
 *                  (psStkBeforeInit) which will save the top of stack. By the time you get the error the stack
 *                  will be initialized and psStkBeforeInit already saved the top of stack. We will use that top with
 *                  the beginning of the stack to get all the line numbers.  If the macro executed successfully, in
 *                  MREndMacro function will pop the stack, initialize the PC and OldPc to NULL and psStkBeforeInit
 *                  also set to NULL. After execute the macro, execute the RLN command. The psStkBeforeInit should be 0 now
 *                  and it will be less than the beginning of the stack when doing comparison; therefore, we just print
 *                  out the line number of local global OldPC, which should be 0.  In the case of error while executing,
 *                  we print out the line number while the beginning of the stack is less than the top of
 *                  stack (psStkBeforeInit). The last line number that is printed is also the current executed line
 *                  and error line.
 *
 * Return:          SUCCESS or FAILURE
 * **************************************************************************************************/
int    MAReadMacroLineNumber(void);


/*** Get and set function for global variables ***/

/*************************************************
 * Function Name:   MRGetPStatus
 * Description:     Get the first pointer address of program status
 * return:          first pointer to the program status
 * *************************************************/
pstProgStatus MRGetPStatus();
/*************************************************
 * Function Name:   MRGetStkTop
 * Description:     Get address of the top stack
 * return:          pointer to the stack
 * *************************************************/
pstProgStatus MRGetStkTop();
/*************************************************
 * Function Name:   MASetSavedStkTop
 * Description:     Set the address of saved stack top
 * Returns:         none.
 * *************************************************/
void    MASetSavedStkTop(pstProgStatus);
/***********************************************
 * Function Name:   MRGetCf
 * Description:     Get Carrier flag.
 *                  This flag is used to control the comparison in the macro programming.
 *                  When doing the compare the value of less than, equal or greater than is stored
 *                  in this flag, so later the conditional JUMP command can use this flag to determine
 *                  the jump.
 * Returns:         value of: > 0, <0, or 0
 * **********************************************/
long    MRGetCf(void);
/***********************************************
 * Function Name:   MASetCf
 * Description:     Set the carrier flag.
 *                  This flag is used to control the comparison in the macro programming.
 *                  When doing the compare the value of less than, equal or greater than is stored
 *                  in this flag, so later the conditional JUMP command can use this flag to determine
 *                  the jump.
 * Returns:         none.
 * ***********************************************/
void    MASetCf(long lValueArg);
/************************************************
 * Function Name:   MRGetMacroStatus
 * Description:     Get the current macro status.
 * Returns:         status (unsigned)
 * ************************************************/
int     MRGetMacroStatus();
/************************************************
 * Function Name:   MASetMacroStatus
 * Description:     Set the current macro status.
 * Returns:         status (unsigned)
 * ***********************************************/
void    MASetMacroStatus(unsigned uStatusArg);
/************************************************
 * Function Name:   MRGetPC
 * Description:     Get the current instruction (PC).
 *                  It is holding the current execution instruction.
 * returns:         instruction pointer (instr_ptr)
 ************************************************/
instr_ptr   MRGetPC(void);
/************************************************
 * Function Name:   MASetPC
 * Description:     Set the current instruction (PC).
 *                  It is holding the current execution instruction.
 * Returns:         none.
 ************************************************/
void        MASetPC(instr_ptr PCArg);
/*************************************************
 *      NEW FUNCTION
 * Function Name:   MASetFlagsToFalse
 * Description:     Resets all the interrupt flags to false when:
 *                  - Initialize the macro module.
 *                  - Start teach mode
 *                  - Start diagnostic
 *                  - Inside teach when you want to run a command such as pick, place or Calibration for scanner.
 *                  - When execting the instruction and it's fail, so the other interrupt can execute next time.
 *                    Instead of the specific interrupt keep triggering.
 *                  - In the MREndMacro, if CMER finished executing then all flags is reset for the execute interrupt to come.
 *
 * return:          SUCCESS/FAILURE
 * *************************************************/
int     MASetFlagsToFalse(void);
/*********************************************************
 *          NEW FUNCTION
 * Function Name:   MRGetMacroAvoidRecursive
 * Description:     Get the macro name that during macro
 *                  defining. So we can check for macro call itself
 *                  and lmcr itself.
 * return:          macro name
 * **********************************************************/
char*   MRGetMacroAvoidRecursive(void);
/*********************************************************
 * Function Name:   MADisplayMacroTrace
 * Description:     Display the macro code when the trace mode is on
 *                  and while the macro is executing.
 *                  First, display the line number then check if the line number
 *                  is 0, display the word "MACRO" and the rest of instruction.
 *                  For list the rest of instruction, call function MRListInstruction with
 *                  pass in the flag is TRUE which's meant the trace is on.
 * Return:          SUCCESS/FAILURE
 * **********************************************************/
int     MADisplayMacroTrace(instr_ptr instrArg);

void    MAFindPath(pstProgStatus pstkStatusStackArg, pstProgStatus pstkStatusStackTopArg, char caBufferArg[]);
void    MASetCMEFlag(int iSetValueArg);
int     MRGetCMEFlag(void);
int     MADifferentInstr(instr_ptr instrArg1, instr_ptr instrArg2);

int	MRGetFreezeMacro();
void 	MASetFreezeMacro(int i);
#endif
