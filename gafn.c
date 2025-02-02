/***************************************************************\
 *
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
 * Program:     Motion Control local header
 * File:        gafn.c
 * Functions:   GAIsEndOfString
 *              GABuildAxesArgumentsCommand
 *              GABuildParameterArgumentsCommand
 *              GABuildDoubleParameterArgumentsCommand
 *              GABuildInterrogationCommand
 *              GAConvertAsciiToDouble
 *              GAConvertAsciiToLong
 *              GAReadGalilRevisionInfo
 *              GAValidateCardNo
 *              GAValidateAxis
 *              GAValidateInterrupt
 *              GAClearReadGalilFIFOBuffer
 *              GAClearWriteGalilFIFOBuffer
 *              GASetCommandString
 *
 * Description: Provide interface for variables and routines
 *      used locally in the Motion Control module.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/
#include <sys/io.h>
#include <stdlib.h>
#include <string.h>
#include "sck.h"
#include "fiog.h"
#include "gafn.h"
#include "gag.h"
#include "ser.h"
#include "sctim.h"
#include "scintr.h"
#include "dmclnx.h"

extern HANDLEDMC ghDMC;

unsigned int uGAAXES[8] = { GAXAXIS0, GAYAXIS0, GAZAXIS0, GAWAXIS0,
				GAEAXIS0, GAFAXIS0, GAGAXIS0, GAHAXIS0 }; 

/****************************************************************\
 *
 * Function:    GAValidateCardNo
 *
 * Abstract:    Validate Galil card no
 *
 *              card no = 0x01 for a single galil card configuration
 *              card no 0x01 and 0x02 for a dual galil card configuration
 *
 * Parameters:  int  iCardNoArg     (in)        Galil Card No
 *                                  GA_CARD_0  0x01
 *                                  GA_CARD_1  0x02
 *
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAValidateCardNo(int iCardNoArg)
{
    // Check the card number argument
    if (( iCardNoArg == GA_CARD_0 ) || ( iCardNoArg == GA_CARD_1 ))
    {
        // Check against of number of the card supported
        if (( GAiNumberOfGalilCards == MIN_GALIL_CARDS ) && ( iCardNoArg == GA_CARD_1 ))
            return FAILURE;
        else if (( GAiNumberOfGalilCards < MIN_GALIL_CARDS ) || ( GAiNumberOfGalilCards > MAX_GALIL_CARDS ))
            return FAILURE;
        return SUCCESS;
    }
    return FAILURE;
}


/****************************************************************\
 *
 * Function:    GAValidateAxis
 *
 * Abstract:    Validate Axis
 *              To check if the Gali card version supports the number
 *              of axis.
 *
 * Parameters:  int          iCardNoArg          (in)       Galil Card No
 *              unsigned     uGalilAXisName   (in)       Galil Domain Axis
 *              unsigned     *uAxisName       (out)      Galil AXis
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAValidateAxis(int iCardNoArg, unsigned uGalilAxisNameArg, unsigned *uAxisName)
{
	*uAxisName = uGalilAxisNameArg & GAALLAXES;
//    if ( (iCardNoArg < MIN_GALIL_CARDS) || (iCardNoArg > MAX_GALIL_CARDS))
//        return FAILURE;
//    if ( uGalilAxisNameArg > GL_NUM_AXIS[iCardNoArg] )
//        return FAILURE;
//    else if ( (*uAxisName = uGalilAxisNameArg & GAALLAXIS0) == 0)
//        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GAValidateInterrupt
 *
 * Abstract:   Validate Galil card no
 *
 *             card no = 0x01 for a single galil card configuration
 *             card no 0x01 and 0x02 for a dual galil card configuration
 *
 * Parameters:  int  *iCardNoArg     (in)        Galil Card No
 *                                  GA_CARD_0  0x01
 *                                  GA_CARD_1  0x02
 *
 * Returns:    SUCCESS or FAILURE
 *
\*****************************************************************/
int GAValidateInterrupt(int *iCardNoArg)
{
    if (( *iCardNoArg == GA_CARD_0 ) || ( *iCardNoArg == GA_CARD_1 ) || ( *iCardNoArg == GA_BOTH_CARDS ))
    {
        if (( GAiNumberOfGalilCards == 1 ) && ( *iCardNoArg == GA_CARD_1 ))
            return FAILURE;
        else if (( GAiNumberOfGalilCards == 1 ) && ( *iCardNoArg == GA_BOTH_CARDS ))
            *iCardNoArg = GA_CARD_0;        // Only 1 galil card detected
        else if (( GAiNumberOfGalilCards < 1 ) || ( GAiNumberOfGalilCards > 2 ))
            return FAILURE;
        return SUCCESS;
    }
    return FAILURE;
}


