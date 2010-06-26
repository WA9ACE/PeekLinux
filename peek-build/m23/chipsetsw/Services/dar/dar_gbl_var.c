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
    * @file dar_gbl_var.c   
    *
    * This function contains the global variables that are not initialized in the .bss                                 
    * These variables are stored in the .bss_dar section   
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
    *! 29-Oct-2001 mf: Create
    *! Author Stephanie Gerthoux
    *!
    * =============================================================================
    */
    
    #include "rv/rv_defined_swe.h"
    #ifdef RVM_DAR_SWE

	/* includes */
       #include "general.h"
       #include "rtc/rtc_api.h"
       #include "rv/rv_general.h"
       #include "dar/dar_api.h"
       #include "dar/dar_const_i.h"
       #include "nucleus.h"
    
       /**** Global variables ****/
       /* Buffer used to save some parameters before a reset */
       UINT8    dar_recovery_buffer[DAR_RECOVERY_DATA_MAX_BUFFER_SIZE];
      
       /* dar_current_status : to get the status of the system*/
       T_DAR_RECOVERY_STATUS dar_current_status;
    
       /* dar_exception_status : to get the status of the exception */
       UINT8    dar_exception_status;
    
       UINT8 xdump_buffer[DAR_X_DUMP_BUFFER_SIZE];
    
       /* Write buffer*/
       char    dar_write_buffer[DAR_MAX_BUFFER_SIZE];
    
       /* Software version */
       char sw_version[SW_VERSION_SIZE];
    
       /* Ram buffer that contains the Debug Unit register */
       UINT32  debug_RAM[DEBUG_UNIT_WORD_SIZE];
    
       /* Crash date and time */
       T_RTC_DATE_TIME date_time;
       
       /* Current task information pointer*/
       NU_TASK  * current_task_ptr;
    
       #if (LOCOSTO_LITE==0)	  
       
          /* Task control block information */
          UINT8 task_count;
          char task_cb_array[MAX_TASK_COUNT][TASK_CB_SIZE];
    
          /* HISR control block information */
          UINT8 hisr_count;   
          char hisr_cb_array[MAX_HISR_COUNT][HISR_CB_SIZE];	
      
         /* Semaphore control block information */
         UINT8 sem_count;
         char sem_cb_array[MAX_SEM_COUNT][SEM_CB_SIZE];
    
         /* Queue control block information */
         UINT8 queue_count;
         char queue_cb_array[MAX_QUEUE_COUNT][QUEUE_CB_SIZE];
    	  
       #endif
    
    #endif /* #ifdef RVM_DAR_SWE */
    
