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
|  Purpose :  Default values of the LLC layer parameters 
|             (conforming to GSM 04.64)
+----------------------------------------------------------------------------- 
*/ 


#ifndef LLC_PAR_H
#define LLC_PAR_H


/*
 * Default values for the LLC layer parameters (conforming to GSM 04.64).
 */

#define LLC_VERSION_ALL_SAPIS           0

#define LLC_IOV_UI_ALL_SAPIS            0

/*
 * The default IOV-I values are calculated with the following formula:
 * 2^27 * SAPI
 */
#define LLC_IOV_I_SAPI_3                402653184L
#define LLC_IOV_I_SAPI_5                671088640L
#define LLC_IOV_I_SAPI_9                1207959552L
#define LLC_IOV_I_SAPI_11               1476395008L

/*
 * Timer values are negotiated in units of 0.1 seconds but are stored
 * in milliseconds.
 * NOTE: T201 uses value of T200.
 */
#define LLC_T200_SAPI_1                 (50 * XID_TIMER_CONVERSION)
#define LLC_T200_SAPI_3                 (50 * XID_TIMER_CONVERSION)
#define LLC_T200_SAPI_5                 (100 * XID_TIMER_CONVERSION)
#define LLC_T200_SAPI_7                 (200 * XID_TIMER_CONVERSION)
#define LLC_T200_SAPI_9                 (200 * XID_TIMER_CONVERSION)
#define LLC_T200_SAPI_11                (400 * XID_TIMER_CONVERSION)

#define LLC_N200_SAPI_1                 3
#define LLC_N200_SAPI_3                 3
#define LLC_N200_SAPI_5                 3
#define LLC_N200_SAPI_7                 3
#define LLC_N200_SAPI_9                 3
#define LLC_N200_SAPI_11                3

#define LLC_N201_U_SAPI_1               400
#define LLC_N201_U_SAPI_3               500
#define LLC_N201_U_SAPI_5               500
#define LLC_N201_U_SAPI_7               270
#define LLC_N201_U_SAPI_9               500
#define LLC_N201_U_SAPI_11              500

#define LLC_N201_I_SAPI_3               1503
#define LLC_N201_I_SAPI_5               1503
#define LLC_N201_I_SAPI_9               1503
#define LLC_N201_I_SAPI_11              1503

#define LLC_MD_SAPI_3                   1520
#define LLC_MD_SAPI_5                   760
#define LLC_MD_SAPI_9                   380
#define LLC_MD_SAPI_11                  190

#define LLC_MU_SAPI_3                   1520
#define LLC_MU_SAPI_5                   760
#define LLC_MU_SAPI_9                   380
#define LLC_MU_SAPI_11                  190

#define LLC_KD_SAPI_3                   16
#define LLC_KD_SAPI_5                   8
#define LLC_KD_SAPI_9                   4
#define LLC_KD_SAPI_11                  2

#define LLC_KU_SAPI_3                   16
#define LLC_KU_SAPI_5                   8
#define LLC_KU_SAPI_9                   4
#define LLC_KU_SAPI_11                  2


#endif /* LLC_PAR_H */
