/**
 * @file  nan_bm_hw_functions.c
 *
 * NAN_BM hw functions, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author  J.A. Renia
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/27/2006 J.A. Renia ()   Create.
 *
 * (C) Copyright 2006 by TI, All Rights Reserved
 */

#include <string.h>
#include "nan/nan_i.h"
#ifdef NAN_BM_ENABLE_DMA
#include "dmg/dmg_api.h"
#include "dmg/dmg_message.h"
#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "i2c/i2c_api.h"
#endif
#include "sys_conf.h"
#include "armio.h"
#include "nucleus.h"
#include "swconfig.cfg"

#include "nan_hw_functions.h"


#define USE_MEMCPY   TRUE



/* nan_bm ctrl block */
T_NAN_BM_ENV_CTRL_BLK *nan_bm_env_ctrl_blk_p = NULL;


#define nan_bm_set_chipselect_command_bit() NAN_BM_REG(NAN_BM_COMMAND_REG) |= 1<<1;  	  // Set the ChipSelect bit
#define nan_bm_clear_chipselect_command_bit() NAN_BM_REG(NAN_BM_COMMAND_REG) &= ~(1<<1);    // Reset the ChipSelect bit

#define nan_bm_set_first_command_bytes(n) NAN_BM_REG(NAN_BM_COMMAND_REG) = (NAN_BM_REG(NAN_BM_COMMAND_REG) & (~(COMMAND_BYTES_MASK<<6))) | (n)<<6 // First Command bytes 6:7
#define nan_bm_set_address_bytes(n) NAN_BM_REG(NAN_BM_COMMAND_REG) = (NAN_BM_REG(NAN_BM_COMMAND_REG) & (~(ADDRESS_BYTES_MASK<<8))) | (n)<<8 // address bytes 8:10
#define nan_bm_set_second_command_bytes(n) NAN_BM_REG(NAN_BM_COMMAND_REG) = (NAN_BM_REG(NAN_BM_COMMAND_REG) & (~(COMMAND_BYTES_MASK<<11))) | (n)<<11 // second command bytes 11:12
#define nan_bm_set_data_bytes(n) NAN_BM_REG(NAN_BM_COMMAND_REG) = (NAN_BM_REG(NAN_BM_COMMAND_REG) & (~(DATA_BYTES_MASK<<13))) | (n)<<13 // data bytes 13:29
#define nan_bm_set_third_command_bytes(n) NAN_BM_REG(NAN_BM_COMMAND_REG) = (NAN_BM_REG(NAN_BM_COMMAND_REG) & (~(COMMAND_BYTES_MASK<<30))) | (n)<<30 // third command bytes 30:31
#define nan_bm_enable_transfer() NAN_BM_REG(NAN_BM_COMMAND_REG) |= 1 // transfer enable 0th byte
#define nan_bm_set_write_mode() NAN_BM_REG(NAN_BM_COMMAND_REG) &= ~(1<<5) // RnW enable 5th byte 0 -> write
#define nan_bm_set_read_mode() NAN_BM_REG(NAN_BM_COMMAND_REG) |= 1<<5 // read enable 5th byte 1 -> read
#define nan_bm_set_clk_div(n) NAN_BM_REG(NAN_BM_CONTROL_REG) = (NAN_BM_REG(NAN_BM_CONTROL_REG) & (~(CLK_MASK<<6))) | (n)<<6 // clock divisor value 6th byte
#define nan_bm_set_dummy_cycle(n) NAN_BM_REG(NAN_BM_CONTROL_REG) = (NAN_BM_REG(NAN_BM_CONTROL_REG) & (~(CLK_MASK<<10))) | (n)<<10 // dummy cycles 10:13
#define nan_bm_software_reset() NAN_BM_REG(NAN_BM_CONTROL_REG) = 1  //software reset 1st byte

/* only the lower 16 bits are valid, the remaining bits are reserved */
#define nan_bm_set_block_size(n) NAN_BM_REG(NAN_BM_BLOCK_SIZE_REG) = (n & 0xFFFF) // block size 0:15
#define nan_bm_set_start_address(n) NAN_BM_REG(NAN_BM_START_ADDRESS_REG) = (n & 0xFFFF) // start address 0:15
#define nan_bm_set_ecc_size(n) NAN_BM_REG(NAN_BM_ECC_SIZE_REG) = (n & 0xFFFF) //ecc size 0:15

#define nan_bm_enable_dma() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<3 // dma enable 3rd byte
#define nan_bm_disable_dma() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<3) // dma enable 3rd byte


#define nan_bm_enable_ecc() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<14  //ecc enable 14th byte 1-> ecc is enabled
#define nan_bm_disable_ecc() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<14) //14th byte 0 -> ecc is disabled
#define nan_bm_enable_end_ecc_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<15 // interrupt end ecc 15th byte
#define nan_bm_disable_end_ecc_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<15) // interrupt end ecc 15th byte
#define nan_bm_enable_end_transfer_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<2 // interrupt end transfer 2nd byte
#define nan_bm_disable_end_transfer_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<2) // interrupt end transfer 2nd byte
#define nan_bm_enable_end_page_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<1 // interrupt end page 1st byte
#define nan_bm_disable_end_page_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<1) // interrupt end page 1st byte
#define nan_bm_enable_end_busy_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<16 // interrupt end busy 16th byte
#define nan_bm_disable_end_busy_IT() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<16) // interrupt end busy 16th byte


#define nan_bm_page_ready() NAN_BM_REG(NAN_BM_CONTROL_REG) |= 1<<5 // page ready 5th byte
#define nan_bm_page_not_ready() NAN_BM_REG(NAN_BM_CONTROL_REG) &= ~(1U<<5) // page ready 5th byte
#define nan_bm_clear_pending_ITs() do { volatile UINT32 dummy = NAN_BM_REG (NAN_BM_STATUS_IT_REG); } while (0)
#define nan_bm_clear_ecc_reg() do { volatile UINT32 dummy = NAN_BM_REG (NAN_BM_ECC_VALUE_REG); } while (0)


#define nan_bm_area(column) (column < NAN_BM_FLASH_HALF_PAGE_OFFSET ? COL_OFFSET_0 :  \
                         (column < NAN_BM_FLASH_SPARE_OFFSET ? COL_OFFSET_1 : COL_OFFSET_2))

#define nan_bm_wait_event(expected_event, saved_event) \
        nan_bm_wait_event_to ((expected_event),(saved_event), NAN_BM_TIMEOUT)


#define NAN_BM_TIMEOUT (2)  
#define NAN_BM_IT_MASK_END_PAGE (1u)
#define NAN_BM_IT_MASK_END_TRANSFER (1u << 1)
#define NAN_BM_IT_MASK_END_ECC (1u << 2)
#define NAN_BM_IT_MASK_END_BUSY (1u << 3)
#define NAN_BM_END_PAGE_EVENT (1U<<8)
#define NAN_BM_END_TRANSFER_EVENT (1U<<9)
#define NAN_BM_END_ECC_EVENT (1U<<10)
#define NAN_BM_END_BUSY_EVENT (1U<<11)
#define NAN_BM_PAR_IF_END_PAGE_EVENT (1U<<12)
#define NAN_BM_PAR_IF_END_TRANSFER_EVENT (1U<<13)
#define NAN_BM_ALL_EVENTS ((UINT16)0x3f00u) 

/* these traces are added for debug purposes */

#define NAN_BM_TRACE_EVENT(string, level) NAN_BM_SEND_TRACE(string, level)
#define NAN_BM_TRACE_EVENT_PARAM(string, param, level) NAN_BM_SEND_TRACE_PARAM(string, param, level)

#ifndef NAN_BM_POLLING
static void nan_bm_send_hisr_event_to_nan_bm (UINT16 event_id);
#endif

static UINT32 bm_interrupt_status;
static UINT32 bm_end_page_IT_count = 0;
static UINT32 bm_end_page_IT_threshold = 0;

#ifdef NAN_BM_POLLING
UINT32 bm_poll_status=0;
#endif //#ifdef NAN_BM_POLLING


#if (CHIPSET == 15)

static BOOL nan_mutex_initialized = FALSE;
static T_RVF_MUTEX nan_pin_mutex;

void
initialize_nand_mutex()
{
	T_RVF_RET ret_val;
	if(NULL == nan_bm_env_ctrl_blk_p){
		NAN_BM_TRACE_EVENT ("The NAN Env Ctrl Block not Inited",
						   RV_TRACE_LEVEL_ERROR);
		return;
	}
	if(!nan_mutex_initialized){

 		ret_val = rvf_initialize_static_mutex(&nan_pin_mutex);
		if(RVF_OK == ret_val)
		{
			nan_mutex_initialized = TRUE;
		}
		else
		{
			 
			NAN_BM_TRACE_EVENT ("Could not initialize the mutex",
						   RV_TRACE_LEVEL_ERROR);
			return;
		}
	}
	return;
}

void
delete_nand_mutex()
{
	if(is_nand_mutex_initialized())
		rvf_delete_mutex(&nan_pin_mutex);
	nan_mutex_initialized = FALSE;
	return;
}

BOOL
is_nand_mutex_initialized()
{
	return nan_mutex_initialized;
}

