/**
 * @file	rfsfat_mma.h
 *
 * Header file for MMA.c
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



#ifndef MMA_H
#define MMA_H

/* Before this file is included, the file ffs.h must be included to be sure   */
/* the different feature compiler directives are defined.                     */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

#define MMA_PART_T_SMALL          0x04	/* Partition < 32 Mbyte               */
#define MMA_PART_T_BIG            0x06	/* Partition > 32 Mbyte               */

#define MMA_READ_MODE             0
#define MMA_WRITE_MODE            1

#define INVALID_PART      0x27	/* This value is used for getting the       */
								  /* bootsector, by default the function uses */
/******************************************************************************
*     Macros                                                                  *
******************************************************************************/
#define MMA_BOOT_SECTOR (rfsfat_env_ctrl_blk_p->ffs_globals.\
                                                  media_data.MMA_stBootSector)
#define MMA_AST_MEDIA_FUNCTIONS (rfsfat_env_ctrl_blk_p->ffs_globals.\
                                                  media_data.astMediaFunctions)
#define NR_OF_MOUNTPOINTS     (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.nbr_partitions)
#define MPT_TABLE (rfsfat_env_ctrl_blk_p->ffs_globals.mpt_tbl.partition_info_table)


#define NBR_MEDIA (rfsfat_env_ctrl_blk_p->nmb_of_media)

#define MEDIA_TABLE (rfsfat_env_ctrl_blk_p->media_info_p)


#define MMA_FS_TYPE(mp) (MPT_TABLE[mp].filesys_type)  
//first cluster number of root DIR
#define MMA_F32_ROOT_CLUSTER_NMB        (2)

/* Size of a directory label in the ROOT      */
#define ROOT_LABEL_SIZE 32   


/******************************************************************************
*     Typedefs                                                                *
******************************************************************************/
#define MAX_PART_NAME				11         /* ALERT: Dont change it */
typedef struct
{
  UINT32 uiNrSectorsFAT;		/*number of sectors per FAT                    */
  UINT32 ulNrSectorsVol;		/*number of sectors on the partition           */
  UINT32 ulFAT32RootDirCluster;   /*cl.number of the fat32 root dir*/
  UINT16 uiBytesPerSector;		/*number of bytes per sector                   */
  UINT16 uiReservedSectors;		/*number of reserved sectors                   */
  UINT16 uiMaxRootEntries;		/*maximum number of labels in the Root         */
  UINT16 fsInfoSector;			/*sector number of filesystem info structure */
  UINT8 ucSectorPerCluster;		/*number of sectors per cluster                */
  UINT8 ucNrFatTables;			/*number of FAT tables and FAT copies          */
  UINT8 BootSectIdx;			/*stores last partition whose BS is read       */
  UINT8 fatEntrySize;			/*number of bytes a FAT entry occupies         */
  UINT8 PartStatus;			/*status of partition                          */
  UINT8 reserv_1;					/* reserved, padding */
  UINT8 reserv_2;					/* reserved, padding */
  UINT8 reserv_3;					/* reserved, padding */
  UINT8 partname[MAX_PART_NAME];	/*partition name */
  UINT8 reserv_4;					/* reserved, padding */
  UINT32 ulMaxDataClusters;
 } MMA_BOOT_SECTOR_INFO_STRUCT;

typedef struct
{
  FFS_FILE_LENGTH_TYPE tOffset;	/* Offset in the partition                     */
  FFS_FILE_LENGTH_TYPE tNrBytes;	/* Number of bytes to read or write        */
  UINT8 * pucBuffer;				/* Buffer with data                        */
} MMA_RW_STRUCT;

typedef struct
{
  UINT8 (*MMA_bFuncRead) (MMA_RW_STRUCT * pstRead, UINT8 mpt_idx);	//Read Function
  UINT8 (*MMA_bFuncWrite) (MMA_RW_STRUCT * pstWrite, UINT8 mpt_idx);	//Write Function
} MMA_MEDIA_STRUCT;


/******************************************************************************
*     Variable Declaration                                                    *
******************************************************************************/

/******************************************************************************
*     Function prototypes                                                     *
******************************************************************************/
UINT8 MMA_ucInit (void);
void MMA_ucInit_mpt(UINT8 mpt_idx);
UINT8 MMA_ucCreateBootSector (const UINT8 mpt_idx, const UINT8 * name);
UINT8 MMA_ucGetBootSectorInfo (const UINT8 mpt_idx);
UINT8 MMA_ucSetBootSectorLabel (const UINT8 mpt_idx, const UINT8 * name);
UINT8 MMA_ucGetPartitionStatus (const UINT8 mpt_idx);
UINT8 MMA_ucDirectWrite (const UINT8 mpt_idx,
						 RO_PTR_TO_RW (MMA_RW_STRUCT) pstWrite);
UINT8 MMA_ucDirectRead (const UINT8 mpt_idx,
						RO_PTR_TO_RW (MMA_RW_STRUCT) pstRead);
UINT8 MMA_getfstype(const UINT8 mpt_idx, UINT16* fs_type);
UINT8 MMA_ucMount(void);
UINT8 MMA_ucUnMount(void);
T_GBI_FS_TYPE MMA_verify_fat_type_size(const UINT8 mpt_idx, T_GBI_FS_TYPE fs_type);
UINT8  E_FFS_FATmetabytes(UINT8 mpt_idx ,UINT32 *bytes_p);

#endif
