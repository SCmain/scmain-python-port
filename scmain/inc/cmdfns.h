/***************************************************************\
 *
 *              Copyright (c) 2007 XyZ Automation, Inc.
 *
 * Program:     Structure definition for the low level commands
 *
 * File:        cmdfns.h
 *
 * Description: Structure definition for OpCode Table
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_CMDFNS_H
#define _H_CMDFNS_H

#include "sck.h"

/* General Constants    */
#define CMD_OPCODE_LENGTH         5      /* max length of opcode string */
#define CMD_LONG_CONST_LENGTH    16      /* max length of long constant */

#define  INDIRECT_OPEN         '['
#define  INDIRECT_END     ']'
#define  MACRO_PARAM        '#'

/* the values used for oc_table.flag to validate the command and macro.
 * In each low-level command or macro has it own validation flag. */
#define CMD_VALID_IN_MACRO     1           /* command valid in a macro */
#define CMD_VALID_CMDLINE      2           /* command valid from the command line */
#define CMD_VALID_IN_MOTION    4           /* command valid during machines motion */
#define CMD_DEFINE_CMD        256           /* This is a lowlevel command definition */
#define CMD_DEFINE_MACRO      512           /* This is a macro definition */

#define BR_FLAG     0x08000000L         /* brook flag */
#define BR_INV_FLAG 0xF7FFFFFFL         /* brook invert flag */
#define LLBRCONST   0x0000FFFFL
#define PARLENGTH   15
#define NONCHAR     255 + 1
#define BAL         0
#define BCOR        1
#define BASQ        2
#define ZZ2         3
#define BALI        4

#define CMD_INIT_NUM_MACROS  500         /* maximum number of entries in the oc_table for the first time allocating memory. */
#define CMD_NUM_MACRO_INC    100         /* allocate 100 entries every the oc_table is used up to the maximum of entries. */

#define INVALID_OPCODE (-1)             /* If an invalid opcode found the instruction
                                         * instruction pointer will contain this
                                         * value for the OC pointer value.  Invalid opcode means the
                                         * command is invalid or macro is not defined and it doesn't
                                         * exist in the oc_table. */
#define NOIDX (-1)                      /* The OC index table contains this value
                                         * for entries that are not in use.*/
/* Action flags for the CMDcommands functions */
#define CMDINIT          0               /* initialize the table or update the table by sorting the table */
#define CMDGET           1               /* get an entry from the oc_table. */
#define CMDPUT           2               /* insert an entry into oc_table with sorting */
#define CMDPUT_NOSORT    3               /* insert an entry into oc_table without sorting */
#define CMDDELETE        4               /* delete an entry from the oc table */
#define CMDFIRSTMACRO    5               /* get the first macro insert into the oc table. */
#define CMDNEXTMACRO     6               /* get the the next macro insert into the oc table. */
#define CMDDELALLMACRO   7               /* delete all the macro in the table. */

/* these correspond to entries in the OPCODE table and are must NOT be changed
 * without taking the repercussions into consideration */
#define EQUAL        0
#define QUERY        1
#define ENDM         2
#define LABEL        3
#define JPG          4
#define JPGE         5
#define JPL          6
#define JPLE         7
#define JPNZ         8
#define JPZ          9
#define JUMP        10
#define STAT        11
#define ENDTR       12
#define SVON        13
#define SVOF        14
#define STOP        15
#define SCSI        16
#define STAT2       17
#define RCS         18
#define GALCO       19
#define RSCS        20
#define GLST	    21

#define DRROP       22                  // everything from here down must remain at
#define DRSS        23                  // the end of the list.  All new insertions
#define DRZP        24                  // MUST be placed between GALCO and RROP
#define DSECS       25                  // and the rest should be shifted downwards
#define DRRNMO      26
#define DRRPMO      27
#define DRTNMO      28
#define DRTPMO      29
#define DCDIS       30

extern int RROP;
extern int RSS;
extern int RZP;
extern int SECS;
extern int RRNMO;
extern int RRPMO;
extern int RTNMO;
extern int RTPMO;
extern int CDIS;

extern int CMDinitFlags;
extern int CMDemulFlags;

/* define the operand type */
typedef enum CMDoperand_type_enum
{
    INDIRECT_REG,
    INDIRECT_CHAR,
    LONG_CONST,
    CHAR_CONST,
    MACRO_ALPHA,                        /* character type of macro arguement */
    MACRO_NUM,                          /* numeric type of macro arguement */
    LLMATH_OPR,                         /* math operator */
    JUMP_DEST,                          /* label and jump */
    MACRO_OPCODE,                       /* not use */
    STRING_CONST,
    INDIRECT_STR,
    MACRO_STR,                          /* string type of macro arguement */
    BR_CONST,                           /* brook type (NOVELLUS) */
} opr_type;

