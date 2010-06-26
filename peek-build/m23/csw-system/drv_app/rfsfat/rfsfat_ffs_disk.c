/**
 * @file	rfsfat_ffs_disk.c
 *
 * General file system functions (like init and terminate) and
 * functions for manipulating/activating partitions (normally
 * disks).
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

/******************************************************************************
 *     Includes                                                               *
 ******************************************************************************/

#include "rvf/rvf_api.h"
#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"


#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"
#include <string.h>
#include <ctype.h>

#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_dir.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_fam.h"
#include "rfsfat/rfsfat_ffs_disk.h"
#include "rfsfat/rfsfat_ffs_dman.h"
#include "rfsfat/rfsfat_ffs_file.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfsfat_posconf.h"
#include "rfsfat/rfsfat_api.h"


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

                                         
		 
extern UINT8
ucGetNewLabelPos (DIR_LABEL_LOCATION_STRUCT *pstLabelLocation, UINT8 nr_adj_lbl);


UINT8 ffs_ucDiskGetPartStatus (UINT8 mpt_idx);



/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDiskInit (void)
/* PURPOSE      : This function initialises the file system component.
 *                Different buffers will be initialised, variables will be set
 *                (i.e. the different current locations, the current partition
 *                Id.) and the Media Manager will be initialised.
 *
 * ARGUMENTS    : None
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: ffs_bSystemInitDone is set TRUE.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : One global variable ffs_bSystemInitDone will be set TRUE, after
 *                the init procedure was ended successfully. Using this
 *                variable, other API functions can check if the file system
 *                was initialised, and prevent using the function before this.
 ******************************************************************************/
{
  UINT8 ucCntr;

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucInit entered", RV_TRACE_LEVEL_DEBUG_LOW);

  // In case the file system is already initialised, return error.            
  if (rfsfat_env_ctrl_blk_p->ffs_globals.ffs_bSystemInitDone)
	{
	  RFSFAT_SEND_TRACE
		("RFSFAT: exit E_FFS_ucInit ->FFS_RV_ALREADY_INITIALISED",
		 RV_TRACE_LEVEL_DEBUG_LOW);
	  return FFS_RV_ALREADY_INITIALISED;
	}

  // Set current working partition                                               


  // Mark all entries in the open file list empty.                            
  for (ucCntr = 0; ucCntr < FSCORE_MAX_NR_OPEN_FSCORE_FILES; ucCntr++)
	{
	  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].
		ucAccessMode = FFS_OPEN_FILE_ENTRY_NOT_USED;
	}

  //
  //the MMA init is moved to the rfsfat_configure function
  //

  // Set global variable ffs_bSystemInitDone to TRUE, initialising has        
  // finished.                                                                
  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_bSystemInitDone = TRUE;
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDiskTerminate (void)
/* PURPOSE      : This function stops all running processes within the file
 *                will be system component. The global variable
 *                ffs_bSystemInitDone set to the value FALSE.
 *
 * ARGUMENTS    : None
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: ffs_bSystemInitDone is FALSE.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucDiskTerminate entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  rfsfat_env_ctrl_blk_p->ffs_globals.ffs_bSystemInitDone = FALSE;

  RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucDiskTerminate ",
					 RV_TRACE_LEVEL_DEBUG_LOW);
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDiskGetPartStatus (UINT8 mpt_idx)
/* PURPOSE      : This function will determine the status of a partition.
 *
 * ARGUMENTS    : mpt_idx:
 *            
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

  RFSFAT_SEND_TRACE ("RFSFAT E_FFS_ucDiskGetPartStatus entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
	RFSFAT_SEND_TRACE ("RFSFAT exit E_FFS_ucDiskGetPartStatus ",
					   RV_TRACE_LEVEL_DEBUG_LOW);
  return MMA_ucGetPartitionStatus (mpt_idx);
}



/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDiskDataSpace (UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulSize)
/* PURPOSE      : The function E_FFS_ucDiskDataSpace returns size of data clusters area of partition
 *                in terms of Bytes. This number will be written into the buffer, pulSize is pointing at.
 *
 * ARGUMENTS    : mpt_idx:
 *                -index in mountpoint table.
 *                pulSize:
 *                -The number of free bytes on the partition will be written into
 *                 the buffer, pulSize is pointing at.
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
 * NOTE         :
 ******************************************************************************/
{
  UINT8 ucResult=FFS_NO_ERRORS;
  UINT32 Nuclusters;    

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucDiskSpace entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
	/* Check pointer parameters.                                                */
	if (pulSize == NULL)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
						 RV_TRACE_LEVEL_ERROR);
	  return FFS_RV_ERR_INVALID_PARAMS;
	}

  /* Determine mountpoint. and check if valid.                                 */
  ucResult = MMA_ucGetPartitionStatus (mpt_idx);
  if (ucResult != FFS_RV_FORMATTED)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: partition not formatted",
						 RV_TRACE_LEVEL_ERROR);
	  return ucResult;
	}


   ucResult = MMA_ucGetBootSectorInfo (mpt_idx);
  if (ucResult != FFS_NO_ERRORS)
  {

    RFSFAT_SEND_TRACE ("RFSFAT: Error reading bootsector, E_FFS_ucDiskSpace",
     RV_TRACE_LEVEL_ERROR);

    /* Error reading bootsector                        */
    return ucResult;
  }

  FAM_ucNrDataclusters(mpt_idx, &Nuclusters);


  /* convert into bytes */
  *pulSize = (UINT32) (MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector * MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster * Nuclusters);
  

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucDiskSpace ",
					 RV_TRACE_LEVEL_DEBUG_LOW);


  /* Determine number of free bytes.                                          */
  return ucResult;
}





/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
E_FFS_ucDiskFree (UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulSize)
/* PURPOSE      : The function E_FFS_ucDiskFree() returns the number of free
 *                bytes at the specified. This number will be written into the
 *                buffer, pulSize is pointing at.
 *
 * ARGUMENTS    : mpt_idx:
 *                -index in mountpoint table.
 *                pulSize:
 *                -The number of free bytes on the partition will be written into
 *                 the buffer, pulSize is pointing at.
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
 * NOTE         :
 ******************************************************************************/
{
  UINT8 ucResult;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucDiskFree entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
	/* Check pointer parameters.                                                */
	if (pulSize == NULL)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
						 RV_TRACE_LEVEL_ERROR);
	  return FFS_RV_ERR_INVALID_PARAMS;
	}

  /* Determine mountpoint. and check if valid.                                 */
  ucResult = MMA_ucGetPartitionStatus (mpt_idx);
  if (ucResult != FFS_RV_FORMATTED)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: partition not formatted",
						 RV_TRACE_LEVEL_ERROR);
	  return ucResult;
	}

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucDiskFree ",
					 RV_TRACE_LEVEL_DEBUG_LOW);
  /* Determine number of free bytes.                                          */
  return FAM_ucNrFreeBytes (mpt_idx, pulSize);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucDiskStat
  (RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) label)
