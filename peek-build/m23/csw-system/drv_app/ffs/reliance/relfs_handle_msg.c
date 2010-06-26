/**
 * @file  Relfs_handle_msg.c
 *
 * reliance handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author   ()
 * @version 0.1
 */

#include "relfs_env.h"
#include "relfs.h"
#include "relfs_pei.h"
#include "reliance_rtos.h"
#include "rfs/fscore_types.h"
#include "rfs/rfs_message.h"
#include "gbi/gbi_api.h"




static T_FSCORE_CMD_ID get_rfs_command_id(UINT16 op_id);
static T_FSCORE_RET relfs_convert_ffs_result (int ffs_status, T_FSCORE_RET result);
static T_FSCORE_RET relfs_send_setpart_response_msg_to_rfs (T_FSCORE_PARTITION_TABLE * part_table_p,T_FSCORE_RET result);
static T_FSCORE_RET relfs_send_response_msg_to_rfs (T_FSCORE_CMD_ID cmdId, T_FSCORE_PAIR_VALUE pair_value,  int internal_status, T_FSCORE_RET result);
static int relfs_fsStat_rfs_ffs(T_RFS_STAT *stat_p);

extern void FfxIdleCompactHook(void);

extern unsigned IQ_FrameCount;    /* Used to check if Frame IT TPU*/
int ril_handle_message(T_Rel_req*  msg_p)
{
    int retval = REL_OK;
	int  os_error = REL_OK;
    unsigned short  uAttr;
    int status;
    T_FFS_FILE_CNF *confirm_file = NULL;
    T_FFS_STREAM_CNF *confirm_stream = NULL;
    T_FFS_FD fd;
    char *temp_path;
    T_FFS_REQ_ID temp_id;
	const char * ascii_path =NULL;


    T_Rel_req *req_msg = msg_p;;

    if((msg_p->unicode) && (req_msg->pathname!= NULL) )
    	{
    	   /* Needs a conversion */
		   if(wstrlen((const T_WCHAR *) req_msg->pathname) < MAX_ASCII_ARRAY_SIZE)
		   	{
                 convert_unicode_to_u8((const T_WCHAR*) req_msg->pathname, ril_env_ctrl_blk_p->temp_buf1);
				 ascii_path = ril_env_ctrl_blk_p->temp_buf1;
		   	}
		   else
		   	{
		   	    retval = EFFS_INVALID;  /* Invalid parameter */
				goto error_end;
		   	}
		   	
    	}
	else
		{
		   ascii_path = req_msg->pathname;
		}

	fd = req_msg->fd;

    switch(req_msg->msg)
    {
        case REL_START_GC_REQ:
            {
				RELFS_TRACE_DEBUG_HIGH("RELFS:GC");
                FfxIdleCompactHook();
				ril_env_ctrl_blk_p->Garbage_Collection_Process=FALSE;
                break;

            }    


/* File operations */
	
        case REL_OPEN_NB_REQ:
            {
                retval=ffs_open(ascii_path, req_msg->option);
                break;
            }	
 
        case REL_MKDIR_NB_REQ:
            {
                retval=ffs_mkdir(ascii_path);
                break;
            }
          
        case REL_LINK_NB_REQ:
            {
                retval = ffs_symlink(req_msg->actualpath,ascii_path);
                break;
            }

		case REL_PREFORMAT_NB_REQ:
			 	{
					  if (req_msg->magic!= 0xDEAD)
					  	{
					 	retval = EFFS_INVALID;
					  	}
					  else
					  	{
					  	 retval = REL_OK;
					  	}
					 break;
					}

		
        case REL_FORMAT_NB_REQ:
            {
			
                retval=ffs_format(ascii_path,req_msg->magic);
			
                break;
            }
		
        case REL_REMOVE_NB_REQ:
		case REL_RMDIR_NB_REQ:	
            {
                retval=ffs_remove(ascii_path);
                break;		
            }

		
        case REL_RENAME_NB_REQ:
            {
				const char *ascii_path_1=NULL;			 

				if((msg_p->unicode) && (req_msg->actualpath!= NULL) )
    			{
    	   			/* Needs a conversion */
		   			if(wstrlen((const T_WCHAR *) req_msg->actualpath) < MAX_ASCII_ARRAY_SIZE)
		   			{
                 		convert_unicode_to_u8((const T_WCHAR*) req_msg->actualpath, ril_env_ctrl_blk_p->temp_buf2);
				 		ascii_path_1 = ril_env_ctrl_blk_p->temp_buf2;
					}
		   			else
		   			{
		   	   		 	retval = EFFS_INVALID;  /* Invalid parameter */
					 	goto error_end;
		   			}
		   	
    			}
				else
				{
		   			ascii_path_1 = req_msg->actualpath;
				}			
				retval = ffs_rename(ascii_path_1,ascii_path);
            	break;
            }

		case REL_OPENDIR_NB_REQ:
            {		
                				
                retval = ffs_opendir(ascii_path,(T_FFS_DIR *)(req_msg->ptr_data));
                break;
            }


		case REL_STAT_NB_REQ:
            {
			
			    retval = ffs_stat(ascii_path, (T_FFS_STAT*)(req_msg->ptr_data));
				  
                break;
            }


/* File operations, RFS specific */
		
		case REL_OPENDIR_RFS_NB_REQ:
            {		
				T_FFS_DIR dir_s;
                T_FSCORE_DIR *rfs_dir_p; 
                				
                retval = ffs_opendir(ascii_path,&dir_s);

						rfs_dir_p= (T_FSCORE_DIR *)(req_msg->ptr_data);
						rfs_dir_p->opendir_ino  = dir_s.this;
						rfs_dir_p->lastread_ino = dir_s.index;
						rfs_dir_p->mpt_idx = 0;
                break;
            }

   	  case REL_STAT_RFS_NB_REQ:
            {
				T_FFS_STAT stat;
				T_RFS_STAT  *rfs_stat_p= req_msg->ptr_data;

				if(rfs_stat_p == NULL)
					{
					  retval =EFFS_INVALID;
					}
				else
					{
                     if( (ascii_path == NULL) || (ascii_path[0] == '\0') || ((ascii_path[0] == '/') && (ascii_path[1] == '\0')))
                     	{

						 status = relfs_fsStat_rfs_ffs(rfs_stat_p);

						 if(status != REL_OK)
						 	{
						 	  retval = ffs_map_ril_error(retval);
						 	}
						 else
						 	{
						 	  retval = EFFS_OK;
						 	}
                     	}
					 else
					 	{
					 
						memset(&stat,0,sizeof(T_FFS_STAT));
			            retval = ffs_stat(ascii_path, &stat);
						rfs_stat_p->file_dir.ino =stat.inode;
						rfs_stat_p->file_dir.size = stat.size;
						rfs_stat_p->file_dir.ctime = 0;
						rfs_stat_p->file_dir.mtime = 0;
						rfs_stat_p->file_dir.reserved = 0;
						rfs_stat_p->file_dir.mode = FSCORE_IWUSR | FSCORE_IRUSR;

						if(stat.type == OT_DIR)
							{
								rfs_stat_p->file_dir.mode |=FSCORE_IXUSR ;							  
							}
					 	}
						
					}
				  
                break;
            }


	  case REL_GET_ATTRIB_RFS_REQ:
           	{ 
                 unsigned short attr;
				 int status;

				  status=relFs_Get_Attributes(ascii_path,&attr);
				  if(status != REL_OK)
				  	{
				  	  retval = ffs_map_ril_error(status);
				  	}
				  else
				  	{
				  	  retval = relfs_map_attr_rfs(attr);
				  	}
				
				break;
	  	}


    case REL_SET_ATTRIB_RFS_REQ:
           	{ 
				 int status;

				 status=relFs_Set_Attributes(ascii_path,relfs_map_attr_rfs(req_msg->uMode));
				 if(status != REL_OK)
				  	{
				  	  retval = ffs_map_ril_error(status);
				  	}
				  else
				  	{
				  	  retval = EFFS_OK;
				  	}
				
				break;
	  	}


/* Stream operations */

        case REL_WRITE_NB_REQ:
            {					
                if (ascii_path != NULL)
                {
                    retval= ffs_file_write(ascii_path,req_msg->pBuf,req_msg->size, req_msg->option);
                }
                else
                {
                    retval=ffs_write(req_msg->fd, req_msg->pBuf,req_msg->size);
                }
                break;					
            }		

	    case REL_READ_NB_REQ:
            {					
                if (ascii_path != NULL)
                {
                    retval= ffs_file_read(ascii_path,req_msg->pBuf,req_msg->size);
                }
                else
                {
                    retval=ffs_read(req_msg->fd, req_msg->pBuf,req_msg->size);
                }
                break;					
            }	

		 case REL_TRUNC_NB_REQ:
            {
                if (ascii_path != NULL)
                {
                    retval=ffs_truncate(ascii_path, req_msg->length);
                }
                else
                {
                    retval=ffs_ftruncate(req_msg->fd,req_msg->length);
                }

                break;
            }
		
        case REL_SEEK_NB_REQ:
            {
                retval=ffs_seek(req_msg->fd, req_msg->offset,req_msg->origin);
                break;
            }


       case REL_CLOSE_NB_REQ:
            {

                retval=ffs_close(req_msg->fd);
                break;
            }	


		case REL_READDIR_NB_REQ:
            {

				   if(req_msg->unicode)
				   	{
				   	  unsigned short size;
				   	  if(req_msg->size > MAX_ASCII_ARRAY_SIZE)
				   	  	{
				   	  	    size = MAX_ASCII_ARRAY_SIZE;
				   	  	}
					  else
					  	{
					  	    size = req_msg->size;
					  	}

					   ril_env_ctrl_blk_p->temp_buf2[0]='\0';
					   ril_env_ctrl_blk_p->temp_buf2[1]='\0';
					   ril_env_ctrl_blk_p->temp_buf2[size]='\0';
					  	
				   	   retval = ffs_readdir((T_FFS_DIR*)req_msg->ptr_data, ril_env_ctrl_blk_p->temp_buf2,size);

					    /* convert to unicode */	
						convert_u8_to_unicode_length((const char *)(ril_env_ctrl_blk_p->temp_buf2), (T_WCHAR *)(req_msg->pBuf),retval);
                	}
				   else
				   	{
               			retval = ffs_readdir((T_FFS_DIR*)req_msg->ptr_data, req_msg->pBuf,req_msg->size);
				   	}
		            break;
            }
		
		
		
		case REL_CLOSEDIR_NB_REQ:
            {
                retval = ffs_closedir((T_FFS_DIR*)req_msg->ptr_data);
                break;
            }

	     case REL_FLUSH_NB_REQ:
            {

                retval=ffs_fdatasync(req_msg->fd);
                break;
            }	

		 case REL_FSTAT_NB_REQ:
		 		{
					 retval = ffs_fstat(req_msg->fd,  (T_FFS_STAT *)(req_msg->ptr_data));
					 break;
		 	}


/* stream functions , RFS specific */
		 case REL_FSTAT_RFS_NB_REQ:
            {
				T_FFS_STAT stat;
				T_RFS_STAT  *rfs_stat_p= req_msg->ptr_data;

				if(rfs_stat_p == NULL)
					{
					  retval =EFFS_INVALID;
					}
				else
					{
						memset(&stat,0,sizeof(T_FFS_STAT));
			            retval = ffs_fstat(req_msg->fd, &stat);
						rfs_stat_p->file_dir.ino =stat.inode;
						rfs_stat_p->file_dir.size = stat.size;
						rfs_stat_p->file_dir.ctime = 0;
						rfs_stat_p->file_dir.mtime = 0;
						rfs_stat_p->file_dir.reserved = 0;
						rfs_stat_p->file_dir.mode = FSCORE_IWUSR | FSCORE_IRUSR;

						if(stat.type == OT_DIR)
							{
								rfs_stat_p->file_dir.mode |=FSCORE_IXUSR ;							  
							}
						
						
					}
				  
                break;
            }


		case REL_READDIR_RFS_NB_REQ:
            {

				T_FFS_DIR dir_s;
               	T_FSCORE_DIR *rfs_dir_p = NULL; 
			    rfs_dir_p= (T_FSCORE_DIR*)(req_msg->ptr_data);


				if(rfs_dir_p == NULL)
					{
					   retval = EFFS_INVALID;
					   goto error_end;
					}

				dir_s.this = rfs_dir_p->opendir_ino;
				dir_s.index = rfs_dir_p->lastread_ino;
			
                if(req_msg->unicode)
                	{
                	 
						unsigned short size;

						if(req_msg->size > MAX_ASCII_ARRAY_SIZE)
				   	  	{
				   	  	    size = MAX_ASCII_ARRAY_SIZE;
				   	  	}
					  	else
					  	{
					  	    size = req_msg->size;
					  	}

						ril_env_ctrl_blk_p->temp_buf2[0]= '\0';
						ril_env_ctrl_blk_p->temp_buf2[1]= '\0';
						ril_env_ctrl_blk_p->temp_buf2[size]='\0';
						retval = ffs_readdir(&dir_s, ril_env_ctrl_blk_p->temp_buf2,size);

						if(retval > 0)
							{
									/* convert to unicode */	
								convert_u8_to_unicode_length((const char *)(ril_env_ctrl_blk_p->temp_buf2), (T_WCHAR *)(req_msg->pBuf),retval);
							}		
                	}
				else
					{
                		retval = ffs_readdir(&dir_s, req_msg->pBuf,req_msg->size);
					}	
                break;
            }

		case REL_CLOSEDIR_RFS_NB_REQ:					
	     	{
				T_FFS_DIR dir_s;
               	T_FSCORE_DIR *rfs_dir_p = NULL; 
			    rfs_dir_p= (T_FSCORE_DIR*)(req_msg->ptr_data);


				if(rfs_dir_p == NULL)
					{
					   retval = EFFS_INVALID;
					   goto error_end;
					}

				dir_s.this = rfs_dir_p->opendir_ino;
				dir_s.index = rfs_dir_p->lastread_ino;
				retval = ffs_closedir(&dir_s);
				break;
			}	


/* Set partition Table from RFS */
       case REL_SETPART_RFS_NB_REQ:
	   		{
				T_FSCORE_PARTITION_TABLE * part_table_p= ((T_FSCORE_PARTITION_TABLE *)req_msg->ptr_data);
				
				
                if(part_table_p == NULL)
                	{
                	  retval = EFFS_INVALID;
                	}
				else
					{
					  unsigned long uStatus;
					  
						/* Set the File system Number */
						ril_env_ctrl_blk_p->fscore_nmb = part_table_p->fscore_numb;
						part_table_p->nbr_partitions = 0;
						if(part_table_p->partition_info_table == NULL)
							{
                        		RELFS_GETBUF(sizeof(T_FSCORE_PARTITION_INFO), part_table_p->partition_info_table);
							}	

						if(part_table_p->partition_info_table == NULL)
							{
							    retval = FSCORE_EMEMORY;
							}
						else
							{
							  part_table_p->nbr_partitions = 1;
							  uStatus = FlashFXDeviceIoctl(0,FLASHFX_IOCTL_MEDIA_PARAMS,  (&(ril_env_ctrl_blk_p->rel_sDiskMediaParams)));
							  uStatus = FlashFXDeviceIoctl(0,FLASHFX_IOCTL_PARTITION_PARAMS, (&(ril_env_ctrl_blk_p->rel_sDiskPartitionParams)));
                              part_table_p->partition_info_table[0].filesys_type = GBI_RELIANCE;
							  part_table_p->partition_info_table[0].blk_size = ril_env_ctrl_blk_p->rel_sDiskMediaParams.ulSectorSize;
							  part_table_p->partition_info_table[0].is_mounted = TRUE;
							  part_table_p->partition_info_table[0].media_nbr = 0;
							  part_table_p->partition_info_table[0].media_type = GBI_INTERNAL_NOR;
							  part_table_p->partition_info_table[0].partition_nbr=0;
							  part_table_p->partition_info_table[0].partition_size = ril_env_ctrl_blk_p->rel_sDiskPartitionParams.ulSectorCount;
							  strncpy((char*)part_table_p->partition_info_table[0].partition_name,RFS_MPT_NAME_FFS,GBI_MAX_PARTITION_NAME);
							  strncpy((char*)part_table_p->partition_info_table[0].gbi_part_name,RFS_MPT_NAME_FFS,GBI_MAX_PARTITION_NAME);	
							}
						
						retval =FSCORE_EOK;
                        relfs_send_setpart_response_msg_to_rfs(part_table_p, retval);
                         
						goto function_end;
					}	

				
				break;				
       	}
		
		 

        default:
            {
                retval =  REL_ERR;
            }

    }



error_end:

    if (req_msg->cp) { 
        T_RV_RETURN mycp;

		
        // We reuse the mail we received for our call-back. Due to
        // this reuse, we must be careful with the order of
        // assignments. If this is a stream modify function use
        // ffs_stream_cnf else use ffs_file_cnf  

        // Save cp and id before we reuse the mail mem.
        if(req_msg->cp != NULL)
        	{
		        memcpy(&mycp, req_msg->cp, sizeof(mycp));
        	}
		else
			{
			    mycp.addr_id = 0;
				mycp.callback_func = NULL;
			}

		
        temp_id = req_msg->req_id;  

        switch (req_msg->msg) {
	        case REL_WRITE_NB_REQ: 
			case REL_READ_NB_REQ:				
    	    case REL_SEEK_NB_REQ: 
        	case REL_CLOSE_NB_REQ: 
        	case REL_TRUNC_NB_REQ:
        	case REL_FLUSH_NB_REQ:
				
			case REL_READDIR_NB_REQ:
			case REL_READDIR_RFS_NB_REQ:	
				
			case REL_CLOSEDIR_NB_REQ:	
			case REL_CLOSEDIR_RFS_NB_REQ:					
				
			case REL_FSTAT_NB_REQ:	
			case REL_FSTAT_RFS_NB_REQ:

				RELFS_MSG_GETBUF(sizeof(T_FFS_STREAM_CNF),confirm_stream);

				if(confirm_stream == NULL)
					{
					//  while(1)
					//  	{
				      //	   ;   /* debug */
			      	//	  	}
					  return REL_MEMORY_ERR;
					}
					

                confirm_stream->error = retval;
                confirm_stream->fdi = fd; 
			    confirm_stream->request_id = temp_id;				
				confirm_stream->op_id = req_msg->msg;
                confirm_stream->header.msg_id = FFS_MESSAGE_OFFSET;

                if (mycp.callback_func) {
                    mycp.callback_func((void *) confirm_stream);
                }
                else if (mycp.addr_id) {
                    os_error = OS_MAIL_SEND(mycp.addr_id, confirm_stream);
                }
                else {

              			/* should not reach here */
						   	
                }
                break;
            
            
            default:
                temp_path = (char *) ascii_path;

				RELFS_MSG_GETBUF(sizeof(T_FFS_FILE_CNF),confirm_file);

				if(confirm_file == NULL)
					{
					  // while(1)
					  //	{
					  //       ;   /* debug */
					 // 	} 
					  return REL_MEMORY_ERR;
					}

				confirm_file->error = retval;
                confirm_file->request_id = temp_id;
                confirm_file->path = temp_path;
				confirm_file->op_id = req_msg->msg;
                confirm_file->header.msg_id = FFS_MESSAGE_OFFSET;

                if (mycp.callback_func) {
                    mycp.callback_func((void *) confirm_file);
                }   

                else if (mycp.addr_id) {
                    os_error = OS_MAIL_SEND(mycp.addr_id, confirm_file);
                }

                else {
					         /* should not reach here */
					       						   	
					 	}
                  				               
                break;
        }

    }


function_end:
 os_error = OS_FREE(req_msg);



    return REL_OK;
}




