/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  This modul is part of the entity LLC and implements all 
|             procedures and functions as described in the 
|             SDL-documentation (TX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#ifndef LLC_TXF_C
#define LLC_TXF_C
#endif

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <string.h>     /* to get memcpy() */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "llc.h"        /* to get the global entity definitions */

#include "llc_f.h"      /* to get global functions, e.g. llc_generate_input */
#include "llc_txf.h"    /* to get the global entity definitions */
#include "llc_txp.h"    /* to get the function tx_cci_cipher_cnf */

#include "llc_uitxs.h"  /* to get signal interface to UITX */
#include "llc_itxs.h"   /* to get signal interface to ITX */

#ifndef TI_PS_OP_CIPH_DRIVER
#include "cci_fbsf.h"     /* to get functional interface */
#endif


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : tx_init
+------------------------------------------------------------------------------
| Description : This procedure initialises all necessary variables of send_pdu.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void llc_tx_init (void)
{
  TRACE_FUNCTION( "tx_init" );

  /*
   * Initialise TX with state NOT_READY.
   */
  INIT_STATE (TX, TX_TLLI_UNASSIGNED_NOT_READY);

  /*
   * Flush TX queue, if there is (in case of an LLC 
   * restart) old stuff queued.
   */
  tx_clear_buffer();

  return;
} /* tx_init() */


/*
+------------------------------------------------------------------------------
| Function    : tx_clear_buffer
+------------------------------------------------------------------------------
| Description : This procedure frees all buffered CCI_CIPHER_CNF primitives.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_clear_buffer (void)
{
  T_TX_QUEUE        *elem;
  T_TX_QUEUE        *elem_next;


  TRACE_FUNCTION( "tx_clear_buffer" );

  for (elem = llc_data->tx.queue; elem NEQ NULL; elem = elem_next)
  {
    elem_next = elem->next;

    /*
     * Free primitive if any is stored in queue element.
     */
    if (elem->primitive NEQ (ULONG)NULL)
    {
      PFREE (elem->primitive);
    }

    MFREE (elem);
  }

  llc_data->tx.queue = NULL;

  /*
   * Initialise UITX space counter for each SAPI.
   */
  llc_data->tx.queue_counter_uitx[0] = UITX_1_QUEUE_SIZE;
  llc_data->tx.queue_counter_uitx[1] = UITX_3_QUEUE_SIZE;
  llc_data->tx.queue_counter_uitx[2] = UITX_5_QUEUE_SIZE;
  llc_data->tx.queue_counter_uitx[3] = UITX_7_QUEUE_SIZE;
  llc_data->tx.queue_counter_uitx[4] = UITX_9_QUEUE_SIZE;
  llc_data->tx.queue_counter_uitx[5] = UITX_11_QUEUE_SIZE;

  /*
   * Initialise ITX space counter for each SAPI.
   */
  llc_data->tx.queue_counter_itx[0] = ITX_3_QUEUE_SIZE;
  llc_data->tx.queue_counter_itx[1] = ITX_5_QUEUE_SIZE;
  llc_data->tx.queue_counter_itx[2] = ITX_9_QUEUE_SIZE;
  llc_data->tx.queue_counter_itx[3] = ITX_11_QUEUE_SIZE;

  return;
} /* tx_clear_buffer() */

