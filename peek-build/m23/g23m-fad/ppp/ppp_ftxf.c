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
|  Purpose :  This modul is part of the entity PPP and implements all
|             procedures and functions as described in the
|             SDL-documentation (FTX-statemachine)
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
/*lint -efile(766,gsm.h) */
#include "gsm.h"        /* to get a lot of macros */
/*lint -efile(766,cnf_ppp.h) */
#include "cnf_ppp.h"    /* to get cnf-definitions */
/*lint -efile(766,mon_ppp.h) */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#include "ppp_arbf.h"   /* to get function interface from arb */

/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

#ifndef PPP_INT_RAM
/*
+------------------------------------------------------------------------------
| Function    : ftx_init
+------------------------------------------------------------------------------
| Description : The function ftx_init() initializes Frame Transmit (FTX)
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void ftx_init ()
{
  USHORT i;

  TRACE_FUNCTION( "ftx_init" );

  ppp_data->ftx.acfc         = PPP_ACFC_DEFAULT;
  ppp_data->ftx.pfc          = PPP_PFC_DEFAULT;

  /*
   * inititialise ACCMTAB
   */
  for(i=0;i<=255;i++)
  {
    if(i<0x20)
    {
      ppp_data->ftx.accmtab[i] = (PPP_ACCM_DEFAULT & (1UL << i)) ? 0xFF : 0x00;
    }
    else if((i EQ PPP_HDLC_FLAG) ||
            (i EQ PPP_HDLC_ESCAPE) ||
            (i EQ PPP_ASCII_DEL))
    {
      ppp_data->ftx.accmtab[i] = 0xFF;
    }
    else
    {
      ppp_data->ftx.accmtab[i] = 0x00;
    }
  }


  INIT_STATE( PPP_SERVICE_FTX , FTX_DEAD );
} /* ftx_init() */
#endif  /* PPP_INT_RAM */



