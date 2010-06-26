#ifndef __BIP_H
#define __BIP_H
/****************************************************************************
 *
 * File:          bip.h
 *     $Workfile:bip.h$ for XTNDAccess Blue SDK, Version 1.4
 *     $Revision:50$
 *
 * Description:   This file specifies defines and function prototypes for the
 *                BIP application.
 * 
 * Created:       August 6, 2002
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2002-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#include "goep.h"
/*---------------------------------------------------------------------------
 * Basic Imaging Profile (BIP) layer
 *
 *     The Basic Imaging Profile (BIP) specification defines Initiator and
 *     Responder roles to enable transferring images through a wireless
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
 * BIP_SUPPORTED_CAPABILITIES constant
 *     Defines the supported imaging capabilities of the responder.  This
 *     value is stored in the SDP entry as a UINT8 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_CAPABILITIES
#define BIP_SUPPORTED_CAPABILITIES  0x01
#endif

/*---------------------------------------------------------------------------
 * BIP_SUPPORTED_FEATURES constant
 *     Defines the supported imaging features of the responder. This value is
 *     stored in the SDP entry as a UINT16 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_FEATURES
#define BIP_SUPPORTED_FEATURES      0x0091
#endif

/*---------------------------------------------------------------------------
 * BIP_SUPPORTED_FUNCTIONS constant
 *     Defines the supported imaging functions of the responder. This value
 *     is stored in the SDP entry as a UINT32 bit mask. See the BIP
 *     SDP documentation for the bit mask definition.
 */
#ifndef BIP_SUPPORTED_FUNCTIONS
#define BIP_SUPPORTED_FUNCTIONS     0x41EB
#endif

/*---------------------------------------------------------------------------
 * BIP_IMAGING_DATA_CAPACITY constant
 *     Defines the total imaging data capacity in bytes supported by the
 *     responder. This value is stored in the SDP entry as a UINT64. For
 *     portability, it is represented here as an 8 byte array. The default
 *     is 8 Megabytes.
 */
#ifndef BIP_IMAGING_DATA_CAPACITY
#define BIP_IMAGING_DATA_CAPACITY \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00
#endif

#define BIPTYPESTR_CAPABILITIES    "x-bt/img-capabilities"
#define BIPTYPESTR_LISTING         "x-bt/img-listing"
#define BIPTYPESTR_PROPERTIES      "x-bt/img-properties"
#define BIPTYPESTR_IMG             "x-bt/img-img"
#define BIPTYPESTR_THM             "x-bt/img-thm"
#define BIPTYPESTR_ATTACHMENT      "x-bt/img-attachment"
#define BIPTYPESTR_PARTIAL         "x-bt/img-partial"
#define BIPTYPESTR_MONITORING      "x-bt/img-monitoring"
#define BIPTYPESTR_STATUS          "x-bt/img-status"
#define BIPTYPESTR_DISPLAY         "x-bt/img-display"
#define BIPTYPESTR_PRINT           "x-bt/img-print"
#define BIPTYPESTR_ARCHIVE         "x-bt/img-archive"

/* Number of Responder SDP Attributes */
#define BIP_NUM_ATTRIBUTES        9

/* Number of Initiator SDP search attributes */
#define BIP_NUM_SEARCH_ATTRIBUTES 21


/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BIP_NUM_INITIATORS Constant
 *
 *  Defines the number of concurrent Initiators supported.
 *
 *  The default value is 1. Set to 0 to disable Initiator support.
 */
#ifndef BIP_NUM_INITIATORS
#define BIP_NUM_INITIATORS          1
#endif

/*---------------------------------------------------------------------------
 * BIP_NUM_RESPONDERS Constant
 *
 *  Defines the number of concurrent Responders supported.
 *
 *  The default value is 1. Set to 0 to disable Responder support.
 */
#ifndef BIP_NUM_RESPONDERS
#define BIP_NUM_RESPONDERS          1
#endif

/* bipobs.h must be included after BIP_NUM_INITIATORS and BIP_NUM_RESPONDERS
 * are defined.
 */
#include "bipobs.h"

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

#if GOEP_ADDITIONAL_HEADERS < 3
#error "GOEP_ADDITIONAL_HEADERS must be 3 or greater!"
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
 * BIP_AUTO_ARCHIVE
 *
 *  Enables/Disables the Automatic Archive feature.
 *
 *  The default value is XA_DISABLED as this feature is currently not
 *  supported.
 */
#ifndef BIP_AUTO_ARCHIVE
#define BIP_AUTO_ARCHIVE        XA_DISABLED
#endif

#if BIP_AUTO_ARCHIVE == XA_ENABLED
#error "Auto Archive is not supported!"
#endif

/*---------------------------------------------------------------------------
 * BIP_REFERENCED_OBJECTS
 *
 *  Enables/Disables the Referenced Objects feature.
 *
 *  The default value is XA_DISABLED as this feature is currently not
 *  supported.
 */
#ifndef BIP_REFERENCED_OBJECTS
#define BIP_REFERENCED_OBJECTS  XA_DISABLED
#endif

#if BIP_REFERENCED_OBJECTS == XA_ENABLED
#error "Referenced Objects is not supported!"
#endif


#if TI_CHANGES == XA_ENABLED

/*---------------------------------------------------------------------------
 * BIP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 *  The default value is 20.
 */
#ifndef BIP_MAX_PASSWORD_LEN
#define BIP_MAX_PASSWORD_LEN     20
#endif

/*---------------------------------------------------------------------------
 * BIP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 *  The default value is 20.
 */
#ifndef BIP_MAX_USERID_LEN
#define BIP_MAX_USERID_LEN       20
#endif

/*---------------------------------------------------------------------------
 * BIP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 *  The default value is 20.
 */
#ifndef BIP_MAX_REALM_LEN
#define BIP_MAX_REALM_LEN        20
#endif

#endif	/* TI_CHANGES == XA_ENABLED */


/****************************************************************************
 *
 * Types
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BipEvent type
 *
 *  Indicates the current event type. All event indications and confirmations
 *  are passed to a callback function of type BipCallback. The
 *  "BipCallbackParms.event" field indicates one of the event types below.
 *  The "BipCallbackParms.data.bipOp" field indicates the applicable event
 *  operation.
 *      
 */
typedef U8 BipEvent;

#if (TI_CHANGES == XA_ENABLED)

/* Group: Initiator Events */

