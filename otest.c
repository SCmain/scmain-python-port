<<<<<<< HEAD
/***************************************************************\
 *
 * Program:     Motion Control Main Routines
 * File:        MCMain.C
 * Functions:   GAInit
 *              GAGetMaxNumberOfAxisSupported
 *              GAInitTimerCounters
 *              GAInitStatusWord
 *              GATurnOnGalilInt
 *              GATurnOffGalilInt
 *              GASetUpInterruptVector
 *
 * Description: Provide routines for initializing the Motion
 *      Control module and getting initization realted information.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/
#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sck.h"
#include "gamn.h"
#include "dmclnx.h"

/*********************************************\
 * GLOBAL VARIABLES DECLARATION
\*********************************************/
HANDLEDMC      	ghDMC = -1;         // Handle to controller
CONTROLLERINFO 	gControllerInfo;    // Controller information structure	


/****************************************************************\
 *
 * Function:    GAInit
 *
 * Abstract:    Initialize the Motion Control Module Timer and Galil Interrupt
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAInit(int i, int j)
{
    long rc = 0;
    char buffer[80];


    memset(&gControllerInfo, '\0', sizeof(gControllerInfo));

/////////////////////////////////////////////////////////////////////////////////////
//  MODEL 2100 ETHERNET INTERFACE
    gControllerInfo.cbSize = sizeof(gControllerInfo);
    gControllerInfo.usModelID = MODEL_2100; 
    gControllerInfo.fControllerType = ControllerTypeEthernet;
    gControllerInfo.ulTimeout = 1000;
    gControllerInfo.ulDelay = 5;
//    gControllerInfo.ulDelay = 0;
//    strcpy(gControllerInfo.hardwareinfo.socketinfo.szIPAddress, "169.254.82.100");
    strcpy(gControllerInfo.hardwareinfo.socketinfo.szIPAddress, "10.10.1.11");
    gControllerInfo.hardwareinfo.socketinfo.fProtocol = EthernetProtocolTCP;
/////////////////////////////////////////////////////////////////////////////////////
//  MODEL 1800 PCI INTERFACE
//    gControllerInfo.cbSize = sizeof(gControllerInfo);
//    gControllerInfo.usModelID = MODEL_1800; 
//    gControllerInfo.fControllerType = ControllerTypePCIBus;
//    gControllerInfo.ulTimeout = 1000;
//    gControllerInfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessFIFO;
//    gControllerInfo.ulSerialNumber = 0;  //use relative number
//    gControllerInfo.ulRelativeNumber = 0; //(argc == 2 ? atoi(argv[1]) : 0); //0 is for /
//////////////////////////////////////////////////////////////////////////////////////

    DMCInitLibrary();
	
    // Open the connection
    rc = DMCOpen( &gControllerInfo, &ghDMC );
    if (rc)
    {
	printf("DMCOpen Error: %ld\n", rc);
        return rc;  
    }

    // Init The Galil Timer Counters
    rc = DMCCommand(ghDMC, "\x12\x16", buffer, sizeof(buffer));
    printf("Connected to %s", buffer);
    rc = DMCCommand(ghDMC, "MG _BN", buffer, sizeof(buffer));
    printf("The serial number is %s", buffer);

    return 0;
 }
=======
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
 * Program:     Motion Control Main Routines
 * File:        otest.c
 * Functions:   GAInit
 *              GAGetMaxNumberOfAxisSupported
 *              GAInitTimerCounters
 *              GAInitStatusWord
 *              GATurnOnGalilInt
 *              GATurnOffGalilInt
 *              GASetUpInterruptVector
 *
 * Description: Provide routines for initializing the Motion
 *      Control module and getting initization realted information.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/
#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sck.h"
#include "gamn.h"
#include "dmclnx.h"

/*********************************************\
 * GLOBAL VARIABLES DECLARATION
\*********************************************/
HANDLEDMC      	ghDMC = -1;         // Handle to controller
CONTROLLERINFO 	gControllerInfo;    // Controller information structure	


