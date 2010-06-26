/* =============================================================================*/
/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file OMX_IMGComponent.c
*
* This file contains methods that provides functionality for the imaging OMX
* component compliant to OpenMax 1.0 Specification for Nucleus OS.
*
* @path  chipsetsw\services\img\omx_img\src
*
* @rev  0.1
*
*/
/* ------------------------------------------------------------------------- */
/* =========================================================================
*!
*! Revision History
*! ===================================
*! 15-Feb-2006 anandhi@ti.com: Initial Release
*!
* ========================================================================= */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <string.h>
#include <stdlib.h>

/*-------program files ----------------------------------------*/
#include "OMX_Core.h"
#include "OMX_TIImage.h"
#include "OMX_IMGComponent.h"
#include "OMX_IMG_Private.h"
extern T_HANDLE mm_ext_data_pool_handle;

/* Note: This imaging component is OMX 1.0 compliant */
static const OMX_VERSIONTYPE SupportedOMXVersion = {0x01,0x00, 0x00, 0x00};

/* ========================================================================== */
/**
* @fn OMX_IMG_GetComponentVersion will return information about the component.
*
* @param [in] hComponent
*     handle of component to execute the command
* @param [out] pComponentName
*     pointer to an empty string of length 128 bytes.
* @param [out] pComponentVersion
*     pointer to an OMX Version structure that the component will fill in.
* @param [out] pSpecVersion
*     pointer to an OMX Version structure that the component will fill in.
* @param [out] pComponentUUID
*     pointer to the UUID of the component which will be filled in by
*     the component.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_GetComponentVersion(OMX_HANDLETYPE hComponent,
OMX_STRING pComponentName,
OMX_VERSIONTYPE* pComponentVersion,
OMX_VERSIONTYPE* pSpecVersion,
OMX_UUIDTYPE* pComponentUUID)
{

    OMX_COMPONENTTYPE * pIMGHandle;
    OMX_ERRORTYPE               tRetVal         = OMX_ErrorNone;
    OMX_IMG_PRIVATE     *   pImgPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_GetComponentVersion");
    OMX_IMG_EXIT_IF((NULL == hComponent), OMX_ErrorInvalidComponent);
    OMX_IMG_EXIT_IF((NULL == pComponentVersion), OMX_ErrorBadParameter);
    OMX_IMG_EXIT_IF((NULL == pSpecVersion), OMX_ErrorBadParameter);

    pIMGHandle = (OMX_COMPONENTTYPE *)hComponent;
    pImgPrivate = (OMX_IMG_PRIVATE *)pIMGHandle->pComponentPrivate ;

    pComponentName     = OMX_TIIMG_COMPONENT;
    *(OMX_U32 *)pComponentVersion = pImgPrivate->nCompVersion.nVersion;
    *(OMX_U32 *)pSpecVersion      = pIMGHandle->nVersion.nVersion;
    *(OMX_U32 *)pComponentUUID    = NULL;

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_GetComponentVersion");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_SendCommand method send a command to the component. The component must
* check the parameters and then queue the command to the component thread to
* be executed. The component thread must send the HandleEvent callback at the
* conclusion of the command.  This method will go directly from the application
* to the component (via a core macro).
*
* @param [in] hComponent
*     handle of component to execute the command
* @param [in] Cmd
*     Command for the component to execute
* @param [in] nParam
*     Parameter for the command to be executed.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_SendCommand( OMX_HANDLETYPE hComponent,
OMX_COMMANDTYPE Cmd,
OMX_U32 nParam,
OMX_PTR pCmdData)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_SendCommand");
    /* Check the input parameters. */
    OMX_IMG_EXIT_IF (NULL == hComponent, OMX_ErrorInvalidComponent);

    //Only state change command is implemented so far.
    OMX_IMG_EXIT_IF (OMX_CommandStateSet != Cmd, OMX_ErrorNotImplemented);

    /* Function to handle the command sequence/valid command */
    tRetVal = __OMX_IMG_HandleCommand(hComponent, nParam);

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_SendCommand");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_GetParameter method will get one of the current parameter
* settings from the component.  This method can only be invoked when
* the component is in the OMX_LoadedState state.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] nParamIndex
*     Index of the structure to be filled.  This value is from the
*     OMX_TIIMAGE_INDEXTYPE enumeration above.
* @param [in,out] ComponentParameterStructure
*     Pointer to application allocated structure to be filled by the
*     component.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_GetParameter(OMX_HANDLETYPE hComponent,
OMX_TIIMAGE_INDEXTYPE nParamIndex,
OMX_PTR ComponentParameterStructure)
{
    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_GetParameter");
#ifdef IMG_ADVANCED
    OMX_ERRORTYPE tRetVal       = OMX_ErrorNone;
    OMX_IMG_PRIVATE     *               pImgPrivate;
    OMX_IMG_COMP_PARAMTYPE*     tCompParam;

    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    tCompParam =  (OMX_IMG_COMP_PARAMTYPE*) &(pImgPrivate->tCompParam);

    /** check if the component is in Loaded State **/
    OMX_IMG_EXIT_IF (pImgPrivate->tCurState == OMX_StateLoaded, OMX_ErrorInvalidState);

    switch(nParamIndex){

    case OMX_IndexParamEncode:
        if (tCompParam->tCompType == OMX_ENCODE_COMPTYPE)
        {
            memcpy(ComponentParameterStructure, &(tCompParam->tParamType.tEncodeParam),
            sizeof(OMX_TIIMAGE_ENCODE_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexParamDecode:
        if (tCompParam->tCompType == OMX_DECODE_COMPTYPE)
        {
            memcpy(ComponentParameterStructure, &(tCompParam->tParamType.tDecodeParam),
            sizeof(OMX_TIIMAGE_DECODE_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

        default : tRetVal = OMX_ErrorUndefined;
    }


EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_GetParameter");
    return tRetVal ;
#endif

#ifndef IMG_ADVANCED
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_GetParameter");
    return OMX_ErrorNotImplemented;
#endif

}

/* ========================================================================== */
/**
* @fn OMX_IMG_OMX_SetParameter method will send a initialization parameter
* structure to a component.  Each structure must be sent one at a time,
* each in a separate invocation of the method.  This method can only
* be invoked when the component is in the OMX_LoadedState state.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] nIndex
*     Index of the structure to be sent.  This value is from the
*     OMX_TIIMAGE_INDEXTYPE enumeration above.
* @param [in] ComponentParameterStructure
*     pointer to application allocated structure to be used for
*     initialization by the component.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_SetParameter(OMX_HANDLETYPE hComponent,
OMX_TIIMAGE_INDEXTYPE nIndex,
OMX_PTR ComponentParameterStructure)
{
    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_SetParameter");
#ifndef IMG_ADVANCED
    return OMX_ErrorNotImplemented;
#endif

#ifdef IMG_ADVANCED
    OMX_ERRORTYPE tRetVal       =       OMX_ErrorNone;
    OMX_IMG_PRIVATE                     *pImgPrivate;


    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    OMX_IMG_EXIT_IF (pImgPrivate->tCurState != OMX_StateLoaded, OMX_ErrorInvalidState) ;
    OMX_IMG_EXIT_IF (NULL == hComponent, OMX_ErrorInvalidComponent);
    OMX_IMG_EXIT_IF (ComponentParameterStructure == NULL, OMX_ErrorBadParameter);

    switch(nIndex) {

    case OMX_IndexParamEncode: {

            OMX_TIIMAGE_ENCODE_PARAMTYPE* pEncodeParam;
            pEncodeParam = (OMX_TIIMAGE_ENCODE_PARAMTYPE*)ComponentParameterStructure;

            // check for validity of parameters
            OMX_IMG_EXIT_IF ((pEncodeParam->tImageCodingType != OMX_IMAGE_CodingJPEG ||
            (pEncodeParam->nQualityFactor <1 ||
            pEncodeParam->nQualityFactor >100 ) ||
            (pEncodeParam->tInputImageFormat != OMX_TICOLOR_YUV420Planar &&
            pEncodeParam->tInputImageFormat != OMX_TICOLOR_YUV422Planar&&
            pEncodeParam->tInputImageFormat != OMX_TICOLOR_YCbYCr) ||
            pEncodeParam->tImageEncodeFormat != OMX_TICOLOR_YCbYCr ) ,OMX_ErrorBadParameter);

            memcpy(&(pImgPrivate->tCompParam.tParamType.tEncodeParam), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_ENCODE_PARAMTYPE));

            pImgPrivate->tCompParam.tCompType = OMX_ENCODE_COMPTYPE;

            break;
        }
    case OMX_IndexParamDecode : {

            memcpy(&(pImgPrivate->tCompParam.tParamType.tDecodeParam), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_DECODE_PARAMTYPE));
            pImgPrivate->tCompParam.tCompType  = OMX_DECODE_COMPTYPE;
            break;
        }

    case OMX_IndexParamRotate: {

            OMX_TIIMAGE_ROTATE_PARAMTYPE *pRotateParam;
            pRotateParam = (OMX_TIIMAGE_ROTATE_PARAMTYPE*)ComponentParameterStructure;

            // check for validity of parameters
            OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pRotateParam->tInputImageFormat),
            OMX_ErrorBadParameter);

            memcpy(&(pImgPrivate->tCompParam.tParamType.tRotateParam), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_ROTATE_PARAMTYPE));

            pImgPrivate->tCompParam.tCompType = OMX_ROTATE_COMPTYPE;

            break;
        }
    case OMX_IndexParamRescale: {

            OMX_TIIMAGE_RESCALE_PARAMTYPE *pRescaleParam;
            pRescaleParam = (OMX_TIIMAGE_RESCALE_PARAMTYPE*)ComponentParameterStructure;

            // check for validity of parameters
            OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pRescaleParam->tInputImageFormat),
            OMX_ErrorBadParameter);

            memcpy(&(pImgPrivate->tCompParam.tParamType.tRescaleParam ), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE));

            pImgPrivate->tCompParam.tCompType= OMX_RESCALE_COMPTYPE;

            break;
        }
    case OMX_IndexParamEffect: {

            OMX_TIIMAGE_EFFECT_PARAMTYPE *pEffectParam;
            pEffectParam = (OMX_TIIMAGE_EFFECT_PARAMTYPE*)ComponentParameterStructure;

            // check for validity of parameters
            OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pEffectParam->tInputImageFormat),
            OMX_ErrorBadParameter);

            memcpy(&(pImgPrivate->tCompParam.tParamType.tEffectParam ), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_EFFECT_PARAMTYPE));

            pImgPrivate->tCompParam.tCompType = OMX_EFFECT_COMPTYPE;

            break;
        }
    case OMX_IndexParamOverlay : {

            OMX_TIIMAGE_OVERLAY_PARAMTYPE *pOverlayParam;
            pOverlayParam = (OMX_TIIMAGE_OVERLAY_PARAMTYPE*)ComponentParameterStructure;

            // check for validity of parameters
            OMX_IMG_EXIT_IF ((pOverlayParam->tSrcImageFormat != OMX_TICOLOR_16bitRGB565 ||
            pOverlayParam->tOverlayImageFormat != OMX_TICOLOR_16bitRGB565 ),
            OMX_ErrorBadParameter);

            memcpy(&(pImgPrivate->tCompParam.tParamType.tOverlayParam), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_OVERLAY_PARAMTYPE));

            pImgPrivate->tCompParam.tCompType = OMX_OVERLAY_COMPTYPE;

            break;
        }
    case OMX_IndexParamColorConversion: {

            OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE *pColConvParam;
            pColConvParam = (OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE*)ComponentParameterStructure;
            // check for validity of parameters
            OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pColConvParam->tInputImageFormat),
            OMX_ErrorBadParameter);
            memcpy(&(pImgPrivate->tCompParam.tParamType.tColorConvParam ), ComponentParameterStructure,
            sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));
            pImgPrivate->tCompParam.tCompType = OMX_COLORCONVERSION_COMPTYPE;

            break;
        }
        default : tRetVal = OMX_ErrorNotImplemented;
    }


EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_SetParameter");
    return tRetVal ;
#endif
}

/* ========================================================================== */
/**
* @fn OMX_IMG_OMX_GetConfig method will get one of the configuration structures
* from a component.  This method can be invoked anytime after the
* component has been loaded.  The nParamIndex parameter is used to
* indicate which structure is being requested from the component.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] nIndex
*     Index of the structure to be filled.  This value is from the
*     OMX_TIIMAGE_INDEXTYPE enumeration above.
* @param [in,out] ComponentConfigStructure
*     pointer to application allocated structure to be filled by the
*     component.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_GetConfig(OMX_HANDLETYPE hComponent,
OMX_INDEXTYPE nIndex,
OMX_PTR pComponentConfigStructure)
{

    OMX_ERRORTYPE tRetVal       =       OMX_ErrorNone;
    OMX_IMG_PRIVATE     *       pImgPrivate;
    OMX_IMG_COMP_PARAMTYPE*     tCompParam;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_GetConfig");
    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    tCompParam =  (OMX_IMG_COMP_PARAMTYPE*) &(pImgPrivate->tCompParam);

    OMX_IMG_EXIT_IF (pImgPrivate->tCurState == OMX_StateInvalid, OMX_ErrorInvalidState);
    OMX_IMG_EXIT_IF (NULL == hComponent, OMX_ErrorInvalidComponent);
    OMX_IMG_EXIT_IF (pComponentConfigStructure == NULL, OMX_ErrorBadParameter);


    switch((OMX_TIIMAGE_INDEXTYPE)nIndex) {
#ifdef IMG_ADVANCED
    case OMX_IndexConfigEncode :
        if (tCompParam->tCompType == OMX_ENCODE_COMPTYPE)
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tEncodeParam),
            sizeof(OMX_TIIMAGE_ENCODE_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;


    case OMX_IndexConfigDecode :
        if (tCompParam->tCompType == OMX_DECODE_COMPTYPE)
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tDecodeParam),
            sizeof(OMX_TIIMAGE_DECODE_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexConfigRotate:
        if (tCompParam->tCompType == OMX_ROTATE_COMPTYPE)
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tRotateParam ),
            sizeof(OMX_TIIMAGE_ROTATE_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexConfigRescale:
        if (tCompParam->tCompType == OMX_RESCALE_COMPTYPE)
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tRescaleParam),
            sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexConfigEffect:
        if (tCompParam->tCompType == OMX_EFFECT_COMPTYPE  )
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tEffectParam ),
            sizeof(OMX_TIIMAGE_EFFECT_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexConfigOverlay:
        if (tCompParam->tCompType == OMX_OVERLAY_COMPTYPE  )
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tOverlayParam),
            sizeof(OMX_TIIMAGE_OVERLAY_PARAMTYPE));
        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexConfigColorConversion:
        if (tCompParam->tCompType == OMX_COLORCONVERSION_COMPTYPE  )
        {
            memcpy(pComponentConfigStructure, &(tCompParam->tParamType.tColorConvParam),
            sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));
        }

        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;
#endif
    case OMX_IndexConfigEncodeImgInfo:
        if (tCompParam->tCompType == OMX_ENCODE_COMPTYPE)
        {
            memcpy(pComponentConfigStructure, &(pImgPrivate->tCompParam.tParamType.tEncodeInfoParam),
            sizeof(OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE));

        }
        else
        {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

    case OMX_IndexConfigDecodeImgInfo:
        if (tCompParam->tCompType == OMX_DECODE_COMPTYPE)
        {
            memcpy(pComponentConfigStructure, &(pImgPrivate->tCompParam.tParamType.tDecodeInfoParam),
            sizeof(OMX_TIIMAGE_DECODE_IMAGEINFOTYPE));

        }
        else {
            tRetVal = OMX_ErrorUnsupportedIndex;
        }
        break;

        default : tRetVal = OMX_ErrorUndefined;
    }

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_GetConfig");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_OMX_SetConfig method will send one of the configuration
* structures to a component.  Each structure must be sent one at a
* time, each in a separate invocation of the method.  This method can
* be invoked anytime after the component has been loaded.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] nIndex
*     Index of the structure to be sent.  This value is from the
*     OMX_TIIMAGE_INDEXTYPE enumeration above.
* @param [in] ComponentConfigStructure
*     pointer to application allocated structure to be used for
*     initialization by the component.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_SetConfig(OMX_HANDLETYPE hComponent,
OMX_INDEXTYPE nIndex,
OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE tRetVal       =       OMX_ErrorNone;
    OMX_IMG_PRIVATE                     *pImgPrivate;
    OMX_IMG_COMP_PARAMTYPE*     tCompParam;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_SetConfig");
    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    tCompParam =  (OMX_IMG_COMP_PARAMTYPE*) &(pImgPrivate->tCompParam);

    OMX_IMG_EXIT_IF ((pImgPrivate->tCurState == OMX_StateInvalid )
    ,OMX_ErrorInvalidState) ;

    OMX_IMG_EXIT_IF (NULL == hComponent, OMX_ErrorInvalidComponent);

    OMX_IMG_EXIT_IF (pComponentConfigStructure == NULL, OMX_ErrorBadParameter);


    switch((OMX_TIIMAGE_INDEXTYPE)nIndex) {

    case OMX_IndexConfigEncode :
        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType == OMX_ENCODE_COMPTYPE)
            {
                OMX_TIIMAGE_ENCODE_PARAMTYPE *pCompConfig;
                pCompConfig = (OMX_TIIMAGE_ENCODE_PARAMTYPE *) pComponentConfigStructure;
                // check for validity of parameters
                OMX_IMG_EXIT_IF ((pCompConfig->tImageCodingType != OMX_IMAGE_CodingJPEG ||
                (pCompConfig->nQualityFactor <1 || pCompConfig->nQualityFactor >100) ||
                (pCompConfig->tInputImageFormat != OMX_TICOLOR_YUV420Planar &&
                pCompConfig->tInputImageFormat != OMX_TICOLOR_YUV422Planar&&
                pCompConfig->tInputImageFormat != OMX_TICOLOR_YCbYCr) ||
                (pCompConfig->tImageEncodeFormat == OMX_TICOLOR_YCbYCr) ) ,OMX_ErrorBadParameter);
            }
        }
        memcpy(&(tCompParam->tParamType.tEncodeParam), pComponentConfigStructure, sizeof(OMX_TIIMAGE_ENCODE_PARAMTYPE));
        tCompParam->tCompType = OMX_ENCODE_COMPTYPE;

        break;

    case OMX_IndexConfigDecode :
        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType == OMX_DECODE_COMPTYPE)
            {
                OMX_TIIMAGE_DECODE_PARAMTYPE *pCompConfig;

                pCompConfig = (OMX_TIIMAGE_DECODE_PARAMTYPE *) pComponentConfigStructure;

            }
        }
        memcpy(&(tCompParam->tParamType.tDecodeParam), pComponentConfigStructure,
        sizeof(OMX_TIIMAGE_DECODE_PARAMTYPE));
        tCompParam->tCompType = OMX_DECODE_COMPTYPE;

        break;

    case OMX_IndexConfigRotate:
        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType > OMX_IMG_PROC_COMPTYPE)
            {
                OMX_TIIMAGE_ROTATE_PARAMTYPE *pCompConfig;

                pCompConfig = (OMX_TIIMAGE_ROTATE_PARAMTYPE *) pComponentConfigStructure;

                OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pCompConfig->tInputImageFormat),
                OMX_ErrorBadParameter);
            }
        }
        memcpy(&(tCompParam->tParamType.tRotateParam), pComponentConfigStructure,
        sizeof(OMX_TIIMAGE_ROTATE_PARAMTYPE));
        tCompParam->tCompType = OMX_ROTATE_COMPTYPE;

        break;

    case OMX_IndexConfigRescale :
        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType > OMX_IMG_PROC_COMPTYPE)
            {
                OMX_TIIMAGE_RESCALE_PARAMTYPE *pCompConfig;

                pCompConfig = (OMX_TIIMAGE_RESCALE_PARAMTYPE *) pComponentConfigStructure;

                OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pCompConfig->tInputImageFormat),
                OMX_ErrorBadParameter);

            }
        }
        memcpy(&(tCompParam->tParamType.tRescaleParam), pComponentConfigStructure,
        sizeof(OMX_TIIMAGE_RESCALE_PARAMTYPE));
        tCompParam->tCompType = OMX_RESCALE_COMPTYPE;
        break;

    case OMX_IndexConfigEffect:

        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType > OMX_IMG_PROC_COMPTYPE)
            {
                OMX_TIIMAGE_EFFECT_PARAMTYPE *pCompConfig;

                pCompConfig = (OMX_TIIMAGE_EFFECT_PARAMTYPE *) pComponentConfigStructure;

                OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pCompConfig->tInputImageFormat),
                OMX_ErrorBadParameter);
            }
        }
        memcpy(&(tCompParam->tParamType.tEffectParam), pComponentConfigStructure,
        sizeof(OMX_TIIMAGE_EFFECT_PARAMTYPE));
        tCompParam->tCompType = OMX_EFFECT_COMPTYPE;
        break;

    case OMX_IndexConfigOverlay:

        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType > OMX_IMG_PROC_COMPTYPE)
            {
                OMX_TIIMAGE_OVERLAY_PARAMTYPE *pCompConfig;

                pCompConfig = (OMX_TIIMAGE_OVERLAY_PARAMTYPE *) pComponentConfigStructure;
            }
        }
        memcpy(&(tCompParam->tParamType.tOverlayParam), pComponentConfigStructure,
        sizeof(OMX_TIIMAGE_OVERLAY_PARAMTYPE));
        tCompParam->tCompType = OMX_OVERLAY_COMPTYPE;
        break;

    case OMX_IndexConfigColorConversion:

        if(pImgPrivate->tCurState > OMX_StateLoaded) {
            if (tCompParam->tCompType > OMX_IMG_PROC_COMPTYPE)
            {
                OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE *pCompConfig;

                pCompConfig = (OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE *) pComponentConfigStructure;

                OMX_IMG_EXIT_IF (__check_imgproc_colorformat(pCompConfig->tInputImageFormat),
                OMX_ErrorBadParameter);
            }
        }    memcpy(&(tCompParam->tParamType.tColorConvParam), pComponentConfigStructure,
        sizeof(OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE));
        tCompParam->tCompType = OMX_COLORCONVERSION_COMPTYPE;

        break;

        default : tRetVal = OMX_ErrorNotImplemented;
    }

    OMX_IMG_EXIT_IF(tRetVal != OMX_ErrorNone, tRetVal);

    if(pImgPrivate->tCurState > OMX_StateLoaded)
    {
        //Send message to the IMG Core to set the encode parameters.
        __OMX_IMG_SendMsgToCore(hComponent, OMX_IMG_CMD_SETCONFIG, tCompParam);
    }

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_SetConfig");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_OMX_GetState method will return the current state of the
* component and place the state value into the location pointed
* to by pState.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [out] pState
*     pointer to the location to receive the state.  The value returned
*     is one of the OMX_STATETYPE members
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_GetState(OMX_HANDLETYPE hComponent,
OMX_STATETYPE* pState)
{
    OMX_ERRORTYPE tRetVal               = OMX_ErrorNone;
    OMX_IMG_PRIVATE                     *pImgPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_GetState");

    /* Check for valid component handle */
    OMX_IMG_EXIT_IF ((NULL == hComponent), OMX_ErrorBadParameter);

    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /* set the present state of the component */
    *pState = pImgPrivate->tCurState;

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_GetState");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_ComponentTunnelRequest method will interact with another OMX
* component to determine if tunneling is possible and to setup the
* tunneling if it is possible.  The return codes for this method
* can be used to determine of tunneling is not possible
* or if deep or shallow tunneling is used.
*
* @param [in] hInput
*     Handle of the component to be accessed.
* @param [in] nPortInput
*     nPortIndex1 is used to select the port on component 1 to be
*     used in the tunnel.  The port can be found by using the nPortIndex1
*           value as an index into the Port Definition array of component 1.
* @param [in] hOutput
*     This is the component to setup the tunnel with.  In addition,
*     this is the handle of the component that will be used when the
*     nPortIndex2 parameter is used to determine which of the component's
*     ports to tunnel.  This is the component handle returned by the call
*     to the GetHandle function.
* @param [in] nPortOutput
*     nPortIndex2 is used to select the port on component 2 to be
*     used in the tunnel.  The port can be found by using the nPortIndex2
*     value as an index into the Port Definition array of component 2.
* @param [in] eDir direction of the call.
* @param [in] pCallbacks
*     This is a pointer to the callback structure of the caller (used for
*     shallow data tunneling).
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_ComponentTunnelRequest(OMX_HANDLETYPE hInput,
OMX_U32 nPortInput,
OMX_HANDLETYPE hOutput,
OMX_U32 nPortOutput,
OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNotImplemented;

EXIT:
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_EmptyThisBuffer method will send a buffer full of data to an
* input port of the component.  The buffer will be emptied by the
* component and returned to the application via the EmptyBufferDone
* call back.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] nPortIndex
*     nPortIndex1 is used to select the port on the component to be
*     used.  The port can be found by using the nPortIndex
*     value as an index into the Port Definition array of the component.
* @param [in] pBuffer
*     pointer to an OMX_BUFFERHEADERTYPE structure used to provide or
*     receive the pointer to the buffer header.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_EmptyThisBuffer(OMX_HANDLETYPE hComponent,
OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_IMG_PRIVATE                     *pImgPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_EmptyThisBuffer");
    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);


    /* Check for state - The component has to be in Executing or Idle or Paused state */
    OMX_IMG_EXIT_IF ((pImgPrivate->tCurState != OMX_StateExecuting &&
    pImgPrivate->tCurState != OMX_StateIdle &&
    pImgPrivate->tCurState != OMX_StatePause), OMX_ErrorInvalidState);

    /* send a command to the core to queue the buffer and check for success */
    OMX_IMG_EXIT_IF ((__OMX_IMG_SendMsgToCore(hComponent, OMX_IMG_CMD_QUEUE_INPBUF,
    (OMX_PTR)pBuffer) != OMX_ErrorNone), OMX_ErrorNotReady);

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_EmptyThisBuffer");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_OMX_FillThisBuffer method will send an empty buffer to an
* output port of a component.  The buffer will be filled by the
* component and returned to the application via the FillBufferDone
* call back.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] nPortIndex
*     nPortIndex1 is used to select the port on the component to be
*     used.  The port can be found by using the nPortIndex
*     value as an index into the Port Definition array of the component.
* @param [in] pBuffer
*     pointer to an OMX_BUFFERHEADERTYPE structure used to provide or
*     receive the pointer to the buffer header.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*           OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_FillThisBuffer(OMX_HANDLETYPE hComponent,
OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_ERRORTYPE tRetVal = OMX_ErrorNone;
    OMX_IMG_PRIVATE                     *pImgPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_FillThisBuffer");
    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /* Check for state - The component has to be in Executing or Idle or Paused state */
    OMX_IMG_EXIT_IF ((pImgPrivate->tCurState != OMX_StateExecuting &&
    pImgPrivate->tCurState != OMX_StateIdle &&
    pImgPrivate->tCurState != OMX_StatePause), OMX_ErrorInvalidState);

    /* send a command to the core to queue the buffer and check for success */
    OMX_IMG_EXIT_IF ((__OMX_IMG_SendMsgToCore(hComponent, OMX_IMG_CMD_QUEUE_OUTBUF,
    (OMX_PTR)pBuffer) != OMX_ErrorNone), OMX_ErrorNotReady);

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_FillThisBuffer");
    return tRetVal ;
}

/* ========================================================================== */
/** @fn OMX_IMG_SetCallbacks method will allow the core to transfer the callback
* structure from the application to the component.  This is a blocking
* call.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
* @param [in] pCallbacks
*     pointer to an OMX_CALLBACKTYPE structure used to provide the
*     callback information to the component
* @param [in] pAppData
*     pointer to an application defined value.  It is anticipated that
*     the application will pass a pointer to a data structure or a "this
*     pointer" in this area to allow the callback (in the application)
*     to determine the context of the call
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_SetCallbacks(     OMX_HANDLETYPE hComponent,
OMX_CALLBACKTYPE* pCallBacks,
OMX_PTR pAppData)
{
    OMX_ERRORTYPE tRetVal               = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle  = (OMX_COMPONENTTYPE *)hComponent;
    OMX_IMG_PRIVATE*                    pImgPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_SetCallbacks");
    pImgPrivate = (OMX_IMG_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /**Check if the component is in loaded state **/
    OMX_IMG_EXIT_IF ((pImgPrivate->tCurState  !=  OMX_StateLoaded), OMX_ErrorInvalidState);

    /* Check the input parameters. */
    OMX_IMG_EXIT_IF ((hComponent == NULL), OMX_ErrorInvalidComponent);
    OMX_IMG_EXIT_IF ((pCallBacks == NULL), OMX_ErrorBadParameter);
    OMX_IMG_EXIT_IF ((pCallBacks->EmptyBufferDone == NULL), OMX_ErrorBadParameter);
    OMX_IMG_EXIT_IF ((pCallBacks->FillBufferDone == NULL), OMX_ErrorBadParameter);

    //copy to components private structure
    memcpy(&(pImgPrivate->tAppCallback), pCallBacks,  sizeof(OMX_CALLBACKTYPE));
    pHandle->pApplicationPrivate = pAppData;

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_SetCallbacks");
    return tRetVal ;
}

/* ========================================================================== */
/** @fn OMX_IMG_ComponentDeInit method is used to deinitialize the component
* providing a means to free any resources allocated at component
* initialization.  NOTE:  After this call the component handle is
* not valid for further use.
*
* @param [in] hComponent
*     Handle of the component to be accessed.  This is the component
*     handle returned by the call to the GetHandle function.
*
* @return OMX_ERRORTYPE
*     If the command successfully executes, the return code will be
*     OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_ComponentDeInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tRetVal               =       OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle  =       (OMX_COMPONENTTYPE *)hComponent;
    OMX_IMG_PRIVATE*                            pImgPrivate = (OMX_IMG_PRIVATE*)pHandle->pComponentPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_ComponentDeInit");
    /* Check for valid component handle */
    OMX_IMG_EXIT_IF ((NULL == hComponent), OMX_ErrorBadParameter);

    pImgPrivate = (OMX_IMG_PRIVATE*)pHandle->pComponentPrivate;

    OMX_IMG_EXIT_IF ((OMX_StateLoaded != pImgPrivate->tCurState), OMX_ErrorInvalidState);

    /* free the the component private structure */
    OMX_FREE(pHandle->pComponentPrivate);
    pHandle->pComponentPrivate = NULL;

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_ComponentDeInit");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn OMX_IMG_ComponentInit method is the first call into a component
* and is used to perform any one time initialization specific to a component.
* The component MUST fill in all function pointers into the handle.  The
* component may assume that the application has allocated the handle and
* filled in the SIZE and VERSION fields. These should be checked by the
* component.  The component should also allocate and fill in the component
* private data structure, but do no other allocations of memory or
* resources.  Any software or hardware resource or capabilities detection
* should be performed. If the required hardware is not detected, this
* method MUST return OMX_Error_Hardware to inform the application that
* further processing is not possible. In the case of an error, the
* component private data structure must be deallocated.
*
* @param [in] hComponent
*      Handle of the component to be accessed.  This is the component
*       handle returned by the call to the GetHandle function.
*
* @return OMX_ERRORTYPE
*       If the command successfully executes, the return code will be
*       OMX_NoError.  Otherwise the appropriate OMX error will be returned.
*
*
*  @see  OMX_Component.h
*/
/* ========================================================================== */
OMX_ERRORTYPE OMX_IMG_ComponentInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tRetVal               = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle  = (OMX_COMPONENTTYPE *)hComponent;
    OMX_IMG_PRIVATE*                    pImgPrivate;
    T_HANDLE entityHandle;

    OMX_ENTRYEXITFUNC_TRACE ("Enter OMX_IMG_ComponentInit");
    /* Check for valid component handle */
    OMX_IMG_EXIT_IF ((NULL == hComponent) , OMX_ErrorBadParameter);

    entityHandle = e_running[os_MyHandle()];

    /* Check size field of the component structure */
    OMX_IMG_EXIT_IF ((sizeof(OMX_COMPONENTTYPE) != pHandle->nSize), OMX_ErrorBadParameter);

    /* Check version field of the component structure */
    OMX_IMG_EXIT_IF ((pHandle->nVersion.nVersion < SupportedOMXVersion.nVersion),
    OMX_ErrorVersionMismatch);

    /* if need any private data, allocate here  */
    pHandle->pComponentPrivate = NULL;

    /** TODO learn about memory allocation to replace mslrm_malloc**/
    pHandle->pComponentPrivate = (OMX_IMG_PRIVATE*)OMX_ALLOC(sizeof(OMX_IMG_PRIVATE));
    OMX_IMG_EXIT_IF ((NULL == pHandle->pComponentPrivate) , OMX_ErrorInsufficientResources);

    pImgPrivate = (OMX_IMG_PRIVATE*)pHandle->pComponentPrivate;
    pImgPrivate->hSemCaller = (OMX_U32) entityHandle;
    pImgPrivate->pIMGCoreHandle        =    vsi_e_handle(pImgPrivate->hSemCaller, "IMG");
    pImgPrivate->tCurState            =    OMX_StateInvalid;
    pImgPrivate->tCoreErrSts        =    OMX_ErrorNone;

    /* filling the function pointers */
    pHandle->GetComponentVersion = OMX_IMG_GetComponentVersion;
    pHandle->SendCommand = OMX_IMG_SendCommand;
    pHandle->GetParameter = OMX_IMG_GetParameter;
    pHandle->SetParameter = OMX_IMG_SetParameter;
    pHandle->GetConfig = OMX_IMG_GetConfig;
    pHandle->SetConfig = OMX_IMG_SetConfig;
    pHandle->GetState = OMX_IMG_GetState;
    pHandle->ComponentTunnelRequest = OMX_IMG_ComponentTunnelRequest;
    pHandle->EmptyThisBuffer = OMX_IMG_EmptyThisBuffer;
    pHandle->FillThisBuffer = OMX_IMG_FillThisBuffer;
    pHandle->SetCallbacks = OMX_IMG_SetCallbacks;
    pHandle->ComponentDeInit = OMX_IMG_ComponentDeInit;

    /* component state is set to loaded */
    pImgPrivate->tCurState              =       OMX_StateLoaded;

EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit OMX_IMG_ComponentInit");
    return tRetVal ;
}

/* ========================================================================== */
/**
* @fn __OMX_IMG_HandleCommand() This function handles the command sent by
* SendCommand(). It decides which course of action to take based on the
* previous state and the requested state.  It is called from within SendCommand().
* The parameters  hComp and newState are the handle to the component and the state
* that is to be set, respectively.
*
* @param hComponent Handle to the component.
*
* @param newState   The new state that is to be set.
*
* @pre              component has been loaded
*
* @return           OMX_ErrorNone = Successful handle command.
*                   OMX_ErrorInvalidState = improper state transition.
*                   OMX_ErrorBadParameter = parameter is not valid to transition
*
* @see              SendCommand()
*/
/* ========================================================================== */

OMX_ERRORTYPE __OMX_IMG_HandleCommand (OMX_HANDLETYPE hComponent,
OMX_U32 newState)
{
    OMX_ERRORTYPE                           tRetVal            = OMX_ErrorNone;
    OMX_COMPONENTTYPE               *pHandle            = (OMX_COMPONENTTYPE*)hComponent;
    OMX_IMG_PRIVATE                         *pImgPrivate;
    OMX_IMG_COMP_PARAMTYPE          *tCompParam;
    OMX_STATETYPE                         *reqState;
    OMX_IMG_CMDTYPE                         tCmd;
    OMX_PTR                                        tParam;

    OMX_ENTRYEXITFUNC_TRACE ("Enter __OMX_IMG_HandleCommand");
    pImgPrivate = (OMX_IMG_PRIVATE *)pHandle->pComponentPrivate;
    tCompParam = &(pImgPrivate->tCompParam);


    switch ( newState)
    {

    case OMX_StateLoaded:
        /** valid prior states are INVALID, IDLE **/
        OMX_IMG_EXIT_IF ((pImgPrivate->tCurState != OMX_StateIdle && pImgPrivate->tCurState != OMX_StateInvalid
        && pImgPrivate->tCurState != OMX_StateExecuting), OMX_ErrorInvalidState);
        if (pImgPrivate->tCurState == OMX_StateExecuting)
        tCmd = OMX_IMG_CMD_CORE_ABORT;
        if (pImgPrivate->tCurState == OMX_StateIdle)
        tCmd = OMX_IMG_CMD_CORE_FREE;

        tParam = NULL;
        break;

    case OMX_StateIdle:
        /* valid previous states: LOADED, EXECUTING or PAUSE*/
        if (pImgPrivate->tCurState == OMX_StateLoaded)
        {
            /** since all buffers are allocated by the application, no functions to be performed **/
            OMX_IMG_EXIT_IF((tCompParam == NULL), OMX_ErrorNotReady);

            pImgPrivate->tCurState = OMX_StateIdle;
            /* Initialize the core instance */
            tCmd = OMX_IMG_CMD_CORE_INIT;
            tParam = (OMX_PTR)tCompParam;
            break;
        }
        if (pImgPrivate->tCurState == OMX_StateExecuting ||
                pImgPrivate->tCurState == OMX_StatePause)
        {
            tCmd = OMX_IMG_CMD_STATE_CHANGE;
            break;
        }

        OMX_IMG_EXIT_IF(1, OMX_ErrorInvalidState);

    case OMX_StateExecuting:
        /* valid previous states: IDLE or PAUSE*/

        OMX_IMG_EXIT_IF (((pImgPrivate->tCurState != OMX_StateIdle) &&
        (pImgPrivate->tCurState != OMX_StatePause)), OMX_ErrorInvalidState);
        tCmd = OMX_IMG_CMD_STATE_CHANGE;
        break;

    case OMX_StatePause:
        /** valid prior state is Executing  **/
        OMX_IMG_EXIT_IF ((pImgPrivate->tCurState != OMX_StateExecuting), OMX_ErrorInvalidState);
        tCmd = OMX_IMG_CMD_STATE_CHANGE;

        break;

    case OMX_StateInvalid:
        /** valid prior state is Loaded **/
        OMX_IMG_EXIT_IF ((pImgPrivate->tCurState != OMX_StateLoaded), OMX_ErrorInvalidState);
        tCmd = OMX_IMG_CMD_STATE_CHANGE;
        break;

    default:
        /* always exit in this case */
        OMX_IMG_EXIT_IF(1, OMX_ErrorInvalidState);

    } /* End of Switch */

    if(OMX_IMG_CMD_STATE_CHANGE ==  tCmd)
    {
        reqState                 = (OMX_STATETYPE*)OMX_ALLOC(sizeof(OMX_STATETYPE));
        *reqState               = (OMX_STATETYPE) newState;
        tParam                   = reqState;
    }


EXIT:
    if (tRetVal == OMX_ErrorNone)
    {
        pImgPrivate->tCurState = (OMX_STATETYPE) newState;
        __OMX_IMG_SendMsgToCore(hComponent,tCmd,tParam);
    }
    else
    {
        pImgPrivate->tAppCallback.EventHandler (hComponent, pHandle->pApplicationPrivate,
        OMX_EventError, tRetVal, 0, NULL);
    }
    OMX_ENTRYEXITFUNC_TRACE ("Exit __OMX_IMG_HandleCommand");
    return tRetVal;
}
/* ========================================================================== */
/**
* __OMX_IMG_SendMsgToCore()
*
* This function populates a message structure and sends it as a GPF message to the IMG Core task.
* Besides the command and the arguement, it also sends the CALL BACK function to the core so
* that the core may convey the status of the command.
*
* NOTE:
* Synchronization occurs between the Core and the Client through a GPF Semaphore. This is
* to be released by the core to acknowledge a command - the client waits for this semaphore
* and reads the status populated in its private structure to know of any error status.
* For asynchronous functions, though, the client returns to the application immediately and
* must communicate to the application by a callback provided.
*
* @param Cmd command to the IMG core from the client
*
* @param tCoreparam parameter to the IMG Core from the client
*
* @return           None
*
* @see              None
*/
/* ========================================================================== */

OMX_ERRORTYPE __OMX_IMG_SendMsgToCore(OMX_HANDLETYPE hComp, OMX_IMG_CMDTYPE Cmd, OMX_PTR tCoreparam)
{
    OMX_ERRORTYPE               tRetVal         =       OMX_ErrorNone;
    OMX_COMPONENTTYPE*  pHandle         =       (OMX_COMPONENTTYPE*) hComp;
    OMX_IMG_PRIVATE*    pImgPrivate     =       (OMX_IMG_PRIVATE *)pHandle->pComponentPrivate;
    OMX_IMG_CORE_MSGTYPE*       pMsgBuf;

    /** allocate memory for pMsgBuf here **/
    OMX_ENTRYEXITFUNC_TRACE ("Enter __OMX_IMG_SendMsgToCore");
    //TODO: Change the partition
    pMsgBuf = (OMX_IMG_CORE_MSGTYPE *) vsi_c_pnew (sizeof(OMX_IMG_CORE_MSGTYPE), 0 FILE_LINE_MACRO);

    OMX_IMG_EXIT_IF ((pMsgBuf == NULL), OMX_ErrorInsufficientResources);

    /** compose message **/
    pMsgBuf->nCmd = Cmd;
    pMsgBuf->tCompParam = tCoreparam;
    pMsgBuf->pComponentHandle = pHandle ;
    pMsgBuf->pIMGCallback = __OMX_IMG_CoreCallback;

    PSEND(pImgPrivate->pIMGCoreHandle, (T_VOID_STRUCT *) pMsgBuf);
    /*if((PSEND(pImgPrivate->pIMGCoreHandle, (T_VOID_STRUCT *) pMsgBuf)) != VSI_OK)
    {
        tRetVal = OMX_ErrorNotReady;
}*/
EXIT:
    OMX_ENTRYEXITFUNC_TRACE ("Exit __OMX_IMG_SendMsgToCore");
    return tRetVal;
}

/* ========================================================================== */
/**
* __OMX_IMG_CoreCallback()
*
* This is the callback function that is called by the IMG Core for any command
* that is sent. It has all OUT params sent to it as pointers. It calls the function
* with the error status and also populates a strucutre and passes to the
* function in response.
*
* @param hComponent Calling component's handle
*
* @param nErrorStatus Error status updated by the IMG Core
*
* @param pData For OMX_IMG_CMD_QUEUEOUTBUFFER /OMX_IMG_CMD_QUEUEINPBUFFER call,
*                               this parameter is the buffer header data type.
*
* @return        None
*
* @see  __OMX_IMG_SendMsgToCore()
*/
/* ========================================================================== */
void __OMX_IMG_CoreCallback (OMX_HANDLETYPE hComponent,  OMX_ERRORTYPE nErrorStatus,
OMX_IMG_CMDTYPE tCmd, OMX_PTR pData )
{
    OMX_COMPONENTTYPE * pComponent = (OMX_COMPONENTTYPE *)hComponent;
    OMX_BUFFERHEADERTYPE* pBuff = (OMX_BUFFERHEADERTYPE *)pData;
    OMX_IMG_PRIVATE * pImgPrivate = (OMX_IMG_PRIVATE *)pComponent->pComponentPrivate;

    OMX_ENTRYEXITFUNC_TRACE ("Enter __OMX_IMG_CoreCallback");

    if(NULL == hComponent || NULL == pImgPrivate) {

        OMX_ENTRYEXITFUNC_TRACE ("Exit __OMX_IMG_CoreCallback IMG CB EXCEPTION: Comp Pvt or Component Null");

        if(OMX_IMG_CMD_STATE_CHANGE == tCmd ) {
            OMX_FREE(pData);
        }
        return;
    }

    pImgPrivate->tCoreErrSts = nErrorStatus;

    if(OMX_IMG_CMD_QUEUE_INPBUF == tCmd) {
        if(nErrorStatus == OMX_ErrorNone) {
            pImgPrivate->tAppCallback.EmptyBufferDone(hComponent, pComponent->pApplicationPrivate,
            pBuff);
        }
    }
    /*
        * Check if the call back is for FillThisBuffer call.
        * If yes, we need to call back the FillBufferDone function.
    */
    else if(OMX_IMG_CMD_QUEUE_OUTBUF == tCmd) {
        if(nErrorStatus == OMX_ErrorNone) {
            pImgPrivate->tAppCallback.FillBufferDone(hComponent, pComponent->pApplicationPrivate,
            pBuff);
        }
    }

    else if(OMX_IMG_CMD_CORE_ABORT == tCmd ||
            OMX_IMG_CMD_CORE_FREE == tCmd ||
            OMX_IMG_CMD_STATE_CHANGE == tCmd ||
            OMX_IMG_CMD_CORE_INIT == tCmd)
    {
        if( nErrorStatus == OMX_ErrorNone)
        {

            pImgPrivate->tAppCallback.EventHandler (hComponent, pComponent->pApplicationPrivate,
            OMX_EventCmdComplete, OMX_CommandStateSet, pImgPrivate->tCurState, NULL);
        }
        else
        {
            pImgPrivate->tAppCallback.EventHandler (hComponent, pComponent->pApplicationPrivate,
            OMX_EventError, nErrorStatus, 0, NULL);
        }

        if(OMX_IMG_CMD_STATE_CHANGE == tCmd )
        OMX_FREE(pData);
    }

    else if(OMX_IMG_RETURN_DECODE_INFO== tCmd)
    {
        OMX_TIIMAGE_DECODE_IMAGEINFOTYPE *tDecInfo = (OMX_TIIMAGE_DECODE_IMAGEINFOTYPE*) pData;
        OMX_TIIMAGE_DECODE_IMAGEINFOTYPE *tClientInfo = &(pImgPrivate->tCompParam.tParamType.tDecodeInfoParam);
        tClientInfo->nActHeight = tDecInfo->nActHeight;
        tClientInfo->nActWidth= tDecInfo->nActWidth;
        tClientInfo->nHeight= tDecInfo->nHeight;
        tClientInfo->nWidth= tDecInfo->nWidth;
        tClientInfo->tImageFormat= tDecInfo->tImageFormat;
        tClientInfo->nPlanarFormat = tDecInfo->nPlanarFormat;
        tClientInfo->nProgressiveFormat = tDecInfo->nProgressiveFormat;
    }
    else if(OMX_IMG_CMD_SETCONFIG ==tCmd)
    {}
    else if(OMX_IMG_CMD_GETCONFIG == tCmd)
    {}
    else
    pImgPrivate->tAppCallback.EventHandler (hComponent, pComponent->pApplicationPrivate,
    OMX_EventError, nErrorStatus, 0, NULL);

    OMX_ENTRYEXITFUNC_TRACE("Exit __OMX_IMG_CoreCallback");
    return;
}

/* ========================================================================== */
/**
* @fn __OMX_IMG_SetDefaults() This function initializes the private structure
* of the component. It is called by the ComponentInit function.
*
* @param pImgPrivate Handle to the private structure.
*
* @see              OMX_IMG_ComponentInit()
*/
/* ========================================================================== */
OMX_BOOL __check_imgproc_colorformat(OMX_TICOLOR_FORMATTYPE tColor)
{
    if ((tColor != OMX_TICOLOR_YCbYCr) &&
            (tColor != OMX_TICOLOR_YUV422H) &&
            (tColor != OMX_TICOLOR_YUV422V) &&
            (tColor != OMX_TICOLOR_YUV444Planar) &&
            (tColor != OMX_TICOLOR_YUV420Planar) &&
            (tColor != OMX_TICOLOR_12bitRGB444) &&
            (tColor != OMX_TICOLOR_16bitRGB565) &&
            (tColor != OMX_TICOLOR_Monochrome))
    return OMX_TRUE;
    else
    return OMX_FALSE;
}



