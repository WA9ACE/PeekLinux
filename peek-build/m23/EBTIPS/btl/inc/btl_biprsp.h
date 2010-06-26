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
*   FILE NAME:      btl_biprsp.h
*
*   BRIEF:          This file defines the API of the BTL Basic Image Profile
*                   for the Responder role.
*
*   DESCRIPTION:    General
*
*                   This module only represents the Responder (BIPRSP) role,
*                   which is the server type of role (accepting a connection)
*                   in the BIP profile.
*                   The Initiator (BIPINT) is defined in btl_bipint.h.
*                   For a better understanding of the API, the functions
*                   can be divided into 4 sections.
*
*                   1. Management functions:
*                   These are the functions that can be found in all/most of
*                   the BTL modules as a consistent API:
*                   - BTL_BIPRSP_Init
*                   - BTL_BIPRSP_Deinit
*                   - BTL_BIPRSP_Create
*                   - BTL_BIPRSP_Destroy
*                   - BTL_BIPRSP_Enable
*                   - BTL_BIPRSP_Disable
*                   - BTL_BIPRSP_GetConnectedDevice
*                   - BTL_BIPRSP_SetSecurityLevel
*                   - BTL_BIPRSP_GetSecurityLevel
*                   The remaining functions are more specific for BIPRSP.
*
*                   2. Responder specific functions:
*                   Some extra Responmder specific management functions are
*                   added for convinience of the application:
*                   - BTL_BIPRSP_SetDefaultImagePath
*                   - BTL_BIPRSP_SetAutoRequestLinkedThumbnail
*                   - BTL_BIPRSP_GetAutoRequestLinkedThumbnail
*
*                   3. Respond functions:
*                   The Initiator can request several images information from
*                   this Responder via the GET functions taht will be received
*                   as events by this Responder. The Responder can handle
*                   these events by responding on it via the corresponding
*                   BTL_BIPRSP_RespondGet*** function:
*                   - BTL_BIPRSP_RespondGetCapabilities
*                   - BTL_BIPRSP_RespondGetImagesList
*                   - BTL_BIPRSP_RespondGetImageProperties
*                   - BTL_BIPRSP_RespondGetImage
*                   - BTL_BIPRSP_RespondGetLinkedThumbnail
*                   - BTL_BIPRSP_RespondGetMonitoringImage
*
*                   4. Build functions:
*                   Some responses need to be encode in XML format. This is
*                   applicable for:
*                   'BTL_BIPRSP_RespondGetCapabilities',
*                   'BTL_BIPRSP_RespondGetImageProperties' and
*                   'BTL_BIPRSP_RespondGetImagesList'
*                   This API does provide support functions to build these
*                   XML respondes. These functions all have the prefix
*                   BTL_BIPRSP_Build***:
*                   For these 3 responses,3 group of Build functions are
*                   provided:
*                   - BTL_BIPRSP_BuildCapabilities***
*                   - BTL_BIPRSP_BuildProperties***
*                   - BTL_BIPRSP_BuildListing***
*                   A build sequence always satrts with a
*                   BTL_BIPRSP_Build***Start function and ends with a
*                   BTL_BIPRSP_Build***Finish function where in between some
*                   BTL_BIPRSP_Build***Add*** functions can be called.
*                   Details of building a specific response is further given
*                   in the corresponding function descriptions.
*
*                   Further is the a BTL_BIPRSP_Abort function defines to
*                   be able to abort an OBEX exchange session. E.g. when it
*                   takes too much time for the user.
*
*                   An overview of the corresponding events is described in
*                   detail via the 'BtlBiprspEvent' structure definition.
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
*                   The number of connections that can be handled in parallel
*                   is limited to 1. So BIP_NUM_RESPONDERS must be set to 1.
*                   
*   AUTHOR:         Gerrit Slot
*
\*******************************************************************************/

#ifndef __BTL_BIPRSP_H
#define __BTL_BIPRSP_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h" /* for types such as U8, U16, S8, S16,... */
#include "btl_common.h"
#include "bip.h"
#include "btl_bip_common.h"
#include "btl_unicode.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
typedef struct _BtlBiprspEvent   BtlBiprspEvent;
typedef struct _BtlBiprspContext BtlBiprspContext;

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBipCallBack type
 *
 *     A function of this type is called to indicate BTL BIP events.
 */
typedef void (*BtlBiprspCallBack)(const BtlBiprspEvent *event);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBiprspEvent structure
 *
 *   Represents BTL BIP event for a Responder.
 *   This event contains some fields that need to be read in order to detect
 *   the exact event:
 *   - event  = parms->event       : event type
 *   - bipOp  = parms->data->bipOp : OBEX operation
 *   - status = parms->status      : ObexRespCode in case of a problem.
 *   The 'event' is always present and should be interpreted first.
 *   The other fields are dependent on the 'event' value. These dependencies
 *   are summarized below.
 *
 *   events:
 *   - BIPRESPONDER_REQUEST: Remote Initiator wants something from the Responder
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
 *   - BIPRESPONDER_COMPLETE: (OBEX) operation completed.
 *     The 'bipOp' indicates which operation was completed
 *
 *   - BIPRESPONDER_FAILED: (OBEX) operation failed.
 *     The 'bipOp' indicates which operation did fail.
 *     The 'status' does contain the reason (ObexRespCode) for this failure.
 *
 *   - BIPRESPONDER_ABORT: (OBEX) operation aborted
 *     The 'bipOp' indicates which operation did abort.
 *     The 'status' does contain the reason (ObexRespCode) for this abort.
 *
 *   - BIPRESPONDER_CONNECTED: Connection is established
 *
 *   - BIPRESPONDER_DISCONNECTED: Connection is released
 *
 *   - BIPRESPONDER_DISABLED: Disable has finished (in case there was still
 *     a connection present when calling BTL_BIPRSP_Disable)
 *
 */
