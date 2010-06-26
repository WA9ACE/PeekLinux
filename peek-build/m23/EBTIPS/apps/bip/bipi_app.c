/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bipi_app.c
*
*   DESCRIPTION:    This file contains the implementation of the BIP Initiator sample 
*					application layer of the Neptune platform.
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#include "btl_config.h"
#include "osapi.h"
#if BTL_CONFIG_BIP == BTL_CONFIG_ENABLED

#include <string.h>

#include "btl_common.h"
#include "debug.h"
#include "../app_main.h"

#include "btl_bipint.h"
#include "bthal_config.h"
#include "bthal_fs.h"
#include "bttypes.h"

#include "btl_defs.h"
#include "btl_commoni.h"
#include "btl_pool.h"
#include "btl_utils.h"
#include "lineparser.h"

/********************************************************************************
 *
 * Definitions 
 *
 *******************************************************************************/
#define BIPIA_VERIFY(condition, msg, returnValue)		\
			if (!condition)							\
			{										\
				UI_DISPLAY(msg);					\
				return returnValue;					\
			}

#define BIPIA_VERIFY_NO_RETVAR(condition, msg)		\
			if (!condition)							\
			{										\
				UI_DISPLAY(msg);					\
				return;					\
			}

#define XML_OBJ_MAX_LEN ((U16) 1000) /* Max number of bytes for the globalXmlObj storage. */
#define IMG_PROPERTIES_MAX_LINE_LEN			50

/*******************************************************************************
 *
 * Globals
 *
 ******************************************************************************/
BtlBipintContext 	*bipintContext = NULL;

static U8			xmlRxObj[XML_OBJ_MAX_LEN + 1]; /* memory area for receiving an XML object for the Initiator. */
static U8			imageFullPath[LINE_PARSER_MAX_STR_LEN + 1];

static BtlObject	responseXmlObj;
static BtlObject	responseImageObj;

static const U8 *bipiaDummyObexFileName = (const U8*)"Dummy";
static U8			imageHandle[7 + 1];

/*******************************************************************************
 *
 * BIPR Internal Function prototypes
 *
 ******************************************************************************/
static void BIPINTA_Callback(const BtlBipintEvent *btlbipEvent);

static void BipiaInitRxXmlResponseObj(BtlObject *rxResponseObj);
static void BipiaInitRxImageResponseObj(BtlObject *rxResponseObj, const U8 *imageName, const U8 *imageFullPath);
static BOOL BipiaConvertEncodingStr(const char *encodingStr, BtlBipEncoding *encoding);

static const char *GetBipTransformationString(BtlBipTranformation bipTransformation);
static const char* GetBooleanString(BOOL booleanType);
static const char* GetBipEncodingString(BtlBipEncoding bipEncoding);

static const char *pBipOp(BipOp bipOp);
static const char *pBipEvent(BipEvent bipEvent);
static const char *pBipInd(BipDataType indType);

static void DisplayListingInfo(const BtlBipintImagesListMetaData *elementsMetaData);
static BOOL DisplayListingElement(const BtlBipintImagesListMetaData *elementsMetaData);
static void DisplayCapabilities(const BtlBipintCapabilitiesElements *elementsDataCaps);
static void DisplayProperties(const BtlBipintImgPropertiesElements *elementsDataProps);

/*******************************************************************************
 *
 * BIPR External Function prototypes
 *
 ******************************************************************************/

void BIPIA_Init(void);
void BIPIA_Deinit(void);

void BIPIA_Create(void);
void BIPIA_Destroy(void);
void BIPIA_Enable(void);
void BIPIA_Disable(void);
void BIPIA_Abort(void);

void BIPIA_GetSecurityLevel(void);
void BIPIA_SetSecurityLevel(void);

void BIPIA_Connect(void);
void BIPIA_Disconnect(void);

void BIPIA_GetCapabilities(void);
void BIPIA_GetImagesList(void);
void BIPIA_GetImageProperties(void);
void BIPIA_GetImage(void);
void BIPIA_GetLinkedThumbnail(void);
void BIPIA_GetMonitoringImage(void);

void BIPIA_PutImage(void);
void BIPIA_PutLinkedThumbnail(void);

	
/*---------------------------------------------------------------------------
 *            BIPIA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the BIP Initiator.
 *            
 *
 * Return:    void
 *
 */
void BIPIA_Init(void)
{
	BtStatus status;

	status = BTL_BIPINT_Create(NULL, BIPINTA_Callback, NULL, &bipintContext);	

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPIA: BTL_BIPINT_Create() Failed(%s)", pBT_Status(status)));
		return;
	}

	status = BTL_BIPINT_Enable(bipintContext);
	
	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPIA: BTL_BIPINT_Enable() Failed(%s)", pBT_Status(status)));
		return;
	}

	Report(("BIPIA: BIPINT Successfully Initialized"));
}


/*---------------------------------------------------------------------------
 *            BIPIA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the BIP Initiator
 *
 * Return:    void
 *
 */