/* Brooks special keyword commands that are supported by Equipe software (ENUM type)*/
typedef enum par_e
{
	peSTN = NONCHAR,
	pePOS,
	peARM,
	peCOMM,
	peLOWER,
	peNSLOTS,
	pePITCH,
	peABS,
	peSLOT,
	peUA,
	peFA,
	peOFST,
	peWRIST,
	peRTRCT,
	peHISPD,
	peLOSPD,
	peSTNSENSOR,
	peCPTR,
	peIO,
	peALL,
	peMB,
	peFLOW,
	peMON,
	pePKT,
	peBKG,
	peSEQ,
	peEX,
	peRE,
	peUP,
	peDN,
	peON,
	peOFF,
	peAT,
	peAZ,
	peAH,
	peFAA,
	peFL,
	peUN,
    peVAC,
	peLASTPAR,
} par_t;

/* this structure can handle difference type of operand content, and
 * it is one of the element in the OC_table structure */
union opr_content                       /* what the operand contents could be */
{
    int  i;                             /* all register index or character operand value or long constant value*/
    long l;                             /* long constant operand value */
    struct instr_st *p;          /* pointer macro instruction or function pointer to validate routine */
    char *s;                      /* string constant operand value */
};

/* this structure holds the property of the operand. The operand has
 * type, value, next pointer to next operand.
 * Example of Operand:  R1 = R2 + 1
 *          R1, R2, +, and 1 is an operand.
 *          = is opcode key to check in the oc_table to find out that this
 *            line of instrution is valid or not. */
struct CMDoperand_st
{
    opr_type                    type;     /* type of operand */
    union opr_content           opr;      /* contents */
    struct CMDoperand_st        *next;    /* pointer to next operand */
};

typedef struct CMDoperand_st         CMDoperand;
typedef struct CMDoperand_st    *CMDoperand_ptr;

/* Definitions of the structures for the instructions. This instruction is represent one
 * line of code inside a defined macro or a command line that is typed from terminal mode by the user.
 * It has number of operands, pointer of the list of operand, line number of instruction in the defined macro,
 * pointer of next instruction and Opcode of the instruction to access to oc table. */
struct instr_st
{
    int             nofopr;                         /* number of operands */
    CMDoperand_ptr   opr_ptr;                        /* pointer to the 1st operand */
    unsigned        line_number;                    /* line number, used by macros */
    struct          instr_st  *next_instr;     /* next instruction, used by macros */
    int             OC;                             /* Opcode number, used by macros to access to oc table to get information.*/
};

typedef struct instr_st instr_t;
typedef struct instr_st  *instr_ptr;

/* EX_PTR is the typedef for the pointer to an integer funciton */
typedef int (*EX_PTR)(instr_ptr pinst);

/* VLD_PTR is the typedef for a pointer to the validation routines */
typedef int (*VLD_PTR)(instr_ptr pinist);

/* this structure hold either function pointer of validation routine or
 * pointer of first instruction of the macro. It is one of the member in the oc_table entry.*/
union oc_ptr
{
    VLD_PTR     vld;        /* pointer for the validation routine */
    instr_ptr   macro;      /* first instruction of the macro. */
};

/* oc_table structure is entry of oc table.  The table is an array of oc_table structures.
 * Each structure is represent one low-level command or macro and it contains all the information
 * of the command and macro.  It has string name of low-level command or macro, Opcode number, pointer of first
 * macro or validation routine, validation flag and pointer to execution routine.
 * NOTE: In the old firmware, there are two tables; one is for low-level commands and one is for macros.
 *       Now, every thing is put together into one table. */
struct oc_table
{
    char    OC_str[CMD_OPCODE_LENGTH+1]; /* max length of an opcode string */
    int     OC;                         /* OpCode number */
    union   oc_ptr p;                   /* pointer to the instruction or validation routine */
    int     flag;                       /* validation flag, when the command is valid. */
    EX_PTR  exec;                       /*  ptr to execution routine */
};
typedef struct oc_table OC_TABLE;

struct par_table
{
    char    par_str[PARLENGTH+1];       /* parameter string  */
    int     ipar;                       /* opcode number   */
};
typedef struct par_table par_table_t;

