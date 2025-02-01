/***************************************************************\
 *
<<<<<<< HEAD
 * Program:     Validation routines for the low level commands
 *
 * File:        LLvalid.c
=======
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * Original Author      : (Deceased)
 * Current Maintainer   : gsancosme (georges@sancosme.net)
 * Maintained Since     : 13.01.2025
 * Created On           : 04.06.2007
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Program:     Validation routines for the low level commands
 *
 * File:        cmdval.c
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
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
 *              vld_SBR
 *              vld_AxisParms
 *              vld_AxisRead
 *              vld_macro_def
 *              vld_macro_call
 *              vld_STRMAN
 *              vld_WaferParm
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

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "cmdfns.h"
#include "cmdval.h"
#include "cmdsp.h"
#include "ser.h"
#include "sctag.h"
#include "ro.h"
#include "fiog.h"
#include "scmem.h"

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
int vld_always(instr_ptr instr)
{
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_no_args
 * Descriptoin:     validation for an OpCode which needs no arguments
 * Validate for:    This function is for validating the following command(s):
 *                      EDNDTR, ABM, AMPI, DATA, DUMPC, DUMPS, DUMPW, EIS, HEAP, HNGUP,
 *                      LIS, QSCAN, RES, RESC, RLN, RMCS, SAV, WRIS, WRIW.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_no_args(instr_ptr instr)
{
    return( instr->nofopr==0 ? SUCCESS : FAILURE );
}
/****************************************************************************
 * Function Name:   vln_Stn_N
 * Description:     Validate a station name followed by a number or register.
 * Command syntax:  OPCODE <Station/Character Register>, <Number/Numeric Register>
 * Validate for:    This function is for validating the following command(s):
 *                      SRET.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Stn_N(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    if( instr->nofopr == 0 || instr->nofopr > 2 )   /* must have 1 or 2 operands */
        return FAILURE;

    Optr=instr->opr_ptr;                    /* get 1st operand */
    if(CMDisCharacter(Optr)==FAILURE)
        return FAILURE;
    if( instr->nofopr == 1 )                        /* Only 1 operand OK */
        return SUCCESS;

    return CMDisNum(Optr->next);
}
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
int vld_Stn_Np(instr_ptr instr )
{
    CMDoperand_ptr Optr;

    if( instr->nofopr < 2 )            /* if not at least 2 operands it's no good */
        return FAILURE;

    Optr=instr->opr_ptr;        /* get 1st operand */
    if(CMDisCharacter(Optr)==FAILURE)
        return FAILURE;

    return CMDisNum(Optr->next);
}
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
int vld_Axis(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char cVal;
    unsigned long ulAxis;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    /* these commands are valid only if there is 0 or 1 operand and that operand is a valid axis */
    if( (instr->OC==SVON || instr->OC==SVOF) && instr->nofopr>1 )
        return FAILURE;

    if( !(Optr=instr->opr_ptr) )        /* acceptable for no operand */
        return SUCCESS;

    switch(Optr->type)
    {
        case INDIRECT_CHAR :
            break;
        case CHAR_CONST :                   /* constant value, then validate the axis name */
            if(CMDgetCharacter(Optr, &cVal)==FAILURE)    /* get the axis name from operand */
				return FAILURE;
            switch( cVal )
            {
                case 'a' :
                    if( ROGetSpecialAxis(RO_INDEXER_T1) || ROGetSpecialAxis(RO_INDEXER_T2) ||
                        ROGetSpecialAxis(RO_INDEXER_Z1) || ROGetSpecialAxis(RO_INDEXER_Z2) )
                        return FAILURE;
                case 'A' :
                case 'T' :
                case 'R' :
                case 'Z' :
                case 't' :
                case 'r' :
                case 'z' :
                case 'W' :
                case 'w' :
                    if((ulAxis=CMDgetAxis(instr))==(unsigned long)FAILURE)
                        return FAILURE;
                    if( ROValidAxis(ulAxis, &iCardNum, &iFileType, &uGalilAxes)==FAILURE )
                        return FAILURE;
                    break;
                default:                /* invalid axis */
                    return FAILURE;
            }
            break;
        default:                        /* invalid operand type */
            return FAILURE;
    }
    return SUCCESS;
}
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
int vld_Stop(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;

    if( instr->nofopr==0 )              /* No operands, Ok */
        return SUCCESS;
    if( instr->nofopr > 2 )             /* can be no more than 2 operands */
        return FAILURE;
    if( vld_Axis(instr) == FAILURE )    /* check axis validity */
        return FAILURE;
    if( instr->nofopr==1 )              /* Only 1 operand OK */
        return SUCCESS;
    /* if the no of operand is 2, then there must be a second operand. */
    if( instr->nofopr == 2 && !(Optr=instr->opr_ptr->next) )
        return FAILURE;
    switch(Optr->type)
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if (CMDgetValue(Optr, &lVal) == FAILURE)  /* get the value */
                return FAILURE;
			if(lVal != 0 && lVal != 1)
				return FAILURE;
            break;
        default:                        /* invalid operand type for second operand  */
            return FAILURE;
    }
    return SUCCESS;
}
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
int vld_CMP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int numMode=FALSE;                  /* flag to control the valid type for comparing two operands. */
    int iIndex;

    if( instr->nofopr != 2 )            /* have to be 2 operands */
        return FAILURE;

    /* valid the type of two operands to be sure they are valid for comparison. */
    for (iIndex=0, Optr=instr->opr_ptr; iIndex < instr->nofopr; iIndex++, Optr=Optr->next)
    {
        if(!Optr)
            return FAILURE;
        switch( Optr->type )
        {
            /* character and numeric can be valid for comparision */
            case CHAR_CONST :
            case INDIRECT_CHAR :
            case LONG_CONST :
            case INDIRECT_REG :
                if(numMode)             /* to make sure the first operand is not string type */
                    return FAILURE;
                break;
            /* for string comparison, both operands have to be the same string type */
            case STRING_CONST :
            case INDIRECT_STR :
                if(iIndex == 1 && !numMode)  /* to make sure the first operand is a string also. */
                    return FAILURE;
                numMode = TRUE;
                break;
            default:                        /* for Brook emulator mode, only take constant value for comparison */
                if( CMDemulFlags&DFEMULATORB && Optr->type==BR_CONST )
                {
                    if(numMode)
                        return FAILURE;
                    numMode = FALSE;
                }
                else
                    return FAILURE;
                break;
        }
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_Num
 * Description:     Validates that there is a single operand and it is a number.
 * Validate for:    This function is for validating the following command(s):
 *                      SDCM.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Num(instr_ptr instr)
{
    if(instr->opr_ptr)
    {
        if( instr->nofopr==1 && instr->opr_ptr->type==LONG_CONST )
            return SUCCESS;
    }
    return FAILURE;
}
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
int vld_Num0(instr_ptr instr)
{
    if( instr->nofopr == 0 )            /* no operands necessary */
        return SUCCESS;
    return(vld_Num(instr));     /* if there is an operand validate that it is a number */
}
/****************************************************************************
 * Function Name:   vld_NumMCR
 * Desciption:      validates operands for the Read/Save macro file from/to NVSRAM
 * Command syntax:  OPCODE [LONG_CONST]
 * Validate for:    This function is for validating the following command(s):
 *                      RMCR, SMCR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_NumMCR(instr_ptr instr)
{
    long lVal;

    if( vld_Num0(instr) == SUCCESS )    /* if no operands or operand is a number */
    {
        if(instr->nofopr == 1)
        {
            lVal = instr->opr_ptr->opr.l;  /* check the operands numeric range  */
            if( lVal>=0 && lVal<=999 )
                return SUCCESS;
        }
        return SUCCESS;
    }
    return FAILURE;
}
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
int vld_RegNumIAMandENDM(instr_ptr instr)
{
    long lVal;

    if(instr->nofopr == 0 && instr->OC == ENDM)
        return SUCCESS;
   /* validate the operand to make sure there is only one operand and it is
    * INDIRECT_REG or LONG_CONST type.*/
    if( vld_WaferParm(instr)==SUCCESS )
    {
        /* the operand pointer is already checked from the vld_WaferParm(), so it's ok without checking
         * the operand pointer here. */
        switch( instr->opr_ptr->type )
        {
            case LONG_CONST :
                if( CMDgetValue(instr->opr_ptr,&lVal) == SUCCESS )  // get value of operand
                {
                    if( lVal>=0 && lVal<=3 )
                        return SUCCESS;
                }
                break;
            case INDIRECT_REG :         /* the value of this will be validated in the ex_IAM when executing. */
                return SUCCESS;
        }
    }
    return FAILURE;
}
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
int vld_RegNumAST(instr_ptr instr)
{
    long lVal;
    CMDoperand_ptr Optr;

    Optr = instr->opr_ptr;
    if(!Optr)
        return FAILURE;
    if( instr->nofopr == 1 || instr->nofopr == 2 )  /* 1 or 2  operands */
    {
        switch(Optr->type)
        {
            case LONG_CONST:
                if( CMDgetValue(Optr,&lVal) == SUCCESS )  /* get value and validate the value */
                {
                    if( lVal!=COM1 && lVal!=COM2 && lVal!=COM3 && lVal!=COM4)
                        return FAILURE;
                }
                else
                    return FAILURE;
                break;
            case INDIRECT_REG:
                break;
            default:
                return FAILURE;
        }
        /* there is a second operand, then make sure operand is INDIRECT_REG type */
        if(instr->nofopr == 2)
        {
            Optr = Optr->next;
            if(!Optr)
                return FAILURE;
            if(Optr->type != INDIRECT_REG)  /* only take INDIRECT_REG */
                return FAILURE;
        }
        return SUCCESS;
    }
    return FAILURE;
}
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
int vld_TaskNum(instr_ptr instr)
{
    long lVal;
    CMDoperand_ptr Optr;

    if( CMDemulFlags&DFEMULATORB )         /* no checking of operands for this type of emulator  */
        return SUCCESS;
    if( instr->nofopr == 1 )   /* must be one operand */
    {
        if((Optr = instr->opr_ptr))
        {
            switch(Optr->type)
            {
                case INDIRECT_REG :
                    break;
                case LONG_CONST :   /* get the value and validate the value from 0..9 */
                    if( CMDgetValue(instr->opr_ptr,&lVal) == FAILURE )
                        return FAILURE;
                    if( lVal<0 || lVal>=MAXTASKS )
                        return FAILURE;
                    break;
                default :
                    return FAILURE;
            }
            return SUCCESS;
        }
    }
    return FAILURE;
}
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
int vld_TaskNum0(instr_ptr instr)
{
    if( instr->nofopr == 0 )            /* if no operands OPCODE valid */
        return SUCCESS;
    return( vld_TaskNum(instr) );   /* must be an operand and it is verified */
}
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
int vld_WaferParm(instr_ptr instr)
{
    if( instr->nofopr == 1 )
        return CMDisNum(instr->opr_ptr);
    return FAILURE;
}
/****************************************************************************
 * Function Name:   vld_Waferparm1
 * Description:     Validates one number or register for wafer parameter structure
 *                  Requires one operand for validity.
 *                  NOT USED IN THIS VERSION (6.00 I2BRT).
 * Command syntax:  OPCODE <Number/Numeric Register>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_Waferparm1(instr_ptr instr)
{
    if( instr->nofopr != 1 )
        return FAILURE;
    return vld_WaferParm(instr);
}
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
int vld_Ro(instr_ptr instr)
{
    switch(instr->nofopr)
    {
        case 0 :                        /* no operands is valid */
            return SUCCESS;
        case 1 :                        /* if one operand */
            if(instr->opr_ptr)
            {
                if( instr->opr_ptr->type == INDIRECT_REG )  /* if it is an INDIRECT_REG */
                    return SUCCESS;         /* it is valid */
            }
    }
    return FAILURE;
}
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
int vld_stat(instr_ptr instr)
{
    long lVal;

    switch(instr->nofopr)
    {
        case 0 :                        /* no operands is valid */
            break;
        case 1 :                        /* if one operand */
            if( instr->opr_ptr->type == INDIRECT_REG )
                return SUCCESS;         /* it is valid */
            else if(instr->opr_ptr->type == LONG_CONST)  /* for constant value, get the value and validate the value. */
            {
                if(CMDgetValue(instr->opr_ptr, &lVal)==FAILURE)
                    return FAILURE;
                if(lVal<0 || lVal>2)    /* must be either 1 or 2 */
                    return FAILURE;
            }
            else
                return FAILURE;
            break;
        default :
            return FAILURE;
    }
    return SUCCESS;
}

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
int vld_Label(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    CMDoperand_ptr new_Optr;

    if( instr->nofopr == 1 )
    {
        Optr = instr->opr_ptr;
        /* if operand correct type and valid value */
        if( Optr->type==LONG_CONST && Optr->opr.l > 0 )
        {           /* allocate memory for new operand */
            if( (new_Optr=(CMDoperand_ptr)malloc(sizeof(CMDoperand))) == NULL )
            {
#ifdef DEBUG
                SERPutsTxBuff( SERGetCmdPort(), "\n\n*** vld_Label: Out of Memory ***\n\r?" );
#endif
                return FAILURE;
            }
            instr->nofopr = 2;      // create new operand and it's instruction
            new_Optr->type = JUMP_DEST;
            new_Optr->opr.p = NULL;
            new_Optr->next = (CMDoperand_ptr)Optr;
            instr->opr_ptr = new_Optr;
            return SUCCESS;
        }
    }
    return FAILURE;
}
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
int vld_AxisParms(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char cVal;
    unsigned long ulAxis;
    int iIndex;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    if( instr->nofopr!=2 && instr->nofopr!=4 )  /* invalid number of operands */
        return FAILURE;
    Optr = instr->opr_ptr;              /* get first operand */
    if(!Optr)
        return FAILURE;
    switch( Optr->type )                /* check first operand type */
    {
        case INDIRECT_CHAR :
             break;
        case CHAR_CONST :
            if(CMDgetCharacter(Optr,&cVal)==FAILURE)  /* get the axis name and validate */
				return FAILURE;
            switch( cVal )
            {
                case 'a' :
                    if( ROGetSpecialAxis(RO_INDEXER_T1) || ROGetSpecialAxis(RO_INDEXER_T2) ||
                        ROGetSpecialAxis(RO_INDEXER_Z1) || ROGetSpecialAxis(RO_INDEXER_Z2) )
                        return FAILURE;
                case 'A' :
                    if( instr->nofopr!=4 )
                        return FAILURE;  /* has to be 4 operands */
                    /* get the real value of according axis name and pass to MECH function to do validate if the axis exists
                     * in the system configuration. */
                    if((ulAxis=CMDgetAxis(instr))==(unsigned long)FAILURE)
                        return FAILURE;
                    if( ROValidAxis(ulAxis, &iCardNum, &iFileType, &uGalilAxes)==FAILURE )
                        return FAILURE;
                    break;
                case 'T' :
                case 'R' :
                case 'Z' :
                case 'W' :
                case 't' :
                case 'r' :
                case 'z' :
                case 'w' :
                    if( instr->nofopr!=2 )
                        return FAILURE;  /* has to be 2 operands */
                    /* get the real value of according axis name and pass to MECH function to do validate if the axis exists
                     * in the system configuration. */
                    if((ulAxis=CMDgetAxis(instr))==(unsigned long)FAILURE)
                        return FAILURE;
                    if( ROValidAxis(ulAxis, &iCardNum, &iFileType, &uGalilAxes)==FAILURE )
                        return FAILURE;
                    break;
                default:
                    return FAILURE;
            }
            break;
        default:
            return FAILURE;
    }

    /* validate each operands to make the type is either INDIRECT_REG or LONG_CONST.
     * The FOR loop will start at the second operand and index is start at 1 to represent the second operand and
     * the completion is depended on the nofopr.*/
    for (iIndex = 1; iIndex < instr->nofopr; iIndex++)
    {
        Optr = Optr->next;
        if(!Optr)
            return FAILURE;
        if( CMDisNum(Optr) == FAILURE )
            return FAILURE;
    }
    return SUCCESS;                     /* validity check successful */
}
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
int vld_MTCR(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;
    int nOperand;

    nOperand = instr->nofopr;

    if( nOperand<1 || nOperand>4 )          /* at least 1 operand and up to 4 */
        return FAILURE;                 /* 4th optional, used for scanning flag */

    Optr = instr->opr_ptr;              /* get first operand and check it */
    if( CMDisCharacter(Optr) == FAILURE )
        return FAILURE;

    if( nOperand == 1 )            /* if only one operand, it is valid */
        return SUCCESS;

    Optr = Optr->next;    /* get second operand */
    if(!Optr)
        return FAILURE;
    switch(Optr->type)
    {
        case LONG_CONST:
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if( lVal < 0 || lVal > 2)                /* check range of operand */
                return FAILURE;
            break;
        case INDIRECT_REG:
            break;
        default:
            return FAILURE;
    }
    if( nOperand == 2 )            /* only two operands, and both valid */
        return SUCCESS;

   /* validate for third and fourth operand if there are available. */
    while(nOperand-- > 2)
    {
        Optr = Optr->next;
        if(!Optr)
            return FAILURE;
        switch(Optr->type)
        {
            case LONG_CONST:
                if( CMDgetValue(Optr,&lVal)!=SUCCESS)  /* get and check operands */
                    return FAILURE;
                if(lVal !=0 && lVal != 1)
                    return FAILURE;
                break;
            case INDIRECT_REG:
                break;
            default:
                return FAILURE;
        }
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_MTCS
 * Description:     Validate for MTCS command. There is only 2 operands. Call the
 *                  function vld_MTCR() to validate the operands and value.
 * Validate for:    This function is for validating the following command(s):
 *                      MTCS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_MTCS(instr_ptr instr)
{
    if( instr->nofopr > 2 )             /* the MTCS command can have a maximum of two operands */
        return FAILURE;
    return(vld_MTCR(instr));        /* validity the same as MTCR */
}
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
int vld_AxisRead(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iMode, iCount;
    unsigned long ulAxis;
    int iCardNum, iFileType;
    unsigned uGalilAxes;

    if( instr->nofopr == 0 )   /* valid without parameters */
        return SUCCESS;

    Optr = instr->opr_ptr;
    if(!Optr)
        return FAILURE;
    switch( Optr->type )                /* check first operand */
    {
        case CHAR_CONST :
            switch( Optr->opr.i )
            {
                case 'a' :
                    if( ROGetSpecialAxis(RO_INDEXER_T1) || ROGetSpecialAxis(RO_INDEXER_T2) ||
                        ROGetSpecialAxis(RO_INDEXER_Z1) || ROGetSpecialAxis(RO_INDEXER_Z2) )
                        return FAILURE;
                case 'A' :
                    iMode = 1;
                    /* validate the axis in the case the axis name is not avaliable in
                     * the system configuration. In the case, durring the defining macro state
                     * the user types in the instruction which have a valid axis name but there is no
                     * available in the system configuration.*/
                    if((ulAxis = CMDgetAxis(instr))==(unsigned long)FAILURE)
                        return FAILURE;
                    if( ROValidAxis(ulAxis, &iCardNum, &iFileType, &uGalilAxes)==FAILURE )
                        return FAILURE;
                    break;
                case 'T' :
                case 'R' :
                case 'Z' :
                case 't' :
                case 'r' :
                case 'z' :
                case 'W' :
                case 'w' :
                    iMode = 2;
                    /* validate the axis in the case the axis name is not avaliable in
                     * the system configuration. In the case, durring the defining macro state
                     * the user types in the instruction which have a valid axis name but there is no
                     * available in the system configuration.*/
                    if((ulAxis = CMDgetAxis(instr))==(unsigned long)FAILURE)
                        return FAILURE;
                    if( ROValidAxis(ulAxis, &iCardNum, &iFileType, &uGalilAxes)==FAILURE )
                        return FAILURE;
                    break;
                default:
                    return FAILURE;
            }
            break;
        /* indirect type, you don't know the value at this point, so
         * we assume axis name is either for all 3 axes or one axis. Therefore
         * the number operands are either 2 or 4. */
        case INDIRECT_CHAR :
            iMode = 3;
            break;
        default:
            return FAILURE;
    }

    if( instr->nofopr == 1 )            /* only one parameter, and its valid */
        return SUCCESS;

    switch( iMode )                      /* check number of passed parameters for each mode type */
    {
        case 1 :                        /* mode 1 */
            if( instr->nofopr != 4 )    /* has to be 4 operands */
                return FAILURE;
            break;
        case 2 :                        /* mode 2 */
            if( instr->nofopr != 2 )    /* must be 2 operands */
                return FAILURE;
            break;
        case 3 :                        /* mode 3 */
            if( instr->nofopr!=2 && instr->nofopr!=4 )  /* can be either 2 or 4 operands */
                return FAILURE;
            break;
        default:
            return FAILURE;
    }
    /* start at 2nd operands to nofopr, validate the operand type which must be INDIRECT_REG. */
    for( iCount=2, Optr=Optr->next; iCount<=instr->nofopr; iCount++, Optr=Optr->next )
    {
        if(!Optr)
            return FAILURE;
        if( Optr->type != INDIRECT_REG )    /* if operand type invalid */
            return FAILURE;
    }

    return SUCCESS;                 /* checked validity of all operands successful */
}
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
int vld_StnRead(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    if( instr->nofopr == 0 )     /* if no operands passed */
        return FAILURE;

    Optr = instr->opr_ptr;              /* get first operand */
    if( CMDisCharacter(Optr) == FAILURE )  /* check operand which must be character type */
        return FAILURE;
    /* if only one operand, it is valid */
    if( instr->nofopr == 1 && instr->OC != RROP && instr->OC != RSS)
        return SUCCESS;

    Optr = Optr->next;   /* get second operand */
    if(!Optr)
        return FAILURE;
    if(instr->OC == RROP || instr->OC == RSS || instr->OC == RZP)
    {
        if( CMDisNum(Optr) == FAILURE )      /* check operand which must be numeric type */
            return FAILURE;
    }
    else
    {
        if(Optr->type != INDIRECT_REG)
            return FAILURE;
    }
    /* if only two operands, return SUCCESS */
    if( instr->nofopr == 2 && instr->OC != RSCS && instr->OC != RROP && instr->OC != RSS && instr->OC != RCS)
        return SUCCESS;

    Optr = Optr->next;                  /* get third operand */
    if(!Optr)
        return FAILURE;
    if(instr->OC == RSCS || instr->OC == RCS)
    {
        if(Optr->type != INDIRECT_REG)
            return FAILURE;
    }
    else
    {
        if( CMDisNum(Optr) == FAILURE )      /* check operand which must be numeric type */
            return FAILURE;
    }
    /* if only 3 operands, return SUCCESS */
    if( instr->nofopr == 3 && instr->OC != RSCS && instr->OC != RCS)
        return SUCCESS;

    Optr = Optr->next;                  /* get fourth operand */
    if(!Optr)
        return FAILURE;
    if( Optr->type != INDIRECT_REG )    /* must be INDIRECT_REG */
        return FAILURE;
    return SUCCESS;                     /* everything a success */
}
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
int vld_StnWrite3(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iCount;

    if( instr->nofopr != 4 )   /* check for valid number of operands */
        return FAILURE;

    Optr = instr->opr_ptr;              /* get 1st operand */
    if( CMDisCharacter(Optr) == FAILURE )    /* check operand */
        return FAILURE;

    for( iCount=1; iCount<instr->nofopr; iCount++ )  /* check 2nd-4th operands */
    {
        Optr = Optr->next;
        if( CMDisNum(Optr) == FAILURE )      /* check operand which can be INDIRECT_REG or LONG_CONST */
            return FAILURE;
    }
    return SUCCESS;                     /* everything valid, return SUCCESS */
}
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
int vld_Stn_F(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;

    if( instr->nofopr != 2 )            /* check for valid number of operands */
        return FAILURE;

    Optr = instr->opr_ptr;              /* get first operand */
    if( CMDisCharacter(Optr) == FAILURE )
        return FAILURE;

    Optr = Optr->next;                  /* get second operand */
    if (!Optr)
        return FAILURE;
    switch(Optr->type)
    {
        case LONG_CONST:
            if( CMDgetValue(Optr,&lVal) == SUCCESS )  /* validate the value of second operand */
            {
                if( lVal==0 || lVal==1 )
                    return SUCCESS;
            }
            break;
        case INDIRECT_REG:
            return SUCCESS;
    }

    return FAILURE;
}
/****************************************************************************
 * Function Name:   vld_INP_OUTP
 * Description:     Validate parameters for INPUT and OUTPUT commands. The command
 *                  has to have at least 1 operand.
 * Command syntax:
 *                  OPCODE <0-39> [, <Number/Numeric Register>]
 *                  OPCODE <A-J/L(for prealigner systems)> [, <Number/Numeric Register>]
 *                  OPCODE <Character Register> [, <Number/Numeric Register>]
 *                  OPCODE <Numeric Register> [, <Number/numeric Register>]
 * Validate for:    This function is for validating the following command(s):
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_INP_OUTP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;
    char cVal;

    if( !(Optr=instr->opr_ptr) )
        return FAILURE;

    switch( Optr->type )
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if( lVal < 0 )
                return FAILURE;
            else                        /* !brooks */
            {
                if (CMDinitFlags & DFPRE)
                {
                    if( lVal > 47 )
                        return FAILURE;  /* invalid value */
                }
                else
                {
                    if( lVal > 39 )
                        return FAILURE;  /* invalid value */
                }
            }
            break;
        case INDIRECT_CHAR :
            break;
        case CHAR_CONST :
        	if(CMDgetCharacter(Optr,&cVal)==FAILURE)
				return FAILURE;
            if( cVal <'A' )
				return FAILURE;
            else
            {
                if (CMDinitFlags & DFPRE)
                {
                    if( cVal>'L' )
                        return FAILURE;
                }
                else
                {
                    if( cVal>'J' )
                        return FAILURE;
                }
            }
            break;
        default:
            return FAILURE;
    }

    if( instr->nofopr == 1 )            /* one operand and it's valid */
        return SUCCESS;

    return CMDisNum(Optr->next);
}
/****************************************************************************
 * Function Name:   vld_INPUT
 * Description:     Validation routine used in conjunction with vld_INP_OUTP
 * Validate for:    This function is for validating the following command(s):
 *                      INPUT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_INPUT(instr_ptr instr)
{
    if( instr->nofopr!=2 && instr->nofopr!=1 )  /* can have either 1 or 2 operands*/
        return FAILURE;
    else if( instr->nofopr == 2 )        /* if 2 operands second can not be A LONG_CONST */
    {
        if( instr->opr_ptr->next->type == LONG_CONST )
            return FAILURE;
    }
    return vld_INP_OUTP(instr);
}
/****************************************************************************
 * Function Name:   vld_OUTP
 * Description:     Validation routine used in conjunction with vld_INP_OUTP
 * Validate for:    This function is for validating the following command(s):
 *                      CCT, MEAS, OUTP, TSKG.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_OUTP(instr_ptr instr)
{
    if( instr->nofopr == 2 )                /* must have 2 operands for validity */
        return vld_INP_OUTP(instr);
    return FAILURE;
}
/****************************************************************************
 * Function Name:   vld_align
 * Description:     Validation routine used in conjunction with vld_INP_OUTP
 * Validate for:    This function is for validating the following command(s):
 *                      BALN, BCOR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_align(instr_ptr instr)
{
    CMDoperand_ptr Optr;
	long lStep;

	if(instr->nofopr == 0)
	{
        if(~CMDinitFlags & DFSAP4)
			return SUCCESS;
		else
			return FAILURE;
	}
	Optr = instr->opr_ptr;	            /* first operand */
	if(CMDisNum(Optr)==FAILURE)          /* only check to make sure it is number */
		return FAILURE;
	if(instr->nofopr == 1)              /* can be one operand only */
		return SUCCESS;
	Optr = Optr->next;                  /* second operand  */
	if(!Optr || Optr->type != INDIRECT_REG)  /* must be indirect type if there is one */
		return FAILURE;

	Optr = Optr->next;                  /* third operand, if there is one. */
	if(Optr)
	{
        switch (Optr->type)
        {
            case LONG_CONST :
                if(CMDgetValue(Optr, &lStep)==FAILURE)
                    return FAILURE;
                if(lStep < 0 || lStep > 9)
                    return FAILURE;
            case INDIRECT_REG :
                break;
            default:
                return FAILURE;
        }
	}
	return SUCCESS;
}
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
int vld_RESP(instr_ptr instr)
{
    long lVal;
    CMDoperand_ptr Optr;

    if(instr->nofopr == 0)
        return SUCCESS;

    Optr = instr->opr_ptr;              /* get first operand */
    if (!Optr)
        return FAILURE;
    if( vld_Axis(instr) == FAILURE )    /* check operand */
        return FAILURE;

    if( instr->nofopr == 1 )            /* only one operand and it's INvalid */
        return SUCCESS;

    Optr = Optr->next;                  /* get second operand */
    if (!Optr)
        return FAILURE;
    switch(Optr->type)
    {
        case LONG_CONST:
            if( CMDgetValue(Optr,&lVal) == FAILURE )
                return FAILURE;
            if(lVal<0 || lVal > 15)     /* Validate the value */
                return FAILURE;
            break;
        case INDIRECT_REG:
            break;
    }
    if(instr->nofopr == 2)              /* ONLY 2 OPERANDS */
        return SUCCESS;
    Optr = Optr->next;                  /* THERE IS THIRD OPERAND */
    if(!Optr)
        return FAILURE;
    if(Optr->type != INDIRECT_REG)
        return FAILURE;
    return SUCCESS;
}
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
int vld_all_QUERY(CMDoperand_ptr Optr)
{
    if( Optr == NULL )              /* There Should be an operand */
        return FAILURE;
    do{
        switch( Optr->type )            /* check operand types */
        {
            case LONG_CONST :
            case CHAR_CONST :
            case STRING_CONST :
            case INDIRECT_REG :
            case INDIRECT_CHAR :
            case INDIRECT_STR :
                break;
            default:                    /* if invalid operand type */
                return FAILURE;
        }
        Optr = Optr->next;
    } while( Optr != NULL );            /* continue while operands to process */
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_QUERYo
 * Description:     Validate other querries and validates the operands for READC command.
 * Validate for:    This function is for validating the following command(s):
 *                      READC.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_QUERYo(instr_ptr instr)
{
    return vld_all_QUERY(instr->opr_ptr);
}
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
int vld_READ(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;

    if( (Optr=instr->opr_ptr) )        /* get first operand */
    {
        switch(Optr->type)
        {
            case INDIRECT_REG :
                return SUCCESS;
            case LONG_CONST :
                if( CMDgetValue(Optr,&lVal) == SUCCESS )  /* get the value */
                {
                    if( lVal==COM1 || lVal==COM2 || lVal==COM3 || lVal==COM4 || lVal==NO_PORT_NUM)  /* validate operand value */
                    {
                        if( (Optr=Optr->next) )     /* get second operand and validate it */
                            return( vld_all_QUERY(Optr) );
                    }
                }
        }
    }

    return FAILURE;
}
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
int vld_QUERY(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    if( (Optr=instr->opr_ptr) && Optr->type==CHAR_CONST )  /* get operand, check type */
    {
        switch( Optr->opr.i )           /* check value of operand */
        {
            case 'D' :
            case 'H' :
                if( (Optr=Optr->next) )  /* get second operand */
                    return vld_all_QUERY(Optr);  /* validate second operand */
        }
    }
    return FAILURE;
}
/****************************************************************************
 * Function Name:   vld_CSTR
 * Description:     Validate the string manupulation OpCode CSTR
 * Command syntax:  CSTR    <character/Character Register>, <Number/Numeric Register>, <String Register>
 * Validate for:    This function is for validating the following command(s):
 *                      CSTR.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_CSTR(instr_ptr instr)
{
    CMDoperand Otype[6]={
        { CHAR_CONST,    0, 0 },
        { INDIRECT_CHAR, 0, 0 },
        { LONG_CONST,    0, 0 },
        { INDIRECT_REG,  0, 0 },
        { INDIRECT_STR,  0, 0 },
        { INDIRECT_STR,  0, 0 }
    };
    if (instr->nofopr!=3)
        return FAILURE;
    return vld_STRMAN(instr,Otype);
}
/****************************************************************************
 * Function Name:   vld_STON
 * Description:     Validate the string manipulation opCode STON
 * Command syntax:  STON    <String/String Register>, <Numeric Register>, <number/numeric Register>
 * Validate for:    This function is for validating the following command(s):
 *                      STON.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_STON(instr_ptr instr)
{
    CMDoperand Otype[6]={
        { STRING_CONST,   0, 0 },
        { INDIRECT_STR,   0, 0 },
        { INDIRECT_REG,   0, 0 },
        { INDIRECT_REG,   0, 0 },
        { LONG_CONST,     0, 0 },
        { INDIRECT_REG,   0, 0 }
    };
    if (instr->nofopr != 3)
        return FAILURE;
    return vld_STRMAN(instr,Otype);
}
/****************************************************************************
 * Function Name:   vld_NTOS
 * Description:     Validate the string manipulation OpCode NTOS
 * Command syntax:  NTOS    <number/Numeric Register>, <String Register>, <Number/numeric Register> (3rd param is radix)
 * Validate for:    This function is for validating the following command(s):
 *                      NTOS.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_NTOS(instr_ptr instr)
{
    CMDoperand Otype[6]={
        { LONG_CONST,     0, 0 },
        { INDIRECT_REG,   0, 0 },
        { INDIRECT_STR,   0, 0 },
        { INDIRECT_STR,   0, 0 },
        { LONG_CONST,     0, 0 },
        { INDIRECT_REG,   0, 0 }
    };
    if (instr->nofopr != 3)
        return FAILURE;
    return vld_STRMAN(instr,Otype);
}
/****************************************************************************
 * Function Name:   vld_SHLN
 * Description:     Validate the string manipulation OpCode SHLN
 * Command syntax:  SHLN    <number/Numeric Register>, <String Register/charater string>
 * Validate for:    This function is for validating the following command(s):
 *                      SHLN.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_SHLN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;
    int nOperand;

    nOperand = instr->nofopr;
    if(nOperand != 2)                   /* must be 2 operands */
        return FAILURE;
    if( !(Optr=instr->opr_ptr) )        /* no operand, invalid */
        return FAILURE;
    /* validate the type and value of the first operand */
    switch( Optr->type )
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if( lVal < 0 || lVal > 5)  /* must be in the range of  0..5 */
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    if( !(Optr=Optr->next) )            /* get second operand */
        return FAILURE;
    /* validate the type and value of second operand */
    switch(Optr->type)
    {
        case INDIRECT_STR:
            break;
        case STRING_CONST:
            break;
        default:
            return FAILURE;
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_RHLN
 * Description:     Validate the string manipulation OpCode RHLN
 * Command syntax:  RHLN    <number/Numeric Register>[, <String Register>]
 * Validate for:    This function is for validating the following command(s):
 *                      RHLN.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_RHLN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;
    int nOperand;

    nOperand = instr->nofopr;
    if(nOperand <1 || nOperand > 2)     /* must be 1 or 2 operands */
        return FAILURE;
    if( !(Optr=instr->opr_ptr) )        /* no operand, invalid */
        return FAILURE;
    /* validate the type and value of the first operand */
    switch( Optr->type )
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if( lVal < 0 || lVal > 5)  /* must be in the range of  0..5 */
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    if( !(Optr=Optr->next) )            /* get second operand */
        return SUCCESS;                 /* no second operand, success */
    /* validate the type and value of second operand */
    switch(Optr->type)
    {
        case INDIRECT_STR:
            break;
        default:
            return FAILURE;
    }
    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_STRC
 * Description:     Validate the string manipulation OpCode STRC
 * Command syntax:  STRC    <string/String Register>, <Number/Numeric Register>, <Character Register>
 * Validate for:    This function is for validating the following command(s):
 *                      STRC.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_STRC(instr_ptr instr)
{
    CMDoperand Otype[8]={
        { STRING_CONST,   0, 0 },
        { INDIRECT_STR,   0, 0 },
        { LONG_CONST,     0, 0 },
        { INDIRECT_REG,   0, 0 },
        { INDIRECT_CHAR,  0, 0 },
        { INDIRECT_CHAR,  0, 0 },
        { LONG_CONST,     0, 0 },
        { INDIRECT_REG,   0, 0 }
    };
    if (instr->nofopr!=3 && instr->nofopr!=4)
        return FAILURE;
    return vld_STRMAN(instr,Otype);
}
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
int vld_macro_def(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    for( Optr=instr->opr_ptr; Optr; Optr=Optr->next )  /* for each operand */
    {
        switch( Optr->type )            /* validate operand type */
        {
            case MACRO_NUM :
            case MACRO_ALPHA :
            case MACRO_STR :
                break;
            default:
                return FAILURE;         /* invalid operand type */
        }
    }
    return SUCCESS;                     /* if all operands valid return SUCCESS */
}
/****************************************************************************
 * Function Name:   vld_CalTbl
 * Description:     Validation routine for RCS and SCT commands.
 *                  NOT USED IN (6.00 I2BRT)
 * Validate for:    This function is for validating the following command(s):
 *                      RCT, SCT.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_CalTbl(instr_ptr instr)
{
    CMDoperand_ptr Optr;

    if( instr->nofopr!=3 && instr->nofopr!=4 )  /* invalid number of operands */
        return FAILURE;

    for( Optr=instr->opr_ptr; Optr; Optr=Optr->next )  /* get operand */
    {
        if( CMDisNum(Optr) == FAILURE )  /* check operand */
            return FAILURE;
    }
    return SUCCESS;                     /* all operands are of valid types */
}
/****************************************************************************
 * Function Name:   vld_macro_call
 * Description:     Validate operands for a macro call. Make sure all the
 *                  argument is has the correct type and name of the macro is defined and in
 *                  OC table.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_macro_call(instr_ptr instr)
{
    CMDoperand_ptr macroPtr, currPtr;
    OC_TABLE entry;

    if( CMDcommands(CMDGET,instr->OC,&entry) != SUCCESS )  /* get corresponding Opcode */
        return FAILURE;

    currPtr = instr->opr_ptr;
    macroPtr = entry.p.macro->opr_ptr;

    if( CMDemulFlags&DFEMULATORB && !macroPtr )        /* no operands to validate */
        return SUCCESS;
    while( currPtr )
    {
        switch( currPtr->type )
        {
            case LONG_CONST :
            case MACRO_ALPHA :          /* validation if character operand */
            case CHAR_CONST :
            case INDIRECT_CHAR :
            case INDIRECT_REG :         /* validation of numeric operand */
            case MACRO_NUM :
            case STRING_CONST :         /* validation of string operand */
            case INDIRECT_STR :
            case MACRO_STR :
                break;
            case BR_CONST :
                /* if there is BR_CONST but there is no Brooks emulator mode configure, then invalid */
                if( !(DFEMULATORB&CMDemulFlags) )
                    return FAILURE;
                break;
            default:
#ifdef DEBUG
                SERPutsTxBuff( SERGetCmdPort(), "\n\n*** vld_macro_call: Bad Macro Definition ***\n\n" );
#endif
                return FAILURE;
        }
        currPtr = currPtr->next;
    }                                   /* end while loop */
    return SUCCESS;
}

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
int vld_EIM(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;
    int nOperand;

    nOperand = instr->nofopr;
    if(nOperand != 2)                   /* must be 2 operands */
        return FAILURE;
    if( !(Optr=instr->opr_ptr) )        /* no operand, invalid */
        return FAILURE;
    /* validate the type and value of the first operand */
    switch( Optr->type )
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if( lVal < 0 || lVal > 10)  /* must be in the range of  0..10 */
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    if( !(Optr=Optr->next) )            /* get second operand */
        return FAILURE;
    /* validate the type and value of second operand */
    switch(Optr->type)
    {
        case INDIRECT_REG:
            break;
        case LONG_CONST:
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if(lVal != 0 && lVal != 1)  /* must be either 0 or 1 */
                return FAILURE;
            break;
    }
    return SUCCESS;
}

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
int vld_RCSIandSCSI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lValue;

    if( instr->nofopr < 2 )             /* less than 2 operands, invalid */
        return FAILURE;

    Optr = instr->opr_ptr;              /* get 1st operand */
    if(!Optr)
       return FAILURE;
    switch( Optr->type )
    {
        case CHAR_CONST :               /* validate the value of station name in the first operand */
            if ((Optr->opr.i<65) || (Optr->opr.i>122) ||
                ((Optr->opr.i>90) && (Optr->opr.i<97)))
                return FAILURE;
            break;
        case INDIRECT_CHAR :
            break;
        default:
            return FAILURE;
    }
    Optr = Optr->next;                  /* get second operand */
    if(!Optr)
        return FAILURE;
    switch( Optr->type )
    {
        case LONG_CONST :               /* validate the item index and it must be 0..15 */
            if( CMDgetValue(Optr,&lValue) == FAILURE )
                return FAILURE;
            if(lValue < 0 || lValue >= FIO_MAX_NUM_CSI)
                return FAILURE;
            break;
        case INDIRECT_REG :
            break;
        default:
            return FAILURE;
    }
    /* for third operand */
    if(instr->nofopr == 3)
    {
        Optr = Optr->next;
        if(!Optr)
            return FAILURE;
        switch( Optr->type )
        {
            case LONG_CONST :
                if(instr->OC != SCSI)   /* if command is RCSI then it will fail */
                    return FAILURE;     /* since the third operand for RCSI have to be INDIRECT_REG type */
                break;
            case INDIRECT_REG :
                break;
            default:
                return FAILURE;
        }
    }
    return SUCCESS;
}

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
int vld_dmpas(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lVal;

    if( !(Optr=instr->opr_ptr) )        /* if no operand, invalid */
        return FAILURE;
    if(instr->nofopr != 1)              /* if not one operand, invalid */
        return FAILURE;
    /* validate the type and the value. Only validate the value if the type of
     * the operand is LONG_CONST. */
    switch( Optr->type )
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if( CMDgetValue(Optr,&lVal) != SUCCESS )
                return FAILURE;
            if( lVal < COM1 || lVal > COM3)
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}

