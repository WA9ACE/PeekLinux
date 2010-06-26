/**
 * @file	gbi_pi_mc_env.c
 *
 * Plugin MMC/SD: Resources and Dependency functions.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	03/16/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */
 

#include "gbi/gbi_i.h"
#include "gbi/gbi_pi_cfg.h"
#include "gbi/gbi_pi_mc_env.h"
#include "gbi/gbi_pi_mc_handle_message.h"
#include "gbi/gbi_pi_mc_i.h"

T_GBI_RESULT	gbi_plugin_mc_get_info(T_GBI_PLUGIN_GET_INFO *plugin_getinfo);
T_GBI_RESULT	gbi_plugin_mc_start(void);
T_GBI_RESULT	gbi_plugin_mc_stop(void);


const T_GBI_PLUGIN_FNCTBL gbi_plugin_fnctbl_mc = {
	gbi_plugin_mc_reg_msg,
	gbi_plugin_mc_reg_multiple_msg,		
	gbi_plugin_mc_rsp_msg,
	gbi_plugin_mc_get_info,
	gbi_plugin_mc_start,
	gbi_plugin_mc_stop
};

/**environment controlblock needed for mmc/sd plugin*/
T_GBI_MC_ENV_CTRL_BLK *gbi_mc_env_ctrl_blk_p;

/**
 * Component:    Plugin Resources and Dependency, start and stop
 * Sub-function: gbi_plugin_mc_get_info
 *
 * This function provides Riviera with info about the resources the GBI SWE needs
 * This resource change due to the number and properties of the plugins included. 
 * There for each plugin must provide a function that provides this information.
 *
 * @param		plugin_getinfo		Pointer to plugin get information
 * @return	GBI_OK
 */
T_GBI_RESULT	gbi_plugin_mc_get_info(T_GBI_PLUGIN_GET_INFO *plugin_getinfo)
{
	GBI_SEND_TRACE("gbi_plugin_mc_get_info", RV_TRACE_LEVEL_DEBUG_MEDIUM);

#ifdef _WINDOWS
	// WHen building for PC no MC-plugin 
	plugin_getinfo->linked_swe_id[0] 		= MC_USE_ID;
	plugin_getinfo->nb_linked_swe 			= 0;
#else
	plugin_getinfo->linked_swe_id[0]    = MC_USE_ID;
	plugin_getinfo->nb_linked_swe 			= 1;
#endif

	return GBI_OK;
}

/**
 * Component:    Plugin Resources and Dependency, start and stop
 * Sub-function: gbi_plugin_mc_start
 *
 * This function provides the plugin the opportunity for pre-initialization.
 *
 * @param		None
 * @return	GBI_OK
 */
