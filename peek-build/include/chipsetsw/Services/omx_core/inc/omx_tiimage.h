/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file OMX_TIImage.h
*
* This file contains the structure definitions for Imaging. 
*
* @path  chipsetsw\services\img\omx_img\
*
* @rev  0.1
*
*/
/* ------------------------------------------------------------------------- */
/* ==========================================================================
*!
*! Revision History
*! ===================================
*! 15-Feb-2006 anandhi@ti.com: Initial Release

*!
* ========================================================================== */
#ifndef OMX_TIIMAGE_H
#define OMX_TIIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "OMX_Types.h"
#include "OMX_Index.h"

#include "typedefs.h"
#include "vsi.h"
#include  "os_const.h"

#define OMX_VGA_HEIGHT  480
#define OMX_VGA_WIDTH   640
#define OMX_SXGA_HEIGHT 1024
#define OMX_SXGA_WIDTH  1280
#define OMX_QCIF_HEIGHT 144
#define OMX_QCIF_WIDTH  176

/* ==========================================================================*/
/*
* OMX_TICOLOR_FORMATTYPE structure contains the list of colour formats 
* available for the input or output images for all encoding, decoding  
* and image processing functions. 
 * @param OMX_TICOLOR_Unused                  Placeholder value when format is N/A
 * @param OMX_TICOLOR_Monochrome              black and white
 * @param OMX_TICOLOR_8bitRGB332              Red 7:5, Green 4:2, Blue 1:0
 * @param OMX_TICOLOR_12bitRGB444             Red 11:8, Green 7:4, Blue 3:0
 * @param OMX_TICOLOR_16bitARGB4444           Alpha 15:12, Red 11:8, Green 7:4, Blue 3:0
 * @param OMX_TICOLOR_16bitARGB1555           Alpha 15, Red 14:10, Green 9:5, Blue 4:0
 * @param OMX_TICOLOR_16bitRGB565             Red 15:11, Green 10:5, Blue 4:0
 * @param OMX_TICOLOR_16bitBGR565             Blue 15:11, Green 10:5, Red 4:0
 * @param OMX_TICOLOR_18bitRGB666             Red 17:12, Green 11:6, Blue 5:0
 * @param OMX_TICOLOR_18bitARGB1665           Alpha 17, Red 16:11, Green 10:5, Blue 4:0
 * @param OMX_TICOLOR_19bitARGB1666           Alpha 18, Red 17:12, Green 11:6, Blue 5:0
 * @param OMX_TICOLOR_24bitRGB888             Red 24:16, Green 15:8, Blue 7:0
 * @param OMX_TICOLOR_24bitBGR888             Blue 24:16, Green 15:8, Red 7:0
 * @param OMX_TICOLOR_24bitARGB1887           Alpha 23, Red 22:15, Green 14:7, Blue 6:0
 * @param OMX_TICOLOR_25bitARGB1888           Alpha 24, Red 23:16, Green 15:8, Blue 7:0
 * @param OMX_TICOLOR_32bitBGRA8888           Blue 31:24, Green 23:16, Red 15:8, Alpha 7:0
 * @param OMX_TICOLOR_32bitARGB8888           Alpha 31:24, Red 23:16, Green 15:8, Blue 7:0
 * @param OMX_TICOLOR_YUV411Planar            U,Y are subsampled by a factor of 4 horizontally
 * @param OMX_TICOLOR_YUV411PackedPlanar      packed per payload in planar slices
 * @param OMX_TICOLOR_YUV420Planar            Three arrays Y,U,V.
 * @param OMX_TICOLOR_YUV420PackedPlanar      packed per payload in planar slices
 * @param OMX_TICOLOR_YUV420SemiPlanar        Two arrays, one is all Y, the other is U and V
 * @param OMX_TICOLOR_YUV422Planar            Three arrays Y,U,V.
 * @param OMX_TICOLOR_YUV422PackedPlanar      packed per payload in planar slices
 * @param OMX_TICOLOR_YUV422SemiPlanar        Two arrays, one is all Y, the other is U and V
 * @param OMX_TICOLOR_YUV422H				  Chrominance is decimated horizontally
 * @param OMX_TICOLOR_YUV422V				  Chrominance is decimated vertically
 * @param OMX_TICOLOR_YCbYCr                  Organized as 16bit YUYV (i.e. YCbYCr)
 * @param OMX_TICOLOR_YCrYCb                  Organized as 16bit YVYU (i.e. YCrYCb)
 * @param OMX_TICOLOR_CbYCrY                  Organized as 16bit UYVY (i.e. CbYCrY)
 * @param OMX_TICOLOR_CrYCbY                  Organized as 16bit VYUY (i.e. CrYCbY)
 * @param OMX_TICOLOR_YUV444Interleaved       Each pixel contains equal parts YUV
 * @param OMX_TICOLOR_RawBayer8bit            SMIA camera output format
 * @param OMX_TICOLOR_RawBayer10bit           SMIA camera output format
 * @param OMX_TICOLOR_RawBayer8bitcompressed  SMIA camera output format
*/
/* ==========================================================================*/
typedef enum OMX_TICOLOR_FORMATTYPE  {
    
    /* colour format types*/
    OMX_TICOLOR_UNUSED = 0,	
    OMX_TICOLOR_Monochrome,
    OMX_TICOLOR_8bitRGB332,
    OMX_TICOLOR_12bitRGB444,
    OMX_TICOLOR_12bitRGB444Planar,
    OMX_TICOLOR_12bitRGB444Interleaved,
    OMX_TICOLOR_16bitARGB4444,
    OMX_TICOLOR_16bitARGB1555,
    OMX_TICOLOR_16bitRGB565,
    OMX_TICOLOR_16bitBGR565,
    OMX_TICOLOR_18bitRGB666,
    OMX_TICOLOR_18bitARGB1665,
    OMX_TICOLOR_19bitARGB1666, 
    OMX_TICOLOR_24bitRGB888,
    OMX_TICOLOR_24bitBGR888,
    OMX_TICOLOR_24bitRGB888Planar,
    OMX_TICOLOR_24bitARGB1887,
    OMX_TICOLOR_25bitARGB1888,
    OMX_TICOLOR_32bitBGRA8888,
    OMX_TICOLOR_32bitARGB8888,
    OMX_TICOLOR_YUV411Planar,
    OMX_TICOLOR_YUV411PackedPlanar,
    OMX_TICOLOR_YUV420Planar,
    OMX_TICOLOR_YUV420PackedPlanar,
    OMX_TICOLOR_YUV420SemiPlanar,
    OMX_TICOLOR_YUV420Interleaved,
    OMX_TICOLOR_YUV422Planar,
    OMX_TICOLOR_YUV422PackedPlanar,
    OMX_TICOLOR_YUV422SemiPlanar,
    OMX_TICOLOR_YUV422H,		
    OMX_TICOLOR_YUV422V,
    OMX_TICOLOR_YCbYCr,
    OMX_TICOLOR_YCrYCb,
    OMX_TICOLOR_CbYCrY,
    OMX_TICOLOR_CrYCbY,
    OMX_TICOLOR_YUV444Planar,
    OMX_TICOLOR_YUV444Interleaved,
    OMX_TICOLOR_RawBayer8bit,
    OMX_TICOLOR_RawBayer10bit,
    OMX_TICOLOR_RawBayer8bitcompressed,
    OMX_TICOLOR_L2, 
    OMX_TICOLOR_L4, 
    OMX_TICOLOR_L8, 
    OMX_TICOLOR_L16, 
    OMX_TICOLOR_L24, 
    OMX_TICOLOR_L32,
    OMX_TICOLOR_Max = 0x7FFFFFFF
    
} OMX_TICOLOR_FORMATTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_INDEXTYPE structure contains the list of indexes that are available to 
 * the application to set/get config/parameters for the component. 
 *
 * @param OMX_IndexParamCameraModes :Gets the camera modes supported as in OMX_CAM_CAPTUREMODETYPE
 *
 * @param OMX_IndexParamCameraOrientationType : Gets the Camera Orientation as in OMX_CAM_ORIENTATIONTYPE
 *
 * @param OMX_IndexParamCameraRotationType : Gets the camera rotation modes as in OMX_TIIMAGE_ROTATIONTYPE
 *
 * @param OMX_IndexParamImageSizeType : Gets the standard image sizes supported as in OMX_TIIMAGE_IMAGESIZETYPE
 *
 * @param OMX_IndexParamFlashMode : Gets the Flash modes supported as in OMX_CAM_FLASH_MODETYPE
 *
 * @param OMX_IndexParamExposureMode : Gets the Exposure modes as in OMX_EXPOSURECONTROLTYPE
 *
 * @param OMX_IndexParamColorMode : Gets the image formats supported as in OMX_TIIMAGE_FORMATTYPE
 *
 * @param OMX_IndexParamFrameRateMode : Gets the Framerates supported as in OMX_CAM_FRAMERATETYPE
 *
 * @param OMX_IndexParamMirrorType : Gets the mirroring features offered by the camera as per OMX_MIRRORTYPE
 *
 * @param OMX_IndexParamGammaSupport : Gets the Gamma correction support (Boolean)
 *
 * @param OMX_IndexParamOpticalZoomSupport : Gets the Optical Zoom support status (Boolean)
 *
 * @param OMX_IndexParamDigitalZoomSupport : Gets the Digital Zoom support status  (Boolean)
 *
 * @param OMX_IndexParamContrastControlSupport : Gets the Contrast control support status  (Boolean)
 *
 *@param OMX_IndexParamBrightnessControlSupport : Gets the brightness control support status  (Boolean)
 *
 * @param OMX_IndexParamSharpnessControlSupport: Gets the Sharpness control support status  (Boolean)
 *
 * @param OMX_IndexParamHalfToneSupport : Gets the Dithering support status  (Boolean)
 *
 * @param OMX_IndexConfigCaptureMode :                    Gets the current Capture mode of the camera
 *
 * @param OMX_IndexConfigFrameRateMode :                Gets the current Frame rate mode (Standard)of the camera
 *
 * @param OMX_IndexConfigFrameRate :                        Gets the current Frame rate of the camera (if Non-Standard)
 *
 * @param OMX_IndexConfigDither :                               Gets the currently configured dithering mode for the camera
 *
 * @param OMX_IndexConfigSharpness :                        Gets the current Sharpness control setting for the camera
 *
 * @param OMX_IndexConfigContrast :                           Gets the current Contrast control setting for the camera
 *
 * @param OMX_IndexConfigBrightness :                        Gets he current brightness setting for the camera.
 *
 * @param OMX_IndexConfigImageSizeType :                 Gets the Standard image size configuration is standard
 *
 * @param OMX_IndexConfigImageWidth :                      Gets the capture width setting (Non-Standard)
 *
 * @param OMX_IndexConfigImageHeight:                      Gets the capture height setting (Non-Standard)
 *
 * @param OMX_IndexConfigMirrorType:                         Gets the Mirroring Set for the camera
 *
 * @param OMX_IndexConfigImageFormat:                     Gets the Image format set for the capture mode
 *
 * @param OMX_IndexConfigRotation :                            Gets the rotation mode set for camera
 *
 * @param OMX_IndexConfigFlashmode :                        Gets the Flash mode set for the camera
 *
 * @param OMX_IndexConfigExposureMode :                   Gets the Exposure mode set for the camera
 *
 * @param OMX_IndexConfigFocusMode :                        Gets the Focus mode set for the camera
 *
 * @param OMX_IndexConfigOpticalZoom :                      Gets the Optical Zoom value set
 *
 * @param OMX_IndexConfigDigitalZoom:                        Gets the Digital Zoom value set
 *
 * @param OMX_IndexConfigGamma :                             Gets the Gamma values set
 *
 * @param OMX_IndexConfigConversionMatrixType:         Gets the color range - that decides the martix choice
 *
 * @param OMX_IndexConfigConversionMatrix :               Gets the matrix used for color conversion
 *
 * @param OMX_IndexConfigCameraSettings :                 Gets a structure with all the above features
 * 
 * @param OMX_IndexParamEncode	Index for setting/querying encode parameters
 *
 * @param OMX_IndexConfigEncode	Index for setting/querying encode configuration
 *
 * @param OMX_IndexConfigEncodeImgInfo	Index for querying encode output information
 *
 * @param OMX_IndexParamEncodeDefault	 Index for querying default encode parameters
 *
 * @param OMX_IndexParamDecode 	Index for setting/querying decode parameters 
 *
 * @param OMX_IndexConfigDecode 	Index for setting/querying decode configuration 
 *
 * @param OMX_IndexConfigDecodeImgInfo Index for querying decode output information
 *
 * @param OMX_IndexParamDecodeDefault  Index for querying default decode parameters 
 *
 * @param OMX_IndexParamRotate Index for setting/querying rotate parameters 
 *
 * @param OMX_IndexConfigRotate Index for setting/querying rotate configuration 
 *
 * @param OMX_IndexParamRotateDefault  Index for querying rotate defaults parameters 
 *
 * @param OMX_IndexParamRescale Index for setting/querying rescale parameters 
 *
 * @param OMX_IndexConfigRescale Index for setting/querying rescale configuration 
 *
 * @param OMX_IndexParamRescaleDefault Index for querying rescale defaults parameters 
 *
 * @param OMX_IndexParamEffect Index for setting/querying effect parameters 
 *
 * @param OMX_IndexConfigEffect Index for setting/querying effect configuration 
 *
 * @param OMX_IndexParamEffectDefault Index for querying effect defaults parameters 
 *
 * @param OMX_IndexParamOverlay Index for setting/querying overlay parameters 
 *
 * @param OMX_IndexConfigOverlay Index for setting/querying overlay configuration 
 *
 * @param OMX_IndexParamOverlayDefault  Index for querying overlay defaults parameters
 *
 * @param OMX_IndexParamColorConversion  Index for setting/querying colorconversion parameters 
 *
 * @param OMX_IndexConfigColorConversion  Index for setting/querying colorconversion configuration
 *
 * @param OMX_IndexParamColorConversionDefault Index for querying colorconversion default parameters 
 *
 * @param OMX_IndexParamNumAvailableDisplays : Number of Physical Displays available
 *
 * @param OMX_IndexParamDisplayInfo    : Vendor Names and Device IDs
 *
 * @param OMX_IndexParamDisplayProperties : Properties of the default/set Physical Display
 *                                   - needs a valid device ID.    
 *
 * @param OMX_IndexParamDisplayMaxWidth  : Maximum display width for the display
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayMaxHeight : Maximum display height for the display
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayDataFormat : Display format set for the display
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayDitherSupport : Flag that indicates if dithering is enabled.
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayOrientation : The orientation set in the current display
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayEnabled : Flag that indicates if the current display is enabled or not
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDiplayBackLitConfig : Structure that provides the current BackLite configuration
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayVendorName : Parameter that returns the display vendor name
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexParamDisplayIsActive : Flag that returns if a Display is active now - means that a physical 
 *                                 display is attached to the current plane.
 *                                   - needs a valid device ID.
 *
 * @param OMX_IndexConfigSSLPlaneAlpha : Obtain the aplha set for this plane
 *
 * @param OMX_IndexConfigSSLPlaneAlwaysOnTop : Identifies if this plane is an Always on Top plane
 *
 * @param OMX_IndexConfigSSLPlaneActiveFlag : Indicates if this plane is active or not
 *
 * @param OMX_IndexConfigSSLPlaneSuspendedFlag : Indicates if this plane is 'Paused' or not
 *
 * @param OMX_IndexConfigSSLPlaneSrcPtr : Contains the source data pointer for this plane
 *
 * @param OMX_IndexConfigSSLPlaneShadowPtr : Contains the shadow data pointer for this plane
 *
 * @param OMX_IndexConfigSSLPlaneXOffset : X Offset for the current plane
 *
 * @param OMX_IndexConfigSSLPlaneYOffset : Y Offset for the current plane
 *
 * @param OMX_IndexConfigSSLPlaneXLen : Width for the current plane
 *
 * @param OMX_IndexConfigSSLPlaneYLen : Height for the current plane
 *
 * @param OMX_IndexConfigSSLPlaneFrameBuffPtr : FrameBuffer pointer for the current plane
 *
 * @param OMX_IndexConfigSLLPlaneIsDSAPlane : Indicates if this plane is a DSA Plane
 *
 * @param OMX_IndexConfigSSLPlaneConfig : Plane configuration details in a structure
 *
 * @param OMX_IndexConfigSSLPlaneDataFrmt : Plane's data format
 *
 * @param OMX_IndexConfigSSLPlaneActiveDispID : ID of the active display
 *
 */
