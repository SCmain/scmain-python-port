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
 * File:        scver.c
 * Functions:   InitVersionString
 *              GetVersionString
 *              GetFilename
 *              InitMechArrays
 *              ValidateSysCfgString
 *
 * Description: Gets and sets executable and library versions
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "sck.h"
#include "scver.h"
#include "ser.h"
#include "gag.h"
#include "romain.h"
#include "fiog.h"

//#pragma message(LIBVERSTRING)

char caSysCfgString[15];
char caVersionString[VERSTRLEN+1];
char caFilename[15];
/* list of the recognized system configuration strings */
char *caSysCfgTypeList[MAXNUMSYSCFGS] =
{
    "A",
    "ABR",
    "AV5",
    "I2AF",
    "I2AT",
    "I2BRF",
    "I2BRT",
    "I2DA",
    "PA",
    "I1A",
    "IA",
    "I1AV",
    "I1AV5",
    "I3AVR11",
    "I3AV5R11",
    "I3DAT",
    "I3ATF",
    "I3BRTF",
    "ASG",
    "ISG",
    "I3SGTF",
    "IPS",
    "I4ATF",
    "I4DAT",
    "AK",
    "ASF",
    "I3AS",
    "I3A1",
    "I3DA",
    "I2AS",
    "I2AXO"
};
/* corresponding list of the current revision strings matched by system configuration */
char *caVersionList[MAXNUMSYSCFGS] =
{
    "6.00",     /* A 		1 	*/
    "6.00",     /* ABR 		2	*/
    "6.00",     /* AV5 		3	*/
    "6.00",     /* I2AF 	4	*/
    "6.00",     /* I2AT 	5	*/
    "6.00",     /* I2BRF 	6	*/
    "6.00",     /* I2BRT 	7	*/
    "6.00",     /* I2DA 	8	*/
    "6.00",     /* PA 		9	*/
    "6.00",     /* I1A 		10	*/
    "6.00",     /* IA 		11	*/
    "6.00",     /* I1AV 	12	*/
    "6.00",     /* I1AV5 	13	*/
    "6.00",     /* I3AVR11 	14	*/
    "6.00",     /* I3AV5R11 	15	*/
    "6.00",     /* I3DAT 	16	*/
    "6.00",     /* I3ATF 	17	*/
    "6.00",     /* I3BRTF 	18	*/
    "6.00",     /* ASG 		19	*/
    "6.00",     /* ISG 		20	*/
    "6.00",     /* I3SGTF 	21	*/
    "6.00",     /* IPS 		22	*/
    "6.00",     /* I4ATF 	23	*/
    "6.00",     /* I4DAT 	24	*/
    "6.00",     /* AK 		25	*/
    "6.00",	/* ASF 		26	*/
    "5.03",	/* I3AS 	27	*/
    "5.09",	/* I3A1 	28	*/
    "4.56",	/* I3DA 	29	*/
    "4.72",	/* I2AS 	30	*/
    "4.53"	/* I2AXO	31	*/
};

int giVersionPA = 0;
int giSysCfgNum = 0;

/****************************************************************
 * Function:    InitVersionString
 * Abstract:    Initializes the version strings for both executable
 *      and library versions. The version string, e.g. I2AS, is passed
 *      from main's initialization as is the executable filename.
 * Parameters:
 *      cpVerStrArg     (in) The version string, e.g. I2AS
 *      cpFilenameArg   (in) The executable filename
 * Return:      SUCCESS or FAILURE
 ***************************************************************/
void InitVersionString(char *cpSysCfgArg, char *cpFilenameArg)
{
    int iSysCfgNum, iSlashIndex, iCharIndex, i;
    char caTempStr[MAXLINE];

    /* Search from the end of the string backward for the backslash */
    for (iSlashIndex=((int)(strlen(cpFilenameArg))-1); iSlashIndex>=0; iSlashIndex--)
    {
        if (cpFilenameArg[iSlashIndex] == '\\') break;
    }
    /* Step past the backslash to the first letter of the filename */
    iSlashIndex++;
    /* Copy the filename over */
    for (iCharIndex=iSlashIndex; iCharIndex<(int)(strlen(cpFilenameArg)); iCharIndex++)
    {
        if ((caTempStr[iCharIndex-iSlashIndex] = cpFilenameArg[iCharIndex]) == '.') break;
    }
    caTempStr[iCharIndex-iSlashIndex] = '\0';
    for (i=0; i<strlen(caTempStr); ++i) caTempStr[i] = toupper(caTempStr[i]);
//    strupr(caTempStr);
//    strcat(caTempStr, ".EXE");  no need to append
    /* Save the executable filename for future use */
    strcpy(caFilename, caTempStr);

    /* Save the version string for future use */
    strcpy(caSysCfgString, cpSysCfgArg);
    iSysCfgNum = ValidateSysCfgString(caSysCfgString);
    if (iSysCfgNum == MAXNUMSYSCFGS)
        iSysCfgNum = 0;
    /* Create the complete version string */
//    sprintf(caTempStr, "%s.%s %s", caVersionList[iSysCfgNum], LIBVERSTRING, caSysCfgString);
    sprintf(caTempStr, "%s%s", caVersionList[iSysCfgNum], caSysCfgString);
    /* Save it for future use */
    strncpy(caVersionString, caTempStr, VERSTRLEN);

//    strcpy(caVersionString, "4.49IAS");
    return;
}


