/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by Georges Sancosme after the author (Canh Le) passed away and
 * published under GNU GPLv3
 *
 * Original Author      : Canh Le (Deceased)
 * Current Maintainer   : Georges Sancosme (georges@sancosme.net)
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
 * Program:     Controller firmware
 * File:        eth.c
 *
 * Description: Handles low-level RS-232 communications
 *		Implements Ethernet communication commands & responses.
 *
 ****************************************************************/
#include <sys/io.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>        /* for POSIX threads */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

#include <netinet/in.h>
#include <linux/if.h>

#include "sck.h"
//#include "ser.h"
//#include "serm.h"
//#include "serl.h"
//#include "sctim.h"
#include "scstat.h"
//#include "scmac.h"
//#include "secsg.h"
//#include "secsl.h"
//#include "scttr.h"
//#include "scproc.h"
#include "scver.h"
#include "fiog.h"
#include "fio.h"
//#include "scmem.h"
#include "roga.h"
#include "ro.h"
#include "scio.h"
#include "otf.h"

//#undef COMPORT6
//#define COMPORT6

//int giMaxFD;
//fd_set	fdsInput;
//fd_set	fdsOutput;

//struct timeval	stTimeout;

//unsigned int uiEOTretries = 0;
//unsigned int uiPrevAct = 99999;
//int giSECSComplete = 1;
//int giChkSum = 0;

//int giFlowDropCount = 1;
//int SERFlushRxBuff(int iPortNumArg);

int giETHCount = 0;
int giPortno = 11313;
//char *gaIPAddress;
char gaIPAddress[20];
extern long glSN3;
char gaIPAddPort[50];

extern unsigned SSuSystemStatus;

unsigned int guPortData[15];

int giFrictionTest = 0;	// Friction Test to be performed
long glFrictionDataReady = 0;	// Friction data ready?
long glPickCounter = 0;	// wafer pick counter, when it's greater than 2000
long glIdleTime = 30;	// System Idle time 30 seconds.
long glMaxWaferPickCounter = 200; // wafer pick counter will be checked against this value
int giFrictionTestAxis = 0;	// Axis tested for friction test

#define RCVBUFSIZE 256   /* Size of receive buffer */
char echoBuffer[RCVBUFSIZE];
char TCPMsgBuffer[RCVBUFSIZE];
int giTCPMsgBufferPtr = 0;
int giTCPCmd = 0;

char caTCPCmdA[10] = "CMDA";	// drift calibration table 80 integers [8][2][5]
char caTCPCmdB[10] = "CMDB";	// drift calib table Eng Unit 80 int [8][2][5]
char caTCPCmdC[10] = "CMDC";	// latch data pointers 32 integers [8][2][2]

char caTCPCmdD1[10] = "CMDD1";	// T Torque (TT) 2000 integers [2000]
char caTCPCmdD2[10] = "CMDD2";	// T Torque (TT) 2000 integers [2000]
char caTCPCmdE1[10] = "CMDE1";	// T Position (TP) 2000 integers [2000]
char caTCPCmdE2[10] = "CMDE2";	// T Position (TP) 2000 integers [2000]
char caTCPCmdF1[10] = "CMDF1";	// R TT
char caTCPCmdF2[10] = "CMDF2";	// R TT
char caTCPCmdG1[10] = "CMDG1";	// R TP
char caTCPCmdG2[10] = "CMDG2";	// R TP
char caTCPCmdH1[10] = "CMDH1";	// Z TT
char caTCPCmdH2[10] = "CMDH2";	// Z TT
char caTCPCmdI1[10] = "CMDI1";	// Z TP
char caTCPCmdI2[10] = "CMDI2";	// Z TP
char caTCPCmdJ1[10] = "CMDJ1";	// W TT
char caTCPCmdJ2[10] = "CMDJ2";	// W TT
char caTCPCmdK1[10] = "CMDK1";	// W TP
char caTCPCmdK2[10] = "CMDK2";	// W TP
char caTCPCmdL1[10] = "CMDL1";	// t TT
char caTCPCmdL2[10] = "CMDL2";	// t TT
char caTCPCmdM1[10] = "CMDM1";	// t TP
char caTCPCmdM2[10] = "CMDM2";	// t TP
char caTCPCmdN1[10] = "CMDN1";	// r TT
char caTCPCmdN2[10] = "CMDN2";	// r TT
char caTCPCmdO1[10] = "CMDO1";	// r TP
char caTCPCmdO2[10] = "CMDO2";	// r TP
char caTCPCmdP1[10] = "CMDP1";	// z TT
char caTCPCmdP2[10] = "CMDP2";	// z TT
char caTCPCmdQ1[10] = "CMDQ1";	// z TP
char caTCPCmdQ2[10] = "CMDQ2";	// z TP
char caTCPCmdR1[10] = "CMDR1";	// w TT
char caTCPCmdR2[10] = "CMDR2";	// w TT
char caTCPCmdS1[10] = "CMDS1";	// w TP
char caTCPCmdS2[10] = "CMDS2";	// w TP

