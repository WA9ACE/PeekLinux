/*******************************************************************************\
*                                                                                                                                        	*
*  	TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  	LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		       *
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      Btl_bppsnd_soapBuilder.c
*
*   DESCRIPTION:  
*				
*					
*
*   AUTHOR:         Zvi Schneider
*
\*******************************************************************************/

#include "btl_config.h"

#if BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_bppsnd_soap.h"
#include "btl_bppsnd.h"
#include "osapi.h"
 #include "oslib.h"

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

 /********************************************************************************
 *
 * Types
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/



/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/
U16	getsoapCreateJobReqContentLength(const BtlBppsndCreateJobAttributesMask attributesMask , 
									const BtlBppsndCreateJobRequestData	*attributesData);
U16 	getsoapPrinterAttrReqContentLength(BtlBppsndPrinterAttributesMask	attributesMask);
U16 getsoapJobAttrReqContentLength(BtlBppsndPrinterAttributesMask	attributesMask, BtlBppsndJobId	 jobId);
U32 getNumberLength(U32 num);
void intToString(U8 *stNum, U32 num, U32 numLength);
void builedHead(U16 contentLength , char *soapAction , U8 *soapPtr , U16 *length ,U32 *index);


void insertStringStartBodyEnd(char *start , char *body , char *end , U8 *soapPtr , U32 *index);
void insertStringStartBodyEndL(char *start , char *body , char *end , U8 *soapPtr , U32 startLen, U32 endLen, U32 *index);

void copyAndMoveIndex(U32 *index, char *strFrom, U8 *strTo);
void copyAndMoveIndexL(U32 *index, char *strFrom, char *strTo, U32 length);

void insertStringStartNumberEnd(char *start , U32 num , char *end , U8 *soapPtr , U32 *index);

/*******************************************************************************
*
*	Function definitions
*
*******************************************************************************/


/*-------------------------------------------------------------------------------
 * getsoapPrinterAttrReqContentLength()
 *
 *		
 *		returns the length of the request file that will be created
 *		
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] - the mask of the attributes thet should be requested. 
 *
 *
 * Returns:
 *		OS_StrLen
 *		U16 - length of the file
 *		
 */
U16 getsoapPrinterAttrReqContentLength(BtlBppsndPrinterAttributesMask	attributesMask)
{
	U16 attrLength = 0,attrStartLength = 0,attrEndLength = 0;
	
	attrLength			= (U16)(OS_StrLen(ENVELOPE_START) + OS_StrLen(BODY_START_GET_PRINTER_ATTR) + 
																		OS_StrLen(ENVELOPE_BODY_END_GET_PRINTER_ATTR));
	
	attrStartLength		= (U16) OS_StrLen(PRINTER_ATTRIBUTE_START);
	attrEndLength			= (U16) OS_StrLen(PRINTER_ATTRIBUTE_END); 

	if(!((attributesMask&BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL) == BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL))
	{	/*	If we need all attr then by defualt if we don't request any attr then we will get them all	*/	
		attrLength = (U16)(attrLength + OS_StrLen(REQ_PRINTER_ATTRIBUTES_START) + OS_StrLen(REQ_PRINTER_ATTRIBUTES_END));
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_HEIGHT)
		{
			attrLength = (U16)(attrLength + OS_StrLen(BASIC_TEXT_PAGE_HEIGHT) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_WIDTH)
		{
			attrLength = (U16)(attrLength + OS_StrLen(BASIC_TEXT_PAGE_WIDTH) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_COLOR_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(COLOR_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_DOCUMENT_FORMATS_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINTER_DOCUMENTFORMATS_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_IMAGE_FORMATS_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(IMAGE_FORMATS_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MAX_COPIES_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(MAX_COPIES_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_LOADED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(MEDIA_LOADED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_SIZES_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(MEDIA_SIZES_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_TYPES_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(MEDIA_TYPES_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_NUMBER_UP_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(NUMBER_UP_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ORIENTATIONS_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(ORIENTATIONS_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_GENERAL_CURRENT_OPERATOR)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINTER_GENERAL_CURRENT_OPERATOR) + attrStartLength + attrEndLength);
		}
		if((attributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_LOCATION)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINTER_LOCATION) + attrStartLength + attrEndLength);
		}
		if(attributesMask &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_NAME)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINTER_NAME) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINTER_STATE) + attrStartLength + attrEndLength);
		}
		if(attributesMask &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE_REASONS)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINTER_STATE_REASONS) + attrStartLength + attrEndLength);
		}
		if(attributesMask &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINT_QUALITY_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(PRINT_QUALITY_SUPPORTED) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_QUEUED_JOB_COUNT)
		{
			attrLength = (U16)(attrLength + OS_StrLen(QUEUE_JOB_COUNT) + attrStartLength + attrEndLength);
		}
		if(attributesMask & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_SIDES_SUPPORTED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(SIDES_SUPPORTED) + attrStartLength + attrEndLength);
		}
	}
	return attrLength;
}



