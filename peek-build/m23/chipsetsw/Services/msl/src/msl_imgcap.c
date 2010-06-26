/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/* =========================================================================
*!
*! Revision History
*! ===================================
*! 11-March-2006 sowmya Priya: Initial Release
*! 16-March-2006 Narendran M R: code rewrite to make the flow fully asynchronous
*! 05-June-2006       Narendran M R: Optimization for performance improvement (triple buffering), traces
*!                                 and profiling stubs added to all msl functions.
*! 10-July-2006     Sowmya Priya: Fixed Burstmode, RFS flag issues
*! 24-July -2006    Narendran M R: Check for error Callbacks from camd and img.
*! 9-March-2007    Ramesh Anandhi  Add support for encoder output streaming
* ========================================================================= */


#if OMX_CAM_STATE

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdlib.h>
#include <string.h>
#include "_msl_imgcap.h"
#include "typedefs.h"
#include "vsi.h"
#include "os.h"
#include "frm_glob.h"
#include "rvm/rvm_api.h"
#include "rvm/rvm_use_id_list.h"
#include "tests/rtest/rtest_api.h"
#include "rfs/rfs_fm.h"
MSL_IMGCAP_PRIV_STRUCT *hMslImgCapComp;
T_WCHAR * sfilename_uc;
MSL_U8 nSavedCount;
T_RFS_FD hJPGFile;

#define MCU_MULTIPLE 122880
/* ========================================================================== */
/**
* @fn MSL_ImgCap_Create :Creates the pipeline.
*
* @param [in] hMSLIMGCAP
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Create (MSL_HANDLE* phMSLIMGCAP)
{
    MSL_U16                     i;
    MSL_U32                     ulSize;
    MSL_IMGCAP_PRIV_STRUCT      *_hMSLIMGCAP;
    MSL_IMGCAP_STATUS                  tRetVal = MSL_IMGCAP_STATUS_OK;
    MSL_U8                      cSemName[20];
    T_HANDLE entityHandle;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Create : Entry");
    MSL_PROFILESTUB("MSL_ImgCap_Create start");

    /* Memory allocation */
    _hMSLIMGCAP     = (MSL_IMGCAP_PRIV_STRUCT *)MSL_ALLOC(sizeof(MSL_IMGCAP_PRIV_STRUCT));
    MSL_EXIT_IF(_hMSLIMGCAP == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    *phMSLIMGCAP    = _hMSLIMGCAP;

    entityHandle = e_running[os_MyHandle()];
    ltoa((MSL_U32)_hMSLIMGCAP, (OMX_STRING)&cSemName[0]);
    _hMSLIMGCAP->nSemHandle = vsi_s_open(entityHandle, (OMX_STRING)cSemName, 1);
    _hMSLIMGCAP->nSemCaller = (MSL_U32) entityHandle;

    /* set omx callback functions */
    _hMSLIMGCAP->tOMXCallbacks.EventHandler             = MSL_ImgCap_OMXEventHandle;
    _hMSLIMGCAP->tOMXCallbacks.EmptyBufferDone          = MSL_ImgCap_OMXEmptybufferDone;
    _hMSLIMGCAP->tOMXCallbacks.FillBufferDone           = MSL_ImgCap_OMXFillbufferDone;

    /* initialize all OMX handle to NULL */
    _hMSLIMGCAP->hOMXCAM                                = NULL;
    _hMSLIMGCAP->hOMXSSL                                = NULL;
    _hMSLIMGCAP->hOMXRescale                            = NULL;
    _hMSLIMGCAP->hOMXRotate                             = NULL;
    _hMSLIMGCAP->hOMXOverlay                            = NULL;
    _hMSLIMGCAP->hOMXEnc                                = NULL;
    _hMSLIMGCAP->hOMXEffect                = NULL;

    /* initialize buffer pointers to NULL */
    _hMSLIMGCAP->pCameraBuffHeader0                     = NULL;
    _hMSLIMGCAP->pCameraBuffHeader1                     = NULL;
    _hMSLIMGCAP->pIMGBuffHeader0                        = NULL;
    _hMSLIMGCAP->pIMGBuffHeader1                        = NULL;
    _hMSLIMGCAP->pEncBuffHeader                         = NULL;

    _hMSLIMGCAP->bErrorStatus                           = MSL_FALSE;

    //allocate OMX_BUFFERHEADERTYPE for camera buffer header 0
    _hMSLIMGCAP->pCameraBuffHeader0 = (OMX_BUFFERHEADERTYPE *)MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGCAP->pCameraBuffHeader0 == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    //allocate OMX_BUFFERHEADERTYPE for camera buffer header 1
    _hMSLIMGCAP->pCameraBuffHeader1 = (OMX_BUFFERHEADERTYPE *)MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGCAP->pCameraBuffHeader1 == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    //allocate OMX_BUFFERHEADERTYPE for img buffer0
    _hMSLIMGCAP->pIMGBuffHeader0 = (OMX_BUFFERHEADERTYPE *) MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGCAP->pIMGBuffHeader0 == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    //allocate OMX_BUFFERHEADERTYPE for img buffer1
    _hMSLIMGCAP->pIMGBuffHeader1 = (OMX_BUFFERHEADERTYPE *) MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGCAP->pIMGBuffHeader1 == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    //allocate OMX_BUFFERHEADERTYPE for enc buff
    _hMSLIMGCAP->pEncBuffHeader = (OMX_BUFFERHEADERTYPE *) MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGCAP->pEncBuffHeader == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    //calculate camera buffer size
    ulSize = tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight;
    if(MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport)
    {
        ulSize= ulSize* 3;
    }
    else
    {
        ulSize= ulSize* 2;
    }

    //allocate buffer for VF/SS capture. Allocate for the worst case
    _hMSLIMGCAP->pCameraBuff = MSL_ALLOC(ulSize);
    MSL_EXIT_IF(_hMSLIMGCAP->pCameraBuff == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
    {
        //allocate separate buffer for img. Allocate for the worst case
        _hMSLIMGCAP->pIMGBuff = MSL_ALLOC(ulSize);
        MSL_EXIT_IF(_hMSLIMGCAP->pIMGBuff == NULL, MSL_IMGCAP_ERROR_NOMEMORY);
    }
    else
    {
        //allocate separate buffer for img. Allocate for the worst case
        _hMSLIMGCAP->pIMGBuff = MSL_ALLOC(DISPLAY_IMAGE_HEIGHT*DISPLAY_IMAGE_WIDTH*BITSPERPIXEL_IMAGE);
        MSL_EXIT_IF(_hMSLIMGCAP->pIMGBuff == NULL, MSL_IMGCAP_ERROR_NOMEMORY);
    }

    _hMSLIMGCAP->apEncOutBuff = (OMX_PTR *)MSL_ALLOC(tMSLGloablFeatures.nMaxBurstCount * sizeof(OMX_PTR));
    MSL_EXIT_IF(_hMSLIMGCAP->apEncOutBuff == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    /* init all encode burst buffers to NULL */
    for(i=0; i<tMSLGloablFeatures.nMaxBurstCount; i++)
    _hMSLIMGCAP->apEncOutBuff[i]                     = NULL;

    _hMSLIMGCAP->aEncodeBuffSize = (OMX_U32 *)MSL_ALLOC(tMSLGloablFeatures.nMaxBurstCount * sizeof(OMX_U32));
    MSL_EXIT_IF(_hMSLIMGCAP->aEncodeBuffSize == NULL, MSL_IMGCAP_ERROR_NOMEMORY);

    //allocate buffer for encoder output
    for(i=0; i<tMSLGloablFeatures.nMaxBurstCount; i++)
    {
        _hMSLIMGCAP->apEncOutBuff[i] = MSL_ALLOC(tMSLGloablFeatures.nMaxCompressedImageSize);
        MSL_EXIT_IF(_hMSLIMGCAP->apEncOutBuff[i] == NULL, MSL_IMGCAP_ERROR_NOMEMORY);
    }

    /* get handle to OMX IMG client for rescale*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXRescale,
    OMX_TIIMG_COMPONENT,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for overlay*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXOverlay,
    OMX_TIIMG_COMPONENT,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for rotate*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXRotate,
    OMX_TIIMG_COMPONENT,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for encode*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXEnc,
    OMX_TIIMG_COMPONENT,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for color convert */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXColorConv,
    OMX_TIIMG_COMPONENT,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for SEPIA effect */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXEffect,
    OMX_TIIMG_COMPONENT,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);

    /* get handle to OMX CAM client */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXCAM,
    OMXCAM_COMP_NAME,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_INVALID_HANDLE);
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetConfig(_hMSLIMGCAP->hOMXCAM,
     (OMX_INDEXTYPE)OMX_IndexConfigCameraFeatures,
    &_hMSLIMGCAP->tOMXCamFeature), MSL_IMGCAP_ERROR_INVALID_HANDLE);
    /* get handle to OMX SSL client */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGCAP->hOMXSSL,
    OMXSSL_COMP_NAME,
    _hMSLIMGCAP,
    &_hMSLIMGCAP->tOMXCallbacks
    ), MSL_IMGCAP_ERROR_NOMEMORY);
    //set mode to none
    _hMSLIMGCAP->tIMGCAPState                           = MSL_IMGCAP_STATE_NONE;

    _hMSLIMGCAP->tOMXEncodeConfig.nQualityFactor        = MSL_DEFAULT_ENCQUALITY;
    _hMSLIMGCAP->tOMXRescaleConfig.nZoomFactor          = MSL_ZOOM_NONE;
    _hMSLIMGCAP->tOMXOverlayConfig.pOverlayBuffer       = NULL;
    _hMSLIMGCAP->tOMXRotateConfig.tRotate               = OMX_TIIMAGE_ROTATE0;
    _hMSLIMGCAP->nBurstCount                            = 1;
    _hMSLIMGCAP->tOMXEffectConfig.tEffect               = OMX_TIIMAGE_EFFECTUNUSED;

    //do default initialization for some non-mandatory parameters
    _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nWidth   = 0;
    _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nHeight  = 0;
    _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nXOffset = 0;
    _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nYOffset = 0;
    _hMSLIMGCAP->tOMXCameraConfig.tMirrorType= OMX_MirrorNone;
    _hMSLIMGCAP->bAutoSave = MSL_TRUE;

EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Create : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn MSL_ImgCap_Init :Initializes the pipeline
*
* @param [in] hIMGCap
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Init(MSL_HANDLE hIMGCap)
{
    OMX_SSL_PLANE_CONFIGTYPE tOMXSSLConfig;
    OMX_TIIMAGE_RESCALE_PARAMTYPE tOMXRescaleConfig;

    MSL_IMGCAP_STATUS tRetVal = MSL_IMGCAP_STATUS_OK;
    // dereference pointer
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)hIMGCap;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Init : Entry");

    MSL_PROFILESTUB("MSL_ImgCap Initialization: Started");

    //set current burst count as zero
    _hMSLIMGCAP->nCurrentBurstCount                     = 0;

    //state if not equal to none give an error
    MSL_EXIT_IF(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE
    ,MSL_IMGCAP_ERROR_INVALID_STATE);
     // set dependent component parameters
    _hMSLIMGCAP->tOMXRescaleConfig.bInPlace                 = OMX_FALSE;
    _hMSLIMGCAP->tOMXRescaleConfig.nInputImageWidth         = _hMSLIMGCAP->tOMXCameraConfig.unImageWidth;
    _hMSLIMGCAP->tOMXRescaleConfig.nInputImageHeight        = _hMSLIMGCAP->tOMXCameraConfig.unImageHeight;
    _hMSLIMGCAP->tOMXRescaleConfig.tInputImageFormat        = _hMSLIMGCAP->tOMXCameraConfig.tImageFormat;
    _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth        = _hMSLIMGCAP->tOMXSSLConfig.nXLen;
    _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight       = _hMSLIMGCAP->tOMXSSLConfig.nYLen;


    if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
    {
        _hMSLIMGCAP->tOMXSSLConfig.bDSAPlane                = OMX_TRUE;
    }
    else
    {
        _hMSLIMGCAP->tOMXSSLConfig.bDSAPlane                = OMX_FALSE;
      #ifdef MSL_1_3MP_MODE
           if(_hMSLIMGCAP->tOMXCameraConfig.tImageSizeType == OMX_TIIMAGE_SXGA)
                tMSLGloablFeatures.bSingleBufferMode = MSL_TRUE;
           else
                tMSLGloablFeatures.bSingleBufferMode = MSL_FALSE;
       #endif
        //In SS mode, rescaler is first used for zoom only (no rescaling)
        _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth    = _hMSLIMGCAP->tOMXRescaleConfig.nInputImageWidth;
        _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight   = _hMSLIMGCAP->tOMXRescaleConfig.nInputImageHeight;

        //set color conversion parameters
        _hMSLIMGCAP->tOMXColorConvConfig.tOutputImageFormat     = OMX_TICOLOR_YCbYCr;
        if(tMSLGloablFeatures.bSingleBufferMode == MSL_TRUE)
        _hMSLIMGCAP->tOMXColorConvConfig.bInPlace           = OMX_TRUE;
        else
        _hMSLIMGCAP->tOMXColorConvConfig.bInPlace           = OMX_FALSE;

    }

    //work on local copy of rescale config
    memcpy(&tOMXRescaleConfig, &_hMSLIMGCAP->tOMXRescaleConfig, sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE));

    if(tOMXRescaleConfig.tCropWindow.nWidth  == 0)
    {
        tOMXRescaleConfig.tCropWindow.nWidth = tOMXRescaleConfig.nInputImageWidth -
        tOMXRescaleConfig.tCropWindow.nXOffset;
    }

    if(tOMXRescaleConfig.tCropWindow.nHeight == 0)
    {
        tOMXRescaleConfig.tCropWindow.nHeight = tOMXRescaleConfig.nInputImageHeight -
        tOMXRescaleConfig.tCropWindow.nYOffset;
    }

    if(_hMSLIMGCAP->tOMXCameraConfig.tImageFormat != _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat &&
            _hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
    {
        _hMSLIMGCAP->tOMXColorConvConfig.nSrcImageHeight = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
        _hMSLIMGCAP->tOMXColorConvConfig.nSrcImageWidth  = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
        _hMSLIMGCAP->tOMXColorConvConfig.tInputImageFormat = _hMSLIMGCAP->tOMXCameraConfig.tImageFormat;
        _hMSLIMGCAP->tOMXColorConvConfig.tOutputImageFormat = _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat;
        _hMSLIMGCAP->tOMXColorConvConfig.bInPlace = OMX_TRUE;
    }
    _hMSLIMGCAP->tOMXOverlayConfig.bInPlace             = OMX_TRUE;
    _hMSLIMGCAP->tOMXOverlayConfig.nSrcImageWidth       = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGCAP->tOMXOverlayConfig.nSrcImageHeight      = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGCAP->tOMXOverlayConfig.tSrcImageFormat      = _hMSLIMGCAP->tOMXRescaleConfig.tInputImageFormat;
    _hMSLIMGCAP->tOMXEffectConfig.bInPlace             =OMX_TRUE;
    _hMSLIMGCAP->tOMXEffectConfig.nImageHeight          = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGCAP->tOMXEffectConfig.nImageWidth           = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGCAP->tOMXEffectConfig.tInputImageFormat     = _hMSLIMGCAP->tOMXRescaleConfig.tInputImageFormat;
    _hMSLIMGCAP->tOMXEffectConfig.tEffect                       = OMX_TIIMAGE_EFFECTUNUSED;

    if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == SNAPSHOT_MODE
            && tMSLGloablFeatures.bSingleBufferMode == MSL_TRUE)
    _hMSLIMGCAP->tOMXRotateConfig.bInPlace              = OMX_TRUE;
    else
    _hMSLIMGCAP->tOMXRotateConfig.bInPlace              = OMX_FALSE;
    _hMSLIMGCAP->tOMXRotateConfig.nImageWidth           = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGCAP->tOMXRotateConfig.nImageHeight          = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
    if(_hMSLIMGCAP->tOMXCameraConfig.tImageFormat != _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat &&
            _hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
    _hMSLIMGCAP->tOMXRotateConfig.tInputImageFormat     = _hMSLIMGCAP->tOMXColorConvConfig.tOutputImageFormat;
    else
    _hMSLIMGCAP->tOMXRotateConfig.tInputImageFormat     = _hMSLIMGCAP->tOMXRescaleConfig.tInputImageFormat;
    if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
    _hMSLIMGCAP->tOMXRotateConfig.nStride           = DISPLAY_WIDTH;
    else {
        if(_hMSLIMGCAP->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE90||
                _hMSLIMGCAP->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE270 )
        _hMSLIMGCAP->tOMXRotateConfig.nStride              = _hMSLIMGCAP->tOMXRotateConfig.nImageHeight;
        else
        _hMSLIMGCAP->tOMXRotateConfig.nStride              = _hMSLIMGCAP->tOMXRotateConfig.nImageWidth;
    }

    _hMSLIMGCAP->tOMXEncodeConfig.bInsertHeader         = OMX_TRUE;
    _hMSLIMGCAP->tOMXEncodeConfig.nEncodeAUSize         = 0;
    _hMSLIMGCAP->tOMXEncodeConfig.tImageEncodeFormat    = OMX_TICOLOR_YUV422H;
    _hMSLIMGCAP->tOMXEncodeConfig.tInputImageFormat     = OMX_TICOLOR_YCbYCr;
    _hMSLIMGCAP->tOMXEncodeConfig.tImageCodingType      = OMX_IMAGE_CodingJPEG;
    _hMSLIMGCAP->tOMXEncodeConfig.nImageWidth           = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGCAP->tOMXEncodeConfig.nImageHeight          = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGCAP->tOMXEncodeConfig.tStreamingMode      = OMX_TIIMAGE_STREAMINGNONE;
    _hMSLIMGCAP->bEncComplete = MSL_FALSE;

    //work on copy of ssl config, as original ssl config is used as a reference to set other parameters
    memcpy(&tOMXSSLConfig, &_hMSLIMGCAP->tOMXSSLConfig, sizeof(OMX_SSL_PLANE_CONFIGTYPE));

    //if rotate by 90 or 270, swap the width & height of affected components
    if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGCAP->tOMXRotateConfig.tRotate ||
            OMX_TIIMAGE_ROTATE270 == _hMSLIMGCAP->tOMXRotateConfig.tRotate)
    {
        MSL_SWAP(_hMSLIMGCAP->tOMXEncodeConfig.nImageWidth, _hMSLIMGCAP->tOMXEncodeConfig.nImageHeight);
        if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == SNAPSHOT_MODE)
        {   MSL_SWAP(_hMSLIMGCAP->tOMXColorConvConfig.nSrcImageWidth, _hMSLIMGCAP->tOMXColorConvConfig.nSrcImageHeight);
        }
        MSL_SWAP(tOMXSSLConfig.nXLen, tOMXSSLConfig.nYLen);
    }

    /*---------------------------------------------------------------------------
    * do configuration for all omx components
    ---------------------------------------------------------------------------*/

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXCAM,
    (OMX_INDEXTYPE)OMX_IndexConfigCameraSettings,
    (OMX_PTR)&_hMSLIMGCAP->tOMXCameraConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXRescale,
    (OMX_INDEXTYPE)OMX_IndexConfigRescale,
    &tOMXRescaleConfig),MSL_IMGCAP_ERROR_INVALID_ARGUMENT );

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXOverlay,
    (OMX_INDEXTYPE)OMX_IndexConfigOverlay,
    &_hMSLIMGCAP->tOMXOverlayConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXEffect,
    (OMX_INDEXTYPE)OMX_IndexConfigEffect,
    &_hMSLIMGCAP->tOMXEffectConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXRotate,
    (OMX_INDEXTYPE)OMX_IndexConfigRotate,
    &_hMSLIMGCAP->tOMXRotateConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

   if(_hMSLIMGCAP->tOMXCameraConfig.tImageFormat != _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat
    && _hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXColorConv,
        (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
        &_hMSLIMGCAP->tOMXColorConvConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

    //encode and colorconversion configured only for snapshot
    if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == SNAPSHOT_MODE)
    {
        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXEnc,
        (OMX_INDEXTYPE)OMX_IndexConfigEncode,
        &_hMSLIMGCAP->tOMXEncodeConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXColorConv,
        (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
        &_hMSLIMGCAP->tOMXColorConvConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
    }

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXSSL,
    (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig ,
    &tOMXSSLConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

    //change the state of the OMX components to IDLE
    OMX_SendCommand(_hMSLIMGCAP->hOMXCAM,
    OMX_CommandStateSet,
    OMX_StateIdle,
    NULL
    );
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Init : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn MSL_ImgCap_Deinit :Deinitializes the pipeline
*
* @param [in] hIMGCap
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Deinit (MSL_HANDLE hIMGCap)
{
    /* dereference the local pointer */
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)hIMGCap;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Deinit : Entry");
    MSL_PROFILESTUB("MSL_ImgCap DeInitialization: Started");

    //set mode to deinit
    _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_DEINIT;

    //change the state of the OMX components to Loaded to trigger back the transition
    OMX_SendCommand(_hMSLIMGCAP->hOMXCAM,
    OMX_CommandStateSet,
    OMX_StateLoaded,
    NULL
    );

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Deinit : Exit");
    return MSL_IMGCAP_STATUS_OK;
}


/* ========================================================================== */
/**
* @fn MSL_ImgCap_Destroy :Destroys the Pipeline
*
* @param [in] hMSLIMG
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Destroy (MSL_HANDLE hMSLIMGCAP)
{
    MSL_U16 i;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Destroy : Entry");

    MSL_PROFILESTUB("MSL_ImgCap Destory: Started");
             #ifdef MSL_1_3MP_MODE
	           tMSLGloablFeatures.bSingleBufferMode = MSL_TRUE;
	         #endif   if(NULL != hMSLIMGCAP)

    if(NULL != hMSLIMGCAP)
    {
        /* dereference the local pointer */
        MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)hMSLIMGCAP;

        /* free omx client handles */
        OMX_FreeHandle(_hMSLIMGCAP->hOMXCAM);
        _hMSLIMGCAP->hOMXCAM = NULL;
            OMX_FreeHandle(_hMSLIMGCAP->hOMXEnc);
        _hMSLIMGCAP->hOMXEnc = NULL;
        OMX_FreeHandle(_hMSLIMGCAP->hOMXSSL);
        _hMSLIMGCAP->hOMXSSL = NULL;
        OMX_FreeHandle(_hMSLIMGCAP->hOMXColorConv);
        _hMSLIMGCAP->hOMXColorConv = NULL;
        OMX_FreeHandle(_hMSLIMGCAP->hOMXOverlay);
        _hMSLIMGCAP->hOMXOverlay = NULL;
        OMX_FreeHandle(_hMSLIMGCAP->hOMXRescale);
        _hMSLIMGCAP->hOMXRescale = NULL;
        OMX_FreeHandle(_hMSLIMGCAP->hOMXRotate);
        _hMSLIMGCAP->hOMXRotate = NULL;
        OMX_FreeHandle(_hMSLIMGCAP->hOMXEffect);
        _hMSLIMGCAP->hOMXEffect= NULL;
        /* free buffers */
        MSL_FREE(_hMSLIMGCAP->pCameraBuffHeader0);
        MSL_FREE(_hMSLIMGCAP->pCameraBuffHeader1);
        MSL_FREE(_hMSLIMGCAP->pIMGBuffHeader0);
        MSL_FREE(_hMSLIMGCAP->pIMGBuffHeader1);
        MSL_FREE(_hMSLIMGCAP->pEncBuffHeader);
        MSL_FREE(_hMSLIMGCAP->pCameraBuff);
        MSL_FREE(_hMSLIMGCAP->pIMGBuff );
        for(i=0; i < tMSLGloablFeatures.nMaxBurstCount; i++)
        {
            MSL_FREE(_hMSLIMGCAP->apEncOutBuff[i]);
        }
        MSL_FREE(_hMSLIMGCAP->apEncOutBuff);
        MSL_FREE(_hMSLIMGCAP->aEncodeBuffSize);
        MSL_FREE(hMSLIMGCAP);
    }

    MSL_PROFILESTUB("MSL_ImgCap Destory: completed");
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Destroy : Exit");

    return  MSL_IMGCAP_STATUS_OK  ;
}

