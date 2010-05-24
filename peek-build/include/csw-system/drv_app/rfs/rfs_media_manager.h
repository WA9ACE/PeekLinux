/**
 * @file   rfs_media_manager.h
 *
 * Header file containing definitions of rfs_media_manager.c
 *
 * @author  
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

#ifndef _RFS_MEDIA_MANAGER_
#define _RFS_MEDIA_MANAGER_

/* Defines the possible states of the RFS Media Manager. */
typedef enum{
  RFS_MED_MGR_NOT_INITIALISED,
  RFS_MED_MGR_WT_FOR_EVENT_IND,
  RFS_MED_MGR_WT_FOR_MEDIA_INFO_RSP,
  RFS_MED_MGR_WT_FOR_PARTITION_INFO_RSP,
  RFS_MED_MGR_WT_FOR_FSCORE_PARTITION_UPD
} T_RFS_MEDIA_MANAGER_STATE;


#endif /* #ifndef _RFS_MEDIA_MANAGER_ */
