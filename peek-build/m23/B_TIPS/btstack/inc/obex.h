#ifndef __OBEX_H
#define __OBEX_H
/****************************************************************************
 *
 * File:          obex.h
 *
 * Description:   This file specifies the OBEX Layer API.
 * 
 * Created:       May 8, 1997
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
#include "xatypes.h"
#include "xastatus.h"
#include "obconfig.h"
#include "obexerr.h"

#if BT_STACK == XA_ENABLED
#include "rfcomm.h"
#include "sdp.h"
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
#include "irlmpttp.h"
#endif /* IRDA_STACK == XA_ENABLED */
#if TCP_STACK == XA_ENABLED
#endif /* TCP_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX API layer
 *
 *     The OBEX component allows applications to put and get any kind of
 *     digital object. This section describes the APIs necessary to
 *     set up an OBEX Client and Server, start Client operations, and
 *     accept operations at the server.
 *
 *     Also defined here are the basic Object Store functions that must
 *     be provided by the application.
 */

/****************************************************************************
 *
 * Section: Constants
 *
 ****************************************************************************/

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * UNKNOWN_OBJECT_LENGTH constant
 *
 *     This constant is returned by OBS_GetObjectLen() when the objects
 *     length is unknown. It is commonly used with objects that are generated
 *     dynamically during the exchange. In exchanges where this value is
 *     returned, the protocol calls the OBS_ReadFlex() read function.
 *
 * Requires:
 *     OBEX_DYNAMIC_OBJECT_SUPPORT set to XA_ENABLED.
 */
#define UNKNOWN_OBJECT_LENGTH   0xFFFFFFFF
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX_INVALID_CONNID constant
 *
 *     ObexConnection.connId is set to this value when an OBEX connection is
 *     not active.  This value can be used by clients without enabling server
 *     connection support.
 */
#define OBEX_INVALID_CONNID     0xFFFFFFFF

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_TIMEOUT_DONTCARE constant
 *
 *     This constant is used with the OBEX Reliable Session functions to 
 *     indicate that the application does not care what session suspend
 *     timeout value is used. When this value is specified, the previously
 *     requested value will be used. If no value has been previously specified,
 *     the default, infinite timeout will be used.
 */
#define OBEX_TIMEOUT_DONTCARE   0

/*---------------------------------------------------------------------------
 * OBEX_TIMEOUT_INFINITE constant
 *
 *     This constant is used with the OBEX Reliable Session functions to 
 *     indicate that the application requests that the session suspend timeout
 *     be set to infinite.
 */
#define OBEX_TIMEOUT_INFINITE   0xFFFFFFFF

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObStatus type
 *
 * Indicates the status of various OBEX Layer API functions.
 */
typedef XaStatus ObStatus;

/* The function call was successful. */
#define OB_STATUS_SUCCESS          XA_STATUS_SUCCESS 

/* The operation has failed to start. */
#define OB_STATUS_FAILED           XA_STATUS_FAILED 

/* The operation has successfully started, callback indicates end. */
#define OB_STATUS_PENDING          XA_STATUS_PENDING

/* The transport layer link is disconnected. */
#define OB_STATUS_DISCONNECT       XA_STATUS_DISCONNECT

/* No connection exists, when one is required */
#define OB_STATUS_NO_CONNECT       XA_STATUS_NO_LINK

/* IrDA Only: The media is busy. */
#define OB_STATUS_MEDIA_BUSY       XA_STATUS_MEDIA_BUSY

/* A request is pending or resources are in use. */
#define OB_STATUS_BUSY             XA_STATUS_BUSY

/* The necessary resources are not available. */
#define OB_STATUS_NO_RESOURCES     XA_STATUS_NO_RESOURCES

/* An invalid function parameter was provided. */
#define OB_STATUS_INVALID_PARM     XA_STATUS_INVALID_PARM

/* Bluetooth Only: The request failed because the specified device
 *                 is in the process of being disconnected.
 */
#define OB_STATUS_IN_PROGRESS      XA_STATUS_IN_PROGRESS

/* Feature not supported */
#define OB_STATUS_NOT_SUPPORTED    XA_STATUS_NOT_SUPPORTED

/* Bluetooth Only : Feature is restricted, due to a 
 *                  Bluetooth security failure 
 */
#define OB_STATUS_RESTRICTED       XA_STATUS_RESTRICTED

#if TI_CHANGES == XA_ENABLED
/* Create connection was cancelled */
#define OB_STATUS_CANCELLED         XA_STATUS_CANCELLED
#endif /* TI_CHANGES == XA_ENABLED */

/* The object handle was invalid */
#define OB_STATUS_INVALID_HANDLE   (XA_STATUS_OPEN+0)

/* The OBEX packet is too small to support the request. */
#define OB_STATUS_PACKET_TOO_SMALL (XA_STATUS_OPEN+1)

/* No active session exists, when one is required. */
#define OB_STATUS_NO_SESSION       (XA_STATUS_OPEN+2)

/* End of ObStatus */

/*---------------------------------------------------------------------------
 * ObClientEvent type
 *
 *     Events received by the Client application callback (ObClientAppCb).
 */
typedef U8 ObClientEvent;

/* No Event. This event can be used by the application internally. */
#define OBCE_NO_EVENT           0

/* The transport layer is connected. This is the successful result of
 * calling OBEX_ClientTpConnect().
 */
#define OBCE_CONNECTED          1

/* The transport layer connection has been disconnected. For the IrDA
 * transport only, this event may indicate either a service (IrLMP) or 
 * complete transport (IrLAP) disconnection.  In the case of a server
 * initiated transport disconnection, two events may be received, one
 * for each transport level connection.  These events can be distinguished
 * by calling the OBEX_IsIrLapConnected function.
 */
#define OBCE_DISCONNECT         2

/* The requested OBEX operation has completed successfully. Used to signal
 * completion of Get, Put, SetPath, Connect, and Disconnect operations.
 */
#define OBCE_COMPLETE           3

/* Indicates that the current OBEX operation has been aborted. The abort
 * reason is provided in the client callback parameters "u.abortReason" field.
 */
#define OBCE_ABORTED            4

/* An OBEX header (or a portion thereof) has been received and is available to
 * the application. The client callback parameters "u.headerRx" structure
 * contains the information about the header, including the header type and
 * contents.
 */
#define OBCE_HEADER_RX          5

/* IrDA Only: Device discovery has failed to find an OBEX capable device.
 * This event notifies of the failure of a call to OBEX_ClientTpConnect().
 */
#define OBCE_DISCOVERY_FAILED   6

/* The device was found, but no suitable OBEX server/service could be
 * located. Most likely the SDP or IAS query failed.
 */
#define OBCE_NO_SERVICE_FOUND   7

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
/* Only for packet based flow control. It indicates that it is
 * time to send a continue command to the server. The client must
 * call OBEX_ClientSendCommand() once for each time this event is
 * received. To avoid delaying the command, the call should be made
 * during the event notification.
 */
#define OBCE_SEND_COMMAND       8
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/* The active session has been suspended due to an unexpected link disconnect.
 * This event indicates that the current reliable session has been suspended
 * and the unreliable session is now active. If the suspend timeout is not
 * infinite, the application should start a suspend timer for this session.
 */
#define OBCE_SESSION_SUSPENDED   9

/* Session parameters have been received and parsed as part of a session 
 * operation. The parsed parameters are available in the "u.sessionParms"
 * field in the client callback parameters structure.
 */
#define OBCE_SESSION_PARMS_RX   10

/* The active OBEX operation has been suspended due to a link disconnect.
 * This event indicates that a Get, Put, SetPath, Connect or Disconnect
 * operation active over a reliable session has been suspended. This 
 * event also indicates that the current reliable session has been suspended
 * and the unreliable session is now active. If the suspend timeout is not
 * infinite, the application should start a suspend timer for this session.
 *
 * Additionally, all of the parameters provided in the "u.suspended" structure
 * of the client callback parameters should be preserved until the session is
 * resumed, closed or the timeout expires. If the ability to resume the session
 * in the future is desired, these session parameters MUST be preserved.
 */
#define OBCE_SUSPENDED          11

/* A suspended OBEX operation has been resumed. This event is indicated
 * when an OBEX operation is resumed in conjunction with the resumption of
 * a reliable session. The operation type is provided in opcode field of 
 * the client callback parameters structure.
 */
#define OBCE_RESUME_OPER        12

/* OBEX has detected a critical error in the session state. This may be
 * caused by a protocol violation by the server or by an inconsistency between
 * the client and server session state. The active session is no longer
 * active and the unreliable session is now active. The client may attempt
 * to close the session, but in any case it should destroy the session because
 * it is unlikely that the session can be resumed.
 */
#define OBCE_SESSION_ERROR      13

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/* End of ObClientEvent */


/*---------------------------------------------------------------------------
 * ObServerEvent type
 *
 *     Events received by the Server application callback (ObServerAppCb).
 */
typedef U8 ObServerEvent;

/** No Event. This event can be used by the application internally. */
#define OBSE_NO_EVENT           0

/** A transport connection has been established to the server.
 * The connection was initiated by another device.
 */
#define OBSE_CONNECTED          1

/** The transport layer connection has been disconnected. For the IrDA
 * transport only, this event may indicate either a service (IrLMP) or 
 * complete transport (IrLAP) disconnection.  In the case of a client
 * initiated transport disconnection, two events may be received, one
 * for each transport level connection.  These events can be distinguished
 * by calling the OBEX_IsIrLapConnected function.
 */
#define OBSE_DISCONNECT         2

/** The current server operation is now complete. If the current opcode is
 * OBSE_OPCODE_SUSPEND_SESSION the application should suspend the active
 * session. The session should be preserved until the session is resumed,
 * closed or the timeout expires. If the ability to resume the session in
 * the future is desired, the session context must be preserved.
 */
#define OBSE_COMPLETE           3

/** Indicates that the current server operation was aborted. */
#define OBSE_ABORTED            4

/** An OBEX header (or a portion thereof) has been received and is available to
 * the application. The server callback parameters "u.headerRx" structure
 * contains the information about the header, including the header type and
 * contents.
 */
#define OBSE_HEADER_RX          5

/** The server has begun processing an OBEX Put request. */
#define OBSE_PUT_START          6

/** The server has begun processing an OBEX Get request. */
#define OBSE_GET_START          7

/** The server has begun processing an OBEX SetPath request. The server callback
 * parameters "u.setPathFlags" field contains the flags that were sent in the
 * client's request.
 */
#define OBSE_SET_PATH_START     8

/** The server has begun processing an OBEX Connect request. */
#define OBSE_CONNECT_START      9

/** The server has begun processing an OBEX Disconnect request. */
#define OBSE_DISCONNECT_START   10

/** The server has begun processing an OBEX Abort request. */
#define OBSE_ABORT_START        11

/** Instructs the server to provide an object store handle to the protocol.
 * This event is generated during the processing of Put and Get operations.
 * The server calls OBEX_ServerAccept() to provide the object. Failure to 
 * do so will cause the operation to be rejected by the protocol.
 */
#define OBSE_PROVIDE_OBJECT     12

/** Instructs the server to delete an object. This event is generated 
 * during the processing of a Put-Delete operation. The delete is assumed
 * to be successful, so the server must call OBEX_ServerAbort() if the 
 * object could not be deleted. 
 */
#define OBSE_DELETE_OBJECT      13

/** Indicates to the server that the current operation is about to be
 * completed. This gives the server an opportunity to perform final steps
 * to complete the operation while the opportunity to abort the operation
 * in case of failure still exists. Note that when OBEX_PACKET_FLOW_CONTROL
 * is enabled, this event will be generated during the application's call
 * to OBEX_ServerSendResponse().
 */
#define OBSE_PRECOMPLETE        14

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
/** Only for packet based flow control. This event indicates to the server
 * that it is time to send a response to the client. The server must call
 * OBEX_ServerSendResponse() once for each time this event is received. 
 * To avoid delaying the response, the call should be made during the
 * event notification. 
 */
#define OBSE_SEND_RESPONSE      15
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/** The server has begun processing an OBEX Session request. The session
 * request type is provided in the opcode field of the server callback
 * parameters. The opcode will be one of the following session opcodes:
 * Create, Resume, Suspend, Close, or SetTimeout.
 */
#define OBSE_SESSION_START      16

/** The active session has been suspended due to an unexpected link disconnect.
 * This event indicates that the current reliable session has been suspended
 * and the unreliable session is now active. If the suspend timeout is not
 * infinite, the application should start a suspend timer for this session.
 *
 * Additionally, all of the parameters provided in the "u.suspended" structure
 * of the server callback parameters should be preserved until the session is
 * resumed, closed or the timeout expires. If the ability to resume the session
 * in the future is desired, these session parameters MUST be preserved.
 */
#define OBSE_SESSION_SUSPENDED  17

/** Session parameters have been received and parsed as part of a session 
 * operation. The parsed parameters are available in the "u.sessionParms"
 * field in the server callback parameters structure.
 */
#define OBSE_SESSION_PARMS_RX   18

/** Provide a new or suspended session to the OBEX Server. This event is
 * indicated during the processing of both Create Session and Resume Session
 * operations. The server application calls OBEX_ServerAcceptSession() to
 * provide the session. Failure to do so will cause the session operation
 * to be rejected by OBEX Server. When the session operation is resume, the
 * session provided by the application must match the session parameters
 * received from the client in the OBSE_SESSION_PARMS_RX event.
 */
