#include "Gprs.h"
#include "Debug.h"
#include "exeapi.h"
#include "bal_os.h"
#include "balapi.h"
#include "msg.h"

#include "DataObject.h"
#include "URL.h"


/**
 *  GPRS call back function
 *  High-4 bit indicates the device signal and the Low-4 bit indicated GPRS attached whether.
 *
 **/

static uint8 mSignal;
static bool mGprs;

static DataObject* GPRS_DO;
static DataObjectField* GPRS_SIGNAL_LEVEL;
static DataObjectField* GPRS_STATUS;

static void gprs_set_status(bool status)
{
        if (!GPRS_DO)
                return;

        GPRS_STATUS->field.uinteger = status;
        dataobject_setValue(GPRS_DO, "status", GPRS_STATUS);
}

static void gprs_set_signal_level(uint32 level)
{
        if (!GPRS_DO)
                return;

        emo_printf("GPRS Signal Level Update: %d", level);
        GPRS_SIGNAL_LEVEL->field.uinteger = level;
        dataobject_setValue(GPRS_DO, "signal-level", GPRS_SIGNAL_LEVEL);
}

static void rssiEventHandler(RegIdT RegId, uint32 MsgId, void* MsgBufferP)
{
  bool  bGprs;
  UIMsg *tmpMsg = (UIMsg *)BOSMsgBufferGet(sizeof(UIMsg));
  uint8 ucMessage=0;
  uint8 uSignalV =0;
  uint8 uGprsFlg =0;

  BalL1dRssiRptMsgT *rMsg = (BalL1dRssiRptMsgT *)MsgBufferP;

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
	/* Send UI update for Signal */
	gprs_set_signal_level(mSignal);
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
		emo_printf("rssiEventHandler() GPRS_ATTACHED\n");
		BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_2_ID, UI_RSSI_REG, (void *)tmpMsg, sizeof(UIMsg));
		gprs_set_status(GPRS_REGISTERED);
          } else{
		// GPRS detached
		emo_printf("rssiEventHandler() GPRS_DETACHED\n");
		BOSMsgSend(BOS_UI_ID, BOS_MAILBOX_2_ID, UI_RSSI_DEREG, (void *)tmpMsg, sizeof(UIMsg));
		gprs_set_status(GPRS_NOT_REGISTERED);
		// Signal UI to display detached view
          }
  }
}

void GprsRegisterRssi(void) {

       URL *url = url_parse(GPRS_URI, URL_ALL);
       mSignal=0;
       mGprs = 0;

       GPRS_DO = dataobject_construct(url, 1);
       GPRS_SIGNAL_LEVEL = dataobjectfield_uint(GPRS_NOT_REGISTERED);
       GPRS_STATUS = dataobjectfield_uint(GPRS_NOT_REGISTERED);


       BalRssiRegister(rssiEventHandler);
}
