/**
 * @file  nan_bm_i.h
 *
 * Internal definitions for NAN_BM.
 *
 * @author  J.A. Renia
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/27/2006 J.A. Renia     Create.
 *
 * (C) Copyright 2006 by ICT Embedded, All Rights Reserved
 */

#ifndef __NAN_BM_INST_I_H_
#define __NAN_BM_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#ifndef _WINDOWS
#include "nucleus.h"
#include "inth/sys_inth.h"
#endif

#include "nan/nan_cfg.h"
#include "nan/nan_api.h"
#include "nan/nan_message.h"

#include "nan/nan_state_i.h"
#include "swconfig.cfg"

#define NAN_BM_ASSERT_PREFIX "NAND_BM Assertion failed: "

#define NAN_BM_ASSERT(expr)

// to enable the assert statements, uncomment the following
/*
#define NAN_BM_ASSERT(expr) do { if(!(expr)) \
       rvf_send_trace(NAN_BM_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line",  \
               sizeof(NAN_BM_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line")-1, \
                __LINE__, RV_TRACE_LEVEL_ERROR, NAN_USE_ID); rvf_delay(1000);} while(0)
*/

#define NAN_BM_ASSERT2(expr, fail_code)
// to enable the assert statements, uncomment the following
/*
#define NAN_BM_ASSERT2(expr, fail_code) do {if(!(expr)) {  \
      rvf_send_trace(NAN_BM_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line", \
               sizeof(NAN_BM_ASSERT_PREFIX #expr " is FALSE in " __FILE__ " line")-1, \
               __LINE__, RV_TRACE_LEVEL_ERROR, NAN_USE_ID); fail_code;}} while(0)

*/

// Uncomment to enable the traces
//#define NAN_BM_ENABLE_TRACE 

#ifdef NAN_BM_ENABLE_TRACE
#define NAN_BM_SEND_TRACE(string, trace_level) \
 rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, NAN_USE_ID)

#define NAN_BM_SEND_TRACE_PARAM(string, param, trace_level) \
rvf_send_trace (string, (sizeof(string) - 1), param, trace_level, NAN_USE_ID)
#else
#define NAN_BM_SEND_TRACE(string, trace_level)
 
#define NAN_BM_SEND_TRACE_PARAM(string, param, trace_level)
#endif 
 
// Uncomment to disable api traces
// #define NAN_BM_ENABLE_API_TRACE  

#ifdef NAN_BM_ENABLE_API_TRACE
#define NAN_BM_API_TRACE(str) NAN_BM_SEND_TRACE((str), RV_TRACE_LEVEL_DEBUG_HIGH)
#else
#define NAN_BM_API_TRACE(str)   /* nothing */
#endif

/* to be enabled when running unit tests */
#define NAND_BM_ENABLE_UTS

/* client spare area size */
#define NAN_BM_CLIENT_SPARE_SIZE    6


#define PRF_NAN_BM 1

// for change in NAND configuration
#define NAN_32_TO_64


/**
 * wether or not to use polling instead of the interrupts. Note that polling is used
 * for nan_bm_handle_read_req() and nan_bm_handle_program_req() functions only
 */
#define NAN_BM_POLLING

/* do not verify (read) after program */
#define NAN_BMD_NO_VERIFY

/* only support one NF */
#define NAN_BM_MAX_CHIP_SELECT       (0)

/**
 * DMA channel for NF and parallel port reads
 */
#define NAN_BM_DMA_CHANNEL 4

/**
 * use DMG (or DMA)
 */
#undef NAN_BM_USE_DMG

/**
 * data limit for DMA autoselect
 */
#define NAN_BM_DMA_DATA_SIZE_THRESHOLD (3*128)

#if 0
/**
 * wether or not to use 32-bits FIFO access or not
 */
#define NAN_BM_32_BITS_FIFO


#endif

/**
 * wether or not NAN_BM should use DMA
 */
// #define NAN_BM_ENABLE_DMA


typedef enum
{ NAN_BM_RW_MODE0, NAN_BM_RW_MODE1, NAN_BM_RW_MODE2 }
T_NAN_BM_RW_MODE;


typedef UINT32 T_NAN_BM_ECC;

/* register addresses */
#ifndef MEM_NAN_BM
#define MEM_NAN_BM 0x9D00000U
#endif
#define NAN_BM_COMMAND_REG (MEM_NAN_BM)
#define NAN_BM_CONTROL_REG (MEM_NAN_BM + 0x04)
#define NAN_BM_STATUS_IT_REG (MEM_NAN_BM + 0x08)
#define NAN_BM_STATUS_STATE_REG (MEM_NAN_BM + 0x0c)
#define NAN_BM_BLOCK_SIZE_REG (MEM_NAN_BM + 0x10)
#define NAN_BM_START_ADDRESS_REG (MEM_NAN_BM + 0x14)
#define NAN_BM_ECC_SIZE_REG (MEM_NAN_BM + 0x18)
#define NAN_BM_ECC_VALUE_REG (MEM_NAN_BM + 0x1c)


/* all NAN_BMD registers are accessed in 32 bits */
#define NAN_BM_REG(NAN_BM_XXX_REG) (*((volatile UINT32 *)(NAN_BM_XXX_REG)))