#ifndef PPP_FLASH_ROM
/*
+------------------------------------------------------------------------------
| Function    : ftx_get_frame
+------------------------------------------------------------------------------
| Description : The function ftx_get_frame() puts the given packet into a HDLC
|               frame. A FCS is calculated and put at the the end of the
|               packet. After that every byte is examined and preceeded with an
|               escape character when certain condiitions are met.
|
|               This function has been optimized - FCS calculation and escaping
|               are done in the same loop.
|
| Parameters  : ptype     - packet type
|               packet    - pointer to a generic data descriptor
|               ptr_frame - returns a list of generic data descriptors
|
+------------------------------------------------------------------------------
*/
GLOBAL void ftx_get_frame (USHORT ptype, T_desc2* packet, T_desc_list2* ptr_frame)
{
  T_desc2 *temp_desc1, *temp_desc2;
  USHORT  calc_fcs;
  USHORT  i;
  UBYTE   currentByte;
  UBYTE   escape;

  TRACE_FUNCTION( "ftx_get_frame" );

  /*
   * create a new descriptor for address, control and protocol field and put
   * it in front of the linked descriptor list
   */
    /*
     * 1 octet address field
     * 1 octet control field
     * 2 octets protocol field (uncompressed)
     *----------
     * 4 octets
     */
  MALLOC (temp_desc1, (USHORT)(sizeof(T_desc2) - 1 + 4));
  temp_desc1->next = (ULONG)packet;
  packet           = temp_desc1;
  packet->len      = 0;
    /*
     * if no address and control field compression add both
     * in LCP packets we need always address and control field
     */
  if((ppp_data->ftx.acfc EQ FALSE) ||
     (ptype EQ DTI_PID_LCP))
  {
    packet->buffer[packet->len] = 0xff;
    packet->len++;
    packet->buffer[packet->len] = 0x03;
    packet->len++;
  }
    /*
     * if no protocol field compression add most significant octet
     * of the protocol field
     * in LCP packets we need always uncompressed protocol field
     */
  if((ppp_data->ftx.pfc EQ FALSE) ||
     (ptype > 0x00ff) ||
     (ptype EQ DTI_PID_LCP))
  {
    packet->buffer[packet->len] = (UBYTE)(ptype >> 8);
    packet->len++;
  }
    /*
     * add protocol field least significant octet
     */
  packet->buffer[packet->len] = (UBYTE)(ptype & 0xff);
  packet->len++;


  /*
   * create complete frame
   * use async-control-character-map and copy the packet into descriptor with
   * a size of FTX_GET_DESC_SIZE
   */
  MALLOC (temp_desc1, (USHORT)(sizeof(T_desc2) - 1 + FTX_GET_DESC_SIZE));
  temp_desc1->next = (ULONG)NULL;
  ptr_frame->first = (ULONG)temp_desc1;


  /*
   * reset ptr_frame->list_len, it will be incremented by temp_desc1->len
   * when the packet is completely processed, at the end it will contain
   * the length of the whole frame in bytes
   */
  ptr_frame->list_len   = 0;


  /*
   * insert HDLC-Flag - to mark the start of the frame
   */
  temp_desc1->len       = 1;
  temp_desc1->size      = 1;
  temp_desc1->offset    = 1;
  temp_desc1->buffer[0] = PPP_HDLC_FLAG;
  escape=FALSE;


  /*
   * init FCS
   */
  calc_fcs   = PPP_INITFCS;


  /*
   * ATTENTION: the following two while loops contain equivalent
   * code - if you edit the code make sure you edit BOTH loops
   */
  if(ptype EQ DTI_PID_LCP)
  {
    while(packet NEQ NULL)
    {
      i=0;
      while(i < packet->len)
      {
        currentByte = packet->buffer[i];

        if(escape EQ TRUE)
        {
          temp_desc1->buffer[temp_desc1->len] = currentByte ^ 0x20;

          calc_fcs=(calc_fcs >> 8) ^
            ppp_data->fcstab[(calc_fcs ^ currentByte) & 0xff];

          escape=FALSE;
          i++;
        }

        /*
         * In LCP packets we escape ALL characters below 0x20.
         * In addition to that PPP_HDLC_FLAG, PPP_HDLC_ESCAPE,
         * PPP_ASCII_DEL are escaped too. These characters are marked in
         * in the ACCM table
         */
        else if((currentByte < 0x20) || ppp_data->ftx.accmtab[currentByte])
        {
          temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_ESCAPE;
          escape = TRUE;
        }

        else
        {
          temp_desc1->buffer[temp_desc1->len] = currentByte;

          calc_fcs=(calc_fcs >> 8) ^
            ppp_data->fcstab[(calc_fcs ^ currentByte) & 0xff];

          i++;
        }

        temp_desc1->len++;

        if(temp_desc1->len >= FTX_GET_DESC_SIZE)
        {
          ptr_frame->list_len +=temp_desc1->len;

          temp_desc2=temp_desc1;
          MALLOC (temp_desc1,
            (USHORT)(sizeof(T_desc2) - 1 + FTX_GET_DESC_SIZE));

          temp_desc1->len=0;
          temp_desc1->next = (ULONG)NULL;

          temp_desc2->next=(ULONG)temp_desc1;
        }
      }

      temp_desc2=packet;
      packet=(T_desc2*)packet->next;
      MFREE(temp_desc2);
    }
    ptr_frame->list_len +=temp_desc1->len;
  }
  else
  {
    while(packet NEQ NULL)
    {
      i=0;
      while(i < packet->len)
      {
        currentByte = packet->buffer[i];

        if(escape EQ TRUE)
        {
          temp_desc1->buffer[temp_desc1->len] = currentByte ^ 0x20;

          calc_fcs=(calc_fcs >> 8) ^
            ppp_data->fcstab[(calc_fcs ^ currentByte) & 0xff];

          escape=FALSE;
          i++;
        }

        /*
         * In packets other than LCP we escape only those characters
         * below 0x20 that are marked in the ACCM table.
         * In addition to that PPP_HDLC_FLAG, PPP_HDLC_ESCAPE,
         * PPP_ASCII_DEL are escaped too. These characters are marked in
         * in the same table.
         */
        else if(ppp_data->ftx.accmtab[currentByte])
        {
          temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_ESCAPE;
          escape = TRUE;
        }

        else
        {
          temp_desc1->buffer[temp_desc1->len] = currentByte;

          calc_fcs=(calc_fcs >> 8) ^
            ppp_data->fcstab[(calc_fcs ^ currentByte) & 0xff];

          i++;
        }

        temp_desc1->len++;

        if(temp_desc1->len >= FTX_GET_DESC_SIZE)
        {
          ptr_frame->list_len +=temp_desc1->len;

          temp_desc2=temp_desc1;
          MALLOC (temp_desc1,
            (USHORT)(sizeof(T_desc2) - 1 + FTX_GET_DESC_SIZE));
          temp_desc1->len=0;
          temp_desc1->next = (ULONG)NULL;

          temp_desc2->next=(ULONG)temp_desc1;
        }
      }

      temp_desc2=packet;
      packet=(T_desc2*)packet->next;
      MFREE(temp_desc2);
    }
    ptr_frame->list_len +=temp_desc1->len;
  }

  /*
   * finish FCS calculation
   */
  calc_fcs ^= 0xffff;


  /*
   * create a new descriptor for FCS and HDLC-Flag
   * put the descriptor at the end of the descriptor list
   *
   * FCS must be escaped too, allocate two bytes more - just in case
   * 2 for FCS, 2 for PPP_HDLC_ESCAPE, 1 for PPP_HDLC_FLAG
   *
   */
  if(temp_desc1->len + 5 >= FTX_GET_DESC_SIZE)
  {
    MALLOC (temp_desc2, (USHORT)(sizeof(T_desc2) - 1 + 5));
    temp_desc2->len    = 0;
    temp_desc2->size   = 0;
    temp_desc2->offset = 0;
    temp_desc2->next   = (ULONG)NULL;

    temp_desc1->next = (ULONG)temp_desc2;
    temp_desc1 = temp_desc2;
  }

  /*
   * insert the FCS, escape when necessary
   */
#ifdef _SIMULATION_
  temp_desc1->buffer[temp_desc1->len]=(UBYTE)(PPP_GOODFCS >> 8);
  temp_desc1->buffer[temp_desc1->len+1]=(UBYTE)(PPP_GOODFCS & 0x00ff);
  temp_desc1->len      += 2;
  ptr_frame->list_len  += 2;
#else /* _SIMULATION_ */
  /*
   * least significant octet first
   */
  if(ptype EQ DTI_PID_LCP)
  {
    currentByte = (UBYTE)(calc_fcs & 0x00ff);
    if((currentByte < 0x20) || ppp_data->ftx.accmtab[currentByte])
    {
      temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_ESCAPE;
      temp_desc1->buffer[temp_desc1->len+1] = currentByte ^ 0x20;
      temp_desc1->len      += 2;
      ptr_frame->list_len  += 2;
    }
    else
    {
      temp_desc1->buffer[temp_desc1->len] = currentByte;
      temp_desc1->len++;
      ptr_frame->list_len++;
    }

    currentByte = (UBYTE)(calc_fcs >> 8);
    if((currentByte < 0x20) || ppp_data->ftx.accmtab[currentByte])
    {
      temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_ESCAPE;
      temp_desc1->buffer[temp_desc1->len+1] = currentByte ^ 0x20;
      temp_desc1->len      += 2;
      ptr_frame->list_len  += 2;
    }
    else
    {
      temp_desc1->buffer[temp_desc1->len] = currentByte;
      temp_desc1->len++;
      ptr_frame->list_len++;
    }
  }
  else
  {
    currentByte = (UBYTE)(calc_fcs & 0x00ff);
    if(ppp_data->ftx.accmtab[currentByte])
    {
      temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_ESCAPE;
      temp_desc1->buffer[temp_desc1->len+1] = currentByte ^ 0x20;
      temp_desc1->len      += 2;
      ptr_frame->list_len  += 2;
    }
    else
    {
      temp_desc1->buffer[temp_desc1->len] = currentByte;
      temp_desc1->len++;
      ptr_frame->list_len++;
    }

    currentByte = (UBYTE)(calc_fcs >> 8);
    if(ppp_data->ftx.accmtab[currentByte])
    {
      temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_ESCAPE;
      temp_desc1->buffer[temp_desc1->len+1] = currentByte ^ 0x20;
      temp_desc1->len      += 2;
      ptr_frame->list_len  += 2;
    }
    else
    {
      temp_desc1->buffer[temp_desc1->len] = currentByte;
      temp_desc1->len++;
      ptr_frame->list_len++;
    }
  }

#endif /* _SIMULATION_ */

  /*
   * insert HDLC-Flag - to mark the start of the frame
   */
  temp_desc1->buffer[temp_desc1->len] = PPP_HDLC_FLAG;
  temp_desc1->len++;
  temp_desc1->size = temp_desc1->len;
  ptr_frame->list_len++;


} /* ftx_get_frame() */
#endif  /* PPP_FLASH_ROM */



