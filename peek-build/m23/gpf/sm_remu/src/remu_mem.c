/*
+------------------------------------------------------------------------------
|  File:       remu_mem.c
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
|  Purpose :  This Modul defines the riviera emulator memory managment.
+-----------------------------------------------------------------------------
*/



#ifndef __REMU_MEM_C__
#define __REMU_MEM_C__
#endif

/*==== INCLUDES ===================================================*/

#include "../inc/remu_internal.h"


/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)
/* from remu point of view, 0 is always ext pool and 1 is int pool. This will make remu
     independent of lite or plus build */

#define EXT_MEM_POOL 0x0 
#define INT_MEM_POOL 0x1
/*==== TYPES ======================================================*/

/*==== VARIABLES ==================================================*/

#if RVF_ENABLE_STATS
extern UINT32 gsp_required_size;
extern UINT32 gsp_obtained_size;
extern UINT32 gsp_used_size;
extern UINT32 gsp_mem_in_use;
#endif

/*===============================================================*/

/*
+--------------------------------------------------------------------+
  NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET gsp_create_mb(T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM mb_param, T_RVF_MB_ID* mb_id)
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
T_RVF_RET  gsp_delete_mb(T_RVF_MB_NAME mb_name)
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
T_RVF_RET  gsp_get_mb_id(T_RVF_MB_NAME mb_name, T_RVF_MB_ID* mb_id)
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
T_RVF_MB_STATUS gsp_get_mb_status(T_RVF_MB_ID mb_id)
{
  T_RVF_MB_STATUS returnStatus = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnStatus;
}


/*
+--------------------------------------------------------------------+
  NYI
+--------------------------------------------------------------------+
*/
T_RVF_MB_STATUS gsp_get_mb_unused_mem(T_RVF_MB_ID mb_id)
{
  T_RVF_MB_STATUS returnStatus = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnStatus;
}

/*
+--------------------------------------------------------------------+
  NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_mb_is_used(T_RVF_MB_NAME mb_name, UINT8* isUsed)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}


/*
+--------------------------------------------------------------------+
 allocate a buffer
+--------------------------------------------------------------------+
*/
extern T_HANDLE BspGroupHandle;
extern T_HANDLE BspRvtGroupHandle;
extern T_HANDLE BspIntGroupHandle;
extern T_HANDLE ext_bsp_data_pool_handle;


T_RVF_MB_STATUS gsp_get_buf(T_RVF_MB_ID mb_id, UINT32 buffer_size, T_RVF_BUFFER** p_buffer)
{
  T_RVF_RET returnStatus = RV_MEMORY_ERR;
  T_HANDLE entityHandle;

  entityHandle = e_running[os_MyHandle()];

/*
if(*p_buffer != NULL)
  {
    vsi_o_ttrace(entityHandle, 1, "%s", "REMU: gsp_get_buf() with a dirty entityHandle");
  }
*/
   buffer_size += sizeof(T_PRIM_HEADER);
   if (mb_id==EXT_MEM_POOL)
	  *p_buffer = (T_RVF_BUFFER*) vsi_c_pnew_generic(entityHandle, buffer_size, 0,  (BspGroupHandle|VSI_MEM_NON_BLOCKING) FILE_LINE_MACRO);
   else if(mb_id==INT_MEM_POOL)
  	*p_buffer = (T_RVF_BUFFER*) vsi_c_pnew_generic(entityHandle, buffer_size, 0,  (BspIntGroupHandle|VSI_MEM_NON_BLOCKING) FILE_LINE_MACRO);
   else
   	return RVF_RED;
   if(*p_buffer != NULL)
  {
    returnStatus = RVF_OK;
//    *p_buffer =(T_RVF_BUFFER*) P2D((T_VOID_STRUCT*)*p_buffer); /* lift *p_buffer to riviera level */

#if RVF_ENABLE_STATS
    gsp_required_size += buffer_size;
    /*  Rriviera watermark feature and according buffer managment will be implemented in REMU Stage II.
      It's unclear by now how following sizes will be calculated. */
    gsp_obtained_size += rvf_get_buf_size((UINT8*) *p_buffer);
    gsp_used_size += rvf_get_buf_size((UINT8*) *p_buffer);
    gsp_mem_in_use += rvf_get_buf_size((UINT8*) *p_buffer);
#endif
  return RVF_GREEN;
  }
  return RVF_RED;
}

#if 0
T_RVF_MB_STATUS gsp_get_buf(T_RVF_MB_ID mb_id, UINT32 buffer_size, T_RVF_BUFFER** p_buffer)
{
T_VOID_STRUCT *prim;

   buffer_size += sizeof(T_PRIM_HEADER);

  if ( os_AllocateMemory ( os_MyHandle(), (T_VOID_STRUCT**)p_buffer,
                           buffer_size, OS_NO_SUSPEND, ext_bsp_data_pool_handle) !=OS_OK)
    return RVF_RED;

    P_OPC(*p_buffer)   = 0;
    P_LEN(*p_buffer)   = buffer_size;
    P_SDU(*p_buffer)   = NULL;    
    P_CNT(*p_buffer)   = 1;
    P_SHO(*p_buffer)   = 0;
    P_DPHO(*p_buffer)  = 0;
   *p_buffer =(T_RVF_BUFFER*) P2D((T_VOID_STRUCT*)*p_buffer); /* lift *p_buffer to riviera level */
  
  return RVF_GREEN;

}
#endif
/*
+--------------------------------------------------------------------+
  NYI
+--------------------------------------------------------------------+
*/
T_RVF_MB_STATUS gsp_count_buf(T_RVF_MB_ID mb_id, T_RVF_BUFFER*  p_buffer)
{
  T_RVF_MB_STATUS returnStatus = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnStatus;
}