/*-------------------------------------------------------------------------------
 * buildsoapPrinterAttrRequest()
 *
 *		build the soap file requesting for the Printer Attributes.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] -  the mask of the attributes thet should be requested. 
 *
 *		soapPtr [out] - a buffer thet holdes the request string.
 *
 *		length [out] - the length of the buffer.
 *
 * Returns:
 *		
 *
 *		
 */

void buildsoapPrinterAttrRequest(BtlBppsndPrinterAttributesMask	attributesMask, U8 *soapPtr , U16 *length)
{
	U32	index=0, attributeStartLen=0, attributeEndLen=0;
	U16 contentLength = 0;	/*the length of the buffer */
	
	contentLength = getsoapPrinterAttrReqContentLength(attributesMask);

	builedHead(contentLength , SOAP_ACTION_GET_PRINTER_ATTR , soapPtr , length ,&index);

	copyAndMoveIndex(&index, BODY_START_GET_PRINTER_ATTR, soapPtr);
		
	if(!((attributesMask&BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL) == BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL))
	{	/*	If we need all attributes then by defualt if we don't request any attr then we will get them all	*/	

	/* Build Printer Attributes */
		copyAndMoveIndex(&index, REQ_PRINTER_ATTRIBUTES_START, soapPtr);
		
		attributeStartLen = OS_StrLen(PRINTER_ATTRIBUTE_START);				
		attributeEndLen	  = OS_StrLen(PRINTER_ATTRIBUTE_END); 	
			
		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_NAME)
		{
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINTER_NAME , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_LOCATION)
		{
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINTER_LOCATION , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE)
		{
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINTER_STATE , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE_REASONS)
		{
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINTER_STATE_REASONS , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_DOCUMENT_FORMATS_SUPPORTED)
		{
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINTER_DOCUMENTFORMATS_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_COLOR_SUPPORTED)
		{
				
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , COLOR_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MAX_COPIES_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , MAX_COPIES_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_SIDES_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , SIDES_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_NUMBER_UP_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , NUMBER_UP_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}


		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ORIENTATIONS_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , ORIENTATIONS_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_SIZES_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , MEDIA_SIZES_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}
		
		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_TYPES_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , MEDIA_TYPES_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_LOADED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , MEDIA_LOADED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINT_QUALITY_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINT_QUALITY_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_QUEUED_JOB_COUNT)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , QUEUE_JOB_COUNT , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_IMAGE_FORMATS_SUPPORTED)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , IMAGE_FORMATS_SUPPORTED , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_WIDTH)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , BASIC_TEXT_PAGE_WIDTH , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_HEIGHT)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , BASIC_TEXT_PAGE_HEIGHT , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}
			
		if((attributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_GENERAL_CURRENT_OPERATOR)
		{
			
			insertStringStartBodyEndL(PRINTER_ATTRIBUTE_START , PRINTER_GENERAL_CURRENT_OPERATOR , PRINTER_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		copyAndMoveIndex(&index, REQ_PRINTER_ATTRIBUTES_END,soapPtr);
		
	}
	copyAndMoveIndex(&index, ENVELOPE_BODY_END_GET_PRINTER_ATTR,soapPtr);
}



/*-------------------------------------------------------------------------------
 * getsoapJobAttrReqContentLength()
 *
 *
 *		returns the length of the request file that will be created.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] - the mask of the attributes thet should be requested.
 *
 *		jobId [in] - the job id.
 *
 *
 * Returns:
 *		
 *		U16 - length of the file.
 *		
 */

U16 getsoapJobAttrReqContentLength(BtlBppsndGetJobAttributesMask	attributesMask,BtlBppsndJobId	 jobId)
{
	U16 attrLength = 0,attrStartLength = 0,attrEndLength = 0;	
	
	attrLength			= (U16)(OS_StrLen(ENVELOPE_START) + OS_StrLen(BODY_START_GET_JOB_ATTR) + 
								 
								OS_StrLen(ENVELOPE_BODY_END_GET_JOB_ATTR));
	attrStartLength		= (U16) OS_StrLen(JOB_ATTRIBUTE_START);
	attrEndLength			= (U16) OS_StrLen(JOB_ATTRIBUTE_END); 

	attrLength 			=(U16)(attrLength + OS_StrLen(JOB_ID_START)+OS_StrLen(JOB_ID_END)+getNumberLength(jobId)); 	

	if(!((attributesMask&BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL)== BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL))
	{	/*	If we need all attr then by defualt if we don't request any attr then we will get them all	*/	
		attrLength = (U16)(attrLength + OS_StrLen(REQ_JOB_ATTRIBUTES_START) + OS_StrLen(REQ_JOB_ATTRIBUTES_END));
		
		if((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_STATE)
		{
			attrLength = (U16)(attrLength + OS_StrLen(JOB_STATE) + attrStartLength + attrEndLength);
		}
		if((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_NAME)
		{
			attrLength = (U16)(attrLength + OS_StrLen(JOB_NAME) + attrStartLength + attrEndLength);
		}
		if((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME)
		{
			attrLength = (U16)(attrLength + OS_StrLen(JOB_ORIGINATING_USER_NAME) + attrStartLength + attrEndLength);
		}
		if((attributesMask) &  BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_MEDIA_SHEETS_COMPLETED)
		{
			attrLength = (U16)(attrLength + OS_StrLen(JOB_MEDIA_SHEET_COMPLETED) + attrStartLength + attrEndLength);
		}
		if((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_NUM_OF_INTERVENING_JOBS)
		{
			attrLength = (U16)(attrLength + OS_StrLen(NUM_INTERVENING_JOBS) + attrStartLength + attrEndLength);
		}

	}
	return attrLength;
}
/*-------------------------------------------------------------------------------
 * buildsoapJobAttrRequest()
 *
 *		build the soap file requesting for the Job Attributes.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] -  the mask of the attributes thet should be requested. 
 *
 *		jobId [in] - the id of the job.		
 *
 *		soapPtr [out] - a buffer thet holdes the request string.
 *
 *		length [out] - the length of the buffer.
 */

void buildsoapJobAttrRequest(BtlBppsndGetJobAttributesMask	attributesMask, U8 *soapPtr , U16 *length, BtlBppsndJobId	 jobId)
{
	U32	index=0, attributeStartLen=0, attributeEndLen=0;
	U16 contentLength = 0;

	contentLength = getsoapJobAttrReqContentLength(attributesMask,jobId);
	

	builedHead(contentLength, SOAP_ACTION_GET_JOB_ATTR,soapPtr,length,&index);
	
	copyAndMoveIndex(&index, BODY_START_GET_JOB_ATTR, soapPtr);

	insertStringStartNumberEnd(JOB_ID_START, jobId, JOB_ID_END,  soapPtr, &index);

	
	if(!((attributesMask & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL) == BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL))
		{	/*	If we need all attr then by defualt if we don't request any attr then we will get them all	*/	

		/* Build Printer Attributes */
		copyAndMoveIndex(&index, REQ_JOB_ATTRIBUTES_START, soapPtr);

		attributeStartLen = OS_StrLen(JOB_ATTRIBUTE_START);				
		attributeEndLen	  = OS_StrLen(JOB_ATTRIBUTE_END); 	
			
		if((attributesMask) &  BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_NAME)
		{
		
			insertStringStartBodyEndL(JOB_ATTRIBUTE_START , JOB_NAME , JOB_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_STATE)
		{
			
			insertStringStartBodyEndL(JOB_ATTRIBUTE_START , JOB_STATE , JOB_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME)
		{
			
			insertStringStartBodyEndL(JOB_ATTRIBUTE_START , JOB_ORIGINATING_USER_NAME , JOB_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_MEDIA_SHEETS_COMPLETED)
		{
			
			insertStringStartBodyEndL(JOB_ATTRIBUTE_START , JOB_MEDIA_SHEET_COMPLETED , JOB_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}

		if((attributesMask) &  BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_NUM_OF_INTERVENING_JOBS)
		{
			
			insertStringStartBodyEndL(JOB_ATTRIBUTE_START , NUM_INTERVENING_JOBS , JOB_ATTRIBUTE_END , soapPtr , attributeStartLen, attributeEndLen, &index);
		}
		copyAndMoveIndex(&index, REQ_JOB_ATTRIBUTES_END, soapPtr);

	}
	copyAndMoveIndex(&index, ENVELOPE_BODY_END_GET_JOB_ATTR, soapPtr);
}
/*-------------------------------------------------------------------------------
 * getsoapCreateJobReqContentLength()
 *
 *		returns the length of the request file that will be created.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] - the mask of the attributes thet should be requested.
 *
 *		attributesData [in] - the attributes of the job to create.
 *
 *
 * Returns:
 *		
 *		U16 - length of the file.
 *		
 */
U16 getsoapCreateJobReqContentLength(const BtlBppsndCreateJobAttributesMask attributesMask ,
												const BtlBppsndCreateJobRequestData	*attributesData)
{

	U16	length = 0;
	
	U16 attrLength = (U16)(OS_StrLen(ENVELOPE_START)+ OS_StrLen(BODY_START_CREATE_JOB) + OS_StrLen(ENVELOPE_BODY_END_CREATE_JOB));

	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_JOB_NAME)
	{
		length = (U16) OS_StrLen((char*)attributesData->jobName);  
		if (length > 0)
		{
			attrLength = (U16) (attrLength + length + OS_StrLen(JOB_NAME_START)+ OS_StrLen(JOB_NAME_END));	
		}
	}
	
	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME)
	{
		length = (U16) OS_StrLen((char*)attributesData->jobOriginatingUserName);
		if (length > 0)
		{
			attrLength = (U16)(attrLength + length + OS_StrLen(JOB_ORIGINATING_USER_NAME_START) + OS_StrLen(JOB_ORIGINATING_USER_NAME_END));	
		}
	}
	
	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_DOCUMENT_FORMAT)
	{
		length = (U16) OS_StrLen((char*)attributesData->documentFormat);
		if (length > 0)
		{
			attrLength = (U16)(attrLength + length + OS_StrLen(DOCUMENT_FORMAT_START) + OS_StrLen(DOCUMENT_FORMAT_END));	
		}
	}
	
	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_MEDIA_SIZE)
	{
		length = (U16) OS_StrLen((char*)attributesData->mediaSize);
		if (length > 0)
		{
			attrLength = (U16)(attrLength + length + OS_StrLen(MEDIA_SIZE_START) + OS_StrLen(MEDIA_SIZE_END));	
		}
	}
	
	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_MEDIA_TYPE)
	{
		length = (U16) OS_StrLen((char*)attributesData->mediaType);
		if (length > 0)
		{
			attrLength = (U16)(attrLength + length + OS_StrLen(MEDIA_TYPE_START) + OS_StrLen(MEDIA_TYPE_END));	
		}
	}
	
	if(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_CANCEL_ON_LOST_LINK)
	{	
		attrLength = (U16)(attrLength + OS_StrLen(CANCEL_ON_LOST_LINK_START)+	OS_StrLen(CANCEL_ON_LOST_LINK_END));
		if (attributesData->cancelOnLostLink)
		{
			
			attrLength = (U16)(attrLength + OS_StrLen(CANCEL_ON_LOST_LINK_TRUE));
		}
		else
		{
			attrLength = (U16)(attrLength + OS_StrLen(CANCEL_ON_LOST_LINK_FALSE));
		}
	}
	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_NUM_OF_COPIES)
	{
		attrLength = (U16)(attrLength + OS_StrLen(COPIES_START) + OS_StrLen(COPIES_END));

		attrLength = (U16)(attrLength + getNumberLength(attributesData->numOfCopies));

	}

	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_NUMBER_UP)
	{
		attrLength = (U16)(attrLength + OS_StrLen(NUMBER_UP_START) + OS_StrLen(NUMBER_UP_END));
		
		attrLength = (U16)(attrLength + getNumberLength(attributesData->numberUp));
	}

	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_ORIENTATION_REQUESTED)
	{
		switch(attributesData->orientationRequested)									/* can get 1-4 */
		{
				case BTL_BPPSND_PRINT_ORIENTATION_PORTRAIT :
					attrLength = (U16)(attrLength + OS_StrLen(ORIENTATION_REQUESTED_PORTRAIT) + OS_StrLen(ORIENTATION_REQUESTED_START) + OS_StrLen(ORIENTATION_REQUESTED_END));	
					break;
				case BTL_BPPSND_PRINT_ORIENTATION_LANDSCAPE :
					attrLength = (U16)(attrLength + OS_StrLen(ORIENTATION_REQUESTED_LANDSCAPE) + OS_StrLen(ORIENTATION_REQUESTED_START) + OS_StrLen(ORIENTATION_REQUESTED_END));
					break;
				case BTL_BPPSND_PRINT_ORIENTATION_REVERSE_LANDSCAPE :
					attrLength = (U16)(attrLength + OS_StrLen(ORIENTATION_REQUESTED_REVERSE_LANDSCAPE) + OS_StrLen(ORIENTATION_REQUESTED_START) + OS_StrLen(ORIENTATION_REQUESTED_END));	
					break;
				case BTL_BPPSND_PRINT_ORIENTATION_REVERSE_PORTRAIT :
					attrLength = (U16)(attrLength + OS_StrLen(ORIENTATION_REQUESTED_REVERSE_PORTRAIT) + OS_StrLen(ORIENTATION_REQUESTED_START) + OS_StrLen(ORIENTATION_REQUESTED_END));	
					break;
				default :
					break;
	
		}
		
	}

	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_PRINT_QUALITY)
	{
		switch(attributesData->printQuality)									/* can get 1-3 */
		{
			case BTL_BPPSND_PRINT_QUALITY_DRAFT :
				attrLength = (U16) (attrLength + OS_StrLen( PRINT_QUALITY_DRAFT)+ OS_StrLen(PRINT_QUALITY_START) + OS_StrLen(PRINT_QUALITY_END));
				break;
			case BTL_BPPSND_PRINT_QUALITY_NORMAL :
				attrLength = (U16) (attrLength + OS_StrLen(PRINT_QUALITY_NORMAL)+ OS_StrLen(PRINT_QUALITY_START) + OS_StrLen(PRINT_QUALITY_END));
				break;
			case BTL_BPPSND_PRINT_QUALITY_HIGH :
				attrLength = (U16) (attrLength + OS_StrLen(PRINT_QUALITY_HIGH)+ OS_StrLen(PRINT_QUALITY_START) + OS_StrLen(PRINT_QUALITY_END));
				break;
			default :
				break;
		}

	
	}
	
	if ((attributesMask) & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_SIDES)
	{
		switch(attributesData->sides)									/* can get 1-3 */
			{
				case BTL_BPPSND_PRINT_SIDES_ONE_SIDED :
					attrLength = (U16) (attrLength + OS_StrLen(SIDES_ONE_SIDED)+OS_StrLen(SIDES_START) + OS_StrLen(SIDES_END));
					break;
				case BTL_BPPSND_PRINT_SIDES_TWO_SIDED_LONG_EDGE :
					attrLength = (U16) (attrLength + OS_StrLen(SIDES_TWO_SIDED_LONG_EDGE)+OS_StrLen(SIDES_START) + OS_StrLen(SIDES_END));
					break;
				case BTL_BPPSND_PRINT_SIDES_TWO_SIDED_SHORT_EDGE :
					attrLength = (U16) (attrLength + OS_StrLen(SIDES_TWO_SIDED_SHORT_EDGE)+OS_StrLen(SIDES_START) + OS_StrLen(SIDES_END));
					break;
				default :
					break;
	
			}
	}

	return attrLength;
}

