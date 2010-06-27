/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AUDIO_CUST.C
 *
 *        Filename l1audio_cust.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

/************************************/
/* Include files...                 */
/************************************/

#include "l1_macro.h"
#include "l1_confg.h"

#if (AUDIO_TASK == 1)

  #include "l1_types.h"
  #include "sys_types.h"

  #if (CODE_VERSION == SIMULATION) && (AUDIO_SIMULATION)

    #include "tc_defs.h"

    #include <stdlib.h>
    #include <string.h>

    #include "iq.h"             // Debug / Init hardware  ("eva3.lib")
    #include "l1_ver.h"
    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
    #include "l1audio_varex.h"
    #include "l1audio_signa.h"

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

    #if (L1_AAC == 1)
      #include "l1aac_defty.h"
    #endif
    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"

    #include "l1_mftab.h"
    #include "l1_tabs.h"
    #include "l1_ctl.h"


    #include "l1_time.h"
    #include "l1_scen.h"

    #if L1_GPRS
      #include "l1p_cons.h"
      #include "l1p_msgt.h"
      #include "l1p_deft.h"
      #include "l1p_vare.h"
      #include "l1p_tabs.h"
      #include "l1p_macr.h"
      #include "l1p_sign.h"
    #endif

    #include "sim_cfg.h"
    #include "sim_cons.h"
    #include "sim_def.h"
    #include "sim_var.h"
    #include "sim_prot.h"

    #include "audio_sim_cons.h"
    #include "audio_sim_def.h"
    #include "mmi_simul.h"
    #include "audio_sim_var.h"

  #else
  // Layer1 and debug include files.

    #include <ctype.h>
    #include <string.h>
    #include <math.h>
    #include "l1_ver.h"
    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
    #include "l1audio_varex.h"
    #include "l1audio_signa.h"

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
    #include "tpudrv.h"       // TPU drivers.           ("eva3.lib")
    #include "l1_varex.h"

    #include "l1_proto.h"
    #include "l1_mftab.h"
    #include "l1_tabs.h"
    #include "mem.h"
    #include "Armio.h"
    #include "timer.h"
    #include "timer1.h"
    #include "dma.h"
    #include "inth.h"
    #include "ulpd.h"
    #include "rhea_arm.h"
    #include "clkm.h"         // Clockm  ("eva3.lib")
    #include "l1_ctl.h"

    #include "l1_time.h"
    #if L2_L3_SIMUL
      #include "l1_scen.h"
    #endif
    #if (AUDIO_L1_STANDALONE)
      #include "mmi_simul.h"
    #endif

    #if (OP_RIV_AUDIO == 1)
      #if (MELODY_E1) || (VOICE_MEMO) || (SPEECH_RECO) || (L1_EXT_AUDIO_MGT) || (MELODY_E2)
        #include "rv_general.h"
        #include "audio_api.h"
        #include "audio_structs_i.h"
        #include "audio_var_i.h"
        #include "audio_ffs_i.h"
        #include "audio_const_i.h"
        #include "audio_error_hdlr_i.h"
        #include "ffs/ffs_api.h"
        #include "audio_macro_i.h"
      #endif
    #endif

  #endif

  #include "l1_trace.h"
    //extern void L1_trace_string(char *s);
    //extern void L1_trace_char  (char s);

  /**************************************/
  /* Prototypes for L1 ASYNCH task      */
  /**************************************/
  #if (SPEECH_RECO)
    void Cust_srback_save_model  (UWORD8 database, UWORD8 index, API *RAM_address);
    void Cust_srback_save_speech    (UWORD8 database, UWORD8 index, UWORD16 *start_buffer, UWORD16 *stop_buffer, UWORD16 *start_speech, UWORD16 *stop_speech);
    void Cust_srback_load_model  (UWORD8 database, UWORD8 index, API *RAM_address);
  #endif
  UWORD8 Cust_get_pointer  (UWORD16 **ptr, UWORD16 *buffer_size, UWORD8 session_id);

  #if (L1_EXT_AUDIO_MGT)
    void Cust_ext_audio_mgt_hisr(void);
  #endif
  void l1a_bt_audio_noti_process(void);

  /**************************************/
  /* External prototypes                */
  /**************************************/
  #if (OP_RIV_AUDIO == 1)
    #if (SPEECH_RECO)
      extern  void audio_sr_error_trace(UINT8 error_id);
    #endif
  #endif


  /***************************************/
  /* Global variables for MP3 management */
  /***************************************/
  #if (L1_MP3 == 1)
    UWORD8  mp3_tempbuf_idx;
    UWORD16 Cust_get_pointer_mp3_last_buffer_size;
    BOOL    Cust_get_pointer_mp3_buffer_last;
  #endif

  /***************************************/
  /* Global variables for AAC management */
  /***************************************/
  #if (L1_AAC == 1)
    UWORD8  aac_tempbuf_idx;
    UWORD16 Cust_get_pointer_aac_last_buffer_size;
    BOOL    Cust_get_pointer_aac_buffer_last;
  #endif



  #if (SPEECH_RECO)
    /*-------------------------------------------------------*/
    /* Cust_srback_save_model()                              */
    /*-------------------------------------------------------*/
    /*                                                       */
    /* Parameters : database                                 */
    /*              index                                    */
    /*              RAM_address                              */
    /*                                                       */
    /* Return     : none                                     */
    /*                                                       */
    /* Description : save the model into a database.         */
    /*                                                       */
    /*-------------------------------------------------------*/
    void Cust_srback_save_model  (UWORD8 database, UWORD8 index, API *RAM_address)
    {
      #if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
        UWORD16 model_size;
        UWORD16 *ptr_dst;
        UWORD8  frame_size;

        // initialize the pointer to the database
        ptr_dst = sr_mmi_database[database][index].model;

        // look the size of the model in model frame unit (16 words unit)
        model_size = *RAM_address++;

        // save the header of the model
        *ptr_dst++ = model_size;

        while( (model_size != 0) &&
               (l1_srback_com.emergency_stop == FALSE) )
        {
          frame_size = SC_SR_MODEL_FRAME_SIZE;

          while ( (frame_size != 0) &&
                  (l1_srback_com.emergency_stop == FALSE) )
          {
            *ptr_dst++ = *RAM_address++;
            frame_size--;
          }

          model_size--;
        }
      #endif

      #if (OP_RIV_AUDIO == 1)
        T_FFS_FD ffs_fd;
        UWORD16  model_size;
        UWORD8   i;
        char     *p_model_path;

        if (p_audio_gbl_var->speech_reco.sr_enroll.state != AUDIO_SR_ENROLL_IDLE)
        {
          ffs_fd = p_audio_gbl_var->speech_reco.sr_enroll.sr_ffs_fd;
        }
        else
        if (p_audio_gbl_var->speech_reco.sr_update.state != AUDIO_SR_UPDATE_IDLE)
        {
          /* reach the path of the corresponding model */
          p_model_path = p_audio_gbl_var->speech_reco.sr_update.p_database;
          for (i=0; i<index; i++)
          {
            p_model_path += AUDIO_PATH_NAME_MAX_SIZE;
          }

          ffs_fd = ffs_open(p_model_path,
                      FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
          if ( ffs_fd <= 0)
          {
            audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_SAVED);
            return;
          }
        }

        /* look the size of the model in model frame unit (16 words unit) */
        model_size = ((*RAM_address)<<1);
        model_size *= SC_SR_MODEL_FRAME_SIZE;

        /* save the model in flash */
        if (ffs_write(ffs_fd, (void *)RAM_address, model_size) <= EFFS_OK)
        {
          audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_SAVED);
        }

        /* close the model file flash */
        if (ffs_close(ffs_fd) != EFFS_OK)
        {
          audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_SAVED);
        }
      #endif // OP_RIV_AUDIO
    }

    /*-------------------------------------------------------*/
    /* Cust_srback_save_speech()                             */
    /*-------------------------------------------------------*/
    /*                                                       */
    /* Parameters : database                                 */
    /*              index                                    */
    /*              start_buffer                             */
    /*              stop_buffer                              */
    /*              start_speech                             */
    /*              stop_speech                              */
    /*                                                       */
    /* Return     : none                                     */
    /*                                                       */
    /* Description : save the speech from a circular buffer  */
    /*               to a database. And add the end VM mask  */
    /*                                                       */
    /*-------------------------------------------------------*/
    void Cust_srback_save_speech    (UWORD8 database, UWORD8 index, UWORD16 *start_buffer, UWORD16 *stop_buffer, UWORD16 *start_speech, UWORD16 *stop_speech)
    {
      #if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
        UWORD16 *ptr_dst;

        // initialize the pointer to the database
        ptr_dst = sr_mmi_database[database][index].speech;

        // The speech samples can be in two positions in the circular buffer:

        // Position 1:
        //      [.............XXXXXXXXXXXXX.............]
        //      ^             ^            ^            ^
        //      |             |            |            |
        // start_buffer  start_speech  stop_speech  stop_buffer
        if (start_speech < stop_speech)
        {
          while ( (start_speech < stop_speech) &&
                  (l1_srback_com.emergency_stop == FALSE) )
          {
            *ptr_dst++ = *start_speech++;
          }

          // Write the end VM mask
          *ptr_dst = SC_VM_END_MASK;
        }
        else
        // Position 2:
        //      [XXXXXXXXXXXX..............XXXXXXXXXXXXX]
        //      ^            ^             ^            ^
        //      |            |             |            |
        // start_buffer  stop_speech  start_speech  stop_buffer
        {
          while ( (start_speech < stop_buffer) &&
                  (l1_srback_com.emergency_stop == FALSE) )
          {
            *ptr_dst++ = *start_speech++;
          }

          while ( (start_buffer < stop_speech) &&
                  (l1_srback_com.emergency_stop == FALSE) )
          {
            *ptr_dst++ = *start_buffer++;
          }

          // Write the end VM mask
          *ptr_dst = SC_VM_END_MASK;
        }
      #endif

      #if (OP_RIV_AUDIO == 1)
        T_FFS_FD  ffs_fd;
        UWORD16   speech_size;
        char      *p_model_path,
                  sr_speech_path[AUDIO_SR_PATH_NAME_MAX_SIZE];
        UWORD8    i;

        if (p_audio_gbl_var->speech_reco.sr_enroll.state !=AUDIO_SR_ENROLL_IDLE )
        {
          ffs_fd = p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd;
        }
        else
        if (p_audio_gbl_var->speech_reco.sr_update.state != AUDIO_SR_UPDATE_IDLE)
        {
          /* reach the path of the corresponding model */
          p_model_path = p_audio_gbl_var->speech_reco.sr_update.p_database;
          for (i=0; i<index; i++)
          {
            p_model_path += AUDIO_PATH_NAME_MAX_SIZE;
          }

          /* build the speech path name */
          speech_size = ( strlen(p_model_path)- 3);
          strncpy(sr_speech_path, p_model_path, speech_size);
          sr_speech_path[speech_size] = 0;

          ffs_fd = ffs_open(sr_speech_path,
                      FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
          if (ffs_fd <= 0)
          {
            audio_sr_error_trace(AUDIO_ENTITY_SPEECH_NO_SAVED);
            return;
          }
        }

        // The speech samples can be in two positions in the circular buffer:

        // Position 1:
        //      [.............XXXXXXXXXXXXX.............]
        //      ^             ^            ^            ^
        //      |             |            |            |
        // start_buffer  start_speech  stop_speech  stop_buffer
        if (start_speech < stop_speech)
        {
          /* Calculate the size of the speech file */
          speech_size = (stop_speech - start_speech)<<1;

          /* save the model in flash */
          if (ffs_write(ffs_fd, (void *)start_speech, speech_size) <= EFFS_OK)
          {
            audio_sr_error_trace(AUDIO_ENTITY_SPEECH_NO_SAVED);
          }
        }
        else
        // Position 2:
        //      [XXXXXXXXXXXX..............XXXXXXXXXXXXX]
        //      ^            ^             ^            ^
        //      |            |             |            |
        // start_buffer  stop_speech  start_speech  stop_buffer
        {
          /* Calculate the size of the speech file */
          speech_size = (stop_buffer - start_speech)<<1;

          /* save the model in flash */
          if (ffs_write(ffs_fd, (void *)start_speech, speech_size) <= EFFS_OK)
          {
            audio_sr_error_trace(AUDIO_ENTITY_SPEECH_NO_SAVED);
          }

          /* Calculate the size of the speech file */
          speech_size = (stop_speech - start_buffer)<<1;

          /* save the model in flash */
          if (ffs_write(ffs_fd, (void *)start_speech, speech_size) <= EFFS_OK)
          {
            audio_sr_error_trace(AUDIO_ENTITY_SPEECH_NO_SAVED);
          }
        }
          // Write the end VM mask
          speech_size = SC_VM_END_MASK;
          if (ffs_write(ffs_fd, (void *)(&speech_size), 2) <= EFFS_OK)
          {
            audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_SAVED);
          }

          /* close the speech file flash */
          if (ffs_close(ffs_fd) != EFFS_OK)
          {
            audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_SAVED);
          }
      #endif // OP_RIV_AUDIO
    }

    /*-------------------------------------------------------*/
    /* Cust_srback_load_model()                              */
    /*-------------------------------------------------------*/
    /*                                                       */
    /* Parameters : database                                 */
    /*              index                                    */
    /*              RAM_address                              */
    /*                                                       */
    /* Return     : none                                     */
    /*                                                       */
    /* Description : load the model into the API.            */
    /*                                                       */
    /*-------------------------------------------------------*/
    void Cust_srback_load_model  (UWORD8 database, UWORD8 index, API *RAM_address)
    {
      #if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
        UWORD16 model_size;
        UWORD16 *ptr_src;
        UWORD8  frame_size;

        // initialize the pointer to the database
        ptr_src = sr_mmi_database[database][index].model;

        // look the size of the model in model frame unit (16 words unit)
        model_size = *ptr_src++;

        // save the header of the model
        *RAM_address++ = model_size;

        while( (model_size != 0) &&
               (l1_srback_com.emergency_stop == FALSE) )
        {
          frame_size = SC_SR_MODEL_FRAME_SIZE;
          while ( (frame_size != 0) &&
                  (l1_srback_com.emergency_stop == FALSE) )
          {
            *RAM_address++ = *ptr_src++;
            frame_size--;
          }
          model_size--;
        }
      #endif

      #if (OP_RIV_AUDIO == 1)
        char      *p_model_path;
        T_FFS_FD  ffs_fd;
        UWORD8    i;

        if (p_audio_gbl_var->speech_reco.sr_reco.state !=AUDIO_SR_RECO_IDLE )
        {
          p_model_path = p_audio_gbl_var->speech_reco.sr_reco.p_database;
        }
        else
        if (p_audio_gbl_var->speech_reco.sr_update.state != AUDIO_SR_UPDATE_IDLE)
        {
          p_model_path = p_audio_gbl_var->speech_reco.sr_update.p_database;
        }

        /* reach the path of the corresponding model */
        for (i=0; i<index; i++)
        {
          p_model_path += AUDIO_PATH_NAME_MAX_SIZE;
        }

        /* open the model file flash */
        ffs_fd = ffs_open(p_model_path, FFS_O_RDONLY);
        if ( ffs_fd <= 0)
        {
          audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_LOAD);
          return;
        }

        /* download the model to the API buffer */
        if (ffs_read(ffs_fd, (void *)RAM_address, (SC_SR_MODEL_API_SIZE<<1)) <= EFFS_OK)
        {
          audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_LOAD);
        }

        /* close the model file flash */
        if (ffs_close(ffs_fd) != EFFS_OK)
        {
          audio_sr_error_trace(AUDIO_ENTITY_MODEL_NO_LOAD);
        }
      #endif // OP_RIV_AUDIO
    }
  #endif // SPEECH_RECO

  /*-------------------------------------------------------*/
  /* Cust_get_pointer                                      */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Parameters : ptr                                      */
  /*              buffer_size                              */
  /*              session_id                               */
  /*                                                       */
  /* Return     : error_id :                               */
  /*                DATA_AVAILABLE: 0, no error occured    */
  /*                SESSION_ERROR: 1, wrong session id     */
  /*                POINTER_ERROR: 2, wrong ptr            */
  /*                DATA_AVAIL_ERROR: 3, no more data      */
  /*                                     available         */
  /*                DATA_LAST:    4, last buffer available */
  /*              buffer_size                              */
  /*              ptr                                      */
  /*                                                       */
  /* Description :                                         */
  /* The L1 calls this function to request new data buffer */
  /* (requested size: buffer_size). This function returns  */
  /* the description of the new data buffer                */
  /* (start address: ptr and the size: buffer_size)        */
  /* Moreover, the L1 indicates the position of the last   */
  /* data used via the ptr argument.                       */
  /* Note that this function can be returns an error.      */
  /*                                                       */
  /*-------------------------------------------------------*/
  UWORD8 Cust_get_pointer (UWORD16 **ptr, UWORD16 *buffer_size, UWORD8 session_id)
  {
    #if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)

      UWORD8  database_id, word_index;

      switch (session_id & 0x0F)
      {
        #if (MELODY_E1)
          // Melody E1 0
          case 0:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&melody_0;
              *buffer_size = SC_MELODY_0_SCORE_SIZE>>1;
            }
            else
            {
              *buffer_size = SC_MELODY_0_SCORE_SIZE>>1;
            }

            return(DATA_AVAILABLE);
          }
