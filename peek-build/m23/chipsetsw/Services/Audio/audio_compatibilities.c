/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_compatibilities.c                                     */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the audio */
/*            feature compatibilites                                        */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------------------------------------------*/
/*  12 November 2001  Create                                                */
/*                                                                          */
/*  Author      Francois Mazard                                             */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif


  #include "l1_types.h"
  #include "l1_confg.h"
  #include "rvf/rvf_api.h"
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_api.h"
  #include "audio/audio_env_i.h"
  #include "audio/audio_ffs_i.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_messages_i.h"
  #include "audio/audio_compatibilities.h"
  #include "audio_features_i.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "ffs/ffs_api.h"
#endif
  #if (L1_GTT == 1)
    #include "tty/tty_i.h"
  #endif
  #if (L1_GTT == 1)
    extern void tty_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
    extern T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p;
  #endif
  /* Audio compatibilities table described in the annex of thespec S916 */
    /* 0: uncompatible */
    /* 1: compatible */
    /* Note the melody is compatible with a melody: GAME mode */
  /*VENKAT*/
  #if ((DSP == 36) || (DSP == 37) ||(DSP == 38) ||(DSP == 39) || (DSP == 35))
    T_AUDIO_COMPATIBILITES audio_feature_compatibilities_table[AUDIO_NUMBER_OF_FEATURES] =
    {
                /* keybeep  Tones  Melody E1  VM play  VM record enroll  update reco  MelodyE2 TTY VMA pl VMA rec MIDI MP3  AAC VMP pl VMP rec VBUF play VBUF reco*/
    /* Keybeep  */ {0,      1,     1,         1,       1,        0,      0,     0,    1,       1,  1,     1,      1,    1,   1,   0,   0,   0,	 0},
    /* Tone     */ {1,      0,     0,         1,       0,        0,      0,     0,    0,       1,  1,     1,      1,    1,   1,   0,   0,   0,   0},
    /* Melody E1*/ {1,      0,     1,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VM play  */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VM record*/ {1,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* SR enroll*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* SR update*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* SR reco  */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* Melody E2*/ {1,      0,     0,         0,       0,        0,      0,     0,    1,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* TTY      */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VMA play */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VMA rec  */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* MIDI     */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* MP3      */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* AAC      */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VMP play */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VMP reco */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   0},
    /* VBUF play */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   0,   1},
    /* VBUF reco */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0,   0,   0,   1,   0}
    };
  /*!VENKAT*/
  #endif /* DSP=36 */
  #if (DSP == 34)
    T_AUDIO_COMPATIBILITES audio_feature_compatibilities_table[AUDIO_NUMBER_OF_FEATURES] =
    {
                /* keybeep  Tones  Melody E1  VM play  VM record enroll  update reco  MelodyE2 TTY VMA pl VMA rec MIDI MP3  AAC*/
    /* Keybeep  */ {0,      1,     1,         1,       1,        0,      0,     0,    1,       0,  1,     1,      0,    0,   0},
    /* Tone     */ {1,      0,     0,         1,       0,        0,      0,     0,    0,       0,  1,     1,      0,    0,   0},
    /* Melody E1*/ {1,      0,     1,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VM play  */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VM record*/ {1,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* SR enroll*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* SR update*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* SR reco  */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* Melody E2*/ {1,      0,     0,         0,       0,        0,      0,     0,    1,       0,  0,     0,      0,    0,   0},
    /* TTY      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VMA play */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VMA rec  */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* MIDI     */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* MP3      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* AAC      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    };
  #endif
  #if ((DSP == 32) || (DSP == 33))
    T_AUDIO_COMPATIBILITES audio_feature_compatibilities_table[AUDIO_NUMBER_OF_FEATURES] =
    {
                 /* keybeep  Tones  Melody E1  VM play  VM record enroll  update reco  MelodyE2 TTY VMA pl VMA rec MIDI MP3 AAC */
    /* Keybeep   */ {0,      1,     1,         1,       1,        0,      0,     0,    1,       0,  0,     0,      0,   0,   0},
    /* Tone      */ {1,      0,     0,         1,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* Melody E1 */ {1,      0,     1,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* VM play   */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* VM record */ {1,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* SR enroll */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* SR update */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* SR reco   */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* Melody E2 */ {1,      0,     0,         0,       0,        0,      0,     0,    1,       0,  0,     0,      0,   0,   0},
    /* TTY       */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* VMA play  */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* VMA rec   */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* MIDI      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* MP3       */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    /* AAC       */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,   0,   0},
    };
  #endif /* DSP=32 or 33 */

  #if (DSP == 17)
    T_AUDIO_COMPATIBILITES audio_feature_compatibilities_table[AUDIO_NUMBER_OF_FEATURES] =
    {
                /* keybeep  Tones  Melody E1  VM play  VM record enroll  update reco  MelodyE2 TTY VMA pl VMA rec MIDI  MP3 AAC*/
    /* Keybeep  */ {0,      1,     1,         1,       1,        0,      0,     0,    1,       0,  0,     0,      0,    0,   0},
    /* Tone     */ {1,      0,     0,         1,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* Melody E1*/ {1,      0,     1,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VM play  */ {1,      1,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VM record*/ {1,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* SR enroll*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* SR update*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* SR reco  */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* Melody E2*/ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* TTY      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VMA play */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* VMA rec  */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* MIDI     */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* MP3      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    /* AAC      */ {0,      0,     0,         0,       0,        0,      0,     0,    0,       0,  0,     0,      0,    0,   0},
    };
  #endif /* DSP=17 */

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_compatibilities_feature_id                         */
  /*                                                                              */
  /*    Purpose:  Return the identifier of the feature to start.                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start message from any audio features                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        identifier of the audio features or AUDIO_NO_START_MESSAGE if it      */
  /*        isn't a starting message.                                             */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  INT8 audio_compatibilities_feature_id (T_RV_HDR *p_message)
  {
    switch(p_message->msg_id)
    {
      #if (KEYBEEP)
        case AUDIO_KEYBEEP_START_REQ:
        {
          return (AUDIO_KEYBEEP_FEATURE);
        }
      #endif
      #if (TONE)
        case AUDIO_TONES_START_REQ:
        {
          return (AUDIO_TONES_FEATURE);
        }
      #endif
      #if (MELODY_E1)
        case AUDIO_MELODY_E1_START_REQ:
        {
          return (AUDIO_MELODY_E1_FEATURE);
        }
      #endif
      #if (VOICE_MEMO)
        case AUDIO_VM_PLAY_START_REQ:
        {
          return (AUDIO_VOICE_MEMO_PLAY_FEATURE);
        }
        case AUDIO_VM_RECORD_START_REQ:
        {
          return (AUDIO_VOICE_MEMO_RECORD_FEATURE);
        }
      #endif
	  /*VENKAT*/
	   #if (L1_PCM_EXTRACTION) //VENKAT
        case AUDIO_VM_PCM_PLAY_START_REQ:
        {
          return (AUDIO_VOICE_MEMO_PCM_PLAY_FEATURE);
        }
        case AUDIO_VM_PCM_RECORD_START_REQ:
        {
          return (AUDIO_VOICE_MEMO_PCM_RECORD_FEATURE);
        }
      #endif
	  /*!VENKAT*/

	    /*VENKAT*/
   #if (L1_PCM_EXTRACTION) //VENKAT
        case AUDIO_VBUF_PCM_PLAY_START_REQ:
        {
          return (AUDIO_VBUF_PCM_PLAY_FEATURE);
        }
        case AUDIO_VBUF_PCM_RECORD_START_REQ:
        {
          return (AUDIO_VBUF_PCM_RECORD_FEATURE);
        }
      #endif
	  /*!VENKAT*/

      #if (L1_VOICE_MEMO_AMR)
        case AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ:
        case AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ:
        {
          return (AUDIO_VOICE_MEMO_AMR_PLAY_FEATURE);
        }
        case AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ:
        case AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ:
        {
          return (AUDIO_VOICE_MEMO_AMR_RECORD_FEATURE);
        }
      #endif
      #if (SPEECH_RECO)
        case AUDIO_SR_ENROLL_START_REQ:
        {
          return (AUDIO_SR_ENROLL_FEATURE);
        }
        case AUDIO_SR_UPDATE_START_REQ:
        {
          return (AUDIO_SR_UPDATE_FEATURE);
        }
      #endif
      #if (MELODY_E2)
        case AUDIO_MELODY_E2_START_REQ:
        {
          return (AUDIO_MELODY_E2_FEATURE);
        }
      #endif
      #if (L1_GTT == 1)
        case TTY_START_REQ:
        {
          return (AUDIO_TTY_FEATURE);
        }
      #endif
      #if (L1_EXT_AUDIO_MGT==1)
        case AUDIO_MIDI_START_REQ:
        {
          return (AUDIO_MIDI_FEATURE);
        }
      #endif
      #if (L1_MP3==1)
        case AUDIO_MP3_START_REQ:
        {
          return (AUDIO_MP3_FEATURE);
        }
      #endif
      #if (L1_AAC==1)
        case AUDIO_AAC_START_REQ:
        {
          return (AUDIO_AAC_FEATURE);
        }
      #endif
      default:
      {
        return (AUDIO_NO_START_MESSAGE);
      }
    }
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_compatibilities_feature_active                     */
  /*                                                                              */
  /*    Purpose:  Return the list of the active feature.                          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        An empty table.                                                       */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        List of the current audio feature actif.                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_compatibilities_feature_active (T_AUDIO_ACTIVE_FEATURE *p_audio_feature_active)
  {
    #if (KEYBEEP)
      p_audio_feature_active->feature[AUDIO_KEYBEEP_FEATURE] =
        (p_audio_gbl_var->keybeep.state > AUDIO_KEYBEEP_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_KEYBEEP_FEATURE] = 0;
    #endif

    #if (TONE)
      p_audio_feature_active->feature[AUDIO_TONES_FEATURE] =
        (p_audio_gbl_var->tones.state > AUDIO_TONES_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_TONES_FEATURE] = 0;
    #endif

    #if (MELODY_E1)
      p_audio_feature_active->feature[AUDIO_MELODY_E1_FEATURE] =
        ((p_audio_gbl_var->melody_E1_0.state > AUDIO_MELODY_E1_IDLE) ||
        (p_audio_gbl_var->melody_E1_1.state > AUDIO_MELODY_E1_IDLE));
    #else
      p_audio_feature_active->feature[AUDIO_MELODY_E1_FEATURE] = 0;
    #endif

    #if (VOICE_MEMO)
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_PLAY_FEATURE] =
        (p_audio_gbl_var->vm_play.state > AUDIO_VM_PLAY_IDLE);

      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_RECORD_FEATURE] =
        (p_audio_gbl_var->vm_record.state > AUDIO_VM_RECORD_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_PLAY_FEATURE]   = 0;
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_RECORD_FEATURE] = 0;
    #endif

	/*VENKAT*/
    #if (L1_PCM_EXTRACTION) //VENKAT
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_PCM_PLAY_FEATURE] =
        (p_audio_gbl_var->vm_pcm_play.state > AUDIO_VM_PCM_PLAY_IDLE);

      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_PCM_RECORD_FEATURE] =
        (p_audio_gbl_var->vm_pcm_record.state > AUDIO_VM_PCM_RECORD_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_PCM_PLAY_FEATURE]   = 0;
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_PCM_RECORD_FEATURE] = 0;
    #endif //VOICE_MEMO_PCM
	/*!VENKAT*/

		/*VENKAT*/
    #if (L1_PCM_EXTRACTION) //VENKAT
      p_audio_feature_active->feature[AUDIO_VBUF_PCM_PLAY_FEATURE] =
        (p_audio_gbl_var->vm_pcm_play.state > AUDIO_VBUF_PCM_PLAY_IDLE);

      p_audio_feature_active->feature[AUDIO_VBUF_PCM_RECORD_FEATURE] =
        (p_audio_gbl_var->vm_pcm_record.state > AUDIO_VBUF_PCM_RECORD_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_VBUF_PCM_PLAY_FEATURE]   = 0;
      p_audio_feature_active->feature[AUDIO_VBUF_PCM_RECORD_FEATURE] = 0;
    #endif //VOICE_MEMO_PCM
	/*!VENKAT*/

    #if (L1_VOICE_MEMO_AMR)
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_AMR_PLAY_FEATURE] =
        (p_audio_gbl_var->audio_vm_amr_play.state > AUDIO_IDLE);

      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_AMR_RECORD_FEATURE] =
        (p_audio_gbl_var->audio_vm_amr_record.state > AUDIO_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_AMR_PLAY_FEATURE]   = 0;
      p_audio_feature_active->feature[AUDIO_VOICE_MEMO_AMR_RECORD_FEATURE] = 0;
    #endif

    #if (SPEECH_RECO)
      p_audio_feature_active->feature[AUDIO_SR_ENROLL_FEATURE] =
        (p_audio_gbl_var->speech_reco.sr_enroll.state > AUDIO_SR_ENROLL_IDLE);
      p_audio_feature_active->feature[AUDIO_SR_UPDATE_FEATURE] =
        (p_audio_gbl_var->speech_reco.sr_update.state > AUDIO_SR_UPDATE_IDLE);
      p_audio_feature_active->feature[AUDIO_SR_RECO_FEATURE] =
        (p_audio_gbl_var->speech_reco.sr_reco.state > AUDIO_SR_RECO_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_SR_ENROLL_FEATURE] = 0;
      p_audio_feature_active->feature[AUDIO_SR_UPDATE_FEATURE] = 0;
      p_audio_feature_active->feature[AUDIO_SR_RECO_FEATURE]   = 0;
    #endif

    #if (MELODY_E2)
      p_audio_feature_active->feature[AUDIO_MELODY_E2_FEATURE] =
        ((p_audio_gbl_var->melody_E2_0.state > AUDIO_MELODY_E2_IDLE) ||
        (p_audio_gbl_var->melody_E2_1.state > AUDIO_MELODY_E2_IDLE));
    #else
      p_audio_feature_active->feature[AUDIO_MELODY_E2_FEATURE] = 0;
    #endif

    #if (L1_GTT == 1)
      p_audio_feature_active->feature[AUDIO_TTY_FEATURE] =
        (tty_env_ctrl_blk_p->state > TTY_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_TTY_FEATURE] = 0;
    #endif

    #if (L1_EXT_AUDIO_MGT==1)
      p_audio_feature_active->feature[AUDIO_MIDI_FEATURE] =
        (p_audio_gbl_var->midi.state>AUDIO_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_MIDI_FEATURE] = 0;
    #endif

    #if (L1_MP3)
      p_audio_feature_active->feature[AUDIO_MP3_FEATURE] =
        (p_audio_gbl_var->audio_mp3.state > AUDIO_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_MP3_FEATURE]   = 0;
    #endif

    #if (L1_AAC)
      p_audio_feature_active->feature[AUDIO_AAC_FEATURE] =
        (p_audio_gbl_var->audio_aac.state > AUDIO_IDLE);
    #else
      p_audio_feature_active->feature[AUDIO_AAC_FEATURE]   = 0;
    #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_compatibilities_manager                            */
  /*                                                                              */
  /*    Purpose:  manage the compatibilities between the audio fatures.           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        The message of the start request.                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Flag to indicate if the task can be start or not.                     */
  /*        Note: this manager send automatically the status message              */
  /*        corresponding to the audio feature.                                   */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  BOOLEAN audio_compatibilities_manager (T_RV_HDR *p_message)
  {
    T_AUDIO_ACTIVE_FEATURE  active_list;
    INT8                   feature_to_start, i;
    BOOLEAN                 feature_can_start = TRUE;
//            AUDIO_SEND_TRACE("audio_compatibilities_manager():",RV_TRACE_LEVEL_DEBUG_HIGH);
    /* Find the features to start */
    feature_to_start = audio_compatibilities_feature_id(p_message);

    /* if the message is a start message */
    if (feature_to_start != AUDIO_NO_START_MESSAGE)
    {
      /* List the current active feature */
      audio_compatibilities_feature_active(&active_list);

      i = -1;
      while ((feature_can_start) &&
             (++i < AUDIO_NUMBER_OF_FEATURES))
      {
        feature_can_start = (!(active_list.feature[i]) || //OMAPS00090550
          audio_feature_compatibilities_table[feature_to_start].compatibilities_flag[i]);
      }

      /* send the status message if the feature can't start */
      if (feature_can_start == FALSE)
      {
        switch (feature_to_start)
        {
          #if (KEYBEEP)
            case AUDIO_KEYBEEP_FEATURE:
            {
              audio_keybeep_send_status (AUDIO_ERROR, ((T_AUDIO_KEYBEEP_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: Keybeep isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
          #if (TONE)
            case AUDIO_TONES_FEATURE:
            {
              audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: Tone isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
          #if (VOICE_MEMO)
            case AUDIO_VOICE_MEMO_PLAY_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                {
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: VM play isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
            case AUDIO_VOICE_MEMO_RECORD_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                {
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_vm_record_send_status (AUDIO_ERROR, 0, ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: VM record isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif

	      /*VENKAT*/
            #if (L1_PCM_EXTRACTION) //VENKAT
            case AUDIO_VOICE_MEMO_PCM_PLAY_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                {
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: VM play isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
            case AUDIO_VOICE_MEMO_PCM_RECORD_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_VM_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                {
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_vm_pcm_record_send_status (AUDIO_ERROR, 0, ((T_AUDIO_VM_PCM_RECORD_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: VM record isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
	  /*!VENKAT*/


		  	      /*VENKAT*/
            #if (L1_PCM_EXTRACTION) //VENKAT
            case AUDIO_VBUF_PCM_PLAY_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VBUF_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_VBUF_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
				{
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_vbuf_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VBUF_PCM_PLAY_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: VM play isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
            case AUDIO_VBUF_PCM_RECORD_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
				{
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_vbuf_pcm_record_send_status (AUDIO_ERROR, 0, ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: VM record isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
	  /*!VENKAT*/


          #if (L1_VOICE_MEMO_AMR)
            case AUDIO_VOICE_MEMO_AMR_PLAY_FEATURE:
            {
              switch (p_message->msg_id)
              {
              #if (AUDIO_NEW_FFS_MANAGER)
                case AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ:
                  /* close the file opened */
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
				  {
                    audio_new_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                  audio_vm_amr_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *)p_message)->return_path);
                break;
              #endif
              #if (AUDIO_RAM_MANAGER)
                case AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ:
                  audio_vm_amr_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_AMR_PLAY_FROM_RAM_START *)p_message)->return_path);
                break;
              #endif
              }
              AUDIO_SEND_TRACE(" AUDIO ERROR: AMR play isn't compatible with a running task",
                               RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
            }
            break;
            case AUDIO_VOICE_MEMO_AMR_RECORD_FEATURE:
            {
              switch (p_message->msg_id)
              {
              #if (AUDIO_NEW_FFS_MANAGER)
                case AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ:
                  /* close the file opened */
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
				  {
                    audio_new_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                  audio_vm_amr_record_send_status (AUDIO_ERROR, 0, ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->return_path);
                break;
              #endif
              #if (AUDIO_RAM_MANAGER)
                case AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ:
                  audio_vm_amr_record_send_status (AUDIO_ERROR, 0, ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->return_path);
                break;
              #endif
              }
              AUDIO_SEND_TRACE(" AUDIO ERROR: AMR record isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
            }
            break;
          #endif
          #if (MELODY_E1)
            case AUDIO_MELODY_E1_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
				{
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: MELODY E1 isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
          #if (SPEECH_RECO)
            case AUDIO_SR_ENROLL_FEATURE:
            {
              /* close the model file */
              /* close the voice sample file if it exists*/
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->sr_ffs_fd);
#else
			  ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->sr_ffs_fd);
#endif
                if ( ((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                {

#if(AS_RFS_API == 1)
				rfs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd);
#else
				  ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd);
#endif
                }
              #endif

              /* event error - send an error message*/
              audio_sr_enroll_send_status (AUDIO_ERROR,
                ((T_AUDIO_SR_ENROLL_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: SR Enroll isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
            case AUDIO_SR_UPDATE_FEATURE:
            {
              /* Free the database buffer */
              rvf_free_buf( (T_RVF_BUFFER *)(((T_AUDIO_SR_UPDATE_START *)(p_message))->p_database) );

              /* event error - send an error message*/
              audio_sr_update_send_status (AUDIO_ERROR,
                ((T_AUDIO_SR_UPDATE_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: SR upate isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
            case AUDIO_SR_RECO_FEATURE:
            {
              /* Free the database buffer */
              rvf_free_buf( (T_RVF_BUFFER *)(((T_AUDIO_SR_RECO_START *)(p_message))->p_database) );

              /* event error - send an error message*/
              audio_sr_reco_send_status (AUDIO_ERROR, 0,
                ((T_AUDIO_SR_RECO_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: SR reco isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
          #if (MELODY_E2)
            case AUDIO_MELODY_E2_FEATURE:
            {
              /* close the file opened */
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_MELODY_E2_START *)(p_message))->audio_E2_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_MELODY_E2_START *)(p_message))->audio_E2_ffs_fd) != EFFS_OK )
#endif
				{
                  audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_melody_E2_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E2_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: MELODY E2 isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
          #if (L1_GTT == 1)
            case AUDIO_TTY_FEATURE:
            {
              tty_send_status (AUDIO_ERROR, ((T_TTY_START *)(p_message))->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: TTY isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif
          #if (L1_MIDI==1)
            case AUDIO_MIDI_FEATURE:
            {
              audio_midi_send_status(AUDIO_ERROR,&((T_AUDIO_MIDI_START *)p_message)->return_path);
              AUDIO_SEND_TRACE(" AUDIO ERROR: MIDI isn't compatible with a running task",
                             RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
          #endif

          #if (L1_MP3)
            case AUDIO_MP3_FEATURE:
            {
              switch (p_message->msg_id)
              {
                case AUDIO_MP3_START_REQ:
                  /* close the file opened */
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_MP3_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_MP3_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
				  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                  audio_mp3_send_status (AUDIO_ERROR, ((T_AUDIO_MP3_START *)p_message)->return_path);
                break;
                 }
              AUDIO_SEND_TRACE(" AUDIO ERROR: MP3 isn't compatible with a running task",
                               RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
            }
            break;
        #endif

        #if (L1_AAC)
            case AUDIO_AAC_FEATURE:
            {
              switch (p_message->msg_id)
              {
                case AUDIO_AAC_START_REQ:
                  /* close the file opened */
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_AAC_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_AAC_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
				  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                  audio_aac_send_status (AUDIO_ERROR, ((T_AUDIO_AAC_START *)p_message)->return_path);
                break;
                 }
              AUDIO_SEND_TRACE(" AUDIO ERROR: AAC isn't compatible with a running task",
                               RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->message_processed = TRUE;
            }
            break;
        #endif
        }
      } /* feature_can_start == FALSE */
    }
    return (feature_can_start);
  }
#endif /* RVM_AUDIO_MAIN_SWE */
