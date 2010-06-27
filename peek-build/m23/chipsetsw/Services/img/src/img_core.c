/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file img_core.c
*
* This file implements the internal functions for img entity
*
* @path  chipsetsw\services\img\src
*
* @rev  0.2
*
*/
/* ------------------------------------------------------------------------- */
/* =========================================================================
*!
*! Revision History
*! ===================================
*! 31-Jan-2006 Ramesh Anandhi: Initial Release
*! 17-Apr-2006 Ramesh Anandhi; Major Review comments incorporated
*! 12-May-2006 Ramesh Anandhi; Renamed internal functions with more
*!                             meaningful names.
*!
* ========================================================================= */
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "tools.h"
#include "OMX_IMG_Private.h"
#include "img.h"
#include "exp_JPEG_Enc.h"
#include "common.h"
#include "baseImageEnc.h"
#include "exp_colorconv_rotate_scaling.h"
#include "jpeg_dec_api.h"
#ifdef IMG_TIMER
#include "timer1.h"
#endif

#define IMG_NO_ZOOM_VALUE  (1024)
#if CAM_SENSOR == 1
#define IMG_WIDTH_MAX       (1280)
#define IMG_HEIGHT_MAX      (1024)
#else
#define IMG_WIDTH_MAX       (640)
#define IMG_HEIGHT_MAX      (480)
#endif

