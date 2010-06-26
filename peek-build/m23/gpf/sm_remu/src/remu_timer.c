/*
+------------------------------------------------------------------------------
|  File:       remu_timer.c
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
|  Purpose :  This Modul defines the riviera emulator timer managment.
+-----------------------------------------------------------------------------
*/

#ifndef __REMU_TIMER_C__
#define __REMU_TIMER_C__
#endif

/*==== INCLUDES ===================================================*/

#include "../inc/remu_internal.h"
#include "rvf_i.h"
#include "frame.h"
/* ugly as hell: dependencies from riviera manager api. But that's the way it is in riviera...
    The only way to learn what a BUILD_MESSAGE_OFFSET is, on which RVM_TMS_MSG relies, and
    it's parameter RVM_USE_ID */
#include "rvm_use_id_list.h"


/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)
extern unsigned int os_tick_to_time_multiplier;
#define GSP_IRQ_DISABLED_MASK 0x00000080

/*==== VARIABLES ==================================================*/

static INT16   OSDisableNesting = 0;
static BOOL  OSInterruptAlreadyMasked = FALSE;
static INT32  OSLastIntLevel;

/*==== TYPES ======================================================*/

/*==== PROTOTYPES ==================================================*/

void gsp_timer_notify(T_HANDLE TaskHandle, T_HANDLE EntityHandle, USHORT TimerIndex);

/*===============================================================*/


/* Disable Interrupts, Enable Interrupts*/

#ifdef _WIN32_

/*******************************************************************************
**
** Function         GSP_INT_Check_IRQ_Mask()
**
** Description      This function checks if the IRQ are disabled(outside RVF).
**
** Returns          IRQ mask
**
*******************************************************************************/
UINT32 GSP_INT_Check_IRQ_Mask(void)
{
  return 0;
}
#else

/*-------------------------------------------------------*/
/* GSP_INT_Check_IRQ_Mask()                                  */
/*-------------------------------------------------------*/
/*                                                       */
/* Description: check in the CPSR register if the IRQ    */
/*              are masked out or not.                   */
/* ------------                                          */
/*                                                       */
/*-------------------------------------------------------*/
/* Declaration of ASM GSP_INT_Check_IRQ_Mask function */
UINT32 GSP_INT_Check_IRQ_Mask(void);

asm("           .def  $GSP_INT_Check_IRQ_Mask");
asm("$GSP_INT_Check_IRQ_Mask  ");
asm("           .ref _GSP_INT_32_Check_IRQ_Mask");
asm(".state16");
asm("  ADR  r0,_GSP_INT_32_Check_IRQ_Mask  ");
asm("  BX r0  ");

asm("  .align");
asm("  .state32");
asm("           .def _GSP_INT_32_Check_IRQ_Mask");
asm("_GSP_INT_32_Check_IRQ_Mask  ");

asm("  MRS  r0,CPSR  ");   // pick up CPSR
asm("  BX  lr  ");        // return to caller

#endif


/*
+--------------------------------------------------------------------+
Enable interrupts
+--------------------------------------------------------------------+
*/
void   gsp_enable(void)
{
  if( --OSDisableNesting == 0)  /* Control nesting interrupt */
  {
    if( OSInterruptAlreadyMasked == TRUE) /*  check if interrupts have been disabled outside RVF,
                          in that case, do not enable interrupts */
    {  OSInterruptAlreadyMasked = FALSE;
    }
    else
    {  NU_Control_Interrupts(OSLastIntLevel);
    }
  }
}

/*
+--------------------------------------------------------------------+
disable interrupts
+--------------------------------------------------------------------+
*/
void   gsp_disable()
{

  /* Control interrupt nesting ourselves */
  if(OSDisableNesting == 0)
  {
    if( GSP_INT_Check_IRQ_Mask() & GSP_IRQ_DISABLED_MASK)  /* if IRQ are disabled(outside RVF) */
    {  OSInterruptAlreadyMasked = TRUE;
    }
    else
    {  OSLastIntLevel = NU_Control_Interrupts(NU_DISABLE_INTERRUPTS);
    }
  }
  OSDisableNesting++;
}

/* Timer management*/

