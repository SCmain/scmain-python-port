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
 * Program:     VerGen decryption program
 * File:        vergen.c
 * Functions:   main
 * Description: This is a program to generate a binary file with
 *      all the system configuration information that can be read
 *      and searched by the firmware to get configuration information
 *      for a given version.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
 * WARNING NOTE: REMEMBER TO COPY OVER THE RELEVANT HEADERS AND THE
 *               caSysCfgTypeList BEFORE COMPILING!!!!
 *
\***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "sck.h"
#include "scver.h"
#include "fiog.h"
#include "fiol.h"
#include "romain.h"
#include "gag.h"
#include "vergen.h"

/* list of the recognized system configuration strings */
char *caSysCfgTypeList[MAXNUMSYSCFGS] =
{
    "A",		// 3-axis Robot Only
    "ABR",		// 3-axis Robot Only BRooks emulation
    "AV5",		// 3-axis Vac Robot 
    "I2AF",		// 3-axis Robot + 1-axis Flipper
    "I2AT",		// 3-axis Robot + 1-axis Track
    "I2BRF",		// 3-axis Robot + 1-axis Flipper BRooks emulation
    "I2BRT",		// 3-axis Robot + 1-axis Track BRooks emulation
    "I2DA",		// 4-axis Dual-Arm Robot
    "PA",		// 3-axis Prealigner Only
    "I1A",		// 3-axis Robot + 1-axis Prealigner
    "IA",		// 3-axis Robot + 3-axis Prealigner
    "I1AV",		// 3-axis Vac Robot + 1-axis Prealigner
    "I1AV5",		// 3-axis Vac Robot + 1-axis Prealigner
    "I3AVR11",		// 3-axis Vac Robot + 1-axis Prealigner + 2-axis elevator
    "I3AV5R11",		// 3-axis Vac Robot + 1-axis Prealigner + 2-axis elevator
    "I3DAT",		// 4-axis Dual-Arm Robot + 3-axis Prealigner + 1-axis Track
    "I3ATF",		// 3-axis Robot + 3-axis Prealigner + 1-axis Track + 1-axis Flipper
    "I3BRTF",		//     "       "          "               "           " BRooks emul
    "ASG",		// 3-axis Robot Only Genmark Emulation
    "ISG",		// 3-axis Robot + 3-axis Prealigner Genmark Emulation
    "I3SGTF",		// 3-axis Robot + 3-axis Prealigner + 1-axis Track + 1-axis Flipper Gen
    "IPS",		// 3-axis Robot + 3-axis Prealigner Prometrix Emulation
    "I4ATF",		// 3-axis Robot + 1-axis Prealigner + 1-axis Track
    "I4DAT",		// 4-axis Dual-Arm + 3-axis Prealigner + 1-axis Track
    "AK",		// 3-axis Robot Only 
    "ASF",		// 3-axis Robot Only
    "I3AS",		// 3-axis Robot + 3-axis Prealigner + 1-axis Track
    "I3A1",		// 3-axis Robot + 3-axis Prealigner + 1-axis Track
    "I3DA",		// 4-axis Dual-Arm + 3-axis Prealigner + 1-axis Track
    "I2AS",		// 3-axis Robot + 1-axis Track
    "I2AXO"		// 3-axis Robot + 1-axis Track
};


/****************************************************************
 *
 * Function:    main
 *
 * Abstract:    Mainline for entire program coordination
 *
 * Parameters:
 *      argv    (in) The number of command line arguments
 *                  including the call to the executable
 *      argc    (in) An array of strings of those arguments
 *
 * Returns:     N/A
 *
 ***************************************************************/