/* ========================================================================== */
/** @fn img_core_state_change method is used to change the state
* of the IMG core. This is called when the OMX IMG client sends a request
* for state change or when the core contains all resources needed
* to start executing.
*
* @param [in] pOp
*     The operation handle for the current operation.
*
* @param [in] pState
*     The requested state. This is an OMX_STATETYPE value.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  img.h
*/
/* ========================================================================== */
OMX_ERRORTYPE img_core_state_change (IMG_CORE_OPERATIONTYPE *pOp, OMX_STATETYPE pState)
{
    OMX_ERRORTYPE res;
    OMX_U32 run_res;

    OMX_ENTRYEXITFUNC_TRACE("Enter img_core_state_change");

    res = OMX_ErrorNone;

    switch(pState)
    {
    case OMX_StateIdle:
        if((pOp->tCompType == OMX_ENCODE_COMPTYPE) && (pOp->tParam.tEncode.tEncode.tStreamingMode != OMX_TIIMAGE_STREAMINGNONE))
        if(pOp->tParam.tEncode.hEncoder!= NULL)
        {
            pOp->tParam.tEncode.hEncoder->vDelete(pOp->tParam.tEncode.hEncoder);
        }
        if(pOp->tCompType == OMX_DECODE_COMPTYPE &&  pOp->tParam.tDecode.hDecoder != NULL )
        JDEmz_gDeleteJpegDecoder(pOp->tParam.tDecode.hDecoder);
        __release_buffers(pOp);
        pOp->tCoreSts = IMG_CORE_STS_IDLE;
        pOp->hComp              = NULL;
        break;
    case OMX_StateExecuting:
        if(pOp->tCoreSts == IMG_CORE_STS_WAITING || pOp->tCoreSts == IMG_CORE_STS_IDLE)
        {
            if ((IS_BUF_QUEUE_EMPTY(pOp->pInpBufQueue))||
                    (IS_BUF_QUEUE_EMPTY(pOp->pOutBufQueue)))
            pOp->tCoreSts = IMG_CORE_STS_WAITING;
            else
            pOp->tCoreSts = IMG_CORE_STS_RUNNING;

            //if only single buffer then check for only input buffer
            if(pOp->bInPlace && !IS_BUF_QUEUE_EMPTY(pOp->pInpBufQueue))
            {
                pOp->tCoreSts = IMG_CORE_STS_RUNNING;
            }

        }
        break;
    case OMX_StatePause:
        pOp->tCoreSts = IMG_CORE_STS_IDLE;
        break;
    }
    OMX_ENTRYEXITFUNC_TRACE("Exit img_core_state_change");
    return res;
}
/* ========================================================================== */
/** @fn img_queue_buf method is used to queue the buffers
* to the input or output buffer queues of the IMG core.
*
* @param [in] pOp
*     The operation handle for the current operation.
*
* @param [in] buf
*     The buffer to be queued.
*
* @param [in] bufType
*     The buffer type- distinguishes an input buffer from an output buffer.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  img.h
*/
/* ========================================================================== */
OMX_ERRORTYPE img_queue_buf (IMG_CORE_OPERATIONTYPE *pOp, OMX_PTR buf, IMG_BUF_TYPE bufType)
{
    OMX_ERRORTYPE res = OMX_ErrorNone;
    OMX_U32 tRes;
    OMX_BUFFERHEADERTYPE * pBuf = (OMX_BUFFERHEADERTYPE*)buf;
    OMX_BOOL inPlace;

    OMX_ENTRYEXITFUNC_TRACE("Enter img_queue_buf");
    if(bufType == IMG_INPUT)
    {
        if(pOp->tCompType == OMX_DECODE_COMPTYPE)
        {

            OMX_TIIMAGE_DECODE_IMAGEINFOTYPE *tImgInfo;
            tImageInfo_JDEmz imgCodecInfo;
            tRect_JDEmz tCropWindow ;
            tImgInfo = &(pOp->tParam.tDecode.tDecodeInfo);

            tRes = JDEmz_gParseJpegHeader(pBuf->pBuffer, pBuf->nAllocLen, &imgCodecInfo);
            if(tRes != E_SUCCESS)
            return OMX_ErrorUndefined;
            tImgInfo->nActHeight        = imgCodecInfo.actualHeight;
            tImgInfo->nActWidth         = imgCodecInfo.actualWidth;
            tImgInfo->nHeight           = imgCodecInfo.height;
            tImgInfo->nWidth            = imgCodecInfo.width;
            tImgInfo->tImageFormat      = __map_to_TII_Format(imgCodecInfo.frameType);
            tImgInfo->nPlanarFormat   = imgCodecInfo.planarFormat;
            tImgInfo->nProgressiveFormat  = imgCodecInfo.progressiveFormat;
            
            tCropWindow.wndHeight  = tImgInfo->nHeight;
            tCropWindow.wndWidth  = tImgInfo->nWidth;
            tCropWindow.xOffset   = pOp->tParam.tDecode.tDecode.tCropWindow.nXOffset;
            tCropWindow.yOffset   = pOp->tParam.tDecode.tDecode.tCropWindow.nYOffset;
            
            tRes= JDEmz_gCreateJpegDecoder((int32**)&(pOp->tParam.tDecode.hDecoder),
            (uint8 *)pBuf->pBuffer,
            pBuf->nAllocLen,
            &tCropWindow,
            &imgCodecInfo,
            pOp->tParam.tDecode.tDecode.nImageScalingFactor);
            if(tRes!= E_SUCCESS)
            return OMX_ErrorUndefined;
            __OMX_IMG_CoreCallback(pOp->hComp,OMX_ErrorNone, OMX_IMG_RETURN_DECODE_INFO, tImgInfo);
        }
        else if(pOp->tCompType == OMX_ENCODE_COMPTYPE)
        {
            pOp->tParam.tEncode.tEncode.tYUVFrameBuf.lum = pBuf->pBuffer;
            pOp->tParam.tEncode.tEncode.tYUVFrameBuf.cb = NULL;
            pOp->tParam.tEncode.tEncode.tYUVFrameBuf.cr = NULL;
            if(pOp->tParam.tEncode.tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT)
             {
                if(sizeof(pOp->tParam.tEncode.hEncoder) > sizeof(tBaseImageEncoder*))
                (pOp->tParam.tEncode.hEncoder)->vDelete(pOp->tParam.tEncode.hEncoder);
                res = __encoder_create(&pOp->tParam.tEncode.tEncode, &(pOp->tParam.tEncode.hEncoder));
            }   
        }

        /* enqueue input buffer */
        buf_enqueue(&pOp->pInpBufQueue, buf);
    }
    else
    {
        buf_enqueue(&pOp->pOutBufQueue, buf);
    }

    if (pOp->tCoreSts == IMG_CORE_STS_WAITING)
    {
        img_core_state_change(pOp, OMX_StateExecuting);
    }
    OMX_ENTRYEXITFUNC_TRACE("Exit img_queue_buf");
    return res;
}
/* ========================================================================== */
/** @fn img_core_setconfig method is used to set the
* configurations sent from the client at the core.
*
* @param [in] pOp
*     The operation handle for the current operation.
*
* @param [in] pParam
*     The configuration parameters to be set.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  img.h
*/
/* ========================================================================== */
OMX_ERRORTYPE img_core_setconfig(IMG_CORE_OPERATIONTYPE *pOp, OMX_PTR pParam)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_IMG_COMP_PARAMTYPE *pCompParam;
    OMX_S8 res;

    OMX_ENTRYEXITFUNC_TRACE("Enter img_core_setconfig");
    pCompParam = (OMX_IMG_COMP_PARAMTYPE*)pParam;
    pOp->tCompType = pCompParam->tCompType;

    switch(pCompParam->tCompType)
    {

    case OMX_ENCODE_COMPTYPE :
        {
            memcpy(&(pOp->tParam.tEncode.tEncode), &(pCompParam->tParamType.tEncodeParam),
            sizeof(OMX_TIIMAGE_ENCODE_PARAMTYPE));
            if(pOp->tParam.tEncode.tEncode.tStreamingMode != OMX_TIIMAGE_STREAMINGNONE)
            {
                if(sizeof(pOp->tParam.tEncode.hEncoder) > sizeof(tBaseImageEncoder*))
                (pOp->tParam.tEncode.hEncoder)->vDelete(pOp->tParam.tEncode.hEncoder);
                res = __encoder_create(&pOp->tParam.tEncode.tEncode, &(pOp->tParam.tEncode.hEncoder));
                if(res == E_SUCCESS)
                {
                    //pOp->tParam.tEncode.tStatus = 0x50;
                }
            }
            break;
        }
    case OMX_DECODE_COMPTYPE :
        memcpy(&(pOp->tParam.tDecode.tDecode), &(pCompParam->tParamType.tDecodeParam),
        sizeof(OMX_TIIMAGE_DECODE_PARAMTYPE));
        break;
    case OMX_ROTATE_COMPTYPE :
        memcpy(&(pOp->tParam.tRotate), &(pCompParam->tParamType.tRotateParam),
        sizeof(OMX_TIIMAGE_ROTATE_PARAMTYPE));
        break;
    case OMX_RESCALE_COMPTYPE :
        memcpy(&(pOp->tParam.tRescale), &(pCompParam->tParamType.tRescaleParam),
        sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE));
        break;
    case OMX_OVERLAY_COMPTYPE :
        memcpy(&(pOp->tParam.tOverlay), &(pCompParam->tParamType.tOverlayParam),
        sizeof(OMX_TIIMAGE_OVERLAY_PARAMTYPE));
        break;
    case OMX_EFFECT_COMPTYPE :
        memcpy(&(pOp->tParam.tEffect), &(pCompParam->tParamType.tEffectParam),
        sizeof(OMX_TIIMAGE_EFFECT_PARAMTYPE));
        break;
    case OMX_COLORCONVERSION_COMPTYPE :
        memcpy(&(pOp->tParam.tColConv), &(pCompParam->tParamType.tColorConvParam),
        sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));
        break;
        default : break;
    }

    //set inplace computation buffer flag
    switch(pOp->tCompType )
    {
    case OMX_ROTATE_COMPTYPE :
        pOp->bInPlace = pOp->tParam.tRotate.bInPlace;
        break;
    case OMX_RESCALE_COMPTYPE :
        pOp->bInPlace = pOp->tParam.tRescale.bInPlace;
        break;
    case OMX_OVERLAY_COMPTYPE :
        pOp->bInPlace = pOp->tParam.tOverlay.bInPlace;
        break;
    case OMX_EFFECT_COMPTYPE :
        pOp->bInPlace = pOp->tParam.tEffect.bInPlace ;
        break;
    case OMX_COLORCONVERSION_COMPTYPE :
        pOp->bInPlace = pOp->tParam.tColConv.bInPlace ;
        break;
    default:
        pOp->bInPlace = OMX_FALSE;
        break;
    }
    OMX_ENTRYEXITFUNC_TRACE("Exit img_core_setconfig");
    return retVal;

}
/* ========================================================================== */
/** @fn img_core_getconfig method is used to get the
* configuration of the core component requested by the
* OMX IMG client.
*
* @param [in] pOp
*     The operation handle for the current operation.
*
* @param [in] pParam
*     The configuration parameters to be returned.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*       This function is not implemented and hence an ErrorNone will be
*       returned always.
*
*  @see  img.h
*/
/* ========================================================================== */

