/*******************************************************************************\
*                                                                                                                                        	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		       *
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      Btl_bppsnd_soapInterface.c
*
*   DESCRIPTION:  this file holds the functions that are actually insert parsed data to a Btl_Bppsnd object 
*				depending on which function was called. this file uses the general parsing functin  
*				which is in btl_bppsnd_soapParser.c (see the author in the file description ).	
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
#include "bthal_fs.h" 
#include "btl_defs.h"
#include "Btl_bppsnd_soap.h"
#include "Btl_bppsnd_soapinterface.h"


BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BPPSND);

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_MAX_SOAP_FILE_SIZE constant
 *
 *     Represents the maximal size of soap file that a printer could send .
 */
#define BTL_BPPSND_MAX_SOAP_FILE_SIZE 	(5000)
/*-------------------------------------------------------------------------------
 * Buffer constant
 *
 *     Represents the maximal size of soap file that a printer could send .
 */
U8 	buffer[BTL_BPPSND_MAX_SOAP_FILE_SIZE+1]={0};

/*-------------------------------------------------------------------------------
 *
 *     Represents status of internal function.
 */
#define INTERNAL_FUNC_SUCCESS                                                 	1
#define INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN		2
#define INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM		3
#define INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE		4
#define INTERNAL_FUNC_FAILED_PARSING                                	5
#define INTERNAL_FUNC_FAILED_OPENING_FILE				6
#define INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE		7

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE constant
 *
 *     Represents defualt star value of fields in struct to see if they where  changed.
 */
#define BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE	255
#define BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE		(0xFFFFFFFFUL)

 /********************************************************************************
 *
 * Types
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Dat a Structures
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


static U32 fromFileToBuffer(const BtlUtf8* fsPath , BtlUtf8 *buffer);

static int fromBufferToObjLoc(const BtlObject 	*requestObj,const BtlUtf8* buffer, U16 contentLength);

static int fromBufferToUIAppInstanceData(UIAppInstanceData * instanceData, const BtlObject *dataToParse);

void cleanBuffer(U8 *buffToClean);


/*******************************************************************************
*
*	Function definitions
*
*******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_BuildReqGetPrinterAttributes()
 *
 *		Initiates a BPP GetPrinterAttributes operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		attributesMask [in] - Mask of printer attributes to request from the printer. 
 *						   The caller should set the bits in the mask that corrsepond to features
 *							in which it is interested. Multiple features may be Ored together.
 *					
 *							The pointer is free after the function returns
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 */
BtStatus Btl_BppSnd_BuildGetPrinterAttributes(
			BtlBppsndPrinterAttributesMask	attributesMask,											
			const BtlObject 				*requestObj,
			U16 							*size)
{
	U16 contentLength = 0;
	BtStatus status = BT_STATUS_SUCCESS;
	BTL_FUNC_START("Btl_BppSnd_BuildGetPrinterAttributes");
	buildsoapPrinterAttrRequest(attributesMask,buffer,&contentLength);

	switch(fromBufferToObjLoc(requestObj, buffer, contentLength))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_BuildGetPrinterAttributes-SUCCESS WRITING TO REQUESTOBJ"));
			break;
		case INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetPrinterAttributes-INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN"));
			break;
		case INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetPrinterAttributes-INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetPrinterAttributes-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetPrinterAttributes-INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE"));
			break;
		default:
			break;
	}

	*size = contentLength;
	
	BTL_FUNC_END();
	return status;
}


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_BuildReqCreateJob()
 *
 *		Initiates a BPP CreateJob operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
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
 *							Multiple features may be Ored together. 
 *							If attributesData is 0, then this argument should be 0 as well
 *					
 *							The pointer is free after the function returns
 *
 *		responseObj [in] - describes where to place the SOAP XML response contents
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful. 
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
 
BtStatus Btl_BppSnd_BuildCreateJob(
									const BtlBppsndCreateJobRequestData		*attributesData,
									const BtlBppsndCreateJobAttributesMask	attributesMask,
									const BtlObject 						*requestObj,
									U16 									*size)
{
	U16 contentLength = 0;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_BppSnd_BuildCreateJob");

	buildsoapCreateJobRequest(attributesMask , buffer, attributesData ,&contentLength);

	switch(fromBufferToObjLoc(requestObj, buffer, contentLength))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_BuildGetPrinterAttributes-SUCCESS WRITING TO REQUESTOBJ"));
			break;
		case INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCreateJob-INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN"));
			break;
		case INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCreateJob-INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCreateJob-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCreateJob-INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE"));
			break;
		default:
			break;
	}

	*size = contentLength;

	BTL_FUNC_END();
	return status;

}



