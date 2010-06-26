/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  RFSFAT
+-----------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  This module implements the process body interface
|             for the entity RFSFAT
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_run       - Process Messages
|          pei_primitive - Process Primitive
|          pei_signal    - Process Signals
|          pei_timeout   - Process Timeout
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+-----------------------------------------------------------------------------
*/

/*============================ Includes ======================================*/
#ifndef _WINDOWS
#include "nucleus.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_cfg.h"
#include "rfsfat/rfsfat_env.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_posconf.h"
#include "rfsfat/rfsfat_pool_size.h"
#include "rfsfat/rfsfat_ffs_disk.h"

#include "../sm_remu/inc/remu_task.h"

/*============================ Macros ========================================*/
#define VSI_CALLER RFSFAT_handle,
#define pei_create rfsfat_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_RFSFAT_ENV_CTRL_BLK *rfsfat_env_ctrl_blk_p = NULL;

T_HANDLE RFSFAT_handle;
T_HANDLE rfsfat_hCommRFSFAT = -1;

static BOOL first_access = TRUE;

//THIS IS CONFIGURABLE AND MINIMAL 512BYTES
#define BYTES_PER_SECTOR    512	/* Number of bytes in a sector              */



//extern T_RVF_MUTEX gbi_rfsfat_sync_mutex;

/*============================ Function Definition============================*/

/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   RVM_TRACE_DEBUG_HIGH ("RFSFAT: pei_exit");

   /*
    * Close communication channels
    */

   vsi_c_close (VSI_CALLER rfsfat_hCommRFSFAT);
   rfsfat_hCommRFSFAT = VSI_ERROR;

   rfsfat_stop(NULL);

   rfsfat_kill();

   return PEI_OK;
}/* End pei_exit(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
|  Parameters   :  taskhandle  - handle of current process
|                  comhandle   - queue handle of current process
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;
   char buf[64] = "";

   RVM_TRACE_DEBUG_HIGH("RFSFAT: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         rfsfat_handle_message(msg_p);
      }
   }

   return PEI_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
//    T_HANDLE gbi_hCommRFSFAT = -1;
    T_RV_RET ret = RV_OK;
    int size;
    RVM_TRACE_DEBUG_HIGH("RFSFAT: Initialization");

    /*
     * Initialize task handle
     */
    RFSFAT_handle = handle;
    rfsfat_hCommRFSFAT = -1;

/*    if (gbi_hCommRFSFAT < VSI_OK)
    {
      if ((gbi_hCommRFSFAT = vsi_c_open (VSI_CALLER "GBI" )) < VSI_OK)
        return PEI_ERROR;
    }
*/
//    rvf_lock_mutex(&gbi_rfsfat_sync_mutex);

    if(RFSFAT_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("RFSFAT_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( RFSFAT_handle, "RFSFAT" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (rfsfat_hCommRFSFAT < VSI_OK)
    {
      if ((rfsfat_hCommRFSFAT = vsi_c_open (VSI_CALLER "RFSFAT" )) < VSI_OK)
        return PEI_ERROR;
    }

    RVM_TRACE_DEBUG_HIGH("RFSFAT Open communication channels done");
    size= sizeof(T_RFSFAT_ENV_CTRL_BLK);
    /* Create instance gathering all the variable used by RFSFAT instance */
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_RFSFAT_ENV_CTRL_BLK), (T_RVF_BUFFER**)&rfsfat_env_ctrl_blk_p) != RVF_GREEN)
    {
       /* The environemnt will cancel the EXPL instance creation. */
       RFSFAT_SEND_TRACE ("RFSFAT: Error to get memory in pei_init ", RV_TRACE_LEVEL_ERROR);
       return RVM_MEMORY_ERR;
    }

	memset(rfsfat_env_ctrl_blk_p,0,sizeof(T_RFSFAT_ENV_CTRL_BLK));

    /* Store the address ID. */
    rfsfat_env_ctrl_blk_p->addr_id = RFSFAT_handle;

    rfsfat_env_ctrl_blk_p->state = RFSFAT_UNINITIALISED;

    /* Store the pointer to the error function. */
    rfsfat_env_ctrl_blk_p->error_ft = rvm_error;

    rfsfat_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;

    rfsfat_init ();

    rfsfat_start();

    return (PEI_OK);

} /* End pei_init(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_create
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when the process is
|                created.
|
| Parameters  : out_name          - Pointer to the buffer in which to locate
|                                   the name of this entity
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

static const T_PEI_INFO pei_info =
              {
               "RFSFAT",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  NULL,     /* pei_primitive */
                  NULL,     /* pei_timeout */
                  NULL,     /* pei_signal */
                  pei_run,
                  NULL,     /* pei_config */
                  NULL      /* pei_monitor */
	       },
               RFSFAT_STACK_SIZE,         /* stack size */
               10,                        /* queue entries */
               (255-RFSFAT_TASK_PRIORITY),  /* priority (1->low, 255->high) */
	           0,                         /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // | PASSIVE_BODY
              };

  RVM_TRACE_DEBUG_HIGH("RFSFAT: pei_create");

  /*
   * Close Resources if open
   */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit();

  /*
   * Export startup configuration data
   */
  *info = (T_PEI_INFO *)&pei_info;

  return PEI_OK;
}/* End pei_create(..) */


