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
*!
*! 05-June-2006  Narendran M R(narendranmr@ti.com) : Code update to add profile, traces
*! features. Rearchitected the code. Added overlay feature.
*! 
*!
* ========================================================================= */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdlib.h>
#include <string.h>
#include"_msl_imgthmb.h"
#include "rfs/rfs_fm.h"

/* ========================================================================== */
/**
* @fn MSL_ImgThmb_Create : Creates the pipeline.
*
* @param [in] phMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*
========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Create (MSL_HANDLE *phMSLIMGThmb)
{
    MSL_IMGTHMB_PRIV_STRUCT *_hMSLIMGThmb;
    MSL_U32                 ulSize;
    MSL_IMGTHMB_STATUS               tRetVal = MSL_IMGTHMB_STATUS_OK;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Create : Entry");

    MSL_PROFILESTUB("MSL_ImgThmb_Create start");

    /*Memory allocation */
    *phMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT *)MSL_ALLOC(sizeof(MSL_IMGTHMB_PRIV_STRUCT));
    _hMSLIMGThmb  = *phMSLIMGThmb;

    MSL_EXIT_IF(_hMSLIMGThmb == NULL, MSL_IMGTHMB_ERROR_NOMEMORY);

    _hMSLIMGThmb->tOMXCallbacks.EmptyBufferDone = MSL_ImgThmb_OMXEmptybufferDone;
    _hMSLIMGThmb->tOMXCallbacks.EventHandler    = MSL_ImgThmb_OMXEventHandle;
    _hMSLIMGThmb->tOMXCallbacks.FillBufferDone  = MSL_ImgThmb_OMXFillbufferDone;

    /* initialize all OMX handle to NULL */
    _hMSLIMGThmb->hOMXDec                       = NULL;
    _hMSLIMGThmb->hOMXRescale                   = NULL;
    _hMSLIMGThmb->hOMXColorConv                 = NULL;
    _hMSLIMGThmb->hOMXEnc                       = NULL;

    _hMSLIMGThmb->pBuff0                        = NULL;
    _hMSLIMGThmb->pBuff1                        = NULL;     
    _hMSLIMGThmb->pImgProcBuffHeader0           = NULL;
    _hMSLIMGThmb->pImgProcBuffHeader1           = NULL;

    /*allocate OMX_BUFFERHEADERTYPE for IMG buffer header */
    _hMSLIMGThmb->pImgProcBuffHeader0       = (OMX_BUFFERHEADERTYPE *)MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGThmb->pImgProcBuffHeader0 == NULL, MSL_IMGTHMB_ERROR_NOMEMORY);

    /*allocate OMX_BUFFERHEADERTYPE for IMG buffer header */
    _hMSLIMGThmb->pImgProcBuffHeader1       = (OMX_BUFFERHEADERTYPE *)MSL_ALLOC(sizeof(OMX_BUFFERHEADERTYPE));
    MSL_EXIT_IF(_hMSLIMGThmb->pImgProcBuffHeader1 == NULL, MSL_IMGTHMB_ERROR_NOMEMORY);

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
    _hMSLIMGThmb->pBuff1                           = (OMX_U8*)MSL_ALLOC(ulSize);
    MSL_EXIT_IF(_hMSLIMGThmb->pBuff1 == NULL, MSL_IMGTHMB_ERROR_NOMEMORY);

    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
    {
        _hMSLIMGThmb->pBuff0                           = (OMX_U8*)MSL_ALLOC(ulSize);
        MSL_EXIT_IF(_hMSLIMGThmb->pBuff0 == NULL, MSL_IMGTHMB_ERROR_NOMEMORY);
    }
    else
    {
         _hMSLIMGThmb->pBuff0                           = (OMX_U8*)MSL_ALLOC(MAX_ENCODED_BUFFER_SIZE);
        MSL_EXIT_IF(_hMSLIMGThmb->pBuff0 == NULL, MSL_IMGTHMB_ERROR_NOMEMORY);
    }
    /* initialize the bufferheaders */
    _hMSLIMGThmb->pImgProcBuffHeader1->pBuffer     =_hMSLIMGThmb->pBuff1;
    _hMSLIMGThmb->pImgProcBuffHeader1->nAllocLen   = ulSize;

    _hMSLIMGThmb->pImgProcBuffHeader0->pBuffer     =_hMSLIMGThmb->pBuff0;
    if(tMSLGloablFeatures.bSingleBufferMode == MSL_FALSE)
    {
         _hMSLIMGThmb->pImgProcBuffHeader0->nAllocLen   = ulSize;
     }
    else
    {
         _hMSLIMGThmb->pImgProcBuffHeader0->nAllocLen   = MAX_ENCODED_BUFFER_SIZE;
     }
   

    /* get handle to OMX IMG client for decode*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGThmb->hOMXDec, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGThmb,
    &_hMSLIMGThmb->tOMXCallbacks
    ), MSL_IMGTHMB_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for rescale*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGThmb->hOMXRescale, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGThmb,
    &_hMSLIMGThmb->tOMXCallbacks
    ), MSL_IMGTHMB_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for color convert */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGThmb->hOMXColorConv, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGThmb,
    &_hMSLIMGThmb->tOMXCallbacks
    ), MSL_IMGTHMB_ERROR_NOMEMORY);

    /* get handle to OMX IMG client for encode */
    MSL_EXIT_IF(OMX_ErrorNone != OMX_GetHandle(&_hMSLIMGThmb->hOMXEnc, 
    OMX_TIIMG_COMPONENT,
    _hMSLIMGThmb,
    &_hMSLIMGThmb->tOMXCallbacks
    ), MSL_IMGTHMB_ERROR_NOMEMORY);


    /*Set the state to none */
    _hMSLIMGThmb->tImgThmbState    = MSL_IMGTHMB_STATE_NONE;

    //do default initialization for some non-mandatory parameters
    _hMSLIMGThmb->tOMXRescaleConfig.tCropWindow.nWidth      = 0;
    _hMSLIMGThmb->tOMXRescaleConfig.tCropWindow.nHeight     = 0;
    _hMSLIMGThmb->tOMXRescaleConfig.tCropWindow.nXOffset    = 0;
    _hMSLIMGThmb->tOMXRescaleConfig.tCropWindow.nYOffset    = 0;

    _hMSLIMGThmb->tOMXRescaleConfig.nZoomFactor             = MSL_ZOOM_NONE;
    _hMSLIMGThmb->tOMXEncodeConfig.nQualityFactor           = MSL_DEFAULT_ENCQUALITY;

