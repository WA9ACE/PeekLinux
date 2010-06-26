/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file OMX_IMG_Private.h
*
* This file contains the structure and functions for the imaging component. 
*
* @path  chipsetsw\services\img\omx_img\
*
* @rev  0.1
*
*/
/* ------------------------------------------------------------------------- */
/* ==========================================================================
*!
*! Revision History
*! ===================================
*! 15-Feb-2006 anandhi@ti.com: Initial Release

*!
* ========================================================================== */
#ifndef OMX_IMG_PRIVATE_H
#define OMX_IMG_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* OMX_Image common header file*/
#include "typedefs.h"
//#include "stddefs.h"
#include "vsi.h"
#include "os.h"
#include "frm_glob.h"

#include "OMX_IMGComponent.h"
#include "OMX_component.h"


/*******************************************************************************
* Macros
*******************************************************************************/
#define OMX_SEMHANDLE OMX_U32

#define OMX_IMG_EXIT_IF(CONDITION, ERRORCODE) {\
    if ((CONDITION)) {\
        tRetVal = ERRORCODE;\
        goto EXIT;\
    }\
} // End of Macro OMX_IMG_EXIT_IF()

/*******************************************************************************
* Structures
*******************************************************************************/
/* ==========================================================================*/
/*
* OMX_IMG_COMPTYPE structure contains the list of component types.
* 
* @param OMX_ENCODE_COMPTYPE			Encoder Component. 
*
* @param OMX_DECODE_COMPTYPE			Decoder Component
*
* @param OMX_IMG_PROC_COMPTYPE			Image Processing Component. A component that is of
*										a type greater than IMG_PROC can take any of the 
*										following values based on the setconfig calls.
*
*
* @param OMX_ROTATE_COMPTYPE			Rotation Component
*
* @param OMX_RESCALE_COMPTYPE			Rescale Component
*
* @param OMX_OVERLAY_COMPTYPE			Overlay Component
*
* @param OMX_EFFECT_COMPTYPE			Effect Component
*
* @param OMX_COLORCONVERSION_COMPTYPE	Color Conversion Component
*/
/* ==========================================================================*/
typedef enum OMX_IMG_COMPTYPE {

	OMX_ENCODE_COMPTYPE,
	OMX_DECODE_COMPTYPE,
	OMX_IMG_PROC_COMPTYPE = 0x00001000,
	OMX_ROTATE_COMPTYPE,
	OMX_RESCALE_COMPTYPE,
	OMX_OVERLAY_COMPTYPE,
	OMX_EFFECT_COMPTYPE,
	OMX_COLORCONVERSION_COMPTYPE

} OMX_IMG_COMPTYPE;

typedef	union param_types{
		OMX_TIIMAGE_ENCODE_PARAMTYPE tEncodeParam;	/**< Encoder parameters */

		OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE tEncodeInfoParam;	/**< Encoder output image parameters */

		OMX_TIIMAGE_DECODE_PARAMTYPE tDecodeParam;	/**< Decoder parameters */

		OMX_TIIMAGE_DECODE_IMAGEINFOTYPE tDecodeInfoParam; /**< Decoder output image parameters */

		OMX_TIIMAGE_ROTATE_PARAMTYPE tRotateParam;	/**< Rotation parameters */

		OMX_TIIMAGE_RESCALE_PARAMTYPE tRescaleParam;	/**< Rescaling parameters */

		OMX_TIIMAGE_OVERLAY_PARAMTYPE tOverlayParam;	/**< Overlay parameters */

		OMX_TIIMAGE_EFFECT_PARAMTYPE tEffectParam;	/**< Effects parameters */

		OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE tColorConvParam;	/**< Color Conversion parameters */

}OMX_TIIMAGE_PARAMTYPES;
/* ==========================================================================*/
/*
* OMX_IMG_COMP_PARAMTYPE structure contains the component type as well as the
* structure containing the parameters for the component. This is a part of the
* component private structure and is updated or accesed during set/get parameter 
* calls. 
*/
/* ==========================================================================*/
typedef struct OMX_IMG_COMP_PARAMTYPE
{
	OMX_IMG_COMPTYPE	tCompType;		/**< Component Type. */
	OMX_TIIMAGE_PARAMTYPES tParamType; /**< Union of the parameter structures for 
											 the different component types. Based on
											 the tCompType field, this structure is 
											 typecase to the corresponding parameter
											 structure */
}OMX_IMG_COMP_PARAMTYPE;

