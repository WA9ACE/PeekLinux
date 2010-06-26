/* =============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file OMX_camdcomponent.c
 *
 * This file contains methods that provides functionality for the camera driver client component
 * for the Nucleus(tm) operating system on the Locosto platform.
 *
 * @path  chipsetsw\services\omx_cam\
 *
 * @rev  0.1
 *
 */
/* ------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 17-Feb-2006 jrk@ti.com: Initial Release
 *! 16-Apr-2006 jrk@ti.com: Initial Alpha Release
 *! 6-June-2006 narendranmr@ti.com : Fixed pointer assignment issue in callback
 *! routine.
 *! 10-jul-2006 narendranmr@ti.com: Removed semphore blocking during data buffer
 *! passing to camera core as this was affecting performance. 
 * ========================================================================= */


#if OMX_CAM_STATE

/****************************************************************
 *  INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <string.h>
#include <stdlib.h>
#include <string.h>

/*-------program files ----------------------------------------*/
#include "OMX_Core.h"
#include "OMX_camdcomponent.h"
#include "OMX_camd_Private.h"

/* Note: This component is OMX 1.0 compliant */
static const OMX_VERSIONTYPE SupportedOMXVersion = {0x01,0x00, 0x00, 0x00};
static    OMX_HANDLETYPE hComponentGS;
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
    OMX_ERRORTYPE tRetVal;
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvtStructure;
    OMX_COMPONENTTYPE * pCAMHandle;

    OMX_CAM_EXIT_IF((NULL == hComponent));
    OMX_CAM_EXIT_IF((NULL == pComponentVersion));
    OMX_CAM_EXIT_IF((NULL == pSpecVersion));    

    pCAMHandle = (OMX_COMPONENTTYPE *)hComponent;
    pCAMPvtStructure = (OMX_CAM_COMPONENT_PRIVATETYPE *)pCAMHandle->pComponentPrivate;

    pComponentName    = OMXCAM_COMP_NAME;
    *(OMX_U32 *)pComponentVersion = pCAMPvtStructure->nCompVersion.nVersion;
    *(OMX_U32 *)pSpecVersion      = pCAMHandle->nVersion.nVersion;
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
 * to the component (via a core macro). The implementation for CAM Comprises of a 
 * command handling routine that essentially maps, the commands for state 
 * transitions to GPF commands to the CAM Core Task.
 *
 * @param [in] hComponent
 *     handle of component to execute the command
 * @param [in] Cmd
 *     Command for the component to execute
 * @param [in] nParam1
 *     Parameter for the command to be executed.  
 * @param  [in] Aauxilary command data.
 *
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
    OMX_CAM_EXIT_IF (NULL == hComponent);

    OMX_CAM_EXIT_IF (NULL == nParam1);

    //Currently only State Change Commands are supported.
    OMX_CAM_EXIT_IF (OMX_CommandStateSet != Cmd);

    /* Function to handle the command sequence/valid command */
    tRetVal = __OMX_CAM_HandleCommand(hComponent, nParam1);

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
 * @param OMX_IndexParamCameraModes :Gets the camera modes supported as in OMX_CAM_CAPTUREMODETYPE
 * @param OMX_IndexParamCameraOrientationType : Gets the Camera Orientation as in OMX_CAM_ORIENTATIONTYPE
 * @param OMX_IndexParamCameraRotationType : Gets the camera rotation modes as in OMX_TIIMAGE_ROTATIONTYPE
 * @param OMX_IndexParamImageSizeType : Gets the standard image sizes supported as in OMX_TIIMAGE_IMAGESIZETYPE
 * @param OMX_IndexParamFlashMode : Gets the Flash modes supported as in OMX_CAM_FLASH_MODETYPE
 * @param OMX_IndexParamExposureMode : Gets the Exposure modes as in OMX_EXPOSURECONTROLTYPE
 * @param OMX_IndexParamColorMode : Gets the image formats supported as in OMX_TIIMAGE_FORMATTYPE
 * @param OMX_IndexParamFrameRateMode : Gets the Framerates supported as in OMX_CAM_FRAMERATETYPE
 * @param OMX_IndexParamMirrorType : Gets the mirroring features offered by the camera as per OMX_MIRRORTYPE
 * @param OMX_IndexParamGammaSupport : Gets the Gamma correction support (Boolean)
 * @param OMX_IndexParamOpticalZoomSupport : Gets the Optical Zoom support status (Boolean)
 * @param OMX_IndexParamDigitalZoomSupport : Gets the Digital Zoom support status  (Boolean)
 * @param OMX_IndexParamContrastControlSupport : Gets the Contrast control support status  (Boolean)
 * @param OMX_IndexParamBrightnessControlSupport : Gets the brightness control support status  (Boolean)
 * @param OMX_IndexParamSharpnessControlSupport: Gets the Sharpness control support status  (Boolean)
 * @param OMX_IndexParamHalfToneSupport : Gets the Dithering support status  (Boolean)
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
#ifdef __CAMD_ADVANCED__
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    OMX_U32 nDeviceID;
    OMX_CAM_CMDTYPE tCMD;

    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pCAMPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;

    /* Check if the input params are valid */
    OMX_CAM_EXIT_IF((NULL == pComponent));
    OMX_CAM_EXIT_IF((NULL == nParamIndex));     
    OMX_CAM_EXIT_IF((NULL == ComponentParameterStructure));

    //Dereference the display ID if available
    nDeviceID = *(OMX_U32*)(ComponentParameterStructure);

    /* Check if the component is in Loaded State; else exit */
    OMX_CAM_EXIT_IF_IS((pCAMPvt->tCurState != OMX_StateLoaded));

    nSemCaller  = pCAMPvt->nSemCaller;
    nSemHandle  = pCAMPvt->nSemHandle;

    switch(nParamIndex){
        case OMX_IndexParamCameraModes :
            {
                tCMD = OMX_CAM_CMD_GETCAPTUREMODES;
            }
            break;
        case OMX_IndexParamCameraOrientationType: 
            {
                tCMD = OMX_CAM_CMD_GETORIENTATIONS;                 
            }
            break;
        case OMX_IndexParamCameraRotationType:
            {
                tCMD = OMX_CAM_CMD_GETROTATIONMODES;                    
            }
            break;
        case OMX_IndexParamImageSizeType:
            {
                tCMD = OMX_CAM_CMD_GETIMAGESIZES;
            }
            break;
        case OMX_IndexParamFlashMode :
            {
                tCMD = OMX_CAM_CMD_GETFLASHMODES;                   
            }
            break;
        case OMX_IndexParamExposureMode :
            {
                tCMD = OMX_CAM_CMD_GETEXPOSUREMODES;                    
            }
            break;
        case OMX_IndexParamColorMode: 
            {
                tCMD = OMX_CAM_CMD_GETCOLORFORMATS;
            }
            break;
        case OMX_IndexParamFrameRateMode:
            {
                tCMD = OMX_CAM_CMD_GETFRAMERATES;                   
            }
            break;
        case OMX_IndexParamMirrorType: 
            {
                tCMD = OMX_CAM_CMD_GETMIRRORMODES;                  
            }
            break;
        case OMX_IndexParamFocusMode: 
            {
                tCMD = OMX_CAM_CMD_GETFOCUSMODES;                   
            }
            break;
        case OMX_IndexParamGammaSupport :
            {
                tCMD = OMX_CAM_CMD_GETGAMMACORRSUPPORT;                 
            }
            break;
        case OMX_IndexParamOpticalZoomSupport :
            {
                tCMD = OMX_CAM_CMD_GETMAXOPTICALZOOM;                   
            }
            break;
        case OMX_IndexParamDigitalZoomSupport : 
            {
                tCMD = OMX_CAM_CMD_GETMAXDIGITALZOOM;
            }
            break;
        case OMX_IndexParamContrastControlSupport :
            {
                tCMD = OMX_CAM_CMD_GETCONTRASTCONTROL;
            }
            break;
        case OMX_IndexParamBrightnessControlSupport :
            {
                tCMD = OMX_CAM_CMD_GETBRIGHTNESSCONTROL;                    
            }
            break;
        case OMX_IndexParamSharpnessControlSupport: 
            {
                tCMD = OMX_CAM_CMD_GETSHARPNESSCONTROL;                 
            }
            break;
        case OMX_IndexParamHalfToneSupport :      
            {
                tCMD = OMX_CAM_CMD_GETDITHERINGSUPPORT;                 
            }
            break;
        default:
            /* Wrong Index Arguement */
            tCMD = NULL;
            tRetVal = OMX_ErrorBadParameter;    
    }
    if(tCMD !=NULL)
    {
        __OMX_CAM_SendMsgToCAMCore(hComponent ,tCMD, ComponentParameterStructure);
        vsi_s_get(nSemCaller, nSemHandle);
        OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));     
    }
