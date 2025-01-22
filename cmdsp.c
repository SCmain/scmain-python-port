/***************************************************************\
 *
 * Program:     low-level command.
 *
 * File:        LLexecSP.c
 *
 * Functions:
 *              CMDstringEqual
 *              CMDstationNumber
 *              CMDstationCharacter
 *              CMDgetStnIndx
 *              CMDgetAxis
 *              CMDgetGalilParms
 *              CMDgetStationParameters
 *              CMDgetStation
 *              CMDdumpVariables
 *              CMDdumpBuf
 *              CMDdumpLparms
 *              CMDreadLparms
 *              CMDreadPort
 *              CMDreadBackParms
 *              CMDsendSTAT
 *              CMDsendSTAT2
 *              vld_STRMAN
 *              CMDgetValue
 *              CMDgetCharacter
 *              CMDgetString
 *              CMDisNum
 *              CMDisCharacter
 *              CMDgetAxisLoc
 *              CMDgetDecryptedPassword
 *              CMDgetIndirectStrValue
 *
 * Description: special function is used for validate the low-level comamnds
 *              Support functions for the ex_XXXXXX
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/
#include <sys/io.h>
#include <malloc.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "cmdfns.h"
#include "cmdsp.h"
#include "scregg.h"
#include "ser.h"
#include "scstat.h"
#include "scio.h"
#include "ro.h"
#include "gag.h"
#include "scmac.h"
#include "sctim.h"
#include "scintr.h"
#include "fiog.h"
#include "scmem.h"

extern int giVersionPA;
extern int giSysCfgNum;
extern int giNumOfAxes;

/* These variables are used for password decryption. */
double dTimeDateTemp, dDecryptedPassword, dTemp1, dTemp2, dTemp3, dMult1;


/*******************************************************************************
 * Function Name:   CMDgetAxisLoc
 * Description:     Get the Axis Location.  This function is used in conjunction with galil functions.
 *                  This function presumes that the axis has been validated before it is called.
 * Parameters:      ulAxis - An unsigned long containing the OR'd value of the
 *                           selected ax(i/e)s
 * Returned Value:  The axis location for the first parameter to galil functions
 *                      Axis Value      Returned Value
 *                        T or t            0
 *                        R or r            1
 *                        Z or z            2
 *                          otherwise       FAILURE
 ********************************************************************************/
int CMDgetAxisLoc(ULONG ulAxis)
{
    if( ulAxis==RO_AXIS_ALL || ulAxis==RO_AXIS_all )  /* either all robot axes or all prealigner axes. */
        return 0;
    if( ulAxis==RO_AXIS_T)
		return 0;
	if( ulAxis==RO_AXIS_t)
		return 4;
	if( ulAxis==RO_AXIS_R)
		return 1;
	if( ulAxis==RO_AXIS_r)
		return 5;
	if( ulAxis==RO_AXIS_Z)
		return 2;
	if( ulAxis==RO_AXIS_z)
		return 6;
	if( ulAxis==RO_AXIS_W)
		return 3;
	if( ulAxis==RO_AXIS_w)
		return 7;
    return FAILURE;
}
/********************************************************************************
 * Function Name:   CMDgetCharacter
 * Description:     Get a character value from constant type operand or indirect
 *                  C-register type operand and return the value
 * NOTE:            make sure to check for operand is not NULL before passing the
 *                  operand pointer to this function.
 * Parameters:      Optr - Operand Pointer
 * Returned Value : The character contained in the operand pointer
 ********************************************************************************/
int CMDgetCharacter(CMDoperand_ptr Optr, char *cRetArg)
{
	if(!Optr)
		return FAILURE;
	switch(Optr->type)
	{
		case INDIRECT_CHAR :
        	if(RGGetCharacter(Optr->opr.i,cRetArg)==FAILURE)
				return FAILURE;
			break;
		case CHAR_CONST :
        	*cRetArg = (char)Optr->opr.i;    /* get a character from constant type operand. */
			break;
		default:
			return FAILURE;
	}
	return SUCCESS;
}
/********************************************************************************
 * Function Name:   CMDgetValue
 * Description:     Get a numeric value from a constant type operand or
 *                  indirect R-register type operand and return the value
 * NOTE:            make sure to check for operand is not NULL before passing the
 *                  operand pointer to this function.
 * Parameters:      Optr - Operand Pointer
 * OUTPUT:          lVal - A pointer to the value being returned
 * Returned Value:     SUCCESS or FAILURE
 ********************************************************************************/
int CMDgetValue(CMDoperand_ptr Optr, long *lVal)
{
    if(Optr)
    {
		switch(Optr->type)
		{
			case INDIRECT_REG :  /* get a numeric value from an indirect R-register type operand.*/
				if( RGGetRegister( Optr->opr.i, lVal ) != SUCCESS )
					return FAILURE;
				break;
			case LONG_CONST :     /* get the numeric value from constant type operand. */
            	*lVal = Optr->opr.l;
				break;
			default:
				return FAILURE;
		}
        return SUCCESS;
    }
    return FAILURE;
}
/********************************************************************************
 * Function Name:   CMDgetString
 * Description:     Get a string value from a constant type operand or
 *                  indirect S-register type operand and return the value
 * NOTE:            make sure to check for operand is not NULL before passing the
 *                  operand pointer to this function.
 * Parameter:       Optr - Operand Pointer
 * OUTPUT:          sRetVal - The string being returned
 * Returned Value:  SUCCESS or FAILURE
 ********************************************************************************/
