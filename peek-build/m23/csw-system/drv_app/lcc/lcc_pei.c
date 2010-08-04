/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Modul   :  LCC
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
|  Purpose :  This module implements the process body interface
	|             for the entity Low cost charger (LCC)
|
|             Exported functions:
|
|             pei_create    - Create the Protocol Stack Entity
|             pei_init      - Initialize Protocol Stack Entity
|             pei_primitive - Process Primitive
|             pei_timeout   - Process Timeout
|             pei_exit      - Close resources and terminate
|             pei_run       - Process Primitive
|             pei_config    - Dynamic Configuration
|             pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/

/*==== INCLUDES =============================================================*/

#ifndef _WINDOWS
#include "nucleus.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "inth/sys_inth.h"
#include "lcc/lcc.h"
#include "lcc/lcc_env.h"
#include "lcc/lcc_task.h"
#include "lcc/lcc_trace.h"
#include "lcc/lcc_handle_timers.h"
#include "lcc/lcc_pool_size.h"
#include "lcc/lcc_handle_timers.c"
//#include "etm/etm_tmpwr.c"
#include "chipset.cfg"
#if (ANLG_FAM !=11)
#include "abb/abb.h"
#else
#include "abb/bspTwl3029.h"
#include "abb/bspTwl3029_I2c.h"
#include "abb/bspTwl3029_Intc.h"
#include "abb/bspTwl3029_Madc.h"
#include "abb/bspTwl3029_Bci.h"
#endif

/*==== DEFINITIONS ==========================================================*/

#define VSI_CALLER LCC_handle, 
#define pei_create lcc_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

T_HANDLE LCC_handle;
T_HANDLE lcc_hCommLCC = -1;
T_HANDLE  L1_hCommL1 = VSI_ERROR;

static  BOOL          first_access  = TRUE;

/* Define a pointer to the PWR environment control block */
T_PWR_CTRL_BLOCK *pwr_ctrl = NULL;

/* Define a pointer to the PWR configuration block */
T_PWR_CFG_BLOCK  *pwr_cfg  = NULL;

#ifndef EMO_PROD
void ttr(unsigned trmask, char *format, ...);
void str(unsigned mask, char *string);
#endif


T_RVM_RETURN pwr_task_init (void);

/* Define global pointer to the error function */
T_RVM_RETURN (*pwr_error_ft) (T_RVM_NAME    swe_name,
                                     T_RVM_RETURN  error_cause,
                                     T_RVM_ERROR_TYPE  error_type,
                                     T_RVM_STRING   error_msg);

// Handle timers
#include "lcc/lcc_handle_timers.c"

/*
+------------------------------------------------------------------------------
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_config (char *inString)
{
  /* do it from scratch */
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - exit sucessful
|               PEI_ERROR         - exit not sueccessful
|
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit");
  
   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER lcc_hCommLCC);
   lcc_hCommLCC = VSI_ERROR;

   pwr_stop(NULL);

   pwr_kill();
 
   return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - entity initialised
