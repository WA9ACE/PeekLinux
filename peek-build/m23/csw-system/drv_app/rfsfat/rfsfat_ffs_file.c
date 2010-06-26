 /**
 * @file  ffs_file.c
 *
 *  In this file, the implementation of functions for
 *  changing and reading file data can be found here.
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
#include "rtc/rtc_api.h"

#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"

#include <string.h>

#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_cfg.h"
#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_message.h"
#include "rfsfat/rfsfat_dir.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_clm.h"
#include "rfsfat/rfsfat_fam.h"
#include "rfsfat/rfsfat_ffs_disk.h"
#include "rfsfat/rfsfat_ffs_dman.h"
#include "rfsfat/rfsfat_ffs_file.h"
#include "rfsfat/rfsfat_ffs_fman.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfsfat/rfsfat_blk.h"


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

#define NO_FREE_ENTRY                                                      0xFF
#define INITIAL_VALUE_FILE_POINTER                                         0x00



/******************************************************************************/
/* PURPOSE      : This function creates a file at a specified location. For
 *                this, a directory label in the location's directory table
 *                will be added (no cluster for the file's data will be
 *                allocated). In case the directory is not given, the current
 *                location will be used. In case no partition Id. is given, the
 *                current partition will be used.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *                pucNameAndExt:
 *                -Pointer at a NULL terminated string, containing the file
 *                 name and extension, separated with a dot ('.').
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : A file can not be created in case a file or directory with
 *                the same name and extension is already available in the
 *                directory.
 *
 * POSTCONDITION: The meta data of the file is written in the location's
 *                directory table, indicated with pucPath.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8
E_FFS_ucFileCreate (RO_PTR_TO_RO (UINT8) pucPath,
          RO_PTR_TO_RO (UINT8) pucNameAndExt, BOOL flag_create)
{

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileCreate entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucNameAndExt == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }
  return ffs_ucFileOrDirCreate (TRUE, pucPath, pucNameAndExt, flag_create);
}

/******************************************************************************/
/* PURPOSE      : This function opens a file. After opening a file, read or
 *                write operations to the file's data area can directly take
 *                place by only using a handle to a specific file. During the
 *                open operation, an entry in the 'open file list' will be
 *                created.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *                ucAccessMode:
 *                -A file can be opened for reading only (FSS_OPEN_MODE_READ)
 *                 or for reading and writing (FSS_OPEN_MODE_READWRITE).
 *                pucHandle:
 *                -Pointer to the buffer, used for storing the handle.
 *
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : In case a file is opened in FSS_OPEN_MODE_READWRITE, any
 *                other application can not open the file for reading nor
 *                writing.
 *
 * POSTCONDITION: The file, indicated by pucPath, is accessible using the
 *                returned handle.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8
E_FFS_ucFileOpen (RO_PTR_TO_RO (UINT8) pucPath,
          UINT8 ucAccessMode,
          UINT16 flag, RO_PTR_TO_RW (UINT8) pucHandle)
{
  UINT8 ucCntr;
  UINT8 ucResult;
  UINT8 ucIndexFreeEntry;

  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  OPEN_FILE_ENTRY_STRUCT *pstFileEntry;

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileOpen entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucHandle == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if valid access mode.                                              */
  if ((ucAccessMode != FFS_OPEN_MODE_READWRITE) &&
    (ucAccessMode != FFS_OPEN_MODE_READ))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_ACCESS_MODE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_ACCESS_MODE;
  }

  ucResult = ffs_ucGetFileLocation (pucPath, &stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT ffs_ucGetFileLocation failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Check if file is read only, and user is trying to open for writing.      */
  if ((ucAccessMode == FFS_OPEN_MODE_READWRITE) &&
    (stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_FILE_READ_ONLY",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_FILE_READ_ONLY;
  }
  /* Check if file is read only, and user is trying to open for truncate (writing)*/
  if ((flag == FSCORE_O_TRUNC) && (stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT cannot truncate FFS_RV_ERR_FILE_READ_ONLY ");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Assume, no entries are empty in the open file list.                      */
  ucIndexFreeEntry = NO_FREE_ENTRY;

  for (ucCntr = 0; ucCntr < FSCORE_MAX_NR_OPEN_FSCORE_FILES; ucCntr++)
  {
    if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
      ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED)
    {
      /* An free entry in the open file list found.                           */
      ucIndexFreeEntry = ucCntr;
    }
    else
    if ((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
       stLabLoc.mpt_idx == stLabLoc.mpt_idx)
      && (rfsfat_env_ctrl_blk_p->ffs_globals.
        ffs_astOpenFileList[ucCntr].stLabLoc.uiFirstCl ==
        stLabLoc.uiFirstCl)
      && (rfsfat_env_ctrl_blk_p->ffs_globals.
        ffs_astOpenFileList[ucCntr].stLabLoc.tLblPos ==
        stLabLoc.tLblPos))
    {
      /* File allready in open file list.                                     */
      if ((ucAccessMode == FFS_OPEN_MODE_READWRITE) ||
        (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
         ucAccessMode == FFS_OPEN_MODE_READWRITE))
      {
        RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_FILE_ALLREADY_OPEN",
                 RV_TRACE_LEVEL_ERROR);
        return FFS_RV_ERR_FILE_ALLREADY_OPEN;
      }
    }

  }

  /* It's ok to open the file, let's see if there is a free entry in the list. */

  if (ucIndexFreeEntry == NO_FREE_ENTRY)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_TO_MUCH_OPEN_FILES",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_TO_MUCH_OPEN_FILES;
  }

  pstFileEntry = &rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucIndexFreeEntry];

 pstFileEntry->stLabLoc.mpt_idx   = stLabLoc.mpt_idx;
 pstFileEntry->stLabLoc.uiFirstCl = stLabLoc.uiFirstCl;
 pstFileEntry->stLabLoc.tLblPos   = stLabLoc.tLblPos;

 pstFileEntry->uiFirstClusterNumber = ((stMetDat.uiFirstClusterNumberHigh<<16) | (stMetDat.uiFirstClusterNumber));
 pstFileEntry->tFileSize = (FFS_FILE_LENGTH_TYPE) stMetDat.ulFileSize;
 pstFileEntry->ucAccessMode = ucAccessMode;

 pstFileEntry->uiClusterNumber_L 	=  pstFileEntry->uiFirstClusterNumber;
 pstFileEntry->uiClusterNumber_H 	=  pstFileEntry->uiFirstClusterNumber;
 pstFileEntry->tFilePointer_H 		= 0;
 pstFileEntry->tFilePointer_L 		= 0;

  
  if ((flag & FSCORE_O_APPEND) == FSCORE_O_APPEND)
  {
   pstFileEntry->tFilePointer = pstFileEntry->tFileSize;
  }
  else
  {
   	 pstFileEntry->tFilePointer = INITIAL_VALUE_FILE_POINTER;
  	}	
  
  
  if ((flag & FSCORE_O_TRUNC) == FSCORE_O_TRUNC)
  {
    //delete cluster chain in Fat Table
    if(stMetDat.ulFileSize != DEFAULT_FILE_SIZE)
    {
      ucResult = FAM_ucDelLinkedList (stLabLoc.mpt_idx, pstFileEntry->uiFirstClusterNumber);
      if ((ucResult != FFS_NO_ERRORS) && (ucResult != FAM_RET_LAST_CLUSTER))
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: error deleting linked list for Truncate");
        return ucResult;
      }
    }
    //update file label with new filesize and start cluster
    stMetDat.uiFirstClusterNumber = DEFAULT_FILE_FIRST_CLUSTER;
    stMetDat.uiFirstClusterNumberHigh = DEFAULT_FILE_FIRST_CLUSTER;
    stMetDat.ulFileSize = DEFAULT_FILE_SIZE;
    
    CPY_LBL_TO_LBL(DIR_META_DATA_P, &stMetDat);
    ucResult = ucSetMetaData (&stLabLoc, (META_TAG_CLUSTER | META_TAG_SIZE));
    if (ucResult != FFS_NO_ERRORS) 
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: updating file label for Truncate failed");
      return ucResult;
    }

    //update openfile list
    pstFileEntry->uiFirstClusterNumber = DEFAULT_FILE_FIRST_CLUSTER;
    pstFileEntry->tFilePointer = INITIAL_VALUE_FILE_POINTER;
    pstFileEntry->tFileSize = DEFAULT_FILE_SIZE;
  }
  /* Return handle.                                                           */
  *pucHandle = ucIndexFreeEntry;

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileOpen ",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
}


