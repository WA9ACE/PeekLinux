/**
 * @file  gbi_handle_message.c
 *
 * GBI handle_msg function, which is called when the SWE
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
 *  12/29/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */
#include <string.h>

#include "gbi/gbi_i.h"
#include "gbi/gbi_operations.h"
#include "gbi/gbi_pi_cfg.h"
#include "gbi/gbi_api.h"
#include "chipset.cfg"
#include "rv_swe.h"

#ifdef USE_MC
#include "gbi/gbi_mmc.h"
#include "mc/mc_message.h"
#include "gbi/gbi_pi_mc.h"
#include "gbi/gbi_pi_mc_i.h"
#endif

#define TIMELOG_GBI_API
#include "rfs/time_stamp/time_stamp.h"

#ifdef GBI_NAND_BM_SWE
#include "nan/nan_api.h"
#include "gbi_pi_nandbm.h"
#endif


#ifdef GBI_NOR_BM_SWE
#include "nor_bm/nor_bm_api.h"
#endif

#ifdef GBI_DATALIGHT_SWE
#include "datalight/datalight_api.h"
#endif

/*
 *  Local defintions of variables and functions
 */
#define MAX_NR_OF_QUEUED_MSG 8

static T_RV_HDR* message_queue[MAX_NR_OF_QUEUED_MSG] =
  {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/* Global component functions */
T_GBI_RESULT  gbi_i_handle_req_msg(T_RV_HDR *msg_p);
T_GBI_RESULT  gbi_i_handle_resp_msg(T_RV_HDR *msg_p);

T_GBI_RESULT  gbi_i_create_partition_names(UINT8                  plugin_idx,
                                           UINT8                  nmb_of_partitions,
                                           T_GBI_PARTITION_INFO   *partition_info_p);

T_GBI_RESULT  gbi_i_create_filesystem_names(UINT8                 plugin_idx,
                                            UINT8                 nmb_of_partitions,
                                            T_GBI_PARTITION_INFO  *partition_info_p);


/* Local component functions */
static BOOL           gbi_i_msg_is_request_message (T_RV_HDR *msg_p);
static BOOL           gbi_i_get_message (T_RV_HDR **query_msg_p, UINT8 *recover_index_p);
static void           gbi_i_save_message (T_RV_HDR *msg_p);
static void           gbi_i_remove_saved_message(UINT8 recover_index);

static T_GBI_RESULT   gbi_i_handle_plugin_single_req_msg(T_RV_HDR *msg_p);
static T_GBI_RESULT   gbi_i_handle_plugin_multiple_req_msg (T_RV_HDR *msg_p);

/* Local support functions */
static T_GBI_RESULT gbi_i_all_plugins_ready_multiple_req(UINT32       req_msg_id,
                                                         T_RVF_MB_ID  req_mb_id,
                                                         T_RV_RETURN  req_ret_path,
                                                         T_RV_HDR *msg_p);

static BOOL gbi_i_check_if_all_plugins_ready(T_RV_HDR *msg_p);

static short int gbi_get_plugin_num(unsigned short media_num);

/*
 *  Now local, but must become global
 */
#define MAX_NR_OF_GROUP_MSG 20
#define NO_MESSAGE_ID       0xFFFFFFFF

const char      *part_name_table[GBI_MEDIA_TYPE_END] = {GBI_MEDIA_NAMES};

T_GBI_FS_NAMES  fs_name_table[GBI_NMB_OF_FS_NAMES] =  {GBI_FS_NAMES};

/* Important remark, the filled messages in this groups should always be
 * extended with NO_MESSAGE_ID until MAX_NR_OF_GROUP_MSG is reached
 */
static UINT32   message_id_groups[GBI_NMB_PLUGINS][MAX_NR_OF_GROUP_MSG] = {

#ifdef GBI_DATALIGHT_SWE
{
		DL_READ_RSP_MSG,
	  	DL_WRITE_RSP_MSG,
	  	DL_ERASE_RSP_MSG,
	  	DL_INIT_RSP_MSG,
	  	NO_MESSAGE_ID
}
#else
		#ifdef USE_NOR_BM
 		{
  		  NOR_BM_READ_RSP_MSG,
	      NOR_BM_WRITE_RSP_MSG,
	      NOR_BM_ERASE_RSP_MSG,
    	  NOR_BM_FLUSH_RSP_MSG,
	      NO_MESSAGE_ID,
    	  NO_MESSAGE_ID,
      	  NO_MESSAGE_ID,
      	  NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID

		}
		#endif

		#ifdef USE_NAND_BM
			  #ifdef USE_NOR_BM
  				,
  			  #endif
		{
      		NAN_BM_READ_RSP_MSG,
		     NAN_BM_WRITE_RSP_MSG,
	      NAN_BM_ERASE_RSP_MSG,
    	  NAN_BM_FLUSH_RSP_MSG,
	      NAN_BM_DMA_MODE_RSP_MSG,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID,
	      NO_MESSAGE_ID
		 }
		#endif /* USE NAND_BM */

#endif    /* GBI_DATALIGHT_SWE */

#ifdef USE_MC

	#ifdef GBI_DATALIGHT_SWE
   	,
	#else
  		#ifdef USE_NAND_BM
   		,
  		#else
		#ifdef USE_NOR_BM
	  	,
		#endif
 	#endif

  	#endif  /* GBI_DATALIGHT_SWE */
{
		MC_SUBSCRIBE_RSP_MSG,
  		MC_UNSUBSCRIBE_RSP_MSG,
  		MC_READ_RSP_MSG,
  		MC_WRITE_RSP_MSG,
  		MC_ERASE_GROUP_RSP_MSG,
  		MC_SET_PROTECT_RSP_MSG,
        MC_CLR_PROTECT_RSP_MSG,
  		MC_GET_PROTECT_RSP_MSG,
  		MC_CARD_STATUS_RSP_MSG,
  		MC_UPDATE_ACQ_RSP_MSG,
  		MC_RESET_RSP_MSG,
  		MC_READ_OCR_RSP_MSG,
  		MC_READ_CID_RSP_MSG,
  		MC_READ_CSD_RSP_MSG,
  		MC_WRITE_CSD_RSP_MSG,
  		MC_NOTIFICATION_RSP_MSG,
  		MC_EVENT_IND_MSG,
  		MC_SD_CARD_STATUS_RSP_MSG,
  		MC_READ_SCR_RSP_MSG,
        NO_MESSAGE_ID
}


#endif
    };


static T_GBI_MEDIA_INFO media_table[GBI_MAX_NR_OF_MEDIA];
static UINT8            media_cnt_table[GBI_MAX_NR_OF_MEDIA];

extern unsigned long gbi_start_time;
unsigned long gbi_operational_time;

static BOOL first_GC_event = TRUE;



/**
 * Component: Message Distribution
 * Function:  gbi_i_handle_message
 *
 * The message Distribution component receives as input al messages intended
 * for the GBI component.
 *
 * @param   msg_p Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN gbi_handle_message (T_RV_HDR *msg_p)
{
  T_RV_HDR      *int_msg_p;     // internal message pointer
  T_RV_HDR      *query_msg_p;   // query message pointer
  BOOL          result = FALSE;
  T_GBI_RESULT  gbi_status = GBI_OK;
  BOOL          message_eval_proceed = TRUE;
  BOOL          recovered_message = FALSE;
  UINT8         recover_index = 0;

  TS_StoreCurrentTimeStamp_Turbo (TIMELOG_GBI);
  int_msg_p   = msg_p;
  query_msg_p = NULL;

  while (message_eval_proceed == TRUE)
  {
    result = gbi_i_msg_is_request_message(int_msg_p);

    if (result == TRUE)
    {
      if (int_msg_p->msg_id == GBI_I_STARTUP_EVENT_REQ_MSG ||
          gbi_env_ctrl_blk_p->state == GBI_OPERATIONAL)
      {
        gbi_status = gbi_i_handle_req_msg(int_msg_p);

        if (gbi_status == GBI_BUSY)
        {
          if (recovered_message == FALSE)
          {
            query_msg_p = int_msg_p;
            gbi_i_save_message(int_msg_p);
          }
          else
          { //the message is still in the queue
            recover_index++;
          }
        }
        else
        {
          if (recovered_message != FALSE)
          {
            gbi_i_remove_saved_message(recover_index);
            recover_index = 0; //resume from begin
          }
          
   
	  /* Status is GBI_OK or an error */
          rvf_free_msg(int_msg_p);
        }
      }
      else
      { //It's a request while not ready.
        //Save it, but there is no reason to inspect the queue contents.
        if (recovered_message == FALSE)
        {
          query_msg_p = int_msg_p;
          gbi_i_save_message(int_msg_p);
        }
        break;
      }
    }
    else
    {
      gbi_status = gbi_i_handle_resp_msg(int_msg_p);
      if (gbi_status == GBI_BUSY)
      {
        if (recovered_message == FALSE)
        {
          query_msg_p = int_msg_p;
          gbi_i_save_message(int_msg_p);
        }
        else
        { //the message is still in the queue
          recover_index++;
        }
      }
      else
      {
        if (recovered_message != FALSE)
        {
          gbi_i_remove_saved_message(recover_index);
          recover_index = 0; //resume from begin
        }
        /* Status is GBI_OK or an error */
        rvf_free_msg(int_msg_p);
      }
    }

    result = gbi_i_get_message(&query_msg_p, &recover_index);
    if (result == TRUE)
    {
      int_msg_p = query_msg_p;
      recovered_message = TRUE;
    }
    else
    {
      /* No waiting messages, exit function */
      message_eval_proceed = FALSE;
    }
  }

  // GBI_SEND_TRACE("return gbi_handle_message", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  return RVM_OK;
}


