/**
 * @file
 * @brief Public declarations for RVF API.
 *
 * This file contains public declarations and types definitions for the
 * Riviera Frame (RVF) API.
 *
 * In particular, this file provides definitions for the following topics:
 *
 * - memory management (memory banks and buffer allocation),
 * - timer management,
 * - task management,
 * - inter-task communication (messages) and event waiting,
 * - inter-task synchronization (mutexes).
 */

/*
 *  Author      David Lamy-Charrier
 *
 *  Date       	Modification
 *  ------------------------------------
 *   3/12/1999  Create
 *  11/15/1999  remove useless functions,
 *              change tasks priority and time_slicing
 *  11/15/1999  compliant to RV coding guidelines
 *  12/23/1999  change buffer management, add memory bank handling
 *
 * (C) Copyright 1999 by Texas Instruments Incorporated, All Rights Reserved
 */


#ifndef _RVF_API_H
#define _RVF_API_H

#include "general.h"
#include "rv/rv_general.h"
#include "rvf/rvf_target.h"
#include "config/rv.cfg"

#ifndef _WINDOWS
  #include "config/swconfig.cfg"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* macros used to trace messages */

// WCP patch: default config is no RV trace for WCP
#if (OP_WCP == 0)
  #define DEBUG_ENABLE 1
#endif


#ifndef DEBUG_ENABLE
#define rvf_send_trace
#else
void rvf_send_trace1(char * msg, UINT8 msg_length, UINT32 val, UINT8 trace_level, UINT32 swe_use_id);
#define rvf_send_trace rvf_send_trace1
#endif

#define T_RVF_GD_ID UINT8

/* return parameters definition */
typedef T_RV_RET T_RVF_RET;

#define	RVF_OK                  RV_OK
#define	RVF_NOT_SUPPORTED       RV_NOT_SUPPORTED
#define	RVF_NOT_READY           RV_NOT_READY
#define	RVF_MEMORY_ERR          RV_MEMORY_ERR
#define	RVF_INTERNAL_ERR        RV_INTERNAL_ERR
#define	RVF_INVALID_PARAMETER   RV_INVALID_PARAMETER

/**
 * @brief Memory bank states.
 *
 * The state a memory bank can take according to the currently used
 * memory, the watermark and the maximum size.
 */
typedef enum
{
    RVF_GREEN,  ///< Current used memory < watermark.
    RVF_YELLOW, ///< Watermark < Current used memory < maximum.
    RVF_RED     ///< Last call to rvf_get_buf() failed due to not
                ///  enough remaining memory.
} T_RVF_MB_STATUS;

/* task state */
typedef enum
{
    SUSPEND,
    RUNNING
} T_RVF_TASK_STATE;

/************************************************************************
** Mailbox definitions. Each task has 4 mailboxes that are used to
** send buffers to the task.
*/
#define RVF_TASK_MBOX_0    0
#define RVF_TASK_MBOX_1    1
#define RVF_TASK_MBOX_2    2
#define RVF_TASK_MBOX_3    3

#define RVF_NUM_TASK_MBOX  4

#define RVF_TASK_MBOX_0_EVT_MASK   0x0001
#define RVF_TASK_MBOX_1_EVT_MASK   0x0002
#define RVF_TASK_MBOX_2_EVT_MASK   0x0004
#define RVF_TASK_MBOX_3_EVT_MASK   0x0008

/************************************************************************
** Event definitions.
**
** There are 4 reserved events used to signal messages rcvd in task mailboxes.
** There are 4 reserved events used to signal timeout events.
** There are 8 general purpose events available for applications.
*/
#define RVF_MAX_EVENTS	        16

#define RVF_NUM_TASK_TIMERS     4

#define RVF_TIMER_0             0
#define RVF_TIMER_1             1
#define RVF_TIMER_2             2
#define RVF_TIMER_3             3

#define RVF_TIMER_0_EVT_MASK    0x0010
#define RVF_TIMER_1_EVT_MASK    0x0020
#define RVF_TIMER_2_EVT_MASK    0x0040
#define RVF_TIMER_3_EVT_MASK    0x0080

#define RVF_APPL_EVT_0          8
#define RVF_APPL_EVT_1          9
#define RVF_APPL_EVT_2          10
#define RVF_APPL_EVT_3          11
#define RVF_APPL_EVT_4          12
#define RVF_APPL_EVT_5          13
#define RVF_APPL_EVT_6          14
#define RVF_APPL_EVT_7          15

