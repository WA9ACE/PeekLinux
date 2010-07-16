#include "typedefs.h"
#include "Task.h"
#include "UITask.h"
#include "LcdControl.h"
#include "mfw_mfw.h"
#include "mfw_mme.h"
#include "Debug.h"
#include "Gprs.h"
#include "lgui.h"
#include "tweet.h"
#include "msg.h"

/* For CSQ */
#include "p_mmi.h"
#include "m_fac.h"
#include "p_mmreg.h"
#include "p_mncc.h"
#include "p_mnsms.h"
#include "p_em.h"
#include "aci_lst.h"
#include "aci_cmh.h"

void backlightInit() {
         int i;
         for (i=0;i<BL_LAST_OPTION;i++) {
                mme_setBacklightEvent(i,BL_MAX_LIGHT);
         }
         mme_setBacklightEvent(BL_IDLE_TIMER,BL_SET_IDLE);
         mme_setBacklightEvent(BL_KEY_PRESS,BL_MAX_LIGHT);
         mme_setBacklightEvent(BL_INCOMING_CALL,BL_MAX_LIGHT);
         mme_setBacklightEvent(BL_EXIT,BL_NO_LIGHT);

         /*and tell the backlight that the init event has happened */
         mme_backlightEvent(BL_INIT);
}

extern void mmiInit(void);

void EmoTask(void) {

	BalMemInit();
	//backlightInit();
        //display_init();

	/*
        sim_init();
        nm_init();
        sim_activate();
        sAT_PercentCSQ ( CMD_SRC_LCL, CSQ_Enable );
	*/

        //EmoStatusSet();
  	mmiInit();

	while(1) {
		TCCE_Task_Sleep(50);
	}
}
