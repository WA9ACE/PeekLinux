#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "DataObject.h"
#include "Application.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ProtocolHandler_t {
	int (*authHandler)(char **username, char **password);
};
typedef struct ProtocolHandler_t ProtocolHandler;

struct Protocol_T;
typedef struct Protocol_t Protocol;

Protocol *protocol_new(ProtocolHandler *h);
Application *protocol_loadApplication(Protocol *p, const char *URL);
DataObject *protocol_loadDataObject(Protocol *p, const char *URL);

#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_H_ */