/********************************************************************************************
 * Function Name:   CMDcommands
 * Description:     This function is managing the whole oc_table and oc_indx. It does all the manipulation of
 *                  table such as init table with include updating and sorting the oc_table and oc_indx,
 *                  insert, delete, and get the entry from the table. Also, keep track of the location
 *                  of the first macro in the table. This does the action depending on the action that you
 *                  passed as an arguement.
 *                  - INIT action:          Sorting the table, updating the oc_indx and save the location of
 *                                          first inserted macro into the table.
 *                  - GET action:           Get the entry from the table by providing OC number or string name.
 *                  - PUT action:           First need to check the entry is not in the table. Then, assign the OC
 *                                          number depending on the put option with sort or without sort.  With sorting
 *                                          assign the available OC in the table. Without sorting assign the last one
 *                                          in the table. Before inserting if the number of entries is greater than
 *                                          the maximum table entries then we need to reallocate more memory. After inserted
 *                                          the entry into the table if action is put with sorting then sort and update the
 *                                          table; otherwise just sort.  In Addition, if the first time put to the table and
 *                                          table is empty, you need to allocation memory and initialize all OC in each entry
 *                                          to be -1 and also oc_indx table.
 *                  - DELETE action:        There are two options for deleting macro entry from the table. Either delete
 *                                          specific macro entry or delete all macro entry from the table.
 *                                          When deleting specific macro entry from the table, the table is sorted and updated
 *                                          each time.
 *                                          However, when deleting all macros entry from the table, the table is sorted and
 *                                          updated after all the entry is deleted (only one time sorting and updating).
 *                                          There is a variable to keep track of the number of deleted macro, then update the
 *                                          number of table entries after table is sorted and updated.
 *                  - FIRSTMACRO action:    Get back first macro entry that inserted into the table.
 *                  - NEXTMACRO action:     Get the next macro entry from the table.
 * Parameters:      action - An integer flag indicating the intended action
 *                           CMDINIT       =  INITIALIZE the OpCode table, no other
 *                                          parameters are required with this action
 *                           CMDPUT        =  PUT an entry to the OpCode table
 *                           CMDPUT_NOSORT =  PUT an entry at the end of the OpCode
 *                                          table.  This should ONLY be used when the
 *                                          CMDcommands function will be called with the
 *                                          CMDINIT flag afterwards.  This s/b used
 *                                          when adding many commands to the table at
 *                                          one time.
 *                           CMDDELETE     =  DELETE an entry, only the
 *                                          oc_table->OC_Str is further required for
 *                                          this type of action
 *                           CMDGET        =  Get the entry in the OpCode.
 *                                           if oc_num is valid (>=0) the corresponding
 *                                                 table entry will be returned
 *                                           otherwise the entry corresponding to the
 *                                                 oc_table->OC_Str variable will be
 *                                                 returned
 *                           CMDFIRSTMACRO =  return the first macro as the oc_entry
 *                           CMDNEXTMACRO  =  replace the macro contained in oc_entry with
 *                                          the next macro in the OpCode table, if the
 *                                          oc_entry is the last entry in the table,
 *                                          FAILURE will be returned, else SUCCESS
 *                           CMDDELALLMACRO = Delete all Macros in the OpCode Table
 *                  oc_num - The index into the OC_index table that corresponds to a
 *                           specific entry
 *                  oc_entry - A pointer to the information OpCode entry
 * OUTPUT:          oc_entry will contain a returned value if the action is CMDGET
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDcommands(int action, int oc_num, OC_TABLE *oc_entry);

/*********************************************************************************************
 * Function Name:   CMDcmndLineAction
 * Description:     Parsing the command line and validate the low-level command or
 *                  macro. This function is main interface for parsing or it is an interpreter
 *                  for command line. It is called after getting the command line from the
 *                  com port.
 *                  - First, check if the command line is empty then return BLANKLINE (-2) to
 *                    PCProcessCmdLine().
 *                  - if There is a command in the command line, get the opcode by calling
 *                    CMDget_opcode() and CMDget_opcode will return the oc_entry structure with
 *                    information of opcode and next pointer in the cmdline.
 *                  - Then make sure the oc_entry has a valid opcode.
 *                  - Then check for special opcode that we need to have special parsing since they
 *                    can not use CMDget_operand such as
 *                    - if EQUAL, then call CMDparseMath to parse the rest of instruction.
 *                    - if MACRO, then check to make sure the macro is not a defining state before
 *                      calling MADefineMacro().
 *                    - if LMCR, then call CMDparseLMCR to parse the rest of instruction.
 *                    - if MCLR, then call CMDparseMCLR to parse the rest of instruction.
 *                    Otherwise, get the number of operands in the an instruction by calling
 *                    CMDgetOperands() which will parse the operands.
 *                  - Then validate the instruction by calling CMDvalidateInstr() if the string name of
 *                    opcode is not "MACRO".
 *
 * Parameters:      cmdline - The ascii command line entered by the user.  If the
 *                            command line corresponds to a low level command
 *                            the command is acted upon.
 *                  iSecsFlag -  = 1 indicates that command is called from Secs module and in case
 *                              when on command line is macro we don't go to define macro we just
 *                              return -3
 * OUTPUT:          instr -   the instruction pointer is returned if the
 *                            operation is successful.
 * Returned Value:  SUCCESS (0)
 *                  FAILURE (-1) Not defined in the OpCode table
 *                  BLANKLINE (-2)
 *                  MACRO (-3)
 *********************************************************************************************/
