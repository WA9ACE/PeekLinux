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
/*      io_extr.h                                      Nucleus PLUS 1.15 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      IO - Input/Output Driver Management                              */
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
/*      io_defs.h                           I/O Driver Management consts */
/*      tc_defs.h                           Thread control constants     */
/*                                                                       */
/*************************************************************************/

#include        "plus/inc/io_defs.h"        /* Include IO constants      */
#include        "plus/inc/tc_defs.h"        /* Thread control constants  */


/* Check to see if the file has been included already.  */

#ifndef IO_EXTR
#define IO_EXTR


/*  Initialization functions.  */

VOID            IOI_Initialize(VOID);


/* Error checking functions.  */

STATUS          IOCE_Create_Driver(NU_DRIVER *driver, CHAR *name, 
                    VOID (*driver_entry)(NU_DRIVER *, NU_DRIVER_REQUEST *));
STATUS          IOCE_Delete_Driver(NU_DRIVER *driver);
STATUS          IOCE_Request_Driver(NU_DRIVER *driver, 
                                                NU_DRIVER_REQUEST *request);
STATUS          IOCE_Resume_Driver(NU_TASK *task);
STATUS          IOCE_Suspend_Driver(VOID (*terminate_routine)(VOID *),
                                      VOID *information, UNSIGNED timeout);


/* Core processing functions.  */

STATUS          IOC_Create_Driver(NU_DRIVER *driver, CHAR *name, 
                    VOID (*driver_entry)(NU_DRIVER *, NU_DRIVER_REQUEST *));
STATUS          IOC_Delete_Driver(NU_DRIVER *driver);
STATUS          IOC_Request_Driver(NU_DRIVER *driver, 
                                                NU_DRIVER_REQUEST *request);
STATUS          IOC_Resume_Driver(NU_TASK *task);
STATUS          IOC_Suspend_Driver(VOID (*terminate_routine)(VOID *),
                                        VOID *information, UNSIGNED timeout);


/* Information retrieval functions.  */

UNSIGNED        IOF_Established_Drivers(VOID);
UNSIGNED        IOF_Driver_Pointers(NU_DRIVER **pointer_list, 
                                                UNSIGNED maximum_pointers);

#endif





