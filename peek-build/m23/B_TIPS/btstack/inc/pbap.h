#ifndef __PBAP_H
#define __PBAP_H

/****************************************************************************
 *
 * File:
 *     $Workfile:pbap.h$ for iAnywhere Blue SDK, PBAP Version 1.1.2
 *     $Revision:29$
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Phonebook Access Profile program interface.
 * 
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2005-2006 Extended Systems, Inc.
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
#include "goep.h"
#include "pbapobs.h"

/*---------------------------------------------------------------------------
 * Phonebook Access Profile (PBAP) Layer 
 *
 *     The PBAP layer provides functions for building and sending Phonebook 
 *     Access compliant messages.  These messages are based on the OBEX 
 *     protocol and are written directly on top of our GOEP profile layer.
 */

/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * PBAP_NUM_CLIENTS Constant
 *
 *  Defines the number of concurrent clients supported.
 *
 *  The default value is 1. Set to 0 to disable client support.
 */
#ifndef PBAP_NUM_CLIENTS
#define PBAP_NUM_CLIENTS            1
#endif

#if GOEP_NUM_OBEX_CONS < PBAP_NUM_CLIENTS
#error GOEP_NUM_OBEX_CONS must be greater than or equal to PBAP_NUM_CLIENTS
#endif

/*---------------------------------------------------------------------------
 * PBAP_NUM_SERVERS Constant
 *
 *  Defines the number of concurrent servers supported.
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef PBAP_NUM_SERVERS
#define PBAP_NUM_SERVERS            1
#endif

#if GOEP_NUM_OBEX_CONS < PBAP_NUM_SERVERS
#error GOEP_NUM_OBEX_CONS must be greater than or equal to PBAP_NUM_SERVERS
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 *  The default value is 20.
 */
#ifndef PBAP_MAX_PASSWORD_LEN
#define PBAP_MAX_PASSWORD_LEN       20
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 *  The default value is 20.
 */
#ifndef PBAP_MAX_USERID_LEN
#define PBAP_MAX_USERID_LEN         20
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 *  The default value is 20.
 */
#ifndef PBAP_MAX_REALM_LEN
#define PBAP_MAX_REALM_LEN          20
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_APP_PARMS_LEN Constant
 *
 *  Defines the maximum length of the application parameter header information
 *  used within PBAP.
 *
 *  The default value is 30.
 */
#ifndef PBAP_MAX_APP_PARMS_LEN
#define PBAP_MAX_APP_PARMS_LEN      30
#endif

#if PBAP_NUM_CLIENTS == 0 && PBAP_NUM_SERVERS == 0
#error Either PBAP_NUM_CLIENTS or PBAP_NUM_SERVERS should be enabled
#endif

/*---------------------------------------------------------------------------
 * PBAP_LOCAL_PHONEBOOK_SUPPORTED constant
 * 
 *  Indicates whether local phonebook support is available on the PBAP server. 
 *  This value is stored in the SDP entry.
 */
#ifndef PBAP_LOCAL_PHONEBOOK_SUPPORTED
#define PBAP_LOCAL_PHONEBOOK_SUPPORTED      XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * PBAP_SIM_PHONEBOOK_SUPPORTED constant
 * 
 *  Indicates whether SIM card phonebook support is available on the PBAP 
 *  server.  This value is stored in the SDP entry.
 */
#ifndef PBAP_SIM_PHONEBOOK_SUPPORTED
#define PBAP_SIM_PHONEBOOK_SUPPORTED        XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * Checks for correct configuration values.
 */

#if PBAP_NUM_CLIENTS > 0
#if OBEX_ROLE_CLIENT == XA_DISABLED
#error "OBEX_ROLE_CLIENT must be enabled!"
#endif
#endif

#if PBAP_NUM_SERVERS > 0
#if OBEX_ROLE_SERVER == XA_DISABLED
#error "OBEX_ROLE_SERVER must be enabled!"
#endif
#endif

#if GOEP_MAX_UNICODE_LEN == 0
#error "GOEP_MAX_UNICODE_LEN must be greater than zero!"
#endif

#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
#error "GOEP_DOES_UNICODE_CONVERSIONS Must be enabled."
#endif

/* Provide enough room for the vCard listing type header - 'x-bt/vcard-listing' */
#if GOEP_MAX_TYPE_LEN < 19
#error "GOEP_MAX_TYPE_LEN must be 19 or greater!"
#endif

#if OBEX_SERVER_CONS_SIZE < 1
#error "OBEX_SERVER_CONS_SIZE must be 1 or greater!"
#endif

#if OBEX_BODYLESS_GET != XA_ENABLED
#error "OBEX_BODYLESS_GET must be enabled!"
#endif

#if OBEX_ALLOW_SERVER_TP_DISCONNECT != XA_ENABLED
#error "OBEX_ALLOW_SERVER_TP_DISCONNECT must be enabled!"
#endif

#if SDP_SERVER_SUPPORT != XA_ENABLED
#error "SDP_SERVER_SUPPORT must be enabled!"
#endif

#if SDP_CLIENT_SUPPORT != XA_ENABLED
#error "SDP_CLIENT_SUPPORT must be enabled!"
#endif

/*----------------------------------------------------------------------------
 * PULL_PHONEBOOK_TYPE constant
 *
 *     The PULL_PHONEBOOK_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting a phonebook
 *     object.
 */
#define PULL_PHONEBOOK_TYPE            "x-bt/phonebook"

 /*---------------------------------------------------------------------------
 * VCARD_LISTING_OBJECT_TYPE constant
 *
 *     The VCARD_LISTING_OBJECT_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting the vCard listing
 *     object.
 */
#define VCARD_LISTING_OBJECT_TYPE      "x-bt/vcard-listing"

/*----------------------------------------------------------------------------
 * VCARD_OBJECT_TYPE constant
 *
 *     The VCARD_OBJECT_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting a vCard
 *     object.
 */
#define VCARD_OBJECT_TYPE              "x-bt/vcard"

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_UNKNOWN_OBJECT_LENGTH constant
 *
 *     This constant is returned by PBAPOBS_GetObjectLen() when a phonebook
 *     object length is unknown. It is commonly used with objects that are 
 *     generated dynamically during the exchange. In exchanges where this 
 *     value is returned, the protocol calls the PBAPOBS_ReadFlex() read 
 *     function.
 *
 * Requires:
 *     OBEX_DYNAMIC_OBJECT_SUPPORT set to XA_ENABLED.
 */
#define PBAP_UNKNOWN_OBJECT_LENGTH      0xFFFFFFFF
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAP_FILTER_SIZE constant
 *
 *     This constant defines the size (in bytes) of the vCard filter used 
 *     in the Pull Phonebook and Pull Vcard Entry operations.
 */
#define PBAP_FILTER_SIZE                8

/****************************************************************************
 *
 * Section: Types
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * PbapEvent type
 *
 *     Indicates the current event type. All event indications and 
 *     confirmations are passed to a callback function of type 
 *     PbapClientCallback or PbapServerCallback. The "PbapCallbackParms.event" 
 *     field indicates one of the event types below.  The 
 *     "PbapCallbackParms.oper" field indicates the applicable event 
 *     operation.
 */
