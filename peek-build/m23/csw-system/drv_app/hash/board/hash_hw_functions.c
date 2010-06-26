/**
 * @file  hash_hw_functions.c
 *
 * HW functions for hash SWE.
 *  BOARD file
 * @author  ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/31/2003 Eddy Oude Middendorp (ICT)  split from common &Completion.
 *  
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#include "memif/mem.h"
#include "hash/hash_i.h"
#include <string.h>

#include "hash/hash_api.h"
#include "hash/hash_hw_functions.h"
#include "chipset.cfg"

#if (CHIPSET!=15)
#define HASH_USE_DMG 1
#else
#define HASH_USE_DMG 0
#endif


#if (HASH_USE_DMG==1)
#include "dmg/dmg_api.h" 
#include "dmg/dmg_message.h" 
#else
#include "dma/dma_api.h" 
#include "dma/dma_message.h" 
#endif

#define LAST_BLOCK TRUE
#define ptr_is_aligned(p) (0 == ((UINT32)(p) & 0x03))
static BOOL context_saved;
static void hash_poll_input_ready (void);
static void hash_poll_output_ready (void);

/**
 * get_hardware_version
 *
 * Detailled description.
 * This function returns the hardware version of the driver. The version 
 * is coded in hardware register in the hash.
 * 
 * @return  HW_VERSION
 */
UINT32 hash_hw_get_hardware_version (void)
{
  return HASH_REV_REG;          /* Read the hardware version */
}

/**
 * hash_init_hw
 *
 * Detailled description.
 * This function disables DMA/DMG transfer and resets interrupt mask.
 * 
 */
void hash_hw_init (void)
{
  HASH_MASK_REG = HASH_DMA_DISABLE | HASH_AUTO_IDLE_ENABLE; // No DMA, Mask Interrupt, No softreset, Enable Auto Idle
}

static void restore_context (T_HASH_CLIENT_STATE * context_p)
{
  context_saved     = FALSE;
  HASH_DIGEST_A_REG = context_p->digest_a;
  HASH_DIGEST_B_REG = context_p->digest_b;
  HASH_DIGEST_C_REG = context_p->digest_c;
  HASH_DIGEST_D_REG = context_p->digest_d;
  HASH_DIGEST_E_REG = context_p->digest_e;
  HASH_DIGCNT_REG   = context_p->digcnt;
}

static void save_context (T_HASH_CLIENT_STATE * context_p)
{
  context_p->digest_a = HASH_DIGEST_A_REG;
  context_p->digest_b = HASH_DIGEST_B_REG;
  context_p->digest_c = HASH_DIGEST_C_REG;
  context_p->digest_d = HASH_DIGEST_D_REG;
  context_p->digest_e = HASH_DIGEST_E_REG;
  context_p->digcnt   = HASH_DIGCNT_REG;
  context_saved       = TRUE;
}

static void hw_process_aligned_block (T_HASH_CLIENT_STATE * context_p,
                                      UINT8 * data8_p, UINT32 length,
                                      BOOL close)
{
  UINT32 *data32_p;
  UINT32 i;

  if (!context_p->force_init && context_saved)
    {
      restore_context (context_p);
    }
  else
    {
      HASH_DIGCNT_REG = context_p->digcnt;
    }

  HASH_CTRL_REG = (context_p->hash_type == HASH_TYPE_SHA1 ? HASH_CTRL_SHA1 : 0) /* algo SHA1? */
    | (context_p->force_init ? HASH_CTRL_BEGIN : 0)     /* algo_constant */
    | (close ? HASH_CTRL_CLOSE : 0)     /* close hash */
    | (length << HASH_CTRL_LENGTH_SHL); /* length */

   
  data32_p = (UINT32 *) data8_p;
  hash_poll_input_ready ();

  for (i = 0; i < HASH_HW_FIFOSIZE / sizeof (UINT32); i++)
    {
      HASH_DIN_ARRAY32[i] = data32_p[i];
    }
  context_p->force_init = FALSE;
  context_saved = FALSE;
  context_p->rest_cnt = 0;
  hash_poll_output_ready ();
  context_p->digcnt = HASH_DIGCNT_REG;
}