/*-------------------------------------------------------------------------------
 * buildsoapCreateJobRequest()
 *
 *		builed the request soap for create job
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] - describes the job attributes
 *
 *		soapPtr [out] - the buffer that will hold the created soap
 *
 *		attributesData [in] - hold the attributes of the job
 *
 *		totalLength [out] - the length of the soap that was created
 *
 * Returns:
 *		
 */
void buildsoapCreateJobRequest(const BtlBppsndCreateJobAttributesMask attributesMask ,
									U8 *soapPtr ,const BtlBppsndCreateJobRequestData	*attributesData ,
									U16 *totalLength)
{
	U32	index = 0, length = 0;
	BOOL allAttr = FALSE; /* will be true if all attributes whanted*/

	U16 contentLength = getsoapCreateJobReqContentLength(attributesMask,attributesData);

	builedHead(contentLength, SOAP_ACTION_CREATE_JOB , soapPtr , totalLength , &index);

	copyAndMoveIndex( &index,BODY_START_CREATE_JOB,soapPtr);

	if((attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_ALL)== BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_ALL)
	{
		allAttr = TRUE;
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_JOB_NAME))
	{
		length = (U16) OS_StrLen( (char*)attributesData->jobName);  
		if (length > 0)
		{	
			insertStringStartBodyEnd(JOB_NAME_START,(char *) attributesData->jobName,JOB_NAME_END,
									soapPtr,&index);
		}
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME))
	{
		length = (U16) OS_StrLen( (char*)attributesData->jobOriginatingUserName);
		if (length > 0)
		{
			insertStringStartBodyEnd(JOB_ORIGINATING_USER_NAME_START,(char *)attributesData->jobOriginatingUserName,JOB_ORIGINATING_USER_NAME_END,
									soapPtr,&index);
		}
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_DOCUMENT_FORMAT))
	{
		length = (U16) OS_StrLen( (char*)attributesData->documentFormat);
		if (length > 0)
		{
			insertStringStartBodyEnd(DOCUMENT_FORMAT_START,(char *)attributesData->documentFormat,DOCUMENT_FORMAT_END,
									soapPtr,&index);

		}
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_NUM_OF_COPIES))
	{
		insertStringStartNumberEnd(COPIES_START, attributesData->numOfCopies , COPIES_END , soapPtr , &index);
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_SIDES))
	{
		copyAndMoveIndex(&index, SIDES_START, soapPtr);
		switch(attributesData->sides)									/* can get 1-3 */
		{
			case BTL_BPPSND_PRINT_SIDES_ONE_SIDED :
				copyAndMoveIndex(&index, SIDES_ONE_SIDED, soapPtr);					
				break;
			case BTL_BPPSND_PRINT_SIDES_TWO_SIDED_LONG_EDGE :
				copyAndMoveIndex(&index, SIDES_TWO_SIDED_LONG_EDGE, soapPtr);					
				break;
			case BTL_BPPSND_PRINT_SIDES_TWO_SIDED_SHORT_EDGE :
				copyAndMoveIndex(&index, SIDES_TWO_SIDED_SHORT_EDGE, soapPtr);
				break;
			default :
				break;
	
		}
		copyAndMoveIndex(&index, SIDES_END, soapPtr);
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_NUMBER_UP))
	{
		insertStringStartNumberEnd(NUMBER_UP_START, attributesData->numberUp, NUMBER_UP_END , soapPtr , &index);
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_ORIENTATION_REQUESTED))
	{
		copyAndMoveIndex(&index, ORIENTATION_REQUESTED_START, soapPtr);
		switch(attributesData->orientationRequested)									/* can get 1-4 */
		{
			case BTL_BPPSND_PRINT_ORIENTATION_PORTRAIT :
				copyAndMoveIndex(&index, ORIENTATION_REQUESTED_PORTRAIT, soapPtr);
				break;
			case BTL_BPPSND_PRINT_ORIENTATION_LANDSCAPE :
				copyAndMoveIndex(&index, ORIENTATION_REQUESTED_LANDSCAPE, soapPtr);
				break;
			case BTL_BPPSND_PRINT_ORIENTATION_REVERSE_LANDSCAPE :
				copyAndMoveIndex(&index, ORIENTATION_REQUESTED_REVERSE_LANDSCAPE, soapPtr);					
				break;
			case BTL_BPPSND_PRINT_ORIENTATION_REVERSE_PORTRAIT :
				copyAndMoveIndex(&index, ORIENTATION_REQUESTED_REVERSE_PORTRAIT, soapPtr);					
				break;
			default :
				break;
		}
		copyAndMoveIndex(&index, ORIENTATION_REQUESTED_END, soapPtr);
	}

	if (allAttr||(attributesMask  & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_MEDIA_SIZE))
	{
		length = (U16) OS_StrLen( (char*)attributesData->mediaSize);
		if (length > 0)
		{
			insertStringStartBodyEnd(MEDIA_SIZE_START,(char *)attributesData->mediaSize, MEDIA_SIZE_END, soapPtr, &index);
		}
	}
	if (allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_MEDIA_TYPE))
	{
		length = (U16) OS_StrLen((char*)attributesData->mediaType);
		if (length > 0)
		{
			insertStringStartBodyEnd(MEDIA_TYPE_START,(char *)attributesData->mediaType, MEDIA_TYPE_END, soapPtr, &index);		
		}
	}
	if (allAttr||(attributesMask  & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_PRINT_QUALITY))
	{
		copyAndMoveIndex(&index, PRINT_QUALITY_START, soapPtr);
		switch(attributesData->printQuality)									/* can get 1-3 */
		{
			case BTL_BPPSND_PRINT_QUALITY_DRAFT :
				copyAndMoveIndex(&index, PRINT_QUALITY_DRAFT, soapPtr);
				break;
			case BTL_BPPSND_PRINT_QUALITY_NORMAL :
				copyAndMoveIndex(&index, PRINT_QUALITY_NORMAL, soapPtr);
				break;
			case BTL_BPPSND_PRINT_QUALITY_HIGH :
				copyAndMoveIndex(&index, PRINT_QUALITY_HIGH, soapPtr);
				break;
			default :
				break;
		}
		copyAndMoveIndex(&index, PRINT_QUALITY_END, soapPtr);
	}
	if(allAttr||(attributesMask & BTL_BPPSND_CREATE_JOB_ATTRIBUTE_MASK_CANCEL_ON_LOST_LINK))
	{
		copyAndMoveIndex(&index, CANCEL_ON_LOST_LINK_START, soapPtr);
		if (attributesData->cancelOnLostLink)
		{
			copyAndMoveIndex(&index, CANCEL_ON_LOST_LINK_TRUE, soapPtr);
		}
		else
		{
			copyAndMoveIndex(&index, CANCEL_ON_LOST_LINK_FALSE, soapPtr);
		}
		copyAndMoveIndex(&index, CANCEL_ON_LOST_LINK_END, soapPtr);
	}
	copyAndMoveIndex(&index, ENVELOPE_BODY_END_CREATE_JOB, soapPtr);
}

