/***************************************************************\
 *
 * Program:     Aligner module low level commands
 *
 * File:        cmdal.h
 *
 * Functions:   ex_SCSN
 *              ex_ALST
 *              ex_BAL
 *              ex_BASQ
 *              ex_BCOR
 *              ex_CALC
 *              ex_CCT
 *              ex_MCOR
 *              ex_MEAS
 *              ex_RAD1
 *              ex_RAD2
 *              ex_RCAFP
 *              ex_RCASP
 *              ex_RCFP
 *              ex_RCSP
 *              ex_RCT
 *              ex_RDA
 *              ex_RFAA
 *              ex_RFT
 *              ex_RMAC
 *              ex_RMAF
 *              ex_RMI
 *              ex_RMSC
 *              ex_RMSC2
 *              ex_RMSC3
 *              ex_RMSF
 *              ex_RMSF2
 *              ex_RMSF3
 *              ex_RMT
 *              ex_RPMO
 *              ex_RPTY
 *              ex_RSP1
 *              ex_RSP2
 *              ex_RWL
 *              ex_RWS
 *              ex_RWT
 *              ex_RWU
 *              ex_SAD1
 *              ex_SAD2
 *              ex_SCAFP
 *              ex_SCASP
 *              ex_SCFP
 *              ex_SCSP
 *              ex_SCT
 *              ex_SDA
 *              ex_SFAA
 *              ex_SFT
 *              ex_SMAC
 *              ex_SMAF
 *              ex_SMSC
 *              ex_SMSC2
 *              ex_SMSC3
 *              ex_SMSF
 *              ex_SMSF2
 *              ex_SMSF3
 *              ex_SMT
 *              ex_SPMO
 *              ex_SSP1
 *              ex_SSP2
 *              ex_SWL
 *              ex_SWS
 *              ex_SWT
 *              ex_SWU
 *              ex_TRACE
 *              ex_ZZ1
 *              ex_ZZ2
 *              ex_DUMPC
 *              ex_DUMPM
 *              ex_DUMPW
 *              ex_WRIW
 * 				ex_SCVAC
 * 				ex_RCVAC
 *              CMDAlign
 *              CMDReadParam
 *              CMDSetParam3
 *
 * Description: Routines for all low level commands for Pre-aligner
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_CMDAL_H
#define _H_CMDAL_H

/**************************************************************************
 * Function Name:   ex_SCSN
 * Description:     Sets calibration file serial number
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCSN(instr_ptr instr);
/**************************************************************************
 * Function Name:   ex_ALST
 * Description:     Gets Alignment status error message
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ALST(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_BAL
 * Description:     Begin Alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BAL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_BASQ
 * Description:     Begin Square Wafer Alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BASQ(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_BCOR
 * Description:     Begin Correction of offset and flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_BCOR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_CALC
 * Description:     Get item from the calibration table.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CALC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_CCT
 * Description:     Remove CT node.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_CCT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_MCOR
 * Description:     Makes Correction
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MCOR(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_MEAS
 * Description:     Measure Alignment, don't correct
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_MEAS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RAD1
 * Description:     Read the Wafer Parameter, Measurement acceleration, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RAD1(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RAD2
 * Description:     Read the Wafer Parameter, Measurement acceleration, no wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RAD2(instr_ptr instr);
/***************************************************************************
 * Function Name:  ex_RCAFP
 * Description:     Read the Wafer Parameter, CCD1 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCAFP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCASP
 * Description:     Read the Wafer Parameter, CCD2 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCASP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCFP
 * Description:     Read the Wafer Parameter, CCD1 First Pixel Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCFP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCSP
 * Description:     Read the Wafer Parameter, CCD2 first Pixel position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCSP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RCT
 * Description:     Read Calibration Table.
 * Implementation:  First, make sure there is at least 3 operand in the instruction.  Then get the
 *                  first operand which wafer size.  Get the second operand which is the type of wafer and
 *                  get the third operand which the item or index in the calibration table structure.  Then use
 *                  informtion that was provided from first 3 operands to get the data from calibration table. If
 *                  there is the fourth operand then set the data into the register; otherwise send the data to
 *                  comm port.
 * NOTE:            This function will get executed when the user typed RCT command
 *                  at the command line. The function is actually called in MRExecuteInstructions()
 *                  and execute it by getting the function pointer from oc_entry.
 *                  Also, in the oc_table each entry has information of its own execution
 *                  function pointer. Please look at LLinitCommnad for more information ex_XXX.
 *
 *                  Command Syntax: RCT <WAFER SIZE>, <WAFER TYPE>, <ITEM>, [RETURN VALUE]
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RCT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RDA
 * Description:     For calibration table
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RDA(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RFAA
 * Description:     Read the Wafer Parameter, Flat angle after alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFAA(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RFT
 * Description:     Read the Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RFT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMAC
 * Description:     Read the Wafer Parameter, Measurement acceleration for centering
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMAC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMAF
 * Description:     Read the Wafer Parameter, Measurement Acceleration for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMAF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMI
 * Description:     Read last good data in each section of data. There are 3
 *                  portion of data from 0-1999, 2000-2499, and 2500-2999.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMI(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMSC
 * Description:     Read the Wafer Parameter, Measurement speed for centering
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMSC2
 * Description:     Read the Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSC2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMSC3
 * Description:     Read the Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSC3(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMSF
 * Description:     Read the Wafer Parameter, Measurement speed for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMSF2
 * Description:     Read the Wafer Parameter, Measurement speed for flat F200
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSF2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMSF3
 * Description:     Read the Wafer Parameter, Measurement speed for flat F300
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMSF3(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RMT
 * Description:     Read the Wafer Parameter, Measurement Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RMT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RPMO
 * Description:     Read Prealigner Mode
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPMO(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RPTY
 * Description:     Read Prealigner Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RPTY(instr_ptr instr);
 /***************************************************************************
 * Function Name:   ex_RSP1
 * Description:     Read the Wafer Parameter, Measurement speed, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSP1(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RSP2
 * Description:     Read the Wafer Parameter, Measurement speed, no wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RSP2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RWL
 * Description:     Read the Wafer Parameter, Wafer Loading Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RWS
 * Description:     Read the Wafer Parameter, Wafer Size
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RWT
 * Description:     Read the Wafer Parameter, Wafer Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_RWU
 * Description:     Read the Wafer Parameter, Wafer Unloading Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_RWU(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SAD1
 * Description:     Set the wafer parameter Movement acceleration, wafer on chuck v
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAD1(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SAD2
 * Description:     Set Wafer parameters movement acceleration, wafer not on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SAD2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCAFP
 * Description:     Set the wafer parameter, CCD1 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCAFP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCASP
 * Description:     Set the wafer parameter, CCD2 Angle Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCASP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCFP
 * Description:     Set the wafer parameter, CCD1 First Pixel Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCFP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCSP
 * Description:     Set the wafer parameter, CCD2 First Pixel Position
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCSP(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SCT
 * Description:     Set Calibration Table.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SCT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SDA
 * Description:     For calibration table
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SDA(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SFAA
 * Description:     Set the wafer parameter flat angle after alignment
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SFAA(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SFT
 * Description:     Set the wafer parameter, Flat Type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SFT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMAC
 * Description:     Set the wafer parameter measurement acceleration for centering
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMAC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMAF
 * Description:     Set the wafer parameter Measurement speed for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMAF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMSC
 * Description:     Set the wafer parameter measurement speed for centering.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMSC2
 * Description:     Set Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSC2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMSC3
 * Description:     Set Wafer Parameter
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSC3(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMSF
 * Description:     Set the wafer parameter Measurement acceleration for flat
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSF(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMSF2
 * Description:     prealigner
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSF2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMSF3
 * Description:     prealigner
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMSF3(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SMT
 * Description:     Set the wafer parameter measurement type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SMT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SPMO
 * Description:     Set Prealigner Mode
 *                  0 = standard, 1 = fast speed prealigner.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE, returns FAILURE if trying to set 1
 *                  for standard prealigner.
 ***************************************************************************/
