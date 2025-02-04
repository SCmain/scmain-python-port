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
 * File:        fiog.c
 *
 * Program:     Controller firmware
 * Functions:   FIOTestFileChkSum
 *              FIOTestNVSRAM
 *              FIOSetDefaultParam
 *              FIOInitParamModule
 *              FIOReadParam
 *              FIOWriteParam
 *              FIOInitCoordinate
 *              FIOReadCoord
 *              FIOWriteCoord
 *              FIOInitConfig
 *              FIOReadConfig
 *              FIOWriteConfig
 *              FIOValidateParaDesc
 *              FIOValidateStationIndex
 *              FIOGetParamFileType
 *              FIOErrHandler
 *              FIOInitNVSErrorHandler
 *              FIOComputeChkSum
 *              FIONoBDrive
 *              FIOTestBlockChkSum
 *              FIOGetParam
 *              FIOSetParam
 *              FIOGetCfgFeatureFlags
 *              FIOSetCfgFeatureFlags
 *              FIOWriteOTFFile
 *              FIOReadOTFFile
 *              FIOWriteBytes
 *
 * Description: Provide interface between PARA and Mechanism
 *              modules
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * NOTE: The ??? in the comments indicates that the variable
 *      was not used at the time of commenting so it's
 *      function was not understood enough to write a comment.
 *      They must be fixed as they come into use and are debugged.
 *
 * NOTE: CSI stands for Coordinate Special Item. These items are used
 *      for special values that are intended for use by the user
 *      and are recorded in the coordinate file during a save so they
 *      can be recalled and used again at a later date. Some of them
 *      have come to have standard meanings. For example, CSI 8
 *      is now typically used for the cross-slot threshold.
 *
 * NOTE: When values are changed using host commands, they are
 *      only changed locally in the file structure. If the
 *      system power cycles, those changes are lost, UNLESS
 *      they are saved with the SAV host command. Then the
 *      values are gathered and written to data files in the
 *      NVSRAM.
 *
\***************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>

#include "fio.h"
#include "fiol.h"
#include "sctim.h"
#include "scttr.h"
#include "sck.h"
#include "scstat.h"
#include "ser.h"
#include "romain.h"
#include "otf.h"
#include "scmem.h"
#include "dmclnx.h"

#include "scver.h"
#include "rofn.h"

extern HANDLEDMC ghDMC;

extern int giNumOfAxes;
extern int giSysCfgNum;

/********** Variables Used Locally **********/
/* Parameter file structures for holding variable values pertaining to the parameter files. */
stCtrlParam sRobotParam;        /* ctrl_par     Global parameter structure for the robot parameter file */
stCtrlParam sPreParam;          /* ctrl_par_t, prectrl_par
                                 *              Global parameter structure for the pre-aligner parameter file. */
stCtrlParam sAuxParam;          /* ctrl_par_t, *pauxctrl_par, auxctrl_par
                                 *              Global parameter structure for the auxilliary parameter file. */
unsigned uRobotChkSum;
unsigned uPreChkSum;
unsigned uAuxChkSum;

//stLatchCalData aLatchCalData;

/* Configuration table structure for holding variable values pertaining to the configuration file. */
stCfgTable sCfgTbl;

/* Station/coordinate variables */
stStationData aStationData[FIO_MAX_NUM_STATIONS];    /* Array of structures to hold the variable
                                                     * values for each station. */
int iCurrentStation;            /* The station currently selected to be active. Typically
                                 * the last station used in a move command. */

float gfCoordFileRevision;        /* Coordinate file revision */

stVectorParameter sVectorParameter;

extern int iDefineFlag, iEmulatorMode, iaDiagParms[3];

int FIODownGalilMac()
{
    FILE *iFP;
    char* ipos;
    char* istr; 
    int i,iDone=0;
    char aMacLine[180];
    char pcRespStr[180];
    int iStatus;
    int ctrlQ = 0x11;
    int nLine;
//    char* strBuf[999];
//    char aMacBuf[999][180];

//    int iNumAxes;
//    int iaEquipeAxis[8], iaGalilAxis[8], iaMechType[8], iaSpecialAxis[8];

//    struct timespec tv;
//    tv.tv_sec = 0;
//    iStatus = DMCDownloadFile(ghDMC, (PSZ)GMCFILENAME, NULL);
//    iStatus = DMCSendFile(ghDMC, (PSZ)GMCFILENAME);
//    printf("DMCSendFile: %d\n",iStatus);
//    return SUCCESS;
//
//int ctrlD = 0x04;
//aMacLine[0]=ctrlD;
//aMacLine[1]='\0';
//sprintf(aMacLine,"DL\n");
//iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 80);
//sprintf(aMacLine,"gmac\n");
//iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 80);
//return SUCCESS;

    /* Open the parameter file in NVSRAM. */
    if (giNumOfAxes > 4)
	iFP = fopen(GM8FILENAME, "r");
    else
	iFP = fopen(GM4FILENAME, "r");

    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful configuration table file open... */
        printf( "Galil Macro Open Error\n" );
	return FAILURE;
    }
//    sprintf(aMacLine, "RS\n");
//    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
    iDone = nLine = 0;
/*
    while(!iDone)
    {
//	nRead = getline(aMacLine, &len, iFP);
	istr = fgets(aMacBuf[nLine], 180, iFP);
	if(istr != NULL && istr != EOF)
	{
	    nLine++;
	}
	else
	    iDone = 1;
    }
    //printf("nLine = %d\n",nLine);
    fclose( iFP );
*/
    sprintf(aMacLine, "DL\n");
    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
    if(iStatus |= 0)
 	printf("DL cmd failed: %d\n",iStatus);
/* 
    for(i=0; i<nLine; ++i)
    {
	iStatus = DMCCommand( ghDMC, aMacBuf[i], pcRespStr, 180);
	if(iStatus != 0)
	    //printf("DMC error:%d comm:%s resp:%s\n",iStatus, aMacBuf[i], pcRespStr);
    }
*/

    iDone = nLine = 0;
    while(!iDone)
    {
//	nRead = getline(aMacLine, &len, iFP);
	istr = fgets(aMacLine, 180, iFP);
	if(istr != NULL && istr != EOF)
	{
//	    ipos = index(aMacLine, 0x27); // 0x27 = "'"
//	    if(ipos != 0)
//	    {
//		*ipos++ = '\n';
//		*ipos = '\0';
//		aMacLine[ipos] = '\0';
//	 	aMacLine[ipos+1] = '\0';
//	    }
//	    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
//    tv.tv_nsec = 1000000;
//    nanosleep(&tv, NULL);
	    iStatus = DMCCommand( ghDMC, istr, pcRespStr, 180);
	    if(iStatus != 0)
		printf("DMC line:%d code:%d comm:%s resp:%s\n",nLine, iStatus, istr, pcRespStr);
	    nLine++;
	}
	else
	    iDone = 1;
    }
    //printf("nLine = %d\n",nLine);
    fclose( iFP );


    aMacLine[0]=ctrlQ; aMacLine[1]='\n'; aMacLine[2]='\0';
    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);

//    sprintf(aMacLine, "BN\n");
//    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
////printf("BN: %d\n",iStatus);
    sprintf(aMacLine, "BP\n");
    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
if(iStatus)
printf("BP: %d\n",iStatus);
//    sprintf(aMacLine, "BV\n");
//    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
//printf("BV: %d\n",iStatus);
//    sprintf(aMacLine, "MT 1,1,1,1,1,1,1,-1\n");
//    iStatus = DMCCommand( ghDMC, aMacLine, pcRespStr, 180);
//    InitMechArrays(&iNumAxes, &iDefineFlag, &iEmulatorMode, iaEquipeAxis, iaGalilAxis, iaMechType, iaSpecialAxis, iaDiagParms);
//    if (ROInit(iNumAxes, iaMechType, iaEquipeAxis, iaGalilAxis, iaSpecialAxis, iDefineFlag, iEmulatorMode) == FAILURE)
//    {
//        return FAILURE;
	//FailureExit("ROInit has failed");
        //SSSetModulesInitializationsStatus(MECHANISM_MODULE, TRUE);
//    }
    return SUCCESS;
}
/****************************************************************
 *
 * Function:    FIOErrHandler
 *
 * Abstract:    Critical error handler for int 0x24.  Checks if
 *      error occurred on drive B (the NVSRAM) and sets NVSRAM_ST
 *      if so. Always returns to application program with an error code.
 *
 * Parameters:
 *      uDevError       (in) Device error status word
 *      uErrCode        (in) Device error code
 *      ufDevHdr        (in) ???
 *
 * Returns:     None
 *
 * Replaces:    devhdr
 * Used in:     not used anywhere
 *
 ***************************************************************/
void FIOErrHandler(unsigned uDevError, unsigned uErrCode, unsigned *ufDevHdr )
{
    /* Check for BAD_NVSRAM or bad robot parameters file. */
    if( (!(uDevError &  0x8000 )) && (uDevError & 0x0001) )
    {
        /* Set the System Status Word if there is a problem, accessible through STAT. */
        SSSetStatusWord( NVSRAM_ST, TRUE );
    }
//    _hardretn( uErrCode );
}


/****************************************************************
 *
 * Function:    FIOInitNVSErrorHandler
 *
 * Abstract:    Install critical error handler defined above.
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 * Replaces:    init_nvs_err_handler
 * Used in:     main/init
 *
 ***************************************************************/
void FIOInitNVSErrorHandler()
{
    //int iReturn;

//    /* Initialize critical error handler and try to read NVSRAM. */
//    _harderr( FIOErrHandler );
//
//    /* Check whether NVSRAM is being recognized. */
//    iReturn = FIONoBDrive();
//    if( iReturn )
//        SSSetCheckSumStatus(BAD_NVSRAM, TRUE);
//    else
    //Dos&Counters
    // If there is no WriteToNVSRAMLogFile in NVSRAM create one with counters equal 0

    FIOWriteToNVSRAMLogFile(INIT_FILECOUNTERS);

    return;
}


/****************************************************************
 *
 * Function:    FIOComputeChkSum
 *
 * Abstract:    Computes a check sum for a specified block.  The checksum is
 *      computed as a 16 bit exclusive-or of all words in the block.
 *
 * Parameters:
 *      vfStartAddr     (in) The address from which to start
 *                          computing the check sum.
 *      iBlockSize      (in) The number of blocks through which
 *                          the check sum computation is conducted.
 *
 * Returns:     computed check sum
 *
 * Replaces:    get_chk_sum
 * Used in:     Save_parameters, Load_parameters, test_chk_sum,
 *
 ***************************************************************/
unsigned FIOComputeChkSum( void *vfStartAddr, int iBlockSize )
{
    unsigned uChkSum = 0;
    int iCount;
    char *fcBlock;

    fcBlock = vfStartAddr;
    for( iCount=0; iCount<iBlockSize; iCount++ )
    {
        if( iCount%2 == 1 )
            uChkSum ^= fcBlock[iCount]<<8;
        else
            uChkSum ^= fcBlock[iCount];
    }
    return( uChkSum );
}