/****************************************************************\
 *
 * Function:    GAIsEndOfString
 *
 * Abstract:    Interface function to End of Galil command string
 *              Return the End of String Flag (Not Start Queuing)
 *
 * Parameters:  int     (in)     iCardNoArg
 *              int     (out)    Return the End of Starting flag
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAIsEndOfString(int iCardNoArg, int *iEndOfString)
{
//    if (GAValidateCardNo(iCardNoArg) == SUCCESS)
    *iEndOfString = !stGAQueueCmdString[iCardNoArg].m_StartQueuing;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GABuildAxesArgumentsCommand
 *
 * Abstract:    Build Galil Axes Argument Commnad
 *              This function is to build the Galil command that uses the Axes
 *              arguments. This command is followed by uppercase X, Y, Z, W.
 *              No commas are needed and the order of axes is not important
 *
 * Parameters:  int            (in)   iCardNoArg
 *              unsigned       (in)   uGalilAxisNameArgs  Galil Axis
 *              char           (in)   cpOpCode         Galil Opcode
 *              int            (in)   iEndOfString      if TRUE  Appends '\r'
 *              char           (out)  cpCommandString  Galil Command String
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GABuildAxesArgumentsCommand(int iCardNoArg, unsigned uGalilAxisNameArg, char *cpOpCode, int iEndOfString, char *cpCommandString)
{
    unsigned uAxisName;

    // Don't check the number of axis for SS command as this command is called in TurnOnGalilInt()
    // and at this time the Galil card version has not been read
//    if ( strncmp (cpOpCode, SENSE_SWITCHES_COMMAND, 2) == 0 )
//    {
        // SS command for all axes
//        uAxisName = GAALLAXIS0;
//    }
//    else
//    {
        // Check if this configuration supports this axis name
        if ( GAValidateAxis(iCardNoArg, uGalilAxisNameArg, &uAxisName) == FAILURE )
             return FAILURE;
//    }

    // Initialze the command String
    memset(cpCommandString, 0, sizeof(char)*MAXGASTR);

    // Get the Galil function opcode
    // No check on the type of Galil opcode for axes argument
    // It is the responsibilty of the user to use a correct Galil command
    strcpy( cpCommandString, cpOpCode );

    // Build up the parameter Arguments for Galil Command
    if (uAxisName & GAXAXIS0)
        strcat(cpCommandString, "X");
    if (uAxisName & GAYAXIS0)
        strcat(cpCommandString, "Y");
    if (uAxisName & GAZAXIS0)
        strcat(cpCommandString, "Z");
    if (uAxisName & GAWAXIS0)
        strcat(cpCommandString, "W");
    if (uAxisName & GAEAXIS0)
        strcat(cpCommandString, "E");
    if (uAxisName & GAFAXIS0)
        strcat(cpCommandString, "F");
    if (uAxisName & GAGAXIS0)
        strcat(cpCommandString, "G");
    if (uAxisName & GAHAXIS0)
        strcat(cpCommandString, "H");
    // Are more Galil commands to follow?
    if (iEndOfString == TRUE)
    {
        // No, this is end of the Galil command string, ready to send out to Galil
        strcat(cpCommandString, "\r");
    }
    else
    {
        // Yes, other galil command to append
        strcat(cpCommandString, ";");
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GABuildParameterArgumentsCommand
 *
 * Abstract:    Build Galil Parameter Argument Commnad
 *              This function is to build the Galil command that requires numerical
 *              arguments (long integer).
 *              This command is followed by values. Values may be specified
 *              for any axis separately or any combination of axes.
 *              The argument for each axis is separated by commas.
 *
 * Parameters:  int            (in)   iCardNoArg           Card No
 *              unsigned       (in)   uGalilAxisNameArgs  Galil Axis
 *              long           (in)   lValue[]           Galil Parameters
 *              char           (in)   cpOpCode        Galil Opcode
 *              int            (in)   iEndOfString      if TRUE  Appends '\r'
 *              char           (out)  cpCommandString  Galil Command String
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GABuildParameterArgumentsCommand(int iCardNoArg,
                                    unsigned uGalilAxisNameArg,
                                    long *lValue,
                                    char *cpOpCode,
                                    int iEndOfString,
                                    char *cpCommandString)
{
    unsigned  uAxisName;
    char      cValueInASCII[15];
	int		  i;	// index to axis bit, where to get lValue for the existing axis

    // Check if this configuration supports this axis name
    if ( GAValidateAxis(iCardNoArg, uGalilAxisNameArg, &uAxisName) == FAILURE )
        return FAILURE;

    // Initialze the command String
    memset(cpCommandString, 0, sizeof(char)*MAXGASTR);
	i = 0;

    // Get the Galil function opcode
    // No check on the type of Galil opcode for parameter argument
    // It is the responsibilty of the user to use a correct Galil command
    strcpy( cpCommandString, cpOpCode );

    // Build up the parameter Arguments for Galil Command
    if (uAxisName & GAXAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[0]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAYAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[1]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAZAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[2]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAWAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[3]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAEAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[4]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAFAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[5]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAGAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[6]);
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAHAXIS0)
    {
		sprintf(cValueInASCII, "%ld", lValue[7]);
        strcat(cpCommandString, cValueInASCII);
    }
    // Are more Galil commands to follow?
    if (iEndOfString == TRUE)
    {
        // No, append the end of Galil command token and ready to send out
        strcat(cpCommandString, "\r");
    }
    else
    {
        // Yes, ready to append the next Galil command string
        strcat(cpCommandString, ";");
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GABuildDoubleParameterArgumentsCommand
 *
 * Abstract:    Build Galil Double Parameter Argument Commnad
 *              This function is to build the Galil command that requires numerical
 *              arguments (double).
 *              This command is followed by values. Values may be specified
 *              for any axis separately or any combination of axes.
 *              The argument for each axis is separated by commas.
 *
 * Parameters:  int             (in)    iCardNoArg     Card No
 *              unsigned        (in)   uGalilAxisNameArgs  Galil Axis
 *              float           (in)   dValue[]        Galil Parameters
 *              char            (in)   cpOpCode        Galil Opcode
 *              int             (in)   iEndOfString      if TRUE  Appends '\r'
 *              char            (out)  cpCommandString  Galil Command String
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GABuildDoubleParameterArgumentsCommand(int iCardNoArg,
                                            unsigned uGalilAxisNameArg,
                                            double *dValue,
                                            char *cpOpCode,
                                            int iEndOfString,
                                            char *cpCommandString)
{
    unsigned  uAxisName;
    char      cValueInASCII[20];
	int 	  i;		// index to dValue for existing axis

    // Check if this configuration supports this axis name
    if ( GAValidateAxis(iCardNoArg, uGalilAxisNameArg, &uAxisName) == FAILURE )
        return FAILURE;

    // Initialze the command String
    memset(cpCommandString, 0, sizeof(char)*MAXGASTR);
	i = 0;

    // Get the Galil function opcode
    // No check on the type of Galil opcode for parameter argument
    // It is the responsibilty of the user to use a correct Galil command
    strcpy( cpCommandString, cpOpCode );

    // Build up the parameter Arguments Galil Command
    if (uAxisName & GAXAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[0] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAYAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[1] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAZAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[2] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAWAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[3] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAEAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[4] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAFAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[5] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAGAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[6] );
        strcat(cpCommandString, cValueInASCII);
    }
    strcat(cpCommandString, ",");
    if (uAxisName & GAHAXIS0)
    {
        sprintf(cValueInASCII, "%8.3f", dValue[7] );
        strcat(cpCommandString, cValueInASCII);
    }
    // Are more Galil commands to follow?
    if (iEndOfString == TRUE)
    {
        // No, append the termination token
        strcat(cpCommandString, "\r");
    }
    else
    {
        // Yes, ready for next galil command string
        strcat(cpCommandString, ";");
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GABuildInterrogationCommand
 *
 * Abstract:    Build Galil Interrogation Command
 *              This function is to build the Galil command that accept a question
 *              mark ('?' ) as an argument.
 *              This command is followed by a ?. The syntax format is the same as the parameter
 *              arguments except '?' replaces the values. The Question mark may be specified
 *              for any axis separately or any combination of axes.
 *              The argument for each axis is separated by commas.
 *              This '?' argument causes the controller to return parameter information listed
 *              in the command description.
 *
 * Parameters:  int            (in)   iCardNoArg       Card No
 *              unsigned       (in)   uGalilAxisNameArgs  Galil Axis
 *              char           (in)   cpOpCode        Galil Opcode
 *              int            (in)   iEndOfString      if TRUE  Appends '\r'
 *              char           (out)  cpCommandString  Galil Command String
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GABuildInterrogationCommand(int iCardNoArg, unsigned uGalilAxisNameArg, char *cpOpCode, int iEndOfString, char *cpCommandString)
{
    unsigned  uAxisName;

    // Check if this configuration supports this axis name
    if ( GAValidateAxis(iCardNoArg, uGalilAxisNameArg, &uAxisName) == FAILURE )
        return FAILURE;

    // Initialze the command String
    memset(cpCommandString, 0, sizeof(char)*MAXGASTR);

    // Get the Galil function opcode
    // No check on the type of Galil opcode for interrogation argument
    // It is the responsibilty of the user to use a correct Galil command
    strcpy( cpCommandString, cpOpCode );

    // Build up the Interrogation Arguments for Galil Command
    if (uAxisName & GAXAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAYAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAZAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAWAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAEAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAFAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAGAXIS0)
        strcat(cpCommandString, "?");
    strcat(cpCommandString, ",");
    if (uAxisName & GAHAXIS0)
        strcat(cpCommandString, "?");
    // Are more Galil commands to follow?
    if (iEndOfString == TRUE)
    {
        // No, append the termination token
        strcat(cpCommandString, "\r");
    }
    else
    {
        // Yes, ready for the next galil command string
        strcat(cpCommandString, ";");
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GAConvertAsciiToLong
 *
 * Abstract:    Convert the ASCII format reply string from Galil to a long numerical value
 *              Store the long value to the array for the respective axis.
 *              If the axis is not specified, then set the value in the respective array
 *              to zero.
 *
 * Parameters:  unsigned    uGalilAxesArg      (in)    The Galil axes to parse
 *              char        cpReadStringArg    (in)    Galil card  Reply string
 *              long        lpGalilParmArg     (out)   Galil Parameters
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAConvertAsciiToLong(unsigned uGalilAxesArg, char *cpReadStringArg, long *lpGalilParmArg)
{
    long lTemp;
    int  iRetValue = FAILURE; /* SUCCESS only if uGalilAxesArg is properly set */
	int  i;		// index to lpGalilParmArg for existing axis
	int  j;


	i = 0;
    // Initialize the lTemp pointer to the first entry
    lTemp = atol( strtok(cpReadStringArg, ",") );

	for (i=0; i<8; ++i)
	{
		if (uGalilAxesArg & uGAAXES[i])
		{
			lpGalilParmArg[i] = lTemp;
        	iRetValue = SUCCESS;
			if (i < 7 && uGalilAxesArg & uGAAXES[i+1])
				lTemp = atol( strtok(NULL, ","));
		}
	}

