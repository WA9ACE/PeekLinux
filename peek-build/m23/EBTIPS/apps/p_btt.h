/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_btt.h                                                        |
| SOURCE  : "y:\g23m\condat\ms\DFILE\PRIM\BTT.pdf"                         |
| LastModified : "2005-05-01"                                              |
| IdAndVersion : "..."                                                     |
| SrcFileTime  : "Tue May 10 11:55:55 2005"                                |
| Generated by CCDGEN_2.5.5 on Wed Jun 29 14:40:04 2005                    |
|           !!DO NOT MODIFY!!DO NOT MODIFY!!DO NOT MODIFY!!                |
+--------------------------------------------------------------------------+
*/

/* PRAGMAS
 * PREFIX                 : NONE
 * COMPATIBILITY_DEFINES  : NO (require PREFIX)
 * ALWAYS_ENUM_IN_VAL_FILE: NO
 * ENABLE_GROUP: NO
 * CAPITALIZE_TYPENAME: NO
 */


#ifndef P_BTT_H
#define P_BTT_H


#define CDG_ENTER__P_BTT_H

#define CDG_ENTER__FILENAME _P_BTT_H
#define CDG_ENTER__P_BTT_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_BTT_H__LAST_MODIFIED _2005_05_01
#define CDG_ENTER__P_BTT_H__ID_AND_VERSION _

#define CDG_ENTER__P_BTT_H__SRC_FILE_TIME _Tue_May_10_11_55_55_2005

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_BTT_H

#undef CDG_ENTER__FILENAME


#include "p_btt.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_BTT_CMD__
#define __T_BTT_CMD__
/*
 * 
 * CCDGEN:WriteStruct_Count==13098
 */
typedef struct
{
  U8                        app_id;                 /*<  0:  1> The message type                                   */
  U8                        buf[BT_MSG_MAX_BUF];      /*<  1:256> Buffer element                                     */
  U8                        zzz_align0;               /*<257:  1> alignment                                          */
  U8                        zzz_align1;               /*<258:  1> alignment                                          */
  U8                        zzz_align2;               /*<259:  1> alignment                                          */
} T_BTT_CMD;
#endif


#include "CDG_LEAVE.h"


#endif
