/**
 * @file  rfsfat_ffs.h
 *
 * This file contains all API function definitions of the 
 * file system component
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
 *	04/29/2004	E. Oude Middendorp		FAT32 added.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


#ifndef FFS_FAT_H
#define FFS_FAT_H

#include "rv/rv_general.h"
#include "rfs/fscore_types.h"


/* Default partition after system startup.                                    */
/* Value should be an valid Ascii value!                                      */
#define FFS_DEFAULT_PART_ID                                               'A'

/* File open methods.                                                         */
#define FFS_OPEN_MODE_READ                                                    0
#define FFS_OPEN_MODE_READWRITE                                               1


/* Attribute masks */
#define FFS_ATTRIB_READWRITE_MASK                                          0x00
#define FFS_ATTRIB_READONLY_MASK                                           0x01
#define FFS_ATTRIB_HIDDEN_MASK                                             0x02
#define FFS_ATTRIB_SYSTEM_MASK                                             0x04
#define FFS_ATTRIB_PART_MASK                                               0x08
#define FFS_ATTRIB_SUBDIR_MASK                                             0x10
#define FFS_ATTRIB_ARCHIVE_MASK                                            0x20

#define LFN_MASK   (FFS_ATTRIB_READONLY_MASK | \
                    FFS_ATTRIB_HIDDEN_MASK |   \
                    FFS_ATTRIB_SYSTEM_MASK |   \
                    FFS_ATTRIB_PART_MASK)

#define FFS_ATTRIB_NAME_SMALL_MASK              0x08
#define FFS_ATTRIB_EXT_SMALL_MASK              0x10



/*Macro's used for ffs - Pointer macros */
#define RW_PTR_TO_RO(DataType)          const DataType *				//i will not change the variable pointed to	
#define RO_PTR_TO_RO(DataType)          const DataType * const	//i will not change the variable pointed to nor the pointer itself
#define RO_PTR_TO_RW(DataType)                DataType * 		//i will not change the pointer to the variable
#define RO_PTR_TO_RW_XDATA(DataType)          DataType * const	//i will not change the pointer to the variable
//#define void      void


#define FFS_FILE_LENGTH_TYPE               UINT32
#define FFS_MAX_VALUE_FILE_LENGTH_TYPE     0xFFFFFFFF

#define DIR_SIZE_META_NAME         0x08	/* Size of meta data fields in        */
#define DIR_SIZE_META_EXT          0x03	/* directory label.                   */
#define DIR_SIZE_META_ATTR         0x01
#define DIR_SIZE_META_LCASE          0x01 

#define FSCORE_MAX_NR_OPEN_FSCORE_DIRS    1

/* Size of name(8) + '.'(1) + extension (3) + string terminator (1). */
#define FFS_MAX_SIZE_NAME_DOT_EXT                         (DIR_SIZE_META_NAME+DIR_SIZE_META_EXT+2)                   



/* Features of the file system that can enabled/disabled to save RAM/ROM.     */
/* In case you are not intend to use relative paths, and you are sure you     */
/* use absolute paths, remove the following line to save RAM and ROM.         */
#define FFS_FEATURE_DIRECTORIES

/**
 * @name FFS_SMALL_DIRECTORY_LABEL_STRUCT
 *
 * This type definition contains compact directory label information
 */
/*@{*/
typedef struct
{
  UINT8 aucNameExt[FFS_MAX_SIZE_NAME_DOT_EXT];
  UINT8 ucAttribute;
  UINT16 uiLastChangedTime;
  UINT16 uiLastChangedDate;
  UINT32 ulFileSize;
} FFS_SMALL_DIRECTORY_LABEL_STRUCT;
/*@}*/



/*   
  ALERT ALERT ALERT:  Dont change this structure, Dont change the order of this structure elements
*/
typedef struct
{
  UINT8 aucName[DIR_SIZE_META_NAME];				/* 8 bytes */
  UINT8 aucExtension[DIR_SIZE_META_EXT];			/* 3 bytes */
  UINT8 ucAttribute;								/* 1 byte  */
  UINT8 lcase; 										/* 1 byte  */
  UINT8 crtTimeTenth;								/* 1 byte  */
  UINT16 crtTime;									/* 2 bytes */
  UINT16 crtDate;									/* 2 bytes */
  UINT16 lstAccDate;								/* 2 bytes */
  UINT16 uiFirstClusterNumberHigh;					/* 2 bytes */
  UINT16 uiLastChangedTime;							/* 2 bytes */
  UINT16 uiLastChangedDate;							/* 2 bytes */
  UINT16 uiFirstClusterNumber;						/* 2 bytes */
  UINT32 ulFileSize;								/* 4 bytes */
} DIR_DIRECTORY_LABEL_STRUCT;

 


 
/**
 * @name OS_TIME_STRUCT
 *
 * This type definition contains time information
 */