OMX_ERRORTYPE img_core_getconfig(IMG_CORE_OPERATIONTYPE *pOp, OMX_PTR pParam)
{
    OMX_ERRORTYPE retVal=OMX_ErrorNone;
    return retVal;
}
/* ========================================================================== */
/** @fn img_core_running method is called when the core
* has all the resources required to commence execution.
* This function is called from the img_pei_primitive function.
*
* @param [in] pOp
*     The operation handle for the current operation.
*
* @return OMX_S32
*     If the command successfully executes, the return code will be
*     E_SUCCESS.  Otherwise the appropriate error will be returned.
*
*  @see  img.h
*/
/* ========================================================================== */

OMX_S32 img_core_running(IMG_CORE_OPERATIONTYPE *pOp)
{
    OMX_S32 res;
    OMX_ERRORTYPE tErrReturn =OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE* rawInpBuf, *rawOutBuf;
    tBaseVideoFrame_JDEmz inpImgBuf, outImgBuf;

    OMX_U16 timer_read1, timer_read2, res1;
    OMX_ENTRYEXITFUNC_TRACE("Enter img_core_running");
    __timer_init();

    if((IS_BUF_QUEUE_EMPTY(pOp->pInpBufQueue)) ||
            (!pOp->bInPlace && (IS_BUF_QUEUE_EMPTY(pOp->pOutBufQueue))))
    {
        return OMX_ErrorNone;
    }
    rawInpBuf = (OMX_BUFFERHEADERTYPE*)buf_dequeue(&pOp->pInpBufQueue);
    //get output buffer only if not in place
    if(!pOp->bInPlace)
    {
        rawOutBuf = (OMX_BUFFERHEADERTYPE*)buf_dequeue(&pOp->pOutBufQueue);
    }
    OMX_PROFILEENTRY((unsigned long)img_core_running);
    switch (pOp->tCompType)
    {
    case OMX_ENCODE_COMPTYPE :
        {
            tBaseImageEncoder *hEncoder = NULL;
            OMX_U32 nBufSize, nInBytes;
            tBaseYUVFrame tInYUVFrame;
            IMG_CORE_ENCODEPARAM *tParam;
            OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE tImgInfo;

            tParam = &(pOp->tParam.tEncode);
            /** create an instance */
            if(tParam->tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGNONE)
            {
                res = __encoder_create(&(tParam->tEncode),
                &(tParam->hEncoder));
                if(res!=E_SUCCESS)
                break;
            }
            nBufSize = rawOutBuf->nAllocLen;
            nInBytes = rawInpBuf->nAllocLen;
            hEncoder = tParam->hEncoder;

            __convert_to_imgprocbuftype(rawInpBuf,
            &inpImgBuf,
            tParam->tEncode.nImageHeight,
            tParam->tEncode.nImageWidth,
            tParam->tEncode.tInputImageFormat);

            tInYUVFrame.luma = inpImgBuf.lum;
            tInYUVFrame.cb    = inpImgBuf.cb;
            tInYUVFrame.cr    = inpImgBuf.cr;

            //call jpeg encoder
            nInBytes = tParam->tEncode.nImageHeight * tParam->tEncode.nImageWidth * 2;
            timer_read1 = __timer_start();
            OMX_DETAILFUNC_TRACE("Start Encoding ");
            OMX_PROFILESTUB("Start Encoding ");
            res = hEncoder->vEncodeFrame(hEncoder,
            &tInYUVFrame,
            (uint8*)(rawOutBuf->pBuffer),
            (uint32*)(&nBufSize),
            nInBytes);
            OMX_PROFILESTUB("Encoding Complete");
            timer_read2 = __timer_stop();
            OMX_DETAILFUNC_TRACE("Encoding Complete");

            if(tParam->tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGNONE)
            {
                if (res == E_SUCCESS)
                rawOutBuf->nFilledLen = nBufSize;
                else
                rawOutBuf->nFilledLen = 0;
                if(hEncoder != NULL)
                //delete encoder instance
                hEncoder->vDelete(hEncoder);
                hEncoder =NULL;
            }
            else if(tParam->tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT)
            {
                if(res == E_NOT_COMPLETE || res == E_SUCCESS)
                rawOutBuf->nFilledLen = nBufSize;
                else
                rawOutBuf->nFilledLen = 0;
            }
            else if(tParam->tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGINPUT)
            {
                if(res == E_COMPLETE || res == E_SUCCESS)
                rawOutBuf->nFilledLen = nBufSize;
                else
                rawOutBuf->nFilledLen = 0;
            }
            break;
        }
    case OMX_DECODE_COMPTYPE :
        {
            IMG_CORE_DECODEPARAM *tParam;
            tRect_JDEmz tCropWindow ;
            tImageInfo_JDEmz imageInfo;
            int32 *handleJpegDec = NULL;

            tParam= &(pOp->tParam.tDecode);
            handleJpegDec = tParam->hDecoder;

            if(tParam->tDecodeInfo.tImageFormat == OMX_TICOLOR_YUV422H ||
                    tParam->tDecodeInfo.tImageFormat == OMX_TICOLOR_YUV422V)
            {
                tParam->tDecodeInfo.nWidth  = ((tParam->tDecodeInfo.nWidth + 15)/16) * 16;
                tParam->tDecodeInfo.nHeight = ((tParam->tDecodeInfo.nHeight + 7)/8) * 8;
            }
            else if (tParam->tDecodeInfo.tImageFormat == OMX_TICOLOR_YUV420Planar)
            {
                tParam->tDecodeInfo.nWidth  = ((tParam->tDecodeInfo.nWidth + 15)/16) * 16;
                tParam->tDecodeInfo.nHeight = ((tParam->tDecodeInfo.nHeight + 15)/16) * 16;
            }
            else if ((tParam->tDecodeInfo.tImageFormat == OMX_TICOLOR_YUV444Planar ||
                        tParam->tDecodeInfo.tImageFormat == OMX_TICOLOR_24bitRGB888Planar))
            {
                tParam->tDecodeInfo.nWidth  = ((tParam->tDecodeInfo.nWidth + 7)/8) * 8;
                tParam->tDecodeInfo.nHeight = ((tParam->tDecodeInfo.nHeight + 7)/8) * 8;
            }
            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tParam->tDecodeInfo.nHeight,
            tParam->tDecodeInfo.nWidth,
            tParam->tDecodeInfo.tImageFormat);

            if(tParam->tDecodeInfo.nWidth * tParam->tDecodeInfo.nHeight <= IMG_WIDTH_MAX * IMG_HEIGHT_MAX )
            {
                OMX_DETAILFUNC_TRACE("Start Decoding " );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start Decoding " );
                if (tParam->tDecode.nDecodeAUSize == 0)
                {    
                    res = JDEmz_gDecodeJpegImage(handleJpegDec, tParam->tDecode.nDecodeAUSize, outImgBuf);
                }
                else
                {    
                    res = JDEmz_gDecodeJpegImage(handleJpegDec, tParam->tDecode.nDecodeAUSize, outImgBuf);//to get all three components three calls
                    res = JDEmz_gDecodeJpegImage(handleJpegDec, tParam->tDecode.nDecodeAUSize, outImgBuf);
                    res = JDEmz_gDecodeJpegImage(handleJpegDec, tParam->tDecode.nDecodeAUSize, outImgBuf);
                }
                OMX_PROFILESTUB("Decoding Complete" );
                timer_read2 = __timer_stop();
                OMX_DETAILFUNC_TRACE("Decoding Complete" );
            }
            if(!(tParam->tDecode.nDecodeAUSize != 0 && res == E_PARTIAL_DECODED))
            {
                res = JDEmz_gDeleteJpegDecoder(handleJpegDec);
                tParam->hDecoder = NULL;
            }
            /** todo free decoder instance in the structure ? */
            break;
        }
    case OMX_ROTATE_COMPTYPE :
        {
            OMX_TIIMAGE_ROTATE_PARAMTYPE *tRotate;
            OMX_U8 nSrcClrFmt ;
            OMX_U16 rotateFlag;

            tRotate= &(pOp->tParam.tRotate);

            nSrcClrFmt = __map_to_Codecformat(tRotate->tInputImageFormat);

            __convert_to_imgprocbuftype(rawInpBuf,
            &inpImgBuf,
            tRotate->nImageHeight,
            tRotate->nImageWidth,
            tRotate->tInputImageFormat);


            switch(tRotate->tRotate)
            {
            case OMX_TIIMAGE_ROTATE0 :
                rotateFlag = 0;
                break;
            case OMX_TIIMAGE_ROTATE90 :
                rotateFlag = 1;
                break;
            case OMX_TIIMAGE_ROTATE180 :
                rotateFlag = 2;
                break;
            case OMX_TIIMAGE_ROTATE270 :
                rotateFlag = 3;
                break;
            default:
                rotateFlag = 0;
                break;
            }
            if(rotateFlag == 0 || rotateFlag == 2)
            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tRotate->nImageHeight,
            tRotate->nStride,
            tRotate->tInputImageFormat);
            else
            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tRotate->nImageWidth,
            tRotate->nStride,
            tRotate->tInputImageFormat);

            #if 0
            if(rotateFlag == 0) {
                OMX_PTR tempBuffer;
                tempBuffer = rawInpBuf->pBuffer;
                rawInpBuf->pBuffer= rawOutBuf->pBuffer;
                rawOutBuf->pBuffer = tempBuffer;
                IMG_TRACE("IMG rotation bypassed");

            }
            else
            #endif
            {
                if(pOp->bInPlace) {
                    OMX_DETAILFUNC_TRACE("Start Rotation Through single buffer" );
                    timer_read1 = __timer_start();
                    OMX_PROFILESTUB("Start Rotation Through single buffer" );
                    res = IPEmz_gRotateFrame_singleBuff(&inpImgBuf, rotateFlag, &(nSrcClrFmt));
                    OMX_PROFILESTUB("Rotation Through single buffer Complete" );
                    timer_read2= __timer_stop();
                    OMX_DETAILFUNC_TRACE("Rotation Through single buffer Complete" );
                }
                else {
                    OMX_DETAILFUNC_TRACE("Start Rotation" );
                    OMX_PROFILESTUB("Start Rotation" );
                    timer_read1 = __timer_start();
                    res = IPEmz_gRotateFrame(&inpImgBuf, &outImgBuf, rotateFlag, &(nSrcClrFmt));
                    timer_read2 = __timer_stop();
                    OMX_PROFILESTUB("Rotation Complete" );
                    OMX_DETAILFUNC_TRACE("Rotation Complete" );
                }
            }
            break;
        }
    case OMX_RESCALE_COMPTYPE :
        {
            OMX_TIIMAGE_RESCALE_PARAMTYPE *tRescale;
            OMX_U32 cropWidth, cropHeight, xOffset, yOffset;
            OMX_U8 nSrcClrFmt;

            tRescale= &(pOp->tParam.tRescale);

            if(tRescale->nZoomFactor < IMG_NO_ZOOM_VALUE)
            tRescale->nZoomFactor = IMG_NO_ZOOM_VALUE;

            //if zoom factor greater than default zoom, then overwrite crop parameters
            if(tRescale->nZoomFactor > IMG_NO_ZOOM_VALUE)
            {
                cropWidth  = (tRescale->nInputImageWidth * IMG_NO_ZOOM_VALUE) /tRescale->nZoomFactor ;
                cropHeight = (tRescale->nInputImageHeight * IMG_NO_ZOOM_VALUE) /tRescale->nZoomFactor ;
                xOffset    = (tRescale->nInputImageWidth  - cropWidth)/2;
                yOffset    = (tRescale->nInputImageHeight - cropHeight)/2;
            }
            else
            {
                cropWidth  = tRescale->tCropWindow.nWidth;
                cropHeight = tRescale->tCropWindow.nHeight;
                xOffset    = tRescale->tCropWindow.nXOffset;
                yOffset    = tRescale->tCropWindow.nYOffset;
            }

            //if width crop region greater than image width, reset to default
            if( ((xOffset + cropWidth) > tRescale->nInputImageWidth) || ((xOffset + cropWidth) == 0))
            {
                xOffset   = 0;
                cropWidth = tRescale->nInputImageWidth;
            }

            //if height crop region greater than image height, reset to default
            if( ((yOffset + cropHeight) > tRescale->nInputImageHeight) || ((yOffset + cropHeight) == 0))
            {
                yOffset    = 0;
                cropHeight = tRescale->nInputImageHeight;
            }

            if( cropWidth  == tRescale->nInputImageWidth   &&
                    cropHeight == tRescale->nInputImageHeight  &&
                    cropWidth  == tRescale->nOutputImageWidth &&
                    cropHeight == tRescale->nOutputImageHeight )
            {
                OMX_PTR tempBuffer;
                OMX_U32 tempLength;
                tempBuffer = rawInpBuf->pBuffer;
                tempLength = rawInpBuf->nAllocLen;
                rawInpBuf->pBuffer= rawOutBuf->pBuffer;
                rawInpBuf->nAllocLen = rawOutBuf->nAllocLen;
                rawOutBuf->pBuffer = tempBuffer;
                rawOutBuf->nAllocLen = tempLength;
                res = E_SUCCESS;
                OMX_PROFILESTUB("IMG rescale bypassed");
                break;
            }
            nSrcClrFmt = __map_to_Codecformat(tRescale->tInputImageFormat);

            __convert_to_imgprocbuftype(rawInpBuf,
            &inpImgBuf,
            tRescale->nInputImageHeight,
            tRescale->nInputImageWidth,
            tRescale->tInputImageFormat);

            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tRescale->nOutputImageHeight,
            tRescale->nOutputImageWidth,
            tRescale->tInputImageFormat);

            //call the actual codec
            OMX_DETAILFUNC_TRACE("Start Scaling" );
            OMX_PROFILESTUB("Start Scaling" );

            if(inpImgBuf.lum == NULL ||
                    outImgBuf.lum == NULL ||
                    inpImgBuf.height == 0 ||
                    inpImgBuf.width == 0 ||
                    outImgBuf.height == 0 ||
                    outImgBuf.width == 0 ||
                    cropHeight == 0 ||
                    cropHeight == 0)
            {
                res = OMX_ErrorBadParameter;
                break;
            }
            timer_read1 = __timer_start();
            res = IPEmz_gScaleFrame(&inpImgBuf, &outImgBuf, xOffset,
            yOffset, cropWidth, cropHeight, nSrcClrFmt);
            timer_read2 = __timer_stop();
            OMX_PROFILESTUB("Scaling Complete" );
            OMX_DETAILFUNC_TRACE("Scaling Complete" );
            break;
        }
    case OMX_OVERLAY_COMPTYPE :
        {
            OMX_TIIMAGE_OVERLAY_PARAMTYPE *tOvly;
            OMX_U16 nSrcClrFmt;
            OMX_BUFFERHEADERTYPE rawOvlyBuf;
            tBaseVideoFrame_JDEmz  ovlyImgBuf;

            tOvly= &(pOp->tParam.tOverlay);
            
            if(tOvly->pOverlayBuffer == NULL)
            {
                res = E_SUCCESS;
                OMX_PROFILESTUB("Overlay Bypassed");
                break;
            }
            rawOvlyBuf.pBuffer = (void *)tOvly->pOverlayBuffer;
            nSrcClrFmt = __map_to_Codecformat(tOvly->tSrcImageFormat);

            __convert_to_imgprocbuftype(rawInpBuf,
            &inpImgBuf,
            tOvly->nSrcImageHeight,
            tOvly->nSrcImageWidth,
            tOvly->tSrcImageFormat);

            if(tOvly->bInPlace == OMX_FALSE)
            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tOvly->nSrcImageHeight,
            tOvly->nSrcImageWidth,
            tOvly->tSrcImageFormat);
			else
				memcpy(&outImgBuf, &inpImgBuf, sizeof(inpImgBuf));

            __convert_to_imgprocbuftype(&rawOvlyBuf,
            &ovlyImgBuf,
            tOvly->nOverlayImageHeight,
            tOvly->nOverlayImageWidth,
            tOvly->tSrcImageFormat);


            if(tOvly->tOverlay == OMX_TIIMAGE_OVERLAY|| tOvly->tOverlay== OMX_TIIMAGE_ALPHAOVERLAY) {
                OMX_DETAILFUNC_TRACE("Start Overlay" );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start Overlay" );
                res = IPEmz_gOverLay((OMX_U8 *)rawInpBuf->pBuffer,
                (OMX_U8 *)rawOvlyBuf.pBuffer,
                (OMX_U8 *)rawOutBuf->pBuffer,
                tOvly->nSrcImageWidth,
                tOvly->nSrcImageHeight,
                tOvly->nOverlayImageWidth,
                tOvly->nOverlayImageHeight,
                tOvly->nSrcXOffset,
                tOvly->nSrcYOffset,
                tOvly->nOverlay,
                !( tOvly->bInPlace));
                OMX_PROFILESTUB("Overlay Complete" );
                timer_read2 = __timer_stop();
                OMX_DETAILFUNC_TRACE("Overlay Complete" );
            }

            if(tOvly->tOverlay == OMX_TIIMAGE_ALPHABLEND || tOvly->tOverlay== OMX_TIIMAGE_ALPHAOVERLAY)
            {
                OMX_U32 nAlpha;
                if(tOvly->nAlpha >= 0 && tOvly->nAlpha <= 100) 
                    nAlpha = tOvly->nAlpha;
                else
                {
                switch (tOvly->tTransmissivity)
                {



                case OMX_TIIMAGE_ALPHA25 :
                    nAlpha = 25;
                    break;
                case OMX_TIIMAGE_ALPHA50:
                    nAlpha = 50;
                    break;
                case OMX_TIIMAGE_ALPHA75 :
                    nAlpha = 75;
                    break;
                case OMX_TIIMAGE_ALPHA100:
                    nAlpha = 100;
                    break;
                    default :
                        nAlpha = 0;
                        break;
                }


                }

                OMX_DETAILFUNC_TRACE("Start AlphaBlend" );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start AlphaBlend" );
                res = IPEmz_gAlphaBlending(&inpImgBuf,
                &ovlyImgBuf,
                &outImgBuf,
                tOvly->nSrcXOffset,
                tOvly->nSrcYOffset,
                nAlpha);
                OMX_PROFILESTUB("Alpha Blend Complete" );
                timer_read2 = __timer_stop();
                OMX_DETAILFUNC_TRACE("Alpha Blend Complete" );

            }
            break;
        }//end OMX_OVERLAY_COMPTYPE

    case OMX_EFFECT_COMPTYPE :
        {

            OMX_TIIMAGE_EFFECT_PARAMTYPE*tEffect;
            OMX_U16 nSrcClrFmt;

            tEffect= &(pOp->tParam.tEffect);
            if(tEffect->tEffect == OMX_TIIMAGE_EFFECTUNUSED)
            {
                res = E_SUCCESS;
                OMX_PROFILESTUB("Effect bypassed");
                break;
            }
            nSrcClrFmt = __map_to_Codecformat(tEffect->tInputImageFormat);

            __convert_to_imgprocbuftype(rawInpBuf,
            &inpImgBuf,
            tEffect->nImageHeight,
            tEffect->nImageWidth,
            tEffect->tInputImageFormat);

            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tEffect->nImageHeight,
            tEffect->nImageWidth,
            tEffect->tInputImageFormat);

            switch (tEffect->tEffect) {
            case OMX_TIIMAGE_GRAYEFFECT:
                OMX_DETAILFUNC_TRACE("Start Grayscale" );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start Grayscale" );
                res = IPEmz_gGrayscaleMask(&inpImgBuf,& outImgBuf, nSrcClrFmt, !(tEffect->bInPlace));
                OMX_PROFILESTUB("Start Grayscale" );
                timer_read2 = __timer_stop();
                OMX_ENTRYEXITFUNC_TRACE("Gray Scale Complete" );
                break;
            case OMX_TIIMAGE_SEPIAEFFECT :
                OMX_ENTRYEXITFUNC_TRACE("Start Sepia" );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start Sepia" );
                res = IPEmz_gAddSepiaMask(&inpImgBuf, &outImgBuf, nSrcClrFmt, !(tEffect->bInPlace));
                OMX_PROFILESTUB("Start Sepia" );
                timer_read2 = __timer_stop();
                OMX_DETAILFUNC_TRACE("SepiaToning Complete" );
                break;
            }
            break;
        }//end OMX_EFFECT_COMPTYPE

    case OMX_COLORCONVERSION_COMPTYPE :
        {
            OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE*tColConv;
            OMX_U16 nSrcClrFmt, nDestClrFmt, numBytes;

            tColConv= &(pOp->tParam.tColConv);
            if(tColConv->tInputImageFormat == tColConv->tOutputImageFormat)
            {
                if(tColConv->bInPlace == OMX_FALSE)
                {
                    OMX_PTR tempBuffer;
                    OMX_U32 tempLength;
                    tempBuffer = rawInpBuf->pBuffer;
                    tempLength = rawInpBuf->nAllocLen;
                    rawInpBuf->pBuffer= rawOutBuf->pBuffer;
                    rawInpBuf->nAllocLen = rawOutBuf->nAllocLen;
                    rawOutBuf->pBuffer = tempBuffer;
                    rawOutBuf->nAllocLen = tempLength;
                }
                OMX_PROFILESTUB("Col COnv Bypassed");
                res = E_SUCCESS;
                break;
            }
            if(tColConv->bInPlace == OMX_TRUE)
            rawOutBuf = rawInpBuf;
            nSrcClrFmt = __map_to_Codecformat(tColConv->tInputImageFormat);

            switch(tColConv->tOutputImageFormat )
            {
            case OMX_TICOLOR_24bitBGR888:
                nDestClrFmt = 0;
                break;

            case OMX_TICOLOR_16bitARGB1555:
            case OMX_TICOLOR_16bitARGB4444:
            case OMX_TICOLOR_16bitBGR565:
            case OMX_TICOLOR_16bitRGB565:
                nDestClrFmt = 1;
                break;

            case OMX_TICOLOR_YCbYCr:
                nDestClrFmt = 2;
                break;

            default:
                nDestClrFmt = 1;
                break;
            }

            __convert_to_imgprocbuftype(rawInpBuf,
            &inpImgBuf,
            tColConv->nSrcImageHeight,
            tColConv->nSrcImageWidth,
            tColConv->tInputImageFormat);

            __convert_to_imgprocbuftype(rawOutBuf,
            &outImgBuf,
            tColConv->nSrcImageHeight,
            tColConv->nSrcImageWidth,
            tColConv->tOutputImageFormat);
            
            if (nDestClrFmt == 1)
            numBytes = tColConv->nSrcImageWidth;
            else
            numBytes= (tColConv->nSrcImageWidth * 4)/3;

            //if destination format is rgb24, then call conversion to rgb API
            if(nDestClrFmt == 0)
            {
                OMX_DETAILFUNC_TRACE("Start ColorConversion to RGB24" );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start ColorConversion to RGB24" );
                res = IPEmz_gColorConversion(&inpImgBuf,
                (uint8*)rawOutBuf->pBuffer,
                tColConv->nSrcImageWidth,
                tColConv->nSrcImageHeight,
                nSrcClrFmt,
                numBytes,
                1);
                OMX_PROFILESTUB("ColorConversion to RGB24 Complete" );
                timer_read2 = __timer_stop();
                OMX_DETAILFUNC_TRACE("ColorConversion to RGB Complete" );
            }


            //if destination format is rgb16, then call conversion to rgb API
            if(nDestClrFmt == 1)
            {
                OMX_DETAILFUNC_TRACE("Start ColorConversion to RGB16" );
                timer_read1 = __timer_start();
                OMX_PROFILESTUB("Start ColorConversion to RGB16" );
                res = IPEmz_gColorConversion(&inpImgBuf,
                (uint8*)rawOutBuf->pBuffer,
                tColConv->nSrcImageWidth,
                tColConv->nSrcImageHeight,
                nSrcClrFmt,
                numBytes,
                0);
                OMX_PROFILESTUB("ColorConversion to RGB16 Complete" );
                timer_read2 = __timer_stop();
                OMX_DETAILFUNC_TRACE("ColorConversion to RGB16 Complete" );
            }

            //for rgb16 to yuyv format conversion
            if(nDestClrFmt == 2)
            {
                if( nSrcClrFmt == RGB16 )
                {
                    OMX_DETAILFUNC_TRACE("Start ColorConversion from RGB16 to YUYV" );
                    timer_read1 = __timer_start();
                    OMX_PROFILESTUB("ColorConversion from RGB16 to YUYV Complete" );
                    res = IPEmz_gColorConversionRGB((uint8*)rawInpBuf->pBuffer,
                    &outImgBuf,
                    tColConv->nSrcImageWidth,
                    tColConv->nSrcImageHeight,
                    RGB16,
                    YUYV);
                    OMX_PROFILESTUB("ColorConversion from RGB16 to YUYV Complete" );
                    timer_read2 = __timer_stop();
                    OMX_DETAILFUNC_TRACE("ColorConversion from RGB16 to YUYV Complete" );
                }
                //if destination format is yuyv (and input other than rgb16), then call conversion to yuyv API
                else
                {
                    OMX_DETAILFUNC_TRACE("Start ColorConversion to YUYV" );
                    OMX_PROFILESTUB("Start ColorConversion to YUYV" );
                    timer_read1 = __timer_start();
                    res = IPEmz_gTransCoding(&inpImgBuf,
                    (uint8*)rawOutBuf->pBuffer,
                    tColConv->nSrcImageWidth,
                    tColConv->nSrcImageHeight,
                    nSrcClrFmt);
                    OMX_PROFILESTUB("ColorConversion to YUYV Complete" );
                    timer_read2 = __timer_stop();
                    OMX_DETAILFUNC_TRACE("ColorConversion to YUYV Complete" );
                }
            }
            break;
        }//end OMX_COLORCONVERSION_COMPTYPE
    }

    OMX_PROFILEEXIT((unsigned long)img_core_running);
    pOp->tCoreSts = IMG_CORE_STS_WAITING;
    img_core_state_change(pOp, OMX_StateExecuting);
    if (res ==E_SUCCESS ) {
        tErrReturn = OMX_ErrorNone;
        pOp->pCoreCallback(pOp->hComp, tErrReturn, OMX_IMG_CMD_QUEUE_INPBUF, rawInpBuf);
        //only if inplace is false, make callback for output buffer
        if(!pOp->bInPlace)
        pOp->pCoreCallback(pOp->hComp, tErrReturn, OMX_IMG_CMD_QUEUE_OUTBUF, rawOutBuf);
    }
    else
    {
        if(pOp->tCompType == OMX_ENCODE_COMPTYPE)
        {
            if(pOp->tParam.tEncode.tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGINPUT &&
                    res == E_COMPLETE)
            {
                buf_addToFront(&pOp->pOutBufQueue, rawOutBuf);
                pOp->pCoreCallback(pOp->hComp, OMX_ErrorNone, OMX_IMG_CMD_QUEUE_INPBUF, rawInpBuf);
            }
            else if(pOp->tParam.tEncode.tEncode.tStreamingMode == OMX_TIIMAGE_STREAMINGOUTPUT&&
                    res == E_NOT_COMPLETE)
            {
                buf_addToFront(&pOp->pInpBufQueue, rawInpBuf);
                pOp->pCoreCallback(pOp->hComp, tErrReturn, OMX_IMG_CMD_QUEUE_OUTBUF, rawOutBuf);
            }
            else
            pOp->pCoreCallback(pOp->hComp, OMX_ErrorInsufficientResources, OMX_IMG_CMD_MAX, NULL);
        }
        else if(pOp->tCompType == OMX_DECODE_COMPTYPE)
        {
            if(pOp->tParam.tDecode.tDecode.nDecodeAUSize != 0 &&
                    res == E_PARTIAL_DECODED)
            {
                buf_addToFront(&pOp->pInpBufQueue, rawInpBuf);
                pOp->pCoreCallback(pOp->hComp, OMX_ErrorNone, OMX_IMG_CMD_QUEUE_OUTBUF, rawOutBuf);
            }
            else
            pOp->pCoreCallback(pOp->hComp, OMX_ErrorInsufficientResources, OMX_IMG_CMD_MAX, NULL);
        }
        else
        {
            tErrReturn = OMX_ErrorInsufficientResources;
            pOp->pCoreCallback(pOp->hComp, tErrReturn, OMX_IMG_CMD_MAX, NULL);
        }
    }

    OMX_ENTRYEXITFUNC_TRACE("Exit img_core_running");
    return res;
}
/* ========================================================================== */
/** @fn __map_to_Codecformat method is used to map
* the OMX_TICOLOR)FORMATTYPE colour format passed
* OMX IMG client to the colour formats understood by the codec.
*
* @param [in] tColor
*     The OMX colour format type sent by the client.
*
* @return OMX_U32
*     The colour format type understood by the codec.
*
*  @see  OMX_TIImage.h, common.h
*/
/* ========================================================================== */

