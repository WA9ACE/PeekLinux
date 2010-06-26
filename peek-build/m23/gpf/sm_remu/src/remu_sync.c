/*
+------------------------------------------------------------------------------
|  File:       remu_sync.c
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
|  Purpose :  This Modul defines the riviera emulator sync managment.
+-----------------------------------------------------------------------------
*/



#ifndef __REMU_SYNC_C__
#define __REMU_SYNC_C__
#endif

/*==== INCLUDES ===================================================*/


#include "sm_defs.h"
#include "../inc/remu_internal.h"
#include "tools.h"
//OMAPSXXXXX
#include "../inc/remu_timer.h"

/*==== CONSTANTS ==================================================*/
#define SORRY_NOT_YET_IMPLEMENTED ("blah" == 0)
extern T_OS_SEM_TABLE_ENTRY SemTable[];
#define SEM_GEN_NAME_LENGHT 4

/*==== TYPES ======================================================*/





//OMAPS72906 major rework of semaphre creation. Due to the wrap around of the
// Sem names it is possible that in subsequent wrap rounds of the semaphore allocation
// the same name may be used as for an existing static/long living semaphore. This will cause
// issues as two tasks may be using same underlying semaphore and lock/unlock the semaphore randomly
// Solution is to create a new class of semaphores for static use in the drivers. This will ensure that
// there is no conflict between dynamic allocation for FFS etc and drivers own static ones




/*==== LOCALS =====================================================*/

unsigned int rivSemCount = 0;

//OMAPS69681 move semaphore names to caller stacks

//OMAPS72906
unsigned int rivStaticSemCount=0;

/*===============================================================*/


/*
+--------------------------------------------------------------------+
create a semaphore
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_initialize_mutex( T_RVF_MUTEX*  mutex)
{
  T_RVF_RET returnCode = RVF_OK;
  T_HANDLE entityHandle;
  SM_SCB*  mutexP =(SM_SCB*)mutex;

//OMAPS69681 add
// Put names on stack so that each task has its own copy so makes it re-entrant.
// Otherwise calls fail when this function is pre-empted by itself at a higher priority
// and they are both sharing the same string. Leads to failure as 2 sems are created with same name.
char rivSemGenName[5];
char rivSemBaseName[8] = "RV";
//OMAPS69681 end

  if (mutex==NULL)
  	return RVF_INVALID_PARAMETER;

//OMAPS69681
//Do not copy a 4 character number, just 3 to allow a char for task number.
//Name will be RVTXXX, where T is task handle and XXX is numerical value
//This protects against fact that rivSemCount is still global and shared. Theoretical possibility that
// first call could be prempted bt second in cycles updating rivSemCount value

  HexToASCII(rivSemCount, rivSemGenName, SEM_GEN_NAME_LENGHT);

  //Protect the update of rivSemCount
  gsp_disable();
  rivSemCount++;
  gsp_enable();


  memcpy(&rivSemBaseName[3], rivSemGenName, SEM_GEN_NAME_LENGHT);
//OMAPS69681 end

//OMAPS72906 add
  rivSemBaseName[2] = (char)os_MyHandle() + '0';


  entityHandle = vsi_s_open(e_running[os_MyHandle()], rivSemBaseName, 1);
//OMAPS72906 end

  if(entityHandle  != VSI_ERROR)
  {
    /* in the riviera world, *mutex argument designates a user-defined buffer for the
        semaphore object. GPF-FRAME allocates semaphore objects on it's own. So we do copy
        the initialized object to the user-defined buffer. When *mutex is passed to the
        other semaphore REMU functions, it is used to identify the name of the semaphore.
        With REMU, a user-stored semaphore object is never used for synchronisation!
        Furthermore, its magic NU_SEMAPHORE_ID sm_id identifier(ULONG 0x53454d41) will be
        destroyed and used as an index for quick-referencing the SemTable with the operators.
        DIRTY!!! */
    memcpy(mutex, &SemTable[entityHandle].SemCB, sizeof(T_RVF_MUTEX));
    mutexP->sm_id = entityHandle;
     returnCode = RVF_OK;
  }
  else
  {
       returnCode = RVF_INTERNAL_ERR;
  }
  return returnCode;
}


