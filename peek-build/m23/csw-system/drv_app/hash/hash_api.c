/**
 * @file	hash_api.c
 *
 * API for SHA SWE.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					           Modification
 *	-------------------------------------------------------------------
 *	7/31/2003	Eddy Oude Middendorp (ICT)  split from common &Completion.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */


#include "hash/hash_i.h"

#include "hash/hash_api.h"
#include "hash/hash_env.h"
#include "hash/hash_hw_functions.h"



/**
 * Sample bridge functions.
 *
 * Detailled description.
 *
 * @return	HASH_MEMORY_ERR in case of a memory error,
 *			the return value of rvf_send_msg otherwise.
 */

/********************************************************************************/
/*                                                                              */
/*    Function Name:   hash_get_request_msg                                      */
/*                                                                              */
/*    Purpose:     Allocates a request message with checks and fills in         */
/*                 some general values that are the same for all bridge         */
/*                 functions.                                                   */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - msg_pp : address of a message pointer                          */
/*             - msg_id : the request message id                                */
/*                                                                              */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : HAS task not ready                              */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
static T_RV_RET hash_get_request_msg (T_HASH_MSG ** msg_pp, UINT32 msg_id)
{
  /* HAS task ready & initialised? */
  if (hash_env_ctrl_blk_p == NULL || hash_env_ctrl_blk_p->initialised == FALSE)
  {
    HASH_SEND_TRACE ("hash_get_request_msg: not ready",
                      RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }
  /* reserve message buffer */
  if (rvf_get_msg_buf (hash_env_ctrl_blk_p->prim_mb_id, sizeof (T_HASH_MSG),
                       msg_id, (T_RV_HDR **) msg_pp) == RVF_RED)
  {
    HASH_SEND_TRACE ("hash_get_request_msg: out of memory",
                      RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  return RV_OK;
}


/**
 * function: hash_get_sw_version
 */
UINT32 hash_get_sw_version (void)
{
  return BUILD_VERSION_NUMBER(HASH_MAJOR,HASH_MINOR,HASH_BUILD);
}


/**
 * function: hash_get_hw_version
 */
UINT32 hash_get_hw_version (void)
{
  return hash_hw_get_hardware_version();
}


/**
 * function: hash_set_dma
 */
T_RV_RET hash_set_dma (T_HASH_DMA_MODE hash_dma_mode)
{
  extern T_HASH_ENV_CTRL_BLK *hash_env_ctrl_blk_p;
  T_RV_RET ret_val;

  /* Check if initialization has been correctly done */
  if (hash_env_ctrl_blk_p->initialised == 0)
  {
    HASH_SEND_TRACE("HASH Initialization is not yet done or failed", 
                   RV_TRACE_LEVEL_DEBUG_HIGH);
    ret_val = RV_INTERNAL_ERR;
  }
  else
  {
    HASH_SEND_TRACE("HASH API set dma called", RV_TRACE_LEVEL_DEBUG_HIGH);

    /*
      This function is called to set the dma mode 
    */
    switch (hash_dma_mode)
    {
      case HASH_FORCE_CPU:
#ifdef HASH_ENABLE_DMA
      case HASH_FORCE_DMA:
      case HASH_DMA_AUTO:
#endif
        HASH_DMA_MODE = hash_dma_mode;
        ret_val = RV_OK;
        break;

      default:
        ret_val = RV_INVALID_PARAMETER;
        HASH_SEND_TRACE("HASH API invalid parameter for set_dma", RV_TRACE_LEVEL_DEBUG_HIGH);
        return ret_val;
    }
  }

  return ret_val;
}



/**
 * function: hash_sha1_begin
 */
T_RV_RET hash_sha1_begin (void *context_p, T_RV_RETURN return_path)
{
  T_HASH_CLIENT_STATE *state_p = (T_HASH_CLIENT_STATE *)context_p;

  HASH_SEND_TRACE ("SHA API entering hash_sha1_begin (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);

  if (context_p == NULL)
  {
    return RV_INVALID_PARAMETER;
  }
  //Initialize context
  state_p->hash_type = HASH_TYPE_SHA1;
  state_p->force_init = TRUE;
  state_p->digcnt = 0;
  state_p->rest_cnt = 0;
  state_p->return_path = return_path;

  return RV_OK;
}



/**
 * function: hash_sha1_hash
 */
T_RV_RET hash_sha1_hash (void *context_p, UINT32 length,
                         UINT8 * data_begin_p, BOOL last_block)
{
  T_HASH_MSG *msg_p;
  T_RV_RET ret_val;
  T_HASH_CLIENT_STATE *state_p = (T_HASH_CLIENT_STATE *)context_p;

//  HASH_SEND_TRACE ("SHA API entering hash_sha1_hash (bridge)",
//                  RV_TRACE_LEVEL_DEBUG_HIGH);

  if (context_p == NULL || length == 0 || data_begin_p == 0)
  {
    return RV_INVALID_PARAMETER;
  }
  ret_val = hash_get_request_msg (&msg_p, HASH_SHA1_HASH_REQ_MSG);
  if (ret_val == RV_OK)
  {
    msg_p->sha1_hash.context_p    = context_p;
    msg_p->sha1_hash.length       = length;
    msg_p->sha1_hash.data_begin_p = data_begin_p;
    msg_p->sha1_hash.return_path  = state_p->return_path;
    msg_p->sha1_hash.last_block   = last_block;
    ret_val = rvf_send_msg (hash_env_ctrl_blk_p->addr_id, msg_p);
  }

  return ret_val;
}



/**
 * function: hash_md5_begin
 */
T_RV_RET hash_md5_begin (void *context_p, T_RV_RETURN return_path)
{
  T_HASH_CLIENT_STATE *state_p = (T_HASH_CLIENT_STATE *)context_p;

  HASH_SEND_TRACE ("SHA API entering hash_md5_begin (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);

  if (context_p == NULL)
  {
    return RV_INVALID_PARAMETER;
  }
  //Initialize context
  state_p->hash_type = HASH_TYPE_MD5;
  state_p->force_init = TRUE;
  state_p->digcnt = 0;
  state_p->rest_cnt = 0;
  state_p->return_path = return_path;

  return RV_OK;
}



/**
 * function: hash_md5_hash
 */
T_RV_RET hash_md5_hash (void *context_p, UINT32 length,
                        UINT8 * data_begin_p, BOOL last_block)
{
  T_HASH_MSG *msg_p;
  T_RV_RET ret_val;
  T_HASH_CLIENT_STATE *state_p = (T_HASH_CLIENT_STATE *)context_p;

//  HASH_SEND_TRACE ("SHA API entering hash_md5_hash (bridge)",
//                  RV_TRACE_LEVEL_DEBUG_HIGH);

  if (context_p == NULL || length == 0 || data_begin_p == 0)
  {
    return RV_INVALID_PARAMETER;
  }
  ret_val = hash_get_request_msg (&msg_p, HASH_MD5_HASH_REQ_MSG);
  if (ret_val == RV_OK)
  {
    msg_p->md5_hash.context_p = context_p;
    msg_p->md5_hash.length = length;
    msg_p->md5_hash.data_begin_p = data_begin_p;
    msg_p->md5_hash.return_path = state_p->return_path;
    msg_p->md5_hash.last_block = last_block;
    ret_val = rvf_send_msg (hash_env_ctrl_blk_p->addr_id, msg_p);
  }

  return ret_val;
}