/*
+------------------------------------------------------------------------------
| Function    : rfsfat_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfsfat_init (void)
{
  UINT8 i,result;
  /*
   * Here the instance (rfsfat_env_ctrl_blk_p) could be initialised.
   */

  //initialise the second mailbox! that is initialy used for GBI responses
  return_queue_init (rfsfat_env_ctrl_blk_p->addr_id, 1 << 15,
					 &rfsfat_env_ctrl_blk_p->return_path_2nd_queue);

  rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_mpt_idx_rd = RFSFAT_INVALID_MPT;
  rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_mpt_idx_wr = RFSFAT_INVALID_MPT;
  rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_read_blk = BLK_NULL;
  rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_write_blk = BLK_NULL;


  // set mountpoint info table to null config
  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.nbr_partitions = 0;
  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table = NULL;

  //fs core "globals" initialisation
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_bSystemInitDone = FALSE;
  rfsfat_env_ctrl_blk_p->ffs_globals.media_data.astMediaFunctions.MMA_bFuncRead = rfsfat_blk_read;
  rfsfat_env_ctrl_blk_p->ffs_globals.media_data.astMediaFunctions.MMA_bFuncWrite = rfsfat_blk_write;
  //initialize "globals"
  rfsfat_env_ctrl_blk_p->ffs_globals.bInitialised = FALSE;
  rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker[0] = 0x55;
  rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker[1] = 0xaa;
   rfsfat_env_ctrl_blk_p->ffs_globals.aucJmpCode[0] = 0xeb;
  rfsfat_env_ctrl_blk_p->ffs_globals.aucJmpCode[1] = 0x3e;
  rfsfat_env_ctrl_blk_p->ffs_globals.aucJmpCode[2] = 0x90;
  rfsfat_env_ctrl_blk_p->ffs_globals.ucMediaDescr = 0xF8;
  //rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.progress_trckr = 0;
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_stQuery.stLabLoc.tLblPos =
	FFS_QUERY_NOT_STARTED;
  LFN_QUERY_FLAG = FALSE;
  //LFN_QUERY_NAME = NULL;
  RFSFAT_LFN_FLAG = FALSE;
  //QUERY_STARTED =  FALSE;
  RFSFAT_LFN_FROM_LABEL = NULL;


  #if (FFS_UNICODE ==1)

  rfsfat_env_ctrl_blk_p->uc_conv.convert_unicode_to_u8 = convertUcToSFN;
  rfsfat_env_ctrl_blk_p->uc_conv.convert_u8_to_unicode = NULL;
  #endif


  //initialise the fs core
  if (result=E_FFS_ucDiskInit () != FFS_NO_ERRORS)
	{
	  RFSFAT_SEND_TRACE
		("RFSFAT error E_FFS_ucInit, RVM_INTERNAL_ERR returned",
		 RV_TRACE_LEVEL_ERROR);
	  return RVM_INTERNAL_ERR;
	}

  /* Initialize File descriptor table */
  for (i = 0; (i < FSCORE_MAX_NR_OPEN_FSCORE_FILES); i++)
	{
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].fd = RFS_FD_DEF_VALUE;
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].ret_path.addr_id =
		RFS_ADDR_ID_DEF_VALUE;
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].ret_path.callback_func =
		RFS_CB_DEF_VALUE;
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[i].op_mode =
		RFS_OPMODE_DEF_VALUE;
	}
  rfsfat_env_ctrl_blk_p->ffs_globals.fd_rr = 0;	//round robin counter (high FD byte)

  FAM_ucInitFATcash();
  

  /* Initialize mutex data for blocking and secure data handling */
  rvf_initialize_static_mutex (&rfsfat_env_ctrl_blk_p->mutex_pair_id);
  // start value of mesage pair id.
  rfsfat_env_ctrl_blk_p->pair_id = 0;

  rfsfat_env_ctrl_blk_p->state = RFSFAT_INITIALISED;


  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : rfsfat_start
