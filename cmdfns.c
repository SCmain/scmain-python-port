/***************************************************************\
 *
<<<<<<< HEAD
 * Program:    low-level command, command interpreter and Opcode table.
 *
 * File:        CMDcommands.c
=======
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * File Name            : cmdfns.c
 * Description          : Functions which act as an interface for the LowLevel
 *              commands. These functions are local to the low-level command module.
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
 * Program:    low-level command, command interpreter and Opcode table.
 *
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 *
 * Functions:   CMDopcodeUpdateIndxTbl
 *              CMDFirstMacro
 *              CMDfreeOperands
 *              CMDopcodeEntryComp
 *              CMDopcodeGet
 *              CMDputOpcode
 *              CMDinitCommands
 *              CMDdelOpcodeEntry
 *              CMDGetOpcode
 *              CMDcommands
 *              CMDinitialize
 *              CMDget_opcode
 *              CMDparseMath
 *              CMDparseLMCR
 *              CMDparseMCLR
 *              CMDSearchBRpar
 *              CMDgetOperands
 *              CMDgetADEoperands
 *              CMDvalidateInstr
 *              CMDcmndLineAction
 *              CMDgetBRSpecialString
 *
 * Description: Functions which act as an interface for the LowLevel
 *              commands. These functions are locally to low-level command module.
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "cmdfns.h"
#include "cmdval.h"
#include "cmdex.h"
#include "cmdser.h"
#include "cmdinf.h"
#include "cmdmac.h"
#include "cmdfn2.h"
#include "cmdga.h"
#include "cmdal.h"
#include "cmdro.h"
#include "cmdmap.h"
#include "cmdstn.h"
<<<<<<< HEAD
#include "cmdsp.h"
=======
#include "cmdsp.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

#include "sctag.h"
#include "ser.h"
#include "scregg.h"
#include "scmac.h"
#include "sctim.h"
#include "fiog.h"
#include "gag.h"
#include "scmem.h"
<<<<<<< HEAD

#include "pdio.h"
=======

#include "pdio.h"
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
 
int macroEmulatorA = FALSE;
int num_table_entries=0;                /* number of entry currently in the oc table. */
int max_table_entries=0;                /* maximum number of entries in the table. */
int FirstMacro=-1;                      /* location of the first macro had inserted into the table. */
OC_TABLE *OC_table;               /* oc_table structure pointer (oc table).
                                         * This is a list of oc_table structure. In this
                                         * version, the oc_table structure is called
                                         * "oc_entry" or "MacroEntry and the whole table is
                                         * called "OC_table". This table first time will
                                         * allocated the 250 entries. After that each time
                                         * table is needed more space, it will reallocate
                                         * 25 entries more.  Table contains all the
                                         * low-level commands and defined macro.*/
int *oc_indx;                     /* integer pointer (index table).  This is
                                         * an array of index.  Or it holds an index
                                         * to access to OC_table and it's index is an
                                         * Opcode of entry in the OC_table.
                                         * Ex: oc_indx[oc]  is index of OC_table. */
int CMDinitFlags=0, CMDemulFlags=0, LLinitFlag=FALSE;
int LLNumberMacroDeleted = 0;           /* number of macro is deleted.  In the case of
                                         * delete all the macro without sorting, so
                                         * you want to keep track of number of deleted
                                         * macro.  Then table is sorted and updated, you
                                         * need to subtract this number from num_table_entries. */
int RROP = DRROP;
int RSS = DRSS;
int RZP = DRZP;
int SECS = DSECS;
int RRNMO = DRRNMO;
int RRPMO = DRRPMO;
int RTNMO = DRTNMO;
int RTPMO = DRTPMO;
int CDIS = DCDIS;

