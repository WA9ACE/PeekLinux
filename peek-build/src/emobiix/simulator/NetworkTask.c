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

ConnectionContext *connectionContext;

//extern CRITICAL_SECTION emoCrit;

void NetworkTask(void)
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
			0
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
		//EnterCriticalSection(&emoCrit);
		connectionContext_loopIteration(ctx);
		//LeaveCriticalSection(&emoCrit);
		Sleep(100);
/*                if (!hasPrinted && dataobject_getState(dobj) == DOS_OK) {
                        dataobject_debugPrint(dobj);
                        hasPrinted = 1;
                }
	        updateScreen();*/
	}
}
