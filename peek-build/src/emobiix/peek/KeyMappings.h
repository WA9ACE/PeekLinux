#ifndef _KEYMAPPINGS_H_
#define _KEYMAPPINGS_H_

#include "general.h"
#include "typedefs.h"
#include "mfw_kbd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EKEY_ACTIVATE		KCD_NAV_CENTER
#define EKEY_FOCUSPREV		KCD_UP
#define EKEY_FOCUSNEXT		KCD_DOWN
#define EKEY_ALTTAB		KCD_LOCK

int MapKeyToInternal(int key);

#ifdef __cplusplus
}
#endif

#endif
