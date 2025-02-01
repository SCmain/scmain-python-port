/***************************************************************\
 *
 * Program:     Commands whose use are internal to the lowlevel execution function
 *              ONLY.
 *
 * File:        cmdsp.h
 *
 * Functions:       CMDstringEqual
 *                  CMDstationNumber
 *                  CMDstationCharacter
 *                  CMDgetStnIndx
 *                  CMDgetCharacter
 *                  CMDgetValue
 *                  CMDgetString
 *                  CMDgetAxis
 *                  CMDgetGalilParms
 *                  CMDgetStationParameters
 *                  CMDgetStation
 *                  CMDdumpVariables
 *                  CMDdumpBuf
 *                  CMDdumpLparms
 *                  CMDreadLparms
 *                  CMDreadPort
 *                  CMDreadBackParms
 *                  CMDsendSTAT
 *                  CMDsendSTAT2
 *                  CMDisNum
 *                  CMDisCharacter
 *                  CMDisString
 *                  CMDgetAxisLoc
 *                  CMDShowStatDescription
 *                  CMDShowStat2Description
 *                  CMDgetDecryptedPassword
 *
 * Description: Commands whose use is restricted to the lowlevel execution functions.
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
 *
\***************************************************************/

#ifndef _H_CMDSP_H
#define _H_CMDSP_H

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
int CMDstringEqual(int iRegister, CMDoperand_ptr Optr);

/********************************************************************************
 * Function Name:   CMDstationNumber
 * Description:     Convert the character to the station index, where the index is represented as
 *                  follows:
 *                          A-Z (ASCII 65- 90) has indices 0-26
 *                          a-z (ascii 97-122) has indices 26-51
 * Parameter:       c - Character representation of the station index
 * Returned Value:  The numeric index of the station
 ********************************************************************************/
int CMDstationNumber(char c);

/********************************************************************************
 * Function Name:   CMDstationCharacter
 * Description:     Convert the station index to a character, where the character is represented as
 *                  follows:
 *                          A-Z (ASCII 65- 90) has indices 0-26
 *                          a-z (ascii 97-122) has indices 26-51
 * Parameter:       i - Character representation of the station index
 * Returned Value:  The character representation of the station
 ********************************************************************************/
int CMDstationCharacter(int i);

/********************************************************************************
 * Funtion Name:    CMDgetStnIndx
 * Description:     Get the station name from an operand and convert to an index and return it.
 * Parameter:       Optr - An operand Pointer
 * Returned Value:  The station index which is computed as follows:
 *                  A-Z (ASCII 65- 90) will have indices 0-25 respectively
 *                  a-z (ASCII 97-122) will have indices 26-51 respectively
 *                  FAILURE will be returned if there was a problem
 ********************************************************************************/
int CMDgetStnIndx(CMDoperand_ptr Optr);

/********************************************************************************
 * Function Name:   CMDgetCharacter
 * Description:     Get a character value from constant type operand or indirect
 *                  C-register type operand and return the value
 * NOTE:            make sure to check for operand is not NULL before passing the
 *                  operand pointer to this function.
 * Parameters:      Optr - Operand Pointer
 * Returned Value : The character contained in the operand pointer
 ********************************************************************************/
int CMDgetCharacter(CMDoperand_ptr Optr, char *cRetArg);

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
int CMDgetValue(CMDoperand_ptr Optr, long *lVal);

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
int CMDgetString(CMDoperand_ptr OptrArg, char *sRetVal, int iSecFlagArg);

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
ULONG CMDgetAxis(instr_ptr instr);

/********************************************************************************
 * Function Name:   CMDgetGalilParms
 * Description:     This is an internal Lowlevel command which pulls the parameters necessary for the
 *                  galil functions from the instruction pointer. Get value of each axis from the instruction pointer
 *                  to set the information to galil.
 *                  Example:  ER A, 100,100,100. This functin will get all 3 values of three axes (T,R,Z) and put into
 *                  an array. The ex_XXX will use this array to pass into the Mechanism function (MExxx) to set the value
 *                  to the galil.
 * NOTE:            FIXME (ANNA)- For now, this function is using for any command the read and set value to galil, but
 *                  I think there is something doesn't make sense.  It is more making sense if this function is
 *                  for set command only. For read, you have not had any information to read before you get the
 *                  information from the galil. When you call CMDgetValue(), you will all garbage value to store into an
 *                  array, but I believed that for reading data value from galil you don't need to this function(CMDgetGalilParms)
 *                  For future, if there is any problem, please talk with Arin, or Anna.
 * Parameter:       instr - The instruction Pointer
 * OUTPUT:          ulAxis - a long representation of the Axis specified
 *                  lParms - an array of longs to contain the parameters for the
 *                           galil function
 * Returned Value:     SUCCESS or FAILURE
 ********************************************************************************/
