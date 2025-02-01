/***************************************************************\
 *
 *              Copyright (c) 1999 Equipe Technologies, Inc.
 *
 * Program:     validation routines
 *
 * File:        cmdval.h
 *
 * Functions:   vld_QUERYo
 *              vld_QUERY
 *              vld_always
 *              vld_no_args
 *              vld_Ro
 *              vld_stat
 *              vld_Stn_N
 *              vld_Stn_Np
 *              vld_Stn_F
 *              vld_MTCR
 *              vld_MTCS
 *              vld_NTOS
 *              vld_STON
 *              vld_Stop
 *              vld_STRC
 *              vld_StnRead
 *              vld_READ
 *              vld_StnWrite3
 *              vld_Axis
 *              vld_RegNum
 *              vld_RegNum1
 *              vld_RegNumAST
 *              vld_RegNumIAMandENDM
 *              vld_TaskNum
 *              vld_TaskNum0
 *              vld_Label
 *              vld_CMP
 *              vld_Num
 *              vld_Num0
 *              vld_NumMCR
 *              vld_INPUT
 *              vld_INP_OUTP
 *              vld_OUTP
 *              vld_align
 *              vld_RESP
 *              vld_AxisParms
 *              vld_AxisRead
 *              vld_macro_def
 *              vld_macro_call
 *              vld_STRMAN
 *              vld_WaferParm
 *              vld_WaferParm1
 *              vld_CalTbl
 *              vld_CSTR
 *              vld_GALIL
 *              vld_EIM
 *              vld_RCSIandSCSI
 *              vld_dmpas
 *              vld_Secs
 *
 * Description: prototype of all the validation rountines for low-level commands and macro definition.
 *              The validate routine will be called to validate the commands or macro before execute the commands or macros.
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_CMDVAL_H
#define _H_CMDVAL_H

/****************************************************************************
 * Function Name:   vld_QUERYo
 * Description:     Validate other querries and validates the operands for READC command.
 * Validate for:    This function is for validating the following command(s):
 *                      READC.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_QUERYo(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_QUERY
 * Description:     Validate parameters for QUERY command.
 * Command syntax:
 *                  ? <D/H>,  <Station/Numeric Register/Character Register> [, ...]
 * Validate for:    This function is for validating the following command(s):
 *                      QUERY.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_QUERY(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_all_QUERY
 * Description:     Validate all 2-n operands passed to the query functions.
 *                  The operand type can be either LONG_CONST, CHAR_CONST, STRING_CONST,
 *                  INDIRECT_CHAR, INDIRECT_REG or INDIRECT_STR.
 * Validate for:    This function is for validating the following command(s):
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_all_QUERY(CMDoperand_ptr Optr);
/***************************************************************************
 * Function Name:   vld_always
 * Description:     Dummy validation routine which alwasys returns success.
 *                  The arguement(s) can be anything since the argrument is arbitrary.
 * Validate for:    This function is for validating the following command(s):
 *                      EQUAL, BYPASS, DCHN, DM, DUMPM, EXINP, EXOUT, EXIT, GALIL, LLOP,
 *                      RROP, LMCR, MACRO, MATS, MCLR, NPAR, RETH, RMI, RSRN, SSS,
 *                      TRACE, VER, WM, WL, WMO, ZZ1, ZZ2, MTRC, MCOR, CALC, RDA,
 *                      SDA.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int vld_always(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_no_args
 * Descriptoin:     validation for an OpCode which needs no arguments
 * Validate for:    This function is for validating the following command(s):
 *                      EDNDTR, ABM, AMPI, DATA, DUMPC, DUMPS, DUMPW, EIS, HEAP, HNGUP,
 *                      LIS, QSCAN, RES, RESC, RLN, RMCS, SAV, WRIS, WRIW.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_no_args(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Ro
 * Description:     Validate operand for commands that can have no operand or
 *                  one operand(optional operand)that must be an INDIRECT_REG.
 * Command syntax:  OPCODE [INDIRECT_REG]
 * Validate for:    This function is for validating the following command(s):
 *                      ALST, GLST, RCAFP, RCASP, RCFP, RFAA,
 *                      RFT, RLS, RMAX, RMAF, RMCS, RMSF, RMT, RNCS, RWL,
 *                      RWS, RWT, RWU, TSKI, STAG, STA.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Ro(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_stat
 * Description:     Validate STAT, STAT2 and STAT3 command's arguement. The command can take no operand or
 *                  one operand. If there is an operand, then operand can INDIRECT_REG or
 *                  LONG_CONST type. If the operand is LONG_CONST type, get the value and validate
 *                  value which must be either 1 or 2.
 * Command syntax:  OPCODE [INDIRECT_REG/LONG_CONST]
 * Validate for:    This function is for validating the following command(s):
 *                      STAT, STAT2, STAT3.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_stat(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Stn_N
 * Description:     Validate a station name followed by a number or register.
 * Command syntax:  OPCODE <Station/Character Register>, <Number/Numeric Register>
 * Validate for:    This function is for validating the following command(s):
 *                      SRET.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Stn_N(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Stn_Np
 * Description:     validation routine used to check the number of parameters before checking
 *                  the parameter contents.  used in conjunction with vld_Stn_N. Some
 *                  of the commands have 2 operands and some of the commands have more than
 *                  2 operands. Therefore, this validate function only validates the first
 *                  2 operands. The rest of the operands will be validated at ex_XXX() when executing.
 * Validate for:    This function is for validating the following command(s):
 *                      FILTR, INDEX, PITCH, PSCAN, SBS, SOF, SST, SBT, SSEE.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 * ***************************************************************************/
