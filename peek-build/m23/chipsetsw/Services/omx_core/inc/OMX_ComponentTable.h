/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file OMX_ComponentTable.h
*
* This file contains the component table definition. 
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
#ifndef OMX_COMPONENTTABLE_H
#define OMX_COMPONENTTABLE_H

#include "OMX_Core.h"

/*-------defines ----------------------------------------------*/
#define OMX_MAX_NAME_SIZE (128)

/* ==========================================================================*/
/*
 * OMX_COMPONENTLIST Structure maintain the table for component names and 
 * function pointer for the respective component init functions. 
 * Component name in the table must be passed by the application when it calls
 * the OMX_GetHandle function.
 *
 */
/* ==========================================================================*/
typedef struct OMX_COMPONENTLIST {
    
    /* pointer to store the component name. The component name must be null 
     * terminated and follow the follwing format:
     *              OMX.<VENDOR_ID>.<COMPONENT_TYPE>.<COMPONENT_NAME>
     * where:
     *     <VENDOR_ID> - 2 or 3 letter vendor id. Example: TI, PSW
     *     <COMPONENT_TYPE> - Type of the component. Example: AUDIO, VIDEO
     *     <COMPONENT_NAME> - Name of the componet. Example: DSP.MIXER
     *
     * example: "OMX.TI.AUDIO.DSP.MIXER\0"
     */
    OMX_STRING  cComponentName;
    
    /* The exported ComponentInit function of the component */
    OMX_ERRORTYPE  (*pComponentInit)(OMX_HANDLETYPE hComponent);
    
} OMX_COMPONENTLIST;

/* external definition for the ComponentTable */
extern OMX_COMPONENTLIST OMXComponentList[];


#endif /* OMX_COMPONENTTABLE_H */