/* ========================================================================== */
/**
* @fn MSL_ImgCap_SetConfig: Set Configurations for the cmponent indicated by index  in UCP
*
* @param [in] hMSLIMGCAP
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_SetConfig(MSL_HANDLE hIMGCap, MSL_INDEXTYPES tIndex, MSL_VOID *pParam)
{
    MSL_IMG_OVERLAYCONFIG   *tOverlayConfig;
    MSL_CAM_CONFIGTYPE      *tSSCameraConfig;
    MSL_DISPLAY_CONFIGTYPE  *tDisplayConfig;
    MSL_CAM_CONFIGTYPE      *tCameraConfig;
    MSL_FILE_CONFIGTYPE     *tFileInfoConfig;
    MSL_IMG_WINDOWTYPE      *tMSLWindowConfig;
    MSL_IMGCAP_STATUS              tRetVal = MSL_IMGCAP_STATUS_OK;

    /* dereference to private structure */
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)hIMGCap;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_SetConfig : Entry");

    switch(tIndex)
    {
    case MSL_CAMERA_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE), MSL_IMGCAP_ERROR_INVALID_STATE);
        tCameraConfig                                               = (MSL_CAM_CONFIGTYPE *) pParam;

        MSL_EXIT_IF( (tCameraConfig->unImgWidth > tMSLGloablFeatures.nMaxImgWidth) ||
        (tCameraConfig->unImgHeight > tMSLGloablFeatures.nMaxImgHeight), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

        /*  omx camera configuration values */
        _hMSLIMGCAP->tOMXCameraConfig.tImageFormat                  = MSL2OMXColor(tCameraConfig->tImgFormat);
        _hMSLIMGCAP->tOMXCameraConfig.unImageHeight                 = tCameraConfig->unImgHeight;
        _hMSLIMGCAP->tOMXCameraConfig.unImageWidth                  = tCameraConfig->unImgWidth;
        if(_hMSLIMGCAP->tOMXCameraConfig.unImageHeight   == OMX_VGA_HEIGHT &&
            _hMSLIMGCAP->tOMXCameraConfig.unImageWidth== OMX_VGA_WIDTH)
        _hMSLIMGCAP->tOMXCameraConfig.tImageSizeType = OMX_TIIMAGE_VGA;
       else if(_hMSLIMGCAP->tOMXCameraConfig.unImageHeight   == OMX_QCIF_HEIGHT &&
        _hMSLIMGCAP->tOMXCameraConfig.unImageWidth== OMX_QCIF_WIDTH)
        _hMSLIMGCAP->tOMXCameraConfig.tImageSizeType = OMX_TIIMAGE_QCIF;
       else if(_hMSLIMGCAP->tOMXCameraConfig.unImageHeight   == OMX_SXGA_HEIGHT &&
        _hMSLIMGCAP->tOMXCameraConfig.unImageWidth== OMX_SXGA_WIDTH)
        _hMSLIMGCAP->tOMXCameraConfig.tImageSizeType = OMX_TIIMAGE_SXGA;
        _hMSLIMGCAP->tOMXCameraConfig.tCaptureMode                  = (OMX_CAM_CAPTUREMODETYPE)tCameraConfig->tMode;
        break;

    case MSL_CALLBACKSET_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE), MSL_IMGCAP_ERROR_INVALID_STATE);
        /* set msl callback function */
        _hMSLIMGCAP->tMSLCallBack                                   = ((MSL_CALLBACK ) pParam);
        break;

    case MSL_DISPLAY_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE), MSL_IMGCAP_ERROR_INVALID_STATE);
        tDisplayConfig                                              = (MSL_DISPLAY_CONFIGTYPE *) pParam;

        MSL_EXIT_IF( (tDisplayConfig->unDisplayImgWidth > tMSLGloablFeatures.nMaxImgWidth) ||
        (tDisplayConfig->unDisplayImgHeight > tMSLGloablFeatures.nMaxImgHeight), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

        /* omx ssl configuration For VIEW FINDER MODE*/
        _hMSLIMGCAP->tOMXSSLConfig.nXLen                            = tDisplayConfig->unDisplayImgWidth;
        _hMSLIMGCAP->tOMXSSLConfig.nYLen                            = tDisplayConfig->unDisplayImgHeight;
        _hMSLIMGCAP->tOMXSSLConfig.nXOffset                         = tDisplayConfig->unDisplayXOffset;
        _hMSLIMGCAP->tOMXSSLConfig.nYOffset                         = tDisplayConfig->unDisplayYOffset;
        _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat                  = MSL2OMXColor(tDisplayConfig->tImgFormat);
        _hMSLIMGCAP->tOMXSSLConfig.bAlwaysOnTop                     = OMX_FALSE;
        _hMSLIMGCAP->tOMXSSLConfig.tPlaneBlendType                  = OMX_SSL_PLANEBLEND_OVERLAP;
        _hMSLIMGCAP->tOMXSSLConfig.bDeferredUpdate                  = OMX_TRUE;
        break;

    case MSL_OVERLAY_CONFIGINDEX:
        tOverlayConfig                                              = (MSL_IMG_OVERLAYCONFIG *) pParam;

        /* img overlay configuration for vf */
        _hMSLIMGCAP->tOMXOverlayConfig.nOverlayImageWidth           = tOverlayConfig->nImgWidth;
        _hMSLIMGCAP->tOMXOverlayConfig.nOverlayImageHeight          = tOverlayConfig->nImgHeight;
        _hMSLIMGCAP->tOMXOverlayConfig.nSrcXOffset                  = tOverlayConfig->nOverlayXOffset;
        _hMSLIMGCAP->tOMXOverlayConfig.nSrcYOffset                  = tOverlayConfig->nOverlayXOffset;
        _hMSLIMGCAP->tOMXOverlayConfig.pOverlayBuffer               = tOverlayConfig->pOverlayBuff;
        _hMSLIMGCAP->tOMXOverlayConfig.nOverlay                     = tOverlayConfig->nTransparencyColor;
        _hMSLIMGCAP->tOMXOverlayConfig.tOverlay                     = MSL2OMXOverlay(tOverlayConfig->tOverlayMode);
        _hMSLIMGCAP->tOMXOverlayConfig.nAlpha                       = tOverlayConfig->nAlpha;
        _hMSLIMGCAP->tOMXOverlayConfig.bInPlace                     = OMX_TRUE;
        if(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE)
        {
            //reconfig overlay image.
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXOverlay,
            (OMX_INDEXTYPE)OMX_IndexConfigOverlay,
            &_hMSLIMGCAP->tOMXOverlayConfig),MSL_IMGCAP_ERROR_INVALID_ARGUMENT );
        }
        break;

    case MSL_CROPWINDOW_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE), MSL_IMGCAP_ERROR_INVALID_STATE);
        tMSLWindowConfig = (MSL_IMG_WINDOWTYPE *) pParam;
        _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nWidth           = tMSLWindowConfig->nImgCropWidth;
        _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nHeight          = tMSLWindowConfig->nImgCropHeight;
        _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nXOffset         = tMSLWindowConfig->nImgXOffset;
        _hMSLIMGCAP->tOMXRescaleConfig.tCropWindow.nYOffset         = tMSLWindowConfig->nImgYOffset;
        break;


    case MSL_ENCFILE_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS), MSL_IMGCAP_ERROR_INVALID_STATE);
        tFileInfoConfig                                               = (MSL_FILE_CONFIGTYPE *) pParam;

        _hMSLIMGCAP->tEncodeFileType                                = tFileInfoConfig->tFileType;
        strcpy((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, tFileInfoConfig->sFileName);
        break;


    case MSL_ENCQUALITY_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS), MSL_IMGCAP_ERROR_INVALID_STATE);
        _hMSLIMGCAP->tOMXEncodeConfig.nQualityFactor                = *((MSL_U32*) pParam);
        break;


    case MSL_ZOOM_CONFIGINDEX:
        // zoom config change effects both VF and SS
        _hMSLIMGCAP->tOMXRescaleConfig.nZoomFactor                  = *((MSL_U16 *)pParam);

        if(MSL_IMGCAP_STATE_NONE != _hMSLIMGCAP->tIMGCAPState)
        {
            //reconfig rescale imge.
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXRescale,
            (OMX_INDEXTYPE)OMX_IndexConfigRescale,
            &_hMSLIMGCAP->tOMXRescaleConfig),MSL_IMGCAP_ERROR_INVALID_ARGUMENT );
        }
        break;

    case MSL_ROTATE_CONFIGINDEX:
        {
            OMX_TIIMAGE_ROTATETYPE tRotParam = MSL2OMXRotate(*((MSL_IMG_ROTATETYPE *)pParam));
            //when mirroring can be done in camera
            if(_hMSLIMGCAP->tOMXCamFeature.bMirrorSupport == MSL_TRUE)
            {
                if(tRotParam == OMX_TIIMAGE_ROTATE180 && _hMSLIMGCAP->tOMXCameraConfig.tMirrorType == OMX_MirrorNone)
               {
                    _hMSLIMGCAP->tOMXCameraConfig.tMirrorType= OMX_MirrorBoth;
                    _hMSLIMGCAP->tOMXRotateConfig.tRotate = OMX_TIIMAGE_ROTATE0;
                    if(_hMSLIMGCAP->tIMGCAPState > MSL_IMGCAP_STATE_NONE)
                   {
                        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXCAM, (OMX_INDEXTYPE)OMX_IndexConfigMirrorType,
                        &_hMSLIMGCAP->tOMXCameraConfig.tMirrorType), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
                        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXRotate,
                        (OMX_INDEXTYPE)OMX_IndexConfigRotate,&_hMSLIMGCAP->tOMXRotateConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
                    }
                 break;
               }
                else
               {
                    if(_hMSLIMGCAP->tOMXCameraConfig.tMirrorType == OMX_MirrorBoth)
                    {
                        _hMSLIMGCAP->tOMXCameraConfig.tMirrorType = OMX_MirrorNone;
                        if(_hMSLIMGCAP->tIMGCAPState > MSL_IMGCAP_STATE_NONE)
                        MSL_EXIT_IF(OMX_SetConfig (_hMSLIMGCAP->hOMXCAM, (OMX_INDEXTYPE)OMX_IndexConfigMirrorType,
                        &_hMSLIMGCAP->tOMXCameraConfig.tMirrorType), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
                   }
                 }
                }
                _hMSLIMGCAP->tOMXRotateConfig.tRotate                  = tRotParam;

                if(MSL_IMGCAP_STATE_NONE != _hMSLIMGCAP->tIMGCAPState)
                {
                    //IMG configurations - config rotate img.
                    if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
                    _hMSLIMGCAP->tOMXRotateConfig.nStride           = _hMSLIMGCAP->tOMXSSLConfig.nXLen;
                    else {
                        if(_hMSLIMGCAP->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE90 ||
                                _hMSLIMGCAP->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE270)
                        _hMSLIMGCAP->tOMXRotateConfig.nStride              = _hMSLIMGCAP->tOMXRotateConfig.nImageHeight;
                        else
                        _hMSLIMGCAP->tOMXRotateConfig.nStride              = _hMSLIMGCAP->tOMXRotateConfig.nImageWidth;
                    }

                    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXRotate,
                    (OMX_INDEXTYPE)OMX_IndexConfigRotate,
                    &_hMSLIMGCAP->tOMXRotateConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
                    if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGCAP->tOMXRotateConfig.tRotate ||
                            OMX_TIIMAGE_ROTATE270 == _hMSLIMGCAP->tOMXRotateConfig.tRotate)
                    {
                        OMX_SSL_PLANE_CONFIGTYPE tOMXSSLConfig;
                        memcpy(&tOMXSSLConfig, &_hMSLIMGCAP->tOMXSSLConfig, sizeof(OMX_SSL_PLANE_CONFIGTYPE));

                        //swap width and height of ssl
                        MSL_SWAP(tOMXSSLConfig.nXLen, tOMXSSLConfig.nYLen);

                        // reconfigure ssl
                        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXSSL,
                        (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig ,
                        &tOMXSSLConfig), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
                    }
                }
            break;
        }
    case MSL_BURSTCOUNT_CONFIGINDEX:
        MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS), MSL_IMGCAP_ERROR_INVALID_STATE);
        /* if burst count greater than max reset it to max */
        if(*((MSL_U16 *)pParam)  > tMSLGloablFeatures.nMaxBurstCount)
        *((MSL_U16 *)pParam)                                    = tMSLGloablFeatures.nMaxBurstCount;

        _hMSLIMGCAP->nBurstCount                                    = *((MSL_U16 *)pParam);

        break;

    case MSL_SEPIAEFFECT_CONFIGINDEX:
    case MSL_GRAYEFFECT_CONFIGINDEX:
	{
        MSL_IMG_EFFECTTYPE tEffect = *((MSL_IMG_EFFECTTYPE*)pParam);
        _hMSLIMGCAP->tOMXEffectConfig.tEffect= (OMX_TIIMAGE_EFFECTTYPE)MSL2OMXEffect(tEffect);
        _hMSLIMGCAP->tOMXEffectConfig.bInPlace                     = OMX_TRUE;
        if(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE)
        {
            //reconfig effect image.
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXEffect,
            (OMX_INDEXTYPE)OMX_IndexConfigEffect,
            &_hMSLIMGCAP->tOMXEffectConfig),MSL_IMGCAP_ERROR_INVALID_ARGUMENT );
        }
        break;
   }

    case MSL_MIRRORING_CONFIGINDEX :
        {
            OMX_MIRRORTYPE *pMirrorType = (OMX_MIRRORTYPE*)pParam;
            MSL_EXIT_IF((_hMSLIMGCAP->tOMXCamFeature.bMirrorSupport!= MSL_TRUE), MSL_IMGCAP_ERROR_INVALID_STATE);
            MSL_EXIT_IF((_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_NONE), MSL_IMGCAP_ERROR_INVALID_STATE);
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGCAP->hOMXCAM,
            (OMX_INDEXTYPE)OMX_IndexConfigMirrorType, pMirrorType), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);
            _hMSLIMGCAP->tOMXCameraConfig.tMirrorType = *pMirrorType;
            break;
        }
    case MSL_SAVESS_CONFIGINDEX :
    {    MSL_U8 nParam = *((MSL_U8*)pParam);
        if (nParam == MSL_FALSE)
        _hMSLIMGCAP->bAutoSave = MSL_FALSE;
       else
        _hMSLIMGCAP->bAutoSave = MSL_TRUE;
        break;
    }
    default:
        MSL_EXIT_IF(1, MSL_IMGCAP_ERROR_INVALID_ARGUMENT)
    }

EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_SetConfig : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn MSL_ImgCap_Viewfinder:Launches the Viewfinder
*
* @param [in] hIMGCap
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Viewfinder(MSL_HANDLE hIMGCap)
{
    MSL_U32                     ulTotalSize, ulVFSize;
    MSL_IMGCAP_STATUS                  tRetVal         = MSL_IMGCAP_STATUS_OK;
    MSL_IMGCAP_PRIV_STRUCT      *_hMSLIMGCAP    = (MSL_IMGCAP_PRIV_STRUCT *)hIMGCap;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Viewfinder : Entry");

    MSL_EXIT_IF(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_INIT, MSL_IMGCAP_ERROR_INVALID_STATE);

    //calculate the allocated buffer size
    ulTotalSize = tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight;
    //calculate viewfinder buffer size
    ulVFSize = _hMSLIMGCAP->tOMXCameraConfig.unImageWidth * _hMSLIMGCAP->tOMXCameraConfig.unImageHeight;

    if(MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport)
    {
        ulTotalSize = ulTotalSize * 3;
        ulVFSize  = ulVFSize  * 3;
    }
    else
    {
        ulTotalSize = ulTotalSize * 2;
        ulVFSize  = ulVFSize * 2;
    }


    //exit if allocated buffer is not good enough to hold atleast two viewfinder buffers
    MSL_EXIT_IF(ulTotalSize < ulVFSize * 2, MSL_IMGCAP_ERROR_NOMEMORY);

    _hMSLIMGCAP->pCameraBuffHeader0->pBuffer     = _hMSLIMGCAP->pCameraBuff;
    _hMSLIMGCAP->pCameraBuffHeader1->pBuffer     = (OMX_U8 *)( (MSL_U32)_hMSLIMGCAP->pCameraBuff + ulVFSize);

    _hMSLIMGCAP->pCameraBuffHeader0->nAllocLen   = ulVFSize;
    _hMSLIMGCAP->pCameraBuffHeader1->nAllocLen   = ulVFSize;

    _hMSLIMGCAP->pIMGBuffHeader0->pBuffer       = _hMSLIMGCAP->pIMGBuff;
    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
    _hMSLIMGCAP->pIMGBuffHeader0->nAllocLen     = ulTotalSize;
    else
    _hMSLIMGCAP->pIMGBuffHeader0->nAllocLen     = ulVFSize;

    //get handle to ssl DSA plane in
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetConfig(_hMSLIMGCAP->hOMXSSL,
    (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneFrameBuffPtr,
    &_hMSLIMGCAP->pIMGBuffHeader1->pBuffer ), MSL_IMGCAP_ERROR_INVALID_ARGUMENT);

    _hMSLIMGCAP->pIMGBuffHeader1->nAllocLen     = ulVFSize;

    //change the state of the OMX components to execute
    OMX_SendCommand(_hMSLIMGCAP->hOMXCAM,
    OMX_CommandStateSet,
    OMX_StateExecuting,
    NULL
    );
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Viewfinder : Exit");
    return  tRetVal;
}