/*-------------------------------------------------------------------------------
 * buildsoapCancelJobRequest()
 *
 *	builed the request soap for cancle job
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		jobId [in] - the id of the job to be canceled
 *
 *		totalLength [out] -  the length of the soap that was created
 *
 *		soapPtr [out] - the buffer that will hold the created soap
 *
 * Returns:
 *		
 *
 */
void buildsoapCancelJobRequest(U8 *soapPtr , BtlBppsndJobId	 jobId, U16 *totalLength)
{
	U32 index = 0 ,idLength=0;
	U16 contentLength;

	idLength = getNumberLength(jobId);
	contentLength = (U16)(OS_StrLen(ENVELOPE_START)+OS_StrLen(BODY_START_CANCEL_JOB)+OS_StrLen(JOB_ID_START)+
					OS_StrLen(JOB_ID_END)+	OS_StrLen(ENVELOPE_BODY_END_CANCEL_JOB)+idLength);

	builedHead(contentLength , SOAP_ACTION_CANCEL_JOB , soapPtr , totalLength ,&index);
	copyAndMoveIndex(&index, BODY_START_CANCEL_JOB, soapPtr);
	insertStringStartNumberEnd(JOB_ID_START, jobId, JOB_ID_END, soapPtr, &index);
	copyAndMoveIndex(&index, ENVELOPE_BODY_END_CANCEL_JOB, soapPtr);
}
/*-------------------------------------------------------------------------------
 * buildsoapGetEventRequest()
 *
 *	builed the request soap for get job
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		jobId [in] - the id of the job 
 *
 *		totalLength [out] -  the length of the soap that was created
 *
 *		soapPtr [out] - the buffer that will hold the created soap
 *
 * Returns:
 *		
 *
 */

