/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file _msl_api.h
 *
 * This contains msl internal data structures and definitions.
 *
 *
 * \chipsetsw\services\msl\src
 *
 * Rev 0.1
 */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 27-May-2006 Narendran M R: Initial Release
 *!
 * ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

/* User code goes here */
/* ------compilation control switches ----------------------------------------*/
#ifndef __MSL_API_H
#define __MSL_API_H

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include "OMX_Types.h"
#include "OMX_Core.h"
#include "OMX_Image.h"
#include "OMX_camdcomponent.h"
#include "OMX_IMGComponent.h"
#include "OMX_camdcomponent.h"
#include "OMX_SSLComponent.h"
#include "omx_tiimage.h"

#ifndef WIN32
#include "typedefs.h"
#include "vsi.h"
#include "ffs/ffs_api.h"
#include "rfs/rfs_api.h"   
#include "prf/prf_api.h"    
#else
#include "vsi_adapt.h"
#endif

#include "msl_api.h"
#include"os_const.h"

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
extern T_HANDLE mm_ext_data_pool_handle;
extern MSL_GLOBALFEATURE_CONFIGTYPE tMSLGloablFeatures;

    
/****************************************************************
 * DEFINES
 ****************************************************************/    
#define MSL_SWAP(a, b)  a = a ^ b;\
                    b = a ^ b;\
                    a = a ^ b;

#define MSL_MAXFILENAME_LENGTH                          100
#define MSL_ZOOM_NONE                                   1024
#define MSL_DEFAULT_ENCQUALITY                          60
#define MSL_VGA_WIDTH                                   640
#define MSL_VGA_HEIGHT                                  480

#if (MSL_TRACE==0)
        #define MSL_ENTRYEXITFUNC_TRACE(a)              ((MSL_VOID)(0)) 
        #define MSL_DETAILFUNC_TRACE(a, b, c)           ((MSL_VOID)(0)) 
#elif (MSL_TRACE==1) 
        #define MSL_ENTRYEXITFUNC_TRACE(a)              TRACE_FUNCTION(a) 
        #define MSL_DETAILFUNC_TRACE(a, b, c)           ((MSL_VOID)(0))
#else
        #define MSL_ENTRYEXITFUNC_TRACE(a)              TRACE_FUNCTION(a)
        #define MSL_DETAILFUNC_TRACE(a, b, c)           TRACE_FUNCTION_P2(a,b,c)     
#endif    

#ifdef MSL_PROF
        #define MSL_PROFILEENTRY(a)                     prf_LogFunctionEntry((unsigned long)a)
        #define MSL_PROFILEEXIT(a)                      prf_LogFunctionExit((unsigned long)a) 
        #define MSL_PROFILESTUB(a)                      prf_LogPointOfInterest(a)
#else
        #define MSL_PROFILEENTRY(a)                     ((MSL_VOID)(0)) 
        #define MSL_PROFILEEXIT(a)                      ((MSL_VOID)(0))
        #define MSL_PROFILESTUB(a)                      ((MSL_VOID)(0))
#endif

OMX_PTR __msl_alloc(MSL_U32);
MSL_U16 __msl_free(OMX_PTR );

#define MSL_ALLOC(size)  __msl_alloc(size)
#define MSL_FREE(ptr)    __msl_free(ptr)

#define MSL_EXIT_IF(CONDITION, ERRORCODE) {\
    if ((CONDITION)) {\
        tRetVal = ERRORCODE;\
        goto EXIT;\
    }\
} // End of Macro MSL_EXIT_IF()

/**
 *  MSL2OMXColor
 *
 * @converts MSL to OMX types  */
OMX_TICOLOR_FORMATTYPE MSL2OMXColor(MSL_COLOR_FORMATTYPE tMSLColor);

/**
 *  MSL2OMXRotate
 *
 * @converts MSL to OMX types  */
OMX_TIIMAGE_ROTATETYPE MSL2OMXRotate(MSL_IMG_ROTATETYPE tMSLColor);


/**
 *  MSL2OMXOverlay
 *
 * @converts MSL to OMX types  */
OMX_TIIMAGE_OVERLAYTYPE	 MSL2OMXOverlay(MSL_OVERLAY_MODE tOverlayMode);

#endif /* __MSL_API_H */
#ifdef __cplusplus
} /* extern "C" */
#endif