/******************************************************************************/
/* PURPOSE      : Using E_FFS_ucFileLseek, the file pointer can be moved up,
 *                without reading the data. In this way, it is possible to read
 *                from or write to any place within the file's data area. When
 *                the file is opened in FSS_OPEN_MODE_READWRITE, the file
 *                pointer can be set one position after the position of the
 *                last byte data. In this way, it is possible to extend the file.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.  Using
 *                 the handle read and write operations can take place.
 *                tFilePointer
 *                -When the file is opened using access mode FSS_OPEN_MODE_READ,
 *                 the value of tFilePointer must be smaller than the file's
 *                 size. Otherwise, the value can be smaller or equal to the
 *                 file's size.
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
UINT8
E_FFS_ucFileLSeek (UINT8 ucHandle, FFS_FILE_LENGTH_TYPE tFilePointer)
{

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileLSeek entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check if handle is valid.                                                */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
     ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_OFFSET",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }

  /* The file pointer can be set at byte 1 of the file data, up to and        */
  /* including the last byte of the file data + 1!                            */
  /* Point at first byte data -> tFilePointer = 0,                            */
  /* point at last byte data -> tFilePointer = (file data size - 1),          */
  /* point at last + 1 byte data -> tFilePointer = file data size.            */

  if (tFilePointer > rfsfat_env_ctrl_blk_p->ffs_globals.
    ffs_astOpenFileList[ucHandle].tFileSize)
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_OFFSET",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_OFFSET;
  }

  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
  tFilePointer = tFilePointer;

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileLSeek ",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
}




/******************************************************************************/
/* PURPOSE      : The function E_FFS_ucFileRead() reads a number of bytes from
 *                a file, and copies it into the buffer, pucBuffer is pointing
 *                at.
 *                The first byte, the read operation starts reading, is
 *                indicated by the file pointer. After reading the data, the
 *                file pointer will be incremented with tNrOfBytes number of
 *                bytes.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.  Using
 *                 the handle read and write operations can take place.
 *                ptNrOfBytes
 *                -Pointer to value that represents the number of bytes to
 *                 read from file. After reading, it contains the number of
 *                 bytes really read.
 *                pucBuffer
 *                -Pointer to buffer which the data from the file must be
 *                 written into.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : The file must be opened before reading from the file is
 *                possible.
 *                The size of the buffer, pucBuffer is pointing at, must be
 *                at least the size indicated by the value, ptNrOfBytes is
 *                pointing at.
 *
 * POSTCONDITION: At most 'ptNrOfBytes' number of bytes data is copied from
 *                the file into the buffer, pucBuffer is pointing at.
 *                'ptNrOfBytes' indicates the actual number of bytes copied.
 *                The copy process is started from the point, the file's
 *                file pointer is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8
E_FFS_ucFileRead (UINT8 ucHandle,
          RO_PTR_TO_RW (FFS_FILE_LENGTH_TYPE) ptNrOfBytes,
          RO_PTR_TO_RW (UINT8) pucBuffer)
{
  UINT8 ucResult;
  FFS_FILE_LENGTH_TYPE 		tMaxNrBytes;
  CLM_READ_WRITE_STRUCT 	stRead;
  UINT32 					clu_num;
  FFS_FILE_LENGTH_TYPE 		tFilePointer=0;
 

#ifdef  ICT_SPEEDUP_RFSFAT1
  UINT16                    uiClusterSize=0; 
  FFS_FILE_LENGTH_TYPE		clum_tFilePointer=0;
#endif
  

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileRead entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((ptNrOfBytes == NULL) || (pucBuffer == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if valid handle was given.                                         */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].ucAccessMode == 
    FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_HANDLE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }

  /* It might be possible that the current file pointer is pointing at the    */
  /* position of (the last byte data + 1)!!!!                                 */

  /* Determine maximum number of bytes to read.                               */
  if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer >= 
  		rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFileSize)
  {
    /* File pointer is pointing outside the file data, so no data can be read. */
    tMaxNrBytes = 0;
  }
  else
  {
    /* Only read file data.                                                   */
    tMaxNrBytes = (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFileSize -
           rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer);
  }

  if ((*ptNrOfBytes) > tMaxNrBytes)
  {
    *ptNrOfBytes = tMaxNrBytes;
  }

 
  /* Read file's data.                                                        */
