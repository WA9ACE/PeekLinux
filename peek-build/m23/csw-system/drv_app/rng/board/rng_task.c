/**
 * @file	rng_task.c
 *
 * TASK for rng SWE.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#include "memif/mem.h"
#include "rng/rng_i.h"

#include "rng/rng_api.h"


/**
 * Get RNG Hardware version
 *
 * Detailled description.
 * This function returns the hardware version of the driver. The version 
 * is coded in hardware register in the RNG.
 * 
 * @return  HW_VERSION
 */
UINT8 get_hw_version(void)
{
  UINT8 hw_version;

  hw_version = *(volatile UINT32 *) RNG_REV; /* Read the hardware version */
	return hw_version;
}

/**
 * RNG busy
 *
 * Detailled description.
 * This function checks if the random number generator is busy genereating a number.
 *
 * @return	the value of the busy bit
 */									
BOOL rng_busy(void)
{
  /* check if random generator is busy */
  return *(volatile UINT32 *) RNG_STAT && RNG_BUSY_BIT_MASK;
}

/**
 * Get random number
 *
 * Detailled description.
 * This function returns a 32 bit random number. The client has to provide
 * an address of a variable where the value will be copied to.
 *
 * @param   rnd_p		pointer to where to copy the value of the random number.
 * @return	RV_OK
 */									
T_RNG_RETURN get_rnd(UINT32 *rnd_p)
{
  *rnd_p = *(volatile UINT32 *) RNG_OUT; /* copy value */
  return RV_OK;
}
