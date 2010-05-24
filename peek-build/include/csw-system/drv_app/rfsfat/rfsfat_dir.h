/**
 * @file	DIR.h
 *
 * The directory handler is responsible for adding, removing
 * and searching directory tables and labels of a directory
 * table. The directory labels are written in the FAT16
 * format
 *
 * @author	Anton van Breemen
 * @version 0.1 This file inherited from a ICT re-use project
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	01/26/2004	Anton van Breemen		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 *	04/29/2004	E. Oude Middendorp		FAT32 added.
 */

#ifndef DIR_H
#define DIR_H

/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/

/* Before this file is included, the file ffs.h must be included to be sure   */
/* the different feature compiler directives are defined.                     */
#ifndef FFS_FAT_H
#error "ERROR: The file rfsfat_ffs.h must be included before this file."
#endif

#define	ROOT_DIR_INODE_NUMBER		-1
/* Last return value. Be sure this value is up to date!                       */
#define DIR_RV_VALUE_LAST          DIR_RV_DIRTABLE_FULL

#define DIR_SIZE_META_TIME         0x02
#define DIR_SIZE_META_DATE         0x02
#define DIR_SIZE_META_CLUSTER_HIGH 0x02
#define DIR_SIZE_META_CLUSTER      0x02
#define DIR_SIZE_META_SIZE         0x04
#define DIR_SIZE_META_C_TIME_TENTH 0x01
#define DIR_SIZE_META_C_TIME	   DIR_SIZE_META_TIME
#define DIR_SIZE_META_C_DATE       DIR_SIZE_META_DATE
#define DIR_SIZE_META_LA_DATE      DIR_SIZE_META_DATE
#define DIR_SIZE_META_ALL          ( DIR_SIZE_META_NAME + DIR_SIZE_META_EXT +  			\
                                     DIR_SIZE_META_ATTR + DIR_SIZE_META_LCASE + 		\
                                     DIR_SIZE_META_C_TIME_TENTH + DIR_SIZE_META_C_TIME+ \
                                     DIR_SIZE_META_C_DATE + DIR_SIZE_META_LA_DATE+ 		\
                                     DIR_SIZE_META_TIME + DIR_SIZE_META_DATE + 			\
                                     DIR_SIZE_META_CLUSTER +                   			\
                                     DIR_SIZE_META_CLUSTER_HIGH + DIR_SIZE_META_SIZE)

/* First cluster nr of root dir.      */
#define DIR_CLUSTER_NR_ROOT_DIRECTORY                                       0x00
/* Offset with table of first entry.  */
#define DIR_OFFSET_FIRST_ENTRY                                              0x00
/* Offset with table of first real entry. (Not counting the '.' and '..'      */
/* entries.                                                                   */
#define DIR_OFFSET_FIRST_REAL_ENTRY    (2*DIR_SIZE_META_ALL)

#define LFN_2CHR  2
#define LFN_5CHR  5
#define LFN_6CHR  6
/*                                                                            */
/* Default values.                                                            */
/*                                                                            */
#define DEFAULT_FILE_ATTRIBUTE     0x00 /* Default attribute for a new        */
                    /* created file.                      */
#define DEFAULT_FILE_FIRST_CLUSTER 0x00 /* Default first cluster number for   */
                    /* a new created file.                */
#define DEFAULT_FILE_SIZE          0x00 /* Default file size for a new        */
                    /* created file.                      */
#define DEFAULT_FILE_RESERVED      0x00 /* Default reserved section value     */
                    /* for a new created file.            */

#define DEFAULT_DIR_ATTRIBUTE      0x10 /* Default attribute for a new        */
                    /* created directory.                 */
#define DEFAULT_DIR_FIRST_CLUSTER  0x00 /* Default first cluster number for   */
                    /* a new created file.                */

#define DEFAULT_DIR_SIZE           0x00 /* Default directory size for a new   */
                    /* created directory.                 */

