#ifndef __GOEP_H
#define __GOEP_H

/****************************************************************************
 *
 * File:          goep.h
 *
 * Description:   Public types, defines, and prototypes for accessing the 
 *                Bluetooth GOEP Object Exchange Profile program interface.
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

#include "obex.h"
#include "obexauth.h"

/*---------------------------------------------------------------------------
 * Generic Object Exchange Profile (GOEP) Layer 
 *
 *     The GOEP layer manages all interactions for applications and profiles
 *     layered above OBEX that desire conformance with the Generic Object
 *     Exchange profile. The GOEP layer provides structures for applications
 *     and profiles to use which simplify the sending and receiving of
 *     protocol compliant information via OBEX. In addition, the GOEP
 *     provides a multiplexing layer which provides support for running
 *     multiple GOEP based profiles simultaneously.
 *
 *     The following profiles are based on the GOEP:
 *       - The Object Push Profile (OPUSH)
 *       - The File Transfer Profile (FTP)
 *       - The Synchronization Profile (SYNC)
 */

/****************************************************************************
 *
 * Section: Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GOEP_NUM_OBEX_CONS constant
 *
 *     Defines the number of OBEX connections used in GOEP.  One GOEP 
 *     multiplexer is allocated for each OBEX connection.
 */
#ifndef GOEP_NUM_OBEX_CONS
#define GOEP_NUM_OBEX_CONS   1
#endif

#if GOEP_NUM_OBEX_CONS < 1
#error GOEP_NUM_OBEX_CONS must be greater than 0
#endif

/*---------------------------------------------------------------------------
 * GOEP_MAX_UNICODE_LEN constant
 *
 *     Defines the maximum number of characters possible in an object, 
 *     folder name, or queued unicode header (including the null-terminator). 
 *     The maximum value is 32,767 (or 0x7FFF). This value must be greater 
 *     than zero, however, it will in all likelihood be larger than one, 
 *     since most filenames exceed one byte in length.
 */
#ifndef GOEP_MAX_UNICODE_LEN
#define GOEP_MAX_UNICODE_LEN   128
#endif

#if GOEP_MAX_UNICODE_LEN < 1 || GOEP_MAX_UNICODE_LEN > 0x7FFF
#error GOEP_MAX_UNICODE_LEN must be between 1 and 32,767
#endif

/*---------------------------------------------------------------------------
 * GOEP_MAX_TYPE_LEN constant
 *
 *     Defines the maximum size, in characters of an object type (including
 *     the null-terminator). Default max type is 'text/x-vcard'. If set to
 *     zero, TYPE headers are not stored. The maximum value is 255.
 */
#ifndef GOEP_MAX_TYPE_LEN
#define GOEP_MAX_TYPE_LEN   13
#endif

/*---------------------------------------------------------------------------
 * GOEP_MAX_WHO_LEN constant
 *
 *     Defines the maximum size, in characters of a WHO header. This size
 *     must be large enough to receive the largest WHO header which we are
 *     interested in. The WHO header is expected when we send a TARGET 
 *     header in the GoepConnectReq and the server has a matching service.
 *     If set to zero, WHO headers are not stored. The maximum value is 255.
 */
#ifndef GOEP_MAX_WHO_LEN 
#define GOEP_MAX_WHO_LEN    0
#endif

/*---------------------------------------------------------------------------
 * GOEP_DOES_UNICODE_CONVERSIONS constant
 *
 *     Defines the format of UNICODE data passed between the GOEP API and the
 *     layered application or profile. If enabled, the GOEP layer expects
 *     all Name and Description data pointers (such as in GoepPushReq) as 
 *     well as all queued unicode header data pointers (such as a unicode 
 *     header queued via GOEP_ClientQueueHeader or GOEP_ServerQueueHeader) to 
 *     point to an ASCII string. In this case the GOEP layer will convert the 
 *     string to UNICODE before transmission. Similarly all received UNICODE 
 *     strings are converted to ASCII. If disabled, all UNICODE headers are 
 *     expected to be provided in UNICODE format.
 */
#ifndef GOEP_DOES_UNICODE_CONVERSIONS
#define GOEP_DOES_UNICODE_CONVERSIONS   XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * GOEP_ADDITIONAL_HEADERS constant
 *
 *     Defines the number of additional headers allowed per GOEP connection. 
 *     Additional headers are sent when a GOEP operation is sent or responded 
 *     to.
 *
 *     NOTE: Larger numbers of additional headers are allowed, but caution 
 *           should be done in allowing this, since some devices may support
 *           only the minimum OBEX packet size (255 bytes).
 *           
 */
#ifndef GOEP_ADDITIONAL_HEADERS
#define GOEP_ADDITIONAL_HEADERS     3
#endif

#if GOEP_ADDITIONAL_HEADERS > 255
#error GOEP_ADDITIONAL_HEADERS must be less than 256
#endif

/*---------------------------------------------------------------------------
 * GOEP_CLIENT_HB_SIZE constant
 * 
 *     Transmit Header block construction buffer size. The upper limit on the
 *     header block size is the OBEX packet size. For RAM efficiency this
 *     value should be calculated as follows. The most header data the GOEP
 *     will ever send is in a client PUT which sends a Name header and a
 *     Length header. The max name length GOEP_MAXIMUM_NAME_LEN times 2 for
 *     UNICODE + 3 bytes for the OBEX Name header. Add in a length header
 *     (5 bytes) and you get the total. 
 *     Now when authentication is enabled there is the issue of space for
 *     the Authentication Response and/or Challenge header(s). There is
 *     likely enough space already allocated by for the NAME & TYPE headers
 *     that so long as they aren't all in the same packet, the is no need
 *     to allocate additional space.
 */
#ifndef GOEP_CLIENT_HB_SIZE
#define GOEP_CLIENT_HB_SIZE   ((GOEP_MAX_UNICODE_LEN*2)+3+5+GOEP_MAX_TYPE_LEN)
#endif

#if GOEP_CLIENT_HB_SIZE > 0xFFFF
#error GOEP_CLIENT_HB_SIZE must be less than 65,535
#endif

#if GOEP_CLIENT_HB_SIZE == 0
#error GOEP_CLIENT_HB_SIZE must be greater than 0
#endif

