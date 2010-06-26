/**
 * @file  rfsfat_fam.c
 *
 * The FAT manager manages the FAT table and the copies of
 * the FAT tables of the current active partition.
 *
 * @author  Anton van Breemen
 * @version 0.1 This file inherited from an ICT re-use project
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  01/28/2004  Anton van Breemen   Create.
 *  04/29/2004  E. Oude Middendorp    FAT32 added.
 *  27/6/2006	Keshava Munegowda		FAT12 added.
 *  2/7/2006	Keshava Munegowda		FAT Cahing Revised.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

/***** Includes **************************************************************/

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"

#include <string.h>
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_pool_size.h"
#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfsfat/rfsfat_clm.h"
#include "rfsfat/rfsfat_fam.h"


/******************************************************************************
*      Module specific Defines and enums                                      *
******************************************************************************/
/* Define module ID string for debugging.                                     */
#ifdef DEBUG_TRACING
#undef  DEBUG_MOD_ID_STRING
#define DEBUG_MOD_ID_STRING                                              "FAM "
#endif

/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

//this define is used to speed up the format procedure. When formatted for FAT16
//the cluster size will 'normally' be 1024 Bytes. 6 clusters will then cause a buffer
//of approx. 6KB. For FAT32 the cluster size 'normally' is 512Bytes needing a 
//buffer of 3KB when this define is set to '6'. 
//When larger number of blocks are to be written to further increase the format 
//speed, the SWE's Pool size must be increased as well, in "rfsfat_pool_size.h" 
//To downsize the RAM usage this value can be as low as 1! But it will notably
//lenghten the format time.
#define RFSFAT_NR_FORMAT_BLKS 30


#define MAIN_FAT_TABLE  0   /* The original FAT table                     */
#ifdef ICT_SPEEDUP_RFSFAT2
#define FAM_CASH_SIZE (10240)
#define FATCACHE_DEF_BASE (0xFFFFFFFF)



#define RFSFATCACHE_SECTOR_BASE(mpt_idx)   (rfsfat_env_ctrl_blk_p->ffs_globals.fat_cache[mpt_idx].fatCashSectorBase)
#define RFSFATCACHE_IS_CHANGED(mpt_idx)    (rfsfat_env_ctrl_blk_p->ffs_globals.fat_cache[mpt_idx].fatCashChanged) 
#define RFSFATCACHE_BUFFER(mpt_idx)		   (rfsfat_env_ctrl_blk_p->ffs_globals.fat_cache[mpt_idx].pFamCash) 
#define RFSFATCACHE_FATENTRIES(mpt_idx)    (rfsfat_env_ctrl_blk_p->ffs_globals.fat_cache[mpt_idx].nmbFatEntriesInFamCash) 


#endif  //ifdef ICT_SPEEDUP_RFSFAT2
/******************************************************************************
*      Module specific Macros and Typedefs                                    *
******************************************************************************/

/******************************************************************************
*      Definition global Variables                                            *
******************************************************************************/

/******************************************************************************
*      Definition of static-Variables                                         *
******************************************************************************/

/******************************************************************************
*      Static-Function prototypes                                             *
******************************************************************************/
static UINT8 ucReadFATEntry (const UINT8 mpt_idx,
               const UINT8 ucFatTable,
               const UINT32 uiFatIndex,
               RO_PTR_TO_RW_XDATA (UINT32) puiFATLabel);

static UINT8 ucWriteFATEntry (const UINT8 mpt_idx,
                const UINT32 uiFatIndex,
                const UINT32 uiFATLabel);

static UINT8 ucLabelStatus (const UINT8 mpt_idx, const UINT32 uiFATLabel);

static UINT32 uiClusterDataSize (UINT8 mpt_idx);
#ifdef ICT_SPEEDUP_RFSFAT2

static UINT8 ucCheckIfFamCashNeedsFlush
  (const UINT8 mpt_idx,
   const UINT8 ucFatTable,
   const UINT32 uiFatIndex);


static UINT8 ucReadFATEntryCash
  (const UINT8 mpt_idx,
   const UINT8 ucFatTable,
   const UINT32 uiFatIndex, RO_PTR_TO_RW_XDATA (UINT32) puiFATLabel);

static UINT8 ucWriteFATEntryCash
  (const UINT8 mpt_idx,
   const UINT32 uiFatIndex, const UINT32 uiFATLabel);

#endif  //ifdef ICT_SPEEDUP_RFSFAT2

/******************************************************************************
*      Implementation of Global Function                                      *
******************************************************************************/

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucGetNextCluster
  (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) puiClusterNr)
/* PURPOSE      : Function gets the next cluster from a cluster-linked list.
 * ARGUMENTS    : mpt_idx index in mountpoint table where from media and part
 *                        can be determined.      
 * RETURNS      : FAM_RET_BAD_CLUSTER - The next cluster is marked as BAD
 *                                      cluster, reading this cluster is not
 *                                      advised.
 *                FAM_RET_FAT_TABLE_ERROR  - The read label contains an invalid
 *                                          cluster number.
 *                FAM_RET_LAST_CLUSTER - There are no clusters left in the chain,
 *                                       the current cluster is the last cluster.
 *                FAM_RET_RESERVED_CLU - The read cluster is a reserved cluster
 *                FAM_RET_FREE_CLUSTER - The read cluster is a free cluster
 *                FAM_RET_NO_FAT_CLUSTER - The FAT number is not valid
 *                FFS_NO_ERRORS - The next cluster is available.
 * PRECONDITION : partition must be formatted and media manager must be initialised
 * POSTCONDITION: Next cluster of a cluster linked list (chain) is determined.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT32 uiClusterLabel;
  UINT8 ucRetVal;

  ucRetVal = MMA_ucGetBootSectorInfo (mpt_idx);
  if (ucRetVal != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: MMA_ucGetBootSectorInfo  failed",
             RV_TRACE_LEVEL_ERROR);

    /* Error reading the boot-sector, return error type                       */
    return ucRetVal;
  }

  //For fat32 there is no root cluster.
  //So when a "next" entry is requested, this MUST be in data area.
  if (*puiClusterNr < FAM_START_DATA_CLU)
  {
    if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
    {
      *puiClusterNr = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
    }
  }

  /* Check if current cluster is within the cluster size                      */
  if (*puiClusterNr > uiClusterDataSize (mpt_idx))
  {

    RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_NO_FAT_CLUSTER ... FAM_ucGetNextCluster",
             RV_TRACE_LEVEL_ERROR);

#ifdef ICT_SPEEDUP_RFSFAT2 
   /* flush if you have changed the cash */
   ucFlushFamCash(mpt_idx);
#endif

    return FAM_RET_NO_FAT_CLUSTER;
  }

  ucRetVal =
  ucReadFATEntry (mpt_idx, MAIN_FAT_TABLE, *puiClusterNr, &uiClusterLabel);
  if (ucRetVal != FFS_NO_ERRORS)
  {

    if (ucRetVal == FAM_RET_ROOT_CLUSTER)
    {

      //root cluster!
      //RFSFAT_SEND_TRACE
      //("RFSFAT: ucReadFATEntry returned FAM_RET_ROOT_CLUSTER",
      // RV_TRACE_LEVEL_WARNING);
      return ucRetVal;
    }
    else
    {

      /* Read error from FAT table, error will be returned immediatly           */
      RFSFAT_SEND_TRACE ("RFSFAT: ucReadFATEntry failed",
               RV_TRACE_LEVEL_ERROR);
      return ucRetVal;
    }
  }

  /****************************************************************************
  * FAT table is read without errors, now determine next cluster in chain     *
  * by determine the label status of uiClusterLabel                           *
  *****************************************************************************/
  ucRetVal = ucLabelStatus (mpt_idx, uiClusterLabel);
  if (ucRetVal == FAM_RET_CLUSTER_NR)
  {
  /**************************************************************************
    * Next cluster is available                                               *
    * *puiClusterNr contains the next cluster number                          *
    ***************************************************************************/
    *puiClusterNr = uiClusterLabel;
    return FFS_NO_ERRORS;
  }
  /*
     The next cluster is not a normal cluster in the chain, the label status is
     returned and *puiClusterNr is not changed
   */
  return ucRetVal;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucCreateNewChain
  (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) puiClusterNr)
