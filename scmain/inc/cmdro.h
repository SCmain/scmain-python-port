/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdro.h
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

#ifndef _H_CMDRO_H
#define _H_CMDRO_H

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
int ex_ER(instr_ptr instr);
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
int ex_RM(instr_ptr instr);
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
int ex_FA(instr_ptr instr);
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
int ex_KP(instr_ptr instr);
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
int ex_KI(instr_ptr instr);
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
int ex_KD(instr_ptr instr);
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
int ex_TL(instr_ptr instr);
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
int ex_RAD(instr_ptr instr);
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
int ex_RDL(instr_ptr instr);
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
int ex_RER(instr_ptr instr);
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
int ex_RFA(instr_ptr instr);
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
int ex_RRM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RKP
 * Description:     Read Robot Proportional Constant   (KP)
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s KP values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s KP values from galil. Ohterwise, read specific
 *                  axis KP. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the KP from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RKP or RGN command
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
int ex_RKP(instr_ptr instr);
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
int ex_RKI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RKD
 * Description:     Read Robot Derivative Constant parameter. It was ZR
 *                  If there is no axis specified or 'A' is specified as axis, then read robot
 *                  axes (T,R,Z)'s KD values from galil. If 'a' is specifed as a axis, then
 *                  read prealigner axes (t,r,z)'s KD values from galil. Ohterwise, read specific
 *                  axis KD. If there is more 1 parameters, then the parameters will store the
 *                  return value(s) after read the KD from galil; otherwise, send the value(s) back
 *                  through the comm port.
 * NOTE:            This function will get executed when the user typed RKD or RZR command
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
int ex_RKD(instr_ptr instr);
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
int ex_RTL(instr_ptr instr);
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
int ex_RNSL(instr_ptr instr);
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
int ex_RPSL(instr_ptr instr);
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
int ex_RSP(instr_ptr instr);
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
int ex_SAD(instr_ptr instr);
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
int ex_SDL(instr_ptr instr);
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
int ex_SNSL(instr_ptr instr);
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
int ex_SPSL(instr_ptr instr);
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
int ex_SSP(instr_ptr instr);
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
int ex_TA(instr_ptr instr);
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
int ex_RIPW(instr_ptr instr );
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
int ex_SIPW(instr_ptr instr );
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
int ex_HFLG(instr_ptr instr);
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
int ex_RTE(instr_ptr instr);
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
int ex_RTT(instr_ptr instr);
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
int ex_RCP(instr_ptr instr);
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
int ex_RESP(instr_ptr instr);
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
int ex_RLS(instr_ptr instr);
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
int ex_RSA(instr_ptr instr);
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
int ex_RLP(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_RHFLG
 * Description:     Read home flag . Only in codemerge
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
int ex_RHFLG(instr_ptr instr);
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
int ex_RTA(instr_ptr instr);
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
int ex_SCALE(instr_ptr instr);
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
int ex_CP(instr_ptr instr);

int ex_IT(instr_ptr instr);
int ex_RIT(instr_ptr instr);


#endif
