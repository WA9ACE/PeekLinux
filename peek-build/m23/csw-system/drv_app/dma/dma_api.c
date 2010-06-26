/**
 * @file  dma_api.c
 *
 * API for DMA SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/2/2003   ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "dma/dma_state_i.h"
#include "dma/dma_i.h"
 
extern T_DMA_ENV_CTRL_BLK* dma_env_ctrl_blk_p;

T_RV_RET dma_reserve_channel (T_DMA_SPECIFIC specific,
                              T_DMA_CHANNEL  channel,
                              T_DMA_QUEUE    queue,
                              T_DMA_SW_PRIORITY sw_priority,                              
                              T_RV_RETURN    return_path)
{
  T_DMA_RESERVE_CHANNEL_REQ_MSG *dma_reserve_channel_req_msg_p;

  T_DMA_RET dma_status       = DMA_RESERVE_OK;
  T_RV_RET  dma_return_value = RV_OK;
  T_RVF_MB_STATUS mb_status;

  /* Check if initialization has been correctly done */
  if (dma_env_ctrl_blk_p == 0)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", DMA_TRACE_LEVEL);
    dma_status       = DMA_NOT_READY;
    dma_return_value = RV_NOT_READY;
  }
  else
  {
    /* Check if parameters have a valid value */
    if ((dma_queue_spec_prio_check (queue, specific, sw_priority) != DMA_PARAMS_OK) ||
       ((specific == DMA_CHAN_SPECIFIC) && (dma_channel_check(channel) != DMA_PARAMS_OK)))
    {
      dma_status = DMA_INVALID_PARAMETER;
      /* 
        message sned back to the client informing that 
        the parameter was not correct is send below
      */
    }
    else
    {
      /* Send a message to the DMA message handler */

      /* Notify the subscriber */
      mb_status = rvf_get_msg_buf (dma_env_ctrl_blk_p->prim_mb_id, 
                                   sizeof(T_DMA_RESERVE_CHANNEL_REQ_MSG), 
                                   DMA_RESERVE_CHANNEL_REQ_MSG,
                                  (T_RV_HDR **) &dma_reserve_channel_req_msg_p);   
    
      if (mb_status != RVF_RED) /* Memory allocation success */
      {
        /* Fill the message */
        dma_reserve_channel_req_msg_p->hdr.msg_id  = DMA_RESERVE_CHANNEL_REQ_MSG; 
        dma_reserve_channel_req_msg_p->specific    = specific;
        dma_reserve_channel_req_msg_p->channel     = channel;
        dma_reserve_channel_req_msg_p->queue       = queue;
        dma_reserve_channel_req_msg_p->sw_priority = sw_priority;
        dma_reserve_channel_req_msg_p->return_path = return_path;
 
        rvf_send_msg(dma_env_ctrl_blk_p->addr_id, dma_reserve_channel_req_msg_p);
        dma_return_value = RV_OK;

      }
      else
      {
        DMA_SEND_TRACE("DMA Memory allocation error reserve channel", DMA_TRACE_LEVEL);
        dma_status       = DMA_MEMORY_ERROR;
        dma_return_value = RV_MEMORY_ERR;
      }
    }
  }

  /* If something went wrong then also send a message back to the client */
  if (dma_status != DMA_RESERVE_OK)
  {
    dma_send_status_message(channel, dma_status, return_path);
  }

  return dma_return_value;
}


T_RV_RET dma_remove_from_queue (T_DMA_CHANNEL channel_queue_id)
{
  T_DMA_REMOVE_FROM_QUEUE_REQ_MSG *dma_remove_from_queue_req_msg_p;
  T_RVF_MB_STATUS mb_status;

  if (dma_env_ctrl_blk_p == 0)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", DMA_TRACE_LEVEL);
    return RV_NOT_READY;
  }
  else
  {    
    /* Send a message to the DMA message handler */

    /* Notify the subscriber */
    mb_status = rvf_get_msg_buf (dma_env_ctrl_blk_p->prim_mb_id, 
                                 sizeof(T_DMA_REMOVE_FROM_QUEUE_REQ_MSG), 
                                 DMA_REMOVE_FROM_QUEUE_REQ_MSG,
                               (T_RV_HDR **) &dma_remove_from_queue_req_msg_p);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
      /* Fill the message */
      dma_remove_from_queue_req_msg_p->hdr.msg_id = DMA_REMOVE_FROM_QUEUE_REQ_MSG; 
      dma_remove_from_queue_req_msg_p->channel_queue_id = channel_queue_id;

 
      rvf_send_msg(dma_env_ctrl_blk_p->addr_id, dma_remove_from_queue_req_msg_p);
      
    }
  }
  return RV_OK;
}