/*@{*/
typedef struct
{
  UINT8 ucSec;					/* Seconds after the minute [0..59]. */
  UINT8 ucMin;					/* Minutes after the hour [0..59].   */
  UINT8 ucHour;					/* Hours since midnight [0..23].     */
  UINT8 ucMDay;					/* Day of the month [0..31].         */
  UINT8 ucMon;					/* Month since january [0..11].      */
  UINT8 ucYear;					/* Year since 1900.                  */
  UINT8 pad_2;					/* padding */
  UINT8 pad_3;					/* padding */
} OS_TIME_STRUCT;
/*@}*/

/******************************************************************************/
/* Settings
 *
 */

/******************************************************************************
 *     Variable Declaration                                                   *
 ******************************************************************************/



/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/

/******************************************************************************/
/* Function defined in ffs_disk.c                                             */
/******************************************************************************/
UINT8 E_FFS_ucDiskInit (void);
UINT8 E_FFS_ucDiskTerminate (void);

//these mpt_idx are not know by the RFS module so if these functions are going 
//to be used by the RFS module they have to be rewritten to use a mountpoint name..
UINT8 E_FFS_ucDiskGetPartStatus (UINT8 mpt_idx);
UINT8 E_FFS_ucDiskDataSpace (UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulSize);
UINT8 E_FFS_ucDiskFree (UINT8 mpt_idx, RO_PTR_TO_RW (UINT32) pulSize);

UINT8 E_FFS_ucDiskFormat (UINT8 mpt_idx, UINT8 * name, T_GBI_FS_TYPE filesys_type);

UINT8 E_FFS_ucDiskSetCurrentDrive (UINT8 mpt_idx);
UINT8 E_FFS_ucDiskGetCurrentDrive (void);
UINT8 E_FFS_ucDiskStat (RO_PTR_TO_RO (UINT8) pucPath,
						RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) label);

UINT8 E_FFS_ucDiskSetVolumeLabel (UINT8 mpt_idx, const char *label);
UINT8 E_FFS_ucDiskGetVolumeLabel (UINT8 mpt_idx, char *label);
/******************************************************************************/
/* Function defined in ffs_dman.c                                             */
/******************************************************************************/

UINT8 E_FFS_open_dir(RO_PTR_TO_RO (UINT8) pucPath);
INT16 E_FFS_ucDmanGetNext (RO_PTR_TO_RW
						   (FFS_SMALL_DIRECTORY_LABEL_STRUCT) pstSmallLabel);
UINT8 E_FFS_ucDmanGetDone (void);

#ifdef FFS_FEATURE_DIRECTORIES
UINT8 E_FFS_ucDmanIsDir (RO_PTR_TO_RO (UINT8) pucPath);

UINT8 E_FFS_ucDmanMkDir (RO_PTR_TO_RO (UINT8) pucPath,
						 RO_PTR_TO_RO (UINT8) pucNameAndExt);

UINT8 E_FFS_ucDmanRmDir (RO_PTR_TO_RO (UINT8) pucPath);

UINT8 E_FFS_ucDmanDelTree (RO_PTR_TO_RO (UINT8) pucPath);

#else
#define E_FFS_ucDmanIsDir(a)  FFS_RV_FEATURE_UNAVAILABLE
#define E_FFS_ucDmanMkDir(a,b)  FFS_RV_FEATURE_UNAVAILABLE
#define E_FFS_ucDmanRmDir(a)    FFS_RV_FEATURE_UNAVAILABLE
#define E_FFS_ucDmanDelTree(a)  FFS_RV_FEATURE_UNAVAILABLE
#endif /* #ifdef FFS_FEATURE_DIRECTORIES                                      */

UINT8 E_FFS_ucDmanIsPart (RO_PTR_TO_RO (UINT8) pucPath);

//#define E_FFS_ucDmanSetCwd(a)    FFS_RV_FEATURE_UNAVAILABLE
//#define E_FFS_ucDmanGetCwd(a,b)  FFS_RV_FEATURE_UNAVAILABLE


/******************************************************************************/
/* Function defined in ffs_file.c                                             */
/******************************************************************************/

UINT8 E_FFS_ucFileCreate (RO_PTR_TO_RO (UINT8) pucPath,
						  RO_PTR_TO_RO (UINT8) pucNameAndExt,
						  BOOL flag_create);

UINT8 E_FFS_ucFileOpen (RO_PTR_TO_RO (UINT8) pucPath,
						UINT8 ucAccessMode,
						UINT16 flag, RO_PTR_TO_RW (UINT8) pucHandle);

UINT8 E_FFS_ucFileLSeek (UINT8 ucHandle, FFS_FILE_LENGTH_TYPE tFilePointer);

UINT8 E_FFS_ucFileRead (UINT8 ucHandle, RO_PTR_TO_RW (FFS_FILE_LENGTH_TYPE)
						ptNrOfBytes, RO_PTR_TO_RW (UINT8) pucBuffer);

