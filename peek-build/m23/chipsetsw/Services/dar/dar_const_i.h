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
    * @file dar_const_i.h  
    *
    * Internal constants used by DAR instance 
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
    *! 26-Sep-2001 mf: Create
    *! Author Stephanie Gerthoux
    *!
    * =============================================================================
    */
    #include "rv/rv_defined_swe.h"
	
    #ifdef RVM_DAR_SWE
    
        #ifndef __DAR_CONST_I_H_
            #define __DAR_CONST_I_H_
     
            /* includes */
            #include "chipset.cfg"
         
            #ifdef __cplusplus
                extern "C"
                {
            #endif
         
            /* Define a mask used to identify the events */
            #define DAR_EVENT_EXTERN                            	(0x0C00)
            #define DAR_EVENT_INTERN                            	(0x0300)
         
            /* The DAR task run without any time out */
            #define DAR_NOT_TIME_OUT                            	(0)
         
            /* The DAR entity processes only the messages send to the following mail box */
            #define DAR_MBOX                                    		(RVF_TASK_MBOX_0)
         
            /* The DAR entity takes into account only the following events: */
            #define DAR_TASK_MBOX_EVT_MASK                	(RVF_TASK_MBOX_0_EVT_MASK)
         
            /* The DAR entity waits all event type */
            #define DAR_ALL_EVENT_FLAGS                  		(0xFFFF)
         
            /* The DAR use max group elements used */
            #define DAR_MAX_GROUP_NB                       		(4)
         
            /* RAM max buffer size    */
            #if (LOCOSTO_LITE)
                #define DAR_MAX_BUFFER_SIZE                     	(500)
            #else
                #define DAR_MAX_BUFFER_SIZE				(3000)
            #endif
         
            /* Recovery data max buffer size    */
            #define DAR_RECOVERY_DATA_MAX_BUFFER_SIZE	(50)
         
            /* Dar invalid value    */
            #define DAR_INVALID_VALUE                           	(0xFFFF)
               
            /* Dar initialization */
            #define DAR_INITIALIZATION_VALUE                 	(0x0000)
         
            /* Define the Watchdog timer register mode */
            #define WATCHDOG_TIM_MODE                           	(0xFFFFF804)
              	  
            #if ((CHIPSET == 7) || (CHIPSET == 8) || (CHIPSET == 10) || (CHIPSET == 11) || (CHIPSET == 14))
         	     /* Define the Debug Unit register mode */
         	     #define DAR_DEBUG_UNIT_REGISTER        	(0x03C00000)
            #elif	(CHIPSET == 12 || CHIPSET == 15)
         	     /* Define the Debug Unit register mode */
         	     #define DAR_DEBUG_UNIT_REGISTER             (0x09F00000)
            #endif  
         
            /* Mask to enable the Debug Unit Module */
            #define ENABLE_DU_MASK                              		(0xF7FF)
         
            /* Mask to disable the Debug Unit Module */
            #define DISABLE_DU_MASK                           		(0x0800)
         
            /* Extra Control register CONF Adress */
            #define DAR_DU_EXTRA_CONTROL_REG              	(0xFFFFFB10)
              
            /* Define the size of the Debug Unit register      */
            /* This size is 64 words of 32 bits = 64*4 bytes */ 
            /* Size in bytes */
            #define DEBUG_UNIT_BYTES_SIZE                       	(256)
            /* Define the size in words */
            #define DEBUG_UNIT_WORD_SIZE                        	(64) 
         
            /* Define the size of the X_dump _buffer */
            /* This size is specified in the gsm_cs_amd4_lj3_test.cmd (in "system" directory) */
            /* Its size is 38*32 bits = 38*4 bytes = 152 bytes */
            #define DAR_X_DUMP_BUFFER_SIZE                      (152)
         
            /* To test watchdog reset bit */
            #define WATCHDOG_RESET_TEST				(8)
         	  
            /* Define size in bytes */
            #define BYTE_SIZE								(1)
            #define CRASH_COUNT_SIZE             				(4)	
            #define CRASH_TIME_SIZE						(9)
         	  
            #define STR2IND_VERSION_SIZE  				(11)  
            #define SW_VERSION_SIZE			       		(20)	  
         
            #define TASK_ID_SIZE							(4)
            #define TASK_NAME_SIZE						(8)
         
            #if (LOCOSTO_LITE==0)	  
                #define MAX_TASK_COUNT					(100)
                #define MAX_HISR_COUNT					(100)
                #define MAX_SEM_COUNT					(255)
                #define MAX_QUEUE_COUNT					(100)
                #define TASK_CB_SIZE						(168)	
                #define HISR_CB_SIZE						(88)		 
                #define SEM_CB_SIZE						(40)		
         	     #define QUEUE_CB_SIZE					(100)
            #endif	  
         	  
            /* Define the exceptions */
            #define DAR_NO_ABORT_EXCEPTION				(0)
            #define DAR_EXCEPTION_DATA_ABORT                 	(1)
            #define DAR_EXCEPTION_PREFETCH_ABORT         	(2)
            #define DAR_EXCEPTION_UNDEFINED                   	(3)
            #define DAR_EXCEPTION_SWI					(4)
            #define DAR_EXCEPTION_RESERVED                      	(5)
         
        #ifdef __cplusplus
            }
        #endif
    	
        #endif /* __DAR_CONST_I_H_ */
    	
    #endif /* #ifdef RVM_DAR_SWE */
    