int CMDcmndLineAction(char *cmdline, instr_ptr instr, int iSecsFlag);

/*********************************************************************************************
 * Function Name:   CMDinitialize
 * Description:     This function is called at initialization time in the mainloop and it will
 *                  initialize the table and insert all the low-level commands to the table and
 *                  get the initialization and emulator flag.
 *
 * Parameters:      action -     Either CMDINIT or CMDGET
 *                               LLinitFlag  An integer which is created by "oring" the
 *                               LLinitxxxx macros.
 *                               LLemulator - An integer which is created by "oring" the
 *                               LLemulatorx macros.
 *                  initFlag -   initialization flag
 *                  emulator -   emulator flag
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDinitialize( int action, int *LLinitFlag, int *LLemulator );

/*********************************************************************************************
 * Function Name:   CMDget_opcode
 * Description:     parses the command line for the opcode.  It return the entry that contains
 *                  the opcode.  If the opcode is invalid the entry will have INVALID_OPCODE.
 *                  To get the opcode: When the cmdline pointer passed in from the arguement, the cmdline
 *                  is either the whole string line that got from the com port or the cmdline already parse
 *                  out part of the string already.  However, in this function we always assume this whole line
 *                  of cmdline that got from com port.
 *                  - First, we need to remove all the space and tab if there is any.
 *                  - Then check for the '?', if it is '?' set "?" to OC_str and OC number into the
 *                    oc_entry structure and return next pointer in the cmdline.
 *                  - However, if not '?', then save the cmdline pointer at that position and move
 *                    through the string looking the next space to determine the length of operand string.
 *                  - If the first printable character is not alphabet then there is nothing in the command line.
 *                  - If there is an alphabet then moving forward until no more alphanumeric.
 *                  - Calculate and validate the length; the opcode string length can only between 1 and 5.
 *                    To calculate the length by using the position of current cmdline pointer minus
 *                    the position of save pointer that we have saved before go through the string.
 *                  - Using the saved pointer to check for the first character if it is R, C, or S.
 *                    This will check for R-register, C-register or S-register.
 *                      If it is S, set the mode = -2.
 *                      If it is C, set the mode = -1.
 *                      If it is R, set the mode = 1.
 *                      otherwise, set the mode = 0, not a register type.
 *                    The mode is to determine type of register and not register.
 *                    If the mode != 0, then it is a register. So we expect the next character in the
 *                    cmdline is digit.  If it is the digit which is an index of the register, then
 *                    validate the index upon the type of register. For S-register the index is 0..20.
 *                    For C-register and R-register the index is 0..99. Since it is register then skip
 *                    spaces if there is any; otherwise expecting the next character is '='. If it is
 *                    '=' then set OC_str to be "=" and OC number into oc_entry structure for returning.
 *                    For the case mode = 0 or the first is R, C, or S but not a register, we set the string
 *                    name of opcode into entry and get for entry from the table by calling CMDcommands function with
 *                    action CMDGET and it return back the whole oc_entry if string name opcode provided is
 *                    found in the table.
 *                  - Before return the next pointer in the cmdline and exit the function, also check for the
 *                    case if macro is at defining and it can't call itseft or LMCR itseft since we don't
 *                    support recursive call. LMCR with other macro name inside another macro is working ok.
 *                    Example:      MACRO ME
 *                                  ME          ===> this is not valid (recursive call)
 *                                  LMCR ME     ===> this is not valid
 *                                  YOU         ===> this is valid
 *                                  LMCR YOU    ===> this is valid
 *                                  ENDM
 *                 - In the case of any thing failed, return an oc_entry with
 *                      oc_entry.OC_Str = ""
 *                      oc_entry.OC  = INVALID_OPCODE ==> this is -1
 *                   and reg value is -1. In the successful case, the reg is either 0 or value of register index.
 *             NOTE: This function is called when you want to get the opcode for low-level comamnd or macro in the
 *                  table by give the string name of opcode.
 *
 * Parameters:      cmdline - the character string containing the command line
 *                  genLMCR - indicator that this function is being called from parse_LMCR with Genmark emulation
 * OUTPUT:          instr -   An instruction pointer containing the information parsed
 *                            on the command line
 *                  reg -     The Register number used as an operand for the OpCode on
 *                            the command line, this will only be returned if applicable
 * Returned Value:            The command line after a token has been popped
 *                            if FAILURE during the operation than reg = -1
 *********************************************************************************************/