void
conf_acquire_pins(T_MUX_DEVS dev_id)
{
#if (WCP_PROF == 1)
	prf_LogFunctionEntry((unsigned long) conf_acquire_pins);
#endif
	if(is_nand_mutex_initialized())
       	rvf_lock_mutex(&nan_pin_mutex);
	
	switch(dev_id){
		case MUX_NANDFLASH:
			//Clear and set the last two bits
			C_CONF_LCD_CAM_NAN_REG &= ~0x3;
			C_CONF_LCD_CAM_NAN_REG|=0x02;

			// Enable the Chip Select for NAND
			nan_bm_clear_chipselect_command_bit();
			break;
		case MUX_LCD:
			C_CONF_LCD_CAM_NAN_REG|=0x03;
			break;
		default:
			/* 'un'lock the mutex here */ 
			NAN_BM_TRACE_EVENT ("Trying to capture muxed pins for UnKnown device",
						   RV_TRACE_LEVEL_ERROR);
			if(is_nand_mutex_initialized())
				rvf_unlock_mutex(&nan_pin_mutex);
			break;
	}
#if (WCP_PROF == 1)
	prf_LogFunctionExit((unsigned long) conf_acquire_pins);
#endif
	return;
}

void
conf_release_pins(T_MUX_DEVS dev_id)
{
#if (WCP_PROF == 1)
	prf_LogFunctionEntry((unsigned long) conf_release_pins);
#endif
	if(is_nand_mutex_initialized())
		{
		if(dev_id == MUX_NANDFLASH)
			{
			// Disable the Chip Select for NAND
			nan_bm_set_chipselect_command_bit();
			}
       	rvf_unlock_mutex(&nan_pin_mutex);
		}

#if (WCP_PROF == 1)
	prf_LogFunctionExit((unsigned long) conf_release_pins);
#endif
	return;
}

#endif //#if (CHIPSET == 15)

/////////////////////////////////////////////////////////////////////////////////////////////
///Static functions
/////////////////////////////////////////////////////////////////////////////////////////////

/**
  See nan_bm_ecc_params_ok
*/
static BOOL nan_bm_ecc_params_ok (UINT32 *ecc_sizes_p, 
                                  T_NAN_BM_ECC *ecc_values_p,
                                  UINT32 ecc_count,
                                  UINT32 data_size)
{
  UINT32 ecc_sum = 0;
  UINT32 i;

  if (ecc_sizes_p  == NULL || ecc_values_p == NULL)
  {
    return FALSE;
  }
  
  /* check sum of ECC sizes */
  for (i = 0; i < ecc_count; i++)
  {
    UINT32 size = ecc_sizes_p[i];
    if (size == 0)
    {
      return FALSE;
    }
    ecc_sum += size;
  }
  return ecc_sum == data_size;
}


/* The following function is only used in polling mode */
#ifdef NAN_BMD_ECC_ENABLED
#ifdef NAN_BM_POLLING
/* change the void function to return error condtions */
static T_RVM_RETURN nan_bm_store_ecc (void)
{
  /* expecting any ECC values? */
  if (nan_bm_env_ctrl_blk_p->ecc_count < nan_bm_env_ctrl_blk_p->ecc_max)
  {
    /* any ECC sizes left? */
    if (nan_bm_env_ctrl_blk_p->ecc_count + 1 < nan_bm_env_ctrl_blk_p->ecc_max)
    {
      /* select next ECC size */
      nan_bm_set_ecc_size (nan_bm_env_ctrl_blk_p->
                           ecc_sizes_p[nan_bm_env_ctrl_blk_p->ecc_count + 1]);
    }
    /* store current ECC value */
    nan_bm_env_ctrl_blk_p->ecc_values_p[nan_bm_env_ctrl_blk_p->ecc_count++] =
    (T_NAN_BM_ECC) NAN_BM_REG (NAN_BM_ECC_VALUE_REG);


    if (nan_bm_env_ctrl_blk_p->ecc_count == nan_bm_env_ctrl_blk_p->ecc_max)
    {
      /* no more ECC sizes, inform NAN_BM task that all ECC values are ready */
       NAN_BM_TRACE_EVENT ("NAN_BM end ECC event", RV_TRACE_LEVEL_ERROR);
      bm_poll_status|=NAN_BM_END_ECC_EVENT;
    }
  }
  else
  {
  	NAN_BM_SEND_TRACE ("nan_bm ecc retrieval error", RV_TRACE_LEVEL_ERROR);
       return RVM_INTERNAL_ERR;
  }
  return RV_OK; 
}
#endif //#ifdef NAN_POLLING
#endif //#ifdef NAN_BMD_ECC_ENABLED

#ifdef NAN_BM_POLLING
static T_RVM_RETURN nan_bm_poll_event (UINT32 event)
{
  int i,j;
  UINT32 status;
  for(i=0; i < 100000; i++)
  {
    status=NAN_BM_REG (NAN_BM_STATUS_IT_REG);
    bm_poll_status|=status;
#ifdef NAN_BMD_ECC_ENABLED
    if (bm_poll_status & NAN_BM_IT_MASK_END_ECC)
    {
      nan_bm_store_ecc();
      bm_poll_status&=~NAN_BM_IT_MASK_END_ECC; //reset the ECC interrupt immediately
    }
#endif //#ifdef NAN_BMD_ECC_ENABLED

    if((bm_poll_status & event) != 0)
    {
      bm_poll_status&=~event; //reset the events
      return RV_OK;
    }
  }
  return RV_INTERNAL_ERR;
}
#endif //#ifdef NAN_POLLING


void nan_bm_controller_init (void)
{

#if (CHIPSET == 15)
	conf_acquire_pins(MUX_NANDFLASH);
#endif
  nan_bm_software_reset ();
/*  waiting for the register to be cleared */
while((NAN_BM_REG(NAN_BM_CONTROL_REG) & 0x01) == 1);

  nan_bm_set_clk_div (NAN_BM_CLK_DIV);
  nan_bm_set_dummy_cycle (NAN_BM_DUMMY_CYCLE);

  /* init command register */
  NAN_BM_REG (NAN_BM_COMMAND_REG) = 0 << 1	/* enable NF1 */
	| 1 << 2					/* disable NF2 */
	| 1 << 3					/* disable write protect NF memory 1 */
	| 0 << 4;					/* enable write protect NF memory 2 */

  /* don't use page switches triggered by block size */

  nan_bm_set_block_size (NAN_BM_PAGE_SIZE);
  nan_bm_set_start_address (0);
  #if (CHIPSET != 15)
  {
	const UINT32 DBG_CLK2 = 0xfffef03eu;
	/* enable parallel port CLKEN and CLKIN */
	*(volatile UINT8 *) DBG_CLK2 |= 1 << 7 | 1 << 6;
  }
  #endif
  
  #if (CHIPSET == 15)
  	conf_release_pins(MUX_NANDFLASH);
  #endif
}


/**
  See nan_bm_dma_wait_status_OK
*/
/* The following function is only used in interrupt mode */
#ifndef NAN_BM_POLLING

#ifdef NAN_BM_ENABLE_DMA
static BOOL nan_bm_dma_wait_status_OK (T_DMA_CHANNEL *channel)
{
  T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg = nan_bm_return_queue_get_msg ();
  BOOL ret_val = TRUE;

  switch (dma_status_rsp_msg->result.status)
  {
  case DMA_RESERVE_OK:
    if (channel)
      *channel=dma_status_rsp_msg->result.channel;
    /* fall through */
  case DMA_OK:
  case DMA_PARAM_SET:
  case DMA_COMPLETED:
  case DMA_CHANNEL_RELEASED:
  case DMA_CHANNEL_ENABLED:
    /* status OK: do nothing */
    break;
  default:
    NAN_BM_SEND_TRACE_PARAM ("NAN_BM DMA error status received: ",
              dma_status_rsp_msg->result.status,
              RV_TRACE_LEVEL_ERROR);
    ret_val = FALSE;
  }             /* end switch */

  if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
  {
    NAN_BM_SEND_TRACE ("NAN_BM DMA could not free DMA status msg",
            RV_TRACE_LEVEL_ERROR);
  }
  return ret_val;
}
#endif //#ifdef NAN_BM_ENABLE_DMA
#endif //#ifndef NAN_BM_POLLING

/**
  See nan_bm_handle_dma_req
*/
T_NAN_BMD_RET nan_bm_handle_dma_req (T_NAN_BM_DMA_MODE dma_mode)
{
  switch (dma_mode)
  {
  case NAN_BM_FORCE_CPU:
    nan_bm_env_ctrl_blk_p->dma_mode = NAN_BM_FORCE_CPU;
    break;
#ifdef NAN_BM_ENABLE_DMA
  case NAN_BM_FORCE_DMA:
    nan_bm_env_ctrl_blk_p->dma_mode = NAN_BM_FORCE_DMA;
    break;
  case NAN_BM_DMA_AUTO:
    nan_bm_env_ctrl_blk_p->dma_mode = NAN_BM_DMA_AUTO;
    break;
#endif //#ifdef NAN_BM_ENABLE_DMA
  default:
    return NAN_BMD_INVALID_PARAMETER;
  }
  return NAN_BMD_OK;
}



/*  See nan_bm_IT_handler */

