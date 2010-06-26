/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file msl_imgcap.h
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
 *! 16-March-2006 Narendran M R: code rewrite to make the flow fully asynchronous
 *!
 * ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSL_IMGCAP_H
#define _MSL_IMGCAP_H

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "_msl_api.h"
#include "msl_imgcap.h"
#include "rfs/rfs_message.h"
    
/******************************************************************************
 * defines
 ******************************************************************************/
    
/******************************************************************************
 * enum types
 ******************************************************************************/

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGCAP_STATE : Different states that IMGCAP can be in
 */
/*---------------------------------------------------------------------------*/    
typedef enum 
{
    MSL_IMGCAP_STATE_NONE,
    MSL_IMGCAP_STATE_INIT,
    MSL_IMGCAP_STATE_VF,
    MSL_IMGCAP_STATE_SS,
    MSL_IMGCAP_STATE_NONE2SS,
    MSL_IMGCAP_STATE_SS2NONE,
    MSL_IMGCAP_STATE_PAUSE,
    MSL_IMGCAP_STATE_DEINIT        
    
}MSL_IMGCAP_STATE_TYPE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGCAP_STATUS
 */
/*---------------------------------------------------------------------------*/
typedef struct MSLIMG_PRIV_STRUCT
{
    /* omx configuration types*/
    OMX_CAM_CONFIGTYPE                          tOMXCameraConfig;    
    OMX_SSL_PLANE_CONFIGTYPE                    tOMXSSLConfig;
    OMX_TIIMAGE_OVERLAY_PARAMTYPE               tOMXOverlayConfig;
    OMX_TIIMAGE_RESCALE_PARAMTYPE               tOMXRescaleConfig;    
    OMX_TIIMAGE_ROTATE_PARAMTYPE                tOMXRotateConfig;
    OMX_TIIMAGE_ENCODE_PARAMTYPE                tOMXEncodeConfig;
    OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE       tOMXColorConvConfig;
    OMX_TIIMAGE_EFFECT_PARAMTYPE                tOMXEffectConfig;
    OMX_CAM_FEATURETYPE                          tOMXCamFeature;
    
    /* Application-registered callback. */
    MSL_CALLBACK                                tMSLCallBack;  
    /* encode file name */
    MSL_U8                                      sEncodeFileName[MSL_MAXFILENAME_LENGTH];  
    MSL_FILETYPE                                tEncodeFileType;
    
    /* OMX callback functions */
    OMX_CALLBACKTYPE                            tOMXCallbacks;
    
    /* current STATE of MSL */
    MSL_IMGCAP_STATE_TYPE                       tIMGCAPState;
    
    /* OMX handles */
    OMX_HANDLETYPE                        hOMXCAM;
    OMX_HANDLETYPE                                    hOMXSSL;
    OMX_HANDLETYPE                                    hOMXRescale;
    OMX_HANDLETYPE                                    hOMXOverlay;
    OMX_HANDLETYPE                                    hOMXRotate;
    OMX_HANDLETYPE                                    hOMXEnc;
    OMX_HANDLETYPE                                    hOMXColorConv;
    OMX_HANDLETYPE                                    hOMXEffect;
    
    /* burst count variable */
    MSL_U16                                     nBurstCount;
    MSL_U16                                     nCurrentBurstCount;
    MSL_BOOL                                    bPreviewCycle;

    /* DSA plane */
    OMX_PTR                                     pSSLDSAPlane;
    
    OMX_BUFFERHEADERTYPE                        *pCameraBuffHeader0;
    OMX_BUFFERHEADERTYPE                        *pCameraBuffHeader1;
    OMX_BUFFERHEADERTYPE                        *pIMGBuffHeader0;
    OMX_BUFFERHEADERTYPE                        *pIMGBuffHeader1;
    OMX_BUFFERHEADERTYPE                        *pEncBuffHeader;
    OMX_PTR                                     pCameraBuff;      
    OMX_PTR                                     *apEncOutBuff;
    OMX_U32                                     *aEncodeBuffSize;
    OMX_PTR                                     pIMGBuff;   
    MSL_BOOL                                    bErrorStatus;
        /* Caller ID*/
    MSL_U32                                     nSemCaller;

    /* Semaphore Handle */
    MSL_U32                                     nSemHandle;
	MSL_BOOL                                    bEncComplete; 
	MSL_BOOL                                    bAutoSave;
	MSL_STATUS                                  nErrorCode; 
}MSL_IMGCAP_PRIV_STRUCT;
        
/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
/**
 * MSLIMgCapt_SaveEncodedBuffers
 *
 * @see saves all encoded buffers. In burst mode there could be more than one buffer */
MSL_STATUS MSL_ImgCap_SaveEncodedBuffers(MSL_HANDLE _hMSLIMGCAP);
void MSL_ImgCap_OpenFFSEventHandle(OMX_PTR );
void MSL_ImgCap_OpenRFSEventHandle( OMX_PTR );
/**
 * MSLIMgCapt_StartEncoder
 *
 * @see does all initialization for encoding */
MSL_STATUS MSL_ImgCap_StartEncoder (MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP);

/**
 * MSL_IMgCapt_OMXFillbufferDone
 *
 * @see fillbuffer call back for omx components  */
OMX_ERRORTYPE MSL_ImgCap_OMXFillbufferDone(OMX_HANDLETYPE hComponent , 
        OMX_PTR pAppData,
        OMX_BUFFERHEADERTYPE* pBuffer);

/**
 * MSL_IMgCapt_OMXEmptybufferDone
 *
 * @see fillbuffer done callback for omx components */
OMX_ERRORTYPE MSL_ImgCap_OMXEmptybufferDone(OMX_HANDLETYPE hComponent , 
        OMX_PTR pAppData, 
        OMX_BUFFERHEADERTYPE* pBuffer);

/**
 * OMX_ERRORTYPE MSL_IMgCapt_OMXEventHandle
 *
 * @see event handler for omx component */
OMX_ERRORTYPE MSL_ImgCap_OMXEventHandle(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,               
        OMX_EVENTTYPE eEvent,    
        OMX_U32 nData1,                            
        OMX_U32 nData2,            
        OMX_PTR pEventData);

#endif /* _MSL_IMGCAP_H */
#ifdef __cplusplus
} /* extern "C" */
#endif