OMX_U32 __map_to_Codecformat(OMX_TICOLOR_FORMATTYPE tColor)
{
    OMX_U32 ret;
    switch(tColor)
    {
    case OMX_TICOLOR_YUV420Planar :
        ret = YUV420;
        break;
    case OMX_TICOLOR_YUV422H :
        ret = YUV422H;
        break;
    case OMX_TICOLOR_YUV422V:
        ret = YUV422V;
        break;
    case OMX_TICOLOR_YUV444Interleaved:
    case OMX_TICOLOR_YUV444Planar:
        ret = YUV444;
        break;
    case OMX_TICOLOR_12bitRGB444:
        ret = RGB444;
        break;
    case OMX_TICOLOR_Monochrome :
        ret = MONOCHROME;
        break;
    case OMX_TICOLOR_YCbYCr:
        ret = YUYV;
        break;
    case OMX_TICOLOR_16bitRGB565:
    case OMX_TICOLOR_16bitBGR565:
        ret = RGB16;
        break;
    default:
        ret = RGB16;
        break;
    }
    return ret;
}
/* ========================================================================== */
/** @fn __map_to_TII_Format method is used to map the
* colour format returned by the codec to a OMX_TICOLOR_FORMATTYPE
* value.
*
* @param [in] nEmzFormat
*     The colour format type sent by the codec.
*
* @return OMX_U32
*     The corresponding OMX_TICOLOR_FORMATTYPE value.
*
*  @see  OMX_TIImage.h, common.h*  @see  img.h
*/
/* ========================================================================== */