void relfs_rfs_stream_clbk_func(void * result)
{
 T_FSCORE_PAIR_VALUE pair_value;
 T_FSCORE_CMD_ID cmdId;
 T_FSCORE_RET rtval;
 T_FFS_STREAM_CNF * result_p = (T_FFS_STREAM_CNF *)(result);

	
 pair_value.fd = result_p->fdi;
 pair_value.pair_id = result_p->request_id;
 cmdId = get_rfs_command_id(result_p->op_id);
 

 rtval = relfs_send_response_msg_to_rfs(cmdId, pair_value, result_p->error, result_p->error);


  /* his function should not fail */
  while(rtval != FSCORE_EOK)
  	{
  	 ;
  	}

RELFS_FREE_BUF(result_p);
	
 return; 
}



void relfs_rfs_file_clbk_func(void * result)
{
 T_FSCORE_PAIR_VALUE pair_value;
 T_FSCORE_CMD_ID cmdId;
 T_FSCORE_RET rtval;
 T_FFS_FILE_CNF * result_p = (T_FFS_FILE_CNF*)(result);

	
 pair_value.fd = 0;
 pair_value.pair_id = result_p->request_id;
 cmdId = get_rfs_command_id(result_p->op_id);
 

 rtval = relfs_send_response_msg_to_rfs(cmdId, pair_value, result_p->error, result_p->error);


  /* his function should not fail */
  while(rtval != FSCORE_EOK)
  	{
  	 ;
  	}

RELFS_FREE_BUF(result_p);
	
 return; 
}





