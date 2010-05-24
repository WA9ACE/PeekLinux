#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

int script_event(DataObject *context, const char *eventname);

#ifdef __cplusplus
}
#endif

#endif

