/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_dti.h                                                        |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\dti.pdf"                 |
| LastModified : "2000-06-29"                                              |
| IdAndVersion : "8411.110.00.007"                                         |
| SrcFileTime  : "Thu Nov 29 09:40:22 2007"                                |
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


#ifndef P_DTI_H
#define P_DTI_H


#define CDG_ENTER__P_DTI_H

#define CDG_ENTER__FILENAME _P_DTI_H
#define CDG_ENTER__P_DTI_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_DTI_H__LAST_MODIFIED _2000_06_29
#define CDG_ENTER__P_DTI_H__ID_AND_VERSION _8411_110_00_007

#define CDG_ENTER__P_DTI_H__SRC_FILE_TIME _Thu_Nov_29_09_40_22_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_DTI_H

#undef CDG_ENTER__FILENAME


#include "p_dti.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_DTI_READY_IND__
#define __T_DTI_READY_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1454
 */
typedef struct
{
  U16                       tui;                      /*<  0:  2> transmission unit identifier                       */
  U8                        c_id;                     /*<  2:  1> channel identifier                                 */
  U8                        op_ack;                   /*<  3:  1> operation mode                                     */
} T_DTI_READY_IND;
#endif

#ifndef __T_DTI_GETDATA_REQ__
#define __T_DTI_GETDATA_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1455
 */
typedef struct
{
  U16                       tui;                      /*<  0:  2> transmission unit identifier                       */
  U8                        c_id;                     /*<  2:  1> channel identifier                                 */
  U8                        op_ack;                   /*<  3:  1> operation mode                                     */
} T_DTI_GETDATA_REQ;
#endif

#ifndef __T_DTI_DATA_REQ__
#define __T_DTI_DATA_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1456
 */
typedef struct
{
  U16                       tui;                      /*<  0:  2> transmission unit identifier                       */
  U8                        c_id;                     /*<  2:  1> channel identifier                                 */
  U8                        p_id;                     /*<  3:  1> protocol identifier                                */
  U8                        op_ack;                   /*<  4:  1> operation mode                                     */
  U8                        st_flow;                  /*<  5:  1> flow control state                                 */
  U8                        st_line_sa;               /*<  6:  1> line state sa                                      */
  U8                        st_line_sb;               /*<  7:  1> line state sb                                      */
  U8                        st_escape;                /*<  8:  1> escape state                                       */
  U8                        _align0;                  /*<  9:  1> alignment                                          */
  U8                        _align1;                  /*< 10:  1> alignment                                          */
  U8                        _align2;                  /*< 11:  1> alignment                                          */
  T_desc_list               desc_list;                /*< 12: ? > list of generic data descriptors                   */
} T_DTI_DATA_REQ;
#endif

#ifndef __T_DTI_DATA_IND__
#define __T_DTI_DATA_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1457
 */
typedef struct
{
  U16                       tui;                      /*<  0:  2> transmission unit identifier                       */
  U8                        c_id;                     /*<  2:  1> channel identifier                                 */
  U8                        p_id;                     /*<  3:  1> protocol identifier                                */
  U8                        op_ack;                   /*<  4:  1> operation mode                                     */
  U8                        st_flow;                  /*<  5:  1> flow control state                                 */
  U8                        st_line_sa;               /*<  6:  1> line state sa                                      */
  U8                        st_line_sb;               /*<  7:  1> line state sb                                      */
  U8                        st_escape;                /*<  8:  1> escape state                                       */
  U8                        _align0;                  /*<  9:  1> alignment                                          */
  U8                        _align1;                  /*< 10:  1> alignment                                          */
  U8                        _align2;                  /*< 11:  1> alignment                                          */
  T_desc_list               desc_list;                /*< 12: ? > list of generic data descriptors                   */
} T_DTI_DATA_IND;
#endif

#ifndef __T_DTI_DATA_TEST_REQ__
#define __T_DTI_DATA_TEST_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1458
 */
typedef struct
{
  U16                       tui;                      /*<  0:  2> transmission unit identifier                       */
  U8                        c_id;                     /*<  2:  1> channel identifier                                 */
  U8                        p_id;                     /*<  3:  1> protocol identifier                                */
  U8                        op_ack;                   /*<  4:  1> operation mode                                     */
  U8                        st_flow;                  /*<  5:  1> flow control state                                 */
  U8                        st_line_sa;               /*<  6:  1> line state sa                                      */
  U8                        st_line_sb;               /*<  7:  1> line state sb                                      */
  U8                        st_escape;                /*<  8:  1> escape state                                       */
  U8                        _align0;                  /*<  9:  1> alignment                                          */
  U8                        _align1;                  /*< 10:  1> alignment                                          */
  U8                        _align2;                  /*< 11:  1> alignment                                          */
  T_sdu                     sdu;                      /*< 12: ? > test data                                          */
} T_DTI_DATA_TEST_REQ;
#endif

#ifndef __T_DTI_DATA_TEST_IND__
#define __T_DTI_DATA_TEST_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1459
 */
typedef struct
{
  U16                       tui;                      /*<  0:  2> transmission unit identifier                       */
  U8                        c_id;                     /*<  2:  1> channel identifier                                 */
  U8                        p_id;                     /*<  3:  1> protocol identifier                                */
  U8                        op_ack;                   /*<  4:  1> operation mode                                     */
  U8                        st_flow;                  /*<  5:  1> flow control state                                 */
  U8                        st_line_sa;               /*<  6:  1> line state sa                                      */
  U8                        st_line_sb;               /*<  7:  1> line state sb                                      */
  U8                        st_escape;                /*<  8:  1> escape state                                       */
  U8                        _align0;                  /*<  9:  1> alignment                                          */
  U8                        _align1;                  /*< 10:  1> alignment                                          */
  U8                        _align2;                  /*< 11:  1> alignment                                          */
  T_sdu                     sdu;                      /*< 12: ? > test data                                          */
} T_DTI_DATA_TEST_IND;
#endif

#ifndef __T_DTI_DUMMY_REQ__
#define __T_DTI_DUMMY_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1460
 */
typedef struct
{
  T_desc                    desc;                     /*<  0: ? > generic data descriptor                            */
} T_DTI_DUMMY_REQ;
#endif


#include "CDG_LEAVE.h"


#endif