+------------------------------------------------------------------------------
| Description : Called to start the RFSFAT SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfsfat_start (void)
{
  rfsfat_env_ctrl_blk_p->state = RFSFAT_IDLE;
  rfsfat_env_ctrl_blk_p->ffs_globals.last_clusterSeqNmb = 0xFFFFFFFE;
  rfsfat_env_ctrl_blk_p->ffs_globals.last_get_next_cluster_next = 0xFFFFFFFE;
  rfsfat_env_ctrl_blk_p->ffs_globals.clusterBaseNmb = 0xFFFFFFFE;
  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : rfsfat_stop
+------------------------------------------------------------------------------
| Description : Called to stop the RFSFAT SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfsfat_stop (T_RV_HDR * hdr)
{
  /*
   * Here we should stop the activities of the SWE
   * It is still possible to send messages to other SWE, to unregister for example.
   */
  RFSFAT_SEND_TRACE ("RFSFAT: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

#ifdef ICT_SPEEDUP_RFSFAT2
   FAM_ucFreeFATcash();
#endif


  //free mountpoint info table
  RFSFAT_FREE_BUF (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
				   partition_info_table);

  if (E_FFS_ucDiskTerminate () != FFS_NO_ERRORS)
	{
	  RFSFAT_SEND_TRACE
		("RFSFAT error E_FFS_ucDiskTerminate, RVM_INTERNAL_ERR returned",
		 RV_TRACE_LEVEL_ERROR);
	  return RVM_INTERNAL_ERR;
	}
  rfsfat_env_ctrl_blk_p->state = RFSFAT_STOPPED;
  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : rfsfat_kill
+------------------------------------------------------------------------------
| Description : Called to stop the RFSFAT SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfsfat_kill (void)
{
  /*
   * Here we cannot send messages anymore. We only free the last
   * used resources, like the control block buffer.
   */

  RFSFAT_SEND_TRACE ("RFSFAT: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

#ifdef ICT_SPEEDUP_RFSFAT2
   FAM_ucFreeFATcash();
#endif

  rvf_delete_mutex (&rfsfat_env_ctrl_blk_p->mutex_pair_id);

  rfsfat_env_ctrl_blk_p->state = RFSFAT_KILLED;
  RFSFAT_FREE_BUF (rfsfat_env_ctrl_blk_p);

  return RVM_OK;
}

/**
*
* Called by the message handler as action on the configure message
* that is send by the RFS module. It copies the mountpoint table
* into RAM and uses this information to initialise the media manager(MMA)
* (temporarily this function is called by the rfsfat_init funciton until
* the RFS module is created and integrated)
*
* @param           T_RFS_MPT_TABLE *core_mnt_tbl pointer to mountpoint table
*
* @return          RFSFAT_MEMORY_ERR    when not enough memory
*                  RFSFAT_OK            when function executed properly
*                  RFSFAT_INVALID_PARAM when input parameter not valid
*/
T_RFSFAT_RETURN rfsfat_configure (const T_FSCORE_PARTITION_TABLE * core_mnt_tbl)
{
  UINT8 mpt_cntr;
  UINT8 str_cntr;
  UINT8 status=RFSFAT_OK;




  //check input paramters
  RFSFAT_ASSERT_LEAVE ((core_mnt_tbl != NULL), return RFSFAT_INVALID_PARAM);

  //check if maybe a null configuration is selected
  if ((core_mnt_tbl->nbr_partitions == 0)
	  && (core_mnt_tbl->partition_info_table == NULL))
	{

	  //null config --> free internal mountpoint table if it existed
	  if (MPT_TABLE != NULL)
		{

		  //free existing mountpoint info table
		  RFSFAT_FREE_BUF (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
						   partition_info_table);
		}
	  rfsfat_env_ctrl_blk_p->state = RFSFAT_IDLE;
	  return RFSFAT_OK;
	}
  else if ((core_mnt_tbl->nbr_partitions == 0)
		   || (core_mnt_tbl->partition_info_table == NULL))
	{

	  //if only one of them is null / NULL return error
	  RFSFAT_SEND_TRACE_ERROR
		("the number of partitions is 0 or the Partitiontable is NULL");
	  return RFSFAT_INTERNAL_ERR;
	}
  else if (MPT_TABLE != NULL)
	{

	  //normal config -->
	  //if previously allocated free existing mountpoint info table
	  RFSFAT_FREE_BUF (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table);
	}


  
  //table is created! now copy data configuration in it
  NR_OF_MOUNTPOINTS = core_mnt_tbl->nbr_partitions;

  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.fscore_numb = core_mnt_tbl->fscore_numb;   /* File system Number */


if(NR_OF_MOUNTPOINTS > GBI_MAX_NR_OF_PARTITIONS)
  	{

     RFSFAT_SEND_TRACE_PARAM_ERROR ("the number of partitions are more than GBI_MAX_NR_OF_PARTITIONS", core_mnt_tbl->nbr_partitions);

	 NR_OF_MOUNTPOINTS = GBI_MAX_NR_OF_PARTITIONS;   /* reset to max partitions */ 	 
	 
     RFSFAT_SEND_TRACE_PARAM_ERROR ("the number of partitions are reseting to GBI_MAX_NR_OF_PARTITIONS", core_mnt_tbl->nbr_partitions);
	  
  	}
 
  

  
  //copy config delivered by RFS into internal mntp_tbl
  //allocate memory for mpt_tbl

  RFSFAT_GETBUF((sizeof (T_FSCORE_PARTITION_INFO) *NR_OF_MOUNTPOINTS),
                &MPT_TABLE);

  for (mpt_cntr = 0; mpt_cntr < NR_OF_MOUNTPOINTS; mpt_cntr++)
	{

	  for (str_cntr = 0; str_cntr < GBI_MAX_PARTITION_NAME; str_cntr++)
		{

		  //copy mountpoint name
		  if (core_mnt_tbl->partition_info_table[mpt_cntr].
			  partition_name[str_cntr] != FFS_ASCII_EOL)
			{

			  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
				partition_info_table[mpt_cntr].partition_name[str_cntr] =
				core_mnt_tbl->partition_info_table[mpt_cntr].
				partition_name[str_cntr];
			}
		  else
			{
			  //end of string reached -> stop copying
			  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
				partition_info_table[mpt_cntr].partition_name[str_cntr] =
				FFS_ASCII_EOL;
			  break;
			}
		}
	  //store the mountpoint name as all uppercases
	  string_toupper (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
					  partition_info_table[mpt_cntr].partition_name);
	  //copy media nbr
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].media_nbr =
		core_mnt_tbl->partition_info_table[mpt_cntr].media_nbr;
	  //copy partition_nbr
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].partition_nbr =
		core_mnt_tbl->partition_info_table[mpt_cntr].partition_nbr;
	  //copy filesys_type
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].filesys_type =
		core_mnt_tbl->partition_info_table[mpt_cntr].filesys_type;
	  //copy blk_size
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].blk_size =
		core_mnt_tbl->partition_info_table[mpt_cntr].blk_size;

	  //Media type
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].media_type =
		core_mnt_tbl->partition_info_table[mpt_cntr].media_type;

      //is the mountpoint mounted
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].is_mounted =
		core_mnt_tbl->partition_info_table[mpt_cntr].is_mounted;




      /* check for the valid block size */
	   RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("Valid block size at mount point number ",rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_cntr].blk_size);
	  switch (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table[mpt_cntr].blk_size)
	  	{
	  	 case 512:
		 case 1024:
		 case 2048:
		 case 4096:
		 	   RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("Valid block size at mount point number ",mpt_cntr);
			   break;

	  	default:
		 	 RFSFAT_SEND_TRACE_PARAM_ERROR("FATAL ERROR: Invalid block size of the mount point number", mpt_cntr);
			 RFSFAT_SEND_TRACE_ERROR("FATAL ERROR: Setting block size of to default 512");
			 break;

	  	}



	  //copy partition_size
	  rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.
		partition_info_table[mpt_cntr].partition_size =
		core_mnt_tbl->partition_info_table[mpt_cntr].partition_size;
	}


