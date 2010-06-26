/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1GTT_ASYNC.C
 *
 *        Filename l1gtt_async.c
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

#define L1GTT_ASYNC_C

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

    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_baudot_functions.h"

    #include "l1gtt_const.h"        
    #include "l1gtt_defty.h"        
    #include "l1gtt_msgty.h"        
    #include "l1gtt_signa.h"        
    #include "l1gtt_varex.h"        
       
    #if (AUDIO_TASK == 1)
      #include "l1audio_const.h"
      #include "l1audio_cust.h"
      #include "l1audio_defty.h"
    #endif

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #if (L1_DYN_DSP_DWNLD==1)
      #include "l1_dyn_dwl_defty.h"
      #include "l1_dyn_dwl_const.h"
      #include "l1_dyn_dwl_signa.h"
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
      #include "l2_l3.h"
    #endif
  #else
    #include <string.h>
    #include "l1_types.h"
    #include "sys_types.h"
    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_baudot_functions.h"

    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_msgty.h"
    #include "l1gtt_signa.h"
    #include "l1gtt_varex.h"

    #if (AUDIO_TASK == 1)
      #include "l1audio_const.h"
      #include "l1audio_cust.h"
      #include "l1audio_defty.h"
    #endif

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #if (L1_DYN_DSP_DWNLD==1)
      #include "l1_dyn_dwl_defty.h"
      #include "l1_dyn_dwl_const.h"
      #include "l1_dyn_dwl_signa.h"
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

    #if (OP_L1_STANDALONE == 0)
      #include "rvf_api.h"
    #endif
  #endif

  #if(L1_DYN_DSP_DWNLD == 1)
    #if (TRACE_TYPE == 1) ||(TRACE_TYPE == 4) || (TRACE_TYPE == 5) || (TRACE_TYPE == 7) || (TESTMODE)
      #include "l1_trace.h"
    #endif
    #if(CODE_VERSION == SIMULATION)
     extern void trace_fct_simu_dyn_dwnld(CHAR *fct_name);
    #endif
  #endif // L1_DYN_DSP_DWNLD == 1

  #if (CODE_VERSION == SIMULATION)
    FILE *gtt_input;
  #endif

  /**************************************/
  /* Prototypes for L1 ASYNCH task      */
  /**************************************/

  void l1a_mmi_gtt_process        (xSignalHeaderRec *msg);

  /**************************************/
  /* Extern Prototypes                  */
  /**************************************/

  extern void l1a_audio_send_confirmation(UWORD32 SignalCode);

  extern void l1gtt_initialize(void);
  extern void l1gtt_exit(void);
  extern void TTY_init_API(void);

  #if (OP_L1_STANDALONE == 0)
    #include "tty_i.h"
    extern T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p;
  #endif

  /*-------------------------------------------------------*/
  /* l1a_mmi_gtt_process()                                 */ 
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* This function is a state machine which handles the    */
  /* tty feature.                                          */
  /*                                                       */
  /* Starting messages:        MMI_TTY_START_REQ           */
  /*                                                       */
  /* Result messages (input):  L1_TTY_START_CON            */
  /*                                                       */
  /* Result messages (output): MMI_TTY_START_CON           */
  /*                                                       */
  /* Reset messages (input):   none                        */
  /*                                                       */
  /* Stop message (input):     MMI_TTY_STOP_REQ            */
  /*                           L1_TTY_STOP_CON             */
  /*                                                       */
  /* Stop message (output):    MMI_TTY_STOP_CON            */
  /*                                                       */
  /* Rem:                                                  */
  /* ----                                                  */
  /*                                                       */
  /*-------------------------------------------------------*/

