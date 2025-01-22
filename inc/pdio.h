/******************************************************************************\
 *
 * Program:     Predictive IO algorithm
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/
#ifndef _PDIO_H
#define _PDIO_H

int PDConvAxis(ULONG ulAxis);
int PDInitLatch(void);
int PDComputeAlarm(int iAxisArg, int iDirArg, int iRunArg, int iTotalArg);
int ex_SPDA(instr_ptr instr);
int ex_RPDA(instr_ptr instr);
int ex_RDEU(instr_ptr instr);
int ex_SPCT(instr_ptr instr);
int ex_RPCT(instr_ptr instr);
int ex_RPCTE(instr_ptr instr);
int ex_SLPT(instr_ptr instr);
int ex_RLPT(instr_ptr instr);
int PDWriteLatchDataFile(void);
int PDReadLatchDataFile(void);
int PDWriteLatchCalFile(void);
int PDReadLatchCalFile(void);
int PDOutOfRange(int iAxisArg, int iDirArg, int iRunArg);
int ex_XCL(instr_ptr instr);

#endif
