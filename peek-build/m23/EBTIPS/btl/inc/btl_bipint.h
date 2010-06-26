/*******************************************************************************\
##                                                                             *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION          *
##                                                                             *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE        *
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##                                                                             *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_bipint.h
*
*   BRIEF:          This file defines the API of the BTL Basic Image Profile
*                   for the Initiator role.
*
*   DESCRIPTION:    General
*
*                   This module only represents the Initiator (BIPINT) role,
*                   which is the client type of role (initiating a connection)
*                   in the BIP profile.
*                   The Responder (BIPRSP) is defined in btl_biprsp.h.
*                   For a better understanding of the API, the functions
*                   can be divided into 4 sections.
*
*                   1. Management functions:
*                   These are the functions that can be found in all/most of
*                   the BTL modules as a consistent API:
*                   - BTL_BIPINT_Init
*                   - BTL_BIPINT_Deinit
*                   - BTL_BIPINT_Create
*                   - BTL_BIPINT_Destroy
*                   - BTL_BIPINT_Enable
*                   - BTL_BIPINT_Disable
*                   - BTL_BIPINT_Connect
*                   - BTL_BIPINT_Disconnect
*                   - BTL_BIPINT_GetConnectedDevice
*                   - BTL_BIPINT_SetSecurityLevel
*                   - BTL_BIPINT_GetSecurityLevel
*                   The remaining functions are more specific for BIPINT.
*
*                   2. PUT functions:
*                   The Initiator can send images to the remote Responder
*                   via the PUT functions in the API (BTL_BIPINT_Put***)
*                   - BTL_BIPINT_PutImage:
*                   - BTL_BIPINT_PutLinkedThumbnail
*
*                   3. GET functions:
*                   The Initiator can get images and imaging information from
*                   the remote Responder via the GET functions in the API
*                   (BTL_BIPINT_Get***):
*                   - BTL_BIPINT_GetCapabilities
*                   - BTL_BIPINT_GetImagesList
*                   - BTL_BIPINT_GetImageProperties
*                   - BTL_BIPINT_GetImage
*                   - BTL_BIPINT_GetLinkedThumbnail
*                   - BTL_BIPINT_GetMonitoringImage
*
*                   4. Parse functions
*                   The information that is retrieved from the Responder via
*                   the GET functions can be encoded in XML format. This API
*                   provide functionality to make this data easy accessible  by
*                   parsing (translating) the Response in data structures.
*                   These data structures do have corresponding MASK
*                   definitions to indicate the presence/absence of optional
*                   elements in the parsed Response data.
*                   This is applicable for the response on the
*                   'BTL_BIPINT_GetCapabilities',
*                   'BTL_BIPINT_GetImageProperties' and
*                   'BTL_BIPINT_GetImagesList':
*                   - BTL_BIPINT_ParseGetCapabilitiesResponse
*                   - BTL_BIPINT_ParseGetImagePropertiesResponse
*                   - BTL_BIPINT_ParseGetImagesListResponse
*                   - BTL_BIPINT_ParseNextFromImagesList
*
*                   Further is the a BTL_BIPINT_Abort function defines to
*                   be able to abort an OBEX exchange session. E.g. when it
*                   takes too much time for the user.
*
*                   An overview of the corresponding events is described in
*                   detail via the 'BtlBipintEvent' structure definition.
*
*                   Restrictions/limitations:
*                   There are 6 features defined with the BIP profile, where
*                   this module does support 3 of them:
*                   - Image Push
*                   - Image Pull
*                   - Remote Camera
*                   The other 3 features are not supported:
*                   - Advanced Image Printing
*                   - Automatic Camera
*                   - Remote Display
*                   
*   AUTHOR:         Gerrit Slot
*
\*******************************************************************************/

#ifndef __BTL_BIPINT_H
#define __BTL_BIPINT_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h" /* for types such as U8, U16, S8, S16,... */
#include "btl_common.h"
#include "btl_unicode.h"
#include "bip.h"
#include "btl_bip_common.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * Define the memory size, used for parsing the received object data:
 * - Imaging Capabilities
 * - Imaging Properties
 */
/* Max number of Image Format elements in an Imaging Capabilities Object */
#define BTL_BIPINT_MAX_NUM_OF_IMAGE_FORMATS 5
/* Max number of Attachment Format elements in an Imaging Capabilities Object */
#define BTL_BIPINT_MAX_NUM_OF_ATTACHMENT_FORMATS 2
/* Max number of Variant Encoding elements in an Imaging Properties Object */
#define BTL_BIPINT_MAX_NUM_OF_VARIANT_ENCODINGS 5
/* Max number of Attachment elements in an Imaging Properties Object */
#define BTL_BIPINT_MAX_NUM_OF_ATTACHMENTS 2
/* Max size (bytes) of a friendly name to be parsed from ImageProperties response */
#define BTL_BIPINT_MAX_SIZE_FRIENDLY_NAME 40

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
/*
 * Event structure as this module will send it to the registered callback
 *  function in the Initiator APP
 */
