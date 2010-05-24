/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file msl_imgthmb.h
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MSL_IMGTHMB_H
#define _MSL_IMGTHMB_H

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
#include "msl_imgthmb.h"

/******************************************************************************
 * enum types
 ******************************************************************************/
/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGTHMB_STATE : Different states that IMGTHMB can be in
 */
/*---------------------------------------------------------------------------*/
typedef enum MSL_IMGTHMB_STATE
{
    MSL_IMGTHMB_STATE_GENERATE,
    MSL_IMGTHMB_STATE_PAUSE,
    MSL_IMGTHMB_STATE_NONE,
    MSL_IMGTHMB_STATE_INIT,
    MSL_IMGTHMB_STATE_DEINIT
} MSL_IMGTHMB_STATE;

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGTHMB_STATUS
 */
/*---------------------------------------------------------------------------*/
typedef struct MSLTHMB_PRIV_STRUCT
{
    /* view finder configurations */
    OMX_TIIMAGE_DECODE_PARAMTYPE                tOMXDecodeConfig;
    OMX_TIIMAGE_ENCODE_PARAMTYPE                tOMXEncodeConfig;
    OMX_TIIMAGE_RESCALE_PARAMTYPE               tOMXRescaleConfig;
    OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE       tOMXColorConvConfig;
    OMX_TIIMAGE_DECODE_IMAGEINFOTYPE            tImgInfo;
        
    MSL_U8                                      sOutputJPEGFileName [MSL_MAXFILENAME_LENGTH];
    MSL_U8                                      sInputJPEGFileName [MSL_MAXFILENAME_LENGTH];
    MSL_FILETYPE                                tEncodeFileType;
    MSL_FILETYPE                                tDecodeFileType;
    OMX_TIIMAGE_DECODE_IMAGEINFOTYPE            tImginfo;
    OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE            tEncinfo;

    //OMX and MSL CALLBACK
    OMX_CALLBACKTYPE                            tOMXCallbacks;
    MSL_CALLBACK                                tMSLCallBack;
    MSL_BOOL                                    bColor;
    MSL_BOOL                                    bEncodedone;
    
    OMX_HANDLETYPE                              hOMXRescale;
    OMX_HANDLETYPE                              hOMXEnc;
    OMX_HANDLETYPE                              hOMXDec;
    OMX_HANDLETYPE                              hOMXColorConv;

    OMX_BUFFERHEADERTYPE                        *pImgProcBuffHeader0;
    OMX_BUFFERHEADERTYPE                        *pImgProcBuffHeader1;

    //OMX_U8 *pEncodeip;
    OMX_U8                                      *pBuff0;
    OMX_U8                                      *pBuff1;
    MSL_IMGTHMB_STATE                           tImgThmbState;

}MSL_IMGTHMB_PRIV_STRUCT;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
/**
 * MSL_IMgThmbt_OMXFillbufferDone
 *
 * @see fillbuffer call back for omx components  */
OMX_ERRORTYPE MSL_ImgThmb_OMXFillbufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE * pBuffHeader);

/**
 * MSL_IMgThmb_OMXEmptybufferDone
 *
 * @see fillbuffer done callback for omx components */
OMX_ERRORTYPE MSL_ImgThmb_OMXEmptybufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE * pBuffHeader);

/**
 * OMX_ERRORTYPE MSL_IMgThmb_OMXEventHandle
 *
 * @see event handler for omx component */
OMX_ERRORTYPE MSL_ImgThmb_OMXEventHandle(
        OMX_HANDLETYPE hComponent,
        OMX_PTR pAppData,               
        OMX_EVENTTYPE eEvent,    
        OMX_U32 nData1,                            
        OMX_U32 nData2,            
        OMX_PTR pEventData);


MSL_IMGTHMB_STATUS MSL_ImgThmb_FillDecoderBuff(MSL_IMGTHMB_PRIV_STRUCT *_hMSLIMGThmb);

MSL_STATUS MSL_ImgThmb_SaveEncodedBuffer(MSL_IMGTHMB_PRIV_STRUCT* _hMSLIMGThmb, OMX_BUFFERHEADERTYPE *pBuffHeader);

#endif /* _MSL_IMGTHMB_H */
#ifdef __cplusplus
} /* extern "C" */
#endif

