/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  Rlp_sndf.c
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
|             the component Radio Link Protocol of the base station 
+----------------------------------------------------------------------------- 
*/ 


#ifndef RLP_SNDF_C
#define RLP_SNDF_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "prim.h"
#include "tok.h"
#include "rlp.h"

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
|  Function    : snd_sendDataToRA
+------------------------------------------------------------------------------
|  Description : send a RA_DATA_REQ primitive to RA. In the implementation
|                this function uses the functional interface of the
|                RA-Adaptation layer to pass the primitive to RA.
|
|
|  Parameters  : dataDesc
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void snd_sendDataToRA(T_FRAME_DESC *dataDesc)
{
#ifdef _SIMULATION_

  UBYTE  i, j;
  USHORT sduLen = (dataDesc->Len[0]+dataDesc->Len[1]+3)<<3;
  
  PALLOC_SDU (ra_data_req, RA_DATA_REQ, sduLen);

  TRACE_FUNCTION ("snd_sendDataToRA()");

  ra_data_req->fr_type   = FR_RLP;
  ra_data_req->dtx_flg   = DTX_DIS;
  ra_data_req->sdu.l_buf = sduLen;
  ra_data_req->sdu.o_buf = 0;
  
  j=0;

  for (i=0; i < dataDesc->Len[0]; i++)
    ra_data_req->sdu.buf[j++] = dataDesc->Adr[0][i]; 

  for (i=0; i < dataDesc->Len[1]; i++)
    ra_data_req->sdu.buf[j++] = dataDesc->Adr[1][i]; 
  
  ra_data_req->sdu.buf[rlp_data->snd.FrameSize - TRAILER_LEN]     = 0x4f;
  ra_data_req->sdu.buf[rlp_data->snd.FrameSize - TRAILER_LEN + 1] = 0x4f;
  ra_data_req->sdu.buf[rlp_data->snd.FrameSize - TRAILER_LEN + 2] = 0x4f;

#ifdef TRACE_RLP_FRAME
  if (rlp_data->uplink_frame_trace)
  {
    ker_trace_rlp_frame((T_RLP_FRAMEPTR)ra_data_req->sdu.buf);
  }
#endif

  PSENDX (RA, ra_data_req);

#else /* _TARGET_ */
  /*
   * use functional interface of RA
   */
  T_RA_DATA_REQ RA_data_req;

  RA_data_req.fr_type  = FR_RLP;
  RA_data_req.dtx_flg  = DTX_DIS;
  RA_data_req.status   = 0;

#ifdef TRACE_RLP_FRAME
  if (rlp_data->uplink_frame_trace)
  {
    ker_trace_rlp_frame((T_RLP_FRAMEPTR)datadesc->Adr[0]);
  }
#endif

  l1i_ra_data_req (&RA_data_req, dataDesc);

#endif /* _TARGET_ */
}

/*
+------------------------------------------------------------------------------
|  Function    : snd_init
+------------------------------------------------------------------------------
|  Description : initialise the rlp data for the send formatter process
|
|  Parameters  : 
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void snd_init(T_SND *snd)
{
  USHORT i;

  TRACE_FUNCTION ("snd_init()");
  
  /*
   * initialize the header only frame descriptor
   */
  snd->HO_FrameDesc.Adr[0] = snd->HO_Frame;
  snd->HO_FrameDesc.Len[0] = FRAME_SIZE_LONG;
  snd->HO_FrameDesc.Adr[1] = NULL;
  snd->HO_FrameDesc.Len[1] = 0;
  
/*  memset (rlp_data->snd.HO_Frame, 0x2b, FRAME_SIZE_LONG); */

  for (i = 2; i < FRAME_SIZE_LONG; i++)
  {
    snd->HO_Frame[i] = 0x41 + i;
  }
  INIT_STATE (SND, SPDU_WAIT_EVENT);
}