/*
+------------------------------------------------------------------------------
| Function    : tx_cipher_req
+------------------------------------------------------------------------------
| Description : Handles the function tx_cipher_req. This functions sets the
|               ciphering parameters and calls the ciphering driver function.
|
| Parameters  : todo
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_cipher_req
(
#ifdef LL_DESC
T_CCI_CIPHER_DESC_REQ *cipher_req
#else
T_CCI_CIPHER_REQ *cipher_req
#endif
)

{ 
  T_CIPH_init_cipher_req_parms  init_cipher_req_parms;
  T_CIPH_cipher_req_parms       cipher_req_parms;
  T_CIPH_in_data_list           in_data_list;
  T_CIPH_out_data               out_data;
  T_CIPH_ck                     ck;
  U16                           i;
  U8                            status;

  TRACE_FUNCTION( "tx_cipher_req" );

#ifdef LLC_TRACE_CIPHERING
  TRACE_EVENT("UPLINK NON CIPHERED DATA");
  llc_trace_desc_list3_content(cipher_req->desc_list3);
#endif
  /*
   * Copy pointer to desc's from CIPHER_REQ to the in_data_list
   * The in_data array in allocated dynamically in this func.
   */
  llc_copy_ul_data_to_list(cipher_req, &in_data_list);
  /*
   * Store ciphering parameters 
   */
  cipher_req_parms.gprs_parameters.pm              = cipher_req->pm;
  cipher_req_parms.gprs_parameters.header_size     = cipher_req->header_size;
  cipher_req_parms.gprs_parameters.ciphering_input = cipher_req->ciphering_input;
  cipher_req_parms.gprs_parameters.threshold       = 0;
  init_cipher_req_parms.direction = CIPH_UPLINK_DIR;
  init_cipher_req_parms.algo      = cipher_req->ciphering_algorithm;
  init_cipher_req_parms.ptr_ck = & ck;
  /*
   * Copy ciphering key
   */
  for (i=0; i<8;i++){
    init_cipher_req_parms.ptr_ck->ck_element[i] = cipher_req->kc.key[i];
  }

  {
    /* Use GRLC_DATA_REQ instead of CCI_CIPHER_CNF to avoid PPASS in LLC*/
    PALLOC_SDU (grlc_data_req, GRLC_DATA_REQ,
                (USHORT)(cipher_req->desc_list3.list_len*8 + FCS_SIZE_BITS));
  
    grlc_data_req->sdu.o_buf = 0;
    grlc_data_req->sdu.l_buf = 0;
    out_data.buf = (U32)(&grlc_data_req->sdu.buf[grlc_data_req->sdu.o_buf]);
    /*
     * Initialize ciphering driver and cipher data
     */
#ifdef TI_PS_OP_CIPH_DRIVER
    ciph_init_cipher_req (&init_cipher_req_parms, NULL);
    ciph_cipher_req (&cipher_req_parms, &in_data_list, &out_data, &status);
#else
    ciph_init_cipher_req_sim (&init_cipher_req_parms, NULL);
    ciph_cipher_req_sim (&cipher_req_parms, &in_data_list, &out_data, &status);
#endif

    /*
     * "Send" CIPHER_CNF to LLC
     */
    grlc_data_req->sdu.l_buf = out_data.len * 8;
    grlc_data_req->tlli = cipher_req->reference1;

#ifdef LLC_TRACE_CIPHERING
    TRACE_EVENT("UPLINK CIPHERED DATA");
    llc_trace_sdu(&grlc_data_req->sdu);
#endif
    tx_cci_cipher_cnf (grlc_data_req);

  }
      /*
       * Remove in use mark from the cipher request primitive
       */
  if (cipher_req != NULL) {
    {
      /*
       * Free cipher request, if not further used by other entities
       */
      if (cipher_req->attached_counter == CCI_NO_ATTACHE) {
#ifdef LL_DESC
        llc_cl_desc3_free((T_desc3*)cipher_req->desc_list3.first);
#endif /* LL_DESC */
      }
    }
    MFREE (cipher_req);
    cipher_req = NULL;
  }
      /*
       * Free in_data array from in_data_list 
       * allocated dynamically in llc_copy_ul_data_to_list() 
       */
  if(in_data_list.ptr_in_data != NULL){
    MFREE(in_data_list.ptr_in_data); 
    in_data_list.ptr_in_data = NULL;
  }

} /* tx_cipher_req() */

