/****************************************************************************/
/*                                                                           */
/*  File Name:  dar_api.c                                                   */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to service          */
/*            primitives.                                                   */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date               Modification                                         */
/*  ----------------------------------------------------------------------  */
/*  2 October 2001  Create                                                  */
/*                                                                          */
/*  Author    - Stephanie Gerthoux                                          */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

  
#ifdef RVM_DAR_SWE
   #ifndef _WINDOWS
      #include "l1sw.cfg"
      #include "swconfig.cfg"
      #include "sys.cfg"
      #include "chipset.cfg"
   #endif
#if (CHIPSET!=15)
   #include "abb/abb.h"
#endif
   #include "rv/rv_general.h"
   #include "rvm/rvm_gen.h"
   #include "dar/dar_api.h"
   #include "dar/dar_env.h"    
   #include "dar/dar_error_hdlr_i.h"
   #include "dar/dar_messages_i.h"
   #include "dar/dar_msg_ft.h"
   #include "dar/dar_macro_i.h" 
   #include "dar/dar_diagnose_i.h"
   #include "dar/dar_const_i.h"
   #include "dar/dar_emergency.h"
   #include "rtc/rtc_api.h"

   #ifndef _WINDOWS
      /* enable the timer */
      #include "memif/mem.h"
      #include "timer/timer.h"
      #include "inth/iq.h"
      #if ((CHIPSET == 12)||(CHIPSET==15))
        #include "inth/sys_inth.h"
      #endif
   #endif


   #include "bspI2cMultiMaster_Map.h"
   #include "sys_map.h"
   #include"bspUtil_BitUtil.h"
   #include "bspI2cMultiMaster_Llif.h"
   #include "Uart.h"
   #include "swconfig.cfg"
   #include "bspTwl3029_Aux_Map.h"
   #include "abb/pg2x/bspTwl3029_Aux_Map_pg2x.h"
   #include "bspTwl3029_Int_Map.h"
   #include "bspTwl3029_Pwr_Map.h"

   #define DAR_GET_HOURTYPE( _hour_reg ) ((_hour_reg) >> 7)
   #define DAR_RTC_CONVERT_BCD_TO_DECIMAL( _bcdVal ) (( (((_bcdVal) & 0xF0) >> 4) * 10 ) + \
	                                                                                                       ( (((_bcdVal) & 0x0F) >> 0) *  1 ))


   /**** Global variable ****/
   /* Define load timer */
   static UINT16 dar_load_tim = 0x0;

   /* Increment variable */
   static BOOLEAN dar_increment = TRUE;

   /* DAR previous status */
   T_DAR_RECOVERY_STATUS   dar_previous_status;

   /* DAR previous exception*/
   UINT8    dar_previous_exception;

    /**** Define extern variables ****/
   /* Get the status of the system */
   extern T_DAR_RECOVERY_STATUS dar_current_status;

   /* dar_exception_status : to get the status of the exception */
   extern UINT8 dar_exception_status;

   /* Define the recovery buffer */
   extern UINT8 dar_recovery_buffer[DAR_RECOVERY_DATA_MAX_BUFFER_SIZE];

   /* Define a pointer to the Global Environment Control block   */
   extern T_DAR_ENV_CTRL_BLK *dar_gbl_var_p;

   /* **************************************************************************** */
   /*                                RECOVERY                                      */
   /* **************************************************************************** */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_recovery_get_status                                  */
   /*                                                                              */
   /*    Purpose:  This function is called by the MMI at the beginning of the      */
   /*              procedure, in order to get the status of the last reset of      */
   /*              the system.                                                     */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        Dar recovery status                                                   */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*         Validation of the function execution.                                */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET dar_recovery_get_status(T_DAR_RECOVERY_STATUS* status)
   {
      /* Variable to know the status of th Omega VRPC register */
      UINT16 dar_pwr_status;

      *status = dar_previous_status;
      switch (dar_previous_status)
      {
         case (DAR_WATCHDOG):
         {
      	   DAR_SEND_TRACE("Dar Entity: Status of the last reset of the system = WATCHDOG",RV_TRACE_LEVEL_DEBUG_HIGH);
            break;
         }
         case (DAR_NORMAL_SCUTTLING):
         {
      	   DAR_SEND_TRACE("Dar Entity: Status of the last reset of the system = NORMAL SCUTTLING",RV_TRACE_LEVEL_DEBUG_HIGH);
            break;
         }
         case (DAR_EMERGENCY_SCUTTLING):
         {
      	   DAR_SEND_TRACE("Dar Entity: Status of the last reset of the system = EMERGENCY SCUTTLING",RV_TRACE_LEVEL_DEBUG_HIGH);
            switch (dar_previous_exception)
            {
                case (DAR_EXCEPTION_DATA_ABORT):
                {
      	            DAR_SEND_TRACE(" A DATA ABORT exception has occured",RV_TRACE_LEVEL_DEBUG_HIGH);
                     break;
                }
                
                case (DAR_EXCEPTION_PREFETCH_ABORT):
                {
      	            DAR_SEND_TRACE("          A PREFETCH ABORT exception has occured",RV_TRACE_LEVEL_DEBUG_HIGH);
                     break;
                }
                
                case (DAR_EXCEPTION_UNDEFINED):
                {
      	            DAR_SEND_TRACE("          A UNDEFINED INSTRUCTION exception has occured",RV_TRACE_LEVEL_DEBUG_HIGH);
                     break;
                }
                
                case (DAR_EXCEPTION_SWI):
                {
      	            DAR_SEND_TRACE("          A SWI exception has occured",RV_TRACE_LEVEL_DEBUG_HIGH);
                     break;
                }
                
                case (DAR_EXCEPTION_RESERVED):
                {
      	            DAR_SEND_TRACE("          A RESERVED exception has occured",RV_TRACE_LEVEL_DEBUG_HIGH);
                     break;
                }

                default:
               {
       	         DAR_SEND_TRACE("          An error has been detected",RV_TRACE_LEVEL_DEBUG_HIGH);
                  break;
               }

            }

            break;
         }
         default:
         {
		 	#if (ANLG_FAM == 11)
			   /* FIXME Port TRITON Here */
			
            #elif (_GSM == 1) || (defined _WINDOWS)
			   dar_pwr_status = ABB_Read_Status();
			   #if ((ANLG_FAM == 1) || (ANLG_FAM == 2))
               if (dar_pwr_status & ONBSTS)
               #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
               if (dar_pwr_status & PWONBSTS)
               #endif
               {			    
	               /* Switch on Condition on ON BUTTON Push */
          	      DAR_SEND_TRACE("Dar Entity: Status of the last reset of the system = POWER ON/OFF",RV_TRACE_LEVEL_DEBUG_HIGH);
                  break;
               }
               else
               {
                  /* Branch to a reset at adress 0 */
          	      DAR_SEND_TRACE("Dar Entity: Status of the last reset of the system = BRANCH to adress 0",RV_TRACE_LEVEL_DEBUG_HIGH);
                  break;
               }
            #else
               /* the SPI is not available in BOARD_TEST configuration */
        	      DAR_SEND_TRACE("Dar Entity: Status of the last reset of the system = POWER ON/OFF or BRANCH to adress 0",RV_TRACE_LEVEL_DEBUG_HIGH);
            #endif
         }
      }/* switch */

      return(RV_OK);	

   } /* dar_recovery_get_status */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_recovery_config                                      */
   /*                                                                              */
   /*    Purpose:  This function is used to store a callback function that will be */
   /*              called by the recovery system when a recovery procedure has     */
   /*              been initiated                                                  */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        dar callback function                                                 */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET dar_recovery_config(T_RV_RET (*dar_store_recovery_data)
                                                      (T_DAR_BUFFER      buffer_p,
                                                      UINT16            length))
   {   
      /* call the callback function */
      dar_gbl_var_p->entity_dar_callback = dar_store_recovery_data; 

      return(RV_OK);
   } /* dar_recovery_config */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_get_recovery_data                                    */
   /*                                                                              */
   /*    Purpose:  This function is used to retrieve data that have been stored    */
   /*              in the buffer just before a reset.                              */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*       - the buffer in whom important data have been stored before the reset  */
   /*       - the length of the buffer                                             */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET dar_get_recovery_data( T_DAR_BUFFER buffer_p, UINT16   length )                                                                               
   { 
      /* Local variables */
      UINT8 i; 

	   if (buffer_p != NULL)
      {
         if(length <= DAR_RECOVERY_DATA_MAX_BUFFER_SIZE)
         {
            /* Retrieve data that have been stored in the global buffer */
            for(i=0;i<length;i++)
            {
               buffer_p[i] = dar_recovery_buffer[i];
            }
         }

         else
         /* the lenth is longer than DAR_RECOVERY_DATA_MAX_BUFFER_SIZE */
         {
            /* So retrieve "DAR_RECOVERY_DATA_MAX_BUFFER_SIZE" data that have been stored */
            /* in the global buffer, the other data (length -"DAR_RECOVERY_DATA_MAX_BUFFER_SIZE") */
            /* are lost*/
            for(i=0;i<DAR_RECOVERY_DATA_MAX_BUFFER_SIZE;i++)
            {
                buffer_p[i] = dar_recovery_buffer[i];
            }

            DAR_SEND_TRACE_PARAM("Bytes nb that haven't be saved due to not enough memory space ",(DAR_RECOVERY_DATA_MAX_BUFFER_SIZE-length),DAR_WARNING);
         }
      } /* if (buffer_p != NULL) */
    
      return(RV_OK);

   } /* dar_get_recovery_data */


   /* **************************************************************************** */
   /*                                  WATCHDOG                                    */
   /* **************************************************************************** */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_start_watchdog_timer                                 */
   /*                                                                              */
   /*    Purpose:  This function uses the timer as a general purpose timer instead */
   /*              of Watchdog. It loads the timer , starts it and then unmasks    */
   /*              the interrupt.                                                  */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        time's interval in milliseconds before the timer expires              */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET   dar_start_watchdog_timer(UINT16 timer_expiration_value)
   {
      DAR_SEND_TRACE("DAR Watchdog timer",RV_TRACE_LEVEL_DEBUG_LOW);

      /* use Watchdog timer set as a general purpose timer                     */
      /* Calculate the load value thanks to the formula:                       */
      /*      timer_expiration_value * 1000 = Tclk * (DAR_LOAD_TIM+1)*2^(PTV+1)*/
      /*      (in Watchdog mode, the value of PTV is fixed to 7)               */
      /* CHIPSET 15: Clock to WDOG timer is 13 MHz/4 = 3.25 MHz		       */
      /* Tclk = 1/3.25 microsecond = .3077 microsecond			       */
      dar_load_tim =(UINT16)(((timer_expiration_value * 1000)/(0.3077*256))-1);

      #ifndef _WINDOWS
       /* Load "dar_load_tim" value */
       TIMER_WriteValue(dar_load_tim);

       /* Start timer with PTV = 7, no autoreload, free = 0 */
       *(volatile UINT16*)TIMER_CNTL_REG = 0x0E80;

       /* Unmask IRQ0 */
       #if ((CHIPSET == 12)||(CHIPSET==15))
         F_INTH_ENABLE_ONE_IT(C_INTH_WATCHDOG_IT);
       #else
         IQ_Unmask(IQ_WATCHDOG);
       #endif
      #endif
   
      return(RV_OK);

   } /* dar_start_watchdog_timer */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_reload_watchdog_timer                                */
   /*                                                                              */
   /*    Purpose:  This function is used to maintain the timer in reloading it     */
   /*              periodically before it expires                                  */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        None                                                                  */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET   dar_reload_watchdog_timer()
   {
      #ifndef _WINDOWS

       DAR_SEND_TRACE("Reload Watchdog ",RV_TRACE_LEVEL_DEBUG_LOW);

       /* Stop the timer */
       *(volatile UINT16*)TIMER_CNTL_REG = 0x0E00;

       /* Reload the timer with a different value each time */
       if (dar_increment == TRUE)
       {
            TIMER_WriteValue(++dar_load_tim);
            dar_increment = FALSE;
       }
       else
       {
            TIMER_WriteValue(--dar_load_tim);
            dar_increment = TRUE;
       }

       /* Restart timer with PTV = 7, no autoreload, free = 0 */
       *(volatile UINT16*)TIMER_CNTL_REG = 0x0E80;
      #endif

      return(RV_OK);

   } /* dar_reload_watchdog_timer */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_stop_watchdog_timer                                  */
   /*                                                                              */
   /*    Purpose:  This function stops the timer used as a general purpose timer   */
   /*              instead of watchdog                                             */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        None                                                                  */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET   dar_stop_watchdog_timer()
   {
      #ifndef _WINDOWS
       /* Stop the timer */
       *(volatile UINT16*)TIMER_CNTL_REG = 0x0E00;

       /* Mask IRQ0 */
       #if ((CHIPSET == 12)||(CHIPSET==15))
         F_INTH_DISABLE_ONE_IT(C_INTH_WATCHDOG_IT);
       #else
         IQ_Mask(IQ_WATCHDOG);
       #endif
      #endif

      return(RV_OK);

   } /* dar_stop_watchdog_timer */


   /* **************************************************************************** */
   /*                                  RESET                                       */
   /* **************************************************************************** */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_reset_system                                         */
   /*                                                                              */
   /*    Purpose:  This function can be used to reset the system voluntarily       */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        None                                                                  */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.                                 */  
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None.                                                                 */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET   dar_reset_system(void)
   {
      T_RV_RET returnVal = RV_OK;
      /* Update the DAR recovery status */
      dar_current_status = DAR_NORMAL_SCUTTLING;

      /* Call the MMI callback function to save some parameters before reset */
      returnVal=dar_gbl_var_p->entity_dar_callback(dar_recovery_buffer,DAR_RECOVERY_DATA_MAX_BUFFER_SIZE);

      /* Send a trace before the reset of the system */
      DAR_SEND_TRACE("Voluntary reset of the system",RV_TRACE_LEVEL_DEBUG_HIGH);

      dar_reset();

      return(returnVal); 
   }

   /* **************************************************************************** */
   /*                                DIAGNOSE                                      */
   /* **************************************************************************** */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_diagnose_swe_filter  		    			                   */
   /*                                                                              */
   /*    Purpose: This function is called to configure the Diagnose filtering.     */
   /*             It allows to determine what Software Entity ( dar_use_id )       */
   /*             wants to use the Diagnose and allows to indicate the level       */
   /*             threshold of the diagnose messages. (Warning or Debug)           */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*         - the dar use id                                                     */
   /*         - the dar level                                                      */
   /*											                                                         */
   /*    Output Parameters:                                                        */
   /*        Validation of the function execution.								                 */
   /*                                                                              */
   /*                                                                              */
   /*    Note:                                                                     */
   /*        None                                                                  */
   /*                                                                              */
   /********************************************************************************/

   T_RV_RET dar_diagnose_swe_filter ( T_RVM_USE_ID  dar_use_id, 
                                     T_DAR_LEVEL   dar_level)
   {
      /* Declare local variables */
      T_RVF_MB_STATUS       mb_status   = RVF_GREEN;
      T_DAR_FILTER_START   *use_id_p = NULL;

      /************************** dar_diagnose_swe_filter function   ***************/

    if (dar_gbl_var_p == NULL )   {
         dar_error_trace(DAR_ENTITY_NOT_START);
         return(RV_NOT_READY);
      }
       
      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (dar_gbl_var_p->mb_dar,
                               sizeof (T_DAR_FILTER_START),
                               (T_RVF_BUFFER **) (&use_id_p));

      /* If insufficient resources, then report a memory error and abort.          */
    if (mb_status == RVF_YELLOW)   {
         /* deallocate the memory */
         rvf_free_buf((T_RVF_BUFFER *)use_id_p);
         dar_error_trace(DAR_ENTITY_NO_MEMORY);
         return (RV_NOT_SUPPORTED);
      }
      else
        if (mb_status == RVF_RED)   {
         dar_error_trace(DAR_ENTITY_NO_MEMORY);
         return (RV_MEMORY_ERR);
      }

      /* fill the message id       */
      use_id_p ->os_hdr.msg_id = DAR_FILTER_REQ;

      /* fill the addr source id */
      use_id_p->os_hdr.src_addr_id = dar_gbl_var_p->addrId;

      /* fill the message parameters (group, mask and level) */
      use_id_p->use_msg_parameter.group_nb = (dar_use_id>>16)& 0x7FFF;
      use_id_p->use_msg_parameter.mask     = (dar_use_id)&0xFFFF;
      use_id_p->use_msg_parameter.level    = dar_level;

      /* send the messsage to the DAR entity */
      rvf_send_msg (dar_gbl_var_p->addrId, 
                    use_id_p);

      return (RV_OK);

	 
   } /* dar_diagnose_swe_filter */

   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_diagnose_write                                       */
   /*                                                                              */
   /*    Purpose:  This function is called to store diagnose data in RAM buffer    */
   /*                                                                              */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        Pointer to the message to store                                       */
   /*        Data Format, ( the Binary format is not supported)                    */
   /*        Data level,                                                           */
   /*        Data Use Id,                                                          */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*         Validation of the diagnose execution.                                */
   /*                                                                              */
   /*                                                                              */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET dar_diagnose_write(  T_DAR_INFO    *buffer_p,
                                 T_DAR_FORMAT  format,
                                 T_DAR_LEVEL   diagnose_info_level,
                                 T_RVM_USE_ID  dar_use_id)
   {
      /* Declare local variables */
      UINT8 index =0 ; /* local index */

      /* Check if the dar_use_id group_nb exists */
      /* If the group exists... */
      if(dar_search_group((dar_use_id>>16)& 0x7FFF,&index) == RV_OK)
      {
         /* Check the Dar level   */
         switch(diagnose_info_level) 
         {
            case DAR_WARNING:
            { 
               /* The DAR entity wants to process Warning messages */
               /* check if the mask_warning is in the dar_filter array */
               if (((dar_gbl_var_p ->dar_filter_array[index].mask_warning) 
                      & ((dar_use_id)&0xFFFF)) !=0)
               {
                  /* The Warning messages must be diagnosed */
                  dar_send_write_data( buffer_p, format, diagnose_info_level, dar_use_id);
               }

               else
               { 
                  /* There is no mask_warning for this use_id in the dar_filter array   */
                  /* The warning messages can't be diagnosed                            */
                  DAR_TRACE_WARNING("The Warning messages can't be diagnosed");
               }

               break;
            } /* case DAR_WARNING */

            case DAR_DEBUG:
            {
               /* The DAR entity wants to process Debug messages       */
               /* Check if the mask_debug is in the dar_filter array */

               if (((dar_gbl_var_p ->dar_filter_array[index].mask_debug) 
                      & ((dar_use_id)&0xFFFF)) !=0) 
               {
                  /* The Debug messages must be diagnosed */
                  dar_send_write_data( buffer_p, format, diagnose_info_level, dar_use_id);
               }
            
               else
               { 
                  /* There is no mask_debug for this use_id in the dar_filter array    */
                  /* The debug messages can't be diagnosed                             */
                  DAR_TRACE_WARNING("The Debug messages can't be diagnosed");
               }
               break;
            } /* case DAR_DEBUG */

            default:
            {
               /* Unknow level has been received */
               DAR_TRACE_WARNING("A DAR unknow level has been received ");
               break; 
            }
         } /* switch(msg_p->use_msg_parameter.level) */
      } /* if (search_group(dar_use_id.group_nb,&index)== RV_OK) */
      
      else
      {
        /* An unknow group message has been received */
        DAR_TRACE_WARNING("A DAR unknow group level message has been received ");
      }
      
      return (RV_OK);
   } /* dar_diagnose_write */


   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_diagnose_generate_emergency                          */
   /*                                                                              */
   /*    Purpose:  This function is called to store diagnose data in RAM buffer    */
   /*              when an emergency has been detected and goes to emergency       */
   /*              (automatic reset)                                               */
   /*                                                                              */
   /*                                                                              */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        Pointer to the message to store                                       */
   /*        Data Format, ( the Binary format is not supported)                    */
   /*        Data Use Id,                                                          */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*         Validation of the diagnose execution.                                */
   /*                                                                              */
   /*                                                                              */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET dar_diagnose_generate_emergency(  T_DAR_INFO    *buffer_p,
                                              T_DAR_FORMAT  format,
                                              T_RVM_USE_ID  dar_use_id)
   {
      if (dar_gbl_var_p != NULL)
      {
         /* Process the diagnose emergency */
         dar_process_emergency(buffer_p, format, dar_use_id, DAR_EMERGENCY_RESET|DAR_NEW_ENTRY);
           
         return (RV_OK);
      }
      else
      {
         return (RV_NOT_READY);
      }

   } /* dar_diagnose_write */

    	   
   /********************************************************************************/
   /*                                                                              */
   /*    Function Name:   dar_diagnose_write_emergency                             */
   /*                                                                              */
   /*    Purpose:  This function is called to store diagnose data in RAM buffer    */
   /*              when an emergency has been detected. Data is written directly   */
   /*              compared to dar_diagnode_write where data is sent to DAR        */
   /*              via messages. Depending on the passed flags a RESET will be     */                                              
   /*              done.                                                           */
   /*                                                                              */
   /*                                                                              */
   /*    Input Parameters:                                                         */
   /*        Pointer to the message to store                                       */
   /*        Data Format, ( the Binary format is not supported)                    */
   /*        Data Use Id,                                                          */
   /*        Flags                                                                 */
   /*                                                                              */
   /*    Output Parameters:                                                        */
   /*         Validation of the diagnose execution.                                */
   /*                                                                              */
   /*                                                                              */
   /*                                                                              */
   /*    Revision History:                                                         */
   /*        None.                                                                 */
   /*                                                                              */
   /********************************************************************************/
   T_RV_RET dar_diagnose_write_emergency(  T_DAR_INFO    *buffer_p,
                                           T_DAR_FORMAT  format,
                                           T_RVM_USE_ID  dar_use_id,
                                           UINT32 flags)
   {
      if (dar_gbl_var_p != NULL)
      {
         /* Process the diagnose emergency */
         dar_process_emergency(buffer_p, format, dar_use_id, flags);
           
         return (RV_OK);
      }
      else
      {
         return (RV_NOT_READY);
      }

   } /* dar_diagnose_write */
   
