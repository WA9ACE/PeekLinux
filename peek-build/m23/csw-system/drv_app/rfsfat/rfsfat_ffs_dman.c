/**
 * @file  ffs_dman.c
 *
 *In this file, the implementation of functions of the
 *directory manager can be found. Functions for creating,
 *removing, activating and de-activating directories are
 *implemented here.
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
#include "rfs/fscore_types.h"
#include <string.h>

#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_dir.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_ffs_disk.h"
#include "rfsfat/rfsfat_ffs_dman.h"
#include "rfsfat/rfsfat_ffs_file.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfsfat_ffs_fman.h"

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

#define POS_PART_ID                                                         0

#define ASCII_VALUE_DOT                                                     '.'
#define ASCII_VALUE_COLON                                                   ':'
#define ASCII_VALUE_BACKSLASH                                              '\\'

/******************************************************************************
 *      Static-Function prototypes                                            *
 ******************************************************************************/
static UINT8 ucGetPartLocation
  (RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc);

#ifdef FFS_FEATURE_DIRECTORIES
static UINT8 ucDelDir (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabLoc);
static UINT8 ucRemoveDir (BOOL bRemoveSubDirs, RO_PTR_TO_RO (UINT8) pucPath);

static INT16 find_next
  (RO_PTR_TO_RW (FFS_SMALL_DIRECTORY_LABEL_STRUCT) pstSmallLabel);
UINT8 find_next_valid_label(DIR_LABEL_LOCATION_STRUCT* lblloc_p);
/******************************************************************************
 *      Implementation of Global Function                                     *
 ******************************************************************************/



/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDmanIsDir (RO_PTR_TO_RO (UINT8) pucPath)
/* PURPOSE      : The function checks if the given path is indicating a
 *                directory or not.
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
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucDmanIsDir entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* The function ffs_ucGetDirLocation will return FFS_NO_ERRORS in case a    */
  /* normal directory was indicated with the given path, or                   */
  /* FFS_RV_ROOTDIR_SELECTED in case the root directory was indicated.        */
  /* In both situations, the path is indicating a directory.                  */
  ucResult = ffs_ucGetDirLocation (pucPath, &stLabLoc, NULL);
  if ((ucResult != FFS_NO_ERRORS) && (ucResult != FFS_RV_ROOTDIR_SELECTED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucDmanIsDir",
             RV_TRACE_LEVEL_DEBUG_LOW);
    return ucResult;
  }

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucDmanIsDir FFS_RV_IS_DIR",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_RV_IS_DIR;
}
#endif

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDmanIsPart (RO_PTR_TO_RO (UINT8) pucPath)
/* PURPOSE      : The function checks if the given path is indicating the
 *                partition name of the partition.
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
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  ucResult = ucGetPartLocation (pucPath, &stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT Not a mountpoint", RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }
  return FFS_RV_IS_PART;
}

/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_open_dir(RO_PTR_TO_RO (UINT8) pucPath)

/* PURPOSE      : The function E_FFS_open_dir() can be used to select a
 *                couple of files and/or directories by a query. Information
 *                about the first file/directory in the directory pucPath is
 *                pointing at, that matches the query will be returned, using
 *                pstSmallLabel.  The following files can be requested using
 *                the function E_FFS_bDman_get_next(). The query result will
 *                not be ordered in any way.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a Volume Id., a location and
 *                 a filename with a file extension. In case no Volume Id. or
 *                 location is given, the 'current values' will be used.
 *               
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The basic information of the first matching directory
 *                label is copied into the buffer pstSmallLabel is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    return FFS_RV_ERR_INVALID_PARAMS;
  }


  /* Check path, receive location.                                            */
  ucResult =
  ffs_ucGetDirLocation (pucPath, &RFSFAT_QUERY.stLabLoc, &stMetDat);
  if (ucResult == FFS_RV_ROOTDIR_SELECTED)
  {
    stMetDat.uiFirstClusterNumberHigh = 0;
    stMetDat.uiFirstClusterNumber = DIR_CLUSTER_NR_ROOT_DIRECTORY;
  }
  else if (ucResult != FFS_NO_ERRORS)
  {
    return ucResult;
  }

  RFSFAT_QUERY.stLabLoc.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
                                     stMetDat.uiFirstClusterNumber;
  RFSFAT_QUERY.stLabLoc.tLblPos = DIR_OFFSET_FIRST_ENTRY;
  return FFS_NO_ERRORS;
}

/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
INT16 E_FFS_ucDmanGetNext
  (RO_PTR_TO_RW (FFS_SMALL_DIRECTORY_LABEL_STRUCT) pstSmallLabel)
