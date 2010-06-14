#include "Task.h"
#include "UITask.h"
#include "LcdControl.h"
#include "Debug.h"
#include "Gprs.h"
#include "lgui.h"
#include "tweet.h"


#include "Transport.h"
#include "ConnectionContext.h"
#include "Platform.h"

#include "DataObject.h"
#include "Application.h"

extern "C" {
#include "bal_socket_api_ti.h"
#include "system_battery.h"

unsigned char *screenBuf; //[320*240*2];
void updateScreen(void);
static int UIInit(void);
static int UIIteration(void);
static int UIWaitForActivity(void);
static void UICleanup(void);
ConnectionContext *connectionContext;

Task UITask = {UIInit, UIIteration, UIWaitForActivity, UICleanup};

DataObject *systemAppObject;
Application *systemApplication;

void UITaskMsgCB(uint32 MsgId, void* MsgDataP, uint32 MsgSize)
{

        switch (MsgId)
        {
	  default:
		emo_printf("UITaskMsgCB(): Got message\n");
		return;
	}
}

static int UIInit(void)
{	
        static int initd = 0;

//	GprsRegisterRssi();

        /* Wait for BAL Task to complete */
        while(!BalStatusGet()) {
                ExeEventWait(EXE_UI_ID,EXE_SIGNAL_FALSE,EXE_MESSAGE_FALSE,0x16);
        }

        /* Update Bui status so baseband initializes in mmi_main() */
        BuiStatusSet();

	extern void KeyPad_Init();
	KeyPad_Init();

        LcdWakeUp();
//#ifndef EMO_SIM
        BalLightInit();

        BalLcdScreenOn();

// 	extern void main_test();
//	main_test();

        if (!initd) {
		screenBuf = (unsigned char *)BalMalloc(320*240*2);
                lgui_attach(screenBuf);
                tweetInit();
                initd = 1;
        }
        tweetDrawScreen();
	updateScreen();
//#endif

	/*
	systemAppObject = dataobject_locateStr("system://local/bootapplication");
	systemApplication = application_new(systemAppObject);
	application_setActive(systemApplication);
	*/
	dataobject_platformInit();
	system_battery_init();

	return 1;
}

static int UIIteration(void)
{
	return 1;
}

static int UIWaitForActivity(void)
{
	Endpoint *ep, *remote;
	URL *url, *durl;
	DataObject *dobj, *cdobj, *idobj;
	Transport *transport;
	int hasPrinted;
	char buf[256];
	static int hasConnected;
 	static int gprsAttached;
	static ConnectionContext *ctx;
	BOSEventWaitT EvtStatus;
        bool          MsgStatus;
        uint32        MsgId;
        uint32        MsgSize;
        void          *MsgBufferP;
        uint8         MailBoxId;
        BOSEventWaitT MailBoxIndex;

	if (!hasConnected)
	{
		url = url_parse("tcp://10.150.9.6:12345/dataobject", URL_ALL);

		transport = transport_get(url->scheme);
		if (transport == NULL) {
			emo_printf("No 'tcp' transport available");
			NU_Sleep(100);
			return 1;
		}

		emo_printf("ANDREY::Calling transport->socket())");

		ep = transport->socket();
		if (ep == NULL) {
			emo_printf("Failed Transport::Socket(), error code: %d",
#ifdef SIMULATOR
					0
#else
					bal_get_socket_last_error()
#endif
					);
			NU_Sleep(100);
			return 1;
		}

		emo_printf("ANDREY::Calling transport->connect())");
		if (transport->connect(ep, url) < 0) {
			emo_printf("Connect failed");
			return 1;
		}

		ctx = connectionContext_new(ep);
		connectionContext = ctx;
		//connectionContext_syncRequest(ctx, url);

		hasConnected = 1;
	}
	//       while (1)
	//     {
	connectionContext_loopIteration(ctx);
#ifdef SIMULATOR
	Sleep(100);
#endif
        EvtStatus = BOSEventWait(BOS_EM_S_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOSCalMsec(100));
        if(EvtStatus & BOS_MESSAGE_TYPE)
	{
		for(MailBoxIndex=BOS_MAILBOX_1,MailBoxId = BOS_MAILBOX_1_ID; MailBoxId<UI_MAX_MAILBOXES; MailBoxId++)
		{
			if(EvtStatus & MailBoxIndex)
			{
				//get the Msg value
				MsgStatus = BOSMsgRead(BOS_EM_S_ID, (BOSMailboxIdT)MailBoxId, &MsgId, &MsgBufferP, &MsgSize);
				if(MsgStatus)
				{

					//call Msg handler
					UITaskMsgCB(MsgId,MsgBufferP,MsgSize);
					if (MsgBufferP != NULL)
					{
						//free the Msg buffer.
						BOSMsgBufferFree(MsgBufferP);
						MsgBufferP = NULL;
					}
				}
			}
			MailBoxIndex = (BOSEventWaitT)(MailBoxIndex << 1);
		}
	}
	updateScreen();

	/*
  	EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_TRUE, BOS_MESSAGE_FALSE, BOSCalMsec(100));
  	if ((EvtStatus & BOS_SIGNAL_TYPE) && (EvtStatus & BOS_SIGNAL_1))
  	{
               // Gprs Attached
               gprsAttached=1;
 	}
	*/

	return 1;
}

static void UICleanup(void)
{

}

}