#if (L1_DYN_DSP_DWNLD==1)

  void l1a_mmi_gtt_process(xSignalHeaderRec *msg)
  {
    enum states 
    {
      RESET             = 0,
      WAIT_INIT     =1,
      WAIT_DYN_DWNLD = 2,
      WAIT_INIT_CON     = 3,
      WAIT_IN_CALL      = 4
    };

    UWORD8    *state      = &l1a.state[L1A_GTT_STATE];
    UWORD32   SignalCode  = msg->SignalCode;
    UWORD8     channel_mode_mod;
    while(1)
    {
      switch(*state)
      {
        case RESET:
        {
          // Reset the command
          l1a_l1s_com.gtt_task.command.start = FALSE;
          l1a_l1s_com.gtt_task.command.stop  = FALSE;

          *state = WAIT_INIT;
        }
        break;
        case WAIT_INIT:
        {
         if(SignalCode == MMI_GTT_START_REQ)
         {
          // If flexibility required to support 45.45 or 50bps keyboards, use
          // l1a_l1s_com.gtt_task.baudot_keyboard_rate = ((T_MMI_GTT_START_REQ *)(msg->SigP))->keyboard_rate;
          l1a_l1s_com.gtt_task.baudot_keyboard_rate = 0;
          l1a_l1s_com.gtt_task.l1_gtt_mode = (T_L1_GTT_MODE)(((T_MMI_GTT_START_REQ *)(msg->SigP))->tty_mode);

          if (l1a.dyn_dwnld.semaphore_vect[TTY_STATE_MACHINE]==GREEN)
          {
          
            // WARNING: code below must be duplicated in WAIT_DYN_DWNLD state
            // Initialize the algorithms          
            #if (OP_L1_STANDALONE == 1)
              os_set_event(L1GTTBACK_EVENTS, INIT_EVENT);
            #else
              rvf_send_event (tty_env_ctrl_blk_p->addr_id, TTY_INIT_EVENT_MASK);
            #endif

            TTY_init_API();
            
            // Start the L1S tty task
            l1a_l1s_com.gtt_task.command.start = TRUE;

            // Change state
            *state = WAIT_INIT_CON;
          
            #if (TRACE_TYPE == 5)
              GTT_send_trace_cpy("GTT start\n");
            #endif
          }
        else
         {
           #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5))
              if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_DYN_DWNLD))
                 {
                   char str[30];
                   sprintf(str,"GTT SM blocked by DYN DWNLD\r\n");
                   #if(CODE_VERSION == SIMULATION)
                     trace_fct_simu_dyn_dwnld(str);
                   #else
                     rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
                   #endif
                 }
           #endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
            *state = WAIT_DYN_DWNLD;
         }
        }
          // End process
         return;
        }
//omaps00090550        break;
        case WAIT_DYN_DWNLD:
        {
           if((SignalCode == API_L1_DYN_DWNLD_FINISHED) && (l1a.dyn_dwnld.semaphore_vect[TTY_STATE_MACHINE] == GREEN))
           	{

                // Initialize the algorithms          
                #if (OP_L1_STANDALONE == 1)
                  os_set_event(L1GTTBACK_EVENTS, INIT_EVENT);
                #else
                  rvf_send_event (tty_env_ctrl_blk_p->addr_id, TTY_INIT_EVENT_MASK);
                #endif
                
                TTY_init_API();
                // Start the L1S tty task
                l1a_l1s_com.gtt_task.command.start = TRUE;

                // Change state
                *state = WAIT_INIT_CON;
                
               #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5))
                if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_DYN_DWNLD))
                 {
                   char str[30];
                   sprintf(str,"GTT SM un-blocked\r\n");
                   #if(CODE_VERSION == SIMULATION)
                     trace_fct_simu_dyn_dwnld(str);
                   #else
                     rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
                   #endif
                 }
                #endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)          

                #if (TRACE_TYPE == 5)
                 GTT_send_trace_cpy("GTT start\n");
                #endif
              }
        return;
       }
//omaps00090550        break;
       case WAIT_INIT_CON:
        {
          if (SignalCode == L1_GTT_START_CON)
          {
            // Disable the start command
            l1a_l1s_com.gtt_task.command.start = FALSE;

            // Send the  confirmation message
            l1a_audio_send_confirmation(MMI_GTT_START_CON);

            // Change state
            *state = WAIT_IN_CALL;
          }

          // End process
          return;
       }