/*-------------------------------------------------------------------------------
 * BTL_BPPSND_BuildReqGetJobAttributes()
 *
 *		Initiates a BPP GetJobAttributes operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		jobId [in] - Job ID obtained via the response event to the corresponding BTL_BPPSND_CreateJob request
 *
 *		attributesMask [in] - Mask of printer attributes to request from the printer. 
 *						   The caller should set the bits in the mask that corrsepond to features
 *							in which it is interested. Multiple features may be Ored together.
 *					
 *							The pointer is free after the function returns
 *
 *		requestObj [in] - describes where to place the SOAP XML response contents
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus Btl_BppSnd_BuildGetJobAttributes(	
											BtlBppsndJobId					jobId,
											const BtlBppsndGetJobAttributesMask	attributesMask,
											const BtlObject 					*requestObj,
											U16								*size)
{
	U16 contentLength = 0;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_BppSnd_BuildGetJobAttributes");
	buildsoapJobAttrRequest(attributesMask, buffer,&contentLength, jobId);

	switch(fromBufferToObjLoc(requestObj, buffer, contentLength))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_BuildGetPrinterAttributes-SUCCESS WRITING TO REQUESTOBJ"));
			break;
		case INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetJobAttributes-INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN"));
			break;
		case INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetJobAttributes-INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetJobAttributes-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildGetJobAttributes-INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE"));
			break;
		default:
			break;
	}
	*size = contentLength;

	BTL_FUNC_END();
	return status;

}



/*-------------------------------------------------------------------------------
 * BTL_BPPSND_BuildReqCancelJob()
 *
 *		Cancels a particular print job
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		jobId [in] - Job ID obtained via the response to the corresponding BTL_BPPSND_CreateJob request
 *
 *		requestObj [in/out] - describes where to place the SOAP XML response contents
 *
 * Returns:
 *
 *		BT_STATUS_SUCCESS - Operation is successful.
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus Btl_BppSnd_BuildCancelJob(	
										BtlBppsndJobId	jobId,
										const BtlObject 	*requestObj,
										U16				*size)
{
	U16 contentLength = 0;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_BppSnd_BuildCancelJob");
	buildsoapCancelJobRequest(buffer, jobId,&contentLength);

	switch(fromBufferToObjLoc(requestObj, buffer, contentLength))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_BuildGetPrinterAttributes-SUCCESS WRITING TO REQUESTOBJ"));
			break;
		case INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCancelJob-INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN"));
			break;
		case INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCancelJob-INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCancelJob-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildCancelJob-INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE"));
			break;
		default:
			break;
	}
	*size = contentLength;
	
	BTL_FUNC_END();
	return status;

}

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_BuildReqStartGetEvent()
 *
 *		Starts a GetEvent operation on the status channel.
 *
 *		Only a single operation may execute on the status channel simultaneously. The GetEvent
 *		allows monitoring of the state of a specific job (identified by its jobId).
 *
 *		The application callback will be signalled with the new printer & job status whenever there is
 *		a change in their state.
 *
 *		The application should call BTL_BPPSND_StopGetEvent to stop monitoring the state. The monitoring will also
 *		stop upon a call to any of the following functions: BTL_BPPSND_GetJobAttributes, BTL_BPPSND_CancelJob, and 
 *		BTL_BPPSND_GetPrinterAttributes. Whenever monitoring stops, a corresponding event shall be sent to the
 *		application callback.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		jobId [in] - Job ID obtained via the response to the corresponding BTL_BPPSND_CreateJob request
 *
 *		requestObj [in] - describes where to place the SOAP XML response contents
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED -  Unable to start the operation.
 *
 *		BT_STATUS_INVALID_PARM - Invalid parameters.
 */
BtStatus Btl_BppSnd_BuildStartGetEvent(
										BtlBppsndJobId	jobId,
										const BtlObject	*requestObj,
										U16				*size)
										
{
	U16 contentLength = 0;
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("Btl_BppSnd_BuildStartGetEvent");
	buildsoapGetEventRequest(buffer, jobId,&contentLength);

	switch(fromBufferToObjLoc(requestObj, buffer, contentLength))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_BuildGetPrinterAttributes-SUCCESS WRITING TO REQUESTOBJ"));
			break;
		case INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildStartGetEvent-INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN"));
			break;
		case INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildStartGetEvent-INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildStartGetEvent-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_BuildStartGetEvent-INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE"));
			break;
		default:
			break;
	}

	*size = contentLength;

	BTL_FUNC_END();
	return status;
}



/*-------------------------------------------------------------------------------
 * Btl_BppSnd_ParseGetPrinterAttributesResponse()
 *
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
 *
 *		dataToParse [in] - describes XML response information 
 *
 *		printerAttributesMask [in] - Indicates which attributes of the printer is needed
 *
 *		printerAttributes [out] - Contains the values of the present attributes.
 *
 *		printerAttributesMaskFound [out] - Indicates which attributes in printerAttributes are valid ,the value is zero when sent!
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */


BtStatus Btl_BppSnd_ParseGetPrinterAttributesResponse(
							const BtlObject *dataToParse,
							BtlBppsndPrinterAttributes	*printerAttributes,
							const BtlBppsndPrinterAttributesMask	printerAttributesMask,
							BtlBppsndPrinterAttributesMask	*printerAttributesMaskFound )
{

	BtStatus status = BT_STATUS_SUCCESS;
	BOOL  allAttr = FALSE;
	int i=0,length=0;
	UIAppInstanceData instanceData={0};  /* set at start to 0 */
		
	BTL_FUNC_START("Btl_BppSnd_ParseGetPrinterAttributesResponse");
	/* the value of state and stateReason set to dufualt because 0 is legit value for them*/
	instanceData.btAppBppInstanceData.printerState = BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE; 
	instanceData.btAppBppInstanceData.printerStateReason = BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE; 
	instanceData.btAppBppInstanceData.colorSupported = BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE;
	instanceData.btAppBppInstanceData.queuedJobCount = BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE;
	instanceData.btAppBppInstanceData.basicTextPageWidth= BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE;
	instanceData.btAppBppInstanceData.basicTextPageHeight= BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE;

	switch(fromBufferToUIAppInstanceData(&instanceData,dataToParse))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_ParseGetPrinterAttributesResponse-SUCCESS READING FROM BUFFER"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetPrinterAttributesResponse-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetPrinterAttributesResponse-INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_PARSING:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetPrinterAttributesResponse-INTERNAL_FUNC_FAILED_PARSING"));
			break;
		default:
			break;
	}
	printerAttributes->operationStatus = (BtlBppsndJobOperationStatus)instanceData.btAppBppInstanceData.operationStatus;
	
	if((printerAttributesMask &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL) == BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ALL)
	{
		allAttr = TRUE;
	}
	if((allAttr)||((printerAttributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_NAME))
	{
		if(*instanceData.btAppBppInstanceData.printerName != '\0')
		{
			OS_StrnCpy(	(char*)printerAttributes->printerName,
						(char*)instanceData.btAppBppInstanceData.printerName,
						BTL_BPPSND_MAX_PRINTER_NAME_LEN);
			
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_NAME;
		}
		
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_LOCATION))
	{
		if(*instanceData.btAppBppInstanceData.printerLocation!= '\0')
		{
			OS_StrnCpy((char*)printerAttributes->printerLocation,
						(char*)instanceData.btAppBppInstanceData.printerLocation,
						BTL_BPPSND_MAX_PRINTER_LOCATION_LEN);
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_LOCATION;
		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE))
	{
		if(!(instanceData.btAppBppInstanceData.printerState == BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE))
		{
			printerAttributes->printerState = (BtlBppsndPrinterState)(instanceData.btAppBppInstanceData.printerState + 1);
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE;
			
 		}
		
	}
	if((allAttr)||((printerAttributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE_REASONS))
	{
	
		if(!(instanceData.btAppBppInstanceData.printerStateReason == BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE))
		{
			printerAttributes->printerStateReasons = (BtlBppsndPrinterStateReasons)(instanceData.btAppBppInstanceData.printerStateReason + 1);	
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_STATE_REASONS;
		}
	}
	if((allAttr)||((printerAttributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_DOCUMENT_FORMATS_SUPPORTED))
	{
		/*while((j<BTL_BPPSND_NUM_OF_DOCUMENT_FORMATS)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_DOCUMENT_FORMATS))
		{
			if(instanceData.btAppBppInstanceData.documentFormat& mask)
			{
				OS_StrnCpy(printerAttributes->documentFormatsSupported[i], documentFormatArr[j], BTL_BPPSND_MAX_DOCUMENT_FORMAT_LEN);
				i++;
			}	
			mask = mask*2;
			j++;
		}
		j=0;
		mask = 1;
		if(i>0)
		{
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_DOCUMENT_FORMATS_SUPPORTED;
		}
		printerAttributes->numDocumentFormatsSupported = i;
		i=0;*/
		i=0;
		length = instanceData.btAppBppInstanceData.numDocumentFormatsSupported;
		while(i<length)
		{
			OS_StrnCpy((char*)printerAttributes->documentFormatsSupported[i],
						(char*)instanceData.btAppBppInstanceData.documentFormatsSupported[i],
						BTL_BPPSND_MAX_DOCUMENT_FORMAT_LEN);
			BTL_LOG_DEBUG(("DocumentFormat-Supported: %s ",printerAttributes->documentFormatsSupported[i]));
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_DOCUMENT_FORMATS_SUPPORTED;
		}
		printerAttributes->numDocumentFormatsSupported = (U32)i;
		i = 0;
		length = 0;
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_COLOR_SUPPORTED))
	{
		if(instanceData.btAppBppInstanceData.colorSupported == 1)
		{
			printerAttributes->isColorSupported = TRUE;
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_COLOR_SUPPORTED;
		}
		else if(instanceData.btAppBppInstanceData.colorSupported == 0)
		{
			printerAttributes->isColorSupported = FALSE;
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_COLOR_SUPPORTED;

		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MAX_COPIES_SUPPORTED))
	{

		if(!(instanceData.btAppBppInstanceData.maxCopies == '\0'))
		{

			printerAttributes->maxCopiesSupported= (U32)OS_AtoU32((char*)&(instanceData.btAppBppInstanceData.maxCopies));
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MAX_COPIES_SUPPORTED;

		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_SIDES_SUPPORTED))
	{

		if((instanceData.btAppBppInstanceData.sides & ONE_SIDED_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_SIDES))
		{
			printerAttributes->sidesSupported[i] = BTL_BPPSND_PRINT_SIDES_ONE_SIDED;
			i++;
		}
		if((instanceData.btAppBppInstanceData.sides & TWO_SIDED_LONG_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_SIDES))
		{
			printerAttributes->sidesSupported[i] = BTL_BPPSND_PRINT_SIDES_TWO_SIDED_LONG_EDGE;
			i++;
		}
		if((instanceData.btAppBppInstanceData.sides & TWO_SIDED_SHORT_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_SIDES))
		{
			printerAttributes->sidesSupported[i] = BTL_BPPSND_PRINT_SIDES_TWO_SIDED_SHORT_EDGE;
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_SIDES_SUPPORTED;
		}
		printerAttributes->numSidesSupported = (U32)i;
		i=0;		

	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_NUMBER_UP_SUPPORTED))
	{
		if(!(instanceData.btAppBppInstanceData.numberUp == '\0'))
		{
			printerAttributes->NumberUpSupported = (U32)OS_AtoU32((char*)&(instanceData.btAppBppInstanceData.numberUp));
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_NUMBER_UP_SUPPORTED;
		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ORIENTATIONS_SUPPORTED))
	{

		if((instanceData.btAppBppInstanceData.orientation & PORTRAIT_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_ORIENTATIONS))
		{
			printerAttributes->orientationsSupported[i] = BTL_BPPSND_PRINT_ORIENTATION_PORTRAIT;
			i++;
		}
		if((instanceData.btAppBppInstanceData.orientation & LANDSCAPE_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_ORIENTATIONS))
		{
			printerAttributes->orientationsSupported[i] = BTL_BPPSND_PRINT_ORIENTATION_LANDSCAPE;
			i++;
		}
		if((instanceData.btAppBppInstanceData.orientation & REVERSE_LANDSCAPE_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_ORIENTATIONS))
		{
			printerAttributes->orientationsSupported[i] = BTL_BPPSND_PRINT_ORIENTATION_REVERSE_LANDSCAPE;
			i++;
		}
		if((instanceData.btAppBppInstanceData.orientation & REVERSE_PORTRAIT_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_ORIENTATIONS))
		{
			printerAttributes->orientationsSupported[i] = BTL_BPPSND_PRINT_ORIENTATION_REVERSE_PORTRAIT;
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_ORIENTATIONS_SUPPORTED;
		}
		printerAttributes->NumberorientationsSupported = (U32)i;
		i =0;		
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_SIZES_SUPPORTED))
	{

		i=0;
		/*while((j<BTL_BPPSND_NUM_OF_MEDIA_SIZES)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_SIZES))
		{
			if(instanceData.btAppBppInstanceData.supportedMediaTypes & mask)
			{
				OS_StrnCpy(printerAttributes->mediaSizesSupported[i], mediaSizeArr[j], BTL_BPPSND_MAX_MEDIA_SIZE_LEN);
				i++;
			}	
			mask = mask*2;
			j++;
		}
		j=0;
		mask = 1;

		if(i>0)
		{
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_SIZES_SUPPORTED;
		}
		printerAttributes->numMediaSizesSupported = (U32)i;
		i=0;*/
		length = instanceData.btAppBppInstanceData.numMediaSizesSupported;
		while(i<length)
		{
			OS_StrnCpy((char*)printerAttributes->mediaSizesSupported[i],
						(char*)instanceData.btAppBppInstanceData.mediaSizesSupported[i],
						BTL_BPPSND_MAX_MEDIA_SIZE_LEN);
			BTL_LOG_DEBUG(("MediaSize-Supported: %s ",printerAttributes->mediaSizesSupported[i]));
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_SIZES_SUPPORTED;
		}
		printerAttributes->numMediaSizesSupported = (U32)i;
		i = 0;
		length = 0;
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_TYPES_SUPPORTED)){

		/*while((j<BTL_BPPSND_NUM_OF_MEDIA_TYPES)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_TYPES)) 
		{
			if(instanceData.btAppBppInstanceData.supportedMediaTypes & mask)
			{
				OS_StrnCpy(printerAttributes->mediaTypesSupported[i], mediaTypeArr[j], BTL_BPPSND_MAX_MEDIA_TYPE_LEN);
				i++;
			}	
			mask = mask*2;
			j++;
		}
		j=0;
		mask = 1;
		if(i>0)
		{
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_TYPES_SUPPORTED;
		}
		printerAttributes->numMediaTypesSupported = (U32)i;
		i=0;*/
		i=0;
		length = instanceData.btAppBppInstanceData.numMediaTypesSupported;
		while(i<length)
		{
			OS_StrnCpy((char*)printerAttributes->mediaTypesSupported[i],
						(char*)instanceData.btAppBppInstanceData.mediaTypesSupported[i],
						BTL_BPPSND_MAX_MEDIA_TYPE_LEN);
			BTL_LOG_DEBUG(("MediaType-Supported: %s ",printerAttributes->mediaTypesSupported[i]));
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_TYPES_SUPPORTED;
		}
		printerAttributes->numMediaTypesSupported = (U32)i;
		i = 0;
		length = 0;
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_LOADED))
	{
		length = instanceData.btAppBppInstanceData.numMediaLoaded;
		while(i<length)
		{
			OS_StrnCpy((char*)printerAttributes->mediaLoadedDetails[i].LoadedMediumSize,
						(char*)instanceData.btAppBppInstanceData.mediaLoadedDetails[i].LoadedMediumSize,
						BTL_BPPSND_MAX_MEDIA_SIZE_LEN);
			OS_StrnCpy((char*)printerAttributes->mediaLoadedDetails[i].LoadedMediumType,
						(char*)instanceData.btAppBppInstanceData.mediaLoadedDetails[i].LoadedMediumType,
						BTL_BPPSND_MAX_MEDIA_TYPE_LEN);
			BTL_LOG_DEBUG(("MediaDetailes---:\nMedia-Size: %s \nMedia-Type: %s",printerAttributes->mediaLoadedDetails[i].LoadedMediumSize
				,printerAttributes->mediaLoadedDetails[i].LoadedMediumType));
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_MEDIA_LOADED;
		}
		printerAttributes->numMediaLoaded= (U32)i;
		i = 0;
		length = 0;
	}
	if((allAttr)||((printerAttributesMask) &  BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINT_QUALITY_SUPPORTED))
	{

		if((instanceData.btAppBppInstanceData.printfQuality & DRAFT_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_PRINT_QUALITY))
		{
			printerAttributes->PrintQualitySupported[i] = BTL_BPPSND_PRINT_QUALITY_DRAFT;
			i++;
		}
		if((instanceData.btAppBppInstanceData.printfQuality & NORMAL_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_PRINT_QUALITY))
		{
			printerAttributes->PrintQualitySupported[i] = BTL_BPPSND_PRINT_QUALITY_NORMAL;
			i++;
		}
		if((instanceData.btAppBppInstanceData.printfQuality & HIGH_MASK)&&(i<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_PRINT_QUALITY))
		{
			printerAttributes->PrintQualitySupported[i] = BTL_BPPSND_PRINT_QUALITY_HIGH;
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINT_QUALITY_SUPPORTED;
		}
		printerAttributes->numPrintQualitySupported= (U32)i;
		i=0;		
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_IMAGE_FORMATS_SUPPORTED))
	{
		length = instanceData.btAppBppInstanceData.numImageFormatsSupported;
		while(i<length)
		{
			OS_StrnCpy((char*)printerAttributes->imageFormatsSupported[i],
						(char*)instanceData.btAppBppInstanceData.imageFormatsSupported[i],
						BTL_BPPSND_MAX_IMAGE_FORMAT_LEN);
			BTL_LOG_DEBUG(("Image-Format: %s ",printerAttributes->imageFormatsSupported[i]));
			i++;
		}
		if(i>0)
		{
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_IMAGE_FORMATS_SUPPORTED;
		}
		printerAttributes->numImageFormatsSupported= (U32)i;
		i = 0;
		length = 0;
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_WIDTH))
	{
		if(!(instanceData.btAppBppInstanceData.basicTextPageWidth== BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE))
		{
			printerAttributes->basicTextPageWidth= instanceData.btAppBppInstanceData.basicTextPageWidth;
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_WIDTH;
		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_HEIGHT))
	{
		if(!(instanceData.btAppBppInstanceData.basicTextPageHeight== BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE))
		{
			printerAttributes->basicTextPageHeight= instanceData.btAppBppInstanceData.basicTextPageHeight;
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_BASIC_TEXT_PAGE_HEIGHT;
		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_GENERAL_CURRENT_OPERATOR))
	{
		if(!(instanceData.btAppBppInstanceData.printerGeneralCurrentOperator == '\0'))
		{
			OS_StrnCpy((char*)printerAttributes->printerGeneralCurrentOperator,
						(char*)instanceData.btAppBppInstanceData.printerGeneralCurrentOperator,
						BTL_BPPSND_MAX_PRINTER_GENERAL_CURRENT_OPERATOR);
			(*printerAttributesMaskFound) += BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_PRINTER_GENERAL_CURRENT_OPERATOR;

		}
	}
	if((allAttr)||((printerAttributesMask) & BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_QUEUED_JOB_COUNT))
	{
		if(!(instanceData.btAppBppInstanceData.queuedJobCount == BTL_BPPSND_PRINTER_ATTR_DEFUALT_START_VALUE))
		{
			printerAttributes->queuedJobCount = instanceData.btAppBppInstanceData.queuedJobCount;
			(*printerAttributesMaskFound) +=BTL_BPPSND_PRINTER_ATTRIBUTE_MASK_QUEUED_JOB_COUNT;
		}
	}
	BTL_FUNC_END();

	return status;
}
/*-------------------------------------------------------------------------------
 * Btl_BppSnd_ParseCreateJobResponse()
 *
 *		Parse a SOAP XML CreateJob response 
 *
 *		This function parses the response, and sets the available attribute values in 
 *		attributesData.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *
 *		dataToParse [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the response attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
BtStatus Btl_BppSnd_ParseCreateJobResponse(
			const BtlObject 					*dataToParse, 
			BtlBppsndCreateJobResponseData	*attributesData)
{

	BtStatus status = BT_STATUS_SUCCESS;

	UIAppInstanceData instanceData={0};  /* set at start to 0 */
	BTL_FUNC_START("Btl_BppSnd_ParseCreateJobResponse");

	switch(fromBufferToUIAppInstanceData(&instanceData,dataToParse))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_ParseCreateJobResponse-SUCCESS READING FROM BUFFER"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, ("Btl_BppSnd_ParseCreateJobResponse-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, ("Btl_BppSnd_ParseCreateJobResponse-INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_PARSING:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, ("Btl_BppSnd_ParseCreateJobResponse-INTERNAL_FUNC_FAILED_PARSING"));
			break;
		default:
			break;
	}	
	attributesData->jobId = (U32)instanceData.btAppBppInstanceData.jobId;
	attributesData->operationStatus = (BtlBppsndJobOperationStatus)instanceData.btAppBppInstanceData.operationStatus;
	BTL_FUNC_END();
	return status;	
}


