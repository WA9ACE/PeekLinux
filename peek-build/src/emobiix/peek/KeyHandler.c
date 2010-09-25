#ifndef SIMULATOR
#include "general.h"
#include "typedefs.h"
#include "KeyMap.h"
#include "Debug.h"
#include "Sounds.h"
#endif

#include "Transport.h"
#include "ConnectionContext.h"
#include "DataObject.h"
#include "Platform.h"
#include "Debug.h"
#include "ApplicationManager.h"
#include "lgui.h"
#include "KeyMappings.h"
#include "RenderManager.h"

#include "List.h"
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

#include "kpd_cfg.h"
#include "hwtask.h"
#include "emo_kbd.h"
#include "ui_pei.h"
#include "p_sim.h"

List *netsurf_get_queue()
{
	static List *netsurf_key_queue = NULL;
	if (!netsurf_key_queue)
		netsurf_key_queue = list_new();

	return netsurf_key_queue;
}

static void netsurf_key_enqueue(unsigned short key, unsigned short state)
{
	List *keyqueue = netsurf_get_queue();
	unsigned int keystate = state << 16 | key;

	list_append(keyqueue, (void *)keystate);
}

typedef UINT16 RegIdT;
void KeyPad_Init(void);

#ifdef TI_PS_HCOMM_CHANGE
#define PSENDX(A,B) PSEND(_hComm##A,B)
#else
#define PSENDX(A,B) PSEND(hComm##A,B)
#endif /* TI_PS_HCOMM_CHANGE */

typedef enum
{
    BAL_KEY_PRESS,
    BAL_KEY_RELEASE,
    BAL_KEY_HOLD,
    BAL_KEY_STATUS_NUM
}BalKeyStatusT;

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
	renderman_flush();
	manager_drawScreen();
	dspl_Enable(0);
	if (!lgui_is_dirty())
		return;

	upper = lgui_index_count();
	if (upper == 0) {
		//emo_printf("Flipping entire screen" NL);
		emo_BitBltFull(screenBuf);
	} else {
		Rectangle *rect;
		int i;
		for (index = 0; index < upper; ++index) {
			rect = lgui_get_region(index);
			//emo_printf("Flipping partial screen: %d, %d %d %d %d" NL, index, rect->x, rect->y, rect->width, rect->height);
			emo_BitBltPartial(rect, screenBuf);
		}
	}
	dspl_Enable(1);
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
	char buf[256];
	FILE *input;
	int i;

	dataobject_platformInit();
	input = fopen("home-server.ini", "rb");
	if (input != NULL) {
		fgets(buf, 256, input);
		fclose(input);
		i = strlen(buf)-1;
		while (i > 0 && !isprint(buf[i])) {
			buf[i] = 0;
			--i;
		}
		url = url_parse(buf, URL_ALL);
	} else {
		url = url_parse("tcp://69.114.111.9:12345/dataobject", URL_ALL);
	}

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

int kbdEmobiixCB(T_EMOBIIX_KBD_EVENT *event)
{
	switch (event->key)
	{
		//case KCD_CANCLE:
	//		break;

		case KPD_KEY_ENTER:
			manager_handleKey(KPD_KEY_ENTER);
			updateScreen();
			break;

		case KPD_KEY_UP:
			manager_handleKey(KPD_KEY_UP);
			updateScreen();
			break;

		case KPD_KEY_DOWN:
			manager_handleKey(KPD_KEY_DOWN);
			updateScreen();
			break;

		default:
			manager_handleKey(MapKeyToInternal(event->key));
			updateScreen();
			break;
	}

	PFREE(event);
	return 1;
}

static int kdbEventCB(hwEventType eventType, void *eventData, void *context)
{
	KBD_EVENT *keyEvent = (KBD_EVENT *)eventData;
	PALLOC(event, EMOBIIX_KBD_EVENT);

	event->context = context;
	event->key = keyEvent->key;
	event->state = keyEvent->state;

	PSEND(hCommUI, event);
	return 1;
}

void emobiixKbdInit()
{
	hwEventSubscribe(HW_KEYBOARD_EVENT, kdbEventCB, 0);
}

int netsurf_start_flag = 0;

#ifdef EMO_SIM
void UiHandleKeyEvents(RegIdT RegId, UINT32 MsgId, void *MsgBufferP)
#else
static void UiHandleKeyEvents(RegIdT RegId, UINT32 MsgId, void *MsgBufferP)
#endif
{
        int UiKeyId = *((int *)MsgBufferP);
		T_EMOBIIX_NETSURF_START *netStart;

        emo_printf("UiHandleKeyEvents MsgId=%d KeyId=%d" NL, MsgId, UiKeyId);
		if(netsurf_start_flag) {
			netsurf_key_enqueue(UiKeyId, MsgId);
			return;
		}
		switch(UiKeyId) {
			case SYS_PWR_KEY:
				emo_printf("Starting power off\n");
				pwr_PowerOffMobile();
				while(1); // Shouldn't get here
			case SYS_SHIFT:
				/* Dont pass through shift.. wait for combo */
				break;
			case SYS_WHEEL_BACK:
			case SYS_WHEEL_FORWARD:
				emo_printf("UiHandleKeyEvents() Passing to manager key [%c]", MapKeyToInternal(UiKeyId));
				manager_handleKey(MapKeyToInternal(UiKeyId));
				break;
			default:
				if (MsgId == BAL_KEY_PRESS)
				{
					emo_printf("UiHandleKeyEvents() Passing to manager key [%c]", MapKeyToInternal(UiKeyId));
					manager_handleKey(MapKeyToInternal(UiKeyId));
				}
		}
        updateScreen();
}

void netsurfStart(void) {
     T_EMOBIIX_NETSURF_START *netStart;

     netsurf_start_flag = TRUE;
     netStart = P_ALLOC(EMOBIIX_NETSURF_START);
     P_OPC(netStart) = EMOBIIX_NETSURF_START;
     PSENDX(UI, netStart);
}


void KeyPad_Init(void) {

     if(BalKeypadRegister(UiHandleKeyEvents) == -1) {
        emo_printf("BalKeypadRegister() Failed to register handler" NL);
        return;
     }
     //emo_printf("BalKeypadRegister() registered" NL);
}