//omaps00090550        break;


        case WAIT_IN_CALL:
        {
          /* If Protocol stack issues a MPHC_CHANNEL_MODE_MODIFY_REQ without stopping */
          /* GTT process through MMI_GTT_STOP_REQ, we must check the channel_mode     */
          /* parameter of the MPHC_CHANNEL_MODE_MODIFY_REQ in order to stop DSP concealing */
          /* this data as TTY data */
          
          if (SignalCode == MPHC_CHANNEL_MODE_MODIFY_REQ)
          	{
          	   channel_mode_mod=((T_MPHC_CHANNEL_MODE_MODIFY_REQ *)(msg->SigP))->channel_mode;
          	   if( channel_mode_mod == TCH_FS_MODE ||
                     channel_mode_mod == TCH_HS_MODE ||
                     channel_mode_mod == TCH_EFR_MODE
                   )
                   channel_mode_mod=0;
          	   else
          	     channel_mode_mod=1;
          	}
          else
          	channel_mode_mod=0;
          
          if ((SignalCode == MMI_GTT_STOP_REQ) ||
              (SignalCode == MPHC_STOP_DEDICATED_REQ) ||
              (SignalCode == MPHC_IMMED_ASSIGN_REQ) ||
              (channel_mode_mod == 1)
              )
          {
            // Enable the stop command
            l1a_l1s_com.gtt_task.command.stop = TRUE;

            // End process
            return;
          }

        #if L2_L3_SIMUL
          else
          /* GTT test messages (allowed in this L1A state only) */
          /*----------------------------------------------------*/

          if ( SignalCode == TST_CLOSE_GTT_LOOP_REQ )
          {
            // Enable loop on DSP side
            l1s_dsp_com.dsp_ndb_ptr->d_tty_loop_ctrl = ((T_TST_CLOSE_GTT_LOOP_REQ *)(msg->SigP))->loop_type;

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_CLOSE_GTT_LOOP_CON);

            // End process
            return;
          }
          else
          if ( SignalCode == TST_OPEN_GTT_LOOP_REQ )
          {
            // Open loop on DSP side
            l1s_dsp_com.dsp_ndb_ptr->d_tty_loop_ctrl = 0;

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_OPEN_GTT_LOOP_CON);

            // End process
            return;
          }
          else
          if ( SignalCode == TST_START_GTT_TEST_REQ )
          {
            // Enable GTT test scenario reading
            l1a_l1s_com.gtt_task.test.scen_enable = ((T_TST_START_GTT_TEST_REQ *)(msg->SigP))->scen_type;
            l1a_l1s_com.gtt_task.test.rate        = ((T_TST_START_GTT_TEST_REQ *)(msg->SigP))->rate;

            
          #if (CODE_VERSION == SIMULATION)  
            if ((l1a_l1s_com.gtt_task.test.scen_enable == CTM_RX_INPUT) ||
                (l1a_l1s_com.gtt_task.test.scen_enable == TTY_DEC_INPUT))
            {
              // Open input file
              char filename[150];
              char number[3];

              strcpy(filename,   test_dir);
              strcat(filename, "gtt_input");
              sprintf(number,"%d", l1a_l1s_com.gtt_task.test.rate);
              strcat(filename, number);
              strcat(filename, ".dat");

              if ((gtt_input = fopen(filename,"r")) == NULL)
              {
                printf("Pb to open the input file gtt_input.dat !\n");
                EXIT;
              }
            }
          #endif

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_START_GTT_TEST_CON);

            // End process
            return;
          }
          else
          if ( SignalCode == TST_STOP_GTT_TEST_REQ )
          {
            // Disable GTT test scenario reading
            l1a_l1s_com.gtt_task.test.scen_enable = 0;

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_STOP_GTT_TEST_CON);
                
            // End process
            return;
          }
        #endif // L2_L3_SIMUL

          else
          if (SignalCode == L1_GTT_STOP_CON)
          {
            // Disable the stop command
            l1a_l1s_com.gtt_task.command.stop = FALSE;

            // Stop the algorithms          
            #if (OP_L1_STANDALONE == 1)
              os_set_event(L1GTTBACK_EVENTS, EXIT_EVENT);
            #else
              rvf_send_event (tty_env_ctrl_blk_p->addr_id, TTY_EXIT_EVENT_MASK);
            #endif

            // Send the  confirmation message
            l1a_audio_send_confirmation(MMI_GTT_STOP_CON);

            // Change state
            *state = RESET;
          }
          else
            return;
        }
        break;

      } // switch

    } // while(1)
  }

