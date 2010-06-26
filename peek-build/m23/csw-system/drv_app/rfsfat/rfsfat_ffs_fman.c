/**
 * @file  ffs_fman.c
 *
 *  In this file, the implementation of functions of the
 *  file manager can be found. Functions for deleting,
 *  moving and attribute changing are implemented here.
 *
 * @author  Anton van Breemen
 * @version 0.1 This file inherited from an ICT re-use project
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  01/26/2004  Anton van Breemen   Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


/******************************************************************************
 *     Includes                                                               *
 ******************************************************************************/

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#include "gbi/gbi_api.h"
#include <string.h>
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_dir.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_ffs_disk.h"
#include "rfsfat/rfsfat_ffs_dman.h"
#include "rfsfat/rfsfat_ffs_fman.h"
#include "rfsfat/rfsfat_mem_operations.h"

/******************************************************************************
 *      Module specific Defines and enums                                     *
 ******************************************************************************/
/* Define module ID string for debugging.                                     */
#ifdef DEBUG_TRACING
#undef  DEBUG_MOD_ID_STRING
#define DEBUG_MOD_ID_STRING                                              "FFS "
#endif

/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

#define ASCII_VALUE_DOT                                                     '.'
#define ASCII_VALUE_COLON                                                   ':'
#define ASCII_VALUE_BACKSLASH                                              '\\'

/******************************************************************************
 *      Module specific Macros and Typedefs                                   *
 ******************************************************************************/
#define GET_LABEL(a,b,c) {UINT8 res; res = ffs_ucGetLabel (a, b, c); \
                          if (res != FFS_NO_ERRORS) { \
                            if (res == FFS_RV_ROOTDIR_SELECTED) { \
                              res = FFS_RV_ERR_MOVING_NOT_ALLOWED; } \
                            RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetLabel failed"); \
                            return res; }}
/******************************************************************************
 *      Definition global Variables                                           *
 ******************************************************************************/

/******************************************************************************
 *      Definition of static-Variables                                        *
 ******************************************************************************/

/******************************************************************************
 *      Static-Function prototypes                                            *
 ******************************************************************************/
#ifdef FFS_FEATURE_DIRECTORIES
static UINT8 ucCheckIfSubdirOfDir
  (DIR_DIRECTORY_LABEL_STRUCT * pstMetDatMainDir,
   DIR_LABEL_LOCATION_STRUCT * pstLabLocSubDir);
#endif
BOOL valid_rename(UINT8* pucCurrentPath,
                  BOOL*  lfn,
                  DIR_LABEL_LOCATION_STRUCT *stLabLocSrc, 
                  DIR_DIRECTORY_LABEL_STRUCT *stMetDatSrc);

BOOL valid_rename_uc (RO_PTR_TO_RO (T_WCHAR)  pucCurrentPath,
                      DIR_LABEL_LOCATION_STRUCT * stLabLocSrc, 
                      DIR_DIRECTORY_LABEL_STRUCT *stMetDatSrc);




/******************************************************************************
 *      Implementation of Global Function                                     *
 ******************************************************************************/

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFmanMv
  (RW_PTR_TO_RO (UINT8) pucCurrentPath,
   RW_PTR_TO_RO (UINT8) pucNewPath, RW_PTR_TO_RO (UINT8) pucNameAndExt)
/* PURPOSE      : The function E_FFS_ucFmanMv() renames or moves a file or a
 *                directory. The current path and name is given by
 *                pucCurrentPath, the new name with pucNewPath.
 *
 *                Both paths must be pointing at one and the same partition.
 *
 * ARGUMENTS    : pucCurrentPath:
 *                -path that can be used to locate the file or directory that
 *                 must be moved.
 *                pucNewPath:
 *                -destionation location of the file or directory.
 *                pucNameAndExt:
 *                -pointer to the new name of the file or directory. In case
 *                the pointer has the value of NULL, the original name of the
 *                file/directory will be used.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pucCurrentPath and pucNewPath may not have the value NULL.
 *                Both paths must be pointing at one and the same partition.
 *
 * POSTCONDITION: The file's meta data is removed from the current directory
 *                table and stored in the lowest directory table of the new
 *                path.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : It is impossible to move and/or rename a file in case it is
 *                opened for read and/or write operations. Besides, it is not
 *                possible to move the file or directory in case a file or
 *                directory with the same name and extention is already
 *                available at the destination path.
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT16 uiNrChars = 0;
  DIR_LABEL_LOCATION_STRUCT stLabLocSrc;
  DIR_LABEL_LOCATION_STRUCT stLabLocDst;
  DIR_LABEL_LOCATION_STRUCT tmp_stLabLocDst;
  DIR_DIRECTORY_LABEL_STRUCT stMetDatSrc;
  DIR_DIRECTORY_LABEL_STRUCT tmp_stMetDatSrc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDatDst;
  DIR_DIRECTORY_LABEL_STRUCT *stMetDatSrc_p;
  UINT16 lfncntr = 0;
  BOOL orgnamelfn = FALSE;
  BOOL rename_flg = FALSE;
  

  stMetDatSrc_p = &stMetDatSrc;
 
  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucCurrentPath == NULL) || (pucNewPath == NULL))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* The directories '.' and '..' may not be moved. So check name.            */
  /* path name ending with '\.' or '\..' are forbidden. Besides, strings      */
  /* that end with ':.' or ':..' and complete strings "." and ".." are not    */
  /* allowed.                                                                 */
  uiNrChars = rfsfat_uiStrlen (pucCurrentPath);

  if ((uiNrChars > 0) && (pucCurrentPath[uiNrChars - 1] == ASCII_VALUE_DOT))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_MOVING_NOT_ALLOWED");
    return FFS_RV_ERR_MOVING_NOT_ALLOWED;
  }
  //check if rename is valid and ifso get source location
  //stLabLocSrc will point to the 8+3 label Even when newpath is a LFN
  //stMetDatSrc will contain the 8+3 lbl info
  if(!valid_rename((UINT8*)pucCurrentPath, &orgnamelfn, &stLabLocSrc, &stMetDatSrc))
  { 
    //invalid rename action
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: trying to rename a short filename that has a valid LFN");
    return FFS_RV_ERR_MOVING_NOT_ALLOWED;
  }

  /* Check if the given source directory/file is readonly. If so, it can not  */
  /* be moved.                                                                */
  if ((stMetDatSrc.ucAttribute & FFS_ATTRIB_READONLY_MASK) != 0)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_FILE_READ_ONLY");
    return FFS_RV_ERR_FILE_READ_ONLY;
  }

  /* Check if the given source directory/file is marked as partition label. If   */
  /* so, it can not be moved.                                                 */
  if (ATTR_CHECK_PART(stMetDatSrc_p))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_IS_PART");
    return FFS_RV_IS_PART;
  }
  /*
   * Get destination location and check destination. 
   * it will point to the 8+3 label Even when newpath is a LFN
   * stMetDatDst will contain the 8+3 lbl info
   */
  ucResult = ffs_ucGetDirLocation (pucNewPath, &stLabLocDst, &stMetDatDst);
  if (ucResult != FFS_NO_ERRORS)
  {
    if (ucResult == FFS_RV_ROOTDIR_SELECTED)
    {
      stMetDatDst.uiFirstClusterNumberHigh = 0;
      stMetDatDst.uiFirstClusterNumber = DIR_CLUSTER_NR_ROOT_DIRECTORY;
    }
    else
    {
      RFSFAT_SEND_TRACE ("RFSFAT: fs_ucGetDirLocation failed",
               RV_TRACE_LEVEL_ERROR);
      return ucResult;
    }
  }
  /*
   * Check source and destination combinations.
   */

  /* Check if both paths point at one and the same partition.                    */
  if (stLabLocSrc.mpt_idx != stLabLocDst.mpt_idx)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_DIFFERENT_PART");
    return FFS_RV_ERR_DIFFERENT_PART;
  }

