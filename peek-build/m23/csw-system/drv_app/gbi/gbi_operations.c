/**
 * @file   gbi_operations.c
 *
 * Implementation of GBI operations.
 * These functions implement the gbi processing for all the messages the
 * gbi task can receive.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  06/01/2004                  Create
 *
 *
* (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved */

#include <string.h>
#include "gbi/gbi_i.h"

#include "gbi/gbi_pi_cfg.h"

#define TIMELOG_GBI_API
#include "rfs/time_stamp/time_stamp.h"
/* Local functions */
static T_RV_RETURN_PATH gbi_i_get_subscribed_event_ret_path(T_GBI_EVENTS  event,
                                                            UINT8         subscribe_idx);

static void             gbi_i_send_event_ind_message( T_GBI_EVENTS      event,
                                                      T_RV_RETURN_PATH  ret_path);



/*******************************************************************************
*  GBI framework functions:   
*  Components: Media Availability, Request-response handling, Timer handling. 
*******************************************************************************/


/**
 * Component: Request Repsonse Handling
 * Sub-function:  gbi_i_generic_client_response
 *
 * This function uses the return_path parameter to determine how the response message is 
 * to be send. The response message is to be reserved and filled before using this 
 * function.
 *
 * @param   rspmsg_p    Response message to be send.
 * @param   return_path Return path specifying the response handling. 
 * @return  GBI_OK, GBI_MEMORY_ERR
 */
T_GBI_RESULT  gbi_i_generic_client_response(T_RV_HDR *rspmsg_p, T_RV_RETURN return_path)
{
  T_RVF_RET result = RVF_OK;
  
  // TS_StoreCurrentTimeStamp_Turbo (TIMELOG_GBI_RESP);
  /* follow return path: callback or mailbox */
  if (return_path.callback_func != NULL)
  {
    return_path.callback_func (rspmsg_p);
  } 
  else 
  {
    result = rvf_send_msg (return_path.addr_id, rspmsg_p);
    if (result != RVF_OK)
    {
      GBI_SEND_TRACE ("GBI gbi_i_generic_client_response: could not send response msg",
                       RV_TRACE_LEVEL_WARNING);
      
      return GBI_MEMORY_ERR;
    }
  }

  return GBI_OK;
}


/**
 * Component:    General support function
 * Sub-function: gbi_i_startup_event_req_msg
 *
 * 
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK
 */
T_GBI_RESULT gbi_i_startup_event_req_msg(T_RV_HDR *msg_p)
{
  T_GBI_RESULT          result        = GBI_OK;
  UINT8                 plugin_idx;
  T_GBI_PLUGIN_FNCTBL   const *func_ptr;

  /* For all plugins, perform start function for pre-initialisation */
  for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
  {
    func_ptr = gbi_plugin_fnctbl[plugin_idx];
    result   = func_ptr->gbi_plugin_reg_msg(msg_p);
  }

  return result;
}

/**
 * Component:    General support function
 * Sub-function: gbi_i_get_plugin_status_info
 *
 * 
 * @param   
 * @return  
 */
T_GBI_PLUGIN_INFO gbi_i_get_plugin_status_info(UINT8 plugin_idx)
{
  T_GBI_PLUGIN_INFO plugin_info;
  UINT8             i;
  
  if (plugin_idx < GBI_NMB_PLUGINS) 
  {
    plugin_info.msg_id_in_progress  = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].msg_id_in_progress;
    plugin_info.plugin_info_sync    = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].plugin_info_sync;
    plugin_info.plugin_state        = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].plugin_state;

    for (i = 0; i < GBI_MAX_NR_OF_MEDIA; i++)
    {
      plugin_info.media_data_p[i]     = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[i];
    }
    plugin_info.nmb_media_p         = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p;   

    for (i = 0; i < GBI_MAX_NR_OF_PARTITIONS; i++)
    {
      plugin_info.partition_data_p[i] = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].partition_data_p[i];
    }
    plugin_info.nmb_partition_p     = gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_partition_p;   
  }
  else 
  {
    plugin_info.msg_id_in_progress  = GBI_NO_MESSAGE_IN_PROGRESS;
    plugin_info.plugin_info_sync    = FALSE;
    plugin_info.plugin_state        = GBI_STATE_READY;

    for (i = 0; i < GBI_MAX_NR_OF_MEDIA; i++)
    {
      plugin_info.media_data_p[i]     = NULL;
    }
    plugin_info.nmb_media_p         = NULL; 

    for (i = 0; i < GBI_MAX_NR_OF_PARTITIONS; i++)
    {
      plugin_info.partition_data_p[i] = NULL;
    }
    plugin_info.nmb_partition_p     = NULL;   
  }

  return plugin_info;
}

