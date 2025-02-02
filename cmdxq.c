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
 * Program:     New commands for scmain controller definition.
 *
 * File:        cmdxq.c
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
#include "cmdga.h"
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
#include "roga.h"
#include "roloc.h"
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
#include "alstep.h"
#include "dmclnx.h"

extern HANDLEDMC ghDMC;


/***************************************************************************
 * Function Name:   ex_XQFN
 * Description:     Execute Galil macros given function number
 *
 * NOTE:            This function send XQ#DAFN command to galil.
 *		    The function to execute is passed in "FNUM=nn" command.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XQFN(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lFNum;
    char caCommand[MAXGASTR];
    char caResponse[MAXGASTR];
    int rc;

    Optr = instr->opr_ptr; /* get first operand, which is the function number to execute */
    if(!Optr)
        return FAILURE;

    switch( Optr->type )     /* based on operand set values of Bit number and Port ID */
    {
        case LONG_CONST :               /* function number */
        case INDIRECT_REG :             /* function number */
	    if(CMDgetValue(Optr, &lFNum)==FAILURE)
		return FAILURE;
            sprintf(caCommand, "FNUM=%d",lFNum);
    	    rc = DMCCommand(ghDMC, caCommand, caResponse, MAXGASTR);
            sprintf(caCommand, "XQ#DAFN");
    	    rc = DMCCommand(ghDMC, caCommand, caResponse, MAXGASTR);
	    return SUCCESS;
            break;
        default:
            return FAILURE;
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_XQGT
 * Description:     Execute Galil macros to retrieve data
 *
 * NOTE:            This function send XQ#GETV command to galil.
 *		    First set VTYPE=nn (Variable Type: which data array)
 *		    Second set VINDX=nn (Index to the array)
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_XQGT(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    long lDataType, lIndex, lValue;
    char caCommand[MAXGASTR];
    char caResponse[MAXGASTR];
    int rc;

    Optr = instr->opr_ptr; /* get first operand, which is the function number to execute */
    if(!Optr)
        return FAILURE;

    switch( Optr->type )     /* based on operand set values of Bit number and Port ID */
    {
        case LONG_CONST :               /* function number */
        case INDIRECT_REG :             /* function number */
	    if(CMDgetValue(Optr, &lDataType)==FAILURE)
		return FAILURE;
            break;
        default:
            return FAILURE;
    }

    if( (Optr=Optr->next)) // the second is index to the array
    {
    	switch( Optr->type )     /* based on operand set values of Bit number and Port ID */
    	{
            case LONG_CONST :               /* function number */
            case INDIRECT_REG :             /* function number */
	    	if(CMDgetValue(Optr, &lIndex)==FAILURE)
		    return FAILURE;
                break;
            default:
                return FAILURE;
   	}
    }

    // now we have the data type and index, issue the command to galil
    sprintf(caCommand, "VTYPE=%d",lDataType);
    rc = DMCCommand(ghDMC, caCommand, caResponse, MAXGASTR);
    sprintf(caCommand, "VINDX=%d",lIndex);
    rc = DMCCommand(ghDMC, caCommand, caResponse, MAXGASTR);
    sprintf(caCommand, "XQ#GETV");
    rc = DMCCommand(ghDMC, caCommand, caResponse, MAXGASTR);

    lValue = atoi(caResponse);

    if( (Optr=Optr->next) )   // if there's 3rd operand, it should be a register.
    {
	if(Optr->type == INDIRECT_REG)
            return CMDSetIndirectReg(Optr, lValue);
	else
	    return FAILURE;
    }
    else     // no 3rd operand, send the response to the com port
    {
        //sprintf( cBuf, "%8d", lValue );
        return CMDShowDescription(SERGetCmdPort(), caResponse);
    }

    return FAILURE;
}
