#ifndef __BPP_H
#define __BPP_H
/****************************************************************************
 *
 * File:          bpp.h
 *     $Workfile:bpp.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision:60$
 *
 * Description:   This file specifies defines and function prototypes for the
 *                BPP application.
 * 
 * Created:       December 10, 2001
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 1999-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

/*---------------------------------------------------------------------------
 * Basic Printing Profile (BPP) layer
 *
 *     The Basic Printing Profile (BPP) specification defines Sender and
 *     Printer roles to enable printing documents through a wireless
 *     Bluetooth connection.
 *     
 */

/****************************************************************************
 *
 * Constants
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Section: Default SDP values that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BPP_1284ID constant
 *     Defines the IEEE 1284 ID for the printer.  This value is stored in the
 *     SDP entry and must be terminated with a null character.
 */
#ifndef BPP_1284ID
#define BPP_1284ID \
    'M','F','G',':','E','X','T','E','N','D','E','D','-','S', \
    'Y','S','T','E','M','S',';','M','D','L',':','X','A','B', \
    'T','P','r','i','n','t','e','r',';','D','E','S',':','X', \
    'T','N','D','A','c','c','e','s','s',' ','B','l','u','e', \
    't','o','o','t','h',' ','P','r','i','n','t','e','r',';', \
    'S','N',':','1','2','3','4','5','\0'
#endif

/*---------------------------------------------------------------------------
 * BPP_1284ID_LEN constant
 *     Defines the length of the IEEE 1284 ID for the printer.  This value is 
 *     stored in the SDP entry.
 */
#ifndef BPP_1284ID_LEN
#define BPP_1284ID_LEN 79
#endif

/*---------------------------------------------------------------------------
 * BPP_DOC_FORMATS constant
 *     Defines the document formats supported by the printer. This value is
 *     stored in the SDP entry and must be terminated with a null character.
 */
#ifndef BPP_DOC_FORMATS
#define BPP_DOC_FORMATS \
    'a','p','p','l','i','c','a','t','i','o','n','/', \
    'v','n','d','.','p','w','g','-','x','h','t','m','l','-', \
    'p','r','i','n','t','+','x','m','l',':','1','.','0',',', \
    't','e','x','t','/','p','l','a','i','n','\0'
#endif

/*---------------------------------------------------------------------------
 * BPP_DOC_FORMATS_LEN constant
 *     Defines the length of the Document Formats String contained in the
 *     BPP_DOC_FORMATS constant. This value is stored in the SDP entry.
 */
#ifndef BPP_DOC_FORMATS_LEN
#define BPP_DOC_FORMATS_LEN 51
#endif

/*---------------------------------------------------------------------------
 * BPP_CHAR_REPERTOIRES constant
 *     Defines the characters supported by the printer. The characters
 *     are represented as a Uint128 bit mask, where each bit represents a
 *     supported character repertoire.
 */
#ifndef BPP_CHAR_REPERTOIRES
#define BPP_CHAR_REPERTOIRES \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff
#endif

/*---------------------------------------------------------------------------
 * BPP_IMG_FORMATS constant
 *     Defines the image formats supported by the printer. This value is
 *     stored in the SDP entry and must be terminated with a null character.
 */
#ifndef BPP_IMG_FORMATS
#define BPP_IMG_FORMATS \
    'i','m','a','g','e','/','j','p','e','g',',', \
    'i','m','a','g','e','/','g','i','f',':','8','9','A','\0'
#endif

/*---------------------------------------------------------------------------
 * BPP_IMG_FORMATS_LEN constant
 *     Defines the length of the Image Formats string contained in the
 *     BPP_IMG_FORMATS constant. This value is stored in the SDP entry.
 */
#ifndef BPP_IMG_FORMATS_LEN
#define BPP_IMG_FORMATS_LEN 25
#endif

/*---------------------------------------------------------------------------
 * BPP_COLOR_SUPPORTED constant
 *     Indicates whether the printer supports color printing. This value is
 *     stored in the SDP entry.
 */
#ifndef BPP_COLOR_SUPPORTED
#define BPP_COLOR_SUPPORTED FALSE
#endif

/*---------------------------------------------------------------------------
 * BPP_DUPLEX_SUPPORTED constant
 *     Indicates whether the printer supports duplex printing. This value is
 *     stored in the SDP entry.
 */
#ifndef BPP_DUPLEX_SUPPORTED
#define BPP_DUPLEX_SUPPORTED FALSE
#endif

/*---------------------------------------------------------------------------
 * BPP_MEDIA_TYPES constant
 *     Defines the Media Types supported by the printer. This value is
 *     stored in the SDP entry and must be terminated with a null character.
 */
#ifndef BPP_MEDIA_TYPES
#define BPP_MEDIA_TYPES \
    's','t','a','t','i','o','e','r','y',',', \
    'c','a','r','d','s','t','o','c','k',',', \
    'e','n','v','e','l','o','p','e','\0'
#endif

/*---------------------------------------------------------------------------
 * BPP_MEDIA_TYPES_LEN constant
 *     Defines the length of the Media Types string contained in the
 *     BPP_MEDIA_TYPES constant. This value is stored in the SDP entry.
 */
#ifndef BPP_MEDIA_TYPES_LEN
#define BPP_MEDIA_TYPES_LEN 29
#endif

/*---------------------------------------------------------------------------
 * BPP_MAX_MEDIA_WIDTH constant
 *     Defines the Maximum Media Width in millimeters supported by the
 *     printer. This value is stored in the SDP entry.
 */
#ifndef BPP_MAX_MEDIA_WIDTH
#define BPP_MAX_MEDIA_WIDTH 210
#endif

