/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file msl_api.h
 *
 * This contains common interface for MSL layer
 *
 * \chipsetsw\services\msl\inc
 *
 * Rev 0.1
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSL_API_H
#define MSL_API_H
//in single buffer mode, the memory requirements are reduced
//#define MSL_SINGLEBUFFERMODE
#if CAM_SENSOR == 1
#define MSL_1_3MP_MODE
	#define MSL_SINGLEBUFFERMODE
#endif
#define DISPLAY_WIDTH 176
#define DISPLAY_HEIGHT 220
#define DISPLAY_IMAGE_WIDTH 176
#define DISPLAY_IMAGE_HEIGHT 144

#ifdef MSL_1_3MP_MODE
 #define SNAPSHOT_WIDTH 1280 
 #define SNAPSHOT_HEIGHT 1024
 #define MAX_ENCODED_BUFFER_SIZE 250000
 #define MAX_BURSTCOUNT 4
#else
 #define SNAPSHOT_WIDTH 640
 #define SNAPSHOT_HEIGHT 480
 #define MAX_ENCODED_BUFFER_SIZE 150000
  #ifdef MSL_SINGLEBUFFERMODE
    #define MAX_BURSTCOUNT 1
  #else
 #define MAX_BURSTCOUNT 4
#endif
#endif

#define BITSPERPIXEL_DISPLAY 2
#define BITSPERPIXEL_IMAGE 2
#define THUMBNAIL_WIDTH 48
#define THUMBNAIL_HEIGHT 48

/******************************************************************************
 * Includes
 ******************************************************************************/

/******************************************************************************
 * defines
 ******************************************************************************/

#define MSL_BASE_STATUS_ENUMS(_UCP_) \
/* info messages */ \
MSL##_UCP_##STATUS_OK = 0, \
MSL##_UCP_##STATUS_EOS, \
/* generic messages */ \
MSL##_UCP_##ERROR_UNKNOWN, \
MSL##_UCP_##ERROR_NOT_IMPLEMENTED, \
/* general errors */ \
MSL##_UCP_##ERROR_INVALID_STATE, \
/* bad arguments */ \
MSL##_UCP_##ERROR_INVALID_ARGUMENT, \
MSL##_UCP_##ERROR_INVALID_UCP, \
MSL##_UCP_##ERROR_INVALID_HANDLE, \
/* resource errors, like memory */ \
MSL##_UCP_##ERROR_NOMEMORY, \
/* data errors */ \
MSL##_UCP_##ERROR_BAD_STREAM, \
MSL##_UCP_##ERROR_IOREAD, \
MSL##_UCP_##ERROR_IOWRITE, \
MSL##_UCP_##ERROR_NOSPACE, \
/* marker for child pipelines */ \
MSL##_UCP_##BASE_LAST_COMMON

/******************************************************************************
 * Enum Types
 ******************************************************************************/
/** generic handle for MSL instances*/
typedef  void*              MSL_HANDLE;

/** void data type*/
typedef  void               MSL_VOID;

/** MSL_U8 is an 8 bit unsigned quantity that is byte aligned */
typedef unsigned char       MSL_U8;

/** MSL_S8 is an 8 bit signed quantity that is byte aligned */
typedef signed char         MSL_S8;

/** MSL_U16 is a 16 bit unsigned quantity that is 16 bit word aligned */
typedef unsigned short      MSL_U16;

/** MSL_S16 is a 16 bit signed quantity that is 16 bit word aligned */
typedef signed short        MSL_S16;

/** MSL_U32 is a 32 bit unsigned quantity that is 32 bit word aligned */
typedef unsigned long       MSL_U32;

/** MSL_S32 is a 32 bit signed quantity that is 32 bit word aligned */
typedef signed long         MSL_S32;

/* MSL string type */
typedef char*               MSL_STRING;

/* MSL string type */
typedef int                 MSL_STATUS;

/* define bool type */
typedef enum MSL_BOOL 
{
    MSL_FALSE = 0,
    MSL_TRUE = !MSL_FALSE
} MSL_BOOL; 

