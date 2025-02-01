/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * Original Author      : (Deceased)
 * Current Maintainer   : gsancosme (georges@sancosme.net)
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
 * File:        fiol.c
 * Functions:   FIOInitWaferParam
 *              FIOReadWafer
 *              FIOWriteWafer
 *              FIOReadCalibTable
 *              FIOWriteCalibTable
 *              FIOSearchCalibTableNode
 *              FIOEnterCalibTableItem
 *              FIOGetCalibTblItem
 *              FIORemoveCTNode
 *              FIOGetCTNext
 *              FIOGetItemFromCalTbl
 *              FIOSetItemToCalTbl
 *              FIOGetCalibSerNum
 *              FIOSetCalibSerNum
 *
 * Description:  Provide an interface between the aligner datafile structures and
 *              all other modules.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/

#include <stdlib.h>
#include <fcntl.h>
//#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#include "sck.h"
#include "fio.h"
#include "scstat.h"
#include "alk.h"
#include "scmem.h"

/********** Variables Used Locally **********/
/* Calibration table variables */
long *fpItemPtr=NULL;     /* Wafer calibration node selected. */
psCalTable psDumpCalTbl=NULL;   /* Pointer to the current calibration node being dumped. */
psCalTable psHeadCalTbl=NULL;   /* Head pointer to the wafer calibration table linked list.
                                 * Marks the top of the linked list. */
psCalTable psTailCalTbl=NULL;   /* Tail pointer to the wafer calibration table linked list.
                                 * Marks the end of the linked list. */

/* Parameter file structures for holding variable values pertaining to the parameter files. */
stWaferParam sWaferParam;       /* Global parameter structure for the wafer parameter file */
unsigned uWaferChkSum;          /* Wafer parameter file check sum value. */
stRobotTROffset gstTROffset[2];
char caCalibSerNum[21];

float gfCalibFileRevision;      /* Calibration File revision number */
/****************************************************************\
 *
 * Function:    FIOInitWaferParam
 *
 * Abstract:    Initializes the wafer structure to a set of default values.
 *
 * Parameters:  None
 *
 * Returns:     None
 *
\*****************************************************************/
void FIOInitWaferParam()
{
    /* Each field initialization is eliminated, for code reduction.
     * Set the whole structure to 0's */
    memset( &sWaferParam, 0, sizeof(stWaferParam));
    /* Set the appropriate serial number. */
    strcpy( sWaferParam.m_cSN, "PRE-100" );
    return;
}


/****************************************************************\
 *
 * Function:    FIOReadWafer
 *
 * Abstract:    Reads wafer parameter structure values from NVSRAM file.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      the wafer structure is initialized to default values and
 *      a file FAILURE for the wafer file is registered
 *      in the STATus module. Otherwise, the file is opened and its
 *      contents read into the wafer structure. A
 *      check is also performed to make sure the entire file was
 *      read. If the file could not be opened or read in its entirety,
 *      the status module is again notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    read_wafer
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
int FIOReadWafer()
{
    FILE *iFP;
    int iTotalInt, iReturn;

    iReturn = SSGetCheckSumStatus( BAD_NVSRAM );
    /* Upon a NVSRAM failure... */
    if( iReturn )
	{
        /* ...initialize the wafer structure to default values... */
		FIOInitWaferParam();
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( WAFER_FILE, TRUE );
        return FAILURE;
    }

    /* Initialize the status module to read "no wafer file failures". */
    SSSetCheckSumStatus( WAFER_FILE, FALSE);

    /* Open the wafer file in NVSRAM. */
