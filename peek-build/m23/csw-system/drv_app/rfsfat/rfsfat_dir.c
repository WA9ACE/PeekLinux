/**
 * @file  rfsfat_dir.c
 *
 * The directory handler is responsible for adding, removing
 * and searching directory tables and labels of a directory
 * table. The directory labels are written in the FAT16
 * format.
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

#include <string.h>
#include <ctype.h>


#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"

#include "rfsfat/rfsfat_ffs.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_dir.h"
#include "rfsfat/rfsfat_clm.h"
#include "rfsfat/rfsfat_fam.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_posconf.h"
#include "rfsfat/rfsfat_ffs_file.h"
#include "rfsfat/rfsfat_ffs_disk.h"
#include "string.h"


/******************************************************************************
 *      Module specific Defines and enums                                     *
 ******************************************************************************/
/* Define module ID string for debugging.                                     */
#ifdef DEBUG_TRACING
#undef  DEBUG_MOD_ID_STRING
#define DEBUG_MOD_ID_STRING                                              "DIR "
#endif

/* Maybe there are features disabled/enabled, so the file ffs.h must be       */
/* included first.                                                            */
#ifndef FFS_FAT_H
#error "ERROR: The file ffs.h must be included before this file."
#endif

#define RFSFAT_SFN_OPT 			   1

#if (RFSFAT_SFN_OPT == 1)
#define RFSFAT_SFN_THILDA_LMT      5
#endif



/* Position within a directory table of the entry of the upper directory      */
/* '..'.                                                                      */
#define POS_META_DATA_UPPER_DIR  0x20

/* Position within a directory table of the entry of the last entry (mark) in */
/* a new created directory.                                                   */
#define POS_META_DATA_LAST_ENTRY 0x40
#define ASCII_DOT           '.' /* ASCII values of often used character.      */
#define ASCII_SPACE         ' ' /* ASCII values of often used character.      */


#define DIR_PART_ATTR     0x08

#define MAX_TAIL          6
#define TAIL_ONES         0x01
#define TAIL_TENS         0x02
#define TAIL_HUND         0x04
#define TAIL_THOUS        0x08
#define TAIL_TENTHOUS     0x10
#define TAIL_HUNDTHOUS    0x20

#define OEM_TILDE                                                   '~'


/******************************************************************************
 *      Module specific Macros and Typedefs                                   *
 ******************************************************************************/
 
      
#define LAST_LABEL (DIR_META_DATA.aucName[0] == ((UINT8)META_MARK_LAST))
#define EMPTY_LABEL (DIR_META_DATA.aucName[0] == ((UINT8)META_MARK_EMPTY))
#define RECOG_LFN (((UINT8)(DIR_META_DATA.ucAttribute & LFN_MASK)) == ((UINT8)LFN_MASK))
#define NR_LFN_ENTRIES (DIR_META_DATA.aucName[0] ^ ((UINT8)FIRST_POS_MARKER))

#define RM_CHAR(p,c)        {UINT8* str; do{ str = (UINT8*)strchr((const char *)p, c); \
                                             if(str != NULL){*str = (UINT8)EOSTR; str++; \
                                             strcat((char*)p, (const char*)str);} \
                                           }while(str != NULL); }

#define NR_LFN_LABELS(a) {UINT16 strsize = strlen((const char*)RFSFAT_LFN); \
                          if((strsize % CHARS_IN_LFNE) != 0) \
                          { a = (strsize / CHARS_IN_LFNE) + 2; } \
                          else { a = (strsize / CHARS_IN_LFNE) + 1; }}
                                  
#define GET_META_DATA(loc, mdat) {UINT8 res; res = ucGetMetaData (loc); \
                                  if(res != FFS_NO_ERRORS) {RFSFAT_SEND_TRACE_ERROR \
                                    ("RFSFAT: ucGetMetaData failed"); return res; } \
                                  CPY_LBL_TO_LBL(mdat, DIR_META_DATA_P);}

#define CREATE_UNI_CODE_STR { UINT16 size; UINT16 cnt; size = ((strlen((const char*)RFSFAT_LFN) + 1) * 2); \
							  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL)	\
                              RFSFAT_GETBUF(size, &RFSFAT_LFN_FROM_LABEL); \
                              rfsfat_vpMemset(RFSFAT_LFN_FROM_LABEL, 0x00, size); \
                              for(cnt=0;cnt<(size/2);cnt++){RFSFAT_LFN_FROM_LABEL[cnt] = \
                              (UINT16)RFSFAT_LFN[cnt];}}

#define FREE_UNI_CODE_STR   {RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL); \
                             RFSFAT_LFN_FROM_LABEL = NULL;}

#define DIR_CLUSTER_NMB_2_UINT32_NMB(dirLabelStruct_p) \
                            ( (dirLabelStruct_p->uiFirstClusterNumberHigh << 16) | \
                               dirLabelStruct_p->uiFirstClusterNumber)
/* Recovery will be done If there are lot of cluster per direcory */
#define START_RECOVERY_CLUSTER_NR		100

typedef enum
{
  TAILNUM_ONES = 0,
  TAILNUM_TENS,
  TAILNUM_HUND,
  TAILNUM_THOUS,
  TAILNUM_TENTHOUS,
  TAILNUM_HUNDTHOUS
}TAIL_TYPE;



/******************************************************************************
 *      Definition global Variables                                           *
 ******************************************************************************/

/******************************************************************************
 *      Definition of static-Variables                                        *
 ******************************************************************************/


/******************************************************************************
 *      Static-Function prototypes                                            *
 ******************************************************************************/
static char is_root_cluster_last(RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation);
UINT8
ucGetNewLabelPos (DIR_LABEL_LOCATION_STRUCT* pstLabelLocation, 
                  UINT8 nr_adj_lbl);


static UINT8 ucGetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
              pstLabelLocation);
DIR_LFN_RET copy_lfn_strpart(LFN_LABEL_STRUCT* const lfne_p, UINT8 * const str_pos_p);
UINT16 copy_strpart_lfn(LFN_LABEL_STRUCT* const lfne_p, UINT8 * const str_pos_p, UINT16 str_size);
BOOL lfn_compare (const UINT8* const name1, const UINT16* const name2);
BOOL sfn_compare (const UINT8* const name1, const UINT8* const name2);
UINT8 lfn_new_label_pos(DIR_LABEL_LOCATION_STRUCT*   tmplabelloc_p);
UINT8 create_short_name(const DIR_LABEL_LOCATION_STRUCT* parentdirloc_p, UINT8* sfn);
UINT8 create_write_label(DIR_LABEL_LOCATION_STRUCT* filedirloc_p,
                         DIR_DIRECTORY_LABEL_STRUCT * pstMetaData,
                         const UINT8* filedir_name);
UINT8 dotdot_new_parent(DIR_DIRECTORY_LABEL_STRUCT* orgmetdat_p, UINT8 mpt_idx, 
                         UINT32 new_first_cl, UINT32 datacl);
static UINT8 is_cluster_recovery(UINT8 mpt_idx, UINT32 cluster_pos);
#if(RFSFAT_SFN_OPT == 1)
static UINT8 RFSFAT_rand_SFN(UINT8 * shortname, UINT32 count);
#endif

#if (FFS_UNICODE ==1)
/**************************************************/
// Unicode       
/**************************************************/
static BOOL  lfn_compare_uc (const UINT16* const name1, const UINT16* const name2);
static UINT8 create_short_name_uc(DIR_LABEL_LOCATION_STRUCT* const  parentdirloc_p,
						          UINT8                    * const  sfn_p,
            					  const  T_WCHAR             *nameUnicode_p);

UINT8 copy_strpart_lfn_uc(LFN_LABEL_STRUCT       *const lfne_p,
					          UINT8                  *const str_pos_p,
            				  UINT16                 str_size,
            				  const T_WCHAR  		 *nameUnicode_p);

UINT8 lfn_new_label_pos_uc(DIR_LABEL_LOCATION_STRUCT  *const tmplabelloc_p,
            			   const T_WCHAR              *nameUnicode_p);


/**************************************************/
// end of Unicode       
/**************************************************/
#endif





/******************************************************************************
 *      Implementation of Global Function                                     *
 ******************************************************************************/


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucGetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
           pstLabelLocation,
           RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData)
/* PURPOSE      : Function gets the directory label meta-data and stores it
 *                in the buffer, pstMetaData is pointing at.
 *
 * ARGUMENTS    : -pstLabelLocation : indicates partition, directory table and
 *                 position of entry in table of which meta data must be read.
 *                -pstMetaData : buffer in which meta data will be copied.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                FFS_RV_ERR_ENTRY_EMPTY:
 *                The requested entry is marked empty.
 *
 *                FFS_RV_ERR_LAST_ENTRY:
 *                The requested entry is marked to be the last entry, and so
 *                the entry is not useable (Like the NULL char in a string.).
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_LIST_ERROR:
 *                Logical error in cluster linked list.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation and pstMetaData may not be NULL.
 *
 * POSTCONDITION: Directory label meta-data is read and stored in structure
 *                pstMetaData is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : -
 ******************************************************************************/
{
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && (pstMetaData != NULL)),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* Get metadata and copy it into the given buffer.                          */
  /****************************************************************************/

  GET_META_DATA(pstLabelLocation, pstMetaData);

  if (pstMetaData->aucName[0] == META_MARK_EMPTY)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_ENTRY_EMPTY",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_ENTRY_EMPTY;
  }

  if (pstMetaData->aucName[0] == META_MARK_LAST)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_LAST_ENTRY",
             RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_LAST_ENTRY;
  }
  return FFS_NO_ERRORS;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucSetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
           pstLabelLocation,
           RO_PTR_TO_RO (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData)
/* PURPOSE      : This function stores a file or directory's meta data to the
 *                medium, using the cluster manager.
 *                Before any write operation will take place, the meta data
 *                is first set to 'empty/not used' by setting the first char
 *                of the name to '0xE5'. After the write operation was
 *                completed successfully, the mark is removed and replaced by
 *                the original character.
 *
 * ARGUMENTS    : -pstLabelLocation : Indicates partition, directory table and
 *                 position of entry in table of which meta data must be read.
 *                -pstMetaData : Buffer in which meta data will be copied.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation and pstMetaData may not be NULL.
 *
 * POSTCONDITION: Directory label meta-data is written.
 *
 * ERROR AND EXCEPTION HANDLING: Nonr.
 *
 * NOTE         : Security: To proof the file-system the first byte of the
 *                label gets value 0xE5 before writing the meta-data. The
 *                first byte of the label indicates now that the label is
 *                deleted, at the end of all write actions the first byte of
 *                the meta-data will be replaced with the first character of
 *                the label name.
 ******************************************************************************/
{
  UINT8 ucResult;

  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && (pstMetaData != NULL)),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* First, mark entry as empty.                                              */
  /****************************************************************************/
  DIR_META_DATA.aucName[0] = META_MARK_EMPTY;
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_NAME);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ucSetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

  /****************************************************************************/
  /* Copy information in entry, except name (because of the empty mark.)      */
  /****************************************************************************/
  CPY_LBL_TO_LBL(DIR_META_DATA_P, pstMetaData);

  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL_BUT_NAME);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: ucSetMetaData failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }

 /****************************************************************************/
  /* Remove empty mark.                                                       */
  /****************************************************************************/
  CPY_LBL_TO_LBL(DIR_META_DATA_P, pstMetaData);
  
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_NAME);

  return ucResult;

}


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucCreateFile (DIR_LABEL_LOCATION_STRUCT * const pstLabelLocation,
                  DIR_DIRECTORY_LABEL_STRUCT * const pstMetaData)
/* PURPOSE      : This function creates a new directory label for a file in
 *                the directory table. The file meta-data will be written and
 *                for the cluster number the value 0x0000 will be used. (The
 *                first cluster of the file-data will be determined when
 *                writing bytes to the file-data.)
 * ARGUMENTS    : -pstLabelLocation : This structure must contain the partition
 *                 Id. and the first cluster number of the directory, used
 *                 for adding the new file.
 *                -pstMetaData : This structure must contain the padded name
 *                 and extension. Besides the date and time fields will be
 *                 used.
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation and pstMetaData may not be NULL.
 * POSTCONDITION: After the operation was ended successfully, the structure
 *                pstLabelLocation will also contain the offset of the new
 *                label within the directory table. The structure pstMetaData
 *                is pointing at, contains all the information, as written
 *                to the partition.
 * ERROR AND EXCEPTION HANDLING: None.
 * NOTE         : None.
 ******************************************************************************/
{
  UINT8 ucResult = FFS_NO_ERRORS;
  DIR_LABEL_LOCATION_STRUCT parentlblloc;
  UINT8 short_file_name[FFS_MAX_SIZE_NAME_DOT_EXT]={0};
 // UINT8 short_file_name[] = "TESTFILETXT";

  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && (pstMetaData != NULL)),
             return DIR_RV_INVALID_PARAM);

  parentlblloc = *pstLabelLocation;
 RFSFAT_SEND_TRACE ("RFSFAT: DIR_ucCreateFile entered",RV_TRACE_LEVEL_DEBUG_LOW);

  /****************************************************************************/
  /* Search for a free place in directory table.                              */
  /****************************************************************************/

  ucResult = lfn_new_label_pos (pstLabelLocation);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: lfn_new_label_pos failed");
    return ucResult;
  }
  if(RFSFAT_LFN_FLAG)
  {
    ucResult = create_short_name(&parentlblloc, short_file_name);
    if (ucResult != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_short_name failed");
      return ucResult;
    }
  }
  //create label
  pstMetaData->ucAttribute = DEFAULT_FILE_ATTRIBUTE;
  pstMetaData->uiFirstClusterNumber = DEFAULT_FILE_FIRST_CLUSTER;
  pstMetaData->uiFirstClusterNumberHigh = DEFAULT_FILE_FIRST_CLUSTER;
  pstMetaData->ulFileSize = DEFAULT_FILE_SIZE;


  ucResult = create_write_label(pstLabelLocation, pstMetaData, short_file_name);
  if(ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_write_label file label failed");
    return ucResult;
  }
  return ucResult;
}

#ifdef FFS_FEATURE_DIRECTORIES
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucCreateDir (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)pstLabelLocation,
         RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData)
/* PURPOSE      : This function creates a new directory label for a sub-
 *                directory. At the end of this function, the new directory
 *                contains three different entries: ".", ".." and an entry
 *                marked as 'the last entry of this table'.
 *
 * ARGUMENTS    : -pstLabelLocation : indicates the partition Id. and the first
 *                 cluster number of the directory table of which the new
 *                 label should be added.
 *                -pstMetaData : must contain name, extension, time and date
 *                 of new directory label.
 *
 * RETURNS      : FAM_RET_FAT_READ_ERROR:
 *                Error occurred while reading from the FAT table.
 *
 *                FAM_RET_FAT_WRITE_ERROR:
 *                Error occurred while writing to the FAT table.
 *
 *                FAM_RET_NO_FAT_CLUSTER:
 *                Not a valid FAT cluster number.
 *
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation and pstMetaData may not be NULL.
 *
 * POSTCONDITION: After the operation, the offset of the new label is stored
 *                in the structure, pstLabelLocation is pointing at. The
 *                real data in the new label is als copied into the buffer,
 *                pstMetaData is pointing at.
 *
 * ERROR AND EXCEPTION HANDLING:
 *
 * NOTE         :
 ******************************************************************************/
{
  DIR_LABEL_LOCATION_STRUCT * tmplblloc_p=NULL, tmplblloc;
  UINT8 ucResult = FFS_NO_ERRORS;
  DIR_LABEL_LOCATION_STRUCT parentlblloc;
  UINT8 short_file_name[FFS_MAX_SIZE_NAME_DOT_EXT]={0};
  UINT16  uiCnt;
  UINT16  uiNmbDirEntriesInCluster;

  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && (pstMetaData != NULL)),
             return DIR_RV_INVALID_PARAM);

  /* Copy label location of directory.                                        */  
  tmplblloc_p= &tmplblloc;
 
  tmplblloc_p->mpt_idx = pstLabelLocation->mpt_idx;
  tmplblloc_p->uiFirstCl= pstLabelLocation->uiFirstCl;
  tmplblloc_p->tLblPos = pstLabelLocation->tLblPos;
  
  /****************************************************************************/
  /* Create new chain, containing the new directory table of the new          */
  /* directory.                                                               */
  /****************************************************************************/
  /* Get free entry in the neighbourhood of this entry.                       */

  ucResult = FAM_ucCreateNewChain (pstLabelLocation->mpt_idx,
                   &(pstLabelLocation->uiFirstCl));
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FAM_ucCreateNewChain failed");
    return ucResult;
  }

  //Clear new directory cluster to ensure that for each entry 
  // at least the first byte is cleared (LAST_ENTRY indicator). Other wise old
  //  data can (and will) be seen as entries (when adding new elements).
  uiCnt = (UINT16) (MMA_BOOT_SECTOR[pstLabelLocation->mpt_idx].ucSectorPerCluster * 
                    MMA_BOOT_SECTOR[pstLabelLocation->mpt_idx].uiBytesPerSector);
  uiNmbDirEntriesInCluster = uiCnt / DIR_SIZE_META_ALL;
  /* clean as many entries as fit in a cluster */
  for(uiCnt = 0; uiCnt < uiNmbDirEntriesInCluster; uiCnt++)
  {
    pstLabelLocation->tLblPos = uiCnt * DIR_SIZE_META_ALL;
  /* Clear meta data. */
   (void) rfsfat_vpMemset ((void*)(DIR_META_DATA_P), DEFAULT_DIR_FIRST_CLUSTER,
              DIR_SIZE_META_ALL);
    ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
    if (ucResult != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData Cleanup failed");
       goto error_label;
    }
  }
  
  /****************************************************************************/
  /* Create the entries in the new directory table (".", ".." and             */
  /* "LAST_ENTRY").                                                           */
  /****************************************************************************/
  pstMetaData->uiFirstClusterNumberHigh = pstLabelLocation->uiFirstCl>>16;
  pstMetaData->uiFirstClusterNumber = (UINT16)pstLabelLocation->uiFirstCl;
  pstMetaData->ucAttribute = DEFAULT_DIR_ATTRIBUTE;
  pstMetaData->ulFileSize = DEFAULT_DIR_SIZE;


  /* Copy given meta data. (Time, date etc. will be used in all labels        */
  /* that will be created.)                                                   */
  CPY_LBL_TO_LBL(DIR_META_DATA_P, pstMetaData);
  
  DIR_META_DATA.uiFirstClusterNumber = 0;
  DIR_META_DATA.uiFirstClusterNumberHigh = 0;

  /****************************************************************************/
  /* Add entry 'LAST_ENTRY at position 64.                                    */
  /****************************************************************************/
  pstLabelLocation->tLblPos = POS_META_DATA_LAST_ENTRY;

/* Copy name and extension "" (last entry).                                 */
  //UGLY however functional, the extention member is implicitely written here 
  (void) rfsfat_vpMemset (DIR_META_DATA.aucName, 0,DIR_SIZE_META_NAME);
  (void) rfsfat_vpMemset (DIR_META_DATA.aucExtension, 0, DIR_SIZE_META_EXT);

  RFSFAT_SEND_TRACE
  ("RFSFAT: write LAST_ENTRY in the third dir entry of new dir",
   RV_TRACE_LEVEL_DEBUG_LOW);
  /* Write to medium.                                                         */
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
     goto error_label;
  }
  /****************************************************************************/
  /* Add entry "." at position 0.                                             */
  /****************************************************************************/
  pstLabelLocation->tLblPos = 0;

/* Copy name and extension "" (last entry).                                 */
  /* copy ASCII_SPACE character */
  (void) rfsfat_vpMemset (DIR_META_DATA.aucName, ASCII_SPACE,DIR_SIZE_META_NAME);
  (void) rfsfat_vpMemset (DIR_META_DATA.aucExtension, ASCII_SPACE, DIR_SIZE_META_EXT);

  DIR_META_DATA.aucName[0] = ASCII_DOT;
  /*set the default lcase value */
  DIR_META_DATA.lcase = DEFAULT_LCASE_VALUE;

  /* Copy first cluster number.                                               */
  DIR_META_DATA.uiFirstClusterNumber = pstMetaData->uiFirstClusterNumber;
  DIR_META_DATA.uiFirstClusterNumberHigh = pstMetaData->uiFirstClusterNumberHigh;
  /* Write to medium.                                                         */
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
     goto error_label;
  }

  /****************************************************************************/
  /* Add entry ".." at position 32.                                           */
  /****************************************************************************/
  pstLabelLocation->tLblPos = DIR_SIZE_META_ALL;

  /* Copy name and extension ".."                                             */
  DIR_META_DATA.aucName[1] = ASCII_DOT;

 /*set the default lcase value */
  DIR_META_DATA.lcase = DEFAULT_LCASE_VALUE;

  /* Copy first cluster number.                                               */
  DIR_META_DATA.uiFirstClusterNumber = tmplblloc_p->uiFirstCl;
  DIR_META_DATA.uiFirstClusterNumberHigh = tmplblloc_p->uiFirstCl>>16;
  /* Write to medium.                                                         */
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);

  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
     goto error_label;
  }
  /****************************************************************************/
  /* Add entry for new directory to the given directory table.                */
  /****************************************************************************/
  parentlblloc = *tmplblloc_p;

  ucResult = lfn_new_label_pos (tmplblloc_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: lfn_new_label_pos failed");
    goto error_label;
  }
  if(RFSFAT_LFN_FLAG)
  {
    ucResult = create_short_name(&parentlblloc, short_file_name);
    if (ucResult != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_short_name failed");
 	   goto error_label;
    }
  }
  //create label
  pstMetaData->ucAttribute = DEFAULT_DIR_ATTRIBUTE;
  pstMetaData->ulFileSize = DEFAULT_DIR_SIZE;
  ucResult = create_write_label(tmplblloc_p, pstMetaData, short_file_name);
  if(ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_write_label file label failed");
    goto error_label;
  }
  
  
  return ucResult;