/**
 * Component:    General support function
 * Sub-function: gbi_i_set_plugin_status_info
 *
 * 
 * @param   
 * @return  
 */
void gbi_i_set_plugin_status_info(UINT8               plugin_idx, 
                                  T_GBI_PLUGIN_INFO   plugin_info)
{
  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].msg_id_in_progress  = plugin_info.msg_id_in_progress;
  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].plugin_info_sync    = plugin_info.plugin_info_sync;
  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].plugin_state        = plugin_info.plugin_state;

  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[0]     = plugin_info.media_data_p[0];

  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p         = plugin_info.nmb_media_p; 
  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].partition_data_p[0] = plugin_info.partition_data_p[0];
  gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_partition_p     = plugin_info.nmb_partition_p; 
}

/**
 * Component: Media Availability
 * Function:  gbi_i_subscribe_event
 *
 * This function can be used to subscribe to a specific media event
 * The first time this function is called (with a valid subscription) the subscriber 
 * is notified immediately. This enables the subscriber to get media and partition 
 * information the first time.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK
 */
T_GBI_RESULT gbi_i_subscribe_event(T_RV_HDR *msg_p)
{
  T_GBI_I_SUBSCRIBE_EVENT_REQ_MSG *int_msg_p;
  T_GBI_EVENTS  event;
  T_RV_RETURN   return_path;
  UINT8         i, j;
  BOOL          subscribed_event_1 = FALSE;                        
  BOOL          subscribed_event_2 = FALSE;           
  BOOL          subscribed_event_3 = FALSE;           
  T_GBI_RESULT  retval = GBI_OK;

  BOOL          same_client_subscribed_event_1 = FALSE;                        
  BOOL          same_client_subscribed_event_2 = FALSE;           
  BOOL          same_client_subscribed_event_3 = FALSE;  


  
  int_msg_p   = (T_GBI_I_SUBSCRIBE_EVENT_REQ_MSG *)msg_p;
  event       = int_msg_p->event;
  return_path = int_msg_p->return_path;

  /* Combination event and return_path is stored. When the specified event occurs the 
     T_GBI_EVENT_IND_MSG is send to all return_path stored with this event.
     To unsubscribe the client should subscribe with event GBI_EVENT_MEDIA_NONE
   */

  if (event == GBI_EVENT_MEDIA_NONE)
  {
    /* The client want to unsubscribe from all events.
     * Search for this client (return_path) the subscribed events and clear the
     * subscription 
     */
    for (i = 0; i < GBI_MAX_NR_OF_EVENTS; i++)
    {
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[i][j].ret_path.callback_func == 
             return_path.callback_func) &&
            (gbi_env_ctrl_blk_p->sub_events[i][j].ret_path.addr_id ==
             return_path.addr_id))
        {
          /* The client is subscribed for this event */
          gbi_env_ctrl_blk_p->sub_events[i][j].ret_path.callback_func = NULL;
          gbi_env_ctrl_blk_p->sub_events[i][j].ret_path.addr_id       = 0; 
        }
      }
    }
  }
  else
  {
    /* Evaluate GBI_EVENT_MEDIA_INSERT event */
    if ((event & GBI_EVENT_MASK_0) == GBI_EVENT_MEDIA_INSERT)
    {
      /* Check if client is already subscribed for this event.*/
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.callback_func 
             == return_path.callback_func) && 
            (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.addr_id 
             == return_path.addr_id))
        {
          /* Client already subscribed, quit */ 
          subscribed_event_1 = TRUE;           
	    same_client_subscribed_event_1 = TRUE;           
             
        }
      }

      if (subscribed_event_1 == FALSE)
      {
        /* Client is not subscribed for this event, search free entry */        
        for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
        {
          if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.callback_func 
               == NULL) && 
              (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.addr_id 
               == 0) && 
               subscribed_event_1 == FALSE)
          {
            /* Free entry found, subscribe this client */               
            gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.callback_func = 
                return_path.callback_func;
            gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.addr_id       = 
                return_path.addr_id;
            
            /* Client subscribed, quit */ 
            subscribed_event_1 = TRUE;
          }
          else 
          {
            /* Already another subscriber in this entry */
          }
        }
      }
    }
    else 
    {
      /* Unsubscribe this client if it is subscribed to this event*/
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.callback_func 
             == return_path.callback_func) && 
            (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.addr_id 
             == return_path.addr_id))
        {
          gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.callback_func = NULL;
          gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.addr_id       = 0; 
        }
      }
    }

    /* Prepare next event subscribe actions */
    subscribed_event_2 = FALSE; 

    /* Evaluate GBI_EVENT_MEDIA_REMOVEAL event */
    if ((event & GBI_EVENT_MASK_1) == GBI_EVENT_MEDIA_REMOVEAL)
    {
      /* Check if client is already subscribed for this event.*/
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.callback_func 
             == return_path.callback_func) && 
            (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.addr_id 
             == return_path.addr_id))
        {
          /* Client already subscribed, quit */ 
          subscribed_event_2 = TRUE;                        
	    same_client_subscribed_event_2 = TRUE;
        }
      }

      if (subscribed_event_2 == FALSE)
      {
        /* Client is not subscribed for this event, search free entry */        
        for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
        {
          if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.callback_func 
               == NULL) && 
              (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.addr_id 
               == 0) && 
               subscribed_event_2 == FALSE)
          {
            /* Free entry found, subscribe this client */               
            gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.callback_func = 
                return_path.callback_func;
            gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.addr_id       = 
                return_path.addr_id;

            /* Client subscribed, quit */ 
            subscribed_event_2 = TRUE;                        
          }
          else 
          {
            /* Already another subscriber in this entry */
          }
        }
      }
    }
    else 
    {
      /* Unsubscribe this client if it is subscribed to this event*/
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.callback_func 
             == return_path.callback_func) && 
            (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.addr_id 
             == return_path.addr_id))
        {
          gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.callback_func = NULL;
          gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.addr_id       = 0; 
        }
      }
    }



    /* Prepare next event subscribe actions */
    subscribed_event_3 = FALSE; 

    /* Evaluate GBI_EVENT_MEDIA_REMOVEAL event */
    if ((event & GBI_EVENT_MASK_2) == GBI_EVENT_NAN_MEDIA_AVAILABLE)
    {
      /* Check if client is already subscribed for this event.*/
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.callback_func 
             == return_path.callback_func) && 
            (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.addr_id 
             == return_path.addr_id))
        {
          /* Client already subscribed, quit */ 
          subscribed_event_3 = TRUE;                        
	    same_client_subscribed_event_3 = TRUE;
        }
      }

      if (subscribed_event_3 == FALSE)
      {
        /* Client is not subscribed for this event, search free entry */        
        for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
        {
          if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.callback_func 
               == NULL) && 
              (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.addr_id 
               == 0) && 
               subscribed_event_3 == FALSE)
          {
            /* Free entry found, subscribe this client */               
            gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.callback_func = 
                return_path.callback_func;
            gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.addr_id       = 
                return_path.addr_id;

            /* Client subscribed, quit */ 
            subscribed_event_3 = TRUE;                        
          }
          else 
          {
            /* Already another subscriber in this entry */
          }
        }
      }
    }
    else 
    {
      /* Unsubscribe this client if it is subscribed to this event*/
      for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
      {
        if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.callback_func 
             == return_path.callback_func) && 
            (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.addr_id 
             == return_path.addr_id))
        {
          gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.callback_func = NULL;
          gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.addr_id       = 0; 
        }
      }
    }




    /* Check if a subscription took place */
    /*if (subscribed_event_1 == TRUE || subscribed_event_2 == TRUE)
    {
         //  Notify client so it can request media and partition info 
        gbi_i_send_event_ind_message(GBI_EVENT_MEDIA_INSERT, return_path);

    }*/

     if (subscribed_event_1 == TRUE)
     {
      if (same_client_subscribed_event_1 == FALSE)
      {
        //Notify client so it can request media and partition info //
        gbi_i_send_event_ind_message(GBI_EVENT_MEDIA_INSERT, return_path);
      }
    }


	
    
  }
  
  return retval;
}