EXIT:
    MSL_PROFILESTUB("MSL_ImgThmb_Create end");
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Create : Exit");
    return  tRetVal; 
}

/* ========================================================================== */
/**
* @fn  MSL_ImgThmb_Init : Initializes the pipeline.
*
* @param [in] hMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Init (MSL_HANDLE hMSLIMGThmb)
{
    /* dereference pointer */
    MSL_IMGTHMB_PRIV_STRUCT  *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) hMSLIMGThmb;
    MSL_IMGTHMB_STATUS  tRetVal = MSL_IMGTHMB_STATUS_OK;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Init : Entry");

    MSL_PROFILESTUB("MSL_ImgThmb Initialization: Started");

    //state if not equal to none give an error 
    MSL_EXIT_IF((_hMSLIMGThmb->tImgThmbState != MSL_IMGTHMB_STATE_NONE), MSL_IMGTHMB_ERROR_INVALID_STATE);

    _hMSLIMGThmb->tOMXRescaleConfig.bInPlace            = OMX_FALSE; 
    _hMSLIMGThmb->tOMXRescaleConfig.nInputImageWidth    = tMSLGloablFeatures.nMaxImgWidth;
    _hMSLIMGThmb->tOMXRescaleConfig.nInputImageHeight   = tMSLGloablFeatures.nMaxImgHeight;
    _hMSLIMGThmb->tOMXRescaleConfig.tInputImageFormat   = OMX_TICOLOR_YCbYCr;

    //set color conversion parameters
    _hMSLIMGThmb->tOMXColorConvConfig.bInPlace          = OMX_FALSE;
    _hMSLIMGThmb->tOMXColorConvConfig.nSrcImageWidth    = _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGThmb->tOMXColorConvConfig.nSrcImageHeight   = _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGThmb->tOMXColorConvConfig.tInputImageFormat = OMX_TICOLOR_YCbYCr;
    _hMSLIMGThmb->tOMXColorConvConfig.tOutputImageFormat= OMX_TICOLOR_YCbYCr;

    //set encode parameters
    _hMSLIMGThmb->tOMXEncodeConfig.bInsertHeader        = OMX_TRUE;
    _hMSLIMGThmb->tOMXEncodeConfig.nEncodeAUSize        = 0;
    _hMSLIMGThmb->tOMXEncodeConfig.nImageHeight         = _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageHeight;
    _hMSLIMGThmb->tOMXEncodeConfig.nImageWidth          = _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageWidth;
    _hMSLIMGThmb->tOMXEncodeConfig.tImageEncodeFormat   = OMX_TICOLOR_YUV422H;
    _hMSLIMGThmb->tOMXEncodeConfig.tInputImageFormat    = OMX_TICOLOR_YCbYCr;
    _hMSLIMGThmb->tOMXEncodeConfig.tImageCodingType     = OMX_IMAGE_CodingJPEG;
    _hMSLIMGThmb->tOMXEncodeConfig.tStreamingMode        = OMX_TIIMAGE_STREAMINGNONE;


    /*---------------------------------------------------------------------------
    * do configuration for all omx components
    ---------------------------------------------------------------------------*/
    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXRescale, 
    (OMX_INDEXTYPE)OMX_IndexConfigRescale,
    &_hMSLIMGThmb->tOMXRescaleConfig), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT );

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXColorConv, 
    (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
    &_hMSLIMGThmb->tOMXColorConvConfig), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXDec, 
    (OMX_INDEXTYPE)OMX_IndexConfigDecode,
    &_hMSLIMGThmb->tOMXDecodeConfig), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);

    MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXEnc, 
    (OMX_INDEXTYPE)OMX_IndexConfigEncode,
    &_hMSLIMGThmb->tOMXEncodeConfig), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);

    //start initialization transition
    OMX_SendCommand(_hMSLIMGThmb->hOMXDec, 
    OMX_CommandStateSet,
    OMX_StateIdle,
    NULL
    );
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Init : Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn MSL_ImgThmb_Generate:Generates the Pipeline
*
* @param [in] hMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Generate(MSL_HANDLE hMSLIMGThmb)
{
    /* dereference pointer */
    MSL_IMGTHMB_PRIV_STRUCT  *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) hMSLIMGThmb;
    MSL_IMGTHMB_STATUS  tRetVal = MSL_IMGTHMB_STATUS_OK;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Generatet : Entry");

    MSL_PROFILESTUB("MSL_ImgThmb_Generate Generate: Started");

    /*check for the valid state*/
    MSL_EXIT_IF(_hMSLIMGThmb->tImgThmbState != MSL_IMGTHMB_STATE_INIT,
    MSL_IMGTHMB_ERROR_INVALID_STATE);

    /*change state*/
    _hMSLIMGThmb->tImgThmbState = MSL_IMGTHMB_STATE_GENERATE;    

    if( MSL_ImgThmb_FillDecoderBuff(_hMSLIMGThmb) != MSL_IMGTHMB_STATUS_OK)
    {
        tRetVal = MSL_IMGTHMB_ERROR_IOREAD;

    }
    else
    {        
        /* send buffer to decoder input */
        OMX_EmptyThisBuffer(_hMSLIMGThmb->hOMXDec, _hMSLIMGThmb->pImgProcBuffHeader0);

        /* Send buffer to decoder output */                    
        OMX_FillThisBuffer(_hMSLIMGThmb->hOMXDec, _hMSLIMGThmb->pImgProcBuffHeader1);
    }


EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Generate: Exit");
    return  tRetVal;
}

/* ========================================================================== */
/**
* @fn  MSL_ImgThmb_Pause : Pauses image thumb
* @param [in] hMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Pause (MSL_HANDLE hMSLIMGThmb)
{
    return MSL_IMGTHMB_ERROR_NOT_IMPLEMENTED;  
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
OMX_ERRORTYPE MSL_ImgThmb_OMXFillbufferDone(OMX_HANDLETYPE hComponent , 
OMX_PTR pAppData,
OMX_BUFFERHEADERTYPE* pBuffHeader)
{
    /* dereference pointer */
    MSL_U16 nWidth, nHeight;
    MSL_IMGTHMB_PRIV_STRUCT  *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) pAppData;
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_OMXFillbufferDone : Entry");

    if (_hMSLIMGThmb->tImgThmbState == MSL_IMGTHMB_STATE_GENERATE)
    {
        if (hComponent == _hMSLIMGThmb->hOMXDec)
        {
            MSL_PROFILESTUB("MSL_ImgThmb : Decoding completed");

            // get info about decode image 
            if(OMX_ErrorNone != OMX_GetConfig(_hMSLIMGThmb->hOMXDec,
                        (OMX_INDEXTYPE)OMX_IndexConfigDecodeImgInfo ,
                        &_hMSLIMGThmb->tImgInfo))
            {
                //inform MSL about the error
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb,  MSL_UCP_IMGTHMB,  MSL_CMD_GENERATE, MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);    
                MSL_EXIT_IF(1, OMX_ErrorUndefined);
            }

            /*Use the decode image info to configure the parameters and allocate the decode output buffer*/
            _hMSLIMGThmb->tOMXColorConvConfig.tInputImageFormat = _hMSLIMGThmb->tImgInfo.tImageFormat;
            _hMSLIMGThmb->tOMXRescaleConfig.tInputImageFormat   = _hMSLIMGThmb->tImgInfo.tImageFormat;

            _hMSLIMGThmb->tOMXRescaleConfig.tCropWindow.nWidth  = _hMSLIMGThmb->tImgInfo.nWidth; 
            _hMSLIMGThmb->tOMXRescaleConfig.tCropWindow.nHeight = _hMSLIMGThmb->tImgInfo.nHeight; 

            nWidth                                              = _hMSLIMGThmb->tImgInfo.nWidth;
            nHeight                                             = _hMSLIMGThmb->tImgInfo.nHeight;

            //check for maximum dimension support
            if(nWidth * nHeight > SNAPSHOT_HEIGHT* SNAPSHOT_WIDTH)
            {
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb, MSL_UCP_IMGTHMB, MSL_CMD_GENERATE, MSL_IMGTHMB_ERROR_BAD_STREAM);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);
            }           

            if(_hMSLIMGThmb->tImgInfo.tImageFormat == OMX_TICOLOR_YUV422H ||
                    _hMSLIMGThmb->tImgInfo.tImageFormat == OMX_TICOLOR_YUV422V)
            {                  
                nWidth  = ((nWidth + 15)/16) * 16;
                nHeight = ((nHeight + 7)/8) * 8;
            }
            else if (_hMSLIMGThmb->tImgInfo.tImageFormat == OMX_TICOLOR_YUV420Planar)
            {
                nWidth  = ((nWidth + 15)/16) * 16;
                nHeight = ((nHeight + 15)/16) * 16;
            }
            else if ((_hMSLIMGThmb->tImgInfo.tImageFormat == OMX_TICOLOR_YUV444Planar ||
                        _hMSLIMGThmb->tImgInfo.tImageFormat == OMX_TICOLOR_24bitRGB888Planar) &&
                    (MSL_TRUE == tMSLGloablFeatures.b24BitFormatSupport))
            {
                nWidth  = ((nWidth + 7)/8) * 8;
                nHeight = ((nHeight + 7)/8) * 8;
            }
            else
            {
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb, MSL_UCP_IMGTHMB, MSL_CMD_GENERATE, MSL_IMGTHMB_ERROR_BAD_STREAM);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);
            }            

            _hMSLIMGThmb->tOMXRescaleConfig.nInputImageWidth    = nWidth;
            _hMSLIMGThmb->tOMXRescaleConfig.nInputImageHeight   = nHeight;

            /*Rescale  config*/
            if(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXRescale, 
                        (OMX_INDEXTYPE)OMX_IndexConfigRescale ,
                        &_hMSLIMGThmb->tOMXRescaleConfig))
            {
                /* inform MSL about the error and then return */
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb, MSL_UCP_IMGTHMB, MSL_CMD_GENERATE, MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);  
            }

            /*COLOR CONVERT  config*/
            if(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXColorConv, 
                        (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
                        &_hMSLIMGThmb->tOMXColorConvConfig))
            {
                /* inform MSL about the error and then return */
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb, MSL_UCP_IMGTHMB, MSL_CMD_GENERATE, MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);
                MSL_EXIT_IF(1, OMX_ErrorInsufficientResources);  
            }

            /*send buffer for Rescale*/
            OMX_EmptyThisBuffer(_hMSLIMGThmb->hOMXRescale, pBuffHeader);
        }

        if(hComponent == _hMSLIMGThmb->hOMXRescale)
        {
            MSL_PROFILESTUB("MSL_ImgThmb : Rescale completed");
            /*send the input buffer*/
            OMX_EmptyThisBuffer(_hMSLIMGThmb->hOMXColorConv, pBuffHeader);
        }    

        if(hComponent == _hMSLIMGThmb->hOMXColorConv)
        {
            MSL_PROFILESTUB("MSL_ImgThmb : color conversion completed");
            /*send the input buffer*/
            OMX_EmptyThisBuffer(_hMSLIMGThmb->hOMXEnc, pBuffHeader);
        }

        if(hComponent == _hMSLIMGThmb->hOMXEnc)
        {

            if( MSL_IMGTHMB_STATUS_OK != MSL_ImgThmb_SaveEncodedBuffer(_hMSLIMGThmb, pBuffHeader))
            {
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb, MSL_UCP_IMGTHMB, MSL_CMD_GENERATE, MSL_IMGTHMB_ERROR_IOWRITE);
            }
            else
            {
                MSL_PROFILESTUB("MSL_ImgThmb Generate: Completed");

                //revert state to init for next generate cycle
                _hMSLIMGThmb->tImgThmbState = MSL_IMGTHMB_STATE_INIT;

                /* inform MSL about the event and return */
                _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb, MSL_UCP_IMGTHMB, MSL_CMD_GENERATE, MSL_IMGTHMB_STATUS_OK);
            }

        }
    }