error_label:
    FAM_ucDelLinkedList(pstLabelLocation->mpt_idx, pstLabelLocation->uiFirstCl);
  return ucResult;
}
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucIsEmpty (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
/* PURPOSE      : This function checks if a directory is empty by using the
 *                first cluster number of the directory.
 *                A directory always starts at a new cluster and in this
 *                cluster the directory table is stored. The directory table
 *                of an empty directory contains only the directory labels of
 *                the current and upper directory (known as . and .. ) and can
 *                contain directory labels of deleted files and directories.
 *                If a directory label is deleted the first byte contains the
 *                value 0xE5. The first byte of a label contains normally the
 *                first character of the file or directory name. A directory
 *                table end is always indicated with an extra label with its
 *                first byte value 0x00.
 *
 *                A directory is not empty if the first byte of a label in the
 *                directory table contains a value different than:
 *
 *                '.'  The dot indicates the current directory or the upper directory.
 *                0xE5 Indicates that the file or directory is deleted.
 *                0x00 End of directory table indication.
 *
 * ARGUMENTS    : pstLabelLocation indicates the partition Id and the first
 *                cluster number of the directory table of the directory,
 *                which must be checked.
 *
 * RETURNS      : DIR_RV_DIR_IS_EMPTY:
 *                The indicated directory is empty.
 *
 *              : DIR_RV_DIR_IS_NOT_EMPTY:
 *                The indicated directory is not empty.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : None.
 *
 ******************************************************************************/
{
  UINT8 ucResult;
  FFS_FILE_LENGTH_TYPE tTmpLabelPos;

  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* Start requesting meta data of first entry.                               */
  /****************************************************************************/
  /* Copy current value of label position.                                    */
  tTmpLabelPos = pstLabelLocation->tLblPos;

  pstLabelLocation->tLblPos = 0;

  do
  {
    ucResult = ucGetMetaData (pstLabelLocation);
    if (ucResult == FFS_NO_ERRORS)
    {
      if (LAST_LABEL)
      {
        /* Directory is empty, return.                                          */
        ucResult = DIR_RV_DIR_IS_EMPTY;
      }
      else if ((!EMPTY_LABEL) && (DIR_META_DATA.aucName[0] != ASCII_DOT))
      {
        /* Directory is not empty, return.                                      */
		/* we can ignore the first .(ASCII_DOT) char in Directory */
		/* NOTE: The Unicode Name which starts with 0XE5 and ASCII_DOT 
		  as MSB might trip some here*/
        ucResult = DIR_RV_DIR_IS_NOT_EMPTY;
      }
      else
      {
        /* Label is not a "." or ".." directory, not empty, and not the last    */
        /* label of the directory table. Try next label.                        */
        /* No free entry found yet, go to next entry.                           */
        NEXT_LABEL(pstLabelLocation->tLblPos);
      }
    }
	else
		{
		   RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData failed failed"); 
		   return ucResult;
    }
  }
  while (ucResult == FFS_NO_ERRORS);

  /* Restore label location. */
  pstLabelLocation->tLblPos = tTmpLabelPos;
  return ucResult;
}
#endif
UINT8 is_cluster_recovery(UINT8 mpt_idx, UINT32 cluster_pos)
{

  UINT16 uiClusterSize;
  UINT16 clusterNr;
  
 /* determine the cluster size */
   uiClusterSize = (UINT16) (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
						     MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);

  /* determine the logical cluster Number */
  clusterNr = cluster_pos/uiClusterSize;

  if(START_RECOVERY_CLUSTER_NR)
  	{

  		if(clusterNr > START_RECOVERY_CLUSTER_NR)
  				return 1;    /* recovery is required */
  	}

  return 0;   /* No Need of recovery , let it be the Normal case */
 
}
#if 0

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucDeleteLabel (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
/* PURPOSE      : This function deletes a directory-table label by changing
 *                the first byte of the file or directory label's meta-data
 *                into 0xE5. The first byte of the label's meta-data is the
 *                first character of the file/directory name in the meta-data.
 *                Besides changing the directory label, the cluster-linked
 *                list will be removed by the FAT manager.
 *                This function removes file-data as well as the directory
 *                table of a subdirectory. In case of removing the label of
 *                a sub-directory be sure the sub directory is empty.
 *                When the label involves a LFN all the LFN labels including the 
 *                8+3 (SFN) label, will be set to E5 (removed).
 *
 * ARGUMENTS    : pstLabelLocation : Indicates the partition, the first cluter
 *                number of the directory table, and the offset within this
 *                table of the label that must be removed.
 *
 * RETURNS      : FAM_RET_FAT_READ_ERROR:
 *                Error occurred while reading from the FAT table.
 *
 *                FAM_RET_FAT_WRITE_ERROR:
 *                Error occurred while writing to the FAT table.
 *
 *                FAM_RET_NO_FAT_CLUSTER:
 *                Not a valid FAT cluster number
 *
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *                In case a directory label of a directory must be deleted, be
 *                sure this directory is empty.
 *
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : In case a directory label of a directory with a file was
 *                deleted, the clusters which are used by the file's data will
 *                be marked as 'used' but will never be accessible again. For
 *                this, be sure the directory of which you want to remove the
 *                label, is empty.
 *
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT8 nr_found_entries = 0;
  UINT8 cntr = 0;
  UINT32 clusterNmb;
  DIR_LABEL_LOCATION_STRUCT label_pos;
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);

  label_pos = *pstLabelLocation;
  /****************************************************************************/
  /* Copy empty mark in the first byte of name field(s).                         */
  /****************************************************************************/
  //determine if label to remove is LFN and how many labels are to be removed
  ucResult = DIR_find_lfn_of_sfn(&label_pos, &nr_found_entries);
  if((ucResult != FFS_NO_ERRORS) && (ucResult != DIR_RV_CORRUPT_LFN))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_find_lfn_of_sfn in DIR_ucDeleteLabel failed");
    return ucResult;
  }
  for(cntr = 0; cntr < nr_found_entries; cntr++)
  {
    ucGetMetaData(&label_pos);   /* read the contents of this label*/
	/* change the first char as 0XE5 -- META_MARK_EMPTY */
    DIR_META_DATA.aucName[0] = META_MARK_EMPTY;
    ucResult = ucSetMetaData (&label_pos, META_TAG_NAME);
    if (ucResult != FFS_NO_ERRORS)
    {
      return ucResult;
    }
    //check if labelpos is not incremented after last action
    if((nr_found_entries - cntr) > 1)
    {
      label_pos.tLblPos += DIR_SIZE_META_ALL;
    }
  }
  /****************************************************************************/
  /* Remove file data.                                                        */
  /****************************************************************************/

  //When LFN are removed there are at least two labels to remove. But always the 
  //8+3 (SFN) label is removed last. Because the ucSetMetaData (previously called)
  //reads the label to write before the actual writing, 
  //"DIR_META_DATA.uiFirstClusterNumber" does contain the actual firstclnr of the 
  //file (dir) after the last label is removed (LFN labels do NOT each store 
  //the FirstClNr) When the order in removing the labels will ever be changed 
  //this has to be accounted for!
  
  /* Delete/remove file data, if any.                                         */
 clusterNmb = (DIR_META_DATA.uiFirstClusterNumberHigh << 16) |
	       DIR_META_DATA.uiFirstClusterNumber;
 if (clusterNmb != 0)
 {
    ucResult = FAM_ucDelLinkedList (pstLabelLocation->mpt_idx, clusterNmb);
  }
  return ucResult;
}
#else
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucDeleteLabel (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
/* PURPOSE      : This function deletes a directory-table label by changing
 *                the first byte of the file or directory label's meta-data
 *                into 0xE5. The first byte of the label's meta-data is the
 *                first character of the file/directory name in the meta-data.
 *                Besides changing the directory label, the cluster-linked
 *                list will be removed by the FAT manager.
 *                This function removes file-data as well as the directory
 *                table of a subdirectory. In case of removing the label of
 *                a sub-directory be sure the sub directory is empty.
 *                When the label involves a LFN all the LFN labels including the 
 *                8+3 (SFN) label, will be set to E5 (removed).
 *
 * ARGUMENTS    : pstLabelLocation : Indicates the partition, the first cluter
 *                number of the directory table, and the offset within this
 *                table of the label that must be removed.
 *
 * RETURNS      : FAM_RET_FAT_READ_ERROR:
 *                Error occurred while reading from the FAT table.
 *
 *                FAM_RET_FAT_WRITE_ERROR:
 *                Error occurred while writing to the FAT table.
 *
 *                FAM_RET_NO_FAT_CLUSTER:
 *                Not a valid FAT cluster number
 *
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *                In case a directory label of a directory must be deleted, be
 *                sure this directory is empty.
 *
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : In case a directory label of a directory with a file was
 *                deleted, the clusters which are used by the file's data will
 *                be marked as 'used' but will never be accessible again. For
 *                this, be sure the directory of which you want to remove the
 *                label, is empty.
 *
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT8 nr_found_entries = 0;
  UINT8 cntr = 0;
  UINT32 clusterNmb;
  UINT8	 ENTRY_FILL=META_MARK_EMPTY;
  DIR_LABEL_LOCATION_STRUCT label_pos,label_pos_tmp,label_start;
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);

  label_pos = *pstLabelLocation;
  label_pos_tmp = *pstLabelLocation;

 if(is_cluster_recovery(label_pos_tmp.mpt_idx,label_pos_tmp.tLblPos))
 {

  /* pstLabelLocation->tLblPos points the SFN (which is last entry ) */
  /* so, check whether you are deleting the last entry or not */
  /* Check the next entry of the SFN is LAST ENTRY or not */
  label_pos_tmp.tLblPos += DIR_SIZE_META_ALL;
  ucResult = ucGetMetaData(&label_pos_tmp);

  if(ucResult == FFS_NO_ERRORS)
  	{
  	   if( DIR_META_DATA.aucName[0] == META_MARK_LAST )
  	   	{
		 		
  	   	    ENTRY_FILL = META_MARK_LAST;   /* Marking will done as last */
  	   	}
	   else
	   	{
	   	  ENTRY_FILL = META_MARK_EMPTY;   /* Marking will done as EMPTY */
	   	}
  	}
  else
  	{
  	 RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMeatData in DIR_ucDeleteLabel failed");
     return ucResult;
  	}
}
 else
 	{
	   	  ENTRY_FILL = META_MARK_EMPTY;   /* Marking will done as EMPTY */ 	 
 	}
  
  /****************************************************************************/
  /* Copy empty mark in the first byte of name field(s).                         */
  /****************************************************************************/
  //determine if label to remove is LFN and how many labels are to be removed
  ucResult = DIR_find_lfn_of_sfn(&label_pos, &nr_found_entries);
  if((ucResult != FFS_NO_ERRORS) && (ucResult != DIR_RV_CORRUPT_LFN))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_find_lfn_of_sfn in DIR_ucDeleteLabel failed");
    return ucResult;
  }

  /* copy the starting position */
  memcpy(&label_start,&label_pos,sizeof(DIR_LABEL_LOCATION_STRUCT));
  
  for(cntr = 0; cntr < nr_found_entries; cntr++)
  {
    if(ENTRY_FILL == META_MARK_LAST)
		{
		 memset(&DIR_META_DATA,META_MARK_LAST,DIR_SIZE_META_ALL);
		 ucResult = ucSetMetaData (&label_pos, META_TAG_ALL);  /* erase the complete label */
		}
	else
		{
		 ucGetMetaData(&label_pos);   /* read the contents of this label*/
  		 DIR_META_DATA.aucName[0] = ENTRY_FILL;   /* deleted */
		 ucResult = ucSetMetaData (&label_pos, META_TAG_NAME);  /* replace only the first char */
		}
  
    if (ucResult != FFS_NO_ERRORS)
    {
      return ucResult;
    }
    //check if labelpos is not incremented after last action
    if((nr_found_entries - cntr) > 1)
    {
      label_pos.tLblPos += DIR_SIZE_META_ALL;
    }
  }
  /****************************************************************************/
  /* Remove file data.                                                        */
  /****************************************************************************/

  //When LFN are removed there are at least two labels to remove. But always the 
  //8+3 (SFN) label is removed last. Because the ucSetMetaData (previously called)
  //reads the label to write before the actual writing, 
  //"DIR_META_DATA.uiFirstClusterNumber" does contain the actual firstclnr of the 
  //file (dir) after the last label is removed (LFN labels do NOT each store 
  //the FirstClNr) When the order in removing the labels will ever be changed 
  //this has to be accounted for!
  
  /* Delete/remove file data, if any.                                         */
 clusterNmb = (DIR_META_DATA.uiFirstClusterNumberHigh << 16) |
	       DIR_META_DATA.uiFirstClusterNumber;
 if (clusterNmb != 0)
 {
    ucResult = FAM_ucDelLinkedList (pstLabelLocation->mpt_idx, clusterNmb);
  }


 if(ENTRY_FILL == META_MARK_LAST)
 	{
 	   /* we have filled the last */
	   /* Truncate the FAT cluster chain is required */
	   /* we have first cluster number and position */


	   /* lets take the starting position */
	   /* Increment it by next last label */
	   label_start.tLblPos += DIR_SIZE_META_ALL;
	   /* At this position we have a last label */

	   /* Truncate at this position */
		FAM_ucTruncLinkedList(label_start.mpt_idx,label_start.uiFirstCl,label_start.tLblPos);
 	}

 
  return ucResult;
}




#endif
#if 0

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucDeleteLabel_only (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
/* PURPOSE      : This function deletes a directory-table label by changing
 *                the first byte of the file or directory label's meta-data
 *                into 0xE5. The first byte of the label's meta-data is the
 *                first character of the file/directory name in the meta-data.
 *                When the label involves a LFN all the LFN labels including the 
 *                8+3 (SFN) label, will be set to E5 (removed).
 *
 * ARGUMENTS    : pstLabelLocation : Indicates the partition, the first cluter
 *                number of the directory table, and the offset within this
 *                table of the label that must be removed.
 *
 * RETURNS      :
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *                In case a directory label of a directory must be deleted, be
 *                sure this directory is empty.
 *
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : In case a directory label of a directory with a file was
 *                deleted, the clusters which are used by the file's data will
 *                be marked as 'used' but will never be accessible again. For
 *                this, be sure the directory of which you want to remove the
 *                label, is empty.
 *
 ******************************************************************************/
{
  UINT8 ucResult=FFS_NO_ERRORS;
  UINT8 nr_found_entries = 0;
  UINT8 cntr = 0;
  UINT32 clusterNmb;
  DIR_LABEL_LOCATION_STRUCT label_pos;
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);

  label_pos = *pstLabelLocation;
  /****************************************************************************/
  /* Copy empty mark in the first byte of name field(s).                         */
  /****************************************************************************/
  //determine if label to remove is LFN and how many labels are to be removed
  ucResult = DIR_find_lfn_of_sfn(&label_pos, &nr_found_entries);
  if((ucResult != FFS_NO_ERRORS) && (ucResult != DIR_RV_CORRUPT_LFN))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_find_lfn_of_sfn in DIR_ucDeleteLabel failed");
    return ucResult;
  }
  for(cntr = 0; cntr < nr_found_entries; cntr++)
  {
    ucGetMetaData(&label_pos);   /* read the contents of this label*/
    DIR_META_DATA.aucName[0] = META_MARK_EMPTY;
    ucResult = ucSetMetaData (&label_pos, META_TAG_NAME);
    if (ucResult != FFS_NO_ERRORS)
    {
      return ucResult;
    }
    //check if labelpos is not incremented after last action
    if((nr_found_entries - cntr) > 1)
    {
      label_pos.tLblPos += DIR_SIZE_META_ALL;
    }
  }

   /* dont delete the file data */
  
  return ucResult;
}


#else


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucDeleteLabel_only (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
/* PURPOSE      : This function deletes a directory-table label by changing
 *                the first byte of the file or directory label's meta-data
 *                into 0xE5. The first byte of the label's meta-data is the
 *                first character of the file/directory name in the meta-data.
 *                When the label involves a LFN all the LFN labels including the 
 *                8+3 (SFN) label, will be set to E5 (removed).
 *
 * ARGUMENTS    : pstLabelLocation : Indicates the partition, the first cluter
 *                number of the directory table, and the offset within this
 *                table of the label that must be removed.
 *
 * RETURNS      :
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *                In case a directory label of a directory must be deleted, be
 *                sure this directory is empty.
 *
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : In case a directory label of a directory with a file was
 *                deleted, the clusters which are used by the file's data will
 *                be marked as 'used' but will never be accessible again. For
 *                this, be sure the directory of which you want to remove the
 *                label, is empty.
 *
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT8 nr_found_entries = 0;
  UINT8 cntr = 0;
  UINT8	 ENTRY_FILL=META_MARK_EMPTY;
  DIR_LABEL_LOCATION_STRUCT label_pos,label_pos_tmp,label_start;
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);

  label_pos = *pstLabelLocation;
  label_pos_tmp = *pstLabelLocation;



 if(is_cluster_recovery(label_pos_tmp.mpt_idx,label_pos_tmp.tLblPos))
 {

  /* pstLabelLocation->tLblPos points the SFN (which is last entry ) */
  /* so, check whether you are deleting the last entry or not */
  /* Check the next entry of the SFN is LAST ENTRY or not */
  label_pos_tmp.tLblPos += DIR_SIZE_META_ALL;
  ucResult = ucGetMetaData(&label_pos_tmp);

  if(ucResult == FFS_NO_ERRORS)
  	{
  	   if( DIR_META_DATA.aucName[0] == META_MARK_LAST )
  	   	{
		 		
  	   	    ENTRY_FILL = META_MARK_LAST;   /* Marking will done as last */
  	   	}
	   else
	   	{
	   	  ENTRY_FILL = META_MARK_EMPTY;   /* Marking will done as EMPTY */
	   	}
  	}
  else
  	{
  	 RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMeatData in DIR_ucDeleteLabel failed");
     return ucResult;
  	}
  }
 else
 	{
	   	  ENTRY_FILL = META_MARK_EMPTY;   /* Marking will done as EMPTY */ 	 
 	}




 
  /****************************************************************************/
  /* Copy empty mark in the first byte of name field(s).                         */
  /****************************************************************************/
  //determine if label to remove is LFN and how many labels are to be removed
  ucResult = DIR_find_lfn_of_sfn(&label_pos, &nr_found_entries);
  if((ucResult != FFS_NO_ERRORS) && (ucResult != DIR_RV_CORRUPT_LFN))
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_find_lfn_of_sfn in DIR_ucDeleteLabel failed");
    return ucResult;
  }

  /* copy the starting position */
  memcpy(&label_start,&label_pos,sizeof(DIR_LABEL_LOCATION_STRUCT));
  
  for(cntr = 0; cntr < nr_found_entries; cntr++)
  {

	 if(ENTRY_FILL == META_MARK_LAST)
		{
		 memset(&DIR_META_DATA,META_MARK_LAST,DIR_SIZE_META_ALL);
		 ucResult = ucSetMetaData (&label_pos, META_TAG_ALL);  /* erase the complete label */
		}
	else
		{
		 ucGetMetaData(&label_pos);   /* read the contents of this label*/
  		 DIR_META_DATA.aucName[0] = ENTRY_FILL;   /* deleted */
		 ucResult = ucSetMetaData (&label_pos, META_TAG_NAME);  /* replace only the first char */
		}
  
    if (ucResult != FFS_NO_ERRORS)
    {
      return ucResult;
    }
    //check if labelpos is not incremented after last action
    if((nr_found_entries - cntr) > 1)
    {
      label_pos.tLblPos += DIR_SIZE_META_ALL;
    }
  }
  /****************************************************************************/
  /* Dont Remove file data.      ALERT ALERT                                   */
  /****************************************************************************/


 if(ENTRY_FILL == META_MARK_LAST)
 	{
 	   /* we have filled the last */
	   /* Truncate the FAT cluster chain is required */
	   /* we have first cluster number and position */


	   /* lets take the starting position */
	   /* Increment it by next last label */
	   label_start.tLblPos += DIR_SIZE_META_ALL;
	   /* At this position we have a last label */


	   /* Truncate at this position */
		FAM_ucTruncLinkedList(label_start.mpt_idx,label_start.uiFirstCl,label_start.tLblPos);
 	}

 
  return ucResult;

}

#endif


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucGetLabel (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
        pstLabelLocation,
        RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData)