/**
 * Component: Request handler
 * Function:  gbi_i_handle_req_msg
 *
 * The 'Request Handler' component processes all the API request messages.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_BUSY
 */
T_GBI_RESULT  gbi_i_handle_req_msg(T_RV_HDR *msg_p)
{
  T_GBI_RESULT result = GBI_OK;

  if (msg_p != NULL)
  {
    switch (msg_p->msg_id)
    {
      case GBI_I_SUBSCRIBE_EVENT_REQ_MSG:
        /* No specific plugin involved */
        result = gbi_i_subscribe_event(msg_p);
        break;

      case GBI_READ_REQ_MSG:
      case GBI_WRITE_REQ_MSG:
      case GBI_WRITE_WITH_SPARE_REQ_MSG:
      case GBI_ERASE_REQ_MSG:
      case GBI_FLUSH_REQ_MSG:
      case GBI_READ_SPARE_DATA_REQ_MSG:

        /* Single plugin involved */
        result = gbi_i_handle_plugin_single_req_msg(msg_p);
        break;

      case GBI_I_STARTUP_EVENT_REQ_MSG:
      case GBI_MEDIA_INFO_REQ_MSG:
      case GBI_PARTITION_INFO_REQ_MSG:

        /* Multiple plugin involved */
        result = gbi_i_handle_plugin_multiple_req_msg(msg_p);
        break;

      default:
        /* No plugin involved */

        /* a. process message */
        /* b. send response message */
        /* c. return status */

        result = GBI_OK;
        break;
    }
  }

  // GBI_SEND_TRACE_PARAM("gbi_i_handle_req_msg, result", result, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  return result;
}

/**
 * Component: Response handler
 * Function:  gbi_i_handle_resp_msg
 *
 * The 'Response Handler' component processes all the plugin response messages.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_BUSY
 */
