/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_mmsms.h                                                      |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\mmsms.pdf"                   |
| LastModified : "2002-07-19"                                              |
| IdAndVersion : "6147.106.97.102"                                         |
| SrcFileTime  : "Thu Nov 29 09:47:08 2007"                                |
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


#ifndef P_MMSMS_H
#define P_MMSMS_H


#define CDG_ENTER__P_MMSMS_H

#define CDG_ENTER__FILENAME _P_MMSMS_H
#define CDG_ENTER__P_MMSMS_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_MMSMS_H__LAST_MODIFIED _2002_07_19
#define CDG_ENTER__P_MMSMS_H__ID_AND_VERSION _6147_106_97_102

#define CDG_ENTER__P_MMSMS_H__SRC_FILE_TIME _Thu_Nov_29_09_47_08_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_MMSMS_H

#undef CDG_ENTER__FILENAME


#include "p_mmsms.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_MMSMS_ESTABLISH_REQ__
#define __T_MMSMS_ESTABLISH_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1899
 */
typedef struct
{
  U8                        ti;                       /*<  0:  1> transaction identifier                             */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_MMSMS_ESTABLISH_REQ;
#endif

#ifndef __T_MMSMS_RELEASE_REQ__
#define __T_MMSMS_RELEASE_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1900
 */
typedef struct
{
  U8                        ti;                       /*<  0:  1> transaction identifier                             */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_MMSMS_RELEASE_REQ;
#endif

#ifndef __T_MMSMS_DATA_REQ__
#define __T_MMSMS_DATA_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1901
 */
typedef struct
{
  U8                        d1;                       /*<  0:  1> dummy, not used                                    */
  U8                        d2;                       /*<  1:  1> dummy, not used                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_MMSMS_DATA_REQ;
#endif

#ifndef __T_MMSMS_DATA_IND__
#define __T_MMSMS_DATA_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1902
 */
typedef struct
{
  U8                        d1;                       /*<  0:  1> dummy, not used                                    */
  U8                        d2;                       /*<  1:  1> dummy, not used                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_MMSMS_DATA_IND;
#endif

#ifndef __T_MMSMS_ERROR_IND__
#define __T_MMSMS_ERROR_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1903
 */
typedef struct
{
  U8                        ti;                       /*<  0:  1> transaction identifier                             */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U16                       cause;                    /*<  2:  2> MM cause                                           */
} T_MMSMS_ERROR_IND;
#endif

#ifndef __T_MMSMS_ESTABLISH_CNF__
#define __T_MMSMS_ESTABLISH_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==1904
 */
typedef struct
{
  U8                        ti;                       /*<  0:  1> transaction identifier                             */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_MMSMS_ESTABLISH_CNF;
#endif

#ifndef __T_MMSMS_ESTABLISH_IND__
#define __T_MMSMS_ESTABLISH_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1905
 */
typedef struct
{
  U8                        d1;                       /*<  0:  1> dummy, not used                                    */
  U8                        d2;                       /*<  1:  1> dummy, not used                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_MMSMS_ESTABLISH_IND;
#endif

#ifndef __T_MMSMS_RELEASE_IND__
#define __T_MMSMS_RELEASE_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1906
 */
typedef struct
{
  U8                        ti;                       /*<  0:  1> transaction identifier                             */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U16                       cause;                    /*<  2:  2> MM cause                                           */
} T_MMSMS_RELEASE_IND;
#endif

#ifndef __T_MMSMS_UNITDATA_IND__
#define __T_MMSMS_UNITDATA_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1907
 */
typedef struct
{
  U8                        d1;                       /*<  0:  1> dummy, not used                                    */
  U8                        d2;                       /*<  1:  1> dummy, not used                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_MMSMS_UNITDATA_IND;
#endif


#include "CDG_LEAVE.h"


#endif
