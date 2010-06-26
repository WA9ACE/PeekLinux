/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_avrcptg.h
*
*   BRIEF:          This file defines the API of the BTL Audio Video Remote 
*					Control Profile Target role.
*
*   DESCRIPTION:    General
*
*					The Audio/Video Remote Control Profile (AVRCP) defines 
*					procedures for exchanging 1394 Trade Association AV/C commands 
*					between Bluetooth enabled Audio/Video devices. 
*					Internally, AVRCP uses the Audio/Video Control Transport 
*					Protocol (AVCTP) to exchange messages.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTL_AVRCPTG_H
#define __BTL_AVRCPTG_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <avrcp.h>
#include <sec.h>
#include <btl_common.h>
#include "btl_unicode.h"


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlAvrcptgEvent 	BtlAvrcptgEvent;
typedef struct _BtlAvrcptgContext 	BtlAvrcptgContext;


/*-------------------------------------------------------------------------------
 * BtlAvrcptgChannelId type
 *
 *     Defines the type of an AVRCPTG channel id.
 */
typedef U32 BtlAvrcptgChannelId;

 
/*-------------------------------------------------------------------------------
 * BtlAvrcptgCallBack type
 *
 *     A function of this type is called to indicate BTL AVRCPTG events.
 */
typedef void (*BtlAvrcptgCallBack)(const BtlAvrcptgEvent *event);


/*-------------------------------------------------------------------------------
 * BtlAvrcptgSupportedCategoriesMask type
 *
 *     Defines which categories are supported in the AVRCPTG, can be ORed together.
 */
typedef U16 BtlAvrcptgSupportedCategoriesMask;

#define BTL_AVRCPTG_SUPPORTED_CATEGORY1_PLAYER_RECORDER		(0x0001)
#define BTL_AVRCPTG_SUPPORTED_CATEGORY2_MONITOR_AMPLIFIER	(0x0002)	
#define BTL_AVRCPTG_SUPPORTED_CATEGORY3_TUNER				(0x0004)	
#define BTL_AVRCPTG_SUPPORTED_CATEGORY4_MENU				(0x0008)


/*-------------------------------------------------------------------------------
 * AV/C Frame Subunit Types
 *
 *     Represents AV/C Frame Subunit Types.
 */
#define SUBUNIT_TYPE_MONITOR				 				(0x00)
#define SUBUNIT_TYPE_AUDIO					 				(0x01)
#define SUBUNIT_TYPE_PRINTER				 				(0x02)
#define SUBUNIT_TYPE_DISC					 				(0x03)
#define SUBUNIT_TYPE_TAPE_RECORDER_PLAYER	 				(0x04)
#define SUBUNIT_TYPE_TUNER					 				(0x05)
#define SUBUNIT_TYPE_CA						 				(0x06)
#define SUBUNIT_TYPE_CAMERA					 				(0x07)
#define SUBUNIT_TYPE_PANEL					 				(0x09)
#define SUBUNIT_TYPE_BULLETIN_BOARD			 				(0x0A)
#define SUBUNIT_TYPE_CAMERA_STORAGE			 				(0x0B)
#define SUBUNIT_TYPE_VENDOR_UNIQUE			 				(0x1C)
#define SUBUNIT_TYPE_ALL_SUBUNIT_TYPES		 				(0x1D)
#define SUBUNIT_TYPE_EXTENDED_TO_NEXT_BYTE	 				(0x1E)
#define SUBUNIT_TYPE_UNIT					 				(0x1F)


/*-------------------------------------------------------------------------------
 * AV/C Frame Subunit Ids
 *
 *     Represents AV/C Frame Subunit Ids.
 */
#define SUBUNIT_ID_INSTANCE_0				 				(0x00)
#define SUBUNIT_ID_INSTANCE_1				 				(0x01)
#define SUBUNIT_ID_INSTANCE_2				 				(0x02)
#define SUBUNIT_ID_INSTANCE_3								(0x03)
#define SUBUNIT_ID_INSTANCE_4				 				(0x04)
#define SUBUNIT_ID_EXTENDED_TO_NEXT_BYTE	 				(0x05)
#define SUBUNIT_ID_ALL_INSTANCES			 				(0x06)
#define SUBUNIT_ID_IGNORE					 				(0x07)


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlAvrcptgEvent structure
 *
 *     Represents BTL AVRCPTG event.
 */
