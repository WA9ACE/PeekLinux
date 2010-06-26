/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  SDCard
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
|             for the entity SDCard
|

|
|          pei_create    - Create the Protocol Stack Entity
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_run       - Process Messages
|          pei_primitive - Process Primitive
|          pei_signal    - Process Signals
|          pei_timeout   - Process Timeout
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+-----------------------------------------------------------------------------
*/

/*============================ Includes ======================================*/

//#ifndef _WINDOWS
///#include "nucleus.h"
//#endif

#ifndef _WINDOWS
  #include "nucleus.h"
  #include "inth/sys_inth.h"

#if 1 // Added for MSDCard compilation in Locosto
#define C_INTH_MMC_SD_MS_IT      27
#endif // #if 1 // Added for MMC compilation in Locosto

  #define C_INTH_MC_SD_MS_IT C_INTH_MMC_SD_MS_IT
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

#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include <string.h>
#include "mc/board/mc_func.h"
#include "mc/mc_i.h"
#include "mc/mc_env.h"
#include "mc/mc_commands.h"
#include "mc/mc_i.h"
#include "mc/board/mc_inth.h"
#include "armio/armio.h"

#include "spi/spi_drv.h"
/*============================ Macros ========================================*/
#define VSI_CALLER MC_handle,
#define pei_create mc_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID)


/*============================ Global Variables ==============================*/

/**
 * Pointer on the structure gathering all the global variables
 * used by MC instance.
 */
T_MC_ENV_CTRL_BLK *mc_env_ctrl_blk_p = NULL;

#ifndef _WINDOWS
#ifndef HISR_STACK_SHARING
  extern char mc_hisr_stack[MC_HISR_STACK_SIZE];
#endif
#endif

T_HANDLE MC_handle;
T_HANDLE mc_hCommMC = -1;

static BOOL first_access = TRUE;

void mc_spi_configuration(void);

T_SPI_DEV init_spi_device;

/*============================ Function Definition============================*/

/*
+------------------------------------------------------------------------------
|  Function     :  pei_timeout
+------------------------------------------------------------------------------
|  Description  :  Process timeout.
|
|  Parameters   :  index     - timer index
|
|  Return       :  PEI_OK    - timeout processed
|                  PEI_ERROR - timeout not processed
+------------------------------------------------------------------------------
*/
LOCAL void pei_timeout (int h1,int h2,USHORT tnum)
{
 // RVM_TRACE_DEBUG_HIGH ("SDCard: pei_timeout");

 // rvf_send_event(mc_env_ctrl_blk_p->addr_id,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );
 // return PEI_OK;

}/* End of pei_timeout(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   RVM_TRACE_DEBUG_HIGH ("SDCard: pei_exit");

   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER mc_hCommMC);
   mc_hCommMC = VSI_ERROR;

   mc_stop (NULL);
   mc_kill();
   return PEI_OK;
}/* End of pei_exit(..) */

/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;
   char buf[64] = "";

   RVM_TRACE_DEBUG_HIGH("SDCard: pei_run");



   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         mc_handle_message(msg_p);
      }
#if  0 // Commented for compiling MMC in Locosto
	  if (received_event & RVF_TIMER_0_EVT_MASK)
      {
	 SDCard_handle_timer(NULL);
      }
#endif //#if 0 // Commented for compiling MMC in Locosto
   }

   return PEI_OK;

}/* End of pei_run(..) */

/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;

    RVM_TRACE_DEBUG_HIGH("SDCardTASK: Initialization");
    /*
     * Initialize task handle
     */
    MC_handle = handle;
    mc_hCommMC = -1;

     if(MC_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("SDCard_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( MC_handle, "MC" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (mc_hCommMC < VSI_OK)
    {
      if ((mc_hCommMC = vsi_c_open (VSI_CALLER "MC" )) < VSI_OK)
        return PEI_ERROR;
    }

    RVM_TRACE_DEBUG_HIGH("SDCard Open communication channels done");

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_MC_ENV_CTRL_BLK), (T_RVF_BUFFER**)&mc_env_ctrl_blk_p) != RVF_OK)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        RVM_TRACE_DEBUG_HIGH ("SDCardTASK: Error to get memory ");

        return PEI_ERROR;

    }

    /* Store the address ID. */
    mc_env_ctrl_blk_p->addr_id = handle;
    mc_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;


	RVM_TRACE_DEBUG_HIGH("calling MC_SPI_CONFIGURATION");
	mc_spi_configuration();
	mc_init();
    mc_start();
    RVM_TRACE_DEBUG_HIGH("SDCardTASK: pei_init Complete");

    return PEI_OK;
} /* End pei_init */

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
               "MC",         /* name */
               {              /* pei-table */
                  pei_init,
	              pei_exit,
                  NULL, //pei_primitive,   /*-- PASSIVE Entity--*/
                  NULL,            /* pei_timeout */
                  NULL, 	   /* pei_signal */
                  pei_run, //NULL,            /* pei_run */
                  NULL,            /* pei_config */
                  NULL             /* pei_monitor */
	           },
               MC_STACK_SIZE,            /* stack size */
               10,                        /* queue entries */
               (255-MC_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               0,  //1,                         /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // | PASSIVE_BODY
		       /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("SDCard: pei_create");

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
}/* End pei_create(..) */





