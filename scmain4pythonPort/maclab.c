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
 * Program:     Label macro module
 * File:        maclab.c
 * Functions:   MAInitLabels
 *              MAResolveLabel
 *              MACleanupLabels
 *              MADestroyLabelTree
 *              MASearchLabel
 *              MAFixupLabels
 *
 * Description: macro labels
 *           This module takes care of associating an instruction address
 *           with the label number given in a jump statement.  The address
 *           of the instruction is placed in the JUMP_DEST operand of the
 *           jump instruction, allowing for faster run time execution.
 *
 *           Procedure:
 *
 *           1) When starting to define a macro, call:
 *                   MAInitLabels();
 *
 *           2) After parsing each LABEL or jump-type instruction, call:
 *                   MAResolveLabel(instr);
 *
 *           3) When finished defining the macro, call:  (Note that this call
 *              will write into the macro body.  Do not call it if the
 *              macro body memory has been freed for any reason.)
 *                   MACleanupLabels();
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
\***************************************************************/

#include <malloc.h>
#include "sck.h"
#include "maclab.h"
#include "ser.h"
#include "scmem.h"

label_ptr   labelTreeRoot;              /* pointer points to root of the binary tree */
label_ptr   nowNode;                    /* current node */

int         iLabelInserted;             /* flag to signal label was inserted */
int         iLabelCode;                 /* hold the OC number of LABEL */
unsigned    uLabelNumber;               /* number or address that will jump to */
instr_ptr   CurrentInsAddr;             /* holding current instruction that is LABEL instruction */


/***************************************************************
 * Function Name:   MAInitLabels
 * Purpose:         Initializes label instruction.
 *                  - destroy the tree
 *                  - set the root to null
 *                  - set the inserted flag to false.
 ***************************************************************/
void MAInitLabels( )
{
    /* Check if the pointer to root of the tree is not NULL,
     * then destroy the tree */
    if( labelTreeRoot != NULL )
		MADestroyLabelTree( labelTreeRoot );

    labelTreeRoot = NULL;
    iLabelInserted = FALSE;
}

/***************************************************************
 * Function Name:   MADestroyLabelTree
 * Purpose:         Destroys label tree after finishing defining macro.
 *                  This is to ensure that label(s) are local within
 *                  one macro body.
 * Parameter:       labTreeArg - pointer to root of label tree.
 ***************************************************************/
void MADestroyLabelTree(label_ptr labTreeArg)
{
    /* Destroy the all the left node recursively */
    if( labTreeArg->m_leftLabel != NULL )
		MADestroyLabelTree( labTreeArg->m_leftLabel );
    /* then destroy all the right node recursively */
    if( labTreeArg->m_rightLabel != NULL )
		MADestroyLabelTree( labTreeArg->m_rightLabel );

    /* free memory of the tree */
    free( labTreeArg );
    return;
}

/***************************************************************
 * Function Name:   MAResolveLabel
 * Purpose:         Resolves label reference
 * Parameter:       instrArg - label number to be resolved
 * return:          SUCCESS or FAILURE
 ***************************************************************/
int MAResolveLabel(instr_ptr instrArg)
{
    instr_ptr Addr, nextInstr;

    /* Store instruction address in global */
    CurrentInsAddr = instrArg;

    /* get the OC number */
    iLabelCode = CurrentInsAddr->OC;

    /* get the label number/address */
    uLabelNumber = (unsigned)CurrentInsAddr->opr_ptr->next->opr.l;

    /* Search for label; if not existing, then define the label.
     * if there is a label number then return node, iLabelInserted, nowNode are also got set */
    labelTreeRoot = MASearchLabel( labelTreeRoot, uLabelNumber );
    if( labelTreeRoot == NULL)
        return FAILURE;
    /* check label is already in the tree after searched.
     * In the case if inserted, then just return success */
    if( !iLabelInserted )
	{
        /* this is the label instruction */
		if( iLabelCode == LABEL )
		{
            if( nowNode->m_defined ) return FAILURE;    /* Redefinition */
			nowNode->m_defined = TRUE;
			Addr = nowNode->m_AddrDefined;
			while(Addr)
			{
				nextInstr = Addr->opr_ptr->opr.p;
				Addr->opr_ptr->opr.p = CurrentInsAddr;
				Addr = nextInstr;
			}
			nowNode->m_AddrDefined = CurrentInsAddr;
		}
		else
		{
            /* this case is jump command */
			if( nowNode->m_defined )
				CurrentInsAddr->opr_ptr->opr.p = nowNode->m_AddrDefined;
			else
			{
				CurrentInsAddr->opr_ptr->opr.p = nowNode->m_AddrDefined;
				nowNode->m_AddrDefined = CurrentInsAddr;
			}
		}

	}
    return SUCCESS;
}

