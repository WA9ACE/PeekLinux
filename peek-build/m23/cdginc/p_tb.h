/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_tb.h                                                         |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\tb.pdf"                |
| LastModified : "2001-07-17"                                              |
| IdAndVersion : "8441.112.01.002"                                         |
| SrcFileTime  : "Thu Nov 29 09:54:48 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:59 2009                   |
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


#ifndef P_TB_H
#define P_TB_H


#define CDG_ENTER__P_TB_H

#define CDG_ENTER__FILENAME _P_TB_H
#define CDG_ENTER__P_TB_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_TB_H__LAST_MODIFIED _2001_07_17
#define CDG_ENTER__P_TB_H__ID_AND_VERSION _8441_112_01_002

#define CDG_ENTER__P_TB_H__SRC_FILE_TIME _Thu_Nov_29_09_54_48_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_TB_H

#undef CDG_ENTER__FILENAME


#include "p_tb.val"

#ifndef __T_tb_meas_result__
#define __T_tb_meas_result__
/*
 * 
 * CCDGEN:WriteStruct_Count==3100
 */
typedef struct
{
  U16                       arfcn;                    /*<  0:  2> channel number                                     */
  U16                       rxlev;                    /*<  2:  2> measurement value                                  */
  U8                        num_meas;                 /*<  4:  1> number                                             */
  U8                        _align0;                  /*<  5:  1> alignment                                          */
  U8                        _align1;                  /*<  6:  1> alignment                                          */
  U8                        _align2;                  /*<  7:  1> alignment                                          */
} T_tb_meas_result;
#endif


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_TB_MEAS_IND__
#define __T_TB_MEAS_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==3101
 */
typedef struct
{
  T_tb_meas_result          tb_meas_result[TB_BA_LIST_SIZE]; /*<  0:264>                                                    */
} T_TB_MEAS_IND;
#endif

#ifndef __T_TB_RXLEV_SC_REQ__
#define __T_TB_RXLEV_SC_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==3102
 */
typedef struct
{
  U8                        sc_rxlev;                 /*<  0:  1> RXLEV value of the serving cell.                   */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_TB_RXLEV_SC_REQ;
#endif


#include "CDG_LEAVE.h"


#endif
