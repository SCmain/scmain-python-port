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
 *
 * Program:     Status
 * File:        scstat.c
 * Functions:   SSSetStatusWord
 *              SSReadStatusBits
 *              SSGetVacuumBits
 *              SSGetSystemStatus
 *              SSGetSystemStatus2
 *              SSGetGENSystemStatus
 *              SSSetCheckSumStatus
 *              SSGetCheckSumStatus
 *              SSSetStatusWord2
 *              SSReadStatusBit2
 *              SSInitStatus
 *              SSSetModulesInitializationsStatus
 *              SSGetModulesInitializationsStatus
 *
 * Description: Maintain the system status and provide the
 *      interface for other modules to access.
 *
\***************************************************************/

#include <sys/io.h>
#include "sck.h"
#include "scstat.h"
#include "scintr.h"
#include "gag.h"
#include "ro.h"
#include "scmac.h"
#include "scregg.h"
#include "fiog.h"
#include "mapio.h"
#include "scio.h"
#include "alk.h"
#include "alfio.h"
#include "sctim.h"

/* global variables in the module */
unsigned SSuSystemStatus;       /* 16-bit main system status word */
unsigned SSuSystemStatus2;      /* 16-bit second system status word */
unsigned SSuCheckSumStatus;     /* check sum status word */
int SSiPrevSystemStatus = 0;    /* System status from the last time
                                 * SSGetStatusWord was called */
int SSiPrevGenmarkStatus = 0;   /* Same as SSiPrevSystemStatus; for Genmark emulation */
int SSiDefineFlag;
unsigned SSuEmulatorType;
int iSystemInitializationStatus = 0;    /* keeps the record of failures during
                                         * main initialization of modules */
unsigned int SSuDebugBit = 0;   /* general purpose debug on/off bit */

/****************************************************************\
 *
 * Function:    SSSetStatusWord
 *
 * Abstract:    Set or clear bit(s) in the system status word directly
 *
 * Parameters:
 *      uMaskArg    (in) mask of bits to set/clear
 *      iFlagArg    (in) TRUE to set else to clear
 *
 * Returns:    N/A
 *
 * Replaces:    void set_status(unsigned mask, int sw)
 *
\*****************************************************************/
void SSSetStatusWord(unsigned uMaskArg, int iFlagArg)
{
    int iCurrentInput;

    /* Change the appropriate bits in the system status word */
    if (iFlagArg)
        SSuSystemStatus |= uMaskArg;
    else
        SSuSystemStatus &= ~uMaskArg;

    /* If the front panel LED(s) status changed, make the change
     * in hardware also. */
    if (uMaskArg & LED_ERROR)
    {
        /* Get the current state */
        iCurrentInput = inb(IO_LED_OUTPORT);
        /* Change the 2nd bit to the new value */
        /* NOTE: PA bit use is reversed!!! */
        if (iFlagArg)
        {
                iCurrentInput &= 0xFE;
        }
        else if (!(SSuSystemStatus & LED_ERROR))
        {
                iCurrentInput |= 0x01;
        }
        /* Send it back out to the I/O card */
        IOWriteIO(-1, iCurrentInput, IO_LED_OUTPORT);
    }

    return;
}


/****************************************************************\
 *
 * Function:    SSReadStatusBits
 *
 * Abstract:    Read the specified bit(s) out of the system status word
 *
 * Parameters:
 *      uMask    (in) mask of bits to read
 *
 * Returns:    TRUE if any bits in mask are set, else FALSE
 *
 * Replaces:    unsigned read_status_bits(unsigned mask)
 *
\*****************************************************************/
unsigned SSReadStatusBits(unsigned uMaskArg)
{
    return (SSuSystemStatus & uMaskArg);
}