/****************************************************************
 * Function:    GetVersionString
 * Abstract:    Returns the version string
 * Returns:     The version string
 ***************************************************************/
char *GetVersionString()
{
    return caVersionString;
}


/****************************************************************
 * Function:    GetFilename
 * Abstract:    Returns the executable filename
 * Returns:     The executable filename
 ***************************************************************/
char *GetFilename()
{
    return caFilename;
}

/****************************************************************
 * Function:    InitMechArrays
 * Abstract:    Sets up the mechanism initialization arrays to
 *      the correct values for the system version being run. The version
 *      string is retrieved from the configuration file and is set with
 *      password protection.
 * Parameters:
 *      ipNumOfAxes         (out) The number of axes in the system
 *      ipDefineFlagArg     (out) The define flags for the system
 *      ipEmulatorArg       (out) The emulator flags for the system
 *      ipEquipeAxesArg     (out) The Equipe axes designations in the system
 *      ipGalilAxesArg      (out) The corresponding Galil axes designations in the system
 *      ipMechTypeArg       (out) The The corresponding parameter files in the system
 *      ipSpecialAxesArg    (out) Any axes which are designated as special in the system
 *      ipDiagParmsArg      (out) The TRUE/FALSE values that are passed to the diagnostics module
 * Returns:     SUCCESS or FAILURE
 ***************************************************************/
