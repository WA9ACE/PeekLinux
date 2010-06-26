#ifndef __OBSTKIF_H
#define __OBSTKIF_H
/****************************************************************************
 *
 * File:        obstkif.h
 *
 * Description: This file specifies the interface provided by the system 
 *              dependent portion of the OBEX transport.
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
#include "utils.h"

/*---------------------------------------------------------------------------
 * OBEX Transport Adapter API layer
 *
 *     This layer defines calls made by the OBEX layer to control a
 *     specific transport adapter. Transport adapters implement this API
 *     by filling a table of functions (ObTransFuncTable) and providing that
 *     table to the OBEX layer in an ObexTransport structure as documented
 *     here.
 *
 *     The OBEX layer uses this API to send data through a transport,
 *     handle flow control, initiate disconnections, and get other
 *     information from the transport.
 *
 *     Each transport adapter must implement this API.
 */

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObexConnState type
 *
 *     OBEX Transport Connection state. Not all connection states
 *     apply to all transports. It is recommended that all transports
 *     base their connection state on these defines.
 */
typedef U8 ObexConnState;

/* Transport is uninitialized. Mandatory initialization of all OBEX RAM to
 * zero sets the state to this value.
 */
#define OCS_UNINITIALIZED    0

/* No transport connection is active or in progress.  This state also 
 * indicates that transport initialization is complete. Deinitialization 
 * will not be allowed until this state is achieved.
 */
#define OCS_IDLE             1

/* IrDA Only: When discovery is performed as part of connect procedure. */
#define OCS_DISCOVERY        2

/* Link layer connection exists or has been initiated. IrDA (IrLAP) or 
 * Bluetooth (ACL).
 */
#define OCS_LINK_CONNECT     3

/* Performing service query. IrDA (IAS) or Bluetooth (SDP). */
#define OCS_SERVICE_QUERY    4

/* Establishing a connection to an OBEX server. IrDA (TinyTP) or
 * Bluetooth (RFCOMM).
 */
#define OCS_CONNECTING       5

/* Transport connection to an OBEX server is up. IrDA (TinyTP) or
 * Bluetooth (RFCOMM).
 */
#define OCS_CONNECTED        6

/* Disconnecting the transport connection. */
#define OCS_DISCONNECTING    7

/* Server waiting for a client connection */
#define OCS_LISTENING        8

/* Additional states can be defined starting with this value. */
#define OCS_OPEN             9

/* End of ObexConnState */

/*---------------------------------------------------------------------------
 * ObexTransportContext type
 *
 *     Context handle provided by the OBEX Protocol layer when calling into 
 *     the transport. It can point to anything the stack
 *     interface driver needs. Its value is transparent to the OBEX protocol.
 */
typedef void *ObexTransportContext;
/* End of ObexTransportContext */


/****************************************************************************
 *
 * Section: Transport Adapter Exports
 *
 *     Functions of these types are called by the OBEX Transport Mux to
 *     perform functions such as initialization and deinitialization of the
 *     client and server components, disconnection of the transport connection,
 *     and flow control pause and resume. They are called either directly
 *     or through the ObTransFuncTable.
 *
 *     The actual function code exists in transport-specific .c files
 *     (e.g., obxbtstk.c, obxirstk.c, etc.).
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * OBTRANS_Disconnect() type
 * 
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by the OBEX protocol to disconnect
 *     the transport connection.
 *
 *     This function is only used internally (by the OBEX protocol). It
 *     should always disconnect the link without performing any of the 
 *     safety checks that are common in User API functions. Such checks
 *     include; checking for a bound client, for an operation in progress,
 *     etc. This function can be used to disconnect both client and server
 *     transport connections.
 *
 *     The transport is required to notify the protocol when the disconnect
 *     is complete by calling the OBPROT_Disconnected() function.
 *
 *     Currently the return status must be set to OB_STATUS_PENDING.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified connection has been disconnected.
 *
 *     OB_STATUS_PENDING - The specified connection is in the process of
 *         being disconnected. Completion must be signaled by a call to
 *         OBPROT_Disconnected().
 *
 *     OB_STATUS_xxxxxxx - The specified connection could not be disconnected.
 */
typedef ObStatus (*OBTRANS_Disconnect)(ObexTransportContext TiContext);
/* End of OBTRANS_Disconnect */

/*---------------------------------------------------------------------------
 * OBTRANS_MaxTxSize() type
 *
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by the OBEX protocol layer to
 *     determine how much data can be presented in an OBTRANS_SendTxBuffer call.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 * Returns:
 *     The maximum size buffer that can be passed to OBTRANS_SendTxBuffer().
 */
typedef U16 (*OBTRANS_MaxTxSize)(ObexTransportContext TiContext);
/* End of OBTRANS_MaxTxSize */

/*---------------------------------------------------------------------------
 * OBTRANS_SendTxBuffer() type
 *
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by the OBEX protocol layer to send
 *     data packets over the transport connection. 
 *
 *     The transmit buffer provided can be converted to an ObexPacket
 *     by using the ContainingRecord() function. It is the responsibility
 *     of this function to return the buffer to the OBEX Protocol layer,
 *     regardless of the return value. The buffer is returned by calling
 *     OBPROT_TxDone() when the transmit has completed successfully.
 *     Or by calling OBPROT_ReturnTxBuffer() in the case of a failure.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 *     TxBuffer - Pointer to data to transmit.
 *
 *     TxLen - Length of data.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The buffer has been transmitted.
 *     OB_STATUS_PENDING - Buffer transmit has started successfully. 
 *     OB_STATUS_xxxxxxx - The transmit has failed.
 */
