#ifndef __OBXBTSTK_H
#define __OBXBTSTK_H
/****************************************************************************
 *
 * File:          obxbtstk.h
 *
 * Description:   This file contains the definitions and typedefs used
 *                by the OBEX Bluetooth transport module. It is not
 *                used by any other component of the OBEX protocol.
 * 
 * Created:       October 21, 1999
 *
 * Version:       MTObex 3.5
 *
 * Copyright 1999-2006 Extended Systems, Inc.
 * Portions copyright 2006 iAnywhere Solutions, Inc.
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
#include "sys/obstkif.h"
#include "sys/obstack.h"

#if BT_STACK == XA_ENABLED
#include "rfcomm.h"
#include "sdp.h"
#include "me.h"
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
#include "btalloc.h"
#include "sys/sdpi.h"
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

/* Forward type declarations  */
typedef struct _ObBtClientTransport ObBtClientTransport;
typedef struct _ObBtServerTransport ObBtServerTransport;

/*---------------------------------------------------------------------------
 *
 * Functions exported for the Application to manage the client connection.
 */
ObStatus BTSTACK_ClientConnect(ObexBtTarget         *Target,
                               ObBtClientTransport  *btxp,
                               ObexTransport          **Trans);
ObStatus BTSTACK_ClientDisconnect(ObBtClientTransport  *btxp);

/****************************************************************************
 *
 * Prototypes for Required Transport Functions (called from obstack.c)
 *
 ****************************************************************************/

/*
 * These functions are used by the 'obstack.c' Init and Deinit functions.
 * Note that when this stack layer is disabled, macros are defined to 
 * replace these functions. See the end of this file for the macros.
 */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ClientInit() type
 *
 *     Initializes an instance of the OBEX Bluetooth client stack transport
 *     driver.  This function will be called directly by OBSTACK_ClientInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     trans - Bluetooth client transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (BTSTACK_ClientInit)(ObBtClientTransport *btxp);
/* End of BTSTACK_ClientInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ClientDeinit() type
 *
 *     Shuts down an instance of the OBEX Bluetooth client stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ClientDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED.
 *
 * Parameters:
 *     trans - Bluetooth client transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (BTSTACK_ClientDeinit)(ObBtClientTransport *btxp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/* End of BTSTACK_ClientDeinit */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ServerInit() type
 *
 *     Initializes an instance of the OBEX Bluetooth server stack transport
 *     driver.  This function will be called directly by OBSTACK_ServerInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     trans - Bluetooth server transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (BTSTACK_ServerInit)(ObBtServerTransport *btxp);
/* End of BTSTACK_ServerInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ServerDeinit() type
 *
 *     Shuts down an instance of the OBEX Bluetooth server stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ServerDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED
 *
 * Parameters:
 *     trans - Bluetooth server transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (BTSTACK_ServerDeinit)(ObBtServerTransport *btxp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/* End of BTSTACK_ServerDeinit */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Structures used internally by the OBEX Bluetooth Transport
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the common Bluetooth transport 
 * components for the OBEX client/server roles.
 */
typedef struct _ObBtCommonTransport {
    U8              flags;
    ObexConnState   state;

    /* Connection oriented endpoints */
    RfChannel       channel; /* channel parameters */

    /* Token passed to parser to identify transport */
    ObexTransport   transport;

    /* Token passed to parser to identify application */
    ObexAppHandle   *app;

    /* Global Handler */
    BtHandler       globalHandler;

    /* Local Bluetooth device address */
    BD_ADDR         devAddr;           

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    /* How many credits to advance after flow control 
     * has been released.
     */
    U8              credits;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
} ObBtCommonTransport;

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the Stack Layer of the OBEX
 * protocol Component. It encapsulates all RFCOMM and ACL connections.
 */
struct _ObBtClientTransport {

    ObBtCommonTransport client;

    /* Handle to device which the client is connected to */
    BtRemoteDevice  *remoteDevice;
    
    /* Handle to ACL connection client is using */
    BtHandler       aclHandle;
    
    /* Client SDP Query Stuff */
    SdpQueryToken   sdpQueryToken;

    /* RFCOMM Service Channels retrieved by SDP */
    U8              remoteRfcServer[OBEX_RF_SERVICE_CHANNELS];

    /* Index into the RFCOMM Service Channels */
    U8              index;

    /* Number of RFCOMM Channels received by SDP */
    U8              numChannels;

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
    ObSdpResultsCb              sdpCallback;
    SdpQueryToken               sdpCallbackQueryToken;
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */
};
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 *
 * OBEX Server Transport - This Structure manages the Stack Layer of the OBEX
 * protocol Component. It encapsulates all RFCOMM and ACL connections.
 */
struct _ObBtServerTransport {

    ObBtCommonTransport server;

    /* Group: Connection oriented endpoints */
    RfService       serverReg;     /* Server's Advertised channel */

    /* Server SDP Registration Stuff */
    U8              sdpProtoDescList[19];
};
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Transport macros
 *
 ****************************************************************************/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * InitBtClientTransport()
 *
 *     Initializes an ObBtClientTransport structure prior to registration.
 *
 * Parameters:
 *     btxp - The structure to initialize.
 *
 */
void InitBtClientTransport(ObBtClientTransport *btxp);

#define InitBtClientTransport(_BTXP)    \
            OS_MemSet((U8 *)(_BTXP), 0, sizeof(ObBtClientTransport))

#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * InitBtServerTransport()
 *
 *     Initializes an ObBtServerTransport structure prior to registration.
 *
 * Parameters:
 *     btxp - The structure to initialize.
 *
 */
void InitBtServerTransport(ObBtServerTransport *btxp);

#define InitBtServerTransport(_BTXP) \
    OS_MemSet((U8 *)(_BTXP), 0, sizeof(ObBtServerTransport))
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/* Transport Flags */
#define BTF_FLOW        0x01

#else /* BT_STACK == XA_ENABLED */
/*
 * Simplify Multi-Transport Init & Deinit code by providing resolution of
 * the Init & Deinit functions even when the transport is not present. In
 * debug builds, calling a missing transport's Init function will generate
 * an assertion. In non-debug the call will return failure. Deinit functions
 * can be called when the transport is not installed and must return TRUE.
 */
#define BTSTACK_ClientInit(_TRANS)      (Assert(0), OB_STATUS_FAILED)
#define BTSTACK_ServerInit(_TRANS)      (Assert(0), OB_STATUS_FAILED)
#define BTSTACK_ClientDeinit(_TRANS)    (TRUE)
#define BTSTACK_ServerDeinit(_TRANS)    (TRUE)

#endif /* BT_STACK == XA_ENABLED */
#endif /* __OBXBTSTK_H */

