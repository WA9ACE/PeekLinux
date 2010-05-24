#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include "URL.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Endpoint_t;
typedef struct Endpoint_t Endpoint;

struct Transport_t {
	Endpoint *(*socket)(void);
	int (*connect)(Endpoint *ep, URL *destination);
	int (*listen)(Endpoint *ep, int backlog);
	int (*bind)(Endpoint *ep, URL *bindaddr);
	Endpoint *(*accept)(Endpoint *ep);
	int (*write)(Endpoint *ep, const void *buffer, size_t len);
	int (*read)(Endpoint *ep, void *output, size_t len);
	int (*peek)(Endpoint *ep, void *output, size_t len);
	int (*close)(Endpoint *ep);
	void (*free)(Endpoint *ep);
	unsigned int (*sequenceID)(Endpoint *ep);
};
typedef struct Transport_t Transport;

Transport *transport_get(const char *transportStr);

Transport *endpoint_getTransport(Endpoint *ep);

#ifdef __cplusplus
}
#endif

#endif /* _TRANSPORT_H_ */