/* PURPOSE      : Function searches for a new cluster in the FAT table starting
 *                at cluster puiClusterNr is pointing at. If a free cluster is
 *                found; the cluster number is written in the variable
 *                puiClusterNr is pointing at and the last cluster label in
 *                the FAT table is written (end-label).
 * ARGUMENTS    : mpt_idx indicates the partition number.
 *                puiClusterNr is pointing at a variable to store the new cluster
 *                number.
 * RETURNS      : FFS_NO_ERRORS - Free cluster is found and its location is
 *                                stored in puiClusterNr.
 *                FAM_RET_FAT_NO_SPACE  - No free clusters left.
 *                FAM_RET_READ_ERROR    - Error occured during reading the FAT.
 *                FAM_WRITE_ERROR       - Error during writing the FAT entry.
 * PRECONDITION : partition must be formatted and media manager must be initialised
 * POSTCONDITION: A new chain is created in the FAT table
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT32 uiLoopCnt;
  UINT32 uiFatLabel, max_cluster_num;
  UINT8 ucRetVal;

  /*
     Determine max clusters in data area. This value is used to limit the search
     action in the FAT table by one complete loop
   */
  ucRetVal = MMA_ucGetBootSectorInfo (mpt_idx);
  if (ucRetVal != FFS_NO_ERRORS)
  {

    RFSFAT_SEND_TRACE ("RFSFAT: Error reading the boot-sector",
             RV_TRACE_LEVEL_ERROR);

    /* Error reading the boot-sector, return error type                       */
    return ucRetVal;
  }

  max_cluster_num = uiClusterDataSize (mpt_idx);

  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("The number of data cluster ", max_cluster_num);

  if(*puiClusterNr > max_cluster_num)
  	{
      RFSFAT_SEND_TRACE ("RFSFAT: Invalid start cluster Number",
                   RV_TRACE_LEVEL_ERROR);  	
  	   return FAM_RET_NO_FAT_CLUSTER;
  	}
  
  
  if ((*puiClusterNr == max_cluster_num) || (*puiClusterNr < FAM_START_DATA_CLU))
  {
    *puiClusterNr = FAM_START_DATA_CLU; //start from first data cluster
  }


  /* total number of cluster to read are max_cluster_num-1 */
  uiLoopCnt=max_cluster_num-1;

  while (uiLoopCnt--)
  {
    //
    ucRetVal = ucReadFATEntry (mpt_idx, MAIN_FAT_TABLE,
                 *puiClusterNr, &uiFatLabel);

    switch (ucRetVal)
    {
    case FFS_NO_ERRORS:
      {
      /* normal cluster in FAT table used or unused                         */

      /* Determine if cluster is free                                       */
      ucRetVal = ucLabelStatus (mpt_idx, uiFatLabel);
      if (ucRetVal == FAM_RET_FREE_CLUSTER)
        {
        /* current cluster is free                                          */
        /* mark cluster as end cluster (last cluster in chain)              */
        if ((ucRetVal= ucWriteFATEntry (mpt_idx, *puiClusterNr, FAM_LAST_CLUSTER(mpt_idx)))
          == FFS_NO_ERRORS)
          {
				/* Make the contents of this cluster as all zeros */
                /* TODO : Need to check */

#if FREE_SPACE_OPT

if(MPT_TABLE[mpt_idx].free_space != INVALID_FREE_SPACE_VAL)
{
      /* decrement the free_space cluster */
	  MPT_TABLE[mpt_idx].free_space--;   
   
}


#endif




				
          return FFS_NO_ERRORS;
          }

        RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_FAT_WRITE_ERROR",
                   RV_TRACE_LEVEL_ERROR);
        return ucRetVal;
        }
      }
      break;
    case FAM_RET_ROOT_CLUSTER:
      /* Fall trough                                                          */
    case FAM_RET_NO_FAT_CLUSTER:
      {
      /* outside the FAT table, start searching at begin of FAT table       */
      *puiClusterNr = (FAM_START_DATA_CLU - 1); //-1: it is incremented later
      }
	RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_NO_FAT_CLUSTER.... FAM_ucCreateNewChain",RV_TRACE_LEVEL_DEBUG_LOW);
      break;
    default:
      {
      /* Unknown return value or read error                                 */

      RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_FAT_READ_ERROR",
                 RV_TRACE_LEVEL_ERROR);
      return FAM_RET_FAT_READ_ERROR;
      }
    };

    /*increment the cluster number to be checked in the FAT table             */
    (*puiClusterNr)++;
    /* Additional safe code */
	if((*puiClusterNr) > max_cluster_num)
		{
		 *puiClusterNr = FAM_START_DATA_CLU;
		}
		
  };
  /* There is no free cluster available                                       */
  RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_FAT_NO_SPACE ", RV_TRACE_LEVEL_ERROR);

  return FAM_RET_FAT_NO_SPACE;
}


/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucAppendCluster
  (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) puiClusterNr,BOOL          clearNewDirCluster)
/* PURPOSE      : This function appends a new cluster to the cluster-linked list
 *                (chain) after cluster puiClusterNr is pointing at.
 *                After the new cluster is appended, puiClusterNr is pointing
 *                at the new cluster.
 * ARGUMENTS    : mpt_idx indicates the partition number.
 *                puiClusterNr points at the last cluster in the chain.
 * RETURNS      : FFS_NO_ERRORS - Cluster is append to linked list.
 *                FAM_RET_FAT_NO_SPACE    - Cluster could not be appended
 *                                          because there is no space left on
 *                                          the partition.
 *                FAM_RET_READ_ERROR      - Error occured during reading the FAT
 *                FAM_RET_ROOT_CLUSTER    - Error in Chain, pointing at ROOT
 *                FAM_RET_RESERVED_CLU    - Error in Chain, pointing at Res.Clu
 *                FAM_RET_FREE_CLUSTER    - Error in chain, pointing at FreeClu
 *                FAM_RET_BAD_CLUSTER     - Error in chain, reading a BAD Clu
 * PRECONDITION : partition must be formatted and media manager must be initialised.
 * POSTCONDITION: Cluster chain is extended.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT32 uiNewCluster;
  UINT8 ucRetVal;
  UINT16                  uiCnt;
  UINT16                  uiNmbDirEntriesInCluster;
  MMA_RW_STRUCT           stDirectRW={0,0,NULL};


  /* Determine last cluster in chain                                          */
  do
  {
    ucRetVal = FAM_ucGetNextCluster (mpt_idx, puiClusterNr);
  /**************************************************************************
    * The loop must continue if ucRetVal == FFS_NO_ERRORS, in this case the   *
    * Last cluster in the chain is not yet reached                            *
    ***************************************************************************/

    if ((ucRetVal != FAM_RET_LAST_CLUSTER) && (ucRetVal != FFS_NO_ERRORS))
    {
    /************************************************************************
      * Logical FAT table error, or a BAD cluster is read                     *
      * The next cluster must be a valid cluster or the last cluster in a     *
      * chain. In all other cases the chain is corrupt.                       *
      * Logical error occurs when the next cluster is a : Free, Reserved or   *
      * Root cluster.                                                         *
      *************************************************************************/

      RFSFAT_SEND_TRACE ("RFSFAT: Logical FAT table error, or a \
        BAD cluster is read", RV_TRACE_LEVEL_ERROR);
      /* Return the error code                                                */
      return ucRetVal;
    }
  }
  while (ucRetVal != FAM_RET_LAST_CLUSTER);

  /* Copy current cluster                                                     */
  uiNewCluster = *puiClusterNr;

  /* Reserve a new cluster in the FAT table after last cluster in chain       */
  ucRetVal = FAM_ucCreateNewChain (mpt_idx, &uiNewCluster);
  if (ucRetVal != FFS_NO_ERRORS)
  {
   RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucCreateNewChain failed", RV_TRACE_LEVEL_ERROR);
    /* FAT table error or disk is full                                        */
    return ucRetVal;
  }

  /* Write new retreived cluster in last cluster of the cluster chain         */
  ucRetVal = ucWriteFATEntry (mpt_idx, *puiClusterNr, uiNewCluster);

  /* Write new retreived cluster to puiClusterNr                              */

  *puiClusterNr = uiNewCluster;

  if (clearNewDirCluster)
  {
    //Clear new directory cluster to ensure that for each entry 
    // at least the first byte is cleared (LAST_ENTRY indicator). Other wise old
    //  data can (and will) be seen as entries (when adding new elements).
    uiCnt = (UINT16) (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster * 
                      MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
    uiNmbDirEntriesInCluster = uiCnt / DIR_SIZE_META_ALL;

    /* Copy label location of directory.                                        */  
    RFSFAT_GETBUF(DIR_SIZE_META_ALL,&stDirectRW.pucBuffer);
    (void) rfsfat_vpMemset(stDirectRW.pucBuffer, DEFAULT_DIR_FIRST_CLUSTER, DIR_SIZE_META_ALL);
    stDirectRW.tNrBytes = DIR_SIZE_META_ALL;
    stDirectRW.tOffset = 0;
    if (!(bClusterOffsetInVol (mpt_idx, uiNewCluster,((FFS_FILE_LENGTH_TYPE *) &(stDirectRW.tOffset)))))
		{
		 RFSFAT_SEND_TRACE_ERROR("RFSFAT: Cluster calucation failed ");
		 RFSFAT_FREE_BUF(stDirectRW.pucBuffer);
		 return CLM_RET_UNDEF_ERROR;
		}
    /* clean as many entries as fit in a cluster */
    for(uiCnt = 0; uiCnt < uiNmbDirEntriesInCluster; uiCnt++)
    {
      ucRetVal = MMA_ucDirectWrite (mpt_idx, &stDirectRW);
      if (ucRetVal != FFS_NO_ERRORS)
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: Dir Cluster Cleanup failed");
        RFSFAT_FREE_BUF(stDirectRW.pucBuffer);
        return ucRetVal;
      }
      stDirectRW.tOffset += DIR_SIZE_META_ALL;
    }
    RFSFAT_FREE_BUF(stDirectRW.pucBuffer);
  }


  return ucRetVal;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8