|               PEI_ERROR         - entity not (yet) initialised
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    
    TRACE_FUNCTION ("pei_init");
    
    RVM_TRACE_DEBUG_HIGH("LCC: Initialization");
    /*
     * Initialize task handle
     */
    LCC_handle = handle;
    lcc_hCommLCC = -1;

     if(LCC_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("LCC_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( LCC_handle, "LCC" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (lcc_hCommLCC < VSI_OK)
    {
      if ((lcc_hCommLCC = vsi_c_open (VSI_CALLER "LCC" )) < VSI_OK)
        return PEI_ERROR;
    }

#if(PSP_STANDALONE==0)
  if (L1_hCommL1 < VSI_OK)
  {
    if ((L1_hCommL1 = vsi_c_open (VSI_CALLER "L1")) < VSI_OK)
    {
      return PEI_ERROR;
    }
  }
#endif
 
    ttw(ttr(TTrEnv, "pwr_set_info(%d)" NL, handle));
    ttw(ttr(TTrEnvLow, "  mbId[0] = %d" NL, 0));

    //mb_status = rvf_get_buf(EXT_MEM_POOL,sizeof(T_PWR_CTRL_BLOCK),(void **) &pwr_ctrl);
    if ((rvf_get_buf(EXT_MEM_POOL,sizeof(T_PWR_CTRL_BLOCK),(void **) &pwr_ctrl)) == RVF_RED) 
    {
        ttr(TTrFatal, "LCC FATAL: no memory: %d" NL, handle);
        return (RVM_MEMORY_ERR);
    }
    memset((void *)pwr_ctrl, 0xBA, sizeof(T_PWR_CTRL_BLOCK));

    //mb_status = rvf_get_buf(EXT_MEM_POOL,sizeof(T_PWR_CFG_BLOCK),(void **) &pwr_cfg);
    if ((rvf_get_buf(EXT_MEM_POOL,sizeof(T_PWR_CFG_BLOCK),(void **) &pwr_cfg)) == RVF_RED) 
    {
        ttr(TTrFatal, "LCC FATAL: no memory: %d" NL, handle);
        return (RVM_MEMORY_ERR);
    }
    memset((void *)pwr_cfg , 0xBA, sizeof(T_PWR_CFG_BLOCK));

    /* store the pointer to the error function */
    pwr_error_ft = rvm_error;

    /* Store the addr id */
    pwr_ctrl->addr_id = handle;


    /* Store the memory bank id */
    pwr_ctrl->prim_id = EXT_MEM_POOL;

    /* Battery & charging related initialisation */

    // State initialisation
    pwr_ctrl->state = CAL;

    // Create timers (Initialize...)
    pwr_create_timer(&pwr_ctrl->time_begin_T0);
    pwr_create_timer(&pwr_ctrl->time_begin_T1);
    pwr_create_timer(&pwr_ctrl->time_begin_T2);
    pwr_create_timer(&pwr_ctrl->time_begin_T3);
    pwr_create_timer(&pwr_ctrl->time_begin_T4);
    pwr_create_timer(&pwr_ctrl->time_begin_mmi_rep);

    /* Flags initialisation and other state related init */

    // Calibration configuration file not read
    pwr_ctrl->flag_cal_cfg_read = 0;

    // Charger configuration file not read
    pwr_ctrl->flag_chg_cfg_read = 0;

    // Default configuration id used
    pwr_cfg->data.cfg_id = '1';

    // Number of battery identification measurements reported
    pwr_ctrl->count_bat_type = 0;
    pwr_ctrl->count_chg_type = 0;
    pwr_ctrl->count_USB_type = 0;

    // Precharge PRE state - Applies only for 3.2V < Vbat < 3.6V
    pwr_ctrl->flag_prechg_started = 0;

    // Charger Interrupts are disabled from the start
    pwr_ctrl->flag_chg_int_disabled = 1;

    // First time in INI state
    pwr_ctrl->flag_ini_virgo = 0;

    // Default charger type is UNREGULATED
    // Meaning charger interrupts will not be enabled
    pwr_cfg->chg.type = UNREGULATED;

    // Unplug default is that charger is unplugged (0)
    memset(&pwr_ctrl->chg_unplug_vec , 0x0, CONSECUTIVE_CHG_UNPLUGS);
    memset(&pwr_ctrl->USB_unplug_vec,0x0,CONSECUTIVE_USB_UNPLUGS);
	
    pwr_ctrl->index = 0;


   // MMI hasn't registered
   pwr_ctrl->flag_mmi_registered = 0;

	   // Topping charge must not be applied
   	   pwr_ctrl->flag_topping_chg = 0;
		
   // Init of plug/unplug state machine - from default the charger is not plugged
   pwr_ctrl->flag_chg_plugged = 0;
   pwr_ctrl->flag_USB_plugged = 0;
   pwr_ctrl->flag_chg_prev_plugged = 0;
   pwr_ctrl->flag_USB_prev_plugged = 0;

    pwr_init ();
	
//UNCOMMENT THE FOLLOWING for the etm_registration !!!!!
    etm_pwr_init();
   
    RVM_TRACE_DEBUG_HIGH("LCC: Complete");
    return (PEI_OK);
   
}

LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{
	pwr_task();
	return (PEI_OK);
}
    


/*
+------------------------------------------------------------------------------
| Function    : pei_create
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when the process is
|                created.
|
| Parameters  : out_name          - Pointer to the buffer in which to locate
|                                   the name of this entity
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{
	static const T_PEI_INFO pei_info =
              {
               "LCC",         
               {              
                 pei_init,      
                 pei_exit,      
                 NULL,
                 NULL,           /* NO pei_timeout */
                 NULL,           /* NO pei_signal */
                 pei_run,           /*  pei_run */
                 pei_config,     /*  pei_config */
                 NULL            /* NO pei_monitor */
               },
               LCC_STACK_SIZE,     /* stack size */
               20,        /* queue entries */
               (255 - LCC_TASK_PRIORITY),      /* priority (1->low, 255->high) */
               0,        /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND |  TRC_NO_SUSPEND
              };

	TRACE_FUNCTION ("pei_create");
        RVM_TRACE_DEBUG_HIGH("LCC: pei_create");
	/*
         * Close Resources if open
         */
       if (first_access)
           first_access = FALSE;
       else
           pei_exit();

      /*
       * Export startup configuration data
       */
      *info = (T_PEI_INFO *)&pei_info;
  
      return PEI_OK;
}

	/*
	+------------------------------------------------------------------------------
	| Function    : pwr_init
	+------------------------------------------------------------------------------
	| Description :  This function is called from pei_init
	|			  Does some LCC specific initialisations(enabling interrupts, registering interrupt handlers,etc.)  	
	|
	| Parameters  : None
	|
	| Return      : PEI_OK            - entity created successfuly
	|               PEI_ERROR         - entity could not be created
	|
	+------------------------------------------------------------------------------
	*/

