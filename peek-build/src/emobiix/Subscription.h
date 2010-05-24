#ifndef _SUBSCRIPTION_H_
#define _SUBSCRIPTION_H_

#include "DataObject.h"
#include "DataObject.h"

#if 0
#include "Policy.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct Subscription_t;
typedef struct Subscription_t Subscription;

Subscription *subscription_new(DataObject *dobj, DataObjectMap *dmap);
#if 0
void subscription_setPolicy(Policy *policy);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SUBSCRIPTION_H_ */