/* ========================================================================== */
/**
* @fn MSL_ImgCap_Snapshot :Launches the snapshot
*
* @param [in] hIMGCap
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Snapshot(MSL_HANDLE hIMGCap)
{
    MSL_IMGCAP_STATUS                  tRetVal         = MSL_IMGCAP_STATUS_OK;
    MSL_IMGCAP_PRIV_STRUCT      *_hMSLIMGCAP    = (MSL_IMGCAP_PRIV_STRUCT *)hIMGCap;


    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Snapshot : Entry");

    MSL_PROFILESTUB("MSL_ImgCap Snapshot: Started");

    MSL_EXIT_IF(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_INIT, MSL_IMGCAP_ERROR_INVALID_STATE);

    _hMSLIMGCAP->pCameraBuffHeader0->pBuffer    = _hMSLIMGCAP->pCameraBuff;
    _hMSLIMGCAP->pCameraBuffHeader0->nAllocLen  = _hMSLIMGCAP->tOMXCameraConfig.unImageWidth * _hMSLIMGCAP->tOMXCameraConfig.unImageHeight * 2;

    _hMSLIMGCAP->pIMGBuffHeader0->pBuffer       = _hMSLIMGCAP->pIMGBuff;
#ifdef MSL_1_3MP_MODE
    if(_hMSLIMGCAP->tOMXCameraConfig.tImageSizeType != OMX_TIIMAGE_SXGA)
    {
        __msl_free(_hMSLIMGCAP->pIMGBuffHeader0->pBuffer);
         _hMSLIMGCAP->pIMGBuffHeader0->pBuffer = _hMSLIMGCAP->pCameraBuffHeader0->pBuffer+ _hMSLIMGCAP->pCameraBuffHeader0->nAllocLen;
         _hMSLIMGCAP->pIMGBuffHeader0->nAllocLen = _hMSLIMGCAP->pCameraBuffHeader0->nAllocLen;
    }
#else
    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
    _hMSLIMGCAP->pIMGBuffHeader0->nAllocLen     =  tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight * BITSPERPIXEL_IMAGE;
    else
    _hMSLIMGCAP->pIMGBuffHeader0->nAllocLen     =  DISPLAY_IMAGE_WIDTH * DISPLAY_IMAGE_HEIGHT * BITSPERPIXEL_IMAGE;
#endif
    _hMSLIMGCAP->pEncBuffHeader->pBuffer        = _hMSLIMGCAP->apEncOutBuff[_hMSLIMGCAP->nCurrentBurstCount];
    _hMSLIMGCAP->pEncBuffHeader->nAllocLen       = tMSLGloablFeatures.nMaxCompressedImageSize;

    _hMSLIMGCAP->bPreviewCycle                   = MSL_FALSE;




    //change the state of the OMX components to execute
    OMX_SendCommand(_hMSLIMGCAP->hOMXCAM,
    OMX_CommandStateSet,
    OMX_StateExecuting,
    NULL
    );
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_Snapshot : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn MSL_ImgCap_Pause :Pauses the Pipeline
*
* @param [in] hIMGCap
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     Not implemented in this release. Need to call deinit->init sequence to
*     simulate the environment.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_IMGCAP_STATUS MSL_ImgCap_Pause(MSL_HANDLE hIMGCap)
{
    //Not implemented in this release.
    return MSL_IMGCAP_ERROR_NOT_IMPLEMENTED;
}

/* ========================================================================== */
/** The FillBufferDone method is used to return filled buffers from an
output port back to the application for emptying and then reuse.
This is a blocking call so the application should not attempt to
empty the buffers during this call, but should queue the buffers
and empty them in another thread.  There is no error return, so
the application shall handle any errors generated internally.  The
application shall also update the buffer header to indicate the
number of bytes placed into the buffer.

The application should return from this call within 5 msec.

@param hComponent
handle of the component to access.  This is the component
handle returned by the call to the GetHandle function.
@param pAppData
pointer to an application defined value that was provided in the
pAppData parameter to the OMX_GetHandle method for the component.
This application defined value is provided so that the application
can have a component specific context when receiving the callback.
@param pBuffer
pointer to an OMX_BUFFERHEADERTYPE structure allocated with UseBuffer
or AllocateBuffer indicating the buffer that was filled.
*/
/* ========================================================================== */
OMX_ERRORTYPE MSL_ImgCap_OMXFillbufferDone(OMX_HANDLETYPE hComponent , OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffHeader)
{
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)pAppData;
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    if (_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF ||
            _hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS )
    {
        MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_OMXFillbufferDone : Entry");
        vsi_s_get(_hMSLIMGCAP->nSemCaller, _hMSLIMGCAP->nSemHandle);

        //check if callback is from camera
        if (hComponent == _hMSLIMGCAP->hOMXCAM)
        {
            MSL_PROFILESTUB("Camera buffer buffer capture completed");
            //increment current burst count
            if (_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS )
            {
                _hMSLIMGCAP->nCurrentBurstCount++;
                if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXRescale, pBuffHeader);
                else
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXOverlay, pBuffHeader);
            }
            else
            OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXRescale, pBuffHeader);
        }

        //check if from rescale
        if(hComponent == _hMSLIMGCAP->hOMXRescale)
        {
            MSL_PROFILESTUB("Rescaling completed ");

            if (_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF)
            {
                if(_hMSLIMGCAP->tOMXCameraConfig.tImageFormat !=
                        _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat)
                {
                    OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
                }
                else
                {
                    OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXOverlay, pBuffHeader);
                }
            }
            else if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS &&
                    MSL_TRUE != _hMSLIMGCAP->bPreviewCycle)
            {
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXOverlay, pBuffHeader);
            }
            //in preview cycle after, rescale do color convert
            else if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS &&
                    MSL_TRUE == _hMSLIMGCAP->bPreviewCycle)
            {
                //send buffer to color convert
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
            }
        }
        //check if callback is from color convert
        if (hComponent == _hMSLIMGCAP->hOMXColorConv)
        {
            MSL_PROFILESTUB("Color convertion completed ");

            if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS)
            {
                if(MSL_TRUE != _hMSLIMGCAP->bPreviewCycle)
                {
                    MSL_PROFILESTUB("Encoding start (when capture format is rgb ");

                    //send this buffer to encode
                    _hMSLIMGCAP->bEncComplete = MSL_FALSE;
                    OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXEnc, pBuffHeader);
                }
                else
                {
                    //send buffer for ssl
                    OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXSSL, pBuffHeader);
                }
            }
        }
        //check if callback is from rotate
        if (hComponent == _hMSLIMGCAP->hOMXRotate)
        {
            MSL_PROFILESTUB("Rotate completed ");
            if (_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF)
            {
                //send buffer for ssl
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXSSL, pBuffHeader);
            }
            else
            {
                OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE tOMXColorConvConfig;
                memcpy(&tOMXColorConvConfig, &_hMSLIMGCAP->tOMXColorConvConfig, sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));

                tOMXColorConvConfig.nSrcImageWidth          = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
                tOMXColorConvConfig.nSrcImageHeight         = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
                tOMXColorConvConfig.tInputImageFormat       = _hMSLIMGCAP->tOMXCameraConfig.tImageFormat;
                tOMXColorConvConfig.tOutputImageFormat      = _hMSLIMGCAP->tOMXEncodeConfig.tInputImageFormat;

                OMX_SetConfig(_hMSLIMGCAP->hOMXColorConv, (OMX_INDEXTYPE)OMX_IndexConfigColorConversion, &tOMXColorConvConfig);


                //send buffer to color convert
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
            }
        }
        //check if callback is from encode
        if (hComponent == _hMSLIMGCAP->hOMXEnc)
        {

            if(_hMSLIMGCAP->tOMXEncodeConfig.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT)
            {

                if(_hMSLIMGCAP->bEncComplete == MSL_FALSE)
                {
                    _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1] =
                    _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1] + pBuffHeader->nFilledLen;
                   pBuffHeader->pBuffer = pBuffHeader->pBuffer+pBuffHeader->nFilledLen;
                    OMX_FillThisBuffer(_hMSLIMGCAP->hOMXEnc, pBuffHeader);
                   MSL_EXIT_IF(1, OMX_ErrorNone);
                }
               else
              {
                   pBuffHeader->pBuffer = pBuffHeader->pBuffer - _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1];
                   _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1] =
                   _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1] + pBuffHeader->nFilledLen;
                   pBuffHeader->nFilledLen  = _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1];
               }
            }
            else
             {       _hMSLIMGCAP->aEncodeBuffSize[_hMSLIMGCAP->nCurrentBurstCount - 1] = pBuffHeader->nFilledLen;
                     _hMSLIMGCAP->bEncComplete = MSL_TRUE;
             }
            //if this is the last burst buffer, then save encoded buffers
            if(_hMSLIMGCAP->nCurrentBurstCount == _hMSLIMGCAP->nBurstCount)
            {
                if(_hMSLIMGCAP->bAutoSave != MSL_FALSE)
                {
                    if( MSL_ImgCap_SaveEncodedBuffers((MSL_HANDLE)_hMSLIMGCAP) != MSL_IMGCAP_STATUS_OK)
                    {
                        _hMSLIMGCAP->bErrorStatus = MSL_TRUE;
                    }
                }
            }
            else
            {
                pBuffHeader->pBuffer = _hMSLIMGCAP->apEncOutBuff[_hMSLIMGCAP->nCurrentBurstCount];
                //send next output buffer to encoder
                 pBuffHeader->nFilledLen = 0;
                 if(_hMSLIMGCAP->tOMXEncodeConfig.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT)
                        pBuffHeader->nAllocLen = MCU_MULTIPLE;
                OMX_FillThisBuffer(_hMSLIMGCAP->hOMXEnc, pBuffHeader);
            }
        }
EXIT:
        MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_OMXFillbufferDone : Exit");
        vsi_s_release(_hMSLIMGCAP->nSemCaller, _hMSLIMGCAP->nSemHandle);
    }
    return  OMX_ErrorNone;
}

/* ========================================================================== */