void buildsoapGetEventRequest( U8 *soapPtr ,BtlBppsndJobId	 jobId, U16 *totalLength)
{
	U32 index = 0,idLength=0 ;
	U16 contentLength=0;

	idLength = getNumberLength(jobId);

	contentLength = (U16)(OS_StrLen(ENVELOPE_START)+OS_StrLen(BODY_START_GET_EVENT)+
					OS_StrLen(JOB_ID_START)+OS_StrLen(JOB_ID_END)+
							OS_StrLen(ENVELOPE_BODY_END_GET_EVENT)+idLength);
	builedHead(contentLength , SOAP_ACTION_GET_EVENT , soapPtr , totalLength ,&index);
	copyAndMoveIndex(&index, BODY_START_GET_EVENT, soapPtr);
	insertStringStartNumberEnd(JOB_ID_START, jobId, JOB_ID_END, soapPtr, &index);
	copyAndMoveIndex(&index, ENVELOPE_BODY_END_GET_EVENT, soapPtr);
}

/*-------------------------------------------------------------------------------
 * builedHead()
 *
 *	builed the header of the soap file	
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		contentLength [in] -  the length without the head			length[out] - the final length of th buffer 
 *
 *		soapAction [in] - the action requsted		 			index[out] - the new place to write on the buffer
 *
 *		soapPtr [in/out] - here the header will be written
 *
 *		
 * Returns:
 *		
 */