typedef U8 PbapEvent;

/* A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define PBAP_EVENT_TP_CONNECTED         0x01

/* The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define PBAP_EVENT_TP_DISCONNECTED      0x02

/* Indicates that a phonebook operation (see PbapOp) has 
 * completed successfully.
 */
#if TI_CHANGES == XA_ENABLED
 /* At the completion of a pull operation, (e.g. PullPhonebook) the 
  * operation name at the application layer (see PbapOp) associated 
  * with the event will always be PBAPOP_PULL - generic pull operation. 
  */
#endif /* TI_CHANGES == XA_ENABLED */
#define PBAP_EVENT_COMPLETE             0x03

/* Indicates that the current operation has failed or was rejected
 * by the remote device.
 */
#define PBAP_EVENT_ABORTED              0x04

/* Delivered to the application when it is time to issue
 * another request or response. The application must call either
 * PBAP_ServerContinue() or PBAP_ClientContinue().
 */
#define PBAP_EVENT_CONTINUE             0x05

/* Delivered to the application when the Application Parameters
 * header information has been fully parsed.
 */
#define PBAP_EVENT_PARAMS_RX            0x06

#if PBAP_NUM_SERVERS > 0
/* Delivered to server application before the last response is going
 * to be sent. It provides the server with an opportunity to complete 
 * the requested operation (such as a SetPhonebook request) and if necessary
 * set an abort response code before the final response is sent (Server Only).
 */
#define PBAP_EVENT_PRECOMPLETE          0x07

/* Indicates the start of a new operation. In the case of an Abort
 * operation, this event may occur during another pending operation 
 * (e.g. PullPhonebook).  In the case where PBAP_ServerContinue() 
 * has not been called during this operation, perhaps awaiting Server 
 * object creation or additional buffer space, it should be called now 
 * or the Abort operation will not complete (Server Only).
 */
#if TI_CHANGES == XA_ENABLED
 /* At the beginning of a pull operation, (e.g. PullPhonebook) the 
  * operation name (see PbapOp) associated with the event will always be 
  * PBAPOP_PULL - generic pull operation.
  */
#endif /* TI_CHANGES == XA_ENABLED */
#define PBAP_EVENT_START                0x08
    
/* Indicates a request for the phonebook parameters needed for the next
 * response. The application should provide these parameters through the 
 * Phonebook Access server callback parameters. This situation occurs only 
 * during Pull Phonebook and Pull VCard Listing operations.  The phonebook
 * size parameter is only needed when a maxListCount of zero is 
 * received from the client device during the PBAP_EVENT_PARAMS_RX event
 * (Server only).
 */
#define PBAP_EVENT_PROVIDE_PARAMS       0x09

/* Indicates a request for the phonebook object data length. The 
 * application should provide the length of the object being 
 * requested through the Phonebook Access server callback parameters
 * Failure to do so will cause the operation to not send an object. 
 * (Server Only)
 */
#define PBAP_EVENT_OBJECT_LENGTH_REQ    0x0a

/* Indicates a request for data. The application should provide the
 * data length requested, and provide a valid buffer pointer through the
 * Phonebook Access server callback parameters. Different callback 
 * parameters exist based on whether PBAPOBS_Read or PBAPOBS_ReadFlex 
 * have triggered this event (Server only).
 */
#define PBAP_EVENT_DATA_REQ             0x0b
#endif /* PBAP_NUM_SERVERS > 0 */

#if PBAP_NUM_CLIENTS > 0
/* Informs the client application of data that has been received. The
 * data length and data buffer pointer that are provided must be handled
 * during this callback, as the memory is only good until this callback
 * returns (Client only). 
 */
#define PBAP_EVENT_DATA_IND             0x0c
#endif /* PBAP_NUM_CLIENTS > 0 */

#if OBEX_AUTHENTICATION == XA_ENABLED
/* Informs the client/server application of the result from the OBEX
 * Authentication operation performed during the OBEX Connect. This event
 * is indicated only to the initiator of the OBEX Authentication (the device
 * that issued the OBEX Authentication Challenge header), as this device
 * is the one requiring authentication to proceed.  If a failure occurs
 * during PBAP Client authentication, the client will not be able to 
 * proceed with any operations until authentication is retried successfully.
 * If a failure occurs during PBAP Server authentication, the server will 
 * reject the OBEX Connect with the Unauthorized response code and the
 * PBAP Client will be forced to retry the connect operation again, before
 * the server will accept any operations.
 */
#define PBAP_EVENT_AUTH_RESULT          0x0d

#if  XA_ENABLED == TI_CHANGES
/* Indicates that an authentication challenge request has
 * been received and is available in the event challenge field. If
 * the application intends on responding to this challenge it should do
 * so during this event notification. This is because there is a risk
 * that the peer may send more than one challenge and the GOEP layer
 * only tracks the last challenge received. However, it is not required
 * that the application respond during the event, as long as the
 * application always responds to the last challenge received.
 */
#define PBAP_EVENT_AUTH_CHALLENGE 		0x0e

/* Indicates that the BTL_PBAPS_Disable operation was completed and the
 * PBAP context is disabled. Note that this event is diffrent from other events,
 * since it generated at the BTL layer.
 */
#define PBAP_EVENT_DISABLED				0x0f

#endif /* XA_ENABLED == TI_CHANGES */

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
/* End of PbapEvent */

/*---------------------------------------------------------------------------
 * PbapOp type
 *
 *     Indicates the operation type of the current event. Each event 
 *     indication has an associated operation passed to a callback function 
 *     of type PbapClientCallback or PbapServerCallback. The 
 *     "PbapCallbackParms.oper" field will indicate one of the operation 
 *     types below.  Since the Pull Phonebook, Pull Vcard Listing, and Pull
 *     Vcard Entry operations cannot be known until the actual OBEX headers
 *     are processed, the initial operation start event will indicate merely
 *     that a generic Pull operation is occurring, until further information
 *     arrives.  The exact operation will be known either during the 
 *     PBAP_EVENT_PARAMS_RX or PBAP_EVENT_OBJECT_LENGTH_REQ events
 */
typedef U32 PbapOp;

#define PBAPOP_NONE                0x0000 /* No current operation */
#define PBAPOP_CONNECT             0x0001 /* Connect operation */
#define PBAPOP_DISCONNECT          0x0002 /* Disconnect operation */
#define PBAPOP_SET_PHONEBOOK       0x0004 /* Set Phonebook operation */
#define PBAPOP_PULL                0x0008 /* Generic Pull operation  */
#define PBAPOP_PULL_PHONEBOOK      0x0010 /* Pull Phonebook operation */
#define PBAPOP_PULL_VCARD_LISTING  0x0020 /* Pull vCard listing operation */
#define PBAPOP_PULL_VCARD_ENTRY    0x0040 /* Pull vCard entry operation */

/* End of PbapOp */

