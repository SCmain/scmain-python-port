/**********************************************************************
 *
 *  Program:        Aligner Module Interrupt Functions
 *  Function:
 *  Description:    Contains Functions used durint the CCD interrupt process while
 *                  collecting data
 *
 *  Modification history:
 *
 *  Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#ifndef _H_ALPROC_H
#define _H_ALPROC_H

int ALSetProc(int iActionArg);
int ALSelectCCDType(unsigned uMaskArg, int iIsrCCDTypeArg);
void ALCCDInterrupt1(void);              // ff_interrupt
void ALCCDInterrupt2(void);              // ff1_interrupt

int ALSelectCCDNumber(int iCCDNumArg);        // set_CCD_no - allows selection of correct CCD
                                        //      corresponding to wafer size
int ALReturnCCDMask(int iCCDNumArg);  // return CCD Mask to select the given CCD Number
int ALEnableCCD(void);                  // enable_CCD - Install CCD interrupt routine
int ALResetChuck(void);          // chuck_counter_reset - reset chuck counter
int ALStartMeasurement(void);           // mesument_start - enable data collection
int ALStopMeasurement(void);            // mesument_stop - disable data collection
int ALTakeMeasurement(long *lpAngleArg, long *lpDataArg);  // mesument_one - enable,
                                        //    - collect some data, disable collection

int ALTestChuckAndCCD(void);               // test_CCD_CHUCK
// Returns debug variable
int ALReturnDebugVar();

#endif