int vld_Stn_Np(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Stn_F
 * Description:     Validate a station specification followed by an operand.
 *                  The first operand can be INDIRECT_CHAR or CHAR_CONST. The
 *                  second operand can be INDIRECT_REG or LONG_CONST and value of the
 *                  operand must be either 0 or 1.
 * Command syntax:
 *      OPCODE <A-z/a-z>, <0/1/Numeric Register>
 *      OPCODE <Character Register>, <0/1/Numeric Register>
 * Validate for:    This function is for validating the following command(s):
 *                      STROK.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Stn_F(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_MTCR
 * Description:     Validate a station followed by one or more optional flags.
 *                  This function handles the validation from 2 or 4 operands.
 *                  The first operand must be CHAR_CONST or INDIRECT_CHAR and value must
 *                  A..Z or a..z. The rest of operand type must be INDIRECT_REG or LONG_CONST.
 *                  The value of second operand must be in the range 0..2, and value of third and fourth
 *                  operand must be either 0 or 1.
 * Command systax:
 *      OPCODE <Station/Character Register> [,<0/1/2/Numeric Register>, <0/1/Numeric Register>, <0/1/Numeric register>]
 * Validate for:    This function is for validating the following command(s):
 *                      MTCR, MTPS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_MTCR(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_MTCS
 * Description:     Validate for MTCS command. There is only 2 operands. Call the
 *                  function vld_MTCR() to validate the operands and value.
 * Validate for:    This function is for validating the following command(s):
 *                      MTCS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_MTCS(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_NTOS
 * Description:     Validate the string manipulation OpCode NTOS
 * Command syntax:  NTOS    <number/Numeric Register>, <String Register>, <Number/numeric Register> (3rd param is radix)
 * Validate for:    This function is for validating the following command(s):
 *                      NTOS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_NTOS(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_STON
 * Description:     Validate the string manipulation opCode STON
 * Command syntax:  STON    <String/String Register>, <Numeric Register>, <number/numeric Register>
 * Validate for:    This function is for validating the following command(s):
 *                      STON.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_STON(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_SHLN
 * Description:     Validate the string manipulation OpCode SHLN
 * Command syntax:  SHLN    <number/Numeric Register>, <String Register/charater string>
 * Validate for:    This function is for validating the following command(s):
 *                      SHLN.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_SHLN(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_RHLN
 * Description:     Validate the string manipulation OpCode RHLN
 * Command syntax:  RHLN    <number/Numeric Register>[, <String Register>]
 * Validate for:    This function is for validating the following command(s):
 *                      RHLN.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_RHLN(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Stop
 * Description:     Validate the stop instruction, checks parameters and axis validity.
 *                  The command can be no operand but if there is operand, it can not be more
 *                  than two operands. It's ok for having one operand which must be INDIRECT_CHAR or
 *                  CHAR_CONST. Validate the axis and the value only if there operand type
 *                  is CONSTANT type; otherwise, the operand type is INDIRECT register type and the value
 *                  will be validate when the command is executed in the ex_XXX(). The second operand can
 *                  only be INDIRECT_REG or LONG_CONST.
 * Validate for:    This function is for validating the following command(s):
 *                      STOP.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Stop(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_STRC
 * Description:     Validate the string manipulation OpCode STRC
 * Command syntax:  STRC    <string/String Register>, <Number/Numeric Register>, <Character Register>
 * Validate for:    This function is for validating the following command(s):
 *                      STRC.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_STRC(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_StnRead
 * Description:     Validate the commands for station read. There must be at least 1 operand.
 *                  The operand type is either INDIRECT_CHAR or CHAR_CONST. The second and
 *                  third operand can INDIRECT_REG or LONG_CONST. If there is the fourth
 *                  operand, it must be INDIRECT_REG.
 * Command syntax:
 *                  OPCODE <A-z/a-z> [, <Numeric Register>, <Numeric Register>, <Numeric Register>]
 *                  OPCODE <Character Register> [, <Numeric Register>, <Numeric Register>, <Numeric Register>]
 * Validate for:    This function is for validating the following command(s):
 *                      RBS, RIND, RRET, RROP, RSCS, RSEE, RST, RZP, RSS, RCS,
 *                      ROF, RPI, RST, RSST, RBM, RTE.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_StnRead(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_READ
 * Description:     Validate parameters for the READ command. The command needs to
 *                  have at least 3 operands. The first operand type can be INDIRECT_REG or
 *                  LONG_CONST. The second operand type must be INDIRECT_REG. The rest of operand types
 *                  can be INDIRECT_REG or INDIRECT_STR or INDIRECT_CHAR.
 * Command syntax:
 *                  READ <1/2/3/INDIRECT_REG>, <INDIRECT_REG>, <INIDRECT_REG/INDIRECT_CHAR/INDIRECT_STR>,
 *                                                      <INDIRECT_REG/INDIRECT_CHAR/INIDRECT_STR>
 * Validate for:    This function is for validating the following command(s):
 *                      READ.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_READ(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_StnWrite3
 * Description:     Validate a station specified by 3 required operands
 * Command syntax:
 *      OPCODE  <A-Z/a-z>, <Number/Numeric Register>, <Number/Numeric Register>, <Number/Numeric Register>
 *      OPCODE <Character Register>, <Number/Numeric Register>, <Number/Numeric Register>, <Number/Numeric Register>
 * Validate for:    This function is for validating the following command(s):
 *                      SPC, SPSC.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_StnWrite3(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Axis
 * Description:     Validate one optional axis. It can be INDIRECT_REG or CONST_CHAR.
 *                  The command can be no operand or only one operand. If there is an operand,
 *                  only validate the axis name if the operand type is CHAR_CONST. Otherwise,
 *                  the value will be validated in ex_XXX() when trying to execute the command.
 * Command syntax:  OPCODE <INDIRECT_CHAR/CONST_CHAR>
 * Validate for:    This function is for validating the following command(s):
 *                      SVON, SVOF, HOMA, HOMR, HOME.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Axis(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_RegNumAST
 * Description:     Validity function for AST command.
 *                  The command can one operand or 2 operands(optional for the second operand)
 *                  and operand type can be INDIRECT_REG or LONG_CONST.
 *                  The value of operand has to be in the range of 1..3. If there is
 *                  the second operand, the operand type must be INDIRECT_REG.
 * Validate for:    This function is for validating the following command(s):
 *                      AST.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_RegNumAST(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_RegNumIAMandENDM
 * Description:     Validation routine for the IAM command, Inform aafter a macro.
 *                  The command has only one operand and operand type can be INDIRECT_REG or
 *                  LONG_CONST. The value of operand has to be in the range of 0..3.
 * Validate for:    This function is for validating the following command(s):
 *                      IAM, ENDM.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_RegNumIAMandENDM(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_TaskNum
 * Description:     Validate for a multi-task command arguement.  It must have
 *                  one operand. The operand type can be INDIRECT_REG or LONG_CONST.
 *                  The value of constant value must be in the range of 0..9--this is task id.
 * Command syntax:  OPCODE [LONG_CONST/INDIRECT_REG]
 * Validate for:    This function is called by vld_taskNum0() to validate
 *                  TSKK and TSKM commands.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_TaskNum(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_TaskNum0
 * Description:     Validate for a task command, used in conjunction
 *                  with vld_TaskNum, accepts no operands as validity for OPCODE that
 *                  can be validated without operand.
 * Validate for:    This function is for validating the following command(s):
 *                      TSKK, TSKM.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_TaskNum0(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Label
 * Description:     validate label operand and other condition and uncondition jump commands.
 *                  This routine inserts an extra operand into the instruction to hold a pointer
 *                  for the jump destination, however the extra operand is not used by the LABEL
 *                  instructions.  The extra operand is inserted before other operands in the
 *                  instruction pointer.
 * Validate for:    This function is for validating the following command(s):
 *                      LABEL, JPGE, JPL, JPLE, JPNZ, JPZ, JUMP.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Label(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_CMP
 * Description:     validate operands for the CMP statement.  There must have 2 operands.
 *                  The operand type of both operands have to be the same type for string
 *                  comparison.  However, for character or numeric and brook const it can be
 *                  mix with character and numeric.
 * Command syntax:
 *      CMP <Character Register/Station Name>, <Character Register/Station Name>
 *      CMP <Numeric Register/Number>, <Numeric Register/number>
 * Validate for:    This function is for validating the following command(s):
 *                      CMP.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_CMP(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Num
 * Description:     Validates that there is a single operand and it is a number.
 * Validate for:    This function is for validating the following command(s):
 *                      SDCM.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Num(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Num0
 * Description:     Validates operand is a number, this is used in
 *                  conjunction with the vld_Num validation routine, this routine
 *                  is called if the OPCODE does not require a numeric operand, but
 *                  could have one.
 * Validate for:    This function is for validating the following command(s):
 *                      DIAG, DUMPP, TEACH, TSKL, WRIP.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Num0(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_NumMCR
 * Desciption:      validates operands for the Read/Save macro file from/to NVSRAM
 * Command syntax:  OPCODE [LONG_CONST]
 * Validate for:    This function is for validating the following command(s):
 *                      RMCR, SMCR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_NumMCR(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_INPUT
 * Description:     Validation routine used in conjunction with vld_INP_OUTP
 * Validate for:    This function is for validating the following command(s):
 *                      INPUT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_INPUT(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_INP_OUTP
 * Description:     Validate parameters for INPUT and OUTPUT commands. The command
 *                  has to have at least 1 operand.
 * Command syntax:
 *                  OPCODE <0-39> [, <Number/Numeric Register>]
 *                  OPCODE <A-J> [, <Number/Numeric Register>]
 *                  OPCODE <Character Register> [, <Number/Numeric Register>]
 *                  OPCODE <Numeric Register> [, <Number/numeric Register>]
 * Validate for:    This function is for validating the following command(s):
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_INP_OUTP(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_OUTP
 * Description:     Validation routine used in conjunction with vld_INP_OUTP
 * Validate for:    This function is for validating the following command(s):
 *                      BALN, BCOR, CCT, MEAS, OUTP, TSKG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_OUTP(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_align
 * Description:     Validation routine used in conjunction with vld_INP_OUTP
 * Validate for:    This function is for validating the following command(s):
 *                      BALN, BCOR, BASQ.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_align(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_RESP
 * Description:     Validate operands for the RESP command. The command can have
 *                  no operand or 2 operands or 3 operands but not 1 operand.
 *                  The first operand type can be INDIRECT_CHAR or CHAR_CONST which is for
 *                  axis name. The second operand type can be INDIRECT_REG or LONG_CONST. The
 *                  third operand is optional but if there is one then the operand type must be
 *                  INDIRECT_REG.
 * Command syntax:
 *                  OPCODE <axis> [, <Number/Numeric Register>, <Number/Numeric Register>]
 *                  OPCODE <Character Register> [, <Number/Numeric Register>, <Number/Numeric Register>]
 * Validate for:    This function is for validating the following command(s):
 *                      RESP.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_RESP(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Axisparms
 * Description:     Validate operands for those commands that set the values of
 *                  the parameter data. It must have either 2 operands or 4 operands.
 * Command syntax:
 *          OPCODE  <Axis>, <INDIRECT_REG/LONG_CONST>, <INDIRECT_REG/LONG_CONST>, <INDIRECT_REG/LONG_CONST>
 *          OPCODE <T/R/Z/t/r/z/w/W>, <INDIRECT_REG/LONG_CONST>
 *          OPCODE <INDIRECT_CHAR>, <INDIRECT_REG/LONG_CONST> [, INDIRECT_REG/LONG_CONST, INDIRECT_REG/LONG_CONST]
 * Validate for:    This function is for validating the following command(s):
 *                      ER, FA, GN, HFLG, KD, KI, KP, MOVA, MOVR, RM, SAD, SAD1,
 *                      SAD2, SDL, SDL1, SDL2, SIPW, SNSL, SPSL, SSP, SSP1, SSP2,
 *                      TA, TL, ZR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_AxisParms(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_AxisRead
 * Description:     Validate an axis specification followed by optional read operands.
 *                  This function validates 2 operands or 4 operands. The first operand must
 *                  be INDIRECT_CHAR or CHAR_CONST for hold the axis name and have to be in the
 *                  system configuration.
 * Command syntax:
 *                  1) OPCODE <Axis> [, <Numeric Register>, <numeric register>, <numeric register>]
 *                  2) OPCODE <T/R/Z> [, <numeric register>]
 *                  3) OPCODE <Character Register> [, <numeric register> [, <numeric register>, <numeric register>]]
 * Validate for:    This function is for validating the following command(s):
 *                      RAD, RAD1, RAD2, RCP, RDL,  RDL1, RDL2, RER, RFA, RGN, RIPW,
 *                      RKD, RKI, RKP, RLP, RNSL, RPSL, RRM, RSP, RSP1, RSP2, RTL, RTT,
 *                      RZR, RHFLG, RTA.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_AxisRead(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_macro_def
 * Description:     Validate operands for a macro defintion
 * Command syntax:  MACRO_NAME [#V [, #V ... ] ]
 *                  only what is listed above will be checked
 * Validate for:    This function is for validating the following command(s):
 *                      MACRO.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_macro_def(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_macro_call
 * Description:     Validate operands for a macro call. Make sure all the
 *                  argument is has the correct type and name of the macro is defined and in
 *                  OC table.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_macro_call(instr_ptr instr);
/**************************************************************************************
 * Function Name:   vld_STRMAN
 * Description:     Generically validate parameters for string manipulation functions such as
 *                  CSTR, STRC, NTOS, STON. Make sure it is order of what the command expected.
 *                  Example:
 *                  CSTR: it will expect to to character/C-register, index in numeric/R-register, and S-register.
 *                        If you put in the last arguement for CSTR command with string constant then it will be failed.
 * Note:            Look at the vld_CSTR, vld_STRC, vld_NTOS and vld_STON for requirement of each command.
 * Parameters:      instr - pointer to an instruction.
 *                  Otype - operand pointer
 * Returns:         SUCCESS/FAILURE.
 **************************************************************************************/
int vld_STRMAN(instr_ptr instr, CMDoperand *Otype);
/****************************************************************************
 * Function Name:   vld_WaferParm
 * Description:     Validate operands for wafer parameter structure commands.
 * Note:            PLEASE VERIFIED THE COMMANDS OR THE FUNCTIONS THAT CALL THIS
 *                  FUNCTION TO MAKE SURE THE VALIDITY IS CORRECT. IT IS NOT USED IN
 *                  (6.00 I2BRT)
 * Command syntax:  OPCODE  [LONG_CONST/INDIRECT_REG]
 * Validate for:    This function is for validating the following command(s):
 *                      SCAFP,SCASP, SCFP, SCSP, SFAA, SFT, SMAC, SMAF, SMSC,
 *                      SMSF, SMT, SWL, SWT, SWU.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_WaferParm(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Waferparm1
 * Description:     Validates one number or register for wafer parameter structure
 *                  Requires one operand for validity.
 *                  NOT USED IN THIS VERSION (6.00 I2BRT).
 * Command syntax:  OPCODE <Number/Numeric Register>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Waferparm1(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_CalTbl
 * Description:     Validation routine for RCS and SCT commands.
 *                  NOT USED IN (6.00 I2BRT)
 * Validate for:    This function is for validating the following command(s):
 *                      RCT, SCT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_CalTbl(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_CSTR
 * Description:     Validate the string manupulation OpCode CSTR
 * Command syntax:  CSTR    <character/Character Register>, <Number/Numeric Register>, <String Register>
 * Validate for:    This function is for validating the following command(s):
 *                      CSTR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_CSTR(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_EIM
 * Description:     validate the operand for EIM command. EIM has 2 operands and they can
 *                  be INDIRECT_REG or LONG_CONST.  The value of the first operand can only
 *                  be 0..8, and the value of the second operand can only be either 0 or 1.
 * Validate for:    This function is for validating the following command(s):
 *                      EIM.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_EIM(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_RCSIandSCSI
 * Description:     Validate the operands for RCSI and SCSI commands. The command has
 *                  to have at least 2 or 3 operands. If the command is RCSI, it has to
 *                  have at least 2 operands. If the command is SCSI, it must have 3 operands.
 * Validate for:    This function is for validating the following command(s):
 *                      RCSI, SCSI.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_RCSIandSCSI(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_dmpas
 * Description:     Validate the operands for DMPAS command. There is only
 *                  one operand and type can be INDIRECT_REG or LONG_CONST.
 *                  The value of operand must be in the range of 1..3 which the
 *                  comm port id.
 * Validate for:    This function is for validating the following command(s):
 *                      DMPAS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_dmpas(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_Secs
 * Description:     Validate the operands for SECS command. The first
 *                  perand and type can be INDIRECT_REG or LONG_CONST.
 *                  The value of operand must be in the range of 1..3 which the
 *                  comm port id. But there must be at least 5 operands which
 *                  must all be numbers for registers.
 * Validate for:    This function is for validating the following command(s):
 *                      SECS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Secs(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_cal_offset
 * Description:     to validate the syntax of those commands listing above.
 *                  These commands are for intellipick calibration data.
 * Validate for:    CDIS, RRNMO, RRPMO, SRNMO, SRPMO, RTNMO, RTPMO, STNMO, STPMO
 * Syntax:			CDIS angle, vector, [return value].
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_cal_offset(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_OTFI
 * Description:
 * Validate for:    SOTFI, ROTFI
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_OTFI(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_OTFD
 * Description:
 * Validate for:    SOTFD, ROTFD
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_OTFD(instr_ptr instr);
/****************************************************************************
 * Function Name:   vld_String
 * Description:     validates the  one argument only, and it must be type of string
 * Validate for:    DOS
 * Syntax:          DOS String/String Reg
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
//Dos&Counters
int vld_String(instr_ptr instr);

#endif
