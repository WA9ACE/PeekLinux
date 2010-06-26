/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  tra_upf.c
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
|  Purpose :  This Modul defines the procedures and functions for
|             the component TRA
+----------------------------------------------------------------------------- 
*/ 

#ifndef TRA_UPF_C
#define TRA_UPF_C
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

#ifdef _TARGET_
  #include "ra_l1int.h"
#endif

/*==== CONST =======================================================*/

/*==== TYPES =======================================================*/

/*==== VAR EXPORT ==================================================*/

/*==== VAR LOCAL ===================================================*/

/*==== FUNCTIONS ===================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : up_init
+------------------------------------------------------------------------------
|  Description : initialise the tra data for the uplink process
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void ul_init(T_TRA_UP *dul)
{
  TRACE_FUNCTION ("ul_init()");

  dul->sa       = FL_INACTIVE;
  dul->sb       = FL_INACTIVE;
  dul->x        = FL_INACTIVE;
  dul->List_off = 0;

  INIT_STATE (UP, UP_INACTIVE);
}

/*
+------------------------------------------------------------------------------
|  Function    : snd_break_to_RA
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      :
+------------------------------------------------------------------------------
*/

GLOBAL void snd_break_to_RA(USHORT break_len)
{
#ifndef _SIMULATION_

  T_RA_BREAK_REQ RA_break_req;
  RA_break_req.break_len = break_len;

#endif

  TRACE_FUNCTION ("snd_break_to_RA()");

#ifndef _SIMULATION_

  l1i_ra_break_req (&RA_break_req);

#else
  {
    PALLOC (ra_break_req, RA_BREAK_REQ);
    ra_break_req->break_len = break_len;
    PSENDX (RA, ra_break_req);
  }
#endif
}

/*
+------------------------------------------------------------------------------
|  Function    : snd_data_to_RA
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      :
+------------------------------------------------------------------------------
*/

GLOBAL void snd_data_to_RA(void)
{
  T_TRA_UP *dul = &tra_data->up;
  T_FD *pFD = &dul->fd;

  T_desc2 *desc = (T_desc2*)dul->Prim->desc_list2.first;

  USHORT  data_off = dul->List_off;                   /* offset within current data block */
  USHORT  data_len = dul->Prim->desc_list2.list_len;  /* local mark for overall data size to be copied */
  
  TRACE_FUNCTION ("snd_data_to_RA()");

  pFD->type = FR_TRANS;
  pFD->status = 0;
  pFD->len = 0;      /* to mark current sdu fill status */

  if (dul->req_frames)
  {
    while (desc)
    {
      if (desc->len <= (dul->req_frames - pFD->len)) /* DTI data size less than SDU space*/
      {
        T_desc2 *desc_im = (T_desc2*)desc->next;
        memcpy(&dul->to_ra[pFD->len], desc->buffer + data_off, desc->len);
        pFD->len  += desc->len;
        data_len -= desc->len;
        data_off = 0;                       /* list is copied completely! */
        MFREE (desc);
        desc = (T_desc2*) desc_im;
      }
      else /* more DTI data than SDU space*/
      {                                                            
        memcpy(&dul->to_ra[pFD->len], desc->buffer + data_off, dul->req_frames - pFD->len);
        data_off  += (dul->req_frames - pFD->len);  /* remember for next copying! */
        desc->len -= (dul->req_frames - pFD->len);
        desc->size = desc->len;
        data_len  -= (dul->req_frames - pFD->len);
        pFD->len    =  dul->req_frames;             /* the SDU is full */
        break;
      }
    }
    /*
     * write back current standings
     */
    dul->Prim->desc_list2.first    = (ULONG)desc;
    dul->Prim->desc_list2.list_len = data_len;
    dul->List_off                  = data_off;
  }

  if (dul->sa EQ DTI_SA_OFF)
    pFD->status |= ST_SA;
  if (dul->sb EQ DTI_SB_OFF)
    pFD->status |= ST_SB;
  if (dul->x EQ DTI_FLOW_OFF)
    pFD->status |= ST_X;

#ifdef _TARGET_

  l1i_ra_data_req_new(pFD);

#else
  {
  PALLOC_SDU (ra_data_req, RA_DATA_REQ, (USHORT)(dul->req_frames<<3));

  if (hCommRA < VSI_OK)
  {
    hCommRA = vsi_c_open (VSI_CALLER RA_NAME);
  }
  ra_data_req->fr_type   = pFD->type;
  ra_data_req->status    = pFD->status;
  ra_data_req->dtx_flg   = DTX_DIS;
  ra_data_req->sdu.l_buf = pFD->len << 3;
  ra_data_req->sdu.o_buf = 0;
  memcpy(&ra_data_req->sdu.buf[0], &dul->to_ra[0], pFD->len);
  PSENDX (RA, ra_data_req);
  }
#endif
}

/*
+------------------------------------------------------------------------------
|  Function    : up_start_dti_flow
+------------------------------------------------------------------------------
|  Description :
|
|  Parameters  : -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void up_start_dti_flow(void)
{
  TRACE_FUNCTION ("up_start_dti_flow()");

  dti_start(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL);        
}
