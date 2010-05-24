#ifndef _CONNECTIONMANAGER_H_
#define _CONNECTIONMANAGER_H_

#include "Transport.h"
#include "URL.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ConnectionManager_t;
typedef struct ConnectionManager_t ConnectionManager;

ConnectionManager *connectionmanager_new(void);
Endpoint *connectionmanager_getConnection(ConnectionManager *cm, URL *url);
void connectionmanager_delete(ConnectionManager *cm);

#ifdef __cplusplus
}
#endif

#endif /* _CONNECTIONMANAGER_H_ */
