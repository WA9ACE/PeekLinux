/* =============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file OMX_SSLComponent.c
 *
 * This file contains methods that provides functionality for the Screen Services Layer component
 * for the Nucleus(tm) operating system on the Locosto platform.
 *
 * @path  chipsetsw\services\omx_ssl\
 *
 * @rev  0.1
 *
 */
/* ------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 31-Jan-2006 jrk@ti.com: Initial Release
 *! 22-Feb-2006 jrk@ti.com: First   Alpha
 *!
 * ========================================================================= */

/****************************************************************
 *  INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <string.h>
#include <stdlib.h>

/*-------program files ----------------------------------------*/
#include "OMX_Core.h"
#include "OMX_SSLComponent.h"
#include "OMX_SSL_Private.h"
#include "OMX_TIImage.h"


/*This Flag enables Get/Set Param*/
//#define __OMX_ENABLE_PARAMGETSET__
/*This Flag enables Granularity in Config setting*/
//#define __OMX_ENABLE_GRAN__

/* Note: This component is OMX 1.0 compliant */
static const OMX_VERSIONTYPE SupportedOMXVersion = {0x01,0x00, 0x00, 0x00};
extern T_HANDLE mm_ext_data_pool_handle;
/* ========================================================================== */
/**
 * @fn GetComponentVersion will return information about the component.
 *
 * @param [in] hComponent
 *     handle of component to execute the command
 * @param [out] pComponentName
 *     pointer to an empty string of length 128 bytes.
 * @param [out] pComponentVersion
 *     pointer to an OMX Version structure that the component will fill in.
 * @param [out] pSpecVersion
 *     pointer to an OMX Version structure that the component will fill in.
 * @param [out] pComponentUUID
 *     pointer to the UUID of the component which will be filled in by
 *     the component.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent,
        OMX_STRING  pComponentName,
        OMX_VERSIONTYPE* pComponentVersion,
        OMX_VERSIONTYPE* pSpecVersion,
        OMX_UUIDTYPE* pComponentUUID)
{

    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvtStructure;
    OMX_COMPONENTTYPE * pSSLHandle;
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;

    OMX_SSL_EXIT_IF((NULL == hComponent));
    OMX_SSL_EXIT_IF((NULL == pComponentVersion));
    OMX_SSL_EXIT_IF((NULL == pSpecVersion));

    pSSLHandle = (OMX_COMPONENTTYPE *)hComponent;
    pSSLPvtStructure = (OMX_SSL_COMPONENT_PRIVATETYPE *)pSSLHandle->pComponentPrivate;

    pComponentName    = OMXSSL_COMP_NAME;
    *(OMX_U32 *)pComponentVersion = pSSLPvtStructure->nCompVersion.nVersion;
    *(OMX_U32 *)pSpecVersion      = pSSLHandle->nVersion.nVersion;
    *(OMX_U32 *)pComponentUUID    = NULL;

EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn SendCommand method sends a command to the component. The component must
 * check the parameters and then queue the command to the component thread to
 * be executed. The component thread must send the HandleEvent callback at the
 * conclusion of the command.  This method will go directly from the application
 * to the component (via a core macro). The implementation for SSL Comprises of a
 * command handling routine that essentially maps, the commands for state
 * transitions to GPF commands to the SSL Core Task.
 *
 * @param [in] hComponent
 *     handle of component to execute the command
 * @param [in] Cmd
 *     Command for the component to execute
 * @param [in] nParam
 *     Parameter for the command to be executed.
 * @param  [in] Auxilary command data.
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComponent,
        OMX_COMMANDTYPE Cmd,
        OMX_U32 nParam1,
        OMX_PTR pCmdData)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    /* Check the input parameters. */
    OMX_SSL_EXIT_IF (NULL == hComponent);

    OMX_SSL_EXIT_IF (NULL == nParam1);

    //Currently only State Change Commands are supported.
    OMX_SSL_EXIT_IF (OMX_CommandStateSet != Cmd);

    /* Function to handle the command sequence/valid command */
    tRetVal = __OMX_SSL_HandleCommand(hComponent, nParam1);
EXIT:
    return tRetVal ;

}

/* ========================================================================== */
/**
 * @fn GetParameter method will get one of the current parameter settings from
 * the component.  This method can only be invoked when the component is in
 * the OMX_LoadedState state. The parameters are essentially pre-create
 * parameters or one-time configuration parameters. The parameters supported
 * and their indices are:
 * @param OMX_IndexParamNumAvailableDisplays : Number of Physical Displays available
 * @param OMX_IndexParamDisplayInfo    : Vendor Names and Device IDs
 * @param OMX_IndexParamDisplayProperties : Properties of the default/set Physical Display
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayMaxWidth  : Maximum display width for the display
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayMaxHeight : Maximum display height for the display
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayDataFormat : Display format set for the display
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayDitherSupport : Flag that indicates if dithering is enabled.
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayOrientation : The orientation set in the current display
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayEnabled : Flag that indicates if the current display is enabled or not
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDiplayBackLitConfig : Structure that provides the current BackLite configuration
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayVendorName : Parameter that returns the display vendor name
 *                                   - needs a valid device ID.
 * @param OMX_IndexParamDisplayIsActive : Flag that returns if a Display is active now - means that a physical display is attached
 *                                 to the current plane.
 *                                   - needs a valid device ID.
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] nParamIndex
 *     Index of the structure to be filled.  This value is from the
 *     OMX_TIIMAGE_INDEXTYPE enumeration above.
 * @param [in,out] ComponentParameterStructure
 *     Pointer to application allocated structure to be filled by the
 *     component. The structure needs to be puluated with the device ID as the first parameter
 *     for device specific indices.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent,
        OMX_TIIMAGE_INDEXTYPE nParamIndex,
        OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
#ifdef __OMX_ENABLE_PARAMGETSET__
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    OMX_U32 nDeviceID;
    OMX_SSL_CMDTYPE tCMD;

    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;

    /* Check if the input params are valid */
    OMX_SSL_EXIT_IF((NULL == pComponent));
    OMX_SSL_EXIT_IF((NULL == nParamIndex));
    OMX_SSL_EXIT_IF((NULL == ComponentParameterStructure));

    //Dereference the display ID if available
    nDeviceID = *(OMX_U32*)(ComponentParameterStructure);

    /* Check if the component is in Loaded State; else exit */
    OMX_SSL_EXIT_IF_IS((pSSLPvt->tCurState != OMX_StateLoaded));

    nSemCaller  = pSSLPvt->nSemCaller;
    nSemHandle  = pSSLPvt->nSemHandle;

    switch(nParamIndex){
        case  OMX_IndexParamNumAvailableDisplays:
            {
                //Send message to the SSL Core to get the number of Physical Displays
                tCMD = OMX_SSL_CMD_GETNUMDISPLAYS;
            }
            break;
        case OMX_IndexParamDisplayInfo:
            {
                //Send message to the SSL Core to get info on displays
                tCMD = OMX_SSL_CMD_GETDISPINFO;
            }
        case OMX_IndexParamDisplayProperties:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = OMX_SSL_CMD_GETDISPPROPERTIES;
            }
            break;
        case OMX_IndexParamDisplayMaxWidth:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = OMX_SSL_CMD_GETDISPMAXWIDTH;
            }
            break;
        case OMX_IndexParamDisplayMaxHeight:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD =  OMX_SSL_CMD_GETDISPMAXHEIGHT;
            }
            break;
        case OMX_IndexParamDisplayDataFormat:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = OMX_SSL_CMD_GETDISPDATAFORMAT;
            }
            break;
        case OMX_IndexParamDisplayDitherSupport:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = OMX_SSL_CMD_GETDISPDITHSUPPORT;
            }
            break;
        case OMX_IndexParamDisplayOrientation:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD =  OMX_SSL_CMD_GETDISPORIENTAION;
            }
            break;
        case OMX_IndexParamDisplayEnabled:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD =  OMX_SSL_CMD_GETDISPPOWERSTATUS;
            }
            break;
        case OMX_IndexParamDiplayBackLitConfig:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = OMX_SSL_CMD_GETDISPBACKLITCONFIG;
            }
            break;
        case OMX_IndexParamDisplayVendorName:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = OMX_SSL_CMD_GETDISPVENDOR;
            }
            break;
        case OMX_IndexParamDisplayIsActive:
            {
                OMX_SSL_EXIT_IF((NULL == nDeviceID));
                tCMD = NULL;
                if(pSSLPvt->pPlaneHandle->nActiveDispID == nDeviceID)
                    *((OMX_U32*)ComponentParameterStructure + sizeof(OMX_U32)) = TRUE;
                else
                    *((OMX_U32*)ComponentParameterStructure + sizeof(OMX_U32)) = FALSE;

            }
            break;
        default:
            /* Wrong Index Arguement */
            tCMD = NULL;
            tRetVal = OMX_ErrorUnsupportedIndex;
    }
    if(tCMD !=NULL)
    {
        __OMX_SSL_SendMsgToSSLCore(hComponent ,tCMD, ComponentParameterStructure);
        vsi_s_get(nSemCaller, nSemHandle);
        OMX_SSL_EXIT_IF_ERR((pSSLPvt->tOMX_SSLErrStatus != OMX_ErrorNone));
    }
