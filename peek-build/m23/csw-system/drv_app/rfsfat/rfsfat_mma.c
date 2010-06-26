/**
 * @file	rfsfat_mma.c
 *
 *  The media manager manages the mountpoints on the different media
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
 *	04/29/2004	E. Oude Middendorp		FAT32 added.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

/***** Includes **************************************************************/

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include <string.h>

#include "gbi/gbi_api.h"
#include "gbi/gbi_i.h"
#include "gbi/gbi_operations.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"

#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_api.h"

/******************************************************************************
*      Module specific Defines and enums                                      *
******************************************************************************/
/* Define module ID string for debugging.                                     */
#ifdef DEBUG_TRACING
#undef  DEBUG_MOD_ID_STRING
#define DEBUG_MOD_ID_STRING                                              "MMA"
#endif

/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif
#define FAT32_TII_MODIFICATION

/******************************************************************************/
/* Boot-sector defines                                                        */
/******************************************************************************/
#define OFFSET_SMALL_SECT   0x13	/* Used when mountpoint < 32 Mb                 */
#define SIZE_SMALL_SECT     0x02	/* Number of bytes to read from bootsector  */
#define OFFSET_BIG_SECT     0x20	/* Used if mountpoint > 32 Mb                   */
#define EXEC_MARKER         0xAA55	/* The executable marker is used to determine
									   if the mountpoint is formatted. The marker is
									   located at offset OFFSET_EXEC_MARKER
									   (at the end of the boot-sector           */
#define OFFSET_EXEC_MARKER  0x1fe	/* Executable marker of bootsector 2bytes   */

/******************************************************************************/
/* Defines for reading / writing to the boot-sector                           */
/******************************************************************************/
#define READ_MODE      0		/* Boot-sector will be read                   ` */
#define WRITE_MODE     1		/* New Boot-sector will be written              */
/******************************************************************************/
/* Struct member defines for reading/filling the boot-sector structure        */
/******************************************************************************/
#define ST_MEMBER_0   0			/* member uiBytesPerSector                      */
#define ST_MEMBER_1   1			/* member ucSectorPerCluster                    */
#define ST_MEMBER_2   2			/* member uiReservedSectors                     */
#define ST_MEMBER_3   3			/* member ucNrFatTables                         */
#define ST_MEMBER_4   4			/* member uiMaxRootEntries                      */
#define ST_MEMBER_5   5			/* member uiNrSectorsFAT                        */
#define ST_MEMBER_6   6			/* member uiNrSectorsVol                        */
#define WRITE_BS_JMP  7			/* Writing boot-sector JMP code                 */
#define WRITE_BS_MD   8			/* Writing boot-sector media descriptor         */
#define WRITE_NAME		9		/* Writing bootsector partition name                        */
#define WRITE_FSTYPE  10    	/* Writing bootsector Filesystem type           */
#define WRITE_SPT	  11	    /* Writing boot-sector  Number of serctors per track     */
#define WRITE_SIDES   12        /* Writing boot-sector  Number of sides in the media     */
#define WRITE_MARKER  13		/* Writing boot-sector executable marker        */
/******************************************************************************
 * Maximum number of items to read from the boot-sector, above this number    *
 * only items may be written to the boot-sector                               *
 ******************************************************************************/
#define MAX_BS_READ         ST_MEMBER_6
#ifndef UINT_MAX
#define UINT_MAX            0xffff
#endif
//#define UINT_MAX            0xffff
#define UINT12_MAX            0x0ff5     /* Maximum of 4085 clusters */
#define UINT32_MAX          0xffffffff	/* Used to check boundaries for UINT32 */
#define MAX_SMALL_PART      0x1f40000	/* mountpoint size 32Mbyte                */
/******************************************************************************/
/* Defines for creating a new boot-sector                                     */
/******************************************************************************/
#define MAX_SECT_CLU        128	/* Maximum sectors in a cluster             */
#define NEW_BS_NR_FAT       0x02	/* Number of FAT tables                     */
#define NEW_BS_SECT_CLU     0x02	/* Number of sectors/cluster                */
#define NEW_BS_SECT_CLU_12  0x01    /* Number of sectors/cluster, FAT12 */
#define NEW_BS_MAX_ROOT     512	/* Maximum number of root entries           */
#define NEW_BS_MAX_ROOT_12	224    /* special case of FAT12 , 224 entries 	*/
#define NEW_BS_RES_SECT     0x01	/* Number of reserved sectors               */
/******************************************************************************/
#define NR_BYTES_FAT_ENTRY  0x02	/* Number of bytes used for an entry in FAT */

#define BS_FSTYPE_FIELD_LENGTH    (8)


#define FAT12_LEAST_BLKS		(1000)


/******************************************************************************/
/* Defines added for FAT32                                                    */
/******************************************************************************/
#define MMA_F32_BOOT_SECTOR_INFO_SIZE	  (52)
#define MMA_F32_OFFS_BYTES_PER_SEC			(11)
#define MMA_F32_OFFS_SEC_PER_CLUS				(13)
#define MMA_F32_OFFS_NMB_RESERVED_SEC 	(14)
#define MMA_F32_OFFS_NMB_FAT            (16)
#define MMA_F32_OFFS_MAX_ROOT_ENTRIES   (17)
#define MMA_F32_OFFS_NMB_SEC_VOL16      (19)
#define MMA_F32_OFFS_NMB_SEC_FAT16      (22)
#define MMA_F32_OFFS_NMB_SEC_PER_TRK    (24)
#define MMA_F32_OFFS_NMB_HEADS 		  	(26)
#define MMA_F32_OFFS_NMB_SEC_VOL32      (32)
#define MMA_F32_OFFS_NMB_SEC_FAT32      (36)
#define MMA_F32_OFFS_EXT_FLAGS					(40)
#define MMA_F32_OFFS_VERSION  					(42)
#define MMA_F32_OFFS_ROOT_DIR_CLUS      (44)
#define MMA_F32_OFFS_FSINFO_SECTOR			(48)
#define MMA_F32_OFFS_BACKUP_BS_SECTOR		(50)
#define MMA_F32_OFFS_FSINFO_FREE_COUNT	(488)
#define MMA_F32_OFFS_FSINFO_NEXT_FREE		(492)

/*******Formatting*************************************************************/
#define MMA_F32_RESERVED_SECTOR_SIZE	(512)
#define MMA_F32_FORMAT_NMB_RES_SECT		(38)    /* In My PC its formating with reserved sectors of 38 */
#define MMA_F32_FORMAT_NMB_FAT_TABLES	(2)
#define MMA_F32_FSINFO_LEAD_SIGN			(0x41615252)
#define MMA_F32_FSINFO_STRUC_SIGN			(0x61417272)
#define MMA_F32_FSINFO_NOT_SUPPORTED	(0xFFFFFFFF)

#define MMA_F32_OFFS_MEDIA_DESCR					(21)
#define MMA_F32_OFFS_PARTITION_NAME				(71)
#define MMA_F32_OFFS_FILESYSTEM_TYPE      (82)
#define MMA_F32_OFFS_EXECUTABLE_MARKER		(510)

#define MMA_F32_OFFS_FSINFO_LEAD_SIGN			(0)
#define MMA_F32_OFFS_FSINFO_STRUC_SIGN  	(484)
/**************** MACROS for FAT type based on Size ***************************/
#define MMA_MAX_FAT12_SIZE_MB 			(4)          /* 4MB , configurable, More than 4MB , media will be formatted with FAT16 by target */
#define MMA_MAX_FAT16_SIZE_MB 			(64)        /* 64MB , configurable, More than 64MB , media will be formatted with FAT32 by target */

#define MMA_DEFAULT_SECTOR_SIZE			(512)
#define MMA_1MB_SIZE_BLOCKS				((1024*1024)/MMA_DEFAULT_SECTOR_SIZE)
#define MMA_MAX_FAT12_BLOCKS            (MMA_MAX_FAT12_SIZE_MB*MMA_1MB_SIZE_BLOCKS)
#define MMA_MAX_FAT16_BLOCKS            (MMA_MAX_FAT16_SIZE_MB*MMA_1MB_SIZE_BLOCKS)



#define MMA_MAX_CHECKS_FAT12_BLOCKS     (8*MMA_1MB_SIZE_BLOCKS)   /* Maximum Support of 8MB */
#define MMA_MIN_CHECKS_FAT16_BLOCKS     (4*MMA_1MB_SIZE_BLOCKS)   /* Minimum Support of 4MB */
#define MMA_MIN_CHECKS_FAT32_BLOCKS     (32*MMA_1MB_SIZE_BLOCKS)   /* Minimum Support of 32MB */


#define RFSFAT_AUTO_FORMAT_ENABLE 0   /*No Autoformat */



/******************************************************************************
*      Module specific Macros and Typedefs                                    *
*******************************************************************************/
/******************************************************************************
*      Definition global Variables                                            *
*******************************************************************************/

/******************************************************************************
*      Definition of static-Variables                                         *
*******************************************************************************/

/******************************************************************************
*      mountpoint table with most important mountpoint information                    *
******************************************************************************/

/******************************************************************************
*     Boot-sector offsets                                                     *
******************************************************************************/
const UINT8 aucBootsector_offsets[] = {
  0x0b,							/* OFFSET # bytes / sector                                            */
  0x0d,							/* OFFSET # sectors / cluster                                         */
  0x0e,							/* OFFSET # reserved sectors                                          */
  0x10,							/* OFFSET # FAT tables                                                */
  0x11,							/* OFFSET maximum root entries                                        */
  0x16,							/* OFFSET # sectors in a FAT table                                    */
  0x13,							/* OFFSET # sectors (small number of sectors, <32Mbyte)               */

  /****************************************************************************/
  /* Only used for creating a new boot-sector                                 */
  /* When changing something here, please check the value MAX_BS_READ         */
  /****************************************************************************/

  0x00,							/* OFFSET_JMP_CODE Only needed for creating a bootsector              */
  0x15,							/* OFFSET_MEDIA_DESCR media descriptor                                */
  0x2B,							/* offset partition name                                                                                            */
  0x36,            				/* offset FileSystem Type                                             */
  0x18,							/*  Number of serctors per track     								  */
  0X1A, 						/*  Number of sides in the media     								  */
  0xFF							/* dummy OFFSET for the executable marker                             */
};

/******************************************************************************
*      FAT32 items                                                            *
******************************************************************************/
typedef struct PART_SIZE_2_SECT_P_CLUS{
	UINT32	partitionSize;
	UINT8		sectorsPerCluster;
	}PART_SIZE_2_SECT_P_CLUS;

const PART_SIZE_2_SECT_P_CLUS partSizeTableFat32[] = {
  {       66600,  0},	//up to 32.5 MB. The 0 value should trip an error!
	{      532480,  1},	//up to 260 MB, .5k cluster
	{    16777216,  8},	//up to   8 GB,  4k cluster
	{    33554432, 16},	//up to  16 GB,  8k cluster
	{    67108864, 32},	//up to  32 GB, 16k cluster
	{  0xFFFFFFFF, 64}	//  >    32 GB, 32k cluster
	};



/******************************************************************************
*      Static-Function prototypes                                             *
******************************************************************************/

static BOOL bcheckFormatsigature (const UINT8 mpt_idx);
static BOOL bReadWriteBS (const UINT8 mpt_idx, const UINT8 ucMode);
static BOOL bGetBootSectorFat32 (const UINT8 mpt_idx);
static BOOL bCheckBootsectorValue (const UINT8 ucItem, UINT8 mpt_idx);
static UINT8 MMA_ucDirectRW (const UINT8 mpt_idx, const UINT8 ucMode,
							 RO_PTR_TO_RW (MMA_RW_STRUCT) pstRW);
static BOOL bCalcBootsector (UINT8 ucVolume);
static BOOL bCalcSides(UINT16 *ucSpt,UINT16 *uxSides,UINT32 ucTotalSector);
static T_GBI_FS_TYPE calc_fat_fstype(const UINT8 mpt_idx);
/******************************************************************************
*      Implementation of Global Function                                      *
******************************************************************************/

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8
MMA_ucInit (void)
/* PURPOSE      : Initialising of Media manager
 * ARGUMENTS    : N/A
 * RETURNS      : FFS_NO_ERRORS
 *                RFSFAT_INTERNAL_ERR
 * PRECONDITION : None
 * POSTCONDITION: All mountpoints are initialised and mountpoint information is stored
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: This is the first function to execute for the filesystem
 *****************************************************************************/
{
  UINT8 mpt_idx = 0;			/* Index of partition                           */

  UINT8                   status = FFS_NO_ERRORS;


  RFSFAT_SEND_TRACE_DEBUG_HIGH ("RFSFAT: Initialize MMA ... start \n");
  /* delete the older memory */
  RFSFAT_FREE_BUF(MMA_BOOT_SECTOR);
  //Get memory for partition specific data table
  RFSFAT_GETBUF(((sizeof (MMA_BOOT_SECTOR_INFO_STRUCT))*NR_OF_MOUNTPOINTS),(&(MMA_BOOT_SECTOR)));

  //FOREACH mountpoint
  for (mpt_idx = 0; mpt_idx < NR_OF_MOUNTPOINTS; mpt_idx++)
	{

	    MMA_ucInit_mpt(mpt_idx);


#ifdef ICT_SPEEDUP_RFSFAT2		
		Reset_FAT_TABLE_cache(mpt_idx);   /* reset the FAT caches */
#endif
 		
	}

  /* Reset the optimization cluster setting */
  rfsfat_env_ctrl_blk_p->ffs_globals.clusterBaseNmb = 0xFFFFFFFE;

  rfsfat_env_ctrl_blk_p->ffs_globals.bInitialised = TRUE;
  RFSFAT_SEND_TRACE_DEBUG_HIGH ("RFSFAT: Initialize MMA ... complete \n");
  return (FFS_NO_ERRORS);
}



void MMA_ucInit_mpt(UINT8 mpt_idx)
{
  UINT8 status;

  if(MPT_TABLE[mpt_idx].is_mounted == TRUE)
  {


#if FREE_SPACE_OPT  

   /* No free space , we have to calculate */
   MPT_TABLE[mpt_idx].free_space = INVALID_FREE_SPACE_VAL;
 
#endif

  
     //Initialise some members
     MMA_BOOT_SECTOR[mpt_idx].PartStatus = FFS_RV_UNFORMATTED; //Assume not formatted
     MMA_BOOT_SECTOR[mpt_idx].BootSectIdx = INVALID_PART; //BS not readed yet
     //check and create bootsector list
     status = MMA_ucGetBootSectorInfo (mpt_idx);

     if (status == FFS_NO_ERRORS)
     {
         RFSFAT_SEND_TRACE_PARAM_DEBUG_HIGH("RFSFAT: MMA_ucInit ...formtted mount point idx \n\r", mpt_idx);
				 		  // Partition formatted
         MMA_BOOT_SECTOR[mpt_idx].PartStatus = FFS_RV_FORMATTED;
     }	   /* End of If */
     else if(status == FFS_RV_UNFORMATTED)
     {
         /* its an unformtted card */
	 RFSFAT_SEND_TRACE_PARAM_DEBUG_HIGH("RFSFAT: MMA_ucInit ...unformtted mount point idx \n\r", mpt_idx);

         if((MMA_FS_TYPE(mpt_idx) == GBI_UNKNOWN)
             && (MPT_TABLE[mpt_idx].media_type != GBI_SD)
             && (MPT_TABLE[mpt_idx].media_type != GBI_MMC))
     	 {
             RFSFAT_SEND_TRACE_DEBUG_HIGH ("RFSFAT: MMA_ucInit ... File system type is unkown \n \r");
	     /* The File system is know, then determine the file system */
             MMA_FS_TYPE(mpt_idx) = calc_fat_fstype(mpt_idx);
         }

#ifdef RFSFAT_AUTO_FORMAT_ENABLE
         RFSFAT_SEND_TRACE_DEBUG_HIGH ("RFSFAT: MMA_ucInit ... Formating the File system \n \r");

          /* Auto format: perform only for internal partitions */
         if((MMA_FS_TYPE(mpt_idx) != GBI_UNKNOWN)
             && (MPT_TABLE[mpt_idx].media_type != GBI_SD)
             && (MPT_TABLE[mpt_idx].media_type != GBI_MMC))
     	 {
             if (E_FFS_ucDiskFormat(mpt_idx, MPT_TABLE[mpt_idx].partition_name,GBI_UNKNOWN) != FFS_NO_ERRORS)
             {
                 RFSFAT_SEND_TRACE_DEBUG_HIGH ("RFSFAT: MMA_ucInit ... Formating the File system failed \n \r");
             }
	     else
	     {
	         MMA_BOOT_SECTOR[mpt_idx].PartStatus = FFS_RV_FORMATTED;
                 RFSFAT_SEND_TRACE_DEBUG_HIGH ("RFSFAT: MMA_ucInit ... Formating the File system Success \n \r");
	     }
         }
#else
         MMA_BOOT_SECTOR[mpt_idx].PartStatus = FFS_RV_UNFORMATTED;   /* set this an unformated */
#endif
     }   /* End of else If (status == FFS_RV_FORMATED) */
     else
     {
	 RFSFAT_SEND_TRACE_PARAM_DEBUG_HIGH("RFSFAT: MMA_ucInit ...Boot sector read error mount point idx \n\r", mpt_idx);

         /* Some other fatal error, Not able to read the boot sector */
	 MPT_TABLE[mpt_idx].is_mounted = FALSE;   /* Unmount this partition */
     }
  } /* End of If */
}


/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8
MMA_ucGetBootSectorInfo (const UINT8 mpt_idx)
/* PURPOSE      : This function reads the boot-sector of a mountpoint
 * ARGUMENTS    : mpt_idx indicates the index in the mountpoint table where
 *                media and partition info can be retrieved
 *
 * RETURNS      : FFS_NO_ERRORS  - Boot-sector is read.
 *                MMA_READ_ERROR - Reading boot-sector failed.
 * PRECONDITION : mountpoint must be formatted
 * POSTCONDITION: Bootsector is read from mountpoint and stored in global struct
 *                MMA_stBootSector.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  BOOL bActionFailed = TRUE; //initial: error
  UINT16  fs_type= GBI_UNKNOWN;
  UINT8 status;
#ifdef ICT_SPEEDUP_RFSFAT4
  UINT32 ulMaxDataClusters;
#endif

  /****************************************************************************
  * Check if boot-sector is already read                                      *
  * The first time the boot-sector must be read from the mountpoint.          *
  * But it doesn't need to be read again  because it will not change unless a *
  * new media is inserted.                                                    *
  * When formatting the mountpoint a new boot-sector will be written.         *
  * Normaly the new boot-sector must be reloaded but in this case the        *
  * structure stBootsector is already filled with the create bootsector       *
  * function.                                                                 *
  *****************************************************************************/
  if (mpt_idx == MMA_BOOT_SECTOR[mpt_idx].BootSectIdx)
	{
	  /* Current bootsector is already read                                     */
	  return FFS_NO_ERRORS;
	}
  /* flush the write cache  & Kill the Readh cache also */
	flush_write_blk_kill_cache();

  /* check only if not a raw tflash/mmc */
  if (MPT_TABLE[mpt_idx].filesys_type != GBI_UNKNOWN)
  {
      if((status = MMA_getfstype(mpt_idx, &fs_type)) != FFS_NO_ERRORS)
      {
         return status;     /* Error in reading a boot block  */
      }
  }

   if(fs_type == GBI_UNKNOWN)
   	{
   	     /* There is no file system , its raw disk */
		 return FFS_RV_UNFORMATTED;   /* Unformatted */
   	}


   if(MMA_FS_TYPE(mpt_idx) != fs_type)
   	{
   	   RFSFAT_SEND_TRACE_PARAM ("RFSFAT: MMA_ucGetBootSectorInfo File system type stored is not matching with the boot sector , for mount point ",mpt_idx, RV_TRACE_LEVEL_WARNING);
	   RFSFAT_SEND_TRACE_PARAM ("RFSFAT: MMA_ucGetBootSectorInfo setting the file system ",fs_type, RV_TRACE_LEVEL_WARNING);

   	   MMA_FS_TYPE(mpt_idx) = fs_type;
   	}

  /* Read new Bootsector information from mountpoint mpt_idx                     */
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
	{
	  MMA_BOOT_SECTOR[mpt_idx].fatEntrySize = sizeof(UINT32);
 	  bActionFailed  = bGetBootSectorFat32(mpt_idx); //keep FAT16 and FAT32 separated
	}
  else
	{
  	  MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster = 0xFFFF;      /* for FAT16, this value should be invalid
 																	and it should trigger error if it is accessed */

  		if(MMA_FS_TYPE(mpt_idx) == GBI_FAT12)
  			{
  			MMA_BOOT_SECTOR[mpt_idx].fatEntrySize = sizeof(UINT8);   /* its suppose to be 1.5 ,
  																but 1 is better*/
  			}
		else
		{
 	  MMA_BOOT_SECTOR[mpt_idx].fatEntrySize = sizeof(UINT16);
  		}
    bActionFailed = bReadWriteBS (mpt_idx, READ_MODE);
  }

/* flush the write cache  & Kill the Readh cache also */
	flush_write_blk_kill_cache();

  if (bActionFailed != FALSE)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: Bootsector read error", RV_TRACE_LEVEL_ERROR);
	  /* Error reading the bootsector                                           */
	  return MMA_RET_READ_ERR;
	}