EXIT:    
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_OMXFillbufferDone : Exit");
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
OMX_ERRORTYPE MSL_ImgThmb_OMXEmptybufferDone(OMX_HANDLETYPE hComponent , 
OMX_PTR pAppData, 
OMX_BUFFERHEADERTYPE* pBuffHeader)
{
    /* dereference pointer */
    MSL_IMGTHMB_PRIV_STRUCT  *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) pAppData;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_OMXEmptybufferDone : Entry");

    if(_hMSLIMGThmb->tImgThmbState== MSL_IMGTHMB_STATE_GENERATE)
    {
        if(hComponent == _hMSLIMGThmb->hOMXDec)
        {
            OMX_FillThisBuffer(_hMSLIMGThmb->hOMXRescale, pBuffHeader);
        }

        if(hComponent == _hMSLIMGThmb->hOMXRescale)
        {
            OMX_FillThisBuffer(_hMSLIMGThmb->hOMXColorConv, pBuffHeader);
        }

        if(hComponent == _hMSLIMGThmb->hOMXColorConv)
        {
            OMX_FillThisBuffer(_hMSLIMGThmb->hOMXEnc, pBuffHeader);
        }
    }
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_OMXEmptybufferDone : Exit");

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
OMX_ERRORTYPE MSL_ImgThmb_OMXEventHandle(
OMX_HANDLETYPE hComponent,
OMX_PTR pAppData,               
OMX_EVENTTYPE eEvent,    
OMX_U32 nData1,                            
OMX_U32 nData2,            
OMX_PTR pEventData)
{
    OMX_STATETYPE tTransitionState;

    MSL_IMGTHMB_PRIV_STRUCT *_hMSLIMGThmb   = (MSL_IMGTHMB_PRIV_STRUCT * )pAppData;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_OMXEventHandle : Entry");

    if(MSL_IMGTHMB_STATE_NONE == _hMSLIMGThmb->tImgThmbState)
    {
        tTransitionState = (OMX_STATETYPE)nData2;
    }    
    else if(MSL_IMGTHMB_STATE_DEINIT == _hMSLIMGThmb->tImgThmbState)
    {
        tTransitionState = OMX_StateLoaded;
    }

    //check for state transition
    if(OMX_CommandStateSet == nData1  && OMX_EventCmdComplete == eEvent)
    {
        if(tTransitionState == nData2)
        {
            if(hComponent == _hMSLIMGThmb->hOMXDec)
            {
                OMX_SendCommand(_hMSLIMGThmb->hOMXRescale,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );     
            }

            if(hComponent == _hMSLIMGThmb->hOMXRescale)
            {
                OMX_SendCommand(_hMSLIMGThmb->hOMXColorConv,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );     
            }

            if(hComponent == _hMSLIMGThmb->hOMXColorConv)
            {
                OMX_SendCommand(_hMSLIMGThmb->hOMXEnc,  
                OMX_CommandStateSet,
                tTransitionState,
                NULL
                );     
            }

            if(hComponent == _hMSLIMGThmb->hOMXEnc)
            {
                if(MSL_IMGTHMB_STATE_NONE == _hMSLIMGThmb->tImgThmbState)
                {
                    if(OMX_StateIdle == tTransitionState) 
                    {
                        //after all components are transitioned to idle in init state, change all components to executing state
                        OMX_SendCommand(_hMSLIMGThmb->hOMXDec,  
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL
                        );     
                    }
                    else
                    {                             
                        /* change the state to init and inform msl */
                        _hMSLIMGThmb->tImgThmbState = MSL_IMGTHMB_STATE_INIT;

                        MSL_PROFILESTUB("MSL_ImgThmb Initialization: Completed");

                        /* INFORM MMI that init is over */
                        _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb,  MSL_UCP_IMGTHMB,  MSL_CMD_INIT, MSL_IMGTHMB_STATUS_OK);

                    }
                }            
                else if(MSL_IMGTHMB_STATE_DEINIT == _hMSLIMGThmb->tImgThmbState)
                {               
                    /* change the state to init and inform msl */
                    _hMSLIMGThmb->tImgThmbState = MSL_IMGTHMB_STATE_NONE;

                    MSL_PROFILESTUB("MSL_ImgThmb DeInitialization: Completed");

                    /* INFORM MMI that init is over */
                    _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb,  MSL_UCP_IMGTHMB,  MSL_CMD_DEINIT, MSL_IMGTHMB_STATUS_OK);
                }
            }

        }
    }
    else
    {
        MSL_CMDTYPE tCMD;
        switch(_hMSLIMGThmb->tImgThmbState)
        {
            case MSL_IMGTHMB_STATE_NONE:
            case MSL_IMGTHMB_STATE_INIT:
                tCMD = MSL_CMD_INIT;
                break;
            case MSL_IMGTHMB_STATE_GENERATE:
                tCMD = MSL_CMD_GENERATE;
                break;
            case MSL_IMGTHMB_STATE_DEINIT:
                tCMD = MSL_CMD_DEINIT;
                break;
            default:
                tCMD = MSL_CMD_INIT;
            }
            MSL_DETAILFUNC_TRACE("MSL_ImgThmb_OMXEventHandl: Error event callback. State: %d\t Event: %d\n", _hMSLIMGThmb->tImgThmbState, eEvent);
            _hMSLIMGThmb->tMSLCallBack(_hMSLIMGThmb,  MSL_UCP_IMGTHMB,  tCMD, MSL_IMGTHMB_ERROR_UNKNOWN);
      }
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_OMXEventHandl : Exit");
    return OMX_ErrorNone;
}