/*-------------------------------------------------------------------------------
 * Btl_BppSnd_ParseGetJobAttributesResponse()
 *
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
 *		dataToParse [in] - describes XML response information. 
  *		attributesData [out] - Contains the values of the present attributes.
 *		attributesMask [in] - Indicates which attributes in attributesData are valid.
 *		jobAttributesMaskFound[out] - Indicates the attributes that was actually found.
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */

BtStatus Btl_BppSnd_ParseGetJobAttributesResponse(
			const BtlObject 						*dataToParse, 
			BtlBppsndGetJobAttributesResponseData	*attributesData,
			BtlBppsndGetJobAttributesMask			attributesMask,
			BtlBppsndGetJobAttributesMask	*jobAttributesMaskFound)
{

	BtStatus status = BT_STATUS_SUCCESS;
	BOOL allAttr = FALSE, stateFound = FALSE;
	UIAppInstanceData instanceData={0};  /*  set at start to 0 */
	BTL_FUNC_START("Btl_BppSnd_ParseGetJobAttributesResponse");

	/* these values are set to dufualt because 0 is legit value for them*/
	instanceData.btAppBppInstanceData.jobMediaSheetsCompleted = BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE; /*  to differ zero from no answer */
	instanceData.btAppBppInstanceData.numOfInterveningJobs = BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE;/*  to differ zero from no answer */
	instanceData.btAppBppInstanceData.jobId =BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE;/*  to differ zero from no answer */
	

	switch(fromBufferToUIAppInstanceData(&instanceData,dataToParse))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_ParseGetJobAttributesResponse-SUCCESS READING FROM BUFFER"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetJobAttributesResponse-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetJobAttributesResponse-INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_PARSING:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetJobAttributesResponse-INTERNAL_FUNC_FAILED_PARSING"));
			break;
		default:
			break;
	}	
	attributesData->operationStatus = (BtlBppsndJobOperationStatus)instanceData.btAppBppInstanceData.operationStatus;

	
	if((attributesMask & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL )== BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_ALL)
	{
		allAttr = TRUE;
	}
	if(allAttr||((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_STATE))
	{
		if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"printing")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_PRINTING;
			stateFound =TRUE;
		}
		else if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"waiting")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_WAITING;
			stateFound =TRUE;
		}
		else if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"stopped")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_STOPPED;
			stateFound =TRUE;
		}
		else if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"completed")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_COMPLETED;
			stateFound =TRUE;
		}
		else if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"aborted")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_ABORTED;
			stateFound =TRUE;
		}
		else if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"cancelled")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_CANCELLED;
			stateFound =TRUE;
		}
		else if(OS_StrCmp((char*)instanceData.btAppBppInstanceData.jobState,"unknown")==0)
		{
			attributesData->jobState = BTL_BPPSND_JOB_STATE_UNKNOWN;
			stateFound =TRUE;
		}
		if(stateFound)
		{
			(*jobAttributesMaskFound) += BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_STATE;
		}
		
	}
	
	if(allAttr||((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_NAME))
	{
		if(*instanceData.btAppBppInstanceData.jobName != '\0')
		{
			OS_StrnCpy((char*)attributesData->jobName,
						(char*)instanceData.btAppBppInstanceData.jobName, 
						BTL_BPPSND_MAX_JOB_NAME_LEN);	
			(*jobAttributesMaskFound) += BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_NAME;
		}
	}
	if(allAttr||((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME))
	{
		if(*instanceData.btAppBppInstanceData.jobOriginatingUserName!= '\0')
		{
			OS_StrnCpy((char*)attributesData->jobOriginatingUserName,
						(char*)instanceData.btAppBppInstanceData.jobOriginatingUserName, 
						BTL_BPPSND_MAX_JOB_ORIGINATING_USER_NAME_LEN);		
			(*jobAttributesMaskFound) += BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ORIGINATING_USER_NAME;
		}
		
	}
	if(allAttr||((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_MEDIA_SHEETS_COMPLETED))
	{
		if(instanceData.btAppBppInstanceData.jobMediaSheetsCompleted != BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE)
		{
			attributesData->jobMediaSheetsCompleted = instanceData.btAppBppInstanceData.jobMediaSheetsCompleted;
			(*jobAttributesMaskFound) += BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_MEDIA_SHEETS_COMPLETED;
		}
	}
	if(allAttr||((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_NUM_OF_INTERVENING_JOBS))
	{
		if(instanceData.btAppBppInstanceData.numOfInterveningJobs != BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE)
		{
			attributesData->numOfInterveningJobs = instanceData.btAppBppInstanceData.numOfInterveningJobs;
			(*jobAttributesMaskFound) += BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_NUM_OF_INTERVENING_JOBS;
		}
	}
	if(allAttr||((attributesMask) & BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ID))
	{
		if(instanceData.btAppBppInstanceData.jobId != BTL_BPPSND_JOB_ATTR_DEFUALT_START_VALUE)
		{
			attributesData->jobId = instanceData.btAppBppInstanceData.jobId;
			(*jobAttributesMaskFound) += BTL_BPPSND_GET_JOB_ATTRIBUTE_MASK_JOB_ID;
		}
	}
	BTL_FUNC_END();
	return status;

}



/*-------------------------------------------------------------------------------
 * Btl_BppSnd_ParseCancelJobResponse()
 *
 *		Parse a SOAP XML CancelJob response 
 *
 *		This function parses the response, and sets the available attribute values in 
 *		attributesData.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		dataToParse [in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the response attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */


BtStatus Btl_BppSnd_ParseCancelJobResponse(
			const BtlObject 					*dataToParse,
			BtlBppsndCancelJobResponseData	*attributesData)
{
	BtStatus status = BT_STATUS_SUCCESS;
	UIAppInstanceData instanceData={0};  /*  set at start to 0 */
	BTL_FUNC_START("Btl_BppSnd_ParseCancelJobResponse");

	switch(fromBufferToUIAppInstanceData(&instanceData,dataToParse))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_ParseCancelJobResponse-SUCCESS READING FROM BUFFER"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseCancelJobResponse-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseCancelJobResponse-INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_PARSING:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseCancelJobResponse-INTERNAL_FUNC_FAILED_PARSING"));
			break;
		default:
			break;
	}	
	attributesData->jobId = (U32)instanceData.btAppBppInstanceData.jobId;
	attributesData->operationStatus = (BtlBppsndJobOperationStatus)instanceData.btAppBppInstanceData.operationStatus;
	BTL_FUNC_END();
	return status;

}