/* PURPOSE      : The function E_FFS_open_dir() can be used to select a
 *                couple of files and/or directories. At most one entry for
 *                each available volume can be added. The function will also
 *                return the meta data of the first matching file.
 *                The function E_FFS_ucDmanGetNext() returns the meta data
 *                of the next valid file. The function uses the 'query list'
 *                for receiving the path information. Each time meta
 *                data is returned, the file pointer, stored in the same entry,
 *                points to the next entry of the directory table of the lowest
 *                directory of the path.
 *
 * ARGUMENTS    : pstSmallLabel:
 *                -This pointer points at the buffer in which the first
 *                 matching directory label information will be stored.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : The query must be started, using E_FFS_open_dir;
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : The function is not capable of returning a file's meta data
 *                more then once. Each time the function is called, the
 *                'next matching' meta data will be returned.
 ******************************************************************************/
{
  INT16 res = FFS_NO_ERRORS;

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pstSmallLabel == NULL)
  {
    return FFS_RV_ERR_INVALID_PARAMS;
  }

RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: E_FFS_ucDmanGetNext.....");

  /* Search for next match.                                                   */
  do
  {  
    res = find_next (pstSmallLabel);
    if( (res != FFS_NO_ERRORS) && 
        (res != FFS_RV_ERR_LAST_ENTRY) && 
        (res != FSCORE_CORRUPT_LFN) &&
        (res != FFS_RV_ERR_ENTRY_EMPTY))
    {
      //an error has occurred
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: find_next failed");
      break;
    }
  }while((res != FFS_NO_ERRORS) && (res != FFS_RV_ERR_LAST_ENTRY));
  return res;
}

/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDmanGetDone (void)
/* PURPOSE      : In case the query information becomes superfluous, the
 *                application must indicate this to the file system, by calling
 *                E_FFS_ucDmanGetDone(). This will remove the entry from the
 *                'query list' and makes it possible to start a new query for
 *                the corresponding volume.
 *
 * ARGUMENTS    : None.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: After calling the function, the entry information of the
 *                query will be removed. It becomes impossible to receive any
 *                result about the query any more.
 *                It becomes possible to start a new query for the
 *                corresponding volume.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
 FFS_STOP_IF_NOT_INITIALISED
 RFSFAT_QUERY.stLabLoc.tLblPos = FFS_QUERY_NOT_STARTED;
  
  return FFS_NO_ERRORS;
}


#ifdef FFS_FEATURE_DIRECTORIES
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucDmanMkDir
  (RO_PTR_TO_RO (UINT8) pucPath, RO_PTR_TO_RO (UINT8) pucNameAndExt)
/* PURPOSE      : This function creates a single (sub-) directory into the
 *                location indicated with pucPath. Both directory name and
 *                extension should be given in same buffer, pucPath is pointing
 *                at.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *                pucNameAndExt:
 *                -Pointer to NULL terminated string.
 *                 The string contains the name (and extension) of the new
 *                 directory, separated with a dot '.' character.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The meta data of the new directory is written in
 *                the directory table of the directory indicated with pucPath.
 *                Additionally, a new directory table is created, with two
 *                entries with meta data of the current and the upper directory.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucNameAndExt == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }
  return ffs_ucFileOrDirCreate (FALSE, pucPath, pucNameAndExt, TRUE);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDmanRmDir (RO_PTR_TO_RO (UINT8) pucPath)
/* PURPOSE      : This function removes a single (sub-) directory from the
 *                directory indicated with pucPath. The directory may not
 *                contain any subdirectories or files.
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
 * POSTCONDITION: In case the directory is the current working directory of the
 *                partition, the current working directory will be set to the root
 *                directory.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameter                                                  */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Remove directory                                                         */
  return ucRemoveDir (FALSE, pucPath);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDmanDelTree (RO_PTR_TO_RO (UINT8) pucPath)
/* PURPOSE      : This function removes a single (sub-) directory and all its
 *                contents from the directory indicated with pucPath. The
 *                directory may contain subdirectories and/or files.
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
 * PRECONDITION : Before removing a directory, be sure none of the files in
 *                this directory or subdirectories may be present in the 'open
 *                file list'!
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameter.                                                 */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }
  return ucRemoveDir (TRUE, pucPath);
}
#endif

/******************************************************************************
 *      Implementation of global functions within module.                     *
 ******************************************************************************/


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 ffs_ucGetDirLocation
  (RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   DIR_DIRECTORY_LABEL_STRUCT * const pstMetDat)