#ifdef FFS_FEATURE_DIRECTORIES
  /* Check if a whole directory must be moved                                 */
  if (ATTR_CHECK_SUBDIR(stMetDatSrc_p) && (!ATTR_CHECK_LFN(stMetDatSrc_p)))
  {
    /* In this situation, a directory must be moved from one directry into    */
    /* another. Of course it may not be possible to move a directory into     */
    /* one of this directory's subdirectories. For this, the following check  */
    /* is executed.                                                           */
    ucResult = ucCheckIfSubdirOfDir (&stMetDatSrc, &stLabLocDst);
    if (ucResult != FFS_RV_NO_SUBDIR_OF_DIR)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucCheckIfSubdirOfDir failed");
      return ucResult;
    }
  }
  else
#endif /* #ifdef FFS_FEATURE_DIRECTORIES                                      */
  {
    /* File must be moved.                                                    */
    /* Check if file is in open file list.                                    */
    if (ffs_bCheckIfInOpenFileList
      (stLabLocSrc.mpt_idx, &stLabLocSrc, NULL))
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_FILES_IN_OPEN_FILE_LIST");
      return FFS_RV_FILES_IN_OPEN_FILE_LIST;
    }
  }

  /* Rename file/directory if necessary.                                      */
  /* Check given any given new name.                                          */
  if (pucNameAndExt != NULL)
  {
    //rename
    rename_flg = TRUE;
  }
  if(rename_flg)
  {
    /* Create a correct name and extension for in meta data (padded with spaces). */
    //LFN mod
    if (test_file_lfn (pucNameAndExt))
    {
      //LFN
      RFSFAT_LFN_FLAG = TRUE;
      rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
      ucResult = rephr_lfn(0, pucNameAndExt, RFSFAT_LFN, &lfncntr);
      if( (ucResult == FFS_RV_ERR_INVALID_PATH) || 
          (ucResult == FFS_RV_ERR_INVALID_PARAMS) ||
          (lfncntr == 0))
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: invalid new name");
        return ucResult;
      } 
    } else {
      RFSFAT_LFN_FLAG = FALSE;
      if (ffs_ucRephraseNameAndExt (POSITION_FIRST_CHAR_IN_PATH, pucNameAndExt,&stMetDatSrc) ==
        POSITION_FIRST_CHAR_IN_PATH)
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_INVALID_NAMEEXT");
        return FFS_RV_ERR_INVALID_NAMEEXT;
      }
    }
  }
  /* Check if file or directory with same name already exists at destination. */
  stLabLocDst.uiFirstCl = ((stMetDatDst.uiFirstClusterNumberHigh<<16) | stMetDatDst.uiFirstClusterNumber);
  memcpy(&tmp_stLabLocDst,&stLabLocDst,sizeof(DIR_LABEL_LOCATION_STRUCT));
  memcpy(&tmp_stMetDatSrc,&stMetDatSrc,sizeof(DIR_DIRECTORY_LABEL_STRUCT));
  ucResult = DIR_ucGetLabel (&tmp_stLabLocDst, &tmp_stMetDatSrc);
  if (ucResult != DIR_RV_DIR_NOT_FOUND)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_FILE_ALREADY_EXISTS");
    return FFS_RV_ERR_FILE_ALREADY_EXISTS;
  }

  //if both old and new name are 8+3 use "old method"
  if((!orgnamelfn) && (!RFSFAT_LFN_FLAG))
  {
    /* Rename file/directory if necessary.                                      */
    /* Check given any given new name.                                          */
    if (rename_flg) //else orig name will be used
    {
      /* Write new name in directory label.                                     */
      ucResult = DIR_ucSetMetaData (&stLabLocSrc, &stMetDatSrc);
      if (ucResult != FFS_NO_ERRORS)
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_FILE_ALREADY_EXISTS");
        return FFS_RV_ERR_FILE_ALREADY_EXISTS;
      }
    }
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit E_FFS_ucFmanMv ");
    ucResult =  DIR_ucMove (&stLabLocSrc, &stLabLocDst);
  } else {
    ucResult =  DIR_lfn_move(rename_flg, RFSFAT_LFN_FLAG, orgnamelfn,
                        &stLabLocSrc, &stLabLocDst, &stMetDatSrc);
  }

