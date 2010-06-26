/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  UPM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    Output functions for primitives from UPM to the ACI entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "upm.h"

#include "upm_aci_output_handler.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : send_upm_count_cnf
+------------------------------------------------------------------------------
| Description : Allocate, pack and send UPM_COUNT_CNF primitive.
|
| Parameters  : nsapi            - NSAPI
|               ul_bytecount     - number of uplink octets transferred
|               ul_pktcount      - number of uplink packets transferred
|               dl_bytecount     - number of downlink octets transferred
|               dl_pktcount      - number of downlink packets transferred
+------------------------------------------------------------------------------
*/
void send_upm_count_cnf(U8 nsapi, U32 ul_bytecount, U32 dl_bytecount,
                        U32 ul_pktcount, U32 dl_pktcount)
{
  (void)TRACE_FUNCTION("send_upm_count_cnf");

  {
    PALLOC(prim, UPM_COUNT_CNF);

    TRACE_ASSERT(prim != NULL);

  /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->nsapi            = nsapi;
    prim->octets_uplink    = ul_bytecount;
    prim->octets_downlink  = dl_bytecount;
    prim->packets_uplink   = ul_pktcount;
    prim->packets_downlink = dl_pktcount;
 /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */

    (void)PSEND(hCommACI, prim);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : send_upm_dti_cnf
+------------------------------------------------------------------------------
| Description : Allocate, pack and send UPM_DTI_CNF primitive.
|
| Parameters  : dti_linkid       - DTI link ID
|               dti_conn         - connection type
+------------------------------------------------------------------------------
*/
void send_upm_dti_cnf(U32 dti_linkid, U8 dti_conn)
{
  (void)TRACE_FUNCTION("send_upm_dti_cnf");
  {
    PALLOC(prim, UPM_DTI_CNF);

    TRACE_ASSERT(prim != NULL);
  /*lint -e613 (Possible use of null pointer 'prim' in left argument to operator '->') */
    prim->dti_linkid = dti_linkid;
    prim->dti_conn   = dti_conn;
  /*lint +e613 (Possible use of null pointer 'prim' in left argument to operator '->') */		

    (void)PSEND(hCommACI, prim);
  }
}

/*==== END OF FILE ==========================================================*/
