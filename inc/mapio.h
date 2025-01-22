/******************************************************************************\
 *
 * Program:     Scan mapper IO Module
 * File:        mapio.h
 * Functions:   Map IO functions.
 *
 * Description: Header file for map.c
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Description
 *
\******************************************************************************/

#ifndef _H_MAPIO_H
#define _H_MAPIO_H

// constants
#define     WES_INTNO           0x73
#define     WES_MASK            0x08
#define     NOTSCANNING         13
#define     Z_S293_SPEED        300     /* S293 Z Speed */

// Interrupt call function prototypes
void  TESTWES_PROC( );
void  WES_PROC( );

// Function prototypes
int     MPInitScanIO(void);
void    MPRestorePROC(void);
int     MPEnableWES( void );
void    MPDisableWES( void );
int     MPEnableTestWES( void );
void    MPDisableTestWES( void );
int     MPReadLatchedPosition(long *lValue);

// new functions for pass TestWes vars for Diagnostic routine
int     MPIsNotScanning( void );
void    MPSetNotScanning( void );

int     MPGetHitcount(int iArg);
void    MPSetScanPointers(int *ipCount, long *lpZPos, int *ipTrans);
void    MPInitScanDataBuffer( void );

int     MPTestScanning( int iCardNoArg, int iVacExistsArg );

void	MPUpdateIO(int iFlagArg);
#endif