UINT8 E_FFS_ucFileWrite (UINT8 ucHandle,
						 RO_PTR_TO_RO (FFS_FILE_LENGTH_TYPE) ptNrOfBytes,
						 RO_PTR_TO_RW (UINT8) pucBuffer);

UINT8 E_FFS_ucFileClose (UINT8 ucHandle);

UINT8 E_FFS_ucFileSync(UINT8 ucHandle);


UINT8 E_FFS_ucFileAllocate (UINT8 ucHandle,
							FFS_FILE_LENGTH_TYPE tNewFileLength);

UINT8 E_FFS_ucFileFstat (UINT8 ucHandle,
						 RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)
						 pstSmallLabel);


#if (FFS_UNICODE ==1)
/**************************************************/
// Unicode FFS       
/**************************************************/
UINT8 E_FFS_ucFileCreate_uc (
              UINT8                 mp_idx,
              const T_WCHAR  		*pathName_p,
              const T_WCHAR  		*fileName_p,
              BOOL                  flag_create);

UINT8 E_FFS_ucFileOpen_uc ( UINT8                 mp_idx,
                            const T_WCHAR  		  *pucPath,
                            UINT8                 ucAccessMode,
                            UINT16                flag,
                            RO_PTR_TO_RW (UINT8)  pucHandle);


UINT8 E_FFS_ucDmanRmDir_uc (UINT8 mp_idx, 
                            RO_PTR_TO_RO (T_WCHAR) pucPath);
UINT8 E_FFS_ucFmanUnlink_uc (UINT8 mp_idx, 
                            RO_PTR_TO_RO (T_WCHAR) pucPath);

UINT8 E_FFS_ucFmanMv_uc (UINT8 mountpoint, 
                          RO_PTR_TO_RO (T_WCHAR) oldfileName,
                          RO_PTR_TO_RO (T_WCHAR) oldfullpathname,
                          RO_PTR_TO_RO (T_WCHAR) newPathname, 
                          RO_PTR_TO_RO (T_WCHAR) newName);

UINT8 E_FFS_ucDmanMkDir_uc(
              UINT8                               mp_idx,
              const T_WCHAR                      *pathName_p,
	          const T_WCHAR                      *dirName_p);

UINT8 E_FFS_ucDiskStat_uc(
           const T_WCHAR                       *pathName_p,
           RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)  label,
           UINT8                                      mp_idx);

UINT8 E_FFS_ucFmanSetAttributes_uc
  (RO_PTR_TO_RO (T_WCHAR) pucPath, UINT8 ucAttributes, UINT8 mountpoint);

UINT8 E_FFS_ucFmanGetAttributes_uc
  (RO_PTR_TO_RO (T_WCHAR) pucPath, RO_PTR_TO_RW (UINT8) pucAttributes, UINT8 mountpoint);


UINT8 E_FFS_ucFmanGetDateTime_uc
  (RO_PTR_TO_RO (T_WCHAR) pucPath, RO_PTR_TO_RW (UINT16) pucDate,RO_PTR_TO_RW (UINT16) pucTime,
  									RO_PTR_TO_RW (UINT16) pcDate,RO_PTR_TO_RW (UINT16) pcTime,
  									UINT8 mountpoint);


UINT8 E_FFS_open_dir_uc(UINT8  mp_idx, RO_PTR_TO_RO (T_WCHAR) pucPath);

/**************************************************/
// end of Unicode FFS       
/**************************************************/
#endif



						 

/******************************************************************************/
/* Function defined in fman_file.c                                            */
/******************************************************************************/
UINT8 E_FFS_ucFmanMv (RW_PTR_TO_RO (UINT8) pucCurrentPath,
					  RW_PTR_TO_RO (UINT8) pucNewPath,
					  RW_PTR_TO_RO (UINT8) pucNameAndExt);

UINT8 E_FFS_ucFmanUnlink (RW_PTR_TO_RO (UINT8) pucPath);

UINT8 E_FFS_ucFmanSetAttributes (RW_PTR_TO_RO (UINT8) pucPath,
								 UINT8 ucAttributes);

UINT8 E_FFS_ucFmanGetAttributes (RW_PTR_TO_RO (UINT8) pucPath,
								 RO_PTR_TO_RW (UINT8) pucAttributes);

UINT8 E_FFS_ucFmanGetDateTime (RW_PTR_TO_RO (UINT8) pucPath,
								 RO_PTR_TO_RW (UINT16) pucDate,RO_PTR_TO_RW (UINT16) pucTime, 
								 RO_PTR_TO_RW (UINT16) pcDate,RO_PTR_TO_RW (UINT16) pcTime);


UINT8 E_FFS_ucFileTrunc (UINT8 ucHandle, INT32 Newsize);
#endif