/* page arrays */
#define NAN_BM_PAGE_BUFFER (0x9d80000U)
#define NAN_BM_PAGE_SIZE (128)
#define NAN_BM_PAGE ((volatile UINT8 *)(NAN_BM_PAGE_BUFFER))
#define NAN_BM_PAGE_32 ((volatile UINT32 *)(NAN_BM_PAGE_BUFFER))

/* message ID's for internal messages (from HISR) */
#define nan_bm_enable_IT()  F_INTH_ENABLE_ONE_IT(C_INTH_NAND_FLASH_IT)
#define nan_bm_disable_IT()  F_INTH_DISABLE_ONE_IT(C_INTH_NAND_FLASH_IT)
#define NAN_BM_RETURN_QUEUE_EVENT (1u<<14)

/* NF status bits for nan_bm_flash_read_status() */
#define NAN_BM_FSTAT_PRG_ERASE (1)
#define NAN_BM_FSTAT_DEVICE_OPERATION (1<<6)
#define NAN_BM_FSTAT_WRITE_PROTECT (1<<7)


// Copy of GBI 
typedef enum {
  NAN_BMD_OK                    = 0,
  NAN_BMD_NOT_SUPPORTED         = -2,
  NAN_BMD_NOT_READY             = -3,
  NAN_BMD_MEMORY_WARNING        = -4,
  NAN_BMD_MEMORY_ERR            = -5,
  NAN_BMD_MEMORY_REMAINING      = -6,
  NAN_BMD_INTERNAL_ERR          = -9,
  NAN_BMD_INVALID_PARAMETER     = -10,
  NAN_BMD_INVALID_BLOCK_ERR     = -11,
  NAN_BMD_PAGE_NOT_EMPTY_ERR    = -12,
  NAN_BMD_ECC_ERROR             = -13,
  NAN_BMD_ALLOCATION_ERROR       = -14,
  // code review comment 81
  NAN_BMD_DEVICE_PROTECT_ERROR 	 = -15

} T_NAN_BMD_RET;


/**
 * The Control Block buffer of NAN_BM, which gathers all 'Global variables'
 * used by NAN_BM instance.
 *
 * A structure should gathers all the 'global variables' of NAN_BM instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_NAN_BM_ENV_CTRL_BLK buffer is allocated when creating NAN_BM instance and is 
 * then always refered by NAN_BM instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
  BOOL initialised;

    /** Pointer to the error function */
    T_RVM_RETURN (*error_ft) (T_RVM_NAME swe_name,
                T_RVM_RETURN error_cause,
                T_RVM_ERROR_TYPE error_type,
                T_RVM_STRING error_msg);
    /** Mem bank id. */
  T_RVF_MB_ID prim_mb_id;
  T_RVF_ADDR_ID addr_id;

    /** HISR */
#ifndef _WINDOWS
  NU_HISR nan_bm_hisr;
#endif
 #ifndef HISR_STACK_SHARING
  char hisr_stack[NAN_BM_HISR_STACK_SIZE];
 #endif
  T_NAN_BM_MSG *saved_req_msg_p;
  T_NAN_BM_DMA_MODE dma_mode;
  T_NAN_BM_ECC *ecc_values_p;
  UINT32 *ecc_sizes_p;
  UINT32 ecc_count;
  UINT32 ecc_max;
  struct
  {
  T_RVF_G_ADDR_ID addr_id;
  UINT16 event;
  T_RVF_BUFFER_Q queue_obj;
  }return_queue;
  T_RV_RETURN_PATH path_to_return_queue;
 

#ifdef _WINDOWS
  /* NAND-driver PC simulation software */
  UINT8* data_main_p;
  UINT8* data_spare_p;
#endif
}
T_NAN_BM_ENV_CTRL_BLK;

extern T_RV_RET nan_bm_use_callback(void);
extern BOOL nan_bm_busy(void);
extern BOOL nan_bm_msg_pending(void);
extern void nan_bm_send_msg(T_NAN_BMD_RET retval, T_NAN_BMD_RET (*callback_func)(T_NAN_BMD_RET));

/** External ref "global variables" structure. */
extern T_NAN_BM_ENV_CTRL_BLK *nan_bm_env_ctrl_blk_p;
extern T_NAN_BMD_RET nan_bm_send_response_to_client (UINT32 msg_id,
                         T_NAN_BM_MSG * msg_p);
extern void nan_bm_IT_handler (void);
#ifndef NAN_BM_POLLING
extern void nan_bm_hisr (void);
#endif 
extern void nan_bm_controller_init (void);
#define nan_bm_get_status() NAN_BM_REG (NAN_BM_STATUS_STATE_REG);
extern void *nan_bm_return_queue_get_msg (void);
extern void nan_bm_return_queue_store_msg (void *msg_p);
extern void nan_bm_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id,
                   UINT16 rq_event,
                   T_RV_RETURN_PATH * path_to_return_queue_p);
extern T_RVF_RET nan_bm_queue_request (T_NAN_BM_MSG * msg_p);
#if(CHIPSET != 15)
#ifdef   CAM_TRISTATE_ENABLE
void nan_bm_camera_tristate_on(void);
void nan_bm_camera_tristate_off(void);
#endif
#endif

#endif /* __NAN_BM_INST_I_H_ */
