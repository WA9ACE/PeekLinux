/**
 * @file	dma_i.h
 *
 * Internal definitions for DMA.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/2/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __DMA_INST_I_H_
#define __DMA_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "dma/dma_cfg.h"
#include "dma/dma_api.h"

#include "dma/dma_state_i.h"

#ifndef _WINDOWS
#include "nucleus.h"
#include "inth/iq.h"
#include "dma/sys_dma.h"
#include "chipset.cfg"
#else
#include "dma/pc/dma_pc_i.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* Size of the HISR stack associated to the DMA interrupt */
#ifndef HISR_STACK_SHARING
#define DMA_HISR_STACK_SIZE       (768)// (512) //OMAPS00147875 
#define DMA_SECURE_HISR_STACK_SIZE (512)
#endif

#define DMA_ALL_CHAN_FREE     (0)
#define DMA_PARAMS_OK         (99)
#define DMA_ONE_BIT_SHIFT     (1)

#define DMA_BITMASK_BIT0      (0x1)
#define DMA_BITMASK_BIT1      (0x2)
#define DMA_BITMASK_BIT2      (0x4)
#define DMA_BITMASK_BIT3      (0x8)
#define DMA_BITMASK_BIT4      (0x10)
#define DMA_BITMASK_BIT5      (0x20)

#define DMA_TRUE              (1)
#define DMA_ZERO              (0)

#define DMA_ONE_BYTE          (1)
#define DMA_TWO_BYTES         (2)
#define DMA_FOUR_BYTES        (4)

#define DMA_SOURCE            (0)
#define DMA_DESTINATION       (1)

#define DMA_BUFFER_ONE        (0)
#define DMA_BUFFER_TWO        (1)

#define DMA_INTERNAL          (0)
#define DMA_EXTERNAL          (1)

#define DMA_NO_DOUBLE_BUF     (0)

/*
 * client wants to use double buffering on the source
 */
#define DMA_SOURCE_DOUBLE_BUF (1)
 
/*
 * destination is double buffered. both addresses 
 * should be SRAM and are provided by the client)  
 */
#define DMA_DEST_DOUBLE_BUF   (2)


/*
 * Source lays in XRAM. Internally a double buffer is used
 */
#define DMA_SOURCE_XRAM       (3)

/*
 * Destination lays in XRAM. Internally a double buffer is used
 */
#define DMA_DEST_XRAM         (4)


/************************************************/



#ifndef _WINDOWS
#define DMA_MAX_NUMBER_OF_CHANNEL C_DMA_NUMBER_OF_CHANNEL
#else
#define DMA_MAX_NUMBER_OF_CHANNEL (6)
#endif


#define CHANNEL0	(0)
#define CHANNEL1	(1)
#define CHANNEL2	(2)
#define CHANNEL3	(3)
#define CHANNEL4	(4)
#define CHANNEL5	(5)

/***********************************/
/* Address mapping internal memory */
/***********************************/
#if (CHIPSET == 15)        
#define DMA_EMIF_MIN (0x00400000)
#define DMA_EMIF_MAX (0x17FFFFFF)
	
#endif

#if (CHIPSET == 15)        
/* Source: CAPLUS005 v.2.5 page 13 */
#define DMA_IMIF_MIN  (0x08000000)
#define DMA_IMIF_MAX  (0x08FFFFFF)

#define DMA_IPER_MIN (0x09000000)
#define DMA_IPER_MAX (0x0FFFFFFF)
#else
#define DMA_IMIF_MIN  (0x08000000)
#define DMA_IMIF_MAX  (0x0FFFFFFF)
#endif
/* Address mapping API RAM */
#define DMA_API_MIN1  (0xFFD00000)
#define DMA_API_MAX1  (0xFFD03FFF)

/* Address mapping API CTRL */
#define DMA_API_MIN2  (0xFFE00000)
#define DMA_API_MAX2  (0xFFE00001)