/* PURPOSE      : The function E_FFS_ucDiskStat returns information about a
 *                file or directory, indicated by its path.
 *
 * ARGUMENTS    : pucPath:
 *                -Each path name may consist of a partition Id., a location and
 *                 a filename with a file extension. In case no partition Id. or
 *                 location is given, the 'current values' will be used.
 *                label:
 *                -Pointer to buffer where the file's meta data will be stored.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: The most important part of the file or directories' meta data
 *                is stored in the buffer, label is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucDiskStat entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
	/* Check pointer parameters)                                                */
	if ((pucPath == NULL) || (label == NULL))
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS",
						 RV_TRACE_LEVEL_ERROR);
	  return FFS_RV_ERR_INVALID_PARAMS;
	}

  ucResult = ffs_ucGetLabel (pucPath, &stLabLoc, label);
  if (ucResult != FFS_NO_ERRORS)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: label not succesfully read",
						 RV_TRACE_LEVEL_ERROR);
	  return ucResult;
	}

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucDiskStat",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  return FFS_NO_ERRORS;
}

UINT8 E_FFS_ucDiskSetVolumeLabel (UINT8 mpt_idx, const char *label)
/* PURPOSE      : The function E_FFS_ucDiskSetVolumeLabel sets the volume label into
 *                        the boot sector and as well as the root directory.
 * ARGUMENTS    :  mpt_idx : mount point index of the partition
 *                          label      : volume label
 * RETURNS      : FFS_NO_ERRORS:
 *
 * PRECONDITION : The partitions are initialised 
 *
 * POSTCONDITION: In case FAT16 if there is a free entry in the root directory cluster, the
 *                            label is written only in boot sector. In all other cases, the label is written
 *                            both in boot sector & root directory cluster.
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/	
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT metdat_p;
  DIR_LABEL_LOCATION_STRUCT pstLabLoc;

  UINT32  uiClusterNr = MMA_F32_ROOT_CLUSTER_NMB;
  UINT8    ucRetVal, uiLen;
  UINT8    uiCnt;
  UINT16  uiNmbDirEntriesInCluster;

   // validate the label name
   uiLen = rfsfat_uiStrlen((UINT8 *)label);
   
   // Length should not exceed MAX_PART_NAME
   if ( uiLen > MAX_PART_NAME)
   	 uiLen = MAX_PART_NAME;
#if 0   
   for(uiCnt=0; uiCnt <uiLen; uiCnt++)
  {
        if (!isalphanum(label[uiCnt]))
        {
             RFSFAT_SEND_TRACE_ERROR("RFSFAT:  Invalid volume label"); 
             return FFS_RV_ERR_INVALID_PARAMS;			 
        }			 
  }
#endif 
#if 0
   // Write the label in the boot sector.
   if(MMA_ucSetBootSectorLabel(mpt_idx, label) != FFS_NO_ERRORS )
   {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT:  Failed to set label in boot sector");   
   }
#endif   
   // get root directory cluster
   pstLabLoc.mpt_idx = mpt_idx;
   pstLabLoc.tLblPos = 0;

  // FAT32 & FAT12/16 differ here
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
  {
   pstLabLoc.uiFirstCl = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
  }
  else
  {
    pstLabLoc.uiFirstCl = 0;
  }

  do
 {
    ucResult = DIR_ucGetMetaData (&pstLabLoc, &metdat_p);
    // search for the volume label only if not last entry
    if (ucResult == FFS_RV_ERR_LAST_ENTRY)
    {		
        break;
    }
    else
    {
       if ((ucResult == FFS_NO_ERRORS) && (metdat_p.ucAttribute & FFS_ATTRIB_PART_MASK))
       {
           RFSFAT_SEND_TRACE_ERROR("RFSFAT: E_FFS_ucDiskGetVolumeLabel : Found");
	    break;	   
       }
    }	
    if (ucResult != FFS_NO_ERRORS)
    {
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: get meta data failed");
	  return ucResult;
    }
    // next offset	
    pstLabLoc.tLblPos += DIR_SIZE_META_ALL;  
  } while (ucResult != FFS_RV_ERR_LAST_ENTRY);

   if (ucResult != FFS_RV_ERR_LAST_ENTRY)
  {
       // label found ! replace it with new label.
       rfsfat_vpMemset(metdat_p.aucName, 0x20, MAX_PART_NAME);
       rfsfat_vpMemcpy(metdat_p.aucName, (void *)label, uiLen);
	ucRetVal = DIR_ucSetMetaData(&pstLabLoc, &metdat_p);
       if (ucRetVal != FFS_NO_ERRORS)
       {
            RFSFAT_SEND_TRACE_ERROR("RFSFAT: Disk set label failed");
       }	   
   }	   
   else
   {
       // label not found ! create a new directory entry for label and insert
       // For FAT16 if there is  no space in root directory cluster its no possible
       // to write label. 
       // For FAT32, if no free entry, a new cluster should be allocated to the chain of root dir clusters

	// if inserted newly, make sure the next directory entry is 000000
      pstLabLoc.mpt_idx = mpt_idx;
      pstLabLoc.tLblPos = 0;
	
      if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
      {
         pstLabLoc.uiFirstCl = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
      }
      else
      {
         pstLabLoc.uiFirstCl = 0;
      }
	
	ucResult = ucGetNewLabelPos( &pstLabLoc, 1);

       if (ucResult == FFS_NO_ERRORS)
	{
	    // fill meta data here, set to zero & initialise rest of the values
          rfsfat_vpMemset(&metdat_p, DEFAULT_DIR_FIRST_CLUSTER, 
                                       DIR_SIZE_META_ALL);
          rfsfat_vpMemset(metdat_p.aucName, 0x20, MAX_PART_NAME);		
          rfsfat_vpMemcpy(metdat_p.aucName, (void *)label, uiLen);
	   metdat_p.ucAttribute = FFS_ATTRIB_PART_MASK;
	   ucRetVal = DIR_ucSetMetaData(&pstLabLoc, &metdat_p);
       }
       else
	{	   	
            RFSFAT_SEND_TRACE_ERROR("RFSFAT: No space to set labels");		   
       }	   
   }

  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit E_FFS_ucDiskSetVolumeLabel");
  return ucResult;
}


UINT8 E_FFS_ucDiskGetVolumeLabel (UINT8 mpt_idx, char *label)
/* PURPOSE      : The function E_FFS_ucDiskGetVolumeLabel 
 *
 * ARGUMENTS    : 
 *
 * RETURNS      : FFS_NO_ERRORS:
 *
 * PRECONDITION : None.
 *
 * POSTCONDITION: 
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/		
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT metdat_p;
  DIR_LABEL_LOCATION_STRUCT pstLabLoc;

  UINT32  uiClusterNr = MMA_F32_ROOT_CLUSTER_NMB;
  UINT8 	ucRetVal;
  UINT16  uiCnt;
  UINT16  uiNmbDirEntriesInCluster;

 // get root directory cluster
  pstLabLoc.mpt_idx = mpt_idx;
  pstLabLoc.tLblPos = 0;

  // FAT32 & FAT12/16 differ here
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
 {
   pstLabLoc.uiFirstCl = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
 }
  else
 {
    pstLabLoc.uiFirstCl = 0;
 }

  do
 {
    ucResult = DIR_ucGetMetaData (&pstLabLoc, &metdat_p);
    if (ucResult != FFS_NO_ERRORS)
    {
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: get meta data failed");
	  return ucResult;
    }
    // search for the volume label only if not last entry
    if (ucResult != FFS_RV_ERR_LAST_ENTRY)
    {
       if (metdat_p.ucAttribute & FFS_ATTRIB_PART_MASK)
       {
           
           // label found ! return to the application
           // strcat aucName & aucExtn & return into label
           rfsfat_vpMemcpy(label, (void *)metdat_p.aucName, DIR_SIZE_META_NAME);
           rfsfat_vpMemcpy((label+DIR_SIZE_META_NAME), (void *)metdat_p.aucExtension, DIR_SIZE_META_EXT);		   
		   
           RFSFAT_SEND_TRACE_ERROR("RFSFAT: E_FFS_ucDiskGetVolumeLabel : Found");
	    break;	   
       }
    }	
	
    // next offset	
    pstLabLoc.tLblPos += DIR_SIZE_META_ALL;  
  } while (ucResult != FFS_RV_ERR_LAST_ENTRY)
  
  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit E_FFS_ucDiskGetVolumeLabel");
  return ucResult;
}

/******************************************************************************
 *      Implementation of global functions within module.                     *
 ******************************************************************************/

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 ffs_ucGetLabel
  (RW_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetDat)