void Config_I2c(void)
{


    /*REset I2C*/

     


    /*Disable I2c*/
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x00;


     /*DISABLE ALL INTERRUPTS*/

*(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_IE_OFFSET)=0x0000;



    /*Set Own Address*/

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_OA_OFFSET)=23;

    /* Setup the clock data that goes into PSC, SCLL and SCLH regs */

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_PSC_OFFSET)=0;
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+ BSP_I2C_MULTI_MASTER_MAP_SCLL_OFFSET) =9 ;
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_SCLH_OFFSET)=9;

    /* Set the Slave Address */

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_SA_OFFSET)=0x2d;

/*Enable I2c*/
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x8000;




}


T_RV_RET i2c_write(UINT8 Reg_Address,UINT8 Data)
{




    /*Configure for master device and write mode*/


    /*Is bus free*/

    while ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_MASK) 
                !=0))   {


    }

    /*Write I2c con 8603*/
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CNT_OFFSET )=0x02;

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x8603;


    /*Wait for NAck or Xrdy or AL*/
    while(((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)))==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)
            ||((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)){


    }

    /*if NACK or AL occurs return*/
    if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
            BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)   {

        return RV_INTERNAL_ERR;

    }

    /*Xrdy should be set*/
    while(BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)
            ==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)   {


    }

    /*Write to fifo*/
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_DATA_OFFSET)=(Reg_Address+(Data<<8));

    /*Clear XRDY*/
    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
            BSP_I2C_XRDY_CLEAR_VALUE);


    /*Wait for ARDY*/
    while ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK)) == BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE);  

    /*CLEAR the ARDY bit*/
  BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK);
    /*Disable Device*/
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x0000;
    return RV_OK;
}






