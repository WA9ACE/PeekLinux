#ifndef __SIM_H
#define __SIM_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:sim.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision:27$
 *
 * Description: Public types, defines, and prototypes for accessing the
 *              SIM Access layer.
 * 
 * Created:     May 1, 2002
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <bttypes.h>
#include <eventmgr.h>
#include <rfcomm.h>
#include <sdp.h>
/*---------------------------------------------------------------------------
 * SIM Access Profile API layer
 *
 * This API allows an application to implement the Client and/or
 * Server role of the SIM Access Profile. The Server is typically a
 * mobile phone with a SIM card installed. The SIM Access Client is a
 * device (such as a car phone) that accesses the Server's SIM card
 * information over a Bluetooth link.
 *
 * This application interface defines both Server and Client APIs.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SIM_SERVER constant
 *     Defines the role of the device as a server.  Server code is included
 *     if SIM_SERVER is defined as XA_ENABLED.  A device can have both the
 *     server and client roles, if necessary.
 */
#ifndef SIM_SERVER
#define SIM_SERVER XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * SIM_CLIENT constant
 *     Defines the role of the device as a client.  Client code is included
 *     if SIM_CLIENT is defined as XA_ENABLED.  A device can have both the
 *     server and client roles, if necessary.
 */
#ifndef SIM_CLIENT
#define SIM_CLIENT XA_ENABLED
#endif

#if TI_CHANGES == XA_ENABLED


#elif (SIM_SERVER != XA_ENABLED) && (SIM_CLIENT != XA_ENABLED)
#error SIM_SERVER or SIM_CLIENT must be enabled
#endif

/*---------------------------------------------------------------------------
 * SIM_MAX_MSG_SIZE constant
 *     Defines the maximum frame size supported by the SIM Access profile.
 *     This value can be up to 64K bytes.  The default size is the same
 *     as the RFCOMM frame size.
 */
#ifndef SIM_MAX_MSG_SIZE
#define SIM_MAX_MSG_SIZE RF_MAX_FRAME_SIZE
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SimEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type SimServerCallback or SimClientCallback. The
 *     "SimCallbackParms.event" field will contain one of the event types
 *     below.
 */
typedef U8 SimEvent;

#if SIM_SERVER == XA_ENABLED
/** A client has requested a connection to the server.  During processing of 
 *  this event, call SIM_ServerRespondOpenInd to indicate whether the connection 
 *  should be allowed.  When this callback is received, the 
 *  "SimCallbackparms.ptrs.remDev" field contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_OPEN_IND                 0x01

/** A connection is open.  This can come as a result of a call to 
 *  SIM_ServerRespondOpenInd.  When this event has been received, the  
 *  connection is available for receiving requests and sending responses.
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_OPEN                     0x02

/** The remote device is closing the connection.  Once the connection is closed, 
 *  a SAEVENT_SERVER_CLOSED event will be received.  Requests will not be 
 *  received and responses cannot be sent in this state.
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_CLOSE_IND                0x03

/** The connection is closed.  This can come as a result of calling 
 *  SIM_ServerClose, if the remote device has closed the connection, or if an 
 *  incoming connection is rejected by a call to SIM_ServerRespondOpenInd.  
 *  Requests will not be received and responses cannot be sent in this state.  
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_CLOSED                   0x04

/** The ATR is being requested by the client.  This can happen at any time
 *  during a connection (after SAEVENT_SERVER_OPEN is received).  It can also
 *  happen before the connection (after SAEVENT_SERVER_OPEN_IND is received).
 *  The server must respond by calling SIM_ServerAtrRsp with a valid ATR
 *  as specified by the GSM spec, or with an error code describing the reason
 *  the response cannot be made.  During this callback, the
 *  "SimCallbackParms.ptrs" field is not valid.
 */
#define SAEVENT_SERVER_ATR_REQ                  0x05

/** An APDU has been received from the client.  This can happen at any time
 *  during a connection (after SAEVENT_SERVER_OPEN is received).
 *  The server must respond by calling SIM_ServerApduRsp with a valid APDU
 *  as specified by the GSM spec, or with an error code describing the reason
 *  the response cannot be made.  During this callback, the 
 *  "SimCallbackParms.ptrs.msg.parms.apdu" field points to the command APDU.
 */
#define SAEVENT_SERVER_APDU_REQ                 0x06

