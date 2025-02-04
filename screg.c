/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * Original Author      : Canh Le (Deceased)
 * Current Maintainer   : Georges Sancosme (georges@sancosme.net)
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
 * Program:     Register R, C, and S
 * File:        screg.c
 * Functions:   RGInit
 *              RGGetRegister
 *              RGSetRegister
 *              RGGetCharacter
 *              RGSetCharacter
 *              RGGetString
 *              RGSetString
 *              RGIsRegisterIndexValid
 *              RGIsCharacterIndexValid
 *              RGIsStringIndexValid
 *              RGCompareNumNum
 *              RGCompareStrStr
 *              RGConcatStrStr
 *              RGMinusNumNum
 *              RGConvertStrToNum
 *              RGConvertStrToNumStrReg
 *              RGConvertNumToStrNumSReg
 *              RGEqualSRegSReg
 *              RGEqualSRegStr
 *              RGEqualCRegCReg
 *              RGEqualCRegChar
 *              RGEqualRegReg
 *              RGEqualRegNum
 *              RGExtractCharacterSRegChar
 *              RGExtractCharacterStrCReg
 *              RGInsertStringSRegChar
 *              RGFreeAllRCS
 *              RGInitSpecialRegister
 *              RGGetSpecialRegister
 *              RGSetSpecialRegister
 *              RGIsSpecialRegisterIndexValid
 *              RGEqualSpecialRegNum
 *              RGEqualSpecialRegReg
 *
 * Description: Provide interface to access to specific private variables and maintain the variables
 *              for everyone use.  Also, provide the interface for manipulate the registers(R,C,S).
 *              1. Set the value to register.
 *              2. Get the value from specified register.
 *              3. Convert Number to string.
 *              4. Convert string to number.
 *              5. Concatenate string.
 *              6. Insert the character into string.
 *              7. Extract character/string from string.
 *              8. Convert decimal to hex.
 *              9. Convert hex to decimal.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "scmac.h"
#include "screg.h"
#include "sck.h"
#include "ser.h"
#include "scmem.h"