/** The EmptyBufferDone method is used to return emptied buffers from an
input port back to the application for reuse.  This is a blocking call
so the application should not attempt to refill the buffers during this
call, but should queue them and refill them in another thread.  There
is no error return, so the application shall handle any errors generated
internally.

The application should return from this call within 5 msec.

@param hComponent
handle of the component to access.  This is the component
handle returned by the call to the GetHandle function.
@param pAppData
pointer to an application defined value that was provided in the
pAppData parameter to the OMX_GetHandle method for the component.
This application defined value is provided so that the application
can have a component specific context when receiving the callback.
@param pBuffHeader
pointer to an OMX_BUFFERHEADERTYPE structure allocated with UseBuffer
or AllocateBuffer indicating the buffer that was emptied.
*/
/* ========================================================================== */
OMX_ERRORTYPE MSL_ImgCap_OMXEmptybufferDone(OMX_HANDLETYPE hComponent , OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffHeader)
{
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)pAppData;
    MSL_BOOL bReleased = MSL_FALSE;
    if (_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF ||
            _hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS )
    {
        MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_OMXEmptybufferDone : Entry");
        vsi_s_get(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
        //check if from rescale
        if(hComponent == _hMSLIMGCAP->hOMXRescale)
        {
            if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF)
            {
                //send buffer back to camera
                OMX_FillThisBuffer(_hMSLIMGCAP->hOMXCAM, pBuffHeader);
            }
            else
            {
                if(MSL_TRUE != _hMSLIMGCAP->bPreviewCycle)
                {
                    //send this buffer to rotate
                    if(OMX_TIIMAGE_ROTATE0 != _hMSLIMGCAP->tOMXRotateConfig.tRotate)
                    OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRotate, pBuffHeader);
                    else
                    {


                        //send buffer to color convert
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
                    }
                   {
                    }
                }
                else
                {
                    //send this buffer to color convert
                    OMX_FillThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
                }
            }
        }
        //check if callback is from overlay
        if (hComponent == _hMSLIMGCAP->hOMXOverlay)
        {
            MSL_PROFILESTUB("Overlay completed ");
            //send buffer for effects
            OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXEffect, pBuffHeader);
        }
        //check if callback is from effect
        if (hComponent == _hMSLIMGCAP->hOMXEffect)
        {
            MSL_PROFILESTUB("Effect completed ");
            //send buffer for effects
            if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS )
            {

                OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE tOMXColorConvConfig;
                memcpy(&tOMXColorConvConfig, &_hMSLIMGCAP->tOMXColorConvConfig, sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));

               tOMXColorConvConfig.nSrcImageWidth          = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageWidth;
                tOMXColorConvConfig.nSrcImageHeight         = _hMSLIMGCAP->tOMXRescaleConfig.nOutputImageHeight;
                tOMXColorConvConfig.tInputImageFormat       = _hMSLIMGCAP->tOMXCameraConfig.tImageFormat;
                tOMXColorConvConfig.tOutputImageFormat      = _hMSLIMGCAP->tOMXEncodeConfig.tInputImageFormat;

                OMX_SetConfig(_hMSLIMGCAP->hOMXColorConv, (OMX_INDEXTYPE)OMX_IndexConfigColorConversion, &tOMXColorConvConfig);


                if( _hMSLIMGCAP->tOMXRotateConfig.tRotate != OMX_TIIMAGE_ROTATE0)
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXRotate, pBuffHeader);
                else
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
            }
            else
            OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXRotate, pBuffHeader);
        }
        //check if buffer is from rotate
        if(hComponent == _hMSLIMGCAP->hOMXRotate)
        {
            if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF)
            {
                //send buffer back to rescale
                OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRescale, pBuffHeader);
            }
            else
            {
                //send buffer to color convert
                if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
                OMX_FillThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);
                else


                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXColorConv, pBuffHeader);

            }
        }
        //check if buffer is from color convert
        if(hComponent == _hMSLIMGCAP->hOMXColorConv)
        {
            if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF)
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXOverlay, pBuffHeader);
            else
            {
                if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
                OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRescale, pBuffHeader);
                else
                    {
                    _hMSLIMGCAP->bEncComplete= MSL_FALSE;
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXEnc, pBuffHeader);
                    }
            }
        }

        //if from ssl, pass buffer back to rotate
        if(hComponent == _hMSLIMGCAP->hOMXSSL)
        {
            MSL_PROFILESTUB("SSL rendering complete");

            if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_VF)
            {
                //send buffer back to rotate
                OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRotate, pBuffHeader);
            }
            else if(_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_SS)
            {
                MSL_PROFILESTUB("MSL_ImgCap Snapshot: Completed");
                //send information back to application about snapshot status
                if(MSL_TRUE == _hMSLIMGCAP->bErrorStatus)
                {
                    vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                    bReleased = MSL_TRUE;
					if(_hMSLIMGCAP->bAutoSave != MSL_FALSE)
					{
						if(_hMSLIMGCAP->nErrorCode == MSL_IMGCAP_ERROR_NOSPACE)
							_hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_SNAPSHOT, MSL_IMGCAP_ERROR_NOSPACE);
						else
                    		_hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_SNAPSHOT, MSL_IMGCAP_ERROR_IOWRITE);
					}
					else
                    _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_PREVIEWDONE, MSL_IMGCAP_ERROR_IOWRITE);
                }
                else
                {
                    vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                    bReleased = MSL_TRUE;
					if(_hMSLIMGCAP->bAutoSave != MSL_FALSE)
				    _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_SNAPSHOT, MSL_IMGCAP_STATUS_OK);
					else
                    _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_PREVIEWDONE, MSL_IMGCAP_STATUS_OK);
               }
                _hMSLIMGCAP->bErrorStatus = MSL_FALSE;

            }
        }

        //check if callback is from encoder
        if (hComponent == _hMSLIMGCAP->hOMXEnc)
        {
        MSL_PROFILESTUB("Encode complete");
        if(_hMSLIMGCAP->tOMXEncodeConfig.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT)
                    _hMSLIMGCAP->bEncComplete = MSL_TRUE;
            if(_hMSLIMGCAP->nCurrentBurstCount == _hMSLIMGCAP->nBurstCount)
            {
                OMX_TIIMAGE_RESCALE_PARAMTYPE tOMXRescaleConfig;
                OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE tOMXColConvConfig;
                memcpy(&tOMXRescaleConfig, &_hMSLIMGCAP->tOMXRescaleConfig, sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE));
                memcpy(&tOMXColConvConfig, &_hMSLIMGCAP->tOMXColorConvConfig, sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));
                //set preview flag to true
                _hMSLIMGCAP->bPreviewCycle = MSL_TRUE;

                //do rescaling for preview. Need to reconfigure output image width and height to match
                //display width and height
                tOMXRescaleConfig.nOutputImageWidth        = _hMSLIMGCAP->tOMXSSLConfig.nXLen;
                tOMXRescaleConfig.nOutputImageHeight       = _hMSLIMGCAP->tOMXSSLConfig.nYLen;
                tOMXRescaleConfig.tInputImageFormat        =  OMX_TICOLOR_YCbYCr;
                tOMXRescaleConfig.nZoomFactor              = MSL_ZOOM_NONE;
                tOMXRescaleConfig.bInPlace = OMX_FALSE;
                if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGCAP->tOMXRotateConfig.tRotate ||
                        OMX_TIIMAGE_ROTATE270 == _hMSLIMGCAP->tOMXRotateConfig.tRotate)
                {
                    MSL_SWAP(tOMXRescaleConfig.nInputImageWidth, tOMXRescaleConfig.nInputImageHeight);
                    MSL_SWAP(tOMXRescaleConfig.nOutputImageWidth, tOMXRescaleConfig.nOutputImageHeight);
                }

                //reconfigure colorconversion for preview.
                tOMXColConvConfig.nSrcImageHeight    = tOMXRescaleConfig.nOutputImageHeight;
                tOMXColConvConfig.nSrcImageWidth     = tOMXRescaleConfig.nOutputImageWidth;
                tOMXColConvConfig.tInputImageFormat =  tOMXRescaleConfig.tInputImageFormat;
                tOMXColConvConfig.tOutputImageFormat = _hMSLIMGCAP->tOMXSSLConfig.tPlaneImgFormat;
                tOMXColConvConfig.bInPlace = OMX_FALSE;

                OMX_SetConfig(_hMSLIMGCAP->hOMXRescale,
                (OMX_INDEXTYPE)OMX_IndexConfigRescale,
                &tOMXRescaleConfig);

                OMX_SetConfig(_hMSLIMGCAP->hOMXColorConv,
                (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
                &tOMXColConvConfig);

                //send buffer to rescaler input
                OMX_EmptyThisBuffer(_hMSLIMGCAP->hOMXRescale, pBuffHeader);
            }
            else
            {
                //send buffer to camera
                  OMX_FillThisBuffer(_hMSLIMGCAP->hOMXCAM, pBuffHeader);
            }
        }

        MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_OMXEmptybufferDone : Exit");
        if(bReleased == MSL_FALSE)
        vsi_s_release(_hMSLIMGCAP->nSemCaller, _hMSLIMGCAP->nSemHandle);
    }
    return  OMX_ErrorNone;
}

/* ========================================================================== */

