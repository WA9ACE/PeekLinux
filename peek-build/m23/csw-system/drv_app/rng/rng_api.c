/**
 * @file	rng_api.c
 *
 * API for RNG SWE.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/10/2003	  ()    		      Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */


#include "rng/rng_i.h"
#include "rng/rng_task.h"

#include "rng/rng_api.h"

/**
 * Get random number
 *
 * Detailled description.
 * This function returns a 32 bit random number. The client has to provide
 * an address of a variable where the value will be copied to.
 *
 * @param   rnd_p		32 bits variable in which to copy the random number.
 * @return	RV_OK or RV_NOT_READY
 */	
T_RV_RET rng_get_rnd(UINT32 *rnd_p)
{
  /* SWE not started */
  if (rng_idle == FALSE)
  {
    return RV_NOT_READY;
  }

  /* Check if parametere is filled */
  if (rnd_p == NULL)
  {
    return RV_INVALID_PARAMETER;
  }
  
  /* Is RNG still initialising */
  if (rng_env_ctrl_blk_p->rng_initialised == FALSE && rng_busy() != FALSE)
  {
    *rnd_p = 0; /* random number is zero */
    return RV_NOT_READY;
  }
  else
  {
    rng_env_ctrl_blk_p->rng_initialised = TRUE;
    /* Is random number generator busy */
    while(rng_busy() != FALSE)
    {
      ; /* WAIT */
    }
    get_rnd(rnd_p); /* Get the number from the RNG */
  }
  return RV_OK;
}

/**
 * Get RNG Hardware version
 *
 * Detailled description.
 * This function returns the hardware version of the driver. The version 
 * is coded in hardware register in the RNG.
 * 
 * @return  HW_VERSION
 */
UINT8 rng_get_hw_version(void)
{
  return get_hw_version();
}

/**
 * Get RNG Software version
 *
 * Detailled description.
 * This function returns the software version of the driver. The version 
 * is hard coded in BCD format within the software.
 * 
 * @return  SW_VERSION
 */
UINT32 rng_get_sw_version(void)
{
  return BUILD_VERSION_NUMBER(RNG_MAJOR,RNG_MINOR,RNG_BUILD);
}

/**
 * Check RNG sleep mode
 *
 * Detailled description.
 * This function checks if the hardware may enter sleep mode.
 * 
 * @return  RV_OK or RV_NOT_READY
 */
BOOL rng_check_sleep(void)
{
  /* Is RNG still initialising */
  if (rng_env_ctrl_blk_p->rng_initialised == FALSE && rng_busy() != FALSE)
  {
    return FALSE;
  }
  else
  {
    rng_env_ctrl_blk_p->rng_initialised = TRUE;
    /* Is random number generator busy */
    while (rng_busy() != FALSE)
    {
      ;/* WAIT */
    }
  }
  return TRUE;
}