#ifdef  ICT_SPEEDUP_RFSFAT1

  stRead.mpt_idx   = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx;
  stRead.pucBuffer = pucBuffer;
  stRead.tNrBytes  = (*ptNrOfBytes);
  tFilePointer	   = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer;  	
  
  if(tFilePointer >= rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H)    /* The file pointer is at Higher */
  	{
  	   stRead.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H;
	   stRead.tOffset = (tFilePointer - rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H);
  	}
  else
  	{
  	   if(tFilePointer >= rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L)   /* The file pointer is at Lower */
  	    {
  	   		stRead.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L;
	   		stRead.tOffset = (tFilePointer - rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L);
 		}
		else
		{
		    stRead.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;
			stRead.tOffset 			= tFilePointer;
		
		}
  	}


  uiClusterSize = CLM_ucDetermineClusterSize( stRead.uiFirstClusterNr, stRead.mpt_idx);

   if(uiClusterSize == 0)
  	{
  	   /* panic */
	   RFSFAT_SEND_TRACE_ERROR("RFSFAT: E_FFS_ucFileRead , FATAL error cluster size is zero ");
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileRead , mount point Index  = ",stRead.mpt_idx );
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileRead , ucSectorPerCluster = ",MMA_BOOT_SECTOR[stRead.mpt_idx].ucSectorPerCluster);
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileRead , uiBytesPerSector = ",MMA_BOOT_SECTOR[stRead.mpt_idx].uiBytesPerSector); 
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileRead , uiMaxRootEntries = ",MMA_BOOT_SECTOR[stRead.mpt_idx].uiMaxRootEntries);  
	   return CLM_RET_UNDEF_ERROR;
  	}

	
  	

  ucResult = CLM_ucReadWrite_FILE (&stRead, CLM_READ_MODE, uiClusterSize,&clu_num);
  
#else

  stRead.mpt_idx = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx;
  stRead.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;
  stRead.pucBuffer = pucBuffer;
  stRead.tNrBytes = (*ptNrOfBytes);
  stRead.tOffset = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer;

  ucResult = CLM_ucReadWrite (&stRead, CLM_READ_MODE);
#endif  /* ICT_SPEEDUP_RFSFAT1  */

  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT CLM_ucReadWrite failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Update file pointer.                                                     */
  tFilePointer += stRead.tNrBytes;
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer = tFilePointer;


#ifdef  ICT_SPEEDUP_RFSFAT1

  clum_tFilePointer = (((tFilePointer-1)/uiClusterSize)*uiClusterSize);
 if(tFilePointer >  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H)    /* The file pointer is at Higher */
  	{
      if(((tFilePointer-rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L) > RFSFAT_SEEK_MAX_WINDOW_SIZE)
	  	 || (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L ==0))
      	{
              /* re-org the window */     	
			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L = 
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H;
			  
	 		   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = 
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H;			   
      	  
      	}
	
  	   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H = clu_num;
	   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H = clum_tFilePointer;
  	}
  else
  	{

     if(tFilePointer <  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L)
     	{

		   if((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H - tFilePointer) > RFSFAT_SEEK_MAX_WINDOW_SIZE)	
		   	{

			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H =
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L;
			   
	 		   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H =
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L;			   
		   	}
       	   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L = clu_num;
		   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = clum_tFilePointer;			   
	     
     	}
	 else
	 	{
           /* tFilePointer is in between Low & Mid  */

		   /* check are is the size in window or not */
		   if((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
		   	tFilePointer_H 	 - rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
		   	tFilePointer_L) 	 > RFSFAT_SEEK_MAX_WINDOW_SIZE   )
		   	{

			   if(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L ==0 )
			   	{
  	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L = clu_num;
	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = clum_tFilePointer;
				  
			   	}
			   else
			   	{
				   /* Window is too big; reduce it  */
  	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H = clu_num;
	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H = clum_tFilePointer;
			   	}  
			  
		   	}
		    else
		    	{
		    	   ; /* we are inside window ; dont do anything */
		    	}
		   	   
			
	 	}
   	
  	}

#endif
  


  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileRead ",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
}


/******************************************************************************/
/* PURPOSE      : The function E_FFS_ucFileWrite() writes a number of bytes
 *                from the buffer, pucBuffer is pointing at, in the file,
 *                indicated by the handle. The write operation starts at the
 *                position of the file's file pointer.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.  Using
 *                 the handle read and write operations can take place.
 *                ptNrOfBytes
 *                -Pointer to value that represents the number of bytes to
 *                 write to file.
 *                pucBuffer
 *                -Pointer to buffer which contains the data that must be
 *                 written into the file.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : The file must be opened before writing, using access mode
 *                FSS_OPEN_MODE_READWRITE.
 *
 * POSTCONDITION: The data in pucBuffer is copied into the file. The file
 *                pointer is increased with tNrOfBytes number of bytes.
 *                The file's data area might be extended, it's meta data
 *                (file length field) is up to date.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8
E_FFS_ucFileWrite (UINT8 ucHandle,
           RW_PTR_TO_RO (FFS_FILE_LENGTH_TYPE) ptNrOfBytes,
           RO_PTR_TO_RW (UINT8) pucBuffer)
{
	
  UINT8 ucResult,rwResult;

  #ifndef ICT_SPEEDUP_RFSFAT3
  UINT32 clusterNmb;
  UINT16* time_p;
  UINT16* date_p;

  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  #endif
  FFS_FILE_LENGTH_TYPE 		tNrBytes;
  CLM_READ_WRITE_STRUCT 	stWrite;
  UINT32 					clu_num;
  FFS_FILE_LENGTH_TYPE 		tFilePointer=0;


#ifdef  ICT_SPEEDUP_RFSFAT1
  UINT16                    uiClusterSize=0;  
  FFS_FILE_LENGTH_TYPE		clum_tFilePointer=0;
#endif

  

  

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFileWrite entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((ptNrOfBytes == NULL) || (pucBuffer == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if valid handle was given.                                         */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
     ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_HANDLE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }

  /* Check if file was opened for writing.                                    */
  if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
    ucAccessMode != FFS_OPEN_MODE_READWRITE)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_WRITING_IN_READMODE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_WRITING_IN_READMODE;
  }

  /* Check if not too much data to write in file.                             */
  tNrBytes = (FFS_MAX_VALUE_FILE_LENGTH_TYPE -
        rfsfat_env_ctrl_blk_p->ffs_globals.
        ffs_astOpenFileList[ucHandle].tFilePointer);
  if ((*ptNrOfBytes) > tNrBytes)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_TOO_MUCH_FILE_DATA",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_TOO_MUCH_FILE_DATA;
  }

  /* New created files do not have an area for writing their data into.       */
  /* The new created file will have a file data size of 0 bytes. To insert    */
  /* file data, a new cluster chain must be reserved, the files meta data     */
  /* must obtain this first cluster number.                                   */
  if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFileSize == 0)
  {
    /* Create copy of first cluster number of file data.                      */
    /* Create a chain near to the directory label of the file.                */
    rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber = 
    rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.uiFirstCl;
	
    ucResult = FAM_ucCreateNewChain (
	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx,
    &rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber);
    if (ucResult != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucCreateNewChain failed",
               RV_TRACE_LEVEL_ERROR);
      return ucResult;
    }


#ifdef  ICT_SPEEDUP_RFSFAT1

rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H =
rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;


rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L =
rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;

rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H = 0;
rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = 0;


#endif


	
  }

  tFilePointer	   = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer;  	

  /* Write data to file.                                                      */