void BIPIA_Deinit(void)
{
	BtStatus status;

	status = BTL_BIPINT_Disable(bipintContext);

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPIA: BTL_BIPINT_Disable() Failed(%s)", pBT_Status(status)));
		return;
	}

	status = BTL_BIPINT_Destroy(&bipintContext);

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPIA: BTL_BIPINT_Destroy() Failed(%s)", pBT_Status(status)));
		return;
	}

	Report(("BIPIA: BIPINT Successfully De-Initialized"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPIA_Create(void)
{
	BtStatus status;
	
	if (bipintContext != NULL)
	{
		UI_DISPLAY(("BIPIA: BIPINT Single Instance already created"));
		return;
	}

	status = BTL_BIPINT_Create(NULL, BIPINTA_Callback, NULL, &bipintContext);	

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_Create() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: BIPIA_Create BIPINT Successfully Created"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Destroy the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPIA_Destroy(void)
{
	BtStatus status;
	
	if (bipintContext == NULL)
	{
		UI_DISPLAY(("BIPIA: BIPINT Single Instance Doesn't Exist"));
		return;
	}

	status = BTL_BIPINT_Destroy(&bipintContext);	

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_Destroy() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: BIPIA_Destroy BIPINT Successfully Destroyed"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPIA_Enable(void)
{
	BtStatus status;
	
	status = BTL_BIPINT_Enable(bipintContext);

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_Enable() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: BIPINT Successfully Enabled"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPIA_Disable(void)
{
	BtStatus status;
	
	status = BTL_BIPINT_Disable(bipintContext);

	if (status == BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: BIPRSP Successfully Disabled"));
	}
	else if (status == BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPRA: Wating for BIPRSP to Complete Disabling"));
	}
	else
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_Disable() Failed (%s).", pBT_Status(status)));	
	}
}

/*---------------------------------------------------------------------------
 *            BIPIA_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Abort the BIP Initiator operation
 *
 * Return:    void
 *
 */
void BIPIA_Abort(void)
{
	BtStatus status;
   
    status = BTL_BIPINT_Abort(bipintContext);
    UI_DISPLAY(("BIPIA: BTL_BIPINT_Abort() returned %s.",pBT_Status(status)));
    
}

/*---------------------------------------------------------------------------
 *            BIPIA_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given BIPINT context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void BIPIA_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_BIPINT_GetSecurityLevel(bipintContext, &level);

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BIPIA_GetSecurityLevel() Failed(%s).", pBT_Status(status)));
		return;
	}

	switch (level)
	{
		case 0:
			UI_DISPLAY(("BIPIA: Security level: BSL_NO_SECURITY"));
			break;
		case 1:
			UI_DISPLAY(("BIPIA: Security level: BSL_AUTHENTICATION_IN"));
			break;
		case 2:
			UI_DISPLAY(("BIPIA: Security level: BSL_AUTHORIZATION_IN"));
			break;
		case 4:
			UI_DISPLAY(("BIPIA: Security level: BSL_ENCRYPTION_IN"));
			break;
		case 3:
			UI_DISPLAY(("BIPIA: Security level: BSL_AUTHENTICATION_IN"));
			UI_DISPLAY(("BIPIA: and BSL_AUTHORIZATION_IN"));
			break;
		case 5:
			UI_DISPLAY(("BIPIA: Security level: BSL_AUTHENTICATION_IN"));
			UI_DISPLAY(("BIPIA: and BSL_ENCRYPTION_IN"));
			break;
		case 6:
			UI_DISPLAY(("BIPIA: Security level: BSL_AUTHORIZATION_IN"));
			UI_DISPLAY(("BIPIA: and BSL_ENCRYPTION_IN"));
			break;
		case 7:
			UI_DISPLAY(("BIPIA: Security level: BSL_AUTHENTICATION_IN"));
			UI_DISPLAY(("BIPIA: and BSL_AUTHORIZATION_IN"));
			UI_DISPLAY(("BIPIA: and BSL_ENCRYPTION_IN"));
			break;
		default:
			UI_DISPLAY(("BIPIA: Error: Wrong security level"));
			break;
			
	}
}

/*---------------------------------------------------------------------------
 *            BIPIA_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given BIPINT context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void BIPIA_SetSecurityLevel(void)
{
	BtStatus 				status;
	LINE_PARSER_STATUS 	lineStatus;
	BtSecurityLevel 		newSecurityLevel;
	
	lineStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, FALSE);
	
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: Failed reading security level argument (%d)", lineStatus));
		return;
	}

	status = BTL_BIPINT_SetSecurityLevel(bipintContext, &newSecurityLevel);

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_SetSecurityLevel() Failed(%s).", pBT_Status(status)));
		return;
	}
	
	UI_DISPLAY(("BIPIA: Security Level was Successfully set"));
}

/*---------------------------------------------------------------------------
 *             BIPIA_Connect
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Connects to the given Responder.
 *			  If flag is TRUE - connect to a BIPSVC_RESPONDER. Otherwise, connect to BIPSVC_REMOTE_CAMERA
 *
 */
void BIPIA_Connect(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	U8 					bdArray[BDADDR_NTOA_SIZE + 1];
	BD_ADDR 			bdAddr;
	U8					responderType[16];
	BipService			bipService;

	/* Handle  BD Address Argument */
	
	lineStatus = LINE_PARSER_GetNextStr(bdArray, BDADDR_NTOA_SIZE);
	
	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading BD Address Argument(%d)", lineStatus));
		return;
	}

	bdAddr = bdaddr_aton((const char *)(bdArray));

	/* Handle  (Optional) Responder Type Argument */

	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineStatus = LINE_PARSER_GetNextStr(responderType, LINE_PARSER_MAX_STR_LEN);
		
		if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
		{
			UI_DISPLAY(("BIPIA: Failed Reading responder Type argument (%d)", lineStatus));
			return;
		}

		if (OS_StriCmp((const char *)responderType, "responder") == 0)
		{
			bipService = BIPSVC_RESPONDER;
		}
		else if (OS_StriCmp((const char *)responderType, "camera") == 0)	
		{
			bipService = BIPSVC_REMOTE_CAMERA;
		}
		else
		{
			UI_DISPLAY(("BIPIA: Invalid Responder Type (%s)", (char *)responderType));
			return;
		}
	}
	else
	{
		bipService = BIPSVC_RESPONDER;
	}
	
	status = BTL_BIPINT_Connect(bipintContext, &bdAddr, bipService);

	if (status == BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: Successfully Connected to %s", (char*)bdArray));
	}
	else if (status == BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: Establishing Connection to %s", (char*)bdArray));
	}
	else
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_Connect to %s Failed (%s)", (char*)bdArray, pBT_Status(status)));
	}
}

/*---------------------------------------------------------------------------
*            BIPIA_Disconnect
*---------------------------------------------------------------------------
*
* Synopsis:  Disconnects from the given Responder
*
*/
void BIPIA_Disconnect(void)
{
	BtStatus status; 	

	status = BTL_BIPINT_Disconnect(bipintContext);

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_Disconnect() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: BIPINT Successfully Disconnected"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_GetCapabilities
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the remote Responder capabilities.
 *
 */
void BIPIA_GetCapabilities()
{
	BtStatus status;

	BipiaInitRxXmlResponseObj(&responseXmlObj);
	
	status = BTL_BIPINT_GetCapabilities(bipintContext,&responseXmlObj);

	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetCapabilities() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: Waiting for GetCapabilities Response"));
}


/*---------------------------------------------------------------------------
 *            BIPIA_GetImageList
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the remote connected Responder image list 
 *
 */
void BIPIA_GetImagesList()
{
	BtStatus status;

	BipiaInitRxXmlResponseObj(&responseXmlObj);
	
	/* Now get it from the Responder.*/
      status = BTL_BIPINT_GetImagesList(	bipintContext, 
										NULL, 			/* No Filtering */
										FALSE,			/* No Latest Captured Images */
                								0,				/* Start From the beginning */
                								65535,			/* Get All images */
                								&responseXmlObj);
	
	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetImagesList() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: Waiting for GetImagesList Response"));
}


/*---------------------------------------------------------------------------
 *            BIPIA_GetImageProperties
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the properties of an image from the remote connected Responder 
 *
 */
void BIPIA_GetImageProperties(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr(imageHandle, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Handle Argument(%d)", lineStatus));
		return;
	}
	
	status = BTL_BIPINT_GetImageProperties(	bipintContext,
				                                          	imageHandle,
				                                          	&responseXmlObj);

	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetImageProperties() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: Waiting for GetImageProperties Response"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_GetImage
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get an image from the remote connected Responder 
 *
 */
void BIPIA_GetImage(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	char					encodingStr[LINE_PARSER_MAX_STR_LEN + 1];
	BtlBipEncoding			encoding;
	
	lineStatus = LINE_PARSER_GetNextStr(imageHandle, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Handle Argument(%d)", lineStatus));
		return;
	}
	
	lineStatus = LINE_PARSER_GetNextStr(imageFullPath, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Full Path Argument(%d)", lineStatus));
		return;
	}

	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineStatus = LINE_PARSER_GetNextStr((U8*)encodingStr, LINE_PARSER_MAX_STR_LEN);

		if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
		{
			UI_DISPLAY(("BIPIA: Failed Reading Encoding Argument(%d)", lineStatus));
			return;
		}

		if (BipiaConvertEncodingStr(encodingStr, &encoding) == FALSE)
		{
			return;
		}
	}
	else
	{
		encoding = BTL_BIP_ENCODING_JPEG;
	}

	BipiaInitRxImageResponseObj(&responseImageObj, NULL, imageFullPath);

	status = BTL_BIPINT_GetImage(	bipintContext,
		                                          	imageHandle,
									encoding,
									0,
									0,
									BTL_BIP_TRANSFORMATION_NONE,
									&responseImageObj);

	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetImage() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: Waiting for GetImage Response"));
}

/*---------------------------------------------------------------------------
 *            BIPIA_GetLinkedThumbnail
 *---------------------------------------------------------------------------
 *
 *
 */
void BIPIA_GetLinkedThumbnail(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	
	lineStatus = LINE_PARSER_GetNextStr(imageHandle, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Handle Argument(%d)", lineStatus));
		return;
	}
	
	lineStatus = LINE_PARSER_GetNextStr(imageFullPath, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Full Path Argument(%d)", lineStatus));
		return;
	}
	
	BipiaInitRxImageResponseObj(&responseImageObj, NULL, imageFullPath);
	
	status = BTL_BIPINT_GetLinkedThumbnail(	bipintContext,
				                                          	imageHandle,
											&responseImageObj);

	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetLinkedThumbnail() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: Waiting for GetLinkedThumbnail Response"));
}


/*---------------------------------------------------------------------------
 *            BIPIA_GetMonitoringImage
 *---------------------------------------------------------------------------
 *
 *
 */
void BIPIA_GetMonitoringImage(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	BOOL				storeFlag;
	
	lineStatus = LINE_PARSER_GetNextStr(imageFullPath, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Full Path Argument(%d)", lineStatus));
		return;
	}
	
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineStatus = LINE_PARSER_GetNextBool(&storeFlag);

		if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
		{
			UI_DISPLAY(("BIPIA: Failed Reading Boolean Store Flag Argument(%d)", lineStatus));
			return;
		}
	}
	else
	{
		storeFlag = FALSE;
	}
	
	BipiaInitRxImageResponseObj(&responseImageObj, NULL, imageFullPath);
	
	status = BTL_BIPINT_GetMonitoringImage(	bipintContext,
				                                          	storeFlag,
											&responseImageObj);

	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetMonitoringImage() Failed (%s).", pBT_Status(status)));	
		return;
	}

	UI_DISPLAY(("BIPIA: Waiting for GetMonitoringImage Response"));
}


