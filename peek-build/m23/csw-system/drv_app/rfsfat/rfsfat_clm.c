/**
 * @file  rfsfat_clm.c
 *
 * The Cluster manager will make file access a lot easier,
 * by creating an abstraction layer above the medium.
 * Using the manager, files can be accessed using the first
 * cluster number of the file only.
 * The manager let it look like files consist of only one
 * single cluster.
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
 *	04/29/2004	E. Oude Middendorp		FAT32 added.
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

#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_clm.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_fam.h"
#include "rfsfat/rfsfat_i.h"


/******************************************************************************
*      Module specific Defines and enums                                      *
******************************************************************************/
/* Define module ID string for debugging.                                     */
#ifdef DEBUG_TRACING
#undef  DEBUG_MOD_ID_STRING
#define DEBUG_MOD_ID_STRING                                              "CLM "
#endif

/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

#define META_DATA_SIZE  32		/* Size of a meta-data label in the ROOT      */
#define ROOT_CLUSTER    0x0000	/* The root cluster has value 0x0000          */
#define START_OFFSET    0		/* Used for setting the offset in new cluster */


/******************************************************************************
*      Static-Function prototypes                                             *
******************************************************************************/



/******************************************************************************
*      Implementation of Global Function                                      *
******************************************************************************/

#ifdef  ICT_SPEEDUP_RFSFAT1
/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT16 CLM_ucDetermineClusterSize( UINT32 uiClusterNmb, UINT8 mpt_idx)
{     
  UINT16  uiClusterSize;
  
  if (uiClusterNmb < FAM_START_DATA_CLU)
  {
  /**************************************************************************
    * Special cluster size for ROOT area                                      *
    ***************************************************************************/
    uiClusterSize = (UINT16) MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries *
                             META_DATA_SIZE;
  }
  else
  {
    /* Normal cluster size for all clusters except the ROOT cluster           */
    uiClusterSize = (UINT16) (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster * 
                              MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
  }

  return uiClusterSize;
}


/******Copyright by ICT Embedded BV All rights reserved **********************/

UINT8 CLM_ucReadWrite_FILE(
      RO_PTR_TO_RW (CLM_READ_WRITE_STRUCT)  pstReadWrite,
      const UINT8                           ucMode,
      UINT16                 				 uiClusterSize,
      UINT32 								*clum_p)
{
  UINT32                  uiSequentialClusterNmb;
  UINT32                  uiClusterNmb;
  UINT32                  uiNewCluster;
  UINT32                  uiPreviousCluster;
  UINT32                  uiSubseedingCluster;
  FFS_FILE_LENGTH_TYPE    tNrOfBytesToRW;
  FFS_FILE_LENGTH_TYPE    tNrBytes;
  FFS_FILE_LENGTH_TYPE    tNrBytesTmp;
  FFS_FILE_LENGTH_TYPE    tOffset;
  MMA_RW_STRUCT           stDirectRW={0,0,NULL};
  UINT8                   ucRetVal;
  UINT8                   mpt_idx;
  BOOL                    bNextClusterIsNextLogicalCluster;
  BOOL					  is_last_cluster;	

  mpt_idx = pstReadWrite->mpt_idx;
  
  if (MMA_ucGetBootSectorInfo (mpt_idx) != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: CLM_RET_UNDEF_ERROR", RV_TRACE_LEVEL_ERROR);
    return CLM_RET_UNDEF_ERROR;
  }

   /* check the cluster size parameters */ 
   if(uiClusterSize == 0)
  	{
  	   /* panic */
	   RFSFAT_SEND_TRACE_ERROR("RFSFAT: CLM_ucReadWrite , FATAL error cluster size is zero ");
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , mount point Index  = ",pstReadWrite->mpt_idx);
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , ucSectorPerCluster = ",MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].ucSectorPerCluster);
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , uiBytesPerSector = ",MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiBytesPerSector); 
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , uiMaxRootEntries = ",MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiMaxRootEntries);  
	   return CLM_RET_UNDEF_ERROR;
  	}
                              
  tOffset      = pstReadWrite->tOffset;
  uiClusterNmb = pstReadWrite->uiFirstClusterNr; //Initialy: start from first
  RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, uiClusterSize = :", uiClusterSize, RV_TRACE_LEVEL_DEBUG_LOW);
  RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, pstReadWrite->uiFirstClusterNr = :", pstReadWrite->uiFirstClusterNr, RV_TRACE_LEVEL_DEBUG_LOW);
  RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, pstReadWrite->tOffset          = :", pstReadWrite->tOffset, RV_TRACE_LEVEL_DEBUG_LOW);
  RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, pstReadWrite->tNrBytes         = :", pstReadWrite->tNrBytes, RV_TRACE_LEVEL_DEBUG_LOW);



  /* Now we skip all the (whole) clusters that lies within the offset area.
     Remember to create all the cluster that are not alreade created (only for
     write operation).
  */
  uiNewCluster = uiClusterNmb; //initialize while loop variable
  while (tOffset >= uiClusterSize)
  {
    ucRetVal = FAM_ucGetNextCluster (mpt_idx, &uiNewCluster);//returns uiNewCluster
    switch (ucRetVal)
    {
    case FAM_RET_ROOT_CLUSTER:
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: CLM_RET_NO_SPACE");
      /**********************************************************************
      The ROOT cluster is a fixed cluster and there is no linked list for
      the ROOT cluster, so returning the status that there is no space left.
      ***********************************************************************/
      return CLM_RET_NO_SPACE;
    case FFS_NO_ERRORS:
      /********************************************************************
       * There is a next cluster in the cluster linked list                *
       * Decrease the offset with the cluster size                         *
       *********************************************************************/
      RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Offset. Found existing cluster = :", uiNewCluster, RV_TRACE_LEVEL_DEBUG_LOW);
      tOffset -= uiClusterSize;
      break;
    case FAM_RET_LAST_CLUSTER:

      if (ucMode == CLM_READ_MODE)
      {
       if((tOffset-uiClusterSize) >= uiClusterSize)
       	{
        RFSFAT_SEND_TRACE_ERROR ("RFSFAT: CLM_RET_LIST_ERROR");
        /* There is no next cluster, the linked list is corrupt ! */
        return CLM_RET_LIST_ERROR;
       	}
	   else
	   	{
	   	   	tOffset -= uiClusterSize;
      		RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Offset. the last cluster = :", uiNewCluster, RV_TRACE_LEVEL_DEBUG_LOW);
	   	}
      }
	  else
	  	{
      		/* Fill-up the gap, append a cluster to the cluster linked list */
	        /* NOTE that this condition should not trigger , but its better to have this */
      		ucRetVal = FAM_ucAppendCluster (mpt_idx, &uiNewCluster,FALSE);
      if (ucRetVal != FFS_NO_ERRORS)
      {
       			pstReadWrite->tNrBytes = 0;   /* Nothing is written */
        RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAM_ucAppendCluster failed");
        return ucRetVal;
      }
      tOffset -= uiClusterSize;
      RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Offset. Append new cluster = :", uiNewCluster, RV_TRACE_LEVEL_DEBUG_LOW);
	  	}  /* CLM_WRITE_MODE */

      break;
    default:
      RFSFAT_SEND_TRACE ("RFSFAT: CLM_RET_UNDEF_ERROR", RV_TRACE_LEVEL_ERROR);
      return CLM_RET_UNDEF_ERROR;
    }
  }


  /* OK. We are now at the cluster from where to start read or write.
  */
  stDirectRW.tNrBytes = 0;
  stDirectRW.tOffset = 0;
  stDirectRW.pucBuffer = pstReadWrite->pucBuffer;
  tNrOfBytesToRW = pstReadWrite->tNrBytes;
  uiPreviousCluster = uiNewCluster;
  uiSubseedingCluster = uiNewCluster;
  is_last_cluster = FALSE;
  while (tNrOfBytesToRW != 0)
  {
    /* start from current cluster */
	//Preset variables for next while loop (possible results from previous loop)
      uiNewCluster = uiSubseedingCluster; //remember chunk begin cluster

    /* Check if we do not need a new cluster.
       This is so, when the current cluster can hold all the data to read/write.
       This can (and must) be done only once at the start here.
    */
    tNrBytes = uiClusterSize - tOffset; /* determine space left to read/write in current cluster */
    /* Can it all be Read / Written in this cluster, and has it not be done allready. */
    if (tNrBytes >= tNrOfBytesToRW)
    {
      stDirectRW.tNrBytes = tNrOfBytesToRW; //Yes, al in this cluster
    }
    else
    {
      /* No, we need more then can fit in this cluster. */
      stDirectRW.tNrBytes += tNrBytes; //This cluster at least can be written
      tNrBytesTmp = tNrOfBytesToRW - tNrBytes;//bytes remaining 
      
      /* Test if the next clusters are consecutive clusters, so we can write one big chunck.*/

      
      bNextClusterIsNextLogicalCluster = FALSE; //Preset while condition
      do
      { 

        if (tNrBytesTmp > 0) // if there is something left
        {
         uiPreviousCluster = uiSubseedingCluster;
          RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. FAM_ucGetNextCluster = :", uiSubseedingCluster, RV_TRACE_LEVEL_DEBUG_LOW);
          ucRetVal = FAM_ucGetNextCluster (pstReadWrite->mpt_idx, &uiSubseedingCluster);
          switch (ucRetVal)
          {
          case FAM_RET_ROOT_CLUSTER:
            RFSFAT_SEND_TRACE_ERROR("RFSFAT: CLM_RET_NO_SPACE");
            /**********************************************************************
            The ROOT cluster is a fixed cluster and there is no linked list for
            the ROOT cluster, so returning the status that there is no space left.
            ***********************************************************************/
            return CLM_RET_NO_SPACE;
          case FFS_NO_ERRORS:
            /********************************************************************
             * OK. There is a next cluster in the cluster linked list            *
             *********************************************************************/
            RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Found existing cluster = :", uiSubseedingCluster, RV_TRACE_LEVEL_DEBUG_LOW);
            break;
          case FAM_RET_LAST_CLUSTER:

   		  if (ucMode == CLM_WRITE_MODE) 
  			{  
	
    			/* Append a cluster to the cluster linked list */
				ucRetVal = FAM_ucAppendCluster (mpt_idx, &uiSubseedingCluster,FALSE); 
				if (ucRetVal != FFS_NO_ERRORS)  
				{  
				 if((ucRetVal == FAM_RET_FAT_NO_SPACE) || (ucRetVal == FAM_RET_NO_FAT_CLUSTER))
				 	{
				 	   is_last_cluster=TRUE;
					   RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAM_ucAppendCluster failed, FAM_RET_FAT_NO_SPACE"); 
				 	}
				 else
				 	{
						RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAM_ucAppendCluster failed ... 2"); 
						return ucRetVal;   
				 	}	
				} 
				RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1 (2), Data. Append new cluster = :", uiSubseedingCluster, RV_TRACE_LEVEL_DEBUG_LOW);
			}

            break;
          default:
            RFSFAT_SEND_TRACE ("RFSFAT: CLM_RET_UNDEF_ERROR", RV_TRACE_LEVEL_ERROR);
            return CLM_RET_UNDEF_ERROR;
          }
          

          bNextClusterIsNextLogicalCluster = FALSE; //Preset while condition

          if (uiSubseedingCluster == (uiPreviousCluster+1))
          {
            // Nice. It's logical number is one higher. We can use this cluster also.
            if (tNrBytesTmp > uiClusterSize)
            {
              RFSFAT_SEND_TRACE("RFSFAT:  SPEEDUP1, FULL FILLED Subseeding Cluster", RV_TRACE_LEVEL_DEBUG_LOW);
              bNextClusterIsNextLogicalCluster = TRUE;
              stDirectRW.tNrBytes += uiClusterSize;
              tNrBytesTmp -= uiClusterSize;
            }
            else
            { // Thats all. Do not set bNextClusterIsNextLogicalCluster to exit while
              RFSFAT_SEND_TRACE("RFSFAT:  SPEEDUP1, PARTLY FILLED Subseeding Cluster", RV_TRACE_LEVEL_DEBUG_LOW);
              stDirectRW.tNrBytes += tNrBytesTmp;
              tNrBytesTmp = 0;
            }

          }
          else
          {
            /* Not subseeding. Cluster is (possible) already added, but we write it in next loop
              First we write the consecutive clusters.
            */
            /* uiSubseedingCluster = uiPreviousCluster; //One back. dont go -- FIX */
			/* This uiSubseedingCluster will be considered in next iteration */
            bNextClusterIsNextLogicalCluster = FALSE;
            RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Not subseeding, reset to cluster = :", uiSubseedingCluster, RV_TRACE_LEVEL_DEBUG_LOW);
          }

        }
      }while (bNextClusterIsNextLogicalCluster);
    }
    
    /* read / write now */

    /* Determine offset and cluster to start Reading / Writing              */
    if (bClusterOffsetInVol (mpt_idx, uiNewCluster,
                 (FFS_FILE_LENGTH_TYPE *) &stDirectRW.tOffset))
    {
      stDirectRW.tOffset += tOffset;
      tOffset = 0; //once read/written, the base offset is gone.
    }

    RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Going to R/W, tOffset   = :", tOffset, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Going to R/W, uiNewCluster   = :", uiNewCluster, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Going to R/W, stDirectRW.tOffset   = :", stDirectRW.tOffset, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Going to R/W, stDirectRW.tNrBytes  = :", stDirectRW.tNrBytes, RV_TRACE_LEVEL_DEBUG_LOW);
    RFSFAT_SEND_TRACE_PARAM("RFSFAT:  SPEEDUP1, Data. Going to R/W, stDirectRW.pucBuffer = :", (UINT32)stDirectRW.pucBuffer, RV_TRACE_LEVEL_DEBUG_LOW);

    if (ucMode == CLM_WRITE_MODE)
    {
      ucRetVal = MMA_ucDirectWrite (mpt_idx, &stDirectRW);
    }
    else
    {
      ucRetVal = MMA_ucDirectRead (mpt_idx, &stDirectRW);
    }

    /* Process return value                                                 */
    if (ucRetVal != FFS_NO_ERRORS)
    {
     if(ucRetVal == MMA_RET_READ_ERR)
     	{
      /* Mark current cluster as a bad cluster !!!                          */
      ucRetVal = FAM_ucMarkBadCluster (mpt_idx, uiNewCluster);
      if (ucRetVal != FFS_NO_ERRORS)
      {
        RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucMarkBadCluster failed ",
                 RV_TRACE_LEVEL_ERROR);
        return ucRetVal;
      }
      		RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_BAD_CLUSTER", RV_TRACE_LEVEL_ERROR);
     
     	}
      else
      	{
	 		RFSFAT_SEND_TRACE ("RFSFAT: outside the partition", RV_TRACE_LEVEL_ERROR);
      	}	
      return FAM_RET_BAD_CLUSTER;
    }

    /* set pointer in buffer after the read bytes                         */
    stDirectRW.pucBuffer += stDirectRW.tNrBytes;

    if(tNrOfBytesToRW < stDirectRW.tNrBytes)
   	{
		tNrOfBytesToRW = 0;   /* Error, move out the loop */
		return CLM_RET_UNDEF_ERROR;   /* Internal Error */
   	}
    else
   	{
   		tNrOfBytesToRW -= stDirectRW.tNrBytes;	   	 
   	}
   	stDirectRW.tNrBytes = 0;
 
   if(is_last_cluster == TRUE)
 	{
 	   /* Disk is full , we can not continue writing */

	   /* Record how much data you have written */
	   pstReadWrite->tNrBytes -=tNrOfBytesToRW; 
	   tNrOfBytesToRW = 0;   

	   /* return the error code */
	   return CLM_RET_NO_SPACE;  /* No space */
	   
 	}

  }  /* while (tNrOfBytesToRW != 0) */
  
  /* Save where we are now, to have this advantage the next time this function is called. */

  if(clum_p != NULL)
  	{
      *clum_p = uiSubseedingCluster;	

  	}


  return FFS_NO_ERRORS;

}
#endif  //ifdef  ICT_SPEEDUP_RFSFAT1

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 CLM_ucReadWrite
  (RO_PTR_TO_RO (CLM_READ_WRITE_STRUCT) pstReadWrite, const UINT8 ucMode)