/** During the connect process, the server must send the status of the SIM
 *  card.  The application will be prompted to send the status with this
 *  event.  During the processing of this event, the application must call 
 *  SIM_ServerSendStatus.  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_STATUS_REQ               0x07

/** When the client requests that the SIM card be turned on, the server will
 *  receive the SAEVENT_SERVER_SIM_ON_REQ event.  The server should respond
 *  to this request by calling SIM_ServerSimOnRsp with the result of the
 *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_SIM_ON_REQ               0x08

/** When the client requests that the SIM card be turned off, the server will
 *  receive the SAEVENT_SERVER_SIM_OFF_REQ event.  The server should respond
 *  to this request by calling SIM_ServerSimOffRsp with the result of the
 *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_SIM_OFF_REQ              0x09

/** When the client requests that the SIM card be reset, the server will
 *  receive the SAEVENT_SERVER_RESET_SIM_REQ event.  The server should respond
 *  to this request by calling SIM_ServerResetSimRsp with the result of the
 *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_RESET_SIM_REQ            0x0A

/** When the client requests the status of the card reader, the server will
 *  receive the SAEVENT_SERVER_CARD_READER_STATUS_REQ event.  The server should 
 *  respond to this request by calling SIM_ServerCardReaderStatusRsp with the 
 *  result of the action (see SimResultCode and SimCardReaderStatus).
 *  During this callback, the "SimCallbackParms.ptrs" field is not valid.
 */
#define SAEVENT_SERVER_CARD_READER_STATUS_REQ   0x0B

/** When the server sends the status of the SIM card, this event will indicate
 *  that the status has been sent successfully.  The SIM card status is the
 *  only unsolicited message that the server can send, therefore, it is 
 *  important to know when it is safe to send the command.  After receiving this
 *  event, the application can send another SIM card status in necessary.
 */
#define SAEVENT_SERVER_STATUS_SENT              0x0C

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
/** A connection is open.  This can come as a result of a call to 
 *  SIM_ClientOpen.  When this event has been received, the connection is 
 *  available for receiving requests and transmitting responses.
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_CLIENT_OPEN                     0x81

/** The server is requesting that the connection be closed.  This is a graceful
 *  termination, and it is up to the application to finish up any work and then
 *  close the connection by calling SIM_ClientClose.  When this callback is 
 *  received, the "SimCallbackParms.ptrs.remDev" field contains a pointer to the 
 *  remote device.
 */
#define SAEVENT_CLIENT_CLOSE_IND                0x82

/** The connection is closed.  This can come as a result of calling 
 *  SIM_ClientClose, or if the remote device has closed (or rejected) the 
 *  connection.  Requests cannot be sent in this state.  When this callback is 
 *  received, the "SimCallbackParms.ptrs.remDev" field contains a pointer to the 
 *  remote device.
 */
#define SAEVENT_CLIENT_CLOSED                   0x83

/** An APDU response has been received as a result of a call to 
 *  SIM_ClientApduReq.  When this callback is received, the 
 *  "SimCallbackParms.ptrs.msg.parms.apdu" field may contains a pointer to the 
 *  APDU depending on the value of the "SimCallbackParms.ptrs.msg.result" field.
 *  If the result field is equal to SIM_RESULT_OK, the apdu field is valid,
 *  otherwise there was an error (see SimMsg).
 */
#define SAEVENT_CLIENT_APDU_RESP                0x84

/** An ATR Response has been received as a result of a call to
 *  SIM_ClientAtrReq or SIM_ClientOpen.  When this callback is received, the 
 *  "SimCallbackParms.ptrs.remDev" field may contain a pointer to the ATR data,
 *  depending on the value of the "SimCallbackParms.ptrs.msg.result" field.
 *  If the result field is equal to SIM_RESULT_OK, the apdu field is valid,
 *  otherwise there was an error (see SimMsg).
 */
#define SAEVENT_CLIENT_ATR_RESP                 0x85

/** When the server responds to a request by the client to turn the 
 *  SIM card off, the client will receive the SAEVENT_CLIENT_SIM_OFF_RESP event.
 *  During this callback, the "SimCallbackParms.ptrs.msg.result" field contains
 *  the results of the action (see SimResultCode).
 */
#define SAEVENT_CLIENT_SIM_OFF_RESP             0x86

/** When the server responds to a request by the client to turn the 
 *  SIM card on, the client will receive the SAEVENT_CLIENT_SIM_ON_RESP event.
 *  During this callback, the "SimCallbackParms.ptrs.msg.result" field contains
 *  the results of the action (see SimResultCode).  This event will always be
 *  followed by an SAEVENT_CLIENT_ATR_RESP event, because the client 
 *  automatically requests the ATR after the SIM is turned on.
 */
#define SAEVENT_CLIENT_SIM_ON_RESP              0x87

/** When the server responds to a request by the client to reset the SIM card,
 *  the client will receive the SAEVENT_CLIENT_RESET_SIM_RESP event.  During 
 *  this callback, the "SimCallbackParms.ptrs.msg.result" field contains the 
 *  results of the action (see SimResultCode).  This event will always be
 *  followed by an SAEVENT_CLIENT_ATR_RESP event, because the client 
 *  automatically requests the ATR after the SIM is reset.
 */
