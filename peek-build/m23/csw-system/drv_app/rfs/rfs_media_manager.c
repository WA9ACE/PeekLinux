/**
 * @file	rfs_media_manager.c
 *
 * Contans the 'Media manager' component. *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/27/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */
#include <string.h>

#include "rfs/rfs_i.h"
#include "gbi/gbi_pi_cfg.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_api.h"
#include "rfs/fscore_types.h"
#include "chipset.cfg"
#include "swconfig.cfg"

#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))
#include "ffs/reliance/relfs.h"
#endif

#define FREE_MSG(p)               {if(p) {rvf_free_msg((T_RVF_MSG *)p);p=NULL;}}

#define RFS_GBI_ITERATIONS			0



T_RFS_FSCORE_LIST    fs_core_list[RFS_NMB_OF_FSCORE_ITEMS] =  {RFS_FSCORES};

/* Local Media Manager functions */
static T_RFS_RETURN rfs_med_mgr_not_initialised_func(T_RV_HDR *msg_p);
static T_RFS_RETURN rfs_med_mgr_wt_for_event_ind_func(T_RV_HDR *msg_p);
static T_RFS_RETURN rfs_med_mgr_wt_for_media_info_rsp_func(T_RV_HDR *msg_p);
static T_RFS_RETURN rfs_med_mgr_wt_for_partition_info_rsp_func(T_RV_HDR *msg_p);
static T_RFS_RETURN rfs_med_mgr_wt_for_fscore_partition_upd_func(T_RV_HDR *msg_p);
static T_RFS_RETURN rfs_med_mgr_media_removed(void);
static void find_media_type(UINT16 media_num,T_FSCORE_PARTITION_INFO *fscore_mpt_table);
T_RV_HDR * rfs_wait_for_message (UINT32 msg_id);


/**
 * Main function of the media manager
 *
 * Detailled description.
 * This function handles all media manager concerned messages. This
 * message can be the internal startup message, GBI messages or FS-core
 * messages concerning the update of thier partition tables. This handling
 * is state-machine controlled.
 *
 * @param   msg_p   Message pointer to message to be handled by the media manager
 *
 * @return  RFS_OK
 */
T_RFS_RETURN rfs_media_manager_process_msg(T_RV_HDR *msg_p)
{
  T_RFS_RETURN retval = RFS_OK;

  RFS_SEND_TRACE("RFS Media manager: rfs_media_manager_process_msg", RV_TRACE_LEVEL_DEBUG_MEDIUM);


 switch (msg_p->msg_id)
 	{
 	case RFS_I_STARTUP_MSG:
		  retval = rfs_med_mgr_not_initialised_func(msg_p);
         break;

    case GBI_EVENT_IND_MSG:
          retval = rfs_med_mgr_wt_for_event_ind_func(msg_p);
		 break;

    case GBI_MEDIA_INFO_RSP_MSG:
		  retval = rfs_med_mgr_wt_for_media_info_rsp_func(msg_p);
		 break;

    case GBI_PARTITION_INFO_RSP_MSG:
          retval = rfs_med_mgr_wt_for_partition_info_rsp_func(msg_p);
		 break;

    case RFSFAT_SET_PARTITION_TABLE_RSP_MSG:
		   retval = rfs_med_mgr_wt_for_fscore_partition_upd_func(msg_p);
		break;

	default:
     /* The media manager is in an unknown state */
      RFS_SEND_TRACE("RFS Media manager: Unknown Message", RV_TRACE_LEVEL_ERROR);
      retval = RFS_INTERNAL_ERR;
      break;

 	}


return retval;

}

/**
 * sub-function of the media manager: rfs_med_mgr_not_initialised_func
 *
 * Detailled description.
 * Handling of the startup message
 *
 * @param   msg_p   Message pointer to message to be handled
 *
 * @return  RFS_OK
 */