T_GBI_RESULT  gbi_i_handle_resp_msg(T_RV_HDR *msg_p)
{
  T_GBI_RESULT          result               = GBI_OK;
  BOOL                  msg_id_found         = FALSE;
  UINT8                 i,j;
  UINT8                 plugin_idx = 0;
  T_GBI_RESULT_RSP_MSG  *rsp_msg_p;
  T_GBI_PLUGIN_FNCTBL   const *func_ptr;
  T_GBI_PLUGIN_INFO     plugin_info;

  if (msg_p == NULL)
  {
    /* Safety action */
    GBI_SEND_TRACE("Message is a null pointer", RV_TRACE_LEVEL_ERROR);
    return GBI_INTERNAL_ERR;
  }

  /* Only needed for result testing */
  rsp_msg_p = (T_GBI_RESULT_RSP_MSG *)msg_p;

  // GBI_SEND_TRACE_PARAM("gbi_i_handle_resp_msg, msg_p->msg_id", msg_p->msg_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);
  for (i = 0; i < GBI_NMB_PLUGINS && msg_id_found == FALSE; i++)
  {
    for (j = 0; j < MAX_NR_OF_GROUP_MSG && msg_id_found == FALSE; j++)
    {
      if(message_id_groups[i][j] == msg_p->msg_id)
      {
        /* Message ID found is in this group */
        msg_id_found = TRUE;
        plugin_idx    = i;
      }
    }
  }

  if (msg_id_found == TRUE)
  {
    func_ptr = gbi_plugin_fnctbl[plugin_idx];
    result   = func_ptr->gbi_plugin_rsp_msg(msg_p);

    if (result == GBI_OK)
    {
      /* Mark Plug-in as ready */
      plugin_info = gbi_i_get_plugin_status_info(plugin_idx);

      plugin_info.plugin_state        = GBI_STATE_READY;
      plugin_info.msg_id_in_progress  = GBI_NO_MESSAGE_IN_PROGRESS;

      gbi_i_set_plugin_status_info(plugin_idx, plugin_info);

      /* Check whether all involved plugins are ready */
      if (gbi_i_check_if_all_plugins_ready(msg_p) == TRUE)
      {
        /* Use stored multiple msg in progress for creating response message */
        result = gbi_i_all_plugins_ready_multiple_req(
          gbi_env_ctrl_blk_p->multiple_msg_in_progress,
          gbi_env_ctrl_blk_p->multiple_msg_mb_id,
          gbi_env_ctrl_blk_p->multiple_msg_ret_path,
          msg_p);

        /* return OK */
        result = GBI_OK;
      }
      else
      {
        /* Plugin is ready, plugin already  */

        /* return OK */
        result = GBI_OK;
      }


/*send the GBI event to its subsrcibers after the first pass of the GC is completed*/
      if((msg_p->msg_id == GBI_NAND_GC_MSG) && (first_GC_event == TRUE))
      	{
		first_GC_event = FALSE;

/*since the NAN plugin initialisation is post poned we need to inform the media state to the subscribers*/
	    gbi_i_media_change_event(PLUGIN_NMB_NAND, GBI_EVENT_NAN_MEDIA_AVAILABLE);

           GBI_SEND_TRACE ("GBI : Sending event GBI_EVENT_NAN_MEDIA_AVAILABLE", RV_TRACE_LEVEL_ERROR);

      	}


    }
    else if (result == GBI_PROCESSING)
    {
      /* No error, but also not ready */
      result = GBI_OK;
    }
    else if (result == GBI_BUSY)
    {
      GBI_SEND_TRACE ("GBI GBI_BUSY not a granted result", RV_TRACE_LEVEL_ERROR);

      /* Overrule result */
      result = GBI_INTERNAL_ERR;
    }
    else
    {
      /* Other status */

      /* Plugin is ready, mark plugin as ready  */
      plugin_info = gbi_i_get_plugin_status_info(plugin_idx);

      plugin_info.plugin_state        = GBI_STATE_READY;
      plugin_info.msg_id_in_progress  = GBI_NO_MESSAGE_IN_PROGRESS;

      gbi_i_set_plugin_status_info(plugin_idx, plugin_info);

      /* Check whether all plugins are involved are ready */
      if (gbi_i_check_if_all_plugins_ready(msg_p) == TRUE)
      {
        /* Send response status message to client */

        /* Use stored multiple msg in progress for creating response message */
        result = gbi_i_all_plugins_ready_multiple_req(
          gbi_env_ctrl_blk_p->multiple_msg_in_progress,
          gbi_env_ctrl_blk_p->multiple_msg_mb_id,
          gbi_env_ctrl_blk_p->multiple_msg_ret_path,
          msg_p);


        /* Overrule result of previous action, return saved status */
        result = GBI_INTERNAL_ERR;
      }
      else
      {
        /* return OK */
        result = GBI_OK;
      }
    }
  }
  else
  {
    GBI_SEND_TRACE("No response message found in message ID group", RV_TRACE_LEVEL_ERROR);
    GBI_SEND_TRACE_PARAM("rsp_msg_p->hdr.src_addr_id", rsp_msg_p->hdr.src_addr_id, RV_TRACE_LEVEL_ERROR);
    GBI_SEND_TRACE_PARAM("rsp_msg_p->hdr.dest_addr_id", rsp_msg_p->hdr.dest_addr_id, RV_TRACE_LEVEL_ERROR);
    GBI_SEND_TRACE_PARAM("rsp_msg_p->hdr.msg_id", rsp_msg_p->hdr.msg_id, RV_TRACE_LEVEL_ERROR);

    result = GBI_INTERNAL_ERR;
  }

  return result;
}


/**
 * Component: Message Distribution
 * Sub-function:  gbi_i_msg_is_request_message
 *
 * Check if message is a GBI request message
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  TRUE or FALSE
 */
static BOOL gbi_i_msg_is_request_message (T_RV_HDR *msg_p)
{
  BOOL result = FALSE;

  //GBI_SEND_TRACE_PARAM("gbi_i_msg_is_request_message, msg_p->msg_id", msg_p->msg_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (msg_p != NULL)
  {
    switch (msg_p->msg_id)
    {
      case GBI_READ_REQ_MSG:
      case GBI_WRITE_REQ_MSG:
      case GBI_WRITE_WITH_SPARE_REQ_MSG:
      case GBI_ERASE_REQ_MSG:
      case GBI_FLUSH_REQ_MSG:
      case GBI_MEDIA_INFO_REQ_MSG:
      case GBI_PARTITION_INFO_REQ_MSG:
      case GBI_READ_SPARE_DATA_REQ_MSG:
      case GBI_I_SUBSCRIBE_EVENT_REQ_MSG:
      case GBI_I_STARTUP_EVENT_REQ_MSG:
        /* Message is a request message */
        result = TRUE;
        break;
      default:
        result = FALSE;
        break;
    }
  }
  //GBI_SEND_TRACE_PARAM("gbi_i_msg_is_request_message, result", result, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  return result;
}

/**
 * Component:    Message Backup
 * Sub-function: gbi_i_get_message
 *
 * This function retrieves a saved message.
 * The function returns TRUE if a message is returned. If the  next queued
 * message pointer is the same as the parameter, FALSE is returned. If the
 * parameter is NULL, any waiting message is returned.
 *
 * @param   query_msg_p   Pointer on the header of the message.
 * @return  TRUE or FALSE
 */