/*
+--------------------------------------------------------------------+
create a riviera mesage driven timer. Message driven timers are constituting a message
based notification system. See Riviera Frame Overview document.
In REMU, this will NOT create a new NUCLEUS timer. The FRAME/Stack will use only
one nucleus timer, which is virtualized to N vsi timers.

Following this, it makes now sense to return the nucleus timer's address, as done
by riviera. We return the timer number [1..4] instead. This behavior is known to the
other REMU functions dealing with such a T_RVF_TIMER_ID.
+--------------------------------------------------------------------+
*/
T_RVF_TIMER_ID  gsp_create_timer(T_RVF_G_ADDR_ID task_id, UINT32  timerDuration, BOOLEAN isContinuous, SHORT (*timeout_action) (OS_HANDLE,OS_HANDLE,USHORT))
{
  UINT8 newTimer;
  OS_HANDLE timerHandle = 0;

  for (newTimer = 0; newTimer < RVF_NUM_TASK_TIMERS; newTimer++)
  {
    if (GSPTaskIdTable[task_id]->vsiTimerHandle[newTimer] == 0)
    {
      break;
    }
  }

  if ((newTimer >= 0) && (newTimer < RVF_NUM_TASK_TIMERS) && (newTimer <= pf_TaskTable[task_id].NumOfTimers))
  {
    /* WARNING: p_dead_action isn't used by os_CreateTimer() in any way (dead historical param).
        But this resembles the way rivera handles this in internal func _invoke_tm(). */
    if (os_CreateTimer(task_id, (void(*)(OS_HANDLE,OS_HANDLE,USHORT))timeout_action, &timerHandle, pf_TaskTable[task_id].MemPoolHandle) == OS_ERROR)
    {
      vsi_o_assert( task_id, OS_SYST_ERR_SIMUL_TIMER, __FILE__, __LINE__,
                      "Number of started timers > MAX_SIMULTANEOUS_TIMER" );
    }
    *(pf_TaskTable[task_id].FirstTimerEntry + newTimer) = timerHandle;

    GSPTaskIdTable[task_id]->vsiTimerHandle[newTimer] = timerHandle;
    GSPTaskIdTable[task_id]->timersInPossession++;
    if (timerDuration)
    {
//      newTimer++; /* lift it up to the riviera timer numbering scheme [1..4] */
      gsp_start_timer(newTimer, timerDuration, isContinuous);
    }
    return timerHandle;
  }
  else
  {
    return 0;
  }
}