/* ==========================================================================*/

typedef enum OMX_TIIMAGE_INDEXTYPE {

	/* Indexes for CAM client */
	OMX_IndexParamCameraModes =  OMX_IndexIndexVendorStartUnused + 1,
	OMX_IndexParamCameraOrientationType ,
	OMX_IndexParamCameraRotationType,
	OMX_IndexParamImageSizeType ,
	OMX_IndexParamFlashMode,
	OMX_IndexParamExposureMode,
	OMX_IndexParamColorMode,
	OMX_IndexParamFrameRateMode ,
	OMX_IndexParamMirrorType ,
	OMX_IndexParamFocusMode ,
	OMX_IndexParamGammaSupport ,
	OMX_IndexParamOpticalZoomSupport,
	OMX_IndexParamDigitalZoomSupport ,
	OMX_IndexParamContrastControlSupport ,
	OMX_IndexParamBrightnessControlSupport,
	OMX_IndexParamSharpnessControlSupport,
	OMX_IndexParamHalfToneSupport,
	
	OMX_IndexConfigCaptureMode,
	OMX_IndexConfigFrameRateMode,
	OMX_IndexConfigFrameRate,
	OMX_IndexConfigDither,
	OMX_IndexConfigSharpness,
	OMX_IndexConfigContrast,
	OMX_IndexConfigBrightness,
	OMX_IndexConfigImageSizeType,
	OMX_IndexConfigImageWidth,
	OMX_IndexConfigImageHeight,              
	OMX_IndexConfigMirrorType,                 
	OMX_IndexConfigImageFormat,             
	OMX_IndexConfigRotation,                    
	OMX_IndexConfigFlashmode,                 
	OMX_IndexConfigExposureMode,            
	OMX_IndexConfigFocusMode,                 
	OMX_IndexConfigOpticalZoom,              
	OMX_IndexConfigDigitalZoom,               
	OMX_IndexConfigGamma,                    
	OMX_IndexConfigConversionMatrixType, 
	OMX_IndexConfigConversionMatrix,   
	OMX_IndexConfigCameraFeatures, 
	OMX_IndexConfigCameraSettings, 
	 	
	/* Indexes for IMG client */
	OMX_IndexParamEncode,
	OMX_IndexConfigEncode ,
	OMX_IndexConfigEncodeImgInfo,
	OMX_IndexParamEncodeDefault ,

	OMX_IndexParamDecode,
	OMX_IndexConfigDecode ,
	OMX_IndexConfigDecodeImgInfo,
	OMX_IndexParamDecodeDefault ,

	OMX_IndexParamRotate,
	OMX_IndexConfigRotate ,
	OMX_IndexParamRotateDefault ,

	OMX_IndexParamRescale,
	OMX_IndexConfigRescale ,
	OMX_IndexParamRescaleDefault ,		

	OMX_IndexParamEffect,
	OMX_IndexConfigEffect,
	OMX_IndexParamEffectDefault ,

	OMX_IndexParamOverlay,
	OMX_IndexConfigOverlay ,
	OMX_IndexParamOverlayDefault ,		

	OMX_IndexParamColorConversion,
	OMX_IndexConfigColorConversion ,
	OMX_IndexParamColorConversionDefault ,

	/* Indexes for SSL client */
	OMX_IndexParamNumAvailableDisplays,
   	OMX_IndexParamDisplayInfo,
   	OMX_IndexParamDisplayProperties,
  	OMX_IndexParamDisplayMaxWidth,
   	OMX_IndexParamDisplayMaxHeight,
   	OMX_IndexParamDisplayDataFormat,
   	OMX_IndexParamDisplayDitherSupport,
   	OMX_IndexParamDisplayOrientation,
   	OMX_IndexParamDisplayEnabled,
   	OMX_IndexParamDiplayBackLitConfig,
   	OMX_IndexParamDisplayVendorName,
   	OMX_IndexParamDisplayIsActive,
   	
   	OMX_IndexConfigSSLPlaneAlpha,
   	OMX_IndexConfigSSLPlaneAlwaysOnTop,
   	OMX_IndexConfigSSLPlaneActiveFlag,
   	OMX_IndexConfigSSLPlaneSuspendedFlag,
   	OMX_IndexConfigSSLPlaneXOffset,
   	OMX_IndexConfigSSLPlaneYOffset,
   	OMX_IndexConfigSSLPlaneXLen,
   	OMX_IndexConfigSSLPlaneYLen,
   	OMX_IndexConfigSSLPlaneFrameBuffPtr,
   	OMX_IndexConfigSLLPlaneIsDSAPlane,
   	OMX_IndexConfigSSLPlaneDataFrmt,
   	OMX_IndexConfigSSLPlaneActiveDispID,
   	OMX_IndexConfigSSLPlaneConfig,
   	OMX_IndexConfigSSLPlaneDSAPlane

} OMX_TIIMAGE_INDEXTYPE ;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_ALPHABLENDTYPE structure contains the list of possible alpha 
 * blend values available for image processing. This is passed as a configuration  
 * parameter in the OMX_TIIMAGE_OVERLAY_PARAMTYPE structure. 
 *
 * @param OMX_TIIMAGE_ALPHA0	Alpha value 0
 *
 * @param OMX_TIIMAGE_ALPHA25	Alpha value 25
 *
 * @param OMX_TIIMAGE_ALPHA50	Alpha value 50
 *
 * @param OMX_TIIMAGE_ALPHA75	Alpha value 75
 *
 * @param OMX_TIIMAGE_ALPHA100	Alpha value 100
 *
 * @param OMX_TIIMAGE_ALPHAMANUAL	Alpha value is user specified
  */