int InitMechArrays(int *ipNumOfAxesArg, int *ipDefineFlagArg, int *ipEmulatorArg,
        int *ipEquipeAxesArg, int *ipGalilAxesArg, int *ipMechTypeArg,
        int *ipSpecialAxesArg, int *ipDiagParmsArg)
{
    int iSysCfgNum, iCount, iReadLoop;
    FILE *iFP;
    stSysCfgs stSysCfgInfo;

    giSysCfgNum = ValidateSysCfgString(caSysCfgString);
    if (giSysCfgNum == MAXNUMSYSCFGS)
        goto return_default_vals;

//printf("syscfgnum=%d str=%s\n",giSysCfgNum, caSysCfgString);

    /* Open the syscfgs file on the PROM. */
//    iFP = open("A:\\SYSCFGS", O_RDONLY|O_BINARY);
    iFP = fopen("/root/controller/scmain/syscfgs", "r");
    if( iFP == NULL )
    {
        /* On an unsuccessful syscfgs file open. */
        perror("Syscfgs Read Open Error ");
        /* Return default values. */
        goto return_default_vals;
    }
    else
    {
        for (iReadLoop=0; iReadLoop<MAXNUMSYSCFGS; iReadLoop++)
        {
            /* Read the file directly into the syscfgs structure. */
//            iCount = read(iFP, &stSysCfgInfo, sizeof(stSysCfgs));
            iCount = fread(&stSysCfgInfo, sizeof(stSysCfgs), 1, iFP);
            if (iCount < 1)
	    {
		fclose( iFP );
                goto return_default_vals;
	    }
            else if (!strcmp(stSysCfgInfo.m_caSysCfgType, caSysCfgTypeList[giSysCfgNum]))
                break;
        }
        /* CLOSE THE SYSCFGS FILE ON THE PROM!!! This is very important to prevent errors. */
        fclose( iFP );
        if (iReadLoop == MAXNUMSYSCFGS)
            goto return_default_vals;
    }

    *ipNumOfAxesArg = stSysCfgInfo.m_iNumOfAxes;
    //1:1
    *ipDefineFlagArg = stSysCfgInfo.m_iDefineFlag;
    *ipEmulatorArg = stSysCfgInfo.m_iEmulator;
    memcpy(ipEquipeAxesArg, stSysCfgInfo.m_iaEquipeAxes, MAXARRAYSIZE*sizeof(int));
    memcpy(ipGalilAxesArg, stSysCfgInfo.m_iaGalilAxes, MAXARRAYSIZE*sizeof(int));
    memcpy(ipMechTypeArg, stSysCfgInfo.m_iaMechType, MAXARRAYSIZE*sizeof(int));
    memcpy(ipSpecialAxesArg, stSysCfgInfo.m_iaSpecialAxes, MAXARRAYSIZE*sizeof(int));
    memcpy(ipDiagParmsArg, stSysCfgInfo.m_iaDiagParms, 3*sizeof(int));

//printf("numOfAxes=%d cfgnum=%d cfgstr=%s",*ipNumOfAxesArg,iReadLoop,stSysCfgInfo.m_caSysCfgType);
int i;
for (i=0; i<*ipNumOfAxesArg; ++i)
//printf("ipGalilAxes[%d]=%d\n",i,ipGalilAxesArg[i]);

    return SUCCESS;

return_default_vals:

    /* The reason this code is included with a #ifdef instead of by checking
     * the define flag is because the define flag isn't known in this module.
     * In fact it is set by this function. */
#ifdef NOFP
    InitVersionString("A", "UNKNOWN");
    /* Copy in the number of axes and flags. */
    *ipNumOfAxesArg = 3;
    *ipDefineFlagArg = 0;
    *ipEmulatorArg = 0;
    /* Fill the Equipe Axes name array. */
    ipEquipeAxesArg[0] = RO_AXIS_T;
    ipEquipeAxesArg[1] = RO_AXIS_R;
    ipEquipeAxesArg[2] = RO_AXIS_Z;
    /* Fill in the corresponding Galil Axes array. */
    ipGalilAxesArg[0] = GAXAXIS0;
    ipGalilAxesArg[1] = GAYAXIS0;
    ipGalilAxesArg[2] = GAZAXIS0;
    /* Fill in the corresponding Datafile's array. */
    ipMechTypeArg[0] = ROBOTFILE;
    ipMechTypeArg[1] = ROBOTFILE;
    ipMechTypeArg[2] = ROBOTFILE;
    /* Fill in any corresponding Special Axes array. */
    ipSpecialAxesArg[0] = RO_ROBOT;
    ipSpecialAxesArg[1] = RO_ROBOT;
    ipSpecialAxesArg[2] = RO_ROBOT;
    /* Fill in the array that is passed to the diagnostics module. */
    ipDiagParmsArg[0] = TRUE;
    ipDiagParmsArg[1] = FALSE;
    ipDiagParmsArg[2] = FALSE;
#else
//    InitVersionString("IA", "UNKNOWN");
    InitVersionString("A", "UNKNOWN");
    /* Copy in the number of axes and flags. */
    *ipNumOfAxesArg = 3;
    *ipDefineFlagArg = 0;
//    *ipDefineFlagArg = DFPRE;
    *ipEmulatorArg = 0;
    /* Fill the Equipe Axes name array. */
    ipEquipeAxesArg[0] = RO_AXIS_T;
    ipEquipeAxesArg[1] = RO_AXIS_R;
    ipEquipeAxesArg[2] = RO_AXIS_Z;
//    ipEquipeAxesArg[0] = RO_AXIS_t; 
//    ipEquipeAxesArg[1] = RO_AXIS_r;
//    ipEquipeAxesArg[2] = RO_AXIS_z;
    ipEquipeAxesArg[3] = RO_AXIS_t;
    ipEquipeAxesArg[4] = RO_AXIS_r;
    ipEquipeAxesArg[5] = RO_AXIS_z;
    /* Fill in the corresponding Galil Axes array. */
    ipGalilAxesArg[0] = GAXAXIS0;
    ipGalilAxesArg[1] = GAYAXIS0;
    ipGalilAxesArg[2] = GAZAXIS0;
    ipGalilAxesArg[3] = GAXAXIS0;
    ipGalilAxesArg[4] = GAYAXIS0;
    ipGalilAxesArg[5] = GAZAXIS0;
    /* Fill in the corresponding Datafile's array. */
    ipMechTypeArg[0] = ROBOTFILE;
    ipMechTypeArg[1] = ROBOTFILE;
    ipMechTypeArg[2] = ROBOTFILE;
//    ipMechTypeArg[0] = PREALIGNFILE;
//    ipMechTypeArg[1] = PREALIGNFILE;
//    ipMechTypeArg[2] = PREALIGNFILE;
    ipMechTypeArg[3] = PREALIGNFILE;
    ipMechTypeArg[4] = PREALIGNFILE;
    ipMechTypeArg[5] = PREALIGNFILE;
    /* Fill in any corresponding Special Axes array. */
    ipSpecialAxesArg[0] = RO_ROBOT;
    ipSpecialAxesArg[1] = RO_ROBOT;
    ipSpecialAxesArg[2] = RO_ROBOT;
//    ipSpecialAxesArg[0] = RO_3_AXIS_PRE;
//    ipSpecialAxesArg[1] = RO_3_AXIS_PRE;
//    ipSpecialAxesArg[2] = RO_3_AXIS_PRE;
    ipSpecialAxesArg[3] = RO_3_AXIS_PRE;
    ipSpecialAxesArg[4] = RO_3_AXIS_PRE;
    ipSpecialAxesArg[5] = RO_3_AXIS_PRE;
    /* Fill in the array that is passed to the diagnostics module. */
    ipDiagParmsArg[0] = TRUE;
    ipDiagParmsArg[1] = FALSE;
    ipDiagParmsArg[2] = FALSE;
#endif

    return FAILURE;
}


/****************************************************************
 * Function:    ValidateSysCfgString
 * Abstract:    Returns the array number corresponding to the system
 *      configuration string passed in.
 * Parameters:
 *      cpSysCfgStringArg   (in) The system configuration string
 * Returns:     Array number
 ***************************************************************/
int ValidateSysCfgString(char *cpSysCfgStringArg)
{
    int iSysCfgNum;

    for (iSysCfgNum=0; iSysCfgNum<MAXNUMSYSCFGS; iSysCfgNum++)
    {
        if (!strcmp(cpSysCfgStringArg, caSysCfgTypeList[iSysCfgNum]))
            break;
    }

    return iSysCfgNum;
}