void BIPIA_PutImage()
{			
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	char					encodingStr[LINE_PARSER_MAX_STR_LEN + 1];
	U16					width;
	U16					height;
	BtlBipEncoding			encoding;
	BthalFsStat			fileStat;
	BtFsStatus			fsStatus;
	
	lineStatus = LINE_PARSER_GetNextStr(imageFullPath, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Full Path Argument(%d)", lineStatus));
		return;
	}
	
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineStatus = LINE_PARSER_GetNextU16(&width, LINE_PARSER_MAX_STR_LEN);

		if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
		{
			UI_DISPLAY(("BIPIA: Failed Reading Width Argument(%d)", lineStatus));
			return;
		}
	}
	else
	{
		width = 1024;
	}

	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineStatus = LINE_PARSER_GetNextU16(&height, LINE_PARSER_MAX_STR_LEN);

		if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
		{
			UI_DISPLAY(("BIPIA: Failed Reading Height Argument(%d)", lineStatus));
			return;
		}
	}
	else
	{
		height = 768;
	}
	
	if (LINE_PARSER_AreThereMoreArgs() == TRUE)
	{
		lineStatus = LINE_PARSER_GetNextStr((U8*)encodingStr, LINE_PARSER_MAX_STR_LEN);

		if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
		{
			UI_DISPLAY(("BIPIA: Failed Reading Encoding Argument(%d)", lineStatus));
			return;
		}

		if (BipiaConvertEncodingStr(encodingStr, &encoding) == FALSE)
		{
			return;
		}
	}
	else
	{
		encoding = BTL_BIP_ENCODING_JPEG;
	}

	fsStatus = BTHAL_FS_Stat((BTHAL_U8 *)imageFullPath, &fileStat);

	if (fsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: BTHAL_FS_Stat for %s Failed (%d)", imageFullPath, fsStatus));
		return;
	}
	
	BipiaInitRxImageResponseObj(&responseImageObj, bipiaDummyObexFileName, imageFullPath);
	
	status = BTL_BIPINT_PutImage(	bipintContext, 
									encoding, 
									width,
									height,
									fileStat.size,
									BTL_BIP_TRANSFORMATION_NONE, 
									&responseImageObj);
	
	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_PutImage failed, reason: %s", pBT_Status(status)));
	}
}