/* The following function is only used in interrupt mode */
void nan_bm_IT_handler (void)
{
  nan_bm_disable_IT ();
  bm_interrupt_status = NAN_BM_REG (NAN_BM_STATUS_IT_REG);

  if (bm_interrupt_status & NAN_BM_IT_MASK_END_PAGE)
  {
    bm_end_page_IT_count++;
  }

  /* drop first end_page ITs events below threshold (triggered by DMA) */
  if (bm_end_page_IT_count <= bm_end_page_IT_threshold)
  {
    bm_interrupt_status &= ~NAN_BM_IT_MASK_END_PAGE;
  }

  /* 
   * IF NAN_BM task not ready OR all ITs dropped
   *    ignore this interrupt
   * ELSE 
   *    trigger HISR 
   * */

  if (nan_bm_env_ctrl_blk_p == NULL || nan_bm_env_ctrl_blk_p->initialised == FALSE
    || bm_interrupt_status == 0)
  {
    nan_bm_enable_IT ();
    return;
  }
  NU_Activate_HISR (&nan_bm_env_ctrl_blk_p->nan_bm_hisr);
}


/* The following function is only used in interrupt mode */
#ifndef NAN_BM_POLLING
void nan_bm_hisr (void)
{
 NAN_BM_TRACE_EVENT_PARAM ("NAN_BM HISR IT reg =", (UINT32) bm_interrupt_status,
             RV_TRACE_LEVEL_ERROR);
  /* ------------- handle end page ITs ------------- */
  if (bm_interrupt_status & NAN_BM_IT_MASK_END_PAGE)
  {
    if (bm_end_page_IT_count <= bm_end_page_IT_threshold)
    {
      NAN_BM_TRACE_EVENT ("NAN_BM end page event dropped",
               RV_TRACE_LEVEL_ERROR);
    }
    else
    {
      NAN_BM_TRACE_EVENT ("NAN_BM end page event", RV_TRACE_LEVEL_ERROR);
      nan_bm_send_hisr_event_to_nan_bm (NAN_BM_END_PAGE_EVENT);
    }
  }
  /* ------------- handle end transfer ITs ------------- */
  if (bm_interrupt_status & NAN_BM_IT_MASK_END_TRANSFER)
  {
    NAN_BM_TRACE_EVENT ("NAN_BM end transfer event", RV_TRACE_LEVEL_ERROR);
    nan_bm_send_hisr_event_to_nan_bm (NAN_BM_END_TRANSFER_EVENT);
  }
  /* ------------- handle end ECC ITs ------------- */
  if (bm_interrupt_status & NAN_BM_IT_MASK_END_ECC)
  {
    /* expecting any ECC values */
    if (nan_bm_env_ctrl_blk_p->ecc_count < nan_bm_env_ctrl_blk_p->ecc_max)
    {
      /* any ECC sizes left */
      if (nan_bm_env_ctrl_blk_p->ecc_count + 1 < nan_bm_env_ctrl_blk_p->ecc_max)
      {
        /* select next ECC size */
        nan_bm_set_ecc_size (nan_bm_env_ctrl_blk_p->
                ecc_sizes_p[nan_bm_env_ctrl_blk_p->ecc_count +
                      1]);
      }
      /* store current ECC value */
      nan_bm_env_ctrl_blk_p->ecc_values_p[nan_bm_env_ctrl_blk_p->ecc_count++] =
      (T_NAN_BM_ECC) NAN_BM_REG (NAN_BM_ECC_VALUE_REG);
      if (nan_bm_env_ctrl_blk_p->ecc_count == nan_bm_env_ctrl_blk_p->ecc_max)
      {
        /* no more ECC sizes, inform NAN_BM task that all ECC values are ready */
        NAN_BM_TRACE_EVENT ("NAN_BM end ECC event", RV_TRACE_LEVEL_ERROR);
        nan_bm_send_hisr_event_to_nan_bm (NAN_BM_END_ECC_EVENT);
      }
    }
    else
    {
      /* we should not be here */
      volatile UINT32 dummy = NAN_BM_REG (NAN_BM_ECC_VALUE_REG);
    }
  }
  /* ------------- handle end busy ITs ------------- */
  if (bm_interrupt_status & NAN_BM_IT_MASK_END_BUSY)
  {
    NAN_BM_TRACE_EVENT ("NAN_BM end busy event", RV_TRACE_LEVEL_ERROR);
    nan_bm_send_hisr_event_to_nan_bm (NAN_BM_END_BUSY_EVENT);
  }
  nan_bm_enable_IT ();
}
#endif //#ifndef NAN_BM_POLLING


#ifndef NAN_BM_POLLING
static void
nan_bm_send_hisr_event_to_nan_bm (UINT16 event_id)
{
  if (rvf_send_event (nan_bm_env_ctrl_blk_p->addr_id, event_id) != RVF_OK)
  {
    NAN_BM_SEND_TRACE ("NAN_BM HISR could not send event", RV_TRACE_LEVEL_ERROR);
  };
}
#endif
static void nan_bm_wait_event_to (UINT16 expected_event, UINT16 saved_event, UINT16 timeout)
{
  UINT16 event_mask = NAN_BM_ALL_EVENTS & ~saved_event;
  UINT16 received_event;

  NAN_BM_TRACE_EVENT_PARAM ("NAN_BM waiting for event", expected_event,
             RV_TRACE_LEVEL_ERROR);
  for (;;)
  {
    received_event = rvf_wait (event_mask, timeout) & event_mask;
    if (received_event == 0)
    {
      NAN_BM_TRACE_EVENT_PARAM ("NAN_BM timeout while waiting for event",
                 expected_event, RV_TRACE_LEVEL_ERROR);
      break;
    }
    /* (at least) expected event  */
    if ((received_event & expected_event) != 0) 
    {
      NAN_BM_TRACE_EVENT_PARAM ("NAN_BM got event", expected_event,
                 RV_TRACE_LEVEL_ERROR);
      break;
    }
  
  }
}

T_NAN_BMD_RET nan_bm_handle_copy_req (UINT8 chip_select, UINT32 src_row, 
                                     UINT32 src_column, UINT32 dest_row,
                                     UINT32 dest_column)

{
  const UINT8 read_command = 0x00;
  const UINT8 copy_back_command = 0x8a;
  BOOL invalid_param = FALSE;

  /* check column range */
  invalid_param |= src_column >= NAN_BM_FLASH_PAGE_SIZE;
  invalid_param |= dest_column >= NAN_BM_FLASH_PAGE_SIZE;
  invalid_param |= nan_bm_area (src_column) != nan_bm_area (dest_column);
  /* check row range */
  invalid_param |= src_row >= NAN_BM_FLASH_NOF_PAGES;
  invalid_param |= dest_row >= NAN_BM_FLASH_NOF_PAGES;
  /* check chip select range */
  invalid_param |= chip_select > NAN_BM_MAX_CHIP_SELECT;
  if (invalid_param)
  {
    return NAN_BMD_INVALID_PARAMETER;;
  }

#ifdef NAN_BM_POLLING  
  bm_poll_status=0;
#endif

  nan_bm_set_read_mode ();
  nan_bm_clear_pending_ITs ();
  NAN_BM_PAGE[0] = read_command;
  NAN_BM_PAGE[1] = src_column;
  NAN_BM_PAGE[2] = src_row & 0xff;
  NAN_BM_PAGE[3] = src_row >> 8;
  nan_bm_set_first_command_bytes  (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes  (0);
  nan_bm_set_address_bytes (3);
  nan_bm_set_data_bytes    (0);
#ifndef NAN_BM_POLLING
  nan_bm_enable_end_transfer_IT ();
#endif 
  nan_bm_enable_transfer ();

#ifndef NAN_BM_POLLING
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, 0);
#else
 if (RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
  	/* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING);
   }
  
#endif

  NAN_BM_PAGE[0] = copy_back_command;
  NAN_BM_PAGE[1] = dest_column;
  NAN_BM_PAGE[2] = dest_row & 0xff;
  NAN_BM_PAGE[3] = dest_row >> 8;
  nan_bm_set_first_command_bytes  (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes  (0);
  nan_bm_set_address_bytes (3);
  nan_bm_set_data_bytes    (0);

  nan_bm_enable_transfer ();
#ifndef NAN_BM_POLLING
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, 0);
#else
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
      /* handling the error condn */
  	NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING);
  }
#endif
  return NAN_BMD_OK;
}