/*---------------------------------------------------------------------------
 * BPP_MAX_MEDIA_LENGTH constant
 *     Defines the Maximum Media Length in millimeters supported by the
 *     printer. This value is stored in the SDP entry.
 */
#ifndef BPP_MAX_MEDIA_LENGTH
#define BPP_MAX_MEDIA_LENGTH 297
#endif

/* Number of Printer SDP Attributes */
#define BPP_NUM_PRN_ATTRIBUTES    15
#define BPP_NUM_SND_ATTRIBUTES    5

/* Number of Sender SDP search attributes */
#define BPP_NUM_SEARCH_ATTRIBUTES 21


/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BPP_NUM_SENDERS Constant
 *
 *  Defines the number of concurrent Senders supported.
 *
 *  The default value is 1. Set to 0 to disable Sender support.
 */
#ifndef BPP_NUM_SENDERS
#define BPP_NUM_SENDERS             1
#endif

/*---------------------------------------------------------------------------
 * BPP_NUM_PRINTERS Constant
 *
 *  Defines the number of concurrent Printers supported.
 *
 *  The default value is 1. Set to 0 to disable Printer support.
 */
#ifndef BPP_NUM_PRINTERS
#define BPP_NUM_PRINTERS            1
#endif

#if TI_CHANGES == XA_ENABLED
#include "goep.h"
#else
#include "Goep.h"
#endif

/* bppobs.h must be included after BPP_NUM_SENDERS and BPP_NUM_PRINTERS
 * are defined.
 */
#include "bppobs.h"

/*---------------------------------------------------------------------------
 * Checks for correct configuration values.
 */
#if GOEP_MAX_TYPE_LEN == 0
#error "GOEP_MAX_TYPE_LEN must be greater than zero!"
#endif

#if GOEP_MAX_UNICODE_LEN == 0
#error "GOEP_MAX_UNICODE_LEN must be greater than zero!"
#endif

#if OBEX_SERVER_CONS_SIZE < 2
#error "OBEX_SERVER_CONS_SIZE must be 2 or greater!"
#endif

#if OBEX_DYNAMIC_OBJECT_SUPPORT != XA_ENABLED
#error "OBEX_DYNAMIC_OBJECT_SUPPORT must be enabled!"
#endif

#if GOEP_ADDITIONAL_HEADERS < 3
#error "GOEP_ADDITIONAL_HEADERS must be 3 or greater!"
#endif

#if OBEX_PROVIDE_SDP_RESULTS != XA_ENABLED
#error "OBEX_PROVIDE_SDP_RESULTS must be enabled!"
#endif

#if SDP_SERVER_SUPPORT != XA_ENABLED
#error "SDP_SERVER_SUPPORT must be enabled!"
#endif

#if SDP_CLIENT_SUPPORT != XA_ENABLED
#error "SDP_CLIENT_SUPPORT must be enabled!"
#endif

#if OBEX_ROLE_SERVER != XA_ENABLED
#error "OBEX_ROLE_SERVER must be enabled!"
#endif

#if OBEX_ROLE_CLIENT != XA_ENABLED
#error "OBEX_ROLE_CLIENT must be enabled!"
#endif


/*---------------------------------------------------------------------------
 * BPP_PRINTING_STATUS Constant
 *
 *  Enables/Disables the Status channel.
 *
 *  The default value is XA_ENABLED
 */
#ifndef BPP_PRINTING_STATUS
#define BPP_PRINTING_STATUS         XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * BPP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 *  The default value is 20.
 */
#ifndef BPP_MAX_PASSWORD_LEN
#define BPP_MAX_PASSWORD_LEN     20
#endif

/*---------------------------------------------------------------------------
 * BPP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 *  The default value is 20.
 */
#ifndef BPP_MAX_USERID_LEN
#define BPP_MAX_USERID_LEN       20
#endif

/*---------------------------------------------------------------------------
 * BPP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 *  The default value is 20.
 */
#ifndef BPP_MAX_REALM_LEN
#define BPP_MAX_REALM_LEN        20
#endif

/****************************************************************************
 *
 * Types
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BppEvent type
 *
 *  Indicates the current event type. All event indications and confirmations
 *  are passed to a callback function of type BppCallback. The
 *  "BppCallbackParms.event" field indicates one of the event types below.
 *  The "BppCallbackParms.data.oper" field indicates the current operation
 *  for which the event applies.
 *      
 */
typedef U8 BppEvent;


/* Group: Client Events */

/* The client pointer is valid. Document data must be provided through the
 * parms.data.buff pointer. The parms.data.len field is valid indicating the
 * requested data size and the parms.data.offset field indicates the data
 * offset. The parms.data.ocx field indicates the object context set when the
 * BPP_SendDocument or BPP_SimplePush call was made.
 * The status on this event is type ObStatus.
 */
#define BPPCLIENT_DATA_REQ  0x01

/* The client pointer is valid and indicates soap or Reference Object data with 
 * the parms.data.buff pointer. The parms.data.len, parms.data.offset and 
 * parms.data.totLen fields are valid indicating the data parameters.  When
 * received by the printer, this event indicates Reference Object data.
 * The status on this event is type ObStatus.
 */
#define BPPCLIENT_DATA_IND  0x02

/* The client pointer is valid. The parms.data.bppOp indicates the soap
 * response type. The soap object has been fully indicated in preceding
 * BPPCLIENT_DATA_IND event(s).
 * The status on this event is type ObStatus.
 */
#define BPPCLIENT_RESPONSE  0x03

/* The client pointer is valid. The client operation has completed. The
 * parms.status field indicates complete status.
 * The status on this event is type ObStatus.
 */
#define BPPCLIENT_COMPLETE  0x04

