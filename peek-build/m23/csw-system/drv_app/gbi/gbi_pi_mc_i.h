/**
 * @file	gbi_pi_mc_i.h
 *
 * Plugin MMC/SD: plugin local items.
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

#include "mc/mc_api.h"

typedef enum
{
	GBI_MMC_NOT_INITIALISED,
	GBI_MMC_IDLE,
	GBI_MMC_INTERNAL_ERROR,

  /** Initialisation*/
  GBI_MMC_INIT_WT_FOR_SUBSCR_RSP,
	GBI_MMC_INIT_WT_FOR_SUBSCR_EVENT_RSP,
	GBI_MMC_INIT_WT_FOR_AQ_STCK_RSP,
	GBI_MMC_INIT_WT_FOR_CRD_STCK_SIZE_RSP,
  GBI_MMC_INIT_WT_FOR_RETREIVE_RCA,
  GBI_MMC_INIT_WT_FOR_SND_NOTIF,

  /** gbi_read*/
  GBI_MMC_WT_FOR_READ_RSP,

  /** gbi_read remainder lenght*/
	GBI_MMC_WT_FOR_READ_REMAINDER_RSP,

  /** gbi_write*/
  GBI_MMC_WT_FOR_WRITE_RSP,

  /** gbi_write remainder lenght*/
	GBI_MMC_WT_FOR_WRITE_REMAINDER_RSP,

  /** gbi_erase*/
  GBI_MMC_WT_FOR_ERASE_RSP,

  /** gbi request media info*/
  GBI_MMC_MED_WT_FOR_CID,
  GBI_MMC_MED_WT_FOR_CSD,

  /** gbi request partition info*/
  GBI_MMC_PAR_WT_FOR_CSD,
  GBI_MMC_PAR_WT_FOR_DATA




} T_GBI_MMC_INTERNAL_STATE;


/**
 * The Control Block buffer of GBI, which gathers all 'Global variables'
 * used by GBI instance.
 *
 * A structure should gathers all the 'global variables' of GBI instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_GBI_ENV_CTRL_BLK buffer is allocated when creating GBI instance and is
 * then always refered by GBI instance when access to 'global variable'
 * is necessary.
 */
typedef struct
{
	// Store the current state of the GBI instance
	T_GBI_MMC_INTERNAL_STATE state;
  // Set when MMC plugin initialise is complete
  BOOL plugin_status;
  BOOL mc_plugin_configured;          // added for mc plugin
  T_MC_SUBSCRIBER subscriber;
  // reference to handling message
  // (contains all information to send a response)
  T_RV_RETURN  org_msg_retpath;    //original message return path

  /** MMC/SD driver variables*/
  UINT16     						card_stack_size;
  T_MC_RCA  		*card_stack_p;
  UINT8                 requested_card;
  UINT8                 partition_counter;
  UINT8                 *mmc_data;

  T_GBI_MEDIA_INFO      requested_media_info_p[GBI_MAX_NR_OF_MEDIA];
  T_GBI_PARTITION_INFO  requested_partition_info_p[GBI_MAX_NR_OF_PARTITIONS];

  UINT8                 *requested_cid_p;
  UINT8                 *requested_csd_p;

  void 									**store_data_p;
  void 									**store_nmb_items_p;

} T_GBI_MC_ENV_CTRL_BLK;

/**environment controlblock needed for mmc/sd plugin*/
extern T_GBI_MC_ENV_CTRL_BLK *gbi_mc_env_ctrl_blk_p;

