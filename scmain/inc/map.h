/******************************************************************************\
 *
 * Program:     scanner mapper Module
 * File:        map.h
 * Functions:   Cassette mapping scanner functions
 *
 * Description: Header file for map.c
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#ifndef _MAP_H
#define _MAP_H

// scan station structure
// function prototypes for execute.c
int  MPDoScan(int iPassNum, int iScOnWhichDev);
int  MPFilterPass0(int iStn, int iMaxTh);
int  MPFilterPass1(int iStn);
int  MPFilterPass2(int iStn, int iPassNum);
int  MPFilterPass3(int iStn, int iCalib, int iIter, int iNewScheme);
int  MPFilterPass4( int iStationIndexArg, int iCalibArg, long lTorgArg, char cCalStationArg, int iIOShift );

#endif
