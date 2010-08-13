#ifndef _KEYMAPPINGS_H_
#define _KEYMAPPINGS_H_

#include "general.h"
#include "typedefs.h"
#include "mfw_kbd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EKEY_ACTIVATE			0xFFFFFF03
#define EKEY_FOCUSPREV		0xFFFFFF01
#define EKEY_FOCUSNEXT		0xFFFFFF02
#define EKEY_ALTTAB				0xFFFFFF04

unsigned int MapKeyToInternal(unsigned int key);

#ifdef __cplusplus
}
#endif

#endif