#ifdef  ICT_SPEEDUP_RFSFAT1

  stWrite.mpt_idx   = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx;
  stWrite.pucBuffer = pucBuffer;
  stWrite.tNrBytes  = (*ptNrOfBytes);
  
  if(tFilePointer >= rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H)    /* The file pointer is at Higher */
  	{
  	   stWrite.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H;
	   stWrite.tOffset = tFilePointer - rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H;
  	}
  else
  	{
  	   if(tFilePointer >= rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L)   /* The file pointer is at Lower */
  	    {
  	   		stWrite.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L;
	   		stWrite.tOffset = tFilePointer - rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L;
 		}
		else
		{
		    stWrite.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;
			stWrite.tOffset 			= tFilePointer;
		
		}
  	}


  uiClusterSize = CLM_ucDetermineClusterSize( stWrite.uiFirstClusterNr, stWrite.mpt_idx);

   if(uiClusterSize == 0)
  	{
  	   /* panic */
	   RFSFAT_SEND_TRACE_ERROR("RFSFAT: E_FFS_ucFileWrite , FATAL error cluster size is zero ");
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileWrite , mount point Index  = ",stWrite.mpt_idx );
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileWrite , ucSectorPerCluster = ",MMA_BOOT_SECTOR[stWrite.mpt_idx].ucSectorPerCluster);
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileWrite , uiBytesPerSector = ",MMA_BOOT_SECTOR[stWrite.mpt_idx].uiBytesPerSector); 
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: E_FFS_ucFileWrite , uiMaxRootEntries = ",MMA_BOOT_SECTOR[stWrite.mpt_idx].uiMaxRootEntries);  
	   return CLM_RET_UNDEF_ERROR;
  	}


  rwResult = CLM_ucReadWrite_FILE (&stWrite, CLM_WRITE_MODE,uiClusterSize,&clu_num);
#else 
  stWrite.mpt_idx = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx;
  stWrite.uiFirstClusterNr = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;
  stWrite.pucBuffer = pucBuffer;
  stWrite.tNrBytes = (*ptNrOfBytes);
  stWrite.tOffset = tFilePointer;


  rwResult = CLM_ucReadWrite (&stWrite, CLM_WRITE_MODE);
#endif  /* ICT_SPEEDUP_RFSFAT1  */

  if(rwResult != FFS_NO_ERRORS )
  	{

	  if(rwResult != CLM_RET_NO_SPACE)
	  	{
	  	   RFSFAT_SEND_TRACE ("RFSFAT: CLM_ucReadWriteSpeed1 failed",  RV_TRACE_LEVEL_ERROR);
		   return rwResult;
	  	}

	  RFSFAT_SEND_TRACE ("RFSFAT: CLM_ucReadWriteSpeed1 failed .... CLM_RET_NO_SPACE",  RV_TRACE_LEVEL_ERROR);
	 
  	}


 tFilePointer += stWrite.tNrBytes;
 
#ifdef  ICT_SPEEDUP_RFSFAT1

  clum_tFilePointer = (((tFilePointer-1)/uiClusterSize)*uiClusterSize);
 if(tFilePointer >  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H)    /* The file pointer is at Higher */
  	{
      if(((tFilePointer-rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L) > RFSFAT_SEEK_MAX_WINDOW_SIZE)
	  	 || (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L ==0))
      	{
              /* re-org the window */     	
			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L = 
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H;
			  
	 		   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = 
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H;			   
      	  
      	}
	
  	   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H = clu_num;
	   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H = clum_tFilePointer;
  	}
  else
  	{

     if(tFilePointer <  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L)
     	{

		   if((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H - tFilePointer) > RFSFAT_SEEK_MAX_WINDOW_SIZE)	
		   	{

			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H =
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L;
			   
	 		   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H =
			   	rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L;			   
		   	}
       	   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L = clu_num;
		   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = clum_tFilePointer;			   
	     
     	}
	 else
	 	{
           /* tFilePointer is in between Low & Mid  */

		   /* check are is the size in window or not */
		   if((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
		   	tFilePointer_H 	 - rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
		   	tFilePointer_L) 	 > RFSFAT_SEEK_MAX_WINDOW_SIZE   )
		   	{

			   if(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L ==0 )
			   	{
  	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_L = clu_num;
	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_L = clum_tFilePointer;
				  
			   	}
			   else
			   	{
				   /* Window is too big; reduce it  */
  	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiClusterNumber_H = clu_num;
	   			   rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer_H = clum_tFilePointer;
			   	}  
			  
		   	}
		    else
		    	{
		    	   ; /* we are inside window ; dont do anything */
		    	}
		   	   
			
	 	}
   	
  	}

#endif


  
  
#ifdef ICT_SPEEDUP_RFSFAT3

  /* dont update the file meta data */

/* Calculate the position of the last new written byte + 1;                 */
  /* Update file size in open file list, if necessary.                        */
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer = tFilePointer;

  /* Calculate (new) file size.                                               */
  if ((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
    tFilePointer) >(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
    tFileSize))
  {
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
    tFileSize) = (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer);
 }
  
  
#else
  /* The meta data of this file must be updated (Date and time).              */
  ucResult = DIR_ucGetMetaData (&(rfsfat_env_ctrl_blk_p->ffs_globals.
                  ffs_astOpenFileList[ucHandle].stLabLoc),
                &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Calculate the position of the last new written byte + 1;                 */
  /* Update file size in open file list, if necessary.                        */
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer = tFilePointer;

  /* Calculate (new) file size.                                               */
  if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
    tFilePointer > stMetDat.ulFileSize)
  {
    stMetDat.ulFileSize = rfsfat_env_ctrl_blk_p->ffs_globals.
    ffs_astOpenFileList[ucHandle].tFilePointer;
  }
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
  tFileSize = stMetDat.ulFileSize;

  /* In case the file didn't had any file data, a new chain was created.      */
  /* Now it's time to write the first cluster number of this chain into       */
  /* the file's meta data.                                                    */
  clusterNmb = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;
  stMetDat.uiFirstClusterNumberHigh = clusterNmb >> 16;
  stMetDat.uiFirstClusterNumber = clusterNmb;
	

  time_p = (UINT16*) (&(stMetDat.uiLastChangedTime));
  date_p = (UINT16*) (&(stMetDat.uiLastChangedDate));
  /* Update date and time info.                                               */
  ucResult = ffs_ucGetTimeAndDate (time_p, date_p);
  if (ucResult != FFS_NO_ERRORS)
  {
   RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucGetTimeAndDate failed",
             RV_TRACE_LEVEL_ERROR);
   /* return ucResult;  , we can ignore this */
  }

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileWrite ",
           RV_TRACE_LEVEL_DEBUG_LOW);

  /* Update directory label.                                                  */
  ucResult= DIR_ucSetMetaData (&(rfsfat_env_ctrl_blk_p->ffs_globals.
                ffs_astOpenFileList[ucHandle].stLabLoc),
              &stMetDat);
  
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucSetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }
  
 #endif
 
 /* return the read write status */
 return rwResult;

 
}