//omaps00090550          break;

          // Melody E1 1
          case 1:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&melody_1;
              *buffer_size = SC_MELODY_1_SCORE_SIZE>>1;
            }
            else
            {
              *buffer_size = SC_MELODY_1_SCORE_SIZE>>1;
            }

            return(DATA_AVAILABLE);
          }
//omaps00090550           break;
        #endif // MELODY_E1
        #if (VOICE_MEMO)
          // Voice memo play
          case 2:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&voice_memo_buffer;
              *buffer_size = SC_VM_BUFFER_SIZE>>1;
            }
            else
            {
              *buffer_size = (SC_VM_BUFFER_SIZE>>1) + 1;
            }

            return(DATA_AVAILABLE);
          }
//omaps00090550           break;

          // Voice memo record
          case 3:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&voice_memo_buffer;
              *buffer_size = SC_VM_BUFFER_SIZE>>1;
            }
            else
            {
              *buffer_size = (SC_VM_BUFFER_SIZE>>1) + 1;
            }

            return(DATA_AVAILABLE);
          }
//omaps00090550           break;
        #endif // VOICE_MEMO
        #if (SPEECH_RECO)
          case 4:
          {
            database_id = 0;
            word_index = (session_id & 0xF0)>>4;
            *ptr = (UWORD16 *)&(sr_mmi_database[database_id][word_index].speech);

            *buffer_size = SC_SR_MMI_DB_SPEECH_SIZE;

            return(DATA_AVAILABLE);
          }
          break;

          case 5:
          {
            database_id = 1;
            word_index = (session_id & 0xF0)>>4;
            *ptr = (UWORD16 *)&(sr_mmi_database[database_id][word_index].speech);

            *buffer_size = SC_SR_MMI_DB_SPEECH_SIZE;

            return(DATA_AVAILABLE);
          }
          break;
        #endif // SPEECH_RECO
        #if (MELODY_E2 && FIR)
          // Melody E2
          // Tchaikowski
          case 6:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&Tchaikowski_danse_russe;
              *buffer_size = TCHAIKOWSKI_DANSE_RUSSE>>1;
            }
            else
            {
              *buffer_size = TCHAIKOWSKI_DANSE_RUSSE>>1;
            }

            return(DATA_AVAILABLE);
          }
          break;

          // Small melody
          case 7:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&Small_melody_E2;
              *buffer_size = SMALL_MELODY_E2>>1;
            }
            else
            {
              *buffer_size = SMALL_MELODY_E2>>1;
            }

            return(DATA_AVAILABLE);
          }
          break;

          // CPU load
          case 8:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&Cpu_load_E2;
              *buffer_size = CPU_LOAD_E2>>1;
            }
            else
            {
              *buffer_size = CPU_LOAD_E2>>1;
            }

            return(DATA_AVAILABLE);
          }
          break;

          // USA hymn
          case 9:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&Usa_E2;
              *buffer_size = USA_E2>>1;
            }
            else
            {
              *buffer_size = USA_E2>>1;
            }

            return(DATA_AVAILABLE);
          }
          break;
        #endif // MELODY_E2
        #if (L1_VOICE_MEMO_AMR)
          // Voice memo amr play
          case 10:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&voice_memo_buffer;
              *buffer_size = SC_VM_BUFFER_SIZE>>1;
            }
            else
            {
              *buffer_size = (SC_VM_BUFFER_SIZE>>1) + 1;
            }

            return(DATA_AVAILABLE);
          }