/****************************************************************\
 *
 * Function:     SSGetVacuumBits
 *
 * Abstract:    Gets vacuum sensor & switch bits for status word,
 *      ready to AND into bits 2 and 3 of the main system status word.
 *
 * Parameters:  None
 *
 * Returns:     Vacuum bits
 *
 * Replaces:    int get_vac_bits(void)
 *
\*****************************************************************/
int SSGetVacuumBits(void)
{
    int iStatus = 0;
    int iEnd, iVac, iCurrentStation, iOnOffFlag;

    /* Find out which end effector we're using */
    iEnd = 0;
    iCurrentStation = FIOGetCurrentStationInt();
    if ((iCurrentStation>=0) && (iCurrentStation<=51))
    {
        /* Get the active end effector number if the current station
         * number is valid. */
        FIOGetStnEEindex(iCurrentStation, &iEnd);
    }
    else
    {
        /* If the current station number is invalid, return 0. */
        return iStatus;
    }

    /* Get the state of the vacuum sensor. */
//    GAReadGalilInputOutputPort(GA_CARD_0, &iVac);
    iVac = inb(IO_ROBOT_INPUT_F);
    /* Adjust it to get the vacuum sensor bit for the selected end effector. */
    iVac = (iVac >> iEnd) & 0x01;
    /* Set the temporary status word, which is used by the system status word.
     * Sensor is ACTIVE LOW!!! */
    if (!iVac)
        iStatus |= VACUUM_SEN;

    /* Get the state of the vacuum switch. */
    iVac = inb(IO_ROBOT_OUTPUT_A);
    iVac = (iVac >> iEnd) & 0x01;
    /* Set the temporary status word, which is used by the system status word.
     * Switch is ACTIVE LOW!!! */
    if (!iVac)
        iStatus |= VACUUM_SW;

    return iStatus;
}


/****************************************************************\
 *
 * Function:     SSGetSystemStatus
 *
 * Abstract:    Gathers bits for system status word. It actually
 *      reaches out to a number of other modules ANDing in
 *      status bits from them.
 *
 * Parameters:  None
 *
 * Returns:     SSuSystemStatus
 *
 * Replaces:    int get_sys_status(void)
 *
 * NOTE: Some bits are continually maintained in "sys_status",
 *      while others are gathered only when this routine is called.
 *
\*****************************************************************/
int SSGetSystemStatus(void)
{
    int iStatus, iAligning;
    unsigned long ulAllEquipeAxes;

    ROUpdateTS(FALSE);

    if (!MPIsNotScanning())
    {
        /* Initialize the update for the system status word. */
//        iStatus = SSiPrevSystemStatus;
        iStatus = 0;
    }
    else
    {
        /* Mask out all except bits which are always kept current. */
        iStatus = SSuSystemStatus & 0xE883;

    }

    /* Add in the Vacuum Sensor Bits */
    iStatus |= SSGetVacuumBits();

    /* Initialize the ulAllEquipeAxes variable from the Mechanism module. */
    ulAllEquipeAxes = ROGetAllAMFlag();

    /* Add in the Motion Error bits. */
    if (ROGetERFlag() & ulAllEquipeAxes)
        iStatus |= MOT_ERROR;

    /* Update the Limit Switch status variables in the Mechanism module. */
    if (MPIsNotScanning())
    {
        ROCheckSwitches(GA_CARD_0);
        ROCheckSwitches(GA_CARD_1);
    }

    /* Add in the bit for the updated Limit Switch flags and
     * updated Limit Switch Momentary flags. */
    if ((ROGetLMPosFlag() | ROGetLMNegFlag() | ROGetLMPosMom() |
            ROGetLMNegMom()) & ulAllEquipeAxes)
    {
        iStatus |= LIMIT_SW;
    }

    /* Add in the Homed flag bit. */
    if (~ROGetHomed() & ulAllEquipeAxes)
        iStatus |= HOME_NOT_EXED;

    /* Add in the Macro Running bit. */
    if (MRGetMacroStatus() == MACRO_RUNNING)
        iStatus |= RUNNING_MACRO;

    /* Add in the After/Still In Motion bit. Curiously, Servo On/Off status
     * is lumped into this check even though the Servo On/Off status
     * has it's own bit in the system status word. */
    if (~ROGetServoFlag() & ~ROGetAMFlag() & ulAllEquipeAxes)
        iStatus |= RBT_MOVING;

    /* Add in the Servo On/Off bit.  */
    if (ROGetServoFlag() & ulAllEquipeAxes)
        iStatus |= SERVO_OFF;

    /* Is it aligning? */
    ALGetIntVars(AL_ALIGNING_FLAG, AL_NOIDX, &iAligning);
    if (iAligning)
        iStatus |= CMD_INVAL;

    /* If there was previously a controller error, add in the bit.
     * The controller error bit can only be cleared by macro and some
     * other modules, NOT by a STAT command. */
    iStatus |= SSuSystemStatus & CTRL_ERR;

    /* In Genmark emulation, the reserved bit that is typically "always 1"
     * may be zero. It represents Multi-Tasking status, Running or Complete. */
    if (SSuEmulatorType & DFEMULATORG)
    {
        /* STACK level is > 1 */
        if (MRGetStkTop() > MRGetPStatus())
            iStatus |= AVAIL_1;
    }
    else
    {
        /* In standard Equipe formats, the reserved bit is always set to 1. */
        iStatus |= AVAIL_1;
    }

    /* Be sure to record this system status as the previous system status
     * before exiting. It will be used the next time STAT is called. */
    return (SSiPrevSystemStatus=iStatus);
}