/*---------------------------------------------------------------------------
 * GOEP_SERVER_HB_SIZE constant
 * 
 *     The server transmit header block must be large enough to send an
 *     OBEX length header (5 bytes). If Authentication is enabled, it must
 *     be large enough to hold an AuthResponse (18+18+22) or an AuthChallenge
 *     (18+3+MaxRealmLength). If we make the Header block the size of the
 *     AuthResponse then that leaves 35 bytes for MaxRealmLength, which seems
 *     like a comfortable value. So that's what is done here. (The server
 *     never sends both AuthResponse & AuthChallenge in one packet).
 */
#ifndef GOEP_SERVER_HB_SIZE
#if OBEX_AUTHENTICATION == XA_ENABLED
#define GOEP_SERVER_HB_SIZE   (5 + (18+18+22))
#else
#define GOEP_SERVER_HB_SIZE   (5)
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif

#if GOEP_SERVER_HB_SIZE > 0xFFFF
#error GOEP_SERVER_HB_SIZE must be less than 65,535
#endif

#if GOEP_SERVER_HB_SIZE == 0
#error GOEP_SERVER_HB_SIZE must be greater than 0
#endif

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GoepOperation type
 *
 *     The GoepOperation type defines operations based on the Generic
 *     Object Exchange Profile's operations.
 */
typedef U8 GoepOperation;

/* This value is used when indicating events which do not occur during
 * an operation, such as TP Connect and TP Disconnect.
 */
#define GOEP_OPER_NONE              0

/* A Connect operation is in progress. The info.connect field in the
 * event structure is valid.
 */
#define GOEP_OPER_CONNECT           1

/* A Disconnect operation is in progress. */
#define GOEP_OPER_DISCONNECT        2

/* A Push operation is in progress. The info.pushpull or info.push field
 * in the event structure is valid.
 */
#define GOEP_OPER_PUSH              3

/* A Delete operation is in progress. The info.pushpull field in the 
 * event structure is valid.
 */
#define GOEP_OPER_DELETE            4

/* A Push operation is in progress. The info.pushpull field in the
 * event structure is valid.
 */
#define GOEP_OPER_PULL              5

/* A Set folder operation is in progress. The info.setfolder field in the
 * event structure is valid.
 */
#define GOEP_OPER_SETFOLDER         6

/* An abort operation is in progress. This event will be delivered while
 * another operation is in progress, if the ABORT is sent to stop that
 * operation.
 */
#define GOEP_OPER_ABORT             7

/* End of GoepOperation */

/*---------------------------------------------------------------------------
 * GoepEventType type
 *
 *     The GoepEventType type defines the events that may be indicated to
 *     the Client and Server applications.
 */
typedef U8 GoepEventType;

/* Reserved event type, unused by the GOEP protocol. 
 */
#define GOEP_EVENT_NONE             0

/* Indicates the start of a new operation (Server Only).  In the case of an Abort
 * operation, this event may occur during another pending operation (Put or Get).
 * In the case where GOEP_ServerContinue() has not been called during a Put or 
 * Get, perhaps awaiting Server object creation or additional buffer space, it 
 * should be called now or the Abort operation will not complete.
 */
#define GOEP_EVENT_START            1

/* Delivered to the application when it is time to issue
 * another request or response. The application must call either
 * GOEP_ServerContinue() or GOEP_ClientContinue().
 */
#define GOEP_EVENT_CONTINUE         2

#if OBEX_AUTHENTICATION == XA_ENABLED
/* Indicates that an authentication challenge request has
 * been received and is available in the event challenge field. If
 * the application intends on responding to this challenge it should do
 * so during this event notification. This is because there is a risk
 * that the peer may send more than one challenge and the GOEP layer
 * only tracks the last challenge received. However, it is not required
 * that the application respond during the event, as long as the
 * application always responds to the last challenge received.
 */
#define GOEP_EVENT_AUTH_CHALLENGE   3

/* Indicates that an authentication response has been
 * received and is available in the event response field.
 */
#define GOEP_EVENT_AUTH_RESPONSE    4
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/* An OBEX header was received that was not handled internally in GOEP.
 */
#define GOEP_EVENT_HEADER_RX        5

/* Delivered to server applications before the last response is going
 * to be sent. It provides the server with an opportunity to complete 
 * the requested operation (such as a SetFolder request) and if necessary
 * set the response code before the final response is sent (Server Only).
 */
#define GOEP_EVENT_PRECOMPLETE      6

/* Instructs the server to provide an object store handle to the protocol.
 * This event is generated during the processing of Put and Get operations.
 * The server calls GOEP_ServerAccept() to provide the object. Failure to 
 * do so will cause the operation to be rejected by the protocol. (Server Only)
 */
#define GOEP_EVENT_PROVIDE_OBJECT   7

/* Instructs the server to delete an object. This event is generated 
 * during the processing of a Put-Delete operation. The delete is assumed
 * to be successful, so the server must call GOEP_ServerAbort() if the 
 * object could not be deleted. 
 */
#define GOEP_EVENT_DELETE_OBJECT    8

/* Indicates that the operation has completed successfully.
 */
#define GOEP_EVENT_COMPLETE         9

/* Indicates that the current operation has failed.
 */
#define GOEP_EVENT_ABORTED          10

/* A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define GOEP_EVENT_TP_CONNECTED     11

/* The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define GOEP_EVENT_TP_DISCONNECTED  12

/* The transport layer discovery process has failed. (Client only)
 */
#define GOEP_EVENT_DISCOVERY_FAILED 13

/* The transport layer OBEX service cannot be found. (Client only)
 */
#define GOEP_EVENT_NO_SERVICE_FOUND 14

/* End of GoepEventType */


/*---------------------------------------------------------------------------
 * GoepUniType type
 *
 *     This type is used for parameters that are encoded in UNICODE when
 *     sent over OBEX. The format of these parameters depends on whether
 *     the GOEP layer or the application is handling the UNICODE formatting.
 */
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
typedef U8      GoepUniType;
#else
typedef U16     GoepUniType;
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */

/* End of GoepUniType */

/*---------------------------------------------------------------------------
 * GoepProfile type
 *
 *     The GOEP Profile types are used in the 'type' parameter of the client
 *     and server registration structures.
 */
