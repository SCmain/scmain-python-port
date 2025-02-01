/**********************************************************************
 *
 *  Program:        Aligner module Process Functions
 *  Function:
 *  Description:    Contains Functions and Declarations used in the alignment process
 *
 *  Modification history:
 *
 *  Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#ifndef _H_ALSTEP_H
#define _H_ALSTEP_H

// prototypes for this part of the module
int ALGetWaferRoundSeg(int iWaferSizeArg, double *dpSegmentArg);
                                // ROUND_SEG_X - size of the wafers round segment
int ALSetWaferSize(int iWaferSizeArg);  // set_wafer_size()

int ALAbort(void);              // abort_alignment - abort alignment process
int ALBegin(void);              // bal_begin, begin alignment process
int ALInit(int);                // bal_init - init. alignment process
int ALStartMeasureProc(void);   // bal_start_mes - start alignment process meas.
int ALCheckEdge(void);          // bal_edge_check - find round and flat in data
int ALFilterEdgeNotch(void);         // bal_edge_filter - find flat in data collected
int ALCenterCalc(int iAlignTypeArg); // bal_center_caucul - find wafer center (BALN only)
int ALCenterCorrectBCOR(void);   // bal_center_corect - find center & flat (BCOR only)
int ALNotchFromCenter(void);     // bal_flat_from_cent - correct center offset
int ALPositionNotch(void);       // bal_flat_position - using flat find (notch) center
int ALCorrectNotchPos(void);        // bal_flat_corect - makes final movement of flat,
                                            //          including customers requested location
int ALEnd(void);                // bal_end - ending alignment process

int ALSQWaferAlignment(int iCCDArg);  // DoAll()
int ALSetDefaultWaferValue(int iWaferSizeArg, int iCCDArg,int iMSKCCDArg, double dPrimFlatArg, double dMainFlatArg,
							int iRndLevelArg, long lMaxOffsetCorArg, double dOffsetSizeCor, int iMinFlatLevelArg  );
int ALLoadWaferCal();
int ALValidWaferSize(long lWaferSizeArg);

#endif