/*
+------------------------------------------------------------------------------
| Function    : tx_send_cipher_req
+------------------------------------------------------------------------------
| Description : This procedure fills all necessary parameters in the primitive 
|               CCI_CIPHER_REQ and sends the primitive to CCI.
|
| Parameters  : cci_cipher_req  - a valid pointer to a CCI_CIPHER_REQ primitive
|               frame_type      - indicates frame type (e.g. UI_FRAME)
|               protected_mode  - PM bit setting for CCI_CIPHER_REQ
|               ns              - N(U) for UI frames
|               cipher          - indicates if frame shall be ciphered or not
|               oc              - 
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_send_cipher_req 
(
#ifdef LL_DESC
  T_CCI_CIPHER_DESC_REQ *cci_cipher_desc_req,
#else
  T_CCI_CIPHER_REQ *cci_cipher_desc_req,
#endif
  T_PDU_TYPE frame_type,
  UBYTE protected_mode,
  T_FRAME_NUM ns,
  UBYTE cipher,
  ULONG oc
)
{
  TRACE_FUNCTION( "tx_send_cipher_req" );

  cci_cipher_desc_req->pm = protected_mode;

  if ( (cipher EQ LL_CIPHER_ON)  AND 
       (llc_data->ciphering_algorithm NEQ LLGMM_CIPHER_NO_ALGORITHM) )
  {
    cci_cipher_desc_req->ciphering_algorithm = llc_data->ciphering_algorithm;
    memcpy (&cci_cipher_desc_req->kc, &llc_data->kc, sizeof(T_kc));

    llc_generate_input (llc_data->current_sapi, frame_type, ns, 
      &cci_cipher_desc_req->ciphering_input, oc);

    cci_cipher_desc_req->direction = CCI_DIRECTION_UPLINK;
  }
  else /* LL_CIPHER_OFF */
  {
    cci_cipher_desc_req->ciphering_algorithm = CCI_CIPHER_NO_ALGORITHM;
  }

  tx_cipher_req (cci_cipher_desc_req);
  return;
} /* tx_send_cipher_desc_req() */


/*
+------------------------------------------------------------------------------
| Function    : tx_reserve_buffer
+------------------------------------------------------------------------------
| Description : This procedure allocates an element for the local transmit 
|               queue. If cause is DEFAULT or MOBILITY_MANAGEMENT, the element 
|               is appended at the end of the queue, otherwise it is inserted 
|               at the beginning. Necessary data like primitive header 
|               information and additional important data for the (resulting) 
|               GRLC_xDATA_REQ primitive is stored in the reserved buffer 
|               element. The primitive pointer in the element is set to NULL, 
|               to indicate that the element is not yet 'ready to send'. 
|               Parameter reservation_no will be set to the allocation number 
|               of the element. Parameter buffer_available will be set to TRUE 
|               if additional data primitives can be buffered for the current 
|               sapi, otherwise it will be set to FALSE.
|
| Parameters  : ll_unitdata_req   - a valid pointer to a LL_UNITDATA_REQ
|                                   primitive, containing the data to be sent
|               prim_type         - indicates GRLC_DATA/UNITDATA_REQ
|               cause             - frame cause, only valid for GRLC_DATA_REQ
|               rx_service        - service for flow control (if any)
|               reservation_no    - associated number for the buffer entry
|               buffer_available  - still buffer space available for current 
|                                   SAPI
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_reserve_buffer 
  (
#ifdef LL_DESC                               
  T_LL_UNITDESC_REQ *ll_unitdesc_req,
#else
  T_LL_UNITDATA_REQ *ll_unitdesc_req,
#endif
  T_PRIM_TYPE prim_type,
  UBYTE cause,
  T_SERVICE rx_service,
  ULONG *reservation_no,
  BOOL *buffer_available
  )
{
  T_TX_QUEUE        *elem;
  T_TX_QUEUE        **insert;

  static ULONG allocation_number = 0;

  TRACE_FUNCTION ("tx_reserve_buffer");

  MALLOC (elem, sizeof(T_TX_QUEUE));
  
  /*
   * Increase allocation number. Use of reservation_no must correspond 
   * to use in tx_store_buffer().
   */
  *reservation_no = ++(allocation_number);

  /*
   * Store all required information in new queue element. Member primitive 
   * is set to NULL to indicate that the element is not yet ready to be sent.
   * Copy necessary primitive header information to ph_* variables.
   */
  elem->primitive       = (ULONG)NULL;
  elem->prim_type       = prim_type;
  elem->reference       = allocation_number;
  elem->rx_service      = rx_service;
  elem->remove_frame    = FALSE;
  elem->ph_sapi         = ll_unitdesc_req->sapi;  
  elem->ph_tlli         = ll_unitdesc_req->tlli;
  elem->ph_grlc_qos_peak = ll_unitdesc_req->ll_qos.peak;
  elem->ph_radio_prio   = ll_unitdesc_req->radio_prio;
  elem->ph_cause        = cause;