void builedHead(U16 contentLength , char *soapAction , U8 *soapPtr , U16 *length ,U32 *index)
{
	U32 bytesInBody = 3;
	U32	index1 =0;

	/* Calculate and write the content length of the soap file */
	contentLength = (U16)(contentLength + OS_StrLen(CONTENT_LENGTH) + OS_StrLen(HTTP_NEWLINE) + 
						OS_StrLen(CONTENT_TYPE) + OS_StrLen(soapAction) + 
						bytesInBody ); 
	bytesInBody = getNumberLength(contentLength+1);

	contentLength = (U16)(contentLength + bytesInBody - 3);		/*allways in range 3-4*/

	/*	insert the file length for returning it to the calling function 	*/
	*length = contentLength;
	copyAndMoveIndex( &index1,CONTENT_LENGTH,soapPtr);
	intToString(&soapPtr[index1], contentLength, bytesInBody);
	index1 += bytesInBody;

	/* Build  soap headers */
	copyAndMoveIndex( &index1,HTTP_NEWLINE,soapPtr);
	copyAndMoveIndex( &index1,CONTENT_TYPE,soapPtr);
	copyAndMoveIndex( &index1,soapAction,soapPtr);
	copyAndMoveIndex( &index1,ENVELOPE_START,soapPtr);
	*index = index1;
}
/*-------------------------------------------------------------------------------
 * insertStringStartBodyEndL()
 *
 *		insert the string start body end to the buffer with the length given
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		start ,body,end[in] - the  start/body/end of the string to write to the buffer
 *
 *		soapPtr[out] - in this buffer the string will be written 
 *		
 *		startLen/endLen[in] - the length of start/end
 *
 *		index[in/out] - the place in the buffer to write in , after the string inserted update the index
 *
 * Returns:	
 *		
 */
