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
/*      hi_extr.h                                      Nucleus PLUS 1.15 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      HI - History Management                                          */
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
/*      hi_defs.h                           History Management constants */
/*                                                                       */
/*************************************************************************/

#include        "plus/inc/hi_defs.h"        /* Include HI constants      */


/* Check to see if the file has been included already.  */

#ifndef HI_EXTR
#define HI_EXTR


/*  Initialization functions.  */

VOID            HII_Initialize(VOID);


/* Core processing functions.  */

VOID            HIC_Disable_History_Saving(VOID);
VOID            HIC_Enable_History_Saving(VOID);
VOID            HIC_Make_History_Entry_Service(UNSIGNED param1, 
                                        UNSIGNED param2, UNSIGNED param3);
VOID            HIC_Make_History_Entry(DATA_ELEMENT id, UNSIGNED param1,
                                        UNSIGNED param2, UNSIGNED param3);
STATUS          HIC_Retrieve_History_Entry(DATA_ELEMENT *id, UNSIGNED *param1,
                                        UNSIGNED *param2, UNSIGNED *param3,
                                        UNSIGNED *time, NU_TASK **task,
                                        NU_HISR **hisr);

#endif





