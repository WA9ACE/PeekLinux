/**
 * @file  rfs_handle_message.c
 *
 * RFS handle_msg function, which is called when the SWE
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
 *  1/23/2004  ()   Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */


#include "rfs/rfs_i.h"
#include "rfs/rfs_process_internal_msg.h"

#include "gbi/gbi_message.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsnand/rfsnand_message.h"


/*
 *  Internal function prototypes
 */
static T_RFS_RETURN rfs_process_msg(T_RV_HDR *msg_p);

/**
 * Called every time the SW entity is in WAITING state
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param msg_p Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN rfs_handle_message (T_RV_HDR *msg_p)
{
  T_RVM_RETURN ret = RVM_OK;

  RFS_SEND_TRACE("RFS: rfs_handle_message called",RV_TRACE_LEVEL_DEBUG_LOW);

  if (msg_p != NULL)
  {
      /* Process message */
      if(rfs_process_msg(msg_p) != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: Unable to process message", RV_TRACE_LEVEL_ERROR);
        ret = RVM_MEMORY_ERR;
      }

      /* Free message */
      if (rvf_free_buf(msg_p) != RVF_OK)
      {
        RFS_SEND_TRACE("RFS: Unable to free message", RV_TRACE_LEVEL_ERROR);

        ret = RVM_MEMORY_ERR;
      }
    }

  return ret;
}


/*
 *  Internal function: process message
 */
