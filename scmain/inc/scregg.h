/***************************************************************\
 *
 * Program:     Register R, C, and S
 * File:        rgglobal.h
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
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_RGGLOBAL_H
#define _H_RGGLOBAL_H

/********** DEFINES **********/

//#define MAXREG              100          /* Number of R-variables */
#define MAXREG              200          /* Number of R-variables */
#define MAXSPECIALREG       25           /* Number of Special R-variables */
#define MAXCHAR             100          /* Number of C-variables */
#define MAXSTRINGVARS       20           /* Number of S-variables */
#define MAXSTRINGLENGTH     256          /* Maximum Length of each S-Variable */


/********** FUNCTION PROTOTYPES **********/

/****************************************************************\
 * Function:    RGInit()
 * Description:    Allocate the memory for R,C,S register and
 *              initialize.
\*****************************************************************/
int RGInit();
/****************************************************************\
 * Function:    RGInitSpecialRegister()
 * Description:    Initialize the special R-Registers
\*****************************************************************/
int RGInitSpecialRegister();
/****************************************************************\
 * Function:    RGGetRegister()
 * Description:    Get the value of R-register at specific index
 * Parameters:
 *              iIndexArg           - index of array or register number
 *              plReturnValueArg    - long value of R[iIndex] (return value)
\*****************************************************************/
int RGGetRegister(int iIndexArg, long *plReturnValueArg);
/****************************************************************\
 * Function:    RGSetRegister()
 * Description:    Set the value of R-register at specific index
 * Parameters:
 *              iIndexArg           - index of array or register number
 *              lValueArg           - long value of R-register to be set
 *                                    at iIndexArg.
\*****************************************************************/
int RGSetRegister(int iIndexArg, long lValueArg);
/****************************************************************\
 * Function:    RGGetSpecialRegister()
 * Description:    Get the value of specific special R-register by providing the
 *              register number/index.
 * Parameters:
 *              iIndexArg           - index of array or register number
 *              plReturnValueArg    - long value of R[iIndex];
 *                                    it's a return value.
\*****************************************************************/
int RGGetSpecialRegister(int iIndexArg, long *plReturnValueArg);
/****************************************************************\
 * Function:    RGSetSpecialRegister()
 * Description:    Set the value of special R-register by providing specific
 *              register number/index.
 * Parameters:
 *              iIndexArg    - index of array or register number
 *              lValueArg    - long value of special R-register to set
 *                             at iIndexArg.
\*****************************************************************/
int RGSetSpecialRegister(int iIndexArg, long lValueArg);
/****************************************************************\
 * Function:    RGGetCharacter()
 * Description:    Get the character in C-Register at specific index
 * Parameters:
 *              iIndexArg           - index of array or character index
 *              pcReturnValueArg    - character at iIndexArg (return value)
\*****************************************************************/
int RGGetCharacter(int iIndexArg, char  *pcReturnValueArg);
/****************************************************************\
 * Function:    RGSetCharacter()
 * Description:    Set the character in C-register at specific index
 * Parameters:
 *              iIndexArg           - index of array or character index
 *              cValueArg           - character value to be set at iIndexArg
\*****************************************************************/
int RGSetCharacter(int iIndexArg, char cValueArg);
/****************************************************************\
 * Function:    RGGetString()
 * Description:    Get the string in S-register at specific index
 *              NOTE: always clear the string before setting to it.
 *                    Also, string always ends with null character.
 * Parameters:
 *              iIndexArg           - index of array or string index
 *              pcReturnValueArg    - string value at iIndexArg (return value)
 *              iSecsFlagArg        - tells if we use strcpy or memcpy in function,
 *                                      created for Secs comm.
\*****************************************************************/
int RGGetString(int iIndexArg, char  *pcReturnValueArg, int iSecsFlagArg);
/****************************************************************\
 * Function:    RGSetString()
 * Description:    Set the string in S-register at specific index.
 *              if the length of the string is longer than the S-register
 *              string length, then the string will get truncated to the
 *              maximum length the S-register can handle.
 *              NOTE: always clear the string before setting to it.
 *                    Also, string always ends with null character.
 * Parameters:
 *              iIndexArg           - index of array or string index
 *              pcValueArg          - string value to be set at iIndexArg
 *              iSecsFlagArg        - tells if we use strcpy or memcpy in function,
 *                                      created for Secs comm.
\*****************************************************************/
int RGSetString(int iIndexArg, char  *pcValueArg, int iSecsFlagArg);
/****************************************************************\
 * Function:    RGIsSpecialRegisterIndexValid()
 * Description:    validate the Register index, to make sure it is in the range [0-25]
 * Note:        FOR NOVELLUS ONLY.
 * Parameters:
 *              iIndexArg           - index to validate
\*****************************************************************/
int RGIsSpecialRegisterIndexValid(int iIndexArg);
/****************************************************************\
 * Function:    RGIsRegisterIndexValid()
 * Description:    validate the Register index, to make sure it is in the range [0-100]
 * Parameters:
 *              iIndexArg           - index to validate
\*****************************************************************/
int RGIsRegisterIndexValid(int iIndexArg);
/****************************************************************
 * Function:    RGIsCharacterIndexValid()
 * Description:    validate the character index, to make sure it is in the range [0-100]
 * Parameters:
 *              iIndexArg           - index to validate
 *****************************************************************/
