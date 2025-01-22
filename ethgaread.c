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
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <linux/if.h>

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
int readGA(int fd)
{
    long rc = 0;
    char buffer[80];
    long  lSN, lSN2;
    int  iTot;


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

    rc = DMCCommand(ghDMC, "MG _BN", buffer, sizeof(buffer));
    if (rc)
    {
	printf("DMC read Error: %ld\n", rc);
        return rc;  
    }

    lSN = atol(buffer);

    // read from the file
    iTot = fread((void *)&lSN2, sizeof(long), 1, fd);
    
    printf("    GalilSN=%ld fileSN=%ld\n",lSN,lSN2);
    if (lSN != lSN2)
    {
	printf("SN does not match!\n");
    }

    rc = DMCClose(ghDMC);
    return 0;
 }

int readETH(int fd)
{
    int giSockfd, giNewSockfd, giPortno, giClilen, giPid, giSockfd2;

    struct sockaddr_in gsServAddr, gsCliAddr;
    struct sockaddr sAddr;
    socklen_t socklen;

    u_char addr[80], addr2[80];
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i, iTot;
    int ok = 0;

    giSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (giSockfd < 0) 
    {
printf("ETHInitPort: socket()=%d failed.\n",giSockfd);
        return FAILURE;
    }

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

    // read from file
    iTot = fread(addr2, sizeof(u_char), 10, fd);
    printf("\nEthernet HW Addr & file Addr\n");
    for(i=0; i<6; ++i)
	printf("%2.2x", addr[i]);
    printf("      ");
    for(i=0; i<6; ++i)
	printf("%2.2x", addr2[i]);
    printf("\n");

    // check
    for(i=0; i<6; ++i)
    {
	if(addr[i] != addr2[i])
	{
	    printf("Eth addr does not match!\n");
	}
    }

    return SUCCESS;
}

/****/
int main()
{
    int fd;
    int iDone = 0, i, j=0, k=0;
    int iAMflag;
    long rc;
    char buffer[80];
    long lVPSdata[10];
    char c;

    fd = fopen("/root/controller/scmain/snpar.par","r");
    if(fd <= 0)
    {
	printf("file open error!\n");
	exit(0);
    }

    rc = readGA(fd);
    if(rc)
    {
	printf("file write error!\n");
	exit(0);
    }

    rc = readETH(fd);
    if(rc)
    {
	printf("file write error!\n");
	exit(0);
    }

    close(fd);

    return 0;

}
