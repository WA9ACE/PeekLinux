/**
 * @file	die_api.h
 *
 * API Definition for DIE SWE.
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

#ifndef __DIE_API_H_
#define __DIE_API_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "chipset.cfg"


#ifdef __cplusplus
extern "C"
{
#endif


/* DIE ID SIZE is 8 bytes long */
#if (CHIPSET==15)
#define DIE_ID_SIZE        16
#define DIE_ID_SIZE0       8
#define DIE_ID_SIZE1       8
#else
#define DIE_ID_SIZE        8
#endif

/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

T_RV_RET die_get_die_id (UINT8* id_p);
UINT32   die_get_sw_version(void);

/*@}*/

#ifdef __cplusplus
}
#endif


#endif /*__DIE_API_H_*/

