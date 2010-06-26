/**
 * @file	hash_handle_timer.c
 *
 * SHA handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */


#include "hash/hash_i.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 *
 * @param	Not used
 * @return	RVM_OK
 */
T_RVM_RETURN hash_handle_timer (T_RV_HDR * msg_p)
{
  /*
   * Got a timer event.
   */


  return RVM_OK;
}