int RGIsCharacterIndexValid(int iIndexArg);
/****************************************************************
 * Function:    RGIsStringIndexValid()
 * Description:    validate the string index, to make sure it is in the range [0-20]
 * Parameters:
 *              iIndexArg           - index to validate
 *****************************************************************/
int RGIsStringIndexValid(int iIndexArg);
/****************************************************************
 * Function:    RGCompareNumNum
 * Description:    compare numberic with numberic.
 * Parameters:
 *              iLeftValueArg           - left value
 *              iRightValueArg          - right value
 *****************************************************************/
int RGCompareNumNum(long lLeftValueArg, long lRightValueArg);
/****************************************************************
 * Function:    RGCompareStrStr
 * Description:    compare string with string
 * Parameters:
 *              pcLeftStringArg           - left value
 *              pcRightStringArg          - right value
 *****************************************************************/
int RGCompareStrStr(char *pcLeftStringArg, char *pcRightStringArg);
/****************************************************************
 * Function:    RGConcatStrStr
 * Description: Concatenate string and string.
 *              if the length of the left and right strings are longer than the S-register
 *              string length(256bytes), then the string will get truncated to the
 *              maximum length the S-register can handle.
 *              NOTE: always clear the string before setting to it.
 *                    Also, string always ends with null character.
 * Parameters:
 *              pcLeftStringArg           - left value
 *              pcRightStringArg          - right value
 *****************************************************************/
int RGConcatStrStr(char *pcLeftStringArg, char *pcRightStringArg);
/****************************************************************
 * Function:    RGConvertStrToNum
 * Description: Convert string to number and store the value into numberic variable.
 * Parameters:
 *              pcStrArg                - string to be converted to numberic.
 *              lReturnNumberArg        - store return value.
 *              iRadixArg               - base of convertion in HEX or DEC. (base 10 or 16)
 *****************************************************************/
int RGConvertStrToNum(char *pcStrArg, long *lReturnNumberArg, int iRadixArg);
/****************************************************************
 * Function:    RGConvertStrToNumStrReg
 * Description: Convert the string to number and store value into R-register.
 * Parameters:
 *              pcStrArg                - string to be converted to number.
 *              iReturnRegIndexArg      - store the return value.
 *              iRadixArg               - base of convertion in HEX or DEC. (base 10 or 16)
 *****************************************************************/
int RGConvertStrToNumStrReg(char *pcStrArg, int iReturnRegIndexArg, int iRadixArg);
/****************************************************************
 * Function:    RGConvertNumToStrNumSReg
 * Description: Convert the number to string and store the string into S-register.
 * Parameters:
 *              lNumberArg          - number to be converted.
 *              iStringIndexArg     - S-register index stores the return value of string.
 *              iRadixArg           - base of convertion in HEX or DEC. (base 10 or 16)
 *****************************************************************/
int RGConvertNumToStrNumSReg(long lNumberArg, int iStringIndexArg, int iRadixArg);
/****************************************************************
 * Function:    RGEqualSRegSReg
 * Description: Assign the right string value from S-register into S-register
 *              from the left side of equal sign.
 * Parameters:
 *              iLeftIndexArg           - S-register index of the left value
 *              iRightIndexArg         -  S-register index fof the right value
 *****************************************************************/