/**
 * Component: Media Availability
 * Ffunction: gbi_i_check_if_media_changed
 *
 * This function can be used by the 'Request Message Handler' to check if a plugin 
 * had a media change event since the last time that the plugin was requested to 
 * provide the media and partition in-formation. The function also returns TRUE the 
 * first time it is called to get the initialization started.
 *
 * @param   plugin_number   Plugin identification.
 * @return  TRUE or FALSE
 */
BOOLEAN   gbi_i_check_if_media_changed( UINT8 plugin_number)
{
  T_GBI_PLUGIN_INFO   plugin_info;

  plugin_info = gbi_i_get_plugin_status_info(plugin_number);
  
  if (plugin_info.plugin_info_sync == FALSE)
  {
    /* When the synchronisation flag is set to FALSE it means the media and 
     * partition information is not synchronised to the client by using the 
     * request media and partition information before. So media is changed.
     */
    GBI_SEND_TRACE("Media changed", RV_TRACE_LEVEL_DEBUG_MEDIUM); 
    return TRUE;
  }
  else 
  {
    /* When the synchronisation flag is set to TRUE it means the media and 
     * partition information is already synchronised to the client by using the 
     * request media and partition information before. So media is not changed.
     */ 
    GBI_SEND_TRACE("Media NOT changed", RV_TRACE_LEVEL_DEBUG_MEDIUM);      
    return FALSE;
  }
}