/******************************************************************************/
/* E_FFS_ucFileClose
 * PURPOSE      : When opening a file, a handle to this file is created and an
 *                entry is added to the 'open file list'. The handle can be
 *                used to read from or write to the file's data area. The number
 *                of simultaneously opened files is limited. After a read or
 *                write operation is done, the file must be closed. This will
 *                remove the entry from the 'open file list'.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.  Using
 *                 the handle read and write operations can take place.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: After closing the file, it becomes inaccessible for read or
 *                write operations. The file must be re-opened to make this
 *                possible again.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8 E_FFS_ucFileClose (UINT8 ucHandle)
{

#ifdef ICT_SPEEDUP_RFSFAT3
  UINT32 clusterNmb;
  UINT8 ucResult;
  UINT16* time_p;
  UINT16* date_p;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
#endif
  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFileClose entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED

 /* Check if valid handle was given.                                         */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].ucAccessMode == 
    FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_HANDLE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }


 #ifdef ICT_SPEEDUP_RFSFAT2
    /* flush the FAM cache */
  ucFlushFamCash((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx));
#endif //ICT_SPEEDUP_RFSFAT2


#ifdef ICT_SPEEDUP_RFSFAT3

 /* The meta data of this file must be updated (Date and time).              */
  ucResult = DIR_ucGetMetaData (&(rfsfat_env_ctrl_blk_p->ffs_globals.
                  ffs_astOpenFileList[ucHandle].stLabLoc),
                &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucGetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }


  /* update the file size */
  if((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFileSize) > stMetDat.ulFileSize)
  	{
      stMetDat.ulFileSize=(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFileSize);
  	} 

  /* In case the file didn't had any file data, a new chain was created.      */
  /* Now it's time to write the first cluster number of this chain into       */
  /* the file's meta data.                                                    */
  clusterNmb = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].uiFirstClusterNumber;
  stMetDat.uiFirstClusterNumberHigh = clusterNmb >> 16;
  stMetDat.uiFirstClusterNumber = clusterNmb;
	

  time_p = (UINT16*) (&(stMetDat.uiLastChangedTime));
  date_p = (UINT16*) (&(stMetDat.uiLastChangedDate));
  /* Update date and time info.                                               */
  ucResult = ffs_ucGetTimeAndDate (time_p, date_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    return ucResult;
  }

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileWrite ",
           RV_TRACE_LEVEL_DEBUG_LOW);

  /* Update directory label.                                                  */
  ucResult= DIR_ucSetMetaData (&(rfsfat_env_ctrl_blk_p->ffs_globals.
                ffs_astOpenFileList[ucHandle].stLabLoc),
              &stMetDat);

  if (ucResult != FFS_NO_ERRORS)
  {
    return ucResult;
  }

#endif

  /* Mark entry empty, in case valid handle was given.                        */
rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].ucAccessMode = FFS_OPEN_FILE_ENTRY_NOT_USED;

RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFileClose ", RV_TRACE_LEVEL_DEBUG_LOW);
return FFS_NO_ERRORS;
}


/******************************************************************************/
/* E_FFS_ucFileSync
 * PURPOSE      : Flush newly written file data 
 * 				  When opening a file, a handle to this file is created and an
 *                entry is added to the 'open file list'. The handle can be
 *                used to read from or write to the file's data area. The number
 *                of simultaneously opened files is limited. After a read or
 *                write operation is done, the file must be closed. This will
 *                remove the entry from the 'open file list'.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.  Using
 *                 the handle read and write operations can take place.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: After closing the file, it becomes inaccessible for read or
 *                write operations. The file must be re-opened to make this
 *                possible again.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8 E_FFS_ucFileSync(UINT8 ucHandle)
{

 /* Check if valid handle was given.                                         */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].ucAccessMode == 
    FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_HANDLE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }


 #ifdef ICT_SPEEDUP_RFSFAT2
    /* flush the FAM cache */
  ucFlushFamCash((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc.mpt_idx));
 #endif //ICT_SPEEDUP_RFSFAT2

 
  flush_write_blk();

   return FFS_NO_ERRORS;


}


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFileAllocate
  (UINT8 ucHandle, FFS_FILE_LENGTH_TYPE tNewFileLength)
/* PURPOSE      : The function E_FFS_ucFileAllocate() allocates a number of
 *                bytes for a specified file. In fact, the file's cluster
 *                chain will be extended, so the total number of bytes data the
 *                file can contain is guaranteed equal to the value of
 *                tNewFileLength.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.  Using
 *                 the handle read and write operations can take place.
 *                tNewFileLength
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : The file indicated with ucHandle, must be opened in
 *                read/write mode (FFS_OPEN_MODE_READWRITE)..
 *
 * POSTCONDITION: The chain length of a file's cluster chain is extended, so
 *                it contains at least tNewFileLength number of bytes data.
 *                The value of the file's meta data field 'file length' is not
 *                changed.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFileAllocate entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Determine if handle is valid.                                            */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
     ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_HANDLE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }

  /* Check if file was opened for writing.                                    */
  if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
    ucAccessMode != FFS_OPEN_MODE_READWRITE)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_WRITE_NOT_ALLOWED",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_WRITE_NOT_ALLOWED;
  }

  return FAM_ucAssureClusterLength (rfsfat_env_ctrl_blk_p->ffs_globals.
                  ffs_astOpenFileList[ucHandle].stLabLoc.
                  mpt_idx,
                  rfsfat_env_ctrl_blk_p->ffs_globals.
                  ffs_astOpenFileList[ucHandle].
                  uiFirstClusterNumber, tNewFileLength);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucFileFstat
  (UINT8 ucHandle, RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstLabel)