/* PURPOSE      : This function determines the location and the directory label
 *                of a file or directory, indicated with its path.
 *
 * ARGUMENTS    : pucPath:
 *                -Null terminated string, containing partition Id. location and
 *                 directory- or filename.
 *                pstLabLoc:
 *                -Pointer to buffer, in which location of file or directory
 *                 will be copied.
 *                pstMetDat
 *                -Pointer to buffer, in which meta data of file or directory
 *                 will be copied.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pucPath, pstLabLoc and pstMetDat may not be null.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;

  ucResult = ffs_ucGetPartAndDirTableFromPath (pucPath, pstLabLoc);
  if (ucResult != FFS_NO_ERRORS)
	{
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: get partition path from dir failed");
	  return ucResult;
	}
  //get metadata of 8+3 label (even inside LFN it points to 8+3 lbl)
  ucResult = DIR_ucGetMetaData (pstLabLoc, pstMetDat);
  if (ucResult != FFS_NO_ERRORS)
	{
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: get meta data failed");
	  return ucResult;
	}
  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucGetLabel");
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/

UINT8 ffs_ucGetPartAndDirTableFromPath
  (RW_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc)
/* PURPOSE      : This function determines the location of the directory label
 *                of the file or directory, indicated with pucPath.
 *                In case no mountpoint index is given in the path, the current
 *                partition will be used.
 *                In case no location is given in the path, the current location
 *                of the partition will be used.
 *
 * ARGUMENTS    : pucPath:
 *                -Null terminated string, containing partition Id. location and
 *                 directory- or filename.
 *                pstLabLoc:
 *                -Pointer to buffer, in which location of file or directory
 *                 will be copied.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pucPath and pstLabLoc may not be NULL.
 *
 * POSTCONDITION: The pstLabLoc->labpos contains the location of the 8+3 label
 *                even if the located path is a LFN.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT16 ucPosLocation;
  UINT16 lfncntr = 0;
  DIR_DIRECTORY_LABEL_STRUCT stMetDat;
  DIR_DIRECTORY_LABEL_STRUCT *stMetDat_p;
  UINT16 ucPosOneAfterLastChar = 0;	/* Position of last char + 1 of file/dir     */
  /* name in path.                             */
  stMetDat_p = &stMetDat;
  
  /****************************************************************************/
  /* Determine partition.                                                        */
  /****************************************************************************/
  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: ffs_ucGetPartAndDirTableFromPath entered, following path");
  RFSFAT_SEND_TRACE_DEBUG_LOW((char*)pucPath);


  pstLabLoc->mpt_idx = RFSFAT_get_mpt_idx((const UINT8*)(pucPath),&ucPosLocation);

  /* ucPosLocation contains the index of the '/' character */
 
  if (pstLabLoc->mpt_idx == RFSFAT_INVALID_MPT)
	{
	  //no matching mountpointnames were found 
	  RFSFAT_SEND_TRACE ("RFSFAT: no matching mountpoint names were found",
						 RV_TRACE_LEVEL_ERROR);
	  return FFS_RV_ERR_INVALID_PATH;
	}
  ucResult = MMA_ucGetPartitionStatus (pstLabLoc->mpt_idx);
  if (ucResult != FFS_RV_FORMATTED)
	{
	  /* partition is not formatted. Return.                                     */
	  RFSFAT_SEND_TRACE ("RFSFAT: disk get partition status failed",
						 RV_TRACE_LEVEL_ERROR);
	  return ucResult;
	}

  pstLabLoc->tLblPos = DIR_OFFSET_FIRST_ENTRY;

  /****************************************************************************/
  /* Determine first cluster number of directory                              */
  /****************************************************************************/
  if (pucPath[ucPosLocation] == FFS_ASCII_VALUE_FWSLASH)
	{
	  /* Initially first entry of root directory will be selected.              */
	  /* (Path is absolute.)                                                    */
	  pstLabLoc->uiFirstCl = DIR_CLUSTER_NR_ROOT_DIRECTORY;
	  ucPosLocation++;
	}
  else
   if(pucPath[ucPosLocation] == FFS_ASCII_EOL)	
  	{
	  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: FFS_RV_ROOTDIR_SELECTED");
   	  pstLabLoc->uiFirstCl = DIR_CLUSTER_NR_ROOT_DIRECTORY;
   
	  return FFS_RV_ROOTDIR_SELECTED;  
	  
   	}
  else
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PATH",
						 RV_TRACE_LEVEL_ERROR);
	  return FFS_RV_ERR_INVALID_PATH;
	}
  /* Receive first directory name (I.e path = 'C:\DIR1\DIR2\FILE.TXT', then   */
  /* 'DIR1       ' will be stored in stMetDat.aucName.                        */
  //LFN mod
  if (test_file_lfn (&pucPath[ucPosLocation]))
	{
    //LFN
    RFSFAT_LFN_FLAG = TRUE;
    rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
    ucResult = rephr_lfn(ucPosLocation, &pucPath[ucPosLocation],RFSFAT_LFN, &lfncntr);
    if(ucResult == FFS_RV_ERR_INVALID_PATH)
    {
      //the file or dir name was not valid
      return ucResult;
    } else if(lfncntr == 0) {

      // Invalid name/no name, so start of name = end of name.
      ucPosOneAfterLastChar = ucPosLocation;
    } else {
      //file or dir name found
      ucPosOneAfterLastChar = lfncntr;
    }
  } else {
    RFSFAT_LFN_FLAG = FALSE;
    /* 8.3 Get next name and extention.                                    */
	  ucPosOneAfterLastChar = ffs_ucRephraseNameAndExt(ucPosLocation, pucPath, &stMetDat);
	}

  /* Check if root directory is selected. If so, stop determination.          */
  if ((ucPosOneAfterLastChar == ucPosLocation) &&
	  (pucPath[ucPosOneAfterLastChar] == EOSTR) &&
	  (pstLabLoc->uiFirstCl == DIR_CLUSTER_NR_ROOT_DIRECTORY))
	{
	  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: FFS_RV_ROOTDIR_SELECTED");
	  return FFS_RV_ROOTDIR_SELECTED;
	}