static T_RFS_RETURN rfs_process_msg(T_RV_HDR *msg_p)
{
  T_RFS_RETURN ret = RFS_OK;

  /*find out what message is recieved*/
  switch (msg_p->msg_id)
  {
    /* Messages to be handled by the Media manager */
    case RFS_I_STARTUP_MSG:
    case GBI_EVENT_IND_MSG:
    case GBI_MEDIA_INFO_RSP_MSG:
    case GBI_PARTITION_INFO_RSP_MSG:
    case RFSFAT_SET_PARTITION_TABLE_RSP_MSG:
    case RFSNAND_SET_PARTITION_TABLE_RSP_MSG:

      RFS_SEND_TRACE ("RFS Media manager message received", RV_TRACE_LEVEL_DEBUG_LOW);
      ret = rfs_media_manager_process_msg(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_media_manager_process_msg failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    /* internal messages */
    case RFS_CLOSE_REQ_MSG:
      RFS_SEND_TRACE ("RFS Close message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_close_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_close_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_WRITE_REQ_MSG:
      RFS_SEND_TRACE ("RFS Write message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_write_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_write_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_READ_REQ_MSG:
      RFS_SEND_TRACE ("RFS Read message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_read_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_read_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_LSEEK_REQ_MSG:
      RFS_SEND_TRACE ("RFS Lseek message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_lseek_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_lseek_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_FCHMOD_REQ_MSG:
      RFS_SEND_TRACE ("RFS Fchmod message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_fchmod_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_fchmod_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_FSTAT_REQ_MSG:
      RFS_SEND_TRACE ("RFS Fstat message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_fstat_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_fstat_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_FSYNC_REQ_MSG:
      RFS_SEND_TRACE ("RFS Fsync message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_fsync_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_fsync_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    /* External/internal messages */

    case RFS_OPEN_REQ_MSG:
      RFS_SEND_TRACE ("RFS Open message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_open_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_open_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

     case RFS_CHMOD_REQ_MSG:
      RFS_SEND_TRACE ("RFS Chmod message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_chmod_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_chmod_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_STAT_REQ_MSG:
      RFS_SEND_TRACE ("RFS Stat message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_stat_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_stat_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_REMOVE_REQ_MSG:
      RFS_SEND_TRACE ("RFS Remove message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_remove_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_remove_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_RENAME_REQ_MSG:
      RFS_SEND_TRACE ("RFS Rename message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_rename_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_rename_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_MKDIR_REQ_MSG:
      RFS_SEND_TRACE ("RFS Mkdir message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_mkdir_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_mkdir_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_RMDIR_REQ_MSG:
      RFS_SEND_TRACE ("RFS Rmdir message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_rmdir_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_rmdir_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_OPENDIR_REQ_MSG:
      RFS_SEND_TRACE ("RFS Opendir message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_opendir_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_opendir_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_READDIR_REQ_MSG:
      RFS_SEND_TRACE ("RFS Readdir message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_readdir_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_readdir_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_PREFORMAT_REQ_MSG:
      RFS_SEND_TRACE ("RFS Preformat message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_preformat_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_preformat_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_FORMAT_REQ_MSG:
      RFS_SEND_TRACE ("RFS Format message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_format_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_format_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

     case RFS_GETATTRIB_REQ_MSG:
      RFS_SEND_TRACE ("RFS RFS_GETATTRIB_REQ_MSG message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_getattrib_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_getattrib_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

     case RFS_SETATTRIB_REQ_MSG:
      RFS_SEND_TRACE ("RFS RFS_SETATTRIB_REQ_MSG message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_setattrib_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_setattrib_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;	

case RFS_GETDATETIME_REQ_MSG:
      RFS_SEND_TRACE ("RFS RFS_GETDATETIME_REQ_MSG message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_getdatetime_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_getdatetime_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

case RFS_CLOSEDIR_REQ_MSG:
      RFS_SEND_TRACE ("RFS RFS_CLOSEDIR_REQ_MSG message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_closedir_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_closedir_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_MOUNT_UNMOUNT_REQ_MSG:
      RFS_SEND_TRACE ("RFS RFS_MOUNT_UNMOUNT_REQ_MSG message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret =(T_RFS_RETURN) rfs_mount_unmount_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_mount_unmount_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    case RFS_SET_LABEL_REQ_MSG:
      RFS_SEND_TRACE ("RFS Set volume label message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_setlabel_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_setlabel_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;
		  
    case RFS_GET_LABEL_REQ_MSG:
      RFS_SEND_TRACE ("RFS Get volume label message received", RV_TRACE_LEVEL_DEBUG_LOW);

      ret = rfs_getlabel_req(msg_p);
      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_getlabel_req failed", RV_TRACE_LEVEL_ERROR);
      }

      break;
	  
	  /* TODO: all cores should return the same ready response !!! */

     case FSCORE_READY_RSP_MSG:
      RFS_SEND_TRACE ("RFS core ready response message received", RV_TRACE_LEVEL_DEBUG_LOW);
			
	  ret = rfs_handle_core_ready_response(msg_p);

      if (ret != RFS_OK)
      {
        RFS_SEND_TRACE("RFS: rfs_handle_core_ready_response failed", RV_TRACE_LEVEL_ERROR);
      }

      break;

    default:
      /* Unknow message has been received */
      RFS_SEND_TRACE("RFS: RFS requests an unknown message", RV_TRACE_LEVEL_ERROR);
      ret = RFS_INTERNAL_ERR;
      break;
  }

  return ret;
}


/* Commented: probably not used in the future
 *
 *
 *  Internal function: store message in return queue, raise an event
 */

/*
static void return_queue_callback (void *msg_p)
{
  T_FS_CORE_READY_MSG *resp_msg_p;

  RFS_SEND_TRACE("RFS: return_queue_callback", RV_TRACE_LEVEL_DEBUG_LOW);

  if (RVF_OK != rvf_enqueue (&rfs_env_ctrl_blk_p->return_queue.queue_obj, msg_p)
      || RVF_OK != rvf_send_event (rfs_env_ctrl_blk_p->return_queue.addr_id,
                                   rfs_env_ctrl_blk_p->return_queue.event))
  {
    RFS_SEND_TRACE ("RFS RETURN QUEUE lost message", RV_TRACE_LEVEL_ERROR);
  }

  resp_msg_p = (T_FS_CORE_READY_MSG *) msg_p;

  // find out what message is recieved
  switch (resp_msg_p->hdr.msg_id)
  {
    // internal messages
    case FS_CORE_READY_MSG:

      RFS_SEND_TRACE ("RFS FS_CORE_READY_MSG", RV_TRACE_LEVEL_DEBUG_LOW);

      global_result = resp_msg_p->result;

      break;
    default:
      // Unknow message has been received
      RFS_SEND_TRACE("RFS: RFS requests an unknown message", RV_TRACE_LEVEL_ERROR);

      break;
  }

  rfs_unlock_mutex_A();

  // No use of any rfs_return_queue_get_msg function, dequeue needed before
  // De-allocating message memory

  rvf_dequeue (&rfs_env_ctrl_blk_p->return_queue.queue_obj);

  if (rvf_free_msg ((T_RV_HDR *) msg_p) != RVF_OK)
  {
    RFS_SEND_TRACE("RFS return_queue_callback: could not free response msg",
                    RV_TRACE_LEVEL_WARNING);
  }

  return;
}
*/

/*
 *  Function: init return queue object
 */

/*
void rfs_return_queue_init (T_RVF_G_ADDR_ID   rq_addr_id,
                            UINT16            rq_event,
                            T_RV_RETURN_PATH  *path_to_return_queue_p)
{
  RFS_SEND_TRACE("RFS: rfs_return_queue_init", RV_TRACE_LEVEL_DEBUG_LOW);

  rfs_env_ctrl_blk_p->return_queue.addr_id            = rq_addr_id;
  rfs_env_ctrl_blk_p->return_queue.event              = rq_event;
  rfs_env_ctrl_blk_p->return_queue.queue_obj.p_first  = NULL;
  rfs_env_ctrl_blk_p->return_queue.queue_obj.p_last   = NULL;
  rfs_env_ctrl_blk_p->return_queue.queue_obj.count    = 0;
  path_to_return_queue_p->addr_id                     = RVF_INVALID_ADDR_ID;
  path_to_return_queue_p->callback_func               = return_queue_callback;
}
*/

/*
 *  Following function is not used:
 *  Function: get a message from the return queue
 */

/*
void *rfs_return_queue_get_msg (void)
{
  RFS_SEND_TRACE("RFS: rfs_return_queue_get_msg", RV_TRACE_LEVEL_DEBUG_LOW);

  if (rfs_env_ctrl_blk_p->return_queue.queue_obj.count == 0)
  {
    RFS_SEND_TRACE ("Wait for message", RV_TRACE_LEVEL_ERROR);
    RFS_SEND_TRACE_PARAM("event", rfs_env_ctrl_blk_p->return_queue.event ,RV_TRACE_LEVEL_DEBUG_LOW);

    // no messages queued, block until msg available
    rvf_wait (rfs_env_ctrl_blk_p->return_queue.event, 0);
  }

  return rvf_dequeue (&rfs_env_ctrl_blk_p->return_queue.queue_obj);
}
*/
