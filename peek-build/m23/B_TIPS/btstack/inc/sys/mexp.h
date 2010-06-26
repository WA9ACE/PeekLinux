#ifndef __MEXP_H
#define __MEXP_H
/***************************************************************************
 *
 * File:
 *     $Workfile:mexp.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:25$
 *
 * Description:
 *     This file contains functions exported to the Event Manager by the
 *     Management Entity.
 *
 * Created:
 *     July 20, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
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

#include "bttypes.h"

#if (TI_CHANGES == XA_ENABLED)

#include "me.h"

#endif

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus ME_Init(void)
 *
 * Descriptions:  Initialize the Management Entity. 
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - ME initialized successfully.
 *
 *                BT_STATUS_FAILED - ME failed to initialize.
 */
BOOL ME_Init(void);

/*---------------------------------------------------------------------------
 * Prototype:     void ME_Deinit(void)
 *---------------------------------------------------------------------------
 *
 * Description:   Deinitialize the Management Entity. 
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void ME_Deinit(void);


#if (TI_CHANGES == XA_ENABLED)

/*---------------------------------------------------------------------------
 * ME_RegisterGlobalHandler()
 *
 *     Register a callback handler with the Management Entity to
 *     receive global events. The protocol stack sends a number
 *     of global events to all registered handlers (who have the
 *     proper event mask set). Clients must register a handler 
 *     in order to receive events. The list of global events for 
 *     which the callback will be called are listed in the 
 *     Callback section below. The event mask for the handler is 
 *     initialized to BEM_NO_EVENTS.
 *
 * Parameters:
 *     handler - pointer to a BtHandler structure. The callback
 *         field of the handler must point to a valid callback function.
 *         The handler must be initialized using ME_InitHandler.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the handle is
 *         already registered, the handle is not initialized properly.
 */
BtStatus ME_IntRegisterGlobalHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_UnregisterGlobalHandler()
 *
 *     Unregister a global handler. Upon successful completion of this 
 *     function the handler is initialized and can be registered again 
 *     without the need for calling ME_InitHandler.
 *
 * Parameters:
 *     handler - pointer to handler to unregister
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
BtStatus ME_IntUnregisterGlobalHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * ME_SetEventMask()
 *
 *     Set the event mask for the global handler. The event mask 
 *     is for global events.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 *     mask - event mask.
 *
 * Returns
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operations failed because handler
 *         was not registered or 0 (error check only).
 */
#if XA_ERROR_CHECK == XA_ENABLED
BtStatus ME_IntSetEventMask(BtHandler *handler, BtEventMask mask);
#else /* XA_ERROR_CHECK */
#define ME_SetEventMask(h,m) ((h)->eMask = (m),BT_STATUS_SUCCESS)
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * ME_GetEventMask()
 *
 *     Get the event mask for the global handler. The event mask
 *     is for global events. This function does not check to
 *     see if the handler is valid.
 *
 * Parameters:
 *     handler - pointer to handler.
 *
 * Returns:
 *     The event mask 
 */
BtEventMask ME_IntGetEventMask(BtHandler *handler);
#define ME_IntGetEventMask(h) ((h)->eMask)

#endif /* TI_CHANGES == XA_DISABLED */


#endif