static BOOL gbi_i_get_message (T_RV_HDR **query_msg_p, UINT8 *recover_index_p)
{
  T_RV_HDR *saved_msg;

  if (*recover_index_p >= MAX_NR_OF_QUEUED_MSG) return FALSE; //boundary check

  /* Get saved message from queue */
  saved_msg = message_queue[*recover_index_p];


  if (saved_msg == NULL)
  {
    /* No messages in queue */
    return FALSE;
  }
  else
  {
    /* There are messages in the queue to process */
    if (*query_msg_p == saved_msg)
    {
      /* Message is just placed in queue*/
      return FALSE;
    }
    else
    {
      /* Saved message could be handled */
      *query_msg_p = saved_msg;

      //GBI_SEND_TRACE_PARAM("GBI: Reprocessing saved message", (UINT32)query_msg_p, RV_TRACE_LEVEL_DEBUG_MEDIUM);
       return TRUE;
    }
  }
}

/**
 * Component:    Message Backup
 * Sub-function: gbi_i_remove_saved_message
 *
 * This function removes a saved message from the queue
 *
 * @param   recover_index   The index of the message in the queue.
 * @return  Nothing
 */
static void gbi_i_remove_saved_message(UINT8 recover_index)
{
  UINT8 i, next_msg_idx;

  if (recover_index >= MAX_NR_OF_QUEUED_MSG) return; //boundary check

  /* Re-order FIFO queue */
  i = recover_index;
  for (; i < MAX_NR_OF_QUEUED_MSG; i++)
  {
    if (i + 1 < MAX_NR_OF_QUEUED_MSG)
    {
      /* Not last entry of queue reached */
      next_msg_idx = i + 1;
      message_queue[i] = message_queue[next_msg_idx];
    }
    else
    {
      /* Last entry of queue reached, clear entry */
      message_queue[i] = NULL;
    }
  }

}

/**
 * Component:    Message Backup
 * Sub-function: gbi_i_save_message
 *
 * This function saves a message
 *
 * @param   query_msg_p   Pointer on the header of the message.
 * @return  Nothing
 */
static void gbi_i_save_message (T_RV_HDR *msg_p)
{
  UINT8   i;
  BOOL    found=FALSE;

  /* Search first free empty entry in FIFO queue, so this msg becomes
   * last in
   */
  for (i = 0; i < MAX_NR_OF_QUEUED_MSG; i++)
  {
    if ( message_queue[i] == NULL)
    {
      /* Entry is empty, store message  */
      message_queue[i] = msg_p;
      found = TRUE;
      break;
    }
  }
  if (found == FALSE)
  {
    GBI_SEND_TRACE("GBI gbi_i_save_message: QUEUE FULL", RV_TRACE_LEVEL_DEBUG_HIGH);
  }
  return;
}




static short int gbi_get_plugin_num(unsigned short media_num)
{
 UINT8                 plugin_idx;
 UINT8				   media_idx;
 T_GBI_MEDIA_INFO	  *temp_gbi_ptr=NULL;



 for(plugin_idx=0;plugin_idx<GBI_NMB_PLUGINS;plugin_idx++)
 	{
 	  for(media_idx=0; media_idx < *((UINT8*)(gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p));media_idx++)
 	  	{
 	  	  temp_gbi_ptr= ((T_GBI_MEDIA_INFO*) (gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[media_idx]));
		  if(temp_gbi_ptr->media_nmb == media_num)
		  	{
				return plugin_idx;  /* found media number and plugin index */
		  	}
 	  	}  /* Medias */

 	} /* plugin numbers */

  //return -1; 
  return 0;//making it return 0 for invalid media number
}


/**
 * Component:    Request handler
 * Sub-function: gbi_i_handle_plugin_single_req_msg
 *
 * The 'Request Handler' component processes all the single API request messages.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_BUSY
 */
static T_GBI_RESULT  gbi_i_handle_plugin_single_req_msg(T_RV_HDR *msg_p)
{
  T_GBI_RESULT          result = GBI_OK;
  short int                 plugin_idx = -1;
  T_GBI_SINGLE_REQ_MSG  *plugin_msg_p;
  T_GBI_PLUGIN_FNCTBL   const *func_ptr;
  T_GBI_PLUGIN_INFO     plugin_info;

  // GBI_SEND_TRACE_PARAM("gbi_i_handle_plugin_single_req_msg, msg_p->msg_id", msg_p->msg_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  plugin_msg_p = (T_GBI_SINGLE_REQ_MSG *)msg_p;

  plugin_idx = gbi_get_plugin_num(plugin_msg_p->media_nmb);

  if(plugin_idx == -1)
  	{
  	    GBI_SEND_TRACE("gbi_i_handle_plugin_single_req_msg, Invalid plugin index", RV_TRACE_LEVEL_ERROR);
		return GBI_INTERNAL_ERR;
  	}


  func_ptr = gbi_plugin_fnctbl[plugin_idx];
  result   = func_ptr->gbi_plugin_reg_msg(msg_p);

  plugin_info = gbi_i_get_plugin_status_info(plugin_idx);

  /* Evaluate plugin status */
  if (result == GBI_OK)
  {
    /* Mark Plug-in is ready or keep it as ready */
    plugin_info.plugin_state        = GBI_STATE_READY;

    gbi_i_set_plugin_status_info(plugin_idx, plugin_info);
  }
  else if (result == GBI_PROCESSING)
  {
    /* Mark Plug-in as not-ready and remember plugin msg */
      plugin_info.plugin_state        = GBI_STATE_PROCESSING;
      plugin_info.msg_id_in_progress  = msg_p->msg_id;

    gbi_i_set_plugin_status_info(plugin_idx, plugin_info);

    /* Return status GBI_OK */
    result = GBI_OK;
  }
  else if (result == GBI_BUSY)
  {
    /* Only return status for single request */
  }
  else
  {
    /* Plugin status is another error, status to client already send */
  }

  return result;
}


/**
 * Component:    Request handler
 * Sub-function: gbi_i_handle_plugin_multiple_req_msg
 *
 * The 'Request Handler' component processes all the multiple API request messages.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_BUSY
 */