typedef struct _BtlBipintEvent     BtlBipintEvent;
/* Unique identification of a Initiator APP context. */
typedef struct _BtlBipintContext   BtlBipintContext;

/*-------------------------------------------------------------------------------
 * BtlBipintCallBack type
 *
 *   A function of this type is called to indicate BTL BIP events.
 */
typedef void (*BtlBipintCallBack)(const BtlBipintEvent *event);

/*---------------------------------------------------------------------------
 * BtlBipintCapabilitiesElementsMask type
 *
 *	This mask is used to indicate which capabilities elements are present
 *  in the response to a BTL_BIPINT_GetCapabilities request
 *  Used in BTL_BIPINT_ParseGetCapabilitiesResponse.
 */
typedef U32 BtlBipintCapabilitiesElementsMask;

#define	BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_PREFERRED_FORMAT     ((U32) 0x00000001UL)
#define	BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_IMAGE_FORMATS        ((U32) 0x00000002UL)
#define	BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_ATTACHMENT_FORMATS   ((U32) 0x00000004UL)
#define	BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_FILTERING_PARAMETERS ((U32) 0x00000008UL)
#define	BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_ALL	                ((U32) 0xFFFFFFFFUL)

/*---------------------------------------------------------------------------
 * BtlBipintImgPropertiesElementsMask type
 *
 *	This mask is used to indicate which image properties elements are present
 *  in the response to a BTL_BIPINT_GetImageProperties request
 *  Used in BTL_BIPINT_ParseGetImagePropertiesResponse.
 */
typedef U32 BtlBipintImgPropertiesElementsMask;

#define	BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_NATIVE_ENCODING  ((U32) 0x00000001UL)
#define	BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_VARIANT_ENCODING ((U32) 0x00000002UL)
#define	BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_ATTACHMENT       ((U32) 0x00000004UL)
#define	BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_ALL	            ((U32) 0xFFFFFFFFUL)

/*---------------------------------------------------------------------------
 * BtlBipintImagesListElementsMask type
 *
 *	This mask is used to indicate which images list elements are present
 *  in the response to a BTL_BIPINT_GetImagesList request
 *  Used in BTL_BIPINT_ParseGetImagesListResponse +
 *  BTL_BIPINT_ParseNextFromImagesList
 */
typedef U32 BtlBipintImagesListElementsMask;

#define	BTL_BIPINT_IMG_LIST_ELEMENT_MASK_FILTER  ((U32) 0x00000001UL)
#define	BTL_BIPINT_IMG_LIST_ELEMENT_MASK_LIST    ((U32) 0x00000002UL)
#define	BTL_BIPINT_IMG_LIST_ELEMENT_MASK_ALL	   ((U32) 0xFFFFFFFFUL)

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBipintEvent structure
 *
 *   Represents BTL BIP event for an Initiator.
 *   This event contains some fields that need to be read in order to detect
 *   the exact event:
 *   - event    = parms->event          : event type
 *   - bipOp    = parms->data->bipOp    : OBEX operation
 *   - status   = parms->status         : ObexRespCode in case of a problem.
 *   - dataType = parms->data->dataType : What kind of data is received.
 *   The 'event' is always present and should be interpreted first.
 *   The other fields are dependent on the 'event' value. These dependencies
 *   are summarized below.
 *
 *   events:
 *   - BIPINITIATOR_DATA_IND: Data received as result of a request from
 *     the Initiator. Trigger is mentioned between brackets. This trigger
 *     can also be detected from the ''bipOp' field.
 *     dataType:
 *     - BIPIND_IMAGE                 (trigger: BTL_BIPINT_GetImage)
 *     - BIPIND_THUMBNAIL             (trigger: BTL_BIPINT_GetLinkedThumbnail)
 *     - BIPIND_IMG_CAPABILITIES_XML  (trigger: BTL_BIPINT_GetCapabilities)
 *     - BIPIND_IMG_LISTING_XML       (trigger: BTL_BIPINT_GetImagesList)
 *     - BIPIND_IMG_PROPERTIES_XML    (trigger: BTL_BIPINT_GetImageProperties)
 *     - BIPIND_HANDLES_DESCRIPTOR    (trigger: BTL_BIPINT_??)
 *     - BIPIND_IMAGE_DESCRIPTOR      (trigger: BTL_BIPINT_??)
 *     - BIPIND_IMAGE_HANDLE          (trigger: BTL_BIPINT_PutImage)
 *
 *   - BIPINITIATOR_COMPLETE: Remote Responder is finished with the
 *     OBEX operation ('bipOp').
 *     bipOp:
 *     - BIPOP_GET_CAPABILITIES
 *     - BIPOP_PUT_IMAGE
 *     - BIPOP_PUT_LINKED_THUMBNAIL
 *     - BIPOP_GET_IMAGES_LIST
 *     - BIPOP_GET_IMAGE_PROPERTIES
 *     - BIPOP_GET_IMAGE
 *     - BIPOP_GET_LINKED_THUMBNAIL
 *     - BIPOP_GET_MONITORING_IMAGE
 *
 *   - BIPINITIATOR_FAILED: (OBEX) operation failed.
 *     The 'bipOp' indicates which operation did fail.
 *     The 'status' does contain the reason (ObexRespCode) for this failure.
 *
 *   - BIPINITIATOR_ABORT: (OBEX) operation aborted
 *     The 'bipOp' indicates which operation did abort.
 *     The 'status' does contain the reason (ObexRespCode) for this abort.
 *
 *   - BIPINITIATOR_CONNECTED: Connection is established
 *
 *   - BIPINITIATOR_DISCONNECTED: Connection is released
 *
 *   - BIPINITIATOR_DISABLED: Disable has finished (in case there was still
 *     a connection present when calling BTL_BIPINT_Disable)
 */
