/**
 * @file	FAM.h
 *
 * Header file of FAM.c
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


#ifndef FAM_H
#define FAM_H

/* Before this file is included, the file ffs.h must be included to be sure   */
/* the different feature compiler directives are defined.                     */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

/******************************************************************************
*     Defines and enums                                                       *
******************************************************************************/

/******************************************************************************
* FAT label values of a FAT table entry
*******************************************************************************/
#define FAM_FREE_CLUSTER        0x0000	/* Free cluster                       */

/* Min.Value reserved cluster         */
#define FAM_RESERVED_CL_MIN(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFF0: \
					 ((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFF0:(UINT32)0xFFF0))

/* Max.Value reserved cluster         */
#define FAM_RESERVED_CL_MAX(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFF6: \
					 ((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFF6:(UINT32)0xFFF6))

/* BAD cluster                        */
#define FAM_BAD_CLUSTER(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFF7: \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFF7:(UINT32)0xFFF7))

/* Min.value of a last cluster label  */
#define FAM_LAST_CLUSTER_MIN(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFF8: \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFF8:(UINT32)0xFFF8))

/* Max.value of a last cluster label  */
#define FAM_LAST_CLUSTER_MAX(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFFF: \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFFF:(UINT32)0xFFFF))

/* Used value for a Last Cluster      */
#define FAM_LAST_CLUSTER(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFFF: \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFFF:(UINT32)0xFFFF))

/* FAT label of FAT table entry 0 =  Media value  */
#define FAM_ROOT_CLUSTER_0(mpt_idx) \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? (UINT32)0x0FFFFFF8: \
					((MMA_FS_TYPE(mpt_idx) == GBI_FAT12)?(UINT32)0xFF8:(UINT32)0xFFF8))


#if 0
/* End cluster of ROOT                */
#define FAM_END_ROOT_CLU(mpt_idx) \
          ((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? 2: 1)

/* Start of the data clusters         */
#define FAM_START_DATA_CLU(mpt_idx) \
          ((MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA) ? 3: 2)

#endif


#define FAM_NMB_OF_RESERVED_CLUSTER_0    (0)
#define FAM_NMB_OF_RESERVED_CLUSTER_1    (1)
#define FAM_START_DATA_CLU               (2) /* Start of the data clusters         */
#define META_DATA_SIZE  32		/* Size of a meta-data label in the ROOT      */


/******************************************************************************
*     Macros                                                                  *
******************************************************************************/

/******************************************************************************
*     Typedefs                                                                *
******************************************************************************/

/******************************************************************************
*     Variable Declaration                                                    *
******************************************************************************/

/******************************************************************************
*     Function prototypes                                                     *
******************************************************************************/

UINT8 FAM_ucGetNextCluster (const UINT8 mpt_idx,
							RO_PTR_TO_RW (UINT32) puiClusterNr);
UINT8 FAM_ucCreateNewChain (const UINT8 mpt_idx,
							RO_PTR_TO_RW (UINT32) puiClusterNr);
UINT8 FAM_ucAppendCluster (const UINT8 mpt_idx,
						   RO_PTR_TO_RW (UINT32) puiClusterNr, BOOL          clearNewDirCluster);
UINT8 FAM_ucDelLinkedList (const UINT8 mpt_idx, const UINT32 uiClusterNr);

UINT8 FAM_ucRestoreFAT (const UINT8 mpt_idx, const UINT8 ucFATCopy);
UINT8 FAM_ucNrFreeBytes (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulNrOfBytes);

UINT8 FAM_ucNrFreeclusters (const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulNrOfClusters);

UINT8 FAM_ucNrDataclusters(const UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulNrOfClusters);


UINT8 FAM_ucMarkBadCluster (const UINT8 mpt_idx,
							const UINT32 uiClusterNumber);

UINT8 FAM_ucAssureClusterLength (const UINT8 mpt_idx,
								 const UINT32 uiFirstClusterNumber,
								 const FFS_FILE_LENGTH_TYPE tNewFileLength);
BOOL FAM_bCreateFatTables (const UINT8 ucVolume);

UINT8 FAM_ucTruncLinkedList (const UINT8 mpt_idx, const UINT32 uiClusterNr,UINT32 NewSize);

#ifdef ICT_SPEEDUP_RFSFAT2
UINT8 ucFlushFamCash(UINT8 mpt_idx);
void Reset_FAT_TABLE_cache(UINT8 mpt_idx);
void FAM_ucFreeFATcash(void);
void FAM_ucInitFATcash(void);
#endif //ICT_SPEEDUP_RFSFAT2

#endif