/**
 * Component: Media Availability
 * Function:  gbi_i_media_change_event
 *
 * This function can be used by the plugins to report a media change (media insertion 
 * or removal). Each plugin has to provide its own way to detect media insertion or 
 * removal (if it is a plugin for remov-able media). It can use driver call back 
 * (subscription to drivers) or polling on timer interval base.
 *
 * @param   plugin_number   Plugin identification.
 * @param   event           Event occured
 * @return  None
 */
void    gbi_i_media_change_event(UINT8         plugin_number,
                                 T_GBI_EVENTS  event)
{
  T_RV_RETURN_PATH  ret_path;
  UINT8             subscribe_idx;
  T_GBI_PLUGIN_INFO plugin_info;

  /* Perform action for all possible subscribers for this event */
  for (subscribe_idx = 0; subscribe_idx < GBI_MAX_EVENT_SUBSCRIBERS; subscribe_idx++)
  {
    ret_path = gbi_i_get_subscribed_event_ret_path(event, subscribe_idx);

    /* Check if a subscriber is assigned to this event */
    if (ret_path.callback_func  != NULL || ret_path.addr_id != 0)
    {
      gbi_i_send_event_ind_message(event, ret_path);
    }
  }

  /* If the media is removed, This media should be marked as not pressent */

  /* BTI: To be worked out during development of RFS */

  /* Mark this plugin has it has no media and partition request before */
  plugin_info = gbi_i_get_plugin_status_info(plugin_number);
  plugin_info.plugin_info_sync = FALSE;
  gbi_i_set_plugin_status_info(plugin_number, plugin_info);

  return;
}