static T_FSCORE_CMD_ID get_rfs_command_id(UINT16 op_id)
{
   switch (op_id)
   	{


/* Stream Functions */
   	 case REL_WRITE_NB_REQ: return FSCORE_WRITE_RSP;
	 	 		
	 case REL_READ_NB_REQ: return FSCORE_READ_RSP;
				
     case REL_SEEK_NB_REQ: return FSCORE_LSEEK_RSP;

     case REL_CLOSE_NB_REQ: return FSCORE_CLOSE_RSP;
				
				
     case REL_FLUSH_NB_REQ:  return FSCORE_FSYNC_RSP;

				
	 case REL_READDIR_NB_REQ: 
	 case REL_READDIR_RFS_NB_REQ:    /* RFS specific */
	 	return FSCORE_READDIR_RSP;

				
	 case REL_CLOSEDIR_NB_REQ: 
	 case REL_CLOSEDIR_RFS_NB_REQ:
	 	return FSCORE_CLOSEDIR_RSP;	

				
	 case REL_FSTAT_NB_REQ:	
	 case REL_FSTAT_RFS_NB_REQ:   /* RFS specific */
	 	return FSCORE_FSTAT_RSP;



/* Non stream functions */	
   	case REL_OPEN_NB_REQ:  return FSCORE_OPEN_RSP;
		
	case REL_MKDIR_NB_REQ: return FSCORE_MKDIR_RSP;

		
	case REL_REMOVE_NB_REQ: return FSCORE_REMOVE_RSP;
	case REL_RMDIR_NB_REQ: return FSCORE_RMDIR_RSP;

		
	case REL_TRUNC_NB_REQ: return FSCORE_TRUNC_RSP;


	case REL_TRUNC_NAME_NB_REQ:  return FSCORE_TRUNC_NAME_RSP;
		 
		
	case REL_FORMAT_NB_REQ: return FSCORE_FORMAT_RSP;

	case REL_PREFORMAT_NB_REQ: return FSCORE_PREFORMAT_RSP;

		
	case REL_RENAME_NB_REQ: return FSCORE_RENAME_RSP;

		
	case REL_OPENDIR_NB_REQ: 
	case REL_OPENDIR_RFS_NB_REQ:
		return FSCORE_OPENDIR_RSP;

		
	case REL_STAT_NB_REQ:
	case REL_STAT_RFS_NB_REQ:      
		return FSCORE_STAT_RSP;


    case REL_GET_ATTRIB_RFS_REQ:    return FSCORE_GETATTRIB_RSP;

    case REL_SET_ATTRIB_RFS_REQ:   return  FSCORE_SETATTRIB_RSP;

	case REL_GET_DATETIME_RFS_REQ: return FSCORE_GETDATETIME_RSP;	

   
	default:
		   return FSCORE_INVALID_RSP;

   	}
  
}