static T_GBI_RESULT  gbi_i_handle_plugin_multiple_req_msg(T_RV_HDR *msg_p)
{
  T_GBI_RESULT          result = GBI_OK;
  UINT8                 plugin_idx, start_idx;
  BOOL                  last_plugin_handled  = FALSE;
  T_GBI_PLUGIN_FNCTBL   const *func_ptr;
  T_GBI_PLUGIN_INFO     plugin_info;
  void                  **data_p      = NULL;
  void                  **nmb_items_p = NULL;
  T_MULTIPLE_REQ_MSG    *req_msg_p;

  // GBI_SEND_TRACE_PARAM("gbi_i_handle_plugin_multiple_req_msg, msg_p->msg_id", msg_p->msg_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  /* Only process if it is a multiple request, which equals this request message in progress
   * OR it is a multiple request and there is no multiple request message in progress
   */
  if ((gbi_env_ctrl_blk_p->multiple_msg_in_progress != GBI_NO_MESSAGE_IN_PROGRESS) &&
      (gbi_env_ctrl_blk_p->multiple_msg_in_progress != msg_p->msg_id))
  {
    /* Another multiple request message then the one in progress */

    return GBI_BUSY;
  }

  /* Start with last busy plugin number, if set */
  if (gbi_env_ctrl_blk_p->plugin_nmb_in_progress != GBI_NO_PLUGIN_IN_PROGRESS)
  {
    start_idx = gbi_env_ctrl_blk_p->plugin_nmb_in_progress;
  }
  else
  {
    /* No plugin busy with this request, so start with plugin number 0 */
    start_idx = 0;
  }

  /* For all plugins */
  for (plugin_idx = start_idx;
      (plugin_idx < GBI_NMB_PLUGINS && last_plugin_handled == FALSE);
       plugin_idx++)
  {
    result = GBI_OK;
    req_msg_p = (T_MULTIPLE_REQ_MSG *)msg_p;

    switch (msg_p->msg_id)
    {
      case GBI_I_STARTUP_EVENT_REQ_MSG:
#if (CHIPSET !=15)
	if(plugin_idx != PLUGIN_NMB_NAND)
#endif
	{
        /* Store this multiple request message as being in progress */
        gbi_env_ctrl_blk_p->multiple_msg_in_progress            = msg_p->msg_id;
        gbi_env_ctrl_blk_p->multiple_msg_mb_id                  = GBI_NO_MB_ID;
        gbi_env_ctrl_blk_p->multiple_msg_ret_path.addr_id       = 0;
        gbi_env_ctrl_blk_p->multiple_msg_ret_path.callback_func = NULL;

        func_ptr = gbi_plugin_fnctbl[plugin_idx];
        result   = func_ptr->gbi_plugin_reg_multiple_msg(msg_p, data_p, nmb_items_p, gbi_env_ctrl_blk_p->multiple_msg_in_progress);
	}

        break;

      case GBI_MEDIA_INFO_REQ_MSG:

        /* Store this multiple request message as being in progress */
        gbi_env_ctrl_blk_p->multiple_msg_in_progress = msg_p->msg_id;
        gbi_env_ctrl_blk_p->multiple_msg_mb_id       = req_msg_p->media_info_msg.mb_id;
        gbi_env_ctrl_blk_p->multiple_msg_ret_path    = req_msg_p->media_info_msg.return_path;

        /* Multiple plugins, media or partition info request */
      //  if (gbi_i_check_if_media_changed(plugin_idx))
      //  {
          data_p      = &gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[0];
          nmb_items_p = &gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p;

          func_ptr    = gbi_plugin_fnctbl[plugin_idx];






             result      = func_ptr->gbi_plugin_reg_multiple_msg(msg_p, data_p, nmb_items_p, gbi_env_ctrl_blk_p->multiple_msg_in_progress);

      //  }

        break;

      case GBI_PARTITION_INFO_REQ_MSG:

        /* Store this multiple request message as being in progress */
        gbi_env_ctrl_blk_p->multiple_msg_in_progress = msg_p->msg_id;
        gbi_env_ctrl_blk_p->multiple_msg_mb_id       = req_msg_p->partition_info_msg.mb_id;
        gbi_env_ctrl_blk_p->multiple_msg_ret_path    = req_msg_p->partition_info_msg.return_path;

        /* Multiple plugins, media or partition info request */
       // if (gbi_i_check_if_media_changed(plugin_idx))
       // {
          data_p      = &gbi_env_ctrl_blk_p->plugin_table[plugin_idx].partition_data_p[0];
          nmb_items_p = &gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_partition_p;

          func_ptr = gbi_plugin_fnctbl[plugin_idx];


          result   = func_ptr->gbi_plugin_reg_multiple_msg(msg_p, data_p, nmb_items_p, gbi_env_ctrl_blk_p->multiple_msg_in_progress);

       // }

        break;

      default:
        GBI_SEND_TRACE("gbi_i_handle_plugin_multiple_req_msg: wrong request", RV_TRACE_LEVEL_ERROR);
        result = GBI_INTERNAL_ERR;
        break;
    }

    plugin_info = gbi_i_get_plugin_status_info(plugin_idx);


    /* Evaluate plugin status */
    if (result == GBI_OK)
    {
      /* Handle plugin result */

      /* Mark Plug-in is ready or keep it as ready */
      plugin_info.plugin_state = GBI_STATE_READY;
      plugin_info.msg_id_in_progress  = GBI_NO_MESSAGE_IN_PROGRESS;
      gbi_i_set_plugin_status_info(plugin_idx, plugin_info);
    }
    else if (result == GBI_PROCESSING)
    {
      /* Mark Plug-in as not-ready and remember plugin msg */
      plugin_info.plugin_state        = GBI_STATE_PROCESSING;
      plugin_info.msg_id_in_progress  = msg_p->msg_id;
      gbi_i_set_plugin_status_info(plugin_idx, plugin_info);
    }
    else if (result == GBI_BUSY)
    {
      if (gbi_env_ctrl_blk_p->multiple_msg_in_progress != GBI_NO_MESSAGE_IN_PROGRESS)
      {
        /* Plugin status is busy, return result */
        /* Remember plugin msg and plugin number to continue from the next
         * time this request comes in
         */
        plugin_info.plugin_state        = GBI_STATE_BUSY;
        plugin_info.msg_id_in_progress  = msg_p->msg_id;
        gbi_i_set_plugin_status_info(plugin_idx, plugin_info);

        gbi_env_ctrl_blk_p->plugin_nmb_in_progress = plugin_idx;
      }

      /* Quit for-loop */
      break;
    }
    else
    {
      /* Plugin status is another error */

      /* Quit for-loop */
      break;
    }

    if (plugin_idx == (GBI_NMB_PLUGINS - 1))
    {
      last_plugin_handled = TRUE;
    }
  }

  /* All plugins ready, this means plugin_idx equals GBI_NMB_PLUGINS */
  if (last_plugin_handled == TRUE)
  {
    /* Post processing: E.g. Gather media and partition information */

    if (gbi_i_check_if_all_plugins_ready(msg_p) == TRUE)
    {
      /*  Handle plugin result when all plugins are ready with this multiple
       *  message: gather all information, update gbi states create and send
       *  response message
       */

      result = gbi_i_all_plugins_ready_multiple_req(
        gbi_env_ctrl_blk_p->multiple_msg_in_progress,
        gbi_env_ctrl_blk_p->multiple_msg_mb_id,
        gbi_env_ctrl_blk_p->multiple_msg_ret_path,
        msg_p);
      result = GBI_OK;
    }
  }

  //GBI_SEND_TRACE_PARAM("gbi_i_handle_plugin_multiple_req_msg, result", result, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  return result;
}