#ifdef ICT_SPEEDUP_RFSFAT4
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

  /* Fill this maximum data cluster to Boot sector information */
  MMA_BOOT_SECTOR[mpt_idx].ulMaxDataClusters = ulMaxDataClusters;

  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("RFSFAT:Maximum Number of Data cluster are ",ulMaxDataClusters);

#endif
  /* Mark bootsector as read (for this mountpoint)                                */
  MMA_BOOT_SECTOR[mpt_idx].BootSectIdx = mpt_idx;

  return FFS_NO_ERRORS;
}

UINT8 MMA_ucSetBootSectorLabel
(const UINT8 mpt_idx, const UINT8 * name)
/* PURPOSE      :
 * ARGUMENTS    : mpt_idx indicates the mountpoint number.
 * RETURNS      : FFS_NO_ERRORS - No error occured
 *
 * PRECONDITION : mountpoint must be initialised by the media manager.
 * POSTCONDITION:
 * NOTE: -
 *****************************************************************************/
{
    MMA_RW_STRUCT stReadWrite={0,0,NULL};

    // Update label in the boot sector & write to the media
    rfsfat_vpMemset(MMA_BOOT_SECTOR[mpt_idx].partname, 0x20,
		                   MAX_PART_NAME);
    rfsfat_vpMemcpy(MMA_BOOT_SECTOR[mpt_idx].partname, (void *) name,
                                  rfsfat_uiStrlen((UINT8*)name));

    if ((MMA_FS_TYPE(mpt_idx) != GBI_FAT32) && (MMA_FS_TYPE(mpt_idx)!=GBI_FAT32_LBA))
    {
    	// For FAT12/FAT16
       if (bCalcBootsector (mpt_idx) == FALSE)
       {
	    return MMA_RET_INCOR_BOOTRECORD;
       }

       /* Write to the media                                                       */
       if (bReadWriteBS (mpt_idx, WRITE_MODE) != FALSE)
       {
	    return MMA_RET_WRITE_ERR;
        }
    }
    else //     if (MMA_FS_TYPE(mpt_idx) != GBI_FAT32)
    {
      	// For FAT32
	stReadWrite.tOffset = MMA_F32_OFFS_PARTITION_NAME;
       stReadWrite.pucBuffer = MMA_BOOT_SECTOR[mpt_idx].partname;
       stReadWrite.tNrBytes = MAX_PART_NAME;
       if (MMA_ucDirectWrite (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
       {
  	     return MMA_RET_WRITE_ERR;
       }
    }
     return FFS_NO_ERRORS;
 }

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 MMA_ucDirectRead
  (const UINT8 mpt_idx, RO_PTR_TO_RW (MMA_RW_STRUCT) pstRead)
/* PURPOSE      : Function reads a number of bytes from the mountpoint without
 *                cluster information. With this function it's possible to read
 *                at any location on the mountpoint.
 * ARGUMENTS    : mpt_idx indicates the mountpoint number.
 *                pstRead is a pointer to the structure with read information
 *                like the number of bytes to read, the offset to start reading
 *                and the buffer to store the read bytes.
 * RETURNS      : FFS_NO_ERRORS - No error occured
 *                MMA_RET_READ_ERR - Read error while reading bytes from media
 *
 * PRECONDITION : mountpoint must be initialised by the media manager.
 * POSTCONDITION: A number of bytes are read from the mountpoint.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  return MMA_ucDirectRW (mpt_idx, READ_MODE, pstRead);
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 MMA_ucDirectWrite
  (const UINT8 mpt_idx, RO_PTR_TO_RW (MMA_RW_STRUCT) pstWrite)
/* PURPOSE      : Function writes a number of bytes to the mountpoint without
 *                cluster information. With this function it's possible to
 *                write at any location on the mountpoint.
 * ARGUMENTS    : mpt_idx indicates the mountpoint number.
 *                pstWrite is a pointer to the structure with write information
 *                like the number of bytes to write, the offset to start
 *                writing and the buffer which hold the bytes to write
 * RETURNS      : FFS_NO_ERRORS  - No error occured
 *                MMA_RET_WRITE_ERR - Write error occured while writing bytes
 *                                    to the media
 *                MMA_RET_NO_SPACE  - There is no space left on the media,
 *                                    writing to the media is aborted.
 * PRECONDITION : Media manager must be initialised.
 * POSTCONDITION: A number of bytes are written to the mountpoint.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: This function can be used for formatted and unformatted mountpoints.
 *****************************************************************************/
{
  return MMA_ucDirectRW (mpt_idx, WRITE_MODE, pstWrite);
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
MMA_ucGetPartitionStatus (const UINT8 mpt_idx)
/* PURPOSE      : This function is used to determine the mountpoint status.
 *                The status of a mountpoint can be:
 *                - Unformatted
 *                - Formatted
 *                - Damaged
 * ARGUMENTS    : mpt_idx indicates the mountpoint number.
 * RETURNS      : FFS_RV_FORMATTED   - mountpoint is formatted
 *                FFS_RV_UNFORMATTED - mountpoint is unformatted
 *                FFS_RV_UNAVAILABLE - mountpoint is damaged, reading mountpoint
 *                                  information failed.
 * PRECONDITION : Media manager must be initialised
 * POSTCONDITION: mountpoint status is returned
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 ******************************************************************************/
{
  /* Check if mountpoint exists                                                   */
  if ((mpt_idx >= NR_OF_MOUNTPOINTS) || (rfsfat_env_ctrl_blk_p->ffs_globals.
										 bInitialised == FALSE))
  {
     RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FFS_RV_UNAVAILABLE");
     return FFS_RV_UNAVAILABLE;
  }

  if (MPT_TABLE[mpt_idx].filesys_type == GBI_UNKNOWN)
  {
     RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FFS_RV_UNFORMATTED");
     return FFS_RV_UNFORMATTED;
  }

  //the mountpoint table is assigned to rfsfat because formatted media were
  //recognised. we can assume that there are no mountpoints delivered to us other
  //than formatted
  return FFS_RV_FORMATTED;
}


/******************************************************************************
*      Implementation of Local (= static) Function                            *
******************************************************************************/


/******Copyright by ICT Embedded BV All rights reserved **********************/
static BOOL
bcheckFormatsigature (const UINT8 mpt_idx)
/* PURPOSE      : This function checks if a mountpoint is formatted by reading the
 *                executable marker from the boot-sector. The value of the
 *                marker must be EXEC_MARKER for a formatted mountpoint.
 * ARGUMENTS    : mpt_idx : mountpoint indication
 * RETURNS      : TRUE  : mountpoint is formatted
 *                FALSE : mountpoint is not formatted
 * PRECONDITION : start position of partition and media function must be present
 * POSTCONDITION: mountpoint status is returned
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  MMA_RW_STRUCT stReadBytes={0,0,NULL};	/* Struct used for reading from the mountpoint      */
  UINT16 uiBootSectMarker;		/* Integer value to store the excecutable marker */

  /****************************************************************************
  * Determine mountpoint status (FORMATTED / UNFORMATTED)                         *
  * Read executable marker from bootsector to determine if the mountpoint is      *
  * formatted                                                                 *
  *****************************************************************************/
  //offset from the start off the bootsector of the specified mountpoint
  stReadBytes.tOffset = OFFSET_EXEC_MARKER;
  stReadBytes.tNrBytes = sizeof (uiBootSectMarker);
  stReadBytes.pucBuffer = (UINT8 *) & uiBootSectMarker;

  if (MMA_AST_MEDIA_FUNCTIONS.MMA_bFuncRead (&stReadBytes, mpt_idx) ==
	  RFSFAT_OK)
	{
	  /* Determine if executable marker is set in the bootsector                */
	  if ((UINT16) uiBootSectMarker == EXEC_MARKER)
		{

		  return TRUE;			/* mountpoint is formatted                                    */
		}
	}
  return FALSE;					/* mountpoint read error or mountpoint is not formatted */
}


/******Copyright by ICT Embedded BV All rights reserved **********************/
static BOOL
bGetBootSectorFat32(const UINT8 mpt_idx)
/* PURPOSE      : This function is used for reading the bootsector
 *                for FAT32 partitions.
 * ARGUMENTS    : mpt_idx: index mountpoint table
 * RETURNS      : TRUE  : an error occured
 *                FALSE : no error.
 * PRECONDITION : Media manager must be initialised
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
	UINT8 fat32Bs[MMA_F32_BOOT_SECTOR_INFO_SIZE];
 	MMA_RW_STRUCT stReadWrite={0,0,NULL};
	UINT8 u8Var1 = 0;
	UINT16 u16Var1 = 0;
	UINT32 u32Var1 = 0;
	UINT8  u8Index;
	UINT8  u8ShiftVal = 1;
	BOOL   bValueOk = FALSE;
	BOOL   bCorrectBootsector = FALSE;



	//Read & verify the FAT32 Boot sector


  //Start reading the part of the BS that is of interest (from media)
	stReadWrite.tOffset = 0; //BS starts at zero
  stReadWrite.pucBuffer = (UINT8*)fat32Bs;
  stReadWrite.tNrBytes = MMA_F32_BOOT_SECTOR_INFO_SIZE;
  if (MMA_ucDirectRead (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
  {

  	return TRUE;
  }

	//***********
	//Start validate and saving
	//***********

	//***********BytesPerSector
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_BYTES_PER_SEC]), sizeof(UINT16));
  if ((u16Var1 != 512) && (u16Var1 != 1024) && (u16Var1 != 2048) && (u16Var1 != 4096))
	{
	  RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. Bytes per sector wrong",RV_TRACE_LEVEL_ERROR);
    return TRUE;
 	}
	MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector = u16Var1;

  //***********SectorPerCluster
	u8Var1 = fat32Bs[MMA_F32_OFFS_SEC_PER_CLUS];
	//Validate. The value Sect/cluster should be: 1,2,4,8,16,32 or 128
	u8ShiftVal = 1;
	bValueOk = FALSE;
  for (u8Index = 0; u8Index < 7 ; u8Index++)
  {
  	if (u8Var1 == (u8ShiftVal << u8Index))
  	{
  		bValueOk = TRUE;
  		break;
  	}
  }
	if (bValueOk == FALSE)
	{
	  RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. Bytes per sector wrong",RV_TRACE_LEVEL_ERROR);
    return TRUE;
 	}
  MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster = u8Var1;

  //***********ReservedSectors
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_NMB_RESERVED_SEC]), sizeof(UINT16));
  //Validate. The value Reserved Sectors is typical 32 (other values not recommend)
	// LCS: Due to a Windows XP formatting bug this value can be set to 38
	if ((u16Var1 != 32) || (u16Var1 != 38))
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. nmb reserved clusters not typical 32",RV_TRACE_LEVEL_WARNING);
	}
	MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors = u16Var1;


  //***********NrFatTables
	u8Var1 = fat32Bs[MMA_F32_OFFS_NMB_FAT];
	//Validate. The number of FAT tables should be: 1,2,3....(recommended is 2)
	if (u8Var1 != 2)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. nmb FAT tables not 2",RV_TRACE_LEVEL_WARNING);
	  fat32Bs[MMA_F32_OFFS_NMB_FAT] = 2;
    bCorrectBootsector = TRUE;
  }
	MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables = 2;

  //***********MaxRootEntries: Not used in FAT32
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_MAX_ROOT_ENTRIES]), sizeof(UINT16));
  //Validate. The value should be 0 for FAT32

	if (u16Var1 != 0)
	{
#ifdef FAT32_TII_MODIFICATION
    u16Var1=0;
	memcpy((void*)&(fat32Bs[MMA_F32_OFFS_MAX_ROOT_ENTRIES]),(const void*)&u16Var1, sizeof(UINT16));
	RFSFAT_SEND_TRACE("RFSFAT: BS readFAT32. Making root entry count as Zero", RV_TRACE_LEVEL_WARNING);
#else
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. max. root entries not zero",RV_TRACE_LEVEL_ERROR);
    return TRUE;
#endif
	}
	//By setting uiMaxRootEntries to the value that matches one cluster,
	//     the root size users will stil work in FAT32 mode.
  MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries =
								(MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
								 MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) / 32;

  //***********NrSectorsVol:
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_NMB_SEC_VOL16]), sizeof(UINT16));
  //Validate. The value should be 0 for FAT32
	if (u16Var1 != 0)
	{
	#ifdef FAT32_TII_MODIFICATION
    u16Var1=0;
	memcpy((void*)&(fat32Bs[MMA_F32_OFFS_NMB_SEC_VOL16]),(const void*)&u16Var1, sizeof(UINT16));
	RFSFAT_SEND_TRACE("RFSFAT: BS readFAT32. Making old nmb sectors of volume as zero ", RV_TRACE_LEVEL_WARNING);
#else
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. old nmb sectors of volume not zero",RV_TRACE_LEVEL_ERROR);
    return TRUE;
#endif
	}
  //Get FAT32 value
  memcpy((void*)&u32Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_NMB_SEC_VOL32]), sizeof(UINT32));
	if (u32Var1 == 0)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. nmb sectors of volume zero",RV_TRACE_LEVEL_ERROR);
    return TRUE;
	}
	MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol = u32Var1;

  //***********NrSectorsFAT:
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_NMB_SEC_FAT16]), sizeof(UINT16));
  //Validate. The value should be 0 for FAT32
	if (u16Var1 != 0)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. old nmb sectors per fat not zero",RV_TRACE_LEVEL_ERROR);
    return TRUE;
	}
  //Get FAT32 value
	memcpy((void*)&u32Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_NMB_SEC_FAT32]), sizeof(UINT32));
	if (u32Var1 == 0)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. nmb sectors per fat zero",RV_TRACE_LEVEL_ERROR);
    return TRUE;
	}
	MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT = u32Var1;

  //***********FAT Mirroring:
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_EXT_FLAGS]), sizeof(UINT16));
  //Validate. Mirrorong should be disabled
	if (((u16Var1 & 0x008F) != 0x0080) && (u16Var1 != 0))
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. FAT mirroring disabled",RV_TRACE_LEVEL_WARNING);
		u16Var1 = 0;
  	memcpy((void*)&(fat32Bs[MMA_F32_OFFS_EXT_FLAGS]),(const void*)&u16Var1, sizeof(UINT16));
    bCorrectBootsector = TRUE;
	}


  //***********FS Version:
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_VERSION]), sizeof(UINT16));
  //Validate. Only version 0.0 is supported
	if (u16Var1 != 0)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. FS Version not 0.0",RV_TRACE_LEVEL_ERROR);
    return TRUE;
	}

	//***********First directory cluster number:
	memcpy((void*)&u32Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_ROOT_DIR_CLUS]), sizeof(UINT32));
  //Validate. Typical 2
	if (u32Var1 != MMA_F32_ROOT_CLUSTER_NMB)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. Root directory cluster nmb not 2",RV_TRACE_LEVEL_ERROR);
	}
  MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster = u32Var1;

	//***********FileSystem Info structure sector number:
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_FSINFO_SECTOR]), sizeof(UINT16));
  //Validate. Typical 1
	if ((u16Var1 == 0) || (u16Var1 > MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors))
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. FSINFO sector nmb invallid",RV_TRACE_LEVEL_ERROR);
    return TRUE;
	}
	MMA_BOOT_SECTOR[mpt_idx].fsInfoSector = u16Var1;


	//***********Backup Bootsector number:
	memcpy((void*)&u16Var1,(const void*)&(fat32Bs[MMA_F32_OFFS_BACKUP_BS_SECTOR]), sizeof(UINT16));
  //Validate. Typical 6
	if (u16Var1 > MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors)
	{
  	RFSFAT_SEND_TRACE("RFSFAT: BS read FAT32. FSINFO sector nmb invallid",RV_TRACE_LEVEL_ERROR);
    return TRUE;
	}
	if (u16Var1 != 0)
	{ //no backup sector
		u16Var1 = 0;
  	memcpy((void*)&(fat32Bs[MMA_F32_OFFS_BACKUP_BS_SECTOR]),(const void*)&u16Var1, sizeof(UINT16));
    bCorrectBootsector = TRUE;
	}


	if (bCorrectBootsector == TRUE)
	{
	  // Write the bootsector because we changed something
  	if (MMA_ucDirectWrite (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
	  {
    return TRUE;
	  }
	}


  //**************
	//we do not support the FSINFO structure usage.
	//Force it to not supported.
	//**************
	u32Var1 = MMA_F32_FSINFO_NOT_SUPPORTED ; //not supported value
	stReadWrite.tOffset = (MMA_BOOT_SECTOR[mpt_idx].fsInfoSector *
												 MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) +
                         MMA_F32_OFFS_FSINFO_FREE_COUNT;
  stReadWrite.pucBuffer = (UINT8*)&u32Var1;
  if (MMA_ucDirectWrite (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
  {
  	return TRUE; //error trace already done in called function
  }
    u32Var1 = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
	stReadWrite.tOffset += sizeof(UINT32); //point to FSINFO NEXT FREE
	stReadWrite.pucBuffer = (UINT8*)&u32Var1;
  if (MMA_ucDirectWrite (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
  {
  	return TRUE; //error trace already done in called function
  }

  return FALSE; //All ok
}


/******Copyright by ICT Embedded BV All rights reserved **********************/

static BOOL
bReadWriteBS (const UINT8 mpt_idx, const UINT8 ucMode)
/* PURPOSE      : This function is used for reading or writing the bootsector
 * ARGUMENTS    : mpt_idx: index mountpoint table
 *                ucMode   : Read or Write mode
 * RETURNS      : TRUE  : An error occured
 *                FALSE : no error.
 * PRECONDITION : Media manager must be initialised
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT8 ucResult = FFS_NO_ERRORS;
  UINT8 ucNrOfItems;
  UINT8 ucStructIndex;
  MMA_RW_STRUCT stReadWrite={0,0,NULL};
  UINT8 ucBuffer[BS_FSTYPE_FIELD_LENGTH];
  UINT16 ucSpt;
  UINT16 ucSides;

  if (ucMode == READ_MODE)
	{
	  /* Limit for loop for the items to read only                              */
	  ucNrOfItems = MAX_BS_READ + 1;
	}
  else
	{
	  /* All items in the for loop will be processed                            */
	  ucNrOfItems = sizeof (aucBootsector_offsets);
	  bCalcSides(&ucSpt,&ucSides,MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol);
	}

  for (ucStructIndex = 0; ucStructIndex < ucNrOfItems; ucStructIndex++)
	{

	  stReadWrite.tOffset = aucBootsector_offsets[ucStructIndex];

	  switch (ucStructIndex)
		{
		case ST_MEMBER_0:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_0");
		  /* Set buffer and the number of bytes to read/write                     */
		  /* The value of bytes per sector should be: 512,1024,2048 or 4096       */
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;
		  stReadWrite.tNrBytes =
			sizeof (MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
		  break;
		case ST_MEMBER_1:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_1");
		  /* Set buffer and the number of bytes to read/write                     */
		  /* The value Sect/cluster should be: 1,2,4,8,16,32 or 128               */
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster;
		  stReadWrite.tNrBytes =
			sizeof (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster);
		  break;
		case ST_MEMBER_2:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_2");
		  /* Set buffer and the number of bytes to read/write                     */
		  /* The value Reserved Sectors should be: 1 (higher values not recommend) */
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors;
		  stReadWrite.tNrBytes =
			sizeof (MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors);
		  break;
		case ST_MEMBER_3:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_3");
		  /* Set buffer and the number of bytes to read/write                     */
		  /* The number of FAT tables should be: 1,2,3....(recommend is 2)        */
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables;
		  stReadWrite.tNrBytes =
			sizeof (MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables);
		  break;
		case ST_MEMBER_4:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_4");
		  /* Set buffer and the number of bytes to read/write                     */
		  /* The maximum ROOT entrie value should be: 512                         */
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries;
		  stReadWrite.tNrBytes =
			sizeof (MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries);
		  break;
		case ST_MEMBER_5:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_5");
		  /* Set buffer and the number of bytes to read/write                     */
		  /* The number of sectors per FAT should be: > 0                         */
		  if (ucMode == READ_MODE)
		  { //uiNrSectorsFAT member is UINT32 (for FAT32).
		    //If we read 2 bytes (FAT16) then we have to clear the upper bytes
		    MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT = 0;
		  }
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT;
		  stReadWrite.tNrBytes = sizeof(UINT16);
		  break;
		case ST_MEMBER_6:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS ST_MEMBER_6");
	  /************************************************************************
      * The position to read/write the number of sectors depend on the        *
      * partition type. But since Windows is not using the partition type, but*
      * the real size it is not very usefull anymore.                         *
      * To determine which position should be used we follow Windows by       *
      * calcualting the mountpoint size.                                          *
      *************************************************************************/

		  /* Determine the number of bytes in the partition                       */
		  stReadWrite.tOffset = OFFSET_SMALL_SECT;
		  stReadWrite.tNrBytes = SIZE_SMALL_SECT;
		  stReadWrite.pucBuffer =
			(UINT8 *) & MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol;
		  /*
		     Read the small number of sectors, if this value is zero than the large
		     number of sectors should be read.
		   */
		  if (ucMode == READ_MODE)
			{

			  MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol = 0;
			  (void) MMA_ucDirectRead (mpt_idx, &stReadWrite);
			  if (MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol == 0)
				{

				  stReadWrite.tOffset =OFFSET_BIG_SECT;
				  stReadWrite.tNrBytes =
					sizeof (MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol);
				}
			  else
				{
				  stReadWrite.tOffset =OFFSET_SMALL_SECT;
				}
			}
		  else
			if ((MPT_TABLE[mpt_idx].partition_size *
				 MPT_TABLE[mpt_idx].blk_size) > MAX_SMALL_PART)
			{

			  stReadWrite.tOffset = OFFSET_BIG_SECT;
			  stReadWrite.tNrBytes =
				sizeof (MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol);
			}
		  break;


		case WRITE_BS_JMP:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS WRITE_BS_JMP");
	  /************************************************************************
      *                                                                       *
      * Only for creating a bootsector                                        *
      *                                                                       *
      *************************************************************************/

		  /* When changing something here, please check MAX_BS_READ               */
		  /* The Jump Code is a fixed value and will not be checked               */
		  stReadWrite.pucBuffer =
			(UINT8 *) rfsfat_env_ctrl_blk_p->ffs_globals.aucJmpCode;
		  stReadWrite.tNrBytes =
			sizeof (rfsfat_env_ctrl_blk_p->ffs_globals.aucJmpCode);
		  break;

		case WRITE_BS_MD:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS WRITE_BS_MD");
	  /************************************************************************
      *                                                                       *
      * Only for creating a bootsector                                        *
      *                                                                       *
      *************************************************************************/
		  /* The media descriptor is a fixed value for all created boot-sectors   */
		  stReadWrite.pucBuffer =
			(UINT8 *) & rfsfat_env_ctrl_blk_p->ffs_globals.ucMediaDescr;
		  stReadWrite.tNrBytes =
			sizeof (rfsfat_env_ctrl_blk_p->ffs_globals.ucMediaDescr);
		  break;
		case WRITE_NAME:
		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS WRITE_NAME");
		  //write partition name
		  stReadWrite.pucBuffer = MMA_BOOT_SECTOR[mpt_idx].partname;
		  stReadWrite.tNrBytes =
			strlen ((const char *) MMA_BOOT_SECTOR[mpt_idx].partname);
		  break;
		  case WRITE_FSTYPE:
    	  //write FileSystem Type
if (MMA_FS_TYPE(mpt_idx) == GBI_FAT12)
{
      memcpy((void*)&(ucBuffer),(const void*)"FAT12   ", BS_FSTYPE_FIELD_LENGTH);
}
else
{
	  memcpy((void*)&(ucBuffer),(const void*)"FAT16   ", BS_FSTYPE_FIELD_LENGTH);
}
      	  stReadWrite.pucBuffer = ucBuffer;
      	  stReadWrite.tNrBytes = BS_FSTYPE_FIELD_LENGTH;
      	  break;

		case WRITE_MARKER:

		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS WRITE_MARKER");
	  /************************************************************************
      *                                                                       *
      * Only for creating a bootsector                                        *
      *                                                                       *
      *************************************************************************/
		  /* The EXECUTABLE MARKER is a fixed value for all created boot-sectors   */
		  stReadWrite.pucBuffer =
			(UINT8 *) rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker;
		  stReadWrite.tNrBytes =
			sizeof (rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker);

		  stReadWrite.tOffset =OFFSET_EXEC_MARKER;

		  break;
		case WRITE_SPT:
			stReadWrite.pucBuffer = (UINT8 *) &ucSpt;
			stReadWrite.tNrBytes =2;
			break;
		case WRITE_SIDES:
			stReadWrite.pucBuffer =(UINT8 *) &ucSides;
			stReadWrite.tNrBytes =2;
			break;
		default:
		  /* Error exit                                                           */
		  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: bReadWriteBS invallid case");
		  return TRUE;
		}
	/**************************************************************************
    * Determine if the boot-sector should be read or written                  *
    ***************************************************************************/
	  if (ucMode == READ_MODE)
		{
		  /* Read the bootsector                                                  */
		  //RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: bReadWriteBS DirectRead");

		  ucResult = MMA_ucDirectRead (mpt_idx, &stReadWrite);

	  /************************************************************************
      * Check the boot-sector values read from the mountpoint                     *
      * All boot-sector values fit in an UINT16 variable, that is why the check *
      * function has an UINT16 as parameter.                                    *
      *************************************************************************/
		  if (!bCheckBootsectorValue (ucStructIndex, mpt_idx))
			{
		/**********************************************************************
        * Boot-sector value is not correct, this may harm the file-system     *
        ***********************************************************************/
				RFSFAT_SEND_TRACE_ERROR ("RFSFAT: bReadWriteBS, Boot-sector value invallid");
			  return TRUE;
			}
		}
	  else if (ucMode == WRITE_MODE)
		{
		  /* Write the bootsector member to the mountpoint                            */
		  ucResult = MMA_ucDirectWrite (mpt_idx, &stReadWrite);
		}

	/**************************************************************************
    * Determine Return value                                                  *
    ***************************************************************************/
	  if (ucResult != FFS_NO_ERRORS)
		{
		  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: Error reading from media");
		  /* Error reading from media                                             */
		  return TRUE;
		}

	/**************************************************************************
    * Check Bootsector min/max values                                         *
    ***************************************************************************/

	  /* Read/Write next bootsector item                                        */
	}
  /* Reading or Writing the bootsector completed                              */
  return FALSE;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
static BOOL
bCheckBootsectorValue (const UINT8 ucItem, UINT8 mpt_idx)
/* PURPOSE      : Check on Bootsector value
 * ARGUMENTS    : ucItem: The Boot-sector item to check
 *                tValue: The Value to check
 * RETURNS      : TRUE  : Value is in range
 *                FALSE : Value incorrect
 * PRECONDITION : -
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE: -
 *****************************************************************************/
{
  BOOL bResult = FALSE;		/* Set result default at ERROR */
  UINT8 ucIndex;				/* Variable used in for loops                         */
  UINT8 ucShift = 0x01;			/* Set bit 0 to '1' of shift variable                 */
  UINT16 temp;

  switch (ucItem)
	{
	case ST_MEMBER_0:
	  /* The value of bytes per sector should be: 512,1024,2048 or 4096         */
	  temp = MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;
	  for (ucIndex = 1; ucIndex < 5; ucIndex++)
		{
		  if (temp == (ucIndex * 512))
			{
			  bResult = TRUE;
			  break;
			}
		}
	  break;
	case ST_MEMBER_1:
	  /* The value Sect/cluster should be: 1,2,4,8,16,32 or 64                  */
	  for (ucIndex = 0; ucIndex < 7; ucIndex++)
		{
		  /* ucShift will be used to get the allowed values                       */
		  if (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster ==
			  (ucShift << ucIndex))
			{
			  bResult = TRUE;
			  break;
			}
		}
	  break;
	case ST_MEMBER_2:
	  /* The value Reserved Sectors should be: 1 (higher values not recommend)  */
	  if (MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors != 0)
		{
		  bResult = TRUE;
		}
	  break;
	case ST_MEMBER_3:
	  /* Value must be > 0 to ensure that divisions can be made without trouble */
	  if (MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables > 0)
		{
		  bResult = TRUE;
		}
	  break;
	case ST_MEMBER_4:
	if (MMA_FS_TYPE(mpt_idx) == GBI_FAT12)
	{
   		if(MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries >= NEW_BS_MAX_ROOT_12)
   		{
   	 		bResult = TRUE;
   		}
	}
	else
	{
    	/* The maximum ROOT entry value should be: 512                            */
	  if (MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries >= NEW_BS_MAX_ROOT)
		{
		  bResult = TRUE;
		}
	}
	  break;
	case ST_MEMBER_5:
	  /* Value must be > 0 to ensure that divisions can be made without trouble */
	  if (MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT > 0)
		{
		  bResult = TRUE;
		}
	  break;
	case ST_MEMBER_6:
	  /* Value must be > 0 to ensure that divisions can be made without trouble */
	  if (MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol > 0)
		{
		  bResult = TRUE;
		}
	  break;

	default:
	  bResult = FALSE;
	 RFSFAT_SEND_TRACE_PARAM_ERROR("BS verify error, unknown switch value, partition: ",mpt_idx);
	}
	if (!bResult)
	{
		RFSFAT_SEND_TRACE_PARAM_ERROR("BS verify error, partition: ",mpt_idx);
		RFSFAT_SEND_TRACE_PARAM_ERROR("BS verify error, item: ",ucItem);
	}
  return bResult;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
static UINT8 MMA_ucDirectRW
  (const UINT8 mpt_idx,
   const UINT8 ucMode, RO_PTR_TO_RW (MMA_RW_STRUCT) pstRW)
/* PURPOSE      : Function reads a number of bytes from the mountpoint without
 *                cluster information. With this function it's possible to read
 *                at any location on the mountpoint.
 * ARGUMENTS    : mpt_idx indicates the mountpoint number.
 *                pstRead is a pointer to the structure with read information
 *                like the number of bytes to read, the offset to start reading
 *                and the buffer to store the read bytes.
 *                ucMode indicates read or write action
 * RETURNS      : FFS_NO_ERRORS - No error occured

 *
 * PRECONDITION : mountpoint must be initialised by the media manager.
 * POSTCONDITION: A number of bytes are read from the mountpoint.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  FFS_FILE_LENGTH_TYPE tRWOffset;
  UINT8 bRetVal;

  //we can assume that partition is always available

  /* Determine read/write startpoint                                          */
  /* Check if read/write point is within the variable range                   */
  /*lint -save -e685 -e568 */
#if (MPT_START != 0)
  if ((UINT32_MAX - pstRW->tOffset) < MPT_START)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: MMA_RET_RW_OUTSIDE_mountpoint",
						 RV_TRACE_LEVEL_ERROR);
	  return MMA_RET_RW_OUTSIDE_PARTITION;
	}
#endif
  /*lint -restore */
  /* Determine the absolute offset on the mountpoint                          */
  tRWOffset = MPT_START + pstRW->tOffset;

  /* Determine if number of bytes can be read/write and not exceed the
     partition */
  /* Check if read point is within the variable range                         */
  if ((UINT32_MAX - tRWOffset) < pstRW->tNrBytes)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: MMA_RET_RW_OUTSIDE_mountpoint",
						 RV_TRACE_LEVEL_ERROR);
	  return MMA_RET_RW_OUTSIDE_PARTITION;
	}

  if ((tRWOffset + pstRW->tNrBytes) >
	  (MPT_TABLE[mpt_idx].partition_size * MPT_TABLE[mpt_idx].blk_size))
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: MMA_RET_RW_OUTSIDE_mountpoint",
						 RV_TRACE_LEVEL_ERROR);
	  return MMA_RET_RW_OUTSIDE_PARTITION;
	}
  /* Set new offset to start reading/writing                                  */
  pstRW->tOffset = tRWOffset;

  /* Start reading/writing bytes                                              */
  if (ucMode == READ_MODE)
	{
	  bRetVal = MMA_AST_MEDIA_FUNCTIONS.MMA_bFuncRead (pstRW, mpt_idx);
	}
  else
	{
	  bRetVal = MMA_AST_MEDIA_FUNCTIONS.MMA_bFuncWrite (pstRW, mpt_idx);
	}
  if (bRetVal != RFSFAT_OK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: MMA Block read/write error", RV_TRACE_LEVEL_ERROR);
	  return MMA_RET_READ_ERR;


	}
  return FFS_NO_ERRORS;
}