char caTCPCmdD3[10] = "CMDD3";	// T Torque (TT) 2000 integers [2000]
char caTCPCmdD4[10] = "CMDD4";	// T Torque (TT) 2000 integers [2000]
char caTCPCmdE3[10] = "CMDE3";	// T Position (TP) 2000 integers [2000]
char caTCPCmdE4[10] = "CMDE4";	// T Position (TP) 2000 integers [2000]
char caTCPCmdF3[10] = "CMDF3";	// R TT
char caTCPCmdF4[10] = "CMDF4";	// R TT
char caTCPCmdG3[10] = "CMDG3";	// R TP
char caTCPCmdG4[10] = "CMDG4";	// R TP
char caTCPCmdH3[10] = "CMDH3";	// Z TT
char caTCPCmdH4[10] = "CMDH4";	// Z TT
char caTCPCmdI3[10] = "CMDI3";	// Z TP
char caTCPCmdI4[10] = "CMDI4";	// Z TP
char caTCPCmdJ3[10] = "CMDJ3";	// W TT
char caTCPCmdJ4[10] = "CMDJ4";	// W TT
char caTCPCmdK3[10] = "CMDK3";	// W TP
char caTCPCmdK4[10] = "CMDK4";	// W TP
char caTCPCmdL3[10] = "CMDL3";	// t TT
char caTCPCmdL4[10] = "CMDL4";	// t TT
char caTCPCmdM3[10] = "CMDM3";	// t TP
char caTCPCmdM4[10] = "CMDM4";	// t TP
char caTCPCmdN3[10] = "CMDN3";	// r TT
char caTCPCmdN4[10] = "CMDN4";	// r TT
char caTCPCmdO3[10] = "CMDO3";	// r TP
char caTCPCmdO4[10] = "CMDO4";	// r TP
char caTCPCmdP3[10] = "CMDP3";	// z TT
char caTCPCmdP4[10] = "CMDP4";	// z TT
char caTCPCmdQ3[10] = "CMDQ3";	// z TP
char caTCPCmdQ4[10] = "CMDQ4";	// z TP
char caTCPCmdR3[10] = "CMDR3";	// w TT
char caTCPCmdR4[10] = "CMDR4";	// w TT
char caTCPCmdS3[10] = "CMDS3";	// w TP
char caTCPCmdS4[10] = "CMDS4";	// w TP

char caTCPCmdV[10] = "CMDV";	// version string
char caTCPCmdX[10] = "CMDX";	// ip address and port no
char caTCPCmdZ313[10] = "CMDZ313"; // Friction Error OUT command
char caTCPCmdZ314[10] = "CMDZ314"; // Friction Error OUT command
char caTCPCmdZ315[10] = "CMDZ315"; // Friction Error OUT command
char caTCPCmdZ316[10] = "CMDZ316"; // Friction Error OUT command
char caTCPCmdFRT[10]  = "CMDFRT";  // Perform Friction test command
char caTCPCmdRDY[10]  = "CMDRDY";  // Is data ready?
char caTCPCmdRES1[10] = "CMDRES1";  // Motor resolution detected by friction test.
char caTCPCmdRES2[10] = "CMDRES2";  // Encoder resolution in parameter file

char caTCPCmdIO[10]   = "CMDIO";   // Get IO's
char caTCPCmdSTA[10]  = "CMDSTA";  // Get Status unsigned integer

char caTCPCmdVTM0[10]  = "CMDVTM0";  // Get Vacuum IO Time in ms
char caTCPCmdVTM1[10]  = "CMDVTM1";  // Get Vacuum IO Time in ms
char caTCPCmdVTM2[10]  = "CMDVTM2";  // Get Vacuum IO Time in ms
char caTCPCmdVTM3[10]  = "CMDVTM3";  // Get Vacuum IO Time in ms
char caTCPCmdVTM4[10]  = "CMDVTM4";  // Get Vacuum IO Time in ms
char caTCPCmdVTM5[10]  = "CMDVTM5";  // Get Vacuum IO Time in ms
char caTCPCmdVTM6[10]  = "CMDVTM6";  // Get Vacuum IO Time in ms
char caTCPCmdVTM7[10]  = "CMDVTM7";  // Get Vacuum IO Time in ms
char caTCPCmdVTM8[10]  = "CMDVTM8";  // Get Vacuum IO Time in ms
char caTCPCmdVTM9[10]  = "CMDVTM9";  // Get Vacuum IO Time in ms
char caTCPCmdVTC[10]  = "CMDVTC";  // Get Vacuum IO Time counter

char caTCPCmd1[10] = "CMD1";	// DFT result T 2000 doubles
char caTCPCmd2[10] = "CMD2";	// DFT result R
char caTCPCmd3[10] = "CMD3";	// DFT result Z
char caTCPCmd4[10] = "CMD4";	// DFT result W
char caTCPCmd5[10] = "CMD5";	// DFT
char caTCPCmd6[10] = "CMD6";    // DFT
char caTCPCmd7[10] = "CMD7";    // DFT
char caTCPCmd8[10] = "CMD8";    // DFT

char caTCPCmda[10] = "CMDa";	// latch T +/- direction data 1000 integers (500 + 500)
char caTCPCmdb[10] = "CMDb";	// latch R
char caTCPCmdc[10] = "CMDc";    // latch Z
char caTCPCmdd[10] = "CMDd";    // latch W
char caTCPCmde[10] = "CMDe";    // latch t
char caTCPCmdf[10] = "CMDf";    // latch r
char caTCPCmdg[10] = "CMDg";    // latch z
char caTCPCmdh[10] = "CMDh";	// latch w