/**
 * Component:    General support function
 * Sub-function: gbi_i_all_plugins_ready_multiple_req
 *
 * This function handles the plugin result when all plugins are ready with this
 * multiple message: gather all information, update gbi states create and send
 * response message
 *
 * @param   req_msg_id    Pointer on the header of the message.
 * @param   req_mb_id     Pointer on the header of the message.
 * @param   req_ret_path  Pointer on the header of the message.
 * @return  GBI_OK, GBI_MEMORY_ERR
 */
static T_GBI_RESULT gbi_i_all_plugins_ready_multiple_req(UINT32       req_msg_id,
                                                         T_RVF_MB_ID  req_mb_id,
                                                         T_RV_RETURN  req_ret_path,
                                                         T_RV_HDR *msg_p)
{
  T_GBI_RESULT  result = GBI_OK;
  UINT8         plugin_idx = 0;
  UINT8         ptr_cnt;
  UINT8         *nmb_of_media_p;
  UINT8         nmb_of_media = 0;
  UINT8         tot_nmb_of_media = 0;
  UINT8         *nmb_of_partitions_p;
  UINT8         nmb_of_partitions = 0;
  UINT8         tot_nmb_of_partitions = 0;
  T_GBI_PARTITION_INFO_RSP_MSG    *part_rsp_p;
  T_GBI_PARTITION_INFO            *partition_info_p     = NULL;
  T_GBI_PARTITION_INFO            *composed_part_info_p = NULL;
  T_GBI_PARTITION_INFO            *plugin_part_info_p   = NULL;
  T_GBI_MEDIA_INFO_RSP_MSG        *media_rsp_p;
  T_GBI_MEDIA_INFO                *media_info_p = NULL;
  T_GBI_MEDIA_INFO                *composed_media_info_p  = NULL;
  T_RV_RETURN                     return_path;
  T_RVF_MB_STATUS                 mb_status;
  void                            *data_p= NULL;
  UINT32                          buffer_size;
#if (CHIPSET!=15)
   T_GBI_PLUGIN_FNCTBL   const *func_ptr;
   void                  **nmb_items_p = NULL;
#endif

  // GBI_SEND_TRACE("gbi_i_all_plugins_ready_multiple_req", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  /* Clear multiple request information */
  gbi_env_ctrl_blk_p->multiple_msg_in_progress = GBI_NO_MESSAGE_IN_PROGRESS;
  gbi_env_ctrl_blk_p->plugin_nmb_in_progress   = GBI_NO_PLUGIN_IN_PROGRESS;

  /* gather all information of the multple request: Get media info or partition
   * information, by using for all plugin's stored data_p and nmb_item_p
   * The information should be copied into the memory bank of the client
   */

  if (req_msg_id == GBI_I_STARTUP_EVENT_REQ_MSG)
  {
    gbi_env_ctrl_blk_p->state = GBI_OPERATIONAL;

    //gbi_operational_time = rvf_get_tick_count( );
    //GBI_TRACE_PARAM("GBI : GBI Initialisation time", RVF_TICKS_TO_MS ((gbi_operational_time - gbi_start_time)), RV_TRACE_LEVEL_ERROR);
#if (CHIPSET!=15)
    func_ptr = gbi_plugin_fnctbl[PLUGIN_NMB_NAND];
    result   = func_ptr->gbi_plugin_reg_multiple_msg(msg_p, data_p, nmb_items_p, req_msg_id);
#endif
  }
  else if (req_msg_id == GBI_MEDIA_INFO_REQ_MSG)
  {
    for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
    {
      if (gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p != NULL)
      {
        /* Get the content */
        nmb_of_media_p    = (UINT8 *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p;
        tot_nmb_of_media += *nmb_of_media_p;
      }

      // GBI_SEND_TRACE_PARAM("tot_nmb_of_media", tot_nmb_of_media, RV_TRACE_LEVEL_DEBUG_MEDIUM);
    }

    /* Get buffer for amount of partitions as calculated on memory bank*/
    buffer_size = tot_nmb_of_media * sizeof(T_GBI_MEDIA_INFO);
    if (buffer_size != 0)
    {
      mb_status = rvf_get_buf(req_mb_id, buffer_size,
          (T_RVF_BUFFER**) &media_info_p);

      if (mb_status == RVF_RED)
      {
        /*
         * The flag returned by rvf_get_buf is red, there is not enough
         * memory to allocate the buffer.
         * The environemnt will cancel the CRY instance creation.
         */
        GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);
        return GBI_MEMORY_ERR;
      }

      /* Set pointer at start of partition info memory */
      composed_media_info_p = media_info_p;

      for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
      {
        if (gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[0] != NULL)
        {
          nmb_of_media_p = (UINT8 *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p;
          if (nmb_of_media_p == NULL)
          {
            nmb_of_media   = 0;
          }
          else
          {
            nmb_of_media   = *nmb_of_media_p;
          }

          /* Get the content of the stored pointers */
          for (ptr_cnt = 0; ptr_cnt < nmb_of_media; ptr_cnt++)
          {
            data_p = (void *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[ptr_cnt];

            /* Copy data for amount of this plugins partitions to allocated memory */
            memcpy(composed_media_info_p, data_p, sizeof(T_GBI_MEDIA_INFO));

/*
            GBI_SEND_TRACE_PARAM("composed_media_info_p->media_id", composed_media_info_p->media_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_media_info_p->media_nmb", composed_media_info_p->media_nmb, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_media_info_p->media_pressent", composed_media_info_p->media_pressent, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_media_info_p->media_type", composed_media_info_p->media_type, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_media_info_p->read_speed", composed_media_info_p->read_speed, RV_TRACE_LEVEL_DEBUG_MEDIUM);
*/
            /* Adjust pointer */
            composed_media_info_p++;
          }
        }
      }
    }
    else
    {
      media_info_p      = NULL;
      tot_nmb_of_media  = 0;
    }

    GBI_SEND_TRACE("gbi_get_media_info_resp FRAMEWORK result", RV_TRACE_LEVEL_ERROR);

    if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_GBI_MEDIA_INFO_RSP_MSG),
                         GBI_MEDIA_INFO_RSP_MSG,
                         (T_RV_HDR **) &media_rsp_p) == RVF_RED)
    {
      GBI_SEND_TRACE ("GBI gbi_get_media_info_resp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return GBI_MEMORY_ERR;
    }

    /* compose message */
    media_rsp_p->hdr.msg_id     = GBI_MEDIA_INFO_RSP_MSG;
    media_rsp_p->result         = GBI_OK;
    media_rsp_p->nmb_of_media   = tot_nmb_of_media;
    media_rsp_p->info_p         = media_info_p;
    return_path                 = req_ret_path;

    /* Send message mailbox */
    result = gbi_i_generic_client_response ((T_RV_HDR *) media_rsp_p, return_path);

  }
  else if (req_msg_id == GBI_PARTITION_INFO_REQ_MSG)
  {
    for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
    {
      if (gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_partition_p != NULL)
      {
        /* Get the content */
        nmb_of_partitions_p = (UINT8 *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_partition_p;
        tot_nmb_of_partitions += *nmb_of_partitions_p;
      }

      //GBI_SEND_TRACE_PARAM("tot_nmb_of_partitions ", tot_nmb_of_partitions, RV_TRACE_LEVEL_DEBUG_MEDIUM);
    }

    /* Get buffer for amount of partitions as calculated on memory bank*/
    buffer_size = tot_nmb_of_partitions * sizeof(T_GBI_PARTITION_INFO);
    if (buffer_size != 0)
    {
      mb_status = rvf_get_buf(req_mb_id, buffer_size,
          (T_RVF_BUFFER**) &partition_info_p);

      if (mb_status == RVF_RED)
      {
        /*
         * The flag returned by rvf_get_buf is red, there is not enough
         * memory to allocate the buffer.
         * The environemnt will cancel the CRY instance creation.
         */
        GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);
        return GBI_MEMORY_ERR;
      }

      /* Set pointer at start of partition info memory */
      composed_part_info_p = partition_info_p;

      for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
      {
        plugin_part_info_p = composed_part_info_p;

        if (gbi_env_ctrl_blk_p->plugin_table[plugin_idx].partition_data_p[0] != NULL)
        {
          nmb_of_partitions_p = (UINT8 *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_partition_p;
          if (nmb_of_partitions_p == NULL)
          {
            nmb_of_partitions   = 0;
          }
          else
          {
            nmb_of_partitions   = *nmb_of_partitions_p;
          }

          /* Get the content of the stored pointers */
          for (ptr_cnt = 0; ptr_cnt < nmb_of_partitions; ptr_cnt++)
          {
            data_p = (void *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].partition_data_p[ptr_cnt];

            /* Copy data for amount of this plugins partitions to allocated memory */
            memcpy(composed_part_info_p, data_p, sizeof(T_GBI_PARTITION_INFO));

/*
            GBI_SEND_TRACE_PARAM("composed_part_info_p->media_nmb", composed_part_info_p->media_nmb, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_part_info_p->bytes_per_block", composed_part_info_p->bytes_per_block, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_part_info_p->filesystem_type", composed_part_info_p->filesystem_type, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_part_info_p->first_block_nmb", composed_part_info_p->first_block_nmb, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_part_info_p->last_block_nmb", composed_part_info_p->last_block_nmb, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_part_info_p->nmb_of_blocks", composed_part_info_p->nmb_of_blocks, RV_TRACE_LEVEL_DEBUG_MEDIUM);
            GBI_SEND_TRACE_PARAM("composed_part_info_p->partition_nmb", composed_part_info_p->partition_nmb, RV_TRACE_LEVEL_DEBUG_MEDIUM);
*/

            /* Adjust pointer */
            composed_part_info_p++;
          }

          result = gbi_i_create_partition_names(plugin_idx, nmb_of_partitions, plugin_part_info_p);
        }

        /*  Mark the retrieval of media and partition information as being synchronised */
        gbi_i_media_info_synchronised(plugin_idx);
      }

      result = gbi_i_create_filesystem_names(plugin_idx, tot_nmb_of_partitions , partition_info_p);
    }
    else
    {
      partition_info_p      = NULL;
      tot_nmb_of_partitions = 0;
    }


    /* A response should be send depending on the request */
    if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_GBI_PARTITION_INFO_RSP_MSG),
                         GBI_PARTITION_INFO_RSP_MSG,
                         (T_RV_HDR **) &part_rsp_p) == RVF_RED)
    {
      GBI_SEND_TRACE ("GBI gbi_get_partition_info_resp: out of memory",
                       RV_TRACE_LEVEL_WARNING);
      return GBI_MEMORY_ERR;
    }

    GBI_SEND_TRACE("gbi_get_partition_info_resp FRAMEWORK result", RV_TRACE_LEVEL_ERROR);
    /* compose message */
    part_rsp_p->hdr.msg_id         = GBI_PARTITION_INFO_RSP_MSG;
    part_rsp_p->result             = GBI_OK;
    part_rsp_p->nmb_of_partitions  = tot_nmb_of_partitions;
    part_rsp_p->info_p             = partition_info_p;
    return_path                    = req_ret_path;

    /* Send message mailbox */
    result = gbi_i_generic_client_response ((T_RV_HDR *) part_rsp_p, return_path);
  }

  gbi_env_ctrl_blk_p->multiple_msg_ret_path.addr_id       = 0;
  gbi_env_ctrl_blk_p->multiple_msg_ret_path.callback_func = NULL;
  return result;
}