#ifdef ICT_SPEEDUP_RFSFAT2

/* Flush the FAM caches */ 
 ucFlushFamCash(stLabLocSrc.mpt_idx);
 ucFlushFamCash(stLabLocDst.mpt_idx);


#endif

 flush_write_blk();    /* flush the write cache */
 
 return ucResult;
}


/******************************************************************************/
/* PURPOSE      : The function E_FFS_ucFmanUnlink() removes a file from the
 *                medium. The meta data of the file, and the file data will
 *                be removed.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The file's meta data and the file's data is removed from
 *                the medium.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8
E_FFS_ucFmanUnlink (RW_PTR_TO_RO (UINT8) pucPath)
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanUnlink entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Determine position of the file's directory label.                        */
  ucResult = ffs_ucGetFileLocation (pucPath, &stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetFileLocation failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Check if the file is not opened.                                         */
  if (ffs_bCheckIfInOpenFileList (stLabLoc.mpt_idx, &stLabLoc, NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_FILES_IN_OPEN_FILE_LIST",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_FILES_IN_OPEN_FILE_LIST;
  }

  /* Check if file is read-only.                                              */
  if ((stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK) != 0)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_FILE_READ_ONLY",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_FILE_READ_ONLY;
  }

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanUnlink",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return DIR_ucDeleteLabel (&stLabLoc);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFmanSetAttributes
  (RW_PTR_TO_RO (UINT8) pucPath, UINT8 ucAttributes)
/* PURPOSE      : Using the function E_FFS_ucFmanSetAttributes(), the values of
 *                the file or directories attributes read only, hidden,
 *                system, archive can be set or reset.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *                ucAttributes:
 *                -The following bits of the value of ucAttributes will be
 *                 used as follow: 0x--A--SHR, where A = archive,
 *                 S = system, H = hidden and R = read only.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The file's attributes read only, hidden, system, and archive
 *                is overwritten with the value of ucAttributes. .
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : When a file is read only, it becomes impossible to open
 *                the file in readwrite access mode..
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanSetAttributes entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if path is valid.                                                  */
  ucResult = ffs_ucGetPartAndDirTableFromPath (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetPartAndDirTableFromPath failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Check if the file is not opened.                                         */
  if (ffs_bCheckIfInOpenFileList (stLabLoc.mpt_idx, &stLabLoc, NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_FILES_IN_OPEN_FILE_LIST",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_FILES_IN_OPEN_FILE_LIST;
  }

  /* Get attribute field, and change it.                                      */
  ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }


  /* Change only the bits the user is allowed to change, other bits must      */
  /* as before.                                                               */
  stMetDat.ucAttribute |= ucAttributes;

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanSetAttributes ",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return DIR_ucSetMetaData (&stLabLoc, &stMetDat);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFmanGetAttributes
  (RW_PTR_TO_RO (UINT8) pucPath, RO_PTR_TO_RW (UINT8) pucAttributes)
/* PURPOSE      : Using the function E_FFS_ucFmanGetAttributes(), the values of
 *                the file or directories attributes read only, hidden, system,
 *                archive can be requested.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *                pucAttributes:
 *                -The following bits of the value of *ucAttributes will be
 *                 used as follow: 0x--A--SHR, where A = archive,
 *                 S = system, H = hidden and R = read only.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The file's attributes are written in the one byte buffer,
 *                pucAttributes is pointing at..
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanGetAttributes entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucAttributes == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Get location of directory label.                                         */
  ucResult = ffs_ucGetPartAndDirTableFromPath (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetPartAndDirTableFromPath failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Get directory label.                                                     */
  ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Store attribute value in given buffer, pucAttributes is pointing at.     */
  *pucAttributes = stMetDat.ucAttribute;

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanGetAttributes",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
}
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFmanGetDateTime (RW_PTR_TO_RO (UINT8) pucPath,
								 RO_PTR_TO_RW (UINT16) pucDate,RO_PTR_TO_RW (UINT16) pucTime,
								 RO_PTR_TO_RW (UINT16) pcDate,RO_PTR_TO_RW (UINT16) pcTime)
/* PURPOSE      : Using the function E_FFS_ucFmanGetDateTime retrives the 
 *                the file or directory date and time.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 * 
 *                pucDate:
 *					Date of the file/dir.
 *
 *				  pucTime:
 *					Time of the file/dir
 *
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The file's attributes are written in the one byte buffer,
 *                pucAttributes is pointing at..
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanGetAttributes entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucDate == NULL) || (pucTime == NULL) || (pcDate == NULL) || (pcTime == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Get location of directory label.                                         */
  ucResult = ffs_ucGetPartAndDirTableFromPath (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetPartAndDirTableFromPath failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Get directory label.                                                     */
  ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Store Date and time.     */
  *pucDate = stMetDat.uiLastChangedDate;
  *pucTime = stMetDat.uiLastChangedTime;
  *pcDate  = stMetDat.crtDate;
  *pcTime  = stMetDat.crtTime;

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanGetAttributes",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
}



/******************************************************************************
 *      Implementation of global functions within module.                     *
 ******************************************************************************/


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 ffs_ucGetFileLocation
  (RW_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   DIR_DIRECTORY_LABEL_STRUCT * const pstMetDat)
/* PURPOSE      : This function determines location and meta data of a file,
 *                using the given path. In case the path is not pointing at a
 *                file, an error will be returned. In case pstMetDat is not
 *                NULL, the meta data of the file will be copied into the
 *                buffer, pstMetDat is pointing at.
 *
 * ARGUMENTS    : pucPath:
 *                -partition Id. location and file name and extension.
 *                pstLabLoc
 *                -Pointer at buffer, in which the location of the directory
 *                 label will be copied.
 *                pstMetDat
 *                -Pointer at buffer, in which the file's directory label
 *                 will be copied. May have the value of NULL.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pucPath and pstLabLoc may not have the value of
 *                NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  DIR_DIRECTORY_LABEL_STRUCT *metdat_p;


  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: ffs_ucGetFileLocation entered");

  if (pstMetDat == NULL)
  {
    metdat_p = &stMetDat;
  } else {
    metdat_p = pstMetDat;
  }
  

  /* Get directory label.                                                     */
  ucResult = ffs_ucGetLabel (pucPath, pstLabLoc, metdat_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetLabel failed");
    return ucResult;
  }

  /* Check if attribute bit 'directory' nor 'partition' is set.                  */
  if (ATTR_CHECK_SUBDIR(metdat_p) || ATTR_CHECK_PART(metdat_p))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_IS_NOT_A_FILE");
    return FFS_RV_IS_NOT_A_FILE;
  }

  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucGetFileLocation ");
  return FFS_NO_ERRORS;
}

