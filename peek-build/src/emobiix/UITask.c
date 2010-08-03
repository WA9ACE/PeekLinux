#include "general.h"
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

#include "Transport.h"
#include "ConnectionContext.h"
#include "Platform.h"

#include "DataObject.h"
#include "Application.h"
#include "ApplicationManager.h"

#include "gdi.h"
#include "dspl.h"
#include "p_sim.h"

#include "system_battery.h"

#include "mfw_mfw.h"
#include "mfw_tim.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_sat.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiMenu.h"

unsigned char *screenBuf; //[320*240*2];
void updateScreen(void);
int UIInit(void);
static int UIIteration(void);
int UIWaitForActivity(void);
static void UICleanup(void);
ConnectionContext *connectionContext;
extern void appProtocolStatus(int status);

Task UITask_s = {UIInit, UIIteration, UIWaitForActivity, UICleanup};

DataObject *systemAppObject;
Application *systemApplication;

extern int gprsAttached;

void uiAppConn(void *connData)
{
	Endpoint *ep;
	URL *url;
	Transport *transport;

	emo_printf("uiAppConn()");

	url = url_parse("tcp://10.150.9.6:12345/dataobject", URL_ALL);

	transport = transport_get(url->scheme);
	if (transport == NULL) {
		emo_printf("No 'tcp' transport available");
		NU_Sleep(100);
		return;
	}

	ep = transport->socket();
	transport->connect(ep, url);

	connectionContext = connectionContext_new(ep);
	if(!connectionContext) {
		emo_printf("uiAppConn() Failed to allocate connectionContext");
		return;
	}

	//connectionContext_syncRequest(connectionContext, url);
	//connectionContext_loopIteration(connectionContext);
}


void uiAppSent()
{
	emo_printf("uiAppSent() Looping...");
	connectionContext_loopIteration(connectionContext);
}

void uiAppRecv(void *recvData)
{
	T_EMOBIIX_WRITEMSG *writeMsg = (T_EMOBIIX_WRITEMSG *) recvData;
	int status = 1;

	emo_printf("uiAppRecv()");

	connectionContext_setBuffer(connectionContext, (char *)writeMsg->data, writeMsg->size);

	while (status > 0)
	{
		status = connectionContext_consumePacket(connectionContext);
		emo_printf("uiAppRecv() Consumed %d bytes" NL, status);
	}

	connectionContext_loopIteration(connectionContext);
	if(emoMenu_get_window())
		SEND_EVENT(emoMenu_get_window(), SCREEN_UPDATE, 0, 0);
}

GLOBAL BOOL appdata_response_cb (ULONG opc, void * data)
{
	//emo_printf("appdata_response_cb(): %d (%08X)", opc, opc);

	switch(opc) 
	{
		case EMOBIIX_SOCK_RECV: // Data received 
			emo_printf("appdata_response_cb(): APP_DATA_RECV");
			uiAppRecv(data);
			/* Free the p_malloc'd buffer that we created in app_core */
			p_free(((T_EMOBIIX_SOCK_RECV *)data)->data);
			//PFREE(data);
			return TRUE;

		case EMOBIIX_SOCK_CREA: // Sock created
			emo_printf("appdata_response_cb(): APP_DATA_CREA");
			break;

		case EMOBIIX_SOCK_SENT: // Sock data sent
			emo_printf("appdata_response_cb(): APP_DATA_SENT");
			uiAppSent();
			break;

		case EMOBIIX_SOCK_CONN: // Sock connected
			emo_printf("appdata_response_cb(): APP_DATA_CONN");
			uiAppConn(data);
			//PFREE(data);
			return TRUE;

		case EMOBIIX_SOCK_DCON: // Sock disconnected
			emo_printf("appdata_response_cb(): APP_DATA_DCON");
			appProtocolStatus(0);
			break;

		default:
			break;
	}

	/* Not handled by this extention */
	return FALSE;
}

void display_init()
{
	dspl_DevCaps displayData;
	UBYTE ret = dspl_Init();
	emo_printf("Initializing display driver");
	if (ret != DRV_OK)
	{
		emo_printf("Display driver initialization failed: %d", ret);
		return;
	}

	displayData.DisplayType = DSPL_TYPE_GRAPHIC;

	dspl_SetDeviceCaps(&displayData);
	dspl_GetDeviceCaps(&displayData);

	emo_printf("Display parameters are %dx%d", displayData.Width, displayData.Height);
}

void UITask(void)
{
	task_threadFunction(&UITask_s);

	return;
}

extern BOOL powered_on;

int UIInit(void)
{	

	static int initd = 0;

	uiStatusSet();

	if (!initd) {
		screenBuf = (unsigned char *)p_malloc(320*240*2);
		if(!screenBuf)
			emo_printf("Failed to allocate screenBuf\n");

		lgui_attach(screenBuf);
		manager_init();
		initd = 1;

		manager_drawScreen();
		updateScreen();

		dataobject_platformInit();
	}

	return 1;
}

static int UIIteration(void)
{
	return 1;
}

int UIWaitForActivity(void)
{
	NU_Sleep(100);
#if 0
	Endpoint *ep, *remote;
	URL *url, *durl;
	DataObject *dobj, *cdobj, *idobj;
	Transport *transport;
	int hasPrinted;
	char buf[256];
	static int hasConnected;
	BOSEventWaitT EvtStatus;
	bool          MsgStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;
	uint8         MailBoxId;
	BOSEventWaitT MailBoxIndex;

	if (!gprsAttached)
		hasConnected = 0;

	if (!hasConnected && gprsAttached)
	{

		//url = url_parse("tcp://10.150.9.6:12345/dataobject", URL_ALL);

		//transport = transport_get(url->scheme);
		/*
			 if (transport == NULL) {
			 emo_printf("No 'tcp' transport available");
			 NU_Sleep(100);
			 return 1;
			 }

			 emo_printf("ANDREY::Calling transport->socket())");

			 ep = transport->socket();
		 */
		//if (ep == NULL) {
		//		emo_printf("Failed Transport::Socket()\n");
		/* removed due to bal_get_socket.. Why are we calling socket functions in UI task?
			 emo_printf("Failed Transport::Socket(), error code: %d",
#ifdef SIMULATOR
0
#else
bal_get_socket_last_error()
#endif
);
NU_Sleep(100);
		 */
		//		return 1;
		//	}

		//	emo_printf("ANDREY::Calling transport->connect())");
		//	if (transport->connect(ep, url) < 0) {
		//		emo_printf("Connect failed");
		//		return 1;
		//	}

		ctx = connectionContext_new();
		connectionContext = ctx;
		//connectionContext_syncRequest(ctx, url);
		hasConnected = 1;
	}

	if (hasConnected)
	{
		connectionContext_loopIteration(ctx);
#ifdef SIMULATOR
		Sleep(100);
#endif
	}
	updateScreen();

	emo_printf("UIWaitForActivity(): Going into event loop");
#endif
	return 1;
}

static void UICleanup(void)
{

}