//    iFP = open( WAFFILENAME, O_RDONLY|O_BINARY );
    iFP = fopen( WAFFILENAME, "r");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful parameter file open... */
        perror( "Wafer Read Open Error " );
        /* ...initialize the wafer structure to default values... */
		FIOInitWaferParam( );
        /* ...and register a failure with the status module. */
        SSSetCheckSumStatus( WAFER_FILE, TRUE );
    }
    else
	{
		uWaferChkSum = 0;
        /* Read the file directly into the wafer structure. */
//        read( iFP, &sWaferParam, sizeof( stWaferParam ) );
        fread(&sWaferParam, sizeof( stWaferParam), 1, iFP);
	/* Then get the check sum. */
//        iTotalInt = read( iFP, &uWaferChkSum, sizeof ( unsigned ) );
        iTotalInt = fread(&uWaferChkSum, sizeof (unsigned), 1, iFP);
	/* If the check sum is no good... */
		if( (iTotalInt < 1) ||
            FIOTestBlockChkSum( &sWaferParam, sizeof( stWaferParam ), uWaferChkSum ) )
		{
            /* ...initialize the wafer structure to default values... */
			FIOInitWaferParam( );
            /* ...and register a failure with the status module. */
            SSSetCheckSumStatus( WAFER_FILE, TRUE );
		}
    	/* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    	fclose( iFP );
    	}

    /* Check the status module in case a failure registered along the way. */
    if (SSGetCheckSumStatus( WAFER_FILE ))
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOWriteWafer
 *
 * Abstract:    Write wafer parameter structure to NVSRAM.
 *      It starts by checking the NVSRAM status. If it's bad,
 *      immediately return a FAILURE. Otherwise, check that the
 *      pre-aligner file serial number and wafer file serial number
 *      match. If they don't, register a FAILURE with the status module
 *      and return. Then the file is opened and
 *      the contents of the wafer structure are dumped
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
 * Replaces:    write_wafer
 * Used in:     ex_WRIW, ex_SAV
 *
 * NOTE: The reason a FAILURE is not returned immediately on a file open
 *      or read failure is because we want to close the file and exit properly.
 *      To that end, a SUCCESS or FAILURE must be retrieved from the
 *      status module before it is returned. We could have used a
 *      variable to record the state, but checking the status module
 *      is just as easy.
 *
\*****************************************************************/
int FIOWriteWafer()
{
    FILE *iFP;
    int iCount;

    /* Upon a NVSRAM failure, immediately return FAILURE. */
    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
        return FAILURE;

    /* Initialize the status module to read "no wafer file failures". */
    SSSetCheckSumStatus( WAFER_FILE, FALSE );

    iCount = strlen( sPreParam.m_cSN );
    /* Check that the pre-aligner file serial number and wafer file serial number match. */
    if( strncmp( sWaferParam.m_cSN, sPreParam.m_cSN, iCount ) )
	{
        /* If they don't, register a FAILURE with the status module. */
        SSSetCheckSumStatus( WAFER_FILE, TRUE );
        return FAILURE;
    }

    //Dos&Counters
    // Increase the WriteToNVSRAM counter for wafer file and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_WAFERFILECOUNTER);

    /* Open the wafer file in NVSRAM. */
//    iFP = open( WAFFILENAME, O_CREAT|O_TRUNC|O_WRONLY|O_BINARY, S_IWRITE|S_IREAD );
    iFP = fopen( WAFFILENAME, "w");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful wafer file open... */
        perror( "Wafer Write Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( WAFER_FILE, TRUE );
    }
    else
	{
        /* Pre-compute the wafer file check sum. */
		uWaferChkSum = FIOComputeChkSum( &sWaferParam, sizeof( stWaferParam ) );
        /* Write the wafer structure directly into the file. */
//        iCount = write( iFP, &sWaferParam, sizeof( stWaferParam ) );
        iCount = fwrite(&sWaferParam, sizeof(stWaferParam), 1, iFP);
	/* Then write the check sum. */
//        iCount += write( iFP, &uWaferChkSum, sizeof( unsigned ) );
        iCount += fwrite(&uWaferChkSum, sizeof(unsigned), 1, iFP);
	/* Make sure everything was written in its entirety. */
        if ( iCount != 2) //sizeof(stWaferParam)+sizeof(unsigned) )
		{
            /* Otherwise register a failure with the status module. */
            SSSetCheckSumStatus( WAFER_FILE, TRUE );
		}
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
    }

    /* Check the status module in case a failure registered along the way. */
    if ( SSGetCheckSumStatus( WAFER_FILE ) )
        return FAILURE;
    return SUCCESS;
}




/****************************************************************\
 *
 * Function:    FIOReadCalibTable
 *
 * Abstract:    Reads the wafer calibration table file from NVSRAM
 *      into a linked list. It starts by checking the NVSRAM status.
 *      If it's bad, a file FAILURE for the wafer calibration table file
 *      is registered in the STATus module. Next, the check sum is
 *      tested. Then the file is opened and checked that the
 *      pre-aligner file serial number and wafer calibration table
 *      file serial number match. Finally, the contents of the
 *      file is read into a linked list for which structures are
 *      allocated as necessary. If the file could not be opened or
 *      read in its entirety, the status module is notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    read_caltbl
 * Used in:     main/init
 *
\*****************************************************************/
int FIOReadCalibTable()
{
    stCalTable stCalTblTemp;        /* Temporary wafer calibration table structure. */
    psCalTable psCalTblTemp;        /* Temporary pointer to wafer calibration table structure. */
    //char cSN[22];                   /* Wafer calibration file serial number. */
    FILE *iFP;
    int iTotalInt, iSizeToRead, iIndex;

    /* Upon a NVSRAM failure... */
    if( SSGetCheckSumStatus(BAD_NVSRAM ))
	{
        strcpy( caCalibSerNum, "PRE-100" );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( CALTBL_FILE, TRUE );
        return FAILURE;
	}

    /* Initialize the status module to read "no wafer calibration table file failures". */
    SSSetCheckSumStatus( CALTBL_FILE, FALSE);

    /* Open the wafer calibration table file in NVSRAM. */
//    iFP = open( CALFILENAME, O_RDONLY|O_BINARY );
    iFP = fopen( CALFILENAME, "r" );
    if( iFP == (FILE *)0 )
	{
        strcpy( caCalibSerNum, "PRE-100" );
        /* On an unsuccessful wafer calibration table file open... */
        perror( "Calibration Table Read Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( CALTBL_FILE, TRUE );
	}
	else
	{
//        read( iFP, caCalibSerNum, 21 );
        fread(caCalibSerNum, sizeof(char), 21, iFP);
	/* Check that the pre-aligner file serial number and the
         * wafer calibration file serial number match. */
        if( strcmp( caCalibSerNum, sPreParam.m_cSN ) )
		{
            /* If they don't, register a FAILURE with the status module. */
            SSSetCheckSumStatus( CALTBL_FILE, TRUE );
		}
        else
        {
            /* Read the file revision first */
//            iTotalInt = read( iFP, &gfCalibFileRevision, sizeof(float) );
            iTotalInt = fread(&gfCalibFileRevision, sizeof(float), 1, iFP);
  	    if(iTotalInt < 1) //sizeof(float))
	    {
                 SSSetCheckSumStatus( CALTBL_FILE, TRUE );
                 goto exit_point;
	    }
	    /* two structures which is 8 line of value in file for TR-offset value */
	    iSizeToRead = sizeof(stRobotTROffset);
            for(iIndex=0; iIndex<2; iIndex++)
            {
//            	iTotalInt = read( iFP, &gstTROffset[iIndex], iSizeToRead );
		iTotalInt = fread(&gstTROffset[iIndex], iSizeToRead, 1, iFP);
		if(iTotalInt < 1) //iSizeToRead)
		{
                    SSSetCheckSumStatus( CALTBL_FILE, TRUE );
                    goto exit_point;
		}
	    }

            /* Get the size of a single entry/structure in the wafer calibration table. */
            iSizeToRead = sizeof( stCalTable ) - sizeof( psCalTable );
            while( 1 )
            {
                /* Read the wafer calibration table entry directly into the structure. */
//               iTotalInt = read( iFP, &stCalTblTemp, iSizeToRead );
                iTotalInt = fread(&stCalTblTemp, iSizeToRead, 1, iFP);
		/* If enough was read to fill a structure... */
                if( iTotalInt >= 1) //iSizeToRead )
                {
                    /* ...allocate memory for another structure... */
//                    if( (psCalTblTemp = (psCalTable)HPmalloc( sizeof( stCalTable )) ) )
                    if( (psCalTblTemp = (psCalTable)malloc( sizeof( stCalTable )) ) )
                    {
                        /* ...and tack it on to the end of the linked list. */
                        *psCalTblTemp = stCalTblTemp;
                        psCalTblTemp->mNextPtr = NULL;
                        if( psHeadCalTbl )
                        {
                            psTailCalTbl->mNextPtr = psCalTblTemp;
                            psTailCalTbl = psCalTblTemp;
                        }
                        else
                        {
                            psHeadCalTbl = psTailCalTbl = psCalTblTemp;
                        }
                    }
                    else
                    {
#ifdef DEBUG
                        SERPutsTxBuff(SERGetCmdPort(), "***FIOReadCalibTable: out of memory.***\n\r?");
#endif
                        return FAILURE;
                    }
                }
                else
                {
                    /* If enough was not read to fill a structure, the file has been
                     * completely read. So we are done reading. */
                    break;
                }
            }
        }

exit_point:
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose( iFP );
	}

    /* Check the status module in case a failure registered along the way. */
    if ( SSGetCheckSumStatus( CALTBL_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOWriteCalibTable
 *
 * Abstract:    Write the wafer calibration table linked list to
 *      a file in NVSRAM. It starts by checking the NVSRAM status.
 *      If it's bad, a FAILURE is returned immediately.
 *      Otherwise, a file is opened and the contents of the
 *      wafer calibration table linked list are dumped to the
 *      file. A check is also performed to make sure the entire file was
 *      written. If the file could not be opened or written in its entirety,
 *      the status module is notified.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    write_caltbl
 * Used in:     ex_SAV
 *
\*****************************************************************/
int FIOWriteCalibTable()
{
    stCalTable stCalTblTemp;        /* Temporary wafer calibration table structure. */
    psCalTable psCalTblTemp;        /* Temporary pointer to wafer calibration table structure. */
    FILE *iFP;
    int iSizeToWrite, iIndex;
    //unsigned uReturn;

    /* Upon a NVSRAM failure, immediately return FAILURE. */
    if( SSGetCheckSumStatus( BAD_NVSRAM ) )
        return FAILURE;

    /* Initialize the status module to read "no wafer calibration table file failures". */
    SSSetCheckSumStatus( CALTBL_FILE, FALSE );

    iIndex = strlen( sPreParam.m_cSN );
    /* Check that the pre-aligner file serial number and calibration file serial number match. */
    if( strncmp( caCalibSerNum, sPreParam.m_cSN, iIndex ) )
	{
        /* If they don't, register a FAILURE with the status module. */
        SSSetCheckSumStatus( CALTBL_FILE, TRUE );
        return FAILURE;
    }

    //Dos&Counters
    // Increase the WriteToNVSRAM counter for calibration file and update the log file
    FIOWriteToNVSRAMLogFile(INCREMENT_CALIBFILECOUNTER);

    /* Open the wafer calibration table file in NVSRAM. */
    iFP = fopen(CALFILENAME, "w");
    if( iFP == (FILE *)0 )
	{
        /* On an unsuccessful wafer calibration table file open... */
        perror( "Calibration Table Read Open Error " );
        /* ...register a failure with the status module. */
        SSSetCheckSumStatus( CALTBL_FILE, TRUE );
    }
    else
	{
        /* Start at the top of the linked list. */
        psCalTblTemp = psHeadCalTbl;
        if (FIOWriteBytes( iFP, 21, (void *)caCalibSerNum ) == SUCCESS)
        {
            /* write the file revision */
            if (FIOWriteBytes( iFP, sizeof(float), (void *)&gfCalibFileRevision ) == FAILURE)
            {
                /* Register a failure with the status module. */
                SSSetCheckSumStatus( CALTBL_FILE, TRUE );
                goto exit_point;
            }
            /* write the 8 intellipick thresholds */
            iSizeToWrite = sizeof(stRobotTROffset);
	    for(iIndex=0; iIndex<2; iIndex++)
	    {
                if (FIOWriteBytes( iFP, iSizeToWrite, (void *)&gstTROffset[iIndex] ) == FAILURE)
                {
                    /* Register a failure with the status module. */
                    SSSetCheckSumStatus( CALTBL_FILE, TRUE );
                    goto exit_point;
                }
	    }

            /* Get the size of a single entry/structure in the wafer calibration table. */
            iSizeToWrite = sizeof( stCalTable ) - sizeof( psCalTable );
            while( psCalTblTemp )
            {
                /* Index through the entire linked list writing the contents
                 * of each structure to the file. */
                stCalTblTemp = *psCalTblTemp;
                psCalTblTemp = psCalTblTemp->mNextPtr;
                if (FIOWriteBytes( iFP, iSizeToWrite, (void *)&stCalTblTemp ) == FAILURE)
                {
                    /* Register a failure with the status module. */
                    SSSetCheckSumStatus( CALTBL_FILE, TRUE );
                    break;
                }
            }
        }
        else
        {
            /* Register a failure with the status module. */
            SSSetCheckSumStatus( CALTBL_FILE, TRUE );
        }

exit_point:
    /* CLOSE THE FILE IN NVSRAM!!! This is very important to prevent errors. */
    fclose(iFP);
    }

    /* Check the status module in case a failure registered along the way. */
    if( SSGetCheckSumStatus( CALTBL_FILE ) )
        return FAILURE;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOGetMatchSize
 *
 * Abstract:    Returns the corresponding wafer size in opposite units.
 *      That mean if the wafer size is in inches, it's corresponding size
 *      in millimeters is returned.
 *      i.e. 3=76mm, 4=100mm, 5=125mm, 6=150mm, 8=200mm.
 *
 * Parameters:
 *      lWaferSizeArg   (in) The wafer size entry to search for
 *      lMatchedSizeArg (out) The wafer size that is matched
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int FIOGetMatchSize(long lWaferSizeArg, long *lMatchedSizeArg)
{
    switch((int)lWaferSizeArg)
    {
        case AL_WAFER_SIZE_3 :
            *lMatchedSizeArg = AL_WAFER_SIZE_76;
            return SUCCESS;
        case AL_WAFER_SIZE_76 :
            *lMatchedSizeArg = AL_WAFER_SIZE_3;
            return SUCCESS;
        case AL_WAFER_SIZE_80 :
            *lMatchedSizeArg = AL_WAFER_SIZE_80;
            return SUCCESS;
        case AL_WAFER_SIZE_90 :
            *lMatchedSizeArg = AL_WAFER_SIZE_90;
            return SUCCESS;
        case AL_WAFER_SIZE_4 :
            *lMatchedSizeArg = AL_WAFER_SIZE_100;
            return SUCCESS;
        case AL_WAFER_SIZE_100 :
            *lMatchedSizeArg = AL_WAFER_SIZE_4;
            return SUCCESS;
        case AL_WAFER_SIZE_5 :
            *lMatchedSizeArg = AL_WAFER_SIZE_125;
            return SUCCESS;
        case AL_WAFER_SIZE_125 :
            *lMatchedSizeArg = AL_WAFER_SIZE_5;
            return SUCCESS;
        case AL_WAFER_SIZE_6 :
            *lMatchedSizeArg = AL_WAFER_SIZE_150;
            return SUCCESS;
        case AL_WAFER_SIZE_150 :
            *lMatchedSizeArg = AL_WAFER_SIZE_6;
            return SUCCESS;
        case AL_WAFER_SIZE_8 :
            *lMatchedSizeArg = AL_WAFER_SIZE_200;
            return SUCCESS;
        case AL_WAFER_SIZE_200 :
            *lMatchedSizeArg = AL_WAFER_SIZE_8;
            return SUCCESS;
        case AL_WAFER_SIZE_12 :
            *lMatchedSizeArg = AL_WAFER_SIZE_300;
            return SUCCESS;
        case AL_WAFER_SIZE_300 :
            *lMatchedSizeArg = AL_WAFER_SIZE_12;
            return SUCCESS;
    }
    return FAILURE;
}


/****************************************************************\
 *
 * Function:    FIOSearchCalibTableNode
 *
 * Abstract:    Searches the calibration table linked list for
 *      the entry with the matching wafer size and wafer type.
 *      If the user enters a wafer size number in an Equipe
 *      coded format, it switches to the internal storage format,
 *      i.e. 3=76mm, 4=100mm, 5=125mm, 6=150mm, 8=200mm.
 *
 * Parameters:
 *      lWaferSizeArg   (in) The wafer size entry to search for
 *      lTypeArg        (in) The wafer type entry to search for
 *
 * Returns:     Pointer to a wafer calibration table structure from the linked list
 *
 * Replaces:
 * Used in:     Internal function, locally used
 *
\*****************************************************************/
psCalTable FIOSearchCalibTableNode( long lWaferSizeArg, long lTypeArg )
{
	psCalTable psCalTblTemp;
    long lWaferSize;
    int iCnt;

    /* Set the temporary pointer to the head of the table so we can
     * index through it without affecting the head pointer. */
    psCalTblTemp = psHeadCalTbl;

    /* Convert the coded wafer size to the internal format or use
     * it directly if it is already in the correct form. */
    if (FIOGetMatchSize(lWaferSizeArg, &lWaferSize) == FAILURE)
        return NULL;

    /* Search the calibration table linked list for a matching entry. */
    while( psCalTblTemp )
	{
        if( (psCalTblTemp->m_lWaferSize == lWaferSizeArg || psCalTblTemp->m_lWaferSize == lWaferSize) &&
            (psCalTblTemp->m_lType == lTypeArg) )
		{
			fpItemPtr = psCalTblTemp->m_alItemArr;
			return psCalTblTemp;
		}
		psCalTblTemp = psCalTblTemp->mNextPtr;
    }

    /* Wafer size and type not in table. So create a new table entry for them.
     * Allocate memory for the entry... */
//    if( (psCalTblTemp = (psCalTable)HPmalloc( sizeof( stCalTable )) ) )
    if( (psCalTblTemp = (psCalTable)malloc( sizeof( stCalTable )) ) )
    {
        /* ...and tack it on to the end of the linked list. */
        if( !psHeadCalTbl )
        {
            psHeadCalTbl = psTailCalTbl = psCalTblTemp;
        }
        else
        {
            psTailCalTbl->mNextPtr = psCalTblTemp;
            psTailCalTbl = psCalTblTemp;
        }
        /* Set the entry information appropriately. */
        psCalTblTemp->m_lWaferSize = lWaferSizeArg;
        psCalTblTemp->m_lType = lTypeArg;
        for( iCnt=0; iCnt<MAXCTITEMS; iCnt++ )
            psCalTblTemp->m_alItemArr[iCnt] = 0L;
        psCalTblTemp->mNextPtr = NULL;
        fpItemPtr = psCalTblTemp->m_alItemArr;
        return psCalTblTemp;
    }
    else
    {
#ifdef DEBUG
        SERPutsTxBuff(SERGetCmdPort(), "****FIOSearchCalibTableNode: out of memory.***\n\r?");
#endif
        fpItemPtr = NULL;
        return NULL;
    }
    /* Unreachable code. */
    return NULL;
}


/****************************************************************\
 *
 * Function:    FIOEnterCalibTableItem
 *
 * Abstract:    Searches the calibration table linked list for
 *      the entry with the matching wafer size and wafer type.
 *      Then set the data of the item number requested to
 *      that entry. If a matching entry couldn't be found,
 *      create a new entry, set the data appropriately, and add
 *      it to the existing table.
 *
 * Parameters:
 *      lWaferSizeArg   (in) The wafer size entry to affect
 *      lTypeArg        (in) The wafer type entry to affect
 *      lItemNoArg      (in) The item number to change
 *      lDataArg        (in) The value to change the item number to
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    enter_ctitem
 * Used in:     ex_SCT
 *
\*****************************************************************/
int FIOEnterCalibTableItem( long lWaferSizeArg, long lTypeArg, long lItemNoArg, long lDataArg )
{
	psCalTable psCalTblTemp;

    /* Search the calibration table linked for an entry matching the wafer size and type. */
    if( (psCalTblTemp = FIOSearchCalibTableNode( lWaferSizeArg, lTypeArg )) )
        /* If a match is found, change the item number to the new value. */
        psCalTblTemp->m_alItemArr[lItemNoArg] = lDataArg;
    else
        return FAILURE;

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOGetCalibTblItem
 *
 * Abstract:    Searches the calibration table linked list for
 *      the entry with the matching wafer size and wafer type.
 *      Then return the data of the item number requested from
 *      that entry.
 *
 * Parameters:
 *      lWaferSizeArg   (in) The wafer size entry to search for
 *      lTypeArg        (in) The wafer type entry to search for
 *      lItemNoArg      (in) The item number to read back
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    get_ctitem
 * Used in:     ex_RCT
 *
\*****************************************************************/
long FIOGetCalibTblItem( long lWaferSizeArg, long lTypeArg, long lItemNoArg )
{
	psCalTable psCalTblTemp;

    /* Search the calibration table linked for an entry matching the wafer size and type. */
    if( (psCalTblTemp = FIOSearchCalibTableNode( lWaferSizeArg, lTypeArg )) )
        /* If a match is found, return the item number value. */
        return( psCalTblTemp->m_alItemArr[lItemNoArg] );
    else
        return FAILURE;
}


/****************************************************************\
 *
 * Function:    FIORemoveCTNode
 *
 * Abstract:    Deletes the entry from the calibration table
 *      linked list matching the wafer size and type. It does its
 *      own search because it needs to remember the previous entry
 *      pointer. So it can't use the FIOSearchCalibTableNode function. If a
 *      matching entry is not found, FAILURE is returned. Otherwise,
 *      the pointers are redirected, effectively eliminating the
 *      entry. Then the memory for it is freed.
 *
 * Parameters:
 *      lWaferSizeArg   (in) The wafer size entry to search for
 *      lTypeArg        (in) The wafer type entry to search for
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    remove_ctnode
 * Used in:     ex_CCT
 *
\*****************************************************************/
int FIORemoveCTNode( long lWaferSizeArg, long lTypeArg )
{
    psCalTable psCurrentTbl;
    psCalTable psNextTbl;
    psCalTable psPrevTbl;

    /* Initialize variables. */
    psPrevTbl = NULL;
    psCurrentTbl = psHeadCalTbl;
    psNextTbl = psCurrentTbl->mNextPtr;

    /* Search the calibration table linked list for the entry
     * matching wafer size and type. */
    while( psCurrentTbl )
	{
		if( psCurrentTbl->m_lWaferSize == lWaferSizeArg && psCurrentTbl->m_lType == lTypeArg )
			break;
        /* If the current node doesn't match, change the pointers
         * and check the next entry. */
		psPrevTbl = psCurrentTbl;
		psCurrentTbl = psCurrentTbl->mNextPtr;
		psNextTbl = psCurrentTbl->mNextPtr;
    }

    /* If a matching entry couldn't be found, return a FAILURE. */
    if( !psCurrentTbl )
        return FAILURE;

    /* Otherwise, redirect the pointers to eliminate the node. */
    if( psCurrentTbl == psHeadCalTbl )
        psHeadCalTbl = psCurrentTbl->mNextPtr;
    if( psCurrentTbl == psTailCalTbl )
        psTailCalTbl = psPrevTbl;
    if( psPrevTbl )
        psPrevTbl->mNextPtr = psNextTbl;

    /* Then free its memory. */
//    HPfree( psCurrentTbl );
    free( psCurrentTbl );

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOGetCTNext
 *
 * Abstract:
 *
 * Parameters:
 *      lWaferSizeArg   (out) The wafer size of the entry
 *      lTypeArg        (out) The wafer type of the entry
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    get_ctnext
 * Used in:     ex_DUMPC
 *
\*****************************************************************/
int FIOGetCTNext( long *lWaferSizeArg, long *lTypeArg )
{
    /* If no calibration table exists, there is nothing to dump. */
    if( psHeadCalTbl == NULL )
        return FAILURE;

    /* If the "dump" pointer is NULL, it is an initial value that
     * indicates the dump must begin. */
    if( psDumpCalTbl == NULL )
    {
        psDumpCalTbl = psHeadCalTbl;
        *lWaferSizeArg = psDumpCalTbl->m_lWaferSize;
        *lTypeArg = psDumpCalTbl->m_lType;
        return SUCCESS;
    }

    /* After a dump has begun, the next node must be retrieved. */
    if( psDumpCalTbl )
        psDumpCalTbl = psDumpCalTbl->mNextPtr;

    /* When the end of the linked list is reached, the dump is complete. */
    if( psDumpCalTbl == NULL )
        return FAILURE;
    /* If the end of the linked list has not yet been reached,
     * return the data from the current calibration table entry. */
    else
    {
        *lWaferSizeArg = psDumpCalTbl->m_lWaferSize;
        *lTypeArg = psDumpCalTbl->m_lType;
    }

    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    FIOGetItemFromCalTbl
 *
 * Abstract:    Returns the data value contained in the currently
 *      selected calibration table entry. The currently selected
 *      calibration table entry is actually a pointer to the
 *      structure's data array. It is set in FIOSearchCalibTableNode.
 *
 * Parameters:
 *      iItemNoArg      (in) The array number to get data from
 *
 * Returns:     Item from wafer calibration table or FAILURE
 *
 * Replaces:    get_ct_it
 * Used in:     set_wafer_size
 *
\*****************************************************************/
long FIOGetItemFromCalTbl( int iItemNoArg )
{
	if( !fpItemPtr )
        return FAILURE;
    return( fpItemPtr[iItemNoArg] );
}


/****************************************************************\
 *
 * Function:    FIOSetItemToCalTbl
 *
 * Abstract:    Sets the data value contained in the currently
 *      selected calibration table entry. The currently selected
 *      calibration table entry is actually a pointer to the
 *      structure's data array. It is set in FIOSearchCalibTableNode.
 *
 * Parameters:
 *      iItemNoArg      (in) The array number to set data to
 *      lItemValue      (in) The new value to set
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    set_ct_it
 * Used in:     CalibrateCCD
 *
\*****************************************************************/
int FIOSetItemToCalTbl( int iItemNoArg, long lItemValue )
{
    if( !fpItemPtr )
        return FAILURE;
    fpItemPtr[iItemNoArg] = lItemValue;
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOGetCalibSerNum
 * Abstract:    Gets the Wafer calibration file serial number
 * Parameters:  pcaCalibSerNumArg - Wafer calibration file serial number
 * Returns:     SUCCESS
 ******************************************************************************/
int FIOGetCalibSerNum(char *pcaCalibSerNumArg)
{
    strncpy( pcaCalibSerNumArg, caCalibSerNum, 20 );
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOSetCalibSerNum
 * Abstract:    Sets the Wafer calibration file serial number
 * Parameters:  pcaCalibSerNumArg - Wafer calibration file serial number
 * Returns:     SUCCESS
 ******************************************************************************/
int FIOSetCalibSerNum(char *pcaCalibSerNumArg)
{
    strncpy( caCalibSerNum, pcaCalibSerNumArg, 20 );
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOGetParamWaferSerialNum
 * Abstract:    Gets the Wafer parameter file serial number
 * Parameters:  pcaWaferSerialNum - Wafer parameter file serial number
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.SN[21], as a global variable
 ******************************************************************************/
int FIOGetParamWaferSerialNum(char *pcaWaferSerialNum)
{
    strcpy( pcaWaferSerialNum, sWaferParam.m_cSN );
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOSetParamWaferSerialNum
 * Abstract:    Sets the Wafer parameter file serial number
 * Parameters:  pcaWaferSerialNum - Wafer parameter file serial number
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.SN[21], as a global variable
 ******************************************************************************/
int FIOSetParamWaferSerialNum(char *pcaWaferSerialNum)
{
    strcpy( sWaferParam.m_cSN, pcaWaferSerialNum );
    return SUCCESS;
}


int FIOGetParamWaferVals(int iValueDescArg, long *plValueArg)
{
    switch (iValueDescArg)
    {
        case MEASURE_SPEED_CENTER :
            *plValueArg = sWaferParam.m_lMeasureSpeedCount;
            break;
        case MEASURE_ACCEL_CENTER :
            *plValueArg = sWaferParam.m_lMeasureAccelCount;
            break;
        case MEASURE_SPEED_FLAT :
            *plValueArg = sWaferParam.m_lMeasureSpeedFlt;
            break;
        case MEASURE_ACCEL_FLAT :
            *plValueArg = sWaferParam.m_lMeasureAccelFlt;
            break;
        case CHUCK_DEAD_BAND :
            *plValueArg = sWaferParam.m_lChuckDeadBand;
            break;
        case MEASURE_SPEED_CNTRF200 :
            *plValueArg = sWaferParam.m_lMeasureSpeedCntrF200;
            break;
        case MEASURE_SPEED_CNTRF300 :
            *plValueArg = sWaferParam.m_lMeasureSpeedCntrF300;
            break;
        case REVERSE_MODE :
            *plValueArg = sWaferParam.m_lReverseMode;
            break;
        case FLAT_ANGLE :
            *plValueArg = sWaferParam.m_lFlatAngle;
            break;
        case MEASURE_TYPE :
            *plValueArg = sWaferParam.m_lMeasureType;
            break;
        case WAFER_TYPE :
            *plValueArg = sWaferParam.m_lWaferType;
            break;
        case WAFER_LOAD_POSITION :
            *plValueArg = sWaferParam.m_lWaferLoadPosition;
            break;
        case WAFER_LIFT_UP_POSITION :
            *plValueArg = sWaferParam.m_lWaferLiftUpPosition;
            break;
        case MEASURE_SPEED_FLAT_F200 :
            *plValueArg = sWaferParam.m_lMeasureSpeedFlatF200;
            break;
        case MEASURE_SPEED_FLAT_F300 :
            *plValueArg = sWaferParam.m_lMeasureSpeedFlatF300;
            break;
        case DUMMY2A :
        case DUMMY2B :
        case DUMMY2C :
        case DUMMY2D :
        case DUMMY2E :
        case DUMMY2F :
        case DUMMY2G :
        case DUMMY2H :
        case DUMMY2I :
            *plValueArg = sWaferParam.m_lDummy2[iValueDescArg-DUMMY2A];
            break;
        case CCD1_FIRST_PIXEL :
            *plValueArg = sWaferParam.m_lCCD1FirstPixel;
            break;
        case CCD1_ANGLE :
            *plValueArg = sWaferParam.m_lCCD1Angle;
            break;
        case CHUCK_VAC_USE :
            *plValueArg = sWaferParam.m_lChuckVacUse;
            break;
        case CCD2_FIRST_PIXEL :
            *plValueArg = sWaferParam.m_lCCD2FirstPixel;
            break;
        case CCD2_ANGLE :
            *plValueArg = sWaferParam.m_lCCD2Angle;
            break;
		case DUMMY4A:
		case DUMMY4B:
		case DUMMY4C:
		case DUMMY4D:
            *plValueArg = sWaferParam.m_lDummy4[iValueDescArg-DUMMY4A];
            break;
        case WAFER_SIZE :
            *plValueArg = sWaferParam.m_lWaferSize;
            break;
        case FLAT_TYPE :
            *plValueArg = sWaferParam.m_lFlatType;
            break;
        case WAFER_CAL_TYPE :
            *plValueArg = sWaferParam.m_lWaferCalType;
            break;
        case DUMMY5A :
        case DUMMY5B :
        case DUMMY5C :
        case DUMMY5D :
        case DUMMY5E :
        case DUMMY5F :
        case DUMMY5G :
            *plValueArg = sWaferParam.m_lDummy5[iValueDescArg-DUMMY5A];
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}


int FIOSetParamWaferVals(int iValueDescArg, long lValueArg)
{
    switch (iValueDescArg)
    {
        case MEASURE_SPEED_CENTER :
            sWaferParam.m_lMeasureSpeedCount = lValueArg;
            break;
        case MEASURE_ACCEL_CENTER :
            sWaferParam.m_lMeasureAccelCount = lValueArg;
            break;
        case MEASURE_SPEED_FLAT :
            sWaferParam.m_lMeasureSpeedFlt = lValueArg;
            break;
        case MEASURE_ACCEL_FLAT :
            sWaferParam.m_lMeasureAccelFlt = lValueArg;
            break;
        case CHUCK_DEAD_BAND :
            sWaferParam.m_lChuckDeadBand = lValueArg;
            break;
        case MEASURE_SPEED_CNTRF200 :
            sWaferParam.m_lMeasureSpeedCntrF200 = lValueArg;
            break;
        case MEASURE_SPEED_CNTRF300 :
            sWaferParam.m_lMeasureSpeedCntrF300 = lValueArg;
            break;
        case REVERSE_MODE :
            sWaferParam.m_lReverseMode = lValueArg;
            break;
        case FLAT_ANGLE :
            sWaferParam.m_lFlatAngle = lValueArg;
            break;
        case MEASURE_TYPE :
            sWaferParam.m_lMeasureType = lValueArg;
            break;
        case WAFER_TYPE :
            sWaferParam.m_lWaferType = lValueArg;
            break;
        case WAFER_LOAD_POSITION :
            sWaferParam.m_lWaferLoadPosition = lValueArg;
            break;
        case WAFER_LIFT_UP_POSITION :
            sWaferParam.m_lWaferLiftUpPosition = lValueArg;
            break;
        case MEASURE_SPEED_FLAT_F200 :
            sWaferParam.m_lMeasureSpeedFlatF200 = lValueArg;
            break;
        case MEASURE_SPEED_FLAT_F300 :
            sWaferParam.m_lMeasureSpeedFlatF300 = lValueArg;
            break;
        case DUMMY2A :
        case DUMMY2B :
        case DUMMY2C :
        case DUMMY2D :
        case DUMMY2E :
        case DUMMY2F :
        case DUMMY2G :
        case DUMMY2H :
        case DUMMY2I :
            sWaferParam.m_lDummy2[iValueDescArg-DUMMY2A] = lValueArg;
            break;
        case CCD1_FIRST_PIXEL :
            sWaferParam.m_lCCD1FirstPixel = lValueArg;
            break;
        case CCD1_ANGLE :
            sWaferParam.m_lCCD1Angle = lValueArg;
            break;
        case CHUCK_VAC_USE :
            sWaferParam.m_lChuckVacUse = lValueArg;
            break;
        case CCD2_FIRST_PIXEL :
            sWaferParam.m_lCCD2FirstPixel = lValueArg;
            break;
        case CCD2_ANGLE :
            sWaferParam.m_lCCD2Angle = lValueArg;
            break;
		case DUMMY4A:
		case DUMMY4B:
		case DUMMY4C:
		case DUMMY4D:
            sWaferParam.m_lDummy4[iValueDescArg-DUMMY4A] = lValueArg;
            break;
        case WAFER_SIZE :
            sWaferParam.m_lWaferSize = lValueArg;
            break;
        case FLAT_TYPE :
            sWaferParam.m_lFlatType = lValueArg;
            break;
        case WAFER_CAL_TYPE :
            sWaferParam.m_lWaferCalType = lValueArg;
            break;
        case DUMMY5A :
        case DUMMY5B :
        case DUMMY5C :
        case DUMMY5D :
        case DUMMY5E :
        case DUMMY5F :
        case DUMMY5G :
            sWaferParam.m_lDummy5[iValueDescArg-DUMMY5A] = lValueArg;
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOGetParamWaferSpeed
 * Abstract:    Gets aligner t, r, z-axis speeds, when a wafer present
 * Parameters:  plSpeed - t, r, z-axis speeds, when a wafer is present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.wafer_spd_T, wafer_spd_R, wafer_spd_Z, as a global variable
 ******************************************************************************/
int FIOGetParamWaferSpeed(long *plSpeed)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        plSpeed[iIndex] = sWaferParam.m_lWaferSpeed[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOSetParamWaferSpeed
 * Abstract:    Sets the aligner t, r, z-axis speeds, when a wafer present
 * Parameters:  plSpeed - t, r, z-axis speeds, when a wafer is present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.wafer_spd_T, wafer_spd_R, wafer_spd_Z, as a global variable
 ******************************************************************************/
int FIOSetParamWaferSpeed(long *plSpeed)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        sWaferParam.m_lWaferSpeed[iIndex] = plSpeed[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOGetParamWaferAccel
 * Abstract:    Gets the aligner t, r, z-axis accelerations, when a wafer present
 * Parameters:  plAccel - t, r, z-axis accelerations, when a wafer is present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.wafer_accel_T, wafer_accel_R, wafer_accel_Z, as a global variable
 ******************************************************************************/
int FIOGetParamWaferAccel(long *plAccel)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        plAccel[iIndex] = sWaferParam.m_lWaferAccel[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOSetParamWaferAccel
 * Abstract:    Sets the aligner t, r, z-axis accelerations, when a wafer present
 * Parameters:  plAccel - t, r, z-axis accelerations, when a wafer is present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.wafer_accel_T, wafer_accel_R, wafer_accel_Z, as a global variable
 ******************************************************************************/
int FIOSetParamWaferAccel(long *plAccel)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        sWaferParam.m_lWaferAccel[iIndex] = plAccel[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOGetParamNoWaferSpeed
 * Abstract:    Gets the aligner t, r, z-axis speeds, when no wafer present
 * Parameters:  plSpeed - t, r, z-axis speeds, when no wafer present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.no_wafer_spd_T, no_wafer_spd_R, no_wafer_spd_Z, as a global variable
 ******************************************************************************/
int FIOGetParamNoWaferSpeed(long *plSpeed)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        plSpeed[iIndex] = sWaferParam.m_lNoWaferSpeed[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOSetParamNoWaferSpeed
 * Abstract:    Sets the aligner t, r, z-axis speeds, when no wafer present
 * Parameters:  plSpeed - t, r, z-axis speeds, when no wafer present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.no_wafer_spd_T, no_wafer_spd_R, no_wafer_spd_Z, as a global variable
 ******************************************************************************/
int FIOSetParamNoWaferSpeed(long *plSpeed)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        sWaferParam.m_lNoWaferSpeed[iIndex] = plSpeed[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOGetParamNoWaferAccel
 * Abstract:    Gets the aligner t, r, z-axis accelerations, when no wafer present
 * Parameters:  plAccel - t, r, z-axis accelerations, when no wafer present.
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.no_wafer_accel_T, no_wafer_accel_R, no_wafer_accel_Z, as a global variable
 ******************************************************************************/
int FIOGetParamNoWaferAccel(long *plAccel)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        plAccel[iIndex] = sWaferParam.m_lNoWaferAccel[iIndex];
    }
    return SUCCESS;
}


/*******************************************************************************
 * Function:    FIOSetParamNoWaferAccel
 * Abstract:    Sets the aligner t, r, z-axis accelerations, when no wafer present
 * Parameters:  plAccel - t, r, z-axis accelerations, when no wafer present
 * Returns:     SUCCESS
 * Replaces:    wafer_par_st.no_wafer_accel_T, no_wafer_accel_R, no_wafer_accel_Z, as a global variable
 ******************************************************************************/
int FIOSetParamNoWaferAccel(long *plAccel)
{
    int iIndex;

    for (iIndex=0; iIndex<3; iIndex++)
    {
        sWaferParam.m_lNoWaferAccel[iIndex] = plAccel[iIndex];
    }
    return SUCCESS;
}

int FIOSetTROffset(int iValueDescArg, long lValueArg, long lWhichArmArg)
{
	switch(iValueDescArg)
	{
        case T_NEG_MAX_OFFSET :
    		gstTROffset[lWhichArmArg].m_lTNegMaxOffset = lValueArg;
			break;
        case T_POS_MAX_OFFSET :
    		gstTROffset[lWhichArmArg].m_lTPosMaxOffset = lValueArg;
			break;
        case R_NEG_MAX_OFFSET :
    		gstTROffset[lWhichArmArg].m_lRNegMaxOffset = lValueArg;
			break;
        case R_POS_MAX_OFFSET :
    		gstTROffset[lWhichArmArg].m_lRPosMaxOffset = lValueArg;
            break;
		default:
			return FAILURE;
	}
	return SUCCESS;
}

int FIOGetTROffset(int iValueDescArg, long *lValueArg, long lWhichArmArg)
{
	switch(iValueDescArg)
	{
        case T_NEG_MAX_OFFSET :
    		*lValueArg = gstTROffset[lWhichArmArg].m_lTNegMaxOffset;
			break;
        case T_POS_MAX_OFFSET :
    		*lValueArg = gstTROffset[lWhichArmArg].m_lTPosMaxOffset;
			break;
        case R_NEG_MAX_OFFSET :
    		*lValueArg = gstTROffset[lWhichArmArg].m_lRNegMaxOffset;
			break;
        case R_POS_MAX_OFFSET :
    		*lValueArg = gstTROffset[lWhichArmArg].m_lRPosMaxOffset;
            break;
		default:
			return FAILURE;
	}
	return SUCCESS;
}

/* Functions to support File Revision */
int FIOGetCalibFileRevision(float *fRevisionArg)
{
    *fRevisionArg = gfCalibFileRevision;
    return SUCCESS;
}
int FIOSetCalibFileRevision(float fRevisionArg)
{
    gfCalibFileRevision = fRevisionArg;
    return SUCCESS;
}
int FIOGetWaferFileRevision(float *fRevisionArg)
{
    *fRevisionArg = sWaferParam.m_fRevision;
    return SUCCESS;
}
int FIOSetWaferFileRevision(float fRevisionArg)
{
    sWaferParam.m_fRevision = fRevisionArg;
    return SUCCESS;
}