/*---------------------------------------------------------------------------*/
/*
 * MSL_CONFIG_INDEXTYPES
* Contains the configuration index types different msl get and set params
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_CALLBACKSET_CONFIGINDEX,
    MSL_DISPLAY_CONFIGINDEX,
    MSL_CAMERA_CONFIGINDEX,
    MSL_OVERLAY_CONFIGINDEX,    
    MSL_BURSTCOUNT_CONFIGINDEX,
    MSL_ENCFILE_CONFIGINDEX,
    MSL_DECFILE_CONFIGINDEX,
    MSL_ZOOM_CONFIGINDEX,
    MSL_RESCALE_CONFIGINDEX,
    MSL_ROTATE_CONFIGINDEX,
    MSL_SEPIAEFFECT_CONFIGINDEX,
    MSL_GRAYEFFECT_CONFIGINDEX,
    MSL_CROPWINDOW_CONFIGINDEX,
    MSL_ENCQUALITY_CONFIGINDEX,
    MSL_IMGINFO_CONFIGINDEX,
    MSL_MIRRORING_CONFIGINDEX,
    MSL_SAVESS_CONFIGINDEX
} MSL_INDEXTYPES;

/*---------------------------------------------------------------------------*/
/*
 * MSL_FILETYPE
 : File type enums.
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_FILETYPE_FFS,         /* FFS file type */
    MSL_FILETYPE_RFS         /* RFS file type */
} MSL_FILETYPE;


/*---------------------------------------------------------------------------*/
/*
 * MSL_UCPTYPE
 : MSL use case pipeline types.
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_UCP_IMGCAP,         /* Camera Application */
    MSL_UCP_IMGTHMB,        /* Thumbnail Generation */
    MSL_UCP_IMGVIEW        /* Image Capture */
} MSL_UCPTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_CMDTYPE
 : MSL use case pipeline types.
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_CMD_VIEWFINDER,
    MSL_CMD_SNAPSHOT,
	MSL_CMD_PREVIEWDONE,
    MSL_CMD_GENERATE,
    MSL_CMD_VIEW,
    MSL_CMD_PAUSE,
    MSL_CMD_INIT,
    MSL_CMD_DEINIT 
} MSL_CMDTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_OVERLAY_MODE
 : Allowed camera modes
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_OVERLAYMODE_NOOVERLAY,
    MSL_OVERLAYMODE_OVERLAP,
    MSL_OVERLAYMODE_COLORKEY,
    MSL_OVERLAYMODE_ALPHABLENDING,
    MSL_OVERLAYMODE_ALPHABLENDINGANDCOLORKEY
} MSL_OVERLAY_MODE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_CAMERA_MODE
 : Allowed camera modes
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_CAMERAMODE_VF,
    MSL_CAMERAMODE_SS
} MSL_CAMERA_MODE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMG_ROTATETYPE
 : Allowed rotate types
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_ROTATE_0,
    MSL_ROTATE_90,
    MSL_ROTATE_180,
    MSL_ROTATE_270    
} MSL_IMG_ROTATETYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMG_EFFECTTYPE
 : Allowed effect types
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{
    MSL_EFFECT_UNUSED,
    MSL_EFFECT_SEPIAEFFECT,
    MSL_EFFECT_GRAYEFFECT,
    MSL_EFFECT_EFFECTMAX   
} MSL_IMG_EFFECTTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_COLOR_FORMATTYPE
 : Defines the color format types supported by MSL
 */
/*---------------------------------------------------------------------------*/
typedef enum 
{    
    /* colour format types*/
     MSL_COLOR_YUYV,
     MSL_COLOR_RGB565,
     MSL_COLOR_YUV444,
     MSL_COLOR_YUV420,
     MSL_COLOR_RGB444,
     MSL_COLOR_MONOCHROME     
}MSL_COLOR_FORMATTYPE;     

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMG_WINDOWTYPE
 : Defines image window parameters
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_IMG_WINDOWTYPE
{
    MSL_U16                     nImgXOffset;
    MSL_U16                     nImgYOffset;
    MSL_U16                     nImgCropWidth;
    MSL_U16                     nImgCropHeight;
} MSL_IMG_WINDOWTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMG_OVERLAYCONFIG
 : Defines configuration info for overlay
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_IMG_OVERLAYCONFIG
{
    MSL_OVERLAY_MODE            tOverlayMode;
    MSL_U16                     nImgWidth;
    MSL_U16                     nImgHeight;
    MSL_U16                     nOverlayXOffset;
    MSL_U16                     nOverlayYOffset;
    MSL_U16                     nTransparencyColor;
    MSL_U16                     nAlpha;
    MSL_VOID                    *pOverlayBuff;
}MSL_IMG_OVERLAYCONFIG;