#endif //__CAMD_ADVANCED__
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
 * Indices Supported for CAM with this function are:
 *
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent,
        OMX_TIIMAGE_INDEXTYPE nIndex,
        OMX_PTR ComponentParameterStructure)
{
    //Only GetConfig supported.
    return OMX_ErrorNotImplemented;
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
 * The GetConfig Indices supported by CAM  are:
 * @param OMX_IndexConfigCaptureMode :                    Gets the current Capture mode of the camera
 * @param OMX_IndexConfigFrameRateMode :                Gets the current Frame rate mode (Standard)of the camera
 * @param OMX_IndexConfigFrameRate :                        Gets the current Frame rate of the camera (if Non-Standard)
 * @param OMX_IndexConfigDither :                               Gets the currently configured dithering mode for the camera
 * @param OMX_IndexConfigSharpness :                        Gets the current Sharpness control setting for the camera
 * @param OMX_IndexConfigContrast :                           Gets the current Contrast control setting for the camera
 * @param OMX_IndexConfigBrightness :                        Gets he current brightness setting for the camera.
 * @param OMX_IndexConfigImageSizeType :                 Gets the Standard image size configuration is standard
 * @param OMX_IndexConfigImageWidth :                      Gets the capture width setting (Non-Standard)
 * @param OMX_IndexConfigImageHeight:                      Gets the capture height setting (Non-Standard)
 * @param OMX_IndexConfigMirrorType:                         Gets the Mirroring Set for the camera
 * @param OMX_IndexConfigImageFormat:                     Gets the Image format set for the capture mode
 * @param OMX_IndexConfigRotation :                            Gets the rotation mode set for camera
 * @param OMX_IndexConfigFlashmode :                        Gets the Flash mode set for the camera
 * @param OMX_IndexConfigExposureMode :                   Gets the Exposure mode set for the camera
 * @param OMX_IndexConfigFocusMode :                        Gets the Focus mode set for the camera
 * @param OMX_IndexConfigOpticalZoom :                      Gets the Optical Zoom value set
 * @param OMX_IndexConfigDigitalZoom:                        Gets the Digital Zoom value set
 * @param OMX_IndexConfigGamma :                             Gets the Gamma values set
 * @param OMX_IndexConfigConversionMatrixType:         Gets the color range - that decides the martix choice
 * @param OMX_IndexConfigConversionMatrix :               Gets the matrix used for color conversion
 * @param OMX_IndexConfigCameraSettings :                 Gets a structure with all the above features
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent,
        OMX_TIIMAGE_INDEXTYPE nIndex,
        OMX_PTR value)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_CAM_CONFIGTYPE * pCAMConfig;
    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pCAMPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    pCAMConfig = (OMX_CAM_CONFIGTYPE *)pCAMPvt->ptCamStatus;

    /* Check if the component is in a State After the Loaded State; else exit */
    OMX_CAM_EXIT_IF_IS((pCAMPvt->tCurState > OMX_StateLoaded));

    switch(nIndex){
#ifdef __CAMD_ADVANCED__
        case OMX_IndexConfigCaptureMode :            
            {
                OMX_CAM_CAPTUREMODETYPE * ptCaptureMode = (OMX_CAM_CAPTUREMODETYPE *)value;
                *ptCaptureMode = pCAMConfig->tCaptureMode;
            }
            break;
        case OMX_IndexConfigFrameRateMode :         
            {
                OMX_CAM_FRAMERATETYPE * ptFrameRateMode = (OMX_CAM_FRAMERATETYPE *) value;
                *ptFrameRateMode =  pCAMConfig->tFrameRateMode;
            }
            break;
        case OMX_IndexConfigFrameRate :                
            {
                OMX_U16  * punFrameRate = (OMX_U16 *)value;
                *punFrameRate = pCAMConfig->unFrameRate;
            }
            break;
        case OMX_IndexConfigDither :                       
            {
                OMX_DITHERTYPE * ptDither = (OMX_DITHERTYPE *)value;
                *ptDither = pCAMConfig->tDither;
            }
            break;
        case OMX_IndexConfigSharpness :                 
            {
                OMX_U16  * punSharpness = (OMX_U16 *)value;
                *punSharpness = pCAMConfig->unSharpness;                    
            }
            break;
        case OMX_IndexConfigContrast :                    
            {
                OMX_U16  * punContrast = (OMX_U16 *)value;
                *punContrast = pCAMConfig->unContrast;                  
            }
            break;
        case OMX_IndexConfigBrightness :                 
            {
                OMX_U16  * punBrightness = (OMX_U16 *)value;
                *punBrightness = pCAMConfig->unBrightness;                  
            }
            break;
        case OMX_IndexConfigImageSizeType :          
            {
                OMX_TIIMAGE_DIMENSIONTYPE* ptImageSizeType = (OMX_TIIMAGE_DIMENSIONTYPE*)value;
                *ptImageSizeType = pCAMConfig->tImageSizeType;
            }
            break;
        case OMX_IndexConfigImageWidth :               
            {
                OMX_U16  * punImageWidth = (OMX_U16 *)value;
                *punImageWidth = pCAMConfig->unImageWidth;                  
            }
            break;
        case OMX_IndexConfigImageHeight:               
            {
                OMX_U16  * punImageHeight = (OMX_U16 *)value;
                *punImageHeight = pCAMConfig->unImageHeight;                    
            }
            break;
        case OMX_IndexConfigMirrorType:                 
            {
                OMX_MIRRORTYPE* ptMirrorType = (OMX_MIRRORTYPE*)value;
                *ptMirrorType = pCAMConfig->tMirrorType;
            }
            break;
        case OMX_IndexConfigImageFormat:              
            {
                OMX_TICOLOR_FORMATTYPE* ptImageFormat = (OMX_TICOLOR_FORMATTYPE*)value;
                *ptImageFormat = pCAMConfig->tImageFormat;
            }
            break;
        case OMX_IndexConfigRotation :                    
            {
                OMX_TIIMAGE_ROTATETYPE* ptRotation = (OMX_TIIMAGE_ROTATETYPE*)value;
                *ptRotation = pCAMConfig->tRotation;
            }
            break;
        case OMX_IndexConfigFlashmode :                 
            {
                OMX_CAM_FLASH_MODETYPE* ptFlashmode = (OMX_CAM_FLASH_MODETYPE*)value;
                *ptFlashmode = pCAMConfig->tFlashmode;
            }
            break;
        case OMX_IndexConfigExposureMode :           
            {
                OMX_EXPOSURECONTROLTYPE* ptExposureMode = (OMX_EXPOSURECONTROLTYPE*)value;
                *ptExposureMode = pCAMConfig->tExposureMode;
            }
            break;
        case OMX_IndexConfigFocusMode :    
            {
                OMX_IMAGE_FOCUSCONTROLTYPE* ptFocusMode = (OMX_IMAGE_FOCUSCONTROLTYPE*)value;
                *ptFocusMode= pCAMConfig->tFocusMode;
            }
            break;            
        case OMX_IndexConfigOpticalZoom :     
            {
                OMX_U16  * punOpticalZoom = (OMX_U16 *)value;
                *punOpticalZoom = pCAMConfig->unOpticalZoom;                    
            }
            break;         
        case OMX_IndexConfigDigitalZoom: 
            {
                OMX_U16  * punDigitalZoom = (OMX_U16 *)value;
                *punDigitalZoom = pCAMConfig->unDigitalZoom;                    
            }
            break;               
        case OMX_IndexConfigGamma :      
            {
                OMX_U8 ** ppunGamma = (OMX_U8 **)value;
                *ppunGamma = pCAMConfig->punGamma;
            }
            break;               
        case OMX_IndexConfigConversionMatrixType: 
            {
                OMX_TIIMAGE_RGBRANGETYPE* ptConversionMatrixType = (OMX_TIIMAGE_RGBRANGETYPE *) value;
                *ptConversionMatrixType =  pCAMConfig->tConversionMatrixType;
            }
            break;
        case OMX_IndexConfigConversionMatrix :      
            {
                OMX_TIIMAGE_YUVMATRIXTYPE* ptConversionMatrix = (OMX_TIIMAGE_YUVMATRIXTYPE *) value;
                *ptConversionMatrix =  pCAMConfig->tConversionMatrix;
            }
            break; 
        case OMX_IndexConfigCameraSettings : 
            {
                OMX_CAM_CONFIGTYPE * ptCamConfig = (OMX_CAM_CONFIGTYPE *)value;
                ptCamConfig->tImageSizeType           =       pCAMConfig->tImageSizeType;                          
                ptCamConfig->tImageFormat              =       pCAMConfig->tImageFormat;          
                ptCamConfig->tRotation                     =        pCAMConfig->tRotation;              
                ptCamConfig->tConversionMatrixType =       pCAMConfig->tConversionMatrixType;
                ptCamConfig->tConversionMatrix        =       pCAMConfig->tConversionMatrix;       
                ptCamConfig->tCaptureMode              =       pCAMConfig->tCaptureMode;             
                ptCamConfig->tFrameRateMode          =       pCAMConfig->tFrameRateMode;          
                ptCamConfig->tMirrorType                 =       pCAMConfig->tMirrorType;                
                ptCamConfig->tFlashmode                 =       pCAMConfig ->tFlashmode;                
                ptCamConfig->tExposureMode            =       pCAMConfig ->tExposureMode;           
                ptCamConfig->tFocusMode                 =       pCAMConfig ->tFocusMode;                
                ptCamConfig->unFrameRate              =       pCAMConfig->unFrameRate;              
                ptCamConfig->tDither                       =       pCAMConfig->tDither;                       
                ptCamConfig->unSharpness              =       pCAMConfig->unSharpness;              
                ptCamConfig->unContrast                 =       pCAMConfig->unContrast;                 
                ptCamConfig->unBrightness              =       pCAMConfig->unBrightness;              
                ptCamConfig->unImageWidth            =       pCAMConfig->unImageWidth;            
                ptCamConfig->unImageHeight           =       pCAMConfig->unImageHeight;           
                ptCamConfig->unOpticalZoom           =       pCAMConfig->unOpticalZoom;           
                ptCamConfig->unDigitalZoom            =       pCAMConfig->unDigitalZoom;            
                ptCamConfig->punGamma               =       pCAMConfig->punGamma;
            }
            break;
#endif//__CAMD_ADVANCED__
      case OMX_IndexConfigCameraFeatures:
		  {
               OMX_CAM_FEATURETYPE *pCamFeatures = (OMX_CAM_FEATURETYPE*)value;
                if(pCAMPvt->tCamFeatures.zoom_support  == CAM_FEATURE_SUPPORTED)
                {
                        pCamFeatures->bDigitalZoomSupport = OMX_TRUE;
                        pCamFeatures->nMaxZoomHeight = pCAMPvt->tCamFeatures.max_height;
                        pCamFeatures->nMaxZoomWidth = pCAMPvt->tCamFeatures.max_width;
                }
               else
                pCamFeatures->bDigitalZoomSupport = OMX_FALSE;
               if(pCAMPvt->tCamFeatures.mirror_support == CAM_FEATURE_SUPPORTED)
               pCamFeatures->bMirrorSupport = OMX_TRUE;
               else
                pCamFeatures->bMirrorSupport = OMX_FALSE;
		  }
               break;
      default:      
            tRetVal = OMX_ErrorUnsupportedIndex;

    }