/*
+------------------------------------------------------------------------------
|  Function    : snd_send_u_frame
+------------------------------------------------------------------------------
|  Description : build and send a U-Frame in uplink direction
|
|  Parameters  : frameType -
|                pFBit     -
|                crBit     -
|                dataDesc  -
|                 
|
|  Return      : -
+------------------------------------------------------------------------------
*/

GLOBAL void snd_send_u_frame 
            (
              T_UF          frameType,
              T_BIT         pFBit,
              T_BIT         crBit,
              T_FRAME_DESC *dataDesc
            )
{
  UBYTE *frame;

  TRACE_FUNCTION ("snd_send_u_frame()");

  if (dataDesc EQ NULL)
    dataDesc = &rlp_data->snd.HO_FrameDesc;
  
  frame = dataDesc->Adr[0];    

  /*
   * M5-M1 Bits (16, 15-11)
   */
  frame[1] = (UBYTE) (frameType << 2);
  /*
   * P/F Bit (10) and Bit 9=1 
   */
  frame[1] |= ((pFBit << 1) | 1);
  /*
   * Bits 8-2=1111100 Bit 1 = crBit
   */
  frame[0] = (0xf8 | crBit);
 
  snd_sendDataToRA (dataDesc);
}

/*
+------------------------------------------------------------------------------
|  Function    : snd_send_s_frame
+------------------------------------------------------------------------------
|  Description : build and send a S-Frame in uplink direction
|
|  Parameters  : 
|                 
|
|  Return      : 
+------------------------------------------------------------------------------
*/


GLOBAL void snd_send_s_frame 
            (
              T_SF          frameType,
              T_FRAME_NUM   nr,
              T_BIT         pFBit,
              T_BIT         crBit,
              T_FRAME_DESC *dataDesc
            )
{
  UBYTE *frame;

  TRACE_FUNCTION ("snd_send_s_frame()");

  if (dataDesc EQ NULL)
    dataDesc = &rlp_data->snd.HO_FrameDesc;
 
  frame = dataDesc->Adr[0];    
  /*
   * N(R) Bits (16-11)
   */
  frame[1] = (UBYTE) (nr << 2);
  /*
   * P/F Bit (10) and Bit 9=1 
   */
  frame[1] |= ((pFBit << 1) | 1);
  /*
   * Bits 8-4=11111, Bits 3-2 = frameType, Bit 1 = crBit
   */
  frame[0] = (0xf0 | (frameType << 1) | crBit);
   
  snd_sendDataToRA (dataDesc);
}

/*
+------------------------------------------------------------------------------
|  Function    : snd_send_si_frame
+------------------------------------------------------------------------------
|  Description : build and send a S+I-Frame in uplink direction
|
|  Parameters  : frameType  -
|                nr         -
|                pFBit      -
|                ns         -
|                crBit      -
|                dataDesc   -
|  
|  Return      : -
+------------------------------------------------------------------------------
*/


GLOBAL void snd_send_si_frame 
            (
              T_SF          frameType,
              T_FRAME_NUM   nr,
              T_BIT         pFBit,
              T_FRAME_NUM   ns,
              T_BIT         crBit,
              T_FRAME_DESC *dataDesc
            )
{
  UBYTE  *frame;

  TRACE_FUNCTION ("snd_send_si_frame()");

  if (dataDesc EQ NULL)
    dataDesc = &rlp_data->snd.HO_FrameDesc;
  
  frame = dataDesc->Adr[0];    

  /*
   * N(R) Bits (16-11)
   */
  frame[1] = (UBYTE) (nr << 2);
  /*
   * P/F Bit (10) and Bit 9=ns(bit 0-4) 
   */
  frame[1] |= ((pFBit << 1) | (ns >> 5));
  /*
   * Bits 8-4 = ns (bits 4-0), Bits 3-2 = frameType, Bit 1 = crBit
   */
  frame[0] = ((ns << 3) | (frameType << 1) | crBit);
   
  snd_sendDataToRA (dataDesc);
}