/* The client pointer is valid. The parms.status field indicates failure
 * reason and is type ObexRespCode.
 */
#define BPPCLIENT_FAILED    0x05

/* The client pointer is valid. The parms.status field indicates abort
 * reason and is type ObexRespCode.
 */
#define BPPCLIENT_ABORT     0x06

#if TI_CHANGES == XA_ENABLED

#define BPPCLIENT_PROGRESS          				(0x07)

/* Indicates that an authentication challenge request has
 * been received and is available in the event challenge field. If
 * the application intends on responding to this challenge it should do
 * so during this event notification. This is because there is a risk
 * that the peer may send more than one challenge and the GOEP layer
 * only tracks the last challenge received. However, it is not required
 * that the application respond during the event, as long as the
 * application always responds to the last challenge received.
 */
#define BPPCLIENT_AUTH_CHALLENGE_RCVD 		(0x08)

#define BPPCLIENT_DISABLED						(0x09)

#endif


/* Group: Server Events */

/* The server pointer is valid. Soap or Reference Object response data must be 
 * provided through the parms.data.buff pointer. The parms.data.len field is 
 * valid indicating the requested data size and the parms.data.offset field 
 * indicates the data offset. The parms.data.ocx field indicates the object 
 * context set when the BPP_SendResponse call was made.  When received by the 
 * sender, this event will be a request for a Reference Object data.
 * The status on this event is type ObStatus.  
 */
#define BPPSERVER_DATA_REQ  0x81

/* The server pointer is valid and indicates document or soap data has arrived.
 * The parms.data.len, parms.data.offset and parms.data.totLen fields are valid
 * indicating the data parameters. If parms.data.oper is BPPOP_SIMPLEPUSH or
 * BPPOP_SENDDOCUMENT, the parms.data.job structure contains a job description.
 * The status on this event is type ObStatus.
 */
#define BPPSERVER_DATA_IND  0x82

/* The server pointer is valid. The parms.data.bppOp field indicates the
 * operation type. This event will precede print requests (SendDocment or
 * SimplePush.) For soap objects, this event will indicate the object has
 * been fully indicated in preceding BPPSERVER_DATA_IND event(s).
 * When received by the sender, this event will be a request for a Reference 
 * Object (BPPOP_GETREFOBJ), and will precede requests for Object data.
 * The status on this event is type ObStatus.
 */
#define BPPSERVER_REQUEST   0x83

/* The server pointer is valid. The server operation has completed. The
 * parms.status field indicates complete status.
 * The status on this event is type ObStatus.
 */
#define BPPSERVER_COMPLETE  0x84

/* The server pointer is valid. The parms.status field indicates failure
 * reason and is type ObexRespCode.
 */
#define BPPSERVER_FAILED    0x85

/* The server pointer is valid. The parms.status field indicates abort
 * reason and is type ObexRespCode.
 */
#define BPPSERVER_ABORT     0x86

/* The server pointer is valid. Allows flow control on the server connection.
 * The server must call BPP_ServerContinue when it is ready to continue
 * the operation. Useful for Printer Flow Control.
 * The status on this event is type ObStatus.
 */
#define BPPSERVER_CONTINUE  0x87
/* End of BppEvent */


/*---------------------------------------------------------------------------
 * BppOp type
 *
 *  Indicates the operation type of the current event. Each event indication
 *  has an associated operation passed to a callback function of type
 *  BppCallback. The "BppCallbackParms.data.oper" field will indicate one
 *  of the operation types below.
 */
typedef U16 BppOp;

#define BPPOP_NOP             0x0000 /* No operation in progress */

/* Connect operation. See the "status" field to determine whether this
 * operation indicates a connect (OB_STATUS_SUCCESS) or a disconnect
 * (OB_STATUS_DISCONNECT) due to transport disconnection.
 *
 * In the case of disconnection on the printer side, any jobs created as
 * part of BPP_SendResponse to a BPPOP_CREATEJOB will be quietly removed
 * from the internal queue. The app is free to reuse BppPrintJob structures
 * at that point.
 */
#define BPPOP_CONNECT         0x2000 
#define BPPOP_SIMPLEPUSH      0x0001 /* Simple File Push operation */
#define BPPOP_CREATEJOB       0x0004 /* CreateJob Request/Response */
#define BPPOP_SENDDOCUMENT    0x0008 /* SendDocument Request */
#define BPPOP_GETJOBATTR      0x0020 /* GetJobAttributes Request/Response */
#define BPPOP_GETPRTATTR      0x0040 /* GetPrinterAttributes Request/Response */
#define BPPOP_CANCELJOB       0x0080 /* CancelJob Request/Response */
#define BPPOP_GETEVENT        0x0100 /* GetEvent Request/Response */
#define BPPOP_SOAP            0x1000 /* SOAP Request (Specific op not yet parsed) */

/* Get Referenced Object request from the printer. Fields included with
 * the BPPSERVER_REQUEST include: "buff" (specifies the name of the referenced
 * object; see GOEP_DOES_UNICODE_CONVERSIONS for format), offset (offset into
 * the object, in bytes), len (number of bytes requested or -1 to return
 * remainder of object), and "totLen" (as indicated by the printer).
 */
#define BPPOP_GETREFOBJ       0x4000

/* End of BppOp */

/* Other operation types */
#define BPPOP_SENDREFERENCE   0x0010 /* SendReference Request */
#define BPPOP_SIMPLEREFERENCE 0x0002 /* Simple Reference Push Request/Response */
#define BPPOP_CREATEPRECISE   0x0200 /* CreatePreciseJob Request/Response */
#define BPPOP_GETMARGINS      0x0400 /* GetMargins Request/Response */
#define BPPOP_GETRUI          0x0800 /* GetRUI Request/Response */