/**
 * Called by the RV manager to initialize the
 * SDCard SWE before creating the task and calling SDCard_start.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN SDCard_init (void)
{
  UINT16 i;

  /*
   * Here the instance (SDCard_env_ctrl_blk_p) is initialised.
   */
  MC_SEND_TRACE("SDCardinit: initialising mc_env_ctrl_blk",
                  RV_TRACE_LEVEL_DEBUG_LOW);

#ifndef _WINDOWS
  // Fill the entire stack with the pattern 0xFE
  if(mc_env_ctrl_blk_p != NULL)
  {
    #ifndef HISR_STACK_SHARING
    	memset (mc_env_ctrl_blk_p->mc_hisr_stack, 0xFE, MC_HISR_STACK_SIZE);
    #else
	memset (HISR_STACK_PRIO2, 0xFE, HISR_STACK_PRIO2_SIZE);
    #endif
  }
  else
  {
    MC_SEND_TRACE ("SDCardinit no ctrl_blk allocated", RV_TRACE_LEVEL_ERROR);
    return RVM_MEMORY_ERR;
  }

  if ( NU_SUCCESS !=
        NU_Create_HISR (&mc_env_ctrl_blk_p->mc_hisr,
                      "MC_HISR",
                      mc_hisr,
                      2,
    		#ifndef HISR_STACK_SHARING
                      mc_env_ctrl_blk_p->mc_hisr_stack,
                      MC_HISR_STACK_SIZE)
    		#else
		      HISR_STACK_PRIO2,
		      HISR_STACK_PRIO2_SIZE)
    		#endif
      )
    {
      MC_SEND_TRACE ("SDCardinit HISR creation FAILED", RV_TRACE_LEVEL_ERROR);
      /** memory error no control block allocated so */
      return RVM_MEMORY_ERR;
    }
#endif

  /** initialise the host configuration */
  mc_env_ctrl_blk_p->conf.dw       = 0x0;            /* data width */
  mc_env_ctrl_blk_p->conf.mode     = 0x0;            /* SDCardmode */
  mc_env_ctrl_blk_p->conf.power_up = 0x1;            /* Power up */
  mc_env_ctrl_blk_p->conf.be       = 0x0;            /* Little endian */
  mc_env_ctrl_blk_p->conf.clk_div  = MC_ID_CLK_DIV; /* Clk devider */
  mc_env_ctrl_blk_p->conf.cto      = 0xFF;           /* CMD time-out */
  mc_env_ctrl_blk_p->conf.dto      = 0xFFFE;          /* Data time-out */
  mc_env_ctrl_blk_p->gpio_status   = 0;


  /** initialise the reserved subscriber id's */
  mc_env_ctrl_blk_p->reserved_subscriber_id = 0;

  for(i=0;i<MC_MAX_SUBSCRIBER;i++)
  {
    /** initialise the list of subscribers */
    mc_env_ctrl_blk_p->subscriber_infos[i] = NULL;
  }

  mc_env_ctrl_blk_p->reserved_card_id = 0;

  for(i=0;i<MC_MAX_STACK_SIZE;i++)
  {
  /** initialise the list of subscribers */
    mc_env_ctrl_blk_p->card_infos[i] = NULL;
  }

  /** set current driver state */
  mc_env_ctrl_blk_p->state = MC_STATE_INITIALISED;

  /** set default mode */
  mc_env_ctrl_blk_p->dma_mode = MC_DMA_DEFAULT_MODE;

  /** initialise IRQ mask */
  mc_env_ctrl_blk_p->irq = (MC_IRQ_EOC | MC_IRQ_BRS | MC_IRQ_AE |
                             MC_IRQ_AF | MC_IRQ_EOFB | MC_IRQ_CTO |
                             MC_IRQ_DTO | MC_IRQ_CERR);

  /** initialised! */
  mc_env_ctrl_blk_p->initialised = FALSE;

  mc_env_ctrl_blk_p->mc_card_inserted = MC_CARD_INSERTED;

  /** Clear event status */
  mc_env_ctrl_blk_p->mc_host_received_events = 0;

  mc_return_queue_init(  mc_env_ctrl_blk_p->addr_id,
                                       MC_RVF_MAILBOX_2,
                                       &mc_env_ctrl_blk_p->path_to_return_queue);




  return RVM_OK;
}


