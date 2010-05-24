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
    * @file dar_msg_ft.h 
    *
    *  This function contains the functions prototypes of the DAR 
    *  entity messages functions. 
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
    
       #ifndef __DAR_MSG_FT_H_
       #define __DAR_MSG_FT_H_
    
       #include "rvm/rvm_gen.h"
    
       /* Functions prototypes */
       T_RV_RET dar_filter_request (T_DAR_FILTER_START *msg_p);
       T_RV_RET dar_empty_mb_and_save_data(  T_DAR_INFO    *buffer_p);
       T_RV_RET dar_write_data_in_buffer( T_DAR_WRITE_START *msg_p);
       T_RV_RET dar_save_system_info(  T_DAR_INFO *buffer_p);
    
       #endif /* __DAR_MSG_FT_H_ */
    
    #endif /* #ifdef RVM_DAR_SWE */