#define SAEVENT_CLIENT_RESET_SIM_RESP           0x88

/** When the server responds to a request by the client to retrieve the card
 *  reader status, the client will receive the 
 *  SAEVENT_CLIENT_CARD_READER_STATUS_RESP event.  During this callback, the 
 *  "SimCallbackParms.ptrs.msg.cardRdrStatus" field contains the status of the
 *  card (see SimCardReaderStatus).
 */
#define SAEVENT_CLIENT_CARD_READER_STATUS_RESP  0x89

/** If the state of the SIM card changes, the client will receive an
 *  SAEVENT_CLIENT_STATUS_IND event.  During this callback, the 
 *  "SimCallbackParms.ptrs.msg.statusChange" field contains the new state of
 *  the SIM card (see SimCardStatus).
 */
#define SAEVENT_CLIENT_STATUS_IND               0x8A

/** If the server receives an unknown message, or a message with an error, the
 *  client will receive the SAEVENT_CLIENT_ERROR_RESP event.  During this 
 *  callback, the "SimCallbackParms.ptrs" is not valid.
 */
#define SAEVENT_CLIENT_ERROR_RESP               0x8B
#endif /* SIM_CLIENT == XA_ENABLED */

/* End of SimEvent */


/*---------------------------------------------------------------------------
 * SimConnStatus type
 *
 *     When the client wishes to connect to the server, the server can respond
 *     with different status codes.  These codes indicate whether the connection
 *     can be accepted or not at the time of the request.
 */
typedef U8 SimConnStatus;

/** The connection is accepted.  When this status code is used, the connection 
 *  will progress normally.  When the connection is established, the 
 *  SAEVENT_SERVER_OPEN event will be received.  Previous to the 
 *  SAEVENT_SERVER_OPEN event, a SAVEVENT_SERVER_STATUS_REQ event will be
 *  received.  The application must respond to this by sending the current
 *  card status by calling SIM_ServerSendStatus.  The application may also
 *  receive a SAEVENT_SERVER_ATR_REQ event.  If this event is received,
 *  the application must call SIM_ServerAtrRsp with the ATR response.
 */
#define SIM_CONN_STATUS_OK           0

/** The connection is rejected.  When this status code is used, the connection
 *  is rejected and the connection remains closed.
 */
#define SIM_CONN_STATUS_UNABLE       1

/* End of SimConnStatus */

/*---------------------------------------------------------------------------
 * SimDisconnectType type
 *
 *     When the server wishes to disconnect, it can send a disconnect 
 *     indication to the client.  There are two types of disconnect that can
 *     be initiated by the server.  
 */
typedef U8 SimDisconnectType;

/** A graceful disconnect allows the client to continue sending APDU's until
 *  while the GSM connection is terminated.  The client will then initiate
 *  the disconnect process.  Until the client initiates the disconnect process,
 *  the connection stays open.
 */
#define SIM_DISCONNECT_GRACEFUL   0x00

/** An immediate disconnect terminates the connection immediately.  No response
 *  is required from the client.  The GSM connection should be terminated
 *  immediately.
 */
#define SIM_DISCONNECT_IMMEDIATE  0x01

/* End of SimDisconnectType */


/*---------------------------------------------------------------------------
 * SimCardStatus type
 *
 *     When the status of the SIM changes, the server must send the new status
 *     to the client.  During the connect procedure, the status must also be
 *     sent.
 */
typedef U8 SimCardStatus;

/** The status of the card is unknown.
 */
#define SIM_CARD_STATUS_UNKNOWN          0x00

/** The card has been reset and is available.
 */
#define SIM_CARD_STATUS_RESET            0x01

/** The card is not accessible.
 */
#define SIM_CARD_STATUS_NOT_ACCESSIBLE   0x02

/** The card has been removed.
 */
#define SIM_CARD_STATUS_REMOVED          0x03

/** The card has just been inserted.
 */
#define SIM_CARD_STATUS_INSERTED         0x04

/** The card has been recovered.
 */
#define SIM_CARD_STATUS_RECOVERED        0x05

/* End of SimCardStatus */

/*---------------------------------------------------------------------------
 * SimResultCode type
 *
 *     Several requests require a result code.  This result code describes
 *     the result of the action initiated by a particular request.
 */
typedef U8 SimResultCode;

/** The request processed correctly.
 */
#define SIM_RESULT_OK                   0x00

/** Error, no reason defined.
 */
#define SIM_RESULT_NO_REASON            0x01

/** Error, card not accessible 
 */
#define SIM_RESULT_CARD_NOT_ACCESSIBLE  0x02

