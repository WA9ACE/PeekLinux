/**
 * @file	camd_i.h
 *
 * Internal definitions for CAMD.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __CAMD_INST_I_H_
#define __CAMD_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#ifndef _WINDOWS
//#include "nucleus.h"
#include "inth/sys_inth.h"
#endif

#include "camd/camd_cfg.h"
#include "camd/camd_api.h"

#include "camd/camd_state_i.h"

/** Macro used for tracing CAMD messages. */
#ifdef REMU
#define CAMD_ENABLE_TRACE
#endif

#ifdef CAMD_ENABLE_TRACE
#define CAMD_SEND_TRACE(string, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, CAMD_USE_ID)
#define CAMD_SEND_TRACE_PARAM(string, param, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), (UINT32)(param), trace_level, CAMD_USE_ID)
#else
#define CAMD_SEND_TRACE(string, trace_level) /* nothing */
#define CAMD_SEND_TRACE_PARAM(string, param, trace_level) /* nothing */
#endif

//#define CAMD_ENABLE_API_TRACE
#ifdef CAMD_ENABLE_API_TRACE
#define CAMD_API_TRACE(str) CAMD_SEND_TRACE((str), RV_TRACE_LEVEL_DEBUG_HIGH)
#define CAMD_API_TRACE_PARAM(string, param) \
    rvf_send_trace (string, (sizeof(string) - 1), (UINT32)(param), RV_TRACE_LEVEL_DEBUG_HIGH, CAMD_USE_ID)
#else
#define CAMD_API_TRACE(str)		/* nothing */
#define CAMD_API_TRACE_PARAM(str,p)	/* nothing */
#endif

/** assert macros **/
#define CAMD_ASSERT_PREFIX "CAMD Assertion failed: "
#define CAMD_ASSERT(expr) do { if(!(expr)) \
   rvf_send_trace(CAMD_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line", \
           sizeof(CAMD_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line")-1, \
           __LINE__, RV_TRACE_LEVEL_ERROR, CAMD_USE_ID); } while(0)
#define CAMD_ASSERT2(expr, fail_code) do {if(!(expr)) { \
   rvf_send_trace(CAMD_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line", \
           sizeof(CAMD_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line")-1, \
           __LINE__, RV_TRACE_LEVEL_ERROR, CAMD_USE_ID); fail_code;}} while(0)

/** events **/
#define CAMD_RETURN_QUEUE_EVENT (1<<15)

/**
 * The Control Block buffer of CAMD, which gathers all 'Global variables'
 * used by CAMD instance.
 *
 * A structure should gathers all the 'global variables' of CAMD instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_CAMD_ENV_CTRL_BLK buffer is allocated when creating CAMD instance and is 
 * then always refered by CAMD instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
    T_CAMD_INTERNAL_STATE state;
    BOOL capturemode;
    BOOL initialised;
    T_RVM_RETURN (*error_ft) (T_RVM_NAME swe_name,
            T_RVM_RETURN error_cause,
            T_RVM_ERROR_TYPE error_type,
            T_RVM_STRING error_msg);
    T_RVF_MB_ID prim_mb_id;
    T_RVF_MB_ID sec_mb_id;
    T_RVF_ADDR_ID addr_id;
    BOOL sensor_enabled;
    BOOL capture_mode;
    BOOL snapshot_parameters_valid;
    T_CAMD_PARAMETERS snapshot_parameters;
    BOOL viewfinder_parameters_valid;
    T_CAMD_PARAMETERS viewfinder_parameters;
    struct
    {
        T_RVF_G_ADDR_ID addr_id;
        UINT16 event;
        T_RVF_BUFFER_Q queue_obj;
    }
    return_queue;
    T_RV_RETURN_PATH path_to_return_queue;
    T_RVF_MUTEX framebuf_mutex;
    BOOL start_viewfinder;
    volatile int camd_flag;
    T_CAMD_MSG * Camd_App_buffer;
    T_CAMD_MSG * Camd_dma_buffer;
}
T_CAMD_ENV_CTRL_BLK;

/** External ref "global variables" structure. */
extern T_CAMD_ENV_CTRL_BLK *camd_env_ctrl_blk_p;
extern T_RVM_RETURN camd_send_response_to_client (UINT32 msg_id,
        T_CAMD_MSG * msg_p);
extern void *camd_return_queue_get_msg (void);
extern void camd_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id,
        UINT16 rq_event,
        T_RV_RETURN_PATH *
        path_to_return_queue_p);
extern T_RVF_RET camd_queue_request (T_CAMD_MSG * msg_p);
extern void camd_return_queue_flush (void);

#define CLK_FREQ1 0x0CC0
#define CLK_FREQ2 0x5D00
#define FAST_CLK_FREQ1 0x0CE0
#define FAST_CLK_FREQ2 0x2E00


#endif /* __CAMD_INST_I_H_ */
