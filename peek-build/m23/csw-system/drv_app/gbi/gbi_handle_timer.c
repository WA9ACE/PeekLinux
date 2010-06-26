/**
 * @file	gbi_handle_timer.c
 *
 * GBI handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	12/29/2003	 ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */


#include "gbi/gbi_i.h"
#include "nan/nan_api.h"
#include "chipset.cfg"
/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN gbi_handle_timer (T_RV_HDR * msg_p)
{	
	/*
	 * Got a timer event.
	 */
  T_GBI_NAND_GC_MSG  *gc_msg_p;

  rvf_free_buf (msg_p);
  
#if (CHIPSET != 15)

if(Is_nan_tristated())
      return RVM_OK;
#endif
   
    /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_GBI_NAND_GC_MSG),
                       GBI_NAND_GC_MSG, 
                       (T_RV_HDR **) &gc_msg_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_handle_timer: out of memory",
                    RV_TRACE_LEVEL_ERROR);
    return RV_MEMORY_ERR;
}

  /* compose message */
  gc_msg_p->hdr.msg_id       = GBI_NAND_GC_MSG; 
  
  /* Send message mailbox */
  rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, gc_msg_p);

	return RVM_OK;
}
