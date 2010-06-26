/* ========================================================================= 
 *               Texas Instruments OMAP(TM) Platform Software
 *    Copyright (c) Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *    Use of this software is controlled by the terms and conditions found
 *    in the license agreement under which this software has been supplied.
 *  ========================================================================= */
/**
 * @file MSL_ImgThmb.h
 *
 * This contains all of the types and functions specific to the
 * MSL ImgThumbnail use-case pipeline.
 *
 * 
 * Path unknown
 *
 * Rev 0.1
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MSL_IMGTHMB_H
#define MSL_IMGTHMB_H

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "msl_api.h"

/******************************************************************************
 * enum types
 ******************************************************************************/
/*---------------------------------------------------------------------------*/
/*
 * MSL_IMGTHMB_STATUS
 */
/*---------------------------------------------------------------------------*/
typedef enum MSL_IMGTHMB_STATUS
{
    MSL_BASE_STATUS_ENUMS(_IMGTHMB_)
} MSL_IMGTHMB_STATUS;


/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
/**
 * Creates the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Create (MSL_HANDLE *phIMGThmb);

/**
 * Initializes the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Init (MSL_HANDLE hIMGThmb);

/**
 * Starts ImageThumbnail 
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Generate(MSL_HANDLE hIMGThmb);

/**
 * Pauses Imageviewer
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Pause (MSL_HANDLE hIMGThmb);

/**
 * Deinitializes the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Deinit (MSL_HANDLE hIMGThmb);

/**
 * Destroys the pipeline.
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_Destroy (MSL_HANDLE hIMGThmb);

/**
 * Set Configurations for the cmponent indicated by index  in UCP
 *
 * @see MSL design documentation for more information. */
MSL_IMGTHMB_STATUS MSL_ImgThmb_SetConfigs(MSL_HANDLE hIMGThmb, MSL_INDEXTYPES tIndex, MSL_VOID *pParam);


#endif /* MSL_IMGTHMB_H */

#ifdef __cplusplus
} /* extern "C" */
#endif


