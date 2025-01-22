#include <string.h>
#include <sys/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "otf.h"
#include "sck.h"
#include "alk.h"
#include "mapk.h"
#include "sctim.h"
#include "fiog.h"
#include "mapio.h"
#include "scintr.h"
#include "romain.h"
#include "ro.h"
#include "rofio.h"
#include "scio.h"
#include "fiol.h"
#include "scmem.h"
#include "dmclnx.h"
#include "gag.h"

#define NUMBER_OF_CTR_POINTS  6

extern HANDLEDMC ghDMC;

int	giOTFnum = 0;
int	iOTFFirstTime = FALSE;
int 	giOTFNullCount = 0;

//int NN[10][4] = { {4,0,0,2}, {5,1,1,2}, {0,2,2,3}, {1,2,2,3}, {2,3,3,5}, 
//		  {2,3,3,4}, {3,4,4,1}, {3,5,5,1}, {3,4,4,0} };
//int NN[10][4] = { {4,0,0,2}, {0,2,2,3}, {1,2,2,3}, 
//		  {2,3,3,4}, {3,4,4,1}, {3,4,4,0} };
//int NN[10][4] = { {4,0,0,2}, {5,1,1,2}, {0,2,2,3}, {1,2,2,3}, {2,3,3,5}, 
//		  {2,3,3,4}, {3,4,4,1}, {3,5,5,1}, {3,4,4,0} };
int NN[10][4] = { {0,2,2,5}, {5,3,3,0}, {1,2,2,5}, 
		  {3,4,4,0}, {1,2,2,3}, {3,4,4,1},
		  {3,4,4,1}, {3,5,5,1}, {3,4,4,0} };
int NS[10][4] = { {0,2,2,5}, {5,3,3,0}, {0,2,2,3}, 
		  {3,4,4,0}, {2,3,3,5}, {3,4,4,1},
		  {3,4,4,1}, {3,5,5,1}, {3,4,4,0} };

extern int 	giOTFCounter;
extern int	giSensorCount;
extern long 	glOTFData[MAX_OTF_INTERRUPTS];
extern int 	aiPortsMap[MAX_OTF_INTERRUPTS];
extern int	giPrevTint;
extern int	giPrevSensor;
extern int	giSensorMap[MAX_OTF_INTERRUPTS];

int MPConvertOTFData( );

char caXQOTFcommand[10] = "XQ#OTF3,7";
char caHXcommand[10]    = "HX7";
char caVCNTcommand[10]  = "MG VCNT";
char caVPS1command[10]  = "MG VPS1";
char caVPS2command[10]  = "MG VPS2";
char caVPS3command[10]  = "MG VPS3";
char caVPS4command[10]  = "MG VPS4";
char caVPS5command[10]  = "MG VPS5";
char caVPS6command[10]  = "MG VPS6";
char caVPS7command[10]  = "MG VPS7";
char caVPS8command[10]  = "MG VPS8";
char caVPS9command[10]  = "MG VPS9";
char caVP10command[10]  = "VPS1=0";
char caVP20command[10]  = "VPS2=0";
char caVP30command[10]  = "VPS3=0";
char caVP40command[10]  = "VPS4=0";
char caVP50command[10]  = "VPS5=0";
char caVP60command[10]  = "VPS6=0";
char caVP70command[10]  = "VPS7=0";
char caVP80command[10]  = "VPS8=0";
char caVP90command[10]  = "VPS9=0";
char caVC00command[10]  = "VCNT=0";

long glValidRPos = 0;
long glR200mm = 0;
int  giSmap[9] = {0,0,0,0,0,0,0,0,0};


psOTFStation pstHeadOTFStationsList = NULL;
psOTFStation pstOTFCurrentStation = NULL;
int iNoOfStations = 0, iOTFAligning, iOTFBufferOverflow = FALSE, iOTFInterruptsCounter = 0;
long fplOTFPosition[MAX_OTF_INTERRUPTS * 4];
int  fpiOTFSensorsBitmap[MAX_OTF_INTERRUPTS * 4];
float fComputedCentersX[10], fComputedCentersY[10];
float Xd[6], Yd[6];

