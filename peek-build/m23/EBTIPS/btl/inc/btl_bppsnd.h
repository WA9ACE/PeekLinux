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
*   FILE NAME:      btl_bppsnd.h
*
*   BRIEF:    This file defines the API of the BTL Basic Printing Profile Sender role
*
*   DESCRIPTION:    
*
*                 The BPP Sender role is meant for sending print jobs to a BPP Printer.
*
*                 The BTL BPP Sender API allows application to connect and send objects to the printer,
*                 either in the Simple Push or in the Job Based transfer model. The abject can be either
*                 a file or a memory object. Referenced objects are supported (e.g. an image
*                 file referenced from an xhtml file).
*
*                 Optional features not supported:
*                      GetEvent command 
*                      Enhanced Layout commands
*                      Reflected User Interface (RUI)
*                      Print By reference (PBR)
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/

#ifndef __BTL_BPPSND_H
#define __BTL_BPPSND_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "goep.h"
#include "bpp.h"
#include "btl_common.h"
#include "btl_unicode.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/

#define 	BTL_BPPSND_MAX_MIME_TYPE_LEN							GOEP_MAX_TYPE_LEN


#define	BTL_BPPSND_MAX_DOCUMENT_FORMAT_LEN					BTL_BPPSND_MAX_MIME_TYPE_LEN
#define	BTL_BPPSND_MAX_IMAGE_FORMAT_LEN						BTL_BPPSND_MAX_MIME_TYPE_LEN

#define	BTL_BPPSND_MAX_PRINTER_NAME_LEN						20
#define	BTL_BPPSND_MAX_PRINTER_LOCATION_LEN					20
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_DOCUMENT_FORMATS	20
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_IMAGE_FORMATS		6
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_SIDES					4
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_PRINT_QUALITY		4
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_ORIENTATIONS		5
#define	BTL_BPPSND_MAX_PRINTER_GENERAL_CURRENT_OPERATOR		20
#define	BTL_BPPSND_MAX_JOB_NAME_LEN								20
#define	BTL_BPPSND_MAX_JOB_ORIGINATING_USER_NAME_LEN			20
#define	BTL_BPPSND_MAX_MEDIA_SIZE_LEN							30
#define	BTL_BPPSND_MAX_MEDIA_TYPE_LEN							30
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_SIZES			10
#define	BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_TYPES			10
#define	BTL_BPPSND_MAX_REF_OBJ_IDENTIFIER_LEN					255
#define	BTL_BPPSND_MAX_NUM_OF_MEDIA_LOADED					6


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* Forward declarations */
typedef struct _BtlBppsndEvent 	BtlBppsndEvent;
typedef struct _BtlBppsndContext 	BtlBppsndContext;

/*-------------------------------------------------------------------------------
 * BtlBppsndCallBack type
 *
 *     A function of this type is called to indicate BTL BPP Sender events.
 */
typedef void (*BtlBppsndCallBack)(const BtlBppsndEvent *event);

/*-------------------------------------------------------------------------------
 * BtlBppFailureReason type
 *
 *     reasons for failures in BPP operations
 */
typedef enum _BtlBppFailureReason
{
	BTL_BPPSND_FAILURE_REASON_NONE,
	BTL_BPPSND_FAILURE_REASON_FFS_ERROR,
	BTL_BPPSND_FAILURE_REASON_USER_ABORT,
	BTL_BPPSND_FAILURE_REASON_ABORTED,
	BTL_BPPSND_FAILURE_REASON_UNSPECIFIED
} BtlBppFailureReason;

/*-------------------------------------------------------------------------------
 * BtlBppsndJobId type
 *
 *     An Id of a Job. 
 */
typedef U32 BtlBppsndJobId;

/*---------------------------------------------------------------------------
 * BppPrinterAttributesMask type
 *
 *	This mask is used to indicate which printer attributes are present in a printer
 *	response to a GetPrinterAttributes request
 *
 */
typedef U32	BtlBppsndPrinterAttributesMask;

#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_NAME 					0x00000001UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_LOCATION 				0x00000002UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE	 				0x00000004UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE_REASONS			0x00000008UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_DOCUMENT_FORMATS_SUPPORTED	0x00000010UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_COLOR_SUPPORTED				0x00000020UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MAX_COPIES_SUPPORTED			0x00000040UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_SIDES_SUPPORTED				0x00000080UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_NUMBER_UP_SUPPORTED		 	0x00000100UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ORIENTATIONS_SUPPORTED		0x00000200UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_SIZES_SUPPORTED		 	0x00000400UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_TYPES_SUPPORTED	 	0x00000800UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_LOADED				 	0x00001000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINT_QUALITY_SUPPORTED	 	0x00002000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_IMAGE_FORMATS_SUPPORTED	 	0x00004000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_WIDTH	 	0x00008000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_HEIGHT	 	0x00010000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_GENERAL_CURRENT_OPERATOR	0x00020000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_QUEUED_JOB_COUNT	 			0x00040000UL
#define	BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL								0xFFFFFFFFUL


