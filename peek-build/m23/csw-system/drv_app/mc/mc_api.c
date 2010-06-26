/**
 * @file  mc_api.c
 *
 * API for MC SWE.
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

#include "mc/mc_api.h"
#include "mc/mc_commands.h"


/**
 * Subscribe
 *
 * Detailled description.
 * This functions shall be used by a client to subscribe to the MC-driver.
 * After registration the client is able to use the driver services like
 * reading and writing data.
 *
 * @param   subscriber_p  Subscriber identification value, which shall be
 *                        allocated by the client and is filled by the driver.
 *          return_path   The return path of the client.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory. Client could not be subscribed
 *                        because maximum number of clients have been reached.
 */
T_RV_RET mc_subscribe (T_MC_SUBSCRIBER *subscriber_p,
                        T_RV_RETURN return_path)
{
  T_MC_SUBSCRIBE_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_subscribe (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }
    /* reserve message buffer */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_SUBSCRIBE_REQ_MSG),
                         MC_SUBSCRIBE_REQ_MSG,
                         (T_RV_HDR **) &msg_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_subscribe: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    msg_p->return_path  = return_path;
    msg_p->subscriber_p = subscriber_p;

    /* Send message mailbox */
    retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Unsubscribe
 *
 * Detailled description.
 * This function shall be used by a client to unsubscribe from the driver.
 * After calling this function the client will no longer able to use the
 * driver services.
 *
 * @param   subscriber_p  Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_INVALID_PARAMETER The subscriber id is invalid.
 */
T_RV_RET mc_unsubscribe (T_MC_SUBSCRIBER *subscriber_p)
{
  T_MC_UNSUBSCRIBE_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_unsubscribe (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check if subscriber is valid */
  if (subscriber_p == NULL)
  {
    MC_SEND_TRACE ("MC mc_unsubscribe: invalid subscriber pointer",
                    RV_TRACE_LEVEL_WARNING);
    retval = RV_INVALID_PARAMETER;
  } else {
    if ((*subscriber_p <= MC_MAX_SUBSCRIBER) &&
        (mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << *subscriber_p)) )
    {
      /* compose message */
      /* reserve message buffer */
      if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                           sizeof (T_MC_UNSUBSCRIBE_REQ_MSG),
                           MC_UNSUBSCRIBE_REQ_MSG,
                           (T_RV_HDR **) &msg_p) == RVF_RED)
      {
        MC_SEND_TRACE ("MC mc_unsubscribe: out of memory",
                        RV_TRACE_LEVEL_WARNING);
        return RV_MEMORY_ERR;
      }

      msg_p->subscriber_p = subscriber_p;

      /* Send message mailbox */
      retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

    } else {
      MC_SEND_TRACE ("MC mc_unsubscribe: subscriber ID out of range",
                       RV_TRACE_LEVEL_WARNING);
      retval = RV_INVALID_PARAMETER;
    }
  }

  return retval;
}


/**
 * @name usb_register_notifcation
 *
 * This function must be called by a Application to subscribe for USB event notification.
 *
 *
 * @param	T_USB_EVENT_CALLBACK: callback	the callback function pointer. This function
 *										    will be called for notifying USB events.
 * @param	void:				  ctx		This is the context information provided with callback.
 *											This will be provided transparently back to the caller.
 */
/*@{*/
T_RV_RET mc_register_notifcation(T_MC_EVENT_CALLBACK callback, void *ctx)
{
	mc_env_ctrl_blk_p->ntfcCallback = callback;
	mc_env_ctrl_blk_p->ntfcCtx = ctx;

	return RV_OK;
}