/****************************************************************\
 *
 * Function:     SSGetGENSystemStatus
 *
 * Abstract:    Gathers bits for system status word. It works
 *      almost exactly the same as the SSGetSystemStatus function
 *      except for some minor changes to emulate Genmark, specifically,
 *      motion error checking by axis.
 *
 * Parameters:
 *      ifromENDMArg    (in) Indicates macro status
 *
 * Returns:     SSuSystemStatus
 *
 * Replaces:    int get_genmark_status(int)
 *
 * NOTE: Some bits are continually maintained in "sys_status",
 *      while others are gathered only when this routine is called.
 *
\*****************************************************************/
int SSGetGENSystemStatus(int ifromENDMArg)
{
    int iStatus, iChuckVacSensorInputPos;
    unsigned long ulErrFlag, ulServoFlag, ulAMFlag, ulAllEquipeAxes, uMacroStatus;
    long lR60, lR33;

    if (!MPIsNotScanning())
    {
        /* Initialize the update for the system status word. */
        iStatus = SSiPrevGenmarkStatus;
    }
    else
    {
        /* Mask out all except bits which are always kept current. */
        iStatus = SSuSystemStatus & 0xE001;

        /* Add in the Vacuum Sensor Bits */
        iStatus |= SSGetVacuumBits();
    }

    /* Initialize the uAllEquipeAxes variable from the Mechanism module. */
    ulAllEquipeAxes = ROGetAllAMFlag();

    /* Update the Limit Switch status variables in the Mechanism module. */
    if (MPIsNotScanning())
    {
        ROCheckSwitches(GA_CARD_0);
        ROCheckSwitches(GA_CARD_1);
    }

    /* Add in the bit for the updated Limit Switch flags and
     * updated Limit Switch Momentary flags. */
    if ((ROGetLMPosFlag() | ROGetLMNegFlag() | ROGetLMPosMom() |
            ROGetLMNegMom()) & ulAllEquipeAxes)
    {
        iStatus |= GEN_LIMIT_SW;
    }

    ulErrFlag = ROGetERFlag();
    ulAMFlag = ROGetAMFlag();
    if (SSiDefineFlag & DFPRE)
    {
        /* Add in the Motion Error bits. */
        if (ulErrFlag & ulAllEquipeAxes)
            iStatus |= MOT_ERROR;

        /* If not vacuum prealigner. */
        if (~SSiDefineFlag & DFVACPRE)
        {
            ALGetIntVars(AL_CHUCK_VAC_SENSOR, AL_NOIDX, &iChuckVacSensorInputPos);
            /* Prealigner on first Galil card. */
            if (iChuckVacSensorInputPos == AL_CHUCK_VAC_SEN_SAP4)
            {
                if (~ROReadGalilPort(0) & iChuckVacSensorInputPos)
                    iStatus |= GEN_PRE_VAC_SEN;
                if (inb(IO_ROBOT_OUTPUT_A) & 0x08)
                    iStatus |= GEN_PRE_VAC_SW;
            }
            else
            {
                if (~ROReadGalilPort(1) & iChuckVacSensorInputPos)
                    iStatus |= GEN_PRE_VAC_SEN;
                if (inb(IO_PRE_OUTPUT_L) & 0x01)
                    iStatus |= GEN_PRE_VAC_SW;
            }
        }
        else
            iStatus &= ~(GEN_PRE_VAC_SEN|GEN_PRE_VAC_SW);

        /* Add in the After/Still In Motion bit. Curiously, Servo On/Off status
         * is lumped into this check even though the Servo On/Off status
         * has it's own bit in the system status word. */
        if (~ulAMFlag & ROAxisAllRobot())
            iStatus |= GEN_RBT_MOVING;

        iStatus &= ~GEN_INTEGRATED_UNUSED;
    }
    else
    {
        /* NEGATIVE TRUE LOGIC!!!   BY INDIVIDUAL AXIS!!!*/
        /* Add in the Motion Error bits. */
        if (ulErrFlag & RO_AXIS_T)
            iStatus |= GEN_MOT_ERROR_T;
        if (ulErrFlag & RO_AXIS_R)
            iStatus |= GEN_MOT_ERROR_R;
        if (ulErrFlag & RO_AXIS_Z)
            iStatus |= GEN_MOT_ERROR_Z;

        /* Add in the After Motion/Still In Motion bit. */
        if (~ulAMFlag & RO_AXIS_T)
            iStatus |= GEN_MOT_IDLE_T;
        if (~ulAMFlag & RO_AXIS_R)
            iStatus |= GEN_MOT_IDLE_R;
        if (~ulAMFlag & RO_AXIS_Z)
            iStatus |= GEN_MOT_IDLE_Z;
    }

    /* Add in the Servo On/Off bit.  */
    ulServoFlag = ROGetServoFlag();
    if (ulServoFlag & ulAllEquipeAxes)
        iStatus |= SERVO_OFF;

    if (~SSuEmulatorType & DFEMULATORA)
    {
        if (SSuEmulatorType & DFEMULATORG)
        {
            /* Add in Multi-Tasking status, Running or Complete, for Genmark emulation. */
            /* STACK level is > 1 */
            if (MRGetStkTop() > MRGetPStatus())
                iStatus |= 0x0380;
        }
        else
        {
            /* Add in the Macro Running bit for non-Genmark emulation. */
            uMacroStatus = MRGetMacroStatus();
            if ((uMacroStatus == MACRO_RUNNING || uMacroStatus == FROZEN) && !ifromENDMArg)
                iStatus |= 0x0380;
        }
    }

    /* Decide whether or not to register a COM2 error in the system status word. */
    if (RGGetRegister(60, &lR60) == SUCCESS)
    {
        if (lR60)
            iStatus |= GEN_TEACH_ACT;
    }

    /* Decide whether or not to set the reserved bit, that is typically
     * "always 1", in the system status word. */
    if (RGGetRegister(33, &lR33) == SUCCESS)
    {
        if (lR33 == 1L)
            iStatus |= GEN_SEARCH_ACT;
    }

    /* If there was previously a controller error, add in the bit.
     * The controller error bit can only be cleared by macro and some
     * other modules, NOT by a STAT command. */
    iStatus |= SSuSystemStatus & CTRL_ERR;

    /* Be sure to record this system status as the previous Genmark system status
     * before exiting. It will be used the next time STAT is called. */
    return (SSiPrevGenmarkStatus=iStatus);
}