EXIT:    
    return tRetVal;
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
 * @param OMX_IndexConfigCaptureMode :               Sets the current Capture mode of the camera
 * @param OMX_IndexConfigFrameRateMode :           Sets the current Frame rate mode (Standard)of the camera
 * @param OMX_IndexConfigFrameRate :                    Sets the current Frame rate of the camera (if Non-Standard)
 * @param OMX_IndexConfigDither :                          Sets the dithering mode for the camera
 * @param OMX_IndexConfigSharpness :                   Sets the Sharpness control setting for the camera
 * @param OMX_IndexConfigContrast :                      Sets the Contrast control setting for the camera
 * @param OMX_IndexConfigBrightness :                   Sets the current brightness setting for the camera.
 * @param OMX_IndexConfigImageSizeType :            Sets the Standard image size configuration
 * @param OMX_IndexConfigImageWidth :                 Sets the capture width setting (Non-Standard)
 * @param OMX_IndexConfigImageHeight:                 Sets the capture height setting (Non-Standard)
 * @param OMX_IndexConfigMirrorType:                     Sets the Mirroring for the camera
 * @param OMX_IndexConfigImageFormat:                Sets the Image format for the capture mode
 * @param OMX_IndexConfigRotation :                       Sets the rotation mode for camera
 * @param OMX_IndexConfigFlashmode :                   Sets the Flash mode for the camera
 * @param OMX_IndexConfigExposureMode :              Sets the Exposure mode for the camera
 * @param OMX_IndexConfigFocusMode :                    Sets the Focus mode for the camera
 * @param OMX_IndexConfigOpticalZoom :                 Sets the Optical Zoom value set
 * @param OMX_IndexConfigDigitalZoom:                   Sets the Digital Zoom value set
 * @param OMX_IndexConfigGamma :                        Sets the Gamma values set
 * @param OMX_IndexConfigConversionMatrixType:    Sets the color range - that decides the martix choice
 * @param OMX_IndexConfigConversionMatrix :          Sets the matrix used for color conversion
 * @param OMX_IndexConfigCameraSettings :            Sets tha above camera configs in a structure
 *  @see  OMX_Component.h
 */