/*-------------------------------------------------------------------------------
 * Btl_BppSnd_ParseGetEventResponse()
 *
 *		Parse a SOAP XML GetEvent response 
 *
 *		This function parses the response, and sets the attribute values in 
 *		attributesData.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		 dataToParse[in] - describes XML response information 
 *
 *		attributesData [out] - Contains the values of the response attributes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - Operation is successful.
 *
 *		BT_STATUS_FAILED - The operation failed.
 */
 
BtStatus Btl_BppSnd_ParseGetEventResponse(
				const BtlObject 				*dataToParse, 
				BtlBppsndGetEventResponseData	*attributesData)
{
	BtStatus status = BT_STATUS_SUCCESS;
	UIAppInstanceData instanceData={0};  /*  set at start to 0 */
	BTL_FUNC_START("Btl_BppSnd_ParseGetEventResponse");

	switch(fromBufferToUIAppInstanceData(&instanceData,dataToParse))
	{
		case INTERNAL_FUNC_SUCCESS:
			BTL_LOG_DEBUG(("Btl_BppSnd_ParseGetEventResponse-SUCCESS READING FROM BUFFER"));
			break;
		case INTERNAL_FUNC_FAILED_OPENING_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetEventResponse-INTERNAL_FUNC_FAILED_OPENING_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetEventResponse-INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE"));
			break;
		case INTERNAL_FUNC_FAILED_PARSING:
			BTL_VERIFY_ERR((TRUE), 	BT_STATUS_FAILED, 
				("Btl_BppSnd_ParseGetEventResponse-INTERNAL_FUNC_FAILED_PARSING"));
			break;
		default:
			break;
	}	
	attributesData->printerState = (BtlBppsndPrinterState)(instanceData.btAppBppInstanceData.printerState + 1);
	attributesData->printerStateReasons = (BtlBppsndPrinterStateReasons)(instanceData.btAppBppInstanceData.printerStateReason + 1);	
	
	attributesData->operationStatus = (BtlBppsndJobOperationStatus)instanceData.btAppBppInstanceData.operationStatus;
	BTL_FUNC_END();
	return status;

}



