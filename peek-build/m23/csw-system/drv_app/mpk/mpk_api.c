/**
 * @file  mpk_api.c
 *
 * API for MPK SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  5/30/2003 Dennis Gallas ()    Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include "mpk/mpk_i.h"

#include "mpk/mpk_api.h"
#include "mpk/mpk_env.h"


extern T_RV_RET get_mpk_id (UINT8* key_p);

/**
 * Get MPK
 *
 * Detailled description.
 * This function returns the Manufacturer Public Key id 128-bit value. 
 * The client has to provide a buffer via the key parameter in which 
 * the key will be copied. This buffer size shall be at least 16 bytes.
 * @param   id_p  Pointer to client buffer of at least 16 bytes.
 * @return  RV_OK or RV_INTERNAL_ERR
 */                 
T_RV_RET mpk_get_mpk_id (UINT8* id_p)
{
  T_RV_RET retval;
  MPK_SEND_TRACE ("MPK API entering mpk_get_mpk_id (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);
  if ( id_p == NULL ) 
  {
    retval = RV_INTERNAL_ERR;
    MPK_SEND_TRACE("MPK: Error, invalid pointer ",RV_TRACE_LEVEL_ERROR);
    return retval;
  }

  /* Call get MPK function */
  retval = get_mpk_id (id_p);

  return retval;
}

/**
 * Get MPK Software version
 *
 * Detailled description.
 * This function returns the software version of the driver. The version 
 * is hard coded in BCD format within the software.
 * 
 * @return  SW_VERSION
 */
UINT32 mpk_get_sw_version(void)
{
  T_RVM_INFO_SWE swe_info;

  MPK_SEND_TRACE ("MPK API entering mpk_get_sw_version (non-bridge)",
                  RV_TRACE_LEVEL_DEBUG_HIGH);
  mpk_get_info (&swe_info);
  return swe_info.type_info.type1.version;
}
