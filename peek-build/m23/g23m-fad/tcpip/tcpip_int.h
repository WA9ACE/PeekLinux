/* 
+------------------------------------------------------------------------------
|  File:       tcpip_int.h
+------------------------------------------------------------------------------
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
|  Purpose :  export declarations for use by the rnet_rt_*.c files
+----------------------------------------------------------------------------- 
*/ 

/* We don't just include tcpip.h in, for instance, rnet_rt_ng_ifinput.c,
 * because it needs a lot of other includes.
 */

#ifndef TCPIP_INT_H
#define TCPIP_INT_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES =================================================================*/

/*==== EXPORTS ===============================================================*/

/** Send a message to self.
 * 
 * @param msg_p    pointer to message
 * @param msg_id   message identification
 */
void tcpip_send_internal_ind(unsigned long msg_p, unsigned long msg_id) ;

/** Callback for rnet_get_host_info().
 * 
 * @param 
 * @return 
 */
void tcpip_hostinfo_callback(void *msg) ;


#endif /* !TCPIP_INT_H */