OMX_TICOLOR_FORMATTYPE __map_to_TII_Format(OMX_U8 nEmzFormat)
{
    switch(nEmzFormat)
    {
    case YUV420://           0x01
        return OMX_TICOLOR_YUV420Planar;
    case  YUV422H:
        return OMX_TICOLOR_YUV422H;
    case  YUV422V:
        return OMX_TICOLOR_YUV422V;
    case  YUV444:
        return OMX_TICOLOR_YUV444Planar;
    case RGB444:
        return OMX_TICOLOR_24bitRGB888Planar;
    case  MONOCHROME:
        return OMX_TICOLOR_Monochrome;
    case  YUYV:
        return OMX_TICOLOR_YCbYCr;
    case RGB16:
        return OMX_TICOLOR_16bitRGB565;
    default:
        return OMX_TICOLOR_16bitRGB565;
    }
}
/* ========================================================================== */
/** @fn __encoder_create method is called when the
* core needs a new instance of the encoder to be created.
*
* @param [in] pParam
*     The paramters required for creating the encoder.
*
* @param [in] hEncoder
*     The handle to the encoder.
*
* @return OMX_S32
*     If the command successfully executes, the return code will be
*     E_SUCCESS.  Otherwise the appropriate error will be returned.
*
*  @see  img.h
*/
/* ========================================================================== */