/**
 * Component:    General support function
 * Sub-function: gbi_i_multiple_request_client_response
 *
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_MEMORY_ERR
 */
T_GBI_RESULT  gbi_i_create_partition_names(UINT8                  plugin_idx,
                                           UINT8                  nmb_of_partitions,
                                           T_GBI_PARTITION_INFO   *partition_info_p)
{
  T_GBI_RESULT          retval = GBI_OK;
  UINT8                 *nmb_of_media_p;
  UINT8                 nmb_of_media = 0;
  UINT8                 ptr_cnt;
  UINT8                 media_cnt, part_cnt, i;
  UINT8                 media_idx, part_idx;
  T_GBI_MEDIA_TYPE      media_type;
  T_GBI_MEDIA_INFO      *media_info_p = NULL;
  T_GBI_MEDIA_INFO      *composed_media_info_p  = NULL;
  T_GBI_PARTITION_INFO  *composed_part_info_p  = NULL;
  void                  *data_p= NULL;
  UINT8                 *part_name_p;
  T_GBI_MEDIA_NAMES     media_name_counter = GBI_MN_MMC_A0;

  /* Set pointer at start of local media memory */
  media_info_p = &media_table[0];
  composed_media_info_p = media_info_p;

  if (gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[0] != NULL)
  {
    nmb_of_media_p = (UINT8 *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].nmb_media_p;
    if (nmb_of_media_p == NULL)
    {
      nmb_of_media   = 0;
    }
    else
    {
      nmb_of_media   = *nmb_of_media_p;
    }

    /* Get the content of the stored pointers */
    for (ptr_cnt = 0; ptr_cnt < nmb_of_media; ptr_cnt++)
    {
      data_p = (void *) gbi_env_ctrl_blk_p->plugin_table[plugin_idx].media_data_p[ptr_cnt];

      /* Copy data for amount of this plugins partitions to allocated memory */
      memcpy(composed_media_info_p, data_p, sizeof(T_GBI_MEDIA_INFO));

      /* Adjust pointer */
      composed_media_info_p++;
    }
  }

  /* Search for each media type through the media table */
  for (media_type = GBI_MMC; media_type < GBI_MEDIA_TYPE_END; media_type++)
  {
    /* synchronise the media_name_counter to the media_type */
    media_name_counter = (T_GBI_MEDIA_NAMES)(media_type * GBI_NR_OF_MEDIAS_AND_PARTITIONS);
    /* Initialize media number table */
    for (media_idx = 0; media_idx < GBI_MAX_NR_OF_MEDIA; media_idx++)
    {
      media_cnt_table[media_idx] = 0xFF;
    }

    for (media_idx = 0; media_idx < nmb_of_media; media_idx++)
    {
      if (media_table[media_idx].media_type == media_type)
      {
        /* Media type found, save media number of this media type */
        media_cnt_table[media_idx] = media_table[media_idx].media_nmb;
      }
    }

    for (media_idx = 0; media_idx < nmb_of_media; media_idx++)
    {
      media_cnt = 0;
      if (media_cnt_table[media_idx] != 0xFF)
      {
        /* Media count table has stored a valid media_nmb */
        media_cnt++;

        composed_part_info_p = partition_info_p;
        part_cnt = 0;

        /* Stored media number found, search it in partition table */
        for (part_idx = 0; part_idx < nmb_of_partitions; part_idx++)
        {
          if (media_cnt_table[media_idx] == composed_part_info_p->media_nmb)
          {
            i = 0;
            if (composed_part_info_p->partition_name[i] == '\0')
            {
              /* No partition name string was created before, so create it */
              part_name_p = &composed_part_info_p->partition_name[0];
              strcpy((char *)part_name_p, part_name_table[media_type]);

              i = (UINT8)strlen(part_name_table[media_type]);

              /* Media count will be 1 or more. To obtain character 'A' it should be
               * decremented with 1
               */
              composed_part_info_p->partition_name[i] = (media_cnt - 1) +  'A';
              i++;
              composed_part_info_p->partition_name[i] = part_cnt + '0';
              i++;
              composed_part_info_p->partition_name[i] = '\0';
            }
            else
            {
              /* Partition name is previously set, no action needed */
            }
            part_cnt++;
            composed_part_info_p->partition_media_names = media_name_counter;
          }

          /* Next partition */
          composed_part_info_p++;
          media_name_counter++;
        }
      }
    }
  }

  return retval;
}