#ifdef FFS_FEATURE_DIRECTORIES
  while (ucPosOneAfterLastChar != ucPosLocation)
#endif
	{
	  /* Get directory label.                                                   */
	  ucResult = DIR_ucGetLabel (pstLabLoc, &stMetDat);
	  //if the label was found and it was a LFN the labelloc will point at the 
	  //8+3 label of that LFN and stMetDat contains the 8+3 meta data
	  if (ucResult != FFS_NO_ERRORS)
		{
      if (ucResult == DIR_RV_DIR_NOT_FOUND)
			{
			  RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_RV_DIR_NOT_FOUND ....2");
			  return DIR_RV_DIR_NOT_FOUND;
			}
		  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: DIR get label failed");
		  return ucResult;
		}

	  ucPosLocation = ucPosOneAfterLastChar;
	  if ((pucPath[ucPosLocation] == FFS_ASCII_VALUE_FWSLASH) &&
  		   ATTR_CHECK_SUBDIR(stMetDat_p))
		{
		  ucPosLocation++;
		}
	  //LFN mod
	  if (test_file_lfn (&pucPath[ucPosLocation]))
		{
      //LFN
      RFSFAT_LFN_FLAG = TRUE;
      rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
      ucResult = rephr_lfn(ucPosLocation, &pucPath[ucPosLocation],RFSFAT_LFN, &lfncntr);
      if((ucResult == FFS_RV_ERR_INVALID_PATH) || (ucResult == FFS_RV_ERR_INVALID_PARAMS))
      {
        //the file or dir name was not valid
        return ucResult;
      } else if(lfncntr == 0) {

        // Invalid name/no name, so start of name = end of name.
        ucPosOneAfterLastChar = ucPosLocation;
      } else {
        //file or dir name found
        ucPosOneAfterLastChar = lfncntr;
      }
    } else {
      RFSFAT_LFN_FLAG = FALSE;
      /* 8.3 Get next name and extention.                                    */
		  ucPosOneAfterLastChar = ffs_ucRephraseNameAndExt (ucPosLocation, pucPath, &stMetDat);
		}
	  if (ucPosOneAfterLastChar != ucPosLocation)
		{
		  /* In this situation, the path is containing more characters then       */
		  /* determined yet, so check if this is a directory, otherwise path is   */
		  /* invalid.                                                             */
		  if (ATTR_CHECK_SUBDIR(stMetDat_p))
			{
			  /*This is a directory. */
			  pstLabLoc->uiFirstCl = (stMetDat.uiFirstClusterNumberHigh<<16) |
			                          stMetDat.uiFirstClusterNumber;
			}
		  else
			{
			  return FFS_RV_ERR_INVALID_PATH;
			}
		}
	}
  /* Last label is found, no characters may follow.                           */
  if (pucPath[ucPosOneAfterLastChar] != EOSTR)
	{
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: FFS_RV_ERR_INVALID_PATH");
	  return FFS_RV_ERR_INVALID_PATH;
	}
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT16 ffs_ucRephraseNameAndExt
  (UINT16 ucStartPos,
   RW_PTR_TO_RO (UINT8) pucPath, 
   DIR_DIRECTORY_LABEL_STRUCT *  prefill_label)
/* PURPOSE      : This function translates a name-dot-extension into a name
 *                and extension, as defined in a directory label.
 *                I.e. "CONFIG.SS" becomes "CONFIG  SS " (including spaces).
 *
 * ARGUMENTS    : ucStartPos:
 *                -Position in pucPath where the file/directory name starts.
 *                pucPath:
 *                -Pointer to buffer, containing at least a file/directory name
 *                 and eventually its extension.
 *                -prefill_label, pointer to label struct were the search info is stored
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : ucStartPos must be smaller then the string length of pucPath,
 *                pucPath and pucName may not be NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucCntr;

  if ((pucPath[ucStartPos] == FFS_ASCII_VALUE_FWSLASH) ||
	  (pucPath[ucStartPos] == EOSTR))
	{
	  /* Invalid name/no name, so start of name = end of name.                  */
	  return ucStartPos;
	}

  /* reset the lcase field */
  prefill_label->lcase = DEFAULT_LCASE_VALUE;
  /* Build first 8 bytes of name (including spaces.                           */
  for (ucCntr = 0; ucCntr < DIR_SIZE_META_NAME; ucCntr++)
	{
	  if (pucPath[ucStartPos] == FFS_ASCII_VALUE_DOT)
		{
		  if ((ucCntr == 0) ||
			  ((ucCntr == 1)
			   && (prefill_label->aucName[ucCntr - 1] == FFS_ASCII_VALUE_DOT)))
			{
			  prefill_label->aucName[ucCntr] = FFS_ASCII_VALUE_DOT;
			  ucStartPos++;
			}
		  else
			{
			  prefill_label->aucName[ucCntr] = FFS_ASCII_VALUE_SPACE;
			}
		}
	  else if ((pucPath[ucStartPos] == EOSTR) ||
			   (pucPath[ucStartPos] == FFS_ASCII_VALUE_FWSLASH))
		{
		  prefill_label->aucName[ucCntr] = FFS_ASCII_VALUE_SPACE;
		}
	  else
		{
     /* check is this small letter, if yes then change to capital */
     if((pucPath[ucStartPos] >= ASCII_VALUE_LOWA) && (pucPath[ucStartPos] <= ASCII_VALUE_LOWZ))
     	{
     	    prefill_label->aucName[ucCntr] = toupper(pucPath[ucStartPos]);	
			prefill_label->lcase = FFS_ATTRIB_NAME_SMALL_MASK;
     	}else
     		{
      		  prefill_label->aucName[ucCntr] = pucPath[ucStartPos];
     		} 
		  ucStartPos++;
		}
  	}

  /* In case the given name has a length of 8 chars, the next dot must be     */
  /* ignored                                                                  */
  if (pucPath[ucStartPos] == FFS_ASCII_VALUE_DOT)
	{
	  ucStartPos++;
	}
  for (ucCntr = 0;
	   ucCntr < (DIR_SIZE_META_EXT ); ucCntr++)
	{
	  if ((pucPath[ucStartPos] == EOSTR) ||
		  (pucPath[ucStartPos] == FFS_ASCII_VALUE_FWSLASH))
		{
		  prefill_label->aucExtension[ucCntr] = FFS_ASCII_VALUE_SPACE;
		}
	  else
		{
            /* check is this small letter, if yes then change to capital */
          if((pucPath[ucStartPos] >= ASCII_VALUE_LOWA) && (pucPath[ucStartPos] <= ASCII_VALUE_LOWZ))
     	  {
     	    prefill_label->aucExtension[ucCntr] = toupper(pucPath[ucStartPos]);	
			prefill_label->lcase |= FFS_ATTRIB_EXT_SMALL_MASK;
     	  }
		  else
		  	{
				  prefill_label->aucExtension[ucCntr] = pucPath[ucStartPos];
		  	}	  
		  ucStartPos++;
		}
	}			
  return ucStartPos;
}

