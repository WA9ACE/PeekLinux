#ifndef __L2CAPXP_H
#define __L2CAPXP_H
/***************************************************************************
 *
 * File:
 *     $Workfile:l2capxp.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:30$
 *
 * Description:
 *     Funtion exports for lower layer (ME).
 *
 * Created:
 *     July 15, 1999
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

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus L2CAP_Init()
 *
 * Descriptions:  Initialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - L2CAP is initialized.
 *
 *                BT_STATUS_FAILED - L2CAP initialization failed.
 */
BtStatus L2CAP_Init(void);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_Deinit()
 *
 * Description:   Denitialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void L2CAP_Deinit(void);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_HciReceive()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_HciReceive(const HciBuffer *BuffDesc, HciHandle HciHndl);


/*---------------------------------------------------------------------------
 * Prototype:     BtPacket *L2CAP_HciTxReady()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
BtPacket *L2CAP_HciTxReady(HciHandle HciHndl);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_HciTxDone()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_HciTxDone(BtPacket *Packet, BtStatus Status, HciHandle HciHndl);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_LinkDisconnect()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_LinkDisconnect(BtRemoteDevice* remDev);

#endif

