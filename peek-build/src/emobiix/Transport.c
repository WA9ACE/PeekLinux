#include "Transport.h"
#include "StaticList.h"

#include "TCPTransport.h"

struct Endpoint_t
{
	Transport *transport;
};

struct TransportStaticList_t {
	const char *key;
	Transport *transport;
};
typedef struct TransportStaticList_t TransportStaticList;

static TransportStaticList transports[] = {
	{"tcp", &TCPTransport },
	{NULL, NULL}
};

Transport *transport_get(const char *transportStr)
{
	return (Transport *)staticlist_find((StaticList *)transports, transportStr);
}

Transport *endpoint_getTransport(Endpoint *ep)
{
	if (ep == NULL)
		return NULL;
	return ep->transport;
}