T_RVM_RETURN pwr_init(void)
{
   T_RVM_RETURN error;
   BspTwl3029_Bci_i2vConfigInfo i2vconfig;
   // Mask off the charger interrupts (plug/unplug) - in case it was a linear charger
   // Don't want to get killed in a vast number of interrupts...
   #if (ANLG_FAM!=11)
   ABB_Write_Register_on_page(PAGE0, ITMASK, CHARGER_IT_MSK);
   #else
   
   	#if (ENABLE_CI_CV_OR_PWM == 1)
   	bspTwl3029_IntC_setHandler(BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER,pwr_abb_interrupt_handler);
	#endif
	
		#if 0
   	bspTwl3029_IntC_setHandler(BSP_TWL3029_INTC_SOURCE_ID_BCI_CHARGE_STOP,pwr_abb_interrupt_handler);
   	bspTwl3029_IntC_setHandler(BSP_TWL3029_INTC_SOURCE_ID_BATT_LOW,pwr_abb_interrupt_handler);
   	bspTwl3029_IntC_setHandler(BSP_TWL3029_INTC_SOURCE_ID_WD_OVERFLOW,pwr_abb_interrupt_handler);
   	//bspTwl3029_IntC_setHandler( BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS, pwr_abb_interrupt_handler);
	   	#endif 
   	
   	#if (ENABLE_CI_CV_OR_PWM == 1)
   	//enable interrupt source for charger plug/unplug	
   	bspTwl3029_IntC_enableSource(BSP_TWL3029_INTC_SOURCE_ID_BATT_CHARGER);
   	#endif 	
   	
      #if ENABLE_USB_CHARGING
    	bspTwl3029_IntC_registerVBUSCallback(BSP_TWL3029_INTC_SOURCE_ID_PM_USB_VBUS, pwr_abb_interrupt_handler);
   	#endif  	 //end of ENABLE_USB_CHARGING
   	//bspTwl3029_IntC_enableSource(BSP_TWL3029_INTC_SOURCE_ID_WD_OVERFLOW);


		//Sets the contents of the register BCICTL2 (Refer bci triton specification for details)
		//Bit		7		6		5		4		3		2			1			0
		//Name		OFFSEN	OFFSN	OFFSN	CGAIN4	CLIB	ACCSUPEN	ACPATHEN	USBPATHEN
		//Read/Write	R/W		R/W		R/W		R/W		R/W		R/W			R/W			R/W
		//Reset 		0		0		0		0		0		0			0			0

	   	i2vconfig.offsetEn = BSP_TWL3029_BCI_DISABLE;    //disable the offset settings
	   	i2vconfig.offsetVal = BSP_TWL3029_BCI_I2VOFF_100MV;	 //offset settings are disabled no significants for this field
	   	i2vconfig.gain= BSP_TWL3029_BCI_I2VGAIN_4;		 //Reduce the gain of the current to voltage converter from 10 (CGAIN4=0) to 4 (CGAIN4=1).	
	   	i2vconfig.calibEn= BSP_TWL3029_BCI_DISABLE;	 //zero calibration rutine is disabled
	   	bspTwl3029_Bci_i2vConfig(NULL,&i2vconfig);   
   #endif

   pwr_ttr_init(TTrFatal | TTrWarning | TTrEvent);
   pwr_ctrl->tmask = TTrFatal | TTrWarning | TTrEvent;
   ttw(ttr(TTrEnv, "pwr_init(%d)" NL, 0));
   return RV_OK;
}

T_RVM_RETURN pwr_start(void)
{
   T_RVM_RETURN error;
   ttw(ttr(TTrEnv, "pwr_start(%d)" NL, 0));
   pwr_task();
   ttw(ttr(TTrEnv, "pwr_start(%d)" NL, 0xFF));
   return RV_OK;
}

T_RVM_RETURN pwr_stop(T_RV_HDR *msg)
{
   ttw(ttr(TTrEnv, "pwr_stop(%d)" NL, 0));
   return RV_OK;
}

T_RVM_RETURN pwr_kill (void)
{
   T_RVM_RETURN error;

   ttw(ttr(TTrEnv, "pwr_kill(%d)" NL, 0));

   /* free all memory buffer previously allocated */
   rvf_free_buf ((void *) pwr_ctrl);
   rvf_free_buf ((void *) pwr_cfg);
   ttw(ttr(TTrEnv, "pwr_kill(%d)" NL, 0xFF));

   return RV_OK;
}

