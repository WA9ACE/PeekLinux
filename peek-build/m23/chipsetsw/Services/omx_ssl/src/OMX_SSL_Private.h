/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */

/**
* @file OMX_SSL_Private.h
*
* This file is the header file for the OMX SSL component.  It contains 
* header information private to the SSL Component.
*
* @path  chipsetsw\services\omx_ssl\
*
* @rev 0.1
*/
/* -------------------------------------------------------------------------- */
/* =============================================================================
*!
*! Revision History
*! ===================================
*! 31-Jan-2006 jrk@ti.com: Initial Release
* =========================================================================== */


#ifndef OMX_SSL_PRIVATE_H
#define OMX_SSL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************************************
* Includes
*******************************************************************************/

/* OMX_Image common header file*/
#include "typedefs.h"
#include "vsi.h"
#include "os.h"
#include "frm_glob.h"


#include "OMX_SSLComponent.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define OMX_SEMHANDLE T_HANDLE;
//This macro is used to exit in case of a bad parameter
#define OMX_SSL_EXIT_IF(CONDITION) {\
    if ((CONDITION)) {\
        tRetVal = OMX_ErrorBadParameter;\
        goto EXIT;\
    }\
} // End of Macro OMX_SSL_EXIT_IF()
//This
#define OMX_SSL_EXIT_IF_IS(CONDITION) {\
    if ((CONDITION)) {\
        tRetVal = OMX_ErrorInvalidState;\
        goto EXIT;\
    }\
} // End of Macro OMX_SSL_EXIT_IF_IS()

#define OMX_SSL_EXIT_IF_ERR(CONDITION) {\
    if ((CONDITION)) {\
        tRetVal = OMX_ErrorNotReady;\
        goto EXIT;\
    }\
} // End of Macro OMX_SSL_EXIT_IF_ERR()
/*******************************************************************************
* Structures
*******************************************************************************/
/* ========================================================================== */
/**
* This is the struct for the private elements of the component.  These 
* parameters are set by the application calling the SSL Component.  
*
*/
/* ========================================================================== */

typedef struct OMX_SSL_COMPONENT_PRIVATETYPE
{
   /** nVersion is the version of the OMX specification that the structure
        is built against.  It is the responsibility of the creator of this
        structure to initialize this value and every user of this structure
        should verify that it knows how to use the exact version of
        this structure found herein. */
    OMX_VERSIONTYPE nCompVersion;

    /** Array of pointers to BUFFERHEADERTYPE structues.  This 
        pBufHeader[INPUT_PORT] will point to all the BUFFERHEADERTYPE 
        structures related to input port, not just one structure. */
    OMX_BUFFERHEADERTYPE* pInBufHeader;

    /** Handle for use with async callbacks */
    OMX_CALLBACKTYPE cbInfo;

    /** This will contain info like how many buffers are there for input/output 
        ports, their size etc, but not BUFFERHEADERTYPE POINTERS. */
    OMX_IMAGE_PORTDEFINITIONTYPE pPortDef;

   /**
    *  The Plane Characteristics. Note that the plane characteristics have
    *  been encapsulated in a single structure instead of a lengthy list.
    **/   
   OMX_SSL_PLANE_QUERYTYPE * pPlaneHandle;
  
   /* Current State for the OMX Component */
   OMX_STATETYPE tCurState;

    /* 
     * Semaphore Related parameters
     * These are used for communication with the SSL Core task
     */
   /* Caller ID*/
   OMX_U32 nSemCaller;

   /* Semaphore Handle */
   OMX_U32 nSemHandle;

   /* SSL Core Task Handle */
   T_HANDLE  pSSLCoreHandle;

   /* Current task Handle */
   T_HANDLE   tSSLClientHandle;

   /* Error Status from the SSL Core */
   OMX_ERRORTYPE tOMX_SSLErrStatus;
   
}OMX_SSL_COMPONENT_PRIVATETYPE;

 /*
  * OMX_SSL_MESSAGESTRUCT: This structure is the message structure that is used for the communication
  * betwwen the OMX SSL Client and the SSL Core task.
  * @params  nCMD : This is the command that is sent to the core task
  * @params  pArguement : This is a pointer to the structure/data passed along with the command. The interpretation is command
  *                       dependent.
  * @params pCallback : This is the call back function that must be called by the core for a response.
  */
typedef struct OMX_SSL_MESSAGE_STRUCTTYPE{
	   OMX_SSL_CMDTYPE tCMD; //Command to the SSL Core
	   OMX_PTR pArgument;// Pointer to auxilary data
	   OMX_COMPONENTTYPE * pComponentHandle;// Component's Handle
	   void (* pSSLCallback) (
	   	OMX_HANDLETYPE hComponent,	   	
	   	OMX_ERRORTYPE nErrorStatus,
	   	OMX_SSL_CMDTYPE tCommand,
	   	OMX_PTR pVoid//To be used later
	   	); //Call back function pointer
}OMX_SSL_MESSAGE_STRUCTTYPE;
 
/* 
 * Internal Command Handler 
 */
OMX_ERRORTYPE __OMX_SSL_HandleCommand (OMX_HANDLETYPE hComponent, 
                                            OMX_U32 newState);

/*
 * Function compose and commnicate the commands to the SSL Core.
 */
OMX_ERRORTYPE __OMX_SSL_SendMsgToSSLCore(OMX_HANDLETYPE hComponent, OMX_SSL_CMDTYPE tCmdToSSLCore, OMX_PTR pAuxDataToSSLCore);

/*
 * Callback function to be called by the core.
 */
void __OMX_SSL_Callback (OMX_HANDLETYPE hComponent,	   	
	   	OMX_ERRORTYPE nErrorStatus,
	   	OMX_SSL_CMDTYPE tCommand,
	   	OMX_PTR pVoid//To be used later
	   	); //Call back function pointer

/*
 * Function that returns the bits per pixel for a given image format
 */
OMX_U8 __OMX_SSL_COMPUTEBITSPERPIXEL(OMX_TICOLOR_FORMATTYPE tPlaneImgFormat);

/*
 * Function that sets default plane configuration values.
 */
//void __OMX_SSL_SetDefaultParams(OMX_SSL_COMPONENT_PRIVATETYPE * pPrivateStruct);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OMX_SSL_PRIVATE_H */