/* PURPOSE      : Function locates the directory label of a file or directory
 *                with the use of its name and extension. The name and
 *                extension should be padded, using spaces (in case the name
 *                length is not 8 bytes, or the extension length is not 3
 *                bytes. )
 * ARGUMENTS    : The given parameter pstMetaData must point at a buffer
 *                containing (at least) the file name and extension. Other
 *                fields will not be used, and so overwritten.
 *                The parameter pstLabelLocation should indicate the partition,
 *                and the first cluster number of the directory table, used
 *                for searching the name in.
 * RETURNS      : DIR_RV_DIR_NOT_FOUND:
 *                The directory could not be found.
 *
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation and pstMetaData may not be NULL.
 * POSTCONDITION: In case the entry was found, it's metadata is copied in the
 *                buffer pstMetaData is pointing at. pstLabelLocation also
 *                contain the offset within the directory table of the found
 *                entry.
 * ERROR AND EXCEPTION HANDLING: -
 * NOTE         : -
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT8 res;
  UINT8 name_in_label[FFS_MAX_SIZE_NAME_DOT_EXT];
  UINT8 lookup_name[FFS_MAX_SIZE_NAME_DOT_EXT];
  DIR_LFN_RET retval;

  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL)
            && ((pstMetaData != NULL))),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* Start requesting meta data of first entry.                               */
  /****************************************************************************/
  pstLabelLocation->tLblPos = 0;

  do
  {
    ucResult = ucGetMetaData (pstLabelLocation);

    if (ucResult != FFS_NO_ERRORS)
    {
      /* Error reading, so stop.                                              */
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData failed");
      return ucResult;
    }
    else if (LAST_LABEL)
    {
      RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: DIR_RV_DIR_NOT_FOUND");
      return DIR_RV_DIR_NOT_FOUND;
    }
    else if (!EMPTY_LABEL)
    {
      //determine if label belongs to LFN
      if(RECOG_LFN)
      {
        //verify if the first entry of the LFN is found
        if(RFSFAT_FIRST_LFN_ENTRY)
        {        
          //when the programmer didnt free the RFSFAT_LFN_FROM_LABEL buffer
          //it is done here to prevent leaking
         
          RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
          RFSFAT_LFN_FROM_LABEL = NULL;
          res = DIR_read_lfn_from_label(pstLabelLocation, &retval);
          if(res != FFS_NO_ERRORS)
          {
			  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;          
            return res;
          }
          switch(retval)
          {
            case MEM_ERROR:
            {
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: not enough mem to store LFN");
              return RFSFAT_MEMORY_ERR;
            }
            /* break; */

            case COPY_ERROR:
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: copying the LFN into buf failed");
              return DIR_RV_INVALID_PARAM;
          /*  break;  */
		  
            case NOT_A_VALID_LFN_LABEL:
            case WRONG_CRC:
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
              RFSFAT_LFN_FROM_LABEL = NULL;
              //don't stop searching just signal the potential flaw
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: the lfn label found is not part of a lfn");
            break;
            case LFN_EMPTY_LABEL:
              //empty label was found so try to find the next one
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
              RFSFAT_LFN_FROM_LABEL = NULL;
            break;
            case NO_ERRORS:
              //compare the names
              //it can still happen that the LFN read from the label has the same name as a
              //8+3 entry so do not default compare with RFSFAT_LFN but check LFN flag to
              //see  
              if(RFSFAT_LFN_FLAG)
              {
                if(lfn_compare(RFSFAT_LFN, RFSFAT_LFN_FROM_LABEL))
                {
                  // Label with same name and extension found, return.
                  // true LFN
                  // return short label for admin
                  
                  CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P); 
                  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
                  RFSFAT_LFN_FROM_LABEL = NULL;
                  return FFS_NO_ERRORS;
                }
              } else {
                //prepare shortname to be compared
                DIR_cpy_lbl_str(lookup_name, pstMetaData->aucName, pstMetaData->aucExtension);
                if(lfn_compare(lookup_name, RFSFAT_LFN_FROM_LABEL))
                {
                  // Label with same name and extension found, return. 
                  // lfn that fits in 8+3, must be case preserved
                  // return short label for admin
                  CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P);
                  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
                  RFSFAT_LFN_FROM_LABEL = NULL;
                  return FFS_NO_ERRORS;
                } else {
                  //maybe were looking for shortname of lfn
                  //prepare shortnames to be compared
                  DIR_cpy_lbl_str(name_in_label, DIR_META_DATA.aucName, DIR_META_DATA.aucExtension); 
                  DIR_cpy_lbl_str(lookup_name, pstMetaData->aucName, pstMetaData->aucExtension);
                  if(sfn_compare(lookup_name, name_in_label))
                  {
                    // Label with same name and extension found, return.
                    // real 8+3
                    CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P);
                    RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
                    RFSFAT_LFN_FROM_LABEL = NULL;
                    return FFS_NO_ERRORS;
                  }
                }
              }
              //return if found
            break;
            
            default :
            break;
          }
        } //else not the first lfn label so skip it
      } else {
        //check if this label contains the name we're searching
        //expand to compare lfn with 8+3
        if(RFSFAT_LFN_FLAG)
        {
          //prepare shortname to be compared
          DIR_cpy_lbl_str(name_in_label, DIR_META_DATA.aucName, DIR_META_DATA.aucExtension);
          if(sfn_compare(RFSFAT_LFN, name_in_label))
          {

            //Label with same name and extension found, return.
            //lfn that fits in 8+3, must be case preserved
            CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P);
            return FFS_NO_ERRORS;
          }
        } else {
          //change
          //prepare shortnames to be compared
          DIR_cpy_lbl_str(name_in_label, DIR_META_DATA.aucName, DIR_META_DATA.aucExtension); 
          DIR_cpy_lbl_str(lookup_name, pstMetaData->aucName, pstMetaData->aucExtension);
          if(sfn_compare(lookup_name, name_in_label))
          {
            // Label with same name and extension found, return.
            // real 8+3
            CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P);
            return FFS_NO_ERRORS;
          }
        }
      }
    }
    /* Not the same name, try the next one.                                   */
    NEXT_LABEL(pstLabelLocation->tLblPos);
    RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
    RFSFAT_LFN_FROM_LABEL = NULL;
  }
  /*lint -save -e506 */
  while (1);
  /*lint -restore */
}

//srclblloc_p will point to the 8+3 label Even when part of a LFN
//dstlblloc_p will point to the 8+3 label Even when part of a LFN
UINT8 DIR_lfn_move( BOOL rename_flg,
                    BOOL newnamelfn, 
                    BOOL orgnamelfn,
                    const DIR_LABEL_LOCATION_STRUCT* srclblloc_p,
                    DIR_LABEL_LOCATION_STRUCT* dstlblloc_p,
                    DIR_DIRECTORY_LABEL_STRUCT* metdat_p)
{
  DIR_DIRECTORY_LABEL_STRUCT orgnamelfn_metdat;
  DIR_DIRECTORY_LABEL_STRUCT* orgmetdat_p;
  DIR_LABEL_LOCATION_STRUCT tmplblloc;
  DIR_LABEL_LOCATION_STRUCT* first_lfne_src_p=NULL;
  UINT8 short_file_name[FFS_MAX_SIZE_NAME_DOT_EXT]={0};
  UINT8 res = FFS_NO_ERRORS;
  UINT8 nr_found_entries = 0;
  UINT8 cntr = 0;
  UINT32 clusterNmb;

  orgmetdat_p = &orgnamelfn_metdat;
  if ((!newnamelfn) && (!rename_flg))
  {
    RFSFAT_GETBUF(sizeof(DIR_LABEL_LOCATION_STRUCT), &first_lfne_src_p);
    /* 
     * the orgname is lfn, the new name is exactly the same
     * MOVE 
     */
    //find out how many entries are involved
    //Watch out! the srclblloc_p will point at the first lfn entry after this function
    //and not at the 8+3 entry with which it started.
    res = DIR_find_lfn_of_sfn(srclblloc_p, &nr_found_entries);
    if(res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_find_lfn_of_sfn in DIR_lfn_move failed");
	  RFSFAT_FREE_BUF(first_lfne_src_p);
      return res;
    }
    //store the first lfn entry position in the src dir (needed for remove)
    CPY_LBL_TO_LBL(first_lfne_src_p, srclblloc_p);
    //create enough label space in new dir table
    res = ucGetNewLabelPos (dstlblloc_p, nr_found_entries);
    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetNewLabelPos in DIR_lfn_move failed");
	  RFSFAT_FREE_BUF(first_lfne_src_p);
      return res;
    }
    //copy label for label to new location      
    for(cntr = 0; cntr < nr_found_entries; cntr++)
    {
      //read orglbl
      GET_META_DATA(srclblloc_p, orgmetdat_p);
      //write org lbl at new location
      res = DIR_ucSetMetaData (dstlblloc_p, orgmetdat_p);
      if(res != FFS_NO_ERRORS)
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: Error copying labels");
		RFSFAT_FREE_BUF(first_lfne_src_p);
        return res;
      }
      dstlblloc_p += DIR_SIZE_META_ALL;
      srclblloc_p += DIR_SIZE_META_ALL;
    }
    //if dir 
      //the entry ".." must point at the new upper directory.   
    clusterNmb = DIR_CLUSTER_NMB_2_UINT32_NMB(orgmetdat_p);
    res = dotdot_new_parent(orgmetdat_p, srclblloc_p->mpt_idx, 
                            dstlblloc_p->uiFirstCl, 
                            clusterNmb);
    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: dotdot_new_parent in DIR_lfn_move failed");
	  RFSFAT_FREE_BUF(first_lfne_src_p);
      return res;
    }
    //remove orgnamelfn entries
    res = DIR_ucDeleteLabel_only(first_lfne_src_p);
    if(res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_ucDeleteLabel in DIR_lfn_move failed");
	  RFSFAT_FREE_BUF(first_lfne_src_p);
      return res;
    }
    RFSFAT_FREE_BUF(first_lfne_src_p);
  } else {
    /* 
     * both old and new name are LFN????
     * RENAME (and move?)
     */
    //get metadata of orgnamelfn
    GET_META_DATA(srclblloc_p, orgmetdat_p);
    
    //create enough label space in new dir table
    res = lfn_new_label_pos(dstlblloc_p);
    if(res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: lfn_new_label_pos in DIR_lfn_move failed");
      return res;
    }
    //(WHY? check if this is correct?)
    if(newnamelfn)
    { 
      //create and write new label in new path
/*      tmplblloc = *dstlblloc_p;   */
	 memcpy(&tmplblloc,dstlblloc_p,sizeof(DIR_LABEL_LOCATION_STRUCT)); 
      //first create sfn
      res = create_short_name(&tmplblloc, short_file_name);
      if (res != FFS_NO_ERRORS)
      {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_short_name in DIR_lfn_move failed");
        return res;
      }
    }
    //create label
    res = create_write_label(dstlblloc_p,metdat_p,short_file_name);
    if(res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_write_label in DIR_lfn_move failed");
      return res;
    }
	
    //replace label
    CPY_LBL_TO_LBL(DIR_META_DATA_P, orgmetdat_p);
    res = ucSetMetaData(dstlblloc_p, META_TAG_REPLACE);
    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData in DIR_lfn_move failed");
      return res;
    }
    //if dir && move
      //the entry ".." must point at the new upper directory.            
    clusterNmb = DIR_CLUSTER_NMB_2_UINT32_NMB(DIR_META_DATA_P);
    res = dotdot_new_parent(orgmetdat_p, srclblloc_p->mpt_idx, 
                            dstlblloc_p->uiFirstCl, clusterNmb);
    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: dotdot_new_parent in DIR_lfn_move failed");
      return res;
    }
    //remove orgnamelfn entries
    res = DIR_ucDeleteLabel_only(srclblloc_p);
    if(res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_ucDeleteLabel in DIR_lfn_move failed");
      return res;
    }
  } 
  return res;
}
/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucMove (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
      pstSrcLabelLocation,
      RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstDstLabelLocation)
/* PURPOSE      : Function moves file or directory label from the source
 *                directory table to the destination table.
 *
 * ARGUMENTS    : -pstSrcLabelLocation contains the partition Id. the directory
 *                 table and the offset within this table of the directory
 *                 label that must be moved.
 *                -pstDstLabelLocation contains the partition Id. and
 *                 first cluster number of the destination directory table.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation and pstMetaData may not be NULL.
 *
 * POSTCONDITION: After the operation was ended, the structure
 *                pstDstLabelLocation is pointing at, also contains the
 *                offset of the new/moved label.
 *
 * ERROR AND EXCEPTION HANDLING:
 *
 * NOTE         : Security : For security reasons (power failure) first the
 *                directory label in the source table is marked as if empty.
 *                Later, this label is copied to the destination table. In
 *                this way, it's impossible to have two different labels,
 *                pointing at the same file data / directory table.
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT stOriginalMetaData;
  DIR_DIRECTORY_LABEL_STRUCT* origmetdat_p;
  UINT32 clusterNmb;

  origmetdat_p = &stOriginalMetaData;
  
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstSrcLabelLocation != NULL)
            && (pstDstLabelLocation != NULL)),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* Get original directory label, that must be moved.                        */
  /****************************************************************************/
  GET_META_DATA(pstSrcLabelLocation, origmetdat_p);

  /****************************************************************************/
  /* Mark original entry emtpy.                                               */
  /****************************************************************************/
  DIR_META_DATA.aucName[0] = META_MARK_EMPTY;

  ucResult = ucSetMetaData (pstSrcLabelLocation, META_TAG_NAME);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
    return ucResult;
  }

  /****************************************************************************/
  /* In case the meta data of a directory must be moved, the entry ".." must  */
  /* point at the new upper directory.                                        */
  /****************************************************************************/
  clusterNmb = DIR_CLUSTER_NMB_2_UINT32_NMB(DIR_META_DATA_P);
  ucResult = dotdot_new_parent(&stOriginalMetaData, pstSrcLabelLocation->mpt_idx, 
                    pstDstLabelLocation->uiFirstCl, clusterNmb);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
    return ucResult;
  }
  /****************************************************************************/
  /* Write entry to destination directory table.                              */
  /* First determine free entry, then copy data.                              */
  /****************************************************************************/

  ucResult = lfn_new_label_pos (pstDstLabelLocation);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: lfn_new_label_pos failed",
             RV_TRACE_LEVEL_ERROR);
    return ucResult;
  }
  return DIR_ucSetMetaData (pstDstLabelLocation, &stOriginalMetaData);
}

  /****************************************************************************/
  /*      Implementation of Local (= static) Function                         */
  /****************************************************************************/
static char is_root_cluster_last(RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
 {
  if(pstLabelLocation->uiFirstCl== 0)
  	{
  	   if(MMA_FS_TYPE(pstLabelLocation->mpt_idx) != GBI_FAT32_LBA)
  	   	{
  	   	   /* Now this could be FAT16/12  */
		   /* If you are the end of the directory entry then dont write */
		   /* determine the cluster size */
		   if (pstLabelLocation->tLblPos == (MMA_BOOT_SECTOR[pstLabelLocation->mpt_idx].uiMaxRootEntries * META_DATA_SIZE))
		   	{
		   	   /* Now, we are the end of root cluster */
			   return 1; /* YES */
		   	}
		   
  	   	}
  	}
  return 0;	/* NO, its not End of root cluster */
 }



/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
ucGetNewLabelPos (DIR_LABEL_LOCATION_STRUCT *pstLabelLocation, UINT8 nr_adj_lbl)
/* PURPOSE      : This function locates a free label entry in the directory
 *                table. A directory label with the first byte value 0xE5 is a
 *                free label. If there are no free labels then a new label will
 *                be appended to the directory table, the first byte of the
 *                end-label has value 0x00. After appending a label the
 *                end-label will be written after the appended label to close
 *                the directory table. In case of a LFN entry, more than 1 adjecent 
 *                labels are needed with a minimum of 2. In case of LFN the label location
 *                In which the first lfn entry must be written shall be returned               
 *
 * ARGUMENTS    : pstLabelLocation: Indicates the first cluster number of the
 *                directory table, in which the free entry should be found.
 *                nr_adj_lbl: contains number of labels that are to be found adjecent 
 *                to each other   
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *
 * POSTCONDITION: None.
 *
 * ERROR AND EXCEPTION HANDLING:
 *
 * NOTE         :
 ******************************************************************************/
{
  UINT8 ucReturn;
  FFS_FILE_LENGTH_TYPE startpos;
  UINT8 adj_lbl_cnt = 0;
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* Start requesting meta data of the first entry.                           */
  /****************************************************************************/
  pstLabelLocation->tLblPos = 0;
  startpos = pstLabelLocation->tLblPos;
  do
  {
    ucReturn = ucGetMetaData (pstLabelLocation);

    if ((ucReturn != FFS_NO_ERRORS))
    {
      /* Empty entry found, or error occurred.                                 */
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData failed");
      return ucReturn;
    } else if (EMPTY_LABEL) {
    
      //added for LFN
      if(adj_lbl_cnt == 0)
      {
        //store first label pos of sequence
        startpos = pstLabelLocation->tLblPos;        
      }
      adj_lbl_cnt++;
      if(adj_lbl_cnt == nr_adj_lbl)
      {
        //the requested nr of labels  is found
        pstLabelLocation->tLblPos = startpos;
        return ucReturn;
      } else {
        //there are more labels needed
        NEXT_LABEL(pstLabelLocation->tLblPos);
      }
    }
    else if (LAST_LABEL)
    {
      //create new LAST label
      adj_lbl_cnt = 0;
      startpos = pstLabelLocation->tLblPos;        
      do 
      {
        NEXT_LABEL(pstLabelLocation->tLblPos);
        adj_lbl_cnt++;
        if(adj_lbl_cnt == nr_adj_lbl)
        {
          //the last enty is reached so set it to META_MARK_LAST
          DIR_META_DATA.aucName[0] = META_MARK_LAST;
       /* If the file system is FAT16/12, and you writing 511th entry then no need to write 
         The last on as LAST ENTRY  */
		if((pstLabelLocation->uiFirstCl== 0) &&(is_root_cluster_last(pstLabelLocation)))
    	 	{
    	 	   pstLabelLocation->tLblPos = startpos;
    	 	   return FFS_NO_ERRORS;
    	 	}
        } else {
          //the next entry in the sequence but not the last -> META_MARK_EMPTY
          DIR_META_DATA.aucName[0] = META_MARK_EMPTY;
        }
        /* (stMetaData.aucName[0] = META_MARK_LAST)                           */
        ucReturn = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
      }while(adj_lbl_cnt != nr_adj_lbl);
      //return start position
      pstLabelLocation->tLblPos = startpos;
      return ucReturn;
    }
    else
    {
      //reset adj_lbl_cnt
      adj_lbl_cnt = 0;
      NEXT_LABEL(pstLabelLocation->tLblPos);
    }
  }
  /*lint -save -e506 */
  while (1);
  /*lint -restore */
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
static UINT8
ucGetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation)
/* PURPOSE      : Reads a selected directory label from a partition, and copies
 *                the values into the global structure stMetaData.
 *
 * ARGUMENTS    : pstLabelLocation: used to select directory label.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : -
 *
 ******************************************************************************/
{
  CLM_READ_WRITE_STRUCT stReadWritePos;
  UINT8 rtval=FFS_NO_ERRORS;

  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),
             return DIR_RV_INVALID_PARAM);


  stReadWritePos.pucBuffer = (UINT8 *) (&DIR_META_DATA);  /* buffer assignment */
  stReadWritePos.tNrBytes = DIR_SIZE_META_ALL;
  stReadWritePos.uiFirstClusterNr = pstLabelLocation->uiFirstCl;
  stReadWritePos.tOffset = pstLabelLocation->tLblPos;
  stReadWritePos.mpt_idx = pstLabelLocation->mpt_idx;


  if((pstLabelLocation->uiFirstCl== 0) &&(is_root_cluster_last(pstLabelLocation)))
  	  	{
  	  	        /* Its other than FAT32 file system, we are reading the out of the
				   FAT root cluster AREA */
     	   	   /* Now, Return with LAST ENTRY */
			   memset(stReadWritePos.pucBuffer,META_MARK_LAST,DIR_SIZE_META_ALL);	   
			   return  FFS_NO_ERRORS;
		   
  	  	}

  /* Read byte and return.                                                    */
  rtval= CLM_ucRead (&stReadWritePos);

  if(rtval == CLM_RET_LIST_ERROR)
  	{
  	   /* This is really a typical case , during read we might see the last cluster,
  	      it means that its we are the end of the cluster chain, so we return with
  	      FFS_NO_ERROS , with last label found */
     memset(stReadWritePos.pucBuffer,META_MARK_LAST,DIR_SIZE_META_ALL);	   
	 rtval = FFS_NO_ERRORS;
  	}
  return rtval;
}

/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
ucSetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
         pstLabelLocation, UINT8 ucSelection)
