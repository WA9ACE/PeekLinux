/**
 * @file	rfsfat_mem_operations.h
 *
 * header file for rfsfat_mem_operations.c
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @verrfsfatn 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

/******************************************************************************
 *     Includes                                                               *
 ******************************************************************************/

/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
void *rfsfat_vpMemmove
  (void *vpDestMem, RW_PTR_TO_RO (void) vpSourceMem, UINT16 uiCount);

void *rfsfat_vpMemcpy
  (void *  vpDestMem, RW_PTR_TO_RO (void) vpSourceMem, UINT16 uiCount);

UINT16 rfsfat_iMemcmp
  (RW_PTR_TO_RO (void) vpMemBuf1,
   RW_PTR_TO_RO (void) vpMemBuf2, UINT16 uiCount);

void *rfsfat_vpMemset (void *  vpDestMem, UINT16 uiMemsetValue, UINT16 uiCount);

UINT16 rfsfat_uiStrlen (RW_PTR_TO_RO (UINT8) pucString);

UINT8 *rfsfat_ucStrcpy
  (UINT8 *  pucDestStr, const UINT8* const pucSourceStr);

UINT8 *rfsfat_ucStrncpy
  (UINT8 * pucDestStr, RW_PTR_TO_RO (UINT8) pucSourceStr, UINT16 uiCount);

UINT16 rfsfat_iStrcmp
  (RW_PTR_TO_RO (UINT8) pucString1, RW_PTR_TO_RO (UINT8) pucString2);

UINT16 rfsfat_iStrncmp
  (RW_PTR_TO_RO (UINT8) pucString1,
   RW_PTR_TO_RO (UINT8) pucString2, UINT16 uiCount);

UINT16 rfsfat_strlen_uint16( const UINT16* const str_p);




#if (FFS_UNICODE == 1)

/**************************************************/
// Unicode      
/**************************************************/
UINT8  rfsfat_conversionUnicodeToU8(
                    const T_WCHAR  *fileName_p,
                    char                  *u8Name_p);

void convertUcToU8(const  T_WCHAR *mp_uc, UINT8 *mp);

void convertU8ToUc(const  UINT8 *mp,T_WCHAR *mp_uc );

/**************************************************/
// end of Unicode      
/**************************************************/

#endif


