/**
 * @file	die_api.c
 *
 * API for DIE SWE.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	5/30/2003	 ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include "die/die_i.h"
#include "die/die_api.h"
#include "die/die_env.h"


extern T_RV_RET get_die_id (UINT8* id_p);

/**
 * Get DIE id
 *
 * Detailled description.
 * This function returns the DIE id 64-bit value (128 bit for Chipset 15).
 * The client has to provide a buffer via the key parameter in which the 
 * key will be copied. This buffer size shall be at least 8 (16) bytes.
 *
 * @param	id_p		Buffer in which to copy the DIE id.
 * @return	RV_OK or RV_INTERNAL_ERR
 */									
T_RV_RET die_get_die_id (UINT8* id_p)
{
  DIE_SEND_TRACE ("DIE API entering die_get_die_id (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);
  if ( id_p == NULL ) 
  {
    DIE_SEND_TRACE("DIE: Error, invalid pointer ",RV_TRACE_LEVEL_ERROR);
    return RV_INTERNAL_ERR;
  }
  return get_die_id (id_p);
}

/**
 * Get DIE Software version
 *
 * Detailled description.
 * This function returns the software version of the driver. The version 
 * is hard coded in BCD format within the software.
 * 
 * @return  SW_VERSION
 */
UINT32 die_get_sw_version(void)
{
  T_RVM_INFO_SWE swe_info;

  DIE_SEND_TRACE ("DIE API entering die_get_sw_version (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);
  die_get_info (&swe_info);

  return swe_info.type_info.type1.version;
}