/*
+--------------------------------------------------------------------+
distroy a timer
NYI
+--------------------------------------------------------------------+
*/
void  gsp_del_timer  (T_RV_TM_ID tm_id){
  T_HANDLE entityHandle;
  UINT         task_id;
  UINT8       rivTimer;
  BOOL        found = FALSE;

  /* search for timer ID handle */
  for (task_id = 0; task_id < MaxEntities; task_id++)
  {
    if (GSPTaskIdTable[task_id])
    {
      for (rivTimer = 0; rivTimer < RVF_NUM_TASK_TIMERS; rivTimer++)
      {
        if (found = (GSPTaskIdTable[task_id]->vsiTimerHandle[rivTimer] == tm_id))
        {
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
  }
  if (found)
  {
//    rivTimer++; /* lift it up to the riviera timer numbering scheme [1..4] */
    gsp_stop_timer(rivTimer);
    entityHandle = e_running[os_MyHandle()];
    os_DestroyTimer(entityHandle, tm_id);
  }
}

/*
+--------------------------------------------------------------------+
reset a timer
+--------------------------------------------------------------------+
*/
void  gsp_reset_timer(T_RV_TM_ID tm_id, UINT32 new_duration, BOOLEAN isContinuous)
{
  UINT task_id;
  UINT8 rivTimer;
  BOOL found = FALSE;

  /* search for timer ID handle */
  for (task_id = 0; task_id < MaxEntities; task_id++)
  {
    if (GSPTaskIdTable[task_id])
    {
      for (rivTimer = 0; rivTimer < RVF_NUM_TASK_TIMERS; rivTimer++)
      {
        if (found = (GSPTaskIdTable[task_id]->vsiTimerHandle[rivTimer] == tm_id))
        {
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
  }
  if (found)
  {
//    rivTimer++; /* lift it up to the riviera timer numbering scheme [1..4] */
    gsp_stop_timer(rivTimer);
    gsp_start_timer(rivTimer, new_duration, isContinuous);
  }
}

/*
+--------------------------------------------------------------------+
sleep 'n' ticks
+--------------------------------------------------------------------+
*/
void   gsp_delay(UINT32 ticks)
{
  T_HANDLE entityHandle;

  entityHandle = e_running[os_MyHandle()];
  vsi_t_sleep(entityHandle, SYSTEM_TICKS_TO_TIME(ticks));
}

/*
+--------------------------------------------------------------------+
start an indexed timer
+--------------------------------------------------------------------+
*/
void   gsp_start_timer(UINT8 tnum, UINT32 ticks, BOOLEAN is_continuous)
{
  T_HANDLE task_id;

  if ((tnum >= 0) && (tnum < RVF_NUM_TASK_TIMERS))
  {
//    tnum--; /* scale down to gsp frame range [0..n] */
    task_id = e_running[os_MyHandle()];

  if ((!(pf_TaskTable[task_id].Flags & PASSIVE_BODY)) &&  (GSPTaskIdTable[task_id]->vsiTimerHandle[tnum] == 0)) /* active entity should create a timer before using it */
    {
      return; /* not a valid timer */
    }

    if (is_continuous == TRUE)
    {
      vsi_t_pstart(task_id, tnum, SYSTEM_TICKS_TO_TIME(ticks), SYSTEM_TICKS_TO_TIME(ticks));
    }
    else
    {
      vsi_t_start(task_id, tnum, SYSTEM_TICKS_TO_TIME(ticks));
    }
  }
}

/*
+--------------------------------------------------------------------+
stop an indexed timer
+--------------------------------------------------------------------+
*/
void   gsp_stop_timer_old(UINT8 tnum)
{
  T_HANDLE task_id;

  if ((tnum >= 0) && (tnum < RVF_NUM_TASK_TIMERS))
  {
//    tnum--; /* scale down to gsp frame range [0..n] */
    task_id = e_running[os_MyHandle()];
    if (GSPTaskIdTable[task_id]->vsiTimerHandle[tnum] != 0)
    {
      vsi_t_stop(task_id, tnum);
      //GSPTaskIdTable[entityHandle]->vsiTimerHandle[tnum] = 0;
    }
  }
}

void   gsp_stop_timer(UINT8 tnum)
{
  T_HANDLE task_id;

  if ((tnum >= 0) && (tnum < RVF_NUM_TASK_TIMERS))
  {
//    tnum--; /* scale down to gsp frame range [0..n] */
    task_id = e_running[os_MyHandle()];
    if (!(pf_TaskTable[task_id].Flags & PASSIVE_BODY)) /* if only active body */
    	{
    	if (GSPTaskIdTable[task_id]->vsiTimerHandle[tnum] != 0)
		    {
		    OS_HANDLE TimerHandle;
		    TimerHandle = (*(pf_TaskTable[task_id].FirstTimerEntry + tnum) & TIMER_HANDLE_MASK);
		    if ( TimerHandle && ( (TimerHandle & TIMER_HANDLE_MASK) < MaxTimer ) ) 
		    os_StopTimer ( task_id, TimerHandle );
    		}
    	}
	else /* passive entity */
		vsi_t_stop(task_id, tnum);
  }
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
char* gsp_get_time_stamp(char* tbuf)
{
  UINT32 ms_time;
  UINT32 s_time;
  UINT32 m_time;
  UINT32 h_time;
  char   *p_out = tbuf;

  ms_time = gsp_get_tick_count();
  s_time  = ms_time/100;   /* 100 Ticks per second */
  m_time  = s_time/60;
  h_time  = m_time/60;

  ms_time -= s_time*100;
  s_time  -= m_time*60;
  m_time  -= h_time*60;

  *p_out++ = (char)((h_time / 10) + '0');
  *p_out++ = (char)((h_time % 10) + '0');
  *p_out++ = ':';
  *p_out++ = (char)((m_time / 10) + '0');
  *p_out++ = (char)((m_time % 10) + '0');
  *p_out++ = ':';
  *p_out++ = (char)((s_time / 10) + '0');
  *p_out++ = (char)((s_time % 10) + '0');
  *p_out++ = ':';
  *p_out++ = (char)((ms_time / 10) + '0');
  *p_out++ = (char)((ms_time % 10) + '0');
  *p_out++ = ':';
  *p_out   = 0;

	return tbuf;
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
UINT32   gsp_get_tick_count(void)
{
   return NU_Retrieve_Clock();
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void   gsp_init_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void   gsp_init_timer_list_entry(T_RVF_TIMER_LIST_ENT * p_tle)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
UINT16   gsp_update_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq)
{
  UINT16 returnExpired = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnExpired;
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void   gsp_add_to_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq, T_RVF_TIMER_LIST_ENT * p_tle)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void   gsp_remove_from_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq, T_RVF_TIMER_LIST_ENT * p_tle)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_TIMER_LIST_ENT*  gsp_get_expired_entry(T_RVF_TIMER_LIST_Q* p_timer_listq)
{
  T_RVF_TIMER_LIST_ENT*  returnCode = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}


