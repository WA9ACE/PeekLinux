/**
 * @file	hash_handle_message.c
 *
 * SHA handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "hash/hash_i.h"
#include "hash/hash_hw_functions.h"



/**
 * function: hash_send_response_to_client
 */
static T_RVM_RETURN hash_send_response_to_client (UINT32 msg_id,
                                                  T_HASH_MSG * msg_p,
                                                  T_RV_RETURN rp)
{
  /* follow return path: callback or mailbox */
  msg_p->os_hdr.msg_id = msg_id;
  if (rp.callback_func != NULL)
    {
//      HASH_SEND_TRACE ("HASH CORE callback", RV_TRACE_LEVEL_DEBUG_HIGH);
      rp.callback_func (msg_p);
      if (rvf_free_msg ((T_RV_HDR *) msg_p) != RVF_OK)
        {
          HASH_SEND_TRACE ("HASH CORE could not free response msg",
                           RV_TRACE_LEVEL_WARNING);
          return RVM_MEMORY_ERR;
        }
    }
  else
    {
//      HASH_SEND_TRACE ("HASH CORE posting response", RV_TRACE_LEVEL_DEBUG_HIGH);
      if (rvf_send_msg (rp.addr_id, msg_p) != RVF_OK)
        {
          HASH_SEND_TRACE ("HASH CORE could not send response msg",
                           RV_TRACE_LEVEL_WARNING);
          return RVM_MEMORY_ERR;
        };
    }
  return RVM_OK;
}

/* store message in return queue, raise an event */
void hash_return_queue_store_msg (void *msg_p)
{
  /* DEBUG: limit the number of incoming messages */
  if (hash_env_ctrl_blk_p->return_queue.queue_obj.count > 3)
	{
	  rvf_free_buf (msg_p);
	  HASH_SEND_TRACE ("HASH RETURN QUEUE dropped message",
					  RV_TRACE_LEVEL_ERROR);
	  return;
	}
  if (RVF_OK !=
	  rvf_enqueue (&hash_env_ctrl_blk_p->return_queue.queue_obj, msg_p)
	  || RVF_OK != rvf_send_event (hash_env_ctrl_blk_p->return_queue.addr_id,
								   hash_env_ctrl_blk_p->return_queue.event))
	{
	  HASH_SEND_TRACE ("HASH RETURN QUEUE lost message", RV_TRACE_LEVEL_ERROR);
	}
}

/* init return queue object */
void hash_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
					   T_RV_RETURN_PATH * path_to_return_queue_p)
{
  hash_env_ctrl_blk_p->return_queue.addr_id = rq_addr_id;
  hash_env_ctrl_blk_p->return_queue.event = rq_event;
  hash_env_ctrl_blk_p->return_queue.queue_obj.p_first = NULL;
  hash_env_ctrl_blk_p->return_queue.queue_obj.p_last = NULL;
  hash_env_ctrl_blk_p->return_queue.queue_obj.count = 0;
  path_to_return_queue_p->addr_id = RVF_INVALID_ADDR_ID;
  path_to_return_queue_p->callback_func = hash_return_queue_store_msg;
}

/* get a message from the return queue */
void *hash_return_queue_get_msg (void)
{
  rvf_wait (hash_env_ctrl_blk_p->return_queue.event, 0);
  return rvf_dequeue (&hash_env_ctrl_blk_p->return_queue.queue_obj);
}


/**
 * function: hash_exec_sha1_hash
 */
T_RVM_RETURN hash_exec_sha1_hash (T_HASH_CLIENT_STATE * context_p,
                                  UINT8 * data_p, UINT32 data_size,
                                  BOOL last_block, T_HASH_MSG * rsp_msg)
{
  T_RVM_RETURN rvm_return = RVM_OK;

  //HASH_SEND_TRACE ("HASH CORE hash_exec_sha1_hash ENTER", RV_TRACE_LEVEL_DEBUG_LOW);

  /* check parameters */
  if ((context_p == NULL) || (data_size == 0)
      || (data_size > HASH_MAX_BLOCKSIZE))
    {
      /* invalid parameter */
      HASH_SEND_TRACE ("HASH CORE hash_exec_sha1_hash INVALLID PARAMETER",
                       RV_TRACE_LEVEL_DEBUG_LOW);
      if (last_block)
        {
          rsp_msg->sha1_result.return_status = RV_INVALID_PARAMETER;
          return hash_send_response_to_client (HASH_SHA1_RESULT_RSP_MSG,
                                               rsp_msg,
                                               context_p->return_path);
        }
      else
        {
          rsp_msg->sha1_req_data.return_status = RV_INVALID_PARAMETER;
          return hash_send_response_to_client (HASH_SHA1_REQ_DATA_RSP_MSG,
                                               rsp_msg,
                                               context_p->return_path);
        }
    }

  context_p->last_block = last_block;
  //rvm_return = hash_hw_exec_hash (context_p, data_p, data_size, rsp_msg);
  
    switch (HASH_DMA_MODE)
    {
#ifdef HASH_ENABLE_DMA
      case HASH_FORCE_DMA:
      case HASH_DMA_AUTO:
        if (data_size >= HASH_HW_FIFOSIZE)
          hash_hw_process_byte_string_DMG (context_p, data_p, data_size);
        else
        {
          HASH_SEND_TRACE ("HASH SHA 1 uses CPU mode because amount of data is too small for DMA",
                          RV_TRACE_LEVEL_DEBUG_LOW);
          hash_hw_process_byte_string (context_p, data_p, data_size);
        }
        break;
#endif
     
      case HASH_FORCE_CPU:
      default:
        hash_hw_process_byte_string (context_p, data_p, data_size);
        break;
    }

  rsp_msg->sha1_result.return_status = rvm_return;
  if (last_block)
    {
      rsp_msg->sha1_result.digest_a = context_p->digest_a;
      rsp_msg->sha1_result.digest_b = context_p->digest_b;
      rsp_msg->sha1_result.digest_c = context_p->digest_c;
      rsp_msg->sha1_result.digest_d = context_p->digest_d;
      rsp_msg->sha1_result.digest_e = context_p->digest_e;
      return hash_send_response_to_client (HASH_SHA1_RESULT_RSP_MSG, rsp_msg,
                                           context_p->return_path);
    }
  else
    {
      return hash_send_response_to_client (HASH_SHA1_REQ_DATA_RSP_MSG,
                                           rsp_msg, context_p->return_path);
    }
}