char *CMDget_opcode( char *cmdline, OC_TABLE *oc_entry, int *reg, int genLMCR );

/*********************************************************
 * Function Name:   CMDfreeOperands
 * Description:     Free all the memory blocks in an operand list and also check if the
 *                  operand type is string constant, then we need to free the memory
 *                  of the constant string.
 * Parameters:      Optr - The pointer to the first operand in the list to be freed
 * Returned Value:  None
 *********************************************************/
void CMDfreeOperands(CMDoperand_ptr Optr);

/******************************************************************************************************
 * Function Name:   CMDopcodeEntryComp
 * Description:     Compares the OC_str value of two OpCode entries. OC_str is string name (either defined
 *                  macro name of low-level command name) of Opcode entry.
 * Parameters:      e1 - entry one to compare
 *                  e2 - entry two to compare
 * Returned Value:  <0 if entry one should be alphebetized before entry two
 *                  =0 if entry one and entry two equal
 *                  >0 if entry two should be alphebetized before entry one
 ******************************************************************************************************/
int CMDopcodeEntryComp(const void *e1, const void *e2);

/******************************************************************************************************
 * Function Name:   CMDopcodeGet
 * Description:     Search for 'entry' in 'table', which has 'n' entries by using
 *                  binary search.
 * Parameters:      entry - oc_table structure.
 * Returned Value:  a pointer into the passed *table
 ******************************************************************************************************/
OC_TABLE *CMDopcodeGet( OC_TABLE *entry);

/*****************************************************************************************************
 * Function Name:   CMDinitCommands
 * Description:     Initializes all the lowlevel commands in the opcode table by insert each one
 *                  into the table.
 *                  This function is called in CMDinitialize().
 * Parameters:      initFlags - initialization flags
 *                  ewmulFlags - emulation flags
 * Returned Value:  SUCCESS/FAILURE
 *****************************************************************************************************/
int CMDinitCommands(int initFlags, int emulFlags );

/*********************************************************************************************
 * Function Name:   CMDparse_PQuery
 * Description:     Parses a Prometrix A query command.
 *                  The best way to verify is to eliminate all the spaces embedded in the
 *                  command string. Then the possible syntax range is narrowed down.
 *                  The following syntax forms are all valid A query command.
 *                  The function returns TRUE if it's a valid A query command.
 * Parameters:      cmdline -   the inputted command line
 * Returned Value:  TRUE/FALSE
 *********************************************************************************************/
int CMDparse_PQuery(char *cmdline);

