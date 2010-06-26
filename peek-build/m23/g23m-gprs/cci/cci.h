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
|  Purpose :  Definitions for entity CCI.
+----------------------------------------------------------------------------- 
*/ 


#ifndef CCI_H
#define CCI_H

#ifndef TI_PS_OP_CIPH_DRIVER

#if !defined(_GEA_SIMULATION_) && !defined(LL_2to1)
#include "config/board.cfg"
#endif /* !_GEA_SIMULATION_ && !LL_2to1 */

/*
 * GEA-HW related constants
 */
#if (BOARD == 0 || BOARD == 50 || BOARD == 51 || BOARD == 52 || BOARD == 53)	/* Dolo1-equipped boards, e.g. F-Sample/Dolo1 */
#define START_ADRESS     0xFFFE6800       /* address independent from bit mapping */
#else

#if (BOARD == 61)	/* G-Sample */
#define START_ADRESS     0xFFFB6800       /* address independent from bit mapping */
#else			/* D-Sample, also in I-Sample */
#define START_ADRESS     0xFFFFC000       /* address independent from bit mapping */
#endif

#endif

/*
 * Macros
 */

/* CNTL_REG R/W flags */
#define RESET_UL         0x0001           /* check bit to get UL reset in progress */
#define RESET_DL         0x0002           /* check bit to get DL reset in progress */

#define NRESET_UL        ~(RESET_UL)      /* mask to init HW reset in UL direction */
#define NRESET_DL        ~(RESET_DL)      /* mask to init HW reset in DL direction */

#define START            0x0004           /* set bit to start HW module */

#define CL_ENABLE        0x0008           /* set bit to enable internal clock */
#define CL_DISABLE       ~(CL_ENABLE)     /* mask to disable internal clock */

#define UL_DL_DOWN       0x0010           /* set bit to select downlink direction */
#define UL_DL_UP         ~(UL_DL_DOWN)    /* mask to set uplink direktion */

#define IT_ENABLE        0x0020           /* set bit to enable HW interrupt */
#define IT_DISABLE       ~(IT_ENABLE)     /* mask to disable HW interrupt */

/* CONF_UL_REG1 and CONF_DL_REG1 R/W flags */
#define INPUT_SHIFT      0x0001           /* set bit to set ignore byte one flag */
#define NO_INPUT_SHIFT   ~(INPUT_SHIFT)   /* mask to clear ignore byte one flag */

#define ENCRYPT          0x0008           /* set bit to enable encryption */
#define NO_ENCRYPT       ~(ENCRYPT)       /* mask to disable encryption */

#define PROTECTED        0x0010           /* set bit to enable protection mode */
#define NON_PROTECTED    ~(PROTECTED)     /* mask to disable protection mode */

#define FCS_COMPUTED     0x0020           /* set bit to enable FCS computation */
#define FCS_NOT_COMPUTED ~(FCS_COMPUTED)  /* mask to disable FCS computation */

#define D_DL             0x0040           /* set bit to enable HW in DL direction */
#define D_UL             ~(D_DL)          /* mask to enable HW in UL direction */

#define GEA_SECOND       0x0080           /* set bit to enable Second GEA algorithm */
#define GEA_FIRST        ~(GEA_SECOND)    /* mask to enable First GEA algorithm  */

/* STATUS_REQ read only flags (R/W if _GEA_SIMULATION_ is set) */
#define CHECK_WORKING    0x0001           /* check bit to get TRUE if HW is working */
#define NOT_WORKING      ~(CHECK_WORKING) /* mask to simulate HW is not working */

#define FCS_FALSE_BIT    0x0002           /* check bit to get TRUE if FSC is bad */

#endif /* TI_PS_OP_CIPH_DRIVER */

/*==== TYPES ======================================================*/


  /*
   * CCI is embedded in LLC thus use llc_data instead of cci_data.
   * the CCI services must then be defined in llc_data, of course.
   */
  #define cci_data              llc_data



/*==== EXPORT =====================================================*/


#endif /* CCI_H */