#define OBSE_PROVIDE_SESSION    19

/** The active OBEX operation has been suspended due to a link disconnect.
 * This event indicates that a Get, Put, SetPath, Connect or Disconnect
 * operation active over a reliable session has been suspended. This 
 * event also indicates that the current reliable session has been suspended
 * and the unreliable session is now active. If the suspend timeout is not
 * infinite, the application should start a suspend timer for this session.
 *
 * Additionally, all of the parameters provided in the "u.suspended" structure
 * of the server callback parameters should be preserved until the session is
 * resumed, closed or the timeout expires. If the ability to resume the session
 * in the future is desired, these session parameters MUST be preserved.
 */
#define OBSE_SUSPENDED          20

/** A suspended OBEX operation has been resumed. This event is indicated
 * when an OBEX operation is resumed in conjunction with the resumption of
 * a reliable session. The operation type is provided in "opcode" field of 
 * the server callback parameters structure.
 */
#define OBSE_RESUME_OPER        21

/** OBEX has detected a critical error in the session. This may be caused
 * by a protocol violation by the client or an inconsistency between
 * the client and server session state. The active session is no longer
 * active and the unreliable session is now active. The server should
 * destroy the session because it is unlikely that the session can be resumed.
 */
#define OBSE_SESSION_ERROR      22

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
/** IrDA Only: Device discovery has failed to find an OBEX capable device.
 * This event notifies of the failure of a call to OBEX_ClientTpConnect().
 */
#define OBSE_DISCOVERY_FAILED   23

/** The device was found, but no suitable OBEX service could be
 * located. Most likely the SDP or IAS query failed.
 */
#define OBSE_NO_SERVICE_FOUND   24
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

/* End of ObServerEvent */

/*---------------------------------------------------------------------------
 * ObexSetPathFlags type
 *
 *     Flags used in the SetPath operation (OBEX_ClientSetPath and
 *     OBEX_ServerGetPathFlags).
 */
typedef U8 ObexSetPathFlags;

/* No flags */
#define OSPF_NONE           0x00

/* Back up one level. */
#define OSPF_BACKUP         0x01

/* Don't create the specified folder if it doesn't exist. */
#define OSPF_DONT_CREATE    0x02

/* End of ObexSetPathFlags */

/*---------------------------------------------------------------------------
 * ObexAbortReason type
 *
 *     Used to describe the reason for an Abort indication (returned by
 *     OBEX_ClientGetAbortReason).
 *
 *     The ObexAbortReason type is a superset of ObexRespCode and therefore
 *     includes all of the OBRC_ defines in obexerr.h. These defines are
 *     specifically for unilateral or client-internal abort reasons.
 */
typedef U8 ObexAbortReason;

/* The underlying transport connection has been disconnected. */
#define OBRC_LINK_DISCONNECT        0x80

/* The client suffered an Object Store read or write error. */
#define OBRC_CLIENT_RW_ERROR        0x81

/* The operation was aborted because of a call to OBEX_ClientAbort(). 
 * This status allows the Abort packet to be sent immediately, with 
 * the possibility of two unacknowledged packets outstanding 
 * (e.g. Put and Abort or Get and Abort).  This status is only valid
 * for an unreliable OBEX session.
 */
#define OBRC_USER_ABORT             0x82

#if OBEX_SESSION_SUPPORT == XA_ENABLED

/* The operation was aborted because of a call to OBEX_ClientAbort(). 
 * This status forces the Abort packet to be sent in the normal 
 * request/response sequence. In this way, the Abort packet will not
 * be sent out until a response has been received for our last request
 * packet. This status is only valid for a reliable OBEX session.
 */
#define OBRC_SESSION_USER_ABORT     0x83

/* The client did not receive a Session Sequence Number header in the last
 * server session response.
 */
#define OBRC_SESSION_NO_SEQ_NUM     0x84

/* Either the client did not receive a required session parameter in
 * the server's session command response. Or a parameter provided by the
 * server was invalid.
 */
#define OBRC_SESSION_INVALID_PARMS  0x85

/* An unrecoverable session error occurred. The session state may not be
 * consistent between the client and server. This event only occurs when
 * a session error is detected during the course of an non-session
 * operation. If a session error is detected during a session operation,
 * the operation will be aborted with the abort reason set to one of the
 * OBRC_SESSION_ error codes.
 */
#define OBRC_SESSION_ERROR          0x86
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
/* End of ObexAbortReason */


/*---------------------------------------------------------------------------
 * ObexTpType type
 *
 *     Specifies the different transport layers supported by OBEX. Protocols
 *     can be ORed together if necessary.
 *
 *     If you are adding support for a new transport, you should
 *    modify obex.h to contain a new bit for the transport.
 *    
 */
typedef U8 ObexTpType;

/* No transport protocol exists */
#define OBEX_TP_NONE            0x00

/* Specifies the IrDA protocol. */
#define OBEX_TP_IRDA            0x01

/* Specified the IrSimple protocol */
#define OBEX_TP_IRSIMPLE        0x02

/* Specifies the Bluetooth protocol. */
#define OBEX_TP_BLUETOOTH       0x04

/* Specifies the TCP protocol. */
#define OBEX_TP_TCP             0x08

/* Specifies an OEM added transport. */
#define OBEX_TP_OEM             0x10

/* End of ObexTpType */

/*--------------------------------------------------------------------------
 * ObexOpcode type 
 *
 *     Definitions of the possible OBEX command opcodes.  These opcodes
 *     are returned in the ObexClientCallbackParms when OBCE_COMPLETE is 
 *     indicated.
 */
typedef U8 ObexOpcode;

/* OBEX Connect operation opcode. */
#define OB_OPCODE_CONNECT               0x00

/* OBEX Disconnect operation opcode. */
#define OB_OPCODE_DISCONNECT            0x01

/* OBEX Put operation opcode. */
#define OB_OPCODE_PUT                   0x02

/* OBEX Get operation opcode. */
#define OB_OPCODE_GET                   0x03

/* OBEX Set Path operation opcode. */
#define OB_OPCODE_SET_PATH              0x05

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/* Create a reliable OBEX session */
#define OB_OPCODE_CREATE_SESSION        0x10

/* Close an existing OBEX session */
#define OB_OPCODE_CLOSE_SESSION         0x11

/* Suspend an active OBEX session */
#define OB_OPCODE_SUSPEND_SESSION       0x12

/* Activate a suspended OBEX session */
#define OB_OPCODE_RESUME_SESSION        0x13

/* Set the suspend timeout for an OBEX session */
#define OB_OPCODE_SET_SESSION_TIMEOUT   0x14

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/* End of ObexOpcode */

/* Forward type declarations needed for obexcmd.h and obxbtstk.h */
typedef struct _ObexAppHandle               ObexAppHandle;
typedef struct _ObexClientApp               ObexClientApp;
typedef struct _ObexServerApp               ObexServerApp;
typedef struct _ObexClientSession           ObexClientSession;
typedef struct _ObexServerSession           ObexServerSession;
typedef struct _ObexSessionParms            ObexSessionParms;
typedef struct _ObexClientCallbackParms     ObexClientCallbackParms;
typedef struct _ObexServerCallbackParms     ObexServerCallbackParms;
typedef struct _ObexTpConnInfo              ObexTpConnInfo;
typedef struct _ObStoreFuncTable            ObStoreFuncTable;
typedef struct _ObexConnection              ObexConnection;
typedef struct _ObexHeaderRx                ObexHeaderRx;
#if BT_STACK == XA_ENABLED
typedef struct _ObexBtTarget                ObexBtTarget;
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
typedef struct _ObexIrTarget                ObexIrTarget;
#endif /* IRDA_STACK == XA_ENABLED */
#if TCP_STACK == XA_ENABLED
typedef struct _ObexTcpTarget               ObexTcpTarget;
#endif /* TCP_STACK == XA_ENABLED */
#if OEM_STACK == XA_ENABLED
typedef struct _ObexOemTarget               ObexOemTarget;
#endif /* OEM_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * ObClientAppCb type
 *
 *     Type of function provided by the application to handle OBEX Client
 *     events. The application callback function is registered using the
 *     OBEX_ClientInit function.  The client event and information pertaining
 *     to this event are contained in the "parms" parameter.
 */
typedef void (*ObClientAppCb)(ObexClientCallbackParms *parms);

/* End of ObClientAppCb type */

/*---------------------------------------------------------------------------
 * ObServerAppCb type
 *
 *     Type of function provided by the application to handle OBEX Server
 *     events. This application callback function is registered using the
 *     OBEX_ServerInit function.  The server event and information pertaining
 *     to this event are contained in the "parms" parameter
 */
typedef void (*ObServerAppCb)(ObexServerCallbackParms *parms);

/* End of ObServerAppCb type */

#if (OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED) && (BT_STACK == XA_ENABLED)
/*---------------------------------------------------------------------------
 * ObSdpResultsCb type
 *
 *     Type of function provided by the application to handle OBEX Client
 *     SDP Query result information. This application callback function is 
 *     registered using the OBEX_RegisterSdpResultsCallback function. The 
 *     SDP Query result information is contained in the "token" parameter, 
 *     while the "customRfChannel" parameter is provided to allow the 
 *     application the ability to override the RFCOMM channel used for the 
 *     transport connection. In profiles such as BPP, the Additional Protocol 
 *     Descriptor List attribute may provide an RFCOMM channel, and in some 
 *     cases that RFCOMM channel should be used in place of the default RFCOMM
 *     channel that OBEX retrieves from the Protocol Descriptor List. Any
 *     additional SDP parsing should occur during this callback with the
 *     "customRfChannel" parameter being set to a nonzero RFCOMM channel value
 *     if the RFCOMM channel is to be overridden.
 */
typedef void (*ObSdpResultsCb)(SdpQueryToken *token, U8 *customRfChannel);

/* End of ObSdpResultsCb type */
#endif /* (OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED) && (BT_STACK == XA_ENABLED) */

/* End of ObSdpResultsCb type */

/****************************************************************************
 *
 * Section: Object Store API functions
 *
 ****************************************************************************
 *
 *     The Object Store component provides the interface through which the
 *     Command Interpreter can store and retrieve objects. It is viewed by
 *     the OBEX Protocol Layer as a part of the application framework. This
 *     system dependent component presents a well-defined interface to the
 *     Command Interpreter and some internal interfaces for the application's
 *     use. This chapter describes the three Object Store functions that are
 *     required for the OBEX Protocol Layer. For a discussion of the Object
 *     Store functions and structures used internally by the application refer
 *     to the Example Object Store in the Writing OBEX Applications chapter
 *     of the OBEX Developer's Guide document.
 *
 *     The required Object Store functions are prototyped in the obex.h
 *     include file. These functions are linked to each Obex Server and Client
 *     through the ObStoreFuncTable defined by the Object Store.  The Object
 *     Store names for these functions are irrelevant, but the parameters must
 *     match those of the function prototypes defined below. 
 *
 *     OBEX version 3.1 introduces a fourth function call that is required
 *     only when OBEX_DYNAMIC_OBJECT_SUPPORT is enabled. This function,
 *     OBS_ReadFlex() is used instead of OBS_Read() when reading
 *     objects whose exact size is not known at the start of the operation.
 *     The protocol will automatically switch to calling Read Flex instead
 *     of Read when the OBS_GetObjectLen() function returns the constant
 *     UNKNOWN_OBJECT_LENGTH. It is recommended that this method only be used
 *     when necessary, because it reduces the transmit efficiency of the OBEX
 *     protocol. However, for small objects and/or slow devices, the
 *     difference is negligible.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * OBS_Write() type
 *
 *      A function of this type is provided by the OBEX Object Store 
 *      and stored in the ObStoreFuncTable.  This function writes 
 *      data to the object store entry.
 *      
 *      This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:
 *      obs -   pointer to the handle of object store entry.
 *              Casting from void* to the object store entry
 *              must be done inside this function.
 *
 *      buff -  pointer to data to write
 *
 *      len -   length in bytes of data to write
 */
typedef ObexRespCode (*OBS_Write)(void *obs, U8 *buff, U16 len);

/*---------------------------------------------------------------------------
 * OBS_Read() type
 *
 *      A function of this type is provided by the OBEX Object Store 
 *      and stored in the ObStoreFuncTable.  This function reads 
 *      data from the object store entry.
 *      
 *      This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:
 *      obs -   pointer to the handle of object store entry.
 *              Casting from void* to the object store entry
 *              must be done inside this function.
 *
 *      buff -  pointer to location where data is read into.
 *
 *      len -   length in bytes of data to read.
 */
typedef ObexRespCode (*OBS_Read)(void *obs, U8 *buff, U16 len);

/*---------------------------------------------------------------------------
 * OBS_GetObjectLen() type
 *
 *      A function of this type is provided by the OBEX Object Store 
 *      and stored in the ObStoreFuncTable.  This function gets 
 *      the length of an object store item.
 *      
 *      If the object store contains objects which have unknown lengths,
 *      OBEX must be compiled with OBEX_DYNAMIC_OBJECT_SUPPORT enabled.
 *      The object store can then return UNKNOWN_OBJECT_LENGTH for those
 *      objects.
 *  
 *      This function is REQUIRED by the OBEX Protocol.
 *
 * Parameters:
 *      obs -   pointer to the handle of object store entry.
 *              Casting from void* to the object store entry
 *              must be done inside this function.
 */