/*---------------------------------------------------------------------------
 * BtlBppsndCreateJobAttributesMask type
 *
 */
typedef U32	BtlBppsndCreateJobAttributesMask;

#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_JOB_NAME							0x00000001UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME		0x00000002UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_DOCUMENT_FORMAT					0x00000004UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_NUM_OF_COPIES						0x00000008UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_SIDES								0x00000010UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_NUMBER_UP		 					0x00000020UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_ORIENTATION_REQUESTED				0x00000040UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_MEDIA_SIZE							0x00000080UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_PRINT_QUALITY						0x00000100UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_MEDIA_TYPE							0x00000200UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_CANCEL_ON_LOST_LINK				0x00000400UL
#define	BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_ALL									0xFFFFFFFFUL


/*---------------------------------------------------------------------------
 * BtlBppsndGetJobAttributesMask type
 *
 */
typedef U32	BtlBppsndGetJobAttributesMask;

#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_STATE							0x00000001UL
#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_NAME							0x00000002UL
#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME		0x00000004UL
#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_MEDIA_SHEETS_COMPLETED		0x00000008UL
#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_NUM_OF_INTERVENING_JOBS			0x00000010UL
#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ID								0x00000020UL
#define	BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL									0xFFFFFFFFUL


/*---------------------------------------------------------------------------
 * BtlBppsndPrinterState type
 *
 */
typedef enum _BtlBppsndPrinterState
{
	BTL_BPPSND_PRINTER_STATE_IDLE					=		1,
	BTL_BPPSND_PRINTER_STATE_PROCESSING			=		2,
	BTL_BPPSND_PRINTER_STATE_STOPPED				=		3
} BtlBppsndPrinterState;

/*---------------------------------------------------------------------------
 * BtlBppsndPrinterStateReasons type
 *
 */
typedef enum _BtlBppsndPrinterStateReasons
{
	BTL_BPPSND_PRINTER_STATE_REASON_NONE						=	1,
	BTL_BPPSND_PRINTER_STATE_REASON_ATTENTION_REQUIRED		=	2,
	BTL_BPPSND_PRINTER_STATE_REASON_MEDIA_JAM					=	3,
	BTL_BPPSND_PRINTER_STATE_REASON_PAUSED					=	4,
	BTL_BPPSND_PRINTER_STATE_REASON_DOOR_OPEN				=	5,
	BTL_BPPSND_PRINTER_STATE_REASON_MEDIA_LOW				=	6,
	BTL_BPPSND_PRINTER_STATE_REASON_MEDIA_EMPTY				=	7,
	BTL_BPPSND_PRINTER_STATE_REASON_OUTPUT_AREA_ALMOST_FL	=	8,
	BTL_BPPSND_PRINTER_STATE_REASON_OUTPUT_AREA_FL			=	9,
	BTL_BPPSND_PRINTER_STATE_REASON_MARKER_SUPPLY_LOW		=	10,
	BTL_BPPSND_PRINTER_STATE_REASON_MARKER_SUPPLY_EMPTY		=	11,
	BTL_BPPSND_PRINTER_STATE_REASON_MARKER_FAILURE			=	12
} BtlBppsndPrinterStateReasons;

/*---------------------------------------------------------------------------
 * BtlBppsndPrintSides type
 *
 */
typedef enum  _BtlBppsndPrintSides
{
	BTL_BPPSND_PRINT_SIDES_ONE_SIDED					=		1,
	BTL_BPPSND_PRINT_SIDES_TWO_SIDED_LONG_EDGE		=		2,
	BTL_BPPSND_PRINT_SIDES_TWO_SIDED_SHORT_EDGE		=		3
} BtlBppsndPrintSides;


/*---------------------------------------------------------------------------
 * BtlBppsndPrintOrientation type
 *
 */
typedef enum  _BtlBppsndPrintOrientation
{
	BTL_BPPSND_PRINT_ORIENTATION_PORTRAIT				=			1,
	BTL_BPPSND_PRINT_ORIENTATION_LANDSCAPE			=			2,
	BTL_BPPSND_PRINT_ORIENTATION_REVERSE_LANDSCAPE	=			3,	
	BTL_BPPSND_PRINT_ORIENTATION_REVERSE_PORTRAIT		=			4
} BtlBppsndPrintOrientation;

/*---------------------------------------------------------------------------
 * BtlBppsndPrintQuality type
 *
 */
typedef enum  _BtlBppsndPrintQuality
{
	BTL_BPPSND_PRINT_QUALITY_DRAFT						=			1,
	BTL_BPPSND_PRINT_QUALITY_NORMAL					=			2,
	BTL_BPPSND_PRINT_QUALITY_HIGH						=			3
} BtlBppsndPrintQuality;

/*---------------------------------------------------------------------------
 * BtlBppsndJobState type
 *
 */
