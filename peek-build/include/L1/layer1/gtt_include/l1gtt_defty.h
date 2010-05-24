/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1GTT_DEFTY.H
 *
 *        Filename l1gtt_defty.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#ifndef _L1GTT_DEFTY_H_
#define _L1GTT_DEFTY_H_

#if (L1_GTT == 1)

  // GTT L1A/L1S COM structure
  //---------------------------

  typedef enum
  {
    TTY_L1_NOTTY=0,
    TTY_L1_VCO,
    TTY_L1_HCO,
    TTY_L1_ALL
  }T_L1_GTT_MODE;

  typedef struct
  {
    BOOL  start;
    BOOL  stop;
  }
  T_GTT_COMMAND;

  typedef struct
  {
    UWORD8  scen_enable;
    UWORD16 rate;
  }
  T_GTT_TEST;

  typedef struct
  {
    T_GTT_TEST    test;
    T_GTT_COMMAND command;
    UWORD16       baudot_keyboard_rate;
    T_L1_GTT_MODE l1_gtt_mode;
  }
  T_GTT_TASK;

  // FIFO structure
  //----------------

    #if (CODE_VERSION == SIMULATION)

      // DSP address do not exist --> 32-bit addresses
      typedef struct
      {
        API *ptr_read;         //API pointer to current data to read
        API *ptr_write;        //API pointer to free area to write to
        API *buffer_start;     //pointer to start
        UWORD16 buffer_length;    //length of buffer
      }
      T_TTY_FIFO_struct;

    #else

      typedef struct
      {
        API ptr_read;         //API pointer to current data to read
        API ptr_write;        //API pointer to free area to write to
        API buffer_start;     //pointer to start
        API buffer_length;    //length of buffer
      }
      T_TTY_FIFO_struct;

    #endif

  // GTT test data
  //--------------

  typedef struct
  {
    UWORD16 scen_index;
    UWORD16 scen_length;
    char   *scen_ptr;
    UWORD8  chars_to_push;
    UWORD16 frame_count;
  }
  T_GTT_TEST_L1S;

  typedef struct
  {
    UWORD16 fn_gtt_sync;
    UWORD16 fn_gtt_main_start;
    UWORD16 fn_gtt_main_end;
    UWORD16 fn_gtt_proc_flag;
    UWORD16 fn_gtt_error_count;
    UWORD16 odd_fifo3_count;
  }T_GTT_DEBUG;

#endif 


#endif //_L1GTT_DEFTY_H_