int CMDgetGalilParms(instr_ptr instr, ULONG *ulAxis, long *lParms);

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
int CMDgetStationParameters(instr_ptr instr, long *lvaluePtr);

/*************************************************************************************
 * Function Name:   CMDgetStation
 * Description:     Get a station name from the operand pointer and validate the station
 *                  name.  Then convert the station name from alphabet to numeric and return
 *                  numeric value.
 * Note:            This function is not called anywhere in the code.
 * Parameter:       Optr - An Operand Pointer
 * Returned Value:  The Stations index
 *************************************************************************************/
int CMDgetStation(CMDoperand_ptr Optr);

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
int CMDdumpVariables(CMDoperand_ptr Optr, int iRadix, int iPort);

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
int CMDdumpBuf(char *sBuf);

/*************************************************************************************
 * Function Name:   CMDdumpLparms
 * Description:     Dump Long parameters value when request for all 3 axis information.
 *                  It is used a lot of place for outputing the parameter file information such
 *                  as ex_DUMPP.
 * Parameter:       lPtr - A Pointer to the parameters to be dumped
 * Returned Value:  SUCCESS or FAILURE
 *************************************************************************************/
int CMDdumpLparms(long *lPtr);

/*************************************************************************************
 * Function Name:   CMDreadLparms
 * Description:     get values from the com port and output the '\r'. This function is
 *                  used for download the parameter file (WRIP).
 * OUTPUT:          lValues - the passed information with additions
 * Returned Value: SUCCESS or FAILURE
 *************************************************************************************/
int CMDreadLparms(long *lValues);

/*************************************************************************************
 * Function Name:   CMDreadPort
 * Description:     A general routine to read an entire port, or just a single bit from a port.  This
 *                  routine is used by the I/0 lowlevel command only.
 * Parameters:      lBitNum - if 0-39, than this is the bit number for a bit read
 *                            if -1, than this is used to indicate a port read whole port.
 *                  cPortID - if (A-J), than read from the port, otherwise 0
 * Returned Value: The byte/bit read or FAILURE
 *************************************************************************************/
int CMDreadPort(long lBitNum, char cPortID);

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
int CMDreadBackParms(CMDoperand_ptr Optr, long *lValue, ULONG ulAxis);

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
 *                  iLongDesc - print out long descriptions
 * Returned Value:  SUCCESS or FAILURE
 *************************************************************************************/
int CMDsendSTAT(instr_ptr instr, int iStatus, int iLongDesc);

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
int CMDsendSTAT2(instr_ptr instr, int iStatus, int iStatus2 );

/*************************************************************************************
 * Function Name:   CMDisNum
 * Description:     Validates if the operand is a LONG_CONST or INDIRECT_REG
 * Parameter:       Optr - Operand pointer
 * Returns:         SUCCESS/FAILURE
 *************************************************************************************/
int CMDisNum(CMDoperand_ptr Optr);

/*************************************************************************************
 * Function Name:   CMDisCharacter
 * Description:     Validates if the operand is a CHAR_CONST or INDIRECT_CHAR.
 * Parameter:       Optr - Operand pointer.
 * Returns:         SUCCESS/FAILURE
 *************************************************************************************/
int CMDisCharacter(CMDoperand_ptr Optr);

/*************************************************************************************
 * Function Name:   CMDisString
 * Description:     Validates if the operand is a STRING_CONST or INDIRECT_STRING
 * Parameter:       Optr - operand pointer
 * Returns:         SUCCESS/FAILURE
 *************************************************************************************/
int CMDisString(CMDoperand_ptr Optr);

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
int CMDgetAxisLoc(ULONG ulAxis);

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
int CMDShowStatDescription(int iPrintType, int iStatus, int iStat2Flag);

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
int CMDShowStat2Description(int iPrintType, int iStatus, int iStatus2, int iStat2Flag);

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
double CMDgetDecryptedPassword(int iFeatureNumArg, long lTimeDateArg);

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
int CMDMakeDecryptedPassword(double dTime1Arg, double dTime2Arg, double dTime3Arg);

int CMDShowStatTT(int iPortArg, char *strArg);
int CMDShowDescription(int iPortArg, char *strArg);
int CMDSetIndirectReg(CMDoperand_ptr OptrArg, long lValueArg);
int CMDSetOrShowStatus(CMDoperand_ptr OptrArg, int iValueArg);
int CMDgetIndirectValue(CMDoperand_ptr OptrArg, int *lValueArg);
int CMDgetIndirectStrValue(CMDoperand_ptr OptrArg, int *iValueArg);
int CMDgetIntegerValue(CMDoperand_ptr OptrArg, int *iValueArg);
int CMDgetAxisValue(ULONG ulAxisArg, CMDoperand_ptr OptrArg, long *lpPositionArg);
int CMDReadHFLGandTAvalue(ULONG *ulAxis, instr_ptr instr, long *lBuf);
int CMDReadStnProperty(CMDoperand_ptr OptrArg, int iTypeArg);

#endif
