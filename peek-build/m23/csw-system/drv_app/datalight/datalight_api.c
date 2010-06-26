/**
 * @file	datalight_api.c
 *
 * API for DL .
 *
 * @author	
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *
 * 
 */
 

#include "typedefs.h"
#include "datalight.h"
#include "datalight_api.h"



   

/**
 * Read
 *
 * Detailled description.
 * This function reads data from a NAND-card.  
 *
 */
T_DL_RESULT dl_read (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p,  
		     T_RV_RETURN    return_path)
{
  T_DL_READ_REQ_MSG *msg_p;
  T_DL_RESULT retval;
#ifdef DL_DBG_TRACE
	DL_SEND_TRACE("<DBG>Inside dlread\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */


  /* DL task ready & initialised? */
  if (dl_env_ctrl_blk_p == NULL || dl_env_ctrl_blk_p->initialised == FALSE)
  {
    DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING);
    return DL_NOT_READY;
  }

  if(dl_env_ctrl_blk_p->disk_initialised[disk_num] == FALSE)
  	{
    	DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING);
     	return DL_NOT_READY;
  	
  	}

  if(rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id, sizeof(T_DL_READ_REQ_MSG), DL_READ_REQ_MSG, (T_RV_HDR **)&msg_p) == RVF_RED)
  {
    DL_SEND_TRACE ("DL dl_read: out of memory",RV_TRACE_LEVEL_WARNING); 
    return DL_MEMORY_ERR;
  }

  /* compose message */
  msg_p->os_hdr.msg_id = DL_READ_REQ_MSG;
  msg_p->disk_num = disk_num;
  msg_p->startsector = start_sector;
  msg_p->numsector = number_of_sectors;
  msg_p->data_p = data_p;
  msg_p->rp = return_path;
  
  /* Send message mailbox */
#if 0
  retval = rvf_send_msg(dl_env_ctrl_blk_p->addr_id, msg_p);
#else
  retval = dl_handle_message ((T_RV_HDR *)msg_p);
#endif
  return retval;
}

/**
 * Write
 *
 * Detailled description.
 * This function writes data to a NAND-card. 
 */
T_DL_RESULT dl_write (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p,  
								T_RV_RETURN    return_path)
{
  T_DL_WRITE_REQ_MSG *msg_p;
  T_DL_RESULT retval;


  /* DL task ready & initialised? */
  if (dl_env_ctrl_blk_p == NULL || dl_env_ctrl_blk_p->initialised == FALSE)
  {
   DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING);  
    return DL_NOT_READY;
  }


  if(dl_env_ctrl_blk_p->disk_initialised[disk_num] == FALSE)
  	{
    	DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING);
     	return DL_NOT_READY;
  	
  	}


  if(rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id, sizeof(T_DL_WRITE_REQ_MSG),
             DL_WRITE_REQ_MSG, (T_RV_HDR **)&msg_p) == RVF_RED)
  {
    DL_SEND_TRACE ("DL dl_write: out of memory",RV_TRACE_LEVEL_WARNING); 
    return DL_MEMORY_ERR;
  }

  /* compose message */
  msg_p->os_hdr.msg_id = DL_WRITE_REQ_MSG;
  msg_p->disk_num = disk_num;  
  msg_p->startsector = start_sector;
  msg_p->numsector = number_of_sectors;
  msg_p->data_p = data_p;
  msg_p->rp = return_path;
  
  /* Send message mailbox */
#ifdef DL_DBG_TRACE  
  DL_SEND_TRACE("<DB>Sending MSG to  Datalight msg hndlr\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */
  retval = rvf_send_msg(dl_env_ctrl_blk_p->addr_id, msg_p);
  return retval;
}

/**
 * Erase
 *
 * Detailled description.
 * This function erases a range of erase groups on the card.
 */
T_DL_RESULT dl_erase (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors, T_RV_RETURN return_path)
{
  T_DL_ERASE_REQ_MSG *msg_p;
  T_DL_RESULT retval;

//  DL_SEND_TRACE ("DL API entering dl_erase (bridge)"); 

  /* DL task ready & initialised? */
  if (dl_env_ctrl_blk_p == NULL || dl_env_ctrl_blk_p->initialised == FALSE)
  {
    DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING); 
    return DL_NOT_READY;
  }


  if(dl_env_ctrl_blk_p->disk_initialised[disk_num] == FALSE)
  	{
    	DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING);
     	return DL_NOT_READY;
  	
  	}


  if(rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id, sizeof(T_DL_ERASE_REQ_MSG),
             DL_ERASE_REQ_MSG, (T_RV_HDR **)&msg_p) == RVF_RED)
  {
    DL_SEND_TRACE ("DL dl_erase: out of memory",RV_TRACE_LEVEL_WARNING); 
    return DL_MEMORY_ERR;
  }

  /* compose message */
  msg_p->os_hdr.msg_id = DL_ERASE_REQ_MSG;
    msg_p->disk_num = disk_num;
  msg_p->startsector = start_sector;
  msg_p->numsector = number_of_sectors;
  msg_p->rp = return_path;

 /* Send message mailbox */
 
  retval = rvf_send_msg(dl_env_ctrl_blk_p->addr_id, msg_p);
  return retval;
}


 T_DL_RESULT dl_init ( T_RV_RETURN   return_path)
 {
 
 T_DL_INIT_REQ_MSG *msg_p;
 
  T_DL_RESULT retval;

   /* DL task ready & initialised? */
  if (dl_env_ctrl_blk_p == NULL)
  {
    DL_SEND_TRACE ("DL API: not ready",RV_TRACE_LEVEL_WARNING); 
    return DL_NOT_READY;
  }

  if(rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id, sizeof(T_DL_INIT_REQ_MSG),
             DL_INIT_REQ_MSG, (T_RV_HDR **)&msg_p) == RVF_RED)
  {
    DL_SEND_TRACE ("DL dl_erase: out of memory",RV_TRACE_LEVEL_WARNING); 
    return DL_MEMORY_ERR;
  }

 /* compose message */
  msg_p->os_hdr.msg_id = DL_INIT_REQ_MSG;
  msg_p->rp = return_path;

  /* Send message mailbox */
  retval = rvf_send_msg(dl_env_ctrl_blk_p->addr_id, msg_p);
  return retval;
}
 
