/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        LLexcomm.c
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <malloc.h>

#include "cmdfns.h"
#include "cmdser.h"
#include "cmdsp.h"
#include "scmac.h"
#include "scio.h"
#include "sctim.h"
#include "gag.h"
#include "scstat.h"
#include "ser.h"
#include "fiog.h"
#include "sctag.h"
#include "scver.h"
#include "scregg.h"
#include "scmem.h"
#include "ro.h"
#include "rofio.h"
#include "gaintr.h"
#include "sctch.h"
#include "dg.h"
#include "scttr.h"
#include "mapio.h"
#include "mapfn.h"
#include "mapstn.h"
#include "map.h"
#include "scproc.h"
#include "secsg.h"
#include "scmem.h"


/***************************************************************************
 * Function Name:   ex_AST
 * Description:     Asynchronous Communications Status Request.
 * Implemetation:   In This function how it work:
 *                  - Get the com port id from the instruction that passed in
 *                    from the function arguement.
 *                  - Get comm status of specified com port id by calling SERGetCommStatus from the
 *                    comm module. It returns 16-bit status word of the comm.
 *                  - get the number bytes that has in the comm buffer now. If it returns -1, that's mean
 *                    there is no data in the comm buffer and set bits 11-15 to 0; otherwise set number of
 *                    bytes in the comm buffer and combine number of bytes in the buffer and 16-bit status word
 *                    into 32-bit status word.
 *                  - Then check if there is next operand, it must be an indirect R-register.  Convert the
 *                    32-bit status word into numeric and store into the R-register at specified index. Otherwise
 *                    just put out to the comm port.
 *                  - Then reset the comm status to 0 by calling SERSetCommStatus(PortId,0) and clear error bit of
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
int ex_AST(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iBuffAvail = 0;
    int iStat = 0;
    int iPort;
    char sBuf[20];
    long ulStatus;
    int iASTstat = SUCCESS;

    Optr = instr->opr_ptr;
    /* get port number */
	if(CMDgetIntegerValue(Optr, &iPort)==FAILURE)
        return FAILURE;

    if( SERGetCommVals(iPort, COMM_STATUS, &iStat) != SUCCESS )  /* get port status */
        return FAILURE;

    if( SERRxBuffAvail(iPort) < 0 )      /* get available buffer size */
        return FAILURE;
    /* bits 16 ... 31  indicate the number of chars in input/receiving buffer */
    if( (iBuffAvail=SERGetNumCharsInBuffer(iPort)) < 0 )
        return FAILURE;

    sprintf(sBuf, "%04X%04X", iBuffAvail, iStat);      /*combine the 32-bit status word */
    sBuf[strlen(sBuf)] = '\0';

    if( !(Optr=Optr->next) )                         /* no second operand */
    {
		if(CMDShowDescription(SERGetCmdPort(), sBuf)==FAILURE)
			return FAILURE;
    }
    else                                /* put the status word into an indirect R-register. */
    {
        /* convert the string to number to set into the R-register for return value */
        if(RGConvertStrToNum(sBuf, &ulStatus, 16)==FAILURE) return FAILURE;

		if(CMDSetIndirectReg(Optr, ulStatus)==FAILURE)
			iASTstat = FAILURE;
    }

    if( SERSetCommVals(iPort, COMM_STATUS, 0) != SUCCESS )  /* Reset comm status */
        iASTstat = FAILURE;

    if( iASTstat== FAILURE)  /* problem prior to setting the comm status */
        return FAILURE;

    if( iPort == COM1 )                /* clear error bit for com1 in system status word. */
    {
        SSSetStatusWord( COM1_ERR, FALSE );
    }
    else if ( iPort == COM2 )           /* clear error bit for com2 in system status word. */
    {
        SSSetStatusWord( COM2_ERR, FALSE );
    }
    else if ( iPort == COM3 )           /* clear error bit for for com3 in system status word. */
    {
        SSSetStatusWord2(COM3_ERR, FALSE);
    }

    return SUCCESS;
}
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
int ex_DCHN(instr_ptr instr)
{
    int iRadix, iPortOpen;
    int iPort = COM3;
    CMDoperand_ptr Optr;

    if( !(Optr=instr->opr_ptr) )
        return FAILURE;
    /* See if in this format: DCHN port, D/H, N/Sn/Vn */
    if( Optr->type == LONG_CONST || Optr->type == INDIRECT_REG )
    {
        if(CMDgetIntegerValue(Optr, &iPort)==FAILURE)
            return FAILURE;
        if(iPort < COM1 || iPort > COM3)
            return FAILURE;
        if( !(Optr=Optr->next) )
            return FAILURE;
    }
    /* can be first or second operand, get the radix value either D(ec) or H(ex) */
    if(Optr->opr.i == 'D')
       iRadix = 10;
    else if (Optr->opr.i == 'H')
       iRadix = 16;
    else
        return FAILURE;
    /* open the comm port with specified port id */
    if( SERGetCommVals(iPort, PORT_OPEN, &iPortOpen) == FAILURE )
        return FAILURE;
    if (iPortOpen == FALSE)
        return FAILURE;

    if( !(Optr=Optr->next) )
        return FAILURE;
    if(Optr)                            /* there must have the second or third operand depending on the port is specified
                                         * or not. If the port is specified, then there must have the third operand. If no port
                                         * specified, then there must have the second operand. */
    {
        /* send the values to comm port while there is still operand in the operand list. */
        return CMDdumpVariables(Optr, iRadix, iPort);
    }
    /* no third operand, fail */
    return FAILURE;
}
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
int ex_BYPAS(instr_ptr instr)
{
    long lCommPort=COM2;                 /* default to COM2 */

    if( instr->opr_ptr )                /* if there is an operand, check commport */
    {
        if( CMDgetValue(instr->opr_ptr,&lCommPort) == FAILURE )
            return FAILURE;
        /* only COM2 or COM3 or COM4*/
        if(lCommPort != COM2 && lCommPort != COM3 && lCommPort != COM4)
            return FAILURE;
    }
    /* send direct port. */
    return ROPortRedirect((int)lCommPort);
}
/***************************************************************************
 * Function Name:   ex_READ
 * Description:     Read string from COM buffer.  READ reads the whole buffer from comm port and
 *                  either store the whole string message in the string register if there is no delimiter(','),
 *                  or parse the message with delimiter (',') and store each part
 *                  of the message to appropriate register that specified in the arguements.
 * Implementation:  First get the Comm port id from the operand list and validate the comm port id to make
 *                  sure it is in the range of 1..3. Get the second operand which has to be the indirect_reg
 *                  type for storing the number of bytes have been read from the comm. Then check if
 *                  there is something in the comm buffer by calling SERRxBuffAvail() from comm module.  If
 *                  there is something in the buffer then it will return number of bytes which is greater than
 *                  zero.  Then get everything from the comm buffer by specifying the number of bytes to
 *                  read as -1 in the function SERGetsRxBuff().  Then get the length of message to store into
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
int ex_READ(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iPort, iReturn, iComErr=0;
    int nLen, iPortOpen, iRindx, iNumBytes, iCmdPort;
    char *sPtr;
    char sBuf[MAXLINE];
    int iReadDone;

    iCmdPort = SERGetCmdPort();

//    sBuf = (char *)malloc(MAXLINE);
//    if(!sBuf)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***EX_READ: out of memory for malloc string buffer.***\n\r?");
//#endif
//        return FAILURE;
//    }
    sBuf[0] = 0;

    Optr = instr->opr_ptr;              /* get first operand, the Port Number */
    if((iReturn=CMDgetIntegerValue(Optr, &iPort))==FAILURE)
	goto exit_point;

    // overwrite the port
    iPort = SERGetCmdPort();

    /* validate the comm port and make sure it is already open. */
    //iPort = iPort + 1;
    if ((iReturn=SERGetCommVals(iPort, PORT_OPEN, &iPortOpen)) == FAILURE)
        goto exit_point;
    if( iPort!=COM1 && iPort!=COM2 && iPort!=COM3 && iPort!=COM4 && iPortOpen == FALSE )
    {
	iReturn = FAILURE;
        goto exit_point;
    }
    /* get second operand, register to store number of bytes have read from the comm buffer */
    Optr = Optr->next;
    if((iReturn=CMDgetIndirectValue(Optr, &iRindx))==FAILURE)
	goto exit_point;
    /* check for anything in comm buffer to read. If 0 returned, that's meant there is nothing
     * in the comm buffer. Otherwise there is something in the comm buffer. */
    iReadDone = 0;

    while(!iReadDone)
    {
    	if(SERRxBuffAvail(iPort)>0)
    	{
//	    iReturn = COReadStringCR(iPort, sBuf);
//            iNumBytes = -1;                 /* specify to read whole message from comm buffer up to \r */
            /* read the message from the comm buffer. */
            if( SERGetsRxBuff(iPort,sBuf,FALSE,&iNumBytes,TRUE) == FAILURE )
            {
            	/* in the case of failure, set -1 to register that hold number of bytes that have read. */
	    	iComErr = TRUE;
	    	goto exit_point;
            }
            nLen = strlen(sBuf);            /* number of bytes have read from the comm buffer. */
	    iReadDone = 1;
//printf("READ: %s\n", sBuf);
    	}
//    	else
//            nLen = 0;                       /* nothing in the comm buffer  */
    }
    /* store the number of bytes just read to the register */
    if( (iReturn=RGSetRegister(iRindx,(long)nLen)) == FAILURE )
        goto exit_point;
    if(nLen == 0)
    {
	iReturn = SUCCESS;
        goto exit_point;
    }
    /* parse the message and store to appropriate operand that was specified. */
    sPtr = strtok( sBuf, "," );
    Optr = Optr->next;              /* get third operand  */
    while( Optr )                       /* while there are still operands */
    {
        if( !sPtr )                     /* if no more info in data read */
        {
	    iComErr = TRUE;
	    goto exit_point;
        }
        switch( Optr->type )            /* process the operands type */
        {
            case INDIRECT_CHAR :        /* character register type */
                while( *sPtr == ' ' )   /* skip over white space */
                    sPtr++;
                if( !isprint(*sPtr) )   /* if character is not printable */
                {
		    iComErr = TRUE;
		    goto exit_point;
                }
                /* store the chacter to character register type at index optr->opr.i */
                if( (iReturn=RGSetCharacter(Optr->opr.i,*sPtr)) == FAILURE )
                    goto exit_point;
                break;
            case INDIRECT_REG :         /* numeric register type */
                /* set the numeric value into the register at optr->opr.i */
                if( (iReturn=RGSetRegister(Optr->opr.i,atol(sPtr))) == FAILURE )
                    goto exit_point;
                break;
            case INDIRECT_STR :           /* string register type */
                /* set the string into the string register at optr->opr.i */
                if( (iReturn=RGSetString(Optr->opr.i,sPtr,0)) == FAILURE )
                    goto exit_point;
                break;
        }
        if( !(sPtr=strtok(NULL,",")) )  /* no more info in data read */
            break;
        Optr = Optr->next;          /* get next operand   */
    }
