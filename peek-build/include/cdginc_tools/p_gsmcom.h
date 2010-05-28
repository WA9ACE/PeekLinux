/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_gsmcom.h                                                     |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\gsmcom.pdf"                    |
| LastModified : "2002-10-04"                                              |
| IdAndVersion : "8443.100.02.008"                                         |
| SrcFileTime  : "Thu Nov 29 09:43:26 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Oct 15 12:22:31 2008                   |
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


#ifndef P_GSMCOM_H
#define P_GSMCOM_H


#define CDG_ENTER__P_GSMCOM_H

#define CDG_ENTER__FILENAME _P_GSMCOM_H
#define CDG_ENTER__P_GSMCOM_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_GSMCOM_H__LAST_MODIFIED _2002_10_04
#define CDG_ENTER__P_GSMCOM_H__ID_AND_VERSION _8443_100_02_008

#define CDG_ENTER__P_GSMCOM_H__SRC_FILE_TIME _Thu_Nov_29_09_43_26_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_GSMCOM_H

#undef CDG_ENTER__FILENAME


#include "p_gsmcom.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_GSMCOM_DUMMY_REQ__
#define __T_GSMCOM_DUMMY_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1700
 */
typedef struct
{
  U8                        gsmcom_dummy;             /*<  0:  1> dummy, not used                                    */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_GSMCOM_DUMMY_REQ;
#endif


#include "CDG_LEAVE.h"


#endif