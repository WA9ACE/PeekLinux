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
 *! 16-March-2006 Narendran M R: Initial code
 *!
 * ========================================================================= */

/****************************************************************
 *  INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include "_msl_api.h"
#include "vsi.h"
#include "os.h"
#include "frm_glob.h"
MSL_GLOBALFEATURE_CONFIGTYPE tMSLGloablFeatures =
{
      SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT, MAX_BURSTCOUNT, 3, MAX_ENCODED_BUFFER_SIZE,
      #if BITSPERPIXEL_IMAGE == 2
        MSL_FALSE,
      #else
        MSL_TRUE,
      #endif
      #ifdef MSL_SINGLEBUFFERMODE
          MSL_TRUE,
        #else
          MSL_FALSE,
        #endif
};

/* ========================================================================== */
/**
 * @fn MSL_SetGloablConfiguration
 * This API sets the global configuration for MSL
 *
 * @param [in] tGlobalFeature
 *     Pointer to global structure parameter
 *
 * @return MSL_IMGCAP_STATUS
 *     If the command successfully executes, the return code will be
 *     MSL_IMGCAP_STATUS_OK.  Otherwise the appropriate MSL  error will be returned.
 *
 *  @see  msl_api.h
 */
/* ========================================================================== */
MSL_VOID MSL_SetGloablConfiguration(MSL_GLOBALFEATURE_CONFIGTYPE *ptGlobalFeature)
{
    //copy parameters to global structures
    tMSLGloablFeatures.nMaxImgWidth                     = ptGlobalFeature->nMaxImgWidth;
    tMSLGloablFeatures.nMaxImgHeight                    = ptGlobalFeature->nMaxImgHeight;
    tMSLGloablFeatures.nMaxBurstCount                   = ptGlobalFeature->nMaxBurstCount;
    tMSLGloablFeatures.nMaxCompressedImageSize          = ptGlobalFeature->nMaxCompressedImageSize;
    tMSLGloablFeatures.b24BitFormatSupport              = ptGlobalFeature->b24BitFormatSupport;
    tMSLGloablFeatures.nMaxSimultaneousImgPreviews      = ptGlobalFeature->nMaxSimultaneousImgPreviews;
    tMSLGloablFeatures.bSingleBufferMode                  = ptGlobalFeature->bSingleBufferMode;
    return ;
}

/* ========================================================================== */
/**
 * @fn MSL2OMXColor
 *
 * @param [in]tMSLColor
 *     msl color enutype
 *
 * @return OMX_TICOLOR_FORMATTYPE
 *
 *
 *  @see  _msl_api.h
 */
/* ========================================================================== */
OMX_TICOLOR_FORMATTYPE MSL2OMXColor(MSL_COLOR_FORMATTYPE tMSLColor)
{
    if(tMSLColor == MSL_COLOR_RGB565)
        return OMX_TICOLOR_16bitRGB565;

    if(tMSLColor == MSL_COLOR_YUYV)
        return OMX_TICOLOR_YCbYCr;

    if(tMSLColor == MSL_COLOR_YUV444)
        return OMX_TICOLOR_YUV444Planar;

    if(tMSLColor == MSL_COLOR_YUV420)
        return OMX_TICOLOR_YUV420Planar;

    if(tMSLColor == MSL_COLOR_RGB444)
        return OMX_TICOLOR_12bitRGB444;

    if(tMSLColor == MSL_COLOR_MONOCHROME)
        return OMX_TICOLOR_Monochrome;

    //default color rgb565
    return OMX_TICOLOR_16bitRGB565;
}
/* ========================================================================== */
/**
 * @fn MSL2OMXRotate
 *
 * @param [in] tMSLRotate
 *
 * @return OMX_TIIMAGE_ROTATETYPE
 *
 *
 *  @see  _msl_api.h
 */
/* ========================================================================== */
OMX_TIIMAGE_ROTATETYPE MSL2OMXRotate(MSL_IMG_ROTATETYPE tMSLRotate)
{
    if(tMSLRotate == MSL_ROTATE_0)
        return OMX_TIIMAGE_ROTATE0;

    if(tMSLRotate == MSL_ROTATE_90)
        return OMX_TIIMAGE_ROTATE90;

    if(tMSLRotate == MSL_ROTATE_180)
        return OMX_TIIMAGE_ROTATE180;

    if(tMSLRotate == MSL_ROTATE_270)
        return OMX_TIIMAGE_ROTATE270;

    //default color rgb565
    return OMX_TIIMAGE_ROTATE0;
}

/* ========================================================================== */
/**
 * @fn MSL2OMXOverlay
 *
 * @param [in] tOverlayMode
 *     Omx overlay type
 *
 * @return OMX_TIIMAGE_OVERLAYTYPE
 *
 *  @see  _msl_api.h
 */
/* ========================================================================== */
OMX_TIIMAGE_OVERLAYTYPE     MSL2OMXOverlay(MSL_OVERLAY_MODE tOverlayMode)
{
    if(MSL_OVERLAYMODE_NOOVERLAY == tOverlayMode)
        return OMX_TIIMAGE_OVERLAYUNUSED;

    if(MSL_OVERLAYMODE_OVERLAP == tOverlayMode)
        return OMX_TIIMAGE_OVERLAYUNUSED;

    if(MSL_OVERLAYMODE_COLORKEY == tOverlayMode)
        return OMX_TIIMAGE_OVERLAY;

    if(MSL_OVERLAYMODE_ALPHABLENDING == tOverlayMode)
        return OMX_TIIMAGE_ALPHABLEND;

    if(MSL_OVERLAYMODE_ALPHABLENDINGANDCOLORKEY == tOverlayMode)
        return OMX_TIIMAGE_ALPHAOVERLAY;

    return OMX_TIIMAGE_OVERLAYUNUSED;
}

/* ========================================================================== */
/**
 * @fn MSL2OMXEffect
 *
 * @param [in] tEffectMode
 *     Omx effect type
 *
 * @return OMX_TIIMAGE_EFFECTTYPE
 *
 *  @see  _msl_api.h
 */
/* ========================================================================== */
OMX_TIIMAGE_EFFECTTYPE MSL2OMXEffect(MSL_IMG_EFFECTTYPE tMSLEffect)
{
    if( tMSLEffect == MSL_EFFECT_UNUSED)
        return OMX_TIIMAGE_EFFECTUNUSED;

    if( tMSLEffect == MSL_EFFECT_SEPIAEFFECT)
        return OMX_TIIMAGE_SEPIAEFFECT;

    if( tMSLEffect == MSL_EFFECT_GRAYEFFECT)
        return OMX_TIIMAGE_GRAYEFFECT;

    if( tMSLEffect == MSL_EFFECT_EFFECTMAX)
        return OMX_TIIMAGE_EFFECTMAX;

    return OMX_TIIMAGE_EFFECTUNUSED;

}
OMX_PTR __msl_alloc(MSL_U32 nSize)
{
    T_HANDLE hTask;
    OMX_PTR pBuffer;
    hTask = e_running[os_MyHandle()];
    if (os_AllocateMemory ( hTask, (T_VOID_STRUCT**)&pBuffer,nSize,OS_NO_SUSPEND, mm_ext_data_pool_handle)!=OS_OK)
        return NULL;
    return pBuffer;
}

MSL_U16 __msl_free(OMX_PTR pBuffer)
{
    MSL_U16 retVal;
    if(pBuffer != NULL)
    {
         T_HANDLE hTask;
         hTask = e_running[os_MyHandle()];
         retVal = os_DeallocateMemory(hTask, (T_VOID_STRUCT *) pBuffer);
    }
    return retVal;
}

