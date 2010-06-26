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
*! 11-March-2006 Sowmya Priya: Initial Release
*!
*! 05-June-2006  Narendran M R(narendranmr@ti.com) : Code update to add profile, traces
*! features. Also rearchitected the code to add overlay feature.
*! 03-July-2006 Sowmya Priya : Fixed the RFS flag, Zoom out ,Overlay issues
*!
* ========================================================================= */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdlib.h>
#include <string.h>
#include"_msl_imgview.h"
#include "rfs/rfs_fm.h"
#include "typedefs.h"
#include "vsi.h"
#include "os.h"
#include "frm_glob.h"

/* ========================================================================== */
/**
* @fn MSL_ImgView_Create : Creates the pipeline.
*
* @param [in] phIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*
========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_Create (MSL_HANDLE *phIMGView)
{
    MSL_IMGVIEW_PRIV_STRUCT *_hMSLIMGView;
    MSL_U16                 i;
    MSL_U32                 ulSize;
    MSL_IMGVIEW_STATUS               tRetVal = MSL_IMGVIEW_STATUS_OK;
    MSL_U8                      cSemName[20];
    T_HANDLE entityHandle;
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Create : Entry");

    MSL_PROFILESTUB("MSL_ImgView_Create start");

    /*Memory allocation */
    *phIMGView = (MSL_IMGVIEW_PRIV_STRUCT *)MSL_ALLOC(sizeof(MSL_IMGVIEW_PRIV_STRUCT));
    _hMSLIMGView  = *phIMGView;

    MSL_EXIT_IF(_hMSLIMGView == NULL, MSL_IMGVIEW_ERROR_NOMEMORY);
   entityHandle = e_running[os_MyHandle()];
    ltoa((MSL_U32)_hMSLIMGView, (OMX_STRING)&cSemName[0]);
   _hMSLIMGView->nSemHandle = vsi_s_open(entityHandle, (OMX_STRING)cSemName, 1);
   _hMSLIMGView->nSemCaller = (MSL_U32) entityHandle;

    _hMSLIMGView->tOMXCallbacks.EmptyBufferDone = MSL_ImgView_OMXEmptybufferDone;
    _hMSLIMGView->tOMXCallbacks.EventHandler    = MSL_ImgView_OMXEventHandle;
    _hMSLIMGView->tOMXCallbacks.FillBufferDone  = MSL_ImgView_OMXFillbufferDone;

    /* initialize all OMX handle to NULL */
    _hMSLIMGView->hOMXRescale                   = NULL;
    _hMSLIMGView->hOMXRotate                    = NULL;
    _hMSLIMGView->hOMXOverlay                   = NULL;
    _hMSLIMGView->hOMXDec                       = NULL;
    _hMSLIMGView->ahOMXSSL                      = NULL;

    _hMSLIMGView->pBuff0                        = NULL;
    _hMSLIMGView->pBuff1                        = NULL;     
    _hMSLIMGView->pImgProcBuffHeader0           = NULL;
    _hMSLIMGView->pImgProcBuffHeader1           = NULL;

    /*allocate OMX_BUFFERHEADERTYPE for IMG buffer header */
    _hMSLIMGView->pImgProcBuffHeader0       = (OMX_BUFFERHEADERTYPE *)MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGView->pImgProcBuffHeader0 == NULL, MSL_IMGVIEW_ERROR_NOMEMORY);

    /*allocate OMX_BUFFERHEADERTYPE for IMG buffer header */
    _hMSLIMGView->pImgProcBuffHeader1       = (OMX_BUFFERHEADERTYPE *)MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGView->pImgProcBuffHeader1 == NULL, MSL_IMGVIEW_ERROR_NOMEMORY);

    //calculate the allocated buffer size
    ulSize = tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight;
    if(MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport) 
    {
        ulSize = ulSize * 3;
    } 
    else 
    {    
        ulSize = ulSize * 2;
    }
    /*allocate buffers for processing */
    _hMSLIMGView->pBuff0                           = (OMX_U8*)MSL_ALLOC(ulSize);
    MSL_EXIT_IF(_hMSLIMGView->pBuff0 == NULL, MSL_IMGVIEW_ERROR_NOMEMORY);
    /* initialize the bufferheaders */
    _hMSLIMGView->pImgProcBuffHeader0->pBuffer     =_hMSLIMGView->pBuff0;
    _hMSLIMGView->pImgProcBuffHeader0->nAllocLen   = ulSize;

    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
{        ulSize = tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight;
    if(MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport) 
    {
        ulSize = ulSize * 3;
    } 
    else 
    {    
        ulSize = ulSize * 2;
    }
        }
    else
        ulSize = tMSLGloablFeatures.nMaxCompressedImageSize;
    /*allocate buffers processing */
    _hMSLIMGView->pBuff1                           = (OMX_U8*)MSL_ALLOC(ulSize);
          MSL_EXIT_IF(_hMSLIMGView->pBuff1 == NULL, MSL_IMGVIEW_ERROR_NOMEMORY);
    _hMSLIMGView->pImgProcBuffHeader1->pBuffer     =_hMSLIMGView->pBuff1;
    _hMSLIMGView->pImgProcBuffHeader1->nAllocLen   = ulSize;

    /* get handle to OMX IMG client for decode*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGView->hOMXDec, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGView,
    &_hMSLIMGView->tOMXCallbacks
    ), MSL_IMGVIEW_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for rescale*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGView->hOMXRescale, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGView,
    &_hMSLIMGView->tOMXCallbacks
    ), MSL_IMGVIEW_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for color convert */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGView->hOMXColorConv, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGView,
    &_hMSLIMGView->tOMXCallbacks
    ), MSL_IMGVIEW_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for overlay*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGView->hOMXOverlay, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGView,
    &_hMSLIMGView->tOMXCallbacks
    ), MSL_IMGVIEW_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for rotate*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGView->hOMXRotate, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGView,
    &_hMSLIMGView->tOMXCallbacks
    ), MSL_IMGVIEW_ERROR_NOMEMORY);

    //TODO: This is possible bug fix for flicker issue in image viewer
    //tMSLGloablFeatures.nMaxSimultaneousImgPreviews = 1;

    _hMSLIMGView->ahOMXSSL = (OMX_HANDLETYPE *)MSL_ALLOC(tMSLGloablFeatures.nMaxSimultaneousImgPreviews * sizeof(OMX_HANDLETYPE));
    MSL_EXIT_IF(_hMSLIMGView->ahOMXSSL == NULL, MSL_IMGVIEW_ERROR_NOMEMORY);

    //initialize with NULL
    for(i = 0; i < tMSLGloablFeatures.nMaxSimultaneousImgPreviews; i++)
    {
        _hMSLIMGView->ahOMXSSL[i]               = NULL;
    }

    for(i = 0; i < tMSLGloablFeatures.nMaxSimultaneousImgPreviews; i++)
    {
        /* get handle to OMX SSL client */
        MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGView->ahOMXSSL[i], 
        OMXSSL_COMP_NAME,
        _hMSLIMGView,
        &_hMSLIMGView->tOMXCallbacks
        ), MSL_IMGVIEW_ERROR_NOMEMORY);
    }

    /*Set the state to none */
    _hMSLIMGView->tImgViewState    = MSL_IMGVIEW_STATE_NONE;
    
    //do default initialization for some non-mandatory parameters
    _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nWidth      = 0;
    _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nHeight     = 0;
    _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nXOffset    = 0;
    _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nYOffset    = 0;

    _hMSLIMGView->tOMXRescaleConfig.nZoomFactor             = MSL_ZOOM_NONE;
    _hMSLIMGView->tOMXRotateConfig.tRotate                  = OMX_TIIMAGE_ROTATE0;
    _hMSLIMGView->tOMXOverlayConfig.pOverlayBuffer          = NULL;