/**
 * Called by the RV manager to start the SDCardSWE,
 * Note: Only for type 1, 2 and 3 SWE.
 *
 * @return  RVM_OK.
 */
T_RVM_RETURN mc_start(void)
{
  T_RVM_RETURN retval = RVM_OK;
  T_MC_INIT_MSG  *msg_p;


  MC_SEND_TRACE("SDCardstart: initialising host controller",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if(mc_env_ctrl_blk_p == NULL)
  {

	MC_SEND_TRACE("SDCardstart: no mc_env_ctrl_blk available",RV_TRACE_LEVEL_ERROR);
    return RVM_INTERNAL_ERR;
  }

  /** Start SDCardSWE timer */
 // rvf_create_timer(mc_env_ctrl_blk_p->addr_id,0,TRUE, pei_timeout );

  /** Inside this timer, card Insert/Removal will be polled*/
 // rvf_start_timer(RVF_TIMER_0, RVF_MS_TO_TICKS(MC_CONF_TIMER_INTERVAL), TRUE);  /* start periodic timer*/



#ifndef _WINDOWS
  F_INTH_ENABLE_ONE_IT(C_INTH_MC_SD_MS_IT)
#endif


    /** send initialize message to MC/SD SWE itself*/
    /** This will be the first messages to be handled*/
    /** When Init Message has been handled by the messagehandler*/
    /** The SDCardSWE will be ready for usage */

      /* reserve message buffer */
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_INIT_MSG),
                         MC_INIT_MSG,
                         (T_RV_HDR **) &msg_p) == RVF_RED)
    {
      MC_SEND_TRACE("SDCardstart: out of memory for init message",
                RV_TRACE_LEVEL_ERROR);

      return RVM_MEMORY_ERR;
    }

    /** Send message*/
    retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p);
    if(retval != RV_OK)
    {
      return RVM_INTERNAL_ERR;
    }
    MC_SEND_TRACE("SDCardstart: send init message",RV_TRACE_LEVEL_DEBUG_LOW);

  return RVM_OK;
}


/**
 * Called by the RV manager to stop the SDCardSWE.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN mc_stop (T_RV_HDR * msg)
{
  /*
   * Here we should stop the activities of the SWE
   * It is still possible to send messages to other SWE, to unregister for example.
   */
  MC_SEND_TRACE("SDCard stop called", RV_TRACE_LEVEL_DEBUG_LOW);

#ifndef _WINDOWS
  if (NU_SUCCESS != NU_Delete_HISR (&mc_env_ctrl_blk_p->mc_hisr))
    {
      MC_SEND_TRACE ("SDCardstop: HISR deletion FAILED",
                       RV_TRACE_LEVEL_ERROR);
    }
  else
    {
      MC_SEND_TRACE ("SDCardstop: HISR deleted",
                       RV_TRACE_LEVEL_WARNING);
    }
#endif
  mc_env_ctrl_blk_p->state = MC_STATE_STOPPED;

  return RVM_OK;
}


/**
 * Called by the RV manager to kill the SDCardSWE,
 * after the mc_stop function has been called.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN mc_kill (void)
{
  /*
   * Here we cannot send messages anymore. We only free the last
   * used resources, like the control block buffer.
   */
  MC_SEND_TRACE("MC: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

  MC_FREE_BUF(mc_env_ctrl_blk_p);

  return RVM_OK;
}

void mc_spi_configuration(void)
{
	UINT8 i;

T_MC_OCR ret_ocr  = 0;
T_MC_CARD_INFO *p;
UINT32 card_stat = 0;

		spi_initdev(); // Initialise the SPI peripheral
#if 0
		go_idle_state(); //Send CMD0
		for (i=0; i<3;i++)
			{
		send_op_cond(0x00, &ret_ocr);//CMD1
			}
        //send_cid(p->cid);
		send_status(&card_stat);
#endif
}



/**
 * Called by the RV manager to initialize the
 * mc SWE before creating the task and calling mc_start.
 *
 * @return  RVM_OK
 */