struct _BtlBipintEvent
{
    /* Associated Initiator context */
    BtlBipintContext *context;
    /* Event data from the stack. */
    BipCallbackParms *parms;
};


/*-------------------------------------------------------------------------------
 * BtlBipintCapabilitiesElements structure
 *
 *   Parsed elements in the imaging-capabilities as received from the remote
 *   Responder. See also 'Imaging-Capabilities Object' chapter in
 *   Bluetooth BIP spec.)
 */
typedef struct _BtlBipintCapabilitiesElements
{
    /* preferred format */
    BtlBipPreferredFormat     preferredFormat;
    
    /* List of image formats */
    U8							          numImageFormats; /* Number of filled elements in next table. */
    BtlBipImageFormats        imageFormats[BTL_BIPINT_MAX_NUM_OF_IMAGE_FORMATS];
    BOOL                      imageFormatsOverflow; /* TRUE = there were more ImageFormats */
                                                      /*  that do not fit in table above.    */
    /* List of attachment formats */
    U8						          	numAttachmentFormats; /* Number of filled elements in next table. */
    BtlBipAttachmentFormats   attachmentFormats[BTL_BIPINT_MAX_NUM_OF_ATTACHMENT_FORMATS];
    BOOL                      attachmentFormatsOverflow; /* TRUE = there were more AttachmentFormats */
                                                            /*  that do not fit in table above.         */
    /* filtering parameters */
    BtlBipFilteringParameters filteringParameters;
    /* DPOF options (not used; only used for the Advanced Image Printing feature) */
} BtlBipintCapabilitiesElements;

/*-------------------------------------------------------------------------------
 * BtlBipintImgPropertiesElements structure
 *
 *   Parsed elements in the Image-Properties as received from the remote
 *   Responder. See also 'Imaging-Properties Object' chapter in
 *   Bluetooth BIP spec.)
 */
typedef struct _BtlBipintImgPropertiesElements
{
    BipImgHandle          imgHandle;
    BtlUtf8               friendlyName[BTL_BIPINT_MAX_SIZE_FRIENDLY_NAME];
    /* native encoding */
    BtlBipNativeEncoding  nativeEncoding;
    
    /* List of variant encodings */
    U8							      numVariantEncodings; /* Number of filled elements in next table. */
    BtlBipVariantEncoding variantEncodings[BTL_BIPINT_MAX_NUM_OF_VARIANT_ENCODINGS];
    BOOL                  variantEncodingOverflow; /* TRUE = there were more variantEncodings */
                                                   /*  that do not fit in table above.        */
    /* List of attachments for this image */
    U8						       	numAttachments; /* Number of filled elements in next table. */
    BtlBipAttachment      attachments[BTL_BIPINT_MAX_NUM_OF_ATTACHMENTS];
    BOOL                  attachmentsOverflow; /* TRUE = there were more Attachments */
                                               /*  that do not fit in table above.   */
} BtlBipintImgPropertiesElements;


/*-------------------------------------------------------------------------------
 * BtlBipImagesListing structure
 *
 *     Represents a BTL BIP Images-listing object as defined
 *     in chapter 'Images-Listing Object' of the BIP spec.
 */
typedef struct _BtlBipImagesListing
{
    BipImgHandle            imgHandle;
    BtlDateAndTimeStruct created;
    BtlDateAndTimeStruct modified;
} BtlBipImagesListing;

/*-------------------------------------------------------------------------------
 * _BtlBipintImagesListMetaData structure
 *
 *   Parsed elements in the ImagesList as received from the remote
 *   Responder. See also 'Image Handles Descriptor' +
 *   'Images-Listing Object' chapters in
 *   Bluetooth BIP spec.)
 */