#if TI_CHANGES == XA_ENABLED

#define BPPOP_DISCONNECT          0x8000

#endif

/*---------------------------------------------------------------------------
 * BppChannel type
 *
 *  Indicates the channel type of the current API or event. Each event
 *  indication has an associated BppChannel passed to a callback function
 *  of type BppCallback. The "BppCallbackParms.channel" field will indicate
 *  one of the channel types below.
 */
typedef U8 BppChannel;

#define BPPCH_SND_JOB       0x01    /* Sender Job channel */
#define BPPCH_SND_STATUS    0x02    /* Sender Status channel */
#define BPPCH_PRN_JOB       0x81    /* Printer Job channel */
#define BPPCH_PRN_STATUS    0x82    /* Printer Status channel */
#define BPPCH_SND_OBJECT    0x03    /* Sender Object channel */
#define BPPCH_PRN_OBJECT    0x83    /* Printer Object channel */
/* End of BppChannel */

/* Other channels (not used) */
#define BPPCH_SND_RUI       0x04    /* Sender RUI channel */
#define BPPCH_PRN_RUI       0x84    /* Printer RUI channel */

/*---------------------------------------------------------------------------
 * BppService type
 *
 *  Describes the BPP Service type required on a connect operation.
 */
typedef U16 BppService;

/* BPP Direct Printing Service */
#define BPPSVC_DPS      SC_DIRECT_PRINTING

/* BPP Printing Status Service */
#define BPPSVC_STS      SC_PRINTING_STATUS

/* BPP Referenced Objects Service. Initiated by a Printer to get
 * referenced objects.
 */
#define BPPSVC_REF_OBJ  SC_DIRECT_PRINTING_REF_OBJECTS

/* End of BppService */

/* BPP Reference Printing Service */
#define BPPSVC_PBR      SC_REFERENCE_PRINTING

/* BPP RUI Service. Initiated by a Printer to provide a User Interface. */
#define BPPSVC_RUI      SC_REFLECTED_UI

/*---------------------------------------------------------------------------
 * BppGEState type
 */
typedef U8 BppGEState;

#define BPPGE_INACTIVE      0x00
#define BPPGE_ACTIVE        0x01
#define BPPGE_DATA_READY    0x02
#define BPPGE_ABORT         0x03
/* End of BppGEState */

/* Forward reference to BppCallbackParms defined below */
typedef struct _BppCallbackParms BppCallbackParms;

/*---------------------------------------------------------------------------
 * BppCallback type
 *
 *  BPPEvents and data are passed to the application through a callback
 *  function of this type defined by the application.
 */
typedef void (*BppCallback)(BppCallbackParms *parms);
/* End of BppCallback */

/* === Internal use only === */
typedef U8 BppChanState;
typedef U8 BppUuid[16];
typedef U8 BppAuthFlags;

/****************************************************************************
 *
 * Data Structures
 *
 ***************************************************************************/

/****************************************************************************
 *
 * External references to SDP objects registered by Basic Printing Servers.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BPP Basic Printer attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * BPP Printer and Sender SDP record.
 *
 */
#if BT_STACK == XA_ENABLED
#if BPP_NUM_PRINTERS > 0
extern const SdpAttribute BppPrnSdpAttributes[BPP_NUM_PRN_ATTRIBUTES];
extern const U8 BppPrnAddProtoDescList[21];
#endif

#if BPP_NUM_SENDERS > 0
extern const SdpAttribute BppSndSdpAttributes[BPP_NUM_SND_ATTRIBUTES];
extern const U8 BppSndAddProtoDescList[21];
#endif
#endif

/****************************************************************************
 *
 * External reference to SDP object used to query Basic Printing Servers.
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 *
 * SDP query info
 *
 * Service search attribute request for Basic Printing.
 * The service search pattern is very specific. It contains the UUIDs
 * for BPP Direct Printing, OBEX, L2CAP, and RFCOMM.
 *
 */
extern const U8 BppServiceSearchAttribReq[BPP_NUM_SEARCH_ATTRIBUTES];


/****************************************************************************
 *
 * Forward references of structures in BppCallbackParms.
 *
 ****************************************************************************/
typedef struct _BppData               BppData;
typedef struct _BppPrintJob           BppPrintJob;
typedef struct _BppObexClient         BppObexClient;
typedef struct _BppObexServer         BppObexServer;

/*---------------------------------------------------------------------------
 * BppCallbackParms structure
 *
 *  Describes a callback event and any data that relates to the event.
 */
struct _BppCallbackParms
{
    /* See documentation on each event for status type (ObStatus
     * or ObexRespCode.)
     */
    U16         status;

    BppEvent               event;      /* Type of BPP event */
    BppChannel             channel;    /* BppChannel of operation */
    BppData               *data;       /* Operation specific data */

    /* Group: BppObexClient or BppObexServer handle dependent on Role
     * (Sender or Printer) and channel. This handle is required when responding
     * to callback events. It is normally not necessary to reference the
     * elements of these handle structures.
     */
    union {
        /* BppObexClient on client channel event */
        BppObexClient   *client;

        /* BppObexServer on server channel event */
        BppObexServer   *server;
    } obex;
};
/* End of BppCallbackParms */

/*----------------------------------------------------------------------
 * BppOcx type
 *
 *  The BppOcx field indicates the Document or SOAP object context.
 *  This context is set by the application in BPP_SendDocument,
 *  BPP_SimplePush, BPP_SendRequest and BPP_SendResponse commands or
 *  BPPSERVER_REQUEST and BPPCLIENT_RESPONSE events. It is then valid
 *  on associated BPPCLIENT_DATA_REQ and BPPSERVER_DATA_REQ or
 *  BPPCLIENT_DATA_IND and BPPSERVER_DATA_IND events.
 */
