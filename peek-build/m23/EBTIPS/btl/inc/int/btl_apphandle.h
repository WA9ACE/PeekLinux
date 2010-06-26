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
*   FILE NAME:      btl_apphandle.h
*
*   DESCRIPTION:
*		An application handle.
*
* 		Each application can link between several modules (such as BMG), via an 
*		optional application handle.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_APPHANDLE_H
#define __BTL_APPHANDLE_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <btl_commoni.h>
#include <btl_config.h>
#include "btl_unicode.h"

 
/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlContextComparisonFunc type definition
 *
 *	A prototype of a comparison function that compares 2 contexts 
 *
 *	The function is used in the BTL_APP_HANDLE_FindMatchingContext function 
 *	to find a matching context from the contexts associated with an App handle
 *	It allows the caller to specify a matching policy that suits his needs.
 *
 *	The function should return:
 *		TRUE - the contexts match
 *		FALSE - the contexts do not match
*/
typedef BOOL (*BtlContextComparisonFunc)(const void* context1, const void* context2);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BtlAppHandle structure
 *
 *     BTL application handle data.
 */
struct _BtlAppHandle
{
	/* allows an App Handle to be placed on a list of all App handles */
	ListEntry	node;							

	/* List of contexts associated with this App handle */
	ListEntry	*contextsList[BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP];

	BtlUtf8		appName[BTL_CONFIG_MAX_APP_NAME + 1];
};

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTL_APP_HANDLE_Create()
 *
 *		Create a new App Handle instance.
 *
 * Parameters:
 *
 *		appHandle [in / out] - App Handle data ("this")
 *		
 *		appName [in] - Optional App name. May be 0. Useful for debugging purposes.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */

BtStatus BTL_APP_HANDLE_Create(BtlAppHandle *appHandle, const BtlUtf8  *appName);

/*-------------------------------------------------------------------------------
 * BTL_APP_HANDLE_Destroy()
 *
 *		Destroys an existing App Handle instance.
 *
 * Parameters:
 *
 *		appHandle [in / out] - App Handle data ("this")
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_APP_HANDLE_Destroy(BtlAppHandle *appHandle);

/*-------------------------------------------------------------------------------
 * BTL_APP_HANDLE_AddContext()
 *
 *		Adds a context. Should be calles whenever the application creates a new instance
 *		of some module.
 *
 * Parameters:
 *
 *		appHandle [in / out] - App Handle data ("this")
 *
 *		context [in] - added context
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_NO_RESOURCES - Max num of contexts per App Handle reached
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_APP_HANDLE_AddContext(BtlAppHandle *appHandle, BtlContext* context);

/*-------------------------------------------------------------------------------
 * BTL_APP_HANDLE_AddContext()
 *
 *		Removes a context. Should be calles whenever the application destroys an instance
 *		of some module
 *
 * Parameters:
 *
 *		appHandle [in / out] - App Handle data ("this")
 *
 *		context [in] - removed context
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_APP_HANDLE_RemoveContext(BtlAppHandle *appHandle, BtlContext* context);

/*-------------------------------------------------------------------------------
 * BTL_POOL_GetNumOfAllocatedElements()
 *
 *		Gets the number of contexts associated with this appHandle.
 *
 * Parameters:
 *
 *		appHandle [in] - App Handle data ("this")
 *
 *		num [out] - number of contexts
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_APP_HANDLE_GetNumOfContexts(const BtlAppHandle *appHandle, U32 *num);

/*-------------------------------------------------------------------------------
 * BTL_APP_HANDLE_IsContextOnList()
 *
 *		Checks if the specified context is associated with this appHandle.
 *
 *		Searches the context based on its address
 *
 * Parameters:
 *
 *		appHandle [in] - App Handle data ("this")
 *
 *		context [in] - context to search for
 *
 *		answer [out] - The answer:
 *						TRUE - The context is associated with this appHandle
 *						FALSE - The context is NOT associated with this appHandle
 *		
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_APP_HANDLE_IsContextOnList(	const BtlAppHandle *appHandle,
													const BtlContext* 		context, 
													BOOL 			*answer);

/*-------------------------------------------------------------------------------
 * BTL_APP_HANDLE_FindMatchingContext()
 *
 *		Checks if the specified context is associated with this appHandle.
 *
 *		The comparison is according to the matching policy defined by comparisonFunc
 *		(see the documentation of BtlContextComparisonFunc for details)		
 *
 * Parameters:
 *
 *		appHandle [in] - App Handle data ("this")
 *
 *		contextToMatch [in] - context to search for
 *
 *		comparisonFunc [in] - comparison function that defines the matching policy
 *
 *		matchingContext [out] - The matching context. 0 if none  was found
 *
 * Returns:
 *		BT_STATUS_SUCCESS - The operation was successful
 *
 *		BT_STATUS_INVALID_PARM - An invalid argument was passed (e.g., null pointer)
 */
BtStatus BTL_APP_HANDLE_FindMatchingContext(
				BtlAppHandle 				*appHandle, 
				const BtlContext* 			contextToMatch, 
				BtlContextComparisonFunc 	comparisonFunc,
				BtlContext				**matchingContext);

#endif /* __BTL_APPHANDLE_H */