/**
 * Component:    General support function
 * Sub-function: gbi_i_create_filesystem_names
 *
 *
 *
 * @return  GBI_OK, GBI_MEMORY_ERR
 */
T_GBI_RESULT  gbi_i_create_filesystem_names(UINT8                 plugin_idx,
                                            UINT8                 nmb_of_partitions,
                                            T_GBI_PARTITION_INFO  *partition_info_p)
{
  T_GBI_RESULT          retval = GBI_OK;
  UINT8                 part_idx;
  UINT8                 fs_name_idx;
  UINT8                 i;
  T_GBI_PARTITION_INFO  *handle_part_info_p = NULL;
  BOOL                  fs_name_entry_found = FALSE;
  UINT8                 *file_system_name_p;

  handle_part_info_p = partition_info_p;

  /* Stored media number found, search it in partition table */
  for (part_idx = 0; part_idx < nmb_of_partitions; part_idx++)
  {
    /* Search file system type in macro definition and copy string the partition
     * information memory
     */
    fs_name_entry_found = FALSE;

    for (fs_name_idx = 0;
         (fs_name_idx < GBI_NMB_OF_FS_NAMES && fs_name_entry_found == FALSE);
         fs_name_idx++)
    {
      if (handle_part_info_p->filesystem_type == fs_name_table[fs_name_idx].fs_name_type)
      {
        fs_name_entry_found = TRUE;

        file_system_name_p = &handle_part_info_p->filesystem_name[0];
        strcpy((char *)file_system_name_p, fs_name_table[fs_name_idx].fs_name);

        /* End string with '\0' */
        i = (UINT8)strlen(fs_name_table[fs_name_idx].fs_name);
        handle_part_info_p->filesystem_name[i] = '\0';
      }
    }

    if (fs_name_entry_found == FALSE)
    {
      /* FS-name entry is not found, empty string */
      handle_part_info_p->filesystem_name[0] = '\0';
    }

    handle_part_info_p++;
  }

  return retval;
}

/**
 * Component:    General support function
 * Sub-function: gbi_i_check_if_all_plugins_ready
 *
 * Check whether message is still in progress by some plugins
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  TRUE, FALSE
 */
static BOOL gbi_i_check_if_all_plugins_ready(T_RV_HDR *msg_p)
{
  T_GBI_PLUGIN_INFO plugin_info;
  UINT8             plugin_idx;

  if (msg_p == NULL)
  {
    return FALSE;
  }

  for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
  {
    plugin_info = gbi_i_get_plugin_status_info(plugin_idx);

    if (plugin_info.plugin_state != GBI_STATE_READY)
    {
      /* If this message is stored in plugin status info, it is not ready yet */

      GBI_SEND_TRACE("gbi_i_check_if_all_plugins_ready, not ready yet", RV_TRACE_LEVEL_DEBUG_MEDIUM);

      return FALSE;
    }
  }

  return TRUE;
}

