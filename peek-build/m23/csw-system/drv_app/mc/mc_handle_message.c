/**
 * @file  mc_handle_message.c
 *
 * MC handle_msg function, which is called when the SWE
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
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */


#include "mc/mc_i.h"
#include "mc/mc_operations.h"

static void *mc_return_queue_get_msg (void);


/**
 * Called by the message handler when the driver is in
 * DETACHED state. The driver is in DETACHED state when the hardware is
 * initialised, but there is no MC card attached to the controller.
 *
 */
T_RV_RET mc_state_initialised (T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC: mc_state_initialised called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if(msg_p->msg_id == MC_INIT_MSG)
  {
    /** Starting to initialse the MC/SD driver*/
    mc_env_ctrl_blk_p->state = MC_STATE_INIT_DRIVER;

    /** Put initialised to TRUE to enable the HISR event handler*/
    mc_env_ctrl_blk_p->initialised = TRUE;

    /** Initialise the driver, state will be attached when cards have been found*/

    retval = mc_op_init_driver();

    if(retval != RV_OK)
    {
      /**Not succesfull, no cards available*/
      MC_SEND_TRACE("MC: mc_handle_message init failed",RV_TRACE_LEVEL_ERROR);
      mc_env_ctrl_blk_p->state = MC_STATE_IDLE_DETACHED;
    }
    else
    {
      /**Succesfull, cards available*/
      mc_env_ctrl_blk_p->state = MC_STATE_IDLE_ATTACHED;
    }

  }
  else
  {
    /**wrong message for this state*/
    MC_SEND_TRACE("MC: STATE_INITIALISED, expecting MC_INIT_MSG",RV_TRACE_LEVEL_ERROR);
  }
  return retval;
}




/**
 * Called by the message handler when the driver is in
 * DETACHED state. The driver is in DETACHED state when the hardware is
 * initialised, but there is no MC card attached to the controller.
 *
 */
T_RV_RET mc_state_idle_detached (T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC: mc_state_detached called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if (msg_p->msg_id == MC_UPDATE_ACQ_REQ_MSG)
  {
    /** Handle update acquisition request */
    retval = mc_op_update_acq_req((T_MC_UPDATE_ACQ_REQ_MSG *) msg_p);

    if (retval == RV_OK)
    {
      /** Promote to next group */
      MC_SEND_TRACE("MC mc_state_detached: promote to attached state",
                      RV_TRACE_LEVEL_DEBUG_LOW);
      mc_env_ctrl_blk_p->state = MC_STATE_IDLE_ATTACHED;
    } else {
      MC_SEND_TRACE("MC mc_state_detached: acquisition failed",
                      RV_TRACE_LEVEL_ERROR);
    }
  }
  else
  {
    /** Driver not ready */
    MC_SEND_TRACE("MC mc_state_detached: driver not able to handle request",
                    RV_TRACE_LEVEL_ERROR);
    retval = mc_op_driver_not_ready(msg_p);
  }

  return retval;
}

/**
 * Called by the message handler when the driver is in
 * ATTACHED state. The driver is in ATTACHED state
 * when a MC card is attached and assigned a RCA (in ready state).
 * The card is ready to accept commands from the controller.
 *
 */