/******************************************************************************
 *      Implementation of Local (= static) Function                           *
 ******************************************************************************/
#ifdef FFS_FEATURE_DIRECTORIES
/******Copyright by ICT Embedded BV All rights reserved ***********************/
static UINT8 ucCheckIfSubdirOfDir
  (DIR_DIRECTORY_LABEL_STRUCT * pstMetDatMainDir,
   DIR_LABEL_LOCATION_STRUCT * pstLabLocSubDir)
/* PURPOSE      : This function detects if a directory is a subdirectory, a
 *                subsubdirectory etc. of a given directory.
 *
 * ARGUMENTS    : pstMetDatMainDir:
 *                -Pointer at meta data of main directory, in which a check
 *                 will be executed to see if other directory is positioned
 *                 within this directory.
 *
 *                pstLabLocSubDir:
 *                -Pointer at location structure, in which the position of
 *                 the possible directory is written.
 *
 * RETURNS      : TRUE:
 *                -The directory is a sub(subsub)directory of the main
 *                 directory.
 *                FALSE:
 *                -The directory is a sub(subsub)directory of the main
 *                 directory.
 *
 * PRECONDITION : Both pstMetDatMainDir and pstLabLocSubDir must indicate
 *                information at one and the same partition!
 *                Both pointers may not have the value NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  UINT32 clusterNmb;

  stLabLoc.uiFirstCl = pstLabLocSubDir->uiFirstCl;
  while (stLabLoc.uiFirstCl != DIR_CLUSTER_NR_ROOT_DIRECTORY)
  {
    clusterNmb = (pstMetDatMainDir->uiFirstClusterNumberHigh<<16) |
                  pstMetDatMainDir->uiFirstClusterNumber;
	  if (stLabLoc.uiFirstCl == clusterNmb)
    {
      RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_SUBDIR_OF_DIR",
               RV_TRACE_LEVEL_ERROR);
      return FFS_RV_SUBDIR_OF_DIR;
    }
    /* Locate the '..' directory                                              */
    /* The '..' directory label points at the upper directory table.          */
    stLabLoc.tLblPos = DIR_SIZE_META_ALL;
    ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
    if (ucResult != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
               RV_TRACE_LEVEL_ERROR);
      return ucResult;
    }
	  stLabLoc.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
	                        stMetDat.uiFirstClusterNumber;
  }
  return FFS_RV_NO_SUBDIR_OF_DIR;
}
#endif