#endif//__OMX_ENABLE_PARAMGETSET__
EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_SetParameter method will send a initialization parameter
 * structure to a component.  Each structure must be sent one at a time,
 * each in a separate invocation of the method.  This method can only
 * be invoked when the component is in the OMX_LoadedState state.
 * Since the parameters are Display specific, indices also need the device ID.
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] nIndex
 *     Index of the structure to be sent.  This value is from the
 *     OMX_TIIMAGE_INDEXTYPE enumeration above.
 * @param [in] ComponentParameterStructure
 *     pointer to application allocated structure to be used for
 *     initialization by the component.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 * Indices Supported for SSL with this function are:
 *
 * @param OMX_IndexParamDisplayProperties : Sets the parameters for a Physical Display
 *                                   - needs a device ID
 * @param OMX_IndexParamDisplayDataFormat : Sets the display format for a Physical Display
 *                                   - needs a device ID
 * @param OMX_IndexParamDisplayDitherSupport : Enables/Disables Dithering for a Physical Display
 *                                   - needs a device ID
 * @param OMX_IndexParamDisplayOrientation : Sets the orientation for a Physical Display
 *                                   - needs a device ID
 * @param OMX_IndexParamDisplayEnabled : Switches ON/OFF  a Physical Display
 *                                   - needs a device ID
 * @param OMX_IndexParamDiplayBackLitConfig : Sets the backlit configuration for a physical display
 *                                   - needs a device ID
 * @param OMX_IndexParamSSLPlaneDSAPlane : Flag that sets the plane as a DSA Plane
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent,
        OMX_TIIMAGE_INDEXTYPE nIndex,
        OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
#ifdef __OMX_ENABLE_PARAMGETSET__
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    OMX_SSL_CMDTYPE tCMD;
    OMX_U32 nDeviceID;

    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;

    /* Check if the input params are valid */
    OMX_SSL_EXIT_IF((NULL==hComponent));
    OMX_SSL_EXIT_IF((NULL==nIndex));
    OMX_SSL_EXIT_IF((NULL==ComponentParameterStructure));

    nDeviceID = *(OMX_U32 *)(ComponentParameterStructure);

    /* Check if the component is in Loaded State; else exit */
    OMX_SSL_EXIT_IF_IS((pSSLPvt->tCurState != OMX_StateLoaded));

    nSemCaller  = pSSLPvt->nSemCaller;
    nSemHandle = pSSLPvt->nSemHandle;

    switch(nIndex){
        case OMX_IndexParamDisplayProperties:
            {
                OMX_SSL_EXIT_IF(nDeviceID == NULL);
                tCMD = OMX_SSL_CMD_SETDISPPROPERTIES;
            }
            break;
        case OMX_IndexParamDisplayDataFormat:
            {
                OMX_SSL_EXIT_IF(nDeviceID == NULL);
                tCMD = OMX_SSL_CMD_SETDISPDATAFORMAT;
            }
            break;
        case OMX_IndexParamDisplayDitherSupport:
            {
                OMX_SSL_EXIT_IF(nDeviceID == NULL);
                tCMD = OMX_SSL_CMD_SETDISPDITHER;
            }
            break;
        case OMX_IndexParamDisplayOrientation:
            {
                OMX_SSL_EXIT_IF(nDeviceID == NULL);
                tCMD = OMX_SSL_CMD_SETDISPORIENTATION;
            }
            break;
        case OMX_IndexParamDisplayEnabled:
            {
                OMX_SSL_EXIT_IF(nDeviceID == NULL);
                tCMD = OMX_SSL_CMD_SETDISPPOWER;
            }
            break;
        case OMX_IndexParamDiplayBackLitConfig:
            {
                OMX_SSL_EXIT_IF(nDeviceID == NULL);
                tCMD = OMX_SSL_CMD_SETDISPBACKLITCONFIG;
            }
            break;
        case OMX_IndexParamSSLPlaneDSAPlane:
            {
                tCMD == OMX_SSL_CMD_SETDSAFLAG;
            }
            break;
        default:
            {
                //Invalid Index recieved.
                tRetVal = OMX_ErrorUnsupportedIndex;
                tCMD = NULL;
            }
    }



    if(tCMD != NULL)
    {
        //Send message to the SSL Core to set the display properties.
        __OMX_SSL_SendMsgToSSLCore(hComponent, tCMD, ComponentParameterStructure);
        //Wait for CallBack to Complete
        vsi_s_get(nSemCaller, nSemHandle);
        OMX_SSL_EXIT_IF_ERR((pSSLPvt->tOMX_SSLErrStatus != OMX_ErrorNone));
    }