/* PURPOSE      : This function replaces selected fields of the original
 *                directory label with values out of the global structure
 *                stMetaData, and stores this at the selected partition.
 *
 * ARGUMENTS    : pstLabelLocation: indicates the table entry.
 *                ucSelection: Selects fields in meta data to be replaced.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *                                           .
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : -
 *
 ******************************************************************************/
{
  UINT8 ucResult;
  DIR_DIRECTORY_LABEL_STRUCT stMetaDataCopy;
  DIR_DIRECTORY_LABEL_STRUCT* metdat_p;
  CLM_READ_WRITE_STRUCT stReadWritePos;

  metdat_p = &stMetaDataCopy;

  CPY_LBL_TO_LBL(metdat_p, DIR_META_DATA_P);
  /* In case not all directory label fields must be overwritten, the current */
  /* label will be read from the partition, changes will be made, and the whole */
  /* label will be written in the end.                                       */
  if (ucSelection != META_TAG_ALL)
  {
    ucResult = ucGetMetaData (pstLabelLocation);
    if (ucResult != FFS_NO_ERRORS)
    {
      return ucResult;
    }
  }

   /* either name of ext , update the lcase */
  if((ucSelection & META_TAG_NAME) || (ucSelection & META_TAG_EXT))
  	{
  	     DIR_META_DATA.lcase = stMetaDataCopy.lcase;
  	}
  if (ucSelection & META_TAG_NAME)
  {
    (void) rfsfat_vpMemcpy ((UINT8 *) (DIR_META_DATA.aucName),
                (UINT8 *) (stMetaDataCopy.aucName),
                DIR_SIZE_META_NAME);
  }

  if (ucSelection & META_TAG_EXT)
  {
    (void) rfsfat_vpMemcpy ((UINT8 *) (DIR_META_DATA.aucExtension),
                (UINT8 *) (stMetaDataCopy.aucExtension),
                DIR_SIZE_META_EXT);
  }

  if (ucSelection & META_TAG_ATTR)
  {
    DIR_META_DATA.ucAttribute = stMetaDataCopy.ucAttribute;
  }

  if (ucSelection & META_TAG_RESERVED)
  {
    DIR_META_DATA.crtTimeTenth = stMetaDataCopy.crtTimeTenth;
  }

  if (ucSelection & META_TAG_TIME)
  {
    DIR_META_DATA.uiLastChangedTime = stMetaDataCopy.uiLastChangedTime;
	DIR_META_DATA.crtTime = stMetaDataCopy.crtTime;
  }

  if (ucSelection & META_TAG_DATE)
  {
    DIR_META_DATA.uiLastChangedDate = stMetaDataCopy.uiLastChangedDate;
	DIR_META_DATA.crtDate = stMetaDataCopy.crtDate;
	DIR_META_DATA.lstAccDate = stMetaDataCopy.lstAccDate;
  }

  if (ucSelection & META_TAG_CLUSTER)
  {
    DIR_META_DATA.uiFirstClusterNumberHigh =
                stMetaDataCopy.uiFirstClusterNumberHigh;
    DIR_META_DATA.uiFirstClusterNumber =
                stMetaDataCopy.uiFirstClusterNumber;
  }

  if (ucSelection & META_TAG_SIZE)
  {
    DIR_META_DATA.ulFileSize = stMetaDataCopy.ulFileSize;
  }

  /* Copy value.                                                              */
  stReadWritePos.pucBuffer = (UINT8 *) (&DIR_META_DATA);
  stReadWritePos.tNrBytes = DIR_SIZE_META_ALL;
  stReadWritePos.uiFirstClusterNr = pstLabelLocation->uiFirstCl;
  stReadWritePos.tOffset = pstLabelLocation->tLblPos;
  stReadWritePos.mpt_idx = pstLabelLocation->mpt_idx;

  /* Read byte and return.                                                    */
  ucResult = CLM_ucWrite (&stReadWritePos);
  CPY_LBL_TO_LBL(metdat_p, DIR_META_DATA_P);
  return ucResult;
}
/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/

UINT8
DIR_ucClearRoot_ORG (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)		// Andrew
         pstLabelLocation, UINT8 * name)
/* PURPOSE      : This function clears the root directory of the volume.
 *
 * ARGUMENTS    : ucVolume : Volume Id. of the volume which root directory
 *                must be marked empty.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *
 * POSTCONDITION: Root directory is cleared. None of the files or directories
 *                are accessible in a valid way.
 *                The buffer, pstLabelLocation is pointing at, indicates the
 *                first cluster number and the offset of the first entry of
 *                the root directory of the given volume.
.*
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : Clearing the root directory does not mark the different
 *                used clusters as 'empty'. So for this, be sure all clusters
 *                are released in some way. (i.e. use FAM_bCreateFatTables()).
 ******************************************************************************/
{
  UINT8 status;
  UINT8 mpt_idx = pstLabelLocation->mpt_idx;
  UINT32  uiClusterNr = MMA_F32_ROOT_CLUSTER_NMB;

  RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: DIR_ucClearRoot entered");

  //FAT32 only. Reserve the ROOT DIR cluster
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
  {
    status = FAM_ucCreateNewChain( mpt_idx, &uiClusterNr);
    if (status != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAT32 ROOT DIR reservation failed");
      return status;
    }
  }
  
  (void) rfsfat_vpMemset ((UINT8 *) & DIR_META_DATA, META_MARK_LAST,
              sizeof (DIR_DIRECTORY_LABEL_STRUCT));

  //The first entry will hold the partition name!
  strcpy ((char *) &DIR_META_DATA.aucName[0], (const char *) name);
  DIR_META_DATA.ucAttribute = DIR_PART_ATTR;

  pstLabelLocation->uiFirstCl = DIR_CLUSTER_NR_ROOT_DIRECTORY;
  pstLabelLocation->tLblPos = DIR_OFFSET_FIRST_ENTRY;

  status = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
  if (status != FFS_NO_ERRORS)
  {

    //write first label containing partition name failed
    RFSFAT_SEND_TRACE_ERROR ("RFSFAT: writing partition name failed");
    return status;
  }
 //FAT32 only. Reserve the ROOT DIR cluster
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
  {
    status = FAM_ucCreateNewChain( mpt_idx, &uiClusterNr);
    if (status != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAT32 ROOT DIR reservation failed");
      return status;
    }
  }
  /****************************************************************************/
  /* Set the first byte of the second entry of the medium to 'last entry'.     */
  /* After this, the second entry is the last entry of the root directory,     */
  /* so the root will be empty.                                               */
  /****************************************************************************/
  /* Copy name and extension "".                                              */
  (void) rfsfat_vpMemset ((UINT8 *) & DIR_META_DATA, META_MARK_LAST,
              sizeof (DIR_DIRECTORY_LABEL_STRUCT));

  pstLabelLocation->uiFirstCl = DIR_CLUSTER_NR_ROOT_DIRECTORY;
  pstLabelLocation->tLblPos = (DIR_OFFSET_FIRST_ENTRY + DIR_SIZE_META_ALL);

  status = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
  if (status != FFS_NO_ERRORS)
  {

    //write first label containing partition name failed
    RFSFAT_SEND_TRACE_ERROR ("RFSFAT: writing partition name failed");
  }
  flush_write_blk ();
  return status;
}
/* This has the bug: only for FAT32. Dont use this function  */

UINT8 DIR_ucClearRoot_new (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)pstLabelLocation, UINT8 *name)
{
  MMA_RW_STRUCT stWrite={0,0,NULL};    // Structure with write information
  FFS_FILE_LENGTH_TYPE tOffset;
  UINT8 mpt_idx = pstLabelLocation->mpt_idx;
  UINT32 uiRootDirSec;    // Sectors of the root directory
  UINT32 i, j;
  UINT32 uiRootDirClusterNr;
  UINT8 *ucRootDirSec_p=NULL;
  UINT8 status;

  //Root Dir offset, The part that is the same for FAT16 and FAT32
  tOffset = ((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors +
        (MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables * MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT)) *
         MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);

  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
  {
    uiRootDirClusterNr = MMA_BOOT_SECTOR[mpt_idx].ulFAT32RootDirCluster;
    //Root dir in FAT32 is normal data and its cluster must be reserved like other data
    status = FAM_ucCreateNewChain( mpt_idx, &uiRootDirClusterNr);
    if (status != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAT32 ROOT DIR reservation failed");
      return status;
    }
    uiRootDirSec = MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster;
    tOffset += ((uiRootDirClusterNr - FAM_START_DATA_CLU) *
                (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster *
                 MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector));
  }
  else
  {
    uiRootDirSec = (MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries * DIR_SIZE_META_ALL)
                  / MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;
  }
  
  RFSFAT_GETBUF(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector, &ucRootDirSec_p);

  for (i = 0; i < uiRootDirSec; i++)
  {
    //set Fat table to empty cluster
    rfsfat_vpMemset(ucRootDirSec_p, 0, (UINT16)MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
    if (i == 0)
    { // partition name
      j = 0;
      while ((name[j] != 0) && (j < 11))
      {
        ucRootDirSec_p[j] = name[j];
        j++;
      }
      if (j != 0)
      {
        for (; j < 11; j++)
          ucRootDirSec_p[j] = 0x20;
        ucRootDirSec_p[11] = DIR_PART_ATTR; //Directory Volume ID attribute
      }
    }

    stWrite.pucBuffer = ucRootDirSec_p;
    stWrite.tNrBytes = MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;
    stWrite.tOffset = tOffset + (i * MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);

    if (MMA_ucDirectWrite (mpt_idx, &stWrite) != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR ("RFSFAT: MMA_ucDirectWrite failed");
      RFSFAT_FREE_BUF(ucRootDirSec_p);
      return FAM_RET_FAT_WRITE_ERROR;
    }
  }

  RFSFAT_FREE_BUF(ucRootDirSec_p);
  flush_write_blk ();
  return FFS_NO_ERRORS;
}
/**MH**Copyright by ICT Embedded BV All rights reserved ***********************/
/* This function is obsoloted */
UINT8
DIR_ucClearRoot (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
         pstLabelLocation, UINT8 * name)
/* PURPOSE      : This function clears the root directory of the volume.
 *
 * ARGUMENTS    : ucVolume : Volume Id. of the volume which root directory
 *                must be marked empty.
 *
 * RETURNS      : FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *
 * POSTCONDITION: Root directory is cleared. None of the files or directories
 *                are accessible in a valid way.
 *                The buffer, pstLabelLocation is pointing at, indicates the
 *                first cluster number and the offset of the first entry of
 *                the root directory of the given volume.
.*
 * ERROR AND EXCEPTION HANDLING: None.
 *
 * NOTE         : Clearing the root directory does not mark the different
 *                used clusters as 'empty'. So for this, be sure all clusters
 *                are released in some way. (i.e. use FAM_bCreateFatTables()).
 ******************************************************************************/
{
  UINT8 mpt_idx = pstLabelLocation->mpt_idx;
  UINT32  uiClusterNr = MMA_F32_ROOT_CLUSTER_NMB;
   UINT8 				  ucRetVal;
  UINT16                  uiCnt;
  UINT16                  uiNmbDirEntriesInCluster;
  MMA_RW_STRUCT           stDirectRW={0,0,NULL};


  
  RFSFAT_SEND_TRACE_DEBUG_LOW ("RFSFAT: DIR_ucClearRoot entered");

//FAT32 only. Reserve the ROOT DIR cluster
  if (MMA_FS_TYPE(mpt_idx) == GBI_FAT32_LBA)
  {
    ucRetVal = FAM_ucCreateNewChain( mpt_idx, &uiClusterNr);
    if (ucRetVal != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAT ROOT DIR reservation failed");
      return ucRetVal;
    }
    uiCnt = (UINT16) (MMA_BOOT_SECTOR[mpt_idx].ucSectorPerCluster * 
                      MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
    uiNmbDirEntriesInCluster = uiCnt / DIR_SIZE_META_ALL;
	uiClusterNr = MMA_F32_ROOT_CLUSTER_NMB;
  }
  else
  	{
  	  /* other than FAT32 partition, clear all root entries */
	  uiNmbDirEntriesInCluster = MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries; 
	  uiClusterNr = 0;   /* No cluster Number */
  	}
 

    /* Copy label location of directory.                                        */  
    RFSFAT_GETBUF(DIR_SIZE_META_ALL, &stDirectRW.pucBuffer);
    rfsfat_vpMemset(stDirectRW.pucBuffer, DEFAULT_DIR_FIRST_CLUSTER, DIR_SIZE_META_ALL);
    stDirectRW.tNrBytes = DIR_SIZE_META_ALL;
    stDirectRW.tOffset = 0;
    if (!(bClusterOffsetInVol (mpt_idx, uiClusterNr,((FFS_FILE_LENGTH_TYPE *) &(stDirectRW.tOffset)))))
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
    flush_write_blk ();	
  	
 
    // once format is done, set the volume label.	 
    if (E_FFS_ucDiskSetVolumeLabel(mpt_idx, (const char *)name) != FFS_NO_ERRORS)
    {
        RFSFAT_SEND_TRACE_ERROR("RFSFAT:  Failed to set label during format");   
    }  
  
  return FFS_NO_ERRORS;
}



// Andrew	/////////////////////////////////

#if 0
UINT8 DIR_ucClearRoot (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)pstLabelLocation, UINT8 *name)
{
	MMA_RW_STRUCT stWrite;	// Structure with write information
	UINT8 mpt_idx = pstLabelLocation->mpt_idx;
	UINT32 uiRootDirSec;		// Sectors of the root directory
	UINT32 i, j;
	UINT8 *ucRootDirSec_p;


	uiRootDirSec = (MMA_BOOT_SECTOR[mpt_idx].uiMaxRootEntries * DIR_SIZE_META_ALL)
		/ MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;

	RFSFAT_GETBUF(MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector, &ucRootDirSec_p);

	for (i = 0; i < uiRootDirSec; i++)
	{
		//set Fat table to empty cluster
		rfsfat_vpMemset((void * const)ucRootDirSec_p, 0, (UINT16)MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);
		if (i == 0)
		{	// partition name
			j = 0;
			while ((name[j] != 0) && (j < 11))
			{
				ucRootDirSec_p[j] = name[j];
				j++;
			}
			if (j != 0)
			{
				for (; j < 11; j++)
					ucRootDirSec_p[j] = 0x20;
				ucRootDirSec_p[11] = DIR_PART_ATTR;
			}
		}

		stWrite.pucBuffer = ucRootDirSec_p;
		stWrite.tNrBytes = MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector;
		stWrite.tOffset = ((MMA_BOOT_SECTOR[mpt_idx].uiReservedSectors +
			(MMA_BOOT_SECTOR[mpt_idx].ucNrFatTables * MMA_BOOT_SECTOR[mpt_idx].uiNrSectorsFAT))
			* MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector)
			+ (i * MMA_BOOT_SECTOR[mpt_idx].uiBytesPerSector);

		if (MMA_ucDirectWrite (mpt_idx, &stWrite) != FFS_NO_ERRORS)
		{
			RFSFAT_SEND_TRACE_ERROR ("RFSFAT: FAM_RET_FAT_WRITE_ERROR");
			RFSFAT_FREE_BUF(ucRootDirSec_p);
			return FAM_RET_FAT_WRITE_ERROR;
		}
	}

	RFSFAT_FREE_BUF(ucRootDirSec_p);
	flush_write_blk ();
	return FFS_NO_ERRORS;
}
#endif
// Andrew	/////////////////////////////////

//the labelloc_p points at the 8+3 label when this function returns
//the DIR_META_DATA struct contains info on that 8+3 label as well
UINT8 DIR_read_lfn_from_label(DIR_LABEL_LOCATION_STRUCT* const labelloc_p, DIR_LFN_RET* retval)
{
  UINT8  nr_lfn_entries = 0;
  UINT8  str_cntr;
  UINT32 startpos;  
  UINT8  result;
  UINT8  shortname[12];
  UINT8  crc_first_lfne;
  UINT8  crc_sfn;
  LFN_LABEL_STRUCT* lfne_p;
  LFN_LABEL_STRUCT lfne;
  UINT8  lfne_cntr;
  
  *retval = NO_ERRORS;
  //determine how many entries should be read to compose the lfn
  nr_lfn_entries = NR_LFN_ENTRIES;  
  //fencepost error--> 13 or 26 or.. entries so count from 0 till 25 
  str_cntr = (((nr_lfn_entries * BYTES_IN_LFNE) / 2) - 1);
  startpos = labelloc_p->tLblPos;

  lfne_p = &lfne;
  //init lfne 
  rfsfat_vpMemset(lfne_p, 0x0000, sizeof(LFN_LABEL_STRUCT));
  convert_from_lfn_sfn(lfne_p, DIR_META_DATA_P);
  //store crc before getting 8+3 label
  crc_first_lfne = lfne_p->lfn_crc;

  //create long enough buffer to store lfn
  /* Additional 2 buytes are required to store the uint16 NULL character */
  RFSFAT_GETBUF(((BYTES_IN_LFNE * nr_lfn_entries)+2), (&(RFSFAT_LFN_FROM_LABEL)));
  //default to 0x0000
  rfsfat_vpMemset(RFSFAT_LFN_FROM_LABEL, 0x0000, 
                  (UINT16)((BYTES_IN_LFNE * nr_lfn_entries)+2));

  //copy string part from lfn entry 
  if(copy_lfn_strpart(lfne_p, &str_cntr) != NO_ERRORS)
  {
    *retval = COPY_ERROR; 
    return FFS_NO_ERRORS;
  }
  //8+3 labelpos
  labelloc_p->tLblPos += (DIR_SIZE_META_ALL * nr_lfn_entries);
  if (VALID_NEXT_LABEL(labelloc_p->tLblPos))
  {
    RFSFAT_SEND_TRACE_ERROR ("RFSFAT: DIR_RV_DIRTABLE_FULL");
    return DIR_RV_DIRTABLE_FULL;
  }
  //read the 8+3 label to calc crc
  result = ucGetMetaData (labelloc_p);

  if (result != FFS_NO_ERRORS)
  {
    /* Error reading, so stop.                                              */
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData failed");
    return result;
  } else if (LAST_LABEL) {
  
    RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: DIR_RV_DIR_NOT_FOUND");
    return DIR_RV_DIR_NOT_FOUND;
  } else if (!EMPTY_LABEL) {
    //copy & validate name from label
    DIR_cpy_lbl_str(shortname, DIR_META_DATA.aucName, DIR_META_DATA.aucExtension);
  } else {
    *retval = LFN_EMPTY_LABEL;
    return FFS_NO_ERRORS;
  }
  //calculate and store crc from short file name
  crc_sfn = DIR_crc_calculate(shortname);
  //verify the crc
  if(crc_sfn != crc_first_lfne)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: crc LFN doesn't match short file name crc");
    *retval = WRONG_CRC;
	/* set the current position to last LFN entry */
	labelloc_p->tLblPos = startpos + (DIR_SIZE_META_ALL * (nr_lfn_entries));
    return FFS_NO_ERRORS;
  }
  //restore to first lfn entry
  labelloc_p->tLblPos = startpos;
  
  //read the next entries
  for(lfne_cntr = 0; lfne_cntr < (nr_lfn_entries - 1); lfne_cntr++)
  {
    NEXT_LABEL(labelloc_p->tLblPos);
    
    //read the next lfn label 
    result = ucGetMetaData (labelloc_p);
    if (result != FFS_NO_ERRORS)
    {
      /* Error reading, so stop.                                              */
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData failed");
      return result;
    } else if (LAST_LABEL) {
    
      RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: DIR_RV_DIR_NOT_FOUND");
      return DIR_RV_DIR_NOT_FOUND;
    } else if (!EMPTY_LABEL) {  
      //determine if label belongs to LFN
      rfsfat_vpMemset(lfne_p, 0x0000, sizeof(LFN_LABEL_STRUCT));
      convert_from_lfn_sfn(lfne_p, DIR_META_DATA_P);
      if(RECOG_LFN)
      {
        crc_first_lfne = lfne_p->lfn_crc;
        if(crc_sfn != crc_first_lfne)
        {
          RFSFAT_SEND_TRACE_ERROR("RFSFAT: crc LFN doesn't match short file name crc");
		  /* set the current position to last LFN entry */
		  labelloc_p->tLblPos = startpos + (DIR_SIZE_META_ALL * (nr_lfn_entries));
          *retval = WRONG_CRC;
          return FFS_NO_ERRORS;
        } else {
           //copy string part from lfn entry 
          if(copy_lfn_strpart(lfne_p, &str_cntr) != NO_ERRORS)
          {
            *retval = COPY_ERROR;
            return FFS_NO_ERRORS;
          }
        }
      } else {
        *retval = NOT_A_VALID_LFN_LABEL;
        return FFS_NO_ERRORS;
      	}
    }
  } 
  //okay all partial strings from the various lfne are copied into the
  //LFN string ( RFSFAT_LFN_FROM_LABEL ) the string counter should be 0 

  //sting parts successfully copied
  //get 8+3 label that belongs with this LFN one more time, must be returned 
  //to caller for admin (first clusternr etc)
  /* set the position to SFN Entry */
  labelloc_p->tLblPos = startpos + (DIR_SIZE_META_ALL * nr_lfn_entries);
  result = ucGetMetaData (labelloc_p);
  if (result != FFS_NO_ERRORS)
  {
    /* Error reading, so stop.                                              */
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: getting 8+3 label that belongs with LFN failed");
    return result;
  }
  *retval = NO_ERRORS;
  return FFS_NO_ERRORS;
}

