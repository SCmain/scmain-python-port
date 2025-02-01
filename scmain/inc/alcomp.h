/**********************************************************************
 *
 *  Program:        Aligner Module Computation Functions
 *
 *  File:           alcomp.h
 *
 *  Function:
 *
 *  Description:    Contains Functions and Declarations used in the alignment
 *                  computations.
 *
 *  Modification history:
 *
 *  Rev     ECO#    Date    Author          Brief Description
 *
 *********************************************************************/

#ifndef _H_ALCOMP_H
#define _H_ALCOMP_H

int ALInitNotchMeasure(int iFlatMeasureArg);
int ALMoveToNotchMeasure(int iFlatMeasArg, int iBegArg);

int ALStartNotchMeasure(int iFlatBeginingArg);
int ALNotchMeasureDir(int *ipFlatBeginingArg);
int ALComputeOffset(long lOffsetArg, int iRMovementArg);
int ALCorrectOffset(void);
int ALHomeChuck(void); 
int ALFilterRound(int iIndexArg); 
int ALFindFlat(void); 
int ALCalcCenterOffset(int *ipIndex1Arg, int *ipIndex2Arg, int *ipIndex3Arg);

int ALCalcCircleOffset(int *ipIndexArg, double *dpAngleArg, double *dpRadiusArg );
int ALFindNotchStartIndex(void);   
int ALCorrectNotchAngle(int iFlatBeginArg, int iPassNumArg);  
int ALFindNotchPosition(int iFlatBeginArg); 
int ALRotateChuck420(void);          
int ALFindShortestMove(long lFixAngleArg, long *lpMoveAngleArg);  
int ALCorrectNormAngle(long lOldPosArg, long *lpNewPosArg); 
int ALComputeWaferCenter(void);  
int ALInitWaferCenterVar(void);  
int ALCenterCircuit(void);  
int ALGetSpeedChange(long lOldPos, long lNewPos, long *lpMult);
int ALOffsetAnglePosition(void); 
int ALMoveToNotchPosition(void);      
void ALSQCalcFittest(double *dpSideXArg, double *dpSideYArg, int iLenArg);
                                    
int  ALSQCheckPeak(int iIndexArg);
void ALSQListBreaks(void); 
int  ALSQAlign(void);        
void ALSQCalibrateCCD(void);   
int  ALSQFindNotch(void);    

int ALDetectPrealigner(void);  
int ALSetPrealignerMode(int iModeArg); 
int ALSetFineScanSpeed(void);
int ALSinglePickAlignment(void);
int ALSinglePickPosition();
int ALFindNotchMidAngle(long *lpPositionArg);
int ALSinglePickNormAngle (double *dAngleArg);
int ALChangeFastMode(int iFlagArg, long lDistArg);
int ALCalcSinglePick(void);

long ALReadNotchAngle( void );

#endif