INT8 i2c_read(UINT8 Reg_Address, UINT8* Data)
{       
    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CNT_OFFSET)=0x0001; 



    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x8601;

    while ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_MASK))!=0 
          )   {


        /* Polled Mode: Busy waiting for bus to get free */


    }

    while ((((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)))==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)
                ||((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)))   {


    }

    if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
            BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)   {
        return RV_INTERNAL_ERR;
    }


    /* XRDY should be setby now */

    while((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    (BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)))==
            BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE);

    /* Set the subaddress byte(s) into the FIFO */

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_DATA_OFFSET)=0x00ff&Reg_Address;

    /* Clear XRDY status */

    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK);


    /* Wait for Write, no Stop to complete ,ARDY to be set */

    while (BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK)
            == BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE);

    /* Clear the ARDY bit */

    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK);


    /*Configure in Master mode to read one byte*/

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CNT_OFFSET)=0x0001; 


    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x8403;







    while(((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_MASK)))==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)
            ||((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL))   {


    }

    /*If NACK or AL occured returned Return ERROR */

    if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
            BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)   {

        return RV_INTERNAL_ERR;
    }




    /*RRDY should be set by now*/

    while (BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_MASK)== BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_FALSE);

    *Data = *((volatile UINT8 *)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_DATA_OFFSET));


    /* RRDY is cleared after reading data from the Data register */


    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
            BSP_I2C_RRDY_CLEAR_VALUE);



    /* ARDY should be set by now */

    while ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK))== BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE);


    /* Clear the ARDY bit */

    BSPUTIL_BITUTIL_BIT_SINGLE_SET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(C_MAP_I2C_TRITON_BASE),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK);

    /* disable the device */

    *(volatile UINT16*)(C_MAP_I2C_TRITON_BASE+BSP_I2C_MULTI_MASTER_MAP_CON_OFFSET)=0x0000;
    return 0;
}



