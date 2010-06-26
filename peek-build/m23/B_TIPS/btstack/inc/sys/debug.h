#ifndef __DEBUG_H
#define __DEBUG_H
/***************************************************************************
 *
 * File:
 *     $Workfile:debug.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:51$
 *
 * Description:
 *     This file contains prototypes for debug message functions.
 *
 * Created:
 *     Aug 13, 1999
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

/*****************************************************************************
 *
 * Debug functions for converting protocol and status types to strings.
 *
 * NOTE: Do not include this file in a header file. It should only be included
 *       in source code files and should be the last file in the include list.
 *
 *****************************************************************************/
#include "me.h"

/*---------------------------------------------------------------------------
 * SniffProtocolId type
 *
 *     This type is used in ChannelInfo to identify the valid protocol id's.
 */
typedef U16 SniffProtocolId;

#define SPI_LM          0x0001  /* LMP Connection Handle */
#define SPI_L2CAP       0x0002  /* L2CAP Protocol */
#define SPI_SDP         0x0004  /* SDP Protocol */
#define SPI_RFCOMM      0x0008  /* RFCOMM Channel protocol */
#define SPI_OBEX        0x0010  /* OBEX Protocol */
#define SPI_TCS         0x0020  /* TCS Binary protocol */
#define SPI_DATA        0x0040  /* Application data */
#define SPI_L2CAP_FEC   0x0080  /* L2CAP Protocol in Flow Control mode */

/* End of SniffProtocolId */

#if XA_SNIFFER == XA_ENABLED && HCI_ALLOW_PRESCAN == XA_ENABLED

/*---------------------------------------------------------------------------
 * SnifferRegisterEndpoint()
 *
 *     This function is called by protocol implementations to register a
 *     new connection in the sniffer connection table. The registering
 *     protocol provides information about the local endpoint, remote
 *     device and the protocol that is running over that endpoint.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     EndpointType - Protocol type of 'Endpoint'. Must be one of SPI_L2CAP
 *         or SPI_RFCOMM.
 *
 *     Endpoint - Identifier for the endpoint. Must be a pointer to an
 *         L2CAP L2capChannelId, or RFCOMM RfChannel->dlci.
 *
 *     RemDev - BtRemoteDevice pointer.
 *
 *     Protocol - Protocol layered above 'Endpoint'.
 */
void SnifferRegisterEndpoint(SniffProtocolId EndpointType, void *Endpoint, 
                             BtRemoteDevice *RemDev, SniffProtocolId Protocol);
#else
#define SnifferRegisterEndpoint(A,B,C,D)   (void)0
#endif

/*
 *---------------------------------------------------------------------------
 *            General Purpose Debug functions
 *---------------------------------------------------------------------------
 */
#if TI_CHANGES == XA_ENABLED
const char *pME_Event(U8 event);

#if (XA_DEBUG == XA_ENABLED) && (XA_DEBUG_PRINT == XA_ENABLED)
const char *pHC_Status(U8 Status);
const char *pBT_Status(I8 Status);
#else 
#define pHC_Status(s) "\0"
#define pBT_Status(s) "\0"
#endif /*  (XA_DEBUG == XA_ENABLED) && (XA_DEBUG_PRINT == XA_ENABLED) */

#else
const char *pBT_Status(I8 Status);
const char *pME_Event(U8 event);
const char *pHC_Status(U8 Status);
#endif /* TI_CHANGES == XA_ENABLED */

const char *pHciEvent(U8 Event);
const char *pHciCommand(U16 Cmd);

#if TI_CHANGES == XA_ENABLED

const char *pSEC_Mode(U8 mode);
const char *pSEC_PairingType(U8 type);

#endif

/*
 *---------------------------------------------------------------------------
 *            L2CAP Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__L2CAP_H)
const char *pL2CAP_Event(I8 Event);
const char *pL2CAP_DiscReason(U16 Reason);
const char *pL2CAP_ConnStatus(U16 Status);
const char *pLLC_Event(I8 Event);
const char *pL2CAP_SignalOpcode(U8 Opcode);
#endif


/*
 *---------------------------------------------------------------------------
 *            HCI Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__HCITRANS_H)
#endif

/*
 *---------------------------------------------------------------------------
 *            RFCOMM Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__RFCOMM_H)
#endif

/*
 *---------------------------------------------------------------------------
 *            SDP Debug functions
 *---------------------------------------------------------------------------
 */
#if TI_CHANGES == XA_ENABLED
const char *pSdpUuid(U32 Service);
const char *pSdpServicesMask(U32 Service);
#else
const char *pSdpService(U32 Service);
#endif
const char *pCodMajorClass(BtClassOfDevice Cod);
U16 BtCod2ServiceString(BtClassOfDevice Cod, char *Result, U16 Max);
/*
 *---------------------------------------------------------------------------
 *            ME Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__ME_H)
#endif

/*
 *---------------------------------------------------------------------------
 *            TCS Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__TCS_H)
const char *pTCS_Event(TcsEvent tcs_event);
const char *pTCS_CallState(TcsCallState State);
const char *pTCS_Message(TcsMsgType msg);
#endif

#endif /* __DEBUG_H */