DIR_LFN_RET copy_lfn_strpart(LFN_LABEL_STRUCT* const lfne_p, UINT8 * const str_pos_p)
{
  UINT8 cnt = 0;

    RFSFAT_SEND_TRACE_DEBUG_LOW("copy_lfn_strpart ... start\n\r");
  cnt = LFN_2CHR;
  while((cnt != 0) && ((*str_pos_p + 1) != 0))
  {
    RFSFAT_LFN_FROM_LABEL[*str_pos_p] = lfne_p->lfn_2char_str[cnt-1];
    (*str_pos_p)--;
    cnt--;
  }        
  cnt = LFN_6CHR;
  while((cnt != 0) && ((*str_pos_p + 1) != 0))
  {
    RFSFAT_LFN_FROM_LABEL[*str_pos_p] = lfne_p->lfn_6char_str[cnt-1];
    (*str_pos_p)--;
    cnt--;
  }
  cnt = LFN_5CHR;
  while((cnt != 0) && ((*str_pos_p + 1) != 0))
  {
    RFSFAT_LFN_FROM_LABEL[*str_pos_p] = lfne_p->lfn_5char_str[cnt-1];
    (*str_pos_p)--;
    cnt--;
  }   
    RFSFAT_SEND_TRACE_DEBUG_LOW("copy_lfn_strpart ... End\n\r");
  if((cnt != 0) && ((*str_pos_p + 1) == 0))
  {
    return COPY_ERROR;
  } else {
    return NO_ERRORS;
  }
}

UINT16 copy_strpart_lfn(LFN_LABEL_STRUCT* const lfne_p, UINT8 * const str_pos_p, UINT16 str_size)
{
  UINT8 cnt = 0;
  
  while((cnt != LFN_5CHR) && (*str_pos_p < str_size))
  {
    lfne_p->lfn_5char_str[cnt] = RFSFAT_LFN_FROM_LABEL[*str_pos_p];
    (*str_pos_p)++;
    cnt++;
  }
  cnt = 0;
  while((cnt != LFN_6CHR) && (*str_pos_p < str_size))
  {
    lfne_p->lfn_6char_str[cnt] = RFSFAT_LFN_FROM_LABEL[*str_pos_p];
    (*str_pos_p)++;
    cnt++;
  }
  cnt = 0;
  while((cnt != LFN_2CHR) && (*str_pos_p < str_size))
  {
    lfne_p->lfn_2char_str[cnt] = RFSFAT_LFN_FROM_LABEL[*str_pos_p];
    (*str_pos_p)++;
    cnt++;
  }
  return RFSFAT_OK;
}

UINT8 DIR_crc_calculate(UINT8* shortname)
{
  UINT8 namelength;
  UINT8 crc = 0;

  for(namelength = 11; namelength != 0; namelength--)
  {
    crc = ((crc & 1) ? 0x80 : 0) + (crc >> 1) + *(shortname++);
  }
  return crc;
}

BOOL lfn_compare (const UINT8* const name1, const UINT16* const name2)
{
  UINT32 i;
  BOOL match = name1 != NULL && name2 != NULL;
  if((name1!=NULL)&&(name2!=NULL))
  	{

    for (i = 0; match && name1[i] != 0 && name2[i] != 0; i++)
      {
        match = (name2[i] <= 0xff) && (toupper (name1[i]) == toupper (name2[i]));
      }
    return (match && (name1[i] == 0) && (name2[i]==0));
}

	return match;
}

BOOL sfn_compare (const UINT8* const name1, const UINT8* const name2)
{
  UINT32 i,j;
  BOOL match = name1 != NULL && name2 != NULL;

  //the name1 last spaces can be filles with whitespaces if      
  i = 0;
	if((name1!=NULL)&&(name2!=NULL))
	{
  while(match && name1[i] != 0 && name2[i] != 0)
  {
    //exception
    if(match && (name1[i] != 0 && 
      (i >= (DIR_SIZE_META_NAME + DIR_SIZE_META_EXT))) && 
      name2[i] == 0)
    { 
      return TRUE;
    }
    match = (toupper (name1[i]) == toupper (name2[i]));
    if(!match)
    {
      //they are not the same if
      break;
    }
    i++;   
    }
  //check if remainder of name2 only contains spaces
  if((name1[i] == 0) && (name2[i] == ASCII_SPACE))
  {
  for(j=i; j < strlen((char*)name2); j++)
  {
    if(name2[j] != ASCII_SPACE)
    {
      break; //not the same so exit.
    }
  }
  if(j == strlen((char*)name2))
  {
    return TRUE; //remainer contains only spaces so they are the same.
  }
  }

  return match && ((name1[i] == 0) || (name1[i] == ASCII_SPACE)) && name2[i] == 0;
}
 	 return match;
}

void DIR_cpy_lbl_str(UINT8* shortname, const UINT8* orgname, const UINT8* orgext)   
{
  UINT16 cnt; 
  UINT16 temp_str_cntr = 0; 
  
  for(cnt = 0; cnt < DIR_SIZE_META_NAME; cnt++) 
  { 
    shortname[temp_str_cntr] = orgname[cnt]; 
    temp_str_cntr++; 
  } 
  for(cnt = 0; cnt < DIR_SIZE_META_EXT; cnt++) 
  { 
    shortname[temp_str_cntr] = orgext[cnt]; 
    temp_str_cntr++;
  }
  shortname[temp_str_cntr] = EOSTR;
}

UINT8 lfn_new_label_pos(DIR_LABEL_LOCATION_STRUCT* tmplabelloc_p)
{
  UINT8 res;
  UINT8 nr_lfne = 0;

  
  //check if LFN 
  if(RFSFAT_LFN_FLAG)
  {    
    //lfn!!!
    //calculate nr of entries (min 2)
    NR_LFN_LABELS(nr_lfne);  
  } else {
    //8+3 label
    nr_lfne = 1;
  }
  res = ucGetNewLabelPos (tmplabelloc_p, nr_lfne);
  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetNewLabelPos failed");
    return res;
  }
  return FFS_NO_ERRORS;
}

//the content that parentdirloc_p points to can be changed after this function is called, it will point to
//the label that contains the new created shortname IF it exist in the directory to place it.
//(if newname is unique it wont be changed)
//sfn will point to the new created sfn after calling this function
UINT8 create_short_name(const DIR_LABEL_LOCATION_STRUCT* parentdirloc_p, UINT8* sfn_p)
{
  UINT8* temp_lfn=NULL;
  UINT8* shortname;
  UINT8* tmpstr = NULL;
  UINT8 cnt = 0;
  UINT8 counter=0;
  UINT8 i,j;
  UINT16 size = 0;
  DIR_DIRECTORY_LABEL_STRUCT stMetaData;
  UINT8 res = FFS_NO_ERRORS;
  BOOL found = FALSE;
  BOOL nodots = FALSE;
  DIR_LABEL_LOCATION_STRUCT tmploc;
  BOOL tmp_flag;

#if (RFSFAT_SFN_OPT != 1)
  
  UINT8 num_tail[MAX_TAIL];
  UINT8 tailmask = 0;
#else
    UINT8 tail_num = 0;
    UINT32 count=0; 
  
#endif  

  RFSFAT_SEND_TRACE ("RFSFAT: create short name entered",RV_TRACE_LEVEL_DEBUG_LOW);
  
  tmploc = *parentdirloc_p;
  //start searching at offset null
  tmploc.tLblPos = 0;

#if (RFSFAT_SFN_OPT != 1)
  //init num_tail
  for(cnt = 0; cnt < MAX_TAIL; cnt++)
  {
    num_tail[cnt] = 0;
  }

#endif  

  size = (strlen((const char*)RFSFAT_LFN) + 1);
  //create short name buffer and a temp buf long enough for long name
  RFSFAT_GETBUF(size, &temp_lfn);
  
  shortname = sfn_p;
  rfsfat_ucStrncpy(temp_lfn, RFSFAT_LFN, size-1);
  // all chars to upper case
  string_toupper(temp_lfn);
  //remove all spaces and leading dots
  RM_CHAR(temp_lfn, ASCII_SPACE);
  //remove all dots from string except the last one
  tmpstr = (UINT8*)strrchr((const char*)temp_lfn, ASCII_DOT);
  if(tmpstr != NULL)
  {
    *tmpstr = (UINT8)EOSTR;
	/* So Except last Dot , remove all other inner dots */
    RM_CHAR(temp_lfn, ASCII_DOT);

	/* retain the last dot */
    *tmpstr = (UINT8)ASCII_DOT;
	nodots = FALSE;
  } else {
    nodots = TRUE;
  }
  //when there was only 1 dot in the name the .ext doesnt need to be extended 
  //anymore (no chars have been removed!) if temp_lfn still contains a . after 
  //this action this was the case so no strcat is needed
  if(((UINT8*)strrchr((const char*)temp_lfn, ASCII_DOT) == NULL) && (!nodots) && (tmpstr != NULL))
  {
    //no dot in string so the for example
    // file.dir.sub.txt has become
    // filedirsub  .txt
    //now it is needed to append dotextention to org string
    strcat((char *) temp_lfn, (const char *) tmpstr);
  }
  //create shortname 8 chars
    cnt = 0;
  do
  {
   if((temp_lfn[cnt] == ASCII_DOT) || (temp_lfn[cnt] == EOSTR))	break;  /* if dot just go out */
   *shortname = temp_lfn[cnt];
    shortname++;
    cnt++;
    counter++;
	
  }while(counter < DIR_SIZE_META_NAME);

 while (counter < DIR_SIZE_META_NAME) 
 	{
 	   *shortname = ASCII_SPACE;
	   shortname++;
      counter++;
 	}

  
  //find last dot and copy 3 char in ext name (or whitespace)
  if((!nodots) && (tmpstr != NULL))
  { 
    //when there are no dots (DIRECTORY) no extension can be copied
    tmpstr = (UINT8*)strchr((const char*)temp_lfn, ASCII_DOT);
    //remove dot from labelname(whitepaper req)
    tmpstr++;
    cnt = 0;
    counter = 0;
    do
    {
      if(tmpstr[cnt] == EOSTR)  break;
	  
        *shortname = tmpstr[cnt];
        shortname++;
        cnt++;
        counter++;
      
            
    }while(counter < DIR_SIZE_META_EXT);

    while (counter < DIR_SIZE_META_EXT) 
 	{
 	   *shortname = ASCII_SPACE;
	     shortname++;
	      counter++;
 	}

    //end with EOSTR
  } else {
    for(cnt = 0; cnt < DIR_SIZE_META_EXT; cnt++)
    {
      *shortname = ASCII_SPACE;
      shortname++;
    }
  }
  *shortname = EOSTR;
  //at this point shortname contains the shortname without the numeric tail
  /* Check whether tis short name exists in the current directory or not */
    //copy short file name in label
    shortname = sfn_p;
    for(i = 0; i < DIR_SIZE_META_NAME; i++)
    {
      stMetaData.aucName[i] = shortname[i];
    }
    for(j = 0; j <DIR_SIZE_META_EXT; j++)
    {
      stMetaData.aucExtension[j] = shortname[i];
      i++;
    }

 
  do
  {
      

#if (RFSFAT_SFN_OPT != 1)
      shortname = sfn_p;
      //create numeric tail
      num_tail[TAILNUM_ONES]++;
      tailmask |= TAIL_ONES;
      if(num_tail[TAILNUM_ONES] > 9)
      {
        tailmask |= TAIL_TENS;
        num_tail[TAILNUM_ONES] = 0;
        tailmask &= (~TAIL_ONES);
        num_tail[TAILNUM_TENS]++;
        if(num_tail[TAILNUM_TENS] > 9)
        {
          tailmask &= (~TAIL_TENS);
          num_tail[TAILNUM_TENS] = 0;
          num_tail[TAILNUM_HUND]++;
          tailmask |= TAIL_HUND;
          if(num_tail[TAILNUM_HUND] > 9)
          {
            tailmask &= (~TAIL_HUND);
            num_tail[TAILNUM_HUND] = 0;
            num_tail[TAILNUM_THOUS]++;
            tailmask |= TAIL_THOUS;
            if(num_tail[TAILNUM_THOUS] > 9)
            {
              tailmask &= (~TAIL_THOUS);
              num_tail[TAILNUM_THOUS] = 0;
              num_tail[TAILNUM_TENTHOUS]++;
              tailmask |= TAIL_TENTHOUS;
              if(num_tail[TAILNUM_TENTHOUS] > 9)
              {
                tailmask &= (~TAIL_TENTHOUS);
                num_tail[TAILNUM_TENTHOUS] = 0;
                num_tail[TAILNUM_HUNDTHOUS]++;
                tailmask |= TAIL_HUNDTHOUS;
                if(num_tail[TAILNUM_HUNDTHOUS] > 9)
                {
                  tailmask = 0;
                  for(i = 0; i < TAILNUM_HUNDTHOUS; i++)
                  {
                    num_tail[i] = 0;
                  }
                  RFSFAT_SEND_TRACE_ERROR("RFSFAT: sfn creation failed, all available sfn are used");
                  res = FFS_RV_ERR_INVALID_NAMEEXT;
                  break;
                }
              }
            }
          }
        }
      }
      if((tailmask & TAIL_HUNDTHOUS) == TAIL_HUNDTHOUS)
      {
        cnt = TAILNUM_HUNDTHOUS;
      } else if((tailmask & TAIL_TENTHOUS) == TAIL_TENTHOUS) {
        cnt = TAILNUM_TENTHOUS;
      } else if((tailmask & TAIL_THOUS) == TAIL_HUNDTHOUS) {
        cnt = TAILNUM_THOUS;
      } else if((tailmask & TAIL_HUND) == TAIL_HUND) {
        cnt = TAILNUM_HUND;
      } else if((tailmask & TAIL_TENS) == TAIL_TENS) {
        cnt = TAILNUM_TENS;
      } else if((tailmask & TAIL_ONES) == TAIL_ONES) {
        cnt = TAILNUM_ONES;
      } else {
        //error
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: creating SFN failed");
        res = DIR_RV_INVALID_PARAM;
      }
      //attach tail to name
      j = (DIR_SIZE_META_NAME - 1);
      for(i = 0; i <= cnt; i++)
      {
        shortname[j] = (num_tail[i] + 0x30);
        j--;
        if(i == cnt)
        {
          shortname[j] = OEM_TILDE;
          break;
        }
    //  }
    }
    shortname = sfn_p;

#else
tail_num++;

shortname[DIR_SIZE_META_NAME-2]= OEM_TILDE;
shortname[DIR_SIZE_META_NAME-1]= tail_num+ASCII_VALUE_0;  /* ASCII char */

#endif
	
    //copy short file name in label
    for(i = 0; i < DIR_SIZE_META_NAME; i++)
    {
      stMetaData.aucName[i] = shortname[i];
    }
    for(j = 0; j <DIR_SIZE_META_EXT; j++)
    {
      stMetaData.aucExtension[j] = shortname[i];
      i++;
    }

/* CHECK FOR THE DUPLICATE ENTRY  ..... START */

    //save flag
    tmp_flag = RFSFAT_LFN_FLAG;
    RFSFAT_LFN_FLAG = FALSE;
    //check if shortname(~tail).ext does exist
    res = DIR_ucGetLabel(&tmploc, &stMetaData);
    //restore flag
    RFSFAT_LFN_FLAG = tmp_flag;

/* CHECK FOR THE DUPLICATE ENTRY  ..... END */
	
    if(res == DIR_RV_DIR_NOT_FOUND)
    {
      //label does not yet exist
      res = FFS_NO_ERRORS;
      found = TRUE;
    } else if(res == FFS_NO_ERRORS) {
      //not yet found current name allready exists try next
      found = FALSE;

#if (RFSFAT_SFN_OPT == 1)
count++;

/* The created SFN is found */
if(tail_num >= RFSFAT_SFN_THILDA_LMT)
{
   tail_num=0;  /* reset the tail numbering */

   /* More numbers, better to randomize the SFNs */
   RFSFAT_rand_SFN(shortname,count); 
  
   
}



#endif
    } else {
      //error return res
      break;
    }
  }while(!found);
  //repeat untill found. a valid one
  RFSFAT_FREE_BUF(temp_lfn);
  return res;
}


#if(RFSFAT_SFN_OPT == 1)


#define RFSFAT_SFN_COL_LIMIT_1    0xFFFF 
#define RFSFAT_SFN_COL_LIMIT_2    0xFFFFF


static UINT8 RFSFAT_rand_SFN(UINT8 * shortname, UINT32 count)
{
 UINT32 ran_num;
 UINT8 val,index,base, rot;

 if(count > RFSFAT_SFN_COL_LIMIT_2)
 	  base = 0;
 else 
 	if (count > RFSFAT_SFN_COL_LIMIT_1)
		base =1;
	else
		base =2;

 rot = 6-base;   /* Number of rotoations to randomize the short name */
 
 ran_num = rand();   /* get a random number */

 for(index=0;index<rot;index++)
 	{
 	   val=((ran_num>>(4*index))&0x0F);

	   if(val > 9)
	   	{
	   	   shortname[base+index]=val+ASCII_VALUE_A;
	   	}
	   else
	   	{
	   	   shortname[base+index]=val+ASCII_VALUE_0;
	   	}
 	}

   return 0;
 
}

#undef RFSFAT_SFN_COL_LIMIT_1     
#undef RFSFAT_SFN_COL_LIMIT_2    




#endif


/* A label or more labels are written. The startposition filedirloc_p will be used to
 * start writing. the LFN label with the highest orderbyte is written first the SFN is 
 * written last. So when writing or creating this label the caller has to provide the 
 * label pos from where the the LFN is written , not the label entry of the SFN*/
 