/* ==========================================================================*/
typedef enum OMX_TIIMAGE_ALPHABLENDTYPE   {
    
    /* alpha values */
	OMX_TIIMAGE_ALPHA0 = 0,
	OMX_TIIMAGE_ALPHA25,
	OMX_TIIMAGE_ALPHA50,
	OMX_TIIMAGE_ALPHA75,
	OMX_TIIMAGE_ALPHA100,
	OMX_TIIMAGE_ALPHAMANUAL,
	OMX_TIIMAGE_ALPHAMAX =	0x7FFFFFFF	
    
} OMX_TIIMAGE_ALPHABLENDTYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_ROTATETYPE structure contains the list of possible rotation 
 * degrees available for image processing. This is passed as a configuration  
 * parameter in the OMX_TIIMAGE_ROTATE_PARAMTYPE structure. 
 *
 * @param OMX_TIIMAGE_ROTATE0	 Rotate by 0 deg
 *
 * @param OMX_TIIMAGE_ROTATE90	 Rotate by 90 deg
 *
 * @param OMX_TIIMAGE_ROTATE180	 Rotate by 180 deg
 *	
 * @param OMX_TIIMAGE_ROTATE270	 Rotate by 270 deg
 *
 * @param OMX_TIIMAGE_ROTATEMANUAL Rotate by a user specified deg
 */
