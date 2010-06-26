#ifndef __OPUSH_H
#define __OPUSH_H

/****************************************************************************
 *
 * File:          opush.h
 *
 * Description:   Public types, defines, and prototypes for accessing the 
 *                Bluetooth Object Push Profile program interface.
 * 
 * Created:       May 15, 1997
 *
 * Version:       MTObex 3.5
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
#if BT_STACK == XA_ENABLED
#include "sdp.h"
#endif /* BT_STACK == XA_ENABLED */
#include "obex.h"
#include "goep.h"

/*---------------------------------------------------------------------------
 * Object Push Profile (OPush) Layer 
 *
 *     The OPUSH layer provides two levels of service for applications
 *     implementing the Object Push profile. The basic service provides
 *     functions for registering the OPush SDP entries and for building
 *     OPush SDP queries. These functions can be used directly with the
 *     OBEX API. The expanded services include functions for building
 *     and sending OPush compliant messages.
 */

/*---------------------------------------------------------------------------
 * OPUSH_EXPANDED_API configuration option
 *
 *     This option enables support for the full OPush API.
 */
#ifndef OPUSH_EXPANDED_API
#define OPUSH_EXPANDED_API  XA_DISABLED
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * OPUSH_Init()
 *
 *      Initialize the OPUSH component.  This must be the first OPUSH 
 *      function called by the application layer, or if multiple 
 *      OPUSH applications exist, this function should be called
 *      at system startup (see XA_LOAD_LIST in config.h).  GOEP and 
 *      OBEX must also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL OPUSH_Init(void);

#if BT_STACK == XA_ENABLED 
#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OPUSH_AddServiceRecord()
 *
 *     Registers the Object Push SDP record in the local SDP database.
 *
 * Returns:
 *     BtStatus of the SDP_AddRecord() function call.
 *     
 */
BtStatus OPUSH_AddServiceRecord(GoepServerApp *Server);

/*---------------------------------------------------------------------------
 * OPUSH_RemoveServiceRecord()
 *
 *     Removes the previously registered Object Push SDP record from the
 *     local SDP database
 *
 * Returns:
 *     BtStatus of the SDP_RemoveRecord() function call.
 *     
 */
BtStatus OPUSH_RemoveServiceRecord(GoepServerApp *Server);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OPUSH_BuildServiceQuery()
 *
 *     Builds an Object Push SDP query compatible with the OBEX ObexTpAddr
 *     format for Bluetooth targets.
 *
 * Parameters:
 *     QueryPtr - This pointer receives a pointer to the SDP Query data.
 *     QueryLen - This pointer is set to the length of the Query data.    
 *     QueryType - This pointer is set to the type of SDP query provided.
 *
 */
void OPUSH_BuildServiceQuery(const U8 **QueryPtr, U16 *QueryLen, SdpQueryType *QueryType);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* BT_STACK == XA_ENABLED */

#if OPUSH_EXPANDED_API == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OPUSH_RegisterServer()
 *
 *     Registers the object push server with the GOEP multiplexor. This 
 *     includes registering the SDP records for the Object Push Service.
 *     All of the events specified in the GOEP layer are delivered to the
 *     OPUSH server.
 *
 * Parameters:
 *     Server - The server application's registration structure. The 
 *          'callback' field must be filled in.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Server is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Server.
 *     
 *     OB_STATUS_BUSY - An OPush server is already registered.
 *     
 */
ObStatus OPUSH_RegisterServer(GoepServerApp *Server, const ObStoreFuncTable *obStoreFuncs);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OPUSH_DeregisterServer()
 *
 *     Deregisters the object push server from the GOEP multiplexor.
 *     This includes removing the SDP records for the Object Push Service.
 *     
 * Parameters:
 *     Server - The server used in the registration call.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The server was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The server is not registered.
 *
 *     OB_STATUS_BUSY - The server could not be deregistered because
 *          it is currently processing an operation.
 *
 *     OB_STATUS_FAILED - The OBEX server failed to deinitialize.
 */
ObStatus OPUSH_DeregisterServer(GoepServerApp *Server);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OPUSH_ServerContinue()
 *
 *     This function is called by the server in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The server is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus OPUSH_ServerContinue(GoepServerApp *Server);