/**
 * Read
 *
 * Detailled description.
 * This function reads data from a MC-card using a specific transfer mode.
 * If partial reads are allowed (if CSD parameter READ_BL_PARTIAL is set) the
 * start address can start and stop at any address within the card address
 * space, otherwise it shall start and stop at block boundaries. The client
 * is responsible for setting the correct mode, address and data size
 * parameter according to the device properties. The client can obtain these
 * properties by reading the CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          mode          The data transfer mode to use. The MC-specification
 *                        defines stream oriented data transfer and block
 *                        oriented data transfer. Only stream mode is supported
 *                        at this moment.
 *          addr          The physical start address in bytes units from where
 *                        to read data.
 *          data_p        Pointer to a destination buffer, provided by the
 *                        client, where the driver will put the data. The
 *                        buffer size shall be at least data_size bytes.
 *          data_size     Number of bytes to be read from the card.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_read (T_MC_RCA rca, T_MC_RW_MODE mode, UINT32 addr,
                   UINT8 *data_p, UINT32 data_size,
                   T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_read (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_read: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_READ_REQ_MSG),
                       MC_READ_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_read: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->addr       = addr;
  msg_p->data_p     = data_p;
  msg_p->data_size  = data_size;
  msg_p->mode       = mode;
  msg_p->rca        = rca;
  msg_p->subscriber = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Write
 *
 * Detailled description.
 * This function writes data to a MC-card using a specific transfer mode. If
 * partial reads are allowed (if CSD parameter READ_BL_PARTIAL is set) the
 * start address can start and stop at any address within the card address
 * space, otherwise it shall start and stop at block boundaries. The client
 * is responsible for setting the correct mode, address and data size parameter
 * according to the device properties. The client can obtain these properties
 * by reading the CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          mode          The data transfer mode to use. The MC-specification
 *                        defines stream oriented data transfer and block
 *                        oriented data transfer. Only stream mode is supported
 *                        at this moment.
 *          addr          The physical start address in bytes units from where
 *                        to write data.
 *          data_p        Pointer to a source buffer, provided by the client,
 *                        from where the driver will read the data. The buffer
 *                        size shall be at least data_size bytes.
 *          data_size     Number of bytes to be written to the card.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_write (T_MC_RCA rca, T_MC_RW_MODE mode, UINT32 addr,
                    UINT8 *data_p, UINT32 data_size,
                    T_MC_SUBSCRIBER subscriber)
{
  T_MC_WRITE_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_write (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_write: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_WRITE_REQ_MSG),
                       MC_WRITE_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_write: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->addr       = addr;
  msg_p->data_p     = data_p;
  msg_p->data_size  = data_size;
  msg_p->mode       = mode;
  msg_p->rca        = rca;
  msg_p->subscriber = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Erase
 *
 * Detailled description.
 * This function erases a range of erase groups on the card. The size of the
 * erase group is specified in the CSD. The erase group start and end address
 * is given in bytes units. This address will be rounded to down to the erase
 * group boundary.
 *
 * @param   rca           Relative Card Address.
 *          start_group   Erase group address in bytes units where erasing will
 *                        start.
 *          end_group     Erase group address in bytes units where erasing will
 *                        end.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_erase (T_MC_RCA rca, UINT32 erase_group_start,
                    UINT32 erase_group_end, T_MC_SUBSCRIBER subscriber)
{
  T_MC_ERASE_GROUP_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_erase (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_erase: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_ERASE_GROUP_REQ_MSG),
                       MC_ERASE_GROUP_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_erase: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca               = rca;
  msg_p->erase_group_start = erase_group_start;
  msg_p->erase_group_end   = erase_group_end;
  msg_p->subscriber        = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Set write protect
 *
 * Detailled description.
 * This function sets the write protection of the addressed write protect
 * group against erase or write. The group size is defined in units of
 * WP_GRP_SIZE erase group as specified in the CSD. This function does not
 * write protect the entire card which can be done by setting the permanent or
 * temporary write protect bits in the CSD. For this the mc_write_CSD()
 * function shall be used.
 *
 * @param   rca           Relative Card Address.
 *          wr_prot_group The group address in byte units. The LSB’s below
 *                        the group size will be ignored.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_set_write_protect (T_MC_RCA rca, UINT32 wr_prot_group,
                                T_MC_SUBSCRIBER subscriber)
{
  T_MC_SET_PROTECT_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_set_write_protect (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_set_write_protect: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }
  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_SET_PROTECT_REQ_MSG),
                       MC_SET_PROTECT_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_set_write_protect: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->wr_prot_group = wr_prot_group;
  msg_p->rca           = rca;
  msg_p->subscriber    = subscriber;


  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Clear write protect
 *
 * Detailled description.
 * This function clears the write protection of the addressed write protect
 * group. The group size is defined in units of WP_GRP_SIZE erase group as
 * specified in the CSD. This function does not disable write protect of the
 * entire card which can be done by erasing the temporary write protect bits
 * in the CSD. For this the mc_write_CSD() function shall be used.
 *
 * @param   rca           Relative Card Address.
 * @param   wr_prot_group The group address in byte units. The LSB’s below
 *                        the group size will be ignored.
 * @param   subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_clr_write_protect (T_MC_RCA rca, UINT32 wr_prot_group,
                                T_MC_SUBSCRIBER subscriber)
{
  T_MC_CLR_PROTECT_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_clr_write_protect (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

 /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_clr_write_protect: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_CLR_PROTECT_REQ_MSG),
                       MC_CLR_PROTECT_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_clr_write_protect: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->wr_prot_group = wr_prot_group;
  msg_p->rca           = rca;
  msg_p->subscriber    = subscriber;


  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Get write protect
 *
 * Detailled description.
 * This function reads 32 write protection bits representing 32 write protect
 * groups starting at the specified address.
 *
 * @param   rca           Relative Card Address.
 *          wr_prot_group The group address in byte units. The LSB’s below
 *                        the group size will be ignored.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_get_write_protect (T_MC_RCA rca, UINT32 wr_prot_group,
                                T_MC_SUBSCRIBER subscriber)
{
  T_MC_GET_PROTECT_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_get_write_protect (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_get_write_protect: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_GET_PROTECT_REQ_MSG),
                       MC_GET_PROTECT_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_get_write_protect: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->wr_prot_group = wr_prot_group;
  msg_p->rca           = rca;
  msg_p->subscriber    = subscriber;


  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * return the mechanical write protection
 *
 * This function will return the mechanical write protection of
 * a SD-card.
 *
 * @param   rca         RCA of card
 *
 * @return  NO_SD_CARD
 *          PROTECTED
 *          NOT_PROTECTED
 */

