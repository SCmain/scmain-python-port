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
 * File:        configgen.c
 *
 *
\***************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#include "fio.h"
#include "fiol.h"
#include "scttr.h"
#include "sck.h"
#include "scmem.h"

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
\*****************************************************************/
void FIOInitConfig()
{
    int iIndex;

    /* Set each field to the appropriate default value.
     * See the header file for a description of the field/member variable.
     * This cannot be determined by checking the define flag because the
     * define flag hasn't been sent into this module yet. It gets set later
     * in FIOInitParamModule. */

//    strcpy(sCfgTbl.m_caSysCfgString, "I3A1");
    strcpy(sCfgTbl.m_caSysCfgString, "I2AXO");

//    for (iIndex=0; iIndex<MAXFEATURECONFIG; iIndex++)
//    {
//        sCfgTbl.m_iaFeatureFlags[iIndex] = 1;
//    }
	sCfgTbl.m_iaFeatureFlags[SECS_HOST] = 1;
	sCfgTbl.m_iaFeatureFlags[SECS_DEVICE] = 0;
	sCfgTbl.m_iaFeatureFlags[TOT] = 1;
	sCfgTbl.m_iaFeatureFlags[OTF] = 1;
	sCfgTbl.m_iaFeatureFlags[MULTI_CHIP_ALIGN] = 0;
	sCfgTbl.m_iaFeatureFlags[COM3_SUPPORT] = 1;
	sCfgTbl.m_iaFeatureFlags[INTELLIPICK] = 0;
	sCfgTbl.m_iaFeatureFlags[SQUARE_WAFER_ALIGN] = 0;
	sCfgTbl.m_iaFeatureFlags[SCANNER] = 1;
	sCfgTbl.m_iaFeatureFlags[EQUIPE_BUS] = 0;
	sCfgTbl.m_iaFeatureFlags[FAST_ALIGN] = 0;
	sCfgTbl.m_iaFeatureFlags[SYSCFG] = 0;

    return;
}
/****************************************************************\
 *
 * Function:    FIOWriteConfig
 *
 * Returns:     SUCCESS or FAILURE
 *
 *
\*****************************************************************/
int FIOWriteConfig()
{
    unsigned uCfgChkSum;            /* Configuration table file check sum value. */
    FILE *iFP;
    int iCount;


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
            /* Otherwise register a failure with the status module. */
            //SSSetCheckSumStatus( CFG_FILE, TRUE );
        }
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }

    return SUCCESS;
}

main()
{
	FIOInitConfig();
	FIOWriteConfig();
}