/* ==========================================================================*/
typedef enum OMX_TIIMAGE_ROTATETYPE   {
    
    /* degrees of rotation */
	OMX_TIIMAGE_ROTATEUNUSED = 0,		
	OMX_TIIMAGE_ROTATE0,				
	OMX_TIIMAGE_ROTATE90,				
	OMX_TIIMAGE_ROTATE180,				
	OMX_TIIMAGE_ROTATE270,				
	OMX_TIIMAGE_ROTATEMANUAL,
	OMX_TIIMAGE_ROTATEMAX =	0x7FFFFFFF	
    
} OMX_TIIMAGE_ROTATETYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_DIMENSIONTYPE structure contains the list of possible image 
 * dimensions available for image processing. 
 *
 * @param OMX_TIIMAGE_QCIF			QCIF Image Dimension - 176 * 144
 *
 * @param OMX_TIIMAGE_CIF			CIF Image Dimension - 352 * 288
 *
 * @param OMX_TIIMAGE_QQVGA		QQVGA Image Dimension - 160 * 120
 *	
 * @param OMX_TIIMAGE_QVGA			QVGA Image Dimension - 320 * 240
 *
 * @param OMX_TIIMAGE_VGA			VGA Image Dimension - 640 * 480
 *
 * @param OMX_TIIMAGE_SVGA			SVGA Image Dimension - 800 * 600
 *
 */
