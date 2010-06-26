/**
 * @file  dma_handle_message.c
 *
 * DMA handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
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

#include "rv/rv_general.h"

#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "dma/dma_env.h"
#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "dma/dma_i.h"

#ifdef _WINDOWS
#include "dma/pc/dma_pc_i.h"
#endif //_WINDOWS

extern T_DMA_CHANNEL_ARRAY dma_channel_array [DMA_MAX_NUMBER_OF_CHANNEL]; 
extern T_DMA_QUEUE_ARRAY   dma_queue_array   [DMA_MAX_QUEUED_REQUESTS];
extern UINT8 dma_function_status [DMA_MAX_NUMBER_OF_CHANNEL];

/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param msg_p Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN dma_handle_message (T_RV_HDR *msg_p)
{
  T_DMA_RET     dma_status = DMA_RESERVE_OK;
  //UINT8         dma_return_path_set = DMA_ZERO;

  extern T_DMA_ENV_CTRL_BLK	*dma_env_ctrl_blk_p;

  /* Check if initialization has been correctly done */
  if (dma_env_ctrl_blk_p == 0)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", DMA_TRACE_LEVEL);
    return RV_INTERNAL_ERR;
  }
  else
  {  
    switch (msg_p->msg_id)
    {
      case DMA_RESERVE_CHANNEL_REQ_MSG:
      {
        T_DMA_RESERVE_CHANNEL_REQ_MSG* dma_reserve_channel_req_msg_p = 
                                    (T_DMA_RESERVE_CHANNEL_REQ_MSG*) msg_p;
        


        /* Check if parameters have a valid value */
        if ((dma_queue_spec_prio_check 
                                   (dma_reserve_channel_req_msg_p->queue, 
                                    dma_reserve_channel_req_msg_p->specific, 
                                    dma_reserve_channel_req_msg_p->sw_priority)
                                 != DMA_PARAMS_OK) ||
             ((dma_reserve_channel_req_msg_p->specific==DMA_CHAN_SPECIFIC)&&
             (dma_channel_check(dma_reserve_channel_req_msg_p->channel) != 
              DMA_PARAMS_OK)))
        {
          /* 
            send a message back to the client informing that 
            the parameter was not correct
          */
          dma_send_status_message(dma_reserve_channel_req_msg_p->channel,
                                  DMA_INVALID_PARAMETER,
                                  dma_reserve_channel_req_msg_p->return_path);
        }
        else
        {
          dma_internal_reserve_chan (msg_p);
        }
      }
      break;

      case DMA_REMOVE_FROM_QUEUE_REQ_MSG:
      {    
        dma_internal_remove_from_queue (msg_p);
      }
      break;
    
      case DMA_SET_CHANNEL_PARAMETERS_REQ_MSG:
      {
        T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG *dma_set_channel_parameters_req_msg_p = 
                                   (T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG*) msg_p;


        /* Check if the channel is already reserved */
        if (dma_function_status[dma_set_channel_parameters_req_msg_p->channel] >= 
                                                               DMA_FUNCTION_RESERVED)
        {
          /* Check the given parameters */
          if (dma_channel_check(
                     dma_set_channel_parameters_req_msg_p->channel) == 
                     DMA_PARAMS_OK) 
          {
            if (dma_check_channel_info(
                     &dma_set_channel_parameters_req_msg_p->channel_info) 
                     != DMA_PARAMS_OK)
            {
              dma_send_status_message 
                                  (dma_set_channel_parameters_req_msg_p->channel,
                                   DMA_INVALID_PARAMETER, 
                                   dma_channel_array[dma_set_channel_parameters_req_msg_p->
                                   channel].return_path);

              DMA_SEND_TRACE ("DMA Illegal channel information used",\
                               DMA_TRACE_LEVEL);
            }
            else
            {
               dma_internal_set_chan_params (msg_p);
            }
         }
        }
        else
        {
        /* 
          when a wrong sequence of API calls is done, no message is send back! 
          There is no return path known.
        */
          DMA_SEND_TRACE ("DMA wrong sequence of API calls",\
                           DMA_TRACE_LEVEL);
        }
      }
      break;

      case DMA_ENABLE_TRANSFER_REQ_MSG:
      {
        /* Create a pointer to the message information */
        T_DMA_ENABLE_TRANSFER_REQ_MSG* dma_enable_transfer_req_p = 
                                        (T_DMA_ENABLE_TRANSFER_REQ_MSG*) msg_p;


        /* Check if channel is used (and thus has been assigned) */
        if (dma_channel_array[dma_enable_transfer_req_p->channel].specific != 
                                                         DMA_CHANNEL_POS_FREE)
        {
          /* check if parameters have been set up */
          if (dma_function_status[dma_enable_transfer_req_p->channel] >= 
                                  DMA_FUNCTION_PARAMS_SET_UP)
          {       
            if (dma_channel_check (dma_enable_transfer_req_p->channel) != 
                                   DMA_INVALID_PARAMETER)
            /* when an illegal channel number is provided, 
               no message is send back ! */
            {  
              DMA_SEND_TRACE_PARAM ("DMA Transfer started channel ",
                  dma_enable_transfer_req_p->channel, DMA_TRACE_LEVEL);
              dma_status = DMA_CHANNEL_ENABLED;
              dma_send_status_message (dma_enable_transfer_req_p->channel, 
                                       dma_status,
                                       dma_channel_array[dma_enable_transfer_req_p->
                                       channel].return_path);
              f_dma_channel_enable (dma_enable_transfer_req_p->channel);
            }
          }
          else
          {
            DMA_SEND_TRACE("DMA Illegal sequence of API call", 
                            DMA_TRACE_LEVEL);
            /* no message send back, as there is no known return path */
          }
        }
      }
      break;

      case DMA_RELEASE_CHANNEL_REQ_MSG:
      {
        /* Create a pointer to the message information */
        T_DMA_RELEASE_CHANNEL_REQ_MSG* dma_release_channel_req_p = (T_DMA_RELEASE_CHANNEL_REQ_MSG*) msg_p;


        if (dma_channel_check (dma_release_channel_req_p->channel) != 
                               DMA_INVALID_PARAMETER)
        /* when an illegal channel number is provided, 
           no message is send back ! */
        {  
          dma_internal_rel_chan (dma_release_channel_req_p->channel);
        }
      }
      break;

      default:
      {
        /* Unknow message has been received */
        DMA_SEND_TRACE("DMA Message received unknown",DMA_TRACE_LEVEL);
      } 
    }
  }


  /* Free message */
  if (rvf_free_buf(msg_p) != RVF_OK)
  {
    DMA_SEND_TRACE("DMA Unable to free message",
            DMA_TRACE_LEVEL);
    return RVM_MEMORY_ERR;
  }

  return RVM_OK;
}
