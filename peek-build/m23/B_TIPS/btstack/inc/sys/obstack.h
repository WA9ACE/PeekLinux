#ifndef __OBSTACK_H
#define __OBSTACK_H
/****************************************************************************
 *
 * File:        obstack.h
 *
 * Description: This file specifies the interface between the system 
 *              dependent portion of the OBEX transport and the OBEX packet 
 *              parsers.
 * 
 * Created:     February 10, 1997
 *
 * Version:     MTObex 3.5
 *
 * Copyright 1997-2006 Extended Systems, Inc.
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
#include "obex.h"
#include "sys/obstkif.h"
#include "sys/obprotif.h"
#include "osapi.h"
#include "utils.h"
#if IRDA_STACK == XA_ENABLED
#include "irias.h"
#endif /* IRDA_STACK == XA_ENABLED */

#if !defined(IR_LMP_DISCONNECT)
#error Missing define for IR_LMP_DISCONNECT
#endif /* !defined(IR_LMP_DISCONNECT) */

/*---------------------------------------------------------------------------
 * OBEX System Independent Transport Adapter API layer
 *
 *     Transport adapters are used to adapt the OBEX protocol layer to
 *     various transport layer interfaces. Multi-Transport OBEX enables
 *     multiple transports to exist concurrently. The OBEX
 *     protocol layer accesses the transports through this API.
 *
 *     Obstack.h defines the interface between OBEX and the transport
 *     multiplexer.  This API is used directly by the OBEX Client & Server
 *     protocol components. This separation facilitates isolation of the
 *     OBEX protocol layers from the transport itself.
 */

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
/*--------------------------------------------------------------------------
 *
 * Determines the "safe" amount of remaining SMP block data where it still 
 * makes sense to send an additional OBEX packet. The major concern is 
 * to ensure we can send a complete OBEX packet, since this will determine 
 * whether the remote device sends an OBEX response, which we will need in 
 * order to send our next SMP block.
 *
 * 3 bytes for OBEX command and packet length, 3 bytes for OBEX 
 * header type and length, 1 or more bytes of actual OBEX data.
 */
#define OBEX_SAFE_SMP_REMAINDER    7
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

#if JETTEST == XA_ENABLED

/*---------------------------------------------------------------------------
 * ObexSniffType type
 *
 *     Allows a sniffer to determine whether the data was detected during
 *     tx or rx.
 */
typedef U8 ObexSniffType;

#define OBSNIFF_RX 0
#define OBSNIFF_TX 1

/* End of ObexSniffType */

/*---------------------------------------------------------------------------
 * OB_Sniff() function
 *
 *      A function of this type is stored in ObInternalData, and is called
 *      whenever a data transmission/reception event is detected. 
 *
 * Parameters:
 *      handle - Handle that uniquely identifies the OBEX entity involved
 *
 *      type - Type of data (tx or rx).
 *
 *      buff - Pointer to data. Data in "buff" is valid only during this call.
 *
 *      len - Length of data in "buff".
 */
typedef void (*OB_Sniff)(void *obs, ObexSniffType type,
                         const U8 *buff, U16 len);
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 * ObInternalData structure
 * 
 *     Structure for managing internal Obex data.  This includes:
 *     - the transmit packet pool, which is shared by all clients and 
 *       servers.  
 *     - list entries to manage a list of app handles for each registered 
 *       client and server registered.  
 *     - an IAS query reference to keep track of the transport 
 *       structure used in the IrDA transport.
 */
typedef struct _ObInternalData
{
    BOOL           initialized;
    U8             obexLock;

    /* Group: Obex Packet Pool */

    /* Packet Pool shared by all stack connections */
    ListEntry      packetList;
    ObexPacket     packets[OBEX_NUM_TX_PACKETS];

    /* Group: List entries for app handle lists */
    ListEntry      clientAppHndlList; /* List entry for client app handle list */
    ListEntry      serverAppHndlList; /* List entry for server app handle list */
    /* Group: Current node for app handle lists */
    ListEntry      *curClientNode; /* Current node for client app handle list */
    ListEntry      *curServerNode; /* Current node for server app handle list */

    ObexClientCallbackParms     clientParms;              
    ObexServerCallbackParms     serverParms;              

#if IRDA_STACK == XA_ENABLED
    /* IAS query reference for IrDA transport */
    IrIasQuery     *iasQuery;
#endif /* IRDA_STACK == XA_ENABLED */

#if JETTEST == XA_ENABLED
    /* Callback to receive OBEX transport data events */
    OB_Sniff        sniffer;
#endif /* JETTEST == XA_ENABLED */
} ObInternalData;


