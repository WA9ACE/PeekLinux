/*
+------------------------------------------------------------------------------
|  File:       remu_trace.c
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
|  Purpose :  This Modul defines the riviera emulator trace managment.
+-----------------------------------------------------------------------------
*/


#ifndef __REMU_TRACE_C__
#define __REMU_TRACE_C__
#endif

#ifdef DEBUG_ENABLE /* define from RIVIERA to enable tracing */
/*==== INCLUDES ===================================================*/

#include "rvf_api.h"
#include "../inc/remu_internal.h"
#include "rvf_i.h"
#include "rvm_use_id_list.h"

#include "os_glob.h"
#include <stdio.h>      /* for sprintf */
#include "rvf_mem_i.h"

/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)

/* for the meaning of the following two definitions see TI-Nice memo "\g23m\ssa_release\doc\riviera\RIVTM031(Tech Memo Trace Filtering).doc" */

extern T_OS_TASK_TABLE_ENTRY TaskTable[];
/* variable for statistics */
#if RVF_ENABLE_STATS
UINT32 gsp_required_size = 0;
UINT32 gsp_obtained_size = 0;
UINT32 gsp_used_size  = 0;
UINT32 gsp_mem_in_use = 0;
#endif
#define ERR_GSP_DUMP_MEM_ERR "rvf_dump_mem not implemented"
#define ERR_GSP_DUMP_MEMPOOL_ERR  "rvf_dump_pool not implemented"

#if 0
#define TRACE_TRACE_LEVEL_FILTER 5
#define TRACE_LAYER_DBG  0xFFFFFFFF
#define RVF_INVALID_INDEX  0xFFFF


/*==== VARIABLES ==================================================*/

UINT8 gsp_trace_level = TRACE_TRACE_LEVEL_FILTER;
UINT32 gsp_layer_mask = TRACE_LAYER_DBG;

extern T_OS_TASK_TABLE_ENTRY TaskTable[];


/******** MEMORY POOLS ******************/
/* Define the buffer pools */
//extern T_RVF_POOL  _rvf_pools[];

/*********** MEMORY BANKS ***********/
/* array of memory bank    */
static T_RVF_MB   rvf_banks[RVF_MAX_REAL_MB];

/* array of waiting buffers */
static T_RVF_BUFFER*  waiting_buffers[RVF_MAX_WAITING_BUF];
static UINT16   next_buffer[RVF_MAX_WAITING_BUF];
static UINT16   first_free_element;

/* array of memory bank name and id*/
static T_RVF_MB_NAME_ID rvf_name_id[RVF_MAX_TOTAL_MB] = RVF_MB_MAPPING;



/*==== TYPES ======================================================*/


/*===============================================================*/

/*
+--------------------------------------------------------------------+
 set the favorized Riviera trace level
+--------------------------------------------------------------------+
*/
void gsp_set_trace_level(T_RVT_BUFFER p_msg, UINT16 msg_length)
{

  /* Checking for an invalid PDU. */
  if((p_msg == NULL) ||(msg_length !=(sizeof(UINT8) + sizeof(UINT32))))
  {
    return;
  }

 /* Update the level of filtering. */
  if(*p_msg <= RV_TRACE_LEVEL_DEBUG_LOW)
  {
    gsp_trace_level = *p_msg;
  }

  /* Update the 32-bit mask related to the software entities to be monitored.
      Note that the 32-bit mask is transmitted LSB first. */
  gsp_layer_mask = *(++p_msg);
  gsp_layer_mask |= *(++p_msg) << 8;
  gsp_layer_mask |= *(++p_msg) << 16;
  gsp_layer_mask |= *(++p_msg) << 24;
}