T_NAN_BMD_RET nan_bm_hw_flash_erase_block(UINT8 chip_select, UINT32 block_number)
{
  const UINT8 block_unprotect_command = 0x2d;
  const UINT8 erase_setup_command   = 0x60;
  const UINT8 erase_confirm_command = 0xd0;
  UINT16 nf_status,counter=0;


  if ((block_number >= NAN_BM_FLASH_NOF_BLOCKS) || (chip_select > NAN_BM_MAX_CHIP_SELECT))
  {
    return NAN_BMD_INVALID_PARAMETER;
  }

#ifdef NAN_BM_POLLING  
  bm_poll_status=0;
#endif

  nan_bm_set_write_mode ();
  nan_bm_clear_pending_ITs ();

  #if (CHIPSET != 15)
  NAN_BM_PAGE[0] = erase_setup_command;
  NAN_BM_PAGE[1] = block_number << 5 & 0xff;
  NAN_BM_PAGE[2] = block_number >> 3;
  NAN_BM_PAGE[3] = erase_confirm_command;

  nan_bm_set_first_command_bytes (1);
  nan_bm_set_second_command_bytes (1);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (2);
  nan_bm_set_data_bytes (0);
  #ifndef NAN_BM_POLLING
  nan_bm_enable_end_transfer_IT ();
  nan_bm_enable_end_busy_IT ();
  #endif
  nan_bm_enable_transfer ();
  #ifndef NAN_BM_POLLING
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_BUSY_EVENT);
  nan_bm_wait_event (NAN_BM_END_BUSY_EVENT, 0);
  #else
  if (RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
      /* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING);
   }
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_BUSY))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
  #endif
  nan_bm_disable_end_busy_IT ();
  nan_bm_disable_end_transfer_IT ();
#else//  (CHIPSET == 15)

  NAN_BM_PAGE[0] = block_unprotect_command;
  NAN_BM_PAGE[1] = erase_setup_command;
  NAN_BM_PAGE[2] = block_number<< 5 & 0xff;
  NAN_BM_PAGE[3] = block_number >> 3;
  NAN_BM_PAGE[4] = block_number >> 11 & 0x01;
  NAN_BM_PAGE[5] = erase_confirm_command;

  nan_bm_set_first_command_bytes (2);
  nan_bm_set_second_command_bytes (1);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (3);
  nan_bm_set_data_bytes (0);
  #ifndef NAN_BM_POLLING
  nan_bm_enable_end_transfer_IT ();
  nan_bm_enable_end_busy_IT ();
  #endif
  nan_bm_enable_transfer ();
  #ifndef NAN_BM_POLLING
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_BUSY_EVENT);
  nan_bm_wait_event (NAN_BM_END_BUSY_EVENT, 0);
  #else
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_BUSY))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
  #endif
  nan_bm_disable_end_busy_IT ();
  nan_bm_disable_end_transfer_IT ();

#endif

 /* check the NAND-status */
do
  {
    nf_status=nan_bm_handle_read_status_req(chip_select);
  }
  /* do polling, max 1000 times */
  while(!(nf_status & NAN_BM_FSTAT_DEVICE_OPERATION) && (++counter < NAN_BM_POLL_COUNT_MAX)); 

  if(nf_status & NAN_BM_FSTAT_PRG_ERASE) 
  {
    NAN_BM_SEND_TRACE_PARAM ("nan nan_bm_hw_flash_erase_block() erase error", nf_status, RV_TRACE_LEVEL_WARNING);
    return NAN_BMD_INVALID_BLOCK_ERR;
  }
/* added check for write protection */
  else if(!(nf_status & NAN_BM_FSTAT_WRITE_PROTECT))
  {
     NAN_BM_SEND_TRACE_PARAM ("nan nan_bm_hw_flash_erase_block() protect error", nf_status, RV_TRACE_LEVEL_WARNING);
    return NAN_BMD_DEVICE_PROTECT_ERROR;
  }
  return NAN_BMD_OK;

}

