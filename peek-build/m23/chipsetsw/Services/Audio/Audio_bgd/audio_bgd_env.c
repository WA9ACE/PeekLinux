/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_env_bgd.c                                             */
/*                                                                          */
/*  Purpose:  This file contains routines that will be called in order to:  */
/*              Memory Banks requirements,                                  */
/*            - set AUDIO BACKGROUND task and memory banks IDs              */
/*            - initialize all the AUDIO BACKGROUND data structures,        */
/*            - start the task                                              */
/*            - stop the task                                               */
/*            - kill the task                                               */
/*                                                                          */
/*  Version   1                                                             */
/*                                                                          */
/*  Date         Modification                                               */
/*  ------------------------------------                                    */
/*  19 June 2003  Create                                                    */
/*                                                                          */
/*  Author       Frederic Turgis                                            */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

// DO NOT REMOVE: ensures library is not created empty to avoid issue at linking
void audio_bgd_dummy(void)
{
}

#ifdef RVM_AUDIO_BGD_SWE

  #include "rvm/rvm_use_id_list.h"
  #include "rvm/rvm_priorities.h"
  #include "rvf/rvf_pool_size.h"
  #include "audio/audio_bgd/audio_bgd_structs_i.h"
  #include "audio/audio_bgd/audio_bgd_macro_i.h"
  #include "audio/audio_pool_size.h"

  #include <string.h>

  T_RVM_RETURN audio_bgd_get_info (T_RVM_INFO_SWE  *infoSWEnt);
  T_RVM_RETURN audio_bgd_set_info(T_RVF_ADDR_ID addrId,
                                T_RV_RETURN   return_path[],
                                T_RVF_MB_ID   mbId[],
                                T_RVM_RETURN  (*callBackFct) ( T_RVM_NAME SWEntName,
                                                               T_RVM_RETURN errorCause,
                                                               T_RVM_ERROR_TYPE errorType,
                                                               T_RVM_STRING errorMsg) );
  T_RVM_RETURN audio_bgd_init (void);
  T_RVM_RETURN audio_bgd_stop (void);
  T_RVM_RETURN audio_bgd_kill (void);

  extern T_RV_RET audio_bgd_core(void);

  /* Initialisation of the pointer to the Global Environment Control block */
  T_AUDIO_BGD_ENV_CTRL_BLK *p_audio_bgd_gbl_var = NULL;


  /********************************************************************************/
  /* Function   : audio_bgd_get_info                                              */
  /*                                                                              */
  /* Description : This function is called by the Bt Environment to learn         */
  /*               audio background requirements in terms of memory, SWEs...      */
  /*                                                                              */
  /* Parameters  : T_RVM_INFO_SWE  * swe_info: pointer to the structure to fill   */
  /*               containing infos related to the Audio SWE.                     */
  /*                                                                              */
  /* Return      :  T_BTE_RETURN                                                  */
  /*                                                                              */
  /* History      : 0.1 (11-May-2001)                                             */
  /*                                                                              */
  /*                                                                              */
  /********************************************************************************/
  T_RVM_RETURN audio_bgd_get_info(T_RVM_INFO_SWE  *infoSWEnt)
  {
    /* SWE info */
    infoSWEnt->swe_type = RVM_SWE_TYPE_4;
    infoSWEnt->type_info.type4.swe_use_id = AUDIO_BGD_USE_ID;

    strncpy(infoSWEnt->type_info.type4.swe_name, "AUDIO_BGD", RVM_NAME_MAX_LEN);

    infoSWEnt->type_info.type4.stack_pool_id = RVF_POOL_INTERNAL_MEM;
    infoSWEnt->type_info.type4.stack_size    = AUDIO_BGD_STACK_SIZE;
    infoSWEnt->type_info.type4.priority      = RVM_AUDIO_BGD_TASK_PRIORITY;


    /* Set the return path */
    infoSWEnt->type_info.type4.return_path.callback_func  = NULL;
    infoSWEnt->type_info.type4.return_path.addr_id        = 0;


    /* memory bank info */
    infoSWEnt->type_info.type4.nb_mem_bank = 1;

    /* Memory bank used to receive/send the message to/from the entity */
    strncpy(((infoSWEnt->type_info.type4.mem_bank[0]).bank_name), "AUDIO_BGD", RVF_MAX_MB_LEN);
    ((infoSWEnt->type_info.type4.mem_bank[0]).initial_params).pool_id   = RVF_POOL_EXTERNAL_MEM;
    ((infoSWEnt->type_info.type4.mem_bank[0]).initial_params).size      = AUDIO_BGD_MB1_SIZE;
    ((infoSWEnt->type_info.type4.mem_bank[0]).initial_params).watermark = AUDIO_BGD_MB1_SIZE;

    /* linked SWE info */
    /* this SWE requires no SWE to run in PC environement*/
    infoSWEnt->type_info.type4.nb_linked_swe = 0;

    /* generic functions */
    infoSWEnt->type_info.type4.set_info = audio_bgd_set_info;
    infoSWEnt->type_info.type4.init     = audio_bgd_init;
    infoSWEnt->type_info.type4.core     = audio_bgd_core;
    infoSWEnt->type_info.type4.stop     = audio_bgd_stop;
    infoSWEnt->type_info.type4.kill     = audio_bgd_kill;

    return (RV_OK);
  } /**************** End of audio_bgd_get_info function ******************************/

  /********************************************************************************/
  /* Function   : audio_bgd_set_info                                              */
  /*                                                                              */
  /* Description : This function is called by the RV Environment to inform        */
  /*               the audio SWE about task_id, mb_id and error function.         */
  /*                                                                              */
  /* Parameters  : - T_RVM_TASK_ID  taskId[]: array of task_id.                   */
  /*               - task_id[0] contains audio task_id.                           */
  /*               - T_RVF_MB_ID mb_id[]: array of memory bank ids.               */
  /*                - callback function to call in case of unrecoverable error.   */
  /*                                                                              */
  /* Return      : T_RVM_RETURN                                                   */
  /*                                                                              */
  /* History    : 1 (19-June-2003 )                                               */
  /*                                                                              */
  /*                                                                              */
  /********************************************************************************/
  T_RVM_RETURN audio_bgd_set_info(T_RVF_ADDR_ID addrId,
                              T_RV_RETURN   return_path[],
                              T_RVF_MB_ID   mbId[],
                              T_RVM_RETURN  (*callBackFct) ( T_RVM_NAME SWEntName,
                                                             T_RVM_RETURN errorCause,
                                                             T_RVM_ERROR_TYPE errorType,
                                                             T_RVM_STRING errorMsg) )
  {

    /* Declare local variable.*/
    T_RVF_MB_STATUS mb_status = RVF_GREEN;

    /* Allocate memory required to store the Global Environment control Block. */
    mb_status = rvf_get_buf(mbId[1],
                            sizeof(T_AUDIO_BGD_ENV_CTRL_BLK),
                            (T_RVF_BUFFER **) & p_audio_bgd_gbl_var);

    /* If insufficient resources to properly run the AUDIO's task, then abort. */
    switch (mb_status)
    {
      case RVF_GREEN:
      {
        /* Initialize the Global Environment Control Block */
        memset((UINT8 *) p_audio_bgd_gbl_var,
               0x00,
               sizeof (T_AUDIO_BGD_ENV_CTRL_BLK));

        /* Store the memory bank IDs assigned to the AUDIO */
        p_audio_bgd_gbl_var->mb = mbId[0];

        /* Store the address ID assigned to the AUDIO */
        p_audio_bgd_gbl_var->addrId = addrId;

        /* Store the function to be called whenever any unrecoverable */
        /* error occurs.                                              */
        p_audio_bgd_gbl_var->callBackFct = callBackFct;
        AUDIO_BGD_SEND_TRACE(" AUDIO BGD (env). information set ",
                             RV_TRACE_LEVEL_DEBUG_LOW);
      }
      break; // case RVF_GREEN:
      case RVF_YELLOW:
      {
        rvf_free_buf((T_RVF_BUFFER *) p_audio_bgd_gbl_var);
        AUDIO_BGD_TRACE_WARNING(" AUDIO BGD memory warning (orange memory)");
        return (RV_MEMORY_ERR);
      }
//omaps00090550        break; // case RVF_YELLOW:
      default:
      {
        AUDIO_BGD_TRACE_WARNING(" AUDIO BGD memory warning (red memory)");
        return (RV_MEMORY_ERR);
      }
    } /* switch (mb_status) */

    return (RV_OK);
  } /*************** End of audio_bgd_set_info function ********************************/



  /********************************************************************************/
  /* Function   : audio_bgd_init                                                      */
  /*                                                                              */
  /* Description : This function is called by the BT Environment to initialize the*/
  /*               audio SWE before creating the task and calling audio_start.    */
  /*                                                                              */
  /* Parameters  : None                                                           */
  /*                                                                              */
  /* Return      : T_RVM_RETURN                                                   */
  /*                                                                              */
  /* History    : 0.1 (20-August-2000)                                            */
  /*                                                                              */
  /*                                                                              */
  /********************************************************************************/
  T_RVM_RETURN audio_bgd_init(void)
  {
    /* Put global audio initialization */

    return RV_OK;
  }


  /*************************************************************************************/
  /* Function	  : audio_bgd_stop                                                           */
  /*                                                                                   */
  /* Description : This function is called by the BT Environment to stop the audio SWE.*/
  /*                                                                                   */
  /* Parameters  : None                                                                */
  /*                                                                                   */
  /* Return      : T_RVM_RETURN                                                        */
  /*                                                                                   */
  /* History    : 0.1 (11-May-2001)                                                    */
  /*                                                                                   */
  /*                                                                                   */
  /*************************************************************************************/
  T_RVM_RETURN audio_bgd_stop(void)
  {
    /* other SWEs have not been killed yet, audio can send messages to other SWEs      */

    return RV_OK;
  }


  /*************************************************************************************/
  /* Function   : audio_bgd_kill                                                           */
  /*                                                                                   */
  /* Description : This function is called by the BT Environment to kill the audio     */
  /*               SWE, after the audio_stop function has been called.                 */
  /*                                                                                   */
  /* Parameters  : None                                                                */
  /*                                                                                   */
  /* Return      : T_BTE_RETURN                                                        */
  /*                                                                                   */
  /* History     : 0.1 (11-May-2001)                                                   */
  /*                                                                                   */
  /*                                                                                   */
  /*************************************************************************************/
  T_RVM_RETURN audio_bgd_kill (void)
  {
    /* free all memory buffer previously allocated */
    rvf_free_buf ((T_RVF_BUFFER *) p_audio_bgd_gbl_var);
    return RV_OK;
  }

#endif // #ifdef RVM_AUDIO_BGD_SWE