/*******************************************************************************************************************
* 
*										Internal Function 
*
*******************************************************************************************************************/
/*----------------------------------------------------------------------------
*  fromFileToBuffer()
*
*	this function used to insert the data to a buffer.
*
*
* Type:
*		Synchronous
*
* Parameters:
*		fsPath [in] - the Path of the file in the file system.
*		
*		buffer [out] - the data buffered here. 
*
* Returns:
*		
*		Returns actual length was ren from file,
*		Returns INTERNAL_FUNC_FAILED_OPENING_FILE if the failed opening file.
*
*/
static U32 fromFileToBuffer(const BtlUtf8* fsPath , BtlUtf8 *sBuffer)
{
	BTHAL_U32 lengthRead =0;
	BthalFsFileDesc	fd;
	BtFsStatus 		fsStatus;
	
	fsStatus = BTHAL_FS_Open(fsPath, BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY, &fd);
	if(fsStatus == BT_STATUS_HAL_FS_SUCCESS )
	{
		fsStatus = BTHAL_FS_Read( fd, sBuffer,sizeof(U8)*BTL_BPPSND_MAX_SOAP_FILE_SIZE, &lengthRead);
		BTHAL_FS_Close( fd);
	}
	
	return lengthRead;
}

/*----------------------------------------------------------------------------
*  fromBufferToObjLoc()
*
*	this function insert's the data to a buffer.
*
*
* Type:
*		Synchronous
*
* Parameters:
*		sBuffer [in] - request string.
*		
*		requestObj [out] - where to place the request. 
*		
*		contentLength[in] - the length of sBuffer. 
* Returns:
*		
* 		
*/
static int fromBufferToObjLoc(const BtlObject 	*requestObj, const BtlUtf8* sBuffer, U16 contentLength)
{
	U32 lengthWritten =0;
	int successIndex = INTERNAL_FUNC_SUCCESS;/* , streamInt; */
	BthalFsFileDesc	fd;
	BtFsStatus 		fsStatus;

	
	
	if(requestObj->objectLocation & BTL_OBJECT_LOCATION_FS)
	{
		/*If we are writing to a file open the file*/
		fsStatus = BTHAL_FS_Open(requestObj->location.fsLocation.fsPath, BTHAL_FS_O_WRONLY | BTHAL_FS_O_BINARY, &fd);
		if(fsStatus == BT_STATUS_HAL_FS_SUCCESS )
		{
			fsStatus = BTHAL_FS_Write( fd, (void *)sBuffer, sizeof(U8)*contentLength, &lengthWritten);
			if(fsStatus != BT_STATUS_HAL_FS_SUCCESS )
			{
				successIndex = INTERNAL_FUNC_FAILED_WRITING_TO_THE_FILE;
			}
			else if(lengthWritten < contentLength)
			{
				successIndex = INTERNAL_FUNC_FAILED_NOT_ALL_DATA_WRITEN; /* not all was writen */
			}
		}
		else
		{
			successIndex = INTERNAL_FUNC_FAILED_OPENING_FILE;
		}
		BTHAL_FS_Close( fd);
	}
	else
	{
		if(!((U32)contentLength > (requestObj->location.memLocation.size)))
		{
			/* the mem alocated is not shorter the the buffer to insert */
			OS_StrnCpyUtf8((BtlUtf8*)(requestObj->location.memLocation.memAddr) , sBuffer, contentLength);
		}
		else
		{	/* the mem alocated shorter the the buffer to insert */
			successIndex = INTERNAL_FUNC_FAILED_DATA_LONGER_THEN_MEM; /* memallocated not long enough */
		}
	}
	cleanBuffer(buffer);

	return successIndex ;
}

