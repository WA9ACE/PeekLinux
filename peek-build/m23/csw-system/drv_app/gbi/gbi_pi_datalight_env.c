/**
 * @file	gbi_pi_datalight_env.c
 *
 * Plugin DATALIGHT: Resources and Dependency functions.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *		 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */



#include "gbi_i.h"
#include "gbi_cfg.h"
#include "gbi_pi_cfg.h"
#include "gbi_pi_datalight_handle_message.h"
#include "gbi_pi_datalight_i.h"
#include "gbi_api.h"
#include "rvm/rvm_use_id_list.h"
#include "string.h"
#include "datalight/datalight_api.h"
#include "swconfig.cfg"

#ifndef DATALIGHT_SIMULATION

/* This macro defination is manditary , otherwise build fails */
#define FFXDRV_INC_LOCOSTO_DATALIGHT_PLUGIN
#include "ffxdrv.h"
#endif

#ifndef DATALIGHT_SIMULATION

/* Structure to get the disk parameters using ioctl */
extern DISK_MEDIA_PARAMS 	sDiskMediaParams[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];
extern DISK_PARTITION_PARAMS 	sDiskPartitionParams[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];
#endif



T_GBI_RESULT	gbi_plugin_datalight_get_info(T_GBI_PLUGIN_GET_INFO *plugin_getinfo);
T_GBI_RESULT	gbi_plugin_datalight_start(void);
T_GBI_RESULT	gbi_plugin_datalight_stop(void);





const T_GBI_PLUGIN_FNCTBL gbi_plugin_fnctbl_datalight = {
	gbi_plugin_datalight_reg_msg,
	gbi_plugin_datalight_reg_multiple_msg,
	gbi_plugin_datalight_rsp_msg,
	gbi_plugin_datalight_get_info,
	gbi_plugin_datalight_start,
	gbi_plugin_datalight_stop
};


/**environment controlblock needed for datalight plugin*/
T_GBI_DATALIGHT_ENV_CTRL_BLK *gbi_datalight_env_ctrl_blk_p=NULL;



/**
 * Component:    Plugin Resources and Dependency, start and stop
 * Sub-function: gbi_plugin_datalight_get_info
 *
 * This function provides application with info about the resources the GBI SWE needs
 * This resource change due to the number and properties of the plugins included.
 * There for each plugin must provide a function that provides this information.
 *
 * @param		plugin_getinfo		Pointer to plugin get information
 * @return		GBI_OK
 */
T_GBI_RESULT	gbi_plugin_datalight_get_info(T_GBI_PLUGIN_GET_INFO *plugin_getinfo)
{
	GBI_SEND_TRACE("gbi_plugin_datalight_get_info",RV_TRACE_LEVEL_DEBUG_HIGH);

	plugin_getinfo->linked_swe_id[0]        = DATALIGHT_USE_ID;
	plugin_getinfo->nb_linked_swe 			= 1;

	return GBI_OK;
}



/**
 * Component:    Plugin Resources and Dependency, start and stop
 * Sub-function: gbi_plugin_datalight_stop
 *
 * This function provides the plugin the opportunity for clean-up
 *
 * @param		None
 * @return		GBI_OK
 */
T_GBI_RESULT	gbi_plugin_datalight_stop(void)
{
	T_GBI_RESULT retval = GBI_OK;

	GBI_SEND_TRACE("gbi_plugin_datalight_stop",RV_TRACE_LEVEL_DEBUG_HIGH);

	/* BTI: TO DO check if pointers are not NULL */

    if(gbi_datalight_env_ctrl_blk_p != NULL)
    	{
			retval = (T_GBI_RESULT) rvf_free_buf(gbi_datalight_env_ctrl_blk_p);
			gbi_datalight_env_ctrl_blk_p=NULL;
    	}
	return retval;
}



/* ************************************************************************
     EXTENDABLE FUNCTIONS
   ************************************************************************
*/


T_GBI_RESULT  init_media_disks()
{

  UINT16 i=0;
  extern FlashFx_Disks_Init();

  
  /* TODO , Disk Initializations */
     FlashFx_Disks_Init();


/* start filling the partition table */
i=0;

  gbi_datalight_env_ctrl_blk_p->nr_partitions = 0;
  gbi_datalight_env_ctrl_blk_p->nr_medias = 0;

#if (DATALIGHT_NOR == 1)

 /* Media info */


    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_nmb 		= GBI_DATALIGHT_NOR_PART_ID_0;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_id  		= GBI_DATALIGHT_MEDIA_0_MEDIA_TYPE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_pressent 	= TRUE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_type 		= GBI_DATALIGHT_MEDIA_0_MEDIA_TYPE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].read_speed 		= GBI_DATALIGHT_MEDIA_0_READ_SPEED;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].writeable 		= TRUE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].write_speed 		= GBI_DATALIGHT_MEDIA_0_WRITE_SPEED;





  /* partition info */
   gbi_datalight_env_ctrl_blk_p->partition_tbl[i].media_nmb = gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_nmb;
	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_nmb = 0;

   gbi_datalight_env_ctrl_blk_p->partition_tbl[i].bytes_per_block  = sDiskMediaParams[GBI_DATALIGHT_NOR_PART_ID_0].ulSectorSize;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].nmb_of_blocks    = sDiskPartitionParams[GBI_DATALIGHT_NOR_PART_ID_0].ulSectorCount;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].first_block_nmb  = sDiskPartitionParams[GBI_DATALIGHT_NOR_PART_ID_0].ulStartSector;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].last_block_nmb   = (sDiskPartitionParams[GBI_DATALIGHT_NOR_PART_ID_0].ulStartSector +sDiskPartitionParams[GBI_DATALIGHT_NOR_PART_ID_0].ulSectorCount - 1);




	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].filesystem_type = GBI_DATALIGHT_MEDIA_0_PAR_0_FILESYSTEM;
	strncpy((char*)gbi_datalight_env_ctrl_blk_p->partition_tbl[i].filesystem_name, GBI_DATALIGHT_MEDIA_0_FS_NAME,GBI_MAX_FS_NAME-1);
	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].filesystem_name[GBI_MAX_FS_NAME-1]='\0';

	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_media_names =GBI_MN_INTERNAL_NOR_A0;
	strncpy((char*)gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_name, GBI_DATALIGHT_MEDIA_0_PAR_0_NAME,GBI_MAX_PARTITION_NAME-1);
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_name[GBI_MAX_PARTITION_NAME-1]='\0';


