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
*   FILE NAME:      btl_defs.h
*
*   DESCRIPTION:    This file defines common macros that should be used for message logging, 
*					and exception checking, handling and reporting
*
*					In addition, it contains miscellaneous other related definitions.
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/


#ifndef __BTL_DEFS_H
#define __BTL_DEFS_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bttypes.h>
#include <btl_log.h>
#include "btl_log_modules.h"

#define BTL_LOCK()		OS_LockStack()
#define BTL_UNLOCK()	OS_UnlockStack()

typedef BtStatus (*BtlDefsMemReleaseFunc)(void **element);

/*---------------------------------------------------------------------------
 *
 * Used to define unused function parameters in EBTIPS RELEASE configuration. Some compilers warn if this
 * is not done.
 */

#ifdef EBTIPS_RELEASE
#define BTL_UNUSED_PARAMETER_EBTIPS_RELEASE(_PARM)		((_PARM) = (_PARM))
#else
#define BTL_UNUSED_PARAMETER_EBTIPS_RELEASE(_PARM)
#endif

 #ifdef EBTIPS_RELEASE
#define BTL_LOG_API_FUNCTION_ENTRY_EXIT		FALSE
#else
#define BTL_LOG_API_FUNCTION_ENTRY_EXIT		TRUE
#endif

#if (TRUE == BTL_LOG_API_FUNCTION_ENTRY_EXIT)

#define BTL_LOG_FUNCTION_ENTRY				BTL_LOG_FUNCTION(("Entered %s", btlDbgFuncName))								
#define BTL_LOG_FUNCTION_EXIT				BTL_LOG_FUNCTION(("Exiting %s", btlDbgFuncName))											
#define BTL_LOG_DEFINE_FUNC_NAME(funcName)	const char* btlDbgFuncName = funcName

#else

#define BTL_LOG_FUNCTION_ENTRY
#define BTL_LOG_DEFINE_FUNC_NAME(funcName)
#define BTL_LOG_FUNCTION_EXIT

#endif

#define BTL_ASSERT_TREAT_ERROR_AS_FATAL		FALSE

