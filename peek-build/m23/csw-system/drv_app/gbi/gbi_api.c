/**
 * @file  gbi_api.c
 *
 * API for GBI SWE.
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


#include "gbi/gbi_i.h"

#define TIMELOG_GBI_API
#include "rfs/time_stamp/time_stamp.h"

#define GBI_ITERATIONS		100
#define GBI_MS_DELAY		5000



/* Local function */
static BOOL gbi_available_subscribe_entries(T_GBI_EVENTS event);

/**
 * Function: gbi_read
 */
extern T_RVM_RETURN gbi_handle_message (T_RV_HDR *msg_p);
T_RV_RET gbi_read ( UINT8           media_nmb,
                    UINT8           partition_nmb,
                    T_GBI_BLOCK     first_block_nmb,
                    T_GBI_BLOCK     number_of_blocks,
                    T_GBI_BYTE_CNT  remainder_length,
                    UINT32          *buffer_p,
                    T_RV_RETURN     return_path)
{
  T_GBI_READ_REQ_MSG    *msg_p;
  T_RV_RET              retval;

#if WCP_PROF
                prf_LogPointOfInterest("GBI_API: read start");
#endif

  GBI_SEND_TRACE ("GBI API: gbi_read", RV_TRACE_LEVEL_DEBUG_LOW);
 //  TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_read);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /** Invalid return path */
    GBI_SEND_TRACE ("GBI API: Invalid return path", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_READ_REQ_MSG),
                       GBI_READ_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /** Insufficient RAM resources to process the request */
    GBI_SEND_TRACE ("GBI API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id         = GBI_READ_REQ_MSG; 
  msg_p->media_nmb          = media_nmb;
  msg_p->partition_nmb      = partition_nmb;
  msg_p->first_block_nmb    = first_block_nmb;
  msg_p->number_of_blocks   = number_of_blocks;
  msg_p->remainder_length   = remainder_length;
  msg_p->buffer_p           = buffer_p;
  msg_p->return_path        = return_path;
#if 1
  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);
 
  return retval;
#else
  return gbi_handle_message((T_RV_HDR *)msg_p);  
#endif  
}


/**
 * Function: gbi_write
 */
T_RV_RET gbi_write (UINT8           media_nmb,
                    UINT8           partition_nmb,
                    T_GBI_BLOCK     first_block_nmb,
                    T_GBI_BLOCK     number_of_blocks,
                    T_GBI_BYTE_CNT  remainder_length,
                    UINT32          *buffer_p,
                    T_RV_RETURN     return_path)
{
  T_GBI_WRITE_REQ_MSG   *msg_p;
  T_RV_RET              retval;

  GBI_SEND_TRACE ("GBI API: gbi_write", RV_TRACE_LEVEL_DEBUG_LOW);
  // TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_write);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /** Invalid return path */
    GBI_SEND_TRACE ("GBI API: Invalid return path", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_WRITE_REQ_MSG),
                       GBI_WRITE_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id         = GBI_WRITE_REQ_MSG; 
  msg_p->media_nmb          = media_nmb;
  msg_p->partition_nmb      = partition_nmb;
  msg_p->first_block_nmb    = first_block_nmb;
  msg_p->number_of_blocks   = number_of_blocks;
  msg_p->remainder_length   = remainder_length;
  msg_p->buffer_p           = buffer_p;
  msg_p->return_path        = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);
 
  return retval;
}

/**
 * Function: gbi_write
 */
T_RV_RET gbi_write_with_spare (UINT8           media_nmb,
                               UINT8           partition_nmb,
                               T_GBI_BLOCK     first_block_nmb,
                               T_GBI_BLOCK     number_of_blocks,
                               T_GBI_BYTE_CNT remainder_length,
                               UINT32         *data_buffer_p,
                               UINT32         *spare_buffer_p,
                               T_RV_RETURN     return_path)
{
  T_GBI_WRITE_WITH_SPARE_REQ_MSG    *msg_p;
  T_RV_RET              retval;

 //  TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_write_with_spare);
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /** Invalid return path */
    GBI_SEND_TRACE ("GBI API: Invalid return path", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_WRITE_WITH_SPARE_REQ_MSG),
                        GBI_WRITE_WITH_SPARE_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id         = GBI_WRITE_WITH_SPARE_REQ_MSG; 
  msg_p->media_nmb          = media_nmb;
  msg_p->partition_nmb      = partition_nmb;
  msg_p->first_block_nmb    = first_block_nmb;
  msg_p->number_of_blocks   = number_of_blocks;
  msg_p->remainder_length   = remainder_length;
  msg_p->data_buffer_p      = data_buffer_p;
  msg_p->spare_buffer_p     = spare_buffer_p;
  msg_p->return_path        = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);
 
  return retval;
}