typedef struct _BtlBipintImagesListMetaData
{
    /* Applied filter for this list */
    BtlBipFilteringParameters filter;
    /* the offset to the XML imagelist object, received from parsing of response GetImageList */
    S8 *xmlImgListObjectOffset; 
    /* Number of images handles in the images list */
    U16 nbReturnedHandles;
} BtlBipintImagesListMetaData;

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Init()
 *
 * Brief:
 *      Initialize the BIP Initiator module.
 *
 * Description:
 *      Initialize the BIP Initiator module.
 *
 *  Type:
 *      Synchronous
 *
 * Parameters:
 *      void
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 */
BtStatus BTL_BIPINT_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Deinit()
 *
 * Brief:
 *      Deinitialize the BIP Initiator module.
 *
 * Description:
 *      Deinitialize the BIP Initiator module.
 *      When calling this function, there should be no context available anymore;
 *      they should have been destroyed before.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      void
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - There is still a BIP Initiator context created.
 *          It should be destroyed before.
 */
BtStatus BTL_BIPINT_Deinit(void);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Create()
 *
 * Brief:
 *      Allocate a unique BIP Initiator context.
 *
 * Description:
 *      Allocate a unique BIP Initiator context.
 *      This function must be called before any other BIP API function.
 *      The allocated context should be supplied in subsequent BIP API calls.
 *      The caller must also provide a callback function, which will be called
 *      on BIP events.
 *      The caller can provide an application handle (previously allocated
 *      with BTL_RegisterApp), in order to link between different modules.
 *      If there is no need to link between different modules, set appHandle
 *      to NULL.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      appHandle [in] - application handle (NULL.= not relevant)
 *
 *      bipCallback [in] - all BIP events will be sent to this callback.
 *
 *      securityLevel [in] - level of security which should be applied, when
 *          connecting to a remote BIP Responder.
 *          NULL = default value (BSL_DEFAULT)
 *
 *      bipContext [out] - allocated BIP context.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - BIP context was created successfully.
 *
 *      BT_STATUS_NO_RESOURCES - No more resources.
 */
BtStatus BTL_BIPINT_Create(BtlAppHandle       *appHandle,
                           BtlBipintCallBack   bipCallback,
                           BtSecurityLevel    *securityLevel,
                           BtlBipintContext  **bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Destroy()
 *
 * Brief:
 *      Release a BIP Initiator context
 *
 * Description:
 *      Release a BIP Initiator context (previously allocated with
 *      BTL_BIPINT_Create). An application should call this function when it
 *      completes using BIP services.
 *      Before calling this function, the BIP Initiator context should have
 *      been disabled.
 *      Upon completion, the BIP context is set to NULL in order to prevent
 *      the application from an illegal attempt to keep using it.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in/out] - Identification of the BIP Initiator context.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - BIP context was destroyed successfully.
 *
 *		  BT_STATUS_IN_USE - BIP context is still in use. It should
 *          be disabled first.
 */