/* ========================================================================== */
static OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent,
        OMX_TIIMAGE_INDEXTYPE nIndex,
        OMX_PTR value)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvt;
    OMX_COMPONENTTYPE * pComponent;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    T_RV_RETURN tCamRetPath;
    OMX_CAM_CONFIGTYPE * pCamConfig;

    tCamRetPath.addr_id = NULL;
    tCamRetPath.callback_func = __OMX_CAM_Callback;

    if(hComponent == NULL)
    {
        return OMX_ErrorBadParameter;
    }

    pComponent = (OMX_COMPONENTTYPE *)hComponent;
    pCAMPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    pCamConfig = pCAMPvt->ptCamStatus;

    /* Check if the component is in a State After the Loaded State; else exit */
    OMX_CAM_EXIT_IF_IS((pCAMPvt->tCurState < OMX_StateLoaded));

    nSemCaller  = pCAMPvt->nSemCaller;
    nSemHandle = pCAMPvt->nSemHandle;

    switch(nIndex){
#ifdef __CAMD_ADVANCED__
        case OMX_IndexConfigCaptureMode:
            {
                //Update the camera's capture mode
                __OMX_CAM_SendMsgToCAMCore(hComponent, OMX_CAM_CMD_SETCAPTUREMODE, value);
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tCaptureMode = (OMX_CAM_CAPTUREMODETYPE)value;              
            }
            break;
        case OMX_IndexConfigFrameRateMode:
            {
                //Update the frame rate mode
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETFRMRATEMODE, value);
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tFrameRateMode = (OMX_CAM_FRAMERATETYPE)value;              
            }
            break;
        case OMX_IndexConfigFrameRate:
            {
                //Update the frame rate 
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETFRMRATE, value);
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);                  
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unFrameRate = (OMX_U16)value;
            }
            break;
        case OMX_IndexConfigDither:
            {
                //Update the dithering mode
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETDITHERING, value);
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);                  
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tDither = (OMX_DITHERTYPE)value;
            }
            break;
        case OMX_IndexConfigSharpness:
            {
                //Update the sharpness setting
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETSHARPNESS, value);
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unSharpness= (OMX_U16)value;
            }
            break;
        case OMX_IndexConfigContrast:
            {
                //Update the contrast setting
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETCONTRAST, value);  
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unContrast = (OMX_U16) value;
            }
            break; 
        case OMX_IndexConfigBrightness:
            {
                //Update the brightness setting
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETBRIGHTNESS, value);                
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unBrightness = (OMX_U16)value;
            }
            break;
        case OMX_IndexConfigImageSizeType:
            {
                //Update the image size for capture
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETSIZETYPE, value);              
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tImageSizeType = (OMX_TIIMAGE_DIMENSIONTYPE)value;
            }
            break;
        case OMX_IndexConfigImageWidth:
            {
                //Update image capture width
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETWIDTH, value);             
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unImageWidth = (OMX_U16)value;
            }
            break;          
        case OMX_IndexConfigImageHeight:
            {
                //Update the capture height
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETHEIGHT, value);
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unImageHeight =  (OMX_U16)value;
            }
            break;
        case OMX_IndexConfigImageFormat:
            {
                //Update the capture format
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETCAPTUREFORMAT, value);             
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tImageFormat = (OMX_TICOLOR_FORMATTYPE)value;
            }
            break;          
        case OMX_IndexConfigRotation :        
            {
                //Update the rotation
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETROTATION, value);              
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tRotation = (OMX_TIIMAGE_ROTATETYPE)value;
            }
            break;          
        case OMX_IndexConfigFlashmode :                 
            {
                //Update the flash configuration
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETFLASHMODE, value);             
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tFlashmode = (OMX_CAM_FLASH_MODETYPE)value;
            }
            break;          
        case OMX_IndexConfigExposureMode :            
            {
                //Update the update the exposure mode
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETEXPOSUREMODE, value);              
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tExposureMode = (OMX_EXPOSURECONTROLTYPE)value;
            }
            break;          
        case OMX_IndexConfigFocusMode :  
            {
                //Update the focus mode
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETFOCUSMODE, value);             
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tFocusMode = (OMX_IMAGE_FOCUSCONTROLTYPE)value;
            }
            break;          
        case OMX_IndexConfigOpticalZoom :     
            {
                //Update the optical zoom value
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETOPTICALZOOM, value);               
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unOpticalZoom = (OMX_U16)value;
            }
            break;          
        case OMX_IndexConfigDigitalZoom:     
            {
                //Update the digital zoom
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETDIGITALZOOM, value);               
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->unDigitalZoom = (OMX_U16)value;
            }
            break;          
        case OMX_IndexConfigGamma :    
            {
                //Update the gamma value
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETGAMMA, value);             
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->punGamma = (OMX_U8 *)value;
            }
            break;          
        case OMX_IndexConfigConversionMatrixType: 
            {
                //Update the data matrix range
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETMATRIXRANGE, value);               
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tConversionMatrixType = (OMX_TIIMAGE_RGBRANGETYPE)value;
            }
            break;          
        case OMX_IndexConfigConversionMatrix :  
            {
                OMX_TIIMAGE_YUVMATRIXTYPE* pValue = (OMX_TIIMAGE_YUVMATRIXTYPE*)value;
                //Update the conversion matrix
                __OMX_CAM_SendMsgToCAMCore(hComponent,OMX_CAM_CMD_SETMATRIX, value);                
                //Wait for confirmation from the CAM Core
                vsi_s_get(nSemCaller, nSemHandle);
                OMX_CAM_EXIT_IF_ERR((pCAMPvt->tOMX_CAMErrStatus != OMX_ErrorNone));             
                pCamConfig->tConversionMatrix.nMult_UB = pValue->nMult_UB;
                pCamConfig->tConversionMatrix.nMult_UG= pValue->nMult_UG;
                pCamConfig->tConversionMatrix.nMult_UR= pValue->nMult_UR;
                pCamConfig->tConversionMatrix.nMult_VB= pValue->nMult_VB;
                pCamConfig->tConversionMatrix.nMult_VG= pValue->nMult_VG;
                pCamConfig->tConversionMatrix.nMult_VR= pValue->nMult_VR;
                pCamConfig->tConversionMatrix.nMult_YB= pValue->nMult_YB;
                pCamConfig->tConversionMatrix.nMult_YG= pValue->nMult_YG;
                pCamConfig->tConversionMatrix.nMult_YR= pValue->nMult_YR;
                pCamConfig->tConversionMatrix.nOffset_U= pValue->nOffset_U;
                pCamConfig->tConversionMatrix.nOffset_V= pValue->nOffset_V;
                pCamConfig->tConversionMatrix.nOffset_Y= pValue->nOffset_Y; }
                break;