EXIT:
    MSL_PROFILESTUB("MSL_ImgView_Create end");
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Create : Exit");
    return  tRetVal; 
}

/* ========================================================================== */
/**
* @fn  MSL_ImgView_Init : Initializes the pipeline.
*
* @param [in] hIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_Init (MSL_HANDLE hIMGView)
{
    /* dereference pointer */
    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) hIMGView;
    MSL_IMGVIEW_STATUS  tRetVal = MSL_IMGVIEW_STATUS_OK;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Init : Entry");

    MSL_PROFILESTUB("MSL_ImgView Initialization: Started");

    //state if not equal to none give an error 
    MSL_EXIT_IF((_hMSLIMGView->tImgViewState != MSL_IMGVIEW_STATE_NONE), MSL_IMGVIEW_ERROR_INVALID_STATE);

    //initialize display counts
    _hMSLIMGView->unPreviewCount   = 0;
    _hMSLIMGView->bDisplayUsedUp   = MSL_FALSE;
    _hMSLIMGView->bViewInProgress = MSL_FALSE;

    _hMSLIMGView->tOMXRescaleConfig.bInPlace            = OMX_FALSE; 
    _hMSLIMGView->tOMXRescaleConfig.nInputImageWidth    = tMSLGloablFeatures.nMaxImgWidth;
    _hMSLIMGView->tOMXRescaleConfig.nInputImageHeight   = tMSLGloablFeatures.nMaxImgHeight;
        
    _hMSLIMGView->tOMXRescaleConfig.tInputImageFormat   = OMX_TICOLOR_YCbYCr;
    _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth   = _hMSLIMGView->tOMXSSLConfig.nXLen;
    _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight  = _hMSLIMGView->tOMXSSLConfig.nYLen;

    _hMSLIMGView->tOMXOverlayConfig.bInPlace            = OMX_TRUE;
    _hMSLIMGView->tOMXOverlayConfig.nSrcImageWidth      = _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGView->tOMXOverlayConfig.nSrcImageHeight     = _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGView->tOMXOverlayConfig.tSrcImageFormat     = _hMSLIMGView->tOMXSSLConfig.tPlaneImgFormat;    

    _hMSLIMGView->tOMXRotateConfig.bInPlace             = OMX_FALSE;
    _hMSLIMGView->tOMXRotateConfig.nImageWidth          = _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGView->tOMXRotateConfig.nImageHeight         = _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight;    
    _hMSLIMGView->tOMXRotateConfig.tInputImageFormat    = _hMSLIMGView->tOMXSSLConfig.tPlaneImgFormat;

    if(_hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE90 ||
            _hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE270)
    _hMSLIMGView->tOMXRotateConfig.nStride           = _hMSLIMGView->tOMXRotateConfig.nImageHeight;
    else
    _hMSLIMGView->tOMXRotateConfig.nStride           = _hMSLIMGView->tOMXRotateConfig.nImageWidth;

    //set color conversion parameters
    _hMSLIMGView->tOMXColorConvConfig.bInPlace           = OMX_FALSE;
    _hMSLIMGView->tOMXColorConvConfig.nSrcImageWidth     = _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGView->tOMXColorConvConfig.nSrcImageHeight    = _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGView->tOMXColorConvConfig.tInputImageFormat  = OMX_TICOLOR_YCbYCr;
    _hMSLIMGView->tOMXColorConvConfig.tOutputImageFormat = _hMSLIMGView->tOMXSSLConfig.tPlaneImgFormat;

    /*---------------------------------------------------------------------------
    * do configuration for all omx components
    ---------------------------------------------------------------------------*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRescale, 
    (OMX_INDEXTYPE)OMX_IndexConfigRescale,
    &_hMSLIMGView->tOMXRescaleConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT );

    //Note: overlay is conditional configuration, still initialize it with some value
    //to ensure that it is in execute phase and ready to accept buffers
    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXOverlay, 
    (OMX_INDEXTYPE)OMX_IndexConfigOverlay,
    &_hMSLIMGView->tOMXOverlayConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRotate, 
    (OMX_INDEXTYPE)OMX_IndexConfigRotate,
    &_hMSLIMGView->tOMXRotateConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXColorConv, 
    (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
    &_hMSLIMGView->tOMXColorConvConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXDec, 
    (OMX_INDEXTYPE)OMX_IndexConfigDecode,
    &_hMSLIMGView->tOMXDecodeConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

    //start initialization transition
    OMX_SendCommand(_hMSLIMGView->hOMXDec, 
    OMX_CommandStateSet,
    OMX_StateIdle,
    NULL
    );
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Init : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn MSL_ImgView_View :Launches the  ImageViewer Pipeline
*
* @param [in] hIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_View(MSL_HANDLE hIMGView)
{
    /* dereference pointer */
    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) hIMGView;
    MSL_IMGVIEW_STATUS  tRetVal = MSL_IMGVIEW_STATUS_OK;
    MSL_U32 ulSize;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_View : Entry");

    MSL_PROFILESTUB("MSL_ImgView View: Started");

    /*check for the valid state*/
    MSL_EXIT_IF(_hMSLIMGView->tImgViewState != MSL_IMGVIEW_STATE_INIT && _hMSLIMGView->tImgViewState != MSL_IMGVIEW_STATE_VIEW
    ,MSL_IMGVIEW_ERROR_INVALID_STATE);
    MSL_EXIT_IF(_hMSLIMGView->bViewInProgress == MSL_TRUE,MSL_IMGVIEW_ERROR_INVALID_STATE);
    
    /*change state*/
    _hMSLIMGView->tImgViewState = MSL_IMGVIEW_STATE_VIEW;
    _hMSLIMGView->bViewInProgress = MSL_TRUE;

    //calculate the allocated buffer size
    ulSize = tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight;
    if(MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport) 
    {
        ulSize = ulSize * 3;
    } 
    else 
    {    
        ulSize = ulSize * 2;
    }
    _hMSLIMGView->pImgProcBuffHeader0->pBuffer     =_hMSLIMGView->pBuff0;
    _hMSLIMGView->pImgProcBuffHeader0->nAllocLen   = ulSize;

    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
{        ulSize = tMSLGloablFeatures.nMaxImgWidth * tMSLGloablFeatures.nMaxImgHeight;
    if(MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport) 
    {
        ulSize = ulSize * 3;
    } 
    else 
    {    
        ulSize = ulSize * 2;
    }
  }
    else
        ulSize = tMSLGloablFeatures.nMaxCompressedImageSize;
        _hMSLIMGView->pImgProcBuffHeader1->pBuffer     =_hMSLIMGView->pBuff1;
    _hMSLIMGView->pImgProcBuffHeader1->nAllocLen   = ulSize;
   // check if all ssl handles are used up
    if(MSL_TRUE == _hMSLIMGView->bDisplayUsedUp)
    {
        /* if no free ssl handle available free  */
        OMX_SendCommand(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount], 
        OMX_CommandStateSet,
        OMX_StateLoaded,
        NULL
        );
    }
    else
    {
        OMX_SSL_PLANE_CONFIGTYPE tOMXSSLConfig;

        //work on copy of ssl config, as original ssl config is used as a reference to set other parameters
        memcpy(&tOMXSSLConfig, &_hMSLIMGView->tOMXSSLConfig, sizeof(OMX_SSL_PLANE_CONFIGTYPE)); 

        //if rotate by 90 or 270, swap the width & height of affected components
        if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGView->tOMXRotateConfig.tRotate ||
                OMX_TIIMAGE_ROTATE270 == _hMSLIMGView->tOMXRotateConfig.tRotate)
        {
            MSL_SWAP(tOMXSSLConfig.nXLen, tOMXSSLConfig.nYLen);
        }

        MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount],
        (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig ,
        &tOMXSSLConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

    
        /* Set ssl component state to idle */
        OMX_SendCommand(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount], 
        OMX_CommandStateSet,
        OMX_StateIdle,
        NULL
        );
    }

EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_View : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn  MSL_ImgView_Pause : Pauses Imageviewer
* @param [in] phIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_Pause (MSL_HANDLE hIMGView)
{
    return MSL_IMGVIEW_ERROR_NOT_IMPLEMENTED;  
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
OMX_ERRORTYPE MSL_ImgView_OMXFillbufferDone(OMX_HANDLETYPE hComponent , 
OMX_PTR pAppData,
OMX_BUFFERHEADERTYPE* pBuffHeader)
{
    /* dereference pointer */
    MSL_U16 nWidth, nHeight;
    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) pAppData;
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    MSL_BOOL bReleased = MSL_FALSE;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_OMXFillbufferDone : Entry");

    if (_hMSLIMGView->tImgViewState==MSL_IMGVIEW_STATE_VIEW)
    {
       vsi_s_get(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
        if (hComponent == _hMSLIMGView->hOMXDec)
        {
            OMX_TIIMAGE_RESCALE_PARAMTYPE tOMXRescaleConfig;

            MSL_PROFILESTUB("MSL_ImgView View: Dec completed");

            // get info about decode image 
            if(OMX_ErrorNone != OMX_GetConfig(_hMSLIMGView->hOMXDec,
                        (OMX_INDEXTYPE)OMX_IndexConfigDecodeImgInfo ,
                        &_hMSLIMGView->tImgInfo))
            {
                //inform MSL about the error
                _hMSLIMGView->bViewInProgress = MSL_FALSE;
                bReleased = MSL_TRUE;
                vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                _hMSLIMGView->tMSLCallBack(_hMSLIMGView,  MSL_UCP_IMGVIEW,  MSL_CMD_VIEW, MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);    
                MSL_EXIT_IF(1, OMX_ErrorUndefined);
            }

            /*Use the decode image info to configure the parameters and allocate the decode output buffer*/
            _hMSLIMGView->tOMXColorConvConfig.tInputImageFormat = _hMSLIMGView->tImgInfo.tImageFormat;
            _hMSLIMGView->tOMXRescaleConfig.tInputImageFormat   = _hMSLIMGView->tImgInfo.tImageFormat;                        

            nWidth                                              = _hMSLIMGView->tImgInfo.nWidth;
            nHeight                                             = _hMSLIMGView->tImgInfo.nHeight;

            //check for maximum dimension support
            if(nWidth * nHeight > tMSLGloablFeatures.nMaxImgWidth* tMSLGloablFeatures.nMaxImgHeight)
            {
                /* inform MSL about the error and then return */
                 _hMSLIMGView->bViewInProgress = MSL_FALSE;
                 bReleased = MSL_TRUE;
                vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                _hMSLIMGView->tMSLCallBack(_hMSLIMGView, MSL_UCP_IMGVIEW, MSL_CMD_VIEW, MSL_IMGVIEW_ERROR_BAD_STREAM);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);  
            }


            if(_hMSLIMGView->tImgInfo.tImageFormat == OMX_TICOLOR_YUV422H ||
                    _hMSLIMGView->tImgInfo.tImageFormat == OMX_TICOLOR_YUV422V)
            {                  
                nWidth  = ((nWidth + 15)/16) * 16;
                nHeight = ((nHeight + 7)/8) * 8;
            }
            else if (_hMSLIMGView->tImgInfo.tImageFormat == OMX_TICOLOR_YUV420Planar)
            {
                nWidth  = ((nWidth + 15)/16) * 16;
                nHeight = ((nHeight + 15)/16) * 16;
            }
            else if ((_hMSLIMGView->tImgInfo.tImageFormat == OMX_TICOLOR_YUV444Planar ||
                        _hMSLIMGView->tImgInfo.tImageFormat == OMX_TICOLOR_24bitRGB888Planar) &&
                    (MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport) )
            {
                nWidth  = ((nWidth + 7)/8) * 8;
                nHeight = ((nHeight + 7)/8) * 8;
            }
            else
            {
                /* inform MSL about the error and then return */
                 _hMSLIMGView->bViewInProgress = MSL_FALSE;
                 bReleased = MSL_TRUE;
                vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                _hMSLIMGView->tMSLCallBack(_hMSLIMGView, MSL_UCP_IMGVIEW, MSL_CMD_VIEW, MSL_IMGVIEW_ERROR_BAD_STREAM);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);  
            }            

            _hMSLIMGView->tOMXRescaleConfig.nInputImageWidth    = nWidth;
            _hMSLIMGView->tOMXRescaleConfig.nInputImageHeight   = nHeight;

            //work on local copy of rescale config
            memcpy(&tOMXRescaleConfig, &_hMSLIMGView->tOMXRescaleConfig, sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE)); 

            if(tOMXRescaleConfig.tCropWindow.nWidth == 0)
            {
                tOMXRescaleConfig.tCropWindow.nWidth  = _hMSLIMGView->tImgInfo.nWidth; 

            }

            if(tOMXRescaleConfig.tCropWindow.nHeight == 0)
            {            
                tOMXRescaleConfig.tCropWindow.nHeight = _hMSLIMGView->tImgInfo.nHeight; 
            }            

            /*Rescale  config*/
            if(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRescale, 
                        (OMX_INDEXTYPE) OMX_IndexConfigRescale ,
                        &tOMXRescaleConfig))
            {
                /* inform MSL about the error and then return */
                 _hMSLIMGView->bViewInProgress = MSL_FALSE;
                   bReleased = MSL_TRUE;
                vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                _hMSLIMGView->tMSLCallBack(_hMSLIMGView, MSL_UCP_IMGVIEW, MSL_CMD_VIEW, MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);  
            }
            /*color convert config*/
            if(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXColorConv, 
                        (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
                        &_hMSLIMGView->tOMXColorConvConfig))
            {
                /* inform MSL about the error and then return */
                 _hMSLIMGView->bViewInProgress = MSL_FALSE;
                  bReleased = MSL_TRUE;
                vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                _hMSLIMGView->tMSLCallBack(_hMSLIMGView, MSL_UCP_IMGVIEW, MSL_CMD_VIEW, MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);  
            }

            /*send buffer for Rescale*/
            OMX_EmptyThisBuffer(_hMSLIMGView->hOMXRescale, pBuffHeader);
        }

        if(hComponent == _hMSLIMGView->hOMXRescale)
        {
            MSL_PROFILESTUB("MSL_ImgView View: Rescale completed");
            /*send the input buffer*/
            OMX_EmptyThisBuffer(_hMSLIMGView->hOMXColorConv, pBuffHeader);

        }    

        if(hComponent == _hMSLIMGView->hOMXColorConv)
        {
            MSL_PROFILESTUB("MSL_ImgView View: Color conversion completed");
                /*send the input buffer*/
                OMX_EmptyThisBuffer(_hMSLIMGView->hOMXOverlay, pBuffHeader);
        }

        if(hComponent == _hMSLIMGView->hOMXRotate)
        {
            MSL_PROFILESTUB("MSL_ImgView View: rotate completed");
            /*send the input buffer*/
            OMX_EmptyThisBuffer(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount], pBuffHeader);
        }
        if(bReleased == MSL_FALSE)
        vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
    }