typedef U32 (*OBS_GetObjectLen)(void *obs);

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBS_SetOffset() type
 *
 *      A function of this type is provided by the OBEX Object Store 
 *      and stored in the ObStoreFuncTable. This function sets the object 
 *      offset into the current object store entry.
 *
 *      This function is only used in conjunction with reliable OBEX sessions. 
 *      When using reliable OBEX sessions, OBEX will maintain an offset into
 *      the current object being processed. In the case of an unexpected
 *      disconnection, OBEX will restore the connection based on the last valid 
 *      object offset. Based on the fact that an entire OBEX packet may not
 *      have been processed before the connection ended, OBEX will have to
 *      reset the object offset on OBEX packet boundaries. This may result
 *      in the reading or writing of a portion of duplicate data.
 *      
 * Requires:
 *      OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *      obs - pointer to the handle of the object store entry. Casting from 
 *            void* to the object store entry must be done inside this function.
 *
 *      offset -   offset into the object store entry.
 *
 * Returns:
 *     OBRC_SUCCESS - Operation is successful.
 *
 *     OBRC_NOT_FOUND - One or both of the object store entries is invalid.
 */
typedef ObexRespCode (*OBS_SetOffset)(void *obs, U32 offset);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBS_ReadFlex() type
 *
 *      A function of this type is provided by the OBEX Object Store 
 *      and stored in the ObStoreFuncTable.  This function reads 
 *      data from the object store entry. This function can read 
 *      any number of bytes, up to 'len'.
 *
 *      This function is only used for objects of unknown length (such an
 *      object will return UNKNOWN_OBJECT_LENGTH from the OBS_GetObjectLen
 *      function. For objects of known length, OBEX uses OBS_Read()
 *      instead.
 *
 * Requires:
 *      OBEX_DYNAMIC_OBJECT_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *      obs -   pointer to the handle of object store entry.
 *              Casting from void* to the object store entry
 *              must be done inside this function.
 *
 *      buff -  pointer to location where data is read into.
 *
 *      len -   On input, contains the maximum length in bytes of data to read.
 *              Before returning, the object store must write the actual number
 *              of bytes read into "buff".
 *
 *      more -  Before returning, the object store must write "TRUE" or
 *              "FALSE" into the boolean referenced by this parameter. TRUE
 *              indicates that there is more data to read, FALSE indicates
 *              that all object data has been read.
 */
typedef ObexRespCode (*OBS_ReadFlex)(void *obs, U8 *buff, U16 *len, BOOL *more);
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Callback Parameter Data Structures - must be defined before obexcmd.h
 *                                      and transport include files.
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 * ObexHeaderRx structure
 * 
 *     
 */
struct _ObexHeaderRx {
    U8              type;     /* Server header type */
    U16             totalLen; /* Overall header length */
    U16             remainLen; /* Remaining header length */
    U16             currLen;  /* Length of current header buffer */
    U8             *buff;     /* For Unicode and Byte Sequence */
    U32             value;    /* For 1-byte and 4-byte headers */
};

/*--------------------------------------------------------------------------
 * ObexClientCallbackParms structure
 *
 *     Describes an OBEX Client callback event and any data that relates 
 *     to the event.  Various fields in the structure may be valid or not, 
 *     depending on the "event" field.
 */
struct _ObexClientCallbackParms {
    ObClientEvent           event;  /* Type of the OBEX Client event. */
    ObexOpcode              opcode; /* Current OBEX operation */
    ObexClientApp          *client; /* Which OBEX Client this event is for. */

    /* Group: The event type determines whether a field in this
     * union is valid.
     */
    union {

        /* During an OBCE_ABORTED event, contains the abort reason code and the 
         * opcode that was just aborted (use opcode field above)
         */
        ObexAbortReason     abortReason;

        /* During an OBCE_HEADER_RX event, contains the partial or fully 
         * received header information
         */
        ObexHeaderRx            headerRx;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
        /* During an OBCE_SESSION_SUSPENDED event, contains the session 
         * that was suspended. During an OBCE_SESSION_ERROR event, contains
         * the active session.
         */
        ObexClientSession      *session; /* Suspended Session */

        /* During OBCE_SESSION_PARMS_RX this structure contains the 
         * parsed Session Parameters header data received in a response.
         */
        ObexSessionParms       *sessionParms;

        /* This structure is used during OBCE_SUSPENDED events. An OBEX
         * operation was in progress when the session was suspended due
         * to a link disconnect. The following suspend operation
         * parameters must be saved for when the session is resumed.
         */
        struct {
            /* Contains the session that was suspended. */
            ObexClientSession  *session;

            /* Current operation's object store handle. If this field is
             * non-zero a valid object store handle must be provided when
             * resuming this session.
             */
            void               *obsh;

            /* Unacknowledged application header information. If the header
             * length field is non-zero, the contents of the header buffer
             * must be saved and provided when resuming this session. The
             * "headerLen" field indicates the number of bytes in the buffer.
             */
            U16                 headerLen;
            U8                 *headerBuff;
        } suspended;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */               
    } u;
};

/*--------------------------------------------------------------------------
 * ObexServerCallbackParms structure
 *
 *     Describes an OBEX Server callback event and any data that relates 
 *     to the event.  Various fields in the structure may be valid or not, 
 *     depending on the "event" field.
 */
struct _ObexServerCallbackParms {
    ObServerEvent           event;  /* Type of the OBEX Server event. */
    ObexOpcode              opcode; /* Current OBEX operation */
    ObexServerApp          *server; /* Which OBEX Server this event is for. */

    /* Group: The event type determines whether a field in this
     * union is valid.
     */
    union {
        /* During an OBSE_HEADER_RX event, contains the partial or fully 
         * received header information
         */
        ObexHeaderRx            headerRx;

        /* During an OBSE_SET_PATH_START event, contains the set path flags. */
        ObexSetPathFlags        setPathFlags;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
        /* During OBSE_SESSION_PARMS_RX this structure contains the 
         * parsed Session Parameters header data.
         */
        ObexSessionParms       *sessionParms;

        /* During OBSE_SESSION_ERROR events. This active session
         * should be destroyed upon reception of this event.
         */
        ObexServerSession      *session;

        /* This structure is used during both OBSE_SUSPENDED and
         * OBSE_SESSION_SUSPENDED events to suspend the active session.
         * During an OBSE_SUSPENDED event, an OBEX operation was in progress
         * when the session was suspended due to a link disconnect. For
         * both events, the following operation suspend parameters must
         * be saved for when the session is resumed.
         */
        struct {
            /* This active session must be suspended upon reception of
             * this event. This event only occurs when the session is
             * suspended due to a link disconnect. 
             */
            ObexServerSession  *session;

            /* Current operation's object store handle. If this field is
             * non-zero a valid object store handle must be provided when
             * resuming this session.
             */
            void               *obsh;

            /* Unacknowledged application header information. If the header
             * length field is non-zero, the contents of the header buffer
             * must be saved and provided when resuming this session. The
             * "headerLen" field indicates the number of bytes in the buffer.
             */
            U16                 headerLen;
            U8                 *headerBuff;
        } suspended;

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
    } u;
};

/* Now include these files, since they are needed for some data 
 * structures below 
 */
#include "sys/obexcmd.h"
#if BT_STACK == XA_ENABLED
#include "sys/obxbtstk.h"
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
#include "sys/obxirstk.h"
#endif /* IRDA_STACK == XA_ENABLED */
#if TCP_STACK == XA_ENABLED
#include "sys/obxtcpstk.h"
#endif /* TCP_STACK == XA_ENABLED */
#if OEM_STACK == XA_ENABLED
#include "sys/obxoemstk.h"
#endif /* OEM_STACK == XA_ENABLED */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

#if OBEX_TLV_PARSER == XA_ENABLED
/*--------------------------------------------------------------------------
 * ObexTlvParser structure
 * 
 *     The Obex Byte Tag Parser structure is used when parsing OBEX ByteSeq
 *     headers that contain tag-length-value encoded data, where the tag
 *     and length fields are each one byte in length. The following headers
 *     fall into this category:
 *
 *     OBEXH_APP_PARAMS  Application specific parameters
 *
 *     OBEXH_AUTH_CHAL   Authentication challenge
 *
 *     OBEXH_AUTH_RESP   Authentication response
 *
 *     This structure is used by the OBEXH_ParseTagByteHeader() function to
 *     track the parser state and is always READ-ONLY to all other modules.
 */
typedef struct _ObexTlvParser {
    /* === Internal use only === */
    U8          tag;        /* "Type" byte of the T-L-V triplet */
    U8          length;     /* "Length" byte of the T-L-V tripent */
    U8          state;      /* Current processing state */
#if OBEX_TLV_BUFF_SIZE > 0
    U8          value[OBEX_TLV_BUFF_SIZE]; /* Reassembly staging area */
#endif /* OBEX_TLV_BUFF_SIZE > 0 */                                               
    U8          valuePos; /* Amount of data in "value" or indicated to app */
    U8          *headerBuff; /* Header buffer available */
    U16         headerLen;   /* Header length available */
} ObexTlvParser;
#endif /* OBEX_TLV_PARSER == XA_ENABLED */


/*---------------------------------------------------------------------------
 * ObStoreFuncTable structure
 *
 *     Table of OBEX Object Store specific functions. This table is
 *     passed down to the OBEX protocol layer in the ObexAppHandle structure
 *     when OBEX_InitAppHandle() is called. It can be allocated in ROM.
 */
struct _ObStoreFuncTable {
    OBS_Read            Read;
    OBS_Write           Write;
    OBS_GetObjectLen    GetObjectLen;
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
    OBS_ReadFlex        ReadFlex;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    OBS_SetOffset       SetOffset;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
};

#if OBEX_ROLE_SERVER == XA_ENABLED
/*--------------------------------------------------------------------------
 * ObexServerTransports structure
 *
 *     This structure contains all possible OBEX server transports that 
 *     should exist for each ObexServerApp that is allocated.  This 
 *     structure will be initialized internally by the transport.
 *
 */
typedef struct _ObexServerTransports {
    /* === Internal use only === */

    /* Ordering of client and server transports must match 
     * ObexClientTransports when OBEX_ALLOW_SERVER_TP_CONNECT
     * is enabled.
     */

#if IRDA_STACK == XA_ENABLED
    /* This field is only accessible when compiled with IRDA_STACK
     * set to XA_ENABLED.
     */
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObIrClientTransport     ObexClientIrTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
    ObIrServerTransport     ObexServerIrTrans;
#endif /* IRDA_STACK == XA_ENABLED */
        
#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
    ObIrSimpleServerTransport ObexServerIrSimpleTrans;
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

#if BT_STACK == XA_ENABLED
    /* This field is only accessible when compiled with BT_STACK
     * set to XA_ENABLED.
     */
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObBtClientTransport     ObexClientBtTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    ObBtServerTransport     ObexServerBtTrans;
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    /* This field is only accessible when compiled with TCP_STACK
     * set to XA_ENABLED.
     */
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObTcpClientTransport    ObexClientTcpTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    ObTcpServerTransport    ObexServerTcpTrans;
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    /* This field is only accessible when compiled with OEM_STACK
     * set to XA_ENABLED.
     */
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObOemClientTransport    ObexClientOemTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    ObOemServerTransport    ObexServerOemTrans;
#endif /* OEM_STACK == XA_ENABLED */

} ObexServerTransports;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 * ObexClientTransports structure
 *
 *     This structure contains all possible OBEX client transports that 
 *     might exist for an ObexClientApp that is allocated.  This 
 *     structure will be initialized internally by the transport.
 *
 */
typedef struct _ObexClientTransports {
    /* === Internal use only === */
    
    /* Ordering of client and server transports must match 
     * ObexServerTransports when OBEX_ALLOW_SERVER_TP_CONNECT
     * is enabled.
     */

#if IRDA_STACK == XA_ENABLED
    /* This field is only accessible when compiled with IRDA_STACK
     * set to XA_ENABLED.
     */
    ObIrClientTransport     ObexClientIrTrans;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObIrServerTransport     ObexServerIrTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* IRDA_STACK == XA_ENABLED */
        
#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
    ObIrSimpleClientTransport ObexClientIrSimpleTrans;
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

#if BT_STACK == XA_ENABLED
    /* This field is only accessible when compiled with BT_STACK
     * set to XA_ENABLED.
     */
    ObBtClientTransport     ObexClientBtTrans;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObBtServerTransport     ObexServerBtTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    /* This field is only accessible when compiled with TCP_STACK
     * set to XA_ENABLED.
     */
    ObTcpClientTransport    ObexClientTcpTrans;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObTcpServerTransport    ObexServerTcpTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    /* This field is only accessible when compiled with OEM_STACK
     * set to XA_ENABLED.
     */
    ObOemClientTransport    ObexClientOemTrans;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    ObOemServerTransport    ObexServerOemTrans;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* OEM_STACK == XA_ENABLED */

} ObexClientTransports;
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if OBEX_SESSION_SUPPORT == XA_ENABLED 
/*----------------------------------------------------------------------------
 * ObexSessionParms structure
 *
 *     This structure is used for indicating parsed session parameters header
 *     information to the application. The "fields" field is a bitmap used to
 *     specify which of the remaining fields in the structure are valid.
 */
struct _ObexSessionParms {
    U8          fields;         /* Bitmap of valid fields (or'd OBSP_FIELD_xxx values). */
    U8          opcode;         /* Session command opcode */
    U8          device[6];      /* Device Address */
    U8          deviceLen;      /* Device Address Length */
    U8          nonce[16];      /* Nonce */
    U8          nonceLen;       /* Nonce Length */
    U8          sessionId[16];  /* Session Id */
    U8          sequenceNum;    /* Sequence Number */
    U32         timeout;        /* Timeout */
};

#define OBSP_FIELD_DEVADDR      0x01    /* The device addres field is valid. */
#define OBSP_FIELD_NONCE        0x02    /* The nonce field is valid. */
#define OBSP_FIELD_SESSIONID    0x04    /* The session Id field is valid. */
#define OBSP_FIELD_SEQNUM       0x08    /* The sequence number field is valid. */
#define OBSP_FIELD_TIMEOUT      0x10    /* The timeout field is valid. */
#define OBSP_FIELD_OPCODE       0x20    /* The opcode field is valid. */

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*--------------------------------------------------------------------------
 * ObexAppHandle structure
 *
 *     The Obex Application handle is used by the OBEX Client and Server to
 *     manage building, parsing and transmitting OBEX headers. This structure
 *     is provided during OBEX initialization and is then referenced when
 *     most OBEX functions are called. The 'buffer' and 'length' fields
 *     must be filled in and the rest of the structure must be zero'd prior
 *     to calling Client or Server Init.
 *
 *     The macro OBEX_InitAppHandle() can be used to initialize the structure.
 */
struct _ObexAppHandle {
    /* === Internal use only === */

    /* Must be the first field */
    ListEntry               node;

    /* Object Store Function Table - should be filled in by the 
     * application layer.
     */
    const ObStoreFuncTable  *obsf;

    /* Whether End Of Body header occurred in the application headers */
    BOOL                    appHeaderIsEndBody;

    /* Application supplied buffer for building transmit headers. */
    U8                      *buffer;

    /* The length of the buffer. */
    U16                     length;

    /* Supported transport type */
    ObexTpType              tpType;  

    /* Transport connection status */
    U8                      tpStatus;  

#if OBEX_TLV_PARSER == XA_ENABLED
    /* Used when calling OBEXH_ParseTLVs() */
    ObexTlvParser           tlv;
#endif /* OBEX_TLV_PARSER == XA_ENABLED */

    U16                     txLength;
    /* OBEX Protocol Parser data structure */    
    ObexParser              parser;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    ObexSessionParms        sessionParms;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
};

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*--------------------------------------------------------------------------
 * ObexClientApp structure
 *
 *     The ObexClientApp represents the entire unit of what an OBEX client 
 *     must contain.  The ObexClientApp must be allocated and initialized by 
 *     the application, prior to it being used in any OBEX API's.  The 
 *     ObexAppHandle should be initialized first, using OBEX_InitAppHandle().  
 *     Once this is complete, OBEX_ClientInit() will complete the 
 *     initialization of the client, with OBEX internally initializing 
 *     the client context and the transport internally initializing the 
 *     client transport.
 *
 */
struct _ObexClientApp {
    /* === Internal use only === */

    /* Must be the first field */
    ObexAppHandle               handle;

    ObexClientTransports        trans;
    
    ObClientContext             client;
};
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*--------------------------------------------------------------------------
 * ObexServerApp structure
 *
 *     The ObexServerApp represents the entire unit of what an OBEX server 
 *     must contain.  The ObexServerApp must be allocated and initialized by 
 *     the application, prior to it being used in any OBEX API's.  The 
 *     ObexAppHandle should be initialized first, using OBEX_InitAppHandle().  
 *     Once this is complete, OBEX_ServerInit() will complete the 
 *     initialization of the server, with OBEX internally initializing 
 *     the server context and the transport internally initializing the 
 *     server transports.
 */
struct _ObexServerApp {
    /* === Internal use only === */

    /* Must be the first field */
    ObexAppHandle               handle;

    ObexServerTransports        trans;

    ObServerContext             server;
};
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*--------------------------------------------------------------------------
 * ObexClientSession structure
 *
 *     The ObexClientSession represents the entire unit of what an OBEX Client
 *     session must contain.  The ObexClientSession must be allocated by the
 *     application. Once this structure has been allocated and a session has 
 *     been created, the OBEX Client will track the memory for this session
 *     to check values as operations are performed.  This memory must exist
 *     in non-volatile memory until the session is closed.
 */
struct _ObexClientSession {
    /* First eight fields must exist in this order */

    /* Client Session information */
    U8                  clientDevAddr[6];   /* Local Device Address */
    U8                  clientNonce[16];    /* Local Nonce */
    U8                  clientNonceLen;     /* Local Nonce Length */

    /* Server Session information */
    U8                  serverDevAddr[6];   /* Remote Device Address */
    U8                  serverNonce[16];    /* Remote Nonce */
    U8                  serverNonceLen;     /* Remote Session Nonce Length */

    /* Session Suspend Timeout */
    U32                 timeout;            /* Timeout for this session */

    /* Transport type */
    ObexTpType          tpType;             /* OBEX transport type */

    /* Session Sequence Number.  For the client this is the sequence 
     * number of the last packet sent.  For the server this is the next
     * sequence number expected.
     */
    U8                  seqNum;             /* Sequence Number for Current Packet */
    U8                  nextSeqNum;         /* Sequence Number for Next Packet */

    U16                 obPacketSize;       /* OBEX Packet Size */

    /* Save the initial OBEX command packet information */
    U8                  startSeqNum;        /* Sequence number of first command packet in the operation */
    ObexOpcode          curOpcode;          /* Opcode in progress */ 
    U32                 param2;             /* Parameter for first command packet */

    U8                  currentOperation;   /* Client callback for operation in progress */
    U32                 ackedObjBytes;      /* Acknowledged bytes into the current object */
    U32                 unackedObjBytes;    /* Unacknowledged bytes for the current object */
    U16                 unackedHeaderLen;   /* Unacknowledged bytes for the application headers */

    U32                 ourTimeout;         /* Client suggested timeout */
};

/*--------------------------------------------------------------------------
 * ObexServerSession structure
 *
 *     The ObexServerSession represents the entire unit of what an OBEX Server
 *     session must contain.  The ObexServerSession must be allocated by the
 *     application. Once this structure has been allocated and a session has 
 *     been created, the OBEX Server will track the memory for this session
 *     to check values as operations are performed.  This memory must exist
 *     in non-volatile memory until the session is closed.
 */
struct _ObexServerSession {
    
    /* First eight fields must exist in this order */

    /* Client Session information */
    U8                  clientDevAddr[6];   /* Remote Device Address */
    U8                  clientNonce[16];    /* Remote Nonce */
    U8                  clientNonceLen;     /* Remote Session Nonce Length */

    /* Server Session information */
    U8                  serverDevAddr[6];   /* Local Device Address */
    U8                  serverNonce[16];    /* Local Nonce */
    U8                  serverNonceLen;     /* Local Nonce Length */

    /* Session Suspend Timeout */
    U32                 timeout;            /* Negotiated timeout */

    /* Transport type */
    ObexTpType          tpType;             /* OBEX transport type */
    
    /* Session Sequence Numbers. */
    /* This is the next sequence number expected from the client. */
    U8                  seqNum;
    /* This is the last sequence number received from the client. */
    U8                  lastSeqNum;

    U16                 obPacketSize;       /* OBEX Packet Size */

    /* Save the OBEX packet information including application headers */
    U8                  currentOperation;   /* Server operation in progress */
    U32                 ackedObjBytes;      /* Acknowledged bytes into the current object */
    U32                 unackedObjBytes;    /* Unacknowledged bytes for the current object */
    U16                 unackedHeaderLen;   /* Unacknowledged bytes into the application headers */

    ObexOpcode          curOpcode;          /* Opcode in progress (Put or Get only) */ 
    U8                  resp;               /* Last response code sent. */
#if OBEX_SERVER_CONS_SIZE > 0
    U8                  connId[OBEX_SERVER_CONS_SIZE];
    U8                  activeConnId;
    U16                 unackedProtoLen;    /* Unacknowledged bytes into the protocol headers */
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    U32                 ourTimeout;         /* Server suggested timeout */
};

/*--------------------------------------------------------------------------
 * ObexSessionResumeParms structure
 *
 *     This structure is provided by the application when resuming a session.
 *     If the session was suspended unexpectedly and the session suspend event
 *     indicated parameters in the "u.suspended" structure, those parameters
 *     (or recreations thereof) are provided during session resume using this
 *     structure.
 */
typedef struct _ObexSessionResumeParms {
    /* Object store handle of restored object. This handle does not need
     * to match the handle provided in the "u.suspended" parameters.
     * However, it must refer to the same object.
     */
    void           *obsh;

    /* Length of preserved header buffer. This value must be the same as
     * that provided in the "u.suspended" parameters.
     */
    U16             headerLen;
    
    /* A pointer to the preserved header buffer. This pointer does not
     * need to match the pointer provided in the "u.suspended" parameters.
     * However, it must point to an exact copy of the suspended header buffer.
     */
    U8             *headerBuff;

} ObexSessionResumeParms;

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*--------------------------------------------------------------------------
 * ObexConnection structure
 *
 *     Used by the OBEX server to track available connections.
 *
 *     Note: this structure is available only when OBEX_SERVER_CONS_SIZE
 *     is greater than 0. 
 */
struct _ObexConnection
{
    /* Contents of Target header for this server connection. Must be filled
     * in by the application before calling OBEX_ServerRegisterTarget.
     */
    const U8   *target;

    /* Length of "target" buffer. Must be filled in by the application before
     * calling OBEX_ServerRegisterTarget.
     */
    U8          targetLen;

    /* Filled in automatically by the stack when a connection is assigned
     * to the target.
     */
    U32         connId;
};

#if BT_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * ObexBtTarget structure
 *
 *     Defines device address and service information required for a 
 *     server to locate a remote Bluetooth OBEX service. The first
 *     3 parameters are identical to those of the same names used with
 *     ME_CreateLink().
 */
struct _ObexBtTarget {
    BD_ADDR         addr;
    BtPageScanInfo  psi;
    BOOL            allowRoleChange;
    const U8       *sdpQuery;
    U16             sdpQueryLen;
    SdpQueryType    sdpQueryType;
};
#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * ObexIrTarget structure
 *
 *     Defines device address and service information required for a 
 *     server to locate a remote IrDA OBEX service. The addr parameter
 *     can be zero for auto-discovery / point and shoot behavior.  The
 *     iasQuery and iasQueryLen parameters must be filled in with the 
 *     IAS query that is desired.
 */
struct _ObexIrTarget {
    /* Remote IrDA device address to connect to */
    IrDeviceAddr    addr;

#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
    IrSmpConnectMode    smpConnMode;
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

    /* IAS Query Information */
    const U8       *iasQuery;
    /* IAS Query Length */
    U8              iasQueryLen;
};
#endif /* IRDA_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * ObexTcpTarget structure
 *
 *     Defines device address and service information required for a 
 *     server to locate a remote TCP/IP OBEX service. The addr parameter
 *     is a 32-bit IP address in network order.
 */
struct _ObexTcpTarget {
    U32             addr;
    U16             customPort;
};
#endif /* TCP_STACK == XA_ENABLED */


#if OEM_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * ObexOemTarget structure
 *
 *     The definition of an OEM transport target is defined in obxoemstk.h
 */
#endif /* OEM_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * ObexTpAddr structure
 *     
 *     Defines a transport-specific addressing information. This structure,
 *     and all of the fields used in it must remain valid for the duration
 *     of the OBEX_ClientTpConnect() call. This means that if the call returns
 *     OB_STATUS_PENDING, the structure must be maintained until the client
 *     application callback receives a completion event.
 */
typedef struct _ObexTpAddr {
    /* Specifies which address in the union is being provided. */
    ObexTpType  type;               

    /* This union contains entries for different transport types.
     * Add new transport types here if necessary.
     */
    union {

#if BT_STACK == XA_ENABLED
        ObexBtTarget    bt;
#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
        ObexIrTarget    ir;
#endif /* IRDA_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
        ObexTcpTarget   tcp;
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
        ObexOemTarget  *oem;
#endif /* OEM_STACK == XA_ENABLED */

    } proto;

} ObexTpAddr;

/*---------------------------------------------------------------------------
 * ObexTpConnInfo structure
 *     
 *     Contains information about the OBEX transport connection. Some info
 *     is general purpose while other information is transport type specific.
 *     This structure is used with the OBEX_GetTpConnInfo() function.
 */
struct _ObexTpConnInfo {
    U16             size;           /* Size of ObexTpConnInfo structure */
    ObexTpType      tpType;
    U16             maxPduSize;
    U16             minRxBuffs;
    U8             *devAddr;

    /* This field is valid only if tpType == OBEX_TP_BLUETOOTH */
#if BT_STACK == XA_ENABLED
    BtRemoteDevice  *remDev;
#endif /* BT_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    /* Add any fields that are valid only if tpType == OBEX_TP_OEM */
#endif /* OEM_STACK == XA_ENABLED */
};

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * OBEX_Init()
 *
 *     Initializes the OBEX component's internal data.  This must be
 *     the first OBEX function called by the application layer, or if 
 *     multiple OBEX applications exist, this function should be called
 *     at system startup (see XA_LOAD_LIST in config.h).
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL OBEX_Init(void);

/*---------------------------------------------------------------------------
 * OBEX_InitAppHandle()
 *
 *     Initializes an ObexAppHandle structure prior to OBEX Client or Server
 *     registration.
 *
 * Parameters:
 *     ObexApp - The application handle structure to initialize.
 *
 *     HeaderBuff - The header buffer to be associated with the AppHandle.
 *         It is used by the OBEX layer to build outgoing headers.
 *
 *     HBLen - The length of the HeaderBuff parameter.
 *
 *     obsf -  The Object Store Function Table.  This function table should 
 *             be allocated by the application and persistent until all 
 *             clients and servers that are using this structure have been
 *             deinitialized.
 */
void OBEX_InitAppHandle(ObexAppHandle *ObexApp, U8 *HeaderBuff, U16 HBLen, 
                        const ObStoreFuncTable *obsf);

#define OBEX_InitAppHandle(_AH, _HB, _HBL, _OBSF)    do {       \
            OS_MemSet((U8 *)(_AH), 0, sizeof(ObexAppHandle));   \
            (_AH)->buffer = (_HB); (_AH)->length = (_HBL);      \
            (_AH)->obsf = _OBSF; } while (0)

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ClientInit()
 *
 *     Initializes the OBEX client.  Each OBEX client must be setup to connect
 *     via one transport only.  If multiple, simultaneous, transport 
 *     connections are required, multiple OBEX clients must be allocated.
 *
 * Parameters:
 *     ObexApp - The OBEX client instance.  This structure is owned by
 *         OBEX until OBEX_ClientDeinit is called.
 *
 *     ClientCb - Application's Client Callback function.
 *
 *     Protocols - Transport protocol(s) that the client should initialize. A 
 *         client can support multiple transport protocols, but only 
 *         transport can be active at a time. 
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Initialized successfully
 *
 *     OB_STATUS_FAILED - Unable to initialize the OBEX Components.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid or the client is
 *          already registered.
 */