/**
* This function converts the FFS internal status into a FS-core specified return value.
* 
*  @param   ffs_status      Input:  internal filesystem status
*
*  @return  fscore_result  
*/
static T_FSCORE_RET relfs_convert_ffs_result (int ffs_status, T_FSCORE_RET result)
{

 if(ffs_status > 0)
 	{
 	   /* consider this as success */
	   return result;
 	}


  switch (ffs_status)
	{
	case EFFS_OK:
	  /* ffs_status doesn't contain any errors, so return result */
	  return result;

	case EFFS_NODEVICE:
	  return FSCORE_ENODEVICE;

	case EFFS_CORRUPTED:
	case EFFS_NOPREFORMAT:	
	case EFFS_NOFORMAT:	
	case EFFS_BADFORMAT:	
	case EFFS_MAGIC:	
    case EFFS_DRIVER:
	  return FSCORE_EDEVICE;
	 
	case EFFS_AGAIN:
	  return FSCORE_EAGAIN;

    case EFFS_NOSYS:
    return FSCORE_ENOTSUPPORTED;

	case EFFS_NOSPACE:
	  return FSCORE_ENOSPACE;

    case EFFS_FSFULL:
		 return FSCORE_EFSFULL;
		 
	case EFFS_BADNAME:
         return FSCORE_EBADNAME; 

    case EFFS_NOTFOUND:
		  return  FSCORE_ENOENT;

	case EFFS_EXISTS:
	  return FSCORE_EEXISTS;

	case EFFS_ACCESS:
	  return FSCORE_EACCES;
  
     case EFFS_NAMETOOLONG:
	 case EFFS_INVALID:	
	 case EFFS_PATHTOODEEP:	
	 	return FSCORE_EINVALID; 

     case EFFS_DIRNOTEMPTY:
		return FSCORE_ENOTEMPTY;

     case EFFS_NOTADIR:
		return FSCORE_ENOTADIR;	 	  

      case EFFS_SPARE:
		return FSCORE_EINTERNALERROR;

     case EFFS_FILETOOBIG:
	 case EFFS_TOOBIG:	
	 	return FSCORE_EFILETOOBIG;

	 case EFFS_NOTAFILE:
	  return FSCORE_ENOTAFILE;

	 case EFFS_NUMFD:
      return FSCORE_ENUMFD;

     case EFFS_BADFD:
		return FSCORE_EBADFD;

     case EFFS_BADOP:
        return FSCORE_EBADOP;	 	

	 case EFFS_LOCKED:
	   return FSCORE_ELOCKED;	
		
 
	default:
		RELFS_TRACE_DEBUG_HIGH("Unknown FFS error, convert to FS-core result is not possible");


	  return FSCORE_EINTERNALERROR;
	}
}



