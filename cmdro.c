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
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdro.c
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
#include "cmdro.h"
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


/***************************************************************************
 * Function Name:   ex_ER
 * Description:     Set Servo Error Limit to galil.  This is sGalil Motion Controller PID parameter.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new Error Limit
 *                  value and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed ER command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: ER [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ER(instr_ptr instr)
{
    long lErrorLimits[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get the value(s) that need to be set from the operand list. Then set the value(s)
     * to galil. */
    if( CMDgetGalilParms(instr,&ulAxis,lErrorLimits) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lErrorLimits, ERROR_LIMIT );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_KP
 * Description:     Set GALIL Proportional Constant to galil.
 *                  We replace the GN with KP for codemerge98 version only.
 *                  If using GN value in the currently parameter file,
 *                  you need to multiply the GN value by 1000.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new KP
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed KP or GN command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: KP [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_KP(instr_ptr instr)
{
    long lKp[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get values that need to be set from operand list. */
    if( CMDgetGalilParms(instr,&ulAxis,lKp) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lKp, PROPORTIONAL_GAIN );  /* set the values to the galil */
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RM
 * Description:     Set acceleratoin Ramp slope to galil. Acceleration ramp slope is the constant that is used when
 *                  S-curve motion profile is enabled.  This setup determines how gradual the acceleration is.
 *                  The recommended range is 1600 to 10000. The smaller the number the heavier the S-curve is
 *                  which means axis moves more smoothly but also more gradually(slowly).
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new RM
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed RM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RM [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RM(instr_ptr instr)
{
    long lRamp[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    return SUCCESS;

    /* get values from operands and set the value to galil.*/
    if( CMDgetGalilParms(instr,&ulAxis,lRamp) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lRamp, JERK );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_FA
 * Description:     Set Galil Feedforward acceleration (FA)  to galil.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new FA
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed FA command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: FA [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_FA(instr_ptr instr)
{
    long lFfAcc[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get the values from operands and set the values to galil */
    if( CMDgetGalilParms(instr,&ulAxis,lFfAcc) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lFfAcc, FEED_FORWARD_ACCEL );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_KI
 * Description:     Set Servo integrator to galil. Range is from 0 to 20487875.
 *                  For codemerge only, if using the current parameter, you need to multiply the
 *                  KI value by 1000.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new KI
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed KI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: KI [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_KI(instr_ptr instr)
{
    long lIntegralGain[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get the values from operands and set the values to galil */
    if( CMDgetGalilParms(instr,&ulAxis,lIntegralGain) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lIntegralGain, INTEGRAL_GAIN );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_TL
 * Description:     Set Torque Limit to galil. Range of torque is 1 to 9998. To
 *                  ensure best servo performance always set this limit to maximum (9998).
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new TL
 *                  value and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed TL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TL [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TL(instr_ptr instr)
{
    long lTorqueLimit[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get the values from operands and set the values to galil */
    if( CMDgetGalilParms(instr,&ulAxis,lTorqueLimit) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lTorqueLimit, TORQUE_LIMIT );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_KD
 * Description:     Set Galil Derivative Constant. It was ZR (only codemerge). Range is 0 to 4095875
 *                  For codemerge only, if using the current parameter, you need to multiply the
 *                  KD/ZR value by 1000.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new KD
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed KD or ZR command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: KD [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): values to be set into the galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_KD(instr_ptr instr)
{
    long lKd[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get the values from operands and set the value to galil */
    if( CMDgetGalilParms(instr,&ulAxis,lKd) == SUCCESS )
        return ROSetParameter( FALSE, ulAxis, lKd, DERIVATIVE_GAIN );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RAD
 * Description:     Read operational Acceleration of specified axis from galil.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s acceleration values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s acceleration values from galil. Ohterwise, read specific
 *                  axis acceleration. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the acceleration from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RAD command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RAD [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RAD(instr_ptr instr)
{
    long lRobotAcceleration[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get the acceleration from galil. */
    if (ROGetParameter(FALSE, ulAxis, lRobotAcceleration, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;

    if( Optr )                  /* if there is first operand, then check for next operand */
        Optr = Optr->next;
   /* if there is next operand(s), set the value(s) into the register(s), otherwise send
    * the value(s) to the comm port. */
    return CMDreadBackParms( Optr, lRobotAcceleration, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RDL
 * Description:     Read operational Deceleration of specified axis from galil.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s deceleration values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s deceleration values from galil. Ohterwise, read specific
 *                  axis deceleration. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the deceleration from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RDL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RDL [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RDL(instr_ptr instr)
{
    long lRobotDeceleration[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get the deceleration from galil. */
    if (ROGetParameter(FALSE, ulAxis, lRobotDeceleration, OPERATIONAL_DECEL) == FAILURE)
        return FAILURE;

    if( Optr )                  /* if there is an operand, move to the next */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lRobotDeceleration, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RSP
 * Description:     Read operational Speed of specified axis from galil.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s speed values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s speed values from galil. Ohterwise, read specific
 *                  axis speed. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the speed from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RSP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSP [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSP(instr_ptr instr)
{
    long lRobotSpeed[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get operational speed from galil */
    if( ROGetParameter(FALSE,ulAxis,lRobotSpeed,OPERATIONAL_SPEED) == FAILURE )
        return FAILURE;

    if( Optr )       /* if there is an operand, move to the next */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lRobotSpeed, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RCP
 * Description:     Read current Position of specified axis.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s current position values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s current position values from galil. Ohterwise, read specific
 *                  axis current position. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the current position from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RCP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCP [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCP(instr_ptr instr)
{
    long lCurrentPosition[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* read current position */
    if( ROReadCurrentPosition(ulAxis,lCurrentPosition) == FAILURE )
        return FAILURE;

    if( Optr )           /* if there is first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lCurrentPosition, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RER
 * Description:     Read Robot Error Limit.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s error limit values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s error limit values from galil. Ohterwise, read specific
 *                  axis error limit. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the error limit from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RER command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RER [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RER(instr_ptr instr)
{
    long lErrorLimit[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get error limit from galil */
    if( ROGetParameter(FALSE,ulAxis,lErrorLimit,ERROR_LIMIT) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lErrorLimit, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RFA
 * Description:     Read Robot Feed Forward Acceleration
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s FA values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s FA values from galil. Ohterwise, read specific
 *                  axis FA. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the FA from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RFA command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RFA [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFA(instr_ptr instr)
{
    long lFeedForwardAcceleration[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get feed forward acceleration. */
    if( ROGetParameter(FALSE,ulAxis,lFeedForwardAcceleration,FEED_FORWARD_ACCEL) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lFeedForwardAcceleration, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RKP
 * Description:     Read Robot Proportional Constant   (KP)
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s KP values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s KP values from galil. Ohterwise, read specific
 *                  axis KP. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the KP from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RKP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RKP [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RKP(instr_ptr instr)
{
    long lKp[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get proportional constant. */
    if( ROGetParameter(FALSE,ulAxis,lKp,PROPORTIONAL_GAIN) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lKp, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RRM
 * Description:     Read acceleration Ramp slope.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s RM values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s RM values from galil. Ohterwise, read specific
 *                  axis RM. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the RM from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RRM command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RRM [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RRM(instr_ptr instr)
{
    long lRamp[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get ramp */
//    if( ROGetParameter(FALSE,ulAxis,lRamp,JERK) == FAILURE )
//        return FAILURE;

    if( Optr )                  /* if there is first operand, move next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lRamp, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RKI
 * Description:     Read Robot Integral Gain.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s KI values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s KI values from galil. Ohterwise, read specific
 *                  axis KI. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the KI from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RKI command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RKI [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RKI(instr_ptr instr)
{
    long lIntegralGain[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get integral gain */
    if( ROGetParameter(FALSE,ulAxis,lIntegralGain,INTEGRAL_GAIN) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a first operand, move next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lIntegralGain, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RTE
 * Description:     Read Robot Current Error
 * NOTE:            This function will get executed when the user typed RTE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RTE(instr_ptr instr)
{
    long lCurrentError[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    if( ROReadCurrentError(ulAxis,lCurrentError) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a next operand move to it */
        Optr = Optr->next;
    return CMDreadBackParms( Optr, lCurrentError, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RTT
 * Description:     Read Robot Current Torque
 * NOTE:            This function will get executed when the user typed RTT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RTT(instr_ptr instr)
{
    long lCurrentTorque[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    if( ROReadCurrentTorque(ulAxis,lCurrentTorque) == FAILURE )
        return FAILURE;

    if( Optr )
        Optr = Optr->next;
    return CMDreadBackParms( Optr, lCurrentTorque, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RTL
 * Description:     Read Robot Torque Limit.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s TL values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s TL values from galil. Ohterwise, read specific
 *                  axis TL. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the TL from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RTL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RTL [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RTL(instr_ptr instr)
{
    long lTorqueLimit[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get torque limit */
    if( ROGetParameter(FALSE,ulAxis,lTorqueLimit,TORQUE_LIMIT) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a first operand, move next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lTorqueLimit, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RKD
 * Description:     Read Robot Derivative Constant parameter. It was ZR
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s KD values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s KD values from galil. Ohterwise, read specific
 *                  axis KD. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the KD from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RKD command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RKD [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RKD(instr_ptr instr)
{
    long lKd[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get derivative constant */
    if( ROGetParameter(FALSE,ulAxis,lKd,DERIVATIVE_GAIN) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is frist operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lKd, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RNSL
 * Description:     Read Software Negative Limit.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s negative limit values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s negative limit values from galil. Otherwise, read specific
 *                  axis negative limit. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the negative limit from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RNSL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RNSL [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RNSL(instr_ptr instr)
{
    long lSoftwareNegativeLimit[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get software negative limit switch */
    if( ROGetParameter(TRUE,ulAxis,lSoftwareNegativeLimit,SOFT_NEG_LIMIT) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lSoftwareNegativeLimit, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RIPW
 * Description:     Read position in window.
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s in-position values from galil. Otherwise, read specific
 *                  axis in-position. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the in-position from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RIPW command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RIPW [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RIPW(instr_ptr instr )
{
    unsigned long ulAxis;
    long lPositions[8]={0,0,0,0,0,0,0,0};
    CMDoperand_ptr Optr=instr->opr_ptr;

    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )  /* get axis */
        return FAILURE;
    /* get in-position-window */
    if( ROReadInPositionWindow(ulAxis,lPositions) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms(Optr, lPositions, ulAxis);
}
/***************************************************************************
 * Function Name:   ex_RPSL
 * Description:     Read robot software positive limit
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s positive limit values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s positive limit values from galil. Ohterwise, read specific
 *                  axis posi limit. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the positive limit from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RPSL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RPSL [AXIS], [VALUE1,VALUE2, VALUE3]
 *                      - AXIS:     Specifying the specific axis or all axes. If A or nothing specified for axis then
 *                                  default is for all axes.
 *                      - VALUE(S): stores return values that read from galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPSL(instr_ptr instr)
{
    long lSoftwarePositiveLimit[8];
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )  /* get axis */
        return FAILURE;
    /* get the software positive limit */
    if( ROGetParameter(TRUE,ulAxis,lSoftwarePositiveLimit,SOFT_POS_LIMIT) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a first operand, move next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lSoftwarePositiveLimit, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_SAD
 * Description:     Set operational acceleration.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new acceleration
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram and local RAM, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed SAD command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SAD <AXIS>, <VALUE1>, [VALUE2, VALUE3]
 *                      - AXIS:         axis name
 *                      - VALUE(S):     value(s) to be set to galil.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAD(instr_ptr instr)
{
    long lOperatingAcceleration[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    //CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis, if axis is A or a, then get values of all 3 axes (T,R,Z) or (t,r,z) from
     * parameters; otherwise, get value of specified axis to be set.*/
    if( CMDgetGalilParms(instr,&ulAxis,lOperatingAcceleration) == SUCCESS )
        return ROSetParameter(FALSE,ulAxis,lOperatingAcceleration,OPERATIONAL_ACCEL);
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SDL
 * Description:     Set operational deceleration
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new deceleration
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 *                  In order to save pernamently into NVSram and local RAM, you need to
 *                  to issue SAV command after set the value.
 * NOTE:            This function will get executed when the user typed SDL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SDL <AXIS>, <VALUE1>, [VALUE2, VALUE3]
 *                      - AXIS:         axis name
 *                      - VALUE(S):     value(s) to be set to galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SDL(instr_ptr instr)
{
    long lOperatingDeceleration[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    //CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis, if axis is A or a, then get values of all 3 axes (T,R,Z) or (t,r,z) from
     * parameters; otherwise, get value of specified axis to be set.*/
    if( CMDgetGalilParms(instr,&ulAxis,lOperatingDeceleration) == SUCCESS )
        return ROSetParameter(FALSE,ulAxis,lOperatingDeceleration,OPERATIONAL_DECEL);
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SNSL
 * Description:     Set Software Negative Limit
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new negative limit
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 * NOTE:            This function will get executed when the user typed SNSL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SNSL <AXIS>, <VALUE1>, [VALUE2, VALUE3]
 *                      - AXIS:         axis name
 *                      - VALUE(S):     value(s) to be set to galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SNSL(instr_ptr instr)
{
    long lSoftwareNegativeLimit[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get axis, if axis is A or a, then get values of all 3 axes (T,R,Z) or (t,r,z) from
     * parameters; otherwise, get value of specified axis to be set.*/
    if( CMDgetGalilParms(instr,&ulAxis,lSoftwareNegativeLimit) == SUCCESS )
        return ROSetParameter( TRUE, ulAxis, lSoftwareNegativeLimit, SOFT_NEG_LIMIT );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SIPW
 * Description:     Set Position in Window for TOT.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new in-position
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 * NOTE:            This function will get executed when the user typed SIPW command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SIPW <AXIS>, <VALUE1>, [VALUE2, VALUE3]
 *                      - AXIS:         axis name
 *                      - VALUE(S):     value(s) to be set to galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SIPW(instr_ptr instr )
{
    unsigned long ulAxis;
    long lPosition[8]={0,0,0,0,0,0,0,0};

    /* get the axis and value(s) to be set from operand list. */
    if( CMDgetGalilParms(instr,&ulAxis,lPosition) == SUCCESS )
        return ROSetInPositionWindow(ulAxis,lPosition); /* set the in-position-window */ 
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_SPSL
 * Description:     Set positive limit switch
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new positive limit
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 * NOTE:            This function will get executed when the user typed SPSL command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SPSL <AXIS>, <VALUE1>, [VALUE2, VALUE3]
 *                      - AXIS:         axis name
 *                      - VALUE(S):     value(s) to be set to galil.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SPSL(instr_ptr instr)
{
    long lSoftwarePositiveLimit[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    /* get axis, if axis is A or a, then get values of all 3 axes (T,R,Z) or (t,r,z) from
     * parameters; otherwise, get value of specified axis to be set.*/
    if( CMDgetGalilParms(instr,&ulAxis,lSoftwarePositiveLimit) == SUCCESS )
        return ROSetParameter( TRUE, ulAxis, lSoftwarePositiveLimit, SOFT_POS_LIMIT );
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RLP
 * Description:     Read the Robots latched position
 * NOTE:            This function will get executed when the user typed RLP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RLP
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RLP(instr_ptr instr )
{
    ULONG ulAxis;
    long lLatchedPosition[8]={0,0,0,0,0,0,0,0};
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;

    if( ulAxis&RO_AXIS_ALL || ulAxis&RO_AXIS_all )     /* if for a specific axis */
        return FAILURE;

    /* read latched position */
    if( ROReadLatchedPosition(ulAxis,lLatchedPosition) == FAILURE )
        return FAILURE;

    if( Optr )           /* if there is first operand, move to next operand */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lLatchedPosition, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_SSP
 * Description:     Set operational Speed into the galil and it will be not got set into
 *                  RAM or NVSRam until providing the SAV command to permanently change the
 *                  speed value. If haven't provided the SAV command and provided RSA command,
 *                  the firmware will restore the speed value from RAM.
 *                  If 'A' is specified as an axis, then set the all robot axes (T,R,Z) with new speed
 *                  values and they all are needed to specify.  If 'a' is specified as an axis, then that's for
 *                  prealigner setting.  Otherwise, setting specific value for specified axis.
 * NOTE:            This function will get executed when the user typed SSP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SSP <AXIS>, <VALUE1>, <VALUE2>, <VALUE3>
 *                      - AXIS:     axis name
 *                      - VALUE(S): the speed value to be set for specified axis
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSP(instr_ptr instr)
{
    long lOperatingSpeed[8]={0,0,0,0,0,0,0,0};
    long lH[8]={0,0,0,0,0,0,0,0};
    //long lSpeedLimit[4]={0,0,0,0};
    ULONG ulAxis;
    int iParamDesc=ROBOTFILE;

    /* get axis and speed value(s) to be set */
    if( CMDgetGalilParms(instr,&ulAxis,lOperatingSpeed) == SUCCESS )
    {
        if( ulAxis&(RO_AXIS_t|RO_AXIS_r|RO_AXIS_z) )  /* checking for prealigner axis */
            iParamDesc = PREALIGNFILE;
        else if( ulAxis&(RO_AXIS_W|RO_AXIS_w) )
            iParamDesc = AUXFILE;

        /* this part is for speedlimit and with ipec version that deal with customer port */
        if( FIOGetParamVals(iParamDesc,H_VALUE,lH) == SUCCESS )
        {
// This code is no longer, as far as anyone knows. But it should be left here just in case
// it is needed in the future.
//            if( lH[2] > 0 )
//            {
//                if( (long)CMDreadPort(-1,'H') & lH[2] )
//                {
//                    if( FIOGetParamVals(iParamDesc,SPEED_LIMIT,lSpeedLimit) == SUCCESS )
//                    {
//                        if( lOperatingSpeed[0] > lSpeedLimit[0] )  /* valid for any axis */
//                            return FAILURE;
//                        else if( ( ulAxis & (RO_AXIS_ALL | RO_AXIS_all) ) && /* only valid for A and a */
//                                ( lOperatingSpeed[1] > lSpeedLimit[1] ||
//                                lOperatingSpeed[2] > lSpeedLimit[2] ) )
//                            return FAILURE;
//                    }
//                }
//            }
            return ROSetParameter( FALSE, ulAxis, lOperatingSpeed, OPERATIONAL_SPEED );
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RSA
 * Description:     Restore speed and acceleration from RAM to galil.  This command reads
 *                  value of speed, deceleration and acceleration setup that was current when the last
 *                  SAV command was issued.  If axis is not specified, it will restore
 *                  all axes of robot (T,R,Z) and the same as specifying 'A'.  If 'a' is specified,
 *                  then restore all axes of prealigner.
 * NOTE:            This function will get executed when the user typed RSA command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RSA <AXIS>
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSA(instr_ptr instr)
{
    ULONG ulAxis;
    long lRobotAcceleration[8]={0,0,0,0,0,0,0,0};
    long lOperatingSpeed[8]={0,0,0,0,0,0,0,0};
    long lH[8]={0,0,0,0,0,0,0,0};
    //long lSpeedLimit[4] = {0,0,0,0};
    long lRobotDeceleration[8]={0,0,0,0,0,0,0,0};
    int iParamDesc=ROBOTFILE; // iReturn; //iSeccard=FALSE, 

    ulAxis = CMDgetAxis(instr);
    /* set the parameter description based on the axis  */
    if( ulAxis&(RO_AXIS_t|RO_AXIS_r|RO_AXIS_z) )
        iParamDesc = PREALIGNFILE;
    else if( ulAxis&(RO_AXIS_W|RO_AXIS_w) )
        iParamDesc = AUXFILE;

    /* get initial values */

    if( FIOGetParamVals(iParamDesc,H_VALUE,lH) == FAILURE )
        return FAILURE;

    /* get the speed from NVSRam */
    if( ROGetParameter(TRUE,ulAxis,lOperatingSpeed,OPERATIONAL_SPEED) == FAILURE )
        return FAILURE;

// This code is no longer, as far as anyone knows. But it should be left here just in case
// it is needed in the future.
    /* get the speed from speed limit from NVSram */
//    if( FIOGetParamVals(iParamDesc,SPEED_LIMIT,lSpeedLimit) == FAILURE )
//        return FAILURE;

    /* operating speed  */
//    if( (lH[2]>0) && ((long)CMDreadPort(-1,'H') & lH[2]) && (lSpeedLimit[0]!=0) )
//    {
        /* this is case if there is a customized door to check for certain speed. */
//        if( ROSetParameter(FALSE,ulAxis,lSpeedLimit,OPERATIONAL_SPEED) == FAILURE )
//            return FAILURE;
//    }
//    else
//    {
        /* for regular of setting the speed to galil */
        if( ROSetParameter(FALSE,ulAxis,lOperatingSpeed,OPERATIONAL_SPEED) == FAILURE )
            return FAILURE;
//    }

    /* get operating acceleration from NVSram*/
    if (ROGetParameter(TRUE, ulAxis, lRobotAcceleration, OPERATIONAL_ACCEL) == FAILURE)
        return FAILURE;

    /* set the operating acceleration to galil */
    if( ROSetParameter(FALSE,ulAxis,lRobotAcceleration,OPERATIONAL_ACCEL) == FAILURE )
        return FAILURE;

    /* get operating deceleration from NVSram*/
    if (ROGetParameter(TRUE, ulAxis, lRobotDeceleration, OPERATIONAL_DECEL) == FAILURE)
        return FAILURE;

    /* set operating deceleration to galil */
    if( ROSetParameter(FALSE,ulAxis,lRobotDeceleration,OPERATIONAL_DECEL) == FAILURE )
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_RESP
 * Description:     Send Response String. This command is only execute within a macro.  This
 *                  command checks certain condition and sends appropriate response string before
 *                  want to move an axis. It can check for any axis in the system. In this command,
 *                  the response string format is sent depend on the mode - regular mode
 *                  or Brooks emulator mode.
 * Implementation:  For Books emulator mode:
 *                      If there is no operand, then don't send anything to comm port. If there is
 *                      an operand, then get the axis from first operand and validate the axis. If there is
 *                      second operand, then get the flag value and get the status of specified axis depend
 *                      on specified flag.  On any case that error occured, it will send "_ERR 0308"
 *                      to indicate of error. Ohterwise, it will not send anything.
 *                  For Regular mode:
 *                      If there is no operand, then send out "\r\n>" to comm port. If there is an operand, then
 *                      get the axis from first operand and validate the axis.  If there is a second operand, then get
 *                      flag value and get the status of specified axis depend on the specified flag.  On the case of
 *                      error, it will send back "\r\n?". Otherwise, send "\r\n>".
 * NOTE:            This function will get executed when the user typed RESP command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RESP <AXIS>, <FLAG>, [RETURN VALUE]
 *                      - AXIS:             axis to check for condition. The axis can be T,R,Z,A,t,r,z,a.
 *                      - FLAG:             flag for checking which condition of axis. And the conditions to check
 *                                          are in bits:
 *                                          - Bit#0:    Error if moving.
 *                                          - Bit#1:    Error if position error.
 *                                          - Bit#2:    Error if Not homed.
 *                                          - Bit#3:    Error if servo off.
 *                      - RETURN VALUE:     This is optional parameter to store the return value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RESP(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    char sBuf[15];
    int iError, iReturn;
    long lFlag;
    ULONG ulAxis;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    if( !(Optr=instr->opr_ptr) )        /* no operand */
    {
        if (CMDemulFlags&DFEMULATORB)    /* brooks emulator mode, send nothing */
        {
            return SUCCESS;
        }
        if (TIGetTimerVals(LOCKED) == 123)
            sprintf(sBuf, "\r\n%c", PCGetWDPrompt());
        else
            sprintf(sBuf, "\r\n>");
        return CMDShowDescription(iCmdPort, sBuf);
    }
    /* get axis from 1st operand,
     * if error while getting the axis or axis is invalid */
    else if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
    {
        if( !((MRGetMacroIntegerVars(MR_CMER_ENABLED) == TRUE) && (MRGetMacroPC(MR_CMER_INTERRUPT) != NULL)) )
        {
            if (CMDemulFlags&DFEMULATORB)    /* for books emulator mode, send back the error message to comm port. */
            {
                if (SERPutsTxBuff( iCmdPort, "_ERR 0308\r" ) == FAILURE)
                    return FAILURE;
            }
            else                            /* for regular mode, send back the "\r\n?" to comm port. */
            {
                if (SERPutsTxBuff(iCmdPort,"\r\n?") == FAILURE)
                    return FAILURE;
            }
            if (SERFlushTxBuff( iCmdPort ) == FAILURE)
                return FAILURE;
        }
        return FAILURE;
    }
    else if( !(Optr=Optr->next) )       /* if no second operand */
    {
        // if Axis is specified and no flag, then return failure
        return FAILURE;
        //if (CMDemulFlags&DFEMULATORB)    /* for brooks mode, do nothing */
        //{
        //    return SUCCESS;
        //}
        //if (TIGetTimerVals(LOCKED) == 123)
        //    sprintf(sBuf, "\r\n%c", PCGetWDPrompt());
        //else
        //    sprintf(sBuf, "\r\n>");
		//return CMDShowDescription(iCmdPort, sBuf);
    }
    else if( CMDgetValue(Optr,&lFlag) == SUCCESS )  /* if there is second operand, get flag value from second operand */
    {
        /* get the status of axis depend flag condition and the error status in the case of error */
        iReturn = ROGetResponseString(ulAxis,(int)lFlag,&iError);
        if( (Optr=Optr->next) )     /* there is third operand, then store the status into the register */
        {
            return RGSetRegister(Optr->opr.i,(long)iError);
        }
        if( iReturn == FAILURE )
        {
            if( !((MRGetMacroIntegerVars(MR_CMER_ENABLED) == TRUE) && (MRGetMacroPC(MR_CMER_INTERRUPT) != NULL)) )
            {
                if (CMDemulFlags&DFEMULATORB)  /* brooks mode, send the error status to comm port */
                {
                    sprintf( sBuf, "_ERR %04d\r", iError );
					if(CMDShowDescription(iCmdPort, sBuf)==FAILURE)
						return FAILURE;
                }
                else                    /* regular mode, send ? */
                {
					if(CMDShowDescription(iCmdPort, "\r\n?")==FAILURE)
						return FAILURE;
                }
            }
            return FAILURE;
        }
        else                            /* if there is no error, just return success */
        {
            return SUCCESS;
            //if (TIGetTimerVals(LOCKED) == 123)
            //    sprintf(sBuf, "\r\n%c", PCGetWDPrompt());
            //else
            //    sprintf(sBuf, "\r\n>");
			//return CMDShowDescription(iCmdPort, sBuf);
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_RLS
 * Description:     Read Limit Switches of all components in the system such as robot, prealigner, track
 *                  and dual arm robot. Please look at "Software Manual" for more information about the
 *                  detail of status bit.
 * Implementation:  First, get the limit switch status by calling ROReadLimitSwitches(). Then if there is operand,
 *                  store the status value into the register; otherwise, send the status through the comm port.
 * NOTE:            This function will get executed when the user typed RLS command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RLS [RETURN VALUE]
 *                      - RETURN VALUE: This is optional parameter to hold the return status.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RLS(instr_ptr instr)
{
    unsigned long lSwitches;
    char sBuf[10];

    if( ROReadLimitSwitches(&lSwitches) == SUCCESS )  /* read limit switch status*/
    {
        if( instr->opr_ptr )            /*if operands, store the status in the register. */
        {
            if( instr->opr_ptr->type == INDIRECT_REG )  /* make sure it is the indirect_reg type */
                return RGSetRegister(instr->opr_ptr->opr.i,lSwitches);
        }
        else                            /* no operands, send to comm port. */
        {
            sprintf( sBuf, "%08lX", lSwitches );  /* output switches value */
		return CMDShowDescription(SERGetCmdPort(), sBuf);
        }
    }
    return FAILURE;
}
/***************************************************************************
 * Function Name:   ex_TA
 * Description:     Enable S-Curve Motion Profile. TA enables or disables S-curve motion profile.
 *                  S-curve is disabled when HOME is executed.  In order to have S-curve working, TA
 *                  axis, 1 must be issue following a successfully completion of HOME command. To disable
 *                  S-curve by setting the TA axis, 0.
 * Implemenation:   Get the axis and validate number of parameters and the value(flag) of each parameter to make sure
 *                  they can be either 0 or 1.  Then Set S-curve profile according to flag (enable(1) or disable(0).
 * NOTE:            This function will get executed when the user typed TA command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: TA <AXIS>, [FLAG](FLAG1, FLAG2, FLAG3)
 *                      - AXIS:     axis to enable or disable the S-curve motion profile.
 *                      - FALG:     (0/1) enable/disable the S-curve motion profile.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TA(instr_ptr instr)
{
//    long lBuf[8]={0,0,0,0,0,0,0,0};
//    ULONG ulAxis;
//	int i;

	return 0;

    /* get the values from operands and set the values to galil */
//    if( CMDgetGalilParms(instr,&ulAxis,lBuf) == SUCCESS )
//	{
//		for (i=0; i<8; ++i)
//		{
//			if (lBuf[i] == 0)
//				lBuf[i] = 1000;
//			else
//				lBuf[i] = 4;
//		}
//        return ROSetParameter( FALSE, ulAxis, lBuf, IT_SCURVE );
//	}
//    return FAILURE;

//    ULONG ulAxis;
//    long lBuf[8]={0,0,0,0,0,0,0,0};

//	if(CMDReadHFLGandTAvalue(&ulAxis, instr, lBuf)==FAILURE)
//		return FAILURE;

//   return ROEnableSCurveProfile( ulAxis, lBuf );  /* set the S-curve motion profile flag (enable/disable) */
}

/***************************************************************************
 * Function Name:   ex_IT
 * Description:     Set Independent Time constants.
 *
 *                  Command Syntax: IT <AXIS>, [VAL](VAL1, VAL2, VAL3)
 *                      - AXIS:     axis to enable or disable the S-curve motion profile.
 *                      - VAL:     unit in thousandth (1000 => 1, 8 => 0.008).
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_IT(instr_ptr instr)
{
    long lBuf[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;

    if( CMDgetGalilParms(instr, &ulAxis, lBuf) == FAILURE )
        return FAILURE;
	return ROSetParameter( FALSE, ulAxis, lBuf, IT_SCURVE );
//   return ROSetITValue( ulAxis, lBuf );  /* set the IT value */
}
/***************************************************************************
 * Function Name:   ex_HFLG
 * Description:     Set the Home flag of an axis or all axes. This command is used in the
 *                  case of wanting to skip the homing for testing and axis can start to move without homing
 *                  after setting the home flag of specified axis to 1.
 * Implementation:  Get the axis and flag value and validate the flag value to make sure it is either 0 or 1.
 *                  Then call ROSetHomeFlag() to set home flag according the specified flag value.
 * NOTE:            This function will get executed when the user typed HFLG command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: HFLG <AXIS>, <FLAG1>,[FLAG2, FLAG3]
 *                      - AXIS:     axis to specified for setting the home flag to either enable(1) or disable(0).
 *                      - FLAG:     Enable(1) or disable(0).
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_HFLG(instr_ptr instr)
{
    ULONG ulAxis;
    long lBuf[8]={0,0,0,0,0,0,0,0};

    /* if axis is A or a, which is for all robot axes or all prealigner axes. */
	if(CMDReadHFLGandTAvalue(&ulAxis, instr, lBuf)==FAILURE)
		return FAILURE;

    return ROSetHomeFlag( ulAxis, lBuf );  /* set the home flag according the flag value(s) */
}
/***************************************************************************
 * Function Name:   ex_RHFLG
 * Description:     Read home flag. Only in codemerge
 * Implementation:  get the home flag value for specific axis or all axis.
 *                  if there is not arguement (operand), then default get all
 *                  axis home flag.  Otherwise, get home flag value for specific
 *                  axis.
 * NOTE:            This function will get executed when the user typed RCT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RHFLG [AXIS], [VALUE1,VALUE2,VALUE3]
 *                      - AXIS: specify the axis that want to read home flag. if axis is not specified,
 *                              then there will be not second parameter.
 *                      - VALUE1,VALUE2,VALUE3: store the home flags of each axis if axis is specified.
 *                                              This is optional parameters.  It depends on the axis to have
 *                                              3 values or 1 value.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RHFLG(instr_ptr instr)
{
    long lHomeFlags[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get the home flag value */
    if( ROReadHomeFlag(ulAxis,lHomeFlags) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a next operand move to it */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lHomeFlags, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RTA
 * Description:     read S-curve profile status. Only in codemerge
 * Implementation:  get the S-curve profile flag for specific axis or all axis.
 *                  if there is no arguement (operand), then default get all axis
 *                  S-curve profile status. Otherwise, get S-curve profile status
 *                  for specified axis.
 * NOTE:            This function will get executed when the user typed RTA command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RTA [AXIS], [VALUE1,VALUE2,VALUE3]
 *                      - AXIS: specify the axis that want to read S-curve flag. if axis is not specified,
 *                              then there will be not second parameter.
 *                      - VALUE1,VALUE2,VALUE3: store the S-curves value of each axis if axis is specified.
 *                                              This is optional parameters.  It depends on the axis to have
 *                                              3 values or 1 value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RTA(instr_ptr instr)
{
    long lTAFlags[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get the S-curve profile flag value either enable(1) or disable(0) */
    if( ROReadSCurveProfile(ulAxis,lTAFlags) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a next operand move to it */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lTAFlags, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_RIT
 * Description:     read Independent Time constants.
 * NOTE:            This function will get executed when the user typed RIT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RIT [AXIS], [VALUE1,VALUE2,VALUE3]
 *                      - AXIS: specify the axis that want to read S-curve flag. if axis is not specified,
 *                              then there will be not second parameter.
 *                      - VALUE1,VALUE2,VALUE3: store the indep-time value of each axis if axis is specified.
 *                                              This is optional parameters.  It depends on the axis to have
 *                                              3 values or 1 value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RIT(instr_ptr instr)
{
    long lITValues[8]={0,0,0,0,0,0,0,0};
    ULONG ulAxis;
    CMDoperand_ptr Optr=instr->opr_ptr;

    /* get axis from first operand*/
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;
    /* get the S-curve profile flag value either enable(1) or disable(0) */
    if( ROReadITValue(ulAxis,lITValues) == FAILURE )
        return FAILURE;

    if( Optr )                  /* if there is a next operand move to it */
        Optr = Optr->next;
    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    return CMDreadBackParms( Optr, lITValues, ulAxis );
}
/***************************************************************************
 * Function Name:   ex_CP
 * Description:     Configure Polarity of Home Switches
 *                  This command is crated for Stage controllers. Stage Home switch polarity needs to be reversed.
 *                  Home switch is initialized correctly in the initialization. This command sends Galil commmands
 *                  CN ,x. Second parameter of this command sets the polarity of the Home switches
 *
 * Command Syntax:  CP [VALUE]
 *                  - VALUE:  1 - for the motor to be driven in forward direction if input is high
 *                           -1 - for motor to be driven in reverse direction if input is high
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CP(instr_ptr instr)
{
    CMDoperand_ptr Optr = instr->opr_ptr;
    long lValue;

    /* Must have motion complete. */
    if ( ~ROGetAMFlag() & RO_AXIS_ALL )
        return FAILURE;

    if(Optr)
    {
        if(CMDgetValue(Optr, &lValue) == FAILURE)
            return FAILURE;
        if( (lValue != 1) && (lValue != -1))
            return FAILURE;
        if(GASendConfigureSystemCommand(GA_CARD_0, 0, (int)lValue, 0, 0) == FAILURE)
            return FAILURE;
    }
    else
        return FAILURE;

    return SUCCESS;
}
/***************************************************************************
 * Function Name:   ex_SCALE
 * Description:     convert encoder to position or vice versa for axis
 * Implementation:  get axis to convert. Get conversion direction, i.e.
 *                  encoder to position or vice versa. Get values to convert
 *                  and convert them.
 * NOTE:            This function will get executed when the user typed SCALE command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: SCALE [AXIS], [DIRECTION], [VALUE1,VALUE2,VALUE3], [VALUE4,VALUE5,VALUE6]
 *                      - AXIS: specify the axis that want to convert on
 *                      - DIRECTION: 0 - encoder to position
 *                                   1 - position to encoder
 *                      - VALUE1,VALUE2,VALUE3: the values to convert
 *                      - VALUE4,VALUE5,VALUE6: store results of conversion
 *                                              This is optional parameters.  It depends on the axis to have
 *                                              3 values or 1 value.
 *
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCALE(instr_ptr instr)
{
    CMDoperand_ptr Optr;
    unsigned long ulAxis;
    int iScaleDirection, iCount;
    long lValue[8]={0,0,0,0,0,0,0,0};
    long lScaleValue[8]={0,0,0,0,0,0,0,0};

    if( !(Optr=instr->opr_ptr) )        /* no operand */
        return FAILURE;
    if( (ulAxis=CMDgetAxis(instr)) == (unsigned long)FAILURE )
        return FAILURE;

    if( !(Optr=Optr->next) )
        return FAILURE;
    if( CMDgetIntegerValue(Optr, &iScaleDirection) == FAILURE )
        return FAILURE;
    if( iScaleDirection != 0 && iScaleDirection != 1 )
        return FAILURE;

    /* get the axis and value(s) to be set from operand list.
     * if axis is A or a, then there must be 3rd and 4th operand */
    if( !(Optr=Optr->next) )
        return FAILURE;
    if( ulAxis == RO_AXIS_ALL || ulAxis == RO_AXIS_all )
    {
        for (iCount=0; iCount<3 && Optr; iCount++, Optr=Optr->next)
        {
            if( CMDgetValue(Optr,&lValue[iCount]) == FAILURE )  /* get value of 2nd operand */
                return FAILURE;
        }
    }
    else if( CMDgetValue(Optr,&lValue[CMDgetAxisLoc(ulAxis)]) == FAILURE )
        return FAILURE;                           /* T, R, or Z axis specified */

    if( iScaleDirection )
    {
        if( ROUnscalePos(ulAxis, lValue, lScaleValue) == FAILURE )
            return FAILURE;
    }
    else
    {
        if( ROScalePos(ulAxis, lValue, lScaleValue) == FAILURE )
            return FAILURE;
    }

    /* if there is next operand(s), then store the value(s) into the register(s);
     * otherwise, send the value(s) to comm port. */
    if( ulAxis != RO_AXIS_ALL && ulAxis != RO_AXIS_all )
        Optr = Optr->next;
    return CMDreadBackParms( Optr, lScaleValue, ulAxis );
}