/*********************************************************************************************
 * Function Name:   CMDparseMath
 * Description:     Parses a math instruction by building each operand localing in
 *                  "local_oper" and transfering it to a malloc'd area after it is
 *                  scanned successfully.
 *                  For parsing the math instruction:
 *                  - First, we know that math operator is only dealing with either
 *                    R-register, C-register or S-register to store the result after
 *                    doing the calculation. So we need to get the register index and register
 *                    type to set into an operand by using the "reg" value that passed in from
 *                    function arguement. If the reg < 0, then reg is either C-register or S-register
 *                    else it is R-register.
 *                  - Initialize the number of operand (num_opr) to 0 and initialize the operand pointer
 *                    to NULL before go through the loop to parse the operator and content.
 *                  - While no error occur,
 *                          - Allocate the memory for operand .
 *                          - Link with the next pointer.
 *                          - Scan for the next operand if there is any space, then skip the space.
 *                          - Check for register type, if it is register, then get index and store
 *                            into the operand structure. If not a register, then either character constant,
 *                            string constant or long constant.  For string constant remember to allocate the
 *                            memory for the string. For the long constant there may be '+' or
 *                            '-' in the front of number. So we need to check for the '+' and '-'. If there
 *                            is either one of those then it must be long constant after that.
 *                          - increment the number of operands (num_opr).
 *                          - Allocate the memory for operand that we just parsed out from the cmdline and
 *                            link it with the operand list.
 *                          - Then the cmdline is either now at end of line ('\r') or there is operator.
 *                            If not end of the line, it must be an operator or else error.
 *                            If end of the line, then set the following to the instr structure:
 *                                  instr->opr_ptr = head_ptr   ===> set pointer in an instr pointer to
 *                                                                   the first pointer of operand list.
 *                                  instr->nofopr = num_opr     ===> set number of operand into the instr
 *                                  instr->OC = EQUAL           ===> set the opcode number to EQUAL.
 *                                  since the instructure is like this, "R1 = R2"
 *                          - validate the operator for '>>' and '<<' to make sure if there is one '>' then
 *                            then next character in the cmdline must be '>' or else invalid operator. The same
 *                            checking for '<<'.  Then set the operator into operand.opr.i. If the operator is
 *                            invalid exit the loop and free operands and return instruction with invalid opcode and
 *                            NULL operand pointer.
 *          Note:   This function is called when you knew the opcode is '='. It is called in LLcmndLIneAction().
 *          Note:   This function returns the next pointer in the cmdline that passed in from the function arguement.
 *
 * Parameters:      cmdline -   the inputted command line
 *                  instr -     used for the output
 *                  reg -       the register number of the lvalue
 * OUTPUT:          instr - pointer to the decoded instruction
 * Returned Value: SUCCESS/FAILURE
 *********************************************************************************************/
int CMDparseMath( char *cmdline, instr_ptr instr, int reg );

/*********************************************************************************************
 * Function Name:   CMDgetOperands
 * Description:     Decodes operand part of the command string.
 *                  While there is not error in parsing:
 *                  - if CR in the cmdline, then return number of operand.
 *                  - otherwise, check if there is an operand and cmdline points at ',', then
 *                    skip the spaces and tabs if there is any.
 *                    - Check if a constant character or special Novellus keyword and set the
 *                      the operand type to either BR_CONST or CHAR_CONST and set the value of
 *                      character into the operand content.
 *                    - else check if it is LONG_CONST, then check for sign '-' or '+' and follow
 *                      the digit.
 *                    - else check if it is INDIRECT_OPEN ([), then it must be R, C, or S register
 *                      and validate the index of the register depending on the type of the register.
 *                      Then there must be a INDIRECT_END (]). If not get out the loop and return
 *                      failure.
 *                    - else check for STRING_CONST. Make sure to allocate memory for string constant.
 *                  - if the operand is valid for any of those case above then, allocate the memory for
 *                    operand structure and copy the information that has stored into the local operand
 *                    and link in the operand list.
 *                  So we keep parsing utill the end of line or error and exit loop and function.
 *                  If there is an instruction that will be returned will have OC with INVALID_OPCODE value
 *                  and operand pointer is NULL.
 *          Note:   This function is called when parsing the command line and it is called in CMDcmndLineAction()
 *                  and MADefineMacro()
 * Parameters:      cmdline - Part of the ascii command line
 * OUTPUT:          instr - The instruction pointer is returned if SUCCESS
 * Returned Value:  >=0 the number of operands found.
 *                  -1 FAILURE
 *********************************************************************************************/
int CMDgetOperands(char *cmdline, instr_ptr instr);