ObStatus OBEX_ClientInit(ObexClientApp *ObexApp, ObClientAppCb ClientCb,
                         ObexTpType Protocols);
                         
#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ClientDeinit()
 *
 *     Deinitializes the OBEX client. Note that it is illegal to call
 *     this function during a callback event indication.  The memory used
 *     for this OBEX client may now be released, if necessary.
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The OBEX Client instance provided during init.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Successfully deinitialized.
 * 
 *     OB_STATUS_BUSY - Unable to deinitialize because the client is active.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientDeinit(ObexClientApp *ObexApp);

#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX_ClientTpConnect()
 *
 *     Attempts to create a transport connection to an OBEX Server.
 *     Completion or failure of the operation is reported by a call to  
 *     the application's ObClientAppCb with an appropriate event. The events
 *     generated as a result of this function returning pending are either
 *     of OBCE_CONNECTED, OBCE_DISCONNECT, OBCE_DISCOVERY_FAILED or 
 *     OBCE_NO_SERVICE_FOUND.
 * 
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Target - The device address of the device to connect to.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_SUCCESS - Operation is successful. The ObClientAppCb
 *         callback will be called before this function returns.
 *
 *     OB_STATUS_INVALID_PARM - A function parameter was invalid.
 *
 *     OB_STATUS_BUSY - The client is already starting up a connection.
 */