#ifndef DAR_CARKIT_DEBUG
#define DAR_CARKIT_DEBUG 0
#endif
#if (DAR_CARKIT_DEBUG==1)   

   #include "Pin_config.h"

#define DAR_DUMP_CON_TEST 0

#define MIN_IRAM_ADDR 0x08000000
#define MAX_IRAM_ADDR 0x0804FFFF
#define MIN_DRAM_ADDR 0x00400000
#define MAX_DRAM_ADDR 0x005FFFFF

const char boardinfo[]=" I-Sample 2.6. IRAM Start=08000000 Len=00050000 DRAM Start=00400000 Len=00200000";
const char resetKey[]="DOOM";
const char errorStr[] = "Error in entry";

/*Carriage Return*/
const char CR[2]={0xD,0xA};

SYS_UWORD32 DAR_WriteNChars (T_tr_UartId uart_id,
                            char *buffer,
                            SYS_UWORD32 chars_to_write)

{
    SYS_UWORD32 written=0;

    while(written!=chars_to_write)
    {
        SYS_UWORD32 temp=0;
        temp = UA_WriteNChars(uart_id,&buffer[written],(chars_to_write-written));
        written += temp;
    }
    return written;

}
void dump_Memory(int add,int len)
{    
    int dummy=0; 
    int temp=0;
    char tempChar[15]={0};		
     int *addPtr=(int *)add;
	 
	if( (add >= MIN_IRAM_ADDR && add <= MAX_IRAM_ADDR))   {
         if(len > ((MAX_IRAM_ADDR-add)/4))   {
            DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
            DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
            return;
        }
    }
    else if( add >= MIN_DRAM_ADDR || add > MAX_DRAM_ADDR )   {
       if(len > ((MAX_DRAM_ADDR-add)/4))   {
            DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
            DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
            return;
        }
    }
    else
    {
        DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
        DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
        return;
    }
 	sprintf(tempChar,"\n\r%08x, ",add);
	DAR_WriteNChars(UA_UART_0, tempChar, strlen(tempChar));
	sprintf(tempChar,"%08x,\n\r",len);
	DAR_WriteNChars(UA_UART_0, tempChar, strlen(tempChar));
    while(len-->0)   {
		sprintf(tempChar,"0x%08x\n\r",*addPtr);
		DAR_WriteNChars(UA_UART_0, tempChar, strlen(tempChar));
        addPtr++;
    }
}    
void dar_dump_reset( char *key)
{

    volatile UINT16 *register_p;
    volatile UINT8   i;




    if(memcmp(resetKey,key,sizeof(resetKey)))   {
         DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
        DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
        return;
    }

    i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0x00);
    i2c_write(BSP_TWL3029_MAP_AUX_REG_TOGGLE2_OFFSET,0x15);
    i2c_write(BSP_TWL3029_MAP_WLED_CTRL_LEDA_OFFSET,0x0);
    i2c_write(BSP_TWL3029_MAP_WLED_CTRL_LEDB_OFFSET,0x0);
    i2c_write(BSP_TWL3029_MAP_WLED_CTRL_LEDC_OFFSET,0x0);



    /* enable the Watchdog timer */
    TM_EnableWatchdog();

    /* Reset the system with the Watchdog */
    /* initialize the adress of the watchdog timer pointer */
    register_p = (volatile UINT16 *)WATCHDOG_TIM_MODE;

    /* Write the 0xF5 value to the Watchdog timer mode register to disable the Watchdog*/
    /* Note the bit 15 must be unchanged ( bit 15 = 1 -> 0x8000)*/
    *register_p =0x80F5;

    /* Wait a couple of time to be sure that this register has a new value */
    for (i=0;i<100;i++);

    /* After having received 0xF5 in the Watchdog timer mode register, if the */
    /* second write access is differennt from 0xA0, ARM core is reset         */
    /*   The ARM HW core is reset + branch to adress 0x0000 ( SW reset)       */
    *register_p=0x80F5;

    /* Wait until the ARM reset */
}