#define OPUSH_ServerContinue(_SERVER) GOEP_ServerContinue(_SERVER)

/*---------------------------------------------------------------------------
 * OPUSH_ServerAccept()
 *
 *     This function is called to accept a Push or Pull request.
 *     It MUST be called during the GOEP_EVENT_PROVIDE_OBJECT indication for
 *     these operations. Failure to do so will abort the operation.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *   
 *     Obsh - The handle of an Object store object handle. This handle
 *          is not interpreted by the GOEP server (but it must not be zero).
 *          It is passed to the registered object store functions.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server does not have an operation in progress.
 *
 *     OB_STATUS_SUCCESS - The object was accepted.
 *     
 */
ObStatus OPUSH_ServerAccept(GoepServerApp *Server, void *Obsh);
#define OPUSH_ServerAccept(_SERVER, _OBSH) GOEP_ServerAccept(_SERVER, _OBSH)

/*---------------------------------------------------------------------------
 * OPUSH_ServerAbort()
 *
 *     Aborts the current server operation.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Resp - The response code to send to the client expressing the reason
 *         for the abort.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The abort was successfully requested.
 *
 *     OB_STATUS_FAILED - The abort operation failed.
 *            
 *     OB_STATUS_NO_CONNECT - No transport connection exists.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OPUSH_ServerAbort(GoepServerApp *Server, ObexRespCode Resp);
#define OPUSH_ServerAbort(_SERVER, _RESP) GOEP_ServerAbort(_SERVER, _RESP)
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OPUSH_RegisterClient()
 *
 *     Registers the object push client with the GOEP multiplexor.
 *     All of the events specified in the GOEP layer are delivered to the
 *     OPUSH client, except for GOEP_EVENT_TP_CONNECTED.
 *     
 * Parameters:
 *     Client - The client application's registration structure. The 
 *          'callback' field must be filled in.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Client is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Client.
 *
 *     OB_STATUS_BUSY - An OPush client is already registered.
 *     
 */
ObStatus OPUSH_RegisterClient(GoepClientApp *Client, const ObStoreFuncTable *obStoreFuncs);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OPUSH_DeregisterClient()
 *
 *     Deregisters the object push client from the GOEP multiplexor.
 *     
 * Parameters:
 *     Client - The structure used to register the client.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The client was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The client is not registered.
 *
 *     OB_STATUS_BUSY - The client could not be deregistered because
 *          it is currently sending an operation.
 *
 *     OB_STATUS_FAILED - The OBEX client failed to deinitialize.
 *
 */
ObStatus OPUSH_DeregisterClient(GoepClientApp *Client);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OPUSH_Connect()
 *
 *     This function is used by the client to create a transport connection
 *     to the specified device and issue an OBEX Connect Request.
 *
 * Parameters:
 *     Client - The structure used to register the client.
 *     
 *     Target - This structure describes the server to which the client
 *         wishes to connect.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled via a call to the application callback.
 *
 *         If the connection is successful, a GOEP_EVENT_COMPLETE event for
 *         the GOEP_OPER_CONNECT operation will be signaled.
 *
 *         If the transport connection is successful, but the OBEX Connect
 *         failed, the completion event will be GOEP_EVENT_TP_DISCONNECTED
 *         for the operation GOEP_OPER_CONNECT. At this point the transport
 *         is DISCONNECTED. 
 * 
 *         If the transport connection is unsuccessful, the completion event
 *         will be GOEP_EVENT_TP_DISCONNECTED, GOEP_EVENT_DISCOVERY_FAILED, or
 *         GOEP_EVENT_NO_SERVICE_FOUND.
 *
 *     OB_STATUS_SUCCESS - The client is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the client
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The client is currently executing an operation.
 *
 */
ObStatus OPUSH_Connect(GoepClientApp *Client, ObexTpAddr *Target);

/*---------------------------------------------------------------------------
 * OPUSH_Disconnect()
 *
 *     This function is used by the client to issue an OBEX Disconnect Request.
 *     When the OBEX Disconnect is completed, the transport connection will
 *     be disconnected automatically.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the GOEP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 */