/* PURPOSE      : This function determines location and meta data of a
 *                directory. In case the path is not pointing at a directory,
 *                an error will be returned.
 *                In case pstMetDat is not NULL, the meta data of the directory
 *                will be copied into the buffer, pstMetDat is pointing at.
 *
 * ARGUMENTS    : pucPath:
 *                -partition Id. location and directory name and extension.
 *                pstLabLoc
 *                -Pointer at buffer, in which the location of the directory
 *                 label will be copied.
 *                pstMetDat
 *                -Pointer at buffer, in which the directory's directory label
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
  DIR_DIRECTORY_LABEL_STRUCT tempmetdat;
  DIR_DIRECTORY_LABEL_STRUCT* metdat_p;

  rfsfat_vpMemset((&tempmetdat), 0x00, sizeof(DIR_DIRECTORY_LABEL_STRUCT));
  if (pstMetDat == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: Cannot return pstmetdat for it is not allocated");
		//to keep the sequence going create a temp met dat wich can be used
    metdat_p = &tempmetdat;
  } else {
    metdat_p = pstMetDat;
  }

  ucResult = ffs_ucGetLabel (pucPath, pstLabLoc, metdat_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT get label failed");
    return ucResult;
  }

if ((!ATTR_CHECK_SUBDIR(metdat_p)) && ATTR_CHECK_LFN(metdat_p))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_IS_NOT_A_DIR");
    return FFS_RV_IS_NOT_A_DIR;
  }
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
BOOL ffs_bCheckIfInOpenFileList
  (UINT8 mpt_idx,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   RO_PTR_TO_RW (UINT8) pucIndexNr)
/* PURPOSE      : This function checks if a specified file or files from a
 *                specified partition are available in the open file list.
 *
 * ARGUMENTS    : mpt_idx:
 *                -partition id of partition, which must be searched for in the open
 *                 file list.
 *                pstLabLoc:
 *                -Label location of a file, which must be searched for in the
 *                 open file list.
 *                pucIndexNr:
 *                -in case not NULL, the index number of the found entry
 *                 will be stored in the buffer, pucIndexNr is pointing at.
 *
 * RETURNS      : TRUE:
 *                -file is in open file list. Indexnumber will be stored in
 *                 pucIndexNr.
 *                FALSE:
 *                -file is not in open file list.
 *
 * PRECONDITION : In case pstLabLoc != NULL, pstLabLoc->ucVol must be
 *                equal to mpt_idx.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : pstLabLoc and pucIndexNr may have the value NULL.
 ******************************************************************************/
{
  UINT8 ucCntr;

  /* Check all entries in the open file list.                               */
  for (ucCntr = 0; ucCntr < FSCORE_MAX_NR_OPEN_FSCORE_FILES; ucCntr++)
  {
    if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
      ucAccessMode != FFS_OPEN_FILE_ENTRY_NOT_USED)
    {
      if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
        stLabLoc.mpt_idx == mpt_idx)
      {
        /* Check if entry with same location is in the list.                */
        if ((pstLabLoc == NULL) ||
          ((pstLabLoc->tLblPos ==
          rfsfat_env_ctrl_blk_p->ffs_globals.
          ffs_astOpenFileList[ucCntr].stLabLoc.tLblPos) &&
           (pstLabLoc->mpt_idx ==
          rfsfat_env_ctrl_blk_p->ffs_globals.
          ffs_astOpenFileList[ucCntr].stLabLoc.mpt_idx) &&
           (pstLabLoc->uiFirstCl ==
          rfsfat_env_ctrl_blk_p->ffs_globals.
          ffs_astOpenFileList[ucCntr].stLabLoc.uiFirstCl)))
        {
          if (pucIndexNr != NULL)
          {
            *pucIndexNr = ucCntr;
          }
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
static INT16 find_next
  (RO_PTR_TO_RW (FFS_SMALL_DIRECTORY_LABEL_STRUCT) pstSmallLabel)
/* PURPOSE      : This function finds the next valid label in the DIR table
 *                when it is empty it will find the next, if the label is LFN 
 *                the LFN name will be copied into LFN_QUERY_NAME instead of Smalllabel

 * ARGUMENTS    : pstSmallLabel:
 *                Pointer to buffer, which will be used to copy matching
 *                directory label information into.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pstSmallLabel may not have the value of NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  INT16 res = FFS_NO_ERRORS;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  DIR_DIRECTORY_LABEL_STRUCT *stMetDat_p;
  UINT16 size = 0;
  UINT16 cntr;
  DIR_LFN_RET retval=NO_ERRORS;
  UINT8  nr_lfn_entries = 0;
  UINT32 start_lfn_pos=0;  

  stMetDat_p = &stMetDat;
  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: find_next.....");
  do
  {
    res = DIR_ucGetMetaData (&(RFSFAT_QUERY.stLabLoc), stMetDat_p);

	if((res == FFS_NO_ERRORS) && (stMetDat_p->ucAttribute ==FFS_ATTRIB_PART_MASK ))
     {
         		res= FFS_RV_ERR_ENTRY_EMPTY;
	 }

    if (res == FFS_NO_ERRORS)
    {
      //when the programmer didnt free the RFSFAT_LFN_FROM_LABEL buffer
      //it is done here to prevent leaking
      RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
      RFSFAT_LFN_FROM_LABEL = NULL;
      //check if SFN or LFN
      if (!ATTR_CHECK_LFN(stMetDat_p))
      {
        LFN_QUERY_FLAG = FALSE;
		RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: find_next. Making SFN as LFN ");
        ffs_vCopyMetaInSmallLabel (stMetDat_p, pstSmallLabel);

		RFSFAT_GETBUF((BYTES_IN_LFNE +2),&RFSFAT_LFN_FROM_LABEL);
		//default to 0x0000
  		rfsfat_vpMemset(RFSFAT_LFN_FROM_LABEL, 0x0000, 
                  ((UINT16)(BYTES_IN_LFNE +2)));
		/* Convert it to unicode */
        for(cntr=0;(pstSmallLabel->aucNameExt[cntr]!= EOSTR) && (cntr < FFS_MAX_SIZE_NAME_DOT_EXT);cntr++)
        	{
        	 	RFSFAT_LFN_FROM_LABEL[cntr]=(T_WCHAR)(pstSmallLabel->aucNameExt[cntr]);
        	}
		
        break;    /* go out of the while loop */


		
      } else 
		 {
        //entry is part of LFN
        LFN_QUERY_FLAG = TRUE;
        //verify if the first entry of the LFN is found
        if(RFSFAT_FIRST_LFN_ENTRY)
        {    
   
		    /* store the number of lfn entries */
		  nr_lfn_entries = (DIR_META_DATA.aucName[0] ^ FIRST_POS_MARKER);  

		  /* store the starting position of lfn entry */
		  start_lfn_pos = RFSFAT_QUERY.stLabLoc.tLblPos;
          res = DIR_read_lfn_from_label(&(RFSFAT_QUERY.stLabLoc), &retval);
          if(res != FFS_NO_ERRORS)
          {
			  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
            return res;
          }
          switch(retval)
          {
            case MEM_ERROR:
            {
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: not enough mem to store LFN");
              return RFSFAT_MEMORY_ERR;
            }
            case COPY_ERROR:
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: copying the LFN into buf failed");
              return DIR_RV_INVALID_PARAM;
            case NOT_A_VALID_LFN_LABEL:
            case WRONG_CRC:
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);   
              RFSFAT_LFN_FROM_LABEL = NULL;
              //don't stop searching just signal the potential flaw
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: the lfn label found is not part of a lfn");
              find_next_valid_label(&(RFSFAT_QUERY.stLabLoc));
              return FSCORE_CORRUPT_LFN;
            case NO_ERRORS:
              size = (rfsfat_strlen_uint16(RFSFAT_LFN_FROM_LABEL)) + 1;
			  if(LFN_QUERY_NAME != NULL)
			  	{
			  	   RFSFAT_FREE_BUF(LFN_QUERY_NAME);
			  	}
              RFSFAT_GETBUF(size+1, &LFN_QUERY_NAME);
			  memset(LFN_QUERY_NAME,0x00,size);
              //copy found string
              for(cntr = 0; cntr < size; cntr++)
              {
                LFN_QUERY_NAME[cntr] = (UINT8)RFSFAT_LFN_FROM_LABEL[cntr];
				if((LFN_QUERY_NAME[cntr] == EOSTR) &&(RFSFAT_LFN_FROM_LABEL[cntr]!= EOSTR))
					{
					   /* reached the Null character at the middle */
					   /* so replace with unkown character,Underscore */
					   LFN_QUERY_NAME[cntr] ='_'; 
					}
              }
#if 0		  
			  /* Dont free this need it for readdir_uc */	
              //free org buffer
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
#endif			  
			  /* place the RFSFAT_QUERY.stLabLoc.tLblPos to the last entry of LFN */
			  RFSFAT_QUERY.stLabLoc.tLblPos = start_lfn_pos + (DIR_SIZE_META_ALL *(nr_lfn_entries-1));

 			  /* set the res to FFS_RV_ERR_ENTRY_EMPTY */
			/*  res =FFS_RV_ERR_ENTRY_EMPTY;   */
	
            break;   /*    go out of the Switch statement  */
            
            default :
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: find next label failed");
              return retval;
          }
        } else {
          //not the first lfn label so skip it
          //look for previous label --> go back twice since default this function
          //will go forward once. This reults in one step back!
#if 0          
        	/* ALERT: This might lead to infinite loop, so skip this*/
          if(RFSFAT_QUERY.stLabLoc.tLblPos >= (2 * DIR_SIZE_META_ALL))
          {
            RFSFAT_QUERY.stLabLoc.tLblPos -= (2 * DIR_SIZE_META_ALL);
            break;
          }
#else		 
			res=FFS_RV_ERR_ENTRY_EMPTY;  /* read the next entry */
#endif
        	}
      }
    } else if (res != FFS_RV_ERR_ENTRY_EMPTY) {
      //probably last entry found
      return res;
    }
    /* Point at next entry, and try this                                      */
    /* ALERT: Dont miss NEXT_LABEL; otherwise it leads to infite loop */
    NEXT_LABEL(RFSFAT_QUERY.stLabLoc.tLblPos);
  }while(res == FFS_RV_ERR_ENTRY_EMPTY);
  //point to next label

  if(res != FFS_RV_ERR_ENTRY_EMPTY)
  {
    // only one time has the label to be increased
    NEXT_LABEL(RFSFAT_QUERY.stLabLoc.tLblPos);  
  }
  return FFS_NO_ERRORS;
}