/*TRITON CONFIG*/

T_RV_RET Config_Triton(void)
{

    T_RV_RET retval=RV_OK;
    /*mw -1 0x00712e50 00 83 40 00 01
     *  "BCI_DISABLE"
     */
    retval=i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0);
    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL3029_MAP_BCI_BCICTL3_OFFSET,0x40);
    } 

    /*mw -1 0x00712e50 01 19 9E 00 01
     * echo "CFG_PU_PD_LSB [W]"
     */

    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0x01);
    }	
    if(retval!=RV_INTERNAL_ERR)   { 
        retval=i2c_write(BSP_TWL3029_MAP_USB_RESERVED_USB_19H_OFFSET,0x9E);
    }


    /*mw -1 0x00712e50 01 5d a0 00 01
     * echo"VRUSB_CFG_STS"
     */
    if(retval!=RV_INTERNAL_ERR)   { 
        retval=i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0x01);
    }
    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL_3029_MAP_PMC_SLAVE_VRUSB_CFG_STS_OFFSET,0xa0);
    }



    /*mw -1 0x00712e50 00 07 0c 00 01*/

    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0);
    } 
    if(retval!=RV_INTERNAL_ERR)   { 
        retval=i2c_write(BSP_TWL3029_MAP_USB_CTRL_2_CLR_OFFSET,0x0c);
    }

    /*
     *  "pin mux"
     */

    /**(UINT16 *)0xfffef10c=0x0001;*/
    CONF_USB_DAT=MUX_CFG(1,0);


    /**(UINT16 *)0xfffef10a=0x0001;*/
    CONF_USB_SE0=MUX_CFG(1,0);

   /*DE-Couple the actual UART Pin fron Uart RX port, THIS CONFIGURATION IS NOT MENTIONED IN THE TRM*/
   #if (RX_THRU_UART==0) 
       CONF_UART_RX=0x0018;
   #endif

    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0);
    }
    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL3029_MAP_USB_CTRL_1_CLR_OFFSET,0x02);
    }

    /*mw -1 0x00712e50 00 04 40 00 01
     * echo "UART_EN"
     */
    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0);
    }
    if(retval!=RV_INTERNAL_ERR)   {
        retval=i2c_write(BSP_TWL3029_MAP_USB_CTRL_1_SET_OFFSET,0x40);
    }

    i2c_write(BSP_TWL_3029_MAP_PAGE0_PAGEREG0_OFFSET,0x00);
    i2c_write(BSP_TWL3029_MAP_AUX_REG_TOGGLE2_OFFSET,0x2A);
    i2c_write(BSP_TWL3029_MAP_WLED_CTRL_LEDA_OFFSET,0x11);
    i2c_write(BSP_TWL3029_MAP_WLED_CTRL_LEDB_OFFSET,0x11);
    i2c_write(BSP_TWL3029_MAP_WLED_CTRL_LEDC_OFFSET,0x11);
    return retval;

}

