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
|             SDL-documentation (FRX-statemachine)
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
#include "ppp_ptxs.h"   /* to get signal interface from ptx */

#include "ppp_arbf.h"   /* to get arb functions */

#include <string.h>     /* to get memcpy */

#ifdef PPP_HDLC_TRACE
#include <stdio.h>      /* to get sprintf */
#endif /* PPP_HDLC_TRACE */
/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



#ifndef PPP_INT_RAM
/*
+------------------------------------------------------------------------------
| Function    : frx_init
+------------------------------------------------------------------------------
| Description : The function frx_init() initializes Frame Receive (FRX)
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void frx_init ()
{ 
  TRACE_FUNCTION( "frx_init" );

  ppp_data->frx.frame_complete   = FALSE;
  ppp_data->frx.stored_packet    = NULL;
  ppp_data->frx.received_data    = NULL;
  ppp_data->frx.store_state      = FRX_ADD_ERROR;
  ppp_data->frx.data_flow_state  = FRX_DATA_FLOW_DEAD;

#ifdef PPP_HDLC_TRACE
  ppp_data->frx.hdlc_frame     = NULL;
#endif /* PPP_HDLC_TRACE */
  
  INIT_STATE( PPP_SERVICE_FRX , FRX_DEAD );
} /* frx_init() */
#endif  /* PPP_INT_RAM */


#ifndef PPP_FLASH_ROM
/*
+------------------------------------------------------------------------------
| Function    : frx_add_desc
+------------------------------------------------------------------------------
| Description : The function frx_add_desc() appends the given 
|               generic data descriptor to the already received descriptors.
|               The function removes transparent characters and calculates
|               the fcs.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void frx_add_desc ()
{
  UBYTE    store_state;
  UBYTE    currentbyte;
  USHORT   fcs;
  UBYTE    escape;
  USHORT   s_offset;
  USHORT   s_size;
  UBYTE*   destination;
  USHORT   d_offset;
  USHORT   d_size;
  USHORT*  fcstab;
  T_desc2* source_desc;
  T_desc2* temp_desc;
  USHORT   packet_len;
#ifdef PPP_HDLC_TRACE
  T_desc2* trace_desc;
  T_desc2* trace_desc2;
  T_desc2* trace_desc3;
  USHORT   trace_pos;
  char     buf[100];
  USHORT   i;
#endif /* PPP_HDLC_TRACE */

  TRACE_FUNCTION( "frx_add_desc" );

#ifdef PPP_HDLC_TRACE
  /*
   * trace HDLC frame
   * the goal is to trace out only erroneous PPP frames
   * if a correct PPP frame is received 
   * we just release the stored copy of this frame without tracing
   */
  /*
   * set source pointer
   */
  trace_desc = ppp_data->frx.received_data;
  trace_pos  = ppp_data->frx.proceed_data;
  switch(ppp_data->frx.store_state)
  {
    case FRX_ADD_ERROR:
      /*
       * we trace out each byte til the first HDLC flag
       */
      /*
       * release old stored HDLC frame
       * this should not be necessary
       */
      MFREE_DESC2(ppp_data->frx.hdlc_frame);
      ppp_data->frx.hdlc_frame = NULL;
      /*
       * trace incomming data until the first HDLC flag
       */
      while((trace_desc) && (trace_pos >= trace_desc->len))
      {
        trace_desc = (T_desc2*)trace_desc->next;
        trace_pos  = 0;
      }
      if((trace_desc) &&
         (trace_desc->buffer[trace_pos] NEQ PPP_HDLC_FLAG))
      {
        TRACE_EVENT("waiting for next HDLC flag:");
        i = 0;
        do
        {
          i+= sprintf(&buf[i], "0x%02x, ", trace_desc->buffer[trace_pos]);
          trace_pos++;
          while((trace_desc) && (trace_pos >= trace_desc->len))
          {
            trace_desc = (T_desc2*)trace_desc->next;
            trace_pos  = 0;
          }
          if(i > 80)
          {
            TRACE_EVENT( buf );
            i = 0;
          }
        }
        while((trace_desc) &&
              (trace_desc->buffer[trace_pos] NEQ PPP_HDLC_FLAG));
        if(i > 0)
        {
          TRACE_EVENT( buf );
          i = 0;
        }
      }
      break;

    case FRX_ADD_HDLC_BEGIN:
      /*
       * we store just the first HDLC flag and fall through 
       * to the default case to store the complete PPP frame
       */
      /*
       * release old stored HDLC frame
       */
      MFREE_DESC2(ppp_data->frx.hdlc_frame);
      ppp_data->frx.hdlc_frame = NULL;

      if(trace_desc)
      {
        /*
         * allocate first descriptor to store new HDLC frame
         */
        MALLOC(trace_desc2,
               (USHORT)(sizeof(T_desc2) - 1 + FRX_ADD_SMALL_PACKET_SIZE));
        trace_desc2->len         = 0;
        trace_desc2->next        = (ULONG)NULL;
        ppp_data->frx.hdlc_frame = trace_desc2;

        /*
         * store first HDLC flag
         */
        TRACE_EVENT_P2("start flag found in desc=0x%08x: pos=%d", 
                       trace_desc, 
                       trace_pos);
        trace_desc2->buffer[trace_desc2->len] = trace_desc->buffer[trace_pos];
        trace_desc2->len++;
        trace_pos++;
        while((trace_desc) && (trace_pos >= trace_desc->len))
        {
          trace_desc = (T_desc2*)trace_desc->next;
          trace_pos  = 0;
        }
      }
      /* fall through */
    default:
      /*
       * to store the complete PPP frame
       */
      /*
       * set destination pointer
       */
      trace_desc2 = ppp_data->frx.hdlc_frame;
      while(trace_desc2->next NEQ (ULONG)NULL)
      {
        trace_desc2 = (T_desc2*)trace_desc2->next;
      }

      /*
       * set source pointer
       */
      while((trace_desc) && (trace_pos >= trace_desc->len))
      {
        trace_desc = (T_desc2*)trace_desc->next;
        trace_pos  = 0;
      }

      if(trace_desc)
      {
        /*
         * store HDLC frame
         */
        while((trace_desc) &&
              (trace_desc->buffer[trace_pos] NEQ PPP_HDLC_FLAG))
        {
          if(trace_desc2->len >= FRX_ADD_SMALL_PACKET_SIZE)
          {
            MALLOC(trace_desc3, (USHORT)(sizeof(T_desc2) - 1 + 
                                  FRX_ADD_SMALL_PACKET_SIZE));
            trace_desc3->len  = 0;
            trace_desc3->next = (ULONG)NULL;
            trace_desc2->next = (ULONG)trace_desc3;
            trace_desc2       = trace_desc3;
          }
          trace_desc2->buffer[trace_desc2->len] = 
            trace_desc->buffer[trace_pos];
          trace_desc2->len++;
          trace_pos++;
          while((trace_desc) && (trace_pos >= trace_desc->len))
          {
            trace_desc = (T_desc2*)trace_desc->next;
            trace_pos  = 0;
          }
        }
        /*
         * store last HDLC flag
         */
        if(trace_desc)
        {
          if(trace_desc2->len >= FRX_ADD_SMALL_PACKET_SIZE)
          {
            MALLOC(trace_desc3, (USHORT)(sizeof(T_desc2) - 1 + 
                                  FRX_ADD_SMALL_PACKET_SIZE));
            trace_desc3->len  = 0;
            trace_desc3->next = (ULONG)NULL;
            trace_desc2->next = (ULONG)trace_desc3;
            trace_desc2       = trace_desc3;
          }
          TRACE_EVENT_P2("stop flag found in desc=0x%08x pos=%d", 
                         trace_desc, 
                         trace_pos);
          trace_desc2->buffer[trace_desc2->len] = 
            trace_desc->buffer[trace_pos];
          trace_desc2->len++;
          trace_pos++;
        }
      }
      break;
  }