#if 0
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 ffs_ucMakeNameDotExtension
  (RO_PTR_TO_RO (UINT8) pucNameExt, UINT8 * pucNameDotExt)
/* PURPOSE      : This is the reverse function of
 *                ffs_ucRephraseNameAndExt. It will translate the name
 *                and extension in the directory label into a string with
 *                name-dot-extionsion, without any padding spaces.
 *
 * ARGUMENTS    : pucNameExt:
 *                -Pointer at NULL terminated string, containing the name and
 *                 extension as defined in the directory label;
 *                pucNameDotExt:
 *                -Pointer at buffer in which the translated name-dot-extension
 *                 will be written.
 *
 * RETURNS      : The string length of the newly created name-dot-extension
 *                will be returned.
 *
 * PRECONDITION : Both pucNameExt and pucNameDotExt may not have the value NULL,
 *                and must point at NULL terminated strings.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucCntr;
  UINT8 ucLength;
  BOOL bDotSet;
  /* pucNameExt = [CONFIG  SYS] ==> pucNameDotExt = [CONFIG.SYS]              */

  ucLength = 0;
  bDotSet = FALSE;

  (void) rfsfat_vpMemset (pucNameDotExt, 0, (FFS_MAX_SIZE_NAME_DOT_EXT));
  for (ucCntr = 0; ucCntr < (DIR_SIZE_META_NAME + DIR_SIZE_META_EXT+1);
	   ucCntr++)
	{
	  if (pucNameExt[ucCntr] != FFS_ASCII_VALUE_SPACE)
		{
		  if ((ucCntr >= DIR_SIZE_META_NAME) && (!bDotSet))
			{
			  *pucNameDotExt = FFS_ASCII_VALUE_DOT;
			  pucNameDotExt++;
			  ucLength++;
			  bDotSet = TRUE;
			}
		    /* if dot character appears before DIR_SIZE_META_NAME */
          if((pucNameExt[ucCntr] == FFS_ASCII_VALUE_DOT) &&(!bDotSet))
		  			bDotSet=TRUE;


		  *pucNameDotExt = pucNameExt[ucCntr];
		  pucNameDotExt++;
		  ucLength++;
		}
	}

  return ucLength;
}

#endif

void ffs_ucCopyNameDotExtension
  (RO_PTR_TO_RO (DIR_DIRECTORY_LABEL_STRUCT) pstMetDat, UINT8 * pucNameDotExt)
{
   UINT8 ucCntr;
   UINT8 o_i=0;
   
#define pucName (pstMetDat->aucName)
#define pucExt  (pstMetDat->aucExtension)
  (void) rfsfat_vpMemset (pucNameDotExt, 0, (FFS_MAX_SIZE_NAME_DOT_EXT));

  for(ucCntr=0,o_i=0;(ucCntr < DIR_SIZE_META_NAME) &&(pucName[ucCntr] != FFS_ASCII_VALUE_SPACE);ucCntr++,o_i++)
  	{
         if(ATTR_CHECK_NAME_SMALL(pstMetDat)) 	      
   	   	   pucNameDotExt[o_i]=tolower(pucName[ucCntr]);
		 else
  	   	   pucNameDotExt[o_i]=pucName[ucCntr]; 
  	   
  	}

     
  /* dont reset the value of o_i */

  /* Check for .3 Ext */
  if(pucExt[0] != FFS_ASCII_VALUE_SPACE)
   	{
  		/* copy the dot character */
  		pucNameDotExt[o_i++]= FFS_ASCII_VALUE_DOT;
    
  	for(ucCntr=0;(ucCntr < DIR_SIZE_META_EXT) &&(pucExt[ucCntr] != FFS_ASCII_VALUE_SPACE);ucCntr++,o_i++)
  		{
  		   if(ATTR_CHECK_EXT_SMALL(pstMetDat))
		      pucNameDotExt[o_i]=tolower(pucExt[ucCntr]);
		   else
  	 	pucNameDotExt[o_i]=pucExt[ucCntr]; 
  		}
  	}

  /* Copy the Null character at the End */
  pucNameDotExt[o_i]=ASCII_EOSTR;
#undef pucName
#undef pucExt
}


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
ffs_bIsNameValid (RW_PTR_TO_RO (UINT8) pucNameDotExt)
/* PURPOSE      : This function checks if given name-dot-extension is valid.
 *
 * ARGUMENTS    : pucNameDotExt:
 *                -Pointer at buffer in which the name-dot-extension must be
 *                 stored.
 *
 * RETURNS      : TRUE:
 *                -given name is valid.
 *                FALSE:
 *                -given name is invalid.
 *
 * PRECONDITION : pucNameDotExt may not have the value NULL and must point
 *                at NULL terminated string.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucCntr;
  UINT8 after_dot_char=0;
   UINT8 is_all_small_char=TRUE;
  /* The variable is_all_small_char particurly determines following.
     all small.ext 
     CAPS.smallext
     small.EXT */
  
  BOOL bNamePart;

  /* Start with name part.                                                    */
  bNamePart = TRUE;

  /* Check name part of name(8)-dot(1)-extension(3).                          */
  for (ucCntr = 0,is_all_small_char=TRUE,after_dot_char=0; ucCntr < (DIR_SIZE_META_NAME + DIR_SIZE_META_EXT + 1); ucCntr++)
	{
	  switch (pucNameDotExt[ucCntr])
		{
		case NULL:
		  if (ucCntr == 0)
			{
			  /* Name must contain at least one character.                          */
			  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name should contain at least 1 character");
			  return FALSE;
			}
		  else
			{
			  /* Done. */
			  return TRUE;
			}
		  /* break; */
		case ASCII_VALUE_DOT:
		  if (ucCntr == 0)
			{
			  /* Name part may not start with dot character and must contain at     */
			  /* least one character                                                */
			  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name may not start with dot character");
			  return FALSE;
			}
		  else
			{
			  if (bNamePart)
				{
				  if (ucCntr > DIR_SIZE_META_NAME)
					{
					  /* Name part consists of too much characters.                     */
					  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name consist of to many \
							characters");
					  return FALSE;
					}
				  /* Done with name part, let's start extension part.                 */
				  bNamePart = FALSE;
				  is_all_small_char=TRUE;
				  after_dot_char = 0;
				}
			  else
				{
				  /* Extension part may not consist a dot character.                  */
				  RFSFAT_SEND_TRACE_ERROR("RFSFAT: extension may not contain dot \
						character");
				  return FALSE;
				}
			}
		  break;
		default:
		  if (!(VALID_SHORT_FN_CHAR((pucNameDotExt[ucCntr]))))
			{
			        if(pucNameDotExt[ucCntr] == ASCII_EOSTR)
			        { 
			          break;
			        }
			       if((is_all_small_char)&&((pucNameDotExt[ucCntr] >= ASCII_VALUE_LOWA) && (pucNameDotExt[ucCntr] <= ASCII_VALUE_LOWZ)))
			       	{
			       	     /* OK, still valid SFN */  /* if all characters before or after are all small */
			  if ((bNamePart) && (ucCntr >= DIR_SIZE_META_NAME))
				{
				  /* Name part consists of too much characters.                       */
				  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name consists out of to many characters");
				  return FALSE;
			       	}
			 if(!bNamePart)	
			  	{
			  	    /* Name part is complete, check the count */
					if(after_dot_char >= DIR_SIZE_META_EXT)
						{
						    return FALSE;  /* its LFN */
						}	
					after_dot_char++;   /* count after dot characters */
			  	}
	       	}
				   else
				   	{
					  /* Name or extension consists invalid character.                    */
					  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name or extension contains invalid SFN\
							character");
					  return FALSE;
					}
		  	}
		  else
			{
			  if((pucNameDotExt[ucCntr] >= ASCII_VALUE_A) && (pucNameDotExt[ucCntr] <= ASCII_VALUE_Z))
			  	{
			  	    /* All characters are not small */
			   is_all_small_char = FALSE;
			  	}	
			  if(!bNamePart)	
			  	{
			  	    /* Name part is complete, check the count */
					if(after_dot_char >= DIR_SIZE_META_EXT)
						{
						    return FALSE;  /* its LFN */
						}	
					after_dot_char++;   /* count after dot characters */
			  	}
			  if ((bNamePart) && (ucCntr >= DIR_SIZE_META_NAME))
				{
				  /* Name part consists of too much characters.                       */
				  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name consists out of to many\
						characters");
				  return FALSE;
				  		   	} 
		
		 
			  else
				{
				  /* OK, valid characters.                                            */
				}
			}
		  break;
		}
	}
  if (pucNameDotExt[ucCntr] != EOSTR)
	{
	  /* Given name has too much characters.                                    */
	  RFSFAT_SEND_TRACE_ERROR("RFSFAT: name contains to many characters");
	  return FALSE;
	}
  return TRUE;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
