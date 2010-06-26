/*
+------------------------------------------------------------------------------
|  File:       remu_task.c
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
|  Purpose :  This Modul defines the riviera emulator task managment.
+-----------------------------------------------------------------------------
*/



#ifndef __REMU_TASK_C__
#define __REMU_TASK_C__
#endif

/*==== INCLUDES ===================================================*/
#include "remu.h"
#include "../inc/remu_internal.h"
#include "../inc/remu_task.h"
#include "rvf_i.h"
#include "pei.h"

/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)
#define PLEASE_USE_gsp_create_pei_task ("blah" == 0)

#define unusedParam 0



/*==== TYPES ======================================================*/

/*==== VARIABLES ==================================================*/

// T_RVF_MB_ID rvm_sys_mem_bank=RVF_INVALID_MB_ID;

extern T_HANDLE CondatMemPoolHandle;
//T_GSP_RT_ADDR_ID_DATA _gsp_rt_addr_id_data;

extern T_GSP_RT_ADDR_ID_DATA*  GSPTaskIdTable[]; /* this defn has been moved to gprscomp.c */
T_HANDLE BspGroupHandle;
T_HANDLE BspRvtGroupHandle;
T_HANDLE BspIntGroupHandle;

static UINT8  task_counter = 0;
/*===============================================================*/

/* Task management*/
/*
+--------------------------------------------------------------------+
  NYI
+--------------------------------------------------------------------+
*/
void   gsp_init(void)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

/*
+--------------------------------------------------------------------+
can't be implemented without changing the interface cause we can't create an entity
without specyfying a pei_create()!
+--------------------------------------------------------------------+
*/
#ifdef JUNK_CODE_INCLUDED
T_RVF_RET  gsp_original_create_legacy_task(TASKPTR task_entry, UINT8 task_id, char* taskname, UINT8* stack, UINT16 stacksize, UINT8 priority, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend)
{
  T_RVF_RET returnCode = RVF_NOT_SUPPORTED;

  /* we can't resemble this interface because we need an additional
       "SHORT (*pei_create)(T_PEI_INFO const **" parameter triggering the pei_create function!
       Please use gsp_create_pei_task() instead, with the address of pei_create as new parameter 4 */

  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}
#endif

/*
+--------------------------------------------------------------------+
can't be implemented without changing the interface cause we can't create an entity
without specyfying a pei_create()!
+--------------------------------------------------------------------+
*/
#ifdef JUNK_CODE_INCLUDED
T_RVF_RET  gsp_rvf_original_create_task(TASKPTR task_entry, T_RVF_G_ADDR_ID task_id,
                                                             char* taskname,  UINT8* stack, UINT16 stacksize,
                                                             UINT8 priority, UINT8 tcode, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend)
{
  T_RVF_RET returnCode = RVF_NOT_SUPPORTED;

  /* we can't resemble this interface because we need an additional
       "SHORT (*pei_create)(T_PEI_INFO const **" parameter triggering the pei_create function!
       Please use gsp_create_pei_task() instead, with the address of pei_create as new parameter 4 */

  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}
#endif

/*
+--------------------------------------------------------------------+
constitute an entry for given process in global REMU task ID table
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_init_remu_task(T_RVF_G_ADDR_ID task_id, char* taskname)
{
  OS_HANDLE timerHandle;
  /* T_HANDLE entityHandle; */
  T_RVF_RET returnCode = RVF_OK;

  /*
  if (MaxEntities > MAX_ENTITIES)
  {
    entityHandle = e_running[os_MyHandle()];
    os_SystemError (entityHandle, OS_SYST_ERR, "MAX_ENTITIES in REMU defined to small");
  }*/
  
  if (task_counter >= MaxEntities)
  {
    return RVF_INTERNAL_ERR;
  }

  if (!GSPTaskIdTable[task_id])
  {
      if (gsp_get_buf(EXT_MEM_POOL, sizeof(T_RVF_RT_ADDR_ID_DATA),  (T_RVF_BUFFER**)&GSPTaskIdTable[task_id]) )
      {
        return RVF_INTERNAL_ERR;
      }
      memset(GSPTaskIdTable[task_id], 0, sizeof(T_GSP_RT_ADDR_ID_DATA));
  }

  GSPTaskIdTable[task_id]->timersInPossession= 0;
  for (timerHandle = 0; timerHandle < RVF_NUM_TASK_TIMERS; timerHandle++)
  {
    GSPTaskIdTable[task_id]->vsiTimerHandle[timerHandle] = 0;
  }

  GSPTaskIdTable[task_id]->host_addr_id = task_id;
  GSPTaskIdTable[task_id]->symbolic_name = taskname;

  GSPTaskIdTable[task_id]->Queue[0] = pf_TaskTable[task_id].QueueHandle;

  /* Create one Event Group for this task */
  if (os_CreateEventGroup(taskname,&(GSPTaskIdTable[task_id]->pOSEvtGrp)) != NU_SUCCESS)
   {
    return RVF_INTERNAL_ERR;
  }
