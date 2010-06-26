/**
 * @file   mc_operations.c
 *
 * Implementation of MC operations.
 * These functions implement the mc processing for all the messages the
 * mc task can receive.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  22/07/2003                  Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "mc/mc_api.h"
#include "mc/mc_env.h"
#include "mc/mc_i.h"
#include "mc/mc_commands.h"

#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

#include <string.h>
/** Private functions*/
static T_RV_RET mc_check_and_get_phys_rca(T_MC_RCA rca, T_MC_RCA *phys_rca_p);
static T_RV_RET mc_check_parm_mode( T_MC_RW_MODE mode);
static T_RV_RET mc_check_data_addres(T_MC_RCA rca, UINT32 addr);
static T_RV_RET mc_check_parm_data(T_MC_RCA rca,  UINT32 addr, UINT32 data_size);
static T_RV_RET mc_check_parm_ocr_p( UINT32 *ocr_p);
static T_RV_RET mc_check_parm_u8_p( UINT8 *u8_p);
static T_RV_RET mc_check_parm_field( T_MC_CSD_FIELD field);
static T_RV_RET mc_send_response_to_client (T_RV_HDR    *rsp_p,
                                             T_RV_RETURN *return_path);
static T_RV_RET mc_send_subscribe_rsp (T_RV_RET          result,
                                        T_MC_SUBSCRIBER  *subscriber_p,
                                        T_RV_RETURN       *return_path);