/*---------------------------------------------------------------------------
 * PbapRespCode type
 *
 *     Response codes used to signify the status of a PBAP operation. They
 *     are included within OBEX Response packets, which are sent out to other 
 *     devices.
 *
 *     The first two response codes listed below are success responses; the 
 *     remainder are considered failures. The failure codes are frequently 
 *     associated with aborted operations.
 */
typedef U8 PbapRespCode;

/* Group: Successful response codes */

#define PBRC_CONTINUE              0x10 /* Continue */
#define PBRC_STATUS_SUCCESS        0x20 /* Success */

/* Group: Failure response codes */

#define PBRC_BAD_REQUEST           0x40 /* Bad Request */
#define PBRC_UNAUTHORIZED          0x41 /* Unauthorized */
#define PBRC_FORBIDDEN             0x43 /* Forbidden - operation is understood */
#define PBRC_NOT_FOUND             0x44 /* Not Found */
#define PBRC_NOT_ACCEPTABLE        0x46 /* Not Acceptable */
#define PBRC_PRECONDITION_FAILED   0x4c /* Precondition Failed */
#define PBRC_NOT_IMPLEMENTED       0x51 /* Not Implemented */
#define PBRC_SERVICE_UNAVAILABLE   0x53 /* Service Unavailable */
#define PBRC_LINK_DISCONNECT       0x80 /* Transport connection has been disconnected. */

/* End of PbapRespCode */

/*---------------------------------------------------------------------------
 * PbapVcardFilterBit type
 * 
 *     Describes the bit location pertaining to each filter value in 
 *     the 64-bit vCard filter.
 */
typedef U8 PbapVcardFilterBit;

#define VCARD_FILTER_VER       0    /* Version (Bit 0) */
#define VCARD_FILTER_FN        1    /* Formatted Name (Bit 1) */
#define VCARD_FILTER_N         2    /* Structured Presentation of Name (Bit 2) */
#define VCARD_FILTER_PHOTO     3    /* Associated Image or Photo (Bit 3) */
#define VCARD_FILTER_BDAY      4    /* Birthday (Bit 4) */
#define VCARD_FILTER_ADR       5    /* Delivery Address (Bit 5) */
#define VCARD_FILTER_LABEL     6    /* Delivery (Bit 6) */
#define VCARD_FILTER_TEL       7    /* Telephone (Bit 7) */
#define VCARD_FILTER_EMAIL     8    /* Electronic Mail Address (Bit 8) */
#define VCARD_FILTER_MAILER    9    /* Electronic Mail (Bit 9) */
#define VCARD_FILTER_TZ        10   /* Time Zone (Bit 10) */
#define VCARD_FILTER_GEO       11   /* Geographic Position (Bit 11) */
#define VCARD_FILTER_TITLE     12   /* Job (Bit 12) */
#define VCARD_FILTER_ROLE      13   /* Role within the Organization (Bit 13) */
#define VCARD_FILTER_LOGO      14   /* Organization Logo (Bit 14) */
#define VCARD_FILTER_AGENT     15   /* vCard of Person Representing (Bit 15) */
#define VCARD_FILTER_ORG       16   /* Name of Organization (Bit 16) */
#define VCARD_FILTER_NOTE      17   /* Comments (Bit 17) */
#define VCARD_FILTER_REV       18   /* Revision (Bit 18) */
#define VCARD_FILTER_SOUND     19   /* Pronunciation of Name (Bit 19) */
#define VCARD_FILTER_URL       20   /* Uniform Resource Locator (Bit 20) */
#define VCARD_FILTER_UID       21   /* Unique ID (Bit 21) */
#define VCARD_FILTER_KEY       22   /* Public Encryption Key (Bit 22) */
#define VCARD_FILTER_NICK      23   /* Nickname (Bit 23) */
#define VCARD_FILTER_CAT       24   /* Categories (Bit 24) */
#define VCARD_FILTER_PRODID    25   /* Product Id (Bit 25) */
#define VCARD_FILTER_CLASS     26   /* Class Information (Bit 26) */
#define VCARD_FILTER_SORT_STR  27   /* Sort string (Bit 27) */
#define VCARD_FILTER_TIMESTAMP 28   /* Time stamp (Bit 28) */
/* Bits 29-38 Reserved for future use */
#define VCARD_FILTER_PROP      39   /* Use of a proprietary filter (Bit 39) */ 
/* Bits 40-63 Reserved for proprietary filter usage */

/* End of PbapVcardFilterBit */

/*---------------------------------------------------------------------------
 * PbapVcardFormat type
 * 
 *     Describes the 1-byte vCard format value sent in the Application 
 *     Parameters OBEX header from the Phonebook Access client to the 
 *     server to dictate which vCard format it wishes to be returned for 
 *     each vCard object. This format is used for both the Pull Phonebook 
 *     and Pull Phonebook Entry operations.
 */
typedef U8 PbapVcardFormat;

#define VCARD_FORMAT_21        0x00       /* Version 2.1 format */
#define VCARD_FORMAT_30        0x01       /* Version 3.0 format */

/* End of PbapVcardFormat */

/*---------------------------------------------------------------------------
 * PbapVcardSortOrder type
 * 
 *     Describes the 1-byte vCard sorting order value sent in the Application 
 *     Parameters OBEX header from the Phonebook Access client to the 
 *     server to dictate the ordering of the vCard entries returned in the 
 *     vCard listing. This format is used for the Pull Vcard Listing operation.
 */
typedef U8 PbapVcardSortOrder;

#define VCARD_SORT_ORDER_INDEXED        0x00       /* Indexed sorting */
#define VCARD_SORT_ORDER_ALPHA          0x01       /* Alphabetical sorting */
#define VCARD_SORT_ORDER_PHONETICAL     0x02       /* Phonetical sorting */

/* End of PbapVcardSortOrder */

/*---------------------------------------------------------------------------
 * PbapVcardSearchAttribute type
 *
 *      Describes the 1-byte vCard search attribute value sent in the
 *      Application Parameters OBEX header from the Phonebook Access client
 *      to the server to dictate the type of search to be performed on
 *      the vCard entries on the Phonebook Access server.  This format is
 *      used for the Pull Vcard Listing operation.
 */
typedef U8 PbapVcardSearchAttribute;

#define VCARD_SEARCH_ATTRIB_NAME        0x00        /* Search by Name */
#define VCARD_SEARCH_ATTRIB_NUMBER      0x01        /* Search by Number */
#define VCARD_SEARCH_ATTRIB_SOUND       0x02        /* Search by Sound */

/* End of PbapVcardSearchAttribute */

/*---------------------------------------------------------------------------
 * PbapAppParmsTag type
 *
 *     Describes the tag values used in the Application Parameters OBEX header
 *     that are used on both the Phonebook Access client and server side.
 */
typedef U8 PbapAppParmsTag;