#ifdef REL99 
  elem->ph_pkt_flow_id  = (UBYTE)ll_unitdesc_req->pkt_flow_id;
#endif /* REL99 */

  if ((cause EQ GRLC_DTACS_DEF) OR (cause EQ GRLC_DTACS_MOBILITY_MANAGEMENT))
  {
    /*
     * "Normal" frame cause, append element at the end of queue. Let insert
     * either point to llc_data->tx.queue or to the member .next of an element.
     */
    insert = &llc_data->tx.queue;
    while (*insert NEQ NULL)
    {
      insert = &((*insert)->next);
    }

    /*
     * Insert new element at found location and mark element as last in queue.
     */
    *insert = elem;
    elem->next = NULL;
  }
  else /* GRLC_DTACS_PAGE_RESPONSE OR GRLC_DTACS_CELL_UPDATE */
  {
    /*
     * No "normal" frame cause, thus LLGMM_TRIGGER_REQ must have been
     * received. Insert element at the beginning of queue.
     */
    elem->next = llc_data->tx.queue;
    llc_data->tx.queue = elem;
  }

  /*
   * Check if a service for flow control is specified (currently only 
   * SERVICE_UITX and SERVICE_ITX are recognized).
   */
  *buffer_available = FALSE;

  if (rx_service EQ SERVICE_UITX)
  {
    /*
     * Decrement the space counter of UITX primitives in the queue for the
     * current SAPI. This means that one place of the queue is occupied.
     * Check if there is space for one more primitive in the queue.
     */
    int n = llc_data->tx.queue_counter_uitx[UIMAP(llc_data->current_sapi)];
    
    if (n > 0)
    {
      llc_data->tx.queue_counter_uitx[UIMAP(llc_data->current_sapi)]--;

      if (n > 1) 
      {
        *buffer_available = TRUE;
      }
    }
    else
    {
      TRACE_EVENT ("Check uitx-queue flow");
    }
  }
  else if (rx_service EQ SERVICE_ITX)
  {
    /*
     * Decrement the space counter of ITX primitives in the queue for the
     * current SAPI. This means that one place of the queue is occupied.
     * Check if there is space for one more primitive in the queue.
     */
    int n = llc_data->tx.queue_counter_itx[IMAP(llc_data->current_sapi)];

    if (n > 0)
    {
      llc_data->tx.queue_counter_itx[IMAP(llc_data->current_sapi)]--;

      if (n > 1)
      {
        *buffer_available = TRUE;
      }
    }
    else
    {
      TRACE_EVENT ("Check itx-queue flow");
    }
  }
  else
  {
    /*
     * All other services (e.g. SERVICE_U) are allowed to send primitives 
     * per default. I.E. no flow control, except for service UITX and ITX.
     */
    *buffer_available = TRUE;
  }
} /* tx_reserve_buffer() */


