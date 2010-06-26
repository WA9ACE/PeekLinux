#ifndef __FTP_H
#define __FTP_H

/****************************************************************************
 *
 * File:          ftp.h
 *
 * Description:   Public types, defines, and prototypes for accessing the 
 *                Bluetooth GOEP File Transfer Profile API extensions.
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
 * File Transfer Profile (Ftp) Layer 
 *
 *     The FTP layer provides two levels of service for applications
 *     implementing the File Transfer profile. The basic service provides
 *     functions for registering the FTP SDP entries and for building
 *     FTP SDP queries. These functions can be used directly with the
 *     OBEX API. The expanded services include functions which expand the
 *     basic GOEP API to include Ftp specific functionality.
 */

/*---------------------------------------------------------------------------
 * FTP_EXPANDED_API configuration option
 *
 *     This option toggles support for the full File Transfer API.
 */
#ifndef FTP_EXPANDED_API
#define FTP_EXPANDED_API  XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * FOLDER_LISTING_OBJECT_TYPE constant
 *
 *     The FOLDER_LISTING_OBJECT_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting a folder listing
 *     object.
 */
#define FOLDER_LISTING_OBJECT_TYPE      "x-obex/folder-listing"

#if FTP_EXPANDED_API == XA_ENABLED

#if OBEX_SERVER_CONS_SIZE == 0
#error "OBEX_SERVER_CONS_SIZE Must be greater than zero."
#endif
#endif /* FTP_EXPANDED_API == XA_ENABLED */
 
/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * FTP_Init()
 *
 *     Initialize the FTP component.  This must be the first FTP 
 *     function called by the application layer, or if multiple 
 *     FTP applications exist, this function should be called
 *     at system startup (see XA_LOAD_LIST in config.h).  GOEP 
 *     and OBEX must also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL FTP_Init(void);

#if BT_STACK == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_AddFtpServiceRecord()
 *
 *     Registers the File Transfer SDP record in the local SDP database.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     BtStatus of the SDP_AddRecord() function call.
 *     
 */
BtStatus FTP_AddServiceRecord(GoepServerApp *Server);

/*---------------------------------------------------------------------------
 * FTP_RemoveFtpServiceRecord()
 *
 *     Removes the previously registered File Transfer SDP record from the
 *     local SDP database
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     BtStatus of the SDP_RemoveRecord() function call.
 *     
 */
BtStatus FTP_RemoveServiceRecord(GoepServerApp *Server);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_BuildServiceQuery()
 *
 *     Builds a File Transfer SDP query compatible with the OBEX ObexTpAddr
 *     format for Bluetooth targets.
 *
 * Parameters:
 *     QueryPtr - This pointer receives a pointer to the SDP Query data.
 *     QueryLen - This pointer is set to the length of the Query data.    
 *     QueryType - This pointer is set to the type of SDP query provided.
 *
 * Returns:
 *     void
 *
 */
void FTP_BuildServiceQuery(const U8 **QueryPtr, U16 *QueryLen, 
                           SdpQueryType *QueryType);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* BT_STACK == XA_ENABLED */

#if FTP_EXPANDED_API == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_RegisterServer()
 *
 *     Registers the file transfer server with the GOEP multiplexor. This 
 *     includes registering the SDP records for the file transfer Service.
 *     All of the events specified in the GOEP layer are delivered to the
 *     FTP server.
 *
 * Parameters:
 *     Server - The server application's registration structure. The 
 *          'callback' and 'obstore' fields must be filled in.
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
ObStatus FTP_RegisterServer(GoepServerApp *Server, 
                                const ObStoreFuncTable *obStoreFuncs);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_DeregisterServer()
 *
 *     Deregisters the file transfer server from the GOEP multiplexor.
 *     This includes removing the SDP records for the file transfer Service.
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
ObStatus FTP_DeregisterServer(GoepServerApp *Server);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * FTP_ServerContinue()
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
ObStatus FTP_ServerContinue(GoepServerApp *Server);
#define FTP_ServerContinue(_SERVER) GOEP_ServerContinue(_SERVER)