#define PBAP_TAG_ORDER              0x01  /* 1-byte, 0x00 (indexed), 0x01 (alpha), or 0x02 (phonetic) */
#define PBAP_TAG_SEARCH_VALUE       0x02  /* Variable length text string */
#define PBAP_TAG_SEARCH_ATTRIB      0x03  /* 1-byte, 0x00 (Name), 0x01 (Number), or 0x02 (Sound) */
#define PBAP_TAG_MAX_LIST_COUNT     0x04  /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_LIST_OFFSET        0x05  /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_FILTER             0x06  /* 8-bytes, 64 bit mask */
#define PBAP_TAG_FORMAT             0x07  /* 1-byte, 0x00 = 2.1, 0x01 = 3.0 */
#define PBAP_TAG_PHONEBOOK_SIZE     0x08  /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_MISSED_CALLS       0x09  /* 1-byte, 0x00 to 0xFF */

/* End of PbapAppParmsTag */

/*---------------------------------------------------------------------------
 * PbapSetPbFlags type
 *
 *     Flags used in the SetFolder operation.  PBAP specification requires
 *     that the PBAP_SETPB_DONT_CREATE flag is always set.
 */
typedef U8 PbapSetPbFlags;

#define PBAP_SETPB_NONE           0x00    /* No flags */
#define PBAP_SETPB_BACKUP         0x01    /* Back up one level. */
#define PBAP_SETPB_DONT_CREATE    0x02    /* Don't create folder if it doesn't exist. */

/* End of PbapSetPbFlags */

/*---------------------------------------------------------------------------
 * PbapSetPbParamFlags type
 *
 *     Flags used in the PbapSetPbParams structure.
 */
typedef U8 PbapSetPbParamFlags;

#define PBAP_PARAM_NONE             0x00    /* No flags */
#define PBAP_PARAM_PB_SIZE          0x01    /* Phonebook Size. */
#define PBAP_PARAM_NEW_MISSED_CALLS 0x02    /* New Missed Calls. */

/* End of PbapSetPbParamFlags */

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapSupportedPhonebooks type
 *
 *     Describes the PBAP supported phonebooks as determined through the 
 *     SDP query during the transport connection.
 */
typedef U8 PbapSupportedPhonebooks;

#define PBAP_PHONEBOOK_LOCAL        0x01  /* Local phonebook supported (bit 0) */
#define PBAP_PHONEBOOK_SIM          0x02  /* SIM card phonebook supported (bit 1) */
                                          /* bits 2-7 are reserved */
/* End of PbapSupportedPhonebooks */
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapDataReqType type
 *
 *     Describes the type of data request associated with the 
 *     PBAP_EVENT_DATA_REQ event. This type will dicate which callback 
 *     parameters are valid during a data request as PBAPOBS_Read has
 *     different parameters from PBAPOBS_ReadFlex.
 */
typedef U8 PbapDataReqType;

/* Data Request is using the normal PBAPOBS_Read routine */
#define PBAP_DATAREQ_READ      0x01
/* Data Request is using the flexible PBAPOBS_ReadFlex routine */
#define PBAP_DATAREQ_READ_FLEX 0x02
/* End of PbapDataReqType */
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapAuthFailReason type
 *
 *  Determines the reason for the PBAP Authentication failure associated
 *  with the PBAP_EVENT_AUTH_RESULT event.
 */
typedef U8 PbapAuthFailReason;

#define PBAP_AUTH_NO_FAILURE            0
#define PBAP_AUTH_NO_RCVD_AUTH_RESP     1
#define PBAP_AUTH_INVALID_CREDENTIALS   2
/* End of PbapAuthFailReason */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if PBAP_NUM_CLIENTS > 0

/* Forward reference to PbapClientCallbackParms defined below */
typedef struct _PbapClientCallbackParms PbapClientCallbackParms;

/*---------------------------------------------------------------------------
 * PbapClientCallback type
 *
 *  Phonebook Access Client events along with data indications are 
 *  passed to the application through a callback function of this type 
 *  defined by the application.
 */
typedef void (*PbapClientCallback)(PbapClientCallbackParms *parms);

/* End of PbapClientCallback */
#endif /* PBAP_NUM_CLIENTS > 0 */

#if PBAP_NUM_SERVERS > 0

/* Forward reference to PbapServerCallbackParms defined below */
typedef struct _PbapServerCallbackParms PbapServerCallbackParms;

/*---------------------------------------------------------------------------
 * PbapServerCallback type
 *
 *  Phonebook Access Server events along with data requests are 
 *  passed to the application through a callback function of this type 
 *  defined by the application.
 */
typedef void (*PbapServerCallback)(PbapServerCallbackParms *parms);

/* End of PbapServerCallback */
#endif /* PBAP_NUM_SERVERS > 0 */

typedef U8 PbapAuthFlags;   /* Used Internally */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

#if PBAP_NUM_CLIENTS > 0
/*----------------------------------------------------------------------
 * PbapClientSession structure
 *
 *  Maintains Phonebook Access client info and status during an 
 *  application session.
 */
 typedef struct _PbapClientSession 
 {
#if BT_SECURITY == XA_ENABLED
    /* Bluetooth security level - set by the application 
     * prior to PBAP_RegisterClient.  An application must have a
     * pairing handler registered in order to receive PIN requests.
     * This can be done through the BlueMgr API or done directly
     * by the application.
     */
    BtSecurityLevel     secLevel;
#endif /* BT_SECURITY == XA_ENABLED */

    /* GOEP Client instance */
    GoepClientApp       cApp;       

    /* === Internal use only === */
    PbapOp              currOp;
    PbapAuthFlags       flags;
    U8                  appParms[PBAP_MAX_APP_PARMS_LEN];
    SdpRecord           record;
    SdpAttribute        attributes[5];
    
    /* Registration status for this PBAP Client instance */
    BOOL                registered;
    /* GOEP Connect information */
    GoepConnectReq      connect;
    /* GOEP Pull information */
    GoepObjectReq       pull;

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* OBEX Authentication password */
    U8                  password[PBAP_MAX_PASSWORD_LEN];
    /* OBEX Authentication userId */
    U8                  userId[PBAP_MAX_USERID_LEN];
    /* OBEX Authentication realm */
    U8                  realm[PBAP_MAX_REALM_LEN];
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

} PbapClientSession;
#endif /* PBAP_NUM_CLIENTS > 0 */

#if PBAP_NUM_SERVERS > 0
/*----------------------------------------------------------------------
 * PbapServerSession structure
 *
 *  Maintains Phonebook Access server info and status during an 
 *  application session.
 */