/* PURPOSE      : The function E_FFS_ucFileFstat() will deliver information
 *                about an open file using the structure pstLabel.
 *
 * ARGUMENTS    : ucHandle:
 *                -Value indicating an entry in the 'open file list'.
 *                pstLabel:
 *                -Pointer to buffer where the file's meta data will be stored.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : The file must be opened before its information can be given..
 *
 * POSTCONDITION: The files meta data is stored in the buffer, pstLabel
 *                is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucFileFstat entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if (pstLabel == NULL)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if valid handle was given.                                         */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
    (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].
     ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_HANDLE",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_HANDLE;
  }

  ucResult = DIR_ucGetMetaData (&(rfsfat_env_ctrl_blk_p->ffs_globals.
                  ffs_astOpenFileList[ucHandle].stLabLoc),
                pstLabel);

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucFileFstat",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return ucResult;
}
/******************************************************************************/
/* PURPOSE      : The function E_FFS_ucFileTrunc() truncates a file, 
 *				to the size specified
 *
 * ARGUMENTS    : ucHandle:
 *		           -Value indicating an entry in the 'open file list'.  Using
 *           		      the handle read and write operations can take place.
 *
 *		             ptNrOfBytes
 *           		     -Pointer to value that represents the new size to truncate
 *		              the file. 
 *
 * RETURNS      : FFS_NO_ERRORS:
 *	                -The operation was ended successfully.
 *	                [Others]:
 *	                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : The file must be opened before reading from the file is
 *		                possible.
 *
 * 		               The size of the buffer, pucBuffer is pointing at, must be
 *		                at least the size indicated by the value, ptNrOfBytes is
 *		                pointing at.
 *
 * POSTCONDITION: The file is truncated to the new size
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
UINT8 E_FFS_ucFileTrunc (UINT8 ucHandle,T_FSCORE_SIZE Newsize)
{
  UINT8 ucResult;

  DIR_LABEL_LOCATION_STRUCT stLabLoc;	/* Location of directory label of file */

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileTrunc entered", RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED

  
  /* Check if valid handle was given. */
  if ((ucHandle >= FSCORE_MAX_NR_OPEN_FSCORE_FILES) ||
  	(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED))
  {
	RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_HANDLE", RV_TRACE_LEVEL_ERROR);
	return FFS_RV_ERR_INVALID_HANDLE;
  }

  /* file mode should not be read only */
  if(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].ucAccessMode == FFS_OPEN_MODE_READ)
  	{
  	   /* return invalid */
	   return FFS_RV_ERR_FILE_READ_ONLY;
  	}

  /* current file position should be less than new file position */
  if(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFilePointer > Newsize )
  	{
  	 RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS", RV_TRACE_LEVEL_ERROR);

	 /* New size is invalid */
	 return FFS_RV_ERR_INVALID_PARAMS;
  	}

  stLabLoc = rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].stLabLoc;

  /* Truncate file's size */
  ucResult = E_FFS_ucFmanTrunc (stLabLoc, Newsize);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT CLM_ucTrunc failed", RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }
  
  /* Update  file size */
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucHandle].tFileSize = Newsize;


  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileTrunc ", RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
  
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 ffs_ucFileOrDirCreate
  (BOOL bCreateFile,
   RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RO (UINT8) pucNameAndExt, BOOL flag_create)
/* PURPOSE      : Function creates a file (bCreateFile = TRUE) or directory
 *                (bCreateFile = FALSE) in the directory, indicated with pucPath,
 *                with the name and extension of pucNameAndExt.
 *
 * ARGUMENTS    : bCreateFile:
 *                -Create a file (TRUE) or a new directory (FALSE).
 *                pucPath:
 *                -partition Id. and location in which the new file or directory
 *                 must be created. Pointer at a NULL terminated string.
 *                pucNameAndExt:
 *                -Pointer at a NULL terminated string. Name and extension of
 *                 the new file/directory separated with a dot.
 *                -flag_create: if TRUE create file when not existing
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : Both pucPath and pucNameAndExt must point at NULL terminated
 *                strings. The value of NULL is not allowed.
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
  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  UINT16 lfncntr = 0;


 rfsfat_vpMemset((&stMetDat), 0x00, sizeof(DIR_DIRECTORY_LABEL_STRUCT));
 rfsfat_vpMemset((&stLabLoc), 0x00, sizeof(DIR_LABEL_LOCATION_STRUCT));
	
  /* Get location of directory (and check if this is a directory).            */
  ucResult = ffs_ucGetDirLocation (pucPath, &stLabLoc, &stMetDat);
  if (ucResult == FFS_RV_ROOTDIR_SELECTED)
  {
	  stMetDat.uiFirstClusterNumberHigh = 0;
    stMetDat.uiFirstClusterNumber = DIR_CLUSTER_NR_ROOT_DIRECTORY;
  }
  else if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetDirLocation failed");
    return ucResult;
  }
  /* Point location at the directory table.                                   */
  stLabLoc.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
                        stMetDat.uiFirstClusterNumber;

  /*Create a correct name and extension for in meta data (padded with spaces). */
  //LFN mod
  if (test_file_lfn (pucNameAndExt))
  {
    //LFN
    RFSFAT_LFN_FLAG = TRUE;
    rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
    ucResult = rephr_lfn(0, pucNameAndExt, RFSFAT_LFN, &lfncntr);
    if( (ucResult == FFS_RV_ERR_INVALID_PARAMS) || 
		    (ucResult == FFS_RV_ERR_INVALID_PATH) ||
        (lfncntr == 0))
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: invalid creation name");
      return ucResult;
    } 
  } else {
    RFSFAT_LFN_FLAG = FALSE;
    if (ffs_ucRephraseNameAndExt (POSITION_FIRST_CHAR_IN_PATH, pucNameAndExt, &stMetDat) ==
      POSITION_FIRST_CHAR_IN_PATH)
    {
  	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_INVALID_NAMEEXT");
      return FFS_RV_ERR_INVALID_NAMEEXT;
    }
  }

  ucResult = DIR_ucGetLabel (&stLabLoc, &stMetDat);

  if(ucResult == FFS_NO_ERRORS )
  {
    /* File or directory with this name allready exists.                      */
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_FILE_ALREADY_EXISTS");
    return FFS_RV_ERR_FILE_ALREADY_EXISTS;
  }
 else
 	{
  		if (ucResult != DIR_RV_DIR_NOT_FOUND) 	  //Verify that the new name does not already exist
  		{
      		RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_ucGetLabel failed, ffs_ucFileOrDirCreate");
      		return ucResult;
 		}
 	}	
  

  if (!flag_create)
  {

    //file doesn't exist and may not be created
    return DIR_RV_DIR_NOT_FOUND;
  }
   
  /* Create file or directory.                                                */
  if (bCreateFile)
  {
    /* Create file.                                                           */
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucFileOrDirCreate making file");
    return DIR_ucCreateFile (&stLabLoc, &stMetDat);
  }
  else
  {
#ifdef FFS_FEATURE_DIRECTORIES
    /* Create directory.                                                      */
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucFileOrDirCreate making directory");
    ucResult= DIR_ucCreateDir (&stLabLoc, &stMetDat);


#ifdef ICT_SPEEDUP_RFSFAT2
    ucFlushFamCash(stLabLoc.mpt_idx);
#endif //ICT_SPEEDUP_RFSFAT2

	
#else
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_FEATURE_UNAVAILABLE");
    return FFS_RV_FEATURE_UNAVAILABLE;
#endif

   return ucResult;
  }
}


#if (FFS_UNICODE == 1)

/**************************************************/
//                              
// Unicode FFS       
//                              
/**************************************************/