/*---------------------------------------------------------------------------
 * FTP_ServerAccept()
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
ObStatus FTP_ServerAccept(GoepServerApp *Server, void *Obsh);
#define FTP_ServerAccept(_SERVER, _OBSH) GOEP_ServerAccept(_SERVER, _OBSH)

/*---------------------------------------------------------------------------
 * FTP_ServerAbort()
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
ObStatus FTP_ServerAbort(GoepServerApp *Server, ObexRespCode Resp);
#define FTP_ServerAbort(_SERVER, _RESP) GOEP_ServerAbort(_SERVER, _RESP)

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_ServerAuthenticate()
 *
 *     This function is used to either respond to an authentication challenge
 *     received from a client, or to send a challenge to the client. The
 *     server never sends both headers at the same time. If the client's
 *     request is challenged, it is automatically rejected with the reason
 *     OBRC_UNAUTHORIZED and the oper will complete with GOEP_EVENT_ABORTED.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Response - The authentication response to a received challenge.
 *
 *     Challenge - The authentication challenge.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server is not the active server or FTP
 *          encountered an error building the authentication header.
 *
 *     OB_STATUS_INVALID_PARM - The supplied authentication information
 *          conflicts with what is expected or acceptable.
 *
 *     OB_STATUS_SUCCESS - The authentication information was accepted.
 */
ObStatus FTP_ServerAuthenticate(GoepServerApp *Server, 
                                ObexAuthResponse *Response, 
                                ObexAuthChallenge *Challenge);
#define FTP_ServerAuthenticate(_SERVER, _RESP, _CHAL) \
	GOEP_ServerAuthenticate(_SERVER, _RESP, _CHAL)

/*---------------------------------------------------------------------------
 * FTP_ServerVerifyAuthResponse()
 *
 *     This function is used by the Server to verify an authentication
 *     response received from the client. The server should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Password - The password that the client was expected to provide. This
 *         value is used in the MD5 verification digest calculation.
 *
 *     PasswordLen - The length, in bytes of the password.
 *
 * Returns:
 *     TRUE - Verification succeeded, the digest provided by the client
 *           matched the one generated locally by the server.
 *
 *     FALSE - Verification failed, the client is not authenticated.
 */
BOOL FTP_ServerVerifyAuthResponse(GoepServerApp *Server, 
								  U8 *Password, U8 PasswordLen);
#define FTP_ServerVerifyAuthResponse(_SERVER, _PASS, _LEN) \
	GOEP_ServerVerifyAuthResponse(_SERVER, _PASS, _LEN)
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED 
/*---------------------------------------------------------------------------
 * FTP_RegisterClient()
 *
 *     Registers the file transfer client with the GOEP multiplexor.
 *     All of the events specified in the GOEP layer are delivered to the
 *     FTP client, except for GOEP_EVENT_TP_CONNECTED.
 *     
 * Parameters:
 *     Client - The client application's registration structure. The 
 *          'callback' and 'obstore' fields must be filled in.
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
 *     OB_STATUS_BUSY - An Ftp client is already registered.
 *     
 */
ObStatus FTP_RegisterClient(GoepClientApp *Client, 
                                const ObStoreFuncTable *obStoreFuncs);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_DeregisterClient()
 *
 *     Deregisters the file transfer client from the GOEP multiplexor.
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
ObStatus FTP_DeregisterClient(GoepClientApp *Client);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * FTP_Connect()
 *
 *     This function is used by the client to create a transport connection
 *     to the specified device and issue an OBEX Connect Request. The FTP
 *     Target header is automatically sent in the OBEX Connect request.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Target - This structure describes the server to which the client
 *         wishes to connect.
 *
 *     ConnReq - Optional connect request parameters. This parameter may
 *         be zero if no authentication headers are necessary.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled by an event to the application callback. 
 *
 *         If the connection is successful, a GOEP_EVENT_COMPLETE event for
 *         the GOEP_OPER_CONNECT operation will be signaled.
 *
 *         If the transport connection is successful, but the OBEX Connect
 *         failed, the completion event will be GOEP_EVENT_ABORTED for the
 *         operation GOEP_OPER_CONNECT. At this point the transport is
 *         CONNECTED but a successful OBEX Connect has not been sent.
 *         Additional calls to this function will generate new OBEX Connect
 *         requests. 
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
ObStatus FTP_Connect(GoepClientApp *Client, ObexTpAddr *Target, 
                         GoepConnectReq *ConnReq);