typedef struct _PbapServerSession 
{
#if BT_SECURITY == XA_ENABLED
    /* Bluetooth security level - set by the application 
     * prior to PBAP_RegisterServer.  An application must have a
     * pairing handler registered in order to receive PIN requests.
     * This can be done through the BlueMgr API or done directly
     * by the application.
     */
    BtSecurityLevel     secLevel;
#endif /* BT_SECURITY == XA_ENABLED */

    /* GOEP Server instance */
    GoepServerApp       sApp;

    /* === Internal use only === */   
    PbapOp              currOp;
    BOOL                rcvdAppParms;
    PbapAuthFlags       flags;
    U8                  appParms[PBAP_MAX_APP_PARMS_LEN];
    SdpRecord           record;
    SdpAttribute        attributes[7];

    /* PBAP object store handle */
    PbapObStoreHandle   object;
    /* Registration status for this PBAP Server instance */
    BOOL                registered;
    /* GOEP Connect information */
    ObexConnection      obexConn;       

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* OBEX Authentication password */
    U8                  password[PBAP_MAX_PASSWORD_LEN];
    /* OBEX Authentication userId */
    U8                  userId[PBAP_MAX_USERID_LEN];
    /* OBEX Authentication realm */
    U8                  realm[PBAP_MAX_REALM_LEN];
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

    U16                 maxListCount;
} PbapServerSession;
#endif /* PBAP_NUM_SERVERS > 0 */

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapAuthInfo structure
 *
 *  Describes the private information exchanged during OBEX authentication.
 *  These parameters must be filled in by the client/server application prior
 *  to calling PBAP_ClientSetAuthInfo or PBAP_ServerSetAuthInfo.
 */
typedef struct _PbapAuthInfo
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
} PbapAuthInfo;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PbapVcardFilter structure
 * 
 *  Describes the 64-bit filter value sent in the Application Parameters
 *  OBEX header from the Phonebook Access client to the server to dictate
 *  which fields it wishes to be returned for each vCard object.  This
 *  filter is used for both the Pull Phonebook and Pull Phonebook Entry 
 *  operations.
 */
typedef struct _PbapVcardFilter 
{
    /* Array of 8 bytes for this 64-bit filter value */
    U8                  byte[PBAP_FILTER_SIZE];
} PbapVcardFilter;

#if PBAP_NUM_CLIENTS > 0
/*---------------------------------------------------------------------------
 * PbapPullPbParms structure
 *
 *  Describes the parameters required to issue a Pull Phonebook operation.
 *  These parameters must be filled in by the client application prior to 
 *  calling PBAP_PullPhonebook.
 */
typedef struct _PbapPullPbParms
{
    /* Full path information including the phonebook name (null-terminated) 
     * (e.g. "telecom\pb.vcf") 
     */
    const char         *pbName;
    /* List of the vCard fields the client wishes the server to return for
     * each phonebook entry.
     */
    PbapVcardFilter     filter;           
    /* Format of vCard (version 2.1 or version 3.0) */
    PbapVcardFormat     format;
    /* Maximum number of vCard entries supported by the client */
    U16                 maxListCount;
    /* Offset of the first vCard entry */
    U16                 listStartOffset; 
} PbapPullPbParms;

/*---------------------------------------------------------------------------
 * PbapPullVcardListingParms structure
 *
 *  Describes the parameters required to issue a Pull VCard Listing operation.
 *  These parameters must be filled in by the client application prior to 
 *  calling PBAP_PullVcardListing.
 */
typedef struct _PbapPullVcardListingParms
{
    /* Subfolder name (null-terminated) - empty string is used to return the
     * current folder listing.
     */
    const char                  *folderName;
    /* Phonebook entries are returned based on the search attribute which
     * can be either Name (0x00), Number (0x01), or Sound (0x02). The name
     * attribute is assumed if no value is provided.
     */
    PbapVcardSearchAttribute    searchAttribute;
    /* Text string that is used to compare against the specified search 
     * attribute for each remote phonebook entry to determine if the entry 
     * will be returned. If this value is not provided, all entries will be 
     * returned.
     */
    const char                  *searchValue;
    /* Indicates the requested sorting method. If no value is provided, 
     * indexed sorting is assumed. Valid sorting methods are 
     * Indexed/Alphabetical/Phonetical 
     */
    PbapVcardSortOrder          order;
    /* Maximum number of vCard entries supported by the client */
    U16                         maxListCount;
    /* Offset of the first vCard entry */
    U16                         listStartOffset;
} PbapPullVcardListingParms;

/*---------------------------------------------------------------------------
 * PbapPullVcardEntryParms structure
 *
 *  Describes the parameters required to issue a Pull VCard Entry operation.
 *  These parameters must be filled in by the client application prior to 
 *  calling PBAP_PullVcardEntry.
 */
typedef struct _PbapPullVcardEntryParms
{
    /* Object name (null-terminated) (e.g. "1.vcf") */
    const char         *objectName;
    /* List of the vCard fields the client wishes the server to return for
     * each phonebook entry.
     */
    PbapVcardFilter     filter;           
    /* Format of vCard (version 2.1 or version 3.0) */
    PbapVcardFormat     format;
} PbapPullVcardEntryParms;

/*---------------------------------------------------------------------------
 * PbapSetPhonebookParms structure
 *
 *  Describes the parameters required to issue a Set Phonebook operation.
 *  These parameters must be filled in by the client application prior to 
 *  calling PBAP_SetPhonebook.
 */
typedef struct _PbapSetPhonebookParms
{
    /* Name of the folder (null-terminated) */
    const char         *folderName;

    /* Set Phonebook operation options. PBAP specification requires
     * that the PBAP_SETPB_DONT_CREATE flag is always set. 
     */
    PbapSetPbFlags      flags;

    /* Set this flag to true to reset the path to the root folder. 
     * No path flags may be specified if this is TRUE.  In addition,
     * any name provided will be ignored if this is TRUE, since reset 
     * uses an empty name header.
     */
    BOOL                reset;
} PbapSetPhonebookParms;

/*---------------------------------------------------------------------------
 * PbapClientCallbackParms structure
 *
 *  Describes a callback event and any data that relates to the event. These
 *  callback parameters are used for all Phonebook Access Clients.  
 *  Determination of the valid fields can be done by evaluating which event 
 *  is being indicated as well as which client it is intended for.
 */
struct _PbapClientCallbackParms
{
    PbapEvent               event;          /* PBAP event */
    PbapOp                  oper;           /* PBAP operation */
    PbapClientSession      *client;         /* PBAP client */

    union {
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
        /* Group: During a PBAP_EVENT_TP_CONNECTED event, contains 
         * the SDP parsed information 
         */
        struct {
            U16                     profileVersion; /* PBAP profile version */
            PbapSupportedPhonebooks suppPhonebooks; /* PBAP supported phonebooks */
        } connect;
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

        /* Group: Valid during PBAP_EVENT_TP_DISCONNECTED event */
        PbapRespCode        discReason;     /* PBAP disconnect reason code */

        /* Group: Valid during PBAP_EVENT_ABORTED event */
        PbapRespCode        abortReason;    /* PBAP abort reason code */

#if OBEX_AUTHENTICATION == XA_ENABLED
        /* Group: Valid during PBAP_EVENT_AUTH_RESULT event */
        struct {
            BOOL                    result;         /* Result of the PBAP Authentication attempt */
            PbapAuthFailReason      reason;         /* PBAP Authentication failure reason */
        } auth;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