/*
+------------------------------------------------------------------------------
| Function    : tx_store_buffer
+------------------------------------------------------------------------------
| Description : This procedure stores the given CCI primitive as GRLC primitive 
|               in the already reserved local transmit queue element that is 
|               given with primitive parameter reference. This queue element 
|               is marked as 'ready to send'. The CCI primitive is being 
|               PPASSED as GRLC_DATA_REQ/GRLC_UNITDATA_REQ, according to 
|               prim_type in the queue element. All header information for 
|               the GRLC primitive is being filled in from the already stored 
|               data in the element.
|
| Parameters  : cci_cipher_cnf - a valid pointer to a CCI_CIPHER_CNF primitive
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_store_buffer (T_GRLC_DATA_REQ *grlc_data_req)
{
  T_TX_QUEUE        *elem = llc_data->tx.queue;


  TRACE_FUNCTION ("tx_store_buffer");

  /*
   * Find corresponding queue entry. Use of reference must correspond to
   * use in tx_reserve_buffer().
   */
  while (elem NEQ NULL)
  {
    if (elem->reference EQ grlc_data_req->tlli)
    {
      break;
    }

    elem = elem->next;
  }
  
  if (elem NEQ NULL)
  {
    if (elem->prim_type EQ PRIM_DATA)
    {
      /*
       * Copy information from stored primitive header variables (ph_*) to 
       * GRLC primitive.
       */
      grlc_data_req->sapi              = elem->ph_sapi;
      grlc_data_req->tlli              = elem->ph_tlli;
#ifdef LL_2to1
      grlc_data_req->grlc_qos.peak      = elem->ph_grlc_qos_peak;
#else
      grlc_data_req->grlc_qos.peak      = elem->ph_grlc_qos_peak;
#endif
      grlc_data_req->radio_prio        = elem->ph_radio_prio;
      grlc_data_req->cause             = elem->ph_cause;

#ifdef REL99 
      grlc_data_req->pkt_flow_id[0]    = elem->ph_pkt_flow_id;
#endif /* REL99*/

#ifdef _SIMULATION_
      /*
       * Initialize all (unused) members of grlc_qos, because otherwise the test
       * cases fail.
       */
#ifdef LL_2to1
      grlc_data_req->grlc_qos.delay     = PS_DELAY_SUB;
      grlc_data_req->grlc_qos.relclass  = PS_RELCLASS_SUB;
      grlc_data_req->grlc_qos.preced    = PS_PRECED_SUB;
      grlc_data_req->grlc_qos.mean      = PS_MEAN_SUB;
#else
      grlc_data_req->grlc_qos.delay     = GRLC_DELAY_SUB;
      grlc_data_req->grlc_qos.relclass  = GRLC_RELCLASS_SUB;
      grlc_data_req->grlc_qos.preced    = GRLC_PRECED_SUB;
      grlc_data_req->grlc_qos.mean      = GRLC_MEAN_SUB;
#endif
      
#endif /* _SIMULATION_ */

      elem->primitive = (ULONG)grlc_data_req;
    }
    else /* PRIM_UNITDATA */
    {
      /*
       * Store CCI primitive as GRLC_UNITDATA_REQ (and mark it as stored).
       */
      PPASS (grlc_data_req, grlc_unitdata_req, GRLC_UNITDATA_REQ);

      /*
       * Copy information from stored primitive header variables (ph_*) to 
       * GRLC primitive (omit ph_cause, because it is not present in 
       * GRLC_UNITDATA_REQ).
       */
      grlc_unitdata_req->sapi              = elem->ph_sapi;
      grlc_unitdata_req->tlli              = elem->ph_tlli;
#ifdef LL_2to1
      grlc_unitdata_req->grlc_qos.peak = elem->ph_grlc_qos_peak;
#else
      grlc_unitdata_req->grlc_qos.peak      = elem->ph_grlc_qos_peak;
#endif
      grlc_unitdata_req->radio_prio        = elem->ph_radio_prio;
#ifdef REL99 
      grlc_unitdata_req->pkt_flow_id[0]    = elem->ph_pkt_flow_id;
#endif /* REL99*/
#ifdef _SIMULATION_
      /*
       * Initialize all (unused) members of grlc_qos, because otherwise the test
       * cases fail.
       */
#ifdef LL_2to1
      grlc_unitdata_req->grlc_qos.delay = PS_DELAY_SUB;
      grlc_unitdata_req->grlc_qos.relclass  = PS_RELCLASS_SUB;
      grlc_unitdata_req->grlc_qos.preced    = PS_PRECED_SUB;
      grlc_unitdata_req->grlc_qos.mean      = PS_MEAN_SUB;
#else
      grlc_unitdata_req->grlc_qos.delay     = GRLC_DELAY_SUB;
      grlc_unitdata_req->grlc_qos.relclass  = GRLC_RELCLASS_SUB;
      grlc_unitdata_req->grlc_qos.preced    = GRLC_PRECED_SUB;
      grlc_unitdata_req->grlc_qos.mean      = GRLC_MEAN_SUB;
#endif /* LL_2to1 */




#endif /* _SIMULATION_ */

      elem->primitive = (ULONG)grlc_unitdata_req;
    }
  }
  else /* elem == NULL */
  {
    /*
     * elem not found is possible in case of an LLC re-init, reset, 
     * unassign, ...
     */
    TRACE_0_INFO("No TX queue entry for given reference found");
    PFREE (grlc_data_req);
  }

} /* tx_store_buffer() */