#endif
        case OMX_IndexConfigMirrorType:
            {
                T_CAMD_PARAMETERS  * tCamParam = &pCAMPvt->tCamParam;
				OMX_MIRRORTYPE *tMirrorConfig = (OMX_MIRRORTYPE*)value;
                    switch(*tMirrorConfig)
                    {
                        case OMX_MirrorNone :
                            tCamParam->flip_x = 0;
                            tCamParam->flip_y = 0;
                            break;
                       case OMX_MirrorHorizontal :
                            tCamParam->flip_x = 1;
                            tCamParam->flip_y = 0;
                            break;
                       case OMX_MirrorVertical :
                            tCamParam->flip_x = 0;
                            tCamParam->flip_y = 1;
                            break;
                       case OMX_MirrorBoth :
                            tCamParam->flip_x = 1;
                            tCamParam->flip_y = 1;
                            break;
                    }              //Update the mirroring for the camera
                    if(pCAMPvt->tCurState > OMX_StateLoaded)
                    {
                        camd_set_configparams(tCamParam, tCamRetPath);
                        vsi_s_get(nSemCaller, nSemHandle);
                        tRetVal = pCAMPvt->tOMX_CAMErrStatus;
                    }    
                pCamConfig->tMirrorType = (OMX_MIRRORTYPE)value;
                break;
            }  
        case OMX_IndexConfigDigitalZoom:
            {
                OMX_U8 *nZoom = (OMX_U8 *)value;
                T_CAMD_PARAMETERS  * tCamParam = &pCAMPvt->tCamParam;
                if (*nZoom > tCamParam->zoom )
                    camd_zoom(ZOOM_IN);
                else if(*nZoom< tCamParam->zoom)
                    camd_zoom(ZOOM_OUT);
                pCAMPvt->tCamParam.zoom = *nZoom;
                break;
            }
        case OMX_IndexConfigCameraSettings:
                {
                    OMX_CAM_CONFIGTYPE* pCamConfigPtr = (OMX_CAM_CONFIGTYPE*)value;
                    T_CAMD_PARAMETERS  * tCamParam = &pCAMPvt->tCamParam;
                    tCamParam->capturemode = pCamConfigPtr->tCaptureMode; 
                    tCamParam->resolution = map_TIItoCamdRes(pCamConfigPtr->tImageSizeType);
                    
                    if(OMX_TICOLOR_YCbYCr == pCamConfigPtr->tImageFormat)
                    {
                         tCamParam->encoding = CAMD_YUYV_INTERLEAVED;
                    }
                    else
                    {
                        tCamParam->encoding = CAMD_RGB_565;
                    }
                    
                    tCamParam->gamma_correction = CAMD_GAMMA_CORR_DEFAULT;
                    tCamParam->imagewidth = pCamConfigPtr->unImageWidth;
                    tCamParam->imageheight = pCamConfigPtr->unImageHeight ;   
                    switch(pCamConfigPtr->tMirrorType)
                    {
                        case OMX_MirrorNone :
                            tCamParam->flip_x = 0;
                            tCamParam->flip_y = 0;
                            break;
                       case OMX_MirrorHorizontal :
                            tCamParam->flip_x = 1;
                            tCamParam->flip_y = 0;
                            break;
                       case OMX_MirrorVertical :
                            tCamParam->flip_x = 0;
                            tCamParam->flip_y = 1;
                            break;
                       case OMX_MirrorBoth :
                            tCamParam->flip_x = 1;
                            tCamParam->flip_y = 1;
                            break;
                    }
                    tCamParam->rotate = 0;
                    tCamParam->zoom = 0;

                    if(pCAMPvt->tCurState > OMX_StateLoaded)
                    {
                        camd_set_configparams(tCamParam, tCamRetPath);
                        vsi_s_get(nSemCaller, nSemHandle);
                        tRetVal = pCAMPvt->tOMX_CAMErrStatus;
                        break;
                    }
                }
                break;
        default:
                //Invalid Index recieved
                tRetVal = OMX_ErrorUnsupportedIndex;
    }

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
    OMX_CAM_COMPONENT_PRIVATETYPE* pPrivateStruct;

    /* Check for valid component handle */
    if ((NULL == hComponent) || (NULL == pState)) {
        tRetVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pPrivateStruct = (OMX_CAM_COMPONENT_PRIVATETYPE *)pHandle->pComponentPrivate;
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
 * The CAM Client, passes on the input buffer to the CAM Core though a message.
 * No inputs for this component - so not implemented.
 */
/* ========================================================================== */
static OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent,
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    return OMX_ErrorNotImplemented;
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
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_U32 i;
    OMX_COMPONENTTYPE * pComponent = (OMX_COMPONENTTYPE *)hComponent;
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    OMX_BYTE tOutBuff = (OMX_BYTE)pBuffer->pBuffer;
    T_RV_RETURN tCamRetPath;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    OMX_ENTRYEXITFUNC_TRACE("OMX_CAM FillThisBuffer : Entry");
    if(pBuffer->pBuffer == NULL)
    {
        return OMX_ErrorBadParameter;
    }
    nSemCaller  = pCAMPvt->nSemCaller;
    nSemHandle  = pCAMPvt->nSemHandle;

    tCamRetPath.addr_id = NULL;
    tCamRetPath.callback_func = __OMX_CAM_Callback;

    //put the buffer header in the array
    pCAMPvt->aBuffHeaderArray[pCAMPvt->unBuffHeaderWriteIndex]  = pBuffer;
    pCAMPvt->unBuffHeaderWriteIndex++;
	if(pCAMPvt->unBuffHeaderWriteIndex == OMX_CAM_MAX_BUFFERS)
    {
        pCAMPvt->unBuffHeaderWriteIndex = 0;
    }
    camd_usebuff(pBuffer->pBuffer, tCamRetPath);
    OMX_ENTRYEXITFUNC_TRACE("OMX_CAM FillThisBuffer : Exit");
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
    OMX_CAM_COMPONENT_PRIVATETYPE *pCompPvt;

    /* Check the input parameters. */
    OMX_CAM_EXIT_IF (hComponent == NULL);
    OMX_CAM_EXIT_IF (pCallbacks == NULL); 
    OMX_CAM_EXIT_IF (pCallbacks->EmptyBufferDone == NULL);

    pHandle = (OMX_COMPONENTTYPE*)hComponent;
    pCompPvt = (OMX_CAM_COMPONENT_PRIVATETYPE*)pHandle->pComponentPrivate;

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
    OMX_CAM_COMPONENT_PRIVATETYPE* pPrivateStruct;
    T_RV_RETURN tCamRetPath;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle; 

    /* Check for valid component handle */
    if (NULL == hComponent) {
        tRetVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pPrivateStruct = (OMX_CAM_COMPONENT_PRIVATETYPE*)pHandle->pComponentPrivate;

    tCamRetPath.addr_id = NULL;
    tCamRetPath.callback_func = __OMX_CAM_Callback;

    nSemCaller  = pPrivateStruct->nSemCaller;
    nSemHandle  = pPrivateStruct->nSemHandle;
   
    camd_registerclient(OMX_FALSE, tCamRetPath);

    //For Unregister call back
    vsi_s_get(nSemCaller, nSemHandle);     

    /* Free the semaphore */
    vsi_s_close(pPrivateStruct->nSemCaller, pPrivateStruct->nSemHandle);
os_DestroySemaphore(pPrivateStruct->nSemCaller, pPrivateStruct->nSemHandle);
    /* Free the camera config structure memory in the component private structure */
    OMX_FREE(pPrivateStruct->ptCamStatus);
	pPrivateStruct->ptCamStatus = NULL;
    /* free the teh component private structure */
    OMX_FREE(pHandle->pComponentPrivate);
pHandle->pComponentPrivate =NULL;
    //to remove task priority ambiguity set it to null
    hComponentGS = NULL;
    
EXIT:    
    return tRetVal ;
}

/* ========================================================================== */
/**
 * @fn OMX_ComponentInit_<comp_name> method is the first call into a component 
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
 * //TODO:
 * THIS FUNCTION NEEDS TO BE ADDED TO THE declaration list in COMPONENTTABLE.C FILE
 * AND TO THE TABLE AS SUCH.
 */
/* ========================================================================== */
OMX_ERRORTYPE OMX_CAM_ComponentInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_CAM_COMPONENT_PRIVATETYPE* pPrivateStruct;
    OMX_U8 count = 0;
    OMX_U8 cSemName[20];
    T_HANDLE entityHandle;
    T_RV_RETURN tCamRetPath;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    
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
    pHandle->pComponentPrivate = (OMX_CAM_COMPONENT_PRIVATETYPE*)OMX_ALLOC(sizeof(OMX_CAM_COMPONENT_PRIVATETYPE));

    if (NULL == pHandle->pComponentPrivate) {
        tRetVal = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pPrivateStruct = (OMX_CAM_COMPONENT_PRIVATETYPE*)pHandle->pComponentPrivate;
    pPrivateStruct->ptCamStatus = (OMX_CAM_CONFIGTYPE *)OMX_ALLOC (sizeof(OMX_CAM_CONFIGTYPE));
    
    /* Create a semaphore resource for Client-Core communication */
    ltoa((OMX_U32)hComponent, (OMX_STRING)&cSemName[0]);
    pPrivateStruct->nSemHandle = vsi_s_open(entityHandle, (OMX_STRING)cSemName, count);
    pPrivateStruct->nSemCaller = (OMX_U32) entityHandle;

    /* Get the CAM Core handle and populate the structure for future use */
    /* Get the current Task Handle*/
    pPrivateStruct->tCamdClientHandle = entityHandle;

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

    pPrivateStruct->unBuffHeaderReadIndex       = 0;
    pPrivateStruct->unBuffHeaderWriteIndex      = 0;

    tCamRetPath.addr_id = NULL;
    tCamRetPath.callback_func = __OMX_CAM_Callback;

    nSemCaller  = pPrivateStruct->nSemCaller;
    nSemHandle  = pPrivateStruct->nSemHandle;

    hComponentGS = hComponent;

    camd_registerclient(OMX_TRUE, tCamRetPath);
    vsi_s_get(nSemCaller, nSemHandle);    
    if(pPrivateStruct->tOMX_CAMErrStatus != OMX_ErrorNone)
    	tRetVal = OMX_ErrorHardware;
    else
	camd_get_sensor_capabilities(&pPrivateStruct->tCamFeatures);
        
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

OMX_ERRORTYPE __OMX_CAM_HandleCommand (OMX_HANDLETYPE hComponent, 
        OMX_U32 newState)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_STATETYPE stateBeforeCmd;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE*)hComponent;
    OMX_CAM_COMPONENT_PRIVATETYPE *pCompPvt;
    OMX_CAM_CONFIGTYPE * ptCamConfig;
    T_RV_RETURN tCamRetPath;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;    

    pCompPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pHandle->pComponentPrivate;

    nSemCaller  = pCompPvt->nSemCaller;
    nSemHandle  = pCompPvt->nSemHandle;

    tCamRetPath.addr_id = NULL;
    tCamRetPath.callback_func = __OMX_CAM_Callback;

    stateBeforeCmd = pCompPvt->tCurState;

    switch (stateBeforeCmd) 
    {
        case OMX_StateLoaded:
            /* 
             * From the loaded state, the component can move to IDLE or be unloaded. Other
             * transitions are invalid.
             */
            if(newState == OMX_StateIdle)
            {
                /*
                 * Command recieved to move to idle state.  TODO: Clarify how this maps
                 */
                camd_set_configparams(&pCompPvt->tCamParam, tCamRetPath);
                vsi_s_get(nSemCaller, nSemHandle);

                tRetVal = pCompPvt->tOMX_CAMErrStatus;
		 if (tRetVal != OMX_ErrorNone)
		 	tRetVal = OMX_ErrorNotReady;
                else {	
                //Set the state to IDLE
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                //Call the event handler call back for the state change that happened.
                pCompPvt->cbInfo.EventHandler(hComponent,
                        pHandle->pApplicationPrivate, 
                        OMX_EventCmdComplete, 
                        OMX_ErrorNone,
                        pCompPvt->tCurState,
                        NULL);
                }
                break;
            }

            OMX_CAM_EXIT_IF_IS(1);

        case OMX_StateIdle: 
            /*
             * In the IDLE State, all resources for the components have been allocated.  
             * The transitions permitted in this state are into: 1) Executing State 2) Pause 3) Loaded
             *  TODO: Clarify how this maps
             */
            if((newState == OMX_StateExecuting)|| (newState == OMX_StateLoaded))
            {             
                //camd_start_capture(tCamRetPath);
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
				if(newState == OMX_StateLoaded)
				{  
					camd_stopviewfinder();
      				{
					OMX_U16 i = 0;	
		            for(;i<OMX_CAM_MAX_BUFFERS; i++)
	                pCompPvt->aBuffHeaderArray[i] = NULL;
	                pCompPvt->unBuffHeaderReadIndex = 0;
	                pCompPvt->unBuffHeaderWriteIndex = 0;
					}
      			}
	            pCompPvt->cbInfo.EventHandler(hComponent,
                        pHandle->pApplicationPrivate, 
                        OMX_EventCmdComplete, 
                        OMX_ErrorNone,
                        pCompPvt->tCurState,
                        NULL);        
                break;
            }

            OMX_CAM_EXIT_IF_IS(1);

        case OMX_StateExecuting: 
            /*
             * The component is in the execute state. The sensor is confgured and ready to give frames as per
             * the configuration.
             */
            if((newState == OMX_StateIdle) || (newState == OMX_StateLoaded))
            {
                pCompPvt->tCurState = (OMX_STATETYPE)newState;
                if(newState == OMX_StateLoaded)
				{
					camd_stopviewfinder();
      				{
					OMX_U16 i = 0;	
		            for(;i<OMX_CAM_MAX_BUFFERS; i++)
	                pCompPvt->aBuffHeaderArray[i] = NULL;
	                pCompPvt->unBuffHeaderReadIndex = 0;
	                pCompPvt->unBuffHeaderWriteIndex = 0;
					}
      			}         
                pCompPvt->cbInfo.EventHandler(hComponent,
                        pHandle->pApplicationPrivate, 
                        OMX_EventCmdComplete, 
                        OMX_ErrorNone,
                        pCompPvt->tCurState,
                        NULL);                
                break;
            }

            OMX_CAM_EXIT_IF_IS(1);

        default:            
            OMX_CAM_EXIT_IF_IS(1);
    }

EXIT:

    if(tRetVal != OMX_ErrorNone)
    {
        pCompPvt->cbInfo.EventHandler(hComponent,
                pHandle->pApplicationPrivate, 
                OMX_EventError, 
                tRetVal,
                pCompPvt->tCurState,
                NULL);
    }

    return tRetVal;
}