        /* Group: Valid during PBAP_EVENT_PARAMS_RX event - provides Application
         * Parameter header information.  Valid for Pull Phonebook and
         * Pull Vcard Listing operations only.
         */
        struct {
            /* Number of new missed calls */
            U8             newMissedCalls;
            /* Provides the size of the requested phonebook. The client 
             * should set its MaxListCount based on the phonebook size, 
             * if it is nonzero. 
             */
            U16            phonebookSize;
        } paramsRx;

        /* Group: Valid during PBAP_EVENT_DATA_IND event */
        struct {
            /* Object name (null-terminated, ASCII) */
            U8             *name;           /* Name pointer */
            /* Data Indication */
            U8             *buffer;         /* Data pointer */
            U16             len;            /* Length of data */
        } dataInd;
    } u;
};
/* End of PbapClientCallbackParms */
#endif /* PBAP_NUM_CLIENTS > 0 */

#if PBAP_NUM_SERVERS > 0
/*---------------------------------------------------------------------------
 * PbapPullInd structure
 *
 *     Provides information to a PBAP Server about a received 
 *     PULL indication.
 */
typedef struct _PbapPullInd {
    /* OBEX final bit */
    BOOL            finalBit;
    
    /* Name of the folder, null-terminated, in ASCII format. */
    const char     *name;

    /* Type of the object, null-terminated, in ASCII format. */
    const char     *type;
} PbapPullInd;

/*---------------------------------------------------------------------------
 * PbapSetPbInd structure
 * 
 *     This structure is used by the Server during SetFolder indications.
 */
typedef struct _PbapSetPbInd {
    /* The client requested a reset to the root folder. */
    BOOL                reset;

    /* Set Phonebook operation options. */
    PbapSetPbFlags      flags;

    /* Name of the folder, null-terminated. */
    const char         *name;
} PbapSetPbInd;

/*---------------------------------------------------------------------------
 * PbapSetPbParams structure
 * 
 *     This structure is used by the Server to setup the phonebook parameters
 *     required in the Pull Phonebook and Pull Vcard Listing operations. This
 *     structure is only needed when the phonebook parameters cannot be
 *     provided during the context of the PBAP_EVENT_PROVIDE_PARAMS event, 
 *     such as in cases where asynchronous access to the phonebook storage is
 *     required.
 */
typedef struct _PbapSetPbParams {
    /* Phonebook parameters that have been provided. */
    PbapSetPbParamFlags      flags;

    /* Number of new missed calls.  The server should provide this value
     * if the phonebook type is for the missed calls history (mch).
     */
    U8                       newMissedCalls;
    
    /* Size of the requested phonebook. The server should provide 
     * this value if the received client maxListCount was zero 
     */
    U16                      phonebookSize;
} PbapSetPbParams;

/*---------------------------------------------------------------------------
 * PbapServerCallbackParms structure
 *
 *  Describes a callback event and any data that relates to the event. These
 *  callback parameters are used for all Phonebook Access Servers.  
 *  Determination of the valid fields can be done by evaluating which event 
 *  is being indicated as well as which server it is intended for.
 */
struct _PbapServerCallbackParms
{
    PbapEvent               event;          /* PBAP event */
    PbapOp                  oper;           /* PBAP operation */
    PbapServerSession      *server;         /* PBAP server */

    union {
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
        /* Group: Valid during PBAP_EVENT_TP_CONNECTED event, contains 
         * the SDP parsed information 
         */
        struct {
            U16                     profileVersion; /* PBAP profile version */
        } connect;
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

        /* Group: Valid during PBAP_EVENT_TP_DISCONNECTED event */
        PbapRespCode        discReason;     /* PBAP disconnect reason code */

#if OBEX_AUTHENTICATION == XA_ENABLED
        /* Group: Valid during PBAP_EVENT_AUTH_RESULT event */
        struct {
            BOOL                    result;         /* Result of the PBAP Authentication attempt */
            PbapAuthFailReason      reason;         /* PBAP Authentication failure reason */
        } auth;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

        /* Group: Valid during PBAP_EVENT_PROVIDE_PARAMS event - requests 
         * the application to provide Application Parameter header information 
         * for the next response packet. Valid for Pull Phonebook and Pull 
         * Vcard Listing operations only.
         */
        struct {
            /* Number of new missed calls - to be set by the application */
            U8             newMissedCalls;
            /* Size of the requested phonebook. The server should provide 
             * this value if the received client maxListCount was zero 
             */
            U16             phonebookSize;
#if TI_CHANGES == XA_ENABLED 
            /* Indicates that the phonebook parameters above are not
             * included during the context of this event, but will instead 
             * be provided asynchronously through a call to the 
             * PBAP_ServerSetPbParams function. 
             */
            BOOL            asynchronous;
#endif /* TI_CHANGES == XA_ENABLED */
        } provideParams;

        /* Group: Valid during PBAP_EVENT_PARAMS_RX event - provides Application
         * Parameter header information.
         */
        struct {
            U8                 *name; /* Object name, null terminated ASCII */

            /* Group: Valid during Pull Phonebook and Pull VCard Entry operations */
            PbapVcardFilter     filter;             /* Filter of the requested vCard fields */
            PbapVcardFormat     format;             /* Format of vCard (version 2.1 or version 3.0) */

            /* Group: Valid during Pull Phonebook and Pull VCard Listing operations */
            U16                 maxListCount;       /* Maximum of vCard entries supported */
            U16                 listStartOffset;    /* Offset of the first vCard entry */

            /* Group: Valid during Pull VCard Listing operation */
            PbapVcardSortOrder          order;          /* Sorting method: indexed/alphabetical/phonetical */
            PbapVcardSearchAttribute    searchAttrib;   /* Search attribute: Name (0x00) Number (0x01), or Sound (0x02) */
            U8                         *searchValue;    /* Text value to search the attribute for */
            U8                          searchValueLen; /* Length of search value */
        } paramsRx;

        /* Group: Valid during PBAP_EVENT_OBJECT_LENGTH_REQ event */
        struct {
            /* Object name (null-terminated, ASCII) */
            U8             *name;
            /* Object Length - to be set by the application */
            U32             objectLen;
        } objectReq;

        /* Group: Valid during PBAP_EVENT_DATA_REQ event */
        struct {
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            /* Type of data request */
            PbapDataReqType type;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

            /* Object name (null-terminated, ASCII) */
            U8             *name;

            /* Data buffer pointer - to be assigned by the application */
            U8             *buffer;     

            /* Group: Valid for PBAP_DATAREQ_READ type only */
            struct {
                /* Length of the requested data */
                U16         len;
            } req;

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            /* Group: Valid for PBAP_DATAREQ_READ_FLEX only. */
            struct {
                /* Length available - the length used is set by application */
                U16        *len;
                /* Dictates whether more data exists to be sent - This value 
                 * is set by the application.
                 */
                BOOL       *more;
            } flex;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
        } dataReq;

        /* Group: Valid during PBAP_EVENT_PRECOMPLETE and PBAP_EVENT_COMPLETE events */
        union {
            PbapPullInd     pull;           /* Valid during Pull operations */
            PbapSetPbInd    setPb;          /* Valid during Set Phonebook operation */
        } info;
		
#if XA_ENABLED == TI_CHANGES