T_RV_RET dma_set_channel_parameters (T_DMA_CHANNEL channel,
                                      T_DMA_CHANNEL_PARAMETERS *channel_info_p)
{
  T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG *dma_set_channel_parameters_req_msg_p;
  T_RV_RET dma_return_value = RV_OK;
  T_RVF_MB_STATUS mb_status;

  /* Check if initialization has been correctly done */
  if (dma_env_ctrl_blk_p == 0)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", DMA_TRACE_LEVEL);
    dma_return_value = RV_INTERNAL_ERR;
  }
  else
  {

    /* Send a message to the handler */

    /* Notify the subscriber */
    mb_status = rvf_get_msg_buf (dma_env_ctrl_blk_p->prim_mb_id, 
                                 sizeof(T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG),
                                 DMA_SET_CHANNEL_PARAMETERS_REQ_MSG,
                          (T_RV_HDR **) &dma_set_channel_parameters_req_msg_p);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
      /* Fill the message */
      dma_set_channel_parameters_req_msg_p->channel      = channel;
      dma_set_channel_parameters_req_msg_p->channel_info = *channel_info_p;
      dma_set_channel_parameters_req_msg_p->hdr.msg_id   = 
                                        DMA_SET_CHANNEL_PARAMETERS_REQ_MSG; 


      rvf_send_msg(dma_env_ctrl_blk_p->addr_id, 
                   dma_set_channel_parameters_req_msg_p);
  
      dma_return_value = RV_OK;
    }
    else
    {
      DMA_SEND_TRACE("DMA Memory allocation error channel parameters", DMA_TRACE_LEVEL);
      dma_return_value = RV_MEMORY_ERR;
    }
  }

  return dma_return_value;
}


T_RV_RET dma_enable_transfer (T_DMA_CHANNEL channel)
{
  T_DMA_ENABLE_TRANSFER_REQ_MSG *dma_enable_transfer_req_msg_p;
  T_RV_RET dma_return_value = RV_NOT_READY;
  T_RVF_MB_STATUS mb_status;

  /* Check if initialization has been correctly done */
  if (dma_env_ctrl_blk_p == 0)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", DMA_TRACE_LEVEL);
    dma_return_value = RV_INTERNAL_ERR;
  }
  else
  {

    /* Send a message to the handler */

    /* Notify the subscriber */
    mb_status = rvf_get_msg_buf (dma_env_ctrl_blk_p->prim_mb_id, 
                                 sizeof(T_DMA_ENABLE_TRANSFER_REQ_MSG),
                                 DMA_ENABLE_TRANSFER_REQ_MSG,
                                (T_RV_HDR **) &dma_enable_transfer_req_msg_p);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
      /* Fill the message */
      dma_enable_transfer_req_msg_p->hdr.msg_id = DMA_ENABLE_TRANSFER_REQ_MSG; 
      dma_enable_transfer_req_msg_p->channel    = channel;


      rvf_send_msg(dma_env_ctrl_blk_p->addr_id, dma_enable_transfer_req_msg_p);

      dma_return_value = RV_OK;
    }
    else
    {
      DMA_SEND_TRACE("DMA Memory allocation error enable transfer", DMA_TRACE_LEVEL);
      dma_return_value = RV_MEMORY_ERR;
    }
  }
  return dma_return_value;
}


T_RV_RET dma_release_channel (T_DMA_CHANNEL channel)
{
  T_DMA_RELEASE_CHANNEL_REQ_MSG *dma_release_channel_req_msg_p;
  T_RV_RET dma_return_value = RV_OK;
  T_RVF_MB_STATUS mb_status;

  /* Check if initialization has been correctly done */
  if (dma_env_ctrl_blk_p == 0)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", 
                   DMA_TRACE_LEVEL);
    dma_return_value = RV_INTERNAL_ERR;
  }
  else
  {

    /*
      This function is called when a channel can be released. It will free the 
      channel, and gets the next request from the queue. If the queue is empty
      this function finishes as well.
    */

    /* Send a message to the handler */
    /* Notify the subscriber */
    mb_status = rvf_get_msg_buf (dma_env_ctrl_blk_p->prim_mb_id, 
                                 sizeof(T_DMA_RELEASE_CHANNEL_REQ_MSG),
                                 DMA_RELEASE_CHANNEL_REQ_MSG,
                                (T_RV_HDR **) &dma_release_channel_req_msg_p);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
      /* Fill the message */
      dma_release_channel_req_msg_p->hdr.msg_id = DMA_RELEASE_CHANNEL_REQ_MSG; 
      dma_release_channel_req_msg_p->channel    = channel;


      rvf_send_msg(dma_env_ctrl_blk_p->addr_id, dma_release_channel_req_msg_p);
      dma_return_value = RV_OK;
    }
    else
    {
      DMA_SEND_TRACE("DMA Memory allocation error release channel", DMA_TRACE_LEVEL);
      dma_return_value = RV_MEMORY_ERR;
    }
  }
  return dma_return_value;
}


UINT32 dma_get_sw_version(void)
{
  DMA_SEND_TRACE("DMA Get dma sw version", DMA_TRACE_LEVEL);
  
  return BUILD_VERSION_NUMBER(DMA_MAJOR,DMA_MINOR,DMA_BUILD);
}

