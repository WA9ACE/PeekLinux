/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_dnf.c
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
|  Purpose :  This Modul defines the procedures and functions for TRA
+----------------------------------------------------------------------------- 
*/ 

#ifndef TRA_DNF_C
#define TRA_DNF_C
#endif

#define ENTITY_L2R

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"

#include "cl_ribu.h"
#include "tra_pei.h"
#include "tra.h"

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : dl_init
+------------------------------------------------------------------------------
|  Description : initialise the l2r data for the downlink process
|
|  Parameters  : T_TRA_DN *ddl
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dl_init(T_TRA_DN *ddl)
{
  TRACE_FUNCTION ("dl_init()");

  ddl->sa               = FL_INACTIVE;
  ddl->sb               = FL_INACTIVE;
  ddl->x                = FL_INACTIVE;
  ddl->prim             = NULL;       /* meaning: no DTI2_DATA_IND allocated yet */
  ddl->Brk_dti_data_ind = NULL;

  SET_STATE (DN, DN_INACTIVE);
}

/*
+------------------------------------------------------------------------------
|  Function    : dl_send_data_ind
+------------------------------------------------------------------------------
|  Description : This procedure copies data from the downlink ring buffer
|                into a a dti_data_ind primitive and sends this primitive
|                to the upper layer.
|                (Just the SDU data are copied into the ring buffer by RA!)
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL BOOL dl_send_data_ind(void)
{
  T_TRA_DN *ddl = &tra_data->dn;

  TRACE_FUNCTION ("dl_send_data_ind()");

  if (ddl->prim NEQ NULL) /* check for existing dti_data_ind */
  {
    ddl->prim->parameters.st_lines.st_line_sa   = ddl->sa;
    ddl->prim->parameters.st_lines.st_line_sb   = ddl->sb;
    ddl->prim->parameters.st_lines.st_flow      = ddl->x;
    ddl->prim->parameters.st_lines.st_break_len = DTI_BREAK_OFF;
    
    ddl->prim->parameters.p_id = DTI_PID_UOS;
    ddl->prim->link_id = 0; /* dummy init */

    dti_send_data (l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, ddl->prim);
    ddl->prim = NULL;
    return TRUE;
  }
  else
  {
    TRACE_EVENT("no prim to send");
  }
  return FALSE;
}

/*
+------------------------------------------------------------------------------
|  Function    : dl_prep_data_ind
+------------------------------------------------------------------------------
|  Description : This procedure appends data from the downlink ring buffer
|                to a already allocated dti_data_ind primitive
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void dl_prep_data_ind(T_FD *pFD)
{
  T_desc2* desc;   /* block to be copied to */
  T_desc2* p_desc; /* prim to be copied to */

  T_TRA_DN *ddl = &tra_data->dn;

  TRACE_FUNCTION ("dl_prep_data_ind()");

  if (ddl->prim EQ NULL) /* check for existing dti_data_ind */
  {
    PALLOC (dti_data_ind, DTI2_DATA_IND); /* start new prim */
    ddl->prim = dti_data_ind;

    ddl->prim->desc_list2.list_len = 0;

    MALLOC (desc, TRA_DTI_BUF_SIZE);      /* contains header bytes, too! */

    dti_data_ind->desc_list2.first =(ULONG) desc;

    p_desc         = (T_desc2 *) dti_data_ind->desc_list2.first;
    p_desc->len    = 0;
    p_desc->size   = 0;
    p_desc->offset = 0;
    p_desc->next   = 0;

    ddl->list_end = desc;   /* set flag for next entrance without sending */
  }
  else
  {
    p_desc = ddl->list_end; /* append to remembered list end */
  }

  if (!pFD) /* status bits changed; no data received */
  {
    return;
  }

  if ((TRA_DTI_BUF_SIZE - p_desc->len - sizeof(T_desc2) + 1) >= MAX_SDU_SIZE)
  {
    memcpy(&p_desc->buffer[p_desc->len], pFD->buf, pFD->len); /* copy data from ring buffer */
  }
  else /* start new block */
  {
    MALLOC (desc, (USHORT)TRA_DTI_BUF_SIZE);
    p_desc->next   = (ULONG) desc;

    /* Initialize the new block's fields to zero */
    p_desc = (T_desc2 *)p_desc->next;
    p_desc->len    = 0;
    p_desc->next   = 0;
    p_desc->size   = 0;
    p_desc->offset = 0; /* temporary solution */

    memcpy(&p_desc->buffer[p_desc->len], pFD->buf, pFD->len); /* copy data from ring buffer */
    ddl->list_end = p_desc;                                     /* write back start of last list */
  }
  p_desc->len += pFD->len;
  p_desc->size = p_desc->len;
  ddl->prim->desc_list2.list_len += pFD->len;
}

/*
+------------------------------------------------------------------------------
|  Function    : rbm_init
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  :
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void rbm_init(T_TRA_DN *ddl)
{
  TRACE_FUNCTION ("rbm_init()");

  cl_ribu_create(&tra_data->dn.ribu, MAX_SDU_SIZE, TRA_DLR_DEPTH);
}

/*
+------------------------------------------------------------------------------
|  Function    : tra_get_next_FrameDesc
+------------------------------------------------------------------------------
|  Description : This function is called from the RA.
|                It returns the frame descriptor for that buffer,
|                which the RA has to write in the received frame.
|
|                WRAPPER: Get frame descriptor for Layer 1 (TI)
|  Parameters  : -
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL T_FD *tra_get_next_FrameDesc(void)
{
  return (cl_ribu_get_new_frame_desc(tra_data->dn.ribu));
}