static void hw_process_aligned_block_DMG (T_HASH_CLIENT_STATE * context_p,
                                          UINT8 * data8_p, UINT32 length,
                                          BOOL close)
{
  UINT32 *data32_p;
  UINT32 i;
  
  if (!context_p->force_init && context_saved)
    {
      restore_context (context_p);
    }
  else
    {
      HASH_DIGCNT_REG = context_p->digcnt;
    }

  HASH_CTRL_REG = (context_p->hash_type == HASH_TYPE_SHA1 ? HASH_CTRL_SHA1 : 0) /* algo SHA1? */
    | (context_p->force_init ? HASH_CTRL_BEGIN : 0)     /* algo_constant */
    | (close ? HASH_CTRL_CLOSE : 0)     /* close hash */
    | (length << HASH_CTRL_LENGTH_SHL); /* length */
   
  /* wait for dmg_completed */
  hash_dmg_wait_status_OK ();

  context_p->force_init = FALSE;
  context_saved = FALSE;
  context_p->rest_cnt = 0;
  context_p->digcnt = HASH_DIGCNT_REG;

}

void hash_hw_process_byte_string (T_HASH_CLIENT_STATE * context_p,
                                  UINT8 * data_p, UINT32 length)
{
  UINT8 *aligned_buf_p = (UINT8 *) context_p->data_rest;
  UINT32 i = 0, j;

  context_saved = TRUE;
  /* partial block from last time ? */
  if (context_p->rest_cnt)
    {
      if (context_p->rest_cnt + length >= HASH_HW_FIFOSIZE)
        {
          /* complete partial block & hash it */
          UINT32 filler_size = HASH_HW_FIFOSIZE - context_p->rest_cnt;
          memcpy (aligned_buf_p + context_p->rest_cnt, data_p, filler_size);
          hw_process_aligned_block (context_p, aligned_buf_p,
                                    HASH_HW_FIFOSIZE, context_p->last_block
                                    && filler_size == length);
          context_p->rest_cnt = 0;
          i = filler_size;
        }
      else
        {
          /* not enough data_p to fill block */
          memcpy (aligned_buf_p + context_p->rest_cnt, data_p, length);
          context_p->rest_cnt += length;
          i = length;
          if (context_p->last_block)
            {
              hw_process_aligned_block (context_p, aligned_buf_p,
                                        context_p->rest_cnt, LAST_BLOCK);
            }
        }
    }
  /* pass entire blocks, avoid memcpy if aligned */
  if (ptr_is_aligned (data_p + i))
    {
      for (; i + HASH_HW_FIFOSIZE <= length; i += HASH_HW_FIFOSIZE)
        {
          hw_process_aligned_block (context_p, data_p + i,
                                    HASH_HW_FIFOSIZE, context_p->last_block
                                    && i + HASH_HW_FIFOSIZE == length);
        }
    }
  else
    {
      for (; i + HASH_HW_FIFOSIZE <= length; i += HASH_HW_FIFOSIZE)
        {
          memcpy (aligned_buf_p, data_p + i, HASH_HW_FIFOSIZE);
          hw_process_aligned_block (context_p, aligned_buf_p,
                                    HASH_HW_FIFOSIZE, context_p->last_block
                                    && i + HASH_HW_FIFOSIZE == length);
        }
    }
  /* handle last partial block, if any */
  context_p->rest_cnt = length - i;
  for (j = 0; j < context_p->rest_cnt; j++)
    {
      aligned_buf_p[j] = data_p[i + j];
    }
  if (context_p->rest_cnt && context_p->last_block)
    {
      hw_process_aligned_block (context_p, aligned_buf_p, 
                                context_p->rest_cnt, LAST_BLOCK);
    }

  if (!context_saved)
    {
      save_context (context_p);
    }
}


