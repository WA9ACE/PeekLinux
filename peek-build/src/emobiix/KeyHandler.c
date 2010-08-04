#ifndef SIMULATOR
#include "general.h"
#include "typedefs.h"
#include "KeyMap.h"
//#include "Debug.h"
#include "ballightapi.h"
#include "Sounds.h"
#include "tweet.h"
#endif

#include "Transport.h"
#include "ConnectionContext.h"
#include "DataObject.h"
#include "Platform.h"
#include "Debug.h"
#include "ApplicationManager.h"
#include "lgui.h"

#include "p_malloc.h"

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif

#ifndef SIMULATOR
#include "dspl.h"
#endif

#define BWIDTH 320
#define BHEIGHT 240

extern unsigned char *screenBuf;
extern unsigned char pwr_PowerOffMobile   (void);
extern U8* get_LCD_bitmap(void);

void emo_BitBltPartial(Rectangle *rect, U8 *bmp)
{
	U8 *dbmp = get_LCD_bitmap();
	int yy, offset;
	
	for (yy = rect->y; yy < rect->y + rect->height; ++yy)
	{
		offset = (BWIDTH * yy + rect->x) * 2;
		memcpy(dbmp + offset, bmp + offset, rect->width * 2);
	}
}

void emo_BitBltFull(U8* bmp)
{
	U8 *dbmp = get_LCD_bitmap();
	memcpy(dbmp, bmp, BWIDTH * BHEIGHT * 2);
}

void updateScreen(void) {
#ifndef SIMULATOR
	int index, upper;
	manager_drawScreen();

	dspl_Enable(0);
	if (!lgui_is_dirty())
		return;

	upper = lgui_index_count();
	if (upper == 0) {
		emo_printf("Flipping entire screen" NL);
		emo_BitBltFull(screenBuf);
	} else {
		Rectangle *rect;
		int i;
		for (index = 0; index < upper; ++index) {
			rect = lgui_get_region(index);
			emo_printf("Flipping partial screen: %d, %d %d %d %d" NL, index, rect->x, rect->y, rect->width, rect->height);
			emo_BitBltPartial(rect, screenBuf);
		}
	}
	//dspl_Enable(1);
	lgui_blit_done();
#endif
}

#ifdef SIMULATOR
ConnectionContext *connectionContext;

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
		emo_printf("No 'tcp' transport available" NL);
		return;
	}

	emo_printf("ANDREY::Calling transport->socket())" NL);

	ep = transport->socket();
	if (ep == NULL) {
		emo_printf("Failed Transport::Socket(), error code: %d" NL,
#ifdef SIMULATOR
			0
#else
			0 //bal_get_socket_last_error()
#endif
			);
		return;
	}

	emo_printf("ANDREY::Calling transport->connect())" NL);
	if (transport->connect(ep, url) < 0) {
		emo_printf("Connect failed" NL);
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

static void UiHandleKeyEvents(RegIdT RegId, uint32 MsgId, void *MsgBufferP)
{
        BalLightLevelT lcdstatus;
        BalLightLevelT kpdstatus;
        UiKeypadKeyIdT UiKeyId = (UiKeypadKeyIdT)*((SysKeyIdT*) MsgBufferP);
#ifdef EMO_SIM
        emo_printf("UiHandleKeyEvents MsgId=%d,UiKeyId=%d" NL, MsgId, UiKeyId);
#endif
	if(UiKeyId == KP_PWR_KEY) {
		emo_printf("Starting power off\n");
		pwr_PowerOffMobile();

	}
        if (MsgId == 1) {
     //           SetSettingValue(SET_UI_RINGTONE, 2);
   //             SetSettingValue(SET_UI_VOLUME, 5);
 //               Sounds_PlayMidi();
       //         flash_led();
                //main_test();
		/*tweetKey(UiKeyId);*/
        manager_handleKey(UiKeyId);
		updateScreen();
        }
#ifndef EMO_SIM
        //BalGetLightStatus(BAL_LIGHT_LCD,&lcdstatus,0);
        //BalGetLightStatus(BAL_LIGHT_KEYPAD,&kpdstatus,0);
#endif
}
void KeyPad_Init(void) {

     /*
     if(BalKeypadRegister(UiHandleKeyEvents) == -1) {
	emo_printf("BalKeypadRegister() Failed to register handler" NL);
	return;
     }
     emo_printf("BalKeypadRegister() registered" NL);
     */
}
#endif
