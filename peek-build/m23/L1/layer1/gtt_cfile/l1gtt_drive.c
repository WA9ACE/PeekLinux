/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1GTT_DRIVE.C
 *
 *        Filename l1gtt_drive.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "nucleus.h"   //omaps00090550
#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)
  #if (CODE_VERSION == SIMULATION)
    #include <string.h>

    #include "l1_types.h"
    #include "sys_types.h"
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
      #include "ctm_transmitter.h"
      #include "ctm_receiver.h"
      #include "l1gtt_baudot_functions.h"

      #include "l1gtt_const.h"
      #include "l1gtt_defty.h"
      #include "l1gtt_msgty.h"
      #include "l1gtt_signa.h"
      #include "l1gtt_varex.h"
    #endif

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "nu_main.h"
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
//    #include "nucleus.h"
    #include "l1_types.h"
    #include "sys_types.h"
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

    #if (OP_L1_STANDALONE == 1)
      #include "nu_main.h"
    #endif

    #if (L1_GTT == 1)
      #include "ctm_transmitter.h"
      #include "ctm_receiver.h"
      #include "l1gtt_baudot_functions.h"

      #include "l1gtt_const.h"
      #include "l1gtt_defty.h"
      #include "l1gtt_msgty.h"
      #include "l1gtt_signa.h"
      #include "ctm_transmitter.h"
      #include "l1gtt_varex.h"
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
  #endif

  #include "ctm_typedefs.h"
  #include "fifo.h"

  #if (CODE_VERSION == SIMULATION)
    // Declaration of FIFO headers
    extern T_TTY_FIFO_struct header_fifo1;
    extern T_TTY_FIFO_struct header_fifo2;
    extern T_TTY_FIFO_struct header_fifo3;
    extern T_TTY_FIFO_struct header_fifo4;

    // Declaration of FIFO data areas
    extern API            fifo1[FIFO_1_size];
    extern API            fifo2[FIFO_2_size];
    extern API            fifo3[FIFO_3_size];
    extern API            fifo4[FIFO_4_size];
  #endif

  // For TI DSP buffers
  extern T_TTY_FIFO_struct *TTY_FIFO_1;
  extern T_TTY_FIFO_struct *TTY_FIFO_2;
  extern T_TTY_FIFO_struct *TTY_FIFO_3;
  extern T_TTY_FIFO_struct *TTY_FIFO_4;

  extern fifo_state_t  *global_ul_ptr;
  extern fifo_state_t  *global_dl_ptr;
  extern fifo_state_t  *baudotCodeFifoState_ptr;
  /* Store the output from CTM transmitter in uplink */
  extern fifo_state_t  *ctmCodeULFifoState_ptr;

  #if (CODE_VERSION == SIMULATION)
    API    *fifo2_write_addr;     // internal write pointer for fifo2.
  #else
  UWORD16 fifo2_write_addr;     // internal write pointer for fifo2.
  #endif
  UWORD8  fifo2_write_counter;    // 8 times write counter for fifo2.

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

 /*********************/
 /* Prototypes        */
 /*********************/

  void    TTY_fifo_initialize            (void);
  WORD16  TTY_fifo_chk_read              (T_TTY_FIFO_struct *FIFO,
                                          WORD16            *data_exchanged);
  WORD16  TTY_fifo_chk_write             (T_TTY_FIFO_struct *FIFO,
                                          WORD16 *data_exchanged);
  void    TTY_read_data_tty_estimator    (WORD16* data_from_tty_estimator,WORD16* status_fifo);
  void    TTY_write_data_ctm_transmitter (WORD16* data_to_ctm_modulator,WORD16* status_fifo);
  void    TTY_read_data_ctm_estimator    (WORD16* data_from_ctm_estimator,WORD16* status_fifo);
  void    TTY_write_data_baudot_encoder  (WORD16* data_to_tty_modulator,WORD16* status_fifo);
  UWORD16 TTY_fifo_read                  (T_TTY_FIFO_struct *FIFO,
                                          WORD16 *data_exchanged,
                                          UWORD16 max_size);
  UWORD16 TTY_fifo_write_buff2           (WORD16 *data_exchanged,
                                          UWORD16 max_size);
  UWORD16 TTY_fifo_write_buff4           (WORD16 *data_exchanged,
                                          UWORD16 max_size);

 /**********************/
 /* External prototype */
 /**********************/

  WORD16   TTY_error(WORD16 return_status);
