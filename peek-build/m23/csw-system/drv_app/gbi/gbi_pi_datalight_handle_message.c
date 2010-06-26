#include <string.h>
#include "typedefs.h"
#include "gbi_i.h"
#include "gbi_pi_datalight_i.h"
#include "gbi_operations.h"
#include "datalight/datalight_api.h"
#include "gbi_pi_datalight_i.h"




/**
 * Component:    Plugin Request Handling
 * Sub-function: gbi_plugin_datalight_nor_reg_multiple_msg
 *
 * This function handles multiple plugin request messages concerning gathering
 * data.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @param   void    Pointer for storage of data
 * @param   void    Pointer for storage of data
 * @return  GBI_OK, GBI_BUSY
 */
T_GBI_RESULT  gbi_plugin_datalight_reg_multiple_msg(T_RV_HDR *msg_p, void **data_p, void **nmb_items_p, UINT32 req_msg_id)
{
  T_GBI_RESULT        result = GBI_OK;
  T_GBI_PLUGIN_INFO   plugin_info;
  T_RV_RETURN_PATH    gbi_return_path;
 

  if ((gbi_datalight_env_ctrl_blk_p != NULL) && (gbi_datalight_env_ctrl_blk_p->initialised == TRUE))
  {
    /* Handle request */
    GBI_SEND_TRACE("datalight plugin_status: started",RV_TRACE_LEVEL_DEBUG_MEDIUM);

    plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_DATALIGHT);
    if (plugin_info.plugin_state == GBI_STATE_PROCESSING)
    {
      return GBI_BUSY;
    }
    
    result = gbi_plugin_datalight_handle_multiple_request(msg_p, data_p, nmb_items_p); 
  }
  else
  {
    GBI_SEND_TRACE("datalight plugin_status: not started",RV_TRACE_LEVEL_DEBUG_MEDIUM);

    if (msg_p->msg_id == GBI_I_STARTUP_EVENT_REQ_MSG)
    {
  
     if(gbi_datalight_env_ctrl_blk_p == NULL)
      	{
      	    if((result=gbi_plugin_datalight_start()) != GBI_OK)
      	    	{
      	    	     GBI_SEND_TRACE("Memory Error gbi_plugin_datalight_nand_reg_multiple_msg",RV_TRACE_LEVEL_DEBUG_ERROR);
      	    	}
      	}
	  gbi_datalight_env_ctrl_blk_p->initialised = TRUE; 

     
    }
    else
    {
      return GBI_BUSY;
    }
  }
  
  if(result == GBI_OK)
  {
	plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_DATALIGHT);
	plugin_info.plugin_state = GBI_STATE_READY;
	gbi_i_set_plugin_status_info(PLUGIN_NMB_DATALIGHT, plugin_info);
  }
  
  return result;
}



/**
 * Component:    Plugin Request Handling
 * Sub-function: gbi_plugin_datalight_reg_msg
 *
 * This function handles all single plugin request messages
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_BUSY
 */
