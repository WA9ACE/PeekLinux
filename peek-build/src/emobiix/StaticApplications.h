#ifndef _STATICAPPLICATIONS_H_
#define _STATICAPPLICATIONS_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif
DataObject *LoadingApplication(void);

DataObject *LockApplication(void);

#ifdef __cplusplus
}
#endif

#endif