/*@{*/
T_MC_SD_MECH_WP mc_sd_get_mech_wp(T_MC_RCA rca)
{
  T_MC_SD_MECH_WP retval = NO_SD_CARD;

  MC_SEND_TRACE ("MC API entering mc_sd_get_mech_wp ((non-bridge))",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if(mc_env_ctrl_blk_p != NULL && mc_env_ctrl_blk_p->initialised == TRUE)
  {
    if((rca <= MC_MAX_STACK_SIZE)&& (rca != 0))
    {
      if((mc_env_ctrl_blk_p->reserved_card_id & (1 << (rca-1)))
         && (mc_env_ctrl_blk_p->card_infos[rca-1] !=NULL))
      {
        if(mc_env_ctrl_blk_p->card_infos[rca-1]->card_type == SD_CARD)
      {

#if 0
        if(MC_WRITE_PROTECTED)
        {
          return PROTECTED; //SINCE MECHANICAL WRITE PROTECT FEATURE NOT AVAILABLE IN LOCOSTO
        }
        else
#endif
      //  {
          return NOT_PROTECTED;
      //  }
      }
    }
  }
  }
  else
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
  }
  return retval;
}
/*@}*/



/**
 * Get card status
 *
 * Detailled description.
 * This function returns the 32-bit status register of the MC-card. This
 * status is not buffered in the driver but will be read directly from
 * the card. See [MC], paragraph 4.10 for an explanation of the status bits.
 *
 * @param   rca           Relative Card Address.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_get_card_status(T_MC_RCA rca,
                             T_MC_SUBSCRIBER subscriber)
{
  T_MC_CARD_STATUS_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_get_card_status (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_get_card_status: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_CARD_STATUS_REQ_MSG),
                       MC_CARD_STATUS_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_get_card_status: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca           = rca;
  msg_p->subscriber    = subscriber;


  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Get controller status
 *
 * Detailled description.
 * This function returns 16-bit MC- host controller status register
 * [MC_STAT].
 *
 * @param   status        Pointer to status register allocated by client and
 *                        filled by driver. See [Host Controller],
 *                        paragraph 2.4 for an explanations of the status bits.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_get_controller_status(UINT16 *status)
{
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_get_controller_status (non-bridge)",
                   RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /* Check status pointer */
  if (status == NULL)
  {
    MC_SEND_TRACE ("MC mc_get_controller_status: invalid status pointer",
                     RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* Call function */
  retval = mc_cmd_send_controller_status(status);

  return retval;
}

/**
 * Enable DMA mode
 *
 * Detailled description.
 * This function selects the DMA mode to be used by the driver. Default setting
 * will be force CPU.
 *
 * @param   dma_mode      Indicates whether to use DMA or let the CPU handle
 *                        the copying.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 *          RV_INVALID_PARAMETER invalid mode.
 */
T_RV_RET mc_dma_mode(T_MC_DMA_MODE dma_mode)
{
  MC_SEND_TRACE ("MC API entering mc_dma_mode (non-bridge)",
                   RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  if ( dma_mode != MC_FORCE_CPU &&
       dma_mode != MC_FORCE_DMA &&
       dma_mode != MC_DMA_AUTO )
  {
    MC_SEND_TRACE ("MC mc_dma_mode: invalid DMA mode",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /** Set DMA mode */
  mc_env_ctrl_blk_p->dma_mode = dma_mode;

  return RV_OK;
}

/**
 * Update acquisition
 *
 * Detailled description.
 * This function starts an identification cycle of a card stack (acquisition
 * procedure). The card management information in the controller will be
 * updated. New cards will be initialised; old cards keep their configuration.
 * At the end all active cards are in Stand-by state.
 * After this function has completed the number of cards connected can be
 * retrieved with the mc_get_stack_size() function. The session address of
 * each connected card can be retrieved with the mc_read_card_stack() function.
 *
 * @param   subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 */
T_RV_RET mc_update_acq (T_MC_SUBSCRIBER subscriber)
{
  T_MC_UPDATE_ACQ_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_update_acq (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_update_acq: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_UPDATE_ACQ_REQ_MSG),
                       MC_UPDATE_ACQ_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_update_acq: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->subscriber    = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Reset
 *
 * Detailled description.
 * This function resets all cards to idle state. This function executes the
 * GO_IDLE_STATE command (CMD0) on the bus. After completion of this service
 * the mc_update_acq() function shall be called before the MC-cards can be
 * used.
 *
 * @param   subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 */
T_RV_RET mc_reset(T_MC_SUBSCRIBER subscriber)
{
  T_MC_RESET_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_reset (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_reset: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_RESET_REQ_MSG),
                       MC_RESET_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_reset: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  go_idle_state();/*CMD0*/

  /* compose message */
  msg_p->subscriber    = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;

}

/**
 * Get stack size
 *
 * Detailled description.
 * This function returns the number of connected MC-cards.
 *
 * @param   size_p        Pointer to integer value allocated by the client, in
 *                        which the driver stores the stack size.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_INVALID_PARAMETER size_p is NULL.
 */
T_RV_RET mc_get_stack_size (UINT16 *size_p)
{
  UINT16 i;

  MC_SEND_TRACE ("MC API entering mc_get_stack_size (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if (size_p == NULL)
  {
    /** Size is a null pointer */
    MC_SEND_TRACE ("MC API: parameter not valid",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  if (mc_env_ctrl_blk_p == NULL ||
      mc_env_ctrl_blk_p->initialised == FALSE)
  {
    /** MC not ready */
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** reset size */
  *size_p = 0;

  /** Check number of reserved rca */
  for (i = 0; i < MC_MAX_STACK_SIZE; i++)
  {
    if ((mc_env_ctrl_blk_p->reserved_card_id & (1 << i))
        &&(mc_env_ctrl_blk_p->card_infos[i] != NULL))
    {
      *size_p += 1;
    }
  }

  return RV_OK;
}

/**
 * Read card stack
 *
 * Detailled description.
 * This function returns the relative card address of each individual MC-card
 * on the MC-bus. The client needs to provide an array of T_MC_RCA. The size
 * of the array can be determined with the mc_get_stack_size() function.
 *
 * @param   stack_p       Pointer to T_MC_RCA array.
 *          size          Array size in units of T_MC_RCA.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_INVALID_PARAMETER stack_p is NULL.
 */
T_RV_RET mc_read_card_stack (T_MC_RCA *stack_p, UINT16 size)
{
  UINT16 i;
  UINT16 j = 0;
  UINT16 size_card_stack;

  MC_SEND_TRACE ("MC API entering mc_get_stack_size (non-bridge)",
                   RV_TRACE_LEVEL_DEBUG_LOW);

  if (mc_env_ctrl_blk_p == NULL ||
      (mc_env_ctrl_blk_p->initialised == FALSE)
      || mc_env_ctrl_blk_p->state == MC_STATE_IDLE_DETACHED)
  {
    /** MC not ready */
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check stack pointer */
  if (stack_p == NULL)
  {
    /** Stack is a null pointer */
    MC_SEND_TRACE ("MC mc_get_stack_size: stack pointer not valid",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /** Check stack pointer */
  if (size == 0)
  {
    /** size is 0*/
    MC_SEND_TRACE ("MC mc_get_stack_size: size is 0",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }



  if (mc_get_stack_size(&size_card_stack) != RV_OK)
  {
        /** Stack is a null pointer */
    MC_SEND_TRACE ("MC mc_get_stack_size: mc_get_stack_size() failed",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /*check if cards attached*/
  if(size_card_stack == 0)
  {
    /** Stack is a null pointer */
    MC_SEND_TRACE ("MC mc_get_stack_size: no cards attached",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }


  /** Check stack size */
  if (size > size_card_stack)
  {
    MC_SEND_TRACE ("MC mc_get_stack_size: size not valid",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /** get numbers of rca's */
  for (i = 0; i < size; i++)
  {
    if (mc_env_ctrl_blk_p->reserved_card_id &  (1 << i))
    {
      stack_p[j++] = i+1;
    }
  }

  return RV_OK;
}

/**
 * Read OCR
 *
 * Detailled description.
 * This function returns the 32-bit OCR-register from a MC-card. This register
 * is not buffered in the driver and therefore will be read directly from the
 * card.
 *
 * @param   rca           Relative Card Address.
 *          ocr_p         Pointer to an 32-bits data location, provided by the
 *                        client, to which the driver copies the OCR.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_read_OCR (T_MC_RCA rca, UINT32 *ocr_p,
                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_OCR_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_read_OCR (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_read_OCR: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_READ_OCR_REQ_MSG),
                       MC_READ_OCR_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_read_OCR: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca        = rca;
  msg_p->ocr_p      = ocr_p;
  msg_p->subscriber = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}


/**
 * Read CID
 *
 * Detailled description.
 * This function returns the 128-bit CID register from a MC-card. This
 * register is not buffered in the driver and therefore will be read
 * directly from the card.
 *
 * @param   rca           Relative Card Address.
 *          cid_p         Pointer to a 128-bit buffer, allocated by the
 *                        client, to which the driver copies the CID. cid_p
 *                        points to the MSB of the CID.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_read_CID (T_MC_RCA rca, UINT8 *cid_p,
                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_CID_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_read_CID (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_read_CID: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_READ_CID_REQ_MSG),
                       MC_READ_CID_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_read_CID: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca = rca;
  msg_p->cid_p = cid_p;
  msg_p->subscriber    = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Read CSD
 *
 * Detailled description.
 * This function returns the 128-bit CSD register from a MC-card. This
 * register is not buffered in the driver and therefore will be read
 * directly from the card.
 *
 * @param   rca           Relative Card Address.
 *          csd_p         Pointer to a 128-bit buffer, allocated by the client,
 *                        to which the driver copies the CSD. csd_p points to
 *                        the MSB of the CSD.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_read_CSD (T_MC_RCA rca, UINT8 *csd_p,
                       T_MC_SUBSCRIBER subscriber)
{
  T_MC_READ_CSD_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_read_CSD (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_read_CSD: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_READ_CSD_REQ_MSG),
                       MC_READ_CSD_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_read_CSD: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca        = rca;
  msg_p->csd_p      = csd_p;
  msg_p->subscriber = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Write CSD
 *
 * Detailled description.
 * This function can be used to write one the programmable fields of the
 * CSD-register.
 *
 * @param   rca           Relative Card Address.
 *          field         CSD field to write.
 *          value         Value to write.
 *          subscriber    Subscriber identification value.
 * @return  RV_OK         Success
 *          RV_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *          RV_MEMORY_ERR Insufficient memory
 */
T_RV_RET mc_write_CSD (T_MC_RCA rca, T_MC_CSD_FIELD field,
                        UINT8 value, T_MC_SUBSCRIBER subscriber)
{
  T_MC_WRITE_CSD_REQ_MSG *msg_p;
  T_RV_RET retval;

  MC_SEND_TRACE ("MC API entering mc_write_CSD (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_write_CSD: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_WRITE_CSD_REQ_MSG),
                       MC_WRITE_CSD_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_write_CSD: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca        = rca;
  msg_p->field      = field;
  msg_p->value      = value;
  msg_p->subscriber = subscriber;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}


/**
 * Get SW version
 *
 * Detailled description.
 * This function returns the software version of the driver.
 *
 * @return  UINT32  The 32-bit software version
 */
UINT32 mc_get_sw_version (void)
{
#if 0 // Commented for porting from Riviera to GPF
  T_RVM_INFO_SWE swe_info;
  MC_SEND_TRACE ("MC API entering nan_get_sw_version (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);
  mc_get_info (&swe_info);
  return swe_info.type_info.type3.version;
#endif

  return BUILD_VERSION_NUMBER(MC_MAJOR,MC_MINOR,MC_BUILD);
}

/**
 * Get HW version
 *
 * Detailled description.
 * This function returns the hardware version number (module revision )
 * of the controller. This number is stored in the Module Revision
 * Register (MC_REV) of the controller.
 *
 * @return  UINT16  The 16-bit hardware version
 */
UINT16 mc_get_hw_version (void)
{
  MC_SEND_TRACE ("MC API entering mc_get_hw_version (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);

  return mc_cmd_get_hw_version();
}

/**
 * Returns the card type belonging to the given rca
 *
 * This function will return the card type belonging to the given
 * relative card address (rca).
 *
 *
 * @param   relative card address number
 *
 * @return  NO_CARD  no card is available at given address (or invalid address)
            SD_CARD  there is a SD card attached
            MC_CARD there is a MC card attached
 */

T_MC_CARD_TYPE mc_get_card_type(T_MC_RCA rca)
{
  T_MC_CARD_TYPE ret_card_type = NO_CARD;

  /** check if mc_swe is up and running*/
  if((mc_env_ctrl_blk_p != NULL) && (mc_env_ctrl_blk_p->initialised == TRUE))
  {
    /** check if rca doesnt exceed limits*/
    if((rca <= MC_MAX_STACK_SIZE)&& (rca != 0))
    {
      if((mc_env_ctrl_blk_p->reserved_card_id & (1 << (rca-1)))
         && (mc_env_ctrl_blk_p->card_infos[rca-1] !=NULL))
      {
        /** get card type from designated rca */
          ret_card_type = mc_env_ctrl_blk_p->card_infos[rca-1]->card_type;
      }
    }
  }

  return ret_card_type;
}


/**
 * Will retreive the 512 bit Status register of a SD card
 *
 * This function will retreive the 512 bit Status register of a SD card
 *
 * @param   subscriber  id
 *
 * @return  RV_INVALID_PARAMETER  no card is available at given address (or invalid address)

            MC_CARD there is a MC card attached
 */
T_RV_RET mc_sd_get_card_status(T_MC_RCA rca,UINT8 *sd_status_p,T_MC_SUBSCRIBER subscriber)
{
  T_RV_RET retval = RV_OK;
  T_MC_SD_CARD_STATUS_REQ_MSG *msg_p;

  MC_SEND_TRACE ("MC API entering mc_sd_get_card_status (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
    {
    MC_SEND_TRACE ("MC mc_sd_get_card_status: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_SD_CARD_STATUS_REQ_MSG),
                       MC_SD_CARD_STATUS_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_sd_get_card_status: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  //msg_p->rca          = rca;
  msg_p->subscriber   = subscriber;
  msg_p->sd_status_p  = sd_status_p;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;

    }


/**
 * Will retreive the 64 bit SCR register of a SD card
 *
 * This function will retreive the 64 bit SCR  register of a SD card
 *
 * @param   rca     relative card address number
 * @param   scr_p   reference to location where scr register can be stored
 * @param   subscriber id
 *
 * @return  RV_INVALID_PARAMETER  no card is available at given address (or invalid address)
            RV_INVALID_PARAMETER  there is no SD card attached
            MC_CARD there is a MC card attached
 */
T_RV_RET mc_read_scr(T_MC_RCA rca, UINT8 *scr_p,T_MC_SUBSCRIBER subscriber)
{
  T_RV_RET retval = RV_OK;
  T_MC_READ_SCR_REQ_MSG *msg_p;

  MC_SEND_TRACE ("MC API entering mc_read_scr (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }

  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_read_scr: invalid subscriber ID",
                     RV_TRACE_LEVEL_WARNING);
    return RV_INVALID_PARAMETER;
}

  /* reserve message buffer */
  if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_MC_READ_SCR_REQ_MSG),
                       MC_READ_SCR_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    MC_SEND_TRACE ("MC mc_read_scr: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->rca          = rca;
  msg_p->subscriber   = subscriber;
  msg_p->scr_p  = scr_p;

  /* Send message mailbox */
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);

  return retval;

}


/**
 * Subscribes the subscriber to MC events
 *
 * This function will subscribe the subscriber to certain events
 *
 *
 * @param   events      desired events:
 *                      MC_EVENT_INSERTION
 *                      MC_EVENT_REMOVAL
 * @param   subscriber  Subscriber ID
 *
 * @return  RV_OK
 *          RV_NOT_READY
 *          RV_INVALID_PARAMETER
 *          RV_MEMORY_ERR
 */
T_RV_RET  mc_send_notification(T_MC_EVENTS events, T_MC_SUBSCRIBER subscriber)
{
  T_MC_NOTIFICATION_REQ_MSG *msg_p;
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE ("MC API entering mc_notification (bridge)",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* MC task ready & initialised? */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    MC_SEND_TRACE ("MC API: not ready",
                     RV_TRACE_LEVEL_WARNING);
    return RV_NOT_READY;
  }
  /** Check if subscriber is valid */
  /** Check subscriber ID */
  if ((subscriber > MC_MAX_SUBSCRIBER) ||
      ((mc_env_ctrl_blk_p->reserved_subscriber_id & (1 << subscriber)) == 0) )
  {
    MC_SEND_TRACE ("MC mc_notification: invalid subscriber pointer",
                     RV_TRACE_LEVEL_WARNING);
    retval = RV_INVALID_PARAMETER;

  } else {
    /* reserve message buffer */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_NOTIFICATION_REQ_MSG),
                         MC_NOTIFICATION_REQ_MSG,
                         (T_RV_HDR **) &msg_p) == RVF_RED)
    {
      MC_SEND_TRACE ("MC mc_notification: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return RV_MEMORY_ERR;
    }

    /* compose message */
    msg_p->events        = events;
    msg_p->subscriber = subscriber;

    /* Send message mailbox */
    retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);
  }

  return retval;
}