typedef enum  _BtlBppsndJobState
{
	BTL_BPPSND_JOB_STATE_PRINTING						=			1,
	BTL_BPPSND_JOB_STATE_WAITING						=			2,
	BTL_BPPSND_JOB_STATE_STOPPED						=			3,
	BTL_BPPSND_JOB_STATE_COMPLETED						=			4,
	BTL_BPPSND_JOB_STATE_ABORTED						=			5,
	BTL_BPPSND_JOB_STATE_CANCELLED						=			6,
	BTL_BPPSND_JOB_STATE_UNKNOWN						=			7
} BtlBppsndJobState;

/*---------------------------------------------------------------------------
 * BtlBppsndJobOperationStatus type
 *
 */
typedef enum _BtlBppsndJobOperationStatus
{

	BTL_BPPSND_JOB_OPER_STATUS_SUCCESSFUL_OK											=	0X0000,
	BTL_BPPSND_JOB_OPER_STATUS_SUCCESSFUL_OK_IGNORED_OR_SUBSTITUTED_ATTRIBUTES	=	0X0001,
	BTL_BPPSND_JOB_OPER_STATUS_SUCCESSFUL_OK_CONFLICTING_ATTRIBUTES				=	0X0002,

	BTL_BPPSND_JOB_OPER_STATUS_CLIENT_ERROR_BAD_REQUEST				=	0X0400,
	BTL_BPPSND_JOB_OPER_STATUS_CLIENT_ERROR_FORBIDDEN				=	0X0401,
	BTL_BPPSND_JOB_OPER_STATUS_CLIENT_ERROR_NOT_AUTHENTICATED		=	0X0402,
	BTL_BPPSND_JOB_OPER_STATUS_CLIENT_ERROR_NOT_AUTHORIZED			=	0X0403,

	BTL_BPPSND_JOB_OPER_STATUS_CLIENT_FIRST_ERROR						=	0X0400,
	BTL_BPPSND_JOB_OPER_STATUS_CLIENT_LAST_ERROR						=	0X0418,
	BTL_BPPSND_JOB_OPER_STATUS_SERVER_FIRST_ERROR						=	0X0500,
	BTL_BPPSND_JOB_OPER_STATUS_SERVER_LAST_ERROR						=	0X0509
} BtlBppsndJobOperationStatus;

/*-------------------------------------------------------------------------------
 *
 *     various string definitions
 */
typedef	U8 BtlBppsndDocumentFormat[BTL_BPPSND_MAX_DOCUMENT_FORMAT_LEN + 1];
typedef	U8 BtlBppsndImageFormat[BTL_BPPSND_MAX_IMAGE_FORMAT_LEN + 1];
typedef U8 BtlMediaSize[BTL_BPPSND_MAX_MEDIA_SIZE_LEN+1];
typedef U8 BtlMediaType[BTL_BPPSND_MAX_MEDIA_TYPE_LEN+1];
typedef U8 BtlJobName[BTL_BPPSND_MAX_JOB_NAME_LEN + 1];
typedef U8 BtlJobOriginatingUserName[BTL_BPPSND_MAX_JOB_ORIGINATING_USER_NAME_LEN + 1];


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlBppsndEvent structure
 *
 *     Represents BTL BPP Sender event.
 */
struct _BtlBppsndEvent 
{
	BtlBppsndContext			*bppsndContext;
	const BppCallbackParms	*bppsndCallbackParms;
};

/*-------------------------------------------------------------------------------
 * BtlMediaLoadedDetails structure
 *
 *     Represents LoadedMediumDetails Printer Attribute as defined in the BPP specification
 */
 typedef struct _BtlMediaLoadedDetails	
{
	BtlMediaSize LoadedMediumSize;
	BtlMediaType LoadedMediumType;
} BtlMediaLoadedDetails;

/*-------------------------------------------------------------------------------
 * BtlBppsndPrinterAttributes structure
 *
 *     Represents Printer Attribute as defined in the BPP specification
 */
typedef struct _BtlBppsndPrinterAttributes
{
	U8							printerName[BTL_BPPSND_MAX_PRINTER_NAME_LEN + 1];
	U8							printerLocation[BTL_BPPSND_MAX_PRINTER_LOCATION_LEN + 1];
	BtlBppsndPrinterState			printerState;
	BtlBppsndPrinterStateReasons	printerStateReasons;
	
	BtlBppsndDocumentFormat		documentFormatsSupported[
									BTL_BPPSND_MAX_NUM_OF_SUPPORTED_DOCUMENT_FORMATS];
	U32							numDocumentFormatsSupported;
	BOOL						isColorSupported;
	U32							maxCopiesSupported;
	BtlBppsndPrintSides			sidesSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_SIDES];
	U32							numSidesSupported;
	U32							NumberUpSupported;
	BtlBppsndPrintOrientation		orientationsSupported [BTL_BPPSND_MAX_NUM_OF_SUPPORTED_ORIENTATIONS];
	U32							NumberorientationsSupported;
	BtlMediaSize					mediaSizesSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_SIZES];
	U32							numMediaSizesSupported;
	BtlMediaType					mediaTypesSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_TYPES];
	U32							numMediaTypesSupported;
	BtlMediaLoadedDetails			mediaLoadedDetails[BTL_BPPSND_MAX_NUM_OF_MEDIA_LOADED];
	U32							numMediaLoaded;
	BtlBppsndPrintQuality			PrintQualitySupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_PRINT_QUALITY];
	U32							numPrintQualitySupported;
	U32							queuedJobCount;
	BtlBppsndImageFormat			imageFormatsSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_IMAGE_FORMATS];
	U32							numImageFormatsSupported;
	U32							basicTextPageWidth;
	U32							basicTextPageHeight;
	U8							printerGeneralCurrentOperator[
									BTL_BPPSND_MAX_PRINTER_GENERAL_CURRENT_OPERATOR+1];
	BtlBppsndJobOperationStatus	operationStatus;
} BtlBppsndPrinterAttributes;