#define DEFAULT_LCASE_VALUE		   0x00

#define DIR_MASK           0x10 /* Bit mask, used to test if attribute        */
                /* indicates meta data is from a directory.   */

#define META_MARK_EMPTY    0xE5 /* Value of first character of name in        */
                /* file's meta-data indicating the meta       */
                /* data is not used.                          */
#define META_MARK_LAST     0x00 /* Value of first character of name in file's */
                /* meta-data indicating the meta data is not  */
                /* used, and the last label of this table.    */

#define META_TAG_NAME      0x01 /* Defines, used to select one of the         */
#define META_TAG_EXT       0x02 /* fields in a directory label.               */
#define META_TAG_ATTR      0x04
#define META_TAG_RESERVED  0x08
#define META_TAG_TIME      0x10
#define META_TAG_DATE      0x20
#define META_TAG_CLUSTER   0x40
#define META_TAG_SIZE      0x80
#define META_TAG_ALL       0xFF

#define META_TAG_ALL_BUT_NAME ( META_TAG_EXT | META_TAG_ATTR |                \
                                META_TAG_RESERVED | META_TAG_TIME |           \
                                META_TAG_DATE | META_TAG_CLUSTER |            \
                                META_TAG_SIZE )

#define META_TAG_REPLACE      ( META_TAG_ATTR | META_TAG_RESERVED |           \
                                META_TAG_TIME | META_TAG_DATE | 			  \
                                META_TAG_CLUSTER | META_TAG_SIZE )

/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/
#define DIR_META_DATA (rfsfat_env_ctrl_blk_p->ffs_globals.stMetaData)
#define VALID_NEXT_LABEL(next_label) ((FFS_MAX_VALUE_FILE_LENGTH_TYPE - \
                                        next_label) < DIR_SIZE_META_ALL) 
                  
#define NEXT_LABEL(p)   {p += DIR_SIZE_META_ALL; if(VALID_NEXT_LABEL(p)) \
                         {RFSFAT_SEND_TRACE_ERROR("RFSFAT: DIR_RV_DIRTABLE_FULL"); \
                          return DIR_RV_DIRTABLE_FULL;}}

#define DIR_META_DATA_P (&(DIR_META_DATA))

/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/
typedef struct
{
  FFS_FILE_LENGTH_TYPE tLblPos;	/* Position of label in directory table.      */
  UINT32 uiFirstCl;				/* First cluster number of directory table.   */
  UINT8 mpt_idx;				/* index in mountpoint table.                 */
  UINT8 pad_1;                  /* padding on remaing bytes */
  UINT8	pad_2;
  UINT8 pad_3;
} DIR_LABEL_LOCATION_STRUCT;

typedef struct
{
  UINT8   lfn_order;                //1 
  UINT8   lfn_attr;                 //1
  UINT8   lfn_type;                 //1
  UINT8   lfn_crc;                  //1
  UINT16  lfn_5char_str[LFN_5CHR];  //10
  UINT16  lfn_6char_str[LFN_6CHR];  //12
  UINT16  lfn_firstcl;              //2
  UINT16  lfn_2char_str[LFN_2CHR];  //4
 /* UINT16  lbn_pad;                  //2  */
} LFN_LABEL_STRUCT;

typedef enum
{
  NO_ERRORS = 200,
  WRONG_CRC,
  MEM_ERROR,
  LFN_EMPTY_LABEL,
  NOT_A_VALID_LFN_LABEL,
  COPY_ERROR
}DIR_LFN_RET;
/******************************************************************************
 *     Variable Declaration                                                   *
 ******************************************************************************/

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
UINT8 DIR_ucGetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
						 pstLabelLocation,
						 RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)
						 pstMetaData);

UINT8 DIR_ucSetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
						 pstLabelLocation,
						 RO_PTR_TO_RO (DIR_DIRECTORY_LABEL_STRUCT)
						 pstMetaData);

UINT8 DIR_ucCreateFile (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
						pstLabelLocation,
						RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)
						pstMetaData);

