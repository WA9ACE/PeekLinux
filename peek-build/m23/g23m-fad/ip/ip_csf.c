/* 
+----------------------------------------------------------------------------- 
|  Project :  GSM-FaD (8411)
|  Modul   :  IP_CSF
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
|  Purpose :  This Modul defines the custom specific functions
|             for the component Radio Link Protocol
|             of the mobile station
+----------------------------------------------------------------------------- 
*/ 

#define ENTITY_IP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "pconst.cdg"
#include "custom.h"
#include "gsm.h"
#include "cnf_ip.h"
#include "mon_ip.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "ccdapi.h"

#include "dti.h"
#include "ip.h"

/*==== EXPORT =====================================================*/

/*==== PRIVAT =====================================================*/

/*==== VARIABLES ==================================================*/

/*==== FUNCTIONS ==================================================*/
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_CSF              |
| STATE   : code                       ROUTINE : csf_init_timer      |
+--------------------------------------------------------------------+

  PURPOSE : Initialize the timer.

*/

GLOBAL BOOL csf_init_timer (void)
{
  USHORT        i;

  TRACE_FUNCTION ("csf_init_timer()");

  for (i=0; i < MAX_IP_TIMER; i++)
  {
    ip_data->lola.timer_reass_running[i % MAX_SEGM_SERVER] = FALSE;
  }
  
  TRACE_FUNCTION ("csf_init_timer() RETURN TRUE");

  return TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_CSF              |
| STATE   : code                       ROUTINE : csf_close_timer     |
+--------------------------------------------------------------------+

  PURPOSE : Closes all timer of the timer pool.

*/

GLOBAL void csf_close_timer (void)
{
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_CSF              |
| STATE   : code                       ROUTINE : csf_stop_timer      |
+--------------------------------------------------------------------+

  PURPOSE : The function stops one timer.

*/


GLOBAL void csf_stop_timer (USHORT index)
{
  vsi_t_stop (VSI_CALLER index);
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_CSF              |
| STATE   : code                       ROUTINE : csf_timer_expired   |
+--------------------------------------------------------------------+

  PURPOSE : After timeout the according instance is searched. 
  
*/


GLOBAL BOOL csf_timer_expired (USHORT           index,
                               USHORT         * layer,
                               USHORT         * segm_server,
                               USHORT         * timer)
{
  if (index < MAX_IP_TIMER)
  {
    * layer       = (USHORT) (index / MAX_SEGM_SERVER);
    * segm_server = (USHORT) (index % MAX_SEGM_SERVER);
    ip_data->lola.timer_reass_running[*segm_server] = FALSE;
    * timer      = TREASSEMBLY;
		
    return TRUE;
	}

  *timer = NOT_PRESENT_16BIT;

  return FALSE;
}

