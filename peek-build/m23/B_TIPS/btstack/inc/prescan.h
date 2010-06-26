#ifndef __PRESCAN_H
#define __PRESCAN_H

/***************************************************************************
 *
 * File:
 *     $Workfile:prescan.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:21$
 *
 * Description:
 *     This file contains code that provides a place to create Prescan
 *     functions.
 *
 * Created:
 *     September 27, 2000
 *
 * Copyright 2000-2005 Extended Systems, Inc.
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

/*---------------------------------------------------------------------------
 * Prescan Module layer
 *
 *     Bluetooth technology is still somewhat in its infancy, and the 
 *     specification is still in a small state of flux.  The iAnywhere 
 *     Blue SDK is kept up to date with the very latest spec, however, it
 *     is required to work with a large number of different radios.  Because
 *     things are changing, and because interpretations in the operation
 *     of the HCI exist, the Blue SDK provides a mechanism for providing
 *     compatibility with radios that vary from the latest specification.
 *
 *     This mechanism is provided through something called a Prescan Module.
 *     Prescan modules are capable of hooking some low level HCI routines
 *     and creating a behavior that is consistent to the expectations of the
 *     upper layers of the stack (ME in particular).    
 *
 *     Most radio modules follow the HCI specification pretty well, with
 *     only a small amount of variation from the spec.  These differences
 *     are easily handled in the ME.  When a radio has a behavior that the
 *     ME cannot handle, then a Prescan Module may be necessary.
 *
 *     This file contains functions exported by Prescan Modules provided
 *     by iAnywhere Solutions.  Each section contains an API and a description
 *     of it's use.
 */

/****************************************************************************
 *
 * Add module-specific entries here, including data structures and 
 * initialization prototypes.
 *
 ****************************************************************************/

#include "sys/hci.h"
#include "sys/evmxp.h"
#include "eventmgr.h"

#if HCI_ALLOW_PRESCAN == XA_ENABLED

/****************************************************************************
 *
 *  Constants
 *
 ****************************************************************************/
 
/****************************************************************************
 *
 *  Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PrescanContext structure
 *
 *     This structure contains the global context for all prescan modules
 *     provided by iAnywhere Solutions.  The context for a particular module
 *     is conditionally included or excluded.
 */
typedef struct _PrescanContext {
    ListEntry       prescanList;    /* List of prescan modules */

    /* Module specific data */

} PrescanContext;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


#endif /* HCI_ALLOW_PRESCAN == XA_ENABLED */

#endif /* __PRESCAN_H */