/** The EventHandler method is used to notify the application when an
event of interest occurs.  Events are defined in the OMX_EVENTTYPE
enumeration.  Please see that enumeration for details of what will
be returned for each type of event. Callbacks should not return
an error to the component, so if an error occurs, the application
shall handle it internally.  This is a blocking call.

The application should return from this call within 5 msec to avoid
blocking the component for an excessively long period of time.

@param hComponent
handle of the component to access.  This is the component
handle returned by the call to the GetHandle function.
@param pAppData
pointer to an application defined value that was provided in the
pAppData parameter to the OMX_GetHandle method for the component.
This application defined value is provided so that the application
can have a component specific context when receiving the callback.
@param eEvent
Event that the component wants to notify the application about.
@param nData1
nData will be the OMX_ERRORTYPE for an error event and will be
an OMX_COMMANDTYPE for a command complete event and OMX_INDEXTYPE for a OMX_PortSettingsChanged event.
@param nData2
nData2 will hold further information related to the event. Can be OMX_STATETYPE for
a OMX_StateChange command or port index for a OMX_PortSettingsCHanged event.
Default value is 0 if not used. )
@param pEventData
Pointer to additional event-specific data (see spec for meaning).
*/
/* ========================================================================== */
OMX_ERRORTYPE MSL_ImgCap_OMXEventHandle(
OMX_HANDLETYPE hComponent,
OMX_PTR pAppData,
OMX_EVENTTYPE eEvent,
OMX_U32 nData1,
OMX_U32 nData2,
OMX_PTR pEventData)
{
    OMX_STATETYPE tTransitionState;
    MSL_BOOL bReleased = MSL_FALSE;
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)pAppData;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_OMXEventHandle : Entry");
    vsi_s_get(_hMSLIMGCAP->nSemCaller, _hMSLIMGCAP->nSemHandle);
    if(MSL_IMGCAP_STATE_NONE == _hMSLIMGCAP->tIMGCAPState)
    {
        tTransitionState = OMX_StateIdle;
    }
    else if(MSL_IMGCAP_STATE_INIT == _hMSLIMGCAP->tIMGCAPState)
    {
        tTransitionState = OMX_StateExecuting;
    }
    else if(MSL_IMGCAP_STATE_DEINIT == _hMSLIMGCAP->tIMGCAPState)
    {
        tTransitionState = OMX_StateLoaded;
    }

    //check for state transition
    if(OMX_CommandStateSet == nData1 && OMX_EventCmdComplete == eEvent)
    {
        if(tTransitionState == nData2)
        {
            if(hComponent == _hMSLIMGCAP->hOMXCAM)
            {
                OMX_SendCommand(_hMSLIMGCAP->hOMXRescale,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
            }
            else if(hComponent == _hMSLIMGCAP->hOMXRescale)
            {
                OMX_SendCommand(_hMSLIMGCAP->hOMXOverlay,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
            }
            else if(hComponent == _hMSLIMGCAP->hOMXOverlay)
            {
                OMX_SendCommand(_hMSLIMGCAP->hOMXRotate,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
            }
            else if(hComponent == _hMSLIMGCAP->hOMXRotate)
            {
                if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
                OMX_SendCommand(_hMSLIMGCAP->hOMXColorConv,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
                else
                OMX_SendCommand(_hMSLIMGCAP->hOMXEffect,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
            }
            else if(hComponent == _hMSLIMGCAP->hOMXEffect)
            {
                OMX_SendCommand(_hMSLIMGCAP->hOMXSSL,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
            }
            else if(hComponent == _hMSLIMGCAP->hOMXSSL)
            {
                if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
                {
                    if(MSL_IMGCAP_STATE_NONE == _hMSLIMGCAP->tIMGCAPState)
                    {
                        //set mode to init
                        _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_INIT;

                        MSL_PROFILESTUB("MSL_ImgCap Initialization: Completed");

                        //call the msl callback function to inform the state transition is complete
                        vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                        bReleased = MSL_TRUE;
                        _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_INIT, MSL_IMGCAP_STATUS_OK);
                    }
                    else if(MSL_IMGCAP_STATE_INIT == _hMSLIMGCAP->tIMGCAPState)
                    {
                        //set mode to VF
                        _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_VF;

                        //call the msl callback function to inform the state transition is complete
                        vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                        bReleased = MSL_TRUE;
                        _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_VIEWFINDER, MSL_IMGCAP_STATUS_OK);

                        //send VFcamera buffer to camera
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXCAM, _hMSLIMGCAP->pCameraBuffHeader0);

                        //send second VFcamera buffer to camera
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXCAM, _hMSLIMGCAP->pCameraBuffHeader1);

                        //send buffer to output of rescale
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRescale, _hMSLIMGCAP->pIMGBuffHeader0);

                         OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRotate, _hMSLIMGCAP->pIMGBuffHeader1);
                    }
                    else if (_hMSLIMGCAP->tIMGCAPState == MSL_IMGCAP_STATE_DEINIT)
                    {
                        //change mode to none
                        _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_NONE;

                        MSL_PROFILESTUB("MSL_ImgCap DeInitialization: Completed");
                        vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                        bReleased = MSL_TRUE;

                        _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_DEINIT, MSL_IMGCAP_STATUS_OK);
                    }
                }
                else
                {
                    OMX_SendCommand(_hMSLIMGCAP->hOMXEnc,
                    OMX_CommandStateSet,
                    tTransitionState,
                    NULL
                    );
                }

            }
            else if(hComponent == _hMSLIMGCAP->hOMXEnc)
            {
                OMX_SendCommand(_hMSLIMGCAP->hOMXColorConv,
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );
            }
            else if(hComponent == _hMSLIMGCAP->hOMXColorConv)
            {
                if(_hMSLIMGCAP->tOMXCameraConfig.tCaptureMode == VIEWFINDER_MODE)
                {
                    OMX_SendCommand(_hMSLIMGCAP->hOMXEffect,
                    OMX_CommandStateSet,
                    tTransitionState,
                    NULL
                    );
                }
                else
                {
                    if(MSL_IMGCAP_STATE_NONE == _hMSLIMGCAP->tIMGCAPState)
                    {
                        //set mode to init
                        _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_INIT;

                        MSL_PROFILESTUB("MSL_ImgCap Initialization: Completed");

                        //call the msl callback function to inform the state transition is complete
                        vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                        bReleased = MSL_TRUE;
                        _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_INIT, MSL_IMGCAP_STATUS_OK);
                    }
                    else if(MSL_IMGCAP_STATE_INIT == _hMSLIMGCAP->tIMGCAPState)
                    {
                        //set mode to SS
                        _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_SS;

                        //send buffer to camera
                        MSL_PROFILESTUB("Buffer sent to camera for snapshot");
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXCAM, _hMSLIMGCAP->pCameraBuffHeader0);

                        //send img buffer to rescale
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXRescale, _hMSLIMGCAP->pIMGBuffHeader0);

                        //send encode output buffer to encoder
                        _hMSLIMGCAP->pEncBuffHeader->nFilledLen = 0;
                        if(_hMSLIMGCAP->tOMXEncodeConfig.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT)
                        _hMSLIMGCAP->pEncBuffHeader->nAllocLen = MCU_MULTIPLE;
                        OMX_FillThisBuffer(_hMSLIMGCAP->hOMXEnc, _hMSLIMGCAP->pEncBuffHeader);

                    }
                    else if (MSL_IMGCAP_STATE_DEINIT == _hMSLIMGCAP->tIMGCAPState)
                    {
                        //change mode to none
                        _hMSLIMGCAP->tIMGCAPState           = MSL_IMGCAP_STATE_NONE;

                        MSL_PROFILESTUB("MSL_ImgCap DeInitialization: Completed");

                        vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
                        bReleased = MSL_TRUE;
                        _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_DEINIT, MSL_IMGCAP_STATUS_OK);
                    }
                }
            }
        }
    }
    else
    {
        MSL_CMDTYPE tCMD;
        switch(_hMSLIMGCAP->tIMGCAPState)
        {
        case MSL_IMGCAP_STATE_NONE:
        case MSL_IMGCAP_STATE_INIT:
            tCMD = MSL_CMD_INIT;
            break;
        case MSL_IMGCAP_STATE_VF:
            tCMD = MSL_CMD_SNAPSHOT;
            break;
        case MSL_IMGCAP_STATE_SS:
            tCMD = MSL_CMD_VIEWFINDER;
            break;
        case MSL_IMGCAP_STATE_DEINIT:
            tCMD = MSL_CMD_DEINIT;
            break;
        default:
            tCMD = MSL_CMD_INIT;
        }
        MSL_PROFILESTUB("Error in ImgCap");
        MSL_DETAILFUNC_TRACE("MSL_ImgCap_OMXEventHandle: Error event callback. State: %d\t Event: %d\n", _hMSLIMGCAP->tIMGCAPState, eEvent);
        vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
        bReleased = MSL_TRUE;
        _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, tCMD, MSL_IMGCAP_ERROR_UNKNOWN);
    }
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_OMXEventHandle : Exit");
    if(bReleased == MSL_FALSE)
    vsi_s_release(_hMSLIMGCAP->nSemCaller,_hMSLIMGCAP->nSemHandle);
    return OMX_ErrorNone;
}

/* ========================================================================== */
/**
* @fn MSLIMgCapt_SaveEncodedBuffers :Saves the Encoded buffers according to the filetype specified
*
* @param [in] hIMGCap
*     handle to MSLIMG object
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
MSL_STATUS MSL_ImgCap_SaveEncodedBuffers(MSL_HANDLE hIMGCap)
{
#ifndef WIN32
    MSL_STATUS tStatus,tRetVal = MSL_IMGCAP_STATUS_OK;
    MSL_U32 nFreeBytes;
	T_RFS_FD hJPGFile;
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = (MSL_IMGCAP_PRIV_STRUCT *)hIMGCap;
    MSL_U8 nStrlen = strlen((MSL_STRING)_hMSLIMGCAP->sEncodeFileName);
    MSL_U8 i;
    MSL_PROFILESTUB("MSL_ImgCap_SaveEncodedBuffers : Entry");
    MSL_PROFILEENTRY(MSL_ImgCap_SaveEncodedBuffers);
	//for OMAPS00149533 - add state check to avoid crash when call occurs while saving
    MSL_EXIT_IF(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_SS, MSL_IMGCAP_ERROR_INVALID_STATE);
    nSavedCount = 0;
    hMslImgCapComp = (MSL_IMGCAP_PRIV_STRUCT*)_hMSLIMGCAP;
    if(MSL_FILETYPE_FFS == _hMSLIMGCAP->tEncodeFileType)
    {
        T_FFS_FD hJPGFile;
        if(_hMSLIMGCAP->bAutoSave == MSL_FALSE)
        {
            T_RV_RETURN *pCallback = (T_RV_RETURN*)MSL_ALLOC(sizeof(T_RV_RETURN));
            pCallback->addr_id = 0;
            pCallback->callback_func = MSL_ImgCap_OpenFFSEventHandle;
            sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"_%d.jpg\0",0);
            ffs_query(Q_BYTES_FREE, &nFreeBytes) ;
            //exit if not enough space left in FFS
            MSL_EXIT_IF(nFreeBytes < _hMSLIMGCAP->aEncodeBuffSize[0], MSL_IMGCAP_ERROR_IOWRITE);
            /* open the file */
            if(ffs_open_nb((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, FFS_O_RDWR | FFS_O_CREATE, pCallback)<EFFS_OK)
            {
                //restore original file name
                sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                return  MSL_IMGCAP_ERROR_IOWRITE;
            }
        }
        else
        {
            for(i = 0;i<_hMSLIMGCAP->nCurrentBurstCount; i++)
            {
                sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"_%d.jpg\0",i);
                ffs_query(Q_BYTES_FREE, &nFreeBytes) ;
                //exit if not enough space left in FFS
                MSL_EXIT_IF(nFreeBytes < _hMSLIMGCAP->aEncodeBuffSize[i], MSL_IMGCAP_ERROR_IOWRITE);
                /* open the file */
                if((hJPGFile=ffs_open((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, FFS_O_RDWR | FFS_O_CREATE))<EFFS_OK)
                {
                    //restore original file name
                    sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                    MSL_EXIT_IF(1, MSL_IMGCAP_ERROR_IOWRITE);
                }
                if (ffs_write(hJPGFile, _hMSLIMGCAP->apEncOutBuff[i], _hMSLIMGCAP->aEncodeBuffSize[i] ) < 0)
                {
                    MSL_EXIT_IF(ffs_close(hJPGFile) < EFFS_OK, MSL_IMGCAP_ERROR_IOWRITE);
                    ffs_remove((MSL_STRING)_hMSLIMGCAP->sEncodeFileName);
                    //restore original file name
                    sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                    MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
                }
                sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                MSL_EXIT_IF(ffs_close (hJPGFile) < EFFS_OK, MSL_IMGCAP_ERROR_IOWRITE);
            }
        }
    }
    else
    {
        #ifdef MSL_USE_RFS
        
        sfilename_uc= (T_WCHAR *) MSL_ALLOC(MSL_MAXFILENAME_LENGTH);
        if(_hMSLIMGCAP->bAutoSave == MSL_FALSE)
        {
            T_RV_RETURN pCallback;
            pCallback.addr_id = 0;
            pCallback.callback_func = MSL_ImgCap_OpenRFSEventHandle;
            sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"_%d.jpg\0",0);

            convert_u8_to_unicode((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, sfilename_uc);
            /* open the file */
            if((tStatus = rfs_open_nb(sfilename_uc, RFS_O_RDWR | RFS_O_CREAT, RFS_IRWXO, pCallback)) < RFS_EOK)
            {
                //restore original file name
                sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
            }
        }
        else
        {
            for(i = 0; i < _hMSLIMGCAP->nCurrentBurstCount; i++)
            {
                sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"_%d.jpg\0",i);
                convert_u8_to_unicode((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, sfilename_uc);
                /* open the file */
                if((hJPGFile = rfs_open(sfilename_uc, RFS_O_RDWR | RFS_O_CREAT, RFS_IRWXO)) < RFS_EOK)
                {
                    //restore original file name
                    sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                    MSL_EXIT_IF(1, MSL_IMGCAP_ERROR_IOWRITE);
                }

                /* write output from the burst buffer */
                if((tStatus = rfs_write(hJPGFile, _hMSLIMGCAP->apEncOutBuff[i], _hMSLIMGCAP->aEncodeBuffSize[i] )) < 0)
                {
                    MSL_EXIT_IF(rfs_close(hJPGFile)<RFS_EOK, MSL_IMGCAP_ERROR_IOWRITE);
                    rfs_remove(sfilename_uc);
                    //restore original file name
                    sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                    MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
                }
              sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
            MSL_EXIT_IF(rfs_close (hJPGFile) < RFS_EOK, MSL_IMGCAP_ERROR_IOWRITE);
          }
                    if(sfilename_uc)
        MSL_FREE(sfilename_uc);
        }
        #endif
    }

