#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
#ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
#endif

  /* include the usefull L1 header */
  #include "l1_confg.h"
  #include "l1sw.cfg"

#if (L1_BT_AUDIO == 1)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"

  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_features_i.h"

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
  #include "rfs/fscore_types.h"
#else
  #include "audio/audio_ffs_i.h"
#endif

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"

  #include "audio/audio_api.h"
  #include "audio/audio_macro_i.h"

void audio_bt_cfg_manager (T_RV_HDR *p_message)
{
    /* Declare local variables.                                                   */
    DummyStruct *p_send_message;

        switch(p_message->msg_id)
          {
             case AUDIO_BT_CFG_REQ:
              {
                /* save the addr id of the entity */
                p_audio_gbl_var->bt.task_id = p_message->src_addr_id;

 // save the return path 
               p_audio_gbl_var->bt.return_path.callback_func = ((T_AUDIO_BT_CFG_REQ*)(p_message))->return_path.callback_func;
               p_audio_gbl_var->bt.return_path.addr_id       = ((T_AUDIO_BT_CFG_REQ*)(p_message))->return_path.addr_id;

                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message (0);

//              ((T_MMI_BT_CFG_REQ *)( p_send_message))->connected_status = 
                if(((T_AUDIO_BT_CFG_REQ *)p_message)->bt_parameter.connected_status==TRUE)
                {
                    if ( p_send_message != NULL)
                   {
                    /* send the start command to the audio L1 */
                    audio_send_l1_message(MMI_BT_ENABLE_REQ, p_send_message);
                   }
               }
               else if(((T_AUDIO_BT_CFG_REQ *)p_message)->bt_parameter.connected_status==FALSE)
              {
                   if ( p_send_message != NULL)
                  {
                    /* send the start command to the audio L1 */
                     audio_send_l1_message(MMI_BT_DISABLE_REQ, p_send_message);
                  }
              }

              break;  }   }}

 #endif /* BT_AUDIO */
#endif /* RVM_AUDIO_MAIN_SWE */