#define EVENT_MASK(evt)	   ((UINT16)0x0001 << evt)

#define MAX_HOSTING_TASKS	10
#define MAX_PARASITES		10


/// The memory pool identifier.
typedef UINT8 T_RVF_POOL_ID;

/// The identifier (index) of the pool of external memory.
#define RVF_POOL_EXTERNAL_MEM 0

#if RVF_ALLOC_INT_RAM == 1
  /// The identifier (index) of the pool of internal memory.
  #define RVF_POOL_INTERNAL_MEM 1
#else
  // Put everything into external memory
  #define RVF_POOL_INTERNAL_MEM (RVF_POOL_EXTERNAL_MEM)
#endif


/// The memory bank identifier.
typedef UINT16 T_RVF_MB_ID;

/// The memory bank name.
typedef char T_RVF_MB_NAME[RVF_MAX_MB_LEN];

#define T_RVF_BUFFER void
typedef void (*CALLBACK_FUNC)(void *);
typedef void (*MB_CALLBACK_FUNC)(T_RVF_MB_ID);

/**
 * @brief The memory bank parameters.
 *
 * This structure provides parameters needed to the creation of a memory
 * bank.
 * - The pool identifier @a pool_id allows to specify which kind of
 * memory pool (internal or external) the function rvf_get_buf() will use to
 * allocate a buffer.
 * - The @a size correspond to the maximum size all allocated
 * buffers shall not exceed. In such a case, the value RVF_RED is returned
 * by rvf_get_buf().
 * - The @a watermark allows an application to set a threshold from which the
 * value RVF_YELLOW will be returned by rvf_get_buf().
 */
typedef struct
{
    /// The pool identifier (RVF_POOL_EXTERNAL_MEM or RVF_POOL_INTERNAL_MEM).
    T_RVF_POOL_ID pool_id;
    /// The size of the memory bank (maximum allocated size).
    UINT32        size;
    /// The watermark, must respect the formula (0 < @a watermark <= @a size).
    UINT32        watermark;
} T_RVF_MB_PARAM;



/* Define a timer list entry
*/
typedef struct _tle
{
    T_RV_HDR     hdr;
    struct _tle* p_next;
    struct _tle* p_prev;
    UINT32       ticks;
    UINT16       event;  /* Event & param must be revised possibily       */
    UINT32       param;  /* sub struct pointed to by p_data if required   */
    UINT32       t_init;
    void*        p_data;
} T_RVF_TIMER_LIST_ENT;

typedef T_RVF_TIMER_LIST_ENT* P_NODE;

/* Define a timer list.
*/
typedef struct
{
    T_RVF_TIMER_LIST_ENT* p_first;
    T_RVF_TIMER_LIST_ENT* p_last;
    UINT32                last_ticks;
    UINT16                timerCnt;
} T_RVF_TIMER_LIST_Q;


/***********************************************************************
** This queue is a general purpose buffer queue, for application use.
*/
typedef struct
{
    void*  p_first;
    void*  p_last;
    UINT16 count;
} T_RVF_BUFFER_Q;

#define RVF_IS_QUEUE_EMPTY(p_q) (p_q.count == 0)

/*******************************************************
* Message parameter of "handle_timer()"
********************************************************/
#define T_RVF_TIMER_ID		UINT32

typedef struct
{
    T_RV_HDR       hdr;
    T_RVF_TIMER_ID tm_id;
    UINT8          cont;
    void*          action;
} T_RVF_TMS_MSG;

/* define a mutex structure */
typedef struct
{
    UINT32 words[11];
} T_RVF_MUTEX;

/* Task constants
*/
#ifndef TASKPTR
typedef void (*TASKPTR)(UINT32);
#endif


#define RVF_INVALID_TASK    (0xFF)
#define RVF_INVALID_MB_ID   (0xFFFF)
#define RVF_INVALID_MAILBOX (0xFF)
#define RVF_INVALID_ADDR_ID (0xFF)

/* Define the value that create pool will return if it fails
*/


/************************************************************************
 * Function prototypes
 ***********************************************************************/

/* Task management*/

/// Initialize the Riviera Frame.
/// @todo: remove from public API.
void            rvf_init(void);