EXIT:    
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_OMXFillbufferDone : Exit");
    return tRetVal;
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
OMX_ERRORTYPE MSL_ImgView_OMXEmptybufferDone(OMX_HANDLETYPE hComponent , 
OMX_PTR pAppData, 
OMX_BUFFERHEADERTYPE* pBuffHeader)
{
    /* dereference pointer */
    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) pAppData;
    MSL_BOOL bReleased = MSL_FALSE;
    
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_OMXEmptybufferDone : Entry");

    if(_hMSLIMGView->tImgViewState== MSL_IMGVIEW_STATE_VIEW)
    {
        vsi_s_get(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
        if(hComponent == _hMSLIMGView->hOMXDec)
        {
            OMX_FillThisBuffer(_hMSLIMGView->hOMXRescale, pBuffHeader);
        }

        if(hComponent == _hMSLIMGView->hOMXRescale)
        {
            OMX_FillThisBuffer(_hMSLIMGView->hOMXColorConv, pBuffHeader);
        }

        if(hComponent == _hMSLIMGView->hOMXColorConv)
        {
             if(_hMSLIMGView->tOMXRotateConfig.tRotate != OMX_TIIMAGE_ROTATE0)
                OMX_FillThisBuffer(_hMSLIMGView->hOMXRotate, pBuffHeader);
        }

        if(hComponent == _hMSLIMGView->hOMXOverlay)
        {
            MSL_PROFILESTUB("MSL_ImgView View: Overlay completed");
            if(_hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE0)
            OMX_EmptyThisBuffer(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount], pBuffHeader);                
            else
            OMX_EmptyThisBuffer(_hMSLIMGView->hOMXRotate, pBuffHeader);
        }

        if(hComponent == _hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount])
        {
            MSL_PROFILESTUB("MSL_ImgView View: Completed");
           _hMSLIMGView->unPreviewCount++;

           if(_hMSLIMGView->unPreviewCount == tMSLGloablFeatures.nMaxSimultaneousImgPreviews)
           {
               _hMSLIMGView->unPreviewCount = 0;
               //Note: this needs to be set only once, but no harm doing every time
               _hMSLIMGView->bDisplayUsedUp = MSL_TRUE;
           }
            /* inform MSL about the event and return */
            _hMSLIMGView->bViewInProgress = MSL_FALSE;
             vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
             bReleased  = MSL_TRUE;
            _hMSLIMGView->tMSLCallBack(_hMSLIMGView, MSL_UCP_IMGVIEW, MSL_CMD_VIEW, MSL_IMGVIEW_STATUS_OK);
        }
        if(bReleased == MSL_FALSE)
           vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
    }

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_OMXEmptybufferDone : Exit");

    return OMX_ErrorNone;
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
OMX_ERRORTYPE MSL_ImgView_OMXEventHandle(
OMX_HANDLETYPE hComponent,
OMX_PTR pAppData,               
OMX_EVENTTYPE eEvent,    
OMX_U32 nData1,                            
OMX_U32 nData2,            
OMX_PTR pEventData)
{
    OMX_STATETYPE tTransitionState;
    MSL_IMGVIEW_PRIV_STRUCT *_hMSLIMGView   = (MSL_IMGVIEW_PRIV_STRUCT * )pAppData;
    MSL_BOOL bReleased = MSL_FALSE;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_OMXEventHandle : Entry");
    vsi_s_get(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
    if(MSL_IMGVIEW_STATE_NONE == _hMSLIMGView->tImgViewState ||
            MSL_IMGVIEW_STATE_VIEW == _hMSLIMGView->tImgViewState)
    {
        tTransitionState = (OMX_STATETYPE )nData2; 
    }    
    else if(MSL_IMGVIEW_STATE_DEINIT == _hMSLIMGView->tImgViewState)
    {
        tTransitionState = OMX_StateLoaded;
    }


    //check for state transition
    if(OMX_CommandStateSet == nData1 && OMX_EventCmdComplete == eEvent)
    {
        if(tTransitionState == nData2)
        {
            if(hComponent == _hMSLIMGView->hOMXDec)
            {
                OMX_SendCommand(_hMSLIMGView->hOMXRescale,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );  
            }

            if(hComponent == _hMSLIMGView->hOMXRescale)
            {
                OMX_SendCommand(_hMSLIMGView->hOMXColorConv,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );     
            }

            if(hComponent == _hMSLIMGView->hOMXColorConv)
            {
                OMX_SendCommand(_hMSLIMGView->hOMXRotate,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );     
            }


            if(hComponent == _hMSLIMGView->hOMXRotate)
            {
                OMX_SendCommand(_hMSLIMGView->hOMXOverlay,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );  
            }

            if(hComponent == _hMSLIMGView->hOMXOverlay)
            {
                if(MSL_IMGVIEW_STATE_NONE == _hMSLIMGView->tImgViewState)
                {
                    if(OMX_StateIdle == tTransitionState) 
                    {
                        //after all components are transitioned to idle in init state, change all components to executing state
                        OMX_SendCommand(_hMSLIMGView->hOMXDec,  
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL
                        );  
                    }
                    else
                    {                             
                        /* change the state to init and inform msl */
                        _hMSLIMGView->tImgViewState = MSL_IMGVIEW_STATE_INIT;

                        MSL_PROFILESTUB("MSL_ImgView Initialization: Completed");
                        bReleased = MSL_TRUE;
                        vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                        /* INFORM MSL that imgview init is over */
                        _hMSLIMGView->tMSLCallBack(_hMSLIMGView,  MSL_UCP_IMGVIEW,  MSL_CMD_INIT, MSL_IMGVIEW_STATUS_OK);

                    }
                }            
                else if(MSL_IMGVIEW_STATE_DEINIT == _hMSLIMGView->tImgViewState)
                {               
                    //set preview count as zero and start deinitializing all ssl components
                    _hMSLIMGView->unPreviewCount = 0;
                    bReleased = MSL_TRUE;
                    vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                    OMX_SendCommand(_hMSLIMGView->ahOMXSSL[0],  
                    OMX_CommandStateSet,
                    OMX_StateLoaded,
                    NULL
                    );                  
                }
            }

            if(hComponent == _hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount])
            {
                if(MSL_IMGVIEW_STATE_DEINIT == _hMSLIMGView->tImgViewState)
                {
                    _hMSLIMGView->unPreviewCount++;

                    if(_hMSLIMGView->unPreviewCount < tMSLGloablFeatures.nMaxSimultaneousImgPreviews)
                    {
                        bReleased = MSL_TRUE;
                        vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                        OMX_SendCommand(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount],  
                        OMX_CommandStateSet,
                        OMX_StateLoaded,
                        NULL
                        );
                    }     
                    else
                    {
                        /*change the state to NONE and inform msl*/
                        _hMSLIMGView->tImgViewState = MSL_IMGVIEW_STATE_NONE;

                        MSL_PROFILESTUB("MSL_ImgView DeInitialization: Completed");
                        bReleased = MSL_TRUE;
                        vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                
                        /*INFORM application about the transition complete*/
                        _hMSLIMGView->tMSLCallBack(_hMSLIMGView,  MSL_UCP_IMGVIEW,  MSL_CMD_DEINIT, MSL_IMGVIEW_STATUS_OK);        
                    }
                }
                else if(MSL_IMGVIEW_STATE_VIEW == _hMSLIMGView->tImgViewState)
                {
                    if(OMX_StateLoaded == tTransitionState)
                    {
                        OMX_SSL_PLANE_CONFIGTYPE tOMXSSLConfig;

                        //work on copy of ssl config, as original ssl config is used as a reference to set other parameters
                        memcpy(&tOMXSSLConfig, &_hMSLIMGView->tOMXSSLConfig, sizeof(OMX_SSL_PLANE_CONFIGTYPE)); 

                        //if rotate by 90 or 270, swap the width & height of affected components
                        if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGView->tOMXRotateConfig.tRotate ||
                                OMX_TIIMAGE_ROTATE270 == _hMSLIMGView->tOMXRotateConfig.tRotate)
                        {
                            MSL_SWAP(tOMXSSLConfig.nXLen, tOMXSSLConfig.nYLen);
                        }

                        OMX_SetConfig(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount],
                        (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig ,
                        &tOMXSSLConfig);

                        /* Set ssl component state to idle */
                        OMX_SendCommand(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount], 
                        OMX_CommandStateSet,
                        OMX_StateIdle,
                        NULL
                        );
                    }
                    else if(OMX_StateIdle == tTransitionState)
                    {
                         
                        /* Set ssl component state to idle */
                        OMX_SendCommand(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount], 
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL
                        );
                    }
                    else if(OMX_StateExecuting == tTransitionState)
                    {
                    MSL_PROFILESTUB("SSL moved to executing");
                        if (MSL_ImgView_FillDecoderBuff(_hMSLIMGView) != MSL_IMGVIEW_STATUS_OK)
                        {
                            /*INFORM application about the transition complete*/
                             _hMSLIMGView->bViewInProgress = MSL_FALSE;
                             bReleased = MSL_TRUE;
                             vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
                             _hMSLIMGView->tMSLCallBack(_hMSLIMGView,  MSL_UCP_IMGVIEW,  MSL_CMD_VIEW, MSL_IMGVIEW_ERROR_IOREAD);

                        }
                        else
                        {

                            MSL_PROFILESTUB("MSL_ImgView Decoder: Started");
                            /* send buffer to decoder input */
                            OMX_EmptyThisBuffer(_hMSLIMGView->hOMXDec, _hMSLIMGView->pImgProcBuffHeader1);
                            /* Send buffer to decoder output */                    
                            OMX_FillThisBuffer(_hMSLIMGView->hOMXDec, _hMSLIMGView->pImgProcBuffHeader0);
                        }
                    }
                }
            }
        }
    }
    else
    {
        MSL_CMDTYPE tCMD;
        switch(_hMSLIMGView->tImgViewState)
        {
        case MSL_IMGVIEW_STATE_NONE:
        case MSL_IMGVIEW_STATE_INIT:
            tCMD = MSL_CMD_INIT;
            break;
        case MSL_IMGVIEW_STATE_VIEW:
            tCMD = MSL_CMD_VIEW;
            break;
        case MSL_IMGVIEW_STATE_DEINIT:
            tCMD = MSL_CMD_DEINIT;
            break;
        default:
            tCMD = MSL_CMD_INIT;
        }
        _hMSLIMGView->bViewInProgress = MSL_FALSE;
         bReleased = MSL_TRUE;
        vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
        _hMSLIMGView->tMSLCallBack(_hMSLIMGView,  MSL_UCP_IMGVIEW,  tCMD, MSL_IMGVIEW_ERROR_UNKNOWN);   
    }
    if(bReleased == MSL_FALSE)
       vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_OMXEventHandle : Exit");
    return OMX_ErrorNone;
}