static T_RFS_RETURN rfs_med_mgr_not_initialised_func(T_RV_HDR *msg_p)
{
  T_GBI_EVENTS event;
  T_RV_RETURN  rfs_ret_path;
  T_RVF_MB_ID         mb_id;
  T_RV_RET      status=RV_OK;
  UINT8 i=0;
  T_FSCORE_FUNC_TABLE  func_table;

  RFS_SEND_TRACE("RFS Media manager: rfs_med_mgr_not_initialised_func", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (msg_p->msg_id != RFS_I_STARTUP_MSG)
  {
    /* wrong message received in this state */
    RFS_SEND_TRACE("RFS Media manager: Unexpected message received", RV_TRACE_LEVEL_ERROR);
    return RFS_INTERNAL_ERR;
  }

  mb_id                      = rfs_env_ctrl_blk_p->prim_mb_id;
  rfs_ret_path.addr_id       = rfs_env_ctrl_blk_p->addr_id;
  rfs_ret_path.callback_func = NULL;



#if ((RELIANCE_FS == 1) || (RELIANCE_FS == 2))

  memset(&func_table,0, sizeof(T_FSCORE_FUNC_TABLE));

  /* Initialize the Reliance */
  relfs_get_func_table(&func_table);

  if(func_table.fscore_set_partition_table != NULL)
  	{

      rfs_env_ctrl_blk_p->fscore_part_table[RFS_RELIANCE_NUM].fscore_numb= RFS_RELIANCE_NUM;
	  rfs_env_ctrl_blk_p->fscore_part_table[RFS_RELIANCE_NUM].nbr_partitions = 0;
	  rfs_env_ctrl_blk_p->fscore_part_table[RFS_RELIANCE_NUM].partition_info_table = NULL;

	  func_table.fscore_set_partition_table((&rfs_env_ctrl_blk_p->fscore_part_table[RFS_RELIANCE_NUM]), rfs_ret_path);

  	}


#endif






  /* RFS subscribes to both insertion and removal */
  event = (GBI_EVENT_MEDIA_INSERT | GBI_EVENT_MEDIA_REMOVEAL|GBI_EVENT_NAN_MEDIA_AVAILABLE);

#if (RFS_GBI_ITERATIONS > 0)

 status=RV_NOT_READY;

  for(i=0;(i<RFS_GBI_ITERATIONS) && (status != RV_OK);i++)
   {
      if ((status = gbi_subscribe_events(event, rfs_ret_path)) != RV_OK)
     {
        RFS_SEND_TRACE("RFS Media manager: API Call gbi_subscribe_events() failed error value ignored ", RV_TRACE_LEVEL_ERROR);
        rvf_delay(RVF_MS_TO_TICKS(2000));
      }
   }

  if(status != RV_OK)
  	{
         RFS_SEND_TRACE("RFS Media manager: API Call gbi_subscribe_events() failed ", RV_TRACE_LEVEL_ERROR);
		 return RFS_INTERNAL_ERR;
  	}



#else
  if ((status = gbi_subscribe_events(event, rfs_ret_path)) != RV_OK)
  {
    RFS_SEND_TRACE("RFS Media manager: API Call gbi_subscribe_events() result not OK ", RV_TRACE_LEVEL_ERROR);
     /* return RFS_INTERNAL_ERR;  */  /* dont return error*/

	/* Initialise media information */
    if (gbi_get_media_info(mb_id, rfs_ret_path) != RV_OK)
    {
      RFS_SEND_TRACE("RFS Media manager: API Call gbi_get_media_info() failed", RV_TRACE_LEVEL_ERROR);
      return RFS_INTERNAL_ERR;
    }

  }
#endif


  return RFS_OK;
}

/**
 * sub-function of the media manager: rfs_med_mgr_wt_for_event_ind_func
 *
 * Detailled description.
 * Handling of subscription to media change events in the GBI entity and Media
 * Change notification (insertion/removal). When a subscription is made to the GBI
 * events, the Media Change notification is also generated by the GBI.
 *
 * In response of the Media Change notification: in response RFS must request media
 * information from the GBI entity.
 *
 * @param   msg_p   Message pointer to message to be handled
 *
 * @return  RFS_OK
 */
static T_RFS_RETURN rfs_med_mgr_wt_for_event_ind_func(T_RV_HDR *msg_p)
{
  T_RFS_RETURN        retval = RFS_OK;
  T_GBI_EVENT_MSG     *event_msg_p;
  T_RVF_MB_ID         mb_id;
  T_RV_RETURN         rfs_ret_path;

  RFS_SEND_TRACE("RFS Media manager: rfs_med_mgr_wt_for_event_ind_func", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (msg_p->msg_id != GBI_EVENT_IND_MSG)
  {
    /* wrong message received in this state */
    RFS_SEND_TRACE("RFS Media manager: Unexpected message received", RV_TRACE_LEVEL_ERROR);
    return RFS_INTERNAL_ERR;
  }

  /* Cast response message */
  event_msg_p = (T_GBI_EVENT_MSG *)msg_p;

  RFS_SEND_TRACE_PARAM("event_msg_p->event", event_msg_p->event, RV_TRACE_LEVEL_DEBUG_LOW);

  if ((event_msg_p->event == GBI_EVENT_MEDIA_INSERT) || (event_msg_p->event == GBI_EVENT_NAN_MEDIA_AVAILABLE))
  {
    /* Clear, perviously allocated media info memory on memory bank */
    if (rfs_env_ctrl_blk_p->media_info_p != NULL)
    {
      rvf_free_buf(rfs_env_ctrl_blk_p->media_info_p);
      rfs_env_ctrl_blk_p->media_info_p = NULL;
      rfs_env_ctrl_blk_p->nmb_of_media = 0;
    }

    mb_id                      = rfs_env_ctrl_blk_p->prim_mb_id;
    rfs_ret_path.addr_id       = rfs_env_ctrl_blk_p->addr_id;
    rfs_ret_path.callback_func = NULL;

    /* Initialise media information */
    if (gbi_get_media_info(mb_id, rfs_ret_path) != RV_OK)
    {
      RFS_SEND_TRACE("RFS Media manager: API Call gbi_get_media_info() failed", RV_TRACE_LEVEL_ERROR);
      return RFS_INTERNAL_ERR;
    }

  }
  else if (event_msg_p->event == GBI_EVENT_MEDIA_REMOVEAL)
  {
    retval = rfs_med_mgr_media_removed();

  }
  else
  {
    RFS_SEND_TRACE("RFS Media manager: Received event can not be handled", RV_TRACE_LEVEL_ERROR);
    return RFS_INTERNAL_ERR;
  }

  return retval;
}


/**
 * sub-function of the media manager: rfs_med_mgr_wt_for_media_info_rsp_func
 *
 * Detailled description.
 * Handling of the received GBI media information and in response request the
 * partition information.
 *
 * @param   msg_p   Message pointer to message to be handled
 *
 * @return  RFS_OK
 */
static T_RFS_RETURN rfs_med_mgr_wt_for_media_info_rsp_func(T_RV_HDR *msg_p)
{
  T_RFS_RETURN              retval = RFS_OK;
  T_GBI_MEDIA_INFO_RSP_MSG  *media_msg_p;
  T_RVF_MB_ID               mb_id;
  T_RV_RETURN               rfs_ret_path;

  RFS_SEND_TRACE("RFS Media manager: rfs_med_mgr_wt_for_media_info_rsp_func", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (msg_p->msg_id != GBI_MEDIA_INFO_RSP_MSG)
  {
    /* wrong message received in this state */
    RFS_SEND_TRACE("RFS Media manager: Unexpected message received", RV_TRACE_LEVEL_ERROR);
    return RFS_INTERNAL_ERR;
  }

  /* Cast response message */
  media_msg_p = (T_GBI_MEDIA_INFO_RSP_MSG *)msg_p;

  /* This pointer points to memory of the RFS memory bank, save this
   * pointer in the RFS control block
   */

  rfs_env_ctrl_blk_p->media_info_p = media_msg_p->info_p;
  rfs_env_ctrl_blk_p->nmb_of_media = media_msg_p->nmb_of_media;

  if(rfs_env_ctrl_blk_p->nmb_of_media == 0)
  	{
  	   RFS_SEND_TRACE("RFS Media manager: Zero Number of medias", RV_TRACE_LEVEL_ERROR);
	   return retval;    /* dont send error, say success, dont change the state of the media manager */
  	}

  if(rfs_env_ctrl_blk_p->media_info_p == NULL)
  	{
  	   RFS_SEND_TRACE("RFS Media manager: NULL media info", RV_TRACE_LEVEL_ERROR);
	   return retval;    /* dont send error, say success, dont change the state of the media manager */
  	}


  mb_id                      = rfs_env_ctrl_blk_p->prim_mb_id;
  rfs_ret_path.addr_id       = rfs_env_ctrl_blk_p->addr_id;
  rfs_ret_path.callback_func = NULL;

  /* Initialise partition information */
  if (gbi_get_partition_info(mb_id, rfs_ret_path) != RV_OK)
  {
    RFS_SEND_TRACE("RFS Media manager: API Call gbi_get_partition_info() failed", RV_TRACE_LEVEL_ERROR);
    return RFS_INTERNAL_ERR;
  }


  return retval;
}



static void find_media_type(UINT16 media_num,T_FSCORE_PARTITION_INFO *fscore_mpt_table)
{
 UINT16 i;

 if(rfs_env_ctrl_blk_p->media_info_p == NULL)
 	{
 	  memset(fscore_mpt_table->partition_name,0, GBI_MAX_PARTITION_NAME);
	  return;
 	}

 for(i=0; i< rfs_env_ctrl_blk_p->nmb_of_media; i++)
 	{
 	  if(rfs_env_ctrl_blk_p->media_info_p[i].media_nmb == media_num)
 	  	{
 	  	  fscore_mpt_table->media_type = rfs_env_ctrl_blk_p->media_info_p[i].media_type;

 	  	   switch(rfs_env_ctrl_blk_p->media_info_p[i].media_type)
 	  	   	{
 	  	   	  case GBI_MMC:
			  case GBI_SD:
			  	   strncpy((char*)fscore_mpt_table->partition_name,RFS_MPT_NAME_MMC, GBI_MAX_PARTITION_NAME);
			       break;

			  case GBI_INTERNAL_NAND:
			  	    strncpy((char*)fscore_mpt_table->partition_name,RFS_MPT_NAME_NAND, GBI_MAX_PARTITION_NAME);
			  	 break;


 			 case GBI_INTERNAL_NOR:
			 		strncpy((char*)fscore_mpt_table->partition_name,RFS_MPT_NAME_NOR, GBI_MAX_PARTITION_NAME);
			 	break;

			 default:
			 	    memset(fscore_mpt_table->partition_name,0, GBI_MAX_PARTITION_NAME);
				break;
 	  	   	}

		   return;    /* No need to search */
 	  	}
 	}

}

/**
 * sub-function of the media manager: rfs_med_mgr_wt_for_partition_info_rsp_func
 *
 * Detailled description.
 * Handling of the received GBI partition information and and associating a file
 * system core with a partition (partition information includes file system types
 * which are compared with a list of file system types the cores support).
 * Sending each file system core the partition information of the partitions it
 * has to manage.
 *
 * @param   msg_p   Message pointer to message to be handled
 *
 * @return  RFS_OK
 */
static T_RFS_RETURN rfs_med_mgr_wt_for_partition_info_rsp_func(T_RV_HDR *msg_p)
{
  T_RFS_RETURN                  retval = RFS_OK;
  T_GBI_PARTITION_INFO_RSP_MSG  *par_msg_p;
  T_GBI_PARTITION_INFO          *par_info_p;
  T_RV_RETURN                   rfs_ret_path;
  UINT8                         fscore_nmb;
  UINT8                         par_idx;
  UINT8                         fs_core_list_idx;
  UINT8                         partition_cnt;
  T_RVF_MB_STATUS               mb_status;
  T_FSCORE_PARTITION_INFO       *fscore_part_info_p=NULL;
  T_FSCORE_PARTITION_INFO       *temp_fscore_info_p=NULL;
  T_FSCORE_FUNC_TABLE           funcptr_tbl;

  RFS_SEND_TRACE("RFS Media manager: rfs_med_mgr_wt_for_partition_info_rsp_func", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (msg_p->msg_id != GBI_PARTITION_INFO_RSP_MSG)
  {
    /* wrong message received in this state */
    RFS_SEND_TRACE("RFS Media manager: Unexpected message received", RV_TRACE_LEVEL_ERROR);
    return RFS_INTERNAL_ERR;
  }

  /* Cast response message */
  par_msg_p = (T_GBI_PARTITION_INFO_RSP_MSG *)msg_p;


  if(par_msg_p->nmb_of_partitions == 0)
  	{
  	   RFS_SEND_TRACE("RFS Media manager: Zero Number of partitions", RV_TRACE_LEVEL_ERROR);
	   return retval;    /* dont send error, say success, dont change the state of the media manager */
  	}

  if(par_msg_p->info_p == NULL)
  	{
 	   RFS_SEND_TRACE("RFS Media manager: NULL partition info", RV_TRACE_LEVEL_ERROR);
	   return retval;    /* dont send error, say success, dont change the state of the media manager */
  	}


  /* For each FS core */
  for (fscore_nmb = RFS_FAT_NUM  /* starts with RFS_FAT_NUM */; fscore_nmb < RFS_NMB_FSCORE; fscore_nmb++)
  {
    /* Step 1, determine the amount of partitions for this FS-core number */

    /* Temporary use pointer */
    par_info_p    = par_msg_p->info_p;
    partition_cnt = 0;

    /* Gather all information for this FS-core */
    for (par_idx = 0; par_idx < par_msg_p->nmb_of_partitions; par_idx++)
    {
      /* Check if the partition matches this FS-core, it is possible that a
       * FS-core can handle more file_system types.
       */
      for (fs_core_list_idx = 0;
           fs_core_list_idx < RFS_NMB_OF_FSCORE_ITEMS;
           fs_core_list_idx++)
      {
        if (fs_core_list[fs_core_list_idx].rfs_fscore_nmb == fscore_nmb)
        {
          /* Check whether this FS-core list item file system type, matches this
           * partition.
           */
          if (fs_core_list[fs_core_list_idx].rfs_fscore_type == par_info_p->filesystem_type)
          {
            /* current evaluated partition matches this FS-core, make copy
             * possible
             */
            partition_cnt++;
          }
        }
      }
      par_info_p++;
    }

    rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].fscore_numb=fscore_nmb;
	if(rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].partition_info_table != NULL)
	 {
	  	    rvf_free_buf (rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].partition_info_table);
      		rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].partition_info_table = NULL;
	 }
	rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].nbr_partitions = 0;

    if (partition_cnt != 0)
    {


      /* Step 2.b, Reserve dynamicly the correct amount of memory for the
       * partition info
       */
       fscore_part_info_p = NULL;
       mb_status = rvf_get_buf(rfs_env_ctrl_blk_p->prim_mb_id,
                              (partition_cnt * sizeof(T_FSCORE_PARTITION_INFO)),
                              (T_RVF_BUFFER**)&fscore_part_info_p);
      if (mb_status == RVF_RED)
      {
        RFS_SEND_TRACE("RFS Media Manager: Error to get memory ", RV_TRACE_LEVEL_ERROR);
        return RFS_MEMORY_ERR;
      }
      else if (mb_status == RVF_YELLOW)
      {
        RFS_SEND_TRACE("RFS Media Manager: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
      }

      /* Step 3, Copy partition data to the reserved memory */
      temp_fscore_info_p = fscore_part_info_p;
      par_info_p         = par_msg_p->info_p;

      /* Gather all information for this FS-core */
      for (par_idx = 0; par_idx < par_msg_p->nmb_of_partitions; par_idx++)
      {
        /* Check if the partition matches this FS-core, it is possible that a
         * FS-core can handle more file_system types.
         */
        for (fs_core_list_idx = 0;
             fs_core_list_idx < RFS_NMB_OF_FSCORE_ITEMS;
             fs_core_list_idx++)
        {
          if (fs_core_list[fs_core_list_idx].rfs_fscore_nmb == fscore_nmb)
          {
            /* Check whether this FS-core list item file system type, matches this
             * partition.
             */
            if (fs_core_list[fs_core_list_idx].rfs_fscore_type == par_info_p->filesystem_type)
            {
              /* current evaluated partition matches this FS-core, copy data */
              temp_fscore_info_p->media_nbr     = par_info_p->media_nmb;
              temp_fscore_info_p->partition_nbr = par_info_p->partition_nmb;
              strncpy ((char *) temp_fscore_info_p->gbi_part_name,(char *) par_info_p->partition_name,GBI_MAX_PARTITION_NAME);
              temp_fscore_info_p->filesys_type   = par_info_p->filesystem_type;
              temp_fscore_info_p->blk_size       = par_info_p->bytes_per_block;
              temp_fscore_info_p->partition_size = par_info_p->nmb_of_blocks;
			  temp_fscore_info_p->is_mounted = TRUE;    /* its mounted */
			  find_media_type (par_info_p->media_nmb,temp_fscore_info_p);

             RFS_SEND_TRACE_PARAM("RFS Media Manager: Block size ", temp_fscore_info_p->blk_size,RV_TRACE_LEVEL_WARNING);
			  /* check the block size validity */
			  switch (temp_fscore_info_p->blk_size)
			  	{
			  	 case 512:
				 case 1024:
				 case 2048:
				 case 4096:
					 RFS_SEND_TRACE("RFS Media Manager: Found valid block size ", RV_TRACE_LEVEL_WARNING);
					 break;

				 default:
       				 RFS_SEND_TRACE_PARAM_ERROR("RFS Media Manager: Error in block size, fscore index ", fs_core_list_idx);
#if 0
			         // Release previously allocated buffers:
           			rvf_free_buf (fscore_part_info_p);
				  return RFS_INTERNAL_ERR;
#endif

			  	}   /* End of Switch */


              /* Go to possible next fscore info entry */
              temp_fscore_info_p++;
            }
          }
        }

        par_info_p++;
      }

      /* For this FS-core all data is gathered, it can be hand over to the according FS-core */
      rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].nbr_partitions       = par_msg_p->nmb_of_partitions;
      rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb].partition_info_table = fscore_part_info_p;

   }

    /* Step 4, Send the data to the FS-core */
    rfs_ret_path.addr_id       = rfs_env_ctrl_blk_p->addr_id;
    rfs_ret_path.callback_func = NULL;

    switch (fscore_nmb)
    {
      case RFS_FAT_NUM:
        /* Initialize response received variable */
        rfsfat_get_func_table(&funcptr_tbl);

        if (funcptr_tbl.fscore_set_partition_table(
                &(rfs_env_ctrl_blk_p->fscore_part_table[fscore_nmb]),
                rfs_ret_path) != FSCORE_EOK)
        {
          RFS_SEND_TRACE("RFS Media manager: Set partition table FAT failed", RV_TRACE_LEVEL_ERROR);
          // Release previously allocated buffers:
         if(fscore_part_info_p != NULL)
        	{
          		rvf_free_buf (fscore_part_info_p);
        	}
          return RFS_INTERNAL_ERR;
        }
        break;

      default:


       // Release previously allocated buffers:
        if(fscore_part_info_p!= NULL)
        	{
          		rvf_free_buf (fscore_part_info_p);
        	}

        break;
    }
  }

  return retval;
}

