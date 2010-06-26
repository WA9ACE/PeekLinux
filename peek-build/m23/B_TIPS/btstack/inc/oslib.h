#ifndef __OSLIB_H
#define __OSLIB_H

/****************************************************************************
 *
 * File:
 *     oslib.h
 *
 * Description:
 *     Defines the APIs used by the stack to access system services.
 * 
 * Copyright 2001-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "xatypes.h"


/*---------------------------------------------------------------------------
 * Operating System API layer
 *
 *     The OS API is a set of functions used by the stack to access
 *     system services. If you are porting to a new operating system,
 *     you must provide the functions described here.
 *
 *     The functions in this API are intended for exclusive use by the
 *     stack. Applications should use the native operating system interface
 *     and avoid making OS_ calls themselves.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * OS_Malloc()
 *
 *     Allocate a block of memory.
 *
 * Parameters:
 *     Size - Bytes to allocate.
 *
 * Returns:
 *     Zero -  if allocation failed.
 *     MemBlockPtr - if allocation succeeded.
 */
void *OS_Malloc(U16 Size);


/*---------------------------------------------------------------------------
 * OS_Free()
 *
 *     Deallocates or frees a memory block.
 *
 * Parameters:
 *     MemBlock - Previously allocated memory block to be freed
 */
void OS_Free(void *MemBlock);


#endif /* __OSLIB_H */