BOOL valid_rename(UINT8* pucCurrentPath,
                  BOOL*  lfn,
                  DIR_LABEL_LOCATION_STRUCT * stLabLocSrc, 
                  DIR_DIRECTORY_LABEL_STRUCT *stMetDatSrc)
{
  DIR_LABEL_LOCATION_STRUCT  prevlbl;
  DIR_DIRECTORY_LABEL_STRUCT sfnlabel;
  LFN_LABEL_STRUCT prevmetdat;
  UINT8 res;
  UINT8* nameafterlastslash = NULL;
  UINT8* tmp_p = NULL;
  UINT8 lngth = 0;
  UINT8 tempstr[FFS_MAX_SIZE_NAME_DOT_EXT];

  //when name is sfn and not completely filled the rest of the string must be 
  //filled with spaced vtempstr is a help string filled with space that can be 
  //used to complete the sfn
  rfsfat_vpMemset(tempstr,FFS_ASCII_VALUE_SPACE,FFS_MAX_SIZE_NAME_DOT_EXT);

  //when a 8+3 label is to be renamed/moved check if it is not part of a LFN
  //find file or dir name to be renamed
  nameafterlastslash = (UINT8*)strrchr((const char *)pucCurrentPath, FFS_ASCII_VALUE_FWSLASH);
  if(nameafterlastslash == NULL)
  {
    //whole pucCurrentPath is fle/dir name
    nameafterlastslash = pucCurrentPath;
  } else {
    nameafterlastslash++;
    if(*nameafterlastslash == EOSTR)
    {
      // looks like this /MMCA1/DIR1/
      //remove last slash and find new last slash
      nameafterlastslash--;
      tmp_p = nameafterlastslash;
      *tmp_p = EOSTR;
      // now it looks like /MMCA1/DIR1\0
      nameafterlastslash = (UINT8*)strrchr((const char *)pucCurrentPath, FFS_ASCII_VALUE_FWSLASH);
      if(nameafterlastslash == NULL)
      {
        // it seemed to be more like this DIR1\0
        //whole pucCurrentPath is file/dir name
        nameafterlastslash = pucCurrentPath;    
      } else {
        // /MMCA1/DIR1\0 now pointing at last slash before the DIR1
        nameafterlastslash++; //point at the D  of DIR1
      }
    }
    //reattach the / to the original string
    if(tmp_p != NULL)
    {
      *tmp_p = FFS_ASCII_VALUE_FWSLASH;
    }
  }
  //check if lfn  
  if (test_file_lfn (nameafterlastslash))
  {
    //LFN --> valid
    /*
     * Get source location and check source. 
     */
    GET_LABEL(pucCurrentPath, stLabLocSrc, stMetDatSrc);
    *lfn = TRUE;
    return TRUE;
  } else {
    // 8+3!! could be illegal 
    /*
     * Get source location and check source. 
     */
    *lfn = FALSE; //the original name is NOT an LFN
    //stLabLocSrc will point to the 8+3 label Even when newpath is a LFN
    //stMetDatSrc will contain the 8+3 lbl info
    GET_LABEL(pucCurrentPath, stLabLocSrc, stMetDatSrc);
    //if 8+3 check if it is not part of LFN
    //check one label before sfn label to see if it happens to be an LFN
    //with the correct crc etc.
    /* prevlbl =  *stLabLocSrc;   */
    memcpy(&prevlbl,stLabLocSrc,sizeof(DIR_LABEL_LOCATION_STRUCT));  
    if(prevlbl.tLblPos >= DIR_SIZE_META_ALL)
    {
      prevlbl.tLblPos -= DIR_SIZE_META_ALL;
    } else {
      //already first label cannot go higher in dir table
      // So it cannot be part of LFN
      return TRUE;
    }
    res = DIR_ucGetMetaData (&prevlbl, &sfnlabel);
    if (res != FFS_NO_ERRORS)
    {
      if(res == FFS_RV_ERR_ENTRY_EMPTY)
      {
        //okay it is empty so it cannot be part of lfn
        return TRUE;
      } else {
        //if(res = FFS_RV_ERR_LAST_ENTRY) is also wrong since it cannot be the last 
        // while we are reading the one above the sfn label
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: valid rename get meta data failed");
        return FALSE;
      }
    }
    //because of alignment issues the sfn label must be converted to a lfn label
    convert_from_lfn_sfn(&prevmetdat, &sfnlabel);
    if(prevmetdat.lfn_attr == LFN_MASK)
    {
      //name afterslash must be exactly 11 chars! (for crc calculation)
      //fill up remainder with spaces
      lngth = strlen((const char*)nameafterlastslash);
      if(lngth < (DIR_SIZE_META_EXT + DIR_SIZE_META_NAME))
      {
        //the nameaftersslash string will be copied into tempstr that will be fed to
        //the crc_calculate function. We cannot change the original string ofcourse.
        strncpy((char*)tempstr, (char*)nameafterlastslash, lngth);
      }
      if(prevmetdat.lfn_crc == DIR_crc_calculate(tempstr))
      { 
        //definitely a long file name memeber
        return FALSE;
      }
    }
    //not part of an LFN so rename is valid
    return TRUE;
  }
}