int RGEqualSRegSReg(int iLeftIndexArg, int iRightIndexArg);
/****************************************************************
 * Function:    RGEqualSRegStr
 * Description: Assign the right string value into S-register
 *              from the left side of equal sign.
 * Parameters:
 *              iLeftIndexArg           - S-register index of the left value
 *              pcRightValueArg         - string value to set into S-register
 *****************************************************************/
int RGEqualSRegStr(int iLeftIndexArg, char *pcRightValueArg);
/****************************************************************
 * Function:    RGEqualCRegCReg
 * Description: Assign the right character value from C-register into C-register
 *              from the left side of equal sign.
 * Parameters:
 *              iLeftIndexArg           - C-register index of the left value
 *              iRightIndexArg          - C-register index of the right value
 *****************************************************************/
int RGEqualCRegCReg(int iLeftIndexArg, int iRightIndexArg);
/****************************************************************
 * Function:    RGEqualCRegChar
 * Description: Assign the right character value into C-register
 *              from the left side of equal sign.
 * Parameters:
 *              iLeftIndexArg           - C-register index of the left value
 *              cRightValueArg          - a character to set into C-register
 *****************************************************************/
int RGEqualCRegChar(int iLeftIndexArg, char cRightValueArg);
/****************************************************************
 * Function:    RGEqualRegReg
 * Description: Assign the right numeric value from R-register into R-register
 *              from the left side of equal sign.
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              iRightIndexArg          - R-register index of the right value
 *****************************************************************/
int RGEqualRegReg(int iLeftIndexArg, int iRightIndexArg);
/****************************************************************
 * Function:    RGEqualRegNum
 * Description: Assign the right numeric value into R-register
 *              from the left side of equal sign.
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              lRightValueArg          - right value
 *****************************************************************/
int RGEqualRegNum(int iLeftIndexArg, long lRightValueArg);
/****************************************************************
 * Function:    RGEqualSpecialRegReg
 * Description: Assign the R-register to another R-register (for novelus only)
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              iRightIndexArg          - R-register index of the right value
 *****************************************************************/
int RGEqualSpecialRegReg(int iLeftIndexArg, int iRightIndexArg);
/****************************************************************
 * Function:    RGEqualSpecialRegNum
 * Description: Assign the right value to R-register (for novelus only)
 * Parameters:
 *              iLeftIndexArg           - R-register index of the left value
 *              lRightValueArg          - right value
 *****************************************************************/
int RGEqualSpecialRegNum(int iLeftIndexArg, long lRightValueArg);
/****************************************************************
 * Function:    RGExtractCharacterStrCReg
 * Description: Retrieve character from S-register and store into C-register.
 *              1. Need to check for S-register index is valid
 *              2. Check for start position for retrieve is valid with 0-255
 * NOTE:        THIS FUNCTION IS IMPLEMENTED BUT IT HAS NEVER BEEN USED IN THE
 *              6.00 I2BRT PROJECT. IT IS JUST FOR FUTURE USE.  IF ANY ONE WANT TO
 *              USE, YOU NEED TO MAKE SURE THE FUNCTION IS WORKING PROPERLY SINCE IT
 *              IS NOT TESTED OR DEBUG PROPERLY. (THERE MAY BE BUG)
 * Parameters:
 *              pcString               - string pointer
 *              iStartIndexArg          - position in the string that string will be inserted into.
 *              nLenArg                 - number of characters to be retrieved.
 *              iIndexReturnCRegArg     - store return value of character into C-register.
 *****************************************************************/
int RGExtractCharacterStrCReg(char *pcString, int iStartIndexArg, int nLenArg, int iIndexReturnCRegArg);
/***************************************************************
 * Function:    RGInsertStringSRegChar
 * Description: Insert character into string in S-register.
 *              1. Need to check for S-register index is valid
 *              2. Check for position is valid with 0-255
 *              3. copy character into string[index][position]
 * Parameters:
 *              iIndexArg       - S-register index
 *              iPositionArg    - position in the string that character will be inserted into.
 *              cInsertCharArg  - character to be inserted.
 *****************************************************************/
int RGInsertStringSRegChar(int iIndexArg, int iPositionArg, char cInsertCharArg);

#endif
