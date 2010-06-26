/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-PS
|  Modul   :  DRV_RX
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
|  Purpose :  This Module defines the engineering mode
|             device driver for the G23 protocol stack.
|             
|             This driver is used to control all engineering mode related
|             functions. 
+----------------------------------------------------------------------------- 
*/ 

#ifndef DRV_EM_C
#define DRV_EM_C
/*==== INCLUDES ===================================================*/
#if defined (NEW_FRAME)

#include <string.h>
#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "gdi.h"
#include "em.h"

#else

#include <string.h>
#include "stddefs.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "vsi.h"
#include "gdi.h"
#include "em.h"

#endif

/*==== EXPORT =====================================================*/

/*==== VARIABLES ==================================================*/
#define EV_BUFFER_SIZE 512


UBYTE  ev_enabled = FALSE;
USHORT ev_first_read;
USHORT ev_first_write;
USHORT ev_size;
UBYTE  ev_buffer [EV_BUFFER_SIZE];
/*==== FUNCTIONS ==================================================*/
LOCAL void  em_get_size                  (UBYTE          size);
LOCAL UBYTE em_check_em_class_infra_data (UBYTE          em_subclass,
                                          UBYTE          em_type,
                                          em_data_type * out_em_data);
LOCAL UBYTE em_check_em_subclass_sc_info (UBYTE          em_type,
                                          em_data_type * out_em_data);
LOCAL UBYTE em_check_em_subclass_nc_info (UBYTE          em_type,
                                          em_data_type * out_em_data);
LOCAL UBYTE em_check_em_subclass_lup_and_pag (UBYTE          em_type,
                                              em_data_type * out_em_data);

/*==== CONSTANTS ==================================================*/
#ifndef FF_EM_MODE 
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_EM                     |
| STATE   : code                ROUTINE : em_Init                    |
+--------------------------------------------------------------------+

  PURPOSE : The function initializes the driver´s internal data.
            The function returns DRV_OK in case of a successful
            completition. The function returns DRV_INITIALIZED if
            the driver has already been initialized and is ready to
            be used or is already in use. In case of an initialization
            failure, which means the that the driver cannot be used,
            the function returns DRV_INITFAILURE.

*/

GLOBAL UBYTE em_Init (void)
{
  ev_enabled = FALSE;

  return DRV_OK;
}
#endif /* FF_EM_MODE */

#ifndef FF_EM_MODE 
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_EM                     |
| STATE   : code                ROUTINE : em_Exit                    |
+--------------------------------------------------------------------+

  PURPOSE : The function is used to indicate that the driver
            and its functionality isn´t needed anymore.

*/

GLOBAL void em_Exit (void)
{
}
#endif /* FF_EM_MODE */

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_EM                     |
| STATE   : code                ROUTINE : em_Read_Parameter          |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to read a parameter of the mobile.
  
*/