/* PURPOSE      : This function is used to read or write bytes to the partition
 *                with the data stored in clusters. This function handles all
 *                cluster switching with help of the FAT manager.
 * ARGUMENTS    : pstReadWrite point to a structure which holds the information
 *                for reading or writing.
                  UINT8  mpt; //mountpoint index -> Null based
 * RETURNS      : CLM_RET_UNDEF_ERROR
 *                CLM_RET_LIST_ERROR
 *                CLM_RET_NO_SPACE
 *                FFS_NO_ERRORS
 *                All possible Errors from the FAT manager
 *                function FAM_ucAppendCluster
 * PRECONDITION : -
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  FFS_FILE_LENGTH_TYPE tNrOfBytesToRW;
  FFS_FILE_LENGTH_TYPE tNewOffset;
  MMA_RW_STRUCT stDirectRW={0,0,NULL};
  UINT32 uiNewCluster;
  UINT32 uiClusterSeqNmb;
  UINT16 uiClusterSize;
  UINT8 ucRetVal;
  BOOL bFitInCluster;

  if(pstReadWrite->tNrBytes == 0)
  	{
  	 	return FFS_NO_ERRORS;
  	}

  stDirectRW.pucBuffer = pstReadWrite->pucBuffer;
  uiNewCluster = pstReadWrite->uiFirstClusterNr;

  if (MMA_ucGetBootSectorInfo (pstReadWrite->mpt_idx) != FFS_NO_ERRORS)
	{

	  RFSFAT_SEND_TRACE ("RFSFAT: CLM_RET_UNDEF_ERROR", RV_TRACE_LEVEL_ERROR);

	  /* Error exit                                                             */
	  return CLM_RET_UNDEF_ERROR;
	}
  tNrOfBytesToRW = pstReadWrite->tNrBytes;
  tNewOffset = pstReadWrite->tOffset;

  if (pstReadWrite->uiFirstClusterNr < FAM_START_DATA_CLU)
	{
	/**************************************************************************
    * Special cluster size for ROOT area                                      *
    ***************************************************************************/
	  uiClusterSize =
		(UINT16) MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiMaxRootEntries * META_DATA_SIZE;
	}
  else
	{
	  /* Normal cluster size for all clusters except the ROOT cluster           */
	  uiClusterSize = (UINT16) (MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].ucSectorPerCluster *
								MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiBytesPerSector);
	}


  if(uiClusterSize == 0)
  	{
  	   /* panic */
	   RFSFAT_SEND_TRACE_ERROR("RFSFAT: CLM_ucReadWrite , FATAL error cluster size is zero ");
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , mount point Index  = ",pstReadWrite->mpt_idx);
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , ucSectorPerCluster = ",MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].ucSectorPerCluster);
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , uiBytesPerSector = ",MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiBytesPerSector); 
	   RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: CLM_ucReadWrite , uiMaxRootEntries = ",MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiMaxRootEntries);  
	   return CLM_RET_UNDEF_ERROR;	   
  	}

  if (rfsfat_env_ctrl_blk_p->ffs_globals.clusterBaseNmb == uiNewCluster)
  { //same start cluster. Check if offset indicate same lookup cluster as before
    uiClusterSeqNmb = tNewOffset / uiClusterSize;
    if (uiClusterSeqNmb  >= rfsfat_env_ctrl_blk_p->ffs_globals.last_clusterSeqNmb)
    {
      /* NOTE: 
	        if uiClusterSeqNmb becomes zero then , tNewOffset does not change
	        */
      //looking in same cluster as before. Remove offset and restore lookup cluster number
      tNewOffset -= (MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].ucSectorPerCluster *
                     MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiBytesPerSector *
                     (rfsfat_env_ctrl_blk_p->ffs_globals.last_clusterSeqNmb));
      uiNewCluster = rfsfat_env_ctrl_blk_p->ffs_globals.last_get_next_cluster_next;
      }
      else
      { //start from base. Recalculate and scan link complete.
       rfsfat_env_ctrl_blk_p->ffs_globals.last_clusterSeqNmb = 0xFFFFFFFE;  //clear history
       rfsfat_env_ctrl_blk_p->ffs_globals.clusterBaseNmb = uiNewCluster;
       rfsfat_env_ctrl_blk_p->ffs_globals.last_get_next_cluster_next  = uiNewCluster;
    
  }
  }
  else
  { //start from base. Recalculate and scan link complete.
    rfsfat_env_ctrl_blk_p->ffs_globals.last_clusterSeqNmb = 0xFFFFFFFE;  //clear history
    rfsfat_env_ctrl_blk_p->ffs_globals.clusterBaseNmb = uiNewCluster;
    rfsfat_env_ctrl_blk_p->ffs_globals.last_get_next_cluster_next  = uiNewCluster;
  }


  
  while (tNrOfBytesToRW != 0)
	{
    /* Determine if the offset is in the current cluster                      */
	  while (tNewOffset >= uiClusterSize)
		{
      /************************************************************************
      Attempting to Read/Write outside the current cluster, get next cluster if
      the Cluster is not the ROOT cluster and decrease the new offset with the
      cluster size.
      THIS CANNOT BE DONE FOR THE ROOT CLUSTER !
      *************************************************************************/
		  ucRetVal = FAM_ucGetNextCluster (pstReadWrite->mpt_idx, &uiNewCluster);
		  switch (ucRetVal)
			{
			case FAM_RET_ROOT_CLUSTER:
				RFSFAT_SEND_TRACE_ERROR("RFSFAT: CLM_RET_NO_SPACE");
		    /**********************************************************************
        The ROOT cluster is a fixed cluster and there is no linked list for
        the ROOT cluster, so returning the status that there is no space left.
        ***********************************************************************/
				return CLM_RET_NO_SPACE;
			case FFS_NO_ERRORS:
  		  /********************************************************************
         * There is a next cluster in the cluster linked list                *
         * Decrease the offset with the cluster size                         *
         *********************************************************************/
				tNewOffset -=
  				  MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].ucSectorPerCluster *
  				  MMA_BOOT_SECTOR[pstReadWrite->mpt_idx].uiBytesPerSector;

			  break;
			case FAM_RET_LAST_CLUSTER:
				if (ucMode == CLM_READ_MODE)
				{
					RFSFAT_SEND_TRACE_ERROR ("RFSFAT: CLM_RET_LIST_ERROR");
					/* There is no next cluster, the linked list is corrupt !         */
					return CLM_RET_LIST_ERROR;
				}
				/* append a cluster to the cluster linked list                      */
				ucRetVal = FAM_ucAppendCluster (pstReadWrite->mpt_idx, &uiNewCluster,TRUE);
				if (ucRetVal != FFS_NO_ERRORS)
				{
					RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAM_ucAppendCluster failed");
					return ucRetVal;
				}
        /********************************************************************
        * starting in a new cluster, offset can be set to 0                 *
        *********************************************************************/
				tNewOffset = START_OFFSET;

			  break;
			default:
				RFSFAT_SEND_TRACE ("RFSFAT: CLM_RET_UNDEF_ERROR", RV_TRACE_LEVEL_ERROR);
				return CLM_RET_UNDEF_ERROR;
			}
		}

	  /* determine number of bytes to write in current cluster                  */
	  stDirectRW.tNrBytes = uiClusterSize - tNewOffset;
	  bFitInCluster = FALSE;

	  if (stDirectRW.tNrBytes >= tNrOfBytesToRW)
		{
		  /* All bytes can be Read / Written in this cluster                      */
		  bFitInCluster = TRUE;
		  /*
		     The cluster contains more bytes than required so set the nr of bytes to
		     the required number of bytes
		   */
		  stDirectRW.tNrBytes = tNrOfBytesToRW;
		}

	  /* Determine offset and cluster to start Reading / Writing              */
	  if (bClusterOffsetInVol (pstReadWrite->mpt_idx, uiNewCluster,
							   (FFS_FILE_LENGTH_TYPE *) & stDirectRW.tOffset))
		{
		  stDirectRW.tOffset += tNewOffset;
		}

	  if (ucMode == CLM_WRITE_MODE)
		{
		  /* start writing bytes to medium                                      */
		  ucRetVal = MMA_ucDirectWrite (pstReadWrite->mpt_idx, &stDirectRW);
		}
	  else
		{
		  /* start reading bytes from medium                                    */
		  ucRetVal = MMA_ucDirectRead (pstReadWrite->mpt_idx, &stDirectRW);
		}
	  /* Process return value                                                 */
	  if (ucRetVal != FFS_NO_ERRORS)
		{
		  /* Mark current cluster as a bad cluster !!!                          */
		  ucRetVal =
			FAM_ucMarkBadCluster (pstReadWrite->mpt_idx, uiNewCluster);
		  if (ucRetVal != FFS_NO_ERRORS)
			{
			  RFSFAT_SEND_TRACE ("RFSFAT: FAM_ucMarkBadCluster failed ",
								 RV_TRACE_LEVEL_ERROR);
			  return ucRetVal;
			}
		  RFSFAT_SEND_TRACE ("RFSFAT: FAM_RET_BAD_CLUSTER",
							 RV_TRACE_LEVEL_ERROR);
		  return FAM_RET_BAD_CLUSTER;
		}
	  tNrOfBytesToRW -= stDirectRW.tNrBytes;

	  if (!bFitInCluster)
		{
		  tNewOffset += stDirectRW.tNrBytes;
		  /* set pointer in buffer after the read bytes                         */
		  stDirectRW.pucBuffer += stDirectRW.tNrBytes;
		}
	}

 rfsfat_env_ctrl_blk_p->ffs_globals.last_get_next_cluster_next = uiNewCluster;
 rfsfat_env_ctrl_blk_p->ffs_globals.last_clusterSeqNmb =
           (pstReadWrite->tOffset + pstReadWrite->tNrBytes-1) / uiClusterSize;
  return FFS_NO_ERRORS;
}

