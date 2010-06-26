/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-F&D (8411)
|  Modul   :  RA_SHM
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
|  Purpose :  This Modul defines the functions for accessing the
|             shared memory segment between MCU and DSP
+----------------------------------------------------------------------------- 
*/ 

#ifndef RA_SHM_C
#define RA_SHM_C
#endif

#define ENTITY_RA

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "cnf_ra.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"
#include "ra.h"

/*==== CONST ======================================================*/
/*==== TYPES ======================================================*/
/*==== VARIABLES ==================================================*/
/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_SHM                   |
| STATE   : code                  ROUTINE : shm_set_dsp_buffer       |
+--------------------------------------------------------------------+

  PURPOSE :
*/

GLOBAL USHORT shm_set_dsp_buffer(T_FRAME_DESC *data, register USHORT *buffer, USHORT offs, USHORT free)
{
  register UBYTE  *b;
  USHORT len, segment;

  buffer = (USHORT*) ( ( (UBYTE*) buffer ) + offs );
  /*
   * write the both segments specified by the frame descriptor data to the DSP mem.
   */
  for (segment = 0; segment < 2; segment++)
  {
    /*
     * limitate the length fields;
     */
    len = free;

    if (len > data->Len[segment])
      len = data->Len[segment];

    if (len)
    {
      data->Len[segment] -= len;
      b = data->Adr[segment];
      {
        memcpy( (UBYTE*) buffer, (UBYTE*) b, (USHORT) len );
        buffer = (USHORT*) ( ( (UBYTE*) buffer ) + len );
        free -= len;
        offs += len;
      }
    }
  }
  return offs;
}

GLOBAL U8 shm_set_dsp_buffer_new(T_FD *pFD, USHORT *buffer, U8 offs, U8 free)
{
  U8 len;

  buffer = (USHORT*)(((UBYTE*)buffer) + offs);
  /*
   * limitate the length fields;
   */
  len = MINIMUM(free, pFD->len);
  if (len)
  {
    memcpy((UBYTE*)buffer, (UBYTE*)pFD->buf, (USHORT)len);
    buffer = (USHORT*)(((UBYTE*)buffer) + len);
  }
  return len;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)        MODULE  : RA_SHM                   |
| STATE   : code                  ROUTINE : shm_get_dsp_buffer       |
+--------------------------------------------------------------------+

  PURPOSE :

*/
GLOBAL USHORT shm_get_dsp_buffer(T_FRAME_DESC *data, register USHORT *buffer, USHORT bytes_to_read)
{
  register UBYTE  *b;
  USHORT  offs = 0;
  USHORT  len, segment;
  /*
   * read the both segments specified by the frame descriptor data from the DSP mem
   */
  for (segment = 0; segment < 2; segment++)
  {
    len = MINIMUM(data->Len[segment], bytes_to_read);
    if (len)
    {
      data->Len[segment] -= len;
      b = data->Adr[segment];
#ifdef TI_PS_16BIT_CPY
      api_memcpy((UBYTE*)b, (UBYTE*)buffer, (USHORT)len );
#else
      memcpy((UBYTE*)b, (UBYTE*)buffer, (USHORT)len );
#endif
      buffer = (USHORT*)(((UBYTE*)buffer) + len);
      bytes_to_read -= len;
      offs += len;
    }
  }
  return offs;
}

GLOBAL U8 shm_get_dsp_buffer_new(USHORT *buffer, U8 bytes_to_read, T_FD *pFD)
{
  U8 len = MINIMUM(pFD->len, bytes_to_read);
  if (len)
  {
    pFD->len = len;
#ifdef TI_PS_16BIT_CPY
    api_memcpy(pFD->buf, (UBYTE*)buffer, len);
#else
    memcpy(pFD->buf, (UBYTE*)buffer, len);
#endif
  }
  return len;
}

