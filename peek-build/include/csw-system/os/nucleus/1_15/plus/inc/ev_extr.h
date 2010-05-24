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
/*      ev_extr.h                                      Nucleus PLUS 1.15 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      EV - Event Group Management                                      */
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
/*      ev_defs.h                           Event Flag management consts */
/*                                                                       */
/*************************************************************************/

#include        "plus/inc/ev_defs.h"        /* Include EV constants      */


/* Check to see if the file has been included already.  */

#ifndef EV_EXTR
#define EV_EXTR


/*  Initialization functions.  */

VOID            EVI_Initialize(VOID);


/* Error checking functions.  */

STATUS          EVCE_Create_Event_Group(NU_EVENT_GROUP *group_ptr, CHAR *name);
STATUS          EVCE_Delete_Event_Group(NU_EVENT_GROUP *group_ptr);
STATUS          EVCE_Set_Events(NU_EVENT_GROUP *group_ptr, UNSIGNED events, 
                                                OPTION operation);
STATUS          EVCE_Retrieve_Events(NU_EVENT_GROUP *group_ptr, 
                        UNSIGNED requested_flags, OPTION operation, 
                        UNSIGNED *retrieved_flags, UNSIGNED suspend);

/* Core processing functions.  */


STATUS          EVC_Create_Event_Group(NU_EVENT_GROUP *group_ptr, CHAR *name);
STATUS          EVC_Delete_Event_Group(NU_EVENT_GROUP *group_ptr);
STATUS          EVC_Set_Events(NU_EVENT_GROUP *group_ptr, UNSIGNED events, 
                                                OPTION operation);
STATUS          EVC_Retrieve_Events(NU_EVENT_GROUP *group_ptr, 
                        UNSIGNED requested_flags, OPTION operation, 
                        UNSIGNED *retrieved_flags, UNSIGNED suspend);


/* Information retrieval functions.  */

UNSIGNED        EVF_Established_Event_Groups(VOID);
STATUS          EVF_Event_Group_Information(NU_EVENT_GROUP *group_ptr, 
                        CHAR *name, UNSIGNED *event_flags, 
                        UNSIGNED *tasks_waiting, NU_TASK **first_task);
UNSIGNED        EVF_Event_Group_Pointers(NU_EVENT_GROUP **pointer_list, 
                                                UNSIGNED maximum_pointers);
#endif





