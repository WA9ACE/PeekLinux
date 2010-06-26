/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * MEM_LOAD.C
 *
 *        Filename mem_load.c
 *        Version  1.6
 *        Date     06/20/01
 *
 ************* Revision Controle System Header *************/

#ifndef _WINDOWS
  #include "chipset.cfg"
  #include "board.cfg"
  #include "swconfig.cfg"
#endif

#include "l1sw.cfg"    /* OP_L1_STANDALONE definition */
                 
#if ((((CHIPSET == 3) || (CHIPSET == 4) || (CHIPSET == 5) || (CHIPSET == 6) || (CHIPSET == 7) || \
       (CHIPSET == 8) || (CHIPSET == 9) || (CHIPSET == 10) || (CHIPSET  ==  11)) && (LONG_JUMP != 0)) || \
       (CHIPSET == 12) ||(CHIPSET == 15))
  /* 
   * USE_GZIP flag is used by modem binaries compression feature on P2. 
   * For P2 build variants it is defined globally - either in master.cfg (HBB)
   * or P2's configdef *.xml files (UBB). 
   * The following definition is for all other platforms - just to 
   * suppress compilation warnings.
   */
  #ifndef USE_GZIP
    #define USE_GZIP       0
  #endif

  #if (OP_L1_STANDALONE == 1)
    #define D_LOAD_START   _470_INT_MEM_out_call_load_start
    #define D_RUN_START    _470_INT_MEM_out_call_run_start
    #define D_RUN_END      _470_INT_MEM_out_call_run_end
  #else
    #define D_LOAD_START   _470_S_MEM_out_call_load_start
    #define D_RUN_START    _470_S_MEM_out_call_run_start
    #define D_RUN_END      _470_S_MEM_out_call_run_end
  #endif

  typedef unsigned char UWORD8;
  typedef unsigned short int UWORD16;
  typedef unsigned long int UWORD32;

  
  UWORD16 d_checksum1;
  UWORD16 d_checksum2;

  /* NEW COMPILER MANAGEMENT
   * INT_memset and INT_memcpy, respectively identical to memset and 
   * memcpy from the rts library of compiler V2.51, are defined in int.s.
   * They are used to make the initialization of the .bss section and the load 
   * of the internal ram code not dependent to the 32-bit alignment.
   * The old code used for the initialization and the load used a loop with
   * 4-byte increment, assuming the 32-bit alignment of the .bss section.
   * This alignment is not true with compiler V2.51.
   * This change applies whatever the compiler version.
   * INT_memset replaces f_zero
   * INT_memcpy replaces f_download
   */
  extern void INT_memset(void *s, int c, UWORD32 n);
  extern void INT_memcpy(void *s1, void *s2, UWORD32 n);
  
  #if (OP_L1_STANDALONE == 0) && (BOARD == 35) && (USE_GZIP == 1)
    extern void decompress(unsigned long input_start,
                           unsigned long input_length,
                           unsigned long output_start);
  #else

#ifdef USE_CHECKSUM
    UWORD16 f_checksum(UWORD8 *p_begin,
                       UWORD32 d_length) {
      UWORD16 d_checksum = 0x0000;

      while(d_length > 0) {
        d_checksum += *(p_begin++);
        d_length --;
      }

      return(d_checksum);
    }