/******************************************************************************/
/* PURPOSE      : The function E_FFS_ucFmanUnlink() removes a file from the
 *                medium. The meta data of the file, and the file data will
 *                be removed.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The file's meta data and the file's data is removed from
 *                the medium.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8 E_FFS_ucFmanTrunc (DIR_LABEL_LOCATION_STRUCT stLabLoc, T_FSCORE_SIZE Newsize )
{

   RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanTrunc entered", RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanTrunc",RV_TRACE_LEVEL_DEBUG_LOW);
  return DIR_ucTruncLabel (&stLabLoc,Newsize);
}


#if (FFS_UNICODE == 1)

/**************************************************/
// Unicode    
/**************************************************/

UINT8 ffs_ucGetFileLocation_uc(
                const T_WCHAR                      *pucPath,
                RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabLoc,
                DIR_DIRECTORY_LABEL_STRUCT * const        pstMetDat)
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  DIR_DIRECTORY_LABEL_STRUCT *metdat_p;


  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: ffs_ucGetFileLocation_uc entered");

  if (pstMetDat == NULL)
  {
    metdat_p = &stMetDat;
  } else {
    metdat_p = pstMetDat;
  }
  

  /* Get directory label.                                                     */
  ucResult = ffs_ucGetLabel_uc (pucPath, pstLabLoc, metdat_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetLabel failed, Uc ");
    return ucResult;
  }

  /* Check if attribute bit 'directory' nor 'partition' is set.                  */
  if (ATTR_CHECK_SUBDIR(metdat_p) || ATTR_CHECK_PART(metdat_p))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_IS_NOT_A_FILE, Uc");
    return FFS_RV_IS_NOT_A_FILE;
  }

  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucGetFileLocation, Uc");
  return FFS_NO_ERRORS;
}


UINT8 E_FFS_ucFmanUnlink_uc (UINT8 mp_idx, RO_PTR_TO_RO (T_WCHAR) pucPath)
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanUnlink entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Determine position of the file's directory label.                        */
  stLabLoc.mpt_idx = mp_idx;  //mointpoint already determined
  ucResult = ffs_ucGetFileLocation_uc (pucPath, &stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetFileLocation_uc failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Check if the file is not opened.                                         */
  if (ffs_bCheckIfInOpenFileList (stLabLoc.mpt_idx, &stLabLoc, NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_FILES_IN_OPEN_FILE_LIST",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_FILES_IN_OPEN_FILE_LIST;
  }

  /* Check if file is read-only.                                              */
  if ((stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK) != 0)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_FILE_READ_ONLY",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_FILE_READ_ONLY;
  }

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanUnlink",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return DIR_ucDeleteLabel (&stLabLoc);
}


UINT8 E_FFS_ucFmanMv_uc (UINT8 mountpoint, 
                          RO_PTR_TO_RO (T_WCHAR) oldfileName,
                          RO_PTR_TO_RO (T_WCHAR) oldfullpathname,
                          RO_PTR_TO_RO (T_WCHAR) newPathname, 
                          RO_PTR_TO_RO (T_WCHAR) newName)
/* PURPOSE      : The function E_FFS_ucFmanMv_uc() is a UNICODE variant of 
 *                E_FFS_ucFmanMv. for details see E_FFS_ucFmanMv()
 */
{
  UINT8 ucResult;
  UINT16 uiNrChars = 0;
  DIR_LABEL_LOCATION_STRUCT stLabLocSrc;
  DIR_LABEL_LOCATION_STRUCT stLabLocDst;
  DIR_DIRECTORY_LABEL_STRUCT stMetDatSrc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDatDst;
  DIR_DIRECTORY_LABEL_STRUCT *stMetDatSrc_p;

  
  stMetDatSrc_p = &stMetDatSrc;
 
  FFS_STOP_IF_NOT_INITIALISED
  
  /* Check pointer parameters)                                                */
  if ((oldfullpathname == NULL) || (newPathname == NULL) || (newName == NULL))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* The directories '.' and '..' may not be moved. So check name.            */
  /* path name ending with '\.' or '\..' are forbidden. Besides, strings      */
  /* that end with ':.' or ':..' and complete strings "." and ".." are not    */
  /* allowed.                                                                 */
  uiNrChars = wstrlen(oldfullpathname);
  if ((uiNrChars > 0) && (oldfullpathname[uiNrChars - 1] == ASCII_VALUE_DOT))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_MOVING_NOT_ALLOWED");
    return FFS_RV_ERR_MOVING_NOT_ALLOWED;
  }
  
  //check if rename is valid and ifso get source location
  //stLabLocSrc will point to the 8+3 label Even while newpath is a LFN
  //stMetDatSrc will contain the 8+3 lbl info
  stLabLocSrc.mpt_idx = mountpoint; 
