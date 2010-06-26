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
*   FILE NAME:      bipr_app.c
*
*   DESCRIPTION:    This file contains the implementation of the BIP Responder sample 
*					application layer of the Neptune platform.
*
*   AUTHOR:         Yoni Shavit
*
\*******************************************************************************/

#include "btl_config.h"

#if BTL_CONFIG_BIP == BTL_CONFIG_ENABLED

#include <string.h>
#include "osapi.h"
#include "btl_common.h"
#include "debug.h"
#include "../app_main.h"

#include "btl_biprsp.h"
#include "bthal_config.h"
#include "bthal_fs.h"

#include "bttypes.h"
#include "btl_config.h"
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

#define BIPRA_VERIFY(condition, msg, returnValue)		\
			if (!condition)							\
			{										\
				UI_DISPLAY(msg);					\
				return returnValue;					\
			}

#define BIPRA_VERIFY_AND_CLEANUP(condition, msg, returnValue)		\
			if (!condition)											\
			{														\
				UI_DISPLAY(msg);									\
				retVal = returnValue;									\
				goto CLEANUP;										\
			}

#define	BIPRA_DEFAULT_IMAGE_PATH				"/bip/"
#define XML_OBJ_MAX_LEN 						((U16) 1000) /* Max number of bytes for the globalXmlObj storage. */
#define BIPRA_DEFAULT_PUSHED_IMAGE_HANDLE	"1234567"
#define IMG_HANDLE_FILE_NAME_PREFIX 			"IMG_"
#define IMG_HANDLE_FILE_EXTENSION 				".jpg"
#define IMG_HANDLE_FILE_NAME_PREFIX_LEN 		(sizeof(IMG_HANDLE_FILE_NAME_PREFIX)-1)
#define IMG_PROPERTIES_SUFFIX 					"-prop.txt"
#define IMG_PROPERTIES_MAX_LINE_LEN			50
#define IMG_DESCRIPTOR_MAX_LEN				1000
#define IMG_THUMBNAIL_SUFFIX 					"-thumb"
#define IMG_THUMBNAIL_WIDTH					160
#define IMG_THUMBNAIL_HEIGHT					120

typedef U32 ImageDescriptorElementsMask;

#define IMAGE_DESCRIPTOR_ELEMENT_MASK_ENCODING 			((U32) 0x00000001UL)
#define IMAGE_DESCRIPTOR_ELEMENT_MASK_PIXEL 		 		((U32) 0x00000002UL)
#define IMAGE_DESCRIPTOR_ELEMENT_MASK_SIZE 		 		((U32) 0x00000004UL)
#define IMAGE_DESCRIPTOR_ELEMENT_MASK_MAX_SIZE 		 	((U32) 0x00000008UL)
#define IMAGE_DESCRIPTOR_ELEMENT_MASK_TRANSFORMATION 	((U32) 0x00000010UL)

typedef enum
{
	PIXEL_RANGE_TYPE_ANY,
	PIXEL_RANGE_TYPE_RANGE,
	PIXEL_RANGE_TYPE_THUMBNAIL,
	PIXEL_RANGE_TYPE_FIXED_ASPECT_RATIO,
	PIXEL_RANGE_TYPE_FIXED_SIZE
} PixelRangeType;

static BtlBiprspContext *biprspContext = NULL;
static U8				pushedImageHandle[7 + 1];
static U8				xmlTxObj[XML_OBJ_MAX_LEN + 1]; /* memory area for receiving an XML object for the Initiator. */
static BtlObject		transferredImageObj;
static char			currentFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
static char			currentFullFileName[BTHAL_FS_MAX_PATH_LENGTH + 1];
static BtlBipPixel		pushedImagePixelValues;
static U8				tempImageDescriptor[IMG_DESCRIPTOR_MAX_LEN + 1];
static BOOL			autoRequestThumbnail = FALSE;

static int initState = 0;

/*******************************************************************************
 *
 * BIPR External Function prototypes
 *
 ******************************************************************************/


void BIPRA_Init(void);
void BIPRA_Deinit(void);

void BIPRA_Create(void);
void BIPRA_Destroy(void);
void BIPRA_Enable(void);
void BIPRA_Disable(void);
void BIPRA_Abort(void);
void BIPRA_GetSecurityLevel(void);
void BIPRA_SetSecurityLevel(void);

void BIPRA_GetPushedImageHandle(void);
void BIPRA_SetPushedImageHandle(void);

void BIPRA_GetAutoRequestThumbnailSetting(void);
void BIPRA_SetAutoRequestThumbnailSetting(void);

static void BipraHandleConnected(const BtlBiprspEvent *btlbipEvent);

static void BipraInitTxXmlResponseObj(BtlObject *txResponseObj, U8 *txXmlObj);
static void BipraInitRxImageResponseObj(	BtlObject *rxObj, 
												const char *imageFileName,
												const char *imageFullFileName);

