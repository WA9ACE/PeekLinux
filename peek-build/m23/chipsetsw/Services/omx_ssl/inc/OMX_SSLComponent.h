/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */

/** OMX_SSLComponent.h
 *  The OMX_SSLComponent header file contains the definitions used to define
 *  the public interface of the SSL.  This header file is intended to
 *  be used by both the application and the component.
 */

#ifndef OMX_SSLComponent_h
#define OMX_SSLComponent_h

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
#include <omx_tiimage.h>

	/*
	 * OMX_SSL_CMDTYPE: This enumeration lists the commands that are exchanged between the SSL Client and the 
	 * SSL Core task. Note that this is defined by the SSL Core task and there is no clash of enum values with the
	 * Application task. The return path is via Callbacks only.
	 */
	typedef enum OMX_SSL_CMDTYPE{
		OMX_SSL_CMD_GETNUMDISPLAYS = 0,
		OMX_SSL_CMD_GETDISPINFO,
		OMX_SSL_CMD_GETDISPPROPERTIES,
		OMX_SSL_CMD_GETDISPMAXWIDTH,
		OMX_SSL_CMD_GETDISPMAXHEIGHT,
		OMX_SSL_CMD_GETDISPDATAFORMAT,
		OMX_SSL_CMD_GETDISPDITHSUPPORT,
		OMX_SSL_CMD_GETDISPORIENTAION,
		OMX_SSL_CMD_GETDISPPOWERSTATUS,
		OMX_SSL_CMD_GETDISPBACKLITCONFIG,
		OMX_SSL_CMD_GETDISPVENDOR,
		OMX_SSL_CMD_SETDISPPROPERTIES,
		OMX_SSL_CMD_SETDISPDATAFORMAT,
		OMX_SSL_CMD_SETDISPDITHER,
		OMX_SSL_CMD_SETDISPORIENTATION,
		OMX_SSL_CMD_SETDISPPOWER,
		OMX_SSL_CMD_SETDISPBACKLITCONFIG,			
		OMX_SSL_CMD_SETALPHA,
		OMX_SSL_CMD_SETALWAYSONTOP,
		OMX_SSL_CMD_SETXOFFSET,
		OMX_SSL_CMD_SETYOFFSET,
		OMX_SSL_CMD_SETXLEN,
		OMX_SSL_CMD_SETYLEN,
		OMX_SSL_CMD_SETDATAFRMT,
		OMX_SSL_CMD_SETACTIVEDISPID,
		OMX_SSL_CMD_SETPLANEPROPERTIES,
		OMX_SSL_CMD_CREATEPLANE,
		OMX_SSL_CMD_ACTIVATEPLANE,
		OMX_SSL_CMD_SUSPENDPLANE,
		OMX_SSL_CMD_DESTROYPLANE,
		OMX_SSL_CMD_DEACTIVATEPLANE,
		OMX_SSL_CMD_RESUMEPLANE,
		OMX_SSL_CMD_SETINBUFF,
		OMX_SSL_CMD_SETDSAFLAG,
		OMX_SSL_CMD_INVALID = 0xFFFF
	}OMX_SSL_CMDTYPE;

	/*
	 * Enemuration Type: OMX_SSL_DISPLAY_ORIENTATIONTYPE
	 * This is an enumeration that is used to set the orientation of the display.
	 */
	typedef enum OMX_SSL_DISPLAY_ORIENTATIONTYPE{
		OMX_SSL_ORIENTATION_PORTRAIT = 0,
		OMX_SSL_ORIENTATION_LANDSCAPE
	}OMX_SSL_DISPLAY_ORIENTATIONTYPE;

	/* 
	 * Enumeration Type: OMX_SSL_PLANE_ALPHATYPE
	 * Enumeration data type that is used to provide for the plane Alpha.
	 * This enumeration lists only those that are supported by the codec.
	 */
	typedef enum OMX_SSL_PLANE_ALPHATYPE{
		OMX_SSL_ALPHA0 = 0,
		OMX_SSL_ALPHA0_50 = 50,
		OMX_SSL_ALPHA0_75 = 75,
		OMX_SSL_ALPHA0_1 = 100
	}OMX_SSL_PLANE_ALPHATYPE;

	/*
	 * Structure Name: OMX_SSL_DISPLAY_INFOTYPE
	 * This structure returns the the device id and vendor name for each of the devices. The user is
	 * expected to query for the number of available displays and allocate enough memory to accomodate
	 * the device IDs and the Vendor Names.
	 * VendorNames: This parameter gives the names of the vendors separated by a ','.This is a null 
	 *              terminated string.
	 * DeviceIDs:   This parameter gives the Device IDs of the display devices available separated by ','. 
	 *              This is a Null terminated string.
	 */
	typedef struct OMX_SSL_DISPLAY_INFOTYPE{
		OMX_STRING cVendorNames;
		OMX_STRING cDeviceIDs;
	}OMX_SSL_DISPLAY_INFOTYPE;

	/*
	 * Structure Name: OMX_SSL_DISPLAY_QUERYTYPE
	 * This structure is used to Query a Physical Display for its parameters. For any query in this
	 * structure, a displayid needs to be sent as well. This structure is used with the GetParameter
	 * function of the SSL Client.
	 * cVendorName : String that gives the vendor name for the current display
	 * bDisplayPowerStatus : Flag that indicates if the display is enabled.	  
	 * bIsActive : Flag that indicates whether the current display is active for the current plane 
	 * nDisplayMaxWidth : Maximum Width of the Display
	 * tDisplayOrientation : Enumeration that indicates the current display orientation 
	 * nDisplayMaxHeight : Maximum Height of the Display
	 * tDisplayDataFormats : Bit mapped value that indicates the display formats supported
	 * bDisplayDitherSupport : Flag that indicates is Dithering is supported
	 * tDisplayBacklightConfig : Structure that gives the backlight configuration
	 * nNumOfDisplays : Number of Available Displays
	 */
	typedef struct OMX_SSL_DISPLAY_QUERYTYPE{	
		OMX_STRING cVendorName;		
		OMX_BOOL bDisplayPowerStatus;		
		OMX_BOOL bIsActive;		
		OMX_U32  nDisplayMaxWidth;
		OMX_U32  nDisplayMaxHeight;
		OMX_SSL_DISPLAY_ORIENTATIONTYPE tDisplayOrientation;		
		OMX_U32 tDisplayDataFormats;
		OMX_BOOL bDisplayDitherSupport;
		OMX_CONFIG_BACKLIGHTTYPE tDisplayBacklightConfig;
	}OMX_SSL_DISPLAY_QUERYTYPE;

	/*
	 * Structure Name: OMX_SSL_DISPLAY_CONFIGTYPE
	 * This structure is used to set the parameters for a Physical Display.
	 * This structure is used with the GetParameter function of the SSL Client.
	 * bEnable : Parameter that switches ON/OFF the display 
	 * DisplayFormat : Value that sets the current display format
	 * tOrientation  : The Orientation for the display
	 * DisplayBackLightConfig: Configuration Structure for the Backlight for the current Display
	 * DitherEnable : Flag that is used to set Dithering ON/OFF if supported
	 */
	typedef struct OMX_SSL_DISPLAY_CONFIGTYPE{
		OMX_BOOL bEnable;		
		OMX_TICOLOR_FORMATTYPE tDisplayFormat;
		OMX_SSL_DISPLAY_ORIENTATIONTYPE tOrientation;		
		OMX_CONFIG_BACKLIGHTTYPE tDisplayBackLightConfig;		
		OMX_BOOL bDitherEnable;
	}OMX_SSL_DISPLAY_CONFIGTYPE;

