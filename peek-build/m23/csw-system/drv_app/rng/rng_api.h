/**
 * @file	rng_api.h
 *
 * API Definition for RNG SWE.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/10/2003	()        Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __RNG_API_H_
#define __RNG_API_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name RNG typedefs and defines.
 *
 * Currently they are the standard RV return types.
 * 
 * The memory definitions and other RNG defines
 */
/*@{*/
typedef T_RV_RET T_RNG_RETURN;
/*@}*/

/**
 * @name API functions
 *
 * API functions declarations.
 */
/*@{*/

T_RV_RET rng_get_rnd(UINT32 *);
UINT8 rng_get_hw_version(void);
UINT32 rng_get_sw_version(void);
BOOL rng_check_sleep(void);

/*@}*/


#ifdef __cplusplus
}
#endif


#endif /*__RNG_API_H_*/