static BOOL BipraHandleRequestPutImage(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRequestPutLinkedThumbnail(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRequestGetCapabilities(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRequestGetImagesList(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRequestGetImageProperties(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRequestGetLinkedThumbnail(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRespGetImage(const BtlBiprspEvent *btlbipEvent);
static BOOL BipraHandleRequestGetMonitoringImage(const BtlBiprspEvent *btlbipEvent);
static void BipraHandleRequest(const BtlBiprspEvent *btlbipEvent);
static void BipraHandleComplete(const BtlBiprspEvent *btlbipEvent);

static void BIPRSPA_Callback(const BtlBiprspEvent *btlbipEvent);

static const char *pBipOp(BipOp bipOp);
static const char *pBipInd(BipDataType indType);
static const char *pBipEvent(BipEvent bipEvent);
static BOOL IsImageHandleValid(const char *handle);
static BOOL BipraIsThumbnailPixelValues(const BtlBipPixel *pixel);
static BOOL BipraCheckPushedImageProperties(	ImageDescriptorElementsMask	mask,
														BtlBipEncoding					encoding,
														BtlBipPixel					*pixel);

static PixelRangeType BipraGetPixelRangeType(const BtlBipPixel *pixelRange);

static BOOL BipraCheckIfRequestedPixelRangeMatchesImage(
													const char			*imageHandle,
													PixelRangeType 		pixelRangeType, 
													const BtlBipPixel 		*pixelRange);

static void	BipraDebugSaveImageDescriptor(const U8 *imageDescriptor);

static BOOL BipraRespCheckRequestedImageProperties(	const char	*imageHandle,
																const U8* 	imageDescriptor,
																BOOL 		*thumbnailVersionRequested);

static BOOL BipraVerifyDefaultSettings(void);
static void BipraInitRequestData(void);
static BOOL BipraInitContext(void);
static BOOL BipraCreateContext(void);

static BOOL CreateImageListingFromDirectory(	BtlBiprspContext 	*bipContext,
													U8				*fsPath,
													U16				offset,
													U16 				requestedHandles,
													U8  				**xmlObj,
													U16				*numOfHandles);

static BOOL NextImgHandleFile(BthalFsDirDesc   fd, char  *imgHandle);

static BOOL BipraGetNextMonitoringImageHandle(char *handle);
static void BipraCompletePutImageReponse(void);
static BOOL BipraGeneratetImageFileProperties(const char *fullPropertiesFileName, U16 width, U16 height);
static BOOL BipraGetImageFileProperties(const char *imageFileName, U16 *width, U16 *height);
static const char *BipraGetPixelRangeStr(PixelRangeType	rangeType);
static const char *BipraGetBipEncodingString(BtlBipEncoding bipEncoding);
static BtlBipEncoding BipraGetBipEncodingFromString(U8* str);
static void BipraGetPixelSizesFromString(U8* str, BtlBipPixel *pixelFormat);
static BtlBipTranformation BipraGetBipTransformationFromString(U8* str);
static U8*BipraGetXmlKeyElementValue(U8* descriptor, U8* field, U8* attribute, U16 attrLen, U16 maxLen);
static void BipraParseImageDescriptor(	U8							*descriptor,
											BtlBipEncoding					*encoding,
											BtlBipPixel					*pixel,
											U32							*size,
											U32							*maxSize,
											BtlBipTranformation			*transformation,
											ImageDescriptorElementsMask	*mask);
static BOOL BipraGetImageFileNameFromHandle(const char *handle, char *fileName);
static void BipraGetFullFileName(const char *fileName, char *fullFileName);
static BOOL BipraGetImageFullFileNameFromHandle(const char *handle, char *fullFileName);
static void BipraGetPropertiesFileNameFromImageFileName(const char *imageFileName, char *propertiesFileName);
static BOOL BipraExtractHandleFromFileName(const char* fileName, char *handle);
static BOOL BipraIsImageFileNameValid(const char *imageFileName);
static BOOL BipraGetThumbnailFileNameFromImageFileName(const char *imageFileName, char *thumbnailFileName);
static BOOL BipraGetThumbnailFileNameFromImageHandle(const char *handle, char *thumbnailFileName);

/*---------------------------------------------------------------------------
 *            BIPRA_Init
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the BIP Initiator.
 *            
 *
 * Return:    void
 *
 */
void BIPRA_Init(void)
{
	BtStatus 		status;
	BOOL		result;
	BipService	bipService = BIPSVC_RESPONDER;
	
	initState++;
	
	result = BipraVerifyDefaultSettings();

	if (result == FALSE)
	{
		Report(("BIPRA: Invalid Default BIPRSP Settings"));
		return;
	}
	initState++;

	result = BipraCreateContext();

	if (result == FALSE)
	{
		Report(("BIPRA: Failed Creating BIPRSP"));
		return;
	}

	initState++;
	
	status = BTL_BIPRSP_Enable(biprspContext, NULL, &bipService);
	
	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPRA: BTL_BIPRSP_Enable() Failed(%s)", pBT_Status(status)));
		return;
	}

	initState++;
		
	Report(("BIPRA: BIPRSP Successfully Initialized"));
}


/*---------------------------------------------------------------------------
 *            BIPRA_Deinit
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the BIP Initiator
 *
 * Return:    void
 *
 */
void BIPRA_Deinit(void)
{
	BtStatus status;

	status = BTL_BIPRSP_Disable(biprspContext);

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPRA: BTL_BIPRSP_Disable() Failed(%s)", pBT_Status(status)));
		return;
	}

	status = BTL_BIPRSP_Destroy(&biprspContext);

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPRA: BTL_BIPRSP_Destroy() Failed(%s)", pBT_Status(status)));
		return;
	}

	Report(("BIPRA: BIPRSP Successfully De-Initialized"));
}

/*---------------------------------------------------------------------------
 *            BIPRA_Create
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPRA_Create(void)
{
	BOOL	result;

	result = BipraCreateContext();

	if (result == FALSE)
	{
		Report(("BIPRA: Failed Creating BIPRSP"));
		return;
	}
	
	Report(("BIPRA: BIPRSP Successfully Created"));
}

/*---------------------------------------------------------------------------
 *            BIPRA_Destroy
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Destroy the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPRA_Destroy(void)
{
	BtStatus status;
	
	if (biprspContext == NULL)
	{
		Report(("BIPRA: BIPRSP Single Instance Doesn't Exist"));
		return;
	}

	status = BTL_BIPRSP_Destroy(&biprspContext);	

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPRA: BTL_BIPRSP_Destroy() Failed (%s).", pBT_Status(status)));	
		return;
	}

	Report(("BIPRA: BIPRA_Destroy BIPRSP Successfully Destroyed"));
}

/*---------------------------------------------------------------------------
 *            BIPRA_Enable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enable the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPRA_Enable(void)
{
	BtStatus status;
	BipService	bipService = BIPSVC_RESPONDER;

	Report(("BIPRA: Init State: %d", initState));
		
	status = BTL_BIPRSP_Enable(biprspContext, (BtlUtf8*)"BTL BIP TestEnv", &bipService);

	if (status != BT_STATUS_SUCCESS)
	{
		Report(("BIPRA: BTL_BIPRSP_Enable() Failed (%s).", pBT_Status(status)));	
		return;
	}

	Report(("BIPRA: BIPRSP Successfully Enabled"));
}

/*---------------------------------------------------------------------------
 *            BIPRA_Disable
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disable the BIP Initiator context
 *
 * Return:    void
 *
 */
void BIPRA_Disable(void)
{
	BtStatus status;
	
	status = BTL_BIPRSP_Disable(biprspContext);

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
 *            BIPRA_Abort
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Abort the BIP Initiator operation
 *
 * Return:    void
 *
 */
void BIPRA_Abort(void)
{
	BtStatus status;
   
    status = BTL_BIPRSP_Abort(biprspContext);
	
	if (status == BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: Operation Successfully Aborted"));
	}
	else if (status == BT_STATUS_PENDING)
	{
		UI_DISPLAY(("BIPRA: Wating for operation to complete abort"));
	}
	else
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_Abort() Failed (%s).", pBT_Status(status)));	
	}
}

/*---------------------------------------------------------------------------
 *            BIPRA_GetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Gets security level for the given BIPRSP context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void BIPRA_GetSecurityLevel(void)
{
	BtStatus status;
	BtSecurityLevel level;
	
	status = BTL_BIPRSP_GetSecurityLevel(biprspContext, &level);

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: BIPRA_GetSecurityLevel() Failed(%s).", pBT_Status(status)));
		return;
	}

	switch (level)
	{
		case 0:
			UI_DISPLAY(("BIPRA: Security level: BSL_NO_SECURITY"));
			break;
		case 1:
			UI_DISPLAY(("BIPRA: Security level: BSL_AUTHENTICATION_IN"));
			break;
		case 2:
			UI_DISPLAY(("BIPRA: Security level: BSL_AUTHORIZATION_IN"));
			break;
		case 4:
			UI_DISPLAY(("BIPRA: Security level: BSL_ENCRYPTION_IN"));
			break;
		case 3:
			UI_DISPLAY(("BIPRA: Security level: BSL_AUTHENTICATION_IN"));
			UI_DISPLAY(("BIPRA: and BSL_AUTHORIZATION_IN"));
			break;
		case 5:
			UI_DISPLAY(("BIPRA: Security level: BSL_AUTHENTICATION_IN"));
			UI_DISPLAY(("BIPRA: and BSL_ENCRYPTION_IN"));
			break;
		case 6:
			UI_DISPLAY(("BIPRA: Security level: BSL_AUTHORIZATION_IN"));
			UI_DISPLAY(("BIPRA: and BSL_ENCRYPTION_IN"));
			break;
		case 7:
			UI_DISPLAY(("BIPRA: Security level: BSL_AUTHENTICATION_IN"));
			UI_DISPLAY(("BIPRA: and BSL_AUTHORIZATION_IN"));
			UI_DISPLAY(("BIPRA: and BSL_ENCRYPTION_IN"));
			break;
		default:
			UI_DISPLAY(("BIPRA: Error: Wrong security level"));
			break;
			
	}
}

/*---------------------------------------------------------------------------
 *            BIPRA_SetSecurityLevel
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets security level for the given BIPRSP context.
 *
 * Return:    TRUE if security level is ON, else FALSE.
 *
 */
void BIPRA_SetSecurityLevel(void)
{
	BtStatus 				status;
	LINE_PARSER_STATUS 	lineStatus;
	BtSecurityLevel 		newSecurityLevel;
	
	lineStatus = LINE_PARSER_GetNextU8(&newSecurityLevel, FALSE);
	
	if (lineStatus != LINE_PARSER_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: Failed reading security level argument (%d)", lineStatus));
		return;
	}

	status = BTL_BIPRSP_SetSecurityLevel(biprspContext, &newSecurityLevel);

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_SetSecurityLevel() Failed(%s).", pBT_Status(status)));
		return;
	}
	
	UI_DISPLAY(("BIPRA: Security Level was Successfully set"));
}

void BIPRA_GetPushedImageHandle(void)
{
	UI_DISPLAY(("BIPRA: Pushed Image Handle Is |%s|", pushedImageHandle));
}

void BIPRA_SetPushedImageHandle(void)
{
	LINE_PARSER_STATUS 	lineStatus;
	U8					imageHandle[7 + 1];
	
	lineStatus = LINE_PARSER_GetNextStr(imageHandle, LINE_PARSER_MAX_STR_LEN);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPRA: Failed Reading Image Handle Argument(%d)", lineStatus));
		return;
	}

	if (IsImageHandleValid((char*)imageHandle) == FALSE)
	{
		UI_DISPLAY(("BIPRA: Invalid Image Handle Argument (%s)", imageHandle));
		return;
	}
	
	strcpy((char*)pushedImageHandle, (char*)imageHandle);

	UI_DISPLAY(("BIPRA: Pushed Image Handle Changed to |%s|", pushedImageHandle));
}

void BIPRA_GetAutoRequestThumbnailSetting(void)
{
	BtStatus		status;
	BOOL		autoRequestValue;
	
	status = BTL_BIPRSP_GetAutoRequestLinkedThumbnail(biprspContext, &autoRequestValue);
	
	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_GetAutoRequestLinkedThumbnail Failed (%s)", pBT_Status(status)));
		return;
	}
	
	UI_DISPLAY(("BIPRA: Auto Request Thumbnail Set To: |%s|", (autoRequestThumbnail ? ("TRUE") : ("FALSE"))));
}

void BIPRA_SetAutoRequestThumbnailSetting(void)
{
	BtStatus				status;
	LINE_PARSER_STATUS 	lineStatus;
	BOOL				autoRequestUserValue;
	
	lineStatus = LINE_PARSER_GetNextBool(&autoRequestUserValue);

	if (LINE_PARSER_STATUS_SUCCESS != lineStatus)
	{
		UI_DISPLAY(("BIPRA: Failed Reading Auto Request Setting (%d)", lineStatus));
		return;
	}

	status = BTL_BIPRSP_SetAutoRequestLinkedThumbnail(biprspContext, autoRequestUserValue);
	
	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_SetAutoRequestLinkedThumbnail Failed (%s)", pBT_Status(status)));
		return;
	}
	
	autoRequestThumbnail = autoRequestUserValue;

	UI_DISPLAY(("BIPRA: Successfully Set Auto Request Thumbnail to |%s|", autoRequestThumbnail ? ("TRUE") : ("FALSE")));
}

static void BipraInitTxXmlResponseObj(BtlObject *txResponseObj, U8 *txXmlObj)
{
	txResponseObj->objectName = NULL;
	txResponseObj->objectMimeType = NULL;
	txResponseObj->objectLocation = BTL_OBJECT_LOCATION_MEM;
	txResponseObj->location.memLocation.memAddr = (char*)txXmlObj;
	txResponseObj->location.memLocation.size    = OS_StrLen((char*)txXmlObj);
}

static void BipraInitRxImageResponseObj(	BtlObject *rxObj, 
												const char *imageFileName,
												const char *imageFullFileName)
{
	rxObj->objectMimeType = NULL;
	rxObj->objectLocation = BTL_OBJECT_LOCATION_FS;
	rxObj->objectName = (BtlUtf8*)imageFileName;
	rxObj->location.fsLocation.fsPath = (BtlUtf8*)imageFullFileName;
}