T_NAN_BMD_RET nan_bm_hw_handle_program_req ( UINT8 chip_select,  
                                            T_NAN_BM_RW_MODE mode,
                                            UINT32 row,         
                                            UINT32 column, 
                                            UINT32 *data_p,      
                                            UINT32 data_size,
                                            T_NAN_BM_ECC *ecc_values_p,  
                                            UINT32 *ecc_sizes_p,
                                            UINT32 ecc_count )
{
  BOOL use_ecc;
 #ifdef NAN_BM_32_BITS_FIFO
   UINT32 *data_p_32;
#endif
  UINT8  *data_p_8;
  UINT32 bytes_written;
  UINT32 bytes_provided;
  UINT8 pointer_command;
  const UINT8 seq_data_input_command = 0x80;
  const UINT8 program_command = 0x10;
  UINT32 column_offset;
  UINT32 column_max;
  BOOL invalid_param = FALSE;
  /* first page has different data size because it contains commands and data  */
  UINT32 page_offset;
  UINT32 page_size;
  UINT32 bytes_left;
  UINT32 data_chunk_size;
  UINT32 i;
  BOOL use_dma;
  UINT16 nf_status,counter=0;

 #ifdef NAN_BM_ENABLE_DMA
  T_DMA_CHANNEL available_dma_channel=NAN_BM_DMA_CHANNEL;
#endif
  bytes_written = 0;
  bytes_provided = data_size;
 #ifdef NAN_BM_32_BITS_FIFO
 data_p_32=(UINT32*)data_p;
#endif
  data_p_8=(UINT8*)data_p;

 NAN_BM_SEND_TRACE_PARAM("nan_bm_handle_program_req: the physical page ",
              row, RV_TRACE_LEVEL_DEBUG_HIGH);

  switch (mode)
  {
    case NAN_BM_RW_MODE0:
      pointer_command = 0x00;
      column_offset   = 0;
      column_max      = NAN_BM_FLASH_HALF_PAGE_OFFSET;
      break;
    case NAN_BM_RW_MODE1:
      pointer_command = 0x01;
      column_offset   = NAN_BM_FLASH_HALF_PAGE_OFFSET;
      column_max      = NAN_BM_FLASH_HALF_PAGE_OFFSET;
      break;
    case NAN_BM_RW_MODE2:
      pointer_command = 0x50;
      column_offset   = NAN_BM_FLASH_SPARE_OFFSET;
      column_max      = NAN_BM_FLASH_PAGE_SIZE - NAN_BM_FLASH_SPARE_OFFSET;
      break;
    default:
      invalid_param   = TRUE;
      pointer_command = 0x00;
      column_offset   = 0;
      column_max      = NAN_BM_FLASH_HALF_PAGE_OFFSET;
  }
  /* trying to write past page boundaries */
  invalid_param |= bytes_provided > NAN_BM_FLASH_PAGE_SIZE - column_offset;
  invalid_param |= bytes_provided == 0;
  /* check for page boundary; code review comment 88  */
  invalid_param |= bytes_provided >= NAN_BM_FLASH_PAGE_SIZE;
  /* check column range */
  invalid_param |= column >= column_max;
  /* check row range */
  invalid_param |= row >= NAN_BM_FLASH_NOF_PAGES;
  /* check chip select range */
  invalid_param |= chip_select > NAN_BM_MAX_CHIP_SELECT;
  /* setup ECC - off by default */
  use_ecc = FALSE;
  nan_bm_disable_ecc ();
  nan_bm_disable_end_ecc_IT ();
  nan_bm_env_ctrl_blk_p->ecc_max = 0;
  if (ecc_count > 0)
  {
    invalid_param |= !nan_bm_ecc_params_ok (ecc_sizes_p, ecc_values_p,
                                            ecc_count,   data_size);
    if (!invalid_param)
    {
      /* enable ECC */
      nan_bm_env_ctrl_blk_p->ecc_max      = ecc_count;
      nan_bm_env_ctrl_blk_p->ecc_count    = 0;
      nan_bm_env_ctrl_blk_p->ecc_values_p = ecc_values_p;
      nan_bm_env_ctrl_blk_p->ecc_sizes_p  = ecc_sizes_p;
      nan_bm_set_ecc_size (nan_bm_env_ctrl_blk_p->ecc_sizes_p[0]);
      use_ecc = TRUE;
      nan_bm_enable_ecc ();
#ifndef NAN_BM_POLLING
      nan_bm_enable_end_ecc_IT ();
#endif
    }
  }

  if (invalid_param)
  {
    return NAN_BMD_INVALID_PARAMETER;
  }

  nan_bm_clear_pending_ITs ();
#ifdef NAN_BM_POLLING
  bm_poll_status=0;
#endif

nan_bm_set_read_mode ();
  nan_bm_disable_end_page_IT ();
  nan_bm_disable_end_busy_IT ();
  NAN_BM_PAGE[0] = pointer_command;
  nan_bm_set_first_command_bytes (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (0);
  nan_bm_set_data_bytes (0);
#ifndef NAN_BM_POLLING
  nan_bm_enable_end_transfer_IT ();
#endif
  nan_bm_enable_transfer ();
#ifndef NAN_BM_POLLING
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, 0);
#else
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
#endif
  /* write command sequence */
  nan_bm_clear_pending_ITs ();
#ifdef NAN_BM_POLLING
  bm_poll_status=0;

#endif 
  bm_end_page_IT_count = 0;
  bm_end_page_IT_threshold = 0;
  nan_bm_set_write_mode ();
  page_offset = 0;
  NAN_BM_PAGE[page_offset++] = seq_data_input_command;
  NAN_BM_PAGE[page_offset++] = column;
  NAN_BM_PAGE[page_offset++] = row & 0xff;
  NAN_BM_PAGE[page_offset++] = (row >> 8) & 0xff;  
  NAN_BM_PAGE[page_offset++] = (row >> 16) & 0x01;
  /* fix by ronald for 32 bit misalignment */
#ifdef NAN_BM_32_BITS_FIFO
  NAN_BM_PAGE[page_offset++] = 0;
  NAN_BM_PAGE[page_offset++] = 0;
  NAN_BM_PAGE[page_offset++] = 0;
#endif

  nan_bm_set_first_command_bytes  (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes  (0);
  /* Adjust the number of addresses*/
  #ifdef NAN_BM_32_BITS_FIFO
  nan_bm_set_address_bytes (7);
  #else
  nan_bm_set_address_bytes (4);
  #endif
  nan_bm_set_data_bytes    (bytes_provided);

  /* fill rest of page with data */
  page_size = NAN_BM_PAGE_SIZE - page_offset;
  bytes_left = bytes_provided;
  data_chunk_size = bytes_left >= page_size ? page_size : bytes_left;
 

#if USE_MEMCPY

 memcpy((void*)(NAN_BM_PAGE+page_offset),data_p_8+bytes_written,data_chunk_size);

#else


#ifdef NAN_BM_32_BITS_FIFO
  /* copy client data to NAN_BM page */
  if((data_chunk_size % NAN_BUFFER_ACCESS_SIZE) == 0)
  {
    /* 32-bits steps */
    for (i = 0; i < (data_chunk_size/NAN_BUFFER_ACCESS_SIZE); i++)
    {
    	/*page_offset/4 = 2 ->  already 2 bytes have been filled with command and address  bytes */
       NAN_BM_PAGE_32[(page_offset/NAN_BUFFER_ACCESS_SIZE)+i] = data_p_32[(bytes_written/NAN_BUFFER_ACCESS_SIZE) + i];
    }
  }
  else
#endif //#ifdef NAN_BM_32_BITS_FIFO
   {
    	/* 8-bits steps */
   	for (i = 0; i < data_chunk_size; i++)
  	{
      		NAN_BM_PAGE[page_offset + i] = data_p_8[bytes_written + i];
   	}
   }

#endif


  bytes_written += data_chunk_size;
  bytes_left = bytes_provided - bytes_written;

  /* should we use DMA */
  switch (nan_bm_env_ctrl_blk_p->dma_mode)
  {
    case NAN_BM_FORCE_CPU:
      use_dma = FALSE;
      break;
    case NAN_BM_FORCE_DMA:
      use_dma = bytes_left >= NAN_BM_PAGE_SIZE;
      break;
    default:
      use_dma = bytes_left >= NAN_BM_DMA_DATA_SIZE_THRESHOLD;
  }

 
#ifdef NAN_BM_ENABLE_DMA
  if (use_dma)
  {
    /* reserve DMA channel */
    T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;

    if (RV_OK != dma_reserve_channel (DMA_CHAN_ANY,
                    0,
                    DMA_QUEUE_DISABLE,
                    128,
                    nan_bm_env_ctrl_blk_p->
                    path_to_return_queue)
      || !nan_bm_dma_wait_status_OK (&available_dma_channel))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_program_req: "
              "dma_reserve_channel failed", RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR; 
    }

    NAN_BM_SEND_TRACE ("nan_bm is using DMA", RV_TRACE_LEVEL_ERROR);

    /* set DMA channel parameters */
    dma_channel_parameters.data_width = DMA_DATA_S8;
    dma_channel_parameters.sync = DMA_SYNC_DEVICE_NAND_FLASH;
    dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_LOW;
    dma_channel_parameters.flush = DMA_FLUSH_DISABLED;
    dma_channel_parameters.nmb_frames = bytes_left / NAN_BM_PAGE_SIZE;
    dma_channel_parameters.nmb_elements = NAN_BM_PAGE_SIZE;

    /* ignore end page ITs caused by DMA except the last one */
    bm_end_page_IT_threshold = dma_channel_parameters.nmb_frames - 1;

    if (bytes_left % NAN_BM_PAGE_SIZE == 0)
    {

      /* no trailing partial page, so the last IT is an 
      end transfer IT --> ignore ALL end page ITs */
      bm_end_page_IT_threshold++;
    }
    dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
    dma_channel_parameters.secure = DMA_NOT_SECURED;
    dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;
    dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;

    dma_channel_parameters.source_address =
    (UINT32) (data_p + bytes_written);
    dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_POST_INC;
    dma_channel_parameters.source_packet = DMA_NOT_PACKED;
    dma_channel_parameters.source_burst = DMA_NO_BURST;

    dma_channel_parameters.destination_address = (UINT32) (NAN_BM_PAGE_BUFFER);
    dma_channel_parameters.destination_address_mode =
    DMA_ADDR_MODE_FRAME_INDEX;
    dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
    dma_channel_parameters.destination_burst = DMA_NO_BURST;

    if (RV_OK != dma_set_channel_parameters (available_dma_channel,
                         &dma_channel_parameters) ||
      !nan_bm_dma_wait_status_OK (NULL))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_program_req: "
              "dma_set_channel_parameters failed",
              RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR; 
    }

    /* DMA is now armed */
    nan_bm_enable_dma ();
    nan_bm_enable_end_page_IT ();
    nan_bm_enable_end_transfer_IT ();
    nan_bm_enable_transfer ();

    /* wait for DMA to complete */
    if (!nan_bm_dma_wait_status_OK (NULL))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_program_req: "
              "unexpected msg while waiting for "
              "DMA COMPLETE", RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR; 
    }

    /* release DMA channel */
    if (RV_OK != dma_release_channel (available_dma_channel)
      || !nan_bm_dma_wait_status_OK (NULL))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_program_req: "
              "dma_release_channel failed", RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR;
    }
    nan_bm_disable_dma ();

    bytes_written += dma_channel_parameters.nmb_frames * NAN_BM_PAGE_SIZE;
    bytes_left = bytes_provided - bytes_written;

    /* use CPU copying to write trailing partial page, if any */
    if (bytes_written < bytes_provided)
    {
      /* wait for end page event from last DMA frame */
      nan_bm_wait_event (NAN_BM_END_PAGE_EVENT, NAN_BM_END_ECC_EVENT);
      bytes_left = bytes_provided - bytes_written;
      data_chunk_size = bytes_left;

#if USE_MEMCPY
      memcpy((void*)NAN_BM_PAGE, data_p+bytes_written, data_chunk_size);

#else
      /* copy client data to NAN_BM page */
      for (i = 0; i < data_chunk_size; i++)
      {
        NAN_BM_PAGE[i] = data_p[bytes_written + i];
      }
#endif	  

      /* manually trigger page swap */
      NAN_BM_PAGE[NAN_BM_PAGE_SIZE - 1] = 0x10;
      bytes_written += data_chunk_size;
    }
    nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_ECC_EVENT);
   }
  else
  #endif // NAN_BM_ENABLE_DMA
  {
    // no DMA 
    nan_bm_disable_dma ();
#ifndef NAN_BM_POLLING
    nan_bm_enable_end_page_IT ();
    nan_bm_enable_end_transfer_IT ();
#endif
    nan_bm_enable_transfer ();

#ifdef NAN_BM_POLLING
    /* immediately fill second page */
    if(bytes_written >= bytes_provided)
    {
    		if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
    		{
 			/* handling the error condn */
  			NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  		}
    }
    else
#endif //#ifdef NAN_BM_POLLING
    {
        while (bytes_written < bytes_provided)
    {
      bytes_left = bytes_provided - bytes_written;
      data_chunk_size =
      bytes_left >= NAN_BM_PAGE_SIZE ? NAN_BM_PAGE_SIZE : bytes_left;

#if USE_MEMCPY

	 memcpy((void*)NAN_BM_PAGE,data_p_8+bytes_written,data_chunk_size);

  
      if ( data_chunk_size < NAN_BM_PAGE_SIZE)
	       {
		/* manually trigger page swap */
		 NAN_BM_PAGE[NAN_BM_PAGE_SIZE - 1] = 0x10;
	       }

#else
#ifdef NAN_BM_32_BITS_FIFO
      /* copy client data to NAN_BM page */
      if((data_chunk_size % NAN_BUFFER_ACCESS_SIZE) == 0)
      {
        /* 32-bits steps */
        for (i = 0; i < (data_chunk_size/NAN_BUFFER_ACCESS_SIZE); i++)
        {
          NAN_BM_PAGE_32[i] = data_p_32[(bytes_written/NAN_BUFFER_ACCESS_SIZE) + i];
        }
            if (i < (NAN_BM_PAGE_SIZE/NAN_BUFFER_ACCESS_SIZE))
        {
        /* manually trigger page swap  */
        NAN_BM_PAGE[NAN_BM_PAGE_SIZE - 1] = 0x10;
        }
      }
      else
#endif //#ifdef NAN_BM_32_BITS_FIFO
      {
	       /* 8-bits steps */
	      for (i = 0; i < data_chunk_size; i++)
	      {
		  NAN_BM_PAGE[i] = data_p_8[bytes_written + i];
		}
	       if (i < NAN_BM_PAGE_SIZE)
	       {
		/* manually trigger page swap */
		 NAN_BM_PAGE[NAN_BM_PAGE_SIZE - 1] = 0x10;
	       }
      }

#endif
		  
	      bytes_written += data_chunk_size;
#ifndef NAN_BM_POLLING
	      nan_bm_wait_event (NAN_BM_END_PAGE_EVENT,
		      NAN_BM_END_ECC_EVENT | NAN_BM_END_TRANSFER_EVENT);
#else
	      if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_PAGE | NAN_BM_IT_MASK_END_TRANSFER))
	      	{
 			/*  handling the error condn */
  			NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  		}
#endif
	    }
	    }
#ifndef NAN_BM_POLLING
	    nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_ECC_EVENT);
#endif
	  }


    if (use_ecc)
  {
  
#ifndef NAN_BM_POLLING
    nan_bm_wait_event (NAN_BM_END_ECC_EVENT, 0);
#else
     if(RV_OK != nan_bm_poll_event(NAN_BM_END_ECC_EVENT))
    {
    	 	/* handling the error condn */
  		NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  	}
#endif
    nan_bm_disable_ecc ();
    nan_bm_disable_end_ecc_IT ();
  }


	  /* send program command command separately, since sending it
	   directly as 'third command' does not seem to work  */
	  nan_bm_disable_end_page_IT ();
	  nan_bm_clear_pending_ITs ();
#ifdef NAN_BM_POLLING
	  bm_poll_status=0;
#endif

	  NAN_BM_PAGE[0] = program_command;

	  nan_bm_set_first_command_bytes (1);
	  nan_bm_set_second_command_bytes (0);
	  nan_bm_set_third_command_bytes (0);
	  nan_bm_set_address_bytes (0);
	  nan_bm_set_data_bytes (0);
	  nan_bm_enable_transfer ();