UINT8 create_write_label(DIR_LABEL_LOCATION_STRUCT* filedirloc_p,
                         DIR_DIRECTORY_LABEL_STRUCT *  pstMetaData,
                         const UINT8* filedir_name)
{
  LFN_LABEL_STRUCT* lfn_lbl_p=NULL;
  LFN_LABEL_STRUCT* tmpstruct_p=NULL;
  DIR_DIRECTORY_LABEL_STRUCT *sfn_lbl_p=NULL;
  DIR_DIRECTORY_LABEL_STRUCT tmpsfn_lbl;
  UINT8* tmpsfn_lbl_p;
  FFS_FILE_LENGTH_TYPE lblpos = 0;
  UINT16 nr_lfne;
  UINT8 crc_sfn;
  UINT16 cntr = 0;
  UINT8 str_size = 0;
  UINT16 size = 0;
  UINT8 i,j = 0;
  UINT8 res;

  if(RFSFAT_LFN_FLAG)
  {
    //reserve lfn labels
    NR_LFN_LABELS(nr_lfne);     
    RFSFAT_GETBUF((nr_lfne * (sizeof(LFN_LABEL_STRUCT))), &lfn_lbl_p);
    CREATE_UNI_CODE_STR;
                                   
    //set all bytes to 0xffff
   (void) rfsfat_vpMemset ( lfn_lbl_p, DEF_LFNE_VALUE,
                            (UINT16)(nr_lfne * DIR_SIZE_META_ALL));

    //calculate and store crc from short file name
    crc_sfn = DIR_crc_calculate(((UINT8*)filedir_name));


    //str size inlcluding trailing \0
    i=0;
    size = (strlen((const char *)RFSFAT_LFN) + 1);
    for(cntr = (nr_lfne-1); cntr >= 1; cntr--)
    {
      tmpstruct_p = (LFN_LABEL_STRUCT*)(((cntr-1) * (sizeof(LFN_LABEL_STRUCT))) + (UINT32)lfn_lbl_p);
      //write string parts in label
      copy_strpart_lfn(tmpstruct_p, &str_size, size);
      //place order byte
      i++;
      if(cntr == 1)
      {
        i |= FIRST_POS_MARKER;
      }
      tmpstruct_p->lfn_order = i;
      //place sfn crc
      tmpstruct_p->lfn_crc = crc_sfn;
      //set LFN attribute
      tmpstruct_p->lfn_attr = LFN_MASK;
      //default remaining label entries to 0
      tmpstruct_p->lfn_firstcl = 0;
      tmpstruct_p->lfn_type = 0;
    }
    FREE_UNI_CODE_STR;
    //
    //create sfn label that belongs with LFN
    //
    tmpstruct_p = ( LFN_LABEL_STRUCT*)(((nr_lfne-1) * 
                    (sizeof(LFN_LABEL_STRUCT))) + (UINT32)lfn_lbl_p);
    sfn_lbl_p = (DIR_DIRECTORY_LABEL_STRUCT*)tmpstruct_p;
    //&tmpsfn_lbl;
    //reset temp label to null again
    rfsfat_vpMemset(sfn_lbl_p, 0x0000, sizeof(DIR_DIRECTORY_LABEL_STRUCT));
    sfn_lbl_p->ulFileSize = pstMetaData->ulFileSize;
    sfn_lbl_p->ucAttribute = pstMetaData->ucAttribute;
    sfn_lbl_p->uiFirstClusterNumber = pstMetaData->uiFirstClusterNumber;
    sfn_lbl_p->uiFirstClusterNumberHigh = pstMetaData->uiFirstClusterNumberHigh;

    /* Set the creation and date */
	 res = ffs_ucGetTimeAndDate ((UINT16*)(&(sfn_lbl_p->crtTime)),
                                (UINT16*)(&(sfn_lbl_p->crtDate)));

    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_FREE_BUF(lfn_lbl_p);
      return res;
    }
	
	/* Update date and time info.     */                                          
    res = ffs_ucGetTimeAndDate ((UINT16*)(&(sfn_lbl_p->uiLastChangedTime)),
                                (UINT16*)(&(sfn_lbl_p->uiLastChangedDate)));
    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_FREE_BUF(lfn_lbl_p);
      return res;
    }
    for(i = 0; i < DIR_SIZE_META_NAME; i++)
    {
      //since weve just prepared the filedir_name string no further checks 
      //on dots and or whitespaces etc.are needed
      sfn_lbl_p->aucName[i] = filedir_name[j];
      j++;
    }
    for(i = 0; i < DIR_SIZE_META_EXT; i++)
    {
      //since weve just prepared the filedir_name string no further checks 
      //on dots and or whitespaces etc.are needed
      sfn_lbl_p->aucExtension[i] = filedir_name[j];
      j++;
    }
  
    //write all lfn
    //
    lblpos = filedirloc_p->tLblPos;
    //we suffer from structure alignment so we have to manually copy the values to the spec
    //SFN buf
    for(cntr = 0; cntr < nr_lfne; cntr++)
    {
      tmpstruct_p = (LFN_LABEL_STRUCT*)((cntr * (sizeof(LFN_LABEL_STRUCT))) + (UINT32)lfn_lbl_p);
      tmpsfn_lbl_p = (UINT8*)&tmpsfn_lbl;
      //reset temp label to null again
      rfsfat_vpMemset(tmpsfn_lbl_p, 0x0000, sizeof(DIR_DIRECTORY_LABEL_STRUCT));

      if(cntr == (nr_lfne - 1))
      {
        //this last label is filled differently so here no conversion is needed
        filedirloc_p->tLblPos = lblpos + (cntr * DIR_SIZE_META_ALL);
        res = DIR_ucSetMetaData (filedirloc_p, (DIR_DIRECTORY_LABEL_STRUCT *)tmpstruct_p);
        if(res != FFS_NO_ERRORS)
        { 
          RFSFAT_FREE_BUF(lfn_lbl_p);
          RFSFAT_SEND_TRACE_ERROR("RFSFAT: Error writing LFN label");
          return res;
        }
      } else {
        //This is ugly hacking to get the bytes from LFN_LABEL_STRUCT to 
        //DIR_DIRECTORY_LABEL_STRUCT although this is not a very nice solution 
        //it HAS to be done because the compiler alligns the two structs differently 
        //so they can not be typecasted easily from LFN_LABEL_STRUCT to 
        //DIR_DIRECTORY_LABEL_STRUCT 
        /*******************************************************************************/
        convert_from_sfn_lfn(&tmpsfn_lbl, tmpstruct_p);
        /*******************************************************************************/        
        filedirloc_p->tLblPos = lblpos + (cntr * DIR_SIZE_META_ALL);
        res = DIR_ucSetMetaData (filedirloc_p, (DIR_DIRECTORY_LABEL_STRUCT *)tmpsfn_lbl_p);
        if(res != FFS_NO_ERRORS)
        { 
          RFSFAT_FREE_BUF(lfn_lbl_p);
          RFSFAT_SEND_TRACE_ERROR("RFSFAT: Error writing LFN label");
          return res;
        }
      }
    }
    RFSFAT_FREE_BUF(lfn_lbl_p);
  } else {
    /****************************************************************************/
    /* Store meta data, using some default values.                              */
    /****************************************************************************/
    pstMetaData->ulFileSize = DEFAULT_FILE_SIZE;
    pstMetaData->crtTimeTenth = 0;
	pstMetaData->lstAccDate = 0;

   /* Set the creation and date */
	 res = ffs_ucGetTimeAndDate ((UINT16*)(&(pstMetaData->crtTime)),
                                (UINT16*)(&(pstMetaData->crtDate)));

    if (res != FFS_NO_ERRORS)
    {
     
      return res;
    }
	
	/* Update date and time info.     */                                          
    res = ffs_ucGetTimeAndDate ((UINT16*)(&(pstMetaData->uiLastChangedTime)),
                                (UINT16*)(&(pstMetaData->uiLastChangedDate)));
    if (res != FFS_NO_ERRORS)
    {

      return res;
    }

	
		
    //time is previously set by caller
    res = DIR_ucSetMetaData (filedirloc_p, pstMetaData);
  }
  return res;
}

UINT8 dotdot_new_parent(DIR_DIRECTORY_LABEL_STRUCT* orgmetdat_p, UINT8 mpt_idx, 
                         UINT32 new_first_cl, UINT32 datacl)
{
  UINT8 res = FFS_NO_ERRORS;
  DIR_LABEL_LOCATION_STRUCT lblloc;
  
  if (orgmetdat_p->ucAttribute & DIR_MASK)
  {
    /* The second entry in the directory table of the directory (the ".."     */
    /* entry) must be changed. It's value of the first cluster number must    */
    /* be set to the directory of the destination table.                      */
    /* (You do have to read the sentences above a couple of times!)           */
    lblloc.mpt_idx = mpt_idx;
    lblloc.uiFirstCl = datacl;
    lblloc.tLblPos = POS_META_DATA_UPPER_DIR;

    DIR_META_DATA.uiFirstClusterNumberHigh = new_first_cl>>16;
    DIR_META_DATA.uiFirstClusterNumber = (UINT16)new_first_cl;

    res = ucSetMetaData (&lblloc, META_TAG_CLUSTER);
    if (res != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: dotdot_new_parent failed");
      return res;
    }
  }
  return res;
}

//This function reads from the indicated DIR table the 8+3 entry, then it searches
//the above positioned labels to check if they are part of a LFN that belongs with 
//the first read 8+3 label. If so it will locate the first label of the LFN and 
//return it in "sfn_loc". Also the number of labels included in this LFN is returned,
//this also includes the 8+3 label.
//When a corrupted LFN is found, the number of labels (again including the 8+3 label)
//that are valid, is returned. And the sfn_loc will point to the last label that was 
//valid
UINT8 DIR_find_lfn_of_sfn(const DIR_LABEL_LOCATION_STRUCT* sfn_loc, UINT8* nr_found_entries)
{
  DIR_LABEL_LOCATION_STRUCT  prevlbl;
  LFN_LABEL_STRUCT prevmetdat;
  DIR_DIRECTORY_LABEL_STRUCT tmpsfnlbl;
  UINT8 res = FFS_NO_ERRORS;
  BOOL first_entry_found = TRUE;
  UINT8 lfne_cntr = 0;
  UINT8 crc = 0;
  UINT8 shortname[FFS_MAX_SIZE_NAME_DOT_EXT];


  memcpy(&prevlbl,sfn_loc,sizeof(DIR_LABEL_LOCATION_STRUCT));

  //before looking for lfne read the sfn and store the crc and the shortname etc.
  res = DIR_ucGetMetaData (&prevlbl, (DIR_DIRECTORY_LABEL_STRUCT *)&prevmetdat);
  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_ucGetMetaData in DIR_find_lfn_of_sfn failed");
    return res;
  }
  //only the first time (thats when 
  //copy & validate name from sfn label 
  DIR_cpy_lbl_str(shortname, DIR_META_DATA.aucName, DIR_META_DATA.aucExtension);
  //calculate crc
  crc = DIR_crc_calculate(shortname);

  lfne_cntr++; //sfn counts as 1
  //read label previous to SFN
  if(prevlbl.tLblPos >= DIR_SIZE_META_ALL)
  {
    prevlbl.tLblPos -= DIR_SIZE_META_ALL;
  } else {
    //already first label cannot go higher in dir table
    // So it cannot be part of LFN
    first_entry_found = FALSE;
  }
  res = DIR_ucGetMetaData (&prevlbl, &tmpsfnlbl);
  //due to alignment a two step strategy
  convert_from_lfn_sfn(&prevmetdat, &tmpsfnlbl);
  //if part of LFN and not empty find more
  if(RECOG_LFN && (!EMPTY_LABEL))
  {
    lfne_cntr++;
    //start looking for lfn entries
    while((!RFSFAT_FIRST_LFN_ENTRY) && RECOG_LFN)
    {
      if(prevlbl.tLblPos >= DIR_SIZE_META_ALL)
      {
        prevlbl.tLblPos -= DIR_SIZE_META_ALL;
      } else {
        //already first label cannot go higher in dir table
        // So it cannot be part of LFN
        first_entry_found = FALSE;
        break;
      }
      res = DIR_ucGetMetaData (&prevlbl, &tmpsfnlbl);
      //due to alignment a two step strategy
      convert_from_lfn_sfn(&prevmetdat, &tmpsfnlbl);
      if (res != FFS_NO_ERRORS)
      {
        if(res == FFS_RV_ERR_ENTRY_EMPTY)
        {
          //okay it is empty so it cannot be part of lfn
          first_entry_found = FALSE;
          break;
        } else {
          //if(res = FFS_RV_ERR_LAST_ENTRY) is also wrong since it cannot be the last 
          // while we are reading the one above the sfn label
          RFSFAT_SEND_TRACE_ERROR("RFSFAT: valid rename get meta data failed");
          return res;
        }
      }
	  if(EMPTY_LABEL)
	  	{
	  	   first_entry_found = FALSE;
		   break;
	  	}
      //found a valid entry -> check if is a LFN that belongs with the sfn
      if(!RECOG_LFN)
      {
        first_entry_found = TRUE;
		prevlbl.tLblPos += DIR_SIZE_META_ALL;
        break;
      } else if(prevmetdat.lfn_crc != crc) { 

        //definitely NOT LFN memeber of the sfn
        first_entry_found = FALSE;
        break;  
      }
      lfne_cntr++;

    }//while
  } else {
    // there was no LFN so return original location data and no errors
    *nr_found_entries = lfne_cntr;
    return FFS_NO_ERRORS;
  }
  //nr of entries found including SFN
  *nr_found_entries = lfne_cntr;
  if(first_entry_found)
  {
    //okay even the first lfn entry is found
    *(DIR_LABEL_LOCATION_STRUCT*)sfn_loc = prevlbl;
    return FFS_NO_ERRORS;
  } else {
    prevlbl.tLblPos += DIR_SIZE_META_ALL; //set to previous (last valid lfn)
    *(DIR_LABEL_LOCATION_STRUCT*)sfn_loc = prevlbl;
    if(lfne_cntr > 1)
    {
      //first entry is not found but there was still a reason to quit searching
      //the can be e.g. that 3 valid lfn entries were found but the 4th is missing
      //when this function is called by a delete function the number of valid lfn that 
      //was found will be deleted eventhough the first entry is not found (or else 
      //the system will be full of corrupted empty labels
      return DIR_RV_CORRUPT_LFN;
    } else if (lfne_cntr == 1) {

      //there is no LFN with this sfn when called by delete function the nr 
      //of entries returned (1 in this case) will be removed
      return FFS_NO_ERRORS;
    } else {
      return RFSFAT_INTERNAL_ERR;
    }
  } 
}


//DUE to allignment issues we must perform a conversion
UINT8 convert_from_lfn_sfn(LFN_LABEL_STRUCT* lfnlbl_p, DIR_DIRECTORY_LABEL_STRUCT* sfnlbl_p)
{
  UINT8* tmp_p;
  UINT8 i=0;

  tmp_p = (UINT8*)sfnlbl_p;
  lfnlbl_p->lfn_order = *tmp_p;
  tmp_p++;
  rfsfat_vpMemcpy(lfnlbl_p->lfn_5char_str, tmp_p, (LFN_5CHR * 2));
  for(i=0; i<(LFN_5CHR*2); i++)
  {
    tmp_p++;
  }
  lfnlbl_p->lfn_attr = *tmp_p;
  tmp_p++;
  lfnlbl_p->lfn_type = *tmp_p;
  tmp_p++;
  lfnlbl_p->lfn_crc = *tmp_p;
  tmp_p++;
  rfsfat_vpMemcpy(lfnlbl_p->lfn_6char_str, tmp_p, (LFN_6CHR * 2));
  for(i=0; i<(LFN_6CHR*2); i++)
  {
    tmp_p++;
  }  
  lfnlbl_p->lfn_firstcl = (UINT16)*tmp_p;
  tmp_p++;
  tmp_p++;
  rfsfat_vpMemcpy(lfnlbl_p->lfn_2char_str, tmp_p, (LFN_2CHR * 2));

  return FFS_NO_ERRORS;
}

UINT8 convert_from_sfn_lfn(DIR_DIRECTORY_LABEL_STRUCT* sfnlbl_p, LFN_LABEL_STRUCT* lfnlbl_p)
{
  UINT8* tmpsfn_lbl_p = (UINT8*)sfnlbl_p;
  UINT8 i = 0;
  
  *tmpsfn_lbl_p = (UINT8)lfnlbl_p->lfn_order;
  tmpsfn_lbl_p++; 
  rfsfat_vpMemcpy(tmpsfn_lbl_p, lfnlbl_p->lfn_5char_str, (LFN_5CHR * 2));
  for(i=0; i<(LFN_5CHR*2); i++)
  {
    tmpsfn_lbl_p++;
  }
  *tmpsfn_lbl_p = lfnlbl_p->lfn_attr;
  tmpsfn_lbl_p++;
  *tmpsfn_lbl_p = 0;
  tmpsfn_lbl_p++;
  *tmpsfn_lbl_p = lfnlbl_p->lfn_crc;
  tmpsfn_lbl_p++;
  rfsfat_vpMemcpy(tmpsfn_lbl_p, lfnlbl_p->lfn_6char_str, (LFN_6CHR * 2));
  for(i=0; i<(LFN_6CHR*2); i++)
  {
    tmpsfn_lbl_p++;
  }
  *tmpsfn_lbl_p = (UINT8)((lfnlbl_p->lfn_firstcl >> 8) & 0xFF);
  tmpsfn_lbl_p++;
  *tmpsfn_lbl_p = (UINT8)(lfnlbl_p->lfn_firstcl & 0xFF);
  tmpsfn_lbl_p++;
  rfsfat_vpMemcpy(tmpsfn_lbl_p, lfnlbl_p->lfn_2char_str, (LFN_2CHR * 2));

  return FFS_NO_ERRORS; 
}


/******Copyright by ICT Embedded BV All rights reserved ***********************/
UINT8
DIR_ucTruncLabel (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation, T_FSCORE_SIZE Newsize)
/* PURPOSE      : This function deletes a directory-table label by changing
 *                the first byte of the file or directory label's meta-data
 *                into 0xE5. The first byte of the label's meta-data is the
 *                first character of the file/directory name in the meta-data.
 *                Besides changing the directory label, the cluster-linked
 *                list will be removed by the FAT manager.
 *                This function removes file-data as well as the directory
 *                table of a subdirectory. In case of removing the label of
 *                a sub-directory be sure the sub directory is empty.
 *                When the label involves a LFN all the LFN labels including the 
 *                8+3 (SFN) label, will be set to E5 (removed).
 *
 * ARGUMENTS    : pstLabelLocation : Indicates the partition, the first cluter
 *                number of the directory table, and the offset within this
 *                table of the label that must be removed.
 *
 * RETURNS      : FAM_RET_FAT_READ_ERROR:
 *                Error occurred while reading from the FAT table.
 *
 *                FAM_RET_FAT_WRITE_ERROR:
 *                Error occurred while writing to the FAT table.
 *
 *                FAM_RET_NO_FAT_CLUSTER:
 *                Not a valid FAT cluster number
 *
 *                FFS_NO_ERRORS:
 *                Operation was ended successfully.
 *
 *                CLM_RET_BAD_INPUT:
 *                The parameters supplied didn't meet the pre-condition of the
 *                function.
 *
 *                CLM_RET_TOO_LITTLE_SPACE:
 *                There wasn't enough space for writing the supplied data at
 *                the medium.
 *
 *                CLM_RET_UNDEF_ERROR:
 *                An undefined error occurred.
 *
 * PRECONDITION : Media manager must be initialised.
 *                pstLabelLocation may not be NULL.
 *                In case a directory label of a directory must be deleted, be
 *                sure this directory is empty.
 *
 * POSTCONDITION: -
 *
 * ERROR AND EXCEPTION HANDLING: -
 *
 * NOTE         : In case a directory label of a directory with a file was
 *                deleted, the clusters which are used by the file's data will
 *                be marked as 'used' but will never be accessible again. For
 *                this, be sure the directory of which you want to remove the
 *                label, is empty.
 *
 ******************************************************************************/
{
  UINT8 ucResult;
  UINT32 clusterNmb;
  DIR_LABEL_LOCATION_STRUCT label_pos;
  
  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE ((pstLabelLocation != NULL),   
  	return DIR_RV_INVALID_PARAM);

  label_pos = *pstLabelLocation;
  
  ucResult = ucGetMetaData(&label_pos);
  if(ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData in DIR_ucTruncLabel failed");
    return ucResult;
  }
    /* Check if file is read-only.                                              */
  if ((DIR_META_DATA.ucAttribute & FFS_ATTRIB_READONLY_MASK) != 0)
  {
    RFSFAT_SEND_TRACE ("RFSFAT: FFS_RV_ERR_FILE_READ_ONLY",RV_TRACE_LEVEL_ERROR);
    return FFS_RV_ERR_FILE_READ_ONLY;
  }

 /* Delete/remove file data, if any.                                         */
   clusterNmb = ((DIR_META_DATA.uiFirstClusterNumberHigh << 16) |( DIR_META_DATA.uiFirstClusterNumber));
    DIR_META_DATA.ulFileSize = Newsize;

    ucResult = ucSetMetaData (&label_pos, META_TAG_SIZE);
    if (ucResult != FFS_NO_ERRORS)
	return ucResult;

	flush_write_blk();

 if (clusterNmb != 0)
 	{
 	 
    	ucResult = FAM_ucTruncLinkedList (pstLabelLocation->mpt_idx, clusterNmb,Newsize);

		#ifdef ICT_SPEEDUP_RFSFAT2 
  		ucFlushFamCash(pstLabelLocation->mpt_idx);
		#endif //ICT_SPEEDUP_RFSFAT2

 	}	
  return ucResult;
}


#if (FFS_UNICODE ==1)

/**************************************************/
// Unicode       
/**************************************************/

UINT8 lfn_new_label_pos_uc(
            DIR_LABEL_LOCATION_STRUCT  *const tmplabelloc_p,
            const T_WCHAR              *nameUnicode_p)
{
  UINT8     res;
  UINT8     nr_lfne;  //number of directory entries requireed for LFN entry
  UINT16    nmbUcElements;  

  nmbUcElements = wstrlen( nameUnicode_p); //without terminating EOS

  if(nmbUcElements <= CHARS_IN_LFNE)
  	{
  	      nr_lfne = 1;  /* only one short file name */
  	}
  else
  if ((nmbUcElements % CHARS_IN_LFNE) != 0)
 	{
    nr_lfne = (nmbUcElements / CHARS_IN_LFNE) + 2;
    }
  else
    {
    nr_lfne = (nmbUcElements / CHARS_IN_LFNE) + 1;
    }
 
  res = ucGetNewLabelPos (tmplabelloc_p, nr_lfne);
  
  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetNewLabelPos_uc failed");
  }
  return res;
}


/**************************************************/
/**************************************************/
UINT8 copy_strpart_lfn_uc(
            LFN_LABEL_STRUCT       *const lfne_p,
            UINT8                  *const str_pos_p,
            UINT16                 str_size,
            const T_WCHAR   *nameUnicode_p)
{
  UINT8 cnt = 0;
  
  while((cnt != LFN_5CHR) && (*str_pos_p < str_size))
  {
    lfne_p->lfn_5char_str[cnt] = nameUnicode_p[*str_pos_p];
    (*str_pos_p)++;
    cnt++;
  }
  cnt = 0;
  while((cnt != LFN_6CHR) && (*str_pos_p < str_size))
  {
    lfne_p->lfn_6char_str[cnt] = nameUnicode_p[*str_pos_p];
    (*str_pos_p)++;
    cnt++;
  }
  cnt = 0;
  while((cnt != LFN_2CHR) && (*str_pos_p < str_size))
  {
    lfne_p->lfn_2char_str[cnt] = nameUnicode_p[*str_pos_p];
    (*str_pos_p)++;
    cnt++;
  }
  return RFSFAT_OK;
}