char caTCPCmdi[10] = "CMDi";	// latch T EU +/- direction data 1000 integers (500 + 500)
char caTCPCmdj[10] = "CMDj";	// latch R EU
char caTCPCmdk[10] = "CMDk";    // latch Z EU
char caTCPCmdl[10] = "CMDl";    // latch W EU
char caTCPCmdm[10] = "CMDm";    // latch t EU
char caTCPCmdn[10] = "CMDn";    // latch r EU
char caTCPCmdo[10] = "CMDo";    // latch z EU
char caTCPCmdp[10] = "CMDp";	// latch w EU

char caTCPCmdq1[10] = "CMDq1";	// T Pos (TP) EU 2000 integers [2000]
char caTCPCmdq2[10] = "CMDq2";	// T Pos (TP) EU 2000 integers [2000]
char caTCPCmdr1[10] = "CMDr1";	// R Pos (TP) EU
char caTCPCmdr2[10] = "CMDr2";	// R Pos (TP) EU
char caTCPCmds1[10] = "CMDs1";	// Z Pos (TP) EU
char caTCPCmds2[10] = "CMDs2";	// Z Pos (TP) EU
char caTCPCmdt1[10] = "CMDt1";	// W Pos (TP) EU
char caTCPCmdt2[10] = "CMDt2";	// W Pos (TP) EU
char caTCPCmdu1[10] = "CMDu1";	// t Pos (TP) EU
char caTCPCmdu2[10] = "CMDu2";	// t Pos (TP) EU
char caTCPCmdv1[10] = "CMDv1";	// r Pos (TP) EU
char caTCPCmdv2[10] = "CMDv2";	// r Pos (TP) EU
char caTCPCmdw1[10] = "CMDw1";	// z Pos (TP) EU
char caTCPCmdw2[10] = "CMDw2";	// z Pos (TP) EU
char caTCPCmdx1[10] = "CMDx1";	// w Pos (TP) EU
char caTCPCmdx2[10] = "CMDx2";	// w Pos (TP) EU

char caTCPCmdq3[10] = "CMDq3";	// T Pos (TP) EU 2000 integers [2000]
char caTCPCmdq4[10] = "CMDq4";	// T Pos (TP) EU 2000 integers [2000]
char caTCPCmdr3[10] = "CMDr3";	// R Pos (TP) EU
char caTCPCmdr4[10] = "CMDr4";	// R Pos (TP) EU
char caTCPCmds3[10] = "CMDs3";	// Z Pos (TP) EU
char caTCPCmds4[10] = "CMDs4";	// Z Pos (TP) EU
char caTCPCmdt3[10] = "CMDt3";	// W Pos (TP) EU
char caTCPCmdt4[10] = "CMDt4";	// W Pos (TP) EU
char caTCPCmdu3[10] = "CMDu3";	// t Pos (TP) EU
char caTCPCmdu4[10] = "CMDu4";	// t Pos (TP) EU
char caTCPCmdv3[10] = "CMDv3";	// r Pos (TP) EU
char caTCPCmdv4[10] = "CMDv4";	// r Pos (TP) EU
char caTCPCmdw3[10] = "CMDw3";	// z Pos (TP) EU
char caTCPCmdw4[10] = "CMDw4";	// z Pos (TP) EU
char caTCPCmdx3[10] = "CMDx3";	// w Pos (TP) EU
char caTCPCmdx4[10] = "CMDx4";	// w Pos (TP) EU

char caTCPCmdOTFA[10]  = "CMDOTFA";  // Get OTF parameters for station A
char caTCPCmdOTFB[10]  = "CMDOTFB";  // Get OTF parameters for station B
char caTCPCmdOTFC[10]  = "CMDOTFC";  // Get OTF parameters for station C
char caTCPCmdOTFD[10]  = "CMDOTFD";  // Get OTF parameters for station D
char caTCPCmdOTF1[10]  = "CMDOTF1";  // Get OTF data
char caTCPCmdOTF2[10]  = "CMDOTF2";  // Get OTF sensor map

extern long glPDCalTable[8][2][5]; // 0(min), 1(max), 2(ave)
extern long glPDCalTableEU[8][2][5]; // 0(min), 1(max), 2(ave)
extern long glLatchData[8][2][500];
extern long glLatchDataEU[8][2][500];
extern long glLatchPtr [8][2][2];
extern int  TTTData[TTARRAYSIZE];
extern int  TTPData[TTARRAYSIZE];
extern int  TTPDataEU[TTARRAYSIZE];
extern double TDFT[TTARRAYSIZE];
extern int  RTTData[TTARRAYSIZE];
extern int  RTPData[TTARRAYSIZE];
extern int  RTPDataEU[TTARRAYSIZE];
extern double RDFT[TTARRAYSIZE];
extern int  ZTTData[TTARRAYSIZE];
extern int  ZTPData[TTARRAYSIZE];
extern int  ZTPDataEU[TTARRAYSIZE];
extern double ZDFT[TTARRAYSIZE];
extern int  WTTData[TTARRAYSIZE];
extern int  WTPData[TTARRAYSIZE];
extern int  WTPDataEU[TTARRAYSIZE];
extern double WDFT[TTARRAYSIZE];
extern int  tTTData[TTARRAYSIZE];
extern int  tTPData[TTARRAYSIZE];
extern int  tTPDataEU[TTARRAYSIZE];
extern double tDFT[TTARRAYSIZE];
extern int  rTTData[TTARRAYSIZE];
extern int  rTPData[TTARRAYSIZE];
extern int  rTPDataEU[TTARRAYSIZE];
extern double rDFT[TTARRAYSIZE];
extern int  zTTData[TTARRAYSIZE];
extern int  zTPData[TTARRAYSIZE];
extern int  zTPDataEU[TTARRAYSIZE];
extern double zDFT[TTARRAYSIZE];
extern int  wTTData[TTARRAYSIZE];
extern int  wTPData[TTARRAYSIZE];
extern int  wTPDataEU[TTARRAYSIZE];
extern double wDFT[TTARRAYSIZE];