/**MH**Copyright by ICT Embedded BV All rights reserved **********************/
UINT8
ucCreateBootSectorFat32 (const UINT8 mpt_idx, const UINT8 * name)
/* PURPOSE      : Function creates a new bootsector on volume (used by format)
 *                for FAT32
 * ARGUMENTS    : mpt_idx mountpoint index
 *                name: mountpoint(volume) name
 * RETURNS      : FFS_NO_ERRORS - Bootsector created on volume ucVolume.
 *                MMA_RET_WRITE_ERR - Bootsector not created.
 * PRECONDITION : Media manager must be initialised.
 * POSTCONDITION: Bootsector is created on volume.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
	UINT8 *fat32Bs_p;
 	MMA_RW_STRUCT stReadWrite={0,0,NULL};
	UINT16 u16Var1 = 0, u16Var2 = 0;
	UINT32 u32Var1 = 0;
	UINT32 u32Var2 = 0;
	UINT32 nrSectorsVol;
	UINT8	nrSectorsPerCluster;
	UINT8  u8Index;
	T_RVF_MB_STATUS mb_status;


#ifdef ICT_SPEEDUP_RFSFAT4
 UINT32	ulMaxDataClusters;

#endif
  //Get memory for partition specific data table
  mb_status = rvf_get_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
						               MMA_F32_RESERVED_SECTOR_SIZE,
						               (T_RVF_BUFFER **) &fat32Bs_p);
  if (mb_status == RVF_RED)
	{
	  // Not enough memory to allocate the buffer.
	  // The environment will cancel the RFSFAT instance creation.
	  RFSFAT_SEND_TRACE("RFSFAT: FAT32 wr BS.Error to get memory ",RV_TRACE_LEVEL_ERROR);
    return MMA_RET_WRITE_ERR;
	}
  else if (mb_status == RVF_YELLOW)
	{
	  // The flag is yellow, memory resource is critical.
	  RFSFAT_SEND_TRACE("RFSFAT:  FAT32 wr BS. Getting short on memory ",RV_TRACE_LEVEL_WARNING);
	}
	memset((void*)fat32Bs_p, 0x00,MMA_F32_RESERVED_SECTOR_SIZE);//clear BS



  //Start initializing BS

  /****************************************************************************
  * Fill boot-sector structure members with pre-defined values                *
  ****************************************************************************/
  MMA_BOOT_SECTOR[mpt_idx].fatEntrySize = sizeof(UINT32);   /* FAT Entry Size */
  MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors = MMA_F32_FORMAT_NMB_RES_SECT;
  MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables = MMA_F32_FORMAT_NMB_FAT_TABLES;
  MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries = 0;
  strncpy ((char *) MMA_BOOT_SECTOR[mpt_idx].partname, (const char *) name,MAX_PART_NAME);


  MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector = MPT_TABLE[mpt_idx].blk_size;

  //***********Determine the number of sectors in the partition
  nrSectorsVol = MPT_TABLE[mpt_idx].partition_size;  /* Partition size in terms of number of blocks */
  MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol = nrSectorsVol;

 	//***********Determine number of sectors in one cluster.
  nrSectorsPerCluster = 0;
  for (u8Index = 0;
       u8Index < (sizeof(partSizeTableFat32)/sizeof(struct PART_SIZE_2_SECT_P_CLUS));
       u8Index++)
  {
    if (nrSectorsVol <= partSizeTableFat32[u8Index].partitionSize)
    {
    	nrSectorsPerCluster = partSizeTableFat32[u8Index].sectorsPerCluster;
    	break;
    }
  }

  if (nrSectorsPerCluster == 0)
  {

	  RFSFAT_SEND_TRACE("RFSFAT: FAT32 wr BS. Size too small for FAT32",RV_TRACE_LEVEL_ERROR);
	  RFSFAT_FREE_BUF (fat32Bs_p);
  	return MMA_RET_WRITE_ERR;

  }
  MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster = nrSectorsPerCluster;

 	//***********Max number of root entries.
 	//Despite the fact that this is not an FAT32 item, we fake the number so existing
 	//(FAT16) code can unchanged. The first directory heap is always one cluster!
  MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries =
								(MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
								 MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector) / 32;