/******************************************************************************
 *      Implementation of Local (= static) Function                           *
 ******************************************************************************/
#ifdef FFS_FEATURE_DIRECTORIES
/******Copyright by ICT Embedded BV All rights reserved ***********************/
static UINT8
ucRemoveDir (BOOL bRemoveSubDirs, RO_PTR_TO_RO (UINT8) pucPath)
/* PURPOSE      : This function checks input parameters and removes the
 *                directory and its contents. In case bRemoveSubDirs is TRUE,
 *                even the subdirectories and its subdirectories etc. will be
 *                removed.
 *
 * ARGUMENTS    : bRemoveSubDirs
 *                -In case bRemoveSubDirs is TRUE, the directory and all
 *                 sub(sub)(etc)directories will be removed. Otherwise, it's
 *                 only possible to remove the directory in case the directory
 *                 is empty.
 *                pucPath:
 *                -partition Id. location and name/extension of the directory that
 *                 must be removed.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT16 uiNrChars;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_LABEL_LOCATION_STRUCT stLabLocTmp;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters                                                 */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Determine location of directory.                                         */
  ucResult = ffs_ucGetDirLocation (pucPath, &stLabLoc, &stMetDat);
  if (ucResult == FFS_RV_ROOTDIR_SELECTED)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_REMOVING_NOT_ALLOWED");
    return FFS_RV_ERR_REMOVING_NOT_ALLOWED;
  }
  else if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT ffs_ucGetDirLocation failed");
    return ucResult;
  }

  /* The directories '.' and '..' may not be moved. So check name.            */
  /* path name ending with '\.' or '\..' are forbidden. Besides, strings      */
  /* that end with ':.' or ':..' and complete strings "." and ".." are not    */
  /* allowed.                                                                 */

  uiNrChars = rfsfat_uiStrlen (pucPath);

  if ((uiNrChars > 0) && (pucPath[uiNrChars - 1] == ASCII_VALUE_DOT))
  {
    if (((uiNrChars == 1) && (pucPath[uiNrChars - 1] == ASCII_VALUE_DOT)) ||
      ((pucPath[uiNrChars - 2] == ASCII_VALUE_DOT) ||
       (pucPath[uiNrChars - 2] == ASCII_VALUE_COLON) ||
       (pucPath[uiNrChars - 2] == ASCII_VALUE_BACKSLASH)))
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_REMOVING_NOT_ALLOWED");
      return FFS_RV_ERR_REMOVING_NOT_ALLOWED;
    }
  }

  if (!bRemoveSubDirs)
  {
    stLabLocTmp.tLblPos = DIR_OFFSET_FIRST_ENTRY;
	  stLabLocTmp.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
	                           stMetDat.uiFirstClusterNumber;
    stLabLocTmp.mpt_idx = stLabLoc.mpt_idx;

    /* Check if directory is empty.                                           */
    ucResult = DIR_ucIsEmpty (&stLabLocTmp);
    if (ucResult != DIR_RV_DIR_IS_EMPTY)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT DIR_ucIsEmpty failed");
      return ucResult;
    }
  }

  /* Remove directory                                                         */
  ucResult = ucDelDir (&stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT ucDelDir failed");
    return ucResult;
  }
  return DIR_ucDeleteLabel (&stLabLoc);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