OMX_S32 __encoder_create(OMX_TIIMAGE_ENCODE_PARAMTYPE *pParam, tBaseImageEncoder **hEncoder)
{
    OMX_S32 ret;
    tJpegImageEncParam tEncParam;
    OMX_TIIMAGE_ENCODE_PARAMTYPE* pCoreParam;
    pCoreParam = (OMX_TIIMAGE_ENCODE_PARAMTYPE *)pParam;
    tEncParam.InputColorFormat  = __map_to_Codecformat(pCoreParam->tInputImageFormat);
    tEncParam.maxXDimension     = pCoreParam->nImageWidth;
    tEncParam.maxYDimension     = pCoreParam->nImageHeight;
    tEncParam.qualityFactor     = pCoreParam->nQualityFactor;
    tEncParam.StreamingMode     = pCoreParam->tStreamingMode;
    tEncParam.YuvFrame        = (tBaseYUVFrame*)&(pCoreParam->tYUVFrameBuf);
    ret = gCreateJpegImageEncoder(hEncoder, &tEncParam);
    return ret;
}
/* ========================================================================== */
/** @fn __convert_to_imgprocbuftype method is used to map the
* buffer sent by the client to a tBaseVideoFrame_JDEmz
* structure required by the codec.
*
* @param [in] rawBuf
*     The buffer type sent by the client.
*
* @param [inout] imgBuf
*     The structure requireed by the codec.
*
* @param [inout] nHeight
*     The height of the image in this buffer.
*
* @param [in] nWidth
*     The width of the image in this buffer.
*
* @param [in] nClrFmt
*     The colour format of the image.
*
* @return none
*
*  @see  img.h, common.h
*/
/* ========================================================================== */