UINT8 ffs_ucFileOrDirCreate_uc(BOOL                   bCreateFile,
                               UINT8                  mp_idx,
                               const  T_WCHAR  *pathName_p,
                               const  T_WCHAR  *fileOrDirName_p,
                               BOOL                   flag_create)
{
  UINT8                       ucResult;
  DIR_DIRECTORY_LABEL_STRUCT  stMetDat;
  DIR_LABEL_LOCATION_STRUCT   stLabLoc;


  
 rfsfat_vpMemset((&stMetDat), 0x00, sizeof(DIR_DIRECTORY_LABEL_STRUCT));
 rfsfat_vpMemset((&stLabLoc), 0x00, sizeof(DIR_LABEL_LOCATION_STRUCT));

  stLabLoc.mpt_idx = mp_idx;  
  
  /* Get location of directory (and check if this is a directory). */
  ucResult = ffs_ucGetDirLocation_uc (pathName_p, &stLabLoc, &stMetDat);
  if (ucResult == FFS_RV_ROOTDIR_SELECTED)
  {
    stMetDat.uiFirstClusterNumberHigh = 0;
    stMetDat.uiFirstClusterNumber = DIR_CLUSTER_NR_ROOT_DIRECTORY;
  }
  else if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ffs_ucGetDirLocation failed, Uc");
    return ucResult;
  }
  /* Point location at the directory table. */
  stLabLoc.uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
                        stMetDat.uiFirstClusterNumber;


  RFSFAT_LFN_FLAG = TRUE; //Enforce LFN

  //Verify that the new name does not already exist
 ucResult = DIR_ucGetLabel_uc (fileOrDirName_p, &stLabLoc, &stMetDat);
 
 if(ucResult == FFS_NO_ERRORS )
  {
    /* File or directory with this name allready exists.                      */
    RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: FFS_RV_ERR_FILE_ALREADY_EXISTS, Uc");
    return FFS_RV_ERR_FILE_ALREADY_EXISTS;
  }
 else
 	{
  		if (ucResult != DIR_RV_DIR_NOT_FOUND) 	  //Verify that the new name does not already exist
  		{
      		RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_ucGetLabel_uc failed, Uc");
      		return ucResult;
 		}
 	}	

  	

  if (!flag_create)
  {
    return DIR_RV_DIR_NOT_FOUND; //file doesn't exist and may not be created
  }
  
 

  /* Create file or directory.                                                */
  if (bCreateFile)
  {
    /* Create file.                                                           */
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucFileOrDirCreate making file, Uc");
    ucResult = DIR_ucCreateFile_uc (&stLabLoc, &stMetDat, fileOrDirName_p);
  }
  else
  {
    /* Create directory.                                                      */
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucFileOrDirCreate making directory, Uc");
    ucResult = DIR_ucCreateDir_uc (&stLabLoc, &stMetDat, fileOrDirName_p);
  }

  return  ucResult;
}




/**************************************************/
/**************************************************/

UINT8 E_FFS_ucFileCreate_uc ( UINT8                 mp_idx,
                              const T_WCHAR  *pathName_p,
                              const T_WCHAR  *fileName_p,
                              BOOL                  flag_create)
{

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileCreate_uc entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED

  /* Check pointer parameters) */
  if ((pathName_p == NULL) || (fileName_p == NULL) || (fileName_p[0] == EOSTR))
  {
    RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileCreate_uc parameter error",RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }
  return ffs_ucFileOrDirCreate_uc (TRUE, mp_idx, pathName_p, fileName_p, flag_create);
}


/**************************************************/
/**************************************************/
UINT8 E_FFS_ucFileOpen_uc ( UINT8                 mp_idx,
                            const T_WCHAR  *pucPath,
                            UINT8                 ucAccessMode,
                            UINT16                flag,
                            RO_PTR_TO_RW (UINT8)  pucHandle)
{
  UINT8 ucCntr;
  UINT8 ucResult;
  UINT8 ucIndexFreeEntry;

  DIR_LABEL_LOCATION_STRUCT stLabLoc;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;

  OPEN_FILE_ENTRY_STRUCT *pstFileEntry;

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucFileOpen_uc entered",
           RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters)                                                */
  if ((pucPath == NULL) || (pucHandle == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_PARAMS, Uc",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Check if valid access mode.                                              */
  if ((ucAccessMode != FFS_OPEN_MODE_READWRITE) &&
    (ucAccessMode != FFS_OPEN_MODE_READ))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_INVALID_ACCESS_MODE, Uc",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_ACCESS_MODE;
  }

  stLabLoc.mpt_idx = mp_idx;  //mointpoint already determined
  ucResult = ffs_ucGetFileLocation_uc (pucPath, &stLabLoc, &stMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT ffs_ucGetFileLocation failed, Uc",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /* Check if file is read only, and user is trying to open for writing.      */
  if ((ucAccessMode == FFS_OPEN_MODE_READWRITE) &&
    (stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK))
  {
    RFSFAT_SEND_TRACE ("RFSFAT FFS_RV_ERR_FILE_READ_ONLY, Uc",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_FILE_READ_ONLY;
  }
  /* Check if file is read only, and user is trying to open for truncate (writing)*/
  if ((flag == FSCORE_O_TRUNC) && (stMetDat.ucAttribute & FFS_ATTRIB_READONLY_MASK))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT cannot truncate FFS_RV_ERR_FILE_READ_ONLY, Uc");
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  /* Assume, no entries are empty in the open file list.                      */
  ucIndexFreeEntry = NO_FREE_ENTRY;

  for (ucCntr = 0; ucCntr < FSCORE_MAX_NR_OPEN_FSCORE_FILES; ucCntr++)
  {
    if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
      ucAccessMode == FFS_OPEN_FILE_ENTRY_NOT_USED)
    {
      /* An free entry in the open file list found.                           */
      ucIndexFreeEntry = ucCntr;
    }
    else
    if ((rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
       stLabLoc.mpt_idx == stLabLoc.mpt_idx)
      && (rfsfat_env_ctrl_blk_p->ffs_globals.
        ffs_astOpenFileList[ucCntr].stLabLoc.uiFirstCl ==
        stLabLoc.uiFirstCl)
      && (rfsfat_env_ctrl_blk_p->ffs_globals.
        ffs_astOpenFileList[ucCntr].stLabLoc.tLblPos ==
        stLabLoc.tLblPos))
    {
      /* File allready in open file list.                                     */
      if ((ucAccessMode == FFS_OPEN_MODE_READWRITE) ||
        (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
         ucAccessMode == FFS_OPEN_MODE_READWRITE))
      {
        RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_FILE_ALLREADY_OPEN, Uc",
                 RV_TRACE_LEVEL_ERROR);
        return FFS_RV_ERR_FILE_ALLREADY_OPEN;
      }
    }

  }

  /* It's ok to open the file, let's see if there is a free entry in the list. */

  if (ucIndexFreeEntry == NO_FREE_ENTRY)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_TO_MUCH_OPEN_FILES",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_TO_MUCH_OPEN_FILES;
  }

  pstFileEntry = &rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucIndexFreeEntry];


 pstFileEntry->stLabLoc.mpt_idx   = stLabLoc.mpt_idx;
 pstFileEntry->stLabLoc.uiFirstCl = stLabLoc.uiFirstCl;
 pstFileEntry->stLabLoc.tLblPos   = stLabLoc.tLblPos;


 pstFileEntry->uiFirstClusterNumber = (stMetDat.uiFirstClusterNumberHigh<<16) |
                                        stMetDat.uiFirstClusterNumber;
 pstFileEntry->tFileSize = (FFS_FILE_LENGTH_TYPE) stMetDat.ulFileSize;
 pstFileEntry->ucAccessMode = ucAccessMode;

 pstFileEntry->uiClusterNumber_L 	=  pstFileEntry->uiFirstClusterNumber;
 pstFileEntry->uiClusterNumber_H 	=  pstFileEntry->uiFirstClusterNumber;
 pstFileEntry->tFilePointer_H 		= 0;
 pstFileEntry->tFilePointer_L 		= 0;

  
	
  if ((flag & FSCORE_O_APPEND) == FSCORE_O_APPEND)
  {
   pstFileEntry->tFilePointer = pstFileEntry->tFileSize;
  }
  else
   pstFileEntry->tFilePointer = INITIAL_VALUE_FILE_POINTER;
  
  
  if ((flag & FSCORE_O_TRUNC) == FSCORE_O_TRUNC)
  {
    //delete cluster chain in Fat Table
    if(stMetDat.ulFileSize != DEFAULT_FILE_SIZE)
    {
      ucResult = FAM_ucDelLinkedList (stLabLoc.mpt_idx, pstFileEntry->uiFirstClusterNumber);
      if ((ucResult != FFS_NO_ERRORS) && (ucResult != FAM_RET_LAST_CLUSTER))
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: error deleting linked list for Truncate");
        return ucResult;
      }
    }
    //update file label with new filesize and start cluster
    stMetDat.uiFirstClusterNumber = DEFAULT_FILE_FIRST_CLUSTER;
    stMetDat.uiFirstClusterNumberHigh = DEFAULT_FILE_FIRST_CLUSTER;
    stMetDat.ulFileSize = DEFAULT_FILE_SIZE;
    
    CPY_LBL_TO_LBL(DIR_META_DATA_P, &stMetDat);
    ucResult = ucSetMetaData (&stLabLoc, (META_TAG_CLUSTER | META_TAG_SIZE));
    if (ucResult != FFS_NO_ERRORS) 
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: updating file label for Truncate failed");
      return ucResult;
    }

    //update openfile list
    pstFileEntry->uiFirstClusterNumber = DEFAULT_FILE_FIRST_CLUSTER;
    pstFileEntry->tFilePointer = INITIAL_VALUE_FILE_POINTER;
    pstFileEntry->tFileSize = DEFAULT_FILE_SIZE;
  }
  /* Return handle.                                                           */
  *pucHandle = ucIndexFreeEntry;

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucFileOpen ",
           RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;

}