//omaps00090550           break;

          // Voice memo record
          case 11:
          {
            if (*ptr == NULL)
            {
              *ptr = (UWORD16 *)&voice_memo_buffer;
              *buffer_size = SC_VM_BUFFER_SIZE>>1;
            }
            else
            {
              *buffer_size = (SC_VM_BUFFER_SIZE>>1) + 1;
            }

            return(DATA_AVAILABLE);
          }
//omaps00090550           break;
        #endif // L1_VOICE_MEMO_AMR

        #if (L1_MIDI == 1)
          // MIDI
          case 12:
          {
            extern struct MIDI_FILE midi_file;
            UWORD8 **ptr_char=(UWORD8 **)ptr;
            static UWORD8 *last_ptr;

            if(*ptr_char==NULL)
            {
              midi_file_offset=0;
              *ptr_char=&(midi_file.midi_file_buffer[0]);
              last_ptr=*ptr_char;

              // All MIDI file available due to SMF1 format constraints
              // For SMF0, *buffer_size should be set to MIDI_BUFFER_SIZE
              *buffer_size=midi_file.midi_file_size;
              return DATA_LAST;
            }
            else
            {
              *buffer_size=MIDI_BUFFER_SIZE;
              *ptr_char=last_ptr+MIDI_BUFFER_SIZE;

              // Check if pointer is out of bounds
              if(*ptr_char > &(midi_file.midi_file_buffer[0])+midi_file.midi_file_size-1)
              {
                *ptr_char=&(midi_file.midi_file_buffer[0]);
                return DATA_AVAIL_ERROR;
              }

              // Check if it's the last valid buffer
              if((*ptr_char+*buffer_size) > &(midi_file.midi_file_buffer[0])+midi_file.midi_file_size-1)
              {
                *buffer_size=MIDI_BUFFER_SIZE-(UWORD16)(*ptr_char+MIDI_BUFFER_SIZE-&(midi_file.midi_file_buffer[0])-midi_file.midi_file_size+1);

                // Update offset in the MIDI file
                midi_file_offset+=*ptr_char-last_ptr;

                last_ptr=*ptr_char;

                return DATA_LAST;
              }
              else
              {
                // Update offset in the MIDI file
                midi_file_offset+=*ptr_char-last_ptr;

                last_ptr=*ptr_char;

                return DATA_AVAILABLE;
              }
            }
          }
        #endif  // L1_MIDI