T_RVF_RET       rvf_create_legacy_task(TASKPTR task_entry, UINT8 task_id, char *taskname, UINT8 *stack, UINT16 stacksize, UINT8 priority, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend);

T_RVF_RET       rvf_create_task (TASKPTR task_entry, T_RVF_G_ADDR_ID task_id, char *taskname, UINT8 *stack, UINT16 stacksize, UINT8 priority, UINT8 tcode, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend);
T_RVF_RET       rvf_resume_task( T_RVF_G_ADDR_ID taskid);
void            rvf_exit_task(T_RVF_G_ADDR_ID vtask);
T_RVF_RET       rvf_suspend_task(T_RVF_G_ADDR_ID vtask);
T_RVF_G_ADDR_ID rvf_get_taskid(void);
char*           rvf_get_taskname(void);
T_RVF_G_ADDR_ID rvf_get_context();

/* Message Handling */
typedef T_RV_HDR T_RVF_MSG ;
typedef UINT32	 T_RVF_MSG_ID;

T_RVF_RET       rvf_send_msg(T_RVF_G_ADDR_ID addr_id, void * p_msg);
T_RVF_MB_STATUS rvf_get_msg_buf(T_RVF_MB_ID mb_id,
                                UINT32 message_size,
                                T_RVF_MSG_ID msg_id,
                                T_RVF_MSG ** pp_msg);
T_RVF_RET       rvf_free_msg(T_RVF_MSG * p_msg);
T_RVF_RET       rvf_free_timer_msg(T_RVF_MSG *p_msg);



/* To send buffers and events between tasks*/
void*           rvf_read_mbox  (UINT8 mbox);
void*           rvf_read_addr_mbox (T_RVF_G_ADDR_ID  task_id, UINT8 mbox);
UINT8           rvf_send_event (T_RVF_G_ADDR_ID task_id, UINT16 event);

/* task synchronization */
T_RVF_RET       rvf_initialize_mutex( T_RVF_MUTEX * mutex);
//OMAPS72906 add the static mutex version
T_RVF_RET       rvf_initialize_static_mutex( T_RVF_MUTEX * mutex);

T_RVF_RET       rvf_lock_mutex( T_RVF_MUTEX * mutex);
T_RVF_RET       rvf_unlock_mutex( T_RVF_MUTEX * mutex);
T_RVF_RET       rvf_delete_mutex( T_RVF_MUTEX * mutex);


/* To manage memory */

/// Create a memory bank and returns its identifier.
T_RVF_RET       rvf_create_mb(T_RVF_MB_NAME  mb_name,
                              T_RVF_MB_PARAM mb_param,
                              T_RVF_MB_ID*   mb_id);

/// Delete a memory bank.
T_RVF_RET       rvf_delete_mb(T_RVF_MB_NAME mb_name);

/// Get the memory bank identifier from an existing one.
T_RVF_RET       rvf_get_mb_id(T_RVF_MB_NAME mb_name, T_RVF_MB_ID* mb_id);

/// Get the current status of the memory bank.
T_RVF_MB_STATUS rvf_get_mb_status(T_RVF_MB_ID mb_id);

T_RVF_RET       rvf_mb_is_used(T_RVF_MB_NAME mb_name, UINT8* isUsed) ;

/// Allocate a buffer from a memory bank.
T_RVF_MB_STATUS rvf_get_buf(T_RVF_MB_ID    mb_id,
                            UINT32         buffer_size,
                            T_RVF_BUFFER** p_buffer);

/// Change the memory bank on which a buffer is counted.
T_RVF_MB_STATUS rvf_count_buf(T_RVF_MB_ID mb_id, T_RVF_BUFFER * p_buffer);

/// Free a buffer.
T_RVF_RET       rvf_free_buf( T_RVF_BUFFER * p_buffer);

T_RVF_RET       rvf_set_callback_func(T_RVF_MB_ID mb_id, MB_CALLBACK_FUNC func);
T_RVF_RET       rvf_change_callback_func(T_RVF_MB_ID mb_id, MB_CALLBACK_FUNC func);

void            rvf_get_protected_buf(T_RVF_MB_ID mb_id, UINT32 buffer_size, T_RVF_BUFFER** p_buffer);


/// Get the size of an allocated buffer.
UINT32          rvf_get_buf_size(void* ptr);

