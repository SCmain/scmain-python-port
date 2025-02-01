/***************************************************************\
 *
 * Program:     Macro Labels
 * File:        maclab.h
 * Functions:   MAInitLabels
 *              MAResolveLabel
 *              MACleanupLabels
 *              MADestroyLabelTree
 *              MASearchLabel
 *              MAFixupLabels
 *
 * Description: handling label inside macro
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_MACLAB_H
#define _H_MACLAB_H

#include "cmdfns.h"

/* maximum number of labels allowed in one macro */
#define MAX_LABELS 100

/********** VARIABLES **********/

/* structure holds the information for each label */
struct label_st
{
	unsigned m_label;
	unsigned m_defined;
	instr_ptr m_AddrDefined;
    struct label_st  *m_leftLabel;
    struct label_st  *m_rightLabel;
};

typedef struct label_st label_t;
typedef struct label_st  *label_ptr;


/********** FUNCTION PROTOTYPES **********/

/***************************************************************
 * Function Name:   MAInitLabels
 * Purpose:         Initializes label instruction.
 *                  - destroy the tree
 *                  - set the root to null
 *                  - set the inserted flag to false.
 ***************************************************************/
void        MAInitLabels(void);
/***************************************************************
 * Function Name:   MAResolveLabel
 * Purpose:         Resolves label reference
 * Parameter:       instrArg - label number to be resolved
 * return:          SUCCESS or FAILURE
 ***************************************************************/
int         MAResolveLabel(instr_ptr instr);
/***************************************************************
 * Function Name:   MACleanupLabels
 * Purpose:         takes care of resolving any undefined labels and
 *                  then destroys the label tree.
 ***************************************************************/
void        MACleanupLabels(void);
/***************************************************************
 * Function Name:   MADestroyLabelTree
 * Purpose:         Destroys label tree after finishing defining macro.
 *                  This is to ensure that label(s) are local within
 *                  one macro body.
 * Parameter:       labTreeArg - pointer to root of label tree.
 ***************************************************************/
void        MADestroyLabelTree(label_ptr labTreeArg);
/***************************************************************
 * Function Name:   MASearchLabel
 * Purpose:         Searches label, inserts if defining
 *                  a non-existing label.
 * Parameter:       labelTreeArg - root of label tree
 *                  uLabelArg    - label to be searched or inserted
 ***************************************************************/
label_ptr   MASearchLabel(label_ptr labTreeArg, unsigned uLabelArg);
/***************************************************************
 * Function Name:   MAFixupLabels
 * Purpose:         This is a tree walking routine that searches for nodes with
 *                  undefined labels. If any such nodes are found, then the JUMP
 *                  instructions associated with the node have their jump destination
 *                  pointers set to NULL.  This allows for unambiguous generation of
 *                  a run time error for this condition.
 * Parameter:       labTreeArg - pointer to next node in tree.
 ***************************************************************/
void        MAFixupLabels(label_ptr labTreeArg);

#endif