extern long glMotorResolution[8];
extern long glEncoderResolution[8];

extern char caSysCfgString[15];

extern char gaSN3[20];

extern long glTimeIO[10][4][100];
extern int giTimeIOCounter[10][4];

extern int iOTFAligning;
extern psOTFStation pstOTFCurrentStation;
extern int aiPortsMap[12];
extern long glOTFData[12];
long   glOTFStruct[8];

/* Structure of arguments to pass to client thread */
struct ThreadArgs
{
    int clntSock;                      /* Socket descriptor for client */
};

int giSockfd, giNewSockfd, giPortno, giClilen, giPid;

struct sockaddr_in gsServAddr, gsCliAddr;

void HandleTCPClient(int clntSocket);
void *ThreadETH(void *threadArgs);
int ETHInitPort(void);
int ETHProcessing(void);
int TCPCmdProcess(int sockArg);

extern u_char gucAddr[10];

int giCMDStat = 0;

int TCPCmdProcess(int sockArg)
{
    int iSize;
    int iRet;
    char *buf;
    int nbytes, flen;
    int fd;
    char sBuf[80];
    int iStat;

////printf("TCPCmdProcess: %d %s long=%d int=%d llong=%d char=%d\n", strlen(TCPMsgBuffer), TCPMsgBuffer, sizeof(long), sizeof(int), sizeof(long long), sizeof(char));
//    iSize = strlen(TCPMsgBuffer);

    int halfTTSize = TTARRAYSIZE / 2;

    iRet = 0;
    if (strcmp(TCPMsgBuffer, caTCPCmdA) == 0)
	iRet = send(sockArg, (void *)glPDCalTable, 80*sizeof(long),0);
    else if (strcmp(TCPMsgBuffer, caTCPCmdB) == 0)
	iRet = send(sockArg, (void *)glPDCalTableEU, 80*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdC) == 0)
	iRet = send(sockArg, (void *)glLatchPtr, 32*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdD1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdD3) == 0)
	iRet = send(sockArg, (void *)TTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdD2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdD4) == 0)
	iRet = send(sockArg, (void *)&TTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdE1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdE3) == 0)
	iRet = send(sockArg, (void *)TTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdE2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdE4) == 0)
	iRet = send(sockArg, (void *)&TTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdF1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdF3) == 0)
	iRet = send(sockArg, (void *)RTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdF2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdF4) == 0)
	iRet = send(sockArg, (void *)&RTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdG1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdG3) == 0)
	iRet = send(sockArg, (void *)RTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdG2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdG4) == 0)
	iRet = send(sockArg, (void *)&RTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdH1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdH3) == 0)
	iRet = send(sockArg, (void *)ZTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdH2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdH4) == 0)
	iRet = send(sockArg, (void *)&ZTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdI1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdI3) == 0)
	iRet = send(sockArg, (void *)ZTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdI2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdI4) == 0)
	iRet = send(sockArg, (void *)&ZTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdJ1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdJ3) == 0)
	iRet = send(sockArg, (void *)WTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdJ2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdJ4) == 0)
	iRet = send(sockArg, (void *)&WTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdK1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdK3) == 0)
	iRet = send(sockArg, (void *)WTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdK2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdK4) == 0)
	iRet = send(sockArg, (void *)&WTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdL1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdL3) == 0)
	iRet = send(sockArg, (void *)tTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdL2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdL4) == 0)
	iRet = send(sockArg, (void *)&tTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdM1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdM3) == 0)
	iRet = send(sockArg, (void *)tTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdM2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdM4) == 0)
	iRet = send(sockArg, (void *)&tTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdN1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdN3) == 0)
	iRet = send(sockArg, (void *)rTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdN2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdN4) == 0)
	iRet = send(sockArg, (void *)&rTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdO1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdO3) == 0)
	iRet = send(sockArg, (void *)rTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdO2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdO4) == 0)
	iRet = send(sockArg, (void *)&rTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdP1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdP3) == 0)
	iRet = send(sockArg, (void *)zTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdP2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdP4) == 0)
	iRet = send(sockArg, (void *)&zTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdQ1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdQ3) == 0)
	iRet = send(sockArg, (void *)zTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdQ2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdQ4) == 0)
	iRet = send(sockArg, (void *)&zTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdR1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdR3) == 0)
	iRet = send(sockArg, (void *)wTTData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdR2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdR4) == 0)
	iRet = send(sockArg, (void *)&wTTData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdS1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdS3) == 0)
	iRet = send(sockArg, (void *)wTPData, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdS2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdS4) == 0)
	iRet = send(sockArg, (void *)&wTPData[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdV) == 0)
	iRet = send(sockArg, (void *)caSysCfgString, strlen(caSysCfgString), 0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdX) == 0)
	iRet = send(sockArg, (void *)gaIPAddPort, strlen(gaIPAddPort), 0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdZ313) == 0)
	ROTurnLight(0, 1);	// turn on 1st LED light
    else if(strcmp(TCPMsgBuffer, caTCPCmdZ314) == 0)
	ROTurnLight(0, 0);	// turn off 1st LED light
    else if(strcmp(TCPMsgBuffer, caTCPCmdZ315) == 0)
	ROTurnLight(1, 1);	// turn on 2nd LED light
    else if(strcmp(TCPMsgBuffer, caTCPCmdZ316) == 0)
	ROTurnLight(1, 0);	// turn off 2nd LED light
    else if(strcmp(TCPMsgBuffer, caTCPCmdFRT) == 0)
    {
	giFrictionTest = 1;
	glFrictionDataReady = 0; // The data is overwritten
	giFrictionTestAxis = 0;
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdRDY) == 0)
	iRet = send(sockArg, (void *)&glFrictionDataReady, sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdRES1) == 0)
{
////printf("motor resolution W=%ld\n",glMotorResolution[3]);
	iRet = send(sockArg, (void *)glMotorResolution, 8*sizeof(long),0);
}
    else if(strcmp(TCPMsgBuffer, caTCPCmdRES2) == 0)
	iRet = send(sockArg, (void *)glEncoderResolution, 8*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdSTA) == 0)
    {
	iRet = send(sockArg, (void *)&giCMDStat, sizeof(long),0);
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdIO) == 0)
    {
	giCMDStat = SSGetSystemStatus();

	guPortData[0] = inb(IO_DIO_BASE_ADR );
	guPortData[1] = inb(IO_DIO_BASE_ADR + 1);
	guPortData[2] = inb(IO_DIO_BASE_ADR + 2);
	guPortData[3] = inb(IO_DIO_BASE_ADR + 4);
	guPortData[4] = inb(IO_DIO_BASE_ADR + 5);
	guPortData[5] = inb(IO_DIO_BASE_ADR + 6);
	guPortData[6] = inb(IO_DIO_BASE_ADR + 8);
	guPortData[7] = inb(IO_DIO_BASE_ADR + 9);
	guPortData[8] = inb(IO_DIO_BASE_ADR + 10);
	guPortData[9] = inb(IO_DIO_BASE_ADR + 12);
	guPortData[10] = inb(IO_DIO_BASE_ADR + 13);
	guPortData[11] = inb(IO_DIO_BASE_ADR + 14);
	guPortData[12] = inb(IO_DIO_BASE_ADR + 16);
	guPortData[13] = inb(IO_DIO_BASE_ADR + 17);
	guPortData[14] = inb(IO_DIO_BASE_ADR + 18);

	iRet = send(sockArg, (void *)guPortData, 15*sizeof(long),0);
    }