/*---------------------------------------------------------------------------
 *            BIPIA_PutLinkedThumbnail
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Put an image thumbnail to the remote connected Responder
 *
 */
void BIPIA_PutLinkedThumbnail(void)
{			
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;	
	
	lineStatus = LINE_PARSER_GetNextStr(imageHandle, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Handle Argument(%d)", lineStatus));
		return;
	}
	
	lineStatus = LINE_PARSER_GetNextStr(imageFullPath, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPIA: Failed Reading Image Full Path Argument(%d)", lineStatus));
		return;
	}
	
	BipiaInitRxImageResponseObj(&responseImageObj, bipiaDummyObexFileName, imageFullPath);

	Report(("BIPIA: Calling BTL_BIPINT_PutLinkedThumbnail, Handle: |%s|", 
			imageHandle, responseImageObj.location.fsLocation.fsPath));
	Report(("BIPIA: File: |%s|", (char*)responseImageObj.location.fsLocation.fsPath));
	
	status = BTL_BIPINT_PutLinkedThumbnail(	bipintContext, 
											imageHandle,
											&responseImageObj);
	
	if (status != BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_PutImage failed, reason: %s", pBT_Status(status)));
	}
}

static void BipiaDebugSaveXml(const U8 *xml, const char *debugFullFileName)
{
	char					tempStr[IMG_PROPERTIES_MAX_LINE_LEN + 1];
	BthalFsFileDesc		fd = 0;
	BtFsStatus			fsStatus;
	BTHAL_U32			numToWrite;
	BTHAL_U32			numWritten;
	BOOL				retVal = TRUE;

	Report(("BIPIA: Saving XML in %s", debugFullFileName));
	
	if (xml == NULL)
	{
		Report(("BIPIA: Null XML"));
		
		return;
	}

	fsStatus = BTHAL_FS_Open(	(BTHAL_U8*)debugFullFileName, 
								BTHAL_FS_O_WRONLY | BTHAL_FS_O_CREATE | BTHAL_FS_O_TRUNC | BTHAL_FS_O_TEXT,
								&fd);
	if (fsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		Report(("BIPIA: ERROR: Failed opening Debug XML file (%s)", debugFullFileName));
		return;
	}

	numToWrite = (BTHAL_U32)strlen((const char*)xml);
	
	Report(("BIPIA: XML Len: %d", numToWrite));

	if (numToWrite > XML_OBJ_MAX_LEN)
	{
		Report(("BIPIA: ERROR: XML Len Exceeds  MAX (%d), truncating", XML_OBJ_MAX_LEN));
		numToWrite = XML_OBJ_MAX_LEN;
	}
	
 	fsStatus = BTHAL_FS_Write(fd, (void*)xml, numToWrite, &numWritten);
	
	BTHAL_FS_Close(fd);	
}

static void BipiaHandleDataIndication(const BtlBipintEvent *btlbipEvent)
{
	BtStatus							status;
	BtlBipintCapabilitiesElements			elementsCapabilitiesData;
      BtlBipintCapabilitiesElementsMask		elementsCapabilitiesMask;
	BtlBipintImagesListMetaData     		elementsListMetaData;
      	BtlBipintImagesListElementsMask		elementsListMask;
	BOOL							moreListingElements;
	BtlBipintImgPropertiesElements		elementsPropertiesData;
	BtlBipintImgPropertiesElementsMask	elementsPropertiesMask;

	UI_DISPLAY(("BIPIA: Initiator received %s", pBipInd(btlbipEvent->parms->data->dataType)));
	
	switch (btlbipEvent->parms->data->dataType)
	{
		
	case BIPIND_IMAGE:
	case BIPIND_THUMBNAIL:

	break;

	case BIPIND_HANDLES_DESCRIPTOR:

		BipiaDebugSaveXml(btlbipEvent->parms->data->i.iHDsc.buff, "/bip/Bipia_HandlesDescriptor.xml");

	break;
	
	case BIPIND_IMAGE_DESCRIPTOR:

		BipiaDebugSaveXml(btlbipEvent->parms->data->i.iIDsc.buff, "/bip/Bipia_ImageDescriptor.xml");
			
	break;
	
	case BIPIND_IMG_CAPABILITIES_XML:

		BipiaDebugSaveXml((const U8*)responseXmlObj.location.memLocation.memAddr, "/bip/Bipia_ImageCapabilities.xml");

		Report(("BIPIA: Calling BTL_BIPINT_ParseGetCapabilitiesResponse"));
		
		status = BTL_BIPINT_ParseGetCapabilitiesResponse(bipintContext, 
														&responseXmlObj, 
														&elementsCapabilitiesData, 
														&elementsCapabilitiesMask);
		BIPIA_VERIFY_NO_RETVAR(	(status == BT_STATUS_SUCCESS), 
									("BIPIA: ERROR: BTL_BIPINT_ParseGetCapabilitiesResponse Failed (%s)", pBT_Status(status)));
	
		DisplayCapabilities(&elementsCapabilitiesData);
		
	break;
	
	case BIPIND_IMG_LISTING_XML:

		Report(("BIPIA: Calling BTL_BIPINT_ParseGetImagesListResponse"));

		BipiaDebugSaveXml((const U8*)responseXmlObj.location.memLocation.memAddr, "/bip/Bipia_ImagesList.xml");
		
		status = BTL_BIPINT_ParseGetImagesListResponse(	bipintContext,
														&responseXmlObj,
														&elementsListMetaData, 
														&elementsListMask);
		BIPIA_VERIFY_NO_RETVAR(	(status == BT_STATUS_SUCCESS), 
									("BIPIA: ERROR: BTL_BIPINT_ParseGetImagesListResponse Failed (%s)", pBT_Status(status)));

		Report(("BIPIA: %d Handles, elementsListMask = %x", elementsListMetaData.nbReturnedHandles, elementsListMask));
		
		/* Display fixed part */
		DisplayListingInfo(&elementsListMetaData);

		do
             {
			moreListingElements = DisplayListingElement(&elementsListMetaData);
			
		} while (moreListingElements == TRUE);

	break;

	case BIPIND_IMG_PROPERTIES_XML:

		BipiaDebugSaveXml((const U8*)responseXmlObj.location.memLocation.memAddr, "/bip/Bipia_ImageProperties.xml");
		
		status = BTL_BIPINT_ParseGetImagePropertiesResponse(	bipintContext,
																&responseXmlObj,
																&elementsPropertiesData,
																&elementsPropertiesMask);
		BIPIA_VERIFY_NO_RETVAR(	(status == BT_STATUS_SUCCESS), 
									("BIPIA: ERROR: BTL_BIPINT_ParseGetImagePropertiesResponse Failed (%s)", pBT_Status(status)));

		DisplayProperties(&elementsPropertiesData);
				
	break;
	
	case BIPIND_IMAGE_HANDLE:

		UI_DISPLAY(("BIPIA: Image Handle: %s", btlbipEvent->parms->data->i.iHndl.imgHandle));		
	
	break;
			
	}
}