struct _BtlAvrcptgEvent 
{
	/* Associated context */
	BtlAvrcptgContext		*avrcptgContext;

	/* Associated channel id */
	BtlAvrcptgChannelId		channelId;

	/* AVRCP TG event */
	AvrcpCallbackParms		*avrcptgEvent;
};


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Init()
 *
 *	Brief:  
 *		Init the AVRCPTG module.
 *
 *	Description:
 *      Init the AVRCPTG module.
 *		Allocates resources needed for the BTL AVRCPTG module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_AVRCPTG_Init(void);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Deinit()
 *
 *	Brief: 
 *		Deinit the AVRCPTG module.
 *
 *	Description:
 *      Deinit the AVRCPTG module.
 *		Deallocates resources needed for the BTL AVRCPTG module.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *		
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_AVRCPTG_Deinit(void);

 
/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Create()
 *
 * Brief:  
 *      Allocates a unique AVRCPTG context.
 *
 * Description:
 *		Allocates a unique AVRCPTG context.
 *		This function must be called before any other AVRCPTG API function.
 *		The allocated context should be supplied in subsequent AVRCPTG API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on AVRCPTG events.
 *		The caller can provide an application handle (previously allocated 
 *		with BTL_RegisterApp), in order to link between different modules.
 *		If there is no need to link between different modules, set appHandle to 0.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		appHandle [in] - application handle, can be 0.
 *
 *		avrcptgCallback [in] - all AVRCPTG events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote AVRCP controller.
 *			If NULL is passed, default value is used.
 *		
 *		avrcptgContext [out] - allocated AVRCPTG context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - AVRCPTG context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create AVRCPTG context.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_RESOURCES - No more resources.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_AVRCPTG_Create(BtlAppHandle *appHandle,
							const BtlAvrcptgCallBack avrcptgCallback,
							const BtSecurityLevel *securityLevel,
							BtlAvrcptgContext **avrcptgContext);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Destroy()
 *
 * Brief:  
 *		Releases a AVRCPTG context (previously allocated with BTL_AVRCPTG_Create).
 *
 * Description:
 *		Releases a AVRCPTG context (previously allocated with BTL_AVRCPTG_Create).
 *		An application should call this function when it completes using AVRCPTG services.
 *		Upon completion, the AVRCPTG context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		avrcptgContext [in/out] - AVRCPTG context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - AVRCPTG context was destroyed successfully.
 *
 *		BT_STATUS_IN_USE -  AVRCPTG context is in use.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_AVRCPTG_Destroy(BtlAvrcptgContext **avrcptgContext);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Enable()
 *
 * Brief:  
 *      Enable AVRCPTG and register AVRCP TG SDP record, called after 
 *		BTL_AVRCPTG_Create.	
 *
 * Description:
 *		Enable AVRCPTG and register AVRCP TG SDP record, called after 
 *		BTL_AVRCPTG_Create.		
 *		After calling this function, AVRCP target is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		avrcptgContext [in] - AVRCPTG context.
 *
 *		serviceName [in] - null-terminated service name which will be written into 
 *			SDP service record and will be discovered by the peer device.
 *			If NULL is passed, default value is used.
 *			The pointer is free after the function returns.
 *
 *		supportedCategories [in] - The categories supported by the TG, 
 *			which will be written into SDP service record and will be discovered 
 *			by the peer device (see BtlAvrcptgSupportedCategoriesMask).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FAILED - The specified context is already enabled.
 */
