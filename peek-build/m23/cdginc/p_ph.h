/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_ph.h                                                         |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\ph.pdf"                |
| LastModified : "2001-10-26"                                              |
| IdAndVersion : "6147.112.01.100"                                         |
| SrcFileTime  : "Thu Nov 29 09:50:28 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:58 2009                   |
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


#ifndef P_PH_H
#define P_PH_H


#define CDG_ENTER__P_PH_H

#define CDG_ENTER__FILENAME _P_PH_H
#define CDG_ENTER__P_PH_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_PH_H__LAST_MODIFIED _2001_10_26
#define CDG_ENTER__P_PH_H__ID_AND_VERSION _6147_112_01_100

#define CDG_ENTER__P_PH_H__SRC_FILE_TIME _Thu_Nov_29_09_50_28_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_PH_H

#undef CDG_ENTER__FILENAME


#include "p_ph.val"

#include "p_mphc.h" 


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_PH_READY_TO_SEND__
#define __T_PH_READY_TO_SEND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2270
 */
typedef struct
{
  U8                        ch_type;                  /*<  0:  1> Layer 2 channel-type                               */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_PH_READY_TO_SEND;
#endif

#ifndef __T_PH_DATA_REQ__
#define __T_PH_DATA_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2271
 */
typedef struct
{
  U8                        ch_type;                  /*<  0:  1> Layer 2 channel-type                               */
  U8                        dummy;                    /*<  1:  1> dummy not used                                     */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  T_sdu                     sdu;                      /*<  4: ? > message unit                                       */
} T_PH_DATA_REQ;
#endif

#ifndef __T_PH_TRACE_IND__
#define __T_PH_TRACE_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2272
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> dummy not used                                     */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_PH_TRACE_IND;
#endif


#include "CDG_LEAVE.h"


#endif