/****************************************************************************
 *
 * Section: Generalized Transport Functions
 *
 ****************************************************************************
 *
 *     These are generalized OBEX Transport API functions which are used
 *     to initialize the transports(s), send data, and manage connections.
 *     They either internally call transport-specific versions of the same
 *     functions or are defined as macros which map to transport-specific
 *     functions (except for OBSTACK_GetTxBuffer, which has no equivalent).
 *     For example, OBSTACK_ClientInit() in turn calls BTSTACK_ClientInit.
 *     These OBSTACK_* functions are called by both the application (via
 *     the OBEX API) and/or the OBEX parser.
 *
 ****************************************************************************/

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ClientInit()
 *
 *     Initialize the OBEX Stack Client Component, including the transport
 *     client component. The client is normally dormant after initialization
 *     until a connection request is made.
 *
 * Parameters:
 *     AppHndl - Handle of application "reserving" this instance. This
 *         handle should be used by the transport to
 *         identify which instance is referenced during future calls
 *         to ClientDeinit(), ClientConnect() or ClientDisconnect().
 *
 *     Protocols - Bit mask specifying which transport protocols to 
 *         initialize for the clients use.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus OBSTACK_ClientInit(ObexClientApp *ObexApp, U8 Protocols);


#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ClientDeinit()
 *
 *     Shutdown the OBEX Stack Client Component. Deinitializes the transport
 *     interface and frees any resources. This function can fail if the
 *     transport is busy managing an active connection.
 *
 * Requires:
 *      OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during init. This handle is
 *         used by the transport to identify which
 *         instance to deinitialize.
 * Returns:
 *     TRUE - The stack component was shutdown successfully.
 *
 *     FALSE - The client is busy and cannot be shutdown.
 */