struct _BtlBiprspEvent 
{
    /* Associated Responder context */
    BtlBiprspContext *context;
    /* Event data from the stack.*/
    BipCallbackParms *parms;
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Init()
 *
 * Brief:
 *      Initialize the BIP Responder module.
 *
 * Description:
 *      Initialize the BIP Responder module.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      void
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 */
BtStatus BTL_BIPRSP_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Deinit()
 *
 * Brief:
 *      Initialize the BIP Responder module.
 *
 * Description:
 *      Deinitialize the BIP Responder module.
 *      When calling this function, there should be no context available anymore;
 *      they should have been destroyed before
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
BtStatus BTL_BIPRSP_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Create()
 *
 * Brief:
 *      Allocate a unique BIP Responder context.
 *
 * Description:
 *      Allocate a unique BIP Responder context.
 *      This function must be called before any other BIP API function.
 *      The allocated context should be supplied in subsequent BIP API calls.
 *      The caller must also provide a callback function, which will be called 
 *      on BIP events.
 *      The caller can provide an application handle (previously allocated 
 *      with BTL_RegisterApp), in order to link between different modules.
 *      If there is no need to link between different modules, set appHandle to NULL.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipCallback [in] - all BIP events will be sent to this callback.
 *
 *      securityLevel [in] - level of security which should be applied, when
 *          accepting a connection from a remote BIP Initiator.
 *          NULL = default value (BSL_DEFAULT)
 *      
 *      bipContext [out] - allocated BIP context.  
 *
 * Returns:
 *      BT_STATUS_SUCCESS - BIP context was created successfully.
 *
 *      BT_STATUS_NO_RESOURCES - No more resources.
 */
BtStatus BTL_BIPRSP_Create(BtlAppHandle           *appHandle,
                           BtlBiprspCallBack       bipCallback,
                           const BtSecurityLevel  *securityLevel,
                           BtlBiprspContext      **bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Destroy()
 *
 * Brief:
 *       Release a BIP Respondercontext.
 *
 * Description:
 *      Release a BIP Respondercontext (previously allocated with BTL_BIPRSP_Create).
 *      An application should call this function when it completes using BIP services.
 *      Before calling this function, the BIP context should have been disabled.
 *      Upon completion, the BIP context is set to NULL in order to prevent 
 *      the application from an illegal attempt to keep using it.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in/out] - Identification of the BIP Responder context.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - BIP context was destroyed successfully.
 *
 *		  BT_STATUS_IN_USE - BIP context is still in use. It should
 *          be disabled first. 
 */
BtStatus BTL_BIPRSP_Destroy(BtlBiprspContext **bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Enable()
 *
 * Brief:
 *      Enable BIP Responder and publish an SDP record.
 *
 * Description:
 *      Enable BIP Responder. It will add an SDP service record, so that this
 *      'service' is detectable by remote BIP Initiator devices.
 *      After calling this function, BIP Responder is ready for usage.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      serviceName [in] - 0-terminated string which will be written into 
 *          SDP service record and will be discovered by the peer device.
 *          If NULL is passed, an empty string is used.
 *			    If NULL is passed, the default name ("Imaging") is used.
 *
 *      service [in] - Supported BIP features by this Responder server.
 *          Only the following values are supported:
 *          - BIPSVC_IMAGE_PUSH = support for ImagePush feature only.
 *          - BIPSVC_IMAGE_PULL = support for ImagePull feature only.
 *          - BIPSVC_REMOTE_CAMERA = support for RemoteCamera feature only.
 *          - BIPSVC_RESPONDER =  support for ImagePush, ImagePull and
 *              RemoteCamera feature (combination of all 3 above)
 *              This is also the default value.
 *          - Any mix (OR function) of the features above.
 *          - NULL = default (BIPSVC_RESPONDER).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_IMPROPER_STATE - This 'context' was already in use (enabled)
 *
 *      BT_STATUS_FAILED  - The operation failed.
 */
BtStatus BTL_BIPRSP_Enable(BtlBiprspContext *bipContext, 
                           BtlUtf8          *serviceName,
                           BipService       *service);

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Disable()
 *
 * Brief:
 *      Disable BIP Responder and removes the publication via the SDP service.
 *
 * Description:
 *      Disable BIP Responder and removes the publication via the SDP service
 *      record. If a connection exists, it will NOT be automatically disconnected.
 *      RESTRICTION: The stack doe s not provide the functionality to disconnect
 *                   an OBEX channel for a Responder.
 *
 * Type:
 *      Asynchronous/Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 * Generated Events:
 *      BIPRESPONDER_DISCONNECTED
 *      BIPRESPONDER_DISABLED
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *          been successfully started. When the connection is removed,
 *          the normal BIPRESPONDER_DISCONNECTED event will be given for that
 *          connection, followed by the BIPRESPONDER_DISABLED event,
 *          indicating that this BIP Responder context is disabled.
 *
 *      BT_STATUS_FAILED - The operation failed, most likely because
 *          it is still connected.
 */
BtStatus BTL_BIPRSP_Disable(BtlBiprspContext *bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_SetSecurityLevel()
 *
 * Brief:
 *      Update the security level for this BIP Responder.
 *
 * Description:
 *      Update the security level for this BIP Responder.
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      securityLevel [in] - level of security which should be applied for
 *          incoming connection from a remote BIP Initiator.
 *          NULL = default value (BSL_DEFAULT).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPRSP_SetSecurityLevel(BtlBiprspContext *bipContext,
                                     BtSecurityLevel  *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_GetSecurityLevel()
 *
 * Brief:
 *      Return the actual security level for this BIP Responder.
 *
 * Description:
 *      Return the actual security level for this BIP Responder.
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      securityLevel [out] - the current security level
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BIPRSP_GetSecurityLevel(BtlBiprspContext *bipContext,
                                     BtSecurityLevel  *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_GetConnectedDevice()
 *
 * Brief:
 *      Return the connected device for this BIP Responder.
 *
 * Description:
 *      Return the connected device for this BIP Responder.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context,
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
BtStatus BTL_BIPRSP_GetConnectedDevice(BtlBiprspContext *bipContext, 
                                       BD_ADDR          *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_SetDefaultImagePath()
 *
 * Brief:
 *      Set the default path for providing images.
 *
 * Description:
 *
 *      This function sets the default path ('rootPath') in the file system
 *      for receiving images from teh remote Initiator.
 *      It is applicable for the following functions:
 *      - BTL_BIPRSP_RespondPutImage
 *      - BTL_BIPRSP_RespondPutLinkedThumbnail
 *
 *	===========================================================
 *		IMPRTANT NOTE:
 *			This function is DEPRECATED and should not be used anymore.
 *			As described above, it is used in conjunction with BTL_BIPRSP_RespondPutImage() and
 *			BTL_BIPRSP_RespondPutLinkedThumbnail() which are themselves deprecated.
 *	===========================================================
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context,
 *
 *      rootPath [in] - 0-terminated string, defining the default root in
 *          file-system for sending images.
 *          Max root path length is PATHMGR_MAX_PATH.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 */
BtStatus BTL_BIPRSP_SetDefaultImagePath(BtlBiprspContext *bipContext,
                                        BtlUtf8          *rootPath);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_SetAutoRequestLinkedThumbnail()
 *
 * Brief:
 *      Set or reset the feature for automatically requesting thumbnail images.
 *
 * Description:
 *      This function sets or resets the option to request automatically for
 *      a thumbnail after receiving a PutImage  from the remote Initiator.
 *      Automatic means: without notifying the Responder application.
 *      Typically set when this Responder has no possibilities  to create
 *      a thumbnail version of an image.
 *      By default, this option is disabled.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context,
 *
 *      enable [in] - Enable/disable this feature.
 *          TRUE  = This module will request a thumbnail after a Initiator PutImage.
 *                  So, after call the BTL_BIPRSP_RespondPutImage, automatically
 *                  the thumbnail version is requested as well.
 *                  The BIP Responder application will receive a
 *                  'put linked thumbnail' event later on.
 *          FALSE = This module will not request for a thumbnail, but pass this
 *                  'put image' event to the BIP Responder application.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 */
BtStatus BTL_BIPRSP_SetAutoRequestLinkedThumbnail(BtlBiprspContext *bipContext,
                                                  BOOL              enable);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_GetAutoRequestLinkedThumbnail()
 *
 * Brief:
 *      Return the setting for this auto request feature.
 *
 * Description:
 *      This function returns the value of the feature for an automatic request
 *      of a thumbnail image after a PutImage.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context,
 *
 *      enable [out] - This feature is enabled/disabled
 *          TRUE  = This module will request a thumbnail on a Initiator PutImage.
 *                  So, the PutImage event is handled completely inside now and the
 *                  BIP Responder application will receive a
 *                  'put linked thumbnail' event later on.
 *          FALSE = This module will not request for a thumbnail, but pass this
 *                  'put image' event to the BIP Responder application.
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 */
BtStatus BTL_BIPRSP_GetAutoRequestLinkedThumbnail(BtlBiprspContext *bipContext,
                                                  BOOL             *enable);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutImage()
 *
 * Brief:
 *      Respond on a PutImage request from the remote Initiator.
 *
 * Description:
 *		
 *	===========================================================
 *		IMPRTANT NOTE:
 *			This function is DEPRECATED and should not be used anymore.
 *			Please use BTL_BIPRSP_RespondPutImageWithObj() instead.
 *	===========================================================
 *
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_PUT_IMAGE
 *
 *      The remote Initiator wants to push an image to this device.
 *      This request can be refused or accepted ('accept').
 *      When accepted, a file will be created in the file-system, where the 
 *      image will be stored. The root path for this file can be defined via
 *      'BTL_BIPRSP_SetDefaultImagePath'.
 *      The created filename itself is equal to the name as defined
 *      in the put image request. The Responder has connected a unique
 *      'imgHandle' to this image as well.
 *
 *      When the Initiator is finished with pushing this image, the following
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_PUT_IMAGE
 *
 *      RESTRICTION: The image that will be received (put) can only be
 *                   stored in the file-system now.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context,
 *
 *      accept [in] -
 *          TRUE = Accept the request to put an image on my file-system.
 *          FALSE = Refuse the request to put an image on my file-system.
 *
 *      imgHandle [in] - Unique image handle (0-terminated string),
 *          identifying the image that will be send.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 *      BT_STATUS_FFS_ERROR -  Error detected while creating a file for
 *          the image.
 */
BtStatus BTL_BIPRSP_RespondPutImage(	
												BtlBiprspContext 	*bipContext,
                                    BOOL              accept,
                                    BipImgHandle      imgHandle);

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutImageWithObj()
 *
 * Brief:
 *      Respond on a PutImage request from the remote Initiator and store the image in a user-specified location
 *
 * Description:
 *	  This function is almost identical to BTL_BIPRSP_RespondPutImage(). The only difference is that this
 *	  function receives a objToReceive argument that allows the user to specify the location of the received
 *	  image.
 *
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_PUT_IMAGE
 *
 *      The remote Initiator wants to push an image to this device.
 *      This request can be refused or accepted ('accept').
 *      When accepted, the received image will be stored in the location specified by 
 *	   objToReceive.
 *
 *      When the Initiator is finished with pushing this image, the following
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_PUT_IMAGE
 *
 *      RESTRICTION: The image that will be received (put) can only be
 *                   stored in the file-system now.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context,
 *
 *      accept [in] -
 *          TRUE = Accept the request to put an image on my file-system.
 *          FALSE = Refuse the request to put an image on my file-system.
 *
 *      imgHandle [in] - Unique image handle (0-terminated string),
 *          identifying the image that will be send.
 *
 *      objToReceive [in / out] - Location where the received image will be stored.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 *      BT_STATUS_FFS_ERROR -  Error detected while creating a file for
 *          the image.
 */
BtStatus BTL_BIPRSP_RespondPutImageWithObj(	
												BtlBiprspContext 	*bipContext,
												BOOL              	accept,
												BipImgHandle      	imgHandle,
												BtlObject			*objToReceive);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutLinkedThumbnail()
 *
 * Brief:
 *      Respond on a PutLinkedThumbnail request from the remote Initiator.
 *
 * Description:
 *
 *	===========================================================
 *		IMPRTANT NOTE:
 *			This function is DEPRECATED and should not be used anymore.
 *			Please use BTL_BIPRSP_RespondPutLinkedThumbnailWithObj() instead.
 *	===========================================================

 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_PUT_LINKED_THUMBNAIL
 *
 *      The remote Initiator wants to push a thumbnail image to this device.
 *      This request can be refused or accepted ('accept').
 *      When accepted, a file ('fileName') will be created in the file-system,
 *      where the thumbnail image will be stored.
 *      The root path for this file can be defined via
 *      'BTL_BIPRSP_SetDefaultImagePath'.
 *
 *      RESTRICTION: The image that will be received (put) can only be
 *                   stored in the file-system now.
 *
 *      When the Initiator is finished with pushing this image, the following
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_PUT_LINKED_THUMBNAIL
 *
 * Type:
 *      Synchronous
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to put an image on my file-system.
 *          FALSE = Refuse the request to put an image on my file-system.
 *
 *      fileName [in] - Name of the file (excluding the file extension)
 *          to be created for the thumbnail that will be received.
 *          A thumbnail is always JPEG encoded. The file will get the
 *          'jpg' extension. It is mandatory that the 'fileName' string
 *          does have enough space to add the 4 character extension
 *          ('.jpg').
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 *      BT_STATUS_FFS_ERROR -  Error detected while creating a file for
 *          the thumbnail image.
 */
BtStatus BTL_BIPRSP_RespondPutLinkedThumbnail(BtlBiprspContext *bipContext,
                                              BOOL              accept,
                                              BtlUtf8          *fileName);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondPutLinkedThumbnailWithObj()
 *
 * Brief:
 *      Respond on a PutLinkedThumbnail request from the remote Initiator and store the thumbnail in a user-specified location.
 *
 * Description:
 *
 *	  This function is almost identical to BTL_BIPRSP_RespondPutLinkedThumbnail(). The only difference is that this
 *	  function receives a objToReceive argument that allows the user to specify the location of the received
 *	  thumbnail.
 *
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_PUT_LINKED_THUMBNAIL
 *
 *      When accepted, the received image will be stored in the location specified by 
 *	   objToReceive.
 *
 *      The remote Initiator wants to push a thumbnail image to this device.
 *      This request can be refused or accepted ('accept').
 *	   When accepted, the received thumbnail will be stored in the location specified by 
 *	   objToReceive.
 *
 *      When the Initiator is finished with pushing this image, the following
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_PUT_LINKED_THUMBNAIL
 *
 * Type:
 *      Synchronous
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to put an image on my file-system.
 *          FALSE = Refuse the request to put an image on my file-system.
 *
 *      objToReceive [in / out] - Location where the received thumbnail will
 *          be stored.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 *      BT_STATUS_FFS_ERROR -  Error detected while creating a file for
 *          the thumbnail image.
 */
BtStatus BTL_BIPRSP_RespondPutLinkedThumbnailWithObj(
															BtlBiprspContext 	*bipContext,
                                              								BOOL           		accept,
								                                             BtlObject			*objToReceive);

/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetCapabilities()
 *
 * Brief:
 *      Respond on a GetCapabilities request from the remote Initiator.
 *
 * Description:
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_GET_CAPABILITIES
 *
 *      The remote Initiator wants to know the capabilities of this device.
 *      
 *      This request can be refused or accepted ('accept').
 *      When accepted, the capabilities (stored in 'responseObj' are returned.
 *      These capabilities are returned via an XML encoded descriptor.
 *      This XML descriptor does contain a sequence of elements:
 *      1. preferred-format     (BTL_BIPRSP_BuildCapabilitiesStart)
 *      2. image-formats*       (BTL_BIPRSP_BuildCapabilitiesAddImgFormat)
 *      3. attachment-formats*  (BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat)
 *      4. filtering-parameters (BTL_BIPRSP_BuildCapabilitiesFinish)
 *
 *      Between brackets, the support function is mentioned to add an element to
 *      this XML descriptor.The creation of this XML descriptor should start with
 *      'BTL_BIPRSP_BuildCapabilitiesStart' and end with
 *      'BTL_BIPRSP_BuildCapabilitiesFinish'.
 * 
 *      When this OBEX exchange is finished th following 
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_GET_CAPABILITIES
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to put an image on my file-system.
 *          FALSE = Refuse the request to put an image on my file-system.
 *
 *      responseObj [in] - capabilities object (XML), either stored in memory
 *          or on the file system.
 *          RESTRICTION: file-system is not supported (yet).
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY    - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 */
BtStatus BTL_BIPRSP_RespondGetCapabilities(BtlBiprspContext *bipContext,
                                           BOOL              accept,
                                           BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetImagesList()
 *
 * Brief:
 *      Respond on a GetImagesList request from the remote Initiator.
 *
 * Description:
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_IMAGE_LIST
 *
 *      The remote Initiator wants to get a list images that can be retrieved.
 *
 *      This request can be refused or accepted ('accept').
 *      When accepted, the images list (stored in 'responseObj' is returned.
 *      This images list is returned via an XML encoded descriptor.
 *      This XML descriptor does contain a sequence of elements:
 *      1. images-handles   (BTL_BIPRSP_BuildListingStart)
 *      2. images-listing*  (BTL_BIPRSP_BuildListingAddImg)
 *
 *      Between brackets, the support function is mentioned to add an element to
 *      this XML descriptor. The creation of this XML descriptor should start with
 *      'BTL_BIPRSP_BuildListingStart' and end with
 *      'BTL_BIPRSP_BuildListingFinish'.
 *
 *      When this OBEX exchange is finished th following 
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_GET_IMAGES_LIST
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to provide an images list.
 *          FALSE = Refuse the request to provide an images list.
 *
 *      nbOfReturnedHandles [in] - Number of image handles in the images list.
 *
 *      filter [in] - applied filter settings when generating  this images list
 *          NULL = same filter applied as defined in the request.
 *          RESTRICTION: Only NULL supported. The stack does not support a value
 *                       for this parameter. This parameter will be ignored.
 *
 *      responseObj [in] - images list descriptor (XML), either stored in memory
 *          or on the file system.
 *          NULL = respond only the 'nbOfReturnedHandles' to the remote
 *              Initiator, because it has asked for it. (The request did contain
 *              a 'nbOfReturnedHandles' field with value 0.
 *          RESTRICTION: file-system is not supported (yet).
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY    - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 */
BtStatus BTL_BIPRSP_RespondGetImagesList(BtlBiprspContext *bipContext,
                                         BOOL              accept,
                                         U16               nbOfReturnedHandles,
                                         BtlBipFilter     *filter,
                                         BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetImageProperties()
 *
 * Brief:
 *      Respond on a GetImageProperties request from the remote Initiator.
 *
 * Description:
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_IMAGE_PROPERTIES
 *
 *      The remote Initiator wants to know the properties of a specific image.
 *      
 *      This request can be refused or accepted ('accept').
 *      When accepted, the properties (stored in 'responseObj' are returned.
 *      These properties are returned via an XML encoded string
 *      This XML string does contain a sequence of elements:
 *      1. image-handle        (BTL_BIPRSP_BuildPropertiesStart)
 *      2. friendly-name       (BTL_BIPRSP_BuildPropertiesStart)
 *      3  native-encoding     (BTL_BIPRSP_BuildPropertiesAddNativeEncoding)
 *      4. variant-encoding*   (BTL_BIPRSP_BuildPropertiesAddVariantEncoding)
 *      5. attachment*         (BTL_BIPRSP_BuildPropertiesAddAttachment)
 *
 *      Between brackets, the support function is mentioned to add an element to
 *      this XML descriptor. This construction of the XML descriptor should
 *      end with a call to 'BTL_BIPRSP_BuildPropertiesFinish'.
 *
 *      When this OBEX exchange is finished the following 
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_GET_IMAGE_PROPERTIES
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to provide the image properties.
 *          FALSE = Refuse the request to provide the image properties.
 *
 *      responseObj [in] - image properties object (XML), either stored in memory
 *          or on the file system.
 *          RESTRICTION: file-system is not supported (yet).
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *          BIPINITIATOR_COMPLETE - The server operation has completed
 *
 *      BT_STATUS_BUSY    - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 */
BtStatus BTL_BIPRSP_RespondGetImageProperties(BtlBiprspContext *bipContext,
                                              BOOL              accept,
                                              BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetImage()
 *
 * Brief:
 *      Respond on a GetImage request from the remote Initiator.
 *
 * Description:
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_GET_IMAGE
 *
 *      The remote Initiator wants to retrieve a specific image.
 *
 *      When this OBEX exchange is finished the following 
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_GET_IMAGE
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to provide the image.
 *          FALSE = Refuse the request to provide the image
 *
 *      responseObj [in] - Image data, either stored in memory or in a file.
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
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
 *      BT_STATUS_FFS_ERROR -  Error detected while opening the file
 *          (in case the image data is stored in a file).
 */
BtStatus BTL_BIPRSP_RespondGetImage(BtlBiprspContext *bipContext,
                                    BOOL              accept,
                                    BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetLinkedThumbnail()
 *
 * Brief:
 *      Respond on a GetLinkedThumbnail request from the remote Initiator.
 *
 * Description:
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_GET_LINKED_THUMBNAIL
 *
 *      The remote Initiator wants to retrieve a specific thumbnail image.
 *
 *      When this OBEX exchange is finished the following 
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_GET_LINKED_THUMBNAIL
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to provide the thumbnail
 *          FALSE = Refuse the request to provide the thumbnail.
 *
 *      responseObj [in] - Image data, either stored in memory or in a file.
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
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
 *      BT_STATUS_FFS_ERROR -  Error detected while opening the file
 *          (in case the image data is stored in a file).
 */
BtStatus BTL_BIPRSP_RespondGetLinkedThumbnail(BtlBiprspContext *bipContext,
                                              BOOL              accept,
                                              BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_RespondGetMonitoringImage()
 *
 * Brief:
 *      Respond on a GetMonitoringImages request from the remote Initiator.
 *
 * Description:
 *      Respond on the following event:
 *      - event = BIPRESPONDER_REQUEST
 *      - bipOp = BIPOP_GET_MONITORING_IMAGE
 *
 *      The remote Initiator wants to retrieve a monitor image, which will be
 *      returned with this function as a thumbnail image.
 *
 *      When this OBEX exchange is finished the following 
 *      event will be signaled to the application callback:
 *      - event = BIPRESPONDER_COMPLETE
 *      - bipOp = BIPOP_GET_MONITORING_IMAGE
 *
 * Type:
 *      Synchronous/Asynchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      accept [in] -
 *          TRUE = Accept the request to provide the monitoring image
 *          FALSE = Refuse the request to provide the monitoring image
 *
 *      imgHandle [in] - Unique image handle, identifying the image that
 *          will be send.
 *          NULL = no imgHandle to be send.
 *          RESTRICTION: Only NULL is supported. The stack cannot respond
 *                       an imgHandle value. This parameter is ignored.
 *
 *      responseObj [in] - Thumbnail image data, either stored in memory or in a file.
 *
 * Generated Events:
 *      BIPRESPONDER_COMPLETE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_PENDING - Operation was successfully started.
 *          Completion will be signaled via an event to the
 *          application callback:
 *
 *      BT_STATUS_BUSY    - Operation was not started because an other
 *          operation is pending.
 *
 *      BT_STATUS_FAILED  - Unable to start the operation.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *          there is no remote service connected.
 *
 *      BT_STATUS_INVALID_PARM  - Invalid parameters.
 *
 *      BT_STATUS_FFS_ERROR -  Error detected while opening the file
 *          (in case the image data is stored in a file).
 */
BtStatus BTL_BIPRSP_RespondGetMonitoringImage(BtlBiprspContext *bipContext,
                                              BOOL              accept,
                                              BipImgHandle      imgHandle,
                                              BtlObject        *responseObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_Abort()
 *
 * Brief:
 *      Abort the current active OBEX operation
 *
 * Description:
 *      Abort the current active OBEX operation to the remote Initiator for this
 *      BIP Responder ('bipContext')
 *
 * Type:
 *      Synchronous.
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation was successful.
 *
 *      BT_STATUS_FAILED - Operation failed to start.
 *
 *      BT_STATUS_NO_CONNECTION - Operation was not started because
 *           this Responder is not connected.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BIPRSP_Abort(BtlBiprspContext *bipContext);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesStart()
 *
 * Brief:
 *      Start the building (creation) of an 'Image-properties' object.
 *
 * Description:
 *      Start the building (creation) of an 'Image-properties' object for this
 *      image ('imgHandle').
 *      This object is XML encoded. With this function the creation is started.
 *      The result of this XML building will be stored in 'xmlObj' as a
 *      0-terminated string.
 *      The fixed attributes for this object ('imgHandle'+ 'friendlyName') will
 *      already be added to this 'xmlObj' with this function.
 *
 *      The building session started with this function should have been finished
 *      ('BTL_BIPRSP_BuildPropertiesFinish') before starting another
 *      building session. The 'xmlObj' and 'xmlObjMaxLen' with the relevant
 *      building administration is maintained inside the 'bipContext'
 *      administration.
 *
 *      A complete session for building an 'Image-properties' object looks
 *      like:
 *      1. BTL_BIPRSP_BuildPropertiesStart
 *      2. ({BTL_BIPRSP_BuildPropertiesAddNativeEncoding})
 *      3. ({BTL_BIPRSP_BuildPropertiesAddVariantEncoding})
 *      4. ({BTL_BIPRSP_BuildPropertiesAddAttachment})
 *      5. BTL_BIPRSP_BuildPropertiesFinish
 *
 *      () = optional
 *      {} = iteration
 *
 *      With the BTL_BIPRSP_BuildPropertiesAdd*** routines, other attributes
 *      can be added.
 *      With the BTL_BIPRSP_BuildPropertiesFinish, this 'xmlObj' is finalized
 *      and it is ready to be used in the BTL_BIPRSP_RespondGetImageProperties
 *      call.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      xmlObj [in/out] - start of the XML encoded object to be responded
 *          when finished. This memory space is used to fill the XML encoded
 *          'Image-properties'.
 *
 *      xmlObjMaxLen [in] - Maximum amount of bytes that can be written in
 *           'xmlObj'
 *
 *      imgHandle [in] - Unique identification of this image
 *
 *      friendlyName [in] - Utf-8 encoded 0-terminated string representing
 *            a human friendly readable name. Typically the file-name.
 *            NULL = no Friendly Name.
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - XML object to build exceeds the provided memory
 *          space ('xmlObjMaxLen')
 *          Or previous build session was not finished yet.
 *
 */
BtStatus BTL_BIPRSP_BuildPropertiesStart(BtlBiprspContext *bipContext,
                                         U8               *xmlObj,
                                         U16               xmlObjMaxLen,
                                         BipImgHandle      imgHandle,
                                         BtlUtf8          *friendlyName);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesAddNativeEncoding()
 *
 * Brief:
 *      Add a native 'encoding' attribute to the 'Image-properties' object.
 *
 * Description:
 *      Add a native 'encoding' attribute to the 'Image-properties'
 *      object ('xmlObj') as being constructed in the 'bipContext' admin.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *      
 *      encoding [in] - A native image encoding format
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildPropertiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildPropertiesAddNativeEncoding(BtlBiprspContext     *bipContext,
                                                     BtlBipNativeEncoding *encoding);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesAddVariantEncoding()
 *
 * Brief:
 *      Add a variant 'encoding' attribute to the 'Image-properties' object.
 *
 * Description:
 *      Add a variant 'encoding' attribute to the 'Image-properties'
 *      object ('xmlObj') as being constructed in the 'bipContext' admin.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *
 *      encoding [in] - A variant image encoding format
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildPropertiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildPropertiesAddVariantEncoding(BtlBiprspContext      *bipContext,
                                                      BtlBipVariantEncoding *encoding);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesAddAttachment()
 *
 * Brief:
 *      Add an 'attachment' attribute to the 'Image-properties' object.
 *
 * Description:
 *      Add an 'attachment' attribute to the 'Image-properties'
 *      object ('xmlObj') as being constructed in the 'bipContext' admin.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *      
 *      attachment [in] - A attachment attribute.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildPropertiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildPropertiesAddAttachment(BtlBiprspContext *bipContext,
                                                 BtlBipAttachment *attachment);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildPropertiesFinish()
 *
 * Brief:
 *      Finalize the building (creation) session of an 'Image-properties' object.
 *
 * Description:
 *      Finalize the building (creation) session of an 'Image-properties' object.
 *      The created XML string ('xmlObj) is also returned. It is the same 'xmlObj'
 *      as provide before with BTL_BIPRSP_BuildPropertiesStart.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *      
 *      xmlObj [out] - Created XML encoded object (0-terminated string).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildPropertiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildPropertiesFinish(BtlBiprspContext  *bipContext,
                                          U8               **xmlObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesStart()
 *
 * Brief:
 *      Start the building (creation) of an 'Imaging-capabilities' object.
 *
 * Description:
 *      Start the building (creation) of an 'Imaging-capabilities' object for this
 *      Responder.
 *      This object is XML encoded. With this function the creation is started.
 *      The result of this XML building will be stored in 'xmlObj' as a
 *      0-terminated string.
 *      The fixed attributes for this object ('preferredFormat') will already be
 *      added to this 'xmlObj' with this function.
 *
 *      The building session started with this function should have been finished
 *      ('BTL_BIPRSP_BuildCapabilitiesFinish') before starting another
 *      building session. The 'xmlObj' and 'xmlObjMaxLen' with the relevant
 *      building administration is maintained inside the 'bipContext'
 *      administration.
 *
 *      A complete session for building an 'Imaging-capabilities' object looks
 *      like:
 *      1. BTL_BIPRSP_BuildCapabilitiesStart
 *      2. ({BTL_BIPRSP_BuildCapabilitiesAddImgFormat***})
 *      3. ({BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat***})
 *      4. BTL_BIPRSP_BuildCapabilitiesFinish
 *
 *      () = optional
 *      {} = iteration
 *
 *      With the BTL_BIPRSP_BuildCapabilitiesAdd*** routines, other attributes
 *      can be added.
 *      With the BTL_BIPRSP_BuildCapabilitiesFinish, this 'xmlObj' is finalized
 *      and it is ready to be used in the BTL_BIPRSP_RespondGetCapabilities
 *      call.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      xmlObj [in/out] - start of the XML encoded object to be responded
 *          when finished. This memory space is used to fill the XML encoded
 *          'Imaging-capabilities'.
 *
 *      xmlObjMaxLen [in] - Maximum amount of bytes that can be written in
 *           'xmlObj'
 *
 *      preferredFormat [in] - Preferred image format attribute.
 *          NULL = don't care (no preferred format).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - XML object to build exceeds the provided memory
 *          space ('xmlObjMaxLen')
 *          Or previous build session was not finished yet.
 *
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesStart(BtlBiprspContext      *bipContext,
                                           U8                    *xmlObj,
                                           U16                    xmlObjMaxLen,
                                           BtlBipPreferredFormat *preferredFormat);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesAddImgFormat()
 *
 * Brief:
 *      Add an image 'format' attribute to the 'Image-capabilities' object.
 *
 * Description:
 *      Add an image 'format' attribute to the 'Image-capabilities'
 *      object ('xmlObj') as being constructed in the 'bipContext' admin.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *      
 *      attachment [in] - A attachment attribute.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildCapabilitiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesAddImgFormat(BtlBiprspContext   *bipContext,
                                                  BtlBipImageFormats *format);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat()
 *
 * Brief:
 *      Add an attachment 'format' attribute to the 'Image-capabilities' object.
 *
 * Description:
 *      Add an attachment 'format' attribute to the 'Image-capabilities'
 *      object ('xmlObj') as being constructed in the 'bipContext' admin.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *      
 *      attachment [in] - A attachment attribute.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildCapabilitiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesAddAttachmentFormat(BtlBiprspContext        *bipContext,
                                                         BtlBipAttachmentFormats *format);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildCapabilitiesFinish()
 *
 * Brief:
 *      Finalize the building (creation) session of an 'Image-capabilities' object.
 *
 * Description:
 *      Finalize the building (creation) session of an 'Image-capabilities' object.
 *      The created XML string ('xmlObj) is also returned. It is the same 'xmlObj'
 *      as provide before with BTL_BIPRSP_BuildCapabilitiesStart.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *
 *      filterParms [in] - Filtering parameters attribute.
 *          NULL = don't care (support everything).
 *      
 *      xmlObj [out] - Created XML encoded object (0-terminated string).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildCapabilitiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildCapabilitiesFinish(BtlBiprspContext           *bipContext,
                                            BtlBipFilteringParameters  *filterParms,
                                            U8                        **xmlObj);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildListingStart()
 *
 * Brief:
 *      Start the building (creation) of an 'Images-Listing' object.
 *
 * Description:
 *      Start the building (creation) of an 'Images-Listing' object for this
 *      Responder.
 *      This object is XML encoded. With this function the creation is started.
 *      The result of this XML building will be stored in 'xmlObj' as a
 *      0-terminated string.
 *
 *      The building session started with this function should have been finished
 *      ('BTL_BIPRSP_BuildCapabilitiesFinish') before starting another
 *      building session. The 'xmlObj' and 'xmlObjMaxLen' with the relevant
 *      building administration is maintained inside the 'bipContext'
 *      administration.
 *
 *      A complete session for building an 'Images-Listing' object looks
 *      like:
 *      1. BTL_BIPRSP_BuildListingStart
 *      2. ({BTL_BIPRSP_BuildListingAddImg***})
 *      3. BTL_BIPRSP_BuildListingFinish
 *
 *      () = optional
 *      {} = iteration
 *
 *      With the BTL_BIPRSP_BuildListingAddImg routine,  several image listings
 *      can be added.
 *      With the BTL_BIPRSP_BuildListingFinish, this 'xmlObj' is finalized
 *      and it is ready to be used in the BTL_BIPRSP_RespondGetImagesList
 *      call.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context.
 *
 *      xmlObj [in/out] - start of the XML encoded object to be responded
 *          when finished. This memory space is used to fill the XML encoded
 *          'Images-Listing'.
 *
 *      xmlObjMaxLen [in] - Maximum amount of bytes that can be written in
 *           'xmlObj'
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - XML object to build exceeds the provided memory
 *          space ('xmlObjMaxLen')
 *          Or previous build session was not finished yet.
 */
BtStatus BTL_BIPRSP_BuildListingStart(BtlBiprspContext *bipContext,
                                      U8               *xmlObj,
                                      U16               xmlObjMaxLen);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildListingAddImg()
 *
 * Brief:
 *      Add an attachment 'format' attribute to the 'Images-Listing' object.
 *
 * Description:
 *      Add an attachment 'format' attribute to the 'Images-Listing'
 *      object ('xmlObj') as being constructed in the 'bipContext' admin.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *      
 *      imgHandle [in] - Unique identification of this image
 *
 *      creation [in] - Creation date and time of this image
 *          NULL = ignore it. It will not be put in the listing.
 *
 *      modified [in] - Last modification date and time of this image
 *          NULL = ignore it. It will not be put in the listing.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildCapabilitiesStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildListingAddImg(BtlBiprspContext        *bipContext,
                                       BipImgHandle             imgHandle,
                                       BtlDateAndTimeStruct *created,
                                       BtlDateAndTimeStruct *modified);


/*-------------------------------------------------------------------------------
 * BTL_BIPRSP_BuildListingFinish()
 *
 * Brief:
 *      Finalize the building (creation) session of an 'Images-Listing' object.
 *
 * Description:
 *      Finalize the building (creation) session of an 'Images-Listing' object.
 *      The created XML string ('xmlObj) is also returned. It is the same 'xmlObj'
 *      as provide before with BTL_BIPRSP_BuildListingStart.
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      bipContext [in] - Identification of the BIP Responder context
 *          It identifies the 'xmlObj' that is under construction.
 *
 *      xmlObj [out] - Created XML encoded object (0-terminated string).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *
 *      BT_STATUS_FAILED - Object to build exceeds the provided memory space
 *          as defined in 'BTL_BIPRSP_BuildListingStart'.
 *
 */
BtStatus BTL_BIPRSP_BuildListingFinish(BtlBiprspContext  *bipContext,
                                       U8               **xmlObj);

#endif /* __BTL_BIPRSP_H */

