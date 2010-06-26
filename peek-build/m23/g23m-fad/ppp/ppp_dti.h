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
|  Purpose :  DTI Definitions for Point-to-Point Protocol (PPP).
+----------------------------------------------------------------------------- 
*/ 

#ifndef PPP_DTI_H
#define PPP_DTI_H


/*==== CONST ================================================================*/

/*==== TYPES ================================================================*/


EXTERN void sig_any_dti_data_ind (
  DTI_HANDLE hDTI,
  T_DTI2_DATA_IND *dti_data_ind,
  UBYTE instance,
  UBYTE inter_face,
  UBYTE channel,
  UBYTE p_id
  );

EXTERN void sig_callback(
  U8 instance,
  U8 inter_face,
  U8 channel, 
  U8 reason,
  T_DTI2_DATA_IND *dti_data_ind
  );

#endif /* PPP_DTI_H */
