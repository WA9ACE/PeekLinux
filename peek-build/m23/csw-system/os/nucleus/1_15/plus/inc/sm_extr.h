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
/*      sm_extr.h                                      Nucleus PLUS 1.15 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      SM - Semaphore Management                                        */
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
/*      sm_defs.h                           Semaphore Management constant*/
/*                                                                       */
/*************************************************************************/

#include        "plus/inc/sm_defs.h"        /* Include SM constants      */


/* Check to see if the file has been included already.  */

#ifndef SM_EXTR
#define SM_EXTR


/*  Initialization functions.  */

VOID            SMI_Initialize(VOID);


/* Error checking core functions.  */

STATUS          SMCE_Create_Semaphore(NU_SEMAPHORE *semaphore_ptr, CHAR *name,
                        UNSIGNED initial_count, OPTION suspend_type);
STATUS          SMCE_Delete_Semaphore(NU_SEMAPHORE *semaphore_ptr);
STATUS          SMCE_Obtain_Semaphore(NU_SEMAPHORE *semaphore_ptr, 
                                                        UNSIGNED suspend);
STATUS          SMCE_Release_Semaphore(NU_SEMAPHORE *semaphore_ptr);


/* Error checking supplemental functions.  */

STATUS          SMSE_Reset_Semaphore(NU_SEMAPHORE *semaphore_ptr, 
                                                    UNSIGNED initial_count);


/* Core processing functions.  */

STATUS          SMC_Create_Semaphore(NU_SEMAPHORE *semaphore_ptr, CHAR *name,
                        UNSIGNED initial_count, OPTION suspend_type);
STATUS          SMC_Delete_Semaphore(NU_SEMAPHORE *semaphore_ptr);
STATUS          SMC_Obtain_Semaphore(NU_SEMAPHORE *semaphore_ptr, 
                                                        UNSIGNED suspend);
STATUS          SMC_Release_Semaphore(NU_SEMAPHORE *semaphore_ptr);


/* Supplemental processing functions.  */

STATUS          SMS_Reset_Semaphore(NU_SEMAPHORE *semaphore_ptr, 
                                                UNSIGNED initial_count);


/* Information retrieval functions.  */

UNSIGNED        SMF_Established_Semaphores(VOID);
STATUS          SMF_Semaphore_Information(NU_SEMAPHORE *semaphore_ptr, 
                  CHAR *name, UNSIGNED *current_count, OPTION *suspend_type, 
                  UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        SMF_Semaphore_Pointers(NU_SEMAPHORE **pointer_list, 
                                                UNSIGNED maximum_pointers);
#endif




