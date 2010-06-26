/**
 * @file	camd_handle_timer.c
 *
 * CAMD handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author	raymond zandbergen (raymond.zandbergen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen (raymond.zandbergen@ict.nl)		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include "camd/camd_i.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN
camd_handle_timer (T_RV_HDR * msg_p)
{
  /*
   * Got a timer event.
   */


  return RVM_OK;
}