T_GBI_RESULT  gbi_plugin_datalight_reg_msg(  T_RV_HDR *msg_p)
{

  T_GBI_RESULT      result = GBI_OK;
  T_GBI_PLUGIN_INFO plugin_info;
 

  //GBI_SEND_TRACE("gbi_plugin_datalight_reg_msg", RV_TRACE_LEVEL_DEBUG_MEDIUM);

#ifdef DL_DBG_TRACE
  GBI_SEND_TRACE("<DBG>entering gbi_plugin_datalight_reg_msg\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /*DL_DBG_TRACE */
  

  if ((gbi_datalight_env_ctrl_blk_p != NULL) && (gbi_datalight_env_ctrl_blk_p->initialised == TRUE))
  {
    /* Handle single request */

    plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_DATALIGHT);
    if (plugin_info.plugin_state == GBI_STATE_PROCESSING)
    {
#ifdef DL_DBG_TRACE
	  GBI_SEND_TRACE("<DBG>gbi_plugin_datalight_reg_msg:GBI_STATE_PROCESSING\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif
	  return GBI_BUSY;  
    }
    result = gbi_plugin_datalight_handle_single_request(msg_p);    
  }
  else
  {
      return GBI_INTERNAL_ERR;
  }

  if (result == GBI_OK)
  {
      if(gbi_datalight_env_ctrl_blk_p->state != GBI_DATALIGHT_IDLE)
      {
      	plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_DATALIGHT);
      	plugin_info.plugin_state = GBI_STATE_PROCESSING;
      	gbi_i_set_plugin_status_info(PLUGIN_NMB_DATALIGHT, plugin_info);
      
      	return GBI_PROCESSING;
      }
      else
      {
	      /*Return GBI_OK */
	      return GBI_OK;
      }		
	  
  }
  else 
  {
    /* Return error */
    return result;
  }

}