/*---------------------------------------------------------------------------
 * FTP_Disconnect()
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
ObStatus FTP_Disconnect(GoepClientApp *Client);
#define FTP_Disconnect(_CLIENT) GOEP_Disconnect(_CLIENT)

/*---------------------------------------------------------------------------
 * FTP_ClientAbort()
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
ObStatus FTP_ClientAbort(GoepClientApp *Client);
#define FTP_ClientAbort(_CLIENT) GOEP_ClientAbort(_CLIENT)

/*---------------------------------------------------------------------------
 * FTP_Push()
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
ObStatus FTP_Push(GoepClientApp *Client, GoepObjectReq *Object);
#define FTP_Push(_CLIENT, _OBJECT) GOEP_Push(_CLIENT, _OBJECT)

/*---------------------------------------------------------------------------
 * FTP_Pull()
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
 *     request will be sent when FTP_ClientContinue is called during the 
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
ObStatus FTP_Pull(GoepClientApp *Client, GoepObjectReq *object, BOOL more);
#define FTP_Pull(_CLIENT, _OBJECT, _MORE) GOEP_Pull(_CLIENT, _OBJECT, _MORE)

/*---------------------------------------------------------------------------
 * FTP_Delete()
 *
 *     Initiates the OBEX "Put" operation to delete an object on the remote
 *     server. The 'ObjectName' field is required.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     ObjectName - The name of the object to delete on the server.
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
 */
ObStatus FTP_Delete(GoepClientApp *Client, const GoepUniType *ObjectName);

/*---------------------------------------------------------------------------
 * FTP_SetFolderFwd()
 *
 *     Performs the OBEX "SetPath" operation to a new folder.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     FolderName - The name of the child folder to change to (required).
 *
 *     AllowCreate - Set to TRUE to allow the server to create the folder
 *          if it does not exist (like a CreateFolder function).
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
 */
ObStatus FTP_SetFolderFwd(GoepClientApp *Client, 
                           const GoepUniType *FolderName, 
                           BOOL AllowCreate);

/*---------------------------------------------------------------------------
 * FTP_SetFolderBkup()
 *
 *     Performs the OBEX "SetPath" operation to change to the parent folder.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
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
 */
ObStatus FTP_SetFolderBkup(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * FTP_SetFolderRoot()
 *
 *     Performs the OBEX "SetPath" operation to change to the root folder.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
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
 */
ObStatus FTP_SetFolderRoot(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * FTP_ClientContinue()
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
ObStatus FTP_ClientContinue(GoepClientApp *Client);
#define FTP_ClientContinue(_CLIENT) GOEP_ClientContinue(_CLIENT)

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * FTP_ClientVerifyAuthResponse()
 *
 *     This function is used by the Client to verify an authentication
 *     response received from the server. The client should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Password - The password that the server was expected to provide. This
 *         value is used in the MD5 verification digest calculation.
 *
 *     PasswordLen - The length, in bytes of the password.
 *
 * Returns:
 *     TRUE - Verification succeeded, the digest provided by the server
 *           matched the one generated locally by the client.
 *
 *     FALSE - Verification failed, the server is not authenticated.
 */
BOOL FTP_ClientVerifyAuthResponse(GoepClientApp *Client, 
								  U8 *Password, U8 PasswordLen);
#define FTP_ClientVerifyAuthResponse(_CLIENT, _PASS, _LEN) \
	GOEP_ClientVerifyAuthResponse(_CLIENT, _PASS, _LEN)
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* FTP_EXPANDED_API == XA_ENABLED */

/****************************************************************************
 *
 * Internal Data Structures
 *
 ****************************************************************************/

#if (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER)
/*---------------------------------------------------------------------------
 * FtpRecords structure
 *
 *     This structure is used internally by the GOEP Server layer.
 */
typedef struct _FtpRecords {
    BOOL                registered;     /* SDP Registration flag */
    SdpRecord           record;         /* SDP record */
    SdpAttribute        attributes[5];  /* Service Class Id & Proto Desc. */
} FtpRecords;
#endif /* (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER) */

/*---------------------------------------------------------------------------
 * FtpExtensionData structure
 *
 *     This structure is used internally by the GOEP Client & Server layers.
 */
typedef struct _FtpExtensionData {
    BOOL                initialized;                    

#if FTP_EXPANDED_API == XA_ENABLED
#if OBEX_ROLE_SERVER == XA_ENABLED
    /* OBEX Server connection registration structure */
    ObexConnection      obexConn;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
    /* Holds clients OBEX Connect request while transport is connecting. */
    GoepConnectReq      connect;
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#endif /* FTP_EXPANDED_API == XA_ENABLED */

#if (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER)
    /* SDP entries registered by the FTP server */
    FtpRecords          sdp[GOEP_NUM_OBEX_CONS];
#endif /* (BT_STACK == XA_ENABLED) && (OBEX_ROLE_SERVER) */

} FtpExtensionData;


/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
#if XA_CONTEXT_PTR == XA_ENABLED
#define FTP(f) (Ftp->f)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define FTP(f) (Ftp.f)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

#endif /* __FTP_H */