/* The client pointer is valid. This event indicates data for a PutImage
 * or PutLinkedThumbnail operation must be provided to complete a
 * BIP_SendRequest. The object context set by the Initiator in the
 * BIP_SendRequest is valid in the parms.data.ocx field. The parms.data.len
 * field indicates the amount of data requested. This amount must be the
 * actual amount provided. The parms.data.offset field indicates the offset
 * of data already provided. The parms.data.totLen field indicates the
 * total length of the data as specified in the BIP_SendRequest.
 * The status on this event is type ObStatus.
 */
#define BIPINITIATOR_DATA_REQ  (U8)(0x01)

/* The client pointer is valid and indicates data has arrived. The
 * parms.data.p union contains a valid data structure indicated by the
 * parms.data.dataType field. The object context set in the associated
 * BIPINITIATOR_RESPONSE event is valid in the parms.data.ocx field. The
 * parms.data.len, parms.data.totLen and parms.data.offset fields are
 * valid as described in the BIPINITIATOR_DATA_REQ event. This event is
 * always preceded by a BIPINITIATOR_RESPONSE event except for the
 * GetImagesList operation XML-HandlesDescriptor response and the PutImage
 * operation Img-Handle response. These will be indicated as they arrive
 * from the Responder.
 * The status on this event is type ObStatus.
 */
#define BIPINITIATOR_DATA_IND (U8)(0x02)
 
/* The client pointer is valid. This event will precede GetImage,
 * GetLinkedThumbnail and XML indications to allow the Initiator to
 * set up an object context for reception of data in following
 * BIPINITIATOR_DATA_IND events. The image handle field set in the
 * associated BIP_SendRequest in the parms.data.u union is preserved
 * and valid for reference.
 * The status on this event is type ObStatus.
 */
#define BIPINITIATOR_RESPONSE  (U8)(0x03)

/* The client pointer is valid. The client operation has completed. The
 * parms.status field indicates complete status. Any associated open handles
 * should be closed at this time. If on a BIPOP_PUT_IMAGE operation, the complete
 * status is OBRC_PARTIAL_CONTENT, the application must supply the Thumbnail
 * version of the image in a PutLinkedThumbnail operation immediately following
 * the PutImage operation, otherwise the status is type ObStatus.
 */
#define BIPINITIATOR_COMPLETE  (U8)(0x04)

/* The client pointer is valid. The parms.status field indicates failure
 * reason and is type ObexRespCode.
 */
#define BIPINITIATOR_FAILED    (U8)(0x05)

/* The client pointer is valid. The parms.status field indicates abort
 * reason and is type ObexRespCode.
 */
#define BIPINITIATOR_ABORT     (U8)(0x06)

/* A new connection has been established.
 * The ESI event fields in BipCallbackParms are:
 * ->event       = BIPINITIATOR_COMPLETE
 * ->data->bipOp = BIPOP_CONNECT
 * ->status      = OB_STATUS_SUCCESS
 * This was complex to detect, so the detection is simplified by replacing the
 * event value with this new one:
 * ->event       = BIPINITIATOR_CONNECTED
 */
#define BIPINITIATOR_CONNECTED (U8)(0x07)

/* The connection is lost now.
 * The ESI event fields in BipCallbackParms are:
 * ->event       = BIPINITIATOR_COMPLETE
 * ->data->bipOp = BIPOP_CONNECT
 * ->status      = OB_STATUS_DISCONNECT
 * This was complex to detect, so the detection is simplified by replacing the
 * event value with this new one:
 * ->event       = BIPINITIATOR_DISCONNECTED
 */
#define BIPINITIATOR_DISCONNECTED (U8)(0x08)

/* The Initiator context is disabled now.
 * It is triggered via the BTL_BIPINT_Disable function in the API.
 * It indicates the finish of a disconnection process because there
 * was stil a connection while the BTL_BIPINT_Disable was called.
 */
#define BIPINITIATOR_DISABLED (U8)(0x09)

/* Indicates that an authentication challenge request has
 * been received and is available in the event challenge field. If
 * the application intends on responding to this challenge it should do
 * so during this event notification. This is because there is a risk
 * that the peer may send more than one challenge and the GOEP layer
 * only tracks the last challenge received. However, it is not required
 * that the application respond during the event, as long as the
 * application always responds to the last challenge received.
 */
#define BIPINITIATOR_AUTH_CHALLENGE_RCVD (U8)(0x0A)

#define BIPINITIATOR_PROGRESS	(U8)(0x0B)

/* Group: Responder Events */

/* The server pointer is valid. This event indicates data for a GetImage,
 * GetLinkedThumbnail, GetMonitoringImage or XML operation must be provided
 * to complete a BIP_SendResponse. The object context set by the Responder in
 * the BIP_SendResponse is valid in the parms.data.ocx field. The parms.data.len
 * field indicates the amount of data requested. This amount must be the
 * actual amount provided. The parms.data.offset field indicates the offset
 * of data already provided. The parms.data.totLen field indicates the
 * total length of the data as specified in the BIP_SendResponse.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_DATA_REQ  (U8)(0x81)

/* The server pointer is valid and indicates data has arrived. The
 * parms.data.p union contains a valid data structure indicated by the
 * parms.data.dataType field. The object context set in the associated
 * BIPRESPONDER_REQUEST event is valid in the parms.data.ocx field. The
 * parms.data.len, parms.data.totLen and parms.data.offset fields are
 * valid as described in the BIPRESPONDER_DATA_REQ event. This event is
 * always preceded by a BIPRESPONDER_REQUEST event except for
 * XML-HandlesDescriptor and XML-ImageDescriptor indications. These will
 * be indicated as they arrive from the Initiator.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_DATA_IND  (U8)(0x82)
 
/* The server pointer is valid. This event will precede PutImage and
 * PutLinkedThumbnail indications to allow the Responder to
 * set up an object context for reception of data in following
 * BIPRESPONDER_DATA_IND events. The imgHandle field is valid for
 * PutLinkedThumbnail requests and the name field is valid for PutImage
 * requests in the parms.data.u union structure indicated by the
 * parms.data.dataType field.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_REQUEST   (U8)(0x83)

/* The server pointer is valid. The server operation has completed. The
 * parms.status field indicates complete status. Any associated open handles
 * should be closed at this time.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_COMPLETE  (U8)(0x84)

/* The server pointer is valid. The parms.status field indicates failure
 * reason and is type ObexRespCode.
 */
#define BIPRESPONDER_FAILED    (U8)(0x85)