#ifndef NAN_BM_POLLING
	  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_ECC_EVENT);
#else
	  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
	  {
 			/*handling the error condn */
  			NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  	  }
         /* wait till the NAND is ready */
         if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_BUSY))
          {
 		/* handling the error condn*/
  		NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  	   }
#endif
	  nan_bm_disable_end_transfer_IT ();
	 
	  /*check the NAND-status */
	  do
	  {
	    nf_status=nan_bm_handle_read_status_req(chip_select);
	  }
	  while(!(nf_status & NAN_BM_FSTAT_DEVICE_OPERATION) && (++counter < NAN_BM_POLL_COUNT_MAX)); // do polling, max 1000 times
	  if(nf_status & NAN_BM_FSTAT_PRG_ERASE)
	  {
	    NAN_BM_SEND_TRACE_PARAM ("nan nan_bm_hw_handle_program_req() program error", nf_status, RV_TRACE_LEVEL_WARNING);
	    return NAN_BMD_INVALID_BLOCK_ERR;
	  }

	  return NAN_BMD_OK;
}

 

T_NAN_BMD_RET nan_bm_hw_handle_read_req (
			 UINT8 chip_select,
			 T_NAN_BM_RW_MODE mode, 
			 UINT32 row,
			 UINT32 column, 
			 UINT32 *data_p,
			 UINT32 data_size, 
			 T_NAN_BM_ECC *ecc_values_p,
			 UINT32 *ecc_sizes_p,
			 UINT32 ecc_count) 
	{ 
	  BOOL use_ecc;
	  UINT32 bytes_expected;
	  UINT32 bytes_received;
	  UINT32 bytes_left;
	  UINT32 data_chunk_size;
	  UINT8 read_command;
	  UINT32 column_offset;
	  UINT32 column_max;
	  BOOL invalid_param = FALSE;
#ifdef NAN_BM_32_BITS_FIFO
    UINT32 *data_p_32;
#endif
  UINT8  *data_p_8;
  UINT32 i;
  BOOL use_dma;
#ifdef NAN_BM_ENABLE_DMA
  T_DMA_CHANNEL available_dma_channel=NAN_BM_DMA_CHANNEL;
#endif
  bytes_received = 0;
  bytes_expected = data_size;
#ifdef NAN_BM_32_BITS_FIFO
   data_p_32=(UINT32*)data_p;
#endif
  data_p_8=(UINT8*)data_p;

  switch (mode)
  {
    case NAN_BM_RW_MODE0:
      read_command  = 0x00;
      column_offset = 0;
      column_max    = NAN_BM_FLASH_HALF_PAGE_OFFSET;
      break;
    case NAN_BM_RW_MODE1:
      read_command  = 0x01;
      column_offset = NAN_BM_FLASH_HALF_PAGE_OFFSET;
      column_max    = NAN_BM_FLASH_HALF_PAGE_OFFSET;
      break;
    case NAN_BM_RW_MODE2:
      read_command  = 0x50;
      column_offset = NAN_BM_FLASH_SPARE_OFFSET;
      column_max    = NAN_BM_FLASH_PAGE_SIZE - NAN_BM_FLASH_SPARE_OFFSET;
      break;
    default:
      invalid_param = TRUE;
      read_command  = 0x00;
      column_offset = 0;
      column_max    = NAN_BM_FLASH_HALF_PAGE_OFFSET;
  }
  /* trying to read past page boundaries */
  invalid_param |= bytes_expected > NAN_BM_FLASH_PAGE_SIZE - column_offset;
  invalid_param |= bytes_expected == 0;
  /* check column range */
  invalid_param |= column >= column_max;
  /* check row range */
  invalid_param |= row >= NAN_BM_FLASH_NOF_PAGES;
  /* check chip select range */
  invalid_param |= chip_select > NAN_BM_MAX_CHIP_SELECT;
  /* setup ECC - off by default */
  use_ecc = FALSE;
  nan_bm_disable_ecc ();
  nan_bm_disable_end_ecc_IT ();
  nan_bm_env_ctrl_blk_p->ecc_max = 0;
  if (ecc_count > 0)
  {
    invalid_param |= !nan_bm_ecc_params_ok (ecc_sizes_p, ecc_values_p,
                                            ecc_count,    data_size);
    if (!invalid_param)
    {
      /* enable ECC */
      nan_bm_env_ctrl_blk_p->ecc_max      = ecc_count;
      nan_bm_env_ctrl_blk_p->ecc_count    = 0;
      nan_bm_env_ctrl_blk_p->ecc_values_p = ecc_values_p;
      nan_bm_env_ctrl_blk_p->ecc_sizes_p  = ecc_sizes_p;
      nan_bm_set_ecc_size (nan_bm_env_ctrl_blk_p->ecc_sizes_p[0]);
      use_ecc = TRUE;
      nan_bm_enable_ecc ();
#ifndef NAN_BM_POLLING
      nan_bm_enable_end_ecc_IT ();
#endif 
    }
  }

  if (invalid_param)
  {    
    return NAN_BMD_INVALID_PARAMETER;
  }

  nan_bm_clear_pending_ITs ();
#ifdef NAN_BM_POLLING  
  bm_poll_status=0;
#endif
  bm_end_page_IT_count = 0;
  bm_end_page_IT_threshold = 0;

  /* should we use DMA? */
  switch (nan_bm_env_ctrl_blk_p->dma_mode)
  {
  case NAN_BM_FORCE_CPU:
    use_dma = FALSE;
    break;
  case NAN_BM_FORCE_DMA:
    use_dma = bytes_expected % NAN_BM_PAGE_SIZE == 0;
    break;
  default:
    use_dma = bytes_expected % NAN_BM_PAGE_SIZE == 0
    && bytes_expected >= NAN_BM_DMA_DATA_SIZE_THRESHOLD;
  }

  if (use_dma)
  {
#ifdef NAN_BM_ENABLE_DMA
    /* reserve DMA channel */
    T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;

    if (RV_OK != dma_reserve_channel (DMA_CHAN_ANY,
                    0,
                    DMA_QUEUE_DISABLE,
                    128,
                    nan_bm_env_ctrl_blk_p->
                    path_to_return_queue)
      || !nan_bm_dma_wait_status_OK (&available_dma_channel))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_read_req: "
              "dma_reserve_channel failed", RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR;
    }

    /* set DMA channel parameters */
    dma_channel_parameters.data_width = DMA_DATA_S8;
    dma_channel_parameters.sync = DMA_SYNC_DEVICE_NAND_FLASH;
    dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_LOW;
    dma_channel_parameters.flush = DMA_FLUSH_DISABLED;
    dma_channel_parameters.nmb_frames = bytes_expected / NAN_BM_PAGE_SIZE;
    dma_channel_parameters.nmb_elements = NAN_BM_PAGE_SIZE;
    /* ignore end page ITs caused by DMA (the last IT is an end transfer IT) */
    end_page_IT_threshold = dma_channel_parameters.nmb_frames - 1;
    dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
    dma_channel_parameters.secure = DMA_NOT_SECURED;
    dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;
    dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;

    dma_channel_parameters.source_address = (UINT32) (NAN_BM_PAGE_BUFFER);
    dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_FRAME_INDEX;
    dma_channel_parameters.source_packet = DMA_NOT_PACKED;
    dma_channel_parameters.source_burst = DMA_NO_BURST;

    dma_channel_parameters.destination_address = (UINT32) data_p;
    dma_channel_parameters.destination_address_mode =
    DMA_ADDR_MODE_POST_INC;
    dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
    dma_channel_parameters.destination_burst = DMA_NO_BURST;

    if (RV_OK != dma_set_channel_parameters (available_dma_channel,
                         &dma_channel_parameters) ||
      !nan_bm_dma_wait_status_OK (NULL))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_read_req: "
              "dma_set_channel_parameters failed",
              RV_TRACE_LEVEL_ERROR);
      
      return NAN_BMD_INTERNAL_ERR;
    }
    /* DMA is now armed */
#endif // NAN_BM_ENABLE_DMA
  }
  nan_bm_set_read_mode ();
  
 
  NAN_BM_PAGE[0] = read_command;
  NAN_BM_PAGE[1] = column;
  NAN_BM_PAGE[2] = row & 0xff;
  NAN_BM_PAGE[3] = row >> 8;
  NAN_BM_PAGE[4] = row >> 16 & 0x01;

  nan_bm_set_first_command_bytes (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (4);
  nan_bm_set_data_bytes (bytes_expected);


  if (use_dma)
  {
#ifdef NAN_BM_ENABLE_DMA
    nan_bm_enable_dma ();
    nan_bm_enable_end_page_IT ();
    nan_bm_enable_end_transfer_IT ();
    nan_bm_enable_transfer ();

    /* wait for DMA to complete */
    if (!nan_bm_dma_wait_status_OK (NULL))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_read_req: "
              "unexpected msg while waiting for "
              "DMA COMPLETE", RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR;
    }

    /* release DMA channel */
    if (RV_OK != dma_release_channel (available_dma_channel)
      || !nan_bm_dma_wait_status_OK (NULL))
    {
      NAN_BM_SEND_TRACE ("nan_bm_handle_read_req: "
              "dma_release_channel failed", RV_TRACE_LEVEL_ERROR);
      return NAN_BMD_INTERNAL_ERR;
    }

    /* wait for end transfer  */
    nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_ECC_EVENT);