void ffs_vCopyMetaInSmallLabel
  (RO_PTR_TO_RO (DIR_DIRECTORY_LABEL_STRUCT) pstMetDat,
   RO_PTR_TO_RW (FFS_SMALL_DIRECTORY_LABEL_STRUCT) pstSmallLabel)
/* PURPOSE      : Function copies the most important fields of the directory
 *                label into the smaller label.
 *
 * ARGUMENTS    : pstMetDat:
 *                -Pointer at directory label, containing the source values.
 *                pstSmallLabel:
 *                -Pointer at small label, in which source values will be
 *                 written into.
 *
 * RETURNS      : None.
 *
 * PRECONDITION : Both pstMetDat and pstSmallLabel may not be NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{
  /* Watch out using memcpy for more then one field simultaneously.           */
  /* Padding bytes in structure can make a mess of it.                        */
 #if 0
  (void) ffs_ucMakeNameDotExtension (pstMetDat->aucName,
									 pstSmallLabel->aucNameExt);
 #endif

  
  ffs_ucCopyNameDotExtension(pstMetDat,pstSmallLabel->aucNameExt);
  pstSmallLabel->ucAttribute = pstMetDat->ucAttribute;
  pstSmallLabel->uiLastChangedTime = pstMetDat->uiLastChangedTime;
  pstSmallLabel->uiLastChangedDate = pstMetDat->uiLastChangedDate;
  pstSmallLabel->ulFileSize = pstMetDat->ulFileSize;
}

 /**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8 E_FFS_ucDiskFormat (UINT8 mpt_idx, UINT8 * name, T_GBI_FS_TYPE filesys_type)
/* PURPOSE      : This function formats the volume with the Id. of the value of
 *                ucVolumeID. During this operation, a new FAT table and new FAT
 *                table copies will be created on the volume, and the root
 *                directory is made empty. Besides, a new volume boot sector
 *                will be stored on the volume.
 *
 * ARGUMENTS    : mpt_idx:
 *                -mountpoint index that will be used.
 *								name
 *								-mountpoint (partition) name after format
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
  DIR_LABEL_LOCATION_STRUCT stLabelLocation;
  UINT8 ucCntr;
  T_GBI_FS_TYPE ver_fs_type;

  FFS_STOP_IF_NOT_INITIALISED

  /* This has requested some file system type */
  /* Check for the validaty */
 ver_fs_type= MMA_verify_fat_type_size(mpt_idx,filesys_type);

 if(ver_fs_type == GBI_UNKNOWN) 
	{
	  #if 0
	    if(filesys_type != GBI_UNKNOWN)
	    	{
	  	        return MMA_RET_BS_BAD_VAL; /* requested file system type is not allowed */
	    	}   
	  #else
	    ;
	  #endif
  	}
  else
  	{
  	   MMA_FS_TYPE(mpt_idx) = ver_fs_type;    /* change file system type */
  	}



#if 0
  /*No need to check the open files */
  if (ffs_bCheckIfInOpenFileList (mpt_idx, NULL, NULL))
	{
	  return FFS_RV_FILES_IN_OPEN_FILE_LIST;
	}
#else
  /* clear all the entries of open file list */

  // Mark all entries in the open file list empty.                            
  for (ucCntr = 0; ucCntr < FSCORE_MAX_NR_OPEN_FSCORE_FILES; ucCntr++)
	{
	 
	  if(rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[ucCntr].fd != RFS_FD_DEF_VALUE)
	  	{

         if(rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].stLabLoc.mpt_idx == mpt_idx)
          {
			 rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].ucAccessMode = FFS_OPEN_FILE_ENTRY_NOT_USED;
		     rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[ucCntr].fd = RFS_FD_DEF_VALUE;		 
          }
		  
	  	}    /* End of If */
	}     /* End of If */

#endif  

#ifdef ICT_SPEEDUP_RFSFAT2

  Reset_FAT_TABLE_cache(mpt_idx);

#endif
  /* flush the write block once */
  flush_write_blk_kill_cache();

  /* Create new bootsector for volume.                                        */
  ucResult = MMA_ucCreateBootSector (mpt_idx, name);
  if (ucResult != FFS_NO_ERRORS)
	{
	  return ucResult;
	}

  /* Create new fat table(s) for volume.                                      */
  if (!FAM_bCreateFatTables (mpt_idx))
	{
	  return FFS_RV_ERR_WRITING_FAT_TABLES;
	}

  /* flush the write block once */
  flush_write_blk_kill_cache();
  /* Clear the root directory of volume.                                      */
  stLabelLocation.mpt_idx = mpt_idx;
  ucResult = DIR_ucClearRoot (&stLabelLocation, name);
  if (ucResult != FFS_NO_ERRORS)
	{
	  return ucResult;
	}
#ifdef ICT_SPEEDUP_RFSFAT2

  /* flush the FAT cache*/
  ucFlushFamCash(mpt_idx);

#endif

/* flush the write block once */
  flush_write_blk_kill_cache();

 /* Mark bootsector as read (for this mountpoint)                                */
  MMA_BOOT_SECTOR[mpt_idx].BootSectIdx = mpt_idx;

  rfsfat_env_ctrl_blk_p->ffs_globals.bInitialised = TRUE;
  return FFS_NO_ERRORS;
}