/* The server pointer is valid. The parms.status field indicates abort
 * reason and is type ObexRespCode.
 */
#define BIPRESPONDER_ABORT     (U8)(0x86)

/* The server pointer is valid. Allows flow control on the server connection.
 * The server must call BIP_ResponderContinue when it is ready to continue
 * the operation. Useful for Responder Flow Control.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_CONTINUE  (U8)(0x87)

/* A new connection has been established.
 * The ESI event fields in BipCallbackParms are:
 * ->event       = BIPRESPONDER_COMPLETE
 * ->data->bipOp = BIPOP_CONNECT
 * ->status      = OB_STATUS_SUCCESS
 * This was complex to detect, so the detection is simplified by replacing the
 * event value with this new one:
 * ->event       = BIPRESPONDER_CONNECTED
 */
#define BIPRESPONDER_CONNECTED (U8)(0x88)

/* The connection is lost now.
 * The ESI event fields in BipCallbackParms are:
 * ->event       = BIPRESPONDER_COMPLETE
 * ->data->bipOp = BIPOP_CONNECT
 * ->status      = OB_STATUS_DISCONNECT
 * This was complex to detect, so the detection is simplified by replacing the
 * event value with this new one:
 * ->event       = BIPRESPONDER_DISCONNECTED
 */
#define BIPRESPONDER_DISCONNECTED (U8)(0x89)

/* The Responder context is disabled now.
 * It is triggered via the BTL_BIPRSP_Disable function in the API.
 * It indicates the finish of a disconnection process because there
 * was stil a connection while the BTL_BIPRSP_Disable was called.
 */
#define BIPRESPONDER_DISABLED (U8)(0x8A)

#define BIPRESPONDER_PROGRESS	(U8)(0x8B)


#else /* TI_CHANGES == XA_ENABLED */

/* Group: Initiator Events */

/* The client pointer is valid. This event indicates data for a PutImage
 * or PutLinkedThumbnail operation must be provided to complete a
 * BIP_SendRequest. The object context set by the Initiator in the
 * BIP_SendRequest is valid in the parms.data.ocx field. The parms.data.len
 * field indicates the amount of data requested. This amount must be the
 * actual amount provided. The parms.data.offset field indicates the offset
 * of data already provided. The parms.data.totLen field indicates the
 * total length of the data as specified in the BIP_SendRequest.
 * The status on this event is type ObStatus.
 */
#define BIPINITIATOR_DATA_REQ  0x01

/* The client pointer is valid and indicates data has arrived. The
 * parms.data.p union contains a valid data structure indicated by the
 * parms.data.dataType field. The object context set in the associated
 * BIPINITIATOR_RESPONSE event is valid in the parms.data.ocx field. The
 * parms.data.len, parms.data.totLen and parms.data.offset fields are
 * valid as described in the BIPINITIATOR_DATA_REQ event. This event is
 * always preceded by a BIPINITIATOR_RESPONSE event except for the
 * GetImagesList operation XML-HandlesDescriptor response and the PutImage
 * operation Img-Handle response. These will be indicated as they arrive
 * from the Responder.
 * The status on this event is type ObStatus.
 */
#define BIPINITIATOR_DATA_IND  0x02
 
/* The client pointer is valid. This event will precede GetImage,
 * GetLinkedThumbnail and XML indications to allow the Initiator to
 * set up an object context for reception of data in following
 * BIPINITIATOR_DATA_IND events. The image handle field set in the
 * associated BIP_SendRequest in the parms.data.u union is preserved
 * and valid for reference.
 * The status on this event is type ObStatus.
 */
#define BIPINITIATOR_RESPONSE  0x03

/* The client pointer is valid. The client operation has completed. The
 * parms.status field indicates complete status. Any associated open handles
 * should be closed at this time. If on a BIPOP_PUT_IMAGE operation, the complete
 * status is OBRC_PARTIAL_CONTENT, the application must supply the Thumbnail
 * version of the image in a PutLinkedThumbnail operation immediately following
 * the PutImage operation, otherwise the status is type ObStatus.
 */
#define BIPINITIATOR_COMPLETE  0x04

/* The client pointer is valid. The parms.status field indicates failure
 * reason and is type ObexRespCode.
 */
#define BIPINITIATOR_FAILED    0x05

/* The client pointer is valid. The parms.status field indicates abort
 * reason and is type ObexRespCode.
 */
#define BIPINITIATOR_ABORT     0x06

/* Group: Responder Events */

/* The server pointer is valid. This event indicates data for a GetImage,
 * GetLinkedThumbnail, GetMonitoringImage or XML operation must be provided
 * to complete a BIP_SendResponse. The object context set by the Responder in
 * the BIP_SendResponse is valid in the parms.data.ocx field. The parms.data.len
 * field indicates the amount of data requested. This amount must be the
 * actual amount provided. The parms.data.offset field indicates the offset
 * of data already provided. The parms.data.totLen field indicates the
 * total length of the data as specified in the BIP_SendResponse.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_DATA_REQ  0x81

/* The server pointer is valid and indicates data has arrived. The
 * parms.data.p union contains a valid data structure indicated by the
 * parms.data.dataType field. The object context set in the associated
 * BIPRESPONDER_REQUEST event is valid in the parms.data.ocx field. The
 * parms.data.len, parms.data.totLen and parms.data.offset fields are
 * valid as described in the BIPRESPONDER_DATA_REQ event. This event is
 * always preceded by a BIPRESPONDER_REQUEST event except for
 * XML-HandlesDescriptor and XML-ImageDescriptor indications. These will
 * be indicated as they arrive from the Initiator.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_DATA_IND  0x82
 
/* The server pointer is valid. This event will precede PutImage and
 * PutLinkedThumbnail indications to allow the Responder to
 * set up an object context for reception of data in following
 * BIPRESPONDER_DATA_IND events. The imgHandle field is valid for
 * PutLinkedThumbnail requests and the name field is valid for PutImage
 * requests in the parms.data.u union structure indicated by the
 * parms.data.dataType field.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_REQUEST   0x83

/* The server pointer is valid. The server operation has completed. The
 * parms.status field indicates complete status. Any associated open handles
 * should be closed at this time.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_COMPLETE  0x84

/* The server pointer is valid. The parms.status field indicates failure
 * reason and is type ObexRespCode.
 */
#define BIPRESPONDER_FAILED    0x85

/* The server pointer is valid. The parms.status field indicates abort
 * reason and is type ObexRespCode.
 */