#if L1_PCM_EXTRACTION
        case 13:
        {
          *ptr = (UWORD16 *)&pcm_download_buffer;
          *buffer_size = SC_PCM_DOWNLOAD_BUFFER_SIZE;
          return DATA_AVAILABLE;
        }
//omaps00090550        break;

        case 14:
        {
          *ptr = (UWORD16 *)&pcm_upload_buffer;
          *buffer_size = SC_PCM_UPLOAD_BUFFER_SIZE;
          return DATA_AVAILABLE;
        }
//omaps00090550        break;
#endif /* L1_PCM_EXTRACTION */
        default:
        {
          return(SESSION_ERROR);
        }
//omaps00090550         break;
      }
    #endif

    #if (OP_RIV_AUDIO == 1)

      #if (MELODY_E1) || (VOICE_MEMO) || (MELODY_E2) || (L1_VOICE_MEMO_AMR) || (L1_EXT_AUDIO_MGT)
        UWORD8   index_l1, *current_ptr_8;
        UWORD16  *current_ptr, *size_ptr, *end_ptr;
        UWORD32  current, end;
        T_AUDIO_FFS_SESSION *p_session;

      #if (L1_AUDIO_DRIVER == 1)
        T_AUDIO_DRIVER_SESSION *p_driver_session = NULL;
        UINT8 channel_id;
        xSignalHeaderRec *msg;

        /* special handling for features in driver */
        if ((session_id == AUDIO_VM_AMR_RECORD_SESSION_ID) ||
            (session_id == AUDIO_VM_AMR_PLAY_SESSION_ID) ||
            (session_id == AUDIO_EXT_MIDI_SESSION_ID))
        {
          channel_id = 0;
          /* find active channel_id associated to session_id */
          while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL)&&
                ((p_audio_gbl_var->audio_driver_session[channel_id].session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)||
                 (p_audio_gbl_var->audio_driver_session[channel_id].session_req.session_id != session_id)) )
          {
            channel_id++;
          }
          /* get driver session */
          if(channel_id < AUDIO_DRIVER_MAX_CHANNEL)
          p_driver_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);

          /* first buffer, index_l1 = 0 */
        if(p_driver_session != NULL)
         {
          if (*ptr == NULL)
          {
            *ptr = (UWORD16 *)(p_driver_session->session_info.buffer[0].p_start_pointer);
            *buffer_size = (p_driver_session->session_info.buffer[0].size >> 1);

            return(DATA_AVAILABLE);
          }
          /* following buffers */
          else
          {
            /* prepare notification */
            msg = os_alloc_sig(sizeof(T_L1_AUDIO_DRIVER_IND));
            DEBUGMSG(status,NU_ALLOC_ERR)
            msg->SignalCode = L1_AUDIO_DRIVER_IND;
            ((T_L1_AUDIO_DRIVER_IND *)msg->SigP)->channel_id = channel_id;
            ((T_L1_AUDIO_DRIVER_IND *)msg->SigP)->p_buffer   = NULL;

            /* fill message with current buffer for record */
            index_l1 = p_driver_session->session_info.index_l1;
            if (session_id == AUDIO_VM_AMR_RECORD_SESSION_ID)
              ((T_L1_AUDIO_DRIVER_IND *)msg->SigP)->p_buffer =
                (UWORD16 *)p_driver_session->session_info.buffer[index_l1].p_start_pointer;

            /* give new buffer = index_l1++ % nb_buffer */
            p_driver_session->session_info.index_l1++;
            if ((p_driver_session->session_info.index_l1) == (p_driver_session->session_req.nb_buffer))
              p_driver_session->session_info.index_l1 =(UINT8)( 0);//omaps

            /* Update new current buffer parameters */
            index_l1 = (UWORD8)(p_driver_session->session_info.index_l1);//oamps
            *ptr = (UWORD16 *)(p_driver_session->session_info.buffer[index_l1].p_start_pointer);
            *buffer_size = (p_driver_session->session_info.buffer[index_l1].size >> 1);

            // Send confirmation message...
            os_send_sig(msg, L1C1_QUEUE);
            DEBUGMSG(status,NU_SEND_QUEUE_ERR)

            return (DATA_AVAILABLE);
          }
	  	 }
	  	 return(DATA_AVAIL_ERROR);
        }
        /* Other features */
        else
        {
      #endif // L1_AUDIO_DRIVER
        p_session = &(p_audio_gbl_var->audio_ffs_session[session_id]);

        switch (p_session->session_info.cust_get_pointer_state)
        {
          case AUDIO_CUST_GET_POINTER_INIT:
          {
            /* the first buffer, the index_l1 is already OK so the index_l1 doesn't change */
            index_l1 =
              p_session->session_info.index_l1;
            *ptr =
              (UWORD16 *)(p_session->session_info.buffer[index_l1].p_start_pointer);
            *buffer_size =
              (p_session->session_info.buffer[index_l1].size>>1);


            p_session->session_info.cust_get_pointer_state =
              AUDIO_CUST_GET_POINTER_NORMAL;
            break;
          }
          case AUDIO_CUST_GET_POINTER_NORMAL:
          {
            /* in loopback mode, when the melody restarts, the L1 pointer management is different */
            if ( (*buffer_size == 0) &&
                 (p_session->session_req.loop_mode == TRUE) )
            {
              p_session->session_info.cust_get_pointer_state =
                AUDIO_CUST_GET_POINTER_LOOP;
              break;
            }

            /* The index_l1 needs to be changed */
            p_session->session_info.index_l1++;
            if (p_session->session_info.index_l1
              == AUDIO_MAX_FFS_BUFFER_PER_SESSION)
            {
              p_session->session_info.index_l1 = 0;
            }

            index_l1 =
              p_session->session_info.index_l1;
            *ptr =
              (UWORD16 *)(p_session->session_info.buffer[index_l1].p_start_pointer);
            *buffer_size =
              (p_session->session_info.buffer[index_l1].size>>1);

            break;
          }
          case AUDIO_CUST_GET_POINTER_LOOP:
          {
            /* in loopback mode, the next melody data is contiguous to the first */
            /* if it's the end of the buffer , we use the next buffer */
            index_l1 =
              p_session->session_info.index_l1;

            current_ptr = (UWORD16 *)(*ptr);
            current_ptr_8 = (UWORD8 *)(*ptr);
            end_ptr     = (UWORD16 *)(p_session->session_info.buffer[index_l1].p_stop_pointer);

            if ( current_ptr >= end_ptr )
            {
              p_session->session_info.index_l1++;
              if (p_session->session_info.index_l1
                  == AUDIO_MAX_FFS_BUFFER_PER_SESSION)
              {
                p_session->session_info.index_l1 = 0;
              }

              index_l1 =
                p_session->session_info.index_l1;
              *ptr =
                (UWORD16 *)(p_session->session_info.buffer[index_l1].p_start_pointer);
              *buffer_size =
                (p_session->session_info.buffer[index_l1].size>>1);
            }
            else
            {
              // Realign the pointer to the next 16 bit in case of melody E2
              current = (UWORD32)current_ptr_8;
              if (current & 0x01)
              {
                current_ptr_8++;
                *ptr = (UWORD16 *)current_ptr_8;
              }

              end = (UWORD32)end_ptr;
              current = (UWORD32)current_ptr;

              *buffer_size = (UWORD16)((end-current)>>1);
            }

            p_session->session_info.cust_get_pointer_state =
              AUDIO_CUST_GET_POINTER_NORMAL;
            break;
          }
        } /* switch(p_session->session_info.cust_get_pointer_state) */

        return(DATA_AVAILABLE);
      #if (L1_AUDIO_DRIVER == 1)
        } // added
      #endif
      #endif // MELODY_E1 || VOICE_MEMO || MELODY_E2
    #endif // OP_RIV_AUDIO
  }