/*-------------------------------------------------------------------------------
 * BtlBppsndCreateJobRequestData structure
 *
 *     Represents CreateJob data as defined in the BPP specification
 */
typedef struct _BtlBppsndCreateJobRequestData
{
	BtlJobName				jobName;
	BtlJobOriginatingUserName	jobOriginatingUserName;
	BtlBppsndDocumentFormat	documentFormat;
	U32						numOfCopies;
	BtlBppsndPrintSides		sides;
	U32						numberUp;
	BtlBppsndPrintOrientation	orientationRequested;
	BtlMediaSize				mediaSize;
	BtlBppsndPrintQuality		printQuality;
	BtlMediaType				mediaType;
	BOOL					cancelOnLostLink;
} BtlBppsndCreateJobRequestData;

/*-------------------------------------------------------------------------------
 * BtlBppsndCreateJobResponseData structure
 *
 *     Represents CreateJob response data as defined in the BPP specification
 */
typedef struct _BtlBppsndCreateJobResponseData {
	BtlBppsndJobId				jobId;
	BtlBppsndJobOperationStatus	operationStatus;
} BtlBppsndCreateJobResponseData;

/*-------------------------------------------------------------------------------
 * BtlBppsndGetJobAttributesResponseData structure
 *
 *     Represents GetJobAttributes response data as defined in the BPP specification
 */
typedef struct _BtlBppsndGetJobAttributesResponseData
{
	BtlBppsndJobId			jobId;
	BtlBppsndJobState			jobState;
	BtlJobName				jobName;
	BtlJobOriginatingUserName	jobOriginatingUserName;
	U32						jobMediaSheetsCompleted;
	U32						numOfInterveningJobs;
	BtlBppsndJobOperationStatus		operationStatus;
} BtlBppsndGetJobAttributesResponseData;

/*-------------------------------------------------------------------------------
 * BtlBppsndCancelJobResponseData structure
 *
 *     Represents CancelJob response data as defined in the BPP specification
 */
typedef struct _BtlBppsndCancelJobResponseData {
	BtlBppsndJobId				jobId;
	BtlBppsndJobOperationStatus	operationStatus;
} BtlBppsndCancelJobResponseData;

/*-------------------------------------------------------------------------------
 * BtlBppsndGetEventResponseData structure
 *
 *     Represents GetEvent response data as defined in the BPP specification
 */
typedef struct _BtlBppsndGetEventResponseData
{
	BtlBppsndJobId				jobId;
	BtlBppsndJobState				jobState;
	BtlBppsndPrinterState			printerState;
	BtlBppsndPrinterStateReasons	printerStateReasons;
	BtlBppsndJobOperationStatus	operationStatus;
} BtlBppsndGetEventResponseData;


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------

 * BTL_BPPSND_Init()
 *
 * Brief:  
 *                Init the BPPSND module.
 *
 * Description:
 *                Init the BPPSND module.
 *
 * Type:
 *                Synchronous
 *
 * Parameters:
 *                void.
 *
 * Returns:
 *                BT_STATUS_SUCCESS - Operation is successful.
 *
 *                BT_STATUS_FAILED - The operation failed.
 *                
 *                BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_BPPSND_Init(void);

/*-------------------------------------------------------------------------------

 * BTL_BPPSND_Deinit()
 *
 * Brief:  
 *                Deinit the BPPSND module.
 *
 * Description:
 *                Deinit the BPPSND module.
 *
 * Type:
 *                Synchronous
 *
 * Parameters:
 *                void.
 *
 * Returns:
 *                BT_STATUS_SUCCESS - Operation is successful.
 *
 *                BT_STATUS_FAILED - The operation failed.
 *                
 *                BT_STATUS_INTERNAL_ERROR - Internal error has occur.
 */
BtStatus BTL_BPPSND_Deinit(void);
 
/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Create()
 *
 * Brief:  
 *		Allocates a unique BPP Sender context.
 *
 * Description:
 *		Allocates a unique BPP Sender context.
 *		This function must be called before any other BPP Sender API function.
 *		The allocated context should be supplied in subsequent BPP Sender API calls.
 *		The caller must also provide a callback function, which will be called 
 *		on BPP Sender events.
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
 *		bppsndCallback [in] - all BPP Sender events will be sent to this callback.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote BPP printer. For default value, pass null.
 *		
 *		bppsndContext [out] - allocated BPP Sender context.	
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BPP Sender context was created successfully.
 *
 *		BT_STATUS_FAILED -  Failed to create BPP Sender context.
 */