/* ========================================================================== */
/**
* @fn MSL_ImgThmb_SetConfig  : Set Configurations for the cmponent indicated by index  in UCP
*
* @param [in] hMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_SetConfigs (MSL_HANDLE hMSLIMGThmb, MSL_INDEXTYPES tIndex, MSL_VOID *pParam)
{
    MSL_IMG_OVERLAYCONFIG       *tOverlayConfig;    
    MSL_DISPLAY_CONFIGTYPE      *tDisplayConfig;
    MSL_IMG_WINDOWTYPE          *tMSLWindowConfig;
    MSL_FILE_CONFIGTYPE         *tMSLFileConfig;
    MSL_RESCALE_CONFIGTYPE      *tRescaleConfig;
    MSL_IMGTHMB_PRIV_STRUCT     *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) hMSLIMGThmb;
    MSL_IMGTHMB_STATUS                   tRetVal = MSL_IMGTHMB_STATUS_OK;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_SetConfig : Entry");

    switch(tIndex)
    {
    case MSL_CALLBACKSET_CONFIGINDEX:
        // MSL_EXIT_IF(MSL_IMGTHMB_STATE_NONE != _hMSLIMGThmb->tImgThmbState, MSL_IMGTHMB_ERROR_INVALID_STATE);
        /* set msl callback function */
        _hMSLIMGThmb->tMSLCallBack                          = ((MSL_CALLBACK) pParam);
        break;

    case MSL_RESCALE_CONFIGINDEX:
        // MSL_EXIT_IF(MSL_IMGTHMB_STATE_GENERATE == _hMSLIMGThmb->tImgThmbState, MSL_IMGTHMB_ERROR_INVALID_STATE);
        tRescaleConfig                                      = (MSL_RESCALE_CONFIGTYPE *) pParam;

        MSL_EXIT_IF( ((tRescaleConfig->unRescaledImgWidth > tMSLGloablFeatures.nMaxImgWidth) ||
        (tRescaleConfig->unRescaledImgHeight > tMSLGloablFeatures.nMaxImgHeight)), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);

        _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageWidth  = tRescaleConfig->unRescaledImgWidth;
        _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageHeight = tRescaleConfig->unRescaledImgHeight;

        _hMSLIMGThmb->tOMXColorConvConfig.nSrcImageWidth    = _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageWidth;
        _hMSLIMGThmb->tOMXColorConvConfig.nSrcImageHeight   = _hMSLIMGThmb->tOMXRescaleConfig.nOutputImageHeight;

        //set all dependent component config spec too
        if(MSL_IMGTHMB_STATE_INIT== _hMSLIMGThmb->tImgThmbState)
        {
            /*---------------------------------------------------------------------------
                * do configuration for all affected omx components
                ---------------------------------------------------------------------------*/               
            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXRescale, 
            (OMX_INDEXTYPE) OMX_IndexConfigRescale,
            &_hMSLIMGThmb->tOMXRescaleConfig), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);

            MSL_EXIT_IF(OMX_ErrorNone != OMX_SetConfig(_hMSLIMGThmb->hOMXColorConv, 
            (OMX_INDEXTYPE)OMX_IndexConfigColorConversion,
            &_hMSLIMGThmb->tOMXColorConvConfig), MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);
        }

        break;

    case MSL_DECFILE_CONFIGINDEX:
        //  MSL_EXIT_IF(MSL_IMGTHMB_STATE_GENERATE == _hMSLIMGThmb->tImgThmbState, MSL_IMGTHMB_ERROR_INVALID_STATE);
        tMSLFileConfig = (MSL_FILE_CONFIGTYPE *) pParam;

        _hMSLIMGThmb->tDecodeFileType =  tMSLFileConfig->tFileType; 
        strcpy((MSL_STRING)_hMSLIMGThmb->sInputJPEGFileName, tMSLFileConfig->sFileName);

        /*set the decoder configurations*/
        _hMSLIMGThmb->tOMXDecodeConfig.nDecodeAUSize              = 0;
        _hMSLIMGThmb->tOMXDecodeConfig.tCropWindow.nHeight        = 0;
        _hMSLIMGThmb->tOMXDecodeConfig.tCropWindow.nWidth         = 0;
        _hMSLIMGThmb->tOMXDecodeConfig.tCropWindow.nXOffset       = 0;
        _hMSLIMGThmb->tOMXDecodeConfig.tCropWindow.nYOffset       = 0;
        _hMSLIMGThmb->tOMXDecodeConfig.nImageScalingFactor        = 1;
        break;

    case MSL_ENCFILE_CONFIGINDEX:
        //MSL_EXIT_IF(MSL_IMGTHMB_STATE_GENERATE == _hMSLIMGThmb->tImgThmbState, MSL_IMGTHMB_ERROR_INVALID_STATE);
        tMSLFileConfig = (MSL_FILE_CONFIGTYPE *) pParam;

        _hMSLIMGThmb->tEncodeFileType =  tMSLFileConfig->tFileType; 
        strcpy((MSL_STRING)_hMSLIMGThmb->sOutputJPEGFileName, tMSLFileConfig->sFileName);
        break;

    default:
        MSL_EXIT_IF(1, MSL_IMGTHMB_ERROR_INVALID_ARGUMENT);

    }