typedef U8 GoepProfile;

#define GOEP_PROFILE_OPUSH      0   /* Object Push profile */
#define GOEP_PROFILE_FTP        1   /* File Transfer profile */
#define GOEP_PROFILE_SYNC       2   /* Sync profile */
#define GOEP_PROFILE_BPP        3   /* Basic Printing profile */
#define GOEP_PROFILE_BPP_PBR    4   /* Basic Printing profile PBR */
#define GOEP_PROFILE_BIP        5   /* Basic Imaging profile */
#define GOEP_PROFILE_PBAP       6   /* Phonebook Access profile */
#define GOEP_MAX_PROFILES       7   /* Number of supported profiles */

/* End of GoepProfile */

/*---------------------------------------------------------------------------
 * GoepConnTypes
 *
 *     Generic Object Exchange connection types for connection management 
 *     (for Client and Server)
 */
typedef U8 GoepConnTypes;

#define GOEP_MUX_CONN       0x01    /* Multiplex an existing OBEX connection */
#define GOEP_NEW_CONN       0x02    /* Start a new OBEX connection, don't multiplex */

/* End of GoepConnTypes */

/*---------------------------------------------------------------------------
 * GoepRole
 *
 *     Generic Object Exchange Roles
 */
typedef U8 GoepRole;

#define GOEP_SERVER         0x01
#define GOEP_CLIENT         0x02
 
/* End of GoepRole */

/* Forward declaration of the callback parameters */
#if OBEX_ROLE_SERVER == XA_ENABLED
typedef struct _GoepServerEvent GoepServerEvent;

/*---------------------------------------------------------------------------
 * GoepServerCallback type
 *
 *     A function of this type is called to indicate GOEP events to the
 *     GOEP Server application. When the GOEP layer calls this function
 *     it provides a server event structure which contains information
 *     about the event. The EventType, Operation and ServerApp fields
 *     are always valid. Based on the value of 'oper' the corresponding
 *     'info' subfield can also be interpreted. The values in the 'info'
 *     subfield are valid for the duration of the operation (from GOEP_EVENT
 *     START to GOEP_EVENT_COMPLETE (or GOEP_EVENT_ABORTED).
 */
typedef void (*GoepServerCallback)(GoepServerEvent *Event);
/* End of GoepServerCallback type */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
typedef struct _GoepClientEvent GoepClientEvent;

/*---------------------------------------------------------------------------
 * GoepClientCallback type
 *
 *     A function of this type is called to indicate GOEP events to the
 *     GOEP Client application. When the GOEP layer calls this function
 *     it provides a client event structure which contains information
 *     about the event. The EventType, Operation and ClientApp fields
 *     are always valid. Based on the value of 'oper' the corresponding
 *     'info' subfield can also be interpreted. The values in the 'info'
 *     subfield are valid for the duration of the operation (from GOEP_EVENT
 *     START to GOEP_EVENT_COMPLETE (or GOEP_EVENT_ABORTED).
 */
typedef void (*GoepClientCallback)(GoepClientEvent *Event);
/* End of GoepClientCallback type */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * SEC_GOEP_ID
 *
 * Security ID for GOEP access.
 */
#define SEC_GOEP_ID (BtProtocolId)GoepSecCallback
void GoepSecCallback(const BtEvent *Event);
/* End of GoepSecCallback */
#endif /* BT_SECURITY == XA_ENABLED */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GoepHeaderInd structure
 * 
 *     This structure is used by the during Header indications.
 */
typedef struct _GoepHeaderInd {
    ObexHeaderType type;            /* OBEX Header Type */

    /* Group: These fields are valid for 1-byte and 4-byte headers only */
    U32            value;           /* 1-byte or 4-byte header value */

    /* Group: These fields are valid for Unicode and Byte-Sequence headers only */
    U8            *buffer;          /* Header buffer */
    U16            len;             /* Length of the header buffer */

    /* Group: These fields are valid for Byte-Sequence headers only */
    U16            totalLen;        /* Total header length */
    U16            remainLen;       /* Remaining header length */

    /* Group: These fields are valid for Unicode headers only */

    /* The format of the unicode buffer (ASCII/UNICODE) is determined by the 
     * GOEP_DOES_UNICODE_CONVERSIONS option. 
     */
    GoepUniType    unicodeBuffer[GOEP_MAX_UNICODE_LEN]; /* "buffer" points to this data */

} GoepHeaderInd;

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * GoepServerApp structure
 *
 *     This structure is used by the Server Application (or Profile) to
 *     register with the GOEP Server multiplexer. If target information
 *     is provided, the GOEP Server takes care of registering it. 
 *
 *     If registering directly, using the GOEP_RegisterServer() function all
 *     fields must be filled in prior to registration.
 *
 *     If registering via a profile manager, such as GOEP_RegisterFtpServer()
 *     only the 'callback' field should be filled in.
 */
typedef struct _GoepServerApp {
    /* Group: These fields must be filled in prior to registration */
    GoepServerCallback      callback;
    
    /* Group: These fields must be filled in only if using GOEP_RegisterServer() */
    GoepProfile             type;       /* GOEP_PROFILE_xxx define */
    GoepServerCallback      appParent;  /* Must be set to zero if unused */
#if OBEX_SERVER_CONS_SIZE > 0
    ObexConnection         *target[OBEX_SERVER_CONS_SIZE];
    U32                     numTargets; /* Must be set to zero if unused */
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    GoepConnTypes           connFlags;  /* Type of connection - new or multiplexed */
    /* This field must be filled in if connFlags is GOEP_MUX_CONN.  It will be
     * filled in automatically otherwise. 
     */
    U8                      connId;     /* Id of the Obex connection - existing or new */

    /* === Internal use only === */
#if BT_SECURITY == XA_ENABLED
    BtSecurityRecord        secRecord;
    BtSecurityToken         secToken;
#endif /* BT_SECURITY == XA_ENABLED */
} GoepServerApp;

#if GOEP_MAX_WHO_LEN > 0
/*---------------------------------------------------------------------------
 * GoepConnectInd structure
 * 
 *     This structure is used by the GOEP Server during OBEX Connect event
 *     indications.
 */
