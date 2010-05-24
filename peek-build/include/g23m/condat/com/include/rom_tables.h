/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-PS
|  Modul   :  DRV_ROM_TABLES
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This Module defines the operator list tables in ROM
+-----------------------------------------------------------------------------
*/

#ifndef ROM_TABLES_H
#define ROM_TABLES_H


/*==== INCLUDES ===================================================*/

/* The swconfig.cfg is genarated file and we are including here to remove
   multiple inclusions in other .c files */

#include "swconfig.cfg"


/* NHK:
 * Once the tables plmn_compressed, plmn_dict_offset and plmn_dict go to ROM, their names below should match corresponding ROM addresses
 * KSR: Moved to g23m\condat\com\src\drivers\rom_tables.c
 */
#ifdef TI_PS_OP_OPN_TAB_ROMBASED
EXTERN const UBYTE *const ptr_plmn_compressed;
EXTERN const USHORT *const ptr_plmn_dict_offset;
EXTERN const UBYTE *const ptr_plmn_dict;
#endif  /* TI_PS_OP_OPN_TAB_ROMBASED */ 

/* Changes for ROM data */
/* NHK:
 * Once the tables chset and gsmToAsciiTable  go to ROM, their names below should match corresponding ROM addresses
 * KSR: Moved to g23m\condat\com\src\drivers\rom_tables.c
 */
#ifdef TI_PS_OP_CNV_TAB_ROMBASED
#define CSCS_CHSET_Chars 256
EXTERN const UBYTE (*const chset)[CSCS_CHSET_Chars];
EXTERN const UBYTE *const gsmToAsciiTable;
EXTERN const UBYTE *const hexVal;
#endif /* TI_PS_OP_CNV_TAB_ROMBASED */

#endif /* #ifndef ROM_TABLES_H */ 