//***********Root Directory cluster number.
  MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster = MMA_F32_ROOT_CLUSTER_NMB;


 	//***********Determine number of sectors in one FAT table.
/*
  Calculation:
    T = Totoal Number of sectors - Reserved Sectors
    MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT = T/((128*nrSectorsPerCluster)+2)
    Round up the value   MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT

    NOTE: 128 is maximum number of FAT entries per sector

    512 = Sector size
    4 =FAT etnry size
    128=512/4;
*/

	u32Var1 = nrSectorsVol - MMA_F32_FORMAT_NMB_RES_SECT;
	u32Var2 = (128 * nrSectorsPerCluster) + MMA_F32_FORMAT_NMB_FAT_TABLES;
	u32Var1 = u32Var1 / u32Var2;
	if((u32Var1%u32Var2) != 0)
		{
		        /* round up the value */
				u32Var1++;
		}
	MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT = u32Var1;


	//***********
  //***********Ready computing. Write values to reserved buffer for boot sector.
	//***********

	//***********Jump Code
	memcpy((void*)&(fat32Bs_p[0]),(const void*)rfsfat_env_ctrl_blk_p->ffs_globals.aucJmpCode,3);

 //***********Filesystem type
    memcpy((void*)&(fat32Bs_p[3]),(const void*)"MSDOS5.0", BS_FSTYPE_FIELD_LENGTH);


	//***********Bytes per Sector
	memcpy(((void*)(&(fat32Bs_p[MMA_F32_OFFS_BYTES_PER_SEC]))),
	       ((const void*)(&(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector))), sizeof(UINT16));

	//***********Sectors per Cluster (one byte)
	fat32Bs_p[MMA_F32_OFFS_SEC_PER_CLUS] = MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster;

	//***********Number of Reserved Sectors
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_NMB_RESERVED_SEC]),
	       (const void*)&MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors, sizeof(UINT16));

	//***********Sectors per Cluster (one byte)
	fat32Bs_p[MMA_F32_OFFS_SEC_PER_CLUS] = MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster;

	//***********Number of FATS (one byte)
	fat32Bs_p[MMA_F32_OFFS_NMB_FAT] = MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables;

	//***********Media Descriptor (one byte)
	fat32Bs_p[MMA_F32_OFFS_MEDIA_DESCR] = rfsfat_env_ctrl_blk_p->ffs_globals.ucMediaDescr;

	//***********Total number of sectors on partition
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_NMB_SEC_VOL32]),
	       (const void*)&MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol,
         sizeof(UINT32));

	//***********Total number of sectors in one FAT table
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_NMB_SEC_FAT32]),
	       (const void*)&MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT,
         sizeof(UINT32));

	//***********Cluster number of ROOT dir
	u32Var1 = MMA_F32_ROOT_CLUSTER_NMB;
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_ROOT_DIR_CLUS]),
	       (const void*)&u32Var1,
         sizeof(UINT32));

	//***********Sector number in Reserved area of FS info struct
	u16Var1 = 1;
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_FSINFO_SECTOR]),
	       (const void*)&u16Var1,
         sizeof(UINT16));

 //***********Extended Boot Signature
  u8Index = 0x29;
  memcpy((void*)&(fat32Bs_p[66]),
         (const void*)&u8Index,
         sizeof(u8Index));


	//***********Partition Name
	strncpy((void*)&(fat32Bs_p[MMA_F32_OFFS_PARTITION_NAME]),
	       (const void*)MMA_BOOT_SECTOR[mpt_idx].partname,
            MAX_PART_NAME);

  //***********Filesystem type
  memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_FILESYSTEM_TYPE]),
         (const void*)"FAT32   ", BS_FSTYPE_FIELD_LENGTH);

	//***********EXECUTABLE MARKER
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_EXECUTABLE_MARKER]),
	       (const void*)rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker,
         sizeof (rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker));
   /* calculate the Number of sides(heads) sectors per Track */
   bCalcSides( &u16Var1,&u16Var2, MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol);

   memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_NMB_SEC_PER_TRK]), (const void*)&u16Var1,sizeof(UINT16));
   memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_NMB_HEADS]), (const void*)&u16Var2,sizeof(UINT16));

	//***********
  //BS buffer prepared. Write it to the media
	//***********
	stReadWrite.tOffset = 0; //BS starts at zero
  stReadWrite.pucBuffer = (UINT8*)fat32Bs_p;
  stReadWrite.tNrBytes = MMA_F32_RESERVED_SECTOR_SIZE;
  if (MMA_ucDirectWrite (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
  {
	  RFSFAT_FREE_BUF (fat32Bs_p);
  	return MMA_RET_WRITE_ERR;
  }

	//***********
	//Now the FS info sector
	//***********
	memset((void*)fat32Bs_p, 0x00,MMA_F32_RESERVED_SECTOR_SIZE);//clear sector

	//***********FSINFO LEAD SIGNATURE
	u32Var1 = MMA_F32_FSINFO_LEAD_SIGN;
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_FSINFO_LEAD_SIGN]),
	       (const void*)&u32Var1,
         sizeof(UINT32));

	//***********FSINFO STRUC SIGNATURE
	u32Var1 = MMA_F32_FSINFO_STRUC_SIGN;
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_FSINFO_STRUC_SIGN]),
	       (const void*)&u32Var1,
         sizeof(UINT32));

	//***********FSINFO FREE COUNT
	u32Var1 = MMA_F32_FSINFO_NOT_SUPPORTED;
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_FSINFO_FREE_COUNT]),
	       (const void*)&u32Var1,
         sizeof(UINT32));

	//***********FSINFO FREE CLUSTER-- HINT
	u32Var1 = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_FSINFO_NEXT_FREE]),
	       (const void*)&u32Var1,
         sizeof(UINT32));

	//***********EXECUTABLE MARKER
	memcpy((void*)&(fat32Bs_p[MMA_F32_OFFS_EXECUTABLE_MARKER]),
	       (const void*)rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker,
         sizeof (rfsfat_env_ctrl_blk_p->ffs_globals.aucExecMarker));

	//***********
  //FS Info sector buffer prepared. Write it to the media
	//***********
	stReadWrite.tOffset = 1*MMA_F32_RESERVED_SECTOR_SIZE; //FS info starts point
  stReadWrite.pucBuffer = (UINT8*)fat32Bs_p;
  stReadWrite.tNrBytes = MMA_F32_RESERVED_SECTOR_SIZE;
  if (MMA_ucDirectWrite (mpt_idx, &stReadWrite) != FFS_NO_ERRORS)
  {
	  RFSFAT_FREE_BUF (fat32Bs_p);
  	return MMA_RET_WRITE_ERR;
  }

	RFSFAT_FREE_BUF (fat32Bs_p);
