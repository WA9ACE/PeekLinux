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
*   FILE NAME:      btl_apphandle.c
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

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include <bttypes.h>
#include <utils.h>
#include <btl_defs.h>
#include <btl_apphandle.h>

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BTL_COMMON);

/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

static BOOL BTL_APP_HANDLE_CompareByAddress(const void* context1, const void* context2)
{
	if (context1 == context2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BtStatus BTL_APP_HANDLE_Create(BtlAppHandle *appHandle, const BtlUtf8* appName)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_APP_HANDLE_Create");
	
	/* Add the new appHandle to the list of existing App Handles */
	InitializeListEntry(&(appHandle->node));

	OS_MemSet((U8*)appHandle->contextsList, 0, BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP * sizeof(ListEntry*));

	appHandle->appName[BTL_CONFIG_MAX_APP_NAME] = '\0';
	
	if (0 != appName)
	{
		/* [ToDo][UdiR] Needs a call to a UTF 8 Version */
		OS_StrnCpy((char*)appHandle->appName, (const char*)appName, BTL_CONFIG_MAX_APP_NAME);
	}
	else
	{
		/* [ToDo][UdiR] Needs a call to a UTF 8 Version */
		OS_StrCpy((char*)appHandle->appName, "");
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_APP_HANDLE_Destroy(BtlAppHandle *appHandle)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	U32		numOfContexts = 0;

	BTL_FUNC_START("BTL_APP_HANDLE_Destroy");
		
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle"));
	BTL_VERIFY_FATAL((0 != IsEntryAvailable(&appHandle->node)), BT_STATUS_INTERNAL_ERROR, 
						("appHandle is still on a list"));

	/* It is illegal to destroy an appHandle while it has associated contexts */
	status = BTL_APP_HANDLE_GetNumOfContexts(appHandle, &numOfContexts);
	BTL_VERIFY_ERR_NORET((0 == numOfContexts), ("App Handle still has %d associated contexts", numOfContexts));

	OS_MemSet((U8*)appHandle->contextsList, 0, BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP * sizeof(ListEntry*));

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_APP_HANDLE_AddContext(BtlAppHandle *appHandle, BtlContext* context)
{
	BtStatus	status = BT_STATUS_NO_RESOURCES;
	U32		contextIndex = 0;

	BTL_FUNC_START("BTL_APP_HANDLE_AddContext");
	
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle"));
	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context"));

	for (contextIndex = 0; contextIndex < BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP; ++contextIndex)
	{
		if (0 == appHandle->contextsList[contextIndex])
		{
			appHandle->contextsList[contextIndex] = &context->node;
			status = BT_STATUS_SUCCESS;
			break;
		}
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_APP_HANDLE_RemoveContext(BtlAppHandle *appHandle, BtlContext* context)
{
	BtStatus	status = BT_STATUS_INVALID_PARM;
	U32		contextIndex = 0;

	BTL_FUNC_START("BTL_APP_HANDLE_RemoveContext");
	
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle argument "));
	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context argument"));
	
	for (contextIndex = 0; contextIndex < BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP; ++contextIndex)
	{
		if (&context->node == appHandle->contextsList[contextIndex])
		{
			appHandle->contextsList[contextIndex] = 0;
			status = BT_STATUS_SUCCESS;
		}
	}

	BTL_VERIFY_FATAL(	(BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, 
						("Context is not associated with this App Handle"));
	
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_APP_HANDLE_GetNumOfContexts(const BtlAppHandle *appHandle, U32 *num)
{
	BtStatus	status = BT_STATUS_SUCCESS;
	U32		contextIndex = 0;
	
	BTL_FUNC_START("BTL_APP_HANDLE_RemoveContext");
	
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle argument"));
	BTL_VERIFY_FATAL((0 != num), BT_STATUS_INTERNAL_ERROR, ("Null num argument"));
	
	*num = 0;
	
	for (contextIndex = 0; contextIndex < BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP; ++contextIndex)
	{
		if (0 != appHandle->contextsList[contextIndex])
		{
			++(*num);
		}
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_APP_HANDLE_IsContextOnList(	const BtlAppHandle *appHandle,
													const BtlContext	*context, 
													BOOL 			*answer)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BtlContext*	matchingContext = 0;		
	
	BTL_FUNC_START("BTL_APP_HANDLE_IsContextOnList");
	
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle argument"));
	BTL_VERIFY_FATAL((0 != context), BT_STATUS_INTERNAL_ERROR, ("Null context argument"));
	BTL_VERIFY_FATAL((0 != answer), BT_STATUS_INTERNAL_ERROR, ("Null answer argument"));

	status = BTL_APP_HANDLE_FindMatchingContext(	(BtlAppHandle*)appHandle, 
													context, 
													BTL_APP_HANDLE_CompareByAddress,
													&matchingContext);
	BTL_VERIFY_FATAL((BT_STATUS_SUCCESS == status), BT_STATUS_INTERNAL_ERROR, (""));

	if (0 != matchingContext)
	{
		*answer = TRUE;
	}
	else
	{
		*answer = FALSE;
	}

	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_APP_HANDLE_FindMatchingContext(
				BtlAppHandle 				*appHandle, 
				const BtlContext* 			contextToMatch, 
				BtlContextComparisonFunc 	comparisonFunc,
				BtlContext				**matchingContext)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	U32		contextIndex = 0;
	
	BTL_FUNC_START("BTL_APP_HANDLE_FindMatchingContext");
	
	BTL_VERIFY_FATAL((0 != appHandle), BT_STATUS_INTERNAL_ERROR, ("Null appHandle argument"));
	BTL_VERIFY_FATAL((0 != contextToMatch), BT_STATUS_INTERNAL_ERROR, ("Null contextToMatch argument"));
	BTL_VERIFY_FATAL((0 != comparisonFunc), BT_STATUS_INTERNAL_ERROR, ("Null comparisonFunc argument"));
	BTL_VERIFY_FATAL((0 != matchingContext), BT_STATUS_INTERNAL_ERROR, ("Null matchingContext argument"));

	*matchingContext = 0;
	
	for (contextIndex = 0; contextIndex < BTL_CONFIG_MAX_NUM_OF_CONTEXTS_PER_APP; ++contextIndex)
	{
		if (0 != appHandle->contextsList[contextIndex])
		{
			if (TRUE == comparisonFunc(contextToMatch, appHandle->contextsList[contextIndex]))
			{
				*matchingContext = (BtlContext*)appHandle->contextsList[contextIndex];
			}
		}
	}

	BTL_FUNC_END();
	
	return status;
}