/*********************************************************************************************
 * Function Name:   CMDparseLMCR
 * Description:     parses arguments of an LMCR statement, CMDgetOperands() cannot be used
 *                  since it will not scan for a macro name.
 *                  For LMCR command, the command will be either:
 *                      LMCR [name of macro], [0/1]     or
 *                      LMCR
 *                      0 is for not showing the line number
 *                      1 and default is showing the line number.
 *                  For parse the validate the LMCR:
 *                  - First, the pointer of cmdline passed in from function arguement is pointing to
 *                    the next position after "LMCR" already parsed. So we need to skip all the spaces
 *                    and tabs if there is any.
 *                  - If there is end of line ('\r'), then exit the function that's mean there is no
 *                    operand.
 *                  - If there is an operand, get opcode by calling CMDget_opcode() by passing in the
 *                    cmdline pointer and we should get back the oc_entry and reg value. Then check for
 *                    valid opcode in the oc_entry and make sure this opocode is defined macro. To check
 *                    for opcode is define macro by check the validation flag in the oc_entry structure.
 *                    This command (LMCR) only accepts macro type if there is an operand.
 *                  - After got the opcode successfully, allocate memory for operand and
 *                    store information of OC and operand type into the instruction structure for return
 *                    instr that's passed from function arguement.
 *                  - Continue skip spaces or tabs. If it is end of the line then exit function.
 *                    Otherwise check for ',' and then check for next operand which is either '0' or '1'.
 *                    If it is not '0' or '1' then free memory of the operand and return failure. Otherwise
 *                    allocate memory for an operand and set the number of operand to 2 into the instruction structure
 *                    and operand type is LONG_CONST.
 *                  - Then now we expect if there is some more space or tabs and '\r', then return success; otherwise
 *                    return failure if there is still some character in the cmdline.
 *          Note:   This function is called when opcode is "LMCR" and it is called in CMDcmndLineAction().
 * Parameters:      cmdline - Part of the ascii command line entered by the user.
 * OUTPUT:          instr - The instruction pointer, returned if the operation
 *                          is successful.
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDparseLMCR( char *cmdline, instr_ptr instr );

/*********************************************************************************************
 * Function Name:   CMDgetADEoperands
 * Description:     Decodes operands part of the command string, contained in ADE format
 * NOTE:            NOT USED IN THIS VERSION (6.00 I2BRT)
 * Parameters:      cmdline - Part of the ascii command line
 * OUTPUT:          instr - The instruction pointer is returned if SUCCESS
 * Returned Value:  >=0 number of operands found
 *                  -1 FAILURE
 *********************************************************************************************/
int CMDgetADEoperands(char *cmdline, instr_ptr instr);

/*********************************************************************************************
 * Function Name:   CMDvalidateInstr
 * Description:     "Validate" a decoded instruction, insures that operands parsed by get_operands()
 *                  have types compatible with the instruction's opcode. It validates by check if the OC is
 *                  macro type then call the macro validation routine; otherwise call the low-level validation
 *                  routine.
 * Parameters:      ocEntry - The OpCode Entry that corresponds to the instruction
 *                            pointer
 *                  instr -   A pointer to the decoded instruction
 * Returned Value:  SUCCESS / FAILURE
 *********************************************************************************************/
int CMDvalidateInstr(instr_ptr instr);

/********************************************************************************************
 * Function Name:   CMDgetBRspecialString
 * Description:     get the string name of Novellus special keyword.
 * Parameters:      iIndexArg - index to access to the table.
 *                  pcReturnStringArg - return string value.
 * Returns:         SUCCESS/FAILURE
 *********************************************************************************************/
int CMDgetBRspecialString(int iIndexArg, char *pcReturnStringArg);

/*********************************************************************************************
 * Function Name:   CMDSearchBRpar
 * Description:     checks the command line for a brooks keyword by search the name of keyword
 *                  from the PAR_TABLE table. The table contains the string name of all the special
 *                  keyword for Novellus.
 *                  To search for the special keyword:
 *                  - While there is something in the command line that is not a space, comma, or CR,
 *                    advance command line pointer (or index).
 *                  - save the position and character pointing to either space or comma or CR.
 *                  - set the location of that to '\0', so we can have the string name of special
 *                    keyword ending with '\0'.
 *                  - Start beginning the special keyword table and search for the name that we parse
 *                    out from the command line. If found the name in the table, then reset the command line
 *                    as it was upon entering by setting back the saved character to the position that we saved
 *                    and return the index.  Otherwise return failure (-1).  If index is not -1 that's mean it is
 *                    Brook constant type otherwise it is character constant.
 *                  This is ONLY VALID for a brooks emulation
 * Parameters:      cmdline - Part of the ascii command line
 * OUTPUT:          BRindex - The index of the found keyword, only if SUCCESSFUL
 * Returned Value:  SUCCESS or FAILURE
 *********************************************************************************************/
int CMDSearchBRpar(char *cmdline, int *BRindex);