BtStatus BTL_BPPSND_Create(	BtlAppHandle 				*appHandle,
									const BtlBppsndCallBack 	bppsndCallback,
									const BtSecurityLevel 		*securityLevel,
									BtlBppsndContext 			**bppsndContext);


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Destroy()
 *
 * Brief:  
 *		Releases a BPP Sender context.
 *
 * Description:
 *		Releases a BPP Sender context (previously allocated with BTL_BPPSND_Create).
 *		An application should call this function when it completes using BPP Sender services.
 *		Upon completion, the BPP Sender context is set to null in order to prevent 
 *		the application from an illegal attempt to keep using it.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in/out] - BPP Sender context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - BPP Sender context was destroyed successfully.
 *
 *		BT_STATUS_FAILED -  Failed to destroy BPP Sender context.
 */
BtStatus BTL_BPPSND_Destroy(BtlBppsndContext **bppsndContext);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Enable()
 *
 * Brief:  
 *		Enable BPPSND.
 *
 * Description:
 *		Enable BPPSND, called after BTL_BPPSND_Create.
 *           An SDP record is registered for the Sender serving as server for referenced objects.
 *		After calling this function, BPP sender is ready for usage.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPPSND context.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_Enable(BtlBppsndContext *bppsndContext);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Disable()
 *
 * Brief:  
 *		Disable BPPSND.
 *
 * Description:
 *		Disable BPPSND, called before BTL_BPPSND_Destroy.
 *		If a connection exists, it will be disconnected automatically.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPPSND context.
 *
 * Generated Events:
 *		BPPCLIENT_COMPLETE (bppOp=BPPOP_DISCONNECT)
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_PENDING - The link is connected. The disconnect process has 
 *			been successfully started. When the disconnect process is complete, the
 *         	application callback will receive the disconnect event,
 *			which indicates the BPPSND is disabled.
 */
BtStatus BTL_BPPSND_Disable(BtlBppsndContext *bppsndContext);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Connect()
 *
 * Brief:  
 *		Create a transport connection to the specified device.
 *
 * Description:
 *		This function is used by the sender to create a transport connection
 *     	to the specified device. This function establishes 2 Sender channels to the Printer:
 *		One BPP Job channel and another for the BPP status channel.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_CONNECT)
 *           BPPCLIENT_AUTH_CHALLENGE_RCVD
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Connection was successfully started. Completion
 *         	will be signaled via a call to the application callback.
 *
 *		BT_STATUS_SUCCESS - The sender is now connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 */
BtStatus BTL_BPPSND_Connect(	BtlBppsndContext 	*bppsndContext, 
									const BD_ADDR 	*bdAddr);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Disconnect()
 *
 * Brief:  
 *		Disconnect all Sender channels from the Printer
 *
 * Description:
 *		This function is used by the sender to disconnect all Sender channels from the Printer
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_DISCONNECT)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 */
BtStatus BTL_BPPSND_Disconnect(BtlBppsndContext *bppsndContext);

/*-------------------------------------------------------------------------------
 * BTL_OPPS_GetConnectedDevice()
 *
 * Brief:  
 *                Returns the connected device.
 *
 * Description:
 *                This function returns the connected device.
 *
 * Type:
 *                Synchronous
 *
 * Parameters:
 *                bppsndContext [in] - BPP Sender context.
 *
 *                bdAddr [out] - pointer to 48-bit address of the connected device.
 *
 * Returns:
 *                BT_STATUS_SUCCESS - Operation is successful.
 *
 *                BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *                BT_STATUS_NO_CONNECTION -  The server is not connected.
 */