/* ==========================================================================*/
/*
* OMX_IMG_CMDTYPE structure contains the list of commands that are allowed
* between the IMG client and the IMG core. 
* 
* @param OMX_IMG_CMD_CORE_INIT			Initializes an instance of the core. 
*
* @param OMX_IMG_CMD_STATE_IDLE			Informs the core that the client is Idle
*
* @param OMX_IMG_CMD_STATE_EXEC			Informs the core that the client is Executing
*
* @param OMX_IMG_CMD_STATE_PAUSE		Informs the core that the client is Paused
*
* @param OMX_IMG_CMD_QUEUE_INPBUF		Adds a buffer to the input queue
*
* @param OMX_IMG_CMD_QUEUE_OUTBUF		Adds a buffer to the output queue
*
* @param OMX_IMG_CMD_ENCODE_SETCONFIG	Sets encoder configurations
*
* @param OMX_IMG_CMD_DECODE_SETCONFIG	Sets decoder configurations
*
* @param OMX_IMG_CMD_ROTATE_SETCONFIG	Sets rotate configurations
*
* @param OMX_IMG_CMD_RESCALE_SETCONFIG	Sets rescale configurations
*
* @param OMX_IMG_CMD_OVERLAY_SETCONFIG	Sets overlay configurations
*
* @param OMX_IMG_CMD_EFFECT_SETCONFIG	Sets effect configurations
*
* @param OMX_IMG_CMD_COLORCONVERSION_SETCONFIG	Sets color conversion configurations
*
* @param OMX_IMG_CMD_CORE_FREE			Frees the core instance. 
*
* @param OMX_IMG_RETURN_DECODE_INFO		Returns the decode output info from core to client
*/
/* ==========================================================================*/

typedef enum OMX_IMG_CMDTYPE {
	
	OMX_IMG_CMD_CORE_INIT,
	OMX_IMG_CMD_STATE_CHANGE,
//	OMX_IMG_CMD_STATE_IDLE,
//	OMX_IMG_CMD_STATE_EXEC,
//	OMX_IMG_CMD_STATE_PAUSE,
	OMX_IMG_CMD_QUEUE_INPBUF,
	OMX_IMG_CMD_QUEUE_OUTBUF,
	OMX_IMG_CMD_SETCONFIG,
	OMX_IMG_CMD_GETCONFIG,
	OMX_IMG_CMD_CORE_ABORT,
	OMX_IMG_CMD_CORE_FREE,
	OMX_IMG_RETURN_DECODE_INFO,
	OMX_IMG_CMD_MAX
	
} OMX_IMG_CMDTYPE;

/* ==========================================================================*/
/*
* OMX_IMG_CORE_MSGTYPE structure is used to send messages from the IMG 
* Client to the IMG core. It contains the command to be sent along with the
* parameters and the callback function for the IMG Core to respond once the 
* operation is complete. 
*/
/* ==========================================================================*/

typedef struct OMX_IMG_CORE_MSGTYPE {

	    OMX_IMG_CMDTYPE		nCmd; /** <Command to the IMG Core */
	    OMX_PTR	tCompParam; /**< Parameters for the command handling */
		OMX_COMPONENTTYPE *	pComponentHandle; /** < Component's Handle */
	    void (* pIMGCallback) (
	   		OMX_HANDLETYPE hComponent,	   	
	   		OMX_ERRORTYPE nErrorStatus,
			OMX_IMG_CMDTYPE tCmd, 
	   		OMX_PTR pCompData
	   		); /** <Call back function pointer */

}OMX_IMG_CORE_MSGTYPE;

/* ========================================================================== */
/**
* This is the struct for the private elements of the component.  These 
* parameters are initialised during ComponentInit and are updated with 
* subsequent calls from the application. The contents of this structure are
* accesible only to the component. .  
*/
/* ========================================================================== */
typedef struct OMX_IMG_PRIVATE {

   /** nVersion is the version of the OMX specification that the structure
     *  is built against.  It is the responsibility of the creator of this
     *  structure to initialize this value and every user of this structure
     *  should verify that it knows how to use the exact version of
     *  this structure found herein. */
    OMX_VERSIONTYPE				nCompVersion;

    /** Handle for use with async callbacks to application */
	OMX_CALLBACKTYPE			tAppCallback ;
	
	/** current state of the OMX component **/
	OMX_STATETYPE				tCurState ;

	/** Type of component based on the operation it performs.
	 *	This is assigned to the component with the first setparam call.
	 *	For encoder and decoder it remains unchanged. 
	 *	For image processing components it changes during a setconfig call 
	**/
	OMX_IMG_COMP_PARAMTYPE		tCompParam  ;
    
	OMX_SEMHANDLE                           hSemCaller;

    /* IMG Core Task Handle */
	T_HANDLE					pIMGCoreHandle;
    
   /** Error Status returned from the core **/
	OMX_ERRORTYPE				tCoreErrSts;

	/**Flag to check if the component is stopping **/
	OMX_BOOL					bStopping;
	

}OMX_IMG_PRIVATE;



/* 
 * Internal Command Handler 
 */
extern OMX_ERRORTYPE __OMX_IMG_HandleCommand (OMX_HANDLETYPE hComponent, 
                                            OMX_U32 newState);

/*
 * Function compose and commnicate the commands to the SSL Core.
 */
extern OMX_ERRORTYPE __OMX_IMG_SendMsgToCore(OMX_HANDLETYPE hComponent, OMX_IMG_CMDTYPE tCmd, 
										OMX_PTR pParam);

/*
 * Callback function to be called by the core.
 */
extern void __OMX_IMG_CoreCallback (OMX_HANDLETYPE hComponent, OMX_ERRORTYPE tErrorStatus, 
										OMX_IMG_CMDTYPE tCmd, OMX_PTR pData);
										
OMX_BOOL __check_imgproc_colorformat(OMX_TICOLOR_FORMATTYPE tColor);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OMX_IMG_PRIVATE_H */