/*********************************************************************************************
 * Function Name:   CMDparseMCLR
 * Descripton:      parses arguments for MCLR (clear macro) statement.  The CMDgetOperands()
 *                  function could not be used, it would not scan for a macro name.
 *                  This will parse and validate the command line to make it in the following format:
 *                          MCLR *          or
 *                          MCLR <name of defined macro>
 *                  To parse and validate:
 *                  - First, if there is spaces tabs in the begin of the cmdline then skip until
 *                    there no more space or tab.
 *                  - check for either '*' or macro name.
 *                  - If there is '*' then set the flag for deleting all macro in the oc_table.
 *                  - if there is name of macro, then get the opcode and oc_entry by calling
 *                    CMDget_opcode(). Check make sure the opcode number is valid and it is
 *                    a defined macro.
 *                  - Now, the cmdline should at position end of line ('\r'). Then allocate
 *                    the memory for operand and store the information into operand structure and
 *                    instruction structure.
 *                    If '*' then set the number of operand is 1, operand type is MACRO_OPCODE, and
 *                    operand content is -255. -255 is represented the '*'. Also, set the instruction's
 *                    operand pointer points to local operand pointer that we just allocated the memory.
 *                  - The instruction pointer that is passed in from functin is arguemnet will return with
 *                    the information of operand.
 * Parameters:      cmdline - The ascii command line entered by the user, after
 *                            the MCLR command has been encountered.
 * OUTPUT:          instr -   The instruction pointer is returned in the
 *                            the operation was a success
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDparseMCLR(char *cmdline, instr_ptr instr);

/*******************************************************************************
 * Function Name:   CMDGetOpcode
 * Description:     Get the OC number to assign the oc number to new entry. You get
 *                  available location in the oc_indx table. Assume after the occupied
 *                  location is the available location.
 * Returns:         Opcode number (OC)
 *******************************************************************************/
int CMDGetOpcode(void);

/*****************************************************************************************************
 * Function Name:   CMDdelOpcodeEntry
 * Description:     Delete an entry from the table with sorting right at that time or without sorting.
 *                  The purpose for not sorting is to reduce the time for deleting all macros in the
 *                  table. It will take a lot of time if delete one entry and sorting.  However, we
 *                  will sorting table after all the macro is deleted. Also, in the middle process of
 *                  deleting each entry, we keep track the number of entry that is deleted so we can
 *                  update the number of entries in the table. The deleting without sorting is used for
 *                  "MCLR *".
 * Parameters:      iEntryNum - The location in the OC table that needs to be deleted
 *                  iSort - flag for either sorting table or not sorting the table when delete an entry.
 * Returned Value:  SUCCESS or FAILURE
 *****************************************************************************************************/
int CMDdelOpcodeEntry(int iEntryNum, int iSort);

/*****************************************************************************************************
 * Function Name:   CMDputOpcode
 * Description:     populates a structure of OC_TABLE type and puts it into the OC_TABLE
 * Parameters:      str     A character string representing the OpCode to be
 *                          inserted in the OC_TABLE
 *                  OC      The OpCode number, should be -1 (NOIDX) if a any slot
 *                          in the OC table is acceptable, if this is the case
 *                          than this value will be filled and returned.
 *                  vld     Pointer to the validation routine for this OpCode
 *                  flag    Validation flag for this entry
 *                  exec    Pointer to the routine to be executed for this OpCode
 * OUTPUT:          OC      Will be set if the value passed into this funciton was
 *                          NOIDX
 * Returned Value: SUCCESS/FAILURE
 *****************************************************************************************************/
int CMDputOpcode(char *str, int OC, VLD_PTR vld, int flag, EX_PTR exec);

/***********************************************************************************************************
 * Function Name:   CMDFirstMacro
 * Description:     Get and return the integer value corresponding to the location of the First
 *                  macro contained in the OpCode table.
 * Returns:         none.
 ***********************************************************************************************************/
void CMDFirstMacro(void);

/********************************************************
 * Function Name:   CMDopcodeUpdateIndxTbl
 * Description:     Update the OpCode Index Table. Go through the whole
 *                  Opcode index table and shipping the
 *                  entries that have valid OC and skip those
 *                  that have INVALID_OPCODE (-1). This function is called every time
 *                  sorting table, delete or insert entry to the table.
 * Returns:         none.
 *********************************************************/
void CMDopcodeUpdateIndxTbl(void);

/********************************************************************************************
 * Function Name:   CMDParseGalco
 * Description:     parses the command line containing Galil command, string begins with '\',
 *                  function parses the string from command line and simulates the operand from it
 *                  in order to be able to call already coded function CMDgetOperands
 * Parameters:      pcCmdStringArg - string from command line without '\' character
 *                  instr - instruction pointer
 *                  pcReturnStringArg - return string value.
 * Returns:         SUCCESS/FAILURE
 *********************************************************************************************/
int CMDParseGalco(char *cmdline, instr_ptr instr);

#endif
