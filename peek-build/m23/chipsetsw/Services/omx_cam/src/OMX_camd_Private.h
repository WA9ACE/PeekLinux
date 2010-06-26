/* =============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */

/**
 * @file OMX_CAM_Private.h
 *
 * This file is the header file for the OMX CAM component.  It contains 
 * header information private to the CAM Component.
 *
 * @path  chipsetsw\services\omx_CAM\
 *
 * @rev 0.1
 */
/* -------------------------------------------------------------------------- */
/* =============================================================================
 *!
 *! Revision History
 *! ===================================
 *! 31-Jan-2006 jrk@ti.com: Initial Release
 *! 06-June-2006 narendranmr@ti.com: Added additional fields in the private structure
 *! to manage omx buffer header pointers.
 * =========================================================================== */

#ifndef OMX_CAMD_PRIVATE_H
#define OMX_CAMD_PRIVATE_H

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
#include "rv_general.h"
#include "camd/camd_api.h"

#include "OMX_CAMDComponent.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define OMX_CAM_MAX_BUFFERS     4

#define OMX_SEMHANDLE T_HANDLE;
//This macro is used to exit in case of a bad parameter
#define OMX_CAM_EXIT_IF(CONDITION) {\
    if ((CONDITION)) {\
        tRetVal = OMX_ErrorBadParameter;\
        goto EXIT;\
    }\
} // End of Macro OMX_CAM_EXIT_IF()
//This
#define OMX_CAM_EXIT_IF_IS(CONDITION) {\
    if ((CONDITION)) {\
        tRetVal = OMX_ErrorInvalidState;\
        goto EXIT;\
    }\
} // End of Macro OMX_CAM_EXIT_IF_IS()

#define OMX_CAM_EXIT_IF_ERR(CONDITION) {\
    if ((CONDITION)) {\
        tRetVal = OMX_ErrorNotReady;\
        goto EXIT;\
    }\
} // End of Macro OMX_CAM_EXIT_IF_ERR()
/*******************************************************************************
 * Structures
 *******************************************************************************/
/*
 * OMX_CAM_CMDTYPE: This enumeration lists the commands that are exchanged between the CAM Client and the 
 * CAM Core task. Note that this is defined by the CAM Core task and there is no clash of enum values with the
 * Application task. The return path is via Callbacks only.
 */
typedef enum OMX_CAM_CMDTYPE{
    OMX_CAM_CMD_GETCAPTUREMODES =0,
    OMX_CAM_CMD_GETORIENTATIONS,
    OMX_CAM_CMD_GETROTATIONMODES,
    OMX_CAM_CMD_GETIMAGESIZES,
    OMX_CAM_CMD_GETFLASHMODES,
    OMX_CAM_CMD_GETEXPOSUREMODES,
    OMX_CAM_CMD_GETCOLORFORMATS,
    OMX_CAM_CMD_GETFRAMERATES,
    OMX_CAM_CMD_GETMIRRORMODES,
    OMX_CAM_CMD_GETFOCUSMODES,
    OMX_CAM_CMD_GETGAMMACORRSUPPORT,
    OMX_CAM_CMD_GETMAXOPTICALZOOM,
    OMX_CAM_CMD_GETMAXDIGITALZOOM,
    OMX_CAM_CMD_GETCONTRASTCONTROL,
    OMX_CAM_CMD_GETBRIGHTNESSCONTROL,
    OMX_CAM_CMD_GETSHARPNESSCONTROL,
    OMX_CAM_CMD_GETDITHERINGSUPPORT,
    OMX_CAM_CMD_SETCAPTUREMODE,
    OMX_CAM_CMD_SETFRMRATEMODE,
    OMX_CAM_CMD_SETFRMRATE,
    OMX_CAM_CMD_SETDITHERING,
    OMX_CAM_CMD_SETSHARPNESS,
    OMX_CAM_CMD_SETCONTRAST,
    OMX_CAM_CMD_SETBRIGHTNESS,
    OMX_CAM_CMD_SETSIZETYPE,
    OMX_CAM_CMD_SETWIDTH,
    OMX_CAM_CMD_SETHEIGHT,
    OMX_CAM_CMD_SETMIRRORTYPE,
    OMX_CAM_CMD_SETCAPTUREFORMAT,
    OMX_CAM_CMD_SETROTATION,
    OMX_CAM_CMD_SETFLASHMODE,
    OMX_CAM_CMD_SETEXPOSUREMODE,
    OMX_CAM_CMD_SETFOCUSMODE,
    OMX_CAM_CMD_SETOPTICALZOOM,
    OMX_CAM_CMD_SETDIGITALZOOM,
    OMX_CAM_CMD_SETGAMMA,
    OMX_CAM_CMD_SETMATRIXRANGE,
    OMX_CAM_CMD_SETMATRIX,
    OMX_CAM_CMD_SETCONFIG,
    OMX_CAM_CMD_USEBUFF,
    OMX_CAM_CMD_REGISTERCLIENT,
    OMX_CAM_CMD_WAIT,	      
    OMX_CAM_CMD_MOVETOREADY,
    OMX_CAM_CMD_UNREGISTERCLIENT
}OMX_CAM_CMDTYPE;