#endif /* PPP_HDLC_TRACE */
  /*
   * copy all important values into local variables
   */
  store_state = ppp_data->frx.store_state;
  fcs         = ppp_data->frx.calc_fcs;
  escape      = ppp_data->frx.escape;
  source_desc = ppp_data->frx.received_data;
  s_offset    = ppp_data->frx.proceed_data;
  fcstab      = ppp_data->fcstab;
  /*
   * set destination pointer
   */
  destination = NULL;
  d_offset    = 0;
  d_size      = 0;
  switch(store_state)
  {
    case FRX_ADD_INFORMATION:
    case FRX_ADD_FCS1:
    case FRX_ADD_FCS2:
      destination = ppp_data->frx.cur_desc->buffer;
      d_offset    = ppp_data->frx.cur_desc->len;
      d_size      = ppp_data->frx.cur_desc_size;
      break;
  } /*lint !e744 switch statement has no default */
  /*
   * while there is still data to analyze and 
   * a complete packet is not yet received
   */
  while((source_desc) && (ppp_data->frx.frame_complete EQ FALSE))
  {
    /*
     * while the current descriptor is not yet complete analyzed.
     * if a complete packet is analyzed we leave the loop via break command
     */
    for(s_size = source_desc->len; s_offset < s_size; s_offset++)
    {
      /*
       * set current byte
       */
      currentbyte = source_desc->buffer[s_offset];
      /*
       * detect HDLC flag
       */
      if(currentbyte EQ PPP_HDLC_FLAG)
      {
        if(store_state EQ FRX_ADD_ERROR)
        {
#ifdef PPP_HDLC_TRACE
          /*
           * store next HDLC frame
           */
          /*
           * release old stored HDLC frame
           */
          MFREE_DESC2(ppp_data->frx.hdlc_frame);
          ppp_data->frx.hdlc_frame = NULL;
          /*
           * set source pointer
           */
          trace_desc = source_desc;
          trace_pos  = s_offset;
          /*
           * allocate first descriptor to store new HDLC frame
           */
          MALLOC(trace_desc2, (USHORT)(sizeof(T_desc2) - 1 + 
                                FRX_ADD_SMALL_PACKET_SIZE));
          trace_desc2->len         = 0;
          trace_desc2->next        = (ULONG)NULL;
          ppp_data->frx.hdlc_frame = trace_desc2;

          /*
           * store first HDLC flag
           */
          TRACE_EVENT_P2("start flag found in desc=0x%08x pos=%d", 
                         trace_desc, 
                         trace_pos);
          trace_desc2->buffer[trace_desc2->len] = 
            trace_desc->buffer[trace_pos];
          trace_desc2->len++;
          trace_pos++;
          while((trace_desc) && (trace_pos >= trace_desc->len))
          {
            trace_desc = (T_desc2*)trace_desc->next;
            trace_pos  = 0;
          }
          if(trace_desc)
          {
            /*
             * store HDLC frame
             */
            while((trace_desc) &&
                  (trace_desc->buffer[trace_pos] NEQ PPP_HDLC_FLAG))
            {
              if(trace_desc2->len >= FRX_ADD_SMALL_PACKET_SIZE)
              {
                MALLOC(trace_desc3, (USHORT)(sizeof(T_desc2) - 1 + 
                                      FRX_ADD_SMALL_PACKET_SIZE));
                trace_desc3->len  = 0;
                trace_desc3->next = (ULONG)NULL;
                trace_desc2->next = (ULONG)trace_desc3;
                trace_desc2       = trace_desc3;
              }
              trace_desc2->buffer[trace_desc2->len] = 
                trace_desc->buffer[trace_pos];
              trace_desc2->len++;
              trace_pos++;
              while((trace_desc) && (trace_pos >= trace_desc->len))
              {
                trace_desc = (T_desc2*)trace_desc->next;
                trace_pos  = 0;
              }
            }
            /*
             * store last HDLC flag
             */
            if(trace_desc)
            {
              if(trace_desc2->len >= FRX_ADD_SMALL_PACKET_SIZE)
              {
                MALLOC(trace_desc3, (USHORT)(sizeof(T_desc2) - 1 + 
                                      FRX_ADD_SMALL_PACKET_SIZE));
                trace_desc3->len  = 0;
                trace_desc3->next = (ULONG)NULL;
                trace_desc2->next = (ULONG)trace_desc3;
                trace_desc2       = trace_desc3;
              }
              TRACE_EVENT_P2("stop flag found in desc=0x%08x pos=%d", 
                             trace_desc, 
                             trace_pos);
              trace_desc2->buffer[trace_desc2->len] = 
                trace_desc->buffer[trace_pos];
              trace_desc2->len++;
              trace_pos++;
            }
          }
#endif /* PPP_HDLC_TRACE */
          store_state = FRX_ADD_HDLC_BEGIN;
        }
        /*
         * begin of frame detected
         */
        if(store_state EQ FRX_ADD_HDLC_BEGIN)
        {
          /*
           * initialize new packet
           */
          ppp_data->frx.stored_len   = 0;
          d_offset                   = 0;
          ppp_data->frx.stored_ptype = 0x0000;
          fcs                        = PPP_INITFCS;
          escape                     = FALSE;
          store_state                = FRX_ADD_ADDRESS;
        }
        /*
         * end of frame detected
         * no error occured, frame complete
         * if we are in INFORMATION state we have to keep in mind the FCS at MRU check
         */
        else if(((ppp_data->frx.stored_len + d_offset) >= 4) && 
                (((ppp_data->frx.stored_len + d_offset) <= ppp_data->mru) ||
                 (((ppp_data->frx.stored_len + d_offset) <= 
                   (ppp_data->mru + 2)) && 
                  (store_state EQ FRX_ADD_INFORMATION))) &&
                (fcs EQ PPP_GOODFCS))
        {
          /* 
           * determine the length of the packet
           */
          packet_len                  = 0;
          ppp_data->frx.stored_len   += d_offset;
          ppp_data->frx.cur_desc->len = d_offset;
          switch(store_state)
          {
            case FRX_ADD_INFORMATION:
              packet_len = ppp_data->frx.stored_len - 2;
              break;

            case FRX_ADD_FCS1:
              packet_len = ppp_data->frx.stored_len - 1;
              break;

            case FRX_ADD_FCS2:
              packet_len = ppp_data->frx.stored_len;
              break;
          } /*lint !e744 switch statement has no default */
          if(packet_len < ppp_data->frx.stored_len)
          {
            /*
             * remove FCS
             */
            ppp_data->frx.stored_len = packet_len;
            temp_desc                = ppp_data->frx.stored_packet;
            while(packet_len > temp_desc->len)
            {
              packet_len-= temp_desc->len;
              temp_desc  = (T_desc2*)temp_desc->next;
            }
            temp_desc->len = packet_len;
            /*
             * free the rest of packet
             */
            arb_discard_packet((T_desc2*)temp_desc->next);
            temp_desc->next = (ULONG)NULL;
          }
#ifdef PPP_HDLC_TRACE
          /*
           * remove stored HDLC frame because of correct reception
           * the storage of the next PPP frame is done
           * in the next call of frx_add_desc()
           */
          MFREE_DESC2(ppp_data->frx.hdlc_frame);
          ppp_data->frx.hdlc_frame = NULL;
#endif /* PPP_HDLC_TRACE */
          /*
           * esape loop
           */
          ppp_data->frx.frame_complete = TRUE;
          store_state                  = FRX_ADD_HDLC_BEGIN;
          break;
        }
        /*
         * end of frame detected
         * error occured, discard frame
         */
        else
        {
          /*
           * error traces
           */
          if(((ppp_data->frx.stored_len + d_offset) >= 4) &&
             (fcs NEQ PPP_GOODFCS))
          {
            TRACE_EVENT("ERROR HDLC PACKET PPP FCS FAULT");
          }
          else if((ppp_data->frx.stored_len + d_offset) > ppp_data->mru)
          {
            TRACE_EVENT_P3("ERROR PPP PACKET TO LONG stored_len=%d  d_offset=%d  mru=%d",
                           ppp_data->frx.stored_len,
                           d_offset,
                           ppp_data->mru);
          }
#ifdef PPP_HDLC_TRACE
          if((ppp_data->frx.stored_len + d_offset) > 0)
          {
            /*
             * trace HDLC frame and store next frame
             */
            TRACE_EVENT("wrong HDLC frame:");
            i           = 0;
            trace_pos   = 0;
            trace_desc2 = ppp_data->frx.hdlc_frame;
            while(trace_desc2)
            {
              i+= sprintf(&buf[i], 
                          "0x%02x, ", 
                          trace_desc2->buffer[trace_pos]);
              trace_pos++;
              if(trace_desc2->len <= trace_pos)
              {
                trace_desc2 = (T_desc2*)trace_desc2->next;
                trace_pos   = 0;
              }
              if(i > 80)
              {
                TRACE_EVENT( buf );
                i = 0;
              }
            }
            if(i > 0)
            {
              TRACE_EVENT( buf );
              i = 0;
            }
          }
          /*
           * release stored HDLC frame
           */
          MFREE_DESC2(ppp_data->frx.hdlc_frame);
          ppp_data->frx.hdlc_frame = NULL;
          /*
           * store next HDLC frame
           */
          /*
           * set source pointer
           */
          trace_desc = source_desc;
          trace_pos  = s_offset;
          /*
           * allocate first descriptor to store new HDLC frame
           */
          MALLOC(trace_desc2, (USHORT)(sizeof(T_desc2) - 1 + 
                                FRX_ADD_SMALL_PACKET_SIZE));
          trace_desc2->len         = 0;
          trace_desc2->next        = (ULONG)NULL;
          ppp_data->frx.hdlc_frame = trace_desc2;

          /*
           * store first HDLC flag
           */
          TRACE_EVENT_P2("start flag found in desc=0x%08x pos=%d", 
                         trace_desc, 
                         trace_pos);
          trace_desc2->buffer[trace_desc2->len] = 
            trace_desc->buffer[trace_pos];
          trace_desc2->len++;
          trace_pos++;
          while((trace_desc) && (trace_pos >= trace_desc->len))
          {
            trace_desc = (T_desc2*)trace_desc->next;
            trace_pos  = 0;
          }
          if(trace_desc)
          {
            /*
             * store HDLC frame
             */
            while((trace_desc) &&
                  (trace_desc->buffer[trace_pos] NEQ PPP_HDLC_FLAG))
            {
              if(trace_desc2->len >= FRX_ADD_SMALL_PACKET_SIZE)
              {
                MALLOC(trace_desc3, (USHORT)(sizeof(T_desc2) - 1 + 
                                      FRX_ADD_SMALL_PACKET_SIZE));
                trace_desc3->len  = 0;
                trace_desc3->next = (ULONG)NULL;
                trace_desc2->next = (ULONG)trace_desc3;
                trace_desc2       = trace_desc3;
              }
              trace_desc2->buffer[trace_desc2->len] = 
                trace_desc->buffer[trace_pos];
              trace_desc2->len++;
              trace_pos++;
              while((trace_desc) && (trace_pos >= trace_desc->len))
              {
                trace_desc = (T_desc2*)trace_desc->next;
                trace_pos  = 0;
              }
            }
            /*
             * store last HDLC flag
             */
            if(trace_desc)
            {
              if(trace_desc2->len >= FRX_ADD_SMALL_PACKET_SIZE)
              {
                MALLOC(trace_desc3, (USHORT)(sizeof(T_desc2) - 1 + 
                                      FRX_ADD_SMALL_PACKET_SIZE));
                trace_desc3->len  = 0;
                trace_desc3->next = (ULONG)NULL;
                trace_desc2->next = (ULONG)trace_desc3;
                trace_desc2       = trace_desc3;
              }
              TRACE_EVENT_P2("stop flag found in desc=0x%08x pos=%d", 
                             trace_desc, 
                             trace_pos);
              trace_desc2->buffer[trace_desc2->len] = 
                trace_desc->buffer[trace_pos];
              trace_desc2->len++;
              trace_pos++;
            }
          }
#endif /* PPP_HDLC_TRACE */
          /*
           * remove receiced packet because of an error
           * decrease source offset beacause the HDLC end flag 
           * can also be the HDLC start flag of the next frame
           */
          s_offset--;
          arb_discard_packet(ppp_data->frx.stored_packet);
          ppp_data->frx.stored_packet = NULL;
          store_state                 = FRX_ADD_HDLC_BEGIN;
        }
      }
      /*
       * detect Control Escape octet 
       */
      else if((currentbyte EQ PPP_HDLC_ESCAPE) && (escape EQ FALSE))
      {
        escape = TRUE;
      }
      /*
       * usual octet
       */
      else
      {
        /*
         * bit 5 complement for the octet followed by Control Escape
         */
        if(escape EQ TRUE)
        {
          currentbyte^= 0x20;
          escape      = FALSE;
        }
        /*
         * calculate FCS 
         */
#ifdef _SIMULATION_
        fcs = (fcs << 8) + currentbyte; /*lint !e734 Loss of precision */
#else /* _SIMULATION_ */
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ currentbyte) & 0xff];
#endif /* _SIMULATION_ */
        /*
         * store the packet and determine the protocol 
         */
        switch(store_state)
        {
          case FRX_ADD_INFORMATION:
            if(d_offset >= d_size)
            {
              if((ppp_data->frx.stored_len + d_offset) < ppp_data->mru)
              {
                /*
                 * allocate new descriptor
                 */
                switch(ppp_data->frx.stored_ptype)
                {
                  case DTI_PID_LCP:
                  case DTI_PID_PAP:
                  case DTI_PID_CHAP:
                  case DTI_PID_IPCP:
                    /*
                     * allocate a big descriptor, copy the data into the new
                     * one and free the small descriptor
                     */
                    MALLOC (ppp_data->frx.stored_packet, 
                            (USHORT)(sizeof(T_desc2) - 1 + ppp_data->mru));
                    memcpy(ppp_data->frx.stored_packet->buffer, 
                           destination, 
                           d_offset); /*lint !e668 Possibly passing a null pointer */
                    MFREE(ppp_data->frx.cur_desc);
                    ppp_data->frx.cur_desc       = ppp_data->frx.stored_packet;
                    ppp_data->frx.cur_desc->next = (ULONG)NULL;
                    destination = ppp_data->frx.cur_desc->buffer;
                    d_size      = ppp_data->mru;
                    break;

                  default:
                    /*
                     * allocate a new small descriptor
                     */
                    MALLOC (temp_desc, (USHORT)(sizeof(T_desc2) - 1 + 
                                        FRX_ADD_SMALL_PACKET_SIZE));
                    ppp_data->frx.cur_desc->next = (ULONG)temp_desc;
                    ppp_data->frx.cur_desc->len  = d_offset;
                    ppp_data->frx.cur_desc       = temp_desc;
                    ppp_data->frx.cur_desc->next = (ULONG)NULL;
                    ppp_data->frx.stored_len    += d_offset;
                    destination = ppp_data->frx.cur_desc->buffer;
                    d_offset    = 0;
                    d_size      = FRX_ADD_SMALL_PACKET_SIZE;
                    break;
                }
              }
              else if((ppp_data->frx.stored_len + d_offset) >= 
                      (ppp_data->mru + 2))
              {
                /*
                 * remove receiced packet because it is to long
                 */
                arb_discard_packet(ppp_data->frx.stored_packet);
                ppp_data->frx.stored_packet = NULL;
                store_state = FRX_ADD_ERROR;

                TRACE_EVENT("ERROR PPP: HDLC packet to long");
#ifdef PPP_HDLC_TRACE
                /*
                 * trace HDLC frame
                 */
                TRACE_EVENT("wrong HDLC frame:");
                i           = 0;
                trace_pos   = 0;
                trace_desc2 = ppp_data->frx.hdlc_frame;
                while(trace_desc2)
                {
                  i+= sprintf(&buf[i], 
                              "0x%02x, ", 
                              trace_desc2->buffer[trace_pos]);
                  trace_pos++;
                  if(trace_desc2->len <= trace_pos)
                  {
                    trace_desc2 = (T_desc2*)trace_desc2->next;
                    trace_pos   = 0;
                  }
                  if(i > 80)
                  {
                    TRACE_EVENT( buf );
                    i = 0;
                  }
                }
                if(i > 0)
                {
                  TRACE_EVENT( buf );
                  i = 0;
                }
                /*
                 * release stored HDLC frame
                 */
                MFREE_DESC2(ppp_data->frx.hdlc_frame);
                ppp_data->frx.hdlc_frame = NULL;
#endif /* PPP_HDLC_TRACE */
                break;
              }
              else if((ppp_data->frx.stored_len + d_offset) >= 
                      (ppp_data->mru + 1))
              {
                d_offset--;
                store_state = FRX_ADD_FCS2;
                break;
              }
              else
              {
                store_state = FRX_ADD_FCS1;
                break;
              }
            }
            /*
             * copy data
             */
            destination[d_offset] = currentbyte; /*lint !e613 Possible use of null pointer */
            /*
             * increase destination offset
             */
            d_offset++;
            break;

          case FRX_ADD_CONTROL:
            if(currentbyte EQ 0x03)
              store_state = FRX_ADD_PROTOCOL1;
            else
            {
              TRACE_EVENT("ERROR PPP: wrong HDLC control field");
#ifdef PPP_HDLC_TRACE
              /*
               * trace HDLC frame
               */
              TRACE_EVENT("wrong HDLC frame:");
              i           = 0;
              trace_pos   = 0;
              trace_desc2 = ppp_data->frx.hdlc_frame;
              while(trace_desc2)
              {
                i+= sprintf(&buf[i], "0x%02x, ", trace_desc2->buffer[trace_pos]);
                trace_pos++;
                if(trace_desc2->len <= trace_pos)
                {
                  trace_desc2 = (T_desc2*)trace_desc2->next;
                  trace_pos   = 0;
                }
                if(i > 80)
                {
                  TRACE_EVENT( buf );
                  i = 0;
                }
              }
              if(i > 0)
              {
                TRACE_EVENT( buf );
                i = 0;
              }
              /*
               * release stored HDLC frame
               */
              MFREE_DESC2(ppp_data->frx.hdlc_frame);
              ppp_data->frx.hdlc_frame = NULL;
#endif /* PPP_HDLC_TRACE */
              store_state = FRX_ADD_ERROR;
            }
            break;

          case FRX_ADD_ADDRESS:
            if(currentbyte EQ 0xff)
            {
              store_state = FRX_ADD_CONTROL;
              break;
            }
            /*
             * address and control field compression detected
             */
            /* fall through */
          case FRX_ADD_PROTOCOL1:
            if((currentbyte & 0x01) EQ 0)
            {
              ppp_data->frx.stored_ptype = currentbyte;
              ppp_data->frx.stored_ptype = (ppp_data->frx.stored_ptype << 8); /*lint !e734 Loss of precision */
              store_state = FRX_ADD_PROTOCOL2;
              break;
            }
            /*
             * protocol field compression detected
             */
            /* fall through */
          case FRX_ADD_PROTOCOL2:
            if(currentbyte & 0x01)
            {
              /*
               * store protocol
               */
              ppp_data->frx.stored_ptype|= currentbyte;
              /*
               * allocate new packet
               */
              d_size = FRX_ADD_SMALL_PACKET_SIZE;
              /*
               * Allocate the necessary size for the data descriptor. 
               * The size is calculated as follows:
               * - take the size of a descriptor structure
               * - subtract one because of the array buffer[1] to get
               *   the size of descriptor control information
               * - add number of octets of descriptor data
               */
              MALLOC (ppp_data->frx.stored_packet, 
                      (USHORT)(sizeof(T_desc2) - 1 + d_size));
              ppp_data->frx.cur_desc       = ppp_data->frx.stored_packet;
              ppp_data->frx.cur_desc->next = (ULONG)NULL;
              destination = ppp_data->frx.cur_desc->buffer;
              d_offset    = 0;
              store_state = FRX_ADD_INFORMATION;
            }
            else
            {
              TRACE_EVENT("ERROR PPP: wrong HDLC protocol field");
#ifdef PPP_HDLC_TRACE
              /*
               * trace HDLC frame
               */
              TRACE_EVENT("wrong HDLC frame:");
              i           = 0;
              trace_pos   = 0;
              trace_desc2 = ppp_data->frx.hdlc_frame;
              while(trace_desc2)
              {
                i+= sprintf(&buf[i], "0x%02x, ", trace_desc2->buffer[trace_pos]);
                trace_pos++;
                if(trace_desc2->len <= trace_pos)
                {
                  trace_desc2 = (T_desc2*)trace_desc2->next;
                  trace_pos   = 0;
                }
                if(i > 80)
                {
                  TRACE_EVENT( buf );
                  i = 0;
                }
              }
              if(i > 0)
              {
                TRACE_EVENT( buf );
                i = 0;
              }
              /*
               * release stored HDLC frame
               */
              MFREE_DESC2(ppp_data->frx.hdlc_frame);
              ppp_data->frx.hdlc_frame = NULL;
#endif /* PPP_HDLC_TRACE */
              store_state = FRX_ADD_ERROR;
            }
            break;

          case FRX_ADD_FCS1:
            store_state = FRX_ADD_FCS2;
            break;

          case FRX_ADD_FCS2:
            /*
             * remove receiced packet because its to long
             */
            arb_discard_packet(ppp_data->frx.stored_packet);
            ppp_data->frx.stored_packet = NULL;
            store_state = FRX_ADD_ERROR;
            
            TRACE_EVENT("ERROR PPP: HDLC packet to long");
#ifdef PPP_HDLC_TRACE
            /*
             * trace HDLC frame
             */
            TRACE_EVENT("wrong HDLC frame:");
            i           = 0;
            trace_pos   = 0;
            trace_desc2 = ppp_data->frx.hdlc_frame;
            while(trace_desc2)
            {
              i+= sprintf(&buf[i], "0x%02x, ", trace_desc2->buffer[trace_pos]);
              trace_pos++;
              if(trace_desc2->len <= trace_pos)
              {
                trace_desc2 = (T_desc2*)trace_desc2->next;
                trace_pos   = 0;
              }
              if(i > 80)
              {
                TRACE_EVENT( buf );
                i = 0;
              }
            }
            if(i > 0)
            {
              TRACE_EVENT( buf );
              i = 0;
            }
            /*
             * release stored HDLC frame
             */
            MFREE_DESC2(ppp_data->frx.hdlc_frame);
            ppp_data->frx.hdlc_frame = NULL;
#endif /* PPP_HDLC_TRACE */
            break;

          case FRX_ADD_ERROR:
            /*
             * we wait for the next HDLC flag
             */
            break;

          case FRX_ADD_HDLC_BEGIN:
            TRACE_ERROR("ERROR: frx_add_desc(): in FRX_ADD_HDLC_BEGIN state without HDLC flag");
            break;

          default:
            TRACE_ERROR("ERROR: frx_add_desc(): wrong state");
            break;
        }
      }
    }
    /*
     * check if current descriptor is complete analyzed
     */
    if(s_offset >= s_size)
    {
      temp_desc = (T_desc2*)source_desc->next;
      MFREE(source_desc);
      source_desc = temp_desc;
      s_offset    = 0;
    }
  }
  /*
   * store important values in global variables
   */
  switch(store_state)
  {
    case FRX_ADD_INFORMATION:
    case FRX_ADD_FCS1:
    case FRX_ADD_FCS2:
      ppp_data->frx.cur_desc->len = d_offset;
      break;
  } /*lint !e744 switch statement has no default */
  ppp_data->frx.store_state   = store_state;
  ppp_data->frx.calc_fcs      = fcs;
  ppp_data->frx.escape        = escape;
  ppp_data->frx.received_data = source_desc;
  ppp_data->frx.cur_desc_size = d_size;
  ppp_data->frx.proceed_data  = s_offset;
} /*lint !e550 fcstab not accessed in _SIMULATION_ frx_add_desc() */
#endif /* PPP_FLASH_ROM */