typedef void *BppOcx;
/* End of BppOcx */

#if TI_CHANGES == XA_ENABLED

typedef struct _BtlObjProgressInd BtlObjProgressInd;

#endif

/*----------------------------------------------------------------------
 * BppData structure
 *
 *  Maintains Print Job or SOAP request/response elements.
 */
struct _BppData {
    /* The current BPP operation is always valid. Other
     * elements are valid depending on this operation.
     */
    BppOp           bppOp;

    /* Group: The validity of the following are operation dependent. */
    U32             jobId;      /* Current JobId (if available) */
    BppPrintJob    *job;        /* Job description (on Printing op) */
    U8             *buff;       /* Buffer pointer for object data */
    U32             len;        /* Current length of Doc or SOAP */
    U32             offset;     /* Progress length of Doc or SOAP */
    U32             totLen;     /* Total length of Doc or SOAP */
    U8              lenBuff[4]; /* Used for queing the len header */
    BppOcx          ocx;        /* BppData object context */

#if TI_CHANGES == XA_ENABLED

    BtlObjProgressInd*	progressInfo;

#endif
};
/* End of BppData */

/*----------------------------------------------------------------------
 * BppPrintJob structure
 *
 *  Describes all job characteristics.
 */
struct _BppPrintJob {
    ListEntry   node;       /* Internal use only */

    U8  name[GOEP_MAX_UNICODE_LEN]; /* Name of the job (file name) */
    U32 jobId;              /* Job ID obtained from CreateJob */
    U8  docFormat[GOEP_MAX_UNICODE_LEN];   /* Doc format as mime type */

    /* === Internal use only === */
    BppObexServer *server;  /* OBEX Server associated with this job */
};
/* End of BppPrintJob */

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * BppAuthInfo structure
 *
 *  Describes the private information exchanged during OBEX authentication.
 *  These parameters must be filled in by the client/server application prior
 *  to calling BPP_PrnSetAuthInfo and BPP_SndSetAuthInfo.
 */
typedef struct _BppAuthInfo
{
    /* OBEX Authentication Password */
    const U8           *password;
    /* OBEX Authentication Password Length */
    U16                 passwordLen;
    /* OBEX Authentication Username */
    const U8           *userId;
    /* OBEX Authentication Username Length */
    U16                 userIdLen;
    /* OBEX Authentication Realm - describes the type of username used */
    const U8           *realm;
    /* OBEX Authentication Realm length */
    U16                 realmLen;
} BppAuthInfo;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/*----------------------------------------------------------------------
 * BppObexClient structure
 *
 *  Maintains connection info and status for OBEX clients.
 */
struct _BppObexClient {
    /* This must be the first element of the structure. Its value is
     * copied to the BppCallbackParms.channel field on a callback.
     */
    BppChannel      channel;

    /* === Internal use only === */
    BOOL            init;
#if TI_CHANGES == XA_ENABLED
    BOOL            cancelCreateConn;
#endif /* TI_CHANGES == XA_ENABLED */
    BppData         response;      
    BppUuid         bppUuid;       
    BppChanState    state;         
    GoepClientApp   cApp;          
    GoepConnectReq  connect;       
    GoepObjectReq   pushPull;      
    U8              appParams[255];
    BOOL            getSoap;
    BppAuthFlags    flags;

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* OBEX Authentication password */
    U8                  password[BPP_MAX_PASSWORD_LEN];
    /* OBEX Authentication userId */
    U8                  userId[BPP_MAX_USERID_LEN];
    /* OBEX Authentication realm */
    U8                  realm[BPP_MAX_REALM_LEN];
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
};
/* End of BppObexClient */

/*----------------------------------------------------------------------
 * BppRefObjParms structure
 *
 *  Contains Application Parms for the referenced object.
 */
typedef struct _BppRefObjParms {
    U32   offset;
    U32   totLen;
} BppRefObjParms;

/* End of BppRefObjParms */

/*----------------------------------------------------------------------
 * BppObexServer structure
 *
 *  Maintains connection info and status for OBEX servers.
 */
struct _BppObexServer {
    /* This must be the first element of the structure. Its value is
     * copied to the BppCallbackParms.channel field on a callback.
     */
    BppChannel        channel;

    /* === Internal use only === */
    BOOL              init;
    BppData           request;       
    BppRefObjParms    refObj;
    BppUuid           bppUuid1;      
    BppUuid           bppUuid2;      
    BppChanState      state;         
    BppChanState      obState;
    GoepServerApp     sApp;          
    ObexConnection    targetHdr[2];  
    U8                appParams[255];
    BppObStoreHandle  object;     
#if BPP_PRINTING_STATUS == XA_ENABLED
    BppObStoreHandle  geObject;   
#endif
    BppAuthFlags      flags;

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* OBEX Authentication password */
    U8                  password[BPP_MAX_PASSWORD_LEN];
    /* OBEX Authentication userId */
    U8                  userId[BPP_MAX_USERID_LEN];
    /* OBEX Authentication realm */
    U8                  realm[BPP_MAX_REALM_LEN];
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

};
/* End of BppObexServer */

#if BPP_NUM_SENDERS > 0
/*----------------------------------------------------------------------
 * BppSndSession structure
 *
 *  Maintains Sender info and status during an application Sender
 *  session.
 */