	/* Use during PBAPS_EVENT_AUTH_CHALLENGE */
#if OBEX_AUTHENTICATION == XA_ENABLED
	    ObexAuthChallengeInfo   challenge;  	
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#endif /* XA_ENABLED == TI_CHANGES */
    } u;
};
/* End of PbapServerCallbackParms */
#endif /* PBAP_NUM_SERVERS > 0 */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PBAP_Init()
 *
 *      Initialize the PBAP component.  This must be the first PBAP 
 *      function called by the application layer, or if multiple 
 *      PBAP applications exist, this function should be called
 *      at system startup (see XA_LOAD_LIST in config.h).  GOEP and 
 *      OBEX must also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL PBAP_Init(void);

#if PBAP_NUM_SERVERS > 0
/*---------------------------------------------------------------------------
 * PBAP_RegisterServer()
 *
 *     Registers the Phonebook Access Server with the GOEP multiplexor. This 
 *     includes registering the SDP records for the Phonebook Access Service.
 *     All of the events specified in the GOEP layer are delivered to the
 *     PBAP server.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Callback - callback function for the Phonebook Access server.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The PBAP Server is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the PBAP Server.
 *     
 *     OB_STATUS_BUSY - A PBAP server is already registered.
 *     
 */
ObStatus PBAP_RegisterServer(PbapServerSession *Server, 
                             PbapServerCallback Callback);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_DeregisterServer()
 *
 *     Deregisters the Phonebook Access Server from the GOEP multiplexor.
 *     This includes removing the SDP records for the Phonebook Access Service.
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
 *     OB_STATUS_FAILED - The PBAP server failed to deinitialize.
 */
ObStatus PBAP_DeregisterServer(PbapServerSession *Server);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAP_ServerContinue()
 *
 *     This function is called by the server in response to a received
 *     PBAP_EVENT_CONTINUE event. It must be called once for every
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
ObStatus PBAP_ServerContinue(PbapServerSession *Server);

/*---------------------------------------------------------------------------
 * PBAP_ServerAbort()
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
ObStatus PBAP_ServerAbort(PbapServerSession *Server, PbapRespCode Resp);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_ServerAuthenticate()
 *
 *     This function is called by the server to initiate OBEX authentication
 *     on the upcoming Phonebook Access OBEX connection.  This function must
 *     be called prior to calling PBAP_ServerContinue during the Phonebook 
 *     Access OBEX connection, in order to setup the OBEX Authentication 
 *     Challenge information. In addition, the PBAP_ServerSetAuthInfo routine 
 *     should be called prior to both of these functions to ensure that the 
 *     proper username, password, and challenge realm information are being 
 *     used.
 *     
 *     The authentication information is cleared when an OBEX Disconnect or
 *     transport disconnect occurs, so this should take place each time the
 *     connection is coming up.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The request to authenticate failed.
 *
 *     OB_STATUS_SUCCESS - The request to authenticate was successful.
 */
ObStatus PBAP_ServerAuthenticate(PbapServerSession *Server);

/*---------------------------------------------------------------------------
 * PBAP_ServerSetAuthInfo()
 *
 *     This function is called by the server to set the proper username, 
 *     password, and challenge realm information for the upcoming Phonebook
 *     Access OBEX connection.  This information is used when authenticating 
 *     this OBEX connection.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Info - Structure containing the authentication information.
 *
 * Returns:
 *     OB_STATUS_FAILED - Setting the authentication information failed.
 *
 *     OB_STATUS_SUCCESS - Setting the authentication information succeeded.
 */
ObStatus PBAP_ServerSetAuthInfo(PbapServerSession *Server, PbapAuthInfo *Info);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_ServerTpDisconnect()
 *
 *     This function is used by the server to issue a transport disconnection.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the PBAP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the server is not connected.
 */
ObStatus PBAP_ServerTpDisconnect(PbapServerSession *Server);
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAP_ServerSetPbParams()
 *
 *     This function is used by the Server to setup the phonebook parameters
 *     required in the Pull Phonebook and Pull Vcard Listing operations.  This
 *     function is called prior to PBAP_ServerContinue to ensure the phonebook 
 *     parameters are setup and built as an Application Parameters OBEX header 
 *     for the PBAP response.
 *
 *     This function is only needed when the phonebook parameters cannot be
 *     provided during the context of the PBAP_EVENT_PROVIDE_PARAMS event, 
 *     such as in cases where asynchronous access to the phonebook storage is
 *     required.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Params - The server phonebook parameters.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid parameter.
 *
 *     OB_STATUS_SUCCESS - The phonebook parameters have been set.
 *
 *     OB_STATUS_FAILED - Problem setting the phonebook parameters.
 */
ObStatus PBAP_ServerSetPbParams(PbapServerSession *Server, PbapSetPbParams *Params);

#endif /* PBAP_NUM_SERVERS > 0 */

#if PBAP_NUM_CLIENTS > 0
/*---------------------------------------------------------------------------
 * PBAP_RegisterClient()
 *
 *     Registers the Phonebook Access Client with the GOEP multiplexor.
 *     All of the events specified in the PBAP layer are delivered to the
 *     PBAP client.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Callback - callback function for the Phonebook Access client.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The PBAP Client is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the PBAP Client.
 *
 *     OB_STATUS_BUSY - A PBAP client is already registered.
 *     
 */
ObStatus PBAP_RegisterClient(PbapClientSession *Client, 
                             PbapClientCallback Callback);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_DeregisterClient()
 *
 *     Deregisters the Phonebook Access Client from the GOEP multiplexor.
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
 *     OB_STATUS_FAILED - The PBAP client failed to deinitialize.
 *
 */
ObStatus PBAP_DeregisterClient(PbapClientSession *Client);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAP_ClientConnect()
 *
 *     This function is used by the client to create a transport connection
 *     to the specified device and issue an OBEX Connect Request. If the 
 *     transport connection was issued by the server, this function will only
 *     issue the OBEX Connect Request.
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
 *         If the connection is successful, a PBAP_EVENT_COMPLETE event for
 *         the PBAPOP_CONNECT operation will be signaled.
 *
 *         If the transport connection is successful, but the OBEX Connect
 *         failed, the completion event will be PBAP_EVENT_TP_DISCONNECTED
 *         for the operation PBAP_OPER_CONNECT. At this point the transport
 *         is DISCONNECTED. 
 * 
 *         If the transport connection is unsuccessful, the completion event
 *         will be PBAP_EVENT_TP_DISCONNECTED.
 *
 *     OB_STATUS_SUCCESS - The client is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the client
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The client is currently executing an operation.
 *
 */
ObStatus PBAP_ClientConnect(PbapClientSession *Client, ObexTpAddr *Target);

/*---------------------------------------------------------------------------
 * PBAP_ClientDisconnect()
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
 *         Completion will be signaled with the PBAP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 */
ObStatus PBAP_ClientDisconnect(PbapClientSession *Client);

