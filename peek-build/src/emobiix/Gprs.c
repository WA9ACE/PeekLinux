#include "Gprs.h"
#include "Debug.h"


/**
 *  GPRS call back function
 *  High-4 bit indicates the device signal and the Low-4 bit indicated GPRS attached whether.
 *
**/

static void rssiEventHandler(uint8 ucMessage)
{
  static bool mGprs = 0;
  bool  bGprs;
  static uint8 mSignal=0; //XXX: Fix we should or it against system wide value


  uint8 uSignalV = (ucMessage&0x0F); // Get Signal bits
  uint8 uGprsFlg = (ucMessage>>4)&0x0F; // Get GPRS status


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
          } else{
		// GPRS detached
		// Signal UI to display detached view
          }
  }

  emo_printf("rssiEventHandler[0x%x]",ucMessage);
}

void GprsRegisterRssi(void) {
       BalRssiRegister(rssiEventHandler);
}