void insertStringStartBodyEndL(char *start , char *body , char *end , U8 *soapPtr , U32 startLen, U32 endLen, U32 *index)
{
	copyAndMoveIndexL(index, start, (char*)soapPtr, startLen);
	copyAndMoveIndex(index, body, soapPtr);
	copyAndMoveIndexL(index, end , (char*)soapPtr, endLen);
}
/*-------------------------------------------------------------------------------
 * insertStringStartBodyEnd()
 *
 *		insert the string start body end to the buffer
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		start ,body,end[in] - the  start/body/end of the string to write to the buffer
 *
 *		soapPtr[out] - in this buffer the string will be written 
 *		
 *		index[in/out] - the place in the buffer to write in , after the string inserted update the index
 * Returns:
 */
void insertStringStartBodyEnd(char *start , char *body , char *end , U8 *soapPtr , U32 *index)
{
	U32 startLen,endLen;
	startLen = OS_StrLen(start);
	endLen = OS_StrLen(end);
	insertStringStartBodyEndL(start , body , end , soapPtr , startLen,  endLen, index);
}

/*-------------------------------------------------------------------------------
 * insertStringStartNumberEnd()
 *
 *		insert the string start body end to the buffer where the body is an integer
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		start ,num,end[in] - the  start/num/end of the string to write to the buffer
 *
 *		soapPtr[out] - in this buffer the string will be written 
 *		
 *		index[in/out] - the place in the buffer to write in , after the string inserted update the index
 * Returns:
 */