/*
    // Is the first axis exists?
    if (uGalilAxesArg & GAXAXIS0)
    {
        // yes, get the value & update the lTemp pointer to the next entry
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAYAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[0] = 0;

    // Is the second axis exists?
    if (uGalilAxesArg & GAYAXIS0)
    {
        // yes, get the value & update the lTemp pointer to the next entry
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAZAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[1] = 0;

    // Is the third axis exists?
    if (uGalilAxesArg & GAZAXIS0)
    {
        // yes, get the value & update the lTemp pointer to the next entry
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAWAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[2] = 0;

    // Is the fourth axis exists?
    if (uGalilAxesArg & GAWAXIS0)
    {
        // Yes, get the value
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAEAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[3] = 0;

    // Is the fifth axis exists?
    if (uGalilAxesArg & GAEAXIS0)
    {
        // Yes, get the value
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAFAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[4] = 0;

    // Is the sixth axis exists?
    if (uGalilAxesArg & GAFAXIS0)
    {
        // Yes, get the value
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAGAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[5] = 0;

    // Is the seventh axis exists?
    if (uGalilAxesArg & GAGAXIS0)
    {
        // Yes, get the value
        lpGalilParmArg[i++] = lTemp;
		if (uGalilAxesArg & GAHAXIS0)
            lTemp = atol( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[6] = 0;

    // Is the eighth axis exists?
    if (uGalilAxesArg & GAHAXIS0)
    {
        // Yes, get the value
        lpGalilParmArg[i++] = lTemp;
        iRetValue = SUCCESS;
    }
//    else lpGalilParmArg[7] = 0;
*/

    //if(iRetValue == FAILURE)
    //{
    //    COPutsTxBuff( SERGetCmdPort(), "GAConvertAsciiToLong ERROR!\n" );
    //}

    return iRetValue;
}


