/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file MSL_ImgView.h
 *
 * This contains all of the types and functions specific to the
 * MSL Imageviewer use-case pipeline.
 *
 * 
 * \chipsetsw\services\msl\inc
 *
 * Rev 0.1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSL_IMGVIEW_H
#define MSL_IMGVIEW_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "msl_api.h"

/******************************************************************************
 * enum types
 ******************************************************************************/

/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGVIEW_STATUS
 */
/*---------------------------------------------------------------------------*/
typedef enum MSL_IMGVIEW_STATUS
{
    MSL_BASE_STATUS_ENUMS(_IMGVIEW_)
} MSL_IMGVIEW_STATUS;


/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * Creates the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_Create (MSL_HANDLE *phIMGView);

/**
 * Initializes the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_Init (MSL_HANDLE hIMGView);

/**
 * Starts Imageviewer (with preview).
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_View(MSL_HANDLE hIMGView);

/**
 * Pauses Imageviewer
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_Pause (MSL_HANDLE hIMGView);

/**
 * Deinitializes the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_Deinit (MSL_HANDLE hIMGView);

/**
 * Destroys the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_Destroy (MSL_HANDLE hIMGView);

/**
 * Set Configurations for the cmponent indicated by index  in UCP
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_SetConfig (MSL_HANDLE hIMGView, MSL_INDEXTYPES tIndex, MSL_VOID *pParam);

/**
 * Get Configurations for the cmponent indicated byindex  in UCP
 *
 * @see MSL design documentation for more information. */
MSL_IMGVIEW_STATUS MSL_ImgView_GetConfig (MSL_HANDLE hIMGView, MSL_INDEXTYPES tIndex, MSL_VOID *pParam);

#endif /* MSL_IMGVIEW_H */
#ifdef __cplusplus
} /* extern "C" */
#endif