/*
+------------------------------------------------------------------------------
| Function    : tx_get_next_frame
+------------------------------------------------------------------------------
| Description : When LLC is not suspended, this procedure gets (and removes) 
|               the first primitive out of the local transmit queue and stores 
|               it in one of the parameters, according to the type of the 
|               primitive (which is written in prim_type). If no frame is 
|               available in the queue (i.e. first queue element is not marked
|               as 'ready to send', or the queue is empty), the value NO_PRIM
|               is written in prim_type. The parameter rx_service indicates the
|               originator service. In case the rx_service is UITX or ITX an 
|               READY signal for flow control will be send after frame 
|               transmission.
|               When LLC is suspended, this procedure returns the first 
|               primitive for SAPI 1 or the first U frame for any SAPI, which 
|               is ready to send.
|
| Parameters  : grlc_data_req - a valid pointer to a pointer to a GRLC-DATA-REQ 
|                              primitive
|               grlc_unitdata_req - a valid pointer to a pointer to a 
|                                  GRLC-UNITDATA-REQ primitive
|               prim_type - will be set to PRIM_DATA or PRIM_UNITDATA, must be
|                           a valid pointer
|               rx_service - indicates the originater service 
|                            must be a valid pointer
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_get_next_frame (T_GRLC_DATA_REQ **grlc_data_req,
                               T_GRLC_UNITDATA_REQ **grlc_unitdata_req,
                               T_PRIM_TYPE *prim_type,
                               T_SERVICE *rx_service,
                               UBYTE *sapi)
{ 
  int               frame_len;
  int               ctrl_len;
  T_TX_QUEUE        *elem;
  T_TX_QUEUE        **find;


  TRACE_FUNCTION( "tx_get_next_frame" );


  /*
   * Initialise find w/ queue start. find points always to the element 
   * "before" the examined one, to be able to modifiy the queue, if
   * an element is found.
   */
  find = &llc_data->tx.queue;

  /*
   * If LLC is suspended search the queue for the first GRLC_DATA_REQ primitive,
   * otherwise take the first primitive in queue (if any).
   */
  if (llc_data->suspended EQ TRUE)
  {
    /*
     * LLC is in suspended mode. Search the queue for the first primitive 
     * for SAPI 1 or the first U frame for any SAPI
     */
    while ( (*find              NEQ NULL)       AND 
           ((*find)->ph_sapi    NEQ LL_SAPI_1)  AND 
           ((*find)->rx_service NEQ SERVICE_U)  AND
           ((*find)->primitive  NEQ (ULONG)NULL)  )
    {
      find = &((*find)->next);
    }
  }

  /*
   * Let elem point to queue element (NULL, if no element present/found).
   */
  elem = *find;


  /*
   * Check if queue is empty or "first" queue element (depending on suspended
   * mode) is marked as not 'ready to send', i.e. it contains a NULL 
   * primitive pointer.
   */
  if ((elem EQ NULL) OR (elem->primitive EQ (ULONG)NULL))
  {
    /*
     * Queue is empty or "first" element is not yet 'ready to send'.
     */
    *prim_type = NO_PRIM;
  }
  else /* (elem NEQ NULL) AND (elem->primitive NEQ NULL) */
  {
    /*
     * Found a valid (and 'ready to send') element in queue.
     */

    /*
     * Remove found element from queue (set member next of element before
     * the found element to the element behind the found element and thus
     * skip the found element).
     */
    *find = (*find)->next;
  
    /*
     * Store data of element in the given parameters.
     */
    *prim_type  = elem->prim_type;
    *rx_service = elem->rx_service;
    *sapi       = elem->ph_sapi;

    if (elem->prim_type EQ PRIM_DATA)
    {
      *grlc_data_req = (T_GRLC_DATA_REQ *)elem->primitive;

      frame_len = BYTELEN((*grlc_data_req)->sdu.l_buf);
    }
    else /* PRIM_UNITDATA */
    {
      *grlc_unitdata_req = (T_GRLC_UNITDATA_REQ *)elem->primitive;

      frame_len = BYTELEN((*grlc_unitdata_req)->sdu.l_buf);
    }

    /*
     * If the information fild doesn't fit in current N201_U, remove frame
     * (only applies to service U and UITX).
     */
    if (*rx_service == SERVICE_U || *rx_service == SERVICE_UITX)
    {
      ctrl_len = (*rx_service == SERVICE_U) ? U_FRAME_MIN_OCTETS
                                            : UI_FRAME_MIN_OCTETS;

      if (frame_len > llc_data->n201_u_base[UIMAP(*sapi)] + ctrl_len)
      {
        elem->remove_frame = TRUE;
        TRACE_0_INFO("Primitive in TX exceeds N201-U: marked to remove");
      }
    }

    /*
     * If frame is marked to remove, do it here
     */
    if (elem->remove_frame == TRUE)
    {
      if (elem->prim_type EQ PRIM_DATA)
      {
        PFREE (*grlc_data_req);
        TRACE_0_INFO("GRLC_DATA_REQ removed from TX queue");
      }
      else
      {
        PFREE (*grlc_unitdata_req);
        TRACE_0_INFO("GRLC_UNITDATA_REQ removed from TX queue");
      }

      *prim_type  = PRIM_REMOVED;
    }

    /*
     * Remove TX queue entry management element
     */
    MFREE (elem);

    /*
     * Check if service queue space should be tracked (at the moment only 
     * for service UITX and ITX, due to flow control to this service).
     */
    if (*rx_service EQ SERVICE_UITX)
    {
      /*
       * Increment the UITX space counter for the SAPI of the primitive
       * to indicate that the primitive has been stored in queue.
       */
      if (*prim_type EQ PRIM_DATA)
      {
        llc_data->tx.queue_counter_uitx[UIMAP(*sapi)]++;
      }
      else /* PRIM_UNITDATA */
      {
        llc_data->tx.queue_counter_uitx[UIMAP(*sapi)]++;
      }
    }
    else if (*rx_service EQ SERVICE_ITX)
    {
      /*
       * Increment the ITX space counter for the SAPI of the primitive
       * to indicate that the primitive has been stored in queue.
       */
      llc_data->tx.queue_counter_itx[IMAP(*sapi)]++;
    }
  }

  return;
} /* tx_get_next_frame() */