#ifdef ICT_SPEEDUP_RFSFAT4

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

  /* Fill this maximum data cluster to Boot sector information */
  MMA_BOOT_SECTOR[mpt_idx].ulMaxDataClusters = ulMaxDataClusters;

  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("RFSFAT:Maximum Number of Data cluster are ",ulMaxDataClusters);

#endif
  /* flush the write cache  & Kill the Readh cache also */
	flush_write_blk_kill_cache();



 	return FFS_NO_ERRORS; //All ok
}

/**MH**Copyright by ICT Embedded BV All rights reserved **********************/


UINT8
MMA_ucCreateBootSector (const UINT8 mpt_idx, const UINT8 * name)
/* PURPOSE      : Function creates a new bootsector on volume (used by format)
 * ARGUMENTS    : mpt_idx mountpoint index
 * RETURNS      : FFS_NO_ERRORS - Bootsector created on volume ucVolume.
 *                MMA_RET_WRITE_ERR - Bootsector not created.
 * PRECONDITION : Media manager must be initialised.
 * POSTCONDITION: Bootsector is created on volume.
 * ERROR AND EXCEPTION HANDLING: N/A
 * NOTE: -
 *****************************************************************************/
{
  UINT16 uiIndex;
  UINT8 ucBuffer = 0;
  MMA_RW_STRUCT stWrite={0,0,NULL};


#ifdef ICT_SPEEDUP_RFSFAT4

UINT32 ulMaxDataClusters;

#endif
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
	{
    RFSFAT_SEND_TRACE ("RFSFAT: MMA_ucCreateBootSector FAT32 entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);
	  return ucCreateBootSectorFat32(mpt_idx, name);
	}

  RFSFAT_SEND_TRACE ("RFSFAT: MMA_ucCreateBootSector FAT16 entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);
  stWrite.tOffset = 0;
  stWrite.pucBuffer = (UINT8 *) & ucBuffer;
  stWrite.tNrBytes = sizeof (UINT16);

  /****************************************************************************
   * Clear boot-sector area byte after byte                                   *
   ****************************************************************************/
  for (uiIndex = 0; uiIndex <= (MPT_TABLE[mpt_idx].blk_size * NEW_BS_RES_SECT);
	   uiIndex++)
	{
	  if (MMA_ucDirectWrite (mpt_idx, &stWrite) != FFS_NO_ERRORS)
		{
		  return MMA_RET_WRITE_ERR;
		}
	  stWrite.tOffset = uiIndex;
	}
  /****************************************************************************
  * Fill boot-sector structure members with pre-defined values                *
  ****************************************************************************/
  MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors = NEW_BS_RES_SECT;
  MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables = NEW_BS_NR_FAT;
  MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries = NEW_BS_MAX_ROOT;
  strncpy ((char *) MMA_BOOT_SECTOR[mpt_idx].partname, (const char *) name,MAX_PART_NAME);
  /****************************************************************************
  * Calculate the bootsector values for the volume size given in the master   *
  * boot-record.                                                              *
  * MMA_stBootSector.ucSectorPerCluster                                       *
  * MMA_stBootSector.uiBytesPerSector                                         *
  * MMA_stBootSector.uiNrSectorsFAT                                           *
  * MMA_stBootSector.ulNrSectorsVol .                                         *
  ****************************************************************************/

  if (bCalcBootsector (mpt_idx) == FALSE)
	{
	  return MMA_RET_INCOR_BOOTRECORD;
	}

  /* Write to the media                                                       */
  if (bReadWriteBS (mpt_idx, WRITE_MODE) != FALSE)
	{
	  return MMA_RET_WRITE_ERR;
	}

#ifdef ICT_SPEEDUP_RFSFAT4

  /*Determine maximum number of sectors in data area                          */
  /*Total number of sectors - (reserved region + total FAT size + Root size)  */

  ulMaxDataClusters = MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol;
  ulMaxDataClusters -= MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors;
  ulMaxDataClusters -= (MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables *
            MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT);

if (MMA_FS_TYPE(mpt_idx) != GBI_FAT32_LBA)
{
   /* In case FAT16 and FAT12 */

  ulMaxDataClusters -=  ((MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries * ROOT_LABEL_SIZE) /
						  MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
}

  /* Determine the number of clusters in data area                            */
  ulMaxDataClusters =  (ulMaxDataClusters / MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster);

 /* Since the cluster number start from 2, and the mximum cluster number would be n+1 */
 /* since this function should indicate the maximum cluster number, its required to less than one */
  ulMaxDataClusters+=1;

  /* Fill this maximum data cluster to Boot sector information */
  MMA_BOOT_SECTOR[mpt_idx].ulMaxDataClusters = ulMaxDataClusters;

  RFSFAT_SEND_TRACE_PARAM_DEBUG_LOW("RFSFAT:Maximum Number of Data cluster are ",ulMaxDataClusters);

#endif
  return (FFS_NO_ERRORS);
}


/**MH**Copyright by ICT Embedded BV All rights reserved **********************/
static BOOL
bCalcBootsector (UINT8 mpt_idx)
/* PURPOSE      : Calculate Bootsector Bytes per sector and number sectors in
 *                cluster values.
 * ARGUMENTS    : ucVolume: The volume to calculate the boot-sector values.
 * RETURNS      : TRUE  : Values are calculated successful.
 *                FALSE : Volume size is incorrect.
 * PRECONDITION : -
 * POSTCONDITION: -
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE: -
 *****************************************************************************/
{
  UINT32 ulSize;

  RFSFAT_SEND_TRACE ("RFSFAT: bCalcBootsector entered",
					 RV_TRACE_LEVEL_DEBUG_LOW);

  if (MMA_BOOT_SECTOR[mpt_idx].PartStatus == FFS_RV_UNAVAILABLE)
	{
	  return FALSE;
	}

  MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector = MPT_TABLE[mpt_idx].blk_size;

  MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol = MPT_TABLE[mpt_idx].partition_size;


 ulSize = MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol;


  /* Determine the number sectors in a cluster                                */
 if (MMA_FS_TYPE(mpt_idx) == GBI_FAT12)
  	{
  	  if(ulSize >= UINT12_MAX) /* Maximum number of clusters/sectors */
	 	{
	 	  	 MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster = NEW_BS_SECT_CLU;
	 	}
	  else
	  	{
			 MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster = NEW_BS_SECT_CLU_12;
	  	}

  	 ulSize = ulSize / UINT12_MAX;

     MMA_BOOT_SECTOR[mpt_idx].fatEntrySize = sizeof(UINT8);   /* its suppose to be 1.5 ,
  																but 1 is better*/

 	}
 else
 	{
  		ulSize = ulSize / UINT_MAX;
  		 MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster = NEW_BS_SECT_CLU;
        MMA_BOOT_SECTOR[mpt_idx].fatEntrySize = sizeof(UINT16);
 	}
  if (ulSize > 1)
	{
	  if (ulSize > MAX_SECT_CLU)
		{
		  return FALSE;
		}

	  while (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster < ulSize)
		{

		  MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *= NEW_BS_SECT_CLU;
		}
	}
  /****************************************************************************/
  /* Calculate the number of sectors needed for a FAT table                   */
  /****************************************************************************/

  /* Determine the number of clusters in the partition                        */
  /* The number of clusters in the partition is a little bit more than the    */
  /* number of FAT entries                                                    */
  ulSize =	(MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol-NEW_BS_RES_SECT) /
	MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster;

  /* Determine the number of bytes in the FAT table                           */
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT12)
  	{
  		/* In case of FAT12 file system , the FAT Entry size is 12 bits (1 1/2 bytes) */
  	     ulSize = ((ulSize * 3)/2);
  	}
  else
  	{
     ulSize = (ulSize * NR_BYTES_FAT_ENTRY);
  	}

  MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT =
	(UINT16) (ulSize / MPT_TABLE[mpt_idx].blk_size);
  ulSize = ulSize % MPT_TABLE[mpt_idx].blk_size;
  if (ulSize != 0)
	{
	  MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT++;
	}

  /* For FAT12 , Determine the Maxroot Enries */
  /* If the media is less than FAT12_LEAST_BLKS , then configure it like a typical floppy */
 if ((MMA_FS_TYPE(mpt_idx) == GBI_FAT12) && (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster == NEW_BS_SECT_CLU_12) && (MMA_BOOT_SECTOR[mpt_idx].ulNrSectorsVol < FAT12_LEAST_BLKS))
  	{
  	  MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries = NEW_BS_MAX_ROOT_12;
 	}

  return TRUE;
}
#define MAX_SPT         63
#define MAX_SIDES       255
#define MAX_TRACKS      1024
#define MAX_SECS		8192

static BOOL bCalcSides(UINT16 * ucSpt, UINT16 * ucSides, UINT32 ucTotalSector)
{
	UINT32 ulCylinders;

    if(ucTotalSector >= MAX_SECS )   /* optimization for larger memories */
    	{
    	   *ucSides = MAX_SIDES;
		    *ucSpt   = MAX_SPT;
			return TRUE;
    	}


	/*  Determine our default values
    */
    *ucSides = 2;
    *ucSpt   = 16;
    ulCylinders = ucTotalSector/ (*ucSides) / (*ucSpt);

    if(!ulCylinders)
        return FALSE;

    /*  Get the CHS values down to reasonable values

        The conditions below prevent a system hang-up
        when have a large number of sectors, e.g. 4MB.
    */
    while((ulCylinders > MAX_TRACKS)
          && !(((*ucSides) >= MAX_SIDES) && ((*ucSpt) >= MAX_SPT)))
    {
        /*  If we can, up the number of sides!
        */
        if((*ucSides) < MAX_SIDES)
        {
            (*ucSides) += 2;
            ulCylinders = ucTotalSector/ (*ucSides) / (*ucSpt);

            /*  Quit if the number of cylinders is to a good point
            */
            if(ulCylinders < MAX_TRACKS)
                break;
        }

        /*  If we can, up the number of sectors per track!
        */
        if((*ucSpt) < MAX_SPT)
        {
            (*ucSpt) += 16;

            /*  Changed MAX_SPT from 64 to 63 for > 190MB disks
            */
            if((*ucSpt) > MAX_SPT)
                (*ucSpt) = MAX_SPT;

            ulCylinders = ucTotalSector/ (*ucSides) / (*ucSpt);

            /*  Quit if the number of cylinders is to a good point
            */
            if(ulCylinders < MAX_TRACKS)
                break;
        }
    }
	return TRUE;
}


#undef MAX_SECS
#undef MAX_TRACKS
#undef MAX_SIDES
#undef MAX_SPT



UINT8 MMA_getfstype(const UINT8 mpt_idx, UINT16* fs_type)
{
 UINT8 buffer[MMA_F32_BOOT_SECTOR_INFO_SIZE];
 MMA_RW_STRUCT stReadWrite={0,0,NULL};
 UINT8  status = FFS_NO_ERRORS;

  RFSFAT_SEND_TRACE ("RFSFAT: MMA_getfstype, entered",  RV_TRACE_LEVEL_DEBUG_LOW);

 //Start reading the part of the BS that is of interest (from media)
  stReadWrite.tOffset = 0; //BS starts at zero
  stReadWrite.pucBuffer = (UINT8*)buffer;
  stReadWrite.tNrBytes = MMA_F32_BOOT_SECTOR_INFO_SIZE;
  if ((status = MMA_ucDirectRead (mpt_idx, &stReadWrite)) != FFS_NO_ERRORS)
  {
   RFSFAT_SEND_TRACE ("RFSFAT: MMA_getfstype, MMA_ucDirectRead Failed",  RV_TRACE_LEVEL_ERROR);
	return status;
  }

  if (bcheckFormatsigature (mpt_idx) != TRUE)
   {
		 *fs_type = GBI_UNKNOWN;
    }
  else
  	{
      *fs_type= gbi_i_determine_fat_type(buffer);
  	}


  return status;

}



UINT8 MMA_ucMount(void)
{
  if(rfsfat_env_ctrl_blk_p->state == RFSFAT_CONFIGURED)
  	{
  	     /* FAT is already configured No need to mount , return OK */
		 return FFS_NO_ERRORS;
  	}

  /* Update the Boot sectors of all the media */
  if (MMA_ucInit () != FFS_NO_ERRORS)
     {
       RFSFAT_SEND_TRACE_ERROR("RFSFAT: exit rfsfat_getpartion info -> media manager initialisation failed");
		  return FFS_RV_IS_NOT_A_PART; /* Its mounting error */
 	}

  rfsfat_env_ctrl_blk_p->state = RFSFAT_CONFIGURED;
  return FFS_NO_ERRORS;

}

UINT8 MMA_ucUnMount(void)
{
 UINT8 ucCntr, mpt_idx;

 if(rfsfat_env_ctrl_blk_p->state == RFSFAT_STOPPED)
  	{
  	     /* FAT is already stoped No need to mount , return OK */
		 return FFS_NO_ERRORS;
  	}

#ifdef ICT_SPEEDUP_RFSFAT2

  /* flush caches of all mount points */
  for(mpt_idx=0;mpt_idx< NR_OF_MOUNTPOINTS;mpt_idx++)
  {

     ucCntr = ucFlushFamCash(mpt_idx);

    if(ucCntr != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: MMA_ucUnMount, ucFlushFamCash failed ");
      return ucCntr;
     }

     /* Reset the cache */
      Reset_FAT_TABLE_cache(mpt_idx);
 
   }

 #endif 


 flush_write_blk_kill_cache();

 kill_gbi_readcash();




  /* clear all fds */
  for (ucCntr = 0; ucCntr < FSCORE_MAX_NR_OPEN_FSCORE_FILES; ucCntr++)
	{
      rfsfat_env_ctrl_blk_p->ffs_globals.ffs_astOpenFileList[ucCntr].ucAccessMode = FFS_OPEN_FILE_ENTRY_NOT_USED;
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[ucCntr].fd = RFS_FD_DEF_VALUE;

	}

 /* Reset the optimization cluster setting */
  rfsfat_env_ctrl_blk_p->ffs_globals.clusterBaseNmb = 0xFFFFFFFE;

  rfsfat_env_ctrl_blk_p->unmount_count++;   /* Increment the Unmount counter */
  rfsfat_env_ctrl_blk_p->state = RFSFAT_STOPPED;

  return FFS_NO_ERRORS;

}






static T_GBI_FS_TYPE calc_fat_fstype(const UINT8 mpt_idx)
{
  UINT32 blocks_512;

  blocks_512 = (MPT_TABLE[mpt_idx].partition_size)*((MPT_TABLE[mpt_idx].blk_size)/MMA_DEFAULT_SECTOR_SIZE);

  if(blocks_512 >= MMA_MAX_FAT16_BLOCKS)  return GBI_FAT32_LBA;

  if(blocks_512 >= MMA_MAX_FAT12_BLOCKS)  return GBI_FAT16_LBA;

  return GBI_FAT12;  /* default file system */

}

T_GBI_FS_TYPE MMA_verify_fat_type_size(const UINT8 mpt_idx, T_GBI_FS_TYPE fs_type)
{
  UINT32 blocks_512;

  blocks_512 = (MPT_TABLE[mpt_idx].partition_size)*((MPT_TABLE[mpt_idx].blk_size)/MMA_DEFAULT_SECTOR_SIZE);

  /* for raw t-flash dd*/
  if((MMA_FS_TYPE(mpt_idx) == GBI_UNKNOWN)
    && ((MPT_TABLE[mpt_idx].media_type != GBI_SD)
    || (MPT_TABLE[mpt_idx].media_type != GBI_MMC)))
  {
     if(blocks_512 >= MMA_MIN_CHECKS_FAT32_BLOCKS)
     {
         return GBI_FAT32_LBA;
     }
     else
     {
         return GBI_UNKNOWN;
     }
  }

  switch (fs_type)
  	{
  	    case GBI_FAT12:
             if(blocks_512 > MMA_MAX_CHECKS_FAT12_BLOCKS)
             	{
             	     return GBI_UNKNOWN;
             	}
			 return GBI_FAT12;


		case GBI_FAT16_A:
		case GBI_FAT16_B:
		case GBI_FAT16_LBA:
              if(blocks_512 < MMA_MIN_CHECKS_FAT16_BLOCKS)
              	{
              	     return GBI_UNKNOWN;
              	}
			 return GBI_FAT16_LBA;

		case GBI_FAT32:
		case GBI_FAT32_LBA:
			if(blocks_512 < MMA_MIN_CHECKS_FAT32_BLOCKS)
              	{
              	     return GBI_UNKNOWN;
              	}
			 return GBI_FAT32_LBA;

		default : return GBI_UNKNOWN;

  	}

}
