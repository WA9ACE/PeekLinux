/**
 * @file	rng_task.h
 *
 * TASK Definition for RNG SWE.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/10/2003	  ()        		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __RNG_TASK_H_
#define __RNG_TASK_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name TASK functions
 *
 * TASK functions declarations (bridge functions).
 */
/*@{*/
UINT8 get_hw_version();
BOOL rng_busy();
T_RNG_RETURN get_rnd(UINT32 *);
/*@}*/


#ifdef __cplusplus
}
#endif


#endif /*__RNG_TASK_H_*/

