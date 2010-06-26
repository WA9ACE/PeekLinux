/*******************************************************************************\
*                                                                                                                                         	*
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
*   FILE NAME:      Btl_bppsnd_soapInterface.h
*
*   DESCRIPTION:  
*				
*					
*
*   AUTHOR:         Zvi Schneider
*
\*******************************************************************************/
#ifndef __BTL_BPPSND_SOAPINTERFACE_H
#define __BTL_BPPSND_SOAPINTERFACE_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_bppsnd.h"

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



/*******************************************************************************/



/*******************************************************************************/

/******************************************************************************************************
*
*												BUILDING FUNCTIONS
*
*******************************************************************************************************/
/*-------------------------------------------------------------------------------
 * BTL_BPPSND_GetPrinterAttributes()
 *
 *		Initiates a BPP GetPrinterAttributes operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		attributesMask [in] - Mask of printer attributes to request from the printer. 
 *						   The caller should set the bits in the mask that corrsepond to features
 *							in which it is interested. Multiple features may be Ored together.
 *					
 *							The pointer is free after the function returns
 *
 *		requestObj [in/out] - describes where to place the SOAP XML response contents
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
BtStatus Btl_BppSnd_BuildGetPrinterAttributes(
			BtlBppsndPrinterAttributesMask	attributesMask,											
			const BtlObject 				*requestObj,
			U16 *size);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_CreateJob()
 *
 *		Initiates a BPP CreateJob operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
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
 *		requestObj [in/out] - describes where to place the SOAP XML response contents
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
 
BtStatus Btl_BppSnd_BuildCreateJob(
									const BtlBppsndCreateJobRequestData		*attributesData,
									const BtlBppsndCreateJobAttributesMask	attributesMask,
									const BtlObject 						*requestObj,
									U16 *size);


/*-------------------------------------------------------------------------------
 * BTL_BPPSND_GetJobAttributes()
 *
 *		Initiates a BPP GetJobAttributes operation.
 *		This function can be called only after connecting to the printer via 
 *		BTL_BPPSND_Connect().
 *
 * Type:
 *		Asynchronous
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
 *		requestObj [in/out] - describes where to place the SOAP XML response contents
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
BtStatus Btl_BppSnd_BuildGetJobAttributes(	
											BtlBppsndJobId					jobId,
											const BtlBppsndGetJobAttributesMask	attributesMask,
											const BtlObject 					*requestObj,
											U16 *size);

/*-------------------------------------------------------------------------------
 * BTL_BPPSND_CancelJob()
 *
 *		Cancels a particular print job
 *
 * Type:
 *		Asynchronous
 *
 * Parameters:
 *
 *		jobId [in] - Job ID obtained via the response to the corresponding BTL_BPPSND_CreateJob request
 *
 *		requestObj [in/out] - describes where to place the SOAP XML response contents
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
BtStatus Btl_BppSnd_BuildCancelJob(	
										BtlBppsndJobId	jobId,
										const BtlObject 	*requestObj,
										U16 *size);
/*-------------------------------------------------------------------------------
 * BTL_BPPSND_StartGetEvent()
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
 *		Asynchronous
 *
 * Parameters:
 *
 *		jobId [in] - Job ID obtained via the response to the corresponding BTL_BPPSND_CreateJob request
 *
 *		requestObj [in/out] - describes where to place the SOAP XML response contents
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
BtStatus Btl_BppSnd_BuildStartGetEvent(
										BtlBppsndJobId	jobId,
										const BtlObject	*requestObj,
										U16 *size);





/******************************************************************************************************
*
*												PARSINGS FUNCTIONS
*
*******************************************************************************************************/
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
							BtlBppsndPrinterAttributesMask	*printerAttributesMaskFound );
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
			BtlBppsndCreateJobResponseData	*attributesData);


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
 *		dataToParse [in] - describes XML response information 
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

BtStatus Btl_BppSnd_ParseGetJobAttributesResponse(
			const BtlObject 						*dataToParse, 
			BtlBppsndGetJobAttributesResponseData	*attributesData,
			BtlBppsndGetJobAttributesMask			attributesMask,
			BtlBppsndGetJobAttributesMask	*jobAttributesMaskFound);


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
			BtlBppsndCancelJobResponseData	*attributesData);


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
				BtlBppsndGetEventResponseData	*attributesData);




#endif  /* ___BTL_BPPSND_SOAPINTERFACE_H */







