/** Error, the card is already off.
 */
#define SIM_RESULT_CARD_ALREADY_OFF     0x03

/** Error, the card is removed.
 */
#define SIM_RESULT_CARD_REMOVED         0x04

/** Error, the card is already on.
 */
#define SIM_RESULT_CARD_ALREADY_ON      0x05

/** Error, data is not available.
 */
#define SIM_RESULT_DATA_NOT_AVAILABLE   0x06

/* End of SimResultCode */

/*---------------------------------------------------------------------------
 * SimCardReaderStatus type
 *
 *      Specifies the identity of the card reader, and flags to indicate the
 *      status of the reader.
 */
typedef U8 SimCardReaderStatus;

#define SIM_CRS_ID0                     0x00
#define SIM_CRS_ID1                     0x10
#define SIM_CRS_ID2                     0x20
#define SIM_CRS_ID3                     0x30
#define SIM_CRS_ID4                     0x40
#define SIM_CRS_ID5                     0x50
#define SIM_CRS_ID6                     0x60
#define SIM_CRS_ID7                     0x70
#define SIM_CRS_NOT_REMOVABLE           0x00
#define SIM_CRS_REMOVABLE               0x80
#define SIM_CRS_NOT_PRESENT             0x00
#define SIM_CRS_PRESENT                 0x01
#define SIM_CRS_NOT_ID1_SIZE            0x00
#define SIM_CRS_ID1_SIZE                0x02
#define SIM_CRS_NO_CARD_PRESENT         0x00
#define SIM_CRS_CARD_PRESENT            0x04
#define SIM_CRS_NO_CARD_POWERED         0x00
#define SIM_CRS_CARD_POWERED            0x08

/* End of SimCardReaderStatus */

/* Forward references */
typedef struct _SimCallbackParms SimCallbackParms;
#if SIM_SERVER == XA_ENABLED
typedef struct _SimServer         SimServer;
#endif /* SIM_SERVER == XA_ENABLED */
#if SIM_CLIENT == XA_ENABLED
typedef struct _SimClient         SimClient;
#endif /* SIM_CLIENT == XA_ENABLED */
typedef struct _SimApdu           SimApdu;
typedef struct _SimAtr            SimAtr;
typedef struct _SimMsg            SimMsg;

/* SIM packet */
typedef struct _SimPacket {
    BtPacket      packet;
    U8            packetData[280];
    U16           dataToSend;
    U16           maxMsgSize;
} SimPacket;

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimServerCallback type
 *
 *     Represents a callback function called by the SIM Access layer to 
 *     indicate events and data to the server application. The event is 
 *     targeted to the SIM Access Server identified by the "Server" parameter. 
 *     Information about the event is contained in the "Parms" parameter.
 */
typedef void (*SimServerCallback)(SimServer *Server, SimCallbackParms *Parms);
#endif /* SIM_SERVER == XA_ENABLED */

/* End of SimServerCallback */

#if SIM_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimClientCallback type
 *
 *     Represents a callback function called by the SIM Access layer to 
 *     indicate events and data to the client application. The event is 
 *     targeted to the SIM Access Client identified by the "Client" parameter. 
 *     Information about the event is contained in the "Parms" parameter.
 */
typedef void (*SimClientCallback)(SimClient *Client, SimCallbackParms *Parms);
#endif /* SIM_CLIENT == XA_ENABLED */

/* End of SimClientCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SimApdu structure
 *
 *     Describes a portion of an APDU.  An APDU may be received in multiple
 *     pieces by the application.  This structure tracks the position within
 *     the APDU, and provides information about the current portion.
 */
struct _SimApdu
{
    U16  dataLen;     /* Length of the APDU data */
    U8   data[280];   /* Pointer to APDU data    */

    /* === Internal use only === */
    U16  offset;      /* Offset into the data    */
};

/*---------------------------------------------------------------------------
 * SimAtr structure
 *
 *     Describes a portion of an ATR.  An ATR may be received in multiple
 *     pieces by the application.  This structure tracks the position within
 *     the ATR, and provides information about the current portion.
 */
struct _SimAtr
{
    U16  dataLen;     /* Length of the ATR data */
    U8   data[280];   /* Pointer to ATR data    */

    /* === Internal use only === */
    U16  offset;    /* Offset into the data     */
};

/*---------------------------------------------------------------------------
 * SimMsg structure
 *
 *     Describes a SIM message.  The SIM messages are passed between the client
 *     and the server in order to perform specific functions.  During a callback,
 *     this information is provided to the application in the SimCallbackParms
 *     structure.
 */
