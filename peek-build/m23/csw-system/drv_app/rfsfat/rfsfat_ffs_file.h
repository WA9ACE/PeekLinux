/**
 * @file	ffs_file.h
 *
 * In this file, the prototypes of functions for changing
 * and reading file data can be found here.
 * Definitions, variables and function prototypes that are only
 * used within the interpreter block (ffs_) are stated here.
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


#ifndef FFS_FILE_H
#define FFS_FILE_H

/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/
#define FFS_OPEN_FILE_ENTRY_NOT_USED                                        0xFF

/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/
#define FFS_ASCII_VALUE_A                                                    'A'
#define FFS_ASCII_VALUE_B                                                    'B'
#define FFS_ASCII_VALUE_Z                                                    'Z'
#define FFS_ASCII_VALUE_COLON                                                ':'
#define FFS_ASCII_VALUE_DOT                                                  '.'
#define FFS_ASCII_VALUE_SPACE                                                ' '
#define FFS_ASCII_VALUE_BACKSLASH                                           '\\'
#define FFS_ASCII_VALUE_FWSLASH                                             '/'
#define FFS_ASCII_VALUE_ASTERIX                                              '*'
#define FFS_ASCII_EOL                                                       '\0'

/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/
/**
 * @name OPEN_FILE_ENTRY_STRUCT
 *
 * This type definition contains the file entry information
 */
/*@{*/
typedef struct
{
  DIR_LABEL_LOCATION_STRUCT stLabLoc;	/* Location of directory label of file */
  UINT32 uiFirstClusterNumber;	/* Loc. of the file's data. */
  FFS_FILE_LENGTH_TYPE tFilePointer;
  FFS_FILE_LENGTH_TYPE tFileSize;

  /* File pointers optimizations */
  UINT32 uiClusterNumber_L;	
  FFS_FILE_LENGTH_TYPE tFilePointer_L;


  UINT32 uiClusterNumber_H;	
  FFS_FILE_LENGTH_TYPE tFilePointer_H;


  UINT8 ucAccessMode;
  UINT8 reserv1;                      /*Next 3 bytes */
  UINT8 reserv2;
  UINT8 reserv3;

  
  
} OPEN_FILE_ENTRY_STRUCT;
/*@}*
 ******************************************************************************
 *     Variable Declaration                                                   *
 ******************************************************************************/

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
UINT8 ffs_ucFileOrDirCreate
  (BOOL bCreateFile,
   RO_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RO (UINT8) pucNameAndExt, BOOL flag_create);

UINT8 ffs_ucGetTimeAndDate (UINT16 * puiTime, UINT16 * puiDate);

INT8 OS_ucGetTime (OS_TIME_STRUCT * pstTime);



#if (FFS_UNICODE == 1)
/**************************************************/
// Unicode FFS       
/**************************************************/
UINT8 ffs_ucFileOrDirCreate_uc(BOOL                   bCreateFile,
                               UINT8                  mp_idx,
                               const  T_WCHAR  		 *pathName_p,
                               const  T_WCHAR        *fileName_p,
                               BOOL                   flag_create);

UINT8 E_FFS_ucFmanTrunc (DIR_LABEL_LOCATION_STRUCT stLabLoc,T_FSCORE_SIZE Newsize );

                               
/**************************************************/
// end of unicode FFS       
/**************************************************/
#endif


#endif
