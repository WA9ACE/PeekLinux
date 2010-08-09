#ifndef _KEYMAPPINGS_H_
#define _KEYMAPPINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EKEY_ACTIVATE		13
#define EKEY_FOCUSPREV		87
#define EKEY_FOCUSNEXT		86
#define EKEY_ALTTAB			42

int MapKeyToInternal(int key);

#ifdef __cplusplus
}
#endif

#endif