/****************************************************************\
 *
 * Function:    GAConvertAsciiToDouble
 *
 * Abstract:    Convert the ASCII reply string from Galil to Double numerical values.
 *              Store the double value to the array for the respective axis.
 *              If the axis is not specified, then set the value in the respective array
 *              to zero.
 *
 * Parameters:  unsigned    uGalilAxesArg      (in)    The Galil axes to parse
 *              char        cpReadStringArg    (in)    Galil card  Reply string
 *              double       dpGalilParmArg    (out)   Galil Parameters
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAConvertAsciiToDouble(unsigned uGalilAxesArg, char *cpReadStringArg, double *dpGalilParmArg)
{
    double dTemp;
    int iRetValue = FAILURE; /* SUCCESS only if uGalilAxesArg is set properly */
	int i;			// index to dpGalilParmArg for existing axes

	i = 0;
    dTemp = atof( strtok(cpReadStringArg, ",") );

	for (i=0; i<8; ++i)
	{
		if (uGalilAxesArg & uGAAXES[i])
		{
			dpGalilParmArg[i] = dTemp;
        	iRetValue = SUCCESS;
			if (i < 7 && uGalilAxesArg & uGAAXES[i+1])
				dTemp = atof( strtok(NULL, ","));
		}
	}

/*
    if (uGalilAxesArg & GAXAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAYAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[0] = 0;
    if (uGalilAxesArg & GAYAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAZAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[1] = 0;
    if (uGalilAxesArg & GAZAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAWAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[2] = 0;
    if (uGalilAxesArg & GAWAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAEAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[3] = 0;
    if (uGalilAxesArg & GAEAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAFAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[4] = 0;
    if (uGalilAxesArg & GAFAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAGAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[5] = 0;
    if (uGalilAxesArg & GAGAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
		if (uGalilAxesArg & GAHAXIS0)
	        dTemp = atof( strtok(NULL, ",") );
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[6] = 0;
    if (uGalilAxesArg & GAHAXIS0)
    {
        dpGalilParmArg[i++] = dTemp;
        iRetValue = SUCCESS;
    }
//    else dpGalilParmArg[7] = 0;
*/

    return iRetValue;
}