#endif // NAN_BM_ENABLE_DMA
  }
  else
  {
    /* CPU copying */
#ifndef NAN_BM_POLLING
    nan_bm_enable_end_page_IT ();
    nan_bm_enable_end_transfer_IT ();
#endif
    nan_bm_disable_dma ();
    nan_bm_enable_transfer ();
    do
    {
#ifndef NAN_BM_POLLING
      nan_bm_wait_event (NAN_BM_END_PAGE_EVENT | NAN_BM_END_TRANSFER_EVENT,
              NAN_BM_END_ECC_EVENT);
#else
      if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_PAGE | NAN_BM_IT_MASK_END_TRANSFER))
      	{
 		/*handling the error condn */
  		NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  	}
#endif
      bytes_left = bytes_expected - bytes_received;
      data_chunk_size =
      bytes_left >= NAN_BM_PAGE_SIZE ? NAN_BM_PAGE_SIZE : bytes_left;

#if USE_MEMCPY

     memcpy(data_p_8+bytes_received, (const void*)NAN_BM_PAGE, data_chunk_size);

#else
#ifdef NAN_BM_32_BITS_FIFO
      if(data_chunk_size == NAN_BM_PAGE_SIZE)
      {
        for (i = 0; i < (data_chunk_size/NAN_BUFFER_ACCESS_SIZE); i++)
        {
          /* read in steps of 4 bytes */
          data_p_32[(bytes_received/NAN_BUFFER_ACCESS_SIZE) + i] = NAN_BM_PAGE_32[i];
        }
      }
      else
#endif 
      {
      for (i = 0; i < data_chunk_size; i++)
      {
          /* read in steps of 1 byte */
          data_p_8[bytes_received + i] = NAN_BM_PAGE[i];
        }
      }

#endif

      bytes_received += data_chunk_size;
    }
    while (bytes_received < bytes_expected);
  }
  nan_bm_disable_dma ();
  nan_bm_disable_end_page_IT ();
  nan_bm_disable_end_transfer_IT ();
  /* all data read */
  if (use_ecc)
  {
#ifndef NAN_BM_POLLING
    nan_bm_wait_event (NAN_BM_END_ECC_EVENT, 0);
#else
       if(RV_OK != nan_bm_poll_event(NAN_BM_END_ECC_EVENT))
      	{
 		/* handling the error condn */
  		NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  	}
#endif
    nan_bm_disable_ecc ();
    nan_bm_disable_end_ecc_IT ();
  }
   return NAN_BMD_OK;
  
}



UINT16 nan_bm_handle_read_status_req (UINT8 chip_select)
{
  const UINT8 read_status_command = 0x70;

  if (chip_select > NAN_BM_MAX_CHIP_SELECT)
  {
    NAN_BM_SEND_TRACE ("invalid chip select: ",
               RV_TRACE_LEVEL_ERROR);
  }

  nan_bm_set_read_mode ();
  nan_bm_clear_pending_ITs ();

  NAN_BM_PAGE[0] = read_status_command;

  nan_bm_set_first_command_bytes (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (0);
  nan_bm_set_data_bytes (1);
#ifndef NAN_BM_POLLING
  nan_bm_enable_end_transfer_IT ();
#endif
  nan_bm_disable_end_busy_IT ();
  nan_bm_enable_transfer ();
#ifdef NAN_BM_POLLING
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
 		/* handling the error condn */
  		NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
#else
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, 0);
#endif
  nan_bm_disable_end_transfer_IT ();

  /* copy result to re-used response message */
return NAN_BM_PAGE[0] ;
}


T_NAN_BMD_RET nan_bm_handle_reset_req (UINT8 chip_select)
{
  const UINT8 reset_command = 0xff;

  if (chip_select > NAN_BM_MAX_CHIP_SELECT)
  {
    return NAN_BMD_INVALID_PARAMETER;
  }

#ifdef NAN_BM_POLLING  
  bm_poll_status=0;
#endif

  nan_bm_set_write_mode ();
  nan_bm_clear_pending_ITs ();

  NAN_BM_PAGE[0] = reset_command;

  nan_bm_set_first_command_bytes (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (0);
  nan_bm_set_data_bytes (0);
  nan_bm_disable_end_transfer_IT ();
#ifndef NAN_BM_POLLING
  nan_bm_enable_end_busy_IT ();
  nan_bm_enable_end_transfer_IT ();
#endif //#ifndef NAN_BM_POLLING
  nan_bm_enable_transfer ();
#ifndef NAN_BM_POLLING
  nan_bm_wait_event (NAN_BM_END_TRANSFER_EVENT, NAN_BM_END_BUSY_EVENT);
  nan_bm_wait_event (NAN_BM_END_BUSY_EVENT, 0);
#else
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
  if(RV_OK != nan_bm_poll_event(NAN_BM_IT_MASK_END_BUSY))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
  }
#endif //#ifndef NAN_BM_POLLING
  nan_bm_disable_end_busy_IT ();
  nan_bm_disable_end_transfer_IT ();

  return NAN_BMD_OK;


}

#if (DATALIGHT_NAND == 1)
/* Exported Functions for FlashFx */

void conf_acquire_pins_nand()
{
 	#if (CHIPSET == 15)			
	conf_acquire_pins(MUX_NANDFLASH);
 	#endif
return;
}


void conf_release_pins_nand()
{
#if (CHIPSET == 15)			
	conf_release_pins(MUX_NANDFLASH);
#endif
return;
}
 


int nan_bm_env_ctrl_init()
{
  int test_erase=0;
  int block_number;

  /* Create instance gathering all the variable used by EXPL instance */
   if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_NAN_BM_ENV_CTRL_BLK), (T_RVF_BUFFER**)&nan_bm_env_ctrl_blk_p) != RVF_GREEN)
       {
               return -1;	
	   }

    nan_bm_env_ctrl_blk_p->initialised = FALSE;

    /* Store the address ID. */
    nan_bm_env_ctrl_blk_p->addr_id =0 ; /* No handle */

	/* Store the pointer to the error function. */
    nan_bm_env_ctrl_blk_p->error_ft = rvm_error;
	
    nan_bm_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;
    
  	nan_bm_env_ctrl_blk_p->dma_mode = NAN_BM_FORCE_CPU;
  	nan_bm_env_ctrl_blk_p->ecc_count = 0;

    nan_bm_env_ctrl_blk_p->saved_req_msg_p = NULL;

    initialize_nand_mutex();   /* This might be an additional call */
	
    nan_bm_controller_init ();


	conf_acquire_pins_nand();

    nan_bm_handle_reset_req(0); // chip_select = 0

	conf_release_pins_nand();
 
    if(test_erase)
    	{


	conf_acquire_pins_nand();
 
		
    	  for(block_number=0;block_number<4096;block_number++)
    	  	{
    	   		nan_bm_hw_flash_erase_block(0, block_number);
    	  	}	
conf_release_pins_nand();
		  
    	}

 	
	return 0;
}


int nan_bm_env_ctrl_clean()
{
  delete_nand_mutex();

  if(nan_bm_env_ctrl_blk_p!= NULL)
  	{
	  	rvf_free_buf (nan_bm_env_ctrl_blk_p);
		nan_bm_env_ctrl_blk_p= NULL;
  	}
  return 0;
  
}



int  nan_bm_hw_handle_read_req_mode0 (	 unsigned int  row,
												 unsigned int column, 
												 unsigned char *data_p,
												 unsigned int data_size, 
												 unsigned int *ecc_values_p,
												 unsigned int *ecc_sizes_p,
												 unsigned int ecc_count) 
{
 int ret;

 conf_acquire_pins_nand();

  ret = (int) nan_bm_hw_handle_read_req( 0,
										 NAN_BM_RW_MODE0,
										 row,
										 column,
										 (UINT32*) data_p,
										 data_size,
										 (T_NAN_BM_ECC *)ecc_values_p,
										 (UINT32 *)ecc_sizes_p,
										 ecc_count);

conf_release_pins_nand();

 while(ret != 0)
 	{
 	  ;
 	}
 


  	
  return ret;
}



int  nan_bm_hw_handle_read_req_mode1 (	 unsigned int  row,
												 unsigned int column, 
												 unsigned char *data_p,
												 unsigned int data_size, 
												 unsigned int *ecc_values_p,
												 unsigned int *ecc_sizes_p,
												 unsigned int ecc_count) 
{

 int ret;
 
conf_acquire_pins_nand();

 ret = (int) nan_bm_hw_handle_read_req( 0,
										 NAN_BM_RW_MODE1,
										 row,
										 column,
										 (UINT32*) data_p,
										 data_size,
										 (T_NAN_BM_ECC *)ecc_values_p,
										 (UINT32 *)ecc_sizes_p,
										 ecc_count);

conf_release_pins_nand();
 while(ret != 0)
 	{
 	  ;
 	}


 



  return ret;
}



int  nan_bm_hw_handle_read_req_mode2(	 unsigned int  row,
												 unsigned int column, 
												 unsigned char *data_p,
												 unsigned int data_size, 
												 unsigned int *ecc_values_p,
												 unsigned int *ecc_sizes_p,
												 unsigned int ecc_count) 
{

 int ret;
 
conf_acquire_pins_nand();
 
 ret = (int) nan_bm_hw_handle_read_req( 0,
										 NAN_BM_RW_MODE2,
										 row,
										 column,
										 (UINT32*) data_p,
										 data_size,
										 (T_NAN_BM_ECC *)ecc_values_p,
										 (UINT32 *)ecc_sizes_p,
										 ecc_count);

conf_release_pins_nand();
 while(ret != 0)
 	{
 	 ;
 	}
 


  return ret;
 
}


