/**
 * @file	die_hw_functions.c
 *
 * Functions for DIE SWE.
 *
 * @author	ICT Embedded (dennis.gallas@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	5/20/2003	ICT Embedded (dennis.gallas@ict.nl)		Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

/*
 * Note: This module will be build for target environment.
 */

#include "die/die_i.h"

#include "die/die_api.h"


/**
 * Get DIE id
 *
 * Detailled description.
 * This function returns the DIE id 64-bit value (128 bit for Chipset 15).
 * The client has to provide a buffer via the key parameter in which the 
 * key will be copied. This buffer size shall be at least 8 (16) bytes.
 *
 * @param	id_p		Buffer in which to copy the DIE id.
 * @return	RV_OK
 */									
T_RV_RET get_die_id (UINT8* id_p)
{
  /*
   * DIE ID will be retrieved from DIE register
   */
  #if (CHIPSET==15)
    volatile UINT8 *reg_p0 = (UINT8 *) DIE_ID_REG;
    volatile UINT8 *reg_p1 = (UINT8 *) DIE_ID_REG1;
	UINT8 *id0 = &id_p[0];
	UINT8 *id1 = &id_p[DIE_ID_SIZE0]; 

	BE_STREAM_TO_ARRAY(id0, reg_p0, DIE_ID_SIZE0);
	BE_STREAM_TO_ARRAY(id1, reg_p1, DIE_ID_SIZE1);
  #else
    volatile UINT8 *reg_p = (UINT8 *) DIE_ID_REG;
 

	BE_STREAM_TO_ARRAY(id_p, reg_p, DIE_ID_SIZE);
  #endif
  return RV_OK;
}
