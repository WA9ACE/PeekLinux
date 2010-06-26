/**
 * @file  mpk_api.h
 *
 * API Definition for MPK SWE.
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

#ifndef __MPK_API_H_
#define __MPK_API_H_


#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/* Manufacturer Public Key SIZE is 16 bytes long */
#define MPK_SIZE        16

/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

T_RV_RET mpk_get_mpk_id (UINT8* id_p);
UINT32 mpk_get_sw_version(void);

/*@}*/

#ifdef __cplusplus
}
#endif


#endif /*__MPK_API_H_*/