static UINT8
ucDelDir (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabLoc)
/* PURPOSE      : This function deletes the indicated directory, and all its
 *                contents (even subdirectories, subsubdirectories etc.)
 *
 *                This function is recursive!
 *
 *
 * ARGUMENTS    : pstLabLoc:
 *                -points at the location of the directory's meta data.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pstLabLoc may not be NULL, and must point at a location of a
 *                directory. None of the files in the directory or its
 *                subdirectories may be placed in the open file list (if open
 *                files were found, they will be closed!).
 *                In case the directory, or one of its subdirectories was the
 *                current working directory of the partition, the current
 *                working directory will be set to the root directory of the
 *                partition.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT8 ucIndexNr;
  DIR_LABEL_LOCATION_STRUCT stLabLocDirTable;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  DIR_DIRECTORY_LABEL_STRUCT *stMetDat_p;

  stMetDat_p = &stMetDat;
  /* Get meta data of directory, to determine the first cluster number of the */
  /* directory table.                                                         */
  ucResult = DIR_ucGetMetaData (pstLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT DIR_ucGetMetaData failed");
    return ucResult;
  }

  /* Check if directory is read-only. If so, it cannot be removed.            */
  if ((stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK) != 0)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_FILE_READ_ONLY");
    return FFS_RV_ERR_FILE_READ_ONLY;
  }

  stLabLocDirTable.mpt_idx = pstLabLoc->mpt_idx;
  stLabLocDirTable.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
                                stMetDat.uiFirstClusterNumber;
  stLabLocDirTable.tLblPos = DIR_OFFSET_FIRST_REAL_ENTRY;

  /* Remove all entries in this directory table.                              */
  ucResult = DIR_ucGetMetaData (&stLabLocDirTable, &stMetDat);
  if ((ucResult != FFS_NO_ERRORS) && (ucResult != FFS_RV_ERR_LAST_ENTRY) &&
    (ucResult != FFS_RV_ERR_ENTRY_EMPTY))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT DIR_ucGetMetaData failed");
    return ucResult;
  }

  while (ucResult != FFS_RV_ERR_LAST_ENTRY)
  {
    if (ucResult != FFS_RV_ERR_ENTRY_EMPTY)
    {
      if (ATTR_CHECK_SUBDIR(stMetDat_p))
      {
        /* The meta data is used by a directory, so remove all entries in     */
        /* this directory.                                                    */
        ucResult = ucDelDir (&stLabLocDirTable);
        if (ucResult != FFS_NO_ERRORS)
        {
          RFSFAT_SEND_TRACE_ERROR("RFSFAT reentrant ucDelDir failed");
          return ucResult;
        }
      }
      else
      {
        /* Check if file is in open-file-list.                                */
        if (ffs_bCheckIfInOpenFileList (stLabLocDirTable.mpt_idx,
                        &stLabLocDirTable, &ucIndexNr))
        {
          /* File is in open file list!!!!                                    */
          return FFS_RV_FILES_IN_OPEN_FILE_LIST;
        }
      }
      /* Remove the meta data. In case it's owned by a directory, it's        */
      /* for sure this directory is empty now.                                */
      ucResult = DIR_ucDeleteLabel (&stLabLocDirTable);
      if (ucResult != FFS_NO_ERRORS)
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT DIR_ucDeleteLabel failed");
        return ucResult;
      }
    }
    stLabLocDirTable.tLblPos += DIR_SIZE_META_ALL;

    ucResult = DIR_ucGetMetaData (&stLabLocDirTable, &stMetDat);
    if ((ucResult != FFS_NO_ERRORS) && (ucResult != FFS_RV_ERR_LAST_ENTRY)
      && (ucResult != FFS_RV_ERR_ENTRY_EMPTY))
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT DIR_ucGetMetaData failed");
      return ucResult;
    }
  }

  return FFS_NO_ERRORS;
}
#endif

