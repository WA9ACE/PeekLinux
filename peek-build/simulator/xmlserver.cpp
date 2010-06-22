#include "Transport.h"
#include "ConnectionContext.h"
#include "DataObject.h"

#include <asn_application.h>

#include "FRIPacketP.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <pthread.h>
#endif

#ifdef LINUX
#include <unistd.h>
#define Sleep(x) usleep(x*10)
#endif

static Transport *transport;

static void handle_client(void *endpoint);

int main(/*@unused@*/ int argc, /*@unused@*/ char **argv)
{
    Endpoint *ep, *remote;
	URL *url, *durl;
	DataObject *dobj, *cdobj, *idobj;
#ifdef WIN32
	DWORD thread;
#else
	pthread_t thread;
#endif

	dataobject_platformInit();

	durl = url_parse("system://localhost/dataobject", URL_ALL);
	dobj = dataobject_construct(durl, 1);
#if 0
	dataobject_setValue(dobj, "test1", dataobjectfield_string("Hi, Test"));
	dataobject_setValue(dobj, "test2", dataobjectfield_data((void *)"12345", 5));

	cdobj = dataobject_new();
	dataobject_setValue(cdobj, "childval1", dataobjectfield_string("Childrens"));
	dataobject_setValue(cdobj, "childval2", dataobjectfield_string("Damage"));
	dataobject_pack(dobj, cdobj);

	idobj = dataobject_new();
	dataobject_setValue(idobj, "product", dataobjectfield_string("handset"));
	dataobject_setValue(idobj, "value", dataobjectfield_string("$20"));
	dataobject_pack(cdobj, idobj);

	idobj = dataobject_new();
	dataobject_setValue(idobj, "product", dataobjectfield_string("charger"));
	dataobject_setValue(idobj, "value", dataobjectfield_string("$10"));
	dataobject_pack(cdobj, idobj);

	cdobj = dataobject_new();
	dataobject_setValue(cdobj, "location", dataobjectfield_string("home"));
	dataobject_setValue(cdobj, "reason", dataobjectfield_string("sleep"));
	dataobject_pack(dobj, cdobj);
#else
	dataobject_setRecordType(dobj, 1);

	cdobj = dataobject_new();
	dataobject_setStamp(cdobj, 1, 0);
	dataobject_setValue(cdobj, "childval1", dataobjectfield_string("Childrens"));
	dataobject_setValue(cdobj, "childval2", dataobjectfield_string("Damage"));
	dataobject_appendRecord(dobj, cdobj);

/*	idobj = dataobject_new();
	dataobject_setValue(idobj, "product", dataobjectfield_string("handset"));
	dataobject_setValue(idobj, "value", dataobjectfield_string("$20"));
	dataobject_pack(cdobj, idobj);

	idobj = dataobject_new();
	dataobject_setValue(idobj, "product2", dataobjectfield_string("charger"));
	dataobject_setValue(idobj, "value2", dataobjectfield_string("$10"));
	dataobject_pack(cdobj, idobj);*/

	cdobj = dataobject_new();
	dataobject_setStamp(cdobj, 2, 0);
	dataobject_setValue(cdobj, "location", dataobjectfield_string("home"));
	dataobject_setValue(cdobj, "reason", dataobjectfield_string("sleep"));
	dataobject_appendRecord(dobj, cdobj);
#endif

	dataobject_debugPrint(dobj);

	url = url_parse("tcp://0.0.0.0:1234/", URL_ALL);

    transport = transport_get(url->scheme);
    if (transport == NULL) {
        fprintf(stderr, "No 'tcp' transport available\n");
        exit(EXIT_FAILURE);
    }

    ep = transport->socket();
    if (ep == NULL) {
        fprintf(stderr, "Failed Transport::Socket()\n");
        exit(EXIT_FAILURE);
    }

    if (transport->bind(ep, url) < 0) {
        fprintf(stderr, "Bind failed\n");
        exit(EXIT_FAILURE);
    }

    if (transport->listen(ep, 5) < 0) {
        fprintf(stderr, "Listen Failed\n");
        exit(EXIT_FAILURE);
    }

    do {
        remote = transport->accept(ep);
        if (remote == NULL)
            ;//fprintf(stderr, "Transport::Accept() returned null\n");
        else
#ifdef WIN32
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handle_client,
					(LPVOID)remote,	0, &thread);
#else
            pthread_create(&thread, NULL, (void *(*)(void*))handle_client, remote);
#endif
	} while (1);

    return EXIT_SUCCESS;
}

static void handle_client(void *endpoint)
{
	Endpoint *ep;
	ConnectionContext *ctx;

	ep = (Endpoint *)endpoint;
	ctx = connectionContext_new(ep);
	connectionContext_requestAuth(ctx);

	while (1) {
		connectionContext_loopIteration(ctx);
		Sleep(100);
	}
#if 0
    Endpoint *ep;
    unsigned char inbuf[2048];
    int bytesRead, bpos;
    asn_dec_rval_t retval;
    asn_codec_ctx_t *opt_codec_ctx = NULL;
    void *structure = NULL;
    static asn_TYPE_descriptor_t *pduType = &asn_DEF_FRIPacket;
    asn_codec_ctx_t s_codec_ctx;

    ep = (Endpoint *)endpoint;
    bpos = 0;
    
    s_codec_ctx.max_stack_size = 30000;
    opt_codec_ctx = &s_codec_ctx;

    do {
        bytesRead = transport->read(ep, inbuf+bpos, 100);
        if (bytesRead < 1) {
            fprintf(stderr, "Read error from client\n");
            return;
        }
        bpos += bytesRead;
        retval = uper_decode_complete(opt_codec_ctx, pduType,
                (void **)&structure, inbuf, bpos);
        fprintf(stderr, "Decode %d bytes consumed %d bytes, ret(%d)\n",
                bpos, retval.consumed, retval.code);
		switch (retval.code) {
			case RC_OK:
				break;
			case RC_WMORE:
				break;
			case RC_FAIL:
				fprintf(stderr, "Client send unrecognized packet\n");
				return;
				break;
		}
    } while(1);
#endif
}