/// Get parameters of a memory bank.
T_RVF_RET       rvf_get_mb_param(T_RVF_MB_NAME   mb_name,
                                 T_RVF_MB_PARAM* param);

/// Change parameters of a memory bank.
T_RVF_RET       rvf_set_mb_param(T_RVF_MB_NAME   mb_name,
                                 T_RVF_MB_PARAM* param);


/* User buffer queue management*/
T_RVF_RET       rvf_enqueue (T_RVF_BUFFER_Q *p_q, void *p_buf);
T_RVF_RET       rvf_enqueue_head (T_RVF_BUFFER_Q *p_q, void *p_buf);
void *          rvf_dequeue  (T_RVF_BUFFER_Q *p_q);
T_RVF_BUFFER *  rvf_scan_next (T_RVF_BUFFER_Q * p_q, T_RVF_BUFFER * p_buf);
T_RVF_RET       rvf_remove_from_queue (T_RVF_BUFFER_Q * p_q, T_RVF_BUFFER * p_buf);

/* Timer management*/

UINT16          rvf_wait(UINT16 flag, UINT32 ticks);
UINT16          rvf_evt_wait(T_RVF_G_ADDR_ID rtask, UINT16 flag, UINT32 timeout) ;
void            rvf_delay(UINT32 ticks);

void            rvf_start_timer(UINT8 tnum, UINT32 ticks, BOOLEAN is_continuous);
void            rvf_stop_timer (UINT8 tnum);
char *          rvf_get_time_stamp(char *tbuf);
UINT32          rvf_get_tick_count(void);
void            rvf_init_timer_list (T_RVF_TIMER_LIST_Q *p_timer_listq);
void            rvf_init_timer_list_entry (T_RVF_TIMER_LIST_ENT  *p_tle);
UINT16          rvf_update_timer_list (T_RVF_TIMER_LIST_Q *p_timer_listq);
void            rvf_add_to_timer_list (T_RVF_TIMER_LIST_Q *p_timer_listq, T_RVF_TIMER_LIST_ENT  *p_tle);
void            rvf_remove_from_timer_list (T_RVF_TIMER_LIST_Q *p_timer_listq, T_RVF_TIMER_LIST_ENT  *p_tle);
T_RVF_TIMER_LIST_ENT* rvf_get_expired_entry (T_RVF_TIMER_LIST_Q *p_timer_listq);


#define T_RV_TM_ID UINT32

/*************************************************************************/
/* :)  USER FRIENDLY TIMER MANAGEMENT API consisting of add, delete and modify timer */
/*************************************************************************/
/* Returns 0 on failure */
T_RVF_TIMER_ID	rvf_create_timer(T_RVF_G_ADDR_ID g_addrId,
                                 UINT32          timerDuration,
                                 BOOLEAN         isContinuous,
                                 SHORT (*timeout_action) (int ,int,USHORT));


void            rvf_del_timer(T_RV_TM_ID tm_id);

void            rvf_reset_timer(T_RV_TM_ID tm_id,
                                UINT32     new_duration,
                                BOOLEAN    isContinuous);


/* Disable Interrupts, Enable Interrupts*/
void            rvf_enable(void);
void            rvf_disable(void);

/* Trace for debug purposes*/
void            rvf_dump_mem();
void            rvf_dump_tasks();
void            rvf_dump_pool();

/* specific function */
T_RVF_BUFFER*   rvf_wait_for_specific_msg(UINT16 msg_code,
                                          UINT8  mbox,
                                          UINT32 timeout);

//for defining rvm_error functions
#include "rvm/rvm_api.h"
#include "rvm/rvm_gen.h"
#define	T_RVM_STRING			char *
#define	T_RVM_ERROR_TYPE		UINT32

//#include "rvm/rvm_i.h"
T_RVM_RETURN	rvm_error ( T_RVM_NAME swe_name, T_RVM_RETURN error_cause,
							T_RVM_ERROR_TYPE error_type, T_RVM_STRING error_msg);


/* Trace definitions */
/// This value will not be displayed by the rvf_send_trace() function.
#define NULL_PARAM   4294967295
#define TRACE_MB_ID  (RVF_MAX_REAL_MB - 1)

#ifdef __cplusplus
}
#endif

#endif // _RVF_API_H