/* ========================================================================== */
/**
 * __OMX_CAM_SendMsgToCAMCore() 
 *
 * This function populates a message structure and sends it as a GPF message to the CAM Core task. Besides the
 * command and the arguement, it also sends the CALL BACK function to the core so that the core may convey the 
 * status of the command. 
 *
 * NOTE:
 * Synchronization occurs between the Core and the Client through a GPF Semaphore. This is to be released by the core
 * to acknowledge a command - the client waits for this semaphore and reads the status populated in its private structure
 * to know of any error status. For asynchronous functions, though, the client returns to the application immediately and
 * must communicate to the application by a callback provided.
 *
 * @param cmdToCAMCore command to the CAM core from the client
 *
 * @param auxDataToCAMCore Auxilary parameter to the CAM Core from the client
 *
 * @return           None
 *
 * @see              __OMX_CAM_SendMsgToCAMCore()
 */
/* ========================================================================== */
OMX_ERRORTYPE __OMX_CAM_SendMsgToCAMCore(OMX_HANDLETYPE hComponent, OMX_CAM_CMDTYPE cmdToCAMCore, OMX_PTR auxDataToCAMCore)
{
    OMX_U32 nBuffSize;
    OMX_U32 nMsgRetStatus;
    OMX_CAM_MESSAGE_STRUCTTYPE * pMSGBuffer;
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pComponent = (OMX_COMPONENTTYPE*)hComponent;
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
#if __CAMD_ADVANCED__
    /* Compute the size of the message buffer */
    nBuffSize = sizeof(OMX_CAM_MESSAGE_STRUCTTYPE);

    pMSGBuffer = (OMX_CAM_MESSAGE_STRUCTTYPE *) vsi_c_pnew_generic(pCAMPvt->tCamdClientHandle, nBuffSize, 0,  (VSI_MEM_NON_BLOCKING) FILE_LINE_MACRO);

    if(pMSGBuffer == NULL)
    {
        tRetVal = OMX_ErrorInsufficientResources;
        return tRetVal;
    }

    //Populate the message here with details
    pMSGBuffer->tCMD = cmdToCAMCore;//The command
    pMSGBuffer->pArgument = auxDataToCAMCore;//auxilary data
    pMSGBuffer->pComponentHandle = pComponent;//Component Handle : Sender's ID actually
    pMSGBuffer->pCAMCallback = __OMX_CAM_Callback;//Call back function

    //Send message to the CAM Core
    nMsgRetStatus = vsi_c_psend(pCAMPvt->tCamdCoreHandle, (T_VOID_STRUCT *) pMSGBuffer);
    if(nMsgRetStatus != VSI_OK)
    {
        //The communication failed.
        tRetVal = OMX_ErrorNotReady;
    }
#endif    
    return tRetVal; 
}

