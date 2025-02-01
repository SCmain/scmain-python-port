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
 * File:        scmem.c
 * Functions:   HPHeapDump
 *              HPHeapStat
 *              HPmalloc
 *              HPfree
 *              HPGetMemLowFlag
 *              HPPreallocate
 *
 * Description: Tests the heap memory and NVSRAM memory for
 *      status and availability
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 *
 ****************************************************************/

#include <malloc.h>
#include <errno.h>
#include "ser.h"
#include "sck.h"
#include "scmem.h"
#include "scstat.h"
#include "scmac.h"

#define MAX_HEAP_MEM    640
#define MAX_PREALLOCATE 4096

/* global for heap module */
char *pHPPreAllocMemory;  /* preallocated memory pointer */
int iHPMemLowFlag = FALSE;      /* set true if memory low */

/****************************************************************
 *
 * Function:    HPGetMemLowFlag
 *
 * Abstract: This function returns the mem flag.
 *
 * Parameters:  None
 *
 * Returns:     iHPMemLowFlag
 *
 ***************************************************************/
int HPGetMemLowFlag()
{
    return iHPMemLowFlag;
}

/****************************************************************
 *
 * Function:    HPPreAllocate
 *
 * Abstract: This function allocates memory of 2K.
 *
 * Parameters:  None
 *
 * Returns:     SUCCESS or FAILURE
 *
 ***************************************************************/
int HPPreAllocate()
{
    pHPPreAllocMemory = (char *) malloc (MAX_PREALLOCATE);
    if (pHPPreAllocMemory)
        return SUCCESS;
    return FAILURE;
}

/****************************************************************
 *
 * Function:    HPmalloc
 *
 * Abstract: This function replaces malloc() used widely in many modules.
 *           This function allocates memory, and if malloc fails, it frees up the pre-allocated
 *           memory of 2K and set the memory flag and reallocate the memory. If again fails, it
 *           returns NULL, otherwise returns the pointer to the mem.
 *
 * Parameters:  MemSizeArg     (in) size_t memory to allocate
 *
 * Returns:     NULL or pointer to the memory allocated
 *
 * Replaces:    malloc() used in various places
 ***************************************************************/
void * HPmalloc(size_t MemSizeArg)
{
    char * pMem;

    pMem = (char *) malloc(MemSizeArg);
    if(pMem == NULL)
    {   /* memory not available, so let's free preallocated memory */
        /* and re-malloc and set the flag */
        iHPMemLowFlag = TRUE;
        SSSetStatusWord2(LOW_MEMORY, TRUE);
        free(pHPPreAllocMemory);
        if (MRGetMacroIntegerVars(MR_DEFINING_MACRO))
            return NULL;
        pMem = (char *) malloc(MemSizeArg);
//        if(pMem == NULL)
//            return NULL;
    }
    return pMem;
}

/****************************************************************
 *
 * Function:    HPfree
 *
 * Abstract: This function replaces _ffree() or free() used widely in many modules.
 *           This function frees memory, and if mem Flag is on, it checks heap size and
 *           see if 2K is available to do malloc. If available, allocate 2K of PreAllocMemory, and
 *           set off the mem Flag.
 *
 * Parameters:  pMem     (in) void * pointer to memory to free
 *
 * Returns:     none
 *
 * Replaces:    _ffree() & free() used in various places
 ***************************************************************/
void HPfree(void *pMem)
{
    /* first, free the memory */
    free((void *)pMem);

    /* check if low memory flag is on */
    if(iHPMemLowFlag)
    {
        /* need to preallocate the memory and reset the mem flag */
        if(HPPreAllocate() == SUCCESS)
        {
            iHPMemLowFlag = FALSE;
            SSSetStatusWord2(LOW_MEMORY, FALSE);
        }
    }
}


/****************************************************************
 *
 * Function:    HPHeapDump
 *
 * Abstract: This function is multi-purpose. It starts by
 *      finding the amount of heap space used and the amount
 *      freed during the last operation. Then it optimizies the
 *      heap. Then it checks the B: drive (NVSRAM) and reports on
 *      the amount of memory still available. Finally it
 *      allocates memory in 1kB blocks until there are no more
 *      contiguous 1kB blocks of memory left. It then frees
 *      that memory and exits.
 *
 * Parameters:  lpHeapResultsArg   (out) The 4 numbers that are displayed during this function
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    heapdump
 * Used in:     execute, heap
 *
 ***************************************************************/