FAM_ucDelLinkedList (const UINT8 mpt_idx, const UINT32 uiClusterNr)
/* PURPOSE      : This function deletes/removes a cluster linked list (chain)
 *                starting at cluster uiClusterNr. The Clusters in the FAT
 *                table will be freed.
 * ARGUMENTS    : mpt_idx indicates the partition number.
 *                uiClusterNr is the start cluster of the linked list.
 * RETURNS      : FFS_NO_ERRORS        - Linked list is removed and clusters
 *                                       are freed.
 *                FAM_RET_BAD_CLUSTER  - The next cluster is marked as BAD
 *                                       cluster, reading this cluster is not
 *                                       advised.
 *                FAM_RET_FAT_TABLE_ERROR  - The read label contains an invalid
 *                                          cluster number.
 *                FAM_RET_RESERVED_CLU - The read cluster is a reserved cluster
 *                FAM_RET_FREE_CLUSTER - The read cluster is a free cluster
 *                FAM_RET_FAT_WRITE_ERROR - Could not delete cluster.
 * PRECONDITION : partition must be formatted and media manager must be initialised
 * POSTCONDITION: Removed clusters in the FAT table are freed.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT8 ucRetNextClu;
  UINT8 ucRetVal;
  UINT32 uiPreviousCluster;
  UINT32 uiNextCluster;

  ucRetNextClu = FFS_NO_ERRORS;
  /*
     Read label of uiClusterNr and write empty label to uiClusterNr
     The read label contains the end-label or the next cluster
   */
  uiPreviousCluster = uiClusterNr;
  uiNextCluster = uiClusterNr;

  while (ucRetNextClu != FAM_RET_LAST_CLUSTER)
  {
    ucRetNextClu = FAM_ucGetNextCluster (mpt_idx, &uiNextCluster);

    if ((ucRetNextClu != FFS_NO_ERRORS)
      && (ucRetNextClu != FAM_RET_LAST_CLUSTER))
    {
    /************************************************************************
      * Error in the Chain.                                                   *
      * Error caused by: BAD cluster or unexpected end in chain               *
      *************************************************************************/
      RFSFAT_SEND_TRACE
      ("RFSFAT: BAD cluster or unexpected end in chain ",
       RV_TRACE_LEVEL_ERROR);

      return ucRetNextClu;
    }
  /**************************************************************************
    * If ucRetNextCluster == FAM_RET_LAST_CLUSTER then the uiNextCluster is   *
    * the same as uiPreviousCluster, so when deleting uiPreviousCluster the   *
    * last entry in the chain is also deleted.                                *
    ***************************************************************************/
    /* Delete uiPreviousCluster                                               */
    ucRetVal =
    ucWriteFATEntry (mpt_idx, uiPreviousCluster, FAM_FREE_CLUSTER);
    if (ucRetVal != FFS_NO_ERRORS)
    {
      /* Error writing the FAT label                                          */
      RFSFAT_SEND_TRACE ("RFSFAT: Error writing the FAT label ",
               RV_TRACE_LEVEL_ERROR);
      return ucRetVal;
    }


#if FREE_SPACE_OPT

if(MPT_TABLE[mpt_idx].free_space != INVALID_FREE_SPACE_VAL)
{
      /* increment the free_space clusters */
	  MPT_TABLE[mpt_idx].free_space++;   
   
}


#endif

	

    /* Set current cluster to new cluster                                     */
    uiPreviousCluster = uiNextCluster;
  }
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucAssureClusterLength
  (const UINT8 mpt_idx,
   const UINT32 uiFirstClusterNumber,
   const FFS_FILE_LENGTH_TYPE tNewFileLength)
/* PURPOSE      : This function assures that the file is minimal tNewFileLength
 *                long.
 * ARGUMENTS    : mpt_idx index in mountpoint table.
 *                uiFirstClusterNumber: First cluster of a file.
 *                tNewFileLength: The minimum required file size.
 * RETURNS      : FFS_NO_ERRORS           - No errors
 *                MMA_RET_READ_ERROR      - Error reading Media manager
 *                FAM_RET_BAD_CLUSTER     - The next cluster is marked as BAD
 *                                          cluster, reading this cluster is not
 *                                          advised.
 *                FAM_RET_FAT_TABLE_ERROR  - The read label contains an invalid
 *                                          cluster number.
 *                FAM_RET_RESERVED_CLU    - The read cluster is a reserved cluster
 *                FAM_RET_FREE_CLUSTER    - The read cluster is a free cluster
 *                FAM_RET_NO_FAT_CLUSTER  - The FAT number is not valid
 *                FAM_RET_FAT_NO_SPACE    - Cluster could not be appended
 *                                          because there is no space left on
 *                                          the partition.
 *                FAM_RET_READ_ERROR      - Error occured during reading the FAT
 *                FAM_RET_ROOT_CLUSTER    - Error in Chain, pointing at ROOT
 * PRECONDITION : Media manager must be initialised.
 *                partition must be formatted.
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE: -
 *****************************************************************************/
{
  UINT8 ucRetVal;       /* Return value of called functions                   */
  UINT32 uiClusterNr;     /* The cluster number used by FAM_ucGetNextCluster    */
  UINT32 uiCurNrClusters;   /* The current number of clusters in the chain        */
  UINT32 uiMinClusters;     /* The minumum number of clusters needed to           */
  /* assure the file length                             */

  /* Read the boot-sector values in the glabal structure MMA_stBootSector     */
  ucRetVal = MMA_ucGetBootSectorInfo (mpt_idx);
  if (ucRetVal != FFS_NO_ERRORS)
  {

    RFSFAT_SEND_TRACE ("RFSFAT: MMA_ucGetBootSectorInfo failed",
             RV_TRACE_LEVEL_ERROR);

    /* return the Error type                                                  */
    return ucRetVal;
  }

  /* Determine the minumum number of clusters needed for the file-size        */
  uiMinClusters = tNewFileLength / (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
                  MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);

  /* If modulo is not zero than increment uiMinClusters                       */
  if (((UINT16) tNewFileLength % (UINT16) (MMA_BOOT_SECTOR[mpt_idx].
                       ucSectorPerCluster *
                       MMA_BOOT_SECTOR[mpt_idx].
                       uiBytesPerSector)) != 0)
  {
    uiMinClusters++;
  }
  /* Determine current number of clusters in the chain                        */
  uiCurNrClusters = 0;
  uiClusterNr = uiFirstClusterNumber;
  do
  {
    /* uiClusterNr contains the last read cluster                             */
    ucRetVal = FAM_ucGetNextCluster (mpt_idx, &uiClusterNr);
    if ((ucRetVal != FFS_NO_ERRORS) && (ucRetVal != FAM_RET_LAST_CLUSTER))
    {
      RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucGetNextCluster failed",
               RV_TRACE_LEVEL_ERROR);
      return ucRetVal;
    }
    /* Increment then number of clusters in the chain                         */
    uiCurNrClusters++;
  }
  while (ucRetVal != FAM_RET_LAST_CLUSTER);

  while (uiCurNrClusters < uiMinClusters)
  {
    ucRetVal = FAM_ucAppendCluster (mpt_idx, &uiClusterNr,TRUE);
    if (ucRetVal != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucAppendCluster failed",
               RV_TRACE_LEVEL_ERROR);
      return ucRetVal;
    }
    uiCurNrClusters++;
  }
  return FFS_NO_ERRORS;
}



/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucNrDataclusters(const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulNrOfClusters)
/* PURPOSE      : This function number of data clusters.
 * ARGUMENTS    : mpt_idx: partition indication.
 *                uiFirstClusterNumber: First cluster of a file.
 *                puiNrOfBytes: The number of free bytes
 * RETURNS      : FFS_NO_ERRORS         - No errors
 *
 * PRECONDITION : Media manager must be initialised.
 *                partition must be formatted.
 * POSTCONDITION: A number
 * ERROR AND EXCEPTION HANDLING:
 * NOTE:
 *****************************************************************************/
{


   /* total number of data clusters*/
  /*last cluster number  - 1 */
  /* Example : If the last cluster number is 5, 
         	   and the first data cluster is 2, then starting from 2 (including cluster 2) 
         	   we have 4 data clusters */
  *pulNrOfClusters = ( uiClusterDataSize(mpt_idx) - 1); 
               

  return FFS_NO_ERRORS;

}