EXIT:
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_SetConfig : Exit");
    return  tRetVal; 
}


/* ========================================================================== */
/**
* @fn MSL_ImgThmb_Deinit : Deinitializes the pipeline.
*
* @param [in] hMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Deinit (MSL_HANDLE hMSLIMGThmb)
{
    MSL_IMGTHMB_PRIV_STRUCT  *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) hMSLIMGThmb;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Deinit : Entry");

    MSL_PROFILESTUB("MSL_ImgThmb DeInitialization: Started");

    _hMSLIMGThmb->tImgThmbState = MSL_IMGTHMB_STATE_DEINIT;

    OMX_SendCommand(_hMSLIMGThmb->hOMXDec,  
    OMX_CommandStateSet,
    OMX_StateLoaded,
    NULL
    );

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Deinit : Exit");
    return  MSL_IMGTHMB_STATUS_OK  ;
}

/* ========================================================================== */
/**
* @fn MSL_ImgThmb_Destroy: Destroys the pipeline.
*
* @param [in] hMSLIMGThmb
*     handle to MSLIMG object
*
* @return MSL_IMGTHMB_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK   Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Destroy (MSL_HANDLE hMSLIMGThmb)
{
    /* dereference the local pointer */
    MSL_IMGTHMB_PRIV_STRUCT  *_hMSLIMGThmb = (MSL_IMGTHMB_PRIV_STRUCT  *) hMSLIMGThmb;

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Destroy : Entry");

    /* free omx client handles */
    OMX_FreeHandle(_hMSLIMGThmb->hOMXDec);
    OMX_FreeHandle(_hMSLIMGThmb->hOMXEnc);
    OMX_FreeHandle(_hMSLIMGThmb->hOMXRescale);
    OMX_FreeHandle(_hMSLIMGThmb->hOMXColorConv);

    /*free the buffers and bufferheaders*/    
    if(_hMSLIMGThmb->pBuff0)
    MSL_FREE(_hMSLIMGThmb->pBuff0);
    if(_hMSLIMGThmb->pBuff1)
    MSL_FREE(_hMSLIMGThmb->pBuff1);

    if(_hMSLIMGThmb->pImgProcBuffHeader0)
    MSL_FREE(_hMSLIMGThmb->pImgProcBuffHeader0);

    if(_hMSLIMGThmb->pImgProcBuffHeader1)
    MSL_FREE(_hMSLIMGThmb->pImgProcBuffHeader1);

    if(hMSLIMGThmb)
    MSL_FREE(hMSLIMGThmb);

    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_Destroy : Exit");
    return  MSL_IMGTHMB_STATUS_OK  ;
}