T_GBI_RESULT	gbi_plugin_mc_start(void)
{
  T_GBI_RESULT 			retval = GBI_OK;
	T_RVF_MB_STATUS 	mb_status;
	UINT8							i;

	GBI_SEND_TRACE("gbi_plugin_mc_start", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  /** Allocate mc plugin buffer */
	/* Create instance gathering all the variable used by GBI instance */
	mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                           sizeof(T_GBI_MC_ENV_CTRL_BLK),
							             (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p);
	
	if (mb_status == RVF_RED)
	{
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the mmc plugin buffer.
		 */
		GBI_SEND_TRACE("GBI: Error to get memory ",RV_TRACE_LEVEL_ERROR);
				
		return GBI_MEMORY_ERR;
	}

  /** Initialise the mmc plugin global variables*/
	gbi_mc_env_ctrl_blk_p->state = GBI_MMC_NOT_INITIALISED;
  gbi_mc_env_ctrl_blk_p->plugin_status = FALSE;

  gbi_mc_env_ctrl_blk_p->org_msg_retpath.callback_func = NULL;
  gbi_mc_env_ctrl_blk_p->card_stack_size = 0;
  gbi_mc_env_ctrl_blk_p->card_stack_p = NULL;
	
  gbi_mc_env_ctrl_blk_p->store_data_p      = NULL;
  gbi_mc_env_ctrl_blk_p->store_nmb_items_p = NULL;


  gbi_mc_env_ctrl_blk_p->requested_cid_p        = NULL;
  gbi_mc_env_ctrl_blk_p->requested_csd_p        = NULL;

	for (i = 0; i < GBI_MAX_NR_OF_MEDIA; i++)
	{
	  gbi_mc_env_ctrl_blk_p->requested_media_info_p[i].media_nmb = GBI_MC_BM_MEDIA_NR_0;
	  gbi_mc_env_ctrl_blk_p->requested_media_info_p[i].media_pressent = TRUE;
	  gbi_mc_env_ctrl_blk_p->requested_media_info_p[i].media_id = 0;
	  gbi_mc_env_ctrl_blk_p->requested_media_info_p[i].media_type = GBI_MMC;
	}

 	for (i = 0; i < GBI_MAX_NR_OF_PARTITIONS; i++)
  {
	  gbi_mc_env_ctrl_blk_p->requested_partition_info_p[i].media_nmb = GBI_MC_BM_MEDIA_NR_0;
	  gbi_mc_env_ctrl_blk_p->requested_partition_info_p[i].partition_nmb = 0;
	  gbi_mc_env_ctrl_blk_p->requested_partition_info_p[i].nmb_of_blocks = 0;
	  gbi_mc_env_ctrl_blk_p->requested_partition_info_p[i].bytes_per_block = 0;
	  gbi_mc_env_ctrl_blk_p->requested_partition_info_p[i].first_block_nmb = 0;
	  gbi_mc_env_ctrl_blk_p->requested_partition_info_p[i].last_block_nmb =0;
	}		

 
  return retval;
}

/**
 * Component:    Plugin Resources and Dependency, start and stop
 * Sub-function: gbi_plugin_mc_start
 *
 * This function provides the plugin the opportunity for clean-up
 *
 * @param		None
 * @return	GBI_OK
 */
T_GBI_RESULT	gbi_plugin_mc_stop(void)
{
  T_RVF_RET    retval = RV_OK;

	GBI_SEND_TRACE("gbi_plugin_mc_stop", RV_TRACE_LEVEL_DEBUG_MEDIUM);
		
  if (gbi_mc_env_ctrl_blk_p->card_stack_p != NULL)
	{
		retval = rvf_free_buf(gbi_mc_env_ctrl_blk_p->card_stack_p);	
		if (retval != RV_OK)
		{
			return (T_GBI_RESULT)retval;
		}
		
		gbi_mc_env_ctrl_blk_p->card_stack_p = NULL;
	}

  /** Free buffers and make pointers NULL*/
  if (gbi_mc_env_ctrl_blk_p->requested_csd_p != NULL) 
  {
		retval = rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);	
		if (retval != RV_OK)
		{
			return (T_GBI_RESULT)retval;
		}
    
    gbi_mc_env_ctrl_blk_p->requested_csd_p = NULL;
  }

  if (gbi_mc_env_ctrl_blk_p->requested_cid_p != NULL)
  {
		retval = rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_cid_p);	
		if (retval != RV_OK)
		{
			return (T_GBI_RESULT)retval;
		}
    
    gbi_mc_env_ctrl_blk_p->requested_cid_p = NULL;
  }

 
  if (gbi_mc_env_ctrl_blk_p->mmc_data != NULL)  
  {
		retval = rvf_free_buf(gbi_mc_env_ctrl_blk_p->mmc_data);	
		if (retval != RV_OK)
		{
			return (T_GBI_RESULT)retval;
		}

    gbi_mc_env_ctrl_blk_p->mmc_data = NULL;
  }
	
	retval = rvf_free_buf(gbi_mc_env_ctrl_blk_p);	
	return (T_GBI_RESULT)retval;
}