#else
    UWORD16 f_checksum(UWORD8 *p_begin,
                       UWORD32 d_length) {
    return 0;                   
}
#endif

  #endif

  #if (CHIPSET == 12) || (CHIPSET == 15)
    /* 
     *  Load start address of the interrupt vector table from FLASH into Internal SRAM
     */
    #define C_NUMBER_OF_INTERRUPT_VECTOR  7
    #pragma DATA_SECTION(d_interrupt_load_start,".intload")
    const UWORD32 d_interrupt_load_start=0L;
    extern const UWORD32 IndirectVectorTable[C_NUMBER_OF_INTERRUPT_VECTOR * 2];

    #define C_INTERRUPT_VECTOR_LOAD_START  (UWORD32)((UWORD32)(&d_interrupt_load_start) + 4)
    #define C_INTERRUPT_VECTOR_RUN_START   (UWORD32)((UWORD32)&IndirectVectorTable[0])
    #define C_INTERRUPT_VECTOR_LENGTH      (UWORD32)(C_NUMBER_OF_INTERRUPT_VECTOR * 2 * sizeof(UWORD32))
  #endif   /* (CHIPSET == 12) || (CHIPSET == 15)*/
  

  #if ((LONG_JUMP == 3) || (((CHIPSET==15)||(CHIPSET == 12)) && (LONG_JUMP == 0)))
    /* 
     *  Load start address of the code downloaded from FLASH into Internal SRAM
     */
    #pragma DATA_SECTION(d_application_load_start,".ldfl")
    const UWORD32 d_application_load_start=0L;
    #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	    #if (TOOL_CHOICE == 0)
	    /* NEW COMPILER MANAGEMENT 
	     * If use of VISUAL LINKER, needs to manage trampoline download.
	     * Case of:
	     *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	     */
	      #if !(((CHIPSET == 12) || (CHIPSET==15)) && (LONG_JUMP == 0))
            extern UWORD8 D_LOAD_START;
	      #endif    /* (((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0)) */
	    #endif   /* (TOOL_CHOICE == 0) */
    #endif   /* (OP_WCP == 0) */

    /* 
     *  Run start address of the code downloaded from FLASH into Internal SRAM
     */
    #pragma DATA_SECTION(d_application_run_start,"S_P_Mem")
    const UWORD32 d_application_run_start = 0L;
    #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	    #if (TOOL_CHOICE == 0)
	    /* NEW COMPILER MANAGEMENT  
	     * If use of VISUAL LINKER, needs to manage trampoline download.
	     * Case of:
	     *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	     */
	      #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
            extern UWORD8 D_RUN_START;
	      #endif    /* (((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0)) */
	    #endif   /* (TOOL_CHOICE == 0) */
    #endif   /* (OP_WCP == 0) */

    /* 
     *  Run end address of the code downloaded from FLASH into Internal SRAM
     */
    #pragma DATA_SECTION(d_application_run_end,"E_P_Mem")
    const UWORD32 d_application_run_end = 0L;
    #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	    #if (TOOL_CHOICE == 0)
	    /* NEW COMPILER MANAGEMENT  
	     * If use of VISUAL LINKER, needs to manage trampoline download.
	     * Case of:
	     *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	     */
	      #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
	        extern UWORD8 D_RUN_END;
	      #endif    /* (((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0)) */
	    #endif   /* (TOOL_CHOICE == 0) */
    #endif   /* (OP_WCP == 0) */

    /*
     *  Application download
     */
    #define C_APPLICATION_LOAD_START  (UWORD32)((UWORD32 *)(&d_application_load_start) + 2)
    #define C_APPLICATION_RUN_START   (UWORD32)((UWORD32)(&d_application_run_start) + sizeof(UWORD32))
    #define C_APPLICATION_RUN_END     (UWORD32)((UWORD32)(&d_application_run_end) + sizeof(UWORD32))
    #define C_APPLICATION_LENGTH      (UWORD32)(C_APPLICATION_RUN_END - C_APPLICATION_RUN_START - sizeof(UWORD32))

    #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	    #if (TOOL_CHOICE == 0)
	    /* NEW COMPILER MANAGEMENT  
	     * If use of VISUAL LINKER, needs to manage trampoline download.
	     * Case of:
	     *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	     */
	      #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
	      /*
	       *  Trampoline download
	       */
            #define C_TRAMPOLINE_LOAD_START  (UWORD32)(&D_LOAD_START)
            #define C_TRAMPOLINE_RUN_START   (UWORD32)(&D_RUN_START)
            #define C_TRAMPOLINE_RUN_END     (UWORD32)(&D_RUN_END)
            #define C_TRAMPOLINE_LENGTH      (UWORD32)(&D_RUN_END - &D_RUN_START)
	      #endif    /* (((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0)) */
	    #endif   /* (TOOL_CHOICE == 0) */
    #endif      /* (OP_WCP == 0) */

    void f_load_int_mem(void) {
      /* 
       *  Reset checksum of code downloaded
       */
      d_checksum1 = 0;  /* Load checksum */
      d_checksum2 = 0;  /* Run checksum */

      /*
       *  Reset the Internal memory where the code must be downloaded
       */
      INT_memset((void *)(C_APPLICATION_RUN_START),0,C_APPLICATION_LENGTH);
      #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	      #if (TOOL_CHOICE == 0)
		    /* NEW COMPILER MANAGEMENT  
		     * If use of VISUAL LINKER, needs to manage trampoline download.
		     * Case of:
		     *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
		     */
	        #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
	          INT_memset((void *)(C_TRAMPOLINE_RUN_START),0,C_TRAMPOLINE_LENGTH);
	        #endif
  	    #endif   /* (TOOL_CHOICE == 0) */

        #if ((CHIPSET == 12)||(CHIPSET==15))
          INT_memset((void *)(C_INTERRUPT_VECTOR_RUN_START),0,C_INTERRUPT_VECTOR_LENGTH);
        #endif
      #endif   /* (OP_WCP == 0) */

      #if (OP_L1_STANDALONE == 0) && (BOARD == 35) && (USE_GZIP == 1)
        #define C_GZIP_START                  (C_APPLICATION_LOAD_START)
        #define C_GZIP_LENGTH                 (*(UWORD32*)C_APPLICATION_LOAD_START)
        #define C_GZIP_OUT                    (C_APPLICATION_RUN_START - sizeof(UWORD32))

        /* De-compress on-the-fly and load to internal RAM */
        decompress(C_GZIP_START,              /* load start (compressed) */
                   C_GZIP_LENGTH,             /* compressed length */
                   C_GZIP_OUT);               /* run start */ 
      #else
        /*
         *  Compute checksum on code in FLASH (code and trampoline functions)
         */
        d_checksum1 =  f_checksum((UWORD8 *)(C_APPLICATION_LOAD_START), C_APPLICATION_LENGTH);
        #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
		#if (TOOL_CHOICE == 0)
	        /* NEW COMPILER MANAGEMENT  
	         * If use of VISUAL LINKER, needs to manage trampoline download.
	         * Case of:
	         *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	         */
	          #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
	            d_checksum1 += f_checksum((UWORD8 *)(C_TRAMPOLINE_LOAD_START),  C_TRAMPOLINE_LENGTH);
	          #endif
		#endif   /* (TOOL_CHOICE == 0) */

          	#if ((CHIPSET == 12)||(CHIPSET==15))
            	   d_checksum1 += f_checksum((UWORD8 *)(C_INTERRUPT_VECTOR_LOAD_START), C_INTERRUPT_VECTOR_LENGTH);
          	#endif
        #endif   /* (OP_WCP == 0) */


        /*
         *  Download code from FLASH into Internal RAM (both code and trampoline functions)
         */
        INT_memcpy((void *)(C_APPLICATION_RUN_START),(void *)(C_APPLICATION_LOAD_START),C_APPLICATION_LENGTH);
        #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	        #if (TOOL_CHOICE == 0)
	        /* NEW COMPILER MANAGEMENT  
	         * If use of VISUAL LINKER, needs to manage trampoline download.
	         * Case of:
	         *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	         */
	          #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
	            INT_memcpy((void *)(C_TRAMPOLINE_RUN_START),(void *)(C_TRAMPOLINE_LOAD_START),C_TRAMPOLINE_LENGTH);
	          #endif
		 #endif   /* (TOOL_CHOICE == 0) */

          	#if ((CHIPSET == 12)||(CHIPSET==15))
		    INT_memcpy((void *)(C_INTERRUPT_VECTOR_RUN_START),(void *)(C_INTERRUPT_VECTOR_LOAD_START),C_INTERRUPT_VECTOR_LENGTH);
		#endif
        #endif   /* (OP_WCP == 0) */


        /*
         *  Compute checksum on code in FLASH (code and trampoline functions)
         */
        d_checksum2 =  f_checksum((UWORD8 *)(C_APPLICATION_RUN_START), C_APPLICATION_LENGTH);
        #if ((OP_WCP == 0) || (OP_L1_STANDALONE == 1))
	        #if (TOOL_CHOICE == 0)
	        /* NEW COMPILER MANAGEMENT  
	         * If use of VISUAL LINKER, needs to manage trampoline download.
	         * Case of:
	         *     - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
	         */
	          #if !(((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))
	            d_checksum2 += f_checksum((UWORD8 *)(C_TRAMPOLINE_RUN_START),  C_TRAMPOLINE_LENGTH);
	          #endif
		 #endif   /* (TOOL_CHOICE == 0) */

          	#if ((CHIPSET == 12)||(CHIPSET==15))
		    d_checksum2 += f_checksum((UWORD8 *)(C_INTERRUPT_VECTOR_RUN_START),  C_INTERRUPT_VECTOR_LENGTH);
                #endif
        #endif   /* (OP_WCP == 0) */

      #endif /* (OP_L1_STANDALONE == 0) && (BOARD == 35) && (USE_GZIP == 1) */

    } /* f_load_int_mem() */
  #endif   /* ((LONG_JUMP == 3) || (((CHIPSET == 12)||(CHIPSET==15)) && (LONG_JUMP == 0))) */
#endif     /* ((((CHIPSET == 3) || (CHIPSET == 4) ||   ...    (CHIPSET == 12) || (CHIPSET == 15)) */
