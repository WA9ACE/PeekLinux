#include "Gprs.h"
#include "Debug.h"
#include "exeapi.h"
#include "bal_os.h"
#include "balapi.h"
#include "msg.h"

/**
 *  GPRS call back function
 *  High-4 bit indicates the device signal and the Low-4 bit indicated GPRS attached whether.
 *
 **/

static void rssiEventHandler(RegIdT RegId, uint32 MsgId, void* MsgBufferP)
{
	static bool mGprs = 0;
	bool  bGprs;
	UIMsg tmpMsg;
	uint8 ucMessage=0;
	uint8 uSignalV =0;
	uint8 uGprsFlg =0;

	BalL1dRssiRptMsgT *rMsg = (BalL1dRssiRptMsgT *)MsgBufferP;

	static uint8 mSignal=0; //XXX: Fix we should or it against system wide value

	ucMessage = (uint8)rMsg->Rssi;

	uSignalV = (ucMessage&0x0F); // Get Signal bits
	uGprsFlg = (ucMessage>>4)&0x0F; // Get GPRS status


	emo_printf("rssiEventHandler[0x%x] - uSignalV [0x%x] - uGprsFlg [0x%x]\n",ucMessage, uSignalV, uGprsFlg);

	if(uSignalV>0 && uSignalV != mSignal){
		if(mSignal == SIGNAL_FLIGHT_M){
			if(uSignalV != SIGNAL_FLIGHT_M){
				emo_printf("rssiEventHandler: Flight mode updated [%d]",uSignalV);
			}
		} else {
			if(uSignalV == SIGNAL_FLIGHT_M){
				emo_printf("rssiEventHandler: Flight mode updated [%d]",uSignalV);
			}
		}
		/*Save signal value*/
		mSignal = uSignalV;
	}

	/*Check GPRS attached*/
	bGprs = (uGprsFlg==GPRS_REGISTERED||uGprsFlg==GPRS_ROAMING)&&(uSignalV > 0);

	/*Do GPRS changed process*/
	if(bGprs!= mGprs){
		/*Change GPRS flag*/
		mGprs = bGprs;
		/*Check GPRS attached whether*/
		if(bGprs){
			// GPRS Attached
			// Signal UI to display attached view
			tmpMsg.msgA = 1;
			emo_printf("rssiEventHandler() GPRS_ATTACHED\n");
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_2_ID, UI_RSSI_REG, (void *)&tmpMsg, sizeof(UIMsg));
		} else{
			// GPRS detached
			tmpMsg.msgA = 0;
			emo_printf("rssiEventHandler() GPRS_DETACHED\n");
			BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_2_ID, UI_RSSI_DEREG, (void *)&tmpMsg, sizeof(UIMsg));
			// Signal UI to display detached view
		}
  }
}

void GprsRegisterRssi(void) {
	BalRssiRegister(rssiEventHandler);
}