/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucNrFreeclusters (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulNrOfClusters)
/* PURPOSE      : This function calculates the number of free bytes.
 * ARGUMENTS    : mpt_idx: partition indication.
 *                pulNrOfClusters: Number of clusters.

 * RETURNS      : FFS_NO_ERRORS         - No errors
 *
 * PRECONDITION : Media manager must be initialised.
 *                partition must be formatted.
 * POSTCONDITION: A number
 * ERROR AND EXCEPTION HANDLING:
 * NOTE:
 *****************************************************************************/
{

  UINT32 uiLabel;       /* Label of the FAT table cluster number            */
  UINT32 uiClusterIndex;    /* Variable used as index in the FAT table          */
  UINT32 uiDataSize;      /* Data size in clusters                            */
  UINT8 ucRetVal = FFS_NO_ERRORS;       /* Return value of called functions                 */

  /*
     Start beginning at cluster FAM_START_DATA_CLU and loop until the last cluster
     of the FAT table is read
   */
  *pulNrOfClusters = 0;
  uiDataSize = uiClusterDataSize (mpt_idx);
  for (uiClusterIndex = FAM_START_DATA_CLU;
     uiClusterIndex <= uiDataSize; uiClusterIndex++)
  {
    ucRetVal =
    ucReadFATEntry (mpt_idx, MAIN_FAT_TABLE, uiClusterIndex, &uiLabel);
    if (ucRetVal != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE ("RFSFAT: Read Error in FAT table",
               RV_TRACE_LEVEL_ERROR);
      /* Read Error in FAT table                                              */
      return ucRetVal;
    }
    if (uiLabel == FAM_FREE_CLUSTER)
    {
      (*pulNrOfClusters)++;
    }
  }

  return ucRetVal;

}





/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucNrFreeBytes
  (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulNrOfBytes)
/* PURPOSE      : This function calculates the number of free bytes.
 * ARGUMENTS    : mpt_idx: partition indication.
 *                uiFirstClusterNumber: First cluster of a file.
 *                puiNrOfBytes: The number of free bytes
 * RETURNS      : FFS_NO_ERRORS         - No errors
 *                MMA_RET_READ_ERROR    - Error reading Media manager
 *                FAM_RET_FAT_READ_ERROR - Error reading FAT label
 *                FAM_RET_NO_FAT_CLUSTER - Not a valid FAT cluster number
 *                FAM_RET_ROOT_CLUSTER   - The read FAT entry is a ROOT cluster
 *
 * PRECONDITION : Media manager must be initialised.
 *                partition must be formatted.
 * POSTCONDITION: A number
 * ERROR AND EXCEPTION HANDLING:
 * NOTE:
 *****************************************************************************/
{

  UINT8 ucRetVal = FFS_NO_ERRORS;       /* Return value of called functions                 */
  UINT32 ucNrFreeClusters=0; 

  ucRetVal = MMA_ucGetBootSectorInfo (mpt_idx);
  if (ucRetVal != FFS_NO_ERRORS)
  {

    RFSFAT_SEND_TRACE
    ("RFSFAT: Error reading bootsector, return 0 sectors free",
     RV_TRACE_LEVEL_ERROR);

    /* Error reading bootsector, return 0 sectors free                        */
    return ucRetVal;
  }


#if FREE_SPACE_OPT  

  if( MPT_TABLE[mpt_idx].free_space ==INVALID_FREE_SPACE_VAL)
  	{
  	 ucRetVal = FAM_ucNrFreeclusters(mpt_idx,&ucNrFreeClusters); 

	 /* If there is no error, cache the free clusters */
     if (ucRetVal == FFS_NO_ERRORS)
     { 
       MPT_TABLE[mpt_idx].free_space = ucNrFreeClusters;
        
     	}
	 
  	}
  else
  	{
  	  /* Free cluster count is already avilable */
  	  ucNrFreeClusters =  MPT_TABLE[mpt_idx].free_space;    
  	}
  

#else
  ucRetVal = FAM_ucNrFreeclusters(mpt_idx,&ucNrFreeClusters);

#endif  

 if (ucRetVal != FFS_NO_ERRORS)
  {

    RFSFAT_SEND_TRACE("RFSFAT: Error in function FAM_ucNrFreeclusters", RV_TRACE_LEVEL_ERROR);

    /* Error reading bootsector, return 0 sectors free                        */
    return ucRetVal;
  }


  *pulNrOfBytes = (UINT32) (MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector *
              MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
              ucNrFreeClusters);
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8
FAM_ucMarkBadCluster (const UINT8 mpt_idx, const UINT32 uiClusterNumber)
/* PURPOSE      : Mark a cluster as BAD and remove cluster linked list after
 *                the BAD cluster.
 * ARGUMENTS    : mpt_idx: The partition number
 *                uiClusterNumber: The cluster number to mark as BAD cluster
 * RETURNS      : FFS_NO_ERRORS
 * PRECONDITION : -
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE: -
 *****************************************************************************/
{
  UINT8 ucRetVal;

  /* Delete linked list after the BAD cluster.                                */
  ucRetVal = FAM_ucDelLinkedList (mpt_idx, uiClusterNumber);
  if (ucRetVal != FFS_NO_ERRORS)
  {
    /* Unexpected end in list, maybe the list is not completely removed       */
    RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucDelLinkedList failed",
             RV_TRACE_LEVEL_ERROR);
    return ucRetVal;
  }

  ucRetVal = ucWriteFATEntry (mpt_idx, uiClusterNumber, FAM_BAD_CLUSTER(mpt_idx));
  return ucRetVal;
}

/******************************************************************************
*      Implementation of Local (= static) Function                            *
******************************************************************************/

#ifdef ICT_SPEEDUP_RFSFAT2

void Reset_FAT_TABLE_cache(UINT8 mpt_idx)
{

 if(mpt_idx < GBI_MAX_NR_OF_PARTITIONS)
 	{
  		RFSFATCACHE_SECTOR_BASE(mpt_idx) = FATCACHE_DEF_BASE;
  		RFSFATCACHE_IS_CHANGED(mpt_idx)  = FALSE;
  		RFSFATCACHE_FATENTRIES(mpt_idx)  = 0;
 	}
 
}




void FAM_ucFreeFATcash()
{
 UINT8 mpt_id;

 /* clear the cache of all the partitions */
for(mpt_id=0;mpt_id<GBI_MAX_NR_OF_PARTITIONS;mpt_id++)
  {
  	Reset_FAT_TABLE_cache(mpt_id);
  	if (RFSFATCACHE_BUFFER(mpt_id) != NULL) //Get the cash ones
  		{
    	RFSFAT_FREE_BUF(RFSFATCACHE_BUFFER(mpt_id));
		RFSFAT_SEND_TRACE ("RFSFAT: Freeing FAM CASH", RV_TRACE_LEVEL_DEBUG_LOW);
   	}
 }
}


void FAM_ucInitFATcash()
{
 UINT8 mpt_id;

 /* clear the cache of all the partitions */
for(mpt_id=0;mpt_id<GBI_MAX_NR_OF_PARTITIONS;mpt_id++)
  {
  	Reset_FAT_TABLE_cache(mpt_id);
  	RFSFATCACHE_BUFFER(mpt_id) = NULL;
   }
}