int nan_bm_hw_handle_program_req_mode0 (          unsigned int  row,         
                                            unsigned int column, 
                                            const unsigned char *data_p,      
                                            unsigned int data_size,
                                            unsigned int *ecc_values_p,  
                                            unsigned int *ecc_sizes_p,
                                            unsigned int ecc_count )
{
int ret;
 
conf_acquire_pins_nand();

ret = (int) nan_bm_hw_handle_program_req ( 0,  
                                            NAN_BM_RW_MODE0,
                                            row,         
                                            column, 
                                            (UINT32*) data_p,      
                                            data_size,
                                            (T_NAN_BM_ECC *) ecc_values_p,  
                                            (UINT32 *) ecc_sizes_p,
                                            ecc_count );

conf_release_pins_nand();
  while(ret != 0)
 	{
 	  ;
 	}




  return ret;
                                            
}




int nan_bm_hw_handle_program_req_mode1 (          unsigned int  row,         
                                            unsigned int column, 
                                            const unsigned char *data_p,      
                                            unsigned int data_size,
                                            unsigned int *ecc_values_p,  
                                            unsigned int *ecc_sizes_p,
                                            unsigned int ecc_count )
{

 int ret;
 
conf_acquire_pins_nand();

ret = (int) nan_bm_hw_handle_program_req ( 0,  
                                            NAN_BM_RW_MODE1,
                                            row,         
                                            column, 
                                            (UINT32*) data_p,      
                                            data_size,
                                            (T_NAN_BM_ECC *) ecc_values_p,  
                                            (UINT32 *) ecc_sizes_p,
                                            ecc_count );

conf_release_pins_nand();
 while(ret != 0)
 	{
 	 ;
 	}

 


  return ret;
                                            
}



int nan_bm_hw_handle_program_req_mode2 (    unsigned int  row,         
                                            unsigned int column, 
                                            const unsigned char *data_p,      
                                            unsigned int data_size,
                                            unsigned int *ecc_values_p,  
                                            unsigned int *ecc_sizes_p,
                                            unsigned int ecc_count )
{
 int ret;
 

 conf_acquire_pins_nand();
ret = (int) nan_bm_hw_handle_program_req ( 0,  
                                            NAN_BM_RW_MODE2,
                                            row,         
                                            column, 
                                            (UINT32*) data_p,      
                                            data_size,
                                            (T_NAN_BM_ECC *) ecc_values_p,  
                                            (UINT32 *) ecc_sizes_p,
                                            ecc_count );

conf_release_pins_nand();
  while(ret != 0)
 	{
 	  ;
 	}




  return ret;
                                            
}

int nan_bm_hw_flash_erase_block_dl(unsigned int block_number)
{
 int ret;
 

conf_acquire_pins_nand();
   ret =  (int) nan_bm_hw_flash_erase_block(0, block_number);
conf_release_pins_nand();

 while(ret != 0)
 	{
 	 ;
 	}




  return ret;
 
}	



#if 0

UINT32 dl_poll_status=0;
T_NAN_BM_ECC gloabl_ecc_data=0;



static T_RVM_RETURN nan_dl_poll_event (UINT32 event)
{
  int i;
  UINT32 status;

  
  
    for(i=0; i < 100000; i++)
  {
    status=NAN_BM_REG (NAN_BM_STATUS_IT_REG);
    dl_poll_status|=status;
#if 0
    if((dl_poll_status&NAN_BM_IT_MASK_END_ECC)== NAN_BM_IT_MASK_END_ECC)
    	{
  			gloabl_ecc_data = (T_NAN_BM_ECC) NAN_BM_REG (NAN_BM_ECC_VALUE_REG);

			/* unset ecc */
			dl_poll_status &=(~(NAN_BM_IT_MASK_END_ECC));
    	}	
#endif	


    if((dl_poll_status & event) != 0)
    {
      return RV_OK;
    }
  }
  return RV_INTERNAL_ERR;
}





int NAND_calc_ecc_read(const unsigned char* pData, unsigned int data_bytes, unsigned char *ecc,unsigned int ecc_size)
{

  UINT32 data_chunk_size,bytes_written,bytes_left;
  UINT8 pointer_command = 0;
  const UINT8 seq_data_input_command = 0x80;
  UINT32 index;
  UINT32 page_size;
  T_NAN_BM_ECC ecc_data;
  int status;
  

 /* Assume the pins are already aquired */

  nan_bm_disable_ecc ();
  nan_bm_disable_end_ecc_IT ();
 

  
  /* write command sequence */

  gloabl_ecc_data = 0;
  bytes_written = 0;
  bytes_left = data_bytes;

  

gloabl_ecc_data= 0;
while(bytes_written < data_bytes)
{

  data_chunk_size = bytes_left >= NAN_BM_PAGE_SIZE? NAN_BM_PAGE_SIZE : bytes_left;


  nan_bm_clear_pending_ITs ();
  nan_bm_set_read_mode ();
  nan_bm_disable_end_page_IT ();
  nan_bm_disable_end_busy_IT ();
  NAN_BM_PAGE[0] = pointer_command;
  nan_bm_set_first_command_bytes (1);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes (0);
  nan_bm_set_address_bytes (0);
  nan_bm_set_data_bytes (0);
  nan_bm_enable_transfer ();
  dl_poll_status = 0;
  if(RV_OK != nan_dl_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
	status = NAN_BMD_NOT_READY;
	goto error_end;
  }



  nan_bm_clear_pending_ITs ();
  nan_bm_disable_end_page_IT ();
  nan_bm_disable_end_busy_IT ();

  nan_bm_set_ecc_size (data_chunk_size);   /* it should be either 128,256 or 512 */
  nan_bm_enable_ecc ();
 
  nan_bm_set_write_mode ();

  nan_bm_set_first_command_bytes  (0);
  nan_bm_set_second_command_bytes (0);
  nan_bm_set_third_command_bytes  (0);
  nan_bm_set_address_bytes(0);   /* No address */

  nan_bm_set_data_bytes(data_chunk_size);


  memcpy((void*)(NAN_BM_PAGE),pData+bytes_written,data_chunk_size);
  nan_bm_disable_dma ();
  nan_bm_enable_transfer();  /* start the transfer */
    	  
  
  
  bytes_written += data_chunk_size;
  bytes_left -= data_chunk_size; 

  if(data_chunk_size < NAN_BM_PAGE_SIZE)
  	{
  	   NAN_BM_PAGE[NAN_BM_PAGE_SIZE-1]=0x10;
  	}

  dl_poll_status=0;
  if(RV_OK != nan_dl_poll_event(NAN_BM_IT_MASK_END_ECC))
			{ 
 			/*  handling the error condn */
  			NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
   			status = NAN_BMD_ECC_ERROR;

			while(status != 0)
				{
				 ;
				}
			goto error_end;   
  			}
  gloabl_ecc_data = (T_NAN_BM_ECC) NAN_BM_REG (NAN_BM_ECC_VALUE_REG);
  nan_bm_disable_ecc ();
  nan_bm_set_ecc_size (0);   /* it should be either 128,256 or 512 */

  if(RV_OK != nan_dl_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
  {
 	/* handling the error condn */
  	NAN_BM_SEND_TRACE ("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
	status = NAN_BMD_INVALID_BLOCK_ERR;
	goto error_end;
  }

 
     nan_bm_disable_end_transfer_IT (); 

}

#if 0
   dl_poll_status=0; 
   if(RV_OK != nan_dl_poll_event(NAN_BM_IT_MASK_END_TRANSFER))
			{ 
 			/*  handling the error condn */
  			NAN_BM_SEND_TRACE("nan_bm_poll event timed out", RV_TRACE_LEVEL_WARNING); 
			status = NAN_BMD_INVALID_BLOCK_ERR;
			
			while(status != 0)
				{
				 ;
				}
			goto error_end;
  			}

#endif

#if 1

   if(gloabl_ecc_data == 0)
   	{
			 ecccalccode(pData, &gloabl_ecc_data);   /* software ECC */   
   	}


    while(gloabl_ecc_data == 0)
  	{
  	  ;
  	}

#endif 	


 
  ecc_data= gloabl_ecc_data;

/*
  if(ecc_data==0)
  	{
  	  status = NAN_BMD_ECC_ERROR;
	  goto error_end;
  	}

*/  	

 
  
  for(index=0;index<ecc_size && index < (sizeof(T_NAN_BM_ECC)); index++)
  	{
  	   ecc[index]=ecc_data&0xFF;
	   ecc_data >>=8;
  	}

  /* fill the remaining bytes with 0s */
  for(;index<ecc_size;index++)
  	{
  	   ecc[index]=0;
  	}

status = NAN_BMD_OK;

error_end:
   while(status != 0)
  	{
  	 ;
  	}

  return status;
}

#else



int NAND_calc_ecc_read(const unsigned char* pData, unsigned int data_bytes, unsigned char *ecc,unsigned int ecc_size)
{

#define  DATA_BYTES 256
#define ECC_BYTES  3


  if((DATA_BYTES != data_bytes) || ((ECC_BYTES != ecc_size)))
  	return -1;

   return -2;


#undef DATA_BYTES
#undef ECC_BYTES
 
}

#endif

#endif	