/******Copyright by ICT Embedded BV All rights reserved ***********************/
static UINT8 ucGetPartLocation
  (RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc)
/* PURPOSE      : This function determines location and meta data of partition,
 *                using the given path. In case the path is not pointing at
 *                the partition label, an error will be returned.
 *
 * ARGUMENTS    : pucPath:
 *                -partition Id. location and partition name and extension.
 *                pstLabLoc
 *                -Pointer at buffer, in which the location of the partition
 *                 label will be copied.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pucPath, pstLabLoc may not have the value of
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
  DIR_DIRECTORY_LABEL_STRUCT *stMetDat_p;

  stMetDat_p = &stMetDat;

  ucResult = ffs_ucGetLabel (pucPath, pstLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT ffs_ucGetLabel failed");
    return ucResult;
  }

  if (!(ATTR_CHECK_PART(stMetDat_p)))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_IS_NOT_A_PART");
    return FFS_RV_IS_NOT_A_PART;
  }
  return FFS_NO_ERRORS;
}

//This function is called when the find next function encountered an invalid label
//It will find the next valid label. The first LFN entry or the first SFN entry will
//be returned. If the LAST label is found in the dir table it will point to that last
//label when returning. (find next) will the n discover that it is LAST. 
UINT8 find_next_valid_label(DIR_LABEL_LOCATION_STRUCT* lblloc_p)
{
  UINT8 res = FFS_NO_ERRORS;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  DIR_DIRECTORY_LABEL_STRUCT *stMetDat_p;

  stMetDat_p = &stMetDat;
  do
  {
    res = DIR_ucGetMetaData (&(RFSFAT_QUERY.stLabLoc), stMetDat_p);
    if (res == FFS_NO_ERRORS)
    {
      //check if SFN or LFN
      if (!ATTR_CHECK_LFN(stMetDat_p))
      {
        //this label is not part of LFN so it must be a SFN.
        break;
      } 
	  else if(RFSFAT_FIRST_LFN_ENTRY) {    
        //very good! this is the next valid entry!
        break;
      } 
    } else if (res != FFS_RV_ERR_ENTRY_EMPTY) {
      //probably last entry found
      break;
    }
    /* Point at next entry, and try this                                      */
    NEXT_LABEL(RFSFAT_QUERY.stLabLoc.tLblPos);
  }while(1);
  return res;
}