typedef struct _GoepConnectInd {
    /* If the client specified a WHO header in the OBEX Connect request,
     * this field contains that information. 'whoLen' specifies its length.
     */
    U8                  who[GOEP_MAX_WHO_LEN];
    U16                 whoLen;
} GoepConnectInd;
#endif /* GOEP_MAX_WHO_LEN > 0 */

/*---------------------------------------------------------------------------
 * GoepObjectInd structure
 *
 *     Provides information to a GOEP Server about a received PUSH,
 *     PULL or DELETE indication.
 */
typedef struct _GoepObjectInd {
    BOOL            finalBit;
    /* Name of the object, null-terminated. The format (ASCII/UNICODE)
     * is determined by the GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType     name[GOEP_MAX_UNICODE_LEN];

    /* Length (in bytes) of the 'name' field. */
    U16             nameLen;

#if GOEP_MAX_TYPE_LEN > 0
    /* Type of the object, null-terminated, in ASCII format.
     */
    U8              type[GOEP_MAX_TYPE_LEN];

    /* Length (in bytes) of the 'type' field. */
    U16             typeLen;
#endif /* GOEP_MAX_TYPE_LEN > 0 */

    /* In a PUSH indication, this field indicates the length of the 
     * incoming object. In a PULL indication, this field should be set
     * by the user during the PROVIDE_OBJECT event, to the length of the object.
     * If it's zero, either the object has a zero length or the length
     * information was not available. This field is not used by DELETE.
     */
    U32             objectLen;
} GoepObjectInd;

/*---------------------------------------------------------------------------
 * GoepSetFolderInd structure
 * 
 *     This structure is used by the Server during SetFolder indications.
 */