/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 ucFlushFamCash(UINT8 mpt_idx)
{
  UINT8         ucStatus = FFS_NO_ERRORS;
  MMA_RW_STRUCT stWrite={0,0,NULL};    /* Structure with write information                 */
  UINT8     ucFatTableNr;   /* Number indicates which FAT table to use*/

  RFSFAT_SEND_TRACE ("RFSFAT: Flushing FAM CASH", RV_TRACE_LEVEL_DEBUG_LOW);

  if(mpt_idx >= GBI_MAX_NR_OF_PARTITIONS)
  	{
  	  return DIR_RV_INVALID_PARAM;     /* Invalid parameter */
  	}


  if (RFSFATCACHE_BUFFER(mpt_idx) == NULL) 
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FAM CASH is not yet existing ", RV_TRACE_LEVEL_DEBUG_LOW);
    return ucStatus;   /* return success */
  }


  if (RFSFATCACHE_IS_CHANGED(mpt_idx) == FALSE)
  {
    
	RFSFAT_SEND_TRACE ("RFSFAT: FAM CASH not changed \n\r", RV_TRACE_LEVEL_DEBUG_LOW);
    return ucStatus; // Fat table Cash has not been changed
  }

  if (RFSFATCACHE_FATENTRIES(mpt_idx) == 0)
  {
   	RFSFAT_SEND_TRACE ("RFSFAT: FAM CASH old_mpt_idx_FamCash is FATCASH_EMPTY \n\r", RV_TRACE_LEVEL_DEBUG_LOW);
    return ucStatus; //First time, unused
  }
  
  /* Writing to the fat table and its copies */
  stWrite.pucBuffer = (UINT8 *) RFSFATCACHE_BUFFER(mpt_idx);
  stWrite.tNrBytes =  RFSFATCACHE_FATENTRIES(mpt_idx) * MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;
  for (ucFatTableNr = 0;
     ucFatTableNr < MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables; ucFatTableNr++)
  {
    /* Determine offset of the FAT table                                        */
    stWrite.tOffset = ((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors +
             (ucFatTableNr * MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT)) *
            MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) +
           (RFSFATCACHE_SECTOR_BASE(mpt_idx) * MMA_BOOT_SECTOR[mpt_idx].fatEntrySize);

    RFSFAT_SEND_TRACE_PARAM("RFSFAT:   Write FAT CASH tOffset  :", stWrite.tOffset, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT:   Write FAT CASH tNrBytes :", stWrite.tNrBytes, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT:   Write FAT CASH pucBuffer:", (UINT32)stWrite.pucBuffer, RV_TRACE_LEVEL_DEBUG_LOW);

    if (MMA_ucDirectWrite (mpt_idx, &stWrite) != FFS_NO_ERRORS)
    {
      ucStatus = FAM_RET_FAT_WRITE_ERROR;
      break;
    }
  }


  RFSFAT_SEND_TRACE ("RFSFAT: Flushing FAM CASH..exit", RV_TRACE_LEVEL_DEBUG_LOW);
  

  RFSFATCACHE_IS_CHANGED(mpt_idx) = FALSE;  // virgin Cash
  return ucStatus;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT8 ucCheckIfFamCashNeedsFlush
  (const UINT8 mpt_idx,
   const UINT8 ucFatTable,
   const UINT32 uiFatIndex)
{
  BOOL          bFlush = FALSE;
  UINT8         ucStatus = FFS_NO_ERRORS;
  MMA_RW_STRUCT stRead={0,0,NULL};     /* Structure with read information*/
  UINT32        fatEntriesPerSector;
  UINT32        NewFatCashSectorBase;
  UINT32        nmbFatEntriesInFamCash_new;
  UINT32  		numSectorsAtIndex;
  UINT32        NewNmbSectorsInFatCash=0;


  if(mpt_idx >= GBI_MAX_NR_OF_PARTITIONS)
  	{
  	  return DIR_RV_INVALID_PARAM;     /* Invalid parameter */
  	}


  
  if ((RFSFATCACHE_FATENTRIES(mpt_idx)==0) ||           /* Unfilled FAT cache , fill this */
  	 (uiFatIndex < RFSFATCACHE_SECTOR_BASE(mpt_idx) ) ||  /* below the FAT cache */
  	 (uiFatIndex >= (RFSFATCACHE_SECTOR_BASE(mpt_idx)+RFSFATCACHE_FATENTRIES(mpt_idx)))) /* above the fat cache */
  {
    //outside cash
    bFlush = TRUE;
    RFSFAT_SEND_TRACE ("RFSFAT: Outside FAM CASH", RV_TRACE_LEVEL_DEBUG_LOW);
  }

  if ((bFlush==TRUE) || (RFSFATCACHE_BUFFER(mpt_idx) == NULL))
  {
   fatEntriesPerSector = MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector / MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;

   NewNmbSectorsInFatCash = FAM_CASH_SIZE/ MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;

   numSectorsAtIndex = ((uiFatIndex* MMA_BOOT_SECTOR[mpt_idx].fatEntrySize)/(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector));

   NewFatCashSectorBase = (numSectorsAtIndex*MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector)/MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;

	
   if(NewNmbSectorsInFatCash >= MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT)
  	{
  	     /* This could happen , if media size is too small */
		 NewNmbSectorsInFatCash = MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT;
		 NewFatCashSectorBase = 0;
  	}
   else
   	{
     if((numSectorsAtIndex+NewNmbSectorsInFatCash) > MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT )
  	 {
  	     /* The FAT cache sholud not cover more than  Total number of FAT sectors */ 
  	    NewNmbSectorsInFatCash = (MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT-numSectorsAtIndex);
  	 }
   	} 
  
    nmbFatEntriesInFamCash_new = fatEntriesPerSector * NewNmbSectorsInFatCash;
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush fatEntriesPerSector", fatEntriesPerSector, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush NewNmbSectorsInFatCash", NewNmbSectorsInFatCash, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush uiFatIndex", uiFatIndex, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush OLD: fatCashSectorBase", RFSFATCACHE_SECTOR_BASE(mpt_idx), RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush NEW: NewFatCashSectorBase", NewFatCashSectorBase, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush OLD: nmbFatEntriesInFamCash", RFSFATCACHE_FATENTRIES(mpt_idx), RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT: FAM CASH Flush NEW: nmbFatEntriesInFamCash_new", nmbFatEntriesInFamCash_new, RV_TRACE_LEVEL_DEBUG_LOW);
    if (RFSFATCACHE_BUFFER(mpt_idx) == NULL) //Get the cash ones
    {
      RFSFAT_GETBUF(FAM_CASH_SIZE, (&RFSFATCACHE_BUFFER(mpt_idx)));
      RFSFAT_SEND_TRACE ("RFSFAT: Creating FAM CASH", RV_TRACE_LEVEL_DEBUG_LOW);
    }
    else
    {
      ucStatus = ucFlushFamCash(mpt_idx);
    }
    
    if (FFS_NO_ERRORS == ucStatus)
    {
      // ReadFamCash(fatCashSectorBase)
      stRead.pucBuffer = (UINT8 *) RFSFATCACHE_BUFFER(mpt_idx);
      stRead.tNrBytes =  nmbFatEntriesInFamCash_new * MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;

      /* Determine offset of the FAT table                                        */
      stRead.tOffset = ((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors +
               (ucFatTable * MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT)) *
              MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) +
             (NewFatCashSectorBase * MMA_BOOT_SECTOR[mpt_idx].fatEntrySize);

      RFSFAT_SEND_TRACE_PARAM("RFSFAT:   Read FAT CASH tOffset  :", stRead.tOffset, RV_TRACE_LEVEL_DEBUG_LOW);
      RFSFAT_SEND_TRACE_PARAM("RFSFAT:   Read FAT CASH tNrBytes :", stRead.tNrBytes, RV_TRACE_LEVEL_DEBUG_LOW);
      RFSFAT_SEND_TRACE_PARAM("RFSFAT:   Read FAT CASH pucBuffer:", (UINT32)stRead.pucBuffer, RV_TRACE_LEVEL_DEBUG_LOW);

      if (MMA_ucDirectRead (mpt_idx, &stRead) != FFS_NO_ERRORS)
      {
        ucStatus = FAM_RET_FAT_READ_ERROR;
      }
      else
      {
        RFSFATCACHE_SECTOR_BASE(mpt_idx) = NewFatCashSectorBase;  //New base from now    
        RFSFATCACHE_FATENTRIES(mpt_idx)   = nmbFatEntriesInFamCash_new;
      }
    }
  }

  return ucStatus;
}


