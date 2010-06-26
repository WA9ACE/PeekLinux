/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file msl_imgcap.h
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

#ifndef MSL_IMGCAP_H
#define MSL_IMGCAP_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "msl_api.h"

/******************************************************************************
 * enum types
 ******************************************************************************/

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGCAP_STATUS
 */
/*---------------------------------------------------------------------------*/
typedef enum MSL_IMGCAP_STATUS
{
    MSL_BASE_STATUS_ENUMS(_IMGCAP_)
} MSL_IMGCAP_STATUS;


/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * Creates the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Create (MSL_HANDLE *phIMGCap);

/**
 * Initializes the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Init (MSL_HANDLE hIMGCap);

/**
 * Set parameters for the cmponent indicated byindex  in UCP
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_SetConfig(MSL_HANDLE hIMGCap, MSL_INDEXTYPES tIndex, MSL_VOID *pParam);

/**
 * Starts Viewfinder (with preview).
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Viewfinder(MSL_HANDLE hIMGCap);

/**
 * Starts Snapshot (with preview and save).
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Snapshot(MSL_HANDLE hIMGCap);

/**
 * Pauses camera
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Pause (MSL_HANDLE hIMGCap);

/**
 * Deinitializes the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Deinit (MSL_HANDLE hIMGCap);

/**
 * Destroys the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGCAP_STATUS MSL_ImgCap_Destroy (MSL_HANDLE hIMGCap);


#endif /* MSL_IMGCAP_H */
#ifdef __cplusplus
} /* extern "C" */
#endif

