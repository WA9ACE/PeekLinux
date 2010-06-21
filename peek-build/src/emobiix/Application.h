#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Application_t;
typedef struct Application_t Application;

Application *application_load(DataObject *dobj);
void application_setActive(Application *app);
DataObject *application_getCurrentScreen(Application *app);

#ifdef __cplusplus
}
#endif

#endif /* _APPLICATION_H_ */