void insertStringStartNumberEnd(char *start , U32 num , char *end , U8 *soapPtr , U32 *index)
{
	U32 tempLength;
	tempLength = getNumberLength(num);
	copyAndMoveIndex(index, start, soapPtr);
	intToString(&soapPtr[*index],num, tempLength);
	*index							+= tempLength;
	copyAndMoveIndex(index, end, soapPtr);
}

/*-------------------------------------------------------------------------------
 * copyAndMoveIndex()
 *
 *		copy string and move index	
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		strTo [in/out] - the place to insert the string
 *
 *		index [in] -  the index in strTo , where it should be written
 *
 *		strFrom [in] - the string to copy
 *			
 * Returns:
 */
 void copyAndMoveIndex(U32 *index, char *strFrom, U8 *strTo)
{
	U32 tempLength = OS_StrLen(strFrom);
	OS_MemCopy(&strTo[*index], strFrom, tempLength);
	*index += tempLength; 
}
/*-------------------------------------------------------------------------------
 * copyAndMoveIndexL()
 *
 *		copy string of known length and move index						
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		strTo [in/out] - the place to insert the string
 *
 *		index [in] -  the index in strTo , where it should be written
 *
 *		strFrom [in] - the string to copy
 *
 *		length [in] - the length of strFrom			
 * Returns:
 */
void copyAndMoveIndexL(U32 *index, char *strFrom, char *strTo, U32 length)
{
	OS_MemCopy(&strTo[*index], strFrom, length);
	*index += length; 
}
/*-------------------------------------------------------------------------------
 * getNumberLength()
 *
 *		the length of number in decimal representation	
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		num [in] -  the number
 *
 * Returns:
 *
 *		i - the length of the number		
 */
U32 getNumberLength(U32 num)
{
	U32 i=0;
	while(num>0)
	{
		i++;
		num = num/10;
	}
	return i;
}

/*-------------------------------------------------------------------------------
 * intToString()
 *
 *		convert an integer of length numLength to string in decimal representation	
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		stNum [in/out] - the string of the num
 *
 *		num [in] - the number
 *
 *		numLength [in] - length of the number
 *
 * Returns:
 */
void intToString(U8 *stNum, U32 num, U32 numLength)
{
	U32 i = numLength;

	while(i>0)
	{
		stNum[i-1] = (U8)((num%10)+'0');
		num = (num/10);
		i--;
	}
}






#endif /*BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED*/

