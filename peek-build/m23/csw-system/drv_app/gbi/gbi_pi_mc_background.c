/**
 * @file	gbi_pi_mc_background.c
 *
 * Plugin MMC/SD: handle of background operations.
 *
 * @author	 
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

#include "gbi/gbi_i.h"

/**
 * Component:    Plugin Timed background tasks
 * Sub-function: gbi_plugin_mc_bgt_start
 *
 * When the component is implemented, it must have an entry that can be called at 
 * startup time. The plugins 'Request Handler' component should on receive of the 
 * startup message call that entry.
 *
 * @param		None
 * @return	GBI_OK
 */
T_GBI_RESULT	gbi_plugin_mc_bgt_start(void)
{

	return GBI_OK;
}
	