/**************************************************/
//                              
// end of Unicode FFS       
//                              
/**************************************************/



#endif






/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
ffs_ucGetTimeAndDate (UINT16 * puiTime, UINT16 * puiDate)
/* PURPOSE      : Funtion determines system time and date, and convert the
 *                information into seperate unsigned integers, ready for the
 *                directory label.
 *
 * ARGUMENTS    : puiTime:
 *                - pointer to unsigned integer, in which the current time
 *                  will be written, as defined in the directory label struct.
 *                puiData:
 *                - pointer at unsigned integer, in which the current date
 *                  will be written, as defined in the directory label struct.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : For both pointers puiTime and puiDate the value of NULL is
 *                not allowed.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  OS_TIME_STRUCT stTime;
  UINT16 uiTmp;

  if (OS_ucGetTime (&stTime) == 0)
  {
    /*
       Time integer:
       MSB %HHHH.HMMM.MMMS.SSSS   LSB
       H = hours
       M = Minutes
       S = Seconds in 2-second steps.
     */
    *puiTime = (((UINT16)stTime.ucSec) &0x001F);
	
	uiTmp = stTime.ucMin;
	uiTmp <<= 5;
    uiTmp &= 0x07E0;
	*puiTime |= uiTmp;
	
    uiTmp = stTime.ucHour;
	uiTmp<<=11;
    uiTmp &=0xF800;
	
    *puiTime |= uiTmp;

    /*
       Date integer:
       MSB %YYYY.YYYM.MMMD.DDDD   LSB
       Y = Year
       M = Month
       D = Day
     */
    *puiDate = (((UINT16)stTime.ucMDay) & 0x001F);

	uiTmp = stTime.ucMon;
	uiTmp<<=5;
	uiTmp&= 0x01E0;
	*puiDate |= uiTmp;
	
    uiTmp = stTime.ucYear;
	
	uiTmp<<=9;
	uiTmp &= 0xFE00;
    *puiDate |= uiTmp;

    return FFS_NO_ERRORS;
  }
  return FFS_RV_ERR_DATE_TIME_DETERMINATION;
}

/******************************************************************************
 *      Implementation of Local (= static) Function                           *
 ******************************************************************************/

/******Copyright by ICT Embedded BV All rights reserved ***********************/
INT8
OS_ucGetTime (OS_TIME_STRUCT * pstTime)
/* PURPOSE      : Function determines system time and date, and stores info
 *                in given structure.
 * ARGUMENTS    : pstTime:
 *                -pointer to buffer in which time and date will be written.
 * RETURNS      : 0: No problems.
 *                1: Error occured.
 * PRECONDITION : None.
 * POSTCONDITION: pstTime may not have the value of NULL.
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE         : -
 ******************************************************************************/
{
  T_RVF_RET status;
  T_RTC_DATE_TIME date_time; 

 status = RTC_GetDateTime (&date_time);
	if (status==RVF_OK)
	{
	pstTime->ucSec  = (UINT8) date_time.second;
    pstTime->ucMin  = (UINT8) date_time.minute;
    pstTime->ucHour = (UINT8) date_time.hour;
    pstTime->ucMDay = (UINT8) date_time.day;
    pstTime->ucMon  = (UINT8) date_time.month;
    pstTime->ucYear = (UINT8) (date_time.year+20);  
	/* Note that RTC gives values from 00 to 99, so 2000 needs be added , and FAT requires the time start from 1980,
	  so (2000-1980)=20 should be added */
    return 0;
  }
  RFSFAT_SEND_TRACE_ERROR("RFSFAT, RTC_GetDateTime error.");
  return ((INT8)-1);

}