UINT8 DIR_ucDeleteLabel (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
						 pstLabelLocation);

#ifdef FFS_FEATURE_DIRECTORIES
UINT8 DIR_ucIsEmpty (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
					 pstLabelLocation);
#endif

UINT8 DIR_ucGetLabel (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
					  pstLabelLocation,
					  RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData);

#ifdef FFS_FEATURE_DIRECTORIES
UINT8 DIR_ucCreateDir (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
					   pstLabelLocation,
					   RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData);
#endif

UINT8 DIR_lfn_move( BOOL rename_flg,
                    BOOL newnamelfn, 
                    BOOL orgnamelfn,
                    const DIR_LABEL_LOCATION_STRUCT* srclblloc_p,
                    DIR_LABEL_LOCATION_STRUCT* dstlblloc_p,
                    DIR_DIRECTORY_LABEL_STRUCT* metdat_p);
                    
UINT8 DIR_ucMove (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
				  pstSrcLabelLocation,
				  RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
				  pstDstLabelLocation);

UINT8 DIR_ucClearRoot (RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)
					   pstLabelLocation, UINT8 * name);


void DIR_cpy_lbl_str(UINT8* shortname, const UINT8* orgname, const UINT8* orgext);
UINT8 DIR_find_lfn_of_sfn(const DIR_LABEL_LOCATION_STRUCT* sfn_loc, 
                          UINT8* nr_found_entries);
UINT8 DIR_read_lfn_from_label( DIR_LABEL_LOCATION_STRUCT* const labelloc_p, 
                                  DIR_LFN_RET* retval);
UINT8 DIR_crc_calculate(UINT8* shortname);

UINT8 convert_from_lfn_sfn( LFN_LABEL_STRUCT* lfnlbl_p, 
                            DIR_DIRECTORY_LABEL_STRUCT* sfnlbl_p);
UINT8 convert_from_sfn_lfn( DIR_DIRECTORY_LABEL_STRUCT* sfnlbl_p, 
                            LFN_LABEL_STRUCT* lfnlbl_p);
UINT8 ucSetMetaData (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT)
              pstLabelLocation, UINT8 ucSelection);


UINT8 DIR_ucTruncLabel (RO_PTR_TO_RO (DIR_LABEL_LOCATION_STRUCT) pstLabelLocation, T_FSCORE_SIZE Newsize);



#if (FFS_UNICODE ==1)
/**************************************************/
// Unicode       
/**************************************************/

UINT8 DIR_ucGetLabel_uc (
                const T_WCHAR                      *pathName_p,
                RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabelLocation,
                RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData);

UINT8 create_write_label_uc(
                         DIR_LABEL_LOCATION_STRUCT  * const filedirloc_p,
                         DIR_DIRECTORY_LABEL_STRUCT * const pstMetaData,
                         UINT8* const                 filedir_name,
                         const  T_WCHAR      * filedir_name_uc);



UINT8  is_all_ascii(const  T_WCHAR              *fileName_p);
	
UINT8 DIR_ucCreateFile_uc (
                  DIR_LABEL_LOCATION_STRUCT  * const pstLabelLocation,
                  DIR_DIRECTORY_LABEL_STRUCT * const pstMetaData,
                  const  T_WCHAR              *fileName_p);

UINT8 DIR_ucCreateDir_uc (
          RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)  pstLabelLocation,
          RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetaData,
          const  T_WCHAR                     *dirName_p);

UINT8 DIR_lfn_move_uc( const  T_WCHAR *  origName,
                      const  T_WCHAR *  newName,
                      const DIR_LABEL_LOCATION_STRUCT* srclblloc_p,
                      DIR_LABEL_LOCATION_STRUCT* dstlblloc_p,
                      DIR_DIRECTORY_LABEL_STRUCT* metdat_p);


/**************************************************/
// end of Unicode       
/**************************************************/
#endif


#endif
