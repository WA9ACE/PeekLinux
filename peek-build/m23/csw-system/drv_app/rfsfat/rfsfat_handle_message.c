/**
 * @file  rfsfat_handle_message.c
 *
 * RFSFAT handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author  Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  1/29/2004 Anton van Breemen (abreemen@ict.nl)   Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"
#include "rfsfat/rfsfat_posconf.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_message.h"
#include "rfsfat/rfsfat_env.h"


static T_FSCORE_RET rfsfat_convert_ffs_result (UINT8 ffs_status,
											   T_FSCORE_RET result);

T_FSCORE_RET set_part_tbl (T_FSCORE_SET_PARTITION_TABLE_REQ_MSG * spreqmsg_p);


/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param msg_p Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN
rfsfat_handle_message (T_RV_HDR * msg_p)
{
  T_FSCORE_SET_PARTITION_TABLE_REQ_MSG *spreqmsg_p;

  if (msg_p != NULL)
	{
	  switch (rfsfat_env_ctrl_blk_p->state)
		{
		case RFSFAT_CONFIGURED:
		  switch (msg_p->msg_id)
			{
			case RFSFAT_OPEN_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: file open message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_open (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Open failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
			  
#if (FFS_UNICODE == 1)

	case RFSFAT_OPEN_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: file open message (Unicode)",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_open_uc (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Open failed (Unicode)",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
#endif


			  
			case RFSFAT_READ_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: read message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

#if (WCP_PROF == 1)
	prf_LogPointOfInterest("RFSFAT FILE READ START \0\0");
#endif

			  if (rfsfat_ffs_file_read (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Read failed",
									 RV_TRACE_LEVEL_ERROR);
				}

#if (WCP_PROF == 1)
	prf_LogPointOfInterest("RFSFAT FILE READ END \0\0");
#endif

			  
			  break;
			case RFSFAT_WRITE_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: write message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_write (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Write failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
			case RFSFAT_CLOSE_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: close message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_close (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Close failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
			case RFSFAT_REMOVE_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: remove message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_remove (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Remove failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#if (FFS_UNICODE == 1)

			case RFSFAT_REMOVE_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: remove message (Unicode)",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_remove_uc (msg_p) != FSCORE_EOK)
				{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Remove failed (Unicode)",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#endif
			  

			case RFSFAT_LSEEK_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: lseek message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  /* lseek action to perform */
			  if (rfsfat_ffs_file_lseek (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE
					("RFSFAT: move file pointer (lseek) failed",
					 RV_TRACE_LEVEL_ERROR);
				}
			  break;

			case RFSFAT_FCHMOD_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Fchmod message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  /* Fchmod action to perform */
			  if (rfsfat_ffs_file_fchmod (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_file_fchmod failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

			case RFSFAT_FSTAT_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Fstat message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  /* Fstat action to perform */
			  if (rfsfat_ffs_file_fstat (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_file_fstat failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

			case RFSFAT_FSYNC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Fsync message",
								 RV_TRACE_LEVEL_DEBUG_LOW);

			  /* Fsync action to perform */
			  if (rfsfat_ffs_file_fsync (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_file_fsync failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

			case RFSFAT_CHMOD_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Chmod message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Chmod action to perform */
			  if (rfsfat_ffs_fman_chmod (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_fman_chmod failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;



#if (FFS_UNICODE == 1)
			case RFSFAT_CHMOD_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Chmod message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Chmod action to perform */
			  if (rfsfat_ffs_fman_chmod_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_fman_chmod failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#endif



			case RFSFAT_STAT_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Stat message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Stat action to perform */
			  if (rfsfat_ffs_stat (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_stat failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
			  
#if (FFS_UNICODE == 1)
			case RFSFAT_STAT_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Stat_uc message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Stat action to perform */
			  if (rfsfat_ffs_stat_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_stat_uc failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#endif

			  

			case RFSFAT_RENAME_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Rename message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Rename action to perform */
			  if (rfsfat_ffs_fman_rename (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_fman_rename failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;


#if (FFS_UNICODE == 1)
			case RFSFAT_RENAME_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Rename message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Unicode Rename action to perform */
			  if (rfsfat_ffs_fman_rename_uc(msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_fman_rename failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
#endif
			  

			case RFSFAT_MKDIR_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Mkdir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Mkdir action to perform */
			  if (rfsfat_ffs_dman_mkdir (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_mkdir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#if (FFS_UNICODE == 1)
			case RFSFAT_MKDIR_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Mkdir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Unicode Mkdir action to perform */
			  if (rfsfat_ffs_dman_mkdir_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_mkdir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#endif
			  

			case RFSFAT_RMDIR_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Rmdir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Rmdir action to perform */
			  if (rfsfat_ffs_dman_rmdir (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_rmdir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#if (FFS_UNICODE ==1)
			case RFSFAT_RMDIR_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Rmdir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Rmdir action to perform */
			  if (rfsfat_ffs_dman_rmdir_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_rmdir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
#endif



			case RFSFAT_OPENDIR_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Opendir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Opendir action to perform */
			  if (rfsfat_ffs_dman_opendir (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_opendir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;


#if (FFS_UNICODE ==1)
			case RFSFAT_OPENDIR_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Opendir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Opendir action to perform */
			  if (rfsfat_ffs_dman_opendir_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_opendir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
#endif

			  

			case RFSFAT_READDIR_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Readdir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Readdir action to perform */
			  if (rfsfat_ffs_dman_readdir (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_readdir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#if (FFS_UNICODE == 1)
			case RFSFAT_READDIR_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Readdir message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Readdir action to perform */
			  if (rfsfat_ffs_dman_readdir_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_dman_readdir failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;
#endif
			  

			case RFSFAT_PREFORMAT_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Preformat message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Preformat action to perform */
			  if (rfsfat_ffs_preformat (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_preformat failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;


#if (FFS_UNICODE == 1)
			case RFSFAT_PREFORMAT_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Format_uc message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Format action to perform */
			  if (rfsfat_ffs_preformat_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_format_uc failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#endif


			case RFSFAT_FORMAT_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Format message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Format action to perform */
			  if (rfsfat_ffs_format (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_format failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#if (FFS_UNICODE == 1)
			case RFSFAT_FORMAT_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Format_uc message",
								 RV_TRACE_LEVEL_DEBUG_LOW);
			  /* Format action to perform */
			  if (rfsfat_ffs_format_uc (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: rfsfat_ffs_format_uc failed",
									 RV_TRACE_LEVEL_ERROR);
				}
			  break;

#endif
			  
			case RFSFAT_TRUNC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Trunc message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_trunc (msg_p) != FSCORE_EOK)
			  	{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Trunc failed", RV_TRACE_LEVEL_ERROR);
			  	}  
			break;

			case RFSFAT_TRUNC_NAME_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Trunc File name message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_trunc_name (msg_p) != FSCORE_EOK)
			  	{
				 RFSFAT_SEND_TRACE ("RFSFAT: File Trunc (filename) failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;

#if (FFS_UNICODE == 1)
			case RFSFAT_TRUNC_NAME_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Trunc File name message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_file_trunc_name_uc (msg_p) != FSCORE_EOK)
			  	{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Trunc (filename) failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;

#endif

			
			case RFSFAT_FILE_READ_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Read File name message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_file_read_by_name(msg_p) != FSCORE_EOK)
			  	{
				RFSFAT_SEND_TRACE ("RFSFAT: File read (filename) failed", RV_TRACE_LEVEL_ERROR);
			  	}
			break;

			case RFSFAT_FILE_WRITE_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Write File name message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_file_write_by_name(msg_p) != FSCORE_EOK)
			  	{
				  RFSFAT_SEND_TRACE ("RFSFAT: File Write (filename) failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;

			case RFSFAT_SET_VOL_LABEL_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Set volume label message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_vol_set_label(msg_p) != FSCORE_EOK)
			  	{
				  RFSFAT_SEND_TRACE ("RFSFAT: Set volume label failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;
			
			case RFSFAT_GET_VOL_LABEL_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: Get volume label message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_vol_get_label(msg_p) != FSCORE_EOK)
			  	{
				  RFSFAT_SEND_TRACE ("RFSFAT: Get volume label failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;	
			
			case RFSFAT_GETATTRIB_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: get file/dir attribute message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_getattribute(msg_p) != FSCORE_EOK)
			  	{
				 RFSFAT_SEND_TRACE ("RFSFAT: get file/dir attribute failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;
			
#if (FFS_UNICODE == 1)
			case RFSFAT_GETATTRIB_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: get file/dir attribute message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_getattribute_uc(msg_p) != FSCORE_EOK)
			  	{
				 RFSFAT_SEND_TRACE ("RFSFAT: get file/dir attribute failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;
#endif

              case RFSFAT_SETATTRIB_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: set file/dir attribute message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_setattribute(msg_p) != FSCORE_EOK)
			  	{
				 RFSFAT_SEND_TRACE ("RFSFAT: set file/dir attribute failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;

#if (FFS_UNICODE == 1)
			case RFSFAT_SETATTRIB_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: get file/dir attribute message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_setattribute_uc(msg_p) != FSCORE_EOK)
			  	{
				RFSFAT_SEND_TRACE ("RFSFAT: get file/dir attribute failed", RV_TRACE_LEVEL_ERROR);
			  	}
			break;

#endif
			
 
			case RFSFAT_GETDATETIME_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: get date and time message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_getdatetime(msg_p) != FSCORE_EOK)
			  	{
				  RFSFAT_SEND_TRACE ("RFSFAT: get date and time  failed", RV_TRACE_LEVEL_ERROR);
			  	} 
			break;


 #if (FFS_UNICODE == 1)
			case RFSFAT_GETDATETIME_UC_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: get date and time message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_getdatetime_uc(msg_p) != FSCORE_EOK)
			  	{
				RFSFAT_SEND_TRACE ("RFSFAT: get date and time  failed", RV_TRACE_LEVEL_ERROR);
			  	}
			break;



#endif


           case RFSFAT_MOUNT_UNMOUNT_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: mount /unmount message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_mount_unmount(msg_p) != FSCORE_EOK)
			  	{
				RFSFAT_SEND_TRACE ("RFSFAT: mount / unmount failed", RV_TRACE_LEVEL_ERROR);
			  	}
			break;		   	


			
			case RFSFAT_SET_PARTITION_TABLE_REQ_MSG:

			  spreqmsg_p = (T_FSCORE_SET_PARTITION_TABLE_REQ_MSG *) msg_p;
			  if (set_part_tbl (spreqmsg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE
					("RFSFAT: set_part_tbl failed from CONFIG state",
					 RV_TRACE_LEVEL_ERROR);
				}
			  break;
#if 0
			case RFSFAT_QUERY_PARTITION_REQ_MSG:
             /* response for get partition info */
           // if(rfsfat_handle_query_partition_info(msg_p)!= FSCORE_EOK)
            	{
         			RFSFAT_SEND_TRACE
					("RFSFAT: Handle query partiton message failed from RFSFAT configure state",
					 RV_TRACE_LEVEL_ERROR);
	           	}
			break;
#endif			
			default:
			  RFSFAT_SEND_TRACE
				("RFSFAT: Got an unexpected event in this state - Ignored",
				 RV_TRACE_LEVEL_WARNING);
			  break;
			}
		  break;
		case RFSFAT_IDLE:
		  switch (msg_p->msg_id)
			{
			case RFSFAT_SET_PARTITION_TABLE_REQ_MSG:

			  spreqmsg_p = (T_FSCORE_SET_PARTITION_TABLE_REQ_MSG *) msg_p;
			  if (set_part_tbl (spreqmsg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE
					("RFSFAT: set_part_tbl failed from IDLE state",
					 RV_TRACE_LEVEL_ERROR);
				}
			  break;
			case RFSFAT_PREFORMAT_REQ_MSG:
			  RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: Preformat message");
			  /* Preformat action to perform */
			  if (rfsfat_ffs_preformat (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE_ERROR
					("RFSFAT: rfsfat_ffs_preformat failed");
				}
			  break;

			case RFSFAT_FORMAT_REQ_MSG:
			  RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: Format message");
			  /* Format action to perform */
			  if (rfsfat_ffs_format (msg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE_ERROR
					("RFSFAT: rfsfat_ffs_format failed");
				}
			  break;
			default:
			  RFSFAT_SEND_TRACE
				("RFSFAT: Got an unexpected event in IDLE state - Ignored",
				 RV_TRACE_LEVEL_WARNING);
			  break;
			}
		  break;
  
		case RFSFAT_STOPPED:

	  switch (msg_p->msg_id)
			{
			

         case RFSFAT_MOUNT_UNMOUNT_REQ_MSG:
			  RFSFAT_SEND_TRACE ("RFSFAT: mount /unmount message", RV_TRACE_LEVEL_DEBUG_LOW);

			  if (rfsfat_ffs_mount_unmount(msg_p) != FSCORE_EOK)
			  	{
				RFSFAT_SEND_TRACE ("RFSFAT: mount / unmount failed", RV_TRACE_LEVEL_ERROR);
			  	}
			break;		   	

			
			case RFSFAT_SET_PARTITION_TABLE_REQ_MSG:

			  spreqmsg_p = (T_FSCORE_SET_PARTITION_TABLE_REQ_MSG *) msg_p;
			  if (set_part_tbl (spreqmsg_p) != FSCORE_EOK)
				{

				  RFSFAT_SEND_TRACE
					("RFSFAT: set_part_tbl failed from STOPPED state",
					 RV_TRACE_LEVEL_ERROR);
				}
			  break;



	  	}
			
		  /* ... */
		//  break;
		default:
		  /* Unknow message has been received */
		  RFSFAT_SEND_TRACE ("RFSFAT: RFSFAT in an unknown state",
							 RV_TRACE_LEVEL_ERROR);
		  break;
		}

	  /* Free message */
	  RFSFAT_FREE_BUF (msg_p);
	}

  return RVM_OK;
}


//********************SECOND MAILBOX****************************************/
/* return queue structure, used to store responses from lower level driver */
static struct
{
  T_RVF_G_ADDR_ID addr_id;
  UINT16 event;
  T_RVF_BUFFER_Q queue_obj;
}
return_queue;

/* store message in return queue, raise an event */
static void
return_queue_callback (void *msg_p)
{
  if (RVF_OK != rvf_enqueue (&return_queue.queue_obj, msg_p) ||
	  0 != rvf_send_event (return_queue.addr_id, return_queue.event))
	{
	  RFSFAT_SEND_TRACE ("RFSFAT RETURN QUEUE lost message",
						 RV_TRACE_LEVEL_ERROR);
	}
}

/* get a message from the return queue */
void *
return_queue_get_msg (void)
{
  while (!(return_queue.queue_obj.count))
	{
	  /* no messages queued, block until msg available */
	  rvf_wait (return_queue.event, 0);
	}
  return rvf_dequeue (&return_queue.queue_obj);
}

/* init return queue object */
void
return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
				   T_RV_RETURN_PATH * path_to_return_queue_p)
{
  return_queue.addr_id = rq_addr_id;
  return_queue.event = rq_event;
  return_queue.queue_obj.p_first = NULL;
  return_queue.queue_obj.p_last = NULL;
  return_queue.queue_obj.count = 0;
  path_to_return_queue_p->addr_id = RVF_INVALID_ADDR_ID;
  path_to_return_queue_p->callback_func = return_queue_callback;
}


/**
 *  This sends Riviera 'command ready' response messages.
 *  @param cmdId        Input: identifies the initiating command
 *  @param pair_value   Input: pairs the command to the response
 *  @param return_path  Input: where to send the ready message too
 *  @param internal_status    Input: internal filesystem status
 *  @param result       Input: core status. only of interrest if internal_status
 *                        is not ok.
 *
 *  @return fscore status.
 */
T_FSCORE_RET
rfsfat_send_response_msg (T_FSCORE_CMD_ID cmdId,
						  T_FSCORE_PAIR_VALUE pair_value,
						  T_RV_RETURN * return_path,
						  UINT8 internal_status, T_FSCORE_RET result)
{
  T_FSCORE_READY_RSP_MSG *msg_p;
  T_FSCORE_RET fscore_result;
  T_RV_RET rv_retval = RV_OK;

  /* Convert FFS error code to FSCORE error code */
  fscore_result = rfsfat_convert_ffs_result (internal_status, result);

  /* reserve response message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_FSCORE_READY_RSP_MSG),
					   RFSFAT_READY_RSP_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message */
	  RFSFAT_SEND_TRACE ("RFSFAT fsfat ready msg: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_READY_RSP_MSG;
  msg_p->fscore_nmb = rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.fscore_numb;   /* File system Number */
  msg_p->command_id = cmdId;
  msg_p->pair_value = pair_value;
  msg_p->result = fscore_result;

  /* follow return path: callback or mailbox */
  if (return_path->callback_func != NULL)
	{
	  return_path->callback_func (msg_p);
	  //do not free message here. Caller may have queue this message
	}
  else
	{
	  rv_retval = rvf_send_msg (return_path->addr_id, msg_p);
	}
  if (rv_retval != RVF_OK)
	{
	  RFSFAT_SEND_TRACE
		("RFS rfsfat_send_response_to_client: could not send response msg",
		 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  return FSCORE_EOK;
}


/**
* This function is used to call the set the internal fscore partition table
* and to send response to the client.
* 
* @param              T_FSCORE_SET_PARTITION_TABLE_REQ_MSG   *spreqmsg_p
* @param              T_FSCORE_SET_PARTITION_TABLE_REQ_MSG   *sprspmsg_p
*
* @return             FSCORE_EOK  
*/
T_FSCORE_RET
set_part_tbl (T_FSCORE_SET_PARTITION_TABLE_REQ_MSG * spreqmsg_p)
{
  T_FSCORE_SET_PARTITION_TABLE_RSP_MSG *sprspmsg_p;
  T_FSCORE_RET ret_value = FSCORE_EOK;

  RFSFAT_SEND_TRACE ("RFSFAT: set partititon table message",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  RFSFAT_ASSERT_LEAVE (((spreqmsg_p != NULL)), return FSCORE_ENOENT);

  //TO DO: solve the different types of return values FSCORE/RFSFAT/RV_
  if (FSCORE_EOK !=
	  rfsfat_int_set_partition_table (spreqmsg_p->partition_table))
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: set partititon table failed",
						 RV_TRACE_LEVEL_ERROR);
	  ret_value = FSCORE_ENOENT;
	}
  else
	{
	  ret_value = FSCORE_EOK;
	}

  /* reserve response message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_FSCORE_SET_PARTITION_TABLE_RSP_MSG),
					   RFSFAT_SET_PARTITION_TABLE_RSP_MSG,
					   (T_RV_HDR **) & sprspmsg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message */
	  RFSFAT_SEND_TRACE ("RFSFAT fsfat ready msg: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	}
  else
	{
	  /* compose message */
	  sprspmsg_p->hdr.msg_id = RFSFAT_SET_PARTITION_TABLE_RSP_MSG;
	  sprspmsg_p->partition_table = spreqmsg_p->partition_table;
	  sprspmsg_p->result = ret_value;

	  /* follow return path: callback or mailbox */
	  if (spreqmsg_p->return_path.callback_func != NULL)
		{
		  spreqmsg_p->return_path.callback_func (sprspmsg_p);
		  //do not free message here. Caller may have queue this message
		}
	  else
		{
		  if (rvf_send_msg (spreqmsg_p->return_path.addr_id, sprspmsg_p) !=
			  RVF_OK)
			{
			  RFSFAT_SEND_TRACE
				("RFS rfsfat_send_response_to_client: could not send response msg",
				 RV_TRACE_LEVEL_WARNING);
			}
		}
	  RFSFAT_SEND_TRACE
		("RFS set partititon table OK, send_response_to_client",
		 RV_TRACE_LEVEL_DEBUG_LOW);
	}
  return FSCORE_EOK;
}

/**
* This function converts the FFS internal status into a FS-core specified return value.
* 
*  @param   ffs_status      Input:  internal filesystem status
*
*  @return  fscore_result  
*/
static T_FSCORE_RET
rfsfat_convert_ffs_result (UINT8 ffs_status, T_FSCORE_RET result)
{

  switch (ffs_status)
	{
	case FFS_NO_ERRORS:
	  /* ffs_status doesn't contain any errors, so return result */
	  return result;

	case FFS_RV_UNAVAILABLE:
	  return FSCORE_ENODEVICE;

	case FFS_RV_FORMATTED:
	  return FSCORE_EOK;

	case FFS_RV_UNFORMATTED:
	  return FSCORE_EUNKNOWNPART;

	case FFS_RV_ERR_NOT_INITIALISED:
	  return FSCORE_EAGAIN;

	case FFS_RV_ERR_FILE_ALREADY_EXISTS:
	  return FSCORE_EEXISTS;

	case FFS_RV_ERR_INVALID_ACCESS_MODE:
	  return FSCORE_EACCES;

	case FFS_RV_ERR_INVALID_HANDLE:
	  return FSCORE_EBADFD;

	case FFS_RV_ERR_INVALID_NAMEEXT:
	  return FSCORE_EBADNAME;

	case FFS_RV_ERR_INVALID_OFFSET:
	  return FSCORE_EINVALID;

	case FFS_RV_ERR_INVALID_PATH:
	  return FSCORE_EINVALID;

	case FFS_RV_IS_DIR:
	  return FSCORE_ENOTAFILE;

	case FFS_RV_IS_PART:
	  return FSCORE_ENOTAFILE;

	case FFS_RV_IS_NOT_A_DIR:
	  return FSCORE_ENOTADIR;

	case FFS_RV_IS_NOT_A_FILE:
	  return FSCORE_ENOTAFILE;

	case FFS_RV_IS_NOT_A_PART:
	  return FSCORE_EMOUNT;

	case FFS_RV_ERR_FILE_ALLREADY_OPEN:
	  return FSCORE_ELOCKED;

	case FFS_RV_ERR_FILE_READ_ONLY:
	  return FSCORE_EACCES;

	case FFS_RV_ERR_TO_MUCH_OPEN_FILES:
	  return FSCORE_ENUMFD;

	case FFS_RV_ERR_INVALID_PARAMS:
	  return FSCORE_EINVALID;

	case FFS_RV_ERR_WRITE_NOT_ALLOWED:
	  return FSCORE_EACCES;

	case FFS_RV_ERR_TOO_MUCH_FILE_DATA:
	  return FSCORE_EFBIG;

	case FFS_RV_ERR_ENTRY_EMPTY:
	  return FSCORE_EEMPTY;

	case FFS_RV_ERR_LAST_ENTRY:
	  return FSCORE_EEMPTY;

	case FFS_RV_FILES_IN_OPEN_FILE_LIST:
	  return FSCORE_ELOCKED;

	case FFS_RV_ROOTDIR_SELECTED:
	  return FSCORE_EBADOP;

	case FFS_RV_ALREADY_INITIALISED:
	  return FSCORE_EINTERNALERROR;

	case FFS_RV_ERR_DIFFERENT_PART:
	  return FSCORE_EBADOP;

	case FFS_RV_ERR_DATE_TIME_DETERMINATION:
	  return FSCORE_EINTERNALERROR;

	case FFS_RV_ERR_MOVING_NOT_ALLOWED:
	  return FSCORE_ENOTALLOWED;

	case FFS_RV_ERR_REMOVING_NOT_ALLOWED:
	  return FSCORE_ENOTALLOWED;

	case FFS_RV_ERR_WRITING_IN_READMODE:
	  return FSCORE_EACCES;

	case FFS_RV_NO_SUBDIR_OF_DIR:
	  return FSCORE_EOK;

	case FFS_RV_SUBDIR_OF_DIR:
	  return FSCORE_EOK;

	case FFS_RV_FEATURE_UNAVAILABLE:
	  return FSCORE_ENOTSUPPORTED;

	/*************************************************/
	  /* Directory module return values.               */
	/*************************************************/
	case DIR_RV_DIR_IS_EMPTY:
	  return FSCORE_EEMPTY;

  case DIR_RV_CORRUPT_LFN:
	  return FSCORE_CORRUPT_LFN;

	case DIR_RV_DIR_IS_NOT_EMPTY:
	  return FSCORE_ENOTEMPTY;

	case DIR_RV_DIR_NOT_FOUND:
	  return FSCORE_ENOENT;

	case DIR_RV_DIRTABLE_FULL:
	  return FSCORE_EFSFULL;

	case DIR_RV_INVALID_PARAM:
	  return FSCORE_EINVALID;

	/*************************************************/
	  /*   Return values used by the FAT manager       */
	/*************************************************/
	  /* Not all FAM return values are returned to FFS top-level, 
	   * altough this fact, they should be supported  
	   */
	case FAM_RET_CLUSTER_NR:
	  return FSCORE_EOK;		/* No error ? */

	case FAM_RET_BAD_CLUSTER:
	  return FSCORE_EDEVICE;

	case FAM_RET_NO_FAT_CLUSTER:
	  return FSCORE_EDEVICE;

	case FAM_RET_LAST_CLUSTER:
	  return FSCORE_EOK;		/* No error ? */

	case FAM_RET_FAT_READ_ERROR:
	  return FSCORE_EDEVICE;

	case FAM_RET_FAT_WRITE_ERROR:
	  return FSCORE_EDEVICE;

	case FAM_RET_FAT_NO_SPACE:
	  return FSCORE_ENOSPACE;

	case FAM_RESERVED_CLUSTER:
	  return FSCORE_EDEVICE;

	case FAM_RET_FREE_CLUSTER:
	  return FSCORE_EOK;		/* No error ? */

	case FAM_RET_ROOT_CLUSTER:
	  return FSCORE_EOK;		/* No error ? */

	/*************************************************/
	  /*   Return values used by the MMA manager       */
	/*************************************************/
	  /* Not all MMA return values are returned to FFS top-level, 
	   * altough this fact, they should be supported  
	   */

	case MMA_RET_READ_ERR:
	  return FSCORE_ENOSPACE;

	case MMA_RET_RW_OUTSIDE_PARTITION:
	case MMA_RET_VOLUME_NOT_AVAILABLE:	
	  return FSCORE_EDEVICE;

	/*************************************************/
	  /*   Return values used by the CLM manager       */
	/*************************************************/
	  /* Not all CLM return values are returned to FFS top-level, 
	   * altough this fact, they should be supported  
	   */

	case CLM_RET_UNDEF_ERROR:
	  return FSCORE_EINTERNALERROR;

	case CLM_RET_LIST_ERROR:
	  return FSCORE_EDEVICE;

	case CLM_RET_NO_SPACE:
	  return FSCORE_ENOSPACE;


	default:
	  RFSFAT_SEND_TRACE
		("Unknown FFS error, convert to FS-core result is not possible",
		 RV_TRACE_LEVEL_ERROR);

	  return FSCORE_EINTERNALERROR;
	}
}


#if 0

//T_FSCORE_RET    rfsfat_handle_query_partition_info(void* msg_p)
{
  T_RFSFAT_QUERY_PARTITION_REQ_MSG* par_query_msg_p;
  int mount_index,retVal=FFS_RV_UNAVAILABLE;
  T_FSCORE_PAIR_VALUE  pairstruct;

  /* Cast query message */
  par_query_msg_p = (T_RFSFAT_QUERY_PARTITION_REQ_MSG *)msg_p;

  pairstruct.pair_id = par_query_msg_p->pair_id;
  pairstruct.fd		 = -1;
  /* check the mount table */
  if (MPT_TABLE == NULL)
  {
	   return FSCORE_EINTERNALERROR;
  }

  /* check for the valid mount point & partition name with query media type*/
  for(mount_index=0,retVal=FFS_RV_UNAVAILABLE;(mount_index<NR_OF_MOUNTPOINTS) &&(retVal==FFS_RV_UNAVAILABLE);mount_index++)
  	{
  	     if((MPT_TABLE[mount_index].media_type == par_query_msg_p->media_type) &&(MPT_TABLE[mount_index].is_mounted==TRUE))
  	     	{
  	     	     /* copy the partition name */
				 memcpy(par_query_msg_p->buf,MPT_TABLE[mount_index].partition_name,GBI_MAX_PARTITION_NAME);
				 retVal=FFS_NO_ERRORS;
  	     	}

  	}

  
   return rfsfat_send_response_msg( FSCORE_QUERY_PARTITION_RSP,
					                            pairstruct,
					                            &(par_query_msg_p->return_path),
					                            retVal,
					                            FSCORE_EOK);    

}

#endif
