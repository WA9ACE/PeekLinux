#include "general.h"
#include "typedefs.h"
#include "Lcd_manager.h"
#include "Task.h"
#include "UITask.h"
#include "mfw_mfw.h"
#include "mfw_mme.h"
#include "Debug.h"
#include "File.h"

//#ifdef DAR_HALT
#include "ffs.h"
//#endif

typedef struct {
        unsigned int eSndAndAlert;
        unsigned int eBKProfile;
        unsigned int eDispTheme;
        unsigned int eFlightMode;
}SettingModeHelplerInfo;

SettingModeHelplerInfo stCurSettingMode;

unsigned int timer_state = 0;

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

int gprsAttached = 0;
extern void mmiInit(void);

void EmoTask(void) {
	int tp;
#ifdef DAR_HALT
	File *fp;
#endif 
        //BalMemInit();
#ifdef DAR_HALT
	fp = file_openRead("/var/dbg/dar");
	if(fp){
		if(file_size(fp) > 0) {
			emo_printf("Dar crash file detected. Going to sleep");
			file_close(fp);
			while(1) {
				lcd_led_onoff(1);
				TCCE_Task_Sleep(1000);
				lcd_led_onoff(0);
			}	
		}
	}
#endif

    	EmoStatusSet();
	//stCurSettingMode.eFlightMode = 0; // Set to normal mode
	while(1) {
#ifdef EMO_SIM
#if 0
	if(uiStatusGet()) {
        	int key = SimReadKey();
        	int hasData = SimReadReg();

        	if(key)
                	simKey(key);

        	if (hasData > 0)
                	app_recv();
	}
#endif
        TCCE_Task_Sleep(20);
#else

        TCCE_Task_Sleep(1000);
#endif
	}
}
