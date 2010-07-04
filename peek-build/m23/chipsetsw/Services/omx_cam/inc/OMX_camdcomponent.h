/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */

/** OMX_camdComponent.h
 *  The OMX_camdComponent header file contains the definitions used to define
 *  the public interface of the camd client.  This header file is intended to
 *  be used by both the application and the component.
 */

#ifndef OMX_camdcomponent_h
#define OMX_camdcomponent_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/* 
 * Each OMX header must include all required header files to allow the
 * header to compile without errors.  The includes below are required
 * for this header file to compile successfully
 */
#include <OMX_Types.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include "omx_tiimage.h"

// ------------------------------------------------------------- //
/***************** PUBLIC INTERFACE STRUCTURES ******************/
// ------------------------------------------------------------- //
 /*
   * OMX_CAM_CAPTURE_MODETYPE: This Enumeration lists the modes in which the camera can be configured to.
   * @params VIEWFINDER : View finder mode - continuous stream video mode
   * @params SNAPSHOT : Snapshot mode - capture a single framebuffers
   */
typedef enum OMX_CAM_CAPTUREMODETYPE
{
       VIEWFINDER_MODE = 0,
	SNAPSHOT_MODE
}OMX_CAM_CAPTUREMODETYPE;


/*
  * OMX_CAM_FLASH_MODETYPE 
  * This enumeration gives the flash modes possible for the camera.
  * @param 	EFlashNone = 0,
  * @param	EFlashAuto,
  * @param	EFlashForced,
  * @param	EFlashFillIn,
  * @param	EFlashRedEyeReduce, 
  * @param	EFlashSlowFrontSync, 
  * @param	EFlashSlowRearSync, 
  * @param	EFlashManual
  */
typedef enum OMX_CAM_FLASH_MODETYPE{
	EFlashNone = 0,
	EFlashAuto,
	EFlashForced,
	EFlashFillIn,
	EFlashRedEyeReduce, 
	EFlashSlowFrontSync, 
	EFlashSlowRearSync, 
	EFlashManual
}OMX_CAM_FLASH_MODETYPE;


/* 
  * OMX_CAM_ORIENTATIONTYPE
  * This ennumeration gives the orientation of the camera sensor
  * @param 	EOrientationOutwards = 0,
  * @param	EOrientationInwards, 
  * @param	EOrientationMobile, 
  * @param	EOrientationUnknown
  */
typedef enum OMX_CAM_ORIENTATIONTYPE
{
	EOrientationOutwards = 0,
	EOrientationInwards, 
	EOrientationMobile, 
	EOrientationUnknown
}OMX_CAM_ORIENTATIONTYPE;

/* 
  * OMX_CAM_FRAMERATETYPE
  * This ennumeration gives the framerate supported by the camera 
  * @param 	EFrameRate5Fps = 0,
  * @param	EFrameRate10Fps, 
  * @param	EFrameRate15Fps, 
  * @param	EFrameRate30Fps,
  * @param    EFrameRateUserDefined
  */
typedef enum OMX_CAM_FRAMERATETYPE
{
	EFrameRate5Fps = 0,
	EFrameRate10Fps, 
	EFrameRate15Fps, 
	EFrameRate30Fps,
	EFrameRateUserDefined
}OMX_CAM_FRAMERATETYPE;

/*
  * OMX_CAM_CAMERA_FEATURESTYPE
  * Thie structure defines the list of all features/capabilities of the camera sensor/driver.
  * @param 	unCameraModes;
  * @param	unCameraOrientationTypes;
  * @param	unCameraRotationTypes;
  * @param	unImageSizeTypes;
  * @param	unFlashModes;
  * @param	unExposureModes;
  * @param	unColorModes;
  * @param	unFrameRateModes;
  * @param	unMirrorTypes;
  * @param	unFocusModes;
  * @param	bGammaSupport;
  * @param	bOpticalZoomSupport;
  * @param	bDigitalZoomSupport;
  * @param	bContrastControlSupport;
  * @param	bBrightnessControlSupport;
  * @param	bSharpnessControlSupport;
  * @param	bHalfToneSupport;
  */
