#ifndef _CONNECTION_CONTEXT_H_
#define _CONNECTION_CONTEXT_H_

#include "Transport.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ConnectionContext_t;
typedef struct ConnectionContext_t ConnectionContext;

ConnectionContext *connectionContext_new(Endpoint *ep);
void connectionContext_delete(ConnectionContext *ctx);
int connectionContext_loopIteration(ConnectionContext *ctx);

/* for servers - or 2 way auth*/
void connectionContext_requestAuth(ConnectionContext *ctx);

int connectionContext_syncRequest(ConnectionContext *ctx, URL *url);

/* used internally, here just incase they are useful */
int connectionContext_consumePacket(ConnectionContext *ctx);


#ifdef __cplusplus
}
#endif

#endif /* _CONNECTION_CONTEXT_H_ */
