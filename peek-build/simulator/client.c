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

static Transport *transport;

static int encode_out(const void *buffer, size_t size, void *key)
{
	Endpoint *ep;

	ep = (Endpoint *)ep;
	return transport->write(ep, buffer, size);
}

int main(/*@unused@*/ int argc, /*@unused@*/ char **argv)
{
    Endpoint *ep;
	DataObject *dobj;
	int hasPrinted;
#if 0
	FRIPacket_t fri_packet;
	unsigned char obuf[1024];
	asn_enc_rval_t retval;
#endif
	URL *url;
	ConnectionContext *ctx;

	dataobject_platformInit();

	url = url_parse("tcp://69.114.111.9:12345/dataobject", URL_ALL);
	/*url = url_parse("tcp://localhost:1234/dataobject", URL_ALL);*/

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

    if (transport->connect(ep, url) < 0) {
        fprintf(stderr, "Connect failed\n");
        exit(EXIT_FAILURE);
    }

	ctx = connectionContext_new(ep);
	connectionContext_syncRequest(ctx, url);

	dobj = dataobject_locate(url);
	hasPrinted = 0;

	while (1) {
		connectionContext_loopIteration(ctx);
		Sleep(100);
		if (!hasPrinted && dataobject_getState(dobj) == DOS_OK) {
			dataobject_debugPrint(dobj);
			hasPrinted = 1;
		}
	}

#if 0
	fri_packet.packetType.present = packetType_PR_protocolHandshake;
	fri_packet.packetType.choice.protocolHandshake.protocolVersion = 10000;

	retval = uper_encode_to_buffer(&asn_DEF_FRIPacket, (void *)&fri_packet, obuf, 1024);

	if (retval.encoded > 1) {
		fprintf(stderr, "Encoded out %d bytes\n", (retval.encoded+7)/8);
	} else {
		fprintf(stderr, "Failed to encode packet\n");
	}

	transport->write(ep, obuf, (retval.encoded+7)/8);
#endif

    return EXIT_SUCCESS;
}