BtStatus BTL_BIPINT_Destroy(BtlBipintContext **bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Enable()
 *
 * Brief:
 *      Enable BIP Initiator
 *
 * Description:
 *		  Enable BIP Initiator, called after BTL_BIPINT_Create.
 *		  After calling this function, BIP Initiator is ready for usage.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_IMPROPER_STATE - This 'context' was already in use (enabled)
 *
 *      BT_STATUS_FAILED  - The operation failed.
 */
BtStatus BTL_BIPINT_Enable(BtlBipintContext *bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Disable()
 *
 * Brief:
 *      Disable BIP Initiator.
 *
 * Description:
 *      Disable BIP Initiator. If a connection exists, it will be
 *      automatically disconnected. This function is normally only used before
 *      calling BTL_BIPINT_Destroy.
 *
 * Type:
 *      Asynchronous/Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 * Generated Events:
 *      BIPINITIATOR_DISCONNECTED
 *      BIPINITIATOR_DISABLED
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - The link is connected. The disconnect process has
 *          been successfully started. When the connection is removed,
 *          the normal BIPINITIATOR_DISCONNECTED event will be given for that
 *          connection, followed by the BIPINITIATOR_DISABLED event,
 *          indicating that this BIP Initiator context is disabled.
 */
BtStatus BTL_BIPINT_Disable(BtlBipintContext *bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_SetSecurityLevel()
 *
 * Brief:
 *      Update the security level for this BIP Initiator.
 *
 * Description:
 *      Update the security level for this BIP Initiator.
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      securityLevel [in] - level of security which should be applied, when
 *          connecting to a remote BIP server.
 *          NULL = default value (BSL_DEFAULT).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPINT_SetSecurityLevel(BtlBipintContext      *bipContext,
                                     const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetSecurityLevel()
 *
 * Brief:
 *      Return the actual security level for this BIP Initiator
 *
 * Description:
 *      Return the actual security level for this BIP Initiator
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      securityLevel [out] - the current security level
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPINT_GetSecurityLevel(BtlBipintContext *bipContext,
                                     BtSecurityLevel  *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetConnectedDevice()
 *
 * Brief:
 *      Return the connected device for this BIP Initiator.
 *
 * Description:
 *      Return the connected device for this BIP Initiator.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *      BT_STATUS_SUCCESS  - Operation is successful.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *      BT_STATUS_NO_CONNECTION - The channel is not connected.
 */
BtStatus BTL_BIPINT_GetConnectedDevice(BtlBipintContext *bipContext,
                                       BD_ADDR          *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Connect()
 *
 * Brief:
 *      Create a transport connection for this BIP Initiator
 *
 * Description:
 *      Create a transport connection for this BIP Initiator with the
 *      Responder on the remote device ('bdAddr').
 *      This Responder should support the indicated 'service'.
 *
 * Type:
 *      Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      bdAddr [in] - BD Address of the remote device.
 *
 *      service [in] - Requested supported BIP features by the remote Responder.
 *          Only the following values are supported:
 *          BIPSVC_RESPONDER =  responder should support ImagePush, ImagePull or
 *              RemoteCamera feature
 *          BIPSVC_REMOTE_CAMERA = responder should support RemoteCamera feature
 *              only.
 *
 * Generated Events:
 *      BIPINITIATOR_COMPLETE: succeeded
 *      BIPINITIATOR_FAILED: failed
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_BUSY    - Operation was not started because an other
 *          operation is pending.
 */
BtStatus BTL_BIPINT_Connect(BtlBipintContext *bipContext,
                            BD_ADDR          *bdAddr,
                            BipService        service);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Disconnect()
 *
 * Brief:
 *      Disconnect this BIP Initiator connection.
 *
 * Description:
 *      Disconnect this BIP Initiator connection.
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 * Generated Events:
 *      BIPINITIATOR_DISCONNECTED: succeeded
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application (event = BIPINITIATOR_DISCONNECTED)
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there was no connection.
 *
 *      BT_STATUS_FAILED: Disconnection failed due to an internal problem.
 *
 */
BtStatus BTL_BIPINT_Disconnect(BtlBipintContext *bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_PutImage()
 *
 * Brief:
 *      Send an image to the remote Responder
 *
 * Description:
 *      Send an image to the remote Responder that is connected to this
 *      BIP Initiator ('bipContext').
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      encoding [in] - Encoding of the image (E.g. JPEG or PNG)
 *
 *      width [in] - width in pixels of the image
 *
 *      height [in] - height in pixels of the image
 *
 *      size [in] - size of the image in bytes
 *
 *      transformation [in] - Indicate if and how this images was
 *          transformed.
 *          BTL_BIP_TRANSFORMATION_NONE = no transformation.
 *
 *      objToSend [in] - Image data, either stored in memory or on
 *          the file system. When stored on the file system, the
 *          objToSend->objectName should contain the file-name and
 *          the 'fsPath' parameter should contain the full
 *          path name.
 *
 * Generated Events:
 *      BIPINITIATOR_COMPLETE
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *          - event = BIPINITIATOR_COMPLETE
 *          - bipOp = BIPOP_PUT_IMAGE
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_BIPINT_PutImage(BtlBipintContext    *bipContext,
                             BtlBipEncoding       encoding,
                             U16                  width,
                             U16                  height,
                             U32                  size,
                             BtlBipTranformation  transformation,
                             BtlObject           *objToSend);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_PutLinkedThumbnail()
 *
 * Brief:
 *      Send a thumbnail to the remote Responder
 *
 * Description:
 *      Send a thumbnail to the remote Responder that is connected to this
 *      BIP Initiator ('bipContext').
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      imgHandle [out] - 0-terminated string, identifying the image.
 *
 *      objToSend [in] - Image data, either stored in memory or on
 *          the file system. When stored on the file system, the
 *          objToSend->objectName should contain the file-name and
 *          the 'fsPath' parameter should contain the full
 *          path name.
 *      objToSend [in] - Image data.
 *
 * Generated Events:
 *      BIPINITIATOR_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *          - event = BIPINITIATOR_COMPLETE
 *          - bipOp = BIPOP_PUT_LINKED_THUMBNAIL
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_BIPINT_PutLinkedThumbnail(BtlBipintContext *bipContext,
                                       BipImgHandle      imgHandle,
                                       BtlObject        *objToSend);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetCapabilities()
 *
 * Brief:
 *      Retrieves the image capabilities from the remote Responder
 *
 * Description:
 *      Retrieves the image capabilities from the remote Responder that is
 *      connected to this BIP Initiator ('bipContext').
 *
 *      When the requested info is received, the 'Initiator' will be notified
 *      with the following event:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMG_CAPABILITIES_XML
 *
 *      This operation is finished when the following event sent to the
 *      'Initiator':
 *      - event = BIPINITIATOR_COMPLETE
 *      - bipOp = BIPOP_GET_CAPABILITIES
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      responseObj [in] - Pointer to a Object indicating where to receive
 *          the capabilities. These capabilities can be explored further via
 *          BTL_BIPINT_ParseGetCapabilitiesResponse.
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters detected.
 *
 */
BtStatus BTL_BIPINT_GetCapabilities(BtlBipintContext *bipContext,
                                    BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetImagesList()
 *
 * Brief:
 *      Retrieve an images list from the remote Responder
 *
 * Description:
 *      Retrieve an images list from the remote Responder that is
 *      connected to this BIP Initiator ('bipContext').
 *      It is possible to retrieve parts of this images list or a filtered
 *      list.
 *
 *      When the requested info is received, the 'Initiator' will be notified
 *      with the following event:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMG_LISTING_XML
 *
 *      This operation is finished when the following event sent to the
 *      'Initiator':
 *      - event = BIPINITIATOR_COMPLETE
 *      - bipOp = BIPIND_IMG_LISTING_XML
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      filter [in] - Filter parameters for the ImageList to be retrieved.
 *          (See .BtlBipFilter type description for details on the
 *           individual parameters).
 *          NULL = don't care (no filtering done).
 *
 *      latestCapturedImages [in] -
 *          TRUE = list only the latest captured images,
 *          FALSE = list all images.
 *
 *      listStartOffset [in] - Zero based offset from the beginning of the
 *          ImagesList where the Responder should start to return the list
 *          of images.
 *
 *      nbReturnedHandles [in] - Maximum number of returned handles [0..65535]
 *          0 = ImagesList response will only contain the number of images that
 *          would be included without providing the list itself
 *
 *      responseObj [in] - Pointer to a Object indicating where to receive
 *          the ImagesList.This ImagesList can be explored further via
 *          BTL_BIPINT_UTIL_GetHandleFromImagesListDescriptor.
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_BIPINT_GetImagesList(BtlBipintContext *bipContext,
                                  BtlBipFilter     *filter,
                                  BOOL              latestCapturedImages,
                                  U16               listStartOffset,
                                  U16               nbReturnedHandles,
                                  BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetImageProperties()
 *
 * Brief:
 *      Retrieve the image properties of a specific image
 *
 * Description:
 *      Retrieve the image properties of a specific image ('imgHandle') from
 *      the remote Responder that is connected to this BIP Initiator ('bipContext').
 *
 *      When the requested info is received, the 'Initiator' will be notified
 *      with the following event:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMG_PROPERTIES_XML
 *
 *      This operation is finished when the following event sent to the
 *      'Initiator':
 *      - event = BIPINITIATOR_COMPLETE
 *      - bipOp = BIPIND_IMG_PROPERTIES_XML
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      imgHandle [out] - 0-terminated string, identifying the image.
 *
 *      responseObj [in] - Pointer to a Object indicating where to receive the
 *          image properties. These properties can be explored further via
 *          BTL_BIPINT_ParseGetImagePropertiesResponse.
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *          BIPINITIATOR_COMPLETE - The server operation has completed
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_BIPINT_GetImageProperties(BtlBipintContext *bipContext,
                                       BipImgHandle      imgHandle,
                                       BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetImage()
 *
 * Brief:
 *      Retrieves a specific image from the remote Responder.
 *
 * Description:
 *      Retrieves a specific image ('imgHandle') from the remote Responder
 *      that is connected to this BIP Initiator ('bipContext').
 *      The image properties ('encoding', 'pixelRange', 'maxSize',
 *      'transformation') can be set as well, but should be in alignment with
 *      the properties as they can be retrieved before with
 *      BTL_BIPINT_GetImageProperties.
 *
 *      The image will be returned with the following event:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMAGE
 *
 *      The end of the GetImage process is indicated with the following event:
 *      - event = BIPINITIATOR_COMPLETE
 *      - bipOp = BIPOP_GET_IMAGE
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      imgHandle [out] - 0-terminated string, identifying the image.
 *
 *      encoding [in] - Encoding of the image (JPEG, PNG, etc.)
 *          See BTL_BIP_ENCODING_*** definitions
 *
 *      pixelRange [in] - Range of acceptable picture sizes, expressed in pixels.
 *          See 'BtlBipPixel' description fro details on how to define a range or
 *          a range with fixed aspect ratio. When this parameter is filled
 *          (not NULL), it should contain a range; a fixed size is not allowed.
 *          If NULL, all sizes are allowed.
 *
 *      maxSize [in] - The maximum size of the image in bytes.
 *          0 = don't care (any size).
 *
 *      transformation [in] - Indicate if and how the image should be transformed.
 *          BTL_BIP_TRANSFORMATION_NONE = don't care
 *
 *      responseObj [in] - Object memory, where to store the responded image.
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BIPINT_GetImage(BtlBipintContext    *bipContext,
                             BipImgHandle         imgHandle,
                             BtlBipEncoding       encoding,
                             BtlBipPixel         *pixelRange,
                             U32                  maxSize,
                             BtlBipTranformation  transformation,
                             BtlObject           *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetLinkedThumbnail()
 *
 * Brief:
 *      Retrieve a specific thumbnail from the remote Responder.
 *
 * Description:
 *      Retrieve a specific thumbnail ('imgHandle') from the remote Responder
 *      that is connected to this BIP Initiator ('bipContext').
 *
 *      The image will be returned with the following event:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMAGE
 *
 *      This operation is finished when the following event sent to the
 *      'Initiator':
 *      - event = BIPINITIATOR_COMPLETE
 *      - bipOp = BIPOP_GET_LINKED_THUMBNAIL
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      imgHandle [out] - 0-terminated string, identifying the image.
 *
 *      responseObj [in] - Pointer to an object indicating where to store the
 *          received image, either stored in memory or on
 *          the file system. When stored on the file system, the
 *          responseObj->objectName should contain the file-name and
 *          the 'fsPath' parameter should contain the full
 *          path name.
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_BIPINT_GetLinkedThumbnail(BtlBipintContext *bipContext,
                                       BipImgHandle      imgHandle,
                                       BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_GetMonitoringImage()
 *
 * Brief:
 *      Retrieves a monitor image from the remote Responder.
 *
 * Description:
 *      Retrieves a monitor image from the remote Responder
 *      that is connected to this BIP Initiator ('bipContext').
 *      With 'storeFlag' it is possible to force the Responder to make a picture
 *      (use the shutter) before sending this monitor image.
 *
 *      The image will be returned with the following event:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPOP_GET_MONITORING_IMAGE
 *
 *      This operation is finished when the following event sent to the
 *      'Initiator':
 *      - event = BIPINITIATOR_COMPLETE
 *      - bipOp = BIPOP_GET_MONITORING_IMAGE
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      storeFlag [in] -
 *          TRUE = The Responder will activate the shutter on the Responder
 *                 and make a picture that will be replied.
 *          FALSE = Get image on the monitor of the Responder.
 *
 *      responseObj [in] - Pointer to an object indicating where to store the
 *          received image, either stored in memory or on
 *          the file system. When stored on the file system, the
 *          responseObj->objectName should contain the file-name and
 *          the 'fsPath' parameter should contain the full
 *          path name.
 *
 * Generated Events:
 *      BIPINITIATOR_DATA_IND
 *      BIPINITIATOR_COMPLETE
 *
 * Returns:
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY  - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_BIPINT_GetMonitoringImage(BtlBipintContext *bipContext,
                                       BOOL              storeFlag,
                                       BtlObject        *responseObj);


#if OBEX_AUTHENTICATION == XA_ENABLED
/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ObexAuthenticationResponse()
 *
 * Brief:
 *      Accept or refuse an OBEX authentication challenge.
 *
 * Description:
 *		  This function must be called after receiving OBEX authentication
 *		  challenge (event = BIPINITIATOR_AUTH_CHALLENGE) from the Responder.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      userId [in] - 0-terminated string used with OBEX authentication
 *          representing the user id.
 *          Maximum length of the string is OBEX_MAX_USERID_LEN
 *          NULL = argument is not needed, as mentioned in the event.
 *
 *      password [in] - 0-terminated string used with OBEX authentication
 *          representing the password.
 *          Maximum length of the string is OBEX_MAX_PASSWORD_LEN
 *          NULL = deny authentication.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - The operation failed.
 *
 *      BT_STATUS_INVALID_PARM - In case of null context or password,
 *           or too long strings.
 */
BtStatus BTL_BIPINT_ObexAuthenticationResponse(BtlBipintContext *bipContext,
                                               const BtlUtf8	  *userId,
                                               const BtlUtf8	  *password);
#endif


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_Abort()
 *
 * Brief:
 *      Abort the current active operation to the remote Responder
 *
 * Description:
 *      Abort the current active operation to the remote Responder for this
 *      BIP Initiator ('bipContext')
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation was successful.
 *
 *      BT_STATUS_FAILED - Operation failed to start.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *           this Initiator is not connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BIPINT_Abort(BtlBipintContext *bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseGetCapabilitiesResponse()
 *
 * Brief:
 *      Parse an XML GetCapabilities response.
 *
 * Description:
 *      Parse an XML GetCapabilities response:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMG_CAPABILITIES_XML
 *
 *		  This function parses the response, and sets the available element values
 *		  in 'elementsData'.
 *
 *	  	The function returns an 'elementsMask' that indicates which values are
 *      present in 'elementsData'. Only those elements for which the
 *      corresponding bit is set in 'elementsMask' should be accessed in
 *      'elementsData'.
 *      The values of all other elements are undefined.
 *
 * Type:
 *		  Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *  		responseObj [in] - describes XML response information
 *
 *	  	elementsData [out] - Contains the values of the present elements.
 *
 *		  elementsMask [out] - Indicates which attributes in 'elementsData' are valid
 *
 * Returns:
 *		  BT_STATUS_SUCCESS - Operation is successful.
 *
 *  		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPINT_ParseGetCapabilitiesResponse(BtlBipintContext                  *bipContext,
                                                 const BtlObject                   *responseObj,
                                                 BtlBipintCapabilitiesElements     *elementsData,
                                                 BtlBipintCapabilitiesElementsMask *elementsMask);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseGetImagePropertiesResponse()
 *
 * Brief:
 *      Parse an XML GetImageProperties response.
 *
 * Description:
 *      Parse an XML GetImageProperties response:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMG_PROPERTIES_XML
 *
 *		  This function parses the response, and sets the available attribute
 *		  values in attributesData.
 *
 *  		The function returns an attributesMask that indicates which values are
 *	  	present in attributesData. Only those attributes for which the
 *		  corresponding bit is set in attributesMask should be accessed in
 *		  attributesData.
 *      The values of all other attributes is undefined.
 *
 * Type:
 *		  Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *  		responseObj [in] - describes XML response information
 *
 *	  	elementsData [out] - Contains the values of the present elements.
 *
 *		  elementsMask [out] - Indicates which attributes in 'elementsData' are valid
 *
 * Returns:
 *		  BT_STATUS_SUCCESS - Operation is successful.
 *
 *  		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPINT_ParseGetImagePropertiesResponse(BtlBipintContext                   *bipContext,
                                                    const BtlObject                    *responseObj,
                                                    BtlBipintImgPropertiesElements     *elementsData,
                                                    BtlBipintImgPropertiesElementsMask *elementsMask);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseGetImagesListResponse()
 *
 * Brief:
 *      Parse an XML GetImagesList response.
 *
 * Description:
 *      Parse an XML GetImagesList response:
 *      - event    = BIPINITIATOR_DATA_IND
 *      - dataType = BIPIND_IMG_LISTING_XML
 *
 *      This function parses the response, and sets the available attribute
 *      values in attributesData.
 *
 *      The function returns an attributesMask that indicates which values 
 *	  	are present in attributesData. Only those attributes for which the
 *      corresponding bit is set in attributesMask should be accessed 
 *      in attributesData.
 *      The values of all other attributes is undefined.
 *
 *      ATTENTION points:
 *      - It is possible that the Response does not contain the list itself, but
 *      only the amount of images (nbReturnedHandles) in the list, because 
 *      that was requested before with BTL_BIPINT_GetImagesList.
 *      - It is possible that the complete list does not fit in the 'elementsData'
 *      This can be detected because the table in this 'elementsData' is full.
 *      (see 'BtlBipintImagesListElements' for details).
 *      In this case, the next part of the list can be parsed via
 *      BTL_BIPINT_ParseNextFromImagesList.
 *
 *      RESTRICTION : The response to a GetImageList may contain supported
 *      Filteringparameters from the responder.
 *      This is not supported by the ESI stack to pass to the upper layer.
 *      Therefore, the mask for filtering is not set. 
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Initiator context.
 *
 *      responseObj [in] - describes XML response information
 *
 *	  	elementsMetaData [out] - Contains the meta data about the list.
 *
 *      elementsMask [out] - Indicates which attributes in 'elementsData' are valid
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPINT_ParseGetImagesListResponse(BtlBipintContext                *bipContext,
                                               const BtlObject                 *responseObj,
                                               BtlBipintImagesListMetaData     *elementsMetaData,
                                               BtlBipintImagesListElementsMask *elementsMask);


/*-------------------------------------------------------------------------------
 * BTL_BIPINT_ParseNextFromImagesList()
 *
 * Brief:
 *      Parse the next elements from the Images List as it was received before
 *
 * Description:
 *      Parse the next elements from the Images List as it was received before.
 *      This function must be preceded by BTL_BIPINT_ParseGetImagesListResponse
 *      That function did provide all the necessary data ('responseObj' input)
 *      for a next parsing.
 *      The result of the parsing is found in the 'imageInfo' argument
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      elementsMetaData [in] - contains a parameter with the offset to the
 *          XML imagelist object
 *
 *	    imageInfo [out] - Structure to hold the next image data
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *	    BT_STATUS_NOT_FOUND - There are no more images in the list.
 *
 *      BT_STATUS_FAILED - The operation failed.
 *
 */
BtStatus BTL_BIPINT_ParseNextFromImagesList(BtlBipintImagesListMetaData *elementsMetaData,
                                            BtlBipImagesListing         *imageInfo);

#endif /* __BTL_BIPINT_H */

