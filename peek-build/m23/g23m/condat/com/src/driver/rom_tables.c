#ifndef ROM_TABLES_C
#define ROM_TABLES_C

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "typedefs.h"

#include "rom_tables.h"

#ifdef TI_PS_OP_OPN_TAB_ROMBASED
GLOBAL const UBYTE *const ptr_plmn_compressed = (UBYTE *)PLMN_COMPRESSED_OPN_TABLE_ADDRESS;
GLOBAL const USHORT *const ptr_plmn_dict_offset =  (USHORT *)PLMN_OFFSET_OPN_TABLE_ADDRESS;
GLOBAL const UBYTE *const ptr_plmn_dict = (UBYTE *)PLMN_DICT_OPN_TABLE_ADDRESS;
#endif /* TI_PS_OP_OPN_TAB_ROMBASED */

#ifdef TI_PS_OP_CNV_TAB_ROMBASED
GLOBAL const UBYTE (*const chset)[CSCS_CHSET_Chars] = (UBYTE(*)[CSCS_CHSET_Chars])CHSET_ROM_TABLE_CONVERSION_ADDRESS;
GLOBAL const UBYTE *const gsmToAsciiTable = (UBYTE *) GSM_ASCII_ROM_TABLE_CONVERSION_ADDRESS;
GLOBAL const UBYTE *const hexVal = (UBYTE *) HEX_VALUE_ROM_TABLE_CONVERSION_ADDRESS;
#endif /*TI_PS_OP_CNV_TAB_ROMBASED */

#endif /* #ifndef ROM_TABLES_C */