/*
+--------------------------------------------------------------------+
 send a trace to the real world
+--------------------------------------------------------------------+
*/
void gsp_send_trace1(char*  msg, UINT8 msg_length, UINT32 val, UINT8 trace_level, UINT32 trace_type)
{
  T_HANDLE entityHandle;

  if((trace_level < RV_TRACE_LEVEL_WARNING) ||((trace_level <= gsp_trace_level) &&( !((trace_type & 0xFFFF0000) &(gsp_layer_mask & 0xFFFF0000)) ||((trace_type & 0x0000FFFF) &(gsp_layer_mask & 0x0000FFFF)))))
  {
    entityHandle = e_running[os_MyHandle()];
    if(val != NULL_PARAM)
    {
      vsi_o_ttrace(entityHandle, 1, "%s %x", msg, val);
    }
    else
    {
      vsi_o_ttrace(entityHandle, 1, "%s", msg);
    }
  }
}
#endif
/*
+--------------------------------------------------------------------+
memory dump
+--------------------------------------------------------------------+
*/
void gsp_dump_mem()
{
rvf_send_trace(ERR_GSP_DUMP_MEM_ERR,strlen(ERR_GSP_DUMP_MEM_ERR),NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
}
#if 0
#if RVF_ENABLE_STATS /* conditional compilation if stats are enabled */
  char mb_info[100];
  UINT16 num_mb, num_buf, index;
  UINT32 total_mem_size = 0;
  UINT32 total_max_used = 0;
  UINT32 total_cur_used = 0;
  T_RVF_MB*  mb;


  /* display memory required, obtained and ratio */
  rvf_send_trace("MEM STAT: Total memory required", 31, gsp_required_size, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
  rvf_send_trace("MEM STAT: Total memory obtained", 31, gsp_obtained_size, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
  rvf_send_trace("MEM STAT: Total memory used    ", 31, gsp_used_size, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
  sprintf( mb_info,"MEM STAT: Mem usage ratio      : %010f%%", ((double)(gsp_required_size) /(double)(gsp_obtained_size))* 100);
  rvf_send_trace(mb_info, 44, NULL_PARAM , RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);

  rvf_send_trace("*** START DUMPING MEMORY BANK ***", 33, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
  /* for each mb, display its name, its id, its cur_size, its watermark, its limit, its max reached, the number of callback functions enqueued,
 (the requested size, the number of allocated buffer), the average buffer size for this mb */
  rvf_send_trace("**MB_NAME* Id Used_mem Watermark  Limit   Peak Nb_buff Avg_buf_size", 67, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);

  for( num_mb = 0; num_mb < RVF_MAX_TOTAL_MB; num_mb++)
  {
    /* trace the mb if it has been created*/
    if( rvf_name_id[num_mb].mb_params.size > 0)
    {
      mb = &rvf_banks[ rvf_name_id[num_mb].mb_id ];
      num_buf = 0;
      index = mb->first_buffer_index;
      while( index != RVF_INVALID_INDEX)
      { num_buf++;
        index = next_buffer[index];
      }
      sprintf( mb_info, "%10.10s %2d   %6d    %6d %6d %6d      %2d       %6d",
      rvf_name_id[num_mb].mb_name, rvf_name_id[num_mb].mb_id,
      mb->cur_memory_used, mb->watermark, mb->max, mb->max_reached, num_buf,
      mb->num_buf == 0? 0:mb->required_size / mb->num_buf);

      rvf_send_trace( mb_info, 67, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);

      total_mem_size += mb->max;
      total_max_used += mb->max_reached;
      total_cur_used += mb->cur_memory_used;
    }
  }
  sprintf( mb_info, "TOTAL: ******** %6d********** %6d %6d", total_cur_used, total_mem_size, total_max_used);

  rvf_send_trace( mb_info, 46, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);

#endif /* RVF_ENABLE_STATS */
}

#endif

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void gsp_dump_tasks()
{
  UINT8 num_task;
  char task_info[100];

  rvf_send_trace("*** START DUMPING TASKS ***", 27, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);

  /* for each task, display its name, its id, its stack size*/
  rvf_send_trace("*TASK_NAME Id Stack_size Used_stack", 35, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);

  for( num_task = 0; num_task < MaxTasks; num_task++)
  {
    /* trace the task if it has been created*/
    if(pf_TaskTable[num_task].PeiTable != 0) {
      sprintf( task_info, "%10.10s %2d      %5d      %5d",
        pf_TaskTable[num_task].Name,
        num_task,
        TaskTable[num_task].TaskCB.TCB.tc_stack_size,
       (unsigned char*) TaskTable[num_task].TaskCB.TCB.tc_stack_pointer -(unsigned char*) TaskTable[num_task].TaskCB.TCB.tc_stack_end);
   rvf_send_trace( task_info, 35, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
  }
 }
}

/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
void gsp_dump_pool()
{
rvf_send_trace(ERR_GSP_DUMP_MEMPOOL_ERR,strlen(ERR_GSP_DUMP_MEMPOOL_ERR),NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, RVM_USE_ID);
}



T_RVM_RETURN	gsp_error ( T_RVM_NAME swe_name, T_RVM_RETURN error_cause, 
							T_RVM_ERROR_TYPE error_type, T_RVM_STRING error_msg)
{	
	char swe_name_string[40];
	UINT8 i=0;

	memcpy(swe_name_string,"RVM: coming from: ",18);
	memcpy((void*)((char*)swe_name_string+18),swe_name,RVM_NAME_MAX_LEN);

	RVM_TRACE_WARNING("RVM: unrecoverable error indication");
	rvf_send_trace(swe_name_string, 18 + RVM_NAME_MAX_LEN, NULL_PARAM, RV_TRACE_LEVEL_WARNING, RVM_USE_ID );
	RVM_TRACE_WARNING_PARAM("RVM: Error Cause: ", error_cause);
	RVM_TRACE_WARNING_PARAM("RVM: Error Type: ", error_type);
	/* check the message to limit its length to RVM_ERROR_MSG_MAX_LENGTH characters. */
	while (error_msg[i])
	{
		i++;
		if (i>RVM_ERROR_MSG_MAX_LENGTH)
			break;
	}

	rvf_send_trace(error_msg, i, NULL_PARAM, RV_TRACE_LEVEL_WARNING, RVM_USE_ID );

	return RVM_OK;
}

#endif /* DEBUG_ENABLE */