#if (FFS_UNICODE == 1)
/**************************************************/
// Unicode FFS       
/**************************************************/

UINT8 ffs_ucGetDirLocation_uc(
                    const T_WCHAR                      *pathName_p,
                    RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabLoc,
                    DIR_DIRECTORY_LABEL_STRUCT * const        pstMetDat)
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT tempmetdat;
  DIR_DIRECTORY_LABEL_STRUCT* metdat_p;

  if (pstMetDat == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: UC, Cannot return pstmetdat for it is not allocated");
    //to keep the sequence going create a temp met dat wich can be used
    metdat_p = &tempmetdat;
  } else {
    metdat_p = pstMetDat;
  }

  ucResult = ffs_ucGetLabel_uc (pathName_p, pstLabLoc, metdat_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE("RFSFAT get label failed, UC . ",RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }
 
  // changed to enable SUBDIR check when it is not a LFN
  if ((!ATTR_CHECK_SUBDIR(metdat_p)) || ATTR_CHECK_LFN(metdat_p))  
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_IS_NOT_A_DIR, UC");
    return FFS_RV_IS_NOT_A_DIR;
  }
  return FFS_NO_ERRORS;
}

/**************************************************/
/**************************************************/
UINT8 E_FFS_ucDmanMkDir_uc(
                UINT8                                     mp_idx,
                const T_WCHAR                      *pathName_p,
                const T_WCHAR                      *dirName_p)
{

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters) */
  if ((pathName_p == NULL) || (dirName_p == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS uc",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }
  return ffs_ucFileOrDirCreate_uc (FALSE, mp_idx, pathName_p, dirName_p, TRUE);
}




/*
 *  ucRemoveDir
 *
 * PURPOSE      : This function is the UNICODE version of ucRemoveDir
 *                            check ucRemoveDir for details
 */
static UINT8
ucRemoveDir_uc (UINT8 mp_idx,BOOL bRemoveSubDirs, RO_PTR_TO_RO (T_WCHAR) pucPath)
{
  UINT8 ucResult;
  UINT16 uiNrChars;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_LABEL_LOCATION_STRUCT stLabLocTmp;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters                                                 */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Determine location of directory.                                         */
  stLabLoc.mpt_idx = mp_idx;  //mountpoint already determined
  ucResult = ffs_ucGetFileLocation_uc (pucPath, &stLabLoc, &stMetDat);
  if (ucResult == FFS_RV_ROOTDIR_SELECTED)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_REMOVING_NOT_ALLOWED");
    return FFS_RV_ERR_REMOVING_NOT_ALLOWED;
  }
  else if ((ucResult != FFS_NO_ERRORS) && (ucResult != FFS_RV_IS_NOT_A_FILE))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT ffs_ucGetDirLocation_uc failed");
    return ucResult;
  }

  /* The directories '.' and '..' may not be moved. So check name.            */
  /* path name ending with '\.' or '\..' are forbidden. Besides, strings      */
  /* that end with ':.' or ':..' and complete strings "." and ".." are not    */
  /* allowed.                                                                 */

  uiNrChars = wstrlen (pucPath);

  if ((uiNrChars > 0) && (pucPath[uiNrChars - 1] == ASCII_VALUE_DOT))
  {
    if (((uiNrChars == 1) && (pucPath[uiNrChars - 1] == ASCII_VALUE_DOT)) ||
      ((pucPath[uiNrChars - 2] == ASCII_VALUE_DOT) ||
       (pucPath[uiNrChars - 2] == ASCII_VALUE_COLON) ||
       (pucPath[uiNrChars - 2] == ASCII_VALUE_BACKSLASH)))
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_REMOVING_NOT_ALLOWED");
      return FFS_RV_ERR_REMOVING_NOT_ALLOWED;
    }
  }

  if (!bRemoveSubDirs)
  {
    stLabLocTmp.tLblPos = DIR_OFFSET_FIRST_ENTRY;
    stLabLocTmp.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
                             stMetDat.uiFirstClusterNumber;
    stLabLocTmp.mpt_idx = stLabLoc.mpt_idx;

    /* Check if directory is empty.                                           */
    ucResult = DIR_ucIsEmpty (&stLabLocTmp);
    if (ucResult != DIR_RV_DIR_IS_EMPTY)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT DIR_ucIsEmpty failed");
      return ucResult;
    }
  }

  /* Remove directory                                                         */
  ucResult = ucDelDir (&stLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT ucDelDir failed");
    return ucResult;
  }
  return DIR_ucDeleteLabel (&stLabLoc);
}

