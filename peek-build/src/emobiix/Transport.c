#include "Transport.h"
#include "StaticList.h"
#include "Debug.h"

#include "TCPTransport.h"

#include <stdio.h>
#include <stdlib.h>

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
	EMO_ASSERT_NULL(transportStr != NULL,"transport get missing transport name")

	return (Transport *)staticlist_find((StaticList *)transports, transportStr);
}

Transport *endpoint_getTransport(Endpoint *ep)
{
	EMO_ASSERT_NULL(ep != NULL,"endpoint get transport missing endpoint")

	if (ep == NULL)
		return NULL;
	return ep->transport;
}