/* Address mapping RHEA (strobe 0) */
#define DMA_RHEA_MIN1 (0xFFFE0000)
#define DMA_RHEA_MAX1 (0xFFFEFFFF)

/* Address mapping RHEA (strobe 1) */
#define DMA_RHEA_MIN2 (0xFFFF0000)
#define DMA_RHEA_MAX2 (0xFFFFFFFF)

#ifndef _WINDOWS
#define		DMA_IMIF_PORT			C_DMA_IMIF_PORT	/*  IMIF PORT definition */
#define		DMA_RHEA_PORT			C_DMA_RHEA_PORT	/*  RHEA PORT definition */
#define		DMA_API_PORT			C_DMA_API_PORT	/*  API PORT definition  */

#if (CHIPSET == 15 )        
#define 	DMA_IPER_PORT           C_DMA_IPHERIF_PORT /* IPER PORT definition */
#define		DMA_EMIF_PORT			C_DMA_EMIF_PORT	/*  EMIF PORT definition  */
#endif

#else
#define		DMA_IMIF_PORT			0 /*  IMIF PORT definition */
#define		DMA_RHEA_PORT			1	/*  RHEA PORT definition */
#define		DMA_API_PORT			2 /*  API PORT definition  */      
#if (CHIPSET == 15 || CHIPSET == 15)
#define 		DMA_IPER_PORT               3 /* IPER PORT definition */
#define		DMA_EMIF_PORT			4 /*  EMIF PORT definition  */
#endif
#endif // _WINDOWS
#define		DMA_PORT_ERR	    9 /*  Error in API PORT definition  */


/* 
  This array will store for each channel if it is free or not. If 
  the channel is free, it will store the channel number and return path
  of the calling function and if the client wants the channel to be 
  specific or not.
*/ 
typedef struct 
{
  T_DMA_SPECIFIC specific;
  T_DMA_CHANNEL  channel;

  T_DMA_TYPE_CHANNEL_PARAMETER channel_info;
  UINT32      bytes_to_be_transferred;
  UINT32      buffer_size;
  UINT8       double_buf_mode;
  BOOL        int_or_ext;
  BOOL        dma_end_notification_bool;
  BOOL        dma_restart;
  T_RV_RETURN return_path;  
} T_DMA_CHANNEL_ARRAY;

/*
  This array does store the same data as the T_DMA_CHANNEL_ARRAY, but then
  for the queued channels. Besides that it does also store the queue index
  which is used to remove a reservation from the queue
*/
typedef struct 
{
  T_DMA_SPECIFIC    specific;
  T_DMA_CHANNEL     channel;
  T_RV_RETURN       return_path;
  T_DMA_CHANNEL     queue_index;
  T_DMA_SW_PRIORITY sw_priority;
} T_DMA_QUEUE_ARRAY;

/** Message ID. */

/* 
  These two defines are used in the array's dma_queue_array and 
  dma_channel_array. When the specific parameter of one of this 
  array has the value DMA_QUEUE_POS_FREE (or DMA_CHANNEL_POS_FREE),
  then this channel of queue place is free
*/

#define DMA_QUEUE_POS_FREE   (0xFF)
#define DMA_CHANNEL_POS_FREE (0xFF)

/*
  This is a sequential number the give a client back an queue index.
  When a queue index is given away, the queueindex counter is increased 
  by 1. When the queue index counter does reach its maximum, it will
  go back to its minimum value.
*/
#define DMA_QUEUE_INDEX_MIN       (40)
#define DMA_QUEUE_INDEX_MAX       (240)
#define DMA_QUEUE_INDEX_STEPSIZE  (8)
/* 
  These defines are used to determine if the api functions are 
  called in the corect dequence
*/
#define DMA_FUNCTION_NONE             (0x0)
#define DMA_FUNCTION_RESERVED         (0x1)
#define DMA_FUNCTION_PARAMS_SET_UP    (0x2)
#define DMA_FUNCTION_CHANNEL_ENABLED  (0x3)


