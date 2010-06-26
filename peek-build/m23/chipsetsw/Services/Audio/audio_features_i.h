#ifndef AUDIO_FEATURES_I_H
#define AUDIO_FEATURES_I_H

  #include "audio/audio_api.h"
  #ifndef _WINDOWS
    #include "cust_os.h"
  #else
    typedef struct
    {
      int dummy;
    }
    DummyStruct;
  #endif

  #ifdef _WINDOWS
    #define L1_MP3 1
    #define L1_AAC 1
  #endif

  /* configuration */
  /* FFS or RAM manager exist only if they are enabled AND a related L1 feature is enabled
     Currently only VOICE MEMO AMR, MP3 and AAC are supported */
  #define AUDIO_ENABLE_NEW_FFS_MANAGER 1
  #define AUDIO_NEW_FFS_MANAGER ((AUDIO_ENABLE_NEW_FFS_MANAGER)&&((L1_VOICE_MEMO_AMR)||(L1_MP3)||(L1_AAC)))
  #define AUDIO_ENABLE_RAM_MANAGER 1
  #define AUDIO_RAM_MANAGER ((AUDIO_ENABLE_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR))

  /* external prototype */
  #if (KEYBEEP)
    void audio_keybeep_manager (T_RV_HDR *p_message);
    void audio_keybeep_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  #endif
  #if (TONE)
    void audio_tones_manager (T_RV_HDR *p_message);
    void audio_tones_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  #endif
  #if (MELODY_E1)
    void audio_melody_E1_manager_0       (T_RV_HDR *p_message);
    void audio_melody_E1_manager_1       (T_RV_HDR *p_message);
    UINT8 audio_melody_E1_message_switch (T_RV_HDR *p_message);
    void audio_melody_E1_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  #endif
  #if (MELODY_E2)
    void audio_melody_E2_manager_0       (T_RV_HDR *p_message);
    void audio_melody_E2_manager_1       (T_RV_HDR *p_message);
    UINT8 audio_melody_E2_message_switch (T_RV_HDR *p_message);
    void audio_background_melody_e2_download_instrument_manager (T_RV_HDR *p_message);
    void audio_melody_E2_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  #endif
  #if (VOICE_MEMO)
    void audio_vm_play_manager (T_RV_HDR *p_message);
    void audio_vm_record_manager(T_RV_HDR *p_message);
    UINT8 audio_voice_memo_message_switch (T_RV_HDR *p_message);
    void audio_vm_play_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
    void audio_vm_record_send_status ( T_AUDIO_RET status,
                                              UINT32 recorded_size,
                                              T_RV_RETURN return_path);
  #endif
  #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO)
    void audio_ffs_manager (T_RV_HDR *p_message);
    void audio_ffs_downloader (void);
  #endif
  #if (L1_VOICE_MEMO_AMR)
    void audio_vm_amr_play_from_memory_manager (T_RV_HDR *p_message);
    void audio_vm_amr_record_to_memory_manager (T_RV_HDR *p_message);
  #endif
  #if (L1_VOICE_MEMO_AMR)
    UINT8 audio_voice_memo_amr_memory_message_switch (T_RV_HDR *p_message);
    void audio_vm_amr_play_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
    void audio_vm_amr_record_send_status ( T_AUDIO_RET status,
                                           UINT32 recorded_size,
                                           T_RV_RETURN return_path);
  #endif
  #if (SPEECH_RECO)
    void audio_sr_enroll_manager (T_RV_HDR *p_message);
    void audio_sr_update_manager (T_RV_HDR *p_message);
    void audio_sr_reco_manager (T_RV_HDR *p_message);
    #ifndef _WINDOWS
      void audio_sr_background_manager(T_RV_HDR *p_message);
    #endif
    void audio_sr_enroll_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
    void audio_sr_update_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
    void audio_sr_reco_send_status (T_AUDIO_RET status, UINT8 word_index, T_RV_RETURN return_path);
  #endif
  #if (L1_AUDIO_DRIVER)
    void audio_driver_manager(T_RV_HDR *p_message);
    UINT8 audio_driver_message_switch(T_RV_HDR *p_message);
  #endif
  #if (L1_EXT_AUDIO_MGT==1)
    void audio_driver_manager_for_midi(T_RV_HDR *p_message);
    void audio_midi_manager(T_RV_HDR *p_message);
    UINT8 audio_midi_message_switch(T_RV_HDR *p_message);
  #endif
  #if (L1_MP3==1)
    void audio_mp3_manager (T_RV_HDR *p_message);
    UINT8 audio_mp3_message_switch(T_RV_HDR *p_message);
    void audio_mp3_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  #endif
  #if (L1_AAC==1)
    void audio_aac_manager (T_RV_HDR *p_message);
    UINT8 audio_aac_message_switch(T_RV_HDR *p_message);
    void audio_aac_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  #endif
  #if ((L1_MP3==1) || (L1_AAC==1))
    UINT8 Cust_get_pointer_next_buffer (UINT16 **ptr, UINT16 *buffer_size, UINT8 session_id);
    void Cust_get_pointer_notify(UINT8 session_id);
    UINT8 Cust_get_next_buffer_status(void);
  #endif
#if(L1_BT_AUDIO==1)
void audio_bt_cfg_manager (T_RV_HDR *p_message);
#endif
  /* audio mode prototype */
  void  audio_mode_full_access_write_manager    (T_RV_HDR *p_message);
  void  audio_mode_load_manager                 (T_RV_HDR *p_message);
  void  audio_mode_save_manager                 (T_RV_HDR *p_message);
  void audio_mode_speaker_volume_manager        (T_RV_HDR *p_message);
  void audio_mode_stereo_speaker_volume_manager (T_RV_HDR *p_message);
  UINT8 audio_mode_message_switch               (T_RV_HDR *p_message);

  DummyStruct *audio_allocate_l1_message(UINT16 size);
  void audio_deallocate_l1_message(DummyStruct *message);
  T_RV_RET audio_send_l1_message(INT16 message_id, DummyStruct *message);
#endif