if((rfsfat_env_ctrl_blk_p->state == RFSFAT_IDLE) || (rfsfat_env_ctrl_blk_p->state == RFSFAT_CONFIGURED))
 {
  status=MMA_ucInit ();

  if ( status != FFS_NO_ERRORS)
	{
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: exit rfsfat_configure -> media manager \
			initialisation failed");

	}
  else
  	{
  	   rfsfat_env_ctrl_blk_p->state = RFSFAT_CONFIGURED;    /* Keep the state of RFSFAT in configured */
  	}

 }

  return status;
}

/**
*
* This function is called to configure the file system and to provide
* it with a mountpoint information list. This function will copy this list
* to its own buffer
*
* @param            T_FSCORE_PARTITION_TABLE *partition_table
* @param            T_RV_RETURN return_path
*
* @return           RFSFAT_MEMORY_ERR    when not enough memory
*                   RFSFAT_OK            when function executed properly
*                   RFSFAT_INVALID_PARAM when input parameter not valid
*/
T_FSCORE_RET rfsfat_int_set_partition_table (const T_FSCORE_PARTITION_TABLE *
								partition_table)
{
  T_FSCORE_PARTITION_INFO *info_table;
  UINT8 i;
  T_RFSFAT_RETURN ret;

  //check input paramters
  RFSFAT_ASSERT_LEAVE ((partition_table != NULL), return FSCORE_EINVALID);

  info_table = partition_table->partition_info_table;
  if (partition_table->nbr_partitions != 0)
	{

	  for (i = 0; i < partition_table->nbr_partitions; i++)
		{
		  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("blk_size", info_table->blk_size);
		  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("filesys_type", info_table->filesys_type);
		  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("media_nbr", info_table->media_nbr);
		  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("partition_nbr", info_table->partition_nbr);
		  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("partition_size",info_table->partition_size);
		  info_table++;
		}
	}
  else
	{
	  RFSFAT_SEND_TRACE_DEBUG_LOW("NR partitions = 0, error or valid NULL config");
	}
  ret = rfsfat_configure (partition_table);
 if (ret != RFSFAT_OK)
	{
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT rfsfat_configure, !RFSFAT_OK returned");
	  return FSCORE_EINTERNALERROR;
	}

  return FSCORE_EOK;
}