/****************************************************************\
 *
 * Function:    FIOTestFileChkSum
 *
 * Abstract:    Test the macro file check sum, ???
 *
 * Parameters:
 *      pcFileName      (in) File Name to be tested
 *
 * Returns:     1 = BAD_NVSRAM,  2 =  block Too small, FAILURE
 *
 * Replaces:    test_macro_chksum
 * Used in:     load_macros, test_NVSRAM
 *
\*****************************************************************/
int FIOTestFileChkSum( char *pcFileName )
{
    unsigned uReadSize = 0;
    //unsigned uCount;
    long lBlockSize = 0;
    long lBytesRead = 0;
    FILE * iFile;
//  int iFile;
    int iReturn;
    char *fpCh;
    struct stat filest;

    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
        return 1;

    iFile = fopen(pcFileName, "r");
    if( iFile == (FILE *)0 )
        return FAILURE;

    if(stat(pcFileName, &filest))
	return FAILURE;
    lBlockSize = filest.st_size - 2;

    if(lBlockSize <= 0)
	return FAILURE;

    uReadSize = 2048;           /* Reads by 2K block or less. */

    while( !(fpCh = (char *)malloc( uReadSize )) )
    {
        uReadSize /= 2;
        if( uReadSize < 64 )    /* Too small, return bad. */
            return 2;
    }
    if( (long)uReadSize > lBlockSize )
        uReadSize = (unsigned)lBlockSize;

    while( 1 )
    {
        iReturn = fread(fpCh, sizeof(char), uReadSize, iFile);
	if(  iReturn != (int)uReadSize )
        {
            fclose( iFile );
	    free( fpCh );
            return FAILURE;
        }
        lBytesRead += (long)uReadSize;      /* Increments bytes read. */
        if( lBytesRead >= lBlockSize )
            break;  /* Done. */

        /* Checks if next read will exceed file size. */
        if( lBytesRead+(long)uReadSize > lBlockSize )
        {
            /* If so, calculate next read bytes. */
            uReadSize = (unsigned)(lBlockSize - lBytesRead);
        }
     }

    fclose(iFile);
    free( fpCh );
   return( 0 );
}


/****************************************************************\
 *
 * Function:    FIONoBDrive
 *
 * Abstract:    Calls a DOS function to set the B drive (NVSRAM)
 *      parameters using the function return code to verify
 *      that there is NVSRAM installed.
 *
 * Parameters:  None
 *
 * Returns:     Value returned by DOS _bios_disk function.
 *              Status of the NVSRAM.
 *
 * Replaces:    no_bdrive
 * Used in:     main/init, test_NVSRAM
 *
\*****************************************************************/
int FIONoBDrive()
{
    int iRetVal;

    /* The only interesting status is the low byte. */
    return( iRetVal&0xFF );
}


/****************************************************************\
 *
 * Function:    FIOTestBlockChkSum
 *
 * Abstract:    Computes a checksum for the specified block
 *      and compares it against a test value.
 *
 * Parameters:
 *      fvStartAddr     (in) The address from which to start
 *                          computing the check sum.
 *      iBlockSize      (in) The number of blocks through which
 *                          the check sum computation is conducted.
 *      uChkSum         (in) The test value to compare against
 *
 * Returns:     0 = SUCCESS; not 0 = FAILURE
 *
\*****************************************************************/
int FIOTestBlockChkSum( void *fvStartAddr, int iBlockSize, unsigned uChkSum )
{
    return( uChkSum != FIOComputeChkSum(fvStartAddr, iBlockSize) );
}


/****************************************************************\
 *
 * Function:    FIOSetDefaultParam
 *
 * Abstract:    Initializes any parameter structure to a set of default values.
 *
 * Parameters:
 *      iFileDes    (in) The numerical designation for the parameter file
 *                      to initialize: ROBOTFILE, PREALIGNFILE, AUXFILE
 *
 * Returns:     None
 *
 * Replaces:    init_ctrl_par
 * Used in:     read_par
 *
\*****************************************************************/
void FIOSetDefaultParam(int iFileDes)
{
    stCtrlParam *psCtrlPar;     /* Temporary pointer to the parameter structure. */

    /* Get the pointer to the appropriate parameter structure,
     * and set the appropriate serial number. */
    if( iFileDes == ROBOTFILE )
    {
        psCtrlPar = &sRobotParam;
        strcpy( psCtrlPar->m_cSN, "ATM-100" );
    }
    else if( iFileDes == PREALIGNFILE )
    {
        psCtrlPar = &sPreParam;
        strcpy( psCtrlPar->m_cSN, "PRE-100" );
    }
    else
    {
        psCtrlPar = &sAuxParam;
        strcpy( psCtrlPar->m_cSN, "AUX-100" );
    }

    /* Set each field to the appropriate default value.
     * See the header file for a description of the field/member variable. */

    psCtrlPar->m_iBaudRate = 9600;

    psCtrlPar->m_laHomeSpeed[0] = 100;
    psCtrlPar->m_laHomeSpeed[1] = 50;
    psCtrlPar->m_laHomeSpeed[2] = 50;

    psCtrlPar->m_laHomeAccel[0] = 500;
    psCtrlPar->m_laHomeAccel[1] = 200;
    psCtrlPar->m_laHomeAccel[2] = 500;

    psCtrlPar->m_laEncdRes[0] = 1000;
    psCtrlPar->m_laEncdRes[1] = 1000;
    psCtrlPar->m_laEncdRes[2] = 2000;

    psCtrlPar->m_laOperaSpeed[0] = 2000;
    psCtrlPar->m_laOperaSpeed[1] = 2000;
    psCtrlPar->m_laOperaSpeed[2] = 2000;

    psCtrlPar->m_laOperaAccel[0] = psCtrlPar->m_laDC[0] = 4000;
    psCtrlPar->m_laOperaAccel[1] = psCtrlPar->m_laDC[1] = 4000;
    psCtrlPar->m_laOperaAccel[2] = psCtrlPar->m_laDC[2] = 4000;

    psCtrlPar->m_laER[0] = 200;
    psCtrlPar->m_laER[1] = 150;
    psCtrlPar->m_laER[2] = 200;

    /* Was GN */
    psCtrlPar->m_laKP[0] = 100000;
    psCtrlPar->m_laKP[1] = 100000;
    psCtrlPar->m_laKP[2] = 100000;

    psCtrlPar->m_laFA[0] = 0;
    psCtrlPar->m_laFA[1] = 0;
    psCtrlPar->m_laFA[2] = 0;

    psCtrlPar->m_laKI[0] = 1000;
    psCtrlPar->m_laKI[1] = 1000;
    psCtrlPar->m_laKI[2] = 1000;

    psCtrlPar->m_laTL[0] = 9999;
    psCtrlPar->m_laTL[1] = 9999;
    psCtrlPar->m_laTL[2] = 9999;

    /* Was ZR */
    psCtrlPar->m_laKD[0] = 200000;
    psCtrlPar->m_laKD[1] = 200000;
    psCtrlPar->m_laKD[2] = 200000;

    psCtrlPar->m_laIT[0] = 3000;
    psCtrlPar->m_laIT[1] = 6000;
    psCtrlPar->m_laIT[2] = 6000;

    psCtrlPar->m_laCHome[0] = psCtrlPar->m_laHomeOffset[0] = 0;
    psCtrlPar->m_laCHome[1] = psCtrlPar->m_laHomeOffset[1] = 0;
    psCtrlPar->m_laCHome[2] = psCtrlPar->m_laHomeOffset[2] = 0;

    psCtrlPar->m_laMR[0] = 33600L;
    psCtrlPar->m_laMR[1] = 42000L;
    psCtrlPar->m_laMR[2] = 3900;

    psCtrlPar->m_laARM[0] = 0;
    psCtrlPar->m_laARM[1] = 10500;
    psCtrlPar->m_laARM[2] = 0;

    psCtrlPar->m_laSoftNegLimit[0] = -1000;
    psCtrlPar->m_laSoftNegLimit[1] = -10500;
    psCtrlPar->m_laSoftNegLimit[2] = -1000;

    psCtrlPar->m_laSoftPosLimit[0] = 36000L;
    psCtrlPar->m_laSoftPosLimit[1] = 10500;
    psCtrlPar->m_laSoftPosLimit[2] = 7200;

    psCtrlPar->m_fRevision = 0.0;

    return;
}


