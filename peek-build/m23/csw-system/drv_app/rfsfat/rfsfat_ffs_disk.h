/**
 * @file	ffs_disk.h
 *
 * Header file with function prototypes of general file system
 * functions (like init and terminate) and functions for
 * manipulating/activating partitions (normally disks).
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
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#ifndef FFS_DISK_H
#define FFS_DISK_H

/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/
#define FFS_POS_COLON_IN_PATH                                                 1

/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/
#define FFS_STOP_IF_NOT_INITIALISED if(!rfsfat_env_ctrl_blk_p->ffs_globals.	  \
										ffs_bSystemInitDone)                  \
                                {                                             \
                                  RFSFAT_SEND_TRACE("RFSFAT: not initialised",\
									                  RV_TRACE_LEVEL_WARNING);				          \
                                  return FFS_RV_ERR_NOT_INITIALISED;          \
                                }
#define POS_partition_IN_PATH                                                0
#define ASCII_VALUE_A                                                       'A'
#define ASCII_VALUE_Z                                                       'Z'
#define ASCII_VALUE_LOWA                                                    'a'
#define ASCII_VALUE_LOWZ                                                    'z'
#define ASCII_VALUE_0                                                       '0'
#define ASCII_VALUE_9                                                       '9'
#define ASCII_VALUE_HASH                                                    '#'
#define ASCII_VALUE_DOLLAR                                                  '$'
#define ASCII_VALUE_PERCENT                                                 '%'
#define ASCII_VALUE_AMPERSANT                                               '&'
#define ASCII_VALUE_SINGLE_QUOTE                                           '\''
#define ASCII_VALUE_LEFT_PARENTHESES                                        '('
#define ASCII_VALUE_RIGHT_PARENTHESES                                       ')'
#define ASCII_VALUE_MINUS                                                   '-'
#define ASCII_VALUE_AT_SIGN                                                 '@'
#define ASCII_VALUE_DOT                                                     '.'
#define ASCII_VALUE_UNDERSCORE                                              '_'
#define ASCII_VALUE_TILDE                                                   '~'
#define ASCII_VALUE_SINGLE_OPEN_QUOTE                                       '`'
#define ASCII_VALUE_EXCLAMATION                                             '!'
#define ASCII_VALUE_LEFT_CURLY_BRACKET                                      '{'
#define ASCII_VALUE_RIGHT_CURLY_BRACKET                                     '}'
#define ASCII_VALUE_LEFT_SQUARE_BRACKET                                     '['
#define ASCII_VALUE_RIGHT_SQUARE_BRACKET                                    ']'
#define ASCII_VALUE_XOR                                                     '^'
#define ASCII_VALUE_EQUAL                                                   '='
#define ASCII_VALUE_PLUS                                                    '+'
#define ASCII_VALUE_COMMA                                                   ','
#define ASCII_VALUE_SEMICOLON                                               ';'
#define ASCII_EOSTR                                                        '\0'

#define VALID_SHORT_FN_CHAR(sfnchar)										   \
	   ((((sfnchar >= ASCII_VALUE_A) && (sfnchar <= ASCII_VALUE_Z) ) 		|| \
         ((sfnchar >= ASCII_VALUE_0) && (sfnchar <= ASCII_VALUE_9) ) 		|| \
		  (sfnchar == ASCII_VALUE_DOLLAR) 									|| \
          (sfnchar == ASCII_VALUE_PERCENT) 									|| \
          (sfnchar == ASCII_VALUE_DOT) 										|| \
          (sfnchar == ASCII_VALUE_MINUS)									|| \
          (sfnchar == ASCII_VALUE_UNDERSCORE)								|| \
          (sfnchar == ASCII_VALUE_TILDE)									|| \
          (sfnchar == ASCII_VALUE_SINGLE_QUOTE)								|| \
          (sfnchar == ASCII_VALUE_SINGLE_OPEN_QUOTE)						|| \
          (sfnchar == ASCII_VALUE_AT_SIGN)									|| \
          (sfnchar == ASCII_VALUE_EXCLAMATION)								|| \
          (sfnchar == ASCII_VALUE_LEFT_PARENTHESES)							|| \
          (sfnchar == ASCII_VALUE_RIGHT_PARENTHESES)						|| \
          (sfnchar == ASCII_VALUE_LEFT_CURLY_BRACKET)						|| \
          (sfnchar == ASCII_VALUE_RIGHT_CURLY_BRACKET)						|| \
          (sfnchar == ASCII_VALUE_XOR)										|| \
          (sfnchar == ASCII_VALUE_AMPERSANT)								|| \
          (sfnchar == ASCII_VALUE_HASH)) ? TRUE : FALSE)
                                            
#define VALID_LFN_CHAR(lfnchar)   											   \
		(((VALID_SHORT_FN_CHAR(lfnchar)) ? TRUE : 							   \
		((((lfnchar >= ASCII_VALUE_LOWA) && (lfnchar <= ASCII_VALUE_LOWZ)) 	|| \
		 (lfnchar == ASCII_VALUE_PLUS)										|| \
		 (lfnchar == ASCII_VALUE_COMMA)										|| \
		 (lfnchar == ASCII_VALUE_SEMICOLON)									|| \
		 (lfnchar == ASCII_VALUE_EQUAL)										|| \
		 (lfnchar == ASCII_VALUE_LEFT_SQUARE_BRACKET)						|| \
		 (lfnchar == ASCII_VALUE_RIGHT_SQUARE_BRACKET)						|| \
		 (lfnchar == FFS_ASCII_VALUE_SPACE)) ? TRUE : FALSE )))

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/

UINT8 ffs_ucGetLabel
  (RW_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc,
   RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT) pstMetDat);
UINT8 ffs_ucGetPartAndDirTableFromPath
  (RW_PTR_TO_RO (UINT8) pucPath,
   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc);
UINT16 ffs_ucRephraseNameAndExt
  (UINT16 ucStartPos,
   RW_PTR_TO_RO (UINT8) pucPath, 
   DIR_DIRECTORY_LABEL_STRUCT * prefill_label);

#if 0
UINT8 ffs_ucMakeNameDotExtension
  (RO_PTR_TO_RO (UINT8) pucNameExt, UINT8 * pucNameDotExt);
#endif

void ffs_ucCopyNameDotExtension
  (RO_PTR_TO_RO (DIR_DIRECTORY_LABEL_STRUCT) pstMetDat, UINT8 * pucNameDotExt);
void ffs_vCopyMetaInSmallLabel
  (RO_PTR_TO_RO (DIR_DIRECTORY_LABEL_STRUCT) pstMetDat,
   RO_PTR_TO_RW (FFS_SMALL_DIRECTORY_LABEL_STRUCT) pstSmallLabel);

UINT8 ffs_bIsNameValid (RW_PTR_TO_RO (UINT8) pucNameDotExt);
BOOL test_file_lfn (const UINT8* path);
UINT16 rephr_lfn(UINT16 startpos, const UINT8* origpath, UINT8*  name_ext, UINT16 *nrchars);

#if (FFS_UNICODE == 1)
/**************************************************/
// Unicode      
/**************************************************/
UINT16 rephr_lfn_uc( char* u8_name_p, char* lfn_name_p);

UINT8 ffs_ucGetLabel_uc(
                   const T_WCHAR                       *pathName_p,
                   RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT)   pstLabLoc,
                   RO_PTR_TO_RW (DIR_DIRECTORY_LABEL_STRUCT)  pstMetDat);


UINT8 ffs_ucGetPartAndDirTableFromPath_uc(
                    const T_WCHAR                     *pathName_p,
                    RO_PTR_TO_RW (DIR_LABEL_LOCATION_STRUCT) pstLabLoc);


void convertUcToSFN(const  T_WCHAR *mp_uc, UINT8 *mp);

void convertU16ToU8(const  T_WCHAR *mp_uc, UINT8 *mp);



/**************************************************/
// end of Unicode      
/**************************************************/

#endif





#endif