T_RV_RET mc_state_idle_attached (T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC: mc_state_attached called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /** Check message ID */
  switch (msg_p->msg_id)
  {
    case MC_READ_REQ_MSG:
      /** Handle read request */
      retval = mc_op_read_req((T_MC_READ_REQ_MSG *) msg_p);
      break;

    case MC_WRITE_REQ_MSG:
      /** Handle write request */
      retval = mc_op_write_req((T_MC_WRITE_REQ_MSG *) msg_p);
      break;

    case MC_ERASE_GROUP_REQ_MSG:
      /** Handle erase group request */
      retval = mc_op_erase_group_req((T_MC_ERASE_GROUP_REQ_MSG *) msg_p);
      break;

    case MC_SET_PROTECT_REQ_MSG:
      /** Handle set write protection request */
	  rvf_send_trace("mc_op_set_prot_req not supported",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = mc_op_set_prot_req((T_MC_SET_PROTECT_REQ_MSG *) msg_p);
      break;

    case MC_CLR_PROTECT_REQ_MSG:
      /** Handle clear write protection request */
	  rvf_send_trace("mc_op_clr_prot_req not supported",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = mc_op_clr_prot_req((T_MC_CLR_PROTECT_REQ_MSG *) msg_p);
      break;

    case MC_GET_PROTECT_REQ_MSG:
      /** Handle get write protection request */
	  rvf_send_trace("mc_op_get_prot_req not supported",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = mc_op_get_prot_req((T_MC_GET_PROTECT_REQ_MSG *) msg_p);
      break;

    case MC_CARD_STATUS_REQ_MSG:
      /** Handle status request */
      retval = mc_op_card_status_req((T_MC_CARD_STATUS_REQ_MSG *) msg_p);
      break;

    case MC_UPDATE_ACQ_REQ_MSG:
      /** Handle update acquisition request */
      retval = mc_op_update_acq_req((T_MC_UPDATE_ACQ_REQ_MSG *) msg_p);
      break;

    case MC_RESET_REQ_MSG:
      /** Handle reset request */
      retval = mc_op_reset_req((T_MC_RESET_REQ_MSG *) msg_p);
      break;

    case MC_READ_OCR_REQ_MSG:
      /** Handle read OCR request */
      retval = mc_op_read_ocr_req((T_MC_READ_OCR_REQ_MSG *) msg_p);
      break;

    case MC_READ_CID_REQ_MSG:
      /** Handle read CID request */
      retval = mc_op_read_cid_req((T_MC_READ_CID_REQ_MSG *) msg_p);
      break;

    case MC_READ_CSD_REQ_MSG:
      /** Handle read CSD request */
      retval = mc_op_read_csd_req((T_MC_READ_CSD_REQ_MSG *) msg_p);
      break;

    case MC_WRITE_CSD_REQ_MSG:
      /** Handle write CSD request */
	  rvf_send_trace("mc_op_write_csd_req not supported",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = mc_op_write_csd_req((T_MC_WRITE_CSD_REQ_MSG *) msg_p);
      break;

    case MC_SD_CARD_STATUS_REQ_MSG:
      /** Handle SD card status request */
      retval = mc_op_sd_card_status_req((T_MC_SD_CARD_STATUS_REQ_MSG *) msg_p);
      break;

    case MC_READ_SCR_REQ_MSG:
      /** Handle SD card status request */
      retval = mc_op_read_scr_req_msg((T_MC_READ_SCR_REQ_MSG *) msg_p);
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
 * Called every time the SW entity is in WAITING state
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param msg_p Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN mc_handle_message (T_RV_HDR *msg_p)
{
  T_RVM_RETURN retval=RVM_OK;

  MC_SEND_TRACE("MC: mc_handle_message called",RV_TRACE_LEVEL_DEBUG_LOW);

  if (msg_p != NULL)
  {
 // Commented for compiling MMC in Locosto
    /** When in STATE_INITIALISED only accept MC_INIT_MSG*/
    if(mc_env_ctrl_blk_p->state == MC_STATE_INITIALISED)
    {
      retval = mc_state_initialised(msg_p);
    }
    else if((mc_env_ctrl_blk_p == NULL) || (mc_env_ctrl_blk_p->initialised != TRUE))
    {
      /** when not initialised then only accept init message*/
      MC_SEND_TRACE("MC:not initialised yet, waiting",RV_TRACE_LEVEL_ERROR);
      rvf_delay(RVF_MS_TO_TICKS(10));
    }
    else
    {
      switch (msg_p->msg_id)
      {
        case MC_SUBSCRIBE_REQ_MSG:
          /** You can always subscribe and unsubscribe and us notification*/
          retval = mc_op_subscribe((T_MC_SUBSCRIBE_REQ_MSG *) msg_p);
          if (retval != RV_OK)
          {
                MC_SEND_TRACE("MC: mc_op_subscribe failed",
                            RV_TRACE_LEVEL_ERROR);
          }
          break;

        case MC_UNSUBSCRIBE_REQ_MSG:
          retval = mc_op_unsubscribe((T_MC_UNSUBSCRIBE_REQ_MSG *) msg_p);
          if (retval != RV_OK)
          {
                MC_SEND_TRACE("MC: mc_op_unsubscribe failed",
                            RV_TRACE_LEVEL_ERROR);
          }
          break;

        case MC_NOTIFICATION_REQ_MSG:
          retval = mc_op_send_notification((T_MC_NOTIFICATION_REQ_MSG *) msg_p);
          if (retval != RV_OK)
          {
                MC_SEND_TRACE("MC: mc_op_send_notification failed",
                            RV_TRACE_LEVEL_ERROR);
          }
          break;

#ifdef T_FLASH_HOT_SWAP

	case MC_CARD_INS_MSG:
               mc_op_handle_card_event(MC_EVENT_INSERTION);
	     break;

	case MC_CARD_REM_MSG:
				mc_op_handle_card_event(MC_EVENT_REMOVAL);
          break;

#endif

          default:
          /** handle all other messages according to state*/
          switch (mc_env_ctrl_blk_p->state)
          {
            case MC_STATE_IDLE_DETACHED:
              retval = mc_state_idle_detached (msg_p);
              if (retval != RV_OK )
              {
                MC_SEND_TRACE("MC: mc_state_idle failed",
                              RV_TRACE_LEVEL_ERROR);
              }
              break;

            case MC_STATE_IDLE_ATTACHED:
              retval = mc_state_idle_attached (msg_p);
              if (retval != RV_OK )
              {
                MC_SEND_TRACE("MC: mc_state_data_trans failed",
                                RV_TRACE_LEVEL_ERROR);
              }
              break;

            case MC_STATE_UNINITIALISED:
            case MC_STATE_INITIALISED:
            case MC_STATE_INIT_DRIVER:
            case MC_STATE_HANDLE_REQ:
            case MC_STATE_STOPPED:
            case MC_STATE_KILLED:
            default:
              /* State does not support messages*/
               MC_SEND_TRACE("MC: does not support messages in this state",
                               RV_TRACE_LEVEL_ERROR);
              break;
          }/** end of switch (mc_env_ctrl_blk_p->state)*/
      }/** end of  switch (msg_p->msg_id) */
    }
 //#if 0 // Commented for compiling MMC in Locosto

    /* Free message */
    if (rvf_free_buf(msg_p) != RVF_OK)
    {
      MC_SEND_TRACE("MC: Unable to free message",
                      RV_TRACE_LEVEL_ERROR);
      return RVM_MEMORY_ERR;
    }
  }

  return retval;
}



/**********************************************************************/
/*  SECOND MAILBOX                                                    */
/**********************************************************************/

/* return queue structure, used to store responses from lower level driver */
static struct
{
  T_RVF_G_ADDR_ID addr_id;
  UINT16 event;
  T_RVF_BUFFER_Q queue_obj;
}
mc_return_queue;

/* store message in return queue, raise an event */
static void mc_return_queue_callback (void *msg_p)
{
  if (RVF_OK != rvf_enqueue (&mc_return_queue.queue_obj, msg_p) ||
      RVF_OK != rvf_send_event (mc_return_queue.addr_id, mc_return_queue.event))
    {
      MC_SEND_TRACE ("MC RETURN QUEUE lost message", RV_TRACE_LEVEL_ERROR);
    }
}

/* get a message from the return queue */
static void *mc_return_queue_get_msg (void)
{
  if (mc_return_queue.queue_obj.count == 0)
    {
      /* no messages queued, block until msg available */
      rvf_wait (mc_return_queue.event, 0);
    }
  return rvf_dequeue (&mc_return_queue.queue_obj);
}

/* init return queue object */
void mc_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
                        T_RV_RETURN_PATH * path_to_mc_return_queue_p)
{
  mc_return_queue.addr_id = rq_addr_id;
  mc_return_queue.event = rq_event;
  mc_return_queue.queue_obj.p_first = NULL;
  mc_return_queue.queue_obj.p_last = NULL;
  mc_return_queue.queue_obj.count = 0;
  path_to_mc_return_queue_p->addr_id = RVF_INVALID_ADDR_ID;
  path_to_mc_return_queue_p->callback_func = mc_return_queue_callback;
}