/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
ffs_ucDiskGetPartStatus (UINT8 mpt_idx)
/* PURPOSE      : This function checks if a valid volume id was given (one
 *                between 'A' up and including 'Z'), calculates the volume id.
 *                number and request its status.
 *
 * ARGUMENTS    : pucVolumeId:
 *                -Pointer at buffer, containing volume id.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                -The operation was ended successfully.
 *                [Others]:
 *                -See Appendix A of the DDD FS Interpreter Block.
 *
 * PRECONDITION : pucVolumeId may not be NULL;
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 ******************************************************************************/
{

  RFSFAT_SEND_TRACE ("RFSFAT: ffs_ucDiskGetVolStatus entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  return MMA_ucGetPartitionStatus (mpt_idx);
}

BOOL
test_file_lfn (const UINT8* path)
{
  INT16   slash = FFS_ASCII_VALUE_FWSLASH;
  UINT8 *slashpos;
  UINT32 namesize;
  BOOL ret = TRUE;
  UINT8 temppath[FFS_MAX_SIZE_NAME_DOT_EXT];



 RFSFAT_SEND_TRACE ("RFSFAT: test_file_lfn entered", RV_TRACE_LEVEL_DEBUG_LOW);
  slashpos = (UINT8*)strchr((char*)path, slash);
  if(slashpos != NULL) 
  {
    //dirname
    namesize = slashpos - path;
  } else {
    //filename
    namesize = strlen((const char*)path);
  }
  if (namesize <= (DIR_SIZE_META_EXT + DIR_SIZE_META_NAME + 1))
	{
    if(namesize != 0)
    { 
      //dirnames are always provided with a trailing slash
      //before the name is checked to be valid this slash must be removed
      if(path[namesize] == FFS_ASCII_VALUE_FWSLASH)
      {
        rfsfat_ucStrncpy((UINT8 *)temppath, (const UINT8*)path, (namesize));
      } else {
        rfsfat_ucStrcpy((UINT8 *)temppath, (const UINT8*)path);
      }
      //create valid EOSTR
      if(namesize < DIR_SIZE_META_EXT + DIR_SIZE_META_NAME)
      { 
        temppath[namesize] = ASCII_EOSTR;
      }
      //could be an 8.3 name
  	  /* Check given file/directory name. */
  	  if (!ffs_bIsNameValid (temppath))
  		{
  		  //not valid 8.3 --> LFN!
  		  return ret;
  		}
  	  else
  		{
  		  return FALSE;
  		}
  	} else {
      //name must contain at least one char!
      return FALSE;
  	}
	}
  //LFN!!
  return ret;
}

  //copy selected pathname to name_ext path.
  //verify if indicated file/dir name is valid
UINT16 rephr_lfn(UINT16 startpos, const UINT8* origpath, UINT8* name_ext, UINT16 *nrchars)
{
  INT16   slash = FFS_ASCII_VALUE_FWSLASH;
  UINT8*   slashpos;
  UINT16  namesize = 0;
  UINT16  cnt = 0;
  
  slashpos = (UINT8*)strchr((const char*)origpath, slash);
  if(slashpos != NULL) 
  {
    //dirname
    namesize = slashpos - origpath;
  } else {
    //filename
    namesize = strlen((const char*)origpath);
  }
  if(namesize != 0)
  {
    if((origpath[0] == FFS_ASCII_VALUE_FWSLASH) ||
		    (origpath[0] == EOSTR))
		{
	    //there is no valid dir or file 
	    return FFS_RV_ERR_INVALID_PATH;
		}
    for(cnt = 0; cnt < namesize; cnt++)
    {
      if(VALID_LFN_CHAR((origpath[cnt])) == TRUE)
      {  
        //the character is valid
        name_ext[cnt] = origpath[cnt];
        
      } else {

	RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: rephr_lfn  FFS_RV_ERR_INVALID_PARAMS ",origpath[cnt]);
        //this file/dir name is invalid
		  return FFS_RV_ERR_INVALID_PARAMS;
      }
    }
  }
  //place EOST
  name_ext[cnt] = EOSTR;
  *nrchars = (cnt + startpos);
  return FFS_NO_ERRORS;
}



#if (FFS_UNICODE == 1)

/**************************************************/
// Unicode      
/**************************************************/
#if 0
//copy selected pathname to name_ext path.
//verify if indicated file/dir name is valid
UINT16 rephr_lfn_uc( char* u8_name_p, char* lfn_name_p)
{
  UINT16  cnt = 0;
  UINT8   tstchar;
  UINT8   snf_cnt = 0;
  UINT8   dot_cnt=0;
  

  tstchar = lfn_name_p[0];

  if (tstchar == EOSTR)
  {
    //this is no valid dir or file name
    return FFS_RV_ERR_INVALID_PATH;
  }

  
  while((tstchar != EOSTR) && (snf_cnt < (DIR_SIZE_META_NAME + DIR_SIZE_META_EXT)))
  {
    if (VALID_SHORT_FN_CHAR( tstchar) == TRUE)
    {
      if ( tstchar == ASCII_VALUE_DOT)  // only one dot allowed
      {
        if (dot_cnt > 0)
        {
          continue;
        }
        dot_cnt++;
      }
      u8_name_p[snf_cnt] = tstchar;
      snf_cnt++;
    }
    tstchar = lfn_name_p[++cnt];
  }

  u8_name_p[snf_cnt] = EOSTR; //end with a terminator

  return FFS_NO_ERRORS;
}

#else

UINT16 rephr_lfn_uc( char* u8_name_p, char* lfn_name_p)
{
  UINT16  cnt = 0;
  UINT16  strlenght;
  UINT8   tstchar;
  UINT8   snf_cnt=0;
  UINT8   dotpos=0;
  UINT8   ext_cnt;
  

  if (lfn_name_p[0] == EOSTR)
  {
    //this is no valid dir or file name
    return FFS_RV_ERR_INVALID_PATH;
  }

  strlenght = strlen( lfn_name_p);

  //First find the last dot position
  dotpos = strlenght-1;
  while ((cnt < strlenght) && (lfn_name_p[dotpos] != ASCII_VALUE_DOT))
  {
    cnt++;
    dotpos--;
  }

  //Now start getting 8 SNF base characters
  cnt = 0;
  while((cnt <= dotpos) && (snf_cnt < DIR_SIZE_META_NAME))
  {
    tstchar = toupper(lfn_name_p[cnt++]);
    if (VALID_SHORT_FN_CHAR( tstchar) == FALSE)
    {
      tstchar = ASCII_VALUE_UNDERSCORE; //Invalid character indicator
    }
    else if (tstchar == ASCII_VALUE_DOT)
    { //skip dots
      continue;
    }
    u8_name_p[snf_cnt++] = tstchar;
  }

  //Check if there are characters for an extension left and the dot as present
  if ((cnt >= strlenght) || (lfn_name_p[dotpos] != ASCII_VALUE_DOT))
  {
    u8_name_p[snf_cnt] = EOSTR; //end with a terminator
    return FFS_NO_ERRORS;
  }

  //Now we try to get the extention part (incl. dot)
  ext_cnt = 0;
  while((cnt < strlenght) && (ext_cnt <= DIR_SIZE_META_EXT))
  {
    tstchar = toupper(lfn_name_p[dotpos++]);
    if (VALID_SHORT_FN_CHAR( tstchar) == FALSE)
    {
      tstchar = ASCII_VALUE_UNDERSCORE; //Invalid character indicator
    }
    u8_name_p[snf_cnt++] = tstchar;
    cnt++;
    ext_cnt++;
  }
  
  u8_name_p[snf_cnt] = EOSTR; //end with a terminator

  return FFS_NO_ERRORS;
}



void convertUcToSFN(const  T_WCHAR *mp_uc, UINT8 *mp)
{
   UINT16 cnt=0;
   
    cnt=0;
	while(mp_uc[cnt] != EOSTR)
		{
		   mp[cnt]=((char)(mp_uc[cnt]));
		   if(!(VALID_SHORT_FN_CHAR(mp[cnt])))
		   	{
		   	  mp[cnt] =ASCII_VALUE_UNDERSCORE; 
		   	}
		   cnt++;
		}
	mp[cnt]=EOSTR;
  
}

void convertU16ToU8(const  T_WCHAR *mp_uc, UINT8 *mp)
{
   UINT16 cnt=0;
   
    cnt=0;
	while(mp_uc[cnt] != EOSTR)
		{
		   mp[cnt]=((char)(mp_uc[cnt]));
		   cnt++;
		}
	mp[cnt]=EOSTR;
  
}


#endif





UINT8 ffs_ucGetPartAndDirTableFromPath_uc(
                        const T_WCHAR                     *pathName_p,
                        RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc)
{
  DIR_DIRECTORY_LABEL_STRUCT  stMetDat;
  const T_WCHAR        *workPath = pathName_p;
  T_WCHAR              ucElement;  
  UINT8                       ucResult; 

  /****************************************************************************/
  /* Some verification                                                        */
  /****************************************************************************/
  ucResult = MMA_ucGetPartitionStatus (pstLabLoc->mpt_idx);
  if (ucResult != FFS_RV_FORMATTED)
  {
    /* partition is not formatted. Return.                                     */
    RFSFAT_SEND_TRACE_ERROR ("RFSFAT: disk get partition status failed, Uc");
    return ucResult;
  }


  /****************************************************************************/
  /* Preparations                                                             */
  /****************************************************************************/
  pstLabLoc->tLblPos    = DIR_OFFSET_FIRST_ENTRY;
  pstLabLoc->uiFirstCl  = DIR_CLUSTER_NR_ROOT_DIRECTORY;

  


  /****************************************************************************/
  /* The body                                                                 */
  /****************************************************************************/


  // Check if root is requested: stop if so.
  if ((pathName_p == NULL) || (pathName_p[0] == EOSTR) || ((pathName_p[0] == FFS_ASCII_VALUE_FWSLASH ) &&(pathName_p[1] == EOSTR) ))
  {
    RFSFAT_SEND_TRACE("RFSFAT: FFS_RV_ROOTDIR_SELECTED, Uc ", RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ROOTDIR_SELECTED;
  }

  /* Ignore the leading '/' */
  workPath = pathName_p;
  ucElement = *workPath;
  
  while(ucElement == FFS_ASCII_VALUE_FWSLASH)
  	{
  	      workPath++;
		  ucElement = *workPath;
  	}	  

  if(ucElement == EOSTR)
  	{
  	 RFSFAT_SEND_TRACE("RFSFAT: FFS_RV_ROOTDIR_SELECTED ...1, Uc ",RV_TRACE_LEVEL_ERROR);
     return FFS_RV_ROOTDIR_SELECTED;
  	}


  //Try to find each directory in the path. Top -> down
  while (1)
  {
    
    /* find and get directory label. */
    ucResult = DIR_ucGetLabel_uc (workPath, pstLabLoc, &stMetDat);
    if (ucResult != FFS_NO_ERRORS)
    {
      if (ucResult == DIR_RV_DIR_NOT_FOUND)
      {
        RFSFAT_SEND_TRACE("RFSFAT: DIR_RV_DIR_NOT_FOUND, Uc ",RV_TRACE_LEVEL_ERROR);
        return DIR_RV_DIR_NOT_FOUND;
      }
      RFSFAT_SEND_TRACE("RFSFAT: DIR get label failed, Uc ",RV_TRACE_LEVEL_ERROR);
      return ucResult;
    }

    //set startof name behind '/' so we can check the next dir
    while ( (ucElement != FFS_ASCII_VALUE_FWSLASH) &&
            (ucElement != FFS_ASCII_EOL))
    {
      ucElement = *workPath;
      workPath++;
    }
    if (ucElement ==  FFS_ASCII_EOL)
    {
      return FFS_NO_ERRORS; // all done when at the end
    }

	/*There is more, so search from last hit */
    ucElement = *workPath; //for next 'while' search
    pstLabLoc->uiFirstCl = ((stMetDat.uiFirstClusterNumberHigh<<16) |(stMetDat.uiFirstClusterNumber));
  }

 /* statement not reachable */
 /*   return DIR_RV_DIR_NOT_FOUND; */
}




UINT8 ffs_ucGetLabel_uc(
                   const T_WCHAR                       *pathName_p,
                   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)   pstLabLoc,
                   RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)  pstMetDat)
{
  UINT8 ucResult;

  ucResult = ffs_ucGetPartAndDirTableFromPath_uc(pathName_p, pstLabLoc);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE("RFSFAT (Unicode): get partition path from dir failed",RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }
  //get metadata of 8+3 label (even inside LFN it points to 8+3 lbl)
  ucResult = DIR_ucGetMetaData (pstLabLoc, pstMetDat);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: get meta data failed");
    return ucResult;
  }
  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: exit ffs_ucGetLabel");
  return FFS_NO_ERRORS;
}


/**************************************************/
UINT8 E_FFS_ucDiskStat_uc(
           const T_WCHAR                       *pathName_p,
           RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)  label,
           UINT8                                      mp_idx)
{
  UINT8 ucResult;
  DIR_LABEL_LOCATION_STRUCT stLabLoc;

  RFSFAT_SEND_TRACE ("RFSFAT: E_FFS_ucDiskStat entered, UC",RV_TRACE_LEVEL_DEBUG_LOW);

  FFS_STOP_IF_NOT_INITIALISED
  /* Check pointer parameters) */
  if ((pathName_p == NULL) || (label == NULL))
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_INVALID_PARAMS, UC",RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_INVALID_PARAMS;
  }

  stLabLoc.mpt_idx = mp_idx;
  ucResult = ffs_ucGetLabel_uc (pathName_p, &stLabLoc, label);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: label not succesfully read, UC", RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  RFSFAT_SEND_TRACE ("RFSFAT: exit E_FFS_ucDiskStat, UC", RV_TRACE_LEVEL_DEBUG_LOW);

  return FFS_NO_ERRORS;
}


/**************************************************/
// end of Unicode      
/**************************************************/

#endif

  

