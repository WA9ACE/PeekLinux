/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  cci_gea_start.c
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
|  Purpose :  This module implements local functions for service FBS of
|             entity CCI.
+----------------------------------------------------------------------------- 
*/ 

#define CCI_GEA_START_C

#ifndef CCI_THREAD
  #define ENTITY_LLC
#else
  #define ENTITY_CCI
#endif


/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "cci.h"        /* to get the global entity definitions */
#ifndef CCI_THREAD
#include "llc.h"        /* to get the global entity definitions */
#endif

#ifndef _SIMULATION_
#include "config/chipset.cfg"
#endif /* ifdef SIMULATION */


/*==== CONST ================================================================*/

/*==== LOCAL VARS ===========================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : gea_start
+------------------------------------------------------------------------------
| Description : The function  .... 
|
| Parameters  : void
|
+------------------------------------------------------------------------------
*/


/* GEA work-around only affects Calypso+ chipset. */
  #ifndef CCI_FLASH_ROM
    /* Assembly routine gea_start() should be executed from the Internal RAM */
    void gea_start(void){
#if (CHIPSET == 12) || (CHIPSET == 14)
      /* Enable Timer1 clock */
      *(volatile USHORT *) 0xfffe3800 |= 0x0020; 

      /* Save into the stack ARM registers r0 to r4 */
      asm(" push {r0-r4}");

      /* Load in r0 READ_TIMER1 address */
      asm(" ldr r0, READ_TIMER1_ADDR");

      /* Set bit START in GEA_CNTL register */
      asm(" ldr r1, GEA_CNTL_ADDR");
      asm(" mov r4,#4");
      asm(" ldrh r2, [r1]");
      asm(" orr r2,r4");
      asm(" strh r2,[r1]");

      /* Load Timer1 in r3 in order to prevent DMA access on strobe0 during the
         GEA clock switch. This reading of Timer1 generates around 13 cycles of
         MCU clock activity on strobe1 and so guaranties that strobe0 will be
         quiet during that time. */
      asm(" ldrh r3,[r0]");

      /* Restore ARM registers r0 to r4 from the stack */
      asm(" pop {r0-r4}");

      /*  Branch to end of the function, to avoid that the 2 following constant 
          definitions are mistaken with code instructions. */
      asm(" br    $gea_end_func");

      /* Must be defined inside the function body, else depending on the compiler 
         version, they are not inserted in the right section */
    asm("READ_TIMER1_ADDR .long 0xFFFE3804");
    asm("GEA_CNTL_ADDR  .long 0xffffc000");

      asm("$gea_end_func:");      
    }
#else
    }    
#endif /*(CHIPSET == 12)*/

  #endif /* CCI_FLASH_ROM */