EXIT:
    if((tStatus == RFS_ENOSPACE) || (tStatus == RFS_EACCES)||(hJPGFile == RFS_EACCES))
	{    
		MSL_ENTRYEXITFUNC_TRACE(" RFS NO Space ");
		tRetVal = MSL_IMGCAP_ERROR_NOSPACE;
	}
     
	_hMSLIMGCAP->nErrorCode = tRetVal;

    MSL_PROFILEEXIT(MSL_ImgCap_SaveEncodedBuffers);
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgCap_SaveEncodedBuffers : Exit");
#endif
    return tRetVal;
}
/* ========================================================================== */
/**
* @fn MSL_ImgCap_OpenFFSEventHandle :Event Handle for FFS File open
*
* @param [in] tFfsRet
*     handle to return message
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
void MSL_ImgCap_OpenFFSEventHandle(OMX_PTR pMsg)
{
    T_FFS_FILE_CNF *tFfsRet = (T_FFS_FILE_CNF*) pMsg;
    MSL_STATUS tRetVal = MSL_IMGCAP_STATUS_OK;
    MSL_U32 nFreeBytes;
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = hMslImgCapComp;
    MSL_U8 nStrlen = strlen((MSL_STRING)_hMSLIMGCAP->sEncodeFileName)-6;
    T_FFS_FD hJPGFile = tFfsRet->error;
    MSL_EXIT_IF(hJPGFile < 0, MSL_IMGCAP_ERROR_IOWRITE)
    /* write output from the burst buffer */
    MSL_PROFILESTUB("msl ffs event handle");
    if (ffs_write(hJPGFile, _hMSLIMGCAP->apEncOutBuff[nSavedCount], _hMSLIMGCAP->aEncodeBuffSize[nSavedCount] ) < 0)
    {
        MSL_EXIT_IF(ffs_close(hJPGFile) < EFFS_OK, MSL_IMGCAP_ERROR_IOWRITE);
        ffs_remove((MSL_STRING)_hMSLIMGCAP->sEncodeFileName);
        //restore original file name
        sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
        MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
    }
    //restore original file name
    sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
    MSL_EXIT_IF(ffs_close (hJPGFile) < EFFS_OK, MSL_IMGCAP_ERROR_IOWRITE);
    for(nSavedCount= 1;nSavedCount<_hMSLIMGCAP->nCurrentBurstCount; nSavedCount++)
    {
        sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"_%d.jpg\0",nSavedCount);
        ffs_query(Q_BYTES_FREE, &nFreeBytes) ;
        //exit if not enough space left in FFS
        MSL_EXIT_IF(nFreeBytes < _hMSLIMGCAP->aEncodeBuffSize[nSavedCount], MSL_IMGCAP_ERROR_IOWRITE);
        /* open the file */
        if((hJPGFile=ffs_open((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, FFS_O_RDWR | FFS_O_CREATE))<EFFS_OK)
        {
            //restore original file name
            sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
            MSL_EXIT_IF(1, MSL_IMGCAP_ERROR_IOWRITE);
        }
        if (ffs_write(hJPGFile, _hMSLIMGCAP->apEncOutBuff[nSavedCount], _hMSLIMGCAP->aEncodeBuffSize[nSavedCount] ) < 0)
        {
            MSL_EXIT_IF(ffs_close(hJPGFile) < EFFS_OK, MSL_IMGCAP_ERROR_IOWRITE);
            ffs_remove((MSL_STRING)_hMSLIMGCAP->sEncodeFileName);
            //restore original file name
            sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
            MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
        }
    }
EXIT:
    MSL_FREE(tFfsRet->header.callback_func);
    rvf_free_buf(tFfsRet);
    hMslImgCapComp = NULL;
    _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_SNAPSHOT, tRetVal);
    _hMSLIMGCAP->bErrorStatus = MSL_FALSE;
}
/* ========================================================================== */
/**
* @fn MSL_ImgCap_OpenRFSEventHandle :Event Handle for RFS File open
*
* @param [in] tRfsRet
*     handle to return message
*
* @return MSL_IMGCAP_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgcap.h
*/
/* ========================================================================== */
void MSL_ImgCap_OpenRFSEventHandle( OMX_PTR pMsg)
{
    T_RFS_READY_RSP_MSG *tRfsRet = (T_RFS_READY_RSP_MSG*) pMsg;
    MSL_U16  i=0;
    MSL_STATUS tStatus,tRetVal = MSL_IMGCAP_STATUS_OK;
    MSL_U32 nFreeBytes;
    MSL_IMGCAP_PRIV_STRUCT *_hMSLIMGCAP = hMslImgCapComp;
    MSL_U8 nStrlen = strlen((MSL_STRING)_hMSLIMGCAP->sEncodeFileName)-6;
    MSL_U8 tOpMode = O_AIO;
    T_RV_RETURN tRetPath;
    tStatus = tRfsRet->result;
    MSL_PROFILESTUB("Entry OpenRFSEventHandle");
	//for OMAPS00149533 - add state check to avoid crash when call occurs while saving
    MSL_EXIT_IF(_hMSLIMGCAP->tIMGCAPState != MSL_IMGCAP_STATE_SS, MSL_IMGCAP_ERROR_INVALID_STATE);
    MSL_EXIT_IF(tStatus < RFS_EOK, MSL_IMGCAP_ERROR_IOWRITE)
    /* write output from the burst buffer */
        tRetPath.addr_id = 0;
        tRetPath.callback_func = MSL_ImgCap_OpenRFSEventHandle;

switch(tRfsRet->command_id)
{
    case RFS_OPEN_RSP:
    {
         MSL_PROFILESTUB("Open Done");
       hJPGFile = tRfsRet->result;
        rfs_fcntl(hJPGFile, F_SETFLAG, &tOpMode);
        rfs_fcntl(hJPGFile, F_SETRETPATH, &tRetPath);
        if((tStatus = rfs_write(hJPGFile, _hMSLIMGCAP->apEncOutBuff[nSavedCount], _hMSLIMGCAP->aEncodeBuffSize[nSavedCount] )) < 0)
        {
           MSL_PROFILESTUB("Write Error");
           MSL_EXIT_IF(rfs_close(hJPGFile)<RFS_EOK, MSL_IMGCAP_ERROR_IOWRITE);
           MSL_EXIT_IF(rfs_remove(sfilename_uc)<RFS_EOK, MSL_IMGCAP_ERROR_IOWRITE);
              //restore original file name
             sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
             MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
       }
    }
	// fix memory leak here 
    	rvf_free_buf(tRfsRet);
    break;
    case RFS_WRITE_RSP:
    {
        MSL_PROFILESTUB("Write Done");
        //restore original file name
        sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
        if(rfs_close (hJPGFile) < RFS_EOK)
        {
              MSL_PROFILESTUB("Close Error");
              MSL_EXIT_IF(rfs_remove(sfilename_uc)<RFS_EOK, MSL_IMGCAP_ERROR_IOWRITE);
             MSL_EXIT_IF(1,MSL_IMGCAP_ERROR_IOWRITE);
        }
	// fix memory leak here 
    	rvf_free_buf(tRfsRet);
    }
    break;
   case RFS_CLOSE_RSP:
    {
        MSL_PROFILESTUB("Close Done");
        nSavedCount = nSavedCount + 1;
        if(nSavedCount < _hMSLIMGCAP->nCurrentBurstCount)
        {
            nStrlen = nStrlen + 6;
            sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"_%d.jpg\0",nSavedCount);
            convert_u8_to_unicode((MSL_STRING)_hMSLIMGCAP->sEncodeFileName, sfilename_uc);
            if((tStatus = rfs_open_nb(sfilename_uc, RFS_O_RDWR | RFS_O_CREAT, RFS_IRWXO, tRetPath)) < RFS_EOK)
            {
                    //restore original file name
                    sprintf(&_hMSLIMGCAP->sEncodeFileName[nStrlen],"\0");
                    MSL_EXIT_IF(1,  MSL_IMGCAP_ERROR_IOWRITE);
            }
		// fix memory leak here 
    		rvf_free_buf(tRfsRet);
        }
        else
            MSL_EXIT_IF(1, MSL_IMGCAP_STATUS_OK);
    }
    break;
}
return;
EXIT:

if((tStatus == RFS_ENOSPACE) || (tStatus == RFS_EACCES))
	tRetVal= MSL_IMGCAP_ERROR_NOSPACE;

if(tRetVal != MSL_IMGCAP_STATUS_OK)
    MSL_PROFILESTUB("RFS Error");
else
    MSL_PROFILESTUB("Save complete");
     if(sfilename_uc)
    MSL_FREE(sfilename_uc);
    //free the message
    rvf_free_buf(tRfsRet);
    hMslImgCapComp = NULL;
    _hMSLIMGCAP->tMSLCallBack(_hMSLIMGCAP, MSL_UCP_IMGCAP, MSL_CMD_SNAPSHOT, tRetVal);
}


#endif