/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT8 ucReadFATEntryCash
  (const UINT8 mpt_idx,
   const UINT8 ucFatTable,
   const UINT32 uiFatIndex, RO_PTR_TO_RW_XDATA (UINT32) puiFATLabel)
{
  UINT8         ucStatus = FFS_NO_ERRORS;
  UINT16        labelOffsetInBytes;

  if(mpt_idx >= GBI_MAX_NR_OF_PARTITIONS)
  	{
  	  return DIR_RV_INVALID_PARAM;     /* Invalid parameter */
  	}


  
  ucStatus = ucCheckIfFamCashNeedsFlush( mpt_idx, ucFatTable, uiFatIndex);
  if (FFS_NO_ERRORS != ucStatus)
  {
    return ucStatus;
  }
  
  *puiFATLabel = 0; /* Preset incase not full UINT32 size is read */

  labelOffsetInBytes = (uiFatIndex - RFSFATCACHE_SECTOR_BASE(mpt_idx)) *
                        MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;
  
  memcpy( (void*)puiFATLabel,
          (const void*)(&(RFSFATCACHE_BUFFER(mpt_idx)[labelOffsetInBytes])),
          MMA_BOOT_SECTOR[mpt_idx].fatEntrySize);

  return ucStatus;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT8 ucWriteFATEntryCash
  (const UINT8 mpt_idx,
   const UINT32 uiFatIndex, const UINT32 uiFATLabel)
{
  UINT8         ucStatus = FFS_NO_ERRORS;
  UINT16        labelOffsetInBytes;


  if(mpt_idx >= GBI_MAX_NR_OF_PARTITIONS)
  	{
  	  return DIR_RV_INVALID_PARAM;     /* Invalid parameter */
  	}

  
  ucStatus = ucCheckIfFamCashNeedsFlush( mpt_idx, MAIN_FAT_TABLE, uiFatIndex);
  if (FFS_NO_ERRORS != ucStatus)
  {
    return ucStatus;
  }
  

  labelOffsetInBytes = (uiFatIndex - RFSFATCACHE_SECTOR_BASE(mpt_idx)) *
                        MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;
  
  memcpy( (void*)(&(RFSFATCACHE_BUFFER(mpt_idx)[labelOffsetInBytes])),
          (const void*)&uiFATLabel,
          MMA_BOOT_SECTOR[mpt_idx].fatEntrySize);

  RFSFATCACHE_IS_CHANGED(mpt_idx) = TRUE;
  
  return ucStatus;
}




#endif //ifdef ICT_SPEEDUP_RFSFAT2


/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT8 ucReadFATEntry
  (const UINT8 mpt_idx,
   const UINT8 ucFatTable,
   const UINT32 uiFatIndex, RO_PTR_TO_RW_XDATA (UINT32) puiFATLabel)
/* PURPOSE      : Function gets the FAT label from the FAT table using the FAT
 *                table index number.
 * ARGUMENTS    : mpt_idx indicates the partition number.
 *                ucFatTable indicates the FAT table to read (nr 0..x).
 *                uiFatIndex is the FAT table index (is the same as cluster nr.)
 *                puiFATLabel is the label of the FAT table entry.
 * RETURNS      : FFS_NO_ERRORS          - The FAT label is read
 *                FAM_RET_FAT_READ_ERROR - Error reading FAT label
 *                FAM_RET_NO_FAT_CLUSTER - Not a valid FAT cluster number
 *                FAM_RET_ROOT_CLUSTER   - The read FAT entry is a ROOT cluster
 *                MMA_READ_ERROR         - Error reading Media manager
 *
 * PRECONDITION : Bootsector must be loaded
 *                uiFatIndex must be in range of the FAT table.
 *
 * POSTCONDITION: None
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT8         ucStatus = FFS_NO_ERRORS;
  UINT32 		uiFatIndex_phy=0;  /* physical FAT index */
  UINT32		puiFATLabel_tmp1=0,puiFATLabel_tmp2=0; 
  /* Temporary variable to read the FAT entry */
  /* This array variables might solve the endian problems */

  if (uiFatIndex < FAM_START_DATA_CLU)
  {

    RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_ROOT_CLUSTER", RV_TRACE_LEVEL_WARNING);

    return FAM_RET_ROOT_CLUSTER;
  }


  if (uiFatIndex > uiClusterDataSize (mpt_idx))
  {

    /* Cluster number not valid */
    RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_NO_FAT_CLUSTER ... ucReadFATEntry",
             RV_TRACE_LEVEL_ERROR);

    return FAM_RET_NO_FAT_CLUSTER;
  }
  

#ifdef ICT_SPEEDUP_RFSFAT2
if(MMA_FS_TYPE(mpt_idx) != GBI_FAT12)
{
  return ucReadFATEntryCash( mpt_idx, ucFatTable, uiFatIndex, puiFATLabel);
}
else
{
    /* Reading FAT12 entry  */
	uiFatIndex_phy = uiFatIndex + (uiFatIndex/2);

	ucStatus = ucReadFATEntryCash( mpt_idx, ucFatTable, uiFatIndex_phy+1, &puiFATLabel_tmp2);
	if(ucStatus != FFS_NO_ERRORS)
	{
		RFSFAT_SEND_TRACE ("RFSFAT: ucReadFATEntryCash failed, uiFatIndex_phy+1 ", RV_TRACE_LEVEL_ERROR);
		return ucStatus;
	}	

	ucStatus = ucReadFATEntryCash( mpt_idx, ucFatTable, uiFatIndex_phy, &puiFATLabel_tmp1);
	if(ucStatus != FFS_NO_ERRORS)
	{
		RFSFAT_SEND_TRACE ("RFSFAT: ucReadFATEntryCash failed, uiFatIndex_phy+1 ", RV_TRACE_LEVEL_ERROR);
		return ucStatus;
	}
	
	
	if(uiFatIndex&0x01)
		{
		    /* Given FAT index or cluster number is odd */
			(*puiFATLabel) = puiFATLabel_tmp2;
			(*puiFATLabel) <<=4;
			(*puiFATLabel) |= ((puiFATLabel_tmp1 & 0x0F0)>>4);
			
		}
	else
		{
		    /* Given FAT index or cluster number is even */
			(*puiFATLabel) = (puiFATLabel_tmp2 & 0x0F);
			(*puiFATLabel) <<= 8;
			(*puiFATLabel) |= puiFATLabel_tmp1;
		}
	return FFS_NO_ERRORS;
}
#else //ifdef ICT_SPEEDUP_RFSFAT2
  *puiFATLabel = 0; /* Preset incase not full UINT32 size is read */
  stRead.pucBuffer = (UINT8 *) puiFATLabel;
  stRead.tNrBytes =  MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;

  /* Determine offset of the FAT table                                        */
  stRead.tOffset = ((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors +
           (ucFatTable * MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT)) *
          MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) +
  (uiFatIndex * MMA_BOOT_SECTOR[mpt_idx].fatEntrySize);

  if (MMA_ucDirectRead (mpt_idx, &stRead) == FFS_NO_ERRORS)
  {
    /* FAT cluster is read succesfull                                         */
    return FFS_NO_ERRORS;
  }
  return FAM_RET_FAT_READ_ERROR;
#endif //ifdef ICT_SPEEDUP_RFSFAT2
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT8 ucWriteFATEntry
  (const UINT8 mpt_idx, const UINT32 uiFatIndex, const UINT32 uiFATLabel)
/* PURPOSE      : Function writes the FAT label of a FAT entry using the FAT
 *                table index number.
 * ARGUMENTS    : mpt_idx indicates the partition number
 *                uiFatIndex is the FAT table index (is the same as cluster nr.)
 *                puiFATLabel is the label of the FAT table entry. (ie cluster
 *                            marker or cluster number)
 * RETURNS      : FFS_NO_ERRORS - The FAT label is read
 *                FAM_RET_FAT_WRITE_ERROR - Error occurred while writing the
 *                FAT label.
 *                FAM_RET_NO_FAT_CLUSTER - Attempting to write to a non-FAT cluster
 *                MMA_RET_READ_ERROR - Error reading bootsector.
 * PRECONDITION : partition must be formatted and media manager must be initialised
 * POSTCONDITION: None
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT8         ucStatus = FFS_NO_ERRORS;
  UINT32 		uiFatIndex_phy=0;  /* physical FAT index */
  UINT32		puiFATLabel_tmp1=0,puiFATLabel_tmp2=0; 

  /* Determine max number of clusters                                         */
  if (uiFatIndex > uiClusterDataSize (mpt_idx))
  {

#ifdef ICT_SPEEDUP_RFSFAT2 
  
    ucFlushFamCash(mpt_idx);

#endif


    /* Cluster number not valid */
    RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_NO_FAT_CLUSTER ... ucWriteFATEntry",
             RV_TRACE_LEVEL_ERROR);

    return FAM_RET_NO_FAT_CLUSTER;
  }