//#ifdef SX
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
int vld_Secs(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lOprValue;
    int iCounter;

    if( instr->nofopr < 4 )             // Secs must have at least 4 operands
        return FAILURE;

    Optr = instr->opr_ptr;
    if(CMDisNum(Optr) == FAILURE)        //1 operand is LONG_CONST or INDIRECT_REG
        return FAILURE;                 //com-port
    switch( Optr->type )
    {
        case INDIRECT_REG :
            break;
        case LONG_CONST :
            if( CMDgetValue(Optr,&lOprValue) != SUCCESS )
                return FAILURE;
            if( lOprValue < COM1 || lOprValue > COM6)
                return FAILURE;
            break;
        default:
            return FAILURE;
    }

    for(iCounter = 0; iCounter < 3; iCounter++)
    {
        Optr = Optr->next;
        if(CMDisNum(Optr) == FAILURE)    //2th - 5th operands are LONG_CONST or INDIRECT_REG
            return FAILURE;             //device ID, Secs Stream, Secs Function, Parameter
    }
    return SUCCESS;
}
//#endif

/****************************************************************************
 * Function Name:   vld_cal_offset
 * Description:     to validate the syntax of those commands listing above.
 *                  These commands are for intellipick calibration data.
 * Validate for:    CDIS, RRNMO, RRPMO, SRNMO, SRPMO, RTNMO, RTPMO, STNMO, STPMO
 * Syntax:			CDIS angle, vector, [return value].
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 * NOTE for new changes:
 * 					For commands: RRNMO, RRPMO, SRNMO, FRPMO, RTNMO, RTPMO, STNMO,
 * 					STPMO, they were changed to the new format:
 * 					Here is the new format:
 * 						READ_COMMAND
 * 						- if there is no operand, then default is 0 and the read
 * 						  value will be sent to comm
 * 						READ_COMMAND <0/1>
 * 						- if there is one operand, then the operand represent the
 * 						  flag (0/1) and the read value will be sent to comm. You
 * 						  can not have the second operand in this case.
 * 						READ_COMMAND <Reg>
 * 						- If there is one operand and the operand type is Register, then
 * 						  the operand is represent the storage place for read value.
 * 						READ_COMMAND [0/1/Reg],[Reg]
 * 						- if there is 2 operands, then first operand is represent
 * 						  value of flag which can be constant or register type,
 * 						  and the second operand store return value into register.
 * 						SET_COMMAND <value/Reg>
 * 						- if there is one operand, then it is the value to be set
 * 						  to the data structure and the default for without second
 * 						  operand is for default robot arm.
 * 						SET_COMMAND <value/Reg>, <0/1>
 *						- if there are two operands, then the first operand is the value
 * 						  to be set and second operand is 0/1 for telling the value
 * 						  is for which robot arm.
 * 					The meaning of flag value (0/1):
 * 					- 0: for reading the default robot arm related value. If there is no
 * 					  dual arm robot then either specify 0 or don't specify anything which
 * 					  is the default.
 * 					- 1: for reading the second arm of robot related value.
 ****************************************************************************/