/****************************************************************\
 *
 * Function:    SSSetCheckSumStatus
 *
 * Abstract:    Gathers bits for checksum status word (Emulate Genmark)
 *
 * Parameters:
 *      uBitArg     (in) Bit number value
 *      uFlagArg    (in) Set the bit 0/1
 *
 * Replaces:    void set_chk_sum_stat(unsigned bitn, unsigned flag)
 *              from n2.c file
 * Returns:     none
 *
\*****************************************************************/
void SSSetCheckSumStatus(unsigned ubitArg, unsigned uFlagArg)
{
    if (uFlagArg)
        SSuCheckSumStatus |= ubitArg;
    else
        SSuCheckSumStatus &= ~ubitArg;

    if (SSuCheckSumStatus)
        SSSetStatusWord(NVSRAM_ST, TRUE);
    else
        SSSetStatusWord(NVSRAM_ST, FALSE);

    return;
}


/****************************************************************\
 *
 * Function:    SSGetCheckSumStatus
 *
 * Abstract:    Read the specified bit out of the check sum status word
 *
 * Parameters:
 *      uMaskArg    (in) Bit number value
 *
 * Returns:     none
 *
\*****************************************************************/
unsigned SSGetCheckSumStatus(unsigned uMaskArg)
{
    return (SSuCheckSumStatus & uMaskArg);
}