ObStatus OPUSH_Disconnect(GoepClientApp *Client);
#define OPUSH_Disconnect(_CLIENT) GOEP_Disconnect(_CLIENT)

/*---------------------------------------------------------------------------
 * OPUSH_ClientAbort()
 *
 *     Aborts the current client operation. The completion event will signal
 *     the status of the operation in progress, either COMPLETE or ABORTED.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Operation was successful.
 *
 *     OB_STATUS_FAILED - Operation failed to start.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameter.
 */
ObStatus OPUSH_ClientAbort(GoepClientApp *Client);
#define OPUSH_ClientAbort(_CLIENT) GOEP_ClientAbort(_CLIENT)

/*---------------------------------------------------------------------------
 * OPUSH_Push()
 *
 *     Initiates the OBEX "Put" operation to send an object to the remote
 *     server.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Object - Parameters necessary to build a push request. This
 *         parameter must be provided.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_FAILED - Operation was not started because
 *         of an underlying transport problem.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 *     
 */
ObStatus OPUSH_Push(GoepClientApp *Client, GoepObjectReq *Object);
#define OPUSH_Push(_CLIENT, _OBJECT) GOEP_Push(_CLIENT, _OBJECT)

/*---------------------------------------------------------------------------
 * OPUSH_Pull()
 *
 *     Initiates the OBEX "Get" operation to retrieve an object from
 *     the remote server.  
 *
 *     If this operation fails due to a failure in adding queued headers
 *     (GOEP_ClientQueueHeader), it may be useful to set the "More" parameter 
 *     when reissuing this command.  Doing so will allow the GET operation to
 *     send all the headers that will fit in the existing GET request, but will 
 *     also force the GET request to not send the final bit. This allows
 *     subsequent GET request packets to send additional headers prior to a 
 *     GET response sending back the object being described in the GET request 
 *     headers.  Any additional headers that cannot fit in the initial GET 
 *     request will be sent when OPUSH_ClientContinue is called during the 
 *     GOEP_EVENT_CONTINUE.  GOEP_ClientQueueHeader should be called before or 
 *     during this event to ensure they are sent in the next GET request packet.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Object - Parameters necessary to build a pull request. This
 *         parameter must be provided.
 *
 *     More - More headers exist in addition to the queued headers already
 *         added via GOEP_ClientQueueHeader.  
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *     
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *     
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 */
ObStatus OPUSH_Pull(GoepClientApp *Client, GoepObjectReq *object, BOOL more);
#define OPUSH_Pull(_CLIENT, _OBJECT, _MORE) GOEP_Pull(_CLIENT, _OBJECT, _MORE)

/*---------------------------------------------------------------------------
 * OPUSH_ClientContinue()
 *
 *     This function is called by the client in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The client is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus OPUSH_ClientContinue(GoepClientApp *Client);
#define OPUSH_ClientContinue(_CLIENT) GOEP_ClientContinue(_CLIENT)
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* OPUSH_EXPANDED_API == XA_ENABLED */

/****************************************************************************
 *
 * Internal Data Structures
 *
 ****************************************************************************/

#if (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER == XA_ENABLED)
/*---------------------------------------------------------------------------
 * OPushRecords structure
 *
 *     This structure is used internally by the GOEP Server layer.
 */
typedef struct _OPushRecords {
    BOOL                registered;     /* Registration for this OPush instance */
    SdpRecord           record;         /* SDP record */
    SdpAttribute        attributes[6];  /* Service Id, Proto Desc & Formats */
} OPushRecords;
#endif /* (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER == XA_ENABLED) */

/*---------------------------------------------------------------------------
 * OPushExtensionData structure
 *
 *     This structure is used internally by the GOEP Server layer.
 */
typedef struct _OPushExtensionData {
    BOOL                initialized;
#if (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER == XA_ENABLED)
    OPushRecords        sdp[GOEP_NUM_OBEX_CONS];
#endif /* (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER == XA_ENABLED) */
} OPushExtensionData;

/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
#if XA_CONTEXT_PTR == XA_ENABLED
#define OPP(f) (OPush->f)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define OPP(f) (OPush.f)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */ 

#endif /* __OPUSH_H */