/* table contains the string name of Brook special keyword commands */
<<<<<<< HEAD
par_table_t   PAR_TABLE[] = 
{   
=======
par_table_t   PAR_TABLE[] = 
{   
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    "STN",	peSTN,		// 2nd level
    "POS",	pePOS,
    "ARM",	peARM,
    "COMM",	peCOMM,
    "LOWER",	peLOWER,
    "NSLOTS",	peNSLOTS,
    "PITCH",	pePITCH,
    "ABS",	peABS,
    "SLOT",	peSLOT,
    "UA",	peUA,
    "FA",	peFA,
    "OFST",	peOFST,
    "WRIST",	peWRIST,
    "RTRCT",	peRTRCT,
    "HISPD",	peHISPD,
    "LOSPD",	peLOSPD,
    "STNSENSOR",    peSTNSENSOR,
    "CPTR",	    peCPTR,
    "IO",	    peIO,
    "ALL",	    peALL,
    "M/B",	    peMB,
    "FLOW",	    peFLOW,
    "MON",	    peMON,
    "PKT",	    pePKT,
    "BKG",	    peBKG,
    "SEQ",	    peSEQ,
    "EX",	    peEX,
    "RE",	    peRE,
    "UP",	    peUP,
    "DN",	    peDN,
    "ON",	    peON,
    "OFF",	    peOFF,
    "AT",	    peAT,
    "AZ",	    peAZ,
    "AH",	    peAH,
    "FAA",	    peFAA,
    "FL",	    peFL,
    "UN",	    peUN,
    "VAC",      peVAC,
    "", 	    peLASTPAR,
  };


/********************************************************
 * Function Name:   CMDopcodeUpdateIndxTbl
 * Description:     Update the OpCode Index Table. Go through the whole
 *                  Opcode index table and shipping the
 *                  entries that have valid OC and skip those
 *                  that have INVALID_OPCODE (-1). This function is called every time
 *                  sorting table, delete or insert entry to the table.
 * Returns:         none.
 *********************************************************/
void CMDopcodeUpdateIndxTbl()
{
    int indx;
    for( indx=0; indx<num_table_entries; indx++ )   /* go from 0 to num_table_entries */
    {
        if( OC_table[indx].OC >= 0 )                /* OC != INVALID_OPCODE */
            oc_indx[OC_table[indx].OC] = indx;      /* assign the new location for each specific OC in the oc_indx table. */
    }
}
/***********************************************************************************************************
 * Function Name:   CMDFirstMacro
 * Description:     Get and return the integer value corresponding to the location of the First
 *                  macro contained in the OpCode table.
 * Returns:         none.
 ***********************************************************************************************************/
void CMDFirstMacro()
{
    int indx;
    for( indx=0; indx<num_table_entries; indx++ )
    {
        if( oc_indx[indx] != INVALID_OPCODE )
        {
            if( OC_table[oc_indx[indx]].flag&CMD_DEFINE_MACRO ) /* look for macro entry and exit the loop */
                break;
        }
    }
    if( indx==num_table_entries )
        FirstMacro = INVALID_OPCODE;        /* no macro found in list */
    else
        FirstMacro = oc_indx[indx];         /* index/location of the oc_table. */
}

/*********************************************************
 * Function Name:   CMDfreeOperands
 * Description:     Free all the memory blocks in an operand list and also check if the
 *                  operand type is string constant, then we need to free the memory
 *                  of the constant string.
 * Parameters:      Optr - The pointer to the first operand in the list to be freed
 * Returned Value:  None
 *********************************************************/
void CMDfreeOperands(CMDoperand_ptr Optr)
{
    CMDoperand_ptr OptrNext;
    while( Optr )
    {
        OptrNext = (CMDoperand_ptr)Optr->next;   /* save the next pointer */
        if( Optr->type == STRING_CONST )        /* if string constant, free the memory of string buffer. */
            free( Optr->opr.s );
        free(Optr);                           /* free an operand. */
        Optr = OptrNext;                        /* continue next operand */
    }
    return;
}
/******************************************************************************************************
 * Function Name:   CMDopcodeEntryComp
 * Description:     Compares the OC_str value of two OpCode entries. OC_str is string name (either defined
 *                  macro name of low-level command name) of Opcode entry.
 * Parameters:      e1 - entry one to compare
 *                  e2 - entry two to compare
 * Returned Value:  <0 if entry one should be alphebetized before entry two
 *                  =0 if entry one and entry two equal
 *                  >0 if entry two should be alphebetized before entry one
 ******************************************************************************************************/
int CMDopcodeEntryComp(const void *e1, const void *e2)
{
    return( strcmp(((OC_TABLE *)e1)->OC_str, ((OC_TABLE *)e2)->OC_str) );
}
/******************************************************************************************************
 * Function Name:   CMDopcodeGet
 * Description:     Search for 'entry' in 'table', which has 'n' entries by using
 *                  binary search.
 * Parameters:      entry - oc_table structure.
 * Returned Value:  a pointer into the passed *table
 ******************************************************************************************************/
OC_TABLE *CMDopcodeGet( OC_TABLE *entry)
{
    if( num_table_entries <= 0 )return NULL;
//    if (TIRefreshWD() == FAILURE) return NULL;
    /* search the entry in the oc_table. */
    return( (OC_TABLE *)bsearch(entry,OC_table,num_table_entries,sizeof(OC_TABLE),CMDopcodeEntryComp) );
}
/*****************************************************************************************************
 * Function Name:   CMDputOpcode
 * Description:     populates a structure of OC_TABLE type and puts it into the OC_TABLE
 * Parameters:      str     A character string representing the OpCode to be
 *                          inserted in the OC_TABLE
 *                  OC      The OpCode number, should be -1 (NOIDX) if a any slot
 *                          in the OC table is acceptable, if this is the case
 *                          than this value will be filled and returned.
 *                  vld     Pointer to the validation routine for this OpCode
 *                  flag    Validation flag for this entry
 *                  exec    Pointer to the routine to be executed for this OpCode
 * OUTPUT:          OC      Will be set if the value passed into this funciton was
 *                          NOIDX
 * Returned Value: SUCCESS/FAILURE
 *****************************************************************************************************/
int CMDputOpcode(char *str, int OC, VLD_PTR vld, int flag, EX_PTR exec)
{
    OC_TABLE entry;

    /* build up the entry before insert/put into the table. */
    strcpy( entry.OC_str, str );        /* set the string name of opcode */
    entry.OC = OC;                      /* set the OC, at this time the oc should be NOIDX(-1) if this
                                         * is new entry. */
    entry.p.vld = vld;                  /* set the pointer of validation routine  */
    entry.flag = flag;                  /* set the validation flag */
    entry.exec = exec;                  /* set the pointer of execution routine. */
    /* insert into the table */
    return( CMDcommands( CMDPUT_NOSORT, entry.OC, &entry ) );
}
/*****************************************************************************************************
 * Function Name:   CMDinitCommands
 * Description:     Initializes all the lowlevel commands in the opcode table by insert each one
 *                  into the table.
 *                  This function is called in CMDinitialize().
 * Parameters:      initFlags - initialization flags
 *                  ewmulFlags - emulation flags
 * Returned Value:  SUCCESS/FAILURE
 *****************************************************************************************************/
int CMDinitCommands(int initFlags, int emulFlags )
{
    /* We need the second flag to check for SECS device and SECS host at same time to enable SECS command. */
    int iOnOffFlag=0, iOnOffFlagMapper=0, iIntellipickFlag=0; // iOnOffFlagSecs=0, iOC;

    /* the first entries in the table have predefined locations corresponding the
     * their OC values, and should NOT be changed */
   if( CMDputOpcode("",EQUAL,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_EQUAL)!=SUCCESS )
        return FAILURE;
    if( emulFlags&DFEMULATORP )
    {
        if( CMDputOpcode("A",QUERY,vld_QUERY,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_QUERY) != SUCCESS )
            return FAILURE;
    }
    else
    {
        if( CMDputOpcode("?",QUERY,vld_QUERY,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_QUERY) != SUCCESS )
            return FAILURE;
    }
    if( CMDputOpcode("ENDM",ENDM,vld_RegNumIAMandENDM,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ENDM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LABEL",LABEL,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_NOP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JPG",JPG,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JPG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JPGE",JPGE,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JPGE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JPL",JPL,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JPL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JPLE",JPLE,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JPLE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JPNZ",JPNZ,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JPNZ)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JPZ",JPZ,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JPZ)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("JUMP",JUMP,vld_Label,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_JUMP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STAT",STAT,vld_stat,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STAT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ENDTR",ENDTR,vld_no_args,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_ENDTR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SVON",SVON,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SVON)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SVOF",SVOF,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SVOF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STOP",STOP,vld_Stop,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STOP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCSI",SCSI,vld_RCSIandSCSI,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCSI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STAT2",STAT2,vld_stat,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STAT2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCS",RCS,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("\\",GALCO,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_GALCO) != SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSCS",RSCS,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSCS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("GLST",GLST,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_GLST)!=SUCCESS )
<<<<<<< HEAD
        return FAILURE;
=======
        return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

    /* get the feature active/deactive value */
//    if (FIOGetCfgFeatureFlags(SCANNER, &iOnOffFlagMapper) == FAILURE)
//        return FAILURE;
//    if(iOnOffFlagMapper)
//	{
    if( CMDputOpcode("RROP",RROP,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RROP)!=SUCCESS )
        return FAILURE;
    if( emulFlags&DFEMULATORG )
    {
        if( CMDputOpcode("RSST",RSS,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSS)!=SUCCESS )
            return FAILURE;
    }
    else
    {
        if( CMDputOpcode("RSS",RSS,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSS)!=SUCCESS )
            return FAILURE;
    }
    if( CMDputOpcode("RZP",RZP,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RZP)!=SUCCESS )
        return FAILURE;
//    }
//#ifdef SX
    /* get the feature active/deactive value */
//    if (FIOGetCfgFeatureFlags(SECS_HOST, &iOnOffFlag) == FAILURE)
//        return FAILURE;
    /* SECS command can be used either in Secs host or Secs Device configuration. Therefore we have to check both flags. */
//    if (FIOGetCfgFeatureFlags(SECS_DEVICE, &iOnOffFlagSecs) == FAILURE)
//        return FAILURE;
//    if (iOnOffFlag || iOnOffFlagSecs)
//    {
//        iOC = (iOnOffFlagMapper) ? 0 : 3;
//        SECS -= iOC;
    if( CMDputOpcode("SECS",SECS,vld_Secs,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SECS)!=SUCCESS )
            return FAILURE;
//    }
//#endif
//#ifdef FP
//    iOC = (iOnOffFlagMapper) ? 0 : 3;
//    iOC += (iOnOffFlag || iOnOffFlagSecs) ? 0 : 1;
//    RRNMO -= iOC;
//    RRPMO -= iOC;
//    RTNMO -= iOC;
//    RTPMO -= iOC;
//    CDIS -= iOC;
    if( CMDputOpcode("RRNMO",RRNMO,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RRNMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RRPMO",RRPMO,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RRPMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RTNMO",RTNMO,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RTNMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RTPMO",RTPMO,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RTPMO)!=SUCCESS )
        return FAILURE;
    /* get the feature active/deactive value */
//    if (FIOGetCfgFeatureFlags(INTELLIPICK, &iIntellipickFlag) == FAILURE)
//        return FAILURE;
//	if(iIntellipickFlag)
//	{
    if( CMDputOpcode("CDIS",CDIS,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_CDIS)!=SUCCESS )
            return FAILURE;
//	}
//#endif
    /* the following entries do not have predefined locations in the OC table */
    if( CMDputOpcode("CLRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_CLRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ABM",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ABM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("AMPI",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_AMPI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("AST",NOIDX,vld_RegNumAST,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_AST)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("BYPAS",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_BYPAS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("CMP",NOIDX,vld_CMP,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_CMP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("CONT",NOIDX,vld_no_args,(CMD_DEFINE_CMD|CMD_VALID_CMDLINE),ex_CONT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("CSTR",NOIDX,vld_CSTR,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_CSTR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DCHN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DCHN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DIAG",NOIDX,vld_Num0,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_DIAG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DMPAS",NOIDX,vld_dmpas,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DMPAS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DOS",NOIDX,vld_String,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DOS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DUMPP",NOIDX,vld_Num0,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_DUMPP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DUMPS",NOIDX,vld_no_args,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_DUMPS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("EIM",NOIDX,vld_EIM,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_EIM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("EIS",NOIDX,vld_no_args,(CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_EIS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ER",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ER)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("EXINP",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_EXINP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("EXIT",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_EXIT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("EXOUT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_EXOUT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("FA",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_FA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("FRMT",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_FRMT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("FRZ",NOIDX,vld_no_args,(CMD_DEFINE_CMD|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION),ex_FRZ)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("GALIL",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_GALIL)!=SUCCESS )
        return FAILURE;
//    if( CMDputOpcode("GLST",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|//CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_GLST)!=SUCCESS )
//        return FAILURE;
    if( CMDputOpcode("GMAC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_GMAC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("GN",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_KP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("HEAP",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_HEAP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("HFLG",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_HFLG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("IAM",NOIDX,vld_RegNumIAMandENDM,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_IAM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("INDEX",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_INDEX)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("INPUT",NOIDX,vld_INPUT,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_INPUT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("KD",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_KD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("KI",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_KI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("KP",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_KP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LIS",NOIDX,vld_no_args,(CMD_VALID_IN_MOTION|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_LIS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LMCR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_LMCR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MACRO",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_NOP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MATS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MATS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MCLR",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MCLR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MIP",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MIP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MHS",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MHS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMOD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SMOD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MOVA",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MOVA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MOVR",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MOVR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MTCR",NOIDX,vld_MTCR,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MTCR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MTCS",NOIDX,vld_MTCS,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MTCS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MTPS",NOIDX,vld_MTCR,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MTPS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("NPAR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_NPAR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("NTOS",NOIDX,vld_NTOS,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_NTOS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("OUTP",NOIDX,vld_OUTP,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_OUTP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("PITCH",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_PITCH)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMOD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMOD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SCM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPSC",NOIDX,vld_StnWrite3,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPSC)!=SUCCESS )
        return FAILURE;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if( CMDputOpcode("PSCAN",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_PSCAN)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("QSCAN",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_QSCAN)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("FILTR",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_FILTR)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("LLOP",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_LLOP)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SSS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SSS)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("RBS",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RBS)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SBS",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SBS)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("RLUX",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RLUX)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SLUX",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SLUX)!=SUCCESS )
            return FAILURE;
<<<<<<< HEAD

=======

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
        if( CMDputOpcode("POTF",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_POTF)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("QOTF",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_QOTF)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SOTFI",NOIDX,vld_OTFI,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SOTFI)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("ROTFI",NOIDX,vld_OTFI,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ROTFI)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SOTFD",NOIDX,vld_OTFD,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SOTFD)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("ROTFD",NOIDX,vld_OTFD,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ROTFD)!=SUCCESS )
            return FAILURE;
    if( CMDputOpcode("RAD",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RAD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCP",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCSI",NOIDX,vld_RCSIandSCSI,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCSI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDBG",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RDBG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDL",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RDL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("READ",NOIDX,vld_READ,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_READ)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("READC",NOIDX,vld_QUERYo,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_READC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("REIM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_REIM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RER",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RER)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RES",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RES)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RESC",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RESC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPAR",NOIDX,vld_Num0,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPAR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RESP",NOIDX,vld_RESP,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RESP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RETH",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RETH)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RFA",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RFA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RGN",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RKP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RFC",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RFC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RHLN",NOIDX,vld_RHLN,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RHLN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSRP",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSRP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RIND",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RIND)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RKD",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RKD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RKI",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RKI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RKP",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RKP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RLN",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RLN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RLP",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RLP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RLS",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RLS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RM)!=SUCCESS )
        return FAILURE;
//    if( CMDputOpcode("RM",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RM)!=SUCCESS )
//        return FAILURE;
    if( CMDputOpcode("RMCR",NOIDX,vld_NumMCR,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RMCR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMCS",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMCS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RNCS",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RNCS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RNSL",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RNSL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ROF",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ROF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPI",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPSL",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPSL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RRET",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RRET)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RRM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RRM)!=SUCCESS )
        return FAILURE;
//    if( CMDputOpcode("RRM",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RRM)!=SUCCESS )
//        return FAILURE;
    if( CMDputOpcode("RSA",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RSA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSEE",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSE)!=SUCCESS )
        return FAILURE;
    if( emulFlags&DFEMULATORP )
    {
        if( CMDputOpcode("RSP0",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSP)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("RSX",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RST)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SAX",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SAV)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SSP0",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SSP)!=SUCCESS )
            return FAILURE;
    }
    else
    {
        if( CMDputOpcode("RSP",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSP)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("RST",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RST)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SAV",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SAV)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("SSP",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SSP)!=SUCCESS )
            return FAILURE;
    }
    if( CMDputOpcode("RSRN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSRN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RTE",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RTE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RTL",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RTL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RTT",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RTT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RZR",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RKD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SAD",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SAD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SDCM",NOIDX,vld_Num,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SDCM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SDL",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SDL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SEOT",NOIDX,vld_Num,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SEOT)!=SUCCESS )
        return FAILURE;
    /* get the feature active/deactive value */
//    if (FIOGetCfgFeatureFlags(TOT, &iOnOffFlag) == FAILURE)
//        return FAILURE;
//    if (iOnOffFlag)
//    {
    if( CMDputOpcode("SIPW",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SIPW)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RIPW",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RIPW)!=SUCCESS )
        return FAILURE;
//    }
    if( CMDputOpcode("SCALE",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCALE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SDBG",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SDBG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SHLN",NOIDX,vld_SHLN,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SHLN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMCR",NOIDX,vld_NumMCR,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SMCR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SNSL",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SNSL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SOF",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SOF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPC",NOIDX,vld_StnWrite3,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SFC",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SFC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SSRP",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SSRP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPSL",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPSL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SRET",NOIDX,vld_Stn_N,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SRET)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SST",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SST)!=SUCCESS )
        return FAILURE;
    /* This would normally use the vld_stat validation, but due to optimization detailed
     * descriptions have been commented out. So vld_Ro is sufficient. */
    if( CMDputOpcode("STAT3",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STAT3)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STON",NOIDX,vld_STON,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STON)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STRC",NOIDX,vld_STRC,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STROK",NOIDX,vld_Stn_F,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STROK)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TA",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TEACH",NOIDX,vld_Num0,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_TEACH)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TL",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKI",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKK",NOIDX,vld_TaskNum0,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKK)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKL",NOIDX,vld_no_args,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKM",NOIDX,vld_TaskNum0,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKM)!=SUCCESS )
        return FAILURE;
/* these command is not supported since the code is not working properly in multi-tasking */
/*    if( CMDputOpcode("TSKG",NOIDX,vld_OUTP,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKS",NOIDX,vld_TaskNum0,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKW",NOIDX,vld_TaskNum0,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKW)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TSKY",NOIDX,vld_TaskNum0,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TSKY)!=SUCCESS )
        return FAILURE;*/
    if( CMDputOpcode("VER",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_VER)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WAIT",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WAIT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WMO",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRIP",NOIDX,vld_Num0,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_WRIP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRIS",NOIDX,vld_no_args,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_WRIS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ZR",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_KD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MTRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MTRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RHFLG",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RHFLG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RTA",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RTA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SSEE",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SSE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RKEY",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_RKEY)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSTIM",NOIDX,vld_Ro,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD|CMD_VALID_IN_MOTION|CMD_VALID_IN_MACRO),ex_RSTIM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCFG",NOIDX,vld_always,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_SCFG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCFG",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCFG)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STAG",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_GSTAT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STA",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_GSTAT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCMEF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCMEF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCMEF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCMEF)!=SUCCESS )
        return FAILURE;
    if( emulFlags&DFEMULATORB )
    {
        if( CMDputOpcode("HOMA",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_HOME)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("HOMR",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_HOME)!=SUCCESS )
            return FAILURE;
        if (iOnOffFlagMapper)
        {
            if( CMDputOpcode("RBM",NOIDX,vld_StnRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RBS)!=SUCCESS )
                return FAILURE;
            if( CMDputOpcode("SBT",NOIDX,vld_Stn_Np,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SBS)!=SUCCESS )
                return FAILURE;
        }
        if( emulFlags&DFEMULATORA )
        {
            if( CMDputOpcode("WM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WM)!=SUCCESS )
                return FAILURE;
        }
    }
    else
    {
        if( CMDputOpcode("HOME",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_HOME)!=SUCCESS )
            return FAILURE;
    }
    if (initFlags & DFAK)
    {
        if( CMDputOpcode("FE",NOIDX,vld_Axis,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_FE)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("CP",NOIDX,vld_Num,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_CP)!=SUCCESS )
            return FAILURE;
    }
    if( emulFlags&DFEMULATORA )
    {
        if( CMDputOpcode("WL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WM)!=SUCCESS )
            return FAILURE;
    }
    if( CMDputOpcode("SWS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SWS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RWS",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RWS)!=SUCCESS )
        return FAILURE;
    /* The reason this uses a #ifdef instead of checking the define flag is that
     * in a prealigner system, the related functions aren't linked in and would
     * have to be stubbed. What a waste of memory. */
//#ifdef FP
    if( CMDputOpcode("SCSN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCSN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DUMPC",NOIDX,vld_no_args,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_DUMPC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DUMPM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DUMPM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRIM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WRIM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DUMPW",NOIDX,vld_no_args,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_DUMPW)!=SUCCESS )
        return FAILURE;
<<<<<<< HEAD


=======


>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if( CMDputOpcode("BCOR",NOIDX,vld_align,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_BCOR)!=SUCCESS )
        return FAILURE;
    /* get the feature active/deactive value */
    if (FIOGetCfgFeatureFlags(SQUARE_WAFER_ALIGN, &iOnOffFlag) == FAILURE)
        return FAILURE;
    if (iOnOffFlag)
    {
        if( CMDputOpcode("BASQ",NOIDX,vld_align,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_BASQ)!=SUCCESS )
            return FAILURE;
    }
    if( CMDputOpcode("MCOR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MCOR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMT",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RWT",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RWT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSP1",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSP1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSP2",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSP2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ZZ1",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ZZ1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCCD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCCD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ZZ2",NOIDX,vld_align,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ZZ2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SSP1",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SSP1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SSP2",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SSP2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RWU",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RWU)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRIW",NOIDX,vld_no_args,(CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_WRIW)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("ALST",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD), ex_ALST)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMSC",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMSC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMSC2",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMSC2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMSC3",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMSC3)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMSF",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMSF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMSF2",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMSF2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMSF3",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMSF3)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("BALN",NOIDX,vld_align,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_BAL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RAD1",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RAD1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RAD2",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RAD2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SAD1",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SAD1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SAD2",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SAD2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDL1",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RAD1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDL2",NOIDX,vld_AxisRead,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RAD2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SDL1",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SAD1)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SDL2",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SAD2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCAFP",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCAFP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCASP",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCASP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMI",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RFAA",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RFAA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("TRACE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_TRACE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("MEAS",NOIDX,vld_OUTP,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_MEAS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("CCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_CCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMAC",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMAC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RMAF",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RMAF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCT",NOIDX,vld_CalTbl,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCFP",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCFP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCSP",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCSP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCT",NOIDX,vld_CalTbl,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RFT",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RFT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RWL",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RWL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("CALC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_CALC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RDA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SDA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SDA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMT",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SWT",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SWT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SWU",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SWU)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCSP",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCSP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMSC",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMSC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMSC2",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMSC2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMSC3",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMSC3)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMSF",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMSF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMSF2",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMSF2)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMSF3",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMSF3)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCAFP",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCAFP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCASP",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCASP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SFAA",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SFAA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMAC",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMAC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCFP",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCFP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SFT",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SFT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SWL",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SWL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SMAF",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SMAF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPMO",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPTY",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPTY)!=SUCCESS )
        return FAILURE;
    /* get the feature active/deactive value */
    //if (FIOGetCfgFeatureFlags(FAST_ALIGN, &iOnOffFlag) == FAILURE)
    //    return FAILURE;
    //if (iOnOffFlag)
    //{
    // Include SPMO unconditionally 
    if( CMDputOpcode("SPMO",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPMO)!=SUCCESS )
        return FAILURE;
    //}
	if(iIntellipickFlag)
	{
        if( CMDputOpcode("BALI",NOIDX,vld_align,(CMD_VALID_IN_MACRO|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_BALI)!=SUCCESS )
            return FAILURE;
        if( CMDputOpcode("ZZ3",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_ZZ3)!=SUCCESS )
            return FAILURE;
	}
    if( CMDputOpcode("SRNMO",NOIDX,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SRNMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SRPMO",NOIDX,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SRPMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STNMO",NOIDX,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STNMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("STPMO",NOIDX,vld_cal_offset,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_STPMO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RCVAC",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RCVAC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SCVAC",NOIDX,vld_WaferParm,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCVAC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RNFA",NOIDX,vld_Ro,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RNFA)!=SUCCESS )
<<<<<<< HEAD
        return FAILURE;
    // Add New Commands for SCMAIN controller
    if( CMDputOpcode("XQFN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XQFN)!=SUCCESS )
=======
        return FAILURE;
    // Add New Commands for SCMAIN controller
    if( CMDputOpcode("XQFN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XQFN)!=SUCCESS )
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	return FAILURE;
    if( CMDputOpcode("XQGT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XQGT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("IT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_DEFINE_CMD),ex_IT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RIT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RIT)!=SUCCESS )
<<<<<<< HEAD
        return FAILURE;
    if( CMDputOpcode("RPDA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPDA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPDA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPDA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPCTE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPCTE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RLPT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RLPT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SLPT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SLPT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XQU",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XQU)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XDM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XDM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SEDF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SEDF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("REDF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_REDF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WRCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LDCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_LDCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRPD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WRPD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LDPD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_LDPD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WXRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WXRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RXRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RXRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RARM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RARM)!=SUCCESS )
        return FAILURE;
=======
        return FAILURE;
    if( CMDputOpcode("RPDA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPDA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPDA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPDA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPCTE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPCTE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RPCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RPCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SPCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SPCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RLPT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RLPT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SLPT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SLPT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XQU",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XQU)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XDM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XDM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("SEDF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SEDF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("REDF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_REDF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WRCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LDCT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_LDCT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WRPD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WRPD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("LDPD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_LDPD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("WXRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_WXRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RXRC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RXRC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RARM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RARM)!=SUCCESS )
        return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    if( CMDputOpcode("SCSR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SCSR)!=SUCCESS )
            return FAILURE;
    if( CMDputOpcode("SRES",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SRES)!=SUCCESS )
            return FAILURE;
<<<<<<< HEAD

    if( CMDputOpcode("SDUMP",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SDUMP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XCL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XCL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XENL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XENL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XREL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XREL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDEU",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RDEU)!=SUCCESS )
        return FAILURE;

    if( CMDputOpcode("XETH",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XETH)!=SUCCESS )
        return FAILURE;

    if( CMDputOpcode("XCLR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XCLR)!=SUCCESS )
        return FAILURE;

    if( CMDputOpcode("XWCI",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XWCI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRWC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRWC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSWC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSWC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XITM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XITM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRTM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRTM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSMX",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSMX)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRMX",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRMX)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRFA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRFA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSFT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSFT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRMR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRMR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XMPA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XMPA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XFDC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XFDC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSTMR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSTMR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XOSN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XOSN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XROS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XROS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRSI",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRSI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XWIO",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XWIO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRIO",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRIO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XROC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XROC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XMVC",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XMVC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSEE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSEE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XREE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XREE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSVS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSVS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRVS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRVS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRVA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRVA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSVA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSVA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRVD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRVD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSVD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSVD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSAV",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSAV)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSRF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSRF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRRF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRRF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XR2W",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XR2W)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XW2R",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XW2R)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XW2E",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XW2E)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XE2W",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XE2W)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XMCD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XMCD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DSEC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DSEC)!=SUCCESS )
        return FAILURE;
=======

    if( CMDputOpcode("SDUMP",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_SDUMP)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XCL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XCL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XENL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XENL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XREL",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XREL)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RDEU",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RDEU)!=SUCCESS )
        return FAILURE;

    if( CMDputOpcode("XETH",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XETH)!=SUCCESS )
        return FAILURE;

    if( CMDputOpcode("XCLR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XCLR)!=SUCCESS )
        return FAILURE;

    if( CMDputOpcode("XWCI",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XWCI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRWC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRWC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSWC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSWC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XITM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XITM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRTM",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRTM)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSMX",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSMX)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRMX",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRMX)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRFA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRFA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSFT",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSFT)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRMR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRMR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XMPA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XMPA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XFDC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XFDC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("RSTMR",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_RSTMR)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XOSN",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XOSN)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XROS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XROS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRSI",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRSI)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XWIO",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XWIO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRIO",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRIO)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XROC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XROC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XMVC",NOIDX,vld_AxisParms,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XMVC)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSEE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSEE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XREE",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XREE)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSVS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSVS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRVS",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRVS)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRVA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRVA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSVA",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSVA)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRVD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRVD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSVD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSVD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSAV",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSAV)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XSRF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XSRF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XRRF",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XRRF)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XR2W",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XR2W)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XW2R",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XW2R)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XW2E",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XW2E)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XE2W",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XE2W)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("XMCD",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_XMCD)!=SUCCESS )
        return FAILURE;
    if( CMDputOpcode("DSEC",NOIDX,vld_always,(CMD_VALID_IN_MACRO|CMD_VALID_CMDLINE|CMD_VALID_IN_MOTION|CMD_DEFINE_CMD),ex_DSEC)!=SUCCESS )
        return FAILURE;
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
//#endif
    return CMDcommands(CMDINIT,0,0);         /* sort and update the table index */
}
/*****************************************************************************************************
 * Function Name:   CMDdelOpcodeEntry
 * Description:     Delete an entry from the table with sorting right at that time or without sorting.
 *                  The purpose for not sorting is to reduce the time for deleting all macros in the
 *                  table. It will take a lot of time if delete one entry and sorting.  However, we
 *                  will sorting table after all the macro is deleted. Also, in the middle process of
 *                  deleting each entry, we keep track the number of entry that is deleted so we can
 *                  update the number of entries in the table. The deleting without sorting is used for
 *                  "MCLR *".
 * Parameters:      iEntryNum - The location in the OC table that needs to be deleted
 *                  iSort - flag for either sorting table or not sorting the table when delete an entry.
 * Returned Value:  SUCCESS or FAILURE
 *****************************************************************************************************/
int CMDdelOpcodeEntry(int iEntryNum, int iSort)
{
    if( iEntryNum>=0 && iEntryNum<max_table_entries )
    {
//        if (TIRefreshWD() == FAILURE) return FAILURE;
        MAFreeInstructions(OC_table[oc_indx[iEntryNum]].p.macro);  /* free memory of the instruction */
        memset( &OC_table[oc_indx[iEntryNum]], 0, sizeof(OC_TABLE) );  /* clear entry */
        OC_table[oc_indx[iEntryNum]].OC_str[0] = '{';
        OC_table[oc_indx[iEntryNum]].OC = NOIDX;  /* set opcode to -1 */
        oc_indx[iEntryNum] = NOIDX;  /* clear OpCode index for this macro */
        if( iSort )
        {
            qsort( OC_table, num_table_entries, sizeof(OC_TABLE), CMDopcodeEntryComp ); /* sort table */
            num_table_entries--;   /* decrement the number of table entries counter */
            CMDopcodeUpdateIndxTbl();   /* update the index table */
            CMDFirstMacro();        /* reset the FirstMacro variable */
        }
        else
             LLNumberMacroDeleted++;    /* no sorting, but keep track number of deleted entry, so
                                         * we can update the number of entries in the table after sorting
                                         * the table. */
        return SUCCESS;
    }
    return FAILURE;
}

/*******************************************************************************
 * Function Name:   CMDGetOpcode
 * Description:     Get the OC number to assign the oc number to new entry. You get
 *                  available location in the oc_indx table. Assume after the occupied
 *                  location is the available location.
 * Returns:         Opcode number (OC)
 *******************************************************************************/
int CMDGetOpcode()
{
    int indx;

    for( indx=max_table_entries-1; indx>0; indx-- )
    {
        if( oc_indx[indx] != NOIDX )    /* get available location */
            return indx+1;
    }
    return NOIDX;
}
/********************************************************************************************
 * Function Name:   CMDcommands
 * Description:     This function is managing the whole oc_table and oc_indx. It does all the manipulation of
 *                  table such as init table with include updating and sorting the oc_table and oc_indx,
 *                  insert, delete, and get the entry from the table. Also, keep track of the location
 *                  of the first macro in the table. This does the action depending on the action that you
 *                  passed as an arguement.
 *                  - INIT action:          Sorting the table, updating the oc_indx and save the location of
 *                                          first inserted macro into the table.
 *                  - GET action:           Get the entry from the table by providing OC number or string name.
 *                  - PUT action:           First need to check the entry is not in the table. Then, assign the OC
 *                                          number depending on the put option with sort or without sort.  With sorting
 *                                          assign the available OC in the table. Without sorting assign the last one
 *                                          in the table. Before inserting if the number of entries is greater than
 *                                          the maximum table entries then we need to reallocate more memory. After inserted
 *                                          the entry into the table if action is put with sorting then sort and update the
 *                                          table; otherwise just sort.  In Addition, if the first time put to the table and
 *                                          table is empty, you need to allocation memory and initialize all OC in each entry
 *                                          to be -1 and also oc_indx table.
 *                  - DELETE action:        There are two options for deleting macro entry from the table. Either delete
 *                                          specific macro entry or delete all macro entry from the table.
 *                                          When deleting specific macro entry from the table, the table is sorted and updated
 *                                          each time.
 *                                          However, when deleting all macros entry from the table, the table is sorted and
 *                                          updated after all the entry is deleted (only one time sorting and updating).
 *                                          There is a variable to keep track of the number of deleted macro, then update the
 *                                          number of table entries after table is sorted and updated.
 *                  - FIRSTMACRO action:    Get back first macro entry that inserted into the table.
 *                  - NEXTMACRO action:     Get the next macro entry from the table.
 * Parameters:      action - An integer flag indicating the intended action
 *                           CMDINIT       =  INITIALIZE the OpCode table, no other
 *                                          parameters are required with this action
 *                           CMDPUT        =  PUT an entry to the OpCode table
 *                           CMDPUT_NOSORT =  PUT an entry at the end of the OpCode
 *                                          table.  This should ONLY be used when the
 *                                          CMDcommands function will be called with the
 *                                          CMDINIT flag afterwards.  This s/b used
 *                                          when adding many commands to the table at
 *                                          one time.
 *                           CMDDELETE     =  DELETE an entry, only the
 *                                          oc_table->OC_Str is further required for
 *                                          this type of action
 *                           CMDGET        =  Get the entry in the OpCode.
 *                                           if oc_num is valid (>=0) the corresponding
 *                                                 table entry will be returned
 *                                           otherwise the entry corresponding to the
 *                                                 oc_table->OC_Str variable will be
 *                                                 returned
 *                           CMDFIRSTMACRO =  return the first macro as the oc_entry
 *                           CMDNEXTMACRO  =  replace the macro contained in oc_entry with
 *                                          the next macro in the OpCode table, if the
 *                                          oc_entry is the last entry in the table,
 *                                          FAILURE will be returned, else SUCCESS
 *                           CMDDELALLMACRO = Delete all Macros in the OpCode Table
 *                  oc_num - The index into the OC_index table that corresponds to a
 *                           specific entry
 *                  oc_entry - A pointer to the information OpCode entry
 * OUTPUT:          oc_entry will contain a returned value if the action is CMDGET
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDcommands(int LLaction, int oc_num, OC_TABLE *oc_entry)
{
    OC_TABLE *entry;
    int indx;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    switch(LLaction)
    {
        case CMDINIT :       /* initialization OpCode Table */
//            if (TIRefreshWD() == FAILURE) return FAILURE;
            /* sort the table using quick sort */
            qsort( OC_table, num_table_entries, sizeof(OC_TABLE), CMDopcodeEntryComp );
            CMDopcodeUpdateIndxTbl();        /* update the indexed table */
            CMDFirstMacro();             /* Find the first inserted macro */
            return SUCCESS;
            break;
        case CMDGET :
            /* Get an entry from the list, if valid index use it, otherwise try string name */
            if( oc_num>=0 && oc_num<=max_table_entries &&
                    oc_indx[oc_num]>=0 && oc_indx[oc_num]<=num_table_entries )
            {
                /* get an entry with valid OC */
                memcpy( oc_entry, &OC_table[oc_indx[oc_num]], sizeof(OC_TABLE) );
                return SUCCESS;
            }
            else if( (entry=CMDopcodeGet(oc_entry)) != NULL )       /* get an entry with string name */
            {
                memcpy( oc_entry, entry,  sizeof(OC_TABLE) );
                return SUCCESS;
            }
            break;
        case CMDPUT :        /* put an entry in the table, sort table after insertion */
        case CMDPUT_NOSORT : /* put an entry in the table, without sorting the table */
            /* make sure the table is not in table */
            if( CMDopcodeGet(oc_entry) != NULL )
            {
#if DEBUG
                SERPutsTxBuff( iCmdPort, "\n***CMDcommands - attempt at adding a duplicate OpCode/Macro Entry\n" );
#endif
                return FAILURE;
            }
            /* determine the next OC entry */
            if( oc_entry->OC == NOIDX )  /* if OC index not assigned, assign it */
            {
                if( LLaction == CMDPUT_NOSORT )
                    oc_entry->OC = num_table_entries;  /* for put without sorting, get oc of the last entry */
                else                    /* for put with sorting, get the available one in the table. */
                    oc_entry->OC = CMDGetOpcode();  /* most entries will need this */
            }

            /* if the table is full, with the last entry being NULL */
            if( num_table_entries>=max_table_entries || oc_entry->OC==NOIDX)
            {
                if( max_table_entries == 0 )  /* table is not created yet. */
                {
                    max_table_entries = CMD_INIT_NUM_MACROS;
                    OC_table = (OC_TABLE *)malloc( max_table_entries*sizeof(OC_TABLE) );
                    oc_indx = (int *)malloc( max_table_entries*sizeof(int) );
                }
                else                    /* add to existing table */
                {
                    max_table_entries += CMD_NUM_MACRO_INC;
                    OC_table = (OC_TABLE *)realloc( (void *)OC_table,
                            (max_table_entries*sizeof(OC_TABLE)) );
                    oc_indx = (int *)realloc( (void *)oc_indx,
                            (max_table_entries*sizeof(int)) );
                }
                if(!OC_table)
                {
#ifdef DEBUG
                    SERPutsTxBuff(iCmdPort, "***CMDcommands: out of memory when doing malloc for oc_table.***\n\r?");
#endif
                    return FAILURE;
                }
                if(!oc_indx)
                {
#ifdef DEBUG
                    SERPutsTxBuff(iCmdPort, "***CMDcommands: out of memory when doing malloc for oc_indx.***\n\r?");
#endif
                    return FAILURE;
                }
                /* Initialize OC of each entry in the oc_table to NOIdX (-1) and
                 * Initialize all location in the oc_indx to NOIDX (-1).*/
                for( indx=num_table_entries; indx<max_table_entries; indx++ )
                {
                    oc_indx[indx] = NOIDX;
                    OC_table[indx].OC = NOIDX;
                }
            }
            /* copy entry into the OC table */
//            if (TIRefreshWD() == FAILURE) return FAILURE;
            memcpy( &OC_table[num_table_entries], oc_entry, sizeof(OC_TABLE) );
            num_table_entries++;
            if( LLaction == CMDPUT )
            {
                /* sort the table */
                qsort( OC_table, num_table_entries, sizeof(OC_TABLE), CMDopcodeEntryComp );
                CMDopcodeUpdateIndxTbl(); /* update the OpCode index table */
                CMDFirstMacro();      /* set the global FirstMacro */
            }
            return SUCCESS;
            break;
        case CMDDELETE :                 /* delete a macro entry from oc_table*/
            /* if the entry that you wanted to delete is in the table */
            if( (entry=CMDopcodeGet(oc_entry)) != NULL )
            {
                if( oc_entry->flag&CMD_DEFINE_MACRO )   /* can only delete a macro */
                {
                    if( CMDdelOpcodeEntry(oc_entry->OC,TRUE) == SUCCESS )
                        return SUCCESS;
                }
            }
            break;
        case CMDFIRSTMACRO :             /* return the first inserted macro in the oc table */
            CMDFirstMacro();             /* get the location of the first macro */
            if( FirstMacro >= 0 )       /* if the FirstMacro location is -1, there is no macro entry in the table */
            {
                memcpy( oc_entry, &OC_table[FirstMacro], sizeof(OC_TABLE) );  /* copy the entry and return */
                return SUCCESS;
            }
            break;
        case CMDNEXTMACRO :              /* return the next macro in the OpCode table */
            /* if entry in table */
            if( (entry=CMDopcodeGet(oc_entry)) != NULL )
            {
                /* for each entry after */
                for( indx=oc_entry->OC+1; indx<max_table_entries; indx++ )
                {
                    /* if there is an associated table entry */
                    if( oc_indx[indx] != INVALID_OPCODE )
                    {
                        /* if the table entry is a macro */
                        if( OC_table[oc_indx[indx]].flag&CMD_DEFINE_MACRO )
                        {
                            /* copy and return the macro */
                            memcpy(oc_entry, &OC_table[oc_indx[indx]], sizeof(OC_TABLE));
                            return SUCCESS;
                        }
                    }
                }
            }
            break;
        case CMDDELALLMACRO :         /* Delete all macro's from the OpCode table */
            /* delete all macro entries in the table without sorting */
            for( indx=0; indx<max_table_entries; indx++ )
            {
                if( oc_indx[indx] != INVALID_OPCODE )
                {
                    if( OC_table[oc_indx[indx]].flag&CMD_DEFINE_MACRO )
                        CMDdelOpcodeEntry(indx,FALSE);
                }
            }
//            if (TIRefreshWD() == FAILURE) return FAILURE;
            /* after deleted all, then sorting the table */
            qsort( OC_table, num_table_entries, sizeof(OC_TABLE), CMDopcodeEntryComp );
            /* update the number of entries in the table. */
            if(LLNumberMacroDeleted)
            {
                num_table_entries -= LLNumberMacroDeleted;
                LLNumberMacroDeleted = 0;
            }
            CMDopcodeUpdateIndxTbl();        /* update the indexed table */
            CMDFirstMacro();             /* Find the first macro */
            return SUCCESS;
            break;
    }
    return FAILURE;
}
/*********************************************************************************************
 * Function Name:   CMDinitialize
 * Description:     This function is called at initialization time in the mainloop and it will
 *                  initialize the table and insert all the low-level commands to the table and
 *                  get the initialization and emulator flag.
 *
 * Parameters:      action -     Either CMDINIT or CMDGET
 *                               LLinitFlag  An integer which is created by "oring" the
 *                               LLinitxxxx macros.
 *                               LLemulator - An integer which is created by "oring" the
 *                               LLemulatorx macros.
 *                  initFlag -   initialization flag
 *                  emulator -   emulator flag
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDinitialize( int action, int *initFlag, int *emulator )
{
    switch(action)
    {
        case CMDINIT :
            if( ! LLinitFlag )
            {
                CMDinitFlags = *initFlag;  /* save the initialzation flag */
                CMDemulFlags = *emulator;  /* save the emulator flag */
                LLinitFlag = TRUE;
                return( CMDinitCommands(CMDinitFlags,CMDemulFlags) );  /* intialize table and insert all
                                                                     * low-level commands into the table. */
            }
            break;
        case CMDGET :                    /* get the initialization and emulator flag */
            if( LLinitFlag )
            {
                *initFlag = CMDinitFlags;
                *emulator = CMDemulFlags;
                return SUCCESS;
                break;
            }
    }
    return FAILURE;
}
/*********************************************************************************************
 * Function Name:   CMDget_opcode
 * Description:     parses the command line for the opcode.  It return the entry that contains
 *                  the opcode.  If the opcode is invalid the entry will have INVALID_OPCODE.
 *                  To get the opcode: When the cmdline pointer passed in from the arguement, the cmdline
 *                  is either the whole string line that got from the com port or the cmdline already parse
 *                  out part of the string already.  However, in this function we always assume this whole line
 *                  of cmdline that got from com port.
 *                  - First, we need to remove all the space and tab if there is any.
 *                  - Then check for the '?', if it is '?' set "?" to OC_str and OC number into the
 *                    oc_entry structure and return next pointer in the cmdline.
 *                  - However, if not '?', then save the cmdline pointer at that position and move
 *                    through the string looking the next space to determine the length of operand string.
 *                  - If the first printable character is not alphabet then there is nothing in the command line.
 *                  - If there is an alphabet then moving forward until no more alphanumeric.
 *                  - Calculate and validate the length; the opcode string length can only between 1 and 5.
 *                    To calculate the length by using the position of current cmdline pointer minus
 *                    the position of save pointer that we have saved before go through the string.
 *                  - Using the saved pointer to check for the first character if it is R, C, or S.
 *                    This will check for R-register, C-register or S-register.
 *                      If it is S, set the mode = -2.
 *                      If it is C, set the mode = -1.
 *                      If it is R, set the mode = 1.
 *                      otherwise, set the mode = 0, not a register type.
 *                    The mode is to determine type of register and not register.
 *                    If the mode != 0, then it is a register. So we expect the next character in the
 *                    cmdline is digit.  If it is the digit which is an index of the register, then
 *                    validate the index upon the type of register. For S-register the index is 0..20.
 *                    For C-register and R-register the index is 0..99. Since it is register then skip
 *                    spaces if there is any; otherwise expecting the next character is '='. If it is
 *                    '=' then set OC_str to be "=" and OC number into oc_entry structure for returning.
 *                    For the case mode = 0 or the first is R, C, or S but not a register, we set the string
 *                    name of opcode into entry and get for entry from the table by calling CMDcommands function with
 *                    action CMDGET and it return back the whole oc_entry if string name opcode provided is
 *                    found in the table.
 *                  - Before return the next pointer in the cmdline and exit the function, also check for the
 *                    case if macro is at defining and it can't call itseft or LMCR itseft since we don't
 *                    support recursive call. LMCR with other macro name inside another macro is working ok.
 *                    Example:      MACRO ME
 *                                  ME          ===> this is not valid (recursive call)
 *                                  LMCR ME     ===> this is not valid
 *                                  YOU         ===> this is valid
 *                                  LMCR YOU    ===> this is valid
 *                                  ENDM
 *                 - In the case of any thing failed, return an oc_entry with
 *                      oc_entry.OC_Str = ""
 *                      oc_entry.OC  = INVALID_OPCODE ==> this is -1
 *                   and reg value is -1. In the successful case, the reg is either 0 or value of register index.
 *             NOTE: This function is called when you want to get the opcode for low-level comamnd or macro in the
 *                  table by give the string name of opcode.
 *
 * Parameters:      cmdline - the character string containing the command line
 *                  genLMCR - indicator that this function is being called from parse_LMCR with Genmark emulation
 * OUTPUT:          instr -   An instruction pointer containing the information parsed
 *                            on the command line
 *                  reg -     The Register number used as an operand for the OpCode on
 *                            the command line, this will only be returned if applicable
 * Returned Value:            The command line after a token has been popped
 *                            if FAILURE during the operation than reg = -1
 *********************************************************************************************/
char *CMDget_opcode( char *cmdline, OC_TABLE *oc_entry, int *reg, int genLMCR )
{
    int numchars, len, mode, i, iRegFlag = TRUE;
    char *start, *emulAbuf;
    char *p1, p2[CMD_OPCODE_LENGTH];

    /* *reg should be initialized in calling function get initialization flags */
    *reg = 0;
    if( !LLinitFlag )
        return cmdline;
    if( CMDemulFlags&DFEMULATORA )       /* set up if emulatorA */
        emulAbuf = cmdline;
    while( *cmdline==' ' || *cmdline=='\t' )  /* remove blanks */
    {
        cmdline++;
    }
    /* Prometrix Emulator uses A as QUERY. The syntax must be checked thoroughly
     * so that it doesn't mistakenly take a Macro or lowlevel command that starts with A */
    if (CMDemulFlags&DFEMULATORP)
    {
        if( *cmdline == 'A' )           /* if match return */
        {
            /* Need to parse the string for all possible syntax forms of A */
            if (CMDparse_PQuery(cmdline+1))
            {
                strcpy( oc_entry->OC_str, "A" );
                oc_entry->OC = QUERY;
                return cmdline+1;
            }
        }
    }
    else
    {
        if( *cmdline == '?' )           /* if match return */
        {
            strcpy( oc_entry->OC_str, "?" );
            oc_entry->OC = QUERY;
            return cmdline+1;
        }
    }
    if( *cmdline == '\\' )              /* if command line begins with '\' set OC to GALCO and return */
    {
        strcpy( oc_entry->OC_str, "\\" );
        oc_entry->OC = GALCO;
        return cmdline+1;
    }
    /* move through the string looking for the next space, determine length of pop */
    start = cmdline;                /* remember beginning of command line */
    if( isalpha(*cmdline) || *cmdline == '#' )
    {
        cmdline++;
    }
    else
    {
        return cmdline;             /* nothing on command line */
    }
    while(isalnum(*cmdline))   /* proceed while reading alpha numeric chars */
    {
        cmdline++;
    }
    len = cmdline - start;          /* remember the length of the operand */
    /* deal with special emulatorG needs */
    if( CMDemulFlags&DFEMULATORG && *cmdline==',' && !genLMCR)
    {
        if( CMDemulFlags&DFEMULATORM )
        {
            numchars = 2;
        }
        else
        {
            numchars = 3;
        }
        strncpy( oc_entry->OC_str, start, numchars );
        oc_entry->OC_str[numchars] = '\0';
<<<<<<< HEAD
//        strupr( oc_entry->OC_str );
=======
//        strupr( oc_entry->OC_str );
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
	for (i = 0; i<strlen(oc_entry->OC_str); ++i) oc_entry->OC_str[i] = toupper(oc_entry->OC_str[i]);
        if( CMDcommands(CMDGET,NOIDX,oc_entry) == SUCCESS )
        {
            return start+numchars;
        }
    }
    /* deal with special emulatorA needs */
    else if( CMDemulFlags&DFEMULATORA && len>CMD_OPCODE_LENGTH )
    {
        strncpy( oc_entry->OC_str, emulAbuf, 2 );
        oc_entry->OC_str[2] = '\0';
//        strupr( oc_entry->OC_str );
	for (i = 0; i<strlen(oc_entry->OC_str); ++i) oc_entry->OC_str[i] = toupper(oc_entry->OC_str[i]);
        if( CMDcommands(CMDGET,NOIDX,oc_entry) == SUCCESS )
        {
            macroEmulatorA = TRUE;
            return start+numchars;
        }
    }
    /* process string if no special needs */
    else if( len <= CMD_OPCODE_LENGTH )
    {
        switch( *start )
        {
            case 'R' :                  /* could be a numeric registers */
                mode = 1;
                break;
            case 'C' :                  /* could be a character register */
                mode = -1;
                break;
            case 'S' :
                mode = -2;              /* could be a string register */
                break;
            default:                    /* could be a macro opcode */
                mode = 0;
                break;
        }
        if( mode )                  /* parse register name, if necessary */
        {
            p1 = start + 1;
            for( i=0; i<len-1; i++ )
            {
                if( isdigit(*p1) )
                {
                    p2[i] = *p1++;
                }
                else
                {
                    iRegFlag = FALSE;
                    break;
                }
            }
            /* The following code will be executed only if the R, C, or S
             * is followed by numbers, not letters: e.g. R10 = 5 is OK, but
             * R (the macro) is not. */
            if (iRegFlag)
            {
                p2[i] = '\0';
                *reg = atoi(p2);
                if( mode!=-2 && RGIsRegisterIndexValid(*reg)==FAILURE )
<<<<<<< HEAD
                {
=======
                {
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
                    iRegFlag = FALSE;
                }
                /* The same check as above. */
                if (iRegFlag)
                {
                    i = 0;
                    while( *cmdline == ' ' )  /* skip over blanks */
                    {
                        cmdline++;
                        i++;
                    }
                    if( *cmdline == '=' )  /* opcode is '=' */
                    {
                        strcpy( oc_entry->OC_str, "" );
                        oc_entry->OC = EQUAL;
                        if( mode != -2 )
                            *reg *= mode;  /* C-register or R-register */
                        else
                            *reg = -(*reg+MAXREG);  /* S-register */
                    /*NOTE for reg: the value of reg will recalculate in CMDparseMath to
                     *              get the real index to store into an operand. Look at CMDparseMath()*/
                        return cmdline+1;
                    }
                    else
                    {
                        cmdline -= i;
                    }
                }
            }
        }      /* if(mode) */
        // FIXME - This is the not_a_reg: goto section. It should be followed
        // by the maybe_ade: and may_be_GA: gotos. They will be run this not_a_reg fails
        strncpy( oc_entry->OC_str, start, len );
        oc_entry->OC_str[len] = '\0';
//        strupr( oc_entry->OC_str );
	for (i = 0; i<strlen(oc_entry->OC_str); ++i) oc_entry->OC_str[i] = toupper(oc_entry->OC_str[i]);
        if(MRGetMacroIntegerVars(MR_DEFINING_MACRO))
        {
            if(strcmp(MRGetMacroAvoidRecursive(),oc_entry->OC_str)==0)
            {
                goto error_exit;
            }
        }
        if( CMDcommands(CMDGET,NOIDX,oc_entry) == SUCCESS )
            return cmdline;
    }          /* parsing */
error_exit:
    oc_entry->OC_str[0] = '\0';
    oc_entry->OC = NOIDX;
    *reg = FAILURE;
    return cmdline;
}
/*********************************************************************************************
 * Function Name:   CMDparse_PQuery
 * Description:     Parses a Prometrix A query command.
 *                  The best way to verify is to eliminate all the spaces embedded in the
 *                  command string. Then the possible syntax range is narrowed down.
 *                  The following syntax forms are all valid A query command.
 *                  The function returns TRUE if it's a valid A query command.
 * Parameters:      cmdline -   the inputted command line
 * Returned Value:  TRUE/FALSE
 *********************************************************************************************/
int CMDparse_PQuery(char *cmdline)
{
    char cTempBuf[5];  /* we'll only parse all but use up to the first 5 non-space char in cmdline */
    int  iTempIndex = 0;
    //char *cTempPtr;

    /* Copy the string while eliminating the spaces */
    while (*cmdline)
    {
        if(*cmdline != ' ')
        {
            cTempBuf[iTempIndex++] = *cmdline;
            if(iTempIndex >= 5)
                break;
        }
        cmdline++;
    }
    /* 'A' must be followed by 'D' or 'H' */
    if(cTempBuf[0] != 'D' && cTempBuf[0] != 'H')
        return FALSE;
    /* 'AD' or 'AH' must be followed by ',', '"', or '[' */
    if(cTempBuf[1] != ',' && cTempBuf[1] != '"' && cTempBuf[1] != '[')
        return FALSE;
    return TRUE;
}
/*********************************************************************************************
 * Function Name:   CMDparseMath
 * Description:     Parses a math instruction by building each operand localing in
 *                  "local_oper" and transfering it to a malloc'd area after it is
 *                  scanned successfully.
 *                  For parsing the math instruction:
 *                  - First, we know that math operator is only dealing with either
 *                    R-register, C-register or S-register to store the result after
 *                    doing the calculation. So we need to get the register index and register
 *                    type to set into an operand by using the "reg" value that passed in from
 *                    function arguement. If the reg < 0, then reg is either C-register or S-register
 *                    else it is R-register.
 *                  - Initialize the number of operand (num_opr) to 0 and initialize the operand pointer
 *                    to NULL before go through the loop to parse the operator and content.
 *                  - While no error occur,
 *                          - Allocate the memory for operand .
 *                          - Link with the next pointer.
 *                          - Scan for the next operand if there is any space, then skip the space.
 *                          - Check for register type, if it is register, then get index and store
 *                            into the operand structure. If not a register, then either character constant,
 *                            string constant or long constant.  For string constant remember to allocate the
 *                            memory for the string. For the long constant there may be '+' or
 *                            '-' in the front of number. So we need to check for the '+' and '-'. If there
 *                            is either one of those then it must be long constant after that.
 *                          - increment the number of operands (num_opr).
 *                          - Allocate the memory for operand that we just parsed out from the cmdline and
 *                            link it with the operand list.
 *                          - Then the cmdline is either now at end of line ('\r') or there is operator.
 *                            If not end of the line, it must be an operator or else error.
 *                            If end of the line, then set the following to the instr structure:
 *                                  instr->opr_ptr = head_ptr   ===> set pointer in an instr pointer to
 *                                                                   the first pointer of operand list.
 *                                  instr->nofopr = num_opr     ===> set number of operand into the instr
 *                                  instr->OC = EQUAL           ===> set the opcode number to EQUAL.
 *                                  since the instructure is like this, "R1 = R2"
 *                          - validate the operator for '>>' and '<<' to make sure if there is one '>' then
 *                            then next character in the cmdline must be '>' or else invalid operator. The same
 *                            checking for '<<'.  Then set the operator into operand.opr.i. If the operator is
 *                            invalid exit the loop and free operands and return instruction with invalid opcode and
 *                            NULL operand pointer.
 *          Note:   This function is called when you knew the opcode is '='. It is called in CMDcmndLineAction().
 *          Note:   This function returns the next pointer in the cmdline that passed in from the function arguement.
 *
 * Parameters:      cmdline -   the inputted command line
 *                  instr -     used for the output
 *                  reg -       the register number of the lvalue
 * OUTPUT:          instr - pointer to the decoded instruction
 * Returned Value: SUCCESS/FAILURE
 *********************************************************************************************/
int CMDparseMath( char *cmdline, instr_ptr instr, int reg )
{
    int char_mode, num_opr, scan_error, indx, reg_num;
    char *Sptr;
    char *Stemp;
    char Ctemp;
    CMDoperand local_operand;
    CMDoperand_ptr head_ptr, prev_ptr;
    CMDoperand_ptr current_ptr;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    Stemp = (char *)malloc(MAXLINE);
    if(!Stemp)
    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "***CMDparseMath: out of memory when doing malloc for string buffer.***\n\r?");
//#endif
        return FAILURE;
   }
    if( reg < 0 )                       /* character to process */
    {
        if( reg >= -MAXREG )            /* this is a character */
        {
            char_mode = 1;
            local_operand.type = INDIRECT_CHAR;
            local_operand.opr.i = -reg;
        }
        else                            /* this is a string */
        {
            char_mode = 2;
            local_operand.type = INDIRECT_STR;
            local_operand.opr.i = -reg-MAXREG;
        }
    }
    else                                /* register to process */
    {
        char_mode = 0;
        local_operand.type = INDIRECT_REG;
        local_operand.opr.i = reg;
    }
    /* parse the rest of the command line and build the instruction */
    num_opr = 0;
    head_ptr = prev_ptr = NULL;
    scan_error = FALSE;

    do
    {
        num_opr++;
        /* alloc mem for current ptr */
        current_ptr = (CMDoperand_ptr)malloc(sizeof(CMDoperand));
        if( current_ptr == NULL )
        {
#ifdef DEBUG
            SERPutsTxBuff( iCmdPort, "\n*** CMDparseMath: out of memory ***\n\n" );
#endif
            scan_error = TRUE;
            break;
        }
        if( head_ptr == NULL )                              /* first time in loop */
            head_ptr = current_ptr;
        local_operand.next = NULL;                          /* initialize next pointer */
        *current_ptr = local_operand;
        if( prev_ptr != NULL )                              /* previous pointer has been initialized */
            prev_ptr->next = (CMDoperand_ptr)current_ptr;    /* update where next points to */
        prev_ptr = current_ptr;                             /* update where previous pointer points */

        /* scan for operand */
        while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white spaces */

        if( *cmdline=='C' || *cmdline=='R' || *cmdline=='S' )  /* if register stuff */
        {
            if( *cmdline == 'C' )
            {
                if( char_mode == 2 )
                    break;
                local_operand.type = INDIRECT_CHAR;
            }
            else if( *cmdline == 'S' )
            {
                if( char_mode != 2 )
                    break;
                local_operand.type = INDIRECT_STR;
            }
            else
            {
                if( char_mode == 2 )
                    break;
                local_operand.type = INDIRECT_REG;
            }
            /* advance the command line and pop off the next opcode */
            cmdline++;
            Sptr = Stemp;                 /* remember start of opcode being popped */
            for( indx=0; indx<CMD_OPCODE_LENGTH; indx++ )  /* pull of numbers */
            {
                if( !isdigit(*cmdline) )break;
                *Sptr++ = *cmdline++;
            }
            *Sptr = '\0';

            reg_num = atoi(Stemp);
            if( local_operand.type != INDIRECT_STR )
            {
                if( RGIsRegisterIndexValid(reg_num)==FAILURE )  /* validate the register index for either R-register or
                                                                 * C-register.*/
                    break;
            }
            else if( RGIsStringIndexValid(reg_num)==FAILURE )  /* validate the register index for S-register. */
                break;
            local_operand.opr.i = reg_num;
        }
        else if( *cmdline == '\'' )     /* if next character is a single quote for character constant */
        {
            if( !char_mode || !isprint(*++cmdline))  /* not character mode and next character not printable */
                break;
            local_operand.type = CHAR_CONST;
            local_operand.opr.i = *cmdline++;  /* get quoted character */
            if( *cmdline++ != '\'' )  /* if ending single quote not found */
                break;
        }
        else if( *cmdline == '\"' )     /* if character is a double quote for string constant */
        {
            if( char_mode != 2 )        /* if not in string mode */
                break;
            cmdline++;
            Sptr = cmdline;
            for( indx=1; *cmdline!='\"'; indx++ )  /* looking for ending double quote */
            {
                if( *cmdline == '\0' || *cmdline++ == '\r' )  /* EOL found before final quote */
                {
                    scan_error = TRUE;
                    goto lscan_error;
                }
            }

            local_operand.type = STRING_CONST;
            if( !(local_operand.opr.s=(char *)malloc(indx)) )  /* check if enough memory to store string variable */
            {
#ifdef DEBUG
                SERPutsTxBuff( iCmdPort, "\n***CMDparseMath: out of memory ***\n\r?" );
#endif
                scan_error = TRUE;
                break;
            }
            strncpy( local_operand.opr.s, (char *)Sptr, indx-1 );
            local_operand.opr.s[indx-1] = '\0';
            local_operand.type = STRING_CONST;
            cmdline++;
        }
        else if( *cmdline=='+' || *cmdline=='-' || isdigit(*cmdline) )  /* long constant */
        {
            Sptr = Stemp;    /* remember the start of the constant being popped */
            *Sptr++ = *cmdline++;       /* advance the pointers */
            /* search until end reached or past ending limit, advancing pointers */
            for( indx=0; indx<CMD_LONG_CONST_LENGTH; indx++ )
            {
                if( !isdigit(*cmdline) )break;
                *Sptr++ = *cmdline++;
            }
            *Sptr = '\0';
            if( strlen(Stemp)==1 && !isdigit(Stemp[0] ) ) /* invalid data found */
                break;
            local_operand.type = LONG_CONST;
            local_operand.opr.l = atol(Stemp);
        }
        else
            break;
        num_opr++;               /* increment the number of operands processed */
        current_ptr = (CMDoperand_ptr)malloc(sizeof(CMDoperand));
        if( current_ptr == NULL )
        {
#ifdef DEBUG
            SERPutsTxBuff( iCmdPort, "\n*** CMDparseMath: out of memory ***\n\r?" );
#endif
            scan_error = TRUE;
            break;
        }
        local_operand.next = NULL;
        *current_ptr = local_operand;
        if( prev_ptr != NULL )
            prev_ptr->next = (CMDoperand_ptr)current_ptr;
        prev_ptr = current_ptr;

        /* scan for an operator */

        while( *cmdline==' ' || *cmdline=='\t' )cmdline++; /* skip over white spaces */

        if( *cmdline == '\0' || *cmdline == '\r' )    /* end of command line found */
        {
            instr->opr_ptr = head_ptr;
            instr->nofopr = num_opr;
            instr->OC = EQUAL;
            if(Stemp)
                free(Stemp);
            return SUCCESS;
        }
        local_operand.type = LLMATH_OPR;  /* this must be a math operand */
        switch( (Ctemp=*cmdline++) )    /* determine operation type */
        {
            case '+' :
            case '-' :
            case '*' :
            case '/' :
            case '&' :
            case '|' :
                local_operand.opr.i = Ctemp;
                break;
            case '<' :
                if( *cmdline++ != '<' )  /* if the command line does not contain << */
                    scan_error = TRUE;
                local_operand.opr.i = Ctemp;
                break;
            case '>' :
                if( *cmdline++ != '>' )  /* if the command line does not contain >> */
                    scan_error = TRUE;
                local_operand.opr.i = Ctemp;
                break;
            default:
                scan_error = TRUE;
                break;
        }
    }while (!scan_error );      /* continue looping until error found
                                 * if no error found will return appropriately
                                 * before this point in the code */
lscan_error:
    CMDfreeOperands(head_ptr);
    instr->OC = INVALID_OPCODE;
    instr->opr_ptr = NULL;
    instr->nofopr = 0;
//    if(Stemp)
//        free(Stemp);
    return FAILURE;
}
/*********************************************************************************************
 * Function Name:   CMDparseLMCR
 * Description:     parses arguments of an LMCR statement, CMDgetOperands() cannot be used
 *                  since it will not scan for a macro name.
 *                  For LMCR command, the command will be either:
 *                      LMCR [name of macro], [0/1]     or
 *                      LMCR
 *                      0 is for not showing the line number
 *                      1 and default is showing the line number.
 *                  For parse the validate the LMCR:
 *                  - First, the pointer of cmdline passed in from function arguement is pointing to
 *                    the next position after "LMCR" already parsed. So we need to skip all the spaces
 *                    and tabs if there is any.
 *                  - If there is end of line ('\r'), then exit the function that's mean there is no
 *                    operand.
 *                  - If there is an operand, get opcode by calling CMDget_opcode() by passing in the
 *                    cmdline pointer and we should get back the oc_entry and reg value. Then check for
 *                    valid opcode in the oc_entry and make sure this opocode is defined macro. To check
 *                    for opcode is define macro by check the validation flag in the oc_entry structure.
 *                    This command (LMCR) only accepts macro type if there is an operand.
 *                  - After got the opcode successfully, allocate memory for operand and
 *                    store information of OC and operand type into the instruction structure for return
 *                    instr that's passed from function arguement.
 *                  - Continue skip spaces or tabs. If it is end of the line then exit function.
 *                    Otherwise check for ',' and then check for next operand which is either '0' or '1'.
 *                    If it is not '0' or '1' then free memory of the operand and return failure. Otherwise
 *                    allocate memory for an operand and set the number of operand to 2 into the instruction structure
 *                    and operand type is LONG_CONST.
 *                  - Then now we expect if there is some more space or tabs and '\r', then return success; otherwise
 *                    return failure if there is still some character in the cmdline.
 *          Note:   This function is called when opcode is "LMCR" and it is called in CMDcmndLineAction().
 * Parameters:      cmdline - Part of the ascii command line entered by the user.
 * OUTPUT:          instr - The instruction pointer, returned if the operation
 *                          is successful.
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDparseLMCR( char *cmdline, instr_ptr instr )
{
    OC_TABLE oc_entry;
    int reg;
    CMDoperand_ptr current_ptr;
    CMDoperand_ptr previous_ptr;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white spaces */
    if( *cmdline == '\0' || *cmdline == '\r')           /* EOL */
        return SUCCESS;
    if (CMDemulFlags&DFEMULATORG)
        cmdline = CMDget_opcode( cmdline, &oc_entry, &reg, TRUE );   /* look in oc table */
    else
        cmdline = CMDget_opcode( cmdline, &oc_entry, &reg, FALSE );  /* look in oc table */
    if((oc_entry.OC == INVALID_OPCODE) || (!(oc_entry.flag & CMD_DEFINE_MACRO)))
        return FAILURE;
    /* unable to allocate memory */
    if( (current_ptr=(CMDoperand_ptr)malloc(sizeof(CMDoperand))) == NULL )
    {
#ifdef DEBUG
        SERPutsTxBuff( iCmdPort, "\n*** CMDparseLMCR: out of memory ***\n\r?" );
#endif
        return FAILURE;
    }
    /* populate information on first operand */
    instr->opr_ptr =  current_ptr;
    instr->nofopr = 1;                  /* populate first operand */
    current_ptr->type = MACRO_OPCODE;
    current_ptr->opr.i = oc_entry.OC;
    current_ptr->next = NULL;

    if( *cmdline == '\0' || *cmdline == '\r' )          /* EOL */
        return SUCCESS;
    if( *cmdline != ',' && *cmdline != ' ' ) /* comma necessary to separate operands */
    {
        CMDfreeOperands(instr->opr_ptr);
        instr->OC = INVALID_OPCODE;
        instr->opr_ptr = NULL;
        instr->nofopr = 0;
        return FAILURE;
    }
    ++cmdline; /* the current char is already checked */
    while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white spaces */
    if( *cmdline!='0' && *cmdline!='1' )  /* invalid command line data */
    {
        CMDfreeOperands(instr->opr_ptr);
        instr->OC = INVALID_OPCODE;
        instr->opr_ptr = NULL;
        instr->nofopr = 0;
        return FAILURE;
    }
    previous_ptr = (CMDoperand_ptr)current_ptr;
    if( (current_ptr=(CMDoperand_ptr)malloc(sizeof(CMDoperand))) == NULL )
    {
#ifdef DEBUG
        SERPutsTxBuff( iCmdPort, "\n*** CMDparseLMCR: out of memory ***\n\r?" );
#endif
        return FAILURE;
    }
    /* populate information on second operand, for option to print line number or not. Default without second
     * operand is printing with line number. */
    instr->nofopr = 2;
    previous_ptr->next = (CMDoperand_ptr)current_ptr;
    current_ptr->type = LONG_CONST;
    if (*cmdline!='0' && *cmdline!='1')
    {
        CMDfreeOperands(instr->opr_ptr);
        instr->OC = INVALID_OPCODE;
        instr->opr_ptr = NULL;
        instr->nofopr = 0;
        return FAILURE;
    }
    current_ptr->opr.l = ((*cmdline++=='0') ? 0 : 1);
    current_ptr->next = NULL;

    while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white spaces */
    if( *cmdline == '\0' || *cmdline == '\r' )          /* EOL reached appropriately */
        return SUCCESS;
    else                                /* EOL not reached and should have been */
    {
        CMDfreeOperands(instr->opr_ptr);
        instr->OC = INVALID_OPCODE;
        instr->opr_ptr = NULL;
        instr->nofopr = 0;
        return FAILURE;
    }
}
/*********************************************************************************************
 * Function Name:   CMDparseMCLR
 * Descripton:      parses arguments for MCLR (clear macro) statement.  The CMDgetOperands()
 *                  function could not be used, it would not scan for a macro name.
 *                  This will parse and validate the command line to make it in the following format:
 *                          MCLR *          or
 *                          MCLR <name of defined macro>
 *                  To parse and validate:
 *                  - First, if there is spaces tabs in the begin of the cmdline then skip until
 *                    there no more space or tab.
 *                  - check for either '*' or macro name.
 *                  - If there is '*' then set the flag for deleting all macro in the oc_table.
 *                  - if there is name of macro, then get the opcode and oc_entry by calling
 *                    CMDget_opcode(). Check make sure the opcode number is valid and it is
 *                    a defined macro.
 *                  - Now, the cmdline should at position end of line ('\r'). Then allocate
 *                    the memory for operand and store the information into operand structure and
 *                    instruction structure.
 *                    If '*' then set the number of operand is 1, operand type is MACRO_OPCODE, and
 *                    operand content is -255. -255 is represented the '*'. Also, set the instruction's
 *                    operand pointer points to local operand pointer that we just allocated the memory.
 *                  - The instruction pointer that is passed in from functin is arguemnet will return with
 *                    the information of operand.
 * Parameters:      cmdline - The ascii command line entered by the user, after
 *                            the MCLR command has been encountered.
 * OUTPUT:          instr -   The instruction pointer is returned in the
 *                            the operation was a success
 * Returned Value:  SUCCESS/FAILURE
 *********************************************************************************************/
int CMDparseMCLR(char *cmdline, instr_ptr instr )
{
    int MCLR_all, reg;
    OC_TABLE oc_entry;
    CMDoperand_ptr Optr;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    while( *cmdline==' ' || *cmdline=='\t' ) cmdline++;  /* skip white space */
    if( *cmdline == '\0' || *cmdline == '\r' )return SUCCESS;  /* EOL */

    if( *cmdline == '*' )
    {
        MCLR_all = TRUE;                /* Clear all macro flag */
        cmdline++; //*cmdline++;
    }
    else
    {
        MCLR_all = FALSE;               /* only clear specified macro */
        cmdline = CMDget_opcode( cmdline, &oc_entry, &reg, FALSE );
        /* make sure it is the macro and valid macro */
        if ((oc_entry.OC == INVALID_OPCODE) || (!(oc_entry.flag & CMD_DEFINE_MACRO)))
            return FAILURE;
    }

    while( *cmdline==' ' || *cmdline=='\t' ) cmdline++;  /* skip white space */
    if( *cmdline != '\0' && *cmdline != '\r' )   /* EOF not found and should have been */
        return FAILURE;
    else
    {
        if( (Optr=(CMDoperand_ptr)malloc(sizeof(CMDoperand))) == NULL )
        {
#ifdef DEBUG
            SERPutsTxBuff( iCmdPort, "\n*** CMDparseMCLR: out of memory ***\n\r?" );
#endif
            return FAILURE;
        }
        if( MCLR_all )                  /* for deleting all macros in the table.*/
        {
            instr->opr_ptr = Optr;
            instr->nofopr = 1;
            Optr->type = MACRO_OPCODE;
            Optr->opr.i = -255;         /* represent '*'  */
            Optr->next = NULL;
        }
        else                            /* delete specific macro in the table. */
        {
            instr->opr_ptr = Optr;
            instr->nofopr = 1;
            Optr->type = MACRO_OPCODE;
            Optr->opr.i = oc_entry.OC;
            Optr->next = NULL;
        }
        return SUCCESS;
    }
}
/*********************************************************************************************
 * Function Name:   CMDSearchBRpar
 * Description:     checks the command line for a brooks keyword by search the name of keyword
 *                  from the PAR_TABLE table. The table contains the string name of all the special
 *                  keyword for Novellus.
 *                  To search for the special keyword:
 *                  - While there is something in the command line that is not a space, comma, or CR,
 *                    advance command line pointer (or index).
 *                  - save the position and character pointing to either space or comma or CR.
 *                  - set the location of that to '\0', so we can have the string name of special
 *                    keyword ending with '\0'.
 *                  - Start beginning the special keyword table and search for the name that we parse
 *                    out from the command line. If found the name in the table, then reset the command line
 *                    as it was upon entering by setting back the saved character to the position that we saved
 *                    and return the index.  Otherwise return failure (-1).  If index is not -1 that's mean it is
 *                    Brook constant type otherwise it is character constant.
 *                  This is ONLY VALID for a brooks emulation
 * Parameters:      cmdline - Part of the ascii command line
 * OUTPUT:          BRindex - The index of the found keyword, only if SUCCESSFUL
 * Returned Value:  SUCCESS or FAILURE
 *********************************************************************************************/
int CMDSearchBRpar(char *cmdline, int *BRindex)
{
    int index;
    char prevChar;

    /* while there is something in the command line that is not a space, comma, or '\r',
     * advance the command line pointer */
    index = 0;
    while(cmdline[index] && cmdline[index]!=' ' && cmdline[index]!=',' && cmdline[index]!='\r')
        index++;

    *BRindex = index;
    prevChar = cmdline[*BRindex];  /* set returned index value and note location */
    cmdline[index] = '\0';              /* assume no more information on the command line */
    for( index=peSTN; index<peLASTPAR; index++ )  /* look in keyword table for a match */
    {
        if( !strcmp(cmdline,PAR_TABLE[index-peSTN].par_str) )  /* if match found */
        {
            break;
        }
    }
    cmdline[*BRindex] = prevChar;       /* reset command line as it was upon entering */

    if(index == peLASTPAR)      /* if entire list parsed=>FAILURE */
        return FAILURE;
    else
        return index;
}
/*********************************************************************************************
 * Function Name:   CMDgetOperands
 * Description:     Decodes operand part of the command string.
 *                  While there is not error in parsing:
 *                  - if CR in the cmdline, then return number of operand.
 *                  - otherwise, check if there is an operand and cmdline points at ',', then
 *                    skip the spaces and tabs if there is any.
 *                    - Check if a constant character or special Novellus keyword and set the
 *                      the operand type to either BR_CONST or CHAR_CONST and set the value of
 *                      character into the operand content.
 *                    - else check if it is LONG_CONST, then check for sign '-' or '+' and follow
 *                      the digit.
 *                    - else check if it is INDIRECT_OPEN ([), then it must be R, C, or S register
 *                      and validate the index of the register depending on the type of the register.
 *                      Then there must be a INDIRECT_END (]). If not get out the loop and return
 *                      failure.
 *                    - else check for STRING_CONST. Make sure to allocate memory for string constant.
 *                  - if the operand is valid for any of those case above then, allocate the memory for
 *                    operand structure and copy the information that has stored into the local operand
 *                    and link in the operand list.
 *                  So we keep parsing utill the end of line or error and exit loop and function.
 *                  If there is an instruction that will be returned will have OC with INVALID_OPCODE value
 *                  and operand pointer is NULL.
 *          Note:   This function is called when parsing the command line and it is called in CMDcmndLineAction()
 *                  and MADefineMacro()
 * Parameters:      cmdline - Part of the ascii command line
 * OUTPUT:          instr - The instruction pointer is returned if SUCCESS
 * Returned Value:  >=0 the number of operands found.
 *                  -1 FAILURE
 *********************************************************************************************/
int CMDgetOperands(char *cmdline, instr_ptr instr)
{
    int numOpr=0, BRidx, index, scan_error=FALSE;
    int brRetval;
    CMDoperand_ptr headPtr, prevPtr;
    CMDoperand_ptr Optr;
    CMDoperand localPtr;
    char *cPtr;
    char *cbuf;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();

    headPtr = prevPtr = NULL;

    if( !LLinitFlag )                  /* get initialization variables */
        return FAILURE;
    cbuf = (char *)malloc(MAXLINE);
    if(!cbuf)
    {
//#ifdef DEBUG
//        SERPutsTxBuff(iCmdPort, "****CMDgetOperands: out of memory.***\n\r?");
//#endif
        return FAILURE;
    }
    do
    {
        while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white space */
        if( *cmdline == '\0' || *cmdline == '\r' )          /* EOL */
        {
            instr->opr_ptr = headPtr;
            instr->nofopr = numOpr;
            if(cbuf)
                free(cbuf);
            return instr->nofopr;       /* valid return from function */
        }
        if( numOpr != 0 && *cmdline==',' )  /* check for commas, CAN NOT be first operand */
        {
            cmdline++;
            while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white space */
        }
        if( isalpha(*cmdline) )         /* if alphabetic character */
        {
            if( CMDemulFlags&DFEMULATORB )  /* if brooks parameter */
            {
                if( (brRetval=CMDSearchBRpar(cmdline,&BRidx)) != FAILURE )  /* and Brooks special */
                {
                    localPtr.type = BR_CONST;
                    localPtr.opr.l = (long)brRetval&0x0000FFFFL;
                    cmdline += BRidx;
                }
                else                    /* valid parameter though not a brooks special */
                {
                    localPtr.type = CHAR_CONST;
                    localPtr.opr.i = *cmdline++;
                }
            }
            else                        /* else assume a character constant */
            {
                localPtr.type = CHAR_CONST;
                localPtr.opr.i = *cmdline++;
            }
        }
        else if( *cmdline=='+' || *cmdline=='-' || isdigit(*cmdline) )  /* math function or number */
        {
            cPtr = cbuf;
            *cPtr++ = *cmdline++;
            /* while possible long and still a number on the command line */
            for( index=0; index<16; index++ )
            {
                if( !isdigit(*cmdline) )break;
                *cPtr++ = *cmdline++;
            }
            *cPtr = 0;
            if( strlen(cbuf)==1 && !isdigit(cbuf[0]) )  /*  one character parsed and not a number */
                break;
            localPtr.type = LONG_CONST;
            localPtr.opr.l = atol(cbuf);  /* convert number and save */
        }
        else if( *cmdline == INDIRECT_OPEN )  /* indirect symbol on the command line */
        {
            cPtr = cbuf;
            cmdline++;
            switch( *cmdline++ )
            {
                case 'R' :              /* numeric register */
                    localPtr.type = INDIRECT_REG;
                    break;
                case 'C' :              /* character register */
                    localPtr.type = INDIRECT_CHAR;
                    break;
                case 'Q' :              /* string register */
                    if( !(CMDemulFlags&DFEMULATORM) )  /* only valid for emulatorM */
                        scan_error = TRUE;
                    else
                        localPtr.type = INDIRECT_STR;
                    break;
                case 'S' :              /* string register */
                    if( CMDemulFlags&DFEMULATORM )  /* not valid for emulatorM */
                        scan_error = TRUE;
                    else
                        localPtr.type = INDIRECT_STR;
                    break;
                default:
                    scan_error = TRUE;
                    break;
            }
            for( index=0; index<5;  index++ )
            {
                if( !isdigit(*cmdline) )break;
                *cPtr++ = *cmdline++;
            }
            *cPtr = 0;
            localPtr.opr.i = atoi(cbuf);        /* get register number and convert to an integer */
            if( localPtr.type != INDIRECT_STR )
            {
                if( RGIsRegisterIndexValid(localPtr.opr.i)==FAILURE )  /* invalid register number */
                    scan_error = TRUE;
            }
            else if( RGIsStringIndexValid(localPtr.opr.i)==FAILURE )
                    scan_error = TRUE;
            if( *cmdline++ != INDIRECT_END )
                scan_error = TRUE;
        }
        else if( *cmdline == MACRO_PARAM )
        {
            cPtr = cbuf;                 /* keep a pointer to the buffer */
            cmdline++; //*cmdline++;                 /* advance the command line pointer */
            switch( *cmdline++ )        /* look for what is next on the command line */
            {
                case 'R' :
                    localPtr.type = MACRO_NUM;
                    break;
                case 'C' :
                    localPtr.type = MACRO_ALPHA;
                    break;
                case 'S' :
                    localPtr.type = MACRO_STR;
                    break;
                default:
                    scan_error = TRUE;
                    break;
            }
            /* look for register number */
            for( index=0; index<5; index++ )
            {
                if( !isdigit(*cmdline) )break;
                *cPtr++ = *cmdline++;
            }
            *cPtr = 0;
            localPtr.opr.i = atoi(cbuf);
            if( localPtr.type != MACRO_STR )
            {
                if( RGIsRegisterIndexValid(localPtr.opr.i)==FAILURE )
                    scan_error = TRUE;
            }
            else if( RGIsStringIndexValid(localPtr.opr.i)==FAILURE )
                    scan_error = TRUE;
        }
        else if( *cmdline == '\"' )     /* character string on command line */
        {
            cmdline++;
            cPtr = cmdline;
            for( index=1; *cmdline != '\"' && !scan_error; index++ )
                if( *cmdline == '\0' || *cmdline++ == '\r' )
                    scan_error = TRUE;
            if( !scan_error )
            {
                localPtr.type = STRING_CONST;
                if( !(localPtr.opr.s=(char *)malloc(index)) )
                {
#ifdef DEBUG
                    SERPutsTxBuff( iCmdPort, "\n***CMDgetOperands: out of memory for malloc string.***\n\r?" );
#endif
                    scan_error = TRUE;
                    break;
                }
                strncpy( localPtr.opr.s, cPtr, index-1 );
                localPtr.opr.s[index-1] = '\0';
                cmdline++; // *cmdline++;
            }
        }
        else if( *cmdline == '*' )
        {
            localPtr.type = CHAR_CONST;
            localPtr.opr.i = *cmdline++;
        }
        else
            scan_error = TRUE;
        if( !scan_error )
        {
            numOpr++;
            Optr = (CMDoperand_ptr)malloc(sizeof(CMDoperand));
            if( Optr == NULL )
            {
#ifdef DEBUG
                SERPutsTxBuff( iCmdPort, "\n***CMDgetOperands: out of memory for malloc an operand.***\n" );
#endif
                scan_error = TRUE;
                break;
            }
            if( headPtr == NULL )
                headPtr = Optr;
            localPtr.next = NULL;
            *Optr = localPtr;
            if( prevPtr != NULL )
                prevPtr->next = (CMDoperand_ptr)Optr;
            prevPtr = Optr;
        }
    } while( !scan_error );

    CMDfreeOperands(headPtr);
    instr->OC = INVALID_OPCODE;
    instr->opr_ptr = NULL;
    instr->nofopr = 0;
//    if(cbuf)
//        free(cbuf);
    return FAILURE;                     /* parsing error encountered */
}
/*********************************************************************************************
 * Function Name:   CMDgetADEoperands
 * Description:     Decodes operands part of the command string, contained in ADE format
 * NOTE:            NOT USED IN THIS VERSION (6.00 I2BRT)
 * Parameters:      cmdline - Part of the ascii command line
 * OUTPUT:          instr - The instruction pointer is returned if SUCCESS
 * Returned Value:  >=0 number of operands found
 *                  -1 FAILURE
 *********************************************************************************************/
int CMDgetADEoperands(char *cmdline, instr_ptr instr)
{
    int num_opr, op_len, scan_error;
    char *Cptr, Cbuf[20];
    CMDoperand_ptr head_ptr, prev_ptr;
    CMDoperand_ptr curr_ptr;
    CMDoperand local_operand;
    int iCmdPort;

    iCmdPort = SERGetCmdPort();
    num_opr = 0;
    scan_error = FALSE;
    head_ptr = prev_ptr = NULL;

    if( !LLinitFlag )                  /* get initialization variables */
        return FAILURE;
    do
    {
        while( *cmdline==' ' || *cmdline=='\t' )cmdline++;  /* skip white space */
        if( *cmdline == '\0' || *cmdline == '\r' )            /* EOL */
        {
            instr->opr_ptr = head_ptr;
            instr->nofopr = num_opr;
            return num_opr;             /* valid exit from function */
        }
        if( isalpha(*cmdline) )         /* alphabetic character first char in string */
        {
            local_operand.type = CHAR_CONST;
            local_operand.opr.i = *cmdline++;
        }
        else if( *cmdline=='+' || *cmdline=='-' || isdigit(*cmdline) )  /* math operation */
        {
            if( !isdigit(*cmdline) )    /* not a digit */
                op_len = 7;
            else
                op_len = 6;             /* a digit */
            Cptr = strncpy( Cbuf, cmdline, op_len );
            Cptr[op_len] = '\0';
            cmdline += strlen(Cbuf);
            local_operand.type = LONG_CONST;
            local_operand.opr.l = atol(Cbuf);
        }
        else
            scan_error = TRUE;          /* invalid data in cmdline */

        if( !scan_error )
        {
            num_opr++;                   /* increment operand counter and add next operand */
            if( (curr_ptr=(CMDoperand_ptr)malloc(sizeof(CMDoperand))) == NULL )
            {
#ifdef DEBUG
                SERPutsTxBuff( iCmdPort, "\n*** CMDparseMath: out of memory ***\n\r?" );
#endif
                scan_error = TRUE;
                break;
            }
            if( head_ptr == NULL )
                head_ptr = curr_ptr;
            local_operand.next = NULL;
            *curr_ptr = local_operand;
            if( prev_ptr != NULL )
                prev_ptr->next = (CMDoperand_ptr)curr_ptr;
            prev_ptr = curr_ptr;
        }
    } while( !scan_error );
    instr->OC = INVALID_OPCODE;
    CMDfreeOperands(head_ptr);
    instr->opr_ptr = NULL;
    instr->nofopr = 0;
    return FAILURE;
}
/*********************************************************************************************
 * Function Name:   CMDvalidateInstr
 * Description:     "Validate" a decoded instruction, insures that operands parsed by get_operands()
 *                  have types compatible with the instruction's opcode. It validates by check if the OC is
 *                  macro type then call the macro validation routine; otherwise call the low-level validation
 *                  routine.
 * Parameters:      ocEntry - The OpCode Entry that corresponds to the instruction
 *                            pointer
 *                  instr -   A pointer to the decoded instruction
 * Returned Value:  SUCCESS / FAILURE
 *********************************************************************************************/
int CMDvalidateInstr(instr_ptr instr)
{
    int num_opr=0;
    int vldResults=SUCCESS;
    int iReturn;
    CMDoperand_ptr Optr;
    OC_TABLE ocEntry;

    /* check nofopr is correct, it always should be */
    Optr = instr->opr_ptr;
    while( Optr != NULL )
    {
        Optr = (CMDoperand_ptr)Optr->next;
        num_opr++;
    }
    if( num_opr != instr->nofopr )
    {
#if DEBUG
        SERPutsTxBuff( SERGetCmdPort(), "\n\n*** CMDvalidateInstr - bad 'nofopr' ***\n\n" );
#endif
        return FAILURE;
    }
    /* get table entry corresponding to the instruction */
    iReturn = CMDcommands(CMDGET,instr->OC,&ocEntry);
    if( (instr->OC!=NOIDX) && (iReturn==SUCCESS) )
    {
        /* call the appropriate validation routine */
        if( ocEntry.flag&CMD_DEFINE_CMD )       /* This is a lowlevel command definition */
        {
            vldResults = (*ocEntry.p.vld)(instr);
        }
        else                                /* this is a macro definition */
        {
            vldResults = vld_macro_call(instr);
        }
        if(MRGetMacroIntegerVars(MR_DEFINING_MACRO))
        {
            if(!(ocEntry.flag&CMD_VALID_IN_MACRO)) /* make sure the command is valid inside macro. */
                return FAILURE;
        }
        else
        {
            if(!(ocEntry.flag&CMD_VALID_CMDLINE))  /* if not command in the macro, make sure the command is
                                                 * valid for the command line. */
               return FAILURE;
        }
    }
    else
    {
        vldResults = FAILURE;
    }
    if( vldResults == FAILURE )
    {
        CMDfreeOperands(instr->opr_ptr );
        instr->OC = INVALID_OPCODE;
        instr->opr_ptr = NULL;
        instr->nofopr = 0;
    }
    return vldResults;
}
/*********************************************************************************************
 * Function Name:   CMDcmndLineAction
 * Description:     Parsing the command line and validate the low-level command or
 *                  macro. This function is main interface for parsing or it is an interpreter
 *                  for command line. It is called after getting the command line from the
 *                  com port.
 *                  - First, check if the command line is empty then return BLANKLINE (-2) to
 *                    PCProcessCmdLine().
 *                  - if There is a command in the command line, get the opcode by calling
 *                    CMDget_opcode() and CMDget_opcode will return the oc_entry structure with
 *                    information of opcode and next pointer in the cmdline.
 *                  - Then make sure the oc_entry has a valid opcode.
 *                  - Then check for special opcode that we need to have special parsing since they
 *                    can not use CMDget_operand such as
 *                    - if EQUAL, then call CMDparseMath to parse the rest of instruction.
 *                    - if MACRO, then check to make sure the macro is not a defining state before
 *                      calling MADefineMacro().
 *                    - if LMCR, then call CMDparseLMCR to parse the rest of instruction.
 *                    - if MCLR, then call CMDparseMCLR to parse the rest of instruction.
 *                    Otherwise, get the number of operands in the an instruction by calling
 *                    CMDgetOperands() which will parse the operands.
 *                  - Then validate the instruction by calling CMDvalidateInstr() if the string name of
 *                    opcode is not "MACRO".
 *
 * Parameters:      cmdline - The ascii command line entered by the user.  If the
 *                            command line corresponds to a low level command
 *                            the command is acted upon.
 *                  iSecsFlag -  = 1 indicates that command is called from Secs module and in case
 *                              when on command line is macro we don't go to define macro we just
 *                              return -3
 * OUTPUT:          instr -   the instruction pointer is returned if the
 *                            operation is successful.
 * Returned Value:  SUCCESS (0)
 *                  FAILURE (-1) Not defined in the OpCode table
 *                  BLANKLINE (-2)
 *                  MACRO (-3)
 *********************************************************************************************/
int CMDcmndLineAction(char *cmdline, instr_ptr instr, int iSecsFlagArg)
{
    int len, indx, reg, getOperands=0;
    int iRetVal, i;
    unsigned int crll, sent_cs;
    char str[3];
    unsigned calc_cs=0;
    OC_TABLE oc_entry;

    if( !LLinitFlag )                  /* get initialization variables */
        return FAILURE;

    while( *cmdline == ' ' || *cmdline == '\t' ) /* skip white space */
        cmdline++;

    if( *cmdline == '\0' || *cmdline == '\r' )  /* BLANKLINE */
        return BLANKLINE;

    instr->nofopr = 0;
    instr->opr_ptr = (CMDoperand_ptr) NULL;
    instr->line_number = 0;
    instr->next_instr = (struct instr_st *) NULL;
    instr->OC = INVALID_OPCODE;

    if( CMDemulFlags&DFEMULATORA )   /* Deal with special case for DFEMULATORA */
    {
        if( isdigit(cmdline[0]) )    /* if the first char is a number */
        {
            strncpy( str, cmdline, 2 );  /* get first two chars */
            str[2] = '\0';  /* get lower byte of numeric representation */
            crll = (unsigned int)atoi(str) & 0x00ff;
            len = strlen(cmdline) - 2;  /* get remaining length of cmdline */
            for( indx=0; indx<len; indx++ )
                calc_cs ^= (unsigned)cmdline[indx];  /* manipulation */
            sscanf( cmdline+len, "%X", &sent_cs );
            if( sent_cs == calc_cs )  /* compare manipulated values */
            {
                cmdline[len] = '\0';
                cmdline++;
            }
            else
                return FAILURE;
        }
        else
            return FAILURE;
    }
    /* Get opcode of low-level command or macro by searching the string name of low-level
     * or defined macro in the table and get back the entry with the opcode in the structure. */
    cmdline = CMDget_opcode( cmdline, &oc_entry, &reg, FALSE );
    /* the OC is invalid and reg = -1 that mean the command or macro is not in the table. */
    if(oc_entry.OC == INVALID_OPCODE && reg == -1)
        return FAILURE;
    instr->OC = oc_entry.OC;
    if(instr->OC == GALCO) /* special parsing for the command which carries Galil commands */
    {
        if(CMDParseGalco(cmdline, instr))
            return FAILURE;
    }
    else if( instr->OC == EQUAL )  /* opcode is EQUAL  */
    {
        if( CMDparseMath(cmdline,instr,reg) )
            return FAILURE;
    }
    else if( !strcmp(oc_entry.OC_str,"MACRO") )  /* defining macro */
    {
        /* macro is at defining state, you can't define another macro. The macro
         * is ended defining with ENDM comamnd */
        if(MRGetMacroIntegerVars(MR_DEFINING_MACRO))
            return FAILURE;
        /* if iSecsFlag == 1, we don't want to define macro just return back to Secs module.
         * This function is later called again from normal process module, but we need to
         * signalize to SECS module the result of evaluation of this command. */
        if(iSecsFlagArg != 1)
        {
            /* define a macro */
//            strupr( cmdline );
	    for (i = 0; i<strlen(cmdline); ++i) cmdline[i] = toupper(cmdline[i]);
            if( MRMacroDefine(cmdline) )
                return FAILURE;
        }
    }
    else if( !strcmp(oc_entry.OC_str,"LMCR") )  /* LMCR command is called */
    {
//        strupr( cmdline );
	for (i = 0; i<strlen(cmdline); ++i) cmdline[i] = toupper(cmdline[i]);
        if( CMDparseLMCR(cmdline,instr) )
           return FAILURE;
    }
    else if( !strcmp(oc_entry.OC_str,"MCLR") )  /* MCLR command is called */
    {
//        strupr( cmdline );
	for (i = 0; i<strlen(cmdline); ++i) cmdline[i] = toupper(cmdline[i]);
        if( CMDparseMCLR(cmdline,instr) )
            return FAILURE;
    }
    else
    {   /* get parameters or operands from the command line. */
        if( CMDemulFlags&DFEMULATORA )
            getOperands = CMDgetADEoperands( cmdline, instr );
        else
            getOperands = CMDgetOperands( cmdline, instr );

        if( getOperands < 0 )                /* if no parameters  */
            return FAILURE;
    }
    /* validate low-level command */
    if( strcmp(oc_entry.OC_str,"MACRO") )
    {
        iRetVal = CMDvalidateInstr(instr);
        if (oc_entry.flag & CMD_DEFINE_MACRO)
        {
            if (iRetVal == FAILURE)
                return FAILURE;
            else
                return MACRO;
        }
        else
            return iRetVal;
    }

    return SUCCESS;
}
/********************************************************************************************
 * Function Name:   CMDgetBRspecialString
 * Description:     get the string name of Novellus special keyword.
 * Parameters:      iIndexArg - index to access to the table.
 *                  pcReturnStringArg - return string value.
 * Returns:         SUCCESS/FAILURE
 *********************************************************************************************/
int CMDgetBRspecialString(int iIndexArg, char *pcReturnStringArg)
{
    strcpy(pcReturnStringArg,PAR_TABLE[iIndexArg-peSTN].par_str);
    return SUCCESS;
}
/********************************************************************************************
 * Function Name:   CMDParseGalco
 * Description:     parses the command line containing Galil command, string begins with '\',
 *                  function parses the string from command line and simulates the operand from it
 *                  in order to be able to call already coded function CMDgetOperands
 * Parameters:      pcCmdStringArg - string from command line without '\' character
 *                  instr - instruction pointer
 *                  pcReturnStringArg - return string value.
 * Returns:         SUCCESS/FAILURE
 *********************************************************************************************/
int CMDParseGalco(char *pcCmdLineArg, instr_ptr instr)
{
    int iGalilCommandLength = 1;    /* Length of Galil command string, = 1 because '"' is first char */
    char caTemp[50];
    int iNumOfOperands;
    long lGalilCardNumber;

    /* Determine which card the galil command is to be sent to */
    if( pcCmdLineArg[0] == '\\' )       /* if for second galil card */
    {
        lGalilCardNumber = (long)GA_CARD_1;
        pcCmdLineArg++;                 /* remove second card indicator, not necessary for command */
    }
    else                                /* else for first galil card */
    {
        lGalilCardNumber = (long)GA_CARD_0;
    }

    caTemp[0] = '"';
    strcpy(&caTemp[1], pcCmdLineArg);

    while((*pcCmdLineArg != '>') && (*pcCmdLineArg != '\0') && (*pcCmdLineArg != '\r'))
    {
        iGalilCommandLength++;
        pcCmdLineArg++;
    }
    if(*pcCmdLineArg == '>')
    {
        iGalilCommandLength++;
        caTemp[iGalilCommandLength++] = '"';
    }
    else
        caTemp[iGalilCommandLength++] = '"';
    if(*pcCmdLineArg == '>')
    {
        caTemp[iGalilCommandLength++] = ',';
        pcCmdLineArg++;
        strcpy(&caTemp[iGalilCommandLength], pcCmdLineArg);
    }
    if((iNumOfOperands = CMDgetOperands(caTemp, instr)) == FAILURE)
        return FAILURE;

    /* allocate memory for another operand pointer */
    instr->opr_ptr->next = (CMDoperand_ptr)malloc( sizeof(CMDoperand) );
    /* populate the new operand pointer with the galil card number */
    instr->opr_ptr->next->type = LONG_CONST;
    instr->opr_ptr->next->opr.l = lGalilCardNumber;
    instr->opr_ptr->next->next = (CMDoperand_ptr)NULL;
    /* increment the operand counter */
    instr->nofopr++;
    return SUCCESS;
}