/****************************************************************\
 *
 * Function:    FIOInitParamModule
 *
 * Abstract:    Initializes the datafile module. It first initializes
 *      the NVSRAM error handler, then checks that the NVSRAM exists
 *      and is recognized, and finally sets the parameter structures
 *      to default values. To change the parameter file structures
 *      to real values from files saved in the NVSRAM, FIOReadParam
 *      must be called.
 *
 * Parameters:  None
 *
 * Returns:     Status of the NVSRAM
 *
\*****************************************************************/
int FIOInitParamModule()
{
    /* Initialize parameters. */
    FIOSetDefaultParam( ROBOTFILE );         /* Initialize Robot Param to default values. */
    FIOSetDefaultParam( PREALIGNFILE );      /* Initialize aligner Param to default values. */
    FIOSetDefaultParam( AUXFILE );           /* Initialize Aux Param to default values. */

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOReadParam
 *
 * Abstract:    Reads communication, robot, and galil pars.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      the parameter structure is initialized to default values and
 *      a file FAILURE for the specified parameter file is registered
 *      in the STATus module. Otherwise, the file is opened and its
 *      contents read into the appropriate parameter structure. A
 *      check is also performed to make sure the entire file was
 *      read. If the file could not be opened or read in its entirety,
 *      the status module is again notified. There is special
 *      consideration for the Encoder Resolution of the robot parameter
 *      file.
 *
 * Parameters:
 *      iFileDes    (in) The numerical designation for the parameter file
 *                      to initialize: ROBOTFILE, PREALIGNFILE, AUXFILE
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    read_par
 * Used in:     main/init
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOReadParam(int iFileDes)
{
    unsigned uParamChkSum;  /* Parameter file check sum value. */
    FILE * iFP;
    int iTotalInt, iReturn;
    char caParFile[80];   /* Filename of the parameter file to open. */
    stCtrlParam *psCtrlPar; /* Temporary pointer to the parameter structure. */

    iReturn = SSGetCheckSumStatus( BAD_NVSRAM );
    /* Upon a NVSRAM failure... */
    if( iReturn )
    {
        /* ...initialize the parameter structure to default values... */
        FIOSetDefaultParam(iFileDes);
        /* ...and register a failure with the status module. */
        if ( iFileDes == ROBOTFILE )
            SSSetCheckSumStatus( PARA_FILE, TRUE );
        else if( iFileDes == PREALIGNFILE )
            SSSetCheckSumStatus( PARA_FILE1, TRUE );
	else
            SSSetCheckSumStatus( PARA_FILE2, TRUE );
        return FAILURE;
	}

    /* If all is well, pick up the structure of the appropriate parameter file. */
    if( iFileDes == ROBOTFILE )
	psCtrlPar = &sRobotParam;
    else if( iFileDes == PREALIGNFILE )
        psCtrlPar = &sPreParam;
    else
        psCtrlPar = &sAuxParam;
    /* Initialize the status module to read "no parameter file failures". */
    SSSetCheckSumStatus( iFileDes==ROBOTFILE?PARA_FILE:iFileDes==PREALIGNFILE?PARA_FILE1:PARA_FILE2, FALSE );

    /* Get the filename of the appropriate parameter file. */
    sprintf( caParFile, "%s.%d", PARFILENAME, iFileDes );

    /* Open the parameter file in NVSRAM. */
//    iFP = open( acParamFile, O_RDONLY|O_BINARY );
    iFP = fopen(caParFile, "r");
    if( iFP == (FILE *)0)
    {
        /* On an unsuccessful parameter file open... */
        perror( "Parameter Read Open Error " );
        /* ...initialize the parameter structure to default values... */
        FIOSetDefaultParam( iFileDes );
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( iFileDes==0?PARA_FILE:iFileDes==1?PARA_FILE1:PARA_FILE2, TRUE );
    }
    else
    {
        uParamChkSum = 0;
        /* Read the file directly into the parameter structure. */
//        read( iFP, psCtrlPar, sizeof( stCtrlParam ));
        fread(psCtrlPar, sizeof( stCtrlParam ), 1, iFP);
	/* Then get the check sum. */
//        iTotalInt = read( iFP, &uParamChkSum, sizeof ( unsigned ) );
        iTotalInt = fread(&uParamChkSum, sizeof(unsigned), 1, iFP);
	if(iFileDes == ROBOTFILE )
            uRobotChkSum = uParamChkSum;
        else if(iFileDes == PREALIGNFILE )
            uPreChkSum = uParamChkSum;
        else
            uAuxChkSum = uParamChkSum;
        /* If the check sum is no good... */
        if( (iTotalInt < 1) || //sizeof( unsigned )) ||
            FIOTestBlockChkSum( psCtrlPar, sizeof( stCtrlParam ), uParamChkSum ) )
        {
            /* ...initialize the parameter structure to default values... */
            FIOSetDefaultParam(iFileDes);
            /* ...and register a failure with the status module. */
            SSSetCheckSumStatus( iFileDes==0?PARA_FILE:iFileDes==1?PARA_FILE1:PARA_FILE2, TRUE );
        }
        else if( iFileDes == ROBOTFILE )    /* for robot only */
        {
            /* Set up the encoder resolution values correctly. */
            if( !psCtrlPar->m_laEncdRes[0] )
            {
                psCtrlPar->m_laEncdRes[0] = psCtrlPar->m_laEncdRes[1] = 1000;
                if( psCtrlPar->m_cSN[0] == 'V' )
					psCtrlPar->m_laEncdRes[2] = 1000;
                else
                    psCtrlPar->m_laEncdRes[2] = 2000;
            }
        }

    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }
    /* Check the status module in case a failure registered along the way. */
    if(iFileDes == ROBOTFILE)
    {
        if (SSGetCheckSumStatus(PARA_FILE))
            return FAILURE;
    }
    else if(iFileDes == PREALIGNFILE)
    {
        if (SSGetCheckSumStatus(PARA_FILE1))
            return FAILURE;
    }
    else
    {
        if (SSGetCheckSumStatus(PARA_FILE2))
            return FAILURE;
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOWriteParam
 *
 * Abstract:    Writes communication, robot, and galil pars.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      immediately return a FAILURE. Otherwise, the file is opened and
 *      the contents of the appropriate parameter structure are dumped
 *      to the file. A check is also performed to make sure the entire file was
 *      written. If the file could not be opened or written in its entirety,
 *      the status module is notified.
 *
 * Parameters:
 *      iFileDes    (in) The numerical designation for the parameter file
 *                      to initialize: ROBOTFILE, PREALIGNFILE, AUXFILE
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    write_par
 * Used in:     ex_WRIP, ex_SAV
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOWriteParam(int iFileDes)
{
    unsigned uParamChkSum;  /* Parameter file check sum value. */
    FILE *iFP;
    int iCount;
    char caParFile[80];   /* Filename of the parameter file to open. */
    stCtrlParam *psCtrlPar; /* Temporary pointer to the parameter structure. */

    /* Upon a NVSRAM failure, immediately return FAILURE. */
    if( SSGetCheckSumStatus( BAD_NVSRAM ))
        return FAILURE;

    /* If all is well, pick up the structure of the appropriate parameter file. */
    if( iFileDes == ROBOTFILE )
    {
        psCtrlPar = &sRobotParam;
        //Dos&Counters
        // Increase the WriteToNVSRAM counter for robot parameter file and update the log file
        FIOWriteToNVSRAMLogFile(INCREMENT_ROBPARFILECOUNTER);
    }
    else if( iFileDes == PREALIGNFILE )
    {
        //Dos&Counters
        // Increase the WriteToNVSRAM counter for prealigner parameter file and update the log file
        FIOWriteToNVSRAMLogFile(INCREMENT_PREPARFILECOUNTER);
        psCtrlPar = &sPreParam;
    }
    else
    {
        //Dos&Counters
        // Increase the WriteToNVSRAM counter for auxiliary parameter file and update the log file
        FIOWriteToNVSRAMLogFile(INCREMENT_AUXPARFILECOUNTER);
        psCtrlPar = &sAuxParam;
    }
    /* Initialize the status module to read "no parameter file failures". */
    SSSetCheckSumStatus( iFileDes==ROBOTFILE?PARA_FILE:iFileDes==PREALIGNFILE?PARA_FILE1:PARA_FILE2, FALSE );

    /* Get the filename of the appropriate parameter file. */
    sprintf( caParFile, "%s.%d", PARFILENAME, iFileDes );

    /* Open the parameter file in NVSRAM. */
//    iFP = open( acParamFile, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, S_IWRITE|S_IREAD );
    iFP = fopen(caParFile, "w");
    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful parameter file open... */
        perror( "Parameter Write Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( iFileDes==ROBOTFILE?PARA_FILE:iFileDes==PREALIGNFILE?PARA_FILE1:PARA_FILE2, TRUE );
    }
    else
    {
        /* Pre-compute the parameter file check sum. */
        uParamChkSum = FIOComputeChkSum( psCtrlPar, sizeof( stCtrlParam ) );
        if(iFileDes == ROBOTFILE )
            uRobotChkSum = uParamChkSum;
        else if(iFileDes == PREALIGNFILE )
            uPreChkSum = uParamChkSum;
        else
            uAuxChkSum = uParamChkSum;
        /* Write the parameter structure directly into the file. */
//        iCount = write( iFP, psCtrlPar, sizeof( stCtrlParam ) );
        iCount = fwrite(psCtrlPar, sizeof( stCtrlParam), 1, iFP);
   	/* Then write the check sum. */
//        iCount += write( iFP, &uParamChkSum, sizeof( unsigned ) );
        iCount += fwrite( &uParamChkSum, sizeof(unsigned), 1, iFP);
	/* Make sure everything was written in its entirety. */
        if( iCount != 2) //sizeof(stCtrlParam)+sizeof(unsigned) )
            /* Otherwise register a failure with the status module. */
            SSSetCheckSumStatus( iFileDes==ROBOTFILE?PARA_FILE:iFileDes==PREALIGNFILE?PARA_FILE1:PARA_FILE2, TRUE );
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }

    /* Check the status module in case a failure registered along the way. */
    if(iFileDes == ROBOTFILE )
    {
        if (SSGetCheckSumStatus(PARA_FILE))
            return FAILURE;
    }
    else if( iFileDes == PREALIGNFILE)
    {
        if (SSGetCheckSumStatus(PARA_FILE1))
            return FAILURE;
    }
    else
    {
        if (SSGetCheckSumStatus(PARA_FILE2))
            return FAILURE;
    }
    return SUCCESS;
}



/****************************************************************\
 *
 * Function:    FIOReadCoord
 *
 * Abstract:    Reads station coordinates from NVSRAM.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      the station coordinate structure is initialized to default values and
 *      a file FAILURE for the station coordinate file is registered
 *      in the STATus module. Otherwise, the file is opened and its
 *      contents read into the station coordinate structure. A
 *      check is also performed to make sure the entire file was
 *      read. If the file could not be opened or read in its entirety,
 *      the status module is again notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    read_coord
 * Used in:     main/init, ex_RESC
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOReadCoord()
{
    unsigned uStnChkSum;            /* Station file check sum value. */
    FILE *iFP;
    int iTotalInt;

    /* Upon a NVSRAM failure... */
    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
	{
        /* ...initialize the station coordinate structure to default values... */
        FIOInitCoordinate();
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( COOR_FILE, TRUE );
        return FAILURE;
	}

    /* Initialize the status module to read "no coordinate file failures". */
    SSSetCheckSumStatus( COOR_FILE, FALSE );

    /* Open the station coordinate file in NVSRAM. */
//    iFP = open( CORFILENAME, O_RDONLY|O_BINARY );
    iFP = fopen(CORFILENAME, "r");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful station coordinate file open... */
        perror( "Coordinate Read Open Error " );
        /* ...initialize the station coordinate structure to default values... */
        FIOInitCoordinate();
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( COOR_FILE, TRUE );
    }
	else
	{
		uStnChkSum = 0;
        /* Read the file directly into the station coordinate structure. */
//        read( iFP, &aStationData, FIO_MAX_NUM_STATIONS*sizeof( stStationData ) );
        iTotalInt = fread( &aStationData, sizeof( stStationData ), FIO_MAX_NUM_STATIONS, iFP);
	/* Get the file revision number */
//        iTotalInt = read( iFP, &gfCoordFileRevision, sizeof (float));
//        iTotalInt = fread(&gfCoordFileRevision, sizeof(float), 1, iFP);
	/* Then get the check sum. */
//        iTotalInt += read( iFP, &uStnChkSum, sizeof ( unsigned ) );
        iTotalInt += fread(&uStnChkSum, sizeof(unsigned), 1, iFP);
	/* If the check sum is no good... */
		if( (iTotalInt<53) || //sizeof(unsigned)+sizeof(float)) ||
            FIOTestBlockChkSum( &aStationData, FIO_MAX_NUM_STATIONS*sizeof( stStationData ), uStnChkSum ) )
		{
            /* ...initialize the station coordinate structure to default values... */
            FIOInitCoordinate();
            /* ...and register a failure with the status module. */
            SSSetCheckSumStatus( COOR_FILE, TRUE );
		}
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }

    /* Check the status module in case a failure registered along the way. */
    if ( SSGetCheckSumStatus( COOR_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOWriteCoord
 *
 * Abstract:    Writes station coordinates to NVSRAM.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      immediately return a FAILURE. Otherwise, the file is opened and
 *      the contents of the station coordinate structure are dumped
 *      to the file. A check is also performed to make sure the entire file was
 *      written. If the file could not be opened or written in its entirety,
 *      the status module is notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    write_coord
 * Used in:     ex_WRIS, ex_SAV
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOWriteCoord()
{
    unsigned uStnChkSum;            /* Station file check sum value. */
    FILE *iFP;
    int iCount;
    
    /* Upon a NVSRAM failure, immediately return FAILURE. */
    if( SSGetCheckSumStatus( BAD_NVSRAM ))
        return FAILURE;

    /* Initialize the status module to read "no coordinate file failures". */
    SSSetCheckSumStatus( COOR_FILE, FALSE);

    //Dos&Counters
    // Increase the WriteToNVSRAM counter for coordinate file and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_COORDFILECOUNTER);

    /* Open the station coordinate file in NVSRAM. */
//    iFP = open( CORFILENAME, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, S_IWRITE|S_IREAD );
    iFP = fopen( CORFILENAME, "w");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful station coordinate file open... */
        perror( "Coordinate Write Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( COOR_FILE, TRUE );
    }
    else
    {
        /* Pre-compute the station coordinate file check sum. */
        uStnChkSum = FIOComputeChkSum( &aStationData, FIO_MAX_NUM_STATIONS*sizeof( stStationData ) );
        /* Write the station coordinate structure directly into the file. */
//        iCount = write( iFP, &aStationData, FIO_MAX_NUM_STATIONS*sizeof( stStationData ) );
        iCount = fwrite(&aStationData, sizeof(stStationData), FIO_MAX_NUM_STATIONS, iFP);
        /* Then, write the file revision number */
//        iCount += fwrite(&gfCoordFileRevision, sizeof(float), 1, iFP);
	/* Then write the check sum. */
	iCount += fwrite(&uStnChkSum, sizeof(unsigned), 1, iFP);
//        iCount += write( iFP, &uStnChkSum, sizeof( unsigned ) );
//        iCount += write( iFP, &gfCoordFileRevision, sizeof( float ) );
        /* Make sure everything was written in its entirety. */
        if (iCount != FIO_MAX_NUM_STATIONS+1) //*sizeof(stStationData)+sizeof(unsigned)+sizeof(float) )
        {
            /* Otherwise register a failure with the status module. */
            SSSetCheckSumStatus( COOR_FILE, TRUE );
        }
        
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }

    /* Check the status module in case a failure registered along the way. */
    if ( SSGetCheckSumStatus( COOR_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOInitCoordinate
 *
 * Abstract:    Initializes the station coordinate structure
 *      to a set of default values for all array points, i.e.
 *      all stations.
 *
 * Parameters:  None
 *
 * Returns:     None
 *
\*****************************************************************/
void FIOInitCoordinate()
{
    stStationData *psStn;   /* Temporary pointer to the station coordinate
                             * structure for the current index/station. */
    int iIndex;             /* The index of the coordinate special item
                             * (CSI) being initialized. */
    long *plValue;          /* Pointer to the CSI starting index from
                             * which all the CSI's are indexed. */

    /* For all stations in the array. */
    for( psStn=aStationData; psStn<aStationData+FIO_MAX_NUM_STATIONS; psStn++ )
    {
        psStn->m_alCoord[0] = sRobotParam.m_laCHome[0];
        psStn->m_alCoord[1] = sRobotParam.m_laCHome[1];
        psStn->m_alCoord[2] = sRobotParam.m_laCHome[2];
        psStn->m_lFprCoord = 0;
        psStn->m_lSafeR = 0;
        psStn->m_lOffset = 35;
        psStn->m_lZIndex = 1;
        psStn->m_lStroke = 70;
        /* The coordinate special item initialization has been moved
         * to the end of this function. */
        psStn->m_alScanCoord[0] = sRobotParam.m_laCHome[0];
        psStn->m_alScanCoord[1] = sRobotParam.m_laCHome[1];
        psStn->m_alScanCoord[2] = sRobotParam.m_laCHome[2];
        /* Since m_lSearchRetract is not used anymore,
         * it doesn't have to be initialized. */
        psStn->m_lRHome = psStn->m_alScanCoord[1];
        psStn->m_lPitch = 2500;
        psStn->m_iEEindex = 0;

        /* Since the coordinate special items are all just long variables
         * listed in a row, a pointer is set to the start point and can
         * just be incremented to index through all the CSI's sequentially. */
        plValue = &(psStn->m_lStartAngle);
        plValue[0] = 50;       /* CSI #0 */
        plValue[1] = 250;      /* CSI #1 */
        plValue[2] = 30;       /* CSI #2 */
        plValue[3] = 75;       /* CSI #3 */
        plValue[4] = 30;       /* CSI #4 */
        plValue[5] = 70;       /* CSI #5 */
        plValue[6] = 0;        /* CSI #6 */
        plValue[7] = 50;       /* CSI #7 */
        plValue[8] = 233;      /* CSI #8 */
        plValue[9] = 0;        /* CSI #9 */

        /* Set the rest of the CSI's to 0. This is basically the lscsi array. */
        for( iIndex=10; iIndex<FIO_MAX_NUM_CSI; iIndex++ )
			plValue[iIndex] = 0;
    }
    gfCoordFileRevision = 0.0;
}


/****************************************************************\
 *
 * Function:    FIOTestNVSRAM
 *
 * Abstract:    Meat of the diagnostics mode test #1. It starts by
 *      checking that the NVSRAM exists. Then it opens and reads
 *      the appropriate parameter file into a temporary
 *      parameter structure, depending on the diagnostics mode,
 *      robot or pre-aligner. It then tests the check sum.
 *      This file open/read file/test check sum strategy is then used
 *      for the coordinate and macro files (for robot diagnostics mode)
 *      or the wafer and wafer calibration files (for pre-aligner
 *      diagnostics mode).
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 * Replaces:    test_NVSRAM
 * Used in:     step1_diag
 *
\*****************************************************************/
int FIOTestNVSRAM( int iDevModeArg )
{
    unsigned uStnChkSum;    /* The file check sum. */
    char *fpTemp;     /* The temporary file structure which data is read into. */
    char caParFile[15];     /* The parameter filename, par.0 = ROBOTFILE, par.1 = PREFILE. */
    FILE *iFP;
    int iRet;
    unsigned uCount;
	float gfCoordFileRevision;

    TTPrintsAt( 1, 1, "Test NVSRAM frmt.." );
    /* Check for NVSRAM existence. */
    if( FIONoBDrive( ) )
	{
        TTClearScreen();
        TTPrintsAt( 2, 1, "  NVSRAM NOT READY" );
        return FAILURE;
    }

    /* Pretend that we are formatting the NVSRAM. */
    TIDelay( 50 );
    TTPrintsAt( 2, 1, "NVSRAM formatted  " );
    /* Give them some time to read the "NVSRAM formatted" message. */
    TIDelay( 100 );

    /* Move on to the parameter file. */
    TTPrintsAt( 3, 1, "Opening Par File..." );
    /* Give them some time to read the "Opening Par File..." message. */
    TIDelay( 100 );

    /* Get the filename of the appropriate parameter file. */
    sprintf( caParFile, "%s.%d", PARFILENAME, iDevModeArg );

    /* Open the parameter file in NVSRAM. */
    iFP = fopen(caParFile, "r");
    if( iFP == (FILE *)0 )
    {
        TTPrintsAt( 3, 1, "Par File Not Open  " );
    }
    else
	{
        TTPrintsAt( 3, 1, "Par CheckSum:      " );
        /* Create a temporary structure for the parameter file to be read into. */
        if( !(fpTemp=(char *)malloc( sizeof( stCtrlParam ) )) )
		{
            TTPrintsAt( 4, 1,"***Out of Memory***" );
            return FAILURE;
		}
		uStnChkSum = 0;
        /* Read the file directly into the parameter structure. */
        uCount = fread(fpTemp, sizeof(stCtrlParam), 1, iFP);
	/* Then get the check sum. */
        uCount = fread(&uStnChkSum, sizeof(unsigned), 1, iFP);
		/* If the check sum is no good... */
		if( FIOTestBlockChkSum( fpTemp, sizeof( stCtrlParam ), uStnChkSum ) )
            TTPrintsAt( 3, 15, "BAD" );
		else
            TTPrintsAt( 3, 15, "OK" );
        /* Delete the temporary structure for the parameter file. */
		free( fpTemp );
    }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );

    /* For ROBOT files */
    if( !iDevModeArg )
	{
        /*** COORDINATE FILE Check ***/
        TTPrintsAt( 4, 1, "Opening Coor File.." );
        TIDelay( 1000 );

        /* Open the station coordinate file in NVSRAM. */
        iFP = fopen(CORFILENAME, "r");
	if( iFP == (FILE *)0)
        {
            TTPrintsAt( 4, 1, "Coor File Not Open " );
        }
		else
		{
            TTPrintsAt( 4, 1, "Coor CheckSum:     " );
            /* Create a temporary structure for the station coordinate file to be read into. */
            if( !(fpTemp=(char *)malloc( FIO_MAX_NUM_STATIONS*sizeof( stStationData ) )) )
	    	{
                TTPrintsAt( 4, 1,"\n\r***Out of Memory***" );
                return FAILURE;
	    	}
			uStnChkSum = 0;
            /* Read the file directly into the station coordinate structure. */
            uCount = fread(fpTemp, sizeof(stStationData), FIO_MAX_NUM_STATIONS, iFP);
        	uCount = fread(&gfCoordFileRevision, sizeof(float), 1, iFP);
	    	/* Then get the check sum. */
            uCount = fread(&uStnChkSum, sizeof(unsigned), 1, iFP);
	    	/* If the check sum is no good... */
            if( FIOTestBlockChkSum( fpTemp, FIO_MAX_NUM_STATIONS*sizeof( stStationData ), uStnChkSum ) )
                TTPrintsAt( 4, 15, "BAD" );
            else
                TTPrintsAt( 4, 15, "OK" );
            /* Delete the temporary structure for the station coordinate file. */
			free( fpTemp );
		}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
		fclose( iFP );

        /*** MACRO FILE Check ***/
        TTPrintsAt( 4, 1, "\n\rOpening Mac File..." );
        TIDelay( 100 );

        /* Get the check sum. */
        iRet = FIOTestFileChkSum( MACFILENAME );
        /* If the check sum is no good... */
        if( iRet == -1 )
        {
            TTPrintsAt( 4, 1, "Mac File Not Open  " );
        }
		else
		{
            TTPrintsAt( 4, 1, "Mac CheckSum:      ");
//            if( iFP )
            if( iRet )
                TTPrintsAt( 4, 15, "BAD" );
			else
                TTPrintsAt( 4, 15, "OK" );
		}
    }


    /* For PRE_ALIGNER files */
    else
	{
        /*** WAFER FILE Check ***/
        TTPrintsAt( 4, 1, "Opening Waf File.." );
        TIDelay( 100 );

        /* Open the wafer file in NVSRAM. */
        iFP = fopen(WAFFILENAME, "r");
		if( iFP == (FILE *)0)
        {
            TTPrintsAt( 4, 1, "Waf File Not Open" );
        }
		else
		{
            TTPrintsAt( 4, 1, "Waf CheckSum:    " );
            /* Create a temporary structure for the wafer file to be read into. */
            if( !(fpTemp=(char *)malloc( sizeof( stWaferParam ) )) )
			{
                TTPrintsAt( 4, 1,"\n\r***Out of Memory***" );
                return FAILURE;
			}
			uStnChkSum = 0;
            /* Read the file directly into the wafer structure. */
            uCount = fread( fpTemp, sizeof(stWaferParam), 1, iFP);
	    /* Then get the check sum. */
            uCount = fread(&uStnChkSum, sizeof(unsigned), 1, iFP);
	    /* If the check sum is no good... */
			if( FIOTestBlockChkSum( fpTemp, sizeof(stWaferParam), uStnChkSum ) )
                TTPrintsAt( 4, 15, "BAD  " );
			else
                TTPrintsAt( 4, 15, "OK   " );
            /* Delete the temporary structure for the wafer file. */
			free( fpTemp );
		}
        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
		fclose(iFP);
	

        /*** WAFER CALIBRATION FILE Check ***/
        TTPrintsAt( 4, 1, "\r\nOpening Cal File.." );
        TIDelay( 100 );

        /* Get the check sum. */
        iRet = FIOTestFileChkSum( CALFILENAME );
        /* If the check sum is no good... */
        if( iRet == -1 )
        {
            TTPrintsAt( 4,1, "Cal File Not Open " );
        }
		else
		{
            TTPrintsAt( 4, 1, "Cal CheckSum:     " );
            if( iFP )
                TTPrintsAt( 4, 15, "BAD  " );
			else
                TTPrintsAt( 4, 15, "OK   " );
		}
    }

    TTPrintsAt( 4, 1, "\n\r" );

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOInitConfig
 *
 * Abstract:    Initializes the configuration table structure to a set of default values.
 *
 * Parameters:  None
 *
 * Returns:     None
 *
\*****************************************************************/
void FIOInitConfig()
{
    int iIndex;

    /* Set each field to the appropriate default value.
     * See the header file for a description of the field/member variable.
     * This cannot be determined by checking the define flag because the
     * define flag hasn't been sent into this module yet. It gets set later
     * in FIOInitParamModule. */
#ifdef NOFP
    strcpy(sCfgTbl.m_caSysCfgString, "A");
#else
    strcpy(sCfgTbl.m_caSysCfgString, "IA");
#endif
    for (iIndex=0; iIndex<MAXFEATURECONFIG; iIndex++)
    {
        sCfgTbl.m_iaFeatureFlags[iIndex] = 0;
    }

    return;
}


/****************************************************************\
 *
 * Function:    FIOReadConfig
 *
 * Abstract:    Reads coonfiguration table
 *      It starts by checking the NVSRAM status. If it's bad,
 *      the configuration table structure is initialized to default values and
 *      a file FAILURE for the configuration table file is registered
 *      in the STATus module. Otherwise, the file is opened and its
 *      contents read into the configuration table structure. A
 *      check is also performed to make sure the entire file was
 *      read. If the file could not be opened or read in its entirety,
 *      the status module is again notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOReadConfig()
{
    unsigned uCfgChkSum;            /* Configuration table file check sum value. */
    FILE *iFP;
    int iTotalInt;

    /* Upon a NVSRAM failure... */
    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
    {
        /* ...initialize the configuration table structure to default values... */
        FIOInitConfig();
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( CFG_FILE, TRUE );
        return FAILURE;
	}

    /* Initialize the status module to read "no configuration table file failures". */
    SSSetCheckSumStatus( CFG_FILE, FALSE );

    /* Open the parameter file in NVSRAM. */
    iFP = fopen(CFGFILENAME, "r");
    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful configuration table file open... */
        perror( "Configuration Table Read Open Error " );
        /* ...initialize the configuration table structure to default values... */
        FIOInitConfig();
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( CFG_FILE, TRUE );
    }
    else
    {
        uCfgChkSum = 0;
        /* Read the file directly into the configuration table structure. */
        fread(&sCfgTbl, sizeof(stCfgTable), 1, iFP);
	/* Then get the check sum. */
        iTotalInt = fread(&uCfgChkSum, sizeof(unsigned), 1, iFP);
	/* If the check sum is no good... */
        if( (iTotalInt < 1) || //sizeof( unsigned )) ||
            FIOTestBlockChkSum( &sCfgTbl, sizeof( stCfgTable ), uCfgChkSum ) )
        {
            /* ...initialize the configuration table structure to default values... */
            FIOInitConfig();
            /* ...and register a failure with the status module. */
            SSSetCheckSumStatus( CFG_FILE, TRUE );
        }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
	}

    /* Check the status module in case a failure registered along the way. */
    if (SSGetCheckSumStatus(CFG_FILE))
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOWriteConfig
 *
 * Abstract:    Writes configuration table to NVSRAM.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      immediately return a FAILURE. Otherwise, the file is opened and
 *      the contents of the configuration table structure are dumped
 *      to the file. A check is also performed to make sure the entire file was
 *      written. If the file could not be opened or written in its entirety,
 *      the status module is notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOWriteConfig()
{
    unsigned uCfgChkSum;            /* Configuration table file check sum value. */
    FILE *iFP;
    int iCount;

    /* Upon a NVSRAM failure, immediately return FAILURE. */
    if( SSGetCheckSumStatus( BAD_NVSRAM ))
        return FAILURE;

    /* Initialize the status module to read "no configuration table file failures". */
    SSSetCheckSumStatus( CFG_FILE, FALSE);

    //Dos&Counters
    //Increase the WriteToNVSRAM counter for config file and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_CONFIGFILECOUNTER);

    /* Open the configuration table file in NVSRAM. */
    iFP = fopen( CFGFILENAME, "w");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful configuration table file open... */
        perror( "Configuration Table Write Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( CFG_FILE, TRUE );
    }
    else
    {
        /* Pre-compute the configuration table file check sum. */
        uCfgChkSum = FIOComputeChkSum( &sCfgTbl, sizeof( stCfgTable ) );
        /* Write the configuration table structure directly into the file. */
        iCount = fwrite(&sCfgTbl, sizeof(stCfgTable), 1, iFP);
	/* Then write the check sum. */
        iCount += fwrite(&uCfgChkSum, sizeof(unsigned), 1, iFP);
        /* Make sure everything was written in its entirety. */
        if (iCount != 2) //sizeof(stCfgTable)+sizeof(unsigned) )
        {
            /* Otherwise register a failure with the status module. */
            SSSetCheckSumStatus( CFG_FILE, TRUE );
        }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }

    /* Check the status module in case a failure registered along the way. */
    if ( SSGetCheckSumStatus( CFG_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOGetParamFileType
 *
 * Abstract:    Returns a pointer to the structure of the
 *      requested parameter file type, i.e. the argument
 *      ROBOTFILE would return a pointer to the robot parameter
 *      structure.
 *
 * Parameters:
 *      iParamDesc  (in) The numerical designation for the parameter structure
 *                      to retrieve: ROBOTFILE, PREALIGNFILE, AUXFILE
 *
 * Returns:     Pointer to the requested parameter structure
 *
\*****************************************************************/
pstCtrlParam FIOGetParamFileType( int iParamDesc )
{
    stCtrlParam *pTempCtrlPar;  /* Temporary pointer to parameter structure. */

    /* Get the pointer to the appropriate parameter structure. */
    switch( iParamDesc )
    {
        case ROBOTFILE:
             pTempCtrlPar = &sRobotParam;
             break;
        case PREALIGNFILE:
             pTempCtrlPar = &sPreParam;
             break;
        case AUXFILE:
             pTempCtrlPar = &sAuxParam;
             break;
    }
	return pTempCtrlPar;
}


/****************************************************************\
 *
 * Function:    FIOGetParam
 *
 * Abstract:    Copies values from the data buffer to the output
 *      buffer from index 0 to 2.
 *
 * Parameters:
 *      lOutBuffer      (in)
 *      laDataBuffer    (in)
 *
 * Returns:     Pointer to the requested parameter structure
 *
\*****************************************************************/
int FIOGetParam(long *lOutBuffer, long *laDataBuffer)
{
    int index;

    for (index=0; index<3; index++)
    {
        lOutBuffer[index] = laDataBuffer[index];
    }
	return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOSetParam
 *
 * Abstract:    Copies values from the output buffer to the data
 *      buffer from index 0 to 2.
 *
 * Parameters:
 *      laOutBuffer     (in)
 *      laDataBuffer    (in)
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int FIOSetParam(long *laOutBuffer, long *laDataBuffer)
{
    int index;

    for (index=0; index<3; index++)
    {
        laOutBuffer[index] = laDataBuffer[index];
    }
	return SUCCESS;
}


#ifdef NOFP
/* These stubs are required for non aligner builds. For aligner builds,
 * there are actual functions in the datafaln.c file. */
int FIOWriteCalibTable()
{
    return SUCCESS;
}
int FIOReadCalibTable()
{
    return SUCCESS;
}
int FIOWriteWafer()
{
    return SUCCESS;
}
int FIOReadWafer()
{
    return SUCCESS;
}
//for OTF, OTF algorithm needs to know size of the wafer eventhough the aligner code is not included
long lWaferSize = 200;

int FIOGetParamWaferVals(int iValueDescArg, long *plValueArg)
{
    if (iValueDescArg == WAFER_SIZE)
    {
        *plValueArg = lWaferSize;
        return SUCCESS;
    }
    return FAILURE;
}
int FIOSetParamWaferVals(int iValueDescArg, long lValueArg)
{
    if (iValueDescArg == WAFER_SIZE)
    {
        lWaferSize = lValueArg;
        return SUCCESS;
    }
    return FAILURE;
}

/* Functions to support File Revision */
int FIOGetCalibFileRevision(float *fRevisionArg)
{
    return SUCCESS;
}
int FIOSetCalibFileRevision(float fRevisionArg)
{
    return SUCCESS;
}
int FIOGetWaferFileRevision(float *fRevisionArg)
{
    return SUCCESS;
}
int FIOSetWaferFileRevision(float fRevisionArg)
{
    return SUCCESS;
}
#endif


/****************************************************************\
 *
 * Function:    FIOValidateParaDesc
 *
 * Abstract:    Checks that the numerical designation for the
 *      parameter file being accessed is either ROBOTFILE,
 *      PREALIGNFILE, or AUXFILE, the acceptable range
 *      of station coordinate indexes.
 *
 * Parameters:
 *      iFileDesc   (in) The numerical designation for the parameter file
 *                      to validate: ROBOTFILE, PREALIGNFILE, AUXFILE
 *
 * Returns:    SUCCESS or FAILURE
 *
\*****************************************************************/
int FIOValidateParaDesc(int iParaDesc)
{
    if( (iParaDesc != ROBOTFILE) &&
        (iParaDesc != PREALIGNFILE) &&
        (iParaDesc != AUXFILE) )
    {
        return FAILURE;
    }
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOValidateStationIndex
 *
 * Abstract:    Checks that the index of coordinate/station
 *      being accessed is between 0 and 51, the acceptable range
 *      of station coordinate indexes.
 *
 * Parameters:
 *      iStn        (in) Station index from 0 to 51 (A to Z and a to z)
 *
 * Returns:    SUCCESS or FAILURE
 *
\*****************************************************************/
int FIOValidateStationIndex(int iStn)
{
    if( (iStn<0) || (iStn>( FIO_MAX_NUM_STATIONS - 1 )) )
        return FAILURE;
    return SUCCESS;
}


/***** The following functions GET and SET variable values from the PARAMETER structure. *****/

int FIOGetParamSerialNum(int iParaDesc, char *pcSN)
{
    stCtrlParam* TempCtrlPar;
    int iResult;

    if (( iResult = FIOValidateParaDesc(iParaDesc)) == SUCCESS)
    {
        TempCtrlPar = FIOGetParamFileType(iParaDesc);
        strcpy(pcSN, TempCtrlPar->m_cSN);
    }
    return iResult;
}

int FIOSetParamSerialNum(int iParaDesc, char *pcSN)
{
    stCtrlParam* TempCtrlPar;
    int iResult;

    if (( iResult = FIOValidateParaDesc(iParaDesc)) == SUCCESS)
    {
        TempCtrlPar = FIOGetParamFileType(iParaDesc);
        strcpy(TempCtrlPar->m_cSN, pcSN);
    }
    return iResult;
}

int FIOGetParamBaudRate(int iParaDesc, int *piBaudRate)
{
    stCtrlParam* TempCtrlPar;
    int iResult;

    if (( iResult = FIOValidateParaDesc(iParaDesc)) == SUCCESS)
    {
        TempCtrlPar = FIOGetParamFileType(iParaDesc);
        *piBaudRate = TempCtrlPar->m_iBaudRate;
    }
    return iResult;
}

int FIOSetParamBaudRate( int iParaDesc, int iBaudRate)
{
    stCtrlParam* TempCtrlPar;
    int iResult;

    if (( iResult = FIOValidateParaDesc(iParaDesc)) == SUCCESS)
    {
        TempCtrlPar = FIOGetParamFileType(iParaDesc);
        TempCtrlPar->m_iBaudRate = iBaudRate ;
    }
    return iResult;
}

int FIOGetParamVals(int iParamFileArg, int iParamDescArg, long *plaValuesArg)
{
    stCtrlParam* pstTempCtrlPar;
    int iResult, i;

    if (FIOValidateParaDesc(iParamFileArg) == FAILURE)
        return FAILURE;

    pstTempCtrlPar = FIOGetParamFileType(iParamFileArg);

    switch (iParamDescArg)
    {
        case HOME_SPEED :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laHomeSpeed);
            break;
        case HOME_ACCEL :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laHomeAccel);
            break;
        case ENCODER_RESOLUTION :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laEncdRes);
            break;
        case LIMIT_SWITCH_ACCEL :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laLMAccel);
            break;
        case OPERATIONAL_SPEED :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laOperaSpeed);
            break;
        case OPERATIONAL_ACCEL :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laOperaAccel);
            break;
        case ERROR_LIMIT :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laER);
            break;
        case PROPORTIONAL_GAIN :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laKP);
            break;
        case FEED_FORWARD_ACCEL :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laFA);
            break;
        case INTEGRAL_GAIN :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laKI);
            break;
        case TORQUE_LIMIT :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laTL);
            break;
        case DERIVATIVE_GAIN :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laKD);
            break;
        case JERK :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laIT);
            break;
        case IT_SCURVE :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laIT);
            break;
        case CUSTOMIZED_HOME :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laCHome);
            break;
        case HOME_OFFSET :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laHomeOffset);
            break;
        case MECHANICAL_RATIO :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laMR);
            break;
        case ARM_LENGTH :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laARM);
            break;
        case H_VALUE :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laH);
            break;
        case OPERATIONAL_DECEL :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laDC);
            break;
        case SOFT_NEG_LIMIT :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laSoftNegLimit);
            break;
        case SOFT_POS_LIMIT :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laSoftPosLimit);
            break;
        case SCAN_OFFSET :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laScanOffset);
            break;
        case SPEED_LIMIT :
            iResult = FIOGetParam(plaValuesArg, pstTempCtrlPar->m_laSpeedLimit);
            break;
        default:
            return FAILURE;
    }

	if (iParamFileArg == 1)
		for (i=0; i<4; i++)
			plaValuesArg[i+4] = plaValuesArg[i];

    return iResult;
}

