/* ============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ========================================================================== */
/**
* @file OMX_ComponentTable.c
*
* This file contains the component table. 
*
* @path  OMAPSW_SysDev\OMAPV1030\Multimedia\src\System_Core\omx\core
*
* @rev  0.2
*
*/
/* ------------------------------------------------------------------------- */
/* ==========================================================================
*!
*! Revision History
*! ===================================
*! 20-Sep-2005 pr: Initial Release
*! 06-Oct-2005 pr: Updated for Inspection findings
*!
* ========================================================================== */

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdlib.h>

/*-------program files ----------------------------------------*/
#include "OMX_ComponentTable.h"

/* Add extrnal ComponentInit function declaration here */
extern OMX_ERRORTYPE OMX_IMG_ComponentInit(OMX_HANDLETYPE hComponent);

#if OMX_CAM_STATE
extern OMX_ERRORTYPE OMX_CAM_ComponentInit (OMX_HANDLETYPE hComponent);
#endif

extern OMX_ERRORTYPE OMX_SSL_ComponentInit (OMX_HANDLETYPE hComp);


/* ==========================================================================*/
/*
 * OMX_COMPONENTLIST Structure maintain the table for component names and 
 * function pointer for the respective component init functions. 
 * Component name in the table must be passed by the application when it calls
 * the OMX_GetHandle function.
 *
 * The component name must be null terminated and follow the follwing format:
 *              OMX.<VENDOR_ID>.<COMPONENT_TYPE>.<COMPONENT_NAME>
 * 
 * where:
 *       <VENDOR_ID> - 2 or 3 letter vendor id. Example: TI, PSW etc..
 *       <COMPONENT_TYPE> - Type of the component. Example: AUDIO, VIDEO etc.
 *       <COMPONENT_NAME> - Name of the componet. Example DSP.MIXER etc..
 *
 * example: "OMX.TI.AUDIO.DSP.MIXER\0"
 *
 * Note: Last component entry of the structure need to fill with (NULL,NULL) 
 */
/* ==========================================================================*/

OMX_COMPONENTLIST OMXComponentList[] = {
    {"OMX.TI.IMAGE.IMAGING", &OMX_IMG_ComponentInit},

#if OMX_CAM_STATE		
    {"OMX.TI.IMAGE.CAM", &OMX_CAM_ComponentInit},
#endif

    {"OMX.TI.IMAGE.SSL", &OMX_SSL_ComponentInit},
    {NULL, NULL}
};