#define BIPRESPONDER_ABORT     0x86

/* The server pointer is valid. Allows flow control on the server connection.
 * The server must call BIP_ResponderContinue when it is ready to continue
 * the operation. Useful for Responder Flow Control.
 * The status on this event is type ObStatus.
 */
#define BIPRESPONDER_CONTINUE  0x87

#endif /* TI_CHANGES == XA_ENABLED */

/* End of BipEvent */


/*---------------------------------------------------------------------------
 * BipOp type
 *
 *  Indicates the operation type of the current event. Each event indication
 *  has an associated operation passed to a callback function of type
 *  BipCallback. The "BipCallbackParms.data.bipOp" field will indicate one
 *  of the operation types below.
 */
typedef U32 BipOp;

#define BIPOP_GET_CAPABILITIES      0x80000000 /* GetCapabilities Op */
#define BIPOP_PUT_IMAGE             0x00000001 /* PutImage Op */
#define BIPOP_PUT_LINKED_THUMBNAIL  0x00000004 /* PutLinkedThumbnail Op */
#define BIPOP_GET_IMAGES_LIST       0x00000010 /* GetImagesList Op */
#define BIPOP_GET_IMAGE_PROPERTIES  0x00000020 /* GetImageProperties Op */
#define BIPOP_GET_IMAGE             0x00000040 /* GetImage Op */
#define BIPOP_GET_LINKED_THUMBNAIL  0x00000080 /* GetLinkedThumbnail Op */
#define BIPOP_GET_MONITORING_IMAGE  0x00002000 /* GetMonitoringImage Op
                                                * (for Remote Camera feature)
                                                */

#define BIPOP_CONNECT               0x20000000 /* Connect operation */
#define BIPOP_NOP                   0x00000000 /* No operation in progress */
/* End of BipOp */
#define BIPOP_PUT_LINKED_ATTACHMENT 0x00000002 /* PutLinkedAttachment Op */
#define BIPOP_REMOTE_DISPLAY        0x00000008 /* RemoteDisplay Op */
#define BIPOP_GET_LINKED_ATTACHMENT 0x00000100 /* GetLinkedAttachment Op */
#define BIPOP_DELETE_IMAGE          0x00000200 /* DeleteImage Op */
#define BIPOP_START_PRINT           0x00000400 /* StartPrint Op */
#define BIPOP_START_ARCHIVE         0x00001000 /* StarArchive Op */
#define BIPOP_GETSTATUS             0x00008000 /* GetStatus Op */

/*---------------------------------------------------------------------------
 * BipDataType type
 *
 *  Specifies the type of data in a request or indication.
 */
typedef U16 BipDataType;

/* Group: Request event types. The BIPREQ_XX events occur when additional
 * data is required to complete an Initiator request or Responder response.
 */
#define BIPREQ_IMAGE                0x0010    /* BipImageReq Data */
#define BIPREQ_THUMBNAIL            0x0020    /* BipThumbnailReq Data */
#define BIPREQ_IMG_CAPABILITIES_XML 0x0030    /* BipImgCapabilitiesReq XML */
#define BIPREQ_IMG_LISTING_XML      0x0040    /* BipImgListingReq XML */
#define BIPREQ_IMG_PROPERTIES_XML   0x0050    /* BipImgPropertiesReq XML */

/* Group: Indication event types. The BIPIND_XX events occur to provide
 * Initiator data to the Responder on a request, or Responder data to an
 * Initiator on a response.
 */
#define BIPIND_NO_DATA              0x0000    /* No Valid Data */
#define BIPIND_IMAGE                0x0001    /* BipImageInd Data */
#define BIPIND_THUMBNAIL            0x0002    /* BipThumbnailInd Data */
#define BIPIND_IMG_CAPABILITIES_XML 0x0004    /* BipImgCapabilitiesInd XML */
#define BIPIND_IMG_LISTING_XML      0x0005    /* BipImgListingInd XML */
#define BIPIND_IMG_PROPERTIES_XML   0x0006    /* BipImgPropertiesInd XML */
#define BIPIND_HANDLES_DESCRIPTOR   0x0007    /* BipHandlesDescriptorInd XML */
#define BIPIND_IMAGE_DESCRIPTOR     0x0008    /* BipImgDescriptorInd XML */
#define BIPIND_IMAGE_HANDLE         0x0009    /* BipHandleInd data */
/* End of BipDataType */

/*---------------------------------------------------------------------------
 * BipChannel type
 *
 *  Indicates the channel type of the current API or event. Each event
 *  indication has an associated BipChannel passed to a callback function
 *  of type BipCallback. The "BipCallbackParms.channel" field will indicate
 *  one of the channel types below.
 */
typedef U8 BipChannel;

#define BIPCH_INITIATOR_PRIMARY     0x01    /* Initiator Primary channel */
#define BIPCH_RESPONDER_PRIMARY     0x81    /* Responder Primary channel */
/* End of BipChannel */
#define BIPCH_INITIATOR_SECONDARY   0x02    /* Initiator Secondary channel */
#define BIPCH_RESPONDER_SECONDARY   0x82    /* Responder Secondary channel */

/*---------------------------------------------------------------------------
 *  These BIP Feature definitions map directly to the BIP Supported Features
 *  bit mask defined for SDP.
 */
#define BIPF_IMAGE_PUSH          0x0001
#define BIPF_IMAGE_PUSH_STORE    0x0002
#define BIPF_IMAGE_PUSH_PRINT    0x0004
#define BIPF_IMAGE_PUSH_DISPLAY  0x0008
#define BIPF_IMAGE_PULL          0x0010
#define BIPF_ADVANCED_PRINTING   0x0020
#define BIPF_AUTOMATIC_ARCHIVE   0x0040
#define BIPF_REMOTE_CAMERA       0x0080
#define BIPF_REMOTE_DISPLAY      0x0100
#define BIPF_REFERENCED_OBJECTS  0x8000

/*---------------------------------------------------------------------------
 * BipService type
 *
 *  Describes the BIP Service type required for connections.
 */
typedef U16 BipService;

/* BIP Imaging Responder Service (Image Push, Image Pull and
 * Remote Camera services.)
 */
#define BIPSVC_RESPONDER            (BIPF_IMAGE_PUSH | BIPF_IMAGE_PULL | BIPF_REMOTE_CAMERA)

/* BIP Imaging Responder Image Push Service only. */
#define BIPSVC_IMAGE_PUSH           BIPF_IMAGE_PUSH