T_RV_RET hash_hw_process_byte_string_DMG (T_HASH_CLIENT_STATE * context_p,
                                      UINT8 * data_p, UINT32 length)
{
#if (HASH_USE_DMG==1)
  UINT8 *aligned_buf_p = (UINT8 *) context_p->data_rest;
  UINT32 i = 0, j;
  T_DMG_CHANNEL dmg_channel;
  T_DMG_CHANNEL_PARAMETERS dmg_channel_parameters;
  UINT32 dmg_length=0;

  T_DMG_REQUEST_DATA dmg_request_data;
  T_DMG_CHANNEL_DATA dmg_channel_data;

  context_saved = TRUE;
  /* partial block from last time ? */
  if (context_p->rest_cnt)
    {
      if (context_p->rest_cnt + length >= HASH_HW_FIFOSIZE)
        {
          /* complete partial block & hash it */
          UINT32 filler_size = HASH_HW_FIFOSIZE - context_p->rest_cnt;
          memcpy (aligned_buf_p + context_p->rest_cnt, data_p, filler_size);
          hw_process_aligned_block (context_p, aligned_buf_p,
                                    HASH_HW_FIFOSIZE, context_p->last_block
                                    && filler_size == length);
          context_p->rest_cnt = 0;
          i = filler_size;
        }
      else
        {
          /* not enough data_p to fill block */
          memcpy (aligned_buf_p + context_p->rest_cnt, data_p, length);
          context_p->rest_cnt += length;
          i = length;
          if (context_p->last_block)
            {
              hw_process_aligned_block (context_p, aligned_buf_p,
                                        context_p->rest_cnt, LAST_BLOCK);
            }
        }
    }

    /* Reserve DMG channel */
    /* setup dmg channel parameters */
    dmg_channel_data.lock           = DMG_CHANNEL_LOCKED;
    dmg_channel_data.channel        = DMG_ANY_CHANNEL;
    dmg_request_data.dmg_peripheral = DMG_PERIPHERAL_SHA1_TX;
    dmg_request_data.dmg_queued     = DMG_QUEUE_DISABLE;
    /*if (dma_reserve_channel (DMA_CHAN_ANY,0, DMA_QUEUE_DISABLE, 100,
         hash_env_ctrl_blk_p->path_to_return_queue) != RV_OK)*/

    if (RV_OK != dmg_reserve_channel (&dmg_channel_data, &dmg_request_data,
                      hash_env_ctrl_blk_p->path_to_return_queue))
    {
      HASH_SEND_TRACE ("HASH dmg_reserve_channel failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
    }
    else
    {
      /* get the channel number */
      dmg_channel = hash_dmg_wait_status_OK ();
      if (dmg_channel == DMG_MAX_CHANNEL+1) return RV_INTERNAL_ERR;
    }    

    /* pass entire blocks, avoid memcpy if aligned */
    if (ptr_is_aligned (data_p + i))
    { 
      /* Setup parameters */
      HASH_MASK_REG = HASH_DMA_ENABLE | HASH_AUTO_IDLE_ENABLE; // enable DMA in the HASH register

      /* set dmg parameters */
      dmg_channel_parameters.data_width = DMG_DATA_S32;
      dmg_channel_parameters.sync = DMG_PERIPHERAL_SHA1_TX;
      dmg_channel_parameters.hw_priority = DMG_HW_PRIORITY_LOW;
      dmg_channel_parameters.flush = DMG_FLUSH_DISABLED;
      dmg_channel_parameters.nmb_frames = (length -i)/ HASH_HW_FIFOSIZE;
      dmg_channel_parameters.nmb_elements = HASH_HW_FIFOSIZE/(1<<DMG_DATA_S32);

      dmg_length = dmg_channel_parameters.nmb_frames *
                   dmg_channel_parameters.nmb_elements * 
                  (1<<dmg_channel_parameters.data_width);

      dmg_channel_parameters.dmg_end_notification = DMG_NOTIFICATION;
      dmg_channel_parameters.secure = DMG_SECURED;
      dmg_channel_parameters.repeat = DMG_SINGLE;

      dmg_channel_parameters.source_address = (UINT32) data_p+i; 
      dmg_channel_parameters.source_address_mode = DMG_ADDR_MODE_POST_INC;
      dmg_channel_parameters.source_packet = DMG_NOT_PACKED;
      dmg_channel_parameters.source_burst = DMG_NO_BURST;

      dmg_channel_parameters.destination_address = (UINT32) HASH_DIN_ARRAY32;
      dmg_channel_parameters.destination_address_mode = DMG_ADDR_MODE_FRAME_INDEX;
      dmg_channel_parameters.destination_packet = DMG_NOT_PACKED;
      dmg_channel_parameters.destination_burst = DMG_NO_BURST;
      dmg_channel_parameters.double_buf_source_address      = 0;
      dmg_channel_parameters.double_buf_destination_address = 0;

      if (RV_OK != dmg_set_channel_parameters (dmg_channel,
                           &dmg_channel_parameters, 
                           hash_env_ctrl_blk_p->path_to_return_queue ))
      {
        HASH_SEND_TRACE ("HASH dmg_set_channel_parameters failed",
                RV_TRACE_LEVEL_ERROR);
        return RV_INTERNAL_ERR;      
      }
      else
      {
        /* wait for dmg_param_set */
        if ((DMG_MAX_CHANNEL+1)==hash_dmg_wait_status_OK ()) return RV_INTERNAL_ERR;
       
        hw_process_aligned_block_DMG (context_p, data_p + i,
                                  dmg_length, context_p->last_block
                                  && dmg_length == length-i);
        
        HASH_MASK_REG = HASH_DMA_DISABLE | HASH_AUTO_IDLE_ENABLE; /* disable DMA in the HASH register */
      }
    }
    else
    {
      for (; i + HASH_HW_FIFOSIZE <= length; i += HASH_HW_FIFOSIZE)
      {
        memcpy (aligned_buf_p, data_p + i, HASH_HW_FIFOSIZE);
        hw_process_aligned_block (context_p, aligned_buf_p,
                                  HASH_HW_FIFOSIZE, context_p->last_block
                                  && i + HASH_HW_FIFOSIZE == length);
      }
    }


  if (RV_OK != dmg_release_channel (dmg_channel, 
               hash_env_ctrl_blk_p->path_to_return_queue))
  {
    HASH_SEND_TRACE ("HASH release dmg channel failed",
            RV_TRACE_LEVEL_ERROR);

    return RV_INTERNAL_ERR;
  }
  if ((DMG_MAX_CHANNEL+1)==hash_dmg_wait_status_OK ())
  {
    return RV_INTERNAL_ERR;
  }


  /* handle last partial block, if any */
  context_p->rest_cnt = length - (i+dmg_length);
  for (j = 0; j < context_p->rest_cnt; j++)
  {
    aligned_buf_p[j] = data_p[i + dmg_length + j];
  }
  if (context_p->rest_cnt && context_p->last_block)
  {
    hw_process_aligned_block (context_p, aligned_buf_p, context_p->rest_cnt,
                              LAST_BLOCK);
  }

  if (!context_saved)
  {
    save_context (context_p);
  }
#else

 UINT8 *aligned_buf_p = (UINT8 *) context_p->data_rest;
  UINT32 i = 0, j;
  T_DMA_CHANNEL dma_channel;
  T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;
  UINT32 dma_length=0;

  context_saved = TRUE;
  /* partial block from last time ? */
  if (context_p->rest_cnt)
    {
      if (context_p->rest_cnt + length >= HASH_HW_FIFOSIZE)
        {
          /* complete partial block & hash it */
          UINT32 filler_size = HASH_HW_FIFOSIZE - context_p->rest_cnt;
          memcpy (aligned_buf_p + context_p->rest_cnt, data_p, filler_size);
          hw_process_aligned_block (context_p, aligned_buf_p,
                                    HASH_HW_FIFOSIZE, context_p->last_block
                                    && filler_size == length);
          context_p->rest_cnt = 0;
          i = filler_size;
        }
      else
        {
          /* not enough data_p to fill block */
          memcpy (aligned_buf_p + context_p->rest_cnt, data_p, length);
          context_p->rest_cnt += length;
          i = length;
          if (context_p->last_block)
            {
              hw_process_aligned_block (context_p, aligned_buf_p,
                                        context_p->rest_cnt, LAST_BLOCK);
            }
        }
    }

    if (RV_OK != dma_reserve_channel (DMA_CHAN_ANY,0, DMA_QUEUE_DISABLE, 10,
         hash_env_ctrl_blk_p->path_to_return_queue) )
    {
      HASH_SEND_TRACE ("HASH dma_reserve_channel failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
    }
    else
    {
      /* get the channel number */
      dma_channel = hash_dmg_wait_status_OK ();
      if (dma_channel == DMA_MAX_CHANNEL+1) return RV_INTERNAL_ERR;
    }    

    /* pass entire blocks, avoid memcpy if aligned */
    if (ptr_is_aligned (data_p + i))
    { 
      /* Setup parameters */
      HASH_MASK_REG = HASH_DMA_ENABLE | HASH_AUTO_IDLE_ENABLE; // enable DMA in the HASH register

      /* set dma parameters */
      dma_channel_parameters.data_width = DMA_DATA_S32;
      dma_channel_parameters.sync = DMA_SYNC_DEVICE_SHA1_TX;
      dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_LOW;
      dma_channel_parameters.flush = DMA_FLUSH_DISABLED;
      dma_channel_parameters.nmb_frames = (length -i)/ HASH_HW_FIFOSIZE;
      dma_channel_parameters.nmb_elements = HASH_HW_FIFOSIZE/(1<<DMA_DATA_S32);

      dma_length = dma_channel_parameters.nmb_frames *
                   dma_channel_parameters.nmb_elements * 
                  (1<<dma_channel_parameters.data_width);

      dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
      dma_channel_parameters.secure = DMA_SECURED;
      dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;
      dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;

      dma_channel_parameters.source_address = (UINT32) data_p+i; 
      dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_POST_INC;
      dma_channel_parameters.source_packet = DMA_NOT_PACKED;
      dma_channel_parameters.source_burst = DMA_NO_BURST;

      dma_channel_parameters.destination_address = (UINT32) HASH_DIN_ARRAY32;
      dma_channel_parameters.destination_address_mode = DMA_ADDR_MODE_FRAME_INDEX;
      dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
      dma_channel_parameters.destination_burst = DMA_NO_BURST;


      if (RV_OK != dma_set_channel_parameters (dma_channel,
                           &dma_channel_parameters))
      {
        HASH_SEND_TRACE ("HASH dma_set_channel_parameters failed",
                RV_TRACE_LEVEL_ERROR);
        return RV_INTERNAL_ERR;      
      }
      else
      {
        /* wait for dma_param_set */
        if ((DMA_MAX_CHANNEL+1)==hash_dmg_wait_status_OK ()) return RV_INTERNAL_ERR;
       
        hw_process_aligned_block_DMG (context_p, data_p + i,
                                  dma_length, context_p->last_block
                                  && dma_length == length-i);
        
        HASH_MASK_REG = HASH_DMA_DISABLE | HASH_AUTO_IDLE_ENABLE; /* disable DMA in the HASH register */
      }
    }
    else
    {
      for (; i + HASH_HW_FIFOSIZE <= length; i += HASH_HW_FIFOSIZE)
      {
        memcpy (aligned_buf_p, data_p + i, HASH_HW_FIFOSIZE);
        hw_process_aligned_block (context_p, aligned_buf_p,
                                  HASH_HW_FIFOSIZE, context_p->last_block
                                  && i + HASH_HW_FIFOSIZE == length);
      }
    }


  if (RV_OK != dma_release_channel (dma_channel))
  {
    HASH_SEND_TRACE ("HASH release dma channel failed",
            RV_TRACE_LEVEL_ERROR);

    return RV_INTERNAL_ERR;
  }
  if ((DMA_MAX_CHANNEL+1)==hash_dmg_wait_status_OK ())
  {
    return RV_INTERNAL_ERR;
  }


  /* handle last partial block, if any */
  context_p->rest_cnt = length - (i+dma_length);
  for (j = 0; j < context_p->rest_cnt; j++)
  {
    aligned_buf_p[j] = data_p[i + dma_length + j];
  }
  if (context_p->rest_cnt && context_p->last_block)
  {
    hw_process_aligned_block (context_p, aligned_buf_p, context_p->rest_cnt,
                              LAST_BLOCK);
  }

  if (!context_saved)
  {
    save_context (context_p);
  }
#endif

  return RV_OK;
}


/**
 * function: hash_poll_input_ready
 */
static void hash_poll_input_ready (void)
{
  const int max = 10000;
  int j;
  for (j = 0; j < max && !(HASH_CTRL_REG & 0x02); j++);
  if (j == max)
    {
      HASH_SEND_TRACE ("HASH CORE poll_input_ready timeout",
                       RV_TRACE_LEVEL_ERROR);
    }
}

/**
 * function: hash_poll_output_ready
 */
static void hash_poll_output_ready (void)
{
  const int max = 10000;
  int j;
  for (j = 0; j < max && !(HASH_CTRL_REG & 0x01); j++);
  if (j == max)
    {
      HASH_SEND_TRACE ("HASH CORE poll_output_ready timeout",
                       RV_TRACE_LEVEL_ERROR);
    }
}

#if (HASH_USE_DMG==1)
static T_DMG_CHANNEL hash_dmg_wait_status_OK (void)
#else
static T_DMA_CHANNEL hash_dmg_wait_status_OK (void)
#endif
{
#if (HASH_USE_DMG==1)  
  T_DMG_STATUS_RSP_MSG *dmg_status_rsp_msg = hash_return_queue_get_msg ();
  T_DMG_CHANNEL ret_val = DMG_MAX_CHANNEL+1;   /* default illegal value */
#else
  T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg = hash_return_queue_get_msg ();
  T_DMA_CHANNEL ret_val = DMA_MAX_CHANNEL+1;   /* default illegal value */
#endif

#if (HASH_USE_DMG==1)
  switch (dmg_status_rsp_msg->result.status)
  {
  case DMG_RESERVE_OK:
#else
  switch (dma_status_rsp_msg->result.status)
  {
  case DMA_RESERVE_OK:
#endif
#if 0
    HASH_SEND_TRACE_PARAM ("HASH DMA received channel ",
              dma_status_rsp_msg->result.channel,
              RV_TRACE_LEVEL_DEBUG_HIGH);
#endif
    /* fall through */
#if (HASH_USE_DMG==1)
  case DMG_OK:
  case DMG_PARAM_SET:
  case DMG_COMPLETED:
  case DMG_CHANNEL_RELEASED:
  case DMG_CHANNEL_ENABLED:
    ret_val = dmg_status_rsp_msg->result.channel;
#else
  case DMA_OK:
  case DMA_PARAM_SET:
  case DMA_COMPLETED:
  case DMA_CHANNEL_RELEASED:
  case DMA_CHANNEL_ENABLED:
    ret_val = dma_status_rsp_msg->result.channel;
#endif
    /* status OK: do nothing */
    break;

  default:
#if (HASH_USE_DMG==1)
    HASH_SEND_TRACE_PARAM ("HASH DMG error status received: ",
              dmg_status_rsp_msg->result.status,
#else
    HASH_SEND_TRACE_PARAM ("HASH DMA error status received: ",
              dma_status_rsp_msg->result.status,
#endif
              RV_TRACE_LEVEL_ERROR);
  }             /* end switch */

#if (HASH_USE_DMG==1)
  if (RVF_OK != rvf_free_buf (dmg_status_rsp_msg))
#else
  if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
#endif
  {
#if (HASH_USE_DMG==1)
    HASH_SEND_TRACE ("HASH DMG could not free DMG status msg",
#else
    HASH_SEND_TRACE ("HASH DMA could not free DMA status msg",
#endif
            RV_TRACE_LEVEL_ERROR);
  }
  return ret_val;
}