static void BipiaHandleComplete(const BtlBipintEvent *btlbipEvent)
{
	UI_DISPLAY(("BIPIA: %s Complete status = %d", pBipOp(btlbipEvent->parms->data->bipOp), btlbipEvent->parms->status));
	
	switch (btlbipEvent->parms->data->bipOp)
	{
	case BIPOP_CONNECT:

		/* Should not occur anymore, replaced by BIPINITIATOR_CONNECTED + BIPINITIATOR_DISCONNECTED events*/

	break;

	case BIPOP_PUT_IMAGE:

		if (btlbipEvent->parms->status == OBRC_PARTIAL_CONTENT)
		{
			UI_DISPLAY(("BIPIA: Reponder Requests Linked Thumbnail - Put it using Responder's returned Handle"));
		}

	break;
	
	case BIPOP_PUT_LINKED_THUMBNAIL:

	break;
	
	case BIPOP_GET_IMAGE:

		UI_DISPLAY(("BIPIA: Received image in %s", imageFullPath));

	break;
	
	case BIPOP_GET_LINKED_THUMBNAIL:

		UI_DISPLAY(("BIPIA: Received image in %s", imageFullPath));

	break;
	
	case BIPOP_GET_MONITORING_IMAGE:

		UI_DISPLAY(("BIPIA: Received image in %s", imageFullPath));

	break;

	case BIPOP_GET_CAPABILITIES:

	break;
	
	case BIPOP_GET_IMAGES_LIST:
	
	break;

	case BIPOP_GET_IMAGE_PROPERTIES:
	
	break;
			
	}
	
}

static void BipiaHandleConnected(const BtlBipintEvent *btlbipEvent)
{
	BtStatus	status;
	BD_ADDR	bdAddr;
	char 	addr[BDADDR_NTOA_SIZE];
	
	UI_DISPLAY(("BIPIA: BIPINT CONNECTED on channel = %x", btlbipEvent->parms->channel));
		
	status = BTL_BIPINT_GetConnectedDevice(bipintContext, &bdAddr);
		
	if (status == BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPIA: Connected device: %s.", bdaddr_ntoa(&(bdAddr), addr)));
	}
	else
	{
		UI_DISPLAY(("BIPIA: BTL_BIPINT_GetConnectedDevice Failed (%s)", pBT_Status(status)));
	}
}

/*---------------------------------------------------------------------------
 *            BIPINTA_Callback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle the BIPINTA_Callback events
 *
 * Return:    void
 *
 */
static void BIPINTA_Callback(const BtlBipintEvent *btlbipEvent)
{
	UI_DISPLAY(("BIPIA: BIPINTA_Callback: Event: %s, Op:%s", 
				pBipEvent(btlbipEvent->parms->event), pBipOp(btlbipEvent->parms->data->bipOp)));
	
	switch ( btlbipEvent->parms->event )
	{
		case BIPINITIATOR_DATA_REQ:
		
		break;
		
		case BIPINITIATOR_DATA_IND:

			BipiaHandleDataIndication(btlbipEvent);
			
		break;
		
		case BIPINITIATOR_RESPONSE:

			
		break;
		
		case BIPINITIATOR_COMPLETE:
			
			BipiaHandleComplete(btlbipEvent);
			
		break;
		
		case BIPINITIATOR_FAILED:
			
			UI_DISPLAY(("BIPIA: BTL BIP Initiator Session Failed, status = %d", btlbipEvent->parms->status));

		break;

		case BIPINITIATOR_ABORT:
			
			UI_DISPLAY(("BIPIA: BTL BIP Initiator OBEX session aborted, response code = %d", btlbipEvent->parms->status));

		break;

		case BIPINITIATOR_CONNECTED:

			BipiaHandleConnected(btlbipEvent);
						
		break;

		case BIPINITIATOR_DISCONNECTED:

			UI_DISPLAY(("BIPIA: BTL BIP Initiator DISCONNECTED channel = %x", btlbipEvent->parms->channel));

		break;

		case BIPINITIATOR_DISABLED:

			UI_DISPLAY(("BIPIA: BIPINT Disabled"));
			
		break;

		case BIPINITIATOR_AUTH_CHALLENGE_RCVD:

			UI_DISPLAY(("BIPIA: OBEX authentication challenge was received. Ignored."));

		break;

		case BIPINITIATOR_PROGRESS:

			UI_DISPLAY(("BIPIA: Progress: %d Bytes", btlbipEvent->parms->data->progressInfo->currPos));

		break;

		default:

			UI_DISPLAY(("BIPIA: Initiator received unknown event = %d",btlbipEvent->parms->event));
			
		}
}