/**
 * sub-function of the media manager: rfs_med_mgr_wt_for_fscore_partition_upd_func
 *
 * Detailled description.
 * Handling the responses of the FS-cores regarding their processing of the partition
 * information. When all FS-cores handled the partition information, the RFS can become
 * operational,
 *
 * @param   msg_p   Message pointer to message to be handled
 *
 * @return  RFS_OK
 */


static T_RFS_RETURN rfs_med_mgr_wt_for_fscore_partition_upd_func(T_RV_HDR *msg_p)
{
  T_FSCORE_SET_PARTITION_TABLE_RSP_MSG* rsp_msg_p=(T_FSCORE_SET_PARTITION_TABLE_RSP_MSG*)(msg_p);
  T_FSCORE_PARTITION_TABLE* part_table_p= rsp_msg_p->partition_table;

  RFS_SEND_TRACE("RFS Media manager: rfs_med_mgr_wt_for_fscore_partition_upd_func", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  rfs_env_ctrl_blk_p->fscore_part_table[part_table_p->fscore_numb].nbr_partitions = part_table_p->nbr_partitions;
  rfs_env_ctrl_blk_p->fscore_part_table[part_table_p->fscore_numb].partition_info_table = part_table_p->partition_info_table;


   if((rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL) && (part_table_p != NULL) && (part_table_p->nbr_partitions > 0))
  	{
   		/* Go to the default RFS Media Manager state */
   		rfs_env_ctrl_blk_p->state         = RFS_OPERATIONAL;
   		RFS_SEND_TRACE("RFS is now OPERATIONAL", RV_TRACE_LEVEL_DEBUG_MEDIUM);
  	}

   return RFS_OK;
}



/**
 * sub-function of the media manager: rfs_med_mgr_media_removed
 *
 * Detailled description.
 * When a media is removed, certain actions regarding the mount table should be done.
 *
 * @param   None
 *
 * @return  RFS_OK
 */
static T_RFS_RETURN rfs_med_mgr_media_removed(void)
{
  T_RFS_RETURN           retval = RFS_INTERNAL_ERR;
  T_FSCORE_PARTITION_INFO *tmp_part_table_p;
  UINT8                  nmb_of_parts;
  UINT8                  part_idx, fs_index;


for(fs_index=0; fs_index < rfs_env_ctrl_blk_p->nmb_of_fs;fs_index++)
{
  tmp_part_table_p   = rfs_env_ctrl_blk_p->fscore_part_table[fs_index].partition_info_table;
  nmb_of_parts = rfs_env_ctrl_blk_p->fscore_part_table[fs_index].nbr_partitions;

  for (part_idx = 0; part_idx < nmb_of_parts; part_idx++)
  {
    /* only MC media can be removed run-time */
    if ((tmp_part_table_p->media_type == GBI_MMC) ||(tmp_part_table_p->media_type == GBI_SD) )
     {
      tmp_part_table_p->is_mounted= FALSE;
	  retval = RFS_OK;
    }

    tmp_part_table_p++;
  }
 }
  return retval;
}





/**
 * Blocks RFS waiting for a message.
 *
 * If the parameter msg_id is not null, the function blocks until receiving
 * a message with the same ID, DISCARDING all other messages.
 * If the parameter msg_id is null, the function returns the first message received.
 *
 * Usage:
 * To be called from RTEST only!
 *
 * @param	msg_id	Waited message ID, null for any message.
 * @return	The received message.
 */
T_RV_HDR * rfs_wait_for_message (UINT32 msg_id)
{
	T_RV_HDR * msg_p;

	for (;;)
	{
		/* Waiting for an event, blocking RTEST. */
		if (rvf_wait(0xFFFF, 0) & RVF_TASK_MBOX_0_EVT_MASK)
		{
			msg_p = (T_RV_HDR*)rvf_read_mbox(0);
			if (msg_p != NULL)
			{
				if (msg_id)
				{
					/* If expected msg received, return it */
					if (msg_p->msg_id == msg_id)
					{
						return msg_p;
					}

					RFS_SEND_TRACE("RTEST WARNING: Unexpected message received!",
									RV_TRACE_LEVEL_WARNING);
					rvf_free_buf(msg_p);
				}
				else
				{
					return msg_p;
				}
			}
		}
	}
}



