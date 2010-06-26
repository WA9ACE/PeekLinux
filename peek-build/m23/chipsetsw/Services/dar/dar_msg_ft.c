    /*
    ==============================================================================
    * Texas Instruments OMAP(TM) Platform Software
    * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
    *
    * Use of this software is controlled by the terms and conditions found
    * in the license agreement under which this software has been supplied.
    * ==============================================================================
    */
    /*
    * @file dar_msg_ft.c 
    *
    *  This function is called when the DAR entity receives a new message in its mailbox 
    *  and wants to process the message.
    *
    * Version    0.1  
    *
    */
    /* ----------------------------------------------------------------------------
    *!
    *! Revision History
    *! ===================================
    *! 01-Aug-2006 mf: Enhancement as per OMAPS00076834
    *! Author Nimmi M
    *!
    *! 17-Oct-2001 mf: Create
    *! Author Stephanie Gerthoux
    *!
    * =============================================================================
    */
    
    #include "rv/rv_defined_swe.h"
    #ifdef RVM_DAR_SWE

	/* includes */
       #ifndef _WINDOWS
          #include "swconfig.cfg"
          #include "sys.cfg"
          #include "chipset.cfg"
       #endif
       #include <string.h>
       #include "nucleus.h"
       #include "ffs/pcm.h"
       #include "rtc/rtc_api.h"
       #include "rvm/rvm_gen.h"
       #include "dar/dar_api.h"
       #include "dar/dar_macro_i.h"
       #include "dar/dar_messages_i.h"
       #include "dar/dar_const_i.h"
       #include "dar/dar_diagnose_i.h"
       #include "dar/dar_error_hdlr_i.h"
       #ifndef _WINDOWS
          #include "timer/timer.h"
       #endif
    
       /**** Global variable ****/
       /* index used in the circular buffer*/
       UINT16 dar_current_index = 0;
       
    
       /* Write buffer*/
       extern char dar_write_buffer[DAR_MAX_BUFFER_SIZE];
    
       /* Get the dar_current status */
       extern T_DAR_RECOVERY_STATUS dar_current_status;
    
       /* Define a pointer to the Global Environment Control block  */
       extern T_DAR_ENV_CTRL_BLK *dar_gbl_var_p;
    
       /* Define the recovery buffer */
       extern UINT8 dar_recovery_buffer[DAR_RECOVERY_DATA_MAX_BUFFER_SIZE];
    
       /* Crash date and time */
       extern T_RTC_DATE_TIME date_time;   
    
       /* Software version number */
       extern char sw_version[SW_VERSION_SIZE];
       extern UBYTE pcm_default_values[];				/* values for the permanent memory configuration driver */
       extern T_PCM_DESCRIPTION pcm_table[];
    
       /* current task info pointer */
       extern NU_TASK * current_task_ptr;
    
       #if (LOCOSTO_LITE==0)	  
       
          /* Task control block information */
          extern UINT8 task_count;
          extern char task_cb_array[MAX_TASK_COUNT][TASK_CB_SIZE];
          
          /* HISR control block information */
          extern UINT8 hisr_count;
          extern char hisr_cb_array[MAX_HISR_COUNT][HISR_CB_SIZE];
       
          /* Semaphore control block information */
          extern UINT8 sem_count;
          extern char sem_cb_array[MAX_SEM_COUNT][SEM_CB_SIZE];
    
          /* Queue information */
          extern UINT8 queue_count;
          extern char queue_cb_array[MAX_QUEUE_COUNT][QUEUE_CB_SIZE];
          
       #endif
       
       /**** Extern functions ****/
       extern  void * dar_read_mbox (UINT8 mbox);
    
       extern void exception(void);
    
       /* =========================================================== */
       /* Function         dar_filter_request                                          
                                                                                     
            Description      This function checks if the use_id group_nb exists:         
                               - if the group_nb exists, it adds the warning and debug 
                                 masks in the dar_array_filter                         
                               - otherwise, this function add the new group_nb and the 
                                 masks in the dar_array_filter                        
       */                                                                              
       /* =========================================================== */
    
       T_RV_RET dar_filter_request (T_DAR_FILTER_START *msg_p)
       {
          /* Declare local variables*/
          UINT8 index = 0; 
    
          /* check if the DAR entity is started */
          if (dar_gbl_var_p == NULL )
          {
             dar_error_trace(DAR_ENTITY_NOT_START);
             return(RV_NOT_READY);
          }
    
          /*** check if the group exists ****/
          /* If the group exists... */
          if(dar_search_group(msg_p->use_msg_parameter.group_nb,&index)== RV_OK)
          {
             /* Check the Dar level  */
             switch(msg_p->use_msg_parameter.level) 
             {
                case DAR_WARNING:
                {  /* The DAR entity wants to process Warning messages */
                   /* add the mask_warning in the dar_filter array */
                   dar_gbl_var_p ->dar_filter_array[index].mask_warning |= 
                                                               msg_p->use_msg_parameter.mask;
                   dar_gbl_var_p ->dar_filter_array[index].mask_debug    = 0x00;
    
                break;
                }
    
                case DAR_DEBUG:
                {
                   /* The DAR entity wants to process Debug messages                   */
                   /* As the Warning messages are more important than debug messages,  */
                   /* it processes warning message too                                 */
      
                   /* add the mask_debug in the dar_filter array */
                   dar_gbl_var_p ->dar_filter_array[index].mask_debug |= 
                                                                msg_p->use_msg_parameter.mask;
    
                   /* add the mask_warning in the dar_filter array  */
                   dar_gbl_var_p ->dar_filter_array[index].mask_warning |= 
                                                                msg_p->use_msg_parameter.mask;
                break;
                }
              
                case DAR_NO_DIAGNOSE:
                {
                   /* The DAR entity doesn't want to process Diagnose messages         */
      
                   /* delete the mask_debug in the dar_filter array */
                   dar_gbl_var_p ->dar_filter_array[index].mask_debug = 0x00;
    
                   /* delete the mask_warning in the dar_filter array  */
                   dar_gbl_var_p ->dar_filter_array[index].mask_warning = 0x00;
                   break;
                }
    
                default:
                {
                   /* Unknow level has been received */
                   DAR_TRACE_WARNING("A DAR unknow level has been received ");
                   break; 
                }
             } /* switch(msg_p->use_msg_parameter.level) */
          } /* if (search_group(msg_p->use_msg_parameter.group_nb,*index_gbl_p)== RV_OK) */
    
          else
          {
             /* if the group doesn't exist and if there is enough space in the dar_filter_array */
             if ( dar_add_group(&index)== RV_OK)
             {
                /* ... add the group in the dar_array_filter */
                dar_gbl_var_p ->dar_filter_array[index].group_nb |= msg_p->use_msg_parameter.group_nb;
                /* Check the Dar level  */
                switch(msg_p->use_msg_parameter.level) 
                {
                   case DAR_WARNING:
                   {   /* The DAR entity wants to process Warning messages */
                       /* add the mask_warning in the dar_filter array */
                      dar_gbl_var_p ->dar_filter_array[index].mask_warning |= 
                                                               msg_p->use_msg_parameter.mask;
                      break;
                   }
    
                   case DAR_DEBUG:
                   {
                      /* The DAR entity wants to process Debug messages                   */
                      /* As the Warning messages are more important than debug messages,  */
                      /* it processes warning message too                                 */
      
                      /* add the mask_debug in the dar_filter array */
                      dar_gbl_var_p ->dar_filter_array[index].mask_debug |= 
                                                                msg_p->use_msg_parameter.mask;
    
                      /* add the mask_warning in the dar_filter array  */
                      dar_gbl_var_p ->dar_filter_array[index].mask_warning |= 
                                                                msg_p->use_msg_parameter.mask;
                   break;
                   }
    
                   default:
                   {
                      /* Unknow level has been received */
                      DAR_TRACE_WARNING("A DAR unknow level has been received ");
                      break; 
                    }
                } /* switch */
             }/* if ( add_group(msg_p->use_msg_parameter.group_nb,*index_gbl_p)== RV_OK)) */
    
             else 
             {
                /* There is not enough space in the dar_array_filter */
                DAR_TRACE_WARNING("Not enough space in the dar_array_filter for adding a new group ");
             }
          }
          return(RV_OK);
       }/* dar_filter_request   */
    
    
       /* =========================================================== */
       /* Function Name:   dar_crash_time	                                      
                                                                                     
            Description      This function is called to capture the date and time of crash   
    
            Input Parameters:  None
       */                                                                              
       /* =========================================================== */
    
       T_RV_RET dar_crash_time(void)
       {
          dar_get_date_time(&date_time);    	  






          return(RV_OK);
        }         
    
       /* =========================================================== */
       /* Function Name:   dar_sw_version	                                      
                                                                                     
            Description      This function is called to capture the software version of the system   
    
            Input Parameters:  None
       */                                                                              
       /* =========================================================== */
    
       T_RV_RET dar_sw_version(void)
       {
          int i,length,start_loc,temp;
          i=length=start_loc=0;
          temp=1;
    
          for(i=0;temp!=0;i++) {
             temp=strcmp("/pcm/" "CGMR",pcm_table[i].identifier);
             length=pcm_table[i].length;
             start_loc=pcm_table[i].start;
          }  
    
          /* correction factors */
          i--;
          start_loc=start_loc-(i*2);			
          length=length-2;	  
    	  
          for(i=0;i<length;i++) {
       	   sw_version[i]=pcm_default_values[start_loc];
    	   start_loc++;
           }
    
          return(RV_OK);
        }         
    
       /* =========================================================== */
       /* Function Name:   dar_current_task_info	                                      
                                                                                     
            Description      This function is called to store the current task information 
    
            Input Parameters:  None
       */                                                                              
       /* =========================================================== */
    
       T_RV_RET dar_current_task_info(void)
       {
          current_task_ptr=NU_Current_Task_Pointer();
    
          return(RV_OK);
        }         
    
       #if (LOCOSTO_LITE==0)	  
       
          /* =========================================================== */
          /* Function Name:   dar_task_info                                      
                                                                                        
               Description      This function is called to save the task control block information  
       
               Input Parameters:  None
          */                                                                              
          /* =========================================================== */
          T_RV_RET dar_task_info(void)
          {
             int i;   
             NU_TASK * task_cb_ptr[MAX_TASK_COUNT];
      
             task_count=NU_Established_Tasks();									
             NU_Task_Pointers(&task_cb_ptr[0], task_count);
       
             /* TCB dump */	  
             for(i=0;i<task_count;i++) {										
                memcpy((void*)task_cb_array[i],(void *)task_cb_ptr[i],(TASK_CB_SIZE));	  
             	}
             
             return(RV_OK);
           }         
    
          /* =========================================================== */
          /* Function Name:   dar_hisr_info                                      
                                                                                        
               Description      This function is called to save the hisr control block information  
       
               Input Parameters:  None
          */                                                                              
          /* =========================================================== */
          T_RV_RET dar_hisr_info(void)
          {
             int i=0;   
             NU_HISR * hisr_cb_ptr[MAX_HISR_COUNT];
       	  
             hisr_count=NU_Established_HISRs();									
             NU_HISR_Pointers(&hisr_cb_ptr[0], hisr_count);
       
             /* HCB dump */	  
             for(i=0;i<task_count;i++) {										
                memcpy((void*)hisr_cb_array[i],(void *)hisr_cb_ptr[i],(HISR_CB_SIZE));	  
             }
             
             return(RV_OK);
           }         
    
          /* =========================================================== */
          /* Function Name:   dar_semaphore_info                                      
                                                                                        
               Description      This function is called to store control blocks of established semaphores  
       
               Input Parameters:  None
          */                                                                              
          /* =========================================================== */
          T_RV_RET dar_semaphore_info(void)
          {
             int i=0;
             NU_SEMAPHORE * sem_cb_ptr[MAX_SEM_COUNT];
       	  
             /* No. of established semaphores */
             sem_count=NU_Established_Semaphores();
       
             /* Pointers to control block of established semaphores */     	  
             NU_Semaphore_Pointers(&sem_cb_ptr[0], sem_count);
       
             /* SCB dump */
             for(i=0;i<sem_count;i++) {										
                memcpy((void*)sem_cb_array[i],(void *)sem_cb_ptr[i],(SEM_CB_SIZE));	  
             }
       	  
             return(RV_OK);
           }         
    
          /* =========================================================== */
          /* Function Name:   dar_queue_info                                             
                                                                                        
               Description      This function is called to store information about established queues	
       
               Input Parameters:  None
          */                                                                              
          /* =========================================================== */
          T_RV_RET dar_queue_info(void)
          {
             int i, ret_status;
    	  i=0;
    	  ret_status=0;
    	  queue_count=0;
                	  
             /* Queue information dump */
             for(i=0;i<MAX_QUEUE_COUNT;i++) {										
                memset((void*)queue_cb_array[i], 0,(QUEUE_CB_SIZE));	  
             }
    
             for(i=0;i<MAX_QUEUE_COUNT;i++) {										
    		ret_status=os_QueueInformation (i, (void *)queue_cb_array[i]);   
    		if(ret_status==0)
    			queue_count=i;
    	  }
    
             return(RV_OK);
           }         
    
       #endif
    
    
       /* =========================================================== */
       /* Function Name:   dar_write_data_in_buffer                                              
                                                                                        
               Description      This function is called to store diagnose data in RAM buffer    
                                                                                     
               Input Parameters:  Pointer to the message to store                                       
       
            Input Parameters:  None
       */                                                                              
       /* =========================================================== */
       T_RV_RET dar_write_data_in_buffer( T_DAR_WRITE_START *msg_p)
       {
          /* Local variables */
          UINT8   i = 0;
          UINT8 length = 0;
          
          /* Diagnose string length */
          length = (UINT16) strlen(msg_p->data_write.char_p);
       
          /***   Circular buffer to store data ***/
          /* Add 0xFF to separate 2 strings */
          dar_write_buffer[dar_current_index] = 0xF; 
          DAR_PLUS_PLUS(dar_current_index); /* to detected if it is the end of the buffer */
          dar_write_buffer[dar_current_index] = 0xF; 
          DAR_PLUS_PLUS(dar_current_index); /* to detected if it is the end of the buffer */
    
    
          /* The group_nb is 16 bit length, and the buffer is an UINT8 length */
          /* So the group_nb must be stocked by dividing it in 2 parts */
          dar_write_buffer[dar_current_index] = (msg_p->data_write.use_id.group_nb)>>8;
                                             /*add the 8 first bits of the Use id group*/
          DAR_PLUS_PLUS(dar_current_index);
          dar_write_buffer[dar_current_index] = msg_p->data_write.use_id.group_nb;
                                             /*add the 8 last bits of the Use id group*/
          DAR_PLUS_PLUS(dar_current_index);
          /* The mask is 16 bit length, and the buffer is an UINT8 length */
          /* So the mask must be stocked by dividing it in 2 parts */
          dar_write_buffer[dar_current_index] = (msg_p->data_write.use_id.mask)>>8;
                                             /* add the 8 first bits of the Use id mask */
          DAR_PLUS_PLUS(dar_current_index);
          dar_write_buffer[dar_current_index] = msg_p->data_write.use_id.mask;
                                             /* add the 8 last bits of the Use id mask */
          DAR_PLUS_PLUS(dar_current_index);
    
          /* Add the dar_level data */
          dar_write_buffer[dar_current_index] = msg_p->data_write.level;
          DAR_PLUS_PLUS(dar_current_index);
    
          /* circular buffer to store diagnose data in RAM buffer */
          for (i=0; i < length; i++ )
          {
             /* copy string in the RAM char by char*/
             dar_write_buffer[dar_current_index]=msg_p->data_write.char_p[i];
             
             /* detection of the end of the buffer */
             /* When current = DAR_MAX_BUFFER_SIZE , current = 0 */
             DAR_PLUS_PLUS(dar_current_index);
          }
    
          /* DAR information is redirected to standard trace */
          //DAR_SEND_TRACE("circular buffer : ",RV_TRACE_LEVEL_DEBUG_HIGH);
          //rvf_send_trace(msg_p->data_write.char_p, length, NULL_PARAM, RV_TRACE_LEVEL_DEBUG_HIGH, DAR_USE_ID); 
    
          return(RV_OK);
    
       } /* dar_send_write_data */
    
       /* =========================================================== */
       /* Function Name:   dar_empty_mb_and_save_data                                            
                                                                                        
               Description      This function is used to empty the mailbox and save data in the RAM buffer 
               
               Input Parameters: Pointer to the message to store                                       
       */                                                                              
       /* =========================================================== */
       T_RV_RET dar_empty_mb_and_save_data(  T_DAR_INFO *buffer_p)
       {
          /* Declare local variables */
          UINT8       i        = 0;
          UINT16      length   = 0;
          T_RV_HDR    *msg_p   = ( T_RV_HDR*) dar_read_mbox(DAR_MBOX);
    
          /**** Empty the mail box ****/
          while(msg_p != NULL)
          {
             /* If it's a Write message, store it in the Ram */
             /* ( it is not interesting to store filter message ) */
             if ((msg_p->msg_id) == DAR_WRITE_REQ)
             {
                /* store themessage in the RAM*/
                dar_write_data_in_buffer((T_DAR_WRITE_START *)msg_p);
             }
             /* free the Header of the message */
             rvf_free_buf((T_RVF_BUFFER *) msg_p);
    
             /* Read the next message */ 
             msg_p = ( T_RV_HDR*) dar_read_mbox(DAR_MBOX);
          } /* while (msg_p != NULL) */
    
          /**** Store data in RAM buffer ****/            
          /* Diagnose string length */
          length = (UINT16) strlen(buffer_p);
          
          /**   Circular buffer to store data **/
          /* Add 0xFF to separate 2 strings */
          dar_write_buffer[dar_current_index] = 0xF; 
          DAR_PLUS_PLUS(dar_current_index); /* to detected if it is the end of the buffer */
          dar_write_buffer[dar_current_index] = 0xF; 
          DAR_PLUS_PLUS(dar_current_index); /* to detected if it is the end of the buffer */
    
          /* Add the dar_level data */
          dar_write_buffer[dar_current_index] = (char)DAR_EXCEPTION;
          DAR_PLUS_PLUS(dar_current_index);
          /* circular buffer to store diagnose data in RAM buffer */
          for (i=0; i < length; i++ )
          {
             /* copy string in the RAM char by char*/
             dar_write_buffer[dar_current_index]=buffer_p[i];
             
             /* detection of the end of the buffer */
             /* When current = DAR_MAX_BUFFER_SIZE , current = 0 */
             DAR_PLUS_PLUS(dar_current_index);
          }
     
          return(RV_OK);
       }
    
       T_RV_RET dar_save_system_info(  T_DAR_INFO *buffer_p)
       {
          /* Crash date and time */
          dar_crash_time();	  
       
          /* Current task info */
          dar_current_task_info();
    
          /* Software version number */
          dar_sw_version();
    	  
          /* Save data in RAM buffer */
          dar_empty_mb_and_save_data(buffer_p);
    
          #if (LOCOSTO_LITE==0)	
    	  
             /* Task control block information */
             dar_task_info();
    
             /* HISR control block information */
             dar_hisr_info();
    
             /* Semaphore information */
             dar_semaphore_info();
    
             /* Queue information */
             dar_queue_info();
    		 
          #endif
    	  
          return(RV_OK);
       }
       
    #endif /* #ifdef RVM_DAR_SWE */
    
    /* =========================================================== */
    /* 
                  ------------------------------------------------                
                 |       WARNING       -      IMPORTANT                             |   
                  ------------------------------------------------                
    
        Function Name:   dar_lib                                           
                                                                                     
            Description      This function is only used in order to have a function in the 
                                  dar_lib when the DAR is NOT_COMPILED  
            
            Input Parameters: Pointer to the message to store                                       
    */                                                                              
    /* =========================================================== */
    
    void dar_lib(void)
    {
    }
    
    
