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
 * File:        rofio.c
 * Functions:   MEGetXXXXXXX() - To allow the Mechanism's customer to get
 *                                     the parameters from Mechanism module
 *              MESetXXXXXXX() - To allow the Mechanism's customer to put
 *                                   the parameters to Mechanism module
 *                                   &/or save it to PARA
 *              ROGetMilScale
 *              ROSetMilScale
 *              ROArrangeGetArrayForW
 *              ROArrangeSetArrayForW
 *
 * Description: Provides a dual interface to read and write
 *      from and to either the datafiles, the motion control cards,
 *      or both
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#include <math.h>
#include <string.h>
#include "sck.h"
#include "rofio.h"
#include "ro.h"
#include "roloc.h"
#include "gag.h"
#include "fiog.h"
#include "gaintr.h"


/****************************************************************
 *
 * Function:    ROGetParameter
 *
 * Abstract:    Generic function for getting the parameter from the parameter file or from
 *              Galil motion card, depending on the type of parameter
 *              and flag passed
 *              the parameters that cannot be read from motion card don't
 *              have the appropriate case in switch statement, so in case
 *              they are tried to call from motion card respond is FAILURE
 * Parameters:
 *      iGetFromParaFileArg -   indicates whether to get parameter from
 *                              parameter file or from Galil motion card
 *      ulEquipeAxisArg -       (in) The axes to read
 *      lParameterValueArg -    (out) The values read
 *      iParameterType -        value indicating the type of parameter requested
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROGetParameter(int iGetFromParamFileArg, unsigned long ulEquipeAxisArg, long *lParameterValueArg, int iParameterTypeArg)
{
    long laTempParameter[8]={0,0,0,0,0,0,0,0};
    double dTempParameter[8];
    int iReturn, iCardNum, iFileType;
    unsigned uGalilAxes;

    /* Make sure the axes requested are valid and exist in this system. This
     * function also sets up the correct file type for the Equipe axes requested. */
    if (ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;

    /* Either get the info from the datafile internal structure... */
    if (iGetFromParamFileArg)
    {
        if(FIOGetParamVals(iFileType, iParameterTypeArg, lParameterValueArg) == FAILURE)
            return FAILURE;
        /* In the case of a W or w axis, the data must be transferred to the
         * fourth element of the array. */
        if ((ulEquipeAxisArg == RO_AXIS_W) || (ulEquipeAxisArg == RO_AXIS_w))
        {
            ROArrangeGetArrayForW(ulEquipeAxisArg, lParameterValueArg);
        }
    }
    /* ...or directly from the Galil card. */
    else
    {
        if((iParameterTypeArg == OPERATIONAL_SPEED) || (iParameterTypeArg == OPERATIONAL_ACCEL) ||
                (iParameterTypeArg == OPERATIONAL_DECEL))
        {
            /* The motion module already puts the 4th axis data in the 4th element of the array. */
            iReturn = GAGetValsLongDefined(iCardNum, iParameterTypeArg, uGalilAxes, laTempParameter);
            if (iReturn == FAILURE)
                return FAILURE;

            /* Scale the number from encoder counts to normal units. */
            ROScaleSpd(ulEquipeAxisArg, laTempParameter, lParameterValueArg);
        }
        else if((iParameterTypeArg == TORQUE_LIMIT) ||
				(iParameterTypeArg == FEED_FORWARD_ACCEL) ||
				(iParameterTypeArg == JERK) ||
				(iParameterTypeArg == IT_SCURVE) )
        {
            /* The motion module already puts the 4th axis data in the 4th element of the array. */
            iReturn = GAGetValsDoubleDefined(iCardNum, iParameterTypeArg, uGalilAxes, dTempParameter);
            if (iReturn == FAILURE)
                return FAILURE;

            /* Don't forget the Galil data must be multiplied by 1000. */
            ROGetMilScale(dTempParameter, lParameterValueArg);
        }
        else
        {
            /* The motion module already puts the 4th axis data in the 4th element of the array. */
            iReturn = GAGetValsLongDefined(iCardNum, iParameterTypeArg, uGalilAxes, lParameterValueArg);
            if (iReturn == FAILURE)
                return FAILURE;
        }
    }

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROSetParameter
 *
 * Abstract:    Generic function for setting the parameter in the parameter file or in
 *              Galil motion card, depending on the type of parameter and flag passed,
 *              the parameters that cannot be set in motion card don't
 *              have the appropriate case in switch statement, so in case
 *              they are tried to be set in motion card respond is FAILURE
 * Parameters:
 *      iSetFromParaFileArg -   indicates whether to set parameter in
 *                              parameter file or in Galil motion card
 *      ulEquipeAxisArg -       (in) The axes to set
 *      lParameterValueArg -    (out) The values set
 *      iParameterType -        value indicating the type of parameter requested
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int ROSetParameter(int iSaveToParamFileArg, unsigned long ulEquipeAxisArg, long *lParameterValueArg, int iParameterTypeArg)
{
    long laTempParameter[8];
    double dTempParameter[8];
    int iReturn, iCardNum, iFileType, i;
    unsigned uGalilAxes;

    /* Make sure the axes requested are valid and exist in this system. This function
     * also sets up the correct file type and Galil axes for the Equipe axes requested. */
    if(ROValidAxis(ulEquipeAxisArg, &iCardNum, &iFileType, &uGalilAxes) == FAILURE)
        return FAILURE;


    if ((iParameterTypeArg == OPERATIONAL_SPEED) || (iParameterTypeArg == OPERATIONAL_ACCEL) ||
        (iParameterTypeArg == JERK) || (iParameterTypeArg == OPERATIONAL_DECEL) ||
        (iParameterTypeArg == DERIVATIVE_GAIN) || (iParameterTypeArg == INTEGRAL_GAIN) ||
        (iParameterTypeArg == IT_SCURVE) || (iParameterTypeArg == PROPORTIONAL_GAIN))
    {
        /* Operating speed can NOT be set if the axis is currently in motion. */
        if((ulAMFlag&ulEquipeAxisArg) != ulEquipeAxisArg)
	{
//printf("AMFlag=%d axis=%d\n",ulAMFlag, ulEquipeAxisArg);
            return FAILURE;
 	}
    }

    /* Set the new values to the datafile internal structure if desired. */
    if(iSaveToParamFileArg)
    {
        /* Make a local copy of the values to be set. */
        memcpy(laTempParameter, lParameterValueArg, sizeof(long)*8);
        /* Get the parameter from the datafile internal structure into the local
         * copy so that only the reqested changes are registered while other values are preserved. */
        if(FIOGetParamVals(iFileType, iParameterTypeArg, laTempParameter) == FAILURE)
            return FAILURE;

        if((ulEquipeAxisArg == RO_AXIS_W) || (ulEquipeAxisArg == RO_AXIS_w))
        {
            /* In the case of a W or w axis, the data must be transferred from the
             * fourth element of the array where it remains from the previous copy
             * operation. The datafile routine only overwrites the first 3 elements. */
//printf("W before = ");
//for(i=0;i<8;++i) printf("%d, ",laTempParameter[i]);
//printf("\n");
            ROArrangeSetArrayForW(ulEquipeAxisArg, laTempParameter);
//printf("W after = ");
//for(i=0;i<8;++i) printf("%d, ",laTempParameter[i]);
//printf("\n");

        }
        else
        {
            /* Other axes must be copied indiviually from the master copy that was passed in
             * to the local copy. A request for A or a will actually copy all the values over.
             * This way, for a single axis request, the other values will be preserved. */
            if (ulEquipeAxisArg&RO_AXIS_T) 
				laTempParameter[0] = lParameterValueArg[0];
            if (ulEquipeAxisArg&RO_AXIS_R) 
				laTempParameter[1] = lParameterValueArg[1];
            if (ulEquipeAxisArg&RO_AXIS_Z) 
				laTempParameter[2] = lParameterValueArg[2];
            if (ulEquipeAxisArg&RO_AXIS_W) 
				laTempParameter[3] = lParameterValueArg[3];
            if (ulEquipeAxisArg&RO_AXIS_t) 
				laTempParameter[4] = lParameterValueArg[4];
            if (ulEquipeAxisArg&RO_AXIS_r) 
				laTempParameter[5] = lParameterValueArg[5];
            if (ulEquipeAxisArg&RO_AXIS_z) 
				laTempParameter[6] = lParameterValueArg[6];
            if (ulEquipeAxisArg&RO_AXIS_w) 
				laTempParameter[7] = lParameterValueArg[7];
        }

        /* Set the new values to the datafile internal structure. */
        if(FIOSetParamVals(iFileType, iParameterTypeArg, laTempParameter) == FAILURE)
            return FAILURE;
    }

    if((iParameterTypeArg == OPERATIONAL_SPEED) || (iParameterTypeArg == OPERATIONAL_ACCEL) ||
            (iParameterTypeArg == OPERATIONAL_DECEL))
    {
        /* Unscale the number from normal units to encoder counts. */
        memset(laTempParameter, 0, sizeof(long)*8);
        ROUnscaleSpd(ulEquipeAxisArg, lParameterValueArg, laTempParameter);

        /* Don't allow values between -5 and +5 to be set. It might result in 0 speed. */
        if ((ulEquipeAxisArg & RO_AXIS_T) && 
			(laTempParameter[0] < 5) && (laTempParameter[0] > -5))
            return FAILURE;
        if ((ulEquipeAxisArg & RO_AXIS_R) && 
			(laTempParameter[1] < 5) && (laTempParameter[1] > -5))
            return FAILURE;
        if ((ulEquipeAxisArg & RO_AXIS_Z) && 
			(laTempParameter[2] < 5) && (laTempParameter[2] > -5))
            return FAILURE;
        if ((ulEquipeAxisArg & RO_AXIS_W) && 
			(laTempParameter[3] < 5) && (laTempParameter[3] > -5))
            return FAILURE;

        if ((ulEquipeAxisArg & RO_AXIS_t) && 
			(laTempParameter[4] < 5) && (laTempParameter[4] > -5))
            return FAILURE;
        if ((ulEquipeAxisArg & RO_AXIS_r) && 
			(laTempParameter[5] < 5) && (laTempParameter[5] > -5))
            return FAILURE;
        if ((ulEquipeAxisArg & RO_AXIS_z) && 
			(laTempParameter[6] < 5) && (laTempParameter[6] > -5))
            return FAILURE;
        if ((ulEquipeAxisArg & RO_AXIS_w) && 
			(laTempParameter[7] < 5) && (laTempParameter[7] > -5))
            return FAILURE;
        iReturn = GASetValsLongDefined(iCardNum, iParameterTypeArg, uGalilAxes, laTempParameter);
        if (iReturn == FAILURE)
            return FAILURE;
    }
    else if((iParameterTypeArg == TORQUE_LIMIT) || (iParameterTypeArg == FEED_FORWARD_ACCEL)
			|| (iParameterTypeArg == JERK) || (iParameterTypeArg == IT_SCURVE))
    {
        /* Don't forget, the normal units must be divided by 1000. */
        ROSetMilScale(lParameterValueArg, dTempParameter);
        iReturn = GASetValsDoubleDefined(iCardNum, iParameterTypeArg, uGalilAxes, dTempParameter);
        if (iReturn == FAILURE)
            return FAILURE;
    }
    else if ((iParameterTypeArg == ERROR_LIMIT) || 
      	(iParameterTypeArg == DERIVATIVE_GAIN) || (iParameterTypeArg == PROPORTIONAL_GAIN) ||
        (iParameterTypeArg == INTEGRAL_GAIN))
    {
        iReturn = GASetValsLongDefined(iCardNum, iParameterTypeArg, uGalilAxes, lParameterValueArg);
        if (iReturn == FAILURE)
            return FAILURE;
    }
// These 2 lines take care of values that exists in the parameter but cannot be set in Galil.
// But they will cause problems in WRIP. So let's leave them out and see what happens.
//    else
//        return FAILURE;

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    ROGetMilScale
 *
 * Abstract:    Multiplies all values by 1000 converting the double
 *      into a long.
 *
 * Parameters:
 *      dInArg      (in) The double array to scale by 1000
 *      lOutArg     (out) The long array to put the results into
 *
 * Returns:     None
 *
 ***************************************************************/
void ROGetMilScale(double *dInArg, long *lOutArg)
{
    int iAxis;

    /* For each element in the array... */
    for (iAxis=0; iAxis<8; iAxis++)
    {
        /* ...check if the number is above the "1/2 way point." */
        if ((dInArg[iAxis]*1000.0) >= (floor(dInArg[iAxis]*1000.0)+0.5))
            /* If it is, round up. */
            lOutArg[iAxis] = (long)(ceil(dInArg[iAxis]*1000.0));
        else
            /* Otherwise, round down. */
            lOutArg[iAxis] = (long)(floor(dInArg[iAxis]*1000.0));
    }

    return;
}


/****************************************************************
 *
 * Function:    ROSetMilScale
 *
 * Abstract:    Divides all values by 1000 converting the long
 *      into a double.
 *
 * Parameters:
 *      lInArg      (in) The long array to scale by 1000
 *      dOutArg     (out) The double array to put the results into
 *
 * Returns:     None
 *
 ***************************************************************/
void ROSetMilScale(long *lInArg, double *dOutArg)
{
    int iAxis;

    /* For all elements of the array... */
    for (iAxis=0; iAxis<8; iAxis++)
    {
        /* ...divide tha value by 1000. */
        dOutArg[iAxis] = (double)(lInArg[iAxis]) / 1000.0;
    }

    return;
}


/****************************************************************
 *
 * Function:    ROArrangeGetArrayForW
 *
 * Abstract:    Sets up the array appropriately for W or w
 *      axes. The data file returns the information from the
 *      first or second column of the auxiliary file. That
 *      data has to be appropriately put into the fourth
 *      index of the array in order for all the other modules
 *      to read it.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to set
 *      lValue              (in) The array to rearrange
 *
 * Returns:     None
 *
 ***************************************************************/
void ROArrangeGetArrayForW(unsigned long ulEquipeAxisArg, long *lValue)
{
    /* If the axis is a W, read from the first column of the
     * auxiliary file. The exception is when the track is on the
     * first Galil card, we must read its values from the second column. */
    if (ulEquipeAxisArg == RO_AXIS_W) //&& (~iDefineFlag & DFSS))
    {
        /* Copy the value... */
        lValue[3] = lValue[0];
        /* ...and zero out the rest of the array. We don't want and garbage
         * information for be accidentally output and misinterpreted. */
        lValue[0]=lValue[1]=lValue[2]=lValue[4]=lValue[5]=lValue[6]=lValue[7] = 0;
    }
    /* If the axis is a w, read from the second column of the
     * auxiliary file. */
    else /* if (ulEquipeAxisArg == RO_AXIS_w) */
    {
        lValue[7] = lValue[1];
        lValue[0]=lValue[1]=lValue[2]=lValue[3]=lValue[4]=lValue[5]=lValue[6] = 0;
    }
    return;
}


/****************************************************************
 *
 * Function:    ROArrangeSetArrayForW
 *
 * Abstract:    Sets up the array appropriately for W or w
 *      axes. The data file requires the information from the
 *      fourth column of the array to be appropriately put into
 *      the first or second index.
 *
 * Parameters:
 *      ulEquipeAxisArg     (in) The axes to set
 *      lValue              (in) The array to rearrange
 *
 * Returns:     None
 *
 ***************************************************************/
void ROArrangeSetArrayForW(unsigned long ulEquipeAxisArg, long *lValue)
{
    /* If the axis is a W, put the value into the first column of the
     * auxiliary file. The exception is when the track is on the
     * first Galil card, we must write its values to the second column. */
    if ((ulEquipeAxisArg == RO_AXIS_W) && (~iDefineFlag & DFSS))
    {
        lValue[0] = lValue[3];
    }
    /* If the axis is a w, write to the second column of the
     * auxiliary file. */
    else /* if (ulEquipeAxisArg == RO_AXIS_w) */
    {
        lValue[1] = lValue[7];
    }
    return;
}