/* Increment i */
  i++;
  gbi_datalight_env_ctrl_blk_p->nr_medias++ ;
  gbi_datalight_env_ctrl_blk_p->nr_partitions++;

#endif

#if (DATALIGHT_NAND == 1)


    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_nmb 		= GBI_DATALIGHT_NAND_PART_ID_0;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_id  		= GBI_DATALIGHT_MEDIA_1_MEDIA_TYPE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_pressent 	= TRUE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_type 		= GBI_DATALIGHT_MEDIA_1_MEDIA_TYPE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].read_speed 		= GBI_DATALIGHT_MEDIA_1_READ_SPEED;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].writeable 		= TRUE;
    gbi_datalight_env_ctrl_blk_p->media_tbl[i].write_speed 		= GBI_DATALIGHT_MEDIA_1_WRITE_SPEED;






    /* partition info */
   gbi_datalight_env_ctrl_blk_p->partition_tbl[i].media_nmb = gbi_datalight_env_ctrl_blk_p->media_tbl[i].media_nmb;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_nmb = 0;

    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].bytes_per_block  = sDiskMediaParams[GBI_DATALIGHT_NAND_PART_ID_0].ulSectorSize;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].nmb_of_blocks    = sDiskPartitionParams[GBI_DATALIGHT_NAND_PART_ID_0].ulSectorCount;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].first_block_nmb  = sDiskPartitionParams[GBI_DATALIGHT_NAND_PART_ID_0].ulStartSector;
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].last_block_nmb   = (sDiskPartitionParams[GBI_DATALIGHT_NAND_PART_ID_0].ulStartSector +sDiskPartitionParams[GBI_DATALIGHT_NAND_PART_ID_0].ulSectorCount - 1);


	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].filesystem_type = GBI_DATALIGHT_MEDIA_1_PAR_0_FILESYSTEM;
	strncpy((char*)gbi_datalight_env_ctrl_blk_p->partition_tbl[i].filesystem_name, GBI_DATALIGHT_MEDIA_1_FS_NAME,GBI_MAX_FS_NAME-1);
	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].filesystem_name[GBI_MAX_FS_NAME-1]='\0';

	gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_media_names =GBI_MN_INTERNAL_NAND_A0;
	strncpy((char*)gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_name, GBI_DATALIGHT_MEDIA_1_PAR_0_NAME,GBI_MAX_PARTITION_NAME-1);
    gbi_datalight_env_ctrl_blk_p->partition_tbl[i].partition_name[GBI_MAX_PARTITION_NAME-1]='\0';


/* Increment i */
  i++;
  gbi_datalight_env_ctrl_blk_p->nr_medias++ ;
  gbi_datalight_env_ctrl_blk_p->nr_partitions++;


#endif



  /* TODO: You can have some more media and partions here */

  return GBI_OK;
}



/**
 * Component:    Plugin Resources and Dependency, start and stop
 * Sub-function: gbi_plugin_datalight_start
 *
 * This function provides the plugin the opportunity for pre-initialization.
 *
 * @param		None
 * @return		GBI_OK
 */
T_GBI_RESULT	gbi_plugin_datalight_start(void)
{
	T_RVF_MB_STATUS 		mb_status;


	GBI_SEND_TRACE("gbi_plugin_datalight_start",RV_TRACE_LEVEL_DEBUG_HIGH);

  if(gbi_datalight_env_ctrl_blk_p != NULL)
  	{
  	 	GBI_SEND_TRACE("gbi_datalight_env_ctrl_blk_p Memory exist ",RV_TRACE_LEVEL_DEBUG_LOW);
		return GBI_OK;
  	}

  /** Allocate datalight plugin buffer */
	/* Create instance gathering all the variable used by GBI instance */
	mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                           sizeof(T_GBI_DATALIGHT_ENV_CTRL_BLK),
							         (T_RVF_BUFFER**)&gbi_datalight_env_ctrl_blk_p);

  	if (mb_status == RVF_RED)
	{
   		GBI_SEND_TRACE("Memory allocation error for:var_p->gbi_datalight_env_ctrl_blk_p",RV_TRACE_LEVEL_ERROR);
		return GBI_MEMORY_ERR;
  	}

	memset(gbi_datalight_env_ctrl_blk_p,0,sizeof(T_GBI_DATALIGHT_ENV_CTRL_BLK));

	gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_NOT_INITIALISED;


  	/** Initialise the datalight plugin global variables*/
	gbi_datalight_env_ctrl_blk_p->initialised = FALSE;


    init_media_disks();

   gbi_datalight_env_ctrl_blk_p->state = GBI_DATALIGHT_IDLE;


 	return GBI_OK;
}