int vld_cal_offset(instr_ptr instr)
{
	CMDoperand_ptr Optr;
    int iNoOperand;
	long lValue;

	iNoOperand = instr->nofopr;
    if( (iNoOperand == 0) && ( (instr->OC == RRPMO) || (instr->OC == RRNMO) ||
            (instr->OC == RTPMO) || (instr->OC == RTNMO) ) )
    {
        return SUCCESS;
    }

    Optr = instr->opr_ptr;
    if( !Optr )
        return FAILURE;
    switch(Optr->type)   /* check 1st operand */
    {
        case LONG_CONST:
            if( (instr->OC == RRPMO) || (instr->OC == RRNMO) ||
                (instr->OC == RTPMO) || (instr->OC == RTNMO) )
            {
				/* if there is constant value then, there should be no
				 * more argument. If there is an arguement, then return failure */
                if(CMDgetValue(Optr, &lValue)==FAILURE)
					return FAILURE;
				if(lValue != 0 && lValue != 1)
					return FAILURE;
                if(instr->nofopr > 2)
                    return FAILURE;
                if(Optr->next)
                    break;
				return SUCCESS;
            }
        case INDIRECT_REG:
			if(instr->OC != CDIS && !Optr->next)
                return SUCCESS;
            break;
        default:
            return FAILURE;
    }
    if(iNoOperand >= 2)
    {
        Optr = Optr->next;
        if(!Optr)
            return FAILURE;
        if( (instr->OC == RRPMO) || (instr->OC == RRNMO) ||
                (instr->OC == RTPMO) || (instr->OC == RTNMO) )
		{
            if(Optr->type != INDIRECT_REG)
                return FAILURE;
			return SUCCESS;
		}
		else if(instr->OC == CDIS)
		{
			if(CMDisNum(Optr)==FAILURE)
				return FAILURE;
		}
		else   /* for STPMO, STNMO, SRNMO, SRPMO */
		{
    		switch(Optr->type)   /* check 1st operand */
    		{
        		case LONG_CONST:
                	if(CMDgetValue(Optr, &lValue)==FAILURE)
						return FAILURE;
					if(lValue != 0 && lValue != 1)
						return FAILURE;
        		case INDIRECT_REG:
                    return SUCCESS;
        		default:
            		return FAILURE;
    		}
		}
        /* Only for CDIS */
        Optr = Optr->next;
        if(Optr)
        {
            if(Optr->type != INDIRECT_REG)
                return FAILURE;
        }
        return SUCCESS;
    }
    return FAILURE;
}
/****************************************************************************
 * Function Name:   vld_OTFI
 * Description:     validates the SOTFI, ROTFI commands which access the OTF structure items
 * Validate for:    SOTFI, ROTFI
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_OTFI(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iNumOfOperands, iMaxOperands;

    iNumOfOperands = instr->nofopr;
    if ((iNumOfOperands < 2) || (iNumOfOperands > 5))
        return FAILURE;

    if (!(Optr = instr->opr_ptr))
        return FAILURE;

    if(CMDisCharacter(Optr) == FAILURE)              // check 1. operand, station name
        return FAILURE;
    if (!(Optr = Optr->next))
        return FAILURE;
    if(Optr->type == INDIRECT_REG)
    {
        return SUCCESS;
    }
    else if(Optr->type == LONG_CONST)
    {
        switch(Optr->opr.l)
        {
            case 1 :
            case 2 :
            case 3 :
            case 4 :
            case 5 :
                iMaxOperands = 3;
                break;
            case 0 :
            case 6 :
            case 7 :
                iMaxOperands = 4;
                if(!(Optr = Optr->next))
                    return FAILURE;
                if(CMDisNum(Optr) == FAILURE)
                    return FAILURE;
                break;
            case 8 :
            case 9 :
                iMaxOperands = 5;
                if(!(Optr = Optr->next))
                    return FAILURE;
                if(CMDisNum(Optr) == FAILURE)
                    return FAILURE;
                if(!(Optr = Optr->next))
                    return FAILURE;
                if(CMDisNum(Optr) == FAILURE)
                    return FAILURE;
                break;
            default :
                return FAILURE;
        }

        if(iNumOfOperands == iMaxOperands)
        {
            Optr = Optr->next;
            if(CMDisNum(Optr) == FAILURE)
                return FAILURE;
        }
        if(iNumOfOperands > iMaxOperands)
            return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_OTFD
 * Description:
 * Validate for:    ROTFD
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
int vld_OTFD(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iNumOfOperands, iIndex, iCount;

    iNumOfOperands = instr->nofopr;
    if (!(Optr = instr->opr_ptr))
        return FAILURE;

    if(Optr->type == INDIRECT_REG)
    {
        return SUCCESS;
    }
    else if(Optr->type == LONG_CONST)
    {
        iIndex = Optr->opr.i;                        // mandatory 2. operand, number or R register
        if( (iIndex < 1) || (iIndex > 4) )
            return FAILURE;

        switch(iIndex)
        {
            case 1 :
                if((iNumOfOperands != 1) && (iNumOfOperands != 2))
                    return FAILURE;
                break;
            case 2 :
                if((iNumOfOperands != 2) && (iNumOfOperands != 3))
                    return FAILURE;
                if( !(Optr = Optr->next) )
                    return FAILURE;
                for (iCount=1; iCount<iNumOfOperands; iCount++, Optr=Optr->next)
                {
                    if(CMDisNum(Optr) == FAILURE)
                        return FAILURE;
                }
                break;
            case 3 :
            case 4 :
                if((iNumOfOperands < 2) || (iNumOfOperands > 4))
                    return FAILURE;
                if( !(Optr = Optr->next) )
                    return FAILURE;
                for (iCount=1; iCount<iNumOfOperands; iCount++, Optr=Optr->next)
                {
                    if(CMDisNum(Optr) == FAILURE)
                        return FAILURE;
                }
                break;
            default:
                break;
        }
    }
    else
        return FAILURE;

    return SUCCESS;
}
/****************************************************************************
 * Function Name:   vld_String
 * Description:     validates the  one argument only, and it must be type of string
 * Validate for:    DOS
 * Syntax:          DOS String/String Reg
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ****************************************************************************/
//Dos&Counter
int vld_String(instr_ptr instr)
{
    if (instr->nofopr != 1)
        return FAILURE;

    if((instr->opr_ptr->type != STRING_CONST) && (instr->opr_ptr->type != INDIRECT_STR))
        return FAILURE;

    return SUCCESS;
}
