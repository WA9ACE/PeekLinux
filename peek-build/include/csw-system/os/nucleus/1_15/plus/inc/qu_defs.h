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
/*      qu_defs.h                                      Nucleus PLUS 1.15 */
/*                                                                       */
/* COMPONENT                                                             */
/*                                                                       */
/*      QU - Queue Management                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This file contains data structure definitions and constants for  */
/*      the message Queue component.                                     */
/*                                                                       */
/* DATA STRUCTURES                                                       */
/*                                                                       */
/*      QU_QCB                              Queue control block          */
/*      QU_SUSPEND                          Queue suspension block       */
/*                                                                       */
/* DEPENDENCIES                                                          */
/*                                                                       */
/*      cs_defs.h                           Common service definitions   */
/*      tc_defs.h                           Thread Control definitions   */
/*                                                                       */
/*************************************************************************/

#include        "plus/inc/cs_defs.h"        /* Common service constants  */
#include        "plus/inc/tc_defs.h"        /* Thread control constants  */


/* Check to see if the file has been included already.  */

#ifndef QU_DEFS
#define QU_DEFS


/* Define constants local to this component.  */

#define         QU_QUEUE_ID             0x51554555UL


/* Define the Queue Control Block data type.  */

typedef struct QU_QCB_STRUCT 
{
    CS_NODE             qu_created;            /* Node for linking to    */
                                               /*   created queue list   */
    UNSIGNED            qu_id;                 /* Internal QCB ID        */
    CHAR                qu_name[NU_MAX_NAME];  /* Queue name             */
    BOOLEAN             qu_fixed_size;         /* Fixed-size messages?   */
    BOOLEAN             qu_fifo_suspend;       /* Suspension type flag   */
#if     PAD_2
    DATA_ELEMENT        qu_padding[PAD_2];
#endif
    UNSIGNED            qu_queue_size;         /* Total size of queue    */
    UNSIGNED            qu_messages;           /* Messages in queue      */
    UNSIGNED            qu_message_size;       /* Size of each message   */
    UNSIGNED            qu_available;          /* Available words        */
    UNSIGNED_PTR        qu_start;              /* Start of queue area    */
    UNSIGNED_PTR        qu_end;                /* End of queue area + 1  */
    UNSIGNED_PTR        qu_read;               /* Read pointer           */
    UNSIGNED_PTR        qu_write;              /* Write pointer          */
    UNSIGNED            qu_tasks_waiting;      /* Number of waiting tasks*/
    struct QU_SUSPEND_STRUCT
                       *qu_urgent_list;        /* Urgent message suspend */
    struct QU_SUSPEND_STRUCT
                       *qu_suspension_list;    /* Suspension list        */
} QU_QCB;    


/* Define the queue suspension structure.  This structure is allocated off of
   the caller's stack.  */
   
typedef struct QU_SUSPEND_STRUCT
{
    CS_NODE             qu_suspend_link;       /* Link to suspend blocks */
    QU_QCB             *qu_queue;              /* Pointer to the queue   */
    TC_TCB             *qu_suspended_task;     /* Task suspended         */
    UNSIGNED_PTR        qu_message_area;       /* Pointer to message area*/
    UNSIGNED            qu_message_size;       /* Message size requested */
    UNSIGNED            qu_actual_size;        /* Actual size of message */
    STATUS              qu_return_status;      /* Return status          */
} QU_SUSPEND;

#endif




