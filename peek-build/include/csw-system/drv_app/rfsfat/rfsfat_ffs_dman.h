/**
 * @file	ffs_dman.h
 *
 * Header file with function prototypes of the directory
 * manager can be found. Functions for creating,
 * removing, activating and de-activating directories are
 * implemented in the corresponding code file.
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

#ifndef FFS_DMAN_H
#define FFS_DMAN_H

/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/
#define FFS_QUERY_NOT_STARTED                                                 0


/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/
#define RFSFAT_QUERY      rfsfat_env_ctrl_blk_p->ffs_globals.ffs_stQuery
#define LFN_QUERY_FLAG    rfsfat_env_ctrl_blk_p->ffs_globals.ffs_stQuery.query_lfn
#define LFN_QUERY_NAME    rfsfat_env_ctrl_blk_p->ffs_globals.ffs_stQuery.lfn_query
/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/
typedef struct
{
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  BOOL query_lfn;
  UINT8* lfn_query;
} QUERY_LIST_STRUCT;

/******************************************************************************
 *     Variable Declaration                                                   *
 ******************************************************************************/

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
UINT8 ffs_ucGetDirLocation
  (RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   DIR_DIRECTORY_LABEL_STRUCT * const pstMetDat);

BOOL ffs_bCheckIfInOpenFileList
  (UINT8 mpt_idx,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   RO_PTR_TO_RW (UINT8) pucIndexNr);



#if (FFS_UNICODE == 1)
/**************************************************/
// Unicode FFS       
/**************************************************/

UINT8 ffs_ucGetDirLocation_uc(
                    const T_WCHAR                      *pathName_p,
                    RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabLoc,
                    DIR_DIRECTORY_LABEL_STRUCT * const        pstMetDat);
/**************************************************/
// end of Unicode FFS       
/**************************************************/
#endif

#endif