/*---------------------------------------------------------------------------
 * PBAP_ClientTpDisconnect()
 *
 *     This function is used by the client to initiate a transport 
 *     disconnection. PBAP_ClientDisconnect will cause the transport to be 
 *     brought down automatically, but if the transport must be brought 
 *     down immediately, perhaps due an error condition, this routine may 
 *     be used. 
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the PBAP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 */
ObStatus PBAP_ClientTpDisconnect(PbapClientSession *Client);

/*---------------------------------------------------------------------------
 * PBAP_ClientAbort()
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
ObStatus PBAP_ClientAbort(PbapClientSession *Client);

/*---------------------------------------------------------------------------
 * PBAP_PullPhonebook()
 *
 *     Initiates the OBEX "Get" operation to retrieve a phonebook object from
 *     the remote Phonebook Access Server.  
 *
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Parms - Parameters to specify the application specific parameters
 *             including the phonebook name.
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
ObStatus PBAP_PullPhonebook(PbapClientSession *Client, PbapPullPbParms *Parms);

/*---------------------------------------------------------------------------
 * PBAP_PullVcardListing()
 *
 *     Initiates the OBEX "Get" operation to retrieve a vCard folder listing 
 *     from the remote Phonebook Access Server.  
 *
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Parms - Parameters to specify the application specific parameters
 *             including the folder name.
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
ObStatus PBAP_PullVcardListing(PbapClientSession *Client, 
                               PbapPullVcardListingParms *Parms);

/*---------------------------------------------------------------------------
 * PBAP_PullVcardEntry()
 *
 *     Initiates the OBEX "Get" operation to retrieve a vCard entry object 
 *     from the remote Phonebook Access Server.  
 *
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Parms - Parameters to specify the application specific parameters
 *             including the object name.
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
ObStatus PBAP_PullVcardEntry(PbapClientSession *Client, 
                             PbapPullVcardEntryParms *Parms);

/*---------------------------------------------------------------------------
 * PBAP_SetPhonebook()
 *
 *     Performs the OBEX "SetPath" operation to set the path to the current
 *     phonebook.  Afterwards, all operations are based on this phonebook
 *     until this function is called again.
 *     
 * Parameters:
 *     Parms - Parameters to specify the application specific parameters
 *             including the folder name.
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
ObStatus PBAP_SetPhonebook(PbapClientSession *Client, 
                           PbapSetPhonebookParms *Parms);

/*---------------------------------------------------------------------------
 * PBAP_ClientContinue()
 *
 *     This function is called by the client in response to a received
 *     PBAP_EVENT_CONTINUE event. It must be called once for every
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
ObStatus PBAP_ClientContinue(PbapClientSession *Client);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_ClientAuthenticate()
 *
 *     This function is called by the client to initiate OBEX authentication
 *     on the upcoming Phonebook Access OBEX connection.  This function must 
 *     be called prior to initiating an OBEX connection through 
 *     PBAP_ClientConnect in order to setup the OBEX Authentication Challenge 
 *     information. In addition, the PBAP_ClientSetAuthInfo routine should be 
 *     called prior to both of these functions to ensure that the proper 
 *     username, password, and challenge realm information are being used.
 *
 *     The authentication information is cleared when an OBEX Disconnect or
 *     transport disconnect occurs, so this should take place each time the
 *     connection is coming up.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The request to authenticate failed.
 *
 *     OB_STATUS_SUCCESS - The request to authenticate was successful.
 */
ObStatus PBAP_ClientAuthenticate(PbapClientSession *Client);

/*---------------------------------------------------------------------------
 * PBAP_ClientSetAuthInfo()
 *
 *     This function is called by the client to set the proper username, 
 *     password, and challenge realm information for the upcoming 
 *     Phonebook Access OBEX connection.  This information is used when 
 *     authenticating this OBEX connection.
 *     
 * Parameters:
 *     Client - The client application's registration structure.
 *
 *     Info - Structure containing the authentication information.
 *
 * Returns:
 *     OB_STATUS_FAILED - Setting the authentication information failed.
 *
 *     OB_STATUS_SUCCESS - Setting the authentication information succeeded.
 */
ObStatus PBAP_ClientSetAuthInfo(PbapClientSession *Client, PbapAuthInfo *Info);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif /* PBAP_NUM_CLIENTS > 0 */

/*---------------------------------------------------------------------------
 * PBAP_SetFilterBit()
 *
 *     Sets the appropriate filter bit in the 64-bit vCard filter.
 *
 * Parameters:
 *     Bit - Bit to set in the vCard filter.
 *
 *     Filter - vCard filter structure.
 */
void PBAP_SetFilterBit(PbapVcardFilterBit Bit, PbapVcardFilter *Filter);

#define PBAP_SetFilterBit(_BIT, _FILTER)   do {                 \
        Assert((_FILTER));                                      \
        (_FILTER)->byte[(_BIT)/8] |= (U8)(1 << ((_BIT)%8));     \
        } while (0);

/*---------------------------------------------------------------------------
 * PBAP_ClearFilterBit()
 *
 *     Clears the appropriate filter bit in the 64-bit vCard filter.
 *
 * Parameters:
 *     Bit - Bit to clear in the vCard filter.
 *
 *     Filter - vCard filter structure.
 */
void PBAP_ClearFilterBit(PbapVcardFilterBit Bit, PbapVcardFilter *Filter);

#define PBAP_ClearFilterBit(_BIT, _FILTER)  do {                \
        Assert((_FILTER));                                      \
        (_FILTER)->byte[(_BIT)/8] &= ~(U8)(1 << ((_BIT)%8));    \
        } while (0);

/*---------------------------------------------------------------------------
 * PBAP_ClearAllFilterBits()
 *
 *     Clears all of the filter bits in the 64-bit vCard filter. This should
 *     be done prior to setting any of the filter bits, just to ensure the
 *     memory is properly initialized prior to issuing a Pull Phonebook or 
 *     Pull Vcard Entry operation.
 *
 * Parameters:
 *     Filter - vCard filter structure.
 */
void PBAP_ClearAllFilterBits(PbapVcardFilter *Filter);

#define PBAP_ClearAllFilterBits(_FILTER)  do {                  \
        Assert((_FILTER));                                      \
        OS_MemSet((U8 *)(_FILTER)->byte, 0, PBAP_FILTER_SIZE);  \
        } while (0);

/*---------------------------------------------------------------------------
 * PBAP_IsSetFilterBit()
 *
 *     Returns the status of the appropriate filter bit in the 64-bit 
 *     vCard filter.
 *
 * Parameters:
 *     Bit - Bit to check in the vCard filter.
 *
 *     Filter - vCard filter structure.
 *
 * Returns:
 *     TRUE or FALSE
 */
BOOL PBAP_IsSetFilterBit(PbapVcardFilterBit Bit, PbapVcardFilter *Filter);

#define PBAP_IsSetFilterBit(_BIT, _FILTER)                      \
        (Assert((_FILTER)),                                      \
        ((_FILTER)->byte[(_BIT)/8] & (1 << ((_BIT)%8)) ? TRUE : FALSE))

#endif /* __PBAP_H */