#ifdef ICT_SPEEDUP_RFSFAT2
if(MMA_FS_TYPE(mpt_idx) != GBI_FAT12)
{
  if((MMA_BOOT_SECTOR[mpt_idx].fatEntrySize != sizeof (UINT16)) && (MMA_BOOT_SECTOR[mpt_idx].fatEntrySize != sizeof (UINT32)))
  	{
  	    /* Invalid FAT Entry Size */
  	    return FAM_RET_FAT_READ_ERROR;
  	}
  return ucWriteFATEntryCash( mpt_idx, uiFatIndex, uiFATLabel);
}
else
{
  /* Reading/Writing FAT12 entry  */
	uiFatIndex_phy = uiFatIndex + (uiFatIndex/2);

	ucStatus = ucReadFATEntryCash( mpt_idx, MAIN_FAT_TABLE, uiFatIndex_phy+1, &puiFATLabel_tmp2);
	if(ucStatus != FFS_NO_ERRORS)
	{
		RFSFAT_SEND_TRACE ("RFSFAT: ucReadFATEntryCash failed, uiFatIndex_phy+1 ", RV_TRACE_LEVEL_ERROR);
	
		return ucStatus;
	}	

	ucStatus = ucReadFATEntryCash( mpt_idx, MAIN_FAT_TABLE, uiFatIndex_phy, &puiFATLabel_tmp1);
	if(ucStatus != FFS_NO_ERRORS)
	{
	RFSFAT_SEND_TRACE ("RFSFAT: ucReadFATEntryCash failed, uiFatIndex_phy ", RV_TRACE_LEVEL_ERROR);
		return ucStatus;
	}
	
	if(uiFatIndex&0x01)
		{
		    /* Given FAT index or cluster number is odd */
			puiFATLabel_tmp1 &= 0x0F;
			puiFATLabel_tmp1 |= ((uiFATLabel & 0x00F)<<4);
			puiFATLabel_tmp2 = ((uiFATLabel & 0x0FF0)>>4);
		}
	else
		{
		    /* Given FAT index or cluster number is even */
			puiFATLabel_tmp2 &= 0x0F0;
			puiFATLabel_tmp2 |= ((uiFATLabel & 0x0F00)>>8);
			puiFATLabel_tmp1 = (uiFATLabel & 0x0FF);
		
		}

	ucStatus = ucWriteFATEntryCash( mpt_idx,  uiFatIndex_phy,puiFATLabel_tmp1 );
	if(ucStatus != FFS_NO_ERRORS)
	{
	RFSFAT_SEND_TRACE ("RFSFAT: ucWriteFATEntryCash failed, uiFatIndex_phy ", RV_TRACE_LEVEL_ERROR);
		return ucStatus;
	}

	ucStatus = ucWriteFATEntryCash( mpt_idx, uiFatIndex_phy+1,puiFATLabel_tmp2 );
	if(ucStatus != FFS_NO_ERRORS)
	{
	RFSFAT_SEND_TRACE ("RFSFAT: ucWriteFATEntryCash failed, uiFatIndex_phy+1 ", RV_TRACE_LEVEL_ERROR);
		return ucStatus;
	}	
	return FFS_NO_ERRORS;
}
#else //ifdef ICT_SPEEDUP_RFSFAT2
  stWrite.pucBuffer = (UINT8 *) & uiFATLabel;
  stWrite.tNrBytes = MMA_BOOT_SECTOR[mpt_idx].fatEntrySize;

  /* Writing to the fat table and its copies                                  */
  for (ucFatTableNr = 0;
     ucFatTableNr < MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables; ucFatTableNr++)
  {
    /* Determine offset of the FAT table                                      */

    stWrite.tOffset = ((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors +
              (ucFatTableNr *
               MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT)) *
             MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) +
    (uiFatIndex * MMA_BOOT_SECTOR[mpt_idx].fatEntrySize);

    if (MMA_ucDirectWrite (mpt_idx, &stWrite) != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_FAT_WRITE_ERROR",
               RV_TRACE_LEVEL_ERROR);
      return FAM_RET_FAT_WRITE_ERROR;
    }
  }
  return FFS_NO_ERRORS;