#ifndef PPP_INT_RAM
/*
+------------------------------------------------------------------------------
| Function    : frx_detect_frame
+------------------------------------------------------------------------------
| Description : The function frx_detect_frame() detects begin and end of 
|               PPP frames.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/
GLOBAL void frx_detect_frame ()
{
  UBYTE*   source;
  UBYTE*   destination;
  T_desc2* temp_desc1;
  T_desc2* temp_desc2;
  
  TRACE_FUNCTION( "frx_detect_frame" );

  /*
   * copy received data stream pointer
   */
  temp_desc1 = ppp_data->frx.received_data;
  /*
   * set destination pointer if necessary
   */
  if(ppp_data->frx.store_state EQ FRX_ADD_INFORMATION)
  {
    /*
     * to avoid erroneuos code generation of target compiler,
     * it is written in two lines instead of one
     */
    destination = ppp_data->frx.cur_desc->buffer;
    destination+= ppp_data->frx.cur_desc->len;
  }

  /*
   * while there is still data to analyze and 
   * a complete frame is not yet received
   */
  while((temp_desc1) && (ppp_data->frx.frame_complete EQ FALSE))
  {
    /*
     * set source pointer
     */
    source = &temp_desc1->buffer[ppp_data->frx.proceed_data];
    /*
     * We count down the length of the current descriptor.
     * while the current descriptor is not yet complete analyzed and 
     * a complete frame is not yet received
     */
    while((temp_desc1->len) && (ppp_data->frx.frame_complete EQ FALSE))
    {
      switch(ppp_data->frx.store_state)
      {
        case FRX_ADD_ERROR:
          /*
           * search for next HDLC flag
           */
          while((temp_desc1->len) &&
                (*source NEQ PPP_HDLC_FLAG))
          {
            /*
             * increase source pointer
             */
            source++;
            temp_desc1->len--;
            ppp_data->frx.proceed_data++;
          }
          if(temp_desc1->len EQ 0) /*lint !e661 Possible access of out-of-bounds pointer */
            /*
             * end of descriptor reached
             * analyze next descriptor
             */
            break;
          ppp_data->frx.store_state = FRX_ADD_HDLC_BEGIN;
          /* fall through */
        case FRX_ADD_HDLC_BEGIN:
          /*
           * create new packet (frame) to store detected frame
           */
          ppp_data->frx.stored_len    = 0;
          ppp_data->frx.stored_ptype  = DTI_PID_UOS;
          ppp_data->frx.cur_desc_size = FRX_ADD_SMALL_PACKET_SIZE;
          /*
           * Allocate the necessary size for the data descriptor. The size is 
           * calculated as follows:
           * - take the size of a descriptor structure
           * - subtract one because of the array buffer[1] to get the size of
           *   descriptor control information
           * - add number of octets of descriptor data
           */
          MALLOC (ppp_data->frx.stored_packet,
                  (USHORT)(sizeof(T_desc2) - 1 + FRX_ADD_SMALL_PACKET_SIZE));
          ppp_data->frx.cur_desc       = ppp_data->frx.stored_packet;
          ppp_data->frx.cur_desc->len  = 0;
          ppp_data->frx.cur_desc->next = (ULONG)NULL;
          destination = ppp_data->frx.cur_desc->buffer;

          *destination = *source; /*lint !e613 Possible use of null pointer */
          /*
           * increase source pointer
           */
          source++; /*lint !e613 Possible use of null pointer */
          temp_desc1->len--;
          ppp_data->frx.proceed_data++;
          /*
           * increase destination pointer
           */
          ppp_data->frx.stored_len++;
          destination++;
          ppp_data->frx.cur_desc->len++;

          ppp_data->frx.store_state = FRX_ADD_INFORMATION;
          break;

        case FRX_ADD_INFORMATION:
          /*
           * copy data
           */
          while((temp_desc1->len) &&
                (ppp_data->frx.stored_len < ppp_data->mru) &&
                (ppp_data->frx.cur_desc->len < ppp_data->frx.cur_desc_size) &&
                (*source NEQ PPP_HDLC_FLAG))
          {
            /*
             * copy data
             */
            *destination = *source; /*lint !e644 destination may not have been initialized */
            /*
             * increase source pointer
             */
            source++;
            temp_desc1->len--;
            ppp_data->frx.proceed_data++;
            /*
             * increase destination pointer
             */
            ppp_data->frx.stored_len++;
            destination++;
            ppp_data->frx.cur_desc->len++;
          }
          /*
           * analyze why the loop is left
           */
          if(ppp_data->frx.stored_len < ppp_data->mru) /*lint !e661 Possible access of out-of-bounds pointer */
          {
            if(ppp_data->frx.cur_desc->len >= ppp_data->frx.cur_desc_size)
            {
              /*
               * allocate a new small descriptor
               */
              MALLOC (temp_desc2, (USHORT)(sizeof(T_desc2) - 1 + 
                                  FRX_ADD_SMALL_PACKET_SIZE));
              ppp_data->frx.cur_desc->next = (ULONG)temp_desc2;
              ppp_data->frx.cur_desc       = 
                (T_desc2*)ppp_data->frx.cur_desc->next;
              ppp_data->frx.cur_desc->len  = 0;
              ppp_data->frx.cur_desc->size = 0;
              ppp_data->frx.cur_desc->offset = 0;
              ppp_data->frx.cur_desc->next = (ULONG)NULL;
              ppp_data->frx.cur_desc_size  = FRX_ADD_SMALL_PACKET_SIZE;
              destination = ppp_data->frx.cur_desc->buffer;
            }
            if((temp_desc1->len) &&
               (*source EQ PPP_HDLC_FLAG)) /*lint !e613 Possible use of null pointer */
            {
              /*
               * end of frame detected
               */
              ppp_data->frx.store_state = FRX_ADD_HDLC_BEGIN;
              /*
               * copy HDLC flag
               */
              *destination = *source; /*lint !e613 Possible use of null pointer */
              /*
               * increase length values
               */
              ppp_data->frx.stored_len++;
              ppp_data->frx.cur_desc->len++;
              /*
               * check for correct length
               */
              if(ppp_data->frx.stored_len > 2)
              {
                /*
                 * no error occured, frame complete
                 */
                ppp_data->frx.frame_complete = TRUE;
              }
              else
              {
                /*
                 * remove receiced packet because its to short
                 */
                arb_discard_packet(ppp_data->frx.stored_packet);
                ppp_data->frx.stored_packet = NULL;
              }
            }
          }
          else
          {
            /*
             * remove receiced packet because its to long
             */
            arb_discard_packet(ppp_data->frx.stored_packet);
            ppp_data->frx.stored_packet  = NULL;
            ppp_data->frx.store_state    = FRX_ADD_ERROR;
          }
          break;
      } /*lint !e744 switch statement has no default */
    }
    if(temp_desc1->len EQ 0)
    {
      temp_desc2 = (T_desc2*)temp_desc1->next;
      MFREE(temp_desc1);
      temp_desc1                 = temp_desc2;
      ppp_data->frx.proceed_data = 0;
    }
  }
  /*
   * store received data stream pointer
   */
  ppp_data->frx.received_data = temp_desc1;
} /* frx_detect_frame() */