/******************************************************************************
*      Implementation of Local (= static) Function                            *
******************************************************************************/

/******Copyright by ICT Embedded BV All rights reserved **********************/
BOOL bClusterOffsetInVol
  (const UINT8 mpt_idx,
   const UINT32 uiClusterNr, RO_PTR_TO_RW (FFS_FILE_LENGTH_TYPE) ptOffset)
/* PURPOSE      : This function determines the offset of a cluster number in
 *              . the partition.
 *
 * ARGUMENTS    : mpt_idx : partition indication.
 *                uiClusterNr : Cluster number to determine the offset of.
 *                tOffset : Determined Offset.
 *                mpt: mountpoint index of used to find media & partition     
 * RETURNS      : TRUE : Offset is determined
 *                FALSE : Determined offset is not valid.
 * PRECONDITION : partition must be formatted.
 * POSTCONDITION: Offset of the cluster is determined.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE:
 * The determined offset (irp to the begin of the boot-sector)
 * (this is not alays per definition the first byte of the medium).
 * Clusters 0 and 1 are a l w a y s reserved clusters !!!!
 *
 * Before the data erea lies:
 * - Reserved region, includes the boot sector (same for FAT16 and FAT32)
 * - FAT tables (same for FAT16 and FAT32)
 * - Root Directory (FAT16 only)
 *   FAT16: this is always a fix size of 512,
 *   FAT32: there is no 'special' root dir. The root dir is a normal cluster
 *          and part of the data area. This root dir cluster number is saved
 *          in the boot area (offset 44).
 * 
 * The first data cluster is always 2 (FAM_START_DATA_CLU).
 * When the offset of cluster lower then 2 is requested, the root dir offset
 * will be returned (which calculates different for FAT16 and FAt32).
 *****************************************************************************/
{
  UINT32 uiClusterNrLocal = uiClusterNr;

  if ( ((MPT_TABLE[mpt_idx].media_type == GBI_SD)
        ||(MPT_TABLE[mpt_idx].media_type == GBI_MMC))
      && (MPT_TABLE[mpt_idx].filesys_type == GBI_UNKNOWN))
  {
  // for raw flash card compute boot info based on known data
     MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables = 2;
     MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors = 1;
     MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector = MPT_TABLE[mpt_idx].blk_size;
     MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT = 
                    MPT_TABLE[mpt_idx].partition_size/MPT_TABLE[mpt_idx].blk_size; 
     //MPT_TABLE[mpt_idx].filesys_type == GBI_FAT32_LBA;                    
  }
  else 
  {
     if (MMA_ucGetBootSectorInfo (mpt_idx) != FFS_NO_ERRORS)
     {
        return FALSE; //No vallid bootsector info available
     }   
  }

  /* FAT16 and FAT32 common: reserved region and FAT tables */
  *ptOffset = (((MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables *
                  MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT) +
                 MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors) *
                  MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);

  /* So, here we are at FAM_START_DATA_CLU in case of FAT32 */
  /* we are root directory in case of FAT16 */

  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) 
  	{
     if (uiClusterNrLocal < FAM_START_DATA_CLU)
  		    uiClusterNrLocal = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
  
  	} 
  else    /* it could be FAT16 or FAT12 */
  	if (uiClusterNrLocal >= FAM_START_DATA_CLU)
  {
      /* If we are at the cluster FAM_START_DATA_CLU , then we need to be at
      	 very next offset to reserved root directory 
         */
	  
    *ptOffset +=
      (UINT16) MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries * META_DATA_SIZE;

  }

  /* SUSPECTION:  There could be a Bug in case of FAT16 */
  /* SOME MORE TRIALS are required , GAPS Might form */

  
  if (uiClusterNrLocal > FAM_START_DATA_CLU)
  {
    *ptOffset += ((uiClusterNrLocal - FAM_START_DATA_CLU) *
                  (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
                   MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector));
  }

  return TRUE;
}