BOOL OBSTACK_ClientDeinit(ObexClientApp *ObexApp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ServerInit()
 *
 *     Initialize the OBEX Stack Server Component, including the transport
 *     server component. The server will normally perform service
 *     registration in this function. The server then sits idle until a
 *     connection is established to it from a remote device.
 *
 * Parameters:
 *     AppHndl - Handle of application "reserving" this instance. This
 *         handle should be used by the transport to
 *         identify which instance is referenced during future calls
 *         to ServerDeinit().
 *
 *     Protocols - Bit mask specifying which transport protocols to 
 *                initialize for the servers use.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus OBSTACK_ServerInit(ObexServerApp *ObexApp, U8 Protocols);


#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ServerDeinit()
 *
 *     Shutdown the OBEX Stack Server Component. Deinitializes the transport
 *     interface and frees any resources. This function can fail if the
 *     transport is busy managing an active connection.
 *
 * Requires:
 *      OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during init. This handle is
 *         used by the transport to identify which
 *         instance to deinitialize.
 * Returns:
 *     TRUE - The stack component was shutdown successfully.
 *
 *     FALSE - The server is busy and cannot be shutdown.
 */
BOOL OBSTACK_ServerDeinit(ObexServerApp *ObexApp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBSTACK_GetTxBuffer()
 *
 *     Return a pointer to a buffer in which to transmit an OBEX packet.
 *     The OBEX packet may be sent in multiple buffers so the buffer does
 *     not have to be big enough to hold a whole OBEX packet. The size of
 *     buffer is returned by OBSTACK_MaxTxSize.
 *
 * Returns:
 *     Pointer to transmit buffer, or 0 if no buffers are available.
 */
U8 *OBSTACK_GetTxBuffer(void);


/*---------------------------------------------------------------------------
 * OBSTACK_SendTxBuffer()
 *
 *     Send the transmit buffer over the transport connection.
 *
 * Parameters:
 *     trans - OBEX client transport to send buffer over.
 *
 *     buff - Pointer to buffer to send.
 *
 *     len - Number of bytes in the buffer.
 *
 * Returns:
 *     OB_STATUS_PENDING - Means operation successful
 *
 *     OB_STATUS_FAILED - Usually an IrUltra error or unexpected link drop.
 *
 *     OB_STATUS_MEDIA_BUSY - Operation failed because the media is busy.
 */
ObStatus OBSTACK_SendTxBuffer(ObexTransport *trans, U8 *buff, U16 len);

#define OBSTACK_SendTxBuffer(_SIF, _BUF, _LEN) \
        (Assert(_SIF),(_SIF)->funcTab->SendTxBuffer((_SIF)->context, (_BUF), (_LEN)))


/*---------------------------------------------------------------------------
 * OBSTACK_IsConnectionUp()
 *
 *     Returns status of specified Transport connection.
 *
 * Parameters:
 *     trans - OBEX client transport used to query connection state.
 *
 * Returns:
 *     TRUE - Connection is up.
 *
 *     FALSE - Connection is not up.
 */
BOOL OBSTACK_IsConnectionUp(ObexTransport *trans);


/*---------------------------------------------------------------------------
 * OBSTACK_Disconnect()
 *
 *     Disconnect the server connection. The disconnect is complete
 *     when the parser's ObDisconnectInd function is called.
 *
 * Parameters:
 *     trans - OBEX client transport used to disconnect connection.
 *
 * Returns:
 *     Status of Disconnect operation.
 */
ObStatus OBSTACK_Disconnect(ObexTransport *trans);

#define OBSTACK_Disconnect(_SIF) \
        (Assert(_SIF),(_SIF)->funcTab->Disconnect((_SIF)->context))


/*---------------------------------------------------------------------------
 * OBSTACK_MaxTxSize()
 *
 *     Return maximum TPDU size that can be sent on the connection.
 *
 * Parameters:
 *     trans - OBEX client transport used to return information.
 *
 * Returns:
 *     Size of transport packet data area.
 */
U16 OBSTACK_MaxTxSize(ObexTransport *trans);

#define OBSTACK_MaxTxSize(_SIF) \
        (Assert(_SIF),(_SIF)->funcTab->MaxTxSize((_SIF)->context))


#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_VerifyClientAppHandle()
 *
 *     Verifies that the passed in AppHandle is actually on the list
 *     of registered client AppHandles.
 *
 * Parameters:
 *     ObexApp - AppHandle to verify.
 *
 * Returns:
 *     Status of AppHandle on the registered client list.
 */
BOOL OBSTACK_VerifyClientAppHandle(ObexAppHandle *ObexApp);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_VerifyServerAppHandle()
 *
 *     Verifies that the passed in AppHandle is actually on the list
 *     of registered server AppHandles.
 *
 * Parameters:
 *     ObexApp - AppHandle to verify.
 *
 * Returns:
 *     Status of AppHandle on the registered client list.
 */
BOOL OBSTACK_VerifyServerAppHandle(ObexAppHandle *ObexApp);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
/*---------------------------------------------------------------------------
 * OBSTACK_GetClientSimpleTransport()
 *
 *     Return a pointer to the IrSimple client OBEX transport.
 *
 * Returns:
 *     Pointer to OBEX transport.
 */
ObexTransport* OBSTACK_GetClientSimpleTransport(ObexClientApp *ObexApp);

/*---------------------------------------------------------------------------
 * OBSTACK_GetSimpleRole()
 *
 *     Retrieves the IrSimple role of the transport instance.
 *
 * Returns:    
 *     IrSimple role
 */
IrSmpRole OBSTACK_GetSimpleRole(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBSTACK_GetSimpleMode()
 *
 *     Retrieves the IrSimple mode of the transport instance.
 *
 * Returns:    
 *     IrSimple mode
 */
IrSmpMode OBSTACK_GetSimpleMode(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBSTACK_IsSimpleRequest()
 *
 *     Queries the transport to determine if a SMP request is
 *     expected to occur.
 *
 * Returns:    
 *     TRUE or FALSE
 */
BOOL OBSTACK_IsSimpleRequest(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBSTACK_IsSimpleResponse()
 *
 *     Queries the transport to determine if a SMP response is
 *     expected to occur.
 *
 * Returns:    
 *     TRUE or FALSE
 */
BOOL OBSTACK_IsSimpleResponse(ObexAppHandle *AppHndl);
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

void OS_LockObex(void);
void OS_UnlockObex(void);
BOOL IsObexLocked(void);

#endif /* __OBSTACK_H */