/* ========================================================================== */
/**
* @fn MSL_ImgView_SetConfig  : Set Configurations for the cmponent indicated by index  in UCP
*
* @param [in] hIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_SetConfig (MSL_HANDLE hIMGView, MSL_INDEXTYPES tIndex, MSL_VOID *pParam)
{
    MSL_IMG_OVERLAYCONFIG       *tOverlayConfig;    
    MSL_DISPLAY_CONFIGTYPE      *tDisplayConfig;
    MSL_IMG_WINDOWTYPE          *tMSLWindowConfig;
    MSL_FILE_CONFIGTYPE         *tMSLDecFileConfig;
    MSL_IMGVIEW_PRIV_STRUCT     *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) hIMGView;
    MSL_IMGVIEW_STATUS                   tRetVal = MSL_IMGVIEW_STATUS_OK;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_SetConfig : Entry");
    vsi_s_get(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
    switch(tIndex)
    {
    case MSL_CALLBACKSET_CONFIGINDEX:
        MSL_EXIT_IF(MSL_IMGVIEW_STATE_NONE != _hMSLIMGView->tImgViewState, MSL_IMGVIEW_ERROR_INVALID_STATE)
        /* set msl callback function */
        _hMSLIMGView->tMSLCallBack                          = ((MSL_CALLBACK ) pParam);
        break;

    case MSL_DISPLAY_CONFIGINDEX:
        tDisplayConfig                                      = (MSL_DISPLAY_CONFIGTYPE *) pParam;

        MSL_EXIT_IF( (tDisplayConfig->unDisplayImgWidth > tMSLGloablFeatures.nMaxImgWidth) ||
        (tDisplayConfig->unDisplayImgHeight > tMSLGloablFeatures.nMaxImgHeight), MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

        /* omx ssl configuration */       
        _hMSLIMGView->tOMXSSLConfig.nXLen                   = tDisplayConfig->unDisplayImgWidth;
        _hMSLIMGView->tOMXSSLConfig.nYLen                   = tDisplayConfig->unDisplayImgHeight;
        _hMSLIMGView->tOMXSSLConfig.nXOffset                = tDisplayConfig->unDisplayXOffset;
        _hMSLIMGView->tOMXSSLConfig.nYOffset                = tDisplayConfig->unDisplayYOffset;
        _hMSLIMGView->tOMXSSLConfig.tPlaneImgFormat         = MSL2OMXColor(tDisplayConfig->tImgFormat);
        _hMSLIMGView->tOMXSSLConfig.bAlwaysOnTop            = OMX_FALSE;
        _hMSLIMGView->tOMXSSLConfig.tPlaneBlendType         = OMX_SSL_PLANEBLEND_OVERLAP;
        _hMSLIMGView->tOMXSSLConfig.bDeferredUpdate         = OMX_FALSE;
        _hMSLIMGView->tOMXSSLConfig.bDSAPlane               = OMX_FALSE;

        //set all dependent component config spec too
        if(MSL_IMGVIEW_STATE_NONE != _hMSLIMGView->tImgViewState)
        {
            OMX_SSL_PLANE_CONFIGTYPE tOMXSSLConfig;
            memcpy(&tOMXSSLConfig, &_hMSLIMGView->tOMXSSLConfig, sizeof(OMX_SSL_PLANE_CONFIGTYPE)); 

            if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGView->tOMXRotateConfig.tRotate ||
                    OMX_TIIMAGE_ROTATE270 == _hMSLIMGView->tOMXRotateConfig.tRotate)
            {
                //swap width and height of ssl
                MSL_SWAP(tOMXSSLConfig.nXLen, tOMXSSLConfig.nYLen);
            }

            _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth  = _hMSLIMGView->tOMXSSLConfig.nXLen;
            _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight = _hMSLIMGView->tOMXSSLConfig.nYLen;

            _hMSLIMGView->tOMXOverlayConfig.nSrcImageWidth      = _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth;
            _hMSLIMGView->tOMXOverlayConfig.nSrcImageHeight     = _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight;

            _hMSLIMGView->tOMXRotateConfig.nImageWidth          = _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth;
            _hMSLIMGView->tOMXRotateConfig.nImageHeight         = _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight; 

            if(_hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE90 ||
                    _hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE270)
            _hMSLIMGView->tOMXRotateConfig.nStride              = _hMSLIMGView->tOMXRotateConfig.nImageHeight;
            else
            _hMSLIMGView->tOMXRotateConfig.nStride              = _hMSLIMGView->tOMXRotateConfig.nImageWidth;

            _hMSLIMGView->tOMXColorConvConfig.nSrcImageWidth    = _hMSLIMGView->tOMXRescaleConfig.nOutputImageWidth;
            _hMSLIMGView->tOMXColorConvConfig.nSrcImageHeight   = _hMSLIMGView->tOMXRescaleConfig.nOutputImageHeight;

            /*---------------------------------------------------------------------------
                * do configuration for all affected omx components
                ---------------------------------------------------------------------------*/               
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRescale, 
            (OMX_INDEXTYPE)OMX_IndexConfigRescale,
            &_hMSLIMGView->tOMXRescaleConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

            //overlay is conditional configuration, but still set the parameters
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXOverlay, 
            (OMX_INDEXTYPE)OMX_IndexConfigOverlay,
            &_hMSLIMGView->tOMXOverlayConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRotate, 
            (OMX_INDEXTYPE)OMX_IndexConfigRotate,
            &_hMSLIMGView->tOMXRotateConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXColorConv, 
            (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
            &_hMSLIMGView->tOMXColorConvConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount],
            (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig ,
            &tOMXSSLConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);
        }

        break;

    case MSL_ZOOM_CONFIGINDEX:
        // zoom config 
        _hMSLIMGView->tOMXRescaleConfig.nZoomFactor         = *((MSL_U16 *)pParam);
       if(MSL_IMGVIEW_STATE_NONE != _hMSLIMGView->tImgViewState)
        {
            //reconfig rescale imge.
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRescale, 
            (OMX_INDEXTYPE)OMX_IndexConfigRescale,
            &_hMSLIMGView->tOMXRescaleConfig), MSL_IMGVIEW_ERROR_INVALID_ARGUMENT );
        }
        break;

    case MSL_OVERLAY_CONFIGINDEX:
        tOverlayConfig                                           = (MSL_IMG_OVERLAYCONFIG *) pParam;

        /* img overlay configuration for vf */     
        _hMSLIMGView->tOMXOverlayConfig.nOverlayImageWidth           = tOverlayConfig->nImgWidth;
        _hMSLIMGView->tOMXOverlayConfig.nOverlayImageHeight          = tOverlayConfig->nImgHeight;
        _hMSLIMGView->tOMXOverlayConfig.nSrcXOffset                  = tOverlayConfig->nOverlayXOffset;
        _hMSLIMGView->tOMXOverlayConfig.nSrcYOffset                  = tOverlayConfig->nOverlayXOffset;
        _hMSLIMGView->tOMXOverlayConfig.pOverlayBuffer               = tOverlayConfig->pOverlayBuff;
        _hMSLIMGView->tOMXOverlayConfig.nOverlay                     = tOverlayConfig->nTransparencyColor; 
        _hMSLIMGView->tOMXOverlayConfig.nOverlay                     = tOverlayConfig->nTransparencyColor; 
        _hMSLIMGView->tOMXOverlayConfig.tOverlay                     = MSL2OMXOverlay(tOverlayConfig->tOverlayMode);
        _hMSLIMGView->tOMXOverlayConfig.nAlpha                       = tOverlayConfig->nAlpha;
        if(MSL_IMGVIEW_STATE_NONE != _hMSLIMGView->tImgViewState)
        {
            //reconfig rescale imge.
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXOverlay, 
            (OMX_INDEXTYPE)OMX_IndexConfigOverlay,
            &_hMSLIMGView->tOMXOverlayConfig), MSL_IMGVIEW_ERROR_INVALID_ARGUMENT );
        }
        break; 


    case MSL_CROPWINDOW_CONFIGINDEX:
        tMSLWindowConfig = (MSL_IMG_WINDOWTYPE *) pParam;
        _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nWidth      = tMSLWindowConfig->nImgCropWidth; 
        _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nHeight     = tMSLWindowConfig->nImgCropHeight; 
        _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nXOffset    = tMSLWindowConfig->nImgXOffset; 
        _hMSLIMGView->tOMXRescaleConfig.tCropWindow.nYOffset    = tMSLWindowConfig->nImgYOffset; 
        break;

    case MSL_ROTATE_CONFIGINDEX:
        _hMSLIMGView->tOMXRotateConfig.tRotate              = MSL2OMXRotate(*((MSL_IMG_ROTATETYPE *)pParam));

        if(MSL_IMGVIEW_STATE_NONE != _hMSLIMGView->tImgViewState)
        {
            //IMG configurations - config rotate img.
            if(_hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE90 ||
                    _hMSLIMGView->tOMXRotateConfig.tRotate == OMX_TIIMAGE_ROTATE270)
            _hMSLIMGView->tOMXRotateConfig.nStride              = _hMSLIMGView->tOMXRotateConfig.nImageHeight;
            else
            _hMSLIMGView->tOMXRotateConfig.nStride              = _hMSLIMGView->tOMXRotateConfig.nImageWidth;

            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->hOMXRotate, 
            (OMX_INDEXTYPE)OMX_IndexConfigRotate,
            &_hMSLIMGView->tOMXRotateConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

            if(OMX_TIIMAGE_ROTATE90 == _hMSLIMGView->tOMXRotateConfig.tRotate ||
                    OMX_TIIMAGE_ROTATE270 == _hMSLIMGView->tOMXRotateConfig.tRotate)
            {
                OMX_SSL_PLANE_CONFIGTYPE tOMXSSLConfig;
                memcpy(&tOMXSSLConfig, &_hMSLIMGView->tOMXSSLConfig, sizeof(OMX_SSL_PLANE_CONFIGTYPE)); 

                //swap width and height of ssl
                MSL_SWAP(tOMXSSLConfig.nXLen, tOMXSSLConfig.nYLen);

                // reconfigure ssl
                MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGView->ahOMXSSL[_hMSLIMGView->unPreviewCount],
                (OMX_INDEXTYPE)OMX_IndexConfigSSLPlaneConfig ,
                &tOMXSSLConfig),  MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);
            }
        }
        break;

    case MSL_DECFILE_CONFIGINDEX:
        tMSLDecFileConfig = (MSL_FILE_CONFIGTYPE *) pParam;

        _hMSLIMGView->tDecodeFileType =  tMSLDecFileConfig->tFileType; 
        strcpy((MSL_STRING)_hMSLIMGView->sInputJPEGFileName, tMSLDecFileConfig->sFileName);

        /*set the decoder configurations*/
        _hMSLIMGView->tOMXDecodeConfig.nDecodeAUSize              = 0;
        _hMSLIMGView->tOMXDecodeConfig.tCropWindow.nHeight        = 0;
        _hMSLIMGView->tOMXDecodeConfig.tCropWindow.nWidth         = 0;
        _hMSLIMGView->tOMXDecodeConfig.tCropWindow.nXOffset       = 0;
        _hMSLIMGView->tOMXDecodeConfig.tCropWindow.nYOffset       = 0;
        _hMSLIMGView->tOMXDecodeConfig.nImageScalingFactor        = 1;
        break;

    default:
        MSL_EXIT_IF(1, MSL_IMGVIEW_ERROR_INVALID_ARGUMENT);

    }