#ifdef _DMA_TRACING_ON
/** Macro used for tracing DMA messages. */
#define DMA_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, DMA_USE_ID)

/** Macro used for tracing DMA messages with a parameter. */
#define DMA_SEND_TRACE_PARAM(text,param,level) \
  rvf_send_trace(text, sizeof(text)-1, param, level, DMA_USE_ID )
#else
#define DMA_SEND_TRACE(string, trace_level) 
#define DMA_SEND_TRACE_PARAM(text,param,level) 
#endif

#define DMA_HISR_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, DMA_USE_ID)
#define DMA_HISR_SEND_TRACE_PARAM(text,param,level) \
  rvf_send_trace(text, sizeof(text)-1, param, level, DMA_USE_ID )

/**
 * The Control Block buffer of DMA, which gathers all 'Global variables'
 * used by DMA instance.
 *
 * A structure should gathers all the 'global variables' of DMA instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_DMA_ENV_CTRL_BLK buffer is allocated when creating DMA instance and is 
 * then always refered by DMA instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
	/** Store the current state of the DMA instance */

	/** Pointer to the error function */
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);
	/** Mem bank id. */
	T_RVF_MB_ID prim_mb_id;

	T_RVF_ADDR_ID	addr_id;
#ifndef _WINDOWS
  /** HISR */
  NU_HISR dma_hisr;
  /** Secure HISR */
  NU_HISR dma_secure_hisr;

#endif
#ifndef HISR_STACK_SHARING
  char dma_hisr_stack        [DMA_HISR_STACK_SIZE];
  char dma_secure_hisr_stack [DMA_SECURE_HISR_STACK_SIZE];
#endif
  UINT8 dma_queue_index;

} T_DMA_ENV_CTRL_BLK;


/* 
  External decalarations for hisr and secure hisr 
*/
extern void dma_hisr        (void);
extern void dma_secure_hisr (void);

#ifndef _WINDOWS
typedef T_DMA_TYPE_CHANNEL_PORT T_DMA_INTERNAL_TYPE_CHANNEL_PORT; 
#else
typedef UINT8 T_DMA_INTERNAL_TYPE_CHANNEL_PORT;
#endif // _WINDOWS


/* prtotype declaration */
void dma_send_status_message(T_DMA_CHANNEL channel,
                             T_DMA_RET status,
                             T_RV_RETURN return_path);

T_DMA_RET dma_channel_check      (T_DMA_CHANNEL channel);
T_DMA_RET dma_check_channel_info (T_DMA_CHANNEL_PARAMETERS *channel_info);
T_DMA_RET dma_queue_spec_prio_check (T_DMA_QUEUE queue, T_DMA_QUEUE specific,
                                     T_DMA_SW_PRIORITY sw_priority);

void dma_internal_reserve_chan      (T_RV_HDR *msg_p);
void dma_internal_remove_from_queue (T_RV_HDR *msg_p);
void dma_internal_set_chan_params   (T_RV_HDR *msg_p);
void dma_internal_rel_chan          (T_DMA_CHANNEL channel);

T_DMA_INTERNAL_TYPE_CHANNEL_PORT dma_determine_port (UINT32 address);
void write_hw_dest_adress (UINT16 dma_channel_number, UINT32 address);
void write_hw_source_adress (UINT16 dma_channel_number, UINT32 address);

#ifndef _WINDOWS

void dma_call_back_it_chan0(SYS_UWORD16 dma_status);
void dma_call_back_it_chan1(SYS_UWORD16 dma_status);
void dma_call_back_it_chan2(SYS_UWORD16 dma_status);
void dma_call_back_it_chan3(SYS_UWORD16 dma_status);
void dma_call_back_it_chan4(SYS_UWORD16 dma_status);
void dma_call_back_it_chan5(SYS_UWORD16 dma_status);

#endif



#ifdef __cplusplus
}
#endif


#endif /* __DMA_INST_I_H_ */
