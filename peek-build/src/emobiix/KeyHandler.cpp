#ifndef SIMULATOR
#include "KeyMap.h"
#include "Debug.h"
#include "buidcc.h"
#include "balkeypad.h"
#include "ballightapi.h"
#include "Sounds.h"
#include "balapi.h"
#include "tweet.h"
#endif

#include "Transport.h"
#include "ConnectionContext.h"
#include "DataObject.h"
#include "Platform.h"
#include "Debug.h"
#include "lgui.h"

#include "p_malloc.h"

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif

#define BWIDTH 320
#define BHEIGHT 240

extern "C" {
extern unsigned char *screenBuf;



void updateScreen(void) {
#ifndef SIMULATOR
		static DCC dc;
        static BalDispBitmapT Bmp;
        Bmp.Width = BWIDTH;
        Bmp.Height = BHEIGHT;
        Bmp.BitCount = BAL_DISP_BIT_COUNT_16;
        Bmp.DataBuf = (unsigned char *)screenBuf;
        if (screenBuf == NULL) {
                emo_printf("\n RYAN screenbuf is NULL here\n\n");
                return;
        }
        emo_printf("\n RYAN Calling tweetDrawScreen() \n\n");
        tweetDrawScreen();

        if (!lgui_is_dirty())
            return;

        int index, upper;
        upper = lgui_index_count();
        if (upper == 0) {
            emo_printf("Flipping entire screen\n");
            dc.BitBlt(0,0,BWIDTH,BHEIGHT,Bmp,0,0);
            dc.UpdateDisplay();
        } else {
            Rectangle *rect;
            for (index = 0; index < upper; ++index) {
                rect = lgui_get_region(index);
		    emo_printf("Flipping partial screen: %d\n", index);

                dc.BitBlt(rect->x, rect->y, rect->width, rect->height, Bmp,
                        rect->x, rect->y);
            }
            dc.UpdateDisplay();
        }
	lgui_blit_done();
#endif
}

#ifdef SIMULATOR
extern "C" {
ConnectionContext *connectionContext;
}

void main_test(void)
{
	Endpoint *ep;
	URL *url;
	DataObject *dobj;
	Transport *transport;
	ConnectionContext *ctx;
	int hasPrinted;
	//char buf[256];

	dataobject_platformInit();
	url = url_parse("tcp://69.114.111.9:12345/dataobject", URL_ALL);

	transport = transport_get(url->scheme);
	if (transport == NULL) {
		emo_printf("No 'tcp' transport available");
		return;
	}

	emo_printf("ANDREY::Calling transport->socket())");

	ep = transport->socket();
	if (ep == NULL) {
		emo_printf("Failed Transport::Socket(), error code: %d",
#ifdef SIMULATOR
			0
#else
			0 //bal_get_socket_last_error()
#endif
			);
		return;
	}

	emo_printf("ANDREY::Calling transport->connect())");
	if (transport->connect(ep, url) < 0) {
		emo_printf("Connect failed");
		return;
	}

	ctx = connectionContext_new(ep);
	connectionContext = ctx;
	//connectionContext_syncRequest(ctx, url);

        dobj = dataobject_locate(url);
        hasPrinted = 0;
	while (1) 
	{
		connectionContext_loopIteration(ctx);
#ifdef SIMULATOR
		Sleep(100);
#else
		NU_Sleep(100);
#endif
/*                if (!hasPrinted && dataobject_getState(dobj) == DOS_OK) {
                        dataobject_debugPrint(dobj);
                        hasPrinted = 1;
                }
	        updateScreen();*/
	}
}
#endif

#ifndef SIMULATOR

}
static void UiHandleKeyEvents(RegIdT RegId, uint32 MsgId, void *MsgBufferP)
{
        BalLightLevelT lcdstatus;
        BalLightLevelT kpdstatus;
        UiKeypadKeyIdT UiKeyId = (UiKeypadKeyIdT)*((SysKeyIdT*) MsgBufferP);
#ifdef EMO_SIM
        emo_printf("UiHandleKeyEvents MsgId=%d,UiKeyId=%d\r\n", MsgId, UiKeyId);
#endif
        //if (MsgId == 1) {
     //           SetSettingValue(SET_UI_RINGTONE, 2);
   //             SetSettingValue(SET_UI_VOLUME, 5);
 //               Sounds_PlayMidi();
       //         flash_led();
                //main_test();
		tweetKey(UiKeyId);
		updateScreen();
        //}
#ifndef EMO_SIM
        BalGetLightStatus(BAL_LIGHT_LCD,&lcdstatus,0);
        BalGetLightStatus(BAL_LIGHT_KEYPAD,&kpdstatus,0);
#endif
}
extern "C" {
void KeyPad_Init(void) {

     if(BalKeypadRegister(UiHandleKeyEvents) == -1) {
	emo_printf("BalKeypadRegister() Failed to register handler\n");
	return;
     }
     emo_printf("BalKeypadRegister() registered\n");
}
#endif
}

