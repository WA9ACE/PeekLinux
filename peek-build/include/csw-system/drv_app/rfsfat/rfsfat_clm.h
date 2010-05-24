/**
 * @file	rfsfat_clm.h
 *
 * Header file of CLM.c
 *
 * @author	Anton van Breemen
 * @version 0.1 This file inherited from a ICT re-use project
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	01/28/2004	Anton van Breemen		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 *	04/29/2004	E. Oude Middendorp		FAT32 added.
 */

#ifndef CLM_H
#define CLM_H

/******************************************************************************
*     Defines and enums                                                       *
*******************************************************************************/
/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

#define CLM_READ_MODE             0	/* Mode for reading the boot-sector     */
#define CLM_WRITE_MODE            1	/* Mode for writing the boot-sector     */


/******************************************************************************
*     Macros                                                                  *
*******************************************************************************/

#define CLM_ucRead(pstRead)   CLM_ucReadWrite (pstRead, CLM_READ_MODE)
#define CLM_ucWrite(pstWrite) CLM_ucReadWrite (pstWrite, CLM_WRITE_MODE)

/******************************************************************************
*     Typedefs                                                                *
*******************************************************************************/

typedef struct
{
  UINT8 mpt_idx;				/* Unique mountpoint               */
  UINT8 pad_1;					/* padding to next 4 bytes alignment */
  UINT8 pad_2;
  UINT8	pad_3;
  UINT32 uiFirstClusterNr;		/* First cluster number of file    */
  FFS_FILE_LENGTH_TYPE tFileLength;	/* (Current) total file length     */
  FFS_FILE_LENGTH_TYPE tOffset;	/* Offset in file                  */
  FFS_FILE_LENGTH_TYPE tNrBytes;	/* Number of bytes to read/write.  */
  UINT8 *pucBuffer;				/* Buffer with data.               */
} CLM_READ_WRITE_STRUCT;


/******************************************************************************
*     Variable Declaration                                                    *
******************************************************************************/

/******************************************************************************
*     Function prototypes                                                     *
******************************************************************************/

UINT8 CLM_ucReadWrite (RO_PTR_TO_RO (CLM_READ_WRITE_STRUCT) pstReadWrite,
					   const UINT8 ucMode);

/* Cluser Read/Write optimization */
extern UINT8 CLM_ucReadWriteSpeed1(
      RO_PTR_TO_RW (CLM_READ_WRITE_STRUCT)  pstReadWrite,
      const UINT8                           ucMode);


BOOL bClusterOffsetInVol (const UINT8 mpt_idx,
								 const UINT32 uiClusterNr,
								 RO_PTR_TO_RW (FFS_FILE_LENGTH_TYPE)
								 ptOffset);
#endif