void __convert_to_imgprocbuftype(OMX_BUFFERHEADERTYPE * rawBuf,
tBaseVideoFrame_JDEmz* imgBuf,
OMX_U32 nHeight,
OMX_U32 nWidth,
OMX_TICOLOR_FORMATTYPE nClrFmt)
{
    switch(nClrFmt)
    {
    case OMX_TICOLOR_YUV420Planar :
        imgBuf->lum         = rawBuf->pBuffer;
        imgBuf->cb          = imgBuf->lum + (nHeight * nWidth);
        imgBuf->cr          = imgBuf->cb + (nHeight * nWidth /4);
        imgBuf->width       = nWidth;
        imgBuf->height      = nHeight;
        imgBuf->timeStamp   = 0;
        break;
    case OMX_TICOLOR_YUV422Planar:
        imgBuf->lum = rawBuf->pBuffer;
        imgBuf->cb          = imgBuf->lum + (nHeight * nWidth);
        imgBuf->cr          = imgBuf->cb + (nHeight * nWidth/2 );
        imgBuf->width       = nWidth;
        imgBuf->height      = nHeight;
        imgBuf->timeStamp   = 0;
        break;
    case OMX_TICOLOR_YUV444Planar:
        imgBuf->lum = rawBuf->pBuffer;
        imgBuf->cb          = imgBuf->lum + (nHeight * nWidth);
        imgBuf->cr          = imgBuf->cb + (nHeight * nWidth );
        imgBuf->width       = nWidth;
        imgBuf->height      = nHeight;
        imgBuf->timeStamp   = 0;
        break;
    default:
        imgBuf->lum         = rawBuf->pBuffer;
        imgBuf->cb          = NULL;
        imgBuf->cr          = NULL;
        imgBuf->width       = nWidth;
        imgBuf->height      = nHeight;
        imgBuf->timeStamp   = 0;
        break;
    }
}