#else

  void l1a_mmi_gtt_process(xSignalHeaderRec *msg)
  {
    enum states 
    {
      RESET             = 0,
      WAIT_INIT         = 1,
      WAIT_INIT_CON     = 2,
      WAIT_IN_CALL      = 3
    };

    UWORD8    *state      = &l1a.state[L1A_GTT_STATE];
    UWORD32   SignalCode  = msg->SignalCode;
    UWORD8     channel_mode_mod;
    while(1)
    {
      switch(*state)
      {
        case RESET:
        {
          // Reset the command
          l1a_l1s_com.gtt_task.command.start = FALSE;
          l1a_l1s_com.gtt_task.command.stop  = FALSE;

          *state = WAIT_INIT;
        }
        break;

        case WAIT_INIT:
        {
          if (SignalCode == MMI_GTT_START_REQ )
          {
            // Initialize the algorithms          
            #if (OP_L1_STANDALONE == 1)
              os_set_event(L1GTTBACK_EVENTS, INIT_EVENT);
            #else
              rvf_send_event (tty_env_ctrl_blk_p->addr_id, TTY_INIT_EVENT_MASK);
            #endif

            // Initialize TTY API
            TTY_init_API();

            // Start the L1S tty task
            l1a_l1s_com.gtt_task.command.start = TRUE;

            // Change state
            *state = WAIT_INIT_CON;
          
            #if (TRACE_TYPE == 5)
              GTT_send_trace_cpy("GTT start\n");
            #endif
          }

          // End process
          return;
        }
        break;

        case WAIT_INIT_CON:
        {
          if (SignalCode == L1_GTT_START_CON)
          {
            // Disable the start command
            l1a_l1s_com.gtt_task.command.start = FALSE;

            // Send the  confirmation message
            l1a_audio_send_confirmation(MMI_GTT_START_CON);

            // Change state
            *state = WAIT_IN_CALL;
          }

          // End process
          return;
       }
       break;


        case WAIT_IN_CALL:
        {
          /* If Protocol stack issues a MPHC_CHANNEL_MODE_MODIFY_REQ without stopping */
          /* GTT process through MMI_GTT_STOP_REQ, we must check the channel_mode     */
          /* parameter of the MPHC_CHANNEL_MODE_MODIFY_REQ in order to stop DSP concealing */
          /* this data as TTY data */
          
          if (SignalCode == MPHC_CHANNEL_MODE_MODIFY_REQ)
          	{
          	   channel_mode_mod=((T_MPHC_CHANNEL_MODE_MODIFY_REQ *)(msg->SigP))->channel_mode;
          	   if( channel_mode_mod == TCH_FS_MODE ||
                     channel_mode_mod == TCH_HS_MODE ||
                     channel_mode_mod == TCH_EFR_MODE
                   )
                   channel_mode_mod=0;
          	   else
          	     channel_mode_mod=1;
          	}
          else
          	channel_mode_mod=0;
          
          if ((SignalCode == MMI_GTT_STOP_REQ) ||
              (SignalCode == MPHC_STOP_DEDICATED_REQ) ||
              (SignalCode == MPHC_IMMED_ASSIGN_REQ) ||
              (channel_mode_mod == 1)
              )
          {
            // Enable the stop command
            l1a_l1s_com.gtt_task.command.stop = TRUE;

            // End process
            return;
          }

        #if L2_L3_SIMUL
          else
          /* GTT test messages (allowed in this L1A state only) */
          /*----------------------------------------------------*/

          if ( SignalCode == TST_CLOSE_GTT_LOOP_REQ )
          {
            // Enable loop on DSP side
            l1s_dsp_com.dsp_ndb_ptr->d_tty_loop_ctrl = ((T_TST_CLOSE_GTT_LOOP_REQ *)(msg->SigP))->loop_type;

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_CLOSE_GTT_LOOP_CON);

            // End process
            return;
          }
          else
          if ( SignalCode == TST_OPEN_GTT_LOOP_REQ )
          {
            // Open loop on DSP side
            l1s_dsp_com.dsp_ndb_ptr->d_tty_loop_ctrl = 0;

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_OPEN_GTT_LOOP_CON);

            // End process
            return;
          }
          else
          if ( SignalCode == TST_START_GTT_TEST_REQ )
          {
            // Enable GTT test scenario reading
            l1a_l1s_com.gtt_task.test.scen_enable = ((T_TST_START_GTT_TEST_REQ *)(msg->SigP))->scen_type;
            l1a_l1s_com.gtt_task.test.rate        = ((T_TST_START_GTT_TEST_REQ *)(msg->SigP))->rate;

            
          #if (CODE_VERSION == SIMULATION)  
            if ((l1a_l1s_com.gtt_task.test.scen_enable == CTM_RX_INPUT) ||
                (l1a_l1s_com.gtt_task.test.scen_enable == TTY_DEC_INPUT))
            {
              // Open input file
              char filename[150];
              char number[3];

              strcpy(filename,   test_dir);
              strcat(filename, "gtt_input");
              sprintf(number,"%d", l1a_l1s_com.gtt_task.test.rate);
              strcat(filename, number);
              strcat(filename, ".dat");

              if ((gtt_input = fopen(filename,"r")) == NULL)
              {
                printf("Pb to open the input file gtt_input.dat !\n");
                EXIT;
              }
            }
          #endif

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_START_GTT_TEST_CON);

            // End process
            return;
          }
          else
          if ( SignalCode == TST_STOP_GTT_TEST_REQ )
          {
            // Disable GTT test scenario reading
            l1a_l1s_com.gtt_task.test.scen_enable = 0;

            // Send confirmation message to L3.
            l1a_audio_send_confirmation(TST_STOP_GTT_TEST_CON);
                
            // End process
            return;
          }
        #endif // L2_L3_SIMUL

          else
          if (SignalCode == L1_GTT_STOP_CON)
          {
            // Disable the stop command
            l1a_l1s_com.gtt_task.command.stop = FALSE;

            // Stop the algorithms          
            #if (OP_L1_STANDALONE == 1)
              os_set_event(L1GTTBACK_EVENTS, EXIT_EVENT);
            #else
              rvf_send_event (tty_env_ctrl_blk_p->addr_id, TTY_EXIT_EVENT_MASK);
            #endif

            // Send the  confirmation message
            l1a_audio_send_confirmation(MMI_GTT_STOP_CON);

            // Change state
            *state = RESET;
          }
          else
            return;
        }
        break;

      } // switch

    } // while(1)
  }
 #endif // L1_DYN_DSP_DWNLD
#endif // tty_cmd
