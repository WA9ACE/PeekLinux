/***************************************************************************
 *
 * File:
 *     $Workfile:btrom.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:27$
 *
 * Description:
 *     This file contains external references to global constant data
 *     for the stack.
 *
 * Created:
 *     March 6, 2000
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
#include "sdp.h"

/* CRC table for calculating FCS for RFCOMM frames */
extern const U8 crctable[256];

#define SDP_ZERO_SERV_CLASS_ID_SIZE 5
extern const U8 ZeroServClassIdVal[SDP_ZERO_SERV_CLASS_ID_SIZE];

#define SDP_ZERO_VER_NUMBER_SIZE    8
extern const U8 ZeroVersionNumberList[SDP_ZERO_VER_NUMBER_SIZE];

/* Bluetooth base UUID for SDP */
extern const U8 BT_BASE_UUID[16];

/* Table for new version of CRC-CCITT generator used by L2CAP.*/
extern const U16 L2CrcTab[16];