struct _SimMsg {
    U8 resultCode;      /* Results code for the message.  Only valid
                         * during the following responses:
                         *
                         *     SAEVENT_CLIENT_APDU_RESP
                         *     SAEVENT_CLIENT_ATR_RESP               
                         *     SAEVENT_CLIENT_SIM_OFF_RESP           
                         *     SAEVENT_CLIENT_SIM_ON_RESP            
                         *     SAEVENT_CLIENT_RESET_SIM_RESP         
                         *     SAEVENT_CLIENT_CARD_READER_STATUS_RESP
                         */
    
    /* Parameters */
    union {
        /* During an SAEVENT_SERVER_APDU_IND or SAEVENT_CLIENT_APDU_RESP event, 
         * contains the APDU data received from the remote device.
         */
        SimApdu              apdu;

        /* During an SAEVENT_CLIENT_ATR_RESP event, contains the ATR data 
         * received from the SIM server.
         */
        SimAtr               atr;

        /* During an SAEVENT_CLIENT_CARD_READER_STATUS_RESP event, contains the
         * card reader status.
         */
        SimCardReaderStatus  cardRdrStatus;

        /* During an SAEVENT_CLIENT_STATUS_IND, contains the status of the SIM
         * card.
         */
        SimCardStatus        statusChange;

        /* === Internal use only === */
        U16                  maxMsgSize;
        SimDisconnectType    discType;
        BtPacket            *packet;
    } parm;

    /* === Internal use only === */
    U8  type;      /* Message type             */
    U8  numParms;  /* Number of parms returned */
    U8 connStatus;
};

/*---------------------------------------------------------------------------
 * SimCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Various fields in the structure may be valid or not, depending
 *     on the "event" field.
 */
struct _SimCallbackParms
{
    SimEvent            event;        /* Type of the SIM Access event */

    BtStatus            status;       /* Link status or error information */

    U16                 len;          /* Length of the object pointed to by
                                       * 'ptrs' 
                                       */

    SimResultCode       results;      /* Result code for the SIM operation */

    /* Group: The event type determines whether a field in this
     * union is valid.
     */
    union {
        /* During the following events, contains information specific to the
         * message that was exchanged (see SimMsg).
         *
         *     SAEVENT_CLIENT_APDU_RESP              
         *     SAEVENT_CLIENT_ATR_RESP               
         *     SAEVENT_CLIENT_SIM_OFF_RESP           
         *     SAEVENT_CLIENT_SIM_ON_RESP            
         *     SAEVENT_CLIENT_RESET_SIM_RESP         
         *     SAEVENT_CLIENT_CARD_READER_STATUS_RESP
         *     SAEVENT_CLIENT_STATUS_IND
         */
        SimMsg          *msg;

        /* During an SAEVENT_SERVER_OPEN_IND, SAEVENT_SERVER_OPEN, 
         * SAEVENT_CLIENT_OPEN, SAEVENT_SERVER_CLOSED, SAEVENT_CLIENT_CLOSE_IND,
         * or SAEVENT_CLIENT_CLOSED event, contains the remote device structure.
         */
        BtRemoteDevice  *remDev;

    } ptrs;
};

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimServer structure
 *
 *     The server channel.  This structure is used to identify a SIM Access
 *     Server.
 */
struct _SimServer {

    /* === Internal use only === */
    ListEntry           node;  
    BtRemoteDevice     *remDev;
    RfChannel           rfChannel;

    /* Internal packet variable */
    SimPacket           simPacket;

    /* SDP variables for server */
    U8                  sdpProtoDescList[14];
    SdpAttribute        sdpAttribute[5];
    SdpRecord           sdpRecord;

    /* Other state variables */
    SimServerCallback   callback;
    U16                 flags;
    U8                  state;

    /* Parser state information */
    struct {
        U8              stageBuff[4];          /* Staging buffer            */
        U8              stageLen;              /* Length of staged data     */
        U8              curStageOff;           /* Offset into staged data   */
        U8              *rxBuff;               /* The Received buffer       */
        U16             rxSize;                /* Remaining rx buff size    */
        U8              rxState;               /* Parser state              */
    } parser;

    /* Parser message structure */
    SimMsg              msg;

};
#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimClient structure
 *
 *     The client channel.  This structure is used to identify a SIM Access
 *     Client.
 */
struct _SimClient {

    /* === Internal use only === */
    ListEntry           node;  
    BtRemoteDevice     *remDev;
    RfChannel           rfChannel;
    U8                  rfServerChannel;

    /* SDP variables for client */
    SdpQueryToken       sdpQueryToken;
    BOOL                foundChannel;
    U8                  sdpSearchAttribReq[18];
    
    /* Internal packet variable */
    SimPacket           simPacket;

    /* Other state variables */
    SimClientCallback   callback;
    U16                 flags;
    U8                  state;

    /* Parser message structure */
    SimMsg              msg;

