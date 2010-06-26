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
|  Purpose :  Constants to determine the dimensions of the frame
+-----------------------------------------------------------------------------
*/

#ifndef GPRSCONST_H
#define GPRSCONST_H
#include "chipset.cfg"
#include "swconfig.cfg"

#ifdef MAX_EVENT_GROUPS
#undef MAX_EVENT_GROUPS
#endif

#if (CHIPSET==15) && (REMU==1)
#if (LOCOSTO_LITE==1)
#define MAX_REMU_SIMULTANEOUS_TIMER      15
#define MAX_REMU_ENTITIES                15
#define MAX_REMU_OS_TASKS               15
#define MAX_REMU_SEMAPHORES              15
#define MAX_REMU_EVENT_GROUPS 15
#else
#define MAX_REMU_SIMULTANEOUS_TIMER      30
#define MAX_REMU_ENTITIES                30
#define MAX_REMU_OS_TASKS               30
#if (RELIANCE_FS==1)
#if ((DATALIGHT_NOR==1) && (DATALIGHT_NAND==1))
#define MAX_REMU_SEMAPHORES              90
#else
#define MAX_REMU_SEMAPHORES              75
#endif
#else
#define MAX_REMU_SEMAPHORES              55
#endif
#define MAX_REMU_EVENT_GROUPS 30
#endif
#define MAX_EVENT_GROUPS  		    ( MAX_REMU_ENTITIES + 5 )
#else
#define MAX_REMU_SIMULTANEOUS_TIMER      0
#define MAX_REMU_ENTITIES                0
#define MAX_REMU_OS_TASKS               0
#define MAX_REMU_SEMAPHORES              0
#define MAX_REMU_EVENT_GROUPS 0
#define MAX_EVENT_GROUPS  		   0
#endif



#ifdef BTS  /* BTU is included */
	#ifdef BTT /* BTAV is included */
	#ifdef FMS
	    #define BT_ADAPTER_ADD_ENTITIES   9
	    #define BT_ADAPTER_ADD_OS_TASKS   9
	  #define BT_ADAPTER_ADD_SEMAPHORES  10
	#else
	    #define BT_ADAPTER_ADD_ENTITIES   8
	    #define BT_ADAPTER_ADD_OS_TASKS   8
	    #define BT_ADAPTER_ADD_SEMAPHORES  9
	#endif
	#else
	  #define BT_ADAPTER_ADD_ENTITIES   6
	  #define BT_ADAPTER_ADD_OS_TASKS   6
	  #define BT_ADAPTER_ADD_SEMAPHORES  2
	#endif
#else
#define BT_ADAPTER_ADD_ENTITIES   3
#define BT_ADAPTER_ADD_OS_TASKS   3
#define BT_ADAPTER_ADD_SEMAPHORES 0
#endif



/*==== CONSTANTS ============================================================*/

#define MAX_TIMER                  231
#define MAX_SIMULTANEOUS_TIMER     (50 + MAX_REMU_SIMULTANEOUS_TIMER)


#ifdef FF_BAT /* with GDD_DIO entity */
#if(PSP_STANDALONE==0)
#define MAX_ENTITIES                (39 + MAX_REMU_ENTITIES + BT_ADAPTER_ADD_ENTITIES)
#define MAX_OS_TASKS                (33 + MAX_REMU_OS_TASKS + BT_ADAPTER_ADD_OS_TASKS)
#else
#define MAX_ENTITIES                (2 + MAX_REMU_ENTITIES + 0)
#define MAX_OS_TASKS                (2 + MAX_REMU_OS_TASKS + 0)
#endif
#define MAX_SEMAPHORES              (14+ MAX_REMU_SEMAPHORES + BT_ADAPTER_ADD_SEMAPHORES)
#else
#if(PSP_STANDALONE==0)
#define MAX_ENTITIES                (38 + MAX_REMU_ENTITIES + BT_ADAPTER_ADD_ENTITIES)
#define MAX_OS_TASKS                (32 + MAX_REMU_OS_TASKS + BT_ADAPTER_ADD_OS_TASKS)
#else
#define MAX_ENTITIES                (2 + MAX_REMU_ENTITIES + 0)
#define MAX_OS_TASKS                (2 + MAX_REMU_OS_TASKS + 0)
#endif
#define MAX_SEMAPHORES              (13 + MAX_REMU_SEMAPHORES + BT_ADAPTER_ADD_SEMAPHORES)
#endif /* FF_BAT */

#define MAX_OSISRS                   0
#define MAX_COMMUNICATIONS           MAX_OS_TASKS
#define MAX_POOL_GROUPS              7
#define MAX_MEMORY_POOLS             7

#endif /* GPRSCONST_H */



