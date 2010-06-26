/* ============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================================== */
/**
* @file OMX_Core.c
*
* This file contains methods that provides OMX_GetHandle and OMX_Freehandle
* functionality that is OMX 1.0 complaint.
*
* @path  chipsetsw\services\omx_core\
*
* @rev  0.1
*
*/
/* ------------------------------------------------------------------------- */
/* ===========================================================================
*!
*! Revision History
*! ===================================
*! 15-Feb-2006 anandhi@ti.com: Initial Release
*!
* ========================================================================== */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <string.h>
#include <stdlib.h>

#include "typedefs.h"
#include "vsi.h"
#include "os.h"
#include "os_const.h"
#include "frm_glob.h"
/*-------program files ----------------------------------------*/
#include "OMX_Core.h"
#include "OMX_ComponentTable.h"
#include "OMX_TIImage.h"

extern  T_HANDLE mm_ext_data_pool_handle;
extern  T_HANDLE mm_int_data_pool_handle;


/* Note: This OMX core is TI OMX 1.0.0.0 compliant(In BCD format) */
static const OMX_VERSIONTYPE OMXCoreVersion = {0x01,0x00, 0x00, 0x00};


/* ========================================================================= */
/**
 * @fn OMX_Init
 *
 * Note: This method is not fully implemented in Nucleus version of OMX 1.0
 *       and it has been implemented with a return.
 *
 * @see OMX_Core.h
 */
/* ========================================================================= */

OMX_ERRORTYPE OMX_Init (void)
{
    return OMX_ErrorNone;
}



/* ========================================================================= */
/**
 * @fn OMX_Deinit
 *
 * Note: This method is not fully implemented in Nucleus version of OMX 1.0
 *       and it has been implemented with a return.
 *
 * @see OMX_Core.h
 */
/* ========================================================================= */

OMX_ERRORTYPE OMX_Deinit (void)
{
    return OMX_ErrorNone;
}



/* ========================================================================= */
/**
 * @fn OMX_GetHandle
 *
 * @see  OMX_Core.h
 */
/* ========================================================================= */

OMX_ERRORTYPE OMX_GetHandle (OMX_HANDLETYPE* pHandle,
                             OMX_STRING cComponentName,
                             OMX_PTR pAppData,
                             OMX_CALLBACKTYPE* pCallBacks)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* componentType = NULL;
    int i = 0;
    int bFound = 0;

    /* Check for valid component handle, Application has to allocate and
     * fill the size of the structure and version fields; Also validate other
     * input parameters.
     */
    if ((NULL == pHandle) ||
        (NULL == cComponentName) ||
        (NULL == pAppData) ||
        (NULL == pCallBacks)) {

        retVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    /* Verify that the name is not too long and could cause a crash.  Notice
     * that the comparison is a greater than or equals.  This is to make
     * sure that there is room for the terminating NULL at the end of the
     * name.
     */
    if (strlen(cComponentName) >= OMX_MAX_NAME_SIZE) {

        retVal = OMX_ErrorInvalidComponentName;
        goto EXIT;
    }

    /* Search through the component list for the matching component name and
     * execute the Init function of the component
     */
    i = 0;
    while (NULL != OMXComponentList[i].cComponentName) {
        if (strcmp(cComponentName, OMXComponentList[i].cComponentName) == 0) {

            /* Component found */
            bFound = 1;
            break;
        }
        i++;
    }

    if (bFound) {
        /* Allocate memory for the component handle which is of the type
         * OMX_COMPONENTTYPE and fill in the size and version field of the
         * structure
         */
        *pHandle = NULL;
        *pHandle = (OMX_HANDLETYPE)OMX_ALLOC (sizeof(OMX_COMPONENTTYPE));
       if(NULL == *pHandle) {
            retVal = OMX_ErrorInsufficientResources;
            goto EXIT;
        }
        componentType = (OMX_COMPONENTTYPE*) *pHandle;
        componentType->nSize = sizeof(OMX_COMPONENTTYPE);

        /* OMX version is TI OMX 1.0 i.e version 1.0.0.0 */
        componentType->nVersion.nVersion = OMXCoreVersion.nVersion;

        /* Call the Init function of the component using Init function
         * pointer updated in the component list table
         */
        retVal = OMXComponentList[i].pComponentInit(*pHandle);
        if (OMX_ErrorNone != retVal) {
            goto EXIT;
        }

        /* Setting the callback function pointers */
        retVal = (componentType->SetCallbacks)(*pHandle, pCallBacks, pAppData);
        if (OMX_ErrorNone != retVal) {
            /* Since the SetCallbacks() failed, need to restore to the original
             * state, hence call ComponentDeInit() here
             */
            componentType->ComponentDeInit(*pHandle);
            goto EXIT;
        }
    }
    else {
        /* Component was not found in the component table */
        retVal = OMX_ErrorComponentNotFound;
    }

EXIT:
    /* Perform cleanup on error */
    if ((OMX_ErrorNone  != retVal) && (NULL != *pHandle)) {
        OMX_FREE(*pHandle);
        *pHandle = NULL;
    }
    return retVal;
}



/* ========================================================================= */
/*
 * @fn OMX_FreeHandle
 *
 * @see  OMX_Core.h
 */
/* ========================================================================= */

OMX_ERRORTYPE OMX_FreeHandle (OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_STATETYPE componentState;

    /* Check for valid input handle */
    if (NULL == hComponent) {
       retVal = OMX_ErrorBadParameter;
       goto EXIT;
    }

    /* Get the present state of the component */
    retVal = pHandle->GetState(hComponent, &componentState);
    if (OMX_ErrorNone != retVal) {
        goto EXIT;
    }

    /* The precondition to call this method is that the compononet to be freed
     * must be in the "Loaded" State. Hence checking the component for valid
     * state
     */
    if (OMX_StateLoaded != componentState)
    {
        retVal = OMX_ErrorInvalidState;
        goto EXIT;
    }

    /* Call the ComponentDeInit function */
    retVal = pHandle->ComponentDeInit(hComponent);
    if (OMX_ErrorNone != retVal) {
        goto EXIT;
    }
    /* Free the memory for the component handle */
    OMX_FREE(hComponent);

EXIT:
    return retVal;
}

OMX_PTR __omx_alloc(OMX_U32 nSize)
{
    T_HANDLE hTask;
    OMX_PTR pBuffer;
    hTask = e_running[os_MyHandle()];
    if (os_AllocateMemory ( hTask, (T_VOID_STRUCT**)&pBuffer,nSize,OS_NO_SUSPEND, mm_ext_data_pool_handle)!=OS_OK)
        return NULL;
    return pBuffer;
}

OMX_PTR __omx_int_alloc(OMX_U32 nSize)
{
    T_HANDLE hTask;
    OMX_PTR pBuffer;
    hTask = e_running[os_MyHandle()];

        if(OS_OK != os_AllocateMemory ( hTask, (T_VOID_STRUCT**)&pBuffer,nSize,OS_NO_SUSPEND, mm_int_data_pool_handle))
        {
            if(os_AllocateMemory ( hTask, (T_VOID_STRUCT**)&pBuffer,nSize,OS_NO_SUSPEND, mm_ext_data_pool_handle) != OS_OK)
                return NULL;
        }

        return(pBuffer);
}

OMX_U16 __omx_free(OMX_PTR pBuffer)
{
    if(pBuffer != NULL)
	{
	    T_HANDLE hTask;
        OMX_U16 retVal;
        hTask = e_running[os_MyHandle()];
        retVal = os_DeallocateMemory(hTask, (T_VOID_STRUCT *) pBuffer);
        return retVal;
    }
	else
	   return 0;
}