/* ========================================================================== */
/** @fn __release_buffers method is used to release all the
* bufffers in the input and output queue of the core component
* and send the callbacks to the client.
*
* @param [in] oper
*     The operation handle for the current operation.
*
* @return none
*
*  @see  img.h, img_buffer_queue.h
*/
/* ========================================================================== */

void __release_buffers(IMG_CORE_OPERATIONTYPE *oper)
{
    OMX_PTR relBuf;
    while(oper->pInpBufQueue.nLast)
    {
        relBuf =(OMX_PTR *) buf_dequeue(&oper->pInpBufQueue);
        oper->pCoreCallback(oper->hComp, OMX_ErrorNone, OMX_IMG_CMD_QUEUE_INPBUF, relBuf );
    }

    while(oper->pOutBufQueue.nLast)
    {
        relBuf =(OMX_PTR *) buf_dequeue(&oper->pOutBufQueue);
        oper->pCoreCallback(oper->hComp, OMX_ErrorNone, OMX_IMG_CMD_QUEUE_OUTBUF, relBuf );
    }
}

/* ========================================================================== */
/** @fn __timer_start method starts the timer and
* returns the clock count.
*
* @return OMX_U8
*              Value of the timer register
*
*  @see  img.h
*/
/* ========================================================================== */

OMX_U16 __timer_start()
{
#ifdef IMG_TIMER
    Dtimer1_Start(1);
    {
        volatile int foo=10000;
        while(foo--);
    }
    return (OMX_U16)Dtimer1_ReadValue();
#endif
    return 0;
}

/* ========================================================================== */
/** @fn __timer_stop method stops the timer and
* returns the clock count.
*
* @return OMX_U8
*              Value of the timer register
*
*  @see  img.h
*/
/* ========================================================================== */

OMX_U16 __timer_stop()
{
#ifdef IMG_TIMER
    OMX_U16 timer_read;
    timer_read= Dtimer1_ReadValue();
    Dtimer1_Start(0);
    return (OMX_U16)timer_read;
#endif
    return 0;
}

/* ========================================================================== */
/** @fn __timer_init method initializes the timer
*
* @return OMX_U8
*              Value of the timer register
*
*  @see  img.h
*/
/* ========================================================================== */

void __timer_init()
{
#ifdef IMG_TIMER
    Dtimer1_Start(0);
    Dtimer1_Init_cntl(0xFFFF,0,1,1);
#endif
}
