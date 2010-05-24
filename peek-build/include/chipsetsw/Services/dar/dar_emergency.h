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
    * @file dar_emergency.h    
    *
    * This function contains the functions prototypes of the DAR entity when emergencies occured.                              
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
    
    #ifndef __DAR_EMERGENCY_H_
    #define __DAR_EMERGENCY_H_
    
      #include "rv/rv_defined_swe.h"
    
      #ifdef RVM_DAR_SWE
    
        #include "rvm/rvm_gen.h"
    
        /* Functions prototypes */
        T_RV_RET dar_process_emergency( T_DAR_INFO    *buffer_p,
                                        T_DAR_FORMAT  format,
                                        T_RVM_USE_ID  dar_use_id,
                                        UINT32 flags);
        void dar_exception_arm_undefined(void);
        void dar_exception_arm_swi(void);
        void dar_exception_arm_abort_prefetch(void);
        void dar_exception_arm_abort_data(void);
        void dar_exception_arm_reserved(void);
    
      #endif /* #ifdef RVM_DAR_SWE */
    
      void dar_exception(int abort_type);
    
    #endif /* __DAR_EMERGENCY_H_ */