/* BIP Imaging Responder Image Pull Service only. */
#define BIPSVC_IMAGE_PULL           BIPF_IMAGE_PULL

/* BIP Imaging Responder Remote Camera Service only. */
#define BIPSVC_REMOTE_CAMERA        BIPF_REMOTE_CAMERA

/* BIP Imaging Automatic Archive Service (not supported) */
#define BIPSVC_AUTO_ARCHIVE         BIPF_AUTOMATIC_ARCHIVE

/* BIP Imaging Referenced Objects Service (not supported) */
#define BIPSVC_REFERENCED_OBJECTS   BIPF_REFERENCED_OBJECTS
/* End of BipService */

/* Forward reference to BipCallbackParms defined below */
typedef struct _BipCallbackParms BipCallbackParms;

/*---------------------------------------------------------------------------
 * BipCallback type
 *
 *  BIP events and data are passed to the application through a callback
 *  function of this type defined by the application.
 */
typedef void (*BipCallback)(BipCallbackParms *parms);
/* End of BipCallback */

typedef U8 BipChanState;    /* Used internally */
typedef U8 BipUuid[16];     /* Used internally */
#if TI_CHANGES == XA_ENABLED
typedef U8 BipAuthFlags;
#endif	/* TI_CHANGES == XA_ENABLED */

/****************************************************************************
 *
 * Data Structures
 *
 ***************************************************************************/

#if BT_STACK == XA_ENABLED
/****************************************************************************
 *
 * External references to SDP objects registered by Basic Printing Servers.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BIP Basic Imaging Responder attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * BIP Responder SDP record.
 *
 */
extern const SdpAttribute BipSdpAttributes[BIP_NUM_ATTRIBUTES];

/****************************************************************************
 *
 * External reference to SDP object used to query Basic Imaging Responders.
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 *
 * SDP query info
 *
 * Service search attribute request for Basic Imaging Responder.
 * The service search pattern is very specific. It contains the UUIDs
 * for BIP Imaging Responder, OBEX, L2CAP, and RFCOMM.
 *
 */
extern const U8 BipServiceSearchAttribReq[BIP_NUM_SEARCH_ATTRIBUTES];
#endif /* BT_STACK == XA_ENABLED */


/****************************************************************************
 *
 * Forward references of structures in BipCallbackParms.
 *
 ****************************************************************************/
typedef struct _BipData         BipData;
typedef struct _BipObexClient   BipObexClient;
typedef struct _BipObexServer   BipObexServer;

/*---------------------------------------------------------------------------
 * BipCallbackParms structure
 *
 *  Describes a callback event and any data that relates to the event.
 */
struct _BipCallbackParms
{
    /* See documentation on each event for status type (ObStatus
     * or ObexRespCode.)
     */
    U16         status;

    BipEvent    event;      /* Type of BIP event */
    BipChannel  channel;    /* BipChannel of operation */
    BipData    *data;       /* Operation specific data */

    /* Group: BipObexClient or BipObexServer handle dependent on Role
     * (Initiator or Responder) and channel. This handle is required when
     * responding to callback events. It is normally not necessary to
     * reference the elements of these structures.
     */
    union {
        /* BipObexClient when event is on Initiator Primary channel */
        BipObexClient   *client;

        /* BipObexServer when event is on Responder Primary channel */
        BipObexServer   *server;
    } obex;
};
/* End of BipCallbackParms */

/*----------------------------------------------------------------------
 * BipImgHandle type
 *
 *  The Img-Handle references images on a Responder device. The
 *  Initiator must specify it for GetImage, GetImageProperties,
 *  GetLinkedThumbnail and PutLinkedThumbnail requests. The Responder
 *  returns it in the PutImage response. It is a 7 character, null
 *  terminated, UTF-16 encoded string containing only digits 0
 *  through 9. For convenience, the BIP profile will convert UTF-8
 *  characters to UTF-16.
 */
typedef U8 BipImgHandle[8];
/* End of BipImgHandle */

/*----------------------------------------------------------------------
 * BipImgDescription type
 *
 *  The Img-Description describes XML data for GetImagesList, GetImage
 *  and PutImage requests. The creator of the Img-Description data must
 *  indicate the length in the len field.
 */
typedef U8 *BipImgDescription;
/* End of BipImgDescription */

/*----------------------------------------------------------------------
 * BipOcx type
 *
 *  The BipOcx indicates the Image or XML object context. This
 *  context is set by the application in BIP_SendRequest and
 *  BIP_SendResponse commands or BIPRESPONDER_REQUEST and
 *  BIPINITIATOR_RESPONSE events. It is then valid on the associated
 *  BIPINITIATOR_DATA_REQ and BIPRESPONDER_DATA_REQ or BIPINITIATOR_DATA_IND
 *  and BIPRESPONDER_DATA_IND events.
 */
typedef void *BipOcx;
/* End of BipOcx */

/*----------------------------------------------------------------------
 * BipImageReq structure
 *
 *  Contains data elements for an Image request.
 *  BipDataType == BIPREQ_IMAGE
 *
 */
typedef struct _BipImageReq {
    BipImgHandle        imgHandle;          /* Set on PutImage and GetImage only */
    U8          name[GOEP_MAX_UNICODE_LEN]; /* Set on PutImage only */
    U8         *buff;                       /* Segment of Image */
    BipImgDescription   imageDescriptor;    /* Image Descriptor */       
    U16                 imageDescriptorLen; /* Descriptor length */       

#if (TI_CHANGES == XA_ENABLED)
	BOOL            noImgHandle;            /* Used for GetMonitoringImage only */
#endif
} BipImageReq;
/* End of BipImageReq */

/*----------------------------------------------------------------------
 * BipThumbnailReq structure
 *
 *  Contains data elements for a Thumbnail request.
 *  BipDataType == BIPREQ_THUMBNAIL
 *
 */
typedef struct _BipThumbnailReq {
    BipImgHandle    imgHandle;  /* Image handle */
    U8             *buff;       /* Segment of Thumbnail */
} BipThumbnailReq;
/* End of BipThumbnailReq */

/*----------------------------------------------------------------------
 * BipImgCapabilitiesReq structure
 *
 *  Contains data elements for an Image Capabilities request.
 *  BipDataType == BIPREQ_IMG_CAPABILITIES_XML
 *
 */
typedef struct _BipImgCapabilitiesReq {
    U8     *buff;           /* Segment of Image Capabilities XML */
} BipImgCapabilitiesReq;
/* End of BipImgCapabilitiesReq */

