/* HW Task */

#include "nucleus.h"
#include "typedefs.h"
#include "header.h"
#include "Armio.h"
#include "p_em.h"
#include "p_mncc.h"
#include "p_mmreg.h"
#include "p_mnsms.h"
#include "m_fac.h"
#include "p_8010_137_nas_include.h"
#include "p_mmi.h"
#include "aci_cmh.h"
#include "p_8010_152_ps_include.h"
#include "gaci_cmh.h"
#include "p_dl.h"
#include "ffs.h"

#include "exeapi.h"
#include "File.h"

/* Defines */
#define BSP_UICC_DEFAULT_READER_ID 0x01


/* 
 * Setup and start hardware
 * 
 */

extern int recvProcess;

extern int uiGetStatus(void);
extern void setEntryStub(void);

extern void BalGetImei(char *ImeiBuffer);

void hwStart(void) {
    BOSEventWaitT EvtStatus;
    bool          MsgStatus;
    uint32        MsgId;
    uint32        MsgSize;
    void          *MsgBufferP;
    uint8         MailBoxId;
    BOSEventWaitT MailBoxIndex;
    char ImeiBuffer[15];
    int i;
	int key,state, hasData;
#ifdef DAR_HALT
    File *fp;
#endif
	peek_file_init();

#ifdef DAR_HALT
		if(simAutoDetect()) 
		{
			fp = file_openRead("/var/dbg/dar");
			if(fp){
				if(file_size(fp) > 0) {
					emo_printf("Dar crash file detected. Going to sleep");
					file_close(fp);
					while(1) {
						lcd_led_onoff(1);
						TCCE_Task_Sleep(10);
						lcd_led_onoff(0);
					}
				}
			}
	  }
#endif

 // Setup time/date
 hw_td_init();

 // Unlock UI
 //HwStatusSet();
 //emo_printf("hwStart() time: %s", hw_td_get_clock_str());

 if(simAutoDetect()) {
	  BalMiscInit();
		emo_printf("IMEI: %s", EmoGetImei());
 // Enter main notify loop
    while(1) {
				HwStatusSet();
        EvtStatus = BOSEventWait(EXE_BAL_ID, BOS_SIGNAL_TRUE, BOS_MESSAGE_TRUE,BOS_TIMEOUT_FALSE);//BOSCalMsec(10000)
        if(EvtStatus & BOS_MESSAGE_TYPE)
        {
			//emo_printf("hwStart() got MSG");
            for(MailBoxIndex=BOS_MAILBOX_1,MailBoxId = BOS_MAILBOX_1_ID; MailBoxId<2; MailBoxId++)
            {
                if(EvtStatus & MailBoxIndex)
                {
                    //get the Msg value
                    MsgStatus = BOSMsgRead(EXE_BAL_ID, (BOSMailboxIdT)MailBoxId, &MsgId, &MsgBufferP, &MsgSize);
            		//emo_printf("hwStart() read MSG for mailbox %d", MailBoxId);

                    if(MsgStatus)
                    {	
                        //emo_printf("hwStart() handling msg id[%d]", MsgId);
						switch(MsgId) {
							case HW_KEY_MSG:
                //emo_printf("hwStart(): Got HW_KEY_MSG press");
								BalKeypadProcess(MsgId, MsgBufferP, MsgSize);
								break;
						}
                        if (MsgBufferP != NULL)
                        {
                            //XXX: should we free buffer?
                            BOSMsgBufferFree(MsgBufferP);
                            MsgBufferP = NULL;
                        }
                    }
                }
                MailBoxIndex = (BOSEventWaitT)(MailBoxIndex << 1);
            }
        }

	}

 /* Must be in emulator */
 } else  {
	setRecvProcess(1);
	while(1) {

			if(uiGetStatus())
			{
				key = SimReadKey();
				state = SimReadKeyState();

				if(!getRecvProcess()) {
					hasData = SimReadReg();
					if (hasData > 0) {
						app_sim_recv();
						setRecvProcess(1);
					}
				}
				if(key) {
					UiHandleKeyEvents(state, key);
				}
			}

			TCCE_Task_Sleep(50);
	}
 }
}
/* 
 * Shutdown Peek
 *
 */

void hwStop(int delay) {
	 simdrv_poweroff(BSP_UICC_DEFAULT_READER_ID);
	 AI_Power(0);
}
