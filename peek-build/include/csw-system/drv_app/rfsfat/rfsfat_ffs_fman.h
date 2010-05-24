/**
 * @file	ffs_fman.h
 *
 * In this file, the prototypes of functions of the
 * file manager can be found. Functions for deleting,
 * moving and attribute changing are implemented here.
 * Definitions, variables and function prototypes that are only
 * used within the interpreter block (ffs_) are stated here.
 *
 * @author	Anton van Breemen
 * @version 0.1 This file inherited from an ICT re-use project
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	01/26/2004	Anton van Breemen		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


#ifndef FFS_FMAN_H
#define FFS_FMAN_H


/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/

/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/

/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/

/******************************************************************************
 *     Variable Declaration                                                   *
 ******************************************************************************/

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
UINT8 ffs_ucGetFileLocation
  (RW_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   DIR_DIRECTORY_LABEL_STRUCT * const pstMetDat);


#if (FFS_UNICODE == 1)

/**************************************************/
// Unicode    
/**************************************************/

UINT8 ffs_ucGetFileLocation_uc(
                const T_WCHAR                      *pucPath,
                RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabLoc,
                DIR_DIRECTORY_LABEL_STRUCT * const        pstMetDat);

/**************************************************/
// end of Unicode    
/**************************************************/


#endif



#endif