/*----------------------------------------------------------------------
 * BipImgListingReq structure
 *
 *  Contains data elements for an Images Listing request.
 *  BipDataType == BIPREQ_IMG_LISTING_XML
 *
 */
typedef struct _BipImgListingReq {
    U8     *buff;           /* Segment of Images Listing XML */
    U16     nbReturnedHandles;      /* Number of returned handles */
    U16     listStartOffset;        /* Start offset of listing */
    U8      latestCapturedImages;   /* Boolean: 1 == True, 0 == False */
    BipImgDescription   handlesDescriptor;  /* Handles Descriptor */
    U16                 handlesDescriptorLen;   /* Descriptor length */
} BipImgListingReq;
/* End of BipImgListingReq */

/*----------------------------------------------------------------------
 * BipImgPropertiesReq structure
 *
 *  Contains data elements for an Image Properties request.
 *  BipDataType == BIPREQ_IMG_PROPERTIES_XML
 *
 */
typedef struct _BipImgPropertiesReq {
    BipImgHandle    imgHandle;  /* Image handle */
    U8             *buff;       /* Segment of Image Properties XML */
} BipImgPropertiesReq;
/* End of BipImgPropertiesReq */

/*----------------------------------------------------------------------
 * BipImgMonitorReq structure
 *
 *  Contains data elements for a Get Monitoring Image request.
 *  BipDataType == BIPOP_GET_MONITORING_IMAGE
 *
 */
typedef struct _BipImgMonitorReq {
    U8                  storeFlag;  /* StoreFlag: 0x00 or 0x01 */       
} BipImgMonitorReq;
/* End of BipImgMonitorReq */

/*----------------------------------------------------------------------
 * BipHandleInd structure
 *
 *  Contains data elements for an Image Handle indication.
 *   BipDataType == BIPIND_IMAGE_HANDLE
 *
 */
typedef struct _BipHandleInd {
    BipImgHandle    imgHandle;          /* Returned on PutImage */
} BipHandleInd;
/* End of BipHandleInd */

/*----------------------------------------------------------------------
 * BipImageInd structure
 *
 *  Contains data elements for an Image indication.
 *   BipDataType == BIPIND_IMAGE
 *
 */
typedef struct _BipImageInd {
    U8              name[GOEP_MAX_UNICODE_LEN]; /* On PutImage only */
    U8             *buff;       /* Segment of Image */
} BipImageInd;
/* End of BipImageInd */

/*----------------------------------------------------------------------
 * BipThumbnailInd structure
 *
 *  Contains data elements for a Thumbnail indication.
 *  BipDataType == BIPIND_THUMBNAIL
 *
 */
typedef struct _BipThumbnailInd {
    U8             *buff;       /* Segment of Thumbnail */
} BipThumbnailInd;
/* End of BipThumbnailInd */

/*----------------------------------------------------------------------
 * BipImgCapabilitiesInd structure
 *
 *  Contains data elements for an Image Capabilities indication.
 *  BipDataType == BIPIND_IMG_CAPABILITIES_XML
 *
 */
typedef struct _BipImgCapabilitiesInd {
    U8     *buff;           /* Segment of Image Capabilities XML */
} BipImgCapabilitiesInd;
/* End of BipImgCapabilitiesInd */

/*----------------------------------------------------------------------
 * BipImgListingInd structure
 *
 *  Contains data elements for an Images Listing indication.
 *  BipDataType == BIPIND_IMG_LISTING_XML
 *
 */
typedef struct _BipImgListingInd {
    U8     *buff;           /* Segment of Images Listing XML */
    U16     nbReturnedHandles;      /* Number of returned handles */
    U16     listStartOffset;        /* Start offset of listing */
    U8      latestCapturedImages;   /* Boolean: 1 == True, 0 == False */
} BipImgListingInd;
/* End of BipImgListingInd */

/*----------------------------------------------------------------------
 * BipImgPropertiesInd structure
 *
 *  Contains data elements for an Image Properties indication.
 *  BipDataType == BIPIND_IMG_PROPERTIES_XML
 *
 */
typedef struct _BipImgPropertiesInd {
    U8             *buff;       /* Segment of Image Properties XML */
} BipImgPropertiesInd;
/* End of BipImgPropertiesInd */

/*----------------------------------------------------------------------
 * BipHandlesDescriptorInd structure
 *
 *  Contains data elements for an Handles Descriptor indication.
 *  BipDataType == BIPIND_HANDLES_DESCRIPTOR
 *
 */
typedef struct _BipHandlesDescriptorInd {
    BipImgDescription   buff;       /* Segment of Handles Descriptor */
} BipHandlesDescriptorInd;
/* End of BipHandlesDescriptorInd */

/*----------------------------------------------------------------------
 * BipImgDescriptorInd structure
 *
 *  Contains data elements for an Image Descriptor indication.
 *  BipDataType == BIPIND_IMAGE_DESCRIPTOR
 *
 */
typedef struct _BipImgDescriptorInd {
    BipImgDescription   buff;       /* Segment of Image Descriptor */       
} BipImgDescriptorInd;
/* End of BipImgDescriptorInd */

#if TI_CHANGES == XA_ENABLED

typedef struct _BtlObjProgressInd BtlObjProgressInd;

#endif

/*----------------------------------------------------------------------
 * BipData structure
 *
 *  Maintains XML and Image requests or responses.
 */
struct _BipData {
    /* The current BIP operation is always valid. Other
     * elements are valid depending on the operation.
     */
    BipOp   bipOp;

    /* BipDataType indicates which structure is valid for the operation
     * in the following union.
     */
    BipDataType     dataType;

    /* Union of operation dependent data structures. The dataType
     * parameter indicates which structure is valid.
     */
    union {
        /* Group: Request structure types are used to provide parameters for
         * Initiator requests and may also be valid on certain BIPRESPONDER_REQUEST
         * and BIPXX_DATA_REQ events to provide additional required parameters.
         */
        BipImageReq                rImg;
        BipThumbnailReq            rThm;
        BipImgCapabilitiesReq      rCapa;
        BipImgListingReq           rList;
        BipImgPropertiesReq        rProp;
        BipImgMonitorReq           rMon;
    } r;