int FIOSetParamVals(int iParamFileArg, int iParamDescArg, long *plaValuesArg)
{
    stCtrlParam* pstTempCtrlPar;
    int iResult, i;

    if (FIOValidateParaDesc(iParamFileArg) == FAILURE)
        return FAILURE;

    pstTempCtrlPar = FIOGetParamFileType(iParamFileArg);

    if (iParamFileArg == 1 && giSysCfgNum != 30) // special case I2AXO W is in pre-file
	for (i=0; i<4; i++)
		plaValuesArg[i] = plaValuesArg[i+4];

    switch (iParamDescArg)
    {
        case HOME_SPEED :
            iResult = FIOSetParam(pstTempCtrlPar->m_laHomeSpeed, plaValuesArg);
            break;
        case HOME_ACCEL :
            iResult = FIOSetParam(pstTempCtrlPar->m_laHomeAccel, plaValuesArg);
            break;
        case ENCODER_RESOLUTION :
            iResult = FIOSetParam(pstTempCtrlPar->m_laEncdRes, plaValuesArg);
            break;
        case LIMIT_SWITCH_ACCEL :
            iResult = FIOSetParam(pstTempCtrlPar->m_laLMAccel, plaValuesArg);
            break;
        case OPERATIONAL_SPEED :
            iResult = FIOSetParam(pstTempCtrlPar->m_laOperaSpeed, plaValuesArg);
            break;
        case OPERATIONAL_ACCEL :
            iResult = FIOSetParam(pstTempCtrlPar->m_laOperaAccel, plaValuesArg);
            break;
        case ERROR_LIMIT :
            iResult = FIOSetParam(pstTempCtrlPar->m_laER, plaValuesArg);
            break;
        case PROPORTIONAL_GAIN :
            iResult = FIOSetParam(pstTempCtrlPar->m_laKP, plaValuesArg);
            break;
        case FEED_FORWARD_ACCEL :
            iResult = FIOSetParam(pstTempCtrlPar->m_laFA, plaValuesArg);
            break;
        case INTEGRAL_GAIN :
            iResult = FIOSetParam(pstTempCtrlPar->m_laKI, plaValuesArg);
            break;
        case TORQUE_LIMIT :
            iResult = FIOSetParam(pstTempCtrlPar->m_laTL, plaValuesArg);
            break;
        case DERIVATIVE_GAIN :
            iResult = FIOSetParam(pstTempCtrlPar->m_laKD, plaValuesArg);
            break;
        case JERK :
            iResult = FIOSetParam(pstTempCtrlPar->m_laIT, plaValuesArg);
            break;
        case IT_SCURVE :
            iResult = FIOSetParam(pstTempCtrlPar->m_laIT, plaValuesArg);
            break;
        case CUSTOMIZED_HOME :
            iResult = FIOSetParam(pstTempCtrlPar->m_laCHome, plaValuesArg);
            break;
        case HOME_OFFSET :
            iResult = FIOSetParam(pstTempCtrlPar->m_laHomeOffset, plaValuesArg);
            break;
        case MECHANICAL_RATIO :
            iResult = FIOSetParam(pstTempCtrlPar->m_laMR, plaValuesArg);
            break;
        case ARM_LENGTH :
            iResult = FIOSetParam(pstTempCtrlPar->m_laARM, plaValuesArg);
            break;
        case H_VALUE :
            iResult = FIOSetParam(pstTempCtrlPar->m_laH, plaValuesArg);
            break;
        case OPERATIONAL_DECEL :
            iResult = FIOSetParam(pstTempCtrlPar->m_laDC, plaValuesArg);
            break;
        case SOFT_NEG_LIMIT :
            iResult = FIOSetParam(pstTempCtrlPar->m_laSoftNegLimit, plaValuesArg);
            break;
        case SOFT_POS_LIMIT :
            iResult = FIOSetParam(pstTempCtrlPar->m_laSoftPosLimit, plaValuesArg);
            break;
        case SCAN_OFFSET :
            iResult = FIOSetParam(pstTempCtrlPar->m_laScanOffset, plaValuesArg);
            break;
        case SPEED_LIMIT :
            iResult = FIOSetParam(pstTempCtrlPar->m_laSpeedLimit, plaValuesArg);
            break;
        default:
            return FAILURE;
    }

    return iResult;
}