BtStatus BTL_BPPSND_GetConnectedDevice(BtlBppsndContext *bppsndContext,  BD_ADDR *bdAddr);


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_SimplePushPrint()
 *
 * Brief:  
 *		Initiates a BPP Simple Push operation.
 *
 * Description:
 *		Initiates a BPP Simple Push operation to print the specified object on the remote printer.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		objToPrint [in] - Object to print  (see BtlObject for details).
 *			The pointer is free after the function returns.
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_SIMPLEPUSH)
 *           BPPCLIENT_PROGRESS
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_SimplePushPrint(	BtlBppsndContext 	*bppsndContext, 
											const BtlObject 	*objToPrint);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ConnectSimplePushPrintDisconnect()
 *
 * Brief:  
 *		Encapsulates connecting to a Printer, Simple-Push an object, and disconnecting
 *           in a single API call
 *
 * Description:
 *		This function is used by the Sender to connect to a Printer, send an object
 *		using Simple-Push transfer model, and disconnect from the Printer.
 *		This function is a combined operation of 3 functions: 
 *		BTL_BPPSND_Connect(), BTL_BPPSND_Print() and BTL_BPPSND_Disconnect().
 *
 *		Separate events will be sent to the application callback. See the description of the
 *		individual function for details.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		bdAddr [in] - pointer to 48-bit address of the device.
 *
 *		objToPrint [in] - Object to print (see BtlObject for details).
 *			The pointer is free after the function returns.
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE
 *           BPPCLIENT_PROGRESS
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_ConnectSimplePushPrintDisconnect(
			BtlBppsndContext 	*bppsndContext, 
			const BD_ADDR 	*bdAddr, 
			const BtlObject 	*objToPrint);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ObexAuthenticationResponse()
 *
 * Brief:  
 *		Provides seponse to an OBEX authentication challenge
 *
 * Description:
 *		This function must be called after receiving OBEX authentication 
 *		challenge from the printer.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPPSND context.
 *
 *		userId [in] - null-terminated string used with OBEX authentication 
 *			representing the user id. If the argument is not needed by the 
 *			client (see in event), NULL can be passed.
 *
 *		password [in] - null-terminated string used with OBEX authentication 
 *			representing the password. Pass NULL to deny authentication.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 *
 *		BT_STATUS_INVALID_PARM - In case of null context or password, or too long strings 
 */
BtStatus BTL_BPPSND_ObexAuthenticationResponse(	BtlBppsndContext 	*bppsndContext, 
													const BtlUtf8	*userId, 
													const BtlUtf8	*password);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_Abort()
 *
 * Brief:  
 *		Aborts the current sender operations.
 *
 * Description:
 *		Aborts the current sender operations on the applicable channels (job and / or status channels).
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 * Generated Events:
 *           BPPCLIENT_ABORT
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation was successful.
 *
 *		BT_STATUS_FAILED - Operation failed to start.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_Abort(BtlBppsndContext 	*bppsndContext);

	