/*---------------------------------------------------------------------------*/
/*
 * MSL_CAM_CONFIGTYPE : 
 * defines the camera configuration structure
 * @params tCaptureMode
 * @params tImgSizeType
 * @params unImgWidth
 * @params unImgHeight
 * @params tImgFormat
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_CAM_CONFIGTYPE
{
    MSL_COLOR_FORMATTYPE        tImgFormat;
    MSL_U16                     unImgWidth;
    MSL_U16                     unImgHeight;
    MSL_CAMERA_MODE             tMode;
}MSL_CAM_CONFIGTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_DISPLAY_CONFIGTYPE
 : defines display configuration structure
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_DISPLAY_CONFIGTYPE
{
    MSL_COLOR_FORMATTYPE        tImgFormat;
    MSL_U16                     unDisplayImgWidth;
    MSL_U16                     unDisplayImgHeight;    
    MSL_U16                     unDisplayXOffset;
    MSL_U16                     unDisplayYOffset;    
}MSL_DISPLAY_CONFIGTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_RESCALE_CONFIGTYPE
 : defines file details
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_RESCALE_CONFIGTYPE
{
    MSL_U16                     unRescaledImgWidth;
    MSL_U16                     unRescaledImgHeight;
}MSL_RESCALE_CONFIGTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_FILE_CONFIGTYPE
 : defines file details
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_FILE_CONFIGTYPE
{
    MSL_STRING                  sFileName;
    MSL_FILETYPE                tFileType;
}MSL_FILE_CONFIGTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGINFO_CONFIGTYPE
 : defines image details
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_IMGINFO_CONFIGTYPE
{
    MSL_U16                     nExtendedImgWidth;
    MSL_U16                     nExtendedImgHeight;
    MSL_U16                     nActualImgWidth;
    MSL_U16                     nActualImgHeight;
    MSL_COLOR_FORMATTYPE        tColorFormat;
}MSL_IMGINFO_CONFIGTYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_GLOBALFEATURE_CONFIGTYPE
 * Configuration structure to configuration MSL's. Note that these parameters
 * determine the memory and MIPs figures.
 *
 * @params nMaxImgWidth: Max Image width that needs to be supported by MSL.
 * Default value is 640.
 *
 * @params nMaxImgHeight: Max Image height that needs to be supported by MSL.
 * Default value is 480
 *
 * @params nMaxBurstCount: Maximum burst picture mode required. Default value is
 * set to 5.
 *
 * @param nMaxSimultaneousImgPreviews: Maximum simultaneous images that needs to be
 * displayed in the imgview pipeline.
 *
 * @param nMaxCompressedImageSize: Maximum size of the compressed images. The 
 * defaul value is set to 100KBytes.
 *
 * @param b24BitFormatSupport: Boolean flag to decide if 24 bit formats (yuv444,
 * and rgb444) needs to be supported. 
 */
/*---------------------------------------------------------------------------*/
typedef struct MSL_GLOBALFEATURE_CONFIGTYPE
{
    MSL_U16                     nMaxImgWidth;
    MSL_U16                     nMaxImgHeight;
    MSL_U16                     nMaxBurstCount;
    MSL_U16                     nMaxSimultaneousImgPreviews;
    MSL_U32                     nMaxCompressedImageSize;
    MSL_BOOL                    b24BitFormatSupport;    
    MSL_BOOL                    bSingleBufferMode;
}MSL_GLOBALFEATURE_CONFIGTYPE;
/******************************************************************************
 * function prototypes
 ******************************************************************************/

//Callback for all the UCPS
typedef MSL_VOID (* MSL_CALLBACK) (MSL_HANDLE hMSL, MSL_UCPTYPE tUCPType, MSL_CMDTYPE tCMd, MSL_STATUS tStatus);

//MSL configuration function. This API needs to be called to override default MSL configuration for image dimensions
//and other features.
MSL_VOID MSL_SetGloablConfiguration(MSL_GLOBALFEATURE_CONFIGTYPE *tGlobalFeature);

#endif /* MSL_API_H */

#ifdef __cplusplus
} /* extern "C" */
#endif


