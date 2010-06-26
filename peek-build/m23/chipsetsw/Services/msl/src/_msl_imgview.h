/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file msl_imgview.h
 *
 * This contains all of the types and functions specific to the
 * MSL Camera use-case pipeline.
 *
 * The application layer should not include this file directly; 
 *
 * \chipsetsw\services\msl\inc
 *
 * Rev 0.1
 */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 11-March-2006 sowmya Priya: Initial Release
 *! 
 *!
 * ========================================================================= */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSL_IMGVIEW_H
#define _MSL_IMGVIEW_H

/******************************************************************************
 * Includes
 ******************************************************************************/
/* OMX_Image common header file*/
#include "omx_types.h"
#include "omx_core.h"
#include "omx_image.h"
#include "OMX_camdcomponent.h"
#include "omx_imgcomponent.h"
#include "omx_camdcomponent.h"
#include "omx_sslcomponent.h"
#include "omx_tiimage.h"
#include "_msl_api.h"
#include "msl_imgview.h"


#ifndef WIN32
#include "typedefs.h"
#include "vsi.h"
#include "ffs/ffs_api.h"
#include "rfs/rfs_api.h"   
#else
#include "vsi_adapt.h"
#endif

/******************************************************************************
 * defines
 ******************************************************************************/


/******************************************************************************
 * enum types
 ******************************************************************************/
/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGVIEW_STATE : Different states that IMGVIEW can be in
 */
/*---------------------------------------------------------------------------*/
typedef enum MSL_IMGVIEW_STATE
{
    MSL_IMGVIEW_STATE_NONE,
    MSL_IMGVIEW_STATE_INIT,
    MSL_IMGVIEW_STATE_VIEW,
    MSL_IMGVIEW_STATE_PAUSE,
    MSL_IMGVIEW_STATE_DEINIT
} MSL_IMGVIEW_STATE;


/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGVIEW_STATUS
 */
/*---------------------------------------------------------------------------*/
typedef struct MSLVIEW_PRIV_STRUCT
{
    OMX_SSL_PLANE_CONFIGTYPE                    tOMXSSLConfig;
    OMX_TIIMAGE_OVERLAY_PARAMTYPE               tOMXOverlayConfig;
    OMX_TIIMAGE_RESCALE_PARAMTYPE               tOMXRescaleConfig;
    OMX_TIIMAGE_ROTATE_PARAMTYPE                tOMXRotateConfig;
    OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE       tOMXColorConvConfig;
    OMX_TIIMAGE_DECODE_PARAMTYPE                tOMXDecodeConfig;
    OMX_TIIMAGE_DECODE_IMAGEINFOTYPE            tImgInfo;                
    MSL_CALLBACK                                tMSLCallBack;
    MSL_U8                                      sInputJPEGFileName[MSL_MAXFILENAME_LENGTH];        
    MSL_FILETYPE                                tDecodeFileType;
    MSL_IMGVIEW_STATE                           tImgViewState;        

    OMX_CALLBACKTYPE                            tOMXCallbacks;

    OMX_HANDLETYPE                              hOMXDec;
    OMX_HANDLETYPE                              hOMXRescale;
    OMX_HANDLETYPE                              hOMXRotate;
    OMX_HANDLETYPE                              hOMXOverlay;
    OMX_HANDLETYPE                              hOMXColorConv;
    OMX_HANDLETYPE                              *ahOMXSSL;  

    OMX_BUFFERHEADERTYPE                        *pImgProcBuffHeader0;
    OMX_BUFFERHEADERTYPE                        *pImgProcBuffHeader1;

    OMX_U8*                                     pBuff0;
    OMX_U8*                                     pBuff1;

    MSL_U32                          nSemCaller;
    MSL_U32                          nSemHandle;

    MSL_BOOL                                    bDisplayUsedUp;
    MSL_BOOL                           bViewInProgress;
    MSL_U16                                     unPreviewCount; 

}MSL_IMGVIEW_PRIV_STRUCT;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
/**
 * MSLIMgView_StartDecoder
 *
 * @ starts the decoder configurations */

MSL_IMGVIEW_STATUS MSL_ImgView_StartDecoder (MSL_IMGVIEW_PRIV_STRUCT *_hIMGView);

/**
 * MSL_IMgView_OMXFillbufferDone
 *
 * @see fillbuffer call back for omx components  */
OMX_ERRORTYPE MSL_ImgView_OMXFillbufferDone(OMX_HANDLETYPE hComponent , 
        OMX_PTR pAppData,
        OMX_BUFFERHEADERTYPE* pBuffer);

/**
 * MSL_IMgView_OMXEmptybufferDone
 *
 * @see emptybuffer done callback for omx components */
OMX_ERRORTYPE MSL_ImgView_OMXEmptybufferDone(OMX_HANDLETYPE hComponent , 
        OMX_PTR pAppData, 
        OMX_BUFFERHEADERTYPE* pBuffer);

/**
 * OMX_ERRORTYPE MSL_IMgView_OMXEventHandle
 *
 * @see event handler for omx component */
OMX_ERRORTYPE MSL_ImgView_OMXEventHandle(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,               
        OMX_EVENTTYPE eEvent,    
        OMX_U32 nData1,                            
        OMX_U32 nData2,            
        OMX_PTR pEventData);

MSL_COLOR_FORMATTYPE __OMX2MSLColorType (OMX_TICOLOR_FORMATTYPE);
    
#endif /* _MSL_IMGVIEW_H */
#ifdef __cplusplus
} /* extern "C" */
#endif