    union {
        /* Group: Indication structure types. The BIPXX_DATA_IND events
         * occur to provide Initiator data to the Responder on a request,
         * or Responder data to an Initiator on a response.
         */
        BipImageInd                iImg;
        BipThumbnailInd            iThm;
        BipImgCapabilitiesInd      iCapa;
        BipImgListingInd           iList;
        BipImgPropertiesInd        iProp;
        BipHandlesDescriptorInd    iHDsc;
        BipImgDescriptorInd        iIDsc;
        BipHandleInd               iHndl;
    } i;

    U32     len;        /* Current segment length of data */
    U32     offset;     /* Progress length of data */
    U32     totLen;     /* Total length of data */
    BipOcx  ocx;        /* BipData object context */

#if TI_CHANGES == XA_ENABLED

    BtlObjProgressInd*	progressInfo;

#endif
};
/* End of BipData */


#if TI_CHANGES == XA_ENABLED
#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * BipAuthInfo structure
 *
 *  Describes the private information exchanged during OBEX authentication.
 *  These parameters must be filled in by the client/server application prior
 *  to calling BIP_ClientSetAuthInfo.
 */
typedef struct _BipAuthInfo
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
} BipAuthInfo;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif	/* TI_CHANGES == XA_ENABLED */


/*----------------------------------------------------------------------
 * BipObexClient structure
 *
 *  Maintains connection info and status for OBEX clients.
 */
struct _BipObexClient {
    /* This must be the first element of the structure. Its value is
     * copied to the BipCallbackParms.channel field on a callback.
     */
    BipChannel      channel;

    BipData        *data;   /* Pointer to data passed in on request */

    /* === Internal use only === */
    BipData         tData;
    BipUuid         bipUuid;
    BipChanState    state;
    GoepClientApp   cApp;
    GoepConnectReq  connect;
    GoepObjectReq   pushPull;
    U8              appParms[20];
    BipImgHandle    imgHandle;

#if TI_CHANGES == XA_ENABLED
	BipAuthFlags    flags;

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* OBEX Authentication password */
    U8                  password[BIP_MAX_PASSWORD_LEN];
    /* OBEX Authentication userId */
    U8                  userId[BIP_MAX_USERID_LEN];
    /* OBEX Authentication realm */
    U8                  realm[BIP_MAX_REALM_LEN];
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#endif	/* TI_CHANGES == XA_ENABLED */
};
/* End of BipObexClient */

/*----------------------------------------------------------------------
 * BipObexServer structure
 *
 *  Maintains connection info and status for OBEX servers.
 */
struct _BipObexServer {
    /* This must be the first element of the structure. Its value is
     * copied to the BipCallbackParms.channel field on a callback.
     */
    BipChannel      channel;

    /* === Internal use only === */
    BipData         request;
    BipUuid         bipImgPushUuid;
    BipUuid         bipImgPullUuid;
    BipUuid         bipRemoteCameraUuid;
    BipChanState    state;
    GoepServerApp   sApp;
    ObexConnection  targetHdr[3];
    U8              appParms[20];
    BipImgHandle    imgHandle;
    BipObStoreHandle   object;
};
/* End of BipObexServer */

#if BIP_NUM_INITIATORS > 0
/*----------------------------------------------------------------------
 * BipInitiatorSession structure
 *
 *  Maintains Initiator info and status during an application Initiator
 *  session.
 */
typedef struct _BipInitiatorSession {
    BipObexClient   primary;    /* Initiator Primary channel */
    U8              srchAttr[BIP_NUM_SEARCH_ATTRIBUTES];
} BipInitiatorSession;
/* End of BipInitiatorSession */
#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
/*----------------------------------------------------------------------
 * BipResponderSession structure
 *
 *  Maintains Responder info and status during an application Responder
 *  session.
 */
typedef struct _BipResponderSession {
    BipObexServer   primary;        /* Responder Primary channel */
    SdpRecord       responderRecord;/* SDP record for BIP Responder */
    /* Responder SDP attributes */
    SdpAttribute    responderAttrib[BIP_NUM_ATTRIBUTES];
} BipResponderSession;
/* End of BipResponderSession */
#endif /* BIP_NUM_RESPONDERS */

/****************************************************************************
 *
 * Function Reference
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Section: Functions common between Initiator and Responder
 *
 ***************************************************************************/

void BIP_ServerAccept(BipObexServer *server, ObexRespCode code, BipData *data);
void BIP_ServerContinue(BipObexServer *server);
void BIP_ClientAccept(BipObexClient *client, BipData *data);

/*---------------------------------------------------------------------------
 * BIP_Abort()
 *
 *      Abort operation on specified channel of Initiator or Responder
 *      session.
 *
 * Parameters:
 *      session - Pointer to aborting BipInitiatorSession or 
 *                BipResponderSession.
 *      channel - Aborting channel.
 *
 */
void BIP_Abort(void *session, BipChannel channel);

#if BIP_NUM_INITIATORS > 0
/****************************************************************************
 *
 * Section: Functions specific to Initiator
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BIP_RegisterInitiator()
 *
 *      Registers BIP Initiator and callback. This function must be called
 *      before any other BIP Initiator functions.
 *
 * Parameters:
 *      initiator - Pointer to BipInitiatorSession object to register.
 *      callback - Identifies the application function that will be called
 *          with BIP Initiator events. If configured for multiple Initiators,
 *          the callback must match previous Initiator registrations.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BIP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_FAILED - BIP callback failed to register.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *      BT_STATUS_NO_RESOURCES - The BIP callback was already registered. 
 */
BtStatus BIP_RegisterInitiator(BipInitiatorSession *initiator, BipCallback callback);

/*---------------------------------------------------------------------------
 * BIP_DeregisterInitiator()
 *
 *      Deregisters BIP Initiator and callback.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BIP application was
 *      successfully deregistered.
 *
 *      BT_STATUS_IN_PROGRESS - BIP application is busy.
 *
 */
BtStatus BIP_DeregisterInitiator(BipInitiatorSession *initiator);

