/**
 * @file  datalight_handle_message.c
 *
 * DL handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *
 */

#include "typedefs.h"
#include "datalight.h"
#include "datalight_interface.h"
#include "datalight_api.h"





/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param msg_p Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_DL_RESULT dl_handle_message (T_RV_HDR *msg_p)
{
  T_DL_RESULT retval=DL_OK;
 
   switch (msg_p->msg_id)
  {
    	case DL_INIT_REQ_MSG:
	/** Initialization of FFX */
	retval =  dl_op_init_req((T_DL_INIT_REQ_MSG *) msg_p);
	break;
	
	case DL_READ_REQ_MSG:
      	/** Handle read request */
     	retval = dl_op_read_req((T_DL_READ_REQ_MSG *) msg_p);
      	break;

    	case DL_WRITE_REQ_MSG:
     	/** Handle write request */
     	retval = dl_op_write_req((T_DL_WRITE_REQ_MSG *) msg_p);
      	break;

   	case DL_ERASE_REQ_MSG:
      	/** Handle erase group request */
      	retval = dl_op_erase_req((T_DL_ERASE_REQ_MSG *) msg_p);
      	break;

    	default:
/*      DL_SEND_TRACE ("DL CORE attached - unknown message"); */
      	retval = DL_INTERNAL_ERR;
  
  }

   if(rvf_free_buf(msg_p) != RV_OK)
   {
	   DL_SEND_TRACE("FFX unable to free memory",RV_TRACE_LEVEL_WARNING);
	   retval = DL_MEMORY_ERR;
   }
	
    
  return retval;
}