/**
 * Function: gbi_erase
 */
T_RV_RET gbi_erase (UINT8           media_nmb,
                    UINT8           partition_nmb,
                    T_GBI_BLOCK     first_block_nmb,
                    T_GBI_BLOCK     number_of_blocks,
                    T_RV_RETURN     return_path)
{
  T_GBI_ERASE_REQ_MSG   *msg_p;
  T_RV_RET              retval;

  GBI_SEND_TRACE ("GBI API: gbi_erase", RV_TRACE_LEVEL_DEBUG_LOW);
  // TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_erase);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /** Invalid return path */
    GBI_SEND_TRACE ("GBI API: Invalid return path", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_ERASE_REQ_MSG),
                       GBI_ERASE_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI API: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id         = GBI_ERASE_REQ_MSG; 
  msg_p->media_nmb          = media_nmb;
  msg_p->partition_nmb      = partition_nmb;
  msg_p->first_block_nmb    = first_block_nmb;
  msg_p->number_of_blocks   = number_of_blocks;
  msg_p->return_path        = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);
 
  return retval;
}

/**
 * Function: gbi_flush
 */
T_RV_RET gbi_flush (UINT8           media_nmb,
                    UINT8           partition_nmb,
                    T_RV_RETURN     return_path)
{
  T_GBI_FLUSH_REQ_MSG   *msg_p;
  T_RV_RET              retval;

  GBI_SEND_TRACE ("GBI API: gbi_flush", RV_TRACE_LEVEL_DEBUG_LOW);
 //  TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_flush);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /** Invalid return path */
    GBI_SEND_TRACE ("GBI API: Invalid return path", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_FLUSH_REQ_MSG),
                       GBI_FLUSH_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_flush: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id     = GBI_FLUSH_REQ_MSG; 
  msg_p->media_nmb      = media_nmb;
  msg_p->partition_nmb  = partition_nmb;
  msg_p->return_path    = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Function: gbi_get_media_info
 */
T_RV_RET gbi_get_media_info(T_RVF_MB_ID   mb_id,
                            T_RV_RETURN   return_path)
{
  T_GBI_MEDIA_INFO_REQ_MSG  *msg_p;
  T_RV_RET                  retval;
  char 							  i=0;			

  GBI_SEND_TRACE ("GBI API: gbi_get_media_info", RV_TRACE_LEVEL_DEBUG_LOW);
  // TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_get_media_info);


#if (GBI_ITERATIONS > 0)

  for(i=0;i<GBI_ITERATIONS;i++)
  {
  	if(gbi_env_ctrl_blk_p->state == GBI_OPERATIONAL)
  		{
        break;
	   	}
     GBI_ASSERT(0);
	 rvf_delay(RVF_MS_TO_TICKS(GBI_MS_DELAY));	 	 
  } 
 
#endif

  if(gbi_env_ctrl_blk_p->state != GBI_OPERATIONAL)
 	{
 	    return RV_NOT_READY;
 	}


  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    GBI_SEND_TRACE ("GBI parameter: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  if (mb_id == RVF_INVALID_MB_ID)
  {
    /* mb_id is invalid */
    GBI_SEND_TRACE ("GBI parameter: mb_id is invalid", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_MEDIA_INFO_REQ_MSG),
                       GBI_MEDIA_INFO_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_get_media_info: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id       = GBI_MEDIA_INFO_REQ_MSG; 
  msg_p->mb_id            = mb_id;
  msg_p->return_path      = return_path;
  
  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Function: gbi_get_partition_info
 */
T_RV_RET gbi_get_partition_info(T_RVF_MB_ID   mb_id,
                                T_RV_RETURN   return_path)

{
  T_GBI_PARTITION_INFO_REQ_MSG  *msg_p;
  T_RV_RET                      retval;

  GBI_SEND_TRACE ("GBI API: gbi_get_partition_info", RV_TRACE_LEVEL_DEBUG_LOW);
 //  TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_get_partition_info);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    GBI_SEND_TRACE ("GBI API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  if (mb_id == RVF_INVALID_MB_ID)
  {
    /* mb_id is invalid */
    GBI_SEND_TRACE ("GBI parameter: mb_id is invalid", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_PARTITION_INFO_REQ_MSG),
                       GBI_PARTITION_INFO_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id       = GBI_PARTITION_INFO_REQ_MSG; 
  msg_p->mb_id            = mb_id;
  msg_p->return_path      = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}


/**
 * Function: gbi_subscribe_events
 */
T_RV_RET gbi_subscribe_events(T_GBI_EVENTS  event,
                              T_RV_RETURN   return_path)
{
  T_RV_RET                        retval = RV_OK;
  T_GBI_I_SUBSCRIBE_EVENT_REQ_MSG *msg_p;
  BOOL                            free_entries = FALSE;
  char 							  i=0;						

  GBI_SEND_TRACE ("GBI API: gbi_subscribe_events", RV_TRACE_LEVEL_DEBUG_LOW);
  // TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_subscribe_events);


#if (GBI_ITERATIONS > 0)

  for(i=0;i<GBI_ITERATIONS;i++)
  {
  	if(gbi_env_ctrl_blk_p->state == GBI_OPERATIONAL)
  		{
        break;
	   	}
     GBI_ASSERT(0);
	 rvf_delay(RVF_MS_TO_TICKS(GBI_MS_DELAY));	 	 
  } 

#endif

 if(gbi_env_ctrl_blk_p->state != GBI_OPERATIONAL)
 	{
 	    return RV_NOT_READY;
 	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    GBI_SEND_TRACE ("GBI parameter: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  free_entries = gbi_available_subscribe_entries(event);  
  if (free_entries == FALSE)
  {
    /* There are no more subscription entries for this client */
    GBI_SEND_TRACE ("GBI parameter: Amount of subscribers reached", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_I_SUBSCRIBE_EVENT_REQ_MSG),
                       GBI_I_SUBSCRIBE_EVENT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id       = GBI_I_SUBSCRIBE_EVENT_REQ_MSG; 
  msg_p->event            = event;
  msg_p->return_path      = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/**
 * Function: gbi_read_spare_data
 */
T_RV_RET gbi_read_spare_data( UINT8         media_nmb,
                              UINT8         partition_nmb,
                              T_GBI_BLOCK   first_block,
                              T_GBI_BLOCK   number_of_blocks,
                              UINT32        *info_data_p,
                              T_RV_RETURN   return_path)
{ 
  T_GBI_READ_SPARE_DATA_REQ_MSG  *msg_p;
  T_RV_RET                       retval;

//  GBI_SEND_TRACE ("GBI API: gbi_read_spare_data", RV_TRACE_LEVEL_DEBUG_LOW);
  // TS_StoreCurrentTimeStamp_Turbo (TIMELOG_gbi_read_spare_data);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    GBI_SEND_TRACE ("GBI API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RV_INVALID_PARAMETER;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_READ_SPARE_DATA_REQ_MSG),
                       GBI_READ_SPARE_DATA_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI API: out of memory",
                    RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id       = GBI_READ_SPARE_DATA_REQ_MSG; 
  msg_p->media_nmb        = media_nmb;
  msg_p->partition_nmb    = partition_nmb;
  msg_p->first_block      = first_block;
  msg_p->number_of_blocks = number_of_blocks;
  msg_p->info_data_p      = info_data_p;
  msg_p->return_path      = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}




/**
 * Function: gbi_get_sw_version
 */
UINT32 gbi_get_sw_version(void)
{
  GBI_SEND_TRACE ("GBI API: gbi_get_sw_version", RV_TRACE_LEVEL_DEBUG_LOW);

  return BUILD_VERSION_NUMBER(GBI_MAJOR, GBI_MINOR, GBI_BUILD);
}

/**
 * Function: gbi_get_sw_version
 */
static BOOL gbi_available_subscribe_entries(T_GBI_EVENTS event)
{
  UINT8 j = 0;

  /* Evaluate GBI_EVENT_MEDIA_INSERT event */
  if ((event & GBI_EVENT_MASK_0) == GBI_EVENT_MEDIA_INSERT)
  {
    /* Check if any of the subscription entries of this event is empty */
    for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
    {
      if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.callback_func == NULL) &&
          (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_0][j].ret_path.addr_id == 0))
      {
        return TRUE;
      }
    }
    /* No empty entry is found */
    return FALSE;
  }

  if ((event & GBI_EVENT_MASK_1) == GBI_EVENT_MEDIA_REMOVEAL)
  {
    /* Evaluate GBI_EVENT_MEDIA_REMOVEAL event */
    for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
    {
      /* Check if any of the subscription entries of this event is empty */
      if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.callback_func == NULL) &&
          (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_1][j].ret_path.addr_id == 0))
      {
        return TRUE;
      }        
    }
    /* No empty entry is found */
    return FALSE;
  }

  if ((event & GBI_EVENT_MASK_2) == GBI_EVENT_NAN_MEDIA_AVAILABLE)
  {
    /* Evaluate GBI_EVENT_MEDIA_REMOVEAL event */
    for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
    {
      /* Check if any of the subscription entries of this event is empty */
      if ((gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.callback_func == NULL) &&
          (gbi_env_ctrl_blk_p->sub_events[GBI_EVENT_2][j].ret_path.addr_id == 0))
      {
        return TRUE;
      }        
    }
    /* No empty entry is found */
    return FALSE;
  }


  return TRUE;
}  