/*
if (( GSPTaskIdTable[task_id]->com_handle= vsi_c_open (task_id, taskname)) < VSI_OK)
          return PEI_ERROR;
*/
   task_counter++;

   return returnCode;
}

/*
+--------------------------------------------------------------------+
constitute an entry for given, just created process in global REMU task ID table
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_init_remu_dyn_task(T_RVF_G_ADDR_ID task_id, char* taskname, T_HANDLE qhandle, UINT8 priority)
{
  T_RVF_RET returnCode = RVF_OK;


  returnCode = gsp_init_remu_task(task_id, taskname);
  
  if (returnCode == RVF_OK)
  {
    GSPTaskIdTable[task_id]->priority = priority;

    if (qhandle == 0)
    {
      assert(pf_TaskTable[task_id].QueueEntries > 0);
      if (os_CreateQueue (task_id, 0, taskname, pf_TaskTable[task_id].QueueEntries, &GSPTaskIdTable[task_id]->Queue[0], pf_TaskTable[task_id].MemPoolHandle) != OS_OK)
      {
        return RVF_INTERNAL_ERR;
      }
    }
    else
    {
      GSPTaskIdTable[task_id]->Queue[0] = pf_TaskTable[task_id].QueueHandle;
    }
  }

   return returnCode;
}
/*
+--------------------------------------------------------------------+
create a process and trigger init of GSPTaskIdTable
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_create_pei_task(TASKPTR task_entry, T_RVF_G_ADDR_ID task_id, char* taskname,
                                                 SHORT (*pei_create)(T_PEI_INFO const **), UINT8* stack, UINT16 stacksize,
                                                 UINT8 priority, UINT8 tcode, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend)
{
  T_HANDLE entityHandle;

  entityHandle = e_running[os_MyHandle()];

  /* pool_handle set 1 for dynamic pool, to be changed when newer frame supports 2 different dyn pools*/
  if (vsi_p_create (entityHandle,  pei_create,  (void (*)(T_HANDLE, ULONG))task_entry, ext_data_pool_handle) == VSI_ERROR)
  {
    return RVF_INTERNAL_ERR;
  }

  return gsp_init_remu_dyn_task(task_id,  taskname,  newQueue, priority);
}

/*
+--------------------------------------------------------------------+
create a riv class 4 entity process and trigger init of GSPTaskIdTable
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_create_pei_legacy_task(TASKPTR task_entry, UINT8 task_id, char* taskname, SHORT (*pei_create)(T_PEI_INFO const **),  UINT8* stack, UINT16 stacksize, UINT8 priority, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend)
{
  return gsp_create_pei_task(task_entry, task_id, taskname, pei_create, stack, stacksize, priority, ET4_TASK, time_slicing, is_suspend);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_resume_task( T_RVF_G_ADDR_ID taskid)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void   gsp_exit_task(T_RVF_G_ADDR_ID vtask)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_suspend_task(T_RVF_G_ADDR_ID vtask)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}

/*
+--------------------------------------------------------------------+
deliver the task handle of the caller
+--------------------------------------------------------------------+
*/
T_RVF_G_ADDR_ID gsp_get_taskid(void)
{
  T_HANDLE entityHandle;

  entityHandle = e_running[os_MyHandle()];
  return(T_RVF_G_ADDR_ID) entityHandle;
}

/*
+--------------------------------------------------------------------+
return the name of current task initialized by gsp_init_remu_task() or
created with GSPTaskIdTable()
+--------------------------------------------------------------------+
*/
char*    gsp_get_taskname(void)
{
  T_RVF_G_ADDR_ID task_id = rvf_get_taskid();
  return GSPTaskIdTable[task_id]->symbolic_name;
}

/*
+--------------------------------------------------------------------+
Check if current entity is a class 2 entity or not. If so, return ID, group otherwise.
Cause group is a global var set by riviera manager, it can't be resembled here
and return value is always the ID
+--------------------------------------------------------------------+
*/
T_RVF_G_ADDR_ID gsp_get_context()
{
  T_RVF_G_ADDR_ID task_id=rvf_get_taskid();

  /* Riviera: _RDV is set to a global group ID (gid) by rvm_t2_proxy() in rvm_task.c,
      calling rvf_setRDV(p_msg->dest_addr_id); */

  /*
  if (GSPTaskIdTable[gid]->type_code==ET2_HOST_TASK) {
    return _RDV ;
  } */

  return GSPTaskIdTable[task_id]->host_addr_id;
}