/****************************************************************\
 *
 * Function:    RGInit()
 *
 * Description:    Allocate the memory for R,C,S register and
 *              initialize.
 *
 * Parameters:  None
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1 and SUCCESS is 0
 *
\*****************************************************************/
int RGInit()
{
    int iIndex;
    int iReturn, iCmdPort;

    iCmdPort = SERGetCmdPort();

    iReturn = SUCCESS;

    /* allocate the memory for Special R-variables */
//    RGplSpecialRegister = (long *)HPmalloc(sizeof(long)*(MAXSPECIALREG+1));
    RGplSpecialRegister = (long *)malloc(sizeof(long)*(MAXSPECIALREG+1));
    if (RGplSpecialRegister == NULL)
    {
#ifdef DEBUG
        SERPutsTxBuff(iCmdPort,"***RGInit: out of memory when malloc Special registers.***\n\r?");
#endif
        iReturn = FAILURE;
        goto TO_EXIT;
    }

    /* allocate the memory for R-variables */
//    RGplRegister = (long *)HPmalloc(sizeof(long)*(MAXREG+1));
    RGplRegister = (long *)malloc(sizeof(long)*(MAXREG+1));
    if (RGplRegister == NULL)
    {
#ifdef DEBUG
        SERPutsTxBuff(iCmdPort,"***RGInit: out of memory when malloc registers.***\n\r?");
#endif
        iReturn = FAILURE;
        goto TO_EXIT;
    }

    /* Allocate the memory for C-variables */
//    RGpcCharacter = (char *)HPmalloc(MAXCHAR+1);
    RGpcCharacter = (char *)malloc(MAXCHAR+1);
    if (RGpcCharacter == NULL)
    {
#ifdef DEBUG
        SERPutsTxBuff(iCmdPort,"***RGInit: out of memory when malloc characters.***\n\r?");
#endif
        iReturn = FAILURE;
        goto TO_EXIT;
    }

    /* Allocate the memory for 20 S-variables and each S-variable's length is 256 */
    for ( iIndex = 0; iIndex <= MAXSTRINGVARS; iIndex++)
    {
//         RGpacString[iIndex] = (char *)HPmalloc(MAXSTRINGLENGTH + 1);
         RGpacString[iIndex] = (char *)malloc(MAXSTRINGLENGTH + 1);
         if ( RGpacString[iIndex] == NULL)
         {
#ifdef DEBUG
            SERPutsTxBuff(iCmdPort,"***RGInit: out of memory when malloc string registers.***\n\r?");
#endif
            iReturn = FAILURE;
            goto TO_EXIT;
         }
         *RGpacString[iIndex] = 0;           /* initialize the s-variable to 0 */
    }

    /*Initialize the R-variables and C-variables */
    for ( iIndex = 0; iIndex < MAXREG+1; iIndex++)
    {
        RGplRegister[iIndex] = 0;            /* initialize to 0 */
        RGpcCharacter[iIndex] = '\0';        /* initialize to null */
    }
    /* Initialize the Special R-variables */
    for ( iIndex = 0; iIndex < MAXSPECIALREG+1; iIndex++)
    {
        RGplSpecialRegister[iIndex] = 0;            /* initialize to 0 */
    }

TO_EXIT:
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGInitSpecialRegister()
 *
 * Description:    Initialize the special R-Registers
 *
 * Parameters:
 *              None
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGInitSpecialRegister()
{
    int iIndex;

    /* Initialize the Special R-variables */
    for ( iIndex = 0; iIndex < MAXSPECIALREG+1; iIndex++)
    {
        RGplSpecialRegister[iIndex] = -1;            /* initialize to -1 */
    }
    return SUCCESS;
}

/****************************************************************\
 *
 * Function:    RGGetSpecialRegister()
 *
 * Description:    Get the value of specific special R-register by providing the
 *              register number/index.
 *
 * Parameters:
 *              iIndexArg           - index of array or register number
 *              plReturnValueArg    - long value of R[iIndex];
 *                                    it's a return value.
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGGetSpecialRegister(int iIndexArg, long *plReturnValueArg)
{
    int iReturn;

   /* validate the special register number/index */
    if (RGIsSpecialRegisterIndexValid(iIndexArg)==SUCCESS)
    {
        *plReturnValueArg = RGplSpecialRegister[iIndexArg];
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGSetSpecialRegister()
 *
 * Description:    Set the value of special R-register by providing specific
 *              register number/index.
 *
 * Parameters:
 *              iIndexArg    - index of array or register number
 *              lValueArg    - long value of special R-register to set
 *                             at iIndexArg.
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGSetSpecialRegister(int iIndexArg, long lValueArg)
{
    int iReturn;

   /* validate the special R-register index/number */
    if (RGIsSpecialRegisterIndexValid(iIndexArg)==SUCCESS)
    {
        RGplSpecialRegister[iIndexArg] = lValueArg;
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}


/****************************************************************\
 *
 * Function:    RGGetRegister()
 *
 * Description:    Get the value of R-register at specific index
 *
 * Parameters:
 *              iIndexArg           - index of array or register number
 *              plReturnValueArg    - long value of R[iIndex] (return value)
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGGetRegister(int iIndexArg, long *plReturnValueArg)
{
    int iReturn;

   /* validate the register index/number */
    if (RGIsRegisterIndexValid(iIndexArg)==SUCCESS)
    {
        *plReturnValueArg = RGplRegister[iIndexArg];
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGSetRegister()
 *
 * Description:    Set the value of R-register at specific index
 *
 * Parameters:
 *              iIndexArg           - index of array or register number
 *              lValueArg           - long value of R-register to be set
 *                                    at iIndexArg.
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGSetRegister(int iIndexArg, long lValueArg)
{
    int iReturn;

    /* validate the register index/number */
    if (RGIsRegisterIndexValid(iIndexArg)==SUCCESS)
    {
        RGplRegister[iIndexArg] = lValueArg;
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGGetCharacter()
 *
 * Description:    Get the character in C-Register at specific index
 *
 * Parameters:
 *              iIndexArg           - index of array or character index
 *              pcReturnValueArg    - character at iIndexArg (return value)
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGGetCharacter(int iIndexArg, char  *pcReturnValueArg)
{
    int iReturn;

    /* validate the C-register index/number */
    if (RGIsCharacterIndexValid(iIndexArg)==SUCCESS)
    {
        *pcReturnValueArg = RGpcCharacter[iIndexArg];
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGSetCharacter()
 *
 * Description:    Set the character in C-register at specific index
 *
 * Parameters:
 *              iIndexArg           - index of array or character index
 *              cValueArg           - character value to be set at iIndexArg
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGSetCharacter(int iIndexArg, char cValueArg)
{
    int iReturn;

    /* validate C-register index/number */
    if (RGIsCharacterIndexValid(iIndexArg)==SUCCESS)
    {
        RGpcCharacter[iIndexArg] = cValueArg;
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGGetString()
 *
 * Description:    Get the string in S-register at specific index
 *              NOTE: always clear the string before setting to it.
 *                    Also, string always ends with null character.
 *
 * Parameters:
 *              iIndexArg           - index of array or string index
 *              pcReturnValueArg    - string value at iIndexArg (return value)
 *              iSecsFlagArg        - if it gets called for Secs comm., then we need to copy
 *                                      all 256 bytes even they contain "\0" character
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGGetString(int iIndexArg, char  *pcReturnValueArg, int iSecsFlagArg)
{
    int iReturn;

   /* validate the S-register index/number */
    if (RGIsStringIndexValid(iIndexArg)==SUCCESS)
    {
        /* clear the return string value before store the value and return */
        pcReturnValueArg[0] = 0;
        if(iSecsFlagArg)
            memcpy(pcReturnValueArg, RGpacString[iIndexArg], MAXSTRINGLENGTH);
        else
            strcpy(pcReturnValueArg, RGpacString[iIndexArg]);
        pcReturnValueArg[strlen(RGpacString[iIndexArg])] = '\0';
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGSetString()
 *
 * Description:    Set the string in S-register at specific index.
 *              if the length of the string is longer than the S-register
 *              string length, then the string will get truncated to the
 *              maximum length the S-register can handle.
 *              NOTE: always clear the string before setting to it.
 *                    Also, string always ends with null character.
 *
 * Parameters:
 *              iIndexArg           - index of array or string index
 *              pcValueArg          - string value to be set at iIndexArg
 *              iSecsFlagArg        - if it gets called for Secs comm., then we need to copy
 *                                      all 256 bytes even they contain "\0" character
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGSetString(int iIndexArg, char *pcValueArg, int iSecsFlagArg)
{
    int iReturn;
	int nLen = 0;

    /* validate the S-register index/number */
    if (RGIsStringIndexValid(iIndexArg)==SUCCESS)
    {
        /* clear register before setting the value to it */
        *RGpacString[iIndexArg] = '\0';
        /* in the case of the string length is longer than 256,
         * we truncate the string to 256 and then set the value to
         * S-register at iIndexArg.*/
        if(strlen(pcValueArg) > MAXSTRINGLENGTH)
        {
            strncpy(RGpacString[iIndexArg], pcValueArg, MAXSTRINGLENGTH);
            RGpacString[iIndexArg][MAXSTRINGLENGTH-1] = '\0';
        }
        else
        {
            /* if the string length is less than 256 then just
             * set the S-register at iIndexArg.*/
            if(iSecsFlagArg)
	    {
                memcpy(RGpacString[iIndexArg], pcValueArg, MAXSTRINGLENGTH);

//int i;
//for(i=0; i<20; ++i)
//printf("RGSetString: i=%d RG=%2x pcVal=%2x\n",i,RGpacString[iIndexArg][i],pcValueArg[i]);

	    }
            else
            {
                strcpy(RGpacString[iIndexArg], pcValueArg);
                nLen = strlen(RGpacString[iIndexArg]);
                RGpacString[iIndexArg][nLen] = '\0';
            }
        }
        iReturn = SUCCESS;
    }
    else
    {
        iReturn = FAILURE;
    }
    return iReturn;
}

/****************************************************************\
 *
 * Function:    RGIsSpecialRegisterIndexValid()
 *
 * Description:    validate the Register index, to make sure it is in the range [0-25]
 * Note:        FOR NOVELLUS ONLY.
 *
 * Parameters:
 *              iIndexArg           - index to validate
 *
 * Returns:     return FAILURE or SUCCESS   ;  if FAILURE, index is bad
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGIsSpecialRegisterIndexValid(int iIndexArg)
{
    if (iIndexArg >= 0 && iIndexArg <= MAXSPECIALREG)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

/****************************************************************\
 *
 * Function:    RGIsRegisterIndexValid()
 *
 * Description:    validate the Register index, to make sure it is in the range [0-100]
 *
 * Parameters:
 *              iIndexArg           - index to validate
 *
 * Returns:     return FAILURE or SUCCESS   ;  if FAILURE, index is bad
 *              FAILURE is -1  and SUCCESS is 0
 *
\*****************************************************************/
int RGIsRegisterIndexValid(int iIndexArg)
{
    if (iIndexArg >= 0 && iIndexArg <= MAXREG)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

/****************************************************************
 *
 * Function:    RGIsCharacterIndexValid()
 *
 * Description:    validate the character index, to make sure it is in the range [0-100]
 *
 * Parameters:
 *              iIndexArg           - index to validate
 *
 * Returns:     return FAILURE or SUCCESS   ;  if FAILURE, index is bad
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGIsCharacterIndexValid(int iIndexArg)
{
    if (iIndexArg >= 0 && iIndexArg <= MAXCHAR)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

/****************************************************************
 *
 * Function:    RGIsStringIndexValid()
 *
 * Description:    validate the string index, to make sure it is in the range [0-20]
 *
 * Parameters:
 *              iIndexArg           - index to validate
 *
 * Returns:     return FAILURE or SUCCESS   ;  if FAILURE, index is bad
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGIsStringIndexValid(int iIndexArg)
{
    if (iIndexArg >= 0 && iIndexArg <= MAXSTRINGVARS)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

/****************************************************************
 *
 * Function:    RGCompareNumNum
 *
 * Description:    compare numberic with numberic.
 *
 * Parameters:
 *              iLeftValueArg           - left value
 *              iRightValueArg          - right value
 *
 * Returns:     return FAILURE or SUCCESS   ;  if FAILURE, index is bad
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGCompareNumNum(long lLeftValueArg, long lRightValueArg)
{
    long lReturn;

    /* subtract right value from left value  and set the difference to
     * carrier flag, so condition jump command will use this carrier flag value
     * to jump to >, < or =.
     * if value  < 0, then less than
     * if value > 0, then greater than
     * if value = 0, then equal*/
    lReturn = lLeftValueArg - lRightValueArg;

    MASetCf(lReturn);
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    RGCompareStrStr
 *
 * Description:    compare string with string
 *
 * Parameters:
 *              pcLeftStringArg           - left value
 *              pcRightStringArg          - right value
 *
 * Returns:     return FAILURE or SUCCESS   ;  if FAILURE, index is bad
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGCompareStrStr(char *pcLeftStringArg, char *pcRightStringArg)
{
    /* subtract right value from left value  and set the difference to
     * carrier flag, so condition jump command will use this carrier flag value
     * to jump to >, < or =.
     * if value  < 0, then less than
     * if value > 0, then greater than
     * if value = 0, then equal*/
    MASetCf(strcmp(pcLeftStringArg, pcRightStringArg));
	return SUCCESS;
}

/****************************************************************
 *
 * Function:    RGConcatStrStr
 *
 * Description: Concatenate string and string.
 *              if the length of the left and right strings are longer than the S-register
 *              string length(256bytes), then the string will get truncated to the
 *              maximum length the S-register can handle.
 *
 *              NOTE: always clear the string before setting to it.
 *                    Also, string always ends with null character.
 *
 * Parameters:
 *              pcLeftStringArg           - left value
 *              pcRightStringArg          - right value
 *
 * Returns:     SUCCESS OR FAILURE
 *
 *****************************************************************/
int RGConcatStrStr(char *pcLeftStringArg, char *pcRightStringArg)
{
    char pString[MAXSTRINGLENGTH * 3];    /* temporary string buffer to hold the concatenated string */
    int nLen = 0;           /* holds the string length */
    int iCmdPort;           /* for command port id */

   /* get command port */
    iCmdPort = SERGetCmdPort();

    /* Even though max string is 256, we still allocate the temp string to hold
     * left and right string that passed in from the function arguements. So, we can
     * void of memory problem or crash at the run time. Therefore, allocated 3 times of
     * the MAXSTRINGLENGTH.*/
//    pString = (char *)HPmalloc(MAXSTRINGLENGTH * 3);
//    pString = (char *)malloc(MAXSTRINGLENGTH * 3);

    /* In the case of pString can allocate the memory, we print out the message to the
     * use. So we know what happen to the system at that time. */
//    if(!pString)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***RGConcatStrStr: out of memory.****\n\r?");
//#endif
//        return FAILURE;
//    }

    strcpy(pString, pcLeftStringArg);
    strcat(pString, pcRightStringArg);
    /* get the string length of both string */
    nLen = strlen(pcRightStringArg) + strlen(pcLeftStringArg);

    /* check if the length > MAXSTRINGLENGTH, then truncate the string to 256 length only*/
	if(nLen > MAXSTRINGLENGTH)
	{
        /* truncate the string by copy string up to MAXSTRINGLENGTH */
        strncpy(pcLeftStringArg, pString, MAXSTRINGLENGTH);
        pcLeftStringArg[MAXSTRINGLENGTH-1] = '\0';
	}
    else
    {
        strcpy(pcLeftStringArg, pString);
        pcLeftStringArg[nLen] = '\0';
    }

//    HPfree(pString);  /* free the temporary buffer */
//    free(pString);  /* free the temporary buffer */
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    RGConvertStrToNum
 *
 * Description: Convert string to number and store the value into numberic variable.
 *
 * Parameters:
 *              pcStrArg                - string to be converted to numberic.
 *              lReturnNumberArg        - store return value.
 *              iRadixArg               - base of convertion in HEX or DEC. (base 10 or 16)
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGConvertStrToNum(char *pcStrArg, long *lReturnNumberArg,int iRadixArg)
{
    char pBuffer[MAXSTRINGLENGTH + 2];       /* temporary buffer */
    int iCmdPort;              /* command port id */

    iCmdPort = SERGetCmdPort();

//    pBuffer = (char *) HPmalloc(MAXSTRINGLENGTH+2);
//    pBuffer = (char *) malloc(MAXSTRINGLENGTH+2);
//    if(!pBuffer)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***RGConvertStrToNum: out of memory.****\n\r?");
//#endif
//        return FAILURE;
//    }

    /* copy to temporary buffer before convert, so the value will not get overwritten */
    strcpy((char *)pBuffer, (char *)pcStrArg);

    /* convert the string to long */
    *lReturnNumberArg = strtol(pBuffer, NULL, iRadixArg);
//    HPfree(pBuffer);                /* free memory of temporary buffer */
//    free(pBuffer);                /* free memory of temporary buffer */
	return SUCCESS;
}

/****************************************************************
 *
 * Function:    RGConvertStrToNumStrReg
 *
 * Description: Convert the string to number and store value into R-register.
 *
 * Parameters:
 *              pcStrArg                - string to be converted to number.
 *              iReturnRegIndexArg      - store the return value.
 *              iRadixArg               - base of convertion in HEX or DEC. (base 10 or 16)
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGConvertStrToNumStrReg(char *pcStrArg, int iReturnRegIndexArg, int iRadixArg)
{
    char pBuffer[MAXSTRINGLENGTH + 2];          /* temporary buffer */
    int iCmdPort;                 /* command port id */

    iCmdPort = SERGetCmdPort();

//    pBuffer = (char *) HPmalloc(MAXSTRINGLENGTH+2);
//    pBuffer = (char *) malloc(MAXSTRINGLENGTH+2);
//    if(!pBuffer)
//    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***RGConvertStrToNumStrReg: out of memory.****\n\r?");
//#endif
//        return FAILURE;
//    }
    /* copy the string into temporary buffer, so the value will not get overwritten */
    strcpy((char *)pBuffer, (char *)pcStrArg);

    /* convert the string to long */
    RGplRegister[iReturnRegIndexArg] = strtol(pBuffer, NULL, iRadixArg);
//    HPfree(pBuffer);               /* free memory of temporary buffer */
//    free(pBuffer);               /* free memory of temporary buffer */
	return SUCCESS;

}

/****************************************************************
 *
 * Function:    RGConvertNumToStrNumSReg
 *
 * Description: Convert the number to string and store the string into S-register.
 *
 * Parameters:
 *              lNumberArg          - number to be converted.
 *              iStringIndexArg     - S-register index stores the return value of string.
 *              iRadixArg           - base of convertion in HEX or DEC. (base 10 or 16)
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGConvertNumToStrNumSReg(long lNumberArg, int iStringIndexArg, int iRadixArg)
{
    /* convert the number to ascii */
	// ltoa(lNumberArg, RGpacString[iStringIndexArg], iRadixArg);
    if(iRadixArg == 10)
        sprintf(RGpacString[iStringIndexArg],"%ld",lNumberArg);
    else
        sprintf(RGpacString[iStringIndexArg],"%lX",lNumberArg);
    return SUCCESS;
}

/****************************************************************
 *
 * Function:    RGEqualSRegSReg
 *
 * Description: Assign the right string value from S-register into S-register
 *              from the left side of equal sign.
 *
 * Parameters:
 *              iLeftIndexArg           - S-register index of the left value
 *              iRightIndexArg         -  S-register index fof the right value
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualSRegSReg(int iLeftIndexArg, int iRightIndexArg)
{
    return(RGSetString(iLeftIndexArg, RGpacString[iRightIndexArg],0));
}

/****************************************************************
 *
 * Function:    RGEqualSRegStr
 *
 * Description: Assign the right string value into S-register
 *              from the left side of equal sign.
 *
 * Parameters:
 *              iLeftIndexArg           - S-register index of the left value
 *              pcRightValueArg         - string value to set into S-register
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualSRegStr(int iLeftIndexArg, char *pcRightValueArg)
{
    return(RGSetString(iLeftIndexArg, pcRightValueArg,0));
}

/****************************************************************
 *
 * Function:    RGEqualCRegCReg
 *
 * Description: Assign the right character value from C-register into C-register
 *              from the left side of equal sign.
 *
 * Parameters:
 *              iLeftIndexArg           - C-register index of the left value
 *              iRightIndexArg          - C-register index of the right value
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualCRegCReg(int iLeftIndexArg, int iRightIndexArg)
{
	return(RGSetCharacter(iLeftIndexArg, RGpcCharacter[iRightIndexArg]));
}

/****************************************************************
 *
 * Function:    RGEqualCRegChar
 *
 * Description: Assign the right character value into C-register
 *              from the left side of equal sign.
 *
 * Parameters:
 *              iLeftIndexArg           - C-register index of the left value
 *              cRightValueArg          - a character to set into C-register
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualCRegChar(int iLeftIndexArg, char cRightValueArg)
{
	return (RGSetCharacter(iLeftIndexArg, cRightValueArg));
}

/****************************************************************
 *
 * Function:    RGEqualRegReg
 *
 * Description: Assign the right numeric value from R-register into R-register
 *              from the left side of equal sign.
 *
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              iRightIndexArg          - R-register index of the right value
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualRegReg(int iLeftIndexArg, int iRightIndexArg)
{
	return(RGSetRegister(iLeftIndexArg, RGplRegister[iRightIndexArg]));
}

/****************************************************************
 *
 * Function:    RGEqualRegNum
 *
 * Description: Assign the right numeric value into R-register
 *              from the left side of equal sign.
 *
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              lRightValueArg          - right value
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualRegNum(int iLeftIndexArg, long lRightValueArg)
{
	return(RGSetRegister(iLeftIndexArg, lRightValueArg));
}

/****************************************************************
 *
 * Function:    RGEqualSpecialRegReg
 *
 * Description: Assign the R-register to another R-register (for novelus only)
 *
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              iRightIndexArg          - R-register index of the right value
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualSpecialRegReg(int iLeftIndexArg, int iRightIndexArg)
{
    return(RGSetSpecialRegister(iLeftIndexArg, RGplRegister[iRightIndexArg]));
}

/****************************************************************
 *
 * Function:    RGEqualSpecialRegNum
 *
 * Description: Assign the right value to R-register (for novelus only)
 *
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              lRightValueArg          - right value
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGEqualSpecialRegNum(int iLeftIndexArg, long lRightValueArg)
{
    return(RGSetSpecialRegister(iLeftIndexArg, lRightValueArg));
}

/****************************************************************
 *
 * Function:    RGExtractCharacterStrCReg
 *
 * Description: Retrieve character from S-register and store into C-register.
 *              1. Need to check for S-register index is valid
 *              2. Check for start position for retrieve is valid with 0-255
 *
 * NOTE:        THIS FUNCTION IS IMPLEMENTED BUT IT HAS NEVER BEEN USED IN THE
 *              6.00 I2BRT PROJECT. IT IS JUST FOR FUTURE USE.  IF ANY ONE WANT TO
 *              USE, YOU NEED TO MAKE SURE THE FUNCTION IS WORKING PROPERLY SINCE IT
 *              IS NOT TESTED OR DEBUG PROPERLY. (THERE MAY BE BUG)
 *
 * Parameters:
 *              pcString               - string pointer
 *              iStartIndexArg          - position in the string that string will be inserted into.
 *              nLenArg                 - number of characters to be retrieved.
 *              iIndexReturnCRegArg     - store return value of character into C-register.
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGExtractCharacterStrCReg(char *pcString, int iStartIndexArg, int nLenArg, int iIndexReturnCRegArg)
{
    if(iStartIndexArg < 0 || iStartIndexArg > 255)
    {
        printf("RGExtractStringStrCReg:  bad index %d\n", iStartIndexArg);
        return FAILURE;
    }
    if(RGIsCharacterIndexValid(iIndexReturnCRegArg))
    {
        printf("RGExtractStringStrCReg: bad index %d\n", iIndexReturnCRegArg);
        return FAILURE;
    }
    RGpcCharacter[iIndexReturnCRegArg] = pcString[iStartIndexArg];
    return SUCCESS;
}

/***************************************************************
 *
 * Function:    RGInsertStringSRegChar
 *
 * Description: Insert character into string in S-register.
 *              1. Need to check for S-register index is valid
 *              2. Check for position is valid with 0-255
 *              3. copy character into string[index][position]
 *
 * Parameters:
 *              iIndexArg       - S-register index
 *              iPositionArg    - position in the string that character will be inserted into.
 *              cInsertCharArg  - character to be inserted.
 *
 * Returns:     return FAILURE or SUCCESS
 *              FAILURE is -1  and SUCCESS is 0
 *
 *****************************************************************/
int RGInsertStringSRegChar(int iIndexArg, int iPositionArg, char cInsertCharArg)
{
    /* validate the S-register index */
    if(RGIsStringIndexValid(iIndexArg))
    {
        printf("RGInsertStringSRegChar: bad index %d\n", iIndexArg);
        return FAILURE;
    }
    /* check for position is valid in the range of 0-255 */
    if(iPositionArg <0 || iPositionArg > 255)
    {
        printf("RGInsertStringSRegChar:  bad index %d\n", iPositionArg);
        return FAILURE;
    }

    /*  insert the character into the string at iPositionArg */
    RGpacString[iIndexArg][iPositionArg] = cInsertCharArg;
    return SUCCESS;
}