//////////////////////////////////////////////////////////////////////////////
//    else if(strcmp(TCPMsgBuffer, caTCPCmd1) == 0)
//	iRet = send(sockArg, (void *)TDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd2) == 0)
//	iRet = send(sockArg, (void *)RDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd3) == 0)
//	iRet = send(sockArg, (void *)ZDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd4) == 0)
//	iRet = send(sockArg, (void *)WDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd5) == 0)
//	iRet = send(sockArg, (void *)tDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd6) == 0)
//	iRet = send(sockArg, (void *)rDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd7) == 0)
//	iRet = send(sockArg, (void *)zDFT, TTARRAYSIZE*sizeof(double),0);
//    else if(strcmp(TCPMsgBuffer, caTCPCmd8) == 0)
//	iRet = send(sockArg, (void *)zDFT, TTARRAYSIZE*sizeof(double),0);
//////////////////////////////////////////////////////////////////////////////
    else if(strcmp(TCPMsgBuffer, caTCPCmda) == 0)
	iRet = send(sockArg, (void *)&glLatchData[0][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdb) == 0)
	iRet = send(sockArg, (void *)&glLatchData[1][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdc) == 0)
	iRet = send(sockArg, (void *)&glLatchData[2][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdd) == 0)
	iRet = send(sockArg, (void *)&glLatchData[3][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmde) == 0)
	iRet = send(sockArg, (void *)&glLatchData[4][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdf) == 0)
	iRet = send(sockArg, (void *)&glLatchData[5][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdg) == 0)
	iRet = send(sockArg, (void *)&glLatchData[6][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdh) == 0)
	iRet = send(sockArg, (void *)&glLatchData[7][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdi) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[0][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdj) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[1][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdk) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[2][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdl) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[3][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdm) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[4][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdn) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[5][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdo) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[6][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdp) == 0)
	iRet = send(sockArg, (void *)&glLatchDataEU[7][0][0], 1000*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdq1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdq3) == 0)
	iRet = send(sockArg, (void *)TTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdq2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdq4) == 0)
	iRet = send(sockArg, (void *)&TTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdr1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdr3) == 0)
	iRet = send(sockArg, (void *)RTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdr2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdr4) == 0)
	iRet = send(sockArg, (void *)&RTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmds1) == 0 || strcmp(TCPMsgBuffer, caTCPCmds3) == 0)
	iRet = send(sockArg, (void *)ZTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmds2) == 0 || strcmp(TCPMsgBuffer, caTCPCmds4) == 0)
	iRet = send(sockArg, (void *)&ZTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdt1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdt3) == 0)
	iRet = send(sockArg, (void *)WTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdt2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdt4) == 0)
	iRet = send(sockArg, (void *)&WTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdu1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdu3) == 0)
	iRet = send(sockArg, (void *)tTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdu2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdu4) == 0)
	iRet = send(sockArg, (void *)&tTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdv1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdv3) == 0)
	iRet = send(sockArg, (void *)rTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdv2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdv4) == 0)
	iRet = send(sockArg, (void *)&rTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdw1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdw3) == 0)
	iRet = send(sockArg, (void *)zTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdw2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdw4) == 0)
	iRet = send(sockArg, (void *)&zTPDataEU[halfTTSize-1], halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdx1) == 0 || strcmp(TCPMsgBuffer, caTCPCmdx3) == 0)
	iRet = send(sockArg, (void *)wTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdx2) == 0 || strcmp(TCPMsgBuffer, caTCPCmdx4) == 0)
	iRet = send(sockArg, (void *)&wTPDataEU, halfTTSize*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM0) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[0][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM1) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[1][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM2) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[2][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM3) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[3][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM4) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[4][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM5) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[5][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM6) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[6][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM7) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[7][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM8) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[8][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTM9) == 0)
	iRet = send(sockArg, (void *)&glTimeIO[9][0][0], 400*sizeof(long),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdVTC) == 0)
	iRet = send(sockArg, (void *)giTimeIOCounter, 40*sizeof(int),0);
    else if(strcmp(TCPMsgBuffer, caTCPCmdOTFA) == 0)
    {
	if(!iOTFAligning)
	{
	    if(MPFindOTFStation('A', FALSE))
	    {
	    	glOTFStruct[0] = (long)pstOTFCurrentStation->m_fOTFDistance[0]; // sensor 1 pos
	    	glOTFStruct[1] = (long)pstOTFCurrentStation->m_fOTFDistance[1]; // sensor 2 pos
	    	glOTFStruct[2] = (long)pstOTFCurrentStation->m_fOTFDistance[2]; // sensor 3 pos
	    	glOTFStruct[3] = (long)pstOTFCurrentStation->m_lEELength; 	    // EE length
	    	glOTFStruct[4] = (long)(pstOTFCurrentStation->m_fOTFRef[0][0]*1000.0); // X ref
	    	glOTFStruct[5] = (long)(pstOTFCurrentStation->m_fOTFRef[0][1]*1000.0); // Y ref
	    }
	}

	iRet = send(sockArg, (void *)glOTFStruct, 8*sizeof(long),0);
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdOTFA) == 0)
    {
	if(!iOTFAligning)
	{
	    if(MPFindOTFStation('B', FALSE))
	    {
	    	glOTFStruct[0] = (long)pstOTFCurrentStation->m_fOTFDistance[0]; // sensor 1 pos
	    	glOTFStruct[1] = (long)pstOTFCurrentStation->m_fOTFDistance[1]; // sensor 2 pos
	    	glOTFStruct[2] = (long)pstOTFCurrentStation->m_fOTFDistance[2]; // sensor 3 pos
	    	glOTFStruct[3] = (long)pstOTFCurrentStation->m_lEELength; 	    // EE length
	    	glOTFStruct[4] = (long)(pstOTFCurrentStation->m_fOTFRef[0][0]*1000.0); // X ref
	    	glOTFStruct[5] = (long)(pstOTFCurrentStation->m_fOTFRef[0][1]*1000.0); // Y ref
	    }
	}

	iRet = send(sockArg, (void *)glOTFStruct, 8*sizeof(long),0);
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdOTFA) == 0)
    {
	if(!iOTFAligning)
	{
	    if(MPFindOTFStation('C', FALSE))
	    {
	    	glOTFStruct[0] = (long)pstOTFCurrentStation->m_fOTFDistance[0]; // sensor 1 pos
	    	glOTFStruct[1] = (long)pstOTFCurrentStation->m_fOTFDistance[1]; // sensor 2 pos
	    	glOTFStruct[2] = (long)pstOTFCurrentStation->m_fOTFDistance[2]; // sensor 3 pos
	    	glOTFStruct[3] = (long)pstOTFCurrentStation->m_lEELength; 	    // EE length
	    	glOTFStruct[4] = (long)(pstOTFCurrentStation->m_fOTFRef[0][0]*1000.0); // X ref
	    	glOTFStruct[5] = (long)(pstOTFCurrentStation->m_fOTFRef[0][1]*1000.0); // Y ref
	    }
	}

	iRet = send(sockArg, (void *)glOTFStruct, 8*sizeof(long),0);
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdOTFA) == 0)
    {
	if(!iOTFAligning)
	{
	    if(MPFindOTFStation('D', FALSE))
	    {
	    	glOTFStruct[0] = (long)pstOTFCurrentStation->m_fOTFDistance[0]; // sensor 1 pos
	    	glOTFStruct[1] = (long)pstOTFCurrentStation->m_fOTFDistance[1]; // sensor 2 pos
	    	glOTFStruct[2] = (long)pstOTFCurrentStation->m_fOTFDistance[2]; // sensor 3 pos
	    	glOTFStruct[3] = (long)pstOTFCurrentStation->m_lEELength; 	    // EE length
	    	glOTFStruct[4] = (long)(pstOTFCurrentStation->m_fOTFRef[0][0]*1000.0); // X ref
	    	glOTFStruct[5] = (long)(pstOTFCurrentStation->m_fOTFRef[0][1]*1000.0); // Y ref
	    }
	}

	iRet = send(sockArg, (void *)glOTFStruct, 8*sizeof(long),0);
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdOTF1) == 0)
    {
	iRet = send(sockArg, (void *)glOTFData, 12*sizeof(long),0);
    }
    else if(strcmp(TCPMsgBuffer, caTCPCmdOTF2) == 0)
    {
	iRet = send(sockArg, (void *)aiPortsMap, 12*sizeof(int),0);
    }

