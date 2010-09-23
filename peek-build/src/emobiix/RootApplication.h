#ifndef _ROOTAPPLICATION_H_
#define _ROOTAPPLICATION_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

DataObject *RootApplication(void);

extern char* hw_td_get_clock_str();

#ifdef __cplusplus
}
#endif

#endif