/* ========================================================================== */
/**
 * __OMX_CAM_Callback() 
 *
 * This is the callback function that is called by the CAM Core for any command that is sent. It has all OUT params
 * sent to it as pointers. It calls the function with the error status and also populates a strucutre and passes to the
 * function in response to the function.
 *
 * @param hComponent Calling component's handle
 *
 * @param nErrorStatus Error status updated by the CAM Core
 *
 * @param pData For OMX_CAM_CMD_SETINBUFF call, this parameter is the buffer header data type.
 *
 * @return    None
 *
 * @see  __OMX_CAM_SendMsgToCAMCore()
 */
/* ========================================================================== */
void __OMX_CAM_Callback ( OMX_PTR msgCam)
{
    OMX_COMPONENTTYPE * pComponent = (OMX_COMPONENTTYPE *)hComponentGS;
    OMX_CAM_COMPONENT_PRIVATETYPE * pCAMPvt = (OMX_CAM_COMPONENT_PRIVATETYPE *)pComponent->pComponentPrivate;
    OMX_U32 nSemCaller;
    OMX_U32 nSemHandle;
    T_OMX_CAMDMSGTYPE *pCBMsg;

    OMX_ENTRYEXITFUNC_TRACE("OMX_CAM CALLBACK: Entry");

    pCBMsg = (T_OMX_CAMDMSGTYPE*)msgCam;
    
    if(pCAMPvt == NULL || pCBMsg == NULL || hComponentGS == NULL)
    {
        if(pCBMsg != NULL)
        {
            OMX_DETAILFUNC_TRACE("OMX_CAM_Callback: CAM CB EXCEPTION: Msg NOT Null");        
            rvf_free_buf (pCBMsg); 
        }
        OMX_DETAILFUNC_TRACE("OMX_CAM_Callback:CAM CB EXCEPTION: Comp or Pvt Null");                
        return;
    }

    //TODO Map Errors from CAMD to OMX_CAMD
    pCAMPvt->tOMX_CAMErrStatus = mapRivToTII_Error(pCBMsg->status);
    nSemCaller  = pCAMPvt->nSemCaller;
    nSemHandle  = pCAMPvt->nSemHandle;
    /*
     * Check if the command is the OMX_CAM_CMD_SETINBUFF command.
     * If yes, we need to call back the EmptyBufferDone function.
     */
    if(CAMD_VIEWFINDER_DATA_RSP_MSG == pCBMsg->os_hdr.msg_id || CAMD_SNAPSHOT_DATA_RSP_MSG == pCBMsg->os_hdr.msg_id)
    {
        if(pCAMPvt->tCurState == OMX_StateExecuting ){
        	if(pCAMPvt->tOMX_CAMErrStatus == OMX_ErrorNone) {
		pCAMPvt->cbInfo.FillBufferDone(hComponentGS, pComponent->pApplicationPrivate, 
                (OMX_BUFFERHEADERTYPE* )pCAMPvt->aBuffHeaderArray[pCAMPvt->unBuffHeaderReadIndex]);  
        }
      	else {
      		OMX_PROFILESTUB("Error in capturing frame from camera Callback");
      	 pCAMPvt->cbInfo.EventHandler(hComponentGS,
                pComponent->pApplicationPrivate,
                OMX_EventError,
                (OMX_U32)pCAMPvt->tOMX_CAMErrStatus, 
                0,
                NULL);
      	}
      	}      	
        pCAMPvt->unBuffHeaderReadIndex++;
		
        if(pCAMPvt->unBuffHeaderReadIndex == OMX_CAM_MAX_BUFFERS)
        {
            pCAMPvt->unBuffHeaderReadIndex = 0;
        }
        
    }
    else if(CAMD_REGISTERCLIENT_REQ_MSG == pCBMsg->os_hdr.msg_id || CAMD_SET_CONFIGPARAMS_REQ_MSG == pCBMsg->os_hdr.msg_id)
    {
        //release semaphore for all control message call backs
        OMX_PROFILESTUB("Registered/Setconfig to Camera Callback");
        vsi_s_release(nSemCaller, nSemHandle);
    }
    else
    {
        OMX_DETAILFUNC_TRACE("OMX_CAM_Callback: unexpected callback from camera driver");
        OMX_PROFILESTUB("Registered/Setconfig to Camera Callback");
        pCAMPvt->cbInfo.EventHandler(hComponentGS,
                pComponent->pApplicationPrivate,
                OMX_EventError, 
                pCAMPvt->tOMX_CAMErrStatus,
                0,
                NULL);
    }

    OMX_ENTRYEXITFUNC_TRACE("OMX_CAM CALLBACK: Exit");
   
    //free the message structure
    rvf_free_buf (msgCam);
    return;
}
T_CAMD_RESOLUTION map_TIItoCamdRes(OMX_TIIMAGE_DIMENSIONTYPE inpRes)
{
	switch(inpRes)
	{
		case OMX_TIIMAGE_QCIF : 
			return CAMD_QCIF;
		case OMX_TIIMAGE_VGA : 
			return CAMD_VGA;
	       #if CAM_SENSOR == 1
		case OMX_TIIMAGE_SXGA : 
			return CAMD_SXGA;
	       #endif
		default : 
			return CAMD_QCIF;			
	}
}

OMX_ERRORTYPE mapRivToTII_Error(T_RV_RET rivError)
{
	switch(rivError)
	{
		case RV_OK :
			return OMX_ErrorNone;
		case RV_NOT_SUPPORTED :
			return OMX_ErrorNotImplemented;
		case RV_NOT_READY :
			return OMX_ErrorInvalidState;
		case RV_MEMORY_WARNING :
			return OMX_ErrorInsufficientResources;
		case RV_MEMORY_ERR :
			return OMX_ErrorUnderflow;
		case RV_MEMORY_REMAINING :
			return OMX_ErrorNone;
		case RV_INTERNAL_ERR :
			return OMX_ErrorUndefined;
		case RV_INVALID_PARAMETER :
			return OMX_ErrorBadParameter;
		default : 
			return OMX_ErrorNotImplemented;
	}
}


#endif