/*
 *  PLANE BLEND-TYPE ENUMERATION
 */
typedef enum
{
	OMX_SSL_PLANEBLEND_OVERLAP,
	OMX_SSL_PLANEBLEND_OVERLAY,
	OMX_SSL_PLANEBLEND_ALPHA,
	OMX_SSL_PLANEBLEND_OSD
}OMX_SSL_PLANE_BLENDTYPE;

	/*
	 * Sructure Name: OMX_SSL_PLANE_QUERYTYPE
	 * This is plane configuration query structure. This structure returns the currently configured plane parameters.
	 * Parameters and their use are as follows:
	 * nXOffset:	X Axis Offset for this plane in the Framebuffer with (0,0) as the top left.
	 * nYOffset:	Y Axis Offset for this plane in the Framebuffer with (0,0) as the top left.
	 * nXLen:	Width of the plane.
	 * nYLen:	Height of the plane.
	 * tAlpha:	AlphaBlending Parameter for this plane.
	 * tPlaneImgFormat : Data format for the current plane.
	 * bActive:	Flag that indicates if the plane is active; default=0 = inactive.
	 * bAlwaysOnTop:Flag that indicate if this is an 'Always On Top' Plane.
	 * bSuspended:	Flag that is set if the OMX Component goes to Pause. Suspended Buffers have their configuration updated,
	 *              retain their place in the queue, but not considered for a refresh.
	 * bDSAPlane:	Flag that indicates if this plane is a DSA plane. Only if this is TRUE, the Frame Buffer Pointer is returned.
	 * pSSLDSAFramePtr:The framebuffer pointer. This is valid only if the bDSAPlane is TRUE.
	 * nActiveDispID : The plane's active display ID. This feature provides for DYNAMIC DISPLAY CHOICE.
	 */
	typedef struct OMX_SSL_PLANE_QUERYTYPE{
		OMX_U32 nXOffset;
		OMX_U32 nYOffset;
		OMX_U32 nXLen;
		OMX_U32 nYLen;
		OMX_SSL_PLANE_ALPHATYPE tAlpha;
		OMX_TICOLOR_FORMATTYPE tPlaneImgFormat;
		OMX_BOOL bActive;
		OMX_BOOL bAlwaysOnTop;
		OMX_BOOL bSuspended;
		OMX_BOOL bDSAPlane;
		OMX_BYTE pSSLDSAFramePtr;
		OMX_U32 nActiveDispID;
		/* Blend-Related properties */
		OMX_SSL_PLANE_BLENDTYPE tPlaneBlendType;
		OMX_U8 unPlaneAlpha;//Used if Alpha-Blending is chosen
		OMX_U16 unPlaneTransparency;//Used for Overlay		
		/* Deferred Refresh */
		OMX_BOOL bDeferredUpdate;		
	}OMX_SSL_PLANE_QUERYTYPE;

	/*
	 * Structure Name: OMX_SSL_PLANE_CONFIGTYPE
	 * This structure is used to Configure a plane and can be used with the SetConfig function call.
	 * The members of this structure and their utility are as follows:
	 * nXOffset:	X Axis Offset for this plane in the Framebuffer with (0,0) as the top left.
	 * nYOffset:	Y Axis Offset for this plane in the Framebuffer with (0,0) as the top left.
	 * nXLen:	Width of the plane.
	 * nYLen:	Height of the plane.
	 * tAlpha:	AlphaBlending Parameter for this plane.
	 * tPlaneImgFormat : The plane's data format.
	 * bAlwaysOnTop: Flag that indicate if this is an 'Always On Top' Plane.
	 * bDSAPlane	Flag: that makes this plane region accessible using DSA. Refer to Appendix for more information.
	 */
	typedef struct OMX_SSL_PLANE_CONFIGTYPE{
		OMX_U32  nXOffset;
		OMX_U32  nYOffset;
		OMX_U32 nXLen;
		OMX_U32 nYLen;
		OMX_SSL_PLANE_ALPHATYPE tAlpha;
		OMX_TICOLOR_FORMATTYPE tPlaneImgFormat;
		OMX_BOOL bAlwaysOnTop;
		OMX_BOOL bDSAPlane;
		OMX_U32 nActiveDispID;
		/* Blend-Related properties */
		OMX_SSL_PLANE_BLENDTYPE tPlaneBlendType;
		OMX_U8 unPlaneAlpha;//Used if Alpha-Blending is chosen
		OMX_U16 unPlaneTransparency;//Used for Overlay		
		/* Deferred Refresh */
		OMX_BOOL bDeferredUpdate;
	}OMX_SSL_PLANE_CONFIGTYPE ;

	/* OMX SSL COMPONENT NAME */
#define OMXSSL_COMP_NAME      "OMX.TI.IMAGE.SSL"

	/* Display formats supported - Bitmapped Integer */
#define OMX_SSL_DISPLAY_FORMATS_SUPPORTEDTYPE OMX_U32
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/* File EOF */