int main(int argv, char *argc[])
{
    int iRet=0;                         /* Return code. */
    FILE *iFP;                            /* File pointer to handle open file. */
    int iCount;                         /* Temporary loop counter. */
    int iSysCfgNum;                     /* System configuration number.
                                         * Corresponds to the number in the list above. */
    stSysCfgs stSysCfgsInfo;            /* The System Configuration structure. */

    /* Open the syscfgs file for writing. */
    iFP = fopen("/root/controller/scmain/syscfgs", "w" );
    if(iFP <= 0)
    {
        /* On an unsuccessful syscfgs file open. */
        perror("File Open FAILURE ");
        return -1;
    }

    /* Loop for each configuration type. */
    for (iSysCfgNum=0; iSysCfgNum<MAXNUMSYSCFGS; iSysCfgNum++)
    {
        /* Copy the version string into the structure. */
        strcpy(stSysCfgsInfo.m_caSysCfgType, caSysCfgTypeList[iSysCfgNum]);

        /* Copy all relevant configuration data into the structure. */
        switch (iSysCfgNum)
        {
            case 0 :                    /* A */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = 0;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 1 :                    /* ABR */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = 0;
                stSysCfgsInfo.m_iEmulator = DFEMULATORB;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 2 :                    /* AV5 */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = DFVAC514;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 3 :                    /* I2AF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = 0;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 4 :                    /* I2AT */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = DFSS;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 5 :                    /* I2BRF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = 0;
                stSysCfgsInfo.m_iEmulator = DFEMULATORB;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 6 :                    /* I2BRT */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = DFSS;
                stSysCfgsInfo.m_iEmulator = DFEMULATORB;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 7 :                    /* I2DA */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = 0;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_DUAL_ARM;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 8 :                    /* PA */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[1] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[2] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_3_AXIS_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 9 :                    /* I1A */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_SINGLE_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 10 :                   /* IA */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAGAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 11 :                   /* I1AV */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4 | DFVACPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_SINGLE_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 12 :                   /* I1AV5 */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4 | DFVACPRE | DFVAC514;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_SINGLE_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 13 :                   /* I3AVR11 */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4 | DFVACPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_SINGLE_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_INDEXER_Z1;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_INDEXER_Z2;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = TRUE;
                break;

            case 14 :                   /* I3AV5R11 */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4 | DFVACPRE | DFVAC514;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_SINGLE_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_INDEXER_Z1;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_INDEXER_Z2;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = TRUE;
                break;

            case 15 :                   /* I3DAT */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_DUAL_ARM;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 16 :                   /* I3ATF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 17 :                   /* I3BRTF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = DFEMULATORB;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 18 :                    /* ASG */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = 0;
                stSysCfgsInfo.m_iEmulator = DFEMULATORG;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 19 :                   /* ISG */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = DFEMULATORG;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAGAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 20 :                   /* I3SGTF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = DFEMULATORG;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 21 :                   /* IPS */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = DFEMULATORP;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAGAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 22 :                   /* I4ATF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_SINGLE_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 23 :                   /* I4DAT */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 6;
                stSysCfgsInfo.m_iDefineFlag = DFPRE | DFSAP4;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_DUAL_ARM;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_SINGLE_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 24 :                   /* AK */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = DFAK;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 25 :                    /* ASF */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 3;
                stSysCfgsInfo.m_iDefineFlag = 1; //DFSERVOFLIPPER;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 26 :                   /* I3AS */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 27 :                   /* I3A1 */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_FLIPPER;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 28 :                   /* I3DA */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 8;
                stSysCfgsInfo.m_iDefineFlag = DFPRE;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                stSysCfgsInfo.m_iaEquipeAxes[4] = RO_AXIS_t;
                stSysCfgsInfo.m_iaEquipeAxes[5] = RO_AXIS_r;
                stSysCfgsInfo.m_iaEquipeAxes[6] = RO_AXIS_z;
                stSysCfgsInfo.m_iaEquipeAxes[7] = RO_AXIS_w;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[4] = GAEAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[5] = GAFAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[6] = GAGAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[7] = GAHAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                stSysCfgsInfo.m_iaMechType[4] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[5] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[6] = PREALIGNFILE;
                stSysCfgsInfo.m_iaMechType[7] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_DUAL_ARM;
                stSysCfgsInfo.m_iaSpecialAxes[4] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[5] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[6] = RO_3_AXIS_PRE;
                stSysCfgsInfo.m_iaSpecialAxes[7] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 29 :                    /* I2AS */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = DFSS;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;

            case 30 :                    /* I2AXO */
                /* Copy in the number of axes and flags. */
                stSysCfgsInfo.m_iNumOfAxes = 4;
                stSysCfgsInfo.m_iDefineFlag = 0;
//                stSysCfgsInfo.m_iDefineFlag = DFSS;
                stSysCfgsInfo.m_iEmulator = 0;
                /* Fill the Equipe Axes name array. */
                stSysCfgsInfo.m_iaEquipeAxes[0] = RO_AXIS_T;
                stSysCfgsInfo.m_iaEquipeAxes[1] = RO_AXIS_R;
                stSysCfgsInfo.m_iaEquipeAxes[2] = RO_AXIS_Z;
                stSysCfgsInfo.m_iaEquipeAxes[3] = RO_AXIS_W;
                /* Fill in the corresponding Galil Axes array. */
                stSysCfgsInfo.m_iaGalilAxes[0] = GAXAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[1] = GAYAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[2] = GAZAXIS0;
                stSysCfgsInfo.m_iaGalilAxes[3] = GAWAXIS0;
                /* Fill in the corresponding Datafile's array. */
                stSysCfgsInfo.m_iaMechType[0] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[1] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[2] = ROBOTFILE;
                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
//                stSysCfgsInfo.m_iaMechType[3] = AUXFILE;
//                stSysCfgsInfo.m_iaMechType[3] = PREALIGNFILE;
                /* Fill in any corresponding Special Axes array. */
                stSysCfgsInfo.m_iaSpecialAxes[0] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[1] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[2] = RO_ROBOT;
                stSysCfgsInfo.m_iaSpecialAxes[3] = RO_TRACK;
                /* Fill in the array that is passed to the diagnostics module. */
                stSysCfgsInfo.m_iaDiagParms[0] = TRUE;
                stSysCfgsInfo.m_iaDiagParms[1] = FALSE;
                stSysCfgsInfo.m_iaDiagParms[2] = FALSE;
                break;


            default:
                break;
        }

        /* Fill in the remainder of the array with 0. */
        for (iCount=stSysCfgsInfo.m_iNumOfAxes; iCount<MAXARRAYSIZE; iCount++)
        {
            stSysCfgsInfo.m_iaEquipeAxes[iCount] = 0;
            stSysCfgsInfo.m_iaGalilAxes[iCount] = 0;
            stSysCfgsInfo.m_iaMechType[iCount] = -1;
            stSysCfgsInfo.m_iaSpecialAxes[iCount] = RO_NO_SPECIAL;
        }

        /* Write the structure directly into the file. */
        iCount = fwrite(&stSysCfgsInfo, sizeof(stSysCfgs), 1, iFP);
        /* Make sure everything was written in its entirety. */
        if( iCount != 1) //sizeof(stSysCfgs) )
        {
	    //printf("Write Error: iCount=%d, size=%d\n", iCount, sizeof(stSysCfgs));
            perror("Write FAILURE ");
            iRet = -1;
            break;
        }
    }

    /* CLOSE THE SYSCFGS FILE!!! This is very important to prevent errors. */
    fclose( iFP );

    return iRet;
}