static const char *pBipInd(BipDataType indType)
{
    switch (indType) {
    case BIPIND_HANDLES_DESCRIPTOR: 	return "XML-HandlesDescriptor";
    case BIPIND_IMAGE_DESCRIPTOR:		return "XML-ImageDescriptor";
    case BIPIND_IMG_CAPABILITIES_XML:	return "GetCapabilities XML";
    case BIPIND_IMG_LISTING_XML:		return "GetImagesList XML";
    case BIPIND_IMG_PROPERTIES_XML:	return "GetImageProperties XML";
    default:							return "UNKNOWN";

    }
}

/*-------------------------------------------------------------------------------
 * GetBooleanString()
 */
static const char* GetBooleanString(BOOL booleanType)
{
	if (booleanType == TRUE)
	{
		return "TRUE";
	}
	else
	{
		return "FALSE";
	}
}


/*-------------------------------------------------------------------------------
 * GetBipEncodingString()
 */
static const char* GetBipEncodingString(BtlBipEncoding bipEncoding)
{
    switch( bipEncoding)
    {
        case BTL_BIP_ENCODING_JPEG:     	return "JPEG";
        case BTL_BIP_ENCODING_JPEG2000: return "JPEG2000";
        case BTL_BIP_ENCODING_PNG:      	return "PNG";
        case BTL_BIP_ENCODING_GIF:   	return "GIF";
        case BTL_BIP_ENCODING_BMP:      	return "BMP";
        case BTL_BIP_ENCODING_WBMP:     return "WBMP";
        default: 						return "UNKNOWN";
    }
}

/*-------------------------------------------------------------------------------
 * GetBipTransformationString()
 */
static const char *GetBipTransformationString(BtlBipTranformation bipTransformation)
{
    switch( bipTransformation)
    {
        case BTL_BIP_TRANSFORMATION_NONE:    	return "None";
        case BTL_BIP_TRANSFORMATION_STRETCH:  	return "stretch";
        case BTL_BIP_TRANSFORMATION_FILL:     	return "fill";
        case BTL_BIP_TRANSFORMATION_CROP:     	return "crop";
        default: 								return "RESERVED";
    }
}

static void BipiaInitRxXmlResponseObj(BtlObject *rxResponseObj)
{
	rxResponseObj->objectName = NULL;
	rxResponseObj->objectMimeType = NULL;
	rxResponseObj->objectLocation = BTL_OBJECT_LOCATION_MEM;
	rxResponseObj->location.memLocation.memAddr = (char*)xmlRxObj;
	rxResponseObj->location.memLocation.size    = XML_OBJ_MAX_LEN;

	memset(rxResponseObj->location.memLocation.memAddr, 0, XML_OBJ_MAX_LEN);
}

static void BipiaInitRxImageResponseObj(BtlObject *rxResponseObj, const U8 *imageName, const U8 *imageFullPath)
{
	if (imageName != NULL)
	{
		rxResponseObj->objectName = (BtlUtf8*)imageName;
	}
	else
	{
		rxResponseObj->objectName = NULL;
	}
	
	rxResponseObj->objectMimeType = NULL;
	rxResponseObj->objectLocation = BTL_OBJECT_LOCATION_FS;
	rxResponseObj->location.fsLocation.fsPath = (BtlUtf8*)imageFullPath;
}

static BOOL BipiaConvertEncodingStr(const char *encodingStr, BtlBipEncoding *encoding)
{
	if (OS_StriCmp(encodingStr, "JPEG") == 0)
	{
		*encoding = BTL_BIP_ENCODING_JPEG;
	}
	else if (OS_StriCmp(encodingStr, "JPEG2000") == 0)
	{
		*encoding = BTL_BIP_ENCODING_JPEG2000;
	}
	else if (OS_StriCmp(encodingStr, "PNG") == 0)
	{
		*encoding = BTL_BIP_ENCODING_PNG;
	}
	else if (OS_StriCmp(encodingStr, "GIF") == 0)
	{
		*encoding = BTL_BIP_ENCODING_GIF;
	}
	else if (OS_StriCmp(encodingStr, "BMP") == 0)
	{
		*encoding = BTL_BIP_ENCODING_BMP;
	}
	else if (OS_StriCmp(encodingStr, "WBMP") == 0)
	{
		*encoding = BTL_BIP_ENCODING_WBMP;
	}
	else
	{
		UI_DISPLAY(("BIPIA: Invalid Encoding (%s)", encodingStr));
		return FALSE;
	}

	return TRUE;		
}

static const char *pBipOp(BipOp bipOp)
{
    switch (bipOp) {
    case BIPOP_GET_CAPABILITIES:
        return "GetCapabilities XML";

    case BIPOP_GET_IMAGES_LIST:
        return "GetImagesList XML";

    case BIPOP_GET_IMAGE_PROPERTIES:
        return "GetImageProperties XML";

    case BIPOP_PUT_IMAGE:
        return "PutImage Op";

    case BIPOP_PUT_LINKED_ATTACHMENT:
        return "PutLinkedAttachment Op";

    case BIPOP_PUT_LINKED_THUMBNAIL:
        return "PutLinkedThumbnail Op";

    case BIPOP_GET_IMAGE:
        return "GetImage Op";

    case BIPOP_GET_LINKED_THUMBNAIL:
        return "GetLinkedThumbnail Op";

    case BIPOP_GET_LINKED_ATTACHMENT:
        return "GetLinkedAttachment Op";

    case BIPOP_GET_MONITORING_IMAGE:
        return "GetMonitoringImage Op";

    case BIPOP_REMOTE_DISPLAY:
    case BIPOP_DELETE_IMAGE:
    case BIPOP_START_PRINT:
    case BIPOP_START_ARCHIVE:
    case BIPOP_GETSTATUS:
    default:
        return "UNKNOWN";
 
    }
}