/* ==========================================================================*/
typedef enum OMX_TIIMAGE_DIMENSIONTYPE   {
    
    /* image dimensions */
	OMX_TIIMAGE_QCIF = 0,		
	OMX_TIIMAGE_CIF,				
	OMX_TIIMAGE_QQVGA,				
	OMX_TIIMAGE_QVGA,				
	OMX_TIIMAGE_VGA,
	OMX_TIIMAGE_SVGA,
	OMX_TIIMAGE_SXGA,
	OMX_TIIMAGE_DIMENSIONMAX =	0x7FFFFFFF	
    
} OMX_TIIMAGE_DIMENSIONTYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_ERGBRANGETYPE structure contains the list of possible ranges within which the 
 * R,G,B values may be present. This is a parameter for colour conversion. If the value is Manual,
 * the multiplication coefficients and constants for conversion are provided using the 
 *OMX_TIIMAGE_YUVMATRIXTYPE structure
 *
 * @param OMX_TIIMAGE_ERGBMX_0_255			RGB values range from 0 to 255
 *
 * @param OMX_TIIMAGE_ERGBMX_19_235		RGB values range from 19 to 255
 *
 * @param OMX_TIIMAGE_ERGBMX_MANUAL		RGB values range from 0 to 255
 *	
 */
/* ==========================================================================*/
typedef enum OMX_TIIMAGE_RGBRANGETYPE   {
    
    /*  conversion ranges */
	OMX_TIIMAGE_RGBMX_0_255= 0,		
	OMX_TIIMAGE_RGBMX_16_235,				
	OMX_TIIMAGE_RGBMX_MANUAL,
	OMX_TIIMAGE_RGBMX_MAX =	0x7FFFFFFF	
    
} OMX_TIIMAGE_RGBRANGETYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_YUVMATRIXTYPE structure contains the list of multiplication coefficients and the 
 * constants required for the conversion of a RGB to YUV. The conversion formula is :
 *
 *		 Y  =      (nMult_YR * R) + (nMult_YG * G) + (nMult_YB * B) + nOffset_Y
 *
 *		Cr = V =  (nMult_VR * R) + (nMult_VG * G) + (nMult_VB * B) + nOffset_V
 *
 *		Cb = U = (nMult_UR * R) + (nMult_UG * G) + (nMult_UB * B) + nOffset_U
 *	
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_YUVMATRIXTYPE   {
    
    /*  conversion coefficients */
	OMX_S32 nMult_YR ;  /** <Multiplication Coeff for R component of input and Y component of output */
	OMX_S32 nMult_YG ;  /** <Multiplication Coeff for G component of input and Y component of output */
	OMX_S32 nMult_YB ;  /** <Multiplication Coeff for B component of input and Y component of output */	
	OMX_S32 nMult_VR ;  /** <Multiplication Coeff for R component of input and V component of output */
	OMX_S32 nMult_VG ;  /** <Multiplication Coeff for G component of input and V component of output */
	OMX_S32 nMult_VB ;  /** <Multiplication Coeff for B component of input and V component of output */
	OMX_S32 nMult_UR ;  /** <Multiplication Coeff for R component of input and U component of output */
	OMX_S32 nMult_UG ;  /** <Multiplication Coeff for G component of input and U component of output */
	OMX_S32 nMult_UB ;  /** <Multiplication Coeff for B component of input and U component of output */
	OMX_S32 nOffset_Y ;  /** <Offset  for Y component of output */
	OMX_S32 nOffset_V ;  /** <Offset for V component of output */
	OMX_S32 nOffset_U ;  /** <Offset for U component of output */
    
} OMX_TIIMAGE_YUVMATRIXTYPE;

extern T_HANDLE mm_ext_data_pool_handle;
extern T_HANDLE mm_int_data_pool_handle;

OMX_PTR __omx_alloc(OMX_U32);
OMX_PTR __omx_int_alloc(OMX_U32);
OMX_U16 __omx_free(OMX_PTR );

#define OMX_ALLOC(size) __omx_alloc(size)
#define OMX_INT_ALLOC(size) __omx_int_alloc(size)
#define OMX_FREE(ptr)   __omx_free(ptr)

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* OMX_TIIMAGE_H */