//    iRet = write(sockArg, TCPMsgBuffer, iSize);
//    iRet = send(sockArg, TCPMsgBuffer, iSize, 0);
//printf("write ret=%d\n",iRet);
//    TIDelay(2000);

exit_return:
    return SUCCESS;
}

int inet_ntoa_r( struct in_addr in, char *s, unsigned int slen)
{
    unsigned char *bytes = (unsigned char *) &(in.s_addr);
    if (slen < 16 || !s)
    {
	return -1;
    }
    snprintf(s, slen, "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
    return 0;
}

int ETHInitPort(void)
{
    u_char addr[80];
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i, iTot;
    int ok = 0;
    int iDone;

    FILE *iFP;
    char aStr[80];
    char *istr;

    giSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (giSockfd < 0) 
    {
//printf("ETHInitPort: socket()=%d failed.\n",giSockfd);
        return FAILURE;
    }

    // get Ethernet HW addr
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(giSockfd, SIOCGIFCONF, &ifc);

    IFR = ifc.ifc_req;
    for(i=ifc.ifc_len/sizeof(struct ifreq); --i >= 0; IFR++)
    {
	strcpy(ifr.ifr_name, IFR->ifr_name);
  	if (ioctl(giSockfd, SIOCGIFFLAGS, &ifr) == 0)
	{
	    if (!(ifr.ifr_flags & IFF_LOOPBACK)) 
	    {
		if(ioctl(giSockfd, SIOCGIFHWADDR, &ifr) == 0)
		{
		    ok = 1;
		    break;
		}
	    }
	}
    }

    if(ok)
    {
	bcopy (ifr.ifr_hwaddr.sa_data, addr, 6);
    }

    // check
    for(i=0; i<6; ++i)
    {
	if(addr[i] != gucAddr[i])
	{
	    //printf("Eth addr does not match!\n");
	    return FAILURE;
	}
    }





    bzero((char *) &gsServAddr, sizeof(gsServAddr));
    gsServAddr.sin_family = AF_INET;
    gsServAddr.sin_addr.s_addr = INADDR_ANY;
    gsServAddr.sin_port = htons(giPortno);
    if (bind(giSockfd, (struct sockaddr *) &gsServAddr, sizeof(gsServAddr)) < 0)
    {
	close(giSockfd);
        giSockfd = socket(AF_INET, SOCK_STREAM, 0);
	++giPortno;
        gsServAddr.sin_port = htons(giPortno);
        if (bind(giSockfd, (struct sockaddr *) &gsServAddr, sizeof(gsServAddr)) < 0)
        {
  	    close(giSockfd);
            giSockfd = socket(AF_INET, SOCK_STREAM, 0);
	    ++giPortno;
            gsServAddr.sin_port = htons(giPortno);
            if (bind(giSockfd, (struct sockaddr *) &gsServAddr, sizeof(gsServAddr)) < 0)
            {
//printf("ETHInitPort: bind() failed. Portno=%d\n",giPortno);
  	        close(giSockfd);
                return FAILURE;
	    }
	}
    }
printf("ETHInitPort: bind() OK Portno=%d\n",giPortno);

    iFP = fopen("/etc/sysconfig/network-scripts/ifcfg-enp1s8", "r");
    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful configuration table file open... */
        printf( "Ethernet config file: /etc/sysconfig/network-scripts/ifcfg-enp1s8: Open Error\n" );
  	close(giSockfd);
        return FAILURE;
    }

    iDone = 0;
    while(!iDone)
    {
	istr = fgets(aStr, 180, iFP);
	if(istr != NULL && istr != EOF)
	{
	    if (strncmp(istr,"IPADDR", 6) == 0)
	    {
		for (i=7; i<30 && istr[i] != '\n'; ++i)
		    gaIPAddress[i-7]=istr[i];
 		gaIPAddress[i-7] = '\0';
	    }
	}
	else
	    iDone = 1;
    }
    fclose(iFP);

    sprintf(gaIPAddPort,"%s %d %d", gaIPAddress, giPortno, glSN3);

printf("gaIPAddPort: %s\n", gaIPAddPort);

    listen(giSockfd,5);
    giClilen = sizeof(gsCliAddr);

    return SUCCESS;
}