GLOBAL UBYTE em_Read_Parameter (UBYTE          em_class,
                                UBYTE          em_subclass,
                                UBYTE          em_type,
                                em_data_type * out_em_data)
{
  switch (em_class)
  {
    case EM_CLASS_INFRA_DATA:
      return (em_check_em_class_infra_data (em_subclass, em_type, out_em_data));
    default:
      return EM_INVALID_CLASS;
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_EM                     |
| STATE   : code                ROUTINE : em_Enable_Post_Mortem      |
+--------------------------------------------------------------------+

  PURPOSE : This function enables recording of event data in the 
            post mortem memory.
              
*/

GLOBAL UBYTE em_Enable_Post_Mortem ()
{
  ev_first_read  = 0;
  ev_first_write = 0;
  ev_size        = EV_BUFFER_SIZE;
  ev_enabled     = TRUE;
  return DRV_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_EM                     |
| STATE   : code                ROUTINE : em_Disable_Post_Mortem     |
+--------------------------------------------------------------------+

  PURPOSE : This function disables recording of event data in the 
            post mortem memory and allows reading of the data.
              
*/

GLOBAL UBYTE em_Disable_Post_Mortem ()
{
  ev_enabled     = FALSE;
  return DRV_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6103)       MODULE  : DRV_EM                     |
| STATE   : code                ROUTINE : em_Read_Post_Mortem        |
+--------------------------------------------------------------------+

  PURPOSE : This function is used to read a parameter of the 
            post mortem memory.
  
*/

GLOBAL UBYTE em_Read_Post_Mortem (em_data_type * out_em_data)
{
  UBYTE subclass;
  UBYTE i;
#if defined (WIN32)
  char buf[60];
  sprintf (buf, "before %d %d read", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif
  if (ev_enabled EQ FALSE)
  {
    if (ev_size EQ EV_BUFFER_SIZE)
      return EM_NO_MORE_DATA;

    out_em_data->em_class    = EM_CLASS_EVENT_TRACE;
    subclass = ev_buffer [ev_first_read++];
    out_em_data->em_subclass = subclass & 0x3F;
    if (ev_first_read >= EV_BUFFER_SIZE)
      ev_first_read -= EV_BUFFER_SIZE;
    out_em_data->em_type = ev_buffer [ev_first_read++];
    if (ev_first_read >= EV_BUFFER_SIZE)
      ev_first_read -= EV_BUFFER_SIZE;
#if defined (WIN32)
    sprintf (buf, "subclass %x read", subclass);
    TRACE_EVENT (buf);
#endif
    switch (subclass & 0xC0)
    {
      case 0x00:   
        // single entry
        ev_size += 2;
        out_em_data->em_length = 0;
        break;
      case 0x40:   
        // ubyte entry
        ev_size += 3;
        out_em_data->em_length = 1;
        out_em_data->em_parameter[0] = ev_buffer [ev_first_read++];
        if (ev_first_read >= EV_BUFFER_SIZE)
          ev_first_read -= EV_BUFFER_SIZE;
        break;
      case 0x80:   
        // ushort entry
        ev_size += 4;
        out_em_data->em_length = 2;
        out_em_data->em_parameter[0] = ev_buffer [ev_first_read++];
        if (ev_first_read >= EV_BUFFER_SIZE)
          ev_first_read -= EV_BUFFER_SIZE;
        out_em_data->em_parameter[1] = ev_buffer [ev_first_read++];
        if (ev_first_read >= EV_BUFFER_SIZE)
          ev_first_read -= EV_BUFFER_SIZE;
        break;
      case 0xC0:   
        // array entry
        // skip to length
        out_em_data->em_length = ev_buffer [ev_first_read++];
        if (ev_first_read >= EV_BUFFER_SIZE)
          ev_first_read -= EV_BUFFER_SIZE;
        for (i=0;i<out_em_data->em_length;i++)
        {
          out_em_data->em_parameter[i] = ev_buffer[ev_first_read++];
          if (ev_first_read >= EV_BUFFER_SIZE)
            ev_first_read -= EV_BUFFER_SIZE;
        }
        ev_size += (out_em_data->em_length+3);
        break;
    }
#if defined (WIN32)
    sprintf (buf, "after %d %d read", ev_first_read, ev_first_write);
    TRACE_EVENT (buf);
#endif
    return DRV_OK;
  }
  else
  {
    return EM_INVALID_ACCESS;
  }
}


/*
 *************************************************************************************
 *
 * Internal Functions
 *
 *************************************************************************************
 */

/*
 * the internal event buffer is organised as a ring buffer. Using different
 * access functions the data is stored in one of the four following ways:
 *
 * SINGLE TRACE (no parameter)
 *
 * Byte 1:     00 xxxxxx  subclass
 * Byte 2:     yyyyyyyyy  type
 *
 * UBYTE TRACE (one byte parameter)
 *
 * Byte 1:     01 xxxxxx  subclass
 * Byte 2:     yyyyyyyyy  type
 * Byte 3:     zzzzzzzzz  parameter
 *
 * USHORT TRACE (two bytes parameter)
 *
 * Byte 1:     10 xxxxxx  subclass
 * Byte 2:     yyyyyyyyy  type
 * Byte 3:     zzzzzzzzz  parameter
 * Byte 4:     zzzzzzzzz  parameter
 *
 * ARRAY TRACE (n bytes parameter)
 *
 * Byte 1:     11 xxxxxx  subclass
 * Byte 2:     yyyyyyyyy  type
 * Byte 3:     aaaaaaaaa  len
 * Byte 4-n+3: zzzzzzzzz  parameter
 *
 * The parameter ev_enabled defines whether recording is enabled or not.
 * after power on it is disabled.
 *
 * The parameter ev_first_read indicates the first position in the ev_buffer
 * which shall be read.
 * 
 * The parameter ev_first_write indicates the first position in the ev_buffer
 * which shall be used for the next recorded event.
 *
 * The parameter ev_size defines the free memory in the ev_buffer. If the next
 * write event is greater than the free memory the ev_first_read is changed to
 * have enough memory.
 *
 */

GLOBAL void em_trace_single (UBYTE class,
                             UBYTE subclass,
                             UBYTE type)
{
#if defined (WIN32)
  char buf[60];
#endif
  TRACE_FUNCTION ("em_trace_single");

#if defined (WIN32)  
  sprintf (buf, "before %d %d Single", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif
  if (ev_enabled)
  {
    // get memory if needed
    em_get_size (2);
    // write subclass
    ev_buffer[ev_first_write++] = subclass;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write type
    ev_buffer[ev_first_write++] = type;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    ev_size -= 2;
  }
#if defined (WIN32)
  sprintf (buf, "after %d %d Single", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif
}

GLOBAL void em_trace_ubyte  (UBYTE  class,
                             UBYTE  subclass,
                             UBYTE  type,
                             UBYTE  para_ubyte)
{
#if defined (WIN32)
  char buf[60];

  TRACE_FUNCTION ("em_trace_ubyte");

  sprintf (buf, "before %d %d ubyte", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif

  if (ev_enabled)
  {
    // get memory if needed
    em_get_size (3);
    // write subclass
    ev_buffer[ev_first_write++] = subclass + 0x40;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write type
    ev_buffer[ev_first_write++] = type;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write parameter
    ev_buffer[ev_first_write++] = para_ubyte;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    ev_size -= 3;
  }
#if defined (WIN32)
  sprintf (buf, "after %d %d ubyte", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif
}

GLOBAL void em_trace_ushort (UBYTE  class,
                             UBYTE  subclass,
                             UBYTE  type,
                             USHORT para_ushort)
{
#if defined (WIN32)
  char buf[60];

  TRACE_FUNCTION ("em_trace_ushort");
  
  sprintf (buf, "before %d %d ushort", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif

  if (ev_enabled)
  {
    // get memory if needed
    em_get_size (4);
    // write subclass
    ev_buffer[ev_first_write++] = subclass + 0x80;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write type
    ev_buffer[ev_first_write++] = type;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write parameter
    ev_buffer[ev_first_write++] = para_ushort >> 8;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write parameter
    ev_buffer[ev_first_write++] = para_ushort & 8;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    ev_size -= 4;
  }
#if defined (WIN32)
  sprintf (buf, "after %d %d ushort", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif
}


GLOBAL void em_trace_array  (UBYTE    class,
                             UBYTE    subclass,
                             UBYTE    type,
                             UBYTE  * array,
                             UBYTE    length)
{
  UBYTE i;
#if defined (WIN32)
  char buf[60];

  TRACE_FUNCTION ("em_trace_array");
  
  sprintf (buf, "before %d %d array", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
#endif

  if (ev_enabled)
  {
    // get memory if needed
    em_get_size ((UBYTE)(length+3));
    // write subclass
    ev_buffer[ev_first_write++] = subclass + 0xC0;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write type
    ev_buffer[ev_first_write++] = type;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write length
    ev_buffer[ev_first_write++] = length;
    if (ev_first_write >= EV_BUFFER_SIZE)
      ev_first_write -= EV_BUFFER_SIZE;
    // write parameter
    for (i=0;i<length;i++)
    {
      ev_buffer[ev_first_write++] = array[i];
      if (ev_first_write >= EV_BUFFER_SIZE)
        ev_first_write -= EV_BUFFER_SIZE;
    }
    ev_size -= (length+3);
  }
#if defined (WIN32)
  sprintf (buf, "after %d %d array", ev_first_read, ev_first_write);
  TRACE_EVENT (buf);
  sprintf (buf, "%x %x %x %x %x",
                ev_buffer[0],
                ev_buffer[1],
                ev_buffer[2],
                ev_buffer[3],
                ev_buffer[4]);
  TRACE_EVENT (buf);
#endif
}


LOCAL void em_get_size (UBYTE size)
{
  while (ev_size < size)
  {
    /*
     * increase ev_first_read to get memory
     */
    switch (ev_buffer [ev_first_read & 0xC0])
    {
      case 0x00:   
        // single entry
        ev_size += 2;
        ev_first_read += 2;
        break;
      case 0x40:   
        // ubyte entry
        ev_size += 3;
        ev_first_read += 3;
        break;
      case 0x80:   
        // ushort entry
        ev_size += 3;
        ev_first_read += 3;
        break;
      case 0xC0:   
        // array entry
        // skip to length
        ev_first_read += 2;
        if (ev_first_read >= EV_BUFFER_SIZE)
          ev_first_read -= EV_BUFFER_SIZE;
        ev_first_read += (1+ev_buffer[ev_first_read]);
        break;
    }
    // align if at the end of the buffer
    if (ev_first_read >= EV_BUFFER_SIZE)
      ev_first_read -= EV_BUFFER_SIZE;
  }
}

LOCAL UBYTE em_check_em_class_infra_data (UBYTE          em_subclass,
                                          UBYTE          em_type,
                                          em_data_type * out_em_data)
{
  switch (em_subclass)
  {
    case EM_SUBCLASS_SC_INFO:
      return (em_check_em_subclass_sc_info (em_type, out_em_data));
    case EM_SUBCLASS_NC_INFO:
      return (em_check_em_subclass_nc_info (em_type, out_em_data));
    case EM_SUBCLASS_LUP_AND_PAG:
      return (em_check_em_subclass_lup_and_pag (em_type, out_em_data));
    default:
      return EM_INVALID_SUBCLASS;
  }
}

LOCAL UBYTE em_check_em_subclass_sc_info (UBYTE          em_type,
                                          em_data_type * out_em_data)
{
  T_EM_NC_DATA em_nc_data;
  /*
   * ret   0   not available
   *       1   idle mode
   *       2   dedicated mode
   */
  UBYTE        ret;

  out_em_data->em_class    = EM_CLASS_INFRA_DATA;
  out_em_data->em_subclass = EM_SUBCLASS_SC_INFO;
  out_em_data->em_type     = em_type;

  ret = em_get_nc_data (&em_nc_data, 6);  // serving cell data
  
  switch (em_type)
  {
    case EM_IN_SC_BCCH_ARFCN:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.arfcn);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_C1:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.c1);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_C2:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.c2);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_BSIC:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.bsic);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_LAC:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.lac);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_BA:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.cba);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_BQ:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.cbq);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_TIL_STATE:
#if !defined(FF_GTI)
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.til_state);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
#endif  /* !FF_GTI */
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_RX:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.rxlev);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_SC_NMR_RAW:
      if (em_get_network_meas (out_em_data->em_parameter) EQ 1)
      {
        out_em_data->em_length = 16;
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */
  
    case EM_IN_SC_BCCH_LIST_RAW:
      if (em_get_bcch_chan_list (out_em_data) NEQ 1)
        return DRV_OK;
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    default:
      return EM_INVALID_TYPE;
  }
}

LOCAL UBYTE em_check_em_subclass_nc_info (UBYTE          em_type,
                                          em_data_type * out_em_data)
{
  T_EM_NC_DATA em_nc_data;
  /*
   * ret   0   not available
   *       1   idle mode
   *       2   dedicated mode
   */
  UBYTE        ret;
  UBYTE        cell_id;

  out_em_data->em_class    = EM_CLASS_INFRA_DATA;
  out_em_data->em_subclass = EM_SUBCLASS_NC_INFO;
  out_em_data->em_type     = em_type;

  switch( em_type )
  {
    case EM_IN_NC_BCCH_1:
    case EM_IN_NC_RX_1:
    case EM_IN_NC_C1_1:
    case EM_IN_NC_C2_1:
    case EM_IN_NC_BSIC_1:
    case EM_IN_NC_CID_1:
    case EM_IN_NC_FN_OFFSET_1:
    case EM_IN_NC_TA_OFFSET_1:
    case EM_IN_NC_LAC_1:
    case EM_IN_NC_BA_1:
    case EM_IN_NC_BQ_1:
    case EM_IN_NC_TIL_STATE_1:
          cell_id = 0;
    break;

    case EM_IN_NC_BCCH_2:
    case EM_IN_NC_RX_2:
    case EM_IN_NC_C1_2:
    case EM_IN_NC_C2_2:
    case EM_IN_NC_BSIC_2:
    case EM_IN_NC_CID_2:
    case EM_IN_NC_FN_OFFSET_2:
    case EM_IN_NC_TA_OFFSET_2:
    case EM_IN_NC_LAC_2:
    case EM_IN_NC_BA_2:
    case EM_IN_NC_BQ_2:
    case EM_IN_NC_TIL_STATE_2:
      cell_id = 1;
    break;

    case EM_IN_NC_BCCH_3:
    case EM_IN_NC_RX_3:
    case EM_IN_NC_C1_3:
    case EM_IN_NC_C2_3:
    case EM_IN_NC_BSIC_3:
    case EM_IN_NC_CID_3:
    case EM_IN_NC_FN_OFFSET_3:
    case EM_IN_NC_TA_OFFSET_3:
    case EM_IN_NC_LAC_3:
    case EM_IN_NC_BA_3:
    case EM_IN_NC_BQ_3:
    case EM_IN_NC_TIL_STATE_3:
      cell_id = 2;
    break;

    case EM_IN_NC_BCCH_4:
    case EM_IN_NC_RX_4:
    case EM_IN_NC_C1_4:
    case EM_IN_NC_C2_4:
    case EM_IN_NC_BSIC_4:
    case EM_IN_NC_CID_4:
    case EM_IN_NC_FN_OFFSET_4:
    case EM_IN_NC_TA_OFFSET_4:
    case EM_IN_NC_LAC_4:
    case EM_IN_NC_BA_4:
    case EM_IN_NC_BQ_4:
    case EM_IN_NC_TIL_STATE_4:
      cell_id = 3;
    break;

    case EM_IN_NC_BCCH_5:
    case EM_IN_NC_RX_5:
    case EM_IN_NC_C1_5:
    case EM_IN_NC_C2_5:
    case EM_IN_NC_BSIC_5:
    case EM_IN_NC_CID_5:
    case EM_IN_NC_FN_OFFSET_5:
    case EM_IN_NC_TA_OFFSET_5:
    case EM_IN_NC_LAC_5:
    case EM_IN_NC_BA_5:
    case EM_IN_NC_BQ_5:
    case EM_IN_NC_TIL_STATE_5:
      cell_id = 4;
    break;

    case EM_IN_NC_BCCH_6:
    case EM_IN_NC_RX_6:
    case EM_IN_NC_C1_6:
    case EM_IN_NC_C2_6:
    case EM_IN_NC_BSIC_6:
    case EM_IN_NC_CID_6:
    case EM_IN_NC_FN_OFFSET_6:
    case EM_IN_NC_TA_OFFSET_6:
    case EM_IN_NC_LAC_6:
    case EM_IN_NC_BA_6:
    case EM_IN_NC_BQ_6:
    case EM_IN_NC_TIL_STATE_6:
      cell_id = 5;
    break;

    case EM_IN_NC_TIL_SYNC_CNT:
    case EM_IN_NC_TIL_BCCH_CNT:
       cell_id = 6; /* Serving cell  */
    break;

    default:
      return EM_DATA_NOT_AVAIL;
  }


  ret = em_get_nc_data (&em_nc_data, cell_id);  /* ncell data  */

  switch (em_type)
  {
    case EM_IN_NC_BCCH_1:
    case EM_IN_NC_BCCH_2:
    case EM_IN_NC_BCCH_3:
    case EM_IN_NC_BCCH_4:
    case EM_IN_NC_BCCH_5:
    case EM_IN_NC_BCCH_6:
      if (ret NEQ 0)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.arfcn);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_RX_1:
    case EM_IN_NC_RX_2:
    case EM_IN_NC_RX_3:
    case EM_IN_NC_RX_4:
    case EM_IN_NC_RX_5:
    case EM_IN_NC_RX_6:
      if (ret NEQ 0)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.rxlev);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_BSIC_1:
    case EM_IN_NC_BSIC_2:
    case EM_IN_NC_BSIC_3:
    case EM_IN_NC_BSIC_4:
    case EM_IN_NC_BSIC_5:
    case EM_IN_NC_BSIC_6:
      if (ret NEQ 0)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.bsic);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_C1_1:
    case EM_IN_NC_C1_2:
    case EM_IN_NC_C1_3:
    case EM_IN_NC_C1_4:
    case EM_IN_NC_C1_5:
    case EM_IN_NC_C1_6:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.c1);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_C2_1:
    case EM_IN_NC_C2_2:
    case EM_IN_NC_C2_3:
    case EM_IN_NC_C2_4:
    case EM_IN_NC_C2_5:
    case EM_IN_NC_C2_6:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.c2);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_LAC_1:
    case EM_IN_NC_LAC_2:
    case EM_IN_NC_LAC_3:
    case EM_IN_NC_LAC_4:
    case EM_IN_NC_LAC_5:
    case EM_IN_NC_LAC_6:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.lac);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_BA_1:
    case EM_IN_NC_BA_2:
    case EM_IN_NC_BA_3:
    case EM_IN_NC_BA_4:
    case EM_IN_NC_BA_5:
    case EM_IN_NC_BA_6:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.cba);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_BQ_1:
    case EM_IN_NC_BQ_2:
    case EM_IN_NC_BQ_3:
    case EM_IN_NC_BQ_4:
    case EM_IN_NC_BQ_5:
    case EM_IN_NC_BQ_6:
      if (ret EQ 1)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3x",em_nc_data.cbq);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_TIL_STATE_1:
    case EM_IN_NC_TIL_STATE_2:
    case EM_IN_NC_TIL_STATE_3:
    case EM_IN_NC_TIL_STATE_4:
    case EM_IN_NC_TIL_STATE_5:
    case EM_IN_NC_TIL_STATE_6:
#if !defined(FF_GTI)
      if (ret NEQ 0)
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.til_state);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
#endif  /* !FF_GTI */
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_TIL_SYNC_CNT:
#if !defined(FF_GTI)
      if ( (ret EQ 1) && (em_nc_data.sync_cnt NEQ 0xFF) )
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.sync_cnt);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
#endif  /* !FF_GTI */
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    case EM_IN_NC_TIL_BCCH_CNT:
#if !defined(FF_GTI)
      if ( (ret EQ 1) && (em_nc_data.bcch_cnt NEQ 0xFF) )
      {
        sprintf ((char *)out_em_data->em_parameter,"%3d",em_nc_data.bcch_cnt);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
      else
#endif  /* !FF_GTI */
        return EM_DATA_NOT_AVAIL;
      /* break is removed as return is using before break,which will never execute */

    default:
          return EM_INVALID_TYPE;
      /* break is removed as return is using before break,which will never execute */

  }
}

LOCAL UBYTE em_check_em_subclass_lup_and_pag (UBYTE          em_type,
                                              em_data_type * out_em_data)
{
  UBYTE * sys_info_3 = NULL;

  out_em_data->em_class    = EM_CLASS_INFRA_DATA;
  out_em_data->em_subclass = EM_SUBCLASS_LUP_AND_PAG;
  out_em_data->em_type     = em_type;

#if defined(FF_GTI)
  sys_info_3 = em_get_sysinfo_type3 ();  /* sys info 3  */
#else
  sys_info_3 = em_get_sys_info (0x1B);  /* sys info 3   */
#endif  /* FF_GTI */

  switch (em_type)
  {
    case EM_IN_LP_BS_PA_MFRMS:
      if (sys_info_3 EQ NULL)
        return EM_DATA_NOT_AVAIL;
      else
      {
        sprintf ((char *)out_em_data->em_parameter,"%d",(sys_info_3[10]&7)+2);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
    case EM_IN_LP_T3212:
      if (sys_info_3 EQ NULL)
        return EM_DATA_NOT_AVAIL;
      else
      {
        sprintf ((char *)out_em_data->em_parameter,"%d",sys_info_3[11]);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
    case EM_IN_LP_MCC:
      if (sys_info_3 EQ NULL)
        return EM_DATA_NOT_AVAIL;
      else
      {
        USHORT mcc;

        mcc = (sys_info_3[4] & 0x0F)*100 + 
              ((sys_info_3[4] & 0xF0)>>4)*10 +
               (sys_info_3[5] & 0x0F);
        sprintf ((char *)out_em_data->em_parameter,"%d",mcc);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
    case EM_IN_LP_MNC:
      if (sys_info_3 EQ NULL)
        return EM_DATA_NOT_AVAIL;
      else
      {
        USHORT mnc;

        mnc = (sys_info_3[6] & 0x0F)*10 + 
              ((sys_info_3[6] & 0xF0)>>4);
        sprintf ((char *)out_em_data->em_parameter,"%d",mnc);
        out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        return DRV_OK;
      }
    case EM_IN_LP_LAC:
    case EM_IN_LP_LAC_RAW:
      if (sys_info_3 EQ NULL)
        return EM_DATA_NOT_AVAIL;
      else
      {
        USHORT lac;

        lac = (sys_info_3[7] << 8) + 
               sys_info_3[8];
        if (em_type EQ EM_IN_LP_LAC)
        {
          sprintf ((char *)out_em_data->em_parameter,"%d",lac);
          out_em_data->em_length = strlen ((char *)out_em_data->em_parameter);
        }
        else
        {
          out_em_data->em_parameter[0] = sys_info_3[7];
          out_em_data->em_parameter[1] = sys_info_3[8];
          out_em_data->em_length       = 2;
        }
        return DRV_OK;
      }
    case EM_IN_LP_CI_RAW:
      if (sys_info_3 EQ NULL)
        return EM_DATA_NOT_AVAIL;
      else
      {
        out_em_data->em_parameter[0] = sys_info_3[2];
        out_em_data->em_parameter[1] = sys_info_3[3];
        out_em_data->em_length       = 2;
      }
      return DRV_OK;

   default:
      return EM_INVALID_TYPE;
  }
}

#endif