T_RVM_RETURN mc_init (void)
{
  UINT16 i;
	volatile UINT16 Gpio_12_pin_state = 0xFF;
  /*
   * Here the instance (mc_env_ctrl_blk_p) is initialised.
   */
  MC_SEND_TRACE("MC init: initialising mc_env_ctrl_blk",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  rvf_send_trace("MC init: initialising mc_env_ctrl_blk",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
#ifndef _WINDOWS
  // Fill the entire stack with the pattern 0xFE
  if(mc_env_ctrl_blk_p != NULL)
  {
    #ifndef HISR_STACK_SHARING
    	memset (mc_env_ctrl_blk_p->mc_hisr_stack, 0xFE, MC_HISR_STACK_SIZE);
    #else
	memset (HISR_STACK_PRIO2, 0xFE, HISR_STACK_PRIO2_SIZE);
    #endif
  }
  else
  {
    MC_SEND_TRACE ("MC init no ctrl_blk allocated", RV_TRACE_LEVEL_ERROR);
    return RVM_MEMORY_ERR;
  }

#endif

#ifndef T_FLASH_HOT_SWAP		/* According to new GPIO framework HISR is already created and user needs to plugin his own callback function */

  if ( NU_SUCCESS !=
        NU_Create_HISR (&mc_env_ctrl_blk_p->mc_hisr,
                      "MC_HISR",
                      mc_hisr,
                      2,
    		#ifndef HISR_STACK_SHARING
                      mc_env_ctrl_blk_p->mc_hisr_stack,
                      MC_HISR_STACK_SIZE)
    		#else
		      HISR_STACK_PRIO2,
		      HISR_STACK_PRIO2_SIZE)
    		#endif
      )
    {
      MC_SEND_TRACE ("MC init HISR creation FAILED", RV_TRACE_LEVEL_ERROR);
      /** memory error no control block allocated so */
      return RVM_MEMORY_ERR;
    }

#endif


#ifdef T_FLASH_HOT_SWAP		/*	Configuring callback function and interrupts with new GPIO framework */

Gpio_12_pin_state= ((*(volatile UINT16*) ARMIO_IN) >>12)& 0x0001;

  if(Gpio_12_pin_state==0)
  {
  	Al_int_config(12, mc_hisr, 1);			/*	Card has been inserted -- Configure next interrupt for rising edge	*/
  	AI_Enable_Debounce(12, 1, 1);
  }
  else
  {
  	Al_int_config(12, mc_hisr, 0);			/*	Card has been removed -- Configure next interrupt for falling edge	*/
  	AI_Enable_Debounce(12, 0, 1);
  }

AI_Set_Debounce_Time(26000);

#endif

  /** initialise the host configuration */
  mc_env_ctrl_blk_p->conf.dw       = 0x0;            /* data width */
  mc_env_ctrl_blk_p->conf.mode     = 0x0;            /* MC mode */
  mc_env_ctrl_blk_p->conf.power_up = 0x1;            /* Power up */
  mc_env_ctrl_blk_p->conf.be       = 0x0;            /* Little endian */
  mc_env_ctrl_blk_p->conf.clk_div  = MC_ID_CLK_DIV; /* Clk devider */
  mc_env_ctrl_blk_p->conf.cto      = 0xFF;           /* CMD time-out */
  mc_env_ctrl_blk_p->conf.dto      = 0xFFFE;          /* Data time-out */
  mc_env_ctrl_blk_p->gpio_status   = 0;


  /** initialise the reserved subscriber id's */
  mc_env_ctrl_blk_p->reserved_subscriber_id = 0;

  for(i=0;i<MC_MAX_SUBSCRIBER;i++)
  {
    /** initialise the list of subscribers */
    mc_env_ctrl_blk_p->subscriber_infos[i] = NULL;
  }

  mc_env_ctrl_blk_p->reserved_card_id = 0;

  for(i=0;i<MC_MAX_STACK_SIZE;i++)
  {
  /** initialise the list of subscribers */
    mc_env_ctrl_blk_p->card_infos[i] = NULL;
  }

  /** set current driver state */
  mc_env_ctrl_blk_p->state = MC_STATE_INITIALISED;

  /** set default mode */
  mc_env_ctrl_blk_p->dma_mode = MC_DMA_DEFAULT_MODE;

  /** initialise IRQ mask */
  mc_env_ctrl_blk_p->irq = (MC_IRQ_EOC | MC_IRQ_BRS | MC_IRQ_AE |
                             MC_IRQ_AF | MC_IRQ_EOFB | MC_IRQ_CTO |
                             MC_IRQ_DTO | MC_IRQ_CERR);

  /** initialised! */
  mc_env_ctrl_blk_p->initialised = FALSE;

  mc_env_ctrl_blk_p->mc_card_inserted = MC_CARD_INSERTED;

  /** Clear event status */
  mc_env_ctrl_blk_p->mc_host_received_events = 0;

  mc_return_queue_init(  mc_env_ctrl_blk_p->addr_id,
                                       MC_RVF_MAILBOX_2,
                                       &mc_env_ctrl_blk_p->path_to_return_queue);

 rvf_send_trace("MC init: initialisation mc_env_ctrl_blk complete",45, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);

AI_SetBit(MC_GPIO_26);//CS HIGH
  return RVM_OK;
}
