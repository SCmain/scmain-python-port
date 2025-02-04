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
 * File:        dfconfig.c
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
 *              DFNoBDrive
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>

#include "fio.h"
#include "fiol.h"
#include "fiog.h"

/* Configuration table structure for holding variable values pertaining to the configuration file. */
stCfgTable sCfgTbl;

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
    strcpy(sCfgTbl.m_caSysCfgString, "IA");
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
//    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
//    {
        /* ...initialize the configuration table structure to default values... */
//        FIOInitConfig();
        /* ...and register a failure with the status module. */
        //SSSetCheckSumStatus( CFG_FILE, TRUE );
//        return FAILURE;
//	}

    /* Initialize the status module to read "no configuration table file failures". */
//    SSSetCheckSumStatus( CFG_FILE, FALSE );

    /* Open the parameter file in NVSRAM. */
//    iFP = open( CFGFILENAME, O_RDONLY|O_BINARY );
    iFP = fopen(CFGFILENAME, "r");
    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful configuration table file open... */
        perror( "Configuration Table Read Open Error " );
        /* ...initialize the configuration table structure to default values... */
        FIOInitConfig();
        /* ...and register a failure with the status module. */
        //SSSetCheckSumStatus( CFG_FILE, TRUE );
    }
    else
    {
        uCfgChkSum = 0;
        /* Read the file directly into the configuration table structure. */
//        read( iFP, &sCfgTbl, sizeof( stCfgTable ));
        fread(&sCfgTbl, sizeof(stCfgTable), 1, iFP);
	/* Then get the check sum. */
//        iTotalInt = read( iFP, &uCfgChkSum, sizeof ( unsigned ) );
        iTotalInt = fread(&uCfgChkSum, sizeof(unsigned), 1, iFP);
	/* If the check sum is no good... */
        if (iTotalInt < 1) 
        {
            /* ...initialize the configuration table structure to default values... */
            FIOInitConfig();
            /* ...and register a failure with the status module. */
            //SSSetCheckSumStatus( CFG_FILE, TRUE );
	}
    }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );

    /* Check the status module in case a failure registered along the way. */
//    if (SSGetCheckSumStatus(CFG_FILE))
//        return FAILURE;
    return 0;
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
//    if( SSGetCheckSumStatus( BAD_NVSRAM ))
//        return FAILURE;

    /* Initialize the status module to read "no configuration table file failures". */
//    SSSetCheckSumStatus( CFG_FILE, FALSE);

    //Dos&Counters
    //Increase the WriteToNVSRAM counter for config file and update the log file
//    FIOWriteToNVSRAMLogFile(INCREMENT_CONFIGFILECOUNTER);

    /* Open the configuration table file in NVSRAM. */
//    iFP = open( CFGFILENAME, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, S_IWRITE|S_IREAD );
    iFP = fopen( CFGFILENAME, "w");
    if( iFP == (FILE *)0 )
    {
        /* On an unsuccessful configuration table file open... */
        perror( "Configuration Table Write Open Error " );
        /* ...register a failure with the status module. */
        //SSSetCheckSumStatus( CFG_FILE, TRUE );
    }
    else
    {
        /* Pre-compute the configuration table file check sum. */
        uCfgChkSum = FIOComputeChkSum( &sCfgTbl, sizeof( stCfgTable ) );
        /* Write the configuration table structure directly into the file. */
//        iCount = write( iFP, &sCfgTbl, sizeof( stCfgTable ) );
        iCount = fwrite(&sCfgTbl, sizeof(stCfgTable), 1, iFP);
	/* Then write the check sum. */
//        iCount += write( iFP, &uCfgChkSum, sizeof( unsigned ) );
        iCount += fwrite(&uCfgChkSum, sizeof(unsigned), 1, iFP);
        /* Make sure everything was written in its entirety. */
        if (iCount != 2) //sizeof(stCfgTable)+sizeof(unsigned) )
        {
		//printf("FIOWriteConfig fwrite iCount=%d\n", iCount);
            /* Otherwise register a failure with the status module. */
            //SSSetCheckSumStatus( CFG_FILE, TRUE );
        }
    }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );

    /* Check the status module in case a failure registered along the way. */
//    if ( SSGetCheckSumStatus( CFG_FILE ) )
//        return FAILURE;
    return 0;
}

main(int argc, char *argv[])
{
  int iIndex;
 
    strcpy(sCfgTbl.m_caSysCfgString, "I2AXO-TS");
    for (iIndex=0; iIndex<MAXFEATURECONFIG; iIndex++)
    {
        sCfgTbl.m_iaFeatureFlags[iIndex] = 0;
    }

    sCfgTbl.m_iaFeatureFlags[SECS_HOST] = 1;
    sCfgTbl.m_iaFeatureFlags[SECS_DEVICE] = 1;
    sCfgTbl.m_iaFeatureFlags[TOT] = 1;
    sCfgTbl.m_iaFeatureFlags[OTF] = 1;
    sCfgTbl.m_iaFeatureFlags[MULTI_CHIP_ALIGN] = 0;
    sCfgTbl.m_iaFeatureFlags[COM3_SUPPORT] = 0;
    sCfgTbl.m_iaFeatureFlags[INTELLIPICK] = 0;
    sCfgTbl.m_iaFeatureFlags[SQUARE_WAFER_ALIGN] = 0;
    sCfgTbl.m_iaFeatureFlags[SCANNER] = 1;
    sCfgTbl.m_iaFeatureFlags[EQUIPE_BUS] = 0;
    sCfgTbl.m_iaFeatureFlags[FAST_ALIGN] = 0;

    FIOWriteConfig();

    return;
}