    /* Parser state information */
    struct {
        U8              stageBuff[4];          /* Staging buffer            */
        U8              stageLen;              /* Length of staged data     */
        U8              curStageOff;           /* Offset into staged data   */
        U8              *rxBuff;               /* The Received buffer       */
        U16             rxSize;                /* Remaining rx buff size    */
        U8              rxState;               /* Parser state              */
        U8              parmCount;             /* Number of parms to parse  */
    } parser;
};
#endif /* SIM_CLIENT == XA_ENABLED */

/****************************************************************************
 *
 * Section: Server APIs
 *
 ****************************************************************************/

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SIM_RegisterServer()
 *
 *     Registers a server for receiving SIM Access connections.  This function
 *     creates the required SDP entries based on the service that is being
 *     registered.  The Major Service Class of the Class of Device is also
 *     registered.  The Major and Minor Device Classes of the Class of Device
 *     are not registered, however.  The application or management software
 *     must register the appropriate values to be compliant with the
 *     SIM Access Profile.
 *
 * Parameters:
 *     Server - Server to register.  This structure is owned by the SIM Access 
 *     layer until the server is deregistered.
 *
 *
 *     Callback - Callback function for receiving events related to the
 *         specified server.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The server was successfully registered.
 *
 *     BT_STATUS_FAILED - The server could not be registered, probably 
 *         because it is already in use.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The Server structure is already in use 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Could not register a channel with RFCOMM.
 */
BtStatus SIM_RegisterServer(SimServer *Server, 
                            SimServerCallback Callback);

/*---------------------------------------------------------------------------
 * SIM_DeregisterServer()
 *
 *     Deregisters a SIM Access server.  All SDP entries are removed.
 *
 * Parameters:
 *     Server - Server to deregister.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The server was successfully deregistered.
 *
 *     BT_STATUS_BUSY - The server has an open channel. All channels
 *         must be successfully closed with SIM_ServerClose before 
 *         calling this function (XA_ERROR_CHECK only).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_DeregisterServer(SimServer *Server);

/*---------------------------------------------------------------------------
 * SIM_ServerRespondOpenInd()
 *
 *     Accepts or rejects an incoming connection in response to an 
 *     SAEVENT_SERVER_OPEN_IND event.  This event occurs when a remote client 
 *     attempts to connect to a registered server. This function must be 
 *     used to respond to the connection request.
 *
 * Parameters:
 *     Server - Identifies the server that is accepting the connection.  This 
 *         server is provided to the callback function as a parameter during the
 *         SAEVENT_SERVER_OPEN_IND event.
 *
 *     Status - Identifies the connection status response.  If Status is 
 *         set to SIM_CONN_STATUS_OK, the connection is accepted.  When the 
 *         connection is established, the SAEVENT_SERVER_OPEN event will be 
 *         received.  Previous to the SAEVENT_SERVER_OPEN event, a 
 *         SAVEVENT_SERVER_STATUS_REQ event will be received.  The application 
 *         must respond to this by sending the current card status by calling 
 *         SIM_ServerSendStatus.  The application may also receive a 
 *         SAEVENT_SERVER_ATR_REQ event.  If this event is received, the 
 *         application must call SIM_SendAtrRsp with the ATR response.
 *
 *         If the Status is set to SIM_CONN_STATUS_UNABLE, the connection is
 *         rejected and the connection remains closed.
 *
 * Returns:
 *     BT_STATUS_PENDING - The accept message will be sent. The application
 *         will receive a SAEVENT_SERVER_OPEN when the accept message has been 
 *         sent and the channel is open.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - The specified server did not have a pending connection
 *         request (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection to accept.
 */
BtStatus SIM_ServerRespondOpenInd(SimServer *Server, SimConnStatus Status);


/*---------------------------------------------------------------------------
 * SIM_ServerSendStatus()
 *
 *     Sends the current status of the SIM card to the client.  Whenever the 
 *     the status of the card changes this function must be called to notify 
 *     the client.  
 *
 *     In addition, during the connection process the application will be
 *     prompted for the card status with a SAEVENT_SERVER_STATUS_REQ event.
 *     The application must respond to this request before the connection
 *     process can continue.
 *
 *     See the SIM Access profile for more information on the use of server
 *     status.
 *
 * Parameters:
 *     Server - Identifies the server that is sending status.
 *
 *     Status - Indicates the status of the card (see SimCardStatus).
 *
 * Returns:
 *     BT_STATUS_PENDING - The status was sent. The application will receive
 *         a SAEVENT_SERVER_STATUS_SENT event when the status has been sent to 
 *         the client.
 *
 *     BT_STATUS_FAILED - There was no open connection for sending status.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 *
 */