#endif //__OMX_ENABLE_PARAMSET__
EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_GetConfig method will get one of the configuration structures
 * from a component.  This method can be invoked anytime after the
 * component has been loaded.  The nParamIndex parameter is used to
 * indicate which structure is being requested from the component.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] nIndex
 *     Index of the structure to be filled.  This value is from the
 *     OMX_TIIMAGE_INDEXTYPE enumeration above.
 * @param [in,out] ComponentConfigStructure
 *     pointer to application allocated structure to be filled by the
 *     component.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 * The GetConfig Indices supported by SSL  are:
 * @param OMX_IndexConfigSSLPlaneAlpha : Obtain the aplha set for this plane
 * @param OMX_IndexConfigSSLPlaneAlwaysOnTop : Identifies if this plane is an Always on Top plane
 * @param OMX_IndexConfigSSLPlaneActiveFlag : Indicates if this plane is active or not
 * @param OMX_IndexConfigSSLPlaneSuspendedFlag : Indicates if this plane is 'Paused' or not
 * @param OMX_IndexConfigSSLPlaneSrcPtr : Contains the source data pointer for this plane
 * @param OMX_IndexConfigSSLPlaneShadowPtr : Contains the shadow data pointer for this plane
 * @param OMX_IndexConfigSSLPlaneXOffset : X Offset for the current plane
 * @param OMX_IndexConfigSSLPlaneYOffset : Y Offset for the current plane
 * @param OMX_IndexConfigSSLPlaneXLen : Width for the current plane
 * @param OMX_IndexConfigSSLPlaneYLen : Height for the current plane
 * @param OMX_IndexConfigSSLPlaneFrameBuffPtr : FrameBuffer pointer for the current plane
 * @param OMX_IndexConfigSLLPlaneIsDSAPlane : Indicates if this plane is a DSA Plane
 * @param OMX_IndexConfigSSLPlaneConfig : Plane configuration details in a structure
 * @param OMX_IndexConfigSSLPlaneDataFrmt : Plane's data format
 * @param OMX_IndexConfigSSLPlaneActiveDispID : ID of the active display
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent,
        OMX_INDEXTYPE nIndex,
        OMX_PTR value)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_SSL_PLANE_QUERYTYPE * pPlaneHandle;

    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    pPlaneHandle = (OMX_SSL_PLANE_QUERYTYPE *)pSSLPvt->pPlaneHandle;

    /* Check if the component is in a State After the Loaded State; else exit */
    OMX_SSL_EXIT_IF_IS(!(pSSLPvt->tCurState > OMX_StateLoaded));

    switch((OMX_TIIMAGE_INDEXTYPE)nIndex){
#ifdef __OMX_ENABLE_GRAN__
        case OMX_IndexConfigSSLPlaneAlpha:
            {
                OMX_SSL_PLANE_ALPHATYPE * ptAlpha = (OMX_SSL_PLANE_ALPHATYPE *) value;
                //Retrieve plane alpha from the component private structure
                * ptAlpha = pPlaneHandle->tAlpha;
            }
            break;
        case OMX_IndexConfigSSLPlaneAlwaysOnTop:
            {
                OMX_BOOL * pbAlwaysOnTop = (OMX_BOOL *)value;
                //Retrieve the AlwaysOnTop setting from the component private structure
                * pbAlwaysOnTop = pPlaneHandle->bAlwaysOnTop;
            }
            break;
        case OMX_IndexConfigSSLPlaneActiveFlag:
            {
                OMX_BOOL * pbSSLPlaneActiveFlag = (OMX_BOOL *)value;
                //Retrieve the plane active status from the component provate structure
                * pbSSLPlaneActiveFlag = pPlaneHandle->bActive;
            }
            break;
        case OMX_IndexConfigSSLPlaneSuspendedFlag:
            {
                OMX_BOOL * pbSuspended = (OMX_BOOL *)value;
                //Retrieve the status regarding plane pausing
                * pbSuspended = pPlaneHandle->bSuspended;
            }
            break;
        case OMX_IndexConfigSSLPlaneXOffset:
            {
                OMX_U32 * pnXOffset = (OMX_U32 *)value;
                //Retrieve X Offset value for the plane
                * pnXOffset = pPlaneHandle->nXOffset;
            }
            break;
        case OMX_IndexConfigSSLPlaneYOffset:
            {
                OMX_U32 * pnYOffset = (OMX_U32 *)value;
                //Retrieve Y Offset value for the plane
                * pnYOffset = pPlaneHandle->nYOffset;
            }
            break;
        case OMX_IndexConfigSSLPlaneXLen:
            {
                OMX_U32 * pnXLen = (OMX_U32 *) value;
                //Retrieve Width of the plane
                * pnXLen = pPlaneHandle->nXLen;
            }
            break;
        case OMX_IndexConfigSSLPlaneYLen:
            {
                OMX_U32 * pnYLen = (OMX_U32 *)value;
                //Retrieve Height of the plane
                * pnYLen = pPlaneHandle->nYLen;
            }
            break;
        case OMX_IndexConfigSSLPlaneFrameBuffPtr:
            {
                OMX_PTR *pSSLDSAFramePtr = (OMX_PTR *)value;
                //Retrieve FrameBuffer Pointer
                if(pPlaneHandle->bDSAPlane == TRUE)
                    *pSSLDSAFramePtr = (OMX_PTR)pPlaneHandle->pSSLDSAFramePtr;
                else
                    value = NULL;
            }
            break;
        case OMX_IndexConfigSLLPlaneIsDSAPlane:
            {
                OMX_BOOL * pbDSAPlane = (OMX_BOOL *)value;
                //Retrieve the DSA Flag
                * pbDSAPlane = pPlaneHandle->bDSAPlane;
            }
            break;
        case OMX_IndexConfigSSLPlaneDataFrmt:
            {
                OMX_TICOLOR_FORMATTYPE * pPlaneImgFormat = (OMX_TICOLOR_FORMATTYPE *) value;
                //Retrieve the Plane's data format
                * pPlaneImgFormat = pPlaneHandle->tPlaneImgFormat;
            }
            break;
#endif //__OMX_ENABLE_GRAN__
        case OMX_IndexConfigSSLPlaneFrameBuffPtr:
            {
                OMX_PTR *pSSLDSAFramePtr = (OMX_PTR *)value;
                //Retrieve FrameBuffer Pointer
                if(pPlaneHandle->bDSAPlane == TRUE)
                    *pSSLDSAFramePtr = (OMX_PTR)pPlaneHandle->pSSLDSAFramePtr;
                else
                    value = NULL;

            }
            break;
        case OMX_IndexConfigSSLPlaneActiveDispID:
            {
                OMX_U32 * pnActiveDisp = (OMX_U32 *)value;
                //Retrieve the Plane's data format
                * pnActiveDisp = pPlaneHandle->nActiveDispID;
            }
            break;
        case OMX_IndexConfigSSLPlaneConfig:
            {
                //Populate the plane Query Structure with component private parameters
                OMX_SSL_PLANE_QUERYTYPE * pPlaneQuery = (OMX_SSL_PLANE_QUERYTYPE *)value;
                pPlaneQuery->tAlpha = pPlaneHandle->tAlpha;//PlaneAlpha
                pPlaneQuery->bActive = pPlaneHandle->bActive;//Active Flag
                pPlaneQuery->bAlwaysOnTop = pPlaneHandle->bAlwaysOnTop;//Always on Top flag
                pPlaneQuery->bDSAPlane = pPlaneHandle->bDSAPlane;//DSA Plane Flag
                pPlaneQuery->bSuspended = pPlaneHandle->bSuspended;// Suspended Flag
                pPlaneQuery->pSSLDSAFramePtr = pPlaneHandle->pSSLDSAFramePtr; //Frame Buffer Pointer
                pPlaneQuery->nXLen = pPlaneHandle->nXLen; //X Length
                pPlaneQuery->nXOffset = pPlaneHandle->nXOffset;//X Offset
                pPlaneQuery->nYLen = pPlaneHandle->nYLen;//Height of plane
                pPlaneQuery->nYOffset = pPlaneHandle->nYOffset;//Y Offset
                pPlaneQuery->tPlaneImgFormat = pPlaneHandle->tPlaneImgFormat;//Plane Data Format
                pPlaneQuery->nActiveDispID = pPlaneHandle->nActiveDispID;//Plane's Active Disp ID
                pPlaneQuery->tPlaneBlendType = pPlaneHandle->tPlaneBlendType;//Blend Type
                pPlaneQuery->unPlaneAlpha = pPlaneHandle->unPlaneAlpha;//Alpha value
                pPlaneQuery->unPlaneTransparency = pPlaneHandle->unPlaneTransparency;//Transparency value
            }
            break;
        default:
            tRetVal = OMX_ErrorUnsupportedIndex;
    }

EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_SetConfig method will send one of the configuration
 * structures to a component.  Each structure must be sent one at a
 * time, each in a separate invocation of the method.  This method can
 * be invoked anytime after the component has been loaded.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] nIndex
 *     Index of the structure to be sent.  This value is from the
 *     OMX_TIIMAGE_INDEXTYPE enumeration above.
 * @param [in] ComponentConfigStructure
 *     pointer to application allocated structure to be used for
 *     initialization by the component.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 * The list of valid indices are their significance are as follows:
 * @param OMX_IndexConfigSSLPlaneAlpha : Used to set the plane's alpha value
 * @param OMX_IndexConfigSSLPlaneAlwaysOnTop : Used to set the plane as an Always on Top plane
 * @param OMX_IndexConfigSSLPlaneXOffset : Used to set the X Offset for the plane
 * @param OMX_IndexConfigSSLPlaneYOffset : Used to set the Y Offset for the plane
 * @param OMX_IndexConfigSSLPlaneXLen : Used to set the width of the plane
 * @param OMX_IndexConfigSSLPlaneYLen : Used to set the height of the plane
 * @param OMX_IndexConfigSSLPlaneDataFrmt  : Value that sets the data format for the plane
 * @param OMX_IndexConfigSSLPlaneActiveDispID: ID of the display that is active for the current plane
 **** CURRENTLY THE ONLY SUPPORTED INDEX IS THE PLANE CONFIG INDEX   ******
 * @param OMX_IndexConfigSSLPlaneConfig : Structure used to set the plane Configuration
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent,
        OMX_INDEXTYPE nIndex,
        OMX_PTR value)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    OMX_STATETYPE tState;
    OMX_SSL_PLANE_QUERYTYPE * pPlaneHandle;
    OMX_SSL_PLANE_CONFIGTYPE tPlaneData;

    //Check for NULL Pointer input!
    if(value == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    //Check for NULL Pointer input!
    if(hComponent == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    pPlaneHandle = pSSLPvt->pPlaneHandle;

    /* Check if the component is in a State at or After the Loaded State; else exit */
    OMX_SSL_EXIT_IF_IS((pSSLPvt->tCurState < OMX_StateLoaded));

    tState = pSSLPvt->tCurState;
    nSemCaller  = pSSLPvt->nSemCaller;
    nSemHandle = pSSLPvt->nSemHandle;

    switch((OMX_TIIMAGE_INDEXTYPE)nIndex){
#ifdef __OMX_ENABLE_GRAN__
        case OMX_IndexConfigSSLPlaneAlpha:
            {
                tPlaneData.tAlpha = (OMX_SSL_PLANE_ALPHATYPE)value;
            }
            break;
        case OMX_IndexConfigSSLPlaneAlwaysOnTop:
            {
                tPlaneData.bAlwaysOnTop= (OMX_BOOL)value;
            }
            break;
        case OMX_IndexConfigSSLPlaneXOffset:
            {
                tPlaneData.nXOffset= (OMX_U32)value;
            }
            break;
        case OMX_IndexConfigSSLPlaneYOffset:
            {
                tPlaneData.nYOffset = (OMX_U32)value;
            }
            break;
        case OMX_IndexConfigSSLPlaneXLen:
            {
                tPlaneData.nXLen= (OMX_U32)value;
            }
            break;
        case OMX_IndexConfigSSLPlaneYLen:
            {
                tPlaneData.nYLen = (OMX_U32) value;
            }
            break;
        case OMX_IndexConfigSSLPlaneDataFrmt:
            {
                tPlaneData.tPlaneImgFormat = (OMX_TICOLOR_FORMATTYPE)value;
            }
            break;
        case OMX_IndexConfigSSLPlaneActiveDispID:
            {
                tPlaneData.nActiveDispID = (OMX_U32)value;
            }
            break;
#endif //__OMX_ENABLE_GRAN__
            // Set the configuration parameter for the plane as a whole.
        case OMX_IndexConfigSSLPlaneConfig:
            {
                OMX_SSL_PLANE_CONFIGTYPE * ptConfig  = (OMX_SSL_PLANE_CONFIGTYPE *)value;
                tPlaneData.tAlpha =  ptConfig->tAlpha;
                tPlaneData.bAlwaysOnTop = ptConfig->bAlwaysOnTop;
                tPlaneData.bDSAPlane = ptConfig->bDSAPlane;
                tPlaneData.nXLen= ptConfig->nXLen;
                tPlaneData.nXOffset = ptConfig->nXOffset;
                tPlaneData.nYLen = ptConfig->nYLen;
                tPlaneData.nYOffset= ptConfig->nYOffset;
                tPlaneData.tPlaneImgFormat = ptConfig->tPlaneImgFormat;
                tPlaneData.nActiveDispID = ptConfig->nActiveDispID;
                tPlaneData.tPlaneBlendType = ptConfig->tPlaneBlendType;
                tPlaneData.unPlaneAlpha = ptConfig->unPlaneAlpha;
                tPlaneData.unPlaneTransparency = ptConfig->unPlaneTransparency;
                tPlaneData.bDeferredUpdate = ptConfig->bDeferredUpdate;
            }
            break;
        default:
            //Invalid Index recieved
            tRetVal = OMX_ErrorUnsupportedIndex;
    }

    if(tState > OMX_StateLoaded)
    {

        //Update the Plane's Configuration Parameter Structure
        __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_SETPLANEPROPERTIES, &tPlaneData);
        //Wait for confirmation from the SSL Core
        vsi_s_get(nSemCaller, nSemHandle);
        OMX_SSL_EXIT_IF_ERR((pSSLPvt->tOMX_SSLErrStatus != OMX_ErrorNone));
    }
    pPlaneHandle->tAlpha =  tPlaneData.tAlpha;
    pPlaneHandle->bAlwaysOnTop = tPlaneData.bAlwaysOnTop;
    pPlaneHandle->bDSAPlane = tPlaneData.bDSAPlane;
    pPlaneHandle->nXLen= tPlaneData.nXLen;
    pPlaneHandle->nXOffset = tPlaneData.nXOffset;
    pPlaneHandle->nYLen = tPlaneData.nYLen;
    pPlaneHandle->nYOffset= tPlaneData.nYOffset;
    pPlaneHandle->tPlaneImgFormat = tPlaneData.tPlaneImgFormat;
    pPlaneHandle->nActiveDispID = tPlaneData.nActiveDispID;
    pPlaneHandle->tPlaneBlendType = tPlaneData.tPlaneBlendType;
    pPlaneHandle->unPlaneAlpha = tPlaneData.unPlaneAlpha;
    pPlaneHandle->unPlaneTransparency = tPlaneData.unPlaneTransparency;
    pPlaneHandle->bDeferredUpdate = tPlaneData.bDeferredUpdate;
EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_GetState method will return the current state of the
 * component and place the state value into the location pointed
 * to by pState.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [out] pState
 *     pointer to the location to receive the state.  The value returned
 *     is one of the OMX_STATETYPE members
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComponent,
        OMX_STATETYPE* pState)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_SSL_COMPONENT_PRIVATETYPE* pPrivateStruct;

    /* Check for valid component handle */
    if ((NULL == hComponent) || (NULL == pState)) {
        tRetVal = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pPrivateStruct = (OMX_SSL_COMPONENT_PRIVATETYPE *)pHandle->pComponentPrivate;
    /* set the present state of the component */
    *pState = pPrivateStruct->tCurState;
EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn ComponentTunnelRequest method will interact with another OMX
 * component to determine if tunneling is possible and to setup the
 * tunneling if it is possible.  The return codes for this method
 * can be used to determine of tunneling is not possible
 * or if deep or shallow tunneling is used.
 *
 * @param [in] hInput
 *     Handle of the component to be accessed.
 * @param [in] nPortInput
 *     nPortIndex1 is used to select the port on component 1 to be
 *     used in the tunnel.  The port can be found by using the nPortIndex1
 *           value as an index into the Port Definition array of component 1.
 * @param [in] hOutput
 *     This is the component to setup the tunnel with.  In addition,
 *     this is the handle of the component that will be used when the
 *     nPortIndex2 parameter is used to determine which of the component's
 *     ports to tunnel.  This is the component handle returned by the call
 *     to the GetHandle function.
 * @param [in] nPortOutput
 *     nPortIndex2 is used to select the port on component 2 to be
 *     used in the tunnel.  The port can be found by using the nPortIndex2
 *     value as an index into the Port Definition array of component 2.
 * @param [in] eDir direction of the call.
 * @param [in] pCallbacks
 *     This is a pointer to the callback structure of the caller (used for
 *     shallow data tunneling).
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE ComponentTunnelRequest( OMX_HANDLETYPE hComp,
        OMX_U32 nPort,
        OMX_HANDLETYPE hTunneledComp,
        OMX_U32 nTunneledPort,
        OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNotImplemented;
    /* This function is not implemented in the current release */
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn EmptyThisBuffer method will send a buffer full of data to an
 * input port of the component.  The buffer will be emptied by the
 * component and returned to the application via the EmptyBufferDone
 * call back.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] nPortIndex
 *     nPortIndex1 is used to select the port on the component to be
 *     used.  The port can be found by using the nPortIndex
 *     value as an index into the Port Definition array of the component.
 * @param [in] pBuffer
 *     pointer to an OMX_BUFFERHEADERTYPE structure used to provide or
 *     receive the pointer to the buffer header.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 * The SSL Client, passes on the input buffer to the SSL Core though a message.
 */
/* ========================================================================== */
static OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent,
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt;
    OMX_SSL_PLANE_QUERYTYPE *pPlaneHandle;
    OMX_U8 nBitsPerPixel;
    OMX_COMPONENTTYPE * pComponent = (OMX_COMPONENTTYPE *) hComponent;

    //Check for NULL Pointer input buffer
    if(pBuffer->pBuffer == NULL)
    {
        tRetVal = OMX_ErrorBadParameter;
        return tRetVal;
    }

    //Check for NULL Pointer input!
    if(hComponent == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *) pComponent->pComponentPrivate;
    if (pSSLPvt->tCurState > OMX_StateLoaded)
    {
        pPlaneHandle = pSSLPvt->pPlaneHandle;

        nBitsPerPixel = __OMX_SSL_COMPUTEBITSPERPIXEL(pPlaneHandle->tPlaneImgFormat);

        __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_SETINBUFF, pBuffer);
    }
    else
    {
        return OMX_ErrorInvalidState;
    }
EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_FillThisBuffer method will send an empty buffer to an
 * output port of a component.  The buffer will be filled by the
 * component and returned to the application via the FillBufferDone
 * call back.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] nPortIndex
 *     nPortIndex1 is used to select the port on the component to be
 *     used.  The port can be found by using the nPortIndex
 *     value as an index into the Port Definition array of the component.
 * @param [in] pBuffer
 *     pointer to an OMX_BUFFERHEADERTYPE structure used to provide or
 *     receive the pointer to the buffer header.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *           OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComponent,
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNotImplemented;

    // This routine is not implemented for this component. THis is a sink component and there is no output buffer.
    return tRetVal ;
}

/* ========================================================================== */
/** @fn SetCallbacks method will allow the core to transfer the callback
 * structure from the application to the component.  This is a blocking
 * call.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 * @param [in] pCallbacks
 *     pointer to an OMX_CALLBACKTYPE structure used to provide the
 *     callback information to the component
 * @param [in] pAppData
 *     pointer to an application defined value.  It is anticipated that
 *     the application will pass a pointer to a data structure or a "this
 *     pointer" in this area to allow the callback (in the application)
 *     to determine the context of the call
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComponent,
        OMX_CALLBACKTYPE* pCallbacks,
        OMX_PTR pAppData)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle;
    OMX_SSL_COMPONENT_PRIVATETYPE *pCompPvt;

    /* Check the input parameters. */
    OMX_SSL_EXIT_IF (hComponent == NULL);
    OMX_SSL_EXIT_IF (pCallbacks == NULL);
    OMX_SSL_EXIT_IF (pCallbacks->EmptyBufferDone == NULL);

    pHandle = (OMX_COMPONENTTYPE*)hComponent;
    pCompPvt = (OMX_SSL_COMPONENT_PRIVATETYPE*)pHandle->pComponentPrivate;

    /* Copy the callbacks of the application to the component private. */
    memcpy(&(pCompPvt->cbInfo), pCallbacks, sizeof(OMX_CALLBACKTYPE));

    /* Copy the application private data to component memory. */
    pHandle->pApplicationPrivate = pAppData;

    /* Set the current State to Loaded. */
    pCompPvt->tCurState = OMX_StateLoaded;
EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/** @fn ComponentDeInit method is used to deinitialize the component
 * providing a means to free any resources allocated at component
 * initialization.  NOTE:  After this call the component handle is
 * not valid for further use.
 *
 * @param [in] hComponent
 *     Handle of the component to be accessed.  This is the component
 *     handle returned by the call to the GetHandle function.
 *
 * @return OMX_ERRORTYPE
 *     If the command successfully executes, the return code will be
 *     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_SSL_COMPONENT_PRIVATETYPE* pPrivateStruct;

    /* Check for valid component handle */
    if (NULL == hComponent) {
        tRetVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pPrivateStruct = (OMX_SSL_COMPONENT_PRIVATETYPE*)pHandle->pComponentPrivate;

    //Cannot DeInit if the component is not in the Loaded state
    //This will cause Orphan resources
    if (OMX_StateLoaded != pPrivateStruct->tCurState) {
        tRetVal = OMX_ErrorInvalidState;
        goto EXIT;
    }

    /* Free the semaphore */
    vsi_s_close(pPrivateStruct->nSemCaller, pPrivateStruct->nSemHandle);
	os_DestroySemaphore(pPrivateStruct->nSemCaller, pPrivateStruct->nSemHandle);
    /* Free the plane query structure memory in the component private structure */
    OMX_FREE(pPrivateStruct->pPlaneHandle);
    pPrivateStruct->pPlaneHandle = NULL;
    /* free the teh component private structure */
    OMX_FREE(pHandle->pComponentPrivate);
    pHandle->pComponentPrivate = NULL;

EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_SSL_ComponentInit method is the first call into a component
 * and is used to perform any one time initialization specific to a component.
 * The component MUST fill in all function pointers into the handle.  The
 * component may assume that the application has allocated the handle and
 * filled in the SIZE and VERSION fields. These should be checked by the
 * component.  The component should also allocate and fill in the component
 * private data structure, but do no other allocations of memory or
 * resources.  Any software or hardware resource or capabilities detection
 * should be performed. If the required hardware is not detected, this
 * method MUST return OMX_Error_Hardware to inform the application that
 * further processing is not possible. In the case of an error, the
 * component private data structure must be deallocated.
 *
 * @param [in] hComponent
 *      Handle of the component to be accessed.  This is the component
 *       handle returned by the call to the GetHandle function.
 *
 * @return OMX_ERRORTYPE
 *       If the command successfully executes, the return code will be
 *       OMX_NoError.  Otherwise the appropriate OMX error will be returned.
 *
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
OMX_ERRORTYPE OMX_SSL_ComponentInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
	  OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_SSL_COMPONENT_PRIVATETYPE* pPrivateStruct;
    OMX_U8 count =0;
    OMX_U8 cSemName[20];
    T_HANDLE entityHandle;

    /* Check for valid component handle */
    if (NULL == hComponent) {
        tRetVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    entityHandle = e_running[os_MyHandle()];

    /* Check size field of the component structure */
    if (sizeof(OMX_COMPONENTTYPE) != pHandle->nSize) {
        tRetVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    /* Check version field of the component structure */
    if (pHandle->nVersion.nVersion < SupportedOMXVersion.nVersion) {
        tRetVal = OMX_ErrorVersionMismatch;
        goto EXIT;
    }

    /* if need any private data, allocate here  */
    pHandle->pComponentPrivate = NULL;
    pHandle->pComponentPrivate = (OMX_SSL_COMPONENT_PRIVATETYPE*)OMX_ALLOC(sizeof(OMX_SSL_COMPONENT_PRIVATETYPE));

    if (NULL == pHandle->pComponentPrivate) {
        tRetVal = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pPrivateStruct = (OMX_SSL_COMPONENT_PRIVATETYPE*)pHandle->pComponentPrivate;
    pPrivateStruct->pPlaneHandle = (OMX_SSL_PLANE_QUERYTYPE *)OMX_ALLOC(sizeof(OMX_SSL_PLANE_QUERYTYPE));

    if (NULL == pPrivateStruct->pPlaneHandle) {
        tRetVal = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
   /* Create a semaphore resource for Client-Core communication */
    ltoa((OMX_U32)hComponent, (OMX_STRING)&cSemName[0]);
    pPrivateStruct->nSemHandle = vsi_s_open(entityHandle, (OMX_STRING)cSemName, count);
    pPrivateStruct->nSemCaller = (OMX_U32) entityHandle;

    /* Get the SSL Core handle and populate the structure for future use */
    /* Get the current Task Handle*/
    pPrivateStruct->tSSLClientHandle = e_running[os_MyHandle()];
    pPrivateStruct->pSSLCoreHandle = vsi_e_handle(pPrivateStruct->nSemCaller, "SSL");

    /* filling the function pointers */
    pHandle->GetComponentVersion = GetComponentVersion;
    pHandle->SendCommand = SendCommand;
    pHandle->GetParameter = GetParameter;
    pHandle->SetParameter = SetParameter;
    pHandle->GetConfig = GetConfig;
    pHandle->SetConfig = SetConfig;
    pHandle->GetState = GetState;
    pHandle->ComponentTunnelRequest = ComponentTunnelRequest;
    pHandle->EmptyThisBuffer = EmptyThisBuffer;
    pHandle->FillThisBuffer = FillThisBuffer;
    pHandle->SetCallbacks = SetCallbacks;
    pHandle->ComponentDeInit = ComponentDeInit;

    /* component state is set to loaded */
    pPrivateStruct->tCurState = OMX_StateLoaded;

EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
 * HandleCommand() This function handles the command sent by SendCommand().  It
 * decides which course of action to take based on the previous state and the
 * requested state.  It is called from within SendCommand().  The parameters
 * hComp and newState are the handle to the component and the state that is to
 * be set, respectively.
 *
 * @param hComponent Handle to the component.
 *
 * @param newState   The new state that is to be set.
 *
 * @pre              component has been loaded
 *
 * @return           OMX_ErrorNone = Successful handle command.
 *                   OMX_ErrorInvalidState = improper state transition.
 *                   OMX_ErrorBadParameter = parameter is not valid to transition
 *
 * @see              SendCommand()
 */
/* ========================================================================== */

OMX_ERRORTYPE __OMX_SSL_HandleCommand (OMX_HANDLETYPE hComponent,
        OMX_U32 newState)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_STATETYPE stateBeforeCmd;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE*)hComponent;
    OMX_SSL_COMPONENT_PRIVATETYPE *pCompPvt;
    OMX_SSL_PLANE_QUERYTYPE * pPlaneHandle;
    OMX_SSL_PLANE_CONFIGTYPE * pPlaneData;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    OMX_BOOL bImmediateCb = OMX_FALSE;
    pCompPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pHandle->pComponentPrivate;
    pPlaneHandle = (OMX_SSL_PLANE_QUERYTYPE *) pCompPvt->pPlaneHandle;


    stateBeforeCmd = pCompPvt->tCurState;

    nSemCaller  = pCompPvt->nSemCaller;
    nSemHandle = pCompPvt->nSemHandle;

    switch (stateBeforeCmd) {
        case OMX_StateLoaded:
            /*
             * From the loaded state, the component can move to IDLE or be unloaded. Other
             * transitions are invalid.
             */
            if(newState == OMX_StateIdle)
            {
                /*
                 * Command recieved to move to idle state. Before we can move on to the IDLE state all resources
                 * need to have been allocated. The component private structure would have already been allocated.
                 * We need to create the plane resource at the SSL Core task. For this the plane configuration needs
                 * to have been done. Also, the display properties need to have been allocated.
                 * If not, we cannot move to the IDLE state. The same would happen even if the Core is not able to
                 * allocate resources.
                 */
                //Check if all parameters needed to create a plane are set.
                /*
                 *  Parameters essential to plane creation are: Width, Height, X Offset, Y Offset, Alpha (can be zero),
                 *  DSA Flag (can be zero), Always on Top flag (can be zero).
                 */
                OMX_SSL_EXIT_IF(pPlaneHandle->nXLen == 0);//Width
                OMX_SSL_EXIT_IF(pPlaneHandle->nYLen ==0 );//Height

                pPlaneData = (OMX_SSL_PLANE_CONFIGTYPE *)OMX_ALLOC(sizeof(OMX_SSL_PLANE_CONFIGTYPE));
                if(!pPlaneData)
                {
                    return OMX_ErrorInsufficientResources;
                }
                pPlaneData->bAlwaysOnTop = pPlaneHandle->bAlwaysOnTop;
                pPlaneData->bDSAPlane = pPlaneHandle->bDSAPlane;
                pPlaneData->nActiveDispID = 0;
                pPlaneData->nXLen = pPlaneHandle->nXLen;
                pPlaneData->nYLen = pPlaneHandle->nYLen;
                pPlaneData->nXOffset =pPlaneHandle->nXOffset;
                pPlaneData->nYOffset =pPlaneHandle->nYOffset;
                pPlaneData->tAlpha = pPlaneHandle->tAlpha;
                pPlaneData->tPlaneBlendType = pPlaneHandle->tPlaneBlendType;
                pPlaneData->unPlaneAlpha = pPlaneHandle->unPlaneAlpha;
                pPlaneData->unPlaneTransparency = pPlaneHandle->unPlaneTransparency;
                pPlaneData->tPlaneImgFormat = pPlaneHandle->tPlaneImgFormat;
                pPlaneData->bDeferredUpdate = pPlaneHandle->bDeferredUpdate;

                /*
                 * Since all resources/resource configurations are ready from the SSL Client side, send a message
                 * to the SSL Core to create the plane with the configuration set.
                 * Note that all the parameters required have already been sent to the SSL Core task.
                 */
                //Set the state to IDLE
                pCompPvt->tCurState= (OMX_STATETYPE)newState;

                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_CREATEPLANE,pPlaneData);
                break;
            }
            if(newState == OMX_StatePause)
            {
                /*
                 * The application has issued a command to move the SSL Client from the Loaded state
                 * to the Pause state. In pause state, the currrent plane is to be suspended to omit it from the
                 * display refresh chain, but since the component is not yet in IDLE, the plane as such has not
                 * been created! Hence, there is nothing to be done here. This could be an invalud call and the
                 * component returns an Invalid State Error.
                 */
                bImmediateCb = OMX_TRUE;
                OMX_SSL_EXIT_IF_IS(1);
            }
            if(newState == OMX_StateExecuting)
            {
                /*
                 * The application is trying to execute without any resource allocation. Return Invalid State error.
                 */
                bImmediateCb = OMX_TRUE;
                OMX_SSL_EXIT_IF_IS(1);
            }
            if(newState == OMX_StateInvalid)
            {
                /*
                 * Application is trying to move the component to the Invalid State from the Loaded state.
                 * This is component De Initalization. The application needs to do this only through the
                 * Component de-init.
                 */
                bImmediateCb = OMX_TRUE;
                OMX_SSL_EXIT_IF_IS(1);
            }
            if(newState == OMX_StateLoaded)
            {
                //loaded to loaded transition is allowed. Note: immediate callback
                pCompPvt->tCurState = (OMX_STATETYPE)newState;

                pCompPvt->cbInfo.EventHandler(hComponent,
                        pHandle->pApplicationPrivate,
                        OMX_EventCmdComplete,
                        OMX_CommandStateSet,
                        pCompPvt->tCurState,
                        NULL);
            }
            break;
        case OMX_StateIdle:
            /*
             * In the IDLE State, all resources for the components have been allocated.
             * The transitions permitted in this state are into: 1) Executing State 2) Pause 3) Loaded
             * Transitioning to execute state involves activating the plane, This means the SSL Core moves it
             * into the scan queue for display. Transitioning to Pause means that the plane is to be suspended.
             * This excludes it from the scan but retains it in the queue.
             */
            if(newState == OMX_StateExecuting)
            {
                /*
                 * The application has moved the component to executing state. This means that it has activated
                 * the plane. The client sets the plane's active status flag and sends the configuration command
                 * to the SSL Core task.
                 */
                pCompPvt->tCurState= (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_ACTIVATEPLANE, NULL);

                break;
            }
            if(newState == OMX_StatePause)
            {
                /*
                 * The application has moved the component to the pause state. This means that it is excluded from
                 * the display scan.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_SUSPENDPLANE, NULL);

                break;
            }
            if(newState == OMX_StateLoaded)
            {
                /*
                 * The application has moved the component to the Loaded State. This is a call to destroy plane
                 * actually. ONCE THIS IS DONE. RECONFIGURATION MAY BE NEEDED TO CREATE THE NEW PLANE.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_DESTROYPLANE, NULL);
                break;
            }
            /*
             * Any other state should be categorized as Invalid. The component returns an Error for this.
             */
            bImmediateCb = OMX_TRUE;
            OMX_SSL_EXIT_IF_IS(1);
        case OMX_StateExecuting:
            /*
             * The component is in the execute state. This means that, the plane is active, and responds to any
             * change in input data or any configuration setting. From the execute state, the component can be
             * moved to the pause, idle or the loaded state. The response to a command to move to loaded
             * state is identical to the case when the component is in IDLE state.
             */
            if(newState == OMX_StateIdle)
            {
                /*
                 * When moved to IDLE State, the plane is deactivated. This means that, the SSL Core removes
                 * it from the active queues. It doesn;t however destroy the plane. The plane is still sensitive to input data
                 * and configuration changes. NOTE: DEACTIVATING A PLANE CAUSES THE PLANE TO LOSE ITS POSITION
                 * in the active queue. For this not to happen, we must choose to PAUSE the plane instead.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_DEACTIVATEPLANE, NULL);

                break;
            }
            if(newState == OMX_StateLoaded)
            {
                /*
                 * The application has moved the component to the Loaded State. This is a call to destroy plane
                 * actually. ONCE THIS IS DONE. RECONFIGURATION MAY BE NEEDED TO CREATE THE NEW PLANE.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_DESTROYPLANE, NULL);

                break;
            }
            if(newState == OMX_StatePause)
            {
                /*
                 * The application has moved the component to the pause state. This means that it is excluded from
                 * the display scan.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_SUSPENDPLANE, NULL);

                break;
            }
            bImmediateCb = OMX_TRUE;
            //Any other state is Invalid here.
            OMX_SSL_EXIT_IF_IS(1);
        case OMX_StatePause:
            /*
             *   The Component is in the Pause state. From the pause state, it can move to the Executing or the IDLE
             * or the Loaded state. Any other state is consided Invalid. Moving a plane to any of these is equivalent
             * to RESUMING the plane.
             */
            if(newState == OMX_StateIdle)
            {
                /*
                 * When moved to IDLE State, the plane is deactivated. This means that, the SSL Core removes
                 * it from the active queues. It doesn't however destroy the plane. The plane is still sensitive to input data
                 * and configuration changes. NOTE: DEACTIVATING A PLANE CAUSES THE PLANE TO LOSE ITS POSITION
                 * in the active queue. For this not to happen, we must choose to PAUSE the plane instead.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_DEACTIVATEPLANE, NULL);

                break;
            }
            if(newState == OMX_StateLoaded)
            {
                /*
                 * The application has moved the component to the Loaded State. This is a call to destroy plane
                 * actually. ONCE THIS IS DONE. RECONFIGURATION MAY BE NEEDED TO CREATE THE NEW PLANE.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_DESTROYPLANE, NULL);
            }
            if(newState == OMX_StateExecuting)
            {
                /*
                 * The application is moving the plane to the executing state. This means that the plane is included
                 * in the scan for display update. This is similar to a resum operation. The plane retains its place in the
                 * active queue.
                 */
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                __OMX_SSL_SendMsgToSSLCore(hComponent,OMX_SSL_CMD_RESUMEPLANE, NULL);
                break;
            }
            bImmediateCb = OMX_TRUE;
            //Any other state transition is invalid here
            OMX_SSL_EXIT_IF_IS(1);
        default:
            bImmediateCb = OMX_TRUE;
            OMX_SSL_EXIT_IF_IS(1);
    }

EXIT:
    //Give an immediate call back if needed!
    if(bImmediateCb == OMX_TRUE)
    {
        pCompPvt->cbInfo.EventHandler(hComponent,
                pHandle->pApplicationPrivate,
                OMX_EventError,
                (OMX_U32)OMX_ErrorInvalidState,
                (OMX_U32)pCompPvt->tCurState,
                NULL);
    }
    return tRetVal;
}


/* ========================================================================== */
/**
 * __OMX_SSL_SendMsgToSSLCore()
 *
 * This function populates a message structure and sends it as a GPF message to the SSL Core task. Besides the
 * command and the arguement, it also sends the CALL BACK function to the core so that the core may convey the
 * status of the command.
 *
 * NOTE:
 * Synchronization occurs between the Core and the Client through a GPF Semaphore. This is to be released by the core
 * to acknowledge a command - the client waits for this semaphore and reads the status populated in its private structure
 * to know of any error status. For asynchronous functions, though, the client returns to the application immediately and
 * must communicate to the application by a callback provided.
 *
 * @param cmdToSSLCore command to the SSL core from the client
 *
 * @param auxDataToSSLCore Auxilary parameter to the SSL Core from the client
 *
 * @return           None
 *
 * @see              __OMX_SSL_SendMsgToSSLCore()
 */
/* ========================================================================== */
OMX_ERRORTYPE __OMX_SSL_SendMsgToSSLCore(OMX_HANDLETYPE hComponent, OMX_SSL_CMDTYPE cmdToSSLCore, OMX_PTR auxDataToSSLCore)
{
    OMX_U32 nBuffSize;
    OMX_U32 nMsgRetStatus;
    OMX_SSL_MESSAGE_STRUCTTYPE * pMSGBuffer;
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pComponent = (OMX_COMPONENTTYPE*)hComponent;
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    OS_QDATA Msg;

    /* Compute the size of the message buffer */
    nBuffSize = sizeof(OMX_SSL_MESSAGE_STRUCTTYPE);

    pMSGBuffer = (OMX_SSL_MESSAGE_STRUCTTYPE *) vsi_c_pnew (nBuffSize,0 FILE_LINE_MACRO);
    if(pMSGBuffer == NULL)
    {
        tRetVal = OMX_ErrorInsufficientResources;
        return tRetVal;
    }

    //Populate the message here with details
    pMSGBuffer->tCMD = cmdToSSLCore;//The command
    pMSGBuffer->pArgument = auxDataToSSLCore;//auxilary data
    pMSGBuffer->pComponentHandle = pComponent;//Component Handle : Sender's ID actually
    pMSGBuffer->pSSLCallback = __OMX_SSL_Callback;//Call back function

    //Send message to the SSL Core
    nMsgRetStatus = PSEND(pSSLPvt->pSSLCoreHandle, (T_VOID_STRUCT *) pMSGBuffer);
    if(nMsgRetStatus != VSI_OK)
    {
        //The communication failed.
        tRetVal = OMX_ErrorNotReady;
    }
    return tRetVal;
}

/* ========================================================================== */
/**
 * __OMX_SSL_Callback()
 *
 * This is the callback function that is called by the SSL Core for any command that is sent. It has all OUT params
 * sent to it as pointers. It calls the function with the error status and also populates a strucutre and passes to the
 * function in response to the function.
 *
 * @param hComponent Calling component's handle
 *
 * @param nErrorStatus Error status updated by the SSL Core
 *
 * @param pData For OMX_SSL_CMD_SETINBUFF call, this parameter is the buffer header data type.
 *
 * @return    None
 *
 * @see  __OMX_SSL_SendMsgToSSLCore()
 */
/* ========================================================================== */
void __OMX_SSL_Callback (OMX_HANDLETYPE hComponent,  OMX_ERRORTYPE nErrorStatus, OMX_SSL_CMDTYPE tCommand,  OMX_PTR pData )
{

    OMX_COMPONENTTYPE * pComponent = (OMX_COMPONENTTYPE *)hComponent;
    OMX_BUFFERHEADERTYPE* pBuff = (OMX_BUFFERHEADERTYPE *)pData;
    OMX_SSL_COMPONENT_PRIVATETYPE * pSSLPvt;
    OMX_STATETYPE tState;
    OMX_BOOL bEventFlag = OMX_FALSE;

    OMX_ENTRYEXITFUNC_TRACE("OMX_SSL CALLBACK");

    if(pComponent == NULL)
    {
        OMX_ENTRYEXITFUNC_TRACE("SSL CB EXCEPTION: Component Null");
        goto EXIT;
    }
    pSSLPvt = (OMX_SSL_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    if(pSSLPvt == NULL)
    {
        OMX_ENTRYEXITFUNC_TRACE("SSL CB EXCEPTION: Private Null");
        goto EXIT;
    }

    pSSLPvt->tOMX_SSLErrStatus = nErrorStatus;


    switch(tCommand)
    {
        /*
         * Check if the command is the OMX_SSL_CMD_SETINBUFF command.
         * If yes, we need to call back the EmptyBufferDone function.
         */
        case OMX_SSL_CMD_SETINBUFF:
        	if(nErrorStatus == OMX_ErrorNone)
            		pSSLPvt->cbInfo.EmptyBufferDone(hComponent, pComponent->pApplicationPrivate, pBuff);
        	else
        	      pSSLPvt->cbInfo.EventHandler(hComponent,
                		pComponent->pApplicationPrivate,
                		OMX_EventError,
                		(OMX_U32)nErrorStatus,
                		0,
                		NULL);
            break;
            /*
             * Check if the command is a set DSA flag plane with DSA set; if yes, then return Framebuffer pointer.
             */
        case OMX_SSL_CMD_SETDSAFLAG:
            if(pSSLPvt->pPlaneHandle->bDSAPlane == OMX_TRUE)
            {
                pSSLPvt->pPlaneHandle->pSSLDSAFramePtr = (OMX_BYTE)pData;
            }
            break;
        case OMX_SSL_CMD_SETPLANEPROPERTIES:
            if(pSSLPvt->tCurState>OMX_StateLoaded)
            {
                //Release the semaphore if set of course
                vsi_s_release(pSSLPvt->tSSLClientHandle, pSSLPvt->nSemHandle);
            }
            break;

        case OMX_SSL_CMD_CREATEPLANE:
            if(nErrorStatus == OMX_ErrorNone)
            {
                if( pSSLPvt->pPlaneHandle->bDSAPlane == OMX_TRUE)
                {
                    pSSLPvt->pPlaneHandle->pSSLDSAFramePtr = (OMX_BYTE)pData;
                }
                tState= OMX_StateIdle;
            }
            bEventFlag = OMX_TRUE;
            break;
        case OMX_SSL_CMD_ACTIVATEPLANE:
            if(nErrorStatus == OMX_ErrorNone)
            {
                pSSLPvt->pPlaneHandle->bActive = OMX_TRUE;
                tState= OMX_StateExecuting;
            }
            bEventFlag = OMX_TRUE;
            break;
        case OMX_SSL_CMD_SUSPENDPLANE:
            if(nErrorStatus == OMX_ErrorNone)
            {
                pSSLPvt->pPlaneHandle->bSuspended = OMX_TRUE;
                tState= OMX_StatePause;
            }
            bEventFlag = OMX_TRUE;
            break;
        case OMX_SSL_CMD_DEACTIVATEPLANE:
            if(nErrorStatus == OMX_ErrorNone)
            {
                pSSLPvt->pPlaneHandle->bActive = OMX_FALSE;
                if(pSSLPvt->tCurState == OMX_StatePause)
                {
                    pSSLPvt->pPlaneHandle->bSuspended = OMX_FALSE;
                }
                tState = OMX_StateIdle;
            }
            bEventFlag = OMX_TRUE;
            break;

        case OMX_SSL_CMD_DESTROYPLANE:
            if(nErrorStatus == OMX_ErrorNone)
            {
                pSSLPvt->pPlaneHandle->bSuspended = OMX_FALSE;
                tState = OMX_StateLoaded;
            }
            bEventFlag = OMX_TRUE;
            break;

        case OMX_SSL_CMD_RESUMEPLANE:
            if(nErrorStatus == OMX_ErrorNone)
            {

                tState = OMX_StateExecuting;
                pSSLPvt->pPlaneHandle->bSuspended = OMX_FALSE;
            }
            bEventFlag = OMX_TRUE;
            break;
        default:
            break;
    }
    if(bEventFlag == OMX_TRUE)
    {
	if(nErrorStatus == OMX_ErrorNone)
        //Call the event handler call back for the state change that happened.
        pSSLPvt->cbInfo.EventHandler(hComponent,
                pComponent->pApplicationPrivate,
                OMX_EventCmdComplete,
                (OMX_U32)OMX_ErrorNone,
                (OMX_U32)tState,
                NULL);
	else
		pSSLPvt->cbInfo.EventHandler(hComponent,
                pComponent->pApplicationPrivate,
                OMX_EventError,
                (OMX_U32)nErrorStatus,
                (OMX_U32)tState,
                NULL);
    }
EXIT:
		return;
}

/* ========================================================================== */
/**
 * __OMX_SSL_COMPUTEBITSPERPIXEL()
 *
 * This is a local function that returns the bits per pixel for a given image format
 *
 * @param tPlaneImgFormat: Image format
 *
 * @return bits per pixel for the format
 *
 * @see  __OMX_SSL_SendMsgToSSLCore()
 */
/* ========================================================================== */
OMX_U8 __OMX_SSL_COMPUTEBITSPERPIXEL(OMX_TICOLOR_FORMATTYPE tPlaneImgFormat)
{
    //At present only the RGB565 (16 Bit) format is supported
    //No computation is done, but returns 16 always!
    switch(tPlaneImgFormat)
    {
        default:
            return 16;
    }
}