static const char *pBipEvent(BipEvent bipEvent)
{
    switch( bipEvent )
    {
		case BIPINITIATOR_DATA_REQ: 				return "BIPINITIATOR_DATA_REQ";
		case BIPINITIATOR_DATA_IND: 				return "BIPINITIATOR_DATA_IND";
		case BIPINITIATOR_RESPONSE: 				return "BIPINITIATOR_RESPONSE";
		case BIPINITIATOR_COMPLETE: 				return "BIPINITIATOR_COMPLETE";
		case BIPINITIATOR_FAILED: 				return "BIPINITIATOR_FAILED";
		case BIPINITIATOR_ABORT: 				return "BIPINITIATOR_ABORT";
		case BIPINITIATOR_CONNECTED: 			return "BIPINITIATOR_CONNECTED";
		case BIPINITIATOR_DISCONNECTED: 			return "BIPINITIATOR_DISCONNECTED";
		case BIPINITIATOR_DISABLED: 				return "BIPINITIATOR_DISABLED";
		case BIPINITIATOR_AUTH_CHALLENGE_RCVD: 	return "BIPINITIATOR_AUTH_CHALLENGE_RCVD";
		case BIPINITIATOR_PROGRESS: 				return "BIPINITIATOR_PROGRESS";
		case BIPRESPONDER_DATA_REQ: 			return "BIPRESPONDER_DATA_REQ";
		case BIPRESPONDER_DATA_IND: 				return "BIPRESPONDER_DATA_IND";
		case BIPRESPONDER_REQUEST: 				return "BIPRESPONDER_REQUEST";
		case BIPRESPONDER_COMPLETE: 			return "BIPRESPONDER_COMPLETE";
		case BIPRESPONDER_FAILED: 				return "BIPRESPONDER_FAILED";
		case BIPRESPONDER_ABORT: 				return "BIPRESPONDER_ABORT";
		case BIPRESPONDER_CONTINUE: 			return "BIPRESPONDER_CONTINUE";
		case BIPRESPONDER_CONNECTED: 			return "BIPRESPONDER_CONNECTED";
		case BIPRESPONDER_DISCONNECTED: 		return "BIPRESPONDER_DISCONNECTED";
		case BIPRESPONDER_DISABLED: 				return "BIPRESPONDER_DISABLED";
		case BIPRESPONDER_PROGRESS: 			return "BIPRESPONDER_PROGRESS";
		default: 									return "INVALID";
    }
}

/*-------------------------------------------------------------------------------
 * DisplayCapabilities()
 *
 *    Display the parsed Capabilities as stored in the global 'elementsDataCaps'.
 */
static void DisplayCapabilities(const BtlBipintCapabilitiesElements *elementsDataCaps)
{
	U8 k;

	UI_DISPLAY(("BIPIA: Image Capabilities"));

	UI_DISPLAY(("BIPIA: #Formats =%d, numAttachmentFormats =%d",
				elementsDataCaps->numImageFormats, elementsDataCaps->numAttachmentFormats));

	UI_DISPLAY(("BIPIA: imageFormatsOverflow = %s, attachmentFormatsOverflow = %s",
				GetBooleanString(elementsDataCaps->imageFormatsOverflow),
				GetBooleanString(elementsDataCaps->attachmentFormatsOverflow)));
		
	UI_DISPLAY(("BIPIA: Preferred: Encoding = %s", GetBipEncodingString(elementsDataCaps->preferredFormat.encoding)));

	if (elementsDataCaps->preferredFormat.filledPixel == TRUE)
	{
		UI_DISPLAY(("BIPIA: Preferred: wSmall=%d, hSmall=%d",
					elementsDataCaps->preferredFormat.pixel.widthSmall, elementsDataCaps->preferredFormat.pixel.heightSmall));

		UI_DISPLAY(("BIPIA: Preferred: wLarge=%d, hLarge=%d",
					elementsDataCaps->preferredFormat.pixel.widthLarge, elementsDataCaps->preferredFormat.pixel.heightLarge));
	}
	
	UI_DISPLAY(("BIPIA: Preferred: maxsize=%d, transformation=%s",
				elementsDataCaps->preferredFormat.maxSize,
				GetBipTransformationString(elementsDataCaps->preferredFormat.transformation) ));

	UI_DISPLAY(("BIPIA: ImageFormats:"));

	for (k=0; k < elementsDataCaps->numImageFormats; k++)
	{
		UI_DISPLAY(("BIPIA: Supported: Enc = %s", GetBipEncodingString(elementsDataCaps->imageFormats[k].encoding)));
		
		if (elementsDataCaps->imageFormats[k].filledPixel == TRUE)
		{
			UI_DISPLAY(("BIPIA: Supported: wSmall=%d, hSmall=%d",
						elementsDataCaps->imageFormats[k].pixel.widthSmall, elementsDataCaps->imageFormats[k].pixel.heightSmall));

			UI_DISPLAY(("BIPIA: wLarge=%d, hLarge=%d",
						elementsDataCaps->imageFormats[k].pixel.widthLarge, elementsDataCaps->imageFormats[k].pixel.heightLarge));
		}

		UI_DISPLAY(("BIPIA: maxsize=%d", elementsDataCaps->imageFormats[k].maxSize));
	}

	for (k=0; k < elementsDataCaps->numAttachmentFormats; k++)
	{
		UI_DISPLAY(("BIPIA: AttachmentFormat: contentType = %s, charset = %s",
				elementsDataCaps->attachmentFormats[k].contentType,
				elementsDataCaps->attachmentFormats[k].charset));
	}

	UI_DISPLAY(("BIPIA: FilterParameters: Created=%d, Modified=%d,",
			elementsDataCaps->filteringParameters.created,
			elementsDataCaps->filteringParameters.modified));

	UI_DISPLAY(("BIPIA: Encoding=%d, Pixel=%d",
			elementsDataCaps->filteringParameters.encoding,
			elementsDataCaps->filteringParameters.pixel));
}

/*-------------------------------------------------------------------------------
 * DisplayListingInfo()
 *
 *    Display start of the parsed ImagesList as stored in the global 'elementsMetaData'.
 */