/*
 *  E_FFS_ucDmanRmDir_uc
 *
 * PURPOSE      : This function is the UNICODE version of E_FFS_ucDmanRmDir
 *                            check E_FFS_ucDmanRmDir for details
*/
UINT8
E_FFS_ucDmanRmDir_uc (UINT8 mp_idx, RO_PTR_TO_RO (T_WCHAR) pucPath)
{

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucDmanRmDir_uc entered",
                                              RV_TRACE_LEVEL_DEBUG_LOW);
           
  FFS_STOP_IF_NOT_INITIALISED

  /* Check pointer parameter                                                  */
  if (pucPath == NULL)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT FFS_RV_ERR_INVALID_PARAMS");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Remove directory                                                         */
  return ucRemoveDir_uc (mp_idx, FALSE, pucPath);
}


UINT8 E_FFS_open_dir_uc(UINT8  mp_idx,RO_PTR_TO_RO (T_WCHAR) pucPath)

/* PURPOSE      : The function E_FFS_open_dir_uc() can be used to select a
 *                couple of files and/or directories by a query. Information
 *                about the first file/directory in the directory pucPath is
 *                pointing at, that matches the query will be returned, using
 *                pstSmallLabel.  The following files can be requested using
 *                the function E_FFS_bDman_get_next(). The query result will
 *                not be ordered in any way.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a Volume Id., a location and
 *                 a filename with a file extension. In case no Volume Id. or
 *                 location is given, the 'current values' will be used.
 *               
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The basic information of the first matching directory
 *                label is copied into the buffer pstSmallLabel is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pucPath == NULL)
  {
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check path, receive location.                                            */
  /* mount point is existing */
  RFSFAT_QUERY.stLabLoc.mpt_idx = mp_idx;
  ucResult =  ffs_ucGetDirLocation_uc (pucPath, &RFSFAT_QUERY.stLabLoc, &stMetDat);
  if (ucResult == FFS_RV_ROOTDIR_SELECTED)
  {
    stMetDat.uiFirstClusterNumberHigh = 0;
    stMetDat.uiFirstClusterNumber = DIR_CLUSTER_NR_ROOT_DIRECTORY;
  }
  else if (ucResult != FFS_NO_ERRORS)
  {
    return ucResult;
  }

  RFSFAT_QUERY.stLabLoc.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
                                     stMetDat.uiFirstClusterNumber;
  RFSFAT_QUERY.stLabLoc.tLblPos = DIR_OFFSET_FIRST_ENTRY;
  return FFS_NO_ERRORS;
}




/**************************************************/
// end of Unicode FFS       
/**************************************************/
#endif