typedef struct OMX_CAM_CAMERA_FEATURESTYPE
{
	OMX_U16   unCameraModes;
	OMX_U16   unCameraOrientationTypes;
	OMX_U16   unCameraRotationTypes;
	OMX_U16   unImageSizeTypes;
	OMX_U16   unFlashModes;
	OMX_U16   unExposureModes;
	OMX_U16   unColorModes;
	OMX_U16   unFrameRateModes;
	OMX_U16   unMirrorTypes;
	OMX_U16   unFocusModes;
	OMX_U16   unOpticalZoomSupport;	
	OMX_U16   unDigitalZoomSupport;	
	OMX_BOOL bGammaSupport;
	OMX_BOOL bContrastControlSupport;
	OMX_BOOL bBrightnessControlSupport;
	OMX_BOOL bSharpnessControlSupport;
	OMX_BOOL bHalfToneSupport;
}OMX_CAM_CAMERA_FEATURESTYPE;

/* 
  * OMX_CAM_HW_VERSIONTYPE
  * This structure is used to accomodate the Sensor and Controller version for
  * the camera.
  * @params iSensorVer
  * @params iCamControllerVer
  */
typedef struct OMX_CAM_HW_VERSIONTYPE
{
	OMX_U16 unSensorVer;
	OMX_U16 unCamControllerVer;
}OMX_CAM_HW_VERSIONTYPE;



/*
  * OMX_CAM_FEATURETYPE
  * This structure is used to accomodate the camera feature support details.
  * @params tMirrorSupport
  * @params bDigitalZoomSupport
  * @params nMaxZoomWidth
  * @params nMaxZoomHeight
 */
typedef struct OMX_CAM_FEATURETYPE
{
    OMX_BOOL bMirrorSupport;
    OMX_BOOL bDigitalZoomSupport;
    OMX_U16    nMaxZoomWidth;
    OMX_U16    nMaxZoomHeight;
}OMX_CAM_FEATURETYPE;

/*
  * OMX_CAM_CONFIGTYPE
  * This structure is used to configure the various parameters of the camera using the SetConfig Indices.
  * @params tCaptureMode
  * @params tFrameRateMode
  * @params unFrameRate
  * @params tDither
  * @params unSharpness
  * @params unContrast
  * @params unBrightness
  * @params tImageSizeType
  * @params unImageWidth
  * @params unImageHeight
  * @params tMirrorType
  * @params tImageFormat
  * @params tRotation
  * @params tFlashmode
  * @params tExposureMode
  * @params tFocusMode
  * @params unOpticalZoom
  * @params unDigitalZoom
  * @params punGamma
  * @params tConversionMatrixType
  * @params tConversionMatrix
  */
typedef struct OMX_CAM_CONFIGTYPE
{
  //Members to be defined the common header
  OMX_TIIMAGE_DIMENSIONTYPE tImageSizeType;
  OMX_TICOLOR_FORMATTYPE tImageFormat;
  OMX_TIIMAGE_ROTATETYPE tRotation;
  OMX_TIIMAGE_RGBRANGETYPE tConversionMatrixType;
  OMX_TIIMAGE_YUVMATRIXTYPE tConversionMatrix;
  //Locallay defined data types
  OMX_CAM_CAPTUREMODETYPE tCaptureMode;
  OMX_CAM_FRAMERATETYPE tFrameRateMode;
  OMX_MIRRORTYPE  tMirrorType;
  OMX_CAM_FLASH_MODETYPE tFlashmode;
  OMX_EXPOSURECONTROLTYPE tExposureMode;
  OMX_IMAGE_FOCUSCONTROLTYPE tFocusMode;
  OMX_U16  unFrameRate;
  OMX_DITHERTYPE tDither;	
  OMX_U16 unSharpness;
  OMX_U16 unContrast;
  OMX_U16 unBrightness;
  OMX_U16 unImageWidth;
  OMX_U16 unImageHeight;
  OMX_U16 unOpticalZoom;
  OMX_U16 unDigitalZoom;
  OMX_U8 *punGamma;
}OMX_CAM_CONFIGTYPE;

/* OMX SSL COMPONENT NAME */
#define OMXCAM_COMP_NAME      "OMX.TI.IMAGE.CAM"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/* File EOF */