#if 0  
  if(!valid_rename_uc(oldfullpathname, &stLabLocSrc, &stMetDatSrc))
  { 
    //invalid rename action
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: trying to rename a short filename that has a valid LFN");
    return FFS_RV_ERR_MOVING_NOT_ALLOWED;
  }
#endif

 ucResult= ffs_ucGetLabel_uc(oldfullpathname, &stLabLocSrc, &stMetDatSrc); 
  if (ucResult!= FFS_NO_ERRORS) 
  {
    if (ucResult== FFS_RV_ROOTDIR_SELECTED) 
    { 
      ucResult= FFS_RV_ERR_MOVING_NOT_ALLOWED;  
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: cannot rename root dir "); 
	  return ucResult;
    }
	else
		{
		 RFSFAT_SEND_TRACE_ERROR("RFSFAT: Source path not found "); 
		  return ucResult;
		}
	
  }
  

  /* Check if the given source directory/file is readonly. If so, it can not  */
  /* be moved.                                                                */
  if ((stMetDatSrc.ucAttribute & FFS_ATTRIB_READONLY_MASK) != 0)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_FILE_READ_ONLY");
    return FFS_RV_ERR_FILE_READ_ONLY;
  }

  /* Check if the given source directory/file is marked as partition label. If   */
  /* so, it can not be moved.                                                 */
  if (ATTR_CHECK_PART(stMetDatSrc_p))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_IS_PART");
    return FFS_RV_IS_PART;
  }
  /*
   * Get destination location and check destination. 
   * it will point to the 8+3 label Even when newpath is a LFN
   * stMetDatDst will contain the 8+3 lbl info
   */
  stLabLocDst.mpt_idx = mountpoint;  //mountpoint already determined
  ucResult = ffs_ucGetDirLocation_uc (newPathname, &stLabLocDst, &stMetDatDst);
  if (ucResult != FFS_NO_ERRORS)
  {
    if (ucResult == FFS_RV_ROOTDIR_SELECTED)
    {
      stMetDatDst.uiFirstClusterNumberHigh = 0;
      stMetDatDst.uiFirstClusterNumber = DIR_CLUSTER_NR_ROOT_DIRECTORY;
    }
    else
    {
      RFSFAT_SEND_TRACE ("RFSFAT: fs_ucGetDirLocation of new path location failed",
               RV_TRACE_LEVEL_ERROR);
      return ucResult;
    }
  }
  /*
   * Check source and destination combinations.
   */
#ifdef FFS_FEATURE_DIRECTORIES
  /* Check if a whole directory must be moved                                 */
  if (ATTR_CHECK_SUBDIR(stMetDatSrc_p) && (!ATTR_CHECK_LFN(stMetDatSrc_p)))
  {
    /* In this situation, a directory must be moved from one directry into    */
    /* another. Of course it may not be possible to move a directory into     */
    /* one of this directory's subdirectories. For this, the following check  */
    /* is executed.                                                           */
    ucResult = ucCheckIfSubdirOfDir (&stMetDatSrc, &stLabLocDst);
    if (ucResult != FFS_RV_NO_SUBDIR_OF_DIR)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucCheckIfSubdirOfDir failed");
      return ucResult;
    }
  }
  else
#endif /* #ifdef FFS_FEATURE_DIRECTORIES                                      */
  {
    /* File must be moved.                                                    */
    /* Check if file is in open file list.                                    */
    if (ffs_bCheckIfInOpenFileList
      (stLabLocSrc.mpt_idx, &stLabLocSrc, NULL))
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_FILES_IN_OPEN_FILE_LIST");
      return FFS_RV_FILES_IN_OPEN_FILE_LIST;
    }
  }

  /* Rename file/directory if necessary.                                      */
  /* Check given any given new name.                                          */
     
  /* Check if file or directory with same name already exists at destination. */
  stLabLocDst.uiFirstCl = ((stMetDatDst.uiFirstClusterNumberHigh<<16) |(stMetDatDst.uiFirstClusterNumber));
  ucResult = DIR_ucGetLabel_uc (newName, &stLabLocDst, &stMetDatSrc);
  if (ucResult != DIR_RV_DIR_NOT_FOUND)
  {
    RFSFAT_SEND_TRACE_WARNING("RFSFAT: FFS_RV_ERR_FILE_ALREADY_EXISTS 1" );
    return FFS_RV_ERR_FILE_ALREADY_EXISTS;
  }

  return DIR_lfn_move_uc(oldfileName , newName,&stLabLocSrc, &stLabLocDst, &stMetDatSrc);
}

BOOL valid_rename_uc (RO_PTR_TO_RO (T_WCHAR)  pucCurrentPath,
                      DIR_LABEL_LOCATION_STRUCT * stLabLocSrc, 
                      DIR_DIRECTORY_LABEL_STRUCT *stMetDatSrc)