EXIT:
    vsi_s_release(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);  
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_SetConfig : Exit");
    return  tRetVal; 
}


/* ========================================================================== */
/**
* @fn MSL_ImgView_GetConfig   :  Get Configurations for the cmponent indicated byindex  in UCP
*n UCP
*
* @param [in] hIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_GetConfig (MSL_HANDLE hIMGView, MSL_INDEXTYPES tIndex, MSL_VOID *pParam)
{
    MSL_IMGINFO_CONFIGTYPE *tImgConfig;

    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) hIMGView;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_GetConfig : Entry");

    if(MSL_IMGINFO_CONFIGINDEX == tIndex)
    {
        tImgConfig                      = pParam;
        tImgConfig->nActualImgHeight    = _hMSLIMGView->tImgInfo.nActHeight;
        tImgConfig->nActualImgWidth     = _hMSLIMGView->tImgInfo.nActWidth;
        tImgConfig->nExtendedImgHeight  = _hMSLIMGView->tImgInfo.nHeight;
        tImgConfig->nExtendedImgWidth   = _hMSLIMGView->tImgInfo.nWidth;
        tImgConfig->tColorFormat        = __OMX2MSLColorType(_hMSLIMGView->tImgInfo.tImageFormat);
    }

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_GetConfig : Entry");
    return  MSL_IMGVIEW_STATUS_OK;
}


/* ========================================================================== */
/**
* @fn MSL_ImgView_Deinit : Deinitializes the pipeline.
*
* @param [in] hIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_Deinit (MSL_HANDLE hIMGView)
{
    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) hIMGView;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Deinit : Entry");

    _hMSLIMGView->tImgViewState = MSL_IMGVIEW_STATE_DEINIT;
     _hMSLIMGView->bViewInProgress = MSL_FALSE;
    OMX_SendCommand(_hMSLIMGView->hOMXDec,  
    OMX_CommandStateSet,
    OMX_StateLoaded,
    NULL
    );

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Deinit : Exit");
    return  MSL_IMGVIEW_STATUS_OK  ;
}

/* ========================================================================== */
/**
* @fn MSL_ImgView_Destroy: Destroys the pipeline.
*
* @param [in] hIMGView
*     handle to MSLIMG object
*
* @return MSL_IMGVIEW_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGVIEW_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_Destroy (MSL_HANDLE hIMGView)
{
    MSL_U16 i;
    /* dereference the local pointer */
    MSL_IMGVIEW_PRIV_STRUCT  *_hMSLIMGView = (MSL_IMGVIEW_PRIV_STRUCT  *) hIMGView;
    if(_hMSLIMGView->tImgViewState != MSL_IMGVIEW_STATE_DEINIT &&
        _hMSLIMGView->tImgViewState != MSL_IMGVIEW_STATE_NONE)
        return MSL_IMGVIEW_ERROR_INVALID_STATE;
        
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Destroy : Entry");

    /* free omx client handles */
    OMX_FreeHandle(_hMSLIMGView->hOMXDec);
    OMX_FreeHandle(_hMSLIMGView->hOMXRescale);
    OMX_FreeHandle(_hMSLIMGView->hOMXRotate);
    OMX_FreeHandle(_hMSLIMGView->hOMXOverlay);
    OMX_FreeHandle(_hMSLIMGView->hOMXColorConv);

    for(i = 0; i < tMSLGloablFeatures.nMaxSimultaneousImgPreviews; i++)
    {
        OMX_FreeHandle(_hMSLIMGView->ahOMXSSL[i]);
    }

    if(_hMSLIMGView->ahOMXSSL)
    MSL_FREE(_hMSLIMGView->ahOMXSSL);

    /*free the buffers and bufferheaders*/    
    if(_hMSLIMGView->pBuff0)
    MSL_FREE(_hMSLIMGView->pBuff0);
    if(_hMSLIMGView->pBuff1)
    MSL_FREE(_hMSLIMGView->pBuff1);

    if(_hMSLIMGView->pImgProcBuffHeader0)
    MSL_FREE(_hMSLIMGView->pImgProcBuffHeader0);

    if(_hMSLIMGView->pImgProcBuffHeader1)
    MSL_FREE(_hMSLIMGView->pImgProcBuffHeader1);
    vsi_s_close(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);
   os_DestroySemaphore(_hMSLIMGView->nSemCaller, _hMSLIMGView->nSemHandle);

    if(hIMGView)
    MSL_FREE(hIMGView);

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_Destroy : Exit");
    return  MSL_IMGVIEW_STATUS_OK  ;
}

