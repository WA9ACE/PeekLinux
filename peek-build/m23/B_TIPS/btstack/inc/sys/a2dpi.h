/****************************************************************************
 *
 * File:
 *     $Workfile:a2dpi.h$ for iAnywhere AV SDK, version 1.4
 *     $Revision:16$
 *
 * Description: This file contains internal definitions for the Advanced Audio
 *     Distribution Profile (A2DP).
 *             
 * Created:     June 3, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __A2DPI_H_
#define __A2DPI_H_

#include "sys/a2alloc.h"

/* SDP Query Flags */
#define A2DP_SDP_QUERY_FLAG_SERVICE   0x01
#define A2DP_SDP_QUERY_FLAG_PROTOCOL  0x02
#define A2DP_SDP_QUERY_FLAG_PROFILE   0x03
#define A2DP_SDP_QUERY_FLAG_FEATURES  0x08

/* Device Flags */                                        
#define A2DP_DEV_FLAG_OUTGOING      0x01
                                      
/* Stream Flags */
#define A2DP_STRM_FLAG_DISCOVERY    0x02
#define A2DP_STRM_FLAG_CLOSE_INT    0x04
#define A2DP_STRM_FLAG_QUERY_CODEC  0x08
#define A2DP_STRM_FLAG_QUERY_CP     0x10
#define A2DP_STRM_FLAG_GET_CAP      0x20
#define A2DP_STRM_FLAG_IDLE         0x40

/* Function Prototypes */
BtStatus A2dpRegisterSdpService(A2dpEndpointType streamType);
BtStatus A2dpDeregisterSdpService(A2dpEndpointType streamType);
BtStatus A2dpStartServiceQuery(A2dpDevice *Device, A2dpEndpointType streamType, 
                               SdpQueryMode mode);
void A2dpAvdtpCallback(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms);
void A2dpFreeStreamInfo(A2dpStream *Stream);
void A2dpStreamClose(A2dpStream *Stream, AvdtpCallbackParms *Parms);
void A2dpClose(A2dpDevice *Device);
void A2dpCmgrCallback(CmgrHandler *cHandler, CmgrEvent Event, BtStatus Status);
BtStatus A2dpStreamSendSbcPacket(A2dpStream *Stream, A2dpSbcPacket *Packet);

#endif /* __A2DPI_H_ */