exit_point:
//    if(sBuf)
 //       free(sBuf);
    /* in the case of failure, set -1 to register that hold number of bytes that have read. */
	if(iComErr)
    	return RGSetRegister(iRindx,-1);
	return iReturn;
}
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
int ex_READC(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    int iNumBytes, iNumCharsLeft, iNumCharsLeftAfterRead, iStatus, iReturn, iCmdPort, iPortOpen;
    long lPort, lBytesRead, lNumBytes;
    char sBuf[MAXLINE];
    char sBuf2[MAXLINE];

    iCmdPort = SERGetCmdPort();


    sBuf[0] = 0;
    sBuf2[0] = 0;

    Optr = instr->opr_ptr;              /* get first operand which is the comm port id */
    if((iReturn=CMDgetValue(Optr, &lPort))==FAILURE)
	goto exit_point;
    /* validate the comm port */
    if ((iReturn=SERGetCommVals((int)lPort, PORT_OPEN, &iPortOpen)) == FAILURE)
        goto exit_point;
    if( lPort!=COM1 && lPort!=COM2 && lPort!=COM3 /*&& lPort!=COM4*/ &&(iPortOpen == FALSE) )
    {
        iReturn = FAILURE;
        goto exit_point;
    }

    Optr = Optr->next;  /* get second operand which is number of bytes to read from the comm port*/
    if((iReturn=CMDgetIntegerValue(Optr, &iNumBytes))==FAILURE)
	goto exit_point;

//    SERFlushRxBuff((int)lPort);
    SERFlushRxBuff(iCmdPort);
    /* check for comm buffer if there is anything in comm buffer. */
//    if(SERRxBuffAvail((int)lPort) > 0)
//    {
        /* read the number of bytes that specified from comm buffer. */
//        if( (iReturn=SERGetsRxBuff((int)lPort,sBuf,TRUE,&iNumBytes,TRUE))==FAILURE )
//	if (iReturn = COReadStringCR((int)lPort, sBuf, &iNumCharsLeftAfterRead)==FAILURE)
	if (iReturn = COReadStringCR(iCmdPort, sBuf, &iNumCharsLeftAfterRead)==FAILURE)
	{
//        if( (iReturn=SERGetcRxBuff((int)lPort,sBuf,TRUE))==FAILURE )
printf("Port=%d COreadStrCR=%s\n",iCmdPort,sBuf);
            goto exit_point;
	}
printf("Port=%d COreadStrCR=%s\n",iCmdPort,sBuf);
//        lBytesRead = (long)iNumBytes;
        lBytesRead = (long)strlen(sBuf);
//    }
//    else
//        lBytesRead = 0; /* nothing in the comm buffer. */

    Optr = Optr->next;  /* get third operand which is the register index to store the number of bytes that
                         * had read from the comm buffer */
    if((iReturn=CMDSetIndirectReg(Optr, lBytesRead))==FAILURE)
	goto exit_point;

    /* get the fourth operand which is the register to store the character or string value */
    Optr = Optr->next;
    if(!Optr)
    {
        iReturn = FAILURE;
        goto exit_point;
    }

//printf("lBytesRead=%d\n", lBytesRead);
    /* Clear the S or C register, if number of bytes had read is 0*/
    if(lBytesRead == 0)
    {
        switch( Optr->type )
        {
            case INDIRECT_CHAR :
                iReturn=RGSetCharacter(Optr->opr.i,0);
                goto exit_point;
            case INDIRECT_STR :
                iReturn = RGSetString(Optr->opr.i,"",0);
                goto exit_point;
         }
    }
    /* Copy the read chars to indirect registers C or S. In the way the code is implemented here, you store
     * the value into both character and string register.  However, at right now, we use to store only either character
     * or string register.
     * For Example:     currently usuage:   READC 1, 2, [R1], [S1].     ==> this is what we are all using right now.
     *                  for existence code: READC 1, 2, [R1], [C1],[C2]   or   ==> no one has used this way yet.
     *                                      READC 1, 3, [R1], [C1], [S1] */
    lNumBytes = 0;
    while( Optr )
    {
        if( lNumBytes == lBytesRead )   /* if all bytes read */
        {
            iReturn = SUCCESS;
            goto exit_point;
        }
        switch( Optr->type )
        {
            case INDIRECT_CHAR :        /* store a character into character register type */
                if( (iReturn=RGSetCharacter(Optr->opr.i,sBuf[(int)lNumBytes++])) == FAILURE )
                    goto exit_point;
                break;
            case INDIRECT_STR :         /* store a string into string register type */
                memcpy( sBuf2, sBuf+((int)lNumBytes), (size_t)(lBytesRead-lNumBytes) );
                sBuf2[lBytesRead-lNumBytes] = '\0';
                iReturn = RGSetString(Optr->opr.i,sBuf2,0);
                goto exit_point;
                break;
        }
        Optr = Optr->next;
    }
exit_point:
//    if(sBuf != NULL)
//        free(sBuf);
//    if(sBuf2 != NULL)
//        free(sBuf2);
    return iReturn;

}
/***************************************************************************
 * Function Name:   ex_SEOT
 * Description:     Set End of Transmission character. (only use for cybeq emulator)
 * Implementation:  First, get the mode. if 0 then call SERSetEOTChar() to set the end of transmission character
 *                  is CR and ignore LF. If 1 then call SERSetEOTChar() to set the end of transmission character
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
int ex_SEOT(instr_ptr instr)
{
    long lTransChar;

    if( CMDgetValue(instr->opr_ptr,&lTransChar) == SUCCESS )
    {
        switch( lTransChar )
        {
            case 0 :
                SERSetEOTChar( '\r' );
                SERSetIGNOREChar( '\n' );
                break;
            case 1 :                    /* cybeq emulator mode. */
                SERSetEOTChar( '\n' );
                SERSetIGNOREChar( '\r' );
                break;
            default:
                return FAILURE;
        }
        return SUCCESS;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SDCM
 * Description:     Set Daisy Chain Mode.  Enables or disables Daisy Chain Mode.  When on Daisy Chain
 *                  Mode any data from COM2 will not be interpreted by the controller.  Data will be stored
 *                  in a buffer until read by READC or REAC commands.  This also allows COM2 macro to handle
 *                  the data. The flag value is either 0(disable) or 1(enable).
 * Implementation:  Get the flag value from the operand and call SERSetDCMode() to set the daisy chain
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
int ex_SDCM(instr_ptr instr)
{
    long lVal;
    if(instr->opr_ptr)
    {
        /* get the flag value either 0 or 1 */
        if( CMDgetValue(instr->opr_ptr,&lVal) == SUCCESS )
        {
            if (lVal == 0 || lVal == 1)
            {
                return SERSetCommVals(NO_PORT_NUM, DC_MODE, (int)lVal);  /* set the daisy chain mode to be disabled or enabled. */
            }
        }
    }
    return FAILURE;
}
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
int ex_DMPAS(instr_ptr instr )
{
    long lValue;

    /* get the comm port id */
    if( CMDgetValue(instr->opr_ptr, &lValue) == FAILURE )
        return FAILURE;
    /* validate the port id */
    if(lValue <COM1 || lValue > COM3)
        return FAILURE;
    /* check for comm buffer */
    if ( SERRxBuffAvail((int)lValue) < 0 )
        return FAILURE;
    /* send the comm history list in the buffer through the comm port. */
    if( SERGetHistoryList((int)lValue) == FAILURE )
        return FAILURE;

    return SUCCESS;
}