static T_FSCORE_RET relfs_send_setpart_response_msg_to_rfs (T_FSCORE_PARTITION_TABLE * part_table_p,T_FSCORE_RET result)
{
   T_FSCORE_SET_PARTITION_TABLE_RSP_MSG* msg_p;
  if(part_table_p == NULL)   return FSCORE_EINVALID;

  
 /* reserve response message buffer */
  if (rvf_get_msg_buf (ril_env_ctrl_blk_p->mbid,
					   sizeof (T_FSCORE_READY_RSP_MSG),
					   FSCORE_SET_PARTITION_TABLE_RSP_MSG, 
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message */
	  RELFS_TRACE_DEBUG_HIGH ("RFSFAT fsfat ready msg: out of memory");
	  return FSCORE_EMEMORY;
	}

  msg_p->hdr.msg_id= FSCORE_SET_PARTITION_TABLE_RSP_MSG;
  msg_p->result = result;
  msg_p->partition_table = part_table_p;

  
  if (rfs_send_message((T_RV_HDR *)msg_p) != RVF_OK)
	{
	  RELFS_TRACE_DEBUG_HIGH("RFS rfsfat_send_response_to_client: could not send response msg");

	  return FSCORE_EMEMORY;
	}

  return FSCORE_EOK;

 
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
static T_FSCORE_RET relfs_send_response_msg_to_rfs (T_FSCORE_CMD_ID cmdId,
						  T_FSCORE_PAIR_VALUE pair_value,
						  int internal_status, T_FSCORE_RET result)
{
  T_FSCORE_READY_RSP_MSG *msg_p;
  T_FSCORE_RET fscore_result;
  T_RV_RET rv_retval = RV_OK;

  /* Convert FFS error code to FSCORE error code */
  fscore_result = relfs_convert_ffs_result (internal_status, result);

  /* reserve response message buffer */
  if (rvf_get_msg_buf (ril_env_ctrl_blk_p->mbid,
					   sizeof (T_FSCORE_READY_RSP_MSG),
					   FSCORE_READY_RSP_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message */
	  RELFS_TRACE_DEBUG_HIGH ("RFSFAT fsfat ready msg: out of memory");

	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = FSCORE_READY_RSP_MSG;
   msg_p->fscore_nmb = ril_env_ctrl_blk_p->fscore_nmb;   /* File system Number */
  msg_p->command_id = cmdId;
  msg_p->pair_value = pair_value;
  msg_p->result = fscore_result;
 
  if (rfs_send_message((T_RV_HDR *)msg_p) != RVF_OK)
	{
	  RELFS_TRACE_DEBUG_HIGH ("RFS rfsfat_send_response_to_client: could not send response msg");

	  return FSCORE_EMEMORY;
	}

  return FSCORE_EOK;
}



static int relfs_fsStat_rfs_ffs(T_RFS_STAT *stat_p)
{
  char szDrive[3];
  int fd, rtval;
  REL_STATFS sStatFs;
  
  
	szDrive[0] = relFs_Get_Default_Drive()+'A';
    szDrive[1] = ':';
    szDrive[2] = 0;

    /*  get the drive info
    */
    memset(&sStatFs, 0, sizeof(REL_STATFS));
    fd = relFs_Open(szDrive, REL_O_RDONLY, REL_IREAD);
    if(fd < REL_SUCCESS)
    {
    RELFS_TRACE_DEBUG_HIGH ("RFS relfs_fsStat_rfs_ffs: unable to open the drive");
        return fd;
    }
    else
    {
        rtval = relFs_StatFs(fd, &sStatFs);
        relFs_Close(fd);
    }

  if(rtval == REL_SUCCESS)
  	{
       /* convert rfs type */
	   stat_p->mount_point.blk_size = sStatFs.ulBlockSize;
	   stat_p->mount_point.nr_blks  = sStatFs.ulNumBlocks;
	   stat_p->mount_point.mpt_size = (sStatFs.ulBlockSize*sStatFs.ulNumBlocks)/1024;
	   stat_p->mount_point.mpt_id = sStatFs.uDrive;
	   stat_p->mount_point.free_space = (sStatFs.ulFreeBlocks*sStatFs.ulBlockSize)/1024;
	   stat_p->mount_point.used_size = (sStatFs.ulUsedBlocks*sStatFs.ulBlockSize)/1024;
	   stat_p->mount_point.reserved_1 = GBI_INTERNAL_NOR;
	   stat_p->mount_point.reserved_2 = GBI_RELIANCE;
	   strcpy(stat_p->mount_point.fs_type, RFS_RELIANCE_FS_NAME);
	   strcpy(stat_p->mount_point.media_type, "NOR\0");
	   
  	}
  else
  	{
  	RELFS_TRACE_DEBUG_HIGH ("RFS relfs_fsStat_rfs_ffs: relFs_StatFs failed"); 
  	}

  return rtval;
}