/*Uart Config*/


void dar_dump_core(void);





void dar_dump_init(void)
{
    INT8 retval=0;
    /* Init Triton */
    Config_I2c();
    Config_Triton();



    /* Do Pin Muxing */
    UA_Init(UA_UART_0, TR_BAUD_115200,NULL);

    dar_dump_core();

    return;

}



void dar_dump_core(void)
{
    char uart_data=0;
    int  add=0;
    int  len=0;
    int  count=0;
    char key[sizeof(resetKey)];
    T_PARSE_STATE state=PARSE_INIT;

#if (DAR_DUMP_CON_TEST==1)
    char uart_wr_data='a';
    char uart_rd_data;
#endif

    while(1)   {
#if (DAR_DUMP_CON_TEST==1)
       DAR_WriteNChars(UA_UART_0, &uart_wr_data, 1);
       while(UA_ReadNChars(UA_UART_0,&uart_wr_data, 1)!=1);
       uart_wr_data++;
#endif

#if (DAR_DUMP_CON_TEST==0)
        while(UA_ReadNChars(UA_UART_0,&uart_data, 1)!=1);
        /* This Write for echo back */
      DAR_WriteNChars(UA_UART_0, &uart_data, 1);
        switch(state)   {
            case PARSE_INIT:
                if(uart_data =='t' || uart_data== 'T')   {
                    state = PARSE_KEY;
                    memset(key, 0, sizeof(key));
                    count=0;
                }
                else if(uart_data == 'i' || uart_data == 'I')   {
                     DAR_WriteNChars(UA_UART_0, (char *)boardinfo, sizeof(boardinfo));
                    DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
                }
                else if(uart_data == 'r' || uart_data =='R')   {
                    state = PARSE_ADD;
                    add=0;
                    count=0;
                }
                break;
            case PARSE_ADD:
                if(uart_data ==' ')   {
                    if(count)   {
                        state = PARSE_LEN;
                        count=0;
                        len=0;
                    }
                    continue;
                }
                else if((uart_data >= '0' && uart_data <='9'))   {
                    uart_data -= '0';
                    add = add<<4|(int)uart_data;
                    count++;
                }
                else if (uart_data >= 'A' && uart_data <= 'F' )   {
                    uart_data -= 'A';
                    uart_data += 10;
                    add = add<<4|(int)uart_data;
                    count++;					
                }
                else if(uart_data >= 'a' && uart_data <= 'f' )   {
                    uart_data -= 'a';
                    uart_data += 10;
                    add = add<<4|(int)uart_data;
                    count++;					
                }
                else
                {
                    state = PARSE_INIT;
                     DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
                    DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
                    continue;
                }
                if(count==8)   {
                    state=PARSE_LEN;
                    count=0;
                    len=0;
                }
                break;
            case PARSE_LEN:
                if((uart_data == ' ')||(uart_data==0x0D))   {
                    if(count)   {
                        state = PARSE_INIT;
                        dump_Memory(add,len);
                        count=0;
                    }
                    continue;
                }
                else if((uart_data >= '0' && uart_data <='9'))   {
                    uart_data -= '0';
                    len = len<<4|(int)uart_data;
                    count++;
                }
                else if (uart_data >= 'A' && uart_data <= 'F' )   {
                    uart_data -= 'A';
                    uart_data += 10;
                    len = len<<4|(int)uart_data;
                    count++;					
                }
                else if(uart_data >= 'a' && uart_data <= 'f' )   {
                    uart_data -= 'a';
                    uart_data += 10;
                    len = len<<4|(int)uart_data;
                    count++;					
                }
                else  {
                    state = PARSE_INIT;
                   DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
                    DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
                    continue;
                }
                if(count==8)   {
                    state=PARSE_INIT;
                    dump_Memory(add,len);
                    count=0;
                    len=0;
                }
                break;
            case PARSE_KEY:
                if((uart_data == ' ')||(uart_data==0x0D))   {
                    if(count)   {
                        state = PARSE_INIT;
                        dar_dump_reset(key);
                        count=0;
                    }
                    continue;
                } 
				else {
                    key[count++] = uart_data;
                }
                if(((unsigned int)count)>(sizeof(key)+1))
                {
                    state = PARSE_INIT;
                    DAR_WriteNChars(UA_UART_0, (char *)errorStr, sizeof(errorStr));
                    DAR_WriteNChars(UA_UART_0,(char *)CR, 2);
                    continue;
                }
                break;             
            default:
                break;
                //Shouldn't Reach Here
        }
#endif
    }
}
#endif