#if XA_DEBUG == XA_ENABLED
#define BTL_ASSERT(condition)						\
		OS_Assert(#condition, __FILE__, (U16)__LINE__)
#else
#define BTL_ASSERT(condition)
#endif /* XA_DEBUG == XA_ENABLED */

#if (TRUE == 	BTL_ASSERT_TREAT_ERROR_AS_FATAL)

#define BTL_ASSERT_ERROR(condition)					\
			BTL_ASSERT(condition)

#else

#define BTL_ASSERT_ERROR(condition)

#endif


extern BOOL BTL_IsInitialized();
extern BOOL BTL_IsOperational();

#define BTL_FUNC_START_AND_LOCK_COMMON(funcName)		\
	BTL_LOG_DEFINE_FUNC_NAME(funcName);			\
	BTL_LOCK();										\
	BTL_LOG_FUNCTION_ENTRY							\

#define BTL_FUNC_START_AND_LOCK(funcName)			\
	BTL_FUNC_START_AND_LOCK_COMMON(funcName);		\
	BTL_VERIFY_ERR((BTL_IsOperational() == TRUE), 		\
					BT_STATUS_RADIO_OFF,				\
					("Radio Off (BTIPS Not Operational)"))

#define BTL_FUNC_START_AND_LOCK_RADIO_OFF(funcName)	\
	BTL_FUNC_START_AND_LOCK_COMMON(funcName);			\
	BTL_VERIFY_ERR((BTL_IsInitialized() == TRUE), 	\
					BT_STATUS_RADIO_OFF,			\
					("BTIPS Not Initialized)"))

#define BTL_FUNC_END_AND_UNLOCK()				\
	goto CLEANUP;								\
	CLEANUP:									\
	BTL_LOG_FUNCTION_EXIT;						\
	BTL_UNLOCK()


#define BTL_FUNC_START(funcName)			\
	BTL_LOG_DEFINE_FUNC_NAME(funcName);	\
	BTL_LOG_FUNCTION_ENTRY

#define BTL_FUNC_END()						\
	goto CLEANUP;							\
	CLEANUP:								\
	BTL_LOG_FUNCTION_EXIT

#define BTL_LOG_SET_MODULE(moduleType)	\
	static const U8 btlLogModuleType = moduleType

#define BTL_VERIFY_ERR_NORET(condition, msg)	\
		if ((condition) == 0)						\
		{										\
			BTL_LOG_ERROR(msg);				\
			BTL_ASSERT_ERROR(condition);		\
		}
		
#define BTL_VERIFY_FATAL_NORET(condition, msg)		\
		if ((condition) == 0)							\
		{											\
			BTL_LOG_FATAL(msg);					\
			BTL_ASSERT(condition);					\
		}

#define BTL_VERIFY_ERR_NO_RETVAR(condition, msg)	\
		if ((condition) == 0)						\
		{										\
			BTL_LOG_ERROR(msg);				\
			BTL_ASSERT_ERROR(condition);		\
			goto CLEANUP;						\
		}
		
#define BTL_VERIFY_FATAL_NO_RETVAR(condition, msg)		\
		if ((condition) == 0)							\
		{											\
			BTL_LOG_FATAL(msg);					\
			BTL_ASSERT(condition);					\
			goto CLEANUP;							\
		}

#define BTL_ERR_NORET(msg)							\
			BTL_LOG_ERROR(msg);					\
			BTL_ASSERT_ERROR(0);
		
#define BTL_FATAL_NORET(msg)						\
			BTL_LOG_FATAL(msg);					\
			BTL_ASSERT(0);

#define BTL_RET_SET_RETVAR(setRetVarExp)			\
	(setRetVarExp);									\
	goto CLEANUP
	
#define BTL_VERIFY_ERR_SET_RETVAR(condition, setRetVarExp, msg)		\
		if ((condition) == 0)											\
		{															\
			BTL_LOG_ERROR(msg);									\
			(setRetVarExp);											\
			BTL_ASSERT_ERROR(condition);							\
			goto CLEANUP;											\
		}			

#define BTL_VERIFY_FATAL_SET_RETVAR(condition, setRetVarExp, msg)	\
		if ((condition) == 0)						\
		{										\
			BTL_LOG_FATAL(msg);				\
			(setRetVarExp);						\
			BTL_ASSERT(condition);				\
			goto CLEANUP;						\
		}

#define BTL_ERR_SET_RETVAR(setRetVarExp, msg)		\
		BTL_LOG_ERROR(msg);						\
		(setRetVarExp);								\
		BTL_ASSERT_ERROR(0);						\
		goto CLEANUP;

#define BTL_FATAL_SET_RETVAR(setRetVarExp, msg)	\
			BTL_LOG_FATAL(msg);					\
			(setRetVarExp);							\
			BTL_ASSERT(0);							\
			goto CLEANUP;


#define BTL_RET(returnCode)								\
		BTL_RET_SET_RETVAR(status = returnCode)

#define BTL_RET_NO_RETVAR()							\
		BTL_RET_SET_RETVAR(status = status)

#define BTL_VERIFY_ERR(condition, returnCode, msg)		\
		BTL_VERIFY_ERR_SET_RETVAR(condition, (status = (returnCode)), msg)

 #define BTL_VERIFY_FATAL(condition, returnCode, msg)	\
		BTL_VERIFY_FATAL_SET_RETVAR(condition, (status = (returnCode)),  msg)

#define BTL_ERR(returnCode, msg)		\
		BTL_ERR_SET_RETVAR((status = (returnCode)), msg)

 #define BTL_FATAL(returnCode, msg)	\
		BTL_FATAL_SET_RETVAR((status = (returnCode)),  msg)

#define BTL_ERR_NO_RETVAR(msg)					\
			BTL_LOG_ERROR(msg);				\
			BTL_ASSERT_ERROR(condition);			\
			goto CLEANUP;

#define BTL_FATAL_NO_RETVAR(msg)				\
			BTL_LOG_FATAL(msg);				\
			BTL_ASSERT(0);						\
			goto CLEANUP;

#endif /* __BTL_DEFS_H */