ObStatus OBEX_ClientTpConnect(ObexClientApp *ObexApp, ObexTpAddr *Target);

/*---------------------------------------------------------------------------
 * OBEX_ClientTpDisconnect()
 *
 *     Requests that the transport connection be disconnected.  The Client can
 *     disconnect the transport connection regardless if it was initiated by 
 *     the Client or Server.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Force - (Used by IrDA transport only). When true, indicates 
 *         that the OBEX client should disconnect the IrLAP link 
 *         even if another client or server is still using it.  When 
 *         IR_LMP_DISCONNECT is disabled, disconnections will fail if
 *         other IrLMP connections exist, unless the force parameter 
 *         is set.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via a call to the ObClientAppCb
 *         callback.
 *
 *     OB_STATUS_SUCCESS - Operation was successfully performed. The
 *         connection has been disconnected. This value is not returned
 *         when running OBEX over the XTNDAccess Blue or IrDA SDK's.
 *
 *     OB_STATUS_FAILED - Operation failed.
 *
 *     OB_STATUS_NO_CONNECT - Operation failed because there is
 *         no client connection.
 */
ObStatus OBEX_ClientTpDisconnect(ObexClientApp *ObexApp, BOOL Force);

/*---------------------------------------------------------------------------
 * OBEX_ClientGetAbortReason()
 *
 *     Returns the reason for the current abort indication. This call is
 *     only valid during calls to the ObClientAppCb callback with the event
 *     OBCE_ABORTED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     ObexAbortReason - Reason for abort.
 */
ObexAbortReason OBEX_ClientGetAbortReason(ObexClientApp *ObexApp);

#define OBEX_ClientGetAbortReason(_OBEXAPP) ((_OBEXAPP)->client.status)

#if IRDA_STACK == XA_ENABLED
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * OBEX_ClientFastConEnable()
 *
 *     Enables FastConnect discovery for the transport layer. After this 
 *     function returns successfully, subsequent transport discoveries are 
 *     performed according to the Fast Connect application note from IrDA. 
 *     FastConnect can be disabled by calling OBEX_ClientFastConDisable().
 *
 * Requires:
 *     IRDA_STACK set to XA_ENABLED.
 *
 * Parameters:
 *     initiator - TRUE: discoveries are performed without regard for
 *         media busy. Only fixed devices should set this to TRUE
 *         as it can block or negatively impact the IR space. FALSE: 
 *         Fast Connect logic will be used but media busy rules are 
 *         honored.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Subsequent transport discoveries will use Fast 
 *         Connect.
 *
 *     OB_STATUS_FAILED - Failure to set Fast Connect mode.
 *
 *     OB_STATUS_NOT_SUPPORTED - Fast Connect is not supported by the transport.
 */
ObStatus OBEX_ClientFastConEnable(BOOL initiator);
#define OBEX_ClientFastConEnable(_INITIATOR) IRSTACK_ClientFastConEnable(_INITIATOR)

/*---------------------------------------------------------------------------
 * OBEX_ClientFastConDisable()
 *
 *     Disables FastConnect transport discovery. If a transport discovery is 
 *     already in progress, it will be allowed to complete.
 *
 * Requires:
 *     IRDA_STACK set to XA_ENABLED.
 */
void OBEX_ClientFastConDisable(void);
#define OBEX_ClientFastConDisable() IRSTACK_ClientFastConDisable()

#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
/*---------------------------------------------------------------------------
 * OBEX_ClientIrSimpleEnable()
 *
 *     Enables IrSimple discovery for the transport layer. After this 
 *     function returns successfully, subsequent transport discoveries are 
 *     performed according to the IrSimple specification from IrDA. 
 *     IrSimple can be disabled by calling OBEX_ClientIrSimpleDisable().
 *
 * Requires:
 *     IRDA_STACK set to XA_ENABLED.
 *     IR_SIMPLE set to IR_ENABLED
 *
 * Parameters:
 *     mode - IrSimple mode to use, uni-directional or bi-directional.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Subsequent transport discoveries will use IrSimple.
 *
 *     OB_STATUS_FAILED - Failure to set IrSimple mode.
 *
 *     OB_STATUS_NOT_SUPPORTED - IrSimple is not supported by the transport.
 */
ObStatus OBEX_ClientIrSimpleEnable(ObexClientApp *ObexApp, IrSmpMode mode);
#define OBEX_ClientIrSimpleEnable(_OBEXAPP, _MODE) \
    IRSTACK_ClientSimpleEnable(&(_OBEXAPP)->trans.ObexClientIrSimpleTrans, _MODE)

/*---------------------------------------------------------------------------
 * OBEX_ClientIrSimpleDisable()
 *
 *     Disables IrSimple transport discovery. If a transport discovery is 
 *     already in progress, it will be allowed to complete.
 *
 * Requires:
 *     IRDA_STACK set to XA_ENABLED.
 *     IR_SIMPLE set to IR_ENABLED
 */
void OBEX_ClientIrSimpleDisable(void);
#define OBEX_ClientIrSimpleDisable() IRSTACK_ClientSimpleDisable()

#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
#endif /* IRDA_STACK == XA_ENABLED */

#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX_EnumTransports()
 *
 *     Returns the set of currently supported OBEX transport layers.
 *
 * Returns:
 *     ObexTpType - Bitwise combination of OBEX_TP_ values.
 */