#if (CODE_VERSION != SIMULATION)
  UWORD16* TTY_addr_dsp2mcu(UWORD16 dsp_address);
#else
    #define TTY_addr_dsp2mcu
#endif
  extern BOOL ctm_fifo2_flag;

  extern T_GTT_DEBUG l1_gtt_debug;

  /*-------------------------------------------------------*/
  /* l1g_ctm_check_read()                                  */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Check GTT data in NDB and set OS flag (message) to    */
  /* L1GTT task.                                           */
  /*-------------------------------------------------------*/
  void l1g_ctm_check_read(void)
  {
    WORD16 repeat;
    WORD16 workingbuffer[60];

    //---------------------------------------
    // Downlink (FIFO 3) read check and read
    //---------------------------------------

    TTY_fifo_chk_read(TTY_FIFO_3, &repeat);

    if(repeat > 0)
    {
      if(repeat & 0x0001){
        l1_gtt_debug.odd_fifo3_count++;
      }
      repeat &= 0xFFFE;
      repeat = TTY_fifo_read(TTY_FIFO_3, workingbuffer, repeat);
      Shortint_fifo_push(global_dl_ptr, workingbuffer, repeat);
    }

    //-------------------------------------
    // Uplink (FIFO 1) read check and read
    //-------------------------------------

    TTY_fifo_chk_read(TTY_FIFO_1, &repeat);

    if(repeat > 0)
    {
      repeat = TTY_fifo_read(TTY_FIFO_1, workingbuffer, repeat);
      // When we are in transparent mode, in l1ggt_back.c, l1ggt_ul_processing is not called
      // Hence after a long time we will see FIFO overflow errors on the GTT traces
      // To avoid this, do not push into the FIFO as long as transparent mode is true
      if(transparentMode == false)
      {
        Shortint_fifo_push(global_ul_ptr, workingbuffer, repeat);
      }
    }
  }

  /*-------------------------------------------------------*/
  /* l1g_DSP_buffer_writes()                               */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Writes processed data in FIFO2 and FIFO4.             */
  /*-------------------------------------------------------*/
  void l1g_DSP_buffer_writes(void)
  {
    WORD16 repeat;
    WORD16 workingbuffer[120];

    if (ctm_fifo2_flag == FALSE)
    {
      if(ctmCodeULFifoState_ptr->num_entries_actual >0)
      {
        TTY_fifo_chk_write(TTY_FIFO_2, &repeat);

        if(repeat > ctmCodeULFifoState_ptr->num_entries_actual)
        {
            repeat = ctmCodeULFifoState_ptr->num_entries_actual;
        }
        repeat = repeat & 0xFFFE;

        Shortint_fifo_pop(ctmCodeULFifoState_ptr, workingbuffer, repeat);

        TTY_fifo_write_buff2(workingbuffer, repeat);
      }
    }

    if(baudotCodeFifoState_ptr->num_entries_actual > 0)
    {
      TTY_fifo_chk_write(TTY_FIFO_4, &repeat);

      if(repeat > baudotCodeFifoState_ptr->num_entries_actual)
      {
          repeat = baudotCodeFifoState_ptr->num_entries_actual;
      }

      Shortint_fifo_pop(baudotCodeFifoState_ptr, workingbuffer, repeat);
      TTY_fifo_write_buff4(workingbuffer, repeat);
    }
  }

  /*-------------------------------------------------------*/
  /* TTY_fifo_initialize()                                 */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Initialize all the FIFO buffer                        */
  /*-------------------------------------------------------*/
  void TTY_fifo_initialize(void)
  {
    // FIFO1
    TTY_FIFO_1->ptr_read      = FIFO_1_dsp+HEADER_size;
    TTY_FIFO_1->ptr_write     = FIFO_1_dsp+HEADER_size;
    TTY_FIFO_1->buffer_start  = FIFO_1_dsp+HEADER_size;
    TTY_FIFO_1->buffer_length = FIFO_1_size;

    // FIFO2
    TTY_FIFO_2->ptr_read      = FIFO_2_dsp+HEADER_size;
    TTY_FIFO_2->ptr_write     = FIFO_2_dsp+HEADER_size;
    TTY_FIFO_2->buffer_start  = FIFO_2_dsp+HEADER_size;
    TTY_FIFO_2->buffer_length = FIFO_2_size;

    fifo2_write_addr    = TTY_FIFO_2->ptr_write;  // Init FIFO2 internal pointer
    fifo2_write_counter = 0;                      // Init write counter for FIFO2

    // FIFO3
    TTY_FIFO_3->ptr_read      = FIFO_3_dsp+HEADER_size;
    TTY_FIFO_3->ptr_write     = FIFO_3_dsp+HEADER_size;
    TTY_FIFO_3->buffer_start  = FIFO_3_dsp+HEADER_size;
    TTY_FIFO_3->buffer_length = FIFO_3_size;

    // FIFO4
    TTY_FIFO_4->ptr_read      = FIFO_4_dsp+HEADER_size;
    TTY_FIFO_4->ptr_write     = FIFO_4_dsp+HEADER_size;
    TTY_FIFO_4->buffer_start  = FIFO_4_dsp+HEADER_size;
    TTY_FIFO_4->buffer_length = FIFO_4_size;
  }

  /*-------------------------------------------------------*/
  /* TTY_init_API()                                        */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Initialize TTY MCU-DSP interface                      */
  /*-------------------------------------------------------*/
  void TTY_init_API(void)
  {
     API* ptr;
     UWORD16 i;

    #if (CODE_VERSION == SIMULATION)

       // Init API FIFOs address
       TTY_FIFO_1 = (T_TTY_FIFO_struct*) &header_fifo1;
       TTY_FIFO_2 = (T_TTY_FIFO_struct*) &header_fifo2;
       TTY_FIFO_3 = (T_TTY_FIFO_struct*) &header_fifo3;
       TTY_FIFO_4 = (T_TTY_FIFO_struct*) &header_fifo4;

       //buffer1
       TTY_FIFO_1->ptr_read      = (UWORD32)fifo1;
       TTY_FIFO_1->ptr_write     = (UWORD32)fifo1;
       TTY_FIFO_1->buffer_start  = (UWORD32)fifo1;
       TTY_FIFO_1->buffer_length = FIFO_1_size;
       //buffer2
       TTY_FIFO_2->ptr_read      = (UWORD32)fifo2;
       TTY_FIFO_2->ptr_write     = (UWORD32)fifo2;
       TTY_FIFO_2->buffer_start  = (UWORD32)fifo2;
       TTY_FIFO_2->buffer_length = FIFO_2_size;

       fifo2_write_addr    = TTY_FIFO_2->ptr_write;  // Init FIFO2 internal pointer
       fifo2_write_counter = 0;                      // Init write counter for FIFO2

       //buffer3
       TTY_FIFO_3->ptr_read      = (UWORD32)fifo3;
       TTY_FIFO_3->ptr_write     = (UWORD32)fifo3;
       TTY_FIFO_3->buffer_start  = (UWORD32)fifo3;
       TTY_FIFO_3->buffer_length = FIFO_3_size;
       //buffer4
       TTY_FIFO_4->ptr_read      = (UWORD32)fifo4;
       TTY_FIFO_4->ptr_write     = (UWORD32)fifo4;
       TTY_FIFO_4->buffer_start  = (UWORD32)fifo4;
       TTY_FIFO_4->buffer_length = FIFO_4_size;

   #else
         //init API FIFOs address
       TTY_FIFO_1 =(T_TTY_FIFO_struct*)FIFO_1_mcu;
       TTY_FIFO_2 =(T_TTY_FIFO_struct*)FIFO_2_mcu;
       TTY_FIFO_3 =(T_TTY_FIFO_struct*)FIFO_3_mcu;
       TTY_FIFO_4 =(T_TTY_FIFO_struct*)FIFO_4_mcu;

       TTY_fifo_initialize();

   #endif

       ptr = (API*) TTY_addr_dsp2mcu (TTY_FIFO_1->buffer_start);
       for (i=0; i<FIFO_1_size; i++)
         *ptr++ = 0;
       ptr = (API*) TTY_addr_dsp2mcu (TTY_FIFO_2->buffer_start);
       for (i=0; i<FIFO_2_size; i++)
         *ptr++ = 0;
       ptr = (API*) TTY_addr_dsp2mcu (TTY_FIFO_3->buffer_start);
       for (i=0; i<FIFO_3_size; i++)
         *ptr++ = 0;
       ptr = (API*) TTY_addr_dsp2mcu (TTY_FIFO_4->buffer_start);
       for (i=0; i<FIFO_4_size; i++)
         *ptr++ = 0;
  }

  /*-------------------------------------------------------*/
  /* TTY_fifo_chk_read()                                   */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Check the occupied place into the FIFO                */
  /*                                                       */
  /* Parameters:                                           */
  /* -----------                                           */
  /*    -FIFO:                                             */
  /*          Pointer on the FIFO used                     */
  /*                                                       */
  /*    -data_exchanged:                                   */
  /*          Pointer data to write                        */
  /*                                                       */
  /* Return:                                               */
  /* -------                                               */
  /*    -return_status:                                    */
  /*          TTY_FIFO_OK if no problem                    */
  /*          TTY_FIFO_OVERFLOW if overflow                */
  /*          TTY_FIFO_UNDERFLOW if underflow              */
  /*                                                       */
  /*    -data_exchanged:                                   */
  /*           Pointer on the data to read                 */
  /*-------------------------------------------------------*/
  WORD16 TTY_fifo_chk_read(T_TTY_FIFO_struct *FIFO,
                  WORD16            *data_exchanged)
  {
    WORD16 return_status =0; //omaps00090550
  #if (CODE_VERSION == SIMULATION)
    API  *save_write;
  #else
    API   save_write;
  #endif

    if (FIFO == TTY_FIFO_2)
    {
      save_write = fifo2_write_addr;
    }
    else
    {
      save_write = FIFO->ptr_write;
    }

    // Calculate unread data in the FIFO
    if(save_write == FIFO->ptr_read)
    {
      *data_exchanged = 0;
      return_status = TTY_FIFO_UNDERFLOW;
    }
    else if(save_write > FIFO->ptr_read)
    {
      *data_exchanged = save_write - FIFO->ptr_read;
      return_status = TTY_FIFO_OK;
    }
    else if(save_write < FIFO->ptr_read)
    {
      *data_exchanged = FIFO->buffer_length + save_write - FIFO->ptr_read;
      return_status = TTY_FIFO_OK;
    }

    return return_status;
  }

  /*-------------------------------------------------------*/
  /* TTY_fifo_chk_write                                    */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Check the free place into the FIFO                    */
  /*                                                       */
  /* Parameters:                                           */
  /* -----------                                           */
  /*    -FIFO:                                             */
  /*          Pointer on the FIFO used                     */
  /*                                                       */
  /*    -data_exchanged:                                   */
  /*          Pointer data to write                        */
  /*                                                       */
  /* Return:                                               */
  /* -------                                               */
  /*    -return_status:                                    */
  /*            TTY_FIFO_OK if no problem                  */
  /*            TTY_FIFO_OVERFLOW if overflow              */
  /*            TTY_FIFO_UNDERFLOW if underflow            */
  /*                                                       */
  /*    -data_exchanged:                                   */
  /*           Pointer on the data to read                 */
  /*-------------------------------------------------------*/
  WORD16 TTY_fifo_chk_write(T_TTY_FIFO_struct *FIFO,
                            WORD16            *data_exchanged)
  {
    WORD16   return_status;
  #if (CODE_VERSION == SIMULATION)
    API *save_read;
    API *temp_ptr;
  #else
    API  save_read;
    API  temp_ptr;
  #endif

    if (FIFO == TTY_FIFO_2)
    {
      temp_ptr = fifo2_write_addr;
    }
    else
    {
      temp_ptr = FIFO->ptr_write;
    }

    save_read = FIFO->ptr_read; /* save the value of the read pointer to avoid alteration */

    // Calculate available place for writing in FIFO
    if(temp_ptr == save_read)
    {
      *data_exchanged = FIFO->buffer_length - 1;
    }
    else if(temp_ptr > save_read)
    {
      *data_exchanged = FIFO->buffer_length + save_read - temp_ptr - 1;
    }
    else if(temp_ptr < save_read)
    {
      *data_exchanged = save_read - temp_ptr - 1;
    }

    //  Process status
    if(*data_exchanged == 0)
    {
      // No enough room to write data
      return_status = TTY_FIFO_OVERFLOW;
    }
    else
    {
      // Available to write data
      return_status = TTY_FIFO_OK;
    }

    return return_status;
  } //end TTY_fifo_chk_write()

  /*-------------------------------------------------------*/
  /* TTY_fifo_read()                                       */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Read from the FIFO 1 or 3                             */
  /*                                                       */
  /* Parameters:                                           */
  /* -----------                                           */
  /*    -FIFO:                                             */
  /*            Pointer on the FIFO used                   */
  /*                                                       */
  /*    -data_exchanged:                                   */
  /*            Pointer data to read                       */
  /*                                                       */
  /*    -max_size:                                         */
  /*            size to read                               */
  /*                                                       */
  /* Return:                                               */
  /* -------                                               */
  /*    -num_read:                                         */
  /*            number of read                             */
  /*-------------------------------------------------------*/
  UWORD16 TTY_fifo_read(T_TTY_FIFO_struct *FIFO,
                        WORD16 *data_exchanged,
                        UWORD16 max_size)
  {
#if (CODE_VERSION == SIMULATION)
    API *DSP_ADDR;
#else
    API  DSP_ADDR;
#endif
    UWORD16 num_read;

    //read the dsp@
    DSP_ADDR = FIFO->ptr_read;

    for(num_read=0; num_read < max_size; num_read++)
    {
      //put the mcu@ content to the output data pointer
      *data_exchanged = *(TTY_addr_dsp2mcu(DSP_ADDR));

      //if(DSP@>End)
      if(++DSP_ADDR >= (FIFO->buffer_start + FIFO->buffer_length))
      {
          DSP_ADDR = FIFO->buffer_start;
      }

      data_exchanged++;
    }

    // return new dsp@ to the struct
    FIFO->ptr_read = DSP_ADDR;

    return num_read;
  }

  /*-------------------------------------------------------*/
  /* TTY_fifo_write_buffer2()                              */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Write into FIFO 2                                     */
  /*                                                       */
  /* Parameters:                                           */
  /* -----------                                           */
  /*    -data_exchanged:                                   */
  /*            Pointer data to read                       */
  /*                                                       */
  /*    -max_size:                                         */
  /*            size to read                               */
  /*                                                       */
  /* Return:                                               */
  /* -------                                               */
  /*    -num_read:                                         */
  /*            number of write                            */
  /*-------------------------------------------------------*/
  UWORD16 TTY_fifo_write_buff2(WORD16 *data_exchanged,
                               UWORD16 max_size)
  {
    UWORD16 num_write;

    for(num_write = 0; num_write < max_size; num_write++)
    {
      //put the mcu@ content to the output data pointer
      *(TTY_addr_dsp2mcu(fifo2_write_addr)) = *data_exchanged;

      //if(DSP@>End)
      if(++fifo2_write_addr >= (TTY_FIFO_2->buffer_start + TTY_FIFO_2->buffer_length))
      {
            fifo2_write_addr = TTY_FIFO_2->buffer_start;
      }

      data_exchanged++;

      if(++fifo2_write_counter == 8)      // Check data write 8 times
      {
        fifo2_write_counter = 0;      // clear write counter
        TTY_FIFO_2->ptr_write = fifo2_write_addr;   //finally update shared write pointer
      }

      // In any case, at the end of the copy loop, the write pointer must be updated
      // Write counter also must be reset as the copy loop has modulo 8 granularity
      fifo2_write_counter = 0;
      TTY_FIFO_2->ptr_write = fifo2_write_addr;
    }

    return num_write;
  }


  /*-------------------------------------------------------*/
  /* TTY_fifo_write_buffer4()                              */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Write into FIFO 4                                     */
  /*                                                       */
  /* Parameters:                                           */
  /* -----------                                           */
  /*    -data_exchanged:                                   */
  /*            Pointer data to read                       */
  /*                                                       */
  /*    -max_size:                                         */
  /*            size to read                               */
  /*                                                       */
  /* Return:                                               */
  /* -------                                               */
  /*    -num_read:                                         */
  /*            number of write                            */
  /*-------------------------------------------------------*/
  UWORD16 TTY_fifo_write_buff4(WORD16 *data_exchanged,
                               UWORD16 max_size)
  {
#if (CODE_VERSION == SIMULATION)
    API *DSP_ADDR;
#else
    API  DSP_ADDR;
#endif
    UWORD16 num_write;

    //read the dsp@
    DSP_ADDR = TTY_FIFO_4->ptr_write;

    for(num_write = 0; num_write < max_size; num_write++)
    {
      //put the mcu@ content to the output data pointer
      *(TTY_addr_dsp2mcu(DSP_ADDR)) = *data_exchanged;

      //if(DSP@>End)
      if(++DSP_ADDR >= (TTY_FIFO_4->buffer_start + TTY_FIFO_4->buffer_length))
      {
          DSP_ADDR = TTY_FIFO_4->buffer_start;
      }

      data_exchanged++;
    }

    //return new dsp@ to the struct
    TTY_FIFO_4->ptr_write = DSP_ADDR;

    return num_write;
  }

#endif // L1_GTT