int ex_SPMO(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SSP1
 * Description:     Set the wafer parameter movement speed, wafer on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSP1(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SSP2
 * Description:     Set the wafer parameter movement speed, wafer not on chuck
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SSP2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SWL
 * Description:     Set the wafer parameter wafer loading type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWL(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SWS
 * Description:     Set the wafer parameter, Wafer Size
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWS(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SWT
 * Description:     Set the wafer parameter wafer type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWT(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_SWU
 * Description:     Set the wafer parameter, Wafer unloading type
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_SWU(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_TRACE
 * Description:     Read Trace variable
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_TRACE(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ZZ1
 * Description:     Debugging command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ZZ1(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_ZZ2
 * Description:     Debugging command
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_ZZ2(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DUMPC
 * Description:     Dump Calibration Table
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPC(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_DUMPM
 * Description:     Send Prealigner data
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPM(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_WRIM
 * Description:     Write Prealigner data
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIM(instr_ptr instr );
/***************************************************************************
 * Function Name:   ex_DUMPW
 * Description:     Dump Wafer Parameters - prealigner
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_DUMPW(instr_ptr instr);
/***************************************************************************
 * Function Name:   ex_WRIW
 * Description:     Write wafer file to controller. This command writes directly
 *                  to NVSRam and the effect is apparent immediately. The data is
 *                  sending line by line. The controller
 *                  will return back CR each line had sent down. Please look at
 *                  "Software Manual" for the order of the data.
 * Parameter:       instr - instruction pointer.
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int ex_WRIW(instr_ptr instr);
/***************************************************************************
 * Function Name:   CMDAlign
 * Description:     Common function for BAL, BCOR, BASQ and ZZ2 commands
 * Parameter:       intstr - instruction pointer
 *                  iFlag - distinguishing parameter, BAL, BASQ, BCOR, ZZ2
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDAlign(instr_ptr instr, int iFlag);
/***************************************************************************
 * Function Name:   CMDReadParam
 * Descrition:      Common code routine for commands reading wafer parameter values.
 * Parameter:       intstr - instruction pointer
 *                  lParameter - parameter value to be read
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDReadParam(CMDoperand_ptr OperandArg, long lParameterArg);
/***************************************************************************
 * Function Name:   CMDSetParam3
 * Descrition:      Reads from the command line or registers either one or all
 *                  3 values and sets the passed array to them
 *                  if only one value want to be set, the rest two values are untouched
 * Parameter:       intstr - instruction pointer
 *                  laParametersArg - aray of 3 values to be changed
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDSetParam3(instr_ptr instr, long laParametersArg[]);
/***************************************************************************
 * Function Name:   CMDReadAlignStatus
 * Descrition:      Reads the alignment status from the aligner module
 *                  and sets the passed register to it or displays it
 *                  to the screen
 * Parameter:       intstr - instruction pointer
 *                  OptrArg - last operand on the command line
 * Returns:         SUCCESS/FAILURE.
 ***************************************************************************/
int CMDReadAlignStatus(CMDoperand_ptr OptrArg);

int ex_BALI(instr_ptr instr);
int ex_CDIS(instr_ptr instr);
int ex_RRNMO(instr_ptr instr);
int ex_RRPMO(instr_ptr instr);
int ex_RTNMO(instr_ptr instr);
int ex_RTPMO(instr_ptr instr);
int ex_SRNMO(instr_ptr instr);
int ex_SRPMO(instr_ptr instr);
int ex_STNMO(instr_ptr instr);
int ex_STPMO(instr_ptr instr);
int ex_ZZ3(instr_ptr instr);
int ex_RCVAC(instr_ptr instr);
int ex_SCVAC(instr_ptr instr);

int ex_RNFA(instr_ptr instr);

int CMDSetWaferValue(CMDoperand_ptr OperandArg, int iTypeArg);
int CMDReadWaferValue(CMDoperand_ptr OperandArg, int iTypeArg);
int CMDReadTROffsetValue(CMDoperand_ptr OperandArg, int iTypeArg, long lWhichArmArg);
int CMDSetTROffsetValue(instr_ptr instr, int iTypeArg);
int CMDReadTRCalibrationVals(instr_ptr instr, int iTypeArg);

int ex_RCCD(instr_ptr instr);
#endif