#if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
/*-------------------------------------------------------*/
/* Cust_get_next_buffer_status                           */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters :                                          */
/*                                                       */
/* Return     :                                          */
/*                                                       */
/* Description :                                         */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 Cust_get_next_buffer_status(void)
{
   return (FALSE);
}

#endif

#if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
/*-------------------------------------------------------*/
/* Cust_get_pointer_next_buffer                          */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : ptr                                      */
/*              buffer_size                              */
/*              session_id                               */
/*                                                       */
/* Return     : error_id :                               */
/*                DATA_AVAILABLE: 0, no error occured    */
/*                SESSION_ERROR: 1, wrong session id     */
/*                POINTER_ERROR: 2, wrong ptr            */
/*                DATA_AVAIL_ERROR: 3, no more data      */
/*                                     available         */
/*                DATA_LAST: 4, returned buffer is the   */
/*                              last                     */
/*              buffer_size                              */
/*              ptr                                      */
/*                                                       */
/* Description :                                         */
/* This function acts the same as Cust_get_pointer() but */
/* does not send notification to upper layers to free    */
/* current buffer                                        */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 Cust_get_pointer_next_buffer(UWORD16 **ptr,
                                    UWORD16 *buffer_size,
                                    UWORD8 session_id)
{
#if (L1_MP3 == 1)
  extern UWORD32 mp3_file_size;
#endif
#if (L1_AAC == 1)
  extern UWORD32 aac_file_size;
#endif

  switch(session_id&0x0F)
  {
#if (L1_MP3 == 1)
    case 13:
    {
      if(*ptr==NULL)
      {
        // Reset flag
        Cust_get_pointer_mp3_buffer_last=FALSE;

        // Set pointer to first buffer with corresponding size
        mp3_tempbuf_idx=0;
        *ptr=mp3_tempbuf0;
        *buffer_size=C_MP3_L1STANDALONE_BUFFER_SIZE;

        Cust_get_pointer_mp3_last_buffer_size=C_MP3_L1STANDALONE_BUFFER_SIZE;

#if (CODE_VERSION == NOT_SIMULATION)
        if(mp3_file_size<C_MP3_L1STANDALONE_BUFFER_SIZE)
        {
          *buffer_size=(UWORD16)mp3_file_size;
          Cust_get_pointer_mp3_buffer_last=TRUE;
          return DATA_LAST;
        }
#endif

        return DATA_AVAILABLE;
      }
      else
      {
        switch(mp3_tempbuf_idx)
        {
          case 0:
            *ptr=mp3_tempbuf1;
          break;

          case 1:
            *ptr=mp3_tempbuf0;
          break;
        }

        // Swap buffer index
        mp3_tempbuf_idx^=1;

        *buffer_size=Cust_get_pointer_mp3_last_buffer_size;

        if(Cust_get_pointer_mp3_buffer_last==FALSE)
          return DATA_AVAILABLE;
        else
        {
          if(l1a_l1s_com.mp3_task.parameters.loopback==TRUE)
            Cust_get_pointer_mp3_buffer_last=FALSE;

          return DATA_LAST;
        }
      }
    }
//omaps00090550     break;
#endif    // L1_MP3

#if (L1_AAC == 1)
    case 14: // session id for AAC
    {
      if(*ptr==NULL)
      {
        // Reset flag
        Cust_get_pointer_aac_buffer_last=FALSE;

        // Set pointer to first buffer with corresponding size
        aac_tempbuf_idx=0;
        *ptr=aac_tempbuf0;
        *buffer_size=C_AAC_L1STANDALONE_BUFFER_SIZE;

        Cust_get_pointer_aac_last_buffer_size=C_AAC_L1STANDALONE_BUFFER_SIZE;

#if (CODE_VERSION == NOT_SIMULATION)
        if(aac_file_size<C_AAC_L1STANDALONE_BUFFER_SIZE)
        {
          *buffer_size=(UWORD16)aac_file_size;
          Cust_get_pointer_aac_buffer_last=TRUE;
          return DATA_LAST;
        }
#endif

        return DATA_AVAILABLE;
      }
      else
      {
        switch(aac_tempbuf_idx)
        {
          case 0:
            *ptr=aac_tempbuf1;
          break;

          case 1:
            *ptr=aac_tempbuf0;
          break;
        }

        // Swap buffer index
        aac_tempbuf_idx^=1;

        *buffer_size=Cust_get_pointer_aac_last_buffer_size;

        if(Cust_get_pointer_aac_buffer_last==FALSE)
          return DATA_AVAILABLE;
        else
        {
          if(l1a_l1s_com.aac_task.parameters.loopback==TRUE)
            Cust_get_pointer_aac_buffer_last=FALSE;

          return DATA_LAST;
        }
      }
    }
//omaps00090550     break;
#endif    // L1_AAC

    default:
    {
      return SESSION_ERROR;
    }
//omaps00090550     break;
  }
}
#endif    // (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)