#ifndef PPP_INT_RAM
/*
+------------------------------------------------------------------------------
| Function    : ftx_check_frame
+------------------------------------------------------------------------------
| Description : The function ftx_check_frame() checks for flag at begin and end
|               of the frame and calculates length of frame.
|
| Parameters  : ptr_frame - gives just a frame and returns a frame with length
|
+------------------------------------------------------------------------------
*/
GLOBAL void ftx_check_frame (T_desc_list2* ptr_frame)
{
  T_desc2* frame;
  T_desc2* temp_desc;
  UBYTE    last_byte;

  TRACE_FUNCTION( "ftx_check_frame" );

  frame = (T_desc2*)ptr_frame->first;

  while((frame) && (frame->len EQ 0))
    frame = (T_desc2*)frame->next;

  if(frame)
  {
    /*
     * if the first character isnt the HDLC flag then
     * create a new first data descriptor containing just the HDLC flag
     */
    if(frame->buffer[0] NEQ PPP_HDLC_FLAG)
    {
      MALLOC (frame, (USHORT)(sizeof(T_desc2) - 1 + 1));
      frame->next      = ptr_frame->first;
      frame->len       = 1;
      frame->buffer[0] = PPP_HDLC_FLAG;
      ptr_frame->first = (ULONG)frame;
    }
    /*
     * search for the last character and if it isnt the HDLC flag then
     * create a new last data descriptor containing just the HDLC flag.
     * calculate length of whole data.
     */
    ptr_frame->list_len = 0;
    for(frame = (T_desc2*)ptr_frame->first;
        frame;
        frame = (T_desc2*)frame->next)
    {
      if(frame->len)
      {
        ptr_frame->list_len+= frame->len;
        last_byte           = frame->buffer[frame->len - 1];
      }
      temp_desc = frame;
    }
    if(last_byte NEQ PPP_HDLC_FLAG) /*lint !e644 last_byte may not have been initialized */
    {
      MALLOC (frame, (USHORT)(sizeof(T_desc2) - 1 + 1));
      frame->next      = (ULONG)NULL;
      frame->len       = 1;
      frame->buffer[0] = PPP_HDLC_FLAG;
      temp_desc->next  = (ULONG)frame; /*lint !e771 temp_desc conceivably not initialized */
    }
  }
  else
  {
    /*
     * free the empty frame
     */
    arb_discard_packet((T_desc2*)ptr_frame->first);
    ptr_frame->first    = (ULONG)NULL;
    ptr_frame->list_len = 0;
  }
} /* ftx_check_frame() */
#endif  /* PPP_INT_RAM */