/*
+------------------------------------------------------------------------------
| Function    : frx_send_pack_ready_mode()
+------------------------------------------------------------------------------
| Description : send as many packets to ptx as can be extracted from received frame.
|               Frx is in ready mode.
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/

GLOBAL void frx_send_pack_ready_mode ()
{
  T_desc2* temp_desc;

  TRACE_FUNCTION( "frx_send_pack_ready_mode ()" );

  while( ppp_data->frx.frame_complete   EQ TRUE            AND
         ppp_data->frx.data_flow_state  EQ FRX_DATA_FLOW_READY)
  {
    /*
     * to avoid any side effects
     * first reset all necessary variables and then call the signal 
     */
    ppp_data->frx.frame_complete = FALSE;
    temp_desc                    = ppp_data->frx.stored_packet;
    ppp_data->frx.stored_packet  = NULL;
    sig_frx_ptx_packet_ind(ppp_data->frx.stored_ptype, 
                           ppp_data->frx.stored_len,
                           temp_desc);
    frx_add_desc();
  }
}


/*
+------------------------------------------------------------------------------
| Function    : frx_send_pack_transp_mode
+------------------------------------------------------------------------------
| Description : send as many packets to ptx as can be extracted from received frame.
|               Frx is in transparent mode .
|
| Parameters  : no parameters
|
+------------------------------------------------------------------------------
*/

GLOBAL void frx_send_pack_transp_mode ()
{
  T_desc2* temp_desc;

  TRACE_FUNCTION( "frx_send_pack_transp_mode ()" );

  while( ppp_data->frx.frame_complete  EQ TRUE            AND
         ppp_data->frx.data_flow_state EQ FRX_DATA_FLOW_READY)
  {
    /*
     * to avoid any side effects
     * first reset all necessary variables and then call the signal 
     */
    ppp_data->frx.frame_complete = FALSE;
    temp_desc                    = ppp_data->frx.stored_packet;
    ppp_data->frx.stored_packet  = NULL;
    sig_frx_ptx_packet_ind(ppp_data->frx.stored_ptype, 
                           ppp_data->frx.stored_len,
                           temp_desc);
    frx_detect_frame();
  }

}

#endif  /* PPP_INT_RAM */