//the content that parentdirloc_p points to can be changed after this function is called,
//it will point to the label that contains the new created shortname IF it exist in 
//the directory to place it.(if newname is unique it wont be changed)
//sfn will point to the new created sfn after calling this function
static UINT8 create_short_name_uc(
            DIR_LABEL_LOCATION_STRUCT* const  parentdirloc_p,
            UINT8                    * const  sfn_p,
            const  T_WCHAR             *nameUnicode_p)
{
  UINT8* temp_lfn;
  UINT8* shortname;
  UINT8* tmpstr = NULL;
  UINT8 cnt = 0;
  UINT8 counter=0;
  UINT8 i,j;
  UINT16 size = 0;
  DIR_DIRECTORY_LABEL_STRUCT stMetaData;
  UINT8 res = FFS_NO_ERRORS;
  BOOL found = FALSE;
  BOOL nodots = FALSE;
  DIR_LABEL_LOCATION_STRUCT tmploc;
  BOOL tmp_flag;
  BOOL stopAtDot;
  UINT8 nameOnlyLength;
  UINT8 *convertedName_p;

#if (RFSFAT_SFN_OPT != 1)
  
  UINT8 num_tail[MAX_TAIL];
  UINT8 tailmask = 0;
#else
    UINT8 tail_num = 0;
    UINT32 count=0;; 
#endif  
 
  tmploc = *parentdirloc_p;
  //start searching at offset null
  tmploc.tLblPos = 0;

#if (RFSFAT_SFN_OPT != 1)
   //init num_tail
  for(cnt = 0; cnt < MAX_TAIL; cnt++)
  {
    num_tail[cnt] = 0;
  }

#endif
  size = (wstrlen(nameUnicode_p) + 1);
  RFSFAT_GETBUF(size, &convertedName_p);
  convertUcToU8(nameUnicode_p, convertedName_p); 

  //create temp buf long enough for namein UINT8
  RFSFAT_GETBUF_NO_RETURN(size, &temp_lfn);

  if(temp_lfn == NULL)
  	{
  	   /* remove the earlier memory allocation */
	   RFSFAT_FREE_BUF(convertedName_p);
	   return RFSFAT_MEMORY_ERR;
  	}
  
 
  memset(temp_lfn,0x00,size);
  shortname = sfn_p;
  memset(shortname,ASCII_SPACE,(DIR_SIZE_META_NAME+DIR_SIZE_META_EXT));
  rfsfat_ucStrncpy(temp_lfn, convertedName_p,size-1);


  // all chars to upper case
  string_toupper(temp_lfn);
  //remove all spaces and leading dots
  RM_CHAR(temp_lfn, ASCII_SPACE);
  //remove all dots from string except the last one
  tmpstr = (UINT8*)strrchr((const char*)temp_lfn, ASCII_DOT);
  if(tmpstr != NULL)
  {
    *tmpstr = (UINT8)EOSTR;
    RM_CHAR(temp_lfn, ASCII_DOT);
    *tmpstr = (UINT8)ASCII_DOT;
  } else {
    nodots = TRUE;
  }
  //when there was only 1 dot in the name the .ext doesnt need to be extended 
  //anymore (no chars have been removed!) if temp_lfn still contains a . after 
  //this action this was the case so no strcat is needed
  if(((UINT8*)strrchr((const char*)temp_lfn, ASCII_DOT) == NULL) && (!nodots) && (tmpstr != NULL))
  {
    //no dot in string so the for example
    // file.dir.sub.txt has become
    // filedirsub  .txt
    //now it is needed to append dotextention to org string
    strcat((char *) temp_lfn, (const char *) tmpstr);
  }
  //create shortname 8 chars
  cnt = 0;
  counter = 0;
  stopAtDot = FALSE;
  nameOnlyLength = 0;
  do
  {
    if (temp_lfn[cnt] == ASCII_DOT)
    {
      stopAtDot = TRUE;
    }
    if((!stopAtDot) && (temp_lfn[cnt] != EOSTR))
    {
      *shortname = temp_lfn[cnt];
      nameOnlyLength++;
	  cnt++;
    } else {
      stopAtDot=TRUE;   /* after this character fill only ASCII_SPACE */   
      *shortname = ASCII_SPACE;
    } 
    shortname++;
    counter++;
  }while(counter < DIR_SIZE_META_NAME);
  // nameOnlyLength is Nmb of vallid copied characters in meta name

  if ((nameOnlyLength == DIR_SIZE_META_NAME) &&
      (temp_lfn[cnt] != EOSTR) && (temp_lfn[cnt] != ASCII_DOT))
  {
    //The name before the dot is longer then DIR_SIZE_META_NAME:
    //This means we must use the abreviation (~)
    nameOnlyLength = DIR_SIZE_META_NAME + 1;
  }
  //nameOnlyLength <= DIR_SIZE_META_NAME means we should try 
  // the first time not to use the '~'
  
  //find last dot and copy 3 char in ext name (or whitespace)
  if(!nodots)
  { 
    //when there are no dots (DIRECTORY) no extension can be copied
    tmpstr = (UINT8*)strchr((const char*)temp_lfn, ASCII_DOT);
    //remove dot from labelname(whitepaper req)
    tmpstr++;
    cnt = 0;
    counter = 0;
    do
    {
      if(tmpstr[cnt] != EOSTR)
      {
        *shortname = tmpstr[cnt];
        shortname++;
        cnt++;
        counter++;
      } else {
        *shortname = ASCII_SPACE;
        shortname++;
        counter++;
      }  
    }while(counter < DIR_SIZE_META_EXT);
    //end with EOSTR
  } else {
    for(cnt = 0; cnt < DIR_SIZE_META_EXT; cnt++)
    {
      *shortname = ASCII_SPACE;
      shortname++;
    }
  }
  *shortname = EOSTR;
  //at this point shortname contains the shortname without the numeric tail
  /* Check whether tis short name exists in the current directory or not */
   
    //copy short file name in label
    shortname = sfn_p;
    for(i = 0; i < DIR_SIZE_META_NAME; i++)
    {
      stMetaData.aucName[i] = shortname[i];
    }
    for(j = 0; j <DIR_SIZE_META_EXT; j++)
    {
      stMetaData.aucExtension[j] = shortname[i];
      i++;
    }

    
  do
  {


#if(RFSFAT_SFN_OPT != 1)  
    //Try to skip '~' algorithm the first time when name was short enough
    if (nameOnlyLength > DIR_SIZE_META_NAME)
    {
      shortname = sfn_p;
      //create numeric tail
      num_tail[TAILNUM_ONES]++;
      tailmask |= TAIL_ONES;
      if(num_tail[TAILNUM_ONES] > 9)
      {
        tailmask |= TAIL_TENS;
        num_tail[TAILNUM_ONES] = 0;
        tailmask &= (~TAIL_ONES);
        num_tail[TAILNUM_TENS]++;
        if(num_tail[TAILNUM_TENS] > 9)
        {
          tailmask &= (~TAIL_TENS);
          num_tail[TAILNUM_TENS] = 0;
          num_tail[TAILNUM_HUND]++;
          tailmask |= TAIL_HUND;
          if(num_tail[TAILNUM_HUND] > 9)
          {
            tailmask &= (~TAIL_HUND);
            num_tail[TAILNUM_HUND] = 0;
            num_tail[TAILNUM_THOUS]++;
            tailmask |= TAIL_THOUS;
            if(num_tail[TAILNUM_THOUS] > 9)
            {
              tailmask &= (~TAIL_THOUS);
              num_tail[TAILNUM_THOUS] = 0;
              num_tail[TAILNUM_TENTHOUS]++;
              tailmask |= TAIL_TENTHOUS;
              if(num_tail[TAILNUM_TENTHOUS] > 9)
              {
                tailmask &= (~TAIL_TENTHOUS);
                num_tail[TAILNUM_TENTHOUS] = 0;
                num_tail[TAILNUM_HUNDTHOUS]++;
                tailmask |= TAIL_HUNDTHOUS;
                if(num_tail[TAILNUM_HUNDTHOUS] > 9)
                {
                  tailmask = 0;
                  for(i = 0; i < TAILNUM_HUNDTHOUS; i++)
                  {
                    num_tail[i] = 0;
                  }
                  RFSFAT_SEND_TRACE_ERROR("RFSFAT: sfn creation failed, all available sfn are used");
                  res = FFS_RV_ERR_INVALID_NAMEEXT;
                  break;
                }
              }
            }
          }
        }
      }
      if((tailmask & TAIL_HUNDTHOUS) == TAIL_HUNDTHOUS)
      {
        cnt = TAILNUM_HUNDTHOUS;
      } else if((tailmask & TAIL_TENTHOUS) == TAIL_TENTHOUS) {
        cnt = TAILNUM_TENTHOUS;
      } else if((tailmask & TAIL_THOUS) == TAIL_HUNDTHOUS) {
        cnt = TAILNUM_THOUS;
      } else if((tailmask & TAIL_HUND) == TAIL_HUND) {
        cnt = TAILNUM_HUND;
      } else if((tailmask & TAIL_TENS) == TAIL_TENS) {
        cnt = TAILNUM_TENS;
      } else if((tailmask & TAIL_ONES) == TAIL_ONES) {
        cnt = TAILNUM_ONES;
      } else {
        //error
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: creating SFN failed");
        res = DIR_RV_INVALID_PARAM;
      }
      //attach tail to name
      j = (DIR_SIZE_META_NAME - 1);
      for(i = 0; i <= cnt; i++)
      {
        shortname[j] = (num_tail[i] + 0x30);
        j--;
        if(i == cnt)
        {
          shortname[j] = OEM_TILDE;
          break;
        }
      }
    }
    nameOnlyLength = DIR_SIZE_META_NAME + 1; //Skip only ones
    shortname = sfn_p;
#else
  shortname = sfn_p;
tail_num++;

shortname[DIR_SIZE_META_NAME-2]= OEM_TILDE;
shortname[DIR_SIZE_META_NAME-1]= tail_num+ASCII_VALUE_0;

#endif
    //copy short file name in label
    for(i = 0; i < DIR_SIZE_META_NAME; i++)
    {
      stMetaData.aucName[i] = shortname[i];
    }
    for(j = 0; j <DIR_SIZE_META_EXT; j++)
    {
      stMetaData.aucExtension[j] = shortname[i];
      i++;
    }


/* CHECK FOR THE DUPLICATE ENTRY  ..... START */
	
    //save flag
    tmp_flag = RFSFAT_LFN_FLAG;
    RFSFAT_LFN_FLAG = FALSE;
    //check if shortname(~tail).ext does exist
    res = DIR_ucGetLabel((&tmploc), (&stMetaData));
    //restore flag
    RFSFAT_LFN_FLAG = tmp_flag;

/* CHECK FOR THE DUPLICATE ENTRY  ..... END */
	
    if(res == DIR_RV_DIR_NOT_FOUND)
    {
      //label does not yet exist
      res = FFS_NO_ERRORS;
      found = TRUE;
    } else if(res == FFS_NO_ERRORS) {
      //not yet found current name allready exists try next
      found = FALSE;

#if (RFSFAT_SFN_OPT == 1)
count++;

/* The created SFN is found */
if(tail_num >= RFSFAT_SFN_THILDA_LMT)
{
   tail_num=0;  /* reset the tail numbering */

   /* More numbers, better to randomize the SFNs */
   RFSFAT_rand_SFN(shortname,count);   
   
}

#endif
    } else {
      //error return res
      break;
    }
  }while(!found);
  //repeat untill found. a valid one
  RFSFAT_FREE_BUF(temp_lfn);
  RFSFAT_FREE_BUF(convertedName_p);
  return res;
}







static BOOL lfn_compare_uc (const UINT16* const name1, const UINT16* const name2)
{
  UINT32 i;
  BOOL match = name1 != NULL && name2 != NULL;
 if((name1 !=NULL)&& (name2 !=NULL))
	{
    for (i = 0; match && name1[i] != EOSTR && name2[i] != EOSTR && name1[i] != ((UINT16)FFS_ASCII_VALUE_FWSLASH); i++)
    {
      match =  (name1[i] == name2[i]);

	  /* ASCII Backward compatability */
	  if((!match) && (!(name1[i]&0xFF00)) && (!(name2[i]&0xFF00)))
	         match = ((toupper((char)name1[i])) == (toupper((char)name2[i])));

    }
    return (match && (name2[i] == EOSTR)  && ((name1[i] == EOSTR) || (name1[i] == ((UINT16)FFS_ASCII_VALUE_FWSLASH))));
}
	return match;
}



/**************************************************/
/**************************************************/
UINT8 DIR_ucGetLabel_uc (
                const T_WCHAR                      *pathName_p,
                RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabelLocation,
                RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData)
{
  UINT8 ucResult;
  UINT8 res;
  DIR_LFN_RET retval;
  UINT8 name_in_label[FFS_MAX_SIZE_NAME_DOT_EXT];
  
   RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: DIR_ucGetLabel_uc Entered ");


  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && ((pstMetaData != NULL))),
             return DIR_RV_INVALID_PARAM);

  /****************************************************************************/
  /* Start requesting meta data of first entry.                               */
  /****************************************************************************/
  pstLabelLocation->tLblPos = 0;

  do
  {
    ucResult = ucGetMetaData (pstLabelLocation);
	RFSFAT_LFN_FLAG = FALSE;
    if (ucResult != FFS_NO_ERRORS)
    {

      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucGetMetaData failed, Uc ");
      return ucResult;
    }

    if (LAST_LABEL)
    {
      RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: DIR_RV_DIR_NOT_FOUND, Uc ");
      return DIR_RV_DIR_NOT_FOUND;
    }

    if (!EMPTY_LABEL)
    {

     //RFSFAT_LFN_FROM_LABEL is a buffer for holding the unicode name
     //When the programmer didnt free the RFSFAT_LFN_FROM_LABEL buffer
     //it is done here to prevent memory leaking
     RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
     RFSFAT_LFN_FROM_LABEL = NULL;

	
      //determine if label belongs to LFN
      if(RECOG_LFN)
      {
      RFSFAT_LFN_FLAG = TRUE;
        //verify if the first entry of the LFN is found
        if(RFSFAT_FIRST_LFN_ENTRY)
        {        
     
          res = DIR_read_lfn_from_label(pstLabelLocation, &retval);
          if(res != FFS_NO_ERRORS)
          {
			 RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
             RFSFAT_LFN_FROM_LABEL = NULL;          
            return res;
          }
          switch(retval)
          {
            case MEM_ERROR:
            {
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
			  RFSFAT_LFN_FLAG = FALSE;
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: not enough mem to store LFN");
              return RFSFAT_MEMORY_ERR;
            }
           /* break; */
		   
            case COPY_ERROR:
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
              RFSFAT_LFN_FROM_LABEL = NULL;
			  RFSFAT_LFN_FLAG = FALSE;
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: copying the LFN into buf failed");
              return DIR_RV_INVALID_PARAM;
          /*  break;   */
		  
            case NOT_A_VALID_LFN_LABEL:
            case WRONG_CRC:
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
              RFSFAT_LFN_FROM_LABEL = NULL;
              //don't stop searching just signal the potential flaw
              RFSFAT_SEND_TRACE_ERROR("RFSFAT: the lfn label found is not part of a lfn");
            break;
            case LFN_EMPTY_LABEL:
              //empty label was found so try to find the next one
              RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
              RFSFAT_LFN_FROM_LABEL = NULL;
            break;
            case NO_ERRORS:
              //compare the names
              if(RFSFAT_LFN_FLAG)
              {

                if(lfn_compare_uc(pathName_p, RFSFAT_LFN_FROM_LABEL))
                {
                  // Label found, return.
                  // true LFN
                  // return short label for admin
                  
                  CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P); 
                  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
                  RFSFAT_LFN_FROM_LABEL = NULL;
				  RFSFAT_LFN_FLAG = FALSE;
                  return FFS_NO_ERRORS;
                }
              }              //return if found
            break;
            
            default :
            break;
          }
        } //else not the first lfn label so skip it
      }
	  else
	  	{
	  	    /* This is SFN */
			RFSFAT_LFN_FLAG = FALSE;

			//prepare shortnames to be compared
			ffs_ucCopyNameDotExtension(DIR_META_DATA_P, name_in_label);

     		/* Make the unicode name */
			RFSFAT_GETBUF(((FFS_MAX_SIZE_NAME_DOT_EXT) *2), (&(RFSFAT_LFN_FROM_LABEL)));

  			//default to 0x0000
			rfsfat_vpMemset(RFSFAT_LFN_FROM_LABEL, 0x0000, 
                  (UINT16)((FFS_MAX_SIZE_NAME_DOT_EXT) *2));            

            convertU8ToUc(name_in_label, RFSFAT_LFN_FROM_LABEL);
			
 			if(lfn_compare_uc(pathName_p, RFSFAT_LFN_FROM_LABEL))
                {
                  // Label found, return.
                  // SFN, converted to LFN
                  // return short label for admin
                  
                  CPY_LBL_TO_LBL(pstMetaData, DIR_META_DATA_P); 
                  RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
                  RFSFAT_LFN_FROM_LABEL = NULL;
				  RFSFAT_LFN_FLAG = FALSE;
                  return FFS_NO_ERRORS;
                }
			RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);
		}
     }
    /* Not the same name, try the next one.                                   */
    NEXT_LABEL(pstLabelLocation->tLblPos);
    RFSFAT_FREE_BUF(RFSFAT_LFN_FROM_LABEL);       
    RFSFAT_LFN_FROM_LABEL = NULL;
  }
  /*lint -save -e506 */
  while (1);
  /*lint -restore */
}



/**************************************************/
/**************************************************/
UINT8 create_write_label_uc(
                         DIR_LABEL_LOCATION_STRUCT  * const filedirloc_p,
                         DIR_DIRECTORY_LABEL_STRUCT * const pstMetaData,
                         UINT8* const                 filedir_name,
                         const  T_WCHAR      * filedir_name_uc)
{
  LFN_LABEL_STRUCT* lfn_lbl_p;
  LFN_LABEL_STRUCT* tmpstruct_p;
  DIR_DIRECTORY_LABEL_STRUCT *sfn_lbl_p;
  DIR_DIRECTORY_LABEL_STRUCT tmpsfn_lbl;
  UINT8* tmpsfn_lbl_p;
  FFS_FILE_LENGTH_TYPE lblpos = 0;
  UINT16 nr_lfne;
  UINT8 crc_sfn;
  UINT16 cntr = 0;
  UINT8 str_size = 0;
  UINT16 size = 0;
  UINT8 i,j = 0;
  UINT8 res;

  

  //reserve lfn labels. Count how many dir entries we need
  size = wstrlen(filedir_name_uc); //length unicode name
  //size inlcluding trailing \0
  size++;
  if((size % CHARS_IN_LFNE) != 0)
  {
    nr_lfne = (size / CHARS_IN_LFNE) + 2; //+ one for remainder + 1 for short name
  }
  else
  {
    nr_lfne = (size / CHARS_IN_LFNE) + 1;  // + 1 for short name
  }
  //size inlcluding trailing \0

  RFSFAT_GETBUF((nr_lfne * (sizeof(LFN_LABEL_STRUCT))), &lfn_lbl_p);
  //set all bytes to 0xffff
  (void) rfsfat_vpMemset ( lfn_lbl_p, DEF_LFNE_VALUE,
                            (UINT16)(nr_lfne * DIR_SIZE_META_ALL));

  //calculate and store crc from short file name
  crc_sfn = DIR_crc_calculate(filedir_name);

  //Copy unicode data (pointed to by RFSFAT_LFN_FROM_LABEL) to the memory
  //that has been already reserved to hold all the unicode metadata structures
  i=0;
  for(cntr = (nr_lfne-1); cntr >= 1; cntr--)
  {
    tmpstruct_p = (LFN_LABEL_STRUCT*)(((cntr-1) * (sizeof(LFN_LABEL_STRUCT))) + (UINT32)lfn_lbl_p);
    //write string parts in label
    copy_strpart_lfn_uc(tmpstruct_p, &str_size, size, filedir_name_uc);
    //place order byte
    i++;
    if(cntr == 1)
    {
      i |= FIRST_POS_MARKER;
    }
    tmpstruct_p->lfn_order = i;
    //place sfn crc
    tmpstruct_p->lfn_crc = crc_sfn;
    //set LFN attribute
    tmpstruct_p->lfn_attr = LFN_MASK;
    //default remaining label entries to 0
    tmpstruct_p->lfn_firstcl = 0;
    tmpstruct_p->lfn_type = 0;
  }

  //
  //create sfn label that belongs with LFN
  //
  tmpstruct_p = ( LFN_LABEL_STRUCT*)(((nr_lfne-1) * 
                  (sizeof(LFN_LABEL_STRUCT))) + (UINT32)lfn_lbl_p);
  sfn_lbl_p = (DIR_DIRECTORY_LABEL_STRUCT*)tmpstruct_p;

  //reset temp label to null again
  rfsfat_vpMemset(sfn_lbl_p, 0x0000, sizeof(DIR_DIRECTORY_LABEL_STRUCT));
  sfn_lbl_p->ulFileSize = pstMetaData->ulFileSize;
  sfn_lbl_p->ucAttribute = pstMetaData->ucAttribute;
  sfn_lbl_p->uiFirstClusterNumber = pstMetaData->uiFirstClusterNumber;
  sfn_lbl_p->uiFirstClusterNumberHigh = pstMetaData->uiFirstClusterNumberHigh;

  
  // Update date and time info.                                               
  res = ffs_ucGetTimeAndDate ((UINT16*)(&(sfn_lbl_p->crtTime)),
                              (UINT16*)(&(sfn_lbl_p->crtDate)));
  

  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_FREE_BUF(lfn_lbl_p);
    return res;
  }
  
  // Update date and time info.                                               
  res = ffs_ucGetTimeAndDate ((UINT16*)(&(sfn_lbl_p->uiLastChangedTime)),
                              (UINT16*)(&(sfn_lbl_p->uiLastChangedDate)));
  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_FREE_BUF(lfn_lbl_p);
    return res;
  }
  for(i = 0; i < DIR_SIZE_META_NAME; i++)
  {
    //since weve just prepared the filedir_name string no further checks 
    //on dots and or whitespaces etc.are needed
    sfn_lbl_p->aucName[i] = filedir_name[j];
    j++;
  }
  for(i = 0; i < DIR_SIZE_META_EXT; i++)
  {
    //since weve just prepared the filedir_name string no further checks 
    //on dots and or whitespaces etc.are needed
    sfn_lbl_p->aucExtension[i] = filedir_name[j];
    j++;
  }

  //write all lfn directory entries
  //
  lblpos = filedirloc_p->tLblPos;

  //we suffer from structure alignment so we have to manually copy the values to the spec
  //SFN buf
  for(cntr = 0; cntr < nr_lfne; cntr++)
  {
    tmpstruct_p = (LFN_LABEL_STRUCT*)((cntr * (sizeof(LFN_LABEL_STRUCT))) + (UINT32)lfn_lbl_p);
    tmpsfn_lbl_p = (UINT8*)&tmpsfn_lbl;
    //reset temp label to null again
    rfsfat_vpMemset(tmpsfn_lbl_p, 0x0000, sizeof(DIR_DIRECTORY_LABEL_STRUCT));

    if(cntr == (nr_lfne - 1))
    {
      //this last label is filled differently so here no conversion is needed
      filedirloc_p->tLblPos = lblpos + (cntr * DIR_SIZE_META_ALL);
      res = DIR_ucSetMetaData (filedirloc_p, (DIR_DIRECTORY_LABEL_STRUCT *)tmpstruct_p);
      if(res != FFS_NO_ERRORS)
      { 
        RFSFAT_FREE_BUF(lfn_lbl_p);
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: Error writing LFN label");
        return res;
      }
    } else {
      //This is ugly hacking to get the bytes from LFN_LABEL_STRUCT to 
      //DIR_DIRECTORY_LABEL_STRUCT although this is not a very nice solution 
      //it HAS to be done because the compiler alligns the two structs differently 
      //so they can not be typecasted easily from LFN_LABEL_STRUCT to 
      //DIR_DIRECTORY_LABEL_STRUCT 
      /*******************************************************************************/
      convert_from_sfn_lfn(&tmpsfn_lbl, tmpstruct_p);
      /*******************************************************************************/        
      filedirloc_p->tLblPos = lblpos + (cntr * DIR_SIZE_META_ALL);
      res = DIR_ucSetMetaData (filedirloc_p, (DIR_DIRECTORY_LABEL_STRUCT *)tmpsfn_lbl_p);
      if(res != FFS_NO_ERRORS)
      { 
        RFSFAT_FREE_BUF(lfn_lbl_p);
        RFSFAT_SEND_TRACE_ERROR("RFSFAT: Error writing LFN label");
        return res;
      }
    }
  }
  RFSFAT_FREE_BUF(lfn_lbl_p);
  return res;
}

