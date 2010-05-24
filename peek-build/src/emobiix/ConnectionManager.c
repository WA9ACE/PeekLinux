#include "ConnectionManager.h"

#include "Map.h"

#include "p_malloc.h"

#include <string.h>

struct ConnectionManager_t {
	Map *connections;
};

ConnectionManager *connectionmanager_new(void)
{
	ConnectionManager *output;

	output = (ConnectionManager *)p_malloc(sizeof(ConnectionManager));
	if (output == NULL)
		return NULL;
	output->connections = map_string();
	if (output->connections == NULL) {
		p_free(output);
		return NULL;
	}

	return output;
}

Endpoint *connectionmanager_getConnection(ConnectionManager *cm, URL *url)
{
	Endpoint *ep;
	Transport *transport;

	if (cm == NULL || url == NULL || url->authority == NULL || url->scheme == NULL)
		return NULL;

	ep = (Endpoint *)map_find(cm->connections, url->authority);
	if (ep != NULL)
		return ep;

	transport = transport_get(url->scheme);
	if (transport == NULL)
		return NULL;
	ep = transport->socket();
	if (ep == NULL)
		return NULL;

	if (transport->connect(ep, url) < 0) {
		transport->free(ep);
		return NULL;  
	}

	return ep;
}

void connectionmanager_delete(ConnectionManager *cm)
{
	map_delete(cm->connections);
	p_free(cm);
}
