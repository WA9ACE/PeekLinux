/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1GTT_FUNC.H
 *
 *        Filename l1gtt_func.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
    #include "nucleus.h"
#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)
  #if (CODE_VERSION == SIMULATION)
    #include <string.h>
    #include "l1_types.h"
    #include "sys_types.h"
    #include "nu_main.h"
    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #if (AUDIO_TASK == 1)
      #include "l1audio_const.h"
      #include "l1audio_cust.h"
      #include "l1audio_defty.h"
    #endif

    #if (L1_GTT == 1)
      #include "l1gtt_const.h"
      #include "l1gtt_defty.h"
    #endif

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"
    #include "l1_proto.h"
    #include "l1_mftab.h"

    #include "l1_tabs.h"

    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif
  #else
    #include <string.h>
    #include "nucleus.h"
    #include "l1_types.h"
    #include "sys_types.h"

    #if (OP_L1_STANDALONE == 1)
      #include "nu_main.h"
    #endif

    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #if (AUDIO_TASK == 1)
      #include "l1audio_const.h"
      #include "l1audio_cust.h"
      #include "l1audio_defty.h"
    #endif

    #if (L1_GTT == 1)
      #include "l1gtt_const.h"
      #include "l1gtt_defty.h"
    #endif

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"
    #include "l1_proto.h"
    #include "l1_mftab.h"

    #include "l1_tabs.h"

    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif

    #if (OP_L1_STANDALONE == 0) && (LOCOSTO_LITE == 1) && (BAE_STATE == 1)

    #include "typedefs.h"
    #include "os.h"
    #include "vsi.h"
    extern T_HANDLE bae_data_pool_handle;

    #endif

  #endif

  /********************/
  /* Prototypes       */
  /********************/

  WORD16   TTY_error(WORD16 return_status);

#if (CODE_VERSION != SIMULATION)
  UWORD16* TTY_addr_dsp2mcu(UWORD16 dsp_address);
#endif

#if (OP_L1_STANDALONE == 0)
  #include "tty_i.h"
  extern T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p;
#endif

  /*-------------------------------------------------------*/
  /* TTY_error()                                           */
  /*-------------------------------------------------------*/
  /* Parameters :                                          */
  /* Return     :                                          */
  /* Functionality :                                       */
  /*  Detect the fifo error                                */
  /*-------------------------------------------------------*/
  WORD16 TTY_error(WORD16 return_status)
  {
   if(return_status==TTY_FIFO_OVERFLOW)
    {
      return 0;
    }
    if(return_status==TTY_FIFO_UNDERFLOW)
    {
      return 0;
    }
    return 1;
  }

#if (CODE_VERSION != SIMULATION)

  /*-------------------------------------------------------*/
  /* TTY_addr_dsp2mcu()                                    */
  /*-------------------------------------------------------*/
  /* Parameters : DSP address                              */
  /* Return     : MCU address                              */
  /* Functionality :                                       */
  /*  Transform the @dsp API address @mcu API              */
  /*-------------------------------------------------------*/
  UWORD16* TTY_addr_dsp2mcu(UWORD16 DSP_ADDRESS)
  {
      UWORD32 MCU_ADDR_INT;

      MCU_ADDR_INT = (UWORD32)(DSP_ADDRESS - DSP_API_START);
      MCU_ADDR_INT *= 2;
      MCU_ADDR_INT += DB_W_PAGE_0;
      return (UWORD16 *)MCU_ADDR_INT;
  }

#endif

  /*-------------------------------------------------------*/
  /* l1gtt_malloc()                                        */
  /*-------------------------------------------------------*/
  /* Parameters : size of memory to allocate               */
  /* Return     : pointer on the allocated memory          */
  /* Functionality :                                       */
  /*  Allocate memory for GTT data processing              */
  /*-------------------------------------------------------*/
  void *l1gtt_malloc(UWORD32 size)
  {
    void *return_ptr;

    #if (OP_L1_STANDALONE == 1)
      UWORD8 status;

      status = NU_Allocate_Memory(&L1GTT_Memory,
                                  (void **) &return_ptr,
                                  size,
                                  NU_NO_SUSPEND);
    #else
    #if ((LOCOSTO_LITE == 0) || (BAE_STATE == 0))
      T_RVF_MB_STATUS mb_status;

      mb_status = rvf_get_buf(tty_env_ctrl_blk_p->prim_mb_id,
                              size,
                              (T_RVF_BUFFER **) (&return_ptr));

      if (mb_status == RVF_RED)
      {
        TTY_SEND_TRACE("TTY: Error to get memory ",RV_TRACE_LEVEL_ERROR);
        return NULL;
      }
      else if (mb_status == RVF_YELLOW)
      {
        /*
        * The flag is yellow, there will soon be not enough memory anymore.
        */
        TTY_SEND_TRACE("TTY: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
      }
    #else // (LOCOSTO_LITE == 0) || (BAE_STATE == 0)
      {
          long retval = 0;

          retval = os_AllocateMemory ( gsp_get_taskid(),
                (unsigned long **)&(return_ptr),
                size,
    		    OS_NO_SUSPEND,
                bae_data_pool_handle );
          if(retval != 0)
          {
              TTY_SEND_TRACE("TTY: Error to get memory ",RV_TRACE_LEVEL_ERROR);
              while(1); // Just for debugging needs to be removed
//omaps00090550              return NULL;
          }
      }
    #endif // (LOCOSTO_LITE == 0) || (BAE_STATE == 0)

    #endif

    return(return_ptr);
  }

  /*-------------------------------------------------------*/
  /* l1gtt_free()                                          */
  /*-------------------------------------------------------*/
  /* Parameters : pointer on the memory buffer to          */
  /*              deallocate                               */
  /* Return     :                                          */
  /* Functionality :                                       */
  /*  Free memory used for GTT data processing             */
  /*-------------------------------------------------------*/
  void l1gtt_free(void *ptr)
  {
    #if (OP_L1_STANDALONE == 1)
      UWORD8 status;

      status = NU_Deallocate_Memory(ptr);
    #else
    #if ((LOCOSTO_LITE == 0) || (BAE_STATE == 0))
      T_RVF_RET mb_status;

      mb_status = rvf_free_buf((T_RVF_BUFFER *) ptr);
      if (mb_status != RVF_GREEN)
      {
        TTY_SEND_TRACE("TTY: Error to deallocate buffer ",RV_TRACE_LEVEL_ERROR);
      }
    #else
      os_DeallocateMemory(gsp_get_taskid(),
            (T_VOID_STRUCT *)ptr );
    #endif

    #endif
  }
#endif