int HPHeapDump(long *lpHeapResultsArg)
{
//    struct _heapinfo stHeapInfo;    /* Returns information about the heap. */
//    struct diskfree_t stDrive;      /* Used to get information on the B: drive (NVSRAM) */
    int iHeapStat;                  /* Status code returned by heap query functions. */
//    int iHeapUsedIndex;             /* Maintains the index to the 1kB block that are allocated into
//                                     * an array of pointers to find the max amount of memory still available. */
    unsigned long ulHeapUsed, ulHeapFree;   /* Keeps track of the quantity of heap space
                                             * used and the amount freed during the last op. */
//    unsigned long ulNVSRAMFree;     /* The quantity of NVSRAM space still available. */
    char caOutMsg[80];
//    char **cpMemAvailDummy;           /* Array of pointers that 1kB blocks are
//                                             * allocated into. */

    /* Check heap status. Should be OK at start of heap. */
//    if (HPHeapStat(_fheapchk()) == FAILURE)
//        return FAILURE;

    /* Initialize variables. */
//    stHeapInfo._pentry = NULL;
//    ulHeapUsed = ulHeapFree = 0;

    /* Find out how much of the heap is used and how much was freed
     * during the last operation. */
//    while ((iHeapStat = _fheapwalk(&stHeapInfo)) == _HEAPOK)
//   {
//        if (stHeapInfo._useflag == _USEDENTRY)
//            ulHeapUsed += stHeapInfo._size;
//        else
//            ulHeapFree += stHeapInfo._size;
//    }

    /* Print out the results to the command port. */
    sprintf(caOutMsg, "Total used heap space = %ld\r\n", ulHeapUsed);
    lpHeapResultsArg[0] = (signed long)(ulHeapUsed);
    if (SERPutsTxBuff(SERGetCmdPort(), caOutMsg) == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;
    sprintf(caOutMsg, "Total freed heap space = %ld\r\n", ulHeapFree);
    lpHeapResultsArg[1] = (signed long)(ulHeapFree);
    if (SERPutsTxBuff(SERGetCmdPort(), caOutMsg) == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    /* Report on the heap status again like at the beginning. Make sure nothing
     * was screwed up during the _heapwalk query. */
    if (HPHeapStat(iHeapStat) == FAILURE)
        return FAILURE;

    /* Optimize the heap. */

    return SUCCESS;
}


/****************************************************************
 *
 * Function:    HPHeapStat
 *
 * Abstract: Reports on the status returned by _heapwalk,
 *      _heapset, or _heapchk. This function is the first
 *      call made to check initially that the heap is OK.
 *      It is called a second time after the check for heap
 *      space used and freed during the last operation. It is
 *      basically a status printing function.
 *
 * Parameters:
 *      iHeapStatArg    (in) The status returned by _heapwalk,
 *                          _heapset, or _heapchk
 *
 * Returns:     SUCCESS or FAILURE
 *
 * Replaces:    heapstat
 * Used in:     heap
 *
 ***************************************************************/
int HPHeapStat(int iHeapStatArg)
{
    if (SERPutsTxBuff(SERGetCmdPort(), "Heap status: ") == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    /* Based on the heap query function results, print out the general status
     * of the heap to the command port. */
//    switch (iHeapStatArg)
//    {
//        case _HEAPOK:
//            if (SERPutsTxBuff(SERGetCmdPort(), "OK - heap is fine") == FAILURE)
//                return FAILURE;
//            break;
//        case _HEAPEMPTY:
//            if (SERPutsTxBuff(SERGetCmdPort(), "OK - empty heap") == FAILURE)
//                return FAILURE;
//            break;
//        case _HEAPEND:
//            if (SERPutsTxBuff(SERGetCmdPort(), "OK - end of heap") == FAILURE)
//                return FAILURE;
//            break;
//        case _HEAPBADPTR:
//            if (SERPutsTxBuff(SERGetCmdPort(), "ERROR - bad pointer to heap") == FAILURE)
//                return FAILURE;
//            break;
//        case _HEAPBADBEGIN:
//            if (SERPutsTxBuff(SERGetCmdPort(), "ERROR - bad start of heap") == FAILURE)
//                return FAILURE;
//            break;
//        case _HEAPBADNODE:
//            if (SERPutsTxBuff(SERGetCmdPort(), "ERROR - bad node in heap") == FAILURE)
//                return FAILURE;
//            break;
//    }
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    if (SERPutsTxBuff(SERGetCmdPort(), "\r\n") == FAILURE)
        return FAILURE;
    if (SERFlushTxBuff(SERGetCmdPort()) == FAILURE)
        return FAILURE;

    return SUCCESS;
}