dar_get_date_time(T_RTC_DATE_TIME *date_time)
{
    UINT8 i=0, hour=0;
    UINT8 * timeArrayPtr=(UINT8*)date_time;
	
    Config_I2c();
	
    /*Change page to PAGE 1*/ 
    i2c_write(0xFF,0x01);
	
    for ( i = 0 ; i < 7; i++)
    {
        i2c_read((i+DAR_MAP_RTC_SECOND_REG_OFFSET), timeArrayPtr);
        timeArrayPtr++;
    }
	
    hour = (0x7f & (date_time->hour));
    
    date_time->hour = DAR_GET_HOURTYPE(date_time->hour);
    date_time->second  = DAR_RTC_CONVERT_BCD_TO_DECIMAL( date_time->second );
    date_time->minute  = DAR_RTC_CONVERT_BCD_TO_DECIMAL( date_time->minute );
    date_time->hour    = DAR_RTC_CONVERT_BCD_TO_DECIMAL( hour);
    date_time->day     = DAR_RTC_CONVERT_BCD_TO_DECIMAL( date_time->day  );
    date_time->month   = DAR_RTC_CONVERT_BCD_TO_DECIMAL( date_time->month );
    date_time->year    = DAR_RTC_CONVERT_BCD_TO_DECIMAL( date_time->year );
    date_time->wday = DAR_RTC_CONVERT_BCD_TO_DECIMAL( date_time->wday );

    BspTwl3029_I2c_shadowRegRead(0x10 ,&i);  

    if ( i & DAR_RTC_MODE_12_24){
        date_time->mode_12_hour=DAR_RTC_12_HOURMODE;
    }
    else{
        date_time->mode_12_hour=DAR_RTC_24_HOURMODE;
    }
}   

