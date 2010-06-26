/*
+------------------------------------------------------------------------------
|  File:       remu_sync.h
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG
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
|  Purpose :  Definitions for the riviera emulator synchronisation.
+-----------------------------------------------------------------------------
*/

#ifndef REMU_SYNC_H
#define REMU_SYNC_H

/*==== INCLUDES =============================================================*/

/*==== CONSTANTS ============================================================*/

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/


/* task synchronization */
T_RVF_RET  gsp_initialize_mutex( T_RVF_MUTEX*  mutex);
//OMAPS72906 add static mutex
T_RVF_RET  gsp_initialize_static_mutex( T_RVF_MUTEX*  mutex);



T_RVF_RET  gsp_lock_mutex( T_RVF_MUTEX*  mutex);

T_RVF_RET  gsp_unlock_mutex( T_RVF_MUTEX*  mutex);

T_RVF_RET  gsp_delete_mutex( T_RVF_MUTEX*  mutex);


/*==== MACROS ===============================================================*/


#endif /* remu_sync_H */