typedef struct _BppSndSession {
    BppPrintJob    *printJob;   /* Current Print Job description */
    BppObexClient   sndJob;     /* Sender Job channel */
    U8              srchAttr[BPP_NUM_SEARCH_ATTRIBUTES];

#if BPP_PRINTING_STATUS == XA_ENABLED
    BppObexClient   sndStatus;  /* Sender Status channel */
    BppGEState      getEventState; /* Indicates active getEvent op */
#endif

    BppObexServer   sndObject;  /* Sender Object channel */
    SdpRecord       sndRecord;  /* SDP record for OBJ channel */
    /* Sender SDP attributes */
    SdpAttribute    sndAttrib[BPP_NUM_SND_ATTRIBUTES];

} BppSndSession;
/* End of BppSndSession */
#endif /* BPP_NUM_SENDERS */

#if BPP_NUM_PRINTERS > 0
/*----------------------------------------------------------------------
 * BppPrnSession structure
 *
 *  Maintains Printer info and status during an application Printer
 *  session.
 */
typedef struct _BppPrnSession {
    BppObexServer   prnJobDps;  /* Direct Printing Job channel */
    SdpRecord       prnRecord;  /* SDP record for BPP printer */
    /* Printer SDP attributes */
    SdpAttribute    prnAttrib[BPP_NUM_PRN_ATTRIBUTES];

#if BPP_PRINTING_STATUS == XA_ENABLED
    BppObexServer   prnStatus;  /* Printer Status channel */
    /* Printer Status SDP Additional Protocol Descriptor list */
    U8              stsAddProtDescList[sizeof(BppPrnAddProtoDescList)];
    BppGEState      getEventState; /* Indicates active getEvent op */
    U8              sendContinue;  /* Indicates ServerContinue state */
#endif

    BppObexClient   prnObject;  /* Printer Object channel */

    U8              srchAttr[BPP_NUM_SEARCH_ATTRIBUTES];
} BppPrnSession;
/* End of BppPrnSession */
#endif /* BPP_NUM_PRINTERS */

/****************************************************************************
 *
 * Function Reference
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Section: Functions common between Sender and Printer
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BPP_Init()
 *
 *      Initializes BPP internal structures. This function must be called
 *      before any other BPP functions.
 *
 * Returns:
 *      TRUE - Initialization successful.
 *
 *      FALSE - Initialization failed. 
 */
BOOL BPP_Init(void);

/*---------------------------------------------------------------------------
 * BPP_Deinit()
 *
 *      Deinitializes BPP internal structures and protocols. This
 *      function must be called before the application terminates.
 *
 */
void BPP_Deinit(void);

/*---------------------------------------------------------------------------
 * BPP_ClientAccept()
 *
 *      Indicates client accepts the current operation.
 *
 * Parameters:
 *      client - Pointer to BppObexClient object.
 *      soap - Pointer to BppData structure associated with operation.
 *
 */
void BPP_ClientAccept(BppObexClient *client, BppData *soap);

/*---------------------------------------------------------------------------
 * BPP_ServerAccept()
 *
 *      Indicates server accepts the current operation. The accept must
 *      include a BppData object pointer when a SOAP operation is accepted.
 *
 * Parameters:
 *      server - Pointer to BppObexServer object.
 *      accept - ObexRespCode indicating accept or abort reason.
 *      soap - Pointer to BppData structure associated with operation.
 *
 */
void BPP_ServerAccept(BppObexServer *server, ObexRespCode code, BppData *soap);

/*---------------------------------------------------------------------------
 * BPP_ServerContinue()
 *
 *      Indicates server is ready to continue the current operation.
 *      Useful for flow control.
 *
 * Parameters:
 *      server - Pointer to BppObexServer object.
 *
 */
void BPP_ServerContinue(BppObexServer *server);

/*---------------------------------------------------------------------------
 * BPP_Abort()
 *
 *      Abort operation on specified server or client.
 *
 * Parameters:
 *      session - Pointer to aborting BppSenderSession or BppPrinterSession.
 *      channel - Aborting channel.
 *
 */
void BPP_Abort(void *session, BppChannel channel);

/* Forward references */
ObStatus BppConnect(void *session, ObexTpAddr *addr, BppService bppService);
ObStatus BppDisconnect(void *session, BppService bppService);

#if BPP_NUM_SENDERS > 0
/****************************************************************************
 *
 * Section: Functions specific to Sender
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BPP_RegisterSender()
 *
 *      Registers BPP Sender and callback. This function must be called
 *      before any other BPP Sender functions.
 *
 * Parameters:
 *      snd - Pointer to BppSndSession object to register.
 *      callback - Identifies the application function that will be called
 *          with BPP Sender events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BPP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_FAILED - BPP callback failed to register.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *      BT_STATUS_NO_RESOURCES - The BPP callback was already registered. 
 */
BtStatus BPP_RegisterSender(BppSndSession *snd, BppCallback callback);

/*---------------------------------------------------------------------------
 * BPP_DeregisterSender()
 *
 *      Deregisters BPP Sender and callback.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BPP application was
 *      successfully deregistered.
 *
 *      BT_STATUS_IN_PROGRESS - BPP application is busy.
 *
 */
BtStatus BPP_DeregisterSender(BppSndSession *snd);

/*---------------------------------------------------------------------------
 * BPP_InitSenderChannel()
 *
 *      Initialize a BPP Sender channel. A channel must be initialized
 *      before use.
 *
 * Parameters:
 *      session - Pointer to BppSndSession.
 *      channel - Identifies the BppChannel for this session. Each session
 *          may have different channels available. Channels that may be
 *          initialized include BPPCH_SND_JOB and BPPCH_SND_STATUS.
 *          Initializing BPPCH_SND_JOB also automatically initializes the
 *          get referenced object channel (BPPCH_SND_OBJECT).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BPP channel was
 *      successfully initialized.
 *
 *      BT_STATUS_FAILED - Init failure.
 */