static BOOL BipraHandleRequestPutImage(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus						status;
	BOOL						result;
	ImageDescriptorElementsMask	mask = 0;
	BtlBipEncoding					encoding;
	BOOL						retVal;
	
	/* 
		PutImage request must comply with the following:
		1. Have an image descriptor
		2. Image descriptor must have a Pixel element
		3. Pixel values must be fixed
	*/
	if (btlbipEvent->parms->data->i.iIDsc.buff == NULL)
	{
		Report(("BIPRA: Missing mandatory image descriptor in PutImage request"));
		return FALSE;
	}

	BipraDebugSaveImageDescriptor(btlbipEvent->parms->data->i.iIDsc.buff);

	BipraParseImageDescriptor(	btlbipEvent->parms->data->i.iIDsc.buff, 
								&encoding, 
								&pushedImagePixelValues, 
								0, 
								0, 
								0, 
								&mask);

	result = BipraCheckPushedImageProperties(mask, encoding, &pushedImagePixelValues);
	BIPRA_VERIFY_AND_CLEANUP((result == TRUE), ("BIPRA: BipraCheckPushedImageProperties Failed, Rejecting PushImage Request"), FALSE);

	BipraGetImageFileNameFromHandle((char*)pushedImageHandle, currentFileName);
	BipraGetFullFileName(currentFileName, currentFullFileName);
	
	Report(("BIPRA: Pushed Image will be saved in %s", currentFullFileName));
	
	BipraInitRxImageResponseObj(&transferredImageObj, currentFileName, currentFullFileName);
	
	status = BTL_BIPRSP_RespondPutImageWithObj(biprspContext, TRUE, pushedImageHandle, &transferredImageObj);
	BIPRA_VERIFY_AND_CLEANUP((status == BT_STATUS_SUCCESS), 
					("BIPRA: ERROR: BTL_BIPRSP_RespondPutImageWithObj Failed (%s)", pBT_Status(status)),
					FALSE);
	
	UI_DISPLAY(("BIPRA: Receiving image, please wait........"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Put Image Request"));
	
	status = BTL_BIPRSP_RespondPutImageWithObj(biprspContext, FALSE, NULL, NULL);

	return FALSE;
	
}

static BOOL BipraHandleRequestPutLinkedThumbnail(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus status;
	BOOL	result;
	BOOL	retVal;
	char		*imageHandle = (char*)btlbipEvent->parms->data->r.rThm.imgHandle;

	Report(("BIPRA: Pushed Linked Thumbnail Handle: |%s|", imageHandle));

	result = BipraGetThumbnailFileNameFromImageHandle(imageHandle, currentFileName);
	BIPRA_VERIFY_AND_CLEANUP((result == TRUE), ("BIPRA: ERROR: BipraGetThumbnailFileNameFromImageHandle Failed"), FALSE)

	BipraGetFullFileName(currentFileName, currentFullFileName);

	BipraInitRxImageResponseObj(&transferredImageObj, currentFileName, currentFullFileName);

	Report(("BIPRA: Pushed Image will be saved in %s", currentFullFileName));
		
	status = BTL_BIPRSP_RespondPutLinkedThumbnailWithObj(biprspContext, TRUE, &transferredImageObj);
	BIPRA_VERIFY_AND_CLEANUP((status == BT_STATUS_SUCCESS), 
					("BIPRA: ERROR: BTL_BIPRSP_RespondPutLinkedThumbnailWithObj Failed (%s)", pBT_Status(status)),
					FALSE);
	
	UI_DISPLAY(("BIPRA: Receiving Thumbnail, please wait........"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Put Linked Thumbnail Request"));
	
	status = BTL_BIPRSP_RespondPutLinkedThumbnailWithObj(biprspContext, FALSE, NULL);

	return FALSE;
}

static BOOL BipraHandleRequestGetCapabilities(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus				status;
	BOOL				retVal;
	U8         				*xmlObj;
	BtlBipPreferredFormat	prefFormat;
	BtlBipImageFormats	imageFormat;
	
	/* preferred format. */
	prefFormat.encoding = BTL_BIP_ENCODING_JPEG;
	prefFormat.filledPixel = FALSE;
	
	status = BTL_BIPRSP_BuildCapabilitiesStart(biprspContext, xmlTxObj, XML_OBJ_MAX_LEN, &prefFormat);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_BuildCapabilitiesStart Failed (%s)", pBT_Status(status)), FALSE);
		
	imageFormat.filledPixel			= FALSE;
	imageFormat.pixel.widthSmall     	= 0;
	imageFormat.pixel.heightSmall    	= 0;
	imageFormat.pixel.widthLarge    	= 65535;
	imageFormat.pixel.heightLarge   	= 65535;
	imageFormat.encoding             	= BTL_BIP_ENCODING_JPEG;
	
	status = BTL_BIPRSP_BuildCapabilitiesAddImgFormat(biprspContext, &imageFormat);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_BuildCapabilitiesAddImgFormat Failed (%s)", pBT_Status(status)), FALSE);
	
	status = BTL_BIPRSP_BuildCapabilitiesFinish(biprspContext, NULL, &xmlObj);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_BuildCapabilitiesFinish Failed (%s)", pBT_Status(status)), FALSE);
	
	BipraInitTxXmlResponseObj(&transferredImageObj, xmlObj);
		
	status = BTL_BIPRSP_RespondGetCapabilities(biprspContext, TRUE, &transferredImageObj);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_RespondGetCapabilities Failed (%s)", pBT_Status(status)), FALSE);
	
	UI_DISPLAY(("BIPRA: Started Sending GetCapabilities Response"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Get Capabilities Request"));
	
	status = BTL_BIPRSP_RespondGetCapabilities(biprspContext, FALSE, NULL);

	return FALSE;
}

static BOOL BipraHandleRequestGetImagesList(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus			status;
	BOOL			retVal;
    	U8				*xmlObj = 0;
	BOOL			result;	
	BipImgListingReq    *rList = &btlbipEvent->parms->data->r.rList;
	U16				numOfHandles;
	
	result = CreateImageListingFromDirectory(	biprspContext,
											(U8*)BIPRA_DEFAULT_IMAGE_PATH,
											rList->listStartOffset,
											rList->nbReturnedHandles,
											&xmlObj,
											&numOfHandles);
	BIPRA_VERIFY_AND_CLEANUP((result == TRUE), ("BIPRA: ERROR: CreateImageListingFromDirectory Failed"), FALSE);
	
	BipraInitTxXmlResponseObj(&transferredImageObj, xmlObj);
		
	status = BTL_BIPRSP_RespondGetImagesList(	biprspContext,
												TRUE,
												numOfHandles,
												NULL, /* No different filter applied.*/
												&transferredImageObj);	
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_RespondGetImagesList Failed (%s)", pBT_Status(status)), FALSE);
	
	UI_DISPLAY(("BIPRA: Started Sending GetImagesList Response"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Get Images List Request"));
	
	status = BTL_BIPRSP_RespondGetImagesList(biprspContext, FALSE, 0, NULL, NULL);

	return FALSE;
}

static BOOL BipraHandleRequestGetImageProperties(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus				status;
	BOOL				retVal;
	BOOL				result;
	U16					width = 0;
	U16					height = 0;
	BthalFsStat			fileStat;
	BtFsStatus			fsStatus;
	U8             			*xmlObj;
	BtlBipNativeEncoding	nativeEncoding;
	BtlBipVariantEncoding	variantEncoding;
	const char			*imageHandle = (char*)btlbipEvent->parms->data->r.rProp.imgHandle;

	Report(("BIPRA: Get Image Properties Handle: |%s|", imageHandle));

	BipraGetImageFileNameFromHandle(imageHandle, currentFileName);
	BipraGetFullFileName(currentFileName, currentFullFileName);

	result = BipraGetImageFileProperties(currentFileName, &width, &height);
	BIPRA_VERIFY_AND_CLEANUP((result == TRUE), ("BIPRA: BipraGetImageFileProperties Failed"), FALSE);
	
	fsStatus = BTHAL_FS_Stat((BTHAL_U8*)currentFullFileName, &fileStat);
	BIPRA_VERIFY_AND_CLEANUP(	(fsStatus == BT_STATUS_HAL_FS_SUCCESS), 
					("BIPRA: BTHAL_FS_Stat Failed (%d) for %s", fsStatus, currentFullFileName), FALSE);
	
	status = BTL_BIPRSP_BuildPropertiesStart(	biprspContext,
											xmlTxObj,
											XML_OBJ_MAX_LEN,
											(U8*)imageHandle,
											NULL);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: BTL_BIPRSP_BuildPropertiesStart Failed (%s)", pBT_Status(status)), FALSE);

	nativeEncoding.encoding        	= BTL_BIP_ENCODING_JPEG;
	nativeEncoding.pixel.widthSmall = width;
	nativeEncoding.pixel.widthLarge = nativeEncoding.pixel.widthSmall;
	nativeEncoding.pixel.heightSmall = height;
	nativeEncoding.pixel.heightLarge = nativeEncoding.pixel.heightSmall;
	nativeEncoding.size            = fileStat.size;
	
	status = BTL_BIPRSP_BuildPropertiesAddNativeEncoding(biprspContext, &nativeEncoding);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: BTL_BIPRSP_BuildPropertiesAddNativeEncoding Failed (%s)", pBT_Status(status)), FALSE);

	variantEncoding.encoding = BTL_BIP_ENCODING_JPEG;
	
	variantEncoding.pixel.widthSmall = IMG_THUMBNAIL_WIDTH;
	variantEncoding.pixel.widthLarge = variantEncoding.pixel.widthSmall;
	variantEncoding.pixel.heightSmall = IMG_THUMBNAIL_HEIGHT;
	variantEncoding.pixel.heightLarge = variantEncoding.pixel.heightSmall;
	
	variantEncoding.maxSize = 0;
	variantEncoding.transformation = BTL_BIP_TRANSFORMATION_NONE;

	status = BTL_BIPRSP_BuildPropertiesAddVariantEncoding(biprspContext, &variantEncoding);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: BTL_BIPRSP_BuildPropertiesAddVariantEncoding Failed (%s)", pBT_Status(status)), FALSE);
	
	status = BTL_BIPRSP_BuildPropertiesFinish(biprspContext, &xmlObj);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: BTL_BIPRSP_BuildPropertiesFinish Failed (%s)", pBT_Status(status)), FALSE);

	BipraInitTxXmlResponseObj(&transferredImageObj, xmlObj);

	status = BTL_BIPRSP_RespondGetImageProperties(biprspContext, TRUE, &transferredImageObj);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: BTL_BIPRSP_RespondGetImageProperties Failed (%s)", pBT_Status(status)), FALSE);

	UI_DISPLAY(("BIPRA: Started Sending GetImageProperties Response"));
	
	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Get Images List Request"));
	
	status = BTL_BIPRSP_RespondGetImageProperties(biprspContext, FALSE, NULL);

	return FALSE;
}

static BOOL BipraHandleRequestGetLinkedThumbnail(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus	status;
	BOOL	retVal;
	BOOL	result;
	char		*imageHandle = (char*)btlbipEvent->parms->data->r.rImg.imgHandle;
	
	Report(("BIPRA: Get Linked Thumbnail Handle: |%s|", imageHandle));

	result = BipraGetThumbnailFileNameFromImageHandle(imageHandle, currentFileName);
	BIPRA_VERIFY_AND_CLEANUP((result == TRUE), ("BIPRA: ERROR: BipraGetThumbnailFileNameFromImageHandle Failed"), FALSE);
	
	BipraGetFullFileName(currentFileName, currentFullFileName);
	
	BipraInitRxImageResponseObj(&transferredImageObj, currentFileName, currentFullFileName);
	
	status = BTL_BIPRSP_RespondGetLinkedThumbnail(biprspContext, TRUE, &transferredImageObj);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_RespondGetImagesList Failed (%s)", pBT_Status(status)), FALSE);
	
	UI_DISPLAY(("BIPRA: Started Sending GetLinkedThumbnail Response"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Get Linked Thumbnail Request"));
	
	status = BTL_BIPRSP_RespondGetLinkedThumbnail(biprspContext, FALSE, NULL);

	return FALSE;
}

static BOOL BipraHandleRespGetImage(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus	status;
	BOOL	retVal;
	BOOL	isRequestedImageSupported = FALSE;
	BOOL	thumbnailVersionRequested = FALSE;
	char		*imageHandle = (char*)btlbipEvent->parms->data->r.rImg.imgHandle;
						
	Report(("BIPRA: Get Image Handle: |%s|", imageHandle));
	
	isRequestedImageSupported = BipraRespCheckRequestedImageProperties(	imageHandle,
																		btlbipEvent->parms->data->i.iIDsc.buff, 
																		&thumbnailVersionRequested);
	BIPRA_VERIFY_AND_CLEANUP(	(isRequestedImageSupported == TRUE), 
					("BIPRA: Rejecting Get Image Request - Requested Image not supported"), 
					FALSE);

	if (thumbnailVersionRequested == FALSE)		
	{
		BipraGetImageFileNameFromHandle(imageHandle, currentFileName);
	}
	else
	{
		BipraGetThumbnailFileNameFromImageHandle(imageHandle, currentFileName);
	}

	BipraGetFullFileName(currentFileName, currentFullFileName);

	BipraInitRxImageResponseObj(&transferredImageObj, currentFileName, currentFullFileName);
	
	status = BTL_BIPRSP_RespondGetImage(biprspContext, TRUE, &transferredImageObj);
	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_RespondGetImage Failed (%s)", pBT_Status(status)), FALSE);
	
	UI_DISPLAY(("BIPRA: Started Sending GetImage Response"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Get Image Request"));
	
	status = BTL_BIPRSP_RespondGetImage(biprspContext, FALSE, NULL);

	return FALSE;
}

static BOOL BipraHandleRequestGetMonitoringImage(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus status;
	BOOL	retVal;
	char		handle[8];
	BOOL	result;

	Report(("BIPRA: StoreFlag = %d", btlbipEvent->parms->data->r.rMon.storeFlag));

	result = BipraGetNextMonitoringImageHandle(handle);
	BIPRA_VERIFY_AND_CLEANUP((result == TRUE), ("BIPRA: ERROR: BipraGetNextMonitoringImageHandle Failed"), FALSE);

	Report(("BIPRA:Next Monitoring Image Handle Is %s", handle));
	
	BipraGetThumbnailFileNameFromImageHandle(handle, currentFileName);
	BipraGetFullFileName(currentFileName, currentFullFileName);

	Report(("BIPRA: Next Moniroting Image File Name Is: |%s|", currentFullFileName));

	BipraInitRxImageResponseObj(&transferredImageObj, currentFileName, currentFullFileName);
		
	transferredImageObj.objectName = (BtlUtf8*)currentFileName;
		
	if (btlbipEvent->parms->data->r.rMon.storeFlag == 0x00)
	{
		status = BTL_BIPRSP_RespondGetMonitoringImage(biprspContext, TRUE, NULL, &transferredImageObj);
	}
	else
	{
		status = BTL_BIPRSP_RespondGetMonitoringImage(biprspContext, TRUE, (U8*)handle, &transferredImageObj);
	}

	BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
					("BIPRA: Error: BTL_BIPRSP_RespondGetMonitoringImage Failed (%s)", pBT_Status(status)), FALSE);
	
	UI_DISPLAY(("BIPRA: Sending Monitoring Image"));

	return TRUE;

CLEANUP:

	Report(("BIPRA: Rejecting Get Monitoring Image Request"));
	
	status = BTL_BIPRSP_RespondGetMonitoringImage(biprspContext, FALSE, NULL, NULL);

	return FALSE;
}

static void BipraHandleRequest(const BtlBiprspEvent *btlbipEvent)
{
	BOOL	result;
	
	Report(("BIPRA: Received Request (%s)", pBipOp(btlbipEvent->parms->data->bipOp)));

	BipraInitRequestData();
	
	switch(btlbipEvent->parms->data->bipOp)
	{
		case BIPOP_PUT_IMAGE:
	
			result = BipraHandleRequestPutImage(btlbipEvent);
		
		break;
	
		case BIPOP_PUT_LINKED_THUMBNAIL:

			result = BipraHandleRequestPutLinkedThumbnail(btlbipEvent);
			
		break;
		
		case BIPOP_GET_LINKED_THUMBNAIL:

			result = BipraHandleRequestGetLinkedThumbnail(btlbipEvent);
		
		break;
	
		case BIPOP_GET_MONITORING_IMAGE:

			result = BipraHandleRequestGetMonitoringImage(btlbipEvent);
			
		break;

		case BIPOP_GET_IMAGES_LIST:

			result = BipraHandleRequestGetImagesList(btlbipEvent);

		break;
		
		case BIPOP_GET_IMAGE_PROPERTIES:

			result = BipraHandleRequestGetImageProperties(btlbipEvent);

		break;

		case BIPOP_GET_IMAGE:

			result = BipraHandleRespGetImage(btlbipEvent);

		break;
	
		case BIPOP_GET_CAPABILITIES:

			result = BipraHandleRequestGetCapabilities(btlbipEvent);

		break;
	}
}

static void BipraHandleComplete(const BtlBiprspEvent *btlbipEvent)
{	
	switch( btlbipEvent->parms->data->bipOp )
	{
		case BIPOP_CONNECT:

		break;
	
		case BIPOP_PUT_IMAGE:

			Report(("BIPRA: Received Image is in |%s|", currentFullFileName));
			
			BipraCompletePutImageReponse();
	
		break;
	
		case BIPOP_PUT_LINKED_THUMBNAIL:
	
			Report(("Received Thumbnail is in |%s|", currentFullFileName));

		break;
	
		case BIPOP_GET_IMAGE:
		case BIPOP_GET_LINKED_THUMBNAIL:
		case BIPOP_GET_CAPABILITIES:
		case BIPOP_GET_IMAGES_LIST:
		case BIPOP_GET_IMAGE_PROPERTIES:
			
			Report(("BIPRA: %s Complete status = %d", pBipOp(btlbipEvent->parms->data->bipOp), btlbipEvent->parms->status));

		break;

		default:
			
			Report(("Responder Operation Complete status = %d", btlbipEvent->parms->status));
			
	}
}

static void BipraHandleConnected(const BtlBiprspEvent *btlbipEvent)
{
	BtStatus	status;
	BD_ADDR	bdAddr;
	char 	addr[BDADDR_NTOA_SIZE];
	
	UI_DISPLAY(("BIPRA: BIPRSP CONNECTED on channel = %x", btlbipEvent->parms->channel));
		
	status = BTL_BIPRSP_GetConnectedDevice(biprspContext, &bdAddr);
		
	if (status == BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: Connected device: %s.", bdaddr_ntoa(&(bdAddr), addr)));
	}
	else
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_GetConnectedDevice Failed (%s)", pBT_Status(status)));
	}
}

/*---------------------------------------------------------------------------
 *            BIPRSPA_Callback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle the BTL_BIPRSP events
 *
 * Return:    void
 *
 */
static void BIPRSPA_Callback(const BtlBiprspEvent *btlbipEvent)
{
	UI_DISPLAY(("BIPRA: BIPRSPA_Callback: Event: %s, Op:%s", 
				pBipEvent(btlbipEvent->parms->event), pBipOp(btlbipEvent->parms->data->bipOp)));
		
	switch (btlbipEvent->parms->event)
	{
		case BIPRESPONDER_DATA_REQ:	

			/* DO Nothing */
			
		break;
			
		case BIPRESPONDER_DATA_IND:
			
			/* DO Nothing */

		break;

		case BIPRESPONDER_REQUEST:

			BipraHandleRequest(btlbipEvent);

		break;

		case BIPRESPONDER_COMPLETE:

			BipraHandleComplete(btlbipEvent);

		break;

		case BIPRESPONDER_FAILED:

			UI_DISPLAY(("BIPRA: BTL BIPRSP Session Failed, status = %d", btlbipEvent->parms->status));

		break;
		
		case BIPRESPONDER_ABORT:

			UI_DISPLAY(("BIPRA: BTL BIPRSP OBEX session aborted, response code = %d", btlbipEvent->parms->status));

		break;
		
		case BIPRESPONDER_CONTINUE:

			/* DO Nothing */

		break;
		
		case BIPRESPONDER_CONNECTED:
			
			BipraHandleConnected(btlbipEvent);

		break;
			
		case BIPRESPONDER_DISCONNECTED:

			UI_DISPLAY(("BIPRA: BTL BIPRSP DISCONNECTED channel = %x", btlbipEvent->parms->channel));

		break;
			
		case BIPRESPONDER_DISABLED:

			UI_DISPLAY(("BIPRA: BIPRSP Disabled"));
			
		break;
		
		case BIPRESPONDER_PROGRESS:

			UI_DISPLAY(("BIPRA: Progress: %d Bytes", btlbipEvent->parms->data->progressInfo->currPos));

		break;
			
		default:

			UI_DISPLAY(("BIPRA: BIPRSPA_Callback: Error: Unexpected Event (%d)", btlbipEvent->parms->event));
			
	}
}

static const char *pBipOp(BipOp bipOp)
{
    switch (bipOp) {
    case BIPOP_GET_CAPABILITIES: 			return "Get Capabilities";
    case BIPOP_GET_IMAGES_LIST:        		return "Get Images List";
    case BIPOP_GET_IMAGE_PROPERTIES:  		return "Get Image Properties";
    case BIPOP_PUT_IMAGE:        			return "Put Image";
    case BIPOP_PUT_LINKED_THUMBNAIL:        	return "Put Linked Thumbnail";
    case BIPOP_GET_IMAGE:        			return "Get Image";
    case BIPOP_GET_LINKED_THUMBNAIL:        	return "Get Linked Thumbnail";
    case BIPOP_GET_LINKED_ATTACHMENT:      return "Get Linked Attachment";
    case BIPOP_GET_MONITORING_IMAGE:        return "Get Monitoring Image";
	
    case BIPOP_PUT_LINKED_ATTACHMENT:
    case BIPOP_REMOTE_DISPLAY:
    case BIPOP_DELETE_IMAGE:
    case BIPOP_START_PRINT:
    case BIPOP_START_ARCHIVE:
    case BIPOP_GETSTATUS:
    default:
        return "UNEXPECTED";
//        break;
    }
}

static const char *pBipInd(BipDataType indType)
{
	switch (indType)
	{
	    	case BIPIND_HANDLES_DESCRIPTOR: 		return "XML-HandlesDescriptor";
		case BIPIND_IMAGE_DESCRIPTOR:		return "XML-ImageDescriptor";
		case BIPIND_IMG_CAPABILITIES_XML:	return "GetCapabilities XML";
		case BIPIND_IMG_LISTING_XML:			return "GetImagesList XML";
		case BIPIND_IMG_PROPERTIES_XML:        return "GetImageProperties XML";
		 default: 							return "UNKNOWN";
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

static BOOL IsImageHandleValid(const char *handle)
{
	U32 i;

	if (handle[7] != '\0')
	{
		return FALSE;
	}
	
	if (OS_StrLen(handle) != 7)
	{
		return FALSE;
	}

	for (i = 0; i < OS_StrLen(handle); ++i)
	{
		if ((handle[i] < '0') || (handle[i] > '9'))
		{
			return FALSE;
		}
	}

	return TRUE;
}

static BOOL BipraIsThumbnailPixelValues(const BtlBipPixel *pixel)
{
	if (	(pixel->heightSmall == pixel->heightLarge) &&
		(pixel->widthSmall == pixel->widthLarge) &&
		(pixel->heightSmall == IMG_THUMBNAIL_HEIGHT) &&
		(pixel->widthSmall == IMG_THUMBNAIL_WIDTH))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static BOOL BipraCheckPushedImageProperties(	ImageDescriptorElementsMask	mask,
														BtlBipEncoding					encoding,
														BtlBipPixel					*pixel)
{
	if ((mask & IMAGE_DESCRIPTOR_ELEMENT_MASK_ENCODING) != IMAGE_DESCRIPTOR_ELEMENT_MASK_ENCODING)
	{
		Report(("BIPRA: Missing mandatory Encoding value in Image Descriptor in PutImage request"));
		return FALSE;
	}

	if ((mask & IMAGE_DESCRIPTOR_ELEMENT_MASK_PIXEL) != IMAGE_DESCRIPTOR_ELEMENT_MASK_PIXEL)
	{
		Report(("BIPRA: Missing mandatory pixel values in Image Descriptor in PutImage request"));
		return FALSE;
	}

	if (encoding != BTL_BIP_ENCODING_JPEG)
	{
		Report(("BIPRA: Unsupported encoding (%s) in PutImage Request", BipraGetBipEncodingString(encoding)));
		return FALSE;
	}
	
	if (	(pixel->heightSmall != pixel->heightLarge) || (pixel->widthSmall != pixel->widthLarge))
	{
		Report(("BIPRA: Invalid values (%d*%d - %d*%d) in PutImage",
				pixel->widthSmall, pixel->heightSmall, pixel->widthLarge, pixel->heightLarge));
		return FALSE;
	}

	return TRUE;
}

static PixelRangeType BipraGetPixelRangeType(const BtlBipPixel *pixelRange)
{
	if (pixelRange == 0)
	{
		return PIXEL_RANGE_TYPE_ANY;
	}

	if (	(pixelRange->widthSmall) == (pixelRange->widthLarge) &&
      		(pixelRange->heightSmall) == (pixelRange->heightLarge))
	{
		if (pixelRange->widthSmall == 0)
		{
			return PIXEL_RANGE_TYPE_ANY;
		}
		else if (	(pixelRange->widthSmall == IMG_THUMBNAIL_WIDTH) &&
				(pixelRange->heightSmall == IMG_THUMBNAIL_HEIGHT))
		{
			return PIXEL_RANGE_TYPE_THUMBNAIL;
		}
		else
		{
			return PIXEL_RANGE_TYPE_FIXED_SIZE;
		}
	}
	else
	{
		if (pixelRange->heightSmall == pixelRange->heightLarge)
		{
			return PIXEL_RANGE_TYPE_FIXED_ASPECT_RATIO;
		}
		else
		{
			return PIXEL_RANGE_TYPE_RANGE;
		}
	}
}

static BOOL BipraCheckIfRequestedPixelRangeMatchesImage(
													const char			*imageHandle,
													PixelRangeType 		pixelRangeType, 
													const BtlBipPixel 		*pixelRange)
{
	BtlBipPixel	tempPixelRange = *pixelRange;
	char			imageFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	U16			width;
	U16			height;
	
	switch (pixelRangeType)
	{
		case PIXEL_RANGE_TYPE_ANY: 

			return TRUE;
					
		case PIXEL_RANGE_TYPE_RANGE:

		break;
			
		case PIXEL_RANGE_TYPE_THUMBNAIL:

			return TRUE;
		
		case PIXEL_RANGE_TYPE_FIXED_ASPECT_RATIO:

			Assert(pixelRange->widthLarge != 0);

			tempPixelRange.heightSmall = (U16)((	tempPixelRange.widthSmall * tempPixelRange.heightLarge) / 
												tempPixelRange.widthLarge);

		case PIXEL_RANGE_TYPE_FIXED_SIZE:

		break;

		default: 

			Assert(0);
			
	}

	if (BipraGetImageFileNameFromHandle(imageHandle, imageFileName) == FALSE)
	{
		return FALSE;
	}
	
	if (BipraGetImageFileProperties(imageFileName, &width, &height) == FALSE)
	{
		return FALSE;
	}

	if (	(width >= tempPixelRange.widthSmall) && (width <= tempPixelRange.widthLarge) &&
		(height >= tempPixelRange.heightSmall) && (height <= tempPixelRange.heightLarge))
	{
		if (pixelRangeType != PIXEL_RANGE_TYPE_FIXED_ASPECT_RATIO)
		{
			return TRUE;
		}
		else
		{
			if (width * tempPixelRange.heightLarge == tempPixelRange.widthLarge  * height)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	else
	{
		return FALSE;
	}	
}

static void	BipraDebugSaveImageDescriptor(const U8 *imageDescriptor)
{
	char					tempStr[IMG_PROPERTIES_MAX_LINE_LEN + 1];
	BthalFsFileDesc		fd = 0;
	BtFsStatus			fsStatus;
	BTHAL_U32			numWritten;
	BOOL				retVal = TRUE;
static const char			*debugImageDescriptorFullFileName = "/bip/ImageDescriptor.xml";

	if (imageDescriptor == NULL)
	{
		return;
	}
	
	fsStatus = BTHAL_FS_Open(	(BTHAL_U8*)debugImageDescriptorFullFileName, 
								BTHAL_FS_O_WRONLY | BTHAL_FS_O_CREATE | BTHAL_FS_O_TRUNC | BTHAL_FS_O_TEXT,
								&fd);
	if (fsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		Report(("BIPRA: ERROR: Failed opening Debug Image Descriptor file (%s)", debugImageDescriptorFullFileName));
		return;
	}

 	fsStatus = BTHAL_FS_Write(fd, (void*)imageDescriptor, strlen((const char*)imageDescriptor), &numWritten);
	
	BTHAL_FS_Close(fd);	
}

static BOOL BipraRespCheckRequestedImageProperties(	const char	*imageHandle,
																const U8* 	imageDescriptor,
																BOOL 		*thumbnailVersionRequested)
{
	BtlBipEncoding	 				encoding;
	BtlBipPixel					pixel;
	PixelRangeType				pixelRangeType;
	ImageDescriptorElementsMask	mask;

	*thumbnailVersionRequested = FALSE;

	BipraDebugSaveImageDescriptor(imageDescriptor);
	
	if ((imageDescriptor == NULL) || (imageDescriptor[0] == '\0'))
	{
		return TRUE;
	}
	
	strcpy((char*)tempImageDescriptor, (const char *)imageDescriptor);
	
	BipraParseImageDescriptor(	tempImageDescriptor, 
								&encoding,
								&pixel, 
								0, 
								0, 
								0, 
								&mask);

	/* Accept only JPEG requests or requests that don't care for encoding */
	if ((mask & IMAGE_DESCRIPTOR_ELEMENT_MASK_ENCODING) != 0)
	{
		Report(("BIPRA: Requested Encoding: %s", BipraGetBipEncodingString(encoding)));
		
		if (encoding != BTL_BIP_ENCODING_JPEG)
		{
			return FALSE;
		}
	}

	if ((mask & IMAGE_DESCRIPTOR_ELEMENT_MASK_PIXEL) != 0)
	{
		pixelRangeType = BipraGetPixelRangeType(&pixel);
		
		Report(("BIPRA: Requested Pixel: %d*%d - %d*%d (%s)", 
					pixel.widthSmall, pixel.heightSmall, pixel.widthLarge, pixel.heightLarge, BipraGetPixelRangeStr(pixelRangeType)));

		if (BipraCheckIfRequestedPixelRangeMatchesImage(imageHandle, pixelRangeType, &pixel) == FALSE)
		{
			return FALSE;
		}
		
		*thumbnailVersionRequested = BipraIsThumbnailPixelValues(&pixel);
	}
	else
	{
		Report(("BIPRA: No Pixel Range Specified"));
	}

	return TRUE;
}

static BOOL BipraVerifyDefaultSettings(void)
{
	BtFsStatus 	fsStatus;
	BthalFsStat 	fileStat;
	
	fsStatus = BTHAL_FS_Stat((BTHAL_U8*)BIPRA_DEFAULT_IMAGE_PATH, &fileStat);

	if (fsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: ERROR: BTHAL_FS_Stat Failed For %s", BIPRA_DEFAULT_IMAGE_PATH));
		return FALSE;
	}

	if (fileStat.type != BTHAL_FS_DIR)
	{
		UI_DISPLAY(("BIPRA: ERROR: %s exists but is not a directory", BIPRA_DEFAULT_IMAGE_PATH));
		return FALSE;
	}

	if (IsImageHandleValid(BIPRA_DEFAULT_PUSHED_IMAGE_HANDLE) == FALSE)
	{
		UI_DISPLAY(("BIPRA: Invalid Default Image Handle (%s)", BIPRA_DEFAULT_PUSHED_IMAGE_HANDLE));
		return FALSE;
	}
	
	return TRUE;
}

static void BipraInitRequestData(void)
{
	strcpy(currentFileName, "");
	strcpy(currentFullFileName, "");
	memset(&pushedImagePixelValues, 0, sizeof(pushedImagePixelValues));
}

static BOOL BipraInitContext(void)
{
	BtStatus 		status;

	OS_StrCpy((char*)pushedImageHandle, BIPRA_DEFAULT_PUSHED_IMAGE_HANDLE);
	Report(("BIPRA: Default Pushed Image Handle Set To %s", (char*)pushedImageHandle));
	
	status = BTL_BIPRSP_SetDefaultImagePath(biprspContext, (BtlUtf8*)BIPRA_DEFAULT_IMAGE_PATH);
	
	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: ERROR: BTL_BIPRSP_SetDefaultImagePath For %s Failed (%s)", BIPRA_DEFAULT_IMAGE_PATH, pBT_Status(status)));
		return FALSE;
	}

	Report(("BIPRA: Successsfully set default path to |%s|", BIPRA_DEFAULT_IMAGE_PATH));
	
	return TRUE;
}

static BOOL BipraCreateContext(void)
{
	BtStatus status;
	BOOL	result;
	
	if (biprspContext != NULL)
	{
		UI_DISPLAY(("BIPRA: BIPRSP Single Instance already created"));
		return FALSE;
	}

	initState++;
	
	status = BTL_BIPRSP_Create(NULL, BIPRSPA_Callback, NULL, &biprspContext);	

	initState++;

	if (status != BT_STATUS_SUCCESS)
	{
		UI_DISPLAY(("BIPRA: BTL_BIPRSP_Create() Failed(%s)", pBT_Status(status)));
		return FALSE;
	}

	result = BipraInitContext();

	initState++;

	return result;
}

/*-------------------------------------------------------------------------------
* NextImgHandleFile()
*
*      Return the next imgHandle file, according the critereas as mentioined
*      before in the 'CreateImageListingFromDirectory' routine.
*     
* Type:
*      Synchronous
*
* Parameters:
*      fd [in] - ID of the directory.
*
*      imgHandleLen [out] - Length (characters) of the imgHandle
*
*      fileStat [out] - File statistics (size, creation time, etc.)
*
* Returns:
*      U8* - imgHandle file (NULL = no file found)
*
*/
static BOOL NextImgHandleFile(BthalFsDirDesc   fd, char  *imgHandle)
{
	BtFsStatus	fsStatus;
	char			*fullFilename;
	char			*filename = NULL;
	char*            fileExtension;
	BOOL		finished = FALSE;
	BthalFsStat	fileStat;
	U32			imgHandleLen;

	strcpy(imgHandle, "");
	
	/* Read next file from the directory */
	fsStatus = BTHAL_FS_ReadDir(fd, (BTHAL_U8**)&fullFilename);

	/* End of directory not reached yet + File not found yet? --> continue*/
	while (((fsStatus == BT_STATUS_HAL_FS_SUCCESS)) && (FALSE == finished))
	{
		/* get file size */
		fsStatus = BTHAL_FS_Stat((BTHAL_U8*)fullFilename, &fileStat);
		BIPRA_VERIFY((fsStatus == BT_STATUS_HAL_FS_SUCCESS), ("BIPRA: BTHAL_FS_Stat Failed (%d) For %s", fsStatus, fullFilename), FALSE);
		
		/* File does contain data? --> continue with other criteria. */
		if (fileStat.size > 0)
		{
			/* Skip the path prefix. */
			filename = strrchr(fullFilename, BTHAL_FS_PATH_DELIMITER);
			filename++;

			/* Prefix is OK as well? --> continue with other criteria. */
			if (0 == strncmp(filename, IMG_HANDLE_FILE_NAME_PREFIX, IMG_HANDLE_FILE_NAME_PREFIX_LEN)) 
			{
				/* Skip the prefix */
				filename += IMG_HANDLE_FILE_NAME_PREFIX_LEN;
				
				/* Find file extension. */
				fileExtension = strrchr(filename, '.');
				
				/* Extension is OK as well? --> found it. */
				if (0 == strncmp(fileExtension, IMG_HANDLE_FILE_EXTENSION, sizeof(IMG_HANDLE_FILE_EXTENSION)-1))
				{
					/* return the Length of the imgHandle as well.*/
					imgHandleLen = fileExtension - filename;
					
					if (imgHandleLen == 7)
					{
						strncpy(imgHandle, filename, 7);
						imgHandle[7] = '\0';
						finished = TRUE;
					}  
				}
			}
		}


		/* Not finished yet? --> Read next file from the directory */
		if (FALSE == finished)
		{
			fsStatus = BTHAL_FS_ReadDir(fd, (BTHAL_U8**)&fullFilename);
		}
	}

	/* End of directory reached? --> return NULL*/
	if (fsStatus != BT_STATUS_HAL_FS_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

/*-------------------------------------------------------------------------------
 * CreateImageListingFromDirectory()
 *
 *      Create ImageList Descriptor from directory.
 *      This is environment dependent.
 *      This routine uses the following algorithm to create an image list with a list
 *      of so-called imageHandles (an imageHandle may only contain digit characters
 *      with a maximum length of 8):
 *
 *      Assumption:
 *      The inidcated directory contains a list of JPG files, where those filenames
 *      that need to be replied as an imgHandle do have a fixed prefix and contains
 *      only digit characters behind the prefix.
 *      E.g. IMG_1234567.JPG does have a 'IMG_' prefix, and the created imgHandle
 *      will be '1234567'.
 *
 *      Settings:
 *      - fileNamePrefix: definitionof the fixed filename prefix for teh imgHandles
 *
 *      Algorithm:
 *      1. Make a complete directory list of teh indicated directory.
 *      2. Scan for the files beginning iwth the 'fileNamePrefix'
 *      3. For those 'fileNamePrefix' files: create an imgHandle by removing
 *         this prefix and add it to the list.
 *     
 *
 * Type:
 *      Synchronous
 *
 * Parameters:
 *      fsPath [in]         - Zero-terminated string that holds the directory.
 *
 *      offset [in]         - Offset from which the directory listing must start.
 *
 *      requestedHandles    - Number of requested handles in the descriptor.
 *
 *      xmlObj [in/out] - Pointer to a buffer that will contain the constructed descriptor.
 *
 * Returns:
 *      U16 - Number of Handles in the descriptor.
 *
 */
static BOOL CreateImageListingFromDirectory(	BtlBiprspContext 	*bipContext,
													U8				*fsPath,
													U16				offset,
													U16 				requestedHandles,
													U8  				**xmlObj,
													U16				*numOfHandles)
{
	BtStatus			status;
	BOOL			retVal = TRUE;
	BOOL			result;
	BtFsStatus      	fsStatus;
	BthalFsDirDesc	fd;
	U16             		returnedHandles = 0;
	char		   		imgHandle[8];
	BipImgHandle		bipImgHandle;

	fsStatus = BTHAL_FS_OpenDir(fsPath, &fd);
	BIPRA_VERIFY((fsStatus == BT_STATUS_HAL_FS_SUCCESS), ("BIPRA: BTHAL_FS_OpenDir Failed (%d) For %s", fsStatus, fsPath), FALSE);
	
	status = BTL_BIPRSP_BuildListingStart(biprspContext, xmlTxObj, XML_OBJ_MAX_LEN);
	BIPRA_VERIFY_AND_CLEANUP((status == BT_STATUS_SUCCESS), 
								("BIPRA: BTL_BIPRSP_BuildListingStart Failed (%s)", pBT_Status(status)), FALSE);

	/* Read first imgHandle file. */
	result = NextImgHandleFile(fd, imgHandle);

	/* Move to the indicated offset. */
	while ((TRUE == result) && (offset > 0))
	{
		/* Next */
		offset--;
		result = NextImgHandleFile(fd, imgHandle);
	}

	if (requestedHandles == 0)
	{
		while(TRUE == result)
		{
			/* Next */
			requestedHandles--;
			returnedHandles++;
			result = NextImgHandleFile(fd, imgHandle);
		}
	}
	else
	{
		/* Return requested number of imgHandles, starting at this offset. */
		while ((TRUE == result) && (requestedHandles > 0))
		{
			OS_StrCpy((char*)bipImgHandle, imgHandle);
			
			/* Add it to the list. */
			status = BTL_BIPRSP_BuildListingAddImg(bipContext, bipImgHandle, NULL, NULL);
			BIPRA_VERIFY_AND_CLEANUP(	(status == BT_STATUS_SUCCESS), 
											("BIPRA: BTL_BIPRSP_BuildListingAddImg Failed (%s)", pBT_Status(status)), FALSE);

			/* Next */
			requestedHandles--;
			returnedHandles++;
			result = NextImgHandleFile(fd, imgHandle);
		}
	}

	status = BTL_BIPRSP_BuildListingFinish(bipContext, xmlObj);
	BIPRA_VERIFY_AND_CLEANUP((status == BT_STATUS_SUCCESS), 
								("BIPRA: BTL_BIPRSP_BuildListingFinish Failed (%s)", pBT_Status(status)), FALSE);

CLEANUP:
	
	BTHAL_FS_CloseDir(fd);
	
	*numOfHandles = returnedHandles;

	return retVal;
}

static BOOL BipraGetNextMonitoringImageHandle(char *handle)
{
	BOOL			result;
	BthalFsDirDesc	fd;
	BtFsStatus		fsStat;
	static U32 		nextMonitoringImageIndex = 0;
	U32				i;	
	U32				count;

	strcpy(handle, "");
	
	/* Count # of files */
	
	fsStat = BTHAL_FS_OpenDir((BTHAL_U8*)BIPRA_DEFAULT_IMAGE_PATH, &fd);
	BIPRA_VERIFY(	(fsStat == BT_STATUS_HAL_FS_SUCCESS), 
					("BIPRA: ERROR: BTHAL_FS_OpenDir Failed (%d) for %s", fsStat, BIPRA_DEFAULT_IMAGE_PATH),
					FALSE)

	count = 0;
	
	do
	{
		result = NextImgHandleFile(fd, handle);

		if (result == TRUE)
		{
			count++;
		}
	} while (result == TRUE);

	BTHAL_FS_CloseDir(fd);

	if (count == 0)
	{
		return FALSE;
	}
	
	nextMonitoringImageIndex = (nextMonitoringImageIndex + 1) % count;
	
	fsStat = BTHAL_FS_OpenDir((BTHAL_U8*)BIPRA_DEFAULT_IMAGE_PATH, &fd);
	
	for (i = 0; i < nextMonitoringImageIndex; ++i)
	{
		NextImgHandleFile(fd, handle);
	}

	NextImgHandleFile(fd, handle);
	
	BTHAL_FS_CloseDir(fd);

	return TRUE;
}

static void BipraCompletePutImageReponse(void)
{
	BtlBipPixel					pixel;
	ImageDescriptorElementsMask	mask = 0;
	char							imageFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	char							propertiesFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	char							fullPropertiesFileName[BTHAL_FS_MAX_PATH_LENGTH + 1];

	BipraGetPropertiesFileNameFromImageFileName(currentFileName, propertiesFileName);
	BipraGetFullFileName(propertiesFileName, fullPropertiesFileName);
		
	BipraGeneratetImageFileProperties(fullPropertiesFileName, pushedImagePixelValues.widthSmall, pushedImagePixelValues.heightSmall);
}

static BOOL BipraGeneratetImageFileProperties(const char *fullPropertiesFileName, U16 width, U16 height)
{
	char					tempStr[IMG_PROPERTIES_MAX_LINE_LEN + 1];
	BthalFsFileDesc		fd = 0;
	BtFsStatus			fsStatus;
	BTHAL_U32			numWritten;
	BOOL				retVal = TRUE;

	Report(("BIPRA: Generating Properties File (%s) (W = %d, H = %d)", fullPropertiesFileName, width, height));
	
	fsStatus = BTHAL_FS_Open(	(BTHAL_U8*)fullPropertiesFileName, 
								BTHAL_FS_O_WRONLY | BTHAL_FS_O_CREATE | BTHAL_FS_O_TRUNC | BTHAL_FS_O_TEXT,
								&fd);
	BIPRA_VERIFY(	(fsStatus == BT_STATUS_HAL_FS_SUCCESS), 
					("BIPRA: ERROR: Failed opening image properties file (%s)", fullPropertiesFileName),
					FALSE);

	memset(tempStr, 0, IMG_PROPERTIES_MAX_LINE_LEN + 1);
	sprintf(tempStr, "%d %d", width, height);
	
 	fsStatus = BTHAL_FS_Write(fd, tempStr, strlen(tempStr), &numWritten);
	BIPRA_VERIFY_AND_CLEANUP(	(fsStatus == BT_STATUS_HAL_FS_SUCCESS), 
									("BIPRA: ERROR: Failed Writing to image properties file (%s)", fullPropertiesFileName),
									FALSE);
	BIPRA_VERIFY_AND_CLEANUP(	(numWritten == sizeof(tempStr)), 
									("BIPRA: ERROR: Wrote %d bytes to %s instead of %d", 
										numWritten, fullPropertiesFileName, sizeof(tempStr)),
									FALSE);

	Report(("BIPRA: Successfully wrote |%s| to the Properties File", tempStr));

CLEANUP:
	
	BTHAL_FS_Close(fd);

	return retVal;
}

static BOOL BipraGetImageFileProperties(const char *imageFileName, U16 *width, U16 *height)
{
	char				fullPropertiesFileName[BTHAL_FS_MAX_PATH_LENGTH + 1];
	char				propertiesFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	char				tempStr[IMG_PROPERTIES_MAX_LINE_LEN + 1];
	BthalFsFileDesc	fd = 0;
	BtFsStatus		fsStatus;
	BTHAL_U32		numRead;
	int				numOfFields;
	int				value1 = 0;
	int				value2 = 0;
	BOOL			retVal = TRUE;
	
	BipraGetPropertiesFileNameFromImageFileName(imageFileName, propertiesFileName);
	BipraGetFullFileName(propertiesFileName, fullPropertiesFileName);

	fsStatus = BTHAL_FS_Open(	(BTHAL_U8*)fullPropertiesFileName, 
								BTHAL_FS_O_RDONLY | BTHAL_FS_O_TEXT,
								&fd);
	BIPRA_VERIFY(	(fsStatus == BT_STATUS_HAL_FS_SUCCESS), 
					("BIPRA: ERROR: Failed opening image properties file (%s)", fullPropertiesFileName),
					FALSE);

	fsStatus = BTHAL_FS_Read(fd, tempStr, IMG_PROPERTIES_MAX_LINE_LEN, &numRead);
	BIPRA_VERIFY_AND_CLEANUP(	(fsStatus == BT_STATUS_HAL_FS_SUCCESS), 
									("BIPRA: ERROR: Failed Reading From image properties file (%s)", fullPropertiesFileName),
									FALSE);

	numOfFields = sscanf(tempStr, "%d%d", &value1, &value2);
	BIPRA_VERIFY_AND_CLEANUP(	(numOfFields == 2), 
									("BIPRA: ERROR: Read %d fields (instead of 2)  from properties file (%s)", 
										numOfFields, fullPropertiesFileName),
									FALSE);
	
CLEANUP:
	*width = (U16) value1;
	*height = (U16) value2;
	
	BTHAL_FS_Close(fd);
	
	return retVal;
}

static const char *BipraGetPixelRangeStr(PixelRangeType	rangeType)
{
	switch (rangeType)
	{
		case PIXEL_RANGE_TYPE_ANY: 					return "ANY";
		case PIXEL_RANGE_TYPE_RANGE: 				return "RANGE";
		case PIXEL_RANGE_TYPE_THUMBNAIL: 			return "Thumbnail";
		case PIXEL_RANGE_TYPE_FIXED_ASPECT_RATIO: 	return "Fixed Aspect Ratio";
		case PIXEL_RANGE_TYPE_FIXED_SIZE: 			return "Fixed Size";
		default: 										return "INVALID RANGE TYPE";
	}
}

static const char *BipraGetBipEncodingString(BtlBipEncoding bipEncoding)
{
    switch( bipEncoding)
    {
        case BTL_BIP_ENCODING_JPEG:     	return "JPEG";
        case BTL_BIP_ENCODING_JPEG2000: return "JPEG2000";
        case BTL_BIP_ENCODING_PNG:     	return "PNG";
        case BTL_BIP_ENCODING_GIF:      	return "GIF";
        case BTL_BIP_ENCODING_BMP:      	return "BMP";
        case BTL_BIP_ENCODING_WBMP:     return "WBMP"; 
        default: 						return "INVALID";
    }
}

static BtlBipEncoding BipraGetBipEncodingFromString(U8* str)
{
	BtlBipEncoding encodingFormat;

	if( OS_StriCmp( (char*)str, "JPEG") == 0 )
		encodingFormat = BTL_BIP_ENCODING_JPEG;
	else if( OS_StriCmp( (char*)str, "JPEG2000") == 0 )
		encodingFormat = BTL_BIP_ENCODING_JPEG2000;
	else if( OS_StriCmp( (char*)str, "PNG") == 0 )
		encodingFormat = BTL_BIP_ENCODING_PNG;
	else if( OS_StriCmp( (char*)str, "GIF") == 0 )
		encodingFormat = BTL_BIP_ENCODING_GIF;
	else if( OS_StriCmp( (char*)str, "BMP") == 0 )
		encodingFormat = BTL_BIP_ENCODING_BMP;
	else if( OS_StriCmp( (char*)str, "WBMP") == 0 )
		encodingFormat = BTL_BIP_ENCODING_WBMP;
	else
		/* gesl - UDI Addition - Issue an error in this case */
		encodingFormat = BTL_BIP_ENCODING_ALL;
	
    return encodingFormat;
}

static void BipraGetPixelSizesFromString(U8* str, BtlBipPixel *pixelFormat)
{
	U8  			*dpTemp = 0;
	U8			*pixelStr = 0;

	/* Is it a pixel range ? Eg. 80*60-640*480 */
	pixelStr = (U8*)strchr((char*)str, '-');
	
	if (pixelStr != NULL)
	{   /* pixel range */
		BOOL fixedAspectRatio = FALSE;
		
		*pixelStr = 0;
		pixelStr++;
		
		/* assign Small range */
		dpTemp = (U8*)strchr((char*)str,'*');
		*dpTemp = 0;
		dpTemp++;
		
		sscanf((char*)str, "%d", &pixelFormat->widthSmall);

		/* Check for a fixed aspect ratio ("**") range*/		
		if (*dpTemp != '*')
		{
			sscanf((char*)dpTemp, "%d", &pixelFormat->heightSmall);
		}
		else
		{
			fixedAspectRatio = TRUE;
		}
		
		/* assign Large range */
		dpTemp = (U8*)strchr((char*)pixelStr, '*');
		*dpTemp = 0;
		dpTemp++;
		
		sscanf((char*)pixelStr, "%d", &pixelFormat->widthLarge);
		sscanf((char*)dpTemp, "%d", &pixelFormat->heightLarge);

		if (fixedAspectRatio == TRUE)
		{
			pixelFormat->heightSmall = pixelFormat->heightLarge;
		}
	}
	else
	{   /*normal pixel resolution Eg. 640*480 */
		dpTemp = (U8*)strchr((char*)str,'*');
		*dpTemp = 0;
		dpTemp++;
		
		/* no range defined, so Small sizes will be same as Large */
		sscanf((char*)str, "%d", &pixelFormat->widthSmall);
		sscanf((char*)dpTemp, "%d", &pixelFormat->heightSmall);
		pixelFormat->widthLarge = pixelFormat->widthSmall;
		pixelFormat->heightLarge = pixelFormat->heightSmall;				
	}
}

static BtlBipTranformation BipraGetBipTransformationFromString(U8* str)
{
	BtlBipTranformation transformation;

	if( OS_StriCmp( (char*)str, "stretch") == 0 )
		transformation = BTL_BIP_TRANSFORMATION_STRETCH;
	else if( OS_StriCmp( (char*)str, "crop") == 0 )
		transformation = BTL_BIP_TRANSFORMATION_CROP;
	else if( OS_StriCmp( (char*)str, "fill") == 0 )
		transformation = BTL_BIP_TRANSFORMATION_FILL;
	else
		transformation = BTL_BIP_TRANSFORMATION_NONE;
	
    return transformation;
}

/*-------------------------------------------------------------------------------
 * GetXmlKeyElementValue()
 */
static U8*BipraGetXmlKeyElementValue(U8* descriptor, U8* field, U8* attribute, U16 attrLen, U16 maxLen)
{
    U8 *dp = descriptor;
    U8 *dpRet;
    U8 *dpTemp;
    U16 iLen=0;
    U16 fieldLen = strlen((char*)field);
    BOOL fieldFound = FALSE;
	char	tempStr[200 + 1];

	Assert(fieldLen <= 200);
	
	strcpy(tempStr, (char*)field);
	
	memset(attribute, 0, attrLen);

	Report(("BIPRA: Entered BipraGetXmlKeyElementValue, field = |%s|, maxLen = %d", field, maxLen));
	
	/* Find the field in the object */
    for(iLen=0;iLen<maxLen;iLen++)
    {
    	char tempStr1[201];
	memset(tempStr1, 0, 200);
	OS_StrnCpy(tempStr1, (char*)dp, strlen(tempStr));
		
	/* upper case it before comparison to be more robust */
        if(OS_StriCmp(tempStr1, tempStr) == 0 )
        {
        	Report(("BIPRA: Field Found"));
			
            fieldFound = TRUE;
            break;
        }
        dp++;
    }

	/* We found the field, now determine the end of the value, always ended by character ' " '  */
	/* and add the value to the attribute parameter, making sure that attribute is 0-terminated */
    if( TRUE == fieldFound )
    {
        while( (*dp != '"') && (iLen < maxLen) )
        {
            dp++;
        }
        if( '"' == *dp )
            dp++;
        else
            return dp;
        OS_StrnCpy((char*)attribute,(char*)dp,attrLen);
        dpRet = dp;
        dp = (U8*)OS_StrrChr((char*)attribute, '"' );

	Report(("BIPRA: attribute = |%s|", (char*)attribute));
	
        dpTemp = attribute;
        if( NULL != dp )
        {
            while( dpTemp != dp )
            {
                if( '"' == *dpTemp )
                {
                    *dpTemp = 0;
                    break;
                }
                dpTemp++;
            }
            if( dp == dpTemp )
                *dp = 0;
        }
    }
    else
    {
    	Report(("BIPRA: Field NOT Found"));
		
        dpRet = descriptor;
	}

	Report(("BIPRA: Exiting BipraGetXmlKeyElementValue"));
	
    return dpRet;
}

static void BipraParseImageDescriptor(	U8							*descriptor,
											BtlBipEncoding					*encoding,
											BtlBipPixel					*pixel,
											U32							*size,
											U32							*maxSize,
											BtlBipTranformation			*transformation,
											ImageDescriptorElementsMask	*mask)
{
	U8  	*parsePos1 = descriptor;
	U8  	*parsePos2 = descriptor;
	U8	tempStr[100 + 1];

	Report(("BIPRA: Entered BipraParseImageDescriptor"));

	*mask = 0;

	if (descriptor == 0)
	{
		Report(("BIPRA: Null descriptor - Exiting"));
		return;
	}
	
	parsePos2 = BipraGetXmlKeyElementValue(parsePos1, (U8*)"<image encoding=\"", tempStr, 100, (U16)strlen((char*)parsePos1));

	if (parsePos2 != parsePos1)
	{
		if (tempStr[0] != (U8)'\0')
		{
			if (encoding != 0)
			{
				*encoding = BipraGetBipEncodingFromString(tempStr);		
				*mask |= IMAGE_DESCRIPTOR_ELEMENT_MASK_ENCODING;

				Report(("BIPRA: Found Encoding %s", BipraGetBipEncodingString(*encoding)));
			}
		}
	}
	
	parsePos1 = parsePos2;
		
	parsePos2 = BipraGetXmlKeyElementValue(parsePos1, (U8*)"pixel=\"", tempStr, 100, (U16)strlen((char*)parsePos1));
		
	if (parsePos2 != parsePos1)
	{
		if (tempStr[0] != (U8)'\0')
		{
			if (pixel != 0)
			{
				BipraGetPixelSizesFromString(tempStr, pixel);
				*mask |= IMAGE_DESCRIPTOR_ELEMENT_MASK_PIXEL;

				Report(("BIPRA: Found Pixel (%d*%d - %d*%d)", 
						pixel->widthSmall, pixel->heightSmall, pixel->widthLarge, pixel->heightLarge));
			}
		}
	}
	
	parsePos1 = parsePos2;
	
	parsePos2 = BipraGetXmlKeyElementValue(parsePos1, (U8*)"size=\"", tempStr, 100, (U16)strlen((char*)parsePos1));
			
	if (parsePos2 != parsePos1)
	{
		if (size != 0)
		{
			sscanf((char*)tempStr, "%d", size);
			*mask |= IMAGE_DESCRIPTOR_ELEMENT_MASK_SIZE;
		}
	}
	
	parsePos1 = parsePos2;	

	parsePos2 = BipraGetXmlKeyElementValue(parsePos1, (U8*)"maxsize=\"", tempStr, 100, (U16)strlen((char*)parsePos1));
			
	if (parsePos2 != parsePos1)
	{
		if (maxSize != 0)
		{
			sscanf((char*)tempStr, "%d", maxSize);
			*mask |= IMAGE_DESCRIPTOR_ELEMENT_MASK_MAX_SIZE;
		}
	}
	
	parsePos1 = parsePos2;	

	parsePos2 = BipraGetXmlKeyElementValue(parsePos1, (U8*)"transformation=\"", tempStr, 100, (U16)strlen((char*)parsePos1));
			
	if (parsePos2 != parsePos1)
	{
		if (transformation != 0)
		{
			*transformation = BipraGetBipTransformationFromString(tempStr);
			*mask |= IMAGE_DESCRIPTOR_ELEMENT_MASK_TRANSFORMATION;
		}
	}
	
	parsePos1 = parsePos2;	

	Report(("BIPRA: Exiting BipraParseImageDescriptor, mask = %x", *mask));
}

static BOOL BipraGetImageFileNameFromHandle(const char *handle, char *fileName)
{
	strcpy(fileName, IMG_HANDLE_FILE_NAME_PREFIX);
	strcat(fileName, handle);
	strcat(fileName, IMG_HANDLE_FILE_EXTENSION);

	return TRUE;
}

static void BipraGetFullFileName(const char *fileName, char *fullFileName)
{
	strcpy(fullFileName, BIPRA_DEFAULT_IMAGE_PATH);
	strcat(fullFileName, fileName);
}

static BOOL BipraGetImageFullFileNameFromHandle(const char *handle, char *fullFileName)
{
	char	fileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	
	if (BipraGetImageFileNameFromHandle(handle, fileName) == FALSE)
	{
		return FALSE;
	}

	BipraGetFullFileName(fileName, fullFileName);

	return TRUE;
}

static void BipraGetPropertiesFileNameFromImageFileName(const char *imageFileName, char *propertiesFileName)
{
	strncpy(propertiesFileName, imageFileName, IMG_HANDLE_FILE_NAME_PREFIX_LEN + 7);
	propertiesFileName[IMG_HANDLE_FILE_NAME_PREFIX_LEN + 7] = '\0';
	
	strcat(propertiesFileName, IMG_PROPERTIES_SUFFIX);
}

static BOOL BipraExtractHandleFromFileName(const char* fileName, char *handle)
{
	strncpy(handle, &fileName[IMG_HANDLE_FILE_NAME_PREFIX_LEN], 7);

	handle[7] = '\0';

	return IsImageHandleValid(handle);
}

static BOOL BipraIsImageFileNameValid(const char *imageFileName)
{
	char handle[8];
	
	if (strlen(imageFileName) != (IMG_HANDLE_FILE_NAME_PREFIX_LEN + 7 + strlen(IMG_HANDLE_FILE_EXTENSION)))
	{
		return FALSE;
	}

	if (strncmp(imageFileName, IMG_HANDLE_FILE_NAME_PREFIX, IMG_HANDLE_FILE_NAME_PREFIX_LEN) != 0)
	{
		return FALSE;
	}
	
	if (BipraExtractHandleFromFileName(imageFileName, handle) == FALSE)
	{
		return FALSE;
	}

	if (strncmp(imageFileName, IMG_HANDLE_FILE_NAME_PREFIX, IMG_HANDLE_FILE_NAME_PREFIX_LEN) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

static BOOL BipraGetThumbnailFileNameFromImageFileName(const char *imageFileName, char *thumbnailFileName)
{
	char	*postHandlePos = 0;
	
	if (BipraIsImageFileNameValid(imageFileName) == FALSE)
	{
		return TRUE;
	}

	postHandlePos = strrchr(imageFileName, '.');

	strncpy(thumbnailFileName, imageFileName, postHandlePos - imageFileName);
	thumbnailFileName[postHandlePos - imageFileName] = '\0';
	
	strcat(thumbnailFileName, IMG_THUMBNAIL_SUFFIX);
	strcat(thumbnailFileName, IMG_HANDLE_FILE_EXTENSION);

	return TRUE;
}

static BOOL BipraGetThumbnailFileNameFromImageHandle(const char *handle, char *thumbnailFileName)
{
	char	imageFileName[BTHAL_FS_MAX_FILE_NAME_LENGTH + 1];
	
	BipraGetImageFileNameFromHandle(handle, imageFileName);

	return BipraGetThumbnailFileNameFromImageFileName(imageFileName, thumbnailFileName);
}

#endif /* BTL_CONFIG_BIP == BTL_CONFIG_ENABLED */