//OMAPS72906
/*
+--------------------------------------------------------------------+
create a static semaphore
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_initialize_static_mutex( T_RVF_MUTEX*  mutex)
{
  T_RVF_RET returnCode = RVF_OK;
  T_HANDLE entityHandle;
  SM_SCB*  mutexP =(SM_SCB*)mutex;

// Put names on stack so that each task has its own copy so makes it re-entrant.
// Otherwise calls fail when this function is pre-empted by itself at a higher priority
// and they are both sharing the same string. Leads to failure as 2 sems are created with same name.
char rivSemGenName[5];
char rivSemBaseName[8] = "RV";

  if (mutex==NULL)
  	return RVF_INVALID_PARAMETER;

//Do not copy a 5 character number, just 4 to allow a char for task number.
//Name will be RV/XXX, where / is marked for static sem and XXXX is numerical value
// This makes it a unique name that cannot be reused by normal dynamic semaphores.
  HexToASCII(rivStaticSemCount, rivSemGenName, SEM_GEN_NAME_LENGHT);
   //Protect the update of rivSemCount
  gsp_disable();
  rivStaticSemCount++;
  gsp_enable();
  memcpy(&rivSemBaseName[3], rivSemGenName, SEM_GEN_NAME_LENGHT);


  // As a static semaphore create name RV/XXXX
  rivSemBaseName[2] = (char)'/';

  entityHandle = vsi_s_open(e_running[os_MyHandle()], rivSemBaseName, 1);


  if(entityHandle  != VSI_ERROR)
  {
    /* in the riviera world, *mutex argument designates a user-defined buffer for the
        semaphore object. GPF-FRAME allocates semaphore objects on it's own. So we do copy
        the initialized object to the user-defined buffer. When *mutex is passed to the
        other semaphore REMU functions, it is used to identify the name of the semaphore.
        With REMU, a user-stored semaphore object is never used for synchronisation!
        Furthermore, its magic NU_SEMAPHORE_ID sm_id identifier(ULONG 0x53454d41) will be
        destroyed and used as an index for quick-referencing the SemTable with the operators.
        DIRTY!!! */
    memcpy(mutex, &SemTable[entityHandle].SemCB, sizeof(T_RVF_MUTEX));
    mutexP->sm_id = entityHandle;
     returnCode = RVF_OK;
  }
  else
  {
       returnCode = RVF_INTERNAL_ERR;
  }
  return returnCode;
}



/*
+--------------------------------------------------------------------+
obtain a semaphore
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_lock_mutex( T_RVF_MUTEX*  mutex)
{
  T_RVF_RET returnCode = RVF_INTERNAL_ERR;
  SM_SCB* mutexP =(SM_SCB*)mutex;
  T_HANDLE semHandle;
  T_HANDLE entityHandle;
  SM_SCB* NUCSem =(SM_SCB*)mutex;
  if (mutex==NULL)
  	return RVF_INVALID_PARAMETER;
  semHandle = mutexP->sm_id;
  entityHandle = e_running[os_MyHandle()];
  semHandle = vsi_s_get(entityHandle, semHandle);
  if(semHandle  != VSI_ERROR)
  {
    returnCode = RVF_OK;
  }

  return returnCode;
}


/*
+--------------------------------------------------------------------+
release a semaphore
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_unlock_mutex( T_RVF_MUTEX*  mutex)
{
  T_RVF_RET returnCode = RVF_INTERNAL_ERR;
  SM_SCB* mutexP =(SM_SCB*)mutex;
  T_HANDLE semHandle;
  T_HANDLE entityHandle;
  SM_SCB* NUCSem =(SM_SCB*)mutex;

  if (mutex==NULL)
  	return RVF_INVALID_PARAMETER;
  semHandle = mutexP->sm_id;
  entityHandle = e_running[os_MyHandle()];
  semHandle = vsi_s_release(entityHandle, semHandle);
  if(semHandle  != VSI_ERROR)
  {
    returnCode = RVF_OK;
  }
  return returnCode;
}


/*
+--------------------------------------------------------------------+
delete a semaphore
+--------------------------------------------------------------------+
*/
T_RVF_RET  gsp_delete_mutex( T_RVF_MUTEX*  mutex)
{
  T_RVF_RET returnCode = RVF_INTERNAL_ERR;
  SM_SCB* mutexP =(SM_SCB*)mutex;
  T_HANDLE semHandle;
  T_HANDLE entityHandle;
  SM_SCB* NUCSem =(SM_SCB*)mutex;

  if (mutex==NULL)
  	return RVF_INVALID_PARAMETER;
  semHandle = mutexP->sm_id;

  entityHandle = e_running[os_MyHandle()];
  semHandle = os_DestroySemaphore(entityHandle, semHandle); /* sorry, there is no corresponding VSI function */
  if(semHandle  != VSI_ERROR)
  {
    returnCode = RVF_OK;
  }
  return returnCode;
}