/*---------------------------------------------------------------------------
 * BIP_Connect()
 *
 *      Connects an Initiator to a Responder with the specified service.
 *
 * Parameters:
 *      initiator - Pointer to BipInitiatorSession object to connect.
 *      addr - Pointer to structure containing Responder's address.
 *      bipService - Service of desired connection.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The connection completed successfully.
 *
 *      OB_STATUS_PENDING - The connection procedure was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BIP_Connect(BipInitiatorSession *initiator, ObexTpAddr *addr, BipService bipService);

/*---------------------------------------------------------------------------
 * BIP_Disconnect()
 *
 *      Disconnects all Initiator channels from a Responder.
 *
 * Parameters:
 *      initiator - Pointer to BipInitiatorSession object to disconnect.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - Disconnect completed successfully.
 *
 *      OB_STATUS_PENDING - Disconnect procedure was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BIP_Disconnect(BipInitiatorSession *initiator);

/*---------------------------------------------------------------------------
 * BIP_SendRequest()
 *
 *      Sends an XML Request from the Initiator.
 *
 * Parameters:
 *      initiator - Pointer to BipInitiatorSession object.
 *      data - Pointer to request initialized BipData structure. The BIP
 *             Profile owns the structure until the BIPINITIATOR_COMPLETE event
 *             for the request. It will be returned in the BIPINITIATOR_RESPONSE,
 *             BIPINITIATOR_DATA_REQ and BIPINITIATOR_DATA_IND events associated
 *             with the request.
 *
 * Returns:
 *      OB_STATUS_PENDING - The XML request was started.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BIP_SendRequest(BipInitiatorSession *initiator, BipData *data);

/*---------------------------------------------------------------------------
 * BIP_InitiatorAccept()
 *
 *      Indicates Initiator accepts the current operation. The accept must
 *      include a BipData pointer for GetImage and GetLinkedThumbnail
 *      operations.
 *
 * Parameters:
 *      client - Pointer to Initiator BipObexClient object indicated in
 *               event.
 *      data - Pointer to BipData structure with handle set for receiving
 *             object on GetImage and GetLinkedThumbnail operations.
 *
 */
void BIP_InitiatorAccept(BipObexClient *client, BipData *data);
#define BIP_InitiatorAccept(c, d) (BIP_ClientAccept(c, d))

#if  TI_CHANGES == XA_ENABLED
#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * BIP_ClientSetAuthInfo()
 *
 *     This function is called by the client to set the proper username, 
 *     password, and challenge realm information for the upcoming BIP OBEX 
 *     connection.  This information is used when authenticating this OBEX
 *     connection.
 *     
 * Parameters:
 *     client - Pointer to BipObexClient object.
 *     info - Structure containing the authentication information.
 *
 * Returns:
 *     OB_STATUS_FAILED - Setting the authentication information failed.
 *
 *     OB_STATUS_SUCCESS - Setting the authentication information succeeded.
 */
ObStatus BIP_ClientSetAuthInfo(BipObexClient *client, BipAuthInfo *info);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif /* TI_CHANGES == XA_ENABLED */

#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
/****************************************************************************
 *
 * Section: Functions specific to Responder
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * BIP_RegisterResponder()
 *
 *      Registers BIP Responder and callback. This function must be called
 *      before any other BIP Responder functions.
 *
 * Parameters:
 *      initiator - Pointer to BipResponderSession object to register.
 *      callback - Identifies the application function that will be called
 *          with BIP Responder events. If configured for multiple Responders,
 *          the callback must match previous Responder registrations.
 *
 * Requires:
 *     BIP_NUM_RESPONDERS set to 1 or higher
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BIP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_FAILED - BIP callback failed to register.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *      BT_STATUS_NO_RESOURCES - The BIP callback was already registered. 
 */
BtStatus BIP_RegisterResponder(BipResponderSession *responder, BipCallback callback);

/*---------------------------------------------------------------------------
 * BIP_DeregisterResponder()
 *
 *      Deregisters BIP Responder and callback.
 *
 * Requires:
 *     BIP_NUM_RESPONDERS set to 1 or higher
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BIP application was
 *      successfully deregistered.
 *
 *      BT_STATUS_IN_PROGRESS - BIP application is busy.
 *
 */
BtStatus BIP_DeregisterResponder(BipResponderSession *responder);

/*---------------------------------------------------------------------------
 * BIP_InitResponderChannel()
 *
 *      Initialize a BIP Responder channel and set an application specific
 *      context. A channel must be initialized before use.
 *
 * Requires:
 *     BIP_NUM_RESPONDERS set to 1 or higher
 *
 * Parameters:
 *      session - Pointer to BipResponderSession.
 *      channel - Identifies the BipChannel.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BIP channel was
 *      successfully initialized.
 *
 *      BT_STATUS_FAILED - Init failure.
 */
BtStatus BIP_InitResponderChannel(BipResponderSession *session, BipChannel channel);

/*---------------------------------------------------------------------------
 * BIP_SendResponse()
 *
 *      Sends an XML Response command from the Responder.
 *
 * Requires:
 *     BIP_NUM_RESPONDERS set to 1 or higher
 *
 * Parameters:
 *      server - Pointer to BipResponderSession server object.
 *      data - Pointer to response initialized BipData structure.
 *
 * Returns:
 *      OB_STATUS_SUCCESS - The response was sent.
 *
 *      OB_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
ObStatus BIP_SendResponse(BipObexServer *server, BipData *data);

/*---------------------------------------------------------------------------
 * BIP_ResponderAccept()
 *
 *      Indicates Responder accepts the current operation. The accept must
 *      include a BipData pointer for PutImage and PutLinkedThumbnail
 *      operations. The "code" parameter may be specified as
 *      OBRC_PARTIAL_CONTENT when accepting PutImage operations. This
 *      indicates that the Initiator must supply the Linked Thumbnail in
 *      the operation immediately following the PutImage operation.
 *
 * Requires:
 *     BIP_NUM_RESPONDERS set to 1 or higher
 *
 * Parameters:
 *      server - Pointer to Responder BipObexServer object indicated in
 *               event.
 *      code - ObexRespCode indicating accept or abort reason.
 *      data - Pointer to BipData structure with handle set for receiving
 *             object on PutImage and PutLinkedThumbnail operations.
 *
 */
void BIP_ResponderAccept(BipObexServer *server, ObexRespCode code, BipData *data);
#define BIP_ResponderAccept(s, c, d) (BIP_ServerAccept(s, c, d))

/*---------------------------------------------------------------------------
 * BIP_ResponderContinue()
 *
 *      Indicates server is ready to continue the current operation.
 *      Useful for flow control.
 *
 * Requires:
 *     BIP_NUM_RESPONDERS set to 1 or higher
 *
 * Parameters:
 *      server - Pointer to Responder BipObexServer object indicated in
 *               event.
 *
 */
void BIP_ResponderContinue(BipObexServer *server);
#define BIP_ResponderContinue(s) (BIP_ServerContinue(s))
#endif /* BIP_NUM_RESPONDERS */

#endif /* __BIP_H */