/****************************************************************\
 *
 * Function:    GAInit
 *
 * Abstract:    Initialize the Motion Control Module Timer and Galil Interrupt
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int GAInit(int i, int j)
{
    long rc = 0;
    char buffer[80];


    memset(&gControllerInfo, '\0', sizeof(gControllerInfo));

/////////////////////////////////////////////////////////////////////////////////////
//  MODEL 2100 ETHERNET INTERFACE
    gControllerInfo.cbSize = sizeof(gControllerInfo);
    gControllerInfo.usModelID = MODEL_2100; 
    gControllerInfo.fControllerType = ControllerTypeEthernet;
    gControllerInfo.ulTimeout = 1000;
    gControllerInfo.ulDelay = 5;
//    gControllerInfo.ulDelay = 0;
//    strcpy(gControllerInfo.hardwareinfo.socketinfo.szIPAddress, "169.254.82.100");
    strcpy(gControllerInfo.hardwareinfo.socketinfo.szIPAddress, "10.10.1.11");
    gControllerInfo.hardwareinfo.socketinfo.fProtocol = EthernetProtocolTCP;
/////////////////////////////////////////////////////////////////////////////////////
//  MODEL 1800 PCI INTERFACE
//    gControllerInfo.cbSize = sizeof(gControllerInfo);
//    gControllerInfo.usModelID = MODEL_1800; 
//    gControllerInfo.fControllerType = ControllerTypePCIBus;
//    gControllerInfo.ulTimeout = 1000;
//    gControllerInfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessFIFO;
//    gControllerInfo.ulSerialNumber = 0;  //use relative number
//    gControllerInfo.ulRelativeNumber = 0; //(argc == 2 ? atoi(argv[1]) : 0); //0 is for /
//////////////////////////////////////////////////////////////////////////////////////

    DMCInitLibrary();
	
    // Open the connection
    rc = DMCOpen( &gControllerInfo, &ghDMC );
    if (rc)
    {
	printf("DMCOpen Error: %ld\n", rc);
        return rc;  
    }

    // Init The Galil Timer Counters
    rc = DMCCommand(ghDMC, "\x12\x16", buffer, sizeof(buffer));
    printf("Connected to %s", buffer);
    rc = DMCCommand(ghDMC, "MG _BN", buffer, sizeof(buffer));
    printf("The serial number is %s", buffer);

    return 0;
 }
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)

void OTFDelay(int iCountArg)
{
    int i, j, k;

    for (i=0;i<iCountArg; ++i)
	for(j=0;j<iCountArg;++j)
		k=0;
<<<<<<< HEAD
}
=======
}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
/****/
int main()
{
    int iDone = 0, i, j=0, k=0;
    int iAMflag;
    long rc;
<<<<<<< HEAD
    char buffer[80];
=======
    char buffer[80];
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
    long lVPSdata[10];
    char c;

    GAInit(0,0);

//    rc = DMCCommand(ghDMC, "SP,694464", buffer, sizeof(buffer)); // RSP 4000
//    rc = DMCCommand(ghDMC, "SP,1041696", buffer, sizeof(buffer));  // RSP 6000
    rc = DMCCommand(ghDMC, "SP,1441696", buffer, sizeof(buffer));  // RSP 8000
    rc = DMCCommand(ghDMC, "AC,4339712", buffer, sizeof(buffer));
    rc = DMCCommand(ghDMC, "DC,4339712", buffer, sizeof(buffer));

    rc = DMCCommand(ghDMC, "SH", buffer, sizeof(buffer));

    rc = DMCCommand(ghDMC, "PA349307;BGX", buffer, sizeof(buffer));
    rc = DMCCommand(ghDMC, "PA,,226511;BGZ", buffer, sizeof(buffer));
    iAMflag = 1;
    while(iAMflag)
    {
	rc = DMCCommand(ghDMC, "MG_BGX", buffer, sizeof(buffer));
	iAMflag = atoi(buffer);
    }
    iAMflag = 1;
    while(iAMflag)
    {
	rc = DMCCommand(ghDMC, "MG_BGZ", buffer, sizeof(buffer));
	iAMflag = atoi(buffer);
    }

    printf("Enter x to continue: ");
    c = getchar();
    if (c != 'x') goto end_exit;

    while(!iDone)
    {
	rc = DMCCommand(ghDMC, "PA,93750;BGY", buffer, sizeof(buffer));
	iAMflag = 1;
	while(iAMflag)
	{
	    rc = DMCCommand(ghDMC, "MG_BGY", buffer, sizeof(buffer));
	    iAMflag = atoi(buffer);
	}
        rc = DMCCommand(ghDMC,"XQ#OTF2", buffer, sizeof(buffer));
	rc = DMCCommand(ghDMC, "PA,-82698;BGY", buffer, sizeof(buffer));

	OTFDelay(20000);

	iAMflag = 1;
	while(iAMflag)
	{
	    rc = DMCCommand(ghDMC, "MG_BGY", buffer, sizeof(buffer));
	    iAMflag = atoi(buffer);
	}

	rc = DMCCommand(ghDMC, "HX", buffer, sizeof(buffer));

        rc = DMCCommand(ghDMC,"MGVCNT", buffer, sizeof(buffer));
	lVPSdata[0] = atol(buffer);
        rc = DMCCommand(ghDMC,"MGVPS1", buffer, sizeof(buffer));
	lVPSdata[1] = atol(buffer);
        rc = DMCCommand(ghDMC,"MGVPS2", buffer, sizeof(buffer));
	lVPSdata[2] = atol(buffer);
        rc = DMCCommand(ghDMC,"MGVPS3", buffer, sizeof(buffer));
	lVPSdata[3] = atol(buffer);
        rc = DMCCommand(ghDMC,"MGVPS4", buffer, sizeof(buffer));
	lVPSdata[4] = atol(buffer);
        rc = DMCCommand(ghDMC,"MGVPS5", buffer, sizeof(buffer));
	lVPSdata[5] = atol(buffer);
        rc = DMCCommand(ghDMC,"MGVPS6", buffer, sizeof(buffer));
	lVPSdata[6] = atol(buffer);

	j++;
	if(lVPSdata[6] == 0)
	{
	    ++k;
	    if(k > 50)
		iDone = 1;
	}
	printf("Cycle=%d errorCount=%d VCNT = %ld\n",j,k, lVPSdata[0]);
        for(i=1;i<7;++i)
	    printf("VPS%d = %ld\n",i, lVPSdata[i]);
    }

end_exit:

    return 0;

<<<<<<< HEAD
}
=======
}
>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