/***************************************************************
 * Function Name:   MASearchLabel
 * Purpose:         Searches label, inserts if defining
 *                  a non-existing label.
 * Parameter:       labelTreeArg - root of label tree
 *                  uLabelArg    - label to be searched or inserted
 ***************************************************************/
label_ptr MASearchLabel(label_ptr labTreeArg, unsigned uLabelArg)
{
    int iCmdPort;
    static int iNumberOfLabelsDefined = 0;

    iCmdPort = SERGetCmdPort();
    if(iNumberOfLabelsDefined++ > MAX_LABELS)
    {
#ifdef DEBUG
        SERPutsTxBuff(iCmdPort, "**search_label:  Too many labels in one macro**\n\r?");
#endif
        iNumberOfLabelsDefined = 0;
        return NULL;
    }
    if(labTreeArg == NULL )
	{
        labTreeArg = (label_ptr)malloc( sizeof(label_t) );
		if (!labTreeArg)
		{
#ifdef DEBUG
            SERPutsTxBuff(iCmdPort, "**search_label:  Out of memory**\n\r?");
#endif
            iNumberOfLabelsDefined = 0;
			return NULL;
		}
		if( iLabelCode == LABEL )
			labTreeArg->m_defined = TRUE;
		else
		{
			labTreeArg->m_defined = FALSE;
			CurrentInsAddr->opr_ptr->opr.l = 0;
		}
		iLabelInserted = TRUE;
        labTreeArg->m_label = uLabelArg;
        labTreeArg->m_AddrDefined = CurrentInsAddr;
		labTreeArg->m_leftLabel = labTreeArg->m_rightLabel = NULL;
    }
    else if( uLabelArg < labTreeArg->m_label )
    {
        labTreeArg->m_leftLabel = MASearchLabel( labTreeArg->m_leftLabel, uLabelArg );
        if( labTreeArg->m_leftLabel == NULL)
        {
            iNumberOfLabelsDefined = 0;
            return NULL;
        }
    }
    else if( uLabelArg > labTreeArg->m_label )
    {
        labTreeArg->m_rightLabel = MASearchLabel( labTreeArg->m_rightLabel, uLabelArg );
        if( labTreeArg->m_rightLabel == NULL)
        {
            iNumberOfLabelsDefined = 0;
            return NULL;
        }
    }
    else
	{
		nowNode = labTreeArg;
		iLabelInserted = FALSE;
    }

    iNumberOfLabelsDefined = 0;
    return labTreeArg;
}

/***************************************************************
 * Function Name:   MACleanupLabels
 * Purpose:         takes care of resolving any undefined labels and
 *                  then destroys the label tree.
 ***************************************************************/
void MACleanupLabels()
{
    MAFixupLabels(labelTreeRoot);
    MAInitLabels();

    return;
}

/***************************************************************
 * Function Name:   MAFixupLabels
 * Purpose:         This is a tree walking routine that searches for nodes with
 *                  undefined labels. If any such nodes are found, then the JUMP
 *                  instructions associated with the node have their jump destination
 *                  pointers set to NULL.  This allows for unambiguous generation of
 *                  a run time error for this condition.
 * Parameter:       labTreeArg - pointer to next node in tree.
 ***************************************************************/
void MAFixupLabels(label_ptr labTreeArg)
{
    instr_ptr instr, nextInstr;

    if (labTreeArg == NULL)
		return;

    MAFixupLabels(labTreeArg->m_leftLabel);
    MAFixupLabels(labTreeArg->m_rightLabel);

    /* If the label at this node was never defined,
       set all the associated pointers to NULL */
    if (!labTreeArg->m_defined)
	{
		instr = labTreeArg->m_AddrDefined;
		while(instr)
		{
			nextInstr = instr->opr_ptr->opr.p;
			instr->opr_ptr->opr.p = NULL;
			instr = nextInstr;
		}
    }
    return;
}