/* ========================================================================== */
/**
* @fn MSL_ImgView_FillDecoderBuff: Reads data from jpeg file and fills decoder
* input buffer 
*
* @param [in] _hMSLIMGView
*     handle to MSLIMG object
*
* @return OMX_TICOLOR_FORMATTYPE
*  
*
*  @see  msl_api.h, msl_imgview.h
*/
/* ========================================================================== */
MSL_IMGVIEW_STATUS MSL_ImgView_FillDecoderBuff (MSL_IMGVIEW_PRIV_STRUCT *_hMSLIMGView)
{
    MSL_IMGVIEW_STATUS               tRetVal = MSL_IMGVIEW_STATUS_OK;
#ifndef WIN32    
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_FillDecoderBuff : Entry");

    MSL_PROFILEENTRY(MSL_ImgView_FillDecoderBuff);

    if(_hMSLIMGView->tImgViewState==MSL_IMGVIEW_STATE_VIEW)
    {
        if(MSL_FILETYPE_FFS == _hMSLIMGView->tDecodeFileType)
        {
            T_FFS_FD JPGFILE;
            T_FFS_STAT stat;

            /*read the file from NAND*/    
            if((JPGFILE = ffs_open((MSL_STRING)_hMSLIMGView->sInputJPEGFileName, FFS_O_RDWR))<EFFS_OK)
            {
                MSL_EXIT_IF(1,  MSL_IMGVIEW_ERROR_IOREAD);  
            }
            MSL_EXIT_IF(ffs_stat((MSL_STRING)_hMSLIMGView->sInputJPEGFileName, (void *)&stat)<EFFS_OK, MSL_IMGVIEW_ERROR_IOREAD);
            MSL_EXIT_IF(stat.size >_hMSLIMGView->pImgProcBuffHeader1->nAllocLen, MSL_IMGVIEW_ERROR_IOREAD);


            /*read into i/p decoder buffer (size to be specified)*/
            if((ffs_read(JPGFILE, _hMSLIMGView->pImgProcBuffHeader1->pBuffer, _hMSLIMGView->pImgProcBuffHeader1->nAllocLen)<0))
            {
                ffs_close (JPGFILE);
                MSL_EXIT_IF(1,  MSL_IMGVIEW_ERROR_IOREAD);  
            }

            ffs_close (JPGFILE);
        }
        else
        { 

#ifdef MSL_USE_RFS
            T_RFS_FD JPGFILE;
            T_WCHAR * sfilename_uc;
            //T_WCHAR sfilename_uc[MSL_MAXFILENAME_LENGTH];
            T_RFS_STAT  stat;
            sfilename_uc= (T_WCHAR *) MSL_ALLOC(MSL_MAXFILENAME_LENGTH);
            convert_u8_to_unicode((MSL_STRING)_hMSLIMGView->sInputJPEGFileName, sfilename_uc);
       

            /*read the file from NAND*/    
            if((JPGFILE = rfs_open(sfilename_uc, RFS_O_RDWR, RFS_IRWXO)) < RFS_EOK)
            {
                MSL_EXIT_IF(1,  MSL_IMGVIEW_ERROR_IOREAD);  
            }

            MSL_EXIT_IF(rfs_stat(sfilename_uc, (void *)&stat)<RFS_EOK, MSL_IMGVIEW_ERROR_IOREAD);
            MSL_EXIT_IF(stat.file_dir.size>_hMSLIMGView->pImgProcBuffHeader1->nAllocLen, MSL_IMGVIEW_ERROR_IOREAD);


            /*read into i/p decoder buffer (size to be specified)*/
            if((rfs_read(JPGFILE, _hMSLIMGView->pImgProcBuffHeader1->pBuffer, _hMSLIMGView->pImgProcBuffHeader1->nAllocLen)<0))
            {
                rfs_close (JPGFILE);
                MSL_EXIT_IF(1,  MSL_IMGVIEW_ERROR_IOREAD);  

            }
            rfs_close (JPGFILE); 

            
            if(sfilename_uc)
            MSL_FREE(sfilename_uc);
#endif
        }

    }

EXIT:
    MSL_PROFILEEXIT(MSL_ImgView_FillDecoderBuff);
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgView_FillDecoderBuff : Exit");
#endif    
    return  tRetVal; 
}

MSL_COLOR_FORMATTYPE __OMX2MSLColorType (OMX_TICOLOR_FORMATTYPE omxColor)
{
    switch(omxColor)
    {
    case OMX_TICOLOR_YCbYCr :
        return MSL_COLOR_YUYV;
    case OMX_TICOLOR_16bitRGB565:
        return MSL_COLOR_RGB565;
    case OMX_TICOLOR_12bitRGB444 :
        return MSL_COLOR_RGB444;
    case OMX_TICOLOR_YUV420Planar:
        return MSL_COLOR_YUV420;
    case OMX_TICOLOR_YUV444Planar:
        return MSL_COLOR_YUV444;
    case OMX_TICOLOR_Monochrome:
        return MSL_COLOR_MONOCHROME;
    default: return MSL_COLOR_YUV420;
    }
}