/* END of interface to PARAMETER structure. */


/***** The following functions GET and SET variable values from the STATION structure. *****/

int FIOGetCurrentStationInt()
{
	return iCurrentStation;
}

int FIOSetCurrentStationInt(int iStn)
{
    int iResult;
    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        iCurrentStation = iStn;
    }
    return iResult;
}

int FIOGetStnCoord(int iStn, int iIndex, long *plCoord)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
        *plCoord = pStation->m_alCoord[iIndex];
    }
	return iResult;
}

int FIOSetStnCoord(int iStn, int iIndex, long lCoord)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
        pStation->m_alCoord[iIndex] = lCoord;
    }
	return iResult;
}

int FIOGetStnlscsi(int iStn, int iIndex, long *plLscsi)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
        *plLscsi = pStation->m_alLscsi[iIndex];
    }
	return iResult;
}

int FIOSetStnlscsi(int iStn, int iIndex, long lLscsi)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
        pStation->m_alLscsi[iIndex] = lLscsi;
    }
	return iResult;
}

int FIOGetStnScanCoord(int iStn, int iIndex, long *plScanCoord)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
        *plScanCoord = pStation->m_alScanCoord[iIndex];
    }
	return iResult;
}

int FIOSetStnScanCoord(int iStn, int iIndex, long lScanCoord)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
        pStation->m_alScanCoord[iIndex] = lScanCoord;
		iResult = SUCCESS;
	}
	return iResult;
}