int CMDgetString(CMDoperand_ptr OptrArg, char *sRetVal, int iSecFlagArg)
{
    int iReg;
    if( OptrArg )
    {
        if( OptrArg->type == STRING_CONST )  /* get a string value from constant type operand. */
        {
            strcpy(sRetVal, OptrArg->opr.s);
        }
        else
        {
            iReg = OptrArg->opr.i;      /* get the S-register index */
//#ifdef SX
            if( RGGetString(iReg,sRetVal,iSecFlagArg) == FAILURE )  /* get a string from an indirect S-register type operand.*/
                return FAILURE;
//#else
//            if( RGGetString(iReg,sRetVal,0) == FAILURE )
//                return FAILURE;
//#endif
        }
        return SUCCESS;
    }
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDstringEqual
 * Description:     An internal Low level function which performs Math assignments for strings such as
 *                  concatenate or copy a string. To perform math operation for string, the default operation
 *                  is always "+". Then while there is operands, get string and concatenate string and get another operand
 *                  which should be an operator.  If there is no more operator then just store the string value and return,
 *                  otherwise continue to concatenate the rest of string. The result needs to store into an indirect S-register.
 *
 * Parameter:       iRegister - The register number where the results are stored
 *                  Optr -      The operand pointers containing the operands and
 *                              Operations intermixed.
 * Returned Value:     SUCCESS or FAILURE
 *************************************************************************************/
int CMDstringEqual(int iRegister, CMDoperand_ptr Optr)
{
    char cMathOperation;
    char cpOperand[MAXSTRINGLENGTH * 3];
    char cpBuffer[MAXSTRINGLENGTH * 3];
    int iCmdPort, iReturn;

    iCmdPort = SERGetCmdPort();

//    cpOperand = (char *)malloc(MAXSTRINGLENGTH * 3);
//    if(!cpOperand)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***CMDstringEqual: out of memory when doing malloc for string buffer.***\n\r?");
//#endif
//        iReturn = FAILURE;
//        goto exit_point;
//    }
//    cpBuffer = (char *)malloc(MAXSTRINGLENGTH * 3);
//    if(!cpBuffer)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***CMDstringEqual: out of memory when doing malloc for string buffer.***\n\r?");
//#endif
//        iReturn = FAILURE;
//        goto exit_point;
//    }
    cpOperand[0] = 0;
    cpBuffer[0] = 0;
    cMathOperation = '+';                /* initial operation */
    while( Optr )                       /* while there are operands */
    {
        /* get the string from the operand */
        if( (iReturn=CMDgetString(Optr,cpOperand,0)) == FAILURE )
            goto exit_point;
        /* Perform the math operation */
        switch( cMathOperation )
        {
            case '+' :                  /* concatenate the string */
                if((iReturn=RGConcatStrStr(cpBuffer, cpOperand))==FAILURE)
                    goto exit_point;
                break;
            default:
                iReturn = FAILURE;
                goto exit_point;
        }
        /* get second operand and if no operand return normal. The second operand
         * must be an operator.*/
        Optr = Optr->next;
        if(!Optr)
            break;
        /* get the math operation */
        cMathOperation = Optr->opr.i;
        Optr = Optr->next;
    }
    /* set the string value into S-register at iRegister index value. */
    if( (iReturn=RGSetString(iRegister,cpBuffer,0)) == FAILURE )
        goto exit_point;

exit_point:
//    if(cpOperand)
//        free(cpOperand);
//    if(cpBuffer)
//        free(cpBuffer);
    return iReturn;
}
/********************************************************************************
 * Function Name:   CMDstationNumber
 * Description:     Convert the character to the station index, where the index is represented as
 *                  follows:
 *                          A-Z (ASCII 65- 90) has indices 0-26
 *                          a-z (ascii 97-122) has indices 26-51
 * Parameter:       c - Character representation of the station index
 * Returned Value:  The numeric index of the station
 ********************************************************************************/
int CMDstationNumber(char c)
{
    return( isupper(c) ? c-'A' : c-'a'+26 );
}
/********************************************************************************
 * Function Name:   CMDstationCharacter
 * Description:     Convert the station index to a character, where the character is represented as
 *                  follows:
 *                          A-Z (ASCII 65- 90) has indices 0-26
 *                          a-z (ascii 97-122) has indices 26-51
 * Parameter:       i - Character representation of the station index
 * Returned Value:  The character representation of the station
 ********************************************************************************/
int CMDstationCharacter(int i)
{
    return( i<26 ? i+(int)('A') : i+(int)('a')-26 );
}
/********************************************************************************
 * Funtion Name:    CMDgetStnIndx
 * Description:     Get the station name from an operand and convert to an index and return it.
 * Parameter:       Optr - An operand Pointer
 * Returned Value:  The station index which is computed as follows:
 *                  A-Z (ASCII 65- 90) will have indices 0-25 respectively
 *                  a-z (ASCII 97-122) will have indices 26-51 respectively
 *                  FAILURE will be returned if there was a problem
 ********************************************************************************/
int CMDgetStnIndx(CMDoperand_ptr Optr)
{
    char cStationIndex;

    if(CMDgetCharacter(Optr, &cStationIndex)==FAILURE)  /* get station index */
		return FAILURE;
    if( isalpha(cStationIndex) )       /* validate station index */
        return( CMDstationNumber(cStationIndex) );  /* compute offset and return */
    return FAILURE;
}
/********************************************************************************
 * Function Name:   CMDgetAxis
 * Description:     Get an optional axis specification out of the passed instruction pointer.
 *                  If instruction is SVON, SVOF and STOP commands and there is no operand, then
 *                  return 0 as for all system axes in the whole system. Example, If the system is
 *                  8 axes then the 0 is considered as for all system axes.  However, for other commands if
 *                  there is no operand, then axis will be default to either all robot axes 'A' or all
 *                  prealigner axes 'a' only.
 * Parameter:       instr - An instruction Pointer
 * Returnd Value:  The axis 0=T, 1=R, 2=Z, 3=A, t=4, r=5, z=6, z=7, or FAILURE
 ********************************************************************************/
ULONG CMDgetAxis(instr_ptr instr)
{
    CMDoperand_ptr Optr;
	char cVal;

    if( !(Optr=instr->opr_ptr) )        /* no operand, than default to 'A' or 'a' except for SVON, SVOF and STOP.*/
    {
        if ((instr->OC == SVON) ||
            (instr->OC == SVOF) ||
            (instr->OC == STOP))
        {
            return 0;                   /* only for SVON, SVOF, and STOP */
        }

	if (giVersionPA)
		return RO_AXIS_all;
	else
        	return RO_AXIS_ALL;             /* other comamnds as default to 'A' or 'a' */
    }

    /* if there is operand then return specific axis value */
    if(CMDgetCharacter(Optr,&cVal)==FAILURE)
	return FAILURE;

    switch(cVal)
    {
        case 'T' :
            // for VAC514 robots we want to control both axes in case of STOP, SVON, SVOF commands
            if(((instr->OC == SVON) || (instr->OC == SVOF) || (instr->OC == STOP)) && (CMDinitFlags == DFVAC514))
            {
                return RO_AXIS_T | RO_AXIS_R;
            }
	    if (giVersionPA)
		return RO_AXIS_t;	// PA version
	    else
            	return RO_AXIS_T;           /* robot T axis, first galil card */
        case 'R' :
            // for VAC514 robots we want to control both axes in case of STOP, SVON, SVOF commands
            if(((instr->OC == SVON) || (instr->OC == SVOF) || (instr->OC == STOP)) && (CMDinitFlags == DFVAC514))
            {
                return RO_AXIS_T | RO_AXIS_R;
            }
	    if (giVersionPA)
		return RO_AXIS_r;	// PA version
	    else
            	return RO_AXIS_R;           /* robot R axis, first galil card */
        case 'Z' :
	    if (giVersionPA)
		return RO_AXIS_z;	// PA version
	    else
            	return RO_AXIS_Z;           /* robot Z axis, first galil card */
        case 'A' :
	    if (giVersionPA)
		return RO_AXIS_all;	// PA version
	    else
            	return RO_AXIS_ALL;         /* robot all axes, first galil card */
        case 't' :
	    if(giSysCfgNum == 30) 	// I2AXO version (use t for Track)
		return RO_AXIS_W;
	    if(giNumOfAxes <= 4)
		return (unsigned long)FAILURE;
            return RO_AXIS_t;           /* prealigner t axis, second galil card */
        case 'r' :
	    if(giNumOfAxes <= 4)
		return (unsigned long)FAILURE;
            return RO_AXIS_r;           /* prealginer r axis, second galil card */
        case 'z' :
	    if(giNumOfAxes <= 4)
		return (unsigned long)FAILURE;
            return RO_AXIS_z;           /* prealigner z axis, second galil card */
        case 'a' :
	    if(giNumOfAxes <= 4)
		return (unsigned long)FAILURE;
            return RO_AXIS_all;         /* prealigner all axes, second galil card */
        case 'W' :
            return RO_AXIS_W;           /* dual arm, flip and track,first galil card.  However,
                                         * track is on first galil card if there is 4-axis system; otherwise
                                         * the track mostly is on the second galil card. */
        case 'w' :
	    if(giNumOfAxes <= 4)
		return (unsigned long)FAILURE;
            return RO_AXIS_w;           /* track, second galil card */
        default:
            return (unsigned long)FAILURE;
    }
    return (unsigned long)FAILURE;
}
/********************************************************************************
 * Function Name:   CMDgetGalilParms
 * Description:     This is an internal Lowlevel command which pulls the parameters necessary for the
 *                  galil functions from the instruction pointer. Get value of each axis from the instruction pointer
 *                  to set the information to galil.
 *                  Example:  ER A, 100,100,100. This functin will get all 3 values of three axes (T,R,Z) and put into
 *                  an array. The ex_XXX will use this array to pass into the Mechanism function (MExxx) to set the value
 *                  to the galil.
 * Parameter:       instr - The instruction Pointer
 * OUTPUT:          ulAxis - a long representation of the Axis specified
 *                  lParms - an array of longs to contain the parameters for the
 *                           galil function
 * Returned Value:     SUCCESS or FAILURE
 ********************************************************************************/
int CMDgetGalilParms(instr_ptr instr, ULONG *ulAxis, long *lParms)
{
    CMDoperand_ptr Optr;
    int iCount;

    if( (*ulAxis=CMDgetAxis(instr)) == FAILURE )  /* get axis from 1st operand */
        return FAILURE;
    if( instr->opr_ptr && (Optr=instr->opr_ptr->next) )  /* if 1st operand, then must have 2nd operand */
    {
        /* if axis is A or a, then there must be 3rd and 4th operand */
        if( *ulAxis == RO_AXIS_ALL)
        {
            for (iCount=0; iCount<3 && Optr; iCount++, Optr=Optr->next)
            {
                if( CMDgetValue(Optr,&lParms[iCount]) != SUCCESS )  /* get value of 2nd operand */
                    return FAILURE;
            }
            return SUCCESS;
        }
        else if( *ulAxis == RO_AXIS_all )
        {
            for (iCount=4; iCount<7 && Optr; iCount++, Optr=Optr->next)
            {
                if( CMDgetValue(Optr,&lParms[iCount]) != SUCCESS )  /* get value of 2nd operand */
                    return FAILURE;
            }
            return SUCCESS;
        }
        else if( CMDgetValue(Optr,&lParms[CMDgetAxisLoc(*ulAxis)]) == SUCCESS )
            return SUCCESS;                           /* T, R, or Z axis specified */
    }
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDgetStationParameters
 * Description:     Pull the station name and up to 3 parameter values from an instruction
 *                  for setting station properties. This funciton will be used in those EX_xxxx
 *                  to set the information to station.
 * NOTE:            It always has to be station name and 3 values
 *                  or station name and one value.
 *                  EXample:
 *                      PITCH A, 100.
 *                  This function will get the station name 'A' and the value is 100 and put into an array.
 *                      SPC A, 1000,1000,1000.
 *                  This function will get the station name 'A' and the 3 values of 1000 and put into an array
 *                  according to their axis.
 * Parameter:     instr - The instruction Pointer
 * OUTPUT:          lvaluePtr - The parameter Values
 * Returned Value:  The Stations index or FAILURE
 *************************************************************************************/
int CMDgetStationParameters(instr_ptr instr, long *lValuePtr)
{
    CMDoperand_ptr Optr;
    int iStation, index;

    Optr = instr->opr_ptr;
    iStation = CMDgetStnIndx(Optr);      /* get the first operand: get station name  */
    if (iStation == FAILURE)
        return FAILURE;
	/* get the value for setting, it has to be 3 values */
    for (index=0, Optr = Optr->next; index<4 && Optr; index++, Optr = Optr->next)
    {
        if( CMDgetValue(Optr, lValuePtr++) != SUCCESS )
            return FAILURE;
    }
    return iStation;
}
/*************************************************************************************
 * Function Name:   CMDgetStation
 * Description:     Get a station name from the operand pointer and validate the station
 *                  name.  Then convert the station name from alphabet to numeric and return
 *                  numeric value.
 * Note:            This function is not called anywhere in the code.
 * Parameter:       Optr - An Operand Pointer
 * Returned Value:  The Stations index
 *************************************************************************************/
int CMDgetStation(CMDoperand_ptr Optr)
{
    char cStation;

	if(CMDgetCharacter(Optr, &cStation)==FAILURE)
		return FAILURE;
    if( isalpha( (int)cStation ) )  /* get the station name which is a character */
        return CMDstationNumber(cStation);   /* convert the station name into number and return the station number */
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDdumpVariables
 * Description:     Dump the information to COM port either in hex or decimal. If radix is 16 base
 *                  then output in HEX else output in DEC. Parse out each operands and output the value of
 *                  each operand to the COM port in HEX or DEC.
 *                  This function is shared by the QUERY and DCHN commands.
 * Parameter:           Optr - Operand Pointer
 *                  iRadix - The Radix
 *                  iPort - The port number
 * Returned Value:  SUCCESS or FAILURE
 *************************************************************************************/
int CMDdumpVariables(CMDoperand_ptr Optr, int iRadix, int iPort)
{
    char sBuf[MAXSTRINGLENGTH + 1];
    long lVal;
    int iReturn;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

//    sBuf = (char *)malloc(MAXSTRINGLENGTH+1);
//    if(!sBuf)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***CMDdumpVariables: out of memory when doing malloc for string buffer.***\n\r?");
//#endif
//        return FAILURE;
//    }
    while( Optr )                       /* while operands */
    {
//        if(TIRefreshWD()==FAILURE)
//            return FAILURE;
        switch( Optr->type )            /* process the operand type correctly */
        {
            case LONG_CONST :           /* long constant value */
            case INDIRECT_REG :         /* indirect R-register value, which mean the value is in R-register. */
                if( CMDgetValue(Optr,&lVal) != SUCCESS )  /* get the numeric value */
                    goto error_exit;
                sprintf( sBuf, iRadix==16?"%lX":"%ld", lVal );        /* radix==16->output hex else output decimal (was "%ld\0") */
                if(CMDShowDescription(iPort, sBuf) == FAILURE)
                    goto error_exit;
                break;
            case CHAR_CONST :           /* character constant value */
            case INDIRECT_CHAR :        /* indirect C-register value, which mean the value is in C-register*/
				if(CMDgetCharacter(Optr, &sBuf[0])==FAILURE)  /* get a character value */
                    goto error_exit;
                sBuf[1] = '\0';
                if(CMDShowDescription(iPort, sBuf) == FAILURE)
                    goto error_exit;
                break;
            case STRING_CONST :         /* string constant value */
            case INDIRECT_STR :         /* indirect S-register value, which mean the value is in S-register */
                iReturn = CMDgetString(Optr,sBuf,0);  /* get the string value */
                if (iReturn==FAILURE)
                    goto error_exit;
                if(CMDShowDescription(iPort, sBuf) == FAILURE)
                    goto error_exit;
                break;
            default:
                goto error_exit;
        }
        Optr = Optr->next;
    }  /* end of while loop */
//    if(sBuf)
//        free(sBuf);
    return SUCCESS;

error_exit:
//    if(sBuf)
//        free(sBuf);
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDdumpBuf
 * Description:     Dump the buffer to the command port and wait for a CR in response inorder to
 *                  dump another buffer(line). This function is used to dump parameter, and  station file.
 *                  Also, in teach pedant mode, for LMCR (list macro) command, this is used to list line by
 *                  line of macro. This is new feature for teach pendant.
 * Parameter:       sBuf - The buffer to be dumped
 * Returned value:  SUCCESS or FAILURE,
 *                  where FAILURE indicates a timeout waiting for the CR
 *************************************************************************************/
int CMDdumpBuf(char *sBuf)
{
    char ch;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    if(CMDShowDescription(iCmdPort, sBuf) == FAILURE)
        return FAILURE;
    while( SERGetcRxBuff(iCmdPort,&ch,FALSE) != FAILURE )  /* wait until there is '\r' enter */
    {
        if( ch == '\r' )                /* if linefeed found then success */
            return SUCCESS;
    }
    return FAILURE;                     /* if line feed found before EOF -> SUCCESS */
}
/*************************************************************************************
 * Function Name:   CMDdumpLparms
 * Description:     Dump Long parameters value when request for all 3 axis information.
 *                  It is used a lot of place for outputing the parameter file information such
 *                  as ex_DUMPP.
 * Parameter:       lPtr - A Pointer to the parameters to be dumped
 * Returned Value:  SUCCESS or FAILURE
 *************************************************************************************/
int CMDdumpLparms(long *lPtr)
{
    char sBuf[80];

    sprintf( sBuf, "%ld,%ld,%ld\r\n", lPtr[0], lPtr[1], lPtr[2] );
    return CMDdumpBuf(sBuf);
}
/*************************************************************************************
 * Function Name:   CMDreadLparms
 * Description:     get values from the com port and output the '\r'. This function is
 *                  used for download the parameter file (WRIP).
 * OUTPUT:          lValues - the passed information with additions
 * Returned Value: SUCCESS or FAILURE
 *************************************************************************************/
int CMDreadLparms(long *lValues)
{
    char sBuf[MAXLINE];
    int iNumBytes = -1;
    int iCmdPort;
	int iReturn;

    iCmdPort = SERGetCmdPort();
//    sBuf = (char *)malloc(MAXLINE);
//    if(!sBuf)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***CMDreadLparms: out of memory when doing malloc for string buffer.***\n\r?");
//#endif
//        return FAILURE;
//    }
    if( (iReturn=SERGetsRxBuff(SERGetCmdPort(),sBuf,FALSE,&iNumBytes,TRUE)) == FAILURE )  /* get values from com port */
        goto exit_point;
//printf("cmdsp: CMDReadLParms sBuf: %s\n",sBuf);
    if( sBuf[0] )                       /* if something there */
    {
        lValues[0] = atol(strtok(sBuf,","));  /* find first non-comma and append it */
        lValues[1] = atol(strtok(NULL,","));  /* find next non-comma and append it */
        lValues[2] = atol(strtok(NULL,","));  /* find next non-comma and append it */
//printf("CMDReadLParms lValues: %d,%d,%d,%d,%d,%d,%d,%d\n",lValues[0],lValues[1],lValues[2],lValues[3],lValues[4],lValues[5],lValues[6],lValues[7]);
    }
	if((iReturn=CMDShowDescription(SERGetCmdPort(), "\r"))==FAILURE)
		goto exit_point;

exit_point:
//    if(sBuf)
//        free(sBuf);
    return iReturn;
}
/*************************************************************************************
 * Function Name:   CMDreadPort
 * Description:     A general routine to read an entire port, or just a single bit from a port.  This
 *                  routine is used by the I/0 lowlevel command only.
 * Parameters:      lBitNum - if 0-39, than this is the bit number for a bit read
 *                            if -1, than this is used to indicate a port read whole port.
 *                  cPortID - if (A-J), than read from the port, otherwise 0
 * Returned Value: The byte/bit read or FAILURE
 *************************************************************************************/
int CMDreadPort(long lBitNum, char cPortID)
{
    int iData;

    if( lBitNum>=0 && lBitNum<=47 )     /* bit read */
        cPortID = 'F' + (char)(lBitNum/8);    /*set portID */
    else if( lBitNum != -1 )            /* not a valid bit number */
        return FAILURE;

    switch(cPortID)
    {
        case 'A' : // OUTP ports
            iData = inb( IO_ROBOT_OUTPUT_A );
            break;
        case 'B' : 
            iData = inb( IO_ROBOT_OUTPUT_B );
            break;
        case 'C' : 
//            iData = inb( IO_ROBOT_OUTPUT_C );
            break;
        case 'D' : 
            iData = inb( IO_ROBOT_OUTPUT_D );
            break;
        case 'E' : 
//            iData = inb( IO_ROBOT_OUTPUT_E );
            break;
        case 'F' : // INPUT ports
            iData = inb( IO_ROBOT_INPUT_F );
            break;
        case 'G' : 
            iData = inb( IO_ROBOT_INPUT_G );
            break;
        case 'H' :
            IOSetIOIntPending(FALSE); /* signal user interrupt is cleared */
            iData = inb( IO_ROBOT_INPUT_H );
            break;
        case 'I' :
//            iData = inb( IO_ROBOT_INPUT_I );
            break;
        case 'J' : 
//            iData = inb( IO_ROBOT_INPUT_J );
            break;
        case 'K' : 
            iData = inb( IO_PRE_INPUT_K );
            break;
        case 'L' : 
            iData = inb( IO_PRE_OUTPUT_L );
            break;
        default:
            return FAILURE;
    }

    if( lBitNum>=0 && lBitNum<=47 )     /* manipulate the info read and return it */
        iData = ( iData >> (lBitNum&0x07)) & 1;
    return ( iData&0xff );
}
/*************************************************************************************
 * Function Name:   CMDreadBackParms
 * Description:     if axis==3 then three values are processed, otherwise only the first value.  this
 *                  routine behaves as follows:
 *                  if 3 or 4 axis, than 3 values processed otherwise only 1 value processed
 *                  if the operand pointer is non-null , than the value(s) are stored in the
 *                  register(s) [Rn] specified by the operand otherwise the value(s) are
 *                  output to the COM port.
 *                  This function is for most command that read back information from the galil or
 *                  station structure.
 * Parameters:      Optr - Operand Pointer
 *                  lValue - A pointer to an array of values
 *                  ulAxis - if 3 or 4 then process 3 values,
 *                           otherwise process only 1 value
 * Returned Value:     SUCCESS or FAILURE
 *************************************************************************************/
int CMDreadBackParms(CMDoperand_ptr Optr, long *lValue, ULONG ulAxis)
{
    char sBuf[80];
	int iCount;

    if( Optr )   /* the operand pointer is non-null => store values in registers */
    {
        if( ulAxis==RO_AXIS_ALL )  /* ALL axis */
        {
            for( iCount=0; iCount<3 && Optr; iCount++, Optr=Optr->next )
            {
                if( RGSetRegister(Optr->opr.i,lValue[iCount]) == FAILURE )
                    return FAILURE;
            }
			return SUCCESS;
        }
        else if( ulAxis==RO_AXIS_all )  /* all axis */
        {
            for( iCount=4; iCount<7 && Optr; iCount++, Optr=Optr->next )
            {
                if( RGSetRegister(Optr->opr.i,lValue[iCount]) == FAILURE )
                    return FAILURE;
            }
			return SUCCESS;
        }
        else                            /* 1 specified axis */
        {
            return RGSetRegister(Optr->opr.i,lValue[CMDgetAxisLoc(ulAxis)]);
        }
    }
    else
    {
        if( ulAxis==RO_AXIS_ALL)
			sprintf( sBuf, "%ld,%ld,%ld", lValue[0], lValue[1], lValue[2] );
		else if (ulAxis==RO_AXIS_all)  /* All axis */
            sprintf( sBuf, "%ld,%ld,%ld", lValue[4], lValue[5], lValue[6] );
        else                            /* 1 specified axis */
		{
	 	    sprintf(sBuf, "%ld", lValue[CMDgetAxisLoc(ulAxis)]);
		}
		return CMDShowDescription(SERGetCmdPort(), sBuf);
    }
    /* Unreachable code. */
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDsendSTAT
 * Description:     Send System 16-bit Status Word to com port if there is no operand.
 *                  if there is an operand which is the option of how to print the status word, then
 *                  send the description of each error status bit with or without providing information
 *                  of how to clear the error bit. If the option is 1, send the description of error status
 *                  bits. If the option is 2, send the description of error status bits with providing inforamtion
 *                  of how to clear error bit.
 * Parameters:      instr  -  instruction pointer
 *                  iStatus - the system status word.
 *                  iLongDesc - print out the long descriptions
 * Returned Value:  SUCCESS or FAILURE
 *************************************************************************************/
int CMDsendSTAT(instr_ptr instr, int iStatus, int iLongDesc)
{
    char sBuf[20];                      /* temporary buffer for storing status word. */
    int iPrintFormat;                   /* option for printing description  */
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    if( instr->opr_ptr )                /* if there is an operand pointer */
    {
        /* indirect register for storing status word (same return value to user),
         * and this one will not send to com port. */
        if( instr->opr_ptr->type == INDIRECT_REG )
            return RGSetRegister(instr->opr_ptr->opr.i,iStatus);   /* set value to R-register. */
        else if( iLongDesc )            /* otherwise just send out to the com port. */
        {
            iPrintFormat = (int)instr->opr_ptr->opr.l;     /* get the option of printing the status word */
			if(CMDShowDescription(iCmdPort, "             Controller Status Word\r\n\r\n")==FAILURE)
				return FAILURE;
            /* print status word descripton depending on the iPrintFormat (1/2).*/
            if(CMDShowStatDescription(iPrintFormat,iStatus,FALSE)==FAILURE)
                return FAILURE;
            return SUCCESS;
        }
    }
    else               /* else output the status only without description.*/
    {
        sprintf( sBuf, "%04X", iStatus );
		return CMDShowDescription(iCmdPort, sBuf);
    }
    /* Unreachable code */
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDsendSTAT2
 * Description:     Send the whole 32-bit status word to com port if there is not operand.
 *                  if there is an operand which is the option of how to print the status word, then
 *                  send the description of each error status bit with or without providing information
 *                  of how to clear the error bit. If the option is 1, send the description of error status
 *                  bits. If the option is 2, send the description of error status bits with providing inforamtion
 *                  of how to clear error bit.
 * Parameters:      instr - instruction pointer
 *                  iStatus - the first 16-bit system status word.
 *                  iStatus2 - the second 16-bit system status word.
 * Returned Value:  SUCCESS or FAILURE
 *************************************************************************************/
int CMDsendSTAT2(instr_ptr instr, int iStatus, int iStatus2)
{
    char sBuf[80];                      /* temporary buffer for storing the  status word. */
    long lStatus2;                      /* holds 32-bit status word. */
    int iPrintFormat;                   /* the option for printing description */
    int iCmdPort;                       /* com port id. */

    iCmdPort = SERGetCmdPort();
    sprintf(sBuf, "%04X%04X", iStatus2, iStatus);  /* combine 2 16-bit status word together (embedded \0) */
    /* convert to numeric if indirect register specified for storing in
     * an operand. */
    if(RGConvertStrToNum(sBuf, &lStatus2, 16)==FAILURE)
        return FAILURE;
    if( instr->opr_ptr )                /* if there is an operand pointer */
    {
        /* indirect register for storing status word (same return value to user),
         * and this one will not send to com port. */
        if( instr->opr_ptr->type == INDIRECT_REG )
            return RGSetRegister(instr->opr_ptr->opr.i,lStatus2);  /* set value to R-register. */
        else                            /* otherwise just send out to the com port. */
        {
            iPrintFormat = (int)instr->opr_ptr->opr.l;  /* get the option of printing the status word */
			if(CMDShowDescription(iCmdPort, "             Controller Status Word\r\n\r\n")==FAILURE)
				return FAILURE;
            /* print status word descripton depending on the iPrintFormat (1/2).*/
            if(CMDShowStat2Description(iPrintFormat, iStatus, iStatus2,TRUE)==FAILURE)
                return FAILURE;
            return SUCCESS;
        }
    }
    else                                /* else output the status only without description.*/
    {
		return CMDShowDescription(iCmdPort, sBuf);
    }
    /* Unreachable code. */
    return FAILURE;
}
/**************************************************************************************
 * Function Name:   CMDShowStatDescription
 * Description:     show the description of each error status bit in the status word with or
 *                  without providing the information of how to clear the error bit.
 * Parameters:      iPrintType - option to print with information of how clear the error bit or without.
 *                               Option 1:  printing description of error bits only.
 *                               Option 2:  printing description of error bits with providing information of
 *                                          how to clear the error bit.
 *                  iStatus -    status word of the first 16 bits.
 *                  iStat2Flag - either STAT or STAT2.  If TRUE, it is STAT2 command else STAT command.
 * Returns:         SUCCESS/FAILRE.
 **************************************************************************************/
int CMDShowStatDescription(int iPrintType, int iStatus, int iStat2Flag)
{
    int iCmdPort;
    char sBuf[80];

    iCmdPort = SERGetCmdPort();
    switch(iPrintType)
    {
        case 1:                         /* printing description of error status bit only. */
                if ((iStatus & 0xEFFF) == 0 && !iStat2Flag)
                {
					if(CMDShowDescription(iCmdPort, "No Error.\r\n")==FAILURE)
					   return FAILURE;
                    return SUCCESS;
                }
                if (iStatus & COM1_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Com1 error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & CTRL_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Controller error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & NVSRAM_ST)
                {
                    if(CMDShowStatTT(iCmdPort, "NVSRAM error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM2_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Com2 error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & SERVO_OFF)
                {
                    if(CMDShowStatTT(iCmdPort, "Servo off on one or more axes.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & RBT_MOVING)
                {
                    if(CMDShowStatTT(iCmdPort, "One or more axes are moving.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & RUNNING_MACRO)
                {
                    if(CMDShowStatTT(iCmdPort, "Running macro.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & ALIGNING)
                {
                    if(CMDShowStatTT(iCmdPort, "Last alignment error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & HOME_NOT_EXED)
                {
                    if(CMDShowStatTT(iCmdPort, "One or more axes are not homed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & LIMIT_SW)
                {
                    if(CMDShowStatTT(iCmdPort, "One or more limit switches are triggered.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MOT_ERROR)
                {
                    if(CMDShowStatTT(iCmdPort, "Motor error on one or more axes.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & VACUUM_SW)
                {
                    if(CMDShowStatTT(iCmdPort, "Vacuum switch on.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & VACUUM_SEN)
                {
                    if(CMDShowStatTT(iCmdPort, "Vacuum sensor is activated.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & CMD_INVAL)
                {
                    if(CMDShowStatTT(iCmdPort, "Previous command invalid.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & CMD_NOT_EXED)
                {
                    if(CMDShowStatTT(iCmdPort, "Previous command not executed.\r\n")==FAILURE)
						return FAILURE;
                }
                return SUCCESS;
        case 2:                          /* show description of each error status bit with providing the
                                         * information of how to clear the error bit. */
                if ((iStatus & 0xEFFF) == 0 && !iStat2Flag)
                {
					if(CMDShowDescription(iCmdPort, "No error.\r\n")==FAILURE)
						return FAILURE;
                    return SUCCESS;
                }
                if (iStatus & COM1_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Com1 error, Cleared by AST 1.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM2_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Com2 error, Cleared by AST 2\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & CTRL_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Controller error, Cleared by GLST.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & NVSRAM_ST)
                {
                    if(CMDShowStatTT(iCmdPort, "NVSRAM error, Cleared by RNCS.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & SERVO_OFF)
                {
                    if(CMDShowStatTT(iCmdPort, "Servo off on one or more axes, Cleared by SVON.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & RBT_MOVING)
                {
                    if(CMDShowStatTT(iCmdPort, "One or more axes are moving, Cleared by STOP.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & RUNNING_MACRO)
                {
                    if(CMDShowStatTT(iCmdPort, "Running macro, Cleared by STOP.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & ALIGNING)
                {
                    if(CMDShowStatTT(iCmdPort, "Last alignment error, Cleared by ALIGN.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & HOME_NOT_EXED)
                {
                    if(CMDShowStatTT(iCmdPort, "One or more axes are not homed, Cleared by HOME.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & LIMIT_SW)
                {
                    if(CMDShowStatTT(iCmdPort, "One or more limit switches are triggered, Cleared by RLS.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MOT_ERROR)
                {
                    if(CMDShowStatTT(iCmdPort, "Motor error on one or more axes, Cleared by SVON.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & VACUUM_SW)
                {
                    if(CMDShowStatTT(iCmdPort, "Vacuum switch on, Cleared by OUTP.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & VACUUM_SEN)
                {
                    if(CMDShowStatTT(iCmdPort, "Vacuum sensor is activated, Cleared by removing the wafer or adjusting the vacuum sensor.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & CMD_INVAL)
                {
                    if(CMDShowStatTT(iCmdPort, "Previous command invalid, Cleared by any except STAT.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & CMD_NOT_EXED)
                {
                    if(CMDShowStatTT(iCmdPort, "Previous command not executed, Cleared by any except STAT.\r\n")==FAILURE)
						return FAILURE;
                }
                return SUCCESS;
        default:
                sprintf(sBuf, "%04X", iStatus);  /* printing only status word if no option is provided */
				if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
					return FAILURE;
    }
    return SUCCESS;
}
/**************************************************************************************
 * Function Name:   CMDShowStat2Description
 * Description:     show the description of each error status bit in the status word 2 with or
 *                  without providing the information of how to clear the error bit.
 * Parameters:      iPrintType - option to print with information of how clear the error bit or without.
 *                               Option 1:  printing description of error bits only.
 *                               Option 2:  printing description of error bits with providing information of
 *                                          how to clear the error bit.
 *                  iStatus -    status word of the first 16 bits.
 *                  iStatus2 -   status word of the second 16 bits.
 *                  iStat2Flag - either STAT or STAT2.  If TRUE, it is STAT2 command else STAT command.
 * Returns:         SUCCESS/FAILRE.
 **************************************************************************************/
int CMDShowStat2Description(int iPrintType, int iStatus, int iStatus2, int iStat2Flag)
{
    int iCmdPort;
    char sBuf[80];

    iCmdPort = SERGetCmdPort();
    switch(iPrintType)
    {
        case 1:                         /* show description of each error status bit only. */
                if(CMDShowStatDescription(iPrintType, iStatus,iStat2Flag)==FAILURE)
                    return FAILURE;
                if((iStatus2 & 0xFFFF) == 0 && (iStatus & 0xEFFF)==0)  /* no error */
                {
                    if(CMDShowDescription(iCmdPort, "No Error.\r\n")==FAILURE)
						return FAILURE;
                    return SUCCESS;
                }
                if (iStatus2 & COM3_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Com3 error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & GA_COM_TIMEOUT)
                {
                    if(CMDShowStatTT(iCmdPort, "Galil communication error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & IO_CARD_VER_2)
                {
                    if(CMDShowStatTT(iCmdPort, "IO card version is older than V3.xx.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & BAD_JUMPER_SET)
                {
                    if(CMDShowStatTT(iCmdPort, "Jumper setting is wrong or parameter file mismatch.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & SCAN_START_ER)
                {
                    if(CMDShowStatTT(iCmdPort, "Scan start position error.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & SPEEDCHK_DISABLE)
                {
                    if(CMDShowStatTT(iCmdPort, "Speed check disabled.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & SPEED_DROP)
                {
                    if(CMDShowStatTT(iCmdPort, "Speed drop.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & OLD_GALIL_VER)
                {
                    if(CMDShowStatTT(iCmdPort, "Galil firmware version F or older.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & LOW_MEMORY)
                {
                    if(CMDShowStatTT(iCmdPort, "Available free memory less than 2KB.\r\n")==FAILURE)
						return FAILURE;
                }
                return SUCCESS;
        case 2:                         /* show description of each error status bit with providing the
                                         * information of how to clear the error bit. */
                if(CMDShowStatDescription(iPrintType, iStatus,iStat2Flag)==FAILURE)
                    return FAILURE;
                if((iStatus2 & 0xEFFF) == 0 && (iStatus & 0xEFFF)==0)
                {
					if(CMDShowDescription(iCmdPort, "No Error\r\n")==FAILURE)
						return FAILURE;
                    return SUCCESS;
                }
                if (iStatus2 & COM3_ERR)
                {
                    if(CMDShowStatTT(iCmdPort, "Com3 error, Cleared by AST 3\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & GA_COM_TIMEOUT)
                {
                    if(CMDShowStatTT(iCmdPort, "Galil communication error, Cleared by GLST or any command\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & IO_CARD_VER_2)
                {
                    if(CMDShowStatTT(iCmdPort, "IO card version is older than V3.xx, Cleared by changing to new IO card\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & BAD_JUMPER_SET)
                {
                    if(CMDShowStatTT(iCmdPort, "Jumper setting is wrong or parameter file mismatch,\r\n\tCleared by checking Mechanical Ratio of Z and matching the parameter file with the robot type\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & SCAN_START_ER)
                {
                    if(CMDShowStatTT(iCmdPort, "Scan start position error,\r\n\tCleared by increasing speed and acceleration for Z and rescanning\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & SPEEDCHK_DISABLE)
                {
                    if(CMDShowStatTT(iCmdPort, "Speed check disabled, Cleared by cutting speed jumper on IO card\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & SPEED_DROP)
                {
                    if(CMDShowStatTT(iCmdPort, "Speed drop, Cleared by rescanning\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & OLD_GALIL_VER)
                {
                    if(CMDShowStatTT(iCmdPort, "Galil firmware version F or older, Cleared by changing Galil firmware chips\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus2 & LOW_MEMORY)
                {
                    if(CMDShowStatTT(iCmdPort, "Available free memory less than 2KB, Cleared by deleting macros\r\n")==FAILURE)
						return FAILURE;
                }
                return SUCCESS;
        default:
                sprintf(sBuf, "%04X%04X", iStatus2, iStatus);  /* just printing the status word if no option is provided. */
                if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
                    return FAILURE;
    }
    return SUCCESS;
}
/**************************************************************************************
 * Function Name:   CMDShowStat3Description
 * Description:     show the description of each error status bit in the status word with or
 *                  without providing the information of how to clear the error bit.
 * Parameters:      iPrintType - option to print with information of how clear the error bit or without.
 *                               Option 1:  printing description of error bits only.
 *                               Option 2:  printing description of error bits with providing information of
 *                                          how to clear the error bit.
 *                  iStatus -    status word of the first 16 bits.
 * Returns:         SUCCESS/FAILRE.
 **************************************************************************************/
/*int CMDShowStat3Description(int iPrintType, int iStatus)
{
    int iCmdPort;
    char sBuf[80];

    iCmdPort = SERGetCmdPort();
    switch(iPrintType)
    {
        case 1:                         // printing description of error status bit only.
                if ((iStatus & 0xFFFF) == 0)
                {
                    if(CMDShowDescription(iCmdPort, "Initialization succeeded completely.\r\n")==FAILURE)
					   return FAILURE;
                    return SUCCESS;
                }
                if (iStatus & MATH_EROR_HANDLER)
                {
                    if(CMDShowDescription(iCmdPort, "ISEnableMathErrorHandler has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & SYSTEM_TIMER)
                {
                    if(CMDShowDescription(iCmdPort, "TIEnableTimer has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & STATUS_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "SSInitStatus has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM1_PORT)
                {
                    if(CMDShowDescription(iCmdPort, "SERInitPorts COM1 has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM2_PORT)
                {
                    if(CMDShowDescription(iCmdPort, "SERInitPorts COM2 has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM3_PORT)
                {
                    if(CMDShowDescription(iCmdPort, "SERInitPorts COM3 has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & TASK_LIST)
                {
                    if(CMDShowDescription(iCmdPort, "Task list init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & REGISTERS_TABLE)
                {
                    if(CMDShowDescription(iCmdPort, "Init R, C, S variables failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & SCANNER_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Mapper module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & DATAFILE_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Datafile module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MOTION_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Motion module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MECHANISM_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Mechanism module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & IO_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "I/O module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & ALIGNER_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Aligner module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & LOW_LEVEL_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Low-level module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MACRO_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Macro module init has failed.\r\n")==FAILURE)
						return FAILURE;
                }
                return SUCCESS;
        case 2:                          // show description of each error status bit with providing the
                                         // information of how to clear the error bit.
                if ((iStatus & 0xFFFF) == 0)
                {
                    if(CMDShowDescription(iCmdPort, "Initialization succeeded completely.\r\n")==FAILURE)
					   return FAILURE;
                    return SUCCESS;
                }
                if (iStatus & MATH_EROR_HANDLER)
                {
                    if(CMDShowDescription(iCmdPort, "ISEnableMathErrorHandler has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & SYSTEM_TIMER)
                {
                    if(CMDShowDescription(iCmdPort, "TIEnableTimer has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & STATUS_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "SSInitStatus has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM1_PORT)
                {
                    if(CMDShowDescription(iCmdPort, "SERInitPorts COM1 has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM2_PORT)
                {
                    if(CMDShowDescription(iCmdPort, "SERInitPorts COM2 has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & COM3_PORT)
                {
                    if(CMDShowDescription(iCmdPort, "SERInitPorts COM3 has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & TASK_LIST)
                {
                    if(CMDShowDescription(iCmdPort, "Task list init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & REGISTERS_TABLE)
                {
                    if(CMDShowDescription(iCmdPort, "Init R, C, S variables failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & SCANNER_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Mapper module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & DATAFILE_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Datafile module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MOTION_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Motion module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MECHANISM_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Mechanism module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & IO_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "I/O module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & ALIGNER_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Aligner module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & LOW_LEVEL_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Low-level module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }
                if (iStatus & MACRO_MODULE)
                {
                    if(CMDShowDescription(iCmdPort, "Macro module init has failed, Cleared by ...\r\n")==FAILURE)
						return FAILURE;
                }

                return SUCCESS;
        default:
                sprintf(sBuf, "%04X", iStatus);  // printing only status word if no option is provided
				if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
					return FAILURE;
    }
    return SUCCESS;
}
*/
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
int vld_STRMAN(instr_ptr instr, CMDoperand *Otype)
{
    int iNumOpr;
    CMDoperand_ptr Optr;

    iNumOpr = 0;
    Optr = instr->opr_ptr;
    if(!Optr)
        return FAILURE;
    do                                  /* while there is operand, it compare the operand type with the table of operand
                                         * type that pass in from vld_xxx function. */
    {
        if( Optr->type==Otype[iNumOpr].type || Optr->type==Otype[iNumOpr+1].type )
        {
            Optr = (CMDoperand_ptr)Optr->next;
            iNumOpr+=2;                 /* it adds 2 because the operand is either constant value or indirect value
                                         * so when you skip you have to skip 2 in the operand type table. */
        }
        else
            return FAILURE;
    } while(Optr);
    return SUCCESS;
}
/*************************************************************************************
 * Function Name:   CMDisNum
 * Description:     Validates if the operand is a LONG_CONST or INDIRECT_REG
 * Parameter:       Optr - Operand pointer
 * Returns:         SUCCESS/FAILURE
 *************************************************************************************/
int CMDisNum(CMDoperand_ptr Optr)
{
    if( Optr )
    {
        /* return success if operand type is long constant or indirect R-register */
        switch( Optr->type )
        {
            case LONG_CONST :
            case INDIRECT_REG :
                return SUCCESS;
        }
    }
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDisCharacter
 * Description:     Validates if the operand is a CHAR_CONST or INDIRECT_CHAR.
 * Parameter:       Optr - Operand pointer.
 * Returns:         SUCCESS/FAILURE
 *************************************************************************************/
int CMDisCharacter(CMDoperand_ptr Optr)
{
    if( Optr )
    {
        /* return success if operand type is character constant or indirect C-register */
        switch( Optr->type )
        {
            case CHAR_CONST :
            case INDIRECT_CHAR :
                return SUCCESS;
        }
    }
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDisString
 * Description:     Validates if the operand is a STRING_CONST or INDIRECT_STRING
 * Parameter:       Optr - operand pointer
 * Returns:         SUCCESS/FAILURE
 *************************************************************************************/
int CMDisString(CMDoperand_ptr Optr)
{
    if( Optr )
    {
        /* return success if operand type is string constant ro indirect S-register */
        switch( Optr->type )
        {
            case STRING_CONST :
            case INDIRECT_STR :
                return SUCCESS;
        }
    }
    return FAILURE;
}
/*************************************************************************************
 * Function Name:   CMDgetDecryptedPassword
 * Description:     Validates the password that has been passed in by the user by
 *                  passing the decryption numbers for the decryption algorithms
 *                  to CMDMakeDecryptedPassword to calculate the matching
 *                  password internally.
 * Parameter:       iFeatureNumArg - the feature number to decrypt the password for
 *                  lTimeDateArg - time and date stamp recorded when the TIME host command was last run
 * Returns:         decrypted password
 *************************************************************************************/
double CMDgetDecryptedPassword(int iFeatureNumArg, long lTimeDateArg)
{
    dTimeDateTemp = (double)(lTimeDateArg);
    switch (iFeatureNumArg)
    {
        case SECS_HOST :
            CMDMakeDecryptedPassword(50.0, 6765.0, 484.0);
            break;

        case SECS_DEVICE :
            CMDMakeDecryptedPassword(89.0, 2905.0, 683.0);
            break;

        case TOT :
            CMDMakeDecryptedPassword(39.0, 5876.0, 571.0);
            break;

        case OTF :
            CMDMakeDecryptedPassword(67.0, 4983.0, 579.0);
            break;

        case MULTI_CHIP_ALIGN :
            CMDMakeDecryptedPassword(24.0, 9878.0, 378.0);
            break;

        case COM3_SUPPORT :
            CMDMakeDecryptedPassword(87.0, 4587.0, 347.0);
            break;

        case INTELLIPICK :
            CMDMakeDecryptedPassword(94.0, 2487.0, 789.0);
            break;

        case SQUARE_WAFER_ALIGN :
            CMDMakeDecryptedPassword(44.0, 2766.0, 258.0);
            break;

        case SCANNER :
            CMDMakeDecryptedPassword(75.0, 7662.0, 981.0);
            break;

        case EQUIPE_BUS :
            CMDMakeDecryptedPassword(18.0, 1971.0, 918.0);
            break;

        case FAST_ALIGN :
            CMDMakeDecryptedPassword(69.0, 1203.0, 508.0);
            break;

        case SYSCFG :
            CMDMakeDecryptedPassword(14.0, 1485.0, 194.0);
            break;

        default:
            return( (double)(FAILURE) );
    }
    dMult1 = dTemp1*dTemp2;
    dDecryptedPassword = dMult1*dTemp3;
    return dDecryptedPassword;
}
/*************************************************************************************
 * Function Name:   CMDMakeDecryptedPassword
 * Description:     Creates the password for the key that has been passed in by the user by
 *                  using the decryption algorithms here to calculate the matching
 *                  password internally.
 * Parameter:       dTime1Arg - decryption code first value
 *                  dTime2Arg - decryption code second value
 *                  dTime3Arg - decryption code third value
 * Returns:         SUCCESS or FAILURE
 *************************************************************************************/
int CMDMakeDecryptedPassword(double dTime1Arg, double dTime2Arg, double dTime3Arg)
{
    dTemp1 = dTimeDateTemp + dTime1Arg;
    dTemp2 = dTimeDateTemp + dTime2Arg;
    dTemp3 = dTimeDateTemp + dTime3Arg;
	return SUCCESS;
}

int CMDShowStatTT(int iPortArg, char *strArg)
{
    if(iPortArg == SERGetTTPort())
    {
        if(CMDdumpBuf(strArg)==FAILURE)
            return FAILURE;
    }
    else
    {
        if(CMDShowDescription(iPortArg, strArg)==FAILURE)
            return FAILURE;
    }
    return SUCCESS;
}

int CMDShowDescription(int iPortArg, char *strArg)
{
    if(SERPutsTxBuff(iPortArg, strArg)==FAILURE)
        return FAILURE;
    if(SERFlushTxBuff(iPortArg)==FAILURE)
        return FAILURE;
    return SUCCESS;
}

int CMDSetIndirectReg(CMDoperand_ptr OptrArg, long lValueArg)
{
    if(!OptrArg)
        return FAILURE;
    if(OptrArg->type != INDIRECT_REG)
        return FAILURE;
    if(RGSetRegister(OptrArg->opr.i, lValueArg)==FAILURE)
	return FAILURE;
    return SUCCESS;
}

int CMDgetIndirectValue(CMDoperand_ptr OptrArg, int *iValueArg)
{
    if(!OptrArg)
        return FAILURE;
    if(OptrArg->type != INDIRECT_REG)
        return FAILURE;
    else
        *iValueArg = OptrArg->opr.i;
	return SUCCESS;
}

int CMDgetIndirectStrValue(CMDoperand_ptr OptrArg, int *iValueArg)
{
    if(!OptrArg)
        return FAILURE;
    if(OptrArg->type != INDIRECT_STR)
        return FAILURE;
    else
        *iValueArg = OptrArg->opr.i;
	return SUCCESS;
}

int CMDSetOrShowStatus(CMDoperand_ptr OptrArg, int iValueArg)
{
	int iCmdPort;
	char caTemp[80];

    if(OptrArg)
    {
		if(CMDSetIndirectReg(OptrArg, (long)iValueArg)==FAILURE)
			return FAILURE;
    }
    else
    {
		caTemp[0] = 0;
        sprintf(caTemp, "%04X", iValueArg);
		iCmdPort = SERGetCmdPort();
        if( SERPutsTxBuff(iCmdPort, caTemp) == FAILURE )
			return FAILURE;
	}
	return SUCCESS;
}

int CMDgetIntegerValue(CMDoperand_ptr OptrArg, int *iValueArg)
{
	long lValue;
    if(CMDgetValue(OptrArg, &lValue) != SUCCESS)
        return FAILURE;
    *iValueArg = (int)lValue;
	return SUCCESS;
}

int CMDgetAxisValue(ULONG ulAxisArg, CMDoperand_ptr OptrArg, long *lpPositionArg)
{
	int iCount;

    if( ulAxisArg==RO_AXIS_ALL || ulAxisArg==RO_AXIS_all )
    {
        for( iCount=0; iCount<3 && OptrArg; iCount++, OptrArg=OptrArg->next )
        {
            if( CMDgetValue(OptrArg,&lpPositionArg[iCount]) == FAILURE )
                return FAILURE;
        }
    }
    /* get the distance value for specified axis. */
    else if( CMDgetValue(OptrArg,&lpPositionArg[CMDgetAxisLoc(ulAxisArg)]) == (unsigned long)FAILURE )
        return FAILURE;
	return SUCCESS;
}

int CMDReadHFLGandTAvalue(ULONG *ulAxis, instr_ptr instr, long *lBuf)
{
	int iCount;

    if( CMDgetGalilParms(instr,ulAxis,lBuf) == FAILURE )
        return FAILURE;
    for( iCount=0; iCount<8; iCount++ )
    {
        if( lBuf[iCount] != 0 && lBuf[iCount] != 1 )
            return FAILURE;
    }
	return SUCCESS;
}

int CMDReadStnProperty(CMDoperand_ptr OptrArg, int iTypeArg)
{
    long lValue;
    int iStationIndex;
	ULONG ulAxis = RO_AXIS_T;

    /* get station index */
    if((iStationIndex = CMDgetStnIndx(OptrArg)) == FAILURE )
        return FAILURE;
    /* get Offset by specifying the station index */
    if( FIOGetStnVals(iStationIndex, iTypeArg, &lValue) == FAILURE )
        return FAILURE;
    /* Next operand; it can be NULL. If it is null, then send the value to comm port; otherwise, set the value
     * into the register */
    return CMDreadBackParms( OptrArg->next, &lValue, ulAxis );
}