/* ========================================================================== */
/**
 * This is the struct for the private elements of the component.  These 
 * parameters are set by the application calling the CAM Component.  
 *
 */
/* ========================================================================== */

typedef struct OMX_CAM_COMPONENT_PRIVATETYPE
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

    /* Current State for the OMX Component */
    OMX_STATETYPE tCurState;

    /* Current configuration set to the camera */
    OMX_CAM_CONFIGTYPE *  ptCamStatus;

    /* 
     * Semaphore Related parameters
     * These are used for communication with the CAMD Core task
     */
    /* Caller ID*/
    OMX_U32 nSemCaller;

    /* Semaphore Handle */
    OMX_U32 nSemHandle;

    /* CAMD Core Task Handle */
    T_HANDLE  tCamdCoreHandle;

    /* Current task Handle */
    T_HANDLE   tCamdClientHandle;

    /* Error Status from the CAMD Core */
    OMX_ERRORTYPE tOMX_CAMErrStatus;

    T_CAMD_PARAMETERS tCamParam;

    OMX_PTR     aBuffHeaderArray[OMX_CAM_MAX_BUFFERS]; 

    OMX_U16     unBuffHeaderReadIndex;
    OMX_U16     unBuffHeaderWriteIndex;

    CAM_SENSOR_CAPABILITIES tCamFeatures;

}OMX_CAM_COMPONENT_PRIVATETYPE;

/*
 * OMX_CAM_MESSAGESTRUCT: This structure is the message structure that is used for the communication
 * betwwen the OMX CAM Client and the CAM Core task.
 * @params  nCMD : This is the command that is sent to the core task
 * @params  pArguement : This is a pointer to the structure/data passed along with the command. The interpretation is command
 *                       dependent.
 * @params pCallback : This is the call back function that must be called by the core for a response.
 */
typedef struct OMX_CAM_MESSAGE_STRUCTTYPE{
    OMX_CAM_CMDTYPE tCMD; //Command to the CAM Core
    OMX_PTR pArgument;// Pointer to auxilary data
    OMX_COMPONENTTYPE * pComponentHandle;// Component's Handle
    void (* pCAMCallback) (
            OMX_HANDLETYPE hComponent,	   	
            OMX_ERRORTYPE nErrorStatus,
            OMX_CAM_CMDTYPE tCommand,
            OMX_PTR pVoid//To be used later
            ); //Call back function pointer
}OMX_CAM_MESSAGE_STRUCTTYPE;

/* framework for all camd messages */
typedef T_CAMD_MSG T_OMX_CAMDMSGTYPE;

/* 
 * Internal Command Handler 
 */
OMX_ERRORTYPE __OMX_CAM_HandleCommand(OMX_HANDLETYPE hComponent, OMX_U32 newState) ;

/*
 * Function compose and commnicate the commands to the CAM Core.
 */
OMX_ERRORTYPE __OMX_CAM_SendMsgToCAMCore(OMX_HANDLETYPE hComponent, OMX_CAM_CMDTYPE tCmdToCAMCore, OMX_PTR pAuxDataToCAMCore);

/*
 * Callback function to be called by the core.
 */
void __OMX_CAM_Callback ( 	
        void * pCBMsg
        ); //Call back function pointer

/*
 * Function that sets default plane configuration values.
 */
void __OMX_CAM_SetDefaultParams(OMX_CAM_COMPONENT_PRIVATETYPE * pPrivateStruct);
T_CAMD_RESOLUTION map_TIItoCamdRes(OMX_TIIMAGE_DIMENSIONTYPE );
OMX_ERRORTYPE mapRivToTII_Error(T_RV_RET rivError);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OMX_CAM_PRIVATE_H */



