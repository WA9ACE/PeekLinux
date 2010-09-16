#include "general.h"
#include "typedefs.h"
#include "nucleus.h"
#include "Task.h"
#include "UITask.h"
#include "mfw_mfw.h"
#include "mfw_mme.h"
#include "Debug.h"
#include "lgui.h"

#include "Transport.h"
#include "ConnectionContext.h"
#include "Platform.h"

#include "DataObject.h"
#include "Application.h"
#include "ApplicationManager.h"
#include "RenderManager.h"

#include "gdi.h"
#include "dspl.h"
#include "p_sim.h"

#include "system_battery.h"
#include "exeapi.h"

unsigned char *screenBuf; //[320*240*2];
void updateScreen(void);
int UIInit(void);
static int UIIteration(void);
int UIWaitForActivity(void);
static void UICleanup(void);
ConnectionContext *connectionContext;
//extern void appProtocolStatus(int status);
int netsurf_main(int argc, char** argv);

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
	/* mfw -> custom menu
	if(emoMenu_get_window())
		SEND_EVENT(emoMenu_get_window(), SCREEN_UPDATE, 0, 0);
	*/
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
			// XXX appProtocolStatus(0);
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

extern void emobiixKbdInit();

int UIInit(void)
{	
	char *argv = "emobiix";
	static int initd = 0;

	if (!initd) {
		screenBuf = (unsigned char *)p_malloc(320*240*2);
		if(!screenBuf)
			emo_printf("Failed to allocate screenBuf\n");

		dataobject_platformInit();
		renderman_init();

		lgui_attach(screenBuf);
		manager_init();
		initd = 1;

		manager_drawScreen();
		updateScreen();
#ifndef EMO_SIM
		KeyPad_Init();
#endif
	}

	uiStatusSet();
	return 1;
}

static int UIIteration(void)
{
	return 1;
}

extern int netsurf_start_flag;

int UIWaitForActivity(void)
{
	char *argv = "emobiix";

	TCCE_Task_Sleep(10);
	
	if (netsurf_start_flag)
	   netsurf_main(1, &argv);

	return 1;
}

static void UICleanup(void)
{

}