BtStatus BPP_InitSenderChannel(BppSndSession *session, BppChannel channel);

/*---------------------------------------------------------------------------
 * BPP_Connect()
 *
 *      Connects a Sender to a Printer address on the specified UUID.  
 *
 * Parameters:
 *      session - Pointer to BppSndSession or BppPrnSession object to connect.
 *      addr - Pointer to structure containing remote address.
 *      bppUuid - UUID of desired connection.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The connection completed successfully.
 *
 *      OB_STATUS_PENDING - The connection procedure was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_Connect(BppSndSession *session, ObexTpAddr *addr, BppService bppService);
#define BPP_Connect(s,a,b) BppConnect(s, a, b)

/*---------------------------------------------------------------------------
 * BPP_Disconnect()
 *
 *      Disconnects all Sender channels from a Printer.  
 *
 * Parameters:
 *      snd - Pointer to BppSndSession object to disconnect.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - Disconnect completed successfully.
 *
 *      OB_STATUS_PENDING - Disconnect procedure was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_Disconnect(BppSndSession *session);
#define BPP_Disconnect(s) BppDisconnect(s, BPPSVC_DPS)

/*---------------------------------------------------------------------------
 * BPP_SimplePush()
 *
 *      Initiates a Simple Push Transfer command from the Sender.
 *
 * Parameters:
 *      snd - Pointer to a valid BppSndSession object.
 *      data - Pointer to BppData with Print Job parameters.
 *      job - Pointer to Print Job parameters.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The Simple Push completed successfully.
 *
 *      OB_STATUS_PENDING - The Simple Push was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_SimplePush(BppSndSession *snd, BppData *data, BppPrintJob *job);

/*---------------------------------------------------------------------------
 * BPP_SendDocument()
 *
 *      Initiates a SendDocument command from the Sender.
 *
 * Parameters:
 *      snd - Pointer to BppSndSession object.
 *      data - Pointer to BppData with Print Job parameters.
 *      job - Pointer to Print Job parameters.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The SendDocument completed successfully.
 *
 *      OB_STATUS_PENDING - The SendDocument was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_SendDocument(BppSndSession *snd, BppData *data, BppPrintJob *job);

/*---------------------------------------------------------------------------
 * BPP_SendRequest()
 *
 *      Initiates a SOAP Request command from the Sender.
 *
 * Parameters:
 *      snd - Pointer to BppSndSession object.
 *      soap - Pointer to request initialized BppData structure.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The SOAP request completed successfully.
 *
 *      OB_STATUS_PENDING - The SOAP request was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
ObStatus BPP_SendRequest(BppSndSession *session, BppData *soap);

#if BPP_PRINTING_STATUS == XA_ENABLED
/*---------------------------------------------------------------------------
 * BPP_IsSndGetEventActive()
 *
 *      Detect an active GetEvent operation on the session Status channel.
 *
 * Parameters:    session - pointer to a valid BppSndSession.
 *
 * Returns:
 *      TRUE if a GetEvent operation is active, FALSE otherwise.
 */
BOOL BPP_IsSndGetEventActive(BppSndSession *session);
#define BPP_IsSndGetEventActive(a) ((a)->getEventState != BPPGE_INACTIVE)
#endif

/*---------------------------------------------------------------------------
 * BPP_SendReferencedObjectRsp()
 *
 *      Sends a response for a requested reference object.
 *
 * Parameters:
 *      server - Pointer to BppPrnSession server object.
 *      data - Reference object parameters.  The 'totLen' field must be 
 *          intialized to file size.
 *
 * Returns:   (See bpp.h)
 *
 *      OB_STATUS_SUCCESS - The request completed successfully.
 *
 *      OB_STATUS_PENDING - The request was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
ObStatus BPP_SendReferencedObjectRsp(BppObexServer *server, BppData *data);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * BPP_ClientSetAuthInfo()
 *
 *     This function is called by the client to set the proper username, 
 *     password, and challenge realm information for the upcoming BPP OBEX 
 *     connection.  This information is used when authenticating this OBEX
 *     connection.
 *     
 * Parameters:
 *     client - Pointer to BppObexClient object.
 *     info - Structure containing the authentication information.
 *
 * Returns:
 *     OB_STATUS_FAILED - Setting the authentication information failed.
 *
 *     OB_STATUS_SUCCESS - Setting the authentication information succeeded.
 */
ObStatus BPP_ClientSetAuthInfo(BppObexClient *client, BppAuthInfo *info);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#endif /* BPP_NUM_SENDERS */

#if BPP_NUM_PRINTERS > 0
/****************************************************************************
 *
 * Section: Functions specific to Printer
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BPP_RegisterPrinter()
 *
 *      Registers BPP Printer and callback. This function must be called
 *      before any other BPP Printer functions.
 *
 * Parameters:
 *      snd - Pointer to BppPrnSession object to register.
 *      callback - Identifies the application function that will be called
 *          with BPP Printer events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BPP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_FAILED - BPP callback failed to register.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *      BT_STATUS_NO_RESOURCES - The BPP callback was already registered. 
 */
BtStatus BPP_RegisterPrinter(BppPrnSession *prn, BppCallback callback);

/*---------------------------------------------------------------------------
 * BPP_DeregisterPrinter()
 *
 *      Deregisters BPP Sender and callback.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BPP application was
 *      successfully deregistered.
 *
 *      BT_STATUS_IN_PROGRESS - BPP application is busy.
 *
 */
BtStatus BPP_DeregisterPrinter(BppPrnSession *prn);