int ETHProcessing(void)
{
    int i, nRead;
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    int maxDescriptor;               /* Maximum socket descriptor value */
    fd_set sockSet;                  /* Set of socket descriptors for select() */
    struct timeval selTimeout;       /* Timeout for select() */
    int iRet;
    int iDone;

//    unsigned short echoServPort;     /* Server port */
//    pthread_t threadID;              /* Thread ID from pthread_create() */
//    struct ThreadArgs *threadArgs;   /* Pointer to argument structure for thread */

//    servSock = accept(giSockfd, (struct sockaddr *) &gsCliAddr, &giClilen);
//    if (servSock < 0) 
//    {
////printf("ETHProcessing: accept()=%d failed.\n",servSock);
//        return FAILURE;
//    }

    if(++giETHCount < 3) return SUCCESS;
    giETHCount = 0;

    /* Zero socket descriptor vector and set for server sockets */
    /* This must be reset every time select() is called */
    FD_ZERO(&sockSet);
    /* Add keyboard to descriptor vector */
    FD_SET(giSockfd, &sockSet);
    maxDescriptor = giSockfd;

//    for (port = 0; port < noPorts; port++)
//    FD_SET(servSock[port], &sockSet);

    /* Timeout specification */
    /* This must be reset every time select() is called */
    selTimeout.tv_sec = 0;       /* timeout (secs.) */
    selTimeout.tv_usec = 0;            /* 0 microseconds */

    /* Suspend program until descriptor is ready or timeout */
    iRet = select(maxDescriptor + 1, &sockSet, NULL, NULL, &selTimeout);
//        //printf("No echo requests for 0 secs...Server still alive\n");
//        if (FD_ISSET(0, &sockSet)) /* Check keyboard */
//        {
//            //printf("Shutting down server\n");
//            getchar();
//            running = 0;
//        }

//        for (port = 0; port < noPorts; port++)
    if (FD_ISSET(giSockfd, &sockSet))
    {
        //printf("Request on port giSockfd=%d:\n", giSockfd);
        servSock = accept(giSockfd, (struct sockaddr *) &gsCliAddr, &giClilen);
        if (servSock < 0) 
        {
//printf("ETHProcessing: accept()=%d failed.\n",servSock);
             return FAILURE;
        }

        bzero(echoBuffer,RCVBUFSIZE);
	iDone=0;

	nRead = recv(servSock, echoBuffer, RCVBUFSIZE, 0);

        if(nRead > 0)
	{
////printf("msg read len: %d %s\n",nRead, echoBuffer);
	    for (i = 0; i <= nRead; ++i)
	    {
		if(echoBuffer[i] == '\n' || echoBuffer[i] == '\r' || echoBuffer[i] == '\0' )
		{
////printf("%c - end\n",echoBuffer[i]);
		    TCPMsgBuffer[giTCPMsgBufferPtr++] = '\0';
		    giTCPCmd = 1;
		}
		else
		{
////printf("%c",echoBuffer[i]);
		    TCPMsgBuffer[giTCPMsgBufferPtr++] = echoBuffer[i];
		}
	    }
//printf("TCPMsg: %s\n",TCPMsgBuffer);
	    if(giTCPCmd)
	    {
		TCPCmdProcess(servSock);
		giTCPCmd = 0;
		giTCPMsgBufferPtr = 0;
                iDone = 1;
	    }
	}
     	close(servSock);

    }
    return SUCCESS;
}