/*----------------------------------------------------------------------------
*  fromBufferToUIAppInstanceData()
*				
*		internal function that inserts the data from a file or memory (given in BtlObject) to a struct
*		that is used by the functions here.
*
* Type:
*		Synchronous
*
* Parameters:
*		dataToParse [in] - the location of the Data.
*		instanceData [out] - the data inserted to this structure . 
*
* Returns:
*		
*
*/

static int fromBufferToUIAppInstanceData(UIAppInstanceData * instanceData,const BtlObject *dataToParse)
{

	U32 sizeRead=0 ;
	int  success = INTERNAL_FUNC_SUCCESS;
	
	if((dataToParse->objectLocation & BTL_OBJECT_LOCATION_FS))
	{
	/*    the data to parse is in a file  */
		sizeRead = fromFileToBuffer(dataToParse->location.fsLocation.fsPath, buffer);
		if(!(sizeRead == INTERNAL_FUNC_FAILED_OPENING_FILE))
		{
			if(sizeRead >= BTL_BPPSND_MAX_SOAP_FILE_SIZE)
			{	
				success = INTERNAL_FUNC_FAILED_DIDNT_READ_ALL_FILE;
			}
			else
			{
				success = parseBppXmlRspData(instanceData,buffer,sizeRead);
			}
		}
		else
		{
			success = INTERNAL_FUNC_FAILED_OPENING_FILE;
		}
	}
	else  
	{
	/*   the data to parse is in a buffer elready */
		
		sizeRead= dataToParse->location.memLocation.size;
		success = parseBppXmlRspData(instanceData, (U8*)(dataToParse->location.memLocation.memAddr),sizeRead);
	}
	if(!success)
	{
		success = INTERNAL_FUNC_FAILED_PARSING;
	}

	cleanBuffer(buffer);
	return  success;
	
}
/*----------------------------------------------------------------------------
*  cleanBuffer()
*				
*		internal function that cleans the buffer after every use.
*
* Type:
*		Synchronous
*
* Parameters:
*
*		buffToClean [in/out] - the buffer to be cleaned.
*
* Returns:
*		
*
*/
void cleanBuffer(U8 *buffToClean)
{
	U32 index = 0;
	while(index<=BTL_BPPSND_MAX_SOAP_FILE_SIZE)
	{
		buffToClean[index] = '\0';
		index++;
	}
}


#endif /* BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED*/