/*
+--------------------------------------------------------------------+
 free a buffer
+--------------------------------------------------------------------+
*/


T_RVF_RET gsp_free_buf( T_RVF_BUFFER*  p_buffer)
{
  T_RVF_RET returnStatus = RVF_MEMORY_ERR;
  T_HANDLE entityHandle;
  int result;

#if RVF_ENABLE_STATS
    gsp_mem_in_use -= rvf_get_buf_size(p_buffer);
#endif

  entityHandle = e_running[os_MyHandle()];
  p_buffer =(T_RVF_BUFFER*) D2P((T_VOID_STRUCT*) p_buffer); /* squeeze *p_buffer to FRAME level */
  result = vsi_c_free(entityHandle,(T_VOID_STRUCT **)&p_buffer  FILE_LINE_MACRO);

  if(result == VSI_OK)
  {
    /* memory banks currently not implemented.
    rvf_disable(20);
    mb->cur_memory_used -= buf_size;
    rvf_enable(void)();
    */

    /* REMU is also lacking by now: call of related callback function if a RVF_RED bank went RVF_GREEN*/

    returnStatus = RV_OK;
  }
  return returnStatus;
}

#if 0
T_RVF_RET gsp_free_buf( T_RVF_BUFFER*  p_buffer)
{
p_buffer =(T_RVF_BUFFER*) D2P((T_VOID_STRUCT*) p_buffer); /* squeeze *p_buffer to FRAME level */
if (os_DeallocateMemory(os_MyHandle(), p_buffer )==OS_OK)
	return RVF_OK;
else
	return RVF_INTERNAL_ERR;
}
#endif

T_RVF_RET gsp_set_callback_func(T_RVF_MB_ID mb_id, MB_CALLBACK_FUNC func)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}

T_RVF_RET gsp_change_callback_func(T_RVF_MB_ID mb_id, MB_CALLBACK_FUNC func)
{
  T_RVF_RET returnCode = RVF_OK;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnCode;
}

void gsp_get_protected_buf(T_RVF_MB_ID mb_id, UINT32 buffer_size, T_RVF_BUFFER** p_buffer)
{
  assert(SORRY_NOT_YET_IMPLEMENTED);
}

 /*
+--------------------------------------------------------------------+
return a size of a buffer. Implemented as size of a primitive, may be not sufficient.
+--------------------------------------------------------------------+
*/
UINT32   gsp_get_buf_size(T_RVF_BUFFER* bptr)
{
  return P_LEN(D2P((T_VOID_STRUCT *)bptr));
}

    /* get the parameters of a specific memory bank */
T_RVF_RET  gsp_get_mb_param(T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM*  param)
{
  T_RVF_RET returnSuccess = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnSuccess;
}

/* change the parameters of a specific memory bank */
/*
+--------------------------------------------------------------------+
NYI
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_set_mb_param(T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM*  param)
{
  T_RVF_RET returnSuccess = 0;
  assert(SORRY_NOT_YET_IMPLEMENTED);
  return returnSuccess;
}


/*
+--------------------------------------------------------------------+
 allocate a message buffer(primitive)
+--------------------------------------------------------------------+
*/
T_RVF_MB_STATUS gsp_get_msg_buf(T_RVF_MB_ID mb_id,
           UINT32 message_size,
           T_RVF_MSG_ID  msg_id,
           T_RVF_MSG** pp_msg)
{
  T_RVF_MB_STATUS mb_status = rvf_get_buf(mb_id,message_size,(T_RVF_BUFFER**) pp_msg);

  if(mb_status != RVF_RED)
  {
    /* this is how riviera stores an OPC-like value */
   (*pp_msg)->msg_id = msg_id;
    /* FRAME-stile: cause  rvf_get_buf() doesn't know an OPC, it couldn't pass one to vsi_c_new(), we have to treat it now */
 /*   P_OPC((T_VOID_STRUCT*)(D2P((T_VOID_STRUCT*)*pp_msg))) = msg_id; */
  }
  return mb_status;
}

/*
+--------------------------------------------------------------------+
 free a message buffer(primitive)
+--------------------------------------------------------------------+
*/
T_RVF_RET gsp_free_msg  (T_RVF_MSG*  p_msg)
{
  return gsp_free_buf((T_RVF_BUFFER*)  p_msg);
}

/*
+--------------------------------------------------------------------+
 free a timer message buffer(which may afford a special timer handling later on?)
+--------------------------------------------------------------------+
*/
T_RVF_RET gsp_free_timer_msg (T_RVF_MSG* p_msg)
{
  return gsp_free_buf((T_RVF_BUFFER*)  p_msg);
}