#else


   /* ************************************************ */
   /*            THE DAR ENTITY IS DISABLED            */
   /* ************************************************ */

   #include "rv/rv_general.h"
   #include "rvm/rvm_gen.h"
   #include "rvm/rvm_priorities.h"
   #include "rvf/rvf_target.h"
   #include "rvf/rvf_i.h"
   #include "dar/dar_gen.h"

   T_RV_RET dar_diagnose_swe_filter ( T_RVM_USE_ID  dar_use_id, 
                                      T_DAR_LEVEL   dar_level)
   {
         return (RV_OK);

   } /* dar_diagnose_swe_filter */
   

   T_RV_RET dar_diagnose_write(  T_DAR_INFO    *buffer_p,
                                 T_DAR_FORMAT  format,
                                 T_DAR_LEVEL   diagnose_info_level,
                                 T_RVM_USE_ID  dar_use_id)
   {
         return (RV_OK);

   } /* dar_diagnose_write */

        
   
   T_RV_RET dar_diagnose_generate_emergency(  T_DAR_INFO    *buffer_p,
                                              T_DAR_FORMAT  format,
                                              T_RVM_USE_ID  dar_use_id)
   {

         return (RV_OK);

   } /* dar_diagnose_generate_emergency */

   T_RV_RET dar_diagnose_write_emergency(  T_DAR_INFO    *buffer_p,
                                           T_DAR_FORMAT  format,
                                           T_RVM_USE_ID  dar_use_id,
                                           UINT32 flags)
   {
         return (RV_OK);
   } /* dar_diagnose_write_emergency */



    	   

#endif /* #ifdef RVM_DAR_SWE */