/* PURPOSE      : This function checks if the name to be used is valid
 *                It is assumed that the UNICODE interface will NOT be used
 *                to rename a SFN name, so here will not be checked if this
 *                name is a SFN.
 */
{
  UINT8 res;
  T_WCHAR* nameafterlastslash = NULL;
  T_WCHAR* tmp_p = NULL;
    
  //find file or dir name to be renamed
  nameafterlastslash = wstrrchr(pucCurrentPath, FFS_ASCII_VALUE_FWSLASH);
  if(nameafterlastslash == NULL)
  {
    //whole pucCurrentPath is file/dir name
    nameafterlastslash = (T_WCHAR*)pucCurrentPath;
  } 
  else 
  {
    nameafterlastslash++;
    if(*nameafterlastslash == EOSTR)
    {
      // looks like this /MMCA1/DIR1/
      //remove last slash and find new last slash
      nameafterlastslash--;
      tmp_p = nameafterlastslash;
      *tmp_p = EOSTR;
      // now it looks like /MMCA1/DIR1\0
      nameafterlastslash = wstrrchr(pucCurrentPath, FFS_ASCII_VALUE_FWSLASH);
      if(nameafterlastslash == NULL)
      {
        // it seemed to be more like this DIR1\0
        //whole pucCurrentPath is file/dir name
        nameafterlastslash = (T_WCHAR*)pucCurrentPath;    
      } else {
        // /MMCA1/DIR1\0 now pointing at last slash before the DIR1
        nameafterlastslash++; //point at the D  of DIR1
      }
    }
    //reattach the / to the original string
    if(tmp_p != NULL)
    {
      *tmp_p = FFS_ASCII_VALUE_FWSLASH;
    }
  }
  res = ffs_ucGetLabel_uc(pucCurrentPath, stLabLocSrc, stMetDatSrc); 
  if (res != FFS_NO_ERRORS) 
  {
    if (res == FFS_RV_ROOTDIR_SELECTED) 
    { 
      res = FFS_RV_ERR_MOVING_NOT_ALLOWED;  
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: cannot rename root dir "); 
      return FALSE; 
    }
  }
  
  return TRUE;
}


UINT8 E_FFS_ucFmanGetAttributes_uc
  (RO_PTR_TO_RO (T_WCHAR) pucPath, RO_PTR_TO_RW (UINT8) pucAttributes, UINT8 mountpoint)
{

  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanGetAttributes_uc entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL || pucAttributes == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if path is valid.                                                  */
  stLabLoc.mpt_idx = mountpoint;
  ucResult = ffs_ucGetPartAndDirTableFromPath_uc (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetPartAndDirTableFromPath_uc failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

   /* Get attribute field, and change it.                                      */
  ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

 /* Store attribute value in given buffer, pucAttributes is pointing at.     */
  *pucAttributes = stMetDat.ucAttribute;

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanGetAttributes_uc",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;

}



UINT8 E_FFS_ucFmanGetDateTime_uc
  (RO_PTR_TO_RO (T_WCHAR) pucPath, RO_PTR_TO_RW (UINT16) pucDate,RO_PTR_TO_RW (UINT16) pucTime,
   								RO_PTR_TO_RW (UINT16) pcDate,RO_PTR_TO_RW (UINT16) pcTime,
   								UINT8 mountpoint)
{

  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanGetAttributes_uc entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucDate == NULL) || (pucTime == NULL) || (pcDate == NULL) || (pcTime == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if path is valid.                                                  */
  stLabLoc.mpt_idx = mountpoint;
  ucResult = ffs_ucGetPartAndDirTableFromPath_uc (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetPartAndDirTableFromPath_uc failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

   /* Get attribute field, and change it.                                      */
  ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

 /* Store attribute value in given buffer, pucAttributes is pointing at.     */
  *pucDate = stMetDat.uiLastChangedDate;
  *pucTime = stMetDat.uiLastChangedTime;
  *pcDate = stMetDat.crtDate;
  *pcTime = stMetDat.crtTime;

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanGetAttributes_uc",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;

}










/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFmanSetAttributes_uc
  (RO_PTR_TO_RO (T_WCHAR) pucPath, UINT8 ucAttributes, UINT8 mountpoint)
/* PURPOSE      : This is the UNICODE variant of the function
 *                E_FFS_ucFmanSetAttributes(). For more details, see 
 *                detailed description of E_FFS_ucFmanSetAttributes 
 */
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFmanSetAttributes_uc entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if path is valid.                                                  */
  stLabLoc.mpt_idx = mountpoint;
  ucResult = ffs_ucGetPartAndDirTableFromPath_uc (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetPartAndDirTableFromPath_uc failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Check if the file is not opened.                                         */
  if (ffs_bCheckIfInOpenFileList (stLabLoc.mpt_idx, &stLabLoc, NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_FILES_IN_OPEN_FILE_LIST",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_FILES_IN_OPEN_FILE_LIST;
  }

  /* Get attribute field, and change it.                                      */
  ucResult = DIR_ucGetMetaData (&stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

 
  /* Change only the bits the user is allowed to change, other bits must      */
  /* as before.                                                               */
  stMetDat.ucAttribute |= ucAttributes ;

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFmanSetAttributes ",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return DIR_ucSetMetaData (&stLabLoc, &stMetDat);
}










/**************************************************/
// end of Unicode
/**************************************************/





#endif



