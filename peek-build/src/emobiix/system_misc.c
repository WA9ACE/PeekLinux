#include "bspTwl3029_Aux.h"
#include "exeapi.h"

ExeTimerT VibrateTimerOut;

void VibrateTimerOutHandler(uint32 TimerId)
{
         bspTwl3029_Aux_VibEnable(0,0);
        //ExeMsgSend(EXE_BAL_ID, EXE_MAILBOX_1_ID, 0x19C8, 0x0, 0x0);
}

void VibratorTimeOut(uint32 interval) {

	ExeTimerStop(&VibrateTimerOut);
	ExeTimerDelete(&VibrateTimerOut);
	
	bspTwl3029_Aux_VibEnable(0, 1);
	ExeTimerCreate(&VibrateTimerOut, VibrateTimerOutHandler, 0x71, ExeCalMsec(interval), 0);
	ExeTimerStart(&VibrateTimerOut);
}

void VibratorOn(void ) {
	bspTwl3029_Aux_VibEnable(0,1);
}

void VibratorOff(void) {
	bspTwl3029_Aux_VibEnable(0,0);
}