/*-------------------------------------------------------------------------------
 * BTL_BPPSND_GetReferenceObjectResponse()
 *
 * Brief:  
 *		Response for a previous request for referenced object data
 *
 * Description:
 *		Response for a previous request for referenced object data
 *
 *		When the application receives the referenced object request, it should
 *		use the specified object identifier, and locate the referenced object file.
 *		It should then call this function, and specify the file's properties, so the 
 *		BTL can response to the request.
 *
 *		Note that the referenced object request has no provisions to identify the
 *		printed file that contains the reference. Note also that the printer may 
 *		request referenced object data after the sender disconnected from the
 *		printer. Therefore, when printing a file that may contain referenced objects,
 *		the application should detect when a print job completed, before starting another
 *		one that may also contain referenced objects. The detection could be done
 *		using job-based capabilities.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		responseObj [in] - object describing the properties of the retrieved referenced object.
 *						the referenced object MUST be a file on the file system
 *						
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation was successful.
 *
 *		BT_STATUS_FAILED - Operation failed to start.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_GetReferenceObjectResponse(	BtlBppsndContext 	*bppsndContext,
															const BtlObject	*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_GetPrinterAttributes()
 *
 * Brief:  
 *		Initiates a BPP GetPrinterAttributes operation.
 *
 * Description:
 *		Initiates a BPP GetPrinterAttributes operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		attributesMask [in] - Mask of printer attributes to request from the printer. 
 *						   The caller should set the bits in the mask that correspond to features
 *							in which it is interested. Multiple features may be OR'ed together.
 *					
 *							The pointer is free after the function returns
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_GETPRTATTR)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_GetPrinterAttributes(
			BtlBppsndContext 				*bppsndContext,
			BtlBppsndPrinterAttributesMask	attributesMask,											
			const BtlObject 				*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ParseGetPrinterAttributesResponse()
 *
 * Brief:  
 *		Parse a SOAP XML GetPrinterAttributes response 
 *
 * Description:
 *		Parse a SOAP XML GetPrinterAttributes response 
 *
 *		This function parses the response, and sets the available attribute values in 
 *		attributesData.
 *
 *		The function returns an attributesMask that indicates which values are present
 *		in attributesData. Only those attributes for which the corresponding bit is set in
 *		attributesMask should be accessed in attributesData. The values of all other 
 *		attributes is undefined.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		responseObj [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the present attributes.
 *
 *		attributesMask [out] - Indicates which attributes in attributesData are valid
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_ParseGetPrinterAttributesResponse(
			BtlBppsndContext 				*bppsndContext, 
			const BtlObject 				*responseObj, 
			BtlBppsndPrinterAttributes		*attributesData,
			BtlBppsndPrinterAttributesMask	*attributesMask);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_CreateJob()
 *
 * Brief:  
 *		Initiates a BPP CreateJob operation.
 *
 * Description:
 *		Initiates a BPP CreateJob operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		attributesData [in] - Structure that contains create job attribute values. The caller may set
 *							any subset of attributes that it wishes to control in the print job.
 *							It signals those features by setting the corresponding bits in 
 *							attributesMask.
 *							The caller may also not specify any job attribute (pass 0). In that case,
 *							the printer will create a job using its default settings.
 *					
 *							The pointer is free after the function returns
 *
 *		attributesMask [in] - Mask of attributes that are valid in the attributesData structure.
 *							Multiple features may be OR'ed together. 
 *							If attributesData is 0, then this argument should be 0 as well
 *					
 *							The pointer is free after the function returns
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_CREATEJOB)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_CreateJob(BtlBppsndContext 						*bppsndContext,
									const BtlBppsndCreateJobRequestData		*attributesData,
									const BtlBppsndCreateJobAttributesMask	*attributesMask,
									const BtlObject 						*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ParseCreateJobResponse()
 *
 * Brief:  
 *		Parse a SOAP XML CreateJob response 
 *
 * Description:
 *		Parse a SOAP XML CreateJob response 
 *
 *		This function parses the response, and sets the available attribute values in 
 *		attributesData.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		responseObj [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the response attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_ParseCreateJobResponse(
			BtlBppsndContext 					*bppsndContext,
			const BtlObject 					*responseObj,
			BtlBppsndCreateJobResponseData	*attributesData);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_SendDocument()
 *
 * Brief:  
 *		Initiates a BPP Simple Push operation.
 *
 * Description:
 *		Initiates a BPP Simple Push operation to print the specified object on the remote printer.
 *		This function can be called only after creating a dedicated job via BTL_BPPSND_CreateJob. 
 *		Only a single document may be sent to printing for every job created.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		jobId [in] - Job ID obtained via the response event to the corresponding BTL_BPPSND_CreateJob request
 *
 *		objToPrint [in] - Object to print  (see BtlObject for details).
 *			The pointer is free after the function returns.
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_SENDDOCUMENT)
 *           BPPCLIENT_PROGRESS
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_SendDocument(	BtlBppsndContext 	*bppsndContext,
											BtlBppsndJobId	jobId,
											const BtlObject 	*objToPrint);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_GetJobAttributes()
 *
 * Brief:  
 *		Initiates a BPP GetJobAttributes operation.
 *
 * Description:
 *		Initiates a BPP GetJobAttributes operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		jobId [in] - Job ID obtained via the response event to the corresponding BTL_BPPSND_CreateJob request
 *
 *		attributesMask [in] - Mask of printer attributes to request from the printer. 
 *						   The caller should set the bits in the mask that correspond to features
 *							in which it is interested. Multiple features may be OR'ed together.
 *					
 *							The pointer is free after the function returns
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_GETJOBATTR)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_GetJobAttributes(	BtlBppsndContext 					*bppsndContext,
											BtlBppsndJobId					jobId,
											const BtlBppsndGetJobAttributesMask	*attributesMask,
											const BtlObject 					*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ParseGetJobAttributesResponse()
 *
 * Brief:  
 *		Parse a SOAP XML GetJobAttributes response 
 *
 * Description:
 *		Parse a SOAP XML GetJobAttributes response 
 *
 *		This function parses the response, and sets the available attribute values in 
 *		attributesData.
 *
 *		The function returns an attributesMask that indicates which values are present
 *		in attributesData. Only those attributes for which the corresponding bit is set in
 *		attributesMask should be accessed in attributesData. The values of all other 
 *		attributes is undefined.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		responseObj [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the present attributes.
 *
 *		attributesMask [out] - Indicates which attributes in attributesData are valid
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_ParseGetJobAttributesResponse(
			BtlBppsndContext 						*bppsndContext, 
			const BtlObject 						*responseObj, 
			BtlBppsndGetJobAttributesResponseData	*attributesData,
			BtlBppsndGetJobAttributesMask			*attributesMask);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_CancelJob()
 *
 * Brief:  
 *		Cancels a particular print job
 *
 * Description:
 *		Cancels a particular print job
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		jobId [in] - Job ID obtained via the response to the corresponding BTL_BPPSND_CreateJob request
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_CANCELJOB)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Repeated results
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation on the status channel.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_CancelJob(	BtlBppsndContext 	*bppsndContext,
										BtlBppsndJobId	jobId,
										const BtlObject 	*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ParseCancelJobResponse()
 *
 * Brief:  
 *		Parse a SOAP XML CancelJob response 
 *
 * Description:
 *		Parse a SOAP XML CancelJob response 
 *
 *		This function parses the response, and sets the available attribute values in 
 *		attributesData.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		responseObj [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the response attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_ParseCancelJobResponse(
			BtlBppsndContext 					*bppsndContext,
			const BtlObject 					*responseObj,
			BtlBppsndCancelJobResponseData	*attributesData);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_StartGetEvent()
 *
 * Brief:  
 *		Starts a GetEvent operation on the status channel.
 *
 * Description:
 *		Starts a GetEvent operation on the status channel.
 *		Only a single operation may execute on the status channel simultaneously. The GetEvent
 *		allows monitoring of the state of a specific job (identified by its jobId).
 *
 *		The application callback will be signaled with the new printer & job status whenever there is
 *		a change in their state.
 *
 *		The application should call BTL_BPPSND_StopGetEvent to stop monitoring the state. The monitoring will also
 *		stop upon a call to any of the following functions: BTL_BPPSND_GetJobAttributes, BTL_BPPSND_CancelJob, and 
 *		BTL_BPPSND_GetPrinterAttributes. Whenever monitoring stops, a corresponding event shall be sent to the
 *		application callback.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		jobId [in] - Job ID obtained via the response to the corresponding BTL_BPPSND_CreateJob request
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Generated Events:
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. Repeated results
 *         will be signaled via an event to the application callback.
 *
 *		BT_STATUS_BUSY - Operation was not started because
 *         the sender is currently executing another operation on the status channel.
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_StartGetEvent(BtlBppsndContext 	*bppsndContext,
										BtlBppsndJobId	jobId,
										const BtlObject	*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_StopGetEvent()
 *
 * Brief:  
 *		Stops the currently active GetEvent operation on the status channel.
 *
 * Description:
 *		Stops the currently active GetEvent operation on the status channel.
 *		A jobId is not required since there may be only one simultaneous active GetEvent operation.
 *
 *		The application will receive a corresponding event when the GetEvent stops.
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_GETEVENT)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 *
 * Returns:
 *		BT_STATUS_PENDING - Operation was successfully started. GetEvent termination will be reported to the
 *							application callback
 *
 *		BT_STATUS_NO_CONNECTION -  Operation was not started because
 *         the sender is not connected.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_StopGetEvent(BtlBppsndContext 	*bppsndContext);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_ParseGetEventResponse()
 *
 * Brief:  
 *		Parse a SOAP XML GetEvent response 
 *
 * Description:
 *		Parse a SOAP XML GetEvent response 
 *		This function parses the response, and sets the attribute values in 
 *		attributesData.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		responseObj [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the response attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_ParseGetEventResponse(
				BtlBppsndContext 				*bppsndContext,
				const BtlObject 				*responseObj,
				BtlBppsndGetEventResponseData	*attributesData);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_IsGetEventActive()
 *
 * Brief:  
 *		Checks if there is an active GetEvent operation.
 *
 * Description:
 *		Checks if there is an active GetEvent operation on the status channel.
 *		A jobId is not required (as input) since there may be only one simultaneous active GetEvent operation.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		answer [out] - TRUE in case GetEvent is active, FALSE otherwise.
 *
 *		jobId [out] - In case the answer is TRUE, contains the jobId of the monitored job (the one specified when calling 
 *					BTL_BPPSND_StartGetEvent)
 *
 * Returns:
 *
 *		B_STATUS_SUCCESS - Operation was successful
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus BTL_BPPSND_IsGetEventActive(	BtlBppsndContext 	*bppsndContext,
											BOOL			*answer,
											BtlBppsndJobId	*jobId);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_SendSoapRequest()
 *
 * Brief:  
 *		Initiates a SOAP Request command from the Sender.
 *
 * Description:
 *		This function is an alternative to the explicit SOAP request functions. It is the caller's
 *		responsibility to build a valid XML SOAP request.
 *		
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *		bppsndContext [in] - BPP Sender context.
 *
 *		requestObj [in] - The location of the contents of the SOAP request to send
 *						The pointer is free after the function returns
 *
 *		responseObj [in] - The location where the SOAP response will be saved
 *							The pointer is free after the function returns
 *
 * Generated Events:
 *           BPPCLIENT_COMPLETE (bppOp=BPPOP_SOAP)
 *           BPPCLIENT_ABORT
 *           BPPCLIENT_FAILED
 *
 *
 * Returns:
 *
 *		BT_STATUS_PENDING -   The SOAP request was started
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 *
 *		BT_STATUS_FFS_ERROR - failed to open or read SOAP file
 */
BtStatus BTL_BPPSND_SendSoapRequest(	BtlBppsndContext 	*bppsndContext,
												const BtlObject 	*requestObj,
												const BtlObject	*responseObj);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_SetSecurityLevel()
 *
 * Brief:  
 *     	Sets security level for the given BPP Sender context.
 *
 * Description:
 *     	Sets security level for the given BPP Sender context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	bppsndContext [in] - pointer to the BPP Sender context.
 *
 *		securityLevel [in] - level of security which should be applied,	when
 *			connecting to a remote OPP server. For default value, pass null.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_SetSecurityLevel(BtlBppsndContext *bppsndContext,
								  		const BtSecurityLevel *securityLevel);


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_GetSecurityLevel()
 *
 * Brief:  
 *     	Gets security level for the given BPP Sender context.
 *
 * Description:
 *     	Gets security level for the given BPP Sender context.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	bppsndContext [in] - pointer to the BPP Sender context.
 *
 *		securityLevel [out] - level of security which should be applied, when
 *			connecting to a remote OPP server.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus BTL_BPPSND_GetSecurityLevel(	BtlBppsndContext 	*bppsndContext,
								  			BtSecurityLevel 	*securityLevel);

#endif /* __BTL_BPPSND_H */