/**
 * Component: Media Availability
 * Sub-function:  gbi_i_get_subscribed_event_ret_path
 *
 * This function retrives the stored return_path of the given event and subscriber index
 *
 * @param   event           Event occured or subscribed to
 * @param   subscribe_idx   subscriber index
 * @return  ret_path        Return path stored for this message
 */
static T_RV_RETURN_PATH gbi_i_get_subscribed_event_ret_path(T_GBI_EVENTS  event,
                                                            UINT8         subscribe_idx)
{
  T_RV_RETURN_PATH ret_path;

  if ((event & GBI_EVENT_MASK_0) == GBI_EVENT_MEDIA_INSERT)
  {
    /* Evaluate whether is GBI_EVENT_MEDIA_INSERT event */
    ret_path.callback_func = 
        gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][subscribe_idx].ret_path.callback_func;
    ret_path.addr_id       = 
        gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][subscribe_idx].ret_path.addr_id;
  }
  else if ((event & GBI_EVENT_MASK_1) == GBI_EVENT_MEDIA_REMOVEAL)
  {
    /* Evaluate whether is GBI_EVENT_MEDIA_REMOVEAL event */
    ret_path.callback_func = 
        gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][subscribe_idx].ret_path.callback_func;
    ret_path.addr_id       = 
        gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][subscribe_idx].ret_path.addr_id;
  }

  else if ((event & GBI_EVENT_MASK_2) == GBI_EVENT_NAN_MEDIA_AVAILABLE)
  {
    /* Evaluate whether is GBI_EVENT_MEDIA_REMOVEAL event */
    ret_path.callback_func = 
        gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][subscribe_idx].ret_path.callback_func;
    ret_path.addr_id       = 
        gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][subscribe_idx].ret_path.addr_id;
  }
  
  else
  {
    /* Non-supported event */
    ret_path.callback_func = NULL;
    ret_path.addr_id       = 0;
  }

  return ret_path;
}

/**
 * Component: Media Availability
 * Sub-function:  gbi_i_send_event_ind_message
 *
 * This function sends the GBI_EVENT_IND_MSG message
 *
 * @param   event     Event occured or subscribed to
 * @param   ret_path  Return path for this message
 * @return  None
 */
static void   gbi_i_send_event_ind_message( T_GBI_EVENTS      event,
                                            T_RV_RETURN_PATH  ret_path)
{
  T_GBI_EVENT_MSG   *event_msg;

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_EVENT_MSG),
                       GBI_EVENT_IND_MSG, 
                       (T_RV_HDR **) &event_msg) == RVF_RED)
  {
    GBI_SEND_TRACE ("gbi_i_send_event_ind_message: out of memory", RV_TRACE_LEVEL_WARNING);
    return;
  }

  /* Compose message */
  event_msg->hdr.msg_id = GBI_EVENT_IND_MSG;
  event_msg->event      = event;

  /* Generic response message to send message to client */
  gbi_i_generic_client_response ((T_RV_HDR *) event_msg, ret_path);
}


/**
 * Component: Media Availability
 * Sub-function:  gbi_i_media_info_synchronised
 *
 * This function marks that the media and partition information is synchronised
 * after request of the client
 *
 * @param   plugin_number   Plugin identification.
 * @return  None
 */
void gbi_i_media_info_synchronised(UINT8 plugin_number)
{
  T_GBI_PLUGIN_INFO   plugin_info;

  /* Mark this plugin has synchronised is media and partition information */
  plugin_info = gbi_i_get_plugin_status_info(plugin_number);
  plugin_info.plugin_info_sync = TRUE;
  gbi_i_set_plugin_status_info(plugin_number, plugin_info);

  return;
}