BtStatus BTL_AVRCPTG_Enable(BtlAvrcptgContext *avrcptgContext, 
							const BtlUtf8 *serviceName,
							const BtlAvrcptgSupportedCategoriesMask supportedCategories);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Disable()
 *
 * Brief:  
 *		Disable AVRCPTG, called before BTL_AVRCPTG_Destroy.
 *
 * Description:
 *		Disable AVRCPTG, called before BTL_AVRCPTG_Destroy.
 *		If any connections exist, they will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		avrcptgContext [in] - AVRCPTG context.
 *
 * Generated Events:
 *      AVRCP_EVENT_DISCONNECT
 *      AVRCP_EVENT_DISABLED
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *		
 *		BT_STATUS_PENDING - At least one stream is connected. The disable process has 
 *			been successfully started. On each connected stream, the application callback 
 *			will receive AVRCP_EVENT_DISCONNECT event.
 *			When the disable process is complete, the application callback will 
 *			receive the AVRCP_EVENT_DISABLED event, indicating the context is disabled.
 */
BtStatus BTL_AVRCPTG_Disable(BtlAvrcptgContext *avrcptgContext);

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Connect()
 *
 * Brief:  
 *      Initiates a connection to a remote AVRCP CT.
 *
 * Description:
 *     	Initiates a connection to a remote AVRCP CT. This function is 
 *     	used to establish the lower layer connection (L2CAP), which allows
 *     	sending messages.
 *
 *     	If the connection attempt is successful, the AVRCP_EVENT_CONNECT event
 *     	will be received.  If the connection attempt is unsuccessful, the
 *     	AVRCP_EVENT_DISCONNECT event will be received.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		channelId [out] - Identifies the ID that corresponds to this channel.
 *
 * Generated Events:
 *      AVRCP_EVENT_CONNECT
 *      AVRCP_EVENT_DISCONNECT
 *
 * Returns:
 *		BT_STATUS_PENDING - The connection process has been successfully
 *         started. When the connection process is complete, the
 *         application callback will receive either the AVRCP_EVENT_CONNECT or 
 *         AVRCP_EVENT_DISCONNECT event.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *     	BT_STATUS_IN_USE - All context channels are already connected or are in the
 *         process of connecting.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     	Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_AVRCPTG_Connect(BtlAvrcptgContext *avrcptgContext,
								BD_ADDR *bdAddr, 
								BtlAvrcptgChannelId *channelId);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_ConnectResponse()
 *
 * Brief:  
 *      Responds to a connection request from the remote AVRCP CT.
 *
 * Description:
 *     	Responds to a connection request from the remote AVRCP CT, indicated by
 *		the event AVRCP_EVENT_CONNECT_IND.
 *		This function is used to establish the lower layer connection (L2CAP),
 *     	which allows the sending of commands.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id, received in AVRCP_EVENT_CONNECT_IND event.
 *
 *		acceptCon [in] - TRUE accepts the connection or FALSE rejects the connection.
 *
 * Generated Events:
 *      AVRCP_EVENT_CONNECT
 *      AVRCP_EVENT_DISCONNECT
 *
 * Returns:
 *		BT_STATUS_PENDING - The connection responses has been successfully
 *         sent. When the connection process is complete, the application 
 *         callback will receive the AVRCP_EVENT_CONNECT event.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *     	BT_STATUS_BUSY - The connection is already connected.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     	Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_AVRCPTG_ConnectResponse(BtlAvrcptgContext *avrcptgContext, 
									BtlAvrcptgChannelId channelId,
									BOOL acceptCon);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_Disconnect()
 *
 * Brief:  
 *      Terminates a connection with a remote AVRCP device.
 *
 * Description:
 *     	Terminates a connection with a remote AVRCP device.  The lower layer
 *     	connection (L2CAP) is disconnected.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 * Generated Events:
 *      AVRCP_EVENT_DISCONNECT
 *
 * Returns:
 *		BT_STATUS_PENDING - The disconnect process has been successfully
 *         started. When the disconnect process is complete, the
 *         application callback will receive the AVRCP_EVENT_DISCONNECT event.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     	BT_STATUS_NO_CONNECTION - No connection exists on the specified context.
 *
 *     	Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus BTL_AVRCPTG_Disconnect(BtlAvrcptgContext *avrcptgContext, BtlAvrcptgChannelId channelId);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_GetConnectedDevice()
 *
 * Brief:  
 *      This function returns the connected device.
 *
 * Description:
 *		This function returns the connected device.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		avrcptgContext [in] - AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 *		bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_NO_CONNECTION -  The target is not connected.
 */