#if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
/*------------------------------------------------------------------*/
/* Cust_get_pointer_notify                                          */
/*------------------------------------------------------------------*/
/*                                                                  */
/* Parameters : ptr                                                 */
/*              session_id                                          */
/*                                                                  */
/* Return     : n/a                                                 */
/*                                                                  */
/* Description :                                                    */
/* This function sends notification to upper layers to              */
/* fill next buffer.                                                */
/* This function should be used with Cust_get_pointer_next_buffer() */
/*                                                                  */
/*------------------------------------------------------------------*/
void Cust_get_pointer_notify(UWORD8 session_id)
{
  switch(session_id&0x0F)
  {
#if (L1_MP3 == 1)
    case 13:
    {
#if (CODE_VERSION == NOT_SIMULATION)
      UWORD32 size;
      UWORD8 *dst;
      extern UWORD32 mp3_file_size;

      // Check if file is ending
      if(mp3_file_offset+C_MP3_L1STANDALONE_BUFFER_SIZE > mp3_file_size)
      {
        size=mp3_file_size-mp3_file_offset;
        Cust_get_pointer_mp3_buffer_last=TRUE;
      }
      else
      {
        size=C_MP3_L1STANDALONE_BUFFER_SIZE;
        Cust_get_pointer_mp3_buffer_last=FALSE;
      }

      switch(mp3_tempbuf_idx)
      {
        case 0:
        {
          // L1 is working on buffer 0 -> fill buffer 1
          dst=(UWORD8 *)mp3_tempbuf1;
        }
        break;

        case 1:
        {
          // L1 is working on buffer 1 -> fill buffer 0
          dst=(UWORD8 *)mp3_tempbuf0;
        }
        break;
      }

      memcpy(dst,(UWORD8 *)mp3_file+mp3_file_offset,size);

      // Update MP3 read offset
      mp3_file_offset+=C_MP3_L1STANDALONE_BUFFER_SIZE;

      // Rewind if loopback mode is on
      if((l1a_l1s_com.mp3_task.parameters.loopback==TRUE) &&
         (Cust_get_pointer_mp3_buffer_last==TRUE))
        mp3_file_offset=0;

      // Keep track of buffer size
      Cust_get_pointer_mp3_last_buffer_size=(UWORD16)size;

#else   // CODE_VERSION == NOT_SIMULATION
      UWORD16 i;
      UWORD8 *dst;
      switch(mp3_tempbuf_idx)
      {
        case 0:
          dst=(UWORD8 *)mp3_tempbuf1;
        break;
        case 1:
          dst=(UWORD8 *)mp3_tempbuf0;
        break;
      }
      for(i=0; i<C_MP3_L1STANDALONE_BUFFER_SIZE/2; i++)
      {
        dst[2*i]  =0x12;
        dst[2*i+1]=0x24;
      }
#endif
    }
    break;
#endif    // L1_MP3

#if (L1_AAC == 1)
    case 14:
    {
#if (CODE_VERSION == NOT_SIMULATION)
      UWORD32 size;
      UWORD8 *dst;
      extern UWORD32 aac_file_size;

      // Check if file is ending
      if(aac_file_offset+C_AAC_L1STANDALONE_BUFFER_SIZE > aac_file_size)
      {
        size=aac_file_size-aac_file_offset;
        Cust_get_pointer_aac_buffer_last=TRUE;
      }
      else
      {
        size=C_AAC_L1STANDALONE_BUFFER_SIZE;
        Cust_get_pointer_aac_buffer_last=FALSE;
      }

      switch(aac_tempbuf_idx)
      {
        case 0:
        {
          // L1 is working on buffer 0 -> fill buffer 1
          dst=(UWORD8 *)aac_tempbuf1;
        }
        break;

        case 1:
        {
          // L1 is working on buffer 1 -> fill buffer 0
          dst=(UWORD8 *)aac_tempbuf0;
        }
        break;
      }

      memcpy(dst,(UWORD8 *)aac_file+aac_file_offset,size);

      // Update AAC read offset
      aac_file_offset+=C_AAC_L1STANDALONE_BUFFER_SIZE;

      // Rewind if loopback mode is on
      if((l1a_l1s_com.aac_task.parameters.loopback==TRUE) &&
         (Cust_get_pointer_aac_buffer_last==TRUE))
        aac_file_offset=0;

      // Keep track of buffer size
      Cust_get_pointer_aac_last_buffer_size=(UWORD16)size;

#else   // CODE_VERSION == NOT_SIMULATION
      UWORD16 i;
      UWORD8 *dst;
      switch(aac_tempbuf_idx)
      {
        case 0:
          dst=(UWORD8 *)aac_tempbuf1;
        break;
        case 1:
          dst=(UWORD8 *)aac_tempbuf0;
        break;
      }
      for(i=0; i<C_AAC_L1STANDALONE_BUFFER_SIZE/2; i++)
      {
        dst[2*i]  =0x10;
        dst[2*i+1]=0x20;
      }
#endif
    }
    break;
#endif    // L1_AAC

  }  // switch(session_id&0x0F)
}
#endif    // (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)


  #if (MELODY_E2 && FIR)
    /*-------------------------------------------------------*/
    /* Cust_audio_melody_E2_load_instrument                  */
    /*-------------------------------------------------------*/
    /*                                                       */
    /* Parameters : customer_instrument_id:                  */
    /*                Identifier of the instrument           */
    /*              API_address                              */
    /*                address where the insturment           */
    /*                is downloaded                          */
    /*              allowed_size                             */
    /*                maximum size of the instrument         */
    /*                                                       */
    /* Return     : instrument_size :                        */
    /*                size of the insturment downloaded in   */
    /*                16-bit word                            */
    /*                                                       */
    /* Description :                                         */
    /* The audio background task calls this function to      */
    /* request to the MMI to download a new instrument       */
    /* description at the API_address.                       */
    /*                                                       */
    /*-------------------------------------------------------*/
    UWORD16 Cust_audio_melody_E2_load_instrument (UWORD8  customer_instrument_id,
                                                  API     *API_address,
                                                  UWORD16 allowed_size)
    {
      #if (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)
        UWORD16 instrument_size = 0, i, *instrument_ptr;

        // default.lsi file:
        //    0   piano31  0x0
        //    1   piano15  0x1
        //    2   pia_811  0x2
        //    3   eba_2536 0x3
        //    4   tsax_255 0x4
        //    5   asax_124 0x5
        //    6   clar_121 0x6
        //    7   clar_130 0x7
        //    8   stdr_515 0x8
        //    9   timp_661 0x9
        //   10   agog_406 0xA
        //   11   wood_110 0xB
        //   12   vib_3836 0xC
        //   13   xylo_120 0xD
        //   14   xylo_315 0xE
        //   15   xylo_516 0xF
        //   16   kali_215 0x10
        //   17   viol_231 0x11
        //   18   viol_215 0x12
        //   19   viol_211 0x13
        //   20   viol_108 0x14
        //   21   flut_436 0x15
        //   22   flut_308 0x16
        //   23   porg_131 0x17
        //   24   acco_211 0x18
        //   25   stng_231 0x19
        //   26   stng_058 0x1A
        //   27   ldsa_131 0x1B
        //   28   sin7     0x1C
        //   29   egu_3036 0x1D
        //   30   jgui_215 0x1E
        //   31   banj_315 0x1F
        //   32   trum_231 0x20
        //   33   tromb31  0x21
        //   34   cho_2116 0x22
        //   35   vooh_331 0x23
        //   36   crc_1210 0x24
        //   37   chc_1210 0x25
        //   38   ohc_1610 0x26
        //   39   lbon_121 0x27
        //   40   esn_0210 0x28
        //   41   crc_0210 0x29
        //   42   ric_0210 0x2A
        //   43   bdm_0122 0x2B
        //   44   vib_0415 0x2C
        //   45   ngu_0343 0x2D
        //   46   sgu_0230 0x2E
        //   47   xyl_0315 0x2F

        switch (customer_instrument_id)
        {
          case 0:
          // Piano31.mwa
          {
            instrument_size = MELODY_E2_PIANO31_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_piano31[0]);
            break;
          }
          case 1:
          // Piano15.mwa
          {
            instrument_size = MELODY_E2_PIANO15_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_piano15[0]);
            break;
          }
          case 2:
          // Pia_811.mwa
          {
            instrument_size = MELODY_E2_PIA_811_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_pia_811[0]);
            break;
          }
          case 3:
          // eba_2536.mwa
          {
            instrument_size = MELODY_E2_EBA_2536_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_eba_2536[0]);
            break;
          }
          case 4:
          // tsax_255.mwa
          {
            instrument_size = MELODY_E2_TSAX_255_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_tsax_255[0]);
            break;
          }
          case 5:
          // asax_124.mwa
          {
            instrument_size = MELODY_E2_ASAX_124_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_asax_124[0]);
            break;
          }
          case 6:
          // clar_121.mwa
          {
            instrument_size = MELODY_E2_CLAR_121_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_clar_121[0]);
            break;
          }
          case 7:
          // clar_130.mwa
          {
            instrument_size = MELODY_E2_CLAR_130_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_clar_130[0]);
            break;
          }
          case 8:
          // stdr_515.mwa
          {
            instrument_size = MELODY_E2_STDR_515_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_stdr_515[0]);
            break;
          }
          case 9:
          // timp_661.mwa
          {
            instrument_size = MELODY_E2_TIMP_661_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_timp_661[0]);
            break;
          }
          case 10:
          // agog_406.mwa
          {
            instrument_size = MELODY_E2_AGOG_406_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_agog_406[0]);
            break;
          }
          case 11:
          // wood_110.mwa
          {
            instrument_size = MELODY_E2_WOOD_110_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_wood_110[0]);
            break;
          }
          case 12:
          // vib_3836.mwa
          {
            instrument_size = MELODY_E2_VIB_3836_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_vib_3836[0]);
            break;
          }
          case 13:
          // xylo_120.mwa
          {
            instrument_size = MELODY_E2_XYLO_120_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_xylo_120[0]);
            break;
          }
          case 14:
          // xylo_315.mwa
          {
            instrument_size = MELODY_E2_XYLO_315_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_xylo_315[0]);
            break;
          }
          case 15:
          // xylo_516.mwa
          {
            instrument_size = MELODY_E2_XYLO_516_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_xylo_516[0]);
            break;
          }
          case 16:
          // kali_215.mwa
          {
            instrument_size = MELODY_E2_KALI_215_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_kali_215[0]);
            break;
          }
          case 17:
          // viol_231
          {
            instrument_size = MELODY_E2_VIOL_231_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_viol_231[0]);
            break;
          }
          case 18:
          // viol_215.mwa
          {
            instrument_size = MELODY_E2_VIOL_215_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_viol_215[0]);
            break;
          }
          case 19:
          // viol_211.mwa
          {
            instrument_size = MELODY_E2_VIOL_211_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_viol_211[0]);
            break;
          }
          case 20:
          // viol_108.mwa
          {
            instrument_size = MELODY_E2_VIOL_108_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_viol_108[0]);
            break;
          }
          case 21:
          // flut_436
          {
            instrument_size = MELODY_E2_FLUT_436_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_flut_436[0]);
            break;
          }
          case 22:
          // flut_308.mwa
          {
            instrument_size = MELODY_E2_FLUT_308_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_flut_308[0]);
            break;
          }
          case 23:
          // porg_131
          {
            instrument_size = MELODY_E2_PORG_131_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_porg_131[0]);
            break;
          }
          case 24:
          // acco_211.mwa
          {
            instrument_size = MELODY_E2_ACCO_211_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_acco_211[0]);
            break;
          }
          case 25:
          // stng_231
          {
            instrument_size = MELODY_E2_STNG_231_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_stng_231[0]);
            break;
          }
          case 26:
          // stng_058.mwa
          {
            instrument_size = MELODY_E2_STNG_058_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_stng_058[0]);
            break;
          }
          case 27:
          // ldsa_131
          {
            instrument_size = MELODY_E2_LDSA_131_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_ldsa_131[0]);
            break;
          }
          case 28:
          // sin7.mwa
          {
            instrument_size = MELODY_E2_SIN7_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_sin7[0]);
            break;
          }
          case 29:
          // Egu_3036.mwa
          {
            instrument_size = MELODY_E2_EGU_3036_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_egu_3036[0]);
            break;
          }
          case 30:
          // Jgui_215.mwa
          {
            instrument_size = MELODY_E2_JGUI_215_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_jgui_215[0]);
            break;
          }
          case 31:
          // Banj_315.mwa
          {
            instrument_size = MELODY_E2_BANJ_315_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_banj_315[0]);
            break;
          }

          case 32:
          // trum_231
          {
            instrument_size = MELODY_E2_TRUM_231_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_trum_231[0]);
            break;
          }
          case 33:
          // tromb31
          {
            instrument_size = MELODY_E2_TROMB31_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_tromb31[0]);
            break;
          }
          case 34:
          // cho_2116
          {
            instrument_size = MELODY_E2_CHO_2116_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_cho_2116[0]);
            break;
          }
          case 35:
          // vooh_331.mwa
          {
            instrument_size = MELODY_E2_VOOH_331_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_vooh_331[0]);
            break;
          }
          case 36:
          // crc_1210.mwa
          {
            instrument_size = MELODY_E2_CRC_1210_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_crc_1210[0]);
            break;
          }
          case 37:
          // chc_1210.mwa
          {
            instrument_size = MELODY_E2_CHC_1210_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_chc_1210[0]);
            break;
          }
          case 38:
          // ohc_1610
          {
            instrument_size = MELODY_E2_OHC_1610_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_ohc_1610[0]);
            break;
          }
          case 39:
          // lbon_121
          {
            instrument_size = MELODY_E2_LBON_121_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_lbon_121[0]);
            break;
          }
          case 40:
          // esn_0210
          {
            instrument_size = MELODY_E2_ESN_0210_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_esn_0210[0]);
            break;
          }
          case 41:
          // crc_0210
          {
            instrument_size = MELODY_E2_CRC_0210_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_crc_0210[0]);
            break;
          }
          case 42:
          // ric_0210
          {
            instrument_size = MELODY_E2_RIC_0210_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_ric_0210[0]);
            break;
          }
          case 43:
          // bdm_0122
          {
            instrument_size = MELODY_E2_BDM_0122_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_bdm_0122[0]);
            break;
          }
          case 44:
          // vib_0415
          {
            instrument_size = MELODY_E2_VIB_0415_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_vib_0415[0]);
            break;
          }
          case 45:
          // ngu_0343
          {
            instrument_size = MELODY_E2_NGU_0343_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_ngu_0343[0]);
            break;
          }
          case 46:
          // sgu_0230
          {
            instrument_size = MELODY_E2_SGU_0230_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_sgu_0230[0]);
            break;
          }
          case 47:
          // xyl_0315
          {
            instrument_size = MELODY_E2_XYL_0315_MWA_SIZE;
            instrument_ptr = (UWORD16 *)(&melody_E2_xyl_0315[0]);
            break;
          }
        }

        i = instrument_size;
        while ( (i != 0) &&
                (l1a_l1s_com.melody0_e2_task.parameters.emergency_stop == FALSE) )
        {
          *API_address++ = *instrument_ptr++;
          i--;
        }

        return(instrument_size);
      #endif  // (AUDIO_SIMULATION) || (AUDIO_L1_STANDALONE)

      #if (OP_RIV_AUDIO == 1)
        #ifndef _WINDOWS
          T_FFS_FD     ffs_fd_1, ffs_fd_2;
          T_FFS_STAT   stat;

          /* Nb of instruments in the .lsi file */
          INT8  i = 0;
          INT16 instrument_id = -1;

          /* basic structure of the .lsi file */
          T_AUDIO_MELODY_E2_ID_NAME file_E2;

          /**** Find the size of the .lsi file ****/
          /* the .lsi file is stores into the flash */
          /* check if the .lsi file exists */

          ffs_fd_1 = ffs_open(p_audio_gbl_var->melody_E2_load_file_instruments.instrument_file_name,
            FFS_O_RDONLY );

          if (ffs_fd_1 < EFFS_OK)
          {
              audio_melody_E2_error_trace(AUDIO_ENTITY_LOAD_FILE_INSTR_ERROR);
              return (0);
          }

          /* find the .mwa file */
          while ( (i < p_audio_gbl_var->melody_E2_load_file_instruments.nb_of_instruments) &&
                  (customer_instrument_id != instrument_id))
          {
            /* Load the instruments file from the FFS */
            if ( (ffs_read ( ffs_fd_1,
                             (&file_E2),
                             (sizeof(INT8) + AUDIO_PATH_NAME_MAX_SIZE))) < EFFS_OK )
            {
              AUDIO_SEND_TRACE("AUDIO MELODY E2: impossible to load the .lsi file", RV_TRACE_LEVEL_ERROR);

              /* Close the file */
              ffs_close(ffs_fd_1);

              return (0);
            }

            instrument_id = file_E2.id;
          }
          /* Close the file */
          ffs_close(ffs_fd_1);

          if (i == p_audio_gbl_var->melody_E2_load_file_instruments.nb_of_instruments)
          {
            AUDIO_SEND_TRACE("AUDIO MELODY E2: the instrument doesn't exist in the .lsi file", RV_TRACE_LEVEL_ERROR);

            return (0);
          }

          /* Open the corresponding .mwa file */
          ffs_fd_2 = ffs_open( file_E2.melody_name,
                               FFS_O_RDONLY );
          if (ffs_fd_2 < EFFS_OK)
          {
            AUDIO_SEND_TRACE("AUDIO MELODY E2: impossible to open the .mwa file instruments", RV_TRACE_LEVEL_ERROR);

            /* Close the .mwa file */
            ffs_close(ffs_fd_2);

            return (0);
          }

          /* download the instrument description */
          ffs_stat(file_E2.melody_name,&stat);

          /* check if the file contains some data */
          if (stat.size ==0)
          {
            /* the file doesn't contains data */
            /* an error is generated */
            audio_melody_E2_error_trace(AUDIO_ENTITY_FILE_ERROR);

            /* Close the .mwa file */
            ffs_close(ffs_fd_2);

            return(0);
          }

          /* check if there's enough memory in the API for this instrument */
          if (allowed_size < (stat.size>>1))
          {
            /* an error is generated */
            audio_melody_E2_error_trace(AUDIO_ERROR_INSTRUMENT_SIZE);

            /* Close the .mwa file */
            ffs_close(ffs_fd_2);

            return(0);
          }

          ffs_read ( ffs_fd_2,
                     (void *)API_address,
                     stat.size );

          /* Close the file */
          ffs_close(ffs_fd_2);

          return((stat.size>>1));
        #endif //_WINDOWS
      #endif // OP_RIV_AUDIO
    }
  #endif // MELODY_E2

  #if (L1_EXT_AUDIO_MGT)
    /*-------------------------------------------------------*/
    /* Cust_ext_audio_mgt_hisr                               */
    /*-------------------------------------------------------*/
    /*                                                       */
    /* Parameters : none                                     */
    /*                                                       */
    /* Return     : none                                     */
    /*                                                       */
    /* Description :                                         */
    /* handle the DMA interrupt in order to request a new    */
    /* buffer                                                */
    /*                                                       */
    /*-------------------------------------------------------*/
    void Cust_ext_audio_mgt_hisr()
    {
     UWORD8  error_id = 0;
     UWORD16 requested_size = l1a_l1s_com.stereopath_drv_task.parameters.frame_number*2;

     error_id = Cust_get_pointer((UWORD16 **)&l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address,
                                 &requested_size,
                                 l1s.ext_audio_mgt.session_id);

    }
  #endif
 void l1a_bt_audio_noti_process()
 {
         UINT8 channel_id=0;
        void *p_message = NULL; //omaps00090550
         T_RVF_MB_STATUS mb_status; //omaps00090550
        T_AUDIO_DRIVER_SESSION *p_session;
        T_RV_RETURN *return_path;
          /* find active channel_id associated to session_id */
          while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL)&&
                ((p_audio_gbl_var->audio_driver_session[channel_id].session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)||
                 (p_audio_gbl_var->audio_driver_session[channel_id].session_req.session_id != AUDIO_EXT_MIDI_SESSION_ID)) )
          {
            channel_id++;
          }
        /* get driver session */
        
        p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);
        p_session->session_info.index_l1=1-p_session->session_info.index_l1;
        return_path = &(p_session->session_req.return_path);

        /* Translate the l1 message into the Riviera compliant message */
        /* Allocate the Riviera buffer */
        mb_status = rvf_get_buf ( p_audio_gbl_var->mb_external,
                                  sizeof (T_AUDIO_DRIVER_NOTIFICATION),
                                  (T_RVF_BUFFER **) (&p_message));
        /* If insufficient resources, then report a memory error and abort. */
        if (mb_status == RVF_RED)
        {
          /* the memory is insufficient to continue the non regression test */
          AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
          return;
        }

        /* Fill the message ID + parameters */
        ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id = AUDIO_DRIVER_NOTIFICATION_MSG;
        ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id = channel_id;
        ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer = NULL;

        /* send answer */
        if (return_path->callback_func == NULL)
          rvf_send_msg (return_path->addr_id, p_message);
        else
        {
          (*return_path->callback_func)((void *)(p_message));
          rvf_free_buf((T_RVF_BUFFER *)p_message);
        }
        
 }