/**
 * function: hash_exec_md5_hash
 */
T_RVM_RETURN hash_exec_md5_hash (T_HASH_CLIENT_STATE * context_p,
                                 UINT8 * data_p, UINT32 data_size,
                                 BOOL last_block, T_HASH_MSG * rsp_msg)
{
  T_RVM_RETURN rvm_return = RVM_OK;

//  HASH_SEND_TRACE ("HASH CORE hash_exec_md5_hash ENTER",
//                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* check parameters */
  if ((context_p == NULL) || (data_size == 0)
      || (data_size > HASH_MAX_BLOCKSIZE))
    {
      /* invalid parameter */
      if (last_block)
        {
          rsp_msg->md5_result.return_status = RV_INVALID_PARAMETER;
          return hash_send_response_to_client (HASH_MD5_RESULT_RSP_MSG,
                                               rsp_msg,
                                               context_p->return_path);
        }
      else
        {
          rsp_msg->md5_result.return_status = RV_INVALID_PARAMETER;
          return hash_send_response_to_client (HASH_MD5_REQ_DATA_RSP_MSG,
                                               rsp_msg,
                                               context_p->return_path);
        }
    }

  context_p->last_block = last_block;
  //rvm_return = hash_hw_exec_hash (context_p, data_p, data_size, rsp_msg);
    switch (HASH_DMA_MODE)
    {
#ifdef HASH_ENABLE_DMA
      case HASH_FORCE_DMA:
      case HASH_DMA_AUTO:
        if (data_size >= HASH_HW_FIFOSIZE) 
          hash_hw_process_byte_string_DMG (context_p, data_p, data_size);
        else
        {
          HASH_SEND_TRACE ("HASH MD5 uses CPU mode because amount of data is too small for DMA",
                          RV_TRACE_LEVEL_DEBUG_LOW);
          hash_hw_process_byte_string (context_p, data_p, data_size);
        }
        break;
#endif
     
      case HASH_FORCE_CPU:
      default:
        hash_hw_process_byte_string (context_p, data_p, data_size);
        break;
    }

  rsp_msg->md5_result.return_status = rvm_return;
  if (last_block)
    {
      rsp_msg->md5_result.digest_a = context_p->digest_a;
      rsp_msg->md5_result.digest_b = context_p->digest_b;
      rsp_msg->md5_result.digest_c = context_p->digest_c;
      rsp_msg->md5_result.digest_d = context_p->digest_d;
      return hash_send_response_to_client (HASH_MD5_RESULT_RSP_MSG, rsp_msg,
                                           context_p->return_path);
    }
  else
    {
      return hash_send_response_to_client (HASH_MD5_REQ_DATA_RSP_MSG, rsp_msg,
                                           context_p->return_path);
    }

}




/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN hash_handle_message (T_RV_HDR * rv_msg_p)
{
  T_HASH_MSG *msg_p = (T_HASH_MSG *) rv_msg_p;
  T_RVM_RETURN ret_val = RVM_OK;
  T_HASH_MSG *rsp_msg_p;

  //HASH_SEND_TRACE ("HASH CORE enter", RV_TRACE_LEVEL_DEBUG_LOW);

  if (msg_p != NULL)
    {
      /* alloc response message in advance */
      if (rvf_get_msg_buf
          (hash_env_ctrl_blk_p->prim_mb_id, sizeof (T_HASH_MSG), 0,
           (T_RV_HDR **) & rsp_msg_p) == RVF_RED)
        {
          HASH_SEND_TRACE ("HASH CORE out of memory", RV_TRACE_LEVEL_WARNING);
          return RV_MEMORY_ERR;
        }

      switch (msg_p->os_hdr.msg_id)
        {
        case HASH_SHA1_HASH_REQ_MSG:
          ret_val = hash_exec_sha1_hash (msg_p->sha1_hash.context_p,
                                         msg_p->sha1_hash.data_begin_p,
                                         msg_p->sha1_hash.length,
                                         msg_p->sha1_hash.last_block,
                                         rsp_msg_p);
          break;

        case HASH_MD5_HASH_REQ_MSG:
          ret_val = hash_exec_md5_hash (msg_p->md5_hash.context_p,
                                        msg_p->md5_hash.data_begin_p,
                                        msg_p->md5_hash.length,
                                        msg_p->md5_hash.last_block,
                                        rsp_msg_p);
          break;
        default:
          HASH_SEND_TRACE ("HASH CORE unknown message", RV_TRACE_LEVEL_ERROR);
          break;
        }

      /* free request message */
      if (ret_val == RVM_OK)
        {
          ret_val = rvf_free_buf (msg_p);
          if (ret_val != RVM_OK)
            {
              HASH_SEND_TRACE ("HASH CORE could not free msg",
                               RV_TRACE_LEVEL_WARNING);
            }
        }

    }
  else
    {
      HASH_SEND_TRACE ("HASH CORE ignoring NULL message",
                       RV_TRACE_LEVEL_ERROR);
    }

  return ret_val;
}
