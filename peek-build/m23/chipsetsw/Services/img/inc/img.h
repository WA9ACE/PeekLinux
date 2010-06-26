/* =============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file img.h
 *
 * This file defines img module interfaces
 *
 * @path  chipsetsw\services\img\inc
 *
 * @rev  0.1
 *
 */
/* ------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 31-Jan-2006 Ramesh Anandhi: Initial Release
 *!
 * ========================================================================= */

#ifndef __IMG_H_
#define __IMG_H_

#include "omx_tiimage.h"
#include "img_buffer_queue.h"
#include "common.h"
#include "baseImageEnc.h"
#include "typedefs.h"   
#include "vsi.h"       
#include "pei.h"        
#include "tools.h"     

/*==== CONSTS ===============================================================*/
#define MY_NAME          "IMG"
#define VSI_CALLER       IMG_handle,

#define hCommIMGClient   img_hCommIMGClient  /* Communication handle */

#define JPEG_JMP_TAB_LOC_BOTTOM  0x0804FDB0 

#define IMG_MAX_COMPONENTS 6
#define IMG_MAX_QUEUE_ELEMS 4

/* make the pei_create function unique */
#define pei_create       img_pei_create

/*==== TYPES =================================================================*/
typedef enum img_core_status 
{
    IMG_CORE_STS_IDLE,
    IMG_CORE_STS_RUNNING,
    IMG_CORE_STS_WAITING
} IMG_CORE_STS;

typedef struct 
{
    OMX_TIIMAGE_ENCODE_PARAMTYPE tEncode;
    OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE tEncodeInfo;
    tBaseImageEncoder *hEncoder;
    OMX_S8 tStatus;
}IMG_CORE_ENCODEPARAM;

typedef struct  
{
    OMX_TIIMAGE_DECODE_PARAMTYPE tDecode;
    OMX_TIIMAGE_DECODE_IMAGEINFOTYPE tDecodeInfo;
    OMX_PTR hDecoder;
}IMG_CORE_DECODEPARAM;

typedef union img_core_paramtype 
{

    IMG_CORE_ENCODEPARAM        tEncode;
    IMG_CORE_DECODEPARAM        tDecode;
    OMX_TIIMAGE_ROTATE_PARAMTYPE tRotate;
    OMX_TIIMAGE_RESCALE_PARAMTYPE tRescale;
    OMX_TIIMAGE_OVERLAY_PARAMTYPE tOverlay;
    OMX_TIIMAGE_EFFECT_PARAMTYPE tEffect;
    OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE tColConv;

}IMG_CORE_PARAMTYPE;

typedef enum img_buf_type
{
    IMG_INPUT,
    IMG_OUTPUT
}IMG_BUF_TYPE;  

typedef struct img_core_operation
{
    /** handler to the component instance that this operation is associated with **/
    OMX_HANDLETYPE      hComp;
    OMX_IMG_COMPTYPE tCompType;
    IMG_CORE_PARAMTYPE tParam;
    IMG_CORE_STS tCoreSts;
    T_BUF_QUEUE pInpBufQueue; 
    T_BUF_QUEUE pOutBufQueue; 
    OMX_BOOL    bInPlace;
    void (* pCoreCallback) (
            OMX_HANDLETYPE hComponent,          
            OMX_ERRORTYPE nErrorStatus,
            OMX_IMG_CMDTYPE tCmd, 
            OMX_PTR pCompData
            ); //Call back function pointer     

}IMG_CORE_OPERATIONTYPE;


/**
 * The Control Block buffer of IMG, which gathers all 'Global variables'
 * used by IMG instance.
 *
 * A structure should gathers all the 'global variables' of IMG instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_IMG_ENV_CTRL_BLK buffer is allocated when creating IMG instance and is 
 * then always refered by IMG instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
    /** handle to the task */
    T_HANDLE                    addr_id;
    IMG_CORE_OPERATIONTYPE      *apIMGComp[IMG_MAX_COMPONENTS];
} T_IMG_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_IMG_ENV_CTRL_BLK       *img_env_ctrl_blk_p;

/*==== EXPORTS ===============================================================*/
short pei_create (T_PEI_INFO **info);

#ifdef IMG_PEI_C

/* Communication handles */
T_HANDLE                 hCommIMGClient= VSI_ERROR;
T_HANDLE                 hCommIMG = VSI_ERROR;
T_HANDLE                 IMG_handle;

#else  /* IMG_PEI_C */

extern T_HANDLE          hCommIMGClient;
extern T_HANDLE          IMG_handle;

/*=====================FUNCTION DECLARATIONS ==================================*/

OMX_ERRORTYPE img_core_state_change (IMG_CORE_OPERATIONTYPE *, OMX_STATETYPE );
OMX_ERRORTYPE img_queue_buf (IMG_CORE_OPERATIONTYPE *, OMX_PTR , IMG_BUF_TYPE );
OMX_ERRORTYPE img_core_setconfig(IMG_CORE_OPERATIONTYPE *, OMX_PTR );
OMX_ERRORTYPE img_core_getconfig(IMG_CORE_OPERATIONTYPE *, OMX_PTR );
OMX_S32 img_core_running(IMG_CORE_OPERATIONTYPE *);
OMX_S32 __encoder_create(OMX_TIIMAGE_ENCODE_PARAMTYPE *, tBaseImageEncoder **);
void __convert_to_imgprocbuftype(OMX_BUFFERHEADERTYPE*, tBaseVideoFrame_JDEmz* , OMX_U32 , OMX_U32 , OMX_TICOLOR_FORMATTYPE );
void __release_buffers(IMG_CORE_OPERATIONTYPE *);
OMX_U32 __map_to_Codecformat(OMX_TICOLOR_FORMATTYPE );
OMX_TICOLOR_FORMATTYPE __map_to_TII_Format(OMX_U8 nEmzFormat);
 void __timer_init();
 OMX_U16 __timer_start();
 OMX_U16 __timer_stop();
#endif /* IMG_PEI_C */
#endif /* __IMG_H_ */