static T_GBI_RESULT gbi_plugin_datalight_handle_multiple_request(T_RV_HDR *msg_p, void **data_p, void **nmb_items_p)
{
  T_GBI_RESULT retval = GBI_OK;

  if (msg_p != NULL)
  {
    switch (msg_p->msg_id)
    {
      case GBI_MEDIA_INFO_REQ_MSG:
	  	
        retval = gbi_get_media_info_req((T_GBI_MEDIA_INFO_REQ_MSG *) msg_p, data_p, nmb_items_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_get_media_info_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_PARTITION_INFO_REQ_MSG:
        retval = gbi_get_partition_info_req((T_GBI_PARTITION_INFO_REQ_MSG *) msg_p, data_p, nmb_items_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_get_partition_info_req failed",RV_TRACE_LEVEL_ERROR);
        }

        break;

      default:
        
        GBI_SEND_TRACE("GBI: gbi_plugin_datalight_handle_multiple_request: Wrong request message", RV_TRACE_LEVEL_ERROR);
        retval = GBI_INTERNAL_ERR;

        break;
    }
  }
  
  return retval;
}


/**
 * Local functions 
 */
static T_GBI_RESULT gbi_plugin_datalight_handle_single_request(T_RV_HDR *msg_p)
{
  T_GBI_RESULT retval = GBI_OK;
#ifdef DL_DBG_TRACE
	GBI_SEND_TRACE("<DBG>entered gbi_plugin_datalight_handle_single_request\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */
  if (msg_p != NULL)
  {
    switch (msg_p->msg_id)
    {
      case GBI_READ_REQ_MSG:

        retval = gbi_read_req ((T_GBI_READ_REQ_MSG *) msg_p);
		  
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_read_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_WRITE_REQ_MSG:

        retval = gbi_write_req((T_GBI_WRITE_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_write_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_ERASE_REQ_MSG:
      retval = gbi_erase_req((T_GBI_ERASE_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_erase_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

	  case GBI_FLUSH_REQ_MSG:
        retval = gbi_flush_req((T_GBI_FLUSH_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_flush_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

		
      default:
        
        GBI_SEND_TRACE("GBI: gbi_plugin_datalight_single_request: Wrong request message", RV_TRACE_LEVEL_ERROR);
        retval = GBI_INTERNAL_ERR;

        break;
    }
  }
  
  return retval;
}



/**
 * Component:    Plugin Response Handling
 * Sub-function: gbi_plugin_datalight_rsp_msg
 *
 * This function handles all plugin response messages
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK
 */
T_GBI_RESULT gbi_plugin_datalight_rsp_msg(  T_RV_HDR *msg_p)
{
  T_GBI_RESULT  retval = GBI_OK;

  retval = gbi_plugin_datalight_handle_response(msg_p);

  return retval;
  
}


static T_GBI_RESULT gbi_plugin_datalight_handle_response(T_RV_HDR *msg_p)
{
  T_GBI_RESULT  retval = GBI_OK;      
  T_GBI_RETURN 	gbi_status = GBI_OK;
#ifdef DL_DBG_TRACE  
  GBI_SEND_TRACE("<DBG Inside gbi_plugin_datalight_handle_response\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /*DL_DBG_TRACE */
  switch(gbi_datalight_env_ctrl_blk_p->state)
  {
    case GBI_DATALIGHT_NOT_INITIALISED:
      break;

	case   GBI_DATALIGHT_WT_FOR_INIT_RSP:
		init_media_disks();
		gbi_datalight_env_ctrl_blk_p->initialised = TRUE;
		gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;

		#ifdef DL_DBG_TRACE  
  		GBI_SEND_TRACE("<DBG Inside gbi_plugin_datalight_handle_response GBI_DATALIGHT_WT_FOR_INIT_RSP",RV_TRACE_LEVEL_DEBUG_HIGH);
		#endif /*DL_DBG_TRACE */		

		
		break;
		
    case  GBI_DATALIGHT_WT_FOR_READ_RSP:	
      retval = gbi_datalight_wt_for_read_rsp(msg_p);
      break;    
     case  GBI_DATALIGHT_WT_FOR_WRITE_RSP:
      retval = gbi_datalight_wt_for_write_rsp(msg_p);
      break;
    case  GBI_DATALIGHT_WT_FOR_ERASE_RSP:
      retval = gbi_datalight_wt_for_erase_rsp(msg_p);
      break;
	  /* Other state, fall through */
    case GBI_DATALIGHT_INTERNAL_ERROR:
    case GBI_DATALIGHT_IDLE:
    default:
      break;
  }

  /* Plugin should return gbi status */
  if (retval == GBI_OK)
  {
    if (gbi_datalight_env_ctrl_blk_p->state != GBI_DATALIGHT_IDLE)
    {
      gbi_status = GBI_PROCESSING;
    }
    else
    {
      gbi_status = GBI_OK;
    }
  }
  else 
  {
    gbi_status = GBI_INTERNAL_ERR;
  }

  if (gbi_status != GBI_OK)
  {
      GBI_SEND_TRACE_PARAM("gbi_status", gbi_status, RV_TRACE_LEVEL_DEBUG_MEDIUM);
  }
 
  return gbi_status;
}

static T_GBI_RETURN gbi_datalight_wt_for_read_rsp(T_RV_HDR *msg_p)
{
  T_GBI_RETURN         retval;
  T_DL_READ_RSP_MSG    *rsp_p;  

    
#ifdef DL_DBG_TRACE
  GBI_SEND_TRACE("<DBG>Inside gbi_datalight_wt_for_read_rsp\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /*DL_DBG_TRACE */ 
  /**is this the response to wait for ?*/
  if(msg_p->msg_id != DL_READ_RSP_MSG)
  {
    /**no a not supported message*/  
    GBI_SEND_TRACE("gbi_datalight_wt_for_read_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return GBI_NOT_SUPPORTED;
  }

  rsp_p = (T_DL_READ_RSP_MSG *) msg_p;

  if(rsp_p->result != GBI_OK)
  {
    GBI_SEND_TRACE("read result is NOT_OK",RV_TRACE_LEVEL_ERROR);
    /**action failed so send response*/
    gbi_datalight_read_resp(GBI_INTERNAL_ERR, gbi_datalight_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;

    return rsp_p->result;
  }
 
  /**action was succesfull send response*/
  retval = gbi_datalight_read_resp(GBI_OK, gbi_datalight_env_ctrl_blk_p->org_msg_retpath);

  /**set state back to idle*/
  gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;
  
  return retval;
}

static T_GBI_RETURN gbi_datalight_wt_for_write_rsp(T_RV_HDR *msg_p)
{
  T_GBI_RETURN			 retval;
  T_DL_WRITE_RSP_MSG	 *rsp_p;

  

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != DL_WRITE_RSP_MSG)
  {
    /**no a not supported message*/  
    GBI_SEND_TRACE("gbi_datalight_wt_for_write_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return GBI_NOT_SUPPORTED;
  }

  rsp_p = (T_DL_WRITE_RSP_MSG *) msg_p;


  if(rsp_p->result != GBI_OK)
  {
    /**action failed so send response*/
    gbi_datalight_write_resp(GBI_INTERNAL_ERR, gbi_datalight_env_ctrl_blk_p->org_msg_retpath);
    gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;
    return rsp_p->result;
  }
  
  /**action was succesfull send response*/
  retval = gbi_datalight_write_resp(GBI_OK, gbi_datalight_env_ctrl_blk_p->org_msg_retpath);
  /**set state back to idle*/
  gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;

  return retval;
}

/*@}*/
/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION 
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_GBI_RETURN:   RETURNVALUES
 */

/*@{*/
static T_GBI_RETURN gbi_datalight_wt_for_erase_rsp(T_RV_HDR *msg_p)
{
  T_GBI_RETURN retval;
  T_DL_ERASE_RSP_MSG *rsp_p;

   
  /**is this the response to wait for ?*/
  if(msg_p->msg_id != DL_ERASE_RSP_MSG)
  {
    /**no a not supported message*/  
    GBI_SEND_TRACE("gbi_datalight_wt_for_erase_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
    return GBI_NOT_SUPPORTED;
  }
  rsp_p = (T_DL_ERASE_RSP_MSG *) msg_p;

  if(rsp_p->result != GBI_OK)
  {
    /**action failed so send response*/
    retval = gbi_datalight_erase_resp(GBI_INTERNAL_ERR, gbi_datalight_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;
    return retval;
  }
  
  /**action was succesfull send response*/
  retval = gbi_datalight_erase_resp(GBI_OK, gbi_datalight_env_ctrl_blk_p->org_msg_retpath);
  /**set state back to idle*/
  gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;

  return retval;
}



/**
 * function: gbi_datalight_read_resp
 */
static T_GBI_RETURN gbi_datalight_read_resp(T_GBI_RESULT read_result, T_RV_RETURN return_path)
{
  	T_GBI_RETURN             retval = GBI_OK;
  	T_GBI_READ_RSP_MSG   *rsp_p;
  
  	if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_READ_RSP_MSG),
                       GBI_READ_RSP_MSG, 
                       (T_RV_HDR **) &rsp_p) == RVF_RED)	
  	{
  		GBI_SEND_TRACE("gbi_datalight_read_resp :Error in allocating memory for GBI_READ_RSP_MSG",RV_TRACE_LEVEL_ERROR);
		return GBI_MEMORY_ERR;
  	}
  
  	/* compose message */
  	rsp_p->hdr.msg_id     = GBI_READ_RSP_MSG;
 	rsp_p->result      = read_result;

  	/* Send message mailbox */
#ifdef DL_DBG_TRACE  
  	GBI_SEND_TRACE("<DBG>gbi_datalight_read_resp:sending response\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /*DL_DBG_TRACE */
#if WCP_PROF
                prf_LogPointOfInterest("GBI_API: b4 read rsp");
#endif
 	retval = (T_GBI_RETURN)gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);
  	return retval;
}

/**
 * function: gbi_datalight_write_resp
 */

static T_GBI_RETURN gbi_datalight_write_resp(T_GBI_RESULT write_result, T_RV_RETURN return_path)
{
  T_GBI_RETURN             retval = GBI_OK;
  T_GBI_WRITE_RSP_MSG   *rsp_p;
  
  /**
   * Create write response message
   */
  
	if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_WRITE_RSP_MSG),
                       GBI_WRITE_RSP_MSG, 
                       (T_RV_HDR **) &rsp_p) == RVF_RED)  
  {
  		GBI_SEND_TRACE("Gbi_pi_datalight_handle_message :Error in allocating memory for GBI_WRITE_RSP_MSG",RV_TRACE_LEVEL_ERROR);
		return GBI_MEMORY_ERR;
  }
  
  /* compose message */
  rsp_p->hdr.msg_id    = GBI_WRITE_RSP_MSG;
  rsp_p->result         = write_result;

  /* Send message mailbox */
#ifdef DL_DBG_TRACE  
  GBI_SEND_TRACE("<DBG>sending write responce to client\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /*DL_DBG_TRACE */
  retval = (T_GBI_RETURN)gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_datalight_erase_resp
 */
static T_GBI_RETURN gbi_datalight_erase_resp(T_GBI_RESULT erase_result, T_RV_RETURN return_path)
{
  T_GBI_RETURN             retval = GBI_OK;
  T_GBI_ERASE_RSP_MSG  *rsp_p;

  /**
   * Create erase response message
   */
  	if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_ERASE_RSP_MSG),
                       GBI_ERASE_RSP_MSG, 
                       (T_RV_HDR **) &rsp_p) == RVF_RED)  
  {
  		GBI_SEND_TRACE("gbi_datalight_erase_resp :Error in allocating memory for GBI_ERASE_RSP_MSG",RV_TRACE_LEVEL_ERROR);
		return GBI_MEMORY_ERR;
  }
  
  
  /* compose message */
  rsp_p->hdr.msg_id     = GBI_ERASE_RSP_MSG;
  rsp_p->result         = erase_result;

  /* Send message mailbox */
#ifdef DL_DBG_TRACE  
   GBI_SEND_TRACE("<DBG>sending write responce to client\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /*DL_DBG_TRACE */
  retval = (T_GBI_RETURN)gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}


static int datalight_check_valid_params(UINT8         	media_numb, T_GBI_BLOCK		  first_block_nmb,T_GBI_BLOCK		  number_of_blocks)
{
  T_GBI_BLOCK     media_number_of_blocks;
  T_GBI_BYTE_CNT  media_bytes_per_blocks; 
  T_GBI_BLOCK     part_first_block_nmb;    
  UINT16  		  i;	 

 if(media_numb >GBI_DATALIGHT_MAX_NR_OF_PARTITIONS )
 	{
        GBI_SEND_TRACE(" Invalid Media Number",RV_TRACE_LEVEL_ERROR);
		return -1;
	}

  /* find the media and partition Index */
  for(i=0;i<gbi_datalight_env_ctrl_blk_p->nr_partitions;i++)
  	{
  	   if(media_numb == gbi_datalight_env_ctrl_blk_p->partition_tbl[i].media_nmb)
  	   	{
  	   	   break; 
  	   	}
  	}

  if(i == gbi_datalight_env_ctrl_blk_p->nr_partitions)
  	{
     GBI_SEND_TRACE(" Invalid Media Number Number, Media and partition Entry Not found",RV_TRACE_LEVEL_ERROR);
		return -1;
	 
  	}
 
  media_number_of_blocks = gbi_datalight_env_ctrl_blk_p->partition_tbl[i].nmb_of_blocks;
  media_bytes_per_blocks = gbi_datalight_env_ctrl_blk_p->partition_tbl[i].bytes_per_block;
  part_first_block_nmb   = gbi_datalight_env_ctrl_blk_p->partition_tbl[i].first_block_nmb;

  if (first_block_nmb >= media_number_of_blocks)
  {
    /* first_block out of range */
    GBI_SEND_TRACE ("GBI parameter: first_block_nmb out of range",  RV_TRACE_LEVEL_ERROR);

    return -1;
  }

  if (first_block_nmb + number_of_blocks > media_number_of_blocks)
  {
    GBI_SEND_TRACE ("GBI parameter: number_of_blocks out of range", RV_TRACE_LEVEL_ERROR);
  
    return -1;
  }

 return 0;
}



/****************************************************************/
/* Generic Plugin functions                 */
/* The DATALIGHT specific functions can be found in this module, the   */ 
/* PC-sim specific functions, can be found in gbi_pc_i.c        */
/****************************************************************/

/**
 * function: gbi_read_req
 */
static T_GBI_RESULT gbi_read_req(T_GBI_READ_REQ_MSG *msg_p)
{
T_GBI_RETURN      retval = GBI_OK;
T_RV_RETURN ReturnPath, rp;


if (msg_p->buffer_p == NULL)
  {
    /* buffer_p is a null pointer */
    GBI_SEND_TRACE ("GBI parameter: buffer_p not valid", RV_TRACE_LEVEL_WARNING);

    retval = gbi_datalight_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }


 if(datalight_check_valid_params(msg_p->media_nmb,msg_p->first_block_nmb,msg_p->number_of_blocks) != 0)
 	{
       retval = gbi_datalight_read_resp(GBI_INTERNAL_ERR, msg_p->return_path);
	   gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;  
	 
 	}
   
   retval = dl_read_sync (msg_p->media_nmb,msg_p->first_block_nmb,msg_p->number_of_blocks,(UINT8*)msg_p->buffer_p);

  if(retval != GBI_OK)
  {
    GBI_SEND_TRACE("dl_read failed\n\r",RV_TRACE_LEVEL_WARNING);
    return gbi_datalight_read_resp(GBI_INTERNAL_ERR, msg_p->return_path);
  }
  return gbi_datalight_read_resp(GBI_OK, msg_p->return_path);  
}



/**
 * function: gbi_write_req
 */
static T_GBI_RESULT gbi_write_req(T_GBI_WRITE_REQ_MSG *msg_p)
{
	T_GBI_RETURN	retval = GBI_OK;
  	T_RV_RETURN ReturnPath;

if (msg_p->buffer_p == NULL)
  {
    /* buffer_p is a null pointer */
    GBI_SEND_TRACE ("GBI parameter: buffer_p not valid", RV_TRACE_LEVEL_WARNING);

    retval = gbi_datalight_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

if(datalight_check_valid_params(msg_p->media_nmb,msg_p->first_block_nmb,msg_p->number_of_blocks) != 0)
 	{
       retval = gbi_datalight_write_resp(GBI_INTERNAL_ERR, msg_p->return_path);
	   gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;  
	 
 	}
	  
  retval = dl_write_sync (msg_p->media_nmb,msg_p->first_block_nmb,msg_p->number_of_blocks,(UINT8*)msg_p->buffer_p);

  if(retval != GBI_OK)
  {
	GBI_SEND_TRACE("dl_write failed\n\r",RV_TRACE_LEVEL_WARNING);
    return gbi_datalight_write_resp(GBI_INTERNAL_ERR, msg_p->return_path);
  }
  return gbi_datalight_write_resp(GBI_OK, msg_p->return_path);  
}




/**
 * function: gbi_erase_req
 */
static T_GBI_RESULT gbi_erase_req(T_GBI_ERASE_REQ_MSG *msg_p)
{
  	T_GBI_RETURN      	retval = GBI_OK;

if(datalight_check_valid_params(msg_p->media_nmb,msg_p->first_block_nmb,msg_p->number_of_blocks) != 0)
 	{
       retval = gbi_datalight_erase_resp(GBI_INTERNAL_ERR, msg_p->return_path);
	   gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;  
	 
 	}

 retval = dl_erase_sync(msg_p->media_nmb,msg_p->first_block_nmb,msg_p->number_of_blocks);             

  if(retval != GBI_OK)
  {
    GBI_SEND_TRACE("dl_erase failed\n\r",RV_TRACE_LEVEL_WARNING);
    return gbi_datalight_erase_resp(GBI_INTERNAL_ERR, msg_p->return_path);
  }
  return gbi_datalight_erase_resp(GBI_OK, msg_p->return_path);  
}


/**
 * function: gbi_flush_req
 */
static T_GBI_RESULT gbi_flush_req(T_GBI_FLUSH_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  UINT8           partition_index;

  /* Retrieve necessary media and partition data */  

  GBI_SEND_TRACE("For FFx there is nothing to flush\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);

  /*for the DATALIGHT-plugin there is nothing to flush, return OK*/
  retval = gbi_flush_resp(GBI_OK,  msg_p->return_path);
  return retval;
}




/****************************************************************/
/*   DATALIGHT Plugin functions: datalight functions        */
/****************************************************************/

static T_GBI_RESULT gbi_datalight_init_req(unsigned short disk_num)
{
  T_GBI_RETURN      retval = GBI_OK;
  T_RV_RETURN ReturnPath;
	
  ReturnPath.addr_id = gbi_env_ctrl_blk_p->addr_id;
  ReturnPath.callback_func = NULL;

   retval = dl_init(ReturnPath);

 	if(retval != GBI_OK)
  	{
	    gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE; 
 	}
 	else
 	{
		//wait for last read respose
 	   	gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_WT_FOR_INIT_RSP;
 	}
  	return retval;
  
	
}



/**
 * function: gbi_get_media_info_req
 */
static T_GBI_RESULT gbi_get_media_info_req(T_GBI_MEDIA_INFO_REQ_MSG *msg_p, 
                                           void **data_p, 
                                           void **nmb_items_p)
{
  T_GBI_RESULT        retval = GBI_OK;
  UINT8				  i;
  
  if(gbi_datalight_env_ctrl_blk_p->initialised)
  {
   *nmb_items_p  = &gbi_datalight_env_ctrl_blk_p->nr_medias;
   for(i=0;i< gbi_datalight_env_ctrl_blk_p->nr_medias;i++)
   	{
       data_p[i]       = &(gbi_datalight_env_ctrl_blk_p->media_tbl[i]);
   	}  
  }
  else
  {
    retval = GBI_NOT_READY;
  }
  return retval;
}

/**
 * function: gbi_get_partition_info_req
 */
static T_GBI_RESULT gbi_get_partition_info_req(T_GBI_PARTITION_INFO_REQ_MSG *msg_p,
                                           void **data_p, 
                                           void **nmb_items_p)
{
  T_GBI_RESULT        retval = GBI_OK;
  UINT8              i;
  
  /* Let given pointers point to according data */
  if(gbi_datalight_env_ctrl_blk_p->initialised)
  {
     *nmb_items_p  = &gbi_datalight_env_ctrl_blk_p->nr_partitions;

   for(i=0;i<gbi_datalight_env_ctrl_blk_p->nr_partitions;i++)
   	{
       data_p[i]       = &(gbi_datalight_env_ctrl_blk_p->partition_tbl[i]);
   	}  
  }
  else
  {
    retval = GBI_NOT_READY;
  }
  return retval;
}




/**
 * function: gbi_flush_resp
 */
static T_GBI_RESULT gbi_flush_resp(T_GBI_RESULT flush_result,T_RV_RETURN  return_path)
{
  T_GBI_RESULT          retval = GBI_OK;
  T_GBI_FLUSH_RSP_MSG   *rsp_p;

  /**
   * Create read block info response message
   */
	if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_FLUSH_RSP_MSG),
                       GBI_FLUSH_RSP_MSG, 
                       (T_RV_HDR **) &rsp_p) == RVF_RED)   
  {
  		GBI_SEND_TRACE("Gbi_pi_datalight_handle_message :Error in allocating memory for GBI_READ_SPARE_DATA_RSP_MSG",RV_TRACE_LEVEL_WARNING);
		return GBI_MEMORY_ERR;
  }

	/* compose message */
  rsp_p->hdr.msg_id    = GBI_FLUSH_RSP_MSG;
  rsp_p->result         = flush_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}