static void DisplayListingInfo(const BtlBipintImagesListMetaData *elementsMetaData)
{
	UI_DISPLAY(("BIPIA: Image Listing Info:"));
	UI_DISPLAY(("BIPIA: nrReturnedHandles = %d", elementsMetaData->nbReturnedHandles));
    	UI_DISPLAY(("BIPIA:encoding = %s, pixel = %s",
				GetBooleanString(elementsMetaData->filter.encoding),
				GetBooleanString(elementsMetaData->filter.pixel)));

    	UI_DISPLAY(("BIPIA:created = %s, modified = %s",
				GetBooleanString(elementsMetaData->filter.created), 
				GetBooleanString(elementsMetaData->filter.modified)));
		
	/* XXXX UI_DISPLAY(("BIPIA: Image Listing Info: XmlObjectImageList = %s",elementsMetaData->xmlImgListObjectOffset));*/
}			

/*-------------------------------------------------------------------------------
	* DisplayListingElement()
	*
	*    Display next element of the parsed ImagesList as stored in the global 'elementsMetaData'.
	*    Return TRUE when end of list is not detected yet
	*    Return FALSE when end of list detected.
	*/
static BOOL DisplayListingElement(const BtlBipintImagesListMetaData *elementsMetaData)
{
	BtlBipImagesListing		imageInfo;
	S8 					*offsetTemp;
	BOOL 				moreImages = FALSE; /* function return */
	BtStatus				status;

	OS_MemSet((U8 *)&imageInfo, 0, sizeof( BtlBipImagesListing) );

	/* save old offset for verification later if image handle was found */
	offsetTemp = elementsMetaData->xmlImgListObjectOffset;

	status = BTL_BIPINT_ParseNextFromImagesList((BtlBipintImagesListMetaData*)elementsMetaData, &imageInfo);

	if (status != BT_STATUS_NOT_FOUND)
	{
			moreImages = TRUE;
			
			UI_DISPLAY(("BIPIA: Image Handle = %s", imageInfo.imgHandle));

			UI_DISPLAY(("BIPIA: Created %d-%d-%d, at %d:%d:%d, GMT = %s",
					imageInfo.created.day,
					imageInfo.created.month,
					imageInfo.created.year,
					imageInfo.created.hour,
					imageInfo.created.minute,
					imageInfo.created.second,
					GetBooleanString(imageInfo.created.utcTime)));				

			UI_DISPLAY(("BIPIA: Modified %d-%d-%d, at %d:%d:%d, GMT = %s",
					imageInfo.modified.day,
					imageInfo.modified.month,
					imageInfo.modified.year,
					imageInfo.modified.hour,
					imageInfo.modified.minute,
					imageInfo.modified.second,
					GetBooleanString(imageInfo.modified.utcTime)));
	}
	else
	{
		moreImages = FALSE;
	}

	return (moreImages);
}      

/*-------------------------------------------------------------------------------
 * DisplayProperties()
 *
 *    Display the parsed Image Properties as stored in the global 'elementsDataProps'.
 */
static void DisplayProperties(const BtlBipintImgPropertiesElements *elementsDataProps)
{
	U8 k;

		UI_DISPLAY(("BIPIA: Props: #Variants =%d, #Attachments =%d",
			elementsDataProps->numVariantEncodings, 
			elementsDataProps->numAttachments));
		
		UI_DISPLAY(("BIPIA: Props: variantsOvrflw =%s, attachmentsOvrflw =%s",
			GetBooleanString(elementsDataProps->variantEncodingOverflow),
			GetBooleanString(elementsDataProps->attachmentsOverflow)));

		UI_DISPLAY(("BIPIA: Props: Handle = %s",elementsDataProps->imgHandle));

		UI_DISPLAY(("BIPIA: Props: Native: Enc = %s, wSmall=%d, hSmall=%d",
				GetBipEncodingString(elementsDataProps->nativeEncoding.encoding),
				elementsDataProps->nativeEncoding.pixel.widthSmall,
				elementsDataProps->nativeEncoding.pixel.heightSmall));

		UI_DISPLAY(("BIPIA: Props: Native: wLarge=%d, hLarge=%d, size=%d",
				elementsDataProps->nativeEncoding.pixel.widthLarge,
				elementsDataProps->nativeEncoding.pixel.heightLarge,
				elementsDataProps->nativeEncoding.size));
		
		for (k=0; k < elementsDataProps->numVariantEncodings; k++)
		{
			UI_DISPLAY(("BIPIA: Props: Var Encs: Supported: Encoding = %s", 
						GetBipEncodingString(elementsDataProps->variantEncodings[k].encoding)));

			UI_DISPLAY(("BIPIA: Props: Var Encs: Supported: wSmall = %d, hSmall=%d", 
						elementsDataProps->variantEncodings[k].pixel.widthSmall,
						elementsDataProps->variantEncodings[k].pixel.heightSmall));

			UI_DISPLAY(("BIPIA: Props: Var Encs: Supported: wLarge = %d, hLarge=%d",
						elementsDataProps->variantEncodings[k].pixel.widthLarge,
						elementsDataProps->variantEncodings[k].pixel.heightLarge));

			UI_DISPLAY(("BIPIA: Props: Var Encs: Supported: Max Size: %d", elementsDataProps->variantEncodings[k].maxSize));

			UI_DISPLAY(("BIPIA: Props: Var Encs: Supported: transformation=%s",
					GetBipTransformationString(elementsDataProps->variantEncodings[k].transformation)));
		}

		for (k=0;k<elementsDataProps->numAttachments;k++)
		{
			UI_DISPLAY(("BIPIA: Props : Attachment : Content = %s",
					elementsDataProps->attachments[k].contentType));

			UI_DISPLAY(("BIPIA: Props : Attachment: name=%s, size=%d",
					elementsDataProps->attachments[k].name,
					elementsDataProps->attachments[k].size));
		}			
}


#else /* BTL_CONFIG_BIP == BTL_CONFIG_ENABLED*/


void BIPIA_Init(void)
{
	
	Report(("BIPIA_Init -BTL_CONFIG_BIP is disabled"));
	
}


void BIPIA_Deinit(void)
{
    
	Report(("BIPIA_Deinit -BTL_CONFIG_BIP is disabled"));

}

#endif /* BTL_CONFIG_BIP == BTL_CONFIG_ENABLED */