/* ========================================================================== */
/**
* @fn MSL_ImgThmb_FillDecoderBuff: Reads data from jpeg file and fills decoder
* input buffer 
*
* @param [in] _hMSLIMGThmb
*     handle to MSLIMG object
*
* @return OMX_TICOLOR_FORMATTYPE
*  
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_IMGTHMB_STATUS MSL_ImgThmb_FillDecoderBuff(MSL_IMGTHMB_PRIV_STRUCT *_hMSLIMGThmb )
{
    MSL_IMGTHMB_STATUS               tRetVal = MSL_IMGTHMB_STATUS_OK;
#ifndef WIN32    
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_FillDecoderBuff : Entry");

    MSL_PROFILEENTRY(MSL_ImgThmb_FillDecoderBuff);

    if(_hMSLIMGThmb->tImgThmbState == MSL_IMGTHMB_STATE_GENERATE)
    {
        if(MSL_FILETYPE_FFS == _hMSLIMGThmb->tDecodeFileType)
        {
            T_FFS_FD hJPGFILE;
            T_FFS_STAT stat;

            /*read the file from NAND*/    
            if((hJPGFILE = ffs_open((MSL_STRING)_hMSLIMGThmb->sInputJPEGFileName, FFS_O_RDWR))<EFFS_OK)
            {
                MSL_EXIT_IF(1,  MSL_IMGTHMB_ERROR_IOREAD);  
            }
            // To fix defect OMAPS00103603
            MSL_EXIT_IF(ffs_stat((MSL_STRING)_hMSLIMGThmb->sInputJPEGFileName, (void *)&stat)<EFFS_OK, MSL_IMGTHMB_ERROR_IOWRITE);
            MSL_EXIT_IF(stat.size >_hMSLIMGThmb->pImgProcBuffHeader0->nAllocLen, MSL_IMGTHMB_ERROR_IOWRITE);

            

            /*read into i/p decoder buffer (size to be specified)*/
            // To fix defect OMAPS00103603 for Decoding file sizes greater than 150 kb
            if((ffs_read(hJPGFILE, _hMSLIMGThmb->pImgProcBuffHeader0->pBuffer,_hMSLIMGThmb->pImgProcBuffHeader0->nAllocLen)<0))
            {
                MSL_EXIT_IF(1,  MSL_IMGTHMB_ERROR_IOREAD);  
            }

            ffs_close (hJPGFILE);
        }
        else
        {    
#ifdef MSL_USE_RFS
            T_RFS_FD hJPGFILE;
            T_RFS_STAT  stat;

            T_WCHAR * sfilename_uc;
            //T_WCHAR sfilename_uc[MSL_MAXFILENAME_LENGTH];
            sfilename_uc= (T_WCHAR *) MSL_ALLOC(MSL_MAXFILENAME_LENGTH);
            convert_u8_to_unicode((MSL_STRING)_hMSLIMGThmb->sInputJPEGFileName, sfilename_uc);

            /*read the file from NAND*/    
            if((hJPGFILE = rfs_open(sfilename_uc, RFS_O_RDWR, RFS_IRWXO)) < RFS_EOK)
            {
                MSL_EXIT_IF(1,  MSL_IMGTHMB_ERROR_IOREAD);  
            }

            MSL_EXIT_IF(rfs_stat(sfilename_uc, (void *)&stat)<RFS_EOK, MSL_IMGTHMB_ERROR_IOREAD);
            MSL_EXIT_IF(stat.file_dir.size>_hMSLIMGThmb->pImgProcBuffHeader0->nAllocLen, MSL_IMGTHMB_ERROR_IOREAD);


            /*read into i/p decoder buffer (size to be specified)*/
            if((rfs_read(hJPGFILE, _hMSLIMGThmb->pImgProcBuffHeader0->pBuffer, _hMSLIMGThmb->pImgProcBuffHeader0->nAllocLen)<0))
            {
                MSL_EXIT_IF(1,  MSL_IMGTHMB_ERROR_IOREAD);  
            }
            rfs_close (hJPGFILE);

            
            if(sfilename_uc)
            MSL_FREE(sfilename_uc);
#endif    

        }
    }