BtStatus BTL_AVRCPTG_GetConnectedDevice(BtlAvrcptgContext *avrcptgContext, 
										BtlAvrcptgChannelId channelId, 
										BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_PanelOperationResponse()
 *
 * Brief:  
 *      Accept or reject a panel operation received from the connected CT.
 *
 * Description:
 *     	Accept or reject a panel operation received from the connected
 *     	controller. This function must be called during
 *     	notification of the AVRCP_EVENT_PANEL_PRESS event.
 *		If accepted, the next panel-related event for the operation will be 
 *		AVRCP_EVENT_PANEL_HOLD (if the controller key is held down for at least 
 *		AVRCP_PANEL_PRESSHOLD_TIME) or AVRCP_EVENT_PANEL_RELEASE (if the controller 
 *		key is released more quickly).
 *		Panel operations intended for "Panel Subunit" target are handled by 
 *		BTL_AVRCPTG_PanelOperationResponse().
 *
 * Requires:
 *     	AVRCP_PANEL_SUBUNIT set to XA_ENABLED
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 *		rsp [in] - Response code. Note that REJECTED is only used
 *         when the operation is already reserved by another controller.
 *         This response code does NOT indicate the result of the
 *         operation itself (for example, a PLAY command when no media
 *         is available should be accepted, but simply have no effect).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_PanelOperationResponse(BtlAvrcptgContext *avrcptgContext,
											BtlAvrcptgChannelId channelId,
											const AvrcpResponse rsp);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_SendResponse()
 *
 * Brief:  
 *      Accept or reject a panel operation received from the connected CT.
 *
 * Description:
 *     	Sends a response on the specified context. The context must be
 *     	connected and in the open state. The "rspFrame" parameter must be
 *     	set with valid response parameters.
 *		Non-Panel operations (such as Vendor Dependent AV/C commands), are handled 
 *		by BTL_AVRCPTG_SendResponse().
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 *		rspFrame [in] - An AvrcpRspFrame structure initialized with valid
 *         response parameters.
 *
 * Generated Events:
 *      AVRCP_EVENT_TX_DONE
 *
 * Returns:
 *		BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVRCP_EVENT_TX_DONE
 *         event.
 *
 *     	BT_STATUS_FAILED - There is no outstanding command to respond to.
 *
 *     	BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *     	BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_AVRCPTG_SendResponse(BtlAvrcptgContext *avrcptgContext,
									BtlAvrcptgChannelId channelId,
									AvrcpRspFrame *rspFrame);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_SetSecurityLevel()
 *
 * Brief:  
 *      Sets security level for the given AVRCPTG context.
 *
 * Description:
 *     	Sets security level for the given AVRCPTG context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote AVRCP controller. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_SetSecurityLevel(BtlAvrcptgContext *avrcptgContext,
								  		const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_GetSecurityLevel()
 *
 * Brief:  
 *      Gets security level for the given AVRCPTG context.
 *
 * Description:
 *     	Gets security level for the given AVRCPTG context.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote AVRCP controller.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_GetSecurityLevel(BtlAvrcptgContext *avrcptgContext,
								  		BtSecurityLevel *securityLevel);



#if AVRCP_METADATA_TARGET == XA_ENABLED

/*-------------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetEventMask()
 *
 * Brief:  
 *     Allows the application to specify which event will be supported by the 
 *	   current media player.
 *
 * Description:
 *     Allows the application to specify which event will be supported
 *     by the current media player.  When a flag is set in the event
 *     mask, then associated feature is supported.  
 *     AVRCP_ENABLE_PLAY_STATUS_CHANGED and AVRCP_ENABLE_TRACK_CHANGED
 *     must both be set, and if not specified, will be added to the
 *     mask.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *     	EventMask [in] - A bitmask with bits set to enable individual events.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_AVRCPTG_MDA_SetEventMask(BtlAvrcptgContext *avrcptgContext,
										AvrcpMetadataEventMask EventMask);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayerSettingsMask()
 *
 * Brief:  
 *     Allows the application to specify which player settings are supported 
 *	   by the current media player.
 *
 * Description:
 *     Allows the application to specify which player settings are 
 *     supported by the current media player.  When a flag is set in 
 *     the event mask, then associated setting is supported.  
 *
 * Type:
 *		Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *     	PlayerAttrIdmask [in] - A bitmask with bits set to enable individual player settings.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayerSettingsMask(BtlAvrcptgContext *avrcptgContext,
                                          		AvrcpPlayerAttrIdMask PlayerAttrIdmask);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetMediaAttributesMask()
 *
 * Brief:  
 *     Allows the application to specify which media attributes are
 *	   supported by the current media player.
 *
 * Description:
 *     Allows the application to specify which media attributes are
 *     supported by the current media player.  When a flag is set in 
 *     the event mask, then associated setting is supported.  
 *
 * Type:
 *		Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      mediaAttrIdmask - A bitmask with bits set to enable individual media attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetMediaAttributesMask(BtlAvrcptgContext *avrcptgContext,
                                          		AvrcpMediaAttrIdMask mediaAttrIdmask);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayerSetting()
 *
 * Brief:  
 *     Allows the application to specify the settings for the current player.
 *
 * Description:
 *     Allows the application to specify the settings for the current player.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      setting [in] - The new player setting.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayerSetting(BtlAvrcptgContext *avrcptgContext, 
                                     		AvrcpPlayerSetting *setting); 

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayerStrings()
 *
 * Brief:  
 *     Allows the application to specify the strings for the current player.
 *
 * Description:
 *     Allows the application to specify the strings for the current player.
 *
 * Type:
 *		Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 *      attrId [in] - The player setting attribute to which the strings apply.
 *
 *      strings [in] - A set of strings in the IANA format specified in the call to 
 *         AVRCP_TgSetMdaCharSet().  If AVRCP_TgSetMdaCharSet() has 
 *         never been called, then the default format is UTF-8.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayerStrings(BtlAvrcptgContext *avrcptgContext,
                                     AvrcpPlayerAttrId attrId,
                                     AvrcpPlayerStrings *strings);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetMediaInfo()
 *
 * Brief:  
 *     Allows the application to specify the media information for the 
 *     current track.  
 *
 * Description:
 *     Allows the application to specify the media information for the 
 *     current track.  
 *
 * Type:
 *	   Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      numElements [in] - The number of elements in the info parameter.
 *
 *      info [in] - An array of structures containing the media information for the
 *         current track (see AvrcpMediaInfo).
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetMediaInfo(BtlAvrcptgContext *avrcptgContext,
									  U8 numElements,
									  AvrcpMediaInfo *info);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayStatus()
 *
 * Brief:  
 *     Allows the application to specify the current player status.
 *
 * Description:
 *     Allows the application to specify the current player status.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      mediaStatus [in] - The current play status of the media.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayStatus(BtlAvrcptgContext *avrcptgContext,
									   AvrcpMediaStatus mediaStatus);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetPlayPosition()
 *
 * Brief:  
 *     Allows the application to specify the media play position and length. 
 *
 * Description:
 *     Allows the application to specify the media play position and length. 
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      position [in] - The current position of the media in milliseconds.
 *
 *      length [in] - The total length of the media in milliseconds.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetPlayPosition(BtlAvrcptgContext *avrcptgContext,
										 U32 position,
										 U32 length);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetBattStatus()
 *
 * Brief:  
 *     Allows the application to specify the current Battery status.
 *
 * Description:
 *     Allows the application to specify the current Battery status.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      battStatus [in] - The current battery status.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetBattStatus(BtlAvrcptgContext *avrcptgContext,
									   AvrcpBatteryStatus battStatus);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetSystemStatus()
 *
 * Brief:  
 *     Allows the application to specify the current System status.
 *
 * Description:
 *     Allows the application to specify the current System status.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      sysStatus [in] - The current system status.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetSystemStatus(BtlAvrcptgContext *avrcptgContext,
										 AvrcpSystemStatus sysStatus);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetTrack()
 *
 * Brief:  
 *     Allows the application to specify the current track.
 *
 * Description:
 *     Allows the application to specify the current track.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *      track [in] - The current track number.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetTrack(BtlAvrcptgContext *avrcptgContext, 
								  U32 track);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SignalTrackStart()
 *
 * Brief:  
 *     Allows the application to signal the track start.
 *
 * Description:
 *     Allows the application to signal the track start.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SignalTrackStart(BtlAvrcptgContext *avrcptgContext);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SignalTrackEnd()
 *
 * Brief:  
 *     Allows the application to signal the end of track.
 *
 * Description:
 *     Allows the application to signal the end of track.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 * Generated Events:
 *      AVCTP_EVENT_TX_DONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *      BT_STATUS_PENDING - The send response operation has been started
 *         successfully on at least one of the connected channels. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *		BT_STATUS_FAILED - The operation failed, this is the message received if no channels are connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SignalTrackEnd(BtlAvrcptgContext *avrcptgContext);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetCurrentCharSet()
 *
 * Brief:  
 *     Allows the application to specify the default IANA character set.
 *
 * Description:
 *     Allows the application to specify the default IANA character set.
 *     This number must match a character set received during the
 *     AVRCP_EVENT_MDA_CHAR_SET event, or it must be set to UTF-8 (default).
 *
 * Type:
 *		Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *     charSet [in] - The IANA character set that will be used.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	   BT_STATUS_FAILED - The operation failed.
 *
 *	   BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_SetCurrentCharSet(BtlAvrcptgContext *avrcptgContext, 
										   U16 charSet);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_GetCurrentCharSet()
 *
 * Brief:  
 *     Retrieves the character set currently selected for display on the CT.
 *
 * Description:
 *     Retrieves the character set currently selected for display on the
 *     controller.
 *
 * Type:
 *		Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		CharSet [out] - pointer to a 16 bit value containing the current IANA character set.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *	   BT_STATUS_FAILED - The operation failed.
 *
 *	   BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_AVRCPTG_MDA_GetCurrentCharSet(BtlAvrcptgContext *avrcptgContext,
											U16 *CharSet);

/*---------------------------------------------------------------------------
 * BTL_AVRCPTG_MDA_SetResponseBuffer()
 *
 * Brief:  
 *     Set the response buffer and size for the channel.
 *
 * Description:
 *     Set the response buffer and size for the channel.  The response buffer
 *     is used for reassembly of responses on the controller or for
 *     assembling a response on the target.  The buffer size must be at least
 *     512 bytes.  Calling this function will override the internal buffer,
 *     if one is allocated.  Defining AVRCP_METADATA_RESPONSE_SIZE to a value
 *     greater than 0 allocates an internal response buffer.  
 *
 * Type:
 *		Synchronous
 *
 * Requires:
 *     AVRCP_METADATA_CONTROLLER or AVRCP_METADATA_TARGET set to XA_ENABLED
 *
 * Parameters:
 *     	avrcptgContext [in] - pointer to the AVRCPTG context.
 *
 *		channelId [in] - associated channel id.
 *
 *      buffer [in] - a buffer to hold responses for transmit (target) or
 *                    receive (controller).
 *      bufLen [in] - the length of the buffer.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The buffer was set successfully.
 *
 *	   BT_STATUS_FAILED - The operation failed.
 *
 *     BT_STATUS_INVALID_PARM - The channel structure or buffer was invalid, 
 *         or the buffer length was less than 512 (XA_ERROR_CHECK enabled).
 */
BtStatus BTL_AVRCPTG_MDA_SetResponseBuffer(BtlAvrcptgContext *avrcptgContext,
										   U8 *buffer,
										   U16 bufLen);

#endif /* AVRCP_METADATA_TARGET == XA_ENABLED */


#endif /* __BTL_AVRCPTG_H */