ObexTpType OBEX_EnumTransports(void);

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerInit()
 *
 *     Initializes the OBEX Server.  Each OBEX server can be setup to accept
 *     connections from multiple, simultaneous transports, but there can only 
 *     be one active transport connection per OBEX server.  If multiple, 
 *     simultaneous, transport connections are required, multiple OBEX servers
 *     must be allocated.
 *
 * Parameters:
 *     ObexApp - The OBEX Server instance.  This structure is owned by
 *         OBEX until OBEX_ServerDeinit is called.
 *
 *     ServerCb - Application's server callback function.
 *
 *     Protocols - Transport protocol(s) server should initialize.  A server
 *         can support multiple transport protocols, since it doesn't
 *         inherently know what type of transport the client will 
 *         choose.  Only one transport can be active at a time.
 *                 
 * Returns:
 *     OB_STATUS_SUCCESS - Initialized successfully
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid or the server is
 *          already registered.
 *
 *     OB_STATUS_FAILED - Unable to initialize the OBEX Components.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerInit(ObexServerApp *ObexApp, ObServerAppCb ServerCb,
                         ObexTpType Protocols);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerDeinit()
 *
 *     Deinitializes the OBEX Server. This function automatically deregisters
 *     the Telecom parameters if necessary.  The memory used for this OBEX 
 *     server may now be released, if necessary.
 *
 * It is illegal to call this function during an event indication.
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The OBEX server instance provided during init.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The parser was deinitialized.
 *
 *     OB_STATUS_BUSY - An Operation is in progress.
 *
 *     OB_STATUS_FAILED - The transport layer is active.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerDeinit(ObexServerApp *ObexApp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerTpConnect()
 *
 *     Attempts to create a transport connection to an OBEX Client.
 *     Completion or failure of the operation is reported by a call to  
 *     the application's ObServerAppCb with an appropriate event. The events
 *     generated as a result of this function returning pending are either
 *     of OBSE_CONNECTED, OBSE_DISCONNECT, OBSE_DISCOVERY_FAILED, or 
 *     OBSE_NO_SERVICE_FOUND.
 * 
 * Requires:
 *     OBEX_ALLOW_SERVER_TP_CONNECT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Target - The device address of the device to connect to.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObServerAppCb callback.
 *
 *     OB_STATUS_SUCCESS - Operation is successful. The ObServerAppCb
 *         callback will be called before this function returns.
 *
 *     OB_STATUS_INVALID_PARM - A function parameter was invalid.
 *
 *     OB_STATUS_BUSY - The client is already starting up a connection.
 */
ObStatus OBEX_ServerTpConnect(ObexServerApp *ObexApp, ObexTpAddr *Target);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerTpDisconnect()
 *
 *     Requests that the transport connection be disconnected.  The Server can
 *     disconnect the transport connection regardless if it was initiated by 
 *     the Client or Server.  If this function returns success, there will
 *     have been a reentrant call generated for the OBSE_DISCONNECT event.   
 *     This is the only case where a reentrant call will be generated in 
 *     response to an immediate transport disconnection.
 *
 * Requires:
 *     OBEX_ALLOW_SERVER_TP_DISCONNECT set to XA_ENABLED.  Notice that
 *     enabling OBEX_ALLOW_SERVER_TP_CONNECT will automatically enable
 *     OBEX_ALLOW_SERVER_TP_DISONNECT.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Force - (Used by IrDA transport only). When true, indicates 
 *         that the OBEX server should disconnect the IrLAP link 
 *         even if another client or server is still using it.  When 
 *         IR_LMP_DISCONNECT is disabled, disconnections will fail if
 *         other IrLMP connections exist, unless the force parameter 
 *         is set.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via a call to the ObServerAppCb
 *         callback.
 *
 *     OB_STATUS_SUCCESS - Operation was successfully performed. The
 *         connection has been disconnected. This value is not returned
 *         when running OBEX over the XTNDAccess Blue or IrDA SDK's.
 *
 *     OB_STATUS_FAILED - Operation failed.
 *
 *     OB_STATUS_NO_CONNECT - Operation failed because there is
 *         no client connection.
 */
ObStatus OBEX_ServerTpDisconnect(ObexServerApp *ObexApp, BOOL Force);
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerGetTcpPort()
 *
 *     Sets the TCP Port for the TCP Server.
 *
 * Requires:
 *     TCP_STACK set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - OBEX Server App 
 *
 * Returns:
 *     Server TCP port
 */
U16 OBEX_ServerGetTcpPort(ObexServerApp *ObexApp); 
#define OBEX_ServerGetTcpPort(_OBEXAPP) \
            ((_OBEXAPP)->trans.ObexServerTcpTrans.serverPort)
#endif /* TCP_STACK == XA_ENABLED */

#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
/*---------------------------------------------------------------------------
 * OBEX_ServerIrSimpleRetransmit()
 *
 *     Informs the IrSMP layer that a retransmission is required of the 
 *     data that has been previously received.  The retransmission will
 *     occur from the sequence number provided all the way to the end
 *     of the SMP block.
 *
 * Requires:
 *     IRDA_STACK set to XA_ENABLED.
 *     IR_SIMPLE set to IR_ENABLED
 *
 * Parameters:
 *     mode - IrSimple mode to use, uni-directional or bi-directional.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Subsequent transport discoveries will use IrSimple.
 *
 *     OB_STATUS_FAILED - Failure to set IrSimple mode.
 *
 *     OB_STATUS_NOT_SUPPORTED - IrSimple is not supported by the transport.
 */
ObStatus OBEX_ServerIrSimpleRetransmit(ObexServerApp *ObexApp, IrSmpSeqNum SeqNum);
#define OBEX_ServerIrSimpleRetransmit(_OBEXAPP, _SEQNUM) \
    IRSTACK_ServerSimpleRetransmit(&(_OBEXAPP)->trans.ObexServerIrSimpleTrans, _SEQNUM)


/*---------------------------------------------------------------------------
 * OBEX_ServerSetSimpleBlockSize()
 *
 *     Sets the IrSMP Maximum Block size for the OBEX Server. The value 
 *     passed to this function will be used to advertise the maximum SDU size 
 *     when establishing a SMP connection.  For the OBEX Server, this
 *     is the maximum receive size.  This value is negotiated between 
 *     the two devices and may change once the IrSMP connection is 
 *     established.   
 *
 * Requires: IR_SIMPLE enabled.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Size - the size of the IrSMP SDU.  Must be a valid block size as 
 *         specified by the IrSmpBlockSize type.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - operation competed successfully.
 *
 *     OB_STATUS_FAILED - operation failed (error checking only) either because
 *         the IrConnect structure was not bound or the size field contained an
 *         invalid number.
 */
ObStatus OBEX_ServerSetSimpleBlockSize(ObexServerApp *ObexApp, IrSmpBlockSize Size);
#define OBEX_ServerSetSimpleBlockSize(_OBEXAPP, _SIZE) \
    IRSTACK_ServerSetSimpleBlockSize(&(_OBEXAPP)->trans.ObexServerIrSimpleTrans, _SIZE)
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

/*---------------------------------------------------------------------------
 * OBEX_ServerGetPathFlags()
 *
 *     Retrieve the flags associated with the SetPath request. This
 *     function is valid during an OBSE_SET_PATH_START indication.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     Flags sent by remote client.
 */
ObexSetPathFlags OBEX_ServerGetPathFlags(ObexServerApp *ObexApp);

#define OBEX_ServerGetPathFlags(_OBEXAPP)   ((_OBEXAPP)->handle.parser.spFlags)

#if OBEX_SERVER_CONS_SIZE > 0
/*---------------------------------------------------------------------------
 * OBEX_ServerRegisterTarget()
 *
 *     Allows the server application to specify a target header
 *     used for by OBEX to establish a directed connection.
 *
 * Requires:
 *     OBEX_SERVER_CONS_SIZE greater than 0.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Conn - pointer to initialized OBEX connection structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Target header accepted
 *
 *     OB_STATUS_FAILED - Insufficient conn table space.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerRegisterTarget(ObexServerApp  *ObexApp, 
                                   ObexConnection *Conn);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerDeregisterTarget()
 *
 *     Removes a registered target header.
 *                
 * Requires:
 *     OBEX_SERVER_CONS_SIZE greater than 0,
 *     and OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Conn - pointer to registered OBEX connection structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Target header removed.
 *
 *     OB_STATUS_FAILED  - Target header not found or conn busy.
 *
 *     OB_STATUS_BUSY - Obex connection exists to this target.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerDeregisterTarget(ObexServerApp  *ObexApp, 
                                     ObexConnection *Conn);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX_ServerGetConnInfo()
 *
 *     Returns a pointer to the currently active OBEX Connection.
 *     This function is valid after the OBSE_XXX_START indication
 *     and until the OBSE_COMPLETE or OBSE_ABORTED indication.
 *
 * Requires:
 *     OBEX_SERVER_CONS_SIZE greater than 0.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     The ObexConnection or NULL if the current connection is for
 *     the default/inbox.
 */
ObexConnection *OBEX_ServerGetConnInfo(ObexServerApp *ObexApp);
#endif  /* OBEX_SERVER_CONS_SIZE > 0 */

#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Section: Command/Response Functions
 *
 ****************************************************************************/

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ClientConnect()
 *
 *     Initiates the OBEX "Connect" operation. OBEX_ClientTpConnect() must
 *     be completed first to establish a transport connection.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because the client is
 *         currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientConnect(ObexClientApp *ObexApp);

/*---------------------------------------------------------------------------
 * OBEX_ClientDisconnect()
 *
 *     Initiates the OBEX "Disconnect" operation.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via a call to the
 *         ObClientAppCb callback.
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
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientDisconnect(ObexClientApp *ObexApp);
        
#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
/*---------------------------------------------------------------------------
 * OBEX_ClientSimpleConnect()
 *
 *     Initiates the OBEX "Connect" operation for the IrSimple protocol.  In
 *     this case the OBEX Connect and transport connection are performed 
 *     simultaneously, with the SNRM containing the OBEX Connect.  The 
 *     normal sequence of OBEX_ClientTpConnect() and OBEX_ClientConnect() are
 *     not used for IrSimple transmissions, otherwise, the OBEX Connect
 *     would occur later.
 *
 * Requires:
 *     IR_SIMPLE set to IR_ENABLED
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Target - The device address of the device to connect to.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because the client is
 *         currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientSimpleConnect(ObexClientApp *ObexApp, ObexTpAddr *Target);

/*---------------------------------------------------------------------------
 * OBEX_ClientSimpleDisconnect()
 *
 *     Initiates the OBEX "Disconnect" operation for the IrSimple protocol.  In
 *     this case the OBEX Disconnect and transport disconnection are performed 
 *     simultaneously, with the DISC containing the OBEX Disconnect.  The 
 *     normal sequence of OBEX_ClientDisconnect() and OBEX_ClientTpDisconnect()
 *     are not used for IrSimple transmissions, otherwise, the OBEX Disconnect
 *     would occur too soon.
 *
 * Requires:
 *     IR_SIMPLE set to IR_ENABLED
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via a call to the
 *         ObClientAppCb callback.
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
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientSimpleDisconnect(ObexClientApp *ObexApp);

/*---------------------------------------------------------------------------
 * OBEX_ClientSetSimpleBlockSize()
 *
 *     Sets the IrSMP Maximum Block size for the OBEX Client. The value 
 *     passed to this function will be used to advertise the maximum SDU size 
 *     when establishing a SMP connection.  For the OBEX Client, this
 *     is the maximum transmit size.  This value is negotiated between 
 *     the two devices and may change once the IrSMP connection is 
 *     established.   
 *
 * Requires: IR_SIMPLE enabled.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Size - the size of the IrSMP SDU.  Must be a valid block size as 
 *         specified by the IrSmpBlockSize type.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - operation competed successfully.
 *
 *     OB_STATUS_FAILED - operation failed (error checking only) either because
 *         the IrConnect structure was not bound or the size field contained an
 *         invalid number.
 */
ObStatus OBEX_ClientSetSimpleBlockSize(ObexClientApp *ObexApp, IrSmpBlockSize Size);
#define OBEX_ClientSetSimpleBlockSize(_OBEXAPP, _SIZE) \
    IRSTACK_ClientSetSimpleBlockSize(&(_OBEXAPP)->trans.ObexClientIrSimpleTrans, _SIZE)
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */
        
/*---------------------------------------------------------------------------
 * OBEX_ClientPut()
 *
 *     Initiates the OBEX "Put" operation to send an object to the remote
 *     server. OBEX_ClientConnect() must be executed first for connection-
 *     based transports. To perform a Put-Delete operation, provide an
 *     object store handle of zero and enable OBEX_PUT_DELETE support.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     obs - Handle of Object store item to Put.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_FAILED - Operation was not started because
 *         of an underlying transport problem.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientPut(ObexClientApp *ObexApp, void *Obsh);

/*---------------------------------------------------------------------------
 * OBEX_ClientGet()
 *
 *     Initiates the OBEX "Get" operation to retrieve an object from
 *     the remote server.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     obs - Handle of Object store item to Get.
 *
 *     moreHeaders - Signifies whether or not more OBEX headers will follow
 *         in subsequent packets in order to further describe the item being
 *         requested.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
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
 *
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientGet(ObexClientApp *ObexApp, void *obs, 
                        BOOL moreHeaders);
         
/*---------------------------------------------------------------------------
 * OBEX_ClientSetPath()
 *
 *     Performs the OBEX "SetPath" operation. 
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     pathFlags - SetPath flags (ex. OSPF_BACKUP).
 *               
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
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
 *     OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ClientSetPath(ObexClientApp *ObexApp, U8 pathFlags);

/*---------------------------------------------------------------------------
 * OBEX_ClientAbort()
 *
 *     Aborts a PUT or GET operation.  Aborting a single 
 *     request/response operation (CONNECT, DISCONNECT, and SETPATH) 
 *     or while no operation is in progress is not allowed.  The
 *     client abort packet will be sent immediately, if possible. 
 *
 *     No OBCE_COMPLETE will be generated for this function.  Instead,
 *     the client event returned in response to this function will
 *     indicate the status of the operation in progress (either
 *     OBCE_COMPLETE (if our Abort occurred too late) or
 *     OBCE_ABORTED (the usual behavior))
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Abort operation was successful.
 *
 *     OB_STATUS_FAILED - Abort operation failed.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ClientAbort(ObexClientApp *ObexApp);

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ClientSendCommand()
 *
 *     Tell the client to continue sending the requested command. This 
 *     function must be called once for every OBCE_SEND_COMMAND event that
 *     is received by the client callback. The function may be called in the
 *     context of the OBCE_SEND_COMMAND event, or at a later time if 
 *     necessary. The OBCE_SEND_COMMAND event is delivered only during PUT
 *     and GET operations.
 *
 * Requires:
 *     OBEX_PACKET_FLOW_CONTROL set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 */
void OBEX_ClientSendCommand(ObexClientApp *ObexApp);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerAbort()
 *
 *     Abort the current server operation. An operation must be in 
 *     progress in order to be aborted.
 *
 * Parameters: 
 *     ObexApp - The Application Handle provided during init.
 *
 *     Resp - The response code to send to the client. This must be one of
 *         the failure OBRC_ codes found in obexerr.h.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Abort operation was successful.
 *
 *     OB_STATUS_FAILED - Abort operation failed.
 *
 *     OB_STATUS_NO_CONNECT - No transport connection exists.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ServerAbort(ObexServerApp *ObexApp, ObexRespCode Resp);

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ForceServerAbort()
 *
 *     Force abort the current server operation. This fucntion was added to 
 *     deal with the call to server abort from the goep security callback in the 
*      context of the OBSE_DISCONNECT , OBSE_COMPLETE and OBSE_ABORTED event.  
 *
 * Parameters: 
 *     ObexApp - The Application Handle provided during init.
 *
 *     Resp - The response code to send to the client. This must be one of
 *         the failure OBRC_ codes found in obexerr.h.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Abort operation was successful.
 *
 *     OB_STATUS_FAILED - Abort operation failed.
 *
 *     OB_STATUS_NO_CONNECT - No transport connection exists.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ForceServerAbort(ObexServerApp *ObexApp, ObexRespCode Resp);

#endif /*TI_CHANGES == XA_ENABLED*/
/*---------------------------------------------------------------------------
 * OBEX_ServerAccept()
 *
 *     Accepts the current server operation. This function is called to
 *     provide an object store handle when an OBSE_PROVIDE_OBJECT event
 *     is received.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Obsh - The handle of an object store item which meets the client's
 *         request.
 */
void OBEX_ServerAccept(ObexServerApp *ObexApp, void *Obsh);


#if OBEX_BODYLESS_GET == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ServerAcceptNoObject()
 *---------------------------------------------------------------------------
 *  Accepts the current operation but does not specify an object
 *  handle. Used in a GET operation which results in no later Body/
 *  End-of-Body headers being returned to the client.
 *
 * Requires:
 *     OBEX_BODYLESS_GET set to XA_ENABLED
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 */
void OBEX_ServerAcceptNoObject(ObexServerApp *ObexApp);
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerSendResponse()
 *
 *     Tell the server to respond to the received request. This function
 *     must be called once for every OBSE_SEND_RESPONSE event that is 
 *     received by the server callback. The function may be called in the
 *     context of the event, or at a later time if necessary.
 *
 * Requires:
 *     OBEX_PACKET_FLOW_CONTROL set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 */
void OBEX_ServerSendResponse(ObexServerApp *ObexApp);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerGetIasLsap()
 *
 *     This function provides the IAS LSAP value for the IrDA
 *     transport.  The application is free to register as many
 *     services as necessary over this LSAP, however, only one 
 *     service can be connected to this LSAP at a time.
 *
 *     OBEX_ServerInit must be called prior to checking this value,
 *     otherwise the result value must be considered invalid.
 *
 * Requires:
 *     IRDA_STACK set to enabled
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Lsap - The returned lsap value.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - LSAP successfully retrieved.
 *
 *     OB_STATUS_FAILED - LSAP retrieval failed.  Server may not be
 *         initialized and bound to the IrDA stack yet.
 */
ObStatus OBEX_ServerGetIasLsap(ObexServerApp *ObexApp, U8 *Lsap);
#define OBEX_ServerGetIasLsap( _OBEXAPP, _LSAP ) \
            IRSTACK_ServerGetIasLsap(&((_OBEXAPP)->trans.ObexServerIrTrans), (_LSAP))
#endif /* IRDA_STACK == XA_ENABLED */

#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED

#if (IRDA_STACK == XA_ENABLED) && (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * OBEX_ClientGetIasLsap()
 *
 *     This function provides the IAS LSAP value for the IrDA
 *     transport.  The application is free to register as many
 *     services as necessary over this LSAP, however, only one 
 *     service can be connected to this LSAP at a time.
 *
 *     OBEX_ClientInit must be called prior to checking this value,
 *     otherwise the result value must be considered invalid.
 *
 * Requires:
 *     IRDA_STACK and OBEX_ALLOW_SERVER_TP_CONNECT set to enabled
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Lsap - The returned lsap value.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - LSAP successfully retrieved.
 *
 *     OB_STATUS_FAILED - LSAP retrieval failed.  Server may not be
 *         initialized and bound to the IrDA stack yet.
 */
ObStatus OBEX_ClientGetIasLsap(ObexClientApp *ObexApp, U8 *Lsap);
#define OBEX_ClientGetIasLsap( _OBEXAPP, _LSAP ) \
            IRSTACK_ServerGetIasLsap(&((_OBEXAPP)->trans.ObexServerIrTrans), (_LSAP))
#endif /* (IRDA_STACK == XA_ENABLED) && (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_IsIrLapConnected()
 *
 *     Returns the status of the IrLAP connection.  Can be used during the
 *     OBCE_DISCONNECT and OBSE_DISCONNECT events to distinguish between 
 *     an IrLMP service disconnection and an IrLAP transport disconnection.
 *
 * Requires:
 *     IRDA_STACK set to XA_ENABLED.
 *
 * Returns:
 *     TRUE - IrLAP connection is still up.
 *
 *     FALSE - No IrLAP connection exists.
 */
BOOL OBEX_IsIrLapConnected(void);
#define OBEX_IsIrLapConnected() IRSTACK_IsIrLapConnected()
#endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX_GetTpConnInfo()
 *
 *     Retrieves OBEX transport layer connection information.  This function
 *     can be called when a transport connection is active to retrieve
 *     connection specific information.  It should be used in conjunction with
 *     the receive flow control API to retrieve the minimum amount of
 *     application storage space (low water mark) used when deciding to pause
 *     and resume data flow.
 *
 * Parameters:
 *     ObexApp - Application context handle provided during init.
 *                
 *     TpConnInfo - Pointer to the structure which receives the connection
 *         information. The "size" parameter must be initialized to the
 *         sizeof() the structure.
 *
 * Returns:
 *     TRUE - The structure was successfully completed.
 *     FALSE - The transport is not connected.
 */
BOOL OBEX_GetTpConnInfo(ObexAppHandle *ObexApp, ObexTpConnInfo *TpConnInfo);

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/****************************************************************************
 *
 * Section: Transport Flow Control API
 *
 ****************************************************************************
 *
 *     The OBEX Transport provides a set of functions which enable an 
 *     application to manage receive flow control at the transport
 *     layer using native transport specific flow control methods. There are
 *     three functions which were created specifically for this purpose:
 *     
 *     OBEX_IsRxFlowAvailable() is used to determine if transport layer
 *         flow control is available.
 *     
 *     OBEX_PauseRxFlow() is used to exert flow control to stop the remote
 *         peer from sending data.
 *     
 *     OBEX_ResumeRxFlow() is used to resume a previously paused connection.
 *
 *     A fourth function OBEX_GetTpConnInfo() is used in conjunction with the
 *     flow control API to retrieve the buffering requirements necessary to
 *     determine when flow control must be exerted and relieved.
 *
 *     Transport flow control can be managed from two perspectives, depending
 *     on your receive buffer arrangement. By calling OBEX_GetTpConnInfo()
 *     the application is informed of the minimum number of receive buffers
 *     and of what size are necessary to store maximum amount of data that
 *     can be received after flow control is exerted via OBEX_PauseRxFlow().
 *     If one contiguous receive buffer is used, its minimum size can be
 *     calculated by multiplying the number of receive buffers by the PDU
 *     size "ObexTpConnInfo.minRxBuffs * ObexTpConnInfo.maxPduSize". Note
 *     that acceptable estimates of these values can be made at compile time.
 *     The minRxBuffs value will be <= OBEX_PERFORMANCE_MULTIPLIER 
 *     configuration option. The maxPduSize value will be <= the transports
 *     PDU size. The configuration option OBEX_TX_BUFF_SIZE is usually
 *     equivalent to the PDU size of the largest transport (unless it is 
 *     overridden with a smaller value).
 *     
 *     OBEX Transport Flow Control affects all OBEX PDU's. But not all PDU's
 *     contain data destined for the Object Store. An application should not
 *     base their flow off/on decisions on counting received buffers or bytes
 *     and comparing them to the values returned from OBEX_GetTpConnInfo().
 *     Instead flow off/on decisions should be based on available receive
 *     buffer space. Once it drops below "ObexTpConnInfo.minRxBuffs" flow
 *     should be paused using OBEX_PauseRxFlow(). As receive buffers become
 *     available again, their number should be compared to
 *     "ObexTpConnInfo.minRxBuffs" once their count meets or exceeds this
 *     value OBEX_ResumeRxFlow() should be called. This cycle should repeat
 *     until the exchange is complete. 
 * 
 *     The flow control state is only reset by OBEX when a new connection is
 *     received. For this reason, applications receiving multiple operations
 *     must preserve their receive state across operations. So if flow was
 *     paused and then the operation completed, the next operation may not
 *     start because flow control is exerted. Again, if flow is resumed
 *     when the necessary receive buffers are available new operations will
 *     proceed normally.
 *
 *     Finally, note that some transports may have limited or no support for
 *     transport layer flow control. For this reason OBEX Packet based flow
 *     control should be considered a better alternative.
 */

/*---------------------------------------------------------------------------
 *
 * OBEX_IsRxFlowAvailable()
 *
 *     Allows the application to determine if a reliable receive flow control
 *     mechanism is available. The return is based on the capability of the
 *     transport and the active connection.
 *
 * Requires:
 *     OBEX_TRANSPORT_FLOW_CTRL  set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - Application context handle provided during init.
 *
 * Returns:
 *     TRUE - Receive flow control is available.
 *     FALSE - Receive flow control is not available.
 */
BOOL OBEX_IsRxFlowAvailable(ObexAppHandle *ObexApp);

/*---------------------------------------------------------------------------
 * OBEX_PauseRxFlow()
 *                
 *     Tell the Server or Client transport layer to stop the remote device
 *     from sending data by refusing to advance transport credit.  Once this
 *     function has been called, no more than "ObexTpConnInfo.minRxBuffs"
 *     PDU's will be received. This amount includes any currently indicated
 *     data when called during an OBS_Write() callback. Flow can be
 *     resumed with OBEX_ResumeRxFlow().
 *
 * Requires:
 *     OBEX_TRANSPORT_FLOW_CTRL  set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - Application context handle provided during init.
 */
void OBEX_PauseRxFlow(ObexAppHandle *ObexApp);

/*---------------------------------------------------------------------------
 * OBEX_ResumeRxFlow()
 *                
 *     Tell the Server or Client transport layer to resume data flow from
 *     the remote device by advancing transport credit.  Once this function
 *     has been called, data will resume flowing until flow control is
 *     again exerted by the application.
 *
 * Requires:
 *     OBEX_TRANSPORT_FLOW_CTRL  set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - Application context handle provided during init.
 */
void OBEX_ResumeRxFlow(ObexAppHandle *ObexApp);

#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if BT_STACK == XA_ENABLED
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            OBEX_RegisterSdpResultsCallback()
 *---------------------------------------------------------------------------
 *
 *     Registers an SDP Query results callback for the OBEX Client. This 
 *     callback allows layers above OBEX to parse the received SDP attributes 
 *     for additional profile specific data.  Such parsing should be in 
 *     addition to the SDP parsing of the Protocol Descriptor List attribute 
 *     that OBEX performs automatically.
 *
 * Requires:
 *     BT_STACK and OBEX_PROVIDE_SDP_RESULTS set to XA_ENABLED.
 *
 * Parameters:
 *     Btxp - The Bluetooth client transport structure pointer.
 *
 *     Callback - The SDP Query results callback for the OBEX Client.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The SDP callback was successfully registered with
 *         the OBEX Bluetooth transport.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_RegisterSdpResultsCallback(ObBtClientTransport *Btxp, 
                                         ObSdpResultsCb Callback);

/*---------------------------------------------------------------------------
 *            OBEX_DeregisterSdpResultsCallback()
 *---------------------------------------------------------------------------
 *
 *     Deregisters an SDP Query results callback for the OBEX Client. 
 *
 * Requires:
 *     BT_STACK and OBEX_PROVIDE_SDP_RESULTS set to XA_ENABLED.
 *
 * Parameters:
 *     Btxp - The Bluetooth client transport structure pointer.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The SDP callback was successfully deregistered with
 *         the OBEX Bluetooth transport.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_DeregisterSdpResultsCallback(ObBtClientTransport *Btxp);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * OBEX_ServerGetSdpProtocolDescList()
 *
 *     Retrieves the servers OBEX Protocol descriptor list. This structure
 *     is used when registering services in SDP. The OBEX Server must be
 *     initialized before making this request.
 *
 * Requires:
 *     BT_STACK set to XA_ENABLED.
 *
 * Parameters:
 *     Btxp - The Bluetooth server transport structure pointer. This
 *         structure provides the Protocol Descriptor List data.
 *
 *     Attribute - The len, value and id fields in this structure are set
 *         by OBEX on return. The data returned in the value field must not
 *         be modified by the caller.
 *
 *     AttribSize - The number of SdpAttribute entries pointed to by the
 *         Attribute pointer. If zero, no entries are set.
 *
 * Returns:
 *     This function always returns the number of Protocol Descriptor
 *     Lists OBEX requires.
 */
U8 OBEX_ServerGetSdpProtocolDescList(ObBtServerTransport *Btxp, 
                                     SdpAttribute *Attribute, 
                                     U8 AttribSize);
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
#endif /* BT_STACK == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/****************************************************************************
 *
 * Section: OBEX Reliable Session API
 *
 ****************************************************************************
 *
 *     The OBEX Reliable Session component provides a set of functions which
 *     enable functionality in OBEX to perform reliable operations.  In this
 *     way, if an operation is interrupted by a transport disconnection, OBEX
 *     can be resumed to the exact position where the disconnect took place.
 *     Once the transport connection is reestablished and the OBEX Resume
 *     command is issued, all OBEX connections will be restored along with
 *     any operation that may have been in progress at the time the transport
 *     disconnection occurred.
 *
 *     Applications and services must save their own context in order to
 *     resume a session. The OBEX layer does not know how commands such as
 *     PUT, GET, SETPATH, etc are used by the application/service so it cannot
 *     save context for them. If the OBEX layer and/or applications are
 *     shut down while a session is suspended then the context must be saved
 *     in a persistent data store if resuming the session later is desired.
 *
 *     Several functions provide the OBEX Reliable Session behavior:
 *
 *     OBEX_ClientCreateSession() is used to create a reliable session.
 *
 *     OBEX_ClientCloseSession() is used to close a reliable session. This 
 *         function can be used to close both an active and a suspended
 *         session. When the active session is closed, the unreliable session
 *         becomes active.
 *
 *     OBEX_ClientSuspendSession() is used to suspend a reliable session. A
 *         suspended session may be closed or resumed.  However, if the 
 *         suspend timeout expires, the session should be closed locally.
 *         When the active session is suspended, the unreliable session
 *         becomes active.
 *     
 *     OBEX_ClientResumeSession() is used to resume a suspended reliable 
 *         session.
 *
 *     OBEX_ClientSetSessionTimeout() is used to set the client's suspend
 *         timeout value. The suspend timeout used is the minimum of the
 *         client and server's requested values.
 *
 *     OBEX_ServerSetSessionTimeout() is used to set the server's suspend
 *         timeout value. The suspend timeout used is the minimum of the
 *         client and server's requested values.
 *
 *     OBEX_ServerAcceptSession() is used to accept the current session 
 *         operation. This function provides a new or matching session when
 *         the OBSE_PROVIDE_SESSION event is received.
 *
 *     OBEX_CreateSessionId() is a utility function used to generate session 
 *         identifier values. It is used to compute the session id from an
 *         OBEX Session structure when closing and resuming a session.
 *
 *     OBEX_GetSessionTimeout() is a utility function used to retrieve the 
 *         session suspend timeout value from an OBEX Session (Client or Server).
 */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ClientCreateSession()
 *
 *     Performs the OBEX "Create Session" operation. 
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Session - Session to use for the create session command. The application
 *               must provide a valid, unused session.
 *
 *     Timeout - Session suspend timeout (in milliseconds).  Use 0xFFFFFFFF 
 *               for an infinite timeout and 0x00 to use the default timeout.
 *               
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_FAILED - Operation failed.
 *
 *     OB_STATUS_BUSY - Operation was not started because the client 
 *         is currently executing another operation or there is
 *         already an active reliable session.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ClientCreateSession(ObexClientApp *ObexApp, 
                                  ObexClientSession *Session, 
                                  U32 Timeout);

/*---------------------------------------------------------------------------
 * OBEX_ClientCloseSession()
 *
 *     Performs the OBEX "Close Session" operation. 
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     SessionId - SessionId of the Session to close. This value can be
 *         obtained by using the OBEX_CreateSessionId function.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_FAILED - Operation failed.
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
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ClientCloseSession(ObexClientApp *ObexApp, U8 SessionId[16]);

/*---------------------------------------------------------------------------
 * OBEX_ClientSuspendSession()
 *
 *     Performs the OBEX "Suspend Session" operation. This function
 *     gracefully suspends the active reliable session. If the OBEX layer
 *     and/or applications are shut down while a session is suspended then
 *     the context must be saved in a persistent data store if resuming the
 *     session later is desired.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Session - Session to set the timeout for.  The application must provide 
 *               the valid session that is currently active.
 *
 *     Timeout - Session suspend timeout (in milliseconds).  Use 0xFFFFFFFF 
 *               for an infinite timeout and 0x00 to use the current timeout.
 *               
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_FAILED - Operation failed.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_NO_SESSION - Operation was not started because
 *         the current session is the unreliable session.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ClientSuspendSession(ObexClientApp *ObexApp, 
                                   ObexClientSession *Session, 
                                   U32 Timeout);

/*---------------------------------------------------------------------------
 * OBEX_ClientResumeSession()
 *
 *     Performs the OBEX "Resume Session" operation. The resume session
 *     command has two components. If the session was suspended gracefully
 *     (via a Suspend Session command) then only the session needs to be 
 *     resumed. If the session was suspended because of a transport link
 *     disconnect, then there may have been an operation in progress when
 *     the suspend occurred. In this case, once the session is resumed the
 *     operation will automatically be resumed. The parameters necessary
 *     to resume a suspended operation are provided in the ResumeParms
 *     parameter.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Session - Session to resume.  The application must provide a valid 
 *         session that has already been created and is suspended.
 *
 *     ResumeParms - If the session being resumed was suspended while an
 *         operation was in progress, the information indicated in the
 *         "u.suspended" structure must be provided here. When resuming
 *         such a session, the operation that was suspended will
 *         automatically resume immediately after the session resume
 *         command completes. An OBCE_RESUME_OPER event will be indicated
 *         to mark the resumption of the operation. The completion of the
 *         resumed operation will be indicated with an OBCE_COMPLETE
 *         or OBCE_ABORTED event. If resume headers are provided with
 *         this structure, the ownership of the memory is transferred to
 *         OBEX until the resume session command completes. If a resume
 *         object store handle is provided with this structure, the handle
 *         is kept by OBEX until the resumed operation is complete.
 *
 *     Timeout - Session suspend timeout (in milliseconds).  Use 0xFFFFFFFF 
 *         for an infinite timeout and 0x00 to use the current timeout.
 *               
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback. If
 *         the completion status is OBCE_ABORTED the session was not resumed
 *         and the unreliable session is still the active session.
 *
 *     OB_STATUS_FAILED - Operation failed.
 *
 *     OB_STATUS_BUSY - Operation was not started because the client
 *         is currently executing another operation or there is
 *         already an active reliable session.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ClientResumeSession(ObexClientApp *ObexApp, 
                                  ObexClientSession *Session, 
                                  ObexSessionResumeParms *ResumeParms,
                                  U32 Timeout);

/*---------------------------------------------------------------------------
 * OBEX_ClientSetSessionTimeout()
 *
 *     Performs the OBEX "Set Session Timeout" operation. 
 *
 *     This function sets the session timeout for the provided active session.
 *     This timeout value will be sent to the server to indicate the
 *     client's suggested OBEX Session Suspend timeout. The actual timeout 
 *     used will be the minimum of the server's and the client's suggested 
 *     values.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Session - Session to set the timeout for.  The application must provide 
 *               the valid session that is currently active.
 *
 *     Timeout - Session suspend timeout (in milliseconds).  Use 0xFFFFFFFF 
 *               for an infinite timeout and 0x00 to use the current timeout.
 *               
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via a call to the ObClientAppCb callback.
 *
 *     OB_STATUS_FAILED - Operation failed.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because there is
 *         no active reliable session.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ClientSetSessionTimeout(ObexClientApp *ObexApp, 
                                      ObexClientSession *Session, 
                                      U32 Timeout);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX_ServerSetSessionTimeout()
 *
 *     Sets the session timeout for the active session on the OBEX server.
 *     This timeout value will be sent to the client to indicate the
 *     server's suggested OBEX Session Suspend timeout during the next
 *     session command exchange. The actual timeout used will be the minimum
 *     of the server's and the client's suggested values.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Timeout - Session suspend timeout (in milliseconds).  Use 0xFFFFFFFF 
 *               for an infinite timeout and 0x00 to use the current timeout.
 * 
 * Returns:
 *     OB_STATUS_SUCCESS - Timeout successfully set.
 *
 *     OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ServerSetSessionTimeout(ObexServerApp *ObexApp, U32 Timeout);

/*---------------------------------------------------------------------------
 * OBEX_ServerAcceptSession()
 *
 *     Accepts the current session create or resume operation. This function
 *     is called to provide a session structure when an OBSE_PROVIDE_SESSION
 *     event is received.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT  set to XA_ENABLED.
 *
 * Parameters:
 *     ObexApp - The Application Handle provided during init.
 *
 *     Session - During a create session request, an empty session structure
 *         is provided. During a resume session request a Session which
 *         matches the client's request is provided.
 *
 *     ResumeParms - (Resume Session Only) If the session being resumed was
 *         suspended while an operation was in progress, the information
 *         indicated in the "u.suspended" structure must be provided here.
 *         When resuming such a session, the operation that was suspended
 *         will automatically resume immediately after the session resume
 *         command completes. An OBSE_RESUME_OPER event will be indicated
 *         to mark the resumption of the operation. The completion of the
 *         resumed operation will be indicated with an OBSE_COMPLETE
 *         or OBSE_ABORTED event. If resume headers are provided with
 *         this structure, the ownership of the memory is transferred to
 *         OBEX until the resume session command completes. If a resume
 *         object store handle is provided with this structure, the handle
 *         is kept by OBEX until the resumed operation is complete.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The session was accepted successfully.
 *
 *     OB_STATUS_FAILED - The server was not ready to accept a session.
 *
 *     OB_STATUS_INVALID_HANDLE - An object store handle is required but
 *         not specified in the resume parameters.
 *
 *     OB_STATUS_INVALID_PARM - A provided session parameter was invalid or 
 *         the session does not match the one the client requested.
 */
ObStatus OBEX_ServerAcceptSession(ObexServerApp *ObexApp, 
                                  ObexServerSession *Session,
                                  ObexSessionResumeParms *ResumeParms);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX_CreateSessionId()
 *
 *     Create a session Id based on the device addresses and nonces 
 *     for the client and server. The session Id is a fixed length
 *     16 byte value.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT  set to XA_ENABLED.
 *
 * Parameters:
 *     Session - The OBEX Session structure to create the SessionId from.
 *
 *     Dest - The destination for the computed OBEX SessionId.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - SessionId was created successfully.
 * 
 *     OB_STATUS_INVALID_PARM - Invalid parameter
 */
ObStatus OBEX_CreateSessionId(void *Session, U8 Dest[16]);


/*---------------------------------------------------------------------------
 * OBEX_GetSessionTimeout()
 *
 *     Retrieve the session suspend timeout value from an OBEX Session
 *     structure.
 *
 * Requires:
 *     OBEX_SESSION_SUPPORT  set to XA_ENABLED.
 *
 * Parameters:
 *     Session - The OBEX (Client or Server) Session structure.
 *
 * Returns:
 *     Timeout value in milliseconds. The value of 0xFFFFFFFF indicates
 *     that the session should never timeout.
 */
U32 OBEX_GetSessionTimeout(void *Session);

#define OBEX_GetSessionTimeout(_SSN)  (((ObexClientSession *)(_SSN))->timeout)

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Types used internally by the OBEX API
 *
 ****************************************************************************/
#if OBEX_ROLE_CLIENT == XA_ENABLED
ObStatus OBEX_ClientSendRequest(ObexClientApp *ObexApp, ObexOpcode Opcode, 
                                U32 param1, U32 param2);

#define OBEX_ClientGet( _OBEXAPP, _OBS, _MORE ) \
            OBEX_ClientSendRequest( (_OBEXAPP), OB_OPCODE_GET, (U32)(_OBS), (_MORE))
#define OBEX_ClientSetPath( _OBEXAPP, _PF ) \
            OBEX_ClientSendRequest((_OBEXAPP), OB_OPCODE_SET_PATH, (U32)(_PF), 0)
#define OBEX_ClientDisconnect(_OBEXAPP) \
            OBEX_ClientSendRequest((_OBEXAPP), OB_OPCODE_DISCONNECT, 0, 0)
#define OBEX_ClientConnect(_OBEXAPP) \
            OBEX_ClientSendRequest((_OBEXAPP), OB_OPCODE_CONNECT, 0, 0)

#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
ObStatus OBEX_ClientSimpleSendRequest(ObexClientApp *ObexApp, ObexOpcode Opcode,
                                      ObexTpAddr *Target);

#define OBEX_ClientSimpleDisconnect(_OBEXAPP) \
            OBEX_ClientSimpleSendRequest((_OBEXAPP), OB_OPCODE_DISCONNECT, 0)
#define OBEX_ClientSimpleConnect(_OBEXAPP, _TARGET) \
            OBEX_ClientSimpleSendRequest((_OBEXAPP), OB_OPCODE_CONNECT, _TARGET)
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
ObStatus OBEX_ClientSendSessionRequest(ObexClientApp *ObexApp, void *Param,
                                       ObexSessionResumeParms *ResumeParms,
                                       U8 opcode, U32 timeout);

#define OBEX_ClientCreateSession( _OBEXAPP, _SESSION, _TIMEOUT ) \
            OBEX_ClientSendSessionRequest( (_OBEXAPP), (_SESSION), 0, OB_SESSION_OPCODE_CREATE, (_TIMEOUT))
#define OBEX_ClientCloseSession( _OBEXAPP, _SESSION_ID ) \
            OBEX_ClientSendSessionRequest( (_OBEXAPP), (_SESSION_ID), 0, OB_SESSION_OPCODE_CLOSE, 0)
#define OBEX_ClientSuspendSession( _OBEXAPP, _SESSION, _TIMEOUT ) \
            OBEX_ClientSendSessionRequest( (_OBEXAPP), (_SESSION), 0, OB_SESSION_OPCODE_SUSPEND, (_TIMEOUT))
#define OBEX_ClientResumeSession( _OBEXAPP, _SESSION, _PARMS, _TIMEOUT ) \
            OBEX_ClientSendSessionRequest( (_OBEXAPP), (_SESSION), (_PARMS), OB_SESSION_OPCODE_RESUME, (_TIMEOUT))
#define OBEX_ClientSetSessionTimeout( _OBEXAPP, _SESSION, _TIMEOUT ) \
            OBEX_ClientSendSessionRequest( (_OBEXAPP), (_SESSION), 0, OB_SESSION_OPCODE_SET_TIMEOUT, (_TIMEOUT))
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
ObStatus OBEX_TpConnect(ObexClientTransports *Transports, ObexAppHandle *AppHndl, 
                        ObexTpAddr *Target);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED)
ObStatus OBEX_TpDisconnect(void *Transports, ObexAppHandle *AppHndl, 
                           BOOL Force);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED) */


#if OBEX_ROLE_CLIENT == XA_ENABLED
#define OBEX_ClientTpConnect( _OBEXAPP, _TARGET ) \
            OBEX_TpConnect(&((_OBEXAPP)->trans), &((_OBEXAPP)->handle), (_TARGET))
#define OBEX_ClientTpDisconnect( _OBEXAPP, _FORCE ) \
            OBEX_TpDisconnect(&((_OBEXAPP)->trans), &((_OBEXAPP)->handle), (_FORCE))
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
#define OBEX_ServerTpConnect( _OBEXAPP, _TARGET ) \
            OBEX_TpConnect((ObexClientTransports *)&((_OBEXAPP)->trans), &((_OBEXAPP)->handle), (_TARGET))
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
#define OBEX_ServerTpDisconnect( _OBEXAPP, _FORCE ) \
            OBEX_TpDisconnect(&((_OBEXAPP)->trans), &((_OBEXAPP)->handle), (_FORCE))
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#endif /* __OBEX_H */