/****************************************************************\
 *
 * Function:    GAReadGalilRevisionInfo
 *
 * Abstract:    Read Galil Rev string from Galil Card
 *
 * Parameters:  int  iCardNoArg     (in)        Galil Card No
 *              char szRevision   (out)      Galil Card Revision
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAReadGalilRevisionInfo(int iCardNoArg, char *szRevision)
{
    int iResult;

    iResult = SUCCESS;
//    if ( GAValidateCardNo(iCardNoArg) == FAILURE)
//        return FAILURE;
    iResult = GASendReceiveGalil(iCardNoArg, (char *)READ_VERSION_COMMAND, (char *)szRevision);
    return iResult;
}


/****************************************************************\
 *
 * Function:    GAClearReadGalilFIFOBuffer
 *
 * Abstract:   Clears the read FIFO buffer on the galil card
 *
 * Parameters:
 *
 *       iCardNoArg            (in) Card No
 *
 * Returns:   SUCCESS or FAILURE
 *
\*****************************************************************/
int GAClearReadGalilFIFOBuffer(int iCardNoArg)
{
	int iRC;

//    if ( GAValidateCardNo(iCardNoArg) == FAILURE)
//        return FAILURE;

	iRC = DMCClear(ghDMC);
    return iRC;
}


/****************************************************************\
 *
 * Function:    GAClearWriteGalilFIFOBuffer
 *
 * Abstract:   Clears the write FIFO buffer on the galil card
 *
 *    NOTE:    The program should not write to the galil for 10 ms
 *             or so after sending this command to allow the galil
 *             to execute it.
 *
 * Parameters:
 *
 *       iCardNoArg            (in) Card No
 *
 * Returns:   SUCCESS or FAILURE
 *
\*****************************************************************/
int GAClearWriteGalilFIFOBuffer(int iCardNoArg)
{
	int iRC;

//    if ( GAValidateCardNo(iCardNoArg) == FAILURE)
//        return FAILURE;

	iRC = DMCClear(ghDMC);

    // Wait for 20 Msec till Galil execute the clear
    if (TISetCounter(GA_TIMER, GA_CLEAR_WRITE_FIFO_WAIT) == FAILURE)
        return FAILURE;
    while( !TICountExpired( GA_TIMER ) );
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    GASetCommandString
 *
 * Abstract:    Copy command string
 *
 * Parameters:  iParamDescArg       (in) Define from Datafile module
 *              cpCommandStringArg  (out) String pointer
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GASetCommandString(int iParamDescArg, char *cpCommandStringArg)
{
    switch (iParamDescArg)
    {
        case OPERATIONAL_SPEED :
            strcpy(cpCommandStringArg, SPEED_COMMAND);
            break;
        case OPERATIONAL_ACCEL :
            strcpy(cpCommandStringArg, ACCELERATION_COMMAND);
            break;
        case ERROR_LIMIT :
            strcpy(cpCommandStringArg, ERROR_LIMIT_COMMAND);
            break;
        case FEED_FORWARD_ACCEL :
            strcpy(cpCommandStringArg, FEED_FORW_ACCEL_COMMAND);
            break;
        case IT_SCURVE :
            strcpy(cpCommandStringArg, INDEP_TIME_COMMAND);
            break;
        case JERK :
            strcpy(cpCommandStringArg, INDEP_TIME_COMMAND);
            break;
//        case JERK :
//            strcpy(cpCommandStringArg, ACC_DECEL_RAMP_COMMAND);
//            break;
        case OPERATIONAL_DECEL :
            strcpy(cpCommandStringArg, DECELERATION_COMMAND);
            break;
        case PROPORTIONAL_GAIN :
            strcpy(cpCommandStringArg, KPROPORTIONAL_COMMAND);
            break;
        case INTEGRAL_GAIN :
            strcpy(cpCommandStringArg, KINTEGRATOR_COMMAND);
            break;
        case TORQUE_LIMIT :
            strcpy(cpCommandStringArg, TORQUE_LIMIT_COMMAND);
            break;
        case DERIVATIVE_GAIN :
            strcpy(cpCommandStringArg, KDERIVATIVE_COMMAND);
            break;
        case JOG_SPEED :
            strcpy(cpCommandStringArg, JOG_MODE_COMMAND);
            break;
        default:
            return FAILURE;
    }
    return SUCCESS;
}