/*
+------------------------------------------------------------------------------
| Function    : tx_get_first_data_frame
+------------------------------------------------------------------------------
| Description : This procedure finds the first GRLC_DATA_REQ primitive in the 
|               local transmit queue and stores its queue element in the 
|               parameter element, if any. The primitive (element) stays in 
|               the queue. If no GRLC_DATA_REQ primitive is found in the queue, 
|               no element is stored in element, but the value NULL.
|
| Parameters  : elem          - must be a valid pointer to a pointer to a 
|                               TX queue element
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_get_first_data_frame (T_TX_QUEUE **elem)
{
  TRACE_FUNCTION( "tx_get_first_data_frame" );

  /*
   * Check the local transmit queue until a GRLC_DATA_REQ is found or the end
   * of the queue has been reached. In the latter case, *elem is automatically
   * set to NULL.
   */
  for (*elem = llc_data->tx.queue; *elem NEQ NULL; *elem = (*elem)->next)
  {
    /*
     * Check if primitive is a GRLC_DATA_REQ primitive.
     */
    if ((*elem)->prim_type EQ PRIM_DATA)
    {
      break;
    }
  }

  return;
} /* tx_get_first_data_frame() */


/*
+------------------------------------------------------------------------------
| Function    : tx_remove_data_frames
+------------------------------------------------------------------------------
| Description : This procedure removes all PRIM_DATA for given SAPI and service
|               from TX queue. If an element is removed and 
|
| Parameters  : service - service which requests to delete its primitives
|               sapi    - the sapi of the primitives to delete
|
+------------------------------------------------------------------------------
*/
GLOBAL void tx_remove_data_frames (T_SERVICE service, T_SAPI sapi)
{
  T_TX_QUEUE   *elem;
  T_TX_QUEUE  **find = &llc_data->tx.queue;
  BOOL          elem_removed = FALSE;

  TRACE_FUNCTION( "tx_remove_data_frames" );

  while (*find NEQ NULL)
  {
    /*
     * Let elem point to queue element
     */
    elem = *find;

    /*
     * Check if primitive is a GRLC_DATA_REQ primitive and
     * check the sapi of the queue element
     */
    if ((elem->prim_type  EQ PRIM_DATA) AND
        (elem->ph_sapi    EQ sapi)      AND
        (elem->rx_service EQ service)     )
    {
      elem_removed = TRUE;

      /*
       * Remove found element from queue (set member next of element before
       * the found element to the element behind the found element and thus
       * skip the found element).
       */
      *find = (*find)->next;

      /*
       * Check if we have a valid pointer to a primitive
       */
      if ((elem->primitive NEQ (ULONG)NULL))
      {
        PFREE ((T_GRLC_DATA_REQ *)elem->primitive);

        TRACE_0_INFO("TX queue element and primitive removed");
      }
      else
      {
        /*
         * Primitive is in use at CCI.
         * It is no problem if the corresponding primitive to this entry 
         * returns from CCI, because the reference number will not be found 
         * and the primitive data will then be freed.
         */
        TRACE_0_INFO("In use TX queue element removed");
      }

      /*
       * Remove TX queue entry management element.
       */
      MFREE (elem);

      /*
       * Check if service queue space should be tracked (at the moment only 
       * for service UITX and ITX, due to flow control to this service).
       * In this case increment the space counter for the SAPI of the 
       * primitive to indicate that the primitive has been stored in queue.
       */
      if (service EQ SERVICE_UITX)
      {
        llc_data->tx.queue_counter_uitx[UIMAP(sapi)]++;
      }
      else if (service EQ SERVICE_ITX)
      {
        llc_data->tx.queue_counter_itx[IMAP(sapi)]++;
      }
    }
    else
    {
      /*
       * Point to the next queue element pointer
       */
      find = &((*find)->next);
    }
  }

  /*
   * Now check, if a ready indication must be send must be send to the 
   * requesting service (if at least one elem was removed, send one).
   * Currently only service UITX and ITX are requesting a signal.
   */
  if (elem_removed)
  {
    SWITCH_LLC (sapi);

    if (service EQ SERVICE_UITX)
    {
      sig_tx_uitx_ready_ind();
    }
    else if (service EQ SERVICE_ITX)
    {
      sig_tx_itx_ready_ind();
    }
  }

  return;
} /* tx_remove_data_frames() */