/*---------------------------------------------------------------------------
 * BPP_InitPrinterChannel()
 *
 *     Initialize a BPP Printer channel. A channel must be initialized
 *     before use.
 *
 * Parameters:
 *     session - Pointer to BppPrnSession.
 *     channel - Identifies the BppChannel for the session. Each session
 *         may have differenct channels available. Channels that may be
 *         initialized include BPPCH_PRN_JOB and BPPCH_PRN_STATUS.
 *         Initializing BPPCH_PRN_JOB also automatically initializes the get
 *         referenced object channel (BPPCH_PRN_OBJECT).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The BPP channel was successfully initialized.
 *
 *     BT_STATUS_FAILED - Init failure.
 */
BtStatus BPP_InitPrinterChannel(BppPrnSession *session, BppChannel channel);

/*---------------------------------------------------------------------------
 * BPP_SendResponse()
 *
 *      Initiates a SOAP Response command from the Printer.
 *
 * Parameters:
 *      server - Pointer to BppPrnSession server object.
 *      soap - Pointer to response initialized BppData structure.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The SOAP response completed successfully.
 *
 *      OB_STATUS_PENDING - The SOAP response was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_SendResponse(BppObexServer *server, BppData *soap);

/*---------------------------------------------------------------------------
 * BPP_ReferencedObjectConnect()
 *
 *      Connects the printer to the sender for getting referenced objects.
 *
 * Parameters:
 *      session - Pointer to BppPrnSession object to connect. The
 *          BPPCH_PRN_OBJECT channel must have been initialized previously
 *          for this session (see BPP_InitPrinterChannel).
 *
 *      addr - Pointer to structure containing remote address.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The connection completed successfully.
 *
 *      OB_STATUS_PENDING - The connection procedure was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_ReferencedObjectConnect(BppPrnSession *session, ObexTpAddr *addr);
#define BPP_ReferencedObjectConnect(s,a) BppConnect(s, a, BPPSVC_REF_OBJ)

/*---------------------------------------------------------------------------
 * BPP_ReferencedObjectDisconnect()
 *
 *      Disconnects the reference object channel.
 *
 * Parameters:
 *      snd - Pointer to BppSndSession object to disconnect.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - Disconnect completed successfully.
 *
 *      OB_STATUS_PENDING - Disconnect procedure was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_ReferencedObjectDisconnect(BppPrnSession *session);
#define BPP_ReferencedObjectDisconnect(s) BppDisconnect(s, BPPSVC_REF_OBJ)

/*---------------------------------------------------------------------------
 * BPP_GetReferencedObject()
 *
 *      Gets an object from the sender that was referenced in the current
 *      print job.  The object channel must first be connected by calling
 *      BPP_ReferencedObjectConnect().
 *
 * Parameters:
 *      session - Pointer to BppPrnSession.
 *      name - Name of the referenced object. Must be ASCII or UNICODE depending
 *          on definition of GOEP_DOES_UNICODE_CONVERSIONS. If enabled,
 *          a normal 7-bit ASCII will be expanded into UTF-16 format
 *          automatically; if disabled, a valid UNICODE sequence must be
 *          provided.
 *      data - Reference object parameters.  The fields must be intialized
 *          as follows:
 *              offset - Offset into the file
 *              len - Number of bytes to return
 *              totLen - File Size (set to 0 on requests)
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The response completed successfully.
 *
 *      OB_STATUS_PENDING - The response was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BPP_GetReferencedObject(BppPrnSession *session, const U8 *name,
                                BppData *data);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * BPP_ServerAuthenticate()
 *
 *     This function is called by the server to initiate OBEX authentication
 *     on the upcoming BPP OBEX connection.  This function must be called prior
 *     to calling BPP_ServerContinue during the BPP OBEX connection, in order 
 *     to setup the OBEX Authentication Challenge information. In addition,
 *     the BPP_ServerSetAuthInfo routine should be called prior to both of 
 *     these functions to ensure that the proper username, password, and 
 *     challenge realm information are being used.
 *     
 * Parameters:
 *     server - Pointer to BppPrnSession server object.
 *
 * Returns:
 *     OB_STATUS_FAILED - The request to authenticate failed.
 *
 *     OB_STATUS_SUCCESS - The request to authenticate was successful.  If, for
 *         some reason, authentication fails, the BPPSERVER_ABORT event will
 *         will be received. 
 */
ObStatus BPP_ServerAuthenticate(BppObexServer *Server);

/*---------------------------------------------------------------------------
 * BPP_ServerSetAuthInfo()
 *
 *     This function is called by the server to set the proper username, 
 *     password, and challenge realm information for the upcoming BPP OBEX 
 *     connection.  This information is used when authenticating this OBEX
 *     connection.
 *     
 * Parameters:
 *     server - Pointer to BppPrnSession server object.
 *     info - Structure containing the authentication information.
 *
 * Returns:
 *     OB_STATUS_FAILED - Setting the authentication information failed.
 *
 *     OB_STATUS_SUCCESS - Setting the authentication information succeeded.
 */
ObStatus BPP_ServerSetAuthInfo(BppObexServer *server, BppAuthInfo *info);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if BPP_PRINTING_STATUS == XA_ENABLED
/*---------------------------------------------------------------------------
 * BPP_IsPrnGetEventActive()
 *
 *      Detect an active GetEvent operation on the session Status channel.
 *
 * Parameters:    session - pointer to a valid BppPrnSession
 *
 * Returns:
 *      TRUE if a GetEvent operation is active, FALSE otherwise.
 */
BOOL BPP_IsPrnGetEventActive(BppPrnSession *session);
#define BPP_IsPrnGetEventActive(a) ((a)->getEventState != BPPGE_INACTIVE)
#endif

#endif /* BPP_NUM_PRINTERS */

#endif /* __BPP_H */

