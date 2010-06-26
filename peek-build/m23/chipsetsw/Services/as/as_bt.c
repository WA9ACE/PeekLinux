



#include "as/as_i.h"
#include "as/as_api.h"
#include "as/as_processing.h"
#include "as/as_audio.h"
#include "as/as_utils.h"
#include "as/as_bt.h"
#include "rvf/rvf_api.h"

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif


T_AS_RET  as_bt_status(void)
{
   T_AS_RET rc = (T_AS_RET)AS_OK;
   T_AUDIO_RET audio_return = AUDIO_OK;
T_AUDIO_BT_PARAMETER bt_param;
   
bt_param.connected_status= as_ctrl_blk->bt_connect_status;
  
//audio_return= audio_bt_cfg (&bt_param);
audio_return = audio_bt_cfg(&bt_param, as_ctrl_blk->rp);
    if (audio_return != AUDIO_OK)
    {
       AS_TRACE_L(ERROR, "bt_connection_cfg_status() error=", audio_return);
        rc =(T_AS_RET) AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "bt_connection_cfg_status() completed");
        
        // Everything is OK. 
	 //AS_CHANGE_STATE(AS_STATE_IDLE);
	
    }
   return (T_AS_RET)rc;
	
    }