int FIOGetStnEEindex(int iStn, int *piEEindex)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
		*piEEindex = pStation->m_iEEindex;
    }
	return iResult;
}

int FIOSetStnEEindex(int iStn, int iEEindex)
{
	int iResult;
	pstStationData pStation;

    if ( (iResult = FIOValidateStationIndex(iStn)) == SUCCESS)
    {
        pStation = aStationData + iStn;
		pStation->m_iEEindex = iEEindex;
    }
	return iResult;
}

int FIOGetStnVals(int iStnArg, int iValueDescArg, long *plValueArg)
{
    stStationData* pstStation;

    if (FIOValidateStationIndex(iStnArg) == FAILURE)
        return FAILURE;
    pstStation = aStationData + iStnArg;

    switch (iValueDescArg)
    {
        case OFFSET :
            *plValueArg = pstStation->m_lOffset;
            break;
        case Z_INDEX :
            *plValueArg = pstStation->m_lZIndex;
            break;
        case STROKE :
            *plValueArg = pstStation->m_lStroke;
            break;
        case START_ANGLE :
            *plValueArg = pstStation->m_lStartAngle;
            break;
        case END_ANGLE :
            *plValueArg = pstStation->m_lEndAngle;
            break;
        case CROSS_SLOT_LOW :
            *plValueArg = pstStation->m_lCSLow;
            break;
        case CROSS_SLOT_HIGH :
            *plValueArg = pstStation->m_lCSHigh;
            break;
        case ACC_LOW :
            *plValueArg = pstStation->m_lAccLow;
            break;
        case ACC_HIGH :
            *plValueArg = pstStation->m_lAccHigh;
            break;
        case AVE_THICK :
            *plValueArg = pstStation->m_lAveThick;
            break;
        case TAUGHT_POS_TO_WAFER :
            *plValueArg = pstStation->m_lPosToWaf;
            break;
        case CROSS_SLOT_THRESH :
            *plValueArg = pstStation->m_lCSThresh;
            break;
        case TRACK_POS :
            *plValueArg = pstStation->m_lTrackPos;
            break;
        case RET_HOME :
            *plValueArg = pstStation->m_lRHome;
            break;
        case PITCH :
            *plValueArg = pstStation->m_lPitch;
            break;
        case FLP_COORD :
            *plValueArg = pstStation->m_lFprCoord;
            break;
        case FLP_SAFE_R :
            *plValueArg = pstStation->m_lSafeR;
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}

int FIOSetStnVals(int iStnArg, int iValueDescArg, long lValueArg)
{
    stStationData* pstStation;

    if (FIOValidateStationIndex(iStnArg) == FAILURE)
        return FAILURE;
    pstStation = aStationData + iStnArg;

    switch (iValueDescArg)
    {
        case OFFSET :
            pstStation->m_lOffset = lValueArg;
            break;
        case Z_INDEX :
            pstStation->m_lZIndex = lValueArg;
            break;
        case STROKE :
            pstStation->m_lStroke = lValueArg;
            break;
        case START_ANGLE :
            pstStation->m_lStartAngle = lValueArg;
            break;
        case END_ANGLE :
            pstStation->m_lEndAngle = lValueArg;
            break;
        case CROSS_SLOT_LOW :
            pstStation->m_lCSLow = lValueArg;
            break;
        case CROSS_SLOT_HIGH :
            pstStation->m_lCSHigh = lValueArg;
            break;
        case ACC_LOW :
            pstStation->m_lAccLow = lValueArg;
            break;
        case ACC_HIGH :
            pstStation->m_lAccHigh = lValueArg;
            break;
        case AVE_THICK :
            pstStation->m_lAveThick = lValueArg;
            break;
        case TAUGHT_POS_TO_WAFER :
            pstStation->m_lPosToWaf = lValueArg;
            break;
        case CROSS_SLOT_THRESH :
            pstStation->m_lCSThresh = lValueArg;
            break;
        case TRACK_POS :
            pstStation->m_lTrackPos = lValueArg;
            break;
        case RET_HOME :
            pstStation->m_lRHome = lValueArg;
            break;
        case PITCH :
            pstStation->m_lPitch = lValueArg;
            break;
        case FLP_COORD :
            pstStation->m_lFprCoord = lValueArg;
            break;
        case FLP_SAFE_R :
            pstStation->m_lSafeR = lValueArg;
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}

/* END of interface to STATION structure. */


/***** The following functions GET and SET variable values from the CONFIGURATION structure. *****/

int FIOGetCfgFeatureFlags(int iFeatureIndexArg, int *ipFeatureFlagsArg)
{
    if ((iFeatureIndexArg<0) || (iFeatureIndexArg>=MAXFEATURECONFIG))
        return FAILURE;

    *ipFeatureFlagsArg = sCfgTbl.m_iaFeatureFlags[iFeatureIndexArg];
    return SUCCESS;
}

unsigned FIOGetCfgFeatureInt( void )
{
    int iIndex;
    unsigned uCfgFeature = 0;

    for (iIndex = 0; iIndex < MAXFEATURECONFIG; iIndex++)
    {
        if (sCfgTbl.m_iaFeatureFlags[iIndex])
            uCfgFeature |= (unsigned) 1 << iIndex;
    }
    return uCfgFeature;
}

int FIOSetCfgFeatureFlags(int iFeatureIndexArg, int iFeatureFlagsArg)
{
    if ((iFeatureIndexArg<0) || (iFeatureIndexArg>=MAXFEATURECONFIG))
        return FAILURE;
    if ((iFeatureFlagsArg != 0) && (iFeatureFlagsArg != 1))
        return FAILURE;

    sCfgTbl.m_iaFeatureFlags[iFeatureIndexArg] = iFeatureFlagsArg;
    return SUCCESS;
}

int FIOGetCfgSysCfgString(char *cpSysCfgStringArg)
{
    strcpy(cpSysCfgStringArg, sCfgTbl.m_caSysCfgString);
    return SUCCESS;
}

int FIOSetCfgSysCfgString(char *cpSysCfgStringArg)
{
    strcpy(sCfgTbl.m_caSysCfgString, cpSysCfgStringArg);
    return SUCCESS;
}

/* END of interface to CONFIGURATION structure. */

/****************************************************************\
 *
 * Function:    FIOWriteOTFFile
 *
 * Abstract:    Saves the OTF station calibration structure into OTF calibration file in NVSRAM
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int FIOWriteOTFFile()
{
    psOTFStation psTempOTFStation;
    FILE *iFP;
    int iSizeToWrite;
    //unsigned uReturn;
    //int iCount;
    //char *cfDataPtr;

    psTempOTFStation = (stOTFStation *)malloc(sizeof(stOTFStation));

    /* Upon a NVSRAM failure, immediately return FAILURE. */
    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
    {
        free(psTempOTFStation);
        return FAILURE;
    }

    /* Initialize the status module to read "no OTF file failures". */
    SSSetCheckSumStatus( OTF_FILE, FALSE );

    //Dos&Counters
    //Increase the WriteToNVSRAM counter for OTF file and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_OTFFILECOUNTER);

    /* Open the wafer calibration table file in NVSRAM. */
    iFP = fopen(OTF_FILE_NAME, "w");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful OTF file open... */
        perror( "OTF File Write Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( OTF_FILE, TRUE );
    }
    else
	{
        /* Start at the top of the linked list. */
        psTempOTFStation = MPGetOTFHeadStationsPointer();
        /* Get the size of a single entry/structure in the wafer calibration table. */
        iSizeToWrite = sizeof( stOTFStation ) - sizeof( psOTFStation );
        while( psTempOTFStation )
        {
            /* Index through the entire linked list writing the contents
             * of each structure to the file. */
            if (FIOWriteBytes( iFP, iSizeToWrite, (void *)psTempOTFStation ) == FAILURE)
            {
                /* Register a failure with the status module. */
                SSSetCheckSumStatus( OTF_FILE, TRUE );
                break;
            }
            psTempOTFStation = psTempOTFStation->m_pstNextOTFStation;
        }
	}
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose(iFP);

    free(psTempOTFStation);

    /* Check the status module in case a failure registered along the way. */
    if( SSGetCheckSumStatus( OTF_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOReadOTFFile
 *
 * Abstract:    Loads the OTF station calibration file from NVSRAM to the internal structures
 *
 * Note:        This function allocates memory and doesn't free it, we are assuming that it is called
 *              only once, upon initialization. In case the function will get called more times or
 *              from command line, care has to be taken of the freeing the memory allocated for
 *              psTempOTFStation, and assigned to the OTF global pointers

 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int FIOReadOTFFile()
{
    psOTFStation psTempOTFStation, psTempOTFStation2, psTempOTFCurrentStation;
    FILE *iFP;
    int iTotalInt, iSizeToRead;

    if(!(psTempOTFStation2 = (psOTFStation)malloc(sizeof(stOTFStation))))
        return FAILURE;

    // Upon a NVSRAM failure...
    if( SSGetCheckSumStatus(BAD_NVSRAM ))
	{
        // ...register a failure with the status module.
        SSSetCheckSumStatus( OTF_FILE, TRUE );
        free(psTempOTFStation2);
        return FAILURE;
	}

    // Initialize the status module to read "no OTF file failures".
    SSSetCheckSumStatus( OTF_FILE, FALSE);

    // Open the OTF file in NVSRAM.
    iFP = fopen( OTF_FILE_NAME, "r");
    if( iFP == (FILE *)0 )
	{
        // On an unsuccessful OTF file open...
        perror( "OTF File Read Open Error " );
        // ...register a failure with the status module.
        SSSetCheckSumStatus( OTF_FILE, TRUE );
	}
	else
	{
        // Get the size of a single entry/structure in the wafer calibration table.
        iSizeToRead = sizeof( stOTFStation ) - sizeof( psOTFStation );
        MPSetOTFHeadStationsPointer(NULL);
        while( 1 )
        {
            // Read the OTF table entry directly into the structure.
            iTotalInt = fread(psTempOTFStation2, iSizeToRead, 1, iFP);
	    // If enough was read to fill a structure...
            if( iTotalInt >= 1) //iSizeToRead )
            {
                // ...allocate memory for another structure...
//                if( (psTempOTFStation = (psOTFStation)HPmalloc( sizeof( stOTFStation )) ) )
                if( (psTempOTFStation = (psOTFStation)malloc( sizeof( stOTFStation )) ) )
                {
                     //...and tack it on to the end of the linked list.
                    *psTempOTFStation = *psTempOTFStation2;
                    psTempOTFStation->m_pstNextOTFStation = NULL;
                    if( MPGetOTFHeadStationsPointer() )
                    {
                        memset(psTempOTFStation->m_lOTFFinalCorrection, 0xFF, (sizeof(long)*NOFSLOTS*3));
                        psTempOTFCurrentStation = MPGetOTFCurrentStationPointer();
                        psTempOTFCurrentStation->m_pstNextOTFStation = psTempOTFStation;
                        MPSetOTFCurrentStationPointer(psTempOTFStation);
                    }
                    else
                    {
                        memset(psTempOTFStation->m_lOTFFinalCorrection, 0xFF, (sizeof(long)*NOFSLOTS*3));
                        MPSetOTFHeadStationsPointer(psTempOTFStation);
                        MPSetOTFCurrentStationPointer(psTempOTFStation);
                   }
                }
                else
                {
#ifdef DEBUG
                    SERPutsTxBuff(SERGetCmdPort(), "***FIOReadOTFFile: out of memory.***\n\r?");
#endif
                    return FAILURE;
                }
            }
            else
            {
                // If enough was not read to fill a structure, the file has been
                // completely read. So we are done reading.
                break;
            }
        }
	}
    // CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors.
    fclose( iFP );

    free(psTempOTFStation2);

    // Check the status module in case a failure registered along the way.
    if ( SSGetCheckSumStatus( OTF_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOWriteBytes
 *
 * Abstract:    Writes bytes to file; accumulates checksum.  Note that the
 *      pointer argument is a far pointer so that we can write directly
 *      from the malloc'd data structures.
 *
 * Parameters:
 *      iFPArg          (in) File pointer to write to
 *      uNumBytesArg    (in) Number of bytes to write
 *      vfDataPtrArg    (in) Pointer to data to write
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    write_bytes
 * Used in:     internal function, locally used
 *
\*****************************************************************/
int FIOWriteBytes(FILE *iFPArg, int iNumBytesArg, void *vfDataPtrArg)
{
    int iCount;
    char *cfDataPtr;

    /* Make local copies of passed in arguments so we can modify them. */
    cfDataPtr = (char *)vfDataPtrArg;

    /* Write the passed in data array to the file. */
    iCount = fwrite(vfDataPtrArg, iNumBytesArg, 1, iFPArg);
    if (iCount != 1) //iNumBytesArg)
    {
#ifdef DEBUG
        printf("FIOWriteBytes: Bytes could not be written\n");
#endif
        return FAILURE;
    }

    return SUCCESS;
}


 /****************************************************************\
 *
 * Function:    FIOWriteToNVSRAMLogFile
 *
 * Abstract:    Function helps to track all writes to NVSRAM from within our firmware.
 *              It was created for debug purposes mainly. The log file in NVSRAM is maintained,
 *              with counters for every write operations. Upon initialization firmware checks
 *              for the file, if it's not there it will create one with counters set to 0.
 *              The counters can be cleared using low level command DOS "CLEAR". They can be
 *              read using low level command DOS "READ".
 *              Counters also get cleared when user issues FRMT 313 command to format NVSRAM
 *
 * Parameters:
 *              iTypeOfOperationArg - parameter which determines the type of operation the
 *              routine should execute, like read, count, clear...
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
//Dos&Counters
int FIOWriteToNVSRAMLogFile(int iTypeOfOperationArg)
{
    FILE *fp;
    int iCounter;
    char sBuf[50];
    /* array of counters for every write to NVSRAM for system files
     [0] = robot parameter file counter
     [1] = prealigner parameter file counter
     [2] = auxiliary parameter file counter
     [3] = coordinate file counter
     [4] = calibration files counter
     [5] = wafer file counter
     [6] = macro file counter
     [7] = SAV command use counter
     [8] = SMCR command  use counter
     [9] = WRIP command use counter
     [10] = WRIS command use counter
     [11] = WRIW command use counter
    */
    int aiSysFilesWriteCounter[NUMBER_OF_NVSRAM_COUNTERS] = {0,0,0,0,0,0,0,0,0,0,0,0};
    char *asText[NUMBER_OF_NVSRAM_COUNTERS] = {"Robot Parameter file","Aligner Parameter file",
                            "Aux Parameter file", "Coordinate file", "Calibration file", "Wafer file",
                            "Macro file","SAV command use", "SMCR command use", "WRIP command use",
                            "WRIS command use", "WRIW command use", "OTF file", "Configuration file"};

    fp = fopen("/root/controller/scmain/wcount.log", "r");
    if(fp != NULL)
    {
        //read the file counters into local structure
        for(iCounter = 0; iCounter < NUMBER_OF_NVSRAM_COUNTERS; iCounter++)
        {
            if(fscanf(fp, "%d\n", &aiSysFilesWriteCounter[iCounter]) == EOF)
            {
                //If read fails set the counters in file to 0
                iTypeOfOperationArg = CLEAR_FILECOUNTERS;
                break;
            }
        }
        fclose( fp );
    }
    //if file doesn't exist it will be created later
    else
    {
        iTypeOfOperationArg = CLEAR_FILECOUNTERS;
    }

    switch(iTypeOfOperationArg)
    {
        // prints counter's values with comments
        case  READ_FILECOUNTERS:
            for(iCounter = 0; iCounter < NUMBER_OF_NVSRAM_COUNTERS; iCounter++)
            {
                sprintf(sBuf, "%s counter = %d\r\n", asText[iCounter], aiSysFilesWriteCounter[iCounter]);
                SERPutsTxBuff( SERGetCmdPort(), sBuf );
            }
            return 0;

        // sets values of counters in NVSRAM and internal globals to 0
        case CLEAR_FILECOUNTERS:
            for(iCounter = 0; iCounter < NUMBER_OF_NVSRAM_COUNTERS; iCounter++)
                aiSysFilesWriteCounter[iCounter] = 0;
            break;
        // increase local counter, later it'll be written into the file
        case INCREMENT_ROBPARFILECOUNTER:
        case INCREMENT_PREPARFILECOUNTER:
        case INCREMENT_AUXPARFILECOUNTER:
        case INCREMENT_COORDFILECOUNTER:
        case INCREMENT_CALIBFILECOUNTER:
        case INCREMENT_WAFERFILECOUNTER:
        case INCREMENT_MACROFILECOUNTER:
        case INCREMENT_SAVCOMMANDCOUNTER:
        case INCREMENT_SMCRCOMMANDCOUNTER:
        case INCREMENT_WRIPCOMMANDCOUNTER:
        case INCREMENT_WRISCOMMANDCOUNTER:
        case INCREMENT_WRIWCOMMANDCOUNTER:
        case INCREMENT_OTFFILECOUNTER:
        case INCREMENT_CONFIGFILECOUNTER:
            aiSysFilesWriteCounter[iTypeOfOperationArg]++;
            break;
        // in case it was called from Initialization don't do anything, ( it's here for case we don't have
        // in NVSRAM and want to create one
        case INIT_FILECOUNTERS :
            break;
        default :
            return -1;
    }

    // open file for writing and write the updated counters from local structure
    fp = fopen("/root/controller/scmain/wcount.log", "w+");
    if(fp == NULL)
        return -1;

    for(iCounter = 0; iCounter < NUMBER_OF_NVSRAM_COUNTERS; iCounter++)
        fprintf(fp, "%d\n", aiSysFilesWriteCounter[iCounter]);

    fclose(fp);


    return SUCCESS;
}

 /****************************************************************\
 *
 * Function:    FIOSetFileRevision
 *
 * Abstract:    Sets the file revision number according to given file number
 *
 * Parameters:  lFileNum
 *                  n = 0 robot par, *.par
 *                      1 prealigner par, *.par
 *                      2 auxiliary par, *.par
 *                      3 coordinate file, *.cor
 *                      4 calibration file, *.cal
 *                      5 wafer parameter, *.waf
 *              sRevisionArg - string containing the revision number.
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int FIOSetFileRevision(long lFileNum, char *sRevisionArg)
{
    float fRevision;

    sscanf( sRevisionArg, "%f", &fRevision );

    switch (lFileNum)
    {
    case 0: /* Robot parameter file */
        sRobotParam.m_fRevision = fRevision;
        break;
    case 1: /* Pre parameter file */
        sPreParam.m_fRevision = fRevision;
        break;
    case 2: /* Aux parameter file */
        sAuxParam.m_fRevision = fRevision;
        break;
    case 3: /* Coordinate file */
        gfCoordFileRevision = fRevision;
        break;
    case 4: /* Calibration file */
        FIOSetCalibFileRevision(fRevision);
        break;
    case 5: /* Wafer parameter file */
        FIOSetWaferFileRevision(fRevision);
        break;
    }
    return SUCCESS;
}

 /****************************************************************\
 *
 * Function:    FIOGetFileRevision
 *
 * Abstract:    Gets the file revision number according to given file number
 *
 * Parameters:  lFileNum
 *                  n = 0 robot par, *.par
 *                      1 prealigner par, *.par
 *                      2 auxiliary par, *.par
 *                      3 coordinate file, *.cor
 *                      4 calibration file, *.cal
 *                      5 wafer parameter, *.waf
 *              sRevisionArg - string containing the revision number.
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int FIOGetFileRevision(long lFileNum, char *sRevisionArg)
{
    float fRevision;

    switch (lFileNum)
    {
    case 0: /* Robot parameter file */
        fRevision = sRobotParam.m_fRevision;
        break;
    case 1: /* Pre parameter file */
        fRevision = sPreParam.m_fRevision;
        break;
    case 2: /* Aux parameter file */
        fRevision = sAuxParam.m_fRevision;
        break;
    case 3: /* Coordinate file */
        fRevision = gfCoordFileRevision;
        break;
    case 4: /* Calibration file */
        FIOGetCalibFileRevision(&fRevision);
        break;
    case 5: /* Wafer parameter file */
        FIOGetWaferFileRevision(&fRevision);
        break;
    }
    sprintf( sRevisionArg, "%3.1f", fRevision);

    return SUCCESS;
}

// latch encoder pos calibration file
//int FIOReadLatchCalFile()
//{
//    FILE *iFP;
//    int iTotalInt;
//
//    iFP = fopen(LATCHCALNAME, "r");
//    if( iFP == (FILE *)0 )
//    {
//        perror( "Latch Cal Read Open Error " );
//    }
//    else
//    {
//       iTotalInt = fread( &aLatchCalData, sizeof( stLatchCalData ), 1, iFP);
//	if( iTotalInt<=0 )
//	{
//            perror( "Latch Cal Read Error " );
//	}
//        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
//        fclose( iFP );
//    }
//
//    return SUCCESS;
//}
//
//int FIWriteLatchCalFile()
//{
//    FILE *iFP;
//    int iCount;
//    
//    iFP = fopen( LATCHCALNAME, "w");
//    if( iFP == (FILE *)0 )
//    {
//        perror( "Latch Cal Write Open Error " );
//    }
//    else
//    {
//        iCount = fwrite(&aLatchCalData, sizeof(stLatchCalData), 1, iFP);
//        if (iCount <= 0) 
//        {
//            perror( "Latch Cal Write Error " );
//        }
//        
//        /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
//        fclose( iFP );
//    }
//
//    return SUCCESS;
//}

int FIOWriteVectorFile()
{
    FILE *iFP;
    int iCount;

    /* Open the wafer calibration table file in NVSRAM. */
    iFP = fopen(VECTORFNAME, "w");
    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful OTF file open... */
        perror( "Vector File Write Open Error " );
	return FAILURE;
    }
    else
    {
        iCount = fwrite(&sVectorParameter, sizeof(stVectorParameter), 1, iFP);
        if (iCount <= 0) 
        {
            perror( "Vector File Write Error " );
	    fclose(iFP);
	    return FAILURE;
        }
        
    }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose(iFP);

    return SUCCESS;
}

int FIOReadVectorFile()
{
    FILE *iFP;
    int iCount;

    /* Open the wafer calibration table file in NVSRAM. */
    iFP = fopen(VECTORFNAME, "r");
    if( iFP == (FILE *)0 )
    {
        perror( "Vector File Read Open Error " );
	return FAILURE;
    }
    else
    {
        iCount = fread(&sVectorParameter, sizeof(stVectorParameter), 1, iFP);
        if (iCount <= 0) 
        {
            perror( "Vector File Read Error " );
	    fclose(iFP);
	    return FAILURE;
        }
        
    }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose(iFP);

    return SUCCESS;
}