static T_RV_RET mc_send_unsubscribe_rsp (T_RV_RET         result,
                                          T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_read_rsp (T_RV_RET         result,
                                   UINT32           card_status,
                                   UINT8            *data_p,
                                   UINT32           data_size,
                                   T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_write_rsp (T_RV_RET         result,
                                    UINT32           card_status,
                                    UINT8            *data_p,
                                    UINT32           data_size,
                                    T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_erase_group_rsp (T_RV_RET         result,
                                          UINT32           card_status,
                                          T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_set_protect_rsp (T_RV_RET         result,
                                          UINT32           card_status,
                                          T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_clr_protect_rsp (T_RV_RET         result,
                                          UINT32           card_status,
                                          T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_get_protect_rsp (T_RV_RET    result,
                                          UINT32      card_status,
                                          UINT32      wr_prot_grps,
                                          T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_card_status_rsp (T_RV_RET    result,
                                          UINT32      card_status,
                                          T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_sd_card_status_rsp (T_RV_RET   result,
                                             UINT8      *sd_status_p,
                                             T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_read_scr_rsp (T_RV_RET   result,
                                             UINT8      *scr_p,
                                             T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_update_acq_rsp (T_RV_RET    result,
                                         UINT16      stack_size,
                                         T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_reset_rsp (T_RV_RET     result,
                                    T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_read_ocr_rsp (T_RV_RET    result,
                                       UINT32      *ocr_p,
                                       T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_read_cid_rsp (T_RV_RET         result,
                                       UINT8            *cid_p,
                                       T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_read_csd_rsp (T_RV_RET         result,
                                       UINT8            *csd_p,
                                       T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_write_csd_rsp (T_RV_RET         result,
                                        UINT32           card_status,
                                        T_MC_SUBSCRIBER subscriber);
static T_RV_RET mc_send_notification_rsp(T_RV_RET result,T_MC_SUBSCRIBER subscriber);




/** Functions */


/**
 * function: mc_check_parm_rca
 */
static T_RV_RET mc_check_and_get_phys_rca(T_MC_RCA rca, T_MC_RCA *phys_rca_p)
{
  /** Check RCA parameter */
  if ( (rca > MC_MAX_STACK_SIZE)
       ||(rca == 0)
       ||(  (mc_env_ctrl_blk_p->reserved_card_id & (1 << (rca-1)))
              == 0
         )
       ||(mc_env_ctrl_blk_p->card_infos[rca-1] == NULL)
      )
  {
    MC_SEND_TRACE ("MC parameter: invalid RCA", RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  else
  {
    *phys_rca_p = mc_env_ctrl_blk_p->card_infos[rca-1]->phys_rca;
  }
  return RV_OK;
}


/**
 * function: mc_check_parm_mode
 */
static T_RV_RET mc_check_parm_mode( T_MC_RW_MODE mode)
{
  /** Check mode parameter */
  if ((mode != MC_RW_BLOCK) && (mode != MC_RW_STREAM))
  {
    MC_SEND_TRACE ("MC parameter: invalid mode", RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  if (mode == MC_RW_STREAM)
  {
    MC_SEND_TRACE ("MC parameter: not supported", RV_TRACE_LEVEL_WARNING);
    return RV_NOT_SUPPORTED;
  }
  return RV_OK;
}

/**
 * function: mc_check_data_addres
 */
static T_RV_RET mc_check_data_addres(T_MC_RCA rca, UINT32 addr)
{
  /** Check if desired data range is inside the cards address space*/
  if((addr) > CSD_CARD_SIZE(mc_env_ctrl_blk_p->card_infos[rca-1]->csd))
  {
    MC_SEND_TRACE ("MC parameter: invalid data adress",RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  return RV_OK;
}

/**
 * function: mc_check_parm_data
 */
static T_RV_RET mc_check_parm_data(T_MC_RCA rca,  UINT32 addr, UINT32 data_size)
{
  /** Check data_size and addr parameter */
  if (data_size == 0)
  {
    MC_SEND_TRACE ("MC parameter: invalid data parameter(s)",RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  return mc_check_data_addres(rca,addr + data_size);
}

/**
 * function: mc_check_parm_ocr_p
 */
static T_RV_RET mc_check_parm_ocr_p( UINT32 *ocr_p)
{
  /** Check OCR parameter */
  if (ocr_p == NULL)
  {
    MC_SEND_TRACE ("MC parameter: invalid OCR pointer", RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  else
  {
  return RV_OK;
  }
}
/**
 * function: mc_check_parm_u8_p
 */
static T_RV_RET mc_check_parm_u8_p( UINT8 *u8_p)
{
  /** Check u8_p parameter */
  if (u8_p == NULL)
  {
    MC_SEND_TRACE ("MC parameter: invalid pointer", RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  return RV_OK;
}
/**
 * function: mc_check_parm_field
 */
static T_RV_RET mc_check_parm_field( T_MC_CSD_FIELD field)
{
  /** Check field parameter */
  if ( field != CSD_FIELD_FILE_FORMAT_GRP    &&
       field != CSD_FIELD_COPY               &&
       field != CSD_FIELD_PERM_WRITE_PROTECT &&
       field != CSD_FIELD_TMP_WRITE_PROTECT  &&
       field != CSD_FIELD_FILE_FORMAT        &&
       field != CSD_FIELD_FILE_ECC           &&
       field != CSD_FIELD_FILE_CRC )
  {
    MC_SEND_TRACE ("MC parameter: invalid field parameter",RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  return RV_OK;
}



/**
 * function: mc_send_response_to_client
 */
static T_RV_RET mc_send_response_to_client (T_RV_HDR    *rsp_p,
                                             T_RV_RETURN *return_path)
{
  /* follow return path: callback or mailbox */
  if (return_path->callback_func != NULL)
  {
    MC_SEND_TRACE ("MC operations: callback",
                     RV_TRACE_LEVEL_DEBUG_HIGH);
    return_path->callback_func (rsp_p);
    if (rvf_free_msg ((T_RV_HDR *) rsp_p) != RVF_OK)
    {
      MC_SEND_TRACE ("MC operations: could not free response msg",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }
  } else {
    MC_SEND_TRACE ("MC operations: sending response", RV_TRACE_LEVEL_DEBUG_HIGH);
    if (rvf_send_msg (return_path->addr_id, rsp_p) != RVF_OK)
    {
      MC_SEND_TRACE ("MC operations: could not send response msg",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    };
  }

  return RV_OK;
}

/**
 * function: mc_send_subscribe_rsp
 */
static T_RV_RET mc_send_subscribe_rsp (T_RV_RET          result,
                                        T_MC_SUBSCRIBER  *subscriber_p,
                                        T_RV_RETURN       *return_path)
{
   T_MC_SUBSCRIBE_RSP_MSG *rsp_p;

   /**
    * Create subscribe response message
    */
   if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                        sizeof (T_MC_SUBSCRIBE_RSP_MSG),
                        MC_SUBSCRIBE_RSP_MSG,
                        (T_RV_HDR **) &rsp_p) == RVF_RED)
   {
      MC_SEND_TRACE ("MC mc_send_subscribe_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
   }

   /* compose message */
   rsp_p->result       = result;
   rsp_p->subscriber_p = subscriber_p;

   /* Send message mailbox */
   return mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                         return_path);
}

/**
 * function: mc_send_unsubscribe_rsp
 */
static T_RV_RET mc_send_unsubscribe_rsp (T_RV_RET         result,
                                          T_MC_SUBSCRIBER subscriber)
{
  T_MC_UNSUBSCRIBE_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create unsubscribe response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_UNSUBSCRIBE_RSP_MSG),
                         MC_UNSUBSCRIBE_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_unsubscribe_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result = result;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_unsubscribe_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_read_rsp
 */
static T_RV_RET mc_send_read_rsp (T_RV_RET         result,
                                   UINT32           card_status,
                                   UINT8            *data_p,
                                   UINT32           data_size,
                                   T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create read response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_READ_RSP_MSG),
                         MC_READ_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_read_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->card_status = card_status;
    rsp_p->data_p      = data_p;
    rsp_p->data_size   = data_size;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_read_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_write_rsp
 */
static T_RV_RET mc_send_write_rsp (T_RV_RET         result,
                                    UINT32           card_status,
                                    UINT8            *data_p,
                                    UINT32           data_size,
                                    T_MC_SUBSCRIBER subscriber)
{
  T_MC_WRITE_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create write response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_WRITE_RSP_MSG),
                         MC_WRITE_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_write_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->card_status = card_status;
    rsp_p->data_p      = data_p;
    rsp_p->data_size   = data_size;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                           &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_write_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_erase_group_rsp
 */
static T_RV_RET mc_send_erase_group_rsp (T_RV_RET         result,
                                          UINT32           card_status,
                                          T_MC_SUBSCRIBER subscriber)
{
  T_MC_ERASE_GROUP_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create erase group response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_ERASE_GROUP_RSP_MSG),
                         MC_ERASE_GROUP_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_erase_group_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->card_status = card_status;

    /* Send message mailbox */
    return mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                         &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_erase_group_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_set_protect_rsp
 */
static T_RV_RET mc_send_set_protect_rsp (T_RV_RET         result,
                                          UINT32           card_status,
                                          T_MC_SUBSCRIBER subscriber)
{
  T_MC_SET_PROTECT_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create protect response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_SET_PROTECT_RSP_MSG),
                         MC_SET_PROTECT_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_set_protect_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->card_status = card_status;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_set_protect_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_clr_protect_rsp
 */
static T_RV_RET mc_send_clr_protect_rsp (T_RV_RET         result,
                                          UINT32           card_status,
                                          T_MC_SUBSCRIBER subscriber)
{
  T_MC_CLR_PROTECT_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create clear protect response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_CLR_PROTECT_RSP_MSG),
                         MC_CLR_PROTECT_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_clr_protect_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->card_status = card_status;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_clr_protect_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_get_protect_rsp
 */
static T_RV_RET mc_send_get_protect_rsp (T_RV_RET    result,
                                          UINT32      card_status,
                                          UINT32      wr_prot_grps,
                                          T_MC_SUBSCRIBER subscriber)
{
  T_MC_GET_PROTECT_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create get protect response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_GET_PROTECT_RSP_MSG),
                         MC_GET_PROTECT_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_get_protect_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result       = result;
    rsp_p->card_status  = card_status;
    rsp_p->wr_prot_grps = wr_prot_grps;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_get_protect_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_card_status_rsp
 */
static T_RV_RET mc_send_card_status_rsp (T_RV_RET    result,
                                          UINT32      card_status,
                                          T_MC_SUBSCRIBER subscriber)
{
  T_MC_CARD_STATUS_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create card status response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_CARD_STATUS_RSP_MSG),
                         MC_CARD_STATUS_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_card_status_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result       = result;
    rsp_p->card_status  = card_status;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_card_status_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_card_status_rsp
 */
static T_RV_RET mc_send_sd_card_status_rsp (T_RV_RET   result,
                                             UINT8      *sd_status_p,
                                             T_MC_SUBSCRIBER subscriber)
{
  T_MC_SD_CARD_STATUS_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create card status response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_SD_CARD_STATUS_RSP_MSG),
                         MC_SD_CARD_STATUS_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_card_status_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result       = result;
    rsp_p->sd_status_p  = sd_status_p;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_sd_card_status_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_card_status_rsp
 */
static T_RV_RET mc_send_read_scr_rsp (T_RV_RET   result,
                                       UINT8      *scr_p,
                                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_SCR_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create card status response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_READ_SCR_RSP_MSG),
                         MC_READ_SCR_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_read_scr_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result = result;
    rsp_p->scr_p  = scr_p;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_read_scr_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_update_acq_rsp
 */
static T_RV_RET mc_send_update_acq_rsp (T_RV_RET    result,
                                         UINT16      stack_size,
                                         T_MC_SUBSCRIBER subscriber)
{
  T_MC_UPDATE_ACQ_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create update acq. response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_UPDATE_ACQ_RSP_MSG),
                         MC_UPDATE_ACQ_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_update_acq_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->stack_size  = stack_size;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_update_acq_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_reset_rsp
 */
static T_RV_RET mc_send_reset_rsp (T_RV_RET     result,
                                    T_MC_SUBSCRIBER subscriber)
{
  T_MC_RESET_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create reset response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_RESET_RSP_MSG),
                         MC_RESET_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_reset_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result = result;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_reset_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_read_ocr_rsp
 */
static T_RV_RET mc_send_read_ocr_rsp (T_RV_RET    result,
                                       UINT32      *ocr_p,
                                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_OCR_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create read ocr response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_READ_OCR_RSP_MSG),
                         MC_READ_OCR_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_read_ocr_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result = result;
    rsp_p->ocr_p  = ocr_p;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_read_ocr_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_read_cid_rsp
 */
static T_RV_RET mc_send_read_cid_rsp (T_RV_RET         result,
                                       UINT8            *cid_p,
                                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_CID_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create read cid response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_READ_CID_RSP_MSG),
                         MC_READ_CID_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_read_cid_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result = result;
    rsp_p->cid_p  = cid_p;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                         &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_read_cid_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_read_csd_rsp
 */
static T_RV_RET mc_send_read_csd_rsp (T_RV_RET         result,
                                       UINT8            *csd_p,
                                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_CSD_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create read csd response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_READ_CSD_RSP_MSG),
                         MC_READ_CSD_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_read_csd_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result = result;
    rsp_p->csd_p  = csd_p;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                         &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_read_csd_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * function: mc_send_write_csd_rsp
 */
static T_RV_RET mc_send_write_csd_rsp (T_RV_RET         result,
                                        UINT32           card_status,
                                        T_MC_SUBSCRIBER subscriber)
{
  T_MC_WRITE_CSD_RSP_MSG *rsp_p;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_RV_RET retval;

  /* Check if subscriber id is correct */
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    /**
     * Create write csd response message
     */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_WRITE_CSD_RSP_MSG),
                         MC_WRITE_CSD_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_write_csd_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;
    rsp_p->card_status = card_status;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(subscriber_infos[subscriber]->return_path));
  } else {
    MC_SEND_TRACE("MC mc_send_write_csd_rsp: Subscriber Id unknown",
                    RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}

/**
 * send response message to client which made a notification request
 *
 * This function will send a response message (MC_SEND_NOTIFICATION_REQ_MSG)
 * to the client after a MC_SEND_NOTIFICATION_REQ_MSG.
 *
 * @param   result      result of requested action.
 * @param   subscriber  subscriber to send response to.
 *
 * @return  RV_OK
 *          RV_MEMORY_ERR
 *          RV_INVALID_PARAMETER
 */

/*@{*/
static T_RV_RET mc_send_notification_rsp(T_RV_RET result,T_MC_SUBSCRIBER subscriber)
{
  T_RV_RET retval = RV_OK;

  T_MC_NOTIFICATION_RSP_MSG *rsp_p;
  if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {


    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_NOTIFICATION_RSP_MSG),
                         MC_NOTIFICATION_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_notification_rsp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->result      = result;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p,
                                          &(mc_env_ctrl_blk_p->subscriber_infos[subscriber]->return_path));
  }
  else
  {
    MC_SEND_TRACE("MC mc_send_notification_rsp: Subscriber Id unknown",
                RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}
/*@}*/

T_RV_RET mc_send_event_ind_msg(T_MC_EVENTS event,T_MC_SUBSCRIBER subscriber)
{
  T_MC_EVENT_IND_MSG *msg_p;
  T_RV_RET retval = RV_OK;

   if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber) )
  {
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_EVENT_IND_MSG),
                         MC_EVENT_IND_MSG,
                         (T_RV_HDR **) &msg_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_send_event_ind_msg: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    msg_p->event      = event;

    /* Send message mailbox */
    retval = mc_send_response_to_client ((T_RV_HDR *) msg_p,
                                          &(mc_env_ctrl_blk_p->subscriber_infos[subscriber]->return_path));
  }
  else
  {
    MC_SEND_TRACE("MC mc_send_event_ind_msg: Subscriber Id unknown",
                RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  return retval;


}
/*@}*/


/**
 * function: mc_add_subscriber
 */
static T_RV_RET mc_add_subscriber(T_MC_SUBSCRIBER *subscriber_p)
{
  UINT8 i;
  UINT8 nb_subscriber = 0;
  T_RV_RET retval = RV_OK;

  /* Check number of subscribers */
  for (i = 0; i < MC_MAX_SUBSCRIBER; i++)
  {
    if (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<i))
    {
      nb_subscriber++;
    }
  }

  if (nb_subscriber >= MC_MAX_SUBSCRIBER)
  {
    MC_SEND_TRACE("MC: Max of subscriber reached", RV_TRACE_LEVEL_WARNING);
    retval = RV_INTERNAL_ERR;
  }
  else
  {
    for (i = 0; i < MC_MAX_SUBSCRIBER; i++)
    {
      if ( (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<i)) == 0)
      {
        mc_env_ctrl_blk_p->reserved_subscriber_id |= 1<<i;
        *subscriber_p = i;
        break;
      }
    }
  }

  return retval;
}

/**
 * function: mc_remove_subscriber
 */
static T_RV_RET mc_remove_subscriber(T_MC_SUBSCRIBER subscriber)
{
   T_RV_RET retval = RV_OK;

   /* Check if subscriber id is correct */
   if ((subscriber <= MC_MAX_SUBSCRIBER) &&
       (mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber)) )
   {
      /* Unreserve the id */
      mc_env_ctrl_blk_p->reserved_subscriber_id &= ~(1U<<subscriber);
   }
   else
   {
      MC_SEND_TRACE("MC: Subscriber Id unknown", RV_TRACE_LEVEL_ERROR);
      retval = RV_INVALID_PARAMETER;
   }

   return retval;
}

/**
 * function: mc_return_path_already_defined
 */
static BOOL mc_return_path_already_defined(T_RV_RETURN return_path)
{
  UINT8 i;
  BOOL retval = FALSE;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;

  for (i = 0; i < MC_MAX_SUBSCRIBER; i++)
  {
    if ( subscriber_infos[i] != NULL )
    {
      if (subscriber_infos[i]->return_path.callback_func != NULL)
      {
        if (subscriber_infos[i]->return_path.callback_func == return_path.callback_func)
        {
           retval = TRUE;
           break;
        }
      } else {
        if (subscriber_infos[i]->return_path.addr_id == return_path.addr_id)
        {
            retval = TRUE;
            break;
        }
      }
    }
  }
  return retval;
}

/**
 * Initialises the MC SD driver
 *
 * This function will initialise the MC SD driver.
 * First the mc driver will be put to initialised to
 * enable event handling.
 * Second, the host controller will be initialised at low speed.
 * Third a card aquisition will be started to indentify
 * the available cards.
 * When this is succesfull the hostcontroller will
 * be set into normal operation speed
 *
 *
 * @return  T_RV_RET:
 */

/*@{*/
T_RV_RET mc_op_init_driver(void)
{
    T_RV_RET retval = RV_OK;
    mc_env_ctrl_blk_p->initialised = TRUE;

    /** initialise the host configuration */
    mc_env_ctrl_blk_p->conf.dw       = 0x0;            /* data width */
    mc_env_ctrl_blk_p->conf.mode     = 0x0;            /* MC mode */
    mc_env_ctrl_blk_p->conf.power_up = 0x1;            /* Power up */
    mc_env_ctrl_blk_p->conf.be       = 0x0;            /* Little endian */
    mc_env_ctrl_blk_p->conf.clk_div  = MC_ID_CLK_DIV; /* Clk devider */
    mc_env_ctrl_blk_p->conf.cto      = 0xFF;           /* CMD time-out */
    mc_env_ctrl_blk_p->conf.dto      = 0xFFFE;          /* Data time-out */

 /* COMMENTED FOR LOCOSTO TRANSFLASH AS NO CONTROLLER AVAILABLE	*/
    retval =  mc_cmd_init_host(&mc_env_ctrl_blk_p->conf ,  mc_env_ctrl_blk_p->irq);
   if (retval == RV_OK)
    {
          /* Update aquisition */
//      retval = mc_cmd_init_stack();
      if (retval == RV_OK)
      {
        /* Speed up */
        mc_env_ctrl_blk_p->conf.clk_div = MC_NORM_CLK_DIV;
       /*COMMENTED FOR LOCOSTO  */
	    //mc_cmd_init_host(&mc_env_ctrl_blk_p->conf ,
        //                   mc_env_ctrl_blk_p->irq);
      }
    }
    return retval;

}
/*@}*/



/**
 * function: mc_op_stack_size
 */
static UINT16 mc_op_stack_size (void)
{
  UINT16 i, stack_size = 0;

  for (i = 0; i < MC_MAX_STACK_SIZE; i++)
  {
    if (mc_env_ctrl_blk_p->reserved_card_id & (1 << i))
    {
      stack_size++;
    }
  }
  return stack_size;
}

/**
 * function: mc_op_driver_not_ready
 */
T_RV_RET mc_op_driver_not_ready(T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  switch (msg_p->msg_id)
  {
    case MC_SUBSCRIBE_REQ_MSG:
      {
        T_MC_SUBSCRIBE_REQ_MSG *req_p = (T_MC_SUBSCRIBE_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_subscribe_rsp(RV_NOT_READY,   /* result */
                                        NULL,           /* subscriber_p */
                                        &(req_p->return_path));
      }
      break;

    case MC_UNSUBSCRIBE_REQ_MSG:
      {
        T_MC_UNSUBSCRIBE_REQ_MSG *req_p = (T_MC_UNSUBSCRIBE_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_unsubscribe_rsp(RV_NOT_READY,   /* result */
                                          *(req_p->subscriber_p));
      }
      break;

    case MC_READ_REQ_MSG:
      {
        T_MC_READ_REQ_MSG *req_p = (T_MC_READ_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_read_rsp(RV_NOT_READY,   /* result */
                                   0x0,            /* card status */
                                   NULL,           /* data_p */
                                   0,              /* data_size */
                                   req_p->subscriber);
      }
      break;

    case MC_WRITE_REQ_MSG:
      {
        T_MC_WRITE_REQ_MSG *req_p = (T_MC_WRITE_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_write_rsp(RV_NOT_READY,   /* result */
                                    0x0,            /* card status */
                                    NULL,           /* data_p */
                                    0,              /* data_size */
                                    req_p->subscriber);
      }
      break;

    case MC_ERASE_GROUP_REQ_MSG:
      {
        T_MC_ERASE_GROUP_REQ_MSG *req_p = (T_MC_ERASE_GROUP_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_erase_group_rsp(RV_NOT_READY,   /* result */
                                          0x0,            /* card status */
                                          req_p->subscriber);
      }
      break;

    case MC_SET_PROTECT_REQ_MSG:
      {
        T_MC_SET_PROTECT_REQ_MSG *req_p = (T_MC_SET_PROTECT_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_set_protect_rsp(RV_NOT_READY,   /* result */
                                          0x0,            /* card status */
                                          req_p->subscriber);
      }
      break;

    case MC_CLR_PROTECT_REQ_MSG:
      {
        T_MC_CLR_PROTECT_REQ_MSG *req_p = (T_MC_CLR_PROTECT_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_clr_protect_rsp(RV_NOT_READY,   /* result */
                                          0x0,            /* card status */
                                          req_p->subscriber);
      }
      break;

    case MC_GET_PROTECT_REQ_MSG:
      {
        T_MC_GET_PROTECT_REQ_MSG *req_p = (T_MC_GET_PROTECT_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_get_protect_rsp (RV_NOT_READY,
                                           0,              /* card_status */
                                           0,              /* wr_prot_grps */
                                           req_p->subscriber);
      }
      break;

    case MC_CARD_STATUS_REQ_MSG:
      {
        T_MC_CARD_STATUS_REQ_MSG *req_p = (T_MC_CARD_STATUS_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_card_status_rsp (RV_NOT_READY,
                                           0,              /* card_status */
                                           req_p->subscriber);
      }
      break;

    case MC_UPDATE_ACQ_REQ_MSG:
      {
        T_MC_UPDATE_ACQ_REQ_MSG *req_p = (T_MC_UPDATE_ACQ_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_update_acq_rsp (RV_NOT_READY,
                                          0,              /* stack_size, */
                                          req_p->subscriber);
      }
      break;

    case MC_RESET_REQ_MSG:
      {
        T_MC_RESET_REQ_MSG *req_p = (T_MC_RESET_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_reset_rsp (RV_NOT_READY,
                                     req_p->subscriber);
      }
      break;

    case MC_READ_OCR_REQ_MSG:
      {
        T_MC_READ_OCR_REQ_MSG *req_p = (T_MC_READ_OCR_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_read_ocr_rsp (RV_NOT_READY,
                                        NULL,              /* ocr_p */
                                        req_p->subscriber);
      }
      break;

    case MC_READ_CID_REQ_MSG:
      {
        T_MC_READ_CID_REQ_MSG *req_p = (T_MC_READ_CID_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_read_cid_rsp (RV_NOT_READY,
                                        NULL,              /* cid_p */
                                        req_p->subscriber);
      }
      break;

    case MC_READ_CSD_REQ_MSG:
      {
        T_MC_READ_CSD_REQ_MSG *req_p = (T_MC_READ_CSD_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_read_csd_rsp (RV_NOT_READY,
                                        NULL,              /* csd_p */
                                        req_p->subscriber);
      }
      break;

    case MC_WRITE_CSD_REQ_MSG:
      {
        T_MC_WRITE_CSD_REQ_MSG *req_p = (T_MC_WRITE_CSD_REQ_MSG *) msg_p;

        /** Compose response message and send it */
        retval = mc_send_write_csd_rsp (RV_NOT_READY,
                                         0,              /* card_status */
                                         req_p->subscriber);
      }
      break;

    case MC_SD_CARD_STATUS_REQ_MSG:
    {
      T_MC_SD_CARD_STATUS_REQ_MSG *req_p = (T_MC_SD_CARD_STATUS_REQ_MSG*) msg_p;

      /** Compose response message and send it */
      retval = mc_send_sd_card_status_rsp(RV_NOT_READY,
                                           NULL,
                                           req_p->subscriber);
    }
    break;
    default:
      MC_SEND_TRACE ("MC CORE attached - unknown message",
                       RV_TRACE_LEVEL_ERROR);
      retval = RV_INTERNAL_ERR;

      break;
  }
  return retval;
}

/**
 * function: mc_op_subscribe
 */
T_RV_RET mc_op_subscribe(T_MC_SUBSCRIBE_REQ_MSG *msg_p)
{
  T_RVF_MB_STATUS mb_status;
  T_RV_RET retval;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_MC_SUBSCRIBER *subscriber_p = msg_p->subscriber_p;

  MC_SEND_TRACE("MC: mc_op_subscribe called",
                  RV_TRACE_LEVEL_DEBUG_LOW);


  if(msg_p->subscriber_p == NULL)
  {
    /* compose message */
    retval = mc_send_subscribe_rsp(RV_INVALID_PARAMETER,
                                    NULL,
                                    &(msg_p->return_path));
     return retval;
  }

  if (mc_return_path_already_defined(msg_p->return_path) == TRUE)
  {
    /* compose message */
    retval = mc_send_subscribe_rsp(RV_INVALID_PARAMETER,
                                    NULL,
                                    &(msg_p->return_path));
  }
  else
  {
    /* Reserve subscriber id */
    retval = mc_add_subscriber(subscriber_p);

    if (retval == RV_OK)
    {
      /* Update subscriber informations */
      /* ------------------------------ */

      /* Reserve memory for subscriber informations */
      mb_status = rvf_get_buf (mc_env_ctrl_blk_p->prim_mb_id,
                               sizeof(T_SUBSCRIBER_INFO),
                               (void **) &subscriber_infos[*subscriber_p]);

      if (mb_status != RVF_RED) /* Memory allocation success */
      {
        /* Initialize structure */
        memset(subscriber_infos[*subscriber_p], 0, sizeof(T_SUBSCRIBER_INFO));

        /* Fill the subscriber structure */
        subscriber_infos[*subscriber_p]->return_path = msg_p->return_path;
         /** More structure parameters in future implementation */

        /* compose message */
        retval = mc_send_subscribe_rsp(RV_OK,
                                        subscriber_p,
                                        &(msg_p->return_path));
      }
      else
      {
        MC_SEND_TRACE("MC mc_op_subscribe: Memory allocation error",
                        RV_TRACE_LEVEL_ERROR);

        /* Remove subscriber id because id was reserved */
        mc_remove_subscriber(*subscriber_p);

        /* compose message */
        retval = mc_send_subscribe_rsp(RV_INTERNAL_ERR,
                                        NULL,
                                        &(msg_p->return_path));
      }
    } else {
      /** could not reserve subscriber id */
      MC_SEND_TRACE("MC mc_op_subscribe: Could not reserve subscriber id",
                      RV_TRACE_LEVEL_ERROR);
      /* compose message */
      retval = mc_send_subscribe_rsp(RV_INTERNAL_ERR,
                                      NULL,
                                      &(msg_p->return_path));
    }
  }

  return retval;
}

/**
 * function: mc_op_unsubscribe
 */
T_RV_RET mc_op_unsubscribe(T_MC_UNSUBSCRIBE_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  T_SUBSCRIBER_INFO** subscriber_infos = mc_env_ctrl_blk_p->subscriber_infos;
  T_MC_SUBSCRIBER subscriber_id = *(msg_p->subscriber_p);
  T_RV_RETURN rp;
  T_MC_UNSUBSCRIBE_RSP_MSG *rsp_p;

  MC_SEND_TRACE("MC: mc_op_unsubscribe called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if(msg_p->subscriber_p == NULL)
  {
     return RV_INVALID_PARAMETER;
  }


  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<subscriber_id) )
  {
    /* Free subscriber Id */
    retval = mc_remove_subscriber(subscriber_id);

    if (retval == RV_OK)
    {
      /* Save return_path before killing the subscriber ID */
      rp = subscriber_infos[subscriber_id]->return_path;

      /* Delete subscriber informations */
      MC_FREE_BUF(subscriber_infos[subscriber_id]);
      subscriber_infos[subscriber_id] = 0;

      /**
       * Create unsubscribe response message
       */
      if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                           sizeof (T_MC_UNSUBSCRIBE_RSP_MSG),
                           MC_UNSUBSCRIBE_RSP_MSG,
                           (T_RV_HDR **) &rsp_p) == RVF_RED)
      {
        MC_SEND_TRACE ("MC mc_op_unsubscribe: out of memory",
                        RV_TRACE_LEVEL_WARNING);
        return RV_MEMORY_ERR;
      }

      /* compose message */
      rsp_p->result = retval;

      /* Send message mailbox */
      retval = mc_send_response_to_client ((T_RV_HDR *) rsp_p, &rp);

    }
  } else {
    /* subscriber id is not correct */
    retval = RV_INVALID_PARAMETER;
  }

  return retval;
}


/**
 * function: mc_op_update_acq
 */
T_RV_RET mc_op_update_acq_req(T_MC_UPDATE_ACQ_REQ_MSG *msg_p)
{
  T_RV_RET retval     = RV_OK;
  UINT16   stack_size = 0;

  MC_SEND_TRACE("MC: mc_op_update_acq called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    /** Reset host controller */
    mc_env_ctrl_blk_p->conf.clk_div = MC_ID_CLK_DIV;
    //mc_cmd_init_host(&mc_env_ctrl_blk_p->conf ,
    //                  mc_env_ctrl_blk_p->irq);

    /** Call MC commands */
    retval = mc_cmd_init_stack();

    if (retval == RV_OK)
    {
      /** MC card(s) attached */
      mc_env_ctrl_blk_p->state = MC_STATE_IDLE_ATTACHED;

      /** Reset host controller */
      mc_env_ctrl_blk_p->conf.clk_div = MC_NORM_CLK_DIV;
      //mc_cmd_init_host(&mc_env_ctrl_blk_p->conf ,
      //                   mc_env_ctrl_blk_p->irq);

      /** Determine mine stack size */
      stack_size = mc_op_stack_size ();

      /** Compose and send OK message */
      mc_send_update_acq_rsp (retval,
                               stack_size,
                               msg_p->subscriber);
    } else {
      /** No stack could be acquired */
      MC_SEND_TRACE("MC mc_op_subscribe: Could not acquire card stack",
                      RV_TRACE_LEVEL_ERROR);
      /* compose message */
      retval = mc_send_update_acq_rsp(retval,
                                       stack_size,
                                       msg_p->subscriber);
    }
  } else {
    MC_SEND_TRACE ("MC mc_op_subscribe: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_reset_req
 */
T_RV_RET mc_op_reset_req(T_MC_RESET_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 i;

  MC_SEND_TRACE("MC: mc_op_reset_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    /** Call MC commands */
    retval = mc_cmd_reset();
    if (retval == RV_OK)
    {
      /** MC card(s) attached */
      mc_env_ctrl_blk_p->state = MC_STATE_IDLE_DETACHED;

      /** Reset stack admin */
      for(i = 0;i < MC_MAX_STACK_SIZE;i++)
      {
        if(mc_env_ctrl_blk_p->card_infos[i] != NULL)
        {
          MC_FREE_BUF(mc_env_ctrl_blk_p->card_infos[i]);
          mc_env_ctrl_blk_p->card_infos[i] = NULL;
        }
      }
      mc_env_ctrl_blk_p->reserved_card_id = 0;
    }
    /** Compose message */
    mc_send_reset_rsp (retval,
                        msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_reset_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_read_req
 */
T_RV_RET mc_op_read_req(T_MC_READ_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  T_RV_RET result=RV_OK;
  UINT32 card_stat =0;
  UINT32 blk_len=0, nblk;
  T_MC_RCA phys_rca;
  UINT32 addr;
  UINT32 data_length;
  UINT32 i=0;
  MC_SEND_TRACE("MC: mc_op_read_req called",RV_TRACE_LEVEL_DEBUG_LOW);


  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    //retval =  mc_check_data_addres(msg_p->rca, msg_p->addr);
	 retval =  mc_check_data_addres(msg_p->rca, msg_p->addr);

   #if 0
	if(retval == RV_OK)
    {
      retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);
    }
   #endif

    if (retval == RV_OK)
    {
      retval = mc_check_parm_mode(msg_p->mode);
    }

    if (retval == RV_OK)
    {
      retval = mc_check_parm_u8_p(msg_p->data_p);
    }

    if (retval == RV_OK)
    {
      retval = mc_check_parm_data(msg_p->rca, msg_p->addr, msg_p->data_size);
    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(msg_p->rca, MC_STATE_STDY, TRUE);
   //   retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);
    }

		 blk_len = 1 << (CSD_GET_READ_BL_LEN(mc_env_ctrl_blk_p->card_infos[msg_p->rca - 1]->csd));
		 if(blk_len>512)
			blk_len=512;

    if (retval == RV_OK)
    {
      /* Read block length from CSD register buffers in card info*/
//      blk_len = 1 << (CSD_GET_READ_BL_LEN(mc_env_ctrl_blk_p->card_infos[msg_p->rca - 1]->csd));
//	  if(blk_len>512)
//	  blk_len=512;
      if(msg_p->data_size%blk_len)
      {
        retval = RV_INVALID_PARAMETER;
      }
    }

#if 0
    //Now the functional code
    if (retval == RV_OK)
    {
      /** Select card */
      //retval = mc_cmd_setup_card(msg_p->rca,NULL,&card_stat);
      retval = mc_cmd_setup_card(NULL,&card_stat);
    }
#endif
    if (retval == RV_OK)
    {
      /**
       *  Calculate data size, must be block aligned
       */
      if (msg_p->data_size % blk_len)
      {
        /** Not aligned */
        MC_SEND_TRACE ("MC mc_op_read_req: Data size is not block aligned",
                       RV_TRACE_LEVEL_ERROR);
        msg_p->addr = 0;
        msg_p->data_size = 0;
        retval = RV_INVALID_PARAMETER;
      }
     else
      {
        nblk = (UINT16)(msg_p->data_size / blk_len);

        /** Call multiple block read */
        if (nblk > 1 ) {

                                   retval = mc_cmd_read_mblock(mc_env_ctrl_blk_p->dma_mode,
                                                               nblk,
                                                               blk_len,
                                                               msg_p->addr,
                                                               msg_p->data_p,
                                                               msg_p->data_size,
                                                               &card_stat);
		send_status(NULL);



      }
	else
	  {
            retval = mc_cmd_read_block(mc_env_ctrl_blk_p->dma_mode,
                                     blk_len,
                                     msg_p->addr,
                                     msg_p->data_p,
                                     msg_p->data_size,
                                     &card_stat);
      }

      }

     send_status(NULL);
    }

     /** Call MC commands */
    retval = mc_send_read_rsp (retval,
                               card_stat,
                               msg_p->data_p,
                               msg_p->data_size,
                               msg_p->subscriber);



  } else {
    MC_SEND_TRACE ("MC mc_op_read_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_write_req
 */
T_RV_RET mc_op_write_req(T_MC_READ_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  UINT32 card_stat = 0;
  UINT16 blk_len=0, nblk,count;
  T_MC_RCA phys_rca = 0;


  MC_SEND_TRACE("MC: mc_op_write_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {

    if (retval == RV_OK)
    {
      retval = mc_check_parm_mode(msg_p->mode);
    }
    if (retval == RV_OK)
    {
      retval = mc_check_parm_u8_p(msg_p->data_p);
    }
    if (retval == RV_OK)
    {
      retval = mc_check_parm_data(msg_p->rca, msg_p->addr, msg_p->data_size);
    }
    if (retval == RV_OK)
    {
	   send_status(NULL);
    //retval=send_status(NULL);
      /** Wait until card is in standby state */
     // retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
     // retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);
    }

      /* Read block length from CSD register buffers in card info*/
      blk_len = 1 << (CSD_GET_READ_BL_LEN(mc_env_ctrl_blk_p->card_infos[msg_p->rca - 1]->csd));
	  if(blk_len>512)
	  blk_len=512;

    if(msg_p->data_size%blk_len)
    {
      retval = RV_INVALID_PARAMETER;
    }

  	if (retval == RV_OK)
    {
      /**
       *  Calculate data size, must be block aligned
       */

	  if (msg_p->data_size % blk_len)
      {
        /** Not aligned */
        MC_SEND_TRACE ("MC mc_op_write_req: Data size is not block aligned",
                       RV_TRACE_LEVEL_ERROR);
        msg_p->addr = 0;
        msg_p->data_size = 0;
        retval = RV_INVALID_PARAMETER;

      }
	  else
	  {
       nblk = (UINT16)(msg_p->data_size / blk_len);


       if (nblk > 1 )
		{

        /** Call multiple block write */
        	retval = mc_cmd_write_mblock(mc_env_ctrl_blk_p->dma_mode,
                                     nblk,
                                     blk_len,
                                     msg_p->addr,
                                     msg_p->data_p,
                                     msg_p->data_size,
                                     &card_stat);
        //    send_status(NULL);

        }
		else

		{
          /** Call block write */




                      retval = mc_cmd_write_block(mc_env_ctrl_blk_p->dma_mode,
                                      blk_len,
                                      (msg_p->addr ),
                                      (msg_p->data_p),//+(count*512)),
                                      msg_p->data_size,
                                      &card_stat);


		send_status(NULL);

      }

      /* the card was already selected so we have to deselect it */
      /* first wait until card is out of receive state */
      // result=mc_wait_card_state(phys_rca, MC_STATE_RCV, FALSE);
      // result=mc_wait_card_state(MC_STATE_RCV, FALSE);

    }
    /** Call MC commands */
    retval = mc_send_write_rsp (retval,
                                card_stat,
                                msg_p->data_p,
                                msg_p->data_size,
                                msg_p->subscriber);
  }
}
  else
  {
    MC_SEND_TRACE ("MC mc_op_write_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;

  }
  return retval;
}


/**
 * function: mc_op_read_ocr_req
 */
T_RV_RET mc_op_read_ocr_req(T_MC_READ_OCR_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_read_ocr_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);



    if (retval == RV_OK)
    {
      retval = mc_check_parm_ocr_p(msg_p->ocr_p);
    }

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Assign OCR parameter */
      *(msg_p->ocr_p) = mc_env_ctrl_blk_p->card_infos[msg_p->rca-1]->ocr;
    }

    /** Compose message */
    retval =mc_send_read_ocr_rsp (retval,msg_p->ocr_p,
                                    msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_read_ocr_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_read_csd_req
 */
T_RV_RET mc_op_read_csd_req(T_MC_READ_CSD_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_read_csd_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    if (retval == RV_OK)
    {
      retval = mc_check_parm_u8_p(msg_p->csd_p);
    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //  retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
      //  retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);
       retval =  mc_cmd_send_status(NULL);
    }

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Assign CSD parameter */
      //retval = mc_cmd_send_csd (phys_rca, msg_p->csd_p);
        retval = mc_cmd_send_csd (msg_p->csd_p);
    }

    /** Compose message */
    retval = mc_send_read_csd_rsp (retval,
                                    msg_p->csd_p,
                                    msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_read_csd_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_read_cid_req
 */
T_RV_RET mc_op_read_cid_req(T_MC_READ_CID_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_read_cid_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    if (retval == RV_OK)
    {
      retval = mc_check_parm_u8_p(msg_p->cid_p);
    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
//     retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
      // retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);
    }

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Assign CSD parameter */
      //retval = mc_cmd_send_cid (msg_p->rca, msg_p->cid_p);
      retval = mc_cmd_send_cid (msg_p->cid_p);
    }

    /** Compose message */
    retval = mc_send_read_cid_rsp (retval,
                                    msg_p->cid_p,
                                    msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_read_cid_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_card_status_req
 */
T_RV_RET mc_op_card_status_req(T_MC_CARD_STATUS_REQ_MSG *msg_p)
{
  T_RV_RET retval      = RV_OK;
  UINT32   card_status = 0;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_card_status_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    //Now the functional code
    if (retval == RV_OK)
    {
      /** Get card status */
      retval = mc_cmd_send_status (&card_status);
    }

    /** Compose message */
     mc_send_card_status_rsp (retval,
                                       card_status,
                                       msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_card_status_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_erase_group
 */
T_RV_RET mc_op_erase_group_req(T_MC_ERASE_GROUP_REQ_MSG *msg_p)
{
  T_RV_RET retval      = RV_OK;
  UINT32   card_status = 0;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_erase_group called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    //retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    if(retval == RV_OK)
    {
      /** Check is start is before end*/
      if(msg_p->erase_group_start > msg_p->erase_group_end)
      {
        retval = RV_INVALID_PARAMETER;
      }

    }

    if(retval == RV_OK)
    {
       /**Check Erase start boundaries*/
       retval = mc_check_data_addres(msg_p->rca, msg_p->erase_group_start);
    }

    if(retval == RV_OK)
    {
       /**Check Erase end boundaries*/
       retval = mc_check_data_addres(msg_p->rca, msg_p->erase_group_end);
    }
#if 0
    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
        retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);
    }
#endif

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Select card */
      //retval = mc_cmd_setup_card(phys_rca,NULL,&card_status);
      //retval = mc_cmd_setup_card(NULL,&card_status);
      //if (retval == RV_OK)
     // {
        /** Get card status */
      //retval = mc_cmd_erase_group(phys_rca,mc_env_ctrl_blk_p->card_infos[msg_p->rca-1]->card_type,
      //                               msg_p->erase_group_start,
      //                               msg_p->erase_group_end,
      //                               &card_status);
		retval = mc_cmd_erase_group(mc_env_ctrl_blk_p->card_infos[msg_p->rca-1]->card_type,
                                     msg_p->erase_group_start,
                                     msg_p->erase_group_end,
                                     &card_status);
     // }

      /** Deselect card */
    //  mc_cmd_deselect_card(NULL);
    }

    /** Compose message */
    retval = mc_send_erase_group_rsp(retval,
                                      card_status,
                                      msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_card_status_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}
/**
 * function: mc_op_write_csd_req
 */
T_RV_RET mc_op_write_csd_req(T_MC_WRITE_CSD_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  UINT32   card_status = 0;
  UINT8   *csd_p;
  T_RVF_MB_STATUS mb_status;
  T_MC_RCA phys_rca = 0;

  MC_SEND_TRACE("MC: mc_op_write_csd_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    if (retval == RV_OK)
    {
      retval = mc_check_parm_field(msg_p->field);
    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
      retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);

    }

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Allocate CSD memory */
      mb_status = rvf_get_buf (mc_env_ctrl_blk_p->prim_mb_id,
                               MC_CSD_SIZE,
                               (void **) &csd_p);

      if (mb_status != RVF_RED) /* Memory allocation success */
      {
        /** Select card */
         //retval = mc_cmd_setup_card(phys_rca,csd_p,&card_status);
         retval = mc_cmd_setup_card(csd_p,&card_status);

        if (retval == RV_OK)
        {
          /** Assign write CSD parameter */
          retval = mc_cmd_write_csd (csd_p,
                                      msg_p->field,
                                      msg_p->value,
                                      &card_status);
        }

        /** Free CSD buffer */
        MC_FREE_BUF(csd_p);
      }

      /** Send card to Stand By status */
   //   mc_cmd_deselect_card(&card_status);

    } else {
      MC_SEND_TRACE ("MC mc_op_write_csd_req: Could not allocate memory",
                       RV_TRACE_LEVEL_ERROR);
      retval = RV_INTERNAL_ERR;
    }

    /** Compose message */
    retval = mc_send_write_csd_rsp (retval,
                                     card_status,
                                     msg_p->subscriber);
  } else {
    MC_SEND_TRACE ("MC mc_op_write_csd_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  /** After writing the CSD retreive the new CSD value       */
  /** This is to assure that the card_infos->csd of the used */
  /** mc/sd card always contains the latest CSD value       */
  if (retval == RV_OK)
  {
    /** Wait until card is in standby state */
    //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
    retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);

  }

  //Now the functional code
  if (retval == RV_OK)
  {
    /** get CSD value*/
    //retval = mc_cmd_send_csd (phys_rca, &mc_env_ctrl_blk_p->card_infos[msg_p->rca - 1]->csd[0]);
	retval = mc_cmd_send_csd (&mc_env_ctrl_blk_p->card_infos[msg_p->rca - 1]->csd[0]);
  }

  return retval;
}

/**
 * function: mc_op_get_prot_req
 */
T_RV_RET mc_op_get_prot_req(T_MC_GET_PROTECT_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  UINT32 card_status = 0;
  UINT32 wr_prot_grps = 0;
  UINT16 blk_len;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_get_prot_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


   if(retval == RV_OK)
    {
       /**Check wp group boundaries*/
       retval = mc_check_data_addres(msg_p->rca, msg_p->wr_prot_group);

    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
      retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);

    }

    //Now the functional code
    if (retval == RV_OK)
    {
      /* Read block length from CSD register buffers in card info*/
      blk_len = 1 << (CSD_GET_READ_BL_LEN(mc_env_ctrl_blk_p->card_infos[msg_p->rca - 1]->csd));

      //retval = mc_cmd_setup_card(phys_rca,NULL,&card_status);
      retval = mc_cmd_setup_card(NULL,&card_status);

      if (retval == RV_OK)
      {
        /** Get card status */
        //retval = mc_cmd_get_write_protect(phys_rca,
        //                                  blk_len,
        //                                  msg_p->wr_prot_group,
        //                                  &wr_prot_grps,
        //                                  &card_status);
       retval = mc_cmd_get_write_protect(
                                          blk_len,
                                          msg_p->wr_prot_group,
                                          &wr_prot_grps,
                                          &card_status);

      }

      /** Send card to Stand By status */
   //   mc_cmd_deselect_card(&card_status);
    }

    /** Send response */
    retval = mc_send_get_protect_rsp(retval,
                                      card_status,
                                      wr_prot_grps,
                                      msg_p->subscriber);

  } else {
    MC_SEND_TRACE ("MC mc_op_get_prot_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}
/**
 * function: mc_op_set_prot_req
 */
T_RV_RET mc_op_set_prot_req(T_MC_SET_PROTECT_REQ_MSG *msg_p)
{
  T_RV_RET retval      = RV_OK;
  UINT32   card_status = 0;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_set_prot_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    if(retval == RV_OK)
    {
       /**Check wp group boundaries*/
       retval = mc_check_data_addres(msg_p->rca, msg_p->wr_prot_group);
    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
     // retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);


    }

  //Now the functional code
    if (retval == RV_OK)
    {
      /** Select card */
      //retval = mc_cmd_setup_card(phys_rca,NULL,&card_status);
     // retval = mc_cmd_setup_card(NULL,&card_status);
      if (retval == RV_OK)
      {
        /** Get card status */
       // retval = mc_cmd_set_write_protect(phys_rca,msg_p->wr_prot_group,&card_status);
        retval = mc_cmd_set_write_protect(msg_p->wr_prot_group,&card_status);

      }

      /** Send card to Stand By status */
    //  mc_cmd_deselect_card(&card_status);
    }

    /** Send response */
    retval = mc_send_set_protect_rsp (retval,
                                       card_status,
                                       msg_p->subscriber);

  } else {
    MC_SEND_TRACE ("MC mc_op_set_prot_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}

/**
 * function: mc_op_clr_prot_req
 */
T_RV_RET mc_op_clr_prot_req(T_MC_CLR_PROTECT_REQ_MSG *msg_p)
{
  T_RV_RET retval      = RV_OK;
  UINT32   card_status = 0;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_clr_prot_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


   if(retval == RV_OK)
    {
       /**Check wp group boundaries*/
       retval = mc_check_data_addres(msg_p->rca, msg_p->wr_prot_group);
    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
     retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);

    }

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Select card */
      //retval = mc_cmd_setup_card(phys_rca,NULL,&card_status);
      retval = mc_cmd_setup_card(NULL,&card_status);


      if (retval == RV_OK)
      {
        /** Get card status */
        //retval = mc_cmd_clr_write_protect(phys_rca,
        //                                   msg_p->wr_prot_group,
        //                                   &card_status);
        retval = mc_cmd_clr_write_protect(
                                           msg_p->wr_prot_group,
                                           &card_status);

      }

      /** Send card to Stand By status */
   //   mc_cmd_deselect_card(&card_status);
    }

    /** Send response */
    retval = mc_send_clr_protect_rsp(retval,
                                      card_status,
                                      msg_p->subscriber);

  } else {
    MC_SEND_TRACE ("MC mc_op_clr_prot_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}


/**
 * Will subscribe a subscriber to certain card events
 *
 * This function will subscribe the subscriber to the desired events
 *
 * @param   msg_p     Reference to message
 *
 *
 * @return  RV_OK
 *          RV_INTERNAL_ERR
 */

/*@{*/
T_RV_RET mc_op_send_notification(T_MC_NOTIFICATION_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  /** Check content parameter*/
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber))
  {
    /** subscriber_id is present, save desired event*/
    /** Check if subscriber_infos[x] pointer is filled*/
    if(msg_p->events <= (MC_EVENT_INSERTION | MC_EVENT_REMOVAL))
    {
      if(mc_env_ctrl_blk_p->subscriber_infos[msg_p->subscriber] != NULL)
      {
        mc_env_ctrl_blk_p->subscriber_infos[msg_p->subscriber]->events = msg_p->events;
      }
      else
      {
        MC_SEND_TRACE ("MC mc_op_send_notification: No subscriber_info available",
                       RV_TRACE_LEVEL_ERROR);
        retval = RV_INTERNAL_ERR;
      }
    }
    else
    {
      retval = RV_INVALID_PARAMETER;
    }

     /** Send response */
    retval = mc_send_notification_rsp(retval,msg_p->subscriber);
  }
  else
  {
    MC_SEND_TRACE ("MC mc_op_send_notification: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }

  return retval;

}
/*@}*/

/**
 * Handle a card insert/remove action and will send an event
 *
 * This function will handle a card insert action and will
 * send all subscribers to the MC_EVENT_INSERTION event a message
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
T_RV_RET mc_op_handle_card_event(T_MC_EVENTS event)
{
  T_RV_RET  retval = RV_OK;
  UINT16 i;

  /** Check if environment is available */
  if((mc_env_ctrl_blk_p == NULL) || mc_env_ctrl_blk_p->initialised != TRUE)
  {
        MC_SEND_TRACE ("MC mc_op_handle_card_insert: no environment",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
  }
  else
  {
    if(event == MC_EVENT_INSERTION)
    {
      /** Reset host controller */
      mc_env_ctrl_blk_p->conf.clk_div = MC_ID_CLK_DIV;
     // mc_cmd_init_host(&mc_env_ctrl_blk_p->conf ,
     //                 mc_env_ctrl_blk_p->irq);

      /** Call MC commands */
      retval = mc_cmd_init_stack();

      if (retval == RV_OK)
      {
        /** MC card(s) attached */
        mc_env_ctrl_blk_p->state = MC_STATE_IDLE_ATTACHED;

        /** Reset host controller */
        mc_env_ctrl_blk_p->conf.clk_div = MC_NORM_CLK_DIV;
      //  mc_cmd_init_host(&mc_env_ctrl_blk_p->conf ,
      //                   mc_env_ctrl_blk_p->irq);
      }
    }
    else
    {
      //it is an removal. Clean up
     /** Reset stack admin */
      for(i = 0;i < MC_MAX_STACK_SIZE;i++)
      {
        if(mc_env_ctrl_blk_p->card_infos[i] != NULL)
        {
          MC_FREE_BUF(mc_env_ctrl_blk_p->card_infos[i]);
          mc_env_ctrl_blk_p->card_infos[i] = NULL;
        }
      }
      mc_env_ctrl_blk_p->reserved_card_id = 0;

      /** set current driver state */
      mc_env_ctrl_blk_p->state = MC_STATE_IDLE_DETACHED;
    }

    /** send notifications, look for subscribers*/
    for (i = 0; i < MC_MAX_SUBSCRIBER;i++)
    {
      if(((mc_env_ctrl_blk_p->reserved_subscriber_id >> i)& 0x01) == 0x01)
      {
        /** found subscriber*/
        /** check for this event*/
        if(mc_env_ctrl_blk_p->subscriber_infos[i] != NULL)
        {
          if((mc_env_ctrl_blk_p->subscriber_infos[i]->events & MC_EVENT_REMOVAL)
            ||(mc_env_ctrl_blk_p->subscriber_infos[i]->events & MC_EVENT_INSERTION))
          {
            mc_send_event_ind_msg(event,i);
            MC_SEND_TRACE_PARAM("MC mc_op_handle_card_event: to subscriber",i,
                          RV_TRACE_LEVEL_DEBUG_LOW);
          }
        }
        else
        {
          MC_SEND_TRACE("MC mc_op_handle_card_insert: no subscriber info",
                          RV_TRACE_LEVEL_ERROR);
          retval = RV_INTERNAL_ERR;
        }


      }
    }
  }
  return retval;
}
/*@}*/


/**
 * Retrieves the SD card status
 *
 * This function will retrieve the SD card status
 *
 * @param   msg_p Reference to message
 *
 * @return  RV_OK
 *          RV_INTERNAL_ERR
 */

/*@{*/
T_RV_RET mc_op_sd_card_status_req(T_MC_SD_CARD_STATUS_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  UINT32 card_status = 0;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_sd_card_status_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
   // retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);


    /** check for valid reference used*/
    if(msg_p->sd_status_p == NULL)
    {
      retval = RV_INVALID_PARAMETER;

    }

    if (retval == RV_OK)
    {
      /** Wait until card is in standby state */
      //retval =  mc_wait_card_state(phys_rca, MC_STATE_STDY, TRUE);
     // retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);

    }

    //Now the functional code
    if (retval == RV_OK)
    {
        /** Select card */
        //retval = mc_cmd_setup_card(phys_rca,NULL,&card_status);
        //retval = mc_cmd_setup_card(NULL,&card_status);
 if (retval == RV_OK)
        {
          /** Get card status */
          //retval = mc_cmd_sd_send_status(phys_rca, msg_p->sd_status_p,&card_status);
          retval = mc_cmd_sd_send_status(msg_p->sd_status_p,&card_status);
        }

        /** Send card to Stand By status */
       // mc_cmd_deselect_card(&card_status);

    }
  }
  else
  	{
    MC_SEND_TRACE ("MC mc_op_sd_card_status_req: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
    }

  /** Send response */
  retval = mc_send_sd_card_status_rsp(retval,
                                       msg_p->sd_status_p,
                                       msg_p->subscriber);

  return retval;

}
/*@}*/

/**
 * Retrieves the scr register
 *
 * This function will retrieve the scr register
 *
 * @param   msg_p Reference to message
 *
 * @return  RV_OK
 *          RV_INTERNAL_ERR
 */

/*@{*/
T_RV_RET mc_op_read_scr_req_msg(T_MC_READ_SCR_REQ_MSG *msg_p)
{
  T_RV_RET retval = RV_OK;
  UINT32 card_status = 0;
  T_MC_RCA phys_rca;

  MC_SEND_TRACE("MC: mc_op_sd_card_status_req called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if subscriber id is correct */
  if ( mc_env_ctrl_blk_p->reserved_subscriber_id & (1<<msg_p->subscriber) )
  {
    //Parameter checks
    retval =  mc_check_and_get_phys_rca(msg_p->rca,&phys_rca);

	if(retval!=RV_OK)
	{
	  retval = RV_INVALID_PARAMETER;
	}


    /** check for valid reference used*/
    if(msg_p->scr_p == NULL)
    {
      retval = RV_INVALID_PARAMETER;

    }

    //if (retval == RV_OK)
    //{
      /** Wait until card is in standby state */
     // retval =  mc_wait_card_state(MC_STATE_STDY, TRUE);


    //}

    //Now the functional code
    if (retval == RV_OK)
    {
      /** Select card */
      //retval = mc_cmd_setup_card(phys_rca,NULL,&card_status);
      //retval = mc_cmd_setup_card(NULL,&card_status);

      if (retval == RV_OK)
      {
        /** Get card status */
      //retval = mc_cmd_send_scr(phys_rca,msg_p->scr_p,&card_status);
      retval = mc_cmd_send_scr(msg_p->scr_p,&card_status);

      }

      /** Send card to Stand By status */
   //   mc_cmd_deselect_card(&card_status);
    }
  } else {
    MC_SEND_TRACE ("MC mc_op_read_scr_req_msg: Unknown subscriber",
                     RV_TRACE_LEVEL_ERROR);
    retval = RV_INVALID_PARAMETER;
  }

  /** Send response */
  retval = mc_send_read_scr_rsp(retval,msg_p->scr_p,msg_p->subscriber);

  return retval;

}
/*@}*/