UINT8  is_all_ascii(const  T_WCHAR              *fileName_p)
{

  while(*fileName_p != EOSTR)
  	{
  	    if((*fileName_p)&0xFF00)
  	    {
			return 0;   /* All chars are not ascii */
  	    	}
		else
			{
			   /* Its ascii , then check is it valid LFN */
			   if(VALID_LFN_CHAR(((char)(*fileName_p))) == FALSE)
			   	{
			   	   return 0;   /* ASCII, but not LFN  */
			   	}
			}
		
		fileName_p++;  /* goto the next Unicode character  */
  	}

  return 1;  /* all characters are ascii */
}





/**************************************************/
/**************************************************/
UINT8 DIR_ucCreateFile_uc (
                  DIR_LABEL_LOCATION_STRUCT  * const pstLabelLocation,
                  DIR_DIRECTORY_LABEL_STRUCT * const pstMetaData,
                  const  T_WCHAR              *fileName_p)
{
  DIR_LABEL_LOCATION_STRUCT parentlblloc;
  UINT8                     short_file_name[FFS_MAX_SIZE_NAME_DOT_EXT];
  UINT8                     ucResult = FFS_NO_ERRORS;
  UINT8						*ascii_path=NULL;
  UINT16					len_path=0,lfncntr=0;


  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && (pstMetaData != NULL)),
             return DIR_RV_INVALID_PARAM);


  /* check the unicode character are all ascii or not */
  if(is_all_ascii(fileName_p))
  {
	 len_path = wstrlen(fileName_p)+1;  //Include the null char

	 RFSFAT_GETBUF(len_path, &ascii_path);

     /* convert to ascii */
	 convertU16ToU8(fileName_p,ascii_path);
	 
	 if (test_file_lfn (ascii_path))
  	 {
    //LFN
    RFSFAT_LFN_FLAG = TRUE;
    rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
    ucResult = rephr_lfn(0, ascii_path, RFSFAT_LFN, &lfncntr);
    if( (ucResult == FFS_RV_ERR_INVALID_PARAMS) || 
		    (ucResult == FFS_RV_ERR_INVALID_PATH) ||
        (lfncntr == 0))
    {
  	  RFSFAT_FREE_BUF(ascii_path);
      RFSFAT_SEND_TRACE_ERROR("RFSFAT (Uc): invalid creation name");
      return ucResult;
    } 
  	} else {
    	RFSFAT_LFN_FLAG = FALSE;
    	if (ffs_ucRephraseNameAndExt (POSITION_FIRST_CHAR_IN_PATH, ascii_path, pstMetaData) ==
      			POSITION_FIRST_CHAR_IN_PATH)
    		{
				RFSFAT_FREE_BUF(ascii_path);
			  	RFSFAT_SEND_TRACE_ERROR("RFSFAT (Uc): FFS_RV_ERR_INVALID_NAMEEXT");
      			return FFS_RV_ERR_INVALID_NAMEEXT;
    		}
  	}

	RFSFAT_FREE_BUF(ascii_path);


    /* create file as per ASCII */ 
	  return DIR_ucCreateFile(pstLabelLocation,pstMetaData);
  }



  /****************************************************************************/
  /* Search for a free place in directory table.                              */
  /****************************************************************************/

  ucResult = lfn_new_label_pos_uc (pstLabelLocation, fileName_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: lfn_new_label_pos failed");
    return ucResult;
  }


  //Created shortname that is not in use (the ~ stuf).
  //The ASCII long name from RFSFAT_LFN is used as input
  // (in earlyer stage the unicode was converted to U8 in it)
  parentlblloc = *pstLabelLocation;
   memset(short_file_name,0x00,FFS_MAX_SIZE_NAME_DOT_EXT);
  ucResult = create_short_name_uc(&parentlblloc, short_file_name, fileName_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_short_name failed");
    return ucResult;
  }

  //create label
  pstMetaData->ucAttribute = DEFAULT_FILE_ATTRIBUTE;
  pstMetaData->uiFirstClusterNumber = DEFAULT_FILE_FIRST_CLUSTER;
  pstMetaData->uiFirstClusterNumberHigh = DEFAULT_FILE_FIRST_CLUSTER;
  pstMetaData->ulFileSize = DEFAULT_FILE_SIZE;


  ucResult = create_write_label_uc(pstLabelLocation, pstMetaData, short_file_name, fileName_p);
  if(ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_write_label file label failed");
    return ucResult;
  }
  return ucResult;
}

  /****************************************************************************/
  /****************************************************************************/
//srclblloc_p will point to the 8+3 label Even when part of a LFN
//dstlblloc_p will point to the 8+3 label Even when part of a LFN
UINT8 DIR_lfn_move_uc( const  T_WCHAR *  origName,
                      const  T_WCHAR *  newName,
                      const DIR_LABEL_LOCATION_STRUCT* srclblloc_p,
                      DIR_LABEL_LOCATION_STRUCT* dstlblloc_p,
                      DIR_DIRECTORY_LABEL_STRUCT* metdat_p)
{
  DIR_DIRECTORY_LABEL_STRUCT orgnamelfn_metdat;
  DIR_DIRECTORY_LABEL_STRUCT* orgmetdat_p;
  DIR_LABEL_LOCATION_STRUCT tmplblloc;
  UINT8 short_file_name[FFS_MAX_SIZE_NAME_DOT_EXT];
  UINT8 res = FFS_NO_ERRORS;
  UINT32 clusterNmb;
  UINT8			*ascii_path=NULL;
  UINT16		len_path=0,lfncntr=0;
  UINT8         ucResult = FFS_NO_ERRORS;
  


  if(is_all_ascii(newName))
  	{
       /* all newmname characters are ASCII */
      len_path = wstrlen(newName)+1;  //Include the null char

	 RFSFAT_GETBUF(len_path, &ascii_path);

     /* convert to ascii */
	 convertU16ToU8(newName,ascii_path);
	 
	 if (test_file_lfn (ascii_path))
  	 {
    //LFN
    RFSFAT_LFN_FLAG = TRUE;
    rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
    ucResult = rephr_lfn(0, ascii_path,RFSFAT_LFN, &lfncntr);
    if( (ucResult == FFS_RV_ERR_INVALID_PARAMS) || 
		    (ucResult == FFS_RV_ERR_INVALID_PATH) ||
        (lfncntr == 0))
    {
  	  RFSFAT_FREE_BUF(ascii_path);
      RFSFAT_SEND_TRACE_ERROR("RFSFAT (Uc): invalid creation name");
      return ucResult;
    } 
  	} else {
    	RFSFAT_LFN_FLAG = FALSE;
    	if (ffs_ucRephraseNameAndExt (POSITION_FIRST_CHAR_IN_PATH, ascii_path,metdat_p) == POSITION_FIRST_CHAR_IN_PATH)
    		{
				RFSFAT_FREE_BUF(ascii_path);
			  	RFSFAT_SEND_TRACE_ERROR("RFSFAT (Uc): FFS_RV_ERR_INVALID_NAMEEXT");
      			return FFS_RV_ERR_INVALID_NAMEEXT;
    		}
  	}

	RFSFAT_FREE_BUF(ascii_path);

	/* use DIR_lfn_move */

	return DIR_lfn_move(TRUE,RFSFAT_LFN_FLAG,FALSE,srclblloc_p, dstlblloc_p, metdat_p);
  	}



  orgmetdat_p = &orgnamelfn_metdat;
  
  /* 
   * both old and new name are UNICODE
   * RENAME (and move?)
   */
  //get metadata of orgnamelfn
  GET_META_DATA(srclblloc_p, orgmetdat_p);

  memset(short_file_name,0x00,(FFS_MAX_SIZE_NAME_DOT_EXT));
  //create enough label space in new dir table if there is a newname then use 
  //the newname to calculate the required size. Otherwise use to original name.
 
  res = lfn_new_label_pos_uc(dstlblloc_p, newName);
 
  if(res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: lfn_new_label_pos in DIR_lfn_move failed");
    return res;
  }

  //create and write new label in new path
  tmplblloc = *dstlblloc_p;
  //first create sfn

   res = create_short_name_uc(&tmplblloc, short_file_name, newName);

   if (res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_short_name in DIR_lfn_move_uc failed");
    return res;
  }
 
  res = create_write_label_uc(dstlblloc_p,metdat_p,short_file_name, newName);
 
  
  if(res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_write_label in DIR_lfn_move_uc failed");
    return res;
  }
  
  //replace label
  CPY_LBL_TO_LBL(DIR_META_DATA_P, orgmetdat_p);
  res = ucSetMetaData(dstlblloc_p, META_TAG_REPLACE);
  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData in DIR_lfn_move_uc failed");
    return res;
  }
  //if dir && move
    //the entry ".." must point at the new upper directory.            
  clusterNmb = DIR_CLUSTER_NMB_2_UINT32_NMB(DIR_META_DATA_P);
  res = dotdot_new_parent(orgmetdat_p, srclblloc_p->mpt_idx, 
                          dstlblloc_p->uiFirstCl, clusterNmb);
  if (res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: dotdot_new_parent in DIR_lfn_move_uc failed");
    return res;
  }
  //remove orgnamelfn entries
  res = DIR_ucDeleteLabel_only(srclblloc_p);
  if(res != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_ucDeleteLabel in DIR_lfn_move_uc failed");
  }

  return res;
}


/**************************************************/
/**************************************************/
UINT8 DIR_ucCreateDir_uc (
          RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabelLocation,
          RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData,
          const  T_WCHAR                     *dirName_p)
{
  DIR_LABEL_LOCATION_STRUCT   *tmplblloc_p,tmplblloc;
  DIR_LABEL_LOCATION_STRUCT   parentlblloc;
  UINT16                      uiCnt;
  UINT16                      uiNmbDirEntriesInCluster;
  UINT8                       ucResult = FFS_NO_ERRORS;
  UINT8                       short_file_name[FFS_MAX_SIZE_NAME_DOT_EXT];
  UINT8						*ascii_path=NULL;
  UINT16					len_path=0,lfncntr=0;



  /****************************************************************************/
  /* Check input parameters.                                                  */
  /****************************************************************************/
  RFSFAT_ASSERT_LEAVE (((pstLabelLocation != NULL) && (pstMetaData != NULL)),
             return DIR_RV_INVALID_PARAM);

 /* check the unicode character are all ascii or not */
  if(is_all_ascii(dirName_p))
  {
	 len_path = wstrlen(dirName_p)+1;  //Include the null char

	 RFSFAT_GETBUF(len_path, &ascii_path);

     /* convert to ascii */
	 convertU16ToU8(dirName_p,ascii_path);
	 
	 if (test_file_lfn (ascii_path))
  	 {
    //LFN
    RFSFAT_LFN_FLAG = TRUE;
    rfsfat_vpMemset(RFSFAT_LFN, 0x0000, LFN_DIR_SIZE_META_NAME);
    ucResult = rephr_lfn(0, ascii_path, RFSFAT_LFN, &lfncntr);
    if( (ucResult == FFS_RV_ERR_INVALID_PARAMS) || 
		    (ucResult == FFS_RV_ERR_INVALID_PATH) ||
        (lfncntr == 0))
    {
  	  RFSFAT_FREE_BUF(ascii_path);
      RFSFAT_SEND_TRACE_ERROR("RFSFAT (Uc): invalid directory creation name");
      return ucResult;
    } 
  	} else {
    	RFSFAT_LFN_FLAG = FALSE;
    	if (ffs_ucRephraseNameAndExt (POSITION_FIRST_CHAR_IN_PATH, ascii_path, pstMetaData) ==
      			POSITION_FIRST_CHAR_IN_PATH)
    		{
				RFSFAT_FREE_BUF(ascii_path);
			  	RFSFAT_SEND_TRACE_ERROR("RFSFAT (Uc): directory FFS_RV_ERR_INVALID_NAMEEXT");
      			return FFS_RV_ERR_INVALID_NAMEEXT;
    		}
  	}

	RFSFAT_FREE_BUF(ascii_path);


    /* create Directory as per ASCII */ 
	  return DIR_ucCreateDir(pstLabelLocation,pstMetaData);
  }


  /* Copy label location of directory.                                        */  
  tmplblloc_p= &tmplblloc;
  tmplblloc_p->mpt_idx = pstLabelLocation->mpt_idx;
  tmplblloc_p->uiFirstCl= pstLabelLocation->uiFirstCl;
  tmplblloc_p->tLblPos = pstLabelLocation->tLblPos;
  
  
  /****************************************************************************/
  /* Create new chain, containing the new directory table of the new          */
  /* directory.                                                               */
  /****************************************************************************/
  /* Get free entry in the neighbourhood of this entry.                       */

  ucResult = FAM_ucCreateNewChain (pstLabelLocation->mpt_idx,
                   &(pstLabelLocation->uiFirstCl));
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: FAM_ucCreateNewChain failed");
    return ucResult;
  }

  //Clear new directory cluster to ensure that for each entry 
  // at least the first byte is cleared (LAST_ENTRY indicator). Other wise old
  //  data can (and will) be seen as entries (when adding new elements).
  uiCnt = (UINT16) (MMA_BOOT_SECTOR[pstLabelLocation->mpt_idx].ucSectorPerCluster * 
                    MMA_BOOT_SECTOR[pstLabelLocation->mpt_idx].uiBytesPerSector);
  uiNmbDirEntriesInCluster = uiCnt / DIR_SIZE_META_ALL;
  /* clean as many entries as fit in a cluster */
  for(uiCnt = 0; uiCnt < uiNmbDirEntriesInCluster; uiCnt++)
  {
    pstLabelLocation->tLblPos = uiCnt * DIR_SIZE_META_ALL;

  /* Clear meta data. */
  (void) rfsfat_vpMemset ((void*)(DIR_META_DATA_P), DEFAULT_DIR_FIRST_CLUSTER,
              DIR_SIZE_META_ALL);

	
    ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
    if (ucResult != FFS_NO_ERRORS)
    {
      RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData Cleanup failed");
       goto error_label;
    }
  }


  /****************************************************************************/
  /* Create the entries in the new directory table (".", ".." and             */
  /* "LAST_ENTRY").                                                           */
  /****************************************************************************/
  /* Clean up given meta data.                                                */
  pstMetaData->uiFirstClusterNumberHigh = pstLabelLocation->uiFirstCl>>16;
  pstMetaData->uiFirstClusterNumber = (UINT16)pstLabelLocation->uiFirstCl;
  pstMetaData->ucAttribute = DEFAULT_DIR_ATTRIBUTE;
  pstMetaData->ulFileSize = DEFAULT_DIR_SIZE;


  /* Copy given meta data. (Time, date etc. will be used in all labels        */
  /* that will be created.)                                                   */
  CPY_LBL_TO_LBL(DIR_META_DATA_P, pstMetaData);
  
  DIR_META_DATA.uiFirstClusterNumber = 0;
  DIR_META_DATA.uiFirstClusterNumberHigh = 0;

  /****************************************************************************/
  /* Add entry 'LAST_ENTRY at position 64.                                    */
  /****************************************************************************/
  pstLabelLocation->tLblPos = POS_META_DATA_LAST_ENTRY;

  /* Copy name and extension "" (last entry).                                 */
  //UGLY however functional, the extention member is implicitely written here 
  (void) rfsfat_vpMemset (DIR_META_DATA.aucName, 0,DIR_SIZE_META_NAME);
  (void) rfsfat_vpMemset (DIR_META_DATA.aucExtension, 0, DIR_SIZE_META_EXT);

  RFSFAT_SEND_TRACE
  ("RFSFAT: write LAST_ENTRY in the third dir entry of new dir",
   RV_TRACE_LEVEL_DEBUG_LOW);
  /* Write to medium.                                                         */
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
     goto error_label;
  }
  /****************************************************************************/
  /* Add entry "." at position 0.                                             */
  /****************************************************************************/
  pstLabelLocation->tLblPos = 0;

 /* Copy name and extension "" (last entry).                                 */
 /* COPY ASCII_SPACE character */
  (void) rfsfat_vpMemset (DIR_META_DATA.aucName, ASCII_SPACE,DIR_SIZE_META_NAME);
  (void) rfsfat_vpMemset (DIR_META_DATA.aucExtension, ASCII_SPACE, DIR_SIZE_META_EXT);
  
  DIR_META_DATA.aucName[0] = ASCII_DOT;

  /* Copy first cluster number.                                               */
  DIR_META_DATA.uiFirstClusterNumber = pstMetaData->uiFirstClusterNumber;
  DIR_META_DATA.uiFirstClusterNumberHigh = pstMetaData->uiFirstClusterNumberHigh;
  /* Write to medium.                                                         */
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
     goto error_label;
  }

  /****************************************************************************/
  /* Add entry ".." at position 32.                                           */
  /****************************************************************************/
  pstLabelLocation->tLblPos = DIR_SIZE_META_ALL;

  /* Copy name and extension ".."                                             */
  DIR_META_DATA.aucName[1] = ASCII_DOT;


  /* Copy first cluster number.                                               */
  DIR_META_DATA.uiFirstClusterNumber = tmplblloc_p->uiFirstCl;
  DIR_META_DATA.uiFirstClusterNumberHigh = tmplblloc_p->uiFirstCl>>16;
  /* Write to medium.                                                         */
  ucResult = ucSetMetaData (pstLabelLocation, META_TAG_ALL);

  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: ucSetMetaData failed");
     goto error_label;
   }

  /****************************************************************************/
  /* Add entry for new directory to the given directory table.                */
  /****************************************************************************/
  parentlblloc = *tmplblloc_p;

  ucResult = lfn_new_label_pos_uc (tmplblloc_p, dirName_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: lfn_new_label_pos failed");
    goto error_label;
  }

  //Created shortname that is not in use (the ~ stuf).
  //The ASCII long name from RFSFAT_LFN is used as input
  // (in earlyer stage the unicode was converted to U8 in it)
  memset(short_file_name,0x00,(FFS_MAX_SIZE_NAME_DOT_EXT));
  ucResult = create_short_name_uc(&parentlblloc, short_file_name, dirName_p);
  if (ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_short_name failed (dir,uc)");
     goto error_label;
  }

  RFSFAT_SEND_TRACE_DEBUG_HIGH("the short file name :...");
  RFSFAT_SEND_TRACE_DEBUG_HIGH((char*)short_file_name);
  
  //create label
  pstMetaData->ucAttribute = DEFAULT_DIR_ATTRIBUTE;
  pstMetaData->ulFileSize = DEFAULT_DIR_SIZE;
  ucResult = create_write_label_uc(tmplblloc_p, pstMetaData, short_file_name, dirName_p);
  if(ucResult != FFS_NO_ERRORS)
  {
    RFSFAT_SEND_TRACE_ERROR("RFSFAT: create_write_label file label failed");
    goto error_label;
  }


  return ucResult;

error_label:
    FAM_ucDelLinkedList(pstLabelLocation->mpt_idx, pstLabelLocation->uiFirstCl);
	return ucResult;
}






/**************************************************/
// end of Unicode       
/**************************************************/
#endif