/****************************************************************\
 *
 * Function:    MPSetOTFPositions
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPSetOTFPositions(int iArrayIndexArg, int iValueArg)
{
    long *fplTemp;

    fplTemp = fplOTFPosition;
    while(iArrayIndexArg > 0)
    {
        fplTemp++;
        iArrayIndexArg--;
    }
    *fplTemp = iValueArg;

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPGetOTFPositions
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPGetOTFPositions(long laXCoordinate[], long laYCoordinate[])
{
    int iCounter;
    long *fplTemp = fplOTFPosition;

    for(iCounter = 0; iCounter < MAX_OTF_INTERRUPTS; iCounter++)
    {
        laXCoordinate[iCounter] = *fplTemp;
        fplTemp++;
        laYCoordinate[iCounter] = Yd[iCounter];
    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPSetOTFPositions
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPSetOTFSensorMap(int iArrayIndexArg, int iValueArg)
{
    int *fpiTemp;

    fpiTemp = fpiOTFSensorsBitmap;
    while(iArrayIndexArg > 0)
    {
        fpiTemp++;
        iArrayIndexArg--;
    }

    *fpiTemp = iValueArg;

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPGetOTFSensorMap
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPGetOTFSensorMap(int iaSensorsBitMap[])
{
    int iCounter;
    int *fpiTemp = fpiOTFSensorsBitmap;

    for(iCounter = 0; iCounter < MAX_OTF_INTERRUPTS; iCounter++)
    {
        iaSensorsBitMap[iCounter] = *fpiTemp;
        fpiTemp++;
    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPGetOTFCenters
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     pointer
 *
\*****************************************************************/
int MPGetOTFCenters(float faXCentersCoordinates[], float faYCentersCoordinates[])
{
    int iCounter;

    for(iCounter = 0; iCounter < 6; iCounter++)
    {
        faXCentersCoordinates[iCounter] = fComputedCentersX[iCounter];
        faYCentersCoordinates[iCounter] = fComputedCentersY[iCounter];
    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPGetNumberOfOTFInterrupts
 *
 * Abstract:
 *
 * Parameters:  None
 *
 * Returns:     pointer
 *
\*****************************************************************/
int MPGetNumberOfOTFInterrupts(int *iNumOfOTFInterrupts)
{
    *iNumOfOTFInterrupts = iOTFInterruptsCounter;

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPGetOTFHeadStationsPointer
 *
 * Abstract:    Returns pointer to Head of OTF Stations list
 *
 * Parameters:  None
 *
 * Returns:     pointer
 *
\*****************************************************************/
psOTFStation MPGetOTFHeadStationsPointer()
{
    return pstHeadOTFStationsList;
}
/****************************************************************\
 *
 * Function:    MPGetOTFCurrentStationPointer
 *
 * Abstract:    Returns pointer to OTF Current Station
 * Parameters:  None
 *
 * Returns:     pointer
 *
\*****************************************************************/
psOTFStation MPGetOTFCurrentStationPointer()
{
    return pstOTFCurrentStation;
}
/****************************************************************\
 *
 * Function:    MPSetOTFHeadStationsPointer
 *
 * Abstract:    Sets pointer to Head of OTF Stations list to passed parameter
 *
 * Parameters:  Pointer to which Head of OTF Stations pointer will be set
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPSetOTFHeadStationsPointer(psOTFStation psPointerArg)
{
    pstHeadOTFStationsList = psPointerArg;
    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPSetOTFCurrentStationPointer
 *
 * Abstract:    Sets pointer to OTF Current Station to passed in the parameter
 * Parameters:  Pointer to which OTF Current stations pointer will be set
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPSetOTFCurrentStationPointer(psOTFStation psPointerArg)
{
    pstOTFCurrentStation = psPointerArg;
    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPAddNewOTFStation
 *
 * Abstract:    Adds one stations on the end of the OTF Stations list
 *              called from MPFindStation only
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPAddNewOTFStation (char cStationNameArg)
{
    psOTFStation pstTempOTFStation;

    pstTempOTFStation = pstHeadOTFStationsList;

    if (pstTempOTFStation != NULL)
    {
        //find the end of the list of the stations
        while (pstTempOTFStation->m_pstNextOTFStation != NULL)
            pstTempOTFStation = pstTempOTFStation->m_pstNextOTFStation;

        if (!(pstTempOTFStation->m_pstNextOTFStation = (stOTFStation *)malloc(sizeof(stOTFStation))))
            return FAILURE;

        memset(pstTempOTFStation->m_pstNextOTFStation, 0, sizeof(stOTFStation));
        pstTempOTFStation->m_pstNextOTFStation->m_pstNextOTFStation = NULL;
        pstTempOTFStation->m_pstNextOTFStation->m_cStationName = cStationNameArg;
        pstTempOTFStation->m_pstNextOTFStation->m_lSlotNumber = 1;
        memset(pstTempOTFStation->m_pstNextOTFStation->m_lOTFFinalCorrection, 0xFF, (sizeof(long)*NOFSLOTS*3));

        pstOTFCurrentStation = pstTempOTFStation->m_pstNextOTFStation;
        iNoOfStations++;
    }
    // no stations defined
    else
    {
        if(!(pstTempOTFStation = (stOTFStation *)malloc(sizeof(stOTFStation))))
            return FAILURE;

        pstHeadOTFStationsList = pstTempOTFStation;
        memset(pstTempOTFStation, 0, sizeof(stOTFStation));
        pstTempOTFStation->m_pstNextOTFStation = NULL;
        pstTempOTFStation->m_cStationName = cStationNameArg;
        pstTempOTFStation->m_lSlotNumber = 1;
        memset(pstTempOTFStation->m_lOTFFinalCorrection, 0xFF, (sizeof(long)*NOFSLOTS*3));

        pstOTFCurrentStation = pstTempOTFStation;
        iNoOfStations = 1;
    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPFindOTFStation
 *
 * Abstract:    Searches the OTF stations list for particular station
 *
 * Parameters:  cStationNameArg - name of the searched station
 *
 * Returns:     TRUE/FALSE = FOUND/NOT FOUND
 *
\*****************************************************************/
int MPFindOTFStation (char cStationNameArg, int iCreateNewFlag)
{
    stOTFStation *pstTempOTFStation;

    if (!(pstTempOTFStation = pstHeadOTFStationsList))
    {
        if(iCreateNewFlag)
        {
            MPAddNewOTFStation(cStationNameArg);
        }
        return FALSE;
    }
    for (;;)
    {
        if (pstTempOTFStation->m_cStationName == cStationNameArg)
            break;
        if (pstTempOTFStation->m_pstNextOTFStation == NULL)
        {
            pstOTFCurrentStation = NULL;
            if(iCreateNewFlag)
            {
                MPAddNewOTFStation(cStationNameArg);
            }
            return FALSE;
        }
        pstTempOTFStation = pstTempOTFStation->m_pstNextOTFStation;
    }
    pstOTFCurrentStation = pstTempOTFStation;

    return TRUE;
}
/****************************************************************\
 *
 * Function:    MPBitMapInit
 *
 * Abstract:
 *
 * Parameters:  iBitMap - bitmap of triggered sensors
 *              piSenArg   - ???
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
void MPBitMapInit(int *piSenArg, int iBitMapArg)
{
    int iCounter;

    for (iCounter = 0; iCounter < 8; iCounter++)
        piSenArg[iCounter] = 0;

    for (iCounter = 7; iCounter >= 0; iCounter--)
    {
        if (iBitMapArg >= (int)1 << iCounter)
        {
            piSenArg[iCounter] = 1;
            iBitMapArg -= (int)1 << iCounter;
        }
//printf("i=%d iBitMap=%d iSen[i]=%d\n",iCounter, iBitMapArg, piSenArg[iCounter]);
    }
}
/****************************************************************\
 *
 * Function:    MPSetOTFItem
 *
 * Abstract:    Sets OTF CSI Item
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPSetOTFItem(char cStationNameArg, long laValuesArg[])
{
    MPFindOTFStation(cStationNameArg, TRUE);

    switch ((int) laValuesArg[0])
    {
        case 0 :
            if ((laValuesArg[1] < 0) || (laValuesArg[1] > 2))
                return FAILURE;
            pstOTFCurrentStation->m_fOTFDistance[laValuesArg[1]] = (float)laValuesArg[2];
            break;
        case 1 :
            return FAILURE;
        case 2 :
            pstOTFCurrentStation->m_lEELength = laValuesArg[1];
            break;
        case 3 :
            if((laValuesArg[1] < 1) || (laValuesArg[1] > NOFSLOTS))
                return FAILURE;
            pstOTFCurrentStation->m_lSlotNumber = laValuesArg[1];
            break;
        case 4 :
            pstOTFCurrentStation->m_lRStartSampling = laValuesArg[1];
            break;
        case 5 :
            pstOTFCurrentStation->m_lROffset = laValuesArg[1];
            break;
        case 6 :
            if ((laValuesArg[1] < 0) || (laValuesArg[1] > 1))
                return FAILURE;
            pstOTFCurrentStation->m_lFinalPos[laValuesArg[1]] = laValuesArg[2];
            break;
        case 7 :
            if ((laValuesArg[1] != 2))
                return FAILURE;
            pstOTFCurrentStation->m_lOTFReserved[2] = laValuesArg[2];
            break;
        case 8 :
            return FAILURE;
        case 9 :
            if ((int) laValuesArg[1] < 1 || (int) laValuesArg[1] > NOFSLOTS)
                return FAILURE;
            if ((int) laValuesArg[2] < 0 || (int) laValuesArg[2] > 2)
                return FAILURE;
            pstOTFCurrentStation->m_lOTFFinalCorrection[(int) laValuesArg[1] - 1][(int) laValuesArg[2]] = laValuesArg[3];
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPReadOTFItem
 *
 * Abstract:    Reads OTF CSI Item
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPReadOTFItem(char cStationNameArg, long *plValuesArg)
{
    if(MPFindOTFStation(cStationNameArg, FALSE))
    {
        switch(plValuesArg[1])
        {
            case 0 :
                if ((plValuesArg[2] < 0) || (plValuesArg[2] > 2))
                    return FAILURE;
                plValuesArg[0] = (long)pstOTFCurrentStation->m_fOTFDistance[plValuesArg[2]];
                break;
            case 1 :
                plValuesArg[0] = (long)pstOTFCurrentStation->m_lOTFReserved[0];
                break;
            case 2 :
                plValuesArg[0] = (long)(pstOTFCurrentStation->m_lEELength);
                break;
            case 3 :
                plValuesArg[0] = (long)(pstOTFCurrentStation->m_lSlotNumber);
                break;
            case 4 :
                plValuesArg[0] = (long)(pstOTFCurrentStation->m_lRStartSampling);
                break;
            case 5 :
                plValuesArg[0] = (long)(pstOTFCurrentStation->m_lROffset);
                break;
            case 6 :
                if ((plValuesArg[2] < 0) || (plValuesArg[2] > 1))
                    return FAILURE;
                plValuesArg[0] = (long)(pstOTFCurrentStation->m_lFinalPos[plValuesArg[2]]);
                break;
            case 7 :
                if ((plValuesArg[2] < 0) || (plValuesArg[2] > 4))
                    return FAILURE;
                plValuesArg[0] = (long)pstOTFCurrentStation->m_lOTFReserved[plValuesArg[2]];
                break;
            case 8 :
                if ((plValuesArg[3] < 0) || (plValuesArg[3] > 1))
                    return FAILURE;
                if ((plValuesArg[2] < 1) || (plValuesArg[2] > NOFSLOTS))
                    return FAILURE;
                    plValuesArg[0] = (long)(pstOTFCurrentStation->m_fOTFRef[plValuesArg[2] - 1][plValuesArg[3]]*1000.0);
                break;
            case 9 :
                if ((plValuesArg[2] < 1) || (plValuesArg[2] > NOFSLOTS))
                    return FAILURE;
                if ((plValuesArg[3] < 0) || (plValuesArg[3] > 2))
                    return FAILURE;
                plValuesArg[0] = pstOTFCurrentStation->m_lOTFFinalCorrection[plValuesArg[2] -1][plValuesArg[3]];
                break;
            default:
                return FAILURE;
        }
    }
    else
        return FAILURE;

    return SUCCESS;
}

static  int iSen[12];
static  float XXXd[10], YYYd[10];
static	float x, y;
static  float fAverageCenterX, fAverageCenterY;
//static	float aa, bb;
static	float xc, yc;
static	double t0, r0, x01, y01, Xdd, Ydd, Xf, Yf;
static  double xx01, Yf2, yy01;

static int iConfidence;
static float fDistanceBetweenCenters, fMaxOffDistanceForCenter;
// sort
static  long lTemp, lAngle[NUMBER_OF_OTF_POINTS];
static  int No[NUMBER_OF_OTF_POINTS];
static	int *No0;
static double b, a1, a2;
/****************************************************************\
 *
 * Function:    MPAligning
 *
 * Abstract:    OTF aligning algorithm
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPAligning(double dTOffsetDestinationArg, double dROffsetDestinationArg, double dTOffsetSourceArg,
            int iCalibrateArg, char cSourceStationNameArg, int iIOShiftArg )
{
    long *lXPos = fplOTFPosition;
    int *iSensorMap = fpiOTFSensorsBitmap;
    int iMask, a, m, n, q, p1, p2, p3, p4;
    float x1, y1, x2, y2, x3, y3, x4, y4, aa1, aa2, b1, b2;
    float dx1, dy1, dx2, dy2, ddxy, dxy;
    double dEELength, dWaferRadiusSqr;
    int iChanged, iSlotIndex, iIndex, iSlotNum;
    long lWaferSize;
//printf("iCalib=%d ToffDest=%f RoffDest=%f ToffSource=%f source=%c shift=%d\n",iCalibrateArg, dTOffsetDestinationArg, dROffsetDestinationArg, dTOffsetSourceArg, cSourceStationNameArg, iIOShiftArg);


    cSourceStationNameArg = ((int)cSourceStationNameArg < 'a') ? ((int)cSourceStationNameArg - 'A') : ((int)cSourceStationNameArg - 'a' + 26);

    // find specified station
    if (!MPFindOTFStation(cSourceStationNameArg, FALSE))
    {
        pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -2;
        return SUCCESS;
    }

    if (MPConvertOTFData() == FAILURE) 
    {
        pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -3;
	return SUCCESS;
    }

    // user access slots from 1 to NOFSLOTS
    // internally we use array from 0 to NOFSLOTS-1
    iSlotNum = (int) pstOTFCurrentStation->m_lSlotNumber - 1;
    dEELength = (double) pstOTFCurrentStation->m_lEELength;

    // Project X Specific, if iCalibrateArg == 3, we copy information from slot 1 to other 25 slots
     if (iCalibrateArg == 3)
     {
        for (iSlotIndex = 1; iSlotIndex < NOFSLOTS; iSlotIndex++)
        {
            for (iIndex = 0; iIndex < NUMBER_OF_OTF_POINTS; iIndex++)
            {
               // Copy theSensor hysteresys compensation
               pstOTFCurrentStation->m_fOTFSensorComp[iSlotIndex][iIndex] = pstOTFCurrentStation->m_fOTFSensorComp[0][iIndex] ;
               // Copy the sorted index over as well
               pstOTFCurrentStation->m_iSortedIndex[iSlotIndex][iIndex] = pstOTFCurrentStation->m_iSortedIndex[0][iIndex];
            }
            pstOTFCurrentStation->m_fOTFRef[iSlotIndex][0] = pstOTFCurrentStation->m_fOTFRef[0][0];
            pstOTFCurrentStation->m_fOTFRef[iSlotIndex][1] = pstOTFCurrentStation->m_fOTFRef[0][1];
       }
       return SUCCESS;
     }

    // Initialize as Bad data first
    pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -1;

    // find out how many sensors were hit
//    for (n = 0; n < 10; n++)
//    {
//       if (iSensorMap[n] == 0)
//        break;
//    }

    // can assume iOTFInterruptsCounter is the # of valid data
    n = iOTFInterruptsCounter;

    // maximum distance the center can be off
    fMaxOffDistanceForCenter = (float)pstOTFCurrentStation->m_lOTFReserved[2];


    // get the X, Y coordinates of the points, roughly calculate center,
    // so we can sort the points by the angle to this center
    a = 0;
    iMask = 255;
    fAverageCenterX = 0.0f;
    fAverageCenterY = 0.0f;
    No0 = pstOTFCurrentStation->m_iSortedIndex[iSlotNum];
    pstOTFCurrentStation->m_lOTFReserved[0] = 0;

    for (m = 0; m < n; m++)
    {
        // shift the data if from second bracket
        iSensorMap[m] >>= iIOShiftArg;
        iSensorMap[m] &= 7;
        MPBitMapInit(iSen, (iSensorMap[m]^iMask));
        iMask = iSensorMap[m];
//printf("m=%d iSmap[m]=%d iMask=%d\n",m,iSensorMap[m],iMask);

        for (q = 0; q < 3; q++)
        {
            if (a >= NUMBER_OF_OTF_POINTS)
                break;
            if (iSen[q] == 1)
            {
                x = (float)lXPos[m];
                switch (q)
                {
                    case 0:
                        // Y position of 1. sensor
                        y = pstOTFCurrentStation->m_fOTFDistance[0];
                        break;
                    case 1:
                        // position of 2. sensor
                        y = pstOTFCurrentStation->m_fOTFDistance[1];
                        break;
                    case 2:
                        // position of 3. sensor
                        y = pstOTFCurrentStation->m_fOTFDistance[2];
                    break;
                }
                Xd[a] = x;
                Yd[a] = y;
                fAverageCenterX += x;
                fAverageCenterY += y;
                a++;
//printf("m=%d a=%d q=%d sen=%d x=%f y=%f\n",m,a,q,iSen[q],x,y);
            }
        }
    }

    // if no data caught on sensors, quit
    if (a == 0)
    {
        pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -2;
        return SUCCESS;
    }

    // compute the average, rough center
    fAverageCenterX /= a;
    fAverageCenterY /= a;
//printf("aveCenterX=%f aveCenterY=%f\n",fAverageCenterX,fAverageCenterY);

    // Sorting the points in order by the angle
    for (n = 0; n < NUMBER_OF_OTF_POINTS; n++)
    {
        lAngle[n] = (long)(atan2(Yd[n] - fAverageCenterY, Xd[n] - fAverageCenterX)*180/PIDEF);
//        lAngle[n] = (long)(atan2(Yd[n] - fAverageCenterY, fAverageCenterX - Xd[n])*180/PIDEF); // retracting positive direction
        No[n] = n;
//		ulTimeStartOld = ulTimeStart;
//printf("n=%d Yd=%f Xd=%f Ang=%d\n",n,Yd[n],Xd[n],lAngle[n]);
    }

    for (;;)
    {
        iChanged = FALSE;
        for (n = 0; n < 5; n++)
        {
            if (lAngle[n] < lAngle[n + 1])
            {
                iChanged = TRUE;
                lTemp = lAngle[n];
                lAngle[n] = lAngle[n + 1];
                lAngle[n + 1] = lTemp;
                lTemp = (long)No[n];
                No[n] = No[n + 1];
                No[n + 1] = (int)lTemp;
           }
        }
        if (!iChanged) break;
    }
    // Sorting finished
//printf("sorted: No[n] = %d %d %d %d %d %d\n",No[0],No[1],No[2],No[3],No[4],No[5]);

    //  iCalibrateArg == 1 we are calibrating the first slot, we clear the Sensor Compensation for all the slots
    if (iCalibrateArg == 1)
    {
        for(m = 0; m < NOFSLOTS; m++)
            for(n = 0; n < NUMBER_OF_OTF_POINTS; n++)
                pstOTFCurrentStation->m_fOTFSensorComp[m][n] = 0;
    }
    // for all calibration modes, iCalibrateArg = 1, 2 or 3
    if(iCalibrateArg)
    {
        for(n = 0; n < NUMBER_OF_OTF_POINTS; n++)
            No0[n] = No[n];
    }

    // adjust for sensor compensation
    for(n = 0 ; n < NUMBER_OF_OTF_POINTS ; n++)
    {
        XXXd[No[n]] = Xd[No[n]] - pstOTFCurrentStation->m_fOTFSensorComp[iSlotNum][No0[n]];
//        XXXd[No[n]] = Xd[No[n]];
        YYYd[No[n]] = Yd[No[n]];
    }

//printf("SensorCompensated\n");
//for (n=0; n<6; ++n)
//  printf("    Xd[%d]=%f Yd[%d]=%f\n",n, XXXd[n], n, YYYd[n]);

    fAverageCenterX = 0.0f;
    fAverageCenterY = 0.0f;

    // calculate the centers
    // 0-1, 1-3, ... chords make center, we compute 6 centers
    if(giOTFnum == 0) // Original Center finding computation
    {
	for (n = 0; n < NUMBER_OF_CTR_POINTS; n++)
    	{
//            p1 = No[n % NUMBER_OF_CTR_POINTS];
//            p2 = No[(n+1) % NUMBER_OF_CTR_POINTS];
//            p3 = No[(n+1) % NUMBER_OF_CTR_POINTS];
//            p4 = No[(n+2) % NUMBER_OF_CTR_POINTS];

	    if(lXPos[1] == lXPos[2])
	    {
	    	p1 = NS[n][0];
	    	p2 = NS[n][1];
	    	p3 = NS[n][2];
	    	p4 = NS[n][3];
	    }
	    else
	    {
	    	p1 = NN[n][0];
	    	p2 = NN[n][1];
	    	p3 = NN[n][2];
	    	p4 = NN[n][3];
	    }

            x1 = XXXd[p1];
            y1 = YYYd[p1];
            x2 = XXXd[p2];
            y2 = YYYd[p2];
            x3 = XXXd[p3];
            y3 = YYYd[p3];
            x4 = XXXd[p4];
            y4 = YYYd[p4];

            if ((x1 != x2) && (x3 != x4))
            {
                aa1 = (y1 - y2)/(x2 - x1);
            	aa2 = (y3 - y4)/(x4 - x3);
            	b1 = (x1 + x2)/2 -(y1 - y2)*(y1 + y2)/(2*(x2 - x1));
            	b2 = (x3 + x4)/2 - (y3 - y4)*(y3 + y4)/(2*(x4 - x3));
            	if (aa1 != aa2)
            	{
                    yc = (b2 - b1)/(aa1 - aa2);
                    xc = aa1*yc + b1;
            	}
            }
            else if ((x1 != x2) && (x3 == x4))
            {
            	yc =(y3 + y4)/2;
            	aa1 = (y1 - y2)/(x2 - x1);
            	b1 = (x1 + x2)/2 - (y1 - y2)*(y1 + y2)/(2*(x2 - x1));
            	xc = aa1*yc + b1;
            }
            else if ((x1 == x2) && (x3 != x4))
            {
            	yc =(y1 + y2)/2;
            	aa2 = (y3 - y4)/(x4 - x3);
            	b2 = (x3 + x4)/2 - (y3 - y4)*(y3 + y4)/(2*(x4 - x3));
            	xc = aa2*yc + b2;
            }

            fAverageCenterX += xc;
            fAverageCenterY += yc;

            // RC stores the coordinates for each computed center
            fComputedCentersX[n] = xc;
            fComputedCentersY[n] = yc;
//printf("CenterComp: n=%d p1=%d p2=%d p3=%d p4=%d xc=%f yc=%f\n",n,p1,p2,p3,p4,xc,yc);
        }
    }
    else // new center finding computation
    {
     	for (n = 0; n < NUMBER_OF_CTR_POINTS; n++)
    	{
//            p1 = No[n % NUMBER_OF_CTR_POINTS];
//            p2 = No[(n+1) % NUMBER_OF_CTR_POINTS];
//            p3 = No[(n+1) % NUMBER_OF_CTR_POINTS];
//            p4 = No[(n+2) % NUMBER_OF_CTR_POINTS];

	    p1 = NN[n][0];
	    p2 = NN[n][1];
	    p3 = NN[n][2];
	    p4 = NN[n][3];

            x1 = (XXXd[p1] + XXXd[p2]) / 2;
            y1 = (YYYd[p1] + YYYd[p2]) / 2;
            x2 = (XXXd[p3] + XXXd[p4]) / 2;
            y2 = (YYYd[p3] + YYYd[p4]) / 2;
	    dy1 = XXXd[p2] - XXXd[p1];
	    dx1 = YYYd[p1] - YYYd[p2];
	    dy2 = XXXd[p4] - XXXd[p3];
	    dx2 = YYYd[p3] - YYYd[p4];

//ddxy = (YYYd[p1] - YYYd[p2])/(XXXd[p1] - XXXd[p2]);
//dxy  = -1.0/ddxy;
//printf("p1=%d p2=%d slope p1 to p2 = %f, inverse = %f\n", p1, p2, ddxy, dxy);

            if ((x1 != x2) && (x3 != x4))
            {
                aa1 = (y1 - y2)/(x2 - x1);
            	aa2 = (y3 - y4)/(x4 - x3);
            	b1 = (x1 + x2)/2 -(y1 - y2)*(y1 + y2)/(2*(x2 - x1));
            	b2 = (x3 + x4)/2 - (y3 - y4)*(y3 + y4)/(2*(x4 - x3));
            	if (aa1 != aa2)
            	{
                    yc = (b2 - b1)/(aa1 - aa2);
                    xc = aa1*yc + b1;
            	}
            }
            else if ((x1 != x2) && (x3 == x4))
            {
            	yc =(y3 + y4)/2;
            	aa1 = (y1 - y2)/(x2 - x1);
            	b1 = (x1 + x2)/2 - (y1 - y2)*(y1 + y2)/(2*(x2 - x1));
            	xc = aa1*yc + b1;
            }
            else if ((x1 == x2) && (x3 != x4))
            {
            	yc =(y1 + y2)/2;
            	aa2 = (y3 - y4)/(x4 - x3);
            	b2 = (x3 + x4)/2 - (y3 - y4)*(y3 + y4)/(2*(x4 - x3));
            	xc = aa2*yc + b2;
            }

	    ddxy = dx1*dy2 - dy1*dx2;
   	    if (ddxy != 0.0)
	    	xc = (y1*dx1*dx2 + x2*dx1*dy2 - x1*dy1*dx2 - y2*dx1*dx2) / ddxy;
	    else
	    	xc = 9999999.99;

	    dxy = dx1 + y1;
	    if (dxy != 0.0)
	    	yc = (xc - x1) * dy1 / dxy;
	    else
	    	yc = 9999999.99;

            fAverageCenterX += xc;
            fAverageCenterY += yc;

            // RC stores the coordinates for each computed center
            fComputedCentersX[n] = xc;
            fComputedCentersY[n] = yc;
        }
    }

//int i;
//for (i=0; i<NUMBER_OF_CTR_POINTS; ++i)
//    printf("center=%d, (%f, %f)\n",i, fComputedCentersX[i], fComputedCentersY[i]);

    // if iCalibrate != 0 we are calibrating
    if (iCalibrateArg !=0 )
    {
        pstOTFCurrentStation->m_fOTFRef[iSlotNum][0] = fAverageCenterX/NUMBER_OF_CTR_POINTS;
        pstOTFCurrentStation->m_fOTFRef[iSlotNum][1] = fAverageCenterY/NUMBER_OF_CTR_POINTS;

//printf("oRefT=%f oRefR=%f\n",pstOTFCurrentStation->m_fOTFRef[iSlotNum][0],pstOTFCurrentStation->m_fOTFRef[iSlotNum][1]);

        if (iCalibrateArg)
        {
            // find offsets so the points would fit into circle with middle in average center computed
            for (n = 0; n < NUMBER_OF_OTF_POINTS; n++)
            {
                x = Xd[No0[n]];
                y = Yd[No0[n]];
                xc = fAverageCenterX/NUMBER_OF_CTR_POINTS;
                yc = fAverageCenterY/NUMBER_OF_CTR_POINTS;
                if(FIOGetParamWaferVals(WAFER_SIZE, &lWaferSize) == FAILURE)
                    return FAILURE;
                // compute wafer radius sqare
                dWaferRadiusSqr =  pow( (double)lWaferSize/(MM_TO_INCHES*2), 2.0 )*1000000.0;
                b = sqrt(dWaferRadiusSqr - (y - yc)*(y - yc));
                a1 = x - xc - b;
                a2 = x - xc + b;
                if (fabs(a1) < fabs(a2))
                    pstOTFCurrentStation->m_fOTFSensorComp[iSlotNum][No0[n]] = (float)a1;
                else
                    pstOTFCurrentStation->m_fOTFSensorComp[iSlotNum][No0[n]] = (float)a2;
//printf("sComp n=%d No0=%d b=%f dWrad=%f a1=%f a2=%f\n",n, No0[n], b, dWaferRadiusSqr, a1, a2);
//printf("      x=%f y=%f xc=%f yc=%f\n",x,y,xc,yc);
           }
        }

        // compute confidence, each center has to be under maximum distance with the rest 5 centers(fMaxOffDistanceForCenter) to qualify
        // center gain weight for each qualification, ideal calibration gives the 5x6 = 30 number
        iConfidence = 0;
        for (n = 0; n < NUMBER_OF_CTR_POINTS; n++)
        {
            for (m = 0; m < NUMBER_OF_CTR_POINTS; m++)
            {
                if( m != n )
                {
                    fDistanceBetweenCenters = (fComputedCentersX[n] - fComputedCentersX[m])*(fComputedCentersX[n] - fComputedCentersX[m])
                             + (fComputedCentersY[n] - fComputedCentersY[m])* (fComputedCentersY[n] - fComputedCentersY[m]);
                    if (fDistanceBetweenCenters < fMaxOffDistanceForCenter)
                    {
                        iConfidence++;
                    }
                }
            }
        }
        pstOTFCurrentStation->m_lOTFReserved[0] = iConfidence;
    }
    // iCalibrateArg == 0 we are in aligning algorithm
    else
    {
        for (n = 0; n < NUMBER_OF_CTR_POINTS; n++)
            iSen[n]=0;

        // compute confidence, each center has to be under maximum distance with the rest 5 centers(fMaxOffDistanceForCenter) to qualify
        // center gain weight for each qualification, ideal alignment gives the 5x6 = 30 number
        iConfidence = 0;
        for (n = 0; n < NUMBER_OF_CTR_POINTS; n++)
        {
            for (m = 0; m < NUMBER_OF_CTR_POINTS; m++)
            {
                if(m != n)
                {
                    fDistanceBetweenCenters = (fComputedCentersX[n] - fComputedCentersX[m])*(fComputedCentersX[n] - fComputedCentersX[m])
                             + (fComputedCentersY[n] - fComputedCentersY[m])* (fComputedCentersY[n] - fComputedCentersY[m]);
                    if (fDistanceBetweenCenters < fMaxOffDistanceForCenter)
                    {
                        iSen[n]++;
                        iConfidence++;
//printf("n=%d m=%d iSen[n]=%d iConfid=%d\n",n,m,iSen[n],iConfidence);
                    }
		    else
		    {
//printf("n=%d m=%d xn=%f xm=%f yn=%f ym=%f fmax=%f fDis=%f\n",n,m,fComputedCentersX[n],fComputedCentersX[m],fComputedCentersY[n],fComputedCentersY[m],fMaxOffDistanceForCenter,fDistanceBetweenCenters);
		    }
                }
            }
        }

        m = 0;
        fAverageCenterX = 0.0f;
        fAverageCenterY = 0.0f;
        pstOTFCurrentStation->m_lOTFReserved[0] = iConfidence;

        // we calculate the average accurate center from all centers which have qualified,
        // we consider only centers which has confidence level more than 1 meaninf the center is within distance at least
        // with two other centers
        for (n = 0; n < NUMBER_OF_CTR_POINTS; n++)
        {
            if (iSen[n] > 1)
            {
                m++;
                fAverageCenterX += fComputedCentersX[n];
                fAverageCenterY += fComputedCentersY[n];
            }
        }

        // if we couldn't find average center, return error
        if (m == 0)
        {
//printf("conf=%d sen=%d %d %d %d %d %d\n",iConfidence,iSen[0],iSen[1],iSen[2],iSen[3],iSen[4],iSen[5]);

            pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -1;
            return SUCCESS;
        }

        fAverageCenterX /= m;
        fAverageCenterY /= m;

        // compute the final correction for the T, and R axes
        r0 = sqrt((fAverageCenterX - pstOTFCurrentStation->m_fOTFRef[iSlotNum][0])*(fAverageCenterX - pstOTFCurrentStation->m_fOTFRef[iSlotNum][0])
                   + (fAverageCenterY - pstOTFCurrentStation->m_fOTFRef[iSlotNum][1])*(fAverageCenterY - pstOTFCurrentStation->m_fOTFRef[iSlotNum][1]));
        if (r0 != 0)
            t0 = atan2(fAverageCenterY - pstOTFCurrentStation->m_fOTFRef[iSlotNum][1], fAverageCenterX - pstOTFCurrentStation->m_fOTFRef[iSlotNum][0]);
        else
            t0 = 0.0f;

//printf("r0=%f maxOff=%f\n", r0, fMaxOffDistanceForCenter);
//printf("fAveX=%f fAveY=%f r0=%f t0=%f\n",fAverageCenterX, fAverageCenterY, r0, t0);

        pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = (long)r0;

/********** ORIGINAL COMPUTATION FOR FINDING FINAL POS ***********************/

//        if(dROffsetDestinationArg < 0.0) // needed to check if taught R pos is negative
//        {
//        y01 = r0 * sin((dTOffsetDestinationArg-dTOffsetSourceArg)*PIDEF/18000 - t0);
//        x01 = r0 * cos((dTOffsetDestinationArg-dTOffsetSourceArg)*PIDEF/18000 - t0);
//        Xdd = (dROffsetDestinationArg-dEELength) * cos((dTOffsetDestinationArg-dTOffsetSourceArg) * PIDEF / 18000);
//        Ydd = (dROffsetDestinationArg-dEELength) * sin((dTOffsetDestinationArg-dTOffsetSourceArg) * PIDEF / 18000);
//	}
//	else
//	{
        y01 = r0 * sin((dTOffsetDestinationArg-dTOffsetSourceArg)*PIDEF/18000 - t0);
        x01 = r0 * cos((dTOffsetDestinationArg-dTOffsetSourceArg)*PIDEF/18000 - t0);
        Xdd = (dROffsetDestinationArg+dEELength) * cos((dTOffsetDestinationArg-dTOffsetSourceArg) * PIDEF / 18000);
        Ydd = (dROffsetDestinationArg+dEELength) * sin((dTOffsetDestinationArg-dTOffsetSourceArg) * PIDEF / 18000);
//	}

        Xf = Xdd + x01;
        Yf = Ydd + y01;

//printf("x01=%f y01=%f Xdd=%f Ydd=%f Xf=%f Yf=%f\n",x01, y01, Xdd, Ydd, Xf, Yf);

        pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][1] = pstOTFCurrentStation->m_lFinalPos[1] = (long)(sqrt (Xf * Xf + Yf * Yf) - dEELength + 0.5);
        pstOTFCurrentStation->m_lFinalPos[0] = (long)((atan2(Yf, Xf) * 18000.0f / PIDEF + 0.5)+dTOffsetSourceArg);

	// MAKE SURE WE DON'T PUT
//        pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -1;

/********************************************************************************/


/********** SIMPLE COMPUTATION FOR FINDING FINAL DESTINATION ****************
	// MAKE IT SIMPLE FINDING FINAL POS
//	x01 = pstOTFCurrentStation->m_fOTFRef[iSlotNum][0] - fAverageCenterX;
//
//  	y01 = pstOTFCurrentStation->m_fOTFRef[iSlotNum][1] - fAverageCenterY;

	x01 = fAverageCenterX - pstOTFCurrentStation->m_fOTFRef[iSlotNum][0];

  	y01 = fAverageCenterY - pstOTFCurrentStation->m_fOTFRef[iSlotNum][1];
	if(dROffsetDestinationArg < 0.0)
		Ydd = dEELength - dROffsetDestinationArg;
	else 
		Ydd = dEELength + dROffsetDestinationArg;

//	Yf  = atan2(y01, Ydd) * 18000.0f / PIDEF + 0.5;	// converted to 0.01 degree
	Yf2 = atan2(y01, Ydd);
//	Yf2 = asin(y01 / Ydd);
	Yf  = Yf2 * 18000.0f / PIDEF + 0.5;	// converted to 0.01 degree

// additional compensation computation for R axis
	xx01 = y01 * sin(Yf2);

// use it to compensate for T axis
	yy01 = atan2(xx01, Ydd);
	Yf2 = Yf2 + yy01;
	Yf = Yf2 * 18000.0f / PIDEF + 0.5;


        pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][1] = pstOTFCurrentStation->m_lFinalPos[1] = (long)(dROffsetDestinationArg + x01 + xx01);
        pstOTFCurrentStation->m_lFinalPos[0] = (long)(dTOffsetDestinationArg - Yf);

//printf("x01=%f xx01=%f y01=%f yy01=%f Yf=%f Ydd=%f\n",x01, xx01, y01, yy01, Yf, Ydd);
	// MAKE SURE WE DON'T PUT
        pstOTFCurrentStation->m_lROffset = pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][2] = -1;

/******************************************************************************/


        // Check for the boundary of T, T should be between 0 to 360 degrees 3/25/98  see ABED's note
        while (pstOTFCurrentStation->m_lFinalPos[0] < 0)
        {
            pstOTFCurrentStation->m_lFinalPos[0] += 36000L;
        }
        while(pstOTFCurrentStation->m_lFinalPos[0] > 36000L)
        {
            pstOTFCurrentStation->m_lFinalPos[0] -= 36000L;
        }
        pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][0] = pstOTFCurrentStation->m_lFinalPos[0];

//printf("finalT=%d finalR=%d\n",pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][0],pstOTFCurrentStation->m_lOTFFinalCorrection[iSlotNum][1]);

    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPPrepareOTFAligning
 *
 * Abstract:
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPPrepareOTFAligning()
{
    char caResp[20];
    int rc, iOutputL;
    long laPos[8]={0,0,0,0,0,0,0,0}, laPosUnscale[8]={0,0,0,0,0,0,0,0};
    long lExtendPos;

    rc = GASendDMCCommand(ghDMC, caXQOTFcommand, caResp, 20);

    iOutputL = inb(IO_PRE_OUTPUT_L);

    iOutputL &= 0x8F;
    iOutputL |= 0x70;		// Select A bracket for OTF
//  iOutputL |= 0x60;		// Select B bracket for OTF

    outb(iOutputL, IO_PRE_OUTPUT_L); // This sends latch signal to Galil

    if(!(memset(fplOTFPosition, 0, (size_t)(sizeof(long)*(MAX_OTF_INTERRUPTS)))))
        return FAILURE;
    if(!(memset(fpiOTFSensorsBitmap, 0, (size_t)(sizeof(int)*(MAX_OTF_INTERRUPTS)))))
        return FAILURE;

    if(FIOGetStnCoord(0, 1, &lExtendPos)==FAILURE) // Station A extend position
    {
    	if(FIOGetStnCoord(1, 1, &lExtendPos)==FAILURE) // Station B extend position
	    return FAILURE;
    }

    laPos[1] = lExtendPos - 500; // 0.5" backward
    ROUnscalePos(RO_AXIS_R, laPos, laPosUnscale);
    glValidRPos = laPosUnscale[1];
    laPos[1] = 8000;	// 8" = 200mm
    ROUnscalePos(RO_AXIS_R, laPos, laPosUnscale);
    glR200mm = laPosUnscale[1];

//    rc = GASendDMCCommand(ghDMC, caHXcommand, caResp, 20);
    

//    iOTFBufferOverflow = FALSE;
    iOTFInterruptsCounter = 0;
    giOTFCounter = 0;
    giSensorCount = 0;
//    giPrevTint = 7;   // dark-ON
    giPrevTint = 0; // light-ON
//    giPrevSensor = inb( IO_ROBOT_INPUT_H );
    giPrevSensor = 0xFF;
//    giPrevSensor &= 0x07;

//    NN[0][0]=4; NN[0][1]=0; NN[0][2]=0; NN[0][3]=2;
//    NN[1][0]=5; NN[1][1]=1; NN[1][2]=1; NN[1][3]=2;
//    NN[2][0]=0; NN[2][1]=2; NN[2][2]=2; NN[2][3]=3;
//    NN[3][0]=1; NN[3][1]=2; NN[3][2]=2; NN[3][3]=3;
//    NN[4][0]=2; NN[4][1]=3; NN[4][2]=3; NN[4][3]=5;
//    NN[5][0]=2; NN[5][1]=3; NN[5][2]=3; NN[5][3]=4;
//    NN[6][0]=3; NN[6][1]=4; NN[6][2]=4; NN[6][3]=1;
//    NN[7][0]=3; NN[7][1]=5; NN[7][2]=5; NN[7][3]=1;
//    NN[8][0]=3; NN[8][1]=4; NN[8][2]=4; NN[8][3]=0;

    iOTFAligning = TRUE;
    iOTFFirstTime = TRUE;
    giOTFNullCount = 0;


    // Set all VPS's to 0 in Galil before XQ#OTF2
//    rc = GASendDMCCommand(ghDMC, caVC00command, caResp, 80);
//    rc = GASendDMCCommand(ghDMC, caVP10command, caResp, 80);
//    rc = GASendDMCCommand(ghDMC, caVP20command, caResp, 80);
//    rc = GASendDMCCommand(ghDMC, caVP30command, caResp, 80);
//    rc = GASendDMCCommand(ghDMC, caVP40command, caResp, 80);
//    rc = GASendDMCCommand(ghDMC, caVP50command, caResp, 80);
//    rc = GASendDMCCommand(ghDMC, caVP60command, caResp, 80);


//    MPEnableOTFInterrupts();


    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPQuitOTFAligning
 *
 * Abstract:
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPQuitOTFAligning(int *piOTFBufferOverflowArg)
{
    int iOutputL;

//    giIOCountMax = 1;

    int rc;
    char caResp[80];
//printf("giOTFNullCount=%d\n",giOTFNullCount);
    iOTFFirstTime = FALSE;
    iOTFAligning = FALSE;

    rc = GASendDMCCommand(ghDMC, caHXcommand, caResp, 80);
//
// Now change the latch input signal to Extra sensor
//
    iOutputL = inb(IO_PRE_OUTPUT_L);
    iOutputL &= 0x8F;
    iOutputL |= 0x40;
//    iOutputL &= 0xDF;		// turn 6th bit to 0 (Drift Latch select)
//    iOutputL &= 0xBF;		// turn 6th bit to 0 (Drift Latch select)
				// 5th bit remains 0 (bracket A select)
    outb(iOutputL, IO_PRE_OUTPUT_L); // This sends latch signal to Galil

//    MPDisableOTFInterrupts();
    *piOTFBufferOverflowArg = iOTFBufferOverflow;

    rc = GASendDMCCommand(ghDMC, "ALB", caResp, 80);

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPEnableOTFInterrupts
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int  MPEnableOTFInterrupts(void)
{

    
//    iSOP = inb(SCANNEROUTPORT);
//    outb(iSOP&0xDF, SCANNEROUTPORT);

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPDisableOTFInterrupts
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPDisableOTFInterrupts(void)
{
    int iSOP;

    if(iOTFAligning == TRUE)
    {
        iOTFAligning = FALSE;
//	iSOP = inb(SCANNEROUTPORT);
//        outb(iSOP|0x20, SCANNEROUTPORT);
    }
    else
        return FAILURE;

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPCollectOTFData
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPCollectOTFData(int aiCustomerPortBitMapArg[])
{
    // In the OTF macros we clear the I/O encoder count reigters at the beginning of the aligning,
    // this way we lose the Customer Home position, so we add the R-Customized Home position to every
    // R position read from I/O encoder registers (lGalilIODiff)
    long lGalilIODiff, lRPos, lTemp1 = 0, lTemp2 = 0;
    long laUnscaledTemp[4]={0,0,0,0}, laScaledTemp[4]={0,0,0,0};
    char *pcRPos;
    int iCounter;


    if(ROGetParameter(TRUE, (ULONG)RO_AXIS_R, laScaledTemp, CUSTOMIZED_HOME) == FAILURE)
        return FAILURE;
    if(ROUnscalePos((ULONG)RO_AXIS_R, laScaledTemp, laUnscaledTemp) == FAILURE)
        return FAILURE;
    lGalilIODiff = laUnscaledTemp[1];

    iCounter = inb(SCANNEROUTPORT);
    outb(iCounter|0x40, SCANNEROUTPORT);

    pcRPos = (char *)&lRPos;
    pcRPos[0] = inb(0x34D);
    pcRPos[1] = inb(0x34E);
    pcRPos[2] = inb(0x34F);
    pcRPos[3] = pcRPos[2]&0x80 ? 0xFF : 0;

    if (iOTFInterruptsCounter >= MAX_OTF_INTERRUPTS)
    {
        iOTFBufferOverflow = TRUE;
    }
    else
    {
        // Merge the 8 bits from each port into one long variable
        for(iCounter = OTF_CUSTOMER_PORTS - 1; iCounter >= 0; iCounter--)
        {
            lTemp1 = (long) aiCustomerPortBitMapArg[iCounter];
            lTemp1 = lTemp1 << 8*iCounter;
            lTemp2 |= lTemp1;
        }

        laUnscaledTemp[1] = lRPos + lGalilIODiff;
        if(ROScalePos((ULONG)RO_AXIS_R, laUnscaledTemp, laScaledTemp) == FAILURE)
            return FAILURE;
        fplOTFPosition[iOTFInterruptsCounter] = laScaledTemp[1];
        fpiOTFSensorsBitmap[iOTFInterruptsCounter++] = lTemp2;
    }

    iCounter = inb(SCANNEROUTPORT);
    outb(iCounter&0xDF, SCANNEROUTPORT);

    return SUCCESS;
}

int MPConvertOTFData( )
{
    char caResp[80];
    int i, j, rc;
    int nVcnt;
    long lVPS1, lVPS2, lVPS3, lVPS4, lVPS5, lVPS6, lVPS7, lVPS8, lVPS9;
    long lRposLast;

    long lGalilIODiff, lRPos, lTemp1 = 0, lTemp2 = 0;
    long laUnscaledTemp[8]={0,0,0,0,0,0,0,0}, laScaledTemp[8]={0,0,0,0,0,0,0,0};
    int  nGoodCnt;
    int iSenMap[18];
    int nSenCnt;

    // just in case, halt the galil macro thread
//    rc = GASendDMCCommand(ghDMC, caHXcommand, caResp, 80);


    for (i=0;i<giSensorCount;++i) 
    {
	iSenMap[i] = ~giSensorMap[i] & 0x07;
//printf("sensorMap %d = %d\n",i,iSenMap[i]);
    }

    lVPS1 = lVPS2 = lVPS3 = lVPS4 = lVPS5 = lVPS6 = lVPS7 = lVPS8 = lVPS9 = 0;
    rc = GASendDMCCommand(ghDMC, caVCNTcommand, caResp, 80);
    nVcnt = atoi(caResp);

////printf("MPConvertOTFData() working... nVcnt=%d\n", nVcnt );
//printf("nVcnt=%d glValidRPos=%d R200mm=%d\n",nVcnt,glValidRPos,glR200mm);

    if(nVcnt < 5) 
	return FAILURE;

    nGoodCnt = 0;
    nSenCnt = 0;
    rc = GASendDMCCommand(ghDMC, caVPS1command, caResp, 80);
    lVPS1 = atol(caResp);
//printf("vps1=%d\n",lVPS1);
    if (lVPS1 < glValidRPos)
    {
	giSmap[nGoodCnt] = iSenMap[nSenCnt];
	glOTFData[nGoodCnt++] = lVPS1;
    }
    nSenCnt++;

    rc = GASendDMCCommand(ghDMC, caVPS2command, caResp, 80);
    lVPS2 = atol(caResp);
//printf("vps2=%d\n",lVPS2);
    if (lVPS2 < glValidRPos && labs(lVPS1-lVPS2)>1500)
    {
	if(iSenMap[nSenCnt] == 0) ++nSenCnt;
        giSmap[nGoodCnt] = iSenMap[nSenCnt];
    	glOTFData[nGoodCnt++] = lVPS2;
    }
    nSenCnt++;

    rc = GASendDMCCommand(ghDMC, caVPS3command, caResp, 80);
    lVPS3 = atol(caResp);
//printf("vps3=%d\n",lVPS3);
    if (lVPS3 < glValidRPos && labs(lVPS2-lVPS3)>1500)
    {
        giSmap[nGoodCnt] = iSenMap[nSenCnt];
	glOTFData[nGoodCnt++] = lVPS3;
    }
    nSenCnt++;

    rc = GASendDMCCommand(ghDMC, caVPS4command, caResp, 80);
    lVPS4 = atol(caResp);
//printf("vps4=%d\n",lVPS4);
    if (lVPS4 < glValidRPos && nVcnt > 3 && labs(lVPS3-lVPS4)>1500)
    {
        giSmap[nGoodCnt] = iSenMap[nSenCnt];
	glOTFData[nGoodCnt++] = lVPS4;
    }
    nSenCnt++;

    rc = GASendDMCCommand(ghDMC, caVPS5command, caResp, 80);
    lVPS5 = atol(caResp);
//printf("vps5=%d\n",lVPS5);
    if (lVPS5 < glValidRPos && nVcnt > 4 && labs(lVPS4-lVPS5)>1500)
    {
        giSmap[nGoodCnt] = iSenMap[nSenCnt];
	glOTFData[nGoodCnt++] = lVPS5;
    }
    nSenCnt++;

    if(nGoodCnt < 1)
	return FAILURE;

    lRposLast = glOTFData[0] - glR200mm - glR200mm/10;

    if(nVcnt > 5)
    {
    	rc = GASendDMCCommand(ghDMC, caVPS6command, caResp, 80);
    	lVPS6 = atol(caResp);
//printf("vps6=%d\n",lVPS6);
    	if (lVPS6 < glValidRPos && lVPS6 > lRposLast && labs(lVPS5-lVPS6)>1500)
	{
	    giSmap[nGoodCnt] = iSenMap[nSenCnt];
	    glOTFData[nGoodCnt++] = lVPS6;
	}
    }
    nSenCnt++;

    if(nVcnt > 6)
    {
    	rc = GASendDMCCommand(ghDMC, caVPS7command, caResp, 80);
    	lVPS7 = atol(caResp);
//printf("vps7=%d\n",lVPS7);
    	if (lVPS7 != 0 && nGoodCnt < 6 && lVPS7 > lRposLast && labs(lVPS6-lVPS7)>1500)
	{
	    giSmap[nGoodCnt] = iSenMap[nSenCnt];
	    glOTFData[nGoodCnt++] = lVPS7;
	}
    }
    nSenCnt++;

    if(nVcnt > 7)
    {
	rc = GASendDMCCommand(ghDMC, caVPS8command, caResp, 80);
	lVPS8 = atol(caResp);
//printf("vps8=%d\n",lVPS8);
	if (lVPS8 != 0 && nGoodCnt < 6 && lVPS7 > lRposLast && labs(lVPS7-lVPS8)>1500)
	{
	    giSmap[nGoodCnt] = iSenMap[nSenCnt];
	    glOTFData[nGoodCnt++] = lVPS8;
	}
    }
    nSenCnt++;

    if(nVcnt > 8)
    {
	rc = GASendDMCCommand(ghDMC, caVPS9command, caResp, 80);
	lVPS9 = atol(caResp);
//printf("vps9=%d\n",lVPS9);
	if (lVPS9 != 0 && nGoodCnt < 6 && lVPS9 > lRposLast && labs(lVPS8-lVPS9)>1500)
	{
	    giSmap[nGoodCnt] = iSenMap[nSenCnt];
	    glOTFData[nGoodCnt++] = lVPS9;
	}
    }

//for(i=0; i<nGoodCnt;++i)
//printf("good %d pos=%d map=%d\n",i,glOTFData[i],giSmap[i]);

    if (nGoodCnt < 5)
	return FAILURE;

    // good count must be 5 or 6
    // if 5, must fix the data

    if (nGoodCnt == 5)
    {
	// DO NOT Rely on only the 1st sensor map data
	// find out which sensor-hit is missing
	// must assume 1st hit is 2nd sensor
	// assume 3rd hit & 4th hit has wafer size / 3 apart
	if (glOTFData[2] - glOTFData[3] > glR200mm / 3)
	{
	    // hit 0, 1, 2 are  OK
	    // hit 3 & 4 are same
	    glOTFData[5] = glOTFData[4]; // bump
	    glOTFData[4] = glOTFData[3]-11; // set 3 & 4 same
//	    giSmap[3] = ~0x03;
//	    giSmap[4] = ~0x02;
//	    giSmap[5] = ~0x00;
	}
	else
	{
	    // hit 3, 4, 5 are  OK
	    // hit 1 & 2 are same
	    glOTFData[5] = glOTFData[4]; // bump
	    glOTFData[4] = glOTFData[3]; // bump
	    glOTFData[3] = glOTFData[2]; // bump
	    glOTFData[2] = glOTFData[1]-11; // set 1 & 2 same
//	    giSmap[1] = ~0x03;
//	    giSmap[2] = ~0x07;
//	    giSmap[3] = ~0x03;
//	    giSmap[4] = ~0x02;
//	    giSmap[5] = ~0x00;
	}

    }
    else if (nGoodCnt == 6) // nGoodCnt = 6 
    {
	if (giSmap[1] == 6)
	{
	    // 3rd sensor trigger before 1st
	    lTemp = glOTFData[2];
	    glOTFData[2] = glOTFData[1];
	    glOTFData[1] = lTemp;
	}
	if (giSmap[3] == 6)
	{
	    // 1st sensor cleared before 3rd
	    lTemp = glOTFData[3];
	    glOTFData[3] = glOTFData[4];
	    glOTFData[4] = lTemp;
	}
    }
    else // nGoodCnt > 6
    {	 // remove spurious interrupts 
printf("nGoodCnt=%d\n",nGoodCnt);
	for (i=1; i<nGoodCnt; ++i)
	{
	    if(labs(glOTFData[i-1]-glOTFData[i])<1500) 
	    {
	    	for(j=i; j<nGoodCnt; ++j)
		{
		    glOTFData[j] = glOTFData[j+1];
		}
		nGoodCnt--;
		break;
	    }
	}
	if (nGoodCnt > 6)
	{
	    for (i=1; i<nGoodCnt; ++i)
	    {
	    	if(labs(glOTFData[i]-glOTFData[i-1])<1500) 
		{
		    for(j=i; j<nGoodCnt; ++j)
		    {
			glOTFData[j] = glOTFData[j+1];
		    }
		    nGoodCnt--;
		    break;
		}
	    }
	}
	if (nGoodCnt > 6)
	{
	    for (i=1; i<nGoodCnt; ++i)
	    {
	    	if(labs(glOTFData[i]-glOTFData[i-1])<1500) 
		{
		    for(j=i; j<nGoodCnt; ++j)
		    {
			glOTFData[j] = glOTFData[j+1];
		    }
		    nGoodCnt--;
		    break;
		}
	    }
	}
printf("nGoodCnt=%d\n",nGoodCnt);
for(i=0;i<nGoodCnt;++i)
printf("OTFdata[%d]=%d\n",i, glOTFData[i]); 
    }


//printf("data: %d %d %d %d %d %d\n",glOTFData[0],glOTFData[1],glOTFData[2],glOTFData[3],glOTFData[4],glOTFData[5]);
//printf("map: %d %d %d %d %d %d\n", giSmap[0], giSmap[1], giSmap[2], giSmap[3], giSmap[4], giSmap[5]);

    nGoodCnt = 6;
    aiPortsMap[0] = 0x02;
    aiPortsMap[1] = 0x03;
    aiPortsMap[2] = 0x07;
    aiPortsMap[3] = 0x03;
    aiPortsMap[4] = 0x02;
    aiPortsMap[5] = 0x00;

    for (i=0; i<6; ++i)
    {
	pstOTFCurrentStation->m_lOTFData[i] = glOTFData[i];
	pstOTFCurrentStation->m_lSensorMap[i] = aiPortsMap[i];
//printf("Filtered %d pos=%d map=%d\n",i, glOTFData[i], aiPortsMap[i]);
    }

//for (i=0; i<6; ++i)
//printf("i=%d VPS=%d sen=%x\n",i,glOTFData[i],aiPortsMap[i]);

//    if(lVPS6 == 0 && lVPS5 == 0 || nVcnt < 6)
//	return FAILURE;

//("MPConvertOTFData() 3 working...\n");

    nVcnt = 6;


    // only one case of missing 1 latch --> latch count = 5
/*****
    if(nVcnt == 0 && lVPS5 != 0)
    {
	if (giSensorCount < 6) // both sensor map & latch counter are less than 6, failure
		return FAILURE;
	////////////////////////////////////////////////////////////////////////
	// Assumption:
	//    1. two sensors are triggered at the same time --> same R position
	//    2. this will happen only with sensors 1 and 3
	//    3. sensor hardware failure is not considered!!!!!!!!!!!!!!!!!!!!!!
	////////////////////////////////////////////////////////////////////////
	if(labs(lVPS2 - lVPS3) > 12000) // assume sensor 3 missing
	{
	    lVPS6 = lVPS5; lVPS5 = lVPS4; lVPS4 = lVPS3; lVPS3 = lVPS2;
	}
	else // assume sensor 1 missing
	{
	    lVPS6 = lVPS5; lVPS5 = lVPS4;
	}
	nVcnt = 6;
    }
*******/

    giOTFCounter = nVcnt;


    if(ROGetParameter(TRUE, (ULONG)RO_AXIS_R, laScaledTemp, CUSTOMIZED_HOME) == FAILURE)
        return FAILURE;
    if(ROUnscalePos((ULONG)RO_AXIS_R, laScaledTemp, laUnscaledTemp) == FAILURE)
        return FAILURE;
    lGalilIODiff = laUnscaledTemp[1];

    iOTFInterruptsCounter = 0;
    for (i = 0; i < giOTFCounter; ++i)
    {
//        laUnscaledTemp[1] = glOTFData[i] + lGalilIODiff;
	laUnscaledTemp[1] = glOTFData[i];
        if(ROScalePos((ULONG)RO_AXIS_R, laUnscaledTemp, laScaledTemp) == FAILURE)
            return FAILURE;
       	fplOTFPosition[iOTFInterruptsCounter] = laScaledTemp[1];
//       	fpiOTFSensorsBitmap[iOTFInterruptsCounter++] = aiPortsMap[i];  // dark-ON
     	fpiOTFSensorsBitmap[iOTFInterruptsCounter++] = ~aiPortsMap[i]; // light-ON

	// if R pos is too little different from the previous data, discard the current
//	if (iOTFInterruptsCounter > 0)
//	{
//	    if(labs(laScaledTemp[1] - fplOTFPosition[iOTFInterruptsCounter-1]) < 100)
//	    {
//        	--iOTFInterruptsCounter;
//	    }
//	}
//printf("i=%d VPS=%d pos=%f sen=%x\n",i,glOTFData[i], fplOTFPosition[i],fpiOTFSensorsBitmap[i]);
    }

//    for (i=0; i<giSensorCount; ++i)
//    {
//printf("sensorIO i=%d sensorMap=%d\n",i,~giSensorMap[i]&7);
//    }

    return SUCCESS;
}
int MPConvertOTFDataOld( )
{
    char caResp[80];
    int i, rc;
    int nVcnt;
    long lVPS1, lVPS2, lVPS3, lVPS4, lVPS5, lVPS6;

    long lGalilIODiff, lRPos, lTemp1 = 0, lTemp2 = 0;
    long laUnscaledTemp[8]={0,0,0,0,0,0,0,0}, laScaledTemp[8]={0,0,0,0,0,0,0,0};

    // just in case, halt the galil macro thread
//    rc = GASendDMCCommand(ghDMC, caHXcommand, caResp, 80);


    for (i=0;i<giSensorCount;++i) 
    {
	giSensorMap[i] &= 0x07;
	giSensorMap[i] = ~giSensorMap[i];
    }

    lVPS1 = lVPS2 = lVPS3 = lVPS4 = lVPS5 = lVPS6 = 0;
    rc = GASendDMCCommand(ghDMC, caVCNTcommand, caResp, 80);
    nVcnt = atoi(caResp);

////printf("MPConvertOTFData() working... nVcnt=%d\n", nVcnt );

//    if(nVcnt < 5) 
//	return FAILURE;

//printf("MPConvertOTFData() 2 working...\n");

    rc = GASendDMCCommand(ghDMC, caVPS1command, caResp, 80);
    lVPS1 = atol(caResp);
    rc = GASendDMCCommand(ghDMC, caVPS2command, caResp, 80);
    lVPS2 = atol(caResp);
    rc = GASendDMCCommand(ghDMC, caVPS3command, caResp, 80);
    lVPS3 = atol(caResp);
    rc = GASendDMCCommand(ghDMC, caVPS4command, caResp, 80);
    lVPS4 = atol(caResp);
    rc = GASendDMCCommand(ghDMC, caVPS5command, caResp, 80);
    lVPS5 = atol(caResp);
    rc = GASendDMCCommand(ghDMC, caVPS6command, caResp, 80);
    lVPS6 = atol(caResp);

    glOTFData[0] = lVPS1; aiPortsMap[0] = 0x02;
    glOTFData[1] = lVPS2; aiPortsMap[1] = 0x03;
    glOTFData[2] = lVPS3; aiPortsMap[2] = 0x07;
    glOTFData[3] = lVPS4; aiPortsMap[3] = 0x03;
    glOTFData[4] = lVPS5; aiPortsMap[4] = 0x02;
    glOTFData[5] = lVPS6; aiPortsMap[5] = 0x00;

//for (i=0; i<6; ++i)
//printf("i=%d VPS=%d sen=%x\n",i,glOTFData[i],aiPortsMap[i]);

    if(lVPS6 == 0 && lVPS5 == 0 || nVcnt < 6)
	return FAILURE;

//("MPConvertOTFData() 3 working...\n");

    nVcnt = 6;


    // only one case of missing 1 latch --> latch count = 5
/*****
    if(nVcnt == 0 && lVPS5 != 0)
    {
	if (giSensorCount < 6) // both sensor map & latch counter are less than 6, failure
		return FAILURE;
	////////////////////////////////////////////////////////////////////////
	// Assumption:
	//    1. two sensors are triggered at the same time --> same R position
	//    2. this will happen only with sensors 1 and 3
	//    3. sensor hardware failure is not considered!!!!!!!!!!!!!!!!!!!!!!
	////////////////////////////////////////////////////////////////////////
	if(labs(lVPS2 - lVPS3) > 12000) // assume sensor 3 missing
	{
	    lVPS6 = lVPS5; lVPS5 = lVPS4; lVPS4 = lVPS3; lVPS3 = lVPS2;
	}
	else // assume sensor 1 missing
	{
	    lVPS6 = lVPS5; lVPS5 = lVPS4;
	}
	nVcnt = 6;
    }
*******/

    giOTFCounter = nVcnt;


    if(ROGetParameter(TRUE, (ULONG)RO_AXIS_R, laScaledTemp, CUSTOMIZED_HOME) == FAILURE)
        return FAILURE;
    if(ROUnscalePos((ULONG)RO_AXIS_R, laScaledTemp, laUnscaledTemp) == FAILURE)
        return FAILURE;
    lGalilIODiff = laUnscaledTemp[1];

    iOTFInterruptsCounter = 0;
    for (i = 0; i < giOTFCounter; ++i)
    {
//        laUnscaledTemp[1] = glOTFData[i] + lGalilIODiff;
	laUnscaledTemp[1] = glOTFData[i];
        if(ROScalePos((ULONG)RO_AXIS_R, laUnscaledTemp, laScaledTemp) == FAILURE)
            return FAILURE;
       	fplOTFPosition[iOTFInterruptsCounter] = laScaledTemp[1];
//       	fpiOTFSensorsBitmap[iOTFInterruptsCounter++] = aiPortsMap[i];  // dark-ON
     	fpiOTFSensorsBitmap[iOTFInterruptsCounter++] = ~aiPortsMap[i]; // light-ON

	// if R pos is too little different from the previous data, discard the current
//	if (iOTFInterruptsCounter > 0)
//	{
//	    if(labs(laScaledTemp[1] - fplOTFPosition[iOTFInterruptsCounter-1]) < 100)
//	    {
//        	--iOTFInterruptsCounter;
//	    }
//	}
//printf("i=%d VPS=%d pos=%f sen=%x\n",i,glOTFData[i], fplOTFPosition[i],fpiOTFSensorsBitmap[i]);
    }

//    for (i=0; i<giSensorCount; ++i)
//    {
//printf("sensorIO i=%d sensorMap=%d\n",i,~giSensorMap[i]&7);
//    }

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    MPGetOTFBufferOverflow
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPGetOTFBufferOverflow()
{
    return iOTFBufferOverflow;
}
/****************************************************************\
 *
 * Function:    MPGetOTFAligning
 *
 * Parameters:
 *
 * Returns:     SUCCESS/FAILURE
 *
\*****************************************************************/
int MPGetOTFAligning()
{
    return iOTFAligning;
}
