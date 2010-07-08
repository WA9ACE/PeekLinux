#include "bspTwl3029_Aux.h"
#include "exeapi.h"

ExeTimerT VibrateTimerOut;

void VibratorOn(void) {
        bspTwl3029_Aux_VibEnable(0,1);
}

void VibratorOff(void) {
        bspTwl3029_Aux_VibEnable(0,0);
}

void VibrateTimerOutHandler(uint32 TimerId)
{
        VibratorOff();
        //ExeMsgSend(EXE_BAL_ID, EXE_MAILBOX_1_ID, 0x19C8, 0x0, 0x0); // Typically BAL TASK would turn this off..
}

void VibratorTimeOut(uint32 interval) {

	ExeTimerStop(&VibrateTimerOut);
	ExeTimerDelete(&VibrateTimerOut);
	
	VibratorOn();
	ExeTimerCreate(&VibrateTimerOut, VibrateTimerOutHandler, 0x71, ExeCalMsec(interval), 0);
	ExeTimerStart(&VibrateTimerOut);
}