void *ThreadETH(void *threadArgs)
{
    int clntSock;                   /* Socket descriptor for client connection */

    /* Guarantees that thread resources are deallocated upon return */
    pthread_detach(pthread_self()); 

    /* Extract socket file descriptor from argument */
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);              /* Deallocate memory for argument */

    HandleTCPClient(clntSock);

    return (NULL);
}

void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    int n = read(clntSocket,echoBuffer,RCVBUFSIZE);
    if (n < 0)
    { 
	//printf("ERROR reading from socket\n");
//	goto exit_return;
    }
    
    if (n>0) 
    {
 	//printf("Message read: %s\n",echoBuffer);
        n = write(clntSocket,"I got your message",18);
     	if (n < 0)
        { 
	    //printf("ERROR reading from socket\n");
//	    goto exit_return;
        }
    }
    goto exit_return;

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
    {
//printf("HandleTCPClient: recv() failed=%d\n",recvMsgSize);
//	goto exit_return;
    }

    sprintf(echoBuffer, "echoBuffer from Server!\n\r");
    recvMsgSize = 26;

    /* Echo message back to client */
    if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
    {
//printf("send failed: recvMsgSize=%d\n",recvMsgSize);
        goto exit_return;
    }

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
        /* Echo message back to client */
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
	{
//printf("send failed: recvMsgSize=%d\n",recvMsgSize);
	    goto exit_return;
	}

        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
	{
//printf("recv failed: recvMsgSize=%d\n",recvMsgSize);
	    goto exit_return;
	}
    }

exit_return:

    close(clntSocket);    /* Close client socket */
}