typedef ObStatus (*OBTRANS_SendTxBuffer)(ObexTransportContext TiContext,
                                       U8 *TxBuffer, U16 TxLen );
/* End of OBTRANS_SendTxBuffer */

/*---------------------------------------------------------------------------
 * OBTRANS_GetTpConnInfo() type
 *
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by OBEX applications to get the
 *     minimum amount of application storage space that can be available
 *     before the application should request flow control on the link.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 *     TpConnInfo - Pointer to the structure which receives the
 *         connection information.
 *
 * Returns: 
 *     TRUE - The structure was successfully completed.
 *
 *     FALSE - The transport is not connected.
 */
typedef BOOL (*OBTRANS_GetTpConnInfo)(ObexTransportContext TiContext, 
                                      ObexTpConnInfo  *TpConnInfo);
/* End of OBTRANS_GetTpConnInfo */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBTRANS_IsRxFlowAvail() type
 *
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by OBEX applications to check if
 *     receive data flow control is supported by the transport.
 *
 * Requires: OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 * Returns:
 *     TRUE - The transport instance (and connection) support the
 *         receive flow control API.
 *
 *     FALSE - The transport instance (or connection) do not support
 *         receive flow control.
 */
typedef BOOL (*OBTRANS_IsRxFlowAvail)(ObexTransportContext TiContext);
/* End of OBTRANS_IsRxFlowAvail */


/*---------------------------------------------------------------------------
 * OBTRANS_PauseRxFlow() type
 *
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by OBEX applications to tell the
 *     transport layer to stop the remote device from sending data by refusing
 *     to advance transport credit. Once this function has been called, no
 *     more than the number of bytes or packets reported by OBEX_GetTpConnInfo()
 *     will be received. This amount includes any currently indicated data 
 *     (if called during a data indication). Flow can be resumed with
 *     OBEX_ResumeRxFlow().
 *
 * Requires: OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 *     role       - Role of the current device (Client/Server).
 */
typedef void (*OBTRANS_PauseRxFlow)(ObexTransportContext TiContext);
/* End of OBTRANS_PauseRxFlow */

/*---------------------------------------------------------------------------
 * OBTRANS_ResumeRxFlow() type
 *
 *     A function of this type is exported by OBEX transport adapter
 *     in the ObTransFuncTable. It is used by OBEX applications to tell the
 *     transport layer to resume data flow from the remote device by
 *     advancing transport credit. Once this function has been called, data
 *     will resume flowing until flow control is exerted again.
 *
 * Requires: OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED.
 *
 * Parameters:
 *     TiContext - transport instance context provided to the
 *         OBEX Protocol layer during transport linking.
 *
 *     role       - Role of the current device (Client/Server).
 */
typedef void (*OBTRANS_ResumeRxFlow)(ObexTransportContext TiContext);
/* End of OBTRANS_ResumeRxFlow */

#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * ObTransFuncTable structure
 *
 *     Table of Transport Adapter specific functions. This table is
 *     passed up to the OBEX protocol layer in the ObexTransport structure
 *     when the transport links to the protocol. It can be allocated in ROM.
 */

typedef struct _ObTransFuncTable
{
    OBTRANS_Disconnect        Disconnect;
    OBTRANS_MaxTxSize         MaxTxSize;
    OBTRANS_SendTxBuffer      SendTxBuffer;

    /* This function is used in conjunction with the RxFlow functions but
     * also exists on its own, even when the RxFlow functions are not 
     * supported.
     */
    OBTRANS_GetTpConnInfo     GetTpConnInfo;

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    /* Group: If the Transport supports credit based flow control
     * (or similar). The ability to manage receive flow should be provided to
     * the application via the following OBSTK RxFlow functions. These 3
     * functions are optional, but if any of the functions are supported, they
     * must all be supported. If RxFlow is not supported, they must be zero.
     */
    OBTRANS_IsRxFlowAvail     IsRxFlowAvail;
    OBTRANS_PauseRxFlow       PauseRxFlow;
    OBTRANS_ResumeRxFlow      ResumeRxFlow;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
} ObTransFuncTable;


/*---------------------------------------------------------------------------
 * ObexTransport structure
 *              
 *     Handle between the OBEX protocol and the transport to
 *     address a particular connection when the OBEX protocol is calling
 *     into the transport. This structure is allocated and
 *     initialized by the transport, then registered with the OBEX
 *     protocol layer. OBEX Servers register their ObexTransport via a call to
 *     OBPROT_LinkTransport() when a connection is established. Clients
 *     are registered when they return their ObexTransport during the
 *     Xyz_ClientConnect() function call.
 */

typedef struct _ObexTransport
{
    const ObTransFuncTable  *funcTab;
    ObexTransportContext     context;
    ObexConnState           *connected;
    ObexTpType               tpType;
} ObexTransport;

#endif /* __OBSTKIF_H */
