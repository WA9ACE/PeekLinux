/**
 * @file  mc_handle_timer.c
 *
 * MC handle_timer function, which is called when a timer
 * of the SWE expires.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */


#include "mc/mc_i.h"
#include "mc/mc_api.h"
#include "mc/mc_operations.h"


/**
 * Called every time the SW entity is in WAITING 
 * state and one of its timers has expired.
 * 
 *
 * @param msg_p riviera message pointer
 * 
 * @return  RVM_OK
 *          RVM_MEMORY_ERR
 */
T_RVM_RETURN mc_handle_timer (T_RV_HDR * msg_p)
{ 
	UINT16 test = 0;
  extern T_MC_ENV_CTRL_BLK *mc_env_ctrl_blk_p;
 
  /*
   * Got a timer event.
   */
     
//  if(msg_p != NULL)
//  {
    #ifndef _WINDOWS
    
//      MCcardvalue = MC_WRITE_PROTECTED | (MC_CARD_INSERTED << 1);
//      MC_SEND_TRACE_PARAM("MC CARDVALUES",MCcardvalue,
//                      RV_TRACE_LEVEL_ERROR);
    
    
      if (mc_env_ctrl_blk_p == NULL || mc_env_ctrl_blk_p->initialised == FALSE)
      {
        /** Do nothing not initialized */
      }
      else
      {
    
        /** Check for Card insertion or removal*/

        if(MC_CARD_INSERTED != mc_env_ctrl_blk_p->mc_card_inserted)
        {
          mc_env_ctrl_blk_p->mc_card_inserted = MC_CARD_INSERTED;
          if(MC_CARD_INSERTED)
          {
            /** Card inserted send this event to all subscribed subscribers*/
            MC_SEND_TRACE("card inserted",RV_TRACE_LEVEL_ERROR);
            mc_op_handle_card_event(MC_EVENT_INSERTION);
          }
          else
          {
            /** Card removed send this event to all subscribed subscribers*/
            MC_SEND_TRACE("card removed",RV_TRACE_LEVEL_ERROR);
            mc_op_handle_card_event(MC_EVENT_REMOVAL);
          }
        }                   
      }
    #else
      /** Test functions to trigger insert/remove events in C++*/
      if(test == 1)
      {
       mc_op_handle_card_event(MC_EVENT_INSERTION);
       test = 0;
      }
      if(test == 2)
      {
       mc_op_handle_card_event(MC_EVENT_REMOVAL);
       test = 0;
      }  
      MC_SEND_TRACE("1 second timer interupt",
                    RV_TRACE_LEVEL_DEBUG_LOW);
    #endif                  
    
      //<<  
//    if (rvf_free_buf(msg_p) != RVF_OK)
//    {
//        MC_SEND_TRACE("MC: Unable to free message",
//                        RV_TRACE_LEVEL_ERROR);
//        return RVM_MEMORY_ERR;
//    }
//  }
  return RVM_OK;
}