//NAVC start/stop/read energy
// Parameters :d_navc_start_stop_read                      //   1=start,2=stop,3=read energy
// UWORD32 d_navc_ctrl_status_energy_val
//    0=action not performed,1=action perf, xxx-value
// Description : this function will called to start/stop the NAVC and  also to read the energy value during call
//The bit map of d_navc_ctrl_status is as follows.
// Bit 0: Start command: If 1 NAVC start. Set to 1 by MCU , Reset by DSp  after module start Bit 1: Stop command :
// If 1 NAVC stop.  Set to 1 by MCU , Reset by DSP after    module stop. Bit 15: Status: Set and reset by DSP. If 1
// module is active. If 0 module is not active.


    UWORD32 Cust_navc_ctrl_status(UWORD8  d_navc_start_stop_read)
    {
     UWORD32 d_navc_ctrl_status_energy_val=0;

     switch (d_navc_start_stop_read)
     {
     case 1:  /* Start NAVC */
		 {
			 if((l1s_dsp_com.dsp_ndb_ptr->d_navc_ctrl_status & 0x8000)==0)l1s_dsp_com.dsp_ndb_ptr->d_navc_ctrl_status |= 0x01; //check 15th bit and if not active-put the last bit to 1 to start
			 d_navc_ctrl_status_energy_val=1; //indicates action performed
		 }
		 break;
     case 2:  /* Stop NAVC */
		 {
			 if((l1s_dsp_com.dsp_ndb_ptr->d_navc_ctrl_status & 0x8000)==0x8000)l1s_dsp_com.dsp_ndb_ptr->d_navc_ctrl_status |= 0x02; //check 15th bit and if active-put the last bit to 1 to start
			 d_navc_ctrl_status_energy_val=1; //indicates action performed
		 }
		 break;
     case 3:  /* NAVC read energy */
		 {
		  if((l1s_dsp_com.dsp_ndb_ptr->d_navc_ctrl_status & 0x8000) == 0x8000)//check 15th bit and if active- read the value--
		  {
			 d_navc_ctrl_status_energy_val = l1s_dsp_com.dsp_ndb_ptr->d_vad_noise_ene_ndb[0]; //MSB part
		  d_navc_ctrl_status_energy_val= (d_navc_ctrl_status_energy_val<<16)|(l1s_dsp_com.dsp_ndb_ptr->d_vad_noise_ene_ndb[1]); //MSB <<16 | LSB   - indicates the energy value
		  }
		 }
		 break;
		 /*intended fall through */
     default:
		 break;


     }
#if (L1_NAVC_TRACE == 1)    //to see the NAVC trace
       l1_trace_navc(d_navc_start_stop_read,d_navc_ctrl_status_energy_val);
#endif

     return (d_navc_ctrl_status_energy_val);
    }

#endif // AUDIO_TASK== 1