/****************************************************************\
 *
 * Function:    SSSetStatusWord2
 *
 * Abstract:    Gathers bits for second system status word
 *
 * Parameters:
 *      uMaskArg    (in) Bit number value
 *      uFlagArg    (in) Set the bit 0/1
 *
 * Returns:     none
 *
 * Replaces:    void set_stat2(unsigned mask, int sw)
 *
\*****************************************************************/
void SSSetStatusWord2(unsigned uMaskArg, int iFlagArg)
{
    /* If a bit is requested to be set... */
    if (iFlagArg)
    {
        /* ...and it is an error bit... */
        if (uMaskArg & ~NON_ERROR_STAT2_BITS)
        {
            /* ...set the first system status word to register a controller error. */
            SSSetStatusWord(CTRL_ERR, TRUE);
        }
        /* Then set the bit itself in the second system status word. */
        SSuSystemStatus2 |= uMaskArg;
    }
    else
    {
        /* Otherwise, unset the bit in the second system status word. */
        SSuSystemStatus2 &= ~uMaskArg;
        /* If you are removing an error bit or the change caused the second
         * system status word to show no errors... */
        if ((SSuSystemStatus2 == 0x0000) || (SSuSystemStatus2 == NON_ERROR_STAT2_BITS))
        {
            /* ...remove the controller error bit setting from the
             * first system status word. */
            SSSetStatusWord(CTRL_ERR, FALSE);
        }
    }

    return;
}


/****************************************************************\
 *
 * Function:    SSReadStatusBit2
 *
 * Abstract:    Read the specified bit out of the second
 *      system status word
 *
 * Parameters:
 *      uMaskArg     (in) Bit number value
 *
 * Returns:     none
 *
 * Replaces:    unsigned read_stat2_bits(unsigned mask)
 *
\*****************************************************************/
unsigned SSReadStatusBit2(unsigned uMaskArg)
{
    return (SSuSystemStatus2 & uMaskArg);
}


/****************************************************************\
 *
 * Function:    SSGetSystemStatus2
 *
 * Abstract:    get system status word for ex_stat2
 *
 * Parameters:  none
 *
 * Returns:     status word
 *
\*****************************************************************/
int SSGetSystemStatus2()
{
    return SSuSystemStatus2;
}


/****************************************************************\
 *
 * Function:    SSInitStatus
 *
 * Abstract:    Initialize the define type and the emulation
 *
 * Parameters:
 *      iDefineFlagArg      (in) Define flags
 *      iEmulatorTypeArg    (in) Initial emulation
 *
 * Returns:     SUCCESS or FAILURE
 *
\*****************************************************************/
int SSInitStatus(int iDefineFlagArg, int iEmulatorTypeArg)
{
    SSiDefineFlag = iDefineFlagArg;
    SSuEmulatorType = iEmulatorTypeArg;
    return SUCCESS;
}


/****************************************************************\
 *
 * Function:    SSGetModulesInitializationsStatus
 *
 * Abstract:    Function returns the iSystemInitializationStatus word
 *              which keeps record of initialization failures,
 *              status bits get set when particular module fails in the main
 *              loop general initialization
 *
 * Parameters:  None
 *
 * Returns:     iSystemInitializationStatus
\*****************************************************************/
int SSGetModulesInitializationsStatus(void)
{
    return iSystemInitializationStatus;
}


/****************************************************************\
 *
 * Function:    SSSetModulesInitializationsStatus
 *
 * Abstract:    Function sereturns the iSystemInitializationStatus word
 *              which keeps record of initialization failures,
 *              status bits get set when particular module fails in the main
 *              loop general initialization
 *
 * Parameters:  iMaskArg - mask to determine which bit to set
 *              iFlagArg - TRUE/FALSE status to which particular bit has to be set
 *
 * Returns:     SUCCESS/FAILURE
\*****************************************************************/
int SSSetModulesInitializationsStatus(int iMaskArg, int iFlagArg)
{
    if(iFlagArg == TRUE)
        iSystemInitializationStatus |= iMaskArg;
    else if(iFlagArg == FALSE)
        iSystemInitializationStatus &= ~iMaskArg;
    else
        return FAILURE;

    return SUCCESS;
}
/****************************************************************\
 *
 * Function:    SSIsDebugBitOn
 *
 * Returns:     TRUE if Bit is on the given bit number. Otherwise, FALSE.
 *
\*****************************************************************/
int SSIsDebugBitOn(unsigned int uBitArg)
{
    return (SSuDebugBit & uBitArg) ? TRUE : FALSE;
}
/****************************************************************\
 *
 * Function:    SSSetDebugBit
 *
 * Returns:     none
 *
\*****************************************************************/
void SSSetDebugBit(unsigned int uBitArg)
{
    SSuDebugBit = uBitArg;
}
/****************************************************************\
 *
 * Function:    SSSetDebugBit
 *
 * Returns:     none
 *
\*****************************************************************/
unsigned int SSGetDebugBit( void )
{
    return SSuDebugBit;
}
