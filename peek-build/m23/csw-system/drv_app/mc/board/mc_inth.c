/*/ * @file mc_inth.c
 *
 * MC initialise and handle interrupt functions .
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  6/17/2003  ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "armio/armio.h"
#ifndef _WINDOWS
  #include "nucleus.h"
  #include "inth/sys_inth.h"
  #include "memif/mem.h"
#if 1 // Added for MMC compilation in Locosto
#define C_INTH_MMC_SD_MS_IT      27
#endif // #if 1 // Added for MMC compilation in Locosto

  #define C_INTH_MC_SD_MS_IT C_INTH_MMC_SD_MS_IT
#endif

//#include "nucleus.h"
#include "STRING.h"
#include "sys_types.h"

#include "mc/board/mc_inth.h"
#include "mc/mc_i.h"
//#include "inth/sys_inth.h"

extern int MC_handle;


UINT16 mc_host_wait_event_to (UINT16 expected_event, UINT16 saved_event,
                        UINT32 timeout)
{
  UINT16 received_event;

  /* Did we already received the event? */
  if ((expected_event & mc_env_ctrl_blk_p->mc_host_received_events) != 0)
  {
    received_event = mc_env_ctrl_blk_p->mc_host_received_events;
    MC_SEND_TRACE_PARAM("MC mc_host_wait_event_to: expected event",
                        received_event, RV_TRACE_LEVEL_DEBUG_LOW);
    mc_env_ctrl_blk_p->mc_host_received_events &= ~expected_event;
  }
  else
  {
    for (;;)
      {
        received_event = rvf_wait (MC_RVF_HOST_EVENT, timeout);
        if (received_event == 0)
        {
          /*event did not occur*/
          MC_SEND_TRACE_PARAM("MC mc_host_wait_event_to: timeout while waiting for event",
                              expected_event, RV_TRACE_LEVEL_DEBUG_LOW);
          break;
        }

        /*get host controller events*/
        received_event = mc_env_ctrl_blk_p->mc_host_received_events;

        if ((received_event & expected_event) != 0 /* (at least) expected event */ )
        {
          MC_SEND_TRACE_PARAM("MC mc_host_wait_event_to: expected event",
                              received_event, RV_TRACE_LEVEL_DEBUG_LOW);
          mc_env_ctrl_blk_p->mc_host_received_events &= ~expected_event;
          break;
        }
        MC_SEND_TRACE_PARAM("MC mc_host_wait_event_to: ignored event",
                              received_event, RV_TRACE_LEVEL_DEBUG_LOW);
        /* only received other events we were not interested in: retry */
      }
    }

    /*delete all pending events*/
    //mc_env_ctrl_blk_p->mc_host_received_events = 0;

    return received_event;
}

void mc_hisr (void)
{
  extern T_MC_ENV_CTRL_BLK *mc_env_ctrl_blk_p;
  BOOL   event_triggered = FALSE;
  T_MC_CARD_INS_MSG *msg_p_insert;
  T_MC_CARD_REM_MSG *msg_p_remove;
  T_RV_RET retval;

#ifdef T_FLASH_HOT_SWAP

  volatile UINT16 interrupt_edge=0xFF;			/*	Variable vale to be passed to change the Interrupt edge	*/
  volatile UINT16 Gpio_12_pin_state = 0xFF;		/*	Variable to read the GPIO 12 pin status to decide card insert/removal	*/

#endif


//rvf_send_trace("Interrupt generated we are in mc_hisr",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);




// T_FLASH_HOT_SWAP
Gpio_12_pin_state= ((*(volatile UINT16*) ARMIO_IN) >>12)& 0x0001;

  if(Gpio_12_pin_state==0)
  interrupt_edge=1;			/*	Card has been inserted -- Configure next interrupt for rising edge	*/
  else
  interrupt_edge=0;			/*	Card has been removed -- Configure next interrupt for falling edge	*/


  /*	Change the interrupt edge so that next card event (either Insert or remove) will be detected		*/

  AI_SelectIOForIT(12,interrupt_edge);
  AI_Enable_Debounce(12, interrupt_edge, 1);
  AI_Set_Debounce_Time(26000);



  //	Interrupt triggred due to card insert Send the card insert message to MC task
  if(Gpio_12_pin_state==0)
  {

    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_CARD_INS_MSG),
                         MC_CARD_INS_MSG,
                         (T_RV_HDR **) &msg_p_insert) != RVF_GREEN)
    {
      MC_SEND_TRACE ("T_MC_CARD_INS_MSG: out of memory",
                      RV_TRACE_LEVEL_WARNING);
  //    return RV_MEMORY_ERR;
   }

   mc_env_ctrl_blk_p->addr_id = MC_handle ;

  (*(mc_env_ctrl_blk_p->ntfcCallback))(MC_INSERT_EVENT,mc_env_ctrl_blk_p->ntfcCtx);
  //	Send message to MC mailbox
  retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p_insert);
  }



  //	Interrupt triggred due to card removal Send the card removal message to MC task
  if(Gpio_12_pin_state==1)
  {

      //	Reserve message buffer
    if (rvf_get_msg_buf (mc_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_MC_CARD_REM_MSG),
                         MC_CARD_REM_MSG,
                         (T_RV_HDR **) &msg_p_remove) == RVF_RED)
    {
      MC_SEND_TRACE ("T_MC_CARD_REM_MSG: out of memory",
                      RV_TRACE_LEVEL_WARNING);
  //    return RV_MEMORY_ERR;
    }

    mc_env_ctrl_blk_p->addr_id = MC_handle ;

	// Send Notification to MMI throu Callback
   (*(mc_env_ctrl_blk_p->ntfcCallback))(MC_REMOVE_EVENT,mc_env_ctrl_blk_p->ntfcCtx);

    //	Send message to MC mailbox
    retval = rvf_send_msg (mc_env_ctrl_blk_p->addr_id, msg_p_remove);
}

}


/*-----------------------------------------------------------------------*/
/* mc_int_handler()                                                     */
/*                                                                       */
/* This function is called when an interrupt occurs. It will first check */
/* if the MC module actually exist and if it is not uninitialised. When */
/* it exists and is initialising, the interrupt High interrupt service   */
/* routine is called that will in time call the mc interrupt handler    */
/*-----------------------------------------------------------------------*/

void mc_int_handler(void)
{

 MC_SEND_TRACE("mc_int_handler function",RV_TRACE_LEVEL_DEBUG_LOW);
#if 0
  extern T_MC_ENV_CTRL_BLK *mc_env_ctrl_blk_p;
  F_INTH_DISABLE_ONE_IT(C_INTH_MC_SD_MS_IT)
  /* If mc is not started, return immediately */
  if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
  {
    /* clear the interrupt */
    F_INTH_ENABLE_ONE_IT(C_INTH_MC_SD_MS_IT)
    return;
  }
  else
  {
    /** Activate HISR */
    NU_Activate_HISR(&mc_env_ctrl_blk_p->mc_hisr);
  }
  #endif
  AI_UnmaskIT(10);
 MC_SEND_TRACE("mc_int_handler function",RV_TRACE_LEVEL_DEBUG_LOW);
}