BtStatus SIM_ServerSendStatus(SimServer *Server, SimCardStatus Status);


/*---------------------------------------------------------------------------
 * SIM_ServerAtrRsp()
 *
 *     Send an ATR response to a client's request.  When a 
 *     SAEVENT_SERVER_ATR_REQ is received, the server should respond using
 *     this function.
 *
 *     See the SIM Access profile for more information on the use of ATR
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Len - Contains the length of the ATR response.
 *
 *     Atr - Contains a pointer to the ATR response
 *
 *     Result - Contains the result of ATR command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerAtrRsp(SimServer *Server, U16 Len, U8 *Atr, 
                          SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerApduRsp()
 *
 *     Send an APDU response to a client's request.  When a 
 *     SAEVENT_SERVER_APDU_REQ is received, the server should respond using
 *     this function.
 *
 *     See the SIM Access profile for more information on the use of APDU
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server for this action.
 *
 *     Len - Identifies the length of the APDU.
 *
 *     Ptr - A pointer to the APDU data.
 *
 *     Result - Contains the result of APDU command (see SimResultCode).
 *
 * Returns:    
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerApduRsp(SimServer *Server, U16 Len, U8 *Apdu,
                           SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerSimOnRsp()
 *
 *     Send an SIM ON response to a client's request.  When a 
 *     SAEVENT_SERVER_SIM_ON_REQ is received, the server should respond using
 *     this function.  If the SIM card is off, this function must be called 
 *     after the SIM has been turned on.  If the SIM card was already on, or
 *     if an error occurs when turning on the SIM, then this function must
 *     be called with the appropriate result code.
 *
 *     See the SIM Access profile for more information on the use of SIM ON
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of SIM ON command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerSimOnRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerSimOffRsp()
 *
 *     Send an SIM OFF response to a client's request.  When a 
 *     SAEVENT_SERVER_SIM_OFF_REQ is received, the server should respond using
 *     this function.  If the SIM card is on, this function must be called 
 *     after the SIM has been turned off.  If the SIM card was already off, or
 *     if an error occurs when turning the SIM off, then this function must
 *     be called with the appropriate result code.
 *
 *     See the SIM Access profile for more information on the use of SIM OFF
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of SIM OFF command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerSimOffRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerResetSimRsp()
 *
 *     Send an RESET SIM response to a client's request.  When a 
 *     SAEVENT_SERVER_RESET_SIM_REQ is received, the server should respond using
 *     this function.  The SIM card should be reset (if possible) before
 *     responding to this request.  If an error occurs when resetting the SIM, 
 *     then this function must be called with the appropriate result code.
 *
 *     See the SIM Access profile for more information on the use of RESET SIM
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of RESET SIM command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerResetSimRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerCardReaderStatusRsp()
 *
 *     Send an CARD READER STATUS response to a client's request.  When a 
 *     SAEVENT_SERVER_CARD_READER_STATUS_REQ is received, the server should 
 *     respond using this function.
 *
 *     See the SIM Access profile for more information on the use of CARD
 *     READER STATUS responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     CardRdrStatus - Contains the status of the card reader (see 
 *         SimCardReaderStatus).
 *
 *     Result - Contains the result of CARD READER STATUS command (see 
 *         SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerCardReaderStatusRsp(SimServer *Server, 
                                       SimCardReaderStatus CardRdrStatus,
                                       SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerClose()
 *
 *     Closes a SIM Access connection between two devices.  When the connection
 *     is closed, the application will receive an SAEVENT_SERVER_CLOSED event.
 *
 * Parameters:
 *     Server - Identifies the server connection to be closed. The 
 *         SAEVENT_SERVER_CLOSED event indicates that the connection is closed 
 *         and a new client may attempt to connect to the server.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to close the connection will be sent.
 *         The application will receive an SAEVENT_SERVER_CLOSED event when the
 *         connection is closed.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only). 
 *
 *     BT_STATUS_BUSY - The server is already in the process of disconnecting.
 *
 *      BT_STATUS_NO_CONNECTION - No connection exists on this Server 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerClose(SimServer *Server, SimDisconnectType Type);

#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * SIM_SetSecurityLevel()
 *
 *     Update the security level for this SIM server
 *
 * Parameters:
 *     securityLevel [in] - New level to be used now
 *
 * Returns: -
 */
void SIM_SetSecurityLevel(BtSecurityLevel *securityLevel);
/*---------------------------------------------------------------------------
 * SIM_GetSecurityLevel()
 *
 *     Return the security level for this SIM server
 *
 * Parameters:
 *     securityLevel [out] - Cuurent security level.
 *
 * Returns: -
 */
void SIM_GetSecurityLevel(BtSecurityLevel *securityLevel);
#endif /* TI_CHANGES == XA_DISABLED */


