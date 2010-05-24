/*************************************************************************/
/*                                                                       */
/*               Copyright Mentor Graphics Corporation 2004              */
/*                         All Rights Reserved.                          */
/*                                                                       */
/* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS  */
/* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS   */
/* SUBJECT TO LICENSE TERMS.                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* FILE NAME                                               VERSION       */
/*                                                                       */
/*      dm_extr.h                                      Nucleus PLUS 1.15 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      DM - Dynamic Memory Management                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains function prototypes of all functions          */
/*      accessible to other components.                                  */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      None                                                             */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      dm_defs.h                           Dynamic Management constants */
/*                                                                       */
/*************************************************************************/

#include        "plus/inc/dm_defs.h"        /* Include DM constants      */


/* Check to see if the file has been included already.  */

#ifndef DM_EXTR
#define DM_EXTR


/*  Initialization functions.  */

VOID            DMI_Initialize(VOID);


/* Error checking functions.  */

STATUS          DMCE_Create_Memory_Pool(NU_MEMORY_POOL *pool_ptr, CHAR *name,
                        VOID *start_address, UNSIGNED pool_size,
                        UNSIGNED min_allocation, OPTION suspend_type);
STATUS          DMCE_Delete_Memory_Pool(NU_MEMORY_POOL *pool_ptr);
STATUS          DMCE_Allocate_Memory(NU_MEMORY_POOL *pool_ptr, VOID **return_pointer,
                                UNSIGNED size, UNSIGNED suspend);
STATUS          DMCE_Deallocate_Memory(VOID *memory);


/* Core processing functions.  */

STATUS          DMC_Create_Memory_Pool(NU_MEMORY_POOL *pool_ptr, CHAR *name, 
                        VOID *start_address, UNSIGNED pool_size,
                        UNSIGNED min_allocation, OPTION suspend_type);
STATUS          DMC_Delete_Memory_Pool(NU_MEMORY_POOL *pool_ptr);
STATUS          DMC_Allocate_Memory(NU_MEMORY_POOL *pool_ptr, 
                     VOID **return_pointer, UNSIGNED size, UNSIGNED suspend);
STATUS          DMC_Deallocate_Memory(VOID *memory);


/* Supplemental service routines */
STATUS DMS_Allocate_Aligned_Memory(NU_MEMORY_POOL *pool_ptr,
                                   VOID **return_pointer, UNSIGNED size,
                                   UNSIGNED alignment, UNSIGNED suspend);


/* Information retrieval functions.  */

UNSIGNED        DMF_Established_Memory_Pools(VOID);
STATUS          DMF_Memory_Pool_Information(NU_MEMORY_POOL *pool_ptr, 
                  CHAR *name, VOID **start_address, UNSIGNED *pool_size,
                  UNSIGNED *min_allocation, UNSIGNED *available,
                  OPTION *suspend_type, UNSIGNED *tasks_waiting, 
                  NU_TASK **first_task);
UNSIGNED        DMF_Memory_Pool_Pointers(NU_MEMORY_POOL **pointer_list, 
                                                UNSIGNED maximum_pointers);
#endif