/**
 * function: gbi_i_determine_fat_type
 *
 *  Determine the FAT file system type (FAT12, FAT16 or FAT32).
 *  The determination is done by interpreting the boot sector
 *  according to the Microsoft Corporation Hardware White Paper
 *  (version 1.02, May 5, 1999).
 */
T_GBI_FS_TYPE gbi_i_determine_fat_type(UINT8 *boot_sector_p)
{
  UINT32  root_dir_sectors;
  UINT32  data_sec;
  UINT32  tot_sec;
  UINT32  count_of_clusters;
  UINT16  fat_size;
  UINT16  hlp_uint16_1;
  UINT16  hlp_uint16_2;
  UINT8   *bts_p;
  T_GBI_FS_TYPE result;

  bts_p = (UINT8*)boot_sector_p;


  if((bts_p[0] != 0xE9) && (bts_p[0] != 0xEB))
 	{
 	  return GBI_UNKNOWN;    /* No Jump Instruction, Unknown File system */
 	}
 	


  hlp_uint16_1 = bts_p[17] + (bts_p[18]<<8); //BPB_RootEntCnt
  hlp_uint16_2 = bts_p[11] + (bts_p[12]<<8); //BPB_BytsPerSec
  root_dir_sectors = ((hlp_uint16_1 *32) + (hlp_uint16_2 -1)) / hlp_uint16_2;

  hlp_uint16_1 = bts_p[22] + (bts_p[23]<<8); //BPB_FARSz16
  if (hlp_uint16_1 != 0)
    fat_size = hlp_uint16_1;
  else
    fat_size = bts_p[36] + (bts_p[37]<<8); //BPB_FARSz32

  hlp_uint16_1 = bts_p[19] + (bts_p[20]<<8);  //BPB_TotSec16
  if (hlp_uint16_1 != 0)
    tot_sec = (UINT32)hlp_uint16_1;
  else
    tot_sec = bts_p[32] + (bts_p[33]<<8) + (bts_p[34]<<16) + (bts_p[35]<<24); //BPB_TotSec32

  hlp_uint16_1 = bts_p[14] + (bts_p[15]<<8); //BPB_ResvdSecCnt
  data_sec = tot_sec - (hlp_uint16_1 + (bts_p[16] * fat_size) + root_dir_sectors);

  count_of_clusters = data_sec / bts_p[13];  //BPB_SecPerClus

  if (count_of_clusters < 4085)
    result = GBI_FAT12;
  else if (count_of_clusters < 65525)
    result = GBI_FAT16_LBA;
  else
    result = GBI_FAT32_LBA;

  GBI_SEND_TRACE("gbi_i_determine_fat_type", RV_TRACE_LEVEL_DEBUG_LOW);
  GBI_SEND_TRACE_PARAM("result", result, RV_TRACE_LEVEL_DEBUG_LOW);

  return result;
}


/********************************************************************/
/* Unused/not implemented timer functions:                                */
/********************************************************************/

/**
 * Component: Timer Handling
 * Sub-function:  gbi_i_set_timer
 *
 * The function can ask a timer with type once or interval. The returned id identifies the 
 * timer. This id can be used to remove the timer. When the timer elapse the callback is 
 * invoked.
 *
 * @param   timer_type    Timer type once or interval.
 * @param   time          Lenght in time of timer.
 * @param   id            Timer ID (necessary for removing the timer).
 * @param   callback      Callback to execute on expiring of timer 
 * @return  RV_OK
 */
/*
T_RV_RET  gbi_i_set_timer(T_GBI_TIMER_REQ_TYPE  timer_type,
                          T_GBI_TIME_T          time, 
                          UINT8                 *id, 
                          void                  *callback)
{

  return RV_OK;
}
*/

/**
 * Component: Timer Handling
 * Sub-function:  gbi_i_delete_timer
 *
 * This function can be used toe remove a timer.
 *
 * @param   id            Timer ID of timer to remove.
 * @return  RV_OK
 */
/*
T_RV_RET  gbi_i_delete_timer(UINT8 id)
{
  
  return RV_OK;
}
*/

