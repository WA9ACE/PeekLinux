#include "Protocol.h"

#include "p_malloc.h"

struct Protocol_t {
	ProtocolHandler *handler;
};

Protocol *protocol_new(ProtocolHandler *h)
{
	Protocol *output;

	output = (Protocol *)p_malloc(sizeof(Protocol));
	if (output == NULL)
		return NULL;

	output->handler = h;

	return output;
}

Application *protocol_loadApplication(Protocol *p, const char *URL)
{
	return NULL;
}

DataObject *protocol_loadDataObject(Protocol *p, const char *URL)
{
	return NULL;
}