#endif //ifdef ICT_SPEEDUP_RFSFAT2
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
static UINT8
ucLabelStatus (const UINT8 mpt_idx, const UINT32 uiFATLabel)
/* PURPOSE      : This function determines the FAT label status
 *                (Reserved cluster, Last Cluster etc.)
 * ARGUMENTS    : uiFATLabel - value of the FAT label
 * RETURNS      : FAM_RET_LAST_CLUSTER
 *                FAM_RET_RESERVED_CLU
 *                FAM_RET_FREE_CLUSTER
 *                FAM_RET_BAD_CLUSTER
 *                FAM_RET_CLUSTER_NR
 *                FFS_NO_ERRORS
 * PRECONDITION : -
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE         : -
 ******************************************************************************/
{

  /* uiFATLabel is checked between min and max range of reserved sector       */
  if ((uiFATLabel >= FAM_RESERVED_CL_MIN(mpt_idx))
    && (uiFATLabel <= FAM_RESERVED_CL_MAX(mpt_idx)))
  {

    RFSFAT_SEND_TRACE ("RFSFAT: exit ucLabelStatus FAM_RESERVED_CLUSTER",
             RV_TRACE_LEVEL_WARNING);
    return FAM_RESERVED_CLUSTER;
  }

  if (uiFATLabel == FAM_BAD_CLUSTER(mpt_idx))
  {

    RFSFAT_SEND_TRACE ("RFSFAT: exit ucLabelStatus FAM_RET_BAD_CLUSTER",
             RV_TRACE_LEVEL_WARNING);
    return FAM_RET_BAD_CLUSTER;
  }

  if (uiFATLabel == FAM_FREE_CLUSTER)
  {

    RFSFAT_SEND_TRACE ("RFSFAT: exit ucLabelStatus FAM_RET_FREE_CLUSTER",
             RV_TRACE_LEVEL_DEBUG_LOW);
    return FAM_RET_FREE_CLUSTER;
  }
  /* uiFATLabel is checked between min and max range of last cluster          */
  /*lint -e685 */
  if ((uiFATLabel >= FAM_LAST_CLUSTER_MIN(mpt_idx))
    && (uiFATLabel <= FAM_LAST_CLUSTER_MAX(mpt_idx)))
  {
  RFSFAT_SEND_TRACE ("RFSFAT: exit ucLabelStatus FAM_RET_LAST_CLUSTER",
             RV_TRACE_LEVEL_DEBUG_LOW);
    return FAM_RET_LAST_CLUSTER;
  }
  /*lint +e685 */
  /* A normal used FAT cluster                                                */
  return FAM_RET_CLUSTER_NR;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT32
uiClusterDataSize (UINT8 mpt_idx)
/* PURPOSE      : This functions determines the data area size in clusters
 * ARGUMENTS    : mpt_idx: indicates the mountpoint
 * RETURNS      : The number of clusters, if an error occured the number of
 *                clusters is 0 will be returned.
 * PRECONDITION : Media manager must be initialised.
 * POSTCONDITION: The number of clusters for the data area is returned
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE: -
 *****************************************************************************/
{

#ifdef ICT_SPEEDUP_RFSFAT4

  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("RFSFAT:Maximum Number of Data cluster are ",MMA_BOOT_SECTOR[mpt_idx].ulMaxDataClusters);
	return MMA_BOOT_SECTOR[mpt_idx].ulMaxDataClusters;

#else
  UINT32 ulMaxDataClusters;

  /*Determine maximum number of sectors in data area                          */
  /*Total number of sectors - (reserved region + total FAT size + Root size)  */

  ulMaxDataClusters = MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol;
  ulMaxDataClusters -= MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors;
  ulMaxDataClusters -= (MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables *
            MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT);

if (MMA_FS_TYPE(mpt_idx) != GBI_FAT32_LBA)
{
   /* In case FAT16 and FAT12 */
	
  ulMaxDataClusters -= ((MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries * ROOT_LABEL_SIZE) /
						  MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
} 

  /* Determine the number of clusters in data area                            */
  ulMaxDataClusters =  (ulMaxDataClusters / MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster);

  /* Since the cluster number start from 2, and the mximum cluster number would be n+1 */
 /* since this function should indicate the maximum cluster number, its required to less than one */
  ulMaxDataClusters+=1;  
     
  return ulMaxDataClusters;
#endif
}


/**MH**Copyright by ICT Embedded BV All rights reserved **********************/
BOOL
FAM_bCreateFatTables (const UINT8 mpt_idx)
/* PURPOSE      : This function creates new FAT tables on the volume; this
 *                function is to be used when formatting the disk.
 * ARGUMENTS    : mpt_idx mountpointnumber
 * RETURNS      : TRUE  - FAT tables created.
 *                FALSE  - FAT tables not created
 * PRECONDITION : Master boot record information must be available/accessible.
 *                Media manager must be initialised.
 * POSTCONDITION: FAT tables are created on the volume.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  
  MMA_RW_STRUCT stWrite={0,0,NULL};    /* Structure with write information                 */
  UINT8* fatsec_p=NULL;
  UINT16 fat_num;
  UINT32 sec_num;

  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: FAM_bCreateFatTables entered");

  /* Allocate the memroy for fatsector */
  RFSFAT_GETBUF(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector, &fatsec_p); 

  stWrite.pucBuffer = fatsec_p;
  stWrite.tNrBytes = MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;

  /* point to first cluster */
  stWrite.tOffset =((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors)*(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector));

  rfsfat_vpMemset(fatsec_p, 0, (UINT16)(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector));
  
  for(fat_num=0;fat_num<MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables;fat_num++)
  	{ 
  	   /* May be two fat tables */
	   for(sec_num=0;sec_num<MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT;sec_num++)
	   	{
	   	   if(sec_num == 0)
	   	   	{
	   	   		   	   	 /* LITTLE ENDIAN ENTRIES */     
			 switch (MMA_BOOT_SECTOR[mpt_idx].fatEntrySize)
			 	{

				 case 1:
				 	     /* FAT12 */
						fatsec_p[0]=0xF8;  /* 1st Entry */
						fatsec_p[1]=0xFF;  /* Part of 1st and 2nd Entry */
						fatsec_p[2]=0xFF;  /* 2nd Entry */						 
						 break;
						 
				 case 2:
				 		/* FAT16 */
						fatsec_p[0]=0xF8;  /* 1st Entry - LSB */
						fatsec_p[1]=0xFF;  /* 1st Entry - MSB */
						fatsec_p[2]=0xFF;  /* 2nd Entry - LSB */
						fatsec_p[3]=0xFF;  /* 2nd Entry - MSB */
						break;

				 case 4:
				 		/* FAT32 */
						fatsec_p[0]=0xF8;  /* 1st Entry - LSB */
						fatsec_p[1]=0xFF;  /* 1st Entry       */
						fatsec_p[2]=0xFF;  /* 1st Entry 		*/
						fatsec_p[3]=0x0F;  /* 1st Entry - MSB */
						
						fatsec_p[4]=0xFF;  /* 2nd Entry - LSB */
						fatsec_p[5]=0xFF;  /* 2nd Entry   	*/
						fatsec_p[6]=0xFF;  /* 2nd Entry 		*/
						fatsec_p[7]=0x0F;  /* 2nd Entry - MSB */
						break;

				 default:
			        RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAT entry size error");
        			RFSFAT_FREE_BUF(fatsec_p);
			        return FALSE;
				 	
				 	
			 	}  /* End of Switch */
	   	   	}/* if(sec_num == 0)*/

			
		    if (MMA_ucDirectWrite (mpt_idx, &stWrite) != FFS_NO_ERRORS)
      		{
			     RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAM_RET_FAT_WRITE_ERROR");
        	     RFSFAT_FREE_BUF(fatsec_p);
			     return FALSE;
      		}

		 /* calcuate the offset , for next iteration */
		stWrite.tOffset += (MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
			
		if(sec_num == 0)
		{
	  		rfsfat_vpMemset(fatsec_p, 0, (UINT16)(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector));				 
		}
						  
	   	}  /* End of for(sec_num=0;...)*/
  	}      /* End of  for(fat_num=0;...) */

RFSFAT_FREE_BUF(fatsec_p);


#if FREE_SPACE_OPT  

   /* Now we have all free clusters  */
   MPT_TABLE[mpt_idx].free_space = uiClusterDataSize(mpt_idx)- 1;  
	/* The last cluster number -1 */
 
#endif






return TRUE;
  
}



/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 FAM_ucTruncLinkedList (const UINT8 mpt_idx, const UINT32 uiClusterNr,UINT32 NewSize)
/* PURPOSE      : This function deletes/removes a cluster linked list (chain)
 *                starting at cluster uiClusterNr. The Clusters in the FAT
 *                table will be freed.
 * ARGUMENTS    : mpt_idx indicates the partition number.
 *                uiClusterNr is the start cluster of the linked list.
 * RETURNS      : FFS_NO_ERRORS        - Linked list is removed and clusters
 *                                       are freed.
 *                FAM_RET_BAD_CLUSTER  - The next cluster is marked as BAD
 *                                       cluster, reading this cluster is not
 *                                       advised.
 *                FAM_RET_FAT_TABLE_ERROR  - The read label contains an invalid
 *                                          cluster number.
 *                FAM_RET_RESERVED_CLU - The read cluster is a reserved cluster
 *                FAM_RET_FREE_CLUSTER - The read cluster is a free cluster
 *                FAM_RET_FAT_WRITE_ERROR - Could not delete cluster.
 * PRECONDITION : partition must be formatted and media manager must be initialised
 * POSTCONDITION: Removed clusters in the FAT table are freed.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT8 ucRetNextClu;
  UINT8 ucRetVal;
  UINT32 uiPreviousCluster;
  UINT32 uiNextCluster;
  UINT16 uiClusterSize;
  UINT16 clusterNr;
  UINT16 cl_index;
  

  if (MMA_ucGetBootSectorInfo (mpt_idx) != FFS_NO_ERRORS)
  {
	RFSFAT_SEND_TRACE ("RFSFAT: CLM_RET_UNDEF_ERROR", RV_TRACE_LEVEL_ERROR);
	return CLM_RET_UNDEF_ERROR;
  }

  uiNextCluster = uiClusterNr;

 if(uiNextCluster < FAM_START_DATA_CLU)
 	{
 	  /* Its pointing to root cluster */
 	   if(MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
 	   	{
 	   	uiNextCluster = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
		uiClusterSize = (UINT16) (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
						     MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
 	   	}
	   else
	   	{
	   	 uiNextCluster = FAM_START_DATA_CLU;
		 uiClusterSize = (UINT16) MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries * META_DATA_SIZE;
	   	}
 	}
 else
 	{

	/* Normal cluster size for all clusters except the ROOT cluster           */
	  uiClusterSize = (UINT16) (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
						     MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
 	}

  /* find the number of cluster to skip from first cluster */
  clusterNr = NewSize/uiClusterSize;

  /* if NewSize is middle of cluster */
  if((NewSize%uiClusterSize) != 0)
  	   clusterNr++;

  /* check the status of first cluster */
  ucRetVal = ucLabelStatus (mpt_idx, uiNextCluster);

  if(ucRetVal != FAM_RET_CLUSTER_NR)
  	{
  	 RFSFAT_SEND_TRACE ("RFSFAT: Invalid label status No truncation ", RV_TRACE_LEVEL_DEBUG_HIGH);
  	    /* The current cluster itself may be last cluster , so dont truncate */
  		return FFS_NO_ERRORS;
  	}
  
 uiPreviousCluster = uiNextCluster;

  /* Traverse the linked list                                          */
  for (cl_index=0,ucRetVal=FFS_NO_ERRORS;
  			((cl_index<clusterNr) &&(ucRetVal==FFS_NO_ERRORS));
			  cl_index++)
  {

  	uiPreviousCluster = uiNextCluster;
	
    ucRetVal = FAM_ucGetNextCluster (mpt_idx, &uiNextCluster);
  /**************************************************************************
    * The loop must continue if ucRetVal == FFS_NO_ERRORS, in this case the   *
    * Last cluster in the chain is not yet reached                            *
    ***************************************************************************/

    if ((ucRetVal != FAM_RET_LAST_CLUSTER) && (ucRetVal != FFS_NO_ERRORS))
    {

      RFSFAT_SEND_TRACE ("RFSFAT: Logical FAT table error, or a \
        BAD cluster is read", RV_TRACE_LEVEL_ERROR);
      /* Return the error code                                                */
      return ucRetVal;
    }
  } 


   if( (uiPreviousCluster == uiNextCluster) || (cl_index < clusterNr))   
   	{
      RFSFAT_SEND_TRACE ("RFSFAT: (uiPreviousCluster == uiNextCluster) || (cl_index < clusterNr) No truncation ", RV_TRACE_LEVEL_DEBUG_HIGH);
	   /* No need to truncate */
	   return FFS_NO_ERRORS;
   	}
   
    ucRetVal = ucWriteFATEntry (mpt_idx, uiPreviousCluster, FAM_LAST_CLUSTER(mpt_idx));
    if (ucRetVal != FFS_NO_ERRORS)
    {
      /* Error writing the FAT label                                          */
      RFSFAT_SEND_TRACE ("RFSFAT: Error writing the FAT label ", RV_TRACE_LEVEL_ERROR);
      return ucRetVal;
    }

	uiPreviousCluster = uiNextCluster;
	ucRetNextClu = FFS_NO_ERRORS;
  while (ucRetNextClu != FAM_RET_LAST_CLUSTER)
  {
    ucRetNextClu = FAM_ucGetNextCluster (mpt_idx, &uiNextCluster);

    if ((ucRetNextClu != FFS_NO_ERRORS) && (ucRetNextClu != FAM_RET_LAST_CLUSTER))
    {
     /* Error in the Chain*/
      RFSFAT_SEND_TRACE("RFSFAT: BAD cluster or unexpected end in chain ", RV_TRACE_LEVEL_ERROR);
      return ucRetNextClu;
    }
	
  /**************************************************************************
    * If ucRetNextCluster == FAM_RET_LAST_CLUSTER then the uiNextCluster is   *
    * the same as uiPreviousCluster, so when deleting uiPreviousCluster the   *
    * last entry in the chain is also deleted.                                *
    ***************************************************************************/
    /* Delete uiPreviousCluster                                               */
    ucRetVal = ucWriteFATEntry (mpt_idx, uiPreviousCluster, FAM_FREE_CLUSTER);
    if (ucRetVal != FFS_NO_ERRORS)
    {
      /* Error writing the FAT label                                          */
      RFSFAT_SEND_TRACE ("RFSFAT: Error writing the FAT label ", RV_TRACE_LEVEL_ERROR);
      return ucRetVal;
    }

#if FREE_SPACE_OPT

if(MPT_TABLE[mpt_idx].free_space != INVALID_FREE_SPACE_VAL)
{
      /* increment the free_space cluster */
	  MPT_TABLE[mpt_idx].free_space++;   
   
}

#endif


	

    /* Set current cluster to new cluster*/
    uiPreviousCluster = uiNextCluster;
  }


  
  return FFS_NO_ERRORS;
}