typedef struct _GoepSetFolderInd {
    /* The client requested a reset to the root folder. */
    BOOL                reset;

    /* OBEX SetPath operation options. See OSPF_ defines from obex.h */
    ObexSetPathFlags    flags;

    /* The name of the requested folder to change to, null-terminated. 
     * The format (ASCII/UNICODE) is determined by the 
     * GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType         name[GOEP_MAX_UNICODE_LEN];
    U16                 nameLen;
} GoepSetFolderInd;

/*---------------------------------------------------------------------------
 * GoepServerEvent structure
 * 
 *     This structure is provided to the Server application each time an
 *     event is indicated. The 'event', 'oper' and 'handler' fields are
 *     always valid. Based on the 'oper' and/or the 'event' field the
 *     server can identify which of the 'info' structures is valid.
 */ 
struct _GoepServerEvent {
    GoepEventType           event;      /* GOEP_EVENT_xxx */
    GoepOperation           oper;       /* GOEP_OPER_xxx */
    GoepServerApp          *handler;    /* App's registration structure */

    union {
#if GOEP_MAX_WHO_LEN > 0
        GoepConnectInd      connect;    /* Use during GOEP_OPER_CONNECT */
#endif /* GOEP_MAX_WHO_LEN > 0 */
        GoepObjectInd       pushpull;   /* Use during PUSH/PULL/DELETE */
        GoepSetFolderInd    setfolder;  /* Use during SETFOLDER */
    } info;

    GoepHeaderInd           header;     /* Use during GOEP_EVENT_HEADER_RX */

#if OBEX_AUTHENTICATION == XA_ENABLED
    ObexAuthChallengeInfo   challenge;  /* OBEX Challenge parameters */
    ObexAuthResponseInfo    response;   /* OBEX Auth Response parameters */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
};
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */


#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GoepClientApp structure
 *
 *     This structure is used by the Client Application (or Profile) to 
 *     register with the GOEP Client multiplexer.
 *
 *     If registering directly, using the GOEP_RegisterClient() function all
 *     fields must be filled in prior to registration.
 *
 *     If registering via a profile manager, such as GOEP_RegisterFtpClient()
 *     only the 'callback' field should be filled in.
 */
typedef struct _GoepClientApp {
    /* Group: These fields must be filled in prior to registration */
    GoepClientCallback      callback;

    /* Group: These fields must be filled in only if using GOEP_RegisterClient() */
    GoepProfile             type;       /* GOEP_PROFILE_xxx define */
    GoepClientCallback      appParent;  /* Must be set to zero if unused */

    GoepConnTypes           connFlags;   /* Type of connection - new or multiplexed */
    U8                      connId;      /* Id of the Obex connection - existing or new */

    /* === Internal use only === */
#if BT_SECURITY == XA_ENABLED
    BtSecurityRecord        secRecord;
    BtSecurityToken         secToken;
#endif /* BT_SECURITY == XA_ENABLED */
    U32                     obexConnId;
    U8                      connState;
#if TI_CHANGES == XA_ENABLED
    BOOL                    cancelCreateConn;
#endif /* TI_CHANGES == XA_ENABLED */
} GoepClientApp;


/*---------------------------------------------------------------------------
 * GoepConnectReq structure
 *
 *     This structure is used by the Client when sending an OBEX Connect.
 *     All fields are optional.
 */
typedef struct _GoepConnectReq {
    /* This field specifies the Target server application. */
    const U8            *target;

    /* This field specifies the local client application. */
    const U8            *who;

    /* The length of the string pointed to by the 'target' field. */
    U8                   targetLen;

    /* The length of the string pointed to by the 'who' field. */
    U16                  whoLen;

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* If enabled, these two fields are used to challenge the server and
     * to respond to a received challenge request.
     */
    ObexAuthChallenge   *challenge;
    ObexAuthResponse    *response;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
} GoepConnectReq;

#if GOEP_MAX_WHO_LEN > 0
/*---------------------------------------------------------------------------
 * GoepConnectCnf structure
 *
 *     This structure is used by the GOEP Client during OBEX Connect event
 *     confirmations.
 */
typedef struct _GoepConnectCnf {
    /* If the server responded with a WHO header in the OBEX Connect response,
     * this field contains that information. 'whoLen' specifies its length.
     */
    U8                  who[GOEP_MAX_WHO_LEN];
    U16                 whoLen;
} GoepConnectCnf;
#endif /* GOEP_MAX_WHO_LEN > 0 */

/*---------------------------------------------------------------------------
 * GoepObjectReq structure
 *
 *     Used by the Client to provide information to GOEP during a Push,
 *     Pull or Delete request.
 */
typedef struct _GoepObjectReq {
    /* The Name of the object, must be null-terminated. The proper format
     * (ASCII/UNICODE) is based on the GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    const GoepUniType   *name;

    /* The Type of the object, null-terminated, in ASCII format.
     */
    const U8            *type;

    /* The handle of the object. */
    void                *object;

    /* In a PUSH request, the length of the object should be provided.
     * In PULL and DELETE requests this field is not used.
     */
    U32                  objectLen;

} GoepObjectReq;

/*---------------------------------------------------------------------------
 * GoepSetFolderReq structure
 * 
 *     This structure is used by the Client when issuing a Set Folder
 *     request.
 */
typedef struct _GoepSetFolderReq {
    /* Name of the folder, null-terminated. The format (ASCII/UNICODE)
     * is determined by the GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    const GoepUniType  *name;

    /* OBEX SetPath operation options. See OSPF_ defines from obex.h */
    ObexSetPathFlags    flags;

    /* Set this flag to true to reset the path to the root folder. 
     * No path flags may be specified if this is TRUE.  In addition,
     * any name provided will be ignored if this is TRUE, since reset 
     * uses an empty name header.
     */
    BOOL                reset;

} GoepSetFolderReq, GoepFolderReq;

/*---------------------------------------------------------------------------
 * GoepPullCnf structure
 *
 *     This structure is indicated to the Client during PULL operations to
 *     provide information that may have been sent back to the client as 
 *     part of a pull operation.
 */
typedef struct _GoepPullCnf {
    /* This field indicates the length of the object being pulled. If zero,
     * then the server did not provide a value, or the object has no content.
     */
    U32             objectLen;
} GoepPullCnf;

/*---------------------------------------------------------------------------
 * GoepClientEvent structure
 * 
 *     This structure is provided to the Client application each time an
 *     event is indicated. The 'event', 'oper' and 'handler' fields are
 *     always valid. Based on the 'oper' and/or the 'event' field the
 *     client can identify which of the 'info' structures is valid.
 */
struct _GoepClientEvent {
    GoepEventType           event;      /* GOEP_EVENT_xxx */
    GoepOperation           oper;       /* GOEP_OPER_xxx */
    GoepClientApp          *handler;    /* App's registration structure */

    ObexRespCode            reason;     /* Check during GOEP_EVENT_ABORTED */

    union {
#if GOEP_MAX_WHO_LEN > 0
        GoepConnectCnf      connect;    /* Use during GOEP_OPER_CONNECT */
#endif /* GOEP_MAX_WHO_LEN > 0 */
        GoepPullCnf         pull;       /* Use during GOEP_OPER_PULL */
    } info;

    GoepHeaderInd           header;     /* Use during GOEP_EVENT_HEADER_RX */

#if OBEX_AUTHENTICATION == XA_ENABLED
    ObexAuthChallengeInfo   challenge;  /* OBEX Challenge parameters */
    ObexAuthResponseInfo    response;   /* OBEX Auth Response parameters */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
};
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GOEP_Init()
 *
 *      Initialize the GOEP component.  This must be the first GOEP 
 *      function called by the application layer, or if multiple 
 *      GOEP applications exist, this function should be called
 *      at system startup (see XA_LOAD_LIST in config.h).  OBEX must 
 *      also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL GOEP_Init(void);

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterServer()
 *
 *     This function is used by an application or profile to register a
 *     service with the GOEP Server multiplexor.
 *     
 * Parameters:
 *     Service - A completed GoepServerApp structure describing the
 *          service must be provided. If registration is successful,
 *          this structure becomes property of the GOEP layer until
 *          GOEP_DeregisterServer() is called.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Server is initialized and the target
 *          registered (if provided).
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Server or registering
 *          the target (if provided).
 */
ObStatus GOEP_RegisterServer(GoepServerApp *Service, const ObStoreFuncTable *obStoreFuncs);

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterServerSecurityRecord()
 *
 *     Registers a security record for the GOEP service.  This is
 *     level 2 security.  An application must have a pairing handler 
 *     registered in order to receive PIN requests.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 *     Level - Sets the security level (See BtSecurityLevel).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully registered.
 *
 *     BT_STATUS_FAILED - The security record could not be registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_RegisterServerSecurityRecord(GoepServerApp *Service, BtSecurityLevel Level);

/*---------------------------------------------------------------------------
 * GOEP_UnregisterServerSecurityRecord()
 *
 *     Unregisters a security record for the GOEP service.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully unregistered.
 *
 *     BT_STATUS_FAILED - The security record was not registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_UnregisterServerSecurityRecord(GoepServerApp *Service);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_GetObexServer()
 *
 *     Retrieves the OBEX Server pertaining to a specific GOEP Server.  This 
 *     function is valid after GOEP_Init and GOEP_RegisterServer have been called.
 *     
 * Parameters:
 *     Service - A registered GoepServerApp structure. 
 *
 * Returns: 
 *     ObexServerApp pointer
 *
 */
ObexServerApp* GOEP_GetObexServer(GoepServerApp *Service);

#if (TI_CHANGES == XA_ENABLED)
/*---------------------------------------------------------------------------
 * GOEP_SetObexObjLen()
 *
 *     Set the object length for the OBEX Server pertaining to a specific GOEP Server.  This 
 *     function is valid after GOEP_Init and GOEP_RegisterServer have been called.
 *     
 * Parameters:
 *     Service - A registered GoepServerApp structure. 
 *     objectLen - the object length of the OBEX server.
 *
 * Returns: 
 *     OB_STATUS_SUCCESS - set operation succeeded.
 *
 *     OB_STATUS_FAILED - set operation failed.
 *
 */
ObStatus GOEP_SetObexObjLen(GoepServerApp *Service, U32 objectLen);
#endif

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_DeregisterServer()
 *
 *     This function is used to deregister a previously registered server.
 *     
 * Parameters:
 *     Service - The application structure used to register the server.
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
ObStatus GOEP_DeregisterServer(GoepServerApp *Service);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_AssignServerConnId()
 *
 *     Assign the connection id from the source GOEP server to the 
 *     destination GOEP server.  This function is used when setting up
 *     a new GOEP server which will multiplex off of an existing server's 
 *     connId.
 *
 * Parameters: 
 *      source - Original GOEP server
 *
 *      dest - New GOEP server that inherits the connection ID from
 *          "source".
 */
void GOEP_AssignServerConnId(GoepServerApp *source, GoepServerApp *dest);

#define GOEP_AssignServerConnId(_SOURCE, _DEST) \
            (_DEST)->connId = (_SOURCE)->connId;

/*---------------------------------------------------------------------------
 * GOEP_ServerAbort()
 *
 *     Aborts the current server operation.
 *     
 * Parameters:
 *     Service - The server's registration handle. If zero, the GOEP layer
 *         automatically selects the active server.
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
ObStatus GOEP_ServerAbort(GoepServerApp *Service, ObexRespCode Resp);

/*---------------------------------------------------------------------------
 * GOEP_ServerAccept()
 *
 *     This function is called to accept a Push or Pull request.
 *     It MUST be called during the GOEP_EVENT_PROVIDE_OBJECT indication for
 *     these operations. Failure to do so will abort the operation.
 *     
 * Parameters:
 *     Service - The active server's registration handle.
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
ObStatus GOEP_ServerAccept(GoepServerApp *Service, void *Obsh);

#if OBEX_BODYLESS_GET == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerAcceptNoObject()
 *
 *     Indicates the server should accept the GET without supplying an
 *     object (bodyless GET).
 *     
 * Requires:
 *     OBEX_BODYLESS_GET is set to XA_ENABLED
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server does not have an operation in progress.
 *
 *     OB_STATUS_SUCCESS - The object was accepted.
 *     
 */
ObStatus GOEP_ServerAcceptNoObject(GoepServerApp *Service);
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_ServerContinue()
 *
 *     This function is called by the server in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus GOEP_ServerContinue(GoepServerApp *Service);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerAuthenticate()
 *
 *     This function is used to either respond to an authentication challenge
 *     received from a client, or to send a challenge to the client. The
 *     server never sends both headers at the same time. If the client's
 *     request is challenged, it is automatically rejected with the reason
 *     OBRC_UNAUTHORIZED and the oper will complete with GOEP_EVENT_ABORTED.
 *
 * Parameters:
 *     Service - The server processing the Authentication.
 *
 *     Response - The authentication response to a received challenge.
 *
 *     Challenge - The authentication challenge.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server is not the active server or the GOEP
 *          encountered an error building the authentication header.
 *
 *     OB_STATUS_INVALID_PARM - The supplied authentication information
 *          conflicts with what is expected or acceptable.
 *
 *     OB_STATUS_SUCCESS - The authentication information was accepted.
 */
ObStatus GOEP_ServerAuthenticate(GoepServerApp *Service, 
                                 ObexAuthResponse *Response, 
                                 ObexAuthChallenge *Challenge);

/*---------------------------------------------------------------------------
 * GOEP_ServerVerifyAuthResponse()
 *
 *     This function is used by the Server to verify an authentication
 *     response received from the client. The server should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Service - The active server's registration handle.
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
BOOL GOEP_ServerVerifyAuthResponse(GoepServerApp *Service, U8 *Password, U8 PasswordLen);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerGetTpConnInfo()
 *
 *     Retrieves OBEX transport layer connection information.  This 
 *     function can be called when a transport connection is active to 
 *     retrieve connection specific information.   
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *     
 *     tpInfo - Pointer to the structure which receives the connection
 *         information. The "size" parameter must be initialized to the
 *         sizeof() the structure.
 *
 * Returns:    
 *     TRUE - The tpInfo structure was successfully completed.
 *     FALSE - The transport is not connected (XA_ERROR_CHECK only).
 */
BOOL GOEP_ServerGetTpConnInfo(GoepServerApp *Service, 
                                   ObexTpConnInfo *tpInfo);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 * GOEP_ServerQueueHeader()
 *
 *     This function queues Byte Sequence, UNICODE, 1-byte, and 4-byte 
 *     OBEX headers for transmission by the GOEP Server.  Queued headers
 *     are transmitted when an operation is responded to with 
 *     GOEP_ServerContinue.
 *
 *     Valid Headers: 
 *          Count, Time, Description, Http, App Params,
 *          Creator ID, Object Class, Body and End Of Body.  
 *
 *     Body headers are typically formed automatically from the Object Store.  
 *     Only manually create Body headers for packets with out of the ordinary 
 *     Body header placement (e.g BPP placing a Body header in a GET request).
 *     Body headers sent in an OBEX response packet cannot span multiple OBEX
 *     packets.
 *
 *     As mentioned before, the OBEX headers are not actually built during the 
 *     processing of this function.  Therefore, it is possible that the building 
 *     of queued headers may fail when the actual OBEX packet is formed. If a
 *     header cannot be built, a failure will be returned from the function 
 *     that added the header, and the queued header that caused the error will 
 *     be purged.  At this point, a new header can be added in its place, or 
 *     the operation can be continued without the failed header.  Any additional 
 *     headers that are needed but cannot fit in the current OBEX packet should 
 *     be sent in the next packet sent by GOEP_ServerContinue during the 
 *     GOEP_EVENT_CONTINUE event.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 *     Type - OBEX header type to build.
 *
 *     Value - OBEX header data (including null-termination for unicode headers).
 *
 *     Len - Length of the OBEX header (including null-termination for unicode headers)
 *
 * Returns:    
 *     TRUE - OBEX Header was built successfully.
 *     FALSE - OBEX Header was not built successfully.
 */
BOOL GOEP_ServerQueueHeader(GoepServerApp *Service, ObexHeaderType Type, 
                            const U8 *Value, U16 Len);


/*---------------------------------------------------------------------------
 * GOEP_ServerFlushQueuedHeaders()
 *
 *     This function flushes any headers queued with 
 *     GOEP_ClientQueueHeader.  Since a header is not flushed by 
 *     GOEP until it has been sent, it is possible that an upper
 *     layer profile API may need to flush these queued headers in
 *     the case of an internal failure.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:    
 *     None.
 */
void GOEP_ServerFlushQueuedHeaders(GoepServerApp *Service);
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerTpConnect()
 *
 *     Initiates an OBEX Transport Connection to the specified client.  
 *     Completion or failure of the operation is reported by a call to the 
 *     application callback with an appropriate event. The events generated 
 *     as a result of this function returning pending are either 
 *     GOEP_EVENT_TP_CONNECTED, GOEP_EVENT_TP_DISCONNECTED, 
 *     GOEP_EVENT_DISCOVERY_FAILED or GOEP_EVENT_NO_SERVICE_FOUND.
 *     
 * Parameters:
 *     Server - The registered server requesting the connection.
 *
 *     Target - This structure describes the client to which the server
 *         wishes to connect.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_SUCCESS - The server is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the server
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The server is currently executing an operation.
 *
 */
ObStatus GOEP_ServerTpConnect(GoepServerApp *Service, ObexTpAddr *Target);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerTpDisconnect()
 *
 *     Initiates the disconnection of the servers transport connection.
 *     
 * Parameters:
 *     Server - The registered server requesting the disconnect.
 *     
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the application
 *         callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation failed because there is
 *         no server connection.
 *
 *     OB_STATUS_FAILED - Operation failed.
 */
ObStatus GOEP_ServerTpDisconnect(GoepServerApp *Service);
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterClient()
 *
 *     This function is used by an application or profile to register a
 *     client with the GOEP Client multiplexor.
 *     
 * Parameters:
 *     Client - A completed GoepClientApp structure describing the
 *          client must be provided. If registration is successful,
 *          this structure becomes property of the GOEP layer until
 *          GOEP_DeregisterClient() is called.
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
 */
ObStatus GOEP_RegisterClient(GoepClientApp *Client, const ObStoreFuncTable *obStoreFuncs);

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterClientSecurityRecord()
 *
 *     Registers a security record for the GOEP client.  This is
 *     level 2 security.  An application must have a pairing handler 
 *     registered in order to receive PIN requests.
 *
 * Parameters:
 *     Client - The active client's registration handle.
 *
 *     Level - Sets the security level (See BtSecurityLevel).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully registered.
 *
 *     BT_STATUS_FAILED - The security record could not be registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_RegisterClientSecurityRecord(GoepClientApp *Client, BtSecurityLevel Level);

/*---------------------------------------------------------------------------
 * GOEP_UnregisterClientSecurityRecord()
 *
 *     Unregisters a security record for the GOEP client.
 *
 * Parameters:
 *     Client - The active client's registration handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully unregistered.
 *
 *     BT_STATUS_FAILED - The security record was not registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_UnregisterClientSecurityRecord(GoepClientApp *Client);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_GetObexClient()
 *
 *     Retrieves the OBEX Client pertaining to a specific GOEP Client.  This
 *     function is valid after GOEP_Init and GOEP_RegisterClient have been called.
 *     
 * Parameters:
 *     Client - A registered GoepClientApp structure. 
 *
 * Returns: 
 *     ObexClientApp pointer
 *
 */
ObexClientApp* GOEP_GetObexClient(GoepClientApp *Client);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_DeregisterClient()
 *
 *     This function is used to deregister a previously registered client.
 *     
 * Parameters:
 *     Client - The GoepClientApp structure used to register the client.
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
 */
ObStatus GOEP_DeregisterClient(GoepClientApp *Client);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_AssignClientConnId()
 *
 *     Assign the connection id from the source GOEP client to the 
 *     destination GOEP client. This function is used when setting up
 *     a new GOEP client which will multiplex off of an existing client's 
 *     connId.
 *
 * Parameters:
 *     source - Original GOEP client
 *
 *     dest - New GOEP client that inherits the connection ID from
 *         "source".
 *
 */
void GOEP_AssignClientConnId(GoepClientApp *source, GoepClientApp *dest);

#define GOEP_AssignClientConnId(_SOURCE, _DEST) \
            (_DEST)->connId = (_SOURCE)->connId;

/*---------------------------------------------------------------------------
 * GOEP_Connect()
 *
 *     This function is used by the client to issue an OBEX Connect Request.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 *     Connect - Optional connect request parameters. This parameter may
 *         be zero if no optional headers are necessary.
 *
 * Returns:
 *     OB_STATUS_BUSY - Operation was not started because the client is
 *         currently executing another operation.
 *
 *     OB_STATUS_INVALID_PARM - The client provided information in the
 *          Connect structure which conflicts with what the GOEP client
 *          was expecting.
 *
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 */
ObStatus GOEP_Connect(GoepClientApp *Client, GoepConnectReq *Connect);

/*---------------------------------------------------------------------------
 * GOEP_Disconnect()
 *
 *     This function is used by the client to issue an OBEX Disconnect Request.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 * Returns:
 *     
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the
 *         application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 */
ObStatus GOEP_Disconnect(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * GOEP_Push()
 *
 *     Initiates the OBEX "Put" operation to send an object to the remote
 *     server. To perform a Delete operation, provide an object store 
 *     handle of zero and make sure OBEX_PUT_DELETE support is enabled.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
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
ObStatus GOEP_Push(GoepClientApp *Client, GoepObjectReq *Object);

/*---------------------------------------------------------------------------
 * GOEP_Pull()
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
 *     request will be sent when GOEP_ClientContinue is called during the 
 *     GOEP_EVENT_CONTINUE.  GOEP_ClientQueueHeader should be called before or 
 *     during this event to ensure they are sent in the next GET request packet.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
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
ObStatus GOEP_Pull(GoepClientApp *Client, GoepObjectReq *Object, BOOL More);

/*---------------------------------------------------------------------------
 * GOEP_SetFolder()
 *
 *     Performs the OBEX "SetPath" operation.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 *     Folder - This structure provides the parameters for the Set Folder
 *         request. It is required.
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
ObStatus GOEP_SetFolder(GoepClientApp *Client, GoepSetFolderReq *Folder);

/*---------------------------------------------------------------------------
 * GOEP_ClientAbort()
 *
 *     Aborts the current client operation. The completion event will signal
 *     the status of the operation in progress, either COMPLETE or ABORTED.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *         If zero, the GOEP layer automatically selects the active client.
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
ObStatus GOEP_ClientAbort(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * GOEP_ClientContinue()
 *
 *     This function is called by the client in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Client - The registered client who received the CONTINUE event.
 *
 * Returns:
 *     OB_STATUS_FAILED - The client is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus GOEP_ClientContinue(GoepClientApp *Client);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ClientVerifyAuthResponse()
 *
 *     This function is used by the Client to verify an authentication
 *     response received from the server. The client should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Client - The registered client requesting the connection.
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
BOOL GOEP_ClientVerifyAuthResponse(GoepClientApp *Client, U8 *Password, U8 PasswordLen);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ClientGetTpConnInfo
 *---------------------------------------------------------------------------
 *
 *     Retrieves OBEX transport layer connection information.  This 
 *     function can be called when a transport connection is active to 
 *     retrieve connection specific information.   
 *
 * Parameters:
 *     Client - The active client's registration handle.
 *     
 *     tpInfo - Pointer to the structure which receives the connection
 *         information. The "size" parameter must be initialized to the
 *         sizeof() the structure.
 *
 * Returns:    
 *     TRUE - The tpInfo structure was successfully completed.
 *     FALSE - The transport is not connected (XA_ERROR_CHECK only).
 *
 */
BOOL GOEP_ClientGetTpConnInfo(GoepClientApp *Client, 
                                   ObexTpConnInfo *tpInfo);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * GOEP_TpConnect()
 *
 *     Initiates an OBEX Transport Connection to the specified server.  
 *     Completion or failure of the operation is reported by a call to the 
 *     application callback with an appropriate event. The events generated 
 *     as a result of this function returning pending are either 
 *     GOEP_EVENT_TP_CONNECTED, GOEP_EVENT_TP_DISCONNECTED, 
 *     GOEP_EVENT_DISCOVERY_FAILED or GOEP_EVENT_NO_SERVICE_FOUND.
 *     
 * Parameters:
 *     Client - The registered client requesting the connection.
 *
 *     Target - This structure describes the server to which the client
 *         wishes to connect.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_SUCCESS - The client is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the client
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The client is currently executing an operation.
 *
 */
ObStatus GOEP_TpConnect(GoepClientApp *Client, ObexTpAddr *Target);

/*---------------------------------------------------------------------------
 * GOEP_TpDisconnect()
 *
 *     Initiates the disconnection of the clients transport connection.
 *     
 * Parameters:
 *     Client - The registered client requesting the disconnect.
 *     
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the application
 *         callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation failed because there is
 *         no client connection.
 *
 *     OB_STATUS_FAILED - Operation failed.
 */
ObStatus GOEP_TpDisconnect(GoepClientApp *Client);

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 * GOEP_ClientQueueHeader()
 *
 *     This function queues Byte Sequence, UNICODE, 1-byte, and 4-byte 
 *     OBEX headers for transmission by the GOEP Client.  Queued headers
 *     are transmitted when the next GOEP operation is initiated.  
 *     All operations except GOEP_ClientAbort will queue headers.
 *
 *     Valid Headers: 
 *          Count, Time, Description, Http, App Params,
 *          Creator ID, Object Class, Body and End Of Body.  
 *
 *     Body headers are typically formed automatically from the Object Store.  
 *     However, certain applications (such as the Basic Printing Profile) may
 *     require Body headers during a GET request.  Body headers during a GET
 *     request is the only situation where a header can span multiple OBEX 
 *     packets.  For this case only, GOEP will segment the Body header into 
 *     valid OBEX packet sizes.  
 *
 *     ALL other headers must combine to fit within a single OBEX packet.
 *
 *     As mentioned before, the OBEX headers are not actually built during the 
 *     processing of this function.  Therefore, it is possible that the building 
 *     of queued headers may fail when the actual OBEX packet is formed. If a
 *     header cannot be built, a failure will be returned from the function 
 *     that added the header, and the queued header that caused the error will 
 *     be purged.  At this point, a new header can be added in its place, or 
 *     the operation can be restarted or continued without the failed header.  
 *     Any additional headers that are needed but cannot fit in the current 
 *     OBEX packet should be sent in the next packet sent by GOEP_ClientContinue 
 *     during the GOEP_EVENT_CONTINUE event.
 *
 *     When calling GOEP_Pull again in response to its failure to add a queued 
 *     header, its "More" parameter should be set if additional headers need to 
 *     be added in the next GET request, sent during the GOEP_EVENT_CONTINUE 
 *     callback.  Setting the "More" parameter will continue the GET request 
 *     portion of the GET operation, where all client headers describing the
 *     requested object are sent prior to the object being returned.
 *
 * Parameters:
 *     Client - The active client's registration handle..
 *
 *     Type - OBEX header type to build.
 *
 *     Value - OBEX header data (including null-termination for unicode headers).
 *
 *     Len - Length of the OBEX header (including null-termination for unicode headers).
 *
 * Returns:    
 *     TRUE - OBEX Header was built successfully.
 *     FALSE - OBEX Header was not built successfully.
 */
BOOL GOEP_ClientQueueHeader(GoepClientApp *Client, ObexHeaderType Type, 
                            const U8 *Value, U16 Len);

/*---------------------------------------------------------------------------
 * GOEP_ClientFlushQueuedHeaders()
 *
 *     This function flushes any headers queued with 
 *     GOEP_ClientQueueHeader.  Since a header is not flushed by 
 *     GOEP until it has been sent, it is possible that an upper
 *     layer profile API may need to flush these queued headers in
 *     the case of an internal failure.
 *
 * Parameters:
 *     Client - The active client's registration handle..
 *
 * Returns:    
 *      None.
 */
void GOEP_ClientFlushQueuedHeaders(GoepClientApp *Client);
#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_GetConnectionId()
 *
 *     This function call facilitates the use of the OBEX client/server
 *     multiplexing capabilities of the GOEP layer, by retrieving a 
 *     connection Id (if one exists) for an OBEX client/server connection.
 *     A connection Id will be returned if an OBEX client/server connection 
 *     exists and if no instances of the requested profile are currently
 *     registered.  This is due to the fact that only one instance of a 
 *     specific profile is allowed over the same OBEX connection.  
 *     
 * Parameters: 
 *     role - GOEP client or GOEP server
 *     profileType - Type of profile
 *     
 * Returns:
 *     Connection Id - 0-254, 255 (No connection Id exists)
 */
U8 GOEP_GetConnectionId(GoepRole role, GoepProfile profileType);
#endif /* __GOEP_H */