#endif /* SIM_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Section: Client APIs
 *
 ****************************************************************************/

#if SIM_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SIM_RegisterClient()
 *
 *     Registers a client for establishing SIM Access connections.  
 *
 * Parameters:
 *     Client - Client to register.  This structure is owned by the SIM Access 
 *     layer until the client is deregistered.
 *
 *     Callback - Callback function for receiving events related to the
 *         specified client.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The client was successfully registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The Client structure is already in use 
 *         (XA_ERROR_CHECK only).
 */
BtStatus SIM_RegisterClient(SimClient *Client, 
                            SimClientCallback Callback);

/*---------------------------------------------------------------------------
 * SIM_DeregisterClient()
 *
 *     Deregisters a SIM Access client.
 *
 * Parameters:
 *     Client - Client to deregister.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The Client was successfully deregistered.
 *
 *     BT_STATUS_BUSY - The Client has an open channel. All channels
 *         must be successfully closed with SIM_ClientClose before 
 *         calling this function (XA_ERROR_CHECK only).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_DeregisterClient(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientOpen()
 *
 *     Attempts to establish a connection with a remote SIM Access Server. The 
 *     application must identify the remote Server by device.
 * 
 * Parameters:
 *     RemDev - Identifies the remote device. 
 *
 *     Client - Identifies the Client for this action.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to open the client was sent.
 *         If the request is accepted by the remote device, a 
 *         SAEVENT_CLIENT_OPEN event will be generated. If the connection is 
 *         rejected, a SAEVENT_CLIENT_CLOSED event will be generated.
 *
 *     BT_STATUS_BUSY - The client is open or in the process of opening the
 *         connection, or the connection is in parked mode.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 */
BtStatus SIM_ClientOpen(BtRemoteDevice *RemDev, SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientAtrReq()
 *
 *     Send an ATR request to a server.
 *
 * Parameters:
 *     Client - Identifies the Client for this action.
 *
 *     See the SIM Access profile for more information on the use of ATR
 *     requests.
 *
 * Returns:    
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_APDU_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientAtrReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientApduReq()
 *
 *     Send an APDU request to a server.
 *
 *     See the SIM Access profile for more information on the use of APDU
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the Client for this action.
 *
 *     Len - Identifies the length of the APDU.
 *
 *     Ptr - A pointer to the APDU data.
 *
 * Returns:    
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_APDU_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientApduReq(SimClient *Client, U16 Len, U8 *Apdu);

/*---------------------------------------------------------------------------
 * SIM_ClientSimOnReq()
 *
 *     Send an SIM ON request to the server.  When the request is sent, the 
 *     server should respond after powering on the SIM card.
 *
 *     See the SIM Access profile for more information on the use of SIM ON
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_SIM_ON_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientSimOnReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientSimOffReq()
 *
 *     Send an SIM OFF request to the server.  When the request is sent, the 
 *     server should respond after powering off the SIM card.
 *
 *     See the SIM Access profile for more information on the use of SIM OFF
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_SIM_OFF_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientSimOffReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientResetSimReq()
 *
 *     Send a Reset SIM request to the server.  When the request is sent, the 
 *     server should respond after resetting the SIM.
 *
 *     See the SIM Access profile for more information on the use of Reset SIM
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_RESET_SIM_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientResetSimReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientCardRdrStatusReq()
 *
 *     Send a Card Reader Status request to the server.  When the request is 
 *     sent, the server should respond with the status of the Card Reader.
 *
 *     See the SIM Access profile for more information on the use of Card Reader
 *     status requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_CARD_READER_STATUS_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientCardRdrStatusReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientClose()
 *
 *     Closes a SIM Access connection between two devices.  When the connection
 *     is closed, the application will receive an SAEVENT_CLIENT_CLOSED event.
 *
 *     If there are any outstanding requests when a connection is closed,
 *     an event will be received for each request with the status code set to 
 *     BT_STATUS_NO_CONNECTION.
 *
 * Parameters:
 *     Client - Identifies the Client connection to be closed. The 
 *         SAEVENT_CLIENT_CLOSED event indicates that the connection is closed 
 *         and a new client may attempt to connect to the Client.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to close the connection will be sent.
 *         The application will receive an SAEVENT_CLIENT_CLOSED event when the
 *         connection is closed.
 *
 *     BT_STATUS_BUSY - The Client is already in the process of disconnecting.
 *
 *     BT_STATUS_IN_PROGRESS - RFCOMM is already in the process of closing.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only). 
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on this Client.
 */
BtStatus SIM_ClientClose(SimClient *Client);

#endif /* SIM_CLIENT */

#endif /* __SIM_H */