EXIT:
    MSL_PROFILEEXIT(MSL_ImgThmb_FillDecoderBuff);
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_FillDecoderBuff : Exit");
#endif    
    return  tRetVal; 
}

/* ========================================================================== */
/**
* @fn MSL_ImgThmb_SaveEncodedBuffer:Saves the Encoded buffers according to the filetype specified
*
* @param [in] hIMGThmb
*     handle to MSLIMG object
*
* @return MSL_STATUS
*     If the command successfully executes, the return code will be
*     MSL_IMGTHMB_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
*
*  @see  msl_api.h, msl_imgthmb.h
*/
/* ========================================================================== */
MSL_STATUS MSL_ImgThmb_SaveEncodedBuffer(MSL_IMGTHMB_PRIV_STRUCT * _hMSLIMGThmb, OMX_BUFFERHEADERTYPE *pBuffHeader)
{
    MSL_STATUS              tRetVal = MSL_IMGTHMB_STATUS_OK;
    MSL_U32                 nFreeBytes;
#ifndef WIN32
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_SaveEncodedBuffer : Entry");

    MSL_PROFILEENTRY(MSL_ImgThmb_SaveEncodedBuffer);

    if(MSL_FILETYPE_FFS == _hMSLIMGThmb->tEncodeFileType)
    {
        T_FFS_FD hJPGFile;
        ffs_query(Q_BYTES_FREE, &nFreeBytes) ;
        MSL_EXIT_IF(nFreeBytes < pBuffHeader->nFilledLen, MSL_IMGTHMB_ERROR_IOWRITE);

        /* open the file */    
        MSL_EXIT_IF(((hJPGFile = ffs_open((MSL_STRING)_hMSLIMGThmb->sOutputJPEGFileName, FFS_O_RDWR | FFS_O_CREATE))<EFFS_OK)
        , MSL_IMGTHMB_ERROR_IOWRITE);

        /* write output buffer */
        if(ffs_write(hJPGFile, pBuffHeader->pBuffer, pBuffHeader->nFilledLen) < 0) 
        {
            MSL_EXIT_IF(ffs_close(hJPGFile)<EFFS_OK, MSL_IMGTHMB_ERROR_IOWRITE);
            ffs_remove((MSL_STRING)_hMSLIMGThmb->sOutputJPEGFileName);
            MSL_EXIT_IF(1,MSL_IMGTHMB_ERROR_IOWRITE);
        }

        MSL_EXIT_IF (ffs_close (hJPGFile) < EFFS_OK, MSL_IMGTHMB_ERROR_IOWRITE);
    }
    else
    {
#ifdef MSL_USE_RFS
        T_RFS_FD hJPGFile;
        
        T_WCHAR * sfilename_uc;
        //T_WCHAR sfilename_uc[MSL_MAXFILENAME_LENGTH];
        sfilename_uc= (T_WCHAR *) MSL_ALLOC(MSL_MAXFILENAME_LENGTH);
        convert_u8_to_unicode((MSL_STRING)_hMSLIMGThmb->sOutputJPEGFileName, sfilename_uc);
        /* open the file */    
        MSL_EXIT_IF(((hJPGFile = rfs_open( sfilename_uc, RFS_O_RDWR | RFS_O_CREAT, RFS_IRWXO)) < RFS_EOK)
        , MSL_IMGTHMB_ERROR_IOWRITE);

        /* write output from the burst buffer */
        if(rfs_write(hJPGFile, pBuffHeader->pBuffer, pBuffHeader->nFilledLen) < 0) 
        {
            MSL_EXIT_IF(rfs_close(hJPGFile)<RFS_OK, MSL_IMGTHMB_ERROR_IOWRITE);
            rfs_remove(sfilename_uc);
            MSL_EXIT_IF(1,MSL_IMGTHMB_ERROR_IOWRITE);
        }


        MSL_EXIT_IF(rfs_close (hJPGFile) < RFS_EOK, MSL_IMGTHMB_ERROR_IOWRITE);

        
        if(sfilename_uc)
        MSL_FREE(sfilename_uc);
#endif    
    }

EXIT:
    MSL_PROFILEEXIT(MSL_ImgThmb_SaveEncodedBuffer);
    MSL_ENTRYEXITFUNC_TRACE("MSL_ImgThmb_SaveEncodedBuffer : Exit");

#endif    
    return  tRetVal; 
}